project(elf)

add_executable(dynelf_includes includes.cpp)
target_include_directories(dynelf_includes PRIVATE ${DYNINST_INCLUDE_DIR} ${Boost_INCLUDE_DIR} ${TBB_INCLUDE_DIRS})
target_link_libraries(dynelf_includes PRIVATE common dynElf)
add_test(dynelf_includes dynelf_includes)