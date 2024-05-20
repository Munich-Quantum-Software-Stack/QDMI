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
#include(CTest)

find_library(RABBITMQ_LIBRARY rabbitmq)
find_path(RABBITMQ_INCLUDE_DIR amqp.h)

if(NOT RABBITMQ_LIBRARY)
    message(FATAL_ERROR "RabbitMQ library not found")
endif()

if(NOT RABBITMQ_INCLUDE_DIR)
    message(FATAL_ERROR "RabbitMQ include directory not found")
endif()

add_compile_options("-g")

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

find_package(CURL REQUIRED)

find_package(qinfo REQUIRED)

find_package(jansson REQUIRED)

find_package(LLVM REQUIRED CONFIG)

find_package(cJSON REQUIRED)

add_compile_options("-g")

add_library(backend_qlm SHARED src/qdmi_backend_qlm.c)
target_link_libraries(backend_qlm PRIVATE qinfo jansson ${RABBITMQ_LIBRARY} LLVM)
target_compile_definitions(backend_qlm PRIVATE _GNU_SOURCE)

add_library(backend_ibm SHARED src/qdmi_backend_ibm.c)
target_link_libraries(backend_ibm PRIVATE qinfo jansson)

add_library(backend_wmi_real SHARED src/qdmi_backend_wmi_real.c)
target_link_libraries(backend_wmi_real PRIVATE qinfo CURL::libcurl)
target_link_libraries(backend_wmi_real PRIVATE qinfo cjson)

add_library(backend_wmi_sim SHARED src/qdmi_backend_wmi_sim.c)
target_link_libraries(backend_wmi_sim PRIVATE qinfo CURL::libcurl)
target_link_libraries(backend_wmi_sim PRIVATE qinfo cjson)

add_library(backend_q7 SHARED src/qdmi_backend_q7.c)
target_link_libraries(backend_q7 PRIVATE qinfo LLVM)

#### START planqc backend

# Find the openapi-generator program
find_program(OPENAPI_GENERATOR_EXECUTABLE NAMES openapi-generator)

if(OPENAPI_GENERATOR_EXECUTABLE)
    # Define the path to the generated project
    set(PLANQC_BACKEND_PATH ${CMAKE_CURRENT_SOURCE_DIR}/planqc_api/)
    set(PLANQC_API_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/planqc_api)

    message(STATUS "Generating planqcapi OpenAPI client files")
    # Execute the openapi-generator command during the configuration phase
    execute_process(
        COMMAND ${OPENAPI_GENERATOR_EXECUTABLE} generate --input-spec ${PLANQC_BACKEND_PATH}/planqc_openapi_v1.yml --generator-name c --config ${PLANQC_BACKEND_PATH}/openapi-generator-config.yml --output ${PLANQC_API_OUTPUT_PATH}
        RESULT_VARIABLE result
        OUTPUT_VARIABLE output
    )

    # Check the result of the command
    if(NOT ${result} EQUAL 0)
        message(FATAL_ERROR "openapi-generator failed: ${output}")
    else()
        message(STATUS "Successfully generated planqcapi OpenAPI client files: ${PLANQC_API_OUTPUT_PATH}")
    endif()

    # Include the generated project's CMakeLists.txt
    add_subdirectory(${PLANQC_API_OUTPUT_PATH} ${PLANQC_API_OUTPUT_PATH}/build)

    # Link the generated library with your backend_planqc library
    include_directories(${PLANQC_API_OUTPUT_PATH})
    add_library(backend_planqc SHARED src/qdmi_backend_planqc.c)
    target_link_libraries(backend_planqc PRIVATE qinfo planqcapi m)
else()
    message(WARNING "openapi-generator not found. Please install it to build the planqc backend. See planqc_api/README.md for more details.")
endif()

#### END planqc backend

set(PUBLIC_QDMI_HEADERS
    "include/qdmi.h"
    "include/qdmi_backend.h"
    "include/qdmi_internal.h"
)

set_target_properties(backend_qlm PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    PUBLIC_HEADER "${PUBLIC_QDMI_HEADERS}"
)

target_include_directories(backend_qlm PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include> ${LLVM_INCLUDE_DIRS}
)

set_target_properties(backend_ibm PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    PUBLIC_HEADER "${PUBLIC_QDMI_HEADERS}"
)

target_include_directories(backend_ibm PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

set_target_properties(backend_wmi_real PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    PUBLIC_HEADER "${PUBLIC_QDMI_HEADERS}"
)

set_target_properties(backend_wmi_sim PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    PUBLIC_HEADER "${PUBLIC_QDMI_HEADERS}"
)

target_include_directories(backend_wmi_real PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_include_directories(backend_wmi_sim PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

set_target_properties(backend_q7 PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    PUBLIC_HEADER "${PUBLIC_QDMI_HEADERS}"
)

target_include_directories(backend_q7 PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include> ${LLVM_INCLUDE_DIRS}
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
    TARGETS qdmi backend_qlm backend_ibm backend_wmi_real backend_wmi_sim backend_q7
    EXPORT qdmiConfig
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_PREFIX}/include
)

export(
    TARGETS qdmi backend_qlm backend_ibm backend_wmi_real backend_wmi_sim backend_q7
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


add_executable(test_planqc testing/qdmi_test_planqc.c)
target_link_libraries(test_planqc qdmi m)