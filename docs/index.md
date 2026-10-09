# QDMI {#mainpage}

<!-- IMPORTANT: Keep the line above as the first line and do not remove the label above. -->

## Quantum Device Management Interface

\snippet{doc} README.md DOXYGEN MAIN

QDMI provides C11 headers for two interfaces: applications use the @ref
client_interface implemented by a driver; device libraries implement the @ref
device_interface called by that driver. Both cover sessions, queries, and jobs.
The [architecture and rationale](rationale.md) explains these components, stable
device IDs, and their lifetimes.

## Choose a Starting Point

| Your goal                     | Guide                                                                                                                              |
| ----------------------------- | ---------------------------------------------------------------------------------------------------------------------------------- |
| Try QDMI locally              | [Getting Started](getting_started.md): build the examples and run discovery, submission, and result checks against the mock device |
| Add QDMI headers to a project | [Using QDMI with CMake](installation.md)                                                                                           |
| Implement a device or driver  | [Examples](examples.md) and [device template](templates.md)                                                                        |
| Understand the contracts      | [Architecture and rationale](rationale.md) and the @ref client_interface and @ref device_interface references                      |
| Find answers or contribute    | [FAQ](faq.md), [Support](support.md), and [Contributing](contributing.md)                                                          |

## Use QDMI in an Application

[MQT Core](https://mqt.readthedocs.io/projects/core/en/latest/qdmi/index.html)
supplies a driver, C++ and Python APIs, local DDSIM execution, and Qiskit and
PennyLane adapters. Its
[compilation guide](https://mqt.readthedocs.io/projects/core/en/latest/compilation/index.html)
explains preparing a program for a target, and its
[Slurm guide](https://mqt.readthedocs.io/projects/core/en/latest/qdmi/slurm.html)
covers scheduler integration. These facilities build on QDMI; their APIs and
configuration formats belong to MQT Core.

For external devices, start with the implementation's own documentation:

- [Amazon Braket](https://amazon-braket-qdmi-device.readthedocs.io/en/latest/):
  installation, gate-model device catalogue, AWS credentials, and result
  storage.
- [IQM](https://iqm-finland.github.io/QDMI-on-IQM/): installation, server
  selection, authentication, calibration, and HPC deployment.

Install a compatible runtime using those instructions. Installing QDMI's header
package separately is needed when developing a consumer or an implementation,
rather than as an extra runtime setup step for packaged users.

## Versions and Background

This documentation describes **QDMI 1.4**. Select the documentation version
matching your installation and consult the [upgrade guide](../UPGRADING.md) when
moving between minor versions. Match the interface version to the driver and
device libraries in your installation.

The @ref rationale-background "background papers" explain QDMI's role within
MQSS and its cloud and real-hardware integrations. They describe particular
versions and deployments; this documentation and the corresponding headers
define the API for the selected version.
