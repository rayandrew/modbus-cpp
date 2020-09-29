#include <modbuscpp/modbuscpp/logger.hpp>

namespace modbus {
logger* logger::instance_ = nullptr;

logger::logger(bool debug) : debug_{debug} {}

logger::~logger() {}

logger* logger::get() {
  if (instance_ == nullptr) {
    create();
  }

  return instance_;
}

void logger::info_impl(const std::string& message) const noexcept {
  fmt::print("{}\n", message);
}

void logger::error_impl(const std::string& message) const noexcept {
  info(message);
}

void logger::debug_impl(const std::string& message) const noexcept {
  if (debug_) {
    info(message);
  }
}
}  // namespace modbus
