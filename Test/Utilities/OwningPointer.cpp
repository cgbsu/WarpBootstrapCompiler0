/*#include <Warp/Utilities/OwningPointer.hpp>
#include "CppUTest/TestHarness.h"

template< auto ParameterConstant >
void take( auto&& other ) {
	CHECK( other.get_resource() == ParameterConstant );
}

TEST_GROUP(OwningPointer)
{
	void setup() {
	}
	void teardown() {
	}
};

TEST(OwningPointer, Create)
{
	Warp::Utilities::OwningPointer< int > test( 42 );
};

TEST(OwningPointer, MovePass)
{
	constexpr const int test_value = 42;
	Warp::Utilities::OwningPointer< int > test( test_value );
	take< test_value >( test );
}

*/
