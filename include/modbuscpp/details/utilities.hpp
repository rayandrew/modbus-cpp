#ifndef LIB_MODBUS_MODBUS_UTILITIES_HPP_
#define LIB_MODBUS_MODBUS_UTILITIES_HPP_

#include <arpa/inet.h>
#include <cstdint>
#include <memory>
#include <string>
#include <type_traits>

#include "types.hpp"

#include "exception.hpp"

/**
 * @def MAKE_STD_SHARED(T)
 *
 * Create shared_ptr out of any class
 *
 * This macro will generate the static function called `create_shared`
 * that returns shared_ptr<T>
 *
 * Usage:
 * MAKE_STD_SHARED(SomeClass)
 *
 * @param T class to be enabled
 */
#define MAKE_STD_SHARED(T)                                   \
  template <typename... Args>                                \
  inline static auto create(Args&&... args) {                \
    return std::make_shared<T>(std::forward<Args>(args)...); \
  }

/**
 * @def MAKE_STD_UNIQUE(T)
 *
 * Create unique_ptr out of any class
 *
 * This macro will generate the static function called `create_unique`
 * that returns unique_ptr<T>
 *
 * Usage:
 * MAKE_STD_UNIQUE(SomeClass)
 *
 * @param T class to be enabled
 */
#define MAKE_STD_UNIQUE(T)                                   \
  template <typename... Args>                                \
  inline static auto create(Args&&... args) {                \
    return std::make_unique<T>(std::forward<Args>(args)...); \
  }

namespace modbus {
namespace utilities {
// Get from  https://stackoverflow.com/a/33083231
// Credits to :
// - R. Martinho Fernandes
// (https://stackoverflow.com/users/46642/r-martinho-fernandes)
// - Class Skeleton (https://stackoverflow.com/users/3387452/class-skeleton)
template <typename T>
constexpr auto to_underlying(T value) noexcept {
  return static_cast<std::underlying_type_t<T>>(value);
}

/**
 * Convert type
 */
template <typename T, std::enable_if_t<(!is_iterable<T>)>* = nullptr>
inline void convert_type(packet_t&                  packet,
                         const T&                   value,
                         const packet_t::size_type& start_index = 0) {
  typedef const typename packet_t::value_type byte_array[sizeof value];

  if (packet.size() <=
      start_index + (sizeof(T) / sizeof(packet_t::size_type))) {
    throw ex::out_of_range("Out of bounds");
  }

  int increment = 0;
  for (auto& byte : reinterpret_cast<byte_array&>(value)) {
    packet[start_index + (increment++)] = byte;
  }
}

/**
 * Unpack
 */
template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
inline constexpr T unpack(const packet_t&            packet,
                          const packet_t::size_type& start_index = 0) {
  if constexpr (std::is_same_v<packet_t::value_type, T>) {
    return packet[start_index];
  } else if constexpr (std::is_same_v<T, std::uint16_t>) {
    if (packet.size() <=
        start_index + (sizeof(std::uint16_t) / sizeof(packet_t::size_type))) {
      throw ex::out_of_range("Out of bounds");
    }

    return ntohs(*(std::uint16_t*)(packet.data() + start_index));
  } else if constexpr (std::is_same_v<T, std::uint32_t>) {
    if (packet.size() <=
        start_index + (sizeof(std::uint32_t) / sizeof(packet_t::size_type))) {
      throw ex::out_of_range("Out of bounds");
    }
    return ntohl(*(std::uint32_t*)(packet.data() + start_index));
  }

  return 0;
}

inline std::string packet_str(const packet_t& packet) {
  packet_t::size_type index = 0;

  std::string s = "[";

  for (const auto& byte : packet) {
    index++;
    if (index < packet.size()) {
      s += fmt::format("{:#04x} ", static_cast<std::uint8_t>(byte));
    } else {
      s += fmt::format("{:#04x}", static_cast<std::uint8_t>(byte));
    }
  }

  s += "]";
  return s;
}
}  // namespace utilities
}  // namespace modbus

#endif  // LIB_MODBUS_MODBUS_UTILITIES_HPP_
