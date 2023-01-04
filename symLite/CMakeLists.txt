project(symLite)

add_executable(symLite_includes includes.cpp)
target_link_libraries(symLite_includes PRIVATE Dyninst::symLite)
