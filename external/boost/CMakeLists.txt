set(BOOST_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(BOOST_INCLUDE_DIR "${BOOST_ROOT}/include")
set(BOOST_LIBRARY_DIR "${BOOST_ROOT}/lib")

find_package(Boost 1.88 REQUIRED COMPONENTS filesystem system)

if(Boost_FOUND)
    message(STATUS "Boost found!")
else()
    message(FATAL_ERROR "Boost not found")
endif()
