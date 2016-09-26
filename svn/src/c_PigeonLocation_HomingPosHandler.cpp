/*****************************************************************************
CAPTION
Project Name: �鳲ϵͳ
Description : �鳲�豸Э�鴦����
File Name   : c_PigeonLocation_HomingPosHandler.cpp
Requires    : LinuxAS3 , Oracle 9i
Create Date : 2016-05-04
******************************************************************************/

#pragma hdrstop
#define OTL_STL

#include "c_PigeonLocation_HomingPosHandler.h"
#include "c_PigeonLocation_config.h"
#include "c_PigeonLocation_task.h"
#include <iostream>
#include <ctime>
#include <fstream>


#pragma package(smart_init)

extern TCPigeonLocationHandler gsPigeonLocationHandler;

extern TCPigeonLocationConfig *gPigeonLocationConfig;


//: ���ݿ�����
extern otl_connect m_dbConnect;


extern TCPigeonLocationLog  *glgMls; 

extern map<str_Lac_Cell_Node, str_Space_Pos_Unit> g_msCellInfo_PosUnit;

//==========================================================================
// ���� : TCPigeonLocationHomingPosHandler::TCPigeonLocationHomingPosHandler
// ��; : ���캯��
// ԭ�� : TCPigeonLocationHomingPosHandler()
// ���� :
// ���� :
// ˵�� :
//==========================================================================
TCPigeonLocationHomingPosHandler::TCPigeonLocationHomingPosHandler(){
}

//==========================================================================
// ���� : TCPigeonLocationHomingPosHandler::~TCPigeonLocationHomingPosHandler()
// ��; : ��������
// ԭ�� : ~TCPigeonLocationHomingPosHandler()
// ���� :
// ���� :
// ˵�� :
//==========================================================================
TCPigeonLocationHomingPosHandler::~TCPigeonLocationHomingPosHandler()
{
}

//==========================================================================
// ���� : TCPigeonLocationHomingPosHandler::Main_Handler
// ��; : ��Ҫʵ�־����ҵ�񽻻��߼�
// ԭ�� : void Main_Handler()
// ���� :
// ���� :
// ˵�� : ��Ҫʵ�־����ҵ�񽻻��߼�
//==========================================================================
bool TCPigeonLocationHomingPosHandler::Main_Handler(TCCustomUniSocket  &cusSocket){
	try{
		vender = gPigeonLocationConfig->GetTrackerVendor();
		//: Զ�̷��͵ĵ�ַ
		m_sTacker_Send_IPAddress=cusSocket.GetRemoteAddress();
		m_nTimeOut=60;
		distance_threshold = gPigeonLocationConfig->GetDistanceThresholdValue() / 1000.0;
		delay_seconds = "60";
		
		printf("���յ� Tracker ��������IP=[%s], Port=[%d], Timeout=[%d]\n", (char*)cusSocket.GetRemoteAddress(), cusSocket.GetRemotePort(), m_nTimeOut);
		//
		LoadRpBrGatherSubProc();
		LoadRpBzNestPad();
		//: �����ӷ�ʽ
		while(1){ 
			Init();
			TCString sLogMsg="Time:Now=["+TCTime::Now()+"],Recv Port=["+IntToStr(cusSocket.GetRemotePort())+"] Data";
            glgMls->AddPigeonLocationRunLog(sLogMsg);
 			RecvRequest(cusSocket);
			threshold_flag = 0;
 			DealRequest(cusSocket);	
 		}
 		cusSocket.Close();
 		return true;
  }catch(TCException &e){
    TCString sLogMsg = TCString("At TCPigeonLocationHomingPosHandler::Main_Handler ��������ʱ����:") + e.GetExceptionMessage();
    glgMls->AddPigeonLocationRunLog(sLogMsg);
    cusSocket.Close();
    return false;
  }catch(...){
    glgMls->AddPigeonLocationRunLog("At TCPigeonLocationHomingPosHandler::Main_Handler Unknow, �ر�Socket����");
    cusSocket.Close();
    return false;
 	}
}

//==========================================================================
// ���� : TCPigeonLocationHomingPosHandler::DealRequest
// ��; : ��������
// ԭ�� : void DealRequest()
// ���� : ��
// ���� : ��
// ˵�� :
//==========================================================================
void TCPigeonLocationHomingPosHandler::DealRequest(TCCustomUniSocket  &cusSocket){
	try{
		//: ���������ֽ��д���
#ifdef __TEST__
   	printf("============receive Tracker request==============\n");
   	printf("Command:=%s, Content=[%s], Time=[%s]\n", (char*)m_sReq_Command, (char*)m_sPkg_Content, (char*)TCTime::Now());
#endif

		//======ִ�з������==========================
    m_sRespCommand="";
    m_sRespPkgContent="";
	 if (m_sReq_Command=="12"){
    	DoCommand_LocInfo(cusSocket);
		} else {
    	//: δ֪����
    	TCString sLogMsg = TCString ("δ֪����,m_sReq_Command: [") + m_sReq_Command+"]";
      glgMls->AddPigeonLocationRunLog(sLogMsg);
      return;
    }
  }catch(TCException &e){
  	try{
      TCString sLogMsg = TCString ("ϵͳ����,At DealRequest: ") + e.GetExceptionMessage();
      glgMls->AddPigeonLocationRunLog(sLogMsg);
			throw e;
    }catch(TCException &e1){
      TCString sLogMsg = TCString("ϵͳ����: when Catch a Error and DealRequest: ") + e1.GetExceptionMessage();
      glgMls->AddPigeonLocationRunLog(sLogMsg);
			throw e1;
    }catch(...){
      glgMls->AddPigeonLocationRunLog("ϵͳ����δ֪�쳣");
			throw;
    }
  }
}

void TCPigeonLocationHomingPosHandler::MsgParsing(const TCString & content, HomingPosPkg & pkg)
{
	try
	{
		TCStringList lsTrackerPkgList;
		lsTrackerPkgList.Clear();
		lsTrackerPkgList.CommaText(content, m_cDelimter);

		pkg.pkg_imei = lsTrackerPkgList[0];
		pkg.pkg_homing_pos_time = lsTrackerPkgList[1];
		pkg.pkg_gps_flag = lsTrackerPkgList[2];
		pkg.pkg_beijing_time = lsTrackerPkgList[3] == "" ?
			"" : TCTime::RelativeTime(lsTrackerPkgList[3], 8 * 3600);
		pkg.pkg_latitude = lsTrackerPkgList[4];
		pkg.pkg_longitude = lsTrackerPkgList[5];
		pkg.pkg_altitude = lsTrackerPkgList[6];
		pkg.pkg_voltage = lsTrackerPkgList[7];
		pkg.pkg_msg_id = lsTrackerPkgList[8];
		std::cout << "PKG:" << (char *)m_sPkg_Content << std::endl;
	}
	catch (const std::exception& e)
	{
		throw e;
	}	
}

void TCPigeonLocationHomingPosHandler::LoadRpBrGatherSubProc()
{
	__ENTERFUNCTION;
	/*
	��ѯRP_BR_GATHER_SUBPROC ����IRING_RFID��IRINGID��IRING_IMEI��IRING_BLEMAC��
	BI_RINGID�� SEX�� COLOR�� SANDEYE �� MATCHID�� MATCHNAME�� OWNERID��ȡ���ڴ��У�
	����IRING_RFIDΪ�������棬���ڻ��߰��ո�IRING_RFID��ѯδ��ѯ��ʱ�������ݿ������¸���
	*/
	std::string td_name = "RP_BR_GATHER_SUBPROC";
	std::string sql_buf = " SELECT SEX,COLOR,SANDEYE ,IRING_RFID,IRINGID,IRING_IMEI, ";
	sql_buf += " IRING_BLEMAC,BI_RINGID,MATCHID,MATCHNAME,OWNERID FROM ";
	sql_buf += td_name;
	otl_stream otl_s(10, sql_buf.c_str(), m_dbConnect);
	br_gather_subproc_infos.clear();
	BRGatherSubprocInfo info;
	while (!otl_s.eof())	{
		info.Clear();
		otl_s >> info.sex_;
		otl_s >> info.color_;
		otl_s >> info.sandeye_;
		otl_s >> info.iring_rfid_;
		otl_s >> info.iringid_;
		otl_s >> info.iring_imei_;
		otl_s >> info.iring_blemac_;
		otl_s >> info.bi_ringid_;
		otl_s >> info.matchid_;
		otl_s >> info.matchname_;
		otl_s >> info.ownerid_;
		br_gather_subproc_infos[info.iring_rfid_] = info;
	}
	otl_s.flush();
	otl_s.close();
	__LEAVEFUNCTION;
}

void TCPigeonLocationHomingPosHandler::LoadRpBzNestPad()	
{
	__ENTERFUNCTION;
	/*
	���� RP_BZ_NESTPAD �� ��ȡVALID_FLAG=1������OWNERID��OWNERNAME��
	OWNERNAME_CHN, DORM_ID�� DORM_NAME��DORM_ADDRESS������λ�ã�DORM_LONGITUDE��DORM_LATITUDE���� 
	NESTPAD_ID�� NESTPAD_IMEI, NESTPAD_BLEMAC�� �鳲�ص�GPS (NESTPAD_LONGITUDE, NESTPAD_LATITUDE),
	���ڴ��б���λ NESTPAD_IMEIΪ���������飬�����ڸ��£���ʱ���º�δ��ѯ����¼ʱ���£�
	*/
	std::string tb_name = "RP_BZ_NESTPAD";
	std::string sql_buf = "SELECT OWNERID,OWNERNAME,OWNERNAME_CHN, DORM_ID, DORM_NAME,";
	sql_buf += "DORM_ADDRESS,DORM_LONGITUDE,DORM_LATITUDE, NESTPAD_ID, NESTPAD_IMEI,";
	sql_buf += " NESTPAD_BLEMAC,NESTPAD_LONGITUDE, NESTPAD_LATITUDE FROM ";
	sql_buf += tb_name;
	sql_buf += " WHERE VALID_FLAG=1";
	otl_stream otl_s(10, sql_buf.c_str(), m_dbConnect);
	RpBzNestPadInfo info;
	while (!otl_s.eof()) {
		info.Clear();
		otl_s >> info.ownerid_;
		otl_s >> info.ownername_;
		otl_s >> info.ownername_chn_;
		otl_s >> info.dorm_id_;
		otl_s >> info.dorm_name_;
		otl_s >> info.dorm_address_;
		otl_s >> info.dorm_longitude_;
		otl_s >> info.dorm_latitude_;
		otl_s >> info.nestpad_id_;
		otl_s >> info.nestpad_imei_;
		otl_s >> info.nestpad_blemac_;
		otl_s >> info.nestpad_longitude_;
		otl_s >> info.nestpad_latitude_;
		bz_nest_pad_infos[info.nestpad_imei_] = info;
	}
	otl_s.flush();
	otl_s.close();
	__LEAVEFUNCTION;
}

void TCPigeonLocationHomingPosHandler::OutputRpBrDevStatusMgr(HomingPosPkg & pkg,
	int const dev_type, int const dev_status)
{
	__ENTERFUNCTION;
	//
	std::map<std::string, RpBzNestPadInfo>::iterator nest_pad_iter;
	nest_pad_iter = bz_nest_pad_infos.find((char *)pkg.pkg_imei);
	std::string nestpad_id = "";
	if (nest_pad_iter == bz_nest_pad_infos.end()) {
		LoadRpBzNestPad();
	}
	nest_pad_iter = bz_nest_pad_infos.find((char *)pkg.pkg_imei);
	if (nest_pad_iter != bz_nest_pad_infos.end()) {
		nestpad_id = nest_pad_iter->second.nestpad_id_;
	}
	//
	std::string tb_name = "RP_BR_DEVSTATUS_MGR";

	std::string sql_buf = "INSERT INTO ";
	sql_buf += tb_name;
	sql_buf += "(DEV_TYPE,";
	sql_buf += "DEV_ID,";
	sql_buf += "DEV_IMEI,";
	sql_buf += "DEV_LONGITUDE,";
	sql_buf += "DEV_LATITUDE,";
	sql_buf += "DEV_HIGH,";
	sql_buf += "STATUS_TIME,";
	sql_buf += "STATUS_TYPE,";
	sql_buf += "THRESHOLD_FLAG";
	sql_buf += ")";
	sql_buf += "VALUES(";
	sql_buf += ":f1DEV_TYPE<int>,";
	sql_buf += ":f2DEV_ID<char[16]>,";
	sql_buf += ":f3DEV_IMEI<char[16]>,";
	sql_buf += ":f4DEV_LONGITUDE<double>,";
	sql_buf += ":f5DEV_LATITUDE<double>,";
	sql_buf += ":f6DEV_HIGH<int>,";
	sql_buf += ":f7STATUS_TIME<TIMESTAMP>,";
	sql_buf += ":f8STATUS_TYPE<int>,";
	sql_buf += ":f9THRESHOLD_FLAG<short>";
	sql_buf += ")";

	otl_stream otl_s(10, sql_buf.c_str(), m_dbConnect);
	otl_s << dev_type;
	otl_s << nestpad_id;
	otl_s << (char *)pkg.pkg_imei;
	otl_s << StrToFloat(pkg.pkg_longitude);
	otl_s << StrToFloat(pkg.pkg_latitude);
	otl_s << (int)StrToInt(pkg.pkg_altitude);
	TCString bs_time;
	bs_time.Empty();
	if (dev_status == 1) {
		bs_time = (pkg.pkg_gps_flag == "1") ? pkg.pkg_beijing_time : TCTime::Now();
	}	else if (dev_status == 2) {
		bs_time = ((pkg.pkg_beijing_time).IsEmpty()) ? pkg.pkg_homing_pos_time : pkg.pkg_beijing_time;
	}
	else {
		;
	}
	otl_datetime otl_date;
	if (String2OTLDateTime(bs_time, otl_date)) {
		otl_s << otl_date;
	}
	else {
		otl_s << otl_null();
	}
	otl_s << dev_status;
	otl_s << threshold_flag;
	otl_s.flush();
	otl_s.close();
	__LEAVEFUNCTION;
}

void TCPigeonLocationHomingPosHandler::OutputRpBrNestPadSubproc(const TCString & pkg_content,
	HomingPosPkg & pkg, const int msg_id)
{
	__ENTERFUNCTION;
	//: �ֽ��û�����
	TCStringList lsTrackerPkgList;
	lsTrackerPkgList.Clear();
	lsTrackerPkgList.CommaText(m_sPkg_Content, m_cDelimter);
	int rfid_num = StrToInt(lsTrackerPkgList[9]);

	otl_stream otl_s;
	std::string sql_buf;
	std::string tb_name;

	//��ѯ�ڴ��е� IRING ��Ϣ,���ٸ�RFID��ȡ���ټ�¼
	std::vector<BRGatherSubprocInfoWithChk> vec_brgathersubprocinfowithchk;
	vec_brgathersubprocinfowithchk.clear();
	BRGatherSubprocInfoWithChk brgathersubprocinfowithchk;
	if (msg_id == 13) {
		for (int idx = 0; idx < rfid_num; idx++) {
			brgathersubprocinfowithchk.Clear();
			brgathersubprocinfowithchk.homing_time = (char *)(lsTrackerPkgList[10 + 6 * idx].IsEmpty()
				? "" : TCTime::RelativeTime(lsTrackerPkgList[10 + 6 * idx], 8 * 3600));
			brgathersubprocinfowithchk.gps_time = (char *)(lsTrackerPkgList[10 + 6 * idx].IsEmpty()
				? "" : TCTime::RelativeTime(lsTrackerPkgList[11 + 6 * idx], 8 * 3600));
			brgathersubprocinfowithchk.mode_st = (char *)lsTrackerPkgList[12 + 6 * idx];
			brgathersubprocinfowithchk.rfid = (char *)lsTrackerPkgList[13 + 6 * idx];
			brgathersubprocinfowithchk.rfid_chk_1[14 + 6 * idx];
			brgathersubprocinfowithchk.rfid_chk_2[15 + 6 * idx];
			std::map<std::string, BRGatherSubprocInfo>::iterator iter_sub = br_gather_subproc_infos.find(brgathersubprocinfowithchk.rfid);
			if (iter_sub != br_gather_subproc_infos.end()) {
				brgathersubprocinfowithchk.info = iter_sub->second;
				vec_brgathersubprocinfowithchk.push_back(brgathersubprocinfowithchk);
			}
		}
	}	else if (msg_id == 15) {
		for (int idx = 0; idx < rfid_num; idx++) {
			brgathersubprocinfowithchk.Clear();
			brgathersubprocinfowithchk.homing_time = (char *)(lsTrackerPkgList[10 + 6 * idx].IsEmpty()
				? "" : TCTime::RelativeTime(lsTrackerPkgList[10 + 6 * idx], 8 * 3600));
			brgathersubprocinfowithchk.gps_time = (char *)(lsTrackerPkgList[10 + 6 * idx].IsEmpty()
				? "" : TCTime::RelativeTime(lsTrackerPkgList[11 + 6 * idx], 8 * 3600));
			brgathersubprocinfowithchk.mode_st = (char *)lsTrackerPkgList[15 + 6 * idx];
			brgathersubprocinfowithchk.rfid = (char *)lsTrackerPkgList[12 + 6 * idx];
			brgathersubprocinfowithchk.rfid_chk_1[13 + 6 * idx];
			brgathersubprocinfowithchk.rfid_chk_2[14 + 6 * idx];

			std::map<std::string, BRGatherSubprocInfo>::iterator iter_sub = br_gather_subproc_infos.find(brgathersubprocinfowithchk.rfid);
			if (iter_sub != br_gather_subproc_infos.end()) {
				brgathersubprocinfowithchk.info = iter_sub->second;
				/*
				�ж��Ƿ��ظ�:
				�Ƿ���Ҫ�ж��ظ��ԣ�����IRING_RFIDΪ������ѯ�鳲��¼�� RP_BR_NESTPAD_SUBPROC
				�����VALID_FLAG=1�ļ�¼����ô��ȡNESTPAD_IMEI��NESTPAD_LONGITUDE��NESTPAD_LATITUDE,
				NESTPAD_BACKTIME, �뱾�α����еľ�γ�ȱȽϾ�����죬
				�Ƚ� NESTPAD_BACKTIME �뱾��ҵ���еġ��鳲����⵽����ʱ�䡱�ֶ�֮��Ĳ��죬
				�����ȣ���Ϊ���ظ���¼������⣩
				*/
				tb_name.clear();
				tb_name = "RP_BR_NESTPAD_SUBPROC";

				sql_buf.clear();
				sql_buf = " SELECT NESTPAD_IMEI��NESTPAD_LONGITUDE��NESTPAD_LATITUDE,	NESTPAD_BACKTIME FROM ";
				sql_buf += tb_name;
				sql_buf += " WHERE IRING_RFID='";
				sql_buf += brgathersubprocinfowithchk.rfid;
				sql_buf += "' AND VALID_FLAG=1 ";
				otl_s.open(1, sql_buf.c_str(), m_dbConnect);
				if (!otl_s.eof()) {
					std::string nestpad_imei;
					double nestpad_longitude, nestpad_latitude;
					otl_datetime nestpad_backtime;
					otl_s >> nestpad_imei;
					otl_s >> nestpad_longitude;
					otl_s >> nestpad_latitude;
					otl_s >> nestpad_backtime;
					TCString time_bk;
					OTLDateTime2String(nestpad_backtime, time_bk);
					if (FloatToStr(nestpad_longitude) != pkg.pkg_longitude
						&& FloatToStr(nestpad_latitude) != pkg.pkg_latitude
						&& nestpad_imei.c_str() != pkg.pkg_imei
						&& (time_bk != brgathersubprocinfowithchk.gps_time.c_str()
							|| time_bk != brgathersubprocinfowithchk.homing_time.c_str())) {
						vec_brgathersubprocinfowithchk.push_back(brgathersubprocinfowithchk);
					}
				}	else {
					//����û�и�rfid��Ӧ�ļ�¼
					vec_brgathersubprocinfowithchk.push_back(brgathersubprocinfowithchk);
				}
			}
			otl_s.flush();
			otl_s.close();
		}
	}	else {
		;
	}

	RpBzNestPadInfo pad_info;
	std::map<std::string, RpBzNestPadInfo>::iterator iter_pad =
		bz_nest_pad_infos.find((char *)pkg.pkg_imei);
	if (iter_pad != bz_nest_pad_infos.end()) {
		pad_info = iter_pad->second;
	}
	// ���
	tb_name.clear();
	tb_name = "RP_BR_NESTPAD_SUBPROC";

	sql_buf.clear();
	sql_buf = " INSERT INTO ";
	sql_buf += tb_name;
	sql_buf += "(";
	sql_buf += "MATCHID,";
	sql_buf += "MATCHNAME,";
	sql_buf += "OWNERID,";
	sql_buf += "OWNERNAME,";
	sql_buf += "OWNERNAME_CHN,";
	sql_buf += "DORM_ID,";
	sql_buf += "DORM_NAME,";
	sql_buf += "DORM_ADDRESS,";
	sql_buf += "DORM_LONGITUDE,";
	sql_buf += "DORM_LATITUDE,";
	sql_buf += "BI_RINGID,";
	sql_buf += "SEX,";
	sql_buf += "COLOR,";
	sql_buf += "SANDEYE,";
	sql_buf += "IRINGID,";
	sql_buf += "IRING_RFID,";
	sql_buf += "IRING_IMEI,";
	sql_buf += "NESTPADID,";
	sql_buf += "NESTPAD_IMEI,";
	sql_buf += "NESTPAD_BLEMAC,";
	sql_buf += "NESTPAD_LONGITUDE,";
	sql_buf += "NESTPAD_LATITUDE,";
	sql_buf += "NESTPAD_HIGH,";
	sql_buf += "NESTPAD_RFID_INITCRC,";
	sql_buf += "GATHER_RFID_2STCRC,";
	sql_buf += "VALID_FLAG,";
	sql_buf += "NESTPAD_BACKTIME,";
	sql_buf += "NESTPAD_SNDTIME,";
	sql_buf += "NESTPAD_RECVTIME,";
	sql_buf += "NESTPAD_RECORDTIME,";
	sql_buf += "THRESHOLD_FLAG";
	sql_buf += ")";
	sql_buf += "VALUES";
	sql_buf += "(";
	sql_buf += ":f01MATCHID<char[16]>,";
	sql_buf += ":f02MATCHNAME<char[256]>,";
	sql_buf += ":f03OWNERID<char[16]>,";
	sql_buf += ":f04OWNERNAME<char[32]>,";
	sql_buf += ":f05OWNERNAME_CHN<char[32]>,";
	sql_buf += ":f06DORM_ID<char[16]>,";
	sql_buf += ":f07DORM_NAME<char[32]>,";
	sql_buf += ":f08DORM_ADDRESS<char[256]>,";
	sql_buf += ":f09DORM_LONGITUDE<double>,";
	sql_buf += ":f10DORM_LATITUDE<double>,";
	sql_buf += ":f11BI_RINGID<char[16]>,";
	sql_buf += ":f12SEX<short>,";
	sql_buf += ":f13COLOR<short>,";
	sql_buf += ":f14SANDEYE<short>,";
	sql_buf += ":f15IRINGID<char[16]>,";
	sql_buf += ":f16IRING_RFID<char[16]>,";
	sql_buf += ":f17IRING_IMEI<char[16]>,";
	sql_buf += ":f18NESTPADID<char[16]>,";
	sql_buf += ":f19NESTPAD_IMEI<char[16]>,";
	sql_buf += ":f20NESTPAD_BLEMAC<char[16]>,";
	sql_buf += ":f21NESTPAD_LONGITUDE<double>,";
	sql_buf += ":f22NESTPAD_LATITUDE<double>,";
	sql_buf += ":f23NESTPAD_HIGH<int>,";
	sql_buf += ":f24NESTPAD_RFID_INITCRC<char[16]>,";
	sql_buf += ":f25GATHER_RFID_2STCRC<char[16]>,";
	sql_buf += ":f26VALID_FLAG<short>,";
	sql_buf += ":f27NESTPAD_BACKTIME<timestamp>,";
	sql_buf += ":f28NESTPAD_SNDTIME<timestamp>,";
	sql_buf += ":f29NESTPAD_RECVTIME<timestamp>,";
	sql_buf += ":f30NESTPAD_RECORDTIME<timestamp>,";
	sql_buf += ":f31THRESHOLD_FLAG<short>";	
	sql_buf += ")";

	short valid_flag = 1;

	otl_s.open(2, sql_buf.c_str(), m_dbConnect);
	for (int i = 0; i < vec_brgathersubprocinfowithchk.size(); i ++) {
		otl_s << vec_brgathersubprocinfowithchk[i].info.matchid_;
		otl_s << vec_brgathersubprocinfowithchk[i].info.matchname_;
		otl_s << pad_info.ownerid_;
		otl_s << pad_info.ownername_;
		otl_s << pad_info.ownername_chn_;
		otl_s << pad_info.dorm_id_;
		otl_s << pad_info.dorm_name_;
		otl_s << pad_info.dorm_address_;
		otl_s << pad_info.dorm_longitude_;
		otl_s << pad_info.dorm_latitude_;
		otl_s << vec_brgathersubprocinfowithchk[i].info.bi_ringid_;
		otl_s << vec_brgathersubprocinfowithchk[i].info.sex_;
		otl_s << vec_brgathersubprocinfowithchk[i].info.color_;
		otl_s << vec_brgathersubprocinfowithchk[i].info.sandeye_;
		otl_s << vec_brgathersubprocinfowithchk[i].info.iringid_;
		otl_s << vec_brgathersubprocinfowithchk[i].info.iring_rfid_;
		otl_s << vec_brgathersubprocinfowithchk[i].info.iring_imei_;
		otl_s << pad_info.nestpad_id_;
		otl_s << (char *)pkg.pkg_imei;
		otl_s << pad_info.nestpad_blemac_;
		otl_s << StrToFloat(pkg.pkg_longitude);
		otl_s << StrToFloat(pkg.pkg_latitude);
		otl_s << (int)StrToInt(pkg.pkg_altitude);
		otl_s << vec_brgathersubprocinfowithchk[i].rfid_chk_1;
		otl_s << vec_brgathersubprocinfowithchk[i].rfid_chk_2;
		otl_s << valid_flag;
		TCString time_tmp;
		time_tmp = (char *)((vec_brgathersubprocinfowithchk[i].gps_time).empty() 
			? vec_brgathersubprocinfowithchk[i].homing_time 
			: vec_brgathersubprocinfowithchk[i].gps_time).c_str();
		otl_datetime otl_date;
		if (String2OTLDateTime(time_tmp, otl_date)) {
			otl_s << otl_date;
		}	else	{
			otl_s << otl_null();
		}

		time_tmp = (pkg.pkg_beijing_time).IsEmpty() ? pkg.pkg_homing_pos_time : pkg.pkg_beijing_time;

		if (String2OTLDateTime(time_tmp, otl_date)) {
			otl_s << otl_date;
		}
		else {
			otl_s << otl_null();
		}

		time_tmp = TCTime::Now();
		//���Ľ���ʱ��--Psռʱ��ô����--ȡ��ǰʱ��
		if (String2OTLDateTime(m_sRecvTime, otl_date)) {
			otl_s << otl_date;
		}
		else {
			otl_s << otl_null();
		}
		// ���ʱ��
		if (String2OTLDateTime(time_tmp, otl_date)) {
			otl_s << otl_date;
		}
		else {
			otl_s << otl_null();
		}
		otl_s << threshold_flag;
	}
	otl_s.flush();
	otl_s.close();
	__LEAVEFUNCTION;
}

void TCPigeonLocationHomingPosHandler::OutputRpBrNestVideoSubproc(const TCString & pkg_content,
	HomingPosPkg & pkg, const char * video_data, long vedio_size)
{
	__ENTERFUNCTION;
	//: �ֽ��û�����
	//std::cout << "long vedio_size:" << vedio_size << std::endl;
	TCStringList lsTrackerPkgList;
	lsTrackerPkgList.Clear();
	lsTrackerPkgList.CommaText(m_sPkg_Content, m_cDelimter);
	int rfid_num = StrToInt(lsTrackerPkgList[12]);
	//��Ƶģʽ
	std::string vedio_mode = (char *)lsTrackerPkgList[9];
	//��Ƶ����鳲��ʱ��
	std::string vedio_mode_time = (char *)lsTrackerPkgList[10];
	//��Ƶ����GPSʱ��
	std::string vedio_mode_time_gps = (char *)lsTrackerPkgList[11];
	//��Ƶ��������
	int vedio_seconds = StrToInt(lsTrackerPkgList[12 + rfid_num + 1]);
	//��Ƶ��֡��N
	int vedio_flames = StrToInt(lsTrackerPkgList[12 + rfid_num + 2]);
	//��Ƶ֡����
	std::string vedio_flames_size = (char *)lsTrackerPkgList[12 + rfid_num + 3];
	std::vector<NestVideoInfo> nestvideoinfos;
	nestvideoinfos.clear();
	NestVideoInfo vedio_info;
	// ���ݱ��ı���rfid
	std::string rfid;
	for (int i = 0; i < rfid_num; i++) {
		vedio_info.Clear();
		vedio_info.rfid = lsTrackerPkgList[13 + i];
		std::map<std::string, BRGatherSubprocInfo>::iterator iter_sub = br_gather_subproc_infos.find(vedio_info.rfid);
		if (iter_sub != br_gather_subproc_infos.end())	{
			vedio_info.info = iter_sub->second;
			nestvideoinfos.push_back(vedio_info);
		}
	}
	//
	RpBzNestPadInfo pad_info;
	std::map<std::string, RpBzNestPadInfo>::iterator iter_pad =
		bz_nest_pad_infos.find((char *)pkg.pkg_imei);
	if (iter_pad != bz_nest_pad_infos.end()) {
		pad_info = iter_pad->second;
	}
	// ���
	//  video_data
	otl_long_string nestpad_video;
	nestpad_video.set_len(vedio_size);
	//
	for (int i = 0; i < vedio_size; i++) {
		//std::cout << "vedio_size" << vedio_size << "video_data:" << video_data[i] << std::endl;
		nestpad_video[i] = video_data[i];
	}
	//
	otl_stream otl_s;
	std::string tb_name = "RP_BR_NESTVIDEO_SUBPROC";
	std::string sql_buf;
	sql_buf.clear();
	sql_buf = "SELECT MAX(VIDEO_ID) AS VEDIO_ID FROM ";
	sql_buf += tb_name;
	
	long cur_seq;
	otl_s.open(1, sql_buf.c_str(), m_dbConnect);
	if (otl_s.eof()) {
		cur_seq = 1;
	} else {
		otl_s >> cur_seq;
		cur_seq++;
	}
	otl_s.flush();
	otl_s.close();
	sql_buf = "INSERT INTO ";
	sql_buf += tb_name;
	sql_buf += "(";
	sql_buf += "MATCHID,";
	sql_buf += "MATCHNAME,";
	sql_buf += "OWNERID,";
	sql_buf += "OWNERNAME,";
	sql_buf += "OWNERNAME_CHN,";
	sql_buf += "DORM_ID,";
	sql_buf += "DORM_NAME,";
	sql_buf += "DORM_ADDRESS,";
	sql_buf += "DORM_LONGITUDE,";
	sql_buf += "DORM_LATITUDE,";
	sql_buf += "NESTPADID,";
	sql_buf += "NESTPAD_IMEI,";
	sql_buf += "NESTPAD_BLEMAC,";
	sql_buf += "NESTPAD_LONGITUDE,";
	sql_buf += "NESTPAD_LATITUDE,";
	sql_buf += "NESTPAD_HIGH,";
	sql_buf += "VALID_FLAG,";
	sql_buf += "VIDEO_ID,";
	sql_buf += "NESTPAD_CONTENTTIME,";
	sql_buf += "NESTPAD_SNDTIME,";
	sql_buf += "NESTPAD_RECVTIME,";
	sql_buf += "NESTPAD_RECORDTIME,";
	sql_buf += "VIDEO_LENGTH��";
	sql_buf += "THRESHOLD_FLAG��";
	sql_buf += "NESTPAD_VIDEO";
	sql_buf += ")";
	sql_buf += "VALUES";
	sql_buf += "(";
	sql_buf += ":f01MATCHID<char[16]>,";
	sql_buf += ":f02MATCHNAME<char[256]>,";
	sql_buf += ":f03OWNERID<char[16]>,";
	sql_buf += ":f04OWNERNAME<char[32]>,";
	sql_buf += ":f05OWNERNAME_CHN<char[32]>,";
	sql_buf += ":f06DORM_ID<char[16]>,";
	sql_buf += ":f07DORM_NAME<char[32]>,";
	sql_buf += ":f08DORM_ADDRESS<char[256]>,";
	sql_buf += ":f09DORM_LONGITUDE<double>,";
	sql_buf += ":f10DORM_LATITUDE<double>,";
	sql_buf += ":f11NESTPADID<char[16]>,";
	sql_buf += ":f12NESTPAD_IMEI<char[16]>,";
	sql_buf += ":f13NESTPAD_BLEMAC<char[16]>,";
	sql_buf += ":f14NESTPAD_LONGITUDE<double>,";
	sql_buf += ":f15NESTPAD_LATITUDE<double>,";
	sql_buf += ":f16NESTPAD_HIGH<int>,";
	sql_buf += ":f17VALID_FLAG<short>,";
	sql_buf += ":f18VIDEO_ID<char[16]>,";
	sql_buf += ":f19NESTPAD_CONTENTTIME<timestamp>,";
	sql_buf += ":f20NESTPAD_SNDTIME<timestamp>,";
	sql_buf += ":f21NESTPAD_RECVTIME<timestamp>,";
	sql_buf += ":f22NESTPAD_RECORDTIME<timestamp>,";
	sql_buf += ":f24VIDEO_LENGTH<int>,";
	sql_buf += ":f25THRESHOLD_FLAG<short>,";
	sql_buf += "empty_clob()";	
	sql_buf += ")";
	sql_buf += "returning NESTPAD_VIDEO into :NESTPAD_VIDEO<clob>";

	otl_s.open(1, sql_buf.c_str(), m_dbConnect);
	short valid_flag = 1;
	for (size_t i = 0; i < nestvideoinfos.size(); i++) {
		otl_s << nestvideoinfos[i].info.matchid_;
		otl_s << nestvideoinfos[i].info.matchname_;
		otl_s << pad_info.ownerid_;
		otl_s << pad_info.ownername_;
		otl_s << pad_info.ownername_chn_;
		otl_s << pad_info.dorm_id_;
		otl_s << pad_info.dorm_name_;
		otl_s << pad_info.dorm_address_;
		otl_s << pad_info.dorm_longitude_;
		otl_s << pad_info.dorm_latitude_;
		otl_s << pad_info.nestpad_id_;
		otl_s << pkg.pkg_imei;
		otl_s << pad_info.nestpad_blemac_;
		otl_s << StrToFloat(pkg.pkg_longitude);
		otl_s << StrToFloat(pkg.pkg_latitude);
		otl_s << (int)StrToInt(pkg.pkg_altitude);
		otl_s << valid_flag;
		otl_s << (char *)IntToStr(cur_seq);
		otl_datetime otl_dt;
		TCString time_tmp = (char *)(vedio_mode_time_gps.empty() ? vedio_mode_time : vedio_mode_time_gps).c_str();
		if (String2OTLDateTime(time_tmp, otl_dt)) {
			otl_s << otl_dt;
		}	else {
			otl_s << otl_null();
		}
		time_tmp = pkg.pkg_beijing_time.IsEmpty() ? pkg.pkg_homing_pos_time : pkg.pkg_beijing_time;
		if (String2OTLDateTime(time_tmp, otl_dt)) {
			otl_s << otl_dt;
		}
		else {
			otl_s << otl_null();
		}
		time_tmp = TCTime::Now();
		if (String2OTLDateTime(m_sRecvTime, otl_dt)) {
			otl_s << otl_dt;
		}
		else {
			otl_s << otl_null();
		}
		if (String2OTLDateTime(time_tmp, otl_dt)) {
			otl_s << otl_dt;
		}
		else {
			otl_s << otl_null();
		}
		
		// ��Ƶ����ʱ��
		otl_s << vedio_seconds;
		// ��Ƶ���ݰ�
		otl_s << threshold_flag;

		otl_s << nestpad_video;
	}
	otl_s.flush();
	otl_s.close();
	__LEAVEFUNCTION;
}

void TCPigeonLocationHomingPosHandler::OutputRpBrPigeonNestVideo(const TCString & pkg_content, HomingPosPkg & pkg)
{
	__ENTERFUNCTION;
	//: �ֽ��û�����
	TCStringList lsTrackerPkgList;
	lsTrackerPkgList.Clear();
	lsTrackerPkgList.CommaText(m_sPkg_Content, m_cDelimter);
	int rfid_num = StrToInt(lsTrackerPkgList[12]);
	//��Ƶģʽ
	std::string vedio_mode = (char *)lsTrackerPkgList[9];
	//��Ƶ����鳲��ʱ��
	std::string vedio_mode_time = (char *)lsTrackerPkgList[10];
	//��Ƶ����GPSʱ��
	std::string vedio_mode_time_gps = (char *)lsTrackerPkgList[11];
	//��Ƶ��������
	int vedio_seconds = StrToInt(lsTrackerPkgList[12 + rfid_num + 1]);
	//��Ƶ��֡��N
	int vedio_flames = StrToInt(lsTrackerPkgList[12 + rfid_num + 2]);
	//��Ƶ֡����
	std::string vedio_flames_size = (char *)lsTrackerPkgList[12 + rfid_num + 3];
	std::vector<NestVideoInfo> nestvideoinfos;
	nestvideoinfos.clear();
	NestVideoInfo vedio_info;
	// ���ݱ��ı���rfid
	std::string rfid;
	for (int i = 0; i < rfid_num; i++) {
		vedio_info.Clear();
		vedio_info.rfid = lsTrackerPkgList[13 + i];
		std::map<std::string, BRGatherSubprocInfo>::iterator iter_sub = br_gather_subproc_infos.find(vedio_info.rfid);
		if (iter_sub != br_gather_subproc_infos.end()) {
			vedio_info.info = iter_sub->second;
			nestvideoinfos.push_back(vedio_info);
		}
	}
	//
	RpBzNestPadInfo pad_info;
	std::map<std::string, RpBzNestPadInfo>::iterator iter_pad =	bz_nest_pad_infos.find((char *)pkg.pkg_imei);
	if (iter_pad != bz_nest_pad_infos.end()) {
		pad_info = iter_pad->second;
	}
	// ���--��ѯ��Ƶid
	otl_stream otl_s;
	std::string tb_name = "RP_BR_NESTVIDEO_SUBPROC";
	std::string sql_buf;
	//std::string sequence_name = "NEST_VIDEO_VEDIO_ID";
	sql_buf.clear();
	sql_buf = "SELECT MAX(VIDEO_ID) AS VIDEO_ID FROM ";
	sql_buf += tb_name;

	long cur_seq;
	otl_s.open(1, sql_buf.c_str(), m_dbConnect);
	if (!otl_s.eof()) {
		otl_s >> cur_seq;
	}
	
	otl_s.flush();
	otl_s.close();
	//
	sql_buf.clear();
	tb_name.clear();
	tb_name = "RP_BR_PIGEON_NESTVIDEO";
	sql_buf += "INSERT INTO ";
	sql_buf += tb_name;
	sql_buf += "(";
	sql_buf += "MATCHID,";
	sql_buf += "MATCHNAME,";
	sql_buf += "OWNERID,";
	sql_buf += "OWNERNAME,";
	sql_buf += "OWNERNAME_CHN,";
	sql_buf += "DORM_ID,";
	sql_buf += "DORM_NAME,";
	sql_buf += "DORM_ADDRESS,";
	sql_buf += "DORM_LONGITUDE,";
	sql_buf += "DORM_LATITUDE,";
	sql_buf += "BI_RINGID,";
	sql_buf += "SEX,";
	sql_buf += "COLOR,";
	sql_buf += "SANDEYE,";
	sql_buf += "IRINGID,";
	sql_buf += "IRING_RFID,";
	sql_buf += "IRING_IMEI,";
	sql_buf += "NESTPADID,";
	sql_buf += "NESTPAD_IMEI,";
	sql_buf += "NESTPAD_BLEMAC,";
	sql_buf += "NESTPAD_LONGITUDE,";
	sql_buf += "NESTPAD_LATITUDE,";
	sql_buf += "NESTPAD_HIGH,";
	sql_buf += "VIDEO_ID,";
	sql_buf += "VALID_FLAG,";
	sql_buf += "NESTPAD_CONTENTTIME,";
	sql_buf += "NESTPAD_SNDTIME,";
	sql_buf += "NESTPAD_RECVTIME,";
	sql_buf += "NESTPAD_RECORDTIME,";
	sql_buf += "THRESHOLD_FLAG";
	sql_buf += ")";
	sql_buf += "VALUES";
	sql_buf += "(";
	sql_buf += ":f01MATCHID<char[16]>,";
	sql_buf += ":f02MATCHNAME<char[256]>,";
	sql_buf += ":f03OWNERID<char[16]>,";
	sql_buf += ":f04OWNERNAME<char[32]>,";
	sql_buf += ":f05OWNERNAME_CHN<char[32]>,";
	sql_buf += ":f06DORM_ID<char[16]>,";
	sql_buf += ":f07DORM_NAME<char[32]>,";
	sql_buf += ":f08DORM_ADDRESS<char[256]>,";
	sql_buf += ":f09DORM_LONGITUDE<double>,";
	sql_buf += ":f10DORM_LATITUDE<double>,";
	sql_buf += ":f11BI_RINGID<char[16]>,";
	sql_buf += ":f12SEX<short>,";
	sql_buf += ":f13COLOR<short>,";
	sql_buf += ":f14SANDEYE<short>,";
	sql_buf += ":f15IRINGID<char[16]>,";
	sql_buf += ":f16IRING_RFID<char[16]>,";
	sql_buf += ":f17IRING_IMEI<char[16]>,";
	sql_buf += ":f18NESTPADID<char[16]>,";
	sql_buf += ":f19NESTPAD_IMEI<char[16]>,";
	sql_buf += ":f20NESTPAD_BLEMAC<char[16]>,";
	sql_buf += ":f21NESTPAD_LONGITUDE<double>,";
	sql_buf += ":f22NESTPAD_LATITUDE<double>,";
	sql_buf += ":f23NESTPAD_HIGH<int>,";
	sql_buf += ":f24VIDEO_ID<char[16]>,";
	sql_buf += ":f25VALID_FLAG<short>,";
	sql_buf += ":f26NESTPAD_CONTENTTIME<timestamp>,";
	sql_buf += ":f27NESTPAD_SNDTIME<timestamp>,";
	sql_buf += ":f28NESTPAD_RECVTIME<timestamp>,";
	sql_buf += ":f29NESTPAD_RECORDTIME<timestamp>,";
	sql_buf += ":f30THRESHOLD_FLAG<short>";
	sql_buf += ")";
	otl_s.open(10, sql_buf.c_str(), m_dbConnect);

	for (int i = 0; i < nestvideoinfos.size(); i++) {
		otl_s << nestvideoinfos[i].info.matchid_;
		otl_s << nestvideoinfos[i].info.matchname_;
		otl_s << pad_info.ownerid_;
		otl_s << pad_info.ownername_;
		otl_s << pad_info.ownername_chn_;
		otl_s << pad_info.dorm_id_;
		otl_s << pad_info.dorm_name_;
		otl_s << pad_info.dorm_address_;
		otl_s << pad_info.dorm_longitude_;
		otl_s << pad_info.dorm_latitude_;
		otl_s << nestvideoinfos[i].info.bi_ringid_;
		otl_s << nestvideoinfos[i].info.sex_;
		otl_s << nestvideoinfos[i].info.color_;
		otl_s << nestvideoinfos[i].info.sandeye_;
		otl_s << nestvideoinfos[i].info.iringid_;
		otl_s << nestvideoinfos[i].rfid;
		otl_s << nestvideoinfos[i].info.iring_imei_;
		otl_s << pad_info.nestpad_id_;
		otl_s << pkg.pkg_imei;
		otl_s << pad_info.nestpad_blemac_;
		otl_s << StrToFloat(pkg.pkg_longitude);
		otl_s << StrToFloat(pkg.pkg_latitude);
		otl_s << (int)StrToInt(pkg.pkg_altitude);
		otl_s << (char *)IntToStr(cur_seq);
		short valid_flag = 1;
		otl_s << valid_flag;

		otl_datetime otl_dt;
		TCString time_tmp = (char *)(vedio_mode_time_gps.empty() ? vedio_mode_time : vedio_mode_time_gps).c_str();
		if (String2OTLDateTime(time_tmp, otl_dt)) {
			otl_s << otl_dt;
		}
		else {
			otl_s << otl_null();
		}
		time_tmp = pkg.pkg_beijing_time.IsEmpty() ? pkg.pkg_homing_pos_time : pkg.pkg_beijing_time;
		if (String2OTLDateTime(time_tmp, otl_dt)) {
			otl_s << otl_dt;
		}
		else {
			otl_s << otl_null();
		}
		time_tmp = TCTime::Now();
		if (String2OTLDateTime(m_sRecvTime, otl_dt)) {
			otl_s << otl_dt;
		}
		else {
			otl_s << otl_null();
		}
		if (String2OTLDateTime(time_tmp, otl_dt)) {
			otl_s << otl_dt;
		}
		else {
			otl_s << otl_null();
		}
		otl_s << threshold_flag;
	}
	otl_s.flush();
	otl_s.close();
	__LEAVEFUNCTION;
}

void TCPigeonLocationHomingPosHandler::OutputRpBzRacingPigeonHomingPos(const TCString & pkg_content, HomingPosPkg & pkg)
{
	__ENTERFUNCTION;
	//: �ֽ��û�����
	TCStringList lsTrackerPkgList;
	lsTrackerPkgList.Clear();
	lsTrackerPkgList.CommaText(m_sPkg_Content, m_cDelimter);
	
	// ��������
	std::vector<RacingPigeonsDataHomingInfo> racing_pigeons_data_homing_infos;
	racing_pigeons_data_homing_infos.clear();
	int pigeon_num = StrToInt(lsTrackerPkgList[9]);
	RacingPigeonsDataHomingInfo pigeons_homing_info;
	// ��ȡ���������������Ϣ
	otl_stream otl_s;
	std::string tb_name;
	std::string sql_buf;
	for (int i = 0; i < pigeon_num; i ++) {
		pigeons_homing_info.Clear();
		pigeons_homing_info.type_ = StrToInt(lsTrackerPkgList[10 + 7 * i]);
		pigeons_homing_info.rfid_ = lsTrackerPkgList[11 + 7 * i];
		pigeons_homing_info.rfid_chk_1_ = lsTrackerPkgList[12 + 7 * i];
		pigeons_homing_info.bi_ringid_ = lsTrackerPkgList[13 + 7 * i];
		pigeons_homing_info.sex_ = StrToInt(lsTrackerPkgList[14 + 7 * i]);
		pigeons_homing_info.color_ = StrToInt(lsTrackerPkgList[15 + 7 * i]);
		pigeons_homing_info.sandeye_ = StrToInt(lsTrackerPkgList[16 + 7 * i]);
		// ����rfid����RP_BZ_IRING ��ȡ IRINGID, IRING_IMEI,IRING_BLEMAC, IRING_MOBILE, IRING_IMSI
		// IRING_RFID=Pkg.RFIDx
		tb_name.clear();
		tb_name = "RP_BZ_IRING";
		sql_buf.clear();
		sql_buf = "SELECT IRINGID, IRING_IMEI,IRING_BLEMAC, IRING_MOBILE, IRING_IMSI FROM ";
		sql_buf += tb_name;
		sql_buf += " WHERE IRING_RFID='";
		sql_buf += pigeons_homing_info.rfid_;
		sql_buf += "'";
		otl_s.open(10, sql_buf.c_str(), m_dbConnect);
		if (!otl_s.eof()) {
			otl_s >> pigeons_homing_info.iringid_;
			otl_s >> pigeons_homing_info.iring_imei_;
			otl_s >> pigeons_homing_info.iring_blemac_;
			otl_s >> pigeons_homing_info.iring_mobile_;
			otl_s >> pigeons_homing_info.iring_imsi_;
			racing_pigeons_data_homing_infos.push_back(pigeons_homing_info);
		}
		otl_s.flush();
		otl_s.close();
		// TODO:RP_BZ_IRING��ѯ���Ϊ��
	}
	//
	RpBzNestPadInfo pad_info;
	std::map<std::string, RpBzNestPadInfo>::iterator iter_pad = bz_nest_pad_infos.find((char *)pkg.pkg_imei);
	if (iter_pad != bz_nest_pad_infos.end()) {
		pad_info = iter_pad->second;
	}
	//���
	tb_name = "RP_BZ_RACINGPIGEON";
	sql_buf = "INSERT INTO ";
	sql_buf += tb_name;
	sql_buf += "(";
	sql_buf += "OWNERID,";
	sql_buf += "OWNERNAME,";
	sql_buf += "OWNERNAME_CHN,";
	sql_buf += "DORM_ID,";
	sql_buf += "DORM_NAME,";
	sql_buf += "DORM_ADDRESS,";
	sql_buf += "DORM_LONGITUDE,";
	sql_buf += "DORM_LATITUDE,";
	sql_buf += "NESTPAD_ID,";
	sql_buf += "NESTPAD_IMEI,";
	sql_buf += "NESTPAD_BLEMAC,";
	sql_buf += "NESTPAD_LONGITUDE,";
	sql_buf += "NESTPAD_LATITUDE,";
	sql_buf += "NESTPAD_HIGH,";
	sql_buf += "BI_RINGID,";
	sql_buf += "SEX,";
	sql_buf += "COLOR,";
	sql_buf += "SANDEYE,";
	sql_buf += "IRINGID,";
	sql_buf += "IRING_RFID,";
	sql_buf += "IRING_IMEI,";
	sql_buf += "IRING_BLEMAC,";
	sql_buf += "IRING_MOBILE,";
	sql_buf += "IRING_IMSI,";
	sql_buf += "FIT_TIME,";
	sql_buf += "FIT_MODEL,";
	sql_buf += "NESTPAD_RFID_INITCRC,";
	sql_buf += "FIT_DEV_TYPE,";
	sql_buf += "FIT_DEV_ID,";
	sql_buf += "FIT_DEV_IMEI,";
	sql_buf += "FIT_DEV_BLEMAC,";
	sql_buf += "FIT_DEV_LONGITUDE,";
	sql_buf += "FIT_DEV_LATITUDE,";
	sql_buf += "FIT_DEV_HIGH,";
	sql_buf += "VALID_FLAG,";
	sql_buf += "THRESHOLD_FLAG";
	sql_buf += ")";
	sql_buf += "VALUES";
	sql_buf += "(";
	sql_buf += ":f01OWNERID<char[16]>,";
	sql_buf += ":f02OWNERNAME<char[32]>,";
	sql_buf += ":f03OWNERNAME_CHN<char[32]>,";
	sql_buf += ":f04DORM_ID<char[16]>,";
	sql_buf += ":f05DORM_NAME<char[32]>,";
	sql_buf += ":f06DORM_ADDRESS<char[256]>,";
	sql_buf += ":f07DORM_LONGITUDE<double>,";
	sql_buf += ":f08DORM_LATITUDE<double>,";
	sql_buf += ":f09NESTPAD_ID<char[16]>,";
	sql_buf += ":f10NESTPAD_IMEI<char[16]>,";
	sql_buf += ":f11NESTPAD_BLEMAC<char[16]>,";
	sql_buf += ":f12NESTPAD_LONGITUDE<double>,";
	sql_buf += ":f13NESTPAD_LATITUDE<double>,";
	sql_buf += ":f14NESTPAD_HIGH<int>,";
	sql_buf += ":f15BI_RINGID<char[16]>,";
	sql_buf += ":f16SEX<short>,";
	sql_buf += ":f17COLOR<short>,";
	sql_buf += ":f18SANDEYE<short>,";
	sql_buf += ":f19IRINGID<char[16]>,";
	sql_buf += ":f20IRING_RFID<char[16]>,";
	sql_buf += ":f21IRING_IMEI<char[16]>,";
	sql_buf += ":f22IRING_BLEMAC<char[16]>,";
	sql_buf += ":f23IRING_MOBILE<char[16]>,";
	sql_buf += ":f24IRING_IMSI<char[16]>,";
	sql_buf += ":f25FIT_TIME<timestamp>,";
	sql_buf += ":f26FIT_MODEL<short>,";
	sql_buf += ":f27NESTPAD_RFID_INITCRC<char[16]>,";
	sql_buf += ":f28FIT_DEV_TYPE<short>,";
	sql_buf += ":f29FIT_DEV_ID<char[16]>,";
	sql_buf += ":f30FIT_DEV_IMEI<char[16]>,";
	sql_buf += ":f31FIT_DEV_BLEMAC<char[16]>,";
	sql_buf += ":f32FIT_DEV_LONGITUDE<double>,";
	sql_buf += ":f33FIT_DEV_LATITUDE<double>,";
	sql_buf += ":f34FIT_DEV_HIGH<int>,";
	sql_buf += ":f35VALID_FLAG<short>,";
	sql_buf += ":f36THRESHOLD_FLAG<short>";	
	sql_buf += ")";
	// ���
	otl_s.open(10, sql_buf.c_str(), m_dbConnect);
	for (int i = 0; i < racing_pigeons_data_homing_infos.size(); i++) {
		otl_s << pad_info.ownerid_;
		otl_s << pad_info.ownername_;
		otl_s << pad_info.ownername_chn_;
		otl_s << pad_info.dorm_id_;
		otl_s << pad_info.dorm_name_;
		otl_s << pad_info.dorm_address_;
		otl_s << pad_info.dorm_longitude_;
		otl_s << pad_info.dorm_latitude_;
		otl_s << pad_info.nestpad_id_;
		otl_s << (char *)pkg.pkg_imei;
		otl_s << pad_info.nestpad_blemac_;
		otl_s << StrToFloat(pkg.pkg_longitude);
		otl_s << StrToFloat(pkg.pkg_latitude);
		otl_s << (int)StrToInt(pkg.pkg_altitude);
		otl_s << racing_pigeons_data_homing_infos[i].bi_ringid_;
		otl_s << racing_pigeons_data_homing_infos[i].sex_;
		otl_s << racing_pigeons_data_homing_infos[i].color_;
		otl_s << racing_pigeons_data_homing_infos[i].sandeye_;		
		otl_s << racing_pigeons_data_homing_infos[i].iringid_;
		otl_s << racing_pigeons_data_homing_infos[i].rfid_;
		otl_s << racing_pigeons_data_homing_infos[i].iring_imei_;
		otl_s << racing_pigeons_data_homing_infos[i].iring_blemac_;
		otl_s << racing_pigeons_data_homing_infos[i].iring_mobile_;
		otl_s << racing_pigeons_data_homing_infos[i].iring_imsi_;
		
		otl_datetime otl_dt;
		// ҵ��ʱ��
		TCString ts_time;
		ts_time = pkg.pkg_beijing_time.IsEmpty() ? pkg.pkg_homing_pos_time : pkg.pkg_beijing_time;
		if (String2OTLDateTime(ts_time, otl_dt)) {
			otl_s << otl_dt;
		}
		else {
			otl_s << otl_null();
		}
		short fit_model = 1;
		otl_s << fit_model;
		otl_s << racing_pigeons_data_homing_infos[i].rfid_chk_1_;
		short fit_dev_type = 1;
		otl_s << fit_dev_type;
		otl_s << pad_info.nestpad_id_;
		otl_s << (char *)pkg.pkg_imei;
		otl_s << pad_info.nestpad_blemac_;
		otl_s << StrToFloat(pkg.pkg_longitude);
		otl_s << StrToFloat(pkg.pkg_latitude);
		otl_s << (int)StrToInt(pkg.pkg_altitude);
		short valid_flag = 1;
		otl_s << valid_flag;
		otl_s << threshold_flag;
	}
	otl_s.flush();
	otl_s.close();
	__LEAVEFUNCTION;
}

bool TCPigeonLocationHomingPosHandler::CheckDistance(TCCustomUniSocket & cusSocket, const HomingPosPkg & pkg)
{
	std::map<std::string, RpBzNestPadInfo>::iterator nest_pad_iter;
	nest_pad_iter = bz_nest_pad_infos.find((char *)pkg.pkg_imei);
	//�Ҳ������¼���
	if (nest_pad_iter == bz_nest_pad_infos.end()) {
		LoadRpBzNestPad();
	}
	nest_pad_iter = bz_nest_pad_infos.find((char *)pkg.pkg_imei);
	if (nest_pad_iter != bz_nest_pad_infos.end()) {
		double distance_homing_pos_pkg = get_distance(StrToFloat(pkg.pkg_latitude),
			StrToFloat(pkg.pkg_longitude),
			nest_pad_iter->second.nestpad_latitude_, nest_pad_iter->second.nestpad_longitude_);
		if (distance_homing_pos_pkg > distance_threshold) {
			return false;
		}	else {
			return true;
		}
	}	else {
		return false;
	}
}

bool TCPigeonLocationHomingPosHandler::HeartBeatingRespHandle(TCCustomUniSocket  &cusSocket,
	const HomingPosPkg & pkg) 
{
	// ���ݱ���ͷ�����IMEI��Ϣ����ѯ�ڴ��л�ȡNESTPAD_ID��  
	// �鳲���ص�GPS (NESTPAD_LONGITUDE, NESTPAD_LATITUDE)
	// �ж��鳲���ľ�γ��gps_flag
	if ((pkg.pkg_gps_flag == "1") && (pkg.pkg_latitude != "") && (pkg.pkg_longitude != "")) {
		return CheckDistance(cusSocket, pkg);
	}
	return true;
}

bool TCPigeonLocationHomingPosHandler::Back2NestRespHandle(TCCustomUniSocket  & cusSocket,
	const HomingPosPkg & pkg)
{
	// TODO:�鳲���жϣ���RP_BZ_RACINGPIGEON ���NESTPAD_IMEI��Pkg.IMEI�Ա��Ƿ� һ��
	//: �ֽ��û�����
	/*TCStringList lsTrackerPkgList;
	lsTrackerPkgList.Clear();
	lsTrackerPkgList.CommaText(m_sPkg_Content, m_cDelimter);
	int rfid_num = StrToInt(lsTrackerPkgList[9]);
	// ��ȡrfid��
	std::vector<std::string> rfids;
	for (int i = 0; i < rfid_num; i++) {		
		rfids.push_back((char *)lsTrackerPkgList[12 + 6 * i]);
	}
	//��rfid������ȡRP_BZ_IRING ���ܻ�id
	//�����ܻ���ID���� RP_BZ_RACINGPIGEON ���ȡNESTPAD_IMEI
	//NESTPAD_IMEI��Pkg.IMEI�Ա��Ƿ� һ��
	std::string sql_buf;
	std::string nestpad_imei;
	otl_stream otl_s;
	for (int i = 0; i < rfids.size(); i++)
	{
		sql_buf.clear();
		nestpad_imei.clear();
		sql_buf = "SELECT B.NESTPAD_IMEI FROM RP_BZ_IRING A LEFT JOIN RP_BZ_RACINGPIGEON B";
		sql_buf += " ON A.IRINGID = B.IRINGID  WHERE A.IRING_RFID ='";
		sql_buf += rfids[i];
		sql_buf += "'";
		otl_s.open(1, sql_buf.c_str(), m_dbConnect);
		if (!otl_s.eof())
		{
			otl_s >> nestpad_imei;
		}
		if (nestpad_imei != (char *)pkg.pkg_imei)	{
			std::cerr << "NESTPAD_IMEI �� pkg.imei ��һ��" << std::endl;
			return false;
		}
	}*/
	// �ж�����
	return CheckDistance(cusSocket, pkg);
}

bool TCPigeonLocationHomingPosHandler::FlashMemDataRespHandle(TCCustomUniSocket & cusSocket, const HomingPosPkg & pkg)
{
	// TODO:�鳲���жϣ���RP_BZ_RACINGPIGEON ���NESTPAD_IMEI��Pkg.IMEI�Ա��Ƿ� һ��

	// �ж�����
	return Back2NestRespHandle(cusSocket, pkg);
}

bool TCPigeonLocationHomingPosHandler::Back2NestVideoRespHandle(TCCustomUniSocket & cusSocket, const HomingPosPkg & pkg)
{
	// TODO:�鳲���жϣ���RP_BZ_RACINGPIGEON ���NESTPAD_IMEI��Pkg.IMEI�Ա��Ƿ� һ��
	// �ж�����
	return Back2NestRespHandle(cusSocket, pkg);
}

bool TCPigeonLocationHomingPosHandler::RacingPigeonsDataRespPosHandle(TCCustomUniSocket & cusSocket, const HomingPosPkg & pkg)
{
	return CheckDistance(cusSocket, pkg);
}

inline double TCPigeonLocationHomingPosHandler::radian(double d)
{
	return d * PI / 180.0;   //�Ƕ�1? = �� / 180
}

inline double TCPigeonLocationHomingPosHandler::get_distance(double lat1, double lng1, double lat2, double lng2)
{
	double radLat1 = radian(lat1);
	double radLat2 = radian(lat2);

	double a = radLat1 - radLat2;
	double b = radian(lng1) - radian(lng2);

	double dst = 2 * asin((sqrt(pow(sin(a / 2), 2) + cos(radLat1) * cos(radLat2) * pow(sin(b / 2), 2))));

	dst = dst * EARTH_RADIUS;
	dst = round(dst * 10000) / 10000;
	return dst;
}

//==========================================================================
// ���� : TCPigeonLocationHomingPosHandler::RecvRequest(TCCustomUniSocket  &cusSocket)
// ��; :	pad�����ݴ���ʽ����������, ��������������
// ԭ�� : void RecvRequest()
// ���� : ��
// ���� : ��
// ˵�� : 
//==========================================================================
void TCPigeonLocationHomingPosHandler::RecvRequest(TCCustomUniSocket  &cusSocket){
	try{
		const int nMaxPkg_Length=1024*1000;
		char sbuff[nMaxPkg_Length];	
		memset(sbuff, 0, sizeof(sbuff));
		m_vsPkgList.clear();
		m_sReq_Command="";
		m_sPkg_Content="";
				
		int nTimeOut= 600 * 1000;
		
		//: ��ʼ���� select �������ж�
		if(!cusSocket.WaitForData(nTimeOut)){
			//: ��ʱ120���ˣ�����ֱ�ӹر����ӣ�
			TCString sLog="Vehicle Tracker[" + m_sTacker_Send_IPAddress + "]�Ѿ���ʱ120��δ�������ݣ��Ͽ�����";
			printf("%s\n", (char*)sLog);
			glgMls->AddPigeonLocationRunLog(sLog);
			throw TCException(sLog);
		}
		
		int nGetPkgLen=0;
		nGetPkgLen=cusSocket.ReceiveBuf(sbuff, nMaxPkg_Length);
		m_sRecvTime = TCTime::Now();

		if(nGetPkgLen==0){
			TCString sLog="Vehicle Tracker[" + m_sTacker_Send_IPAddress + "]�Ѿ��Ͽ�����";
			printf("%s\n", (char*)sLog);
			glgMls->AddPigeonLocationRunLog(sLog);
			throw TCException(sLog);
		}
			
		TCString sPkg_Content(sbuff, nGetPkgLen);
		
    if(Length(sPkg_Content)<4){
    	//: �쳣����������
    	TCString sLogMsg;
			sLogMsg = "";
			sLogMsg += TCString("ERROR: �����ĳ����쳣=[")+IntToStr(Length(sPkg_Content))+"]" ;
	  	glgMls->AddPigeonLocationRunLog(sLogMsg);
    	printf("Error Packet Format: %s\n", (char*)sLogMsg);
    	cusSocket.Close();
			throw TCException(sLogMsg);
    }
    
    m_sReq_Command=Mid(sPkg_Content, 3, 2);
    m_cDelimter=',';
    m_sPkg_Content=Mid(sPkg_Content, 6, Length(sPkg_Content)-4);

    //: ��Ҫ�жϺ����Ƿ��ж�����¼������ǣ��ͷŵ�һ��vector�б���;
    if(AT(m_sPkg_Content, "AT")>0) {
			//: ���滹�ж�����¼
			TCString cmd_str = "AT," + m_sReq_Command;
			m_sPkg_Content = cmd_str + m_sPkg_Content;
			m_sPkg_Content=ReplaceAllSubStr(m_sPkg_Content, cmd_str, ";");
			m_sPkg_Content = Mid(m_sPkg_Content, 2, Length(m_sPkg_Content));
			TCStringList lsRcdPkgList;
      lsRcdPkgList.Clear();
      lsRcdPkgList.CommaText(m_sPkg_Content, ';');
		for(int nPkgSeq=0; nPkgSeq<lsRcdPkgList.GetCount(); nPkgSeq++){
		m_vsPkgList.push_back(lsRcdPkgList[nPkgSeq]);
		//
		std::cout << (char *)lsRcdPkgList[nPkgSeq] << std::endl;
	}
    }else{
	m_vsPkgList.push_back(m_sPkg_Content);
    }
    
    //======== 5. ��¼��־ =============
    TCString sLogMsg;
		sLogMsg = "";
		sLogMsg += TCString("����������:(") + m_sReq_Command, +"), ��¼��=("+IntToStr(m_vsPkgList.size());
		sLogMsg += TCString(") ���ݣ��� ") + m_sPkg_Content;
		sLogMsg += TCString(") ���������\n");
    glgMls->AddPigeonLocationRunLog(sLogMsg);
  }catch (TCException &e){
    cusSocket.Close();
    throw e;
  }catch (...){
    cusSocket.Close();
    throw TCException("TCPigeonLocationHomingPosHandler::RecvRequest() Error:"
                        "Unknown Exception.");
  }
}

//==========================================================================
// ���� : TCPigeonLocationHomingPosHandler::SendRespPkg
// ��; : ����Ӧ���
// ԭ�� : void SendRespPkg(TCString sRespCommand, TCString sRespContent)
// ���� : sRespCommand---�����������   sRespContent ---- ��������(ǰ�����ֽھ���������)
// ���� : ��
// ˵�� :
//==========================================================================
void TCPigeonLocationHomingPosHandler::SendRespPkg(TCCustomUniSocket  &cusSocket, TCString  msg_id, TCString  msg_content){
	TCString sTmpRespContent="AT";
	sTmpRespContent += msg_id;
	sTmpRespContent += ",";
	sTmpRespContent += msg_content;
	sTmpRespContent+=",TA";

  cusSocket.SendBuf((char *)sTmpRespContent, Length(sTmpRespContent));

	m_sSendTime=TCTime::Now();

#ifdef __TEST__		
	printf("RecvTime=[%s], SendTime=[%s], sTmpRespContent=[%s]\n", (char*)m_sRecvTime, (char*)m_sSendTime, (char*)sTmpRespContent);
#endif

}

//==========================================================================
// ���� : void TCPigeonLocationHomingPosHandler::DoCommand_PigeonLocInfo
// ��; : ƽ���ϱ�λ����Ϣ
// ԭ�� : void DoCommand_PigeonLocInfo()
// ���� : ��
// ���� : ��
// ˵�� :  
//==========================================================================
void TCPigeonLocationHomingPosHandler::DoCommand_LocInfo(TCCustomUniSocket  &cusSocket){
	field_counts = 8;
	for(int nPkgSeq=0; nPkgSeq<m_vsPkgList.size(); nPkgSeq++){
		std::srand(time(0));
		m_sPkg_Content = m_vsPkgList[nPkgSeq];
		//: �ֽ��û�����
		TCStringList lsTrackerPkgList;
		lsTrackerPkgList.Clear();
		lsTrackerPkgList.CommaText(m_sPkg_Content, m_cDelimter);
		
		if(lsTrackerPkgList.GetCount() < field_counts){
			//: ����8��������
			TCString sLogMsg;
			sLogMsg = "";
			sLogMsg += TCString("����������:(") + m_sReq_Command;
			sLogMsg += TCString(") �����쳣��������=[") + IntToStr(lsTrackerPkgList.GetCount())+"]";
			glgMls->AddPigeonLocationRunLog(sLogMsg);
			printf("�����쳣TCPigeonLocationHomingPosHandler::DoCommand_PigeonLocInfo: %s\n", (char *)sLogMsg);
			throw TCException(sLogMsg);
		}
		//���Ľ���
		HomingPosPkg pkg;
		MsgParsing(m_sPkg_Content, pkg);
		//�ж���Ϣ����
		TCString msg_id = lsTrackerPkgList[8];
		if (msg_id == "10") {
			// TODO:����Խ���ж�
			if (12 > lsTrackerPkgList.GetCount()) {
				return;
			}
			int err_rfid_num = StrToInt(lsTrackerPkgList[10]);
			int err_ble_num = StrToInt(lsTrackerPkgList[12]);
			TCString resp_status = "0";
			if (err_rfid_num > 0 || err_ble_num > 0) {
				resp_status = "1";
			}
			// ������
			if (!HeartBeatingRespHandle(cusSocket, pkg)) {
				TCString sLogMsg;
				sLogMsg = "";
				sLogMsg += TCString("ERROR: ���볬����ֵ.");
				glgMls->AddPigeonLocationRunLog(sLogMsg);

				//TCString resp_content = delay_seconds + "," + TCTime::Now() + "," + resp_status;
				//SendRespPkg(cusSocket, TCString("11"), resp_content);
				threshold_flag = 1;
				//// ���쳣��main_handle�ر�����
				//throw TCException(sLogMsg);
			};

			TCString resp_content= delay_seconds + "," + TCTime::Now() + "," + resp_status;
			SendRespPkg(cusSocket, TCString("11"), resp_content);
			OutputRpBrDevStatusMgr(pkg, 1, 1);
		}	else if (msg_id == "12") {
			// ����鳲ָʾ
			//TODO:����Խ���ж�
			if (9 > lsTrackerPkgList.GetCount()) {
				return;
			}
			TCString resp_content = lsTrackerPkgList[9] + ",";
			int rfid_num = StrToInt(lsTrackerPkgList[9]);
			TCString ble_num = lsTrackerPkgList[9 + rfid_num * 6 + 1];
			resp_content += ble_num;
			if (!Back2NestRespHandle(cusSocket, pkg)) {
				// 
				TCString sLogMsg;
				sLogMsg = "";
				sLogMsg += TCString("ERROR: ���볬����ֵ.");
				glgMls->AddPigeonLocationRunLog(sLogMsg);

				//SendRespPkg(cusSocket, TCString("13"), resp_content);
				threshold_flag = 1;
				//throw TCException(sLogMsg);
			};
			SendRespPkg(cusSocket, TCString("13"), resp_content);
			//
			// ��� RP_BR_NESTPAD_SUBPROC ����
			OutputRpBrNestPadSubproc(m_sPkg_Content, pkg, 13);
			//RP_BR_DEVSTATUS_MGR
			OutputRpBrDevStatusMgr(pkg, 1, 2);
		}	else if (msg_id == "14") {
			TCString resp_content = lsTrackerPkgList[9] + ",";
			int rfid_num = StrToInt(lsTrackerPkgList[9]);
			TCString ble_num = lsTrackerPkgList[9 + rfid_num * 6 + 1];
			resp_content += ble_num;
			// �鳲��λ���ϴ��洢�ķ�����δ�������յ�RFID��ʱ������
			if (!FlashMemDataRespHandle(cusSocket, pkg)) {
				TCString sLogMsg;
				sLogMsg = "";
				sLogMsg += TCString("ERROR: ���볬����ֵ.");
				glgMls->AddPigeonLocationRunLog(sLogMsg);

				//SendRespPkg(cusSocket, TCString("15"), resp_content);
				threshold_flag = 1;
				//throw TCException(sLogMsg);
			}

			SendRespPkg(cusSocket, TCString("15"), resp_content);
			OutputRpBrNestPadSubproc(m_sPkg_Content, pkg, 15);
			OutputRpBrDevStatusMgr(pkg, 1, 2);
		}	else if (msg_id == "16") {
			// ���ݱ���ͷ��ȡ��Ƶ֡���Լ�����
			if (12 > lsTrackerPkgList.GetCount()) {
				return;
			}
			int rfid_num = StrToInt(lsTrackerPkgList[12]);
			int flame_num = StrToInt(lsTrackerPkgList[14 + rfid_num]);
			int flame_num_size = StrToInt(lsTrackerPkgList[15 + rfid_num]);
			long flame_total_size = flame_num * flame_num_size;
			char *flame_ptr = new char[flame_total_size];

			TCString resp_content = IntToStr(flame_total_size);
			//std::cout << "flame_total_size:" << flame_total_size << std::endl;
			//			
			int rlen = 0;
			// ������Ӧ���ȵ���Ƶ����
			for (int len = flame_total_size; len; len = len - rlen)
			{
				rlen = cusSocket.ReceiveBuf(flame_ptr + rlen, len);
			}
			// ���ս����ַ�",TA"
			char ch[3] = {0};
	
			rlen = cusSocket.ReceiveBuf(ch, sizeof(ch));
			if (ch[1] != 'T' && ch[1] != 'A') {
				std::cout << "��Ƶ���Ĵ����쳣" << std::endl;
			}
			if (!Back2NestVideoRespHandle(cusSocket, pkg)) {
				TCString sLogMsg;
				sLogMsg = "";
				sLogMsg += TCString("ERROR: ���볬����ֵ.");
				glgMls->AddPigeonLocationRunLog(sLogMsg);

				//SendRespPkg(cusSocket, TCString("17"), resp_content);
				threshold_flag = 1;
				//throw TCException(sLogMsg);
			}
			SendRespPkg(cusSocket, TCString("17"), resp_content);
			OutputRpBrNestVideoSubproc(m_sPkg_Content, pkg, flame_ptr, flame_total_size);

			////��Ƶ������ļ�
			std::fstream iof("./vedio_check", std::ios::ate|std::ios::out|std::ios::binary);
			iof.write(flame_ptr, flame_total_size);
			iof.close();
			delete[] flame_ptr;

			OutputRpBrPigeonNestVideo(m_sPkg_Content, pkg);
			OutputRpBrDevStatusMgr(pkg, 1, 2);
		}	else if (msg_id == "18") {
			// ���������Ϣ�ϴ�
			// TODO:�����±�Խ��
			if (9 > lsTrackerPkgList.GetCount()) {
				return;
			}
			TCString resp_content = lsTrackerPkgList[9];
			if (!RacingPigeonsDataRespPosHandle(cusSocket, pkg)) {
				TCString sLogMsg;
				sLogMsg = "";
				sLogMsg += TCString("ERROR: ���볬����ֵ.");
				glgMls->AddPigeonLocationRunLog(sLogMsg);

				//SendRespPkg(cusSocket, TCString("19"), resp_content);
				threshold_flag = 1;
				//throw TCException(sLogMsg);
			}
			SendRespPkg(cusSocket, TCString("19"), resp_content);
			OutputRpBzRacingPigeonHomingPos(m_sPkg_Content, pkg);
			OutputRpBrDevStatusMgr(pkg, 1, 2);
		}
	}
	return;	
}

//==========================================================================
// ���� : void TCPigeonLocationHomingPosHandler::Init
// ��; : ��ʼ��
// ԭ�� : void Init()
// ���� : ��
// ���� : ��
// ˵�� : 
//==========================================================================
void TCPigeonLocationHomingPosHandler::Init(){
	m_sPkg_Content="";
	m_sReq_Command="";
	m_sRespCommand="";
	m_sRespPkgContent="";
	return;
}


