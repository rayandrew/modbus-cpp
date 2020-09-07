#include <modbuscpp/details/operation.hpp>

namespace modbus {
namespace op {

packet_t pack_bits(const block::bits::container_type::const_iterator& begin,
                   const block::bits::container_type::const_iterator& end) {
  packet_t packet;

  char shift = 0;
  char one_byte = 0;

  for (auto ptr = begin; ptr < end; ++ptr) {
    one_byte |= static_cast<char>(*ptr) << shift;
    if (shift == 7) {
      packet.push_back(one_byte);
      one_byte = shift = 0;
    } else {
      shift++;
    }
  }

  if (shift != 0) {
    packet.push_back(one_byte);
  }

  return packet;
}

block::bits::container_type unpack_bits(const packet_t::const_iterator& begin,
                                        const packet_t::const_iterator& end) {
  block::bits::container_type result;

  for (auto ptr = begin; ptr < end; ++ptr) {
    for (int bit = 0x01; bit & 0xff; bit <<= 1) {
      result.push_back(static_cast<block::bits::data_type>(*ptr & bit));
    }
  }

  return result;
}
}  // namespace op
}  // namespace modbus
