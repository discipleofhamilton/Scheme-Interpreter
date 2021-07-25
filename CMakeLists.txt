cmake_minimum_required(VERSION 3.13)
project(Scheme_Interpreter)

set(CMAKE_CXX_STANDARD 11)

include_directories(${PROJECT_SOURCE_DIR}/header/)

add_executable(Scheme_Interpreter src/uGlobal.cpp main.cpp src/Parser.cpp src/Scanner.cpp 
               src/Evaluator.cpp src/Error.cpp )