#include "WaterDrops.h"
#include "Constants.h"
#include "CreatePlane.h"
#include "core/managers/texture_manager.h"

#include <cstdlib>
#include <ctime>

using WL = Constants::WaterfallLake_WaterDrops;


WaterDrops::WaterDrops() : 
    particle_effect(nullptr), 
    offset(WL::SIZE_PARTICLE)
{
    control_p0 = WL::CONTROL_P0;
    control_p1 = WL::CONTROL_P1;
    control_p2 = WL::CONTROL_P2;
    control_p3 = WL::CONTROL_P3;
}

WaterDrops::~WaterDrops()
{
    if (particle_effect)
    {
        delete particle_effect;
        particle_effect = nullptr;
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function: Bezier
// Description: Calculates the Bezier curve at a given parameter t.
// Parameters:
//   - t: Parameter of the Bezier curve.
// Return: The point on the Bezier curve at parameter t.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
glm::vec3 WaterDrops::Bezier(float t) const
{
    float oneMinusT = 1.0f - t;
    return oneMinusT * oneMinusT * oneMinusT * control_p0 +
        3.0f * t * oneMinusT * oneMinusT * control_p1 +
        3.0f * t * t * oneMinusT * control_p2 +
        t * t * t * control_p3;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function: BezierDerivative
// Description: Calculates the derivative of the Bezier curve at a given parameter t.
// Parameters:
//   - t: Parameter of the Bezier curve.
// Return: The derivative of the Bezier curve at parameter t.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
glm::vec3 WaterDrops::BezierDerivative(float t) const
{
    float oneMinusT = 1.0f - t;
    return
        -3.0f * oneMinusT * oneMinusT * control_p0 +
        (3.0f * oneMinusT * oneMinusT - 6.0f * t * oneMinusT) * control_p1 +
        (6.0f * t * oneMinusT - 3.0f * t * t) * control_p2 +
        3.0f * t * t * control_p3;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function: Init
// Description: Initializes the WaterDrops effect by creating the particle effect and setting the initial position of the generator.
// Parameters:
//   - xSize: Width of the particle system.
//   - ySize: Height of the particle system.
//   - zSize: Depth of the particle system.
//   - nrParticles: Number of particles to generate.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WaterDrops::Init(
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

    const float g = 9.81f;
    float displacement_at_p0 = Create::Displacement(
        WL::CONTROL_P0,
        WL::CENTER, WL::RADIUS,  WL::H_MAX,
        WL::CONTROL_P0, WL::CONTROL_P1, WL::CONTROL_P2, WL::CONTROL_P3);

    for (size_t i = 0; i < nrParticles; ++i)
    {
        float t = static_cast<float>(i) / static_cast<float>(nrParticles - 1);
        glm::vec3 position = Bezier(t);
        position.y = 0.55f * displacement_at_p0;
        glm::vec4 pos(position, 1.0f);

        glm::vec3 bezier_tangent = glm::normalize(BezierDerivative(t));
        float speedFactor = glm::mix(0.0005f, 0.0025f, t);
        glm::vec3 speed = bezier_tangent * speedFactor;
        speed.y -= g * 0.1f;
        glm::vec4 speed_vec(speed * 0.2f, 0.0f);

        float lifetime = 5.f + (static_cast<float>(rand() % 1000) / 1000.0f);
        float delay = 0.5f + (static_cast<float>(rand() % 1000) / 10000.0f);

        data[i].SetInitial(pos, speed_vec, delay, lifetime);
    }

    particleSSBO->SetBufferData(data);
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function: Render
// Description: Renders the WaterDrops effect using the provided shader.
// Parameters:
//   - shader: Shader program used for rendering.
//   - deltaTime: Time elapsed since the last frame.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WaterDrops::Render(
    Shader* shader,
    gfxc::Camera* camera,
    float deltaTime)
{
    if (!particle_effect || !shader) return;

    glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);

    glLineWidth(5);
    glDepthMask(GL_FALSE);

    glBlendFunc(GL_ONE, GL_ONE);
    glBlendEquation(GL_FUNC_ADD);

    shader->Use();

    if (shader->GetProgramID())
    {
        glUniform1f(glGetUniformLocation(shader->program, "deltaTime"), deltaTime);
        glUniform1f(glGetUniformLocation(shader->program, "offset"), offset);

        float displacement_at_p0 = Create::Displacement(
            WL::CONTROL_P0, WL::CENTER, WL::RADIUS, WL::H_MAX,
            WL::CONTROL_P0, WL::CONTROL_P1, WL::CONTROL_P2, WL::CONTROL_P3);
        glUniform1f(glGetUniformLocation(shader->program, "displacement_at_p0"), 0.55f * displacement_at_p0);

        glUniform3fv(shader->GetUniformLocation("control_p0"), 1, glm::value_ptr(control_p0));
        glUniform3fv(shader->GetUniformLocation("control_p1"), 1, glm::value_ptr(control_p1));
        glUniform3fv(shader->GetUniformLocation("control_p2"), 1, glm::value_ptr(control_p2));
        glUniform3fv(shader->GetUniformLocation("control_p3"), 1, glm::value_ptr(control_p3));

        glm::mat4 identityView = glm::mat4(1.0f);
        TextureManager::GetTexture("rain.png")->BindToTextureUnit(GL_TEXTURE0);
        particle_effect->Render(camera, shader);
    }

    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}
