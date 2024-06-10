# FindLuaJIT.cmake
# Locate LuaJIT library
# This module defines
#  LUAJIT_INCLUDE_DIR, where to find luajit.h, etc.
#  LUAJIT_LIBRARIES, libraries to link against
#  LUAJIT_FOUND, if false, do not try to use LuaJIT

if (WIN32)
	find_program(VCPKG_PATH NAMES vcpkg.exe)
	get_filename_component(VCPKG_BASE_PATH ${VCPKG_PATH} DIRECTORY)

	if (CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(LUAJIT_INCLUDE_SEARCH_PATH ${LUAJIT_INCLUDE_SEARCH_PATH}
									   ${VCPKG_BASE_PATH}/installed/x64-windows/include
									   ${VCPKG_BASE_PATH}/installed/x64-windows/include/luajit)
		set(LUAJIT_LIB_SEARCH_PATH ${LUAJIT_LIB_SEARCH_PATH} ${VCPKG_BASE_PATH}/installed/x64-windows/lib)
	else()
		set(LUAJIT_INCLUDE_SEARCH_PATH ${LUAJIT_INCLUDE_SEARCH_PATH} ${VCPKG_BASE_PATH}/installed/x86-windows/include ${VCPKG_BASE_PATH}/installed/x86-windows/include/luajit)
		set(LUAJIT_LIB_SEARCH_PATH ${LUAJIT_LIB_SEARCH_PATH} ${VCPKG_BASE_PATH}/installed/x86-windows/lib)
	endif()
else()
	set(LUAJIT_INCLUDE_SEARCH_PATH ${LUAJIT_INCLUDE_SEARCH_PATH} /usr/local/include			   /usr/include
																 /usr/local/include/luajit	   /usr/include/luajit
																 /usr/local/include/luajit-2.1 /usr/include/luajit-2.1)
	set(LUAJIT_LIB_SEARCH_PATH     ${LUAJIT_LIB_SEARCH_PATH}     /usr/local/lib /usr/lib)
endif()

find_path(LUAJIT_INCLUDE_DIR luajit.h
	HINTS ${LUAJIT_INCLUDE_SEARCH_PATH})

find_library(LUAJIT_LIBRARIES NAMES luajit-5.1 luajit lua51.lib
	HINTS ${LUAJIT_LIB_SEARCH_PATH})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LuaJIT DEFAULT_MSG LUAJIT_INCLUDE_DIR LUAJIT_LIBRARIES)

mark_as_advanced(LUAJIT_INCLUDE_DIR LUAJIT_LIBRARIES)