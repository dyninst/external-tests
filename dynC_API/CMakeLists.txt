project(dynC_API)

dyninst_add_test(dynC_API_includes SOURCE_FILES includes.cpp DEPS Dyninst::dynC_API)
