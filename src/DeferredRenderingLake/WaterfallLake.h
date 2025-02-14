#pragma once

#ifndef WATERFALL_LAKE_H
#define WATERFALL_LAKE_H

#include "components/simple_scene.h"
#include "components/camera.h"
#include "utils/glm_utils.h"
#include "core/gpu/mesh.h"
#include "core/gpu/shader.h"
#include "core/gpu/frame_buffer.h"
#include "core/window/window_object.h"

#include "Structures.h"
#include "CubeMap.h"
#include "WaterfallLake.h"
#include "WaterDrops.h"
#include "Firefly.h"
#include "FallingStars.h"

#include <vector>
#include <unordered_map>


class CubeMap;

class WaterfallLake : public gfxc::SimpleScene
{
public:
    explicit WaterfallLake(WindowObject* window);
    ~WaterfallLake();

	// Resize the Framebuffers framebuffer and lightBuffer
    void ResizeBuffers(int width, int height);

	// Initialize the Light sources + Framebuffers
    void Init(
        WindowObject* window,
        std::unordered_map<std::string, Shader*>& shaders,
        std::unordered_map<std::string, Mesh*>& meshes,
        int width, int height, int light_type);

	// Render the WHOLE SCENE WITH DEFERRED RENDERING + CUBEMAP + PARTICLE EFFECTS
    void RenderCompose(
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
        const glm::vec3& cameraPos);

	void SetLightType(int type) { light_type = type; }
	int GetLightType() const { return light_type; }

private:
	// Create the framebuffer for Deferred Rendering
    void CreateFramebuffer(int width, int height);

private:
    ////////////////////////////////////
    WindowObject* window;
    std::unordered_map<std::string, Mesh*>* meshes;
    /////////////////////////////////////
    glm::vec3 control_p0, control_p1, control_p2, control_p3;
    ////////////////////////////////////
    int light_type = 6;
    std::vector<Light> lights;
    FrameBuffer* frameBuffer;
    FrameBuffer* lightBuffer;
	//FrameBuffer* reflectionBuffer;
    ////////////////////////////////////
};

#endif // WATERFALL_LAKE_H
