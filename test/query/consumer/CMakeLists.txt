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

cmake_minimum_required(VERSION 3.24...4.4)
project(qdmi-query-consumer LANGUAGES C CXX)

option(QUERY_TESTS "Explicitly opt in to the query suite" ON)
if(QDMI_SOURCE_DIR)
  set(BUILD_QDMI_TESTS
      OFF
      CACHE BOOL "" FORCE)
  set(BUILD_QDMI_EXAMPLES
      OFF
      CACHE BOOL "" FORCE)
  set(BUILD_QDMI_TEMPLATES
      OFF
      CACHE BOOL "" FORCE)
  set(INSTALL_QDMI
      OFF
      CACHE BOOL "" FORCE)
  add_subdirectory("${QDMI_SOURCE_DIR}" qdmi)
else()
  find_package(qdmi CONFIG REQUIRED)
endif()
if(TARGET GTest::gtest_main OR TARGET cxx-qdmi-device)
  message(FATAL_ERROR "QDMI introduced an implicit test dependency")
endif()
if(NOT QUERY_TESTS)
  if(TARGET query-first OR TARGET query-second)
    message(FATAL_ERROR "Query tests were not explicitly requested")
  endif()
  return()
endif()

enable_testing()
# Dependency setup is explicit and may use a previously downloaded source tree.
if(GOOGLETEST_SOURCE_DIR)
  add_subdirectory("${GOOGLETEST_SOURCE_DIR}" googletest)
else()
  find_package(GTest CONFIG REQUIRED)
endif()
generate_prefixed_qdmi_headers(OFF)
# Reuse the offline example supplied by the caller; both identifiers are
# false-like.
file(READ "${DEVICE_SOURCE}" device_source)
string(REPLACE "CXX" "OFF" device_source "${device_source}")
string(REPLACE "cxx_qdmi" "off_qdmi" device_source "${device_source}")
file(CONFIGURE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/device.cpp" CONTENT
     "@device_source@" @ONLY)
add_library(OFF STATIC "${CMAKE_CURRENT_BINARY_DIR}/device.cpp")
target_include_directories(OFF PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/include")
target_compile_features(OFF PRIVATE cxx_std_20)
target_compile_definitions(
  OFF
  PUBLIC OFF_QDMI_STATIC_DEFINE
  PRIVATE QDMI_VERSION="1.4.0" OFF_QDMI_DEVICE_ID="consumer.local"
          OFF_QDMI_DEVICE_VERSION="1.0.0")
# Include a space in the fixture path, independently of the source checkout
# path.
configure_file(query_fixture.hpp "fixture with spaces/query_fixture.hpp"
               COPYONLY)
foreach(name IN ITEMS query-first query-second)
  add_qdmi_device_query_tests(
    ${name}
    PREFIX
    OFF
    TARGET
    OFF
    FIXTURE_HEADER
    "${CMAKE_CURRENT_BINARY_DIR}/fixture with spaces/query_fixture.hpp"
    ${QUERY_EXTRA_ARGUMENT})
endforeach()
