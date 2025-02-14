#pragma once

#ifndef CUBEMAP_H
#define CUBEMAP_H

#include "components/simple_scene.h"
#include "core/gpu/shader.h"
#include "core/gpu/frame_buffer.h"
#include "core/managers/resource_path.h"
#include "core/window/window_object.h"
#include "stb/stb_image.h"

#include "Structures.h"
#include "WaterfallLake.h"

#include <string>
#include <unordered_map>


class CubeMap : public gfxc::SimpleScene
{
public:
    explicit CubeMap(WindowObject* window);
    ~CubeMap();

    /////////////////////////////////////////////////////////////////////////////////////////////////
    void Init(WindowObject* windowObj, int w, int h);
    void ResizeBuffers(int width, int height);

    GLuint GetFramebufferID() const { return framebuffer_object; }
    GLuint GetCubeTexture() const { return cube_texture; }
    GLuint GetColorTextureID() const { return color_texture; }
    GLuint GetDepthTextureID() const { return depth_texture; }

    float GetCubeAngle() const { return cube_angle; }
    void SetCubeAngle(float angle) { cube_angle = angle; }
    int GetShadowType() const { return shadow_type; }
    void SetShadowType(int type) { shadow_type = type; }

	int GetWidth() const { return width; }
	int GetHeight() const { return height; }

private:
    void CreateFramebuffer(int width, int height);

    // Load cubemap texture from 6 images
    GLuint UploadCubeMapTexture(
        const std::string& posx, const std::string& posy, const std::string& posz,
        const std::string& negx, const std::string& negy, const std::string& negz);

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    WindowObject* window;

    GLuint framebuffer_object;    /// < Framebuffer object ID
    GLuint cube_texture;           /// < CubeMap texture ID
    GLuint color_texture;
    GLuint depth_texture;

    int width, height;
    float cube_angle;
    int shadow_type;

    glm::ivec2 resolution;
    std::unordered_map<std::string, Mesh*> meshes;
};

#endif // CUBEMAP_H
