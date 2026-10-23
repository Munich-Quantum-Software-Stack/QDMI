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

# Compile the shared query tests against one device and its session fixture. The
# caller supplies GoogleTest and enables CTest. The fixture defines
# QDMIQueryFixture, initializes/finalizes the device once per suite, and owns an
# initialized, protected session for each test.
function(add_qdmi_device_query_tests name)
  cmake_parse_arguments(PARSE_ARGV 1 ARG "" "PREFIX;TARGET;FIXTURE_HEADER" "")
  if("${name}" STREQUAL ""
     OR ARG_UNPARSED_ARGUMENTS
     OR ARG_KEYWORDS_MISSING_VALUES)
    message(
      FATAL_ERROR
        "add_qdmi_device_query_tests requires a name, PREFIX, TARGET, and FIXTURE_HEADER"
    )
  endif()
  foreach(required_argument IN ITEMS PREFIX TARGET FIXTURE_HEADER)
    if(NOT ARG_${required_argument})
      message(
        FATAL_ERROR "add_qdmi_device_query_tests requires ${required_argument}")
    endif()
  endforeach()
  if(NOT ARG_PREFIX MATCHES "^[A-Za-z_][A-Za-z0-9_]*$")
    message(FATAL_ERROR "Invalid QDMI device prefix: ${ARG_PREFIX}")
  endif()
  if(NOT TARGET "${ARG_TARGET}")
    message(FATAL_ERROR "Unknown QDMI device target: ${ARG_TARGET}")
  endif()
  get_filename_component(QDMI_QUERY_FIXTURE_HEADER "${ARG_FIXTURE_HEADER}"
                         ABSOLUTE BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
  if(NOT EXISTS "${QDMI_QUERY_FIXTURE_HEADER}"
     OR IS_DIRECTORY "${QDMI_QUERY_FIXTURE_HEADER}")
    message(
      FATAL_ERROR
        "QDMI query fixture header does not exist: ${QDMI_QUERY_FIXTURE_HEADER}"
    )
  endif()
  if(NOT TARGET GTest::gtest_main)
    message(
      FATAL_ERROR
        "add_qdmi_device_query_tests requires an existing GTest::gtest_main target"
    )
  endif()

  set(QDMI_QUERY_PREFIX "${ARG_PREFIX}")
  string(TOLOWER "${ARG_PREFIX}" QDMI_QUERY_prefix)
  set(query_source "${CMAKE_CURRENT_BINARY_DIR}/${name}_query_tests.cpp")
  configure_file("${CMAKE_CURRENT_FUNCTION_LIST_DIR}/device_query_tests.cpp.in"
                 "${query_source}" @ONLY)
  add_executable(${name} "${query_source}")
  target_compile_features(${name} PRIVATE cxx_std_20)
  target_link_libraries(
    ${name} PRIVATE ${ARG_TARGET} qdmi::qdmi qdmi::qdmi_project_warnings
                    GTest::gtest_main)

  if(WIN32)
    get_target_property(device_type "${ARG_TARGET}" TYPE)
    if(device_type STREQUAL "SHARED_LIBRARY")
      add_custom_command(
        TARGET ${name}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "$<TARGET_FILE:${ARG_TARGET}>" "$<TARGET_FILE_DIR:${name}>"
        COMMENT "Copying ${ARG_TARGET} DLL to query test directory")
    endif()
  endif()

  include(GoogleTest)
  gtest_discover_tests(
    ${name}
    TEST_PREFIX "${name}."
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}" DISCOVERY_MODE PRE_TEST
                                                    DISCOVERY_TIMEOUT 60
    PROPERTIES LABELS qdmi-query)
endfunction()
