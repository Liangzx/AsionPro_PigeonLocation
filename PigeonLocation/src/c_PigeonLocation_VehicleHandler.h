/*****************************************************************************
CAPTION
Project Name: 归巢系统--数据采集模块
Description : 车载设备协议处理类
File Name   : C_PIGEONLOCATION_VEHICLE_HANDLER_H
Requires    : LinuxAS3 , Oracle 9i
Create Date : 2016-05-04
******************************************************************************/

#ifndef C_PIGEONLOCATION_VEHICLE_HANDLER_H
#define C_PIGEONLOCATION_VEHICLE_HANDLER_H
//---------------------------------------------------------------------------
#include "cmpublic.h"
#include "cmpublic_net.h"
#include "c_PigeonLocation_config.h"
#include "c_PigeonLocation_Public.h"
#include "c_PigeonLocation_Handle.h"
#include "c_PigeonLocation_log.h"

#include <map>
#include <vector>
#include <cmath>

using namespace std;

#define PKG_LENGTH 7

typedef struct VehiclePkg {
	TCString pkg_pkg_offset;	//数据包时间偏移
	TCString pkg_imei;				//设备识别码
	TCString pkg_gps_flag;		//GPS定位状态 0/1
	TCString pkg_beijing_time;
	TCString pkg_latitude;		//纬度
	TCString pkg_longitude;		//经度
	TCString pkg_altitude;		//海拔高度，以米为单位
	TCString pkg_voltage;			//电池电压
	void Clear() {
		pkg_pkg_offset = "";
		pkg_imei = "";
		pkg_gps_flag = "";
		pkg_beijing_time = "";
		pkg_latitude = "";
		pkg_longitude = "";
		pkg_voltage = "";
	}
}VehiclePkg;

class TCPigeonLocationVehicleHandler
{
private:
	short threshold_flag;
  long  m_nTimeOut;
  long  m_nProcessTime;
  long m_nCmd_Index;  
  bool nLogined_Status;  
  TCString m_sCurAlarm_NotifyTimer;
	TCString m_sDev_ID;
  TCString m_sCust_ID;
  TCString m_sImei;
  TCString m_sImsi;
	vector<TCString> m_vsPkgList;
	//: 记录处理后，客户端未响应的操作序号以及报文内容；
	map<TCString, TCString> m_msOperID_PkgContent;
  TCString  m_sRecvTime;
  TCString  m_sSendTime;
  //: 终端上报IP地址；
  TCString m_sTacker_Send_IPAddress;
	//: 报文头的前缀 CWT
	TCString m_sPkg_Head_Flag;
	//: 报文头的内容的长度
	long m_nContent_Len;
	//: 报文内容域的整个内容
	TCString m_sPkg_Content;
	//: 分割字符
	char m_cDelimter;		
	//: 报文内容域解析后的请求命令字
	TCString m_sReq_Command;		
	//: 报文一次解析 （按照 & 分割解析） 后的字段列表
	TCStringList m_lsContentFieldList;
	//: 报文内容处理后的响应命令字
	TCString m_sRespCommand;
	//: 报文内容处理后的响应报文内容
	TCString m_sRespPkgContent;
	static TCCriticalSection m_csLogLock;
	int field_counts;

private:
	// 报文解析
	void MsgParsing(const TCString & content, VehiclePkg & pkg);
	// 求弧度
	double radian(double d);
	//计算距离
	double get_distance(double lat1, double lng1, double lat2, double lng2);

	//: pad的数据处理方式，接收数据, 按照无阻塞接收
  void RecvRequest(TCCustomUniSocket  &cusSocket);
  void DealRequest(TCCustomUniSocket  &cusSocket);
  void SendRespPkg(TCCustomUniSocket  &cusSocket, int interval);
  void Init();
	void OutputRpBrJudgeSubproc(const VehiclePkg & pkg);
	void OutputRpBrDevStatusMgr(const VehiclePkg & pkg);
private:
  
  //: 智能环上报位置信息
  void DoCommand_LocInfo(TCCustomUniSocket  &cusSocket);

public:
	TCString vender;
public:
  TCPigeonLocationVehicleHandler();
  ~TCPigeonLocationVehicleHandler();
  bool Main_Handler(TCCustomUniSocket  &cusSocket);
};

#endif //!C_PIGEONLOCATION_VEHICLE_HANDLER_H

