# ------------------------------------------------------------------------------
# Part of the MQSS Project, under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
# ------------------------------------------------------------------------------

# set required cmake version
cmake_minimum_required(VERSION 3.19...3.30)

project(
  qdmi
  LANGUAGES C
  VERSION 1.0.0 # Beta 1
  DESCRIPTION "QDMI –– Quantum Device Management Interface")

# make scripts available to cmake
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(cmake/Cache.cmake)

# check if this is the master project or used via add_subdirectory
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  set(QDMI_MASTER_PROJECT ON)
else()
  set(QDMI_MASTER_PROJECT OFF)
endif()

option(QDMI_INSTALL "Generate installation instructions for QDMI"
       ${QDMI_MASTER_PROJECT})
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

# set the include directory for the build tree
set(QDMI_INCLUDE_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(QDMI_TEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/test")

# add main library code
if(QDMI_INSTALL)
  include(GNUInstallDirs)
  set(QDMI_CONFIG_INSTALL_DIR "${CMAKE_INSTALL_DATADIR}/cmake/qdmi")
  set(QDMI_INCLUDE_INSTALL_DIR "${CMAKE_INSTALL_INCLUDEDIR}/qdmi")
  set(QDMI_TARGETS_EXPORT_NAME "qdmi-targets")
  set(QDMI_CMAKE_CONFIG_TEMPLATE
      "${PROJECT_SOURCE_DIR}/cmake/qdmi-config.cmake.in")
  set(QDMI_CMAKE_CONFIG_DIR "${CMAKE_CURRENT_BINARY_DIR}")
  set(QDMI_CMAKE_VERSION_CONFIG_FILE
      "${QDMI_CMAKE_CONFIG_DIR}/qdmi-config-version.cmake")
  set(QDMI_CMAKE_PROJECT_CONFIG_FILE
      "${QDMI_CMAKE_CONFIG_DIR}/qdmi-config.cmake")
  set(QDMI_CMAKE_PROJECT_TARGETS_FILE
      "${QDMI_CMAKE_CONFIG_DIR}/qdmi-targets.cmake")
endif()

if(NOT TARGET project_warnings)
  # Use the warnings specified in CompilerWarnings.cmake
  add_library(project_warnings INTERFACE)

  # Standard compiler warnings
  include(${PROJECT_SOURCE_DIR}/cmake/CompilerWarnings.cmake)
  set_project_warnings(project_warnings)

  # Add alias
  add_library(qdmi::project_warnings ALIAS project_warnings)
endif()

if(NOT TARGET qdmi)
  # add main library code
  add_library(qdmi INTERFACE)

  # set include directories
  target_include_directories(
    qdmi INTERFACE $<BUILD_INTERFACE:${QDMI_INCLUDE_BUILD_DIR}>)

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

# Installation instructions for the main library
if(QDMI_INSTALL)
  include(CMakePackageConfigHelpers)
  configure_package_config_file(
    ${QDMI_CMAKE_CONFIG_TEMPLATE} ${QDMI_CMAKE_PROJECT_CONFIG_FILE}
    INSTALL_DESTINATION ${QDMI_CONFIG_INSTALL_DIR}
    NO_SET_AND_CHECK_MACRO NO_CHECK_REQUIRED_COMPONENTS_MACRO)
  write_basic_package_version_file(
    ${QDMI_CMAKE_VERSION_CONFIG_FILE}
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMinorVersion)

  install(FILES ${QDMI_CMAKE_PROJECT_CONFIG_FILE}
                ${QDMI_CMAKE_VERSION_CONFIG_FILE}
          DESTINATION ${QDMI_CONFIG_INSTALL_DIR})

  install(
    TARGETS qdmi
    EXPORT ${QDMI_TARGETS_EXPORT_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT QDMI_Runtime
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            COMPONENT QDMI_Runtime
            NAMELINK_COMPONENT QDMI_Development
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT QDMI_Development
    INCLUDES
    DESTINATION ${QDMI_INCLUDE_INSTALL_DIR})

  install(
    EXPORT ${QDMI_TARGETS_EXPORT_NAME}
    FILE ${QDMI_PROJECT_TARGETS_FILE}
    NAMESPACE qdmi::
    DESTINATION ${QDMI_CONFIG_INSTALL_DIR}
    COMPONENT QDMI_Development)
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
  add_subdirectory(templates/device)
endif()
