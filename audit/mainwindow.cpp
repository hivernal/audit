#include "audit/mainwindow.h"

#include <iostream>

namespace audit {

MainWindow::MainWindow(std::string_view name, int width, int height)
    : ImGuiGlfwVulkan{name, width, height, kConfigFlags} {
  io_->Fonts->AddFontFromFileTTF(
      "/home/nikita/documents/projects/c_c++/audit/imgui_glfw_vulkan/imgui/"
      "misc/fonts/Roboto-Medium.ttf",
      20.f, nullptr, io_->Fonts->GetGlyphRangesCyrillic());
  ImGui::StyleColorsLight();
}

void MainWindow::Render() {
  ImGuiViewport* viewport{ImGui::GetMainViewport()};
  ImGui::SetNextWindowPos(viewport->Pos);
  ImGui::SetNextWindowSize(viewport->Size);

  ImGui::Begin("Audit", nullptr, kMainWindowFlags);
  auto available{ImGui::GetContentRegionAvail()};
  ImGui::BeginChild("MainWindowLeftChild", {available.x * 0.25f, 0},
                    kMainWindowLeftChildFlags);
  DrawLeft();
  ImGui::EndChild();
  ImGui::SameLine();
  ImGui::BeginChild("MainWindowRightChild", {0, 0}, kMainWindowRightChildFlags);
  DrawRight();
  ImGui::EndChild();
  ImGui::End();
  // ImGui::ShowDemoWindow(nullptr);
}

void MainWindow::DrawLeft() {
  ImGui::SeparatorText("База данных");
  ImGui::BeginChild("Database");
  static int selected{-1};
  ImGuiTreeNodeFlags flags{ImGuiTreeNodeFlags_OpenOnArrow |
                           ImGuiTreeNodeFlags_OpenOnDoubleClick |
                           ImGuiTreeNodeFlags_SpanAvailWidth};
  if (ImGui::TreeNode("audit")) {
    ImGuiTreeNodeFlags node_flags{flags};
    if (selected == 1) node_flags |= ImGuiTreeNodeFlags_Selected;
    bool node_open{ImGui::TreeNodeEx("Хосты", node_flags)};
    if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
      if (selected != 1) std::cout << "hello\n";
      selected = 1;
    }
    if (node_open) {
      ImGui::BulletText("Blah blah\nBlah Blah");
      ImGui::TreePop();
    }

    node_flags = flags;
    if (selected == 2) node_flags |= ImGuiTreeNodeFlags_Selected;
    node_open = ImGui::TreeNodeEx("Пользователи", node_flags);
    if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) selected = 2;
    if (node_open) {
      ImGui::BulletText("Blah blah\nBlah Blah");
      ImGui::TreePop();
    }

    if (ImGui::TreeNode("Группы пользователей")) {
    }

    if (ImGui::TreeNode("Вход пользователей в систему")) {
      ImGui::TreePop();
    }

    if (ImGui::TreeNode("Создание процессов")) {
      ImGui::TreePop();
    }

    if (ImGui::TreeNode("Файлы")) {
      ImGui::TreePop();
    }

    if (ImGui::TreeNode("TCP-соединения")) {
      ImGui::TreePop();
    }

    if (ImGui::TreeNode("Завершение процессов")) {
      ImGui::TreePop();
    }

    ImGui::TreePop();
  }
  ImGui::EndChild();
}

void MainWindow::DrawRight() {
  ImGui::SetNextItemWidth(20.f);
  ImGui::SeparatorText("События");
  if (ImGui::BeginTable("split", 8, ImGuiTableFlags_Resizable | ImGuiTableFlags_Borders)) {
    ImGui::TableSetupColumn("Дата");
    ImGui::TableSetupColumn("ID хоста");
    ImGui::TableSetupColumn("UID");
    ImGui::TableSetupColumn("PID");
    ImGui::TableSetupColumn("PPID");
    ImGui::TableSetupColumn("Рабочий каталог");
    ImGui::TableSetupColumn("Команда");
    ImGui::TableSetupColumn("Аргументы");
    ImGui::TableHeadersRow();
    for (int i = 0; i < 96; i++) {
      ImGui::TableNextColumn();
      ImGui::Text(" ");
    }
    ImGui::EndTable();
  }
}

}  // namespace audit

/*
ImGuiID my_dockspace = ImGui::DockSpaceOverViewport(
    0, nullptr, ImGuiDockNodeFlags_PassthruCentralNode, nullptr);
ImGui::SetNextWindowDockID(my_dockspace, ImGuiCond_Always);
ImGui::SetNextWindowViewport(ImGui::GetMainViewport()->ID);
*/

  /*
  if (ImGui::BeginTabBar("Tabs", ImGuiTabBarFlags_Reorderable)) {
    static bool tab_execve_opened{false};
    auto tab_execve{ImGui::BeginTabItem("Execve")};
    if (!tab_execve_opened && ImGui::IsItemClicked()) std::cout << "execve\n";
    if (tab_execve) {
      tab_execve_opened = true;
      if (ImGui::BeginTable("split", 2, ImGuiTableFlags_Resizable)) {
        for (int i = 0; i < 100; i++) {
          char buf[32];
          sprintf(buf, "%03d", i);
          ImGui::TableNextColumn();
          ImGui::Button(buf, ImVec2(-FLT_MIN, 0.0f));
        }
        ImGui::EndTable();
      }
      ImGui::EndTabItem();
    } else {
      tab_execve_opened = false;
    }

    auto tab2{ImGui::BeginTabItem("Tab2")};
    if (tab2) {
      ImGui::EndTabItem();
    }

    auto tab3{ImGui::BeginTabItem("Tab3")};
    if (tab3) {
      ImGui::EndTabItem();
    }

    auto tab4{ImGui::BeginTabItem("Tab4")};
    if (tab4) {
      ImGui::EndTabItem();
    }

    ImGui::EndTabBar();
  }
  */
