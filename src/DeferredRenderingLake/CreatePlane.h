#pragma once

#ifndef __CREATE_PLANE_H__
#define __CREATE_PLANE_H__

#include "components/simple_scene.h"
#include "components/transform.h"
#include "core/gpu/mesh.h"
#include "core/gpu/frame_buffer.h"

#include <vector>
#include <string>


class Create : public gfxc::SimpleScene
{
public:
    // Create a standard mesh from vertices and indices
    static Mesh* CreateMesh(const char* name,
        const std::vector<VertexFormat>& vertices,
        const std::vector<unsigned int>& indices);

    // Create a grid mesh with specified dimensions and size
    static Mesh* CreateGridMesh(
        const char* name,
        int gridX, int gridZ,
        float gridSizeX, float gridSizeZ,
        const char* textureName);

    // Displacement based on region waterfall or lake
    static float Displacement(
        glm::vec3 position,
        const glm::vec3& center, float radius, float h_max,
        const glm::vec3& p0,
        const glm::vec3& p1,
        const glm::vec3& p2,
        const glm::vec3& p3);

    // Compute a pseudo-random hash value for a 2D point
    static float Hash(glm::vec2 p);

    // Generate a smooth noise value for a given 2D point
    static float Noise(glm::vec2 p);

    // Compute Fractal Brownian Motion (FBM) for a given 2D point
    static float FBM(glm::vec2 p);

private:
    // Compute a point on a cubic Bezier curve for a given t parameter
    static glm::vec3 Bezier(
        float t,
        const glm::vec3& p0,
        const glm::vec3& p1,
        const glm::vec3& p2,
        const glm::vec3& p3);

	// Compute the derivative of a cubic Bezier curve for a given t parameter
    static glm::vec3 BezierDerivative(
        float t,
        const glm::vec3& p0,
        const glm::vec3& p1,
        const glm::vec3& p2,
        const glm::vec3& p3);

    // Find the closest point on a cubic Bezier curve to a given point
    static float Closest_Point_Bezier(
        glm::vec2 vertexXZ,
        float& closest_t,
        const glm::vec3& p0,
        const glm::vec3& p1,
        const glm::vec3& p2,
        const glm::vec3& p3);

    // Compute the vertical displacement for a lake region
    static float Displacement_Lake(
        glm::vec3 position,
        glm::vec3 center, float radius, float h_max);

    // Compute the vertical displacement for a waterfall region
    static float Displacement_Waterfall(
        glm::vec3 position,
        const glm::vec3& center, float radius, float h_max,
        const glm::vec3& p0,
        const glm::vec3& p1,
        const glm::vec3& p2,
        const glm::vec3& p3);
};

#endif // __CREATE_PLANE_H__
