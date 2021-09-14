/*******************************************************************************
 * smdStartup.c
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

#include <stl.h>
#include <dtl.h>
#include <knl.h>
#include <scl.h>
#include <smlDef.h>
#include <smDef.h>
#include <smrDef.h>
#include <smg.h>
#include <smp.h>
#include <smr.h>
#include <smxl.h>
#include <smd.h>
#include <smdmphDef.h>
#include <smdmph.h>
#include <smdmpp.h>

/**
 * @file smdStartup.c
 * @brief Storage Manager Layer Table Startup Internal Routines
 */

smdWarmupEntry * gSmdWarmupEntry;
extern smrRedoVector gSmdRedoVectors[];
extern smxlUndoFunc gSmdUndoFuncs[];
extern knlFxTableDesc gSmdmphOffsetTblHdrTableDesc;
extern knlFxTableDesc gSmdmphRowTableDesc;
extern knlFxTableDesc gSmdmphLogicalHdrTableDesc;
extern knlFxTableDesc gSmdTableCacheTableDesc;
extern knlFxTableDesc gSmdmifFxTableListTableDesc;
extern knlFxTableDesc gSmdmifFxColumnListTableDesc;
extern knlFxTableDesc gSmdmifFxBlockListTableDesc;
extern knlFxTableDesc gSmdmifFxBlockDumpTableDesc;
extern knlFxTableDesc gSmdmihFxTablesTableDesc;
extern knlFxTableDesc gSmdmihFxRowTableDesc;
extern knlFxTableDesc gSmdmihFxDirTableDesc;

/**
 * @addtogroup smd
 * @{
 */

/**
 * @brief Table Component를 NO MOUNT 모드로 전이한다.
 * @param[out] aWarmupEntry Table Component의 Warmup Entry 포인터 
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smdStartupNoMount( void  ** aWarmupEntry,
                            smlEnv * aEnv )
{
    smdWarmupEntry * sWarmupEntry;
    
    STL_TRY( knlCacheAlignedAllocFixedStaticArea( STL_SIZEOF( smdWarmupEntry ),
                                                  (void**)&sWarmupEntry,
                                                  (knlEnv*)aEnv ) == STL_SUCCESS );

    gSmdWarmupEntry = sWarmupEntry;
    *aWarmupEntry = (void*)sWarmupEntry;

    gSmdWarmupEntry->mVersionConflictCount = 0;
    
    STL_TRY( smdRegisterFxTables( aEnv ) == STL_SUCCESS );

    smrRegisterRedoVectors( SMG_COMPONENT_TABLE,
                            gSmdRedoVectors );

    smxlRegisterUndoFuncs( SMG_COMPONENT_TABLE, 
                           gSmdUndoFuncs );
   
    smgRegisterPendOp( SMG_PEND_CREATE_TABLE,
                       smdCreatePending );
    smgRegisterPendOp( SMG_PEND_DROP_TABLE,
                       smdDropPending );
    smgRegisterPendOp( SMG_PEND_FREE_LOCK,
                       smdFreeLockPending );
    smgRegisterPendOp( SMG_PEND_SET_SEGMENT,
                       smdSetSegmentPending );
    smgRegisterPendOp( SMG_PEND_DROP_PENDING_TABLE,
                       smdmppDropPending );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Table Component를 Warmup 시킨다.
 * @param[in] aWarmupEntry Table Component의 Warmup Entry 포인터 
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smdWarmup( void   * aWarmupEntry,
                     smlEnv * aEnv )
{
    gSmdWarmupEntry = (smdWarmupEntry*)aWarmupEntry;

    STL_TRY( smdRegisterFxTables( aEnv ) == STL_SUCCESS );
    
    smrRegisterRedoVectors( SMG_COMPONENT_TABLE,
                            gSmdRedoVectors );

    smxlRegisterUndoFuncs( SMG_COMPONENT_TABLE,
                           gSmdUndoFuncs );
   
    smgRegisterPendOp( SMG_PEND_CREATE_TABLE,
                       smdCreatePending );
    smgRegisterPendOp( SMG_PEND_DROP_TABLE,
                       smdDropPending );
    smgRegisterPendOp( SMG_PEND_FREE_LOCK,
                       smdFreeLockPending );
    smgRegisterPendOp( SMG_PEND_SET_SEGMENT,
                       smdSetSegmentPending );
    smgRegisterPendOp( SMG_PEND_DROP_PENDING_TABLE,
                       smdmppDropPending );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Table Component를 Cooldown 시킨다.
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smdCooldown( smlEnv * aEnv )
{
    return STL_SUCCESS;
}

/**
 * @brief Table Component를 Bootdown 시킨다.
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smdBootdown( smlEnv * aEnv )
{
    return STL_SUCCESS;
}


/**
 * @brief Fixed Table 들을 등록함
 * @param[in]  aEnv   Environment
 * @remarks
 * NO_MOUNT 단계와 Warm-Up 단계에서 동일한 순서로 등록되어야 함.
 */
stlStatus smdRegisterFxTables( smlEnv * aEnv )
{
    STL_TRY( knlRegisterFxTable( &gSmdTableCacheTableDesc,
                                 (knlEnv*)aEnv )
             == STL_SUCCESS );
    STL_TRY( knlRegisterFxTable( &gSmdmphOffsetTblHdrTableDesc,
                                 (knlEnv*)aEnv )
             == STL_SUCCESS );
    STL_TRY( knlRegisterFxTable( &gSmdmphRowTableDesc,
                                 (knlEnv*)aEnv )
             == STL_SUCCESS );
    STL_TRY( knlRegisterFxTable( &gSmdmphLogicalHdrTableDesc,
                                 (knlEnv*)aEnv )
             == STL_SUCCESS );
    STL_TRY( knlRegisterFxTable( &gSmdmifFxTableListTableDesc,
                                 (knlEnv*)aEnv )
             == STL_SUCCESS );
    STL_TRY( knlRegisterFxTable( &gSmdmifFxColumnListTableDesc,
                                 (knlEnv*)aEnv )
             == STL_SUCCESS );
    STL_TRY( knlRegisterFxTable( &gSmdmifFxBlockListTableDesc,
                                 (knlEnv*)aEnv )
             == STL_SUCCESS );
    STL_TRY( knlRegisterFxTable( &gSmdmifFxBlockDumpTableDesc,
                                 (knlEnv*)aEnv )
             == STL_SUCCESS );
    STL_TRY( knlRegisterFxTable( &gSmdmihFxTablesTableDesc,
                                 (knlEnv*)aEnv )
             == STL_SUCCESS );
    STL_TRY( knlRegisterFxTable( &gSmdmihFxRowTableDesc,
                                 (knlEnv*)aEnv )
             == STL_SUCCESS );
    STL_TRY( knlRegisterFxTable( &gSmdmihFxDirTableDesc,
                                 (knlEnv*)aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smdInternalError( smpHandle      * aPageHandle,
                            smlEnv         * aEnv,
                            const stlChar  * aFormat,
                            ... )
{
    stlVarList sVarArgList;
    stlInt32   sMsgSize;

    if( aFormat != NULL )
    {
        stlVarStart( sVarArgList, aFormat );
    
        /**
         * formatting된 문자열의 길이 계산
         */
    
        sMsgSize = stlGetVprintfBufferSize( aFormat, sVarArgList );
        stlVarEnd( sVarArgList );
    
        stlVarStart( sVarArgList, aFormat );
        (void) knlVarLogMsg( NULL,  /* aLogger */
                             KNL_ENV(aEnv),
                             KNL_LOG_LEVEL_ABORT,
                             sMsgSize,
                             aFormat,
                             sVarArgList );
        stlVarEnd( sVarArgList );
    
        (void) knlVarLogMsg( NULL,  /* aLogger */
                             KNL_ENV(aEnv),
                             KNL_LOG_LEVEL_ABORT,
                             sMsgSize,
                             aFormat,
                             sVarArgList );
    }

    /**
     * Page Body 출력
     */
    (void) knlLogMsg( NULL,  /* aLogger */
                      KNL_ENV(aEnv),
                      KNL_LOG_LEVEL_ABORT,
                      "PAGE ID : %d\nPAGE : \n%s\n",
                      smpGetPageId( aPageHandle ),
                      knlDumpBinaryForAssert( SMP_FRAME(aPageHandle),
                                              SMP_PAGE_SIZE,
                                              &aEnv->mLogHeapMem,
                                              KNL_ENV(aEnv) ) );

    (void) knlLogCallStack( NULL, /* aLogger */
                            KNL_ENV(aEnv) );

    stlPushError( STL_ERROR_LEVEL_ABORT,
                  SML_ERRCODE_SEGMENT_CORRUPTED,
                  NULL,
                  KNL_ERROR_STACK(aEnv),
                  smpGetSegmentId(aPageHandle) );

    STL_DASSERT( STL_FALSE );
    
    return STL_FAILURE;
}



/** @} */
