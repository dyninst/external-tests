project(dyninstAPI_analysis_exclusion)

# The library the tests exclude from analysis.  Every function in it reaches
# global data, so on ppc64 a call into it needs a correct TOC pointer.
add_library(exclusion_toc SHARED toclib.c)
set_target_properties(exclusion_toc PROPERTIES OUTPUT_NAME toc)

# dlopen'd after attach: A matches the exclusion pattern, B does not.
add_library(exclusion_plugA SHARED plugA.c)
set_target_properties(exclusion_plugA PROPERTIES OUTPUT_NAME plugA)
add_library(exclusion_plugB SHARED plugB.c)
set_target_properties(exclusion_plugB PROPERTIES OUTPUT_NAME plugB)

add_executable(exclusion_mutatee mutatee.c)
set_target_properties(exclusion_mutatee PROPERTIES OUTPUT_NAME mutatee)
target_link_libraries(exclusion_mutatee PRIVATE exclusion_toc ${CMAKE_DL_LIBS})
# The copy made in each test directory must load the libtoc.so beside it, and
# in particular the *rewritten* one.  CMake would otherwise bake the build tree
# into RPATH, and DT_RPATH is searched before LD_LIBRARY_PATH, so the pristine
# library would win and the rewriting tests would silently prove nothing.
set_target_properties(
  exclusion_mutatee PROPERTIES BUILD_WITH_INSTALL_RPATH TRUE INSTALL_RPATH "$ORIGIN")

add_executable(exclusion_test exclusion_test.cpp)
target_link_libraries(exclusion_test PRIVATE Dyninst::dyninstAPI
                                            Dyninst::parseAPI
                                            Dyninst::symtabAPI)
target_compile_definitions(exclusion_test
                           PRIVATE ARTIFACT_DIR="${CMAKE_CURRENT_BINARY_DIR}")
add_dependencies(exclusion_test exclusion_mutatee exclusion_toc exclusion_plugA
                 exclusion_plugB)

# create, attach and dlopen drive a live process and so need ProcControl.
# rewrite and rewrite-dep only rewrite files on disk.
set(_dynamic_scenarios create attach dlopen)
set(_static_scenarios rewrite rewrite-dep)

# When dynamic tests are off they are still registered, and report
# themselves skipped, rather than vanishing: a test that is absent looks the
# same as a test nobody wrote.
if(DYNINST_ENABLE_DYNAMIC_TESTS)
  target_compile_definitions(exclusion_test PRIVATE DYNINST_ENABLE_DYNAMIC_TESTS=1)
else()
  target_compile_definitions(exclusion_test PRIVATE DYNINST_ENABLE_DYNAMIC_TESTS=0)
endif()

foreach(_s ${_static_scenarios} ${_dynamic_scenarios})
  add_test(NAME dyninstAPI_analysis_exclusion_${_s}
           COMMAND exclusion_test ${_s}
           WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
  # Ask the imported target rather than deriving a path from Dyninst_DIR: the
  # library directory is not always two levels above the config directory, and
  # the unversioned .so symlink is not always installed.
  set_property(
    TEST dyninstAPI_analysis_exclusion_${_s}
    PROPERTY ENVIRONMENT
             "DYNINSTAPI_RT_LIB=$<TARGET_FILE:Dyninst::dyninstAPI_RT>")
  # matches kSkipExitCode in exclusion_test.cpp
  set_property(TEST dyninstAPI_analysis_exclusion_${_s} PROPERTY SKIP_RETURN_CODE 77)
endforeach()
