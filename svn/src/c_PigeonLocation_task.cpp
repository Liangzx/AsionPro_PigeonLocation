/*****************************************************************************
CAPTION
           Project Name: �鳲ϵͳ--���ݲɼ�ģ��
           Description : ����Э���ɷ�����Ӧ��Э�鴦����
           File Name   : c_PigeonLocation_task.cpp
           Requires    : LinuxAS3 , Oracle 9i
           Create Date : 2016-05-04
******************************************************************************/

#pragma hdrstop

#include "c_PigeonLocation_task.h"
#include "c_PigeonLocation_config.h"
#include "c_PigeonLocation_log.h"

#pragma package(smart_init)

extern TCPigeonLocationConfig *gPigeonLocationConfig;
extern TCPigeonLocationLog  *glgMls; 

//==========================================================================
// ���� : TCPigeonLocationTask::TCPigeonLocationTask
// ��; : ���캯��
// ԭ�� : TCPigeonLocationTask(int nSock)
// ���� : �����ӵ��׽���
// ���� :
// ˵�� :
//==========================================================================
TCPigeonLocationTask::TCPigeonLocationTask(int nSock) : TCThreadServerSock(nSock){
}

//==========================================================================
// ���� : TCPigeonLocationTask::TCPigeonLocationTask
// ��; : ���캯��
// ԭ�� : TCPigeonLocationTask()
// ���� :
// ���� :
// ˵�� :
//==========================================================================
TCPigeonLocationTask::TCPigeonLocationTask(){
   
}

//==========================================================================
// ���� : TCPigeonLocationTask::~TCPigeonLocationTask()
// ��; : ��������
// ԭ�� : ~TCPigeonLocationTask()
// ���� :
// ���� :
// ˵�� :
//==========================================================================
TCPigeonLocationTask::~TCPigeonLocationTask()
{
}

//==========================================================================
// ���� : TCPigeonLocationTask::ClientExecute
// ��; : ��Ҫʵ�־����ҵ���߼����ɿͻ�������
// ԭ�� : void ClientExecute()
// ���� :
// ���� :
// ˵�� : Ӧ�ó�����̳д��ಢ���ش˺���
//==========================================================================
void TCPigeonLocationTask::ClientExecute(){
	try{
		//: Զ�̷��͵ĵ�ַ
		sTacker_Send_IPAddress=m_cusSocket.GetRemoteAddress();
		//: ��¼״̬Ϊδ��¼;
		nLogined_Status=false;
		m_nTimeOut=60;
		
		printf("Recv Tracker Requst: IP=[%s], Port=[%d], ThreadID=[%s]�� Timeout=[%d]\n", (char*)m_cusSocket.GetRemoteAddress(), m_cusSocket.GetRemotePort(), (char*)IntToStr(GetThreadID()),  m_nTimeOut);
		 
		m_sRecvTime=TCTime::Now();
		Init();
 		Pkg_Handler();
 		m_cusSocket.Close();
 		
 		printf("ThreadID=[%s] Handle End\n", (char*)IntToStr(GetThreadID()));
		
  }catch(TCException &e){
    TCString sLogMsg = TCString("At Clientexec ��������ʱ����:") + e.GetExceptionMessage();
    glgMls->AddPigeonLocationRunLog(sLogMsg);
    m_cusSocket.Close();
  }catch(...){
    glgMls->AddPigeonLocationRunLog("At Clientexec Unknow, �ر�Socket����");
    m_cusSocket.Close();
 	}
}

//==========================================================================
// ���� : TCPigeonLocationTask::Pkg_Handler()
// ��; : ��������
// ԭ�� : void Pkg_Handler()
// ���� : ��
// ���� : ��
// ˵�� : 
//==========================================================================
void TCPigeonLocationTask::Pkg_Handler(){
	try{
		
		if(gPigeonLocationConfig->GetTrackerVendor()=="PIGEON"){
			//: pad�ɴ����ж��豸��Э��ջ
			TCPigeonLocationPigeonHandler sHandlerObj;
			//: �������ݱ���
			sHandlerObj.Main_Handler(m_cusSocket);
			return;
		}else if(gPigeonLocationConfig->GetTrackerVendor()=="VEHICLE"){
			//: �t���ɴ����ж��豸��Э��ջ
			TCPigeonLocationVehicleHandler sHandlerObj;
			//: �������ݱ���
			sHandlerObj.Main_Handler(m_cusSocket);
			return;
		}else if(gPigeonLocationConfig->GetTrackerVendor()=="HOMINGPOS"){
			//: �����豸��Э��ջ
			TCPigeonLocationHomingPosHandler sHandlerObj;
			//: �������ݱ���
			sHandlerObj.Main_Handler(m_cusSocket);
			return;
		}	else if (gPigeonLocationConfig->GetTrackerVendor() == "PIGEONGATHER") {
			//: �����豸��Э��ջ
			TCPigeonLocationPigeonGatherHandler sHandlerObj;
			//: �������ݱ���
			sHandlerObj.Main_Handler(m_cusSocket);
			return;
		}
  }catch (TCException &e){
    m_cusSocket.Close();
    throw e;
  }catch (...){
    m_cusSocket.Close();
    throw TCException("TCPigeonLocationTask::Pkg_Handler() Error:"
                        "Unknown Exception.");
  }
}
//==========================================================================
// ���� : void TCPigeonLocationTask::Init
// ��; : ��ʼ��
// ԭ�� : void Init()
// ���� : ��
// ���� : ��
// ˵�� : 
//==========================================================================
void TCPigeonLocationTask::Init(){
	m_sPkg_Head_Flag="";
	m_sPkg_Content="";
	m_sReq_Command="";
	m_sRespCommand="";
	m_sRespPkgContent="";
	return;
}
