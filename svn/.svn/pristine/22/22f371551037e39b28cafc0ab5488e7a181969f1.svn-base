/*****************************************************************************
CAPTION
Project Name: 归巢系统--数据采集模块
Description : 主服务线程池程序
File Name   : c_PigeonLocation_server.h
Requires    : LinuxAS3 , Oracle 9i
Create Date : 2016-05-04
******************************************************************************/

#ifndef C_PigeonLocation_SERVER_H
#define C_PigeonLocation_SERVER_H
//---------------------------------------------------------------------------
#include "c_server_thread_pool.h"
class TCPigeonLocationServer : public TCServerThreadPool
{
public:
    TCPigeonLocationServer(){}
    TCPigeonLocationServer(TCString sHostName, TCString sService,
                       long nPort, long nQueue,
                       long nMinThread, long nMaxThread);
    ~TCPigeonLocationServer();
    TCThreadServerSock * GetNewThreadSever(int nSock);
    void DestroyAThreadServer(TCThreadServerSock *pServerSock);
};
#endif
