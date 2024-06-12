cmake_minimum_required(VERSION 3.26)
project(imgui_glfw_vulkan CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(
  INCLUDE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/imgui
  ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends
)

set(
  SRC
  ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_demo.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_draw.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_tables.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_widgets.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_glfw.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_vulkan.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/imgui_glfw_vulkan/imgui_glfw_vulkan.cpp
  )

add_subdirectory(glfw)
find_package(Vulkan REQUIRED)

add_library(imgui_glfw_vulkan ${SRC})
target_include_directories(imgui_glfw_vulkan PUBLIC ${INCLUDE})
# target_include_directories(imgui_glfw_opengl PRIVATE ${INCLUDE})
# target_include_directories(imgui_glfw_vulkan SYSTEM PUBLIC ${INCLUDE})
target_link_libraries(imgui_glfw_vulkan PUBLIC glfw PUBLIC Vulkan::Vulkan)
target_compile_options(imgui_glfw_vulkan
  PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Werror
    -Weffc++
    -Wconversion
    -Wsign-conversion
    -Wnon-virtual-dtor
    -Wold-style-cast
    -Wsign-promo
    -Wduplicated-branches
    -Wduplicated-cond
    -Wzero-as-null-pointer-constant
    -Wlogical-op
)
