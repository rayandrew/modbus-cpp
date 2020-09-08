#ifndef LIB_MODBUS_MODBUS_HPP_
#define LIB_MODBUS_MODBUS_HPP_

/** @file modbus.hpp
 *  @brief Modbus header file
 */

#pragma GCC system_header

#if defined(WIN32) || defined(_WIN32) \
    || defined(__WIN32) && !defined(__CYGWIN__)
#  undef exception_code
#endif

#include "details/asio2.hpp"
#include "details/struct.hpp"

#include "details/constants.hpp"
#include "details/types.hpp"

#include "details/exception.hpp"

#include "details/utilities.hpp"

#include "details/logger.hpp"

#include "details/data-table.hpp"
#include "details/data-table.inline.hpp"

#include "details/operation.hpp"

#include "details/adu.hpp"
#include "details/request.hpp"
#include "details/response.hpp"

// Functions implementation
#include "details/bit-read.hpp"
#include "details/bit-read.inline.hpp"

#include "details/bit-write.hpp"

#include "details/register-read.hpp"
#include "details/register-read.inline.hpp"

#include "details/register-write.hpp"

#include "details/request-handler.hpp"

#include "details/server.hpp"

#endif  // LIB_MODBUS_MODBUS_HPP_
