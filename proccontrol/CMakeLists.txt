project(proccontrol)

add_executable(proccontrol_insertion_operators insertion_operators.cpp)
target_include_directories(proccontrol_insertion_operators PRIVATE ${DYNINST_INCLUDE_DIR} ${Boost_INCLUDE_DIR} ${TBB_INCLUDE_DIRS})
target_link_libraries(proccontrol_insertion_operators PRIVATE pcontrol)

add_executable(proccontrol_includes includes.cpp)
target_include_directories(proccontrol_includes PRIVATE ${DYNINST_INCLUDE_DIR} ${Boost_INCLUDE_DIR} ${TBB_INCLUDE_DIRS})
target_link_libraries(proccontrol_includes PRIVATE pcontrol)