project(module)

dyninst_add_test(module_parser SOURCE_FILES parser.cpp DEPS Dyninst::symtabAPI)

add_library(lib1func1 OBJECT lib1.cpp)
target_compile_definitions(lib1func1 PRIVATE FUNC1)
target_compile_options(lib1func1 PRIVATE -g)

add_library(lib1func2 OBJECT lib1.cpp)
target_compile_definitions(lib1func2 PRIVATE FUNC2)
target_compile_options(lib1func2 PRIVATE -g)

add_library(moduletest SHARED lib2.cpp)
target_link_libraries(moduletest PRIVATE lib1func1 lib1func2)
target_compile_options(moduletest PRIVATE -g)
