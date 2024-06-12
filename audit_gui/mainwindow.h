#ifndef AUDIT_GUI_MAINWINDOW_H_
#define AUDIT_GUI_MAINWINDOW_H_

#include "imgui_glfw_vulkan/imgui_glfw_vulkan.h"

class MainWindow : public ImGuiGlfwVulkan {
 public:
  MainWindow() = default;
  MainWindow(const MainWindow&) = default;
  MainWindow& operator=(const MainWindow&) = default;
  MainWindow(MainWindow&&) = default;
  MainWindow& operator=(MainWindow&&) = default;
  ~MainWindow() = default;
  void Render() override;
};

#endif  // AUDIT_GUI_MAINWINDOW_H_
