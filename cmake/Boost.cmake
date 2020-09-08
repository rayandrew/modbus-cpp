# only activate tools for top level project
if(NOT PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  return()
endif()

option(BOOST_STATIC "Boost static library" OFF)

set(Boost_USE_STATIC_LIBS ${BOOST_STATIC}) # only find static libs
set(Boost_USE_DEBUG_LIBS OFF) # ignore debug libs and
set(Boost_USE_RELEASE_LIBS ON) # only find release libs
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.70.0 REQUIRED COMPONENTS system)
