project(symtabAPI)

add_executable(symtab_parse_function_params parse_function_params.cpp)
target_link_libraries(symtab_parse_function_params PRIVATE Dyninst::symtabAPI)

add_executable(st_insertion_operators insertion_operators.cpp)
target_link_libraries(st_insertion_operators PRIVATE Dyninst::symtabAPI)

add_subdirectory(enum)
add_subdirectory(module)
add_subdirectory(DWARF)

dyninst_add_test(symtab_includes SOURCE_FILES includes.cpp DEPS Dyninst::symtabAPI)
