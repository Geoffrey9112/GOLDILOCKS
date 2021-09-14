/*******************************************************************************
 * knlBreakPoint.c
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
 * @file knlBreakPoint.c
 * @brief Kernel Layer Break Point Routines
 */

#include <stl.h>
#include <dtl.h>
#include <knlDef.h>
#include <knDef.h>
#include <knbBreakPoint.h>

stlChar * gKnlBpNames[KNL_BREAKPOINT_MAX + 1] = 
{
    "BP_QLLFETCHSQL_BEFORE_FETCH",
    "BP_SMNMPBDML_BEFORE_MOVE_TO_LEAF",
    "BP_CHECKPOINT_BEFORE_CHKPT_LOGGING",
    "BP_SMLINSERTRECORD_BEFORE_INSERT",
    "BP_ELDLOCKANDVALIDATETABLE4DDL_AFTER_LOCK",
    "BP_SMNMPBCREATE_AFTER_ALLOCHDR",
    "BP_SMNMPBCREATEFORTRUNCATE_AFTER_ALLOCHDR",
    "BP_SMDMPHCREATE_AFTER_ALLOCHDR",
    "BP_SMDMPHCREATEFORTRUNCATE_AFTER_ALLOCHDR",
    "BP_SMNMPBCHECKUNIQUENESS_BEFORE_FETCH_LEFT",
    "BP_SMNMPBCHECKUNIQUENESS_BEFORE_FETCH_RIGHT1",
    "BP_SMNMPBCHECKUNIQUENESS_BEFORE_FETCH_RIGHT2",
    "BP_QLTEXECUTECREATESPACE_AFTER_CREATE_TBS",
    "BP_QLTEXECUTECREATESPACE_AFTER_REFINE_CACHE",
    "BP_SMFADDDATAFILE_AFTER_LOGGING",
    "BP_SMFVALIDATEDATAFILES_AFTER_VALIDATION",
    "BP_QLTEXECUTEALTERSPACEADDFILE_AFTER_SMLADDDATAFILE",
    "BP_SMFDROPDATAFILE_AFTER_LOGGING",
    "BP_QLTEXECUTEALTERSPACEDROPFILE_AFTER_SMLDROPDATAFILE",
    "BP_QLTEXECUTEALTERSPACERENAME_AFTER_LOCK",
    "BP_QLTEXECUTEALTERSPACERENAME_AFTER_REFINECACHE",
    "BP_SMFONLINETBS_AFTER_PARALLELLOAD",
    "BP_SMNMPBFETCH_PREV_BEFORE_MOVE_PREV_PAGE",
    "BP_QLTEXECUTEALTERSPACEOFFLINE_AFTER_SMLOFFLINETABLESPACE",
    "BP_QLTEXECUTEALTERSPACEONLINE_AFTER_SMLONLINETABLESPACE",
    "BP_QLLCOMMIT_BEFORE_DICTIONARY_PENDING",
    "BP_QLLROLLBACK_BEFORE_DICTIONARY_PENDING",
    "BP_ELDCGETTABLEHANDLEBYNAME_BEFORE_SET_DICT_HANDLE",
    "BP_STARTUPDATABASE_BEFORE_ADDENVEVENT",
    "BP_STARTUPDATABASE_AFTER_ADDENVEVENT",
    "BP_QLTEXECUTEALTERSPACEONLINE_AFTER_SMLFINALIZEONLINETABLESPACE",
    "BP_SMLDELETERECORD_BEFORE_MODULEDELETE",
    "BP_SMLUPDATERECORD_BEFORE_MODULEUPDATE",
    "BP_SMRRECOVER_BEFORE_RECOVER",
    "BP_SMXLRESTRUCTUREUNDORELATION_AFTER_EXPAND_UNDO_RELATION",
    "BP_SMXLRESTRUCTUREUNDORELATION_AFTER_COMMIT",
    "BP_SMDMPHUPDATE_AFTER_COMPACT",
    "BP_SMFBACKUPTBSBEGIN_AFTER_SETTING",
    "BP_BACKUPWAITING_AFTER_SET",
    "BP_BACKUPWAITING_AFTER_RESET",
    "BP_SMRREADONLINEFILEBLOCKS_BEFORE_FIND_LOG_GROUP",
    "BP_SMRREADONLINEFILEBLOCKS_BEFORE_READ",
    "BP_SMRMEDIARECOVER_AFTER_RECOVERY",
    "BP_SMFMWRITELOGCREATE_BEFORE_UNDOLOGGING",
    "BP_SMFMWRITELOGCREATE_AFTER_UNDOLOGGING",
    "BP_SMFWRITELOGDATAFILE_BEFORE_UNDOLOGGING",
    "BP_SMFWRITELOGDATAFILE_AFTER_UNDOLOGGING",
    "BP_SMFWRITELOGTBS_BEFORE_UNDOLOGGING",
    "BP_SMFWRITELOGTBS_AFTER_UNDOLOGGING",
    "BP_SMFWRITELOGRENAMETBS_BEFORE_UNDOLOGGING",
    "BP_SMFWRITELOGRENAMETBS_AFTER_UNDOLOGGING",
    "BP_SHUTDOWNDATABASE_AFTER_PREVENTBACKUP",
    "BP_SMFCREATETBS_BEFORE_ADD_TBSINFO_TO_CONTROLFILE",
    "BP_SMDMPHCREATEUNDO_AFTER_SET_DROPPING",
    "BP_QLDEXECUTERECOVERPAGE_BEFORE_ACQUIRE_LOCK",
    "BP_QLDEXECUTERECOVERPAGE_AFTER_ACQUIRE_LOCK",
    "BP_QLG_VALIDATE_SQL_BEFORE",
    "BP_QLG_VALIDATE_SQL_COMPLETE",
    "BP_QLG_CODE_OPTIMIZE_SQL_BEFORE",
    "BP_QLG_CODE_OPTIMIZE_SQL_COMPLETE",
    "BP_QLG_DATA_OPTIMIZE_SQL_BEFORE",
    "BP_QLG_DATA_OPTIMIZE_SQL_COMPLETE",
    "BP_QLG_EXECUTE_SQL_BEFORE",
    "BP_QLG_EXECUTE_SQL_COMPLETE",
    "BP_QLL_EXECUTE_SQL_BEFORE",
    "BP_QLL_EXECUTE_SQL_COMPLETE",
    "BP_QLNC_QUERY_OPTIMIZER",
    "BP_QLL_OPTIMIZE_CODE_SQL_BEGIN",
    "BP_QLL_OPTIMIZE_CODE_SQL_INTERNAL_BEGIN",
    "BP_QLL_EXPLAIN_SQL_BEGIN",
    "BP_QLG_RECOMPILE_SQL_PHASE_CODE_OPTIMIZE",
    "BP_QLL_OPTIMIZE_DATA_SQL_COMPLETE",
    "BP_QLCR_EXECUTE_OPEN_CURSOR_BEFORE_QUERY_EXECUTE",
    "BP_SMFREWRITEDATAFILEHEADER_BEFORE_WRITE",
    "BP_QLL_VALIDATE_SQL_GET_TABLE_HANDLE",
    "BP_ELDC_PEND_COMMIT_DELETE_HASH_RELATED_COMPLETE",
    "BP_SMFDROPTBS_BEFORE_ADD_COMMIT_PENDING",
    
    NULL
};


stlStatus knlClearActionInfo( knlEnv   * aEnv )
{
    return knbClearActionInfo( aEnv );
}

stlStatus knlClearProcessInfo( knlEnv   * aEnv )
{
    return knbClearProcessInfo( aEnv );
}

stlStatus knlRegisterProcess( stlChar  * aName,
                                knlEnv   * aEnv )
{
    return knbRegisterProcess( aName, aEnv );
}

void knlFindProcIdFromName( stlChar         * aProcName,
                            stlInt32        * aProcId )
{
    return knbFindProcIdFromName( aProcName, aProcId );
}

void knlFindBreakpointIdFromName( stlChar         * aBpName,
                                  knlBreakPointId * aBpId )
{
    knbFindBreakpointIdFromName( aBpName, aBpId );
}

stlStatus knlRegisterAction( knlBreakPointId           aBpId,
                             knlBreakPointSessType     aSessType,
                             stlInt32                  aOwnerProcId,
                             stlInt32                  aSkipCount,
                             knlBreakPointAction       aActionType,
                             knlBreakPointPostAction   aPostActionType,
                             stlInt32                  aValue,
                             knlEnv                  * aEnv )
{
    return knbRegisterAction( aBpId,
                              aSessType,
                              aOwnerProcId,
                              aSkipCount,
                              aActionType,
                              aPostActionType,
                              aValue,
                              aEnv );
}

stlInt32  knlGetActionCount( )
{
    return knbGetActionCount();
}

stlInt32  knlGetProcCount( )
{
    return knbGetProcCount();
}

stlStatus knlDoBreakPoint( knlBreakPointId   aBpId,
                           knlEnv          * aEnv )
{
    return knbDoBreakPoint( aBpId, aEnv );
}

stlStatus knlBreakPointWaitToStart( knlEnv * aEnv )
{
    return knbBreakPointWaitToStart( aEnv );
}

/** @} */
