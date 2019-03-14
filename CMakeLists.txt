cmake_minimum_required(VERSION 3.13)
project(Scheme_Interpreter)

set(CMAKE_CXX_STANDARD 11)
add_executable(Scheme_Interpreter main.cpp
        header/Parser.h src/Parser.cpp
        header/Scanner.h src/Scanner.cpp
        header/Evaluator.h src/Evaluator.cpp
        header/Error.h src/Error.cpp
        header/uGlobal.h)