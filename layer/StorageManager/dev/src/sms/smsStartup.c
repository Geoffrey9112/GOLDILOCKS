/*******************************************************************************
 * smsStartup.c
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
#include <smrDef.h>
#include <smr.h>
#include <sms.h>
#include <smxl.h>
#include <smsDef.h>
#include <smsmbDef.h>
#include <smsmbSegment.h>

/**
 * @file smsStartup.c
 * @brief Storage Manager Layer Segment Startup Internal Routines
 */

extern knlFxTableDesc gSmsmbBitmapTableDesc;
extern knlFxTableDesc gSmsmbBitmapHdrTableDesc;
extern knlFxTableDesc gSmsmcExtentTableDesc;
extern knlFxTableDesc gSmsSegmentTableDesc;
extern smrRedoVector  gSmsRedoVectors[];
extern smxlUndoFunc   gSmsUndoFuncs[];
smsWarmupEntry * gSmsWarmupEntry;

/**
 * @addtogroup sms
 * @{
 */

/**
 * @brief Segment Component를 NO MOUNT 모드로 전이한다.
 * @param[out] aWarmupEntry Segment Component의 Warmup Entry 포인터 
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smsStartupNoMount( void  ** aWarmupEntry,
                             smlEnv * aEnv )
{
    smsWarmupEntry * sWarmupEntry;
    
    smrRegisterRedoVectors( SMG_COMPONENT_SEGMENT,
                            gSmsRedoVectors );
    
    smxlRegisterUndoFuncs( SMG_COMPONENT_SEGMENT, 
                           gSmsUndoFuncs );

    STL_TRY( smsRegisterFxTables( aEnv ) == STL_SUCCESS );
    
    STL_TRY( knlAllocFixedStaticArea( STL_SIZEOF( smsWarmupEntry ),
                                      (void**)&sWarmupEntry,
                                      (knlEnv*)aEnv ) == STL_SUCCESS );

    smgRegisterPendOp( SMG_PEND_UNUSABLE_SEGMENT,
                       smsUnusablePending );
    smgRegisterPendOp( SMG_PEND_USABLE_SEGMENT,
                       smsUsablePending );
    
    STL_RING_INIT_HEADLINK( &sWarmupEntry->mIrrecoverableSegmentList,
                            STL_OFFSETOF( smsIrrecoverableSegment, mSegmentLink ) );
    
    sWarmupEntry->mSegmentMapHandle = NULL;
    gSmsWarmupEntry = sWarmupEntry;
    *aWarmupEntry = (void*)sWarmupEntry;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Segment Component를 MOUNT 모드로 전이한다.
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smsStartupMount( smlEnv * aEnv )
{
    STL_RING_INIT_HEADLINK( &gSmsWarmupEntry->mIrrecoverableSegmentList,
                            STL_OFFSETOF( smsIrrecoverableSegment, mSegmentLink ) );

    return STL_SUCCESS;
}

/**
 * @brief Segment Component를 OPEN 모드로 전이한다.
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smsStartupOpen( smlEnv * aEnv )
{
    smsIrrecoverableSegment * sSegment;
    smsIrrecoverableSegment * sNextSegment;
    
    STL_RING_FOREACH_ENTRY_SAFE( &(gSmsWarmupEntry->mIrrecoverableSegmentList),
                                 sSegment,
                                 sNextSegment )
    {
        STL_TRY( smgFreeShmMem4Mount( sSegment,
                                      aEnv )
                 == STL_SUCCESS );
    }
    
    STL_RING_INIT_HEADLINK( &gSmsWarmupEntry->mIrrecoverableSegmentList,
                            STL_OFFSETOF( smsIrrecoverableSegment, mSegmentLink ) );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Segment Component를 Warmup 시킨다.
 * @param[in] aWarmupEntry Segment Component의 Warmup Entry 포인터 
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smsWarmup( void   * aWarmupEntry,
                     smlEnv * aEnv )
{
    smrRegisterRedoVectors( SMG_COMPONENT_SEGMENT,
                            gSmsRedoVectors );
    
    smxlRegisterUndoFuncs( SMG_COMPONENT_SEGMENT, 
                           gSmsUndoFuncs );

    STL_TRY( smsRegisterFxTables( aEnv ) == STL_SUCCESS );
    
    smgRegisterPendOp( SMG_PEND_UNUSABLE_SEGMENT,
                       smsUnusablePending );
    smgRegisterPendOp( SMG_PEND_USABLE_SEGMENT,
                       smsUsablePending );
    
    gSmsWarmupEntry = (smsWarmupEntry*)aWarmupEntry;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Segment Component를 Cooldown 시킨다.
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smsCooldown( smlEnv * aEnv )
{
    return STL_SUCCESS;
}

/**
 * @brief Segment Component를 Bootdown 시킨다.
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smsBootdown( smlEnv * aEnv )
{
    return STL_SUCCESS;
}

/**
 * @brief Segment Component를 CLOSE 시킨다.
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smsShutdownClose( void * aEnv )
{
    return STL_SUCCESS;
}


/**
 * @brief Fixed Table 들을 등록함
 * @param[in]  aEnv   Environment
 * @remarks
 * NO_MOUNT 단계와 Warm-Up 단계에서 동일한 순서로 등록되어야 함.
 */
stlStatus smsRegisterFxTables( smlEnv * aEnv )
{
    STL_TRY( knlRegisterFxTable( &gSmsSegmentTableDesc,
                                 (knlEnv*)aEnv )
             == STL_SUCCESS );
    STL_TRY( knlRegisterFxTable( &gSmsmbBitmapTableDesc,
                                 (knlEnv*)aEnv )
             == STL_SUCCESS );
    STL_TRY( knlRegisterFxTable( &gSmsmbBitmapHdrTableDesc,
                                 (knlEnv*)aEnv )
             == STL_SUCCESS );
    STL_TRY( knlRegisterFxTable( &gSmsmcExtentTableDesc,
                                 (knlEnv*)aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smsInternalError( void           * aSegmentHandle,
                            smlEnv         * aEnv,
                            const stlChar  * aFormat,
                            ... )
{
    stlVarList sVarArgList;
    stlInt32   sMsgSize;

    stlVarStart( sVarArgList, aFormat );
    
     /*
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
    
    (void) knlLogCallStack( NULL, /* aLogger */
                            KNL_ENV(aEnv) );

    stlPushError( STL_ERROR_LEVEL_ABORT,
                  SML_ERRCODE_SEGMENT_CORRUPTED,
                  NULL,
                  KNL_ERROR_STACK(aEnv),
                  smsGetSegmentId(aSegmentHandle) );
    
    STL_DASSERT( STL_FALSE );
    
    return STL_FAILURE;
}

/** @} */
