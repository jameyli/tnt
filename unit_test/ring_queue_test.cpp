/**
 * @file:   ring_queue_test.cpp
 * @author: jameyli <jameyli AT tencent DOT com>
 * @date:   2013-09-11
 * @brief:  ring_queue_test
 */
#include "gtest/gtest.h"
#include <stdio.h>
#include "ring_queue.h"

using namespace testing;
using namespace tnt;

class RingQueueTest : public Test
{
protected:
    static void SetUpTestCase()
    {
    }

    static void TearDownTestCase()
    {
    }

protected:
};

TEST_F(RingQueueTest, Push)
{
    ring_queue<int, 100> queue;
    int test1=8888;
    int test2=9999;

    std::cout << test1 << test2 <<std::endl;


    ASSERT_TRUE(queue.empty());
//    EXPECT_EQ(100,  queue.capacity());
    std::cout << queue.capacity() << std::endl;
    std::cout << queue.size() << std::endl;



    for (size_t i=1; i<10000; ++i)
    {
        queue.push(-1);

//        std::cout << queue.size() << std::endl;

//        EXPECT_EQ((int)i,  queue.back());
//        EXPECT_EQ(i+1,  queue.size());
//        EXPECT_EQ(0,  queue.front());
    }

    std::cout << queue.debug_str() <<std::endl;

    std::cout << queue.front() <<std::endl;
    std::cout << queue.back() << std::endl;

//    EXPECT_EQ(0,  queue.front());

//    ASSERT_TRUE(queue.full());
//
    for (size_t i=0; i<10000; ++i)
    {
        queue.pop();
    }

    std::cout << queue.debug_str() <<std::endl;
    std::cout << queue.front() <<std::endl;
    std::cout << queue.back() << std::endl;

    for (size_t i=1; i<10000; ++i)
    {
        queue.push(i);
    }

    std::cout << queue.debug_str() <<std::endl;
    std::cout << queue.front() <<std::endl;
    std::cout << queue.back() << std::endl;

    for (size_t i=0; i<200; ++i)
    {
        std::cout << queue[i] << " ";
    }
    std::cout << std::endl;

}

TEST_F(RingQueueTest, Press)
{
    ring_queue<int, 100> queue;

    ASSERT_TRUE(queue.empty());
//    EXPECT_EQ(100,  queue.capacity());
    std::cout << queue.capacity() << std::endl;
    std::cout << queue.size() << std::endl;

    for (size_t i=0; i<100000; ++i)
    {
//        for (size_t j=0; j<1000; ++j)
//        {
//            queue.push(i*1000 + j);
//        }

//        std::cout << queue[30] << std::endl;

        for (size_t j=0; j<200; ++j)
        {
            queue.pop();
        }

//        std::cout << queue.front() << std::endl;
    }
}

