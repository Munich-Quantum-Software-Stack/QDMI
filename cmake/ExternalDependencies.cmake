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

include(FetchContent)
include(CMakeDependentOption)
set(FETCH_PACKAGES "")

if(BUILD_QDMI_DOCS)
  set(CMAKE_POLICY_DEFAULT_CMP0116
      NEW
      CACHE STRING
            "Set the default CMP0116 policy to NEW for documentation builds")
  set(DOXYGEN_VERSION
      1.15.0
      CACHE STRING "Doxygen version")
  set(DOXYGEN_REV
      "7cca38ba5185457e6d9495bf963d4cdeacebc25a"
      CACHE STRING "Doxygen identifier (tag, branch or commit hash)")
  FetchContent_Declare(
    Doxygen
    GIT_REPOSITORY https://github.com/doxygen/doxygen.git
    GIT_TAG ${DOXYGEN_REV}
    FIND_PACKAGE_ARGS ${DOXYGEN_VERSION})
  list(APPEND FETCH_PACKAGES Doxygen)

  set(DOXYGEN_AWESOME_VERSION
      2.4.1
      CACHE STRING "Doxygen Awesome version")
  set(DOXYGEN_AWESOME_REV
      "1f3620084ff75734ed192101acf40e9dff01d848"
      CACHE STRING "Doxygen Awesome identifier (tag, branch or commit hash)")
  FetchContent_Declare(
    doxygen-awesome-css
    GIT_REPOSITORY https://github.com/jothepro/doxygen-awesome-css.git
    GIT_TAG ${DOXYGEN_AWESOME_REV}
    FIND_PACKAGE_ARGS ${DOXYGEN_AWESOME_VERSION})
  list(APPEND FETCH_PACKAGES doxygen-awesome-css)
endif()

if(BUILD_QDMI_TESTS)
  set(gtest_force_shared_crt
      ON
      CACHE BOOL "" FORCE)
  set(GTEST_VERSION
      1.16.0
      CACHE STRING "Google Test version")
  set(GTEST_URL
      https://github.com/google/googletest/archive/refs/tags/v${GTEST_VERSION}.tar.gz
  )
  set(INSTALL_GTEST
      OFF
      CACHE BOOL "Disable GoogleTest installation")
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

if(USE_INSTALLED_QDMI)
  find_package(QDMI ${PROJECT_VERSION} REQUIRED)
endif()
