project(dyninstAPI_plt_stub_lookup)

# Holds the definition the mutatee imports, so that the call to it from the
# executable goes through a PLT stub.
add_library(plt_stub_def SHARED libdef.c)
set_target_properties(plt_stub_def PROPERTIES OUTPUT_NAME def)

add_executable(plt_stub_mutatee mutatee.c)
set_target_properties(plt_stub_mutatee PROPERTIES OUTPUT_NAME mutatee)
target_link_libraries(plt_stub_mutatee PRIVATE plt_stub_def)
# So that the mutatee can be run by hand from here to confirm it works.  It is
# not what lets Dyninst find libdef.so: dependency resolution does not read the
# target's DT_RPATH, which is why the test puts this directory on
# LD_LIBRARY_PATH itself.
set_target_properties(
  plt_stub_mutatee PROPERTIES BUILD_WITH_INSTALL_RPATH TRUE INSTALL_RPATH "$ORIGIN")

add_executable(plt_stub_test plt_stub_test.cpp)
target_link_libraries(plt_stub_test PRIVATE Dyninst::dyninstAPI)
target_compile_definitions(plt_stub_test
                           PRIVATE ARTIFACT_DIR="${CMAKE_CURRENT_BINARY_DIR}")
add_dependencies(plt_stub_test plt_stub_mutatee plt_stub_def)

# Neither scenario drives a live process -- the binaries are opened and
# queried, never written -- so both run whether or not dynamic tests are on.
foreach(_s no-deps deps)
  add_test(NAME dyninstAPI_plt_stub_lookup_${_s}
           COMMAND plt_stub_test ${_s}
           WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
  # Not needed -- nothing here instruments, so the runtime library is never
  # loaded -- but BPatch's constructor complains on stderr without it, and that
  # complaint in a CI log reads like a broken test.  Ask the imported target
  # rather than deriving a path from Dyninst_DIR: the library directory is not
  # always two levels above the config directory, and the .so symlink is not
  # always installed.
  set_property(
    TEST dyninstAPI_plt_stub_lookup_${_s}
    PROPERTY ENVIRONMENT
             "DYNINSTAPI_RT_LIB=$<TARGET_FILE:Dyninst::dyninstAPI_RT>")
endforeach()
