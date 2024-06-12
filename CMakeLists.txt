cmake_minimum_required(VERSION 3.26)
project(audit_gui CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(INCLUDE
  ${CMAKE_CURRENT_SOURCE_DIR}
)

set(SRC
  audit_gui/main.cpp
  audit_gui/mainwindow.cpp
  )

add_subdirectory(imgui_glfw_vulkan)
add_executable(audit_gui ${SRC})
target_link_libraries(audit_gui PRIVATE imgui_glfw_vulkan)
target_include_directories(audit_gui PRIVATE ${INCLUDE})
target_compile_options(audit_gui
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
