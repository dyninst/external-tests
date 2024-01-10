project(parseAPI)

add_executable(parseAPI_includes includes.cpp)
target_link_libraries(parseAPI_includes PRIVATE Dyninst::parseAPI)

add_executable(simpleParser simpleParser.cpp)
target_link_libraries(simpleParser PRIVATE Dyninst::parseAPI)
add_test(NAME parseAPI_simpleParser
         COMMAND sh -c "./simpleParser simpleParser")