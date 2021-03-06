/*****************************************************************************
CAPTION
Project Name: 归巢系统--数据采集模块
Description : 程序入口
File Name   : PigeonLocation.cpp
Requires    : LinuxAS3 , Oracle 9i
Create Date : 2016-05-04
******************************************************************************/

#include "cmpublic.h"
#include "c_PigeonLocation_Public.h"
#include "c_PigeonLocation_config.h"
#include "c_PigeonLocation_Handle.h"
#include "c_PigeonLocation_log.h"
#include "c_PigeonLocation_server.h"
#define OTL_ORA8I

#include "otlv4.h"
#include <vector>
#include <fstream>
#include <map>
#include <algorithm>

using namespace std;

#define VERSION "v1.1"

//: 内存全局数据
static bool bFirstRunFlag = true;
static bool nCurNeedReloadParam=true;

//: 数据库连接
otl_connect m_dbConnect;

TCString g_msCurTableDay;


TCPigeonLocationConfig *gPigeonLocationConfig;
TCPigeonLocationServer *gPigeonLocationServer;

TCPigeonLocationHandler gsPigeonLocationHandler;

//: 日志对象
TCPigeonLocationLog  *glgMls; 

map<str_Lac_Cell_Node, str_Space_Pos_Unit> g_msCellInfo_PosUnit;

void MainFunc();
void ReleaseMainFunc();
bool DbConnect();

//==========================================================================
// 函数 : main
// 用途 : 主程序
// 原型 : void main()
// 参数 : 
// 返回 : 
// 说明 :
//==========================================================================
int main(int argc, char* argv[]){
	
		if(argc>=2){
			//: 可能是版本查询信息；
			TCString sArgv_Str=argv[1];
			if(sArgv_Str=="-v"){
				printf("程序名称:PigeonLocation.bin\n");
				printf("程序作用:会鸽定位\n");
    		printf("配置文件:~/config/PigeonLocation.ini\n");
    		printf("当前版本号:[%s]\n", (char*)VERSION);
    		exit(0);
			}
  	}		
  	
    g_msCurTableDay="";
    gPigeonLocationServer =NULL;
    printf("主程序启动 Time=[%s]\n", (char*)TCTime::Now());
		otl_connect::otl_initialize(1);
    Application.MutiProcess(20);		//: 多进程设置；


    Application.Initialize("PigeonLocation", argc, argv);
    Application.SetRunningHandle(MainFunc);
    
    try{    
    Application.SetRunningDelay(2000);
		StartupSocket(); 
		Application.SetDiskFreePercentPause(0);
    Application.SetDiskFreePercentStop(0);
    Application.DisableCheckTime(); 

		gPigeonLocationConfig = new TCPigeonLocationConfig;
		glgMls = new TCPigeonLocationLog;

		
    Application.Run();
  }catch(TCException &e){
    TCString sLogMsg = TCString("At MainFuc ") + e.GetExceptionMessage();
    printf("sLogMsg=[%s]\n", (char*)sLogMsg);
  }
  
		if (gPigeonLocationConfig != NULL){
			delete gPigeonLocationConfig;
		}
		if (glgMls!=NULL){
			delete glgMls;
		}
		otl_connect::otl_terminate();

	  return 0;
}

//==========================================================================
// 函数 : MainFunc
// 用途 : 主控程序
// 原型 : void MainFunc()
// 参数 : 
// 返回 : 
// 说明 :
//==========================================================================
void MainFunc(){
		if(gPigeonLocationServer==NULL){
		//: 首先连接数据库
		if(!DbConnect()){
    	TCString sLog="Error DB Connect\n";
			printf("%s", (char*)sLog);
			glgMls->AddPigeonLocationRunLog(sLog);
			exit(0);
		}else{
			TCString sLog="\nMainFunc: 系统启动，数据库连接成功，开始创建服务器进程\n";
			printf("%s", (char*)sLog);
			glgMls->AddPigeonLocationRunLog(sLog);
		}

		
		TCPigeonLocationHandler sLacCellParamHandler;
		sLacCellParamHandler.LoadAllCellPosInfo();

		TCString sLog="Main , LoadAllCellPosInfo: Start Load\n";
		printf("%s", (char*)sLog);
		glgMls->AddPigeonLocationRunLog(sLog);
		
  	Application.InstallExitHandle(ReleaseMainFunc);
	
    try{
    	//启动MSC调度 主服务线程
      TCString sServerAddr = gPigeonLocationConfig->GetServerAddress();
      long nPort = gPigeonLocationConfig->GetServerPort();
      long nMinThread = gPigeonLocationConfig->GetMinThread();
      long nMaxThread = gPigeonLocationConfig->GetMaxThread();
      long nQueue= gPigeonLocationConfig->GetQueue();
      
      printf("ServerAddr=[%s], Port=[%d]\n", (char*)sServerAddr, nPort);
        
      gPigeonLocationServer = new TCPigeonLocationServer(sServerAddr, "",nPort, nQueue, nMinThread, nMaxThread);

      if(gPigeonLocationServer!=NULL){
        gPigeonLocationServer->Do();
      }
    }catch(exception & e){
#ifdef __TEST__
			printf("MainFunc Recv a Fatal Error, System Exit,%s\n", e.what());
#endif
      exit(-1);
    }
  }  
}


//==========================================================================
// 函数 : ReleaseMainFunc
// 用途 : 程序执行结束的处理过程
// 原型 : void ReleaseMainFunc()
// 参数 : 
// 返回 : 
// 说明 :
//==========================================================================
void ReleaseMainFunc(){
	if(gPigeonLocationServer!=NULL)
		delete  gPigeonLocationServer;
	gPigeonLocationServer=NULL;
  CleanupSocket(); 
}

//==========================================================================
// 函数 : DbConnect
// 用途 : 连接数据库
// 原型 : DbConnect()
// 参数 : 无
// 返回 : 无
// 说明 : 
//==========================================================================
bool DbConnect()
{
  try{
		//: 获取数据库链接串
		TCString sDBConnStr;
		sDBConnStr=gPigeonLocationConfig->GetDBConnStr();

		char connstr[256];
		memset(connstr, 0, sizeof(connstr));
		strcpy(connstr, (char*)sDBConnStr); 
#ifdef __TEST__	
		printf("连接串：connstr=[%s]\n", (char*)connstr);
#endif
		//: 连接数据库

		m_dbConnect.rlogon(connstr); // connect to Oracle
		return true;
	}catch(...){
		TCString     sLog;
		sLog="initDbConnect()执行失败";
		glgMls->AddPigeonLocationRunLog(sLog);
		return false;
	}
}



