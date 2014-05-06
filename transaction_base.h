/**
 * @file:   tnt_transaction_base.h
 * @author: jameyli <lgy AT live DOT com>
 * @brief:  事务(或会话, 不过会话一般是指一次连接，而这里其实是一次请求)的基类
 */

#ifndef TNT_TRANSACTION_BASE_H
#define TNT_TRANSACTION_BASE_H

#include "tnt_app_frame.h"

/**
 * @brief:  事务类
 * 事务由事务管理类管理, 没有公开其接口
 */
class TransactionBase
{
    friend class TransactionMgr;

protected:
    /**
     * @brief:  事务的状态
     */
    enum TransactionState
    {
        STATE_IDLE = 0,     /// 空闲
        STATE_AWAKE = 1,    /// 唤醒
        STATE_ACTIVE = 2,   /// 活动
        STATE_TIMEOUT = 3,  /// 超时
    };

    enum TransactionReturn
    {
        RETURN_WAIT = 1,        /// 等待, 可能异步等待另一个服务器消息的返回或超时
        RETURN_CONTINUE  = 2,   /// 继续，再次调用
        RETURN_EXIT  = 3,       /// 退出，此时需要回收事务
    };

    enum TransactionWaitInterval
    {
        WAIT_ONE_SECONDS = 1000,
        WAIT_TWO_SECONDS = 2000,
        WAIT_THREE_SECONDS = 3000,
        WAIT_FOUR_SECONDS = 4000,
        WAIT_FIVE_SECONDS = 5000,

        WAIT_TEN_SECONDS = 10000,

        WAIT_FIFTEEN_SECONDS = 15000,
    };

protected:
    TransactionBase(unsigned int cmd);
    virtual ~TransactionBase() = 0;

protected:
    // // 子类生成自己的实例，返回基类指针，类似抽象工厂模式
    // virtual TransactionBase* CreateTransaction(unsigned int cmd) = 0;

    // 重新构造
    // 因为对象是一直存在的，所以再次使用需要初始化
    // 另外由于子类数据也有可能需要初始化, 为了避免子类重载该函数后
    // 忘记调用基类的方法，所以将基类和子类的函数分开
    void ReConstructBase();

    // 提供给子类的接口
    virtual void ReConstruct();

    // 封装一下
    void ReConstructAll();

    void ReDestructBase();

    virtual void ReDestruct();

    void ReDestructAll();

    //XXX: 这几个接口返回值 小于 0 都是退出事务
    // 收到第一个消息
    // 第一个消息比较特殊，用于创建事务时做一些初始化
    int ProcessFirstFrame(const AppFrame& app_frame);

    // 收到其他消息
    int ProcessOtherFrame(const AppFrame& app_frame);

    int SetTimeoutTimer(TransactionWaitInterval interval_usec);
    int CancelTimeoutTimer();
    int ProcessTimeout(size_t timer_id);

    /**
     * @brief:  进入下一个阶段
     *
     * @param  phase 阶段ID
     * @param  interval_usec 超时时间
     * @param  waiting_cmd 进入的阶段要等待的CMD
     */
    void EnterPhase(unsigned int phase,
                    TransactionWaitInterval interval_usec,
                    unsigned int waiting_cmd = 0);


protected:
    /**
     * @brief:  状态机输入
     *
     * @return: < 0 退出事务
     */
    int OnEvent();

    // 需要子类实现的几个接口
    // 第一次被调用
    virtual TransactionReturn OnAwake() = 0;

    // 激活状态再次调用
    virtual TransactionReturn OnActive(){return RETURN_EXIT;}

    // 超时被调用
    virtual TransactionReturn OnTimeout(){return RETURN_EXIT;}

public:
    inline unsigned int id() const
    {
        return id_;
    }

    inline unsigned int uin() const
    {
        return uin_;
    }

    inline void set_uin(unsigned uin)
    {
        uin_ = uin;
    }
    /**
     * @brief:  获取事务注册的CMD
     */
    inline unsigned int cmd() const
    {
        return cmd_;
    }

    inline const TransactionState& state() const
    {
        return state_;
    }

    inline unsigned int phase() const
    {
        return phase_;
    }

    inline void SetPhase(unsigned int phase)
    {
        phase_ = phase;
        curr_cmd_ = 0;
    }


    /**
     * @brief:  当前命令
     */
    inline unsigned int GetCurrCmd() const
    {
        return curr_cmd_;
    }

    inline void SetCurrCmd(unsigned int curr_cmd)
    {
        curr_cmd_ = curr_cmd;
    }



    inline const AppFrame& GetAppFrame() const
    {
        return app_frame_;
    }

    inline const BusHeader& GetBusHeader() const
    {
        return bus_header_;
    }

    inline const AppHeader& GetAppHeader() const
    {
        return app_header_;
    }

    virtual void Dump() const
    {
        // TNT_LOG_DEBUG(0, uin_, "TransactionBase|%u|%u|%u", id_, cmd_, state_);
    }

private:
    // 事务ID
    unsigned int id_;
    // 请求命令字
    unsigned int cmd_;
    // 请求者uin
    unsigned int uin_;
    // 状态
    TransactionState state_;
    // 阶段, 用于控制下一步行为
    unsigned int phase_;
    // 当前的CMD
    // 如果是等待状态，则是等待的CMD
    // 如果已经进入，则是当前正在执行的CMD
    unsigned int curr_cmd_;

    // 超时计时器
    size_t timeout_timer_id_;

    AppFrame app_frame_;

    snslib::BusHeader bus_header_;
    snslib::AppHeader app_header_;
};

#endif //TNT_TRANSACTION_BASE_H

