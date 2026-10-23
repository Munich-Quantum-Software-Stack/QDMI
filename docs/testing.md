# Device Query Conformance Tests

<!-- This file is a static page and included in docs/CMakeLists.txt. -->

The opt-in query suite checks the Device Interface property contracts in
`include/qdmi/device.h` and `include/qdmi/constants.h`. It compiles against a
provider's prefixed symbols and runs through GoogleTest. It creates no jobs and
makes no provider network calls. Supply a controlled local fixture; a fixture
that connects to hardware is outside this suite's offline guarantees.

## Integration

Use CMake 3.24 or newer and a C++20 compiler. Supply GoogleTest with an existing
`GTest::gtest_main` target and enable CTest in the consuming project. QDMI
neither fetches GoogleTest nor enables tests when this helper is merely
included. `add_subdirectory` exposes the helper from a source checkout;
`find_package(qdmi CONFIG REQUIRED)` exposes the same helper and test source
from an installation. QDMI's own tests, examples, and templates may all be
disabled.

```cmake
find_package(qdmi CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)
enable_testing()
# my-device already supplies MY-prefixed headers and device symbols.
add_qdmi_device_query_tests(
  my-query-tests
  PREFIX MY
  TARGET my-device
  FIXTURE_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/query_fixture.hpp")
```

`PREFIX` is the exact device symbol prefix. `TARGET` supplies the device library
and its public include directories. `FIXTURE_HEADER` is an existing header;
relative paths are resolved against the caller's source directory. Paths may
contain spaces. Invoke the helper with distinct executable names to test several
devices. On Windows, the helper copies a shared device target's DLL beside the
test executable. Other runtime dependencies remain the consumer's
responsibility. Tests have distinct executable-name prefixes and the CTest label
`qdmi-query`.

The header defines `QDMIQueryFixture`, derived from `::testing::Test`, with a
protected initialized session named `session` of the prefixed session type.
Initialize the device once in `SetUpTestSuite`, allocate and initialize a
session in `SetUp`, free it in `TearDown`, and finalize the device in
`TearDownTestSuite`. Check setup errors with GoogleTest assertions and make
cleanup safe after partial setup. GoogleTest supplies `main`; do not define
another one. See `test/query/example_fixture.hpp` for a complete offline
example, and `test/query/consumer` for a separate project that opts in twice.

```console
cmake --build build --target my-query-tests
ctest --test-dir build -L qdmi-query --output-on-failure
```

Dependency configuration may download packages. Test execution needs no
credentials or provider access. The example's token is only a local placeholder.
Ordinary consumers that do not call the helper acquire no query executable or
GoogleTest dependency.

## Checked Contracts

For each supported property the suite checks support-only and size-only queries,
exact and oversized typed buffers, optional `size_ret`, and one-byte-short
buffers. It validates scalar types, string termination, array element sizes,
defined enum values, and advertised nonnull handles. Invalid enums, removed
device property IDs 8 and 9, and isolated null arguments are checked against the
specified errors. Error returns may overwrite the supplied buffer.

Device coverage includes name, version, library version, ID, status, qubit
count, queue length, site and operation lists, coupling map, child devices,
supported program formats, length and duration units and scale factors, and
minimum atom distance. Site coverage includes name, mandatory index, T1, T2,
coordinates, zone status, spatial extents, module index, and submodule index.
Operation coverage includes name, qubit and parameter counts, duration,
fidelity, interaction and blocking radii, idling fidelity, zone status, site
tuples, and mean shuttling speed. Relationships include distinct site indices
and supported coordinate triples, coupling endpoints in advertised sites, and
conditional unit availability. The installed headers remain the normative
reference.

The table uses suffixes from the source enums \ref QDMI_Device_Property, \ref
QDMI_Site_Property, and \ref QDMI_Operation_Property in
[constants.h](../include/qdmi/constants.h). The query error and buffer contracts
are defined in [device.h](../include/qdmi/device.h). All payloads receive the
buffer checks above; the final column records additional support rules.

| Property suffix (source enum prefix omitted)   | Payload                          | Required support / assertions                                                                                                                       |
| ---------------------------------------------- | -------------------------------- | --------------------------------------------------------------------------------------------------------------------------------------------------- |
| DEVICE: NAME, VERSION, LIBRARYVERSION, ID      | char string                      | Optional directly; no name, version format, or client ID stability assumptions                                                                      |
| DEVICE: STATUS                                 | QDMI_Device_Status               | Optional; scalar shape and one defined status, excluding MAX                                                                                        |
| DEVICE: QUBITSNUM, QUEUELENGTH                 | size_t                           | Optional; no equality with site count or queue stability                                                                                            |
| DEVICE: SITES                                  | prefixed QDMI_Site array         | Optional; query only advertised nonnull handles                                                                                                     |
| DEVICE: OPERATIONS                             | prefixed QDMI_Operation array    | Optional; query only advertised nonnull handles                                                                                                     |
| DEVICE: COUPLINGMAP                            | prefixed QDMI_Site array         | Optional; even length; endpoints belong to SITES when available                                                                                     |
| DEVICE: LENGTHUNIT, DURATIONUNIT               | char string                      | Required when related length/duration values are supported; string shape; unit notation remains unspecified                                         |
| DEVICE: LENGTHSCALEFACTOR, DURATIONSCALEFACTOR | double                           | Optional; unsupported means default 1.0; no invented numeric bounds                                                                                 |
| DEVICE: MINATOMDISTANCE                        | uint64_t                         | Optional; requires LENGTHUNIT                                                                                                                       |
| DEVICE: SUPPORTEDPROGRAMFORMATS                | QDMI_Program_Format array        | Optional; empty allowed; defined regular/custom formats only; no job calls                                                                          |
| DEVICE: CHILDDEVICES                           | prefixed QDMI_Child_Device array | Optional; no child configuration or job calls                                                                                                       |
| DEVICE: numeric 8, 9                           | none                             | Must return NOTSUPPORTED with a valid session                                                                                                       |
| SITE: INDEX                                    | size_t                           | Mandatory per advertised site; unique, sparse and unsorted allowed                                                                                  |
| SITE: T1, T2                                   | uint64_t                         | Optional per site; requires DURATIONUNIT                                                                                                            |
| SITE: NAME                                     | char string                      | Optional per site                                                                                                                                   |
| SITE: XCOORDINATE, YCOORDINATE, ZCOORDINATE    | int64_t                          | Optional per site; requires LENGTHUNIT; complete coordinate triples unique                                                                          |
| SITE: ISZONE                                   | bool                             | Optional; unsupported means false                                                                                                                   |
| SITE: XEXTENT, YEXTENT, ZEXTENT                | uint64_t                         | Optional on zones; NOTSUPPORTED on nonzones; requires LENGTHUNIT                                                                                    |
| SITE: MODULEINDEX, SUBMODULEINDEX              | uint64_t                         | Optional; grouping IDs need not be unique per site                                                                                                  |
| OPERATION: NAME                                | char string                      | Optional context-free                                                                                                                               |
| OPERATION: QUBITSNUM, PARAMETERSNUM            | size_t                           | Optional context-free                                                                                                                               |
| OPERATION: DURATION                            | uint64_t                         | Optional context-free; requires DURATIONUNIT                                                                                                        |
| OPERATION: FIDELITY, IDLINGFIDELITY            | double                           | Optional context-free; no inferred [0,1] bound                                                                                                      |
| OPERATION: INTERACTIONRADIUS, BLOCKINGRADIUS   | uint64_t                         | Optional context-free; requires LENGTHUNIT                                                                                                          |
| OPERATION: ISZONED                             | bool                             | Optional context-free; unsupported means false                                                                                                      |
| OPERATION: SITES                               | prefixed QDMI_Site array         | Optional context-free; membership; local tuple width when arity available; zone membership for global operations when site classification available |
| OPERATION: MEANSHUTTLINGSPEED                  | uint64_t                         | Optional context-free; requires both units                                                                                                          |
| DEVICE/SITE/OPERATION: MAX                     | none                             | Invalid enum; require INVALIDARGUMENT with other arguments valid                                                                                    |
| DEVICE/SITE/OPERATION: CUSTOM1..CUSTOM5        | implementation-defined           | Deliberately unqueried: neither type nor support is generic                                                                                         |

Optional unsupported properties skip when no advertised object supports them.
Mixed support across objects is valid; mandatory site index support is checked
on every advertised site. Empty arrays and sparse indices are valid. Site counts
need not equal qubit counts, since sites may represent zones. Direct device ID
support is optional; the client-visible stable-ID rule is separate. Repeated
metadata values need not stay equal. Metadata growth during retrieval is
reported explicitly; use controlled metadata for reproducible results. Payloads
above the suite's 64 MiB allocation limit fail with a diagnostic; that is a
harness limit, not a QDMI size restriction.

## Exclusions and Specification Follow-up

The suite does not create jobs, compile programs, set session parameters, load
providers, or validate credentials. It does not probe fabricated, foreign,
freed, or uninitialized handles. Custom properties have implementation-defined
types and are excluded. Operation queries use zero site and parameter counts
with null inputs; context-dependent metadata is excluded. It adds no
undocumented numerical bounds, gate names, ordering, or topology symmetry
requirements.

Unit strings are checked for shape and conditional availability. The headers do
not define accepted symbols, spellings, or aliases, so unit notation needs a
separate specification decision. Radius and idling-fidelity applicability and
zero-arity local-operation tuple semantics also need clarification before adding
stronger assertions. The suite does not change the normative API docstrings.

QDMI CI runs positive controls and fixed negative controls for malformed sizes,
invalid properties, missing mandatory indices, omitted size outputs, and missing
string terminators at every retrieval position. It also runs two source
consumers and relocated installed consumers on Linux and macOS. The consumer
checks include fixture/install paths with spaces, missing installed assets, and
explicit opt-out. This workflow does not run on Windows; local Windows
verification is separate.
