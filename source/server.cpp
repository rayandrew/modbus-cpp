#include <modbuscpp/details/server.hpp>

#include <utility>

#include <fmt/format.h>

#include <modbuscpp/details/adu.hpp>
#include <modbuscpp/details/constants.hpp>
#include <modbuscpp/details/logger.hpp>
#include <modbuscpp/details/request-handler.hpp>
#include <modbuscpp/details/types.hpp>

namespace modbus {
server::server(table::pointer data_table, std::size_t concurrency)
    : server_{constants::max_adu_length, constants::max_adu_length,
              concurrency},
      data_table_{std::move(data_table)},
      on_connect_cb_{[](auto&, auto&) {}},
      on_disconnect_cb_{[](auto&, auto&) {}} {
  server_.bind_start(&server::on_start, this)
      .bind_stop(&server::on_stop, this)
      .bind_connect(&server::on_connect, this)
      .bind_disconnect(&server::on_disconnect, this)
      .bind_recv(&server::on_receive, this);
}

server::~server() {
  server_.stop();
}

void server::on_start(asio::error_code ec) {
  logger::debug("starting tcp server @ {} {}, message: {}",
                server_.listen_address(), server_.listen_port(), ec.message());
}

void server::on_stop(asio::error_code ec) {
  logger::debug("stopping tcp server, message: {}", ec.message());
}

void server::on_connect(session_ptr_t& session_ptr) {
  session_ptr->no_delay(true);
  on_connect_cb_(session_ptr, *data_table_);
  logger::debug("client enters: {} {} {} {}", session_ptr->remote_address(),
                session_ptr->remote_port(), session_ptr->local_address(),
                session_ptr->local_port());
}

void server::on_disconnect(session_ptr_t& session_ptr) {
  on_disconnect_cb_(session_ptr, *data_table_);
  logger::debug("client leaves: {} {} {}", session_ptr->remote_address(),
                session_ptr->remote_port(), asio2::last_error_msg());
}

void server::on_receive(session_ptr_t&   session_ptr,
                        std::string_view raw_packet) {
  auto response = request_handler::handle(data_table_.get(), raw_packet);

#ifdef DEBUG_ON
  logger::debug("[Response, {}]", utilities::packet_str(response));
#endif

  if (!response.empty()) {
    session_ptr->send(response, []([[maybe_unused]] std::size_t bytes_sent) {
#ifdef DEBUG_ON
      logger::debug("bytes sent {}", bytes_sent);
#endif
    });
  }
}

void server::run(std::string_view host, std::string_view port) {
  server_.start(host, port);
}
}  // namespace modbus
