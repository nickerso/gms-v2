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

# The version number.
set(GMS_VERSION_MAJOR 2)
set(GMS_VERSION_MINOR 0)
set(GMS_VERSION_PATCH 0)

foreach(VERSION_PART GMS_VERSION_MAJOR GMS_VERSION_MINOR GMS_VERSION_PATCH)
  string(LENGTH ${${VERSION_PART}} VERSION_PART_LENGTH)
  if(VERSION_PART_LENGTH EQUAL 1)
    set(${VERSION_PART}_PAD 0)
  endif()
endforeach()
set(GMS_VERSION 0x${GMS_VERSION_MAJOR_PAD}${GMS_VERSION_MAJOR}${GMS_VERSION_MINOR_PAD}${GMS_VERSION_MINOR}${GMS_VERSION_PATCH_PAD}${GMS_VERSION_PATCH})
set(GMS_VERSION_STRING "${GMS_VERSION_MAJOR}.${GMS_VERSION_MINOR}.${GMS_VERSION_PATCH}")

# this only works for C files
#INCLUDE (CheckIncludeFiles)
# usage: CHECK_INCLUDE_FILES (<header> <RESULT_VARIABLE> )
#CHECK_INCLUDE_FILES ("codecvt" GMS_HAVE_STD_CODECVT)

INCLUDE (CheckCXXSourceCompiles)
check_cxx_source_compiles("
  #include <codecvt>
  int main() {
    std::codecvt_utf8_utf16<wchar_t> x;
    return 0;
  }
" GMS_HAVE_STD_CODECVT)

set(GMS_CONFIG_H "${CMAKE_CURRENT_BINARY_DIR}/gms_config.h")
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/configure/gms_config.h.in"
  ${GMS_CONFIG_H}
)

find_package(csim CONFIG CONFIGS csim-targets.cmake)
find_package(MHD REQUIRED)
find_package(CURL REQUIRED)
find_package(OAUTH REQUIRED)

if( ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" )
  #SET(PLATFORM_LIBS ${PLATFORM_LIBS} "dl" "pthread" "tinfo" "z")
elseif( ${CMAKE_SYSTEM_NAME} STREQUAL "Darwin" )
  #SET(PLATFORM_LIBS ${PLATFORM_LIBS} "ncurses" "z")
endif()

set(SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/version.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gms.cpp
)

set(HEADER_FILES
  ${GMS_CONFIG_H}
)

add_executable(${GMS_EXECUTABLE_NAME}
  ${SOURCE_FILES}
)

target_compile_definitions(${GMS_EXECUTABLE_NAME}
    PRIVATE
    ${PLATFORM_DEFINES}
)

target_include_directories(${GMS_EXECUTABLE_NAME}
  PUBLIC
  $<INSTALL_INTERFACE:include/>
  PRIVATE
  ${CMAKE_CURRENT_BINARY_DIR}
  ${MHD_INCLUDE_DIRS}
  ${CURL_INCLUDE_DIRS}
)

target_link_libraries(${GMS_EXECUTABLE_NAME}
    PRIVATE
    csim
    ${MHD_LIBRARIES}
    ${CURL_LIBRARIES}
)

set_target_properties(${GMS_EXECUTABLE_NAME} PROPERTIES
    OUTPUT_NAME ${GMS_EXECUTABLE_NAME}
    VERSION ${GMS_VERSION_STRING}
)

set_target_properties(${GMS_EXECUTABLE_NAME} PROPERTIES CXX_STANDARD 11 CXX_STANDARD_REQUIRED ON)

if(GMS_TREAT_WARNINGS_AS_ERRORS)
  target_warnings_as_errors(${GMS_EXECUTABLE_NAME})
endif()

install(TARGETS ${GMS_EXECUTABLE_NAME}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

set(CPACK_PACKAGE_VERSION_MAJOR "${GMS_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${GMS_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${GMS_VERSION_PATCH}")

if(WIN32)
  set(CPACK_GENERATOR ZIP)
else()
  set(CPACK_GENERATOR TGZ)
endif()

include(CPack)
