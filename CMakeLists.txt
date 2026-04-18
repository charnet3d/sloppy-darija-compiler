cmake_minimum_required(VERSION 3.15)
project(DarijaCompiler VERSION 1.0.0 LANGUAGES CXX)

# C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Source files
set(LEXER_SOURCES
    src/lexer/lexer.cpp
)

set(PARSER_SOURCES
    src/parser/parser.cpp
)

set(RUNTIME_SOURCES
    src/runtime.cpp
)

set(AST_SOURCES
    # AST is header-only
)

# Header directories
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/src/lexer)
include_directories(${CMAKE_SOURCE_DIR}/src/parser)
include_directories(${CMAKE_SOURCE_DIR}/src/ast)

# Main compiler executable
add_executable(darija-compiler
    src/compiler.cpp
    ${LEXER_SOURCES}
    ${PARSER_SOURCES}
)

add_library(darija-runtime STATIC
    ${RUNTIME_SOURCES}
)
if(MSVC)
    if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
        target_compile_options(darija-runtime PRIVATE /GS- /O2)
    else()
        target_compile_options(darija-runtime PRIVATE /GS- /Od /RTC1)
    endif()
endif()

target_include_directories(darija-compiler PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/lexer
    ${CMAKE_SOURCE_DIR}/src/parser
    ${CMAKE_SOURCE_DIR}/src/ast
)

# Unit tests
enable_testing()

add_executable(unit_tests
    tests/test_lexer.cpp
    tests/test_parser.cpp
    tests/test_ast.cpp
    tests/test_main.cpp
    ${LEXER_SOURCES}
    ${PARSER_SOURCES}
)

target_include_directories(unit_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/lexer
    ${CMAKE_SOURCE_DIR}/src/parser
    ${CMAKE_SOURCE_DIR}/src/ast
)

# Add tests
add_test(NAME LexerTests COMMAND unit_tests --gtest_filter="LexerTest.*")
add_test(NAME ParserTests COMMAND unit_tests --gtest_filter="ParserTest.*")
add_test(NAME ASTTests COMMAND unit_tests --gtest_filter="ASTTest.*")

# Installation
install(TARGETS darija-compiler
    RUNTIME DESTINATION bin
)

# Print configuration
message(STATUS "Darija Compiler Configuration:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
