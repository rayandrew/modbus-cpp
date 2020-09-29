#ifndef LIB_MODBUS_MODBUS_REGISTER_READ_HPP_
#define LIB_MODBUS_MODBUS_REGISTER_READ_HPP_

#include <cstdint>
#include <iostream>
#include <memory>
#include <string_view>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include "constants.hpp"
#include "types.hpp"

#include "data-table.hpp"
#include "data-table.inline.hpp"

#include "adu.hpp"
#include "request.hpp"
#include "response.hpp"

namespace modbus {
namespace request {
/**
 * base request read registers class
 *
 * @author Ray Andrew
 * @date   August 2020
 *
 * Encode, decode, and execute read registers request
 *
 * Structure :
 * [ (Header...)                     ]
 * [ Function (1 byte)               ]
 * [ Starting Address (2 bytes)      ]
 * [ Quantity of registers (2 bytes) ]
 */
template <constants::function_code function_code> class base_read_registers
    : public internal::request {
public:
  /**
   * request::base_read_registers constructor
   *
   * @param address address requested
   * @param count   count   requested
   */
  explicit base_read_registers(const address_t&       address = address_t{},
                               const read_num_regs_t& count
                               = read_num_regs_t{}) noexcept;

  /**
   * Encode read registers packet from given data
   *
   * @return packet format
   */
  virtual packet_t encode() override;

  /**
   * Decode read registers packet
   *
   * @param data data to be appended
   *
   * @return packet format
   */
  virtual void decode(const packet_t& data) override;

  /**
   * Encode read registers packet
   *
   * @return packet format
   */
  virtual typename internal::response::pointer execute(
      table* data_table) override;

  /**
   * Byte count
   *
   * @return byte count
   */
  std::uint16_t byte_count() const;

  /**
   * Get response size for error checking on client
   *
   * @return response size
   */
  inline virtual typename packet_t::size_type response_size() const override;

  /**
   * Get address
   *
   * @return address
   */
  inline const address_t& address() const { return address_; }

  /**
   * Get count
   *
   * @return count
   */
  inline const read_num_regs_t& count() const { return count_; }

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
   * Data length (4 bytes)
   */
  static constexpr std::uint16_t data_length = 4;
  /**
   * Address
   */
  address_t address_;
  /**
   * Number of registers
   */
  read_num_regs_t count_;
  /**
   * Struct format
   */
  static constexpr std::string_view format = "HH";
};

using read_holding_registers
    = base_read_registers<constants::function_code::read_holding_registers>;
using read_input_registers
    = base_read_registers<constants::function_code::read_input_registers>;
}  // namespace request

namespace response {
/**
 * base response read registers class
 *
 * @author Ray Andrew
 * @date   August 2020
 *
 * Encode and decode read registers response
 *
 * Structure :
 * [ (Header...)                 ]
 * [ Function (1 byte)           ]
 * [ Byte count = 2 x N (1 byte) ]
 * [ Registers (N * 2 bytes)     ]
 *
 * N = quantity of registers
 */
template <constants::function_code function_code> class base_read_registers
    : public internal::response {
public:
  /**
   * Create std::unique_ptr of response::read_registers
   * @return std::unique_ptr of response::read_registers
   */
  MAKE_STD_UNIQUE(base_read_registers)

  /**
   * response::read_registers constructor
   *
   * @param request    read registers request pointer
   * @param data_table data table
   */
  explicit base_read_registers(
      const request::base_read_registers<function_code>* request,
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
   * Byte count
   *
   * @return byte count
   */
  inline std::uint16_t byte_count() const { return count_; }

  /**
   * Get registers
   */
  inline const block::registers::container_type& registers() const {
    return registers_;
  }

private:
  /**
   * Request pointer
   */
  const request::base_read_registers<function_code>* request_;
  /**
   * Byte count
   */
  std::uint16_t count_;
  /**
   * Slice of data from block of registers from data table
   */
  block::registers::container_type registers_;
  /**
   * Struct format
   */
  static constexpr std::string_view format = "B";
};

using read_holding_registers
    = base_read_registers<constants::function_code::read_holding_registers>;
using read_input_registers
    = base_read_registers<constants::function_code::read_input_registers>;
}  // namespace response
}  // namespace modbus

#endif  // LIB_MODBUS_MODBUS_REGISTER_READ_HPP_
