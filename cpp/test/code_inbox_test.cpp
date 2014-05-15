#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <stdio.h>
#include "code_inbox.h"
#include <iostream>

using namespace testing;
using namespace tnt;
using namespace std;

class CodeInboxTest : public Test
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

TEST_F(CodeInboxTest, bit_test)
{
    char a = 0X0000;
    int b = 0X0001;
//    std::size_t c = 0X1000;


    tnt::set_bit(a, b);
    printf("%08X\n", a);

    ASSERT_TRUE(tnt::bit_test_true(a, b));

//    tnt::clr_bit<>(a, b);
//    printf("%08X\n", a);
//    ASSERT_TRUE(tnt::bit_test_true(a, b));

//    ASSERT_TRUE(tnt::bit_test_false(a, b));
//    ASSERT_TRUE(tnt::bit_test_true(a, c));
//    ASSERT_TRUE(tnt::bit_test_false(a, c));
}

