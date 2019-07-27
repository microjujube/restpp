add_definitions(-DRESTPP_DLL_EXPORTS)

add_library(restpp SHARED restpp.cc
        Engine.cpp RequestHandler.cpp Utils.cpp Context.cpp
        )
target_link_libraries(restpp PRIVATE
        glog::glog
        ${Boost_LIBRARIES}
        )


SET(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/install)
target_include_directories(restpp PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)

install(TARGETS restpp
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

message(STATUS "current source dir: ${CMAKE_CURRENT_SOURCE_DIR}")
install(TARGETS restpp
        EXPORT restpp-targets
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)
install(EXPORT restpp-targets
        NAMESPACE restpp::
        FILE restpp-config.cmake
        DESTINATION lib/cmake/restpp)


