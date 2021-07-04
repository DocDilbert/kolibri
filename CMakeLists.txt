cmake_minimum_required(VERSION 3.14)
project(intertest)

# GoogleTest requires at least C++11
set(CMAKE_CXX_STANDARD 14)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()



add_executable(interp
  src/interp.cc
)

add_custom_command(
        TARGET interp POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_SOURCE_DIR}/test_files/equation.txt
                ${CMAKE_CURRENT_BINARY_DIR}/equation.txt)

add_custom_command(
        TARGET interp POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_SOURCE_DIR}/test_files/main.pas
                ${CMAKE_CURRENT_BINARY_DIR}/main.pas)

                          

add_library(intertest_lib STATIC 
  src/token_out.cc
)


include_directories(src)
add_executable(
  tests
  tests/languages/calc/calc_integration_test.cc
  tests/languages/calc/calc_interpreter_test.cc
  tests/languages/calc/calc_parser_test.cc
  tests/languages/calc/calc_lexer_test.cc
  tests/languages/ast_test.cc
  tests/parser/parser_rules_test.cc
  tests/parser/parser_productions_test.cc
  tests/lexer/lexer_rules_test.cc
  tests/base/token_test.cc
)

target_link_libraries(
  tests
  gtest_main
  gmock_main
  intertest_lib
)

target_link_libraries(
  interp
  intertest_lib
)

include(GoogleTest)
gtest_discover_tests(tests)