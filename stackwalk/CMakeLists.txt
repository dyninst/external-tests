project(stackwalk)

add_executable(stackwalk_includes includes.cpp)
target_include_directories(stackwalk_includes PRIVATE ${DYNINST_INCLUDE_DIR} ${Boost_INCLUDE_DIR} ${TBB_INCLUDE_DIRS})
target_link_libraries(stackwalk_includes PRIVATE stackwalk symtabAPI common)