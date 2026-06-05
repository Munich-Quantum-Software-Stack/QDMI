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

if(TARGET qdmi_test_orchestration_layer)
  return()
endif()

# ------------------------------------------------------------------------------
# Non-Functional Tests
# ------------------------------------------------------------------------------
generate_device_defs_executable("O" TARGET qdmi::orchestration_layer_o1)
# set the folder of the target for IDEs
set_target_properties(qdmi_test_o_device_defs PROPERTIES FOLDER "examples/test")

# ------------------------------------------------------------------------------
# Functional Tests
# ------------------------------------------------------------------------------

# create an executable in which the tests will be stored
add_executable(qdmi_test_orchestration_layer test_orchestration_layer.cpp)

# link the Google test infrastructure to the test executable.
target_link_libraries(
  qdmi_test_orchestration_layer
  PRIVATE qdmi::test_orchestration_layer qdmi::test_job qdmi::test_sc
          qdmi::qdmi_project_warnings GTest::gtest_main)

target_compile_features(qdmi_test_orchestration_layer PRIVATE cxx_std_20)

target_compile_definitions(
  qdmi_test_orchestration_layer
  PRIVATE QDMI_EXAMPLE_DEVICE_PATH="$<TARGET_FILE:orchestration_layer_o1>")

# discover tests
gtest_discover_tests(
  qdmi_test_orchestration_layer
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
             DISCOVERY_TIMEOUT 60)

# ensure qdmi::orchestration_layer_o1 are built whenever
# qdmi_test_orchestration_layer is built.
add_dependencies(qdmi_test_orchestration_layer qdmi::orchestration_layer_o1)
# ensure qdmi_test_orchestration_layer_q_device_defs are built whenever
# qdmi_test_orchestration_layer is built.
add_dependencies(qdmi_test_orchestration_layer qdmi_test_o_device_defs)

# set the folder of the target for IDEs
set_target_properties(qdmi_test_orchestration_layer PROPERTIES FOLDER
                                                               "examples/test")
