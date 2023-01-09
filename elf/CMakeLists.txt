project(elf)

foreach(linkage "" "_static")
  add_executable(dynelf_includes${linkage} includes.cpp)
  target_link_libraries(dynelf_includes${linkage}
                        PRIVATE Dyninst::dynElf${linkage})
endforeach()
