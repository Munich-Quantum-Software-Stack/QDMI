# Examples

<!-- IMPORTANT: Keep the line above as the first line. -->

<!----------------------------------------------------------------------------
Copyright 2024 Munich Quantum Software Stack Project

Licensed under the Apache License, Version 2.0 with LLVM Exceptions (the
"License"); you may not use this file except in compliance with the License.
You may obtain a copy of the License at

https://github.com/Munich-Quantum-Software-Stack/QDMI/blob/develop/LICENSE

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
License for the specific language governing permissions and limitations under
the License.

SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
-------------------------------------------------------------------------- -->

<!-- This file is a static page and included in the ./CMakeLists.txt file. -->

This page contains example implementations of devices and other components of the software stack
that use QDMI. All examples distributed with QDMI are contained in the `examples/` directory in the
repository.

\tableofcontents

## Implementing a Device {#device}

Below you find mock implementations of two QDMI devices: One is implemented in C++ and the other one
in C.

\note Keep in mind, that even though the interface is defined in C, the device can be implemented in
C++ or any other language that supports the C ABI.

### Basic String Properties {#device-string}

Every device has to provide a name, its version, and the implemented QDMI library version through
the query interface. The corresponding properties are

- @ref QDMI_DEVICE_PROPERTY_NAME
- @ref QDMI_DEVICE_PROPERTY_VERSION
- @ref QDMI_DEVICE_PROPERTY_LIBRARYVERSION

All of those properties are of type `char*` (string). Since they are properties of the device, they
are returned by the @ref QDMI_device_session_query_device_property function. Below you find the
respective implementation in C++ and C.

<!-- prettier-ignore-start -->
<div class="tabbed">
- <b class="tab-title">C++</b>
  \dontinclude device.cpp
  \skip int CXX_QDMI_device_session_query_device_property
  \until QDMI_DEVICE_PROPERTY_LIBRARYVERSION
  \until size_ret)
  \skip QDMI_ERROR_NOTSUPPORTED
  \until DOXYGEN FUNCTION END
- <b class="tab-title">C</b>
  \dontinclude device.c
  \skip int C_QDMI_device_session_query_device_property
  \until QDMI_DEVICE_PROPERTY_LIBRARYVERSION
  \until size_ret)
  \skip QDMI_ERROR_NOTSUPPORTED
  \until DOXYGEN FUNCTION END
</div>
<!-- prettier-ignore-end -->

Both implementations use an auxiliary macro to add the string properties to the device. For an
explanation of the macro, see the next section [Auxiliary Macros](#device-macros).

### Auxiliary Macros {#device-macros}

The following macro is used to add string properties to the device. The macro is used, e.g., in the
implementation of the @ref QDMI_device_session_query_device_property function.

<!-- prettier-ignore-start -->
<div class="tabbed">
- <b class="tab-title">C++</b>
  \dontinclude device.cpp
  \skip #define ADD_STRING_PROPERTY
  \until DOXYGEN MACRO END
- <b class="tab-title">C</b>
  \dontinclude device.c
  \skip #define ADD_STRING_PROPERTY
  \until DOXYGEN MACRO END
</div>
<!-- prettier-ignore-end -->

A similar macro is defined for other (fixed length) data types, e.g., `int`, `double`.

<!-- prettier-ignore-start -->
<div class="tabbed">
- <b class="tab-title">C++</b>
  \dontinclude device.cpp
  \skip #define ADD_SINGLE_VALUE_PROPERTY
  \until DOXYGEN MACRO END
- <b class="tab-title">C</b>
  \dontinclude device.c
  \skip #define ADD_SINGLE_VALUE_PROPERTY
  \until DOXYGEN MACRO END
</div>
<!-- prettier-ignore-end -->

Another macro is defined for list properties of the data types above.

<!-- prettier-ignore-start -->
<div class="tabbed">
- <b class="tab-title">C++</b>
  \dontinclude device.cpp
  \skip #define ADD_LIST_PROPERTY
  \until DOXYGEN MACRO END
- <b class="tab-title">C</b>
  \dontinclude device.c
  \skip #define ADD_LIST_PROPERTY
  \until DOXYGEN MACRO END
</div>
<!-- prettier-ignore-end -->

The usage of the two latter macros is demonstrated in the following sections.

### Integer or Enumeration Properties {#device-int-enumeration}

The following two examples demonstrate how to return integer or enumeration properties of the
device.

<!-- prettier-ignore-start -->
<div class="tabbed">
- <b class="tab-title">C++</b>
  \dontinclude device.cpp
  \skip int CXX_QDMI_device_session_query_device_property
  \until {
  \skip QDMI_DEVICE_PROPERTY_STATUS
  \until QDMI_DEVICE_PROPERTY_QUBITSNUM
  \until size_ret)
  \skip QDMI_ERROR_NOTSUPPORTED
  \until DOXYGEN FUNCTION END
- <b class="tab-title">C</b>
  \dontinclude device.c
  \skip int C_QDMI_device_session_query_device_property
  \until {
  \skip QDMI_DEVICE_PROPERTY_STATUS
  \until QDMI_DEVICE_PROPERTY_QUBITSNUM
  \until size_ret)
  \skip QDMI_ERROR_NOTSUPPORTED
  \until DOXYGEN FUNCTION END
</div>
<!-- prettier-ignore-end -->

### List Properties {#device-list}

Some properties are returned as a list of various data types. The following example shows how to
return the coupling map of the device as a list of pairs of @ref QDMI_Site's. The pairs are
flattened into a single list of @ref QDMI_Site's.

<!-- prettier-ignore-start -->
<div class="tabbed">
- <b class="tab-title">C++</b>
  \dontinclude device.cpp
  \skipline constexpr std::array<const CXX_QDMI_Site_impl_d *, 20>
  \skip DEVICE_COUPLING_MAP
  \until ;
  \skip int CXX_QDMI_device_session_query_device_property
  \until {
  \skip ADD_LIST_PROPERTY
  \until DOXYGEN FUNCTION END
- <b class="tab-title">C</b>
  \dontinclude device.c
  \skip int C_QDMI_device_session_query_device_property
  \until {
  \skip ADD_LIST_PROPERTY
  \until DOXYGEN FUNCTION END
</div>
<!-- prettier-ignore-end -->

### Complex Properties {#device-complex}

The properties that are returned by @ref QDMI_device_session_query_operation_property may depend on
the actual site. The available @ref QDMI_Operation's and @ref QDMI_Site's, first, need to be
retrieved through @ref QDMI_device_session_query_device_property. With the handles for a @ref
QDMI_Operation and @ref QDMI_Site, corresponding properties can be queried. The following example
demonstrates how different properties of operations, e.g., varying fidelities of two-qubit gates can
be returned.

<!-- prettier-ignore-start -->
<div class="tabbed">
- <b class="tab-title">C++</b>
  \dontinclude device.cpp
  \skip QDMI_Pair_hash
  \until OPERATION_FIDELITIES
  \until ;
  \skip QDMI_device_session_query_operation_property
  \until DOXYGEN FUNCTION END
- <b class="tab-title">C</b>
  \dontinclude device.c
  \skip QDMI_device_session_query_operation_property
  \until DOXYGEN FUNCTION END
</div>
<!-- prettier-ignore-end -->

### Submitting a Job {#device-submit}

One crucial part of QDMI is, that it allows to submit a job to the device for execution. The
following example provides a mock implementation of the necessary functions to submit a job. The
first example shows a mock implementation of @ref QDMI_device_session_create_device_job.

<!-- prettier-ignore-start -->
<div class="tabbed">
- <b class="tab-title">C++</b>
  \dontinclude device.cpp
  \skip QDMI_device_session_create_device_job
  \until DOXYGEN FUNCTION END
- <b class="tab-title">C</b>
  \dontinclude device.c
  \skip QDMI_device_session_create_device_job
  \until DOXYGEN FUNCTION END
</div>
<!-- prettier-ignore-end -->

The function @ref QDMI_device_job_set_parameter allows to set different parameters for the job,
e.g., the number of shots (@ref QDMI_JOB_PARAMETER_SHOTSNUM).

<!-- prettier-ignore-start -->
<div class="tabbed">
- <b class="tab-title">C++</b>
  \dontinclude device.cpp
  \skip QDMI_device_job_set_parameter
  \until DOXYGEN FUNCTION END
- <b class="tab-title">C</b>
  \dontinclude device.c
  \skip QDMI_device_job_set_parameter
  \until DOXYGEN FUNCTION END
</div>
<!-- prettier-ignore-end -->

After the job is set up, it can be submitted to the device. The following example shows a mock
implementation of @ref QDMI_device_job_submit.

<!-- prettier-ignore-start -->
<div class="tabbed">
- <b class="tab-title">C++</b>
  \dontinclude device.cpp
  \skip QDMI_device_job_submit
  \until DOXYGEN FUNCTION END
- <b class="tab-title">C</b>
  \dontinclude device.c
  \skip QDMI_device_job_submit
  \until DOXYGEN FUNCTION END
</div>
<!-- prettier-ignore-end -->

For the full implementation of the example devices we refer to the respective source files in the
QDMI repository, i.e.,
[`device.cpp`](https://github.com/Munich-Quantum-Software-Stack/QDMI/blob/develop/examples/device/cxx/device.cpp)
for the C++ implementation and
[`device.c`](https://github.com/Munich-Quantum-Software-Stack/QDMI/blob/develop/examples/device/c/device.c)
for the C implementation.
