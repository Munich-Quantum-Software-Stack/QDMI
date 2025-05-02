# Upgrade Guide

This document describes breaking changes and how to upgrade. For a complete list of changes
including minor and patch releases, please refer to the [changelog](CHANGELOG.md).

## [Unreleased]

### General Changes

The enum value `QDMI_SITE_PROPERTY_ID` has been renamed to `QDMI_SITE_PROPERTY_INDEX`.

### Device-side Changes

Devices now also need to implement the function `QDMI_device_job_query_job_property`. The function
`QDMI_device_job_wait`must not block longer than the duration in seconds specified in the
`QDMI_DEVICE_SESSION_PARAMETER_TIMEOUT` parameter. Other changes can be resolved by recompiling the
device together with the headers of the latest QDMI version.

### Driver-side Changes

Devices now also need to implement the function `QDMI_job_query_job_property`. The function
`QDMI_device_job_wait` may now also return after a timeout resulting in a new status
`QDMI_JOB_STATUS_TIMEOUT` that must be handled. The function `QDMI_job_wait` must not block longer
than the duration in seconds specified in the `QDMI_SESSION_PARAMETER_TIMEOUT` parameter. Other
changes to the interface can be resolved by recompiling the driver with the headers of the latest
QDMI version.

### Client-side Changes

The function `QDMI_job_wait` may now also return after a timeout resulting in a new status
`QDMI_JOB_STATUS_TIMEOUT` that must be handled. Other changes to the interface can be resolved by
recompiling the client with the headers of the latest QDMI version.

[unreleased]: https://github.com/Munich-Quantum-Software-Stack/QDMI/compare/v1.1.0...HEAD
