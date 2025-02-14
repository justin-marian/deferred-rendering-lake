#pragma once

#ifndef FALLING_STARS_H
#define FALLING_STARS_H

#include "components/simple_scene.h"
#include "core/gpu/particle_effect.h"
#include "Structures.h"


class FallingStars : public gfxc::SimpleScene
{
public:
    explicit FallingStars();
    ~FallingStars();

    // Initialize the FallingStars effect
    void Init(
        int xSize, int ySize, int zSize,
        unsigned int nrParticles);

    // Render the FallingStars effect
    void Render(
        Shader* shader,
        gfxc::Camera* camera,
        float deltaTime);

private:
    float offset;
    ParticleEffect<Particle>* particle_effect;
};

#endif // FALLING_STARS_H
