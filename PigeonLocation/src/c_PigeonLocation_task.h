/*****************************************************************************
CAPTION
Project Name: �鳲ϵͳ--���ݲɼ�ģ��
Description : ����Э���ɷ�����Ӧ��Э�鴦����
File Name   : c_PigeonLocation_task.cpp
Requires    : LinuxAS3 , Oracle 9i
Create Date : 2016-05-04
****************************************************************************/

#ifndef C_PigeonLocation_TASK_H
#define C_PigeonLocation_TASK_H
//---------------------------------------------------------------------------
#include "cmpublic.h"
#include "cmpublic_net.h"
#include "c_PigeonLocation_config.h"
#include "c_PigeonLocation_Public.h"
#include "c_PigeonLocation_PigeonHandler.h"
#include "c_PigeonLocation_Handle.h"
#include "c_PigeonLocation_log.h"
#include "c_PigeonLocation_HomingPosHandler.h"
#include "c_PigeonLocation_PigeonGatherHandler.h"
#include "c_PigeonLocation_VehicleHandler.h"


#include <map>
#include <vector>

using namespace std;

class TCPigeonLocationTask : public TCThreadServerSock
{
private:
    long  m_nTimeOut;
    long  m_nProcessTime;
    
    long m_nCmd_Index;
    
    bool nLogined_Status;
    
    TCString m_sDev_ID;
    TCString m_sCust_ID;
    TCString m_sImei;
    TCString m_sImsi;

		//: ��¼����󣬿ͻ���δ��Ӧ�Ĳ�������Լ��������ݣ�
		map<TCString, TCString> m_msOperID_PkgContent;
    
    TCString  m_sRecvTime;
    TCString  m_sSendTime;
    
    //: �ն��ϱ�IP��ַ��
    TCString sTacker_Send_IPAddress;
	
		//: ����ͷ��ǰ׺ CWT
		TCString m_sPkg_Head_Flag;
		//: ����ͷ�����ݵĳ���
		long m_nContent_Len;

		//: �������������������
		TCString m_sPkg_Content;

		//: ��������������������������
		TCString m_sReq_Command;
		
		//: ����һ�ν��� ������ & �ָ������ ����ֶ��б�
		TCStringList m_lsContentFieldList;
		
		//: �������ݴ�������Ӧ������
		TCString m_sRespCommand;
		
		//: �������ݴ�������Ӧ��������
		TCString m_sRespPkgContent;   
    
private:
    void Pkg_Handler();

    void Init();

private:
	   	

public:
    TCPigeonLocationTask(int nSock);
    TCPigeonLocationTask();
    ~TCPigeonLocationTask();
    void ClientExecute();
};

#endif

