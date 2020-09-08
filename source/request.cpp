#include <modbuscpp/details/request.hpp>

#include <modbuscpp/details/data-table.hpp>
#include <modbuscpp/details/exception.hpp>
#include <modbuscpp/details/struct.hpp>
#include <modbuscpp/details/utilities.hpp>

namespace modbus {
namespace internal {
request::request(constants::function_code function,
                 const initializer_t&     initializer)
    : adu{function, initializer} {}

request::request(constants::function_code function,
                 std::uint16_t            transaction,
                 std::uint8_t             unit)
    : adu{function, transaction, unit} {}
}  // namespace internal

namespace request {
illegal::illegal(constants::function_code function,
                 std::uint16_t            transaction,
                 std::uint8_t             unit) noexcept
    : internal::request(function, transaction, unit) {}

packet_t illegal::encode() {
  return {};
}

void illegal::decode(const packet_t& packet) {
  decode_header(packet);
  throw ex::illegal_function(function(), header());
}

typename internal::response::pointer illegal::execute([
    [maybe_unused]] table* data_table) {
  return nullptr;
}
}  // namespace request
}  // namespace modbus
