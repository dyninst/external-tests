project(symtabAPI)

foreach(linkage "" "_static")
  add_executable(symtab_parse_function_params${linkage}
                 parse_function_params.cpp)
  target_link_libraries(symtab_parse_function_params${linkage}
                        PRIVATE Dyninst::symtabAPI${linkage})

  add_executable(symtab_insertion_operators${linkage} insertion_operators.cpp)
  target_link_libraries(symtab_insertion_operators${linkage}
                        PRIVATE Dyninst::symtabAPI${linkage})

  add_executable(symtab_includes${linkage} includes.cpp)
  target_link_libraries(symtab_includes${linkage}
                        PRIVATE Dyninst::symtabAPI${linkage})
endforeach()
