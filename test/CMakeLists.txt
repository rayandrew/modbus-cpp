cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(modbuscpp_tests LANGUAGES CXX)

# ---- Options ----

option(ENABLE_TEST_COVERAGE "Enable test coverage" OFF)
option(TEST_INSTALLED_VERSION "Test the version found by find_package" OFF)

# --- Import tools ----

include(../cmake/tools.cmake)

# ---- Dependencies ----

include(../cmake/CPM.cmake)

include(../cmake/Asio2.cmake)

CPMAddPackage(
  NAME doctest
  GITHUB_REPOSITORY onqtam/doctest
  GIT_TAG 2.4.0
)

CPMAddPackage(
  NAME fmt
  GITHUB_REPOSITORY fmtlib/fmt
  GIT_TAG 6.2.1
)

CPMAddPackage(
  NAME struc
  GITHUB_REPOSITORY rayandrews/struc
  VERSION 1
  GIT_TAG 201b4f5ec19139136418e3cb6a263a2f404c7510
)

if(TEST_INSTALLED_VERSION)
  find_package(modbuscpp REQUIRED)
else()
  CPMAddPackage(NAME modbuscpp SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/..)
endif()

CPMAddPackage(
  NAME Format.cmake
  GITHUB_REPOSITORY TheLartians/Format.cmake
  VERSION 1.6
  OPTIONS # enable cmake formatting
          "FORMAT_CHECK_CMAKE ON"
)

# ---- Create binary ----

file(GLOB sources CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp)
add_executable(modbuscpp_tests ${sources})
target_link_libraries(modbuscpp_tests doctest modbuscpp asio2 fmt struc)

set_target_properties(modbuscpp_tests PROPERTIES CXX_STANDARD 17)

# enable compiler warnings
if(NOT TEST_INSTALLED_VERSION)
  if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_compile_options(modbuscpp PUBLIC -Wall -pedantic -Wextra) # -Werror)
  elseif(MSVC)
    target_compile_options(modbuscpp PUBLIC /W4 /WX)
    target_compile_definitions(modbuscpp_tests PUBLIC DOCTEST_CONFIG_USE_STD_HEADERS)
  endif()
endif()

# ---- Add modbus-cpp_tests ----

enable_testing()

# Note: doctest and similar testing frameworks can automatically configure CMake tests For other
# testing frameworks add the tests target instead: ADD_TEST(modbuscpp_tests modbuscpp_tests)

include(${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake)
doctest_discover_tests(modbuscpp_tests)

# ---- code coverage ----

if(ENABLE_TEST_COVERAGE)
  # Coverage works for GCC compiler
  # TODO: set multiple environment
  # Clang: https://clang.llvm.org/docs/SourceBasedCodeCoverage.html
  # lcov -c -i -b . -d . -o cov.info --no-external --exclude "*/test/*"           -> initial / before test
  # lcov -c -b . -d . -o cov.info --no-external --exclude "*/test/*"              -> after test
  # lcov --add-tracefile cov-base.info --add-tracefile cov.info -o cov-total.info -> combine
  ## no need: lcov -r cov.info "*/test/*" -o cov.info
  if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    target_compile_options(modbuscpp PUBLIC -O0 -g -fprofile-arcs -ftest-coverage)
    target_link_options(modbuscpp PUBLIC -fprofile-arcs -ftest-coverage)
  elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    target_compile_options(modbuscpp PUBLIC -O0 -g -fprofile-instr-generate -fcoverage-mapping)
    target_link_options(modbuscpp PUBLIC -fprofile-instr-generate -fcoverage-mapping)
  endif()
endif()
