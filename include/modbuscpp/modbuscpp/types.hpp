#ifndef LIB_MODBUS_MODBUS_TYPES_HPP_
#define LIB_MODBUS_MODBUS_TYPES_HPP_

#include <cstdint>
#include <exception>
#include <type_traits>
#include <vector>

#include <fmt/format.h>

#include "constants.hpp"

namespace modbus {
struct header_t {
  /**
   * Transaction id
   */
  std::uint16_t transaction = 0;
  /**
   * Length of PDU
   */
  std::uint16_t length = 0;
  /**
   * Unit id
   */
  std::uint8_t unit = 0;
};

/**
 * Packet type
 */
typedef std::vector<char> packet_t;
/**
 * Base packet type
 */
typedef char* base_packet_t;

namespace internal {
/**
 * Base metadata
 *
 * @tparam T metadata type
 */
template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
class base_metadata_t {
public:
  template <T Value> using constant = std::integral_constant<T, Value>;

  /**
   * Validate value
   *
   * @param value value to test
   *
   * @return true if pass the test
   */
  inline static constexpr bool validate(T value) noexcept {
    return value >= 0 && value <= constants::max_address;
  }

  /**
   * base_metadata constructor
   *
   * @param value value to set
   */
  template <T Value>
  inline explicit constexpr base_metadata_t(constant<Value>) noexcept
      : value_{Value} {
    static_assert(validate(Value));
  }

  /**
   * base_metadata constructor
   *
   * @param value value to set
   */
  inline explicit constexpr base_metadata_t(T    value = 0,
                                            bool do_validation = false) {
    if (do_validation && !validate(value)) {
      throw std::out_of_range(fmt::format("Must less than {} ({:#04x})",
                                          constants::max_address,
                                          constants::max_address));
    }
    value_ = value;
  }

  /**
   * Get reference of value
   */
  inline T& ref() noexcept { return value_; }

  /**
   * Set value
   *
   * @param value value to set
   */
  inline void set(T value) noexcept { value_ = value; }

  /**
   * Set value [operator()]
   *
   * @param other other metadata
   *
   * @param value value to set
   */
  inline void operator()(T value) noexcept { set(value); }

  /**
   * Get value
   *
   * @param other other metadata
   *
   * @return value
   */
  inline constexpr T get() const noexcept { return value_; }

  /**
   * Get value [operator()]
   *
   * @param other other metadata
   *
   * @return value
   */
  inline constexpr T operator()() const noexcept { return get(); }

  /**
   * Equality check
   *
   * @param other other metadata
   *
   * @return true if value is equal with other
   */
  inline bool operator==(const base_metadata_t& other) const noexcept {
    return value_ == other.value_;
  }

  /**
   * Less than check
   *
   * @return true if value is less than with other
   */
  inline bool operator<(const base_metadata_t& other) const noexcept {
    return value_ < other.value_;
  }

  /**
   * Less than equal check
   *
   * @param other other metadata
   *
   * @return true if value is less than equal with other
   */
  inline bool operator<=(const base_metadata_t& other) const noexcept {
    return value_ <= other.value_;
  }

  /**
   * More than check
   *
   * @return true if value is more than with other
   */
  inline bool operator>(const base_metadata_t& other) const noexcept {
    return value_ > other.value_;
  }

  /**
   * More than equal check
   *
   * @param other other metadata
   *
   * @return true if value is more than equal with other
   */
  inline bool operator>=(const base_metadata_t& other) const noexcept {
    return value_ >= other.value_;
  }

  /**
   * Inequality check
   *
   * @param other other metadata
   *
   * @return true if value is not equal with other
   */
  inline bool operator!=(const base_metadata_t& other) const noexcept {
    return value_ != other.value_;
  }

  /**
   * Minus operator
   *
   * @param other other metadata
   *
   * @return new metadata
   */
  inline base_metadata_t operator-(
      const base_metadata_t& other) const noexcept {
    return base_metadata_t(get() - other());
  }

  /**
   * Plus operator
   *
   * @param other other metadata
   *
   * @return new metadata
   */
  inline base_metadata_t operator+(
      const base_metadata_t& other) const noexcept {
    return base_metadata_t(get() + other());
  }

  /**
   * Increment operator
   *
   * @param other other metadata
   *
   * @return instance of metadata
   */
  inline base_metadata_t& operator++() noexcept {
    value_++;
    return *this;
  }

  /**
   * Decrement operator
   *
   * @param other other metadata
   *
   * @return instance of metadata
   */
  inline base_metadata_t& operator--() noexcept {
    value_--;
    return *this;
  }

  /**
   * Ostream operator
   *
   * @param os  ostream
   * @param obj metadata instance
   *
   * @return stream
   */
  template <typename ostream>
  inline friend ostream& operator<<(ostream& os, const base_metadata_t& obj) {
    return os << obj();
  }

  /**
   * Validate value
   *
   * @return true if valid
   */
  inline bool validate() noexcept { return validate(get()); }

private:
  /**
   * Value
   */
  T value_;
};
}  // namespace internal

/**
 * Address type
 */
using address_t = internal::base_metadata_t<std::uint16_t>;
/**
 * Reg value type
 */
using reg_value_t = internal::base_metadata_t<std::uint16_t>;
/**
 * Mask type
 */
using mask_t = internal::base_metadata_t<std::uint16_t>;

template <bool write> class num_bits_t
    : public internal::base_metadata_t<std::uint16_t> {
public:
  using internal::base_metadata_t<std::uint16_t>::constant;
  using internal::base_metadata_t<std::uint16_t>::get;
  using internal::base_metadata_t<std::uint16_t>::validate;

  /**
   * Validate value
   *
   * @param value value to test
   *
   * @return true if pass the test
   */
  inline static constexpr bool validate(std::uint16_t value) noexcept {
    return value > 0
           && value <= (write ? constants::max_num_bits_write
                              : constants::max_num_bits_read);
  }

  /**
   * num_regs constructor
   *
   * @param value         value to set
   */
  template <std::uint16_t Value>
  inline constexpr explicit num_bits_t(constant<Value> value)
      : internal::base_metadata_t<std::uint16_t>{value} {
    static_assert(validate(Value), "Num bits value is not valid");
  }

  /**
   * num_bits constructor
   *
   * @param value         value to set
   * @param do_validation do validation
   */
  inline explicit num_bits_t(std::uint16_t value = 1,
                             bool          do_validation = false)
      : internal::base_metadata_t<std::uint16_t>{value, false} {
    if (do_validation && !validate(value)) {
      throw std::out_of_range(fmt::format(
          "Num bits must less than {} ({:#04x})", constants::max_num_bits_read,
          constants::max_num_bits_read));
    }
  }
};

template <bool write> class num_regs_t
    : public internal::base_metadata_t<std::uint16_t> {
public:
  using internal::base_metadata_t<std::uint16_t>::constant;
  using internal::base_metadata_t<std::uint16_t>::get;
  using internal::base_metadata_t<std::uint16_t>::validate;

  /**
   * Validate value
   *
   * @param value value to test
   *
   * @return true if pass the test
   */
  inline static constexpr bool validate(std::uint16_t value) noexcept {
    return value > 0
           && value <= (write ? constants::max_num_regs_write
                              : constants::max_num_regs_read);
  }

  /**
   * num_regs constructor
   *
   * @param value         value to set
   */
  template <std::uint16_t Value>
  inline constexpr explicit num_regs_t(constant<Value> value)
      : internal::base_metadata_t<std::uint16_t>{value} {
    static_assert(validate(Value), "Num regs value is not valid");
  }

  /**
   * num_regs constructor
   *
   * @param value         value to set
   * @param do_validation do validation
   */
  inline constexpr explicit num_regs_t(std::uint16_t value = 1,
                                       bool          do_validation = false)
      : internal::base_metadata_t<std::uint16_t>{value, false} {
    if (do_validation && !validate(value)) {
      throw std::out_of_range(fmt::format(
          "Num regs must less than {} ({:#04x})", constants::max_num_regs_read,
          constants::max_num_regs_read));
    }
  }
};

using read_num_bits_t = num_bits_t<false>;
using write_num_bits_t = num_bits_t<true>;

using read_num_regs_t = num_regs_t<false>;
using write_num_regs_t = num_regs_t<true>;
}  // namespace modbus

#endif  // LIB_MODBUS_MODBUS_TYPES_HPP_
