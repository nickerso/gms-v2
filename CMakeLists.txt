# Copyright 2015 University of Auckland
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.cmake_minimum_required (VERSION 3.1)


cmake_minimum_required(VERSION 3.2)

set(PROJECT_NAME GMS_v2)
project(${PROJECT_NAME} CXX)

set(GMS_EXECUTABLE_NAME "gms")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(common)

# Use the following variables when configuring the build from the command line to
# set the corresponding cache variables.

# Documentation URL
set(_PARAM_ANNOTATION "GMS documentation URL to include in the server.")
set( GMS_DOCUMENTATION_URL "http://get.readthedocs.org/" CACHE STRING ${_PARAM_ANNOTATION} )
if( DOC_URL )
  set( GMS_DOCUMENTATION_URL "${DOC_URL}" CACHE STRING ${_PARAM_ANNOTATION} FORCE )
endif()


# COVERAGE ==> GMS_COVERAGE
set(_PARAM_ANNOTATION "Enable coverage testing.")
if(CMAKE_COMPILER_IS_GNUCXX)
  set( GMS_COVERAGE OFF CACHE BOOL ${_PARAM_ANNOTATION} )
endif()
if( COVERAGE AND CMAKE_COMPILER_IS_GNUCXX )
  set( GMS_COVERAGE "${COVERAGE}" CACHE BOOL ${_PARAM_ANNOTATION} FORCE )
endif()

# MEMCHECK ==> GMS_MEMCHECK
set(_PARAM_ANNOTATION "Enable memcheck testing.")
if(CMAKE_COMPILER_IS_GNUCXX)
  set( GMS_MEMCHECK OFF CACHE BOOL ${_PARAM_ANNOTATION} )
endif()
if( MEMCHECK AND CMAKE_COMPILER_IS_GNUCXX )
  set( GMS_MEMCHECK "${MEMCHECK}" CACHE BOOL ${_PARAM_ANNOTATION} FORCE )
endif()

# UNIT_TESTS ==> GMS_UNIT_TESTS
set(_PARAM_ANNOTATION "Enable build of automated GMS tests.")
set( GMS_UNIT_TESTS ON CACHE BOOL ${_PARAM_ANNOTATION} )
if( UNIT_TESTS )
  set( GMS_UNIT_TESTS "${UNIT_TESTS}" CACHE BOOL ${_PARAM_ANNOTATION} FORCE )
endif()

if(GMS_COVERAGE OR GMS_MEMCHECK)
  set(GMS_UNIT_TESTS ON CACHE BOOL ${_PARAM_ANNOTATION} FORCE)
endif()

# TWAE ==> GMS_WARNINGS_AS_ERRORS -- Note: This excludes third party code, where warnings are never treated as errors.
set(_PARAM_ANNOTATION "Treat warnings as errors (note: warnings in 3rd party code are never treated as errors)")
set( GMS_TREAT_WARNINGS_AS_ERRORS OFF CACHE BOOL ${_PARAM_ANNOTATION} )
if( TWAE )
  set( GMS_TREAT_WARNINGS_AS_ERRORS "${TWAE}" CACHE BOOL ${_PARAM_ANNOTATION} FORCE )
endif()

# BUILD_TYPE ==> GMS_BUILD_TYPE
set(_PARAM_ANNOTATION "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel.")
set( GMS_BUILD_TYPE "Release" CACHE STRING ${_PARAM_ANNOTATION} )
if( BUILD_TYPE )
  set( GMS_BUILD_TYPE ${BUILD_TYPE} CACHE STRING ${_PARAM_ANNOTATION} FORCE )
endif()
if( GMS_MEMCHECK OR GMS_COVERAGE)
  set( GMS_BUILD_TYPE "Debug" CACHE STRING ${_PARAM_ANNOTATION} FORCE )
endif()

# INSTALL_PREFIX ==> GMS_INSTALL_PREFIX
set(_PARAM_ANNOTATION "Install path prefix, prepended onto install directories.")
set( GMS_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX} CACHE PATH ${_PARAM_ANNOTATION} )
if( INSTALL_PREFIX )
  set( GMS_INSTALL_PREFIX ${INSTALL_PREFIX} CACHE PATH ${_PARAM_ANNOTATION} FORCE )
endif()

# BUILD_SHARED ==> GMS_BUILD_SHARED
set(_PARAM_ANNOTATION "Build shared libraries (so, dylib, DLLs).")
set( GMS_BUILD_SHARED OFF CACHE BOOL ${_PARAM_ANNOTATION} )
if( BUILD_SHARED )
  set( GMS_BUILD_SHARED ${BUILD_SHARED} CACHE BOOL ${_PARAM_ANNOTATION} FORCE )
endif()

# GMS_DEPENDENCY_DIR
set(_PARAM_ANNOTATION "Dependency prefix (where to find non-system dependencies)")
set( GMS_DEPENDENCY_DIR "/usr/local" CACHE PATH ${_PARAM_ANNOTATION} )
if( DEPENDENCY_DIR )
  set( GMS_DEPENDENCY_DIR ${DEPENDENCY_DIR} CACHE PATH ${_PARAM_ANNOTATION} FORCE )
endif()

# Keep the GUI tidy.
internalise_cmake_variables()
hide_distracting_variables()

# GMS library target
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)

if (GMS_UNIT_TESTS)
  # enable testing here so that we can make use of the 'test' target
  enable_testing()
  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests)
endif()



