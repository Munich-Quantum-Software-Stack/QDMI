# QDMI {#mainpage}

<!-- IMPORTANT: Keep the line above as the first line and do not remove the label above. -->

## Quantum Device Management Interface

\snippet{doc} README.md DOXYGEN MAIN

## Choose a Starting Point

| Your goal                          | Guide                                                                                                                                                                                                              |
| ---------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| Evaluate or adopt QDMI             | [Ecosystem and Community](ecosystem.md): deployments, integrations, publications, stewardship, and participation                                                                                                   |
| Develop applications or middleware | The @ref client_interface, [architecture](rationale.md), and [MQT Core guides](https://mqt.readthedocs.io/projects/core/en/latest/qdmi/index.html) for drivers, C++/Python bindings, compilation, and SDK adapters |
| Connect hardware or services       | The @ref device_interface, [examples](examples.md), [device template](templates.md), and [implementation smoke test](getting_started.md)                                                                           |
| Operate infrastructure             | @ref ecosystem-operations "Deployment responsibilities", provider configuration and authentication guides, and [MQT Core's Slurm integration](https://mqt.readthedocs.io/projects/core/en/latest/qdmi/slurm.html)  |

## From Interface to Execution

QDMI provides C11 headers for two interfaces: applications call the @ref
client_interface implemented by a driver; device libraries implement the @ref
device_interface called by that driver. Both cover sessions, queries, and jobs.
The [architecture and rationale](rationale.md) explains compatibility, stable
device IDs, and handle ownership. [Using QDMI with CMake](installation.md)
covers the header package and build helpers.

To execute circuits locally, follow
[MQT Core's simulator tutorial](https://mqt.readthedocs.io/projects/core/en/latest/tutorials/qdmi_execution.html).
MQT Core supplies a driver, C++ and Python APIs, DDSIM execution, and Qiskit and
PennyLane adapters. Its
[compilation guide](https://mqt.readthedocs.io/projects/core/en/latest/compilation/index.html)
explains preparing programs for a target. These facilities build on QDMI; their
APIs and configuration formats belong to MQT Core.

The bundled [mock workflow](getting_started.md) checks an implementation's
session, discovery, submission, and result plumbing with synthetic results. For
external devices, choose an @ref ecosystem-implementations "implementation and
its deployment guide". Installing QDMI's header package separately is needed for
developing a consumer or implementation, rather than as an extra step for
packaged runtime users.

## Versions and Further Reading

Use the version selector to choose documentation for your QDMI release. The
[installation guide](installation.md) explains selecting an interface version.
Match the documentation, driver, and device versions, and consult the
[upgrade guide](../UPGRADING.md) when moving between minor versions.

The @ref ecosystem-publications "publications" provide historical and
architectural context. The headers and documentation for your selected version
define its API. See the [FAQ](faq.md), [Support](support.md), and
[Contributing](contributing.md) for further questions and participation.
