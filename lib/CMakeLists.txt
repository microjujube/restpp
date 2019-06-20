#add_library(Engine Engine.cpp )
#add_library(Request Request.cpp)
#add_library(Response Response.cpp)

add_library(restpp SHARED restpp.cc
        Engine.cpp Response.cpp Response.cpp RequestHandler.cpp
        )
target_link_libraries(restpp
        ${Boost_LIBRARIES}
        glog::glog
        )


install(TARGETS restpp
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)