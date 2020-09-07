#ifndef LIB_MODBUS_MODBUS_REGISTER_WRITE_HPP_
#define LIB_MODBUS_MODBUS_REGISTER_WRITE_HPP_

#include <cstdint>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <string_view>

#include <fmt/format.h>
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
class write_single_register;
class write_multiple_registers;
class mask_write_register;
}  // namespace request

namespace response {
class write_single_register;
class write_multiple_registers;
class mask_write_register;
}  // namespace response

namespace request {
/**
 * request write single register class
 *
 * @author Ray Andrew
 * @date   August 2020
 *
 * Encode, decode, and execute write single request
 *
 * Structure :
 * [ (Header...)                 ]
 * [ Function (1 byte)           ]
 * [ Register Address (2 bytes)  ]
 * [ Register Value (2 bytes)    ]
 */
class write_single_register : public internal::request {
 public:
  /**
   * request::write_single_register constructor
   *
   * @param address address requested
   * @param value   value   requested
   */
  explicit write_single_register(
      const address_t&   address = address_t{},
      const reg_value_t& value = reg_value_t{}) noexcept;

  /**
   * Encode write single register packet from given data
   *
   * @return packet format
   */
  virtual packet_t encode() override;

  /**
   * Decode write single register packet
   *
   * @param data data to be appended
   *
   * @return packet format
   */
  virtual void decode(const packet_t& data) override;

  /**
   * Encode write single register packet
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
  virtual typename packet_t::size_type response_size() const override;

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
  inline const reg_value_t& value() const {
    return value_;
  }

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
   * Value
   */
  reg_value_t value_;
  /**
   * Struct format
   */
  static constexpr std::string_view format = "HH";
};

/**
 * request write multiple registers class
 *
 * @author Ray Andrew
 * @date   August 2020
 *
 * Encode, decode, and execute write multiple registers request
 *
 * Structure:
 * [ (Header...)                   ]
 * [ Function (1 byte)             ]
 * [ Starting Address (2 bytes)    ]
 * [ Quantity of outputs (2 bytes) ]
 * [ Byte count N (1 byte)         ]
 * [ Output value (N x 1 bytes)    ]
 */
class write_multiple_registers : public internal::request {
 public:
  /**
   * request::write_multiple_registers constructor
   *
   * @param address    output address
   * @param count      count
   * @param values     coil values
   */
  explicit write_multiple_registers(
      const address_t&        address = address_t{},
      const write_num_regs_t& count = write_num_regs_t{},
      std::initializer_list<block::registers::data_type> values = {}) noexcept;

  /**
   *
   * Encode write multiple registers packet from given data
   *
   * @return packet format
   */
  virtual packet_t encode() override;

  /**
   * Decode write multiple registers packet
   *
   * @param data data to be appended
   *
   * @return packet format
   */
  virtual void decode(const packet_t& data) override;

  /**
   * Encode write multiple registers packet
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
  virtual typename packet_t::size_type response_size() const override;

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
   * Get count
   *
   * @return count
   */
  inline const write_num_regs_t& count() const { return count_; }

  /**
   * Byte count
   *
   * @return byte count
   */
  std::uint8_t byte_count() const;

  /**
   * Get value
   *
   * @return value
   */
  inline const block::registers::container_type& values() const {
    return values_;
  }

 private:
  /**
   * Get data length
   *
   * Except Function size + header
   *
   * @return data length
   */
  inline std::uint16_t data_length() const { return 4 + 1 + byte_count(); }

 private:
  /**
   * Address
   */
  address_t address_;
  /**
   * Count
   */
  write_num_regs_t count_;
  /**
   * Value
   */
  block::registers::container_type values_;
  /**
   * Struct format
   */
  static constexpr std::string_view format = "HHB";
};

/**
 * request mask write register class
 *
 * @author Ray Andrew
 * @date   August 2020
 *
 * Encode, decode, and execute mask write register request
 *
 * Structure :
 * [ (Header...)                 ]
 * [ Function (1 byte)           ]
 * [ Reference Address (2 bytes) ]
 * [ And Mask (2 bytes)          ]
 * [ Or Mask (2 bytes)           ]
 */
class mask_write_register : public internal::request {
 public:
  /**
   * request::mask_write_register constructor
   *
   * @param address  address requested
   * @param and_mask and mask requested
   * @param or_mask  or mask requested
   */
  explicit mask_write_register(const address_t& address = address_t{},
                               const mask_t&    and_mask = mask_t{0x00},
                               const mask_t& or_mask = mask_t{0x00}) noexcept;

  /**
   * Encode read registers packet from given data
   *
   * @return packet format
   */
  virtual packet_t encode() override;

  /**
   * Decode read registers packet
   *
   * @param packet packet to decode
   *
   * @return packet format
   */
  virtual void decode(const packet_t& packet) override;

  /**
   * Encode read registers packet
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
  virtual typename packet_t::size_type response_size() const override;

  /**
   * Get address
   *
   * @return address
   */
  inline const address_t& address() const { return address_; }

  /**
   * Get and mask
   *
   * @return and mask
   */
  inline const mask_t& and_mask() const { return and_mask_; }

  /**
   * Get and mask
   *
   * @return and mask
   */
  inline const mask_t& or_mask() const { return or_mask_; }

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
   * Data length (6 bytes)
   */
  static constexpr std::uint16_t data_length = 6;
  /**
   * Address
   */
  address_t address_;
  /**
   * And mask
   */
  mask_t and_mask_;
  /**
   * Or mask
   */
  mask_t or_mask_;
  /**
   * Struct format
   */
  static constexpr std::string_view format = "HHH";
};

/**
 * request read/write multiple registers class
 *
 * @author Ray Andrew
 * @date   August 2020
 *
 * Encode, decode, and execute write single request
 *
 * Structure :
 * [ (Header...)                 ]
 * [ Function (1 byte)           ]
 * [ Read Address (2 bytes)      ]
 * [ Read Quantity (2 bytes)     ]
 * [ Write Address (2 bytes)     ]
 * [ Write Quantity (2 bytes)    ]
 * [ Write Byte Count (1 byte)   ]
 */
class read_write_multiple_registers : public internal::request {
 public:
  /**
   * request::read_write_multiple_registers constructor
   *
   * @param read_address  read address requested
   * @param read_count    read count requested
   * @param write_address write address requested
   * @param write_count   write count requested
   * @param values        values requested
   */
  explicit read_write_multiple_registers(
      const address_t&        read_address = address_t{},
      const read_num_regs_t&  read_count = read_num_regs_t{},
      const address_t&        write_address = address_t{},
      const write_num_regs_t& write_count = write_num_regs_t{},
      std::initializer_list<block::registers::data_type> values = {}) noexcept;

  /**
   * Encode read write multiple registers packet from given data
   *
   * @return packet format
   */
  virtual packet_t encode() override;

  /**
   * Decode read write multiple registers packet
   *
   * @param packet packet to decode
   *
   * @return packet format
   */
  virtual void decode(const packet_t& packet) override;

  /**
   * Encode read write multiple registers packet
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
  virtual typename packet_t::size_type response_size() const override;

  /**
   * Byte count
   *
   * @return byte count
   */
  std::uint8_t byte_count() const;

  /**
   * Get read address
   *
   * @return read address
   */
  inline const address_t& read_address() const { return read_address_; }

  /**
   * Get read count
   *
   * @return read count
   */
  inline const read_num_regs_t& read_count() const { return read_count_; }

  /**
   * Get write address
   *
   * @return write address
   */
  inline const address_t& write_address() const { return write_address_; }

  /**
   * Get write count
   *
   * @return write count
   */
  inline const write_num_regs_t& write_count() const { return write_count_; }

  /**
   * Get values
   *
   * @return values
   */
  inline const block::registers::container_type& values() const {
    return values_;
  }

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
   * Get data length
   *
   * Except Function size + header
   *
   * @return data length
   */
  inline std::uint16_t data_length() const {
    return 2 + 2 + 2 + 2 + 1 + byte_count();
  }

 private:
  /**
   * Read address
   */
  address_t read_address_;
  /**
   * Read count
   */
  read_num_regs_t read_count_;
  /**
   * Write address
   */
  address_t write_address_;
  /**
   * Write count
   */
  write_num_regs_t write_count_;
  /**
   * Values
   */
  block::registers::container_type values_;
  /**
   * Struct format
   */
  static constexpr std::string_view format = "HHHHB";
};
}

namespace response {
/**
 * response write single register class
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
class write_single_register : public internal::response {
 public:
  /**
   * Create std::unique_ptr of response::write_single_register
   *
   * @return std::unique_ptr of response::write_single_register
   */
  MAKE_STD_UNIQUE(write_single_register)

  /**
   * response::write_single_register constructor
   *
   * @param request    write single register request pointer
   * @param data_table data table
   */
  explicit write_single_register(const request::write_single_register* request,
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
  inline const reg_value_t& value() const { return value_; }

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
  const request::write_single_register* request_;
  /**
   * Address
   */
  address_t address_;
  /**
   * Value
   */
  reg_value_t value_;
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
 * response write multiple registers class
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
class write_multiple_registers : public internal::response {
 public:
  /**
   * Create std::unique_ptr of response::write_multiple_registers
   *
   * @return std::unique_ptr of response::write_multiple_registers
   */
  MAKE_STD_UNIQUE(write_multiple_registers)

  /**
   * response::write_multiple_registers constructor
   *
   * @param request    read registers request pointer
   * @param data_table data table
   */
  explicit write_multiple_registers(
      const request::write_multiple_registers* request,
      table*                                   data_table = nullptr) noexcept;

  /**
   * Encode packet
   *
   * @return packet format
   */
  virtual packet_t encode() override;

  /**
   * Decode stage passed packet
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
  inline const write_num_regs_t& count() const { return count_; }

 private:
  /**
   * Request pointer
   */
  const request::write_multiple_registers* request_;
  /**
   * Address
   */
  address_t address_;
  /**
   * Count
   */
  write_num_regs_t count_;
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
 * response mask write register class
 *
 * @author Ray Andrew
 * @date   August 2020
 *
 * Structure :
 * [ (Header...)                 ]
 * [ Function (1 byte)           ]
 * [ Reference Address (2 bytes) ]
 * [ And Mask (2 bytes)          ]
 * [ Or Mask (2 bytes)           ]
 */
class mask_write_register : public internal::response {
 public:
  /**
   * Create std::unique_ptr of response::mask_write_register
   *
   * @return std::unique_ptr of response::mask_write_register
   */
  MAKE_STD_UNIQUE(mask_write_register)

  /**
   * response::mask_write_register constructor
   *
   * @param request    mask write register request pointer
   * @param data_table data table
   */
  explicit mask_write_register(const request::mask_write_register* request,
                               table* data_table = nullptr) noexcept;

  /**
   * Encode packet
   *
   * @return packet format
   */
  virtual packet_t encode() override;

  /**
   * Decode stage passed packet
   *
   * @param packet packet to parse
   */
  virtual void decode_passed(const packet_t& packet) override;

  /**
   * Get address
   *
   * @return address
   */
  inline const address_t& address() const { return address_; }

  /**
   * Get and mask
   *
   * @return and mask
   */
  inline const mask_t& and_mask() const { return and_mask_; }

  /**
   * Get and mask
   *
   * @return and mask
   */
  inline const mask_t& or_mask() const { return or_mask_; }

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
  const request::mask_write_register* request_;
  /**
   * Address
   */
  address_t address_;
  /**
   * And mask
   */
  mask_t and_mask_;
  /**
   * Or mask
   */
  mask_t or_mask_;
  /**
   * Data length (4 bytes)
   */
  static constexpr std::uint16_t data_length = 6;
  /**
   * Struct format
   */
  static constexpr std::string_view format = "HHH";
};

/**
 * response read write multiple registers class
 *
 * @author Ray Andrew
 * @date   August 2020
 *
 * Encode and decode read write multiple registers response
 *
 * Structure :
 * [ (Header...)                 ]
 * [ Function (1 byte)           ]
 * [ Byte count = 2 x N (1 byte) ]
 * [ Registers (N * 2 bytes)     ]
 *
 * N = quantity of registers
 */
class read_write_multiple_registers : public internal::response {
 public:
  /**
   * Create std::unique_ptr of response::read_write_multiple_registers
   *
   * @return std::unique_ptr of response::read_write_multiple_registers
   */
  MAKE_STD_UNIQUE(read_write_multiple_registers)

  /**
   * response::read_write_multiple_registers constructor
   *
   * @param request    read write multiple registers request pointer
   * @param data_table data table
   */
  explicit read_write_multiple_registers(
      const request::read_write_multiple_registers* request,
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
  inline std::uint8_t byte_count() const { return count_; }

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
  const request::read_write_multiple_registers* request_;
  /**
   * Byte count
   */
  std::uint8_t count_;
  /**
   * Slice of data from block of registers from data table
   */
  block::registers::container_type registers_;
  /**
   * Struct format
   */
  static constexpr std::string_view format = "B";
};
}  // namespace response
}  // namespace modbus

#endif  // LIB_MODBUS_MODBUS_REGISTER_WRITE_HPP_
