#ifndef AUDIT_MAINWINDOW_H_
#define AUDIT_MAINWINDOW_H_

#include "imgui_glfw_vulkan/imgui_glfw_vulkan.h"

namespace audit {

class MainWindow : public imgui_glfw_vulkan::ImGuiGlfwVulkan {
 public:
  MainWindow(std::string_view name = "window", int width = 800,
             int height = 600);
  MainWindow(const MainWindow&) = default;
  MainWindow& operator=(const MainWindow&) = default;
  MainWindow(MainWindow&&) = default;
  MainWindow& operator=(MainWindow&&) = default;
  ~MainWindow() = default;
  void Render() override;

 private:
  void DrawLeft();
  void DrawRight();
  static constexpr ImGuiConfigFlags kConfigFlags{
      ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_NavEnableGamepad};
  static constexpr ImGuiWindowFlags kMainWindowFlags{
      ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
      ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse};
  static constexpr ImGuiChildFlags kMainWindowLeftChildFlags{
      ImGuiChildFlags_ResizeX | ImGuiChildFlags_Border};
  static constexpr ImGuiChildFlags kMainWindowRightChildFlags{
      ImGuiChildFlags_Border};
};

}  // namespace audit

#endif  // AUDIT_MAINWINDOW_H_
