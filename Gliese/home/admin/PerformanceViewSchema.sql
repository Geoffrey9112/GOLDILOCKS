--###################################################################################
--# build performance views of PERFORMANCE_VIEW_SCHEMA
--###################################################################################

--##############################################################
--# SYS AUTHORIZATION
--##############################################################

SET SESSION AUTHORIZATION SYS;


--##############################################################
--# V$ARCHIVELOG
--##############################################################

--#####################
--# drop view
--#####################

DROP VIEW IF EXISTS PERFORMANCE_VIEW_SCHEMA.V$ARCHIVELOG;

--#####################
--# create view
--#####################

CREATE VIEW PERFORMANCE_VIEW_SCHEMA.V$ARCHIVELOG
(  
       ARCHIVELOG_MODE
     , LAST_ARCHIVED_LOG
     , ARCHIVELOG_DIR
     , ARCHIVELOG_FILE_PREFIX
)
AS 
SELECT 
       CAST( arclog.ARCHIVELOG_MODE AS VARCHAR(32 OCTETS) )        -- ARCHIVELOG_MODE
     , CAST( arclog.LAST_INACTIVATED_LOGFILE_SEQ_NO AS NUMBER )    -- LAST_ARCHIVED_LOG
     , CAST( arclog.ARCHIVELOG_DIR_1 AS VARCHAR(1024 OCTETS) )     -- ARCHIVELOG_DIR
     , CAST( arclog.ARCHIVELOG_FILE AS VARCHAR(128 OCTETS) )       -- ARCHIVELOG_FILE_PREFIX
  FROM 
       FIXED_TABLE_SCHEMA.X$ARCHIVELOG AS arclog
;
             
--#####################
--# comment view
--#####################

COMMENT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$ARCHIVELOG
        IS 'The V$ARCHIVELOG displays information of log archiving';

--#####################
--# comment column
--#####################

COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$ARCHIVELOG.ARCHIVELOG_MODE
        IS 'database log mode: the value in ( NOARCHIVELOG, ARCHIVELOG )';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$ARCHIVELOG.LAST_ARCHIVED_LOG
        IS 'sequence number of last archived log file';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$ARCHIVELOG.ARCHIVELOG_DIR
        IS 'archive destination path';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$ARCHIVELOG.ARCHIVELOG_FILE_PREFIX
        IS 'file prefix name of the archived log';

--#####################
--# grant view
--#####################

GRANT SELECT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$ARCHIVELOG TO PUBLIC;

COMMIT;



--##############################################################
--# V$BACKUP
--##############################################################

--#####################
--# drop view
--#####################

DROP VIEW IF EXISTS PERFORMANCE_VIEW_SCHEMA.V$BACKUP;

--#####################
--# create view
--#####################

CREATE VIEW PERFORMANCE_VIEW_SCHEMA.V$BACKUP
(  
       TBS_NAME
     , BACKUP_STATUS
     , BACKUP_LSN
)
AS 
SELECT 
       CAST( backup.NAME AS VARCHAR(128 OCTETS) ) AS TBS_NAME         -- TBS_NAME
     , CAST( backup.STATUS AS VARCHAR(16 OCTETS) ) AS BACKUP_STATUS   -- BACKUP_STATUS
     , CAST( datafile.BACKUP_LSN AS NUMBER )                          -- BACKUP_LSN
  FROM 
       FIXED_TABLE_SCHEMA.X$BACKUP AS backup
       LEFT OUTER JOIN
       ( SELECT TABLESPACE_ID, MIN(CHECKPOINT_LSN) AS BACKUP_LSN
           FROM FIXED_TABLE_SCHEMA.X$DATAFILE
         GROUP BY TABLESPACE_ID ) AS datafile
       ON backup.ID = datafile.TABLESPACE_ID
;
             
--#####################
--# comment view
--#####################

COMMENT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$BACKUP
        IS 'The V$BACKUP displays information of backup';

--#####################
--# comment column
--#####################

COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$BACKUP.TBS_NAME
        IS 'tablespace name';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$BACKUP.BACKUP_STATUS
        IS 'indicates whether the tablespace begin backup ( ACTIVE ) or not ( INACTIVE )';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$BACKUP.BACKUP_LSN
        IS 'the last checkpoint lsn of tablespace when backup started';

--#####################
--# grant view
--#####################

GRANT SELECT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$BACKUP TO PUBLIC;

COMMIT;



--##############################################################
--# V$COLUMNS
--##############################################################

--#####################
--# drop view
--#####################

DROP VIEW IF EXISTS PERFORMANCE_VIEW_SCHEMA.V$COLUMNS;

--#####################
--# create view
--#####################

CREATE VIEW PERFORMANCE_VIEW_SCHEMA.V$COLUMNS 
( 
       TABLE_OWNER
     , TABLE_SCHEMA
     , TABLE_NAME
     , COLUMN_NAME
     , ORDINAL_POSITION
     , DATA_TYPE
     , DATA_PRECISION
     , DATA_SCALE
     , COMMENTS
)
AS 
SELECT 
       auth.AUTHORIZATION_NAME                          -- TABLE_OWNER
     , sch.SCHEMA_NAME                                  -- TABLE_SCHEMA
     , tab.TABLE_NAME                                   -- TABLE_NAME
     , col.COLUMN_NAME                                  -- COLUMN_NAME
     , CAST( col.LOGICAL_ORDINAL_POSITION AS NUMBER )   -- ORDINAL_POSITION
     , dtd.DECLARED_DATA_TYPE                           -- DATA_TYPE
     , CAST( dtd.DECLARED_NUMERIC_PRECISION AS NUMBER ) -- DATA_PRECISION
     , CAST( dtd.DECLARED_NUMERIC_SCALE AS NUMBER )     -- DATA_SCALE
     , col.COMMENTS                                     -- COMMENTS
  FROM 
       DEFINITION_SCHEMA.COLUMNS               AS col 
     , DEFINITION_SCHEMA.DATA_TYPE_DESCRIPTOR  AS dtd 
     , DEFINITION_SCHEMA.TABLES                AS tab 
     , DEFINITION_SCHEMA.SCHEMATA              AS sch 
     , DEFINITION_SCHEMA.AUTHORIZATIONS        AS auth 
 WHERE  
       sch.SCHEMA_NAME = 'PERFORMANCE_VIEW_SCHEMA'
   AND col.DTD_IDENTIFIER = dtd.DTD_IDENTIFIER 
   AND col.TABLE_ID       = tab.TABLE_ID 
   AND col.SCHEMA_ID      = sch.SCHEMA_ID 
   AND col.OWNER_ID       = auth.AUTH_ID 
ORDER BY 
      col.SCHEMA_ID 
    , col.TABLE_ID 
    , col.PHYSICAL_ORDINAL_POSITION
;

--#####################
--# comment view
--#####################

COMMENT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$COLUMNS 
        IS 'The V$COLUMNS has one row for each column of all the performance views (views beginning with V$).';

--#####################
--# comment column
--#####################

COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$COLUMNS.TABLE_OWNER
        IS 'owner name who owns the performance view';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$COLUMNS.TABLE_SCHEMA
        IS 'schema name of the performance view';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$COLUMNS.TABLE_NAME
        IS 'name of the performance view';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$COLUMNS.COLUMN_NAME
        IS 'column name';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$COLUMNS.ORDINAL_POSITION
        IS 'the ordinal position (> 0) of the column in the performance view';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$COLUMNS.DATA_TYPE
        IS 'the data type name that a user declared';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$COLUMNS.DATA_PRECISION
        IS 'the precision value that a user declared';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$COLUMNS.DATA_SCALE
        IS 'the scale value that a user declared';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$COLUMNS.COMMENTS
        IS 'comments of the column';

--#####################
--# grant view
--#####################

GRANT SELECT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$COLUMNS TO PUBLIC;

COMMIT;


--##############################################################
--# V$DATAFILE
--##############################################################

--#####################
--# drop view
--#####################

DROP VIEW IF EXISTS PERFORMANCE_VIEW_SCHEMA.V$DATAFILE;

--#####################
--# create view
--#####################

CREATE VIEW PERFORMANCE_VIEW_SCHEMA.V$DATAFILE
(  
       TBS_NAME
     , DATAFILE_NAME
     , CHECKPOINT_LSN
     , CREATION_TIME
     , FILE_SIZE
)
AS 
SELECT 
       ( SELECT CAST( NAME AS VARCHAR(128 OCTETS) )
           FROM FIXED_TABLE_SCHEMA.X$TABLESPACE
           WHERE ID = datafile.TABLESPACE_ID )            -- TBS_NAME
     , CAST( datafile.PATH AS VARCHAR(1024 OCTETS) )      -- DATAFILE_NAME
     , CAST( datafile.CHECKPOINT_LSN AS NUMBER )          -- CHECKPOINT_LSN
     , datafile.CREATION_TIME                             -- CREATION_TIME
     , CAST( datafile.SIZE AS NUMBER )                    -- FILE_SIZE
  FROM 
       FIXED_TABLE_SCHEMA.X$DATAFILE AS datafile
  WHERE datafile.STATE = 'CREATED'
;
             
--#####################
--# comment view
--#####################

COMMENT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$DATAFILE
        IS 'The V$DATAFILE displays information of all datafiles.';

--#####################
--# comment column
--#####################

COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$DATAFILE.TBS_NAME
        IS 'tablepsace name';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$DATAFILE.DATAFILE_NAME
        IS 'datafile name ( absolute path )';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$DATAFILE.CHECKPOINT_LSN
        IS 'LSN at last checkpoint ( null if temporary tablespace )';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$DATAFILE.CREATION_TIME
        IS 'timestamp of the datafile creation';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$DATAFILE.FILE_SIZE
        IS 'datafile size ( in bytes )';

--#####################
--# grant view
--#####################

GRANT SELECT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$DATAFILE TO PUBLIC;

COMMIT;



--##############################################################
--# V$ERROR_CODE
--##############################################################

--#####################
--# drop view
--#####################

DROP VIEW IF EXISTS PERFORMANCE_VIEW_SCHEMA.V$ERROR_CODE;

--#####################
--# create view
--#####################

CREATE VIEW PERFORMANCE_VIEW_SCHEMA.V$ERROR_CODE 
(  
       ERROR_CODE
     , SQL_STATE
     , ERROR_MESSAGE
)
AS 
SELECT 
       CAST( xerr.ERROR_CODE AS NUMBER )             -- ERROR_CODE
     , CAST( xerr.SQL_STATE AS VARCHAR(32 OCTETS) )  -- SQL_STATE
     , CAST( xerr.MESSAGE AS VARCHAR(1024 OCTETS) )  -- ERROR_MESSAGE
  FROM 
       FIXED_TABLE_SCHEMA.X$ERROR_CODE AS xerr
;
             
--#####################
--# comment view
--#####################

COMMENT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$ERROR_CODE 
        IS 'The V$ERROR_CODE displays a list of all GOLDILOCKS error codes';

--#####################
--# comment column
--#####################

COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$ERROR_CODE.ERROR_CODE
        IS 'GOLDILOCKS error code';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$ERROR_CODE.SQL_STATE
        IS 'standard SQLSTATE code';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$ERROR_CODE.ERROR_MESSAGE
        IS 'error message';

--#####################
--# grant view
--#####################

GRANT SELECT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$ERROR_CODE TO PUBLIC;

COMMIT;


--##############################################################
--# V$GLOBAL_TRANSACTION
--##############################################################

--#####################
--# drop view
--#####################

DROP VIEW IF EXISTS PERFORMANCE_VIEW_SCHEMA.V$GLOBAL_TRANSACTION;

--#####################
--# create view
--#####################

CREATE VIEW PERFORMANCE_VIEW_SCHEMA.V$GLOBAL_TRANSACTION
(  
       GLOBAL_TRANS_ID
     , LOCAL_TRANS_ID
     , GLOBAL_TRANS_STATE
     , ASSO_STATE
     , START_TIME
     , IS_REPREPARABLE
)
AS 
SELECT 
       CAST( gltrans.GLOBAL_TRANS_ID AS VARCHAR(1024 OCTETS) )     -- GLOBAL_TRANS_ID
     , CAST( gltrans.LOCAL_TRANS_ID AS NUMBER )                    -- LOCAL_TRANS_ID
     , CAST( gltrans.STATE AS VARCHAR(32 OCTETS) )                 -- GLOBAL_TRANS_STATE
     , CAST( gltrans.ASSO_STATE AS VARCHAR(32 OCTETS) )            -- ASSO_STATE
     , gltrans.START_TIME                                          -- START_TIME
     , gltrans.REPREPARABLE                                        -- IS_REPREPARABLE
  FROM 
       FIXED_TABLE_SCHEMA.X$GLOBAL_TRANSACTION AS gltrans   
;
             
--#####################
--# comment view
--#####################

COMMENT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$GLOBAL_TRANSACTION
        IS 'The V$GLOBAL_TRANSACTION displays information on the currently active global transactions.';

--#####################
--# comment column
--#####################

COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$GLOBAL_TRANSACTION.GLOBAL_TRANS_ID
        IS 'global transaction identifier';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$GLOBAL_TRANSACTION.LOCAL_TRANS_ID
        IS 'local transaction identifier';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$GLOBAL_TRANSACTION.GLOBAL_TRANS_STATE
        IS 'state of the global transaction: the value in ( NOTR, ACTIVE, IDLE, PREPARED, ROLLBACK_ONLY, HEURISTIC_COMPLETED )';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$GLOBAL_TRANSACTION.ASSO_STATE
        IS 'associate state of the global transaction: the value in ( NOT_ASSOCIATED, ASSOCIATED, ASSOCIATION_SUSPENDED )';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$GLOBAL_TRANSACTION.START_TIME
        IS 'global transaction start time';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$GLOBAL_TRANSACTION.IS_REPREPARABLE
        IS 'indicates whether the global transaction is repreparable';

--#####################
--# grant view
--#####################

GRANT SELECT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$GLOBAL_TRANSACTION TO PUBLIC;

COMMIT;


--##############################################################
--# V$INCREMENTAL_BACKUP
--##############################################################

--#####################
--# drop view
--#####################

DROP VIEW IF EXISTS PERFORMANCE_VIEW_SCHEMA.V$INCREMENTAL_BACKUP;

--#####################
--# create view
--#####################

CREATE VIEW PERFORMANCE_VIEW_SCHEMA.V$INCREMENTAL_BACKUP
(  
       BACKUP_NAME
     , BACKUP_SCOPE
     , INCREMENTAL_LEVEL
     , INCREMENTAL_TYPE
     , LSN
     , BEGIN_TIME
     , COMPLETION_TIME
)
AS 
SELECT 
       CAST( backup.BACKUP_FILE AS VARCHAR(1024 OCTETS) )     -- BACKUP_NAME
     , CAST( backup.BACKUP_OBJECT AS VARCHAR(128 OCTETS) )    -- BACKUP_SCOPE
     , CAST( backup.BACKUP_LEVEL AS NUMBER )                  -- INCREMENTAL_LEVEL
     , CAST( backup.BACKUP_OPTION AS VARCHAR(32 OCTETS) )     -- INCREMENTAL_TYPE
     , CAST( backup.BACKUP_LSN AS NUMBER )                    -- LSN
     , BEGIN_TIME                                             -- BEGIN_TIME
     , COMPLETION_TIME                                        -- COMPLETION_TIME
  FROM 
       FIXED_TABLE_SCHEMA.X$CONTROLFILE_BACKUP_SECTION AS backup
       LEFT OUTER JOIN
       FIXED_TABLE_SCHEMA.X$CONTROLFILE AS ctrlfile
       ON backup.CONTROLFILE_NAME = ctrlfile.CONTROLFILE_NAME
  WHERE
       ctrlfile.IS_PRIMARY = TRUE
;
             
--#####################
--# comment view
--#####################

COMMENT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$INCREMENTAL_BACKUP
        IS 'The V$INCREMENTAL_BACKUP displays information about control files and datafiles in backup sets from the control file.';

--#####################
--# comment column
--#####################

COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$INCREMENTAL_BACKUP.BACKUP_NAME
        IS 'backup file name ( absolute path )';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$INCREMENTAL_BACKUP.BACKUP_SCOPE
        IS 'incremental backup scope: the value in ( database, tablespace, control )';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$INCREMENTAL_BACKUP.INCREMENTAL_LEVEL
        IS 'incremental backup level: the value in ( 0, 1, 2, 3, 4 )';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$INCREMENTAL_BACKUP.INCREMENTAL_TYPE
        IS 'incremental backup type: the value in ( DIFFERENTIAL, CUMULATIVE )';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$INCREMENTAL_BACKUP.LSN
        IS 'all changes up to checkpoint LSN are included in this backup';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$INCREMENTAL_BACKUP.BEGIN_TIME
        IS 'backup beginning time';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$INCREMENTAL_BACKUP.COMPLETION_TIME
        IS 'backup completion time';

--#####################
--# grant view
--#####################

GRANT SELECT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$INCREMENTAL_BACKUP TO PUBLIC;

COMMIT;


--##############################################################
--# V$KEYWORDS
--##############################################################

--#####################
--# drop view
--#####################

DROP VIEW IF EXISTS PERFORMANCE_VIEW_SCHEMA.V$KEYWORDS;

--#####################
--# create view
--#####################

CREATE VIEW PERFORMANCE_VIEW_SCHEMA.V$KEYWORDS 
(  
       KEYWORD_NAME
     , KEYWORD_LENGTH
     , IS_RESERVED
)
AS 
SELECT 
       CAST( key.NAME AS VARCHAR(128 OCTETS) )   -- KEYWORD_NAME
     , CAST( LENGTH(key.NAME) AS NUMBER )        -- KEYWORD_LENGTH
     , CASE key.CATEGORY 
            WHEN 0 THEN TRUE
                   ELSE FALSE
       END                                       -- IS_RESERVED
  FROM 
       FIXED_TABLE_SCHEMA.X$SQL_KEYWORDS key
;
             
--#####################
--# comment view
--#####################

COMMENT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$KEYWORDS 
        IS 'The V$KEYWORDS displays a list of all SQL keywords';

--#####################
--# comment column
--#####################

COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$KEYWORDS.KEYWORD_NAME
        IS 'name of keyword';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$KEYWORDS.KEYWORD_LENGTH
        IS 'length of the keyword';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$KEYWORDS.IS_RESERVED
        IS 'indicates whether the keyword cannot be used as an identifier (TRUE) or whether the keyword is not reserved (FALSE)';

--#####################
--# grant view
--#####################

GRANT SELECT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$KEYWORDS TO PUBLIC;

COMMIT;

--##############################################################
--# V$LATCH
--##############################################################

--#####################
--# drop view
--#####################

DROP VIEW IF EXISTS PERFORMANCE_VIEW_SCHEMA.V$LATCH;

--#####################
--# create view
--#####################

CREATE VIEW PERFORMANCE_VIEW_SCHEMA.V$LATCH
(  
       LATCH_DESCRIPTION
     , REF_COUNT
     , SPIN_LOCK
     , WAIT_COUNT
     , CURRENT_MODE
)
AS 
SELECT 
       CAST( latch.DESCRIPTION AS VARCHAR(64 OCTETS) )     -- LATCH_DESCRIPTION
     , CAST( latch.REF_COUNT AS NUMBER )                   -- REF_COUNT
     , CAST( CASE WHEN latch.SPIN_LOCK = 1
                       THEN 'YES'
                       ELSE 'NO'
                       END AS VARCHAR(3 OCTETS) )          -- SPIN_LOCK
     , CAST( latch.WAIT_COUNT AS NUMBER )                  -- WAIT_COUNT
     , CAST( latch.CURRENT_MODE AS VARCHAR(32 OCTETS) )    -- CURRENT_MODE
  FROM 
       FIXED_TABLE_SCHEMA.X$LATCH AS latch
;
             
--#####################
--# comment view
--#####################

COMMENT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$LATCH
        IS 'The V$LATCH shows latch information.';

--#####################
--# comment column
--#####################

COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$LATCH.LATCH_DESCRIPTION
        IS 'latch description';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$LATCH.REF_COUNT
        IS 'reference count';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$LATCH.SPIN_LOCK
        IS 'indicates whether the spin lock is locked ( YES ) or not ( NO )';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$LATCH.WAIT_COUNT
        IS 'wait count';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$LATCH.CURRENT_MODE
        IS 'current latch mode: the value in ( INITIAL, SHARED, EXCLUSIVE )';

--#####################
--# grant view
--#####################

GRANT SELECT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$LATCH TO PUBLIC;

COMMIT;


--##############################################################
--# V$LOGFILE
--##############################################################

--#####################
--# drop view
--#####################

DROP VIEW IF EXISTS PERFORMANCE_VIEW_SCHEMA.V$LOGFILE;

--#####################
--# create view
--#####################

CREATE VIEW PERFORMANCE_VIEW_SCHEMA.V$LOGFILE
(  
       GROUP_ID
     , FILE_NAME
     , GROUP_STATE
     , FILE_SEQ
     , FILE_SIZE
)
AS 
SELECT 
       CAST( loggrp.GROUP_ID AS NUMBER )              -- GROUP_ID
     , CAST( logmem.NAME AS VARCHAR(1024 OCTETS) )    -- FILE_NAME
     , CAST( loggrp.STATE AS VARCHAR(32 OCTETS) )     -- GROUP_STATE
     , CAST( loggrp.FILE_SEQ_NO AS NUMBER )           -- FILE_SEQ
     , CAST( loggrp.FILE_SIZE AS NUMBER )             -- FILE_SIZE
  FROM 
       FIXED_TABLE_SCHEMA.X$LOG_GROUP AS loggrp
       LEFT OUTER JOIN   
       FIXED_TABLE_SCHEMA.X$LOG_MEMBER AS logmem
       ON loggrp.GROUP_ID = logmem.GROUP_ID
;
             
--#####################
--# comment view
--#####################

COMMENT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$LOGFILE
        IS 'The V$LOGFILE displays information of all redo log members.';

--#####################
--# comment column
--#####################

COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$LOGFILE.GROUP_ID
        IS 'redo log group identifier';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$LOGFILE.FILE_NAME
        IS 'name of the log member';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$LOGFILE.GROUP_STATE
        IS 'state of the log group: the value in ( UNUSED, ACTIVE, CURRENT, INACTIVE )';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$LOGFILE.FILE_SEQ
        IS 'file sequence number of the log member';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$LOGFILE.FILE_SIZE
        IS 'file size of the log member ( in bytes )';

--#####################
--# grant view
--#####################

GRANT SELECT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$LOGFILE TO PUBLIC;

COMMIT;




--##############################################################
--# V$LOCK_WAIT
--##############################################################

--#####################
--# drop view
--#####################

DROP VIEW IF EXISTS PERFORMANCE_VIEW_SCHEMA.V$LOCK_WAIT;

--#####################
--# create view
--#####################

CREATE VIEW PERFORMANCE_VIEW_SCHEMA.V$LOCK_WAIT
(  
       GRANT_TRANS_ID
     , REQUEST_TRANS_ID
)
AS 
SELECT 
       ( SELECT CAST( LOGICAL_TRANS_ID AS NUMBER )
           FROM FIXED_TABLE_SCHEMA.X$TRANSACTION
           WHERE SLOT_ID = lckwait.GRANTED_TRANSACTION_SLOT_ID )   -- GRANT_TRANS_ID
     , ( SELECT CAST( LOGICAL_TRANS_ID AS NUMBER )
           FROM FIXED_TABLE_SCHEMA.X$TRANSACTION
           WHERE SLOT_ID = lckwait.REQUEST_TRANSACTION_SLOT_ID )   -- REQUEST_TRANS_ID
  FROM 
       FIXED_TABLE_SCHEMA.X$LOCK_WAIT AS lckwait
;
             
--#####################
--# comment view
--#####################

COMMENT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$LOCK_WAIT
        IS 'This view lists the locks currently held and outstanding requests for a lock.';

--#####################
--# comment column
--#####################

COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$LOCK_WAIT.GRANT_TRANS_ID
        IS 'transaction identifier that holds the lock';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$LOCK_WAIT.REQUEST_TRANS_ID
        IS 'transaction identifier that requests the lock';

--#####################
--# grant view
--#####################

GRANT SELECT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$LOCK_WAIT TO PUBLIC;

COMMIT;


--##############################################################
--# V$PROPERTY
--##############################################################

--#####################
--# drop view
--#####################

DROP VIEW IF EXISTS PERFORMANCE_VIEW_SCHEMA.V$PROPERTY;

--#####################
--# create view
--#####################

CREATE VIEW PERFORMANCE_VIEW_SCHEMA.V$PROPERTY 
( 
       PROPERTY_NAME
     , DESCRIPTION
     , DATA_TYPE
     , STARTUP_PHASE
     , VALUE_UNIT
     , PROPERTY_VALUE
     , PROPERTY_SOURCE
     , INIT_VALUE
     , INIT_SOURCE
     , MIN_VALUE
     , MAX_VALUE
     , SES_MODIFIABLE
     , SYS_MODIFIABLE
     , IS_MODIFIABLE
)
AS 
SELECT 
       CAST( PROPERTY_NAME AS VARCHAR(128 OCTETS) )  -- PROPERTY_NAME
     , CAST( DESCRIPTION AS VARCHAR(2048 OCTETS) )   -- DESCRIPTION
     , CAST( DATA_TYPE AS VARCHAR(32 OCTETS) )       -- DATA_TYPE
     , CAST( STARTUP_PHASE AS VARCHAR(32 OCTETS) )   -- STARTUP_PHASE
     , CAST( UNIT AS VARCHAR(32 OCTETS) )            -- VALUE_UNIT
     , CAST( VALUE AS VARCHAR(2048 OCTETS) )         -- PROPERTY_VALUE
     , CAST( SOURCE AS VARCHAR(32 OCTETS) )          -- PROPERTY_SOURCE
     , CAST( INIT_VALUE AS VARCHAR(2048 OCTETS) )    -- INIT_VALUE
     , CAST( INIT_SOURCE AS VARCHAR(32 OCTETS) )     -- INIT_SOURCE
     , CAST( CASE WHEN DATA_TYPE = 'VARCHAR' THEN NULL
                                             ELSE MIN 
             END
             AS NUMBER )                             -- MIN_VALUE
     , CAST( CASE WHEN DATA_TYPE = 'VARCHAR' THEN NULL
                                             ELSE MAX 
             END
             AS NUMBER )                             -- MAX_VALUE
     , CAST( SES_MODIFIABLE AS VARCHAR(32 OCTETS) )  -- SES_MODIFIABLE
     , CAST( SYS_MODIFIABLE AS VARCHAR(32 OCTETS) )  -- SYS_MODIFIABLE
     , CAST( MODIFIABLE AS VARCHAR(32 OCTETS) )      -- IS_MODIFIABLE
  FROM 
       FIXED_TABLE_SCHEMA.X$PROPERTY
 WHERE 
       DOMAIN = 'EXTERNAL'
ORDER BY 
      PROPERTY_ID
;

--#####################
--# comment view
--#####################

COMMENT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$PROPERTY  
        IS 'The V$PROPERTY displays a list of all Properties at current session. otherwise, the instance-wide value';

--#####################
--# comment column
--#####################

COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$PROPERTY.PROPERTY_NAME
        IS 'name of the property';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$PROPERTY.DESCRIPTION
        IS 'description of the property';        
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$PROPERTY.DATA_TYPE
        IS 'data type of the property';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$PROPERTY.STARTUP_PHASE
        IS 'modifiable startup-phase: the value IN ( NO MOUNT / MOUNT / OPEN & [BELOW|ABOVE] )';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$PROPERTY.VALUE_UNIT
        IS 'unit of the property value: the value in ( NONE, BYTE, MS(milisec) )';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$PROPERTY.PROPERTY_VALUE
        IS 'property value for the session. otherwise, the instance-wide value';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$PROPERTY.PROPERTY_SOURCE
        IS 'source of the current property value: the value IN ( USER, DEFAULT, ENV_VAR, BINARY_FILE, FILE, SYSTEM )';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$PROPERTY.INIT_VALUE
        IS 'property init value for the session';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$PROPERTY.INIT_SOURCE
        IS 'source of the current property INIT_VALUE: the value IN ( USER, DEFAULT, ENV_VAR, BINARY_FILE, FILE, SYSTEM )';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$PROPERTY.MIN_VALUE
        IS 'minimum value for property. null if type is varchar.';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$PROPERTY.MAX_VALUE
        IS 'maximum value for property. null if type is varchar.';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$PROPERTY.SES_MODIFIABLE
        IS 'property can be changed with ALTER SESSION or not: the value in ( TRUE, FALSE )';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$PROPERTY.SYS_MODIFIABLE
        IS 'property can be changed with ALTER SYSTEM and when the change takes effect: the value in ( NONE, FALSE, IMMEDIATE, DEFERRED )';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$PROPERTY.IS_MODIFIABLE
        IS 'property can be changed or not: the value in ( TRUE, FALSE )';

--#####################
--# grant view
--#####################

GRANT SELECT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$PROPERTY TO PUBLIC;

COMMIT;

--##############################################################
--# V$PROCESS_STAT
--##############################################################

--#####################
--# drop view
--#####################

DROP VIEW IF EXISTS PERFORMANCE_VIEW_SCHEMA.V$PROCESS_STAT;

--#####################
--# create view
--#####################

CREATE VIEW PERFORMANCE_VIEW_SCHEMA.V$PROCESS_STAT
(  
       STAT_NAME
     , PROC_ID
     , STAT_VALUE
)
AS 
SELECT 
       CAST( knstat.NAME AS VARCHAR(128 OCTETS) )       -- STAT_NAME
     , CAST( knstat.ID AS NUMBER )                      -- PROC_ID
     , CAST( knstat.VALUE AS NUMBER )                   -- STAT_VALUE
  FROM 
       FIXED_TABLE_SCHEMA.X$KN_PROC_STAT AS knstat   
  WHERE knstat.CATEGORY = 0
UNION ALL
SELECT 
       CAST( smenv.NAME AS VARCHAR(128 OCTETS) )        -- STAT_NAME
     , CAST( smenv.ID AS NUMBER )                       -- PROC_ID
     , CAST( smenv.VALUE AS NUMBER )                    -- STAT_VALUE
  FROM 
       FIXED_TABLE_SCHEMA.X$SM_PROC_ENV AS smenv   
  WHERE smenv.CATEGORY = 0
UNION ALL
SELECT 
       CAST( exeenv.NAME AS VARCHAR(128 OCTETS) )       -- STAT_NAME
     , CAST( exeenv.ID AS NUMBER )                      -- PROC_ID
     , CAST( exeenv.VALUE AS NUMBER )                   -- STAT_VALUE
  FROM 
       FIXED_TABLE_SCHEMA.X$EXE_PROC_ENV AS exeenv   
  WHERE exeenv.CATEGORY = 0
UNION ALL
SELECT 
       CAST( sqlenv.NAME AS VARCHAR(128 OCTETS) )       -- STAT_NAME
     , CAST( sqlenv.ID AS NUMBER )                      -- PROC_ID
     , CAST( sqlenv.VALUE AS NUMBER )                   -- STAT_VALUE
  FROM 
       FIXED_TABLE_SCHEMA.X$SQL_PROC_ENV AS sqlenv   
  WHERE sqlenv.CATEGORY = 0
ORDER BY 2, 1
;
             
--#####################
--# comment view
--#####################

COMMENT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$PROCESS_STAT
        IS 'The V$PROCESS_STAT displays goldilocks process statistics.';

--#####################
--# comment column
--#####################

COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$PROCESS_STAT.STAT_NAME
        IS 'statistic name';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$PROCESS_STAT.PROC_ID
        IS 'goldilocks process identifier';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$PROCESS_STAT.STAT_VALUE
        IS 'statistic value';

--#####################
--# grant view
--#####################

GRANT SELECT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$PROCESS_STAT TO PUBLIC;

COMMIT;



--##############################################################
--# V$PROCESS_MEM_STAT
--##############################################################

--#####################
--# drop view
--#####################

DROP VIEW IF EXISTS PERFORMANCE_VIEW_SCHEMA.V$PROCESS_MEM_STAT;

--#####################
--# create view
--#####################

CREATE VIEW PERFORMANCE_VIEW_SCHEMA.V$PROCESS_MEM_STAT
(  
       STAT_NAME
     , PROC_ID
     , STAT_VALUE
)
AS 
SELECT 
       CAST( knstat.NAME AS VARCHAR(128 OCTETS) )       -- STAT_NAME
     , CAST( knstat.ID AS NUMBER )                      -- PROC_ID
     , CAST( knstat.VALUE AS NUMBER )                   -- STAT_VALUE
  FROM 
       FIXED_TABLE_SCHEMA.X$KN_PROC_STAT AS knstat   
  WHERE knstat.CATEGORY = 11
UNION ALL
SELECT 
       CAST( smenv.NAME AS VARCHAR(128 OCTETS) )        -- STAT_NAME
     , CAST( smenv.ID AS NUMBER )                       -- PROC_ID
     , CAST( smenv.VALUE AS NUMBER )                    -- STAT_VALUE
  FROM 
       FIXED_TABLE_SCHEMA.X$SM_PROC_ENV AS smenv   
  WHERE smenv.CATEGORY = 11
UNION ALL
SELECT 
       CAST( exeenv.NAME AS VARCHAR(128 OCTETS) )       -- STAT_NAME
     , CAST( exeenv.ID AS NUMBER )                      -- PROC_ID
     , CAST( exeenv.VALUE AS NUMBER )                   -- STAT_VALUE
  FROM 
       FIXED_TABLE_SCHEMA.X$EXE_PROC_ENV AS exeenv   
  WHERE exeenv.CATEGORY = 11
UNION ALL
SELECT 
       CAST( sqlenv.NAME AS VARCHAR(128 OCTETS) )       -- STAT_NAME
     , CAST( sqlenv.ID AS NUMBER )                      -- PROC_ID
     , CAST( sqlenv.VALUE AS NUMBER )                   -- STAT_VALUE
  FROM 
       FIXED_TABLE_SCHEMA.X$SQL_PROC_ENV AS sqlenv   
  WHERE sqlenv.CATEGORY = 11
ORDER BY 2, 1
;
             
--#####################
--# comment view
--#####################

COMMENT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$PROCESS_MEM_STAT
        IS 'The V$PROCESS_MEM_STAT displays goldilocks process memory statistics.';

--#####################
--# comment column
--#####################

COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$PROCESS_MEM_STAT.STAT_NAME
        IS 'statistic name';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$PROCESS_MEM_STAT.PROC_ID
        IS 'goldilocks process identifier';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$PROCESS_MEM_STAT.STAT_VALUE
        IS 'statistic value';

--#####################
--# grant view
--#####################

GRANT SELECT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$PROCESS_MEM_STAT TO PUBLIC;

COMMIT;

--##############################################################
--# V$PROCESS_SQL_STAT
--##############################################################

--#####################
--# drop view
--#####################

DROP VIEW IF EXISTS PERFORMANCE_VIEW_SCHEMA.V$PROCESS_SQL_STAT;

--#####################
--# create view
--#####################

CREATE VIEW PERFORMANCE_VIEW_SCHEMA.V$PROCESS_SQL_STAT
(  
       STAT_NAME
     , PROC_ID
     , STAT_VALUE
)
AS 
SELECT 
       CAST( sqlprocenv.NAME AS VARCHAR(128 OCTETS) )                        -- STAT_NAME
     , CAST( sqlprocenv.ID AS NUMBER )                                       -- PROC_ID
     , CAST( sqlprocenv.VALUE AS NUMBER )                                    -- STAT_VALUE
  FROM 
       FIXED_TABLE_SCHEMA.X$SQL_PROC_ENV AS sqlprocenv
  WHERE sqlprocenv.CATEGORY = 20
UNION ALL
SELECT 
       CAST( 'COMMAND: ' || sqlprocexec.STMT_TYPE AS VARCHAR(128 OCTETS) )   -- STAT_NAME
     , CAST( sqlprocexec.ID AS NUMBER )                                      -- PROC_ID
     , CAST( sqlprocexec.EXECUTE_COUNT AS NUMBER )                           -- STAT_VALUE
  FROM 
       FIXED_TABLE_SCHEMA.X$SQL_PROC_STAT_EXEC_STMT AS sqlprocexec
ORDER BY 2, 1
;
             
--#####################
--# comment view
--#####################

COMMENT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$PROCESS_SQL_STAT
        IS 'The V$PROCESS_SQL_STAT displays goldilocks process SQL statistics.';

--#####################
--# comment column
--#####################

COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$PROCESS_SQL_STAT.STAT_NAME
        IS 'statistic name';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$PROCESS_SQL_STAT.PROC_ID
        IS 'goldilocks process identifier';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$PROCESS_SQL_STAT.STAT_VALUE
        IS 'statistic value';

--#####################
--# grant view
--#####################

GRANT SELECT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$PROCESS_SQL_STAT TO PUBLIC;

COMMIT;


--##############################################################
--# V$RESERVED_WORDS
--##############################################################

--#####################
--# drop view
--#####################

DROP VIEW IF EXISTS PERFORMANCE_VIEW_SCHEMA.V$RESERVED_WORDS;

--#####################
--# create view
--#####################

CREATE VIEW PERFORMANCE_VIEW_SCHEMA.V$RESERVED_WORDS 
(  
       KEYWORD_NAME
     , KEYWORD_LENGTH
)
AS 
SELECT 
       CAST( key.NAME AS VARCHAR(128 OCTETS) )   -- KEYWORD_NAME
     , CAST( LENGTH(key.NAME) AS NUMBER )        -- KEYWORD_LENGTH
  FROM 
       FIXED_TABLE_SCHEMA.X$SQL_KEYWORDS key
 WHERE key.CATEGORY = 0;
;
             
--#####################
--# comment view
--#####################

COMMENT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$RESERVED_WORDS 
        IS 'The V$RESERVED_WORDS displays a list of all SQL reserved keywords.  Reserved words cannot be used in table name or column name.';

--#####################
--# comment column
--#####################

COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$RESERVED_WORDS.KEYWORD_NAME
        IS 'name of keyword';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$RESERVED_WORDS.KEYWORD_LENGTH
        IS 'length of the keyword';

--#####################
--# grant view
--#####################

GRANT SELECT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$RESERVED_WORDS TO PUBLIC;

COMMIT;


--##############################################################
--# V$SESSION
--##############################################################

--#####################
--# drop view
--#####################

DROP VIEW IF EXISTS PERFORMANCE_VIEW_SCHEMA.V$SESSION;

--#####################
--# create view
--#####################

CREATE VIEW PERFORMANCE_VIEW_SCHEMA.V$SESSION
(  
       SESSION_ID
     , SERIAL_NO
     , TRANS_ID
     , CONNECTION_TYPE
     , USER_NAME
     , SESSION_STATUS
     , SERVER_TYPE
     , PROCESS_ID
     , LOGON_TIME
     , PROGRAM_NAME
     , CLIENT_ADDRESS
     , CLIENT_PORT
)
AS 
SELECT 
       CAST( xsess.ID AS NUMBER )                      -- SESSION_ID
     , CAST( xsess.SERIAL AS NUMBER )                  -- SERIAL_NO
     , CAST( xsess.TRANS_ID AS NUMBER )                -- TRANS_ID
     , CAST( xsess.CONNECTION AS VARCHAR(32 OCTETS) )  -- CONNECTION_TYPE
     , auth.AUTHORIZATION_NAME                         -- USER_NAME
     , CAST( xsess.STATUS AS VARCHAR(32 OCTETS) )      -- SESSION_STATUS
     , CAST( xsess.SERVER AS VARCHAR(32 OCTETS) )      -- SERVER_TYPE
     , CAST( xsess.CLIENT_PROCESS AS NUMBER )          -- PROCESS_ID
     , xsess.LOGON_TIME                                -- LOGON_TIME
     , CAST( xsess.PROGRAM AS VARCHAR(128 OCTETS) )    -- PROGRAM_NAME
     , CAST( xsess.ADDRESS AS VARCHAR(1024 OCTETS) )   -- CLIENT_ADDRESS
     , CAST( xsess.PORT AS NUMBER )                    -- CLIENT_PORT
  FROM 
       FIXED_TABLE_SCHEMA.X$SESSION AS xsess   
       LEFT OUTER JOIN   
       DEFINITION_SCHEMA.AUTHORIZATIONS AS auth
       ON xsess.USER_ID = auth.AUTH_ID
 WHERE 
       xsess.STATUS IN ( 'CONNECTED', 'SIGNALED', 'SNIPED', 'DEAD' ) 
;
             
--#####################
--# comment view
--#####################

COMMENT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$SESSION
        IS 'The V$SESSION displays session information for each current session.';

--#####################
--# comment column
--#####################

COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$SESSION.SESSION_ID
        IS 'session identifier';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$SESSION.SERIAL_NO
        IS 'session serial number';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$SESSION.TRANS_ID
        IS 'transaction identifier ( -1 if inactive transaction )';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$SESSION.CONNECTION_TYPE
        IS 'connection type: the value in ( DA, TCP ) ';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$SESSION.USER_NAME
        IS 'user name';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$SESSION.SESSION_STATUS
        IS 'status of the session: the value in ( CONNECTED, SIGNALED, SNIPED, DEAD )';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$SESSION.SERVER_TYPE
        IS 'server type: the value in ( DEDICATED, SHARED )';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$SESSION.PROCESS_ID
        IS 'client process identifier';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$SESSION.LOGON_TIME
        IS 'logon time';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$SESSION.PROGRAM_NAME
        IS 'program name';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$SESSION.CLIENT_ADDRESS
        IS 'client address ( null if DA )';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$SESSION.CLIENT_PORT
        IS 'client port ( 0 if DA )';

--#####################
--# grant view
--#####################

GRANT SELECT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$SESSION TO PUBLIC;

COMMIT;


--##############################################################
--# V$SESSION_STAT
--##############################################################

--#####################
--# drop view
--#####################

DROP VIEW IF EXISTS PERFORMANCE_VIEW_SCHEMA.V$SESSION_STAT;

--#####################
--# create view
--#####################

CREATE VIEW PERFORMANCE_VIEW_SCHEMA.V$SESSION_STAT
(  
       STAT_NAME
     , SESS_ID
     , STAT_VALUE
)
AS 
SELECT 
       CAST( knstat.NAME AS VARCHAR(128 OCTETS) )       -- STAT_NAME
     , CAST( knstat.ID AS NUMBER )                      -- SESS_ID
     , CAST( knstat.VALUE AS NUMBER )                   -- STAT_VALUE
  FROM 
       FIXED_TABLE_SCHEMA.X$KN_SESS_STAT AS knstat   
  WHERE knstat.CATEGORY = 0
UNION ALL
SELECT 
       CAST( smenv.NAME AS VARCHAR(128 OCTETS) )        -- STAT_NAME
     , CAST( smenv.ID AS NUMBER )                       -- SESS_ID
     , CAST( smenv.VALUE AS NUMBER )                    -- STAT_VALUE
  FROM 
       FIXED_TABLE_SCHEMA.X$SM_SESS_ENV AS smenv   
  WHERE smenv.CATEGORY = 0
UNION ALL
SELECT 
       CAST( smstat.NAME AS VARCHAR(128 OCTETS) )       -- STAT_NAME
     , CAST( smstat.ID AS NUMBER )                      -- SESS_ID
     , CAST( smstat.VALUE AS NUMBER )                   -- STAT_VALUE
  FROM 
       FIXED_TABLE_SCHEMA.X$SM_SESS_STAT AS smstat   
  WHERE smstat.CATEGORY = 0
UNION ALL
SELECT 
       CAST( exeenv.NAME AS VARCHAR(128 OCTETS) )       -- STAT_NAME
     , CAST( exeenv.ID AS NUMBER )                      -- SESS_ID
     , CAST( exeenv.VALUE AS NUMBER )                   -- STAT_VALUE
  FROM 
       FIXED_TABLE_SCHEMA.X$EXE_SESS_ENV AS exeenv   
  WHERE exeenv.CATEGORY = 0
UNION ALL
SELECT 
       CAST( sqlenv.NAME AS VARCHAR(128 OCTETS) )       -- STAT_NAME
     , CAST( sqlenv.ID AS NUMBER )                      -- SESS_ID
     , CAST( sqlenv.VALUE AS NUMBER )                   -- STAT_VALUE
  FROM 
       FIXED_TABLE_SCHEMA.X$SQL_SESS_ENV AS sqlenv   
  WHERE sqlenv.CATEGORY = 0
UNION ALL
SELECT 
       CAST( ssstat.NAME AS VARCHAR(128 OCTETS) )       -- STAT_NAME
     , CAST( ssstat.ID AS NUMBER )                      -- SESS_ID
     , CAST( ssstat.VALUE AS NUMBER )                   -- STAT_VALUE
  FROM 
       FIXED_TABLE_SCHEMA.X$SS_SESS_STAT AS ssstat   
  WHERE ssstat.CATEGORY = 0
ORDER BY 2, 1
;
             
--#####################
--# comment view
--#####################

COMMENT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$SESSION_STAT
        IS 'The V$SESSION_STAT displays session statistics.';

--#####################
--# comment column
--#####################

COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$SESSION_STAT.STAT_NAME
        IS 'statistic name';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$SESSION_STAT.SESS_ID
        IS 'session identifier';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$SESSION_STAT.STAT_VALUE
        IS 'statistic value';

--#####################
--# grant view
--#####################

GRANT SELECT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$SESSION_STAT TO PUBLIC;

COMMIT;


--##############################################################
--# V$SESSION_MEM_STAT
--##############################################################

--#####################
--# drop view
--#####################

DROP VIEW IF EXISTS PERFORMANCE_VIEW_SCHEMA.V$SESSION_MEM_STAT;

--#####################
--# create view
--#####################

CREATE VIEW PERFORMANCE_VIEW_SCHEMA.V$SESSION_MEM_STAT
(  
       STAT_NAME
     , SESS_ID
     , STAT_VALUE
)
AS 
SELECT 
       CAST( knstat.NAME AS VARCHAR(128 OCTETS) )       -- STAT_NAME
     , CAST( knstat.ID AS NUMBER )                      -- SESS_ID
     , CAST( knstat.VALUE AS NUMBER )                   -- STAT_VALUE
  FROM 
       FIXED_TABLE_SCHEMA.X$KN_SESS_STAT AS knstat   
  WHERE knstat.CATEGORY = 11
UNION ALL
SELECT 
       CAST( smenv.NAME AS VARCHAR(128 OCTETS) )        -- STAT_NAME
     , CAST( smenv.ID AS NUMBER )                       -- SESS_ID
     , CAST( smenv.VALUE AS NUMBER )                    -- STAT_VALUE
  FROM 
       FIXED_TABLE_SCHEMA.X$SM_SESS_ENV AS smenv   
  WHERE smenv.CATEGORY = 11
UNION ALL
SELECT 
       CAST( smstat.NAME AS VARCHAR(128 OCTETS) )       -- STAT_NAME
     , CAST( smstat.ID AS NUMBER )                      -- SESS_ID
     , CAST( smstat.VALUE AS NUMBER )                   -- STAT_VALUE
  FROM 
       FIXED_TABLE_SCHEMA.X$SM_SESS_STAT AS smstat   
  WHERE smstat.CATEGORY = 11
UNION ALL
SELECT 
       CAST( exeenv.NAME AS VARCHAR(128 OCTETS) )       -- STAT_NAME
     , CAST( exeenv.ID AS NUMBER )                      -- SESS_ID
     , CAST( exeenv.VALUE AS NUMBER )                   -- STAT_VALUE
  FROM 
       FIXED_TABLE_SCHEMA.X$EXE_SESS_ENV AS exeenv   
  WHERE exeenv.CATEGORY = 11
UNION ALL
SELECT 
       CAST( sqlenv.NAME AS VARCHAR(128 OCTETS) )       -- STAT_NAME
     , CAST( sqlenv.ID AS NUMBER )                      -- SESS_ID
     , CAST( sqlenv.VALUE AS NUMBER )                   -- STAT_VALUE
  FROM 
       FIXED_TABLE_SCHEMA.X$SQL_SESS_ENV AS sqlenv   
  WHERE sqlenv.CATEGORY = 11
UNION ALL
SELECT 
       CAST( ssstat.NAME AS VARCHAR(128 OCTETS) )       -- STAT_NAME
     , CAST( ssstat.ID AS NUMBER )                      -- SESS_ID
     , CAST( ssstat.VALUE AS NUMBER )                   -- STAT_VALUE
  FROM 
       FIXED_TABLE_SCHEMA.X$SS_SESS_STAT AS ssstat   
  WHERE ssstat.CATEGORY = 11
UNION ALL
SELECT 
       CAST( ssstmtstat.NAME AS VARCHAR(128 OCTETS) )   -- STAT_NAME
     , CAST( ssstmtstat.SESSION_ID AS NUMBER )          -- SESS_ID
     , CAST( SUM( ssstmtstat.VALUE ) AS NUMBER )        -- STAT_VALUE
  FROM 
       FIXED_TABLE_SCHEMA.X$SS_STMT_STAT AS ssstmtstat   
  WHERE ssstmtstat.CATEGORY = 11
  GROUP BY NAME, SESSION_ID
ORDER BY 2, 1
;
             
--#####################
--# comment view
--#####################

COMMENT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$SESSION_MEM_STAT
        IS 'The V$SESSION_MEM_STAT displays session memory statistics.';

--#####################
--# comment column
--#####################

COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$SESSION_MEM_STAT.STAT_NAME
        IS 'statistic name';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$SESSION_MEM_STAT.SESS_ID
        IS 'session identifier';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$SESSION_MEM_STAT.STAT_VALUE
        IS 'statistic value';

--#####################
--# grant view
--#####################

GRANT SELECT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$SESSION_MEM_STAT TO PUBLIC;

COMMIT;


--##############################################################
--# V$SESSION_SQL_STAT
--##############################################################

--#####################
--# drop view
--#####################

DROP VIEW IF EXISTS PERFORMANCE_VIEW_SCHEMA.V$SESSION_SQL_STAT;

--#####################
--# create view
--#####################

CREATE VIEW PERFORMANCE_VIEW_SCHEMA.V$SESSION_SQL_STAT
(  
       STAT_NAME
     , SESS_ID
     , STAT_VALUE
)
AS 
SELECT 
       CAST( sqlsessenv.NAME AS VARCHAR(128 OCTETS) )                        -- STAT_NAME
     , CAST( sqlsessenv.ID AS NUMBER )                                       -- SESS_ID
     , CAST( sqlsessenv.VALUE AS NUMBER )                                    -- STAT_VALUE
  FROM 
       FIXED_TABLE_SCHEMA.X$SQL_SESS_ENV AS sqlsessenv
  WHERE sqlsessenv.CATEGORY = 20
UNION ALL
SELECT 
       CAST( 'COMMAND: ' || sqlprocexec.STMT_TYPE AS VARCHAR(128 OCTETS) )   -- STAT_NAME
     , CAST( sqlprocexec.ID AS NUMBER )                                      -- SESS_ID
     , CAST( sqlprocexec.EXECUTE_COUNT AS NUMBER )                           -- STAT_VALUE
  FROM 
       FIXED_TABLE_SCHEMA.X$SQL_SESS_STAT_EXEC_STMT AS sqlprocexec
ORDER BY 2, 1
;
             
--#####################
--# comment view
--#####################

COMMENT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$SESSION_SQL_STAT
        IS 'The V$SESSION_SQL_STAT displays session SQL statistics.';

--#####################
--# comment column
--#####################

COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$SESSION_SQL_STAT.STAT_NAME
        IS 'statistic name';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$SESSION_SQL_STAT.SESS_ID
        IS 'session identifier';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$SESSION_SQL_STAT.STAT_VALUE
        IS 'statistic value';

--#####################
--# grant view
--#####################

GRANT SELECT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$SESSION_SQL_STAT TO PUBLIC;

COMMIT;


--##############################################################
--# V$SHM_SEGMENT
--##############################################################

--#####################
--# drop view
--#####################

DROP VIEW IF EXISTS PERFORMANCE_VIEW_SCHEMA.V$SHM_SEGMENT;

--#####################
--# create view
--#####################

CREATE VIEW PERFORMANCE_VIEW_SCHEMA.V$SHM_SEGMENT
(  
       SHM_NAME
     , SHM_ID
     , SHM_SIZE
     , SHM_KEY
     , SHM_SEQ
     , SHM_ADDR
)
AS 
SELECT 
       CAST( shmseg.NAME AS VARCHAR(32 OCTETS) )    -- SHM_NAME
     , CAST( shmseg.ID AS NUMBER )                  -- SHM_ID
     , CAST( shmseg.SIZE AS NUMBER )                -- SHM_SIZE
     , CAST( shmseg.KEY AS NUMBER )                 -- SHM_KEY
     , CAST( shmseg.SEQ AS NUMBER )                 -- SHM_SEQ
     , CAST( shmseg.ADDR AS VARCHAR(32 OCTETS) )    -- SHM_ADDR
  FROM 
       FIXED_TABLE_SCHEMA.X$SHM_SEGMENT AS shmseg
;
             
--#####################
--# comment view
--#####################

COMMENT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$SHM_SEGMENT
        IS 'The V$SHM_SEGMENT displays a list of all shared memory segments.';

--#####################
--# comment column
--#####################

COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$SHM_SEGMENT.SHM_NAME
        IS 'shared memory segment name';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$SHM_SEGMENT.SHM_ID
        IS 'shared memory segment identifier';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$SHM_SEGMENT.SHM_SIZE
        IS 'shared memory segment size ( in bytes )';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$SHM_SEGMENT.SHM_KEY
        IS 'shared memory segment key';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$SHM_SEGMENT.SHM_SEQ
        IS 'shared memory segment sequence';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$SHM_SEGMENT.SHM_ADDR
        IS 'start address of the shared memory segment';

--#####################
--# grant view
--#####################

GRANT SELECT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$SHM_SEGMENT TO PUBLIC;

COMMIT;



--##############################################################
--# V$SQLFN_METADATA
--##############################################################

--#####################
--# drop view
--#####################

DROP VIEW IF EXISTS PERFORMANCE_VIEW_SCHEMA.V$SQLFN_METADATA;

--#####################
--# create view
--#####################

CREATE VIEW PERFORMANCE_VIEW_SCHEMA.V$SQLFN_METADATA
(  
       FUNC_NAME
     , MINARGS
     , MAXARGS
     , IS_AGGREGATE
)
AS 
SELECT 
       CAST( func.FUNC_NAME AS VARCHAR(128 OCTETS) )   -- FUNC_NAME
     , CAST( func.MIN_ARG_COUNT AS NUMBER )            -- MINARGS
     , CAST( func.MAX_ARG_COUNT AS NUMBER )            -- MAXARGS
     , CAST( FALSE AS BOOLEAN )                        -- IS_AGGREGATE
  FROM 
       FIXED_TABLE_SCHEMA.X$BUILTIN_FUNCTION func
UNION ALL
SELECT 
       CAST( aggr.FUNC_NAME AS VARCHAR(128 OCTETS) )
     , CAST( aggr.MIN_ARG_COUNT AS NUMBER )
     , CAST( aggr.MAX_ARG_COUNT AS NUMBER )
     , TRUE
  FROM 
       FIXED_TABLE_SCHEMA.X$BUILTIN_AGGREGATION aggr
;
             
--#####################
--# comment view
--#####################

COMMENT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$SQLFN_METADATA
        IS 'The V$SQLFN_METADATA contains metadata about operators and built-in functions';

--#####################
--# comment column
--#####################

COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$SQLFN_METADATA.FUNC_NAME
        IS 'name of the built-in function';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$SQLFN_METADATA.MINARGS
        IS 'minimum number of arguments for the function';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$SQLFN_METADATA.MAXARGS
        IS 'maximum number of arguments for the function';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$SQLFN_METADATA.IS_AGGREGATE
        IS 'indicates whether the function is an aggregate function (TRUE) or not (FALSE)';

--#####################
--# grant view
--#####################

GRANT SELECT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$SQLFN_METADATA TO PUBLIC;

COMMIT;


--##############################################################
--# V$SQL_CACHE
--##############################################################

--#####################
--# drop view
--#####################

DROP VIEW IF EXISTS PERFORMANCE_VIEW_SCHEMA.V$SQL_CACHE;

--#####################
--# create view
--#####################

CREATE VIEW PERFORMANCE_VIEW_SCHEMA.V$SQL_CACHE
(  
       SQL_HANDLE
     , HASH_VALUE
     , REF_COUNT
     , CLOCK_ID
     , PLAN_AGE
     , USER_NAME
     , BIND_PARAM_COUNT
     , SQL_TEXT
     , PLAN_COUNT
     , PLAN_ID
     , PLAN_SIZE
     , PLAN_IS_ATOMIC
     , PLAN_TEXT
)
AS 
SELECT 
       /*+ USE_HASH( sqlplan ) */
       CAST( sqlcache.SQL_HANDLE AS NUMBER )            -- SQL_HANDLE
     , CAST( sqlcache.HASH_VALUE AS NUMBER )            -- HASH_VALUE
     , CAST( sqlplan.REF_COUNT AS NUMBER )              -- REF_COUNT
     , CAST( sqlcache.CLOCK_ID AS NUMBER )              -- CLOCK_ID
     , CAST( sqlcache.AGE AS NUMBER )                   -- PLAN_AGE
     , auth.AUTHORIZATION_NAME                          -- USER_NAME
     , CAST( sqlcache.BIND_COUNT AS NUMBER )            -- BIND_PARAM_COUNT
     , CAST( sqlcache.SQL_STRING AS LONG VARCHAR )      -- SQL_TEXT
     , CAST( sqlcache.PLAN_COUNT AS NUMBER )            -- PLAN_COUNT
     , CAST( sqlplan.PLAN_IDX AS NUMBER )               -- PLAN_ID
     , CAST( sqlplan.PLAN_SIZE AS NUMBER )              -- PLAN_SIZE
     , CAST( sqlplan.IS_ATOMIC AS BOOLEAN )             -- PLAN_IS_ATOMIC
     , CAST( sqlplan.PLAN_STRING AS LONG VARCHAR )      -- PLAN_TEXT
  FROM 
       FIXED_TABLE_SCHEMA.X$SQL_CACHE AS sqlcache
       INNER JOIN
       FIXED_TABLE_SCHEMA.X$SQL_CACHE_PLAN AS sqlplan
       ON sqlcache.sql_handle = sqlplan.sql_handle
       LEFT OUTER JOIN   
       DEFINITION_SCHEMA.AUTHORIZATIONS AS auth
       ON sqlcache.USER_ID = auth.AUTH_ID
  WHERE sqlcache.DROPPED = FALSE
;
             
--#####################
--# comment view
--#####################

COMMENT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$SQL_CACHE
        IS 'The V$SQL_CACHE lists statistics of shared SQL plan.';

--#####################
--# comment column
--#####################

COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$SQL_CACHE.SQL_HANDLE
        IS 'SQL handle';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$SQL_CACHE.HASH_VALUE
        IS 'hash value of the SQL statement';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$SQL_CACHE.REF_COUNT
        IS 'count of prepared statements referencing the statement';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$SQL_CACHE.CLOCK_ID
        IS 'clock identifier';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$SQL_CACHE.PLAN_AGE
        IS 'plan age';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$SQL_CACHE.USER_NAME
        IS 'user name';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$SQL_CACHE.BIND_PARAM_COUNT
        IS 'count of bind parameters';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$SQL_CACHE.SQL_TEXT
        IS 'SQL full text';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$SQL_CACHE.PLAN_COUNT
        IS 'physical plan count of the SQL statement';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$SQL_CACHE.PLAN_ID
        IS 'plan identifier';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$SQL_CACHE.PLAN_SIZE
        IS 'the total plan size of the SQL statement ( in bytes )';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$SQL_CACHE.PLAN_IS_ATOMIC
        IS 'plan is atomic array insert or not';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$SQL_CACHE.PLAN_TEXT
        IS 'plan text for SQL statement';

--#####################
--# grant view
--#####################

GRANT SELECT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$SQL_CACHE TO PUBLIC;

COMMIT;



--##############################################################
--# V$STATEMENT
--##############################################################

--#####################
--# drop view
--#####################

DROP VIEW IF EXISTS PERFORMANCE_VIEW_SCHEMA.V$STATEMENT;

--#####################
--# create view
--#####################

CREATE VIEW PERFORMANCE_VIEW_SCHEMA.V$STATEMENT 
( 
       SESSION_ID
     , STMT_ID
     , STMT_VIEW_SCN
     , SQL_TEXT
     , START_TIME
)
AS 
SELECT 
       CAST( xstmt.SESSION_ID AS NUMBER )                -- SESSION_ID
     , CAST( xstmt.ID AS NUMBER )                        -- STMT_ID
     , CAST( xstmt.VIEW_SCN AS NUMBER )                  -- STMT_VIEW_SCN
     , CAST( xstmt.SQL_TEXT AS VARCHAR(1024 OCTETS) )    -- SQL_TEXT
     , xstmt.START_TIME                                  -- START_TIME
  FROM  
       FIXED_TABLE_SCHEMA.X$STATEMENT    AS xstmt
 UNION ALL
SELECT
       CAST( xcur.SESSION_ID AS NUMBER )                 -- SESSION_ID
     , CAST( NULL AS NUMBER )                            -- STMT_ID
     , CAST( xcur.VIEW_SCN AS NUMBER )                   -- STMT_VIEW_SCN
     , CAST( xcur.CURSOR_QUERY AS VARCHAR(1024 OCTETS) ) -- SQL_TEXT
     , xcur.OPEN_TIME                                    -- START_TIME
  FROM  
       FIXED_TABLE_SCHEMA.X$NAMED_CURSOR AS xcur
 WHERE 
       xcur.IS_OPEN = TRUE
ORDER BY 1, 2
;
             
--#####################
--# comment view
--#####################

COMMENT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$STATEMENT 
        IS 'The V$STATEMENT lists all statements.';

--#####################
--# comment column
--#####################

COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$STATEMENT.SESSION_ID
        IS 'session identifier';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$STATEMENT.STMT_ID
        IS 'statement identifier in a session, null if named cursor query';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$STATEMENT.STMT_VIEW_SCN
        IS 'statement view scn';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$STATEMENT.SQL_TEXT
        IS 'first 1024 bytes of the SQL text for the statement';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$STATEMENT.START_TIME
        IS 'statement start time';

--#####################
--# grant view
--#####################

GRANT SELECT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$STATEMENT TO PUBLIC;

COMMIT;


--##############################################################
--# V$SPROPERTY
--##############################################################

--#####################
--# drop view
--#####################

DROP VIEW IF EXISTS PERFORMANCE_VIEW_SCHEMA.V$SPROPERTY;

--#####################
--# create view
--#####################

CREATE VIEW PERFORMANCE_VIEW_SCHEMA.V$SPROPERTY 
( 
       PROPERTY_NAME
     , DESCRIPTION
     , DATA_TYPE
     , STARTUP_PHASE
     , VALUE_UNIT
     , PROPERTY_VALUE
     , PROPERTY_SOURCE
     , INIT_VALUE
     , INIT_SOURCE
     , MIN_VALUE
     , MAX_VALUE
     , SES_MODIFIABLE
     , SYS_MODIFIABLE
     , IS_MODIFIABLE
)
AS 
SELECT 
       CAST( PROPERTY_NAME AS VARCHAR(128 OCTETS) )  -- PROPERTY_NAME
     , CAST( DESCRIPTION AS VARCHAR(2048 OCTETS) )   -- DESCRIPTION
     , CAST( DATA_TYPE AS VARCHAR(32 OCTETS) )       -- DATA_TYPE
     , CAST( STARTUP_PHASE AS VARCHAR(32 OCTETS) )   -- STARTUP_PHASE
     , CAST( UNIT AS VARCHAR(32 OCTETS) )            -- VALUE_UNIT
     , CAST( VALUE AS VARCHAR(2048 OCTETS) )         -- PROPERTY_VALUE
     , CAST( SOURCE AS VARCHAR(32 OCTETS) )          -- PROPERTY_SOURCE
     , CAST( INIT_VALUE AS VARCHAR(2048 OCTETS) )    -- INIT_VALUE
     , CAST( INIT_SOURCE AS VARCHAR(32 OCTETS) )     -- INIT_SOURCE
     , CAST( CASE WHEN DATA_TYPE = 'VARCHAR' THEN NULL
                                             ELSE MIN 
             END
             AS NUMBER )                             -- MIN_VALUE
     , CAST( CASE WHEN DATA_TYPE = 'VARCHAR' THEN NULL
                                             ELSE MAX 
             END
             AS NUMBER )                             -- MAX_VALUE
     , CAST( SES_MODIFIABLE AS VARCHAR(32 OCTETS) )  -- SES_MODIFIABLE
     , CAST( SYS_MODIFIABLE AS VARCHAR(32 OCTETS) )  -- SYS_MODIFIABLE
     , CAST( MODIFIABLE AS VARCHAR(32 OCTETS) )      -- IS_MODIFIABLE
  FROM 
       FIXED_TABLE_SCHEMA.X$SPROPERTY
 WHERE 
       DOMAIN = 'EXTERNAL'
ORDER BY 
      PROPERTY_ID
;

--#####################
--# comment view
--#####################

COMMENT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$SPROPERTY  
        IS 'The V$SPROPERTY displays a list of Properties. This is store a binary property file';

--#####################
--# comment column
--#####################

COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$SPROPERTY.PROPERTY_NAME
        IS 'name of the property';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$SPROPERTY.DESCRIPTION
        IS 'description of the property';        
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$SPROPERTY.DATA_TYPE
        IS 'data type of the property';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$SPROPERTY.STARTUP_PHASE
        IS 'modifiable startup-phase: the value IN ( NO MOUNT / MOUNT / OPEN & [BELOW|ABOVE] )';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$SPROPERTY.VALUE_UNIT
        IS 'unit of the property value: the value in ( NONE, BYTE, MS(milisec) )';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$SPROPERTY.PROPERTY_VALUE
        IS 'property value stored in the binary property file';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$SPROPERTY.PROPERTY_SOURCE
        IS 'source of the current property value: the value is BINARY_FILE';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$SPROPERTY.INIT_VALUE
        IS 'property init value for the system';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$SPROPERTY.INIT_SOURCE
        IS 'source of the current property INIT_VALUE: the value IN ( USER, DEFAULT, ENV_VAR, BINARY_FILE, FILE, SYSTEM )';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$SPROPERTY.MIN_VALUE
        IS 'minimum value for property. null if type is varchar.';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$SPROPERTY.MAX_VALUE
        IS 'maximum value for property. null if type is varchar.';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$SPROPERTY.SES_MODIFIABLE
        IS 'property can be changed with ALTER SESSION or not: the value in ( TRUE, FALSE )';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$SPROPERTY.SYS_MODIFIABLE
        IS 'property can be changed with ALTER SYSTEM and when the change takes effect: the value in ( NONE, FALSE, IMMEDIATE, DEFERRED )';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$SPROPERTY.IS_MODIFIABLE
        IS 'property can be changed or not: the value in ( TRUE, FALSE )';

--#####################
--# grant view
--#####################

GRANT SELECT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$SPROPERTY TO PUBLIC;

COMMIT;


--##############################################################
--# V$SYSTEM_STAT
--##############################################################

--#####################
--# drop view
--#####################

DROP VIEW IF EXISTS PERFORMANCE_VIEW_SCHEMA.V$SYSTEM_STAT;

--#####################
--# create view
--#####################

CREATE VIEW PERFORMANCE_VIEW_SCHEMA.V$SYSTEM_STAT
(  
       STAT_NAME
     , STAT_VALUE
     , COMMENTS
)
AS 
SELECT 
       CAST( kninfo.NAME AS VARCHAR(128 OCTETS) )       -- STAT_NAME
     , CAST( kninfo.VALUE AS NUMBER )                   -- STAT_VALUE
     , CAST( kninfo.COMMENTS AS VARCHAR(1024 OCTETS) )  -- COMMENTS
  FROM 
       FIXED_TABLE_SCHEMA.X$KN_SYSTEM_INFO AS kninfo   
  WHERE kninfo.CATEGORY = 0
UNION ALL
SELECT 
       CAST( sminfo.NAME AS VARCHAR(128 OCTETS) )       -- STAT_NAME
     , CAST( sminfo.VALUE AS NUMBER )                   -- STAT_VALUE
     , CAST( sminfo.COMMENTS AS VARCHAR(1024 OCTETS) )  -- COMMENTS
  FROM 
       FIXED_TABLE_SCHEMA.X$SM_SYSTEM_INFO AS sminfo   
  WHERE sminfo.CATEGORY = 0
UNION ALL
SELECT 
       CAST( exeinfo.NAME AS VARCHAR(128 OCTETS) )      -- STAT_NAME
     , CAST( exeinfo.VALUE AS NUMBER )                  -- STAT_VALUE
     , CAST( exeinfo.COMMENTS AS VARCHAR(1024 OCTETS) ) -- COMMENTS
  FROM 
       FIXED_TABLE_SCHEMA.X$EXE_SYSTEM_INFO AS exeinfo
  WHERE exeinfo.CATEGORY = 0
UNION ALL
SELECT 
       CAST( sqlinfo.NAME AS VARCHAR(128 OCTETS) )      -- STAT_NAME
     , CAST( sqlinfo.VALUE AS NUMBER )                  -- STAT_VALUE
     , CAST( sqlinfo.COMMENTS AS VARCHAR(1024 OCTETS) ) -- COMMENTS
  FROM 
       FIXED_TABLE_SCHEMA.X$SQL_SYSTEM_INFO AS sqlinfo   
  WHERE sqlinfo.CATEGORY = 0
;
             
--#####################
--# comment view
--#####################

COMMENT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$SYSTEM_STAT
        IS 'The V$SYSTEM_STAT displays system statistics.';

--#####################
--# comment column
--#####################

COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$SYSTEM_STAT.STAT_NAME
        IS 'statistic name';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$SYSTEM_STAT.STAT_VALUE
        IS 'statistic value';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$SYSTEM_STAT.COMMENTS
        IS 'comments';

--#####################
--# grant view
--#####################

GRANT SELECT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$SYSTEM_STAT TO PUBLIC;

COMMIT;


--##############################################################
--# V$SYSTEM_MEM_STAT
--##############################################################

--#####################
--# drop view
--#####################

DROP VIEW IF EXISTS PERFORMANCE_VIEW_SCHEMA.V$SYSTEM_MEM_STAT;

--#####################
--# create view
--#####################

CREATE VIEW PERFORMANCE_VIEW_SCHEMA.V$SYSTEM_MEM_STAT
(  
       STAT_NAME
     , STAT_VALUE
     , COMMENTS
)
AS 
SELECT 
       CAST( kninfo.NAME AS VARCHAR(128 OCTETS) )       -- STAT_NAME
     , CAST( kninfo.VALUE AS NUMBER )                   -- STAT_VALUE
     , CAST( kninfo.COMMENTS AS VARCHAR(1024 OCTETS) )  -- COMMENTS
  FROM 
       FIXED_TABLE_SCHEMA.X$KN_SYSTEM_INFO AS kninfo   
  WHERE kninfo.CATEGORY = 11
UNION ALL
SELECT 
       CAST( sminfo.NAME AS VARCHAR(128 OCTETS) )       -- STAT_NAME
     , CAST( sminfo.VALUE AS NUMBER )                   -- STAT_VALUE
     , CAST( sminfo.COMMENTS AS VARCHAR(1024 OCTETS) )  -- COMMENTS
  FROM 
       FIXED_TABLE_SCHEMA.X$SM_SYSTEM_INFO AS sminfo   
  WHERE sminfo.CATEGORY = 11
UNION ALL
SELECT 
       CAST( exeinfo.NAME AS VARCHAR(128 OCTETS) )      -- STAT_NAME
     , CAST( exeinfo.VALUE AS NUMBER )                  -- STAT_VALUE
     , CAST( exeinfo.COMMENTS AS VARCHAR(1024 OCTETS) ) -- COMMENTS
  FROM 
       FIXED_TABLE_SCHEMA.X$EXE_SYSTEM_INFO AS exeinfo
  WHERE exeinfo.CATEGORY = 11
UNION ALL
SELECT 
       CAST( sqlinfo.NAME AS VARCHAR(128 OCTETS) )      -- STAT_NAME
     , CAST( sqlinfo.VALUE AS NUMBER )                  -- STAT_VALUE
     , CAST( sqlinfo.COMMENTS AS VARCHAR(1024 OCTETS) ) -- COMMENTS
  FROM 
       FIXED_TABLE_SCHEMA.X$SQL_SYSTEM_INFO AS sqlinfo   
  WHERE sqlinfo.CATEGORY = 11
;
             
--#####################
--# comment view
--#####################

COMMENT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$SYSTEM_MEM_STAT
        IS 'The V$SYSTEM_MEM_STAT displays system memory statistics.';

--#####################
--# comment column
--#####################

COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$SYSTEM_MEM_STAT.STAT_NAME
        IS 'statistic name';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$SYSTEM_MEM_STAT.STAT_VALUE
        IS 'statistic value';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$SYSTEM_MEM_STAT.COMMENTS
        IS 'comments';

--#####################
--# grant view
--#####################

GRANT SELECT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$SYSTEM_MEM_STAT TO PUBLIC;

COMMIT;



--##############################################################
--# V$SYSTEM_SQL_STAT
--##############################################################

--#####################
--# drop view
--#####################

DROP VIEW IF EXISTS PERFORMANCE_VIEW_SCHEMA.V$SYSTEM_SQL_STAT;

--#####################
--# create view
--#####################

CREATE VIEW PERFORMANCE_VIEW_SCHEMA.V$SYSTEM_SQL_STAT
(  
       STAT_NAME
     , STAT_VALUE
     , COMMENTS
)
AS 
SELECT 
       CAST( sysinfo.NAME AS VARCHAR(128 OCTETS) )       -- STAT_NAME
     , CAST( sysinfo.VALUE AS NUMBER )                   -- STAT_VALUE
     , CAST( sysinfo.COMMENTS AS VARCHAR(1024 OCTETS) )  -- COMMENTS
  FROM 
       FIXED_TABLE_SCHEMA.X$SQL_SYSTEM_INFO AS sysinfo
  WHERE sysinfo.CATEGORY = 20
UNION ALL
SELECT 
       CAST( 'COMMAND: ' || sysexec.STMT_TYPE AS VARCHAR(128 OCTETS) )   -- STAT_NAME
     , CAST( sysexec.EXECUTE_COUNT AS NUMBER )                           -- STAT_VALUE
     , CAST( 'execution count of the command' AS VARCHAR(1024 OCTETS) )  -- COMMENTS
  FROM 
       FIXED_TABLE_SCHEMA.X$SQL_SYSTEM_STAT_EXEC_STMT AS sysexec
;
             
--#####################
--# comment view
--#####################

COMMENT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$SYSTEM_SQL_STAT
        IS 'The V$SYSTEM_SQL_STAT displays system SQL statistics.';

--#####################
--# comment column
--#####################

COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$SYSTEM_SQL_STAT.STAT_NAME
        IS 'statistic name';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$SYSTEM_SQL_STAT.STAT_VALUE
        IS 'statistic value';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$SYSTEM_SQL_STAT.COMMENTS
        IS 'comments';

--#####################
--# grant view
--#####################

GRANT SELECT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$SYSTEM_SQL_STAT TO PUBLIC;

COMMIT;



--##############################################################
--# V$TABLES
--##############################################################

--#####################
--# drop view
--#####################

DROP VIEW IF EXISTS PERFORMANCE_VIEW_SCHEMA.V$TABLES;

--#####################
--# create view
--#####################

CREATE VIEW PERFORMANCE_VIEW_SCHEMA.V$TABLES 
( 
       TABLE_OWNER
     , TABLE_SCHEMA
     , TABLE_NAME
     , STARTUP_PHASE
     , CREATED_TIME
     , MODIFIED_TIME
     , COMMENTS
)
AS 
SELECT 
       auth.AUTHORIZATION_NAME -- TABLE_OWNER
     , sch.SCHEMA_NAME         -- TABLE_SCHEMA
     , tab.TABLE_NAME          -- TABLE_NAME
     , CAST( CASE WHEN ( SELECT COUNT(*) 
                           FROM FIXED_TABLE_SCHEMA.X$FIXED_VIEW xfv
                          WHERE xfv.VIEW_NAME = tab.TABLE_NAME
                            AND xfv.STARTUP_PHASE = 'NO_MOUNT' ) > 0
                       THEN 'NO_MOUNT'
                  WHEN ( SELECT COUNT(*) 
                           FROM FIXED_TABLE_SCHEMA.X$FIXED_VIEW xfv
                          WHERE xfv.VIEW_NAME = tab.TABLE_NAME
                            AND xfv.STARTUP_PHASE = 'MOUNT' ) > 0
                       THEN 'MOUNT'
                  ELSE 'OPEN'
             END AS VARCHAR(32 OCTETS) )
     , tab.CREATED_TIME        -- CREATED_TIME
     , tab.MODIFIED_TIME       -- MODIFIED_TIME
     , tab.COMMENTS            -- COMMENTS
  FROM  
       DEFINITION_SCHEMA.TABLES          AS tab 
     , DEFINITION_SCHEMA.SCHEMATA        AS sch 
     , DEFINITION_SCHEMA.AUTHORIZATIONS  AS auth 
 WHERE 
       sch.SCHEMA_NAME = 'PERFORMANCE_VIEW_SCHEMA'
   AND tab.SCHEMA_ID   = sch.SCHEMA_ID 
   AND tab.OWNER_ID    = auth.AUTH_ID 
 ORDER BY
       tab.SCHEMA_ID
     , tab.TABLE_ID
;
             
--#####################
--# comment view
--#####################

COMMENT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$TABLES 
        IS 'The V$TABLES contains the definitions of all the performance views (views beginning with V$).';

--#####################
--# comment column
--#####################

COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$TABLES.TABLE_OWNER
        IS 'owner name who owns the performance view';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$TABLES.TABLE_SCHEMA
        IS 'schema name of the performance view';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$TABLES.TABLE_NAME
        IS 'name of the performance view';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$TABLES.STARTUP_PHASE
        IS 'visible startup phase of the performance view';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$TABLES.CREATED_TIME
        IS 'created time of the performance view';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$TABLES.MODIFIED_TIME
        IS 'modified time of the performance view';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$TABLES.COMMENTS
        IS 'comments of the performance view';

--#####################
--# grant view
--#####################

GRANT SELECT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$TABLES TO PUBLIC;

COMMIT;



--##############################################################
--# V$TABLESPACE
--##############################################################

--#####################
--# drop view
--#####################

DROP VIEW IF EXISTS PERFORMANCE_VIEW_SCHEMA.V$TABLESPACE;

--#####################
--# create view
--#####################

CREATE VIEW PERFORMANCE_VIEW_SCHEMA.V$TABLESPACE
(  
       TBS_NAME
     , TBS_ID
     , TBS_ATTR
     , IS_LOGGING
     , IS_ONLINE
     , OFFLINE_STATE
     , EXTENT_SIZE
     , PAGE_SIZE
)
AS 
SELECT 
       CAST( tbs.NAME AS VARCHAR(128 OCTETS) )            -- TBS_NAME
     , CAST( tbs.ID AS NUMBER )                           -- TBS_ID
     , CAST( tbs.ATTR AS VARCHAR(128 OCTETS) )            -- TBS_ATTR
     , tbs.LOGGING                                        -- IS_LOGGING
     , tbs.ONLINE                                         -- IS_ONLINE
     , CAST( tbs.OFFLINE_STATE AS VARCHAR(32 OCTETS) )    -- OFFLINE_STATE
     , CAST( tbs.EXTSIZE AS NUMBER )                      -- EXTENT_SIZE
     , CAST( tbs.PAGE_SIZE AS NUMBER )                    -- PAGE_SIZE
  FROM 
       FIXED_TABLE_SCHEMA.X$TABLESPACE AS tbs
  WHERE tbs.STATE = 'CREATED'
;
             
--#####################
--# comment view
--#####################

COMMENT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$TABLESPACE
        IS 'This view displays tablespace information.';

--#####################
--# comment column
--#####################

COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$TABLESPACE.TBS_NAME
        IS 'tablespace name';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$TABLESPACE.TBS_ID
        IS 'tablespace identifier';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$TABLESPACE.TBS_ATTR
        IS 'tablespace attribute: the value in ( device attribute (MEMORY) | temporary attribute (TEMPORARY, PERSISTENT) | usage attribute(DICT, UNDO, DATA, TEMPORARY) )';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$TABLESPACE.IS_LOGGING
        IS 'indicates whether the tablespace is a logging tablespace ( YES ) or not ( NO )';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$TABLESPACE.IS_ONLINE
        IS 'indicates whether the tablespace is ONLINE ( YES ) or OFFLINE ( NO )';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$TABLESPACE.OFFLINE_STATE
        IS 'indicates whether the tablespace can be taken online normally ( CONSISTENT ) or not ( INCONSISTENT ). null if the tablespace is ONLINE';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$TABLESPACE.EXTENT_SIZE
        IS 'extent size of the tablespace ( in bytes )';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$TABLESPACE.PAGE_SIZE
        IS 'page size of the tablespace ( in bytes )';

--#####################
--# grant view
--#####################

GRANT SELECT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$TABLESPACE TO PUBLIC;

COMMIT;



--##############################################################
--# V$TRANSACTION
--##############################################################

--#####################
--# drop view
--#####################

DROP VIEW IF EXISTS PERFORMANCE_VIEW_SCHEMA.V$TRANSACTION;

--#####################
--# create view
--#####################

CREATE VIEW PERFORMANCE_VIEW_SCHEMA.V$TRANSACTION
(  
       TRANS_ID
     , SESSION_ID
     , TRANS_SLOT_ID
     , PHYSICAL_TRANS_ID
     , TRANS_STATE
     , IS_GLOBAL
     , TRANS_ATTRIBUTE
     , ISOLATION_LEVEL
     , TRANS_VIEW_SCN
     , TCN
     , TRANS_SEQ
     , START_TIME
)
AS 
SELECT 
       CAST( xtrans.LOGICAL_TRANS_ID AS NUMBER )       -- TRANS_ID
     , CAST( xsess.ID AS NUMBER )                      -- SESSION_ID
     , CAST( xtrans.SLOT_ID AS NUMBER )                -- TRANS_SLOT_ID
     , CAST( xtrans.PHYSICAL_TRANS_ID AS NUMBER )      -- PHYSICAL_TRANS_ID
     , CAST( xtrans.STATE AS VARCHAR(32 OCTETS) )      -- TRANS_STATE
     , xtrans.IS_GLOBAL                                -- IS_GLOBAL
     , CAST( xtrans.ATTRIBUTE AS VARCHAR(32 OCTETS) )  -- TRANS_ATTRIBUTE
     , CAST( xtrans.ISOLATION_LEVEL AS VARCHAR(32 OCTETS) ) -- ISOLATION_LEVEL
     , CAST( xtrans.VIEW_SCN AS NUMBER )               -- TRANS_VIEW_SCN
     , CAST( xtrans.TCN AS NUMBER )                    -- TCN
     , CAST( xtrans.SEQ AS NUMBER )                    -- TRANS_SEQ
     , xtrans.BEGIN_TIME                               -- START_TIME
  FROM 
       FIXED_TABLE_SCHEMA.X$TRANSACTION AS xtrans   
       LEFT OUTER JOIN   
       FIXED_TABLE_SCHEMA.X$SESSION AS xsess   
       ON xtrans.LOGICAL_TRANS_ID = xsess.TRANS_ID
;
             
--#####################
--# comment view
--#####################

COMMENT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$TRANSACTION
        IS 'The V$TRANSACTION lists the active transactions in the system.';

--#####################
--# comment column
--#####################

COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$TRANSACTION.TRANS_ID
        IS 'transaction identifier';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$TRANSACTION.SESSION_ID
        IS 'session identifier ( null if the global transaction is unassociated )';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$TRANSACTION.TRANS_SLOT_ID
        IS 'transaction slot identifier';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$TRANSACTION.PHYSICAL_TRANS_ID
        IS 'physical transaction identifier';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$TRANSACTION.TRANS_STATE
        IS 'transaction state: the value in ( ACTIVE, BLOCK, PREPARE, COMMIT, ROLLBACK, IDLE, PRECOMMIT )';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$TRANSACTION.IS_GLOBAL
        IS 'indicates whether the transaction is global or not';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$TRANSACTION.TRANS_ATTRIBUTE
        IS 'transaction attribute: the value in ( READ_ONLY, UPDATABLE, LOCKABLE, UPDATABLE | LOCKABLE )';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$TRANSACTION.ISOLATION_LEVEL
        IS 'transaction isolation level: the value in ( READ COMMITTED, SERIALIZABLE )';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$TRANSACTION.TRANS_VIEW_SCN
        IS 'transaction view scn';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$TRANSACTION.TCN
        IS 'transaction change number';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$TRANSACTION.TRANS_SEQ
        IS 'transaction sequence number';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$TRANSACTION.START_TIME
        IS 'transaction start time';

--#####################
--# grant view
--#####################

GRANT SELECT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$TRANSACTION TO PUBLIC;

COMMIT;



--##############################################################
--# V$INSTANCE
--##############################################################

--#####################
--# drop view
--#####################

DROP VIEW IF EXISTS PERFORMANCE_VIEW_SCHEMA.V$INSTANCE;

--#####################
--# create view
--#####################

CREATE VIEW PERFORMANCE_VIEW_SCHEMA.V$INSTANCE
(  
       RELEASE_VERSION
     , STARTUP_TIME
     , INSTANCE_STATUS
     , DATA_ACCESS_MODE
)
AS 
SELECT 
       inst.VERSION            -- RELEASE_VERSION
     , inst.STARTUP_TIME       -- STARTUP_TIME
     , inst.STATUS             -- INSTANCE_STATUS
     , CAST( CASE WHEN ( inst.STATUS = 'OPEN' ) = FALSE
                       THEN 'NONE'
                  ELSE ( CASE WHEN ( SELECT VALUE
                                       FROM X$SM_SYSTEM_INFO
                                      WHERE NAME = 'DATA_ACCESS_MODE' ) = 0
                                   THEN 'READ_ONLY'
                              ELSE 'READ_WRITE'
                         END )
             END AS VARCHAR(16 OCTETS) )  -- DATA_ACCESS_MODE
  FROM 
       FIXED_TABLE_SCHEMA.X$INSTANCE AS inst
;
             
--#####################
--# comment view
--#####################

COMMENT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$INSTANCE
        IS 'This view displays the state of the current instance.';

--#####################
--# comment column
--#####################

COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$INSTANCE.RELEASE_VERSION
        IS 'release version';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$INSTANCE.STARTUP_TIME
        IS 'time when the instance was started';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$INSTANCE.INSTANCE_STATUS
        IS 'status of the instance: the value in ( STARTED, MOUNTED, OPEN )';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$INSTANCE.DATA_ACCESS_MODE
        IS 'data access mode of the instance: the value in ( NONE, READ_ONLY, READ_WRITE )';

--#####################
--# grant view
--#####################

GRANT SELECT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$INSTANCE TO PUBLIC;

COMMIT;



--##############################################################
--# V$CONTROLFILE
--##############################################################

--#####################
--# drop view
--#####################

DROP VIEW IF EXISTS PERFORMANCE_VIEW_SCHEMA.V$CONTROLFILE;

--#####################
--# create view
--#####################

CREATE VIEW PERFORMANCE_VIEW_SCHEMA.V$CONTROLFILE
(  
       STATUS
     , CONTROLFILE_NAME
     , LAST_CHECKPOINT_LSN
     , IS_PRIMARY
)
AS 
SELECT 
       ctrl.CONTROLFILE_STATE                 -- STATUS
     , ctrl.CONTROLFILE_NAME                  -- CONTROLFILE_NAME
     , CAST( ctrl.CHECKPOINT_LSN AS NUMBER )  -- LAST_CHECKPOINT_LSN
     , ctrl.IS_PRIMARY                        -- IS_PRIMARY
  FROM 
       FIXED_TABLE_SCHEMA.X$CONTROLFILE AS ctrl
;
             
--#####################
--# comment view
--#####################

COMMENT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$CONTROLFILE
        IS 'This view displays information about controlfiles.';

--#####################
--# comment column
--#####################

COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$CONTROLFILE.STATUS
        IS 'controlfile status ( valid, invalid )';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$CONTROLFILE.CONTROLFILE_NAME
        IS 'controlfile name ( absolute path )';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$CONTROLFILE.LAST_CHECKPOINT_LSN
        IS 'the last checkpoint lsn';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$CONTROLFILE.IS_PRIMARY
        IS 'indicates whether the controlfile is primary';

--#####################
--# grant view
--#####################

GRANT SELECT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$CONTROLFILE TO PUBLIC;

COMMIT;



--##############################################################
--# V$DB_FILE
--##############################################################

--#####################
--# drop view
--#####################

DROP VIEW IF EXISTS PERFORMANCE_VIEW_SCHEMA.V$DB_FILE;

--#####################
--# create view
--#####################

CREATE VIEW PERFORMANCE_VIEW_SCHEMA.V$DB_FILE
(  
       FILE_NAME
     , FILE_TYPE
)
AS 
SELECT 
       CAST( dbf.PATH AS VARCHAR(1024 OCTETS) )        -- FILE_NAME
     , CAST( dbf.TYPE AS VARCHAR(16 OCTETS) )          -- FILE_TYPE
  FROM 
       FIXED_TABLE_SCHEMA.X$DB_FILE AS dbf
;
             
--#####################
--# comment view
--#####################

COMMENT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$DB_FILE
        IS 'The V$DB_FILE displays a list of all files using in database';

--#####################
--# comment column
--#####################

COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$DB_FILE.FILE_NAME
        IS 'file name';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$DB_FILE.FILE_TYPE
        IS 'file type';

--#####################
--# grant view
--#####################

GRANT SELECT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$DB_FILE TO PUBLIC;

COMMIT;

--##############################################################
--# V$SHARED_MODE
--##############################################################

--#####################
--# drop view
--#####################

DROP VIEW IF EXISTS PERFORMANCE_VIEW_SCHEMA.V$SHARED_MODE;

--#####################
--# create view
--#####################

CREATE VIEW PERFORMANCE_VIEW_SCHEMA.V$SHARED_MODE
(  
       NAME
     , VALUE
)
AS 
SELECT 
       CAST( shm.NAME AS VARCHAR(128 OCTETS) )    -- NAME
     , CAST( 'NO' AS VARCHAR(128 OCTETS) )        -- VALUE
  FROM 
       FIXED_TABLE_SCHEMA.X$SL_SYSTEM_INFO AS shm
  WHERE shm.NAME = 'SHARED_SERVER_ACTIVITY' AND shm.VALUE = 0
UNION ALL
SELECT 
       CAST( shm.NAME AS VARCHAR(128 OCTETS) )    -- NAME
     , CAST( 'YES' AS VARCHAR(128 OCTETS) )       -- VALUE
  FROM 
       FIXED_TABLE_SCHEMA.X$SL_SYSTEM_INFO AS shm
  WHERE shm.NAME = 'SHARED_SERVER_ACTIVITY' AND shm.VALUE = 1
UNION ALL
SELECT 
       CAST( shm.NAME AS VARCHAR(128 OCTETS) )    -- NAME
     , CAST( shm.VALUE AS VARCHAR(128 OCTETS) )   -- VALUE
  FROM 
       FIXED_TABLE_SCHEMA.X$SL_SYSTEM_INFO AS shm
  WHERE shm.NAME != 'SHARED_SERVER_ACTIVITY'
;
             
--#####################
--# comment view
--#####################

COMMENT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$SHARED_MODE
        IS 'The V$SHARED_MODE displays information of shared mode.';

--#####################
--# comment column
--#####################

COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$SHARED_MODE.NAME
        IS 'name';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$SHARED_MODE.VALUE
        IS 'value';

--#####################
--# grant view
--#####################

GRANT SELECT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$SHARED_MODE TO PUBLIC;

COMMIT;

--##############################################################
--# V$DISPATCHER
--##############################################################

--#####################
--# drop view
--#####################

DROP VIEW IF EXISTS PERFORMANCE_VIEW_SCHEMA.V$DISPATCHER;

--#####################
--# create view
--#####################

CREATE VIEW PERFORMANCE_VIEW_SCHEMA.V$DISPATCHER
(  
     PROCESS_ID
    ,RESPONSE_JOB_COUNT
    ,ACCEPT
    ,START_TIME
    ,CUR_CONNECTIONS
    ,CONNECTIONS
    ,CONNECTIONS_HIGHWATER
    ,MAX_CONNECTIONS
    ,RECV_STATUS
    ,RECV_BYTES
    ,RECV_UNITS
    ,RECV_IDLE
    ,RECV_BUSY
    ,SEND_STATUS
    ,SEND_BYTES
    ,SEND_UNITS
    ,SEND_IDLE
    ,SEND_BUSY
    )
AS 
SELECT 
       CAST( dsptr.PROCESS_ID AS NUMBER )               -- PROCESS_ID
     , CAST( dsptr.RESPONSE_JOB_COUNT AS NUMBER )       -- RESPONSE_JOB_COUNT
     , CAST( dsptr.ACCEPT AS BOOLEAN )                  -- ACCEPT
     , CAST( dsptr.START_TIME AS TIMESTAMP )            -- START_TIME
     , CAST( dsptr.CUR_CONNECTIONS AS NUMBER )          -- CUR_CONNECTIONS
     , CAST( dsptr.CONNECTIONS AS NUMBER )              -- CONNECTIONS
     , CAST( dsptr.CONNECTIONS_HIGHWATER AS NUMBER )    -- CONNECTIONS_HIGHWATER
     , CAST( dsptr.MAX_CONNECTIONS AS NUMBER )          -- MAX_CONNECTIONS
     , CAST( dsptr.RECV_STATUS AS VARCHAR(16 OCTETS) )  -- RECV_STATUS
     , CAST( dsptr.RECV_BYTES AS NUMBER )               -- RECV_BYTES
     , CAST( dsptr.RECV_UNITS AS NUMBER )               -- RECV_UNITS
     , CAST( dsptr.RECV_IDLE AS NUMBER )                -- RECV_IDLE
     , CAST( dsptr.RECV_BUSY AS NUMBER )                -- RECV_BUSY
     , CAST( dsptr.SEND_STATUS AS VARCHAR(16 OCTETS) )  -- SEND_STATUS
     , CAST( dsptr.SEND_BYTES AS NUMBER )               -- SEND_BYTES
     , CAST( dsptr.SEND_UNITS AS NUMBER )               -- SEND_UNITS
     , CAST( dsptr.SEND_IDLE AS NUMBER )                -- SEND_IDLE
     , CAST( dsptr.SEND_BUSY AS NUMBER )                -- SEND_BUSY
  FROM 
       FIXED_TABLE_SCHEMA.X$DISPATCHER AS dsptr
;
             
--#####################
--# comment view
--#####################

COMMENT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$DISPATCHER
        IS 'The V$DISPATCHER displays information of dispatchers.';

--#####################
--# comment column
--#####################

COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$DISPATCHER.PROCESS_ID
        IS 'dispatcher process identifier';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$DISPATCHER.RESPONSE_JOB_COUNT
        IS 'response job count';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$DISPATCHER.ACCEPT
        IS 'indicates whether this dispatcher is accepting new connections';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$DISPATCHER.START_TIME
        IS 'process start time';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$DISPATCHER.CUR_CONNECTIONS
        IS 'current number of connections';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$DISPATCHER.CONNECTIONS
        IS 'total number of connections';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$DISPATCHER.CONNECTIONS_HIGHWATER
        IS 'highest number of connections';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$DISPATCHER.MAX_CONNECTIONS
        IS 'maximum connections';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$DISPATCHER.RECV_STATUS
        IS 'receive status';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$DISPATCHER.RECV_BYTES
        IS 'total bytes of received';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$DISPATCHER.RECV_UNITS
        IS 'total units of received';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$DISPATCHER.RECV_IDLE
        IS 'total idle time of receive (1/100 second)';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$DISPATCHER.RECV_BUSY
        IS 'total busy time of receive (1/100 second)';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$DISPATCHER.SEND_STATUS
        IS 'send status';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$DISPATCHER.SEND_BYTES
        IS 'total bytes of sent';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$DISPATCHER.SEND_UNITS
        IS 'total units of sent';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$DISPATCHER.SEND_IDLE
        IS 'total idle time of send (1/100 second)';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$DISPATCHER.SEND_BUSY
        IS 'total busy time of send (1/100 second)';


--#####################
--# grant view
--#####################

GRANT SELECT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$DISPATCHER TO PUBLIC;

COMMIT;


--##############################################################
--# V$SHARED_SERVER
--##############################################################

--#####################
--# drop view
--#####################

DROP VIEW IF EXISTS PERFORMANCE_VIEW_SCHEMA.V$SHARED_SERVER;

--#####################
--# create view
--#####################

CREATE VIEW PERFORMANCE_VIEW_SCHEMA.V$SHARED_SERVER
(  
       PROCESS_ID
     , PROCESSED_JOB_COUNT
     , STATUS
     , IDLE
     , BUSY
)
AS 
SELECT 
       CAST( shsvr.PROCESS_ID AS NUMBER )           -- PROCESS_ID
     , CAST( shsvr.PROCESSED_JOB_COUNT AS NUMBER )  -- PROCESSED_JOB_COUNT
     , CAST( shsvr.STATUS AS VARCHAR(16 OCTETS) )   -- STATUS
     , CAST( shsvr.IDLE AS NUMBER )                 -- IDLE
     , CAST( shsvr.BUSY AS NUMBER )                 -- BUSY
     
  FROM 
       FIXED_TABLE_SCHEMA.X$SHARED_SERVER AS shsvr
;
             
--#####################
--# comment view
--#####################

COMMENT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$SHARED_SERVER
        IS 'The V$SHARED_SERVER displays information of shared servers.';

--#####################
--# comment column
--#####################

COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$SHARED_SERVER.PROCESS_ID
        IS 'shared server process identifier';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$SHARED_SERVER.PROCESSED_JOB_COUNT
        IS 'processed job count';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$SHARED_SERVER.STATUS
        IS 'status';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$SHARED_SERVER.IDLE
        IS 'total idle time (1/100 second)';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$SHARED_SERVER.BUSY
        IS 'total busy time (1/100 second)';


--#####################
--# grant view
--#####################

GRANT SELECT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$SHARED_SERVER TO PUBLIC;

COMMIT;



--##############################################################
--# V$BALANCER
--##############################################################

--#####################
--# drop view
--#####################

DROP VIEW IF EXISTS PERFORMANCE_VIEW_SCHEMA.V$BALANCER;

--#####################
--# create view
--#####################

CREATE VIEW PERFORMANCE_VIEW_SCHEMA.V$BALANCER
(  
       PROCESS_ID
     , CUR_CONNECTIONS
     , CONNECTIONS
     , CONNECTIONS_HIGHWATER
     , MAX_CONNECTIONS
     , STATUS
)
AS 
SELECT 
       CAST( blcr.PROCESS_ID AS NUMBER )            -- PROCESS_ID
     , CAST( blcr.CUR_CONNECTIONS AS NUMBER )       -- CUR_CONNECTIONS
     , CAST( blcr.CONNECTIONS AS NUMBER )           -- CONNECTIONS
     , CAST( blcr.CONNECTIONS_HIGHWATER AS NUMBER ) -- CONNECTIONS_HIGHWATER
     , CAST( blcr.MAX_CONNECTIONS AS NUMBER )       -- MAX_CONNECTIONS
     , CAST( blcr.STATUS AS VARCHAR(16 OCTETS) )    -- STATUS
  FROM 
       FIXED_TABLE_SCHEMA.X$BALANCER AS blcr
;
             
--#####################
--# comment view
--#####################

COMMENT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$BALANCER
        IS 'The V$BALANCER displays information of balancer.';

--#####################
--# comment column
--#####################

COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$BALANCER.PROCESS_ID
        IS 'balancer process identifier';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$BALANCER.CUR_CONNECTIONS
        IS 'current number of connections';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$BALANCER.CONNECTIONS
        IS 'total number of connections';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$BALANCER.CONNECTIONS_HIGHWATER
        IS 'highest number of connections';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$BALANCER.MAX_CONNECTIONS
        IS 'maximum connections';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$BALANCER.STATUS
        IS 'status';


--#####################
--# grant view
--#####################

GRANT SELECT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$BALANCER TO PUBLIC;

COMMIT;



--##############################################################
--# V$QUEUE
--##############################################################

--#####################
--# drop view
--#####################

DROP VIEW IF EXISTS PERFORMANCE_VIEW_SCHEMA.V$QUEUE;

--#####################
--# create view
--#####################

CREATE VIEW PERFORMANCE_VIEW_SCHEMA.V$QUEUE
(  
       TYPE
     , INDEX
     , QUEUED
     , WAIT
     , TOTALQ
)
AS 
SELECT 
       CAST( queue.TYPE AS VARCHAR(16 OCTETS) ) -- TYPE
     , CAST( queue.INDEX AS NUMBER )            -- INDEX
     , CAST( queue.QUEUED AS NUMBER )           -- QUEUED
     , CAST( queue.WAIT AS NUMBER )             -- WAIT
     , CAST( queue.TOTALQ AS NUMBER )           -- TOTALQ
  FROM 
       FIXED_TABLE_SCHEMA.X$QUEUE AS queue
;
             
--#####################
--# comment view
--#####################

COMMENT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$QUEUE
        IS 'The V$QUEUE displays information of queue.';

--#####################
--# comment column
--#####################

COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$QUEUE.TYPE
        IS 'queue type ( COMMON or DISPATCHER )';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$QUEUE.INDEX
        IS 'index';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$QUEUE.QUEUED
        IS 'number of items in the queue';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$QUEUE.WAIT
        IS 'total time that all items in this queue have waited (1/100 second)';
COMMENT ON COLUMN PERFORMANCE_VIEW_SCHEMA.V$QUEUE.TOTALQ
        IS 'total number of items that have ever been in the queue';


--#####################
--# grant view
--#####################

GRANT SELECT ON TABLE PERFORMANCE_VIEW_SCHEMA.V$QUEUE TO PUBLIC;

COMMIT;


