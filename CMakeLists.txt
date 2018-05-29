#
# File: CMakeLists.txt
# Part of commonpp.
#
# Distributed under the 2-clause BSD licence (See LICENCE.TXT file at the
# project root).
#
# Copyright (c) 2015 Thomas Sanchez.  All rights reserved.
#
project(commonpp C CXX)
cmake_minimum_required(VERSION 2.8)

CMAKE_POLICY(SET CMP0005 NEW)

# Require at least Windows 7
# See: https://msdn.microsoft.com/en-us/library/windows/desktop/aa383745(v=vs.85).aspx
if(WIN32)
    add_definitions(-D_WIN32_WINNT=0x0601)
    add_definitions(-DNTDDI_VERSION=0x06010000)
endif()

# Set a default build type if none was specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting cmake build type to 'Debug' as none was specified.")
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo")
endif()

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "/usr" CACHE PATH "Default install prefix" FORCE)
endif()

set(CMAKE_CXX_STANDARD 14 CACHE STRING "C++ standard for all targets.")

set(INSTALL_LIB_DIR     lib CACHE PATH "Installation directory for libraries")
set(INSTALL_BIN_DIR     bin CACHE PATH "Installation directory for executables")
set(INSTALL_INCLUDE_DIR include CACHE PATH "Installation directory for header files")
set(INSTALL_CMAKE_DIR   "${INSTALL_LIB_DIR}/CMake/commonpp/" CACHE PATH "Installation directory for cmake files")

option(BUILD_TESTS "Should the tests be built" ON)
option(BUILD_EXAMPLES "Should the examples be built" ON)
option(BUILD_BENCH "Should the bench be built" ON)

set(commonpp_MAJOR "0")
set(commonpp_MINOR "1")
set(commonpp_MICRO "1")

set(CPACK_PACKAGE_VERSION_MAJOR ${commonpp_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${commonpp_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${commonpp_MICRO})

set(commonpp_VERSION "${commonpp_MAJOR}.${commonpp_MINOR}.${commonpp_MICRO}")

message(STATUS "CMake Version        : ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION}")
message(STATUS "commonpp Version     : ${commonpp_VERSION}")
message(STATUS "Build Tests          : ${BUILD_TESTS}")
message(STATUS "Build Type           : ${CMAKE_BUILD_TYPE}")
message(STATUS "System               : ${CMAKE_SYSTEM_NAME} ${CMAKE_SYSTEM_VERSION}")
message(STATUS "Install Prefix       : ${CMAKE_INSTALL_PREFIX}")
message(STATUS "Source Directory     : ${commonpp_SOURCE_DIR}")
message(STATUS "C++ Compiler ID      : ${CMAKE_CXX_COMPILER_ID}")


if (${CMAKE_CXX_COMPILER_ID} MATCHES GNU OR ${CMAKE_CXX_COMPILER_ID} MATCHES Clang)
    include(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG(-Wno-unused-local-typedefs HAVE_NOUNUSED_LOCAL_TYPEDEF)
    # With clang + gcc we gave __attribute__ ((unused))
    SET(HAVE_UNUSED_ATTR 1)

    IF (UNIX)
        INCLUDE(CheckFunctionExists)
        SET(SAVE ${CMAKE_REQUIRED_LIBRARIES})
        LIST(APPEND CMAKE_REQUIRED_LIBRARIES "rt")
        CHECK_FUNCTION_EXISTS("clock_getcpuclockid" HAVE_POSIX_CPU_CLOCK)
        SET(CMAKE_REQUIRED_LIBRARIES ${SAVE})
    ENDIF()

    if (HAVE_NOUNUSED_LOCAL_TYPEDEF)
        add_definitions("-Wno-unused-local-typedefs")
    endif()

    add_definitions("-Wextra -Wall -Wno-unused-parameter")
    if (${CMAKE_BUILD_TYPE} MATCHES "Debug")
        add_definitions("-fstack-protector-all")

        if (${CMAKE_CXX_COMPILER_ID} MATCHES GNU)
            add_definitions("-ggdb3")
        endif()
    endif ()
endif()

# Actual Stuff ################################################################
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${commonpp_SOURCE_DIR}/cmake)
include(SubDirList)
include(MakeUnique)
include(CheckIncludeFiles)
include(ThreadLocal)

check_have_make_unique(HAVE_MAKE_UNIQUE)
check_include_files(sys/prctl.h HAVE_SYS_PRCTL_H)
check_have_thread_local_specifier(HAVE_THREAD_LOCAL_SPECIFIER)

#Thread
find_package(Threads REQUIRED)
find_package(TBB REQUIRED)
include_directories(${TBB_INCLUDE_DIRS})
link_directories(${TBB_LIBRARY_DIRS})
link_directories(${TBB_DEBUG_LIBRARY_DIRS})

find_package(HWLOC)
if (HWLOC_FOUND)
    set(HAVE_HWLOC 1)
endif()

#Boost
if (NOT WIN32)
	if (${BUILD_SHARED_LIBS})
		set(Boost_USE_STATIC_LIBS    OFF)
		add_definitions("-DBOOST_ALL_DYN_LINK=1")
	ELSE()
		set(Boost_USE_STATIC_LIBS    ON)
	endif()

	set(Boost_USE_MULTITHREADED  ON)
endif()

if ("${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
    set(HAVE_SYSLOG 1)
endif()

find_package(Boost 1.54.0
    COMPONENTS
        unit_test_framework
        log
        log_setup
        date_time
        filesystem
        thread
        system
    REQUIRED)

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})

set(COMMONPP_DEPS
    ${Boost_LIBRARIES}
    ${TBB_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
)

if (UNIX AND NOT APPLE)
    set (COMMONPP_DEPS ${COMMONPP_DEPS} rt)
endif()

if (${HAVE_HWLOC})
    set (COMMONPP_DEPS ${COMMONPP_DEPS} ${HWLOC_LIBRARIES})
    include_directories(${HWLOC_INCLUDE_DIR})
endif()

add_subdirectory(src/)
add_subdirectory(include/)

if (${BUILD_EXAMPLES})
    add_subdirectory(examples/)
endif()

if (${BUILD_BENCH})
    add_subdirectory(bench/)
endif()

if (${BUILD_TESTS})
    enable_testing()
    add_subdirectory(tests/)
endif()


if(${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME})
    find_program(CLANG_FORMAT
            NAMES
            clang-format
            clang-format-9.0
            clang-format-8.1
            clang-format-8.0
            clang-format-7.0
            clang-format-6.2
            clang-format-6.0
            clang-format-5.2
            clang-format-5.0
            clang-format-4.0
            clang-format-3.9
            clang-format-3.8
            )


    if (CLANG_FORMAT)
        file(GLOB_RECURSE ALL_SOURCE_FILES
                src/*.cpp
                include/*.hpp
                tests/*.cpp
                tests/*.hpp
                examples/*.cpp
                examples/*.hpp
                bench/*.cpp
                bench/*.hpp
        )

        add_custom_target(
                format
                COMMAND
                ${CLANG_FORMAT}
                -style=file
                -i
                ${ALL_SOURCE_FILES})
    endif()
endif()

