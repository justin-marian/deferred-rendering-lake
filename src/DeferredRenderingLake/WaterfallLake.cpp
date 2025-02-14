#include "WaterfallLake.h"
#include "Utils.h"
#include "Constants.h"

#include "core/gpu/frame_buffer.h"
#include "core/gpu/mesh.h"
#include "core/gpu/shader.h"
#include "core/managers/texture_manager.h"
#include "utils/glm_utils.h"
#include "utils/text_utils.h"

#include <iostream>


using namespace std;
using WL = Constants::WaterfallLake_WaterDrops;
using DR = Constants::DeferredRender;
using CM = Constants::CubeMap;


WaterfallLake::WaterfallLake(WindowObject* window) :
    window(window), meshes(nullptr),
    frameBuffer(nullptr), lightBuffer(nullptr)
{
    control_p0 = WL::CONTROL_P0;
    control_p1 = WL::CONTROL_P1;
    control_p2 = WL::CONTROL_P2;
    control_p3 = WL::CONTROL_P3;

    light_type = 6;
    lights.clear();
}

WaterfallLake::~WaterfallLake()
{
    delete frameBuffer;
    delete lightBuffer;

    lights.clear();
    meshes = nullptr;
}


void WaterfallLake::CreateFramebuffer(int width, int height)
{
    if (!frameBuffer)
    {
        frameBuffer = new FrameBuffer();
        frameBuffer->Generate(width, height, 3, 32); // G-buffer: Position, Normal, Color
    }

    if (!lightBuffer)
    {
        lightBuffer = new FrameBuffer();
        lightBuffer->Generate(width, height, 1, false, 32); // Light accumulation 
    }
}


void WaterfallLake::ResizeBuffers(int width, int height)
{
    frameBuffer->Resize(width, height, 32);
    lightBuffer->Resize(width, height, 32);
}


void WaterfallLake::Init(
    WindowObject* windowObj,
    std::unordered_map<std::string, Shader*>& shaders,
    std::unordered_map<std::string, Mesh*>& meshesMap,
    int width, int height, int light_type)
{
    window = windowObj;
    meshes = &meshesMap;
    CreateFramebuffer(width, height);

    int maxRetries = 5;
    float minOrbitDistance = 5.0f;
    float maxOrbitDistance = 15.0f;
    float minHeightOffset = 2.0f;
    float maxHeightOffset = 7.0f;

    std::vector<glm::vec3> lightPositions;

    for (int i = 0; i < DR::MAX_LIGHTS; ++i)
    {
        Light lightInfo = { glm::vec3(0), glm::vec3(0), 0.0f, 0.0f, 0.0f, glm::vec3(0) };
        bool valid = false;
        glm::vec3 newPos;

        for (int attempt = 0; attempt < maxRetries; ++attempt)
        {
            lightInfo.orbitRadius = Random::RandF(minOrbitDistance, maxOrbitDistance);
            lightInfo.angle = Random::RandF(0, 1) * glm::radians(360.0f);

            newPos = glm::vec3(
                lightInfo.orbitRadius * cos(lightInfo.angle),
                0.0f,
                lightInfo.orbitRadius * sin(lightInfo.angle)
            );

            valid = true;
            for (const auto& pos : lightPositions)
            {
                if (glm::distance(glm::vec2(newPos.x, newPos.z), glm::vec2(pos.x, pos.z)) < minOrbitDistance)
                {
                    valid = false;
                    break;
                }
            }

            if (valid)
            {
                break;
            }
        }

        lightInfo.offset = glm::vec3(0.0f, Random::RandF(minHeightOffset, maxHeightOffset), 0.0f);
        lightInfo.position = newPos + lightInfo.offset;

        lightInfo.color = glm::vec3(
            Random::RandF(0.3f, 1.0f),
            Random::RandF(0.3f, 1.0f),
            Random::RandF(0.3f, 1.0f)
        );

        lightInfo.radius = DR::LIGHT_RADIUS + Random::RandF(0, 1);
        lightPositions.push_back(lightInfo.position);
        lights.push_back(lightInfo);
    }
}


void WaterfallLake::RenderCompose(
    float deltaTime,
    gfxc::Camera* camera,

    CubeMap* cubeMap,
    WaterDrops* waterDrops,
    Firefly* firefly,
    FallingStars* fallingStars,

    std::unordered_map<std::string, Shader*>& shaders,
    std::unordered_map<std::string, Mesh*>& meshes,

    const glm::mat4& viewMatrix,
    const glm::mat4& projectionMatrix,
    const glm::vec3& cameraPos)
{
    ClearScreen();

    for (auto& l : lights)
    {
        constexpr float rotationSpeed = glm::radians(6.0f);
        l.angle += rotationSpeed * deltaTime;
        l.position.x = l.orbitRadius * cos(l.angle) + l.offset.x;
        l.position.z = l.orbitRadius * sin(l.angle) + l.offset.z;
        glm::mat4 model = glm::translate(glm::mat4(1.0f), l.position);
        model = glm::scale(model, glm::vec3(0.5f));
    }

    std::vector<float> archerAngles (5, 0.0f);

    // ------------------------------------------------------------------------
    // Deferred rendering pass -- GEOMETRY SPACE
    if (cubeMap->GetFramebufferID())
    {
        glBindFramebuffer(GL_FRAMEBUFFER, cubeMap->GetFramebufferID());
        glViewport(0, 0, cubeMap->GetWidth(), cubeMap->GetHeight());
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// ------------------------------------------------------------------------
		// CubeMap pass
        {
            Shader* shader = shaders["CubeMapFramebufferShader"];
            shader->Use();
            glm::mat4 modelMatrix = glm::scale(glm::mat4(1), glm::vec3(30));
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap->GetCubeTexture());
            glUniform1i(glGetUniformLocation(shader->program, "texture_cubemap"), 1);
            glUniform1i(glGetUniformLocation(shader->program, "cube_draw"), 1);
            glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(CM::VIEW_MATRICES[0]));
            glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(CM::PROJECTION_MATRIX));
            glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
            meshes["cube"]->Render();
        }
        // ------------------------------------------------------------------------
        // Archers pass for CubeMap
        {
            Shader* shader = shaders["CubeMapFramebufferShader"];
            shader->Use();

            static float angle = 0.0f;
            constexpr float rotationSpeed = glm::radians(30.0f);
            angle += rotationSpeed * deltaTime;

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, TextureManager::GetTexture("Akai_E_Espiritu.fbm\\akai_diffuse.png")->GetTextureID());
            glUniform1i(glGetUniformLocation(shader->program, "texture_1"), 0);

            for (int i = 0; i < 5; ++i) 
              {
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix *= glm::rotate(glm::mat4(1), angle + i * glm::radians(360.0f) / 5, glm::vec3(0, 1, 0));
                modelMatrix *= glm::translate(glm::mat4(1), glm::vec3(10, 4, 0));
                modelMatrix *= glm::rotate(glm::mat4(1), glm::radians(-90.0f), glm::vec3(0, 1, 0));
                modelMatrix *= glm::scale(glm::mat4(1), glm::vec3(0.01f));

                glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
                glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(CM::PROJECTION_MATRIX));
                glUniformMatrix4fv(glGetUniformLocation(shader->program, "viewMatrices"), 6, GL_FALSE, glm::value_ptr(CM::VIEW_MATRICES[0]));
                glUniform1i(glGetUniformLocation(shader->program, "cube_draw"), 0);

                meshes["archer"]->Render();
            }
        }
		// ------------------------------------------------------------------------
        {
            Shader* shader = shaders["CubeMapFramebufferShader"];
            glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -3.0F, 0.0f));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(1.f));
            
            shader->Use();
            glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
            glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(CM::PROJECTION_MATRIX));
            glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
            glUniformMatrix4fv(glGetUniformLocation(shader->GetProgramID(), "viewMatrices"), 6, GL_FALSE, glm::value_ptr(CM::VIEW_MATRICES[0]));

            glUniform1i(glGetUniformLocation(shader->program, "cube_draw"), 0);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, TextureManager::GetTexture("ground.jpg")->GetTextureID());
            glUniform1i(glGetUniformLocation(shader->program, "texture_1"), 0);

            meshes["dynamicPlane"]->Render();
        }

        glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap->GetColorTextureID());
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, window->GetResolution().x, window->GetResolution().y);
    }
    // ------------------------------------------------------------------------
    // CubeMap pass
    {
        frameBuffer->Bind();
        // ------------------------------------------------------------------------
        // Reflection pass
        {
            Shader* shader = shaders["CubeMapReflectionShader"];
            shader->Use();

            // Set model matrix for the reflective plane
            glm::mat4 modelMatrix = glm::translate(glm::mat4(1), glm::vec3(0, -1.5f, 0));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(0.3f));

            glUniform3f(shader->GetUniformLocation("camera_position"), cameraPos.x, cameraPos.y, cameraPos.z);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap->GetColorTextureID());
            glUniform1i(glGetUniformLocation(shader->GetProgramID(), "texture_cubemap"), 0);

            glActiveTexture(GL_TEXTURE1);
            lightBuffer->BindTexture(0, GL_TEXTURE1);
            glUniform1i(glGetUniformLocation(shader->GetProgramID(), "texture_light"), 1);

            glUniformMatrix4fv(shader->GetUniformLocation("view_matrix"), 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));

            glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
            glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetProjectionMatrix()));
            glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

            meshes["plane"]->Render();
        }
        // ------------------------------------------------------------------------
        // Archers pass
        {
            Shader* shader = shaders["DeferredRender2TextureShader"];
            shader->Use();

            static float angle = 0.0f;
            constexpr float rotationSpeed = glm::radians(30.0f);
            angle += rotationSpeed * deltaTime;

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, TextureManager::GetTexture("Akai_E_Espiritu.fbm\\akai_diffuse.png")->GetTextureID());
            glUniform1i(glGetUniformLocation(shader->program, "texture_1"), 0);
            for (size_t i = 0; i < 5; ++i)
            {
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix *= glm::rotate(glm::mat4(1), angle + i * glm::radians(360.0f) / 5, glm::vec3(0, 1, 0));
                modelMatrix *= glm::translate(glm::mat4(1), glm::vec3(10, 4, 0));
                modelMatrix *= glm::rotate(glm::mat4(1), glm::radians(-90.0f), glm::vec3(0, 1, 0));
                modelMatrix *= glm::scale(glm::mat4(1), glm::vec3(0.01f));

                glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
                glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
                glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetProjectionMatrix()));

                meshes["archer"]->Render();
            }
        }
        // ------------------------------------------------------------------------
        // Normal pass
        {
			Shader* shader = shaders["CubeMapNormalShader"];
            shader->Use();
            glm::mat4 modelMatrix = glm::scale(glm::mat4(1), glm::vec3(30));
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap->GetCubeTexture());
            glUniform1i(shader->GetUniformLocation("texture_cubemap"), 0);
            glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
            glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetProjectionMatrix()));
            glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
            meshes["cube"]->Render();
        }
        // ------------------------------------------------------------------------
        // Deferred texture pass
        {
            Shader* shader = shaders["DeferredRender2TextureShader"];
            glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -3.0f, 0.0f));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(1.f));

            shader->Use();
            glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
            glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetProjectionMatrix()));
            glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
            meshes["dynamicPlane"]->Render();
        }
        // ------------------------------------------------------------------------
        // WaterDrops pass
        {
            Shader* shader = shaders["WaterDrops"];
            waterDrops->Render(shader, camera, deltaTime);
        }
        // Firefly pass
        {
            Shader* shader = shaders["Firefly"];
            firefly->Render(shader, camera, deltaTime /*, lights, WL::NR_PARTICLES / 3 */ );
        }
        // FallingStars pass
        {
            Shader* shader = shaders["FallingStars"];
            fallingStars->Render(shader, camera, deltaTime);
        }
    }
    // ------------------------------------------------------------------------
    // Lighting pass
    {
        glm::vec3 ambientLight(0.2f);
        lightBuffer->SetClearColor(glm::vec4(ambientLight.x, ambientLight.y, ambientLight.z, 1.0f));
        lightBuffer->Bind();
        glClearColor(0.1, 0.1, 0.1, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Enable additive blending for light accumulation
        glEnable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);
        glBlendFunc(GL_ONE, GL_ONE);

        // Disable depth test for light accumulation pass
        glDepthMask(GL_FALSE);
        glDisable(GL_DEPTH_TEST);

        //
        auto shader = shaders["DeferredRenderLightPassShader"];
        shader->Use();
        //
        int texturePositionsLoc = shader->GetUniformLocation("texture_position");
        glUniform1i(texturePositionsLoc, 0);
        frameBuffer->BindTexture(0, GL_TEXTURE0);
        int textureNormalsLoc = shader->GetUniformLocation("texture_normal");
        glUniform1i(textureNormalsLoc, 1);
        frameBuffer->BindTexture(1, GL_TEXTURE0 + 1);
        //
        int loc_eyePosition = shader->GetUniformLocation("eye_position");
        glUniform3fv(loc_eyePosition, 1, glm::value_ptr(cameraPos));
        //
        auto resolution = window->GetResolution();
        int loc_resolution = shader->GetUniformLocation("resolution");
        glUniform2i(shader->GetUniformLocation("resolution"), resolution.x, resolution.y);
        //Front face culling
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        for (auto& lightInfo : lights)
        {
            glUniform3fv(shader->GetUniformLocation("light_position"), 1, glm::value_ptr(lightInfo.position));
            glUniform3fv(shader->GetUniformLocation("light_color"), 1, glm::value_ptr(lightInfo.color));
            glUniform1f(shader->GetUniformLocation("light_radius"), lightInfo.radius);
            glm::mat4 modelMatrix = glm::mat4(1.0f);
            modelMatrix = glm::translate(modelMatrix, lightInfo.position);
            modelMatrix = glm::scale(modelMatrix, glm::vec3(2.0f * lightInfo.radius));
            shader->Use();
            glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
            glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetProjectionMatrix()));
            glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
            meshes["sphere"]->Render();
        }
        //Back face culling
        glDisable(GL_CULL_FACE);
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
    }
    // ------------------------------------------------------------------------
    // Composition pass
    {
        FrameBuffer::BindDefault();
        glDisable(GL_DEPTH_TEST); /// maybe OUT
        glm::mat4 modelMatrix = glm::mat4(0);
        auto shader = shaders["DeferredRenderCompositionShader"];
        shader->Use();
        glUniform1i(shader->GetUniformLocation("output_type"), light_type);
        glUniform1i(shader->GetUniformLocation("texture_position"), 1);
        frameBuffer->BindTexture(0, GL_TEXTURE0 + 1);
        glUniform1i(shader->GetUniformLocation("texture_normal"), 2);
        frameBuffer->BindTexture(1, GL_TEXTURE0 + 2);
        glUniform1i(shader->GetUniformLocation("texture_color"), 3);
        frameBuffer->BindTexture(2, GL_TEXTURE0 + 3);
        glUniform1i(shader->GetUniformLocation("texture_depth"), 4);
        frameBuffer->BindDepthTexture(GL_TEXTURE0 + 4);
        glUniform1i(shader->GetUniformLocation("texture_light"), 5);
        lightBuffer->BindTexture(0, GL_TEXTURE0 + 5);
        glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
        glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetProjectionMatrix()));
        glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
        meshes["quad"]->Render();
    }
}
