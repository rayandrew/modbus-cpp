#ifndef LIB_MODBUS_MODBUS_REGISTER_READ_INLINE_HPP_
#define LIB_MODBUS_MODBUS_REGISTER_READ_INLINE_HPP_

#include "bit-read.hpp"

#include <exception>

#include <struc.hpp>

#include "exception.hpp"
#include "logger.hpp"
#include "operation.hpp"
#include "utilities.hpp"

namespace modbus {
namespace request {
template <constants::function_code function_code>
base_read_registers<function_code>::base_read_registers(
    const address_t&       address,
    const read_num_regs_t& count) noexcept
    : internal::request{function_code}, address_{address}, count_{count} {}

template <constants::function_code function_code>
std::uint16_t base_read_registers<function_code>::byte_count() const {
  return count_() * 2;
}

template <constants::function_code function_code>
typename packet_t::size_type base_read_registers<function_code>::response_size()
    const {
  // byte count (1 byte) + count * 2 bytes
  return calc_adu_length(1 + byte_count());
}

template <constants::function_code function_code>
packet_t base_read_registers<function_code>::encode() {
  if (!address_.validate() || !count_.validate()) {
    throw ex::bad_data();
  }

  calc_length(data_length);
  packet_t packet = header_packet();
  packet.reserve(header_length + 1 + data_length);
  packet_t pdu = struc::pack(fmt::format(">{}", format), address_(), count_());
  packet.insert(packet.end(), pdu.begin(), pdu.end());
  return packet;
}

template <constants::function_code function_code>
void base_read_registers<function_code>::decode(const packet_t& packet) {
  try {
    if (packet.at(header_length) != utilities::to_underlying(function_code)) {
      throw ex::bad_data();
    }

    decode_header(packet);
    struc::unpack(fmt::format(">{}", format), packet.data() + header_length + 1,
                  address_.ref(), count_.ref());
  } catch (...) {
    throw ex::server_device_failure(function(), header());
  }
}
}  // namespace request

namespace response {
template <constants::function_code function_code>
base_read_registers<function_code>::base_read_registers(
    const request::base_read_registers<function_code>* request,
    table*                                             data_table) noexcept
    : internal::response{function_code, request->header(), data_table},
      request_{request} {
  initialize({request_->transaction(), request_->unit()});
}

template <constants::function_code function_code>
void base_read_registers<function_code>::decode_passed(const packet_t& packet) {
  try {
    if (packet.size() != request_->response_size()) {
#ifndef DEBUG_ON
      logger::debug("Packet size is not as same as expected response size");
#endif
      throw ex::bad_data();
    }

    packet_t::size_type byte_idx = header_length + 1;
    count_ = static_cast<std::uint16_t>(packet[byte_idx]);

    if (count_ != request_->byte_count()) {
#ifndef DEBUG_ON
      logger::debug(
          "Bytes count is not as same as expected number of registers");
#endif
      throw ex::bad_data();
    }

    block::registers::container_type buffer;

    for (int idx = 0; idx < request_->byte_count(); idx += 2) {
      std::uint16_t value;
      struc::unpack(">H", packet.data() + byte_idx + 1 + idx, value);
      buffer.push_back(value);
    }

    if (buffer.size() != request_->count().get()) {
#ifndef DEBUG_ON
      logger::debug("Bytes count is not as same as number of registers");
#endif
      throw ex::bad_data();
    }

    registers_.swap(buffer);
  } catch (...) {
    throw ex::bad_data();
  }
}
}  // namespace response
}

#endif // LIB_MODBUS_MODBUS_REGISTER_READ_INLINE_HPP_

