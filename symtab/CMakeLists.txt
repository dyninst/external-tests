project(symtabAPI)

add_executable(parse_function_params parse_function_params.cpp)
target_include_directories(parse_function_params PRIVATE ${DYNINST_INCLUDE_DIR} ${Boost_INCLUDE_DIR})
target_link_libraries(parse_function_params PRIVATE symtabAPI)
