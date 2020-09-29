cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(
  modbuscpp
  VERSION 1.0
  LANGUAGES CXX
)

# ---- Include guards ----
if(${CMAKE_BUILD_TYPE} MATCHES Debug)
  add_definitions(-DDEBUG_ON)
endif()

if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
  message(
    FATAL_ERROR
      "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there."
  )
endif()

# ---- Add dependencies via CPM ----
include(cmake/CPM.cmake)

# Utils
CPMAddPackage(
  NAME PackageProject.cmake
  GITHUB_REPOSITORY TheLartians/PackageProject.cmake
  VERSION 1.3
)

CPMAddPackage(
  NAME GroupSourcesByFolder.cmake
  GITHUB_REPOSITORY TheLartians/GroupSourcesByFolder.cmake
  VERSION 1.0
)

# Threads
find_package(Threads REQUIRED)

# Asio2
include(cmake/Asio2.cmake)

# FMT
CPMAddPackage(
  NAME fmt
  GITHUB_REPOSITORY fmtlib/fmt
  GIT_TAG 6.2.1
)

# Struc
CPMAddPackage(
  NAME struc
  GITHUB_REPOSITORY rayandrews/struc
  VERSION 1
  GIT_TAG 235db327aeec3a83c9204033e3a7b0a74c866151
)

# Asio2
include(cmake/Asio2.cmake)

# ---- Add source files ----

# Note: globbing sources is considered bad practice as CMake's generators may not detect new files
# automatically. Keep that in mind when changing files, or explicitly mention them here.
set(headers
    ${CMAKE_CURRENT_SOURCE_DIR}/include/modbuscpp/modbus.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/modbuscpp/modbuscpp/asio2.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/modbuscpp/modbuscpp/struct.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/modbuscpp/modbuscpp/data-table.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/modbuscpp/modbuscpp/data-table.inline.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/modbuscpp/modbuscpp/constants.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/modbuscpp/modbuscpp/exception.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/modbuscpp/modbuscpp/logger.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/modbuscpp/modbuscpp/operation.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/modbuscpp/modbuscpp/types.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/modbuscpp/modbuscpp/utilities.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/modbuscpp/modbuscpp/adu.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/modbuscpp/modbuscpp/request.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/modbuscpp/modbuscpp/response.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/modbuscpp/modbuscpp/bit-read.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/modbuscpp/modbuscpp/bit-read.inline.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/modbuscpp/modbuscpp/bit-write.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/modbuscpp/modbuscpp/register-read.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/modbuscpp/modbuscpp/register-read.inline.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/modbuscpp/modbuscpp/register-write.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/modbuscpp/modbuscpp/request-handler.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/modbuscpp/modbuscpp/server.hpp
)

set(sources
    ${CMAKE_CURRENT_SOURCE_DIR}/source/data-table.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/logger.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/operation.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/adu.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/request.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/response.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/bit-read.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/bit-write.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/register-write.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/register-read.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/request-handler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/server.cpp
)

# ---- Create library ----
add_library(modbuscpp ${headers} ${sources})
groupsourcesbyfolder(modbuscpp)

set_target_properties(modbuscpp PROPERTIES CXX_STANDARD 17)

# being a cross-platform target, we enforce standards conformance on MSVC
target_compile_options(modbuscpp PUBLIC "$<$<BOOL:${MSVC}>:/permissive->")

# Link dependencies
target_link_libraries(modbuscpp PRIVATE Threads::Threads ${Boost_LIBRARIES} fmt struc asio2)

target_include_directories(
  modbuscpp PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
                   $<INSTALL_INTERFACE:include/${PROJECT_NAME}-${PROJECT_VERSION}>
)

if(Boost_FOUND)
  target_include_directories(modbuscpp PUBLIC $<BUILD_INTERFACE:${Boost_INCLUDE_DIR}>)
endif()

# ---- Create an installable target ----
# this allows users to install and find the library via `find_package()`.

# the location where the project's version header will be placed should match the project's regular
# header paths
string(TOLOWER ${PROJECT_NAME}/version.hpp VERSION_HEADER_LOCATION)

packageProject(
  NAME fmt
  VERSION 6.2.1
  BINARY_DIR ${PROJECT_BINARY_DIR}
  INCLUDE_DIR ${fmt_SOURCE_DIR}/include
  INCLUDE_DESTINATION include/fmt-6.2.1
)

packageProject(
  NAME struc
  VERSION 1
  BINARY_DIR ${PROJECT_BINARY_DIR}
  INCLUDE_DIR ${struc_SOURCE_DIR}/include
  INCLUDE_DESTINATION include/struc-1.0.0
)

packageProject(
  NAME asio2
  VERSION 1
  BINARY_DIR ${PROJECT_BINARY_DIR}
  INCLUDE_DIR ${asio2_SOURCE_DIR}
  INCLUDE_DESTINATION include/asio2-1.0.0
)

packageProject(
  NAME ${PROJECT_NAME}
  VERSION ${PROJECT_VERSION}
  BINARY_DIR ${PROJECT_BINARY_DIR}
  INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include
  INCLUDE_DESTINATION include/${PROJECT_NAME}-${PROJECT_VERSION}
  VERSION_HEADER "${VERSION_HEADER_LOCATION}"
  DEPENDENCIES "fmt;struc;asio2"
)
