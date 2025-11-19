# Upgrade Guide

This document describes breaking changes and how to upgrade. For a complete list of changes
including minor and patch releases, please refer to the [changelog](CHANGELOG.md).

## [Unreleased]

### New Program Formats

Two new program formats were added to the `QDMI_Program_Format` enum:

- `QDMI_PROGRAM_FORMAT_QPY`: Binary representation of a Qiskit `QuantumCircuit` (QPY).
- `QDMI_PROGRAM_FORMAT_IQMJSON`: IQM data transfer format serialized as JSON.

### Units for Length and Duration (Breaking Change)

Length and duration properties now use `int64_t` or `uint64_t` (instead of `double`) and represent
values in device-specific units. Devices must provide:

- `QDMI_DEVICE_PROPERTY_LENGTHUNIT`
- `QDMI_DEVICE_PROPERTY_DURATIONUNIT`
- `QDMI_DEVICE_PROPERTY_LENGTHSCALEFACTOR`
- `QDMI_DEVICE_PROPERTY_DURATIONSCALEFACTOR`

These properties define the units and scale factors for interpreting raw values.

### Enum and Property Updates (Partially Breaking Change)

- **Breaking**: `QDMI_SITE_PROPERTY_ID` renamed to `QDMI_SITE_PROPERTY_INDEX`.
- New: `QDMI_OPERATION_PROPERTY_SITES` returns a list of sites for an operation.

### Neutral Atom Device Properties

New properties for neutral atom devices:

**Device:**

- `QDMI_DEVICE_PROPERTY_MINATOMDISTANCE`: Minimum atom distance.

**Site:**

- `QDMI_SITE_PROPERTY_{X,Y,Z}COORDINATE`: Site coordinates.
- `QDMI_SITE_PROPERTY_ISZONE`: Indicates zone site.
- `QDMI_SITE_PROPERTY_{X,Y,Z}EXTENT`: Zone site extent (`QDMI_ERROR_NOTSUPPORTED` for regular
  sites).
- `QDMI_SITE_PROPERTY_MODULEINDEX` / `SUBMODULEINDEX`: Module/submodule indices.

**Operation:**

- `QDMI_OPERATION_PROPERTY_INTERACTIONRADIUS` / `BLOCKINGRADIUS`: Radii for multi-qubit ops.
- `QDMI_OPERATION_PROPERTY_ISZONED`: Zoned operation indicator.
- `QDMI_OPERATION_PROPERTY_IDLINGFIDELITY`: Fidelity for idling atoms (zoned ops).
- `QDMI_OPERATION_PROPERTY_MEANSHUTTLINGSPEED`: Mean shuttling speed.

### Job Property Query and Timeout (Breaking Change)

- New function: `QDMI_job_query_property` (and device-side `QDMI_device_job_query_property`) for
  querying job properties.
- `QDMI_job_wait` and `QDMI_device_job_wait` now accept a `timeout` parameter (seconds, `0` =
  indefinite). May return `QDMI_ERROR_TIMEOUT`.

### Authentication Options

New authentication options added to session parameter enums:

- `AUTHFILE`: File with authentication info.
- `AUTHURL`: URL for authentication.
- `USERNAME`: Username.
- `PASSWORD`: Password.

Enum order updated; implementations must document supported options.

[unreleased]: https://github.com/Munich-Quantum-Software-Stack/QDMI/compare/v1.1.0...HEAD
