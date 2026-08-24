# Examples

<!-- IMPORTANT: Keep the line above as the first line. -->

<!-- This file is a static page and included in the ./CMakeLists.txt file. -->

This page contains example implementations of devices and other components of
the software stack that use QDMI. All examples distributed with QDMI are
contained in the `examples/` directory in the repository.

\tableofcontents

## Implementing a Client Driver {#client-driver}

A Client driver is a replaceable shared library. It exports every function in
`qdmi/client.h` with `QDMI_DRIVER_EXPORT`, including
`QDMI_driver_get_client_abi_version`. A loader accepts a version if and only if
its major and minor fields equal those of `QDMI_CLIENT_ABI_VERSION`; the patch
field does not affect compatibility. The loader then resolves the complete
Client Interface before it calls `QDMI_session_alloc`. The ABI query is
side-effect free. Session allocation initializes the driver lazily and returns a
null handle on failure.

The example driver's `QDMI_CONF` file contains one device per line:

```text
/path/to/libdevice.so PREFIX deployment.device-id
```

The third field is the nonempty client-visible `QDMI_DEVICE_PROPERTY_ID`. IDs
must be unique in the configured catalog. The driver reads and validates the
complete file transactionally when it allocates the first session. A failed
allocation can be retried with a corrected file. Device libraries can omit this
property because the Client driver owns the public ID.

## Implementing a Device {#device}

Below you find mock implementations of a QDMI device in C++.

\note Keep in mind, that even though the interface is defined in C, the device
can be implemented in C++ or any other language that supports the C ABI.

### Basic String Properties {#device-string}

Every device has to provide a name, its version, and the implemented QDMI
library version through the query interface. The corresponding properties are

- @ref QDMI_DEVICE_PROPERTY_NAME
- @ref QDMI_DEVICE_PROPERTY_VERSION
- @ref QDMI_DEVICE_PROPERTY_LIBRARYVERSION

All of those properties are of type `char*` (string). Since they are properties
of the device, they are returned by the @ref
QDMI_device_session_query_device_property function. Below you find the
respective implementation in C++.

<!-- rumdl-disable -->
\dontinclude cxx_device.cpp
\skip int CXX_QDMI_device_session_query_device_property
\until QDMI_DEVICE_PROPERTY_LIBRARYVERSION
\until size_ret)
\skip QDMI_ERROR_NOTSUPPORTED
\until DOXYGEN FUNCTION END
<!-- rumdl-enable -->

Both implementations use an auxiliary macro to add the string properties to the
device. For an explanation of the macro, see the next section
[Auxiliary Macros](#device-macros).

### Auxiliary Macros {#device-macros}

The following macro is used to add string properties to the device. The macro is
used, e.g., in the implementation of the @ref
QDMI_device_session_query_device_property function.

<!-- rumdl-disable -->
\dontinclude cxx_device.cpp
\skip #define ADD_STRING_PROPERTY
\until DOXYGEN MACRO END
<!-- rumdl-enable -->

A similar macro is defined for other (fixed length) data types, for example,
`int`, `double`.

<!-- rumdl-disable -->
\dontinclude cxx_device.cpp
\skip #define ADD_SINGLE_VALUE_PROPERTY
\until DOXYGEN MACRO END
<!-- rumdl-enable -->

Another macro is defined for list properties of the data types above.

<!-- rumdl-disable -->
\dontinclude cxx_device.cpp
\skip #define ADD_LIST_PROPERTY
\until DOXYGEN MACRO END
<!-- rumdl-enable -->

The usage of the two latter macros is demonstrated in the following sections.

### Integer or Enumeration Properties {#device-int-enumeration}

The following two examples demonstrate how to return integer or enumeration
properties of the device.

<!-- rumdl-disable -->
\dontinclude cxx_device.cpp
\skip int CXX_QDMI_device_session_query_device_property
\until {
\skip QDMI_DEVICE_PROPERTY_STATUS
\until QDMI_DEVICE_PROPERTY_QUBITSNUM
\until size_ret)
\skip QDMI_ERROR_NOTSUPPORTED
\until DOXYGEN FUNCTION END
<!-- rumdl-enable -->

### List Properties {#device-list}

Some properties are returned as a list of various data types. The following
example shows how to return the coupling map of the device as a list of @ref
QDMI_Site pairs. The pairs are flattened into a single list of @ref QDMI_Site's.

<!-- rumdl-disable -->
\dontinclude cxx_device.cpp
\skipline constexpr std::array<const CXX_QDMI_Site_impl_d *, 20>
\skip DEVICE_COUPLING_MAP
\until ;
\skip int CXX_QDMI_device_session_query_device_property
\until {
\skip ADD_LIST_PROPERTY
\until DOXYGEN FUNCTION END
<!-- rumdl-enable -->

### Program-Format Execution Features {#device-program-format-features}

The @ref QDMI_device_session_query_program_features function reports atomic
execution features for one exact program-format descriptor. The following
example device reports two unrestricted OpenQASM features and forward branching
with a maximum nesting depth of one. Its QIR Base descriptors return a
successful empty list because they support no optional feature beyond the QIR
Base baseline.

<!-- rumdl-disable -->
\dontinclude cxx_device.cpp
\skipline QASM2_FEATURES{
\until };
\skip int CXX_QDMI_device_session_query_program_features
\until {
\until DOXYGEN FUNCTION END
<!-- rumdl-enable -->

Each @ref QDMI_Program_Feature record carries a feature ID, a feature-specific
value, and an optional typed constraint. Records for one feature and value form
one conjunctive group. An empty constraint ID means unrestricted support. Use
@ref QDMI_PROGRAM_FEATURE_UNCONSTRAINED to initialize such a record. Unknown or
malformed constraints make the group unusable. The returned list is complete.
Returning @ref QDMI_ERROR_NOTSUPPORTED keeps feature metadata unknown.
Requirements guaranteed by a standard descriptor remain implicit.

### Complex Properties {#device-complex}

The properties that are returned by @ref
QDMI_device_session_query_operation_property may depend on the actual site. The
available @ref QDMI_Operation's and @ref QDMI_Site's, first, need to be
retrieved through @ref QDMI_device_session_query_device_property. With the
handles for a @ref QDMI_Operation and @ref QDMI_Site, corresponding properties
can be queried. The following example demonstrates how different properties of
operations, for example, varying fidelities of two-qubit gates can be returned.

<!-- rumdl-disable -->
\dontinclude cxx_device.cpp
\skip QDMI_Pair_hash
\until OPERATION_FIDELITIES
\until ;
\skip QDMI_device_session_query_operation_property
\until DOXYGEN FUNCTION END
<!-- rumdl-enable -->

### Submitting a Job {#device-submit}

One crucial part of QDMI is that it allows submitting a job to the device for
execution. The following example provides a mock implementation of the necessary
functions to submit a job. The first example shows a mock implementation of @ref
QDMI_device_session_create_device_job.

<!-- rumdl-disable -->
\dontinclude cxx_device.cpp
\skip QDMI_device_session_create_device_job
\until DOXYGEN FUNCTION END
<!-- rumdl-enable -->

The function @ref QDMI_device_job_set_parameter allows setting different
parameters for the job, for example, the number of shots (@ref
QDMI_JOB_PARAMETER_SHOTSNUM).

<!-- rumdl-disable -->
\dontinclude cxx_device.cpp
\skip QDMI_device_job_set_parameter
\until DOXYGEN FUNCTION END
<!-- rumdl-enable -->

After the job is set up, it can be submitted to the device. The following
example shows a mock implementation of @ref QDMI_device_job_submit.

<!-- rumdl-disable -->
\dontinclude cxx_device.cpp
\skip QDMI_device_job_submit
\until DOXYGEN FUNCTION END
<!-- rumdl-enable -->

For the full implementation of the example devices we refer to the respective
source files in the QDMI repository, that is,
[`cxx_device.cpp`](https://github.com/Munich-Quantum-Software-Stack/QDMI/blob/develop/examples/device/cxx_device.cpp)
for the C++ implementation.
