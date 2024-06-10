cmake_minimum_required(VERSION 3.26)
project(audit_gui CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(
  INCLUDE
  ${CMAKE_CURRENT_SOURCE_DIR}
  imgui
  imgui/backends
)

set(LIB glfw Vulkan::Vulkan)

set(
  SRC
  imgui/imgui.cpp
  imgui/imgui_demo.cpp
  imgui/imgui_draw.cpp
  imgui/imgui_tables.cpp
  imgui/imgui_widgets.cpp
  imgui/backends/imgui_impl_glfw.cpp
  imgui/backends/imgui_impl_vulkan.cpp
  audit_gui/main.cpp
  )

add_subdirectory(glfw)
find_package(Vulkan REQUIRED)

add_executable(audit_gui ${SRC})
# add_library(audit_gui ${SRC})
target_include_directories(audit_gui PUBLIC ${INCLUDE})
target_link_libraries(audit_gui PUBLIC ${LIB})
