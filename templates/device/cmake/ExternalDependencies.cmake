# ------------------------------------------------------------------------------
# Part of the MQSS Project, under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
# ------------------------------------------------------------------------------

include(FetchContent)
include(CMakeDependentOption)
set(FETCH_PACKAGES "")

if(TARGET qdmi::qdmi)
  message(STATUS "Device template: QDMI is already available.")
else()
  message(STATUS "Device template: QDMI will be included via FetchContent")
  # cmake-format: off
  set(QDMI_VERSION 1.0.0
          CACHE STRING "QDMI version")
  set(QDMI_REV "146f887ad887fbe81d831863d346e7554b5b1505"
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
