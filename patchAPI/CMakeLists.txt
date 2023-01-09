project(patchAPI)

foreach(linkage "" "_static")
  add_executable(patchAPI_includes${linkage} includes.cpp)
  target_link_libraries(patchAPI_includes${linkage}
                        PRIVATE Dyninst::patchAPI${linkage})
endforeach()
