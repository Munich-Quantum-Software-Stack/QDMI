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

# A function for generating prefixed QDMI headers for a user-defined prefix.
function(generate_prefixed_qdmi_headers prefix)
  # Get the lowercase version of the prefix.
  string(TOLOWER ${prefix} QDMI_prefix)
  # Get the list of all QDMI device headers.
  file(GLOB_RECURSE QDMI_DEVICE_HEADERS ${QDMI_INCLUDE_BUILD_DIR}/qdmi/device.h
       ${QDMI_INCLUDE_BUILD_DIR}/qdmi/types.h)
  # Read the prefix definitions.
  file(READ ${QDMI_CMAKE_DIR}/prefix_defs.txt replacements)
  string(REPLACE "\n" ";" replacements "${replacements}")
  foreach(header ${QDMI_DEVICE_HEADERS})
    # Get the relative path of the header.
    file(RELATIVE_PATH rel_header ${QDMI_INCLUDE_BUILD_DIR}/qdmi ${header})
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
function(generate_device_defs_executable prefix)
  set(QDMI_PREFIX ${prefix})
  # Get the lowercase version of the prefix.
  string(TOLOWER ${prefix} QDMI_prefix)
  # Create the test definitions file.
  configure_file(${QDMI_TEST_DIR}/utils/test_defs.cpp.in
                 ${CMAKE_CURRENT_BINARY_DIR}/${QDMI_prefix}_test_defs.cpp @ONLY)
  # Create the test executable.
  add_executable(qdmi_test_${QDMI_prefix}_device_defs
                 ${CMAKE_CURRENT_BINARY_DIR}/${QDMI_prefix}_test_defs.cpp)
  target_link_libraries(
    qdmi_test_${QDMI_prefix}_device_defs
    PRIVATE qdmi::qdmi qdmi::${QDMI_prefix}_device qdmi::project_warnings)
  target_compile_features(qdmi_test_${QDMI_prefix}_device_defs
                          PRIVATE cxx_std_17)
endfunction()
