# Create a shared library that contains a test suite that can be used to test
# whether all functions are implemented in the device.
add_library(qdmi_test_impl SHARED test_impl.cpp test_impl.hpp)
target_link_libraries(
  qdmi_test_impl PRIVATE GTest::gtest GTest::gmock qdmi::qdmi
                         qdmi::example_driver qdmi::project_warnings)
target_include_directories(qdmi_test_impl PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(qdmi_test_impl PRIVATE cxx_std_17)
add_library(qdmi::test_impl ALIAS qdmi_test_impl)

# build gtest as position independent code to link it with the shared library
set_target_properties(gtest PROPERTIES POSITION_INDEPENDENT_CODE ON)
