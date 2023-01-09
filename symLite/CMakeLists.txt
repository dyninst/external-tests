project(symLite)

foreach(linkage "" "_static")
  add_executable(symLite_includes${linkage} includes.cpp)
  target_link_libraries(symLite_includes${linkage}
                        PRIVATE Dyninst::symLite${linkage})
endforeach()
