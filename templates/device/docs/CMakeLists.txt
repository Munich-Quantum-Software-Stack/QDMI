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

# Set the Doxygen configuration file. In the Doxyfile.in file, there are some
# placeholders that are replaced by CMake. That is also the reason for the
# extension `.in`. For details see below for the command `configure_file`.
set(DOXYGEN_CONFIG_FILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)

# Set the input directories for doxygen, i.e., all files that contain source
# code with documentation.
set(DOXYGEN_INPUT_DIRS ${PROJECT_SOURCE_DIR}/include
                       ${PROJECT_BINARY_DIR}/src/include/my_qdmi)

# This variable is only needed to setup the dependency tracking of CMake
# correctly and is not passed to Doxygen. Whenever such a file is modified,
# CMake detects that change and will rerun the Doxygen command instead of using
# the cached result when the target `docs` is built.

file(GLOB_RECURSE DOXYGEN_INPUT_FILES ${PROJECT_SOURCE_DIR}/include/*.hpp
     ${PROJECT_BINARY_DIR}/src/include/my_qdmi/device.h
     ${CMAKE_CURRENT_SOURCE_DIR}/*.md)
# After the variable above is correctly initialized, we can add the additional
# files that are not source code but still part of the documentation. IMPORTANT:
# The order the files with the static content are added here influences their
# order in the generated documentation.
set(DOXYGEN_INPUT_DIRS
    ${DOXYGEN_INPUT_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/index.md
    ${CMAKE_CURRENT_SOURCE_DIR}/development_guide.md
    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies.md)
# By default the files are separated by a semicolon. This is not what Doxygen
# wants, so we replace it with a space.
string(REPLACE ";" " " DOXYGEN_INPUT_DIRS "${DOXYGEN_INPUT_DIRS}")

# Set the output directory for the generated documentation: Place it in the
# CMake build directory.
set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR})

# Set the Doxygen configured configuration file. This will point to the Doxyfile
# that is written out by CMake with all placeholders substituted.
set(DOXYGEN_CONFIG_FILE_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

# Save the location the files of the project doxygen-awesome-css were cloned
# into. This allows us to get the path to doxygen-awesome.css. For its usage
# also see the occurrences of @AWESOME_CSS_DIR@ in the Doxyfile.in.
FetchContent_GetProperties(doxygen-awesome-css SOURCE_DIR AWESOME_CSS_DIR)

# Ensure the output directory exists
file(MAKE_DIRECTORY ${DOXYGEN_OUTPUT_DIR}/html)

# Copy over the dark mode logos as they are not automatically copied by Doxygen.
# `configure_file` is used here because `file(COPY ...)` does not detect changes
# and copies the files again after they have changed.
configure_file(_static/my_logo_dark.svg ${DOXYGEN_OUTPUT_DIR}/html COPYONLY)
# Set the logo for the project.
set(PROJECT_LOGO "my_logo.svg")

# Replace variables inside @@ with the current values, e.g., the placeholder
# @DOXYGEN_INPUT_DIRS@ is replaced by the content of the variable
# DOXYGEN_INPUT_DIRS.
configure_file(${DOXYGEN_CONFIG_FILE_IN} ${DOXYGEN_CONFIG_FILE_OUT} @ONLY)

if(${DOXYGEN_EXECUTABLE} STREQUAL "DOXYGEN_EXECUTABLE-NOTFOUND")
  set(DOXYGEN_EXECUTABLE ${doxygen_BINARY_DIR}/bin/doxygen)
  message(STATUS "Setting doxygen binary manually to ${DOXYGEN_EXECUTABLE}")
endif()

# Create a custom command to run Doxygen
add_custom_command(
  OUTPUT ${DOXYGEN_OUTPUT_DIR}/html/index.html
  DEPENDS ${DOXYGEN_INPUT_FILES}
          ${DOXYGEN_CONFIG_FILE_IN}
          ${CMAKE_CURRENT_SOURCE_DIR}/header.html
          ${CMAKE_CURRENT_SOURCE_DIR}/layout.xml
          ${CMAKE_CURRENT_SOURCE_DIR}/style.css
          ${CMAKE_SOURCE_DIR}/README.md
          $<IF:$<TARGET_EXISTS:doxygen>,doxygen,>
  COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_CONFIG_FILE_OUT}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  COMMENT "Generating API documentation with Doxygen"
  VERBATIM)

# Create a custom target
add_custom_target(my_qdmi_device_docs ALL
                  DEPENDS ${DOXYGEN_OUTPUT_DIR}/html/index.html)
