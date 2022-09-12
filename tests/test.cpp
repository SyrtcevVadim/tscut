#define CATCH_CONFIG_MAIN
#include<catch2/catch.hpp>

int sum(int a, int b)
{
    return a+b;
}

TEST_CASE("Test sum function", "[sum]") 
{
    REQUIRE(sum(1,2) == 3);
}