#include "catch.hpp"
#include "Driver.hh"
#include "BasicBlockFormer.hpp"

void TestBB(std::string s)
{
	driver d;
	d.parseString(s);
	d.Compile();
}

TEST_CASE("BBsFormed") {
	TestBB(R"(Disp "Good"
			  5 -> A)");
}

TEST_CASE("BBsFormed2") {
	TestBB(R"(Disp "Good"
			  5 -> A
		      If (1 = 1)
			  Then 
			  Disp "Something"
			  Else
			  Disp "Else"
		      END
			  Disp "End")");
}