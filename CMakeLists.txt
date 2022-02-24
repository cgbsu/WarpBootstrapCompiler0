cmake_minimum_required(VERSION 2.8.12)
project(StarterCompiler)

set(CMAKE_CXX_STANDARD 20)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(starter_compiler ${CMAKE_SOURCE_DIR}/Source/Main.cpp)
target_link_libraries(starter_compiler ${CONAN_LIBS})
