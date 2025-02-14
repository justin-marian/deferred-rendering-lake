#include "DeferredRenderingLake/Waterfall.h"
#include "Constants.h"
#include "CreatePlane.h"
#include "utils/text_utils.h"

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <unordered_map>

using namespace std;
\
using LD = Constants::Loader;
using CM = Constants::CubeMap;
using DR = Constants::DeferredRender;
using WL = Constants::WaterfallLake_WaterDrops;


Waterfall::Waterfall() : 
    loader(nullptr), 
    cubeMap(nullptr),
    waterfallLake(nullptr),
	waterDrops(nullptr),
    firefly(nullptr),
    fallingStars(nullptr) {}


Waterfall::~Waterfall()
{
    delete loader;

    delete cubeMap;
	delete waterfallLake;

    delete waterDrops;
	delete firefly;
	delete fallingStars;
}


void Waterfall::Init()
{
    auto resolution = window->GetResolution();
    auto camera = GetSceneCamera();
    camera->SetPositionAndRotation(glm::vec3(0, 10, 4), glm::quat(glm::vec3(RADIANS(10), 0, 0)));
    camera->Update();

    Random::InitRand();

    TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::TEXTURES), "default.png");
    TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::TEXTURES), "ground.jpg");
    TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::TEXTURES), "rain.png");
	TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::TEXTURES), "star.png");
	TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::TEXTURES), "butterfly.jpg");

    std::string path = "ground.jpg";
    char* charPath = new char[path.length() + 1];
    strcpy(charPath, path.c_str());
    Mesh* dynamicPlane = Create::CreateGridMesh("dynamicPlane", 300, 300, 30.0f, 30.0f, charPath);
    meshes["dynamicPlane"] = dynamicPlane;

    loader = new Loader(window);
    loader->LoadAllMeshes(meshes, LD::GetMeshConfigs(window));
    loader->LoadAllShaders(shaders, LD::GetShaderConfigs());

    cubeMap = new CubeMap(window);
    cubeMap->Init(window, CM::WIDTH, CM::HEIGHT);

    waterfallLake = new WaterfallLake(window);
    waterfallLake->Init(window, shaders, meshes, resolution.x, resolution.y, 0);

	waterDrops = new WaterDrops();
	waterDrops->Init(WL::SIZE_X_PARTICLE, WL::SIZE_Y_PARTICLE, WL::SIZE_Z_PARTICLE, WL::NR_PARTICLES);

	firefly = new Firefly();
	firefly->Init(WL::SIZE_X_PARTICLE, WL::SIZE_Y_PARTICLE, WL::SIZE_Z_PARTICLE, WL::NR_PARTICLES / 3);

	fallingStars = new FallingStars();
	fallingStars->Init(WL::SIZE_X_PARTICLE, WL::SIZE_Y_PARTICLE, WL::SIZE_Z_PARTICLE, WL::NR_PARTICLES / 4);
}


void Waterfall::Update(float deltaTimeSeconds)
{
    ClearScreen();
    glEnable(GL_DEPTH_TEST);

    auto camera = GetSceneCamera();
    auto resolution = window->GetResolution();

    glm::vec3 cameraPos = camera->m_transform->GetWorldPosition();
    glm::mat4 viewMatrix = camera->GetViewMatrix();
    glm::mat4 projectionMatrix = camera->GetProjectionMatrix();
	
    glm::mat4 modelMatrix = glm::mat4(1.0f);

    glViewport(0, 0, resolution.x, resolution.y);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    waterfallLake->RenderCompose(
		deltaTimeSeconds, 
        camera, 
        cubeMap, waterDrops, firefly, fallingStars,
		shaders, meshes,
        viewMatrix, projectionMatrix, cameraPos);
}


void Waterfall::OnKeyPress(int key, int mods)
{
    /** Add key press event */
    int index = key - GLFW_KEY_0;
    if (index >= 0 && index <= 9)
    {
        waterfallLake->SetLightType(index);
    }
}

void Waterfall::OnWindowResize(int width, int height)
{
    /** Treat window resize event */
    waterfallLake->ResizeBuffers(width, height);
	cubeMap->ResizeBuffers(width, height); /// NOT NECESSARY
}

void Waterfall::FrameEnd()
{
#if 0 // 1 - ACTIVE / 0 - INACTIVE
    DrawCoordinateSystem();
#endif
}

void Waterfall::FrameStart() { /** Add key release event */ }
void Waterfall::OnKeyRelease(int key, int mods) { /** Add key release event */ }
void Waterfall::OnInputUpdate(float deltaTime, int mods) { /** Treat continous update based on inpute */ }
void Waterfall::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) { /** Add on mouse move */ }
void Waterfall::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) { /** Add on mouse scroll */ }
void Waterfall::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) { /** Add on mouse btn press */ }
void Waterfall::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) { /** Add on mouse btn release */ }
