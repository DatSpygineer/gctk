cmake_minimum_required(VERSION 3.25)

project(tinyfiledialogs C)
project(gctk_test C)
project(gctk_static C)
project(gctk C)

option(GCTK_ENABLE_LUA "Should GCTk use Lua bindings" OFF)
option(GCTK_ENABLE_FLUIDSYNTH "Should GCTk link fluidsynth" OFF)

set(CMAKE_C_STANDARD 11)

if (WIN32)
	add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
	include(vcpkg.cmake)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_LIST_DIR}/cmake)

file(GLOB GCTK_SOURCES	${CMAKE_CURRENT_LIST_DIR}/src/*.c
						${CMAKE_CURRENT_LIST_DIR}/src/rendering/*.c
						${CMAKE_CURRENT_LIST_DIR}/src/ecs/*.c
)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(cJSON REQUIRED)

if (GCTK_ENABLE_LUA)
	find_package(LuaJIT REQUIRED)
	file(GLOB GCTK_LUA_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/lua/*.c)
	set(GCTK_SOURCES ${GCTK_SOURCES} ${GCTK_LUA_SOURCES})
endif()

if (GCTK_ENABLE_FLUIDSYNTH)
	find_package(PkgConfig REQUIRED)
	pkg_check_modules(FluidSynth REQUIRED fluidsynth)
	set(GCTK_SOURCES ${GCTK_SOURCES} ${CMAKE_CURRENT_LIST_DIR}/src/audio/gctk_midi.c)
endif()

add_subdirectory(thirdparty/cglm cglm)

add_library(tinyfiledialogs STATIC
	${CMAKE_CURRENT_LIST_DIR}/thirdparty/tinyfiledialogs/tinyfiledialogs.h
	${CMAKE_CURRENT_LIST_DIR}/thirdparty/tinyfiledialogs/tinyfiledialogs.c
)
target_include_directories(tinyfiledialogs PUBLIC ${CMAKE_CURRENT_LIST_DIR}/thirdparty/tinyfiledialogs)
set_target_properties     (tinyfiledialogs PROPERTIES POSITION_INDEPENDENT_CODE ON)

add_library               (gctk SHARED ${GCTK_SOURCES})
target_link_libraries     (gctk PUBLIC OpenGL::GL glfw GLEW::GLEW cglm_headers tinyfiledialogs cjson)
target_include_directories(gctk PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include ${CMAKE_CURRENT_LIST_DIR}/thirdparty/cJSON)
target_compile_definitions(gctk PRIVATE -DGCTK_EXPORTS)
set_target_properties     (gctk PROPERTIES POSITION_INDEPENDENT_CODE ON)

if (GCTK_ENABLE_LUA)
	target_link_libraries     (gctk PUBLIC ${LUAJIT_LIBRARIES}  )
	target_include_directories(gctk PUBLIC ${LUAJIT_INCLUDE_DIR})
	target_compile_definitions(gctk PUBLIC -DGCTK_ENABLE_LUA=1  )
endif()
if (GCTK_ENABLE_FLUIDSYNTH)
	target_link_libraries     (gctk PUBLIC ${FluidSynth_LIBRARIES}   )
	target_include_directories(gctk PUBLIC ${FluidSynth_INCLUDE_DIRS})
	target_compile_definitions(gctk PUBLIC -DGCTK_ENABLE_FLUIDSYNTH=1)
endif()

add_library               (gctk_static STATIC ${GCTK_SOURCES})
target_link_libraries     (gctk_static PUBLIC OpenGL::GL glfw GLEW::GLEW cglm_headers tinyfiledialogs cjson)
target_include_directories(gctk_static PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
target_compile_definitions(gctk_static PUBLIC -DGCTK_STATIC)

if (GCTK_ENABLE_LUA)
	target_link_libraries     (gctk_static PUBLIC ${LUAJIT_LIBRARIES}  )
	target_include_directories(gctk_static PUBLIC ${LUAJIT_INCLUDE_DIR})
	target_compile_definitions(gctk_static PUBLIC -DGCTK_ENABLE_LUA=1  )
endif()
if (GCTK_ENABLE_FLUIDSYNTH)
	target_link_libraries     (gctk_static PUBLIC ${FluidSynth_LIBRARIES}   )
	target_include_directories(gctk_static PUBLIC ${FluidSynth_INCLUDE_DIRS})
	target_compile_definitions(gctk_static PUBLIC -DGCTK_ENABLE_FLUIDSYNTH=1)
endif()

add_library(gctk::shared ALIAS gctk)
add_library(gctk::static ALIAS gctk_static)

function (gctk_add_game EXE_NAME NAME AUTHOR VERSION_MAJOR VERSION_MINOR VERSION_PATCH VERSION_LIFECYCLE)
	add_executable(${EXE_NAME} ${ARGN})
	target_link_libraries(${EXE_NAME} PRIVATE gctk)
	target_compile_definitions(${EXE_NAME} PRIVATE
		"-DGCTK_GAME_NAME=\"${NAME}\""
		"-DGCTK_GAME_AUTHOR=\"${AUTHOR}\""
		"-DGCTK_GAME_VERSION_MAJOR=${VERSION_MAJOR}"
		"-DGCTK_GAME_VERSION_MINOR=${VERSION_MINOR}"
		"-DGCTK_GAME_VERSION_PATCH=${VERSION_PATCH}"
		"-DGCTK_GAME_VERSION_LIFECYCLE=${VERSION_LIFECYCLE}"
		"-DGCTK_GAME_VERSION=VERSION(GCTK_GAME_VERSION_MAJOR, GCTK_GAME_VERSION_MINOR, GCTK_GAME_VERSION_PATCH, GCTK_GAME_VERSION_LIFECYCLE)"
	)
endfunction()
function (gctk_add_game_static EXE_NAME NAME AUTHOR VERSION_MAJOR VERSION_MINOR VERSION_PATCH VERSION_LIFECYCLE)
	add_executable(${EXE_NAME} ${ARGN})
	target_link_libraries(${EXE_NAME} PRIVATE gctk_static)
	target_compile_definitions(${EXE_NAME} PRIVATE
		"-DGCTK_GAME_NAME=\"${NAME}\""
		"-DGCTK_GAME_AUTHOR=\"${AUTHOR}\""
		"-DGCTK_GAME_VERSION_MAJOR=${VERSION_MAJOR}"
		"-DGCTK_GAME_VERSION_MINOR=${VERSION_MINOR}"
		"-DGCTK_GAME_VERSION_PATCH=${VERSION_PATCH}"
		"-DGCTK_GAME_VERSION_LIFECYCLE=${VERSION_LIFECYCLE}"
		"-DGCTK_GAME_VERSION=VERSION(GCTK_GAME_VERSION_MAJOR, GCTK_GAME_VERSION_MINOR, GCTK_GAME_VERSION_PATCH, GCTK_GAME_VERSION_LIFECYCLE)"
	)
endfunction()

gctk_add_game(gctk_test "Test" "Gctk" 1 0 0 GCTK_VERSION_RELEASE ${CMAKE_CURRENT_LIST_DIR}/src/test/main.c)
add_subdirectory(tools)