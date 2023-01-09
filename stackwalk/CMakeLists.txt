project(stackwalk)

foreach(linkage "" "_static")
  add_executable(stackwalk_includes${linkage} includes.cpp)
  target_link_libraries(stackwalk_includes${linkage}
                        PRIVATE Dyninst::stackwalk${linkage})
endforeach()
