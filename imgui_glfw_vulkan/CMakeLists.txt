cmake_minimum_required(VERSION 3.26)
project(imgui_glfw_vulkan CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(INCLUDE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/imgui
  ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends
)

set(IMGUI_SRC
  imgui/imgui.cpp
  imgui/imgui_demo.cpp
  imgui/imgui_draw.cpp
  imgui/imgui_tables.cpp
  imgui/imgui_widgets.cpp
  imgui/backends/imgui_impl_glfw.cpp
  imgui/backends/imgui_impl_vulkan.cpp
)

set(SRC
  ${CMAKE_CURRENT_SOURCE_DIR}/imgui_glfw_vulkan/imgui_glfw_vulkan.cpp
  )

add_subdirectory(glfw)
find_package(Vulkan REQUIRED)
add_library(imgui ${IMGUI_SRC})
target_include_directories(imgui PRIVATE ${INCLUDE})
target_include_directories(imgui SYSTEM INTERFACE ${INCLUDE})
target_link_libraries(imgui PUBLIC glfw PUBLIC Vulkan::Vulkan)

add_library(imgui_glfw_vulkan ${SRC})
target_link_libraries(imgui_glfw_vulkan PUBLIC imgui)
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
