project(parseAPI)

foreach(linkage "" "_static")
  add_executable(parseAPI_includes${linkage} includes.cpp)
  target_link_libraries(parseAPI_includes${linkage}
                        PRIVATE Dyninst::parseAPI${linkage})
endforeach()
