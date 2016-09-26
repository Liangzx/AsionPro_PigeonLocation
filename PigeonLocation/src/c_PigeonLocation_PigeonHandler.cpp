/*****************************************************************************
CAPTION
Project Name: 归巢系统
Description : 智能环设备协议处理类
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


//: 数据库连接
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
// 函数 : TCPigeonLocationPigeonHandler::TCPigeonLocationPigeonHandler
// 用途 : 构造函数
// 原型 : TCPigeonLocationPigeonHandler()
// 参数 :
// 返回 :
// 说明 :
//==========================================================================
TCPigeonLocationPigeonHandler::TCPigeonLocationPigeonHandler() {
}

//==========================================================================
// 函数 : TCPigeonLocationPigeonHandler::~TCPigeonLocationPigeonHandler()
// 用途 : 析构函数
// 原型 : ~TCPigeonLocationPigeonHandler()
// 参数 :
// 返回 :
// 说明 :
//==========================================================================
TCPigeonLocationPigeonHandler::~TCPigeonLocationPigeonHandler()
{
}

//==========================================================================
// 函数 : TCPigeonLocationPigeonHandler::Main_Handler
// 用途 : 主要实现具体的业务交互逻辑
// 原型 : void Main_Handler()
// 参数 :
// 返回 :
// 说明 : 主要实现具体的业务交互逻辑
//==========================================================================
bool TCPigeonLocationPigeonHandler::Main_Handler(TCCustomUniSocket  &cusSocket) {
	try {
		vender = gPigeonLocationConfig->GetTrackerVendor();
		//: 远程发送的地址
		m_sTacker_Send_IPAddress = cusSocket.GetRemoteAddress();
		m_nTimeOut = 60;

		printf("接收到 Tracker 数据请求：IP=[%s], Port=[%d], Timeout=[%d]\n", (char*)cusSocket.GetRemoteAddress(), cusSocket.GetRemotePort(), m_nTimeOut);

		//: 长连接方式
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
		TCString sLogMsg = TCString("At TCPigeonLocationPigeonHandler::Main_Handler 处理请求时出错:") + e.GetExceptionMessage();
		glgMls->AddPigeonLocationRunLog(sLogMsg);
		cusSocket.Close();
		return false;
	}
	catch (...) {
		glgMls->AddPigeonLocationRunLog("At TCPigeonLocationPigeonHandler::Main_Handler Unknow, 关闭Socket连接");
		cusSocket.Close();
		return false;
	}
}

//==========================================================================
// 函数 : TCPigeonLocationPigeonHandler::DealRequest
// 用途 : 处理请求
// 原型 : void DealRequest()
// 参数 : 无
// 返回 : 无
// 说明 :
//==========================================================================
void TCPigeonLocationPigeonHandler::DealRequest(TCCustomUniSocket  &cusSocket) {
	try {
		//: 按照命令字进行处理；
#ifdef __TEST__
		printf("============receive Tracker request==============\n");
		printf("Command:=%s, Content=[%s], Time=[%s]\n", (char*)m_sReq_Command, (char*)m_sPkg_Content, (char*)TCTime::Now());
#endif

		//======执行服务调用==========================
		m_sRespCommand = "";
		m_sRespPkgContent = "";
		if (m_sReq_Command == "10") {
			DoCommand_LocInfo(cusSocket);
		}
		else {
			//: 未知命令
			TCString sLogMsg = TCString("未知命令,m_sReq_Command: [") + m_sReq_Command + "]";
			glgMls->AddPigeonLocationRunLog(sLogMsg);
			return;
		}
	}
	catch (TCException &e) {
		try {
			TCString sLogMsg = TCString("系统错误,At DealRequest: ") + e.GetExceptionMessage();
			glgMls->AddPigeonLocationRunLog(sLogMsg);
		}
		catch (TCException &e1) {
			TCString sLogMsg = TCString("系统错误: when Catch a Error and DealRequest: ") + e1.GetExceptionMessage();
			glgMls->AddPigeonLocationRunLog(sLogMsg);
		}
		catch (...) {
			glgMls->AddPigeonLocationRunLog("系统发生未知异常");
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
	基础数据查询，根据智能环查询 RP_BZ_RACINGPIGEON 表；IRING_IMEI=Pkg.IMEI 
	获取该信鸽的 鸽舍位置信息：（NESTPAD_LONGITUDE，NESTPAD_LATITUDE）
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
	// 解析数据
	__ENTERFUNCTION;
	TCStringList lsTrackerPkgList;
	lsTrackerPkgList.Clear();
	lsTrackerPkgList.CommaText(content, m_cDelimter);
	// 报文解析
	pkg.pkg_pkg_offset = lsTrackerPkgList[0];		//数据包时间偏移
	pkg.pkg_imei = lsTrackerPkgList[1];				//设备识别码
	pkg.pkg_gps_flag = lsTrackerPkgList[2];			//GPS定位状态 0/1

	if (lsTrackerPkgList[3].IsEmpty()) {
		pkg.pkg_beijing_time = TCTime::Now();
	}
	else
	{
		TCString gps_time_tmp = "000000";		//GPS时间Hhmmss.MMM
		if (lsTrackerPkgList[3].GetLength() == 10) {
			gps_time_tmp = lsTrackerPkgList[3].Left(6);
		}
		// yyyymmddhhmiss 转换成utc
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
	pkg.pkg_latitude = FloatToStr(tmp_d, 6);		//纬度

	tmp_d = long_lac_cal(lsTrackerPkgList[5]);
	pkg.pkg_longitude = FloatToStr(tmp_d, 6);		//经度

	pkg.pkg_altitude = lsTrackerPkgList[6];		//海拔高度，以米为单位
	pkg.pkg_voltage = lsTrackerPkgList[7];			//电池电压
																								// 保存小区信息
	int filed_num = lsTrackerPkgList.GetCount();
	// 末尾带了,TA
	// 小区信息的组数
	int lac_cis = (filed_num - 8 - 1) / 3;
	LacCiRxLev lev;
	for (size_t i = 0; i < lac_cis; i++) {
		lev.Clear();
		lev.lac_ = lsTrackerPkgList[8 + 3 * i];
		lev.ci_ = lsTrackerPkgList[9 + 3 * i];
		lev.rx_lev_ = lsTrackerPkgList[10 + 3 * i];
		lac_ci_rxlev.push_back(lev);
	}
	//----2016-09-23--临时入表----
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
	// 检查小区组是否大于6
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
	//Socket接收完毕时间
	if (String2OTLDateTime(m_sRecvTime, otl_dt)) {
		otl_s << otl_dt;
	}	else {
		otl_s << otl_null();
	}
	//入库处理时间
	if (String2OTLDateTime(ts_time, otl_dt)) {
		otl_s << otl_dt;
	}
	else {
		otl_s << otl_null();
	}
	short loc_type = (pkg.pkg_longitude.IsEmpty() || pkg.pkg_latitude.IsEmpty()) ? 0 : 1;
	otl_s << loc_type;
	// 报文GPS时间，需要加8小时
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
	// 根据pkg.imei 去RP_BZ_CARMON_DEV表查询基础消息
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
	//入表RP_BR_DEVSTATUS_MGR
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
	return d * PI / 180.0;   //角度1? = π / 180
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
// 函数 : TCPigeonLocationPigeonHandler::RecvRequest(TCCustomUniSocket  &cusSocket)
// 用途 :	pad的数据处理方式，接收数据, 按照无阻塞接收
// 原型 : void RecvRequest()
// 参数 : 无
// 返回 : 无
// 说明 :
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

		//: 开始按照 select 来进行判断
		if (!cusSocket.WaitForData(nTimeOut)) {
			//: 超时120秒了，可以直接关闭连接；
			TCString sLog = "PigeonHandle Tracker[" + m_sTacker_Send_IPAddress + "]已经超时120秒未发送数据，断开连接";
			printf("%s\n", (char*)sLog);
			glgMls->AddPigeonLocationRunLog(sLog);
			throw TCException(sLog);
		}

		int nGetPkgLen = 0;
		nGetPkgLen = cusSocket.ReceiveBuf(sbuff, nMaxPkg_Length);
		m_sRecvTime = TCTime::Now();
		if (nGetPkgLen == 0) {
			TCString sLog = "PigeonHandle Tracker[" + m_sTacker_Send_IPAddress + "]已经断开连接";
			printf("%s\n", (char*)sLog);
			glgMls->AddPigeonLocationRunLog(sLog);
			throw TCException(sLog);
		}

		TCString sPkg_Content(sbuff, nGetPkgLen);

		if (Length(sPkg_Content)<4) {
			//: 异常，请检查数据
			TCString sLogMsg;
			sLogMsg = "";
			sLogMsg += TCString("ERROR: 请求报文长度异常=[") + IntToStr(Length(sPkg_Content)) + "]";
			glgMls->AddPigeonLocationRunLog(sLogMsg);
			printf("Error Packet Format: %s\n", (char*)sLogMsg);
			cusSocket.Close();
			throw TCException(sLogMsg);
			return;
		}

		m_sReq_Command = Mid(sPkg_Content, 3, 2);
		m_cDelimter = ',';
		m_sPkg_Content = Mid(sPkg_Content, 6, Length(sPkg_Content) - 4);

		//: 需要判断后续是否有多条记录，如果是，就放到一个vector中保存;
		if (AT(m_sPkg_Content, "AT")>0) {
			//: 后面还有多条记录
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

		//======== 5. 记录日志 =============
		TCString sLogMsg;
		sLogMsg = "";
		sLogMsg += TCString("请求命令码:(") + m_sReq_Command, +"), 记录数=(" + IntToStr(m_vsPkgList.size());
		sLogMsg += TCString(") 内容：（ ") + m_sPkg_Content;
		sLogMsg += TCString(") 请求包结束\n");
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
// 函数 : TCPigeonLocationPigeonHandler::SendRespPkg
// 用途 : 发送应答包
// 原型 : void SendRespPkg(TCString sRespCommand, TCString sRespContent)
// 参数 : sRespCommand---返回命令代码   sRespContent ---- 返回内容(前三个字节就是命令码)
// 返回 : 无
// 说明 :
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
// 函数 : void TCPigeonLocationPigeonHandler::DoCommand_PigeonLocInfo
// 用途 : 平板上报位置信息
// 原型 : void DoCommand_PigeonLocInfo()
// 参数 : 无
// 返回 : 无
// 说明 :
//==========================================================================
void TCPigeonLocationPigeonHandler::DoCommand_LocInfo(TCCustomUniSocket  &cusSocket) {
	field_counts = 9;
	for (int nPkgSeq = 0; nPkgSeq<m_vsPkgList.size(); nPkgSeq++) {
		std::srand(time(0));
		m_sPkg_Content = m_vsPkgList[nPkgSeq];
		//: 分解用户数据
		TCStringList lsTrackerPkgList;
		lsTrackerPkgList.Clear();
		lsTrackerPkgList.CommaText(m_sPkg_Content, m_cDelimter);

		if (lsTrackerPkgList.GetCount() < field_counts) {
			//: 至少15个数据域
			TCString sLogMsg;
			sLogMsg = "";
			sLogMsg += TCString("请求命令码:(") + m_sReq_Command;
			sLogMsg += TCString(") 报文异常，域数量=[") + IntToStr(lsTrackerPkgList.GetCount()) + "]";
			glgMls->AddPigeonLocationRunLog(sLogMsg);
			printf("报文异常TCPigeonLocationPigeonHandler::DoCommand_PigeonLocInfo: %s\n", (char *)sLogMsg);
			throw TCException(sLogMsg);
		}
		// 报文解析
		// 小区组
		std::vector<LacCiRxLev> lac_ci_rxlv;
		lac_ci_rxlv.clear();
		PigeonPkg pkg;
		pkg.Clear();
		// 解析报文
		MsgParsing(m_sPkg_Content, pkg, lac_ci_rxlv);

		std::cout << "PKG:" << (char *)m_sPkg_Content << std::endl;
	
		//TODO:根据IMEI查询表 RP_VD_IRING 获取 延时时间
		//1.CHANGE_TIME_DELAY 比赛延时下时间间隔
		//2.FLY_TIME_DELAY	飞翔状下时间间隔
		//3.NEST_TIME_DELAY	归巢冲刺时间间隔
		TimeDelay delay;
		delay.Clear();
		GetDelay(pkg, delay);
#ifdef OUT_DEBUG
		std::cout << "change_time_delay,fly_time_delay,nest_time_delay"
			<< delay.change_time_delay_ << "," << delay.fly_time_delay_ << "," << delay.nest_time_delay_
			<< std::endl;
#endif // OUT_DEBUG


		/*
		//基础数据查询，根据智能环查询 RP_BZ_RACINGPIGEON 表；IRING_IMEI=Pkg.IMEI 
		//获取该信鸽的 鸽舍位置信息：（NESTPAD_LONGITUDE，NESTPAD_LATITUDE）
		//*/
		//double nestpad_longitude = 0.0;
		//double nestpad_latitude = 0.0;
		//GetNestPadLoc(pkg, nestpad_latitude, nestpad_longitude);
		//TODO:状态确定：查询 RP_BR_GATHER_SUBPROC 表，条件是 VALID_FLAG=1 and IRING_IMEI=Pkg.IMEI;
		// 1)如果没有记录则返回报文中“数据上报延迟”字段的值设为 下一天00点00分00秒减去GPS时间
		RpBrgatherSubprocInfo gather_subproc_info;
		gather_subproc_info.Clear();
		int interval_recv = 60;
		bool have_record = false;
		if (CheckRpBrgatherSubproc(pkg, gather_subproc_info)) {
			// 有记录
			have_record = true;
			// 查询 RP_BZ_MATCH 表；获取当前比赛的 GATHER_TIME 和 ALLOCSTART_TIME(计划施放时间)
			// 以及 MATCHSTART_TIME(实际施放时间)
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
					//获取该信鸽的 鸽舍位置信息：（NESTPAD_LONGITUDE，NESTPAD_LATITUDE）
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
			// 无记录
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
		int random_num = std::rand() % 60;				//数据上报延迟随机数
		SendRespPkg(cusSocket, interval_recv + random_num);
		//
		if (have_record) {
			// 入表RP_BR_REALLOCATION
			OutputRpBrRealLocation(pkg, gather_subproc_info, lac_ci_rxlv, interval_recv + random_num);
		}
		// 入表RP_BR_DEVSTATUS_MGR
		OutputRpBrDevStatusMgr(pkg);
	}
	return;
}


//==========================================================================
// 函数 : void TCPigeonLocationPigeonHandler::Init
// 用途 : 初始化
// 原型 : void Init()
// 参数 : 无
// 返回 : 无
// 说明 :
//==========================================================================
void TCPigeonLocationPigeonHandler::Init() {
	m_sPkg_Content = "";
	m_sReq_Command = "";
	m_sRespCommand = "";
	m_sRespPkgContent = "";
	return;
}
