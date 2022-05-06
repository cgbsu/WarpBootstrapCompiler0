#include <iostream>
#include <vector>
#include <string>
#include <type_traits>
#include <utility>
#include <typeinfo>
#include <any>
#include <optional>

#include <ctpg.hpp>

// Quote from https://github.com/Neargye/magic_enum/blob/master/doc/limitations.md
// This library uses a compiler-specific hack (based on __PRETTY_FUNCTION__ / __FUNCSIG__), which works on Clang >= 5, MSVC >= 15.3 and GCC >= 9
// Lets bank on reflection coming in C++26... even though I have been waiting for it since C++03/I was 14 

// #include <magic_enum.hpp>
