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
  uiTextureID frame_texture_id = ImGui::SDLGL2::CreateTexture();

  // Main loop
  cv::Mat frame;
  bool done = false;
  ImGuiStyle& style = ImGui::GetStyle();
  ImGui::SetStyle(0.0f, 8.0f, 15.0f);

  while (!done)
  {
    ImGui::SDLGL2::NewFrame();

    static int active_menu = -1;
    ImVec2 TotalSize(0, 0);
    {
      ImGuiWindowFlags window_flags =
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

      ImGui::Begin("Menu Buttons", NULL, window_flags);
      ImGui::RadioMenuButton("##menu_live", &active_menu, 0, ImGuiButtonIcon_RightArrow);
      if (ImGui::IsItemHovered()) ImGui::SetTooltip("Live");

      ImGui::RadioMenuButton("##menu_regist", &active_menu, 1, ImGuiButtonIcon_Add);
      if (ImGui::IsItemHovered()) ImGui::SetTooltip("Regist");

      ImGui::RadioMenuButton("##menu_status", &active_menu, 2, ImGuiButtonIcon_Mag);
      if (ImGui::IsItemHovered()) ImGui::SetTooltip("Registration Status");

      if (ImGui::Button("-")) ImGui::OpenPopup("Delete?");
      ImGui::PopupMessageBox("Delete?", "All those beautiful files will be deleted.\nThis operation cannot be undone!\n\n", ImGuiPopMessageBoxFlags_OK_CANCEL | ImGuiPopMessageBoxFlags_DONT_ASK_ME);

      ImGui::SetCursorPosY(io.DisplaySize.y - ImGui::GetItemRectSize().y - style.ItemSpacing.y);
      ImGui::RadioMenuButton("##menu_setting", &active_menu, 3, ImGuiButtonIcon_Setting);
      if (ImGui::IsItemHovered()) ImGui::SetTooltip("Setting");

      TotalSize.x += ImGui::GetWindowSize().x;
      ImGui::End();

      ImGui::PopStyleColor(4);
    }

    if (active_menu >= 0)
    {
      ImGuiWindowFlags window_flags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoNav;

      ImGui::SetNextWindowPos(ImVec2(TotalSize.x, 0), ImGuiCond_Always);
      ImGui::SetNextWindowSize(ImVec2(0, io.DisplaySize.y), ImGuiCond_Always);

      ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.15f, 0.15f, 0.15f, 1.00f));
      ImGui::Begin("Context Menu", NULL, window_flags);
      std::string strCamID; strCamID.reserve(15);

      switch (active_menu)
      {
      case 0:
        ImGui::Text("Resolution : %d x %d", frame.cols, frame.rows);
        ImGui::Text("Display: %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        break;
      case 1:
        ImGui::Text("Regist: %d", 10);
        break;
      case 2:
        ImGui::Text("Registration Status");
        break;
      case 3:

        //ImGui::Text("Setting");
        ImGui::Text("장치");
        ImGui::SameLine();
        ImGui::InputText("##devID", strCamID.data(), 13);
        break;
      default:
        break;
      }
      TotalSize.x += ImGui::GetWindowSize().x;
      ImGui::End();
      ImGui::PopStyleColor(1);
    }

    {
      ImGuiWindowFlags window_flags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMouseInputs |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoNav |
        ImGuiWindowFlags_NoBackground;

      ImGui::SetNextWindowPos(ImVec2(TotalSize.x, 0), ImGuiCond_Always);
      ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x - TotalSize.x, io.DisplaySize.y), ImGuiCond_Always);

      cap.read(frame);

      ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
      ImGui::Begin("View", NULL, window_flags);
      ImGui::SDLGL2::Show(frame, frame_texture_id);
      ImGui::End();
      ImGui::PopStyleVar();
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
