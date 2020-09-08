#ifndef LIB_MODBUS_MODBUS_DATA_TABLE_INLINE_HPP_
#define LIB_MODBUS_MODBUS_DATA_TABLE_INLINE_HPP_

#include <algorithm>
#include <iostream>
#include <iterator>
#include <thread>

#include "data-table.hpp"
#include "exception.hpp"
#include "logger.hpp"

namespace modbus {
namespace block {
/** base block */
template <template <class...> class base_container_t,
          typename data_t,
          typename read_count_t,
          typename write_count_t>
inline base<base_container_t, data_t, read_count_t, write_count_t>::base(
    const address_t& starting_address,
    size_type        capacity,
    data_t           default_value) noexcept
    : starting_address_{std::move(starting_address)},
      container_(capacity, default_value),
      capacity_{capacity},
      default_value_{default_value} {}

template <template <class...> class base_container_t,
          typename data_t,
          typename read_count_t,
          typename write_count_t>
inline base<base_container_t, data_t, read_count_t, write_count_t>::base(
    const address_t&      starting_address,
    const container_type& container) noexcept
    : starting_address_{std::move(starting_address)},
      container_(std::move(container)),
      capacity_{container.size()},
      default_value_{0} {}

template <template <class...> class base_container_t,
          typename data_t,
          typename read_count_t,
          typename write_count_t>
inline bool
base<base_container_t, data_t, read_count_t, write_count_t>::validate(
    const address_t& address) const {
  return validate_sz(address, 1);
}

template <template <class...> class base_container_t,
          typename data_t,
          typename read_count_t,
          typename write_count_t>
inline bool
base<base_container_t, data_t, read_count_t, write_count_t>::validate(
    const address_t&    address,
    const read_count_t& count) const {
  return read_count_t::validate(count()) && validate_sz(address, count());
}

template <template <class...> class base_container_t,
          typename data_t,
          typename read_count_t,
          typename write_count_t>
inline bool
base<base_container_t, data_t, read_count_t, write_count_t>::validate(
    const address_t&     address,
    const write_count_t& count) const {
  return write_count_t::validate(count()) && validate_sz(address, count());
}

template <template <class...> class base_container_t,
          typename data_t,
          typename read_count_t,
          typename write_count_t>
inline bool
base<base_container_t, data_t, read_count_t, write_count_t>::validate_sz(
    const address_t& address,
    size_type        count) const {
  if (count > 0) {
    return (starting_address() <= address)
           && ((starting_address_() + capacity()) >= (address() + count));
  }

  throw ex::out_of_range("Count is not valid");
}

/** sequential block */
template <typename data_t, typename read_count_t, typename write_count_t>
inline sequential<data_t, read_count_t, write_count_t>::sequential(
    const address_t& starting_address,
    size_type        capacity,
    data_t           default_value) noexcept
    : base<std::vector, data_t, read_count_t, write_count_t>{
        starting_address, capacity, default_value} {
  container().resize(capacity);
}

template <typename data_t, typename read_count_t, typename write_count_t>
inline sequential<data_t, read_count_t, write_count_t>::sequential(
    const sequential<data_t, read_count_t, write_count_t>::initializer_t&
        initializer) noexcept
    : base<std::vector, data_t, read_count_t, write_count_t>{
        initializer.starting_address, initializer.capacity,
        initializer.default_value} {
  container().resize(capacity());
}

template <typename data_t, typename read_count_t, typename write_count_t>
inline sequential<data_t, read_count_t, write_count_t>::sequential(
    const address_t&      starting_address,
    const container_type& container) noexcept
    : base<std::vector, data_t, read_count_t, write_count_t>{starting_address,
                                                             container} {}

template <typename data_t, typename read_count_t, typename write_count_t>
inline typename sequential<data_t, read_count_t, write_count_t>::data_reference
sequential<data_t, read_count_t, write_count_t>::ref(const address_t& address) {
  if (!validate(address)) {
    throw ex::out_of_range("Address is not valid");
  }

  address_t idx = address - starting_address();
  return container_[idx()];
}

template <typename data_t, typename read_count_t, typename write_count_t> inline
    typename sequential<data_t, read_count_t, write_count_t>::mutable_slice_type
    sequential<data_t, read_count_t, write_count_t>::ref(
        const address_t& address,
        size_type        count) {
  if (!validate_sz(address, count)) {
    throw ex::out_of_range("Address and count are not valid");
  }

  address_t idx = address - starting_address();
  return {container().begin() + idx(), container().begin() + idx() + count};
}

template <typename data_t, typename read_count_t, typename write_count_t> inline
    typename sequential<data_t, read_count_t, write_count_t>::slice_type
    sequential<data_t, read_count_t, write_count_t>::get(
        const address_t&    address,
        const read_count_t& count) const {
  std::lock_guard<std::shared_mutex> lock(mutex_);
  address_t                          idx = address - starting_address();
  if (!validate(idx, count)) {
    throw ex::out_of_range("Address and count are not valid");
  }

  return {container().cbegin() + idx(), container().cbegin() + idx() + count()};
}

template <typename data_t, typename read_count_t, typename write_count_t> inline
    typename sequential<data_t, read_count_t, write_count_t>::
        const_data_reference
        sequential<data_t, read_count_t, write_count_t>::get(
            const address_t& address) const {
  std::lock_guard<std::shared_mutex> lock(mutex_);
  if (!validate(address)) {
    throw ex::out_of_range("Address is not valid");
  }

  address_t idx = address - starting_address();
  return container_[idx()];
}

template <typename data_t, typename read_count_t, typename write_count_t>
inline void sequential<data_t, read_count_t, write_count_t>::set(
    const address_t&      address,
    const container_type& buffer) {
  std::lock_guard<std::shared_mutex> lock(mutex_);
  if (!validate_sz(address, buffer.size())) {
    throw ex::out_of_range("Starting address is not valid");
  }

  address_t idx = address - starting_address();
  std::transform(buffer.begin(), buffer.end(), container().begin() + idx(),
                 [](const auto& data) -> data_t { return data; });
}

template <typename data_t, typename read_count_t, typename write_count_t>
inline void sequential<data_t, read_count_t, write_count_t>::set(
    const address_t& address,
    data_t           value) {
  std::lock_guard<std::shared_mutex> lock(mutex_);
  if (!validate(address)) {
    throw ex::out_of_range("Starting address is not valid");
  }

  address_t idx = address - starting_address();
  container_[idx()] = value;
}

template <typename data_t, typename read_count_t, typename write_count_t>
inline void sequential<data_t, read_count_t, write_count_t>::reset() {
  std::lock_guard<std::shared_mutex> lock(mutex_);
  std::fill(container().begin(), container().end(), default_value());
}
}  // namespace block
}  // namespace modbus

#endif // LIB_MODBUS_MODBUS_DATA_TABLE_INLINE_HPP_


