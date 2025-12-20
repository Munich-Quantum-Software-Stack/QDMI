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

# A function for generating prefixed QDMI headers for a user-defined prefix.
#
# Arguments: PREFIX - The prefix for the device (required)
#
# Usage: generate_prefixed_qdmi_headers("MY")
function(generate_prefixed_qdmi_headers prefix)
  # Get the lowercase version of the prefix.
  string(TOLOWER ${prefix} QDMI_prefix)

  # Determine the correct include directory
  set(QDMI_INCLUDE_DIR "${QDMI_INCLUDE_BUILD_DIR}")
  if(NOT QDMI_INCLUDE_DIR)
    set(QDMI_INCLUDE_DIR "${qdmi_INCLUDE_DIR}")
  endif()

  # Get the list of all QDMI device headers.
  file(GLOB_RECURSE QDMI_DEVICE_HEADERS ${QDMI_INCLUDE_DIR}/qdmi/device.h
       ${QDMI_INCLUDE_DIR}/qdmi/types.h)

  # Determine the correct CMake directory for prefix_defs.txt
  set(QDMI_PREFIX_DIR "${QDMI_CMAKE_DIR}")
  if(NOT QDMI_PREFIX_DIR)
    set(QDMI_PREFIX_DIR "${qdmi_CMAKE_DIR}")
  endif()

  # Read the prefix definitions.
  file(READ ${QDMI_PREFIX_DIR}/prefix_defs.txt replacements)
  string(REPLACE "\n" ";" replacements "${replacements}")
  foreach(header ${QDMI_DEVICE_HEADERS})
    # Get the relative path of the header.
    file(RELATIVE_PATH rel_header ${QDMI_INCLUDE_DIR}/qdmi ${header})
    get_filename_component(rel_dir ${rel_header} DIRECTORY)
    # Create the directory for the prefixed header.
    file(MAKE_DIRECTORY
         ${CMAKE_CURRENT_BINARY_DIR}/include/${QDMI_prefix}_qdmi/${rel_dir})
    # Read the header content.
    file(READ ${header} header_content)
    # Replace the include for the device header with the prefixed version.
    string(
      REGEX
      REPLACE "#include (\"|<)qdmi/(device|types).h(\"|>)"
              "#include \\1${QDMI_prefix}_qdmi/\\2.h\\3" header_content
              "${header_content}")
    # Replace the prefix definitions.
    foreach(replacement ${replacements})
      string(
        REGEX
        REPLACE "([^a-zA-Z0-9_])${replacement}([^a-zA-Z0-9_])"
                "\\1${prefix}_${replacement}\\2" header_content
                "${header_content}")
    endforeach()
    # Write the prefixed header.
    file(WRITE
         ${CMAKE_CURRENT_BINARY_DIR}/include/${QDMI_prefix}_qdmi/${rel_header}
         "${header_content}")
  endforeach()
endfunction()

# A function for generating test executables that check if all functions are
# implemented by a device.
#
# NOTE: The executables are not meant to be executed, only built.
#
# Arguments: PREFIX - The prefix for the device (required) TARGET - The device
# target to link against (optional, defaults to qdmi::${prefix}_device)
#
# Usage: generate_device_defs_executable("MY")  # Links against qdmi::my_device
# generate_device_defs_executable("MY" TARGET my_custom_device)  # Links against
# my_custom_device
function(generate_device_defs_executable prefix)
  set(QDMI_PREFIX "${prefix}")
  # Get the lowercase version of the prefix.
  string(TOLOWER ${QDMI_PREFIX} QDMI_prefix)

  # Parse arguments
  set(oneValueArgs TARGET)
  cmake_parse_arguments(ARG "" "${oneValueArgs}" "" ${ARGN})

  # Use provided target or default to qdmi::${QDMI_prefix}_device
  if(ARG_TARGET)
    set(DEVICE_TARGET ${ARG_TARGET})
  else()
    set(DEVICE_TARGET qdmi::${QDMI_prefix}_device)
  endif()

  # Determine the correct CMake directory for prefix_defs.txt
  set(QDMI_PREFIX_DIR "${QDMI_CMAKE_DIR}")
  if(NOT QDMI_PREFIX_DIR)
    set(QDMI_PREFIX_DIR "${qdmi_CMAKE_DIR}")
  endif()

  # Create the test definitions file.
  configure_file(${QDMI_PREFIX_DIR}/test_defs.cpp.in
                 ${CMAKE_CURRENT_BINARY_DIR}/${QDMI_prefix}_test_defs.cpp @ONLY)
  # Create the test executable.
  add_executable(qdmi_test_${QDMI_prefix}_device_defs
                 ${CMAKE_CURRENT_BINARY_DIR}/${QDMI_prefix}_test_defs.cpp)
  target_link_libraries(
    qdmi_test_${QDMI_prefix}_device_defs PRIVATE qdmi::qdmi ${DEVICE_TARGET}
                                                 qdmi::qdmi_project_warnings)
  target_compile_features(qdmi_test_${QDMI_prefix}_device_defs
                          PRIVATE cxx_std_17)
endfunction()
