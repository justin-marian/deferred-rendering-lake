#pragma once

#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <glm/glm.hpp>

#include <string>


struct Particle
{
    glm::vec4 position;      // Current position of the particle
    glm::vec4 speed;		 // Current speed of the particle
    glm::vec4 initialPos;    // Initial position of the particle
    glm::vec4 initialSpeed;  // Initial speed of the particle
    float delay;             // Delay until the particle starts moving
    float initialDelay;      // Initial delay before the particle starts moving
    float lifetime; 	     // Time left to live (in seconds) [it respawns at 0]
    float initialLifetime;   // Total time to live (in seconds) [it respawns at 0]

	// float rotationAngle;     // Rotation angle of the particle
    float initialRotationAngle; // Initial rotation angle of the particle

    Particle() :
        position(0.0f), speed(0.0f),
        initialPos(0.0f), initialSpeed(0.0f),
        delay(0.0f), initialDelay(0.0f),
        lifetime(0.0f), initialLifetime(0.0f), initialRotationAngle(0.0f) {}

    Particle(
        const glm::vec4& pos,
        const glm::vec4& speed)
    {
        SetInitial(pos, speed);
    }

    void SetInitial(
        const glm::vec4& pos, const glm::vec4& speed,
		float delay = 0, float lifetime = 0, float initialRotationAngle = 0)
    {
        position = initialPos = pos;
        this->speed = initialSpeed = speed;
        this->delay = initialDelay = delay;
        this->lifetime = initialLifetime = lifetime;
		this->initialRotationAngle = initialRotationAngle;
    }
};

struct Light 
{
    glm::vec3 position;
    glm::vec3 offset;
    float radius;
    float orbitRadius;
    float angle;
    glm::vec3 color;
};

struct ShaderConfig
{
    std::string shaderName;
    std::string vertexShader;
    std::string fragmentShader;
    std::string geometryShader;
    bool hasGeometry;
};

struct MeshConfig
{
    std::string meshName;
    std::string folderPath;
    std::string fileName;
    bool useMaterials;
};

#endif // !STRUCTURES_H
