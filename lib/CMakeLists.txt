add_library(Engine Engine.cpp )
add_library(Request Request.cpp)

add_library(restpp SHARED restpp.cc)
target_link_libraries(restpp
        Request
        Engine
        ${Boost_LIBRARIES}
        glog::glog
        )


install(TARGETS restpp
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)