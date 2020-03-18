#include "catch.hpp"
#include "Driver.hh"
#include "BasicBlockFormer.hpp"

void TestBB(std::string s)
{
	driver d;
	d.parseString(s);
	BasicBlockFormer f;
	f.ParseBlocks(std::move(d.topNode));
}

TEST_CASE("BBsFormed") {
	TestBB(R"(Disp "Good"
			  5 -> A)");
}