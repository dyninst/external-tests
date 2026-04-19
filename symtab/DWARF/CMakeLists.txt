include_guard(GLOBAL)

add_executable(variable_locations variable_locations.cpp)
target_compile_options(variable_locations PRIVATE ${SUPPORTED_CXX_WARNING_FLAGS})
target_link_libraries(variable_locations PRIVATE Dyninst::symtabAPI)
