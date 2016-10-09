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
#include "c_PigeonLocation_VehicleHandler.h"
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
// ���� : TCPigeonLocationVehicleHandler::TCPigeonLocationVehicleHandler
// ��; : ���캯��
// ԭ�� : TCPigeonLocationVehicleHandler()
// ���� :
// ���� :
// ˵�� :
//==========================================================================
TCPigeonLocationVehicleHandler::TCPigeonLocationVehicleHandler(){
}

//==========================================================================
// ���� : TCPigeonLocationVehicleHandler::~TCPigeonLocationVehicleHandler()
// ��; : ��������
// ԭ�� : ~TCPigeonLocationVehicleHandler()
// ���� :
// ���� :
// ˵�� :
//==========================================================================
TCPigeonLocationVehicleHandler::~TCPigeonLocationVehicleHandler()
{
}

//==========================================================================
// ���� : TCPigeonLocationVehicleHandler::Main_Handler
// ��; : ��Ҫʵ�־����ҵ�񽻻��߼�
// ԭ�� : void Main_Handler()
// ���� :
// ���� :
// ˵�� : ��Ҫʵ�־����ҵ�񽻻��߼�
//==========================================================================
bool TCPigeonLocationVehicleHandler::Main_Handler(TCCustomUniSocket  &cusSocket){
	try{
		vender = gPigeonLocationConfig->GetTrackerVendor();
		//: Զ�̷��͵ĵ�ַ
		m_sTacker_Send_IPAddress=cusSocket.GetRemoteAddress();
		m_nTimeOut=60;
#ifdef __TEST__
		LOG_WRITE("���յ� Tracker ��������IP=[%s], Port=[%d], Timeout=[%d]\n"
			, (char*)cusSocket.GetRemoteAddress(), cusSocket.GetRemotePort(), m_nTimeOut);
#endif // __TEST__
		
		//: �����ӷ�ʽ
		while(1){			
			Init();
#ifdef __TEST__
		TCString sLogMsg = "Time:Now=[" + TCTime::Now() + "],Recv Port=[" + IntToStr(cusSocket.GetRemotePort()) + "] Data";
		glgMls->AddPigeonLocationRunLog(sLogMsg);
#endif // __TEST__

 			RecvRequest(cusSocket);
			threshold_flag = 0;
 			DealRequest(cusSocket);	
 		}
 		cusSocket.Close();
 		return true;
  }catch(TCException &e){
    TCString sLogMsg = TCString("At TCPigeonLocationVehicleHandler::Main_Handler ��������ʱ����:") + e.GetExceptionMessage();
    glgMls->AddPigeonLocationRunLog(sLogMsg);
    cusSocket.Close();
    return false;
  }catch(...){
    glgMls->AddPigeonLocationRunLog("At TCPigeonLocationVehicleHandler::Main_Handler Unknow, �ر�Socket����");
    cusSocket.Close();
    return false;
 	}
}

//==========================================================================
// ���� : TCPigeonLocationVehicleHandler::DealRequest
// ��; : ��������
// ԭ�� : void DealRequest()
// ���� : ��
// ���� : ��
// ˵�� :
//==========================================================================
void TCPigeonLocationVehicleHandler::DealRequest(TCCustomUniSocket  &cusSocket){
	try{
		//: ���������ֽ��д���
#ifdef __TEST__
   	printf("============receive Tracker request==============\n");
   	LOG_WRITE("Command[%s], Content[%s], Time=[%s]\n", (char*)m_sReq_Command, (char*)m_sPkg_Content, (char*)TCTime::Now());
#endif

		//======ִ�з������==========================
    m_sRespCommand="";
    m_sRespPkgContent="";
	 if (m_sReq_Command=="11"){
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
    }catch(TCException &e1){
      TCString sLogMsg = TCString("ϵͳ����: when Catch a Error and DealRequest: ") + e1.GetExceptionMessage();
      glgMls->AddPigeonLocationRunLog(sLogMsg);
    }catch(...){
      glgMls->AddPigeonLocationRunLog("ϵͳ����δ֪�쳣");
    }
  }
}


void TCPigeonLocationVehicleHandler::MsgParsing(const TCString & content, VehiclePkg & pkg)
{
	//��������
	TCStringList lsTrackerPkgList;
	lsTrackerPkgList.Clear();
	lsTrackerPkgList.CommaText(content, m_cDelimter);

	pkg.pkg_pkg_offset = lsTrackerPkgList[0];	//���ݰ�ʱ��ƫ��
	pkg.pkg_imei = lsTrackerPkgList[1];				//�豸ʶ����
	pkg.pkg_gps_flag = lsTrackerPkgList[2];		//GPS��λ״̬ 0/1
	TCString pkg_gps_time = lsTrackerPkgList[3];		//GPSʱ��yyyyMMddhhmmss
	pkg.pkg_beijing_time = TCTime::RelativeTime(pkg_gps_time, 8 * 3600);
	pkg.pkg_latitude = lsTrackerPkgList[4];		//γ��
	pkg.pkg_longitude = lsTrackerPkgList[5];		//����
	pkg.pkg_altitude = lsTrackerPkgList[6];		//���θ߶ȣ�����Ϊ��λ
	pkg.pkg_voltage = lsTrackerPkgList[7];			//��ص�ѹ
	std::cout << "PKG:" << m_sPkg_Content << std::endl;
	std::cout << "���ݰ�ʱ��ƫ��:" << (char *)pkg.pkg_pkg_offset <<
		"���θ߶�:" << (char *)pkg.pkg_altitude << std::endl;
	//----2016-09-23--���----
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
}

inline double TCPigeonLocationVehicleHandler::radian(double d)
{
	return d * PI / 180.0;   //�Ƕ�1? = �� / 180
}

inline double TCPigeonLocationVehicleHandler::get_distance(double lat1, double lng1, double lat2, double lng2)
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
// ���� : TCPigeonLocationVehicleHandler::RecvRequest(TCCustomUniSocket  &cusSocket)
// ��; :	pad�����ݴ���ʽ����������, ��������������
// ԭ�� : void RecvRequest()
// ���� : ��
// ���� : ��
// ˵�� : 
//==========================================================================
void TCPigeonLocationVehicleHandler::RecvRequest(TCCustomUniSocket  &cusSocket){
	try{
		const int nMaxPkg_Length=1024*1000;
		char sbuff[nMaxPkg_Length];	
		memset(sbuff, 0, sizeof(sbuff));
		m_vsPkgList.clear();
		m_sReq_Command="";
		m_sPkg_Content="";
				
		const int nTimeOut=600*1000;
		
		//: ��ʼ���� select �������ж�
		if(!cusSocket.WaitForData(nTimeOut)){
			//: ��ʱ120���ˣ�����ֱ�ӹر����ӣ�
			TCString sLog="Vehicle Tracker[" + m_sTacker_Send_IPAddress + "]�Ѿ���ʱ120��δ�������ݣ��Ͽ�����";
			LOG_WRITE("%s\n", (char*)sLog);
			throw TCException(sLog);
		}
		
		int nGetPkgLen=0;
		nGetPkgLen=cusSocket.ReceiveBuf(sbuff, nMaxPkg_Length);
		m_sRecvTime = TCTime::Now();
		if(nGetPkgLen==0){
			TCString sLog="Vehicle Tracker[" + m_sTacker_Send_IPAddress + "]�Ѿ��Ͽ�����";
			LOG_WRITE("%s\n", (char*)sLog);
			//glgMls->AddPigeonLocationRunLog(sLog);
			throw TCException(sLog);
		}
			
		TCString sPkg_Content(sbuff, nGetPkgLen);
		
    if(Length(sPkg_Content)<4){
    	//: �쳣����������
    	TCString sLogMsg;
		sLogMsg = "";
		sLogMsg += TCString("ERROR: �����ĳ����쳣=[")+IntToStr(Length(sPkg_Content))+"]" ;
		LOG_WRITE("%s\n", (char *)sLogMsg);
	  	//glgMls->AddPigeonLocationRunLog(sLogMsg);
		//printf("Error Packet Format: %s\n", (char*)sLogMsg);
    	cusSocket.Close();
		throw TCException(sLogMsg);
    }
    
    m_sReq_Command=Mid(sPkg_Content, 3, 2);
    m_cDelimter=',';
    m_sPkg_Content=Mid(sPkg_Content, 6, Length(sPkg_Content)-4);

    //: ��Ҫ�жϺ����Ƿ��ж�����¼������ǣ��ͷŵ�һ��vector�б���;
    if(AT(m_sPkg_Content, "AT")>0) {
			//: ���滹�ж�����¼
			TCString cmd_str = "AT" + m_sReq_Command + ",";
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
    
 //   //======== 5. ��¼��־ =============
 //   TCString sLogMsg;
	//sLogMsg = "";
	//sLogMsg += TCString("����������:(") + m_sReq_Command, +"), ��¼��=("+IntToStr(m_vsPkgList.size());
	//sLogMsg += TCString(") ���ݣ��� ") + m_sPkg_Content;
	//sLogMsg += TCString(") ���������\n");
 //   glgMls->AddPigeonLocationRunLog(sLogMsg);
  }catch (TCException &e){
    cusSocket.Close();
    throw e;
  }catch (...){
    cusSocket.Close();
    throw TCException("TCPigeonLocationVehicleHandler::RecvRequest() Error:"
                        "Unknown Exception.");
  }
}

//==========================================================================
// ���� : TCPigeonLocationVehicleHandler::SendRespPkg
// ��; : ����Ӧ���
// ԭ�� : void SendRespPkg(TCString sRespCommand, TCString sRespContent)
// ���� : sRespCommand---�����������   sRespContent ---- ��������(ǰ�����ֽھ���������)
// ���� : ��
// ˵�� :
//==========================================================================
void TCPigeonLocationVehicleHandler::SendRespPkg(TCCustomUniSocket  &cusSocket, int interval){
	TCString sTmpRespContent="AT";
	sTmpRespContent+=IntToStr(interval);
	sTmpRespContent+=",TA";

  cusSocket.SendBuf((char *)sTmpRespContent, Length(sTmpRespContent));

	m_sSendTime=TCTime::Now();

#ifdef __TEST__		
	LOG_WRITE("RecvTime=[%s], SendTime=[%s], SendContent=[%s]\n", (char*)m_sRecvTime, (char*)m_sSendTime, (char*)sTmpRespContent);
#endif
}

//==========================================================================
// ���� : void TCPigeonLocationVehicleHandler::DoCommand_PigeonLocInfo
// ��; : ƽ���ϱ�λ����Ϣ
// ԭ�� : void DoCommand_PigeonLocInfo()
// ���� : ��
// ���� : ��
// ˵�� :  
//==========================================================================
void TCPigeonLocationVehicleHandler::DoCommand_LocInfo(TCCustomUniSocket  &cusSocket){
	field_counts = 7;
	for(int nPkgSeq=0; nPkgSeq<m_vsPkgList.size(); nPkgSeq++){
		__ENTERFUNCTION;
		std::srand(time(0));
		m_sPkg_Content = m_vsPkgList[nPkgSeq];
		//: �ֽ��û�����
		TCStringList lsTrackerPkgList;
		lsTrackerPkgList.Clear();
		lsTrackerPkgList.CommaText(m_sPkg_Content, m_cDelimter);
		
		if(lsTrackerPkgList.GetCount() < field_counts){
			//: ����7��������
			TCString sLogMsg;
			sLogMsg = "";
			sLogMsg += TCString("����������:(") + m_sReq_Command;
			sLogMsg += TCString(") �����쳣��������=[") + IntToStr(lsTrackerPkgList.GetCount())+"]";
			//glgMls->AddPigeonLocationRunLog(sLogMsg);
			LOG_WRITE("�����쳣TCPigeonLocationVehicleHandler::DoCommand_PigeonLocInfo: %s\n", (char *)sLogMsg);
			throw TCException(sLogMsg);
		}
		//���Ľ���

		VehiclePkg pkg;
		MsgParsing(m_sPkg_Content, pkg);

		//TODO:����IMEI��ѯ�� RP_VD_CARMON ��ȡ
		//INIT_START_DELAY	��ʼ����ʱ����
		//HIGH_SPEED_DELAY	�����ϵ�ʱ����
		//REACH_TIME_DELAY	�ﵽ�ε�ʱ����
		std::string sql_buf = "SELECT INIT_START_DELAY,HIGH_SPEED_DELAY,REACH_TIME_DELAY";
		sql_buf += " FROM RP_VD_CARMON A ";
		sql_buf += " WHERE A.IMEI='";
		sql_buf += pkg.pkg_imei;
		sql_buf += "'";
		std::cout << "sql_buf:" << sql_buf << std::endl;
		int init_start_delay = 60;
		int high_speed_delay = 60;
		int reach_time_delay = 60;
		otl_stream otl_s;
		try
		{
			otl_s.open(1, sql_buf.c_str(), m_dbConnect);
			if (!otl_s.eof()) {
				otl_s >> init_start_delay;
				otl_s >> high_speed_delay;
				otl_s >> reach_time_delay;
			}
		}
		catch (const otl_exception& e)
		{
			LOG_INFO("SQLִ���쳣\n\tmsg=[%s]\n\tSQL=[%s]\n\tvar_info=[%s]", e.msg, e.stm_text, e.var_info);
		}
		otl_s.flush();
		otl_s.close();
#ifdef __TEST__
		std::cout << "init_start_delay,high_speed_delay,reach_time_delay:"
			<< init_start_delay << ',' << high_speed_delay << ',' << reach_time_delay << std::endl;
#endif // __TEST__


		//TODO:�������ݲ�ѯ RP_BZ_CARMON_DEV ��CarMon_IMEI =Pkg.IMEI;
		//
		//MAINGATHER_LONGITUDE	����ص�GPS����
		//MAINGATHER_LATITUDE	����ص�GPSγ��
		//MATCHSTART_LONGITUDE	�����ŷɵص�GPS����
		//MATCHSTART_LATITUDE	�����ŷɵص�GPSγ��
		//MATCHID	����ID
		//MATCHNAME	��������
		//
		sql_buf.clear();
		sql_buf = "SELECT MAINGATHER_LATITUDE,MAINGATHER_LONGITUDE,MATCHSTART_LATITUDE,";
		sql_buf +=	"MATCHSTART_LONGITUDE,MATCHID,MATCHNAME FROM RP_BZ_CARMON_DEV A WHERE A.CARMON_IMEI ='";
		sql_buf += pkg.pkg_imei;
		sql_buf += "'";
		otl_s.open(10, sql_buf.c_str(), m_dbConnect);

		double gather_latitude = 0.0;
		double gather_longitude = 0.0;
		double start_latitude = 0.0;
		double start_longitude = 0.0;

		std::string match_id = "";
		std::string match_name = "";

		if (!otl_s.eof()) {
			otl_s >> gather_latitude;
			otl_s >> gather_longitude;
			otl_s >> start_latitude;
			otl_s >> start_longitude;
			otl_s >> match_id;
			otl_s >> match_name;
		} else {
			LOG_WRITE("[%s]��RP_BZ_CARMON_DEV�޶�ӦIMEI[%s]��Ϣ\n", (char *)TCTime::Now()
				, (char *)pkg.pkg_imei);
		}
		otl_s.flush();
		otl_s.close();

		int interval_recv = reach_time_delay;
		int random_num = std::rand() % 60;				//�����ϱ��ӳ������

		//���ݵ�ǰ��GPS��Ϣ�뼯����λ�ã������ŷɵ��λ�ã��������
		double gather_distance = get_distance(StrToFloat(pkg.pkg_latitude), StrToFloat(pkg.pkg_longitude),
			gather_latitude, gather_longitude);

#ifdef __TEST__
		std::cout << "gather_distance:" << gather_distance << std::endl;
#endif // __TEST__

		double start_distance = get_distance(StrToFloat(pkg.pkg_latitude), StrToFloat(pkg.pkg_longitude),
			start_latitude, start_longitude);

#ifdef __TEST__
		std::cout << "start_distance:" << start_distance << std::endl;
#endif // __TEST__
		
		if (pkg.pkg_gps_flag == "0" || gather_distance <= 30.0) {
			interval_recv = init_start_delay;
#ifdef __TEST__
			std::cout << "interval_recv::init_start_delay(30]km" << init_start_delay
				<< "," << random_num << std::endl;
#endif // __TEST__
		}	else if (gather_distance > 30.0 && start_distance > 10.0) {
			interval_recv = high_speed_delay;
#ifdef __TEST__
			std::cout << "interval_recv::high_speed_delay(10-30)km" << high_speed_delay
				<< "," << random_num << std::endl;
#endif // __TEST__

		}	else {
#ifdef __TEST__
			std::cout << "interval_recv::reach_time_delay(else]km" << reach_time_delay
				<< "," << random_num << std::endl;
#endif // __TEST__
		}
		
		interval_recv += random_num;
		SendRespPkg(cusSocket, interval_recv);
		// ������дRP_BR_JUDGE_SUBPROC
		OutputRpBrJudgeSubproc(pkg);
		// RP_BR_DEVSTATUS_MGR ��Ǽǳ��ص�״̬��Ϣ
		OutputRpBrDevStatusMgr(pkg);
		__LEAVEFUNCTION;
	}
	return;	
}

//==========================================================================
// ���� : void TCPigeonLocationVehicleHandler::Init
// ��; : ��ʼ��
// ԭ�� : void Init()
// ���� : ��
// ���� : ��
// ˵�� : 
//==========================================================================
void TCPigeonLocationVehicleHandler::Init(){
	m_sPkg_Content="";
	m_sReq_Command="";
	m_sRespCommand="";
	m_sRespPkgContent="";
	return;
}

void TCPigeonLocationVehicleHandler::OutputRpBrJudgeSubproc(const VehiclePkg & pkg)
{
	__ENTERFUNCTION;
	// ����pkg.imei ȥRP_BZ_CARMON_DEV���ѯ������Ϣ
	std::string sql_buf = "SELECT  MATCHID,MATCHNAME,CarMonID FROM RP_BZ_CARMON_DEV A ";
	sql_buf += "WHERE A.CARMON_IMEI ='";
	sql_buf += (char *)pkg.pkg_imei;
	sql_buf += "'";

	otl_stream otl_s(10, sql_buf.c_str(), m_dbConnect);
	std::string match_id, match_name, carmonid;
	if (otl_s.eof()) {
		LOG_WRITE("[%s]��RP_BZ_CARMON_DEV�޶�ӦIMEI[%s]��Ϣ", (char *)TCTime::Now(), (char *)pkg.pkg_imei);
		return;
	}
	otl_s >> match_id;
	otl_s >> match_name;
	otl_s >> carmonid;
	otl_s.flush();
	otl_s.close();

	// ���RP_BR_JUDGE_SUBPROC
	std::string tb_name = "RP_BR_JUDGE_SUBPROC";
	sql_buf.clear();
	sql_buf = "INSERT INTO ";
	sql_buf += tb_name;
	sql_buf += "(MATCHID,";
	sql_buf += "MATCHNAME,";
	sql_buf += "MONITOR_TYPE,";
	sql_buf += "MONITOR_ID,";
	sql_buf += "MONITOR_IMEI,";
	sql_buf += "MONITOR_SNDTIME,";
	sql_buf += "MONITOR_RECVTIME,";
	sql_buf += "MONITOR_RECORDTIME,";
	sql_buf += "MONITOR_LONGITUDE,";
	sql_buf += "MONITOR_LATITUDE,";
	sql_buf += "MONITOR_HIGH)";
	//sql_buf += "MONITOR_CONTENT)";
	sql_buf += "VALUES(";
	sql_buf += ":f1MATCHID<char[33]>,";
	sql_buf += ":f2MATCHNAME<char[256]>,";
	sql_buf += ":f3MONITOR_TYPE<short>,";
	sql_buf += ":f4MONITOR_ID<char[17]>,";
	sql_buf += ":f5MONITOR_IMEI<char[17]>,";
	sql_buf += ":f6MONITOR_SNDTIME<TIMESTAMP>,";
	sql_buf += ":f7MONITOR_RECVTIME<TIMESTAMP>,";
	sql_buf += ":f8MONITOR_RECORDTIME<TIMESTAMP>,";
	sql_buf += ":f9MONITOR_LONGITUDE<double>,";
	sql_buf += ":f10MONITOR_LATITUDE<double>,";
	sql_buf += ":f11MONITOR_HIGH<int>)";
	//sql_buf += ":f12MONITOR_CONTENT<clob>)";

	otl_s.open(10, sql_buf.c_str(), m_dbConnect);
	otl_s << match_id;
	otl_s << match_name;
	short monitor_type = 1;
	otl_s << monitor_type;
	otl_s << carmonid;
	otl_s << (char *)pkg.pkg_imei;
	otl_datetime otl_date;
	if (String2OTLDateTime(pkg.pkg_beijing_time, otl_date)) {
		otl_s << otl_date;
	}	else {
		otl_s << otl_null();
	}
	if (String2OTLDateTime(m_sRecvTime, otl_date)) {
		otl_s << otl_date;
	}
	else {
		otl_s << otl_null();
	}
	if (String2OTLDateTime(TCTime::Now(), otl_date)) {
		otl_s << otl_date;
	}
	else {
		otl_s << otl_null();
	}
	otl_s << StrToFloat(pkg.pkg_longitude);
	otl_s << StrToFloat(pkg.pkg_latitude);
	otl_s << (int)StrToInt(pkg.pkg_altitude);
	//otl_s << otl_null();
	otl_s.flush();
	otl_s.close();
	__LEAVEFUNCTION;
}

void TCPigeonLocationVehicleHandler::OutputRpBrDevStatusMgr(const VehiclePkg & pkg)
{
	__ENTERFUNCTION;
	// ����pkg.imei ȥRP_BZ_CARMON_DEV���ѯ������Ϣ
	std::string sql_buf = "SELECT  CARMONID FROM RP_BZ_CARMON_DEV A ";
	sql_buf += "WHERE A.CARMON_IMEI ='";
	sql_buf += (char *)pkg.pkg_imei;
	sql_buf += "'";
	//
	std::string carmon_id;
	otl_stream otl_s(10, sql_buf.c_str(), m_dbConnect);
	otl_s >> carmon_id;
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
	sql_buf += "THRESHOLD_FLAG)";
	sql_buf += "VALUES(";
	sql_buf += ":f1DEV_TYPE<int>,";
	sql_buf += ":f2DEV_ID<char[17]>,";
	sql_buf += ":f3DEV_IMEI<char[17]>,";
	sql_buf += ":f4DEV_LONGITUDE<double>,";
	sql_buf += ":f5DEV_LATITUDE<double>,";
	sql_buf += ":f6DEV_HIGH<int>,";
	sql_buf += ":f7STATUS_TIME<TIMESTAMP>,";
	sql_buf += ":f8STATUS_TYPE<int>,";
	sql_buf += ":f9THRESHOLD_FLAG<short>";
	sql_buf += ")";

	otl_s.open(1, sql_buf.c_str(), m_dbConnect);
	int dev_type = 4;
	otl_s << dev_type;
	otl_s << carmon_id;
	otl_s << (char *)pkg.pkg_imei;
	otl_s << StrToFloat(pkg.pkg_longitude);
	otl_s << StrToFloat(pkg.pkg_latitude);
	otl_s << (int)StrToInt(pkg.pkg_altitude);
	TCString tmp_time = (pkg.pkg_gps_flag == "1") ? pkg.pkg_beijing_time : TCTime::Now();
	otl_datetime otl_date;
	if (String2OTLDateTime(tmp_time, otl_date)) {
		otl_s << otl_date;
	}	else {
		otl_s << otl_null();
	}
	int status_type = 2;
	otl_s << status_type;
	otl_s << threshold_flag;
	otl_s.flush();
	otl_s.close();
	__LEAVEFUNCTION;
}


