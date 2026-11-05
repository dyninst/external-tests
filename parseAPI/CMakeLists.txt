project(parseAPI)

add_executable(parseAPI_includes includes.cpp)
target_link_libraries(parseAPI_includes PRIVATE Dyninst::parseAPI)

dyninst_add_test_executable(
  simpleParser
  SOURCE_FILES simpleParser.cpp
  DEPS Dyninst::parseAPI
)
dyninst_add_test(parseAPI_simpleParser COMMAND simpleParser $<TARGET_FILE:simpleParser>)
