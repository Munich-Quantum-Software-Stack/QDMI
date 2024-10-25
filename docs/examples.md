# Examples

<!-- IMPORTANT: Keep the line above as the first line. -->
<!-- This file is a static page and included in the ./CMakeLists.txt file. -->

This page contains example implementations of devices and other components of the software stack
that use QDMI. Those example implementations can serve as templates. All examples distributed with
QDMI are contained in the `examples/` directory in the repository.

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
QDMI_query_device_property function. Below you find the respective implementation in C and C++.

<!-- prettier-ignore-start -->
<div class="tabbed">
- <b class="tab-title">C</b>
  \dontinclude device.c
  \skip QDMI_query_device_property_dev
  \until DOXYGEN FUNCTION END
- <b class="tab-title">C++</b>
  \dontinclude device.cpp
  \skip QDMI_query_device_property_dev
  \until DOXYGEN FUNCTION END
</div>
<!-- prettier-ignore-end -->

### Double and Integer Properties {#device-double-int}

Following two examples for returning `double` and `int` properties.

<!-- prettier-ignore-start -->
<div class="tabbed">
- <b class="tab-title">C</b>
  \dontinclude device.c
  \skip QDMI_query_device_property_dev
  \until DOXYGEN FUNCTION END
- <b class="tab-title">C++</b>
  \dontinclude device.cpp
  \skip QDMI_query_device_property_dev
  \until DOXYGEN FUNCTION END
</div>
<!-- prettier-ignore-end -->

### List Properties {#device-list}

Some properties are returned as a list of the various data type. The following example shows how to
return the list of available gates which are represented as strings, i.e., `char*` in the C
language.

<!-- prettier-ignore-start -->
<div class="tabbed">
- <b class="tab-title">C</b>
  \dontinclude device.c
  \skip QDMI_query_device_property_dev
  \until DOXYGEN FUNCTION END
- <b class="tab-title">C++</b>
  \dontinclude device.cpp
  \skip QDMI_query_device_property_dev
  \until DOXYGEN FUNCTION END
</div>
<!-- prettier-ignore-end -->

Another example is the coupling map which is represented as a list of pairs of integers.

<!-- prettier-ignore-start -->
<div class="tabbed">
- <b class="tab-title">C</b>
  \dontinclude device.c
  \skip QDMI_query_device_property_dev
  \until DOXYGEN FUNCTION END
- <b class="tab-title">C++</b>
  \dontinclude device.cpp
  \skip QDMI_query_device_property_dev
  \until DOXYGEN FUNCTION END
</div>
<!-- prettier-ignore-end -->

### Complex Properties {#device-complex}

The properties that are returned by @ref QDMI_query_operation_property_dev may depend on the actual
site. Hence, the implementation of those functions can be more complex. In the following example, we
demonstrate how varying fidelities of two-qubit gates can be returned.

<!-- prettier-ignore-start -->
<div class="tabbed">
- <b class="tab-title">C</b>
  \dontinclude device.c
  \skip QDMI_query_operation_property_dev
  \until DOXYGEN FUNCTION END
- <b class="tab-title">C++</b>
  \dontinclude device.cpp
  \skip pair_hash
  \until DOXYGEN FUNCTION END
</div>
<!-- prettier-ignore-end -->

### Submitting a Job {#device-submit}

One crucial part of QDMI is, that it allows to submit a job to the device for execution. The
following example provides a mock implementation of the @ref QDMI_control_submit_job_dev function.

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
