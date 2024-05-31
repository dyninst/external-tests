project(common_registers)

add_executable(uniqueness uniqueness.cpp)
target_link_libraries(uniqueness PRIVATE Dyninst::symtabAPI)
add_test(NAME common_registers_uniqueness COMMAND uniqueness)
