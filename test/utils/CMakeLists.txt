# Copyright (c) 2024 - 2025 Munich Quantum Software Stack Project
# All rights reserved.
#
# Licensed under the Apache License v2.0 with LLVM Exceptions (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# https://github.com/Munich-Quantum-Software-Stack/QDMI/blob/develop/LICENSE.md
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations under
# the License.
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

# Create a shared library that contains a test suite that can be used to test
# whether all functions are implemented in the device.
add_library(qdmi_test_impl SHARED test_impl.cpp test_impl.hpp)
target_link_libraries(
  qdmi_test_impl PRIVATE GTest::gtest GTest::gmock qdmi::qdmi
                         qdmi::example_driver qdmi::qdmi_project_warnings)
target_include_directories(qdmi_test_impl PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(qdmi_test_impl PRIVATE cxx_std_17)
add_library(qdmi::test_impl ALIAS qdmi_test_impl)

# build gtest as position independent code to link it with the shared library
set_target_properties(gtest PROPERTIES POSITION_INDEPENDENT_CODE ON)
