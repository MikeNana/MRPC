#include <gtest/gtest.h>

class ADD
{
private:
    int a;
    int b;
public:
    ADD(int a, int b);
    int func()
    {
        return a+b;
    }
};


class ADDtest:public testing::Test
{
protected:
    ADDtest(int a, int b):Add(a, b){}
private:
    ADD Add;
};


TEST_F(ADDtest(1,5), func)
{

}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

