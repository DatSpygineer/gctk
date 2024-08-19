cmake_minimum_required(VERSION 3.25)

add_subdirectory(GTexTools)

set(IMGUI_BACKEND "GL3")
set(IMGUI_BACKEND_WINDOW "GLFW")
add_subdirectory(thirdparty/imgui imgui)