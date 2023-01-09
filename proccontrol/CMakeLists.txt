project(proccontrol)

foreach(linkage "" "_static")
  add_executable(proccontrol_insertion_operators${linkage}
                 insertion_operators.cpp)
  target_link_libraries(proccontrol_insertion_operators${linkage}
                        PRIVATE Dyninst::pcontrol${linkage})

  add_executable(proccontrol_includes${linkage} includes.cpp)
  target_link_libraries(proccontrol_includes${linkage}
                        PRIVATE Dyninst::pcontrol${linkage})
endforeach()
