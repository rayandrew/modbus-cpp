[![Actions Status](https://github.com/rayandrews/modbus-cpp/workflows/MacOS/badge.svg)](https://github.com/rayandrews/modbus-cpp/actions)
[![Actions Status](https://github.com/rayandrews/modbus-cpp/workflows/Windows/badge.svg)](https://github.com/rayandrews/modbus-cpp/actions)
[![Actions Status](https://github.com/rayandrews/modbus-cpp/workflows/Ubuntu/badge.svg)](https://github.com/rayandrews/modbus-cpp/actions)
[![Actions Status](https://github.com/rayandrews/modbus-cpp/workflows/Style/badge.svg)](https://github.com/rayandrews/modbus-cpp/actions)
[![Actions Status](https://github.com/rayandrews/modbus-cpp/workflows/Install/badge.svg)](https://github.com/rayandrews/modbus-cpp/actions)
[![codecov](https://codecov.io/gh/rayandrews/modbus-cpp/branch/master/graph/badge.svg)](https://codecov.io/gh/rayandrews/modbus-cpp)

# ModbusC++ (modbus-cpp)

Modbus master and slave implementation in C++ using Boost Asio

## Supported Functions

- Read Coils (0x01)
- Read Discrete Inputs (0x02)
- Read Holding Registers (0x03)
- Read Input Registers (0x04)
- Write Single Coil (0x05)
- Write Single Register (0x06)
- Write Multiple Coils (0x0F)
- Write Multiple Registers (0x10)
- Mask Write Register (0x16)
- Read/Write Multiple Registers	(0x17)

## Usage

### Modbus slave (server)

See [server.cpp](standalone/source/server.cpp)

### Modbus master (client)

See [client.cpp](standalone/source/client.cpp)

## TODOs

- [ ] Add tests
- [ ] Complete modbus client/master
- [ ] ...

## Authors

Ray Andrew <raydreww@gmail.com>

## Acknowledgment

- [PyModbus](https://github.com/riptideio/pymodbus)
- [Libmodbus](https://github.com/stephane/libmodbus)

## LICENSE

[MIT](LICENSE)
