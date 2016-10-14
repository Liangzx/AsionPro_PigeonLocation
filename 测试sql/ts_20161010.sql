select * from  RP_BZ_CARMON_DEV
select * from RP_VD_CARMON

select * from mb_bss_terminal_location partition(PART_20161013)
 where active_time between to_date('2016-10-13 14:10:00', 'yyyy-mm-dd hh24:mi:ss')
 and to_date('2016-10-13 14:44:00', 'yyyy-mm-dd hh24:mi:ss') order by imei

select count(1) from mb_bss_terminal_location partition(PART_20161013) where IMEI='861510030001370'
select * from RP_BR_JUDGE_SUBPROC
select * from RP_BR_DEVSTATUS_MGR where dev_imei='865067022690369'
select * from RP_BR_DEVSTATUS_MGR where dev_imei='865067022697224'
select * from RP_BZ_GATHER_DEV where gatger_imei='865067022697224'
select * from RP_BZ_PIGEON_OWNER

select * from RP_BR_NESTPAD_SUBPROC
delete from RP_BR_NESTPAD_SUBPROC

select * from RP_BR_GATHER_SUBPROC
delete from RP_BR_GATHER_SUBPROC

select * from RP_BZ_RACINGPIGEON where bi_ringid='2016-22-11111111'
delete from RP_BZ_RACINGPIGEON where bi_ringid='2016-22-11111111'
select * from RP_BR_DEVSTATUS_MGR where dev_imei='865067022697224'
select * from RP_BR_DEVSTATUS_MGR where dev_imei='861510030001370'

select * from RP_BR_GATHER_SUBPROC
delete from RP_BR_GATHER_SUBPROC

select * from RP_VD_IRING
select * from RP_BR_REALLOCATION where iring_imei='861510030001370'
select * from RP_BZ_GATHER_DEV
--¹é³²
select * from  RP_BZ_RACINGPIGEON
select * from RP_BZ_IRING
select * from RP_BZ_NESTPAD

select * from RP_BR_MATCH_RESULT
select * from RP_BR_NESTPAD_SUBPROC
delete from RP_BR_MATCH_RESULT
delete from RP_BR_NESTPAD_SUBPROC
delete from RP_BR_MATCH_RESULT
select * from rp_bz_match

delete from RP_BR_NESTVIDEO_SUBPROC
select * from RP_BR_NESTVIDEO_SUBPROC
select * from RP_BR_PIGEON_NESTVIDEO
delete from RP_BR_PIGEON_NESTVIDEO

select iring_rfid,ring_type from RP_BZ_RACINGPIGEON order by iring_rfid
select * from RP_BR_GATHER_SUBPROC

select * from RP_BZ_PIGEON_OWNER
select * from RP_BZ_NESTPAD
select * from RP_BZ_IRING
select * from RP_BR_GATHER_SUBPROC
select * from RP_BZ_RACINGPIGEON
delete from RP_BZ_RACINGPIGEON
delete from RP_BR_GATHER_SUBPROC where iring_rfid='7A0021D772'
delete from RP_BZ_RACINGPIGEON where iring_rfid='XXXXXXXXXX'

select * from RP_BR_NESTPAD_SUBPROC
delete from RP_BR_NESTPAD_SUBPROC
select * from RP_BR_MATCH_RESULT
select * from RP_BZ_RACINGPIGEON




