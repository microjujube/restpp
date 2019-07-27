add_executable(Server Server.cpp)
target_link_libraries(Server restpp glog::glog)

#add_custom_command(TARGET Server PRE_BUILD
#        COMMAND ${CMAKE_COMMAND} -E remove_directory
#        $<TARGET_FILE_DIR:Server>)
#
#add_custom_command(TARGET Server POST_BUILD
#        COMMAND ${CMAKE_COMMAND} -E copy_directory
#        $<TARGET_FILE_DIR:restpp>
#        $<TARGET_FILE_DIR:Server>)