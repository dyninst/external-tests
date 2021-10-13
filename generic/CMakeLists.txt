project(generic)

#include(ElfUtils)

# "christmas tree" test Like the "christmas tree" packet attack, this turns on
# all the includes and library links
add_executable(g_xmas_tree xmas_tree.cpp)
set_target_properties(g_xmas_tree PROPERTIES OUTPUT_NAME xmas_tree)
target_include_directories(
  g_xmas_tree PRIVATE ${DYNINST_INCLUDE_DIR} ${Boost_INCLUDE_DIR}
                      ${ElfUtils_INCLUDE_DIRS})
target_link_libraries(
  g_xmas_tree
  PUBLIC common
         dynC_API
         dynDwarf
         dynElf
         dyninstAPI_RT
         dyninstAPI
         instructionAPI
         parseAPI
         patchAPI
         pcontrol
         stackwalk
         symLite
         symtabAPI)
