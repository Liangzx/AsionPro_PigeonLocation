/*****************************************************************************
CAPTION
Project Name: 归巢系统--数据采集模块
Description : 日志类
File Name   : c_PigeonLocation_log.cpp
Requires    : LinuxAS3 , Oracle 9i
Create Date : 2016-05-04
******************************************************************************/

#include "c_PigeonLocation_log.h"

extern TCPigeonLocationConfig* gPigeonLocationConfig;

//----------------------------------------------------------
TCCriticalSection TCPigeonLocationLog::csLog;

//==========================================================================
// 函数 : TCPigeonLocationLog::AddPigeonLocationRunLog()
// 用途 : 写运行日志
// 原型 : AddPigeonLocationRunLog(const TCString& sLog)
// 参数 : sLog -- 要写入日志的内容
// 返回 : 无
// 说明 : 
//==========================================================================
void TCPigeonLocationLog::AddPigeonLocationRunLog(const TCString& sLog)
{
    csLog.Enter();
    try
    {
        TCString strFileName;
        TCString sLogPath;
        TCString strAppName;
        TCString strBuff;
        TCFileStream cFile;
        strAppName = Application.GetAppName();
        sLogPath = TAppPath::AppLog() + strAppName;
        ForceDirectories(sLogPath);
        strFileName = TCAppLog::GetDailyLogFileNameWithTag(Application.GetProcessFlag());
        cFile.Open(strFileName,omAppend);
        strBuff = TCTime::Now()+": ";
        strBuff += sLog;
        cFile.WriteLn(strBuff);
        cFile.Close();
    }
    catch(TCException& e)
    {
    	printf("\nLog Exception:%s",(char*)e.GetExceptionMessage());
    }
    csLog.Leave();
}

//==========================================================================
// 函数 :TCPigeonLocationLog::AddPigeonLocationRunLog
// 用途 : 写运行日志
// 原型 : AddPigeonLocationRunLog(char *sLog)
// 参数 : sLog -- 要写入日志的内容
// 返回 : 无
// 说明 : 
//==========================================================================
void TCPigeonLocationLog::AddPigeonLocationRunLog(char* sLog)
{
    AddPigeonLocationRunLog(TCString(sLog));
}

//==========================================================================
// 函数 :TCPigeonLocationLog::DebugLog
// 用途 : 写运行日志
// 原型 : DebugLog(char *sLog)
// 参数 : sLog -- 要写入日志的内容
// 返回 : 无
// 说明 : 
//==========================================================================
void TCPigeonLocationLog::DebugLog(char* sLog)
{
    if(gPigeonLocationConfig->IsDebug())
    	AddPigeonLocationRunLog(TCString(sLog));
}

//==========================================================================
// 函数 :TCPigeonLocationLog::DebugLog
// 用途 : 写运行日志
// 原型 : DebugLog(const TCString& sLog)
// 参数 : sLog -- 要写入日志的内容
// 返回 : 无
// 说明 : 
//==========================================================================
void TCPigeonLocationLog::DebugLog(const TCString& sLog)
{
    if(gPigeonLocationConfig->IsDebug())
    	AddPigeonLocationRunLog(sLog);
}

