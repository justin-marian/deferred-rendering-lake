#pragma once

#ifndef FIREFLY_H
#define FIREFLY_H

#include "components/simple_scene.h"
#include "core/gpu/particle_effect.h"
#include "Structures.h"

#include <vector>


class Firefly : public gfxc::SimpleScene
{
public:
    explicit Firefly();
    ~Firefly();

    // Initialize the Firefly effect
    void Init(
        int xSize, int ySize, int zSize,
        unsigned int nrParticles);

    // Render the Firefly effect
    void Render(
        Shader* shader,
        gfxc::Camera* camera,
        float deltaTime);

private:
    float offset;
    ParticleEffect<Particle>* particle_effect;
};

#endif // FIREFLY_H
