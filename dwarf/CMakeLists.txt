project(dwarf)

foreach(linkage "" "_static")
  add_executable(dwarf_includes${linkage} includes.cpp)
  target_link_libraries(dwarf_includes${linkage}
                        PRIVATE Dyninst::dynDwarf${linkage})
  add_test(dwarf_includes${linkage} dwarf_includes${linkage})
endforeach()
