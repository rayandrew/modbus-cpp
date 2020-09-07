#ifndef LIB_MODBUS_MODBUS_REQUEST_HPP_
#define LIB_MODBUS_MODBUS_REQUEST_HPP_

#include <cstdlib>
#include <type_traits>

#include "adu.hpp"
#include "constants.hpp"
#include "logger.hpp"
#include "types.hpp"

#include "response.hpp"

namespace modbus {
// forward declarations
class table;
namespace internal {
class request;
}
namespace request {
class illegal;
}

namespace internal {
class request : public adu {
 public:
  /**
   * Initializer
   */
  using typename adu::initializer_t;

  /**
   * Decode string_view
   */
  using adu::decode;

  /**
   * Request constructor
   *
   * @param function    modbus function
   * @param transaction transaction id
   * @param unit        unit id
   */
  explicit request(constants::function_code function,
                   std::uint16_t            transaction = 0x00,
                   std::uint8_t             unit = 0x00);

  /**
   * Request constructor
   *
   * @param function    modbus function
   * @param initializer initializer
   */
  explicit request(constants::function_code function,
                   const initializer_t&     initializer);

  /**
   * Execute on data store / mapping
   *
   * @param  data_table data table
   *
   * @return modbus response
   */
  virtual typename response::pointer execute(table* data_table) = 0;

  /**
   * Get response size for error checking on client
   *
   * @return response size
   */
  inline virtual typename packet_t::size_type response_size() const {
    return 0;
  }

  /**
   * Check if response packet is mismatch with expected packet size
   */
  inline bool check_response_packet(const packet_t& packet) const {
    return packet.size() == response_size();
  }
};
}  // namespace internal

namespace request {
/**
 * illegal request class
 *
 * @author Ray Andrew
 * @date   August 2020
 *
 * Encode, decode, and execute illegal request
 *
 * Will return modbus illegal function exception
 */
class illegal : public internal::request {
 public:
  /**
   * Illegal request constructor
   *
   * @param function    modbus function
   * @param transaction transaction id
   * @param unit        unit id
   */
  explicit illegal(
      constants::function_code function = constants::function_code::min,
      std::uint16_t            transaction = 0x00,
      std::uint8_t             unit = 0x00) noexcept;

  /**
   * Encode illegal request packet from given data
   *
   * @return packet format
   */
  virtual packet_t encode() override;

  /**
   * Decode illegal request packet
   *
   * @param packet packet to decode
   *
   * @return packet format
   */
  virtual void decode(const packet_t& packet) override;

  /**
   * Get response size for error checking on client
   *
   * @return response size
   */
  inline virtual typename packet_t::size_type response_size() const override {
    return response::error::packet_size;
  }

  /**
   * Execute on data store / mapping
   *
   * @param  data_table data table
   *
   * @return modbus response
   */
  virtual typename internal::response::pointer execute(
      table* data_table) override;
};
}  // namespace request
}  // namespace modbus

#endif  // LIB_MODBUS_MODBUS_REQUEST_HPP_
