cmake_minimum_required(VERSION 3.14.0 FATAL_ERROR)
project(Dyninst-External-Tests)

# Not all systems have static libs (e.g., libc.a)
option(
  DYNINST_DISABLE_STATIC_TESTS
  "Disable building and executing statically-linked tests"
  OFF
)

find_package(Dyninst 13.0.0 REQUIRED)

enable_testing()

add_subdirectory(common)
add_subdirectory(dataflowAPI)
add_subdirectory(dynC_API)
add_subdirectory(dyninstAPI)
add_subdirectory(generic)
add_subdirectory(instructionAPI)
add_subdirectory(parseAPI)
add_subdirectory(patchAPI)
add_subdirectory(proccontrol)
add_subdirectory(stackwalk)
add_subdirectory(symtab)
