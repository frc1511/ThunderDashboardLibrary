#pragma once

#include <cmath>

// Windows missed this one D:
#ifndef M_PI
#define M_PI 3.14159265359f
#endif

// Windows also missed this one D:
#ifndef M_PI_2
#define M_PI_2 1.57079632679f
#endif

#define DEG_2_RAD (M_PI / 180.0f)
#define RAD_2_DEG (180.0f / M_PI)

#include <string>
#include <string_view>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <optional>
#include <vector>
#include <limits>
#include <utility>
#include <filesystem>
#include <numeric>
#include <variant>
#include <chrono>
#include <memory>
#include <map>
#include <sstream>
#include <thread>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <fmt/core.h>

struct GLFWwindow;

#ifdef THUNDER_WINDOWS
#include <Windows.h>
extern int WinMain(HINSTANCE, HINSTANCE, LPSTR, int);
#else
extern int main(int, char**);
#endif

namespace frc1511 {

class ThunderDashboard {
public:
  virtual ~ThunderDashboard() = default;

  constexpr bool is_running() const { return running; }

  virtual void init() = 0;
  virtual void present() = 0;
  virtual void close() = 0;

  virtual void handle_keyboard(int key, int scancode, int action, int mods) = 0;

  inline GLFWwindow* get_window() { return window; }

  virtual void data_clear() = 0;
  virtual bool data_should_open(const char* name) = 0;
  virtual void data_read_line(const char* line) = 0;
  virtual void data_apply(const char* type_name) = 0;
  virtual void data_write(const char* type_name, ImGuiTextBuffer* buffer) = 0;

  virtual const char* get_data_name() = 0;

protected:
  ThunderDashboard(std::string_view name);

  bool running = true;

private:
  void init_window(GLFWwindow* win);

  GLFWwindow* window;
  std::string win_title = "1511 Dashboard";

#ifdef THUNDER_WINDOWS
  friend int ::WinMain(HINSTANCE, HINSTANCE, LPSTR, int);
#else
  friend int ::main(int, char**);
#endif
};

// Implemented by client.
ThunderDashboard* get_dashboard();

} // namespace frc1511
