#include "FallingStars.h"
#include "Constants.h"
#include "core/managers/texture_manager.h"

#include <cstdlib>
#include <ctime>

using WL = Constants::WaterfallLake_WaterDrops;


FallingStars::FallingStars() :
    particle_effect(nullptr),
    offset(WL::SIZE_PARTICLE) {}

FallingStars::~FallingStars()
{
    if (particle_effect)
    {
        delete particle_effect;
        particle_effect = nullptr;
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function: Init
// Description: Initializes the FallingStars effect by creating the particle effect and setting the initial position of the generator.
// Parameters:
//   - xSize: Width of the particle system.
//   - ySize: Height of the particle system.
//   - zSize: Depth of the particle system.
//   - nrParticles: Number of particles to generate.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FallingStars::Init(
    int xSize, int ySize, int zSize,
    unsigned int nrParticles)
{
    if (particle_effect)
    {
        delete particle_effect;
        particle_effect = nullptr;
    }

    particle_effect = new ParticleEffect<Particle>();
    particle_effect->Generate(nrParticles, true);

    auto particleSSBO = particle_effect->GetParticleBuffer();
    Particle* data = const_cast<Particle*>(particleSSBO->GetBuffer());

    glm::vec3 lowerLeftCorner = glm::vec3(-xSize / 2.0f, 0.0f, -zSize / 2.0f);
    glm::vec3 upperRightCorner = glm::vec3(xSize / 2.0f, ySize * 1.5f, zSize / 2.0f);

    for (size_t i = 0; i < nrParticles; ++i)
    {
        float t = static_cast<float>(i) / static_cast<float>(nrParticles - 1);

        glm::vec3 position = glm::vec3(
            glm::mix(lowerLeftCorner.x, upperRightCorner.x, static_cast<float>(rand()) / RAND_MAX),
            glm::mix(lowerLeftCorner.y, upperRightCorner.y, static_cast<float>(rand()) / RAND_MAX),
            glm::mix(lowerLeftCorner.z, upperRightCorner.z, static_cast<float>(rand()) / RAND_MAX)
        );
        glm::vec4 pos(position, 1.0f);

        glm::vec3 speed = glm::vec3(
            (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.2,
            (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.2,
            (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.2
        );
        glm::vec4 speed_vec(speed, 0.0f);

        float lifetime = glm::mix(0.1f, 5.f, static_cast<float>(rand()) / RAND_MAX);
        float delay = glm::mix(0.0f, 0.05f, static_cast<float>(rand()) / RAND_MAX);

        data[i].SetInitial(pos, speed_vec, delay, lifetime);
    }

    particleSSBO->SetBufferData(data);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function: Render
// Description: Renders the FallingStars effect using the provided shader.
// Parameters:
//   - shader: Shader program used for rendering.
//   - deltaTime: Time elapsed since the last frame.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FallingStars::Render(
    Shader* shader,
    gfxc::Camera* camera,
    float deltaTime)
{
    if (!particle_effect || !shader) return;

    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glLineWidth(5);
    glDepthMask(GL_FALSE);

    glBlendFunc(GL_ONE, GL_ONE);
    glBlendEquation(GL_FUNC_ADD);

    shader->Use();

    if (shader->GetProgramID())
    {
        glUniform1f(glGetUniformLocation(shader->program, "offset"), offset);
        glUniform1f(glGetUniformLocation(shader->program, "deltaTime"), deltaTime);
        glUniform2f(glGetUniformLocation(shader->program, "resolution"), camera->GetFieldOfViewX(), camera->GetFieldOfViewY());

        auto texture = TextureManager::GetTexture("star.png");
        if (texture)
        {
            texture->BindToTextureUnit(GL_TEXTURE0);

            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            glGenerateMipmap(GL_TEXTURE_2D);
        }

        particle_effect->Render(camera, shader);
    }

    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}
