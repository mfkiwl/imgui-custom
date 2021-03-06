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
  int initCamWidth = 640;
  int initCamHeight = 480;
  const int initCamFps = 30;

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

  ImGui::GLFWGL2::Init("GUI", 1440, 1620);

  ImGuiIO& io = ImGui::GetIO();
  io.Fonts->AddFontFromFileTTF("fonts/NanumBarunGothic.ttf", 32.0f, NULL, io.Fonts->GetGlyphRangesKorean());

  // Our state
  uiTextureID texture_id; ImGui::GL2::GenTextures(1, &texture_id);
  uiTextureID texture_ids[10]; ImGui::GL2::GenTextures(10, texture_ids);

  // Main loop
  cv::Mat frame;
  bool done = false;
  ImGuiStyle& style = ImGui::GetStyle();
  ImGui::SetStyle(0.0f, ImVec2(0.0f, 0.0f), ImVec2(8.0f, 8.0f), 10.0f);

  const char* menus[] = { "Live", "Face Register", "Registration Status", "Setting" };
  char name[15] = "";
  char work[15] = "";
  bool bBtCapture = false, bBtRecapture = false, bBtManulRegist = false;
  while (!done)
  {
    static int active_menu = cap.isOpened() ? 1 : 3;

    ImGui::GLFWGL2::NewFrame();

    ImGuiWindowFlags window_flags =
      ImGuiWindowFlags_NoSavedSettings |
      ImGuiWindowFlags_NoMove |
      ImGuiWindowFlags_NoNav |
      ImGuiWindowFlags_NoDecoration |
      ImGuiWindowFlags_AlwaysUseWindowPadding;

    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.15f, 0.15f, 0.15f, 1.00f));

    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(io.DisplaySize, ImGuiCond_Always);
    ImGui::Begin("MainWnd", NULL, window_flags);
    {
      ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 10.0f));
      ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.2f, 0.2f, 0.2f, 1.00f));
      ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
      ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
      ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

      float btn_scale = 2.0f;
      ImGui::BeginChild("##Menu", ImVec2(ImGui::CalcRadioIconButtonSize(btn_scale), 0.0f), false, window_flags);
      ImGui::RadioIconButton("##MENU LIVE", &active_menu, 0, ImGuiButtonIcon_RightArrow, btn_scale);
      if (ImGui::IsItemHovered()) ImGui::SetTooltip(menus[0]);

      ImGui::RadioIconButton("##MENU REGIST", &active_menu, 1, ImGuiButtonIcon_Add, btn_scale);
      if (ImGui::IsItemHovered()) ImGui::SetTooltip(menus[1]);

      ImGui::RadioIconButton("##MENU STATUS", &active_menu, 2, ImGuiButtonIcon_Mag, btn_scale);
      if (ImGui::IsItemHovered()) ImGui::SetTooltip(menus[2]);

      ImGui::RadioIconButton("##MENU REFRESH_MENU", &active_menu, 4, ImGuiButtonIcon_RightArrow, btn_scale);


      if (ImGui::Button("-")) ImGui::OpenPopup("Delete?");
      ImGui::PopupMessageBox("Delete?", "All those beautiful files will be deleted.\nThis operation cannot be undone!\n\n", ImGuiPopMessageBoxFlags_OK_CANCEL | ImGuiPopMessageBoxFlags_DONT_ASK_ME);

      ImGui::SetCursorPosY(io.DisplaySize.y - ImGui::CalcRadioIconButtonSize(btn_scale) - style.ItemSpacing.y - style.FramePadding.y);
      ImGui::RadioIconButton("##MENU SETTING", &active_menu, 3, ImGuiButtonIcon_Setting, btn_scale);
      if (ImGui::IsItemHovered()) ImGui::SetTooltip(menus[3]);
      ImGui::EndChild();

      ImGui::PopStyleColor(4);
      ImGui::PopStyleVar();
      ImGui::SameLine(0.0f, 0.0f);
    }

    if (active_menu == 0)
    {
      cap.read(frame);
      ImVec2 viewpos = ImGui::GetCursorPos();

      ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.12f, 0.12f, 0.12f, 1.00f));
      ImGui::BeginChild("##View", ImVec2(0, 0), false, window_flags);
      ImGui::GL2::Image(frame, texture_id, ImGui::GetContentRegionAvail(), ImGuiImageDrawFlgs_Center, true);
      ImVec2 pos = ImGui::GetItemRectMin(), size = ImGui::GetItemRectSize();
      ImGui::SetWindowFontScale(2.0f);
      ImGui::SetCursorScreenPos(ImVec2(pos.x + size.x - ImGui::CalcTextSize("LIVE").x - 32.0f, pos.y + 32.0f));
      ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "LIVE");
      ImGui::GetWindowDrawList()->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(255, 0, 0, 255));

      float showFrameScale = size.x / frame.cols;
      int sz = frame.cols / 10 * showFrameScale;
      int x = pos.x + frame.cols / 2 * showFrameScale, y = pos.y + frame.rows / 2 * showFrameScale;
      //float showFrameScale = 1.0f;
      //int x=pos.z/2, y=pos.w/2;
      //int x = p.x + pos.z / 2, y = p.y + pos.w / 2;
      std::vector<ImVec2> pts =
      {
        ImVec2(x + sz, y), ImVec2(x, y), ImVec2(x, y + sz),
      };
      ImGui::GetWindowDrawList()->AddPolyline(pts.data(), pts.size(), IM_COL32(255, 0, 0, 255), ImDrawFlags_None, 5);
      ImGui::EndChild();
      ImGui::PopStyleColor(1);

      if (0)
      {
        ImGui::SetCursorPos(viewpos);

        ImGui::SetNextWindowBgAlpha(0.5f); // Transparent background
        ImGui::BeginChild("##Overlay Text", ImVec2(0, 0), false, window_flags);
        ImGui::SetWindowFontScale(2.0f);
        ImGui::Text("??????(Confirm)");
        ImGui::EndChild();
      }
    }
    else if (active_menu == 4)
    {

    }
    else
    {
      ImGui::BeginGroup();
      {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16.0f, 16.0f));
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.12f, 0.12f, 0.12f, 1.00f));
        ImGui::BeginChild("##TITLE", ImVec2(0, ImGui::GetFrameHeightWithSpacing()), false, window_flags);
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), menus[active_menu]);
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

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(padding, padding));
        cap.read(frame);
        {
          ImGui::BeginChild("##REGIST VIEW", ImVec2(view_region, 0), false, window_flags);
          const char* items[] = { "Cam1", "Cam2" };
          static int current_item = 0;
          ImGui::SetWindowFontScale(1.0f);
          ImGui::SetNextItemWidth(ImGui::CalcTextSize("Cam10").x + ImGui::GetFrameHeightWithSpacing());
          ImGui::Combo("##Cam", &current_item, items, IM_ARRAYSIZE(items));

          region = ImGui::GetContentRegionAvail();
          ImGui::GL2::Image(frame, texture_id, ImVec2(region.x, region.y - ImGui::GetFrameHeightWithSpacing()), ImGuiImageDrawFlgs_Center, true);
          ImVec2 pos = ImGui::GetItemRectMin(), size = ImGui::GetItemRectSize();
          ImGui::SetWindowFontScale(2.0f);
          ImGui::SetCursorScreenPos(ImVec2(pos.x + size.x - ImGui::CalcTextSize("LIVE").x - 32.0f, pos.y + 32.0f));
          ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "LIVE");
          ImGui::GetWindowDrawList()->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(255, 0, 0, 255));

          ImGui::SetWindowFontScale(1.0f);
          ImGui::SetCursorPosY(ImGui::GetWindowHeight() - ImGui::GetFrameHeightWithSpacing() - padding);
          ImVec2 btn_size(ImGui::CalcTextSize("A").x * 5, 0);
          ImGui::Button("??????", btn_size);
          ImGui::EndChild();
        }

        ImGui::SameLine();

        cv::Mat img = cv::imread("0.jpg");
        ImGui::BeginChild("##REGIST INFO", ImVec2(info_region, 0), false, window_flags);
        ImVec2 wpos = ImGui::GetWindowPos();
        ImGui::SetWindowFontScale(1.0f);


        ImGuiTabBarFlags tab_bar_flags =
          ImGuiTabBarFlags_AutoSelectNewTabs |
          ImGuiTabBarFlags_Reorderable |
          ImGuiTabBarFlags_FittingPolicyResizeDown;
        static int newidx = -1;
        if (ImGui::BeginTabBar("##FACES", tab_bar_flags))
        {
          for (int p = 0; p < 3; p++)
          {
            bool open = true;
            if (ImGui::BeginTabItem(cv::format("Face%d", p).c_str(), &open, ImGuiTabItemFlags_None))
            {
              if (newidx == p)
              {
                ImGui::TabItemReorder(2, -1);
                newidx = -1;
              }
              ImGui::GL2::Image(img, texture_ids[0], ImGui::GetContentRegionAvail(), ImGuiImageDrawFlgs_XCenter, true);
              ImGui::EndTabItem();
            }

            if (!open)
            {
              newidx = p;
            }
          }
          ImGui::EndTabBar();
        }

        ImGui::SetWindowFontScale(1.0f);
        ImGui::SetCursorPosY(ImGui::GetWindowWidth() + ImGui::GetFrameHeightWithSpacing() + padding);
        ImGui::Text("??????: ");  ImGui::SameLine();
        ImGui::PushItemWidth(ImGui::CalcTextSize("A").x * 13);
        ImGui::InputText("##NAME", name, 13);
        ImGui::Text("??????: ");  ImGui::SameLine();
        ImGui::InputText("##WORK", work, 13);
        ImGui::PopItemWidth();

        ImGui::SetCursorPosY(ImGui::GetWindowHeight() - ImGui::GetFrameHeightWithSpacing() - padding);
        ImVec2 btn_size(ImGui::CalcTextSize("A").x * 5, 0);
        bBtManulRegist = ImGui::Button("?????? ", btn_size);
        ImGui::EndChild();

        ImGui::PopStyleVar();
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

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(padding, padding));
        ImGui::BeginChild("##STATUS CHILD", ImVec2(icon_region, 0), false, window_flags & ~ImGuiWindowFlags_NoScrollbar);

        if (0)
        {
          ImVec2 pos = ImGui::GetWindowPos();
          ImVec2 size = ImGui::GetWindowSize();
          ImGui::GetForegroundDrawList()->AddRect(ImVec2(pos.x + padding, pos.y + padding), ImVec2(pos.x + size.x - padding, pos.y + size.y - padding), IM_COL32(255, 0, 0, 255));
        }
        ImGuiTableFlags table_flags =
          ImGuiTableFlags_SizingFixedFit |
          //  ImGuiTableFlags_SizingFixedSame;
          //  ImGuiTableFlags_Borders |
          ImGuiTableFlags_SizingFixedSame;

        static int clicked_id = 0;
        if (ImGui::BeginTable("##STATUS", column, table_flags, ImVec2(icon_region - padding * 2, 0.0f)))
        {
          ImGui::SetWindowFontScale(0.7f);
          for (int r = 0; r < 1; r++)
          {
            ImGui::TableNextRow();
            for (int c = 0; c < column; c++)
            {
              cv::Mat img = cv::imread(cv::format("%d.jpg", (r * column + c) % 5));
              ImGui::TableNextColumn();
              ImGui::GL2::Image(img, texture_ids[r * column + c], ImGui::GetContentRegionAvail(), ImGuiImageDrawFlgs_XCenter);
              float indent = ImGui::GetItemRectMin().x - ImGui::GetCursorScreenPos().x;
              ImGui::Indent(indent);
              ImGui::Text("??????: ?????????");
              int id = r * column + c;
              ImGui::PushID(id);
              if (ImGui::Button("????????????", ImVec2(icon_size, 0)))
                clicked_id = id;
              ImGui::PopID();
              ImGui::Unindent(indent);
            }
            ImGui::NewLine();
          }
          ImGui::EndTable();
        }
        ImGui::EndChild();

        ImGui::SameLine();
        ImGui::SeparatorWithSpacing(ImGuiSeparatorDirs_Vertical);
        ImGui::SameLine();
        ImGui::BeginChild("##INFO CHILD", ImVec2(info_region, 0), false, window_flags);
        ImGui::SetWindowFontScale(1.0f);
        cv::Mat img = cv::imread(cv::format("%d.jpg", clicked_id % 5));
        ImGui::GL2::Image(img, texture_id, ImGui::GetContentRegionAvail(), ImGuiImageDrawFlgs_XCenter, true);
        ImGui::Text("ID: %d", clicked_id);
        ImGui::SeparatorWithSpacing(ImGuiSeparatorDirs_Horizontal);
        ImGui::Text("??????: ?????????");
        ImGui::SeparatorWithSpacing(ImGuiSeparatorDirs_Horizontal);
        ImGui::Text("?????????: ?????????");
        ImGui::SeparatorWithSpacing(ImGuiSeparatorDirs_Horizontal);
        ImGui::EndChild();

        ImGui::PopStyleVar();
      }
      break;
      case 3:
      {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.0f, 8.0f));
        ImGui::BeginChild("##CHILD", ImVec2(0, 0), false, window_flags);
        ImGui::AlignTextToFramePadding();
        ImGui::Text("??????:");
        ImGui::SameLine(0.0f, 10.0f);

        const char* items[] = { "Cam1", "Cam2", "Cam3", "Cam4", "Cam5", "Cam6" };
        static int current_item = 0;
        ImGui::SetNextItemWidth(ImGui::CalcTextSize("Cam1    ").x + ImGui::GetFrameHeight());
        if (ImGui::Combo("##Cam", &current_item, items, IM_ARRAYSIZE(items)))
        {
          cap.open(cv::format("/dev/video%d", current_item));
          if (!cap.isOpened())
          {
            printf("Error: cam cannot open!\n");
            return -1;
          }
          cap.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));
          cap.set(cv::CAP_PROP_FRAME_WIDTH, initCamWidth);
          cap.set(cv::CAP_PROP_FRAME_HEIGHT, initCamHeight);
          cap.set(cv::CAP_PROP_FPS, initCamFps);
        }

        ImGui::EndChild();
        ImGui::PopStyleVar();
      }
      break;
      default:
        break;
      }
      ImGui::EndGroup();
    }

    ImGui::End();
    ImGui::PopStyleColor(1);

    done = ImGui::GLFWGL2::Update();
  }

  ImGui::GLFWGL2::Deinit();
  return 0;
}
