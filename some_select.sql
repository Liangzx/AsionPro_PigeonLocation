select * from RP_BZ_COLLECTION
select * from RP_BZ_PIGEON_OWNER
select * from RP_BZ_PIGEON_OWNER@MY_MOBILE_26

select * from RP_VD_IRING@MY_MOBILE_26
insert into RP_VD_IRING select * from RP_VD_IRING@MY_MOBILE_26
commit;
select * from RP_VD_IRING;
--
select * from RP_BZ_PIGEON_OWNER
select * from RP_BZ_GATHER_DEV
select * from RP_BZ_NESTPAD
select * from RP_BZ_IRING
select * from RP_VD_IRING
select * from RP_BZ_MATCH
select * from RP_BR_REALLOCATION
select * from RP_VD_CARMON
--|select * from RP_BZ_CARMON_DEV@MY_MOBILE_26
insert into RP_BZ_CARMON_DEV select * from RP_BZ_CARMON_DEV@MY_MOBILE_26
commit
select * from RP_BR_JUDGE_SUBPROC

--select * from RP_BZ_RACINGPIGEON
rename RP_BZ_RACINGPIGEON to RP_BZ_RACINGPIGEON_BAK
create table RP_BZ_RACINGPIGEON as select * from RP_BZ_RACINGPIGEON@MY_MOBILE_26

--select *  from RP_BR_DEVSTATUS_MGR
rename RP_BR_DEVSTATUS_MGR to RP_BR_DEVSTATUS_MGR_BAK
create table RP_BR_DEVSTATUS_MGR as select * from RP_BR_DEVSTATUS_MGR@MY_MOBILE_26

--select * from RP_BR_GATHER_SUBPROC
rename RP_BR_GATHER_SUBPROC to RP_BR_GATHER_SUBPROC_BAK
create table RP_BR_GATHER_SUBPROC as select * from RP_BR_GATHER_SUBPROC@MY_MOBILE_26

--select * from RP_BR_NESTPAD_SUBPROC
rename RP_BR_NESTPAD_SUBPROC to RP_BR_NESTPAD_SUBPROC_BAK
create table RP_BR_NESTPAD_SUBPROC as select * from RP_BR_NESTPAD_SUBPROC@MY_MOBILE_26

--select * from RP_BR_NESTVIDEO_SUBPROC
rename RP_BR_NESTVIDEO_SUBPROC to RP_BR_NESTVIDEO_SUBPROC_BAK
create table RP_BR_NESTVIDEO_SUBPROC as select * from RP_BR_NESTVIDEO_SUBPROC@MY_MOBILE_26

--select * from RP_BR_PIGEON_NESTVIDEO
rename RP_BR_PIGEON_NESTVIDEO to RP_BR_PIGEON_NESTVIDEO_BAK
create table RP_BR_PIGEON_NESTVIDEO as select * from RP_BR_PIGEON_NESTVIDEO@MY_MOBILE_26
--
commit;

select * from user_tables where table_name like '%RP%'