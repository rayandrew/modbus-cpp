#ifndef LIB_MODBUS_SERVER_HPP_
#define LIB_MODBUS_SERVER_HPP_

#include <exception>
#include <functional>
#include <memory>
#include <string_view>

#include <boost/core/noncopyable.hpp>

#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>

#include "asio2.hpp"

#include "data-table.hpp"
#include "utilities.hpp"

namespace modbus {
class server : private boost::noncopyable {
 public:
  /**
   * Session pointer type
   */
  typedef std::shared_ptr<asio2::tcp_session> session_ptr_t;

  /**
   * Connection callback type
   *
   * Can be on connect or disconnect
   */
  typedef std::function<void(session_ptr_t&, table&)> conn_cb_t;

  /**
   * Server pointer
   */
  typedef std::unique_ptr<server> pointer;

  /**
   * Server create
   */
  MAKE_STD_UNIQUE(server)

 public:
  /**
   * Server constructor
   *
   * @param data_table  data table pointer
   * @param concurrency number of concurrency
   */
  explicit server(
      table::pointer data_table,
      std::size_t    concurrency = std::thread::hardware_concurrency() * 2);

  /**
   * Server Destructor
   */
  ~server();

  /**
   * Run server
   *
   * @param host        host to listen to
   * @param port        port to listen to
   */
  void run(std::string_view host = "0.0.0.0", std::string_view port = "1502");

  /**
   * Set on connect callback
   *
   * @param on_connect_cb
   */
  inline void bind_connect(conn_cb_t&& on_connect_callback) {
    on_connect_cb_ = std::move(on_connect_callback);
  }

  /**
   * Set on disconnect callback
   *
   * @param on_connect_cb
   */
  inline void bind_disconnect(conn_cb_t&& on_disconnect_callback) {
    on_disconnect_cb_ = std::move(on_disconnect_callback);
  }

 private:
  /**
   * Start server callback
   *
   * @param ec asio error code
   */
  void on_start(asio::error_code ec);

  /**
   * Stop server callback
   *
   * @param ec asio error code
   */
  void on_stop(asio::error_code ec);

  /**
   * Connect callback
   *
   * @param session_ptr session pointer
   */
  void on_connect(session_ptr_t& session_ptr);

  /**
   * Connect callback
   *
   * @param session_ptr session pointer
   */
  void on_disconnect(session_ptr_t& session_ptr);

  /**
   * Receive callback
   *
   * @param session_ptr session pointer
   * @param raw_packet  raw packet
   */
  void on_receive(session_ptr_t& session_ptr, std::string_view raw_packet);

 private:
  /**
   * Asio server
   */
  asio2::tcp_server server_;
  /**
   * Data table
   */
  table::pointer data_table_;
  /**
   * On connect custom callback
   */
  conn_cb_t on_connect_cb_;
  /**
   * On disconnect custom callback
   */
  conn_cb_t on_disconnect_cb_;
};
}  // namespace modbus

#endif  // LIB_MODBUS_SERVER_HPP_
