/*******************************************************************************
 * qlgNoMountPerfView.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id$
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file qlgNoMountPerfView.c 
 * @brief SQL Processor Layer NOMOUNT phase Performance View
 */


#include <qll.h>
#include <qlgStartup.h>

/**
 * @addtogroup qlgStartup
 * @{
 */


/**
 * @brief NOMOUNT 단계의 Performance View Definition
 * @remarks
 * - MOUNT 단계에 정의되어 있지 않아야 함.
 * - OPEN 단계의 $GOLDILOCKS_HOME/admin/PerformanceView.sql 에 정의되어 있어야 함.
 * - V$TABLES 는 D$OBJECT_CACHE 를 이용함
 * - V$COLUMNS 는 구현의 복잡도 때문에 NOMOUNT, MOUNT 단계에서 지원하지 않음.
 * --- Performance View 를 CREATE FORCE VIEW 와 유사한 개념(view string 만 관리) 으로 구현함.
 * --- CREATE performance VIEW 를 위해 COLUMN 정보 뿐 아니라
 * --- DATA_TYPE 정보도 가상 Cache 로 구현해야 하여, 복잡도가 매우 높아짐.
 * --- work-around: \\desc V$XXXX 로 컬럼 정보 조회 가능함
 */
qlgNonServicePerfView gQlgNoMountPerfViewDefinition[] =
{
    {
        /************************************************************
         * V$ERROR_CODE
         ************************************************************/
        
        "V$ERROR_CODE",
        
        " CREATE VIEW PERFORMANCE_VIEW_SCHEMA.V$ERROR_CODE  \n"
        " (   \n"
        "        ERROR_CODE \n"
        "      , SQL_STATE \n"
        "      , ERROR_MESSAGE \n"
        " ) \n"
        " AS  \n"
        " SELECT  \n"
        "        CAST( xerr.ERROR_CODE AS NUMBER )             -- ERROR_CODE \n"
        "      , CAST( xerr.SQL_STATE AS VARCHAR(32 OCTETS) )  -- SQL_STATE \n"
        "      , CAST( xerr.MESSAGE AS VARCHAR(1024 OCTETS) )  -- ERROR_MESSAGE \n"
        "   FROM  \n"
        "        FIXED_TABLE_SCHEMA.X$ERROR_CODE AS xerr \n"
    }
    ,
    {
        /************************************************************
         * V$INSTANCE
         ************************************************************/
        
        "V$INSTANCE",

        " CREATE VIEW PERFORMANCE_VIEW_SCHEMA.V$INSTANCE \n"
        " (   \n"
        "        RELEASE_VERSION \n"
        "      , STARTUP_TIME \n"
        "      , INSTANCE_STATUS \n"
        "      , DATA_ACCESS_MODE \n"
        " ) \n"
        " AS  \n"
        " SELECT  \n"
        "        inst.VERSION            -- RELEASE_VERSION \n"
        "      , inst.STARTUP_TIME       -- STARTUP_TIME \n"
        "      , inst.STATUS             -- INSTANCE_STATUS \n"
        "      , CAST( CASE WHEN ( inst.STATUS = 'OPEN' ) = FALSE \n"
        "                        THEN 'NONE' \n"
        "                   ELSE ( CASE WHEN ( SELECT VALUE \n"
        "                                        FROM X$SM_SYSTEM_INFO \n"
        "                                       WHERE NAME = 'DATA_ACCESS_MODE' ) = 0 \n"
        "                                    THEN 'READ_ONLY' \n"
        "                               ELSE 'READ_WRITE' \n"
        "                          END ) \n"
        "              END AS VARCHAR(16 OCTETS) )\n"
        "   FROM  \n"
        "        FIXED_TABLE_SCHEMA.X$INSTANCE AS inst \n"
    }
    ,
    {
        /************************************************************
         * V$KEYWORDS
         ************************************************************/
        
        "V$KEYWORDS",
        
        " CREATE VIEW PERFORMANCE_VIEW_SCHEMA.V$KEYWORDS  \n"
        " (   \n"
        "        KEYWORD_NAME \n"
        "      , KEYWORD_LENGTH \n"
        "      , IS_RESERVED \n"
        " ) \n"
        " AS  \n"
        " SELECT  \n"
        "        CAST( key.NAME AS VARCHAR(128 OCTETS) )   -- KEYWORD_NAME \n"
        "      , CAST( LENGTH(key.NAME) AS NUMBER )        -- KEYWORD_LENGTH \n"
        "      , CASE key.CATEGORY  \n"
        "             WHEN 0 THEN TRUE \n"
        "                    ELSE FALSE \n"
        "        END                                       -- IS_RESERVED \n"
        "   FROM  \n"
        "        FIXED_TABLE_SCHEMA.X$SQL_KEYWORDS key \n"
    }
    ,
    {
        /************************************************************
         * V$LATCH
         ************************************************************/
        
        "V$LATCH",
    
        " CREATE VIEW PERFORMANCE_VIEW_SCHEMA.V$LATCH \n"
        " (   \n"
        "        LATCH_DESCRIPTION \n"
        "      , REF_COUNT \n"
        "      , SPIN_LOCK \n"
        "      , WAIT_COUNT \n"
        "      , CURRENT_MODE \n"
        " ) \n"
        " AS  \n"
        " SELECT  \n"
        "        CAST( latch.DESCRIPTION AS VARCHAR(64 OCTETS) )     -- LATCH_DESCRIPTION \n"
        "      , CAST( latch.REF_COUNT AS NUMBER )                   -- REF_COUNT \n"
        "      , CAST( CASE WHEN latch.SPIN_LOCK = 1 \n"
        "                        THEN 'YES' \n"
        "                        ELSE 'NO' \n"
        "                        END AS VARCHAR(3 OCTETS) )          -- SPIN_LOCK \n"
        "      , CAST( latch.WAIT_COUNT AS NUMBER )                  -- WAIT_COUNT \n"
        "      , CAST( latch.CURRENT_MODE AS VARCHAR(32 OCTETS) )    -- CURRENT_MODE \n"
        "   FROM  \n"
        "        FIXED_TABLE_SCHEMA.X$LATCH AS latch \n"
    }
    ,

    {
        /************************************************************
         * V$PROPERTY
         ************************************************************/
        
        "V$PROPERTY",
    
        " CREATE VIEW PERFORMANCE_VIEW_SCHEMA.V$PROPERTY  \n"
        " (  \n"
        "        PROPERTY_NAME \n"
        "      , DESCRIPTION \n"
        "      , DATA_TYPE \n"
        "      , STARTUP_PHASE \n"
        "      , VALUE_UNIT \n"
        "      , PROPERTY_VALUE \n"
        "      , PROPERTY_SOURCE \n"
        "      , INIT_VALUE \n"
        "      , INIT_SOURCE \n"
        "      , MIN_VALUE \n"
        "      , MAX_VALUE \n"
        "      , SES_MODIFIABLE \n"
        "      , SYS_MODIFIABLE \n"
        "      , IS_MODIFIABLE \n"
        " ) \n"
        " AS  \n"
        " SELECT  \n"
        "        CAST( PROPERTY_NAME AS VARCHAR(128 OCTETS) )  -- PROPERTY_NAME \n"
        "      , CAST( DESCRIPTION AS VARCHAR(2048 OCTETS) )   -- DESCRIPTION \n"
        "      , CAST( DATA_TYPE AS VARCHAR(32 OCTETS) )       -- DATA_TYPE \n"
        "      , CAST( STARTUP_PHASE AS VARCHAR(32 OCTETS) )   -- STARTUP_PHASE \n"
        "      , CAST( UNIT AS VARCHAR(32 OCTETS) )            -- VALUE_UNIT \n"
        "      , CAST( VALUE AS VARCHAR(2048 OCTETS) )         -- PROPERTY_VALUE \n"
        "      , CAST( SOURCE AS VARCHAR(32 OCTETS) )          -- PROPERTY_SOURCE \n"
        "      , CAST( INIT_VALUE AS VARCHAR(2048 OCTETS) )    -- INIT_VALUE \n"
        "      , CAST( INIT_SOURCE AS VARCHAR(32 OCTETS) )     -- INIT_SOURCE \n"
        "      , CAST( CASE WHEN DATA_TYPE = 'VARCHAR' THEN NULL \n"
        "                                              ELSE MIN  \n"
        "              END \n"
        "              AS NUMBER )                             -- MIN_VALUE \n"
        "      , CAST( CASE WHEN DATA_TYPE = 'VARCHAR' THEN NULL \n"
        "                                              ELSE MAX  \n"
        "              END \n"
        "              AS NUMBER )                             -- MAX_VALUE \n"
        "      , CAST( SES_MODIFIABLE AS VARCHAR(32 OCTETS) )  -- SES_MODIFIABLE \n"
        "      , CAST( SYS_MODIFIABLE AS VARCHAR(32 OCTETS) )  -- SYS_MODIFIABLE \n"
        "      , CAST( MODIFIABLE AS VARCHAR(32 OCTETS) )      -- IS_MODIFIABLE \n"
        "   FROM  \n"
        "        FIXED_TABLE_SCHEMA.X$PROPERTY \n"
        "  WHERE  \n"
        "        DOMAIN = 'EXTERNAL' \n"
        " ORDER BY  \n"
        "       PROPERTY_ID \n"
    }
    ,
    {
        /************************************************************
         * V$PROCESS_STAT
         ************************************************************/
        
        "V$PROCESS_STAT",
    
        " CREATE VIEW PERFORMANCE_VIEW_SCHEMA.V$PROCESS_STAT \n"
        " (   \n"
        "        STAT_NAME \n"
        "      , PROC_ID \n"
        "      , STAT_VALUE \n"
        " ) \n"
        " AS  \n"
        " SELECT  \n"
        "        CAST( knstat.NAME AS VARCHAR(128 OCTETS) )       -- STAT_NAME \n"
        "      , CAST( knstat.ID AS NUMBER )                      -- PROC_ID \n"
        "      , CAST( knstat.VALUE AS NUMBER )                   -- STAT_VALUE \n"
        "   FROM  \n"
        "        FIXED_TABLE_SCHEMA.X$KN_PROC_STAT AS knstat    \n"
        "   WHERE knstat.CATEGORY = 0 \n"
        " UNION ALL \n"
        " SELECT  \n"
        "        CAST( smenv.NAME AS VARCHAR(128 OCTETS) )        -- STAT_NAME \n"
        "      , CAST( smenv.ID AS NUMBER )                       -- PROC_ID \n"
        "      , CAST( smenv.VALUE AS NUMBER )                    -- STAT_VALUE \n"
        "   FROM  \n"
        "        FIXED_TABLE_SCHEMA.X$SM_PROC_ENV AS smenv    \n"
        "   WHERE smenv.CATEGORY = 0 \n"
        " UNION ALL \n"
        " SELECT  \n"
        "        CAST( exeenv.NAME AS VARCHAR(128 OCTETS) )       -- STAT_NAME \n"
        "      , CAST( exeenv.ID AS NUMBER )                      -- PROC_ID \n"
        "      , CAST( exeenv.VALUE AS NUMBER )                   -- STAT_VALUE \n"
        "   FROM  \n"
        "        FIXED_TABLE_SCHEMA.X$EXE_PROC_ENV AS exeenv    \n"
        "   WHERE exeenv.CATEGORY = 0 \n"
        " UNION ALL \n"
        " SELECT  \n"
        "        CAST( sqlenv.NAME AS VARCHAR(128 OCTETS) )       -- STAT_NAME \n"
        "      , CAST( sqlenv.ID AS NUMBER )                      -- PROC_ID \n"
        "      , CAST( sqlenv.VALUE AS NUMBER )                   -- STAT_VALUE \n"
        "   FROM  \n"
        "        FIXED_TABLE_SCHEMA.X$SQL_PROC_ENV AS sqlenv    \n"
        "   WHERE sqlenv.CATEGORY = 0 \n"
        " ORDER BY 2, 1 \n"
    }
    ,

    {
        /************************************************************
         * V$PROCESS_MEM_STAT
         ************************************************************/
        
        "V$PROCESS_MEM_STAT",
    
        " CREATE VIEW PERFORMANCE_VIEW_SCHEMA.V$PROCESS_MEM_STAT \n"
        " (   \n"
        "        STAT_NAME \n"
        "      , PROC_ID \n"
        "      , STAT_VALUE \n"
        " ) \n"
        " AS  \n"
        " SELECT  \n"
        "        CAST( knstat.NAME AS VARCHAR(128 OCTETS) )       -- STAT_NAME \n"
        "      , CAST( knstat.ID AS NUMBER )                      -- PROC_ID \n"
        "      , CAST( knstat.VALUE AS NUMBER )                   -- STAT_VALUE \n"
        "   FROM  \n"
        "        FIXED_TABLE_SCHEMA.X$KN_PROC_STAT AS knstat    \n"
        "   WHERE knstat.CATEGORY = 11 \n"
        " UNION ALL \n"
        " SELECT  \n"
        "        CAST( smenv.NAME AS VARCHAR(128 OCTETS) )        -- STAT_NAME \n"
        "      , CAST( smenv.ID AS NUMBER )                       -- PROC_ID \n"
        "      , CAST( smenv.VALUE AS NUMBER )                    -- STAT_VALUE \n"
        "   FROM  \n"
        "        FIXED_TABLE_SCHEMA.X$SM_PROC_ENV AS smenv    \n"
        "   WHERE smenv.CATEGORY = 11 \n"
        " UNION ALL \n"
        " SELECT  \n"
        "        CAST( exeenv.NAME AS VARCHAR(128 OCTETS) )       -- STAT_NAME \n"
        "      , CAST( exeenv.ID AS NUMBER )                      -- PROC_ID \n"
        "      , CAST( exeenv.VALUE AS NUMBER )                   -- STAT_VALUE \n"
        "   FROM  \n"
        "        FIXED_TABLE_SCHEMA.X$EXE_PROC_ENV AS exeenv    \n"
        "   WHERE exeenv.CATEGORY = 11 \n"
        " UNION ALL \n"
        " SELECT  \n"
        "        CAST( sqlenv.NAME AS VARCHAR(128 OCTETS) )       -- STAT_NAME \n"
        "      , CAST( sqlenv.ID AS NUMBER )                      -- PROC_ID \n"
        "      , CAST( sqlenv.VALUE AS NUMBER )                   -- STAT_VALUE \n"
        "   FROM  \n"
        "        FIXED_TABLE_SCHEMA.X$SQL_PROC_ENV AS sqlenv    \n"
        "   WHERE sqlenv.CATEGORY = 11 \n"
        " ORDER BY 2, 1 \n"
    }
    ,

    {
        /************************************************************
         * V$PROCESS_SQL_STAT
         ************************************************************/
        
        "V$PROCESS_SQL_STAT",
    
        " CREATE VIEW PERFORMANCE_VIEW_SCHEMA.V$PROCESS_SQL_STAT \n"
        " (   \n"
        "        STAT_NAME \n"
        "      , PROC_ID \n"
        "      , STAT_VALUE \n"
        " ) \n"
        " AS  \n"
        " SELECT  \n"
        "        CAST( sqlprocenv.NAME AS VARCHAR(128 OCTETS) )                        -- STAT_NAME \n"
        "      , CAST( sqlprocenv.ID AS NUMBER )                                       -- PROC_ID \n"
        "      , CAST( sqlprocenv.VALUE AS NUMBER )                                    -- STAT_VALUE \n"
        "   FROM  \n"
        "        FIXED_TABLE_SCHEMA.X$SQL_PROC_ENV AS sqlprocenv \n"
        "   WHERE sqlprocenv.CATEGORY = 20 \n"
        " UNION ALL \n"
        " SELECT  \n"
        "        CAST( 'COMMAND: ' || sqlprocexec.STMT_TYPE AS VARCHAR(128 OCTETS) )   -- STAT_NAME \n"
        "      , CAST( sqlprocexec.ID AS NUMBER )                                      -- PROC_ID \n"
        "      , CAST( sqlprocexec.EXECUTE_COUNT AS NUMBER )                           -- STAT_VALUE \n"
        "   FROM  \n"
        "        FIXED_TABLE_SCHEMA.X$SQL_PROC_STAT_EXEC_STMT AS sqlprocexec \n"
        " ORDER BY 2, 1 \n"
    }
    ,
    {
        /************************************************************
         * V$RESERVED_WORDS
         ************************************************************/
        
        "V$RESERVED_WORDS",

        " CREATE VIEW PERFORMANCE_VIEW_SCHEMA.V$RESERVED_WORDS  \n"
        " (   \n"
        "        KEYWORD_NAME \n"
        "      , KEYWORD_LENGTH \n"
        " ) \n"
        " AS  \n"
        " SELECT  \n"
        "        CAST( key.NAME AS VARCHAR(128 OCTETS) )   -- KEYWORD_NAME \n"
        "      , CAST( LENGTH(key.NAME) AS NUMBER )        -- KEYWORD_LENGTH \n"
        "   FROM  \n"
        "        FIXED_TABLE_SCHEMA.X$SQL_KEYWORDS key \n"
        "  WHERE key.CATEGORY = 0; \n"
    }
    ,

    {
        /************************************************************
         * V$SESSION
         * - NOMOUNT, MOUNT 단계에서는 SYS 사용자로만 접근 가능함
         ************************************************************/

        "V$SESSION",
        
        " CREATE VIEW PERFORMANCE_VIEW_SCHEMA.V$SESSION \n"
        " (   \n"
        "        SESSION_ID \n"
        "      , SERIAL_NO \n"
        "      , TRANS_ID \n"
        "      , CONNECTION_TYPE \n"
        "      , USER_NAME \n"
        "      , SESSION_STATUS \n"
        "      , SERVER_TYPE \n"
        "      , PROCESS_ID \n"
        "      , LOGON_TIME \n"
        "      , PROGRAM_NAME \n"
        "      , CLIENT_ADDRESS \n"
        "      , CLIENT_PORT \n"
        " ) \n"
        " AS  \n"
        " SELECT  \n"
        "        CAST( xsess.ID AS NUMBER )                      -- SESSION_ID \n"
        "      , CAST( xsess.SERIAL AS NUMBER )                  -- SERIAL_NO \n"
        "      , CAST( xsess.TRANS_ID AS NUMBER )                -- TRANS_ID \n"
        "      , CAST( xsess.CONNECTION AS VARCHAR(32 OCTETS) )  -- CONNECTION_TYPE \n"
        "      , CAST( 'SYS' AS VARCHAR(128 OCTETS) )            -- USER_NAME \n"
        "      , CAST( xsess.STATUS AS VARCHAR(32 OCTETS) )      -- SESSION_STATUS \n"
        "      , CAST( xsess.SERVER AS VARCHAR(32 OCTETS) )      -- SERVER_TYPE \n"
        "      , CAST( xsess.CLIENT_PROCESS AS NUMBER )          -- PROCESS_ID \n"
        "      , xsess.LOGON_TIME                                -- LOGON_TIME \n"
        "      , CAST( xsess.PROGRAM AS VARCHAR(128 OCTETS) )    -- PROGRAM_NAME \n"
        "      , CAST( xsess.ADDRESS AS VARCHAR(1024 OCTETS) )   -- CLIENT_ADDRESS \n"
        "      , CAST( xsess.PORT AS NUMBER )                    -- CLIENT_PORT \n"
        "   FROM  \n"
        "        FIXED_TABLE_SCHEMA.X$SESSION AS xsess    \n"
        "  WHERE  \n"
        "        xsess.STATUS IN ( 'CONNECTED', 'SIGNALED', 'SNIPED', 'DEAD' )  \n"
    }
    ,

    {
        /************************************************************
         * V$SESSION_STAT
         ************************************************************/

        "V$SESSION_STAT",

        " CREATE VIEW PERFORMANCE_VIEW_SCHEMA.V$SESSION_STAT \n"
        " (   \n"
        "        STAT_NAME \n"
        "      , SESS_ID \n"
        "      , STAT_VALUE \n"
        " ) \n"
        " AS  \n"
        " SELECT  \n"
        "        CAST( knstat.NAME AS VARCHAR(128 OCTETS) )       -- STAT_NAME \n"
        "      , CAST( knstat.ID AS NUMBER )                      -- SESS_ID \n"
        "      , CAST( knstat.VALUE AS NUMBER )                   -- STAT_VALUE \n"
        "   FROM  \n"
        "        FIXED_TABLE_SCHEMA.X$KN_SESS_STAT AS knstat    \n"
        "   WHERE knstat.CATEGORY = 0 \n"
        " UNION ALL \n"
        " SELECT  \n"
        "        CAST( smenv.NAME AS VARCHAR(128 OCTETS) )        -- STAT_NAME \n"
        "      , CAST( smenv.ID AS NUMBER )                       -- SESS_ID \n"
        "      , CAST( smenv.VALUE AS NUMBER )                    -- STAT_VALUE \n"
        "   FROM  \n"
        "        FIXED_TABLE_SCHEMA.X$SM_SESS_ENV AS smenv    \n"
        "   WHERE smenv.CATEGORY = 0 \n"
        " UNION ALL \n"
        " SELECT  \n"
        "        CAST( smstat.NAME AS VARCHAR(128 OCTETS) )       -- STAT_NAME \n"
        "      , CAST( smstat.ID AS NUMBER )                      -- SESS_ID \n"
        "      , CAST( smstat.VALUE AS NUMBER )                   -- STAT_VALUE \n"
        "   FROM  \n"
        "        FIXED_TABLE_SCHEMA.X$SM_SESS_STAT AS smstat    \n"
        "   WHERE smstat.CATEGORY = 0 \n"
        " UNION ALL \n"
        " SELECT  \n"
        "        CAST( exeenv.NAME AS VARCHAR(128 OCTETS) )       -- STAT_NAME \n"
        "      , CAST( exeenv.ID AS NUMBER )                      -- SESS_ID \n"
        "      , CAST( exeenv.VALUE AS NUMBER )                   -- STAT_VALUE \n"
        "   FROM  \n"
        "        FIXED_TABLE_SCHEMA.X$EXE_SESS_ENV AS exeenv    \n"
        "   WHERE exeenv.CATEGORY = 0 \n"
        " UNION ALL \n"
        " SELECT  \n"
        "        CAST( sqlenv.NAME AS VARCHAR(128 OCTETS) )       -- STAT_NAME \n"
        "      , CAST( sqlenv.ID AS NUMBER )                      -- SESS_ID \n"
        "      , CAST( sqlenv.VALUE AS NUMBER )                   -- STAT_VALUE \n"
        "   FROM  \n"
        "        FIXED_TABLE_SCHEMA.X$SQL_SESS_ENV AS sqlenv    \n"
        "   WHERE sqlenv.CATEGORY = 0 \n"
        " UNION ALL \n"
        " SELECT  \n"
        "        CAST( ssstat.NAME AS VARCHAR(128 OCTETS) )       -- STAT_NAME \n"
        "      , CAST( ssstat.ID AS NUMBER )                      -- SESS_ID \n"
        "      , CAST( ssstat.VALUE AS NUMBER )                   -- STAT_VALUE \n"
        "   FROM  \n"
        "        FIXED_TABLE_SCHEMA.X$SS_SESS_STAT AS ssstat    \n"
        "   WHERE ssstat.CATEGORY = 0 \n"
        " ORDER BY 2, 1 \n"
    }
    ,
    {
        /************************************************************
         * V$SESSION_MEM_STAT
         ************************************************************/

        "V$SESSION_MEM_STAT",

        " CREATE VIEW PERFORMANCE_VIEW_SCHEMA.V$SESSION_MEM_STAT \n"
        " (   \n"
        "        STAT_NAME \n"
        "      , SESS_ID \n"
        "      , STAT_VALUE \n"
        " ) \n"
        " AS  \n"
        " SELECT  \n"
        "        CAST( knstat.NAME AS VARCHAR(128 OCTETS) )       -- STAT_NAME \n"
        "      , CAST( knstat.ID AS NUMBER )                      -- SESS_ID \n"
        "      , CAST( knstat.VALUE AS NUMBER )                   -- STAT_VALUE \n"
        "   FROM  \n"
        "        FIXED_TABLE_SCHEMA.X$KN_SESS_STAT AS knstat    \n"
        "   WHERE knstat.CATEGORY = 11 \n"
        " UNION ALL \n"
        " SELECT  \n"
        "        CAST( smenv.NAME AS VARCHAR(128 OCTETS) )        -- STAT_NAME \n"
        "      , CAST( smenv.ID AS NUMBER )                       -- SESS_ID \n"
        "      , CAST( smenv.VALUE AS NUMBER )                    -- STAT_VALUE \n"
        "   FROM  \n"
        "        FIXED_TABLE_SCHEMA.X$SM_SESS_ENV AS smenv    \n"
        "   WHERE smenv.CATEGORY = 11 \n"
        " UNION ALL \n"
        " SELECT  \n"
        "        CAST( smstat.NAME AS VARCHAR(128 OCTETS) )       -- STAT_NAME \n"
        "      , CAST( smstat.ID AS NUMBER )                      -- SESS_ID \n"
        "      , CAST( smstat.VALUE AS NUMBER )                   -- STAT_VALUE \n"
        "   FROM  \n"
        "        FIXED_TABLE_SCHEMA.X$SM_SESS_STAT AS smstat    \n"
        "   WHERE smstat.CATEGORY = 11 \n"
        " UNION ALL \n"
        " SELECT  \n"
        "        CAST( exeenv.NAME AS VARCHAR(128 OCTETS) )       -- STAT_NAME \n"
        "      , CAST( exeenv.ID AS NUMBER )                      -- SESS_ID \n"
        "      , CAST( exeenv.VALUE AS NUMBER )                   -- STAT_VALUE \n"
        "   FROM  \n"
        "        FIXED_TABLE_SCHEMA.X$EXE_SESS_ENV AS exeenv    \n"
        "   WHERE exeenv.CATEGORY = 11 \n"
        " UNION ALL \n"
        " SELECT  \n"
        "        CAST( sqlenv.NAME AS VARCHAR(128 OCTETS) )       -- STAT_NAME \n"
        "      , CAST( sqlenv.ID AS NUMBER )                      -- SESS_ID \n"
        "      , CAST( sqlenv.VALUE AS NUMBER )                   -- STAT_VALUE \n"
        "   FROM  \n"
        "        FIXED_TABLE_SCHEMA.X$SQL_SESS_ENV AS sqlenv    \n"
        "   WHERE sqlenv.CATEGORY = 11 \n"
        " UNION ALL \n"
        " SELECT  \n"
        "        CAST( ssstat.NAME AS VARCHAR(128 OCTETS) )       -- STAT_NAME \n"
        "      , CAST( ssstat.ID AS NUMBER )                      -- SESS_ID \n"
        "      , CAST( ssstat.VALUE AS NUMBER )                   -- STAT_VALUE \n"
        "   FROM  \n"
        "        FIXED_TABLE_SCHEMA.X$SS_SESS_STAT AS ssstat    \n"
        "   WHERE ssstat.CATEGORY = 11 \n"
        " UNION ALL \n"
        " SELECT  \n"
        "        CAST( ssstmtstat.NAME AS VARCHAR(128 OCTETS) )   -- STAT_NAME \n"
        "      , CAST( ssstmtstat.SESSION_ID AS NUMBER )          -- SESS_ID \n"
        "      , CAST( SUM( ssstmtstat.VALUE ) AS NUMBER )        -- STAT_VALUE \n"
        "   FROM  \n"
        "        FIXED_TABLE_SCHEMA.X$SS_STMT_STAT AS ssstmtstat    \n"
        "   WHERE ssstmtstat.CATEGORY = 11 \n"
        "   GROUP BY NAME, SESSION_ID \n"
        " ORDER BY 2, 1 \n"
    }
    ,
    {
        /************************************************************
         * V$SESSION_SQL_STAT
         ************************************************************/

        "V$SESSION_SQL_STAT",
        
        " CREATE VIEW PERFORMANCE_VIEW_SCHEMA.V$SESSION_SQL_STAT \n"
        " (   \n"
        "        STAT_NAME \n"
        "      , SESS_ID \n"
        "      , STAT_VALUE \n"
        " ) \n"
        " AS  \n"
        " SELECT  \n"
        "        CAST( sqlsessenv.NAME AS VARCHAR(128 OCTETS) )                        -- STAT_NAME \n"
        "      , CAST( sqlsessenv.ID AS NUMBER )                                       -- SESS_ID \n"
        "      , CAST( sqlsessenv.VALUE AS NUMBER )                                    -- STAT_VALUE \n"
        "   FROM  \n"
        "        FIXED_TABLE_SCHEMA.X$SQL_SESS_ENV AS sqlsessenv \n"
        "   WHERE sqlsessenv.CATEGORY = 20 \n"
        " UNION ALL \n"
        " SELECT  \n"
        "        CAST( 'COMMAND: ' || sqlprocexec.STMT_TYPE AS VARCHAR(128 OCTETS) )   -- STAT_NAME \n"
        "      , CAST( sqlprocexec.ID AS NUMBER )                                      -- SESS_ID \n"
        "      , CAST( sqlprocexec.EXECUTE_COUNT AS NUMBER )                           -- STAT_VALUE \n"
        "   FROM  \n"
        "        FIXED_TABLE_SCHEMA.X$SQL_SESS_STAT_EXEC_STMT AS sqlprocexec \n"
        " ORDER BY 2, 1 \n"
    }
    ,
    {
        /************************************************************
         * V$SHM_SEGMENT
         ************************************************************/

        "V$SHM_SEGMENT",
        
        " CREATE VIEW PERFORMANCE_VIEW_SCHEMA.V$SHM_SEGMENT \n"
        " (   \n"
        "        SHM_NAME \n"
        "      , SHM_ID \n"
        "      , SHM_SIZE \n"
        "      , SHM_KEY \n"
        "      , SHM_SEQ \n"
        "      , SHM_ADDR \n"
        " ) \n"
        " AS  \n"
        " SELECT  \n"
        "        CAST( shmseg.NAME AS VARCHAR(32 OCTETS) )    -- SHM_NAME \n"
        "      , CAST( shmseg.ID AS NUMBER )                  -- SHM_ID \n"
        "      , CAST( shmseg.SIZE AS NUMBER )                -- SHM_SIZE \n"
        "      , CAST( shmseg.KEY AS NUMBER )                 -- SHM_KEY \n"
        "      , CAST( shmseg.SEQ AS NUMBER )                 -- SHM_SEQ \n"
        "      , CAST( shmseg.ADDR AS VARCHAR(32 OCTETS) )    -- SHM_ADDR \n"
        "   FROM  \n"
        "        FIXED_TABLE_SCHEMA.X$SHM_SEGMENT AS shmseg \n"
    }
    ,

    {
        /************************************************************
         * V$SQLFN_METADATA
         ************************************************************/

        "V$SQLFN_METADATA",

        " CREATE VIEW PERFORMANCE_VIEW_SCHEMA.V$SQLFN_METADATA \n"
        " (   \n"
        "        FUNC_NAME \n"
        "      , MINARGS \n"
        "      , MAXARGS \n"
        "      , IS_AGGREGATE \n"
        " ) \n"
        " AS  \n"
        " SELECT  \n"
        "        CAST( func.FUNC_NAME AS VARCHAR(128 OCTETS) )   -- FUNC_NAME \n"
        "      , CAST( func.MIN_ARG_COUNT AS NUMBER )            -- MINARGS \n"
        "      , CAST( func.MAX_ARG_COUNT AS NUMBER )            -- MAXARGS \n"
        "      , CAST( FALSE AS BOOLEAN )                        -- IS_AGGREGATE \n"
        "   FROM  \n"
        "        FIXED_TABLE_SCHEMA.X$BUILTIN_FUNCTION func \n"
        " UNION ALL \n"
        " SELECT  \n"
        "        CAST( aggr.FUNC_NAME AS VARCHAR(128 OCTETS) ) \n"
        "      , CAST( aggr.MIN_ARG_COUNT AS NUMBER ) \n"
        "      , CAST( aggr.MAX_ARG_COUNT AS NUMBER ) \n"
        "      , TRUE \n"
        "   FROM  \n"
        "        FIXED_TABLE_SCHEMA.X$BUILTIN_AGGREGATION aggr \n"
    }
    ,

    {
        /************************************************************
         * V$SQL_CACHE
         * - NOMOUNT, MOUNT 단계에서는 SYS 계정만 접속 가능함.
         ************************************************************/

        "V$SQL_CACHE",
        
        " CREATE VIEW PERFORMANCE_VIEW_SCHEMA.V$SQL_CACHE \n"
        " (   \n"
        "        SQL_HANDLE \n"
        "      , HASH_VALUE \n"
        "      , REF_COUNT \n"
        "      , CLOCK_ID \n"
        "      , PLAN_AGE \n"
        "      , USER_NAME \n"
        "      , BIND_PARAM_COUNT \n"
        "      , SQL_TEXT \n"
        "      , PLAN_COUNT \n"
        "      , PLAN_ID \n"
        "      , PLAN_SIZE \n"
        "      , PLAN_IS_ATOMIC \n"
        "      , PLAN_TEXT \n"
        " ) \n"
        " AS  \n"
        " SELECT  \n"
        "        CAST( sqlcache.SQL_HANDLE AS NUMBER )            -- SQL_HANDLE \n"
        "      , CAST( sqlcache.HASH_VALUE AS NUMBER )            -- HASH_VALUE \n"
        "      , CAST( sqlplan.REF_COUNT AS NUMBER )              -- REF_COUNT \n"
        "      , CAST( sqlcache.CLOCK_ID AS NUMBER )              -- CLOCK_ID \n"
        "      , CAST( sqlcache.AGE AS NUMBER )                   -- PLAN_AGE \n"
        "      , CAST( 'SYS' AS VARCHAR(128 OCTETS) )             -- USER_NAME \n"
        "      , CAST( sqlcache.BIND_COUNT AS NUMBER )            -- BIND_PARAM_COUNT \n"
        "      , CAST( sqlcache.SQL_STRING AS LONG VARCHAR )      -- SQL_TEXT \n"
        "      , CAST( sqlcache.PLAN_COUNT AS NUMBER )            -- PLAN_COUNT \n"
        "      , CAST( sqlplan.PLAN_IDX AS NUMBER )               -- PLAN_ID \n"
        "      , CAST( sqlplan.PLAN_SIZE AS NUMBER )              -- PLAN_SIZE \n"
        "      , CAST( sqlplan.IS_ATOMIC AS BOOLEAN )             -- PLAN_IS_ATOMIC \n"
        "      , CAST( sqlplan.PLAN_STRING AS LONG VARCHAR )      -- PLAN_TEXT \n"
        "   FROM  \n"
        "        FIXED_TABLE_SCHEMA.X$SQL_CACHE AS sqlcache \n"
        "        INNER JOIN \n"
        "        FIXED_TABLE_SCHEMA.X$SQL_CACHE_PLAN AS sqlplan \n"
        "        ON sqlcache.sql_handle = sqlplan.sql_handle \n"
        "   WHERE sqlcache.DROPPED = FALSE \n"
    }
    ,
    {
        /************************************************************
         * V$STATEMENT 
         ************************************************************/

        "V$STATEMENT",
        
        " CREATE VIEW PERFORMANCE_VIEW_SCHEMA.V$STATEMENT  \n"
        " (  \n"
        "        SESSION_ID \n"
        "      , STMT_ID \n"
        "      , STMT_VIEW_SCN \n"
        "      , SQL_TEXT \n"
        "      , START_TIME \n"
        " ) \n"
        " AS  \n"
        " SELECT  \n"
        "        CAST( xstmt.SESSION_ID AS NUMBER )                -- SESSION_ID \n"
        "      , CAST( xstmt.ID AS NUMBER )                        -- STMT_ID \n"
        "      , CAST( xstmt.VIEW_SCN AS NUMBER )                  -- STMT_VIEW_SCN \n"
        "      , CAST( xstmt.SQL_TEXT AS VARCHAR(1024 OCTETS) )    -- SQL_TEXT \n"
        "      , xstmt.START_TIME                                  -- START_TIME \n"
        "   FROM   \n"
        "        FIXED_TABLE_SCHEMA.X$STATEMENT    AS xstmt \n"
        "  UNION ALL \n"
        " SELECT \n"
        "        CAST( xcur.SESSION_ID AS NUMBER )                 -- SESSION_ID \n"
        "      , CAST( NULL AS NUMBER )                            -- STMT_ID \n"
        "      , CAST( xcur.VIEW_SCN AS NUMBER )                   -- STMT_VIEW_SCN \n"
        "      , CAST( xcur.CURSOR_QUERY AS VARCHAR(1024 OCTETS) ) -- SQL_TEXT \n"
        "      , xcur.OPEN_TIME                                    -- START_TIME \n"
        "   FROM   \n"
        "        FIXED_TABLE_SCHEMA.X$NAMED_CURSOR AS xcur \n"
        "  WHERE  \n"
        "        xcur.IS_OPEN = TRUE \n"
        " ORDER BY 1, 2 \n"
    }
    ,
    {
        /************************************************************
         * V$SPROPERTY 
         ************************************************************/

        "V$SPROPERTY",
        
        " CREATE VIEW PERFORMANCE_VIEW_SCHEMA.V$SPROPERTY  \n"
        " (  \n"
        "        PROPERTY_NAME \n"
        "      , DESCRIPTION \n"
        "      , DATA_TYPE \n"
        "      , STARTUP_PHASE \n"
        "      , VALUE_UNIT \n"
        "      , PROPERTY_VALUE \n"
        "      , PROPERTY_SOURCE \n"
        "      , INIT_VALUE \n"
        "      , INIT_SOURCE \n"
        "      , MIN_VALUE \n"
        "      , MAX_VALUE \n"
        "      , SES_MODIFIABLE \n"
        "      , SYS_MODIFIABLE \n"
        "      , IS_MODIFIABLE \n"
        " ) \n"
        " AS  \n"
        " SELECT  \n"
        "        CAST( PROPERTY_NAME AS VARCHAR(128 OCTETS) )  -- PROPERTY_NAME \n"
        "      , CAST( DESCRIPTION AS VARCHAR(2048 OCTETS) )   -- DESCRIPTION \n"
        "      , CAST( DATA_TYPE AS VARCHAR(32 OCTETS) )       -- DATA_TYPE \n"
        "      , CAST( STARTUP_PHASE AS VARCHAR(32 OCTETS) )   -- STARTUP_PHASE \n"
        "      , CAST( UNIT AS VARCHAR(32 OCTETS) )            -- VALUE_UNIT \n"
        "      , CAST( VALUE AS VARCHAR(2048 OCTETS) )         -- PROPERTY_VALUE \n"
        "      , CAST( SOURCE AS VARCHAR(32 OCTETS) )          -- PROPERTY_SOURCE \n"
        "      , CAST( INIT_VALUE AS VARCHAR(2048 OCTETS) )    -- INIT_VALUE \n"
        "      , CAST( INIT_SOURCE AS VARCHAR(32 OCTETS) )     -- INIT_SOURCE \n"
        "      , CAST( CASE WHEN DATA_TYPE = 'VARCHAR' THEN NULL \n"
        "                                              ELSE MIN  \n"
        "              END \n"
        "              AS NUMBER )                             -- MIN_VALUE \n"
        "      , CAST( CASE WHEN DATA_TYPE = 'VARCHAR' THEN NULL \n"
        "                                              ELSE MAX  \n"
        "              END \n"
        "              AS NUMBER )                             -- MAX_VALUE \n"
        "      , CAST( SES_MODIFIABLE AS VARCHAR(32 OCTETS) )  -- SES_MODIFIABLE \n"
        "      , CAST( SYS_MODIFIABLE AS VARCHAR(32 OCTETS) )  -- SYS_MODIFIABLE \n"
        "      , CAST( MODIFIABLE AS VARCHAR(32 OCTETS) )      -- IS_MODIFIABLE \n"
        "   FROM  \n"
        "        FIXED_TABLE_SCHEMA.X$SPROPERTY \n"
        "  WHERE  \n"
        "        DOMAIN = 'EXTERNAL' \n"
        " ORDER BY  \n"
        "       PROPERTY_ID \n"
    }
    ,

    {
        /************************************************************
         * V$SYSTEM_STAT
         ************************************************************/

        "V$SYSTEM_STAT",
        
        " CREATE VIEW PERFORMANCE_VIEW_SCHEMA.V$SYSTEM_STAT \n"
        " (   \n"
        "        STAT_NAME \n"
        "      , STAT_VALUE \n"
        "      , COMMENTS \n"
        " ) \n"
        " AS  \n"
        " SELECT  \n"
        "        CAST( kninfo.NAME AS VARCHAR(128 OCTETS) )       -- STAT_NAME \n"
        "      , CAST( kninfo.VALUE AS NUMBER )                   -- STAT_VALUE \n"
        "      , CAST( kninfo.COMMENTS AS VARCHAR(1024 OCTETS) )  -- COMMENTS \n"
        "   FROM  \n"
        "        FIXED_TABLE_SCHEMA.X$KN_SYSTEM_INFO AS kninfo    \n"
        "   WHERE kninfo.CATEGORY = 0 \n"
        " UNION ALL \n"
        " SELECT  \n"
        "        CAST( sminfo.NAME AS VARCHAR(128 OCTETS) )       -- STAT_NAME \n"
        "      , CAST( sminfo.VALUE AS NUMBER )                   -- STAT_VALUE \n"
        "      , CAST( sminfo.COMMENTS AS VARCHAR(1024 OCTETS) )  -- COMMENTS \n"
        "   FROM  \n"
        "        FIXED_TABLE_SCHEMA.X$SM_SYSTEM_INFO AS sminfo    \n"
        "   WHERE sminfo.CATEGORY = 0 \n"
        " UNION ALL \n"
        " SELECT  \n"
        "        CAST( exeinfo.NAME AS VARCHAR(128 OCTETS) )      -- STAT_NAME \n"
        "      , CAST( exeinfo.VALUE AS NUMBER )                  -- STAT_VALUE \n"
        "      , CAST( exeinfo.COMMENTS AS VARCHAR(1024 OCTETS) ) -- COMMENTS \n"
        "   FROM  \n"
        "        FIXED_TABLE_SCHEMA.X$EXE_SYSTEM_INFO AS exeinfo \n"
        "   WHERE exeinfo.CATEGORY = 0 \n"
        " UNION ALL \n"
        " SELECT  \n"
        "        CAST( sqlinfo.NAME AS VARCHAR(128 OCTETS) )      -- STAT_NAME \n"
        "      , CAST( sqlinfo.VALUE AS NUMBER )                  -- STAT_VALUE \n"
        "      , CAST( sqlinfo.COMMENTS AS VARCHAR(1024 OCTETS) ) -- COMMENTS \n"
        "   FROM  \n"
        "        FIXED_TABLE_SCHEMA.X$SQL_SYSTEM_INFO AS sqlinfo    \n"
        "   WHERE sqlinfo.CATEGORY = 0 \n"
    }
    ,
    {
        /************************************************************
         * V$SYSTEM_MEM_STAT
         ************************************************************/

        "V$SYSTEM_MEM_STAT",

        " CREATE VIEW PERFORMANCE_VIEW_SCHEMA.V$SYSTEM_MEM_STAT \n"
        " (   \n"
        "        STAT_NAME \n"
        "      , STAT_VALUE \n"
        "      , COMMENTS \n"
        " ) \n"
        " AS  \n"
        " SELECT  \n"
        "        CAST( kninfo.NAME AS VARCHAR(128 OCTETS) )       -- STAT_NAME \n"
        "      , CAST( kninfo.VALUE AS NUMBER )                   -- STAT_VALUE \n"
        "      , CAST( kninfo.COMMENTS AS VARCHAR(1024 OCTETS) )  -- COMMENTS \n"
        "   FROM  \n"
        "        FIXED_TABLE_SCHEMA.X$KN_SYSTEM_INFO AS kninfo    \n"
        "   WHERE kninfo.CATEGORY = 11 \n"
        " UNION ALL \n"
        " SELECT  \n"
        "        CAST( sminfo.NAME AS VARCHAR(128 OCTETS) )       -- STAT_NAME \n"
        "      , CAST( sminfo.VALUE AS NUMBER )                   -- STAT_VALUE \n"
        "      , CAST( sminfo.COMMENTS AS VARCHAR(1024 OCTETS) )  -- COMMENTS \n"
        "   FROM  \n"
        "        FIXED_TABLE_SCHEMA.X$SM_SYSTEM_INFO AS sminfo    \n"
        "   WHERE sminfo.CATEGORY = 11 \n"
        " UNION ALL \n"
        " SELECT  \n"
        "        CAST( exeinfo.NAME AS VARCHAR(128 OCTETS) )      -- STAT_NAME \n"
        "      , CAST( exeinfo.VALUE AS NUMBER )                  -- STAT_VALUE \n"
        "      , CAST( exeinfo.COMMENTS AS VARCHAR(1024 OCTETS) ) -- COMMENTS \n"
        "   FROM  \n"
        "        FIXED_TABLE_SCHEMA.X$EXE_SYSTEM_INFO AS exeinfo \n"
        "   WHERE exeinfo.CATEGORY = 11 \n"
        " UNION ALL \n"
        " SELECT  \n"
        "        CAST( sqlinfo.NAME AS VARCHAR(128 OCTETS) )      -- STAT_NAME \n"
        "      , CAST( sqlinfo.VALUE AS NUMBER )                  -- STAT_VALUE \n"
        "      , CAST( sqlinfo.COMMENTS AS VARCHAR(1024 OCTETS) ) -- COMMENTS \n"
        "   FROM  \n"
        "        FIXED_TABLE_SCHEMA.X$SQL_SYSTEM_INFO AS sqlinfo    \n"
        "   WHERE sqlinfo.CATEGORY = 11 \n"
    }
    ,

    {
        /************************************************************
         * V$SYSTEM_SQL_STAT
         ************************************************************/

        "V$SYSTEM_SQL_STAT",
        
        " CREATE VIEW PERFORMANCE_VIEW_SCHEMA.V$SYSTEM_SQL_STAT \n"
        " (   \n"
        "        STAT_NAME \n"
        "      , STAT_VALUE \n"
        "      , COMMENTS \n"
        " ) \n"
        " AS  \n"
        " SELECT  \n"
        "        CAST( sysinfo.NAME AS VARCHAR(128 OCTETS) )       -- STAT_NAME \n"
        "      , CAST( sysinfo.VALUE AS NUMBER )                   -- STAT_VALUE \n"
        "      , CAST( sysinfo.COMMENTS AS VARCHAR(1024 OCTETS) )  -- COMMENTS \n"
        "   FROM  \n"
        "        FIXED_TABLE_SCHEMA.X$SQL_SYSTEM_INFO AS sysinfo \n"
        "   WHERE sysinfo.CATEGORY = 20 \n"
        " UNION ALL \n"
        " SELECT  \n"
        "        CAST( 'COMMAND: ' || sysexec.STMT_TYPE AS VARCHAR(128 OCTETS) )   -- STAT_NAME \n"
        "      , CAST( sysexec.EXECUTE_COUNT AS NUMBER )                           -- STAT_VALUE \n"
        "      , CAST( 'execution count of the command' AS VARCHAR(1024 OCTETS) )  -- COMMENTS \n"
        "   FROM  \n"
        "        FIXED_TABLE_SCHEMA.X$SQL_SYSTEM_STAT_EXEC_STMT AS sysexec \n"
    }
    ,
    
    {
        /************************************************************
         * V$TABLES
         * - PerformanceViewSchema.sql 와 완전히 상이함
         ************************************************************/

        "V$TABLES",
        
        " CREATE VIEW PERFORMANCE_VIEW_SCHEMA.V$TABLES  \n"
        " (  \n"
        "        TABLE_OWNER \n"
        "      , TABLE_SCHEMA \n"
        "      , TABLE_NAME \n"
        "      , STARTUP_PHASE \n"
        "      , CREATED_TIME \n"
        "      , MODIFIED_TIME \n"
        "      , COMMENTS \n"
        " ) \n"
        " AS  \n"
        " SELECT  \n"
        "        CAST( 'SYS' AS VARCHAR(128 OCTETS) )                        -- TABLE_OWNER \n"
        "      , CAST( 'PERFORMANCE_VIEW_SCHEMA' AS VARCHAR(128 OCTETS) )    -- TABLE_SCHEMA \n"
        "      , CAST( tab.VIEW_NAME AS VARCHAR(128 OCTETS) )                -- TABLE_NAME \n"
        "      , CAST( tab.STARTUP_PHASE AS VARCHAR(32 OCTETS) )             -- STARTUP_PHASE \n"
        "      , CAST( NULL AS TIMESTAMP(2) WITHOUT TIME ZONE )              -- CREATED_TIME \n"
        "      , CAST( NULL AS TIMESTAMP(2) WITHOUT TIME ZONE )              -- MODIFIED_TIME \n"
        "      , CAST( NULL AS VARCHAR(1024 OCTETS) )                        -- COMMENTS \n"
        "   FROM   \n"
        "        FIXED_TABLE_SCHEMA.X$FIXED_VIEW tab   \n"
    }
    ,
    {
        NULL,
        NULL,
    }
};


/** @} qlgStartup */
