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

include("${QDMI_SOURCE_DIR}/cmake/PrefixHandling.cmake")
set(QDMI_CMAKE_DIR "${QDMI_SOURCE_DIR}/cmake")
set(QDMI_INCLUDE_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/prefix-fixture")
file(MAKE_DIRECTORY "${QDMI_INCLUDE_BUILD_DIR}/qdmi")
file(WRITE "${QDMI_INCLUDE_BUILD_DIR}/qdmi/device.h"
     [=[void QDMI_device_initialize(); /* @literal@ ${literal} */
]=])
generate_prefixed_qdmi_headers(TEST)
set(header "${CMAKE_CURRENT_BINARY_DIR}/include/test_qdmi/device.h")
file(READ "${header}" actual)
set(expected [=[void TEST_QDMI_device_initialize(); /* @literal@ ${literal} */
]=])
if(NOT actual STREQUAL expected)
  message(FATAL_ERROR "Prefix generation changed literal header content")
endif()
file(TIMESTAMP "${header}" before "%s")
execute_process(COMMAND "${CMAKE_COMMAND}" -E sleep 1 COMMAND_ERROR_IS_FATAL
                        ANY)
generate_prefixed_qdmi_headers(TEST)
file(TIMESTAMP "${header}" after "%s")
if(NOT before STREQUAL after)
  message(FATAL_ERROR "Unchanged prefixed header was rewritten")
endif()
file(APPEND "${QDMI_INCLUDE_BUILD_DIR}/qdmi/device.h" "changed\n")
generate_prefixed_qdmi_headers(TEST)
file(READ "${header}" actual)
string(APPEND expected "changed\n")
if(NOT actual STREQUAL expected)
  message(FATAL_ERROR "Changed header was not regenerated")
endif()
