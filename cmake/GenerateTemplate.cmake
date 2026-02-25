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

# Script-mode generator for the QDMI device template.
#
# Expected variables (passed via -D...): - QDMI_TEMPLATE_SOURCE_DIR: path to
# templates/device - QDMI_TEMPLATE_OUTPUT_DIR: output directory to
# create/populate - QDMI_TEMPLATE_PREFIX: uppercase prefix, e.g. "ABC" -
# QDMI_TEMPLATE_FORCE: ON/OFF, allow overwriting conflicting files -
# QDMI_TEMPLATE_CLEAN: ON/OFF, remove the entire output dir before generating

cmake_minimum_required(VERSION 3.24)

foreach(var IN ITEMS QDMI_TEMPLATE_SOURCE_DIR QDMI_TEMPLATE_OUTPUT_DIR
                     QDMI_TEMPLATE_PREFIX)
  if(NOT DEFINED ${var} OR "${${var}}" STREQUAL "")
    message(FATAL_ERROR "[qdmi-template] Missing required -D${var}=...")
  endif()
endforeach()

set(QDMI_TEMPLATE_FORCE "${QDMI_TEMPLATE_FORCE}")
if("${QDMI_TEMPLATE_FORCE}" STREQUAL "")
  set(QDMI_TEMPLATE_FORCE OFF)
endif()

set(QDMI_TEMPLATE_CLEAN "${QDMI_TEMPLATE_CLEAN}")
if("${QDMI_TEMPLATE_CLEAN}" STREQUAL "")
  set(QDMI_TEMPLATE_CLEAN OFF)
endif()

# file(REAL_PATH ...) fails if the path doesn't (yet) exist on some
# platforms/CMakes. Use REAL_PATH only when the path exists; otherwise compute
# an absolute path which does not require the target to exist.
if(EXISTS "${QDMI_TEMPLATE_SOURCE_DIR}")
  file(REAL_PATH "${QDMI_TEMPLATE_SOURCE_DIR}" QDMI_TEMPLATE_SOURCE_DIR)
else()
  get_filename_component(QDMI_TEMPLATE_SOURCE_DIR "${QDMI_TEMPLATE_SOURCE_DIR}"
                         ABSOLUTE)
endif()

if(EXISTS "${QDMI_TEMPLATE_OUTPUT_DIR}")
  file(REAL_PATH "${QDMI_TEMPLATE_OUTPUT_DIR}" QDMI_TEMPLATE_OUTPUT_DIR)
else()
  get_filename_component(QDMI_TEMPLATE_OUTPUT_DIR "${QDMI_TEMPLATE_OUTPUT_DIR}"
                         ABSOLUTE)
endif()

string(TOLOWER "${QDMI_TEMPLATE_PREFIX}" QDMI_TEMPLATE_prefix)

if(NOT EXISTS "${QDMI_TEMPLATE_SOURCE_DIR}")
  message(
    FATAL_ERROR
      "[qdmi-template] Source directory does not exist: ${QDMI_TEMPLATE_SOURCE_DIR}"
  )
endif()

# Destination handling By default, we allow generating into an existing
# directory (even if non-empty) and simply overwrite files that belong to the
# template. This avoids surprising configure-time side effects while keeping the
# generation convenient.
if(EXISTS "${QDMI_TEMPLATE_OUTPUT_DIR}")
  if(QDMI_TEMPLATE_CLEAN)
    message(
      STATUS "[qdmi-template] Cleaning output dir: ${QDMI_TEMPLATE_OUTPUT_DIR}")
    file(REMOVE_RECURSE "${QDMI_TEMPLATE_OUTPUT_DIR}")
    file(MAKE_DIRECTORY "${QDMI_TEMPLATE_OUTPUT_DIR}")
  else()
    # Keep the directory and just overwrite files as needed.
    message(
      STATUS
        "[qdmi-template] Output directory exists; updating in-place: ${QDMI_TEMPLATE_OUTPUT_DIR}"
    )
  endif()
else()
  file(MAKE_DIRECTORY "${QDMI_TEMPLATE_OUTPUT_DIR}")
endif()

message(
  STATUS
    "[qdmi-template] Generating device template for prefix '${QDMI_TEMPLATE_PREFIX}' in '${QDMI_TEMPLATE_OUTPUT_DIR}'"
)

# Collect all files under the template source dir, but only regular files.
file(
  GLOB_RECURSE _files
  LIST_DIRECTORIES false
  "${QDMI_TEMPLATE_SOURCE_DIR}/*")

# Apply substitutions file-by-file.
foreach(_src IN LISTS _files)
  file(RELATIVE_PATH _rel "${QDMI_TEMPLATE_SOURCE_DIR}" "${_src}")
  get_filename_component(_rel_dir "${_rel}" DIRECTORY)
  get_filename_component(_name "${_rel}" NAME)

  # Rename path segments / filenames similar to the previous configure-time
  # logic.
  set(_dest_rel_dir "${_rel_dir}")
  set(_dest_name "${_name}")

  # Directory-level rename: python/my -> python/<prefix>
  string(REPLACE "python/my" "python/${QDMI_TEMPLATE_prefix}" _dest_rel_dir
                 "${_dest_rel_dir}")

  # Filename renames that include the placeholder
  string(REPLACE "my_" "${QDMI_TEMPLATE_prefix}_" _dest_name "${_dest_name}")
  string(REPLACE "my-" "${QDMI_TEMPLATE_prefix}-" _dest_name "${_dest_name}")
  string(REPLACE "my.qdmi" "${QDMI_TEMPLATE_prefix}.qdmi" _dest_name
                 "${_dest_name}")

  set(_dest "${QDMI_TEMPLATE_OUTPUT_DIR}/${_dest_rel_dir}/${_dest_name}")
  get_filename_component(_dest_dir "${_dest}" DIRECTORY)
  file(MAKE_DIRECTORY "${_dest_dir}")

  # Read content and replace placeholders.
  file(READ "${_src}" _content)

  # Be conservative: these are all specific placeholders used by the shipped
  # template.
  string(
    REGEX
    REPLACE "set\\(QDMI_PREFIX \"MY\"\\)"
            "set(QDMI_PREFIX \"${QDMI_TEMPLATE_PREFIX}\")" _content
            "${_content}")
  string(REPLACE "MY_" "${QDMI_TEMPLATE_PREFIX}_" _content "${_content}")
  string(REPLACE "MY QDMI" "${QDMI_TEMPLATE_PREFIX} QDMI" _content
                 "${_content}")
  string(REPLACE "my_" "${QDMI_TEMPLATE_prefix}_" _content "${_content}")
  string(REPLACE "my-" "${QDMI_TEMPLATE_prefix}-" _content "${_content}")
  string(REPLACE "python/my" "python/${QDMI_TEMPLATE_prefix}" _content
                 "${_content}")
  string(REPLACE "my/qdmi" "${QDMI_TEMPLATE_prefix}/qdmi" _content
                 "${_content}")
  string(REPLACE "my-qdmi" "${QDMI_TEMPLATE_prefix}-qdmi" _content
                 "${_content}")
  string(REPLACE "my.qdmi" "${QDMI_TEMPLATE_prefix}.qdmi" _content
                 "${_content}")
  string(REPLACE "\"my\"" "\"${QDMI_TEMPLATE_PREFIX}\"" _content "${_content}")
  string(REPLACE "--namespace-pkg my" "--namespace-pkg ${QDMI_TEMPLATE_prefix}"
                 _content "${_content}")

  file(WRITE "${_dest}" "${_content}")
endforeach()

# Explicit folder/file renames that can't be expressed purely via filename
# mapping above. (These are safe no-ops if the file doesn't exist in future
# template revisions.)
if(EXISTS "${QDMI_TEMPLATE_OUTPUT_DIR}/python/my")
  file(RENAME "${QDMI_TEMPLATE_OUTPUT_DIR}/python/my"
       "${QDMI_TEMPLATE_OUTPUT_DIR}/python/${QDMI_TEMPLATE_prefix}")
endif()

foreach(
  _p IN
  ITEMS
    "src/my_device.cpp;src/${QDMI_TEMPLATE_prefix}_device.cpp"
    "test/test_my_device.cpp;test/test_${QDMI_TEMPLATE_prefix}_device.cpp"
    "cmake/my-qdmi-device-config.cmake.in;cmake/${QDMI_TEMPLATE_prefix}-qdmi-device-config.cmake.in"
    "docs/_static/my_logo.svg;docs/_static/${QDMI_TEMPLATE_prefix}_logo.svg"
    "docs/_static/my_logo_dark.svg;docs/_static/${QDMI_TEMPLATE_prefix}_logo_dark.svg"
)
  string(REPLACE ";" ";" _p "${_p}")
  list(GET _p 0 _from)
  list(GET _p 1 _to)
  if(EXISTS "${QDMI_TEMPLATE_OUTPUT_DIR}/${_from}")
    file(RENAME "${QDMI_TEMPLATE_OUTPUT_DIR}/${_from}"
         "${QDMI_TEMPLATE_OUTPUT_DIR}/${_to}")
  endif()
endforeach()

message(STATUS "[qdmi-template] Generation done")
