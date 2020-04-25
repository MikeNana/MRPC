#include <gtest/gtest.h>
#include "../util/Buffer.h"

using namespace mrpc;

TEST_F(Buffer, push) 
{
    Buffer buf;

    size_t ret = buf.PushData("hello", 5);
    EXPECT_TRUE(ret == 5);

    ret = buf.PushData("world\n", 6);
    EXPECT_TRUE(ret == 6);
}

