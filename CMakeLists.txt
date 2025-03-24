cmake_minimum_required(VERSION 3.19.0)
project(stella-interpreter VERSION 0.1.0)

set (Stella_Syntax
  src/Stella/Absyn.H
  src/Stella/Absyn.C
  src/Stella/Parser.H
  src/Stella/Parser.C
  src/Stella/ParserError.H
  src/Stella/Printer.H
  src/Stella/Printer.C
  src/Stella/Buffer.H
  src/Stella/Buffer.C
  src/Stella/Bison.H
  src/Stella/Lexer.C
)

set (SRCS
  src/Interpret.cpp
  src/Eval.h
  src/Eval.cpp
  src/TypeCheck.h
  src/TypeCheck.cpp

        src/Visiting.h
        src/Visiting.cpp
        src/Enum.h
        src/ObjectType.h
)

add_executable(stella-interpreter ${Stella_Syntax} ${SRCS})
set_property(TARGET stella-interpreter PROPERTY CXX_STANDARD 17)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

include(CTest)

FILE(GLOB ILL_TEST_FILES "tests/*/ill-typed/*")
foreach(FILE_PATH IN LISTS ILL_TEST_FILES)
    add_test(NAME ${FILE_PATH} COMMAND stella-interpreter typecheck ${FILE_PATH})
endforeach()

set_tests_properties(${ILL_TEST_FILES}
        PROPERTIES WILL_FAIL TRUE)


FILE(GLOB WELL_TEST_FILES "tests/*/well-typed/*")
foreach(FILE_PATH IN LISTS WELL_TEST_FILES)
    add_test(NAME ${FILE_PATH} COMMAND stella-interpreter typecheck ${FILE_PATH})
endforeach()