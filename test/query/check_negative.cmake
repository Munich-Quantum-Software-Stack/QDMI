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

file(REMOVE "${REPORT}")
execute_process(
  COMMAND "${EXECUTABLE}" "--gtest_filter=QDMIQueryFixture.${CASE}"
          "--gtest_color=no" "--gtest_output=xml:${REPORT}"
  RESULT_VARIABLE result
  OUTPUT_VARIABLE output
  ERROR_VARIABLE errors
  TIMEOUT 30)
if(NOT result STREQUAL "1")
  message(
    FATAL_ERROR
      "Expected test failure exit 1, got ${result}: ${output}${errors}")
endif()
if(NOT EXISTS "${REPORT}")
  message(FATAL_ERROR "Missing GoogleTest report: ${output}${errors}")
endif()
file(READ "${REPORT}" report)
string(REGEX MATCHALL "<failure " failures "${report}")
list(LENGTH failures failure_count)
if(NOT failure_count EQUAL 1)
  message(
    FATAL_ERROR
      "Expected one contract assertion, got ${failure_count}: ${report}")
endif()
if(NOT report MATCHES [[tests="1" failures="1" disabled="0" errors="0"]]
   OR NOT report MATCHES "<testcase name=\"${CASE}\""
   OR NOT report MATCHES [[classname="QDMIQueryFixture"]]
   OR NOT report MATCHES "${DIAGNOSTIC}")
  message(FATAL_ERROR "Wrong failure for ${CASE}: ${report}${output}${errors}")
endif()
