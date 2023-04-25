project(instructionAPI)

add_executable(instructionAPI_includes includes.cpp)
target_link_libraries(instructionAPI_includes PRIVATE Dyninst::instructionAPI)
