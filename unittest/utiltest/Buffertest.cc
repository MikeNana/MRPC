#include <gtest/gtest.h>
#include "../../util/Buffer.h"

using namespace mrpc;

//test push function
TEST(Buffer, push) 
{
    Buffer buf;

    size_t ret = buf.PushData("hello", 5);
    EXPECT_TRUE(ret == 5);

    ret = buf.PushData("world\n", 6);
    EXPECT_TRUE(ret == 6);
}
//test peek function
TEST(Buffer, peek)
{
    Buffer buf;

    buf.PushData("hello ", 6);
    buf.PushData("world\n", 6);

    char temp[12];
    size_t ret = buf.PeekDataAt(temp, 5, 0);
    EXPECT_TRUE(ret == 5);
    EXPECT_TRUE(temp[0] == 'h');
    EXPECT_TRUE(temp[1] == 'e');
    EXPECT_TRUE(temp[4] == 'o');

    ret = buf.PeekDataAt(temp, 2, 6);
    EXPECT_TRUE(ret == 2);
    EXPECT_TRUE(temp[0] == 'w');
    EXPECT_TRUE(temp[1] == 'o');

    EXPECT_TRUE(buf.readablesize() == 12);
}

TEST(Buffer, pop)
{
    Buffer buf; 

    buf.PushData("hello ", 6);
    EXPECT_TRUE(buf.capacity() == 256);
    buf.PushData("world\n", 6);
    EXPECT_TRUE(buf.capacity() == 256);

    size_t cap = buf.capacity();
    EXPECT_TRUE(cap == 256);

    char temp[12];
    size_t ret = buf.PopData(temp, 6);
    EXPECT_TRUE(ret == 6);
    EXPECT_TRUE(temp[0] == 'h');
    EXPECT_TRUE(temp[1] == 'e');
    EXPECT_TRUE(temp[5] == ' ');

    EXPECT_TRUE(buf.readablesize() == 6);

    ret = buf.PopData(temp, 6);
    EXPECT_TRUE(ret == 6);
    EXPECT_TRUE(temp[0] == 'w');
    EXPECT_TRUE(temp[5] == '\n');

    EXPECT_TRUE(buf.Isempty());
    EXPECT_TRUE(buf.capacity() == cap); //pop doesn't change capacity, we didn't use shrink()
}
//test shrink
TEST(Buffer, shrink)
{
    Buffer buf;

    {
        buf.PushData("hello ", 6);
        buf.PushData("world\n", 6);
    }

    EXPECT_FALSE(buf.capacity() == 12);

    buf.Shrink();
    EXPECT_TRUE(buf.capacity() == 16);

    buf.PushData("abcd", 4);
    EXPECT_TRUE(buf.capacity() == 16);

    char temp[16];
    buf.PopData(temp, sizeof(temp));
    EXPECT_TRUE(buf.capacity() == 16);

    buf.Shrink();
    EXPECT_TRUE(buf.capacity() == 16);
}
//test push and pop
TEST(Buffer, pop_and_push)
{
    Buffer buf;
    buf.PushData("hello ", 6);

    char temp[5];
    size_t ret = buf.PopData(temp, 5);

    EXPECT_TRUE(buf.capacity() == 256);
    EXPECT_TRUE(ret == 5);

    buf.Shrink();
    EXPECT_TRUE(buf.capacity() == 1);
}


int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}