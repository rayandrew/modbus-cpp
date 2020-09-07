#include <modbuscpp/details/adu.hpp>

#include <string>

#include <modbuscpp/details/struct.hpp>
#include <modbuscpp/details/utilities.hpp>

namespace modbus {
namespace internal {
adu::adu() noexcept : function_code_{0x00} {}

adu::adu(constants::function_code function,
         std::uint16_t            transaction,
         std::uint8_t             unit) noexcept
    : function_{function},
      function_code_{utilities::to_underlying(function)},
      transaction_{transaction},
      length_{0x00},
      unit_{unit} {}

adu::adu(std::uint8_t  function_code,
         std::uint16_t transaction,
         std::uint8_t  unit) noexcept
    : function_code_{function_code},
      transaction_{transaction},
      length_{0x00},
      unit_{unit} {
  if (check_function(function_code_)) {
    function_ = static_cast<constants::function_code>(function_code_);
  }
}

adu::adu(constants::function_code function,
         const initializer_t&     initializer) noexcept
    : function_{function},
      function_code_{utilities::to_underlying(function)},
      transaction_{initializer.transaction},
      length_{0x00},
      unit_{initializer.unit} {}

adu::adu(std::uint8_t function_code, const initializer_t& initializer) noexcept
    : function_code_{function_code},
      transaction_{initializer.transaction},
      length_{0x00},
      unit_{initializer.unit} {
  if (check_function(function_code_)) {
    function_ = static_cast<constants::function_code>(function_code_);
  }
}

adu::adu(constants::function_code function, const header_t& m_header) noexcept
    : function_{function},
      function_code_{utilities::to_underlying(function)},
      transaction_{m_header.transaction},
      length_{m_header.length},
      unit_{m_header.unit} {}

adu::adu(std::uint8_t function_code, const header_t& m_header) noexcept
    : function_code_{function_code},
      transaction_{m_header.transaction},
      length_{m_header.length},
      unit_{m_header.unit} {
  if (check_function(function_code_)) {
    function_ = static_cast<constants::function_code>(function_code_);
  }
}

/** Setter */
adu& adu::header(const header_t& m_header) {
  transaction_ = m_header.transaction;
  length_ = m_header.length;
  unit_ = m_header.unit;
  return *this;
}

adu& adu::initialize(const initializer_t& initializer) {
  transaction_ = initializer.transaction;
  length_ = 0x00;
  unit_ = initializer.unit;
  return *this;
}

adu& adu::transaction(std::uint16_t new_transaction) {
  transaction_ = new_transaction;
  return *this;
}

adu& adu::calc_length(std::uint16_t pdu_length) {
  // length = unit id (1 byte) + function code (1 byte) + PDU length
  length_ = 1 + 1 + pdu_length;
  return *this;
}

adu& adu::length(std::uint16_t new_length) {
  length_ = new_length;
  return *this;
}

adu& adu::unit(std::uint8_t new_unit) {
  unit_ = new_unit;
  return *this;
}

void adu::decode_header(const packet_t& packet) {
  std::uint16_t temp;
  struc::unpack(fmt::format(">{}", header_func_format), packet.data(),
                transaction_, temp, length_, unit_, function_code_);

  if (check_function(function_code_)) {
    function_ = static_cast<constants::function_code>(function_code_);
  }
}

bool adu::operator==(const adu& other) const {
  return transaction_ && other.transaction_;
}

bool adu::operator>(const adu& other) const {
  return transaction_ > other.transaction_;
}

bool adu::operator<(const adu& other) const {
  return transaction_ < other.transaction_;
}

packet_t adu::header_packet() {
  packet_t packet =
      struc::pack(fmt::format(">{}", header_func_format), transaction_,
                  protocol, length_, unit_, function_code_);
  return packet;
}

void adu::decode(std::string_view packet) {
  decode(packet_t{packet.begin(), packet.end()});
}

std::ostream& adu::dump(std::ostream& os) const {
  fmt::print(os,
             "ADU(header[transaction={:#04x}, protocol={:#04x}, "
             "function={:#04x}, unit={:#04x}])",
             transaction_, protocol, function_code_, unit_);
  return os;
}
}  // namespace internal
}  // namespace modbus
