/*****************************************************************************
CAPTION
Project Name: �鳲ϵͳ--���ݲɼ�ģ��
Description : �����豸Э�鴦����
File Name   : C_PIGEONLOCATION_HOMING_POS_HANDLER_H
Requires    : LinuxAS3 , Oracle 9i
Create Date : 2016-05-04
******************************************************************************/

#ifndef C_PIGEONLOCATION_HOMING_POS_HANDLER_H
#define C_PIGEONLOCATION_HOMING_POS_HANDLER_H
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

typedef struct HomingPosPkg {
	TCString pkg_imei;						//�鳲��imei
	TCString pkg_homing_pos_time;	//�鳲��ʱ��
	TCString pkg_gps_flag;				//GPS��λ״̬ 0/1
	TCString pkg_beijing_time;		//����ʱ��
	TCString pkg_latitude;				//γ��
	TCString pkg_longitude;				//����
	TCString pkg_altitude;				//���θ߶ȣ�����Ϊ��λ
	TCString pkg_voltage;					//��ص�ѹ
	TCString pkg_msg_id;					//��Ϣ����
}HomingPosPkg;

typedef struct BRGatherSubprocInfo {
	short sex_;
	short color_;
	short sandeye_;
	std::string iring_rfid_;
	std::string iringid_;
	std::string iring_imei_;
	std::string iring_blemac_;
	std::string bi_ringid_;
	std::string matchid_;
	std::string matchname_;
	std::string ownerid_;
	void Clear() {
		sex_ = -1;
		color_ = -1;
		sandeye_ = -1;
		iring_rfid_ = "";
		iringid_ = "";
		iring_imei_ = "";
		iring_blemac_ = "";
		bi_ringid_ = "";
		matchid_ = "";
		matchname_ = "";
		ownerid_ = "";
	}
	bool operator<(const BRGatherSubprocInfo & rhs) {
		return iring_rfid_ < rhs.iring_rfid_;
	}
}BRGatherSubprocInfo;

typedef struct BRGatherSubprocInfoWithChk {
public:
	BRGatherSubprocInfoWithChk() {}
	std::string homing_time;
	std::string gps_time;
	std::string mode_st;
	std::string rfid;
	std::string rfid_chk_1;
	std::string rfid_chk_2;
	BRGatherSubprocInfo info;
	void Clear() {
		homing_time = "";
		gps_time = "";
		mode_st = "";
		rfid = "";
		rfid_chk_1 = "";
		rfid_chk_2 = "";
		info.Clear();
	}
}BRGatherSubprocInfoWithChk;

typedef struct RacingPigeonsDataHomingInfo {
	short sex_;
	short color_;
	short sandeye_;
	short type_;//�뻷������
	std::string rfid_;
	std::string rfid_chk_1_;
	std::string bi_ringid_;
	std::string iringid_;
	std::string iring_imei_;
	std::string iring_blemac_;
	std::string iring_mobile_;
	std::string iring_imsi_;
	void Clear() {
		sex_ = -1;
		color_ = -1;
		sandeye_ = -1;
		type_ = -1;
		rfid_ = "";
		rfid_chk_1_ = "";
		bi_ringid_ = "";
		iringid_ = "";
		iring_imei_ = "";
		iring_blemac_ = "";
		iring_mobile_ = "";
		iring_imsi_ = "";
	}
};

typedef struct RpBzNestPadInfo {
	std::string ownerid_;
	std::string ownername_;
	std::string ownername_chn_;
	std::string dorm_id_;
	std::string dorm_name_;
	std::string dorm_address_;
	double dorm_longitude_;
	double dorm_latitude_;
	std::string nestpad_id_;
	std::string nestpad_imei_;
	std::string nestpad_blemac_;
	double nestpad_longitude_;
	double nestpad_latitude_;
	void Clear() {
		ownerid_ = "";
		ownername_ = "";
		ownername_chn_ = "";
		dorm_id_ = "";
		dorm_name_ = "";
		dorm_address_ = "";
		dorm_longitude_ = 0.0;
		dorm_latitude_ = 0.0;
		nestpad_id_ = "";
		nestpad_imei_ = "";
		nestpad_blemac_ = "";
		nestpad_longitude_ = 0.0;
		nestpad_latitude_ = 0.0;
	}
	bool operator<(const RpBzNestPadInfo & rhs) {
		return nestpad_imei_ < rhs.nestpad_imei_;
	}
}RpBzNestPadInfo;

typedef struct NestVideoInfo {
	std::string rfid;
	BRGatherSubprocInfo info;
	void Clear() {
		rfid = "";
		info.Clear();
	}
}NestVideoInfo;

class TCPigeonLocationHomingPosHandler
{
private:
	short threshold_flag;
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
	//: ��¼����󣬿ͻ���δ��Ӧ�Ĳ�������Լ��������ݣ�
	map<TCString, TCString> m_msOperID_PkgContent;
  TCString  m_sRecvTime;
  TCString  m_sSendTime;
  //: �ն��ϱ�IP��ַ��
  TCString m_sTacker_Send_IPAddress;
	//: ����ͷ��ǰ׺ CWT
	TCString m_sPkg_Head_Flag;
	//: ����ͷ�����ݵĳ���
	long m_nContent_Len;
	//: �������������������
	TCString m_sPkg_Content;
	//: �ָ��ַ�
	char m_cDelimter;		
	//: ��������������������������
	TCString m_sReq_Command;		
	//: ����һ�ν��� ������ & �ָ������ ����ֶ��б�
	TCStringList m_lsContentFieldList;
	//: �������ݴ�������Ӧ������
	TCString m_sRespCommand;
	//: �������ݴ�������Ӧ��������
	TCString m_sRespPkgContent;
	static TCCriticalSection m_csLogLock;
	int field_counts;
private:
	// ���Ľ���
	void MsgParsing(const TCString & content, HomingPosPkg & pkg);
	// RP_BR_GATHER_SUBPROC
	void LoadRpBrGatherSubProc();
	// RP_BZ_NESTPAD
	void LoadRpBzNestPad();
	//
	void OutputRpBrDevStatusMgr(HomingPosPkg & pkg, int const dev_type, int const dev_status);
	//RP_BR_NESTPAD_SUBPROC
	void OutputRpBrNestPadSubproc(const TCString & pkg_content,	HomingPosPkg & pkg, const int msg_id);
	// RP_BR_NESTVIDEO_SUBPROC
	void OutputRpBrNestVideoSubproc(const TCString & pkg_content, HomingPosPkg & pkg,
		const char * video_data, long vedio_size);
	// RP_BR_PIGEON_NESTVIDEO
	void OutputRpBrPigeonNestVideo(const TCString & pkg_content, HomingPosPkg & pkg);
	// RP_BZ_RACINGPIGEON
	void OutputRpBzRacingPigeonHomingPos(const TCString & pkg_content, HomingPosPkg & pkg);
private:
	// �ж�����
	bool CheckDistance(TCCustomUniSocket  & cusSocket, const HomingPosPkg & pkg);
	// ��Ϣ������--������
	bool HeartBeatingRespHandle(TCCustomUniSocket  & cusSocket, const HomingPosPkg & pkg);
	bool Back2NestRespHandle(TCCustomUniSocket  & cusSocket, const HomingPosPkg & pkg);
	bool FlashMemDataRespHandle(TCCustomUniSocket  & cusSocket, const HomingPosPkg & pkg);
	bool Back2NestVideoRespHandle(TCCustomUniSocket  & cusSocket, const HomingPosPkg & pkg);
	bool RacingPigeonsDataRespPosHandle(TCCustomUniSocket  & cusSocket, const HomingPosPkg & pkg);
private:
	// �󻡶�
	double radian(double d);
	//�������
	double get_distance(double lat1, double lng1, double lat2, double lng2);
	//: pad�����ݴ���ʽ����������, ��������������
  void RecvRequest(TCCustomUniSocket  &cusSocket);
  void DealRequest(TCCustomUniSocket  &cusSocket);
  void SendRespPkg(TCCustomUniSocket  &cusSocket, TCString  msg_id, TCString  msg_content);
  void Init();
private:
  
  //: ���ܻ��ϱ�λ����Ϣ
  void DoCommand_LocInfo(TCCustomUniSocket  &cusSocket);

public:
	TCString vender;
private:
	std::map<std::string, BRGatherSubprocInfo> br_gather_subproc_infos;
	std::map<std::string, RpBzNestPadInfo> bz_nest_pad_infos;
public:
  TCPigeonLocationHomingPosHandler();
  ~TCPigeonLocationHomingPosHandler();
  bool Main_Handler(TCCustomUniSocket  &cusSocket);
};

#endif //!C_PIGEONLOCATION_HOMING_POS_HANDLER_H

