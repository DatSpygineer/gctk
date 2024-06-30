cmake_minimum_required(VERSION 3.25)

project(GTexTools)

set(CMAKE_CXX_STANDARD 20)

set(IMGUI_BACKEND "GL3")
set(IMGUI_BACKEND_WINDOW "GLFW")

add_subdirectory(../thirdparty/imgui imgui)

add_executable(GTexTools main.cpp)
target_link_libraries(GTexTools PRIVATE gctk_static tinyfiledialogs imgui)
