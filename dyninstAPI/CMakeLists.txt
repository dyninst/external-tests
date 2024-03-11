project(dyninstAPI)

add_executable(dyninstAPI_insertion_operators insertion_operators.cpp)
target_link_libraries(dyninstAPI_insertion_operators
                      PRIVATE Dyninst::dyninstAPI)

add_executable(dyninstAPI_includes includes.cpp)
target_link_libraries(dyninstAPI_includes PRIVATE Dyninst::dyninstAPI)

if(NOT DYNINST_DISABLE_STATIC_TESTS)
  add_subdirectory(static_ctors_dtors)
endif()
