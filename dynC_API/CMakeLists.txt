project(dynC_API)

add_executable(dynC_API_includes includes.cpp)
target_link_libraries(dynC_API_includes PRIVATE Dyninst::dynC_API)
add_test(NAME dynC_API_includes COMMAND dynC_API_includes)
