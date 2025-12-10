# ------------------------------------------------------------------------------
# Copyright 2024 Munich Quantum Software Stack Project
#
# Licensed under the Apache License, Version 2.0 with LLVM Exceptions (the
# "License"); you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# https://github.com/Munich-Quantum-Software-Stack/QDMI/blob/develop/LICENSE
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations under
# the License.
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
# ------------------------------------------------------------------------------

if(NOT CXX_DEVICE)
  # add CXX language support
  enable_language(CXX)
endif()

# ------------------------------------------------------------------------------
# Non-Functional Tests
# ------------------------------------------------------------------------------

generate_device_defs_executable(PREFIX ${QDMI_PREFIX} TARGET my_device)

# ------------------------------------------------------------------------------
# Functional Tests
# ------------------------------------------------------------------------------

# create an executable in which the tests will be stored
add_executable(my_device_test test_my_device.cpp)

# link the Google test infrastructure to the test executable.
target_link_libraries(my_device_test PRIVATE gtest_main qdmi::qdmi my_device
                                             qdmi::qdmi_project_warnings)

# set c++ standard
target_compile_features(my_device_test PRIVATE cxx_std_17)

# ensure my_device and my_device_definitions_test are built whenever
# my_device_test is built
add_dependencies(my_device_test qdmi_test_my_device_defs)

# only discover tests if this is the main project
if(NOT MY_DEVICE_MASTER_PROJECT)
  return()
endif()

gtest_discover_tests(
  my_device_test
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
             DISCOVERY_TIMEOUT 60)
