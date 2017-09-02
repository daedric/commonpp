#.rst:
# FindHWLOC
# ----------
#
# Try to find Portable Hardware Locality (HWLOC) libraries.
# http://www.open-mpi.org/software/hwloc
#
# You may declare HWLOC_ROOT environment variable to tell where
# your hwloc library is installed. 
#
# Once done this will define::
#
#   HWLOC_FOUND            - True if hwloc was found
#   HWLOC_INCLUDE_DIRS     - include directories for hwloc
#   HWLOC_LIBRARIES        - link against these libraries to use hwloc
#   HWLOC_VERSION          - version
#   HWLOC_CFLAGS           - include directories as compiler flags
#   HWLOC_LDLFAGS          - link paths and libs as compiler flags
#

#=============================================================================
# Copyright 2014 Mikael Lepistö
#
# Distributed under the OSI-approved BSD License (the "License");
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================

if(WIN32)
  find_path(HWLOC_INCLUDE_DIR
    NAMES
      hwloc.h
    PATHS
      ENV "PROGRAMFILES(X86)"
      ENV HWLOC_ROOT
    PATH_SUFFIXES
      include
  )

  find_library(HWLOC_LIBRARY
    NAMES 
      libhwloc.lib
    PATHS
      ENV "PROGRAMFILES(X86)"
      ENV HWLOC_ROOT
    PATH_SUFFIXES
      lib
  )

  #
  # Check if the found library can be used to linking 
  #
  SET (_TEST_SOURCE "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/linktest.c")
  FILE (WRITE "${_TEST_SOURCE}"
    "
    #include <hwloc.h>
    int main()
    { 
      hwloc_topology_t topology;
      int nbcores;
      hwloc_topology_init(&topology);
      hwloc_topology_load(topology);
      nbcores = hwloc_get_nbobjs_by_type(topology, HWLOC_OBJ_CORE);
      hwloc_topology_destroy(topology);
      return 0;
    }
    "
  )

  TRY_COMPILE(_LINK_SUCCESS ${CMAKE_BINARY_DIR} "${_TEST_SOURCE}"
    CMAKE_FLAGS
    "-DINCLUDE_DIRECTORIES:STRING=${HWLOC_INCLUDE_DIR}"
    CMAKE_FLAGS
    "-DLINK_LIBRARIES:STRING=${HWLOC_LIBRARY}"
  )

  IF(NOT _LINK_SUCCESS)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
      message(STATUS "You are building 64bit target.")
    ELSE()
      message(STATUS "You are building 32bit code. If you like to build x64 use e.g. -G 'Visual Studio 12 Win64' generator." )
    ENDIF()
    message(FATAL_ERROR "Library found, but linking test program failed.")
  ENDIF()

  #
  # Resolve version if some compiled binary found...
  #
  find_program(HWLOC_INFO_EXECUTABLE
    NAMES 
      hwloc-info
    PATHS
      ENV HWLOC_ROOT 
    PATH_SUFFIXES
      bin
  )
  
  if(HWLOC_INFO_EXECUTABLE)
    execute_process(
      COMMAND ${HWLOC_INFO_EXECUTABLE} "--version" 
      OUTPUT_VARIABLE HWLOC_VERSION_LINE 
      OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    string(REGEX MATCH "([0-9]+.[0-9]+)$" 
      HWLOC_VERSION "${HWLOC_VERSION_LINE}")
    unset(HWLOC_VERSION_LINE)
  endif()
  
  #
  # All good
  #

  set(HWLOC_LIBRARIES ${HWLOC_LIBRARY})
  set(HWLOC_INCLUDE_DIRS ${HWLOC_INCLUDE_DIR})

  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(
    HWLOC
    FOUND_VAR HWLOC_FOUND
    REQUIRED_VARS HWLOC_LIBRARY HWLOC_INCLUDE_DIR
    VERSION_VAR HWLOC_VERSION)

  mark_as_advanced(
    HWLOC_INCLUDE_DIR
    HWLOC_LIBRARY)

  foreach(arg ${HWLOC_INCLUDE_DIRS})
    set(HWLOC_CFLAGS "${HWLOC_CFLAGS} /I${arg}")
  endforeach()

  set(HWLOC_LDFLAGS "${HWLOC_LIBRARY}")

else()

  # Find with pkgconfig
  find_package(PkgConfig)

  if(HWLOC_ROOT)
    set(ENV{PKG_CONFIG_PATH} "${HWLOC_ROOT}/lib/pkgconfig")
  else()
    foreach(PREFIX ${CMAKE_PREFIX_PATH})
      set(PKG_CONFIG_PATH "${PKG_CONFIG_PATH}:${PREFIX}/lib/pkgconfig")
    endforeach()
    set(ENV{PKG_CONFIG_PATH} "${PKG_CONFIG_PATH}:$ENV{PKG_CONFIG_PATH}")
  endif()

  if(HWLOC_FIND_REQUIRED)
    set(_HWLOC_OPTS "REQUIRED")
  elseif(HWLOC_FIND_QUIETLY)
    set(_HWLOC_OPTS "QUIET")
  else()
    set(_HWLOC_output 1)
  endif()

  if(HWLOC_FIND_VERSION)
    if(HWLOC_FIND_VERSION_EXACT)
      pkg_check_modules(HWLOC ${_HWLOC_OPTS} HWLOC=${HWLOC_FIND_VERSION})
    else()
      pkg_check_modules(HWLOC ${_HWLOC_OPTS} HWLOC>=${HWLOC_FIND_VERSION})
    endif()
  else()
    pkg_check_modules(HWLOC ${_HWLOC_OPTS} HWLOC)
  endif()

  if(HWLOC_FOUND)
    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(HWLOC DEFAULT_MSG HWLOC_LIBRARIES)

    if(NOT ${HWLOC_VERSION} VERSION_LESS 1.7.0)
      set(HWLOC_GL_FOUND 1)
    endif()

    if(_HWLOC_output)
      message(STATUS
        "Found HWLOC ${HWLOC_VERSION} in ${HWLOC_INCLUDE_DIRS}:${HWLOC_LIBRARIES}")
    endif()
  endif()
endif()

