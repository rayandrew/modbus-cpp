#ifndef LIB_MODBUS_MODBUS_OPERATION_HPP_
#define LIB_MODBUS_MODBUS_OPERATION_HPP_

#include "data-table.hpp"
#include "types.hpp"

namespace modbus {
namespace op {
packet_t pack_bits(const block::bits::container_type::const_iterator& begin,
                   const block::bits::container_type::const_iterator& end);

block::bits::container_type unpack_bits(const packet_t::const_iterator& begin,
                                        const packet_t::const_iterator& end);
}  // namespace op
}  // namespace modbus

#endif  // LIB_MODBUS_MODBUS_OPERATION_HPP_
