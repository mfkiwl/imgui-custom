#include "imgui_user.h"

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include "imgui_internal.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl2.h"

#include <SDL.h>
#include <SDL_opengl.h>

namespace ImGui
{
  const int CENTER = 0x2FFF0000u;
  ImVec4 clear_color = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);

  namespace SDLGL2
  {
    SDL_GLContext gl_context;
    SDL_Window* window;

    bool Init(const char* title, int x, int y, int w, int h, bool full_screen)
    {
      // Setup SDL
  // (Some versions of SDL before <2.0.10 appears to have performance/stalling issues on a minority of Windows systems,
  // depending on whether SDL_INIT_GAMECONTROLLER is enabled or disabled.. updating to latest version of SDL is recommended!)
      if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
      {
        printf("SDL Error: %s\n", SDL_GetError());
        return false;
      }
      // Setup window
      SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
      SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
      SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
      SDL_WindowFlags sdl_window_flags = (SDL_WindowFlags)(full_screen == true ? (SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN) : (SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI));
      window = SDL_CreateWindow(title, x, y, w, h, sdl_window_flags);
      gl_context = SDL_GL_CreateContext(window);
      SDL_GL_MakeCurrent(window, gl_context);
      SDL_GL_SetSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
      ImGui::CreateContext();

      // Setup Platform/Renderer backends
      ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
      ImGui_ImplOpenGL2_Init();

      // Setup Dear ImGui style
      ImGui::StyleColorsDark();

      ImGui::GetIO().IniFilename = NULL;
      return window != NULL;
    }
    void Deinit()
    {
      // Cleanup
      ImGui_ImplOpenGL2_Shutdown();
      ImGui_ImplSDL2_Shutdown();
      ImGui::DestroyContext();


      SDL_GL_DeleteContext(gl_context);
      SDL_DestroyWindow(window);
      SDL_Quit();

    }

    void NewFrame()
    {
      // Start the Dear ImGui frame
      ImGui_ImplOpenGL2_NewFrame();
      ImGui_ImplSDL2_NewFrame();
      ImGui::NewFrame();
    }

    bool Update()
    {
      Render();

      // Poll and handle events (inputs, window resize, etc.)
      // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
      // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
      // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
      // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
      bool done;
      SDL_Event event;
      while (SDL_PollEvent(&event))
      {
        ImGui_ImplSDL2_ProcessEvent(&event);
        if (event.type == SDL_QUIT)
          done = true;
        if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
          done = true;
      }
      return done;
    }

    void Render()
    {
      ImGuiIO& io = ImGui::GetIO();
      // Rendering
      ImGui::Render();
      glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
      glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
      glClear(GL_COLOR_BUFFER_BIT);
      //glUseProgram(0); // You may want this if using this code in an OpenGL 3+ context where shaders may be bound
      ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
      SDL_GL_SwapWindow(window);
    }

    uiTextureID CreateTexture()
    {
      GLuint texture_id;
      glGenTextures(1, &texture_id);
      glBindTexture(GL_TEXTURE_2D, texture_id);

      // Setup filtering parameters for display
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same
      return (uiTextureID)texture_id;
    }
    void Image(cv::Mat bgr, uiTextureID texture_id, ImGuiImageDrawFlgs align, bool autosize)
    {
      cv::Mat img;
      cv::cvtColor(bgr, img, cv::COLOR_BGR2RGB);

      glBindTexture(GL_TEXTURE_2D, (GLuint)texture_id);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img.cols, img.rows, 0, GL_RGB, GL_UNSIGNED_BYTE, img.data);

      ImVec2 region = ImGui::GetContentRegionAvail();
      ImVec2 view(img.cols, img.rows);
      if (autosize)
      {
        float aspr_region = (float)region.x / region.y;
        float aspr_img = (float)img.cols / img.rows;
        float scale = 1.0;
        if (aspr_region > aspr_img)
          scale = (float)region.y / img.rows;
        else
          scale = (float)region.x / img.cols;
        view = ImVec2(img.cols * scale, img.rows * scale);
      }

      ImVec2 cur_pos = ImGui::GetCursorPos();
      cur_pos.x += (align & 1) / 1 * (region.x - view.x);
      cur_pos.y += (align & 2) / 2 * (region.y - view.y);

      cur_pos.x += (align & 4) / 8.0f * (region.x - view.x);
      cur_pos.y += (align & 8) / 16.0f * (region.y - view.y);

      ImGui::SetCursorPos(cur_pos);
      ImGui::Image((void*)(intptr_t)texture_id, view);
    }
  }

  void SetStyle(float window_border, ImVec2 window_padding, ImVec2 frame_padding, float item_spacing)
  {
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowBorderSize = window_border;
    style.WindowPadding = window_padding;
    style.FramePadding = frame_padding;
    style.ItemSpacing.y = item_spacing;
  }

  void RenderRightArrow(ImDrawList* draw_list, ImVec2 pos, ImU32 col, float scale)
  {
    const float h = draw_list->_Data->FontSize * 1.00f;
    float r = h * 0.40f * scale;
    ImVec2 center = pos + ImVec2(h * 0.50f, h * 0.50f * scale);
    ImVec2 a = ImVec2(+0.750f, +0.000f) * r;
    ImVec2 b = ImVec2(-0.750f, +0.866f) * r;
    ImVec2 c = ImVec2(-0.750f, -0.866f) * r;

    draw_list->AddTriangleFilled(center + a, center + b, center + c, col);
  }

  void RenderAdd(ImDrawList* draw_list, ImVec2 pos, ImU32 col, float scale)
  {
    const float h = draw_list->_Data->FontSize * 1.00f;
    float thickness = 3.0f;
    float r = h * 0.40f * scale;
    ImVec2 center = pos + ImVec2(h * 0.50f, h * 0.50f * scale);
    static float hsz = 0.866f * r;
    const float rounding = hsz * 2.0f / 5.0f;

    draw_list->AddRect(center - ImVec2(hsz, hsz), center + ImVec2(hsz, hsz), col, rounding, ImDrawFlags_None, thickness);
    draw_list->AddLine(center - ImVec2(hsz, 0.0f) * 0.6f, center + ImVec2(hsz, 0.0f) * 0.6f, col, thickness);
    draw_list->AddLine(center - ImVec2(0.0f, hsz) * 0.6f, center + ImVec2(0.0f, hsz) * 0.6f, col, thickness);
  }

  void RenderMagnifyingGlass(ImDrawList* draw_list, ImVec2 pos, ImU32 col, float scale)
  {
    const float h = draw_list->_Data->FontSize * 1.00f;
    float thickness = 3.0f;
    float r = h * 0.40f * scale;
    ImVec2 center = pos + ImVec2(h * 0.50f, h * 0.50f * scale);
    static float hsz = 0.866f * r;
    const float rounding = hsz * 2.0f / 5.0f;

    draw_list->AddCircle(center + ImVec2(hsz, -hsz) * 0.4f, hsz * 0.7f, col, 0, thickness);
    draw_list->AddLine(center + ImVec2(-hsz, hsz), center, col, thickness);
  }

  void CalcPathArc6(ImVector<ImVec2>& points, const ImVec2 center, float radius, float a_max, int num_segments)
  {
    points.clear();
    points.reserve(num_segments);
    for (int i = 0; i < num_segments; i++)
    {
      const float a = ((float)i / (float)(num_segments - 1)) * a_max;
      points.push_back(ImVec2(center.x + ImCos(a) * radius, center.y + ImSin(a) * radius));
    }
  }
  void RenderSetting(ImDrawList* draw_list, ImVec2 pos, ImU32 col, float scale)
  {
    const float h = draw_list->_Data->FontSize * 1.00f;
    float thickness = 3.0f;
    float r = h * 0.40f * scale;
    ImVec2 center = pos + ImVec2(h * 0.50f, h * 0.50f * scale);
    static float hsz = 0.866f * r;
    const float rounding = hsz * 2.0f / 5.0f;


    int num_segments = 6;
    const float a_max = (IM_PI * 2.0f) * ((float)num_segments - 1.0f) / (float)num_segments;
    ImVector<ImVec2> Lsegpoints, Ssegpoints;
    CalcPathArc6(Lsegpoints, center, hsz, a_max, num_segments);
    CalcPathArc6(Ssegpoints, center, hsz * 0.65, a_max, num_segments);

    ImVector<ImVec2> points;
    points.reserve(num_segments * 2);
    for (int i = 0; i < num_segments; i++)
    {
      ImVec2& Lpt1 = Lsegpoints[(i + 0) % num_segments];
      ImVec2& Lpt2 = Lsegpoints[(i + 1) % num_segments];
      ImVec2& Spt1 = Ssegpoints[(i + 0) % num_segments];
      ImVec2& Spt2 = Ssegpoints[(i + 1) % num_segments];
      points.push_back(ImVec2(Spt1.x + (Spt2.x - Spt1.x) * 0.1f, Spt1.y + (Spt2.y - Spt1.y) * 0.1f));
      points.push_back(ImVec2(Lpt1.x + (Lpt2.x - Lpt1.x) * 0.3f, Lpt1.y + (Lpt2.y - Lpt1.y) * 0.3f));
      points.push_back(ImVec2(Lpt1.x + (Lpt2.x - Lpt1.x) * 0.7f, Lpt1.y + (Lpt2.y - Lpt1.y) * 0.7f));
      points.push_back(ImVec2(Spt1.x + (Spt2.x - Spt1.x) * 0.9f, Spt1.y + (Spt2.y - Spt1.y) * 0.9f));
    }
    draw_list->AddPolyline(points.Data, points.Size, col, ImDrawFlags_Closed, thickness);
    draw_list->AddCircle(center, hsz * 0.3f, col, 0, thickness);
  }

  bool RadioIconButton(const char* str_id, int* active_id, int btn_id, int icon, float scale)
  {
    float sz = ImGui::GetFrameHeight() * scale;
    ImVec2 size(sz, sz);

    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
      return false;

    ImGuiContext& g = *GImGui;
    const ImGuiID id = window->GetID(str_id);
    const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + size);
    ImGui::ItemSize(size, g.Style.FramePadding.y);
    if (!ImGui::ItemAdd(bb, id))
      return false;

    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held);
    bool active = *active_id == btn_id;
    if (pressed)
    {
      if (!active) *active_id = btn_id;
      // else *active_id = -1;
    }

    if (held || hovered)
      ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

    // Render
    const ImU32 cr_bg = ImGuiCol_FrameBg;
    const ImU32 cr_btn = ImGui::GetColorU32((active || (held && hovered)) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
    ImGui::RenderNavHighlight(bb, id);
    ImGui::RenderFrame(bb.Min, bb.Max, cr_bg, true, g.Style.FrameRounding);

    switch (icon)
    {
    case ImGuiButtonIcon_RightArrow:
      RenderRightArrow(window->DrawList, bb.Min + ImVec2(ImMax(0.0f, (size.x - g.FontSize) * 0.5f), ImMax(0.0f, (size.y - g.FontSize) * 0.5f)), cr_btn, scale);
      break;
    case ImGuiButtonIcon_Add:
      RenderAdd(window->DrawList, bb.Min + ImVec2(ImMax(0.0f, (size.x - g.FontSize) * 0.5f), ImMax(0.0f, (size.y - g.FontSize) * 0.5f)), cr_btn, scale);
      break;
    case ImGuiButtonIcon_Mag:
      RenderMagnifyingGlass(window->DrawList, bb.Min + ImVec2(ImMax(0.0f, (size.x - g.FontSize) * 0.5f), ImMax(0.0f, (size.y - g.FontSize) * 0.5f)), cr_btn, scale);
      break;
    case ImGuiButtonIcon_Setting:
      RenderSetting(window->DrawList, bb.Min + ImVec2(ImMax(0.0f, (size.x - g.FontSize) * 0.5f), ImMax(0.0f, (size.y - g.FontSize) * 0.5f)), cr_btn, scale);
      break;
    default:
      break;
    }
    return pressed;
  }

  float CalcRadioIconButtonSize(float scale)
  {
    return ImGui::GetFrameHeight() * scale;
  }


  bool PopupMessageBox(const char* name, const char* message, ImGuiPopMessageBoxFlags flags)
  {
    const ImGuiID id = ImGui::GetID(name);
    static ImVector<ImGuiID> dont_ask_me_ids;
    bool find = dont_ask_me_ids.find(id) != dont_ask_me_ids.end();
    if (find) return true;

    // Always center this window when appearing
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopupModal(name, NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
      ImGui::Text(message);

      if (flags > 0)  ImGui::Separator();

      static bool dont_ask_me_next_time = find;
      if (flags & ImGuiPopMessageBoxFlags_DONT_ASK_ME)
      {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
        ImGui::Checkbox("Don't ask me next time", &dont_ask_me_next_time);
        ImGui::PopStyleVar();
      }

      if (flags & ImGuiPopMessageBoxFlags_OK)
      {
        if (ImGui::Button("OK", ImVec2(120, 0)))
        {
          ImGui::CloseCurrentPopup();
          if (dont_ask_me_next_time)   dont_ask_me_ids.push_back(id);
          ImGui::EndPopup();
          return true;
        }
        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
      }
      if (flags & ImGuiPopMessageBoxFlags_CANCEL)
      {
        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
          ImGui::CloseCurrentPopup();
          ImGui::EndPopup();
          dont_ask_me_next_time = false;
          return false;
        }
      }
      ImGui::EndPopup();
    }
    return false;
  }
}
