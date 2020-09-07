#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <string_view>

#include <spdlog/spdlog.h>

#include <boost/asio.hpp>

#include <asio2/tcp/tcp_client.hpp>

#include <modbuscpp/modbus.hpp>

static void cout_bytes(const modbus::packet_t& packet) {
  spdlog::debug("[Packet, {}]", modbus::utilities::packet_str(packet));
}

static void cout_bytes(std::string_view packet) {
  modbus::packet_t packet_{packet.begin(), packet.end()};
  spdlog::debug("[Packet, {}]", modbus::utilities::packet_str(packet_));
}

class client_logger : public modbus::logger {
 public:
   explicit client_logger(bool debug = false) : modbus::logger(debug) {}

   virtual ~client_logger() override {}

 protected:
  inline virtual void error_impl(
      const std::string& message) const noexcept override {
    spdlog::error("{}", message);
  }

  inline virtual void debug_impl(
      const std::string& message) const noexcept override {
    if (debug_) {
      spdlog::debug("{}", message);
    }
  }

  inline virtual void info_impl(
      const std::string& message) const noexcept override {
    spdlog::info("{}", message);
  }
};

int main(int argc, char* argv[]) {
  spdlog::set_level(spdlog::level::debug);
  try {
    if (argc != 3) {
      spdlog::error("Usage: tcp_client <host> <port>");
      return 1;
    }

    modbus::logger::create<client_logger>(true);
    asio2::tcp_client client;

    /*modbus::request::read_coils req(*/
    // modbus::address_t{0x00},
    //// modbus::num_bits_t{modbus::num_bits_t::constant<0x7D0>{}}
    /*modbus::read_num_bits_t{0x7D0});*/

    /*modbus::request::write_single_coil req;*/
    /*modbus::request::write_multiple_coils req(*/
    /*modbus::address_t{0x00}, modbus::write_num_bits_t{2}, {true, true});*/
    /*modbus::request::read_holding_registers req(modbus::address_t{0x00},*/
    /*modbus::read_num_regs_t{5});*/
    /*modbus::request::write_single_register req(modbus::address_t{0x0000},*/
    /*modbus::reg_value_t{15});*/
    /*modbus::request::mask_write_register req(modbus::address_t{0x000},*/
    //[> and_mask <] modbus::mask_t{5},
    /*[> or_mask <] modbus::mask_t{2});*/

    modbus::request::read_write_multiple_registers req(
        /* read address */ modbus::address_t{0x01},
        /* read quantity */ modbus::read_num_regs_t{5},
        /* write address */ modbus::address_t{0x00},
        /* write quantity */ modbus::write_num_regs_t{5},
        /* values */ {1, 2, 3, 4, 5});
    req.initialize({0x1234, 0x01});

    auto request = req.encode();

    client.auto_reconnect(true, std::chrono::milliseconds(1000));
    // client.start_timer(1, std::chrono::seconds(1), []() {});
    client
        .bind_connect([&]([[maybe_unused]] asio::error_code ec) {
          if (asio2::get_last_error()) {
            /*LOG_DEBUG("connect failure : {} {}", asio2::last_error_val(),*/
            /*asio2::last_error_msg());*/
          } else {
            /*LOG_DEBUG("connect success : {} {}", client.local_address(),*/
            /*client.local_port());*/
          }

          cout_bytes(request);
          client.send(request);
        })
        .bind_disconnect([]([[maybe_unused]] asio::error_code ec) {
          /*LOG_DEBUG("disconnect : {} {}", asio2::last_error_val(),*/
          /*asio2::last_error_msg());*/
        })
        .bind_recv([&](std::string_view packet) {
          try {
            // modbus::response::read_coils response(&req);
            // modbus::response::write_single_coil response(&req);
            // modbus::response::write_multiple_coils response(&req);
            // modbus::response::read_holding_registers response(&req);
            // modbus::response::write_single_register response(&req);
            // modbus::response::mask_write_register response(&req);
            modbus::response::read_write_multiple_registers response(&req);
            response.decode(packet);
            cout_bytes(packet);
          } catch (const modbus::ex::specification_error& exc) {
            spdlog::error("Modbus exception occured {}", exc.what());
          } catch (const modbus::ex::base_error& exc) {
            spdlog::error("Internal exception occured {}", exc.what());
          } catch (const std::exception& exc) {
            spdlog::error("Unintended exception occured {}", exc.what());
          }
        });

    // client.start(argv[1], argv[2]);
    client.async_start(argv[1], argv[2]);

    while (std::getchar() != '\n') {
    }

    client.stop();
  } catch (std::exception& e) {
    spdlog::error("Exception {}", e.what());
  }

  return 0;
}
