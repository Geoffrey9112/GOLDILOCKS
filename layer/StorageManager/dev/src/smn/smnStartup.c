/*******************************************************************************
 * smnStartup.c
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
#include <smf.h>
#include <smg.h>
#include <smr.h>
#include <smn.h>
#include <smp.h>
#include <smxl.h>
#include <smnDef.h>
#include <smnmpb.h>

/**
 * @file smnStartup.c
 * @brief Storage Manager Layer Index Startup Internal Routines
 */

extern knlFxTableDesc gSmnmpbKeyTableDesc;
extern knlFxTableDesc gSmnmpbHdrTableDesc;
extern knlFxTableDesc gSmnmpbStatisticsTableDesc;
extern knlFxTableDesc gSmnIndexHdrTableDesc;
extern knlFxTableDesc gSmnIndexCacheTableDesc;
extern knlFxTableDesc gSmnmpbVerifyOrderTableDesc;
extern knlFxTableDesc gSmnmpbVerifyStructureTableDesc;
extern smrRedoVector gSmnRedoVectors[];
extern smxlUndoFunc gSmnUndoFuncs[];
extern smrMtxUndoFunc gSmnMtxUndoFuncs[];

/**
 * @addtogroup smn
 * @{
 */

/**
 * @brief Index Component를 NO MOUNT 모드로 전이한다.
 * @param[out] aWarmupEntry Index Component의 Warmup Entry 포인터 
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smnStartupNoMount( void  ** aWarmupEntry,
                             smlEnv * aEnv )
{
    smrRegisterRedoVectors( SMG_COMPONENT_INDEX,
                            gSmnRedoVectors );
    
    smxlRegisterUndoFuncs( SMG_COMPONENT_INDEX, 
                           gSmnUndoFuncs );

    STL_TRY( smnRegisterFxTables( aEnv ) == STL_SUCCESS );
    
    smgRegisterPendOp( SMG_PEND_CREATE_INDEX,
                       smnmpbCreatePending );
    smgRegisterPendOp( SMG_PEND_DROP_INDEX,
                       smnmpbDropPending );
    
    *aWarmupEntry = NULL;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Index Component를 OPEN 모드로 전이한다.
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smnStartupOpen( smlEnv * aEnv )
{
    if( smfGetServerState() != SML_SERVER_STATE_NONE )
    {
        STL_TRY( knlLogMsg( NULL,
                            KNL_ENV(aEnv),
                            KNL_LOG_LEVEL_INFO,
                            "[STARTUP-SM] REBUILD INDEXES\n" )
                 == STL_SUCCESS );
    
        STL_TRY( smnRebuildAtStartup( aEnv ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Index Component를 Warmup 시킨다.
 * @param[in] aWarmupEntry Index Component의 Warmup Entry 포인터 
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smnWarmup( void   * aWarmupEntry,
                     smlEnv * aEnv )
{
    smrRegisterRedoVectors( SMG_COMPONENT_INDEX,
                            gSmnRedoVectors );
    
    smxlRegisterUndoFuncs( SMG_COMPONENT_INDEX, 
                           gSmnUndoFuncs );

    STL_TRY( smnRegisterFxTables( aEnv ) == STL_SUCCESS );

    smgRegisterPendOp( SMG_PEND_CREATE_INDEX,
                       smnmpbCreatePending );
    smgRegisterPendOp( SMG_PEND_DROP_INDEX,
                       smnmpbDropPending );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Index Component를 Bootdown 시킨다.
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smnBootdown( smlEnv * aEnv )
{
    return STL_SUCCESS;
}

/**
 * @brief Index Component를 Cooldown 시킨다.
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smnCooldown( smlEnv * aEnv )
{
    return STL_SUCCESS;
}

/**
 * @brief Index Component를 Close 시킨다.
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smnShutdownClose( smlEnv * aEnv )
{
    return STL_SUCCESS;
}

/**
 * @brief Fixed Table 들을 등록함
 * @param[in]  aEnv   Environment
 * @remarks
 * NO_MOUNT 단계와 Warm-Up 단계에서 동일한 순서로 등록되어야 함.
 */
stlStatus smnRegisterFxTables( smlEnv * aEnv )
{
    STL_TRY( knlRegisterFxTable( &gSmnmpbKeyTableDesc,
                                 (knlEnv*)aEnv )
             == STL_SUCCESS );
    STL_TRY( knlRegisterFxTable( &gSmnmpbHdrTableDesc,
                                 (knlEnv*)aEnv )
             == STL_SUCCESS );
    STL_TRY( knlRegisterFxTable( &gSmnmpbStatisticsTableDesc,
                                 (knlEnv*)aEnv )
             == STL_SUCCESS );
    STL_TRY( knlRegisterFxTable( &gSmnIndexHdrTableDesc,
                                 (knlEnv*)aEnv )
             == STL_SUCCESS );
    STL_TRY( knlRegisterFxTable( &gSmnIndexCacheTableDesc,
                                 (knlEnv*)aEnv )
             == STL_SUCCESS );
    STL_TRY( knlRegisterFxTable( &gSmnmpbVerifyOrderTableDesc,
                                 (knlEnv*)aEnv )
             == STL_SUCCESS );
    STL_TRY( knlRegisterFxTable( &gSmnmpbVerifyStructureTableDesc,
                                 (knlEnv*)aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smnInternalError( smpHandle      * aPageHandle,
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
