# only activate tools for top level project
if(NOT PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  return()
endif()

include(${CMAKE_CURRENT_LIST_DIR}/CPM.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/Boost.cmake)

CPMAddPackage(
  NAME asio2
  GITHUB_REPOSITORY zhllxt/asio2
  VERSION 1
  GIT_TAG ce6ac7f6ba2a931a19b9abb7627d12cfaeed8ab1)

if(asio2_ADDED)
  add_library(asio2 INTERFACE)

  target_include_directories(asio2
    INTERFACE
      $<BUILD_INTERFACE:${Boost_INCLUDE_DIR}>
      $<BUILD_INTERFACE:${asio2_SOURCE_DIR}>)
endif()
