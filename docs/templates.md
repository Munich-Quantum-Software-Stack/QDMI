# Template

<!-- IMPORTANT: Keep the line above as the first line. -->

<!-- This file is a static page and included in the ./CMakeLists.txt file. -->

Together with QDMI, we provide a template meant to kick-start the implementation
of a new device. The following sections describe how to set up and use the
template.

\tableofcontents

## Creating a new Project {#template-create}

The code for the template is contained in the `templates/device/` directory of
the QDMI repository. To start a new project based on the template, configure the
@ref getting-started-build "QDMI checkout" once to define the prefix and output
path, then explicitly build the `qdmi-template` target that writes the files.

\note Initial configuration fetches build dependencies and requires an internet
connection.

```sh
cmake -DQDMI_GENERATE_TEMPLATE=ON \
      -DTEMPLATE_PREFIX="prefix" \
      -DTEMPLATE_PATH="path/to/dir" \
      -S . -B build

# actually write the template files
cmake --build build --target qdmi-template
```

If the option `TEMPLATE_PATH` is not given it will be placed in
`prefix-qdmi-device` relative to the parent directory where QDMI was cloned in.
The directory name uses the lowercase form of `TEMPLATE_PREFIX`.

To regenerate into an existing directory, run the same target again. This
overwrites files supplied by the template; preserve your implementation changes
before doing so:

```sh
cmake --build build --target qdmi-template
```

After this step you can directly start implementing your device in C++. Example
implementations are provided in the `examples/` directory. See
[Examples](examples.md) for more information.

## Configuring the Template {#template-configure}

Generated projects request the QDMI release selected by `QDMI_VERSION` in
`cmake/ExternalDependencies.cmake`; `QDMI_REV` defaults to the matching release
tag. See the [installation guide](installation.md) for version compatibility. To
use a different tag or commit, override `QDMI_REV` when configuring and keep
`QDMI_VERSION` consistent with the selected interface.

The generated project assigns the default stable ID `prefix.default` to its
device implementation. Set the project-specific `prefix_QDMI_DEVICE_ID` CMake
cache variable to change it. The device reports that default through
`QDMI_DEVICE_PROPERTY_ID`; a driver can override it for each configured device.

The device target calls `configure_qdmi_device_target` to export its default
stable ID and symbol prefix as `QDMI_DEVICE_ID` and `QDMI_DEVICE_PREFIX`.
Consumers use this metadata to discover the device without loading its library
or linking the device to the consuming library.

The device target also calls `configure_qdmi_exports` to restrict shared-library
exports to the prefixed device interface on ELF and Apple systems. See @ref
installation-exports "export control" for additional public symbols and platform
behavior.

When you want to change the prefix after the creation of the template, you need
to change the prefix in a couple of places. We want to give you some hints where
you have to change it, but depending on your personal project setup, they might
be different or there might be more than the ones listed. All paths are given
relative to the root of the template project directory.

- `CMakeLists.txt`,
- `src/CMakeLists.txt`: the target `prefix-qdmi-device` and `prefix_device.cpp`
- Rename `src/prefix_device.cpp` accordingly
- `src/prefix_device.cpp`: adapt the includes and the prefix of each function
- Rename `test/test_prefix_device.cpp` accordingly
- `test/test_prefix_device.cpp`: adapt the includes and the prefix of each
  function
- `pyproject.toml`: adapt the package name and several paths
- `python/prefix`: adapt the package namespace in the directory structure
- `python/prefix/qdmi/*`: adapt the prefix throughout the package
- `test/python/*`: adapt the prefix throughout the tests
- `docs/*`: the package name and several paths

## Working with the Template {#template-working}

The top-level `CMakeLists.txt` contains settings for the entire project. Some
additional CMake code that imports required dependencies is outsourced into
`cmake/`.

The most important directory for your implementation is `src/` and the `.cpp`
file located in that directory. Here you find stubs for all functions that have
to be implemented by a device. For every function the
`return QDMI_ERROR_NOTIMPLEMENTED;` should be replaced by a proper
implementation of the function. In particular, there should not be any
computation path at the end that returns \ref QDMI_ERROR_NOTIMPLEMENTED.
Instead, some other error code from \ref QDMI_STATUS should be returned in case
of an erroneous state.

QDMI entry points have C linkage, so exceptions must not escape them. Catch
exceptions directly at entry points that call throwing C++ code, map
`std::bad_alloc` to \ref QDMI_ERROR_OUTOFMEM, and map unexpected exceptions to
\ref QDMI_ERROR_FATAL. Catch provider-specific exceptions first when a more
precise QDMI status applies. A function-try-block keeps this boundary local and
does not require wrapping non-throwing entry points.

The implementation in the `src/` directory is complemented with a testing
framework in `test/`. The `.cpp` source file already contains some examples for
tests. They are meant to serve as an inspiration, and more tests should be
implemented to cover everything in your device implementation.

The `python` directory in combination with the `pyproject.toml` file contains
some basic packaging setup for distributing the device implementation as a
Python package.

The `docs/` directory contains the documentation for the template. This is
intended to be used as a starting point for your own documentation.

Some more files are present in the root directory of the template, such as
`LICENSE`, `README.md`, and `.gitignore`.

## Building the Template and Running the Tests {#template-building}

All following commands are meant to be executed from the root directory of the
template. After configuring your project (see
[Configuring the Template](#template-configure)), you can build your project
with the following command:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

If you only want to build a specific target, you can append, for example,
`--target prefix-qdmi-device-test` to the command above, which will build the
tests. If you only want to build the device implementation, you can use
`--target prefix-qdmi-device`.

To run the tests, perform the following command:

```sh
ctest --test-dir build
```

The generated implementation contains stubs that return
`QDMI_ERROR_NOTIMPLEMENTED`. Its functional tests fail until you implement the
required entry points; use those failures to track the remaining work.

For more details on the development process, also check out the
[Contributing Guide](contributing.md).
