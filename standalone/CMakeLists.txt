cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(modbuscpp_standalone LANGUAGES CXX)

# --- Import tools ----

include(../cmake/tools.cmake)

# ---- Dependencies ----

include(../cmake/CPM.cmake)
include(../cmake/Asio2.cmake)

CPMAddPackage(
  NAME fmt
  GITHUB_REPOSITORY fmtlib/fmt
  GIT_TAG 6.2.1
)

CPMAddPackage(
  NAME spdlog
  GITHUB_REPOSITORY gabime/spdlog
  VERSION 1.6.1
  OPTIONS "SPDLOG_FMT_EXTERNAL_HO ON"
)

CPMAddPackage(
  NAME struc
  GITHUB_REPOSITORY rayandrews/struc
  VERSION 1
  GIT_TAG 3e1b2521f104e7e785a8f915efe63d63fb11ed51
)

CPMAddPackage(NAME modbuscpp SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/..)

# ---- Create standalone executable ----

file(GLOB sources CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp)

foreach(source ${sources})
  get_filename_component(source_file ${source} NAME)
  string(REPLACE ".cpp" "" executable_name ${source_file})

  add_executable(${executable_name} ${source})
  set_target_properties(
    ${executable_name} PROPERTIES CXX_STANDARD 17 OUTPUT_NAME ${executable_name}
  )
  target_link_libraries(${executable_name} modbuscpp asio2 struc fmt spdlog)
endforeach()
