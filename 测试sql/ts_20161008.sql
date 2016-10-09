select * from MB_BSS_TERMINAL_LOCATION
select * from MB_BSS_TERMINAL_LOCATION partition(PART_20161008);
create table MB_BSS_TERMINAL_LOCATION_LZX as select * from MB_BSS_TERMINAL_LOCATION;

select table_name, partition_name from user_tab_partitions where table_name='MB_BSS_TERMINAL_LOCATION';

ALTER TABLE MB_BSS_TERMINAL_LOCATION  DROP PARTITION PART_20161005
