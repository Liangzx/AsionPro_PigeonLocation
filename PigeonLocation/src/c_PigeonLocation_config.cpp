/*****************************************************************************
CAPTION
Project Name: 归巢系统--数据采集模块
Description : 配置文件类
File Name   : c_PigeonLocation_config.cpp
Requires    : LinuxAS3 , Oracle 9i
Create Date : 2016-05-04
******************************************************************************/

#include "c_PigeonLocation_config.h"
//---------------------------------------------------------------------------
//==========================================================================
// 函数 :   
// 用途 :  
// 原型 : 
// 参数 :
// 返回 :
// 说明 :
//==========================================================================
TCPigeonLocationConfig::TCPigeonLocationConfig(){
  	LoadIniFile();
}
//==========================================================================
// 函数 :    
// 用途 :
// 原型 : 
// 参数 :
// 返回 :
// 说明 :
//==========================================================================
TCPigeonLocationConfig::~TCPigeonLocationConfig(){
}

//==========================================================================
// 函数 : TCPigeonLocationConfig::LoadIniFile
// 用途 : 载入配置文件
// 原型 : LoadIniFile()
// 参数 : 无
// 返回 : 无
// 说明 : 
//==========================================================================
void TCPigeonLocationConfig::LoadIniFile(){
    //初始化相关基础数据
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
    sReadStr=TrimCRLF(sReadStr);	//: 去掉回车换行
    if ( sReadStr == "Y" ) m_bDebugLog = true;
        
    sReadStr = ProfileAppString(Application.GetAppName(),
        "GENERAL","OPEN_LBS_LOC", "");
   	sReadStr=AllTrim(sReadStr);
    sReadStr=TrimCRLF(sReadStr);	//: 去掉回车换行
    if ( sReadStr == "N" ) m_bOpenLBS_Location= false;

    sReadStr = ProfileAppString(Application.GetAppName(),
       "GENERAL","DB_CONSTR", "");
    sReadStr=AllTrim(sReadStr);
    sReadStr=TrimCRLF(sReadStr);	//: 去掉回车换行
    m_sDBConStr=sReadStr;
    ASSERT( m_sDBConStr != "" );

		sReadStr = ProfileAppString(Application.GetAppName(),
       "GENERAL","VENDOR", "");
    sReadStr=AllTrim(sReadStr);
    sReadStr=TrimCRLF(sReadStr);	//: 去掉回车换行
    m_sTracker_Vendor=sReadStr;
    ASSERT( m_sTracker_Vendor != "" );


    //: 服务器IP地址
    sReadStr = ProfileAppString(Application.GetAppName(),
        "GENERAL","SERVER_IP", "");
    sReadStr=AllTrim(sReadStr);
    sReadStr=TrimCRLF(sReadStr);	//: 去掉回车换行
    if(m_sAddress!="")
	    m_sAddress=sReadStr;    
    //: 端口号
    sReadStr = ProfileAppString(Application.GetAppName(),
        "GENERAL","SERVER_PORT", "");
    sReadStr=AllTrim(sReadStr);
    sReadStr=TrimCRLF(sReadStr);	//: 去掉回车换行
     if (sReadStr != "") 
    	m_nPort = StrToInt(sReadStr);
    //: 最大线程数；
    sReadStr = ProfileAppString(Application.GetAppName(),
        "GENERAL","MAX_THREAD", "");
    sReadStr=AllTrim(sReadStr);
    sReadStr=TrimCRLF(sReadStr);	//: 去掉回车换行
     if (sReadStr != "") 
    	m_nMaxThread = StrToInt(sReadStr);
    	
    //: 最小线程数；
    sReadStr = ProfileAppString(Application.GetAppName(),
        "GENERAL","MIN_THREAD", "");
    sReadStr=AllTrim(sReadStr);
    sReadStr=TrimCRLF(sReadStr);	//: 去掉回车换行
     if (sReadStr != "") 
    	m_nMinThread = StrToInt(sReadStr);
    	
    //: 队列数；
    sReadStr = ProfileAppString(Application.GetAppName(),
        "GENERAL","QUEUE", "");
    sReadStr=AllTrim(sReadStr);
    sReadStr=TrimCRLF(sReadStr);	//: 去掉回车换行
     if (sReadStr != "") 
    	m_nQueue = StrToInt(sReadStr);	

		// 距离阈值
		 sReadStr = ProfileAppString(Application.GetAppName(),
			 "GENERAL", "DISTANCE_THRESHOLD_VALUE", "500");
		 m_distance_threshold_value = (int)StrToInt(sReadStr);
	//
	// 表空间
	db_table_space = ProfileAppString(Application.GetAppName(), "GENERAL", "DB_TABLE_SPACE", "USERS");
	//
	// 分区表的初始创建语句的文件名
	long i;
	TCIniFile ifIniFile;
	ifIniFile.Load(MergePathAndFile(TAppPath::AppConfig(), Application.GetAppName() + ".ini"));

	TCStringList lStrList;
	TCStringList lTmpList;
	TCString sSection_Name;
	sSection_Name = "TABLE_NAME|INIT_CRTFILENAME*{";
	sSection_Name += Application.GetProcessFlag();
	sSection_Name += "}*";
	lStrList.Clear();
	ifIniFile.ReadSection(sSection_Name, lStrList);
	if (lStrList.GetCount()>0)
	{
		//当前的特定定义存在;就用当前的；
		printf("当前的特定定义存在: 配置文件加载的配置段[%s]\n", (char*)sSection_Name);
	}
	else
	{
		//当前的特定定义不存在，需要加载通用的配置定义
		sSection_Name = "TABLE_NAME|INIT_CRTFILENAME";
		ifIniFile.ReadSection(sSection_Name, lStrList);
		printf("当前的特定定义不存在，需要加载通用的配置定义: 配置文件加载的配置段[%s]\n", (char*)sSection_Name);
	}
	m_ssTable_InitCrtFileName.clear();
	for (i = 0; i < lStrList.GetCount(); i++)
	{
		lTmpList.Clear();
		lTmpList.CommaText(lStrList[i], '|', false);

		if (lTmpList.GetCount()<2)
			continue;

		TCString sTable_Name = AllTrim(lTmpList[0]);
		sTable_Name = TrimCRLF(sTable_Name);

		TCString sCreateTable_SQLFile = AllTrim(lTmpList[1]);
		sCreateTable_SQLFile = TrimCRLF(sCreateTable_SQLFile);

		m_ssTable_InitCrtFileName[sTable_Name] = sCreateTable_SQLFile;
	}

	// 相关分区表的创建分区语句的文件名
	sSection_Name = "TABLE_NAME|ADDPART_FILENAME*{";
	sSection_Name += Application.GetProcessFlag();
	sSection_Name += "}*";
	lStrList.Clear();
	ifIniFile.ReadSection(sSection_Name, lStrList);
	if (lStrList.GetCount()>0)
	{
		//当前的特定定义存在;就用当前的；
		printf("当前的特定定义存在: 配置文件加载的配置段[%s]\n", (char*)sSection_Name);
	}
	else
	{
		//当前的特定定义不存在，需要加载通用的配置定义
		sSection_Name = "TABLE_NAME|ADDPART_FILENAME";
		ifIniFile.ReadSection(sSection_Name, lStrList);
		printf("当前的特定定义不存在，需要加载通用的配置定义: 配置文件加载的配置段[%s]\n", (char*)sSection_Name);
	}

	m_ssTable_AddPartFileName.clear();
	for (i = 0; i < lStrList.GetCount(); i++)
	{
		lTmpList.Clear();
		lTmpList.CommaText(lStrList[i], '|', false);

		if (lTmpList.GetCount()<2)
			continue;
		TCString sTable_Name = AllTrim(lTmpList[0]);
		sTable_Name = TrimCRLF(sTable_Name);
		TCString sAddPartion_SQLFile = AllTrim(lTmpList[1]);
		sAddPartion_SQLFile = TrimCRLF(sAddPartion_SQLFile);
		m_ssTable_AddPartFileName[sTable_Name] = sAddPartion_SQLFile;
	}
    return;
}
