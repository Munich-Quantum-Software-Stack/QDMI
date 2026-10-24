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
set(root "${BINARY_DIR}/query consumers")
file(MAKE_DIRECTORY "${root}")
set(common
    -G "${GENERATOR}" "-DCMAKE_C_COMPILER=${C_COMPILER}"
    "-DCMAKE_CXX_COMPILER=${CXX_COMPILER}" -DCMAKE_BUILD_TYPE=Release
    -DENABLE_CACHE=OFF)
if(GTEST_SOURCE)
  file(COPY "${GTEST_SOURCE}/" DESTINATION "${root}/googletest source")
  set(dependencies "-DGOOGLETEST_SOURCE_DIR=${root}/googletest source")
endif()
run("${CMAKE_COMMAND}"
    -S
    "${SOURCE_DIR}/test/query/consumer"
    -B
    "${root}/source"
    ${common}
    ${dependencies}
    "-DQDMI_SOURCE_DIR=${SOURCE_DIR}"
    "-DDEVICE_SOURCE=${SOURCE_DIR}/examples/device/src/cxx_device.cpp")
# False-like unknown arguments are still errors; valid OFF identifiers work
# above.
execute_process(
  COMMAND
    "${CMAKE_COMMAND}" -S "${SOURCE_DIR}/test/query/consumer" -B
    "${root}/invalid argument" ${common} ${dependencies}
    "-DQDMI_SOURCE_DIR=${SOURCE_DIR}"
    "-DDEVICE_SOURCE=${SOURCE_DIR}/examples/device/src/cxx_device.cpp"
    -DQUERY_EXTRA_ARGUMENT=OFF
  RESULT_VARIABLE result
  OUTPUT_VARIABLE output
  ERROR_VARIABLE error)
if(result EQUAL 0 OR NOT "${output}${error}" MATCHES "requires a name")
  message(
    FATAL_ERROR "Unknown OFF argument was not rejected: ${output}${error}")
endif()
run("${CMAKE_COMMAND}"
    --build
    "${root}/source"
    --config
    Release
    --parallel
    2)
run("${CTEST_COMMAND}"
    --test-dir
    "${root}/source"
    -C
    Release
    --output-on-failure
    --no-tests=error)
run("${CMAKE_COMMAND}"
    -S
    "${SOURCE_DIR}/test/query/consumer"
    -B
    "${root}/source optout"
    ${common}
    "-DQDMI_SOURCE_DIR=${SOURCE_DIR}"
    -DQUERY_TESTS=OFF)

# Install without test dependencies, then move the package and consumer inputs.
run("${CMAKE_COMMAND}"
    -S
    "${SOURCE_DIR}"
    -B
    "${root}/package"
    ${common}
    -DBUILD_QDMI_TESTS=OFF
    -DBUILD_QDMI_EXAMPLES=OFF
    -DBUILD_QDMI_TEMPLATES=OFF
    -DINSTALL_QDMI=ON)
run("${CMAKE_COMMAND}"
    --install
    "${root}/package"
    --config
    Release
    --prefix
    "${root}/staged")
file(COPY "${root}/staged/" DESTINATION "${root}/relocated package")
file(REMOVE_RECURSE "${root}/staged")
file(COPY "${SOURCE_DIR}/test/query/consumer/" DESTINATION "${root}/consumer")
file(COPY "${SOURCE_DIR}/examples/device/src/cxx_device.cpp"
     DESTINATION "${root}/consumer")
set(package "${root}/relocated package")
file(GLOB_RECURSE package_files "${package}/*.cmake" "${package}/*.in")
foreach(path IN LISTS package_files)
  file(READ "${path}" contents)
  string(FIND "${contents}" "${SOURCE_DIR}" source_reference)
  string(FIND "${contents}" "${root}/package" build_reference)
  if(NOT source_reference EQUAL -1 OR NOT build_reference EQUAL -1)
    message(FATAL_ERROR "Installed asset references the original tree: ${path}")
  endif()
endforeach()
# Missing-asset controls change generated installations, never tracked sources.
foreach(asset IN ITEMS DeviceQueryTests.cmake device_query_tests.cpp.in)
  set(path "${package}/share/cmake/qdmi/${asset}")
  file(RENAME "${path}" "${path}.saved")
  execute_process(
    COMMAND
      "${CMAKE_COMMAND}" -S "${root}/consumer" -B "${root}/missing-${asset}"
      ${common} ${dependencies} "-DCMAKE_PREFIX_PATH=${package}"
      "-DDEVICE_SOURCE=${root}/consumer/cxx_device.cpp"
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
    ERROR_VARIABLE error)
  file(RENAME "${path}.saved" "${path}")
  if(result EQUAL 0 OR NOT "${output}${error}" MATCHES "${asset}")
    message(FATAL_ERROR "Missing ${asset} was not diagnosed: ${output}${error}")
  endif()
endforeach()
run("${CMAKE_COMMAND}"
    -S
    "${root}/consumer"
    -B
    "${root}/installed"
    ${common}
    ${dependencies}
    "-DCMAKE_PREFIX_PATH=${package}"
    "-DDEVICE_SOURCE=${root}/consumer/cxx_device.cpp")
run("${CMAKE_COMMAND}"
    --build
    "${root}/installed"
    --config
    Release
    --parallel
    2)
run("${CTEST_COMMAND}"
    --test-dir
    "${root}/installed"
    -C
    Release
    --output-on-failure
    --no-tests=error)
run("${CMAKE_COMMAND}"
    -S
    "${root}/consumer"
    -B
    "${root}/installed optout"
    ${common}
    "-DCMAKE_PREFIX_PATH=${package}"
    -DQUERY_TESTS=OFF)
message(
  STATUS
    "Source and relocated installed consumers passed; opt-outs and missing assets checked"
)
