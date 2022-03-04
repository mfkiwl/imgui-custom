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

  ImGui::Init("GUI", ImGui::CENTER, ImGui::CENTER, 1280, 720);
  ImGuiIO& io = ImGui::GetIO();
  ImFont* font = io.Fonts->AddFontFromFileTTF("font.ttf", 32.0f, NULL, io.Fonts->GetGlyphRangesKorean());

  // Our state
  ImGui::set_background_cr(0.12f, 0.12f, 0.12f, 1.00f);
  uiTextureID frame_texture_id = ImGui::create_texture();

  // Main loop
  cv::Mat frame;
  bool done = false;
  while (!done)
  {
    ImGui::new_frame();
    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::set_style(0.0f, 8.0f, 15.0f);

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
      ImGui::RadioMenuButton("#menu_live", &active_menu, 0, CustomGuiIcon_RightArrow);
      if (ImGui::IsItemHovered()) ImGui::SetTooltip("Live");

      ImGui::RadioMenuButton("#menu_regist", &active_menu, 1, CustomGuiIcon_Add);
      if (ImGui::IsItemHovered()) ImGui::SetTooltip("Regist");

      ImGui::RadioMenuButton("#menu_status", &active_menu, 2, CustomGuiIcon_Mag);
      if (ImGui::IsItemHovered()) ImGui::SetTooltip("Registration Status");

      ImGui::SetCursorPosY(io.DisplaySize.y - ImGui::GetItemRectSize().y - style.ItemSpacing.y);
      ImGui::RadioMenuButton("#menu_setting", &active_menu, 3, CustomGuiIcon_Setting);
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
        ImGui::Text("Setting");
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
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoNav |
        ImGuiWindowFlags_NoBackground;

      ImGui::SetNextWindowPos(ImVec2(TotalSize.x, 0), ImGuiCond_Always);
      ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x - TotalSize.x, io.DisplaySize.y), ImGuiCond_Always);

      cap.read(frame);
      cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);

      ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
      ImGui::Begin("View", NULL, window_flags);
      ImGui::imshow(frame, frame_texture_id);
      ImGui::End();
      ImGui::PopStyleVar();
    }

    {
      ImGui::OpenPopup("Delete?");

      // Always center this window when appearing
      ImVec2 center = ImGui::GetMainViewport()->GetCenter();
      ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

      if (ImGui::BeginPopupModal("Delete?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
      {
        ImGui::Text("All those beautiful files will be deleted.\nThis operation cannot be undone!\n\n");
        ImGui::Separator();

        //static int unused_i = 0;
        //ImGui::Combo("Combo", &unused_i, "Delete\0Delete harder\0");

        static bool dont_ask_me_next_time = false;
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
        ImGui::Checkbox("Don't ask me next time", &dont_ask_me_next_time);
        ImGui::PopStyleVar();

        if (ImGui::Button("OK", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
        ImGui::EndPopup();
      }
    }
    done = ImGui::update();
  }

  ImGui::deinit();
  return 0;
}
