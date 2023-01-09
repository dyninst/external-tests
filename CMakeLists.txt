cmake_minimum_required(VERSION 3.13)
project(Dyninst-External-Tests)

set(_components
	common
	dynDwarf
	dynC_API
	dyninstAPI
	dynElf
	instructionAPI
	parseAPI
	patchAPI
	pcontrol
	stackwalk
	symLite
	symtabAPI)

foreach(c ${_components})
	list(APPEND _components "${c}_static")
endforeach()

find_package(Dyninst REQUIRED COMPONENTS ${_components})

# Automatically discovers tests in subdirectories
enable_testing()

add_subdirectory(common)
add_subdirectory(dataflowAPI)
add_subdirectory(dwarf)
add_subdirectory(dynC_API)
add_subdirectory(dyninstAPI)
add_subdirectory(elf)
add_subdirectory(generic)
add_subdirectory(instructionAPI)
add_subdirectory(parseAPI)
add_subdirectory(patchAPI)
add_subdirectory(proccontrol)
add_subdirectory(stackwalk)
add_subdirectory(symLite)
add_subdirectory(symtab)
