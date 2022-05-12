cmake_minimum_required(VERSION 2.8.12)
project(StarterCompiler)

set(CMAKE_CXX_STANDARD 20)
add_compile_options(-fconstexpr-ops-limit=3355443299)#-g  -ggdb) #-fsanitize=leak) #-fsanitize=address)
#add_compile_options(-fdiagnostics-format=json -fdiagnostics-show-template-tree  -fdiagnostics-column-origin=1 -fdiagnostics-parseable-fixits -fdiagnostics-path-format=inline-events -fdiagnostics-show-path-depths)
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

include_directories(${CMAKE_SOURCE_DIR}/Include)
FILE(GLOB SourceFiles ${CMAKE_SOURCE_DIR}/Source/*.cpp)
FILE(GLOB TestSourceFiles ${CMAKE_SOURCE_DIR}/Test/*.cpp)
add_executable(starter_compiler ${SourceFiles} ${TestSourceFiles})
target_link_libraries(starter_compiler ${CONAN_LIBS})
