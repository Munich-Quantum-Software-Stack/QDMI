<p align="center">
  <a href="https://www.meetiqm.com">
   <picture>
     <source media="(prefers-color-scheme: dark)" srcset="docs/_static/cxx_logo_dark.svg">
     <img src="docs/_static/cxx_logo.svg" alt="Logo" width="40%">
   </picture>
  </a>
</p>

# CXX QDMI Device

A C++20 library that implements the QDMI Device interface.

## Documentation

The full documentation, including a user guide, development guide, and the C++
API reference, is available in the `docs/` directory and can be built using
[Doxygen](https://doxygen.nl/).

You can build the documentation by running:

```console
cmake -S . -B build -DBUILD_CXX_QDMI_DOCS=ON
cmake --build build --target cxx_qdmi_device_docs
```

The documentation will be generated in the `build/docs/` directory. You can view
the HTML documentation by opening the `index.html` file in a web browser.
