project(enum)

add_executable(e_parser parser.cpp)
set_target_properties(e_parser PROPERTIES OUTPUT_NAME parser)
target_include_directories(e_parser PRIVATE ${DYNINST_INCLUDE_DIR} ${Boost_INCLUDE_DIR} ${TBB_LIBRARY_DIRS})
target_link_libraries(e_parser PRIVATE symtabAPI ${TBB_LIBRARIES})

add_library(enum SHARED mutatee.cpp)
target_compile_options(enum PRIVATE -g)
