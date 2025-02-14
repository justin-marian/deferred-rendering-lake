#include "Constants.h"
#include "core/managers/resource_path.h"
#include "utils/text_utils.h"

using namespace std;


const glm::mat4 Constants::CubeMap::VIEW_MATRICES[6] = 
{
    glm::lookAt(glm::vec3(0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
    glm::lookAt(glm::vec3(0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
    glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
    glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
    glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
    glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))
};


const glm::mat4 Constants::CubeMap::PROJECTION_MATRIX =
    glm::perspective(glm::radians(90.0f), 1.0f, 
    Constants::CubeMap::NEAR_PLANE, Constants::CubeMap::FAR_PLANE);


const std::vector<ShaderConfig> Constants::Loader::GetShaderConfigs()
{
    return
    {
        {"WaterDrops", "WaterDrops", "WaterDrops", "WaterDrops", true},
        {"FallingStars", "FallingStars", "FallingStars", "FallingStars", true},
        {"Firefly", "Firefly", "Firefly", "Firefly", true},
        {"CubeMapFramebufferShader", "Framebuffer", "Framebuffer", "Framebuffer", true},
        {"CubeMapReflectionShader", "CubeMap", "CubeMap", "", false},
        {"CubeMapNormalShader", "Normal", "Normal", "", false},
        {"DeferredRenderCompositionShader", "Composition", "Composition", "", false},
        {"DeferredRenderLightPassShader", "LightPass", "LightPass", "", false},
        {"DeferredRender2TextureShader", "Render2Texture", "Render2Texture", "", false},
    };
};


const std::vector<MeshConfig> Constants::Loader::GetMeshConfigs(WindowObject* window) 
{
    return 
    {
        {"quad", PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "quad.obj", false},
        {"cube", PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj", false},
        {"bunny", PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "animals"), "bunny.obj", false},
        {"box", PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj", false},
        {"sphere", PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj", false},
        {"archer", PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "characters", "archer"), "Archer.fbx", false},
		{"plane", PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "plane50.obj", false}
    };
}
