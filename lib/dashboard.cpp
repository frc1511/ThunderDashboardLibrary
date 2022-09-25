#include <ThunderDashboard/dashboard.h>

#include <GLFW/glfw3.h>

using namespace frc1511;

ThunderDashboard::ThunderDashboard(std::string_view name) {
  if (!name.empty()) {
    win_title += " - ";
    win_title += name;
  }
}

void ThunderDashboard::init_window(GLFWwindow* win) {
  window = win;

  glfwSetWindowTitle(window, win_title.c_str());
}
