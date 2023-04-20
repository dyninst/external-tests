project(dyninstAPI)

add_executable(dyninstAPI_insertion_operators insertion_operators.cpp)
target_include_directories(dyninstAPI_insertion_operators PRIVATE ${DYNINST_INCLUDE_DIR} ${Boost_INCLUDE_DIR} ${TBB_INCLUDE_DIRS})
target_link_libraries(dyninstAPI_insertion_operators PRIVATE dyninstAPI)

add_executable(dyninstAPI_includes includes.cpp)
target_include_directories(dyninstAPI_includes PRIVATE ${DYNINST_INCLUDE_DIR} ${Boost_INCLUDE_DIR} ${TBB_INCLUDE_DIRS})
target_link_libraries(dyninstAPI_includes PRIVATE dyninstAPI)
