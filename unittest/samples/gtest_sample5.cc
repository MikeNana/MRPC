#include <limits.h> 
#include <time.h>  
#include <gtest/gtest.h>
#include "sample1.h"
#include "sample3-inl.h"
//In this sample, we want to ensure that every test finishes 
//within 5 seconds. If the test takes longer to run, we consider it a failure
class QuickTest:public testing::Test
{
protected:
    //This is a good place to record the start time
    void SetUp() override
    {
        start_time_ = time(nullptr);
    }
    //This is a good place to record the end time
    void TearDown() override
    {
        const time_t end_time_ = time(nullptr);
        EXPECT_TRUE(end_time_ - start_time_ <= 5) << "The test took too long";
    }
    time_t start_time_;
};

class IntegerFunctionTest:public QuickTest
{};

TEST_F(IntegerFunctionTest, Factorial)
{
    //test the negative number
    EXPECT_EQ(1, Factorial(-5));
    EXPECT_EQ(1, Factorial(-1));
    EXPECT_GT(Factorial(-10),0);
    //test factorial of 0
    EXPECT_EQ(1, Factorial(0));

}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

