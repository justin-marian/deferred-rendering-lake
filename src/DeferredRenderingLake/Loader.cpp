#include "Loader.h"

#include "utils/gl_utils.h"

#include <iostream>

using namespace std;


Loader::Loader(WindowObject* window) : window(window) {}

Loader::~Loader()
{
    if (window)
    {
        delete window;
        window = nullptr;
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function: LoadShader
// Description: Loads a shader from a configuration and stores it in the provided map.
// Parameters:
//   - shaders: Map of shaders to store the loaded shader.
//   - config: Configuration for loading the shader.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Loader::LoadShader(
    unordered_map<string, Shader*>& shaders,
    const ShaderConfig& config)
{
    string baseShaderPath = PATH_JOIN(window->props.selfDir, SOURCE_PATH::PATH_PROJECT, "DeferredRenderingLake", "Shaders");
    string shaderFolder = PATH_JOIN(baseShaderPath, config.shaderName);

    cout << "Trying to load shader: " << config.shaderName << endl;

    string vertexPath = PATH_JOIN(shaderFolder, config.vertexShader + ".VS.glsl");
    string fragmentPath = PATH_JOIN(shaderFolder, config.fragmentShader + ".FS.glsl");
    string geometryPath = config.hasGeometry ? PATH_JOIN(shaderFolder, config.geometryShader + ".GS.glsl") : "";

    cout << "Vertex Shader Path: " << vertexPath << endl;
    cout << "Fragment Shader Path: " << fragmentPath << endl;
    if (config.hasGeometry)
    {
        cout << "Geometry Shader Path: " << geometryPath << endl;
    }

    Shader* shader = new Shader(config.shaderName);
    shader->AddShader(vertexPath, GL_VERTEX_SHADER);
    shader->AddShader(fragmentPath, GL_FRAGMENT_SHADER);
    if (config.hasGeometry) 
    {
        shader->AddShader(geometryPath, GL_GEOMETRY_SHADER);
    }

    if (!shader->CreateAndLink())
    {
        cerr << "Error linking shader program: " << config.shaderName << endl;
        delete shader;
        return;
    }

    shaders[config.shaderName] = shader;
    cout << "Shader '" << config.shaderName << "' successfully loaded and linked!" << endl;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function: LoadAllShaders
// Description: Loads all shaders from a list of configurations and stores them in the provided map.
// Parameters:
//   - shaders: Map of shaders to store the loaded shaders.
//   - configs: List of configurations for loading shaders.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Loader::LoadAllShaders(
    unordered_map<string, Shader*>& shaders,
    const vector<ShaderConfig>& configs)
{
    for (const auto& config : configs) 
    {
        LoadShader(shaders, config);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function: LoadMesh
// Description: Loads a mesh from a file and stores it in the provided map.
// Parameters:
//   - meshes: Map of meshes to store the loaded mesh.
//   - meshName: Name of the mesh to be loaded.
//   - folderPath: Path to the folder containing the mesh file.
//   - fileName: Name of the mesh file.
//   - useMaterials: Flag indicating whether to use materials for the mesh.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Loader::LoadMesh(
    unordered_map<string, Mesh*>& meshes,
    const MeshConfig& config)
{
    cout << "Loading mesh: " << config.meshName << endl;

    if (meshes.find(config.meshName) != meshes.end())
    {
        cerr << "Warning: Mesh with name '" << config.meshName << "' already exists. Skipping load." << endl;
        return;
    }

    Mesh* mesh = new Mesh(config.meshName);
    string fullPath = PATH_JOIN(config.folderPath, config.fileName);

    if (!mesh->LoadMesh(config.folderPath, config.fileName))
    {
        cerr << "Error: Failed to load mesh '" << config.meshName << "' from '" << fullPath << "'!" << endl;
        delete mesh;
        return;
    }

    mesh->UseMaterials(config.useMaterials);
    meshes[mesh->GetMeshID()] = mesh;
    cout << "Mesh '" << config.meshName << "' successfully loaded!" << endl;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function: LoadAllMeshes
// Description: Loads all meshes from a list of configurations and stores them in the provided map.
// Parameters:
//   - meshes: Map of meshes to store the loaded meshes.
//   - configs: List of configurations for loading meshes.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Loader::LoadAllMeshes(
    unordered_map<string, Mesh*>& meshes,
    const vector<MeshConfig>& configs)
{
    for (const auto& config : configs)
    {
        LoadMesh(meshes, config);
    }
}
