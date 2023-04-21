project(parseAPI)

add_executable(parseAPI_includes includes.cpp)
target_link_libraries(parseAPI_includes PRIVATE Dyninst::parseAPI)