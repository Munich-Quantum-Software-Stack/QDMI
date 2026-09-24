# Examples

<!-- IMPORTANT: Keep the line above as the first line. -->

<!-- This file is a static page and included in the ./CMakeLists.txt file. -->

This page contains example implementations of devices and other components of
the software stack that use QDMI. All examples distributed with QDMI are
contained in the `examples/` directory in the repository.

\tableofcontents

## Implementing a Device {#device}

Below you find mock implementations of a QDMI device in C++.

The example device demonstrates API calls and result serialization. It does not
interpret the submitted program: it generates random shot data and a separate
random statevector. Consequently, it is not a simulator or a reference for
program execution semantics.

### Result Ordering {#device-result-ordering}

A real device must implement the output mapping documented in @ref
QDMI_PROGRAM_FORMAT_T and the serialization documented in @ref
QDMI_JOB_RESULT_T. First select the submitted program's complete output. If it
is entirely binary, identify its logical output bits and serialize them with bit
zero on the right. Do not infer output positions from physical qubit order,
measurement-key names, or a backend response's object-member order.

These asymmetric examples distinguish output selection from bit direction:

| Program output                                                                      | Logical output bits, starting at bit zero | QDMI shot / histogram key |
| ----------------------------------------------------------------------------------- | ----------------------------------------- | ------------------------- |
| `creg c[3];`, with only `c[0]` set to one                                           | `1, 0, 0`                                 | `001`                     |
| `creg a[2]; creg b[3];`, with only `a[0]` and `b[1]` set to one                     | `1, 0, 0, 1, 0`                           | `01001`                   |
| Measure qubit zero in state one into `c[2]`, with the other two output bits zero    | `0, 0, 1`                                 | `100`                     |
| QIR records result one, Boolean false, result zero                                  | `1, 0, 0`                                 | `001`                     |
| QIR records the same result one twice, then result zero                             | `1, 1, 0`                                 | `011`                     |
| IQM JSON measures locus `[QB3, QB1]` yielding `[1, 0]`, then `[QB2]` yielding `[0]` | `1, 0, 0`                                 | `001`                     |

OpenQASM outputs contain the final values of the selected classical variables.
QIR outputs contain values at the executed recording calls. For example, two
OpenQASM measurements that overwrite the same classical bit produce one output
slot; recording both measurements in QIR produces two slots. A compiler must
preserve the source's observable outputs when translating between formats.

For dense and sparse quantum-state results, qubit zero in state one and all
other qubits in state zero gives basis index `1` and key `001` in a three-qubit
system. This remains true even if the program measures that qubit into `c[2]`
and its shot is `100`.

#### Complete Program Output

QIR and OpenQASM 3 can return more than measurement bits. If any shot contains
nonbinary output, all three binary result queries (`SHOTS`, `HIST_KEYS`, and
`HIST_VALUES`) return `QDMI_ERROR_NOTSUPPORTED`, including size queries. A
recorded integer is nonbinary even when its value is zero or one. Successful
execution remains successful; the device must provide the corresponding
full-output result for accepted nonbinary outputs. It must not discard values or
shots to manufacture a bitstring distribution.

For QIR, @ref QDMI_JOB_RESULT_QIR_OUTPUT returns the standard ordered or labeled
output stream, including its headers, shot boundaries, types, containers, and
applicable labels. For example, recording a tuple of result one and integer 42
retains both values in that stream; requesting a bitstring does not return `1`.
Recording result one followed by Boolean false is entirely binary and has the
QDMI shot `01`. The full stream retains its schema's ordering. An ordered
`RESULT_ARRAY` with elements `[1, 0, 0]` contains `100` in the stream, while its
QDMI shot is `001`. Asynchronous labeled results require reconstruction from the
program's recording semantics before binary retrieval; neither arrival order nor
label sorting is a substitute.

For OpenQASM 3, follow the language's output selection: explicit `output`
declarations select only those variables; without them, all declared classical
variables are outputs, subject to the language's scoping rules. QDMI does not
exclude inputs or constants. For example:

```qasm
OPENQASM 3.0;
output bit[2] bits;
output bool accepted;
output int[32] count;
bits = "01";
accepted = true;
count = 42;
```

One execution returns the following @ref QDMI_JOB_RESULT_QASM3_OUTPUT JSON:

```json
[{"bits": [1, 0], "accepted": true, "count": 42}]
```

The integer output makes binary retrieval unsupported. If only `bits` is
declared as an `output`, the JSON is `[{"bits":[1,0]}]` and its binary shot is
`01`; the other variables are internal. If none of the declarations has the
`output` modifier, all three variables are selected again. Compilers must
preserve this selection when introducing temporary variables or translating
between languages.

JSON registers and arrays retain their dimensions and increasing index order;
they are not reversed like bitstrings. Integers retain their exact value, floats
retain sufficient precision to round-trip, and undefined values are `null`. Any
undefined selected value makes binary retrieval unsupported for the entire job.
The full encoding and supported types are specified in @ref
QDMI_JOB_RESULT_QASM3_OUTPUT. Unsupported selected types must be rejected as
unsupported program features, not omitted. Full-output support is optional for
binary-only programs. When both representations are available, they describe the
same executions, not independent samples.

This separation follows established interfaces that preserve typed output:
[CUDA-Q distinguishes sampling from typed return values][cudaq-results],
[Microsoft exposes structured shot and histogram outcomes][azure-results], and
[Quantinuum provides explicit bitstring conversions][quantinuum-results].
QIR defines an [output-stream schema][qir-output]; OpenQASM defines
[output selection][qasm-output], but not a common output wire format. QDMI's
JSON encoding is a defined subset, informed by
[Braket's separate per-shot output field][braket-output], rather than a claim
that arbitrary typed-output flattening is a community convention.

#### IQM Placement and Source Results

@ref QDMI_PROGRAM_FORMAT_IQMJSON accepts one circuit object containing `name`
and `instructions`. Shots, execution settings, and optional qubit mapping are
supplied through the device's job/session interface, rather than a full
`RunRequest` payload. Without a mapping, locus names identify physical sites by
@ref QDMI_SITE_PROPERTY_NAME. A mapping resolves logical names to physical site
names; it does not assign classical output positions.

For example, a circuit measures `[alice, bob]` under key `z`, then `[charlie]`
under key `a`. Suppose placement maps these names to `[QB3, QB1, QB2]`, and the
backend returns key order `[a, z]` with values `[0]` and `[1, 0]`. The logical
output is still `[1, 0, 0]`, giving QDMI shot `001`. Match the keys and columns
to the submitted instructions and loci before serialization. Neither physical
site order nor backend key order defines the output positions.

When translating to IQM JSON, listing a single terminal measurement's locus in
increasing logical output-bit order permits direct QDMI readout. An exporter
must not reverse that locus to compensate for a backend's raw presentation. More
general translations require the compiler or SDK adapter to retain a
source-output mapping, including register widths, source-initialized unmeasured
bits, and final values of overwritten classical destinations.

[IQM's Qiskit adapter][iqm-results] reconstructs classical registers using
measurement-key metadata. That is separate from physical qubit mapping and
belongs to the frontend integration; QDMI treats measurement keys as opaque
identifiers. Compare results at the source-language interface, including its
classical destinations and widths, rather than equating raw IQM bitstrings with
Qiskit results. An exporter unable to represent the source output must reject it
or retain the reconstruction mapping, not silently change it.

[CUDA-Q's IQM adapter][cudaq-iqm] likewise separates circuit emission, request
construction, and frontend result reconstruction. [QRMI][qrmi-iqm] transports
the full request. QDMI retains its circuit format and separate job settings;
converting that transport envelope alone does not preserve source outputs.

#### Conformance Cases

Device and adapter tests should cover the cases below. These are execution
conformance requirements for real implementations; the distributed mock tests
only exercise API behavior and result serialization.

- Asymmetric outputs, multiple registers, partial and repeated measurements,
  overwritten destinations, and equivalent programs across supported formats.
- QIR records ordered differently from result IDs, repeated recordings, nested
  binary containers, and variable-length outcomes. A numeric record on even one
  shot makes every binary result query unsupported for the job; full output
  preserves all shots and values.
- OpenQASM 3 explicit and default output selection, final classical assignments,
  multidimensional Boolean arrays, and undefined scalar or array elements. Check
  exact integers, round-trip floats, and full-output types.
- IQM placement permutations and scrambled response-key order, including
  multiple keys and multi-qubit loci. At the frontend, compare reconstructed
  results with the SDK for permuted destinations, output holes, and entirely
  unmeasured registers with source-defined initialization.
- Dense/sparse key-value correspondence and agreement between a job's shots and
  histogram. Bell-state outcomes `00` and `11` alone cannot detect reversal.

[cudaq-results]: https://nvidia.github.io/cuda-quantum/latest/using/examples/sample_vs_run.html
[azure-results]: https://github.com/microsoft/azure-quantum-python/blob/91d124fbd846d912fd5977f1cd2d2b9fe415603a/azure-quantum/azure/quantum/job/job.py
[quantinuum-results]: https://github.com/Quantinuum/hugr/blob/main/hugr-py/src/hugr/qsystem/result.py
[qir-output]: https://github.com/qir-alliance/qir-spec/tree/f5647346542d5a65225c3eb349847fe4df01d1b2/specification/output_schemas
[qasm-output]: https://openqasm.com/language/directives.html#input-output
[braket-output]: https://github.com/amazon-braket/amazon-braket-schemas-python/blob/5af70416e50305fb614e20dda3924e0739b32583/src/braket/task_result/gate_model_task_result_v1.py
[iqm-results]: https://github.com/iqm-finland/sdk/blob/d9850e628fe6b46991e33d8f421e50aea35a5e12/src/4.6.3/iqm-client/src/iqm/qiskit_iqm/iqm_job.py
[cudaq-iqm]: https://github.com/NVIDIA/cuda-quantum/blob/cb0c0602f772e7013de7f765f7fe147f45d651df/runtime/cudaq/platform/default/rest/helpers/iqm/IQMServerHelper.cpp
[qrmi-iqm]: https://github.com/qiskit-community/qrmi/blob/main/src/iqm/server.rs

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
