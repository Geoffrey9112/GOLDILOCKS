/*******************************************************************************
 * smdpdhScan.c
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
#include <sms.h>
#include <smxm.h>
#include <smxl.h>
#include <smp.h>
#include <sme.h>
#include <sma.h>
#include <smo.h>

#include <smdpfhDef.h>
#include <smdpdhDef.h>
#include <smdpfh.h>
#include <smdpdh.h>

/**
 * @file smdpdhScan.c
 * @brief Storage Manager Layer Dump Table Scanning Routines
 */

/**
 * @addtogroup smdpdh
 * @{
 */

/**
 * @brief 주어진 Iterator를 초기화한다
 * @param[in,out] aIterator  초기화할 iterator
 * @param[in]     aEnv       Storage Manager Environment
 * @remarks
 */
stlStatus smdpdhInitIterator ( void   * aIterator,
                               smlEnv * aEnv )
{
    smdpdhIterator * sIterator = NULL;
    void           * sFxHandle = NULL;

    knlStartupPhase  sUsablePhase = KNL_STARTUP_PHASE_NONE;

    stlChar          sOptionString[KNL_MAX_FIXEDTABLE_COMMENTS_LENGTH] = {0,};
    
    /*
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aIterator != NULL, KNL_ERROR_STACK(aEnv) );

    sIterator = (smdpdhIterator*)aIterator;

    /**
     * Handle 설정
     */

    sFxHandle = sIterator->mFxIterator.mCommon.mRelationHandle;

    /**
     * StartUp Phase 검사
     */

    STL_TRY( knlGetFixedTableUsablePhase( sFxHandle,
                                          & sUsablePhase,
                                          KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sUsablePhase <= knlGetCurrStartupPhase(),
                   RAMP_INVALID_STARTUP_PHASE );
    
    /**
     * Dump Object Handle Callback 함수를 획득
     */
        
    STL_TRY( knlGetDumpObjHandleCallback( sFxHandle,
                                          & sIterator->mGetDumpObj,
                                          KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Dump Object Handle 을 획득
     */

    if ( sIterator->mFxIterator.mCommon.mProperty.mDumpOption == NULL )
    {
        /**
         * desc D$XXX 등을 위해 에러 상황이 아닌 것으로 처리한다.
         */

        sIterator->mDumpHandle = NULL;
    }
    else
    {
        stlStrncpy( sOptionString,
                    sIterator->mFxIterator.mCommon.mProperty.mDumpOption,
                    KNL_MAX_FIXEDTABLE_COMMENTS_LENGTH );
        
        STL_TRY( sIterator->mGetDumpObj(
                     (stlInt64) sIterator->mFxIterator.mCommon.mStatement->mSmlTransId,
                     (void *) sIterator->mFxIterator.mCommon.mStatement,
                     sOptionString,  
                     & sIterator->mDumpHandle,
                     KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        STL_TRY_THROW( sIterator->mDumpHandle != NULL, RAMP_INVALID_DUMP_OPTION );
    }
    
    /**
     * Path Controller의 공간 확보
     */
    
    STL_TRY( knlGetFixedTablePathCtrlSize( sFxHandle,
                                           & sIterator->mFxIterator.mPathCtrlSize,
                                           KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( smaAllocRegionMem( sIterator->mFxIterator.mCommon.mStatement,
                                sIterator->mFxIterator.mPathCtrlSize,
                                (void**) & sIterator->mFxIterator.mPathCtrl,
                                aEnv )
             == STL_SUCCESS );

    /**
     * Dump Table의 Callback 함수들을 획득
     */
    
    STL_TRY( knlGetFixedTableOpenCallback( sFxHandle,
                                           & sIterator->mFxIterator.mOpenFunc,
                                           KNL_ENV(aEnv) )
             == STL_SUCCESS );

    
    STL_TRY( knlGetFixedTableCloseCallback( sFxHandle,
                                            & sIterator->mFxIterator.mCloseFunc,
                                            KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( knlGetFixedTableBuildRecordCallback( sFxHandle,
                                                  & sIterator->mFxIterator.mBuildFunc,
                                                  KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     *  Dump Table을 Open한다.
     */

    if ( sIterator->mDumpHandle == NULL )
    {
        /**
         * desc D$XXX 등의 처리를 위해 SELECT * FROM D$XXX; 로 치환한 형태로
         * 에러 처리하지 않고 no rows 결과를 준다.
         */
    }
    else
    {
        STL_DASSERT( STL_RING_GET_LAST_DATA( &sIterator->mFxIterator.mCommon.mStatement->mStatementMem.mMarkList ) ==
                     ((smiIterator*)sIterator)->mRegionMarkEx );
        
        STL_TRY( sIterator->mFxIterator.mOpenFunc( sIterator->mFxIterator.mCommon.mStatement,
                                                   sIterator->mDumpHandle,
                                                   sIterator->mFxIterator.mPathCtrl,
                                                   KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_INVALID_STARTUP_PHASE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_INVALID_STARTUP_PHASE,
                      NULL,
                      KNL_ERROR_STACK(aEnv),
                      gPhaseString[sUsablePhase] );
    }

    STL_CATCH( RAMP_INVALID_DUMP_OPTION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_INVALID_DUMP_OPTION_STRING,
                      NULL,
                      KNL_ERROR_STACK(aEnv),
                      sIterator->mFxIterator.mCommon.mProperty.mDumpOption );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 주어진 Iterator를 종료한다
 * @param[in,out] aIterator  iterator
 * @param[in]     aEnv       Storage Manager Environment
 * @remarks
 */
stlStatus smdpdhFiniIterator ( void   * aIterator,
                               smlEnv * aEnv )
{
    smdpdhIterator * sIterator = NULL;

    /*
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aIterator != NULL, KNL_ERROR_STACK(aEnv) );

    sIterator = (smdpdhIterator*) aIterator;

    /**
     * Dump Table을 Close한다.
     */
    
    if ( sIterator->mDumpHandle == NULL )
    {
        /**
         * desc D$XXX 등의 처리를 위해 SELECT * FROM D$XXX; 로 치환한 형태로 OPEN 이 수행되지 않음.
         * CLOSE 를 호출하지 않는다.
         */
    }
    else
    {
        STL_TRY( sIterator->mFxIterator.mCloseFunc( sIterator->mDumpHandle,
                                                    sIterator->mFxIterator.mPathCtrl,
                                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 주어진 Iterator를 다시 초기화한다
 * @param[in,out] aIterator         초기화할 iterator
 * @param[in]     aEnv              Storage Manager Environment
 */
stlStatus smdpdhResetIterator( void   * aIterator,
                               smlEnv * aEnv )
{
    smdpdhIterator * sIterator = NULL;
    
    /*
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aIterator != NULL, KNL_ERROR_STACK(aEnv) );

    sIterator = (smdpdhIterator*) aIterator;

    if ( sIterator->mDumpHandle != NULL )
    {
        /**
         * Dump Table을 Close한다.
         */
    
        STL_TRY( sIterator->mFxIterator.mCloseFunc( sIterator->mDumpHandle,
                                                    sIterator->mFxIterator.mPathCtrl,
                                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        /**
         * Dump Table을 Open한다.
         */

        STL_TRY( sIterator->mFxIterator.mOpenFunc( sIterator->mFxIterator.mCommon.mStatement,
                                                   sIterator->mDumpHandle,
                                                   sIterator->mFxIterator.mPathCtrl,
                                                   KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 주어진 Iterator가 가진 현재 위치를 반환한다
 * @param[in]  aIterator  조사할 iterator
 * @param[out] aRowRid    반환되는 Row의 위치
 * @param[in]  aEnv       Storage Manager Environment
 * @remarks
 * Dump Table 에는 적용할 수 없다.
 */
stlStatus smdpdhGetRowRid( void   * aIterator,
                           smlRid * aRowRid,
                           smlEnv * aEnv )
{
    stlPushError( STL_ERROR_LEVEL_ABORT,
                  SML_ERRCODE_INVALID_OPERATION_ON_FIXED_TABLE,
                  NULL,
                  KNL_ERROR_STACK( aEnv ) );
    
    return STL_FAILURE;
}


/**
 * @brief 주어진 Row Rid로 Iterator의 위치를 이동시킨다
 * @param[in,out] aIterator 조사할 iterator
 * @param[in]     aRowRid   이동시킬 위치
 * @param[in]     aEnv      Storage Manager Environment
 * @remarks
 * Dump Table 에는 적용할 수 없다.
 */
stlStatus smdpdhMoveToRowRid( void   * aIterator,
                              smlRid * aRowRid,
                              smlEnv * aEnv )
{
    stlPushError( STL_ERROR_LEVEL_ABORT,
                  SML_ERRCODE_INVALID_OPERATION_ON_FIXED_TABLE,
                  NULL,
                  KNL_ERROR_STACK( aEnv ) );
    
    return STL_FAILURE;
}

/** 
 * @brief 주어진 position number로 Iterator의 위치를 이동시킨다
 * @param[in,out] aIterator 조사할 iterator
 * @param[in]     aPosNum   이동시킬 위치
 * @param[in]     aEnv      Storage Manager Environment
 * @remarks
 * Dump Table 에는 적용할 수 없다.
 */
stlStatus smdpdhMoveToPosNum( void   * aIterator,
                              stlInt64 aPosNum,
                              smlEnv * aEnv )
{   
    stlPushError( STL_ERROR_LEVEL_ABORT,
                  SML_ERRCODE_INVALID_OPERATION_ON_FIXED_TABLE,
                  NULL,
                  KNL_ERROR_STACK( aEnv ) );
        
    return STL_FAILURE;
}

/**
 * @brief 일반 Scan을 위해 열린 Iterator에서 다음 row를 반환한다
 * @param[in]       aIterator    읽은 위치를 가진 iterator
 * @param[in,out]   aFetchInfo   Fetch 정보
 * @param[in]       aEnv         Storage Manager Environment
 * @remarks
 */
stlStatus smdpdhFetchNext( void          * aIterator,
                           smlFetchInfo  * aFetchInfo,
                           smlEnv        * aEnv )
{
    smdpdhIterator * sIterator = NULL;

    stlBool          sExist = STL_FALSE;
    stlBool          sEndOfScan = STL_FALSE;
    stlInt32         sBlockIdx = 0;
    stlInt32         sAggrRowCount = 0;
    
    /*
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aIterator != NULL, KNL_ERROR_STACK(aEnv) );

    /**
     * 초기화
     */

    sIterator = (smdpdhIterator*) aIterator;

    /**
     * Fixed Record 를 구축한다.
     */

    sBlockIdx = 0;

    sEndOfScan = STL_TRUE;
    STL_TRY_THROW( sIterator->mDumpHandle != NULL, RAMP_NO_DATA );
    
    while( ( aFetchInfo->mFetchCnt > 0 ) && ( sBlockIdx < aFetchInfo->mRowBlock->mBlockSize ) )
    {
        STL_TRY( sIterator->mFxIterator.mBuildFunc( sIterator->mDumpHandle,
                                                    sIterator->mFxIterator.mPathCtrl,
                                                    aFetchInfo->mColList,
                                                    sBlockIdx,
                                                    & sExist,
                                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        if ( sExist == STL_TRUE )
        {
            sEndOfScan = STL_FALSE;

            /**
             * Filter Condition을 검사
             * Heap Relation 과 Fixed Relation 등은 Range 처리가 필요없음.
             * Filter 만 검색함. (Condition은 1개만 존재)
             */
            
            if( aFetchInfo->mFilterEvalInfo != NULL )
            {
                aFetchInfo->mFilterEvalInfo->mBlockIdx = sBlockIdx;

                STL_TRY( knlEvaluateOneExpression( aFetchInfo->mFilterEvalInfo,
                                                   KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                STL_DASSERT( KNL_GET_BLOCK_IS_SEP_BUFF(
                                 aFetchInfo->mFilterEvalInfo->mResultBlock )
                             == STL_TRUE );

                if( !DTL_BOOLEAN_IS_TRUE( KNL_GET_BLOCK_SEP_DATA_VALUE(
                                              aFetchInfo->mFilterEvalInfo->mResultBlock,
                                              sBlockIdx ) ) )
                {
                    /**
                     * 조건을 만족하지 않는 경우
                     */
                    continue;
                }
            }

            if( aFetchInfo->mAggrFetchInfo != NULL )
            {
                /**
                 * Aggregation을 수행한다.
                 */
                sBlockIdx++;
            
                if( sBlockIdx == aFetchInfo->mRowBlock->mBlockSize )
                {
                    STL_TRY( smoBlockBuildAggrFunc( aFetchInfo->mAggrFetchInfo,
                                                    0, /* aStartBlockIdx */
                                                    aFetchInfo->mRowBlock->mBlockSize,
                                                    aEnv )
                             == STL_SUCCESS );
                    sBlockIdx = 0;
                    sAggrRowCount = 1;
                }
            }
            else
            {
                /**
                 * 조건을 만족하는 경우
                 * - 다음 Block Idx 로 이동한다.
                 */
                if( aFetchInfo->mSkipCnt > 0 )
                {
                    aFetchInfo->mSkipCnt--;
                }
                else
                {
                    aFetchInfo->mFetchCnt--;
                    sBlockIdx++;
                }
            }
        }
        else
        {
            if( aFetchInfo->mAggrFetchInfo != NULL )
            {
                /**
                 * Aggregation을 수행한다.
                 */
                if( sBlockIdx > 0 )
                {
                    STL_TRY( smoBlockBuildAggrFunc( aFetchInfo->mAggrFetchInfo,
                                                    0, /* aStartBlockIdx */
                                                    sBlockIdx,
                                                    aEnv )
                             == STL_SUCCESS );
                    sAggrRowCount = 1;
                }
            }
            
            sEndOfScan = STL_TRUE;
            break;
        }
    }

    STL_RAMP( RAMP_NO_DATA );
    
    /**
     * Value Block 에 채워진 Block 개수를 설정함
     */

    if( aFetchInfo->mAggrFetchInfo == NULL )
    {
        SML_SET_USED_BLOCK_SIZE( aFetchInfo->mRowBlock, sBlockIdx );
        KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( aFetchInfo->mColList, 0, sBlockIdx );
    }
    else
    {
        SML_SET_USED_BLOCK_SIZE( aFetchInfo->mRowBlock, sAggrRowCount );
    }
    
    /**
     * Output 설정
     */

    aFetchInfo->mIsEndOfScan = sEndOfScan;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 일반 Scan을 위해 열린 Iterator에서 이전 row를 반환한다
 * @param[in]       aIterator    읽은 위치를 가진 iterator
 * @param[in,out]   aFetchInfo   Fetch 정보
 * @param[in]       aEnv         Storage Manager Environment
 * Dump Table 에는 적용할 수 없다.
 */
stlStatus smdpdhFetchPrev( void          * aIterator,
                           smlFetchInfo  * aFetchInfo,
                           smlEnv        * aEnv )
{
    stlPushError( STL_ERROR_LEVEL_ABORT,
                  SML_ERRCODE_INVALID_OPERATION_ON_FIXED_TABLE,
                  NULL,
                  KNL_ERROR_STACK( aEnv ) );
    
    return STL_FAILURE;
}


/** @} */
