#include <modbuscpp/details/bit-write.hpp>

#include <algorithm>
#include <exception>
#include <iterator>

#include <modbuscpp/details/exception.hpp>
#include <modbuscpp/details/logger.hpp>
#include <modbuscpp/details/operation.hpp>
#include <modbuscpp/details/struct.hpp>
#include <modbuscpp/details/utilities.hpp>

namespace modbus {
namespace request {
write_single_coil::write_single_coil(const address_t& address,
                                     value::bits      value) noexcept
    : internal::request{constants::function_code::write_single_coil},
      address_{address},
      value_{value} {}

typename packet_t::size_type write_single_coil::response_size() const {
  return calc_adu_length(data_length);
}

packet_t write_single_coil::encode() {
  if (!address_.validate()) {
    throw ex::bad_data();
  }

  calc_length(data_length);
  packet_t packet = header_packet();
  packet.reserve(header_length + 1 + data_length);
  packet_t pdu = struc::pack(fmt::format(">{}", format), address_(),
                             utilities::to_underlying(value_));
  packet.insert(packet.end(), pdu.begin(), pdu.end());
  return packet;
}

void write_single_coil::decode(const packet_t& packet) {
  try {
    if (packet.at(header_length) != utilities::to_underlying(function())) {
      throw ex::bad_data();
    }

    decode_header(packet);

    std::uint16_t temp;
    struc::unpack(fmt::format(">{}", format), packet.data() + header_length + 1,
                  address_.ref(), temp);

    if (check_bits_value(temp)) {
      value_ = static_cast<value::bits>(temp);
    } else {
      throw ex::bad_data();
    }
  } catch (...) {
    throw ex::server_device_failure(function(), header());
  }
}

typename internal::response::pointer write_single_coil::execute(
    table* data_table) {
  if (!check_bits_value(utilities::to_underlying(value_))) {
    throw ex::illegal_data_value(function(), header());
  }

  if (!data_table->coils().validate(address_)) {
    throw ex::illegal_data_address(function(), header());
  }

  return response::write_single_coil::create(this, data_table);
}

std::ostream& write_single_coil::dump(std::ostream& os) const {
  fmt::print(
      os,
      "RequestWriteSingleCoil(header[transaction={:#04x}, protocol={:#04x}, "
      "unit={:#04x}], pdu[function={:#04x}, address={:#04x}, "
      "value={:#04x}])",
      transaction_, protocol, unit_, function_code_, address()(),
      utilities::to_underlying(value_));
  return os;
}

write_multiple_coils::write_multiple_coils(
    const address_t&                              address,
    const write_num_bits_t&                       count,
    std::initializer_list<block::bits::data_type> values) noexcept
    : internal::request{constants::function_code::write_multiple_coils},
      address_{address},
      count_{count},
      values_(values) {
  byte_count_ = byte_count();
}

packet_t write_multiple_coils::encode() {
  if (!address_.validate() || !count_.validate()) {
    throw ex::bad_data();
  }

  calc_length(data_length());
  packet_t packet = header_packet();
  packet.reserve(header_length + data_length());
  packet_t pdu = struc::pack(fmt::format(">{}", format), address_(), count_(),
                             byte_count());
  packet.insert(packet.end(), pdu.begin(), pdu.end());

  packet_t values_packet = op::pack_bits(values_.begin(), values_.end());

  packet.insert(packet.end(), values_packet.begin(), values_packet.end());

  if (packet.size() != (data_length() + header_length + 1)) {
    throw ex::bad_data();
  }

  return packet;
}

void write_multiple_coils::decode(const packet_t& packet) {
  try {
    if (packet.at(header_length) != utilities::to_underlying(function())) {
      throw ex::bad_data();
    }

    decode_header(packet);

    packet_t::size_type values_idx = header_length + 1 + 5;
    std::uint8_t byte_count_recv;
    struc::unpack(fmt::format(">{}", format), packet.data() + header_length + 1,
                  address_.ref(), count_.ref(), byte_count_recv);
    byte_count_ = byte_count_recv;
    values_ = op::unpack_bits(packet.begin() + values_idx, packet.end());
    values_.resize(count_());
  } catch (...) {
    throw ex::server_device_failure(function(), header());
  }
}

typename internal::response::pointer write_multiple_coils::execute(
    table* data_table) {
  if (!count_.validate()) {
    throw ex::illegal_data_value(function(), header());
  }

  if (!data_table->coils().validate(address_, count_)) {
    throw ex::illegal_data_address(function(), header());
  }

  return response::write_multiple_coils::create(this, data_table);
}

typename packet_t::size_type write_multiple_coils::response_size() const {
  return calc_adu_length(4);
}

std::uint8_t write_multiple_coils::byte_count() const {
  std::uint8_t byte_count = static_cast<std::uint8_t>(count_()) / 8;
  std::uint8_t remainder = static_cast<std::uint8_t>(count_()) % 8;

  if (remainder)
    byte_count++;

  return byte_count;
}

std::ostream& write_multiple_coils::dump(std::ostream& os) const {
  fmt::print(
      os,
      "RequestWriteMultipleCoils(header[transaction={:#04x}, protocol={:#04x}, "
      "unit={:#04x}], pdu[function={:#04x}, address={:#04x}, quantity={:#04x}, "
      "bytes_count={:#04x}, values_size={}])",
      transaction_, protocol, unit_, function_code_, address(), count(),
      byte_count(), values_.size());
  return os;
}
}  // namespace request

namespace response {
write_single_coil::write_single_coil(const request::write_single_coil* request,
                                     table* data_table) noexcept
    : internal::response{request->function(), request->header(), data_table},
      request_{request} {
  initialize({request_->transaction(), request_->unit()});
}

packet_t write_single_coil::encode() {
  try {
    const auto& value = data_table_->coils().get(request_->address());

    value_ = value ? value::bits::on : value::bits::off;

    calc_length(data_length);
    packet_t packet = header_packet();
    packet.reserve(request_->response_size());
    packet_t pdu =
        struc::pack(fmt::format(">{}", format), request_->address()(),
                    utilities::to_underlying(value_));
    packet.insert(packet.end(), pdu.begin(), pdu.end());

    if (!request_->check_response_packet(packet)) {
      throw ex::server_device_failure(function(), header());
    }

    data_table()->coils().set(request_->address(),
                              request_->value() == value::bits::on);
    return packet;
  } catch (const std::out_of_range&) {
    throw ex::illegal_data_address(function(), header());
  } catch (...) {
    throw ex::server_device_failure(function(), header());
  }
}

void write_single_coil::decode_passed(const packet_t& packet) {
  try {
    if (packet.size() != request_->response_size()) {
      throw ex::bad_data();
    }

    packet_t::size_type address_idx = header_length + 1;

    std::uint16_t address, value;
    struc::unpack(fmt::format(">{}", format), packet.data() + address_idx,
                  address, value);

    if (request_->address()() != address) {
      logger::debug("ResponseWriteSingleCoil: Address mismatch");
      throw ex::bad_data();
    }

    if (utilities::to_underlying(request_->value()) != value &&
        check_bits_value(value)) {
      logger::debug("ResponseWriteSingleCoil: Value mismatch");
      throw ex::bad_data();
    }

    value_ = static_cast<value::bits>(value);
  } catch (...) {
    throw ex::bad_data();
  }
}

std::ostream& write_single_coil::dump(std::ostream& os) const {
  fmt::print(
      os,
      "ResponseWriteSingleCoil(header[transaction={:#04x}, protocol={:#04x}, "
      "unit={:#04x}], pdu[function={:#04x}, address={:#04x}, "
      "value={:#04x}])",
      transaction_, protocol, unit_, function_code_, request_->address(),
      utilities::to_underlying(request_->value()));
  return os;
}

write_multiple_coils::write_multiple_coils(
    const request::write_multiple_coils* request,
    table*                               data_table) noexcept
    : internal::response{request->function(), request->header(), data_table},
      request_{request} {
  initialize({request_->transaction(), request_->unit()});
}

packet_t write_multiple_coils::encode() {
  try {
    calc_length(data_length);
    packet_t packet = header_packet();
    packet.reserve(request_->response_size());
    packet_t pdu = struc::pack(fmt::format(">{}", format),
                               request_->address()(), request_->count()());
    packet.insert(packet.end(), pdu.begin(), pdu.end());
    data_table()->coils().set(request_->address(), request_->values());
    return packet;
  } catch (const std::out_of_range&) {
    throw ex::illegal_data_address(function(), header());
  } catch (...) {
    throw ex::server_device_failure(function(), header());
  }
}

void write_multiple_coils::decode_passed(const packet_t& packet) {
  try {
    if (packet.size() != request_->response_size()) {
      throw ex::bad_data();
    }

    packet_t::size_type address_idx = header_length + 1;

    struc::unpack(fmt::format(">{}", format), packet.data() + address_idx,
                  address_.ref(), count_.ref());

    if (request_->address() != address_) {
      logger::debug("ResponseWriteMultipleCoils: Address mismatch");
      throw ex::bad_data();
    }

    if (request_->count() != count_) {
      logger::debug("ResponseWriteMultipleCoils: Count mismatch");
      throw ex::bad_data();
    }
  } catch (...) {
    throw ex::bad_data();
  }
}

std::ostream& write_multiple_coils::dump(std::ostream& os) const {
  fmt::print(os,
             "ResponseWriteMultipleCoils(header[transaction={:#04x}, "
             "protocol={:#04x}, "
             "unit={:#04x}], pdu[function={:#04x}, address={:#04x}])",
             transaction_, protocol, unit_, function_code_, address());
  return os;
}
}  // namespace response
}  // namespace modbus
