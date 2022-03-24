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

enum ImGuiImageDrawFlgs_
{
  ImGuiImageDrawFlgs_AignNone = 0,
  ImGuiImageDrawFlgs_TopLeft = 0,
  ImGuiImageDrawFlgs_TopRight = 1,
  ImGuiImageDrawFlgs_BottomLeft = 2,
  ImGuiImageDrawFlgs_BottomRight = 3,
  ImGuiImageDrawFlgs_XCenter = 4,
  ImGuiImageDrawFlgs_YCenter = 8,
  ImGuiImageDrawFlgs_Center = ImGuiImageDrawFlgs_XCenter | ImGuiImageDrawFlgs_YCenter,
};

typedef int ImGuiPopMessageBoxFlags;
typedef int ImGuiButtonIcon;
typedef int ImGuiImageDrawFlgs;

typedef unsigned int uiTextureID;		/* 4-byte unsigned */
struct ImVec2;
struct ImVec4;

namespace ImGui
{
  extern const int CENTER;
  namespace SDLGL2
  {
    bool        Init(const char* title, int x = CENTER, int y = CENTER, int w = 640, int h = 480, bool full_screen = false);
    void        Deinit();


    void        NewFrame();

    bool        Update();
    void        Render();

    uiTextureID CreateTexture();
    void        Image(cv::Mat img, uiTextureID texture_id, ImVec2 region, ImGuiImageDrawFlgs align = ImGuiImageDrawFlgs_AignNone, bool autosize = false);
  }

  void          SetStyle(float window_border, ImVec2 window_padding, ImVec2 frame_padding, float item_spacing);
  bool          RadioIconButton(const char* str_id, int* active_id, int btn_id, ImGuiButtonIcon icon, float scale = 1.5f);
  float         CalcRadioIconButtonSize(float scale = 1.5f);

  void          TabItemReorder(int itemIdx, int offset);

  // PopupMessageBox with OpenPopup
  bool          PopupMessageBox(const char* name, const char* message, ImGuiPopMessageBoxFlags flags);
}