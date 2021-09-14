/*******************************************************************************
 * smqManager.c
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
#include <smg.h>
#include <smxm.h>
#include <smxl.h>
#include <sma.h>
#include <sme.h>
#include <smp.h>
#include <smq.h>
#include <smqDef.h>
#include <smqMap.h>
#include <smqManager.h>
#include <smqSession.h>
#include <smlRecord.h>
#include <smlGeneral.h>

extern smqWarmupEntry * gSmqWarmupEntry;

/**
 * @file smqManager.c
 * @brief Storage Manager Layer Sequence Manager Internal Routines
 */

/**
 * @addtogroup smq
 * @{
 */

/**
 * @brief Sequence Cache를 생성한다.
 * @param[in] aStatement Statement Handle
 * @param[in] aAttr Seqeunce 속성
 * @param[out] aSequenceId 생성된 Sequence의 물리적 아이디
 * @param[out] aSequenceHandle 생성된 Sequence를 조작할때 사용할 메모리 핸들
 * @param[in,out] aEnv Environment Pointer
 */
stlStatus smqCreateSequence( smlStatement     * aStatement,
                             smlSequenceAttr  * aAttr,
                             stlInt64         * aSequenceId,
                             void            ** aSequenceHandle,
                             smlEnv           * aEnv )
{
    smqCache * sCache = NULL;
    stlBool    sUndoLogged = STL_FALSE;

    STL_TRY( smgAllocShmMem4Open( STL_SIZEOF( smqCache ),
                                  (void**)&sCache,
                                  aEnv )
             == STL_SUCCESS );

    SMQ_BUILD_RECORD( &sCache->mRecord, aAttr );
    sCache->mNxtValue = aAttr->mStartWith;
    sCache->mCurValue = aAttr->mStartWith;
    sCache->mRecord.mRestartValue = sCache->mNxtValue;
    sCache->mValidScn = smxlGetSystemScn();
    sCache->mCacheCount = 0;

    STL_TRY( smqCreateSeqInternal( aStatement,
                                   sCache,
                                   aSequenceId,
                                   aEnv )
             == STL_SUCCESS );
    sUndoLogged = STL_TRUE;
    
    *aSequenceHandle = (void*)sCache;

    return STL_SUCCESS;

    STL_FINISH;

    if( sUndoLogged == STL_FALSE )
    {
        if( sCache != NULL )
        {
            (void)smgFreeShmMem4Open( (void*)sCache, aEnv );
        }
    }

    return STL_FAILURE;
}

/**
 * @brief Sequence Cache를 생성한다.
 * @param[in]     aStatement    Statement Handle
 * @param[in]     aCache        Sequence Cache
 * @param[out]    aSequenceId   Sequence의 물리적 아이디
 * @param[in,out] aEnv Environment Pointer
 */
stlStatus smqCreateSeqInternal( smlStatement     * aStatement,
                                smqCache         * aCache,
                                stlInt64         * aSequenceId,
                                smlEnv           * aEnv )
{
    smlRid       sSequenceRid;
    smpHandle    sPageHandle;
    smxmTrans    sMiniTrans;
    stlInt32     sState = 0;
    smlRid       sUndoRid;

    STL_TRY( knlInitLatch( &aCache->mLatch,
                           STL_TRUE,
                           KNL_ENV( aEnv ) ) == STL_SUCCESS );
    
    STL_TRY( smqInsertRecord( aStatement,
                              &aCache->mRecord,
                              &sSequenceRid,
                              aEnv ) == STL_SUCCESS );

    STL_TRY( smxmBegin( &sMiniTrans,
                        SMA_GET_TRANS_ID( aStatement ),
                        SMQ_SEQUENCE_MAP_RID,
                        SMXM_ATTR_REDO,
                        aEnv ) == STL_SUCCESS );
    sState = 1;

    STL_TRY( smpAcquire( &sMiniTrans,
                         sSequenceRid.mTbsId,
                         sSequenceRid.mPageId,
                         KNL_LATCH_MODE_EXCLUSIVE,
                         &sPageHandle,
                         aEnv ) == STL_SUCCESS );
    
    smpSetVolatileArea( &sPageHandle, (void*)&aCache, STL_SIZEOF(void*) );

    STL_TRY( smxlInsertUndoRecord( &sMiniTrans,
                                   SMG_COMPONENT_SEQUENCE,
                                   SMQ_UNDO_LOG_CREATE_SEQUENCE,
                                   NULL, /* aData */
                                   0,    /* aDataSize */
                                   sSequenceRid,
                                   &sUndoRid,
                                   aEnv )
             == STL_SUCCESS );
    
    sState = 0;
    STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );

    aCache->mRid = sSequenceRid;
    SMS_MAKE_SEGMENT_ID( &aCache->mId, &sSequenceRid );
    
    *aSequenceId = aCache->mId;

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smxmRollback( &sMiniTrans, aEnv );
    }

    return STL_FAILURE;
}

/**
 * @brief Sequence의 속성을 변경한다.
 * @param[in]     aStatement        Statement Handle
 * @param[in]     aOrgSeqHandle     Old Sequence Handle
 * @param[in]     aAttr             New Seqeunce Attriute
 * @param[out]    aNewSeqId         New Sequence Identifier
 * @param[out]    aNewSeqHandle     New Sequence Handle
 * @param[in,out] aEnv              Environment Pointer
 */
stlStatus smqAlterSequence( smlStatement     * aStatement,
                            void             * aOrgSeqHandle,
                            smlSequenceAttr  * aAttr,
                            stlInt64         * aNewSeqId,
                            void            ** aNewSeqHandle,
                            smlEnv           * aEnv )
{
    smqCache   * sNewCache = NULL;
    smqCache   * sOrgCache;
    stlBool      sIsTimedOut;
    stlInt32     sState = 0;
    stlBool      sUndoLogged = STL_FALSE;

    sOrgCache = (smqCache*)aOrgSeqHandle;
    
    /**
     * insert new sequence
     */

    STL_TRY( smgAllocShmMem4Open( STL_SIZEOF( smqCache ),
                                  (void**)&sNewCache,
                                  aEnv )
             == STL_SUCCESS );

    STL_TRY( knlAcquireLatch( &sOrgCache->mLatch,
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sIsTimedOut,
                              KNL_ENV( aEnv ) ) == STL_SUCCESS );
    sState = 1;

    stlMemcpy( sNewCache, sOrgCache, STL_SIZEOF(smqCache) );

    sState = 0;
    STL_TRY( knlReleaseLatch( &sOrgCache->mLatch,
                              KNL_ENV( aEnv ) ) == STL_SUCCESS );

    /**
     * initialize new sequence
     */
    
    sNewCache->mValidScn = smxlGetSystemScn();
    
    SMQ_BUILD_RECORD( &sNewCache->mRecord, aAttr );

    if( aAttr->mValidFlags & SML_SEQ_RESTART_MASK )
    {
        if( (aAttr->mValidFlags & SML_SEQ_RESTART_MASK) == SML_SEQ_RESTART_DEFAULT )
        {
            sNewCache->mNxtValue = sNewCache->mRecord.mStartWith;
            sNewCache->mCurValue = sNewCache->mRecord.mStartWith;
            sNewCache->mRecord.mRestartValue = sNewCache->mRecord.mStartWith;
            sNewCache->mCacheCount = 0;
        }
        else
        {
            STL_DASSERT( (aAttr->mValidFlags & SML_SEQ_RESTART_MASK) ==
                         SML_SEQ_RESTART_VALUE );
            
            sNewCache->mNxtValue = aAttr->mRestartValue;
            sNewCache->mCurValue = aAttr->mRestartValue;
            sNewCache->mRecord.mRestartValue = aAttr->mRestartValue;
            sNewCache->mCacheCount = 0;
        }
    }
    else
    {
        if( sNewCache->mNxtValue != sNewCache->mCurValue )
        {
            sNewCache->mNxtValue = sNewCache->mCurValue + sNewCache->mRecord.mIncrementBy;
        }
        
        sNewCache->mRecord.mRestartValue = sNewCache->mNxtValue;
        sNewCache->mCacheCount = 0;
    }

    STL_TRY_THROW( sNewCache->mRecord.mMaxValue >= sNewCache->mCurValue,
                   RAMP_ERR_MAXVALUE_BELOW_CURRENT_VALUE );
    STL_TRY_THROW( sNewCache->mRecord.mMinValue <= sNewCache->mCurValue,
                   RAMP_ERR_MINVALUE_EXCEED_CURRENT_VALUE );
    
    STL_TRY( smqCreateSeqInternal( aStatement,
                                   sNewCache,
                                   aNewSeqId,
                                   aEnv )
             == STL_SUCCESS );
    sUndoLogged = STL_TRUE;
    
    *aNewSeqHandle = (void*)sNewCache;
    
    /**
     * delete old sequence
     */

    STL_TRY( smqDropSequence( aStatement,
                              aOrgSeqHandle,
                              aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_MAXVALUE_BELOW_CURRENT_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_SEQUENCE_MAXVALUE_BELOW_CURRENT_VALUE,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }
    STL_CATCH( RAMP_ERR_MINVALUE_EXCEED_CURRENT_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_SEQUENCE_MINVALUE_EXCEED_CURRENT_VALUE,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;

    if( sUndoLogged == STL_FALSE )
    {
        if( sNewCache != NULL )
        {
            (void)smgFreeShmMem4Open( (void*)sNewCache, aEnv );
        }
    }
    
    if( sState == 1 )
    {
        (void)knlReleaseLatch( &sOrgCache->mLatch, KNL_ENV( aEnv ) );
    }

    return STL_FAILURE;
}

/**
 * @brief Sequence Cache를 삭제한다.
 * @param[in] aStatement Statement Handle
 * @param[in] aSequenceHandle 삭제할 Sequence Handle
 * @param[in,out] aEnv Environment Pointer
 */
stlStatus smqDropSequence( smlStatement * aStatement,
                           void         * aSequenceHandle,
                           smlEnv       * aEnv )
{
    smqCache  * sCache;
    stlBool     sIsTimedOut;
    stlInt32    sState = 0;
    smqPendArg  sPendArg;

    sCache = (smqCache*)aSequenceHandle;

    STL_TRY( knlAcquireLatch( &sCache->mLatch,
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sIsTimedOut,
                              KNL_ENV( aEnv ) ) == STL_SUCCESS );
    sState = 1;

    STL_TRY( smqDeleteRecord( aStatement,
                              &sCache->mRid,
                              aEnv ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( knlReleaseLatch( &sCache->mLatch,
                              KNL_ENV( aEnv ) ) == STL_SUCCESS );

    sPendArg.mSequenceHandle = aSequenceHandle;
    sPendArg.mTransId = SMA_GET_TRANS_ID(aStatement);
    
    STL_TRY( smgAddPendOp( aStatement,
                           SMG_PEND_DROP_SEQUENCE,
                           SML_PEND_ACTION_COMMIT,
                           (void*)&sPendArg,
                           STL_SIZEOF( smqPendArg ),
                           16,
                           aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)knlReleaseLatch( &sCache->mLatch, KNL_ENV( aEnv ) );
    }

    return STL_FAILURE;
}

stlStatus smqDropPending( stlUInt32   aActionFlag,
                          smgPendOp * aPendOp,
                          smlEnv    * aEnv )
{
    stlInt32      sOffset = 0;
    stlChar       sBuffer[16];
    smlScn        sObjectAgableScn;
    smqPendArg  * sPendArg = (smqPendArg*)aPendOp->mArgs;
    
    sObjectAgableScn = smxlGetNextSystemScn();

    STL_WRITE_MOVE_INT64( sBuffer, &sObjectAgableScn, sOffset );
    STL_WRITE_MOVE_POINTER( sBuffer, &sPendArg->mSequenceHandle, sOffset );

    STL_DASSERT( sOffset <= STL_SIZEOF( sBuffer ) );
    STL_DASSERT( sOffset <= aPendOp->mEventMemSize );

    STL_TRY( knlAddEnvEvent( SML_EVENT_DROP_SEQUENCE,
                             aPendOp->mEventMem,
                             sBuffer,
                             sOffset,
                             SML_AGER_ENV_ID,
                             KNL_EVENT_WAIT_NO,
                             STL_TRUE, /* aForceSuccess */
                             KNL_ENV( aEnv ) ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smqDropAging( void      * aData,
                        stlUInt32   aDataSize,
                        stlBool   * aDone,
                        void      * aEnv )
{
    stlInt32       sOffset = 0;
    smlScn         sObjectAgableScn;
    smlScn         sAgableScn;
    smqCache     * sCache;
    stlInt64       sSequenceId;

    *aDone = STL_FALSE;

    STL_READ_MOVE_INT64( &sObjectAgableScn, aData, sOffset );
    STL_READ_MOVE_POINTER( &sCache, aData, sOffset );

    sAgableScn = smxlGetAgableStmtScn( SML_ENV(aEnv) );

    if( sObjectAgableScn <= sAgableScn )
    {
        /**
         * free sequence cache
         */

        sSequenceId = sCache->mId;
        
        STL_TRY( smgFreeShmMem4Open( sCache,
                                     aEnv )
                 == STL_SUCCESS );
        
        STL_TRY( knlLogMsg( NULL,
                            KNL_ENV( aEnv ),
                            KNL_LOG_LEVEL_INFO,
                            "[AGER] aging sequence - object view scn(%ld), "
                            "physical id(%ld)\n",
                            sObjectAgableScn,
                            sSequenceId )
                 == STL_SUCCESS );

        *aDone = STL_TRUE;
    }

    return STL_SUCCESS;

    STL_FINISH;

    *aDone = STL_TRUE;

    return STL_FAILURE;
}

/**
 * @brief 다음 Sequence 값을 얻는다.
 * @param[in] aSequenceHandle 삭제할 Sequence Handle
 * @param[out] aNextValue 다음 Sequence 값
 * @param[in,out] aEnv Environment Pointer
 * @note 현재 Sequence 값에 Increment By 만큼 증가시키고 이를 리턴한다.
 */
stlStatus smqGetNextVal( void      * aSequenceHandle,
                         stlInt64  * aNextValue,
                         smlEnv    * aEnv )
{
    smqCache  * sCache;
    smqRecord * sRecord;
    stlBool     sIsTimedOut;
    stlInt32    sState = 0;

    sCache = (smqCache*)aSequenceHandle;
    sRecord = &sCache->mRecord;

    STL_TRY( knlAcquireLatch( &sCache->mLatch,
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sIsTimedOut,
                              KNL_ENV( aEnv ) ) == STL_SUCCESS );
    sState = 1;

    if( sRecord->mCycle == STL_TRUE )
    {
        if( sRecord->mIncrementBy > 0 )
        {
            STL_TRY( smqGetNextValPositiveCycle( sCache,
                                                 aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            STL_TRY( smqGetNextValNegativeCycle( sCache,
                                                 aEnv )
                     == STL_SUCCESS );
        }            
    }
    else
    {
        if( sRecord->mIncrementBy > 0 )
        {
            STL_TRY( smqGetNextValPositiveNocycle( sCache,
                                                   aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            STL_TRY( smqGetNextValNegativeNocycle( sCache,
                                                   aEnv )
                     == STL_SUCCESS );
        }            
    }

    sCache->mCurValue = sCache->mNxtValue;
    *aNextValue = sCache->mNxtValue;
    sCache->mNxtValue += sRecord->mIncrementBy;
    
    sState = 0;
    STL_TRY( knlReleaseLatch( &sCache->mLatch,
                              KNL_ENV( aEnv ) ) == STL_SUCCESS );

    STL_TRY( smqSetSessSeqValue( sCache,
                                 *aNextValue,
                                 aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)knlReleaseLatch( &sCache->mLatch, KNL_ENV( aEnv ) );
    }

    return STL_FAILURE;
}

/**
 * @brief 현재 Sessioni의 Sequence 값을 얻는다.
 * @param[in] aSequenceHandle 삭제할 Sequence Handle
 * @param[out] aCurrValue 현재 Sequence 값
 * @param[in,out] aEnv Environment Pointer
 */
stlStatus smqGetCurrSessVal( void      * aSequenceHandle,
                             stlInt64  * aCurrValue,
                             smlEnv    * aEnv )
{
    smqCache  * sCache;
    stlBool     sFound = STL_TRUE;
    stlInt64    sSeqValue = 0;

    sCache = (smqCache*)aSequenceHandle;

    STL_TRY( smqFindSessSeqValue( sCache,
                                  &sFound,
                                  &sSeqValue,
                                  aEnv )
             == STL_SUCCESS );

    STL_TRY_THROW( sFound == STL_TRUE, RAMP_ERR_INVALID_CURRVAL );

    *aCurrValue = sSeqValue;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_CURRVAL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_SEQUENCE_NOT_YET_DEFINED,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 현재 Sequence 값을 얻는다.
 * @param[in] aSequenceHandle 삭제할 Sequence Handle
 * @param[out] aCurrValue 현재 Sequence 값
 * @param[in,out] aEnv Environment Pointer
 */
stlStatus smqGetCurrVal( void      * aSequenceHandle,
                         stlInt64  * aCurrValue,
                         smlEnv    * aEnv )
{
    smqCache  * sCache;

    sCache = (smqCache*)aSequenceHandle;
    *aCurrValue = sCache->mCurValue;
    
    return STL_SUCCESS;
}

/**
 * @brief 현재 Sequence 값을 설정한다.
 * @param[in] aSequenceHandle 삭제할 Sequence Handle
 * @param[in] aCurrValue 설정할 Sequence 값
 * @param[in,out] aEnv Environment Pointer
 */
stlStatus smqSetCurrVal( void      * aSequenceHandle,
                         stlInt64    aCurrValue,
                         smlEnv    * aEnv )
{
    smqCache  * sCache;
    smqRecord * sRecord;
    stlBool     sIsTimedOut;
    stlInt32    sState = 0;

    sCache = (smqCache*)aSequenceHandle;
    sRecord = &sCache->mRecord;

    STL_TRY( knlAcquireLatch( &sCache->mLatch,
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sIsTimedOut,
                              KNL_ENV( aEnv ) ) == STL_SUCCESS );
    sState = 1;

    STL_TRY_THROW( sRecord->mMaxValue > aCurrValue,
                   RAMP_ERR_LIMIT_EXCEEDED );
    STL_TRY_THROW( sRecord->mMinValue < aCurrValue,
                   RAMP_ERR_LIMIT_EXCEEDED );

    if( sRecord->mCycle == STL_TRUE )
    {
        if( sRecord->mIncrementBy > 0 )
        {
            sRecord->mRestartValue = ((aCurrValue - 1) +
                                      (sRecord->mCacheSize * sRecord->mIncrementBy));
            
            if( sRecord->mRestartValue > sRecord->mMaxValue )
            {
                sRecord->mRestartValue = ((sRecord->mMinValue - 1) +
                                          (sRecord->mRestartValue -
                                           sRecord->mMaxValue));
            }
            
            STL_TRY( smqUpdateValue( NULL,  /* aMiniTrans */
                                     sRecord->mRestartValue,
                                     &sCache->mRid,
                                     aEnv ) == STL_SUCCESS );
        }
        else
        {
            sRecord->mRestartValue = ((aCurrValue + 1) +
                                      (sRecord->mCacheSize * sRecord->mIncrementBy));
            
            if( sRecord->mRestartValue < sRecord->mMinValue )
            {
                sRecord->mRestartValue = ((sRecord->mMaxValue + 1) +
                                          (sRecord->mRestartValue -
                                           sRecord->mMinValue));
            }
            
            STL_TRY( smqUpdateValue( NULL,  /* aMiniTrans */
                                     sRecord->mRestartValue,
                                     &sCache->mRid,
                                     aEnv ) == STL_SUCCESS );
        }
    }
    else
    {
        if( sRecord->mIncrementBy > 0 )
        {
            STL_TRY_THROW( aCurrValue >= sCache->mNxtValue,
                           RAMP_ERR_SETVALUE_BELOW_CURRENT_VALUE );
            
            if( aCurrValue > sRecord->mRestartValue )
            {
                sRecord->mRestartValue = ((aCurrValue - 1) +
                                          (sRecord->mCacheSize * sRecord->mIncrementBy));
            
                if( sRecord->mRestartValue > sRecord->mMaxValue )
                {
                    sRecord->mRestartValue = sRecord->mMaxValue;
                }
            
                STL_TRY( smqUpdateValue( NULL,  /* aMiniTrans */
                                         sRecord->mRestartValue,
                                         &sCache->mRid,
                                         aEnv ) == STL_SUCCESS );
            }
        }
        else
        {
            STL_TRY_THROW( aCurrValue <= sCache->mNxtValue,
                           RAMP_ERR_SETVALUE_EXCEED_CURRENT_VALUE );
            
            if( aCurrValue < sRecord->mRestartValue )
            {
                sRecord->mRestartValue = ((aCurrValue + 1) +
                                          (sRecord->mCacheSize * sRecord->mIncrementBy));
            
                if( sRecord->mRestartValue < sRecord->mMinValue )
                {
                    sRecord->mRestartValue = sRecord->mMinValue;
                }
            
                STL_TRY( smqUpdateValue( NULL,  /* aMiniTrans */
                                         sRecord->mRestartValue,
                                         &sCache->mRid,
                                         aEnv ) == STL_SUCCESS );
            }
        }
    }
        
    sCache->mCacheCount = sRecord->mCacheSize - 1;
    sCache->mCurValue = aCurrValue;
    sCache->mNxtValue = aCurrValue + sRecord->mIncrementBy;
    
    sState = 0;
    STL_TRY( knlReleaseLatch( &sCache->mLatch,
                              KNL_ENV( aEnv ) ) == STL_SUCCESS );

    STL_TRY( smqSetSessSeqValue( sCache,
                                 aCurrValue,
                                 aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_LIMIT_EXCEEDED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_SEQUENCE_LIMIT_EXCEEDED,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }
    STL_CATCH( RAMP_ERR_SETVALUE_BELOW_CURRENT_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_SEQUENCE_SETVALUE_BELOW_CURRENT_VALUE,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }
    STL_CATCH( RAMP_ERR_SETVALUE_EXCEED_CURRENT_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_SEQUENCE_SETVALUE_EXCEED_CURRENT_VALUE,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;

    if( sState == 1 )
    {
        (void)knlReleaseLatch( &sCache->mLatch, KNL_ENV( aEnv ) );
    }

    return STL_FAILURE;
}

void smqFillSequenceAttr( void            * aSequenceHandle,
                          smlSequenceAttr * aAttr )
{
    smqCache * sCache;

    sCache = (smqCache*)aSequenceHandle;

    aAttr->mStartWith = sCache->mRecord.mStartWith;
    aAttr->mIncrementBy = sCache->mRecord.mIncrementBy;
    aAttr->mMaxValue = sCache->mRecord.mMaxValue;
    aAttr->mMinValue = sCache->mRecord.mMinValue;
    aAttr->mCacheSize = sCache->mRecord.mCacheSize;
    aAttr->mCycle = sCache->mRecord.mCycle;
    aAttr->mRestartValue = sCache->mRecord.mRestartValue; 
}

stlStatus smqGetSequenceHandle( smlRid      aSequenceRid,
                                void     ** aSequenceHandle,
                                smlEnv    * aEnv )
{
    smpHandle    sPageHandle;
    stlInt32     sState = 0;
    void      ** sVolatileArea;

    STL_TRY( smpFix( aSequenceRid.mTbsId,
                     aSequenceRid.mPageId,
                     &sPageHandle,
                     aEnv ) == STL_SUCCESS );
    sState = 1;
    
    sVolatileArea = (void**)smpGetVolatileArea( &sPageHandle );
    *aSequenceHandle = (void*)sVolatileArea[0];

    sState = 0;
    STL_TRY( smpUnfix( &sPageHandle, aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smpUnfix( &sPageHandle, aEnv );
    }
    
    return STL_FAILURE;
}

stlStatus smqBuildCachesAtStartup( smlEnv * aEnv )
{
    stlInt32              i;
    
    smlIteratorProperty   sProperty;
    smlStatement        * sStatement;
    smiIterator         * sIterator;
    smqRecord             sSeqRecord;
    stlChar               sBuffer[SMQ_SEQUENCE_RECORD_DUMMY_SIZE];
    stlChar             * sDummy = (stlChar*)sBuffer;
    knlValueBlockList   * sColumns[2];
    smlRid                sSeqRid;
    smqCache            * sCache = NULL;
    smpHandle             sPageHandle;
    smxmTrans             sMiniTrans;
    stlInt32              sState = 0;
    knlRegionMark         sMemMark;
    smlRowBlock           sRowBlock;
    smlFetchInfo          sFetchInfo;
    
    STL_TRY( smeGetRelationHandle( SMQ_SEQUENCE_MAP_RID,
                                   &gSmqWarmupEntry->mSequenceMapHandle,
                                   aEnv ) == STL_SUCCESS );
        
    SML_INIT_ITERATOR_PROP( sProperty );

    KNL_INIT_REGION_MARK(&sMemMark);
    STL_TRY( knlMarkRegionMem( &aEnv->mOperationHeapMem,
                               &sMemMark,
                               (knlEnv*)aEnv ) == STL_SUCCESS );
    sState = 1;

    for ( i = 0; i < SMQ_COLUMN_COUNT; i++ )
    {
        STL_TRY( knlInitBlockNoBuffer( &sColumns[i],
                                       STL_LAYER_STORAGE_MANAGER,
                                       1,
                                       DTL_TYPE_BINARY,
                                       &aEnv->mOperationHeapMem,
                                       (knlEnv*)aEnv ) == STL_SUCCESS );
    }
 
    SMQ_BUILD_COLUMN_LIST( sColumns, &sSeqRecord, sDummy );

    STL_TRY( smaAllocStatement( SML_SESS_ENV(aEnv)->mTransId,
                                SML_INVALID_TRANSID,
                                NULL, /* aRelationHandle */
                                SML_STMT_ATTR_READONLY,
                                SML_LOCK_TIMEOUT_INVALID,
                                STL_FALSE, /* aNeedSnapshotIterator */
                                &sStatement,
                                aEnv ) == STL_SUCCESS );
    sState = 2;

    STL_TRY( smaAllocIterator( sStatement,
                               gSmqWarmupEntry->mSequenceMapHandle,
                               SML_TRM_COMMITTED,
                               SML_SRM_SNAPSHOT, 
                               &sProperty,
                               SML_SCAN_FORWARD,
                               (void**)&sIterator,
                               aEnv ) == STL_SUCCESS );
    sState = 3;

    STL_TRY( smlInitRowBlock( &sRowBlock,
                              1,
                              &aEnv->mOperationHeapMem,
                              aEnv )
             == STL_SUCCESS );

    SML_SET_FETCH_INFO_FOR_TABLE( & sFetchInfo,             /* Fetch Info */
                                  NULL,                     /* physical filter */
                                  NULL,                     /* logical filter */
                                  sColumns[0],              /* read column list */
                                  NULL,                     /* rowid column list */
                                  & sRowBlock,              /* row block */
                                  0,                        /* skip count */
                                  SML_FETCH_COUNT_MAX,      /* fetch count */
                                  STL_FALSE,                /* group key fetch */
                                  NULL                      /* filter evaluate info */
        );
    
    while( 1 )
    {
        STL_TRY( sIterator->mFetchNext( sIterator,
                                        &sFetchInfo,
                                        aEnv )
                 == STL_SUCCESS );

        sSeqRid = SML_GET_RID_VALUE( &sRowBlock, 0 );

        if( sFetchInfo.mIsEndOfScan == STL_TRUE )
        {
            break;
        }
        
        STL_TRY( smgAllocShmMem4Open( STL_SIZEOF( smqCache ),
                                      (void**)&sCache,
                                      aEnv ) == STL_SUCCESS );

        STL_TRY( knlInitLatch( &sCache->mLatch,
                               STL_TRUE,
                               KNL_ENV( aEnv ) ) == STL_SUCCESS );
        
        sCache->mNxtValue = sSeqRecord.mRestartValue;
        sCache->mCurValue = sSeqRecord.mRestartValue;
        sCache->mCacheCount = 0;
        sCache->mValidScn = smxlGetSystemScn();
        sCache->mRid = sSeqRid;
        SMS_MAKE_SEGMENT_ID( &sCache->mId, &sSeqRid );
        STL_ASSERT( sCache->mRid.mOffset == 0 );
        sCache->mRecord = sSeqRecord;
        
        STL_TRY( smxmBegin( &sMiniTrans,
                            SMA_GET_TRANS_ID( sStatement ),
                            SMQ_SEQUENCE_MAP_RID,
                            SMXM_ATTR_NOLOG_DIRTY,
                            aEnv ) == STL_SUCCESS );
        sState = 4;

        STL_TRY( smpAcquire( &sMiniTrans,
                             sSeqRid.mTbsId,
                             sSeqRid.mPageId,
                             KNL_LATCH_MODE_EXCLUSIVE,
                             &sPageHandle,
                             aEnv ) == STL_SUCCESS );
    
        smpSetVolatileArea( &sPageHandle, (void*)&sCache, STL_SIZEOF(void*) );

        sState = 3;
        STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );
    }
    
    sState = 2;
    STL_TRY( smaFreeIterator( sIterator,
                              aEnv ) == STL_SUCCESS );
    sState = 1;
    STL_TRY( smaFreeStatement( sStatement,
                               aEnv ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                       &sMemMark,
                                       STL_FALSE, /* aFreeChunk */
                                       (knlEnv*)aEnv ) == STL_SUCCESS );
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 4:
            (void)smxmRollback( &sMiniTrans, aEnv );
        case 3:
            (void)smaFreeIterator( sIterator, aEnv );
        case 2:
            (void)smaFreeStatement( sStatement, aEnv );
        case 1:
            (void)knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                            &sMemMark,
                                            STL_FALSE, /* aFreeChunk */
                                            (knlEnv*)aEnv );
        default:
            break;
    }

    return STL_FAILURE;
}

stlStatus smqGetNextValPositiveCycle( smqCache * aCache,
                                      smlEnv   * aEnv )
{
    smqRecord * sRecord;
    
    STL_PARAM_VALIDATE( aCache != NULL, KNL_ERROR_STACK(aEnv) );
    sRecord = &aCache->mRecord;
    STL_PARAM_VALIDATE( sRecord->mCycle == STL_TRUE, KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( sRecord->mIncrementBy > 0, KNL_ERROR_STACK(aEnv) );

    if( aCache->mNxtValue > sRecord->mMaxValue )
    {
        aCache->mCurValue = aCache->mNxtValue;
        aCache->mNxtValue = sRecord->mMinValue;
    }

    if( aCache->mCacheCount == 0 )
    {
        sRecord->mRestartValue += (sRecord->mCacheSize * sRecord->mIncrementBy);
        
        if( sRecord->mRestartValue > sRecord->mMaxValue )
        {
            sRecord->mRestartValue = ((sRecord->mMinValue - 1) +
                                      (sRecord->mRestartValue -
                                       sRecord->mMaxValue));
        }

        STL_TRY( smqUpdateValue( NULL,  /* aMiniTrans */
                                 sRecord->mRestartValue,
                                 &aCache->mRid,
                                 aEnv ) == STL_SUCCESS );
        
        aCache->mCacheCount = sRecord->mCacheSize - 1;
    }
    else
    {
        aCache->mCacheCount -= 1;
        STL_ASSERT( aCache->mCacheCount >= 0 );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smqGetNextValNegativeCycle( smqCache * aCache,
                                      smlEnv   * aEnv )
{
    smqRecord * sRecord;
    
    STL_PARAM_VALIDATE( aCache != NULL, KNL_ERROR_STACK(aEnv) );
    sRecord = &aCache->mRecord;
    STL_PARAM_VALIDATE( sRecord->mCycle == STL_TRUE, KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( sRecord->mIncrementBy < 0, KNL_ERROR_STACK(aEnv) );

    if( aCache->mNxtValue < sRecord->mMinValue )
    {
        aCache->mCurValue = aCache->mNxtValue;
        aCache->mNxtValue = sRecord->mMaxValue;
    }

    if( aCache->mCacheCount == 0 )
    {
        sRecord->mRestartValue += (sRecord->mCacheSize * sRecord->mIncrementBy);
        
        if( sRecord->mRestartValue < sRecord->mMinValue )
        {
            sRecord->mRestartValue = ((sRecord->mMaxValue + 1) +
                                      (sRecord->mRestartValue -
                                       sRecord->mMinValue));
        }

        STL_TRY( smqUpdateValue( NULL,  /* aMiniTrans */
                                 sRecord->mRestartValue,
                                 &aCache->mRid,
                                 aEnv ) == STL_SUCCESS );
        
        aCache->mCacheCount = sRecord->mCacheSize - 1;
    }
    else
    {
        aCache->mCacheCount -= 1;
        STL_ASSERT( aCache->mCacheCount >= 0 );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smqGetNextValPositiveNocycle( smqCache * aCache,
                                        smlEnv   * aEnv )
{
    smqRecord * sRecord;
    
    STL_PARAM_VALIDATE( aCache != NULL, KNL_ERROR_STACK(aEnv) );
    sRecord = &aCache->mRecord;
    STL_PARAM_VALIDATE( sRecord->mCycle == STL_FALSE, KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( sRecord->mIncrementBy > 0, KNL_ERROR_STACK(aEnv) );

    STL_TRY_THROW( sRecord->mMaxValue >= aCache->mNxtValue, RAMP_ERR_LIMIT_EXCEEDED );
    
    if( aCache->mCacheCount == 0 )
    {
        sRecord->mRestartValue += (sRecord->mCacheSize * sRecord->mIncrementBy);
        aCache->mCacheCount = sRecord->mCacheSize - 1;
            
        if( sRecord->mRestartValue > sRecord->mMaxValue )
        {
            aCache->mCacheCount = (sRecord->mCacheSize -
                                   ((sRecord->mRestartValue - sRecord->mMaxValue) /
                                    sRecord->mIncrementBy));
        }
            
        STL_TRY( smqUpdateValue( NULL,  /* aMiniTrans */
                                 sRecord->mRestartValue,
                                 &aCache->mRid,
                                 aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        aCache->mCacheCount -= 1;
        STL_ASSERT( aCache->mCacheCount >= 0 );
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_LIMIT_EXCEEDED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_SEQUENCE_LIMIT_EXCEEDED,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smqGetNextValNegativeNocycle( smqCache * aCache,
                                        smlEnv   * aEnv )
{
    smqRecord * sRecord;
    
    STL_PARAM_VALIDATE( aCache != NULL, KNL_ERROR_STACK(aEnv) );
    sRecord = &aCache->mRecord;
    STL_PARAM_VALIDATE( sRecord->mCycle == STL_FALSE, KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( sRecord->mIncrementBy < 0, KNL_ERROR_STACK(aEnv) );

    STL_TRY_THROW( sRecord->mMinValue <= aCache->mNxtValue, RAMP_ERR_LIMIT_EXCEEDED );
    
    if( aCache->mCacheCount == 0 )
    {
        sRecord->mRestartValue += (sRecord->mCacheSize * sRecord->mIncrementBy);
        aCache->mCacheCount = sRecord->mCacheSize - 1;
            
        if( sRecord->mRestartValue < sRecord->mMinValue )
        {
            aCache->mCacheCount = (sRecord->mCacheSize +
                                   ((sRecord->mMinValue - sRecord->mRestartValue) /
                                    sRecord->mIncrementBy));
        }
            
        STL_TRY( smqUpdateValue( NULL,  /* aMiniTrans */
                                 sRecord->mRestartValue,
                                 &aCache->mRid,
                                 aEnv ) == STL_SUCCESS );
    }
    else
    {
        aCache->mCacheCount -= 1;
        STL_ASSERT( aCache->mCacheCount >= 0 );
    }
            
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_LIMIT_EXCEEDED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_SEQUENCE_LIMIT_EXCEEDED,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
