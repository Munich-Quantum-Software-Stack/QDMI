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

# ------------------------------------------------------------------------------
# Non-Functional Tests
# ------------------------------------------------------------------------------

generate_device_defs_executable(${QDMI_PREFIX} TARGET ${QDMI_TARGET_NAME})

# ------------------------------------------------------------------------------
# Functional Tests
# ------------------------------------------------------------------------------

set(QDMI_TEST_TARGET_NAME "${QDMI_TARGET_NAME}-test")

# create an executable in which the tests will be stored
add_executable(${QDMI_TEST_TARGET_NAME} test_my_device.cpp)

# link the Google test infrastructure to the test executable.
target_link_libraries(
  ${QDMI_TEST_TARGET_NAME}
  PRIVATE GTest::gtest_main qdmi::qdmi ${QDMI_TARGET_NAME}
          qdmi::qdmi_project_warnings)

# set c++ standard
target_compile_features(${QDMI_TEST_TARGET_NAME} PRIVATE cxx_std_20)

add_dependencies(${QDMI_TEST_TARGET_NAME} qdmi_test_my_device_defs)

if(WIN32)
  # On Windows, copy the DLL to the test executable directory
  add_custom_command(
    TARGET ${QDMI_TEST_TARGET_NAME}
    POST_BUILD
    COMMAND
      ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:${QDMI_TARGET_NAME}>
      $<TARGET_FILE_DIR:${QDMI_TEST_TARGET_NAME}>
    COMMENT "Copying ${QDMI_TARGET_NAME} DLL to test directory")

  # Add the directory containing the DLL to the test's PATH environment
  set(DLL_PATH "$<TARGET_FILE_DIR:${QDMI_TARGET_NAME}>")

  gtest_discover_tests(
    ${QDMI_TEST_TARGET_NAME}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY
               "${CMAKE_CURRENT_BINARY_DIR}"
               ENVIRONMENT
               "PATH=${DLL_PATH};$ENV{PATH}"
               DISCOVERY_TIMEOUT
               60
               DISCOVERY_MODE
               PRE_TEST)
else()
  gtest_discover_tests(
    ${QDMI_TEST_TARGET_NAME}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
               DISCOVERY_TIMEOUT 60)
endif()
