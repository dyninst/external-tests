project(common)

foreach(linkage "" "_static")
  add_executable(common_insertion_operators${linkage} insertion_operators.cpp)
  target_link_libraries(common_insertion_operators${linkage}
                        PRIVATE Dyninst::common${linkage})
  add_test(common_insertion_operators${linkage}
           common_insertion_operators${linkage})

  add_executable(common_includes${linkage} includes.cpp)
  target_link_libraries(common_includes${linkage}
                        PRIVATE Dyninst::common${linkage})
endforeach()
