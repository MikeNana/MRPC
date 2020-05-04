//an advanced feature of googletest called test fixture
#include "sample3-inl.h"
#include "gtest/gtest.h"

namespace
{
// to use a test fixture, derive a class from testing::Test
class QueueTestSmpl3 : public testing::Test
{
//we should make the members protected, they can be accessed from sub-classes
protected:
    //virtual void setup() will be called() before each test is run.
    //You should define it if you need to initialize the variables
    void SetUp() override
    {
        q1_.Enqueue(1);
        q2_.Enqueue(2);
        q2_.Enqueue(3);
    }

    //virtual void teardown() will be called() after each test is run.
    //You should define it if there is cleanup to do.
    //void TearDown() override{}

    static int Double(int n)
    {
        return 2*n;
    }
    void MapTester(const Queue<int> *q)
    {
        //create a new queue, where each element is twice as big as the corresponding one in q;
        const Queue<int>* const new_q = q->Map(Double);
        ASSERT_EQ(q->Size(), new_q->Size());        
        for(const QueueNode<int>* n1 = q->Head(),*n2 = new_q->Head(); n1 != nullptr; n1 = n1->next(), n2 = n2->next())
        {
            EXPECT_EQ(2 * n1->element(), n2->element());
        }
        delete new_q;
    }   
    Queue<int> q0_;
    Queue<int> q1_;
    Queue<int> q2_;
};

}

TEST_F(QueueTestSmpl3, DefaultConstructor) {
  // You can access data in the test fixture here.
  EXPECT_EQ(0u, q0_.Size());
}

// Tests Dequeue().
TEST_F(QueueTestSmpl3, Dequeue) {
  int * n = q0_.Dequeue();
  EXPECT_TRUE(n == nullptr);

  n = q1_.Dequeue();
  ASSERT_TRUE(n != nullptr);
  EXPECT_EQ(1, *n);
  EXPECT_EQ(0u, q1_.Size());
  delete n;

  n = q2_.Dequeue();
  ASSERT_TRUE(n != nullptr);
  EXPECT_EQ(2, *n);
  EXPECT_EQ(1u, q2_.Size());
  delete n;
}

// Tests the Queue::Map() function.
TEST_F(QueueTestSmpl3, Map) {
  MapTester(&q0_);
  MapTester(&q1_);
  MapTester(&q2_);
}



int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}