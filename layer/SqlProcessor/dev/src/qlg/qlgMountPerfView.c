/*******************************************************************************
 * qlgMountPerfView.c
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
 * @file qlgMountPerfView.c 
 * @brief SQL Processor Layer MOUNT phase Performance View
 */

#include <qll.h>
#include <qlgStartup.h>

/**
 * @addtogroup qlgStartup
 * @{
 */


/**
 * @brief MOUNT 단계의 Performance View Definition
 * @remarks
 * - NOMOUNT 단계에 정의되어 있지 않아야 함.
 * - OPEN 단계의 $GOLDILOCKS_HOME/admin/PerformanceView.sql 에 정의되어 있어야 함.
 */
qlgNonServicePerfView gQlgMountPerfViewDefinition[] =
{
    {
        /************************************************************
         * V$ARCHIVELOG
         ************************************************************/

        "V$ARCHIVELOG",
        
        " CREATE VIEW PERFORMANCE_VIEW_SCHEMA.V$ARCHIVELOG \n"
        " (   \n"
        "        ARCHIVELOG_MODE \n"
        "      , LAST_ARCHIVED_LOG \n"
        "      , ARCHIVELOG_DIR \n"
        "      , ARCHIVELOG_FILE_PREFIX \n"
        " ) \n"
        " AS  \n"
        " SELECT  \n"
        "        CAST( arclog.ARCHIVELOG_MODE AS VARCHAR(32 OCTETS) )        -- ARCHIVELOG_MODE \n"
        "      , CAST( arclog.LAST_INACTIVATED_LOGFILE_SEQ_NO AS NUMBER )    -- LAST_ARCHIVED_LOG \n"
        "      , CAST( arclog.ARCHIVELOG_DIR_1 AS VARCHAR(1024 OCTETS) )     -- ARCHIVELOG_DIR \n"
        "      , CAST( arclog.ARCHIVELOG_FILE AS VARCHAR(128 OCTETS) )       -- ARCHIVELOG_FILE_PREFIX \n"
        "   FROM  \n"
        "        FIXED_TABLE_SCHEMA.X$ARCHIVELOG AS arclog \n"
    }
    ,

    {
        /************************************************************
         * V$BACKUP
         ************************************************************/

        "V$BACKUP",
        
        " CREATE VIEW PERFORMANCE_VIEW_SCHEMA.V$BACKUP \n"
        " (   \n"
        "        TBS_NAME \n"
        "      , BACKUP_STATUS \n"
        "      , BACKUP_LSN \n"
        " ) \n"
        " AS  \n"
        " SELECT  \n"
        "        CAST( backup.NAME AS VARCHAR(128 OCTETS) ) AS TBS_NAME         -- TBS_NAME \n"
        "      , CAST( backup.STATUS AS VARCHAR(16 OCTETS) ) AS BACKUP_STATUS   -- BACKUP_STATUS \n"
        "      , CAST( datafile.BACKUP_LSN AS NUMBER )                          -- BACKUP_LSN \n"
        "   FROM  \n"
        "        FIXED_TABLE_SCHEMA.X$BACKUP AS backup \n"
        "        LEFT OUTER JOIN \n"
        "        ( SELECT TABLESPACE_ID, MIN(CHECKPOINT_LSN) AS BACKUP_LSN \n"
        "            FROM FIXED_TABLE_SCHEMA.X$DATAFILE \n"
        "          GROUP BY TABLESPACE_ID ) AS datafile \n"
        "        ON backup.ID = datafile.TABLESPACE_ID \n"
        "      \n"
    }
    ,    

    {
        /************************************************************
         * V$CONTROLFILE
         ************************************************************/

        "V$CONTROLFILE",

        " CREATE VIEW PERFORMANCE_VIEW_SCHEMA.V$CONTROLFILE \n"
        " (   \n"
        "        STATUS \n"
        "      , CONTROLFILE_NAME \n"
        "      , LAST_CHECKPOINT_LSN \n"
        "      , IS_PRIMARY \n"
        " ) \n"
        " AS  \n"
        " SELECT  \n"
        "        ctrl.CONTROLFILE_STATE                 -- STATUS \n"
        "      , ctrl.CONTROLFILE_NAME                  -- CONTROLFILE_NAME \n"
        "      , CAST( ctrl.CHECKPOINT_LSN AS NUMBER )  -- LAST_CHECKPOINT_LSN \n"
        "      , ctrl.IS_PRIMARY                        -- IS_PRIMARY \n"
        "   FROM  \n"
        "        FIXED_TABLE_SCHEMA.X$CONTROLFILE AS ctrl \n"
    }
    ,

    {
        /************************************************************
         * V$DATAFILE
         ************************************************************/
        
        "V$DATAFILE",
        
        " CREATE VIEW PERFORMANCE_VIEW_SCHEMA.V$DATAFILE \n"
        " (   \n"
        "        TBS_NAME \n"
        "      , DATAFILE_NAME \n"
        "      , CHECKPOINT_LSN \n"
        "      , CREATION_TIME \n"
        "      , FILE_SIZE \n"
        " ) \n"
        " AS  \n"
        " SELECT  \n"
        "        ( SELECT CAST( NAME AS VARCHAR(128 OCTETS) ) \n"
        "            FROM FIXED_TABLE_SCHEMA.X$TABLESPACE \n"
        "            WHERE ID = datafile.TABLESPACE_ID )            -- TBS_NAME \n"
        "      , CAST( datafile.PATH AS VARCHAR(1024 OCTETS) )      -- DATAFILE_NAME \n"
        "      , CAST( datafile.CHECKPOINT_LSN AS NUMBER )          -- CHECKPOINT_LSN \n"
        "      , datafile.CREATION_TIME                             -- CREATION_TIME \n"
        "      , CAST( datafile.SIZE AS NUMBER )                    -- FILE_SIZE \n"
        "   FROM  \n"
        "        FIXED_TABLE_SCHEMA.X$DATAFILE AS datafile \n"
        "   WHERE datafile.STATE = 'CREATED' \n"
    }
    ,

    {
        /************************************************************
         * V$INCREMENTAL_BACKUP
         ************************************************************/

        "V$INCREMENTAL_BACKUP",
        
        " CREATE VIEW PERFORMANCE_VIEW_SCHEMA.V$INCREMENTAL_BACKUP \n"
        " (   \n"
        "        BACKUP_NAME \n"
        "      , BACKUP_SCOPE \n"
        "      , INCREMENTAL_LEVEL \n"
        "      , INCREMENTAL_TYPE \n"
        "      , LSN \n"
        "      , BEGIN_TIME \n"
        "      , COMPLETION_TIME \n"
        " ) \n"
        " AS  \n"
        " SELECT  \n"
        "        CAST( backup.BACKUP_FILE AS VARCHAR(1024 OCTETS) )     -- BACKUP_NAME \n"
        "      , CAST( backup.BACKUP_OBJECT AS VARCHAR(128 OCTETS) )    -- BACKUP_SCOPE \n"
        "      , CAST( backup.BACKUP_LEVEL AS NUMBER )                  -- INCREMENTAL_LEVEL \n"
        "      , CAST( backup.BACKUP_OPTION AS VARCHAR(32 OCTETS) )     -- INCREMENTAL_TYPE \n"
        "      , CAST( backup.BACKUP_LSN AS NUMBER )                    -- LSN \n"
        "      , BEGIN_TIME                                             -- BEGIN_TIME \n"
        "      , COMPLETION_TIME                                        -- COMPLETION_TIME \n"
        "   FROM  \n"
        "        FIXED_TABLE_SCHEMA.X$CONTROLFILE_BACKUP_SECTION AS backup \n"
        "        LEFT OUTER JOIN \n"
        "        FIXED_TABLE_SCHEMA.X$CONTROLFILE AS ctrlfile \n"
        "        ON backup.CONTROLFILE_NAME = ctrlfile.CONTROLFILE_NAME \n"
        "   WHERE \n"
        "        ctrlfile.IS_PRIMARY = TRUE \n"
    }
    ,

    {
        /************************************************************
         * V$LOGFILE
         ************************************************************/
        
        "V$LOGFILE",
        
        " CREATE VIEW PERFORMANCE_VIEW_SCHEMA.V$LOGFILE \n"
        " (   \n"
        "        GROUP_ID \n"
        "      , FILE_NAME \n"
        "      , GROUP_STATE \n"
        "      , FILE_SEQ \n"
        "      , FILE_SIZE \n"
        " ) \n"
        " AS  \n"
        " SELECT  \n"
        "        CAST( loggrp.GROUP_ID AS NUMBER )              -- GROUP_ID \n"
        "      , CAST( logmem.NAME AS VARCHAR(1024 OCTETS) )    -- FILE_NAME \n"
        "      , CAST( loggrp.STATE AS VARCHAR(32 OCTETS) )     -- GROUP_STATE \n"
        "      , CAST( loggrp.FILE_SEQ_NO AS NUMBER )           -- FILE_SEQ \n"
        "      , CAST( loggrp.FILE_SIZE AS NUMBER )             -- FILE_SIZE \n"
        "   FROM  \n"
        "        FIXED_TABLE_SCHEMA.X$LOG_GROUP AS loggrp \n"
        "        LEFT OUTER JOIN    \n"
        "        FIXED_TABLE_SCHEMA.X$LOG_MEMBER AS logmem \n"
        "        ON loggrp.GROUP_ID = logmem.GROUP_ID \n"
    }
    ,

    {
        /************************************************************
         * V$TABLESPACE
         ************************************************************/

        "V$TABLESPACE",
        
        " CREATE VIEW PERFORMANCE_VIEW_SCHEMA.V$TABLESPACE \n"
        " (   \n"
        "        TBS_NAME \n"
        "      , TBS_ID \n"
        "      , TBS_ATTR \n"
        "      , IS_LOGGING \n"
        "      , IS_ONLINE \n"
        "      , OFFLINE_STATE \n"
        "      , EXTENT_SIZE \n"
        "      , PAGE_SIZE \n"
        " ) \n"
        " AS  \n"
        " SELECT  \n"
        "        CAST( tbs.NAME AS VARCHAR(128 OCTETS) )            -- TBS_NAME \n"
        "      , CAST( tbs.ID AS NUMBER )                           -- TBS_ID \n"
        "      , CAST( tbs.ATTR AS VARCHAR(128 OCTETS) )            -- TBS_ATTR \n"
        "      , tbs.LOGGING                                        -- IS_LOGGING \n"
        "      , tbs.ONLINE                                         -- IS_ONLINE \n"
        "      , CAST( tbs.OFFLINE_STATE AS VARCHAR(32 OCTETS) )    -- OFFLINE_STATE \n"
        "      , CAST( tbs.EXTSIZE AS NUMBER )                      -- EXTENT_SIZE \n"
        "      , CAST( tbs.PAGE_SIZE AS NUMBER )                    -- PAGE_SIZE \n"
        "   FROM  \n"
        "        FIXED_TABLE_SCHEMA.X$TABLESPACE AS tbs \n"
        "   WHERE tbs.STATE = 'CREATED' \n"
    }
    ,

    {
        /************************************************************
         * V$DB_FILE
         ************************************************************/

        "V$DB_FILE",
        
        " CREATE VIEW PERFORMANCE_VIEW_SCHEMA.V$DB_FILE \n"
        " (   \n"
        "        FILE_NAME \n"
        "      , FILE_TYPE \n"
        " ) \n"
        " AS  \n"
        " SELECT  \n"
        "        CAST( dbf.PATH AS VARCHAR(1024 OCTETS) )           -- FILE_NAME \n"
        "      , CAST( dbf.TYPE AS VARCHAR(16 OCTETS) )             -- FILE_TYPE \n"
        "   FROM  \n"
        "        FIXED_TABLE_SCHEMA.X$DB_FILE AS dbf \n"
    }
    ,

    {
        NULL,
        NULL,
    }
};


/** @} qlgStartup */
