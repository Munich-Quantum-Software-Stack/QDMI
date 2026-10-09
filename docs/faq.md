# FAQ

## What is QDMI?

QDMI is a C interface for managing quantum devices. It connects an application's
Client Interface calls to device implementations through a driver. Its session,
query, and job interfaces cover configuration, capability discovery, submission,
and result retrieval. See the [architecture](rationale.md).

## What is MQSS, and who develops QDMI?

The [Munich Quantum Software Stack (MQSS)][mqss] connects quantum applications,
compilation and runtime infrastructure, and device implementations. QDMI is its
device-management interface and can also be used by other software stacks.

Contributors include the [Munich Quantum Software Company (MQSC)][mqsc], the TUM
Chairs of Design Automation and Computer Architecture and Parallel Systems, and
the Leibniz Supercomputing Centre within the Munich Quantum Valley initiative.
See [Support](support.md) for questions and bug reports.

## Does installing QDMI let me run a quantum program?

The interface package supplies headers and CMake helpers. Executing calls
requires a compatible driver and device implementation. For a local check, use
the [bundled mock device](getting_started.md). For execution APIs and a local
simulator, see
[MQT Core](https://mqt.readthedocs.io/projects/core/en/latest/qdmi/index.html).

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
libraries. QDMI minor releases can introduce breaking changes. QDMI 1.4 includes
a Client Interface ABI query; loaders compare major and minor versions and
ignore patch differences. See the [CMake installation guide](installation.md),
[architecture](rationale.md), and
[upgrade guide](../UPGRADING.md).

## Where can I get help, contribute, or find citation information?

Use [Support](support.md) for questions and bug reports and
[Contributing](contributing.md) for contributions. The
[README](https://github.com/Munich-Quantum-Software-Stack/QDMI#cite-qdmi)
contains the citation and license information.

[mqss]: https://www.munich-quantum-valley.de/research/research-areas/mqss
[mqsc]: https://mq.sc
