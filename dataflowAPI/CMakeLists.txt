project(dataflowAPI)

foreach(linkage "" "_static")
  add_executable(dataflowAPI_insertion_operators${linkage}
                 insertion_operators.cpp)
  target_link_libraries(dataflowAPI_insertion_operators${linkage}
                        PRIVATE Dyninst::parseAPI${linkage})
endforeach()
