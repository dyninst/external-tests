project(stackwalk)

add_executable(stackwalk_includes includes.cpp)
target_link_libraries(stackwalk_includes PRIVATE Dyninst::stackwalk)
