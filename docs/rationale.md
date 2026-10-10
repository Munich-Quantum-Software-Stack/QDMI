# Architecture and Rationale

<!-- IMPORTANT: Keep the line above as the first line. -->

<!-- This file is a static page and included in the ./CMakeLists.txt file. -->

QDMI separates applications, drivers, and device implementations through two C
interfaces. This page explains their responsibilities and the design choices
behind the interface.

\tableofcontents

## The Structure of QDMI {#rationale-structure}

<img class="qdmi-schematic" alt="QDMI Components and Interfaces" src="qdmi_schematic.svg"/>

QDMI connects an application to devices through a driver. The two public C
interfaces define the boundary between these components:

| Term                      | Responsibility                                                                                               |
| ------------------------- | ------------------------------------------------------------------------------------------------------------ |
| **Client**                | An application or software component using QDMI to query devices or submit work.                             |
| **Client Interface**      | The unprefixed functions in `qdmi/client.h`, implemented by the driver and called by clients.                |
| **Driver**                | An implementation of the Client Interface that exposes devices and mediates sessions, queries, and jobs.     |
| **Device Interface**      | The functions in `qdmi/device.h`, implemented with a device-specific symbol prefix and called by the driver. |
| **Device implementation** | A library translating the Device Interface into simulator operations, a hardware API, or a cloud service.    |
| **Application bindings**  | Language wrappers or SDK adapters above the Client Interface, such as MQT Core's C++ and Python APIs.        |

A device implementation can represent one physical system, a simulator, or a
configured service endpoint. A driver can load it as a shared library or link it
statically. QDMI's examples use shared libraries; QDMI's interface package
itself contains headers and CMake helpers, rather than a runtime implementation.

### Sessions and Handle Ownership

The client allocates a @ref QDMI_Session, configures it, and initializes it with
the driver. The driver creates @ref QDMI_Device_Session objects for its device
implementations. Each layer defines its own opaque handles and implements the
corresponding types and functions. Session parameters convey authentication or
configuration; supported parameters and access policies belong to the driver and
device implementation.

The driver exposes client-visible @ref QDMI_Device, @ref QDMI_Job, @ref
QDMI_Site, and @ref QDMI_Operation handles. Keep the originating session alive
while using its handles, free jobs before releasing their session, and use every
handle only with its originating driver. A dynamic loader must keep the driver
library loaded until all its sessions and derived handles have been released.

### Queries, Catalogues, and Stable IDs

The @ref client_query_interface exposes device properties, sites, operations,
and available calibration data through the driver. These queries describe the
selected device's capabilities and constraints; they inform compilation and
execution choices. A driver may cache or adapt information returned by a device.

A **catalogue** is the set of device definitions exposed by a particular driver
or installation. It can contain independently configured instances of the same
device library. The initialized session reports its accessible devices through
@ref QDMI_SESSION_PROPERTY_DEVICES. QDMI does not prescribe a catalogue file
format, installation location, or Python package discovery mechanism.

Each top-level device has a nonempty, stable @ref QDMI_DEVICE_PROPERTY_ID
supplied by the driver. IDs are unique within one initialized session and remain
fixed for the lifetime of their device handles. Equivalent sessions return the
same IDs across process restarts while the logical resources exist. Treat IDs as
opaque strings: compare or store them without deriving meaning from their
spelling. Device libraries may report a default ID, which the driver can
override; child-device IDs remain optional. Save the driver and configuration
context along with an ID. An ID alone does not identify a globally
interchangeable physical device or make a compiled program portable to another
target.

### Jobs and Results

The client creates a @ref QDMI_Job; the driver delegates work through @ref
QDMI_Device_Job objects. @ref QDMI_job_set_programs supplies one or more
programs with a common format and common job parameters. A device may reject
unsupported formats or program counts. Programs are copied before the setter
returns, and results are retrieved by the original input index. Execution order
is unspecified.

Submission, status checks, waiting, cancellation, and retrieval use the job
interface. Optional per-program status can identify individual outcomes. Check
aggregate and, where available, individual status before consuming results;
successful programs can retain results when other programs fail or are canceled.
Available result types depend on the device and execution mode. A driver may
translate formats or adapt results while preserving the Client Interface
contract. Compilation and scheduling policies are supplied by higher layers.

### Shared Libraries and Compatibility

In the Client Interface, a driver exports unprefixed Client Interface symbols
and @ref QDMI_driver_get_client_abi_version. A dynamic loader first calls that
function and compares the returned major and minor versions with @ref
QDMI_CLIENT_ABI_VERSION; patch differences are compatible. It then resolves the
complete Client Interface before allocating a session. Replacing a driver
without rebuilding is possible when the replacement satisfies this ABI.

Device libraries export the Device Interface with their own symbol prefix. Their
implementation version is independent of the QDMI interface version; @ref
QDMI_DEVICE_PROPERTY_LIBRARYVERSION reports the QDMI version they implement. Use
headers, drivers, and device libraries for compatible interface versions. See
[CMake integration](installation.md), the @ref driver "driver example", and the
[upgrade guide](../UPGRADING.md).

### What MQT Core Adds

[MQT Core](https://mqt.readthedocs.io/projects/core/en/latest/qdmi/index.html)
implements a replaceable QDMI driver and supplies owning C++ and Python
wrappers, device implementations, compiler integration, and SDK adapters. Its
builtin driver discovers versioned JSON device manifests, can enumerate
configured IDs without loading devices, and can open one configured device at a
time. These discovery helpers and manifest conventions are MQT Core facilities;
another QDMI driver can use a different catalogue mechanism.

Use MQT Core's
[driver and configuration guides](https://mqt.readthedocs.io/projects/core/en/latest/qdmi/configuration.html)
for those facilities, its
[compiler guide](https://mqt.readthedocs.io/projects/core/en/latest/compilation/index.html)
for preparing target-compatible programs, and its
[Slurm guide](https://mqt.readthedocs.io/projects/core/en/latest/qdmi/slurm.html)
for scheduler integration. Device projects own their provider-specific setup:
[Amazon Braket](https://amazon-braket-qdmi-device.readthedocs.io/en/latest/),
[IQM](https://iqm-finland.github.io/QDMI-on-IQM/), and
[IBM](https://ibm-qdmi-device.readthedocs.io/en/latest/) document their
libraries, credentials, accepted programs, and deployment modes.

## Background and Version Context {#rationale-background}

See [Ecosystem and Community](ecosystem.md) for international adoption,
implementation listings, historical credit, and the complete publication list,
including the original QDMI paper and the openQSE reference-architecture survey.

The
[Munich Quantum Software Stack paper](https://doi.org/10.1145/3773656.3773669)
places QDMI at the device-management boundary beneath adapters, compilers, and
resource managers. The
[Amazon Braket case study](https://arxiv.org/abs/2603.05138) describes mapping a
cloud service's authentication, tasks, and results onto QDMI. The
[IQM case study](https://arxiv.org/abs/2604.19869) covers architecture and
calibration queries, execution, SDK integration, and HPC deployment on IQM
systems.

These papers explain the architecture and particular implementation snapshots.
For example, the Braket paper uses QDMI 1.2, while the current interface uses
program-list setters and a versioned Client Interface ABI. Use the headers and
documentation for the version you deploy, and consult the
[upgrade guide](../UPGRADING.md) for migrations. Capabilities and deployment
options are defined by the selected driver and device implementation, rather
than by an architectural diagram.

## Why does QDMI use opaque pointers? {#rationale-opaque-pointers}

Throughout QDMI, we frequently use opaque pointers to represent objects such as
sessions, jobs, devices, sites, and operations. Opaque pointers are pointers to
a data structure that is not defined in the header file. The actual
implementation is only known to the entity that defines the object. Opaque
pointers have several advantages: They allow changing the internal
representation of the object without breaking the client code, which makes the
interface more stable and easier to maintain. Opaque pointers also prevent the
client from accessing or modifying the internal representation of the object,
which can help to prevent bugs and security vulnerabilities. Finally, opaque
pointers are strongly typed, which can help to catch type errors at compile
time. Opaque pointers also come with some disadvantages: They require an
additional level of indirection in the implementation, which can lead to a
performance overhead compared to direct implementations of the types.

Publicly defining the internal representation of the objects would be an
alternative to using opaque pointers. However, this would expose the internal
details of the objects to the client and would limit the flexibility of the
implementation as well as the ability to change the internal representation of
the objects while maintaining binary compatibility.

Yet another alternative would be to use integer IDs to represent the objects,
for example, by using Universally Unique Identifiers (UUIDs). However, this
would require additional bookkeeping in implementations to map the IDs to the
actual objects. It would also make the interface less type-safe. In contrast,
opaque pointers effectively serve as type-safe IDs that are checked statically
by the compiler.

## Why does QDMI not define individual functions for each property? {#rationale-properties}

The design of the function signatures and semantics in QDMI is heavily inspired
by the design of the [OpenCL](https://www.khronos.org/opencl/) API for parallel
programming of heterogeneous systems. As such, QDMI heavily relies on
enumerations to define properties of devices, sessions, jobs, sites, and
operations. For each type of property, a corresponding enumeration is defined.
The value of a property is retrieved by calling a function with the property
enumeration as an argument.

A generic function signature for querying the value of a property type `<prop>`
from an object `<obj>` looks like this:

```C
int QDMI_<obj>_query_<prop>(
  QDMI_<obj> handle,
  QDMI_<prop> prop,
  size_t size,
  void *value,
  size_t *size_ret
);
```

Here, `QDMI_<obj>` is the opaque pointer type of the object (for example,
`QDMI_Device`, `QDMI_Session`, `QDMI_Job`, `QDMI_Site`, or `QDMI_Operation`).
`QDMI_<prop>` is the enumeration type of the property (for example,
`QDMI_Session_Property`, `QDMI_Device_Property`, `QDMI_Site_Property`, or
`QDMI_Operation_Property`). The function retrieves the value of the property
`<prop>` from the object `<obj>` identified by the handle `handle`. The value of
the property is stored in the memory region pointed to by `value`, which has a
size of `size` bytes. The actual size of the value written to the memory region
is stored in the variable pointed to by `size_ret`. The function returns an
error code indicating success or failure. The `value` is purposely passed as a
`void *` to allow the function to return values of different types. The type of
the value is determined by the property enumeration.

This design has several advantages:

- It allows keeping the interface compact. Instead of defining a separate
  function for each property, the value of a property is retrieved by calling a
  single function with the property enumeration as an argument. This reduces the
  number of functions in the interface and makes the interface easier to use.
- It makes it easier to maintain consistent semantics across different
  properties, making the interface more predictable and easier to learn.
- It allows adding new properties without breaking the interface. If a new
  property is added, the corresponding enumeration can be added to the interface
  without changing the existing functions. This makes the interface more
  extensible and easier to maintain.

Similar design principles are applied to the functions for setting parameters
and retrieving results of jobs.

## Why do device implementations use a prefix? {#rationale-prefix}

Each device implementation prefixes its Device Interface symbols and opaque
types. Distinct implementations linked statically into the same program need
noncolliding prefixes. The prefix also identifies the implementation when
debugging and allows hardware vendors to brand their device implementations.
Different configured instances of one implementation can share a prefix; their
stable device IDs distinguish the logical resources. Prefixes should be short
and descriptive of the implementation.

## Why restrict shared-library exports? {#rationale-exports}

A device or driver may bundle dependencies that are also present in its host
process. On platforms with symbol interposition, exported dependency symbols can
bind to another library's implementation. Compiler visibility settings on the
device or driver do not hide definitions in already compiled dependency
archives. Shared libraries should therefore restrict exports to their intended
public interfaces, including any deliberate vendor extensions. QDMI provides
@ref installation-exports "optional CMake support" for this. The restriction
belongs at the final shared-library link and does not replace dependency
management for statically composed implementations.

## Why do devices have sessions? {#device-session}

Per default, devices do not know which client is calling one of their functions.
This is intentional to keep the devices as simple as possible and to avoid the
need for complex authentication and authorization mechanisms on the device
level. However, the device implementations may want to expose different
capabilities or access modes depending on the client that is calling them. To
this end, the \ref QDMI_Device_Session was introduced. The driver creates a
session with the device for the client. The driver can set parameters on the
session to identify the client to the device and unlock specific features.

This allows device implementers to switch API tokens or endpoints on the fly,
without having to recompile and redistribute the respective implementation. It
also allows them to limit the access of certain clients to specific features of
the device. For example, a device implementation could provide a read-only mode
for clients that are not authorized to run jobs on the device.

## Why do sessions need to be initialized after allocation? {#rationale-session-init}

QDMI defines two kinds of sessions, namely \ref QDMI_Session and \ref
QDMI_Device_Session. The typical workflow for working with both kinds of
sessions involves allocating a session, optionally setting parameters on the
session, and then initializing the session. This three-step process was chosen
to allow for more flexibility in authentication and authorization mechanisms,
similar to how we designed the query interface to be compact and extensible.

## Why are there separate kinds of jobs for devices and clients? {#rationale-job-structs}

QDMI defines two kinds of jobs, namely \ref QDMI_Device_Job and \ref QDMI_Job. A
QDMI client will only ever get access to a \ref QDMI_Job that it can request via
@ref QDMI_device_create_job from a @ref QDMI_Device handle that it received from
the driver. Internally, the driver will use the \ref device_job_interface to
create a corresponding \ref QDMI_Device_Job. In this sense, the \ref QDMI_Job is
a request to the driver to execute a job on the device. The driver will then
translate this request into a \ref QDMI_Device_Job and execute it on the device.
As part of that translation, the driver may decide to modify the job or add
additional information to it. This is also why two kinds of job parameters
exist, namely \ref QDMI_Job_Parameter and \ref QDMI_Device_Job_Parameter. A
device may allow the driver to configure more parameters than the client is
allowed to set. The client will only ever see the \ref QDMI_Job and not the \ref
QDMI_Device_Job. Thus, the client cannot interfere with the execution of the job
on the device. This would not be possible if there were only one kind of job.

## Why are some enum definitions placed in the `constants.h` header and some are not? {#rationale-enum-definitions}

Generally, enum definitions are placed in the header file where they are used.
For example, if an enum is only used in the \ref client_interface, such as \ref
QDMI_Job_Parameter, it is defined in the `client.h` header. Enumerations that
are used across both the \ref client_interface and the \ref device_interface,
such as \ref QDMI_Device_Property, are defined in the `constants.h` header,
which both interfaces include. There is one exception to the above rule:
Enumerations that are only used in the \ref device_interface, such as \ref
QDMI_Device_Job_Parameter, are also centrally defined in the `constants.h`
header. If this were not the case, each device implementation would have to
define the same enumeration, each with a different prefix. It would also not be
possible for the driver to know about all the different values of the
enumeration. Additionally, when linking all devices statically into the driver,
the name-shifted header for each device must be included. If each device would
define the enumerations anew, this would lead to a compile-time error.

## Why are there different kinds of sites? {#rationale-site-types}

Originally, QDMI only defined a single kind of site, which was used to represent
a location that can potentially hold a qubit. Already from the start, we
intentionally avoided using the term `Qubit` to refer to their location, as on
some devices, e.g., neutral atom-based ones, a site must not necessarily hold an
atom representing the qubit. Hence, a site is more general than a qubit and can
represent any kind of location that can hold a qubit, such as a superconducting
qubit, a neutral atom, or a trapped ion. However, when extending the QDMI
support for neutral atom-based devices, we realized that this was not general
enough. These devices offer _global_ operations that execute on multiple atoms
within a given zone simultaneously. In particular, these global operations
cannot be executed on individual atoms but on all atoms within a zone at once.
Consequently, we had to equip QDMI such that it is capable of reporting
operation properties with respect to zones and not only individual qubit
positions. To this end, we introduced the concept of _zone sites_ that represent
a spatial zone. These have the same type as regular sites, i.e., @ref QDMI_Site,
but are used to represent a zone instead of a single qubit position. Hence, they
can be used in the function @ref QDMI_device_query_operation_property as
parameter to query data like fidelity or duration of global operations specific
to a zone. The function @ref QDMI_device_query_device_property will return a
list of all, i.e., regular and zone sites, for the property @ref
QDMI_DEVICE_PROPERTY_SITES. Overall, these modifications allow QDMI to represent
the capabilities of neutral atom-based devices while keeping the interface
consistent with the existing site concept.
