project(enum)

dyninst_add_test(e_parser SOURCE_FILES parser.cpp DEPS Dyninst::symtabAPI)

add_library(enum SHARED mutatee.cpp)
target_compile_options(enum PRIVATE -g)
