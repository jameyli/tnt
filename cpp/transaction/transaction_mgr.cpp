/**
 * @file:   transaction_mgr.cpp
 * @author: jameyli <lgy AT live DOT com>
 * @brief:
 */

#include "transaction_mgr.h"
#include "app_frame.h"
#include "logging.h"

TransactionMgr::TransactionMgr()
{
    is_use_locker_ = false;
}

TransactionMgr::~TransactionMgr()
{
}

int TransactionMgr::InitTransactionMgr()
{
    FUNC_TRACE(0);

    int ret = 0;
    // TODO:要申请多少内存呢？这个定时器的接口真是。。。
    char* timer_pool_mem = new char[102400];
    ret = timer_pool_.Init(timer_pool_mem, 102400, 1);
    if (0 != ret)
    {
        TNT_LOG_ERROR(0, 0, "init timer_pool failed, ret=%d|%s",
                      ret, timer_pool_.GetErrMsg());

        return -1;
    }


    struct timeval t;
    gettimeofday(&t, NULL);
    trans_id_begin_ = (t.tv_sec % 3600) * 1000000  + t.tv_usec;

    return 0;
}

bool
TransactionMgr::CheckCmdIsRegistered(unsigned int cmd) const
{
    FUNC_TRACE(0);

    TransactionBucketMapConstIter iter = idle_transaction_map_.find(cmd);
    if (iter == idle_transaction_map_.end())
    {
        return false;
    }
    else
    {
        return true;
    }
}

// 处理消息
// 主要接口
int
TransactionMgr::ProcessAppFrame(const AppFrame& app_frame)
{
    FUNC_TRACE(0);

    if (!app_frame.CheckIsOk())
    {
        return -1;
    }

    app_frame.Dump();

    // int ret = 0;

    unsigned int uin = app_frame.app_header->uiUin;
    unsigned int cmd = app_frame.app_header->ushCmdID;
    unsigned int trans_id = app_frame.app_header->uiTransactionID;

    TNT_LOG_DEBUG(0, uin, "0X%08X|%u", cmd, trans_id);

    // TODO: 这里是应该先通过命令字查找呢还是先通过id查找？
    TransactionBase* ptrans = NULL;
    if (trans_id > 0)
    {
        ptrans = GetTransaction(trans_id);
        if (NULL == ptrans)
        {
            TNT_LOG_WARN(0, uin, "can not get active transaction|0X%08X|%u", cmd, trans_id);
            return -1;
        }
        else
        {
            // 已有的事务
            return ptrans->ProcessOtherFrame(app_frame);
        }
    }
    else
    {
        // 新的事务
        ptrans = GetNewTransaction(uin, cmd);

        if (NULL == ptrans)
        {
            TNT_LOG_WARN(0, uin, "can not get new transaction|0X%08X|%u", cmd, trans_id);
            return -1;
        }

        return ptrans->ProcessFirstFrame(app_frame);
    }

    return 0;
}

// 定时器接口
int TransactionMgr::SetTimer(unsigned int trans_id, time_t timeout_usec, size_t& timer_id)
{
    FUNC_TRACE(0);

    TransactionTimer timer;
    timer.trans_id = trans_id;

    long long unsigned int add_timer_id = 0;

    int ret = timer_pool_.AddTimer(timeout_usec, timer, &add_timer_id);
    if (0 != ret)
    {
        return -1;
    }

    timer_id = add_timer_id;

    return 0;
}

int TransactionMgr::CancelTimer(size_t timer_id)
{
    FUNC_TRACE(0);

    int ret = timer_pool_.DelTimer(timer_id);
    if (0 != ret)
    {
        return ret;
    }

    return 0;
}

// 处理超时
void TransactionMgr::HandleTimeout()
{
    std::vector<unsigned long long> timer_id_list;
    std::vector<TransactionTimer> timer_list;

    int ret = timer_pool_.GetTimer(timer_id_list, timer_list);
    if (0 != ret)
    {
        TNT_LOG_WARN(0, 0, "get timer failed, ret=%d, errmsg=%s",
                     ret, timer_pool_.GetErrMsg());
        return;
    }

    if (timer_id_list.size() > 0)
    {
        TNT_LOG_INFO(0, 0, "timeout num = %lu", timer_id_list.size());
    }

    for(unsigned int i=0; i<timer_id_list.size(); ++i)
    {
        unsigned int trans_id = timer_list[i].trans_id;
        TransactionBase* ptrans = GetTransaction(trans_id);
        if (NULL == ptrans)
        {
            continue;
        }

        ptrans->ProcessTimeout(timer_id_list[i]);
    }
}

// 统计信息
void TransactionMgr::CheckStatistic()
{
    FUNC_TRACE(0);

    TNT_LOG_INFO(0, 0, "bucket num = %lu", idle_transaction_map_.size());

    size_t idle_transaction_num = 0;

    TransactionBucketMapIter iter = idle_transaction_map_.begin();
    TransactionBucketMapIter iter_end = idle_transaction_map_.end();
    for (; iter != iter_end; ++iter)
    {
        TNT_LOG_INFO(0, 0, "bucket info|0X%08X|%lu",
                     iter->second->cmd(),
                     iter->second->size());

        idle_transaction_num += iter->second->size();
    }

    // bool is_equal = (active_transaction_map_.size() + idle_transaction_num) ==
    //     MAX_TRANSANCTION_NUM_PER_CMD * idle_transaction_map_.size();

    // if (!is_equal)
    // {
    //     TNT_LOG_ERROR(0, 0, "num is not equal|%lu|%lu",
    //                   active_transaction_map_.size(),
    //                   idle_transaction_num);
    // }

    TNT_LOG_INFO(0, 0, "statistic|%lu|%lu",
                 active_transaction_map_.size(),
                 idle_transaction_num);

    return;
}

TransactionBase*
TransactionMgr::GetNewTransaction(unsigned int uin, unsigned int cmd)
{
    FUNC_TRACE(0);

    int ret = LockUinTrans(uin, cmd);
    if (0 != ret && is_use_locker_)
    {
        return NULL;
    }

    TransactionBucketMapIter iter = idle_transaction_map_.find(cmd);
    if (iter == idle_transaction_map_.end())
    {
        TNT_LOG_WARN(0, 0, "Cmd is not Register|0X%08X", cmd);
        return NULL;
    }

    // 从idle中取出一个事务
    TransactionBase* ptrans = iter->second->pop();
    if (NULL == ptrans)
    {
        TNT_LOG_ERROR(0, 0, "idle transaction is not ehough, cmd = 0X%08X", cmd);
        return NULL;
    }

    // 初始化
    ptrans->ReConstructAll();

    // 放入活动列表
    active_transaction_map_[ptrans->id()] = ptrans;

    return ptrans;
}

TransactionBase* TransactionMgr::GetTransaction(unsigned int trans_id)
{
    FUNC_TRACE(0);

    TransactionMapIter iter = active_transaction_map_.find(trans_id);
    if (iter == active_transaction_map_.end())
    {
        TNT_LOG_WARN(0, 0, "trans id is not exist|%u", trans_id);
        return NULL;
    }
    else
    {
        iter->second->Dump();
        return iter->second;
    }
}

int
TransactionMgr::FreeTransaction(TransactionBase* ptrans)
{
    if (NULL == ptrans)
    {
        return -1;
    }

    FUNC_TRACE(ptrans->uin());
    ptrans->Dump();

    TransactionMapIter iter = active_transaction_map_.find(ptrans->id());
    if (iter == active_transaction_map_.end())
    {
        TNT_LOG_WARN(0, 0, "trans id is not exist|%u", ptrans->id());
        return -1;
    }
    else
    {
        TransactionBucketMapIter bucket_iter = idle_transaction_map_.find(ptrans->cmd());
        if (bucket_iter == idle_transaction_map_.end())
        {
            TNT_LOG_ERROR(0, 0, "Cmd is not Register|0X%08X", ptrans->cmd());
            return -2;
        }

        {
            bucket_iter->second->push(ptrans);
            active_transaction_map_.erase(iter);
        }

        UnLockUinTrans(ptrans->uin(), ptrans->cmd());

        ptrans->ReDestructAll();

        return 0;
    }
}

/**
 * @brief:添加一个锁，防止同一个用户相同请求同同时存在多个事务事例
 */
int TransactionMgr::LockUinTrans(unsigned int uin, unsigned int cmd)
{
    FUNC_TRACE(uin);
    TransactionLocker locker(uin, cmd);

    TransactionLockerPoolInserRet insert_ret = locker_pool_.insert(locker);

    if (false == insert_ret.second)
    {
        TNT_LOG_WARN(0, uin, "trans lock failed|%u|0X%08X", uin, cmd);
        return -1;
    }

    return 0;
}

void TransactionMgr::UnLockUinTrans(unsigned int uin, unsigned int cmd)
{
    FUNC_TRACE(uin);
    TransactionLocker locker(uin, cmd);

    locker_pool_.erase(locker);

    return;
}


