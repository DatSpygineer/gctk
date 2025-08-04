cmake_minimum_required(VERSION 3.30)

set(GCTK_EXAMPLE_GAME_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/game)

file(GLOB GCTK_CLIENT_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/client/*.cpp)
file(GLOB GCTK_SERVER_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/server/*.cpp)
file(GLOB GCTK_SHARED_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/shared/*.cpp)

set(GCTK_LAUNCHER_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/launcher/main.cpp)
if (WIN32)
    set(GCTK_LAUNCHER_SOURCES ${GCTK_LAUNCHER_SOURCES} ${CMAKE_CURRENT_LIST_DIR}/src/launcher/game.rc)
endif()

gctk_project(gctk_example)

add_gctk_client(gctk_example ${GCTK_EXAMPLE_GAME_DIRECTORY} ${GCTK_CLIENT_SOURCES} ${GCTK_SHARED_SOURCES})
add_gctk_server(gctk_example ${GCTK_EXAMPLE_GAME_DIRECTORY} ${GCTK_SERVER_SOURCES} ${GCTK_SHARED_SOURCES})
add_gctk_launcher_singleplayer(gctk_example ${GCTK_EXAMPLE_GAME_DIRECTORY} ${GCTK_LAUNCHER_SOURCES})