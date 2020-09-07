# this file contains a list of tools that can be activated and downloaded on-demand each tool is
# enabled during configuration by passing an additional `-DUSE_<TOOL>=<VALUE>` argument to CMake

# only activate tools for top level project
if(NOT PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  return()
endif()

include(${CMAKE_CURRENT_LIST_DIR}/CPM.cmake)

set(Boost_USE_STATIC_LIBS        ON)  # only find static libs
set(Boost_USE_DEBUG_LIBS         OFF) # ignore debug libs and
set(Boost_USE_RELEASE_LIBS       ON)  # only find release libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME     OFF)
find_package(Boost 1.70.0 REQUIRED COMPONENTS
  system)

if(NOT Boost_FOUND)
  CPMAddPackage(
    NAME boost-cmake
    GITHUB_REPOSITORY Orphis/boost-cmake
    VERSION 1.70.0
    GIT_TAG 70b12f62da331dd402b78102ec8f6a15d59a7af9)

  set(Boost_LIBRARIES "Boost::system")
endif()


