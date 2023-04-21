project(dyninstAPI_static_ctor_dtor)

add_executable(main main.cpp)
target_link_libraries(main PRIVATE "-static")

add_executable(mutator mutator.cpp)
target_include_directories(mutator PRIVATE ${DYNINST_INCLUDE_DIR} ${Boost_INCLUDE_DIR} ${TBB_INCLUDE_DIRS})
target_link_libraries(mutator PRIVATE dyninstAPI)
add_test(NAME dyninstAPI_static_ctor_dtor COMMAND sh -c "./mutator && ./main_rewritten")
set_property(TEST dyninstAPI_static_ctor_dtor PROPERTY PASS_REGULAR_EXPRESSION "Foo\nHello\n~Foo")
set_property(TEST dyninstAPI_static_ctor_dtor PROPERTY ENVIRONMENT "DYNINSTAPI_RT_LIB=${Dyninst_DIR}/../../libdyninstAPI_RT.a")
