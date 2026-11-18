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

if(NOT COMMAND add_qdmi_device_query_tests)
  include("${PROJECT_SOURCE_DIR}/cmake/DeviceQueryTests.cmake")
endif()
add_qdmi_device_query_tests(
  cxx-device-query-test
  PREFIX
  CXX
  TARGET
  cxx-qdmi-device
  FIXTURE_HEADER
  example_fixture.hpp)

# Fixed query defects and valid edge cases share a small offline provider.
generate_prefixed_qdmi_headers(CONTROL)
add_library(query-control-device STATIC control_device.cpp)
target_include_directories(query-control-device
                           PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/include")
target_compile_definitions(query-control-device
                           PUBLIC CONTROL_QDMI_STATIC_DEFINE)
target_compile_features(query-control-device PRIVATE cxx_std_20)
target_link_libraries(query-control-device PRIVATE qdmi::qdmi_project_warnings)
set(QDMI_QUERY_PREFIX CONTROL)
set(QDMI_QUERY_prefix control)
set(QDMI_QUERY_FIXTURE_HEADER
    "${CMAKE_CURRENT_SOURCE_DIR}/negative_fixture.hpp")
set_property(
  DIRECTORY
  APPEND
  PROPERTY CMAKE_CONFIGURE_DEPENDS
           "${PROJECT_SOURCE_DIR}/cmake/device_query_tests.cpp.in")
file(READ "${PROJECT_SOURCE_DIR}/cmake/device_query_tests.cpp.in"
     query_template)
string(APPEND query_template
       "\n#include \"${CMAKE_CURRENT_SOURCE_DIR}/positive_controls.inc\"\n")
file(CONFIGURE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/query_control_tests.cpp"
     CONTENT "${query_template}" @ONLY)
add_executable(query-control
               "${CMAKE_CURRENT_BINARY_DIR}/query_control_tests.cpp")
target_link_libraries(query-control PRIVATE query-control-device
                                            GTest::gtest_main)
target_compile_features(query-control PRIVATE cxx_std_20)
add_test(
  NAME query-positive
  COMMAND
    query-control
    "--gtest_filter=-*/QDMIQueryTest.DeviceLengthUnit/*:*/QDMIQueryTest.DeviceDurationUnit/*:*/QDMIQueryTest.SiteXCoordinate/*:*/QDMIQueryTest.DeviceQueueLength/*:*/QDMIQueryTest.DeviceSupportedProgramFormats/*:*/QDMIQueryTest.DeviceInvalidProperty/*:*/QDMIQueryTest.SiteIndex/*:*/QDMIQueryTest.Missing*/*:*/QDMIQueryTest.InvalidStatus/*"
)
set_tests_properties(query-positive PROPERTIES LABELS qdmi-query)
add_test(
  NAME query-positive-mixed-support
  COMMAND
    query-control
    "--gtest_filter=*/QDMIQueryTest.SiteT1/*:*/QDMIQueryTest.OperationName/*:*/QDMIQueryTest.DeviceCouplingMap/*:*/QDMIQueryTest.RemovedDeviceProperties/*:*/QDMIQueryTest.SuccessfulPayloadSnapshot/*"
)
set_tests_properties(
  query-positive-mixed-support
  PROPERTIES LABELS qdmi-query FAIL_REGULAR_EXPRESSION "\\[  SKIPPED \\]")
foreach(
  control IN
  ITEMS "DeviceLengthUnit|unit must not be empty"
        "DeviceDurationUnit|unit must use a canonical symbol"
        "SiteXCoordinate|unit must not be empty"
        "DeviceQueueLength|scalar payload size"
        "DeviceSupportedProgramFormats|array payload size"
        "DeviceInvalidProperty|invalid device property"
        "SiteIndex|mandatory site index"
        "MissingArrayProbeSize|size probe did not set"
        "MissingBoolProbeSize|size probe did not set"
        "MissingArrayRetrievalSize|payload size changed"
        "MissingBoolRetrievalSize|payload size changed"
        "MissingTerminator|string payload must end in NUL"
        "InvalidStatus|device status must be a defined status"
        "DeviceSites|advertised site handle"
        "DeviceOperations|advertised operation handle"
        "DeviceChildDevices|advertised child device handle"
        "SiteCoordinateUniqueness|coordinate triples must be unique")
  string(REPLACE "|" ";" fields "${control}")
  list(GET fields 0 case)
  list(GET fields 1 diagnostic)
  foreach(variant IN ITEMS ExactWithoutSize ExactWithSize OversizedWithoutSize
                           OversizedWithSize)
    if(case MATCHES "Missing.*RetrievalSize" AND variant MATCHES "WithoutSize")
      continue()
    endif()
    add_test(
      NAME "query-negative.${case}.${variant}"
      COMMAND
        "${CMAKE_COMMAND}" "-DEXECUTABLE=$<TARGET_FILE:query-control>"
        "-DCASE=${case}" "-DVARIANT=${variant}" "-DDIAGNOSTIC=${diagnostic}"
        "-DREPORT=${CMAKE_CURRENT_BINARY_DIR}/negative-${case}-${variant}.xml"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/check_negative.cmake")
    set_tests_properties("query-negative.${case}.${variant}"
                         PROPERTIES LABELS qdmi-query)
  endforeach()
endforeach()

foreach(control IN ITEMS coordinate-x coordinate-y coordinate-z
                         partial-coordinates single-coordinate)
  add_test(
    NAME "query-positive.${control}"
    COMMAND
      "${CMAKE_COMMAND}" -E env "QDMI_QUERY_CONTROL=${control}"
      $<TARGET_FILE:query-control>
      "--gtest_filter=*/QDMIQueryTest.SiteCoordinateUniqueness/*")
  set_tests_properties(
    "query-positive.${control}"
    PROPERTIES LABELS qdmi-query FAIL_REGULAR_EXPRESSION "\\[  SKIPPED \\]")
endforeach()

foreach(control IN ITEMS valid-handles empty-handles)
  add_test(
    NAME "query-positive.${control}"
    COMMAND
      "${CMAKE_COMMAND}" -E env "QDMI_QUERY_CONTROL=${control}"
      $<TARGET_FILE:query-control>
      "--gtest_filter=*/QDMIQueryTest.DeviceSites/*:*/QDMIQueryTest.DeviceOperations/*:*/QDMIQueryTest.DeviceChildDevices/*"
  )
  set_tests_properties(
    "query-positive.${control}"
    PROPERTIES LABELS qdmi-query FAIL_REGULAR_EXPRESSION "\\[  SKIPPED \\]")
endforeach()

add_test(
  NAME query-consumers
  COMMAND
    "${CMAKE_COMMAND}" "-DSOURCE_DIR=${PROJECT_SOURCE_DIR}"
    "-DBINARY_DIR=${PROJECT_BINARY_DIR}" "-DGENERATOR=${CMAKE_GENERATOR}"
    "-DC_COMPILER=${CMAKE_C_COMPILER}" "-DCXX_COMPILER=${CMAKE_CXX_COMPILER}"
    "-DGTEST_SOURCE=${googletest_SOURCE_DIR}"
    "-DCTEST_COMMAND=${CMAKE_CTEST_COMMAND}" -P
    "${CMAKE_CURRENT_SOURCE_DIR}/check_consumers.cmake")
set_tests_properties(query-consumers PROPERTIES LABELS qdmi-query TIMEOUT 600)
