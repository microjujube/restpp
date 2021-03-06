cmake_minimum_required(VERSION 3.12)

project(restpp)

set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_SOURCE_DIR}/include)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -O3 -pthread")
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

#BOOST
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
set(BOOST_ALL_DYN_LINK ON)
SET(BOOST_REQUIRED_COMPONENTS
        date_time
        filesystem
        program_options
        regex
        system
        timer
        chrono
        thread
#        unit_test_framework
        )

find_package(Boost COMPONENTS ${BOOST_REQUIRED_COMPONENTS} REQUIRED)
message(STATUS "** Boost Include: ${Boost_INCLUDE_DIR}")
message(STATUS "** Boost Libraries: ${Boost_LIBRARY_DIRS}")
message(STATUS "** Boost Libraries: ${Boost_LIBRARIES}")
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})

find_package(glog CONFIG REQUIRED)

find_package(nlohmann_json CONFIG REQUIRED)

add_subdirectory(include)
add_subdirectory(lib)
add_subdirectory(src)
