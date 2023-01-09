project(generic)

# "christmas tree" test Like the "christmas tree" packet attack, this turns on
# all the includes and library links

foreach(linkage "" "_static")
  add_executable(xmas_tree${linkage} xmas_tree.cpp)
  target_link_libraries(
    xmas_tree${linkage}
    PUBLIC Dyninst::common${linkage}
           Dyninst::dynC_API${linkage}
           Dyninst::dynDwarf${linkage}
           Dyninst::dynElf${linkage}
           Dyninst::dyninstAPI${linkage}
           Dyninst::instructionAPI${linkage}
           Dyninst::parseAPI${linkage}
           Dyninst::patchAPI${linkage}
           Dyninst::pcontrol${linkage}
           Dyninst::stackwalk${linkage}
           Dyninst::symtabAPI${linkage})

  add_test(xmas_tree${linkage} xmas_tree${linkage})
endforeach()
