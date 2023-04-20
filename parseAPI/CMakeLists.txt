project(parseAPI)

add_executable(parseAPI_includes includes.cpp)
target_include_directories(parseAPI_includes PRIVATE ${DYNINST_INCLUDE_DIR} ${Boost_INCLUDE_DIR} ${TBB_INCLUDE_DIRS})
target_link_libraries(parseAPI_includes PRIVATE parseAPI instructionAPI)