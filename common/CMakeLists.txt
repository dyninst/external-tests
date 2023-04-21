project(common)

add_executable(common_insertion_operators insertion_operators.cpp)
target_link_libraries(common_insertion_operators PRIVATE Dyninst::common)

add_executable(common_includes includes.cpp)
target_link_libraries(common_includes PRIVATE Dyninst::common)
add_test(common_includes common_includes)
