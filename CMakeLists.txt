cmake_minimum_required(VERSION 3.7)
project(HW2)

set(CMAKE_CXX_STANDARD 98)

set(SOURCE_FILES
        grammar.cpp
        grammar.h
        hw2.cpp
        hw2.h
        lexer.lex
        main.cpp
        tokens.h)

add_executable(HW2 ${SOURCE_FILES})