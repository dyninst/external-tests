project(generic)

# "christmas tree" test Like the "christmas tree" packet attack, this turns on
# all the includes and library links
add_executable(xmas_tree xmas_tree.cpp)
target_link_libraries(xmas_tree
  PUBLIC Dyninst::common
         Dyninst::dynC_API
         Dyninst::dynDwarf
         Dyninst::dynElf
         Dyninst::dyninstAPI
         Dyninst::instructionAPI
         Dyninst::parseAPI
         Dyninst::patchAPI
         Dyninst::pcontrol
         Dyninst::stackwalk
         Dyninst::symLite
         Dyninst::symtabAPI)

add_test(xmas_tree xmas_tree)
