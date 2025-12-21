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

option(CONFIGURE_TEMPLATE "Configure the template" OFF)

if(CONFIGURE_TEMPLATE)
  set(TEMPLATE_PREFIX
      "MY"
      CACHE STRING "Prefix for the template")
  string(TOLOWER ${TEMPLATE_PREFIX} TEMPLATE_prefix)
  set(TEMPLATE_PATH
      "${CMAKE_SOURCE_DIR}/../${TEMPLATE_prefix}_qdmi_device"
      CACHE STRING "Path to the template")
  file(REAL_PATH ${TEMPLATE_PATH} TEMPLATE_PATH)
  message(
    STATUS
      "[qdmi] Configuring template for prefix \"${TEMPLATE_PREFIX}\" in '${TEMPLATE_PATH}'"
  )
  file(GLOB_RECURSE QDMI_TEMPLATE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/device/**.*)
  foreach(file ${QDMI_TEMPLATE_FILES})
    file(RELATIVE_PATH rel_file ${CMAKE_CURRENT_SOURCE_DIR}/device ${file})
    get_filename_component(rel_dir ${rel_file} DIRECTORY)
    get_filename_component(file_name ${rel_file} NAME)
    file(MAKE_DIRECTORY ${TEMPLATE_PATH}/${rel_dir})
    file(READ ${file} file_content)
    string(
      REGEX
      REPLACE "set\\(QDMI_PREFIX \"MY\"\\)"
              "set(QDMI_PREFIX \"${TEMPLATE_PREFIX}\")" file_content
              "${file_content}")
    string(REGEX REPLACE "MY_" "${TEMPLATE_PREFIX}_" file_content
                         "${file_content}")
    string(REGEX REPLACE "MY QDMI" "${TEMPLATE_PREFIX} QDMI" file_content
                         "${file_content}")
    string(REGEX REPLACE "my_" "${TEMPLATE_prefix}_" file_content
                         "${file_content}")
    string(REGEX REPLACE "my-" "${TEMPLATE_prefix}-" file_content
                         "${file_content}")
    string(REGEX REPLACE "my_" "${TEMPLATE_prefix}_" file_content
                         "${file_content}")
    string(REGEX REPLACE "python/my" "python/${TEMPLATE_prefix}" file_content
                         "${file_content}")
    string(REGEX REPLACE "my/qdmi" "${TEMPLATE_prefix}/qdmi" file_content
                         "${file_content}")
    string(REGEX REPLACE "my-qdmi" "${TEMPLATE_prefix}-qdmi" file_content
                         "${file_content}")
    string(REGEX REPLACE "my.qdmi" "${TEMPLATE_prefix}.qdmi" file_content
                         "${file_content}")
    string(REGEX REPLACE "\"my\"" "\"${TEMPLATE_PREFIX}\"" file_content
                         "${file_content}")
    file(WRITE ${TEMPLATE_PATH}/${rel_dir}/${file_name} "${file_content}")
  endforeach()

  file(RENAME "${TEMPLATE_PATH}/python/my"
       "${TEMPLATE_PATH}/python/${TEMPLATE_prefix}")
  file(RENAME "${TEMPLATE_PATH}/src/my_device.cpp"
       "${TEMPLATE_PATH}/src/${TEMPLATE_prefix}_device.cpp")
  file(RENAME "${TEMPLATE_PATH}/test/test_my_device.cpp"
       "${TEMPLATE_PATH}/test/test_${TEMPLATE_prefix}_device.cpp")
  file(RENAME "${TEMPLATE_PATH}/cmake/my-qdmi-device-config.cmake.in"
       "${TEMPLATE_PATH}/cmake/${TEMPLATE_prefix}-qdmi-device-config.cmake.in")
  file(RENAME "${TEMPLATE_PATH}/docs/_static/my_logo.svg"
       "${TEMPLATE_PATH}/docs/_static/${TEMPLATE_prefix}_logo.svg")
  file(RENAME "${TEMPLATE_PATH}/docs/_static/my_logo_dark.svg"
       "${TEMPLATE_PATH}/docs/_static/${TEMPLATE_prefix}_logo_dark.svg")

  message(
    STATUS
      "[qdmi] Configuring template for prefix \"${TEMPLATE_PREFIX}\" in '${TEMPLATE_PATH}' - done"
  )
endif()

add_subdirectory(device)
