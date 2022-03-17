// Dear ImGui: standalone example application for SDL2 + OpenGL
// (SDL is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

// **DO NOT USE THIS CODE IF YOUR CODE/ENGINE IS USING MODERN OPENGL (SHADERS, VBO, VAO, etc.)**
// **Prefer using the code in the example_sdl_opengl3/ folder**
// See imgui_impl_sdl.cpp for details.

#define  IMGUI_INCLUDE_IMGUI_USER_H
#include "imgui.h"
#include <stdio.h>

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

// Main code
int main(int, char**)
{
  int initCamWidth = 1280;
  int initCamHeight = 720;
  const int initCamFps = 60;

  cv::VideoCapture cap;
  cap.open("/dev/video0");

  if (!cap.isOpened())
  {
    printf("Error: cam cannot open!\n");
    return -1;
  }

  cap.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));
  cap.set(cv::CAP_PROP_FRAME_WIDTH, initCamWidth);
  cap.set(cv::CAP_PROP_FRAME_HEIGHT, initCamHeight);
  cap.set(cv::CAP_PROP_FPS, initCamFps);

  ImGui::SDLGL2::Init("GUI", ImGui::CENTER, ImGui::CENTER, 1280, 720);
  ImGui::GetIO().IniFilename = "GUI.ini";

  ImGuiIO& io = ImGui::GetIO();
  ImFont* font = io.Fonts->AddFontFromFileTTF("font.ttf", 32.0f, NULL, io.Fonts->GetGlyphRangesKorean());

  // Our state
  ImGui::SetBackground(0.12f, 0.12f, 0.12f, 1.00f);
  uiTextureID view_texture_id = ImGui::SDLGL2::CreateTexture();
  std::vector<int> texture_ids;
  for (int r = 0; r < 2; r++)
  {
    for (int c = 0; c < 5; c++)
    {
      texture_ids.push_back(ImGui::SDLGL2::CreateTexture());
    }
  }

  std::string strCamID; strCamID.reserve(15);

  // Main loop
  cv::Mat frame;
  bool done = false;
  ImGuiStyle& style = ImGui::GetStyle();
  ImGui::SetStyle(0.0f, ImVec2(0.0f, 0.0f), ImVec2(8.0f, 8.0f), 10.0f);

  int active_menu = 1;
  std::string name; name.reserve(15);
  std::string work; work.reserve(15);
  bool bBtCapture = false, bBtRecapture = false, bBtManulRegist = false;
  while (!done)
  {
    active_menu = std::max(0, active_menu);

    ImGui::SDLGL2::NewFrame();

    ImVec2 TotalSize(0, 0);
    {
      ImGuiWindowFlags window_flags =
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoNav;

      ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
      ImGui::SetNextWindowSize(ImVec2(0, io.DisplaySize.y), ImGuiCond_Always);

      ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.2f, 0.2f, 0.2f, 1.00f));
      ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
      ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
      ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

      float btn_scale = 2.0f;
      ImGui::Begin("Menu Buttons", NULL, window_flags);
      ImGui::RadioIconButton("##MENU LIVE", &active_menu, 0, ImGuiButtonIcon_RightArrow, btn_scale);
      if (ImGui::IsItemHovered()) ImGui::SetTooltip("Live");

      ImGui::RadioIconButton("##MENU REGIST", &active_menu, 1, ImGuiButtonIcon_Add, btn_scale);
      if (ImGui::IsItemHovered()) ImGui::SetTooltip("Regist");

      ImGui::RadioIconButton("##MENU STATUS", &active_menu, 2, ImGuiButtonIcon_Mag, btn_scale);
      if (ImGui::IsItemHovered()) ImGui::SetTooltip("Registration Status");

      if (ImGui::Button("-")) ImGui::OpenPopup("Delete?");
      ImGui::PopupMessageBox("Delete?", "All those beautiful files will be deleted.\nThis operation cannot be undone!\n\n", ImGuiPopMessageBoxFlags_OK_CANCEL | ImGuiPopMessageBoxFlags_DONT_ASK_ME);

      ImGui::SetCursorPosY(io.DisplaySize.y - ImGui::CalcRadioIconButtonSize(btn_scale) - style.ItemSpacing.y);
      ImGui::RadioIconButton("##MENU SETTING", &active_menu, 3, ImGuiButtonIcon_Setting, btn_scale);
      if (ImGui::IsItemHovered()) ImGui::SetTooltip("Setting");

      TotalSize.x += ImGui::GetWindowSize().x;
      ImGui::End();

      ImGui::PopStyleColor(4);
    }

    if (active_menu == 0)
    {
      ImGuiWindowFlags window_flags =
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMouseInputs |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoNav |
        ImGuiWindowFlags_NoBackground;

      cap.read(frame);

      ImGui::SetNextWindowPos(ImVec2(TotalSize.x, 0.0f), ImGuiCond_Always);
      ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x - TotalSize.x, io.DisplaySize.y), ImGuiCond_Always);
      ImGui::Begin("##VIEW", NULL, window_flags);
      ImVec2 pos = ImGui::SDLGL2::Image(frame, view_texture_id, ImGuiImageDrawFlgs_Center, true);
      ImGui::End();

      ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - pos.x - 32.0f, pos.y + 32.0f), ImGuiCond_Always, ImVec2(1.0f, 0.0f));
      ImGui::SetNextWindowBgAlpha(1.0f); // Transparent background
      ImGui::Begin("##LIVE", NULL, window_flags);
      ImGui::SetWindowFontScale(2.0f);
      ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "LIVE");
      ImDrawList* draw_list = ImGui::GetWindowDrawList();
      draw_list->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(255, 0, 0, 255));
      ImGui::End();
    }
    else
    {
      ImGuiWindowFlags window_flags =
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoNav |
        ImGuiWindowFlags_AlwaysUseWindowPadding;

      int width = active_menu == 0 ? 0 : io.DisplaySize.x - TotalSize.x;
      ImGui::SetNextWindowPos(ImVec2(TotalSize.x, 0), ImGuiCond_Always);
      ImGui::SetNextWindowSize(ImVec2(width, io.DisplaySize.y), ImGuiCond_Always);

      ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.15f, 0.15f, 0.15f, 1.00f));
      ImGui::Begin("Context Menu", NULL, window_flags);
      const char* title = NULL;
      const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();
      int sel = -1;

      switch (active_menu)
      {
      case 1:
        title = "Regist";
        break;
      case 2:
      {
        title = "Registration Status";
      }
      break;
      case 3:
        title = "Setting";
        break;
      default:
        break;
      }

      if (title)
      {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.0f, 8.0f));
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.12f, 0.12f, 0.12f, 1.00f));
        ImGui::BeginChild("##TITLE", ImVec2(0, ImGui::GetTextLineHeight() + 16.0f), false, window_flags);
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), title);
        ImGui::EndChild();
        ImGui::PopStyleColor(1);
        ImGui::PopStyleVar();
      }

      switch (active_menu)
      {
      case 1:
      {
        ImVec2 region = ImGui::GetContentRegionAvail();
        const float padding = 40;
        const float info_region = region.x / 3;
        const float view_region = region.x - info_region;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(padding, 0.0f));

        ImGui::BeginChild("##Cam", ImVec2(0, ImGui::GetTextLineHeight() + 16.0f), false, window_flags);
        const char* items[] = { "Cam1", "Cam2" };
        static int current_item = 0;
        ImGui::SetNextItemWidth(ImGui::CalcTextSize("Cam10      ").x);
        ImGui::Combo("##Cam", &current_item, items, IM_ARRAYSIZE(items));
        ImGui::EndChild();

        cap.read(frame);

        ImGui::BeginChild("##VIEW CHILD", ImVec2(view_region, 0), false, window_flags);
        ImVec2 pos = ImGui::SDLGL2::Image(frame, view_texture_id, ImGuiImageDrawFlgs_Center, true);
        pos.y += ImGui::GetWindowPos().y;
        ImGui::EndChild();

        cv::Mat img = cv::imread("0.jpg");
        ImGui::SameLine();
        ImGui::BeginChild("##INFO CHILD", ImVec2(info_region, 0), false, window_flags);
        ImGui::SetWindowFontScale(1.0f);
        ImGui::SDLGL2::Image(img, texture_ids[0]);
        ImGui::Text("이름: ");  ImGui::SameLine();
        ImGui::PushItemWidth(ImGui::CalcTextSize("A").x * 13);
        ImGui::InputText("##NAME", name.data(), 13);
        ImGui::Text("직급: ");  ImGui::SameLine();
        ImGui::InputText("##WORK", work.data(), 13);
        ImGui::PopItemWidth();
        ImGui::SetCursorPosY(region.y - ImGui::GetWindowPos().y - ImGui::GetFrameHeight() - style.ItemSpacing.y);
        ImVec2 btn_size(ImGui::CalcTextSize("A").x * 5, 0);
        if (!bBtCapture)
          bBtCapture = ImGui::Button("촬영", btn_size);
        else
        {
          bBtRecapture = ImGui::Button("재촬영", btn_size);
          ImGui::SameLine(0.0f, btn_size.x);
          bBtManulRegist = ImGui::Button("등록 ", btn_size);
        }
        ImGui::EndChild();

        ImGui::PopStyleVar();

        ImGui::SetNextWindowPos(ImVec2(TotalSize.x + view_region - pos.x - 16.0f, pos.y + 16.0f), ImGuiCond_Always, ImVec2(1.0f, 0.0f));
        ImGui::SetNextWindowBgAlpha(1.0f); // Transparent background
        ImGui::BeginChild("##LIVE", ImGui::CalcTextSize("LIVE"), false, window_flags);
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "LIVE");
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        draw_list->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(255, 0, 0, 255));
        ImGui::EndChild();
      }
      break;
      case 2:
      {
        ImVec2 region = ImGui::GetContentRegionAvail();
        const float padding = 40;
        const float info_region = region.x / 4;
        const float icon_region = region.x - info_region;
        const float icon_spacing = 40;
        const float icon_size = 112;
        const int column = (icon_region - padding * 2) / (icon_size + icon_spacing);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(padding, 0.0f));

        ImGui::BeginChild("##STATUS CHILD", ImVec2(icon_region, 0), false, window_flags);
        ImGuiTableFlags table_flags =
          ImGuiTableFlags_SizingFixedFit |
          //  ImGuiTableFlags_SizingFixedSame;
          ImGuiTableFlags_SizingFixedSame;

        static int clicked_id = 0;
        if (ImGui::BeginTable("##STATUS", column, table_flags, ImVec2(icon_region - padding * 2, 0.0f)))
        {
          ImGui::SetWindowFontScale(0.7f);
          for (int r = 0; r < 2; r++)
          {
            ImGui::TableNextRow();
            for (int c = 0; c < column; c++)
            {
              cv::Mat img = cv::imread(cv::format("%d.jpg", r * column + c));
              ImGui::TableNextColumn();
              ImVec2 icon_pos = ImGui::SDLGL2::Image(img, texture_ids[r * column + c], ImGuiImageDrawFlgs_XCenter);
              float indent = icon_pos.x - ImGui::GetCursorPos().x;
              ImGui::Indent(indent);
              ImGui::Text("이름: 홍길동");
              int id = r * column + c;
              ImGui::PushID(id);
              if (ImGui::Button("정보조회", ImVec2(icon_size, 0)))
                clicked_id = id;
              ImGui::PopID();
              ImGui::Unindent(indent);
            }
            ImGui::NewLine();
          }
          ImGui::EndTable();
        }
        ImGui::EndChild();

        cv::Mat img = cv::imread(cv::format("%d.jpg", clicked_id));
        ImGui::SameLine();
        ImGui::BeginChild("##INFO CHILD", ImVec2(info_region, 0), false, window_flags);
        ImGui::SetWindowFontScale(1.0f);
        ImVec2 icon_pos = ImGui::SDLGL2::Image(img, view_texture_id, ImGuiImageDrawFlgs_XCenter, true);
        float indent = icon_pos.x - ImGui::GetCursorPos().x;
        ImGui::Indent(indent);
        ImGui::Text("ID: %d", clicked_id);
        ImGui::Text("이름: 홍길동");
        ImGui::Text("등록일: 홍길동");
        ImGui::Unindent(indent);
        ImGui::EndChild();

        ImGui::PopStyleVar();
      }
      break;
      case 3:
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.0f, 8.0f));
        ImGui::BeginChild("##CHILD", ImVec2(0, 0), false, window_flags);
        ImGui::Text("장치");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(ImGui::CalcTextSize("/dev/video10  ").x);
        ImGui::InputText("##DEVID", strCamID.data(), 13);
        ImGui::EndChild();
        ImGui::PopStyleVar();
        break;
      default:
        break;
      }

      TotalSize.x += ImGui::GetWindowSize().x;
      ImGui::End();
      ImGui::PopStyleColor(1);
    }

    // {
    //   ImGuiWindowFlags window_flags =
    //     ImGuiWindowFlags_NoDecoration |
    //     ImGuiWindowFlags_AlwaysAutoResize |
    //     ImGuiWindowFlags_NoSavedSettings |
    //     ImGuiWindowFlags_NoFocusOnAppearing |
    //     ImGuiWindowFlags_NoNav |
    //     ImGuiWindowFlags_NoMove;

    //   ImGui::OpenPopup("##Overlay Text");
    //   ImGui::SetNextWindowPos(ImVec2(TotalSize.x, 0), ImGuiCond_Always);
    //   ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x - TotalSize.x, io.DisplaySize.y), ImGuiCond_Always);
    //   ImGui::SetNextWindowBgAlpha(0.5f); // Transparent background
    //   ImGui::Begin("##Overlay Text", NULL, window_flags);
    //   ImGui::SetWindowFontScale(2.0f);
    //   ImGui::Text("인증(Confirm)");
    //   ImGui::End();
    // }

    // if (!ImGui::IsPopupOpen("Delete?"))
    //   ImGui::OpenPopup("##Overlay Text");
    // {
    //   ImGui::SetNextWindowPos(ImVec2(TotalSize.x + 20, 20), ImGuiCond_Always);
    //   ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1.0f, 0.0f, 0.0f, 0.35f));
    //   if (ImGui::BeginPopupModal("##Overlay Text", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar))
    //   {
    //     ImGui::SetWindowFontScale(2.0f);
    //     ImGui::Text("인증(Confirm)");
    //     ImGui::EndPopup();
    //   }
    //   ImGui::PopStyleColor();
    // }

    done = ImGui::SDLGL2::Update();
  }

  ImGui::SDLGL2::Deinit();
  return 0;
}
