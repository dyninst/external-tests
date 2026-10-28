project(dyninstAPI_static_ctor_dtor)

dyninst_add_test_executable(main SOURCE_FILES main.cpp DEPS "-static" REQUIRES static_exe)
dyninst_add_test_executable(
  mutator
  SOURCE_FILES mutator.cpp
  DEPS Dyninst::dyninstAPI
  REQUIRES static_exe
)
dyninst_add_test(
  dyninstAPI_static_ctor_dtor
  COMMAND sh -c "./mutator && ./main_rewritten"
  REQUIRES static_exe static_rewrite
  PROPERTIES
    PASS_REGULAR_EXPRESSION "Foo\nHello\n~Foo"
    ENVIRONMENT "DYNINSTAPI_RT_LIB=${Dyninst_DIR}/../../libdyninstAPI_RT.a"
)
