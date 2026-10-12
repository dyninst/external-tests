project(dyninstAPI_discovered_function_names)

enable_language(ASM)

# Hand-written, so that the functions asm_entry calls carry an untyped symbol
# and no symbol at all -- neither of which a compiler will produce.
add_library(discovered_names_asm SHARED asmlib.S)
# CMake has no rule for linking a shared library out of ASM alone.
set_target_properties(discovered_names_asm PROPERTIES OUTPUT_NAME asm LINKER_LANGUAGE C)

add_executable(discovered_names_mutatee mutatee.c)
set_target_properties(discovered_names_mutatee PROPERTIES OUTPUT_NAME mutatee)
target_link_libraries(discovered_names_mutatee PRIVATE discovered_names_asm)
# So that the mutatee can be run by hand from here.  It is not what lets
# Dyninst find libasm.so: dependency resolution does not read the target's
# DT_RPATH, which is why the test puts this directory on LD_LIBRARY_PATH.
set_target_properties(
  discovered_names_mutatee PROPERTIES BUILD_WITH_INSTALL_RPATH TRUE INSTALL_RPATH "$ORIGIN")

add_executable(discovered_names_test discovered_names_test.cpp)
target_link_libraries(discovered_names_test PRIVATE Dyninst::dyninstAPI)
target_compile_definitions(discovered_names_test
                           PRIVATE ARTIFACT_DIR="${CMAKE_CURRENT_BINARY_DIR}")
add_dependencies(discovered_names_test discovered_names_mutatee discovered_names_asm)

# The assembly is x86-64.  Elsewhere there is nothing to run.
if(NOT CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64")
  return()
endif()

# Nothing is rewritten, so neither scenario needs ProcControl.
foreach(_s ordinary excluded)
  add_test(NAME dyninstAPI_discovered_names_${_s}
           COMMAND discovered_names_test ${_s}
           WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
  set_property(
    TEST dyninstAPI_discovered_names_${_s}
    PROPERTY ENVIRONMENT
             "DYNINSTAPI_RT_LIB=$<TARGET_FILE:Dyninst::dyninstAPI_RT>")
endforeach()
