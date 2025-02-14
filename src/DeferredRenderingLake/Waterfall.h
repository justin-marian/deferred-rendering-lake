#pragma once

#ifndef WATERFALL_H
#define WATERFALL_H

#include "components/simple_scene.h"
#include "components/transform.h"
#include "core/gpu/frame_buffer.h"

#include "Utils.h"
#include "Structures.h"
#include "Loader.h"

#include "CubeMap.h"
#include "WaterfallLake.h"

#include "WaterDrops.h"
#include "Firefly.h"
#include "FallingStars.h"

#include <string>
#include <vector>


class Waterfall : public gfxc::SimpleScene
{
public:
    Waterfall();
    ~Waterfall();

    void Init() override;

private:
    void FrameStart() override;
    void Update(float deltaTimeSeconds) override;
    void FrameEnd() override;

    void OnInputUpdate(float deltaTime, int mods) override;
    void OnKeyPress(int key, int mods) override;
    void OnKeyRelease(int key, int mods) override;
    void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
    void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
    void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
    void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
    void OnWindowResize(int width, int height) override;

private:
    Loader* loader;
  
    CubeMap* cubeMap;
    WaterfallLake* waterfallLake;

    WaterDrops* waterDrops;
	Firefly* firefly;
	FallingStars* fallingStars;
};

#endif // WATERFALL_H
