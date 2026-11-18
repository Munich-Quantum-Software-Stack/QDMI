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

# Each configure is independent of QDMI's own examples, templates, and tests.
function(run)
  execute_process(
    COMMAND ${ARGV}
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
    ERROR_VARIABLE error)
  if(NOT result EQUAL 0)
    message(FATAL_ERROR "${ARGV}\n${output}\n${error}")
  endif()
endfunction()
set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(root "${SOURCE_DIR}/install-build/query consumers")
set(common -DCMAKE_BUILD_TYPE=Release -DENABLE_CACHE=OFF)
set(dependencies
    "-DGOOGLETEST_SOURCE_DIR=${SOURCE_DIR}/install-build/_deps/googletest-src")
function(check_consumer name source)
  set(build "${root}/${name}")
  run("${CMAKE_COMMAND}"
      -S
      "${source}"
      -B
      "${build}"
      ${common}
      ${dependencies}
      ${ARGN})
  run("${CMAKE_COMMAND}"
      --build
      "${build}"
      --config
      Release
      --parallel
      2)
  run("${CMAKE_CTEST_COMMAND}"
      --test-dir
      "${build}"
      -C
      Release
      --output-on-failure
      --no-tests=error)
  run("${CMAKE_COMMAND}"
      -S
      "${source}"
      -B
      "${build} optout"
      ${common}
      ${ARGN}
      -DQUERY_TESTS=OFF)
endfunction()
check_consumer(
  source "${SOURCE_DIR}/test/query/consumer" "-DQDMI_SOURCE_DIR=${SOURCE_DIR}"
  "-DDEVICE_SOURCE=${SOURCE_DIR}/examples/device/src/cxx_device.cpp"
  "-DFIXTURE_SOURCE=${SOURCE_DIR}/test/query/example_fixture.hpp")
# False-like unknown arguments are still errors; valid OFF identifiers work
# above.
execute_process(
  COMMAND
    "${CMAKE_COMMAND}" -S "${SOURCE_DIR}/test/query/consumer" -B
    "${root}/invalid argument" ${common} ${dependencies}
    "-DQDMI_SOURCE_DIR=${SOURCE_DIR}"
    "-DDEVICE_SOURCE=${SOURCE_DIR}/examples/device/src/cxx_device.cpp"
    "-DFIXTURE_SOURCE=${SOURCE_DIR}/test/query/example_fixture.hpp"
    -DQUERY_EXTRA_ARGUMENT=OFF
  RESULT_VARIABLE result
  OUTPUT_VARIABLE output
  ERROR_VARIABLE error)
if(result EQUAL 0 OR NOT "${output}${error}" MATCHES "requires a name")
  message(
    FATAL_ERROR "Unknown OFF argument was not rejected: ${output}${error}")
endif()
file(COPY "${SOURCE_DIR}/test/query/consumer/" DESTINATION "${root}/consumer")
file(COPY "${SOURCE_DIR}/examples/device/src/cxx_device.cpp"
          "${SOURCE_DIR}/test/query/example_fixture.hpp"
     DESTINATION "${root}/consumer")
set(package "${SOURCE_DIR}/relocated package")
file(GLOB_RECURSE package_files "${package}/*.cmake" "${package}/*.in")
foreach(path IN LISTS package_files)
  file(READ "${path}" contents)
  string(FIND "${contents}" "${SOURCE_DIR}" source_reference)
  if(NOT source_reference EQUAL -1)
    message(FATAL_ERROR "Installed asset references the original tree: ${path}")
  endif()
endforeach()
# Simulate a compatible installation made before the query helper existed.
file(COPY "${package}/" DESTINATION "${root}/older package")
set(older_config "${root}/older package/share/cmake/qdmi/qdmi-config.cmake")
file(READ "${older_config}" older_contents)
string(REGEX REPLACE "include\\([^\n]*DeviceQueryTests\\.cmake[^\n]*\\)\n" ""
                     older_contents "${older_contents}")
if(older_contents MATCHES "DeviceQueryTests\\.cmake")
  message(FATAL_ERROR "Failed to simulate an older QDMI package")
endif()
file(WRITE "${older_config}" "${older_contents}")
run("${CMAKE_COMMAND}"
    -S
    "${SOURCE_DIR}"
    -B
    "${root}/older installed"
    ${common}
    "-DFETCHCONTENT_SOURCE_DIR_GOOGLETEST=${SOURCE_DIR}/install-build/_deps/googletest-src"
    -DUSE_INSTALLED_QDMI=ON
    "-DCMAKE_PREFIX_PATH=${root}/older package")
check_consumer(
  installed "${root}/consumer" "-DCMAKE_PREFIX_PATH=${package}"
  "-DDEVICE_SOURCE=${root}/consumer/cxx_device.cpp"
  "-DFIXTURE_SOURCE=${root}/consumer/example_fixture.hpp")
message(
  STATUS "Source and relocated installed consumers passed; opt-outs checked")
