project(dwarf)

add_executable(dwarf_includes includes.cpp)
target_link_libraries(dwarf_includes PRIVATE Dyninst::dynDwarf)
add_test(dwarf_includes dwarf_includes)