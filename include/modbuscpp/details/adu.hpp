#ifndef LIB_MODBUS_MODBUS_ADU_HPP_
#define LIB_MODBUS_MODBUS_ADU_HPP_

#include <cstdint>
#include <iostream>
#include <string_view>
#include <type_traits>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include "constants.hpp"
#include "types.hpp"
#include "utilities.hpp"

namespace modbus {
// forward declarations
namespace internal {
class adu;
}

namespace internal {
/**
 * @brief ADU base class
 *
 * @author  Ray Andrew
 * @ingroup Modbus/Internal
 *
 * ADU structure for TCP:
 * - Header (7 bytes)
 * --- Transaction (2 bytes)
 * --- Protocol (2 bytes)
 * --- Length (2 bytes) [ unit identifier + PDU ]
 * --- Unit (1 byte)
 * - PDU
 * --- Function (1 byte)
 * --- Rest of data... (N byte)
 */
class adu {
 public:
  /**
   * Initializer
   */
  struct initializer_t {
    /**
     * Transaction
     */
    std::uint16_t transaction;
    /**
     * Unit
     */
    std::uint8_t unit;
  };

  /**
   * ADU constructor
   */
  explicit adu() noexcept;

  /**
   * ADU constructor
   *
   * @param function_code modbus function code
   * @param transaction   transaction id
   * @param unit          unit id
   */
  explicit adu(std::uint8_t  function_code,
               std::uint16_t transaction = 0x00,
               std::uint8_t  unit = 0x00) noexcept;

  /**
   * ADU constructor
   *
   * @param function    modbus function
   * @param transaction transaction id
   * @param unit        unit id
   */
  explicit adu(constants::function_code function,
               std::uint16_t            transaction = 0x00,
               std::uint8_t             unit = 0x00) noexcept;

  /**
   * ADU constructor
   *
   * @param function_code modbus function code
   * @param initializer   initializer
   */
  explicit adu(std::uint8_t         function_code,
               const initializer_t& initializer) noexcept;

  /**
   * ADU constructor
   *
   * @param function    modbus function
   * @param initializer initializer
   */
  explicit adu(constants::function_code function,
               const initializer_t&     initializer) noexcept;

  /**
   * ADU constructor
   *
   * @param function_code modbus function code
   * @param m_header  header struct
   */
  explicit adu(std::uint8_t function_code, const header_t& m_header) noexcept;

  /**
   * ADU constructor
   *
   * @param function  modbus function
   * @param m_header  header struct
   */
  explicit adu(constants::function_code function,
               const header_t&          m_header) noexcept;

 public:
  /**
   * Encode packet
   *
   * @return packet that has been encoded
   */
  virtual packet_t encode() = 0;

  /**
   * Decode packet
   *
   * @param packet packet to be decoded
   */
  virtual void decode(std::string_view packet);

  /**
   * Decode packet
   *
   * @param packet packet to be decoded
   */
  virtual void decode(const packet_t& packet) = 0;

 public:
  /** Getter */
  /**
   * Get function code
   *
   * @return function code
   */
  inline constants::function_code function() const { return function_; }

  /**
   * Get transaction id
   *
   * @return transaction id
   */
  inline std::uint16_t transaction() const { return transaction_; }

  /**
   * Get length
   *
   * @return length
   */
  inline std::uint16_t length() const { return length_; }

  /**
   * Get unit id
   *
   * @return unit id
   */
  inline std::uint8_t unit() const { return unit_; }

  /**
   * Get header
   *
   * @return header
   */
  header_t header() const { return {transaction(), length(), unit()}; }

  /** Setter */

  /**
   * Iniitalize ADU
   *
   * @param initializer initializer
   *
   * @return instance of ADU
   */
  adu& initialize(const initializer_t& initializer);

  /**
   * Set header
   *
   * @param m_header  header struct
   *
   * @return instance of ADU
   */
  adu& header(const header_t& m_header);

  /**
   * Set transaction id
   *
   * @param new_transaction new transaction id
   *
   * @return instance of ADU
   */
  adu& transaction(std::uint16_t new_transaction);

  /**
   * Calculate length of ADU given PDU length
   *
   * @param pdu_length PDU length
   *
   * @return instance of ADU
   */
  adu& calc_length(std::uint16_t pdu_length);

  /**
   * Set length of ADU
   *
   * @param new_length new length
   *
   * @return instance of ADU
   */
  adu& length(std::uint16_t new_length);

  /**
   * Set unit id
   *
   * @param new_unit new unit id
   *
   * @return instance of ADU
   */
  adu& unit(std::uint8_t new_unit);

  /** Operator */
  /**
   * Equality operator
   *
   * @param other other ADU
   *
   * @return true if transaction_id is equal to other transaction_id
   */
  bool operator==(const adu& other) const;

  /**
   * Less-than operator
   *
   * @param other other ADU
   *
   * @return true if transaction_id is less than other transaction_id
   */
  bool operator<(const adu& other) const;

  /**
   * More-than operator
   *
   * @param  other other ADU
   *
   * @return true if transaction_id is more than other transaction_id
   */
  bool operator>(const adu& other) const;

  /**
   * Dump to string
   *
   * @param  os  ostream
   *
   * @return ostream
   */
  virtual std::ostream& dump(std::ostream& os) const;

  /**
   * Ostream operator
   *
   * @param os  ostream
   * @param adu adu instance
   *
   * @return stream
   */
  template <typename ostream>
  inline friend ostream& operator<<(ostream& os, const adu& obj) {
    return obj.dump(os);
  }

 protected:
  /**
   * Decode packet header
   *
   * @param packet packet to be decoded
   *
   * @return instance of ADU
   */
  void decode_header(const packet_t& packet);

  /**
   * Get header packet
   *
   * @return packet consists of header
   */
  packet_t header_packet();

  /**
   * Response size
   *
   * @param pdu_length PDU length
   *
   * @return ADU length
   */
  inline static constexpr std::uint16_t calc_adu_length(
      std::uint16_t data_length) {
    /** header_length (containing unit_id) + 1 (function) + data_length */
    return header_length + 1 + data_length;
  }

 public:
  /**
   * Header length
   */
  static constexpr typename packet_t::size_type header_length = 7;

 protected:
  /**
   * Protocol ID
   */
  static constexpr std::uint16_t protocol = constants::tcp_protocol;

  /**
   * Length index
   */
  static constexpr typename packet_t::size_type length_idx = 4;
  /**
   * Max length
   */
  static constexpr typename packet_t::size_type max_length =
      constants::max_adu_length;
  /**
   * Max PDU length
   */
  static constexpr typename packet_t::size_type max_pdu_size =
      max_length - header_length;
  /**
   * Header struct with function format
   */
  static constexpr std::string_view header_func_format = "HHHBB";

 protected:
  /**
   * Function
   */
  constants::function_code function_;
  /**
   * Function code
   */
  std::underlying_type_t<constants::function_code> function_code_;
  /**
   * Transaction id
   */
  std::uint16_t transaction_;
  /**
   * Length of packet [ unit identifier + PDU ]
   */
  std::uint16_t length_;
  /**
   * Unit id
   */
  std::uint8_t unit_;
};
}  // namespace internal
}  // namespace modbus

#endif  // LIB_MODBUS_MODBUS_ADU_HPP_
