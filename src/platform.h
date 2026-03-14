#pragma once

#include <cmath>
#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <cstdint>
#include <glm/vec2.hpp>
#include <print>

namespace helix {

  enum WindowMode {
    WindowModeFullscreen,
    WindowModeBorderless,
    WindowModeWindowed
  };

  class Platform 
  {
    public:
      Platform(void);
      ~Platform(void);

      double delta = 0;

      void set_window_mode(WindowMode new_window_mode);
      bool should_close(void);
      void pollevents(void);

      GLFWwindow *get_handle(void);

      bool input_pressed(int key);
      bool input_released(int key);
      float input_vector2(int key_a, int key_b, double dt);
      glm::vec2 input_mouse_dt();
      glm::vec2 input_mouse_location();

    private:
      struct WindowState {
        uint32_t width  = 600;
        uint32_t height = 450;
        int x           = 100;
        int y           = 100;
      };

      WindowState m_windowed   = { 500, 500, 100, 100 };
      WindowState m_current    = { 600, 450, 0, 0 };

      const char *m_title      = "Helix";
      WindowMode m_window_mode = WindowModeWindowed;

      GLFWwindow *m_handle     = nullptr;
      GLFWmonitor *m_monitor   = nullptr;

      double m_last_time = 0;
      float m_axis_value = 0.0f;
      double m_mouse_last_x = 0.0;
      double m_mouse_last_y = 0.0;
      bool   m_mouse_first  = true;
  };
}
