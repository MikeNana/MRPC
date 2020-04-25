#include <limits.h>
#include "sample1.h"
#include "gtest/gtest.h"

TEST(FactorialTest, Negative)
{
    EXPECT_EQ(1, factorial(-5));
    EXPECT_EQ(1, factorial(-1));
    EXPECT_GT(factorial(-10), 0);
}

TEST(FactorialTest, Positive)
{
    EXPECT_EQ(1, factorial(1));
    EXPECT_EQ(2, factorial(2));
    EXPECT_EQ(6, factorial(3));
    EXPECT_NE(8, factorial(12));
}

TEST(FactorialTest, Zero)
{
    EXPECT_EQ(1, factorial(0));
}

TEST(IsPrimeTest, Negative)
{
    EXPECT_FALSE(IsPrime(-1));
    EXPECT_FALSE(IsPrime(-2));
    EXPECT_FALSE(IsPrime(INT_MIN));
}

TEST(IsPrimeTest, Trivial)
{
    EXPECT_FALSE(IsPrime(0));
    EXPECT_FALSE(IsPrime(1));
    EXPECT_TRUE(IsPrime(2));
    EXPECT_TRUE(IsPrime(3));
}

TEST(IsPrimeTest, Positive)
{
    EXPECT_FALSE(IsPrime(4));
    EXPECT_FALSE(IsPrime(6));
    EXPECT_TRUE(IsPrime(5));
    EXPECT_TRUE(IsPrime(31));
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

