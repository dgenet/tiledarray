###
#
# @copyright (c) 2009-2015 The University of Tennessee and The University
#                          of Tennessee Research Foundation.
#                          All rights reserved.
# @copyright (c) 2012-2015 Inria. All rights reserved.
# @copyright (c) 2012-2015 Bordeaux INP, CNRS (LaBRI UMR 5800), Inria, Univ. Bordeaux. All rights reserved.
#
###
#
# - Find PARSEC include dirs and libraries
# Use this module by invoking find_package with the form:
#  find_package(PARSEC
#               [version] [EXACT]      # Minimum or EXACT version e.g. 1.1
#               [REQUIRED]             # Fail with error if parsec is not found
#               [COMPONENTS <comp1> <comp2> ...] # dependencies
#              )
#
#  PARSEC depends on the following libraries:
#   - Threads, m, rt
#
#  COMPONENTS are optional libraries PARSEC could be linked with,
#  Use it to drive detection of a specific compilation chain
#  COMPONENTS can be some of the following:
#   - HWLOC: to activate the detection of PARSEC linked with HWLOC
#   - CUDA: to activate the detection of PARSEC linked with CUDA
#   - MPI: to activate the detection of PARSEC linked with MPI
#   - PAPI: to activate the detection of PARSEC linked with PAPI
#   - AYUDAME: ??
#
# Results are reported in variables:
#  PARSEC_FOUND                  - True if headers and requested libraries were found
#  PARSEC_C_FLAGS                - list of required compilation flags (excluding -I)
#  PARSEC_LINKER_FLAGS           - list of required linker flags (excluding -l and -L)
#  PARSEC_INCLUDE_DIRS           - parsec include directories
#  PARSEC_LIBRARY_DIRS           - Link directories for parsec libraries
#  PARSEC_LIBRARIES              - parsec libraries
#  PARSEC_INCLUDE_DIRS_DEP       - parsec + dependencies include directories
#  PARSEC_LIBRARY_DIRS_DEP       - parsec + dependencies link directories
#  PARSEC_LIBRARIES_DEP          - parsec libraries + dependencies
#  PARSEC_parsec_ptgpp_BIN_DIR   - path to parsec driver parsec_ptgpp 
#  PARSEC_PTGPP                  - parsec jdf compiler
# The user can give specific paths where to find the libraries adding cmake
# options at configure (ex: cmake path/to/project -DPARSEC=path/to/parsec):
#  PARSEC_DIR                    - Where to find the base directory of parsec
#  PARSEC_INCDIR                 - Where to find the header files
#  PARSEC_LIBDIR                 - Where to find the library files
# The module can also look for the following environment variables if paths
# are not given as cmake variable: PARSEC_DIR, PARSEC_INCDIR, PARSEC_LIBDIR

#=============================================================================
# Copyright 2012-2013 Inria
# Copyright 2012-2013 Emmanuel Agullo
# Copyright 2012-2013 Mathieu Faverge
# Copyright 2012      Cedric Castagnede
# Copyright 2013      Florent Pruvost
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file MORSE-Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of Morse, substitute the full
#  License text for the above reference.)

include(CheckSymbolExists)

if (NOT DPLASMA_FOUND)
    set(DPLASMA_DIR "" CACHE PATH "Installation directory of DPLASMA library")
    if (NOT DPLASMA_FIND_QUIETLY)
        message(STATUS "A cache variable, namely DPLASMA_DIR, has been set to specify the install directory of DPLASMA")
    endif()
endif()

find_package(PARSEC)
if (NOT PARSEC_FOUND)
    message(ERROR "DPLASMA requires PARSEC to work")
endif (NOT PARSEC_FOUND)

set(ENV_DPLASMA_DIR    "$ENV{DPLASMA_DIR}")
set(ENV_DPLASMA_INCDIR "$ENV{DPLASMA_INCDIR}")
set(ENV_DPLASMA_LIBDIR "$ENV{DPLASMA_LIBDIR}")
if ( DPLASMA_DIR OR ( DPLASMA_INCDIR AND DPLASMA_LIBDIR) OR ENV_DPLASMA_DIR OR (ENV_DPLASMA_INCDIR AND ENV_DPLASMA_LIBDIR) )
    set(DPLASMA_GIVEN_BY_USER "TRUE")
endif()

# Optionally use pkg-config to detect include/library dirs (if pkg-config is available)
# -------------------------------------------------------------------------------------
include(FindPkgConfig)
find_package(PkgConfig QUIET)

if(PKG_CONFIG_EXECUTABLE AND NOT PARSEC_GIVEN_BY_USER)

    pkg_search_module(DPLASMA dplasma)
    if (NOT DPLASMA_FIND_QUIETLY)
        if (DPLASMA_FOUND AND DPLASMA_LIBRARIES)
            message(STATUS "Looking for DPLASMA - found using PkgConfig")
        else()
            message("${Magenta}Looking for DPLASMA - not found using PkgConfig."
                "Perhaps you should add the directory containing dplasma.pc"
                "to the PKG_CONFIG_PATH environment variable.${ColourReset}")
        endif()
    endif()

    set(DPLASMA_INCLUDE_DIRS_DEP "${DPLASMA_INCLUDE_DIRS}")
    set(DPLASMA_LIBRARY_DIRS_DEP "${DPLASMA_LIBRARY_DIRS}")
    set(DPLASMA_LIBRARIES_DEP "${DPLASMA_LIBRARIES};${DPLASMA_LDFLAGS_OTHER}")

endif(PKG_CONFIG_EXECUTABLE AND NOT PARSEC_GIVEN_BY_USER)

if( (NOT PKG_CONFIG_EXECUTABLE) OR (PKG_CONFIG_EXECUTABLE AND NOT DPLASMA_FOUND) OR (DPLASMA_GIVEN_BY_USER) )

    # Looking for include
    # -------------------

    # Add system include paths to search include
    # ------------------------------------------
    unset(_inc_env)
    set(ENV_DPLASMA_DIR "$ENV{DPLASMA_DIR}")
    set(ENV_DPLASMA_INCDIR "$ENV{DPLASMA_INCDIR}")
    if(ENV_DPLASMA_INCDIR)
        list(APPEND _inc_env "${ENV_DPLASMA_INCDIR}")
    elseif(ENV_DPLASMA_DIR)
        list(APPEND _inc_env "${ENV_DPLASMA_DIR}")
        list(APPEND _inc_env "${ENV_DPLASMA_DIR}/include")
    else()
        if(WIN32)
            string(REPLACE ":" ";" _inc_env "$ENV{INCLUDE}")
        else()
            string(REPLACE ":" ";" _path_env "$ENV{INCLUDE}")
            list(APPEND _inc_env "${_path_env}")
            string(REPLACE ":" ";" _path_env "$ENV{C_INCLUDE_PATH}")
            list(APPEND _inc_env "${_path_env}")
            string(REPLACE ":" ";" _path_env "$ENV{CPATH}")
            list(APPEND _inc_env "${_path_env}")
            string(REPLACE ":" ";" _path_env "$ENV{INCLUDE_PATH}")
            list(APPEND _inc_env "${_path_env}")
        endif()
    endif()
    list(APPEND _inc_env "${CMAKE_PLATFORM_IMPLICIT_INCLUDE_DIRECTORIES}")
    list(APPEND _inc_env "${CMAKE_C_IMPLICIT_INCLUDE_DIRECTORIES}")
    list(REMOVE_DUPLICATES _inc_env)

    # Try to find the dplasma headers in the given paths
    # -------------------------------------------------

    # create list of headers to find
    set(DPLASMA_hdrs_to_find "dplasma.h")

    # call cmake macro to find the header path
    if(DPLASMA_INCDIR)
        foreach(dplasma_hdr ${DPLASMA_hdrs_to_find})
            message (ERROR "it is looking for dplasma_hdr")
            set(DPLASMA_${dplasma_hdr}_INCLUDE_DIRS "DPLASMA_${dplasma_hdr}_INCLUDE_DIRS-NOTFOUND")
            find_path(DPLASMA_${dplasma_hdr}_INCLUDE_DIRS
                      NAMES ${dplasma_hdr}
                      HINTS ${DPLASMA_INCDIR})
        endforeach()
    else()
        if(DPLASMA_DIR)
            set(DPLASMA_${dplasma_hdr}_INCLUDE_DIRS "DPLASMA_${dplasma_hdr}_INCLUDE_DIRS-NOTFOUND")
            foreach(dplasma_hdr ${DPLASMA_hdrs_to_find})
                find_path(DPLASMA_${dplasma_hdr}_INCLUDE_DIRS
                          NAMES ${dplasma_hdr}
                          HINTS ${DPLASMA_DIR}
                          PATH_SUFFIXES "include")
            endforeach()
        else()
            foreach(dplasma_hdr ${DPLASMA_hdrs_to_find})
                set(DPLASMA_${dplasma_hdr}_INCLUDE_DIRS "DPLASMA_${dplasma_hdr}_INCLUDE_DIRS-NOTFOUND")
                find_path(DPLASMA_${dplasma_hdr}_INCLUDE_DIRS
                          NAMES ${dplasma_hdr}
                          HINTS ${_inc_env})
            endforeach()
        endif()
    endif()

    # If found, add path to cmake variable
    # ------------------------------------
    set(DPLASMA_INCLUDE_DIRS "")
    foreach(dplasma_hdr ${DPLASMA_hdrs_to_find})

        if (DPLASMA_${dplasma_hdr}_INCLUDE_DIRS)
            # set cmake variables using the pkg-config naming convention
            list(APPEND DPLASMA_INCLUDE_DIRS "${DPLASMA_${dplasma_hdr}_INCLUDE_DIRS}" )
        else ()
            if(NOT DPLASMA_FIND_QUIETLY)
                message(STATUS "Looking for dplasma -- ${dplasma_hdr} not found")
            endif()
        endif ()
        mark_as_advanced(DPLASMA_${dplasma_hdr}_INCLUDE_DIRS)

    endforeach(dplasma_hdr ${DPLASMA_hdrs_to_find})

    if (DPLASMA_INCLUDE_DIRS)
        list(REMOVE_DUPLICATES DPLASMA_INCLUDE_DIRS)
    endif ()

    # Looking for lib
    # ---------------

    set(DPLASMA_LIBRARIES "")
    set(DPLASMA_LIBRARY_DIRS "")

    # Add system library paths to search lib
    # --------------------------------------
    unset(_lib_env)
    set(ENV_DPLASMA_LIBDIR "$ENV{DPLASMA_LIBDIR}")
    if(ENV_DPLASMA_LIBDIR)
        list(APPEND _lib_env "${ENV_DPLASMA_LIBDIR}")
    elseif(ENV_DPLASMA_DIR)
        list(APPEND _lib_env "${ENV_DPLASMA_DIR}")
        list(APPEND _lib_env "${ENV_DPLASMA_DIR}/lib")
    else()
        if(WIN32)
            string(REPLACE ":" ";" _lib_env "$ENV{LIB}")
        else()
            if(APPLE)
                string(REPLACE ":" ";" _lib_env "$ENV{DYLD_LIBRARY_PATH}")
            else()
                string(REPLACE ":" ";" _lib_env "$ENV{LD_LIBRARY_PATH}")
            endif()
            list(APPEND _lib_env "${CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES}")
            list(APPEND _lib_env "${CMAKE_C_IMPLICIT_LINK_DIRECTORIES}")
        endif()
    endif()
    list(REMOVE_DUPLICATES _lib_env)

    # Try to find the dplasma libs in the given paths
    # ----------------------------------------------

    # create list of libs to find
    set(DPLASMA_libs_to_find "dplasma")

    # call cmake macro to find the lib path
    if(DPLASMA_LIBDIR)
        foreach(dplasma_lib ${DPLASMA_libs_to_find})
            set(DPLASMA_${dplasma_lib}_LIBRARY "DPLASMA_${dplasma_lib}_LIBRARY-NOTFOUND")
            find_library(DPLASMA_${dplasma_lib}_LIBRARY
                         NAMES ${dplasma_lib}
                         HINTS ${DPLASMA_LIBDIR})
        endforeach()
    else()
        if(DPLASMA_DIR)
            foreach(dplasma_lib ${DPLASMA_libs_to_find})
                set(DPLASMA_${dplasma_lib}_LIBRARY "DPLASMA_${dplasma_lib}_LIBRARY-NOTFOUND")
                find_library(DPLASMA_${dplasma_lib}_LIBRARY
                             NAMES ${dplasma_lib}
                             HINTS ${DPLASMA_DIR}
                             PATH_SUFFIXES lib lib32 lib64)
            endforeach()
        else()
            foreach(dplasma_lib ${DPLASMA_libs_to_find})
                set(DPLASMA_${dplasma_lib}_LIBRARY "DPLASMA_${dplasma_lib}_LIBRARY-NOTFOUND")
                find_library(DPLASMA_${dplasma_lib}_LIBRARY
                             NAMES ${dplasma_lib}
                             HINTS ${_lib_env})
            endforeach()
        endif()
    endif()

    # If found, add path to cmake variable
    # ------------------------------------
    foreach(dplasma_lib ${DPLASMA_libs_to_find})

        if (DPLASMA_${dplasma_lib}_LIBRARY)

            get_filename_component(${dplasma_lib}_lib_path ${DPLASMA_${dplasma_lib}_LIBRARY} PATH)
            # set cmake variables (respects naming convention)
            list(APPEND DPLASMA_LIBRARIES "${DPLASMA_${dplasma_lib}_LIBRARY}")
            list(APPEND DPLASMA_LIBRARY_DIRS "${${dplasma_lib}_lib_path}")

        else (DPLASMA_${dplasma_lib}_LIBRARY)

            if(NOT DPLASMA_FIND_QUIETLY)
                message(STATUS "Looking for dplasma -- lib ${dplasma_lib} not found")
            endif()

        endif (DPLASMA_${dplasma_lib}_LIBRARY)

        mark_as_advanced(DPLASMA_${dplasma_lib}_LIBRARY)

    endforeach(dplasma_lib ${DPLASMA_libs_to_find})

    list(REMOVE_DUPLICATES DPLASMA_LIBRARIES)
    if (DPLASMA_LIBRARY_DIRS)
        list(REMOVE_DUPLICATES DPLASMA_LIBRARY_DIRS)
    endif ()

    # check a function to validate the find
    if(DPLASMA_LIBRARIES)

        set(REQUIRED_FLAGS)
        set(REQUIRED_LDFLAGS)
        set(REQUIRED_INCDIRS)
        set(REQUIRED_LIBDIRS)
        set(REQUIRED_LIBS)

        # DPLASMA
        if (DPLASMA_INCLUDE_DIRS)
            set(REQUIRED_INCDIRS "${DPLASMA_INCLUDE_DIRS}")
        endif()
        set(CMAKE_REQUIRED_FLAGS)
        foreach(libdir ${DPLASMA_LIBRARY_DIRS})
            if (libdir)
                list(APPEND REQUIRED_LIBDIRS "${libdir}")
            endif()
        endforeach()
        set(REQUIRED_LIBS "${DPLASMA_LIBRARIES}")

        # PaRSEC
        if (PARSEC_FOUND)
            if (PARSEC_INCLUDE_DIRS)
                list(APPEND REQUIRED_INCDIRS "${PARSEC_INCLUDE_DIRS}")
            endif()
            if (PARSEC_LIBRARY_DIRS)
                list(APPEND REQUIRED_LIBDIRS "${PARSEC_LIBRARY_DIRS}")
            endif()
            foreach(lib ${PARSEC_LIBRARIES})
                if (EXISTS ${lib} OR ${lib} MATCHES "^-")
                    list(APPEND REQUIRED_LIBS "${lib}")
                else()
                    list(APPEND REQUIRED_LIBS "-l${lib}")
                endif()
            endforeach()
        endif()

        # set required libraries for link
        set(CMAKE_REQUIRED_INCLUDES "${REQUIRED_INCDIRS}")
        set(CMAKE_REQUIRED_LIBRARIES)
        list(APPEND CMAKE_REQUIRED_LIBRARIES "${REQUIRED_LDFLAGS}")
        foreach(lib_dir ${REQUIRED_LIBDIRS})
            list(APPEND CMAKE_REQUIRED_LIBRARIES "-L${lib_dir}")
        endforeach()
        list(APPEND CMAKE_REQUIRED_LIBRARIES "${REQUIRED_LIBS}")
        list(APPEND CMAKE_REQUIRED_FLAGS "${REQUIRED_FLAGS}")
        string(REGEX REPLACE "^ -" "-" CMAKE_REQUIRED_LIBRARIES "${CMAKE_REQUIRED_LIBRARIES}")

        # test link
        unset(DPLASMA_WORKS CACHE)
        include(CheckFunctionExists)
        check_function_exists(dplasma_dsumma DPLASMA_WORKS)
        mark_as_advanced(DPLASMA_WORKS)

        if(DPLASMA_WORKS)
            # save link with dependencies
            if (REQUIRED_FLAGS)
                set(DPLASMA_LIBRARIES_DEP "${REQUIRED_FLAGS};${REQUIRED_LIBS}")
            else()
                set(DPLASMA_LIBRARIES_DEP "${REQUIRED_LIBS}")
            endif()
            set(DPLASMA_LIBRARY_DIRS_DEP "${REQUIRED_LIBDIRS}")
            set(DPLASMA_INCLUDE_DIRS_DEP "${REQUIRED_INCDIRS}")
            set(DPLASMA_LINKER_FLAGS "${REQUIRED_LDFLAGS}")
            list(REMOVE_DUPLICATES DPLASMA_LIBRARY_DIRS_DEP)
            list(REMOVE_DUPLICATES DPLASMA_INCLUDE_DIRS_DEP)
            list(REMOVE_DUPLICATES DPLASMA_LINKER_FLAGS)
        else()
            if(NOT DPLASMA_FIND_QUIETLY)
                message(STATUS "Looking for dplasma : test of setup_parsec fails")
                message(STATUS "CMAKE_REQUIRED_LIBRARIES: ${CMAKE_REQUIRED_LIBRARIES}")
                message(STATUS "CMAKE_REQUIRED_INCLUDES: ${CMAKE_REQUIRED_INCLUDES}")
                message(STATUS "Check in CMakeFiles/CMakeError.log to figure out why it failed")
            endif()
        endif()
        set(CMAKE_REQUIRED_INCLUDES)
        set(CMAKE_REQUIRED_FLAGS)
        set(CMAKE_REQUIRED_LIBRARIES)
    endif(DPLASMA_LIBRARIES)

endif( (NOT PKG_CONFIG_EXECUTABLE) OR (PKG_CONFIG_EXECUTABLE AND NOT DPLASMA_FOUND) OR (DPLASMA_GIVEN_BY_USER) )

if (DPLASMA_LIBRARIES)
    if (DPLASMA_LIBRARY_DIRS)
        foreach(dir ${DPLASMA_LIBRARY_DIRS})
            if ("${dir}" MATCHES "dplasma")
                set(first_lib_path "${dir}")
            endif()
        endforeach()
    else()
        list(GET DPLASMA_LIBRARIES 0 first_lib)
        get_filename_component(first_lib_path "${first_lib}" PATH)
    endif()
    if (${first_lib_path} MATCHES "/lib(32|64)?$")
        string(REGEX REPLACE "/lib(32|64)?$" "" not_cached_dir "${first_lib_path}")
        set(DPLASMA_DIR_FOUND "${not_cached_dir}" CACHE PATH "Installation directory of DPLASMA library" FORCE)
    else()
        set(DPLASMA_DIR_FOUND "${first_lib_path}" CACHE PATH "Installation directory of DPLASMA library" FORCE)
    endif()
endif()

# check that DPLASMA has been found
# --------------------------------
include(FindPackageHandleStandardArgs)
if (PKG_CONFIG_EXECUTABLE AND DPLASMA_FOUND)
    find_package_handle_standard_args(DPLASMA DEFAULT_MSG
                                      DPLASMA_LIBRARIES)
else()
    find_package_handle_standard_args(DPLASMA DEFAULT_MSG
                                      DPLASMA_LIBRARIES
                                      DPLASMA_WORKS)
endif()

