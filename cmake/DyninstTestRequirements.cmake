# The requirements tests name in dyninst_add_test(... REQUIRES ...). See
# DyninstAddTest.cmake. Each condition is evaluated where the test is added.

include_guard(GLOBAL)

include(DyninstAddTest)

dyninst_test_requirement(
  static_exe
  WHEN DYNINST_ENABLE_STATIC_TESTS
  REASON "needs static executables (DYNINST_ENABLE_STATIC_TESTS=OFF)"
)
