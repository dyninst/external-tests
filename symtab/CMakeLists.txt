project(symtabAPI)

add_executable(symtab_parse_function_params parse_function_params.cpp)
target_link_libraries(symtab_parse_function_params PRIVATE Dyninst::symtabAPI)


add_executable(st_insertion_operators insertion_operators.cpp)
target_link_libraries(st_insertion_operators PRIVATE Dyninst::symtabAPI)

add_subdirectory(enum)

add_executable(symtab_includes includes.cpp)
target_link_libraries(symtab_includes PRIVATE Dyninst::symtabAPI)
add_test(symtab_includes symtab_includes)