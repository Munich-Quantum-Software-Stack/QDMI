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

include(GNUInstallDirs)

# Define the target name for the device library
set(QDMI_PREFIX "CXX")

# Set target name
set(QDMI_TARGET_NAME "cxx-qdmi-device")

# Generate QDMI headers with the specified prefix
generate_prefixed_qdmi_headers(${QDMI_PREFIX})

# Collect source files
file(GLOB_RECURSE SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE PUBLIC_HEADER_FILES "${CMAKE_CURRENT_BINARY_DIR}/include/*.h")

add_library(${QDMI_TARGET_NAME} SHARED)
target_compile_features(${QDMI_TARGET_NAME} PRIVATE cxx_std_20)
get_target_property(qdmi_library_VERSION qdmi::qdmi VERSION)
target_compile_definitions(
  ${QDMI_TARGET_NAME}
  PRIVATE QDMI_VERSION="${qdmi_library_VERSION}"
          ${QDMI_PREFIX}_QDMI_DEVICE_VERSION="${PROJECT_VERSION}"
          ${QDMI_PREFIX}_QDMI_device_EXPORTS)
set_target_properties(
  ${QDMI_TARGET_NAME}
  PROPERTIES C_VISIBILITY_PRESET hidden
             CXX_VISIBILITY_PRESET hidden
             VISIBILITY_INLINES_HIDDEN 1)
target_sources(${QDMI_TARGET_NAME} PRIVATE ${SRC_FILES})

target_sources(
  ${QDMI_TARGET_NAME}
  PUBLIC FILE_SET
         public_headers
         TYPE
         HEADERS
         BASE_DIRS
         ${CMAKE_CURRENT_BINARY_DIR}/include
         FILES
         ${PUBLIC_HEADER_FILES})

target_link_libraries(${QDMI_TARGET_NAME} PRIVATE qdmi::qdmi_project_warnings)

option(QDMI_ENABLE_COVERAGE "Enable coverage reporting" OFF)
if(QDMI_ENABLE_COVERAGE)
  target_link_libraries(${QDMI_TARGET_NAME} PRIVATE qdmi::qdmi_coverage_flags)
endif()
