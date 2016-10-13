/*****************************************************************************
CAPTION
Project Name: 归巢系统--数据采集模块
Description : 集鸽设备协议处理类
File Name   : C_PIGEONLOCATION_PIGEON_GATHER_HANDLER_H
Requires    : LinuxAS3 , Oracle 9i
Create Date : 2016-05-04
******************************************************************************/

#ifndef C_PIGEONLOCATION_PIGEON_GATHER_HANDLER_H
#define C_PIGEONLOCATION_PIGEON_GATHER_HANDLER_H
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

typedef struct PigeonGatherPkg {
	TCString pkg_gather_imei;	//集鸽器imei
	TCString pkg_gather_time;	//集鸽器时间
	TCString pkg_gps_flag;		//GPS定位状态 0/1
	TCString pkg_beijing_time;//北京时间
	TCString pkg_latitude;		//纬度
	TCString pkg_longitude;		//经度
	TCString pkg_altitude;		//海拔高度，以米为单位
	TCString pkg_voltage;			//电池电压
	TCString pkg_msg_id;			//消息类型
}PigeonGatherPkg;

// 赛鸽拥有着信息
typedef struct PigeonOwnerInfo {
	std::string mobile_;
	std::string owner_id_;
	std::string owner_name_;
	std::string ownername_chn_;
	void Clear() {
		mobile_ = "";
		owner_id_ = "";
		owner_name_ = "";
		ownername_chn_ = "";
	}
	bool operator < (const PigeonOwnerInfo & rhs) {
		return owner_id_ < rhs.owner_id_;
	}
}PigeonOwnerInfo;

// 记录集鸽器信息
typedef struct GatherDevInfo {
	std::string gatger_imei_;
	std::string gatger_id_;
	std::string gatger_blemac_;
	std::string match_id_;
	std::string match_name_;
	double gather_addr_latitude_;
	double gather_addr_longitude_;
	void Clear() {
		gatger_imei_ = "";
		gatger_id_ = "";
		gatger_blemac_ = "";
		match_id_ = "";
		match_name_ = "";
		gather_addr_latitude_ = 0.0;
		gather_addr_longitude_ = 0.0;
	}
	bool operator < (const GatherDevInfo & rhs) {
		return gatger_id_ < rhs.gatger_id_;
	}
}GatherDevInfo;

// 记录赛鸽信息
typedef struct RacingPigeon{
	std::string rfid;
	std::string rfid_chk_2;
	std::string owner_id;
	std::string owner_name;
	std::string ownername_chn;
	std::string bi_ringid;
	short sex;
	short color;
	short sandeye;
	short ring_type;
	std::string iringid;
	std::string iring_imei;
	std::string iring_blemac;
	std::string iring_mobile;
	void Clear() {
		rfid = "";
		rfid_chk_2 = "";
		owner_id = "";
		owner_name = "";
		ownername_chn = "";
		bi_ringid = "";
		sex = -1;
		color = -1;
		sandeye = -1;
		iringid = "";
		iring_imei = "";
		iring_blemac = "";
		iring_mobile = "";
	}
}RacingPigeon;

//信鸽查询
typedef struct RFIDStatusRespInfo {
	std::string rfid_;
	std::string rfid_status_;
	std::string ownerid_;
	std::string ownername_;
	std::string ownername_chn_;
	std::string bi_ringid_;
	std::string dorm_id_;
	std::string dorm_address_;
	std::string nestpad_imei_;	// rfid对应踏板imei
	std::string rfid_type_;			// rfid 对应的环的类型 0普通/1智能
	void Clear() {
		rfid_ = "";
		rfid_status_ = "0";
		ownerid_ = "";
		ownername_ = "";
		ownername_chn_ = "";
		bi_ringid_ = "";
		dorm_id_ = "";
		dorm_address_ = "";
		nestpad_imei_ = "";
		rfid_type_ = "0";
	}
}RFIDStatusRespInfo;
// 信鸽补录信息
typedef struct RacingPigeonsDataRespInfo {
	short sex;
	short color;
	short sandeye;
	short ring_type;	// 对应的环的类型 0普通 / 1智能
	std::string rfid;
	std::string rfid_chk;
	std::string iringid;
	std::string iring_imei;
	std::string iring_blemac;
	std::string iring_mobile;
	std::string iring_imsi;
	std::string bi_ringid;

	void Clear() {
		rfid = "";
		rfid_chk = "";
		iringid = "";
		iring_imei = "";
		iring_blemac = "";
		iring_mobile = "";
		iring_imsi = "";
		bi_ringid = "";
		sex = -1;
		color = -1;
		sandeye = -1;
		ring_type = -1;
	}
};

// 踏板消息
typedef struct RpBzGatherNestPadInfo {
	std::string owner_id_;
	std::string dorm_id_;
	std::string dorm_name_;
	std::string dorm_address_;
	std::string nestpad_id_;
	std::string nestpad_index_;
	std::string nestpad_imei_;
	void Clear() {
		owner_id_ = "";
		dorm_id_ = "";
		dorm_name_ = "";
		dorm_address_ = "";
		nestpad_id_ = "";
		nestpad_index_ = "";
		nestpad_imei_ = "";
	}
}RpBzGatherNestPadInfo;


class TCPigeonLocationPigeonGatherHandler
{
private:
	short threshold_flag;	//距离超出阈值flag
	double distance_threshold;
	TCString delay_seconds;
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
	
	// 报文解析
	void MsgParsing(const TCString & content, PigeonGatherPkg & pkg);
	std::string JointPkgFields(const std::vector<RpBzGatherNestPadInfo> & pad_info,
		const std::vector<RFIDStatusRespInfo> & rfid_infos);
	void GetRacingPigeonInfosByRfid(RacingPigeon & racing_pegeons);
	void GetRpBzGatherNestPadInfo(std::string owner_id,
		std::vector<RpBzGatherNestPadInfo> & nest_pad_infos);
	bool GetRFIDStatusRespInfo(std::vector<RFIDStatusRespInfo> & rfidstatusrespinfo);
	//如果全部为空返回false
	void GetRacingPigeonsDataRespInfo(std::vector<RacingPigeonsDataRespInfo> & racingpigeonsdatarespinfos);
	// 加载RP_BZ_PIGEON_OWNER到内存
	void LoadRpBzPigeonOwner();
	// 加载RP_BZ_GATHER_DEV到内存
	void LoadRpBzGatherDev();
	//RP_BR_DEVSTATUS_MGR 表登记集鸽器
	void OutputRpBrDevStatusMgr(const PigeonGatherPkg & pkg, int const dev_status);
	//RP_BR_GATHER_SUBPROC入库
	void OutputRpBrGatherSubProc(const GatherDevInfo & dev_info,
		const std::vector<RacingPigeon> & racing_pegeons, const PigeonGatherPkg & pkg);
	// RP_BZ_RACINGPIGEON 入库
	void OutputRpBzRacingPigeon(const PigeonGatherPkg & pkg,
		const GatherDevInfo & devinfo, const PigeonOwnerInfo & owerinfo,
		const std::vector<RacingPigeonsDataRespInfo> & racing_pigeons_data_resp_info);
private:
	// 求弧度
	double radian(double d);
	//计算距离
	double get_distance(double lat1, double lng1, double lat2, double lng2);
	//: pad的数据处理方式，接收数据, 按照无阻塞接收
  void RecvRequest(TCCustomUniSocket  &cusSocket);
  void DealRequest(TCCustomUniSocket  &cusSocket);
  void SendRespPkg(TCCustomUniSocket  &cusSocket, TCString  msg_id, TCString  msg_content);
  void Init();
private:
  
  //: 智能环上报位置信息
  void DoCommand_LocInfo(TCCustomUniSocket  &cusSocket);
public:
	TCString vender;
public:
  TCPigeonLocationPigeonGatherHandler();
  ~TCPigeonLocationPigeonGatherHandler();
  bool Main_Handler(TCCustomUniSocket  &cusSocket);
	std::map<std::string, PigeonOwnerInfo> pigeon_owner_infos;
	//
	std::map<std::string, GatherDevInfo> gather_dev_infos;
};

#endif //!C_PIGEONLOCATION_PIGEON_GATHER_HANDLER_H

