#ifndef LOADER_H
#define LOADER_H

#include "core/gpu/mesh.h"
#include "core/gpu/shader.h"
#include "core/gpu/frame_buffer.h"
#include "core/managers/resource_path.h"
#include "core/window/window_object.h"

#include "Structures.h"

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>


class Loader
{
public:
    explicit Loader(WindowObject* window);
    ~Loader();

    // Load all shaders using a matrix (list of ShaderConfig)
    void LoadAllShaders(
        std::unordered_map<std::string, Shader*>& shaders,
        const std::vector<ShaderConfig>& configs);

    // Load all meshes using a list of configurations
    void LoadAllMeshes(
        std::unordered_map<std::string, Mesh*>& meshes,
        const std::vector<MeshConfig>& configs);

private:
    // Load a single shader program
    void LoadShader(
        std::unordered_map<std::string, Shader*>& shaders,
        const ShaderConfig& config);

    // Load a single mesh
    void LoadMesh(
        std::unordered_map<std::string, Mesh*>& meshes,
        const MeshConfig& config);

private:
    WindowObject* window;
};

#endif // LOADER_H
