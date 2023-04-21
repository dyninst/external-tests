project(generic)

#include(ElfUtils)

# "christmas tree" test Like the "christmas tree" packet attack, this turns on
# all the includes and library links
add_executable(g_xmas_tree xmas_tree.cpp)
target_link_libraries(
  g_xmas_tree
  PUBLIC Dyninst::dynC_API Dyninst::dyninstAPI Dyninst::stackwalk)
