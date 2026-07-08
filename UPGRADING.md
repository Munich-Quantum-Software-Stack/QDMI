# Upgrade Guide

This document describes breaking changes and provides guidance for upgrading between versions. For a
complete list of changes, including minor and patch releases, please refer to the
[changelog](CHANGELOG.md).

## [Unreleased]

### CMake presets

[CMake presets] have been added to provide a standardized and reproducible way to configure builds across different platforms.
These presets are also used in our CI.

On Unix systems, the `debug`, `release`, and `coverage` presets can be used to configure, build, and test MQT DDSIM.

```console
cmake --preset release
cmake --build --preset release
ctest --preset release
```

Additionally, the `lint` preset can be used to configure and build MQT DDSIM in preparation for a `clang-tidy` run.

### Child device representations in a multicore architecture

The type used to represent core / processing-unit related information has been changed. A new device library implementation specific opaque pointer type named `QDMI_Child_Device` has been introduced for the `QDMI_DEVICE_PROPERTY_CHILDDEVICES` data queried via the *device_query_interface*. 
- Autors of a multicore device library must change the internal type used to represent child devices accordingly. 
- Authors of a driver library are now free to implement the client related type `QDMI_Device` without device library related constraints.

## [1.3.1] - 2026-05-21

### Support for multicore architectures

This release adds three new properties and parameters to the interface to enable support for multicore architectures:

- **`QDMI_DEVICE_PROPERTY_CHILDDEVICES`**: A device property that allows clients to query the child devices (cores, processing units, etc.) of a top-level device. This enables clients to retrieve relevant information about each child device, such as its status and fidelity data.
- **`QDMI_DEVICE_SESSION_PARAMETER_CHILDDEVICE`**: A session parameter that allows clients to specify the child device to use when creating a session. This ensures correct handling of the child device handle in the QDMI driver.
- **`QDMI_PROGRAM_FORMAT_BATCHJOB`**: A new program format that allows clients to submit a batch job that combines multiple jobs for different child devices into a single batch via the top-level device. This enables the top-level device to manage and synchronize job execution across all child devices.

The respective changes are additive and non-breaking, but device implementations that want to support multicore architectures will need to implement the new properties and parameters.

## [1.3.0] - 2026-04-21

### No need to link devices against the QDMI header-only library

As of this release, the QDMI header-only library is no longer required to be linked into QDMI device implementations.
Instead, each QDMI device now bundles all necessary headers in its own include directory.
This allows distributing any QDMI device implementation in a truly standalone manner.
You can remove the `qdmi::qdmi` target from the CMake configuration of your device implementation.

To properly resolve the imports, you will need to adjust the header includes in your device implementation.
Any include of the form

```c++
#include <qdmi/constants.h>
```

must be replaced with

```c++
#include <my_qdmi/constants.h>
```

where `my` is the prefix of your device implementation.

### New header for managing exported symbols of device implementations

Device implementations may now be compiled with hidden symbol visibility, which is a common best practice for C++ libraries to reduce symbol clashes and improve load times.
In practice, this means that only symbols explicitly marked for export are accessible from outside the device library.

The header `my_qdmi/export.h` (where `my` is your device prefix) provides the export macro `MY_QDMI_EXPORT`.
Use this header to control, which symbols are part of your public API.

All QDMI interface functions are already marked for export in `my_qdmi/device.h`.
You only need to add `MY_QDMI_EXPORT` manually for additional custom public functions.

To enable this behavior, add the following CMake code to your device target's configuration:

```cmake
target_compile_definitions(${QDMI_TARGET_NAME} PRIVATE MY_QDMI_device_EXPORTS)
set_target_properties(${QDMI_TARGET_NAME} PROPERTIES
                      C_VISIBILITY_PRESET hidden
                      CXX_VISIBILITY_PRESET hidden
                      VISIBILITY_INLINES_HIDDEN 1)
```

where `MY` is your device prefix.

## [1.2.2] - 2026-04-20

### Updated QDMI device template

The QDMI device template has been updated to be compatible with the latest version of the QDMI specification and to include several improvements.
Most importantly, as described above, the device implementation no longer needs to link against the QDMI header-only library and is now built with hidden symbol visibility by default.

Beyond that, the template has been extended to include:

- More precise licensing information
- A default `cache-keys` setup for `uv` to enable automatic rebuilds
- A more user-friendly CLI with exclusive groups for the CLI options
- More default cibuildwheel configuration for broad macOS compatibility and Windows wheel repairs
- Symbol exports on Windows to ensure the device DLL exposes all symbols
- Fixes for the installation instructions so that component-based installation works correctly
- Fixes for running tests on Windows
- Easier coverage collection configuration via the new `qdmi::qdmi_coverage_flags` target

## [1.2.1] - 2025-12-22

### Fix for using Installed Version of QDMI

In order to avoid a mismatch in the target name for the `qdmi_project_warnings` target between the
source and installed versions of QDMI, the target alias has been adjusted. If your project relies on
`qdmi::project_warnings`, you need to change it to `qdmi::qdmi_project_warnings`. More rationale is
available in [the PR description](https://github.com/Munich-Quantum-Software-Stack/QDMI/pull/270).

### Changes to Prefix Handling

The CMake functionality for handling the prefixing of QDMI devices was refactored to improve the
usability and flexibility. Particularly, the `generate_device_defs_executable` CMake function has
been changed to allow the optional specification of the QDMI device target to link via a `TARGET`
keyword argument. Example usage:

```cmake
generate_device_defs_executable("my_prefix" TARGET my_device)
```

This change is expected to be fully backwards compatible.

### Updated QDMI device template

The QDMI device template has been significantly updated to be more useful and provide a fully
fletched starting point for new devices. This includes updating the existing template code with
the latest best practices, including updating the CMake version range to 3.24-4.2 and using C++20
features.

In addition to these basic changes, the template has been extended to include:

- installation instructions for the device library
- boilerplate documentation infrastructure
- a thin Python wrapper for distributing the device implementation
- linter and formatter configuration
- automatic license header generation
- and more.

The corresponding documentation has been updated to reflect these changes.

In addition, the template instantiation workflow has changed: the template files are no longer
written as a side effect of the CMake configure step. Instead, configuration only defines the
prefix and output path, and the actual file generation happens when the explicit build target
`qdmi-template` is invoked (use `qdmi-template-clean` to overwrite an existing output directory).

## [1.2.0] - 2025-12-01

Version 1.2.0 introduces several breaking changes, primarily related to type system improvements,
duration/length unit handling, and API enhancements for neutral atom devices. Please review all
sections carefully when upgrading.

### New Program Formats (Non-Breaking)

Two new program formats were added to the `QDMI_Program_Format` enum to support additional quantum
programming frameworks:

- **`QDMI_PROGRAM_FORMAT_QPY`**: Binary representation of a Qiskit `QuantumCircuit` using the QPY
  format
- **`QDMI_PROGRAM_FORMAT_IQMJSON`**: IQM's proprietary data transfer format serialized as JSON

These additions are backward-compatible and do not require changes to existing code.

### Units for Length and Duration (Breaking Change)

**Breaking Change**: Length and duration properties now use integer types (`int64_t` or `uint64_t`)
instead of `double`, and represent values in device-specific units rather than standard SI units.

#### What Changed

- All duration-related properties now return integer values in device-specific units
- All length-related properties now return integer values in device-specific units
- The raw integer values must be interpreted using unit metadata provided by the device

#### Required Changes for Device Implementations

Device implementations **must** now provide the following properties to define their unit system:

- **`QDMI_DEVICE_PROPERTY_LENGTHUNIT`**: String describing the length unit (e.g., "m", "μm", "nm")
- **`QDMI_DEVICE_PROPERTY_DURATIONUNIT`**: String describing the duration unit (e.g., "s", "ms",
  "ns")
- **`QDMI_DEVICE_PROPERTY_LENGTHSCALEFACTOR`**: Multiplier to apply to apply to raw length values to
  convert to the device's length unit
- **`QDMI_DEVICE_PROPERTY_DURATIONSCALEFACTOR`**: Multiplier to apply to raw duration values to
  convert to the device's duration unit

#### Migration Example

**Before (v1.1.0):**

```c
double t1;
QDMI_device_query_site_property(
      device, site, QDMI_SITE_PROPERTY_T1, sizeof(double), &t1, nullptr);
// T1 time in us
```

**After (v1.2.0):**

```c
uint64_t t1 = 0;
QDMI_device_query_site_property(
  device, site, QDMI_SITE_PROPERTY_T1, sizeof(uint64_t), &t1, nullptr);

double scale_factor = 0.0;
QDMI_device_query_device_property(
  device, QDMI_DEVICE_PROPERTY_DURATIONSCALEFACTOR, sizeof(double),
  &scale_factor, nullptr);

// T1 time in device duration units
double t1_in_device_units = static_cast<double>(t1) * scale_factor;
```

To get the device's duration unit, use the `QDMI_DEVICE_PROPERTY_DURATIONUNIT` property.

```c++
size_t size = 0;
QDMI_device_query_device_property(
  device, QDMI_DEVICE_PROPERTY_DURATIONUNIT, 0, nullptr, &size);
std::string unit(size - 1, '\0');
QDMI_device_query_device_property(
  device, QDMI_DEVICE_PROPERTY_DURATIONUNIT, size, unit.data(),
  nullptr);
```

### Property Naming and New Properties (Partially Breaking)

#### Breaking Change: Property Rename

- **`QDMI_SITE_PROPERTY_ID`** has been renamed to **`QDMI_SITE_PROPERTY_INDEX`** for improved naming
  consistency

**Migration:** Replace all occurrences of `QDMI_SITE_PROPERTY_ID` with `QDMI_SITE_PROPERTY_INDEX` in
your codebase.

#### New Properties (Non-Breaking)

- **`QDMI_OPERATION_PROPERTY_SITES`**: Returns the list of sites to which an operation applies
- **`QDMI_DEVICE_PROPERTY_SUPPORTEDPROGRAMFORMATS`**: Returns the list of supported program formats
  for the device. Devices are highly encouraged to implement support for this property.
- **`QDMI_DEVICE_PROPERTY_PULSESUPPORT`**: Returns the degree of pulse-level control support using
  the `QDMI_Device_Pulse_Support_Level` enum
  - Devices without pulse support should return `QDMI_DEVICE_PULSE_SUPPORT_LEVEL_NONE`
  - Other levels include `LEVEL_SITE`, `LEVEL_CHANNEL`, and `LEVEL_SITEANDCHANNEL`

### Neutral Atom Device Properties (Non-Breaking)

Version 1.2.0 adds comprehensive support for neutral atom quantum computing platforms through a set
of new device, site, and operation properties. These additions are non-breaking and optional for
non-neutral-atom devices.

#### Device-Level Properties

- **`QDMI_DEVICE_PROPERTY_MINATOMDISTANCE`**: Minimum allowed distance between atoms (in device
  length units)

#### Site-Level Properties

- **`QDMI_SITE_PROPERTY_XCOORDINATE`**, **`YCOORDINATE`**, **`ZCOORDINATE`**: Spatial coordinates of
  sites in device coordinate system
- **`QDMI_SITE_PROPERTY_ISZONE`**: Boolean indicating whether a site is a zone (flexible atom
  placement area) or fixed position
- **`QDMI_SITE_PROPERTY_XEXTENT`**, **`YEXTENT`**, **`ZEXTENT`**: Physical extent of zone sites
  (returns `QDMI_ERROR_NOTSUPPORTED` for regular fixed-position sites)
- **`QDMI_SITE_PROPERTY_MODULEINDEX`**, **`SUBMODULEINDEX`**: Hierarchical module/submodule
  assignment for modular device architectures

#### Operation-Level Properties

- **`QDMI_OPERATION_PROPERTY_INTERACTIONRADIUS`**: Radius within which an operation affects nearby
  atoms
- **`QDMI_OPERATION_PROPERTY_BLOCKINGRADIUS`**: Radius within which other operations are blocked
  during execution
- **`QDMI_OPERATION_PROPERTY_ISZONED`**: Boolean indicating whether the operation is applied to a
  zone (area) rather than fixed sites
- **`QDMI_OPERATION_PROPERTY_IDLINGFIDELITY`**: Fidelity of idling atoms within the operation's
  interaction area (relevant for zoned operations)
- **`QDMI_OPERATION_PROPERTY_MEANSHUTTLINGSPEED`**: Average speed for atom movement operations

These properties enable precise modeling of neutral atom device capabilities.

### Job Property Query and Timeout (Breaking Change)

#### New Job Property Query Functions (Non-Breaking)

Two new functions have been added for querying job properties:

- **`QDMI_job_query_property`**: Client-side function to query job properties
- **`QDMI_device_job_query_property`**: Device-side implementation function

These functions allow clients to retrieve job metadata and previously set parameter values, enabling
better job tracking and debugging.

#### Breaking Change: Timeout Parameter Required

**Breaking Change**: The functions `QDMI_job_wait` and `QDMI_device_job_wait` now require an
additional `timeout` parameter.

**Function Signatures:**

```c
// Before (v1.1.0)
int QDMI_job_wait(QDMI_Job job);
int QDMI_device_job_wait(QDMI_Device_Job job);

// After (v1.2.0)
int QDMI_job_wait(QDMI_Job job, size_t timeout);
int QDMI_device_job_wait(QDMI_Device_Job job, size_t timeout);
```

**Timeout Parameter:**

- Type: `size_t`
- Unit: Seconds
- Value `0`: Wait indefinitely (equivalent to old behavior)
- Non-zero: Maximum wait time in seconds
- New return code: `QDMI_ERROR_TIMEOUT` when timeout expires before job completion

**Migration:**

```c
// To maintain v1.1.0 behavior (indefinite wait):
QDMI_job_wait(job, 0);

// Or specify an explicit timeout (e.g., 30 seconds):
int ret = QDMI_job_wait(job, 30);
if (ret == QDMI_ERROR_TIMEOUT) {
    // Handle timeout case
}
```

### Authentication Options (Breaking Change)

#### New Authentication Parameters (Non-Breaking)

Four new authentication options have been added to `QDMI_SESSION_PARAMETER` and
`QDMI_DEVICE_SESSION_PARAMETER` enums to support diverse authentication mechanisms:

- **`QDMI_SESSION_PARAMETER_AUTHFILE`**: Path to a file containing authentication credentials
- **`QDMI_SESSION_PARAMETER_AUTHURL`**: URL endpoint for authentication
- **`QDMI_SESSION_PARAMETER_USERNAME`**: Username for authentication
- **`QDMI_SESSION_PARAMETER_PASSWORD`**: Password for authentication

#### Breaking Change: Enum Value Ordering

**Breaking Change**: The addition of new authentication options has changed the numeric values of
existing enum entries in `QDMI_SESSION_PARAMETER` and `QDMI_DEVICE_SESSION_PARAMETER`.

**Impact:**

- Code that relies on specific numeric values of enum constants will break
- Code using the enum symbolic names will continue to work correctly

**Migration:**

- **Recommended**: Always use symbolic enum names (e.g., `QDMI_SESSION_PARAMETER_HOST`) rather than
  numeric values
- If numeric values were stored or transmitted, update serialization/deserialization code to use
  version-aware mapping

**Device Implementation Requirements:**

- Device implementations should document the authentication parameters they support
- Unsupported parameters should return `QDMI_ERROR_NOTSUPPORTED`
- New parameters are optional; existing authentication mechanisms remain valid

### Job Status Enum Updates (Breaking Change)

**Breaking Change**: The `QDMI_JOB_STATUS` enum values have been reordered to better reflect the
typical job lifecycle progression.

**Impact:**

- Code relying on numeric values of `QDMI_JOB_STATUS` enum constants will break
- Code using symbolic names will continue to work correctly

**Migration:**

- Use symbolic enum names (e.g., `QDMI_JOB_STATUS_QUEUED`) instead of numeric values
- Review any code that performs numeric comparisons or ordering of job status values
- Update serialization/deserialization code to be version-aware

### CMake Version Requirement (Breaking Change)

**Breaking Change**: The minimum required CMake version has been raised from **3.19** to **3.24**.

**Migration:**

- Update your CMake installation to version 3.24 or later
- Update CI/CD pipelines and build documentation to reflect the new requirement
- Most modern Linux distributions and development environments provide CMake 3.24+

**Rationale:** This change enables better build system features and improved dependency management.

### Summary of Breaking Changes

For quick reference, here are all breaking changes in v1.2.0:

1. **Duration/length properties**: Changed from `double` to integer types with device-specific units
2. **`QDMI_SITE_PROPERTY_ID`**: Renamed to `QDMI_SITE_PROPERTY_INDEX`
3. **Job wait functions**: Now require `timeout` parameter
4. **`QDMI_SESSION_PARAMETER` enums**: Reordered due to new authentication options
5. **`QDMI_JOB_STATUS` enum**: Reordered to reflect job lifecycle
6. **CMake**: Minimum version raised to 3.24

<!-- Version links -->

[unreleased]: https://github.com/Munich-Quantum-Software-Stack/QDMI/compare/v1.3.1...HEAD
[1.3.1]: https://github.com/Munich-Quantum-Software-Stack/QDMI/compare/v1.3.0...v1.3.1
[1.3.0]: https://github.com/Munich-Quantum-Software-Stack/QDMI/compare/v1.2.2...v1.3.0
[1.2.2]: https://github.com/Munich-Quantum-Software-Stack/QDMI/compare/v1.2.1...v1.2.2
[1.2.1]: https://github.com/Munich-Quantum-Software-Stack/QDMI/compare/v1.2.0...v1.2.1
[1.2.0]: https://github.com/Munich-Quantum-Software-Stack/QDMI/compare/v1.1.0...v1.2.0

<!-- Other links -->

[CMake presets]: https://cmake.org/cmake/help/latest/manual/cmake-presets.7.html
