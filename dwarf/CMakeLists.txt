project(dwarf)

add_executable(dwarf_includes includes.cpp)
target_link_libraries(dwarf_includes PRIVATE Dyninst::dynDwarf)
add_test(NAME dwarf_includes COMMAND dwarf_includes)
