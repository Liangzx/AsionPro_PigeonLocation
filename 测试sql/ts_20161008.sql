select * from MB_BSS_TERMINAL_LOCATION
select count(1) from MB_BSS_TERMINAL_LOCATION partition(PART_20161009);
create table MB_BSS_TERMINAL_LOCATION_LZX as select * from MB_BSS_TERMINAL_LOCATION;

select table_name, partition_name from user_tab_partitions where table_name='MB_BSS_TERMINAL_LOCATION';

ALTER TABLE MB_BSS_TERMINAL_LOCATION  DROP PARTITION PART_20161005

select * from RP_BR_PIGEON_NESTVIDEO;
select * from RP_BR_NESTPAD_SUBPROC;

select * from RP_BR_REALLOCATION;


