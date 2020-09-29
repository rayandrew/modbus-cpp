#ifndef LIB_MODBUS_MODBUS_BIT_WRITE_HPP_
#define LIB_MODBUS_MODBUS_BIT_WRITE_HPP_

#include <cstdint>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <string_view>

#include <fmt/core.h>
#include <fmt/ostream.h>

#include "constants.hpp"
#include "types.hpp"

#include "data-table.hpp"

#include "adu.hpp"
#include "request.hpp"
#include "response.hpp"

namespace modbus {
// forward declarations
namespace request {
class write_single_coil;
class write_multiple_coils;
}  // namespace request

namespace response {
class write_single_coil;
class write_multiple_coils;
}  // namespace response

namespace request {
/**
 * request write single coil class
 *
 * @author Ray Andrew
 * @date   August 2020
 *
 * Encode, decode, and execute write singe coil request
 *
 * Structure:
 * [ (Header...)              ]
 * [ Function (1 byte)        ]
 * [ Output Address (2 bytes) ]
 * [ Output Value (2 bytes)   ]
 */
class write_single_coil : public internal::request {
public:
  /**
   * request::write_single_coil constructor
   *
   * @param address    output address
   * @param value      coil value
   */
  explicit write_single_coil(const address_t& address = address_t{},
                             value::bits      value = value::bits::on) noexcept;

  /**
   *
   * Encode write single_coil packet from given data
   *
   * @return packet format
   */
  virtual packet_t encode() override;

  /**
   * Decode write single coil packet
   *
   * @param data data to be appended
   *
   * @return packet format
   */
  virtual void decode(const packet_t& data) override;

  /**
   * Encode write bits packet
   *
   * @return packet format
   */
  virtual typename internal::response::pointer execute(
      table* data_table) override;

  /**
   * Get response size for error checking on client
   *
   * @return response size
   */
  inline virtual typename packet_t::size_type response_size() const override;

  /**
   * Dump to string
   *
   * @param  os  ostream
   *
   * @return ostream
   */
  virtual std::ostream& dump(std::ostream& os) const override;

public:
  /**
   * Get address
   *
   * @return address
   */
  inline const address_t& address() const { return address_; }

  /**
   * Get value
   *
   * @return value
   */
  inline value::bits value() const { return value_; }

private:
  /**
   * Data length (4 bytes)
   */
  static constexpr std::uint16_t data_length = 4;
  /**
   * Address
   */
  address_t address_;
  /**
   * Value
   */
  value::bits value_;
  /**
   * Struct format
   */
  static constexpr std::string_view format = "HH";
};

/**
 * request write multiple coils class
 *
 * @author Ray Andrew
 * @date   August 2020
 *
 * Encode, decode, and execute write multiple coils request
 *
 * Structure:
 * [ (Header...)                   ]
 * [ Function (1 byte)             ]
 * [ Starting Address (2 bytes)    ]
 * [ Quantity of outputs (2 bytes) ]
 * [ Byte count N (1 byte)         ]
 * [ Output value (N x 1 bytes)    ]
 */
class write_multiple_coils : public internal::request {
public:
  /**
   * request::write_multiple_coils constructor
   *
   * @param address    output address
   * @param count      count
   * @param values     coil values
   */
  explicit write_multiple_coils(
      const address_t&                              address = address_t{},
      const write_num_bits_t&                       count = write_num_bits_t{},
      std::initializer_list<block::bits::data_type> values = {}) noexcept;

  /**
   *
   * Encode write single_coil packet from given data
   *
   * @return packet format
   */
  virtual packet_t encode() override;

  /**
   * Decode write single coil packet
   *
   * @param data data to be appended
   *
   * @return packet format
   */
  virtual void decode(const packet_t& data) override;

  /**
   * Encode write bits packet
   *
   * @return packet format
   */
  virtual typename internal::response::pointer execute(
      table* data_table) override;

  /**
   * Get response size for error checking on client
   *
   * @return response size
   */
  inline virtual typename packet_t::size_type response_size() const override;

  /**
   * Byte count
   *
   * @return byte count
   */
  std::uint8_t byte_count() const;

  /**
   * Dump to string
   *
   * @param  os  ostream
   *
   * @return ostream
   */
  virtual std::ostream& dump(std::ostream& os) const override;

public:
  /**
   * Get address
   *
   * @return address
   */
  inline const address_t& address() const { return address_; }

  /**
   * Get address
   *
   * @return address
   */
  inline const write_num_bits_t& count() const { return count_; }

  /**
   * Get value
   *
   * @return value
   */
  inline const block::bits::container_type& values() const { return values_; }

private:
  /**
   * Get data length
   *
   * Except Function size + header
   *
   * @return data length
   */
  inline std::uint16_t data_length() const { return 4 + 1 + byte_count_; }

private:
  /**
   * Address
   */
  address_t address_;
  /**
   * Count
   */
  write_num_bits_t count_;
  /**
   * Value
   */
  block::bits::container_type values_;
  /**
   * Byte count
   */
  std::uint8_t byte_count_;
  /**
   * Struct format
   */
  static constexpr std::string_view format = "HHB";
};
}  // namespace request

namespace response {
/**
 * response write single coil class
 *
 * @author Ray Andrew
 * @date   August 2020
 *
 * Structure:
 * [ (Header...)              ]
 * [ Function (1 byte)        ]
 * [ Output Address (2 bytes) ]
 * [ Output Value (2 bytes)   ]
 */
class write_single_coil : public internal::response {
public:
  /**
   * Create std::unique_ptr of response::write_single_coil
   *
   * @return std::unique_ptr of response::write_single_coil
   */
  MAKE_STD_UNIQUE(write_single_coil)

  /**
   * response::write_single_coil constructor
   *
   * @param request    read coils request pointer
   * @param data_table data table
   */
  explicit write_single_coil(const request::write_single_coil* request,
                             table* data_table = nullptr) noexcept;

  /**
   * Encode packet
   *
   * @return packet format
   */
  virtual packet_t encode() override;

  /**
   * Decode stage passed  packet
   *
   * @param packet packet to parse
   */
  virtual void decode_passed(const packet_t& packet) override;

  /**
   * Dump to string
   *
   * @param  os  ostream
   *
   * @return ostream
   */
  virtual std::ostream& dump(std::ostream& os) const override;

private:
  /**
   * Request pointer
   */
  const request::write_single_coil* request_;
  /**
   * Value
   */
  value::bits value_;
  /**
   * Data length (4 bytes)
   */
  static constexpr std::uint16_t data_length = 4;
  /**
   * Struct format
   */
  static constexpr std::string_view format = "HH";
};

/**
 * response write multiple coils class
 *
 * @author Ray Andrew
 * @date   August 2020
 *
 * Structure:
 * [ (Header...)                   ]
 * [ Function (1 byte)             ]
 * [ Starting Address (2 bytes)    ]
 * [ Quantity of Outputs (2 bytes) ]
 */
class write_multiple_coils : public internal::response {
public:
  /**
   * Create std::unique_ptr of response::write_multiple_coils
   *
   * @return std::unique_ptr of response::write_multiple_coils
   */
  MAKE_STD_UNIQUE(write_multiple_coils)

  /**
   * response::write_multiple_coils constructor
   *
   * @param request    read coils request pointer
   * @param data_table data table
   */
  explicit write_multiple_coils(const request::write_multiple_coils* request,
                                table* data_table = nullptr) noexcept;

  /**
   * Encode packet
   *
   * @return packet format
   */
  virtual packet_t encode() override;

  /**
   * Decode stage passed  packet
   *
   * @param packet packet to parse
   */
  virtual void decode_passed(const packet_t& packet) override;

  /**
   * Dump to string
   *
   * @param  os  ostream
   *
   * @return ostream
   */
  virtual std::ostream& dump(std::ostream& os) const override;

  /**
   * Get address
   *
   * @return address
   */
  inline const address_t& address() const { return address_; }

  /**
   * Get address
   *
   * @return address
   */
  inline const write_num_bits_t& count() const { return count_; }

private:
  /**
   * Request pointer
   */
  const request::write_multiple_coils* request_;
  /**
   * Address
   */
  address_t address_;
  /**
   * Count
   */
  write_num_bits_t count_;
  /**
   * Data length (4 bytes)
   */
  static constexpr std::uint16_t data_length = 4;
  /**
   * Struct format
   */
  static constexpr std::string_view format = "HH";
};
}  // namespace response
}  // namespace modbus

#endif  // LIB_MODBUS_MODBUS_BIT_WRITE_HPP_
