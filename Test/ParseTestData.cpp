#include <Warp/Testing/ParseTest.hpp>

namespace Warp::Parser::Testing
{
    std::array< const char*, 3 > test_suite_names = {
            "alternative_calls", 
            "basic_function_alternatives", 
            "factor_calls"
        };

    TestSuiteType function_alternative_calls{
            WarpTest{ true, "let test( a : a < 64 ) :: test( 1 );" }, 
            WarpTest{ true, "let test( a : a < 64 ) :: test( a );" }, 
            WarpTest{ true, "let test( a : a < 64 ) :: 20 * test( a );" }, 
            WarpTest{ true, "let test( a : a < 64 ) :: 20 + test( a );" }, 
            WarpTest{ true, "let test( a : a < 64, b : test( b ) < 34 ) :: test( a ) * 20;" }, 
            WarpTest{ true, "let test( a : a < 64 ) :: test( a ) + 10;" }, 
            WarpTest{ true, "let test( a : a < 64, b : test( b ) < 34, c : c > 100 * a ) :: test( a * a );" }, 
            WarpTest{ true, "let test( a : a < 64 ) :: test( a(), b( q, 4, 5646, 345345 * 445656 + 34 ), rdfg * 34534 );" }, 
            WarpTest{ true, "let test( a : a < 64, c : c > 100 * a ) :: test( a, q, 4, 5646, 345345 * 445656 + 34, rdfg * 34534 );" }, 
            WarpTest{ true, "let test( a : a < 64, b : test( b ) < 34 ) :: test( ttt(), q, 4, 5646, 345345 * 445656 + 34, rdfg * 34534 );" }, 
            WarpTest{ true, "let test( a : a < 64 ) :: test( ttt(), q, 4 * another_test(), 5646, 345345 * 445656 + 34, rdfg * 34534 );" }, 
            WarpTest{ true, "let test( a : a < 64 ) :: test( ttt(), q, 4 * another_test(), 5646, 345345 * another_test( abc, 123 ) + 34, rdfg * 34534 );" }, 
            WarpTest{ true, "let test( a : a < 64, b : test( b ) < 34 ) :: test( ttt(), q, 4 * another_test(), 5646, 345345 + another_test( abc, 123 ) * 34, rdfg * 34534 );" }, 
            WarpTest{ true, "let test( a : a < 64, b : test( b ) < 34, c : c > 100 * a, d : d <= c + a * 100 ) :: test( ttt(), q, 4 / another_test(), 5646, 345345 / another_test( abc, 123 ) - 34, rdfg / 34534 );" }, 
            WarpTest{ true, "let test( a : a < 64, c : c > 100 * a ) :: test( ttt(), q, 4 / another_test(), 5646, 345345 - another_test( abc, 123 ) - 34, rdfg / 34534 );" } 
        };

    TestSuiteType basic_function_alternatives{
            WarpTest{ true, "let test( a : a < 64 ) :: 1;" }, 
            WarpTest{ true, "let test( a : a < 64 ) :: a;" }, 
            WarpTest{ true, "let test( a : a < 64 ) :: 20 * a;" }, 
            WarpTest{ true, "let test( a : a < 64 ) :: 20 + a;" }, 
            WarpTest{ true, "let test( a : a < 64 ) :: a * 20;" }, 
            WarpTest{ true, "let test( a : a < 64 ) :: a + 10;" }, 
            WarpTest{ true, "let test( a : a < 64 ) :: a * a;" }, 
            WarpTest{ true, "let test( a : a < 64, b : b < 128 && b > a * a ) :: a + b * a * ( 120 + 343 + a );" } 
        };

    TestSuiteType factor_calls{
            WarpTest{ true, "test( 1 )" }, 
            WarpTest{ true, "test( a )" }, 
            WarpTest{ true, "20 * test( a )" }, 
            WarpTest{ true, "20 + test( a )" }, 
            WarpTest{ true, "test( a ) * 20" }, 
            WarpTest{ true, "test( a ) + 10" }, 
            WarpTest{ true, "test( a * a )" }, 
            WarpTest{ true, "test( a(), b( q, 4, 5646, 345345 * 445656 + 34 ), rdfg * 34534 )" }, 
            WarpTest{ true, "test( a, q, 4, 5646, 345345 * 445656 + 34, rdfg * 34534 )" }, 
            WarpTest{ true, "test( ttt(), q, 4, 5646, 345345 * 445656 + 34, rdfg * 34534 )" } 
        };


    std::vector< TestSuiteType > test_suits{ 
            function_alternative_calls, 
            basic_function_alternatives, 
            factor_calls
        };
}
