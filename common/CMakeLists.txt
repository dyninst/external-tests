project(common)

add_executable(common_insertion_operators insertion_operators.cpp)
target_include_directories(common_insertion_operators PRIVATE ${DYNINST_INCLUDE_DIR} ${Boost_INCLUDE_DIR} ${TBB_INCLUDE_DIRS})
target_link_libraries(common_insertion_operators PRIVATE common)

add_executable(common_includes includes.cpp)
target_include_directories(common_includes PRIVATE ${DYNINST_INCLUDE_DIR} ${Boost_INCLUDE_DIR} ${TBB_INCLUDE_DIRS})
target_link_libraries(common_includes PRIVATE common)
add_test(common_includes common_includes)