#-------------------------------------------------------------------------------
# Part of the MQSS Project, under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.9)

project(qdmi VERSION 0.1 DESCRIPTION "QDMI Core Library")

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  #set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/../install CACHE PATH "..." FORCE)
  set(CMAKE_INSTALL_PREFIX $ENV{HOME}/bin CACHE PATH "..." FORCE)
endif()
list(APPEND CMAKE_MODULE_PATH "${CMAKE_INSTALL_PREFIX}/cmake")
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_INSTALL_PREFIX}/cmake")
include_directories("${CMAKE_INSTALL_PREFIX}/include")

include(GNUInstallDirs)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options("-g")

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

find_package(qinfo REQUIRED)

add_compile_options("-g")

set(PUBLIC_QDMI_HEADERS
    "include/qdmi.h"
    "include/qdmi_backend.h"
)

add_library(qdmi SHARED 
    src/qdmi_core.c 
    src/qdmi_internal.c 
    src/qdmi_stubs.c 
)

target_link_libraries(qdmi qinfo)

set_target_properties(qdmi PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    PUBLIC_HEADER "${PUBLIC_QDMI_HEADERS}"
)

# configure_file(qinfo.pc.in qinfo.pc @ONLY)

target_include_directories(qdmi PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

install(
    TARGETS qdmi
    EXPORT qdmiConfig
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_PREFIX}/include
)

export(
    TARGETS qdmi
    NAMESPACE qdmi::
    FILE "${CMAKE_CURRENT_BINARY_DIR}/qdmiConfig.cmake"
)

install(
    EXPORT qdmiConfig
    FILE qdmiConfig.cmake
    DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake"
    NAMESPACE qdmi::
)
    
# install(FILES ${CMAKE_BINARY_DIR}/mylib.pc
#     DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)

add_executable(QDMIOpenClose testing/openclose.c)
target_link_libraries(QDMIOpenClose qdmi)
#add_test(QDMIOpenClose QDMIOpenClose)
