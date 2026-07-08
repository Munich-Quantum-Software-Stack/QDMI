# Copyright (c) 2024 - 2026 QDMI Maintainers
# All rights reserved.
#
# Licensed under the Apache License v2.0 with LLVM Exceptions (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# https://llvm.org/LICENSE.txt
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations under
# the License.
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

# set required cmake version
cmake_minimum_required(VERSION 3.24...4.2)
project(
  qdmi
  LANGUAGES C CXX
  VERSION 1.3.2
  DESCRIPTION "QDMI –– Quantum Device Management Interface")

set(PROJECT_VERSION_PRERELEASE "")
set(PROJECT_VERSION_FULL "${PROJECT_VERSION}${PROJECT_VERSION_PRERELEASE}")

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

include(CMakeDependentOption)
cmake_dependent_option(
  INSTALL_QDMI "Generate installation instructions for QDMI"
  ${QDMI_MASTER_PROJECT} "NOT USE_INSTALLED_QDMI" OFF)
option(BUILD_QDMI_TESTS "Also build tests for the QDMI project"
       ${QDMI_MASTER_PROJECT})
option(BUILD_QDMI_EXAMPLES "Also build examples for the QDMI project"
       ${QDMI_MASTER_PROJECT})
option(BUILD_QDMI_DOCS "Also build documentation for the QDMI project" OFF)
option(BUILD_QDMI_TEMPLATES "Also build templates for the QDMI project"
       ${QDMI_MASTER_PROJECT})

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

if(NOT USE_INSTALLED_QDMI)
  # set some important directories
  set(QDMI_INCLUDE_BUILD_DIR
      "${CMAKE_CURRENT_SOURCE_DIR}/include"
      CACHE INTERNAL "Include directory")
  set(QDMI_CMAKE_DIR
      "${CMAKE_CURRENT_SOURCE_DIR}/cmake"
      CACHE INTERNAL "CMake directory")

  # make scripts available to cmake
  list(APPEND CMAKE_MODULE_PATH ${QDMI_CMAKE_DIR})

  include(cmake/Cache.cmake)

  include(cmake/PrefixHandling.cmake)
  include(GNUInstallDirs)

  # add warnings target
  if(NOT TARGET qdmi::qdmi_project_warnings)
    # use the warnings specified in CompilerWarnings.cmake
    add_library(qdmi_project_warnings INTERFACE)

    # standard compiler warnings
    include(${PROJECT_SOURCE_DIR}/cmake/CompilerWarnings.cmake)
    set_project_warnings(qdmi_project_warnings)

    # add alias
    add_library(qdmi::qdmi_project_warnings ALIAS qdmi_project_warnings)
  endif()

  # add main library code
  if(NOT TARGET qdmi::qdmi)
    # add main library code
    add_library(qdmi INTERFACE)

    # collect header files
    file(GLOB_RECURSE QDMI_HEADERS ${QDMI_INCLUDE_BUILD_DIR}/qdmi/*.h)

    # add headers using file sets
    target_sources(qdmi PUBLIC FILE_SET HEADERS BASE_DIRS
                               ${QDMI_INCLUDE_BUILD_DIR} FILES ${QDMI_HEADERS})

    # set required C standard
    target_compile_features(qdmi INTERFACE c_std_11)

    # always include debug symbols (avoids common problems with LTO)
    if(NOT MSVC)
      target_compile_options(qdmi INTERFACE -g)
    endif()

    # enable coverage collection options
    option(ENABLE_COVERAGE "Enable coverage reporting" FALSE)
    if(ENABLE_COVERAGE)
      add_library(qdmi_coverage_flags INTERFACE)
      target_compile_options(
        qdmi_coverage_flags INTERFACE --coverage -fprofile-arcs -ftest-coverage
                                      -O0)
      target_link_libraries(qdmi_coverage_flags INTERFACE gcov --coverage)
      add_library(qdmi::qdmi_coverage_flags ALIAS qdmi_coverage_flags)
      target_link_libraries(qdmi INTERFACE qdmi::qdmi_coverage_flags)
    endif()

    # set the version of the library
    set_target_properties(
      qdmi
      PROPERTIES VERSION ${PROJECT_VERSION}
                 SOVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR})

    # add alias
    add_library(qdmi::qdmi ALIAS qdmi)
  endif()
endif()

# add documentation
if(BUILD_QDMI_DOCS)
  add_subdirectory(docs)
endif()

# add examples (used by tests)
if(BUILD_QDMI_EXAMPLES OR BUILD_QDMI_TESTS)
  add_subdirectory(examples)
endif()

# add templates
if(BUILD_QDMI_TEMPLATES OR BUILD_QDMI_TESTS)
  # if testing is enabled, also enable building the tests in the templates
  set(BUILD_MY_QDMI_TESTS
      ${BUILD_QDMI_TESTS}
      CACHE BOOL "Build tests for my-qdmi-device" FORCE)
  add_subdirectory(templates)
endif()

# add test code
if(BUILD_QDMI_TESTS)
  enable_testing()
  include(GoogleTest)
  # include the tests itself
  add_subdirectory(test)
endif()

# Installation instructions for the main library
if(INSTALL_QDMI)
  set(QDMI_CONFIG_INSTALL_DIR
      "${CMAKE_INSTALL_DATADIR}/cmake/${PROJECT_NAME}"
      CACHE INTERNAL "Config install directory")

  include(CMakePackageConfigHelpers)
  configure_package_config_file(
    "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}-config.cmake.in"
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
    INSTALL_DESTINATION ${QDMI_CONFIG_INSTALL_DIR}
    NO_SET_AND_CHECK_MACRO NO_CHECK_REQUIRED_COMPONENTS_MACRO)
  write_basic_package_version_file(
    "${PROJECT_NAME}-config-version.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMinorVersion)

  install(
    FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
          "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake"
    DESTINATION ${QDMI_CONFIG_INSTALL_DIR}
    COMPONENT ${PROJECT_NAME}_Development)

  set(QDMI_INSTALL_TARGETS ${QDMI_INSTALL_TARGETS} qdmi qdmi_project_warnings)
  if(ENABLE_COVERAGE)
    list(APPEND QDMI_INSTALL_TARGETS qdmi_coverage_flags)
  endif()

  # FILE_SET should be aligned with LIBRARY and ARCHIVE, so we turn cmake-format
  # off
  # cmake-format: off
  install(
    TARGETS ${QDMI_INSTALL_TARGETS}
    EXPORT ${PROJECT_NAME}-targets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT ${PROJECT_NAME}_Runtime
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            COMPONENT ${PROJECT_NAME}_Runtime
            NAMELINK_COMPONENT ${PROJECT_NAME}_Development
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT ${PROJECT_NAME}_Development
    FILE_SET HEADERS
             DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
             COMPONENT ${PROJECT_NAME}_Development)
  # cmake-format: on

  install(
    EXPORT ${PROJECT_NAME}-targets
    FILE ${PROJECT_NAME}-targets.cmake
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION ${QDMI_CONFIG_INSTALL_DIR}
    COMPONENT ${PROJECT_NAME}_Development)

  install(
    FILES ${PROJECT_SOURCE_DIR}/cmake/Cache.cmake
          ${PROJECT_SOURCE_DIR}/cmake/prefix_defs.txt
          ${PROJECT_SOURCE_DIR}/cmake/PrefixHandling.cmake
          ${PROJECT_SOURCE_DIR}/cmake/test_defs.cpp.in
    DESTINATION ${QDMI_CONFIG_INSTALL_DIR}
    COMPONENT ${PROJECT_NAME}_Development)
endif()
