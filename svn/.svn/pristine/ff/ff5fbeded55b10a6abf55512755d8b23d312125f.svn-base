create table MB_BSS_TERMINAL_LOCATION 
(
DAY           TIMESTAMP(6),
IMEI          VARCHAR2(18),
LONGITUDE     VARCHAR2(32),
LATITUDE      VARCHAR2(32),
LOCATION_TYPE VARCHAR2(2),
ELECTRICITY   VARCHAR2(8),
STATUS        VARCHAR2(2),
ACTIVE_TIME   TIMESTAMP(6),
LBS_LAC       NUMBER,
LBS_CELL      NUMBER,
LBS_RXLEV     NUMBER
)
PARTITION BY RANGE (DAY)
(
   PARTITION PART_YYYYMMDD values less than (TIMESTAMP' YYYY-MM-DD 00:00:00')
   tablespace $DB_TABLE_SPACE
   pctfree 10
   initrans 1
   maxtrans 255
   storage
    (
		initial 8M
  		next 1M
  		minextents 1
  	 	maxextents unlimited
	)
);