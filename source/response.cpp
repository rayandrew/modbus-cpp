#include <modbuscpp/modbuscpp/response.hpp>

#include <fmt/format.h>

#include <modbuscpp/modbuscpp/logger.hpp>
#include <modbuscpp/modbuscpp/struct.hpp>
#include <modbuscpp/modbuscpp/types.hpp>
#include <modbuscpp/modbuscpp/utilities.hpp>

namespace modbus {
namespace internal {
response::response() noexcept {}

response::response(constants::function_code function,
                   const header_t&          req_header,
                   table*                   data_table) noexcept
    : adu{function}, req_header_{req_header}, data_table_{data_table} {}

response::~response() {}

bool response::initial_check(const packet_t& packet) {
  return packet.size() > header_length;
}

stage response::check_stage(const packet_t& packet) {
  // 1. check packet size (at least we got header and the function code)
  if (!initial_check(packet)) {
    // bad packet
    return stage::bad;
  }

  std::uint16_t tr, pr, len;
  std::uint8_t  un, fun;

  struc::unpack(fmt::format(">{}", header_func_format), packet.data(), tr, pr,
                len, un, fun);

#ifdef DEBUG_ON
  logger::debug(
      "Checking header: transaction(req[{:#04x}]=packet[{:#04x}]) "
      "protocol(req[{:#04x}]=packet[{:#04x}]) "
      "unit(req[{:#04x}]=packet[{:#04x}]) "
      "length(expected[{:#04x}]=packet[{:#04x}]",
      req_header_.transaction, tr, protocol, pr, req_header_.unit, un, len,
      (packet.size() - (header_length - 1)));
#endif

  // 2. check transaction, protocol, and unit id
  if ((req_header_.transaction != tr) || (protocol != pr)
      || (req_header_.unit != un)
      || (len != (packet.size() - (header_length - 1)))) {
    // bad packet
    return stage::bad;
  }

  // 3. unpack packet and initialize header metadata
  decode_header(packet);

  // 4. check expected function is valid or not
  if (!check_function(function())) {
    // bad packet or defined class
    return stage::bad;
  }

  auto expected_function = utilities::to_underlying(function());

  // 5. check expected function code equals with function code from packet
  if ((expected_function != function_code_)
      && !check_function(function_code_)) {
    std::uint8_t diff = function_code_ - 0x80;

    // 6. if expected function equals packet's function_code - 0x80
    //    exception is occured
    if (expected_function == diff) {
      return stage::error;
    }

    // bad packet
    return stage::bad;
  }

  // 7. all tests passed, return the "right" response
  return stage::passed;
}

void response::decode(const packet_t& packet) {
  try {
    switch (check_stage(packet)) {
      case internal::stage::bad:
        throw ex::bad_data();
      case internal::stage::error: {
        // decode the packet
        auto exc = packet.at(header_length + 1);
        throw generate_exception(static_cast<constants::exception_code>(exc),
                                 function(), header());
      } break;
      default:
        decode_passed(packet);
        break;
    }
  } catch (const std::out_of_range&) {
    // anything happens, such as packet is malformed
    throw ex::bad_data();
  }
}
}  // namespace internal

namespace response {
error::error() noexcept {}

packet_t error::encode() {
  calc_length(1);
  packet_t packet = header_packet();
  packet.pop_back();
  packet.reserve(header_length + 1 + 1);
  packet_t pdu = struc::pack(fmt::format(">{}", format),
                             utilities::to_underlying(function()) + 0x80,
                             utilities::to_underlying(ec_));
  packet.insert(packet.end(), pdu.begin(), pdu.end());

  if (packet.size() != calc_adu_length(1)) {
    throw ex::bad_data();
  }

  return packet;
}

void error::decode(const packet_t& packet) {
  if (packet.size() != calc_adu_length(1)) {
    throw ex::bad_data();
  }

  decode_header(packet);

  std::uint8_t ec;
  struc::unpack(fmt::format(">{}", format), packet.data() + header_length + 1,
                ec);

  if (!check_exception(ec)) {
    throw ex::bad_exception();
  }

  ec_ = static_cast<constants::exception_code>(ec);
}
}  // namespace response
}  // namespace modbus
