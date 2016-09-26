/*****************************************************************************
CAPTION
Project Name: �鳲ϵͳ
Description : pad�豸Э�鴦����
File Name   : c_PigeonLocation_VehicleHandler.cpp
Requires    : LinuxAS3 , Oracle 9i
Create Date : 2016-05-04
******************************************************************************/

#pragma hdrstop
#define OTL_STL

#include "c_PigeonLocation_PigeonGatherHandler.h"
#include "c_PigeonLocation_config.h"
#include "c_PigeonLocation_task.h"
#include <iostream>
#include <ctime>

#pragma package(smart_init)

extern TCPigeonLocationHandler gsPigeonLocationHandler;

extern TCPigeonLocationConfig *gPigeonLocationConfig;

//: ���ݿ�����
extern otl_connect m_dbConnect;


extern TCPigeonLocationLog  *glgMls;

extern map<str_Lac_Cell_Node, str_Space_Pos_Unit> g_msCellInfo_PosUnit;

//==========================================================================
// ���� : TCPigeonLocationPigeonGatherHandler::TCPigeonLocationPigeonGatherHandler
// ��; : ���캯��
// ԭ�� : TCPigeonLocationPigeonGatherHandler()
// ���� :
// ���� :
// ˵�� :
//==========================================================================
TCPigeonLocationPigeonGatherHandler::TCPigeonLocationPigeonGatherHandler() {
}

//==========================================================================
// ���� : TCPigeonLocationPigeonGatherHandler::~TCPigeonLocationPigeonGatherHandler()
// ��; : ��������
// ԭ�� : ~TCPigeonLocationPigeonGatherHandler()
// ���� :
// ���� :
// ˵�� :
//==========================================================================
TCPigeonLocationPigeonGatherHandler::~TCPigeonLocationPigeonGatherHandler()
{
}

//==========================================================================
// ���� : TCPigeonLocationPigeonGatherHandler::Main_Handler
// ��; : ��Ҫʵ�־����ҵ�񽻻��߼�
// ԭ�� : void Main_Handler()
// ���� :
// ���� :
// ˵�� : ��Ҫʵ�־����ҵ�񽻻��߼�
//==========================================================================
bool TCPigeonLocationPigeonGatherHandler::Main_Handler(TCCustomUniSocket  &cusSocket) {
	try {
		vender = gPigeonLocationConfig->GetTrackerVendor();
		//: Զ�̷��͵ĵ�ַ
		m_sTacker_Send_IPAddress = cusSocket.GetRemoteAddress();
		m_nTimeOut = 60;		
		distance_threshold = gPigeonLocationConfig->GetDistanceThresholdValue() / 1000.0;
		delay_seconds = "60";
		printf("���յ� Tracker ��������IP=[%s], Port=[%d], Timeout=[%d]\n", (char*)cusSocket.GetRemoteAddress(), cusSocket.GetRemotePort(), m_nTimeOut);
		/*
		��ѯRP_BZ_PIGEON_OWNER����������MOBILE��OWNERID��OWNERNAME��OWNERNAME_CHN��ȡ���ڴ��У�
		����MOBILEΪ�������棬���ڻ��߰��ո�MOBILE��ѯδ��ѯ��ʱ�������ݿ������¸���		
		*/
		LoadRpBzPigeonOwner();
		LoadRpBzGatherDev();
		//
		//: �����ӷ�ʽ
		while (1) {
			m_sRecvTime = TCTime::Now();
			Init();
			TCString sLogMsg = "Time:Now=[" + TCTime::Now() + "],Recv Port=[" + IntToStr(cusSocket.GetRemotePort()) + "] Data";
			glgMls->AddPigeonLocationRunLog(sLogMsg);
			RecvRequest(cusSocket);
			threshold_flag = 0;
			DealRequest(cusSocket);
		}
		cusSocket.Close();
		return true;
	}
	catch (TCException &e) {
		TCString sLogMsg = TCString("At TCPigeonLocationPigeonGatherHandler::Main_Handler ��������ʱ����:") + e.GetExceptionMessage();
		glgMls->AddPigeonLocationRunLog(sLogMsg);
		cusSocket.Close();
		return false;
	}
	catch (...) {
		glgMls->AddPigeonLocationRunLog("At TCPigeonLocationPigeonGatherHandler::Main_Handler Unknow, �ر�Socket����");
		cusSocket.Close();
		return false;
	}
}

//==========================================================================
// ���� : TCPigeonLocationPigeonGatherHandler::DealRequest
// ��; : ��������
// ԭ�� : void DealRequest()
// ���� : ��
// ���� : ��
// ˵�� :
//==========================================================================
void TCPigeonLocationPigeonGatherHandler::DealRequest(TCCustomUniSocket  &cusSocket) {
	try {
		//: ���������ֽ��д���
#ifdef __TEST__
		printf("============receive Tracker request==============\n");
		printf("Command:=%s, Content=[%s], Time=[%s]\n", (char*)m_sReq_Command, (char*)m_sPkg_Content, (char*)TCTime::Now());
#endif

		//======ִ�з������==========================
		m_sRespCommand = "";
		m_sRespPkgContent = "";
		if (m_sReq_Command == "13") {
			DoCommand_LocInfo(cusSocket);
		}
		else {
			//: δ֪����
			TCString sLogMsg = TCString("δ֪����,m_sReq_Command: [") + m_sReq_Command + "]";
			glgMls->AddPigeonLocationRunLog(sLogMsg);
			return;
		}
	}
	catch (TCException &e) {
		try {
			TCString sLogMsg = TCString("ϵͳ����,At DealRequest: ") + e.GetExceptionMessage();
			glgMls->AddPigeonLocationRunLog(sLogMsg);
			throw e;
		}
		catch (TCException &e1) {
			TCString sLogMsg = TCString("ϵͳ����: when Catch a Error and DealRequest: ") + e1.GetExceptionMessage();
			glgMls->AddPigeonLocationRunLog(sLogMsg);
			throw e1;
		}
		catch (...) {
			glgMls->AddPigeonLocationRunLog("ϵͳ����δ֪�쳣");
			throw;
		}
	}
}

void TCPigeonLocationPigeonGatherHandler::MsgParsing(const TCString & content, PigeonGatherPkg & pkg)
{
	__ENTERFUNCTION;
	TCStringList lsTrackerPkgList;
	lsTrackerPkgList.Clear();
	lsTrackerPkgList.CommaText(content, m_cDelimter);

	pkg.pkg_gather_imei = lsTrackerPkgList[0];
	pkg.pkg_gather_time = lsTrackerPkgList[1].IsEmpty() ? TCTime::Now() : pkg.pkg_gather_time;
	pkg.pkg_gps_flag = lsTrackerPkgList[2];
	pkg.pkg_beijing_time = lsTrackerPkgList[3].IsEmpty() ? TCTime::Now() : TCTime::RelativeTime(lsTrackerPkgList[3], 8 * 3600);
	pkg.pkg_latitude = lsTrackerPkgList[4];
	pkg.pkg_longitude = lsTrackerPkgList[5];
	pkg.pkg_altitude = lsTrackerPkgList[6];
	pkg.pkg_voltage = lsTrackerPkgList[7];
	pkg.pkg_msg_id = lsTrackerPkgList[8];
	std::cout << "PKG:" << (char *)m_sPkg_Content << std::endl;
	__LEAVEFUNCTION;
}

std::string TCPigeonLocationPigeonGatherHandler::JointPkgFields(
	const std::vector<RpBzGatherNestPadInfo>& pad_infos,
	const std::vector<RFIDStatusRespInfo>& rfid_infos)
{
	__ENTERFUNCTION;
	std::string buf = (char *)IntToStr(pad_infos.size());
	buf += ",";
	// ���ἰ�鳲����Ϣ
	for (int i = 0; i < pad_infos.size(); i++) {
		buf += pad_infos[i].dorm_id_;
		buf += ",";
		buf += pad_infos[i].dorm_name_;
		buf += ",";
		buf += pad_infos[i].dorm_address_;
		buf += ",";
		buf += pad_infos[i].nestpad_index_;
		buf += ",";
		buf += pad_infos[i].nestpad_id_;
		buf += ",";
		buf += pad_infos[i].nestpad_imei_;
		buf += ",";
	}
	// rfid ��Ϣ
	buf += (char *)IntToStr(rfid_infos.size());
	buf += ",";
	for (int i = 0; i < rfid_infos.size(); i++) {
		buf += rfid_infos[i].rfid_status_;
		buf += ",";
		buf += rfid_infos[i].bi_ringid_;
		buf += ",";
		buf += rfid_infos[i].rfid_type_;
		buf += ",";
		buf += rfid_infos[i].nestpad_imei_;
		buf += ",";
	}
	return buf.substr(0, buf.length() - 1);
	__LEAVEFUNCTION;
}

void TCPigeonLocationPigeonGatherHandler::GetRacingPigeonInfosByRfid(RacingPigeon & racing_pegeons)
{
	/*
		ͨ��rfidȥRP_BZ_RACINGPIGEON��ѯ OWNERID��OWNERNAME��OWNERNAME_CHN,BI_RINGID,SEX,COLOR,
		SANDEYE,IRINGID,IRING_IMEI,IRING_BLEMAC,IRING_MOBILE
	*/
	__ENTERFUNCTION;
	std::string tb_name = "RP_BZ_RACINGPIGEON";
	std::string sql_buf;
	otl_stream otl_s;
	sql_buf.clear();
	sql_buf = "SELECT OWNERID,OWNERNAME,OWNERNAME_CHN,BI_RINGID,SEX,COLOR,";
	sql_buf += "SANDEYE,IRINGID, IRING_IMEI, IRING_BLEMAC, IRING_MOBILE FROM ";
	sql_buf += tb_name;
	sql_buf += " WHERE IRING_RFID='";
	sql_buf += racing_pegeons.rfid;
	sql_buf += "'";

	otl_s.open(1, sql_buf.c_str(), m_dbConnect);
	if (!otl_s.eof()) {
			otl_s >> racing_pegeons.owner_id;
			otl_s >> racing_pegeons.owner_name;
			otl_s >> racing_pegeons.ownername_chn;
			otl_s >> racing_pegeons.bi_ringid;
			otl_s >> racing_pegeons.sex;
			otl_s >> racing_pegeons.color;
			otl_s >> racing_pegeons.sandeye;
			otl_s >> racing_pegeons.iringid;
			otl_s >> racing_pegeons.iring_imei;
			otl_s >> racing_pegeons.iring_blemac;
			otl_s >> racing_pegeons.iring_mobile;
		}	else {
			// �����
			racing_pegeons.rfid = "-1";
		}
		otl_s.flush();
		otl_s.close();

	__LEAVEFUNCTION;
}

void TCPigeonLocationPigeonGatherHandler::GetRpBzGatherNestPadInfo(std::string owner_id,
	std::vector<RpBzGatherNestPadInfo>& gather_nest_pad_infos)
{
	__ENTERFUNCTION;
	std::string sql_buf = "SELECT DORM_ID,DORM_NAME,DORM_ADDRESS,NESTPAD_ID,NESTPAD_INDEX,NESTPAD_IMEI ";
	sql_buf += " FROM RP_BZ_NESTPAD A WHERE A.OWNERID='";
	sql_buf += owner_id;
	sql_buf += "'";

	otl_stream otl_s;
	long nest_index = 0;
	otl_s.open(10, sql_buf.c_str(), m_dbConnect);
	RpBzGatherNestPadInfo gather_pad_info;
	while (!otl_s.eof())	{
		gather_pad_info.Clear();
		gather_pad_info.owner_id_ = owner_id;
		otl_s >> gather_pad_info.dorm_id_;
		otl_s >> gather_pad_info.dorm_name_;
		otl_s >> gather_pad_info.dorm_address_;
		otl_s >> gather_pad_info.nestpad_id_;
		otl_s >> nest_index;
		gather_pad_info.nestpad_index_ = (char *)IntToStr(nest_index);
		otl_s >> gather_pad_info.nestpad_imei_;
		gather_nest_pad_infos.push_back(gather_pad_info);
	}
	otl_s.flush();
	otl_s.close();
	__LEAVEFUNCTION;
}

bool TCPigeonLocationPigeonGatherHandler::GetRFIDStatusRespInfo(std::vector<RFIDStatusRespInfo>& rfidstatusrespinfo)
{
	/*
	ͨ��rfidȥRP_BZ_RACINGPIGEON��ѯOWNERID,OWNERNAM,OWNERNAME_CH,BI_RINGID,DORM_ID,DORM_ADDRESS
	*/
	__ENTERFUNCTION;
	std::string tb_name = "RP_BZ_RACINGPIGEON";
	std::string sql_buf;
	otl_stream otl_s;
	RacingPigeon info;
	int null_num = 0;
	for (int i = 0; i < rfidstatusrespinfo.size(); i++) {
		sql_buf.clear();
		info.Clear();
		sql_buf = "SELECT OWNERID,OWNERNAME,OWNERNAME_CHN,BI_RINGID,DORM_ID,DORM_ADDRESS, NESTPAD_IMEI";
		sql_buf += " FROM ";
		sql_buf += tb_name;
		sql_buf += " WHERE IRING_RFID='";
		sql_buf += rfidstatusrespinfo[i].rfid_;
		sql_buf += "'";

		otl_s.open(1, sql_buf.c_str(), m_dbConnect);
		if (!otl_s.eof()) {
			otl_s >> rfidstatusrespinfo[i].ownerid_;
			otl_s >> rfidstatusrespinfo[i].ownername_;
			otl_s >> rfidstatusrespinfo[i].ownername_chn_;
			otl_s >> rfidstatusrespinfo[i].bi_ringid_;
			otl_s >> rfidstatusrespinfo[i].dorm_id_;
			otl_s >> rfidstatusrespinfo[i].dorm_address_;
			otl_s >> rfidstatusrespinfo[i].nestpad_imei_;
			// ��rfid�ѱ���
			rfidstatusrespinfo[i].rfid_status_ = "1";
		}	else {
			null_num++;
		}
		otl_s.flush();
		otl_s.close();

		// �ж��Ƿ�Ϊ���ܻ�����rfid��ѯRP_BZ_IRING����Ϊ���ܷ���Ϊ��ͨ
		sql_buf.clear();
		sql_buf = "select 1 from RP_BZ_IRING where IRING_RFID='";
		sql_buf += rfidstatusrespinfo[i].rfid_;
		sql_buf += "'";
		otl_s.open(1, sql_buf.c_str(), m_dbConnect);
		if (!otl_s.eof()) {
			rfidstatusrespinfo[i].rfid_type_ = "1";
		}
		otl_s.flush();
		otl_s.close();
	}
	if (null_num == rfidstatusrespinfo.size()) {
		return false;
	}
	return true;
	__LEAVEFUNCTION;
	return false;
}

void TCPigeonLocationPigeonGatherHandler::GetRacingPigeonsDataRespInfo(std::vector<RacingPigeonsDataRespInfo>& racingpigeonsdatarespinfos)
{
	// ͨ��rfid ��ѯRP_BZ_IRING��ȡ�Ÿ��IRINGID, IRING_IMEI,IRING_BLEMAC, IRING_MOBILE, IRING_IMSI
	__ENTERFUNCTION;
	std::string tb_name = "RP_BZ_IRING";
	std::string sql_buf;
	otl_stream otl_s;
	for (int i = 0; i < racingpigeonsdatarespinfos.size(); i ++) {
		sql_buf.clear();
		std::string sql_buf = "SELECT IRINGID, IRING_IMEI,IRING_BLEMAC, IRING_MOBILE, IRING_IMSI FROM ";
		sql_buf += tb_name;
		sql_buf += "  A WHERE A.IRING_RFID='";
		sql_buf += racingpigeonsdatarespinfos[i].rfid;
		sql_buf += "'";
		otl_s.open(10, sql_buf.c_str(), m_dbConnect);
		if (!otl_s.eof()) {
			otl_s >> racingpigeonsdatarespinfos[i].iringid;
			otl_s >> racingpigeonsdatarespinfos[i].iring_imei;
			otl_s >> racingpigeonsdatarespinfos[i].iring_blemac;
			otl_s >> racingpigeonsdatarespinfos[i].iring_mobile;
			otl_s >> racingpigeonsdatarespinfos[i].iring_imsi;
		}
		otl_s.flush();
		otl_s.close();
	}
	__LEAVEFUNCTION;
}

void TCPigeonLocationPigeonGatherHandler::LoadRpBzPigeonOwner()
{
	__ENTERFUNCTION;
	std::string tb_name = "RP_BZ_PIGEON_OWNER";
	pigeon_owner_infos.clear();
	std::string sql_buf = "SELECT MOBILE, OWNERID, OWNERNAME, OWNERNAME_CHN FROM ";
	sql_buf += tb_name;
	otl_stream otl_s(100, sql_buf.c_str(), m_dbConnect);
	PigeonOwnerInfo info;	
	while (!otl_s.eof())
	{
		info.Clear();
		otl_s >> info.mobile_;
		otl_s >> info.owner_id_;
		otl_s >> info.owner_name_;
		otl_s >> info.ownername_chn_;
		pigeon_owner_infos[info.mobile_] = info;
	}
	otl_s.flush();
	otl_s.close();
	__LEAVEFUNCTION;
}

void TCPigeonLocationPigeonGatherHandler::LoadRpBzGatherDev()
{
	__ENTERFUNCTION;
	std::string tb_name_ = "RP_BZ_GATHER_DEV";
	gather_dev_infos.clear();
	std::string sql_buf = "SELECT GATGERID, GATGER_IMEI, GATGER_BLEMAC, MATCHID,";
	sql_buf += " MATCHNAME, GATHER_ADDR_LATITUDE, GATHER_ADDR_LONGITUDE FROM ";
	sql_buf += tb_name_;
	sql_buf += " WHERE VALID_FLAG=1";
	otl_stream otl_s(100, sql_buf.c_str(), m_dbConnect);
	GatherDevInfo info;
	while (!otl_s.eof()) {
		info.Clear();
		otl_s >> info.gatger_id_;
		if (otl_s.is_null()) {
			info.gatger_id_  = "";
		}			
		otl_s >> info.gatger_imei_;
		if (otl_s.is_null()) {
			info.gatger_imei_ = "";
		}
		otl_s >> info.gatger_blemac_;
		if (otl_s.is_null()) {
			info.gatger_blemac_ = "";
		}
		otl_s >> info.match_id_;
		if (otl_s.is_null()) {
			info.match_id_ = "";
		}
		otl_s >> info.match_name_;
		if (otl_s.is_null()) {
			info.match_name_ = "";
		}
		otl_s >> info.gather_addr_latitude_;
		if (otl_s.is_null()) {
			info.gather_addr_latitude_ = 0.0;
		}
		otl_s >> info.gather_addr_longitude_;
		if (otl_s.is_null()) {
			info.gather_addr_longitude_ = 0.0;
		}
		gather_dev_infos[info.gatger_imei_] = info;
	}
	otl_s.flush();
	otl_s.close();
	__LEAVEFUNCTION;
}

inline double TCPigeonLocationPigeonGatherHandler::radian(double d)
{
	return d * PI / 180.0;   //�Ƕ�1? = �� / 180
}

inline double TCPigeonLocationPigeonGatherHandler::get_distance(double lat1, double lng1, double lat2, double lng2)
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
// ���� : TCPigeonLocationPigeonGatherHandler::RecvRequest(TCCustomUniSocket  &cusSocket)
// ��; :	pad�����ݴ���ʽ����������, ��������������
// ԭ�� : void RecvRequest()
// ���� : ��
// ���� : ��
// ˵�� :
//==========================================================================
void TCPigeonLocationPigeonGatherHandler::RecvRequest(TCCustomUniSocket  &cusSocket) {
	try {
		const int nMaxPkg_Length = 1024 * 1000;
		char sbuff[nMaxPkg_Length];
		memset(sbuff, 0, sizeof(sbuff));
		m_vsPkgList.clear();
		m_sReq_Command = "";
		m_sPkg_Content = "";

		//int nTimeOut = 1800 * 1000;

		////: ��ʼ���� select �������ж�
		//if (!cusSocket.WaitForData(nTimeOut)) {
		//	//: ��ʱ120���ˣ�����ֱ�ӹر����ӣ�
		//	TCString sLog = "Vehicle Tracker[" + m_sTacker_Send_IPAddress + "]�Ѿ���ʱ120��δ�������ݣ��Ͽ�����";
		//	printf("%s\n", (char*)sLog);
		//	glgMls->AddPigeonLocationRunLog(sLog);
		//	throw TCException(sLog);
		//}

		int nGetPkgLen = 0;
		nGetPkgLen = cusSocket.ReceiveBuf(sbuff, nMaxPkg_Length);

		if (nGetPkgLen == 0) {
			TCString sLog = "Vehicle Tracker[" + m_sTacker_Send_IPAddress + "]�Ѿ��Ͽ�����";
			printf("%s\n", (char*)sLog);
			glgMls->AddPigeonLocationRunLog(sLog);
			throw TCException(sLog);
		}

		TCString sPkg_Content(sbuff, nGetPkgLen);

		if (Length(sPkg_Content)<4) {
			//: �쳣����������
			TCString sLogMsg;
			sLogMsg = "";
			sLogMsg += TCString("ERROR: �����ĳ����쳣=[") + IntToStr(Length(sPkg_Content)) + "]";
			glgMls->AddPigeonLocationRunLog(sLogMsg);
			printf("Error Packet Format: %s\n", (char*)sLogMsg);
			cusSocket.Close();
			throw TCException(sLogMsg);
		}

		m_sReq_Command = Mid(sPkg_Content, 3, 2);
		m_cDelimter = ',';
		m_sPkg_Content = Mid(sPkg_Content, 6, Length(sPkg_Content) - 4);

		//: ��Ҫ�жϺ����Ƿ��ж�����¼������ǣ��ͷŵ�һ��vector�б���;
		if (AT(m_sPkg_Content, "AT")>0) {
			//: ���滹�ж�����¼
			TCString cmd_str = "AT," + m_sReq_Command;
			m_sPkg_Content = cmd_str + m_sPkg_Content;
			m_sPkg_Content = ReplaceAllSubStr(m_sPkg_Content, cmd_str, ";");
			m_sPkg_Content = Mid(m_sPkg_Content, 2, Length(m_sPkg_Content));
			TCStringList lsRcdPkgList;
			lsRcdPkgList.Clear();
			lsRcdPkgList.CommaText(m_sPkg_Content, ';');
			for (int nPkgSeq = 0; nPkgSeq<lsRcdPkgList.GetCount(); nPkgSeq++) {
				m_vsPkgList.push_back(lsRcdPkgList[nPkgSeq]);
				//
				std::cout << (char *)lsRcdPkgList[nPkgSeq] << std::endl;
			}
		}
		else {
			m_vsPkgList.push_back(m_sPkg_Content);
		}

		//======== 5. ��¼��־ =============
		TCString sLogMsg;
		sLogMsg = "";
		sLogMsg += TCString("����������:(") + m_sReq_Command, +"), ��¼��=(" + IntToStr(m_vsPkgList.size());
		sLogMsg += TCString(") ���ݣ��� ") + m_sPkg_Content;
		sLogMsg += TCString(") ���������\n");
		glgMls->AddPigeonLocationRunLog(sLogMsg);
	}
	catch (TCException &e) {
		cusSocket.Close();
		throw e;
	}
	catch (...) {
		cusSocket.Close();
		throw TCException("TCPigeonLocationPigeonGatherHandler::RecvRequest() Error:"
			"Unknown Exception.");
	}
}

//==========================================================================
// ���� : TCPigeonLocationPigeonGatherHandler::SendRespPkg
// ��; : ����Ӧ���
// ԭ�� : void SendRespPkg(TCString sRespCommand, TCString sRespContent)
// ���� : sRespCommand---�����������   sRespContent ---- ��������(ǰ�����ֽھ���������)
// ���� : ��
// ˵�� :
//==========================================================================
void TCPigeonLocationPigeonGatherHandler::SendRespPkg(TCCustomUniSocket  &cusSocket, TCString  msg_id, TCString  msg_content) {
	TCString sTmpRespContent = "AT";
	sTmpRespContent += msg_id;
	sTmpRespContent += ",";
	sTmpRespContent += msg_content;
	sTmpRespContent += ",TA";

	cusSocket.SendBuf((char *)sTmpRespContent, Length(sTmpRespContent));

	m_sSendTime = TCTime::Now();

#ifdef __TEST__
	printf("RecvTime=[%s], SendTime=[%s], sTmpRespContent=[%s]\n", (char*)m_sRecvTime, (char*)m_sSendTime, (char*)sTmpRespContent);
#endif

}

//==========================================================================
// ���� : void TCPigeonLocationPigeonGatherHandler::DoCommand_PigeonLocInfo
// ��; : ƽ���ϱ�λ����Ϣ
// ԭ�� : void DoCommand_PigeonLocInfo()
// ���� : ��
// ���� : ��
// ˵�� :
//==========================================================================
void TCPigeonLocationPigeonGatherHandler::DoCommand_LocInfo(TCCustomUniSocket  &cusSocket) {
	field_counts = 8;
	for (int nPkgSeq = 0; nPkgSeq<m_vsPkgList.size(); nPkgSeq++) {
		std::srand(time(0));
		m_sPkg_Content = m_vsPkgList[nPkgSeq];
		//: �ֽ��û�����
		TCStringList lsTrackerPkgList;
		lsTrackerPkgList.Clear();
		lsTrackerPkgList.CommaText(m_sPkg_Content, m_cDelimter);

		if (lsTrackerPkgList.GetCount() < field_counts) {
			//: ����8��������
			TCString sLogMsg;
			sLogMsg = "";
			sLogMsg += TCString("����������:(") + m_sReq_Command;
			sLogMsg += TCString(") �����쳣��������=[") + IntToStr(lsTrackerPkgList.GetCount()) + "]";
			glgMls->AddPigeonLocationRunLog(sLogMsg);
			printf("�����쳣TCPigeonLocationPigeonGatherHandler::DoCommand_PigeonLocInfo: %s\n", (char *)sLogMsg);
			//throw TCException(sLogMsg);
			return;
		}
		//���Ľ���
		PigeonGatherPkg pkg;
		MsgParsing(m_sPkg_Content, pkg);
		//�ж���Ϣ����
		TCString msg_id = pkg.pkg_msg_id;
		if (msg_id == "30") {
			// ������
			std::map<std::string, GatherDevInfo>::iterator iter;
			iter = gather_dev_infos.begin();
			iter = gather_dev_infos.find((char *)pkg.pkg_gather_imei);
			// TODO:����Ҳ���
			if (iter == gather_dev_infos.end()) {
				LoadRpBzGatherDev();
			}
			iter = gather_dev_infos.find((char *)pkg.pkg_gather_imei);
			if (iter != gather_dev_infos.end()) {
				if (pkg.pkg_gps_flag == "1" && !pkg.pkg_latitude.IsEmpty() && !pkg.pkg_longitude.IsEmpty()) {
					// �Ƚϼ���ص�ͱ���֮��ľ������������ֵ�򽫱��ĵ����쳣���Ĳ�����ϵͳ����ʱ��
					double distance_gather_pkg = get_distance(StrToFloat(pkg.pkg_latitude), StrToFloat(pkg.pkg_longitude),
						iter->second.gather_addr_latitude_, iter->second.gather_addr_longitude_);
					if (distance_gather_pkg > distance_threshold) {
						TCString sLogMsg;
						sLogMsg = "";
						sLogMsg += TCString("ERROR: ���볬����ֵ.");
						glgMls->AddPigeonLocationRunLog(sLogMsg);
						printf("distance: %.4f, Error Packet Format: %s\n", distance_gather_pkg,(char*)sLogMsg);

						//TCString resp_content = delay_seconds +"," + TCTime::Now();
						threshold_flag = 1;
						//SendRespPkg(cusSocket, TCString("31"), resp_content);
						//throw TCException(sLogMsg);
					}
				}
			}
			// RP_BR_DEVSTATUS_MGR ��ǼǼ�����
			OutputRpBrDevStatusMgr(pkg, 1);

			TCString resp_content = delay_seconds + "," + TCTime::Now();
			SendRespPkg(cusSocket, TCString("31"), resp_content);
		}	else if (msg_id == "32") {
			//���������ͼ�����Ϣ
			//
			// TODO:
			if (12 > lsTrackerPkgList.GetCount()) {
				return;
			}
			int	rfid_num = StrToInt(lsTrackerPkgList[12]);
			// �����Ÿ�rfid
			std::vector<RacingPigeon> racing_pigeons_info;
			racing_pigeons_info.reserve(rfid_num);
			racing_pigeons_info.clear();
			RacingPigeon racing_pigeon;
			for (int i = 0; i < rfid_num; i ++) {
				racing_pigeon.Clear();
				racing_pigeon.rfid = lsTrackerPkgList[13 + 2 * i];
				racing_pigeon.rfid_chk_2 = lsTrackerPkgList[14 + 2 * i];
				GetRacingPigeonInfosByRfid(racing_pigeon);
				if (racing_pigeon.rfid != "-1") {
					racing_pigeons_info.push_back(racing_pigeon);
				}				
			}
			// ����rfid����������Ϣ
			// ��ѯ�ڴ��е�GATHER_DEV��Ϣ
			std::map<std::string, GatherDevInfo>::iterator iter;
			iter = gather_dev_infos.find((char *)pkg.pkg_gather_imei);
			GatherDevInfo dev_info;
			if (iter == gather_dev_infos.end()) {
				LoadRpBzGatherDev();
			}
			iter = gather_dev_infos.find((char *)pkg.pkg_gather_imei);
			if (iter != gather_dev_infos.end()) {
				dev_info = iter->second;
				double distance_gather_pkg = get_distance(StrToFloat(pkg.pkg_latitude), StrToFloat(pkg.pkg_longitude),
					iter->second.gather_addr_latitude_, iter->second.gather_addr_longitude_);
				if (distance_gather_pkg > distance_threshold) {
					TCString sLogMsg;
					sLogMsg = "";
					sLogMsg += TCString("ERROR: ���볬����ֵ.");
					glgMls->AddPigeonLocationRunLog(sLogMsg);
					printf("distance: %.4f, Error Packet Format: %s\n", distance_gather_pkg, (char*)sLogMsg);

					//TCString resp_content = lsTrackerPkgList[9] + "," + lsTrackerPkgList[12];
					//SendRespPkg(cusSocket, TCString("33"), resp_content);
					threshold_flag = 1;
					//throw TCException(sLogMsg);
				}
			}	else {
				TCString s_log = "GATHER_DEVδ�ҵ���Ӧ��imei:" + pkg.pkg_gather_imei;
				printf("%s", (char *)s_log);
				glgMls->AddPigeonLocationRunLog(s_log);
			}			

			TCString resp_content = lsTrackerPkgList[9] + "," + lsTrackerPkgList[12];
			SendRespPkg(cusSocket, TCString("33"), resp_content);

			// ���RP_BR_GATHER_SUBPROC
			OutputRpBrGatherSubProc(dev_info, racing_pigeons_info, pkg);
			// RP_BR_DEVSTATUS_MGR
			OutputRpBrDevStatusMgr(pkg, 2);
		}	else if (msg_id == "34") {
			// �����������ѯRFID�Ƿ�Ԥ����
			// ������Ҫ��ѯ���Ÿ�rfid
			std::vector<RFIDStatusRespInfo> rfid_status_resp_infos;
			rfid_status_resp_infos.clear();
			// TODO:�����±��ж�
			if (10 > lsTrackerPkgList.GetCount()) {
				return;
			}
			int	rfid_num = StrToInt(lsTrackerPkgList[10]);
			RFIDStatusRespInfo info;
			
			for (int i = 0; i < rfid_num; i ++) {
				info.Clear();
				info.rfid_ = lsTrackerPkgList[11 + i];
				std::string sql_buf = "SELECT 1 FROM RP_BZ_RACINGPIGEON WHERE IRING_RFID='";
				sql_buf += info.rfid_;
				sql_buf += "'";
				otl_stream otl_s(1, sql_buf.c_str(), m_dbConnect);
				if (!otl_s.eof()) {
					rfid_status_resp_infos.push_back(info);
				}
				otl_s.flush();
				otl_s.close();
			}
			TCString owner_id;
			// ����rfid������Ҫ��ѯ��������Ϣ
			if (!GetRFIDStatusRespInfo(rfid_status_resp_infos)) {
				// TODO:��������Ľ��ȫ��Ϊ�գ���ô��Ҫ���ڴ��в�ѯ���ֻ������Ӧ�Ļ���������û�ID��Ϣ
				/*
				��������Ľ��ȫ��Ϊ�գ���ô��Ҫ���ڴ��в�ѯ���ֻ������Ӧ�Ļ���������û�ID��Ϣ
				��ȡ�� OWNERID��OWNERNAME��OWNERNAME_CHN
				*/				
				std::map<std::string, PigeonOwnerInfo>::iterator iter_pigeon_owner =
					pigeon_owner_infos.find((char *)lsTrackerPkgList[9]);
				if (iter_pigeon_owner == pigeon_owner_infos.end()) {
					LoadRpBzPigeonOwner();					
				}
				iter_pigeon_owner =	pigeon_owner_infos.find((char *)lsTrackerPkgList[9]);
				if (iter_pigeon_owner != pigeon_owner_infos.end()) {
						owner_id = (char *)iter_pigeon_owner->second.owner_id_.c_str();
					}
			} else {
				owner_id = (char *)rfid_status_resp_infos[0].ownerid_.c_str();
			}
			/*			
			������ѯ�� RP_BZ_NESTPAD ����ѯ���û�������̤����Ϣ
			������OWNERID= OWNERID
			DORM_ID��DORM_NAME��DORM_ADDRESS��NESTPAD_ID��NESTPAD_INDEX��NESTPAD_IMEI
			*/ 
			std::vector<RpBzGatherNestPadInfo> nest_pad_infos;

			GetRpBzGatherNestPadInfo((char *)owner_id,nest_pad_infos);
			// ƴ�ӱ���			
			std::string res_buf = JointPkgFields(nest_pad_infos, rfid_status_resp_infos);
			TCString resp_content = lsTrackerPkgList[9];
			resp_content  += ",";
			resp_content += (char *)res_buf.c_str();
			//resp_content += "1,999,����1,��ַ1,001,002,867106022056110,1,1,8989";
			SendRespPkg(cusSocket, TCString("35"), resp_content);
			//
			OutputRpBrDevStatusMgr(pkg, 2);
		}	else if (msg_id == "36") {
			// �貹¼��������Ϣ����			
			// ����moble��ȡ������Ϣ
			// �ж������±�
			if (9 > lsTrackerPkgList.GetCount()) {
				return;
			}
			std::string moble = (char *)lsTrackerPkgList[9];
			std::map<std::string, PigeonOwnerInfo>::iterator owner_iter = pigeon_owner_infos.find(moble);
			PigeonOwnerInfo owerinfo;
			if (owner_iter == pigeon_owner_infos.end()) {
				// �Ҳ������¼���
				LoadRpBzPigeonOwner();
			}
			owner_iter = pigeon_owner_infos.find(moble);
			if (owner_iter != pigeon_owner_infos.end()) {
				owerinfo.Clear();
				owerinfo.mobile_ = owner_iter->second.mobile_;
				owerinfo.ownername_chn_ = owner_iter->second.ownername_chn_;
				owerinfo.owner_id_ = owner_iter->second.owner_id_;
				owerinfo.owner_name_ = owner_iter->second.owner_name_;
			}
			// ����pkg.imei��ȡ��������Ϣ
			std::map<std::string, GatherDevInfo>::iterator dev_iter;
			dev_iter = gather_dev_infos.find((char *)pkg.pkg_gather_imei);
			GatherDevInfo devinfo;
			if (dev_iter == gather_dev_infos.end()) {
				// ���¼���
				LoadRpBzGatherDev();
			}
			dev_iter = gather_dev_infos.find((char *)pkg.pkg_gather_imei);
			if (dev_iter != gather_dev_infos.end()) {
				devinfo.gatger_blemac_ = dev_iter->second.gatger_blemac_;
				devinfo.gatger_id_ = dev_iter->second.gatger_id_;
				devinfo.gatger_imei_ = dev_iter->second.gatger_imei_;
				devinfo.gather_addr_latitude_ = dev_iter->second.gather_addr_latitude_;
				devinfo.gather_addr_longitude_ = dev_iter->second.gather_addr_longitude_;
				devinfo.match_id_ = dev_iter->second.match_id_;
				devinfo.match_name_ = dev_iter->second.match_name_;
				// �ж���Ч��
				double distance_gather_pkg = get_distance(StrToFloat(pkg.pkg_latitude), StrToFloat(pkg.pkg_longitude),
					dev_iter->second.gather_addr_latitude_, dev_iter->second.gather_addr_longitude_);
				if (distance_gather_pkg > distance_threshold) {
					TCString sLogMsg;
					sLogMsg = "";
					sLogMsg += TCString("ERROR: ���볬����ֵ.");
					glgMls->AddPigeonLocationRunLog(sLogMsg);
					printf("distance: %.4f, Error Packet Format: %s\n", distance_gather_pkg, (char*)sLogMsg);
					
					//TCString resp_content = lsTrackerPkgList[10];
					//SendRespPkg(cusSocket, TCString("37"), resp_content);
					threshold_flag = 1;
					//throw TCException(sLogMsg);
				}
			}			
			// ���ݱ��Ļ�ȡ��¼�Ÿ����Ϣ
			TCString add_record_num = lsTrackerPkgList[10];
			std::vector<RacingPigeonsDataRespInfo> racing_pigeons_data_resp_info;
			RacingPigeonsDataRespInfo res_info;
			for (int i = 0; i < StrToInt(add_record_num); i++) {
				res_info.Clear();
				res_info.rfid = lsTrackerPkgList[12 + 7 * i];
				res_info.bi_ringid = lsTrackerPkgList[13 + 7 * i];
				res_info.sex = StrToInt(lsTrackerPkgList[14 + 7 * i]);
				res_info.color = StrToInt(lsTrackerPkgList[15 + 7 * i]);
				res_info.sandeye = StrToInt(lsTrackerPkgList[16 + 7 * i]);
				res_info.rfid_chk = lsTrackerPkgList[17 + 7 * i];
				//
				try	{
					std::string sql_buf = "SELECT 1 from RP_BZ_IRING A ";
					sql_buf += " WHERE A.IRING_RFID='";
					sql_buf += res_info.rfid;
					sql_buf += "'";
					otl_stream otl_s(1, sql_buf.c_str(), m_dbConnect);
					if (!otl_s.eof()) {
						racing_pigeons_data_resp_info.push_back(res_info);
					}
					otl_s.flush();
					otl_s.close();
				}	catch (const otl_exception & e) {
					LOG_INFO("SQLִ���쳣\n\tmsg=[%s]\n\tSQL=[%s]\n\tvar_info=[%s]", e.msg, e.stm_text, e.var_info);
				}
			}
			TCString resp_content = add_record_num;
			SendRespPkg(cusSocket, TCString("37"), resp_content);
			// ͨ��rfid ��ѯRP_BZ_IRING��ȡ�Ÿ��IRINGID, IRING_IMEI,IRING_BLEMAC, IRING_MOBILE, IRING_IMSI
			GetRacingPigeonsDataRespInfo(racing_pigeons_data_resp_info);
			// ��¼�� RP_BZ_RACINGPIGEON
			OutputRpBzRacingPigeon(pkg, devinfo, owerinfo, racing_pigeons_data_resp_info);
			// �Ÿ벹¼������Ҫ�� RP_BR_DEVSTATUS_MGR ��ǼǼ�������״̬��Ϣ
			OutputRpBrDevStatusMgr(pkg, 2);
		}
	}
	return;
}

//==========================================================================
// ���� : void TCPigeonLocationPigeonGatherHandler::Init
// ��; : ��ʼ��
// ԭ�� : void Init()
// ���� : ��
// ���� : ��
// ˵�� :
//==========================================================================
void TCPigeonLocationPigeonGatherHandler::Init() {
	m_sPkg_Content = "";
	m_sReq_Command = "";
	m_sRespCommand = "";
	m_sRespPkgContent = "";
	return;
}

void TCPigeonLocationPigeonGatherHandler::OutputRpBrDevStatusMgr(const PigeonGatherPkg & pkg, int const dev_status)
{
	__ENTERFUNCTION;
	//// ����pkg.imei ȥRP_BZ_GATHER_DEV���ѯ������Ϣ
	std::string sql_buf;

	std::string gatger_id;
	otl_stream otl_s;


	// �Ҳ��������¼���һ�ε��ڴ�
	std::map<std::string, GatherDevInfo>::iterator iter = gather_dev_infos.find((char *)pkg.pkg_gather_imei);
	if (iter != gather_dev_infos.end()) {
		gatger_id = iter->second.gatger_id_;
	}	else {
		LoadRpBzGatherDev();
		iter = gather_dev_infos.find((char *)pkg.pkg_gather_imei);
		if (iter != gather_dev_infos.end()) {
			gatger_id = iter->second.gatger_id_;
		}
	}

	std::string tb_name = "RP_BR_DEVSTATUS_MGR";
	sql_buf.clear();
	sql_buf = "INSERT INTO ";
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

	otl_s.open(1, sql_buf.c_str(), m_dbConnect);
	int dev_type = 3;
	otl_s << dev_type;
	otl_s << gatger_id;
	otl_s << (char *)pkg.pkg_gather_imei;
	otl_s << StrToFloat(pkg.pkg_longitude);
	otl_s << StrToFloat(pkg.pkg_latitude);
	otl_s << (int)StrToInt(pkg.pkg_altitude);
	TCString bs_time;
	bs_time.Empty();
	if (dev_status == 1) {
		bs_time = (pkg.pkg_gps_flag == "1") ? pkg.pkg_beijing_time : TCTime::Now();
	}	else if (dev_status == 2) {
		bs_time = (pkg.pkg_beijing_time == "") ? pkg.pkg_gather_time : pkg.pkg_beijing_time;
	}	else {
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

void TCPigeonLocationPigeonGatherHandler::OutputRpBrGatherSubProc(const GatherDevInfo & dev_info,
	const std::vector<RacingPigeon> & racing_pegeons, const PigeonGatherPkg & pkg)
{
	__ENTERFUNCTION;
	std::string tb_name = "RP_BR_GATHER_SUBPROC";
	std::string sql_buf = " INSERT INTO ";
	sql_buf += tb_name;
	sql_buf += "(MATCHID,";
	sql_buf += "MATCHNAME,";
	sql_buf += "OWNERID,";
	sql_buf += "OWNERNAME,";
	sql_buf += "OWNERNAME_CHN,";
	sql_buf += "BI_RINGID,";
	sql_buf += "SEX,";
	sql_buf += "COLOR,";
	sql_buf += "SANDEYE,";
	sql_buf += "IRINGID,";
	sql_buf += "IRING_RFID,";
	sql_buf += "IRING_IMEI,";
	sql_buf += "IRING_BLEMAC,";
	sql_buf += "IRING_MOBILE,";
	sql_buf += "GATHERID,";
	sql_buf += "GATHER_IMEI,";
	sql_buf += "GATHER_BLEMAC,";
	sql_buf += "GATHER_LONGITUDE,";
	sql_buf += "GATHER_LATITUDE,";
	sql_buf += "GATHER_HIGH,";
	sql_buf += "GATHER_TIME,";
	sql_buf += "GATHER_RFID_2STCRC,";
	sql_buf += "VALID_FLAG,";
	sql_buf += "THRESHOLD_FLAG";
	sql_buf += ")";
	sql_buf += "VALUES(";
	sql_buf += ":f1MATCHID<char[16]>,";
	sql_buf += ":f2MATCHNAME<char[256]>,";
	sql_buf += ":f3OWNERID<char[16]>,";
	sql_buf += ":f4OWNERNAME<char[32]>,";
	sql_buf += ":f5OWNERNAME_CHN<char[32]>,";
	sql_buf += ":f6BI_RINGID<char[16]>,";
	sql_buf += ":f7SEX<short>,";
	sql_buf += ":f8COLOR<short>,";
	sql_buf += ":f9SANDEYE<short>,";
	sql_buf += ":f10IRINGID<char[16]>,";
	sql_buf += ":f11IRING_RFID<char[16]>,";
	sql_buf += ":f12IRING_IMEI<char[16]>,";
	sql_buf += ":f13IRING_BLEMAC<char[16]>,";
	sql_buf += ":f14IRING_MOBILE<char[16]>,";
	sql_buf += ":f15GATHERID<char[16]>,";
	sql_buf += ":f16GATHER_IMEI<char[16]>,";
	sql_buf += ":f17GATHER_BLEMAC<char[16]>,";
	sql_buf += ":f18GATHER_LONGITUDE<double>,";
	sql_buf += ":f19GATHER_LATITUDE<double>,";
	sql_buf += ":f20GATHER_HIGH<int>,";
	sql_buf += ":f21GATHER_TIME<timestamp>,";
	sql_buf += ":f22GATHER_RFID_2STCRC<char[16]>,";
	sql_buf += ":f23VALID_FLAG<short>,";
	sql_buf += ":f24THRESHOLD_FLAG<short>";
	sql_buf += ")";

	otl_stream otl_s(100, sql_buf.c_str(), m_dbConnect);

	short valid_flag = 1;
	for (int i = 0; i < racing_pegeons.size(); i++) {
		otl_s << dev_info.match_id_;
		otl_s << dev_info.match_name_;
		otl_s << racing_pegeons[i].owner_id;
		otl_s << racing_pegeons[i].owner_name;
		otl_s << racing_pegeons[i].ownername_chn;
		otl_s << racing_pegeons[i].bi_ringid;
		otl_s << racing_pegeons[i].sex;
		otl_s << racing_pegeons[i].color;
		otl_s << racing_pegeons[i].sandeye;
		otl_s << racing_pegeons[i].iringid;
		otl_s << racing_pegeons[i].rfid;
		otl_s << racing_pegeons[i].iring_imei;
		otl_s << racing_pegeons[i].iring_blemac;
		otl_s << racing_pegeons[i].iring_mobile;
		otl_s << dev_info.gatger_id_;
		otl_s << (char *)pkg.pkg_gather_imei;
		otl_s << dev_info.gatger_blemac_;
		otl_s << StrToFloat(pkg.pkg_longitude);
		otl_s << StrToFloat(pkg.pkg_latitude);
		otl_s << (int)StrToInt(pkg.pkg_altitude);
		TCString bs_time = (pkg.pkg_beijing_time == "") ? pkg.pkg_gather_time : pkg.pkg_beijing_time;
		otl_datetime otl_d;		
		if (String2OTLDateTime(bs_time, otl_d)) {
			otl_s << otl_d;
		}	else {
			otl_s << otl_null();
		}		
		otl_s << racing_pegeons[i].rfid_chk_2;
		
		otl_s << valid_flag;
		otl_s << threshold_flag;
	}
	__LEAVEFUNCTION;
}

void TCPigeonLocationPigeonGatherHandler::OutputRpBzRacingPigeon(const PigeonGatherPkg & pkg,
	const GatherDevInfo & devinfo,
	const PigeonOwnerInfo & owerinfo,
	const std::vector<RacingPigeonsDataRespInfo> & racing_pigeons_data_resp_info)
{
	__ENTERFUNCTION;
	std::string tb_name = "RP_BZ_RACINGPIGEON";
	std::string sql_buf = "INSERT INTO ";
	sql_buf += tb_name;
	sql_buf += "(";
	sql_buf += "OWNERID,";
	sql_buf += "OWNERNAME,";
	sql_buf += "OWNERNAME_CHN,";
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
	sql_buf += "VALUES(";
	sql_buf += ":f01OWNERID<char[16]>,";
	sql_buf += ":f02OWNERNAME<char[36]>,";
	sql_buf += ":f03OWNERNAME_CHN<char[36]>,";
	sql_buf += ":f04BI_RINGID<char[16]>,";
	sql_buf += ":f05SEX<short>,";
	sql_buf += ":f06COLOR<short>,";
	sql_buf += ":f07SANDEYE<short>,";
	sql_buf += ":f08IRINGID<char[16]>,";
	sql_buf += ":f09IRING_RFID<char[16]>,";
	sql_buf += ":f10IRING_IMEI<char[16]>,";
	sql_buf += ":f11IRING_BLEMAC<char[16]>,";
	sql_buf += ":f12IRING_MOBILE<char[16]>,";
	sql_buf += ":f13IRING_IMSI<char[16]>,";
	sql_buf += ":f14FIT_TIME<timestamp>,";
	sql_buf += ":f15FIT_MODEL<short>,";
	sql_buf += ":f16NESTPAD_RFID_INITCRC<char[16]>,";
	sql_buf += ":f17FIT_DEV_TYPE<short>,";
	sql_buf += ":f18FIT_DEV_ID<char[16]>,";
	sql_buf += ":f19FIT_DEV_IMEI<char[16]>,";
	sql_buf += ":f20FIT_DEV_BLEMAC<char[16]>,";
	sql_buf += ":f21FIT_DEV_LONGITUDE<double>,";
	sql_buf += ":f22FIT_DEV_LATITUDE<double>,";
	sql_buf += ":f23FIT_DEV_HIGH<int>,";
	sql_buf += ":f24VALID_FLAG<short>,";
	sql_buf += ":f25THRESHOLD_FLAG<short>";
	sql_buf += ")";

	otl_stream otl_s(10, sql_buf.c_str(), m_dbConnect);
	short valid_flag = 1;
	short fit_dev_type = 2;
	short fit_model = 2;
	for (int i = 0; i < racing_pigeons_data_resp_info.size(); i++) {
		otl_s << owerinfo.owner_id_;
		otl_s << owerinfo.owner_name_;
		otl_s << owerinfo.ownername_chn_;
		otl_s << racing_pigeons_data_resp_info[i].bi_ringid;
		otl_s << racing_pigeons_data_resp_info[i].sex;
		otl_s << racing_pigeons_data_resp_info[i].color;
		otl_s << racing_pigeons_data_resp_info[i].sandeye;
		otl_s << racing_pigeons_data_resp_info[i].iringid;
		otl_s << racing_pigeons_data_resp_info[i].rfid;
		otl_s << racing_pigeons_data_resp_info[i].iring_imei;
		otl_s << racing_pigeons_data_resp_info[i].iring_blemac;
		otl_s << racing_pigeons_data_resp_info[i].iring_mobile;
		otl_s << racing_pigeons_data_resp_info[i].iring_imsi;

		otl_datetime otl_d;
		TCString time_tmp;
		time_tmp = (pkg.pkg_beijing_time == "") ? pkg.pkg_gather_time : pkg.pkg_beijing_time;
		if (String2OTLDateTime(time_tmp, otl_d)) {
			otl_s << otl_d;
		}	else {
			otl_s << otl_null();
		}
		otl_s << fit_model;
		otl_s << racing_pigeons_data_resp_info[i].rfid_chk;
		otl_s << fit_dev_type;
		otl_s << devinfo.gatger_id_;
		otl_s << pkg.pkg_gather_imei;
		otl_s << devinfo.gatger_blemac_;
		otl_s << StrToFloat(pkg.pkg_longitude);
		otl_s << StrToFloat(pkg.pkg_latitude);
		otl_s << (int)StrToInt(pkg.pkg_altitude);
		otl_s << valid_flag;
		otl_s << threshold_flag;
	}
	otl_s.flush();
	otl_s.close();
	__LEAVEFUNCTION;
}
