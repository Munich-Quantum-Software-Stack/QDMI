# ------------------------------------------------------------------------------
# Part of the MQSS Project, under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
# ------------------------------------------------------------------------------

# set required cmake version
cmake_minimum_required(VERSION 3.19...3.30)

option(CXX_DEVICE "Build a C++ device" ON)

project(
  MyDevice
  LANGUAGES C
  VERSION 0.0.1
  DESCRIPTION "My QDMI Device Implementation")

if(CXX_DEVICE)
  enable_language(CXX)
  # Require C++ standard
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)
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
