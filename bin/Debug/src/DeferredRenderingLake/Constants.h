#pragma once

#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

#include "core/window/window_object.h"
#include "Structures.h"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <unordered_map>


class Constants
{
public:
    struct CreatePlane
    {
        static constexpr float WIDTH = 20.0f;
        static constexpr float HEIGHT = 20.0f;
        static constexpr unsigned int SEGMENTS_X = 100;
        static constexpr unsigned int SEGMENTS_Y = 100;
    };

    struct CubeMap
    {
        static constexpr int WIDTH = 1024;
        static constexpr int HEIGHT = 1024;

        static constexpr float NEAR_PLANE = 0.1f;
        static constexpr float FAR_PLANE = 100.0f;

        static const glm::mat4 VIEW_MATRICES[6];
        static const glm::mat4 PROJECTION_MATRIX;

        static constexpr float ROTATION_SPEED = glm::radians(6.0f);
    };

    struct DeferredRender
    {
        static constexpr unsigned int MAX_LIGHTS = 300;
        static constexpr unsigned int G_BUFFER_COUNT = 3;

        // Light properties
        static constexpr float LIGHT_RADIUS = 2.5f;
        static constexpr float LIGHT_POSITION_SCALE_X = 10.0f;
        static constexpr float LIGHT_POSITION_OFFSET_X = 10.0f;
        static constexpr float LIGHT_POSITION_SCALE_Y = 3.0f;
        static constexpr float LIGHT_POSITION_SCALE_Z = 10.0f;
        static constexpr float LIGHT_POSITION_OFFSET_Z = 10.0f;

        // Box properties
        static constexpr glm::vec3 BOX_TRANSLATION = glm::vec3(1.5f, 0.5f, 0.0f);
        static constexpr glm::vec3 BOX_SCALE = glm::vec3(0.5f);

        // Sphere properties
        static constexpr glm::vec3 SPHERE_TRANSLATION = glm::vec3(-4.0f, 1.0f, 1.0f);
        static constexpr glm::vec3 SPHERE_SCALE = glm::vec3(1.0f);

        // Ground plane properties
        static constexpr glm::vec3 PLANE_TRANSLATION = glm::vec3(0.0f, 0.0f, 0.0f);
        static constexpr glm::vec3 PLANE_SCALE = glm::vec3(0.5f);
    };

    struct Loader
    {
        static constexpr unsigned int MAX_TEXTURES = 32;
        static constexpr unsigned int MAX_MODELS = 50;

        // Shaders and Models Paths
        static const std::vector<ShaderConfig> GetShaderConfigs();
        static const std::vector<MeshConfig> GetMeshConfigs(WindowObject* window);
    };

    struct WaterfallLake_WaterDrops
    {
        static constexpr unsigned int SIZE_X_PARTICLE = 30;
        static constexpr unsigned int SIZE_Y_PARTICLE = 30;
        static constexpr unsigned int SIZE_Z_PARTICLE = 30;

        // Water droplets numbers and size particle
        static constexpr unsigned int NR_PARTICLES = 4000;
        static constexpr float SIZE_PARTICLE = 0.2f;

        static constexpr unsigned int DEFAULT_ID = 0;

        static constexpr unsigned int DEFAULT_FRAMEBUFFER_OBJECT = 0;
        static constexpr unsigned int DEFAULT_COLOR_TEXTURE = 0;
        static constexpr unsigned int DEFAULT_DEPTH_TEXTURE = 0;

        static constexpr glm::vec3 CENTER = glm::vec3(0.0f, 0.0f, 0.0f);
        static constexpr float RADIUS = 10.0f;
        static constexpr float H_MAX = 5.5f;

		// Bezier control points - Waterfall
        static constexpr glm::vec3 CONTROL_P0 = glm::vec3(+08.00f, +00.60f, -14.00f);
        static constexpr glm::vec3 CONTROL_P1 = glm::vec3(+05.50f, -01.20f, -11.00f);
        static constexpr glm::vec3 CONTROL_P2 = glm::vec3(+01.50f, -02.40f, -06.00f);
        static constexpr glm::vec3 CONTROL_P3 = glm::vec3(+00.00f, -04.75f, -03.50f);
    };
};

#endif // __CONSTANTS_H__
