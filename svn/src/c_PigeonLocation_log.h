/*****************************************************************************
CAPTION
Project Name: �鳲ϵͳ--���ݲɼ�ģ��
Description : ��־��
File Name   : c_PigeonLocation_log.h
Requires    : LinuxAS3 , Oracle 9i
Create Date : 2016-05-04
******************************************************************************/

#ifndef C_PigeonLocation_LOG_H
#define C_PigeonLocation_LOG_H

#include "cmpublic.h"
#include "c_critical_section.h"
#include "c_PigeonLocation_config.h"

class TCPigeonLocationLog
{
public:
        static TCCriticalSection csLog;
public:
        void AddPigeonLocationRunLog(const TCString&);        
        void AddPigeonLocationRunLog(char*);
        void DebugLog(const TCString&);
        void DebugLog(char*);
};


#endif
