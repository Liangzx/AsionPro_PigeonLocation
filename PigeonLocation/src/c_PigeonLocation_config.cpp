/*****************************************************************************
CAPTION
Project Name: �鳲ϵͳ--���ݲɼ�ģ��
Description : �����ļ���
File Name   : c_PigeonLocation_config.cpp
Requires    : LinuxAS3 , Oracle 9i
Create Date : 2016-05-04
******************************************************************************/

#include "c_PigeonLocation_config.h"
//---------------------------------------------------------------------------
//==========================================================================
// ���� :   
// ��; :  
// ԭ�� : 
// ���� :
// ���� :
// ˵�� :
//==========================================================================
TCPigeonLocationConfig::TCPigeonLocationConfig(){
  	LoadIniFile();
}
//==========================================================================
// ���� :    
// ��; :
// ԭ�� : 
// ���� :
// ���� :
// ˵�� :
//==========================================================================
TCPigeonLocationConfig::~TCPigeonLocationConfig(){
}

//==========================================================================
// ���� : TCPigeonLocationConfig::LoadIniFile
// ��; : ���������ļ�
// ԭ�� : LoadIniFile()
// ���� : ��
// ���� : ��
// ˵�� : 
//==========================================================================
void TCPigeonLocationConfig::LoadIniFile(){
    //��ʼ����ػ�������
    m_bDebugLog=false;    
    m_nPort=0;
    m_sAddress="127.0.0.1";
    m_nMaxThread=30;
    m_nMinThread=20;
    m_nQueue=20;
    m_sTracker_Vendor="";
		m_bOpenLBS_Location=true;
    
    TCString  sReadStr;
    sReadStr = ProfileAppString(Application.GetAppName(),
        "GENERAL","DEBUG", "");
   	sReadStr=AllTrim(sReadStr);
    sReadStr=TrimCRLF(sReadStr);	//: ȥ���س�����
    if ( sReadStr == "Y" ) m_bDebugLog = true;
        
    sReadStr = ProfileAppString(Application.GetAppName(),
        "GENERAL","OPEN_LBS_LOC", "");
   	sReadStr=AllTrim(sReadStr);
    sReadStr=TrimCRLF(sReadStr);	//: ȥ���س�����
    if ( sReadStr == "N" ) m_bOpenLBS_Location= false;

    sReadStr = ProfileAppString(Application.GetAppName(),
       "GENERAL","DB_CONSTR", "");
    sReadStr=AllTrim(sReadStr);
    sReadStr=TrimCRLF(sReadStr);	//: ȥ���س�����
    m_sDBConStr=sReadStr;
    ASSERT( m_sDBConStr != "" );

		sReadStr = ProfileAppString(Application.GetAppName(),
       "GENERAL","VENDOR", "");
    sReadStr=AllTrim(sReadStr);
    sReadStr=TrimCRLF(sReadStr);	//: ȥ���س�����
    m_sTracker_Vendor=sReadStr;
    ASSERT( m_sTracker_Vendor != "" );


    //: ������IP��ַ
    sReadStr = ProfileAppString(Application.GetAppName(),
        "GENERAL","SERVER_IP", "");
    sReadStr=AllTrim(sReadStr);
    sReadStr=TrimCRLF(sReadStr);	//: ȥ���س�����
    if(m_sAddress!="")
	    m_sAddress=sReadStr;    
    //: �˿ں�
    sReadStr = ProfileAppString(Application.GetAppName(),
        "GENERAL","SERVER_PORT", "");
    sReadStr=AllTrim(sReadStr);
    sReadStr=TrimCRLF(sReadStr);	//: ȥ���س�����
     if (sReadStr != "") 
    	m_nPort = StrToInt(sReadStr);
    //: ����߳�����
    sReadStr = ProfileAppString(Application.GetAppName(),
        "GENERAL","MAX_THREAD", "");
    sReadStr=AllTrim(sReadStr);
    sReadStr=TrimCRLF(sReadStr);	//: ȥ���س�����
     if (sReadStr != "") 
    	m_nMaxThread = StrToInt(sReadStr);
    	
    //: ��С�߳�����
    sReadStr = ProfileAppString(Application.GetAppName(),
        "GENERAL","MIN_THREAD", "");
    sReadStr=AllTrim(sReadStr);
    sReadStr=TrimCRLF(sReadStr);	//: ȥ���س�����
     if (sReadStr != "") 
    	m_nMinThread = StrToInt(sReadStr);
    	
    //: ��������
    sReadStr = ProfileAppString(Application.GetAppName(),
        "GENERAL","QUEUE", "");
    sReadStr=AllTrim(sReadStr);
    sReadStr=TrimCRLF(sReadStr);	//: ȥ���س�����
     if (sReadStr != "") 
    	m_nQueue = StrToInt(sReadStr);	

		// ������ֵ
		 sReadStr = ProfileAppString(Application.GetAppName(),
			 "GENERAL", "DISTANCE_THRESHOLD_VALUE", "500");
		 m_distance_threshold_value = (int)StrToInt(sReadStr);
    return;
}
