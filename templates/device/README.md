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

The exported CMake target publishes the default stable ID configured through
`MY_QDMI_DEVICE_ID` and the QDMI symbol prefix through
`configure_qdmi_device_target`. The device reports this ID through
`QDMI_DEVICE_PROPERTY_ID`. A driver can override it in its configuration, for
example to expose multiple configurations of the same device implementation.
Consumers such as MQT Core use the metadata to discover and register the device
without loading its library.

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
