add_library(Engine Engine.cpp)
add_library(restpp SHARED restpp.cc)
target_link_libraries(restpp
        Engine
        ${Boost_LIBRARIES}
        glog
        )