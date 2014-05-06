/**
 * @file:   application_test.cpp
 * @author: jameyli <jameyli AT tencent DOT com>
 * @date:   2013-09-19
 * @brief:
 */

#include <gtest/gtest.h>
#include "gmock/gmock.h"
#include <stdio.h>
#include "application_base.h"
#include <iostream>

using namespace testing;
using namespace tnt;
using namespace std;

class MyApplication : public ApplicationBase
{
    virtual int OnInit(const char* conf_file)
    {
        cout << "Init" << std::endl;
        return 0;
    }
    virtual int OnTick()
    {
        cout << "Tick" << std::endl;
        return 0;
    }
    virtual int OnReload()
    {
        cout << "Reload" << std::endl;
        return 0;
    }
    virtual int OnProc()
    {
//        cout << "Proc" << std::endl;
//        if (time)
        return 0;
    }
    virtual int OnIdle()
    {
        cout << "Idle" << std::endl;
        return 0;
    }
    virtual int OnExit()
    {
        cout << "Exit" << std::endl;
        return 0;
    }
};

class ApplicationTest : public Test
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

//TEST_F(ApplicationTest, Init)
//{
//    MyApplication app;
//
//    int argc = 0;
//    char** argv = NULL;
//    app.SetVersion(0, 0, 0, 0);
//    app.Init(argc, argv);
//    app.Run();
//}
//
