project(dynC_API)

foreach(linkage "" "_static")
  add_executable(dynC_API_includes${linkage} includes.cpp)
  target_link_libraries(dynC_API_includes${linkage}
                        PRIVATE Dyninst::dynC_API${linkage})
  add_test(dynC_API_includes${linkage} dynC_API_includes${linkage})
endforeach()
