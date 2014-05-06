/**
 * @file:   tnt_transaction_base.cpp
 * @author: jameyli <lgy AT live DOT com>
 * @brief:
 */

#include "tnt_transaction_base.h"
#include "tnt_transaction_mgr.h"

TransactionBase::TransactionBase(unsigned int cmd)
:   id_(0),
    cmd_(cmd),
    uin_(0),
    state_(STATE_IDLE),
    phase_(0),
    curr_cmd_(0),
    timeout_timer_id_(0),
    app_frame_(NULL, 0)
{
}

TransactionBase::~TransactionBase()
{
}

// // 子类生成自己的实例，返回基类指针，类似抽象工厂模式
// virtual TransactionBase* CreateTransaction(unsigned int cmd) = 0;

// 重新构造
// 因为对象是一直存在的，所以再次使用需要初始化
// 另外由于子类数据也有可能需要初始化, 为了避免子类重载该函数后
// 忘记调用基类的方法，所以将基类和子类的函数分开
void TransactionBase::ReConstructBase()
{
    FUNC_TRACE(uin_);

    static unsigned int trans_id_generator_ =
        TransactionMgrSigleton::get_const_instance().trans_id_begin_;


    ++trans_id_generator_;
    if (0 == trans_id_generator_)
    {
        ++trans_id_generator_;
    }

    id_ = trans_id_generator_;

    state_ = STATE_AWAKE;

    TNT_LOG_DEBUG(0, 0, "%u|%u", id_, trans_id_generator_);

    return;
}

// 提供给子类的接口
void TransactionBase::ReConstruct()
{
}

// 封装一下
void TransactionBase::ReConstructAll()
{
    FUNC_TRACE(uin_);

    ReConstructBase();
    ReConstruct();
}

void TransactionBase::ReDestructBase()
{
    FUNC_TRACE(uin_);

    id_ = 0;
    uin_ = 0;
    phase_ = 0;
    curr_cmd_ = 0;
    timeout_timer_id_ = 0;
}

void TransactionBase::ReDestruct()
{
}

void TransactionBase::ReDestructAll()
{
    FUNC_TRACE(uin_);

    ReDestruct();
    ReDestructBase();
}

//XXX: 这几个接口返回值 小于 0 都是退出事务
// 收到第一个消息
// 第一个消息比较特殊，用于创建事务时做一些初始化
int TransactionBase::ProcessFirstFrame(const AppFrame& app_frame)
{
    FUNC_TRACE(uin_);

    app_frame_ = app_frame;
    memcpy(&bus_header_, app_frame.bus_header, sizeof(bus_header_));
    memcpy(&app_header_, app_frame.app_header, sizeof(app_header_));
//    bus_header_ = app_frame.bus_header;
//    app_header_ = app_frame.app_header;

    uin_ = app_header_.uiUin;

    return OnEvent();
}

// 收到其他消息
int TransactionBase::ProcessOtherFrame(const AppFrame& app_frame)
{
    FUNC_TRACE(uin_);

    // 检查是否是期望的CMD
    // 为了和之前的兼容，这里如果没有设置curr_cmd_，则设置一下
    if (GetCurrCmd() == 0)
    {
        SetCurrCmd(app_frame.app_header->ushCmdID);
    }
    else if (GetCurrCmd() != app_frame.app_header->ushCmdID)
    {
        TNT_LOG_WARN(0, uin_, "not waiting cmd|0X%08X|0X%08X",
                     GetCurrCmd(), app_frame.app_header->ushCmdID);

        return -1;
    }

    app_frame_ = app_frame;
    memcpy(&bus_header_, app_frame.bus_header, sizeof(bus_header_));
    memcpy(&app_header_, app_frame.app_header, sizeof(app_header_));

    return OnEvent();
}

int TransactionBase::SetTimeoutTimer(TransactionWaitInterval interval_usec)
{
    FUNC_TRACE(uin_);

    SetCurrCmd(0);

    CancelTimeoutTimer();

    int ret = TransactionMgrSigleton::get_mutable_instance().SetTimer(id_, interval_usec, timeout_timer_id_);
    if (0 != ret)
    {
        TNT_LOG_ERROR(0, uin_, "set timer error|%u", id_);
        return -1;
    }

    return 0;
}

int TransactionBase::CancelTimeoutTimer()
{
    FUNC_TRACE(uin_);

    if (0 == timeout_timer_id_)
    {
        return 0;
    }

    int ret = TransactionMgrSigleton::get_mutable_instance().CancelTimer(timeout_timer_id_);
    if (0 != ret)
    {
        TNT_LOG_ERROR(0, uin_, "cancel timer error|%u|%lu", id_, timeout_timer_id_);
        return -1;
    }

    timeout_timer_id_ = 0;

    return 0;
}

int TransactionBase::ProcessTimeout(size_t timer_id)
{
    FUNC_TRACE(uin_);

    if (timeout_timer_id_ == timer_id)
    {
        timeout_timer_id_ = 0;

        state_ = STATE_TIMEOUT;
    }

    return OnEvent();
}

/**
 * @brief:  进入下一个阶段
 *
 * @param  phase 阶段ID
 * @param  interval_usec 超时时间
 * @param  waiting_cmd 进入的阶段要等待的CMD
 */
void TransactionBase::EnterPhase(unsigned int phase,
                                 TransactionWaitInterval interval_usec,
                                 unsigned int waiting_cmd)
{
    FUNC_TRACE(uin_);

    SetPhase(phase);
    SetTimeoutTimer(interval_usec);

    SetCurrCmd(waiting_cmd);
}


/**
 * @brief:  状态机输入
 *
 * @return: < 0 退出事务
 */
int TransactionBase::OnEvent()
{
    FUNC_TRACE(uin_);

    // 取消定时器
    CancelTimeoutTimer();

    int ret = 0;
    TransactionReturn trans_ret = RETURN_EXIT;

    // 2012-06-15
    // 决定在这里加个异常扑捉，虽然不建议在代码中使用异常
    // 但是确实没有想到比异常更合适的办法
    // 因为子类是未知的，子类的实现也是未知的
    // 无法保证每一个实现都是严谨的
    //
    // 不过发生异常时是应该让程序继续呢还是果断退出?
    //
    try
    {
        switch (state_)
        {
            case STATE_AWAKE:
                {
                    trans_ret = OnAwake();
                    break;
                }
            case STATE_ACTIVE:
                {
                    trans_ret = OnActive();
                    break;
                }
            case STATE_TIMEOUT:
                {
                    trans_ret = OnTimeout();
                    break;
                }
            default:
                {
                    TNT_LOG_ERROR(0, uin_, "error state %u", state_);
                }
        }
    }
    catch (std::exception& e)
    {
        TNT_LOG_ERROR(0, uin_, "exception|0X%08X|%u|%u|%u|%s",
                      cmd_, id_, state_, phase_, e.what());

        // 让这个事务退出吧。。。
        trans_ret = RETURN_EXIT;
    }

    switch (trans_ret)
    {
        case RETURN_WAIT:
            {
                if (0 == timeout_timer_id_)
                {
                    //2013-08-19, jamey
                    //进入等待却又忘了设置时间, 还是让其退出吧
                    state_ = STATE_IDLE;

                    TNT_LOG_DEBUG(0, uin(), "no timer trans exit|%u|%u", id(), cmd());
                    TransactionMgrSigleton::get_mutable_instance().FreeTransaction(this);
                }
                else
                {
                    state_ = STATE_ACTIVE;
                }
                break;
            }
        case RETURN_CONTINUE:
            {
                state_ = STATE_ACTIVE;

                return OnEvent();
            }
        case RETURN_EXIT:
            {
                state_ = STATE_IDLE;

                TNT_LOG_DEBUG(0, uin(), "trans exit|%u|%u", id(), cmd());
                TransactionMgrSigleton::get_mutable_instance().FreeTransaction(this);
//                ret = -1;

                break;
            }
    }

    return ret;
}


