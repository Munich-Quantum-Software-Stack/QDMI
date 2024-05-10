# planqc backend QDMI integration

## Requirements
- python
- python package openapi-generator-cli



## Test planqc backend:
1) Configure make `mkdir build && cd build && cmake ..`
2) Make project `make clean backend_planqc test_planqc`
3) Execute test binary with `QDMI_CONFIG_FILE="../planqc_backend/planqc.qdmi-config" ./test_planqc`

Single dev line to start directly with GDB from the build directory:
```bash
make clean backend_planqc test_planqc && QDMI_CONFIG_FILE="../planqc_backend/planqc.qdmi-config" gdb -ex r --args ./test_planqc
```