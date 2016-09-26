/*****************************************************************************
CAPTION
Project Name: �鳲ϵͳ--���ݲɼ�ģ��
Description : �����豸Э�鴦����
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
	TCString pkg_pkg_offset;	//���ݰ�ʱ��ƫ��
	TCString pkg_imei;				//�豸ʶ����
	TCString pkg_gps_flag;		//GPS��λ״̬ 0/1
	TCString pkg_beijing_time;
	TCString pkg_latitude;		//γ��
	TCString pkg_longitude;		//����
	TCString pkg_altitude;		//���θ߶ȣ�����Ϊ��λ
	TCString pkg_voltage;			//��ص�ѹ
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
	void MsgParsing(const TCString & content, VehiclePkg & pkg);
	// �󻡶�
	double radian(double d);
	//�������
	double get_distance(double lat1, double lng1, double lat2, double lng2);

	//: pad�����ݴ���ʽ����������, ��������������
  void RecvRequest(TCCustomUniSocket  &cusSocket);
  void DealRequest(TCCustomUniSocket  &cusSocket);
  void SendRespPkg(TCCustomUniSocket  &cusSocket, int interval);
  void Init();
	void OutputRpBrJudgeSubproc(const VehiclePkg & pkg);
	void OutputRpBrDevStatusMgr(const VehiclePkg & pkg);
private:
  
  //: ���ܻ��ϱ�λ����Ϣ
  void DoCommand_LocInfo(TCCustomUniSocket  &cusSocket);

public:
	TCString vender;
public:
  TCPigeonLocationVehicleHandler();
  ~TCPigeonLocationVehicleHandler();
  bool Main_Handler(TCCustomUniSocket  &cusSocket);
};

#endif //!C_PIGEONLOCATION_VEHICLE_HANDLER_H

