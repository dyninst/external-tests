project(dynC_API)

add_executable(dynC_API_includes includes.cpp)
target_include_directories(dynC_API_includes PRIVATE ${DYNINST_INCLUDE_DIR} ${Boost_INCLUDE_DIR} ${TBB_INCLUDE_DIRS})
target_link_libraries(dynC_API_includes PRIVATE dyninstAPI symtabAPI instructionAPI parseAPI)
add_test(dynC_API_includes dynC_API_includes)
