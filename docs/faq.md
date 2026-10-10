# FAQ

## What is QDMI?

QDMI is a C interface for managing quantum devices. It connects an application's
Client Interface calls to device implementations through a driver. Its session,
query, and job interfaces cover configuration, capability discovery, submission,
and result retrieval. See the
[architecture](rationale.md).

## Where is QDMI used, and who develops it?

QDMI grew out of Munich Quantum Valley and is used in the Munich Quantum
Software Stack (MQSS). It also contributes to reference-architecture and
integration work across Germany, Europe, and the international openQSE
community. See [Ecosystem and Community](ecosystem.md) for deployments,
implementations, and opportunities to participate.

QDMI was created jointly by TUM's Chairs of Design Automation and Computer
Architecture and Parallel Systems and the Leibniz Supercomputing Centre. It is
maintained by Munich Quantum Valley gGmbH and MQSC, with contributions from the
wider community. Participation is open to organizations and individuals
worldwide.

## Does installing QDMI let me run a quantum program?

The interface package supplies headers and CMake helpers. Executing calls
requires a compatible driver and device implementation. For an implementation
smoke test with synthetic results, use the
[bundled mock device](getting_started.md).
For execution APIs and a local simulator, see
[MQT Core](https://mqt.readthedocs.io/projects/core/en/latest/qdmi/index.html).

## Why is it written in C and not in Python?

The C interface lets native HPC software, drivers, and device libraries share a
common ABI. Opaque handles keep implementation details behind that boundary.
Other languages can use it through bindings, including the C++ and Python
wrappers supplied by MQT Core.

## Can I use QDMI from Python?

Yes. QDMI uses a C ABI that other languages can call. MQT Core provides C++ and
Python wrappers, plus Qiskit and PennyLane adapters. These wrappers and adapters
are implementation facilities, rather than part of the QDMI C specification.

## Does every device support the same programs and results?

No. Query the selected device's capabilities, including accepted program
formats, and check the return codes of optional queries and operations.
Available results depend on the program, device, and execution mode. For
example, statevectors are simulator capabilities, rather than hardware
measurement results.

## How do I select compatible versions?

Use the headers and documentation corresponding to your driver and device
libraries. QDMI minor releases can introduce breaking changes. The Client
Interface provides an ABI query; loaders compare major and minor versions and
ignore patch differences. See the
[CMake installation guide](installation.md),
[architecture](rationale.md),
and
[upgrade guide](../UPGRADING.md).

## Where can I get help, contribute, or find citation information?

Use
[Support](support.md)
for questions and bug reports and
[Contributing](contributing.md)
for contributions. See the @ref ecosystem-publications "publications" for
citation information and the
[license](https://github.com/Munich-Quantum-Software-Stack/QDMI/blob/develop/LICENSE)
for reuse terms.
