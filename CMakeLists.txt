cmake_minimum_required(VERSION 3.4 FATAL_ERROR)
project(Dyninst-External-Tests)

find_package(Dyninst REQUIRED)

add_subdirectory(common)
add_subdirectory(dataflowAPI)
add_subdirectory(dyninstAPI)
add_subdirectory(generic)
add_subdirectory(proccontrol)
add_subdirectory(symtab)
