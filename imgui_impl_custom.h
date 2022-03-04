#pragma once

#include <SDL.h>
#include <SDL_opengl.h>

#include <opencv2/imgproc.hpp>

#include "imgui.h"

enum CustomGuiIcon_
{
  CustomGuiIcon_RightArrow,
  CustomGuiIcon_Add,
  CustomGuiIcon_Mag,
  CustomGuiIcon_Setting,
};

bool RadioMenuButton(const char* str_id, int* active_id, int btn_id, int icon, float scale = 1.5f);
bool RenderImage(cv::Mat img, ImTextureID texture_id);