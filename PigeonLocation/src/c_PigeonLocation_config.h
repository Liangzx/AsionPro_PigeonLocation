/*****************************************************************************
CAPTION
Project Name: 归巢系统--数据采集模块
Description : 配置文件类
File Name   : c_PigeonLocation_config.h
Requires    : LinuxAS3 , Oracle 9i
Create Date : 2016-05-04
******************************************************************************/

#ifndef C_PigeonLocation_CONFIG_H
#define C_PigeonLocation_CONFIG_H

#include "cmpublic.h"
#include "c_PigeonLocation_Public.h"
#include <map>

using namespace std;

class TCPigeonLocationConfig{
private:
	bool 	m_bDebugLog;			//: 是否打开调试	
	bool	m_bOpenLBS_Location;	//: 是否打开LBS定位

  long     m_nPort;				//: 服务器的本地端口
  TCString m_sAddress;		//: 服务器的地址；一般就是本机
  long     m_nMaxThread;	//: 最大线程数
  long     m_nMinThread;	//: 最小线程数
  long     m_nQueue;			//: 队列长度
	int			 m_distance_threshold_value; //距离阈值 m为单位
  
  TCString m_sTracker_Vendor;	//: 前端厂家
	
	TCString m_sDBConStr;		//: 数据库连接串
	TCString m_sTableSpace;	//: 表空间名
	
	//: 指定 分区表的初始创建语句的文件名
	map<TCString, TCString> m_ssTable_InitCrtFileName;	//: 分区表名 --- 初始创建的语句文件名
	
	//: 指定 相关分区表的创建分区语句的文件名
	map<TCString, TCString> m_ssTable_AddPartFileName;	//: 分区表名 --- 新增分区的语句文件名
	
	
private:	
	void LoadIniFile();
	
public:

	TCPigeonLocationConfig();
	~TCPigeonLocationConfig();  
  
	bool IsDebug(){ return m_bDebugLog; }; 

	bool IsOpenLBSLocation() { return m_bOpenLBS_Location; };
		
	TCString & GetDBConnStr(){
		return m_sDBConStr;
	};

	TCString GetServerAddress(){
  	return m_sAddress;
  };
	int 
  long GetServerPort(){
   	return m_nPort;
  };

	long GetMaxThread(){
   	return m_nMaxThread;
  };

  long GetMinThread(){
  	return m_nMinThread;
 	};

  long GetQueue(){
  	return m_nQueue;
  };
	int GetDistanceThresholdValue() {
		return m_distance_threshold_value;
	}
  TCString GetTrackerVendor(){
  	return m_sTracker_Vendor;
  };
  
};
#endif
