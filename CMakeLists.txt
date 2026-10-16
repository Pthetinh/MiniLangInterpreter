cmake_minimum_required(VERSION 3.16)
project(MiniLangInterpreter VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

set(CORE_SOURCES
    src/BSTTable.cpp
    src/Lexer.cpp
    src/Parser.cpp
    src/Interpreter.cpp
)

add_executable(MiniLangInterpreter
    src/main.cpp
    ${SOURCES}
 "include/Common.h" "include/Lexer.h" "src/Lexer.cpp")
