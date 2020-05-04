#include "sample4.h"
#include <gtest/gtest.h>  

TEST(Counter, Increment)
{
    Counter c;
    // Test that counter 0 returns 0
    EXPECT_EQ(0,c.Decrement());
    EXPECT_EQ(0,c.Increment());
    EXPECT_EQ(1,c.Increment());
    EXPECT_EQ(2,c.Increment());
    EXPECT_EQ(3,c.Decrement());
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
//namespace


