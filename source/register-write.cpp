#include <modbuscpp/modbuscpp/register-write.hpp>

#include <algorithm>
#include <exception>
#include <iterator>

#include <modbuscpp/modbuscpp/exception.hpp>
#include <modbuscpp/modbuscpp/logger.hpp>
#include <modbuscpp/modbuscpp/operation.hpp>
#include <modbuscpp/modbuscpp/struct.hpp>
#include <modbuscpp/modbuscpp/utilities.hpp>

namespace modbus {
namespace request {
write_single_register::write_single_register(const address_t&   address,
                                             const reg_value_t& value) noexcept
    : internal::request{constants::function_code::write_single_register},
      address_{address},
      value_{value} {}

typename packet_t::size_type write_single_register::response_size() const {
  return calc_adu_length(data_length);
}

packet_t write_single_register::encode() {
  if (!address_.validate()) {
    throw ex::bad_data();
  }

  calc_length(data_length);
  packet_t packet = header_packet();
  packet.reserve(header_length + 1 + data_length);
  packet_t pdu = struc::pack(fmt::format(">{}", format), address_(), value_());
  packet.insert(packet.end(), pdu.begin(), pdu.end());
  return packet;
}

void write_single_register::decode(const packet_t& packet) {
  try {
    if (packet.at(header_length) != utilities::to_underlying(function())) {
      throw ex::bad_data();
    }

    decode_header(packet);

    struc::unpack(fmt::format(">{}", format), packet.data() + header_length + 1,
                  address_.ref(), value_.ref());

    // if (!(0x0000 <= value && value <= 0xFFFF)) {
    // value_ = value;
    //} else {
    // throw ex::bad_data();
    //}
  } catch (...) {
    throw ex::server_device_failure(function(), header());
  }
}

typename internal::response::pointer write_single_register::execute(
    table* data_table) {
  if (!value_.validate()) {
    throw ex::illegal_data_value(function(), header());
  }

  if (!data_table->holding_registers().validate(address_)) {
    throw ex::illegal_data_address(function(), header());
  }

  return response::write_single_register::create(this, data_table);
}

std::ostream& write_single_register::dump(std::ostream& os) const {
  fmt::print(os,
             "RequestWriteSingleRegister(header[transaction={:#04x}, "
             "protocol={:#04x}, "
             "unit={:#04x}], pdu[function={:#04x}, address={:#04x}, "
             "value={:#04x}])",
             transaction_, protocol, unit_, function_code_, address()(),
             value_);
  return os;
}

write_multiple_registers::write_multiple_registers(
    const address_t&                                   address,
    const write_num_regs_t&                            count,
    std::initializer_list<block::registers::data_type> values) noexcept
    : internal::request{constants::function_code::write_multiple_registers},
      address_{address},
      count_{count},
      values_(values) {}

packet_t write_multiple_registers::encode() {
  if (!address_.validate() || !count_.validate()) {
    throw ex::bad_data();
  }

  calc_length(data_length());
  packet_t packet = header_packet();
  packet.reserve(header_length + data_length());
  packet_t pdu = struc::pack(fmt::format(">{}", format), address_(), count_(),
                             byte_count());
  packet.insert(packet.end(), pdu.begin(), pdu.end());

  for (const auto& value : values_) {
    using namespace std::string_literals;
    auto regs = struc::pack(">H"s, value);
    packet.insert(packet.end(), regs.begin(), regs.end());
  }

  if (packet.size() != (data_length() + header_length + 1)) {
    throw ex::bad_data();
  }

  return packet;
}

void write_multiple_registers::decode(const packet_t& packet) {
  try {
    if (packet.at(header_length) != utilities::to_underlying(function())) {
      throw ex::bad_data();
    }

    decode_header(packet);

    packet_t::size_type values_idx = header_length + 1 + 5;
    std::uint8_t        byte_count_recv;
    struc::unpack(fmt::format(">{}", format), packet.data() + header_length + 1,
                  address_.ref(), count_.ref(), byte_count_recv);

    if (byte_count() != byte_count_recv) {
      throw ex::bad_data();
    }

    block::registers::container_type buffer;

    for (int idx = 0; idx < byte_count(); idx += 2) {
      std::uint16_t value;
      struc::unpack(">H", packet.data() + values_idx + idx, value);
      buffer.push_back(value);
    }

    buffer.resize(count_());
    values_.swap(buffer);
  } catch (...) {
    throw ex::server_device_failure(function(), header());
  }
}

typename internal::response::pointer write_multiple_registers::execute(
    table* data_table) {
  if (!count_.validate()) {
    throw ex::illegal_data_value(function(), header());
  }

  if (!data_table->holding_registers().validate(address_, count_)) {
    throw ex::illegal_data_address(function(), header());
  }

  return response::write_multiple_registers::create(this, data_table);
}

typename packet_t::size_type write_multiple_registers::response_size() const {
  return calc_adu_length(4);
}

std::uint8_t write_multiple_registers::byte_count() const {
  return static_cast<std::uint8_t>(count_() * 2);
}

std::ostream& write_multiple_registers::dump(std::ostream& os) const {
  fmt::print(
      os,
      "RequestWriteMultipleRegisters(header[transaction={:#04x}, "
      "protocol={:#04x}, "
      "unit={:#04x}], pdu[function={:#04x}, address={:#04x}, quantity={:#04x}, "
      "bytes_count={:#04x}, values_size={}])",
      transaction_, protocol, unit_, function_code_, address(), count(),
      byte_count(), values_.size());
  return os;
}

mask_write_register::mask_write_register(const address_t& address,
                                         const mask_t&    and_mask,
                                         const mask_t&    or_mask) noexcept
    : internal::request{constants::function_code::mask_write_register},
      address_{address},
      and_mask_{and_mask},
      or_mask_{or_mask} {}

packet_t mask_write_register::encode() {
  if (!address_.validate()) {
    throw ex::bad_data();
  }

  calc_length(data_length);
  packet_t packet = header_packet();
  packet.reserve(header_length + 1 + data_length);
  packet_t pdu = struc::pack(fmt::format(">{}", format), address_(),
                             and_mask_(), or_mask_());
  packet.insert(packet.end(), pdu.begin(), pdu.end());
  return packet;
}

void mask_write_register::decode(const packet_t& packet) {
  try {
    if (packet.at(header_length) != utilities::to_underlying(function())) {
      throw ex::bad_data();
    }

    decode_header(packet);
    struc::unpack(fmt::format(">{}", format), packet.data() + header_length + 1,
                  address_.ref(), and_mask_.ref(), or_mask_.ref());
  } catch (...) {
    throw ex::server_device_failure(function(), header());
  }
}

typename internal::response::pointer mask_write_register::execute(
    table* data_table) {
  if (!and_mask_.validate() || !or_mask_.validate()) {
    throw ex::illegal_data_value(function(), header());
  }

  if (!data_table->holding_registers().validate(address_)) {
    throw ex::illegal_data_address(function(), header());
  }

  return response::mask_write_register::create(this, data_table);
}

typename packet_t::size_type mask_write_register::response_size() const {
  return calc_adu_length(data_length);
}

std::ostream& mask_write_register::dump(std::ostream& os) const {
  fmt::print(
      os,
      "RequestMaskWriteRegister(header[transaction={:#04x}, protocol={:#04x}, "
      "unit={:#04x}], pdu[function={:#04x}, address={:#04x}, "
      "and_mask={:#04x}, or_mask={:#04x}])",
      transaction_, protocol, unit_, function_code_, address_(), and_mask_(),
      or_mask_());
  return os;
}

read_write_multiple_registers::read_write_multiple_registers(
    const address_t&                                   read_address,
    const read_num_regs_t&                             read_count,
    const address_t&                                   write_address,
    const write_num_regs_t&                            write_count,
    std::initializer_list<block::registers::data_type> values) noexcept
    : internal::
          request{constants::function_code::read_write_multiple_registers},
      read_address_{read_address},
      read_count_{read_count},
      write_address_{write_address},
      write_count_{write_count},
      values_(values) {}

packet_t read_write_multiple_registers::encode() {
  if (!read_address_.validate() || !read_count_.validate()
      || !write_address_.validate() || !write_count_.validate()) {
    throw ex::bad_data();
  }

  calc_length(data_length());
  packet_t packet = header_packet();
  packet.reserve(header_length + data_length());
  packet_t pdu
      = struc::pack(fmt::format(">{}", format), read_address_(), read_count_(),
                    write_address_(), write_count_(), byte_count());
  packet.insert(packet.end(), pdu.begin(), pdu.end());

  for (const auto& value : values_) {
    using namespace std::string_literals;
    auto regs = struc::pack(">H"s, value);
    packet.insert(packet.end(), regs.begin(), regs.end());
  }

  if (packet.size() != (data_length() + header_length + 1)) {
    throw ex::bad_data();
  }

  return packet;
}

void read_write_multiple_registers::decode(const packet_t& packet) {
  try {
    if (packet.at(header_length) != utilities::to_underlying(function())) {
      throw ex::bad_data();
    }

    decode_header(packet);

    packet_t::size_type values_idx = header_length + 1 + 9;
    std::uint8_t        byte_count_recv;
    struc::unpack(fmt::format(">{}", format), packet.data() + header_length + 1,
                  read_address_.ref(), read_count_.ref(), write_address_.ref(),
                  write_count_.ref(), byte_count_recv);

    if (byte_count() != byte_count_recv) {
      throw ex::bad_data();
    }

    block::registers::container_type buffer;

    for (int idx = 0; idx < byte_count(); idx += 2) {
      std::uint16_t value;
      struc::unpack(">H", packet.data() + values_idx + idx, value);
      buffer.push_back(value);
    }

    buffer.resize(write_count_());
    values_.swap(buffer);
  } catch (...) {
    throw ex::server_device_failure(function(), header());
  }
}

typename internal::response::pointer read_write_multiple_registers::execute(
    table* data_table) {
  if (!read_count_.validate() || !write_count_.validate()) {
    throw ex::illegal_data_value(function(), header());
  }

  if (!read_address_.validate() || !write_address_.validate()
      || !data_table->holding_registers().validate(read_address_, read_count_)
      || !data_table->holding_registers().validate(write_address_,
                                                   write_count_)) {
    throw ex::illegal_data_address(function(), header());
  }

  return response::read_write_multiple_registers::create(this, data_table);
}

std::uint8_t read_write_multiple_registers::byte_count() const {
  return static_cast<std::uint8_t>(write_count_() * 2);
}

typename packet_t::size_type read_write_multiple_registers::response_size()
    const {
  return calc_adu_length(1 + static_cast<std::uint8_t>(read_count_() * 2));
}

std::ostream& read_write_multiple_registers::dump(std::ostream& os) const {
  fmt::print(os,
             "RequestReadWriteMultipleRegisters(header[transaction={:#04x}, "
             "protocol={:#04x}, "
             "unit={:#04x}], pdu[function={:#04x}, read_address={:#04x}, "
             "read_quantity={:#04x}, write_address={:#04x}, "
             "write_quantity={:#04x}, bytes_count={:#04x}, values_size={}])",
             transaction_, protocol, unit_, function_code_, read_address(),
             read_count(), write_address(), write_count(), byte_count(),
             values_.size());
  return os;
}
}  // namespace request

namespace response {
write_single_register::write_single_register(
    const request::write_single_register* request,
    table*                                data_table) noexcept
    : internal::response{request->function(), request->header(), data_table},
      request_{request} {
  initialize({request_->transaction(), request_->unit()});
}

packet_t write_single_register::encode() {
  try {
    calc_length(data_length);
    packet_t packet = header_packet();
    packet.reserve(request_->response_size());
    packet_t pdu = struc::pack(fmt::format(">{}", format),
                               request_->address()(), request_->value()());
    packet.insert(packet.end(), pdu.begin(), pdu.end());

    if (!request_->check_response_packet(packet)) {
      throw ex::server_device_failure(function(), header());
    }

    data_table()->holding_registers().set(request_->address(),
                                          request_->value()());
    address_ = request_->address();
    value_ = request_->value();

    return packet;
  } catch (const std::out_of_range&) {
    throw ex::illegal_data_address(function(), header());
  } catch (...) {
    throw ex::server_device_failure(function(), header());
  }
}

void write_single_register::decode_passed(const packet_t& packet) {
  try {
    if (packet.size() != request_->response_size()) {
      throw ex::bad_data();
    }

    packet_t::size_type address_idx = header_length + 1;

    std::uint16_t address, value;
    struc::unpack(fmt::format(">{}", format), packet.data() + address_idx,
                  address, value);

    if (request_->address()() != address) {
      logger::debug("ResponseWriteSingleRegister: Address mismatch");
      throw ex::bad_data();
    }

    if (request_->value()() != value) {
      logger::debug("ResponseWriteSingleRegister: Value mismatch");
      throw ex::bad_data();
    }

    address_ = request_->address();
    value_ = request_->value();
  } catch (...) {
    throw ex::bad_data();
  }
}

std::ostream& write_single_register::dump(std::ostream& os) const {
  fmt::print(os,
             "ResponseWriteSingleRegister(header[transaction={:#04x}, "
             "protocol={:#04x}, "
             "unit={:#04x}], pdu[function={:#04x}, address={:#04x}, "
             "value={:#04x}])",
             transaction_, protocol, unit_, function_code_, address(), value_);
  return os;
}

write_multiple_registers::write_multiple_registers(
    const request::write_multiple_registers* request,
    table*                                   data_table) noexcept
    : internal::response{request->function(), request->header(), data_table},
      request_{request} {
  initialize({request_->transaction(), request_->unit()});
}

packet_t write_multiple_registers::encode() {
  try {
    calc_length(data_length);
    packet_t packet = header_packet();
    packet.reserve(request_->response_size());
    packet_t pdu = struc::pack(fmt::format(">{}", format),
                               request_->address()(), request_->count()());
    packet.insert(packet.end(), pdu.begin(), pdu.end());
    data_table()->holding_registers().set(request_->address(),
                                          request_->values());
    return packet;
  } catch (const std::out_of_range&) {
    throw ex::illegal_data_address(function(), header());
  } catch (...) {
    throw ex::server_device_failure(function(), header());
  }
}

void write_multiple_registers::decode_passed(const packet_t& packet) {
  try {
    if (packet.size() != request_->response_size()) {
      throw ex::bad_data();
    }

    packet_t::size_type address_idx = header_length + 1;

    struc::unpack(fmt::format(">{}", format), packet.data() + address_idx,
                  address_.ref(), count_.ref());

    if (request_->address() != address_) {
      logger::debug("ResponseWriteMultipleRegisters: Address mismatch");
      throw ex::bad_data();
    }

    if (request_->count() != count_) {
      logger::debug("ResponseWriteMultipleRegisters: Count mismatch");
      throw ex::bad_data();
    }
  } catch (...) {
    throw ex::bad_data();
  }
}

std::ostream& write_multiple_registers::dump(std::ostream& os) const {
  fmt::print(os,
             "ResponseWriteMultipleRegisters(header[transaction={:#04x}, "
             "protocol={:#04x}, "
             "unit={:#04x}], pdu[function={:#04x}, address={:#04x}])",
             transaction_, protocol, unit_, function_code_, address());
  return os;
}

mask_write_register::mask_write_register(
    const request::mask_write_register* request,
    table*                              data_table) noexcept
    : internal::response{request->function(), request->header(), data_table},
      request_{request} {
  initialize({request_->transaction(), request_->unit()});
}

packet_t mask_write_register::encode() {
  try {
    calc_length(data_length);
    packet_t packet = header_packet();
    packet.reserve(request_->response_size());
    packet_t pdu
        = struc::pack(fmt::format(">{}", format), request_->address()(),
                      request_->and_mask()(), request_->or_mask()());
    packet.insert(packet.end(), pdu.begin(), pdu.end());

    if (!request_->check_response_packet(packet)) {
      throw ex::server_device_failure(function(), header());
    }

    const auto& current_value
        = data_table()->holding_registers().get(request_->address());

    std::uint16_t new_value
        = (current_value & request_->and_mask()()) | request_->or_mask()();
    data_table()->holding_registers().set(request_->address(), new_value);
    address_ = request_->address();
    and_mask_ = request_->and_mask();
    or_mask_ = request_->or_mask();

    return packet;
  } catch (const std::out_of_range&) {
    throw ex::illegal_data_address(function(), header());
  } catch (...) {
    throw ex::server_device_failure(function(), header());
  }
}

void mask_write_register::decode_passed(const packet_t& packet) {
  try {
    if (packet.size() != request_->response_size()) {
      throw ex::bad_data();
    }

    packet_t::size_type address_idx = header_length + 1;

    std::uint16_t address, and_mask, or_mask;
    struc::unpack(fmt::format(">{}", format), packet.data() + address_idx,
                  address, and_mask, or_mask);

    if (request_->address()() != address) {
      logger::debug("ResponseMaskWriteRegister: Address mismatch");
      throw ex::bad_data();
    }

    if (request_->and_mask()() != and_mask) {
      logger::debug("ResponseMaskWriteRegister: And mask mismatch");
      throw ex::bad_data();
    }

    if (request_->or_mask()() != or_mask) {
      logger::debug("ResponseMaskWriteRegister: Or mask mismatch");
      throw ex::bad_data();
    }

    address_ = request_->address();
    and_mask_ = request_->and_mask();
    or_mask_ = request_->or_mask();
  } catch (...) {
    throw ex::bad_data();
  }
}

std::ostream& mask_write_register::dump(std::ostream& os) const {
  fmt::print(os,
             "ResponseMaskWriteRegister(header[transaction={:#04x}, "
             "protocol={:#04x}, "
             "unit={:#04x}], pdu[function={:#04x}, address={:#04x}, "
             "and_mask={:#04x}, or_mask={:#04x}])",
             transaction_, protocol, unit_, function_code_, address(),
             and_mask(), or_mask());
  return os;
}

read_write_multiple_registers::read_write_multiple_registers(
    const request::read_write_multiple_registers* request,
    table*                                        data_table) noexcept
    : internal::response{request->function(), request->header(), data_table},
      request_{request} {
  initialize({request_->transaction(), request_->unit()});
  count_ = static_cast<std::uint8_t>(request_->read_count()() * 2);
}

packet_t read_write_multiple_registers::encode() {
  try {
    data_table()->holding_registers().set(request_->write_address(),
                                          request_->values());

    const auto& [start, end] = data_table()->holding_registers().get(
        request_->read_address(), request_->read_count());

    registers_ = block::registers::container_type{start, end};

    calc_length(1 + count_);
    packet_t packet = header_packet();
    packet.reserve(request_->response_size());
    packet_t pdu = struc::pack(fmt::format(">{}", format), count_);
    packet.insert(packet.end(), pdu.begin(), pdu.end());

    for (auto ptr = start; ptr < end; ++ptr) {
      using namespace std::string_literals;
      auto regs = struc::pack(">H"s, *ptr);
      packet.insert(packet.end(), regs.begin(), regs.end());
    }

    if (!request_->check_response_packet(packet)) {
      throw ex::server_device_failure(function(), header());
    }

    return packet;
  } catch (const std::out_of_range&) {
    throw ex::illegal_data_address(function(), header());
  } catch (...) {
    throw ex::server_device_failure(function(), header());
  }
}

void read_write_multiple_registers::decode_passed(const packet_t& packet) {
  try {
    if (packet.size() != request_->response_size()) {
      throw ex::bad_data();
    }

    uint8_t byte_count_recv
        = static_cast<std::uint8_t>(request_->read_count().get() * 2);
    packet_t::size_type byte_idx = header_length + 1;
    count_ = static_cast<std::uint8_t>(packet[byte_idx]);

    if (count_ != byte_count_recv) {
      logger::debug(
          "ResponseReadWriteMultipleRegisters: Byte register read count "
          "mismatch");
      throw ex::bad_data();
    }

    block::registers::container_type buffer;

    for (int idx = 0; idx < count_; idx += 2) {
      std::uint16_t value;
      struc::unpack(">H", packet.data() + byte_idx + 1 + idx, value);
      buffer.push_back(value);
    }

    if (buffer.size() != request_->read_count().get()) {
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

std::ostream& read_write_multiple_registers::dump(std::ostream& os) const {
  fmt::print(os,
             "ResponseReadWriteMultipleRegisters(header[transaction={:#04x}, "
             "protocol={:#04x}, "
             "unit={:#04x}], pdu[function={:#04x}, byte_count={:#04x}, "
             "register_size={:#04x}])",
             transaction_, protocol, unit_, function_code_, byte_count(),
             registers_.size());
  return os;
}
}  // namespace response
}  // namespace modbus
