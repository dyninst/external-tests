project(enum)

add_executable(e_parser parser.cpp)
target_link_libraries(e_parser PRIVATE Dyninst::symtabAPI)
add_test(NAME e_parser COMMAND e_parser)

add_library(enum SHARED mutatee.cpp)
target_compile_options(enum PRIVATE -g)
