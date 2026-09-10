# Using QDMI with CMake

QDMI provides C11 headers and CMake helpers through the `qdmi::qdmi` interface
target. Installing QDMI does not install a driver or a device implementation.
Applications need a compatible implementation to execute QDMI calls; see the
[examples](examples.md) and [templates](templates.md).

## Select a Release

Use a tag from the [releases page][releases]. Release tags include the `v`
prefix, for example `v1.3.3`; CMake package versions omit it, for example
`1.3.3`. The examples below pin that release rather than a development branch.
For reproducible source dependencies, a full commit hash can also be used with
`FetchContent`.

QDMI minor releases may contain breaking changes. The installed CMake package
accepts compatible patch releases within the requested minor version; use
`EXACT` if you need one particular version. Consult the
[upgrade guide](../UPGRADING.md) before changing minor versions.

## Install the Interface

The source configuration requires CMake 3.24 or newer and both C and C++
compilers. The interface itself is header-only: no QDMI binary needs to be
compiled for this installation. Disable tests, examples, templates, and
documentation to avoid building optional targets and downloading their
dependencies.

```sh
git clone --depth 1 --branch v1.3.3 \
  https://github.com/Munich-Quantum-Software-Stack/QDMI.git qdmi

cmake -S qdmi -B qdmi/build \
  -DINSTALL_QDMI=ON \
  -DBUILD_QDMI_TESTS=OFF \
  -DBUILD_QDMI_EXAMPLES=OFF \
  -DBUILD_QDMI_TEMPLATES=OFF \
  -DBUILD_QDMI_DOCS=OFF

cmake --install qdmi/build --prefix /opt/qdmi
```

Choose an installation prefix that you can write to. These commands use POSIX
shell line continuations; adapt them to your shell on Windows. A release source
archive can be used in place of the Git checkout.

The installation contains headers and CMake package files, including the
symbol-prefix helpers used by device implementations. QDMI currently does not
publish prebuilt SDK archives; the source installation above provides the
interface independently of the target architecture.

## Use an Installed Package

In your project's `CMakeLists.txt`, after declaring your target:

```cmake
find_package(qdmi 1.3.3 CONFIG REQUIRED)
target_link_libraries(my_target PRIVATE qdmi::qdmi)
```

Configure your project with the installation prefix:

```sh
cmake -S . -B build -DCMAKE_PREFIX_PATH=/opt/qdmi
```

The imported target supplies the include directory and C11 requirement. Use
`PUBLIC` instead of `PRIVATE` when your library exposes QDMI types in its public
headers. Linking this interface target does not supply implementations of QDMI
functions.

## Embed the Source with FetchContent

Alternatively, let CMake obtain QDMI as part of your project:

```cmake
include(FetchContent)
FetchContent_Declare(
  qdmi
  GIT_REPOSITORY https://github.com/Munich-Quantum-Software-Stack/QDMI.git
  GIT_TAG v1.3.3)
FetchContent_MakeAvailable(qdmi)

target_link_libraries(my_target PRIVATE qdmi::qdmi)
```

QDMI defaults to disabling installation, tests, examples, and templates when
included as a subproject. Documentation is also disabled by default. This path
needs network access for the initial checkout, unless you supply a local source
directory with `FETCHCONTENT_SOURCE_DIR_QDMI`.

[releases]: https://github.com/Munich-Quantum-Software-Stack/QDMI/releases
