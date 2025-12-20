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

include(FetchContent)
include(CMakeDependentOption)
set(FETCH_PACKAGES "")

if(TARGET qdmi::qdmi)
  message(STATUS "[qdmi][template] QDMI is already available.")
else()
  message(STATUS "[qdmi][template] QDMI will be included via FetchContent")
  # cmake-format: off
  set(QDMI_VERSION 1.0.0
          CACHE STRING "QDMI version")
  set(QDMI_REV "develop"
          CACHE STRING "QDMI identifier (tag, branch or commit hash)")
  set(QDMI_REPO_OWNER "Munich-Quantum-Software-Stack"
          CACHE STRING "QDMI repository owner (change when using a fork)")
  # cmake-format: on

  if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.24)
    FetchContent_Declare(
      qdmi
      GIT_REPOSITORY https://github.com/${QDMI_REPO_OWNER}/qdmi.git
      GIT_TAG ${QDMI_REV}
      FIND_PACKAGE_ARGS ${QDMI_VERSION})
    list(APPEND FETCH_PACKAGES qdmi)
  else()
    find_package(qdmi ${QDMI_VERSION} QUIET)
    if(NOT qdmi_FOUND)
      FetchContent_Declare(
        qdmi
        GIT_REPOSITORY https://github.com/${QDMI_REPO_OWNER}/qdmi.git
        GIT_TAG ${QDMI_REV})
      list(APPEND FETCH_PACKAGES qdmi)
    endif()
  endif()
  message(
    STATUS "[qdmi][template] QDMI will be included via FetchContent - done")
endif()

if(BUILD_MY_DEVICE_TESTS)
  set(gtest_force_shared_crt
      ON
      CACHE BOOL "" FORCE)
  set(GTEST_VERSION
      1.14.0
      CACHE STRING "Google Test version")
  set(GTEST_URL
      https://github.com/google/googletest/archive/refs/tags/v${GTEST_VERSION}.tar.gz
  )
  if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.24)
    FetchContent_Declare(googletest URL ${GTEST_URL} FIND_PACKAGE_ARGS
                                        ${GTEST_VERSION} NAMES GTest)
    list(APPEND FETCH_PACKAGES googletest)
  else()
    find_package(googletest ${GTEST_VERSION} QUIET NAMES GTest)
    if(NOT googletest_FOUND)
      FetchContent_Declare(googletest URL ${GTEST_URL})
      list(APPEND FETCH_PACKAGES googletest)
    endif()
  endif()
endif()

# Make all declared dependencies available.
FetchContent_MakeAvailable(${FETCH_PACKAGES})
