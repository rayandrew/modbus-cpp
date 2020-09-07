#include <doctest/doctest.h>

#include <string>
#include <string_view>

#include <modbuscpp/modbus.hpp>
#include <modbuscpp/version.hpp>

namespace test {
class adu : public modbus::internal::adu {
public:
  using modbus::internal::adu::initializer_t;

  explicit adu(std::uint8_t  function,
               std::uint16_t transaction = 0x00,
               std::uint8_t  unit = 0x00) noexcept
      : modbus::internal::adu(function, transaction, unit) {}

  explicit adu(modbus::constants::function_code function,
               const initializer_t&             initializer) noexcept
      : modbus::internal::adu(function, initializer) {}

  explicit adu(std::uint8_t            function_code,
               const modbus::header_t& m_header) noexcept
      : modbus::internal::adu(function_code, m_header) {}

  explicit adu(modbus::constants::function_code function,
               const modbus::header_t&          m_header) noexcept
      : modbus::internal::adu(function, m_header) {}

  virtual modbus::packet_t encode() { return {}; }
  virtual void             decode(const modbus::packet_t&) {}
};
}  // namespace test

TEST_CASE("modbuscpp metadata") {
  test::adu adu(0x01, 0x01, 0x02);

  SUBCASE("modbuscpp version") {
    static_assert(std::string_view(MODBUSCPP_VERSION)
                  == std::string_view("1.0"));
    CHECK(std::string(MODBUSCPP_VERSION) == std::string("1.0"));
  }

  SUBCASE("modbuscpp header") {
    auto header = adu.header();
    CHECK(header.length == 0x00);
    CHECK(header.transaction == 0x01);
    CHECK(header.unit == 0x02);
  }
}
