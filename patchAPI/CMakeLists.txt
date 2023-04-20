project(patchAPI)

add_executable(patchAPI_includes includes.cpp)
target_include_directories(patchAPI_includes PRIVATE ${DYNINST_INCLUDE_DIR} ${Boost_INCLUDE_DIR} ${TBB_INCLUDE_DIRS})
target_link_libraries(patchAPI_includes PRIVATE patchAPI)