#include "catch.hpp"
#include "TestCode.hpp"

TEST_CASE("CodeGenSimpleAdd")
{
	TestCode(R"(Disp "Passed")");
}

TEST_CASE("CodeGenVariable")
{
	TestCode(R"(5 -> a
				Disp a)");
}