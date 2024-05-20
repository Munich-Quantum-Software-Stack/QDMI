# planqc backend QDMI integration

The planqc backend for QDMI makes it possible to connect to the planqc infrastructure and submit jobs on connected quantum computers and simulators.

## Requirements
- Python 3.7+
- Python package openapi-generator-cli `pip install openapi-generator-cli`
- The executable `openapi-generator` in your PATH environment variable. This can be checked with `which openapi-generator`


## Test planqc backend:
1) Configure make `mkdir build && cd build && cmake ..`. This step adds the planqc related targets only to the Makefile when openapi-generator can be found in the PATH environment variable.
2) Make project `make clean backend_planqc test_planqc`
3) Execute test binary with `QDMI_CONFIG_FILE="../planqc_api/planqc.qdmi-config" ./test_planqc`

Single dev line to start directly with GDB from the build directory:
```bash
make clean backend_planqc test_planqc && QDMI_CONFIG_FILE="../planqc_api/planqc.qdmi-config" gdb -ex r --args ./test_planqc
```

## Configuration
The planqc backend can be configured over a QDMI config file. A example config file can be found as [`planqc.qdmi-config`](planqc.qdmi-config) in this directory.

#### Parameters:
```
planqc.qdmi-config


```