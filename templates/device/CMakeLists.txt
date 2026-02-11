# Copyright (c) 2024 - 2025 Munich Quantum Software Stack Project
# All rights reserved.
#
# Licensed under the Apache License v2.0 with LLVM Exceptions (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# https://github.com/Munich-Quantum-Software-Stack/QDMI/blob/develop/LICENSE.md
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations under
# the License.
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

cmake_minimum_required(VERSION 3.24...4.2)

project(
  my-qdmi-device
  LANGUAGES C CXX
  VERSION 0.1.0
  DESCRIPTION "MY QDMI Device Implementation")

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE
      Release
      CACHE STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
                                               "MinSizeRel" "RelWithDebInfo")
endif()

option(MY_QDMI_DEVICE_INSTALL "Generate installation instructions for MY QDMI Device" OFF)

option(USE_INSTALLED_MY_QDMI_DEVICE
       "Use an installed version of the MY QDMI Device to build tests" OFF)

if(USE_INSTALLED_MY_QDMI_DEVICE)
  find_package(my-qdmi-device REQUIRED ${PROJECT_VERSION})
endif()

# Generate compile_commands.json to make it easier to work with clang based
# tools
set(CMAKE_EXPORT_COMPILE_COMMANDS
    ON
    CACHE BOOL "Export compile commands" FORCE)

set(CMAKE_VERIFY_INTERFACE_HEADER_SETS
    ON
    CACHE BOOL "Verify interface header sets" FORCE)

set(QDMI_PREFIX "MY")

# check if this is the master project or used via add_subdirectory
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  set(MY_QDMI_MASTER_PROJECT ON)
else()
  set(MY_QDMI_MASTER_PROJECT OFF)
endif()

option(BUILD_MY_QDMI_TESTS "Build tests for the MY QDMI device"
       ${MY_QDMI_MASTER_PROJECT})
option(BUILD_MY_QDMI_DOCS "Build documentation for the MY QDMI device" OFF)

# Manage external dependencies
include(cmake/ExternalDependencies.cmake)

# Set target name
set(QDMI_TARGET_NAME "my-qdmi-device")

# Add the main library code
if(NOT USE_INSTALLED_MY_QDMI_DEVICE)
  add_subdirectory(src)
endif()

# Add the documentation
if(BUILD_MY_QDMI_DOCS)
  add_subdirectory(docs)
endif()

# Add the tests
if(BUILD_MY_QDMI_TESTS)
  enable_testing()
  include(GoogleTest)
  add_subdirectory(test)
endif()

# Installation instructions for the main library
if(MY_QDMI_DEVICE_INSTALL)
  set(MY_QDMI_DEVICE_CONFIG_INSTALL_DIR
          "${CMAKE_INSTALL_DATADIR}/cmake/${PROJECT_NAME}"
          CACHE INTERNAL "Config install directory")

  include(CMakePackageConfigHelpers)
  configure_package_config_file(
          "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}-config.cmake.in"
          "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
          INSTALL_DESTINATION ${MY_QDMI_DEVICE_CONFIG_INSTALL_DIR}
          NO_SET_AND_CHECK_MACRO NO_CHECK_REQUIRED_COMPONENTS_MACRO)
  write_basic_package_version_file(
          "${PROJECT_NAME}-config-version.cmake"
          VERSION ${PROJECT_VERSION}
          COMPATIBILITY SameMinorVersion)

  install(FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
          "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake"
          DESTINATION ${MY_QDMI_DEVICE_CONFIG_INSTALL_DIR})

  # FILE_SET should be aligned with LIBRARY and ARCHIVE, so we turn cmake-format
  # off
  # cmake-format: off
  install(
          TARGETS cxx-qdmi-device
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
          DESTINATION ${MY_QDMI_DEVICE_CONFIG_INSTALL_DIR}
          COMPONENT ${PROJECT_NAME}_Development)

  install(
          FILES ${PROJECT_SOURCE_DIR}/cmake/Cache.cmake
          ${PROJECT_SOURCE_DIR}/cmake/prefix_defs.txt
          ${PROJECT_SOURCE_DIR}/cmake/PrefixHandling.cmake
          ${PROJECT_SOURCE_DIR}/cmake/test_defs.cpp.in
          DESTINATION ${MY_QDMI_DEVICE_CONFIG_INSTALL_DIR}
          COMPONENT ${PROJECT_NAME}_Development)
endif()
