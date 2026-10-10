# Using QDMI with CMake

QDMI provides C11 headers and CMake helpers through the `qdmi::qdmi` interface
target. Installing QDMI does not install a driver or a device implementation.
Applications need a compatible implementation to execute QDMI calls; see the
[examples](examples.md) and [templates](templates.md).

## Select an Interface Version

For a published release, select a tag from the [releases page][releases] and use
its matching documentation. Release tags have a `v` prefix; CMake package
versions do not. Keep drivers, devices, and application headers on compatible
versions.

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
git clone \
  https://github.com/Munich-Quantum-Software-Stack/QDMI.git qdmi
git -C qdmi checkout v1.4.0

cmake -S qdmi -B qdmi/build \
  -DINSTALL_QDMI=ON \
  -DBUILD_QDMI_TESTS=OFF \
  -DBUILD_QDMI_EXAMPLES=OFF \
  -DBUILD_QDMI_TEMPLATES=OFF \
  -DBUILD_QDMI_DOCS=OFF

cmake --install qdmi/build --prefix "$PWD/qdmi-install"
```

Choose an installation prefix that you can write to. These commands use POSIX
shell line continuations; adapt them to your shell on Windows. A source archive
of the selected revision can be used in place of the Git checkout.

The installation contains headers and CMake package files, including the
symbol-prefix helpers used by device implementations. QDMI currently does not
publish prebuilt SDK archives. The package version check is
architecture-independent; device and driver binaries still need to match the
platform and architecture of the application.

## Use an Installed Package

In your project's `CMakeLists.txt`, after declaring your target:

```cmake
find_package(qdmi 1.4.0 CONFIG REQUIRED)
target_link_libraries(my_target PRIVATE qdmi::qdmi)
```

Configure your project with the absolute installation prefix from the previous
step:

```sh
cmake -S . -B build -DCMAKE_PREFIX_PATH=/absolute/path/to/qdmi-install
```

The imported target supplies the include directory and C11 requirement. Use
`PUBLIC` instead of `PRIVATE` when your library exposes QDMI types in its public
headers. Linking this interface target does not supply implementations of QDMI
functions.

## Embed the Source with FetchContent

Alternatively, let CMake obtain QDMI as part of your project. C++ projects must
also enable C in the parent project, for example with
`project(my_project LANGUAGES C CXX)`, before including QDMI. This makes QDMI's
C11 usage requirement available in the consumer's scope.

```cmake
include(FetchContent)
FetchContent_Declare(
  qdmi
  GIT_REPOSITORY https://github.com/Munich-Quantum-Software-Stack/QDMI.git
  GIT_TAG v1.4.0)
FetchContent_MakeAvailable(qdmi)

target_link_libraries(my_target PRIVATE qdmi::qdmi)
```

QDMI defaults to disabling installation, tests, examples, and templates when
included as a subproject. Documentation is also disabled by default. This path
needs network access for the initial checkout, unless you supply a local source
directory with `FETCHCONTENT_SOURCE_DIR_QDMI`.

## Control Shared-Library Exports {#installation-exports}

Use `configure_qdmi_exports` to restrict a shared or module library to its QDMI
interface and explicitly listed additional C symbols:

```cmake
configure_qdmi_exports(TARGET my_device INTERFACE device PREFIX MY)
configure_qdmi_exports(TARGET my_driver INTERFACE client
                      EXTRA_SYMBOLS MY_driver_extension)
```

Call the helper once after creating the target. Device interfaces require
`PREFIX`; client interfaces use unprefixed names. `EXTRA_SYMBOLS` accepts exact
C identifiers for additional public functions. The helper reads the QDMI headers
in use and is available through `find_package` and source dependencies.

The helper uses a linker version script on ELF systems and an exported-symbols
list on Apple systems. These restrict the target's exports, including
definitions from bundled static dependencies. With Ninja and Makefile
generators, changes to the interface header regenerate the list and relink the
library.

Apply export control to the final shared library when using static archives. On
Windows, use explicit `dllexport` declarations; the helper leaves exports
unchanged. Other unsupported platforms produce a warning.

Keep public definitions visible through the appropriate export annotations.
Executables and tests linking against the library must use its exported
interface. Verify the final exports, for example with `nm -D` on ELF systems or
`dlsym`: LLD rejects listed symbols without definitions, whereas GNU ld and gold
silently ignore them.

The device example and template use this helper, as does the example driver when
built as a shared library. Other build systems can apply equivalent linker
settings.

[releases]: https://github.com/Munich-Quantum-Software-Stack/QDMI/releases
