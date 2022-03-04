#pragma once

#include <opencv2/imgproc.hpp>

enum CustomGuiIcon_
{
  CustomGuiIcon_RightArrow,
  CustomGuiIcon_Add,
  CustomGuiIcon_Mag,
  CustomGuiIcon_Setting,
};

typedef unsigned int uiTextureID;		/* 4-byte unsigned */

namespace ImGui
{
  extern const int CENTER;
  bool Init(const char* title, int x = CENTER, int y = CENTER, int w = 640, int h = 480, bool full_screen = false);
  void deinit();

  void set_background_cr(float r, float g, float b, float a);

  void new_frame();
  void set_style(float window_border, float window_padding, float item_spacing);

  bool update();
  void render();

  uiTextureID create_texture();
  bool imshow(cv::Mat img, uiTextureID texture_id);

  bool RadioMenuButton(const char* str_id, int* active_id, int btn_id, int icon, float scale = 1.5f);
}