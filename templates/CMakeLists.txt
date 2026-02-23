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

# Template instantiation should be an explicit action (build target), not a
# configure-time side effect.
option(QDMI_GENERATE_TEMPLATE
       "Enable targets to generate a prefixed device template" OFF)

if(QDMI_GENERATE_TEMPLATE)
  set(TEMPLATE_PREFIX
      "MY"
      CACHE STRING "Prefix for the template")
  string(TOLOWER "${TEMPLATE_PREFIX}" TEMPLATE_prefix)

  set(TEMPLATE_PATH
      "${CMAKE_SOURCE_DIR}/../${TEMPLATE_prefix}-qdmi-device"
      CACHE PATH "Path where the instantiated template should be generated")
  file(REAL_PATH "${TEMPLATE_PATH}" TEMPLATE_PATH)

  set(QDMI_TEMPLATE_GENERATOR_SCRIPT
      "${CMAKE_SOURCE_DIR}/cmake/GenerateTemplate.cmake")

  add_custom_target(
    qdmi-template
    COMMAND
      "${CMAKE_COMMAND}"
      -DQDMI_TEMPLATE_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}/device
      -DQDMI_TEMPLATE_OUTPUT_DIR=${TEMPLATE_PATH}
      -DQDMI_TEMPLATE_PREFIX=${TEMPLATE_PREFIX} -DQDMI_TEMPLATE_FORCE=OFF
      -DQDMI_TEMPLATE_CLEAN=OFF -P "${QDMI_TEMPLATE_GENERATOR_SCRIPT}"
    COMMENT
      "Generate QDMI device template in ${TEMPLATE_PATH} (prefix: ${TEMPLATE_PREFIX})"
    VERBATIM)

  add_custom_target(
    qdmi-template-clean
    COMMAND
      "${CMAKE_COMMAND}"
      -DQDMI_TEMPLATE_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}/device
      -DQDMI_TEMPLATE_OUTPUT_DIR=${TEMPLATE_PATH}
      -DQDMI_TEMPLATE_PREFIX=${TEMPLATE_PREFIX} -DQDMI_TEMPLATE_FORCE=ON
      -DQDMI_TEMPLATE_CLEAN=ON -P "${QDMI_TEMPLATE_GENERATOR_SCRIPT}"
    COMMENT
      "Clean and regenerate QDMI device template in ${TEMPLATE_PATH} (removes output dir first)"
    VERBATIM)
endif()

add_subdirectory(device)
