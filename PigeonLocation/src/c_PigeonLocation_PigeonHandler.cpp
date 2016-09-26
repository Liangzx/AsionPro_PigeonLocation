/*****************************************************************************
CAPTION
Project Name: �鳲ϵͳ
Description : ���ܻ��豸Э�鴦����
File Name   : c_PigeonLocation_PadHandler.cpp
Requires    : LinuxAS3 , Oracle 9i
Create Date : 2016-05-04
******************************************************************************/

#pragma hdrstop
#define OTL_STL
#include "c_PigeonLocation_VehicleHandler.h"
#include "c_PigeonLocation_config.h"
#include "c_PigeonLocation_task.h"
#include <iostream>
#include <ctime>

#define OUT_DEBUG


#pragma package(smart_init)

extern TCPigeonLocationHandler gsPigeonLocationHandler;

extern TCPigeonLocationConfig *gPigeonLocationConfig;


//: ���ݿ�����
extern otl_connect m_dbConnect;


extern TCPigeonLocationLog  *glgMls;

extern map<str_Lac_Cell_Node, str_Space_Pos_Unit> g_msCellInfo_PosUnit;

double long_lac_cal(TCString long_lab) {
	TCStringList tmp;
	if (!long_lab.IsEmpty()) {
		tmp.CommaText(long_lab, '.');
	}
	if (tmp.GetCount() != 2) {
		return 0.0;
	}
	long int_partion = StrToInt(tmp[0]);
	TCString f_tmp = "0." + tmp[1];
	
	double f_partion = StrToFloat(f_tmp);

	double tmp_d = (int_partion + f_partion * 5.0 / 3.0);
	LOG_WRITE("double:%f", tmp_d);

	return tmp_d;
}

//==========================================================================
// ���� : TCPigeonLocationPigeonHandler::TCPigeonLocationPigeonHandler
// ��; : ���캯��
// ԭ�� : TCPigeonLocationPigeonHandler()
// ���� :
// ���� :
// ˵�� :
//==========================================================================
TCPigeonLocationPigeonHandler::TCPigeonLocationPigeonHandler() {
}

//==========================================================================
// ���� : TCPigeonLocationPigeonHandler::~TCPigeonLocationPigeonHandler()
// ��; : ��������
// ԭ�� : ~TCPigeonLocationPigeonHandler()
// ���� :
// ���� :
// ˵�� :
//==========================================================================
TCPigeonLocationPigeonHandler::~TCPigeonLocationPigeonHandler()
{
}

//==========================================================================
// ���� : TCPigeonLocationPigeonHandler::Main_Handler
// ��; : ��Ҫʵ�־����ҵ�񽻻��߼�
// ԭ�� : void Main_Handler()
// ���� :
// ���� :
// ˵�� : ��Ҫʵ�־����ҵ�񽻻��߼�
//==========================================================================
bool TCPigeonLocationPigeonHandler::Main_Handler(TCCustomUniSocket  &cusSocket) {
	try {
		vender = gPigeonLocationConfig->GetTrackerVendor();
		//: Զ�̷��͵ĵ�ַ
		m_sTacker_Send_IPAddress = cusSocket.GetRemoteAddress();
		m_nTimeOut = 60;

		printf("���յ� Tracker ��������IP=[%s], Port=[%d], Timeout=[%d]\n", (char*)cusSocket.GetRemoteAddress(), cusSocket.GetRemotePort(), m_nTimeOut);

		//: �����ӷ�ʽ
		while (1) {			
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
		TCString sLogMsg = TCString("At TCPigeonLocationPigeonHandler::Main_Handler ��������ʱ����:") + e.GetExceptionMessage();
		glgMls->AddPigeonLocationRunLog(sLogMsg);
		cusSocket.Close();
		return false;
	}
	catch (...) {
		glgMls->AddPigeonLocationRunLog("At TCPigeonLocationPigeonHandler::Main_Handler Unknow, �ر�Socket����");
		cusSocket.Close();
		return false;
	}
}

//==========================================================================
// ���� : TCPigeonLocationPigeonHandler::DealRequest
// ��; : ��������
// ԭ�� : void DealRequest()
// ���� : ��
// ���� : ��
// ˵�� :
//==========================================================================
void TCPigeonLocationPigeonHandler::DealRequest(TCCustomUniSocket  &cusSocket) {
	try {
		//: ���������ֽ��д���
#ifdef __TEST__
		printf("============receive Tracker request==============\n");
		printf("Command:=%s, Content=[%s], Time=[%s]\n", (char*)m_sReq_Command, (char*)m_sPkg_Content, (char*)TCTime::Now());
#endif

		//======ִ�з������==========================
		m_sRespCommand = "";
		m_sRespPkgContent = "";
		if (m_sReq_Command == "10") {
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
		}
		catch (TCException &e1) {
			TCString sLogMsg = TCString("ϵͳ����: when Catch a Error and DealRequest: ") + e1.GetExceptionMessage();
			glgMls->AddPigeonLocationRunLog(sLogMsg);
		}
		catch (...) {
			glgMls->AddPigeonLocationRunLog("ϵͳ����δ֪�쳣");
		}
	}
}

void TCPigeonLocationPigeonHandler::GetDelay(const PigeonPkg pkg, TimeDelay & time_delay)
{
	__ENTERFUNCTION;
	{
		std::string sql_buf = "SELECT CHANGE_TIME_DELAY,FLY_TIME_DELAY,NEST_TIME_DELAY FROM RP_VD_IRING A ";
		sql_buf += " WHERE A.IMEI='";
		sql_buf += (char *)pkg.pkg_imei;
		sql_buf += "'";
		otl_stream otl_s(100, sql_buf.c_str(), m_dbConnect);
		otl_s >> time_delay.change_time_delay_;
		if (!otl_s.eof()) {

			otl_s >> time_delay.fly_time_delay_;
			otl_s >> time_delay.nest_time_delay_;
		}
		otl_s.flush();
		otl_s.close();
	}
	__LEAVEFUNCTION;
}

void TCPigeonLocationPigeonHandler::GetNestPadLoc(const PigeonPkg & pkg, double & latitude
	,double & longitude)
{
	/*
	�������ݲ�ѯ���������ܻ���ѯ RP_BZ_RACINGPIGEON ��IRING_IMEI=Pkg.IMEI 
	��ȡ���Ÿ�� ����λ����Ϣ����NESTPAD_LONGITUDE��NESTPAD_LATITUDE��
	*/
	__ENTERFUNCTION;
	std::string sql_buf = "select NESTPAD_LATITUDE, NESTPAD_LONGITUDE FROM RP_BZ_RACINGPIGEON A ";
	sql_buf += " WHERE A.IRING_IMEI='";
	sql_buf += pkg.pkg_imei;
	sql_buf += "'";
	otl_stream otl_s(10, sql_buf.c_str(), m_dbConnect);	
	if (!otl_s.eof()) {	
		otl_s >> latitude;
		otl_s >> longitude;
	}
	otl_s.flush();
	otl_s.close();
	__LEAVEFUNCTION;
}

bool TCPigeonLocationPigeonHandler::CheckRpBrgatherSubproc(const PigeonPkg & pkg,
	RpBrgatherSubprocInfo & info)
{
	__ENTERFUNCTION;
	std::string sql_buf = "SELECT MATCHID,MATCHNAME,BI_RINGID,SEX,COLOR,SANDEYE,";
	sql_buf += "IRING_RFID,IRINGID,IRING_BLEMAC,IRING_MOBILE FROM RP_BR_GATHER_SUBPROC A WHERE ";
	sql_buf += "A.IRING_IMEI='";
	sql_buf += pkg.pkg_imei;
	sql_buf += "'";
	sql_buf += " AND VALID_FLAG=1";
	
	otl_stream otl_s;
	otl_s.open(1, sql_buf.c_str(), m_dbConnect);
	otl_s >> info.matchid_;
	if (otl_s.eof()) {
		return false;
	}	else {
		info.imei_ = pkg.pkg_imei;
		otl_s >> info.matchid_;
		otl_s >> info.matchname_;
		otl_s >> info.bi_ringid_;
		otl_s >> info.sex_;
		otl_s >> info.color_;
		otl_s >> info.sandeye_;
		otl_s >> info.iring_rfid_;
		otl_s >> info.iringid_;
		otl_s >> info.iring_blemac_;
		otl_s >> info.iring_mobile_;
		otl_s.flush();
		otl_s.close();
		return true;
	}
	__LEAVEFUNCTION;
	return false;
}

void TCPigeonLocationPigeonHandler::GetMatchInfo(const std::string match_id, RpBzMatchInfo & info)
{
	__ENTERFUNCTION;
	std::string sql_buf = "SELECT GATHER_TIME,ALLOCSTART_TIME,MATCHSTART_TIME";
	sql_buf += " FROM RP_BZ_MATCH A WHERE A.MATCHID='";
	sql_buf += match_id;
	sql_buf += "'";
	otl_datetime otl_d;
	TCString tmp_time;
	otl_stream otl_s(10, sql_buf.c_str(), m_dbConnect);
	if (!otl_s.eof()) {
		otl_s >> otl_d;		
		OTLDateTime2String(otl_d, tmp_time);
		info.gather_time_ = tmp_time;

		otl_s >> otl_d;
		OTLDateTime2String(otl_d, tmp_time);
		info.allocstart_time_ = tmp_time;

		otl_s >> otl_d;
		OTLDateTime2String(otl_d, tmp_time);
		info.matchstart_time_ = tmp_time;

	}
	otl_s.flush();
	otl_s.close();
	__LEAVEFUNCTION;
}

void TCPigeonLocationPigeonHandler::MsgParsing(const TCString & content, PigeonPkg & pkg,
	std::vector<LacCiRxLev> & lac_ci_rxlev)
{
	// ��������
	__ENTERFUNCTION;
	TCStringList lsTrackerPkgList;
	lsTrackerPkgList.Clear();
	lsTrackerPkgList.CommaText(content, m_cDelimter);
	// ���Ľ���
	pkg.pkg_pkg_offset = lsTrackerPkgList[0];		//���ݰ�ʱ��ƫ��
	pkg.pkg_imei = lsTrackerPkgList[1];				//�豸ʶ����
	pkg.pkg_gps_flag = lsTrackerPkgList[2];			//GPS��λ״̬ 0/1

	if (lsTrackerPkgList[3].IsEmpty()) {
		pkg.pkg_beijing_time = TCTime::Now();
	}
	else
	{
		TCString gps_time_tmp = "000000";		//GPSʱ��Hhmmss.MMM
		if (lsTrackerPkgList[3].GetLength() == 10) {
			gps_time_tmp = lsTrackerPkgList[3].Left(6);
		}
		// yyyymmddhhmiss ת����utc
		TCString utc_now_day = TCTime::RelativeTime(TCTime::Now(), -3600 * 8).Left(8);
		TCString gps_time = utc_now_day + gps_time_tmp;
		pkg.pkg_beijing_time = TCTime::RelativeTime(gps_time, 8 * 3600);
#ifdef OUT_DEBUG
			printf("utc_now_day:%s,gps_time:%s\n", (char *)utc_now_day, (char *)gps_time);
#endif // OUT_DEBUG
	}
	//TCStringList tmp;
	//if (!lsTrackerPkgList[4].IsEmpty()) {
	//	tmp.CommaText(lsTrackerPkgList[4], '.');
	//}
	//if (tmp.GetCount() != 2) {
	//	return;
	//}
	//long int_partion = StrToInt(tmp[0]);
	//float f_partion = StrToFloat(tmp[1]);
	//double tmp_d = int_partion + f_partion * 5.0 / 3.0;

	double tmp_d = long_lac_cal(lsTrackerPkgList[4]);
	pkg.pkg_latitude = FloatToStr(tmp_d, 6);		//γ��

	tmp_d = long_lac_cal(lsTrackerPkgList[5]);
	pkg.pkg_longitude = FloatToStr(tmp_d, 6);		//����

	pkg.pkg_altitude = lsTrackerPkgList[6];		//���θ߶ȣ�����Ϊ��λ
	pkg.pkg_voltage = lsTrackerPkgList[7];			//��ص�ѹ
																								// ����С����Ϣ
	int filed_num = lsTrackerPkgList.GetCount();
	// ĩβ����,TA
	// С����Ϣ������
	int lac_cis = (filed_num - 8 - 1) / 3;
	LacCiRxLev lev;
	for (size_t i = 0; i < lac_cis; i++) {
		lev.Clear();
		lev.lac_ = lsTrackerPkgList[8 + 3 * i];
		lev.ci_ = lsTrackerPkgList[9 + 3 * i];
		lev.rx_lev_ = lsTrackerPkgList[10 + 3 * i];
		lac_ci_rxlev.push_back(lev);
	}
	//----2016-09-23--��ʱ���----
	std::string sql_buf = "insert into mb_bss_terminal_location(day, imei, longitude, latitude, ACTIVE_TIME, LOCATION_TYPE, ELECTRICITY)";
	sql_buf += "values(:f0day<timestamp>, :f1imei<char[17]>, :f2longitude<char[31]>, :f3latitude<char[31]>,sysdate, '0', :f4electricity<char[7]>)";
	TCString day = TCTime::Today();
	if (day.GetLength() == 8) {
		day += "000000";
	}
	otl_datetime dt_time;
	String2OTLDateTime(day, dt_time);
	otl_stream ot_s(1, sql_buf.c_str(), m_dbConnect);
	if (!(pkg.pkg_imei.IsEmpty() || pkg.pkg_longitude.IsEmpty() || pkg.pkg_latitude.IsEmpty())) {
		ot_s << dt_time;
		ot_s << (char *)pkg.pkg_imei;
		ot_s << (char *)pkg.pkg_longitude;
		ot_s << (char *)pkg.pkg_latitude;
		ot_s << (char *)pkg.pkg_voltage;
		ot_s.close();
	}
	LOG_WRITE("longitude:%s,latitude:%s", (char *)pkg.pkg_longitude, (char *)pkg.pkg_latitude);
	//----
	__LEAVEFUNCTION;	
}

void TCPigeonLocationPigeonHandler::OutputRpBrRealLocation(PigeonPkg & pkg,
	RpBrgatherSubprocInfo & info, std::vector<LacCiRxLev>& vec_lac_ci, const long delay_sec)
{
	__ENTERFUNCTION;
	std::string tb_name = "RP_BR_REALLOCATION";
	std::string sql_buf = "INSERT INTO ";
	sql_buf += tb_name;
	sql_buf += "(";
	sql_buf += "MATCHID,";
	sql_buf += "MATCHNAME,";
	sql_buf += "BI_RINGID,";
	sql_buf += "SEX,";
	sql_buf += "COLOR,";
	sql_buf += "SANDEYE,";
	sql_buf += "IRINGID,";
	sql_buf += "IRING_RFID,";
	sql_buf += "IRING_IMEI,";
	sql_buf += "IRING_BLEMAC,";
	sql_buf += "IRING_MOBILE,";
	sql_buf += "RECV_TIME,";
	sql_buf += "RECORD_TIME,";
	sql_buf += "LOC_TYPE,";
	sql_buf += "GPS_TIME,";
	sql_buf += "GPS_LONGITUDE,";
	sql_buf += "GPS_LATITUDE,";
	sql_buf += "GPS_HIGH,";
	sql_buf += "LAC1,";
	sql_buf += "CELLID1,";
	sql_buf += "RSCP1,";
	sql_buf += "LAC2,";
	sql_buf += "CELLID2,";
	sql_buf += "RSCP2,";
	sql_buf += "LAC3,";
	sql_buf += "CELLID3,";
	sql_buf += "RSCP3,";
	sql_buf += "LAC4,";
	sql_buf += "CELLID4,";
	sql_buf += "RSCP4,";
	sql_buf += "LAC5,";
	sql_buf += "CELLID5,";
	sql_buf += "RSCP5,";
	sql_buf += "LAC6,";
	sql_buf += "CELLID6,";
	sql_buf += "RSCP6,";
	sql_buf += "RESP_NEXTSND_DELAY,";
	sql_buf += "BLE_CRC";
	sql_buf += ")";
	sql_buf += "VALUES";
	sql_buf += "(";
	sql_buf += ":f01MATCHID<char[256]>,";
	sql_buf += ":f02MATCHNAME<char[256]>,";
	sql_buf += ":f03BI_RINGID<char[16]>,";
	sql_buf += ":f04SEX<short>,";
	sql_buf += ":f05COLOR<short>,";
	sql_buf += ":f06SANDEYE<short>,";
	sql_buf += ":f07IRINGID<char[16]>,";
	sql_buf += ":f08IRING_RFID<char[16]>,";
	sql_buf += ":f09IRING_IMEI<char[16]>,";
	sql_buf += ":f10IRING_BLEMAC<char[16]>,";
	sql_buf += ":f11IRING_MOBILE<char[16]>,";
	sql_buf += ":f12RECV_TIME<timestamp>,";
	sql_buf += ":f13RECORD_TIME<timestamp>,";
	sql_buf += ":f14LOC_TYPE<short>,";
	sql_buf += ":f15GPS_TIME<timestamp>,";
	sql_buf += ":f16GPS_LONGITUDE<double>,";
	sql_buf += ":f17GPS_LATITUDE<double>,";
	sql_buf += ":f18GPS_HIGH<int>,";
	sql_buf += ":f19LAC1<int>,";
	sql_buf += ":f20CELLID1<long>,";
	sql_buf += ":f21RSCP1<int>,";
	sql_buf += ":f22LAC2<int>,";
	sql_buf += ":f23CELLID2<long>,";
	sql_buf += ":f24RSCP2<int>,";
	sql_buf += ":f25LAC3<int>,";
	sql_buf += ":f26CELLID3<long>,";
	sql_buf += ":f27RSCP3<int>,";
	sql_buf += ":f28LAC4<int>,";
	sql_buf += ":f29CELLID4<long>,";
	sql_buf += ":f30RSCP4<int>,";
	sql_buf += ":f31LAC5<int>,";
	sql_buf += ":f32CELLID5<long>,";
	sql_buf += ":f33RSCP5<int>,";
	sql_buf += ":f34LAC6<int>,";
	sql_buf += ":f35CELLID6<long>,";
	sql_buf += ":f36RSCP6<int>,";
	sql_buf += ":f37RESP_NEXTSND_DELAY<long>,";
	sql_buf += ":f38BLE_CRC<int>";
	sql_buf += ")";
	// ���С�����Ƿ����6
	int num = vec_lac_ci.size();
	int diff = 6 - num;
	if (diff > 0) {
		LacCiRxLev lv;
		for (int i = 0; i < diff; i ++) {
			vec_lac_ci.push_back(lv);
		}
	}
	//
	otl_stream otl_s(10, sql_buf.c_str(), m_dbConnect);
	otl_s << info.matchid_;
	otl_s << info.matchname_;
	otl_s << info.bi_ringid_;
	otl_s << info.sex_;
	otl_s << info.color_;
	otl_s << info.sandeye_;
	otl_s << info.iringid_;
	otl_s << info.iring_rfid_;
	otl_s << info.imei_;
	otl_s << info.iring_blemac_;
	otl_s << info.iring_mobile_;
	TCString ts_time;
	ts_time = TCTime::Now();
	otl_datetime otl_dt;
	//Socket�������ʱ��
	if (String2OTLDateTime(m_sRecvTime, otl_dt)) {
		otl_s << otl_dt;
	}	else {
		otl_s << otl_null();
	}
	//��⴦��ʱ��
	if (String2OTLDateTime(ts_time, otl_dt)) {
		otl_s << otl_dt;
	}
	else {
		otl_s << otl_null();
	}
	short loc_type = (pkg.pkg_longitude.IsEmpty() || pkg.pkg_latitude.IsEmpty()) ? 0 : 1;
	otl_s << loc_type;
	// ����GPSʱ�䣬��Ҫ��8Сʱ
	ts_time = pkg.pkg_beijing_time;
	if (String2OTLDateTime(ts_time, otl_dt)) {
		otl_s << otl_dt;
	}
	else {
		otl_s << otl_null();
	}
	//
	otl_s << StrToFloat(pkg.pkg_longitude);
	otl_s << StrToFloat(pkg.pkg_latitude);
	otl_s << (int)StrToInt(pkg.pkg_altitude);
	//
	otl_s << (int)std::strtol(vec_lac_ci[0].lac_, 0, 16);
	otl_s << std::strtol(vec_lac_ci[0].ci_, 0, 16);
	otl_s << (int)StrToInt(vec_lac_ci[0].rx_lev_);
	otl_s << (int)std::strtol(vec_lac_ci[1].lac_, 0, 16);
	otl_s << std::strtol(vec_lac_ci[1].ci_, 0, 16);
	otl_s << (int)StrToInt(vec_lac_ci[1].rx_lev_);
	otl_s << (int)std::strtol(vec_lac_ci[2].lac_, 0, 16);
	otl_s << std::strtol(vec_lac_ci[2].ci_, 0, 16);
	otl_s << (int)StrToInt(vec_lac_ci[2].rx_lev_);
	otl_s << (int)std::strtol(vec_lac_ci[3].lac_, 0, 16);
	otl_s << std::strtol(vec_lac_ci[3].ci_, 0, 16);
	otl_s << (int)StrToInt(vec_lac_ci[3].rx_lev_);
	otl_s << (int)std::strtol(vec_lac_ci[4].lac_, 0, 16);
	otl_s << std::strtol(vec_lac_ci[4].ci_, 0, 16);
	otl_s << (int)StrToInt(vec_lac_ci[4].rx_lev_);
	otl_s << (int)std::strtol(vec_lac_ci[5].lac_, 0, 16);
	otl_s << std::strtol(vec_lac_ci[5].ci_, 0, 16);
	otl_s << (int)StrToInt(vec_lac_ci[5].rx_lev_);
	//
	otl_s << delay_sec;
	int ble_crc = -1;
	otl_s << ble_crc;
	__LEAVEFUNCTION;
}

void TCPigeonLocationPigeonHandler::OutputRpBrDevStatusMgr(PigeonPkg & pkg)
{
	__ENTERFUNCTION;
	// ����pkg.imei ȥRP_BZ_CARMON_DEV���ѯ������Ϣ
	std::string sql_buf = "SELECT  IRINGID FROM RP_BR_GATHER_SUBPROC A ";
	sql_buf += "WHERE A.IRING_IMEI='";
	sql_buf += (char *)pkg.pkg_imei;
	sql_buf += "'";
	//
	std::string iring_id;
	otl_stream otl_s(10, sql_buf.c_str(), m_dbConnect);
	otl_s >> iring_id;
	otl_s.flush();
	otl_s.close();
	//���RP_BR_DEVSTATUS_MGR
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
	int dev_type = 2;
	otl_s << dev_type;
	otl_s << iring_id;
	otl_s << (char *)pkg.pkg_imei;
	otl_s << StrToFloat(pkg.pkg_longitude);
	otl_s << StrToFloat(pkg.pkg_latitude);
	otl_s << (int)StrToInt(pkg.pkg_altitude);
	TCString tmp_time = (pkg.pkg_gps_flag == "1") ? pkg.pkg_beijing_time : TCTime::Now();
	otl_datetime otl_date;
	if (String2OTLDateTime(tmp_time, otl_date)) {
		otl_s << otl_date;
	}
	else {
		otl_s << otl_null();
	}
	int status_type = 2;
	otl_s << status_type;
	otl_s << threshold_flag;
	otl_s.flush();
	otl_s.close();	
	__LEAVEFUNCTION;
}

inline double TCPigeonLocationPigeonHandler::radian(double d)
{
	return d * PI / 180.0;   //�Ƕ�1? = �� / 180
}

inline double TCPigeonLocationPigeonHandler::get_distance(double lat1, double lng1, double lat2, double lng2)
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
// ���� : TCPigeonLocationPigeonHandler::RecvRequest(TCCustomUniSocket  &cusSocket)
// ��; :	pad�����ݴ���ʽ����������, ��������������
// ԭ�� : void RecvRequest()
// ���� : ��
// ���� : ��
// ˵�� :
//==========================================================================
void TCPigeonLocationPigeonHandler::RecvRequest(TCCustomUniSocket  &cusSocket) {
	try {
		const int nMaxPkg_Length = 1024 * 1000;
		char sbuff[nMaxPkg_Length];
		memset(sbuff, 0, sizeof(sbuff));
		m_vsPkgList.clear();
		m_sReq_Command = "";
		m_sPkg_Content = "";

		int nTimeOut = 3600 * 24 * 1000;

		//: ��ʼ���� select �������ж�
		if (!cusSocket.WaitForData(nTimeOut)) {
			//: ��ʱ120���ˣ�����ֱ�ӹر����ӣ�
			TCString sLog = "PigeonHandle Tracker[" + m_sTacker_Send_IPAddress + "]�Ѿ���ʱ120��δ�������ݣ��Ͽ�����";
			printf("%s\n", (char*)sLog);
			glgMls->AddPigeonLocationRunLog(sLog);
			throw TCException(sLog);
		}

		int nGetPkgLen = 0;
		nGetPkgLen = cusSocket.ReceiveBuf(sbuff, nMaxPkg_Length);
		m_sRecvTime = TCTime::Now();
		if (nGetPkgLen == 0) {
			TCString sLog = "PigeonHandle Tracker[" + m_sTacker_Send_IPAddress + "]�Ѿ��Ͽ�����";
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
			return;
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
		throw TCException("TCPigeonLocationPigeonHandler::RecvRequest() Error:"
			"Unknown Exception.");
	}
}

//==========================================================================
// ���� : TCPigeonLocationPigeonHandler::SendRespPkg
// ��; : ����Ӧ���
// ԭ�� : void SendRespPkg(TCString sRespCommand, TCString sRespContent)
// ���� : sRespCommand---�����������   sRespContent ---- ��������(ǰ�����ֽھ���������)
// ���� : ��
// ˵�� :
//==========================================================================
void TCPigeonLocationPigeonHandler::SendRespPkg(TCCustomUniSocket  &cusSocket, int interval) {
	TCString sTmpRespContent = "AT";
	sTmpRespContent += IntToStr(interval);
	sTmpRespContent += ",TA";
	cusSocket.SendBuf((char *)sTmpRespContent, Length(sTmpRespContent));
	m_sSendTime = TCTime::Now();

#ifdef __TEST__
	printf("RecvTime=[%s], SendTime=[%s], sTmpRespContent=[%s]\n", (char*)m_sRecvTime, (char*)m_sSendTime, (char*)sTmpRespContent);
#endif

}

//==========================================================================
// ���� : void TCPigeonLocationPigeonHandler::DoCommand_PigeonLocInfo
// ��; : ƽ���ϱ�λ����Ϣ
// ԭ�� : void DoCommand_PigeonLocInfo()
// ���� : ��
// ���� : ��
// ˵�� :
//==========================================================================
void TCPigeonLocationPigeonHandler::DoCommand_LocInfo(TCCustomUniSocket  &cusSocket) {
	field_counts = 9;
	for (int nPkgSeq = 0; nPkgSeq<m_vsPkgList.size(); nPkgSeq++) {
		std::srand(time(0));
		m_sPkg_Content = m_vsPkgList[nPkgSeq];
		//: �ֽ��û�����
		TCStringList lsTrackerPkgList;
		lsTrackerPkgList.Clear();
		lsTrackerPkgList.CommaText(m_sPkg_Content, m_cDelimter);

		if (lsTrackerPkgList.GetCount() < field_counts) {
			//: ����15��������
			TCString sLogMsg;
			sLogMsg = "";
			sLogMsg += TCString("����������:(") + m_sReq_Command;
			sLogMsg += TCString(") �����쳣��������=[") + IntToStr(lsTrackerPkgList.GetCount()) + "]";
			glgMls->AddPigeonLocationRunLog(sLogMsg);
			printf("�����쳣TCPigeonLocationPigeonHandler::DoCommand_PigeonLocInfo: %s\n", (char *)sLogMsg);
			throw TCException(sLogMsg);
		}
		// ���Ľ���
		// С����
		std::vector<LacCiRxLev> lac_ci_rxlv;
		lac_ci_rxlv.clear();
		PigeonPkg pkg;
		pkg.Clear();
		// ��������
		MsgParsing(m_sPkg_Content, pkg, lac_ci_rxlv);

		std::cout << "PKG:" << (char *)m_sPkg_Content << std::endl;
	
		//TODO:����IMEI��ѯ�� RP_VD_IRING ��ȡ ��ʱʱ��
		//1.CHANGE_TIME_DELAY ������ʱ��ʱ����
		//2.FLY_TIME_DELAY	����״��ʱ����
		//3.NEST_TIME_DELAY	�鳲���ʱ����
		TimeDelay delay;
		delay.Clear();
		GetDelay(pkg, delay);
#ifdef OUT_DEBUG
		std::cout << "change_time_delay,fly_time_delay,nest_time_delay"
			<< delay.change_time_delay_ << "," << delay.fly_time_delay_ << "," << delay.nest_time_delay_
			<< std::endl;
#endif // OUT_DEBUG


		/*
		//�������ݲ�ѯ���������ܻ���ѯ RP_BZ_RACINGPIGEON ��IRING_IMEI=Pkg.IMEI 
		//��ȡ���Ÿ�� ����λ����Ϣ����NESTPAD_LONGITUDE��NESTPAD_LATITUDE��
		//*/
		//double nestpad_longitude = 0.0;
		//double nestpad_latitude = 0.0;
		//GetNestPadLoc(pkg, nestpad_latitude, nestpad_longitude);
		//TODO:״̬ȷ������ѯ RP_BR_GATHER_SUBPROC �������� VALID_FLAG=1 and IRING_IMEI=Pkg.IMEI;
		// 1)���û�м�¼�򷵻ر����С������ϱ��ӳ١��ֶε�ֵ��Ϊ ��һ��00��00��00���ȥGPSʱ��
		RpBrgatherSubprocInfo gather_subproc_info;
		gather_subproc_info.Clear();
		int interval_recv = 60;
		bool have_record = false;
		if (CheckRpBrgatherSubproc(pkg, gather_subproc_info)) {
			// �м�¼
			have_record = true;
			// ��ѯ RP_BZ_MATCH ����ȡ��ǰ������ GATHER_TIME �� ALLOCSTART_TIME(�ƻ�ʩ��ʱ��)
			// �Լ� MATCHSTART_TIME(ʵ��ʩ��ʱ��)
			RpBzMatchInfo match_Info;
			std::string end_time;
			std::string match_id = gather_subproc_info.matchid_;
			GetMatchInfo(match_id, match_Info);
			if (match_Info.matchstart_time_.empty()) {
				end_time = match_Info.allocstart_time_;
			}	else {
				end_time = match_Info.allocstart_time_ < match_Info.matchstart_time_
					? match_Info.allocstart_time_ : match_Info.matchstart_time_;
			}
			//
			if (pkg.pkg_beijing_time > (char *)match_Info.gather_time_.c_str()
				&& pkg.pkg_beijing_time <= (char *)end_time.c_str()) {
				time_t t1 = TCTime::GetTimeTByDatetimeString((char *)end_time.c_str());
				time_t t2 = TCTime::GetTimeTByDatetimeString(pkg.pkg_beijing_time);

				double time_dif = difftime(t1, t2);
				interval_recv = time_dif + 2 * 120;
			}	else if (pkg.pkg_beijing_time > (char *)end_time.c_str()) {
				if (match_Info.matchstart_time_.empty()) {
					interval_recv = delay.change_time_delay_;
				}	else {
					//��ȡ���Ÿ�� ����λ����Ϣ����NESTPAD_LONGITUDE��NESTPAD_LATITUDE��
					//*/
					double nestpad_longitude = 0.0;
					double nestpad_latitude = 0.0;
					GetNestPadLoc(pkg, nestpad_latitude, nestpad_longitude);
					double distance_nest = get_distance(nestpad_latitude, nestpad_longitude,
						StrToFloat(pkg.pkg_latitude), StrToFloat(pkg.pkg_longitude));
					if (pkg.pkg_longitude.IsEmpty() || pkg.pkg_latitude.IsEmpty() || distance_nest > 30.0) {
						interval_recv = delay.fly_time_delay_;
#ifdef OUT_DEBUG
						std::cout << "distance_nest::fly_time_delay_(30.0,--]" <<
							distance_nest << ":" << std::endl;
#endif // OUT_DEBUG

					}	else if (distance_nest > 0 && distance_nest <= 30.0) {
						if (distance_nest < 10/1000) {
							interval_recv = 24 * 3600;
						}	else {
							interval_recv = delay.nest_time_delay_;
#ifdef OUT_DEBUG
							std::cout << "distance_nest::nest_time_delay_(--,30.0]" <<
								distance_nest << ":" << std::endl;
#endif // OUT_DEBUG
						}						
					}	
				}
			}
		}	else {
			// �޼�¼
			TCString utc_time = TCTime::RelativeTime(pkg.pkg_beijing_time, -8 * 3600);
			interval_recv = (24 * 3600) - (TCTime::Hour(utc_time) * 3600
				+ TCTime::Minute(utc_time) * 60	+ TCTime::Second(utc_time));
#ifdef OUT_DEBUG
			std::string out_s = "utc-time:";
			out_s += (char *)utc_time;
			out_s += ",beijing_time:";
			out_s += (char *)pkg.pkg_beijing_time;
			out_s += ",interval:";
			out_s += (char *)IntToStr(interval_recv);
			std::cout << out_s << std::endl;

#endif // OUT_DEBUG
		}
		int random_num = std::rand() % 60;				//�����ϱ��ӳ������
		SendRespPkg(cusSocket, interval_recv + random_num);
		//
		if (have_record) {
			// ���RP_BR_REALLOCATION
			OutputRpBrRealLocation(pkg, gather_subproc_info, lac_ci_rxlv, interval_recv + random_num);
		}
		// ���RP_BR_DEVSTATUS_MGR
		OutputRpBrDevStatusMgr(pkg);
	}
	return;
}


//==========================================================================
// ���� : void TCPigeonLocationPigeonHandler::Init
// ��; : ��ʼ��
// ԭ�� : void Init()
// ���� : ��
// ���� : ��
// ˵�� :
//==========================================================================
void TCPigeonLocationPigeonHandler::Init() {
	m_sPkg_Content = "";
	m_sReq_Command = "";
	m_sRespCommand = "";
	m_sRespPkgContent = "";
	return;
}
