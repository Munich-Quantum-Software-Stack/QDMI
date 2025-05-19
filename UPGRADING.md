# Upgrade Guide

This document describes breaking changes and how to upgrade. For a complete list of changes
including minor and patch releases, please refer to the [changelog](CHANGELOG.md).

## [Unreleased]

### General Changes

The enum value `QDMI_SITE_PROPERTY_ID` has been renamed to `QDMI_SITE_PROPERTY_INDEX`.

### Device-side Changes

Devices now also need to implement the function `QDMI_device_job_query_job_property` for querying properties of jobs.

A `QDMI_DEVICE_SESSION_PARAMETER_TIMEOUT` parameter has been added that influences how long (in seconds) the `QDMI_device_job_wait` function must wait for a result before returning with a `QDMI_ERROR_TIMEOUT` error.

For full compatibility with this QDMI version, the device needs to be recompiled with the latest header file versions.

### Driver-side Changes

Drivers now also need to implement the function `QDMI_job_query_job_property` for querying properties of jobs. 

The function `QDMI_device_job_wait` may now also return after a timeout resulting in a new status `QDMI_JOB_STATUS_TIMEOUT` that must be handled. 

A `QDMI_SESSION_PARAMETER_TIMEOUT` parameter has been added that influences how long (in seconds) the `QDMI_job_wait` function must wait for a result before returning with a `QDMI_ERROR_TIMEOUT` error.

For full compatibility with this QDMI version, the driver needs to be recompiled with the latest header file versions.

### Client-side Changes

A `QDMI_SESSION_PARAMETER_TIMEOUT` parameter has been added that influences how long (in seconds) the `QDMI_job_wait` will wait before eventually returning with `QDMI_ERROR_TIMEOUT`.

For full compatibility with this QDMI version, the client needs to be recompiled with the latest header file versions.

[unreleased]: https://github.com/Munich-Quantum-Software-Stack/QDMI/compare/v1.1.0...HEAD
