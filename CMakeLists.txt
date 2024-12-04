# ------------------------------------------------------------------------------
# Copyright 2024 Munich Quantum Software Stack Project
#
# Licensed under the Apache License, Version 2.0 with LLVM Exceptions (the
# "License"); you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# https://github.com/Munich-Quantum-Software-Stack/QDMI/blob/develop/LICENSE
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations under
# the License.
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
# ------------------------------------------------------------------------------

# set required cmake version
cmake_minimum_required(VERSION 3.19...3.30)

project(
  qdmi
  LANGUAGES C
  VERSION 1.0.0
  DESCRIPTION "QDMI –– Quantum Device Management Interface")

# set some important directories
set(QDMI_INCLUDE_BUILD_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    CACHE INTERNAL "Include directory")
set(QDMI_TEST_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/test"
    CACHE INTERNAL "Test directory")
set(QDMI_CMAKE_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake"
    CACHE INTERNAL "CMake directory")

# make scripts available to cmake
list(APPEND CMAKE_MODULE_PATH ${QDMI_CMAKE_DIR})

include(cmake/Cache.cmake)

# check if this is the master project or used via add_subdirectory
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  set(QDMI_MASTER_PROJECT ON)
else()
  set(QDMI_MASTER_PROJECT OFF)
endif()

option(
  USE_INSTALLED_QDMI
  "Use an installed version of QDMI to build examples, templates, and tests"
  OFF)

if(USE_INSTALLED_QDMI)
  set(QDMI_INSTALL
      OFF
      CACHE BOOL "Generate installation instructions for QDMI" FORCE)
else()
  option(QDMI_INSTALL "Generate installation instructions for QDMI"
         ${QDMI_MASTER_PROJECT})
endif()
option(BUILD_QDMI_TESTS "Also build tests for the QDMI project"
       ${QDMI_MASTER_PROJECT})
option(BUILD_QDMI_EXAMPLES "Also build examples for the QDMI project"
       ${QDMI_MASTER_PROJECT})
option(BUILD_QDMI_DOCS "Also build documentation for the QDMI project"
       ${QDMI_MASTER_PROJECT})
option(BUILD_QDMI_TEMPLATES "Also build templates for the QDMI project"
       ${QDMI_MASTER_PROJECT})

# enable organization of targets into folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Set a default build type if none was specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE
      Release
      CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui, ccmake
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
                                               "MinSizeRel" "RelWithDebInfo")
endif()

# Generate compile_commands.json to make it easier to work with clang-based
# tools
set(CMAKE_EXPORT_COMPILE_COMMANDS
    ON
    CACHE BOOL "Export compile commands" FORCE)

# Require C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_C_EXTENSIONS False)

include(cmake/ExternalDependencies.cmake)
include(cmake/PrefixHandling.cmake)
include(GNUInstallDirs)

# add warnings target
if(NOT TARGET qdmi::project_warnings)
  # use the warnings specified in CompilerWarnings.cmake
  add_library(qdmi_project_warnings INTERFACE)

  # standard compiler warnings
  include(${PROJECT_SOURCE_DIR}/cmake/CompilerWarnings.cmake)
  set_project_warnings(qdmi_project_warnings)

  # add alias
  add_library(qdmi::project_warnings ALIAS qdmi_project_warnings)
endif()

# add main library code
if(NOT TARGET qdmi::qdmi)
  # add main library code
  add_library(qdmi INTERFACE)

  # set include directories
  target_include_directories(
    qdmi INTERFACE $<BUILD_INTERFACE:${QDMI_INCLUDE_BUILD_DIR}>
                   $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

  # set required C standard
  target_compile_features(qdmi INTERFACE c_std_11)

  # always include debug symbols (avoids common problems with LTO)
  target_compile_options(qdmi INTERFACE -g)

  # enable coverage collection options
  option(ENABLE_COVERAGE "Enable coverage reporting" FALSE)
  if(ENABLE_COVERAGE)
    target_compile_options(qdmi INTERFACE --coverage -fprofile-arcs
                                          -ftest-coverage -O0)
    target_link_libraries(qdmi INTERFACE gcov --coverage)
  endif()

  # set the version of the library
  set_target_properties(
    qdmi PROPERTIES VERSION ${PROJECT_VERSION}
                    SOVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR})

  # add alias
  add_library(qdmi::qdmi ALIAS qdmi)
endif()

# add documentation
if(BUILD_QDMI_DOCS)
  add_subdirectory(docs)
endif()

# add examples (used by tests)
if(BUILD_QDMI_EXAMPLES OR BUILD_QDMI_TESTS)
  add_subdirectory(examples)
endif()

# add test code
if(BUILD_QDMI_TESTS)
  enable_testing()
  include(GoogleTest)
  # include the tests itself
  add_subdirectory(test)
endif()

# add templates
if(BUILD_QDMI_TEMPLATES OR BUILD_QDMI_TESTS)
  # if testing is enabled, also enable building the tests in the templates
  set(BUILD_MY_DEVICE_TESTS
      ${BUILD_QDMI_TESTS}
      CACHE BOOL "Build tests for MyDevice" FORCE)
  add_subdirectory(templates)
endif()

# Installation instructions for the main library
if(QDMI_INSTALL)
  install(
    TARGETS qdmi
    EXPORT ${PROJECT_NAME}-targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Development
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            COMPONENT Runtime
            NAMELINK_COMPONENT Development
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Runtime
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})

  include(CMakePackageConfigHelpers)
  write_basic_package_version_file(
    "${PROJECT_NAME}-config-version.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion)
  configure_package_config_file(
    "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}-config.cmake.in"
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

  install(
    EXPORT ${PROJECT_NAME}-targets
    FILE ${PROJECT_NAME}-targets.cmake
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

  install(FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
                "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake"
          DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

  install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/qdmi DESTINATION include)
endif()
