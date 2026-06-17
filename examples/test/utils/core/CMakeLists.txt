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

# Set target name
set(TARGET_NAME test_core)

# Create a shared library that contains a test suite that can be used to test
# whether all functions are implemented in the device. Note: Using GTest,
# linking these libraries statically will not pull in the TEST_P definitions, so
# they are linked dynamically.
add_library(${TARGET_NAME} SHARED)
target_sources(${TARGET_NAME} PRIVATE test_core.cpp)
target_sources(
  ${TARGET_NAME} PUBLIC FILE_SET HEADERS BASE_DIRS
                        ${CMAKE_CURRENT_SOURCE_DIR}/../.. FILES test_core.hpp)
target_link_libraries(
  ${TARGET_NAME}
  PUBLIC GTest::gtest qdmi::library_wrapper qdmi::session_wrapper qdmi::qdmi
  PRIVATE spdlog::spdlog qdmi::qdmi_project_warnings)
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(${TARGET_NAME} PRIVATE cxx_std_20)
add_library(qdmi::${TARGET_NAME} ALIAS ${TARGET_NAME})

# build gtest as position independent code to link it with the shared library
set_target_properties(gtest PROPERTIES POSITION_INDEPENDENT_CODE ON)
set_target_properties(${TARGET_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)

# set the folder of the target for IDEs
set_target_properties(${TARGET_NAME} PROPERTIES FOLDER "examples/test")
