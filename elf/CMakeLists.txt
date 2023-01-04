project(elf)

add_executable(dynelf_includes includes.cpp)
target_link_libraries(dynelf_includes PRIVATE Dyninst::dynElf)
