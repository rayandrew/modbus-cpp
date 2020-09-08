#include <modbuscpp/details/register-read.hpp>
#include <modbuscpp/details/register-read.inline.hpp>

#include <algorithm>
#include <exception>

#include <struc.hpp>

#include <modbuscpp/details/exception.hpp>
#include <modbuscpp/details/logger.hpp>
#include <modbuscpp/details/operation.hpp>
#include <modbuscpp/details/utilities.hpp>

namespace modbus {
namespace request {
template class base_read_registers<
    constants::function_code::read_holding_registers>;
template class base_read_registers<
    constants::function_code::read_input_registers>;

template <> std::ostream&
base_read_registers<constants::function_code::read_holding_registers>::dump(
    std::ostream& os) const {
  fmt::print(os,
             "RequestReadHoldingRegisters(header[transaction={:#04x}, "
             "protocol={:#04x}, "
             "unit={:#04x}], pdu[function={:#04x}, address={:#04x}, count={}])",
             transaction_, protocol, unit_, function_code_, address()(),
             count());
  return os;
}

template <> typename internal::response::pointer
base_read_registers<constants::function_code::read_holding_registers>::execute(
    table* data_table) {
  if (!count_.validate()) {
    throw ex::illegal_data_value(function(), header());
  }

  if (!data_table->holding_registers().validate(address_, count_)) {
    throw ex::illegal_data_address(function(), header());
  }

  return response::base_read_registers<
      constants::function_code::read_holding_registers>::create(this,
                                                                data_table);
}

template <> std::ostream&
base_read_registers<constants::function_code::read_input_registers>::dump(
    std::ostream& os) const {
  fmt::print(
      os,
      "RequestReadInputRegisters(header[transaction={:#04x}, protocol={:#04x}, "
      "unit={:#04x}], pdu[function={:#04x}, address={:#04x}, count={}])",
      transaction_, protocol, unit_, function_code_, address()(), count());
  return os;
}

template <> typename internal::response::pointer
base_read_registers<constants::function_code::read_input_registers>::execute(
    table* data_table) {
  if (!count_.validate()) {
    throw ex::illegal_data_value(function(), header());
  }

  if (!data_table->input_registers().validate(address_, count_)) {
    throw ex::illegal_data_address(function(), header());
  }

  return response::base_read_registers<
      constants::function_code::read_input_registers>::create(this, data_table);
}
}  // namespace request

namespace response {
template class base_read_registers<
    constants::function_code::read_holding_registers>;
template class base_read_registers<
    constants::function_code::read_input_registers>;

template <> std::ostream&
base_read_registers<constants::function_code::read_holding_registers>::dump(
    std::ostream& os) const {
  fmt::print(os,
             "ResponseReadHoldingRegisters(header[transaction={:#04x}, "
             "protocol={:#04x}, "
             "unit={:#04x}], pdu[function={:#04x}, address={:#04x}, "
             "count={}, registers_size={}])",
             transaction_, protocol, unit_, function_code_,
             request_->address()(), request_->count(), registers_.size());
  return os;
}

template <> packet_t base_read_registers<
    constants::function_code::read_holding_registers>::encode() {
  try {
    const auto& [start, end] = data_table()->holding_registers().get(
        request_->address(), request_->count());

    registers_ = block::registers::container_type{start, end};
    calc_length(request_->byte_count() + 1);
    packet_t packet = header_packet();
    packet.reserve(request_->response_size());
    packet_t pdu
        = struc::pack(fmt::format(">{}", format), request_->byte_count());
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

template <> std::ostream&
base_read_registers<constants::function_code::read_input_registers>::dump(
    std::ostream& os) const {
  fmt::print(os,
             "ResponseReadInputRegisters(header[transaction={:#04x}, "
             "protocol={:#04x}, "
             "unit={:#04x}], pdu[function={:#04x}, address={:#04x}, "
             "count={}, registers_size={}])",
             transaction_, protocol, unit_, function_code_,
             request_->address()(), request_->count(), registers_.size());
  return os;
}

template <> packet_t
base_read_registers<constants::function_code::read_input_registers>::encode() {
  try {
    const auto& [start, end] = data_table()->input_registers().get(
        request_->address(), request_->count());

    registers_ = block::registers::container_type{start, end};
    calc_length(request_->byte_count() + 1);
    packet_t packet = header_packet();
    packet.reserve(request_->response_size());
    packet_t pdu
        = struc::pack(fmt::format(">{}", format), request_->byte_count());
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
}  // namespace response
}  // namespace modbus
