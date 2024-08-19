cmake_minimum_required(VERSION 3.25)

project(GTexTools)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED 1)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

gctk_add_game_static(GTexTools "GTexTools" "Gctk" 1 0 0 GCTK_VERSION_RELEASE main.cpp ui.cpp textureloader.cpp)
target_link_libraries(GTexTools PRIVATE imgui Qt6::Widgets)
target_include_directories(GTexTools PRIVATE ${Qt6Widgets_INCLUDE_DIRS})