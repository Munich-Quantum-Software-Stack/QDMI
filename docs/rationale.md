# Rationale

<!-- IMPORTANT: Keep the line above as the first line. -->
<!----------------------------------------------------------------------------
Copyright 2024 Munich Quantum Software Stack Project

Licensed under the Apache License, Version 2.0 with LLVM Exceptions (the
"License"); you may not use this file except in compliance with the License.
You may obtain a copy of the License at

https://github.com/Munich-Quantum-Software-Stack/QDMI/blob/develop/LICENSE

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
License for the specific language governing permissions and limitations under
the License.

SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
-------------------------------------------------------------------------- -->

<!-- This file is a static page and included in the ./CMakeLists.txt file. -->

During the development of QDMI, we had to make several design decisions, which we want to outline in
the following. This page is supposed to serve as a reference to why things are as they are in QDMI.
Simultaneously, it should help to get a better understanding of the principles of QDMI. To this end,
this page is useful for everyone working with QDMI.

\tableofcontents

## The structure of QDMI {#rationale-structure}

<img class="qdmi-schematic" alt="QDMI Components and Interfaces" src="qdmi_schematic.svg"/>

QDMI interfaces three different entities, namely:

- "devices",
- "clients", and
- a "driver".

These entities are connected via two interfaces that allow the communication between the entities.
We call those interfaces the @ref device_interface and the @ref client_interface. In the following,
we will explain the responsibilities of the entities and the interfaces.

_Devices_ (also commonly called _"backends"_) represent actual quantum devices or simulators. Each
device provides an implementation of the structs and functions defined by the @ref device_interface.
In particular, this includes implementations of types for \ref QDMI_Site "sites", \ref
QDMI_Operation "operations", \ref QDMI_Device_Job "device jobs", and \ref QDMI_Device_Session
"device sessions". Handles to these types are passed across the interface as opaque pointers, which
means that only the device knows the actual implementation of those types.

_Clients_ are the users of the QDMI library that want to interact with the devices. They use the
functions defined by the @ref client_interface "client interface" to interact with the devices. The
clients do not have direct access to the devices but must go through what we refer to as a "driver".
This is to ensure that the devices are used in a controlled manner and that the clients do not
interfere with each other. The clients can create \ref QDMI_Session "sessions" with the driver to
gain access to the devices.

The _driver_ is the component that manages all available devices and provides access to them for the
clients. It implements the structs and functions defined by the @ref client_interface. In
particular, this includes implementations of types for \ref QDMI_Session "sessions" and \ref
QDMI_Job "jobs". Handles to these types are passed across the interface to clients as opaque
pointers, which means that only the driver knows the actual implementation of those types. It is up
to the driver how it exposes the @ref device_interface implementation provided by the devices as
part of the @ref client_interface. For example, the driver could load the devices as dynamic
libraries and translate the calls from the client to the devices or statically link the devices into
the driver. An example implementation using dynamic libraries is provided in the `examples`
directory of the QDMI repository.

As depicted in the schematic above, device and client interfaces each have three parts, namely:

- the "session interface",
- the "query interface", and
- the "job interface".

The session interface is used to establish a connection between two entities. The client creates a
session with the driver, and the driver creates sessions with the devices it manages. The main
purpose of the session interface is to handle the authentication and authorization of the clients at
the driver and the driver at the devices.

The query interface is used to retrieve information from the devices. The client can query the
devices for information about the device itself, its sites, or its operations. Most importantly,
this allows clients to discover the capabilities as well as constraints of the devices and to make
informed decisions about how to use them. The information flow in the query interface is always from
the device to the client via the driver. The driver may cache or modify any information returned by
the device before passing it to the client.

The job interface is used to control the execution of jobs on the devices. Most jobs will be quantum
circuits that the client wants to execute on the device. However, the job interface is not limited
to quantum circuits and can be used to control any kind of computation on the device, such as
calibrations. The client creates a job with the driver, and the driver delegates the job to the
device. The device executes the job and reports the results back to the client via the driver. The
information flow in the job interface is bidirectional. The client can control the job execution and
retrieve the results of the job. The driver may cache or modify any information returned by the
device before passing it to the client.

## Why does QDMI use opaque pointers? {#rationale-opaque-pointers}

Throughout QDMI, we frequently use opaque pointers to represent objects such as sessions, jobs,
devices, sites, and operations. Opaque pointers are pointers to a data structure that is not defined
in the header file. The actual implementation is only known to the entity that defines the object.
Opaque pointers have several advantages: They allow changing the internal representation of the
object without breaking the client code, which makes the interface more stable and easier to
maintain. Opaque pointers also prevent the client from accessing or modifying the internal
representation of the object, which can help to prevent bugs and security vulnerabilities. Finally,
opaque pointers are strongly typed, which can help to catch type errors at compile time. Opaque
pointers also come with some disadvantages: They require an additional level of indirection in the
implementation, which can lead to a performance overhead compared to direct implementations of the
types.

Publicly defining the internal representation of the objects would be an alternative to using opaque
pointers. However, this would expose the internal details of the objects to the client and would
limit the flexibility of the implementation as well as the ability to change the internal
representation of the objects while maintaining binary compatibility.

Yet another alternative would be to use integer IDs to represent the objects, for example, by using
Universally Unique Identifiers (UUIDs). However, this would require additional bookkeeping in
implementations to map the IDs to the actual objects. It would also make the interface less
type-safe. In contrast, opaque pointers effectively serve as type-safe IDs that are checked
statically by the compiler.

## Why does QDMI not define individual functions for each property? {#rationale-properties}

The design of the function signatures and semantics in QDMI is heavily inspired by the design of the
[OpenCL](https://www.khronos.org/opencl/) API for parallel programming of heterogeneous systems. As
such, QDMI heavily relies on enumerations to define properties of devices, sessions, jobs, sites,
and operations. For each type of property, a corresponding enumeration is defined. The value of a
property is retrieved by calling a function with the property enumeration as an argument.

A generic function signature for querying the value of a property type `<prop>` from an object
`<obj>` looks like this:

```C
int QDMI_<obj>_query_<prop>(
  QDMI_<obj> handle,
  QDMI_<prop> prop,
  size_t size,
  void *value,
  size_t *size_ret
);
```

Here, `QDMI_<obj>` is the opaque pointer type of the object (for example, `QDMI_Device`,
`QDMI_Session`, `QDMI_Job`, `QDMI_Site`, or `QDMI_Operation`). `QDMI_<prop>` is the enumeration type
of the property (for example, `QDMI_Session_Property`, `QDMI_Device_Property`, `QDMI_Site_Property`,
or `QDMI_Operation_Property`). The function retrieves the value of the property `<prop>` from the
object `<obj>` identified by the handle `handle`. The value of the property is stored in the memory
region pointed to by `value`, which has a size of `size` bytes. The actual size of the value written
to the memory region is stored in the variable pointed to by `size_ret`. The function returns an
error code indicating success or failure. The `value` is purposely passed as a `void *` to allow the
function to return values of different types. The type of the value is determined by the property
enumeration.

This design has several advantages:

- It allows keeping the interface compact. Instead of defining a separate function for each
  property, the value of a property is retrieved by calling a single function with the property
  enumeration as an argument. This reduces the number of functions in the interface and makes the
  interface easier to use.
- It makes it easier to maintain consistent semantics across different properties, making the
  interface more predictable and easier to learn.
- It allows adding new properties without breaking the interface. If a new property is added, the
  corresponding enumeration can be added to the interface without changing the existing functions.
  This makes the interface more extensible and easier to maintain.

Similar design principles are applied to the functions for setting parameters and retrieving results
of jobs.

## Why do device implementations use a prefix? {#rationale-prefix}

Each device must add a unique prefix to all symbols and types defined within its implementation.
This is necessary to facilitate static linking of multiple device implementations as part of one
driver. It also helps to identify the source of an error when debugging because the name of the
symbol will contain the prefix of the device that defined it. The prefix is also used to avoid
naming conflicts between different devices. Lastly, it allows hardware vendors to brand their device
implementations. Prefixes must be unique across all devices. They should be short and descriptive of
the device.

## Why do devices have sessions? {#device-session}

Per default, devices do not know which client is calling one of their functions. This is intentional
to keep the devices as simple as possible and to avoid the need for complex authentication and
authorization mechanisms on the device level. However, the device implementations may want to expose
different capabilities or access modes depending on the client that is calling them. To this end,
the \ref QDMI_Device_Session was introduced. The driver creates a session with the device for the
client. The driver can set parameters on the session to identify the client to the device and unlock
specific features.

This allows device implementers to switch API tokens or endpoints on the fly, without having to
recompile and redistribute the respective implementation. It also allows them to limit the access of
certain clients to specific features of the device. For example, a device implementation could
provide a read-only mode for clients that are not authorized to run jobs on the device.

## Why do sessions need to be initialized after allocation? {#rationale-session-init}

QDMI defines two kinds of sessions, namely \ref QDMI_Session and \ref QDMI_Device_Session. The
typical workflow for working with both kinds of sessions involves allocating a session, optionally
setting parameters on the session, and then initializing the session. This three-step process was
chosen to allow for more flexibility in authentication and authorization mechanisms, similar to how
we designed the query interface to be compact and extensible.

## Why are there separate kinds of jobs for devices and clients? {#rationale-job-structs}

QDMI defines two kinds of jobs, namely \ref QDMI_Device_Job and \ref QDMI_Job. A QDMI client will
only ever get access to a \ref QDMI_Job that it can request via @ref QDMI_device_create_job from a
@ref QDMI_Device handle that it received from the driver. Internally, the driver will use the \ref
device_job_interface to create a corresponding \ref QDMI_Device_Job. In this sense, the \ref
QDMI_Job is a request to the driver to execute a job on the device. The driver will then translate
this request into a \ref QDMI_Device_Job and execute it on the device. As part of that translation,
the driver may decide to modify the job or add additional information to it. This is also why two
kinds of job parameters exist, namely \ref QDMI_Job_Parameter and \ref QDMI_Device_Job_Parameter. A
device may allow the driver to configure more parameters than the client is allowed to set. The
client will only ever see the \ref QDMI_Job and not the \ref QDMI_Device_Job. Thus, the client
cannot interfere with the execution of the job on the device. This would not be possible if there
were only one kind of job.

## Why are some enum definitions placed in the `constants.h` header and some are not? {#rationale-enum-definitions}

Generally, enum definitions are placed in the header file where they are used. For example, if an
enum is only used in the \ref client_interface, such as \ref QDMI_Job_Parameter, it is defined in
the `client.h` header. Enumerations that are used across both the \ref client_interface and the \ref
device_interface, such as \ref QDMI_Device_Property, are defined in the `constants.h` header, which
both interfaces include. There is one exception to the above rule: Enumerations that are only used
in the \ref device_interface, such as \ref QDMI_Device_Job_Parameter, are also centrally defined in
the `constants.h` header. If this were not the case, each device implementation would have to define
the same enumeration, each with a different prefix. It would also not be possible for the driver to
know about all the different values of the enumeration.
