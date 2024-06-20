cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD 20)

project(gtex_tool)

set(IMGUI_BASE_PATH ${CMAKE_CURRENT_LIST_DIR}/../thirdparty/imgui)

set(IMGUI_SOURCES
	${IMGUI_BASE_PATH}/imgui.cpp
	${IMGUI_BASE_PATH}/imgui_widgets.cpp
	${IMGUI_BASE_PATH}/imgui_tables.cpp
	${IMGUI_BASE_PATH}/imgui_draw.cpp
	${IMGUI_BASE_PATH}/imgui_demo.cpp

	${IMGUI_BASE_PATH}/backends/imgui_impl_glfw.cpp
	${IMGUI_BASE_PATH}/backends/imgui_impl_opengl3.cpp
)

if (NOT WIN32)
	find_package(PkgConfig REQUIRED)
	pkg_check_modules(GTK REQUIRED gtk+-3.0)
	include_directories(${GTK_INCLUDE_DIRS})
	link_directories(${GTK_LIBRARY_DIRS})
	add_definitions(${GTK_CFLAGS_OTHER})
endif()

include_directories(${IMGUI_BASE_PATH} ${IMGUI_BASE_PATH}/backends)

add_executable(gtex_tool main.cpp ${IMGUI_SOURCES})
target_link_libraries(gtex_tool PRIVATE gctk_static)

if (NOT WIN32)
	target_link_libraries(gtex_tool PRIVATE ${GTK_LIBRARIES})
endif()