project(dwarf)

add_executable(dwarf_includes includes.cpp)
target_include_directories(dwarf_includes PRIVATE ${DYNINST_INCLUDE_DIR} ${Boost_INCLUDE_DIR} ${TBB_INCLUDE_DIRS})
target_link_libraries(dwarf_includes PRIVATE common dynDwarf)
add_test(dwarf_includes dwarf_includes)
