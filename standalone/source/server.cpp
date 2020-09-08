#include <chrono>
#include <iomanip>
#include <iostream>
#include <thread>
#include <utility>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>

#include <spdlog/spdlog.h>

#include <modbuscpp/modbus.hpp>

class server_logger : public modbus::logger {
public:
  explicit server_logger(bool debug = false) : modbus::logger(debug) {}

  virtual ~server_logger() override {}

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

int main([[maybe_unused]] int argc, [[maybe_unused]] const char** argv) {
  spdlog::set_level(spdlog::level::debug);
  modbus::logger::create<server_logger>(true);

  auto&& data_table = modbus::table::create(
      /*modbus::table::initializer_t{*/
      // modbus::block::bits::initializer_t{modbus::address_t{0x00}, 0xFFFF,
      // true}, modbus::block::bits::initializer_t{modbus::address_t{0x00},
      // 0xFFFF, true},
      // modbus::block::registers::initializer_t{modbus::address_t{0x00},
      // 0xFFFF, 15},
      /*modbus::block::registers::initializer_t{}}*/
  );
  auto&& server = modbus::server::create(std::move(data_table));

  server->bind_connect(
      []([[maybe_unused]] auto& session_ptr, [[maybe_unused]] auto& table) {
        // session_ptr->start_timer(1, std::chrono::seconds(1), [&table]() {
        /*LOG_INFO("Input registers addr 0x00: {:#04x}",*/
        /*table.input_registers().get(modbus::address_t{0x00}));*/
        //});
      });

  server->run();

  while (std::getchar() != '\n') {
  }

  return 0;
}
