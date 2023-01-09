project(dyninstAPI)

foreach(linkage "" "_static")
  add_executable(dyninstAPI_insertion_operators${linkage}
                 insertion_operators.cpp)
  target_link_libraries(dyninstAPI_insertion_operators${linkage}
                        PRIVATE Dyninst::dyninstAPI${linkage})

  add_executable(dyninstAPI_includes${linkage} includes.cpp)
  target_link_libraries(dyninstAPI_includes${linkage}
                        PRIVATE Dyninst::dyninstAPI${linkage})
endforeach()
