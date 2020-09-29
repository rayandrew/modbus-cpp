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

#include "modbuscpp/asio2.hpp"
#include "modbuscpp/struct.hpp"

#include "modbuscpp/constants.hpp"
#include "modbuscpp/types.hpp"

#include "modbuscpp/exception.hpp"

#include "modbuscpp/utilities.hpp"

#include "modbuscpp/logger.hpp"

#include "modbuscpp/data-table.hpp"
#include "modbuscpp/data-table.inline.hpp"

#include "modbuscpp/operation.hpp"

#include "modbuscpp/adu.hpp"
#include "modbuscpp/request.hpp"
#include "modbuscpp/response.hpp"

// Functions implementation
#include "modbuscpp/bit-read.hpp"
#include "modbuscpp/bit-read.inline.hpp"

#include "modbuscpp/bit-write.hpp"

#include "modbuscpp/register-read.hpp"
#include "modbuscpp/register-read.inline.hpp"

#include "modbuscpp/register-write.hpp"

#include "modbuscpp/request-handler.hpp"

#include "modbuscpp/server.hpp"

#endif  // LIB_MODBUS_MODBUS_HPP_
