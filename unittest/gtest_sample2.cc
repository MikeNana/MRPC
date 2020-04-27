#include "sample2.h"
#include <gtest/gtest.h>

//test the default c'tor
TEST(MyString, DefaultConstructor)
{
    const MyString s;
    EXPECT_STREQ(nullptr, s.c_string());
    EXPECT_EQ(0u, s.Length());
}
//test the c'tor that accepts a C string
const char Khellostring[] = "Hello World!";
TEST(MyString, ConstructorFromString)
{
    const MyString s(Khellostring);
    EXPECT_EQ(0, strcmp(s.c_string(), Khellostring));
    EXPECT_EQ(sizeof(Khellostring)/sizeof(Khellostring[0])-1, s.Length());
}
//test the copy c'tor
TEST(MyString, CopyConstructor)
{
    const MyString s1(Khellostring);
    const MyString s2 = s1;
    EXPECT_EQ(0, strcmp(s2.c_string(), Khellostring));
}
//test Set function
TEST(MyString, Set)
{
    MyString s;
    s.Set(Khellostring);
    EXPECT_EQ(0, strcmp(s.c_string(), Khellostring));
    s.Set(s.c_string());
    EXPECT_EQ(0, strcmp(s.c_string(), Khellostring));
    s.Set(nullptr);
    EXPECT_EQ(nullptr, s.c_string());
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}