#ifndef LIB_MODBUS_MODBUS_BIT_READ_INLINE_HPP_
#define LIB_MODBUS_MODBUS_BIT_READ_INLINE_HPP_

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
base_read_bits<function_code>::base_read_bits(
    const address_t&       address,
    const read_num_bits_t& count) noexcept
    : internal::request{function_code}, address_{address}, count_{count} {}

template <constants::function_code function_code>
std::uint16_t base_read_bits<function_code>::byte_count() const {
  std::uint16_t byte_count = static_cast<std::uint16_t>(count_()) / 8;
  std::uint16_t remainder = static_cast<std::uint16_t>(count_()) % 8;

  if (remainder)
    byte_count++;
  return byte_count;
}

template <constants::function_code function_code>
typename packet_t::size_type base_read_bits<function_code>::response_size()
    const {
  return calc_adu_length(1 + byte_count());
}

template <constants::function_code function_code>
packet_t base_read_bits<function_code>::encode() {
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
void base_read_bits<function_code>::decode(const packet_t& packet) {
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
base_read_bits<function_code>::base_read_bits(
    const request::base_read_bits<function_code>* request,
    table*                                        data_table) noexcept
    : internal::response{function_code, request->header(), data_table},
      request_{request} {
  initialize({request_->transaction(), request_->unit()});
}

template <constants::function_code function_code>
void base_read_bits<function_code>::decode_passed(const packet_t& packet) {
  try {
    if (packet.size() != request_->response_size()) {
      throw ex::bad_data();
    }

    packet_t::size_type byte_idx = header_length + 1;
    count_ = static_cast<std::uint16_t>(packet[byte_idx]);

    if (count_ != request_->count().get()) {
      throw ex::bad_data();
    }

    block::bits::container_type buffer
        = op::unpack_bits(packet.begin() + byte_idx + 1, packet.end());

    if (buffer.size() != request_->count().get()) {
      throw ex::bad_data();
    }

    bits_.swap(buffer);
  } catch (...) {
    throw ex::bad_data();
  }
}
}  // namespace response
}  // namespace modbus

#endif  // LIB_MODBUS_MODBUS_BIT_READ_INLINE_HPP_
