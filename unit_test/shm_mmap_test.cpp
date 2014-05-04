#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <stdio.h>
#include <iostream>

#include "shm_mmap.h"

using namespace testing;
using namespace tnt;
using namespace std;

class ShmMmapTest : public Test
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

class TestClass
{
public:
    int a;
    int b;
    int c;
    int d;
};

TEST_F(ShmMmapTest, Init)
{
    ShmMmap sm;

    int ret = sm.Open("test.mmap", 1024);
    std::cout<< ret<< std::endl;

    EXPECT_TRUE(ret == 0);

    EXPECT_TRUE(NULL != sm.addr());

    std::cout<<sm.addr() << std::endl;

    TestClass* t = new(sm.addr()) TestClass();
    t->a = 1;
    t->b = 3;
    t->c = 5;
    t->d = 7;
}

