�����豸�������ܹ��̱�(RP_BR_JUDGE_SUBPROC)
string sql_buf = "SELECT MATCHID,MATCHNAME,CARMONID FROM RP_BZ_CARMON_DEV A WHERE A.CARMON_IMEI = ";
sql_buf += pkg_imei;

string sql_buffer = "INSERT INTO ";
sql_buffer += tb_name_;
sql_buffer += "(MATCHID,";
sql_buffer += "MATCHNAME,";
sql_buffer += "MONITOR_TYPE,";
sql_buffer += "MONITOR_ID,";
sql_buffer += "MONITOR_IMEI,";
sql_buffer += "MONITOR_SNDTIME,";
sql_buffer += "MONITOR_RECVTIME,";
sql_buffer += "MONITOR_RECORDTIME,";
sql_buffer += "MONITOR_LONGITUDE,";
sql_buffer += "MONITOR_LATITUDE,";
sql_buffer += "MONITOR_HIGH,";
sql_buffer += "MONITOR_CONTENT)";
sql_buffer += "VALUES(";
sql_buffer += ":f1MATCHID<char[255]>,";
sql_buffer += ":f2MATCHNAME<char[255]>,";
sql_buffer += ":f3MONITOR_TYPE<short>,";
sql_buffer += ":f4MONITOR_ID<char[15]>,";
sql_buffer += ":f5MONITOR_IMEI<char[15]>,";
sql_buffer += ":f6MONITOR_SNDTIME<TIMESTAMP>,";
sql_buffer += ":f7MONITOR_RECVTIME<TIMESTAMP>,";
sql_buffer += ":f8MONITOR_RECORDTIME<TIMESTAMP>,";
sql_buffer += ":f9MONITOR_LONGITUDE<double>,";
sql_buffer += ":f10MONITOR_LATITUDE<double>,";
sql_buffer += ":f11MONITOR_HIGH<int>,";
sql_buffer += ":f12MONITOR_CONTENT<clob>)";

------------
���� ���� �鳲 ���ܻ� �����豸
[PIGEON/HOMINGPOS/PIGEONGATHER/VEHICLE]
VEHICLE:7000
1)GPSδ��λ
AT11,10,867106022056110,0,20161010101011,,,100,67,TA
2)���뼯������С��30����
AT11,10,867106022056110,1,20161010101011,30.466549,104.12747,100,67,TA
3)��ǰλ���뼯���ľ������30����,������ŷɵ�������10����
AT11,10,867106022056110,1,20161010101011,31.466549,104.12747,100,67,TA
4)��ŷɵ����С��10����
AT11,10,867106022056110,1,20161010101011,30.446549,104.12747,100,67,TA


PIGEONGATHER:7001
1)��������:
a)������ֵ
AT13,867106022056110,20160719191212,1,20160719191213,26.343084828707404,102.6133426732902,100,67,30,50,0,TA
b)δ������ֵ
AT13,867106022056110,20160719191212,1,20160719191213,30.666549,104.022747,100,67,30,50,0,TA

2)���뱨��:
a)������ֵ
AT13,865067022697224,,1,20160929095804,30.652377,104.008803,489.500,3.980,32,13980716333,,20160929095804,4,8500779A10,,8500779967,,7A000A625C,,7A000AA941,,TA
b)δ������ֵ
AT13,865067022697224,,1,20160929095804,30.652377,104.008803,489.500,3.980,32,13980716333,,20160929095804,4,8500779A10,,8500779967,,7A000A625C,,7A000AA941,,TA

3)��¼����:
AT13,865067022697224,,1,20160929095709,30.652362,104.008797,490.200,3.969,36,13980716333,1,0,7A000AA941,2016-22-44444444,0,0,0,,TA

4)��ѯ����:
a)�ܲ鵽
AT13,865067022697224,,1,20160929095703,30.652362,104.008798,490.200,3.941,34,13980716333,1,7A000AA941,TA
AT13,867106022056110,20160719191212,1,20160719191213,27.343084828707404,102.6133426732902,100,67,34,13980716333,2,6A73005E3B,6A73005E3C,TA
b)�鲻��
AT13,867106022056116,20160719191212,1,20160719191213,27.343084828707404,102.6133426732902,100,67,34,13980716353,2,6A73005E3Bc,TA


HOMINGPOS:7002
1)��������:
a)������ֵ
AT12,867106022056110,20160719191212,1,20160719191213,31.666549,104.022747,100,67,10,23,7,8,6,TA
b)δ������ֵ
AT12,867106022056110,20160719191212,1,20160719191213,30.666549,104.022747,100,67,10,0,0,0,0,TA


2)����鳲ָʾ
a)������ֵ
AT12,867106022056110,20160719191212,1,20160719191213,31.666549,104.022747,100,67,12,2,20160719191212,20160719191213,1,6A73005E3B,6A73005E3Bchk1,6A73005E3Bchk2,20160719191213,20160719191214,0,7A000A625C,6A73005E3Cchk1,6A73005E3Cchk2,0,TA
b)δ������ֵ
AT12,867106022056110,20160719191212,1,20160719191213,30.666549,103.022747,100,67,12,2,20160719191211,20160719191212,1,6A73005E3B,6A73005E3Bchk1,6A73005E3Bchk2,20160719111213,20160719111214,0,6A73005E3C,6A73005E3Cchk1,6A73005E3Cchk2,0,TA

3)�鳲��λ���ϴ��洢�ķ�����δ�������յ�RFID��ʱ������
a)������ֵ
AT12,867106022056110,20160719191212,1,20160719191213,32.666549,104.022747,100,67,14,2,20160719191212,20160719191213,6A73005E3B,chk1_1,chk1_2,1,20160719191212,20160719191213,6A73005E3C,chk2_1,chk2_2,1,0,TA

b)δ������ֵ
AT12,867106022056110,20160719191212,1,20160719191213,30.666549,104.022747,100,67,14,2,20160719191212,20160719191213,6A73005E3B,chk1_1,chk1_2,1,20160719191212,20160719191213,6A73005E3C,chk2_1,chk2_2,1,0,TA

5)���������Ϣ�ϴ�(�Ÿ�󶨱���)
a)������ֵ
AT12,867106022056110,20160719191212,1,20160719191213,31.666549,104.022747,100,67,18,2,1,6A73005E3B,6A73005E3Bchk1,22-01-100101,1,1,1,2,6A73005E3C,6A73005E3Cchk2,22-01-100102,2,2,2,TA
b)δ������ֵ
AT12,867106022056110,20160719191212,1,20160719191213,30.666549,104.022747,100,67,18,2,1,6A73005E3B,6A73005E3Bchk1,22-01-100101,1,1,1,2,6A73005E3C,6A73005E3Cchk2,22-01-100102,2,2,2,TA

4)����鳲��Ƶ
a)������ֵ
AT12,867106022056110,20160713191212,1,20160719191213,31.666549,104.022747,100,67,16,0,20160719191213,20160719191214,1,6A73005E3B,2,6,2,
123456789012
,TA
a)δ������ֵ
AT12,867106022056110,20160713191212,1,20160719191213,30.666549,104.022747,100,67,16,0,20160719191213,20160719191214,1,6A73005E3B,2,5,2,
1234567890
,TA

PIGEON:----δ����7003
AT10,60,867106022056110,1,20160816223312,26.343084828707404,102.6133426732902,100,67,123,333c,76ae,9898.7,TA
--
AT10,0,867106022056110,1,20160719191213,30.666549,104.022747,123,3.928,335e,7595,47,333c,76ae,41,335e,7f27,40,335e,7597,39,335e,797f,36,335e,76a4,34,TA

AT10,0,861510030001370,1,022310.984,30.488658,104.047308,500.7,3.818,8028,ae13,15,0000,ffff,16,0000,ffff,09,8028,ae11,09,800c,71b9,05,0000,ffff,03,7,TA
------------
1)����ǰ:����pkg.imei������ѯRP_BR_GATHER_SUBPROCû�м�¼
AT10,60,867106022056111,1,20160816223312,26.343084828707404,102.6133426732902,100,67,123,333c,76ae,9898.7,TA
2)������:����pkg.imei��ȡRP_BR_GATHER_SUBPROC��Ӧ��MATCHIDȻ����ݸ�MATCHIDȥRP_BZ_MATCH��ѯ�����Ϣ
a)
------------

nohup /home/Asion/bin/PigeonLocation/PigeonLocation.bin -MutiP2 >> /home/Asion/Run_Out/PigeonLocation_P2.out  2>&1&

����:124.161.38.244:7000->7004
����:124.161.38.244:7001
�鳲:124.161.38.244:7002
���ܻ�:124.161.38.244:7003
----
����34:
�����ֻ���														12345678901
�鳲������m														M
����ID_1															999
��������_1														����1
�����ַ_1														��ַ1
�鳲�����_1													001
�鳲��ID_1														002
�鳲��IMEI_1													867106022056110
...
����ID_M															999
��������_M														����1
�����ַ_M														��ַ1
�鳲�����_M													001
�鳲��ID_M														002
�鳲��IMEI_M													867106022056110
RFID����N															N
RFID_1״̬														0
RFID_1��ӦBI_RINGID										8989
...
RFID_N״̬														0
RFID_N��ӦBI_RINGID										8989
----
����:RP_BZ_RACINGPIGEON
OWNERID														varchar2(16)
OWNERNAME                         varchar2(32)
OWNERNAME_CHN                     varchar2(32)
BI_RINGID													varchar2(16)
SEX                               number(1)
COLOR                             number(2)
SANDEYE                           number(1)
IRINGID                           varchar2(16)
IRING_RFID                        varchar2(16)
IRING_IMEI                        varchar2(16)
IRING_BLEMAC                      varchar2(16)
IRING_MOBILE                      varchar2(16)
IRING_IMSI                        varchar2(16)
FIT_TIME                          datetime
FIT_MODEL                         Number(1)
NESTPAD_RFID_INITCRC              varchar2(16)
FIT_DEV_TYPE                      Number(1)
FIT_DEV_ID                        varchar2(16)
FIT_DEV_IMEI                      varchar2(16)
FIT_DEV_BLEMAC                    varchar2(16)
FIT_DEV_LONGITUDE                 Number(10,6)
FIT_DEV_LATITUDE                  Number(10,6)
FIT_DEV_HIGH                      Number(8)
VALID_FLAG                        Number(1)
----
a)�鳲����:
���ݶ�ͷAT
�豸����
�豸ʶ����
�鳲��ʱ��
GPS��λ״̬
GPSʱ��
γ��
����
�߶�
��ص�ѹ
��Ϣ����
��Ϣ����
������TA
b)��������Ӧ:
���ݶ�ͷ
��Ϣ����
��Ϣ����
������

----
/home/liangzx/AsionPro/PigeonLocation/source

nohup /home/liangzx/AsionPro/PigeonLocation/src/PigeonLocation.bin -MutiP3 >> /home/liangzx/Run_Out/PigeonLocation_P3.out  2>&1&
nohup /home/liangzx/AsionPro/PigeonLocation/source/PigeonLocation.bin -MutiP1 >> /home/liangzx/Run_Out/PigeonLocation_P1.out  2>&1&

alter table mb_bss_terminal_location add partition PART_20161010 values less than (TIMESTAMP'2016-10-11 00:00:00.000000') tablespace  T_MOBILE compress
alter table mb_bss_terminal_location add partition PART_20161002 values less than (TIMESTAMP'2016-10-03 00:00:00.000000') tablespace  T_MOBILE compress
alter table mb_bss_terminal_location add partition PART_20161003 values less than (TIMESTAMP'2016-10-04 00:00:00.000000') tablespace  T_MOBILE compress
alter table mb_bss_terminal_location add partition PART_20161004 values less than (TIMESTAMP'2016-10-05 00:00:00.000000') tablespace  T_MOBILE compress
alter table mb_bss_terminal_location add partition PART_20161005 values less than (TIMESTAMP'2016-10-06 00:00:00.000000') tablespace  T_MOBILE compress
alter table mb_bss_terminal_location add partition PART_20161006 values less than (TIMESTAMP'2016-10-07 00:00:00.000000') tablespace  T_MOBILE compress
alter table mb_bss_terminal_location add partition PART_20161007 values less than (TIMESTAMP'2016-10-08 00:00:00.000000') tablespace  T_MOBILE compress
alter table mb_bss_terminal_location add partition PART_20161008 values less than (TIMESTAMP'2016-10-09 00:00:00.000000') tablespace  T_MOBILE compress
alter table mb_bss_terminal_location add partition PART_20161009 values less than (TIMESTAMP'2016-10-10 00:00:00.000000') tablespace  T_MOBILE compress
alter table mb_bss_terminal_location add partition PART_20161010 values less than (TIMESTAMP'2016-10-11 00:00:00.000000') tablespace  T_MOBILE compress


FDD-LTE_MRS_ERICSSON_OMC1_464266_2016092716300022.xml.zip


----
20160929
���뾭γ:
30.652362,104.008797
select * from RP_BR_GATHER_SUBPROC
--30.652377,104.008803
select * from RP_BZ_RACINGPIGEON
--104.008797,30.652362

---
20160930
match_id -->varchar[32]
�漰�ı�:
RP_BR_GATHER_SUBPROC
RP_BR_MATCH_RESULT
RP_BR_NESTVIDEO_SUBPROC
RP_BR_PIGEON_NESTVIDEO
RP_BR_NESTPAD_SUBPROC
RP_BR_REALLOCATION
RP_BR_JUDGE_SUBPROC
RP_BZ_MATCH
----
AT12															AT12           				���ݶ�ͷ �豸����
865067022697042                   867106022056110       �豸ʶ����
                                  20160719191212        �鳲��ʱ��
1                                 1                     GPS��λ״̬
20160930034445                    20160719191213        GPSʱ��
30.647082                         31.666549             γ��
104.015182                        104.022747            ����
513.500                           100                   �߶�
4.159                             67             				��ص�ѹ
12                                12             				��Ϣ����
1                                 2              				�鳲RFID����N
                                  20160719191212 				�鳲��ʱ�� 1
20160930034441                    20160719191213 				GPSʱ��1
0                                 1              				ģʽ1
0                                 6A73005E3B 						�鳲RFID��ʶ1
7A000A625C                        6A73005E3Bchk1
                                  6A73005E3Bchk2
                                  20160719191213
0                                 20160719191214
TA                                0
                                  6A73005E3C
                                  6A73005E3Cchk1
                                  6A73005E3Cchk2
                                  0
                                  TA

AT12,865067022697042,,1,20160930034445,30.647082,104.015182,513.500,4.159,12,1,,20160930034441,0,0,7A000A625C,,,0,TA--v1.10
AT12,865067022697042,,1,20160930034445,30.647082,104.015182,513.500,4.159,12,1,20160930034441,20160930034441,0,7A000A625C,,,0,TA

RP_BZ_MATCH:
29-SEP-16 03.30.00.000000 PM 104.067993 30.617798
RP_BR_MATCH_RESULT:
29-SEP-16 03.30.00.000000 PM 104.067993 30.617798

104.015182 30.647082 30-SEP-16 01.15.58.000000 PM

201609291530
201609301315

20160929153000 20160930114441