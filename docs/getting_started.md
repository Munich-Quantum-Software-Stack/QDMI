# Getting Started

Start with the bundled **example driver and mock device**. This path checks
sessions, discovery, program submission, and result retrieval locally, without
cloud credentials, an HPC allocation, or access to quantum hardware. The mock
returns synthetic results; it does not simulate the supplied quantum circuit.

This page uses **QDMI 1.4**. The [CMake installation guide](installation.md)
separately shows how to install the interface headers. A driver and its devices
must implement the interface version you use; see the
[upgrade guide](../UPGRADING.md).

\tableofcontents

## Build the Local Examples {#getting-started-build}

You need Git, CMake 3.24 or newer, and C11 and C++20 compilers. Configuration
fetches GoogleTest and initially needs network access. From a new checkout:

```sh
git clone --depth 1 --branch v1.4.0 \
  https://github.com/Munich-Quantum-Software-Stack/QDMI.git
cd QDMI

cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_QDMI_TESTS=ON -DBUILD_QDMI_DOCS=OFF
cmake --build build --config Release --target qdmi_test
```

The target builds the example driver, example device, and template checks. All
use the headers from this checkout; no separate QDMI installation is required.

## Verify Discovery, Submission, and Results {#getting-started-checkpoint}

Run the existing integration checks:

```sh
ctest --test-dir build -C Release --output-on-failure \
  -R 'QDMIDriverLoadingTest|ClientVisibleDeviceIdIsStable|QueryDeviceProperties|JobLifecycle|GetHistogram'
```

CTest sets each test's working directory. The checks write their own local
device configuration, load the bundled library, initialize sessions, and
exercise the Client Interface. The checkpoint covers:

- missing libraries, invalid configuration, duplicate IDs, and retry after a
  failed driver allocation;
- discovering configured devices and querying their stable IDs and properties;
- copying an OpenQASM 2 program into a job, submitting it, waiting, and checking
  the final status;
- retrieving histogram keys and values for program index zero, checking their
  correspondence, and verifying that the counts sum to the requested shots.

Read-only sessions intentionally skip submission and result checks. The
read/write cases must pass. To run all checks:

```sh
ctest --test-dir build -C Release --output-on-failure
```

The assertions and API calls are in
[`test/test_qdmi.cpp`](https://github.com/Munich-Quantum-Software-Stack/QDMI/blob/develop/test/test_qdmi.cpp),
with session and configuration setup in
[`test/utils/test_impl.cpp`](https://github.com/Munich-Quantum-Software-Stack/QDMI/blob/develop/test/utils/test_impl.cpp).
These tests also run in QDMI's CI.

## Follow the Application Workflow {#getting-started-workflow}

An application talks to the driver through the @ref client_interface.

1. Allocate a @ref QDMI_Session with @ref QDMI_session_alloc, set the driver's
   supported session parameters, and call @ref QDMI_session_init.
2. Query @ref QDMI_SESSION_PROPERTY_DEVICES, then inspect each device's @ref
   QDMI_DEVICE_PROPERTY_ID, capabilities, sites, and operations. The returned
   handles belong to this session.
3. Prepare a program in a supported format. Create a @ref QDMI_Job with @ref
   QDMI_device_create_job, set parameters such as @ref
   QDMI_JOB_PARAMETER_SHOTSNUM, and call @ref QDMI_job_set_programs. In 1.4,
   even a single program is a list of length one. Text payload sizes include
   exactly one trailing null byte; binary payloads retain their bytes.
4. Submit with @ref QDMI_job_submit, then use @ref QDMI_job_check or @ref
   QDMI_job_wait to follow execution. Check the terminal status before
   interpreting the available results. Read them with @ref QDMI_job_get_results
   using the input program's index. Variable-size queries first request the
   required byte size, then fill an appropriately sized buffer.
5. Free the job with @ref QDMI_job_free, then release the session with @ref
   QDMI_session_free after its handles are no longer needed. Keep the driver
   library loaded throughout those lifetimes.

Check every return code. Supported formats, program counts, optional results,
and authentication methods depend on the selected implementation. The mock's
nonempty token selects read/write access; this is an example policy and needs no
real credentials.

## Diagnose Configuration and Loading Failures {#getting-started-diagnostics}

The example driver reads `QDMI_CONF`, or `qdmi.conf` in the current working
directory when the variable is unset. Each device line contains:

```text
/path/to/libdevice.so PREFIX deployment.device-id
```

Use the platform's library suffix (`.so`, `.dylib`, or `.dll`), the
implementation's exact symbol prefix, and a unique, nonempty ID. Relative
library paths are resolved from the process's working directory. The example
allows configuration files under the current working directory or the user's
home directory after resolving symlinks. This format and path policy belong to
the example driver; other drivers can choose different configuration.

| Symptom                                                  | Check                                                                                                                      |
| -------------------------------------------------------- | -------------------------------------------------------------------------------------------------------------------------- |
| Configuration cannot be opened or its path is rejected   | Check `QDMI_CONF`, the working directory, file permissions, and the example's allowed configuration directories.           |
| Device library cannot be loaded                          | Check its path, platform/architecture, and required shared-library dependencies.                                           |
| Required symbol cannot be resolved                       | Check the symbol prefix and that the library implements the selected QDMI version.                                         |
| Configuration is rejected                                | Supply exactly three fields per device line and unique IDs.                                                                |
| Session or job access is denied                          | Check the selected implementation's authentication and access policy. The mock requires a nonempty token for job creation. |
| A format, program count, query, or result is unsupported | Check capabilities and return codes; select a supported operation rather than assuming every optional feature is present.  |

Use trusted configuration files and libraries: loading a device library runs
native code in the application process. For the ABI check and handle ownership,
see the [architecture](rationale.md).

## Move to a Simulator or External Device {#getting-started-runtime}

[MQT Core's QDMI guides](https://mqt.readthedocs.io/projects/core/en/latest/qdmi/index.html)
cover its runtime installation, local DDSIM execution, C++ and Python APIs, and
SDK adapters. The
[QDMI execution tutorial](https://mqt.readthedocs.io/projects/core/en/latest/tutorials/qdmi_execution.html)
walks through device discovery, compilation, submission, and result checks using
the bundled DDSIM simulator. Its builtin driver supplies configured devices such
as `mqt.ddsim.default`; manifest discovery and `builtin_driver` helpers are Core
conveniences. Follow its
[configuration guide](https://mqt.readthedocs.io/projects/core/en/latest/qdmi/configuration.html)
for library registration and its
[Slurm guide](https://mqt.readthedocs.io/projects/core/en/latest/qdmi/slurm.html)
for scheduler integration.

For [Amazon Braket](https://amazon-braket-qdmi-device.readthedocs.io/en/latest/)
or [IQM](https://iqm-finland.github.io/QDMI-on-IQM/), use the device project's
installation and configuration guides for catalogues, endpoints, credentials,
and deployment. Match the runtime and device versions before submitting work.
Installing the standalone QDMI headers is not an additional runtime step when
using a packaged application stack.
