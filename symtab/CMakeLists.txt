project(symtabAPI)

add_executable(symtab_parse_function_params parse_function_params.cpp)
target_include_directories(symtab_parse_function_params PRIVATE ${DYNINST_INCLUDE_DIR} ${Boost_INCLUDE_DIR})
target_link_libraries(symtab_parse_function_params PRIVATE symtabAPI common)


add_executable(st_insertion_operators insertion_operators.cpp)
target_include_directories(st_insertion_operators PRIVATE ${DYNINST_INCLUDE_DIR} ${Boost_INCLUDE_DIR})
target_link_libraries(st_insertion_operators PRIVATE symtabAPI common)

add_subdirectory(enum)

add_executable(symtab_includes includes.cpp)
target_include_directories(symtab_includes PRIVATE ${DYNINST_INCLUDE_DIR} ${Boost_INCLUDE_DIR} ${TBB_INCLUDE_DIRS})
target_link_libraries(symtab_includes PRIVATE symtabAPI)
add_test(symtab_includes symtab_includes)