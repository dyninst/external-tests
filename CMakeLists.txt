cmake_minimum_required(VERSION 3.4)
project(Dyninst-External-Tests)

find_package(Dyninst REQUIRED)
find_package(Boost REQUIRED)

add_subdirectory(common)
add_subdirectory(dataflowAPI)
add_subdirectory(proccontrol)
add_subdirectory(symtab)
