# The requirements tests name in dyninst_add_test(... REQUIRES ...). See
# DyninstAddTest.cmake. Each condition is evaluated where the test is added.

include_guard(GLOBAL)

include(DyninstAddTest)

dyninst_test_requirement(
  static_exe
  WHEN DYNINST_ENABLE_STATIC_TESTS
  REASON "needs static executables (DYNINST_ENABLE_STATIC_TESTS=OFF)"
)

cmake_host_system_information(RESULT DYNINST_TEST_HOST_ARCH QUERY OS_PLATFORM)
dyninst_test_requirement(
  static_rewrite
  WHEN NOT DYNINST_TEST_HOST_ARCH STREQUAL "aarch64"
  REASON "Dyninst cannot rewrite static executables on aarch64"
)
