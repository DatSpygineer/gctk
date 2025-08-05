cmake_minimum_required(VERSION 3.30)

project(gctk_client CXX)
project(gctk_server CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_POSITION_INDEPENDENT_CODE 1)

set(GCTK_ROOT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/gctk)

add_subdirectory(gctk)
add_subdirectory(example)
add_subdirectory(submodules)
add_subdirectory(tools)