/**
 * @file:   ring_queue_test.cpp
 * @author: jameyli <jameyli AT tencent DOT com>
 * @date:   2013-09-11
 * @brief:  ring_queue_test
 */
#include "gtest/gtest.h"
#include <stdio.h>
#include "timeout_pool.h"

using namespace testing;
using namespace tnt;

class TimeoutPoolTest : public Test
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

TEST_F(TimeoutPoolTest, Simple)
{
    tnt::TimeoutPool pool;

    time_t now = time(NULL);

    EXPECT_EQ(1, pool.Add(now, 5));
    EXPECT_EQ(2, pool.AddRepeating(now, 10));
    EXPECT_EQ(3, pool.AddRepeating(now, 15));
    EXPECT_EQ(4, pool.Add(now, 20));

    pool.runInternal(now, true);
    pool.runInternal(now-30, true);

}

