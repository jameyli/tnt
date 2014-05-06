/**
 * @file:   tnt_transaction_mgr.h
 * @author: jameyli <lgy AT live DOT com>
 * @brief:  事务管理器
 *
 * 事务Transaction, 产生的需求是:一次请求需要多步或异步处理时，
 * 需要缓存上下文
 *
 * 没有使用会话Session的名称, 是一般session好像表示的连接，而且一个
 * session可能会有多个事务.
 *
 * 由于不同命令字的处理逻辑不一样，需要缓存的数据也不一样，所以
 * 一般不同的命令字请求是不同的事务类类型, 这样命令字其实和事务
 * 类型是等价的
 *
 * 命令字需要注册才会有事务进行处理
 *
 * 一次请求会独占一个事务实例.
 *
 * 一个命令字注册后会在服务器初始化时为其分配一个事务桶，桶的大小是统一的，有一
 * 个宏来定义，暂时不支持动态调整。因此，这里会有一个潜在的问题，频率较高的请求
 * (命令)可能会因为没有空闲的事务而无法处理.解决这个问题有几个方法:
 *      1 更改宏值统一提高桶的大小, 可能会造成资源浪费
 *      2 程序运行中动态改变桶的大小, 可能会导致资源分配不可控
 *      3 将事务按请求频率分类，设置不同的上限
 *      4 组合
 *2012-03-01 优化一下桶的大小
 *先区分同步与异步的
 *同步操作，只需要一个就够了
 *
 * XXX:是否需要持久化，能够做到当然更好，但在实现时却引入许多
 * 细节需要考虑，权衡后确定第一个版本先不考虑持久化
 *
 * TODO: 什么时候回收使用完的事务?
 * 1 通过事务接口内部设置状态
 * 这样子类就需要在函数退出时设置
 * 2 通过事务接口的返回值
 * 子类只需要返回统一的返回值
 * 似乎2 更简单一些
 *
 * TODO:
 * 1 定时器, 事务肯定需定时逻辑
 *   事务的定时器由事务管理器单独来维护
 * 2 统一的消息发送接口
 *
 */

#ifndef TNT_TRANSACTION_MGR_H
#define TNT_TRANSACTION_MGR_H

#include <vector>
#include <tr1/unordered_map>
#include <tr1/unordered_set>
#include "boost/serialization/singleton.hpp"
#include "comm/timer_pool/timer_pool.h"
#include "tnt_transaction_base.h"

enum TransctionMode
{
    TRANSCTION_MODE_SYN = 0,
    TRANSCTION_MODE_ASY = 1,

    TRANSCTION_MODE_COUNT
};

/**
 * @brief: 事务桶
 * 当前空闲的统一类型事务
 * 命令字注册后会为其分配一个唯一的桶
 *
 */
class TransctionBucket
{
public:
    TransctionBucket(unsigned int cmd)
        : cmd_(cmd)
    {
        trans_list_.clear();
    }

    ~TransctionBucket()
    {
    }

public:
    unsigned int cmd() const
    {
        return cmd_;
    }

    int push(TransactionBase* ptrans)
    {
        FUNC_TRACE(0);

        trans_list_.push_back(ptrans);
        dump();

        return 0;
    }

    TransactionBase* pop()
    {
        FUNC_TRACE(0);

        dump();

        if (trans_list_.size() <= 0)
        {
            LOG_WARN(0, 0, "there is no free transaction|%u", cmd_);
            return NULL;
        }

        TransactionBase* ptrans = trans_list_.back();
        trans_list_.pop_back();

        return ptrans;
    }

    size_t size() const
    {
        return trans_list_.size();
    }

    void dump() const
    {
        LOG_DEBUG(0, 0, "TransctionBucket|%u|%lu", cmd_, trans_list_.size());
    }

private:
    std::vector<TransactionBase*> trans_list_;

private:
    unsigned int cmd_;
};

typedef struct tagTransactionTimer
{
    unsigned int trans_id;
}TransactionTimer;

// 一个用户同时只能有一个处理特定命令的事务
class TransactionLocker
{
public:
    TransactionLocker(unsigned int uin, unsigned cmd)
    : uin_(uin), cmd_(uin)
    {
    }
    ~TransactionLocker(){}

    unsigned int uin_;
    unsigned int cmd_;
};

class HashOfTransactionLocker
{
public:
    size_t operator() (const TransactionLocker& locker) const
    {
        return (size_t(locker.uin_) +  locker.cmd_);
    }
};

class EqualOfTransactionLocker
{
public:
    size_t operator() (const TransactionLocker& rhs, const TransactionLocker& lhs) const
    {
        return ((rhs.uin_ == lhs.uin_) && (rhs.cmd_ == lhs.cmd_));
    }
};



/**
 * @brief:  事务管理器
 * 负责管理资源事务、事务桶, 如果内存泄漏，就是这里的原因
 */
class TransactionMgr
{
    friend class TransactionBase;

    static const unsigned int MAX_SYN_TRANSANCTION_NUM_PER_CMD = 1;
    static const unsigned int MAX_ASY_TRANSANCTION_NUM_PER_CMD = 1024;

protected:
    TransactionMgr();
    ~TransactionMgr();

public:
    int InitTransactionMgr();

    /**
     * @brief: 注册命令
     *
     * 初始化时调用，会涉及内存分配
     * 刚开始是用继承的多态来实现
     * int RegisterCommand(unsigned int cmd, TransactionBase& trans)
     * 但发现用模版也能实现，而且子类也不需要实现抽象工厂
     *
     * @tparam ConcreteTransactionType 处理命令的具体事务类
     * @param  cmd 需要注册的命令
     *
     * @return: 0 成功， 非0 失败
     */
    template<typename ConcreteTransactionType>
    int RegisterCommand(unsigned int cmd, TransctionMode tm = TRANSCTION_MODE_ASY);


    /**
     * @brief:  检查命令是否已经注册
     *
     * @param  cmd 命令字
     *
     * @return: 检查结果
     */
    bool CheckCmdIsRegistered(unsigned int cmd) const;

    // 主要接口
    /**
     * @brief: 处理网络消息
     *
     * @param  app_frame 网络消息帧
     *
     * @return: 处理结果
     *          0   成功
     *          非0 失败
     */
    int ProcessAppFrame(const AppFrame& app_frame);

    /**
     * @brief: 检查超时, 由主循环定时检查
     */
    void HandleTimeout();

    /**
     * @brief: 统计信息
     */
    void CheckStatistic();

    inline void SetUseLocker()
    {
        is_use_locker_ = true;
    }

private:
    // 定时器接口
    int SetTimer(unsigned int trans_id, time_t timeout_usec, size_t& timer_id);
    int CancelTimer(size_t timer_id);

    // 获得一个新的事务实例
    TransactionBase* GetNewTransaction(unsigned int uin, unsigned int cmd);
    // 取出已有的事务
    TransactionBase* GetTransaction(unsigned int trans_id);

    // 释放事务实例
    int FreeTransaction(TransactionBase* ptrans);

    /**
     * @brief:添加一个锁，防止同一个用户相同请求同同时存在多个事务事例
     */
    int LockUinTrans(unsigned int uin, unsigned int cmd);
    void UnLockUinTrans(unsigned int uin, unsigned int cmd);

public:
    // 事务ID初始值
    unsigned int trans_id_begin_;

private:
    // 当前激活的事务列表，用map方便根据id查找
    typedef std::tr1::unordered_map<unsigned int /*trans_id*/, TransactionBase*> TransactionMap;
    typedef TransactionMap::value_type TransactionMapValueType;
    typedef TransactionMap::iterator TransactionMapIter;
    typedef TransactionMap::const_iterator TransactionMapConstIter;
    typedef std::pair<TransactionMapIter, bool> TransactionMapInserRet;

    TransactionMap active_transaction_map_;

    // 应该还有个空闲池, 根据命令字查找
    typedef std::tr1::unordered_map<unsigned int /*cmd*/, TransctionBucket*> TransctionBucketMap;
    typedef TransctionBucketMap::value_type TransactionBucketMapValueType;
    typedef TransctionBucketMap::iterator TransactionBucketMapIter;
    typedef TransctionBucketMap::const_iterator TransactionBucketMapConstIter;
    typedef std::pair<TransactionBucketMapIter, bool> TransactionBucketMapInserRet;

    TransctionBucketMap idle_transaction_map_;

    // 定时器
    snslib::CTimerPool<TransactionTimer> timer_pool_;

    // 事务锁
    typedef std::tr1::unordered_set<TransactionLocker, HashOfTransactionLocker, EqualOfTransactionLocker> TransactionLockerPool;
    typedef TransactionLockerPool::value_type TransactionLockerPoolValueType;
    typedef TransactionLockerPool::iterator TransactionLockerPoolIter;
    typedef TransactionLockerPool::const_iterator TransactionLockerPoolConstIter;
    typedef std::pair<TransactionLockerPoolIter, bool> TransactionLockerPoolInserRet;


    bool is_use_locker_;
    TransactionLockerPool locker_pool_;
};

typedef boost::serialization::singleton<TransactionMgr> TransactionMgrSigleton;



/**
 * @brief: 注册命令
 *
 * 初始化时调用，会涉及内存分配
 * 刚开始是用继承的多态来实现
 * int RegisterCommand(unsigned int cmd, TransactionBase& trans)
 * 但发现用模版也能实现，而且子类也不需要实现抽象工厂
 *
 * @tparam ConcreteTransactionType 处理命令的具体事务类
 * @param  cmd 需要注册的命令
 *
 * @return: 0 成功， 非0 失败
 */
template<typename ConcreteTransactionType> int
TransactionMgr::RegisterCommand(unsigned int cmd, TransctionMode tm)
{
    FUNC_TRACE(0);

    LOG_DEBUG(0, 0, "cmd = 0X%08X", cmd);

    TransactionBucketMapIter iter = idle_transaction_map_.find(cmd);
    if (iter != idle_transaction_map_.end())
    {
        LOG_ERROR(0, 0, "Cmd Register again|0X%08X", cmd);
        return -1;
    }

    // 创建一个新的桶
    unsigned int trans_num = (tm == TRANSCTION_MODE_SYN) ?
        MAX_SYN_TRANSANCTION_NUM_PER_CMD : MAX_ASY_TRANSANCTION_NUM_PER_CMD;

    TransctionBucket* trans_bucket = new TransctionBucket(cmd);
    for (unsigned int i=0; i<trans_num; ++i)
    {
        TransactionBase* ptrans = new ConcreteTransactionType(cmd);
        if (NULL == ptrans)
        {
            TNT_LOG_ERROR(0, 0, "CreateTransaction failed|0X%08X", cmd);
            return -2;
        }

        trans_bucket->push(ptrans);
    }

    idle_transaction_map_[cmd] = trans_bucket;

    return 0;
}

#endif //TNT_TRANSACTION_MGR_H
