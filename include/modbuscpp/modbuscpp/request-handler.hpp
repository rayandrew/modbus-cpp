#ifndef LIB_MODBUS_MODBUS_REQUEST_HANDLER_HPP_
#define LIB_MODBUS_MODBUS_REQUEST_HANDLER_HPP_

#include <string_view>

#include <boost/core/noncopyable.hpp>

#include "types.hpp"

namespace modbus {
// forward declaration
class table;

class request_handler : private boost::noncopyable {
public:
  /**
   * Handle request
   *
   * @param data_table data table
   * @param packet     request packet
   *
   * @return packet to send
   */
  static packet_t handle(table* data_table, const std::string_view& packet);
  /**
   * Handle request
   *
   * @param data_table data table
   * @param packet     request packet
   *
   * @return packet to send
   */
  static packet_t handle(table* data_table, const packet_t& packet);
};
}  // namespace modbus

#endif  // LIB_MODBUS_MODBUS_REQUEST_HANDLER_HPP_
