project(patchAPI)

add_executable(patchAPI_includes includes.cpp)
target_link_libraries(patchAPI_includes PRIVATE Dyninst::patchAPI)
