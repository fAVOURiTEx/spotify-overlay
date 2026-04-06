#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <OpenGL/gl3.h>
#include <iostream>
#include <string>

#include "animation.hpp"
#include "spotify_track.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

int main() {

  const float MAX_TEXT_WIDTH = 350.f;
  const double DELAY_CHECK_TRACK = 2;
  std::string trackName = "";
  double last_check_time = -1;

  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW\n";
    return 1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
  glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
  glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  GLFWwindow *window =
      glfwCreateWindow(800, 600, "Spotify Overlay Practice", nullptr, nullptr);
  if (!window) {
    std::cerr << "Failed to create GLFW window\n";
    glfwTerminate();
    return 1;
  }

#ifdef GLFW_MOUSE_PASSTHROUGH
  glfwSetWindowAttrib(window, GLFW_MOUSE_PASSTHROUGH, GLFW_TRUE);
#endif

  GLFWmonitor *monitor = glfwGetPrimaryMonitor();
  int wx, wy, ww, wh;
  glfwGetMonitorWorkarea(monitor, &wx, &wy, &ww, &wh);

  int overlay_w = 400;
  int overlay_h = 150;

  bool show_banner = false;
  double banner_start_time = 0.0;
  std::string banner_text = "";
  float alpha = 0.f;
  float slide = 0.f;

  // 🔥 Инициализируем размером по умолчанию (НЕ 0)
  float window_w = 400.f;
  float window_h = 150.f;

  int pos_x = wx + ww - static_cast<int>(window_w) - 20;
  int pos_y = wy + wh - static_cast<int>(window_h) - 20;

  glfwSetWindowSize(window, overlay_w, overlay_h);
  glfwSetWindowPos(window, pos_x, pos_y);

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  ImGuiStyle &style = ImGui::GetStyle();

  io.Fonts->AddFontFromFileTTF("/System/Library/Fonts/Helvetica.ttc", 16.0f,
                               nullptr, io.Fonts->GetGlyphRangesCyrillic());

  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 150");

  const float padding_x = 40.f;
  const float padding_y = 20.f;

  // ========== Главный цикл ==========
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    double now = glfwGetTime();

    if (last_check_time + DELAY_CHECK_TRACK < now) {
      last_check_time = now;
      std::string new_track = get_spotify_track();
      if (!new_track.empty() && new_track != trackName) {
        trackName = new_track;
        show_banner = true;
        banner_start_time = now;
        // Размер вычислим ПОСЛЕ NewFrame()
      }
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // 🔥 Вычисляем размер ПОСЛЕ NewFrame()
    if (show_banner && alpha < 0.01f) { // Только в начале анимации
      ImVec2 text_size = ImGui::CalcTextSize(trackName.c_str(), nullptr, false,
                                             MAX_TEXT_WIDTH);
      window_w = std::min(text_size.x + padding_x * 2.f,
                          MAX_TEXT_WIDTH + padding_x * 2.f);
      window_h = text_size.y + padding_y * 2.f;
      glfwSetWindowSize(window, static_cast<int>(window_w),
                        static_cast<int>(window_h));
    }

    float t = static_cast<float>((now - banner_start_time));

    alpha = calc_alpha(t);
    slide = calc_slide(t);

    if (show_banner && alpha > 0.01f) {
      ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);

      // 🔥 Слайд по Y со знаком + (выезд снизу)
      int pos_x = wx + ww - static_cast<int>(window_w) - 20;
      int pos_y =
          wy + wh - static_cast<int>(window_h) - 20 + static_cast<int>(slide);
      glfwSetWindowPos(window, pos_x, pos_y);

      if (t > 0.2f + 3.3f + 0.5f + 0.1f) {
        show_banner = false;
        glfwHideWindow(window);
      }

      ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 1.f, alpha));
      ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.f, 0.f, 0.f, alpha));
      ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.f, 0.f, 0.f, alpha));

      ImGui::Begin("##", nullptr,
                   ImGuiWindowFlags_NoDecoration |
                       ImGuiWindowFlags_AlwaysAutoResize |
                       ImGuiWindowFlags_NoMove);

      ImGui::Text("%s", trackName.c_str());
      ImGui::PopStyleColor(3);
      ImGui::End();
      ImGui::PopStyleVar();
    }

    ImGui::Render();

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
