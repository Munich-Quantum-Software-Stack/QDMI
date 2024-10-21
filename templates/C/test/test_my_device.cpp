/*------------------------------------------------------------------------------
Part of the MQSS Project, under the Apache License v2.0 with LLVM Exceptions.
See https://llvm.org/LICENSE.txt for license information.
SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
------------------------------------------------------------------------------*/

#include "qdmi/interface.h"
#include "test_impl.hpp"

#include <cstddef>
#include <gtest/gtest.h>
#include <string>

// Instantiate the test suite with different parameters
INSTANTIATE_TEST_SUITE_P(QDMIMyCDevice,          // Custom instantiation name
                         QDMIImplementationTest, // Test suite name
                         // Parameters to test with
                         ::testing::Values("../examples/libmy_device"),
                         [](const testing::TestParamInfo<std::string> &inf) {
                           // Extract the last part of the file path
                           const size_t pos = inf.param.find_last_of("/\\");
                           std::string filename =
                               (pos == std::string::npos)
                                   ? inf.param
                                   : inf.param.substr(pos + 1);

                           // Strip the 'lib' prefix if it exists
                           const std::string prefix = "lib";
                           if (filename.compare(0, prefix.size(), prefix) ==
                               0) {
                             filename = filename.substr(prefix.size());
                           }

                           return filename;
                         });

class QDMITest : public ::testing::Test {
protected:
  void SetUp() override {
    ASSERT_EQ(QDMI_session_alloc(&session), QDMI_SUCCESS)
        << "Failed to allocate session";
  }

  void TearDown() override { QDMI_session_free(session); }

  QDMI_Session session = nullptr;
  QDMI_Device device = nullptr;
  const std::string my_device_name =
      std::string("./libmy_device") + Shared_library_file_extension();
};

TEST_F(QDMITest, OpenDevice) {
  ASSERT_EQ(QDMI_session_open_device(session, my_device_name.c_str(),
                                     QDMI_DEVICE_MODE_READ_WRITE, &device),
            QDMI_SUCCESS)
      << "Failed to open device";
}

TEST_F(QDMITest, QueryNumQubits) {
  QDMI_session_open_device(session, my_device_name.c_str(),
                           QDMI_DEVICE_MODE_READ_WRITE, &device);
  int num_qubits = 0;
  ASSERT_EQ(
      QDMI_query_device_property_int(device, QDMI_NUM_QUBITS, &num_qubits),
      QDMI_SUCCESS)
      << "Failed to query number of qubits";
  ASSERT_EQ(num_qubits, 5); // <-- TODO Insert the correct number of qubits here
}