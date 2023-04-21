project(dataflowAPI)

add_executable(df_insertion_operators insertion_operators.cpp)
target_link_libraries(df_insertion_operators PRIVATE Dyninst::parseAPI)
