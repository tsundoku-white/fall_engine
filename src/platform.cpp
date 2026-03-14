#include "platform.h"
#include "glm/ext/vector_float2.hpp"
#include <GLFW/glfw3.h>
#include <cmath>

namespace helix {

  Platform::Platform(void)
  {
    if (!glfwInit())
    {
      std::printf("[FAIL] glfw init\n");
      return;
    }
    m_monitor = glfwGetPrimaryMonitor();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    m_handle = glfwCreateWindow(m_current.width, m_current.height, m_title, nullptr, nullptr);
    if (!m_handle)
    {
      std::print("[FAIL] create window\n");
      return;
    }

    const GLFWvidmode* mode = glfwGetVideoMode(m_monitor);
    m_windowed.x = (mode->width  - m_current.width)  / 2;
    m_windowed.y = (mode->height - m_current.height) / 2;  // was mode->width (bug)
    glfwSetWindowPos(m_handle, m_windowed.x, m_windowed.y);

    std::print("[0] Platform has been successfully created\n");
  }

  Platform::~Platform(void)
  {
    glfwDestroyWindow(m_handle);
    glfwTerminate();
  }

  // anilize later how it works and why it works idk why?
  void Platform::set_window_mode(WindowMode new_window_mode)
  {
    if (m_window_mode == new_window_mode) return;

    const GLFWvidmode* mode = glfwGetVideoMode(m_monitor);

    switch (new_window_mode)
    {
      case WindowModeFullscreen:
        glfwSetWindowMonitor(m_handle, m_monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
        break;

      case WindowModeBorderless:
        glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
        glfwSetWindowMonitor(m_handle, nullptr, 0, 0, mode->width, mode->height, 0);
        break;

      case WindowModeWindowed:
        glfwSetWindowMonitor(m_handle, nullptr, m_windowed.x, m_windowed.y, m_windowed.width, m_windowed.height, 0);
        break;
    }

    m_window_mode = new_window_mode;
  }
  bool Platform::should_close(void) { return glfwWindowShouldClose(m_handle); }
  void Platform::pollevents(void)
  {
    double now  = glfwGetTime();
    delta       = now - m_last_time;
    m_last_time = now;
    glfwPollEvents();
  }
  GLFWwindow *Platform::get_handle(void) { return m_handle; }

  bool Platform::input_pressed(int key)
  {
    return glfwGetKey(m_handle, key) == GLFW_PRESS;
  }

  bool Platform::input_released(int key)
  {
    return glfwGetKey(m_handle, key) == GLFW_RELEASE;
  }

  float Platform::input_vector2(int key_a, int key_b, double dt)
  {
    float target = 0.0f;
    if (glfwGetKey(m_handle, key_a) == GLFW_PRESS) target -= 1.0f;
    if (glfwGetKey(m_handle, key_b) == GLFW_PRESS) target += 1.0f;

    float speed = 10.0f; // higher = snappier
    m_axis_value = std::lerp(m_axis_value, target, speed * (float)dt);

    if (std::abs(m_axis_value - target) < 0.01f)
    m_axis_value = target;

    return m_axis_value;
  }

   glm::vec2 Platform::input_mouse_dt()
  {
        if (glfwRawMouseMotionSupported())
      glfwSetInputMode(m_handle, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
 
    double cx, cy;
    glfwGetCursorPos(m_handle, &cx, &cy);
 
    glm::vec2 dt = { 0.0f, 0.0f };
    if (!m_mouse_first)
    {
      dt.x = static_cast<float>(cx - m_mouse_last_x);
      dt.y = static_cast<float>(cy - m_mouse_last_y);
    }
    m_mouse_first  = false;
    m_mouse_last_x = cx;
    m_mouse_last_y = cy;
 
    return dt;
  }

  glm::vec2 Platform::input_mouse_location()
  {
    double x, y;
    glfwGetCursorPos(m_handle, &x, &y);
    return { static_cast<float>(x), static_cast<float>(y) };
  }

}

