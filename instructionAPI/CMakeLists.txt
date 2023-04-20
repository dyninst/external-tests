project(instructionAPI)

add_executable(instructionAPI_includes includes.cpp)
target_include_directories(instructionAPI_includes PRIVATE ${DYNINST_INCLUDE_DIR} ${Boost_INCLUDE_DIR} ${TBB_INCLUDE_DIRS})
target_link_libraries(instructionAPI_includes PRIVATE instructionAPI)
