cmake_minimum_required(VERSION 3.4)
project(Dyninst-External-Tests)

find_package(Dyninst REQUIRED)
find_package(Boost REQUIRED)

set (CMAKE_MODULE_PATH "${Dyninst_DIR}" "${Dyninst_DIR}/Modules" ${CMAKE_MODULE_PATH})
include(ThreadingBuildingBlocks)

# Automatically discovers tests in subdirectories
enable_testing()


add_subdirectory(common)
add_subdirectory(dataflowAPI)
add_subdirectory(dwarf)
add_subdirectory(dynC_API)
add_subdirectory(dyninstAPI)
add_subdirectory(generic)
add_subdirectory(proccontrol)
add_subdirectory(symtab)
