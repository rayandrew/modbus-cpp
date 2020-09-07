#ifndef LIB_MODBUS_MODBUS_EXCEPTION_HPP_
#define LIB_MODBUS_MODBUS_EXCEPTION_HPP_

#include <exception>
#include <string>
#include <type_traits>

#include <boost/core/noncopyable.hpp>

#include "constants.hpp"
#include "types.hpp"

namespace modbus {
namespace ex {
// forward declarations
template <constants::exception_code modbus_exception>
class specification;

template <constants::exception_code modbus_exception>
class internal;

using out_of_range = std::out_of_range;

/** modbus spec exception */
using illegal_function =
    specification<constants::exception_code::illegal_function>;
using illegal_data_address =
    specification<constants::exception_code::illegal_data_address>;
using illegal_data_value =
    specification<constants::exception_code::illegal_data_value>;
using server_device_failure =
    specification<constants::exception_code::server_device_failure>;
using acknowledge = specification<constants::exception_code::acknowledge>;
using negative_acknowledge =
    specification<constants::exception_code::negative_acknowledge>;
using server_device_busy =
    specification<constants::exception_code::server_device_busy>;
using memory_parity_error =
    specification<constants::exception_code::memory_parity_error>;
using gateway_path_unavailable =
    specification<constants::exception_code::gateway_path_unavailable>;
using gateway_target_device_failed_to_respond = specification<
    constants::exception_code::gateway_target_device_failed_to_respond>;

/** internal exception */
using bad_data = internal<constants::exception_code::bad_data>;
using bad_data_size = internal<constants::exception_code::bad_data_size>;
using bad_exception = internal<constants::exception_code::bad_exception>;
using no_exception = internal<constants::exception_code::no_exception>;

class base_error : private boost::noncopyable, public std::exception {
 public:
  inline explicit base_error(
      constants::exception_code modbus_exception) noexcept
      : exception_code_{modbus_exception} {}

  /**
   * Get exception code
   *
   * @return exception code
   */
  inline constexpr constants::exception_code code() const noexcept {
    return exception_code_;
  }

 public:
  /**
   * Exception code
   */
  const constants::exception_code exception_code_;
};

class specification_error : public base_error {
 public:
  /**
   * Specification error constructor
   *
   * @param function       modbus function
   * @param request_header request header
   */
  explicit specification_error(constants::exception_code modbus_exception,
                               constants::function_code  function,
                               const header_t&           header) noexcept
      : base_error{modbus_exception}, function_{function}, header_{header} {}

  /**
   * Get request header
   *
   * @return request header
   */
  inline const header_t& header() const noexcept { return header_; }

  /**
   * Get request header
   *
   * @return request header
   */
  inline const constants::function_code& function() const noexcept {
    return function_;
  }

 private:
   /**
    * Function code
    */
   const constants::function_code function_;
   /**
    * Request header
    */
   const header_t header_;
};

template <constants::exception_code modbus_exception>
class specification : public specification_error {
  static_assert(
      modbus_exception >= constants::exception_code::illegal_function &&
      modbus_exception <=
          constants::exception_code::gateway_target_device_failed_to_respond);

 public:
  /**
   * Specification exception constructor
   *
   * @param function       modbus function
   * @param request_header request header
   */
  explicit specification(constants::function_code function,
                         const header_t&          header) noexcept
      : specification_error{modbus_exception, function, header} {}

  /**
   * Exception explanation
   *
   * @return explanation of exception
   */
  virtual const char* what() const noexcept override { return message(); }

 private:
  /**
   * Exception message
   *
   * @return exception message
   */
  inline constexpr const char* message() const noexcept {
    if constexpr (modbus_exception ==
                  constants::exception_code::illegal_function) {
      return "Illegal function";
    } else if constexpr (modbus_exception ==
                         constants::exception_code::illegal_data_address) {
      return "Illegal data address";
    } else if constexpr (modbus_exception ==
                         constants::exception_code::illegal_data_value) {
      return "Illegal data value";
    } else if constexpr (modbus_exception ==
                         constants::exception_code::server_device_failure) {
      return "Service device failure";
    } else if constexpr (modbus_exception ==
                         constants::exception_code::acknowledge) {
      return "Acknowledge";
    } else if constexpr (modbus_exception ==
                         constants::exception_code::server_device_busy) {
      return "Server device busy";
    } else if constexpr (modbus_exception ==
                         constants::exception_code::memory_parity_error) {
      return "Memory parity error";
    } else if constexpr (modbus_exception ==
                         constants::exception_code::gateway_path_unavailable) {
      return "Gateway path unavailable";
    } else if constexpr (modbus_exception ==
                         constants::exception_code::
                             gateway_target_device_failed_to_respond) {
      return "Gateway target device failed to respond";
    }

    return "Specification error";
  }
};

template <constants::exception_code modbus_exception>
class internal : public base_error {
  static_assert(modbus_exception >= constants::exception_code::bad_data &&
                modbus_exception <= constants::exception_code::no_exception);

 public:
  /**
   * Internal exception constructor
   *
   * @param request_header request header
   */
  internal() : base_error{modbus_exception} {}

  /**
   * Exception explanation
   *
   * @return explanation of exception
   */
  virtual const char* what() const noexcept override { return message(); }

 private:
  /**
   * Exception message
   *
   * @return exception message
   */
  inline constexpr const char* message() const noexcept {
    if constexpr (modbus_exception == constants::exception_code::bad_data) {
      return "Bad data";
    } else if constexpr (modbus_exception ==
                         constants::exception_code::bad_data_size) {
      return "Bad data size";
    } else if constexpr (modbus_exception ==
                         constants::exception_code::connection_problem) {
      return "Connection problem";
    } else if constexpr (modbus_exception ==
                         constants::exception_code::bad_exception) {
      return "Bad exception";
    } else if constexpr (modbus_exception ==
                         constants::exception_code::no_exception) {
      return "No exception";
    }

    return "Internal error";
  }
};
}  // namespace ex

inline void throw_exception(constants::exception_code ec,
                            constants::function_code  function,
                            const header_t&           request_header) {
  switch (ec) {
    case constants::exception_code::illegal_function:
      throw ex::illegal_function(function, request_header);
    case constants::exception_code::illegal_data_address:
      throw ex::illegal_data_address(function, request_header);
    case constants::exception_code::illegal_data_value:
      throw ex::illegal_data_value(function, request_header);
    case constants::exception_code::server_device_failure:
      throw ex::server_device_failure(function, request_header);
    case constants::exception_code::acknowledge:
      throw ex::acknowledge(function, request_header);
    case constants::exception_code::server_device_busy:
      throw ex::server_device_busy(function, request_header);
    case constants::exception_code::negative_acknowledge:
      throw ex::negative_acknowledge(function, request_header);
    case constants::exception_code::memory_parity_error:
      throw ex::memory_parity_error(function, request_header);
    case constants::exception_code::gateway_path_unavailable:
      throw ex::gateway_path_unavailable(function, request_header);
    case constants::exception_code::gateway_target_device_failed_to_respond:
      throw ex::gateway_target_device_failed_to_respond(function,
                                                        request_header);
    default:
      throw ex::bad_exception();
  }
}
}  // namespace modbus

#endif // LIB_MODBUS_MODBUS_EXCEPTION_HPP_


