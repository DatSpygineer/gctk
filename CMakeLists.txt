cmake_minimum_required(VERSION 3.25)

project(gctk_test C)
project(gctk_static C)
project(gctk C)

set(CMAKE_C_STANDARD 17)

if (WIN32)
	add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
	include(vcpkg.cmake)
endif ()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_LIST_DIR}/cmake)

file(GLOB GCTK_SOURCES	${CMAKE_CURRENT_LIST_DIR}/src/*.c
						${CMAKE_CURRENT_LIST_DIR}/src/rendering/*.c
						${CMAKE_CURRENT_LIST_DIR}/src/ecs/*.c
)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(LuaJIT REQUIRED)

add_subdirectory(thirdparty/cglm cglm)

add_library(gctk SHARED ${GCTK_SOURCES})
target_link_libraries(gctk PUBLIC OpenGL::GL glfw GLEW::GLEW cglm_headers ${LUAJIT_LIBRARIES})
target_include_directories(gctk PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include ${LUAJIT_INCLUDE_DIR})
target_compile_definitions(gctk PRIVATE GCTK_EXPORTS)

add_library(gctk_static STATIC ${GCTK_SOURCES})
target_link_libraries(gctk_static PUBLIC OpenGL::GL glfw GLEW::GLEW cglm_headers ${LUAJIT_LIBRARIES})
target_include_directories(gctk_static PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include ${LUAJIT_INCLUDE_DIR})
target_compile_definitions(gctk_static PUBLIC GCTK_STATIC)

add_library(gctk::shared ALIAS gctk)
add_library(gctk::static ALIAS gctk_static)

add_executable(gctk_test ${CMAKE_CURRENT_LIST_DIR}/src/test/main.c)
target_link_libraries(gctk_test PRIVATE gctk)

add_subdirectory(tools)