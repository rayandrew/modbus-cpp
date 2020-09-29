#include <modbuscpp/modbuscpp/data-table.hpp>
#include <modbuscpp/modbuscpp/data-table.inline.hpp>

namespace modbus {
table::table(const table::initializer_t& initializer) noexcept
    : coils_{initializer.coils},
      discrete_inputs_{initializer.discrete_inputs},
      holding_registers_{initializer.holding_registers},
      input_registers_{initializer.input_registers} {}
}  // namespace modbus
