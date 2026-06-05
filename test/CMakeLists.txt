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

if(TARGET qdmi_test)
  return()
endif()

# add CXX language support
enable_language(CXX)

add_subdirectory(utils)

# ------------------------------------------------------------------------------
# Functional Tests
# ------------------------------------------------------------------------------

# create an executable in which the tests will be stored
add_executable(qdmi_test test_qdmi.cpp)

# link the Google test infrastructure to the test executable.
target_link_libraries(
  qdmi_test
  PRIVATE qdmi::qdmi
          qdmi::test_impl
          qdmi::example_tool
          qdmi::example_fomac
          qdmi::example_driver
          qdmi::qdmi_project_warnings
          GTest::gtest_main
          GTest::gmock)

target_compile_features(qdmi_test PRIVATE cxx_std_20)

# discover tests
gtest_discover_tests(
  qdmi_test
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
             DISCOVERY_TIMEOUT 60)

# ensure cxx-qdmi-device are built whenever qdmi_test is built. Those targets
# build the dynamic libraries that are loaded in the tests.
add_dependencies(qdmi_test cxx-qdmi-device)
