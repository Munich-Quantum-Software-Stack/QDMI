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

cmake_minimum_required(VERSION 3.24)

set(QDMI_DOCS_HTML_DIR "${TEST_OUTPUT_DIR}/doxygen-index")
file(MAKE_DIRECTORY "${QDMI_DOCS_HTML_DIR}")
set(index_file "${QDMI_DOCS_HTML_DIR}/globals_type.html")
file(
  WRITE "${index_file}"
  [=[
<a href="#index_c">C</a><a href="#index_d">D</a><a href="#index_j">J</a>
<h2 id="index_c">C</h2>
<ul>
<li>QDMI_Child_Device&#160;:&#160;<a href="device.html#child">device.h</a></li>
<li>QDMI_Device&#160;:&#160;<a href="client.html#device">client.h</a></li>
<li>QDMI_Device_Job&#160;:&#160;<a href="device.html#job">device.h</a></li>
<li>QDMI_job_free()&#160;:&#160;<a href="client.html#free">client.h</a></li>
</ul>
]=])
set(expected
    [=[
<a href="#index_c">C</a><a href="#index_d">D</a><a href="#index_j">J</a>
<h2 id="index_c">C</h2>
<ul>
<li>QDMI_Child_Device&#160;:&#160;<a href="device.html#child">device.h</a></li>
<li id="index_d">QDMI_Device&#160;:&#160;<a href="client.html#device">client.h</a></li>
<li>QDMI_Device_Job&#160;:&#160;<a href="device.html#job">device.h</a></li>
<li id="index_j">QDMI_job_free()&#160;:&#160;<a href="client.html#free">client.h</a></li>
</ul>
]=])

# Existing targets and repeated entries must survive both the first and repeat
# run.
foreach(run RANGE 1 2)
  include("${CMAKE_CURRENT_LIST_DIR}/../docs/FixDoxygenIndex.cmake")
  file(READ "${index_file}" actual)
  if(NOT actual STREQUAL expected)
    message(
      FATAL_ERROR
        "API index links must target the first matching entry without changing existing targets or member links (run ${run})."
    )
  endif()
endforeach()

# Fail the documentation build if a future output format cannot be repaired.
file(WRITE "${index_file}"
     "<a href=\"#index_z\">Z</a><ul><li>QDMI_Device</li></ul>")
execute_process(
  COMMAND ${CMAKE_COMMAND} -DQDMI_DOCS_HTML_DIR=${QDMI_DOCS_HTML_DIR} -P
          ${CMAKE_CURRENT_LIST_DIR}/../docs/FixDoxygenIndex.cmake
  RESULT_VARIABLE result
  OUTPUT_QUIET ERROR_QUIET)
if(result EQUAL 0)
  message(FATAL_ERROR "An index link without a matching API entry must fail.")
endif()
