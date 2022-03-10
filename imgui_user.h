#pragma once

#include <opencv2/imgproc.hpp>

enum ImGuiButtonIcon_
{
  ImGuiButtonIcon_RightArrow,
  ImGuiButtonIcon_Add,
  ImGuiButtonIcon_Mag,
  ImGuiButtonIcon_Setting,
};

enum ImGuiPopMessageBoxFlags_
{
  ImGuiPopMessageBoxFlags_None = 0,
  ImGuiPopMessageBoxFlags_OK = 1 << 0,
  ImGuiPopMessageBoxFlags_CANCEL = 1 << 1,
  ImGuiPopMessageBoxFlags_DONT_ASK_ME = 1 << 2,

  ImGuiPopMessageBoxFlags_OK_CANCEL = ImGuiPopMessageBoxFlags_OK | ImGuiPopMessageBoxFlags_CANCEL,
};

typedef int ImGuiPopMessageBoxFlags;
typedef int ImGuiButtonIcon;

typedef unsigned int uiTextureID;		/* 4-byte unsigned */

namespace ImGui
{
  extern const int CENTER;
  namespace SDLGL2
  {
    bool Init(const char* title, int x = CENTER, int y = CENTER, int w = 640, int h = 480, bool full_screen = false);
    void Deinit();


    void NewFrame();

    bool Update();
    void Render();

    uiTextureID CreateTexture();
    bool Show(cv::Mat img, uiTextureID texture_id);
  }

  void SetBackground(float r, float g, float b, float a);
  void SetStyle(float window_border, float window_padding, float item_spacing);
  bool RadioMenuButton(const char* str_id, int* active_id, int btn_id, ImGuiButtonIcon icon, float scale = 1.5f);

  // PopupMessageBox with OpenPopup
  bool PopupMessageBox(const char* name, const char* message, ImGuiPopMessageBoxFlags flags);
}