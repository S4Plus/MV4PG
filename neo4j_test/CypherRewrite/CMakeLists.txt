cmake_minimum_required(VERSION 3.1)
project(CypherRewrite)
set(CMAKE_CXX_STANDARD 17)
include_directories("include")
include_directories("antlr4-runtime")
add_executable(CypherRewrite main.cpp
     generated/LcypherLexer.cpp
     generated/LcypherParser.cpp
     generated/LcypherVisitor.cpp)
target_link_libraries(CypherRewrite antlr4-runtime.so)
