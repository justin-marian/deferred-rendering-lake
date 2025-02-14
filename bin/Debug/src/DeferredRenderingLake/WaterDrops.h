#pragma once

#ifndef WATER_DROPS_H
#define WATER_DROPS_H

#include "components/simple_scene.h"
#include "core/gpu/particle_effect.h"
#include "Structures.h"


class WaterDrops : public gfxc::SimpleScene
{
public:
    explicit WaterDrops();
    ~WaterDrops();

	// Initialize the WaterDrops effect
    void Init(
        int xSize, int ySize, int zSize, 
        unsigned int nrParticles);

	// Render the WaterDrops effect
    void Render(
        Shader* shader,
        gfxc::Camera* camera,
        float deltaTime);

private:
	// Calculate the Bezier curve at a given parameter t
    glm::vec3 Bezier(float t) const;
	// Calculate the derivative of the Bezier curve at a given parameter t
    glm::vec3 BezierDerivative(float t) const;

private:
    float offset;
    glm::vec3 control_p0, control_p1, control_p2, control_p3;
    ParticleEffect<Particle>* particle_effect;
};

#endif // WATER_DROPS_H
