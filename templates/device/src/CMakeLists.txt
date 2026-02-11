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

include(GNUInstallDirs)

# Generate QDMI headers with the specified prefix
generate_prefixed_qdmi_headers(${QDMI_PREFIX})

# Collect source files
file(GLOB_RECURSE SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
# cmake-format: off
# TODO: Uncomment the following line if you have private headers to include
# file(GLOB_RECURSE PRIVATE_HEADER_FILES "${PROJECT_SOURCE_DIR}/include/*.hpp")
# cmake-format: on
file(GLOB_RECURSE PUBLIC_HEADER_FILES "${CMAKE_CURRENT_BINARY_DIR}/include/*.h")

add_library(${QDMI_TARGET_NAME} SHARED)
target_compile_features(${QDMI_TARGET_NAME} PRIVATE cxx_std_20)
get_target_property(qdmi_library_VERSION qdmi::qdmi VERSION)
target_compile_definitions(
  ${QDMI_TARGET_NAME} PRIVATE QDMI_VERSION="${qdmi_library_VERSION}"
                              MY_QDMI_DEVICE_VERSION="${PROJECT_VERSION}")
target_sources(${QDMI_TARGET_NAME} PRIVATE ${SRC_FILES})

target_sources(
  ${QDMI_TARGET_NAME}
  # cmake-format: off
  # TODO: Uncomment the following block if you have private headers to include
  #    PRIVATE FILE_SET
  #    private_headers
  #    TYPE
  #    HEADERS
  #    BASE_DIRS
  #    ${PROJECT_SOURCE_DIR}/include
  #    FILES
  #    ${PRIVATE_HEADER_FILES}
  # cmake-format: on
  PUBLIC FILE_SET
         public_headers
         TYPE
         HEADERS
         BASE_DIRS
         ${CMAKE_CURRENT_BINARY_DIR}/include
         FILES
         ${PUBLIC_HEADER_FILES})

target_link_libraries(${QDMI_TARGET_NAME} PRIVATE qdmi::qdmi_project_warnings)

option(MY_QDMI_COVERAGE "Enable coverage instrumentation" OFF)

if(MY_QDMI_COVERAGE)
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    message(STATUS "Building with coverage instrumentation")
    target_compile_options(${QDMI_TARGET_NAME} PUBLIC --coverage -O0 -g)
    target_link_options(${QDMI_TARGET_NAME} PUBLIC --coverage)
  else()
    message(WARNING "Coverage instrumentation is only supported with GCC/Clang")
  endif()
endif()

set(MY_QDMI_CONFIG_INSTALL_DIR
    "${CMAKE_INSTALL_DATADIR}/cmake/${QDMI_TARGET_NAME}"
    CACHE INTERNAL "Config install directory")

include(CMakePackageConfigHelpers)
configure_package_config_file(
  "${PROJECT_SOURCE_DIR}/cmake/${QDMI_TARGET_NAME}-config.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/${QDMI_TARGET_NAME}-config.cmake"
  INSTALL_DESTINATION ${MY_QDMI_CONFIG_INSTALL_DIR}
  NO_SET_AND_CHECK_MACRO NO_CHECK_REQUIRED_COMPONENTS_MACRO)
write_basic_package_version_file(
  "${QDMI_TARGET_NAME}-config-version.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMinorVersion)

if(INSTALL_MY_QDMI_DEVICE)
  install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/${QDMI_TARGET_NAME}-config.cmake"
          "${CMAKE_CURRENT_BINARY_DIR}/${QDMI_TARGET_NAME}-config-version.cmake"
    DESTINATION ${MY_QDMI_CONFIG_INSTALL_DIR})

  # cmake-format: off
  install(
      TARGETS ${QDMI_TARGET_NAME}
      EXPORT ${QDMI_TARGET_NAME}-targets
      RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT ${QDMI_TARGET_NAME}_Runtime
      LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
              COMPONENT ${QDMI_TARGET_NAME}_Runtime
              NAMELINK_COMPONENT ${QDMI_TARGET_NAME}_Development
      ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT ${QDMI_TARGET_NAME}_Development
      FILE_SET public_headers
               DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
               COMPONENT ${QDMI_TARGET_NAME}_Development)
  # cmake-format: on

  install(
    EXPORT ${QDMI_TARGET_NAME}-targets
    FILE ${QDMI_TARGET_NAME}-targets.cmake
    DESTINATION ${MY_QDMI_CONFIG_INSTALL_DIR}
    COMPONENT ${QDMI_TARGET_NAME}_Development)
endif()
