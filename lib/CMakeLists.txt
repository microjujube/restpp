add_library(restpp SHARED restpp.cc
        Engine.cpp RequestHandler.cpp Utils.cpp Context.cpp
        )
target_link_libraries(restpp PRIVATE
        glog::glog
        ${Boost_LIBRARIES}
        )


install(TARGETS restpp
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)