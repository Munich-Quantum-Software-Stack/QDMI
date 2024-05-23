cmake_minimum_required(VERSION 3.9)

project(qdmi VERSION 0.1 DESCRIPTION "QDMI Core Library")

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/../install CACHE PATH "..." FORCE)
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_INSTALL_PREFIX}/cmake")
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_INSTALL_PREFIX}/cmake")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


include_directories("${CMAKE_INSTALL_PREFIX}/include")
include(GNUInstallDirs)
include(CTest)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

find_package(qinfo REQUIRED)

add_compile_options("-g")

find_package(LLVM REQUIRED CONFIG)
add_definitions(${LLVM_DEFINITIONS})

add_library(QDMI SHARED src/qdmi_core.c src/qdmi_internal.c src/qdmi_stubs.c)
target_link_libraries(QDMI qinfo LLVM ${CMAKE_DL_LIBS})

set_target_properties(QDMI PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    PUBLIC_HEADER include/qdmi.h
    PUBLUC HEADER
    )

target_include_directories(QDMI PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    ${LLVM_INCLUDE_DIRS}
)

install(
    TARGETS QDMI
    EXPORT qdmiConfig
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

export(
    TARGETS QDMI
    NAMESPACE qdmi::
    FILE "${CMAKE_CURRENT_BINARY_DIR}/qdmionfig.cmake"
)

install(
    EXPORT qdmiConfig
    FILE qdmiConfig.cmake
    DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake"
    NAMESPACE qdmi::
)

install(DIRECTORY include/ DESTINATION include)

add_executable(QDMIOpenClose testing/openclose.c)
target_link_libraries(QDMIOpenClose QDMI)
add_test(QDMIOpenClose QDMIOpenClose)


