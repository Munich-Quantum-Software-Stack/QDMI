<p align="center">
  <a href="https://www.meetiqm.com">
   <picture>
     <source media="(prefers-color-scheme: dark)" srcset="docs/_static/my_logo_dark.svg">
     <img src="docs/_static/my_logo.svg" alt="Logo" width="40%">
   </picture>
  </a>
</p>

# MY QDMI Device

<!-- [DOXYGEN MAIN] -->

A C++20 library that implements the QDMI Device interface.

<!-- [DOXYGEN MAIN] -->

The exported CMake target publishes the package ID configured through
`MY_QDMI_DEVICE_ID` and the QDMI symbol prefix through
`configure_qdmi_device_target`. Consumers such as MQT Core can use this metadata
to package and register the device without project-specific loader code. The
package ID is not the runtime `QDMI_DEVICE_PROPERTY_ID`. This metadata does not
add MQT Core as a dependency.

Do not allow C++ exceptions to escape the device's C entry points. Contain them
directly at entry points that call throwing code, for example with a
function-try-block. Map allocation failures to `QDMI_ERROR_OUTOFMEM`, unexpected
exceptions to `QDMI_ERROR_FATAL`, and provider-specific failures to the most
precise available status. Non-throwing entry points need no wrapper.

## Documentation

The full documentation, including project guides, a contributing guide, and the
C++ API reference, is available in the `docs/` directory and can be built using
[Doxygen](https://doxygen.nl/).

You can build the documentation by running:

```console
cmake -S . -B build -DBUILD_MY_QDMI_DOCS=ON
cmake --build build --target my_qdmi_device_docs
```

The documentation will be generated in the `build/docs/` directory. You can view
the HTML documentation by opening the `index.html` file in a web browser.
