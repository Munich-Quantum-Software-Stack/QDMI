# ------------------------------------------------------------------------------
# Part of the MQSS Project, under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
# ------------------------------------------------------------------------------

if(QDMI_INSTALL)
  include(GNUInstallDirs)
  set(QDMI_CONFIG_INSTALL_DIR "${CMAKE_INSTALL_DATADIR}/cmake/qdmi")
  set(QDMI_INCLUDE_INSTALL_DIR "${CMAKE_INSTALL_INCLUDEDIR}/qdmi")
  set(QDMI_TARGETS_EXPORT_NAME "qdmi-targets")
  set(QDMI_CMAKE_CONFIG_TEMPLATE
      "${PROJECT_SOURCE_DIR}/cmake/qdmi-config.cmake.in")
  set(QDMI_CMAKE_CONFIG_DIR "${CMAKE_CURRENT_BINARY_DIR}")
  set(QDMI_CMAKE_VERSION_CONFIG_FILE
      "${QDMI_CMAKE_CONFIG_DIR}/qdmi-config-version.cmake")
  set(QDMI_CMAKE_PROJECT_CONFIG_FILE
      "${QDMI_CMAKE_CONFIG_DIR}/qdmi-config.cmake")
  set(QDMI_CMAKE_PROJECT_TARGETS_FILE
      "${QDMI_CMAKE_CONFIG_DIR}/qdmi-targets.cmake")
endif()

if(NOT TARGET qdmi)
  add_library(
    qdmi SHARED
    ${QDMI_INCLUDE_BUILD_DIR}/qdmi.h ${QDMI_INCLUDE_BUILD_DIR}/qdmi_backend.h
    qdmi_core.c qdmi_internal.c qdmi_stubs.c)

  if(NOT BUILD_SHARED_LIBS)
    target_compile_definitions(qdmi PUBLIC QDMI_STATIC_DEFINE)
  endif()

  # set include directories
  target_include_directories(
    qdmi PUBLIC $<BUILD_INTERFACE:${QDMI_INCLUDE_BUILD_DIR}>)

  # add warnings to the library
  target_link_libraries(qdmi PRIVATE qinfo::project_warnings)

  # link to qinfo
  target_link_libraries(qdmi PUBLIC qinfo::qinfo)

  # set required C standard
  target_compile_features(qdmi PUBLIC c_std_11)

  # always include debug symbols (avoids common problems with LTO)
  target_compile_options(qdmi PUBLIC -g)

  # enable coverage collection options
  option(ENABLE_COVERAGE "Enable coverage reporting for gcc/clang" FALSE)
  if(ENABLE_COVERAGE)
    target_compile_options(qdmi PUBLIC --coverage -fprofile-arcs
                                       -ftest-coverage -O0)
    target_link_libraries(qdmi PUBLIC gcov --coverage)
  endif()

  include(CheckCCompilerFlag)
  check_c_compiler_flag(-mtune=native HAS_MTUNE_NATIVE)
  if(HAS_MTUNE_NATIVE)
    target_compile_options(qdmi PRIVATE -mtune=native)
  endif()

  check_c_compiler_flag(-march=native HAS_MARCH_NATIVE)
  if(HAS_MARCH_NATIVE)
    target_compile_options(qdmi PRIVATE -march=native)
  endif()

  # set the version of the library
  set_target_properties(
    qdmi
    PROPERTIES
      VERSION ${PROJECT_VERSION}
      SOVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
      PUBLIC_HEADER
      "${QDMI_INCLUDE_BUILD_DIR}/qdmi.h;${QDMI_INCLUDE_BUILD_DIR}/qdmi_backend.h"
  )

  # add alias
  add_library(qdmi::qdmi ALIAS qdmi)
endif()

# Installation instructions for the main library
if(QDMI_INSTALL)
  include(CMakePackageConfigHelpers)
  configure_package_config_file(
    ${QDMI_CMAKE_CONFIG_TEMPLATE} ${QDMI_CMAKE_PROJECT_CONFIG_FILE}
    INSTALL_DESTINATION ${QDMI_CONFIG_INSTALL_DIR}
    NO_SET_AND_CHECK_MACRO NO_CHECK_REQUIRED_COMPONENTS_MACRO)
  write_basic_package_version_file(
    ${QDMI_CMAKE_VERSION_CONFIG_FILE}
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMinorVersion)

  install(FILES ${QDMI_CMAKE_PROJECT_CONFIG_FILE}
                ${QDMI_CMAKE_VERSION_CONFIG_FILE}
          DESTINATION ${QDMI_CONFIG_INSTALL_DIR})

  install(
    TARGETS qdmi
    EXPORT ${QDMI_TARGETS_EXPORT_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT QDMI_Runtime
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            COMPONENT QDMI_Runtime
            NAMELINK_COMPONENT QDMI_Development
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT QDMI_Development
    PUBLIC_HEADER DESTINATION ${QDMI_INCLUDE_INSTALL_DIR})

  install(
    EXPORT ${QDMI_TARGETS_EXPORT_NAME}
    FILE ${QDMI_PROJECT_TARGETS_FILE}
    NAMESPACE qdmi::
    DESTINATION ${QDMI_CONFIG_INSTALL_DIR}
    COMPONENT QDMI_Development)
endif()
