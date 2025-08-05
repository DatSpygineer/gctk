cmake_minimum_required(VERSION 3.30)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)

set(GCTK_VERSION_MAJOR 1)
set(GCTK_VERSION_MINOR 0)
set(GCTK_VERSION_PATCH 0)
set(GCTK_VERSION_RELEASE Alpha)

function(add_gctk_library TARGET)
    add_library(${TARGET} SHARED ${ARGN})
    string(TOUPPER ${TARGET} TARGET_UPPER)
    target_compile_definitions(${TARGET} PRIVATE -D${TARGET_UPPER}_EXPORTS)
endfunction()

file(GLOB_RECURSE GCTK_SHARED_SOURCES ${CMAKE_CURRENT_LIST_DIR}/shared/*.hpp ${CMAKE_CURRENT_LIST_DIR}/shared/*.cpp)
file(GLOB_RECURSE GCTK_CLIENT_SOURCES ${CMAKE_CURRENT_LIST_DIR}/client/*.hpp ${CMAKE_CURRENT_LIST_DIR}/client/*.cpp)
file(GLOB_RECURSE GCTK_SERVER_SOURCES ${CMAKE_CURRENT_LIST_DIR}/server/*.hpp ${CMAKE_CURRENT_LIST_DIR}/server/*.cpp)

add_library(gctk_client STATIC ${GCTK_CLIENT_SOURCES} ${GCTK_SHARED_SOURCES})
target_link_libraries(gctk_client PUBLIC OpenGL::GL GLEW::GLEW glfw)
target_include_directories(gctk_client PUBLIC ${CMAKE_CURRENT_LIST_DIR}/client ${CMAKE_CURRENT_LIST_DIR}/shared ${CMAKE_CURRENT_LIST_DIR}/thirdparty)
target_compile_definitions(gctk_client PUBLIC GCTK_CLIENT)

add_library(gctk_server STATIC ${GCTK_SERVER_SOURCES} ${GCTK_SHARED_SOURCES})
target_include_directories(gctk_server PUBLIC ${CMAKE_CURRENT_LIST_DIR}/server ${CMAKE_CURRENT_LIST_DIR}/shared ${CMAKE_CURRENT_LIST_DIR}/thirdparty)
target_compile_definitions(gctk_server PUBLIC GCTK_SERVER)

target_compile_definitions(
    gctk_client PUBLIC
    -DGCTK_OS_NAME="${CMAKE_SYSTEM_NAME}"
    -DGCTK_ARCH_NAME="${CMAKE_SYSTEM_PROCESSOR}"
    -DGCTK_VERSION_MAJOR=${GCTK_VERSION_MAJOR}
    -DGCTK_VERSION_MINOR=${GCTK_VERSION_MINOR}
    -DGCTK_VERSION_PATCH=${GCTK_VERSION_PATCH}
    -DGCTK_VERSION_RELEASE="${GCTK_VERSION_RELEASE}"
    -DGCTK_VERSION_STRING="${GCTK_VERSION_MAJOR}.${GCTK_VERSION_MINOR}.${GCTK_VERSION_PATCH} ${GCTK_VERSION_RELEASE}"
)
target_compile_definitions(
    gctk_server PUBLIC
    -DGCTK_OS_NAME="${CMAKE_SYSTEM_NAME}"
    -DGCTK_ARCH_NAME="${CMAKE_SYSTEM_PROCESSOR}"
    -DGCTK_VERSION_MAJOR=${GCTK_VERSION_MAJOR}
    -DGCTK_VERSION_MINOR=${GCTK_VERSION_MINOR}
    -DGCTK_VERSION_PATCH=${GCTK_VERSION_PATCH}
    -DGCTK_VERSION_RELEASE="${GCTK_VERSION_RELEASE}"
)

add_library(gctk::client ALIAS gctk_client)
add_library(gctk::server ALIAS gctk_server)

function(add_gctk_client TARGET GAME_OUTPUT_DIRECTORY)
    add_gctk_library(${TARGET}_client ${ARGN})
    target_link_libraries(${TARGET}_client PRIVATE gctk::client)
    target_compile_definitions(${TARGET}_client PRIVATE -DGCTK_GAME_NAME="${TARGET}")
    set_target_properties(${TARGET}_client PROPERTIES
        OUTPUT_NAME game_client
        LIBRARY_OUTPUT_DIRECTORY ${GAME_OUTPUT_DIRECTORY}/bin/${CMAKE_SYSTEM_NAME}_${CMAKE_SYSTEM_PROCESSOR}
    )
endfunction()
function(add_gctk_server TARGET GAME_OUTPUT_DIRECTORY)
    add_gctk_library(${TARGET}_server ${ARGN})
    target_link_libraries(${TARGET}_server PRIVATE gctk::server)
    target_compile_definitions(${TARGET}_server PRIVATE -DGCTK_GAME_NAME="${TARGET}")
    set_target_properties(${TARGET}_server PROPERTIES
        OUTPUT_NAME game_server
        LIBRARY_OUTPUT_DIRECTORY ${GAME_OUTPUT_DIRECTORY}/bin/${CMAKE_SYSTEM_NAME}_${CMAKE_SYSTEM_PROCESSOR}
    )
endfunction()
function(add_gctk_launcher TARGET GAME_OUTPUT_DIRECTORY)
    add_executable(${TARGET} ${ARGN})
    target_include_directories(${TARGET} PUBLIC ${GCTK_ROOT_DIRECTORY}/launcher)
    target_link_libraries(${TARGET} PRIVATE gctk::client ${TARGET}_client)
    target_compile_definitions(${TARGET} PRIVATE -DGCTK_GAME_NAME="${TARGET}")
    set_target_properties(${TARGET} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${GAME_OUTPUT_DIRECTORY}/bin/${CMAKE_SYSTEM_NAME}_${CMAKE_SYSTEM_PROCESSOR}
    )
endfunction()
function(add_gctk_launcher_singleplayer TARGET GAME_OUTPUT_DIRECTORY)
    add_executable(${TARGET} ${ARGN})
    target_include_directories(${TARGET} PUBLIC ${GCTK_ROOT_DIRECTORY}/launcher)
    target_link_libraries(${TARGET} PRIVATE gctk::client ${TARGET}_client)
    target_compile_definitions(${TARGET} PRIVATE -DGCTK_GAME_NAME="${TARGET}" -DGCTK_SINGLEPLAYER)
    set_target_properties(${TARGET} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${GAME_OUTPUT_DIRECTORY}/bin/${CMAKE_SYSTEM_NAME}_${CMAKE_SYSTEM_PROCESSOR}
    )
endfunction()

function (gctk_project TARGET)
    project(${TARGET})
    project(${TARGET}_client)
    project(${TARGET}_server)
endfunction()
