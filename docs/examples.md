# Examples

<!-- IMPORTANT: Keep the line above as the first line. -->
<!-- This file is a static page and included in the ./CMakeLists.txt file. -->

This page contains example implementations of devices and other components of the software stack
that use QDMI. All examples distributed with QDMI are contained in the `examples/` directory in the
repository.

\tableofcontents

## Implementing a Device {#device}

Below you find a mock implementation of two QDMI devices: One is implemented in C and the other one
in C++. Keep in mind, that even though the device can be implemented in C++ it has to provide a C
interface.

### Basic String Properties {#device-string}

Every device has to provide a name, its version, and the implemented QDMI library version through
the query interface. The corresponding properties are

- @ref QDMI_DEVICE_PROPERTY_NAME
- @ref QDMI_DEVICE_PROPERTY_VERSION
- @ref QDMI_DEVICE_PROPERTY_LIBRARYVERSION

All of those properties are of type `char*` (string). Hence, they are returned by the @ref
QDMI_query_device_property_dev function. Below you find the respective implementation in C and C++.

<!-- prettier-ignore-start -->
<div class="tabbed">
- <b class="tab-title">C</b>
  \dontinclude device.c
  \skip QDMI_query_device_property_dev
  \until QDMI_DEVICE_PROPERTY_LIBRARYVERSION
  \until size_ret)
- <b class="tab-title">C++</b>
  \dontinclude device.cpp
  \skip QDMI_query_device_property_dev
  \until QDMI_DEVICE_PROPERTY_LIBRARYVERSION
  \until size_ret)
</div>
<!-- prettier-ignore-end -->

Both implementations use an auxiliary macro to add the string properties to the device. For an
explanation of the macro, see the next section [Auxiliary Macros](#device-macros).

### Auxiliary Macros {#device-macros}

The following macro is used to add string properties to the device. The macro is used, e.g., in the
implementation of the @ref QDMI_query_device_property_dev function.

<!-- prettier-ignore-start -->
<div class="tabbed">
- <b class="tab-title">C</b>
  \dontinclude device.c
  \skip #define ADD_STRING_PROPERTY
  \until return QDMI_SUCCESS;
  \until }
  \until }
- <b class="tab-title">C++</b>
  \dontinclude device.cpp
  \skip #define ADD_STRING_PROPERTY
  \until return QDMI_SUCCESS;
  \until }
  \until }
</div>
<!-- prettier-ignore-end -->

A similar macro is defined for other (fixed length) data types, e.g., `int`, `double`.

<!-- prettier-ignore-start -->
<div class="tabbed">
- <b class="tab-title">C</b>
  \dontinclude device.c
  \skip #define ADD_SINGLE_VALUE_PROPERTY
  \until return QDMI_SUCCESS;
  \until }
  \until }
- <b class="tab-title">C++</b>
  \dontinclude device.cpp
  \skip #define ADD_SINGLE_VALUE_PROPERTY
  \until return QDMI_SUCCESS;
  \until }
  \until }
</div>
<!-- prettier-ignore-end -->

Another macro is defined for list properties of the data types above.

<!-- prettier-ignore-start -->
<div class="tabbed">
- <b class="tab-title">C</b>
  \dontinclude device.c
  \skip #define ADD_LIST_PROPERTY
  \until return QDMI_SUCCESS;
  \until }
  \until }
- <b class="tab-title">C++</b>
  \dontinclude device.cpp
  \skip #define ADD_LIST_PROPERTY
  \until return QDMI_SUCCESS;
  \until }
  \until }
</div>
<!-- prettier-ignore-end -->

The usage of the two latter macros is demonstrated in the following sections.

### Integer or Enumeration Properties {#device-int-enumeration}

The following two examples demonstrate how to return integer or enumeration properties of the
device.

<!-- prettier-ignore-start -->
<div class="tabbed">
- <b class="tab-title">C</b>
  \dontinclude device.c
  \skip QDMI_query_device_property_dev
  \until {
  \skip QDMI_DEVICE_PROPERTY_QUBITSNUM
  \until QDMI_DEVICE_PROPERTY_STATUS
  \until size_ret)
- <b class="tab-title">C++</b>
  \dontinclude device.cpp
  \skip QDMI_query_device_property_dev
  \until {
  \skip QDMI_DEVICE_PROPERTY_QUBITSNUM
  \until QDMI_DEVICE_PROPERTY_STATUS
  \until size_ret)
</div>
<!-- prettier-ignore-end -->

### List Properties {#device-list}

Some properties are returned as a list of various data types. The following example shows how to
return the coupling map of the device as a list of pairs of @ref QDMI_Site's. The pairs are
flattened into a single list of @ref QDMI_Site's.

<!-- prettier-ignore-start -->
<div class="tabbed">
- <b class="tab-title">C</b>
  \dontinclude device.c
  \skip QDMI_query_device_property_dev
  \until {
  \skip ADD_LIST_PROPERTY
  \until size_ret)
- <b class="tab-title">C++</b>
  \dontinclude device.cpp
  \skipline constexpr static std::array<const QDMI_Site_impl_d *const, 20>
  \skip DEVICE_COUPLING_MAP
  \until ;
  \skip QDMI_query_device_property_dev
  \until {
  \skip QDMI_DEVICE_PROPERTY_COUPLINGMAP
  \until size_ret)
</div>
<!-- prettier-ignore-end -->

### Complex Properties {#device-complex}

The properties that are returned by @ref QDMI_query_operation_property_dev may depend on the actual
site. The available @ref QDMI_Operation operations and @ref QDMI_Site sites, first, need to be
retrieved through @ref QDMI_query_get_operations_dev and @ref QDMI_query_get_sites_dev,
respectively. Following is an example of how to implement the @ref QDMI_query_get_sites_dev
function.

<!-- prettier-ignore-start -->
<div class="tabbed">
- <b class="tab-title">C</b>
  \dontinclude device.c
  \skip DEVICE_SITES
  \until ;
  \skip QDMI_query_get_sites_dev
  \until DOXYGEN FUNCTION END
- <b class="tab-title">C++</b>
  \dontinclude device.cpp
  \skip device_sites
  \until ;
  \skip QDMI_query_get_sites_dev
  \until DOXYGEN FUNCTION END
</div>
<!-- prettier-ignore-end -->

With the handles for @ref QDMI_Operation and @ref QDMI_Site, corresponding properties can be
queried. The following example demonstrates how different properties of operations, e.g., varying
fidelities of two-qubit gates can be returned.

<!-- prettier-ignore-start -->
<div class="tabbed">
- <b class="tab-title">C</b>
  \dontinclude device.c
  \skip QDMI_query_operation_property_dev
  \until DOXYGEN FUNCTION END
- <b class="tab-title">C++</b>
  \dontinclude device.cpp
  \skip QDMI_Pair_hash
  \until OPERATION_FIDELITIES
  \until ;
  \skip QDMI_query_operation_property_dev
  \until DOXYGEN FUNCTION END
</div>
<!-- prettier-ignore-end -->

### Submitting a Job {#device-submit}

One crucial part of QDMI is, that it allows to submit a job to the device for execution. The
following example provides a mock implementation of the necessary functions to submit a job. The
first example shows a mock implementation of @ref QDMI_control_create_job_dev.

<!-- prettier-ignore-start -->
<div class="tabbed">
- <b class="tab-title">C</b>
  \dontinclude device.c
  \skip QDMI_control_create_job_dev
  \until DOXYGEN FUNCTION END
- <b class="tab-title">C++</b>
  \dontinclude device.cpp
  \skip QDMI_control_create_job_dev
  \until DOXYGEN FUNCTION END
</div>
<!-- prettier-ignore-end -->

The function @ref QDMI_control_set_parameter_dev allows to set different parameters for the job,
e.g., the number of shots (@ref QDMI_JOB_PARAMETER_SHOTS_NUM).

<!-- prettier-ignore-start -->
<div class="tabbed">
- <b class="tab-title">C</b>
  \dontinclude device.c
  \skip QDMI_control_set_parameter_dev
  \until DOXYGEN FUNCTION END
- <b class="tab-title">C++</b>
  \dontinclude device.cpp
  \skip QDMI_control_set_parameter_dev
  \until DOXYGEN FUNCTION END
</div>
<!-- prettier-ignore-end -->

After the job is set up, it can be submitted to the device. The following example shows a mock
implementation of @ref QDMI_control_submit_job_dev.

<!-- prettier-ignore-start -->
<div class="tabbed">
- <b class="tab-title">C</b>
  \dontinclude device.c
  \skip QDMI_control_submit_job_dev
  \until DOXYGEN FUNCTION END
- <b class="tab-title">C++</b>
  \dontinclude device.cpp
  \skip QDMI_control_submit_job_dev
  \until DOXYGEN FUNCTION END
</div>
<!-- prettier-ignore-end -->
