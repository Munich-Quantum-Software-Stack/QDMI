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

# set required cmake version
cmake_minimum_required(VERSION 3.19...3.30)

option(CXX_DEVICE "Build a C++ device" ON)

set(QDMI_PREFIX "MY")

project(
  MyDevice
  LANGUAGES C
  VERSION 0.0.1
  DESCRIPTION "My QDMI Device Implementation")

if(CXX_DEVICE)
  enable_language(CXX)
endif()

# make scripts available to cmake
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# check if this is the master project or used via add_subdirectory
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  set(MY_DEVICE_MASTER_PROJECT ON)
else()
  set(MY_DEVICE_MASTER_PROJECT OFF)
endif()

option(BUILD_MY_DEVICE_TESTS "Build tests for MyDevice"
       ${MY_DEVICE_MASTER_PROJECT})

include(cmake/ExternalDependencies.cmake)

# add main library code
add_subdirectory(src)

# enable testing
if(BUILD_MY_DEVICE_TESTS)
  enable_testing()
  include(GoogleTest)
  add_subdirectory(test)
endif()
