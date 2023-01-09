project(instructionAPI)

foreach(linkage "" "_static")
  add_executable(instructionAPI_includes${linkage} includes.cpp)
  target_link_libraries(instructionAPI_includes${linkage}
                        PRIVATE Dyninst::instructionAPI${linkage})
endforeach()
