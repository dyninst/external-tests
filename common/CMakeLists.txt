project(common)

add_executable(common_insertion_operators insertion_operators.cpp)
target_link_libraries(common_insertion_operators PRIVATE Dyninst::common)

dyninst_add_test(common_includes SOURCE_FILES includes.cpp DEPS Dyninst::common)
