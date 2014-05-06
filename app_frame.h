/**
 * @file:   tnt_app_frame.h
 * @author: jameyli <lgy AT live DOT com>
 * @modified: 添加对于tcaplus消息帧的处理逻辑 jafterwang 2014-4-11
 * @brief:
 */

#ifndef TNT_APP_FRAME_H
#define TNT_APP_FRAME_H

#include "api/include/sns_protocol.h"
using snslib::BusHeader;
using snslib::AppHeader;

class AppFrame
{
public:
    // AppFrame()
    // {
    // }

    AppFrame(char* buff, int buff_len)
        : buff_(buff), buff_len_(buff_len)
    {
        bus_header = (BusHeader*)buff_;
        app_header = (AppHeader*)(buff_ + sizeof(BusHeader));
        data = buff_ + sizeof(BusHeader) + sizeof(AppHeader);
        data_len = buff_len_ - (sizeof(BusHeader) + sizeof(AppHeader));
        m_bBus = true;
    }

    AppFrame(char* pUserBuff, int nUserBuffLen, void *pRsp)
        : buff_(pUserBuff), buff_len_(nUserBuffLen)
    {
        bus_header = (BusHeader*)buff_;
        app_header = (AppHeader*)(buff_ + sizeof(BusHeader));
        data = buff_ + sizeof(BusHeader) + sizeof(AppHeader);
        data_len = buff_len_ - (sizeof(BusHeader) + sizeof(AppHeader));

        pTcaplusRsp = pRsp;
        m_bBus = false;
    }

    ~AppFrame()
    {
    }

    bool CheckIsOk() const
    {
        if (NULL == bus_header || NULL == app_header)
        {
            return false;
        }

        if (m_bBus)
        {
            if (NULL == data || data_len < 0)
            {
                return false;
            }
        }
        // tcaplus对应的data是有可能是空的
        else
        {
            if (NULL == pTcaplusRsp)
            {
                return false;
            }
        }

        return true;
    }

    void Dump() const
    {
        if (!CheckIsOk())
        {
            return;
        }

        TNT_LOG_DEBUG(0, 0, "AppFrame BusHeader|%u|%u|%u|%u|%u",
                      bus_header->uiSrcID,
                      bus_header->uiDestID,
                      bus_header->uiRouterID,
                      bus_header->uiTTL,
                      bus_header->uiClientPos);

        TNT_LOG_DEBUG(0, 0, "AppFrame AppHeader|%u|%u|%u|%u|%u|%u|%u",
                      app_header->uiLength,
                      app_header->uiUin,
                      app_header->uiIP,
                      app_header->ushCmdID,
                      app_header->ushSrcSvrID,
                      app_header->ushDestSvrID,
                      app_header->uiTransactionID);
    }

public:
    BusHeader* bus_header;
    AppHeader* app_header;
    char* data;
    int data_len;
    void *pTcaplusRsp;      // 用于记录tcaplus返回的消息指针

private:
    char* buff_;
    int buff_len_;
    bool m_bBus;            // true表示bus来的消息 false表示来自tcaplus的消息
};


#endif //TNT_TRANSACTION_BASE_H
