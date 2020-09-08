#ifndef LIB_MODBUS_MODBUS_RESPONSE_HPP_
#define LIB_MODBUS_MODBUS_RESPONSE_HPP_

#include <string_view>
#include <type_traits>

#include <boost/core/noncopyable.hpp>

#include "adu.hpp"
#include "constants.hpp"
#include "exception.hpp"
#include "types.hpp"

namespace modbus {
// forward declarations
namespace ex {
class base_error;
}
class table;
namespace internal {
class request;
class response;
}  // namespace internal
/*namespace response {*/
// class error;
//}

namespace internal {
enum class stage {
  bad,
  error,
  passed,
};

class response : public adu {
public:
  /**
   * Pointer type
   */
  typedef std::unique_ptr<response> pointer;

  /**
   * Initializer
   */
  using typename adu::initializer_t;

  /**
   * Decode string_view
   */
  using adu::decode;

  /**
   * Response destructor
   */
  virtual ~response();

  /**
   * Decode response
   *
   * @param packet packet to decode
   */
  virtual void decode(const packet_t& packet) override;

protected:
  /**
   * Response constructor
   */
  explicit response() noexcept;

  /**
   * Response constructor
   *
   * @param function   modbus function
   * @param req_header request header
   * @param data_table data table pointer
   */
  explicit response(constants::function_code function,
                    const header_t&          req_header,
                    table*                   data_table = nullptr) noexcept;

  /**
   * Parse stage passed packet
   *
   * BEWARE:
   * only parse the right (no exception) packet
   *
   * @param packet packet to parse
   */
  virtual void decode_passed(const packet_t& packet) = 0;

  /**
   * Check stage
   *
   * Will process these steps:
   * - Checking packet size
   * - Decode packet and write header metadata
   * - Check function
   * - Check function diff with 0x80
   *
   * Will result in 3 phases as defined in stage enum
   *
   * @param packet packet to check
   *
   * @return exception stage
   */
  stage check_stage(const packet_t& packet);

  /**
   * Initial packet check
   *
   * Check :
   * Minimum size must be > header_length (7 bytes)
   *
   * @param packet packet to check
   *
   * @return test passed or not
   */
  static bool initial_check(const packet_t& packet);

  /**
   * Data table getter
   *
   * @return data table pointer
   */
  inline table* data_table() { return data_table_; }

  /**
   * Data table getter
   *
   * @return data table pointer
   */
  inline const table* data_table() const {
    return const_cast<table*>(data_table_);
  }

protected:
  /**
   * Request header
   */
  header_t req_header_;
  /**
   * Data table pointer
   */
  table* data_table_;
};
}  // namespace internal

namespace response {
class error : private boost::noncopyable, public internal::response {
public:
  /**
   * Create std::unique_ptr of response::error
   *
   * @return std::unique_ptr of response::error
   */
  MAKE_STD_UNIQUE(error)

  /**
   * Modbus exception response
   */
  explicit error() noexcept;

  /**
   * Modbus exception response
   *
   * @param exc     exception
   */
  template <typename T,
            typename
            = std::enable_if<std::is_base_of_v<ex::specification_error, T>>>
  inline explicit error(const T& ec) noexcept
      : internal::response{ec.function(), ec.header()}, ec_{ec.code()} {
    initialize({ec.header().transaction, ec.header().unit});
  }

  /**
   * Decode stage passed packet
   */
  inline virtual void decode_passed(const packet_t&) override {}

  /**
   * Encode response::read_coils packet
   *
   * @return packet format
   */
  virtual packet_t encode() override;

  /**
   * Decode response::read_coils packet
   *
   * @param packet packet to be appended
   *
   * @return packet format
   */
  virtual void decode(const packet_t& packet) override;

public:
  /**
   * Length of packet
   */
  static constexpr typename packet_t::size_type packet_size = header_length + 2;

private:
  /**
   * Struct format
   */
  static constexpr std::string_view format = "BB";
  /**
   * Exception code
   */
  constants::exception_code ec_;
};
}  // namespace response
}  // namespace modbus

#endif  // LIB_MODBUS_MODBUS_RESPONSE_HPP_
