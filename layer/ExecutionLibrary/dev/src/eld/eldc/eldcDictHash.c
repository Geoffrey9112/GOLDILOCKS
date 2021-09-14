/*******************************************************************************
 * eldcDictHash.c
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
 * @file eldcDictHash.c
 * @brief Dictionary Cache 를 위한 전용 Hash 
 */

#include <ell.h>
#include <elDef.h>
#include <eldt.h>
#include <eldc.h>

#include <eldDictionary.h>
#include <elgPendOp.h>
#include <eldcDictHash.h>

/**
 * @addtogroup eldcDictHash
 * @{
 */


/*
 * Dictionary Cache 종류
 */

eldcDictDump     gEldcObjectDump[ELDC_OBJECTCACHE_MAX];

eldcDictDump     gEldcPrivDump[ELDC_PRIVCACHE_MAX];


void eldcSetStableVersionInfo( ellVersionInfo * aVersionInfo )
{
    /* stlMemset( aVersionInfo, 0x00, STL_SIZEOF(ellVersionInfo) ); */

    /**
     * 무조건 읽을수 있는 상태를 설정한다.
     */
    
    aVersionInfo->mCreateTransID   = SML_INVALID_TRANSID;
    aVersionInfo->mCreateCommitSCN = 0;    
    aVersionInfo->mDropTransID     = SML_INVALID_TRANSID;
    aVersionInfo->mDropCommitSCN   = SML_INFINITE_SCN;    
    aVersionInfo->mDropFlag        = STL_FALSE;
    aVersionInfo->mModifyingCnt    = 0;
    aVersionInfo->mLastModifiedSCN = 0;
    /* aVersionInfo->mMemUseCnt = 1; */
}

/**
 * @brief Handle 을 얻어간 후로 변경되었는지의 여부
 * @param[in]  aMyTransID   My Transaction ID
 * @param[in]  aDictHandle  Dictionary Handle
 * @param[out] aIsValid     Valid 여부
 * @param[in]  aEnv         Environment
 * @remarks
 */
stlStatus eldcIsRecentHandle( smlTransId      aMyTransID,
                              ellDictHandle * aDictHandle,
                              stlBool       * aIsValid,
                              ellEnv        * aEnv )
{
    eldcHashElement * sHashElement = NULL;
    ellVersionInfo  * sCurrVersionInfo = NULL;
    eldcLocalCache  * sLocalCache;

    smlScn            sDropCommitSCN;
    smlTransId        sDropTransID;
    stlBool           sIsValid = STL_FALSE;

    smlScn            sCommitSCN;
    stlBool           sStateLatch = STL_FALSE;
    stlBool           sTimeout    = STL_FALSE;
    
    eldcRelatedTransList   * sCurRelatedTrans = NULL;
    
    /**
     * Recent 여부를 검사할수 있는 Handle 인지 검증한다.
     * - Related Handle은 Recent 검사를 할수 없다.
     */
    
    STL_DASSERT( aDictHandle->mRecentCheckable == STL_TRUE );
    
    /**
     * current version information 정보 획득
     */

    if ( aDictHandle->mObjHashElement == NULL )
    {
        sHashElement = (eldcHashElement *) aDictHandle->mPrivHashElement;
        sCurrVersionInfo = & sHashElement->mVersionInfo;
    }
    else
    {
        sHashElement = ((eldcHashElement*)aDictHandle->mObjHashElement);
        
        sCurrVersionInfo = & sHashElement->mVersionInfo;
    }

    sDropTransID = sCurrVersionInfo->mDropTransID;
    sDropCommitSCN = sCurrVersionInfo->mDropCommitSCN;

    /**
     * 객체가 Aging되었다면 유효하지 않은 버전이다.
     * - 1st checking
     */
    
    STL_TRY_THROW( sHashElement->mVersionInfo.mMemUseCnt == aDictHandle->mMemUseCnt,
                   RAMP_FINISH );
    
    /**
     * in-place fashion으로 변경사항이 있었는지 검사한다.
     */
    
    STL_TRY_THROW( sCurrVersionInfo->mLastModifiedSCN <= aDictHandle->mViewSCN,
                   RAMP_FINISH );

    STL_TRY_THROW( sCurrVersionInfo->mTryModifyCnt == aDictHandle->mTryModifyCnt,
                   RAMP_FINISH );
    
    /**
     * 객체가 삭제되었다면 Drop SCN을 확인한다.
     */
    
    if( sCurrVersionInfo->mDropFlag == STL_TRUE )
    {
        if( sDropCommitSCN == SML_INFINITE_SCN )
        {
            /**
             * DROP Commit SCN 이 설정되어 있지 않은 경우
             */

            if( sDropTransID != SML_INVALID_TRANSID )
            {
                STL_TRY( smlGetCommitScn( sDropTransID,
                                          &sDropCommitSCN,
                                          SML_ENV(aEnv) )
                         == STL_SUCCESS );

                if( sDropCommitSCN == SML_RECYCLED_TRANS_SCN )
                {
                    /**
                     * 트랜잭션이 재사용된 경우는 Version의 SCN을 사용한다.
                     * - Version의 SCN이 무한대인 경우는 DROP이 rollback한 경우이다.
                     */
                
                    sDropCommitSCN = sCurrVersionInfo->mDropCommitSCN;

                    /**
                     * DROP이 Commit되었거나 Rollback된 경우이여야 한다.
                     */
                
                    STL_DASSERT( (sDropCommitSCN != SML_INFINITE_SCN) ||
                                 (sCurrVersionInfo->mDropFlag == STL_FALSE) );
                }

                if( sDropCommitSCN == SML_INFINITE_SCN )
                {
                    /**
                     * 객체가 삭제된 이후 Commit이 되지 않은 경우
                     */

                    if( sCurrVersionInfo->mDropTransID == aMyTransID )
                    {
                        /**
                         * MY 트랜잭션이 삭제했다면 유효하지 않다고 판단한다.
                         */
                        
                        sIsValid = STL_FALSE;
                    }
                    else
                    {
                        /**
                         * 다른 트랜잭션이 삭제하고 있다면 유효하다고 판단한다.
                         */
                        
                        sIsValid = STL_TRUE;
                    }
                }
                else
                {
                    /**
                     * 객체가 삭제된 이후 Commit된 경우
                     */

                    sIsValid = STL_FALSE;
                }
            }
            else
            {
                /**
                 * Drop 되지 않은 경우
                 */
                
                sIsValid = STL_TRUE;
            }
        }
        else
        {
            /**
             * 객체가 삭제된 이후 Commit된 경우
             */
            
            sIsValid = STL_FALSE;
        }
    }
    else
    {
        /**
         * Drop 되지 않은 경우
         */

        if( sCurrVersionInfo->mModifyingCnt > 0 )
        {
            eldcSearchLocalCache( aDictHandle->mObjHashElement,
                                  & sLocalCache,
                                  aEnv );

            if( sLocalCache != NULL )
            {
                if( sLocalCache->mDropFlag == STL_TRUE )
                {
                    /**
                     * MY 트랜잭션이 객체를 삭제한 경우
                     */
            
                    sIsValid = STL_FALSE;
                    STL_THROW( RAMP_FINISH );
                }
                else
                {
                    if( aDictHandle->mLocalModifySeq != sLocalCache->mModifySeq )
                    {
                        /**
                         * HANDLE 설정이후 객체가 변경된 경우
                         */
                        
                        sIsValid = STL_FALSE;
                        STL_THROW( RAMP_FINISH );
                    }
                }
            }
            
            STL_TRY( knlAcquireLatch( & sHashElement->mWriteLatch,
                                      KNL_LATCH_MODE_SHARED,
                                      KNL_LATCH_PRIORITY_NORMAL,
                                      STL_INFINITE_TIME,   /* interval */
                                      & sTimeout,
                                      KNL_ENV(aEnv) )
                     == STL_SUCCESS );
            sStateLatch = STL_TRUE;

            /**
             * Related Object를 수정하고 있는 트랜잭션들의 Commit SCN을 구해서
             * Visibility를 검증한다.
             */
            
            sCurRelatedTrans = sHashElement->mRelatedTransList;

            while( sCurRelatedTrans != NULL )
            {
                STL_DASSERT( sCurRelatedTrans->mRefCount > 0 );
                STL_DASSERT( sCurRelatedTrans->mTransId != SML_INVALID_TRANSID );
                
                if( sCurRelatedTrans->mTransId != aMyTransID )
                {
                    STL_TRY( smlGetCommitScn( sCurRelatedTrans->mTransId,
                                              &sCommitSCN,
                                              SML_ENV(aEnv) )
                             == STL_SUCCESS );

                    /**
                     * Related Transaction List에 연결되어 있다는 의미는 아직
                     * Transaction Free 과정이 수행되지 않았다는 것을 의미하기 때문에
                     * 트랜잭션 슬롯이 재사용되었을수가 없다.
                     */
                    STL_DASSERT( sCommitSCN != SML_RECYCLED_TRANS_SCN );

                    if( sCommitSCN != SML_INFINITE_SCN )
                    {
                        if( sCommitSCN > aDictHandle->mViewSCN )
                        {
                            STL_TRY( knlReleaseLatch( & sHashElement->mWriteLatch,
                                                      KNL_ENV(aEnv) )
                                     == STL_SUCCESS );
                        
                            sIsValid = STL_FALSE;
                            STL_THROW( RAMP_FINISH );
                        }
                        else
                        {
                            /**
                             * View 이전에 수정이 왼료된 경우
                             */
                        }
                    }
                    else
                    {
                        /**
                         * 객체가 아직 수정되고 있는 중이다.
                         */
                    }
                }

                sCurRelatedTrans = sCurRelatedTrans->mNext;
            }
            
            sStateLatch = STL_FALSE;
            STL_TRY( knlReleaseLatch( & sHashElement->mWriteLatch,
                                      KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            STL_DASSERT( aDictHandle->mObjHashElement != NULL );
                        
            sIsValid = STL_TRUE;
        }
        else
        {
            /**
             * 객체의 수정이 발생하지 않았다.
             */
            
            sIsValid = STL_TRUE;
        }
        
        STL_DASSERT( sDropCommitSCN == SML_INFINITE_SCN );
    }

    /**
     * 객체가 Aging되었다면 유효하지 않은 버전이다.
     * - 2nd checking
     */

    if( sHashElement->mVersionInfo.mMemUseCnt != aDictHandle->mMemUseCnt )
    {
        sIsValid = STL_FALSE;
    }
    
    if( sCurrVersionInfo->mLastModifiedSCN > aDictHandle->mViewSCN )
    {
        sIsValid = STL_FALSE;
    }
    
    STL_RAMP( RAMP_FINISH );
    
    /**
     * Output 설정
     */

    *aIsValid = sIsValid;
    
    return STL_SUCCESS;

    STL_FINISH;

    if( sStateLatch == STL_TRUE )
    {
        sStateLatch = STL_FALSE;
        (void) knlReleaseLatch( & sHashElement->mWriteLatch,
                                KNL_ENV(aEnv) );
    }

    return STL_FAILURE;
}


/**
 * @brief Cache 의 Version 정보가 내가 읽을 수 있는 Cache 인지 검사
 * @param[in]  aMyTransID     My Transaction ID
 * @param[in]  aMyViewSCN     My View SCN
 * @param[in]  aVersionInfo   Cache 의 Version Information
 * @param[out] aIsValid       Valid 여부
 * @param[in]  aEnv           Environment
 */
stlStatus eldcIsVisibleCache( smlTransId       aMyTransID,
                              smlScn           aMyViewSCN,
                              ellVersionInfo * aVersionInfo,
                              stlBool        * aIsValid,
                              ellEnv         * aEnv )
{
    smlScn      sCreateCommitSCN;
    smlScn      sDropCommitSCN;
    smlTransId  sCreateTransID;
    smlTransId  sDropTransID;
    stlBool     sIsMyTrans = STL_FALSE;
    stlBool     sIsValid = STL_FALSE;
    stlBool     sDropFlag;

    STL_PARAM_VALIDATE( aMyViewSCN != SML_INFINITE_SCN, KNL_ERROR_STACK(aEnv) );

    /**
     * snaphshot version information
     */
    
    sCreateCommitSCN = aVersionInfo->mCreateCommitSCN;
    sDropCommitSCN   = aVersionInfo->mDropCommitSCN;
    
    sCreateTransID   = aVersionInfo->mCreateTransID;
    sDropTransID     = aVersionInfo->mDropTransID;
    sDropFlag        = aVersionInfo->mDropFlag;
    
    if( sCreateCommitSCN == SML_INFINITE_SCN )
    {
        /**
         * CREATE Commit SCN 이 설정되어 있지 않은 경우
         */

        STL_DASSERT( (sCreateTransID != SML_INVALID_TRANSID) || (gEldCacheBuilding == STL_TRUE) );
        
        STL_TRY( smlGetCommitScn( sCreateTransID,
                                  &sCreateCommitSCN,
                                  SML_ENV(aEnv) )
                 == STL_SUCCESS );

        if( sCreateCommitSCN == SML_RECYCLED_TRANS_SCN )
        {
            /**
             * 트랜잭션이 재사용된 경우는 Version의 SCN을 사용한다.
             */
            
            sCreateCommitSCN = aVersionInfo->mCreateCommitSCN;

            /**
             * smlEndTrans()가 호출되었는데 Pending 작업이 수행되지 않은 경우는
             * 없어야 한다.
             */
            STL_DASSERT( sCreateCommitSCN != SML_INFINITE_SCN );
        }

        if( sCreateCommitSCN == SML_INFINITE_SCN )
        {
            /**
             * 객체가 생성된 이후 Commit이 되지 않은 경우
             */
        
            if( sCreateTransID == aMyTransID )
            {
                sIsMyTrans = STL_TRUE;
            }
            else
            {
                /**
                 * 다른 트랜잭션에 의해서 생성되고 커밋되지 않은 객체는 읽을수 없는 버전이다.
                 */

                sIsValid = STL_FALSE;
                STL_THROW( RAMP_FINISH );
            }
        }
    }

    /**
     * 객체가 삭제되었다면 Drop SCN을 확인한다.
     */
    
    if( sDropFlag == STL_TRUE )
    {
        if( sDropCommitSCN == SML_INFINITE_SCN )
        {
            /**
             * DROP Commit SCN 이 설정되어 있지 않은 경우
             */
        
            if( sDropTransID != SML_INVALID_TRANSID )
            {
                STL_TRY( smlGetCommitScn( sDropTransID,
                                          &sDropCommitSCN,
                                          SML_ENV(aEnv) )
                         == STL_SUCCESS );

                if( sDropCommitSCN == SML_RECYCLED_TRANS_SCN )
                {
                    /**
                     * 트랜잭션이 재사용된 경우는 Version의 SCN을 사용한다.
                     * - Version의 SCN이 무한대인 경우는 DROP이 rollback한 경우이다.
                     */
                
                    sDropCommitSCN = aVersionInfo->mDropCommitSCN;

                    /**
                     * DROP이 Commit되었거나 Rollback된 경우이여야 한다.
                     */
                
                    STL_DASSERT( (sDropCommitSCN != SML_INFINITE_SCN) ||
                                 (aVersionInfo->mDropFlag == STL_FALSE) );
                }

                if( sDropCommitSCN == SML_INFINITE_SCN )
                {
                    /**
                     * 객체가 삭제된 이후 Commit이 되지 않은 경우
                     */
        
                    if( sDropTransID == aMyTransID )
                    {
                        sIsMyTrans = STL_TRUE;
                    }
                    else
                    {
                        /**
                         * 다른 트랜잭션에 의해서 삭제되고 커밋되지 않은 경우는 SCN 으로 객체의 유효성을
                         * 판단해야 한다.
                         */
                    }
                }
            }
            else
            {
                /**
                 * Drop 되지 않은 경우
                 */
            }
        }
    }
    else
    {
        STL_DASSERT( sDropCommitSCN == SML_INFINITE_SCN );
    }

    if( sIsMyTrans == STL_TRUE )
    {
        /**
         * MY 트랜잭션의 의해서 생성되었거나 삭제된 경우
         */
        
        /**
         *  CASE 1) - 발생할수 없는 경우
         *                    |---------->
         *                    |          
         * -------------------------------------------> scn
         *             ^
         *         aMyViewSCN
         *         
         *  CASE 2) - 유효한 경우
         *          |--------------------->
         *          |                   
         * -------------------------------------------> scn
         *                    ^
         *                aMyViewSCN
         *                
         *  CASE 3) - 유효하지 않은 경우
         *          |----------|
         *          |          V
         * -------------------------------------------> scn
         *                         ^
         *                     aMyViewSCN
         */
            
        if( aVersionInfo->mDropFlag == STL_FALSE )
        {
            /**
             * CASE 2에 해당됨.
             */
            
            sIsValid = STL_TRUE;
        }
        else
        {
            /**
             * CASE 3에 해당됨.
             */
        }

        STL_THROW( RAMP_FINISH );
    }

    /**
     *  CASE 1) - 유효한 경우     
     *          |--------------------|
     *          |                    V
     * -------------------------------------------> scn
     *                    ^
     *                aMyViewSCN
     *                
     *  CASE 2) - 유효한 경우         
     *          |--------------------->
     *          |                   
     * -------------------------------------------> scn
     *                    ^
     *                aMyViewSCN
     *                
     *  CASE 3) - 유효하지 않은 경우
     *          |----------|
     *          |          V
     * -------------------------------------------> scn
     *                         ^
     *                     aMyViewSCN
     *                     
     *  CASE 4) - 유효하지 않은 경우
     *                    |----------|
     *                    |          V
     * -------------------------------------------> scn
     *             ^
     *         aMyViewSCN
     *                
     *  CASE 5) - 유효하지 않은 경우
     *                    |---------->
     *                    |          
     * -------------------------------------------> scn
     *             ^
     *         aMyViewSCN
     */
    
    if( sCreateCommitSCN <= aMyViewSCN )
    {
        /**
         * 객체가 Statement 시작 이전 시점에 생성되었다면
         */
        
        if( aMyViewSCN < sDropCommitSCN )
        {
            /**
             * CASE 1과 2에 해당됨.
             */
            
            sIsValid = STL_TRUE;
        }
        else
        {
            /**
             * CASE 3에 해당됨.
             */

            STL_DASSERT( aVersionInfo->mDropFlag == STL_TRUE );
        }
    }
    else
    {
        /**
         * CASE 4와 5에 해당됨.
         */
    }

    STL_RAMP( RAMP_FINISH );
    
    /**
     * Output 설정
     */

    *aIsValid = sIsValid;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
                                  




/**
 * @brief Fixed Cache 를 구축한다.
 * @param[in]  aEnv    Environment
 * @remarks
 */
stlStatus eldcBuildFixedCache( ellEnv * aEnv )
{
    stlInt32  sState = 0;
    
    ellSessionEnv * sSessEnv = ELL_SESS_ENV(aEnv);
    
    smlTransId      sTransID  = SML_INVALID_TRANSID;
    smlStatement  * sStmt = NULL;

    elgPendOP   * sPendOP = NULL;
    elgPendOP   * sNextPendOP = NULL;
    
    /**
     * Transaction 초기화
     * Fixed Table 의 Plan Cache 는 Start-Up 의 각 과정에서 각 과정의 Cache 영역에 구축된다.
     * NO_MOUNT 단계일 경우, Transaction을 할당할 수 없으며,
     * Transaction 없는 Statement 를 생성하여 동작해야 함.
     */

    sTransID = SML_INVALID_TRANSID;

    /**
     * Statement 초기화 
     */
    
    STL_TRY( smlAllocStatement( sTransID,
                                NULL, /* aUpdateRelHandle */
                                SML_STMT_ATTR_READONLY,
                                SML_LOCK_TIMEOUT_INVALID,
                                STL_FALSE, /* aNeedSnapshotIterator */
                                & sStmt,
                                SML_ENV( aEnv ) )
             == STL_SUCCESS );
    sState = 1;
    
    /**
     * Fixed Column Cache 구축 
     */

    STL_TRY( eldcBuildCacheFxColumn( sTransID, sStmt, aEnv ) == STL_SUCCESS );
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    /**
     * Fixed Table Cache 구축
     */

    STL_TRY( eldcBuildCacheFxTable( sTransID, sStmt, aEnv ) == STL_SUCCESS );
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );

    /**
     * Statement 자원 해제
     */
    
    sState = 0;
    STL_TRY( smlFreeStatement( sStmt, SML_ENV( aEnv ) )
             == STL_SUCCESS );
    
    /**
     * Transaction 자원 해제
     */
    
    sTransID = SML_INVALID_TRANSID;

    /**
     * Fixed Table Cache 를 위해 사용된 Pending Memory 를 해제한다.
     */

    sPendOP  = (elgPendOP *) sSessEnv->mTxCommitPendOpList;
    while( sPendOP != NULL )
    {
        sNextPendOP = sPendOP->mNext;
        
        STL_TRY( eldClearPendingMem( sPendOP, aEnv ) == STL_SUCCESS );
        
        sPendOP = sNextPendOP;
    }
    sSessEnv->mTxCommitPendOpList = NULL;
    
    sPendOP  = (elgPendOP *) sSessEnv->mTxRollbackPendOpList;
    while( sPendOP != NULL )
    {
        sNextPendOP = sPendOP->mNext;
        
        STL_TRY( eldClearPendingMem( sPendOP, aEnv ) == STL_SUCCESS );
        
        sPendOP = sNextPendOP;
    }
    sSessEnv->mTxRollbackPendOpList = NULL;

    sPendOP  = (elgPendOP *) sSessEnv->mTxPreRollbackActionList;
    while( sPendOP != NULL )
    {
        sNextPendOP = sPendOP->mNext;
        
        STL_TRY( eldClearPendingMem( sPendOP, aEnv ) == STL_SUCCESS );
        
        sPendOP = sNextPendOP;
    }
    sSessEnv->mTxPreRollbackActionList = NULL;
    
    sPendOP  = (elgPendOP *) sSessEnv->mStmtSuccessPendOpList;
    while( sPendOP != NULL )
    {
        sNextPendOP = sPendOP->mNext;
        
        STL_TRY( eldClearPendingMem( sPendOP, aEnv ) == STL_SUCCESS );
        
        sPendOP = sNextPendOP;
    }
    sSessEnv->mStmtSuccessPendOpList = NULL;

    sPendOP  = (elgPendOP *) sSessEnv->mStmtRollbackPendOpList;
    while( sPendOP != NULL )
    {
        sNextPendOP = sPendOP->mNext;
        
        STL_TRY( eldClearPendingMem( sPendOP, aEnv ) == STL_SUCCESS );
        
        sPendOP = sNextPendOP;
    }
    sSessEnv->mStmtRollbackPendOpList = NULL;
    
    sPendOP  = (elgPendOP *) sSessEnv->mStmtPreRollbackActionList;
    while( sPendOP != NULL )
    {
        sNextPendOP = sPendOP->mNext;
        
        STL_TRY( eldClearPendingMem( sPendOP, aEnv ) == STL_SUCCESS );
        
        sPendOP = sNextPendOP;
    }
    sSessEnv->mStmtPreRollbackActionList = NULL;
    
    return STL_SUCCESS;

    STL_FINISH;

    switch ( sState )
    {
        case 1:
            (void) smlFreeStatement( sStmt, SML_ENV( aEnv ) );
        default:
            break;
    }
    
    /**
     * Start-Up 과정의 실패임
     */

    sSessEnv->mTxCommitPendOpList = NULL;
    sSessEnv->mTxRollbackPendOpList = NULL;
    sSessEnv->mTxPreRollbackActionList = NULL;
    sSessEnv->mStmtSuccessPendOpList = NULL;
    sSessEnv->mStmtRollbackPendOpList = NULL;
    sSessEnv->mStmtPreRollbackActionList = NULL;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/**
 * @brief Build Cache 과정중 Pending Operation 을 수행한다.
 * @param[in]  aTransID   Transaction ID
 * @param[in]  aStmt      Statement
 * @param[in]  aEnv       Environment
 * @remarks
 * Start-Up 과정에서 Pending Memory 가 증가하지 않도록 Pending Operation 들을 수행한다.
 */
stlStatus eldcRunPendOP4BuildCache( smlTransId     aTransID,
                                    smlStatement * aStmt,
                                    ellEnv       * aEnv )
{
    ellSessionEnv * sSessEnv = ELL_SESS_ENV(aEnv);

    /**
     * Pending Operation 들을 수행
     */
    
    STL_TRY( ellEndStmtDDL( aStmt, aEnv ) == STL_SUCCESS );
    STL_TRY( ellPreActionCommitDDL( aTransID, aEnv ) == STL_SUCCESS );
    STL_TRY( ellCommitTransDDL( aTransID, aEnv ) == STL_SUCCESS );

    /**
     * 계속 진행될 Cache 구축 과정을 위해 DDL 이 포함되어 있음을 설정
     */
    
    sSessEnv->mIncludeDDL = STL_TRUE;

    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief SQL-Object Cache 를 구축한다.
 * @param[in]  aEnv    Environment
 * @remarks
 * SQL-Object 간에 종속성이 있으므로, Cache 구축 순서를 지켜야 한다.
 */
stlStatus eldcBuildObjectCache( ellEnv * aEnv )
{
    stlInt32   sState = 0;
    
    smlTransId          sTransID  = SML_INVALID_TRANSID;
    smlStatement      * sStmt = NULL;

    stlTime             sTime = 0;
    
    /**
     * Transaction 초기화
     * - Read-Only 이므로 Transaction 을 생성하지 않는다.
     */

    sTransID = SML_INVALID_TRANSID;
    
    /**
     * Statement 초기화 
     */

    STL_TRY( smlAllocStatement( sTransID,
                                NULL, /* aUpdateRelHandle */
                                SML_STMT_ATTR_READONLY,
                                SML_LOCK_TIMEOUT_INVALID,
                                STL_FALSE, /* aNeedSnapshotIterator */
                                & sStmt,
                                SML_ENV( aEnv ) )
             == STL_SUCCESS );
    sState = 3;

    sTime = stlNow();
    
    STL_TRY( ellBeginStmtDDL( sStmt, sTime, aEnv ) == STL_SUCCESS );
    sState = 4;

    /*************************************************************
     * Primitive Object Cache
     * - Cache 구축 순서를 지켜야 하며,
     *   Start-Up 과정과 Service 단계에서의 내부 동작이 다르다.
     *************************************************************/
    
    /**
     * Index Cache 구축전에 Key Range 사용을 위해 Physical Index Handle 을 설정
     */

    STL_TRY( eldcSetIndexPhyHandle4NonCache( sStmt, aEnv ) == STL_SUCCESS );
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    /**
     * SQL-Column Cache 를 구축
     */

    STL_TRY( eldcBuildColumnCache( sTransID, sStmt, aEnv ) == STL_SUCCESS );
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    /**
     * SQL-Table Cache 를 구축
     */

    STL_TRY( eldcBuildTableCache( sTransID, sStmt, aEnv ) == STL_SUCCESS );
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    /**
     * SQL-Index Cache 를 구축
     */

    STL_TRY( eldcBuildIndexCache( sTransID, sStmt, aEnv ) == STL_SUCCESS );
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    /**
     * SQL-Constraint Cache 를 구축
     */

    STL_TRY( eldcBuildConstCache( sTransID, sStmt, aEnv ) == STL_SUCCESS );
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );

    /*************************************************************
     * Additional Object Cache
     * - Start-Up 과정과 Service 단계의 내부 동작이 동일하다.
     *************************************************************/
    
    /**
     * SQL-Sequence Cache 를 구축
     */

    STL_TRY( eldcBuildSequenceCache( sTransID, sStmt, aEnv ) == STL_SUCCESS );
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    /**
     * SQL-Schema Cache 를 구축
     */

    STL_TRY( eldcBuildSchemaCache( sTransID, sStmt, aEnv ) == STL_SUCCESS );
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    /**
     * SQL-Tablespace Cache 를 구축
     */

    STL_TRY( eldcBuildTablespaceCache( sTransID, sStmt, aEnv ) == STL_SUCCESS );
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    /**
     * SQL-Authorization Cache 를 구축
     */

    STL_TRY( eldcBuildAuthCache( sTransID, sStmt, aEnv ) == STL_SUCCESS );
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    /**
     * SQL-Catalog Cache 를 구축
     */

    STL_TRY( eldcBuildCatalogCache( sTransID, sStmt, aEnv ) == STL_SUCCESS );
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );


    /**
     * SQL-Synonym Cache 를 구축
     */

    STL_TRY( eldcBuildSynonymCache( sTransID, sStmt, aEnv ) == STL_SUCCESS );
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );

    /**
     * SQL-PublicSynonym Cache 를 구축
     */

    STL_TRY( eldcBuildPublicSynonymCache( sTransID, sStmt, aEnv ) == STL_SUCCESS );
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );

    /**
     * SQL-Profile Cache 를 구축
     */

    STL_TRY( eldcBuildProfileCache( sTransID, sStmt, aEnv ) == STL_SUCCESS );
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    /**
     * Statement 자원 해제
     */

    sState = 3;
    STL_TRY( ellEndStmtDDL( sStmt, aEnv ) == STL_SUCCESS );

    sState = 2;
    STL_TRY( smlFreeStatement( sStmt, SML_ENV( aEnv ) )
             == STL_SUCCESS );
    
    /**
     * Transaction 자원 해제
     */
    
    sState = 1;
    STL_TRY( ellPreActionCommitDDL( sTransID, aEnv ) == STL_SUCCESS );
    /* STL_TRY( smlCommit( sTransID, */
    /*                     SML_TRANSACTION_CWM_WAIT, */
    /*                     SML_ENV( aEnv ) ) == STL_SUCCESS ); */
    sState = 0;
    STL_TRY( ellCommitTransDDL( sTransID, aEnv ) == STL_SUCCESS );
    /* STL_TRY( smlEndTrans( sTransID, */
    /*                       SML_PEND_ACTION_COMMIT, */
    /*                       SML_TRANSACTION_CWM_WAIT, */
    /*                       SML_ENV( aEnv ) ) == STL_SUCCESS ); */

    return STL_SUCCESS;

    STL_FINISH;

    switch (sState)
    {
        case 4:
            (void) ellRollbackStmtDDL( sTransID, aEnv );
        case 3:
            (void) smlFreeStatement( sStmt, SML_ENV( aEnv ) );
        case 2:
            (void) ellPreActionRollbackDDL( sTransID, aEnv );
        case 1:
            (void) ellRollbackTransDDL( sTransID, aEnv );
        default:
            break;
    }

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}

/**
 * @brief Privilege Cache 를 구축한다.
 * @param[in]  aEnv    Environment
 * @remarks
 */
stlStatus eldcBuildPrivCache( ellEnv * aEnv )
{
    stlInt32   sState = 0;
    
    smlTransId          sTransID  = SML_INVALID_TRANSID;
    smlStatement      * sStmt = NULL;

    stlTime             sTime = 0;
    
    /**
     * Transaction 초기화
     * - Read-Only 이므로 Transaction 을 생성하지 않는다.
     */

    sTransID = SML_INVALID_TRANSID;

    /**
     * Statement 초기화 
     */
    
    STL_TRY( smlAllocStatement( sTransID,
                                NULL, /* aUpdateRelHandle */
                                SML_STMT_ATTR_READONLY,
                                SML_LOCK_TIMEOUT_INVALID,
                                STL_FALSE, /* aNeedSnapshotIterator */
                                & sStmt,
                                SML_ENV( aEnv ) )
             == STL_SUCCESS );
    sState = 3;

    sTime = stlNow();
    STL_TRY( ellBeginStmtDDL( sStmt, sTime, aEnv ) == STL_SUCCESS );
    sState = 4;
    
    /**
     * Database Privilege Cache 를 구축 
     */

    STL_TRY( eldcBuildDatabasePrivCache( sTransID, sStmt, aEnv ) == STL_SUCCESS );
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    /**
     * Tablespace Privilege Cache 를 구축 
     */

    STL_TRY( eldcBuildSpacePrivCache( sTransID, sStmt, aEnv ) == STL_SUCCESS );
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );

    /**
     * Schema Privilege Cache 를 구축 
     */

    STL_TRY( eldcBuildSchemaPrivCache( sTransID, sStmt, aEnv ) == STL_SUCCESS );
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    /**
     * Table Privilege Cache 를 구축 
     */

    STL_TRY( eldcBuildTablePrivCache( sTransID, sStmt, aEnv ) == STL_SUCCESS );
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    /**
     * Usage Privilege Cache 를 구축 
     */

    STL_TRY( eldcBuildUsagePrivCache( sTransID, sStmt, aEnv ) == STL_SUCCESS );
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    /**
     * Column Privilege Cache 를 구축 
     */

    STL_TRY( eldcBuildColumnPrivCache( sTransID, sStmt, aEnv ) == STL_SUCCESS );
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    /**
     * Statement 자원 해제
     */
    
    sState = 3;
    STL_TRY( ellEndStmtDDL( sStmt, aEnv ) == STL_SUCCESS );
    
    sState = 2;
    STL_TRY( smlFreeStatement( sStmt, SML_ENV( aEnv ) )
             == STL_SUCCESS );
    
    /**
     * Transaction 자원 해제
     */
    
    sState = 1;
    STL_TRY( ellPreActionCommitDDL( sTransID, aEnv ) == STL_SUCCESS );
    
    /* STL_TRY( smlCommit( sTransID, */
    /*                     SML_TRANSACTION_CWM_WAIT, */
    /*                     SML_ENV( aEnv ) ) == STL_SUCCESS ); */
    
    sState = 0;
    STL_TRY( ellCommitTransDDL( sTransID, aEnv ) == STL_SUCCESS );
    
    /* STL_TRY( smlEndTrans( sTransID, */
    /*                       SML_PEND_ACTION_COMMIT, */
    /*                       SML_TRANSACTION_CWM_WAIT, */
    /*                       SML_ENV( aEnv ) ) == STL_SUCCESS ); */
    
    return STL_SUCCESS;

    STL_FINISH;

    switch (sState)
    {
        case 4:
            (void) ellRollbackStmtDDL( sTransID, aEnv );
        case 3:
            (void) smlFreeStatement( sStmt, SML_ENV( aEnv ) );
        case 2:
            (void) ellPreActionRollbackDDL( sTransID, aEnv );
        case 1:
            (void) ellRollbackTransDDL( sTransID, aEnv );
        default:
            break;
    }

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/**
 * @brief Index Cache 구축전 Start-Up 과정의 Key Range 를 위한 Index Physical Handle 설정
 * @param[in]  aStmt      Statement
 * @param[in]  aEnv       Environment
 * @remarks
 */
stlStatus eldcSetIndexPhyHandle4NonCache( smlStatement        * aStmt,
                                          ellEnv              * aEnv )
{
    stlBool             sBeginSelect = STL_FALSE;
    
    void              * sTableHandle = NULL;
    knlValueBlockList * sTableValueList = NULL;

    void                * sIterator  = NULL;
    smlIteratorProperty   sIteratorProperty;

    smlRid       sRowRid;
    smlScn       sRowScn;
    smlRowBlock  sRowBlock;
    eldMemMark   sMemMark;

    smlFetchInfo          sFetchInfo;

    dtlDataValue * sNameValue = NULL;
    dtlDataValue * sSchemaValue = NULL;
    dtlDataValue * sPhysicalValue = NULL;

    stlInt32 sState = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * DEFINITION_SCHEMA.INDEXES 테이블의 table handle 을 획득
     */
    
    sTableHandle = gEldTablePhysicalHandle[ELDT_TABLE_ID_INDEXES];

    /**
     * DEFINITION_SCHEMA.INDEXES 테이블의 Value List와 Row Block 할당
     */

    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_INDEXES,
                                     & sMemMark,
                                     & sTableValueList,
                                     aEnv )
             == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( smlInitSingleRowBlock( &sRowBlock,
                                    &sRowRid,
                                    &sRowScn,
                                    SML_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * SELECT 준비
     */

    STL_TRY( eldBeginForTableAccessQuery( aStmt,
                                          sTableHandle,
                                          & sIteratorProperty,
                                          & sRowBlock,
                                          sTableValueList,
                                          NULL,
                                          NULL,
                                          & sFetchInfo,
                                          NULL,
                                          & sIterator,         
                                          aEnv )
             == STL_SUCCESS );

    sBeginSelect = STL_TRUE;

    /**
     * DEFINITION_SCHEMA.DATA_TYPE_DESCRIPTOR_UNIQUE_DTD_IDENTIFIER_INDEX index 를 검색한다.
     */

    sNameValue = eldFindDataValue( sTableValueList,
                                   ELDT_TABLE_ID_INDEXES,
                                   ELDT_Indexes_ColumnOrder_INDEX_NAME );
    
    sSchemaValue = eldFindDataValue( sTableValueList,
                                     ELDT_TABLE_ID_INDEXES,
                                     ELDT_Indexes_ColumnOrder_SCHEMA_ID );
    
    sPhysicalValue = eldFindDataValue( sTableValueList,
                                       ELDT_TABLE_ID_INDEXES,
                                       ELDT_Indexes_ColumnOrder_PHYSICAL_ID );
    
    while ( 1 )
    {
        /**
         * 레코드를 읽는다.
         */
        
        STL_TRY( eldFetchNext( sIterator,
                               & sFetchInfo,
                               aEnv )
                 == STL_SUCCESS );

        if( sFetchInfo.mIsEndOfScan == STL_TRUE )
        {
            break;
        }

        /**
         * DEFINITION_SCHEMA.DATA_TYPE_DESCRIPTOR_UNIQUE_DTD_IDENTIFIER_INDEX 인지 검사
         */

        if ( stlStrncmp( (stlChar*) sNameValue->mValue,
                         ELDT_DTD_IDENTIFIER_INDEX_NAME,
                         stlStrlen( ELDT_DTD_IDENTIFIER_INDEX_NAME ) ) == 0 )
        {
            if ( *(stlInt64*)sSchemaValue->mValue == ELDT_SCHEMA_ID_DEFINITION_SCHEMA )
            {
                /**
                 * 해당 Index 의 Physical Handle 획득
                 */

                STL_TRY( smlGetRelationHandle( *(stlInt64*) sPhysicalValue->mValue,
                                               & gEldNonCacheIndexPhyHandle4DTD,
                                               SML_ENV( aEnv ) )
                         == STL_SUCCESS );
                continue;
            }
            else
            {
                /* nothing to do */
            }
        }
        else
        {
            /* nothing to do */
        }

        /**
         * DEFINITION_SCHEMA.VIEWS_INDEX_TABLE_ID 인지 검사
         */

        if ( stlStrncmp( (stlChar*) sNameValue->mValue,
                         ELDT_VIEWS_INDEX_TABLE_ID,
                         stlStrlen( ELDT_VIEWS_INDEX_TABLE_ID ) ) == 0 )
        {
            if ( *(stlInt64*)sSchemaValue->mValue == ELDT_SCHEMA_ID_DEFINITION_SCHEMA )
            {
                /**
                 * 해당 Index 의 Physical Handle 획득
                 */

                STL_TRY( smlGetRelationHandle( *(stlInt64*) sPhysicalValue->mValue,
                                               & gEldNonCacheIndexPhyHandle4ViewedTableID,
                                               SML_ENV( aEnv ) )
                         == STL_SUCCESS );
                continue;
            }
            else
            {
                /* nothing to do */
            }
        }
        else
        {
            /* nothing to do */
        }

        /**
         * DEFINITION_SCHEMA.COLUMNS_INDEX_TABLE_ID 인지 검사
         */

        if ( stlStrncmp( (stlChar*) sNameValue->mValue,
                         ELDT_COLUMNS_INDEX_TABLE_ID,
                         stlStrlen( ELDT_COLUMNS_INDEX_TABLE_ID ) ) == 0 )
        {
            if ( *(stlInt64*)sSchemaValue->mValue == ELDT_SCHEMA_ID_DEFINITION_SCHEMA )
            {
                /**
                 * 해당 Index 의 Physical Handle 획득
                 */

                STL_TRY( smlGetRelationHandle( *(stlInt64*) sPhysicalValue->mValue,
                                               & gEldNonCacheIndexPhyHandle4ColTableID,
                                               SML_ENV( aEnv ) )
                         == STL_SUCCESS );
                continue;
            }
            else
            {
                /* nothing to do */
            }
        }
        else
        {
            /* nothing to do */
        }
        
        /**
         * DEFINITION_SCHEMA.INDEX_KEY_COLUMN_USAGE_INDEX_INDEX_ID 인지 검사
         */

        if ( stlStrncmp( (stlChar*) sNameValue->mValue,
                         ELDT_INDEX_KEY_COLUMN_USAGE_INDEX_INDEX_ID,
                         stlStrlen( ELDT_INDEX_KEY_COLUMN_USAGE_INDEX_INDEX_ID ) ) == 0 )
        {
            if ( *(stlInt64*)sSchemaValue->mValue == ELDT_SCHEMA_ID_DEFINITION_SCHEMA )
            {
                /**
                 * 해당 Index 의 Physical Handle 획득
                 */

                STL_TRY( smlGetRelationHandle( *(stlInt64*) sPhysicalValue->mValue,
                                               & gEldNonCacheIndexPhyHandle4IndexKeyColIndexID,
                                               SML_ENV( aEnv ) )
                         == STL_SUCCESS );
                continue;
            }
            else
            {
                /* nothing to do */
            }
        }
        else
        {
            /* nothing to do */
        }

        /**
         * DEFINITION_SCHEMA.CHECK_CONSTRAINTS_INDEX_CONSTRAINT_ID 인지 검사
         */

        if ( stlStrncmp( (stlChar*) sNameValue->mValue,
                         ELDT_CHECK_CONSTRAINTS_INDEX_CONSTRAINT_ID,
                         stlStrlen( ELDT_CHECK_CONSTRAINTS_INDEX_CONSTRAINT_ID ) ) == 0 )
        {
            if ( *(stlInt64*)sSchemaValue->mValue == ELDT_SCHEMA_ID_DEFINITION_SCHEMA )
            {
                /**
                 * 해당 Index 의 Physical Handle 획득
                 */

                STL_TRY( smlGetRelationHandle( *(stlInt64*) sPhysicalValue->mValue,
                                               & gEldNonCacheIndexPhyHandle4CheckConstID,
                                               SML_ENV( aEnv ) )
                         == STL_SUCCESS );
                continue;
            }
            else
            {
                /* nothing to do */
            }
        }
        else
        {
            /* nothing to do */
        }

        /**
         * DEFINITION_SCHEMA.KEY_COLUMN_USAGE_INDEX_CONSTRAINT_ID 인지 검사
         */

        if ( stlStrncmp( (stlChar*) sNameValue->mValue,
                         ELDT_KEY_COLUMN_USAGE_INDEX_CONSTRAINT_ID,
                         stlStrlen( ELDT_KEY_COLUMN_USAGE_INDEX_CONSTRAINT_ID ) ) == 0 )
        {
            if ( *(stlInt64*)sSchemaValue->mValue == ELDT_SCHEMA_ID_DEFINITION_SCHEMA )
            {
                /**
                 * 해당 Index 의 Physical Handle 획득
                 */

                STL_TRY( smlGetRelationHandle( *(stlInt64*) sPhysicalValue->mValue,
                                               & gEldNonCacheIndexPhyHandle4KeyColConstID,
                                               SML_ENV( aEnv ) )
                         == STL_SUCCESS );
                continue;
            }
            else
            {
                /* nothing to do */
            }
        }
        else
        {
            /* nothing to do */
        }

        /**
         * DEFINITION_SCHEMA.REFERENTIAL_CONSTRAINTS_INDEX_CONSTRAINT_ID 인지 검사
         */

        if ( stlStrncmp( (stlChar*) sNameValue->mValue,
                         ELDT_REFERENTIAL_CONSTRAINTS_INDEX_CONSTRAINT_ID,
                         stlStrlen( ELDT_REFERENTIAL_CONSTRAINTS_INDEX_CONSTRAINT_ID ) ) == 0 )
        {
            if ( *(stlInt64*)sSchemaValue->mValue == ELDT_SCHEMA_ID_DEFINITION_SCHEMA )
            {
                /**
                 * 해당 Index 의 Physical Handle 획득
                 */

                STL_TRY( smlGetRelationHandle( *(stlInt64*) sPhysicalValue->mValue,
                                               & gEldNonCacheIndexPhyHandle4RefConstID,
                                               SML_ENV( aEnv ) )
                         == STL_SUCCESS );
                continue;
            }
            else
            {
                /* nothing to do */
            }
        }
        else
        {
            /* nothing to do */
        }

        /**
         * DEFINITION_SCHEMA.CHECK_COLUMN_USAGE_INDEX_CONSTRAINT_ID 인지 검사
         */

        if ( stlStrncmp( (stlChar*) sNameValue->mValue,
                         ELDT_CHECK_COLUMN_USAGE_INDEX_CONSTRAINT_ID,
                         stlStrlen( ELDT_CHECK_COLUMN_USAGE_INDEX_CONSTRAINT_ID ) ) == 0 )
        {
            if ( *(stlInt64*)sSchemaValue->mValue == ELDT_SCHEMA_ID_DEFINITION_SCHEMA )
            {
                /**
                 * 해당 Index 의 Physical Handle 획득
                 */

                STL_TRY( smlGetRelationHandle( *(stlInt64*) sPhysicalValue->mValue,
                                               & gEldNonCacheIndexPhyHandle4ChkColConstID,
                                               SML_ENV( aEnv ) )
                         == STL_SUCCESS );
                continue;
            }
            else
            {
                /* nothing to do */
            }
        }
        else
        {
            /* nothing to do */
        }
        

    }

    STL_ASSERT( gEldNonCacheIndexPhyHandle4DTD != NULL );
    STL_ASSERT( gEldNonCacheIndexPhyHandle4ViewedTableID != NULL );
    STL_ASSERT( gEldNonCacheIndexPhyHandle4ColTableID != NULL );
    STL_ASSERT( gEldNonCacheIndexPhyHandle4IndexKeyColIndexID != NULL );
    STL_ASSERT( gEldNonCacheIndexPhyHandle4CheckConstID != NULL );
    STL_ASSERT( gEldNonCacheIndexPhyHandle4KeyColConstID != NULL );
    STL_ASSERT( gEldNonCacheIndexPhyHandle4RefConstID != NULL );
    STL_ASSERT( gEldNonCacheIndexPhyHandle4ChkColConstID != NULL );
    
    /**
     * SELECT 종료
     */

    sBeginSelect = STL_FALSE;
    STL_TRY( eldEndForQuery( sIterator,
                             aEnv )
             == STL_SUCCESS );

    /**
     * 사용한 메모리를 해제한다.
     */

    sState = 0;
    STL_TRY( eldFreeTableValueList( & sMemMark, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    if ( sBeginSelect == STL_TRUE )
    {
        (void) eldEndForQuery( sIterator,
                               aEnv );
    }

    switch (sState)
    {
        case 1:
            (void) eldFreeTableValueList( & sMemMark, aEnv );
        default:
            break;
    }
    
    return STL_FAILURE;
}


/**
 * @brief Index Cache 없이 Start-Up 과정을 빠르게 하기 위한 ID 에 해당하는 Hard-Coded Key Ragne 를 생성한다.
 * @param[in]  aTransID          Transaction ID
 * @param[in]  aStmt             Statement
 * @param[in]  aColumnValue      Column Value
 * @param[in]  aID               Identifier
 * @param[out] aKeyCompList      Key Compare List
 * @param[out] aIndexValueList   조건에 맞는 Data 를 얻어올 Index Value List
 * @param[out] aRangePred        Range Predicate
 * @param[out] aRangeContext     Context for Range Predicate
 * @param[in]  aEnv              Environment
 * @remarks
 */
stlStatus eldcMakeNonCacheRange( smlTransId              aTransID,
                                 smlStatement          * aStmt,
                                 knlValueBlockList     * aColumnValue,
                                 stlInt64                aID,
                                 knlKeyCompareList    ** aKeyCompList,
                                 knlValueBlockList    ** aIndexValueList,
                                 knlPredicateList     ** aRangePred,
                                 knlExprContextInfo   ** aRangeContext,
                                 ellEnv                * aEnv )
{
    knlKeyCompareList * sKeyCompList = NULL;
    knlValueBlockList * sIndexValueList = NULL;
    
    knlValueBlockList * sKeyColumn = NULL;
    knlValueBlockList * sKeyValue  = NULL;

    knlPredicateList    * sRange = NULL;
    knlExprContextInfo  * sRangeContext = NULL;
    
    /**
     * Key Compare List 공간 할당
     */

    STL_TRY( knlCreateKeyCompareList( 1,
                                      & sKeyCompList,
                                      & aEnv->mMemDictOP,
                                      KNL_ENV(aEnv) )
             == STL_SUCCESS );    

    sKeyCompList->mTableColOrder[0] = aColumnValue->mColumnOrder;
    sKeyCompList->mIndexColOffset[0] = 0;


    STL_TRY( knlInitBlock( & sKeyCompList->mValueList,
                           1,
                           STL_TRUE,
                           STL_LAYER_EXECUTION_LIBRARY,
                           aColumnValue->mTableID,
                           aColumnValue->mColumnOrder,
                           DTL_TYPE_NATIVE_BIGINT,
                           gDefaultDataTypeDef[ DTL_TYPE_NATIVE_BIGINT ].mArgNum1,
                           gDefaultDataTypeDef[ DTL_TYPE_NATIVE_BIGINT ].mArgNum2,
                           gDefaultDataTypeDef[ DTL_TYPE_NATIVE_BIGINT ].mStringLengthUnit,
                           gDefaultDataTypeDef[ DTL_TYPE_NATIVE_BIGINT ].mIntervalIndicator,
                           & aEnv->mMemDictOP,
                           KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sKeyCompList->mCompFunc[0] =
        dtlPhysicalFuncArg2FuncList[DTL_TYPE_NATIVE_BIGINT][DTL_TYPE_NATIVE_BIGINT][ DTL_PHYSICAL_FUNC_IS_EQUAL ];

    /**
     * Index 의 Value 공간 확보 
     */

    STL_TRY( knlInitBlock( & sIndexValueList,
                           KNL_NO_BLOCK_READ_CNT,
                           STL_TRUE, /* 컬럼임 */
                           STL_LAYER_EXECUTION_LIBRARY,
                           ELL_DICT_OBJECT_ID_NA,
                           0,  /* aColumnOrder */
                           DTL_TYPE_NATIVE_BIGINT,
                           DTL_PRECISION_NA,
                           DTL_SCALE_NA,
                           DTL_STRING_LENGTH_UNIT_NA,
                           DTL_INTERVAL_INDICATOR_NA,
                           & aEnv->mMemDictOP,
                           KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * Key Column 설정 
     */

    STL_TRY( knlInitShareBlock( & sKeyColumn,
                                sIndexValueList,
                                & aEnv->mMemDictOP,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
                                
    /**
     * Key Value 설정 
     */

    STL_DASSERT( (KNL_GET_BLOCK_DB_TYPE( aColumnValue ) != DTL_TYPE_LONGVARCHAR) &&
                 (KNL_GET_BLOCK_DB_TYPE( aColumnValue ) != DTL_TYPE_LONGVARBINARY) );
    
    STL_TRY( knlInitCopyBlock( & sKeyValue,
                               STL_LAYER_EXECUTION_LIBRARY,
                               KNL_NO_BLOCK_READ_CNT,
                               aColumnValue,
                               & aEnv->mMemDictOP,
                               KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    stlMemcpy( KNL_GET_BLOCK_DATA_PTR( sKeyValue, 0 ),
               & aID,
               STL_SIZEOF(stlInt64) );

    KNL_SET_BLOCK_DATA_LENGTH( sKeyValue, 0, STL_SIZEOF(stlInt64) );
    
    /**
     * Equal Key Range 생성
     */
    
    STL_TRY( knlCreateRangePred( 1,
                                 STL_TRUE,
                                 & sRange,
                                 & sRangeContext,
                                 & aEnv->mMemDictOP,
                                 KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    /* context info 구성 */
    sRangeContext->mContexts[ 0 ].mInfo.mValueInfo = sKeyColumn->mValueBlock;
    sRangeContext->mContexts[ 1 ].mInfo.mValueInfo = sKeyValue->mValueBlock;
    
    STL_TRY( knlAddRange( STL_TRUE,
                          0, /* sKeyColumn */
                          1, /* sKeyValue */
                          STL_TRUE,  /* aIsIncludeEqual */
                          STL_TRUE,  /* aIsAsc */
                          STL_FALSE, /* aIsNullFirst */
                          STL_TRUE,  /* Null Always STOP */
                          sRange,
                          KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    STL_TRY( knlAddRange( STL_FALSE,
                          0, /* sKeyColumn */
                          1, /* sKeyValue */
                          STL_TRUE,  /* aIsIncludeEqual */
                          STL_TRUE,  /* aIsAsc */
                          STL_FALSE, /* aIsNullFirst */
                          STL_TRUE,  /* Null Always STOP */
                          sRange,
                          KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    /**
     * Output 설정
     */

    *aKeyCompList    = sKeyCompList;
    *aIndexValueList = sIndexValueList;
    *aRangePred      = sRange;
    *aRangeContext   = sRangeContext;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief 정의되지 않은 객체에 대한 출력을 위한 함수 포인터 
 * @param[in]   aObjectDesc    Object Descriptor (unused)
 * @param[in]   aStringBuffer  String Buffer
 * @remarks
 */
void eldcPrintINVALID( void    * aObjectDesc,
                       stlChar * aStringBuffer )
{
    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "INVALID OBJECT" );
}



/**
 * @brief Dictionary Table 의 레코드 개수를 얻는다.
 * @param[in]  aTransID   Transaction ID
 * @param[in]  aStmt      Statement
 * @param[in]  aTableID   Dictionary Table ID
 * @param[out] aRecordCnt Record 개수
 * @param[in]  aEnv       Environment
 * @remarks
 */
stlStatus  eldcGetTableRecordCount( smlTransId            aTransID,
                                    smlStatement        * aStmt,
                                    eldtDefinitionTableID aTableID,
                                    stlInt64            * aRecordCnt,
                                    ellEnv              * aEnv )
{
    stlInt64   sRecordCnt = 0;
    
    stlBool    sBeginSelect = STL_FALSE;

    void              * sTableHandle = NULL;

    knlValueBlockList * sTableValueList = NULL;
    
    void                * sIterator  = NULL;
    smlIteratorProperty   sIteratorProperty;

    smlRid              sRowRid;
    smlScn              sRowScn;
    smlRowBlock         sRowBlock;
    eldMemMark          sMemMark;

    smlFetchInfo          sFetchInfo;

    stlInt32  sState = 0;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aTableID > ELDT_TABLE_ID_NA) &&
                        (aTableID < ELDT_TABLE_ID_MAX),
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRecordCnt != NULL, ELL_ERROR_STACK(aEnv) );
    
                        
    /**
     * table handle 을 획득
     */
    
    sTableHandle = gEldTablePhysicalHandle[aTableID];
        
    /**
     * Table Value List와 Row Block 할당
     */

    STL_TRY( eldAllocTableValueList( aTableID,
                                     & sMemMark,
                                     & sTableValueList,
                                     aEnv )
             == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( smlInitSingleRowBlock( &sRowBlock,
                                    &sRowRid,
                                    &sRowScn,
                                    SML_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * SELECT 준비
     */

    STL_TRY( eldBeginForTableAccessQuery( aStmt,
                                          sTableHandle,
                                          & sIteratorProperty,
                                          & sRowBlock,
                                          sTableValueList,
                                          NULL,
                                          NULL,
                                          & sFetchInfo,
                                          NULL,
                                          & sIterator,         
                                          aEnv )
             == STL_SUCCESS );

    sBeginSelect = STL_TRUE;

    
    /**
     * 레코드 개수를 계산 
     */

    sRecordCnt = 0;
    while ( 1 )
    {
        STL_TRY( eldFetchNext( sIterator,
                               & sFetchInfo,
                               aEnv )
                 == STL_SUCCESS );
        
        if( sFetchInfo.mIsEndOfScan == STL_TRUE )
        {
            break;
        }
        sRecordCnt++;
    }
    
    /**
     * SELECT 종료
     */

    sBeginSelect = STL_FALSE;
    STL_TRY( eldEndForQuery( sIterator,
                             aEnv )
             == STL_SUCCESS );

    /**
     * 사용한 메모리를 해제한다.
     */

    sState = 0;
    STL_TRY( eldFreeTableValueList( & sMemMark, aEnv ) == STL_SUCCESS );
    

    /**
     * Output 설정
     */
    
    *aRecordCnt = sRecordCnt;

    return STL_SUCCESS;

    STL_FINISH;

    if ( sBeginSelect == STL_TRUE )
    {
        (void) eldEndForQuery( sIterator,
                               aEnv );
    }

    switch (sState)
    {
        case 1:
            (void) eldFreeTableValueList( & sMemMark, aEnv );
        default:
            break;
    }
    
    return STL_FAILURE;
}

/**
 * @brief Hash Data 와 Object Descriptor를 위한 Hash Element 를 구성한다.
 * @param[out] aHashElement    Hash Element
 * @param[in]  aTransID        Transaction ID
 * @param[in]  aHashData       Hash Data
 * @param[in]  aHashValue      Hash Data 의 Hash Value
 * @param[in]  aObjectDesc     Object Descriptor (nullable)
 * @param[in]  aEnv            Environment
 * @remarks
 */ 
stlStatus eldcMakeHashElement( eldcHashElement ** aHashElement,
                               smlTransId         aTransID,
                               void             * aHashData,
                               stlUInt32          aHashValue,
                               void             * aObjectDesc,
                               ellEnv           * aEnv )
{
    eldcHashElement      * sHashElement      = NULL;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aHashElement != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aHashData != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Hash Element 생성
     */

    STL_TRY( eldAllocHashElementMem( & sHashElement,
                                     aEnv )
             == STL_SUCCESS );

    sHashElement->mVersionInfo.mCreateTransID    = aTransID;
    sHashElement->mVersionInfo.mCreateCommitSCN  = SML_INFINITE_SCN;
    sHashElement->mVersionInfo.mDropTransID      = SML_INVALID_TRANSID;
    sHashElement->mVersionInfo.mDropCommitSCN    = SML_INFINITE_SCN;
    sHashElement->mVersionInfo.mDropFlag         = STL_FALSE;
    sHashElement->mVersionInfo.mLastModifiedSCN  = 0;
    sHashElement->mVersionInfo.mModifyingCnt     = 0;

    /* latch 초기화 */
    STL_TRY( knlInitLatch( & sHashElement->mWriteLatch,
                           STL_TRUE,
                           KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sHashElement->mHashValue        = aHashValue; 
    sHashElement->mHashData         = aHashData;
    sHashElement->mRelatedTransList = NULL;
    sHashElement->mObjectDesc       = aObjectDesc;
    sHashElement->mNext             = NULL;
    

    /**
     * Output Parameter 설정 
     */
    
    *aHashElement = sHashElement;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief Dictionary Hash 를 생성한다.
 * @param[out] aDictHash   생성한 Dictionary Hash
 * @param[in]  aBucketCnt  Bucket 의 갯수
 * @param[in]  aEnv        Environment
 * @remarks
 */
stlStatus eldcCreateDictHash( eldcDictHash ** aDictHash,
                              stlUInt32       aBucketCnt,
                              ellEnv        * aEnv )
{
    stlUInt32 i = 0;
    
    stlUInt32 sSize = 0;
    
    eldcDictHash * sDictHash = NULL;

    /**
     * 필요한 공간의 크기 계산
     * (aBucketCnt - 1) : 1개는 eldcDictHash 에 이미 존재한다.
     */
    sSize = STL_ALIGN_DEFAULT( STL_SIZEOF(eldcDictHash) )
        + STL_ALIGN_DEFAULT( STL_SIZEOF(eldcHashBucket) ) * (aBucketCnt -1);

    /**
     * 공유 메모리 공간 할당
     */

    STL_TRY( eldAllocCacheMem( (void **) & sDictHash, sSize, aEnv ) == STL_SUCCESS );
    stlMemset( sDictHash, 0x00, sSize );

    /**
     * Dictionary Hash 초기화
     */

    sDictHash->mBucketCnt = aBucketCnt;
    
    for ( i = 0; i < aBucketCnt; i++ )
    {
        STL_TRY( knlInitLatch( & sDictHash->mBucket[i].mInsDelLatch,
                               STL_TRUE,  /* In Shared Memory */
                               KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        sDictHash->mBucket[i].mElement = NULL;
    }

    /**
     * Output 설정
     */

    *aDictHash = sDictHash;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Hash Key 에 부합하는 Hash Element 를 검색
 * @param[in]  aTransID     Transaction ID
 * @param[in]  aViewSCN     View SCN
 * @param[in]  aDictHash    Dictionary Hash
 * @param[in]  aHashKey     검색할 임의의 Hash Key 자료구조
 * @param[in]  aHashFunc    Hash Value를 얻기 위한 임의의 함수
 * @param[in]  aCompareFunc Hash Value 가 동일할 경우 Key 가 동일한지를 비교하는 임의의 함수
 * @param[out] aHashElement Hash Element
 * @param[in]  aEnv         Environment
 * @remarks
 */
stlStatus eldcFindHashElement( smlTransId         aTransID,
                               smlScn             aViewSCN,
                               eldcDictHash     * aDictHash,
                               void             * aHashKey,
                               eldcHashFunc       aHashFunc,
                               eldcKeyCompareFunc aCompareFunc,
                               eldcHashElement ** aHashElement,
                               ellEnv           * aEnv )
{
    stlUInt32  sHashValue = 0;
    stlUInt32  sBucketNO = 0;

    eldcHashElement * sHashElement;

    stlBool sIsValid = STL_FALSE;
    
    /*
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aDictHash != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aHashKey != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aHashElement != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Hash Value 및 Bucket Number 획득
     */

    sHashValue = aHashFunc( aHashKey );
    sBucketNO = sHashValue % aDictHash->mBucketCnt;

    /**
     * 부합하는 Hash Element 검색 
     */

    for ( sHashElement = aDictHash->mBucket[sBucketNO].mElement;
          sHashElement != NULL;
          sHashElement = sHashElement->mNext )
    {
        if ( sHashElement->mHashValue == sHashValue )
        {
            if ( aCompareFunc( aHashKey, sHashElement ) == STL_TRUE )
            {
                /**
                 * Visible Hash Element 인지 검사
                 */

                STL_TRY( eldcIsVisibleCache( aTransID,
                                             aViewSCN,
                                             & sHashElement->mVersionInfo,
                                             & sIsValid,
                                             aEnv )
                         == STL_SUCCESS );
                
                if ( sIsValid == STL_TRUE )
                {
                    break;
                }
                else
                {
                    /* 읽을 수 없는 Hash Element 임 */
                    continue;
                }
            }
            else
            {
                /* Key 가 다름 */
                continue;
            }
        }
        else
        {
            /* Hash Value 가 다름 */
            continue;
        }
    }
    
    /**
     * Output 설정
     */

    *aHashElement = sHashElement;
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief Dictionary Hash 에서 첫번째 Hash Element 를 얻는다.
 * @param[in]  aDictHash     Dictionary Hash
 * @param[out] aBucketNO     Hash Element 의 Bucket Number
 * @param[out] aHashElement  Hash Element
 * @param[in]  aEnv          Environment
 * @remarks
 */
stlStatus eldcGetFirstHashElement( eldcDictHash     * aDictHash,
                                   stlUInt32        * aBucketNO,
                                   eldcHashElement ** aHashElement,
                                   ellEnv           * aEnv )
{
    stlUInt32  i = 0;
    eldcHashElement * sElement = NULL;

    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDictHash != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aBucketNO != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aHashElement != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * 처음 발견되는 Hash Element 를 검색
     */
    
    sElement = NULL;
    for ( i = 0; i < aDictHash->mBucketCnt; i++ )
    {
        if ( aDictHash->mBucket[i].mElement != NULL )
        {
            sElement = aDictHash->mBucket[i].mElement;
            break;
        }
    }

    /**
     * Output 설정
     */
    
    if ( sElement != NULL )
    {
        *aBucketNO = i;
        *aHashElement = sElement;
    }
    else
    {
        *aBucketNO = aDictHash->mBucketCnt + 1;
        *aHashElement = NULL;
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Dictionary Hash 에서 현재 Hash Element의 다음 Hash Element 를 얻는다.
 * @param[in]     aDictHash     Dictionary Hash
 * @param[in,out] aBucketNO     Hash Element 의 Bucket Number
 * @param[in,out] aHashElement  Hash Element
 * @param[in]     aEnv          Environment
 * @remarks
 */
stlStatus eldcGetNextHashElement( eldcDictHash     * aDictHash,
                                  stlUInt32        * aBucketNO,
                                  eldcHashElement ** aHashElement,
                                  ellEnv           * aEnv )
{
    stlUInt32  i = 0;
    
    stlUInt32  sBucketNO = 0;
    eldcHashElement * sElement = NULL;

    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDictHash != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aBucketNO != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( *aBucketNO < aDictHash->mBucketCnt, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aHashElement != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( *aHashElement != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * 처음 발견되는 Hash Element 를 검색
     */

    sBucketNO = *aBucketNO;
    sElement = (*aHashElement)->mNext;

    if ( sElement == NULL )
    {
        for ( i = sBucketNO + 1; i < aDictHash->mBucketCnt; i++ )
        {
            if ( aDictHash->mBucket[i].mElement != NULL )
            {
                sElement = aDictHash->mBucket[i].mElement;
                break;
            }
        }
    }
    else
    {
        i = sBucketNO;
    }

    /**
     * Output 설정
     */
    
    if ( sElement != NULL )
    {
        *aBucketNO = i;
        *aHashElement = sElement;
    }
    else
    {
        *aBucketNO = aDictHash->mBucketCnt + 1;
        *aHashElement = NULL;
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief Hash Element 를 추가
 * @param[in] aDictHash    Dictionary Hash
 * @param[in] aHashElement Hash Element
 * @param[in] aEnv         Environment
 * @remarks
 * CREATE object DDL 을 위해 Hash Element 를 추가함.
 */
stlStatus eldcInsertHashElement( eldcDictHash    * aDictHash,
                                 eldcHashElement * aHashElement,
                                 ellEnv          * aEnv )
{
    stlBool    sStateLatch = STL_FALSE;
    
    stlUInt32  sBucketNO = 0;
    stlBool    sTimeout = STL_FALSE;

    eldcSetFlagRollbackArg    sSetFlagPendArg;
    eldcBucketElementPendArg  sRollbackPendArg;
    eldcElementPendArg sCommitPendArg;

    elgPendOP * sSetFlagPendBuffer1 = NULL;
    elgPendOP * sRollbackPendBuffer = NULL;
    elgPendOP * sCommitPendBuffer   = NULL;
    
    stlMemset( & sSetFlagPendArg, 0x00, STL_SIZEOF(eldcSetFlagRollbackArg) );
    stlMemset( & sRollbackPendArg, 0x00, STL_SIZEOF(eldcBucketElementPendArg) );
    stlMemset( & sCommitPendArg, 0x00, STL_SIZEOF(eldcElementPendArg) );
    
    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aDictHash != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aHashElement != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Pending Operation 을 위한 Buffer 확보
     */

    STL_TRY( eldAllocPendingMem( (void **) & sSetFlagPendBuffer1,
                                 STL_SIZEOF(eldcSetFlagRollbackArg),
                                 aEnv )
             == STL_SUCCESS );

    STL_TRY( eldAllocPendingMem( (void **) & sRollbackPendBuffer,
                                 STL_SIZEOF(eldcBucketElementPendArg),
                                 aEnv )
             == STL_SUCCESS );

    STL_TRY( eldAllocPendingMem( (void **) & sCommitPendBuffer,
                                 STL_SIZEOF(eldcElementPendArg),
                                 aEnv )
             == STL_SUCCESS );
    
    /**
     * Bucket Number 를 계산
     */
    
    sBucketNO = aHashElement->mHashValue % aDictHash->mBucketCnt;

    /**
     * X Latch 획득 1
     */

    STL_TRY( knlAcquireLatch( & aDictHash->mBucket[sBucketNO].mInsDelLatch,
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,   /* interval */
                              & sTimeout,
                              KNL_ENV(aEnv) )
             == STL_SUCCESS );
    STL_ASSERT( sTimeout == STL_FALSE );
    sStateLatch = STL_TRUE;
    
    /**
     * Hash Element 연결
     */

    aHashElement->mNext = aDictHash->mBucket[sBucketNO].mElement;
    aDictHash->mBucket[sBucketNO].mElement = aHashElement;
    
    /**
     * X Latch 해제 
     */

    sStateLatch = STL_FALSE;
    STL_TRY( knlReleaseLatch( & aDictHash->mBucket[sBucketNO].mInsDelLatch,
                              KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Drop Flag 에 대한 Pre-ROLLBACK Action 을 등록
     */

    sSetFlagPendArg.mVersionInfo  = & aHashElement->mVersionInfo;
    sSetFlagPendArg.mDropFlag = STL_TRUE;
    
    elgAddPreRollbackAction( ELG_PRE_ROLLBACK_SET_FLAG,
                             sSetFlagPendBuffer1,
                             & sSetFlagPendArg,
                             STL_SIZEOF(eldcSetFlagRollbackArg),
                             aEnv );

    /**
     * Statement/Transaction ROLLBACK 을 위한 Pending Operation 등록
     * - Hash Element 의 연결을 해제한다.
     */
    
    sRollbackPendArg.mDictHash = aDictHash;
    sRollbackPendArg.mBucketNO = sBucketNO;
    sRollbackPendArg.mElement  = aHashElement;

    elgAddStmtRollbackPendOP( ELG_PEND_ROLLBACK_INSERT_HASH_ELEMENT,
                              sRollbackPendBuffer,
                              & sRollbackPendArg,
                              STL_SIZEOF(eldcBucketElementPendArg),
                              aEnv );

    /**
     * Statement 정상 종료후 Transaction COMMIT 을 위한 Pending Operation 등록
     * - Hash Element 의 Create SCN 을 설정한다.
     */

    sCommitPendArg.mElement = aHashElement;

    elgAddStmtSuccessPendOP( ELG_PEND_COMMIT_INSERT_HASH_ELEMENT,
                             sCommitPendBuffer,
                             & sCommitPendArg,
                             STL_SIZEOF(eldcElementPendArg),
                             NULL, /* aAgingEventBuffer */
                             aEnv );

    return STL_SUCCESS;

    STL_FINISH;

    if ( sStateLatch == STL_TRUE )
    {
        (void)knlReleaseLatch( & aDictHash->mBucket[sBucketNO].mInsDelLatch,
                               KNL_ENV(aEnv) );
        sStateLatch = STL_FALSE;
    }

    if ( sSetFlagPendBuffer1 != NULL )
    {
        (void) eldClearPendingMem( sSetFlagPendBuffer1, ELL_ENV(aEnv) );
        sSetFlagPendBuffer1 = NULL;
    }

    if ( sRollbackPendBuffer != NULL )
    {
        (void) eldClearPendingMem( sRollbackPendBuffer, ELL_ENV(aEnv) );
        sRollbackPendBuffer = NULL;
    }

    if ( sCommitPendBuffer != NULL )
    {
        (void) eldClearPendingMem( sCommitPendBuffer, ELL_ENV(aEnv) );
        sCommitPendBuffer = NULL;
    }
    
    return STL_FAILURE;
}



/**
 * @brief Hash Key 에 해당하는 Hash Element 를 제거한다.
 * @param[in] aTransID     Transaction ID
 * @param[in] aDictHash    Dictionary Hash
 * @param[in] aHashElement Hash Element
 * @param[in] aEnv         Environment
 * @remarks
 * DROP object DDL 을 Hash Element 를 제거함.
 * - 실제 연결해제는 Pending Operation 을 통해 처리된다.
 */
stlStatus eldcDeleteHashElement( smlTransId         aTransID,
                                 eldcDictHash     * aDictHash,
                                 eldcHashElement  * aHashElement,
                                 ellEnv           * aEnv )
{
    stlBool    sStateLatch = STL_FALSE;
    
    stlUInt32  sBucketNO = 0;
    stlBool    sTimeout = STL_FALSE;

    eldcSetFlagRollbackArg   sSetFlagPendArg;
    eldcElementPendArg       sRollbackPendArg;
    eldcBucketElementPendArg sCommitPendArg;

    void * sAgingEventBuffer = NULL;

    elgPendOP * sSetFlagPendBuffer1 = NULL;
    elgPendOP * sRollbackPendBuffer = NULL;
    elgPendOP * sCommitPendBuffer   = NULL;
    
    stlMemset( & sSetFlagPendArg, 0x00, STL_SIZEOF(eldcSetFlagRollbackArg) );
    stlMemset( & sRollbackPendArg, 0x00, STL_SIZEOF(eldcElementPendArg) );
    stlMemset( & sCommitPendArg, 0x00, STL_SIZEOF(eldcBucketElementPendArg) );
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDictHash != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aHashElement != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Aging Event Buffer 확보
     */
    
    STL_TRY( knlAllocEnvEventMem( STL_SIZEOF(eldMemFreeAgingEvent),
                                  & sAgingEventBuffer,
                                  KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * Pending Operation 을 위한 Buffer 확보
     */

    STL_TRY( eldAllocPendingMem( (void **) & sSetFlagPendBuffer1,
                                 STL_SIZEOF(eldcSetFlagRollbackArg),
                                 aEnv )
             == STL_SUCCESS );

    STL_TRY( eldAllocPendingMem( (void **) & sRollbackPendBuffer,
                                 STL_SIZEOF(eldcElementPendArg),
                                 aEnv )
             == STL_SUCCESS );

    STL_TRY( eldAllocPendingMem( (void **) & sCommitPendBuffer,
                                 STL_SIZEOF(eldcBucketElementPendArg),
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Bucket Number 를 계산
     */
    
    sBucketNO = aHashElement->mHashValue % aDictHash->mBucketCnt;
    
    /**
     * X Latch 획득 
     */

    STL_TRY( knlAcquireLatch( & aDictHash->mBucket[sBucketNO].mInsDelLatch,
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,   /* interval */
                              & sTimeout,
                              KNL_ENV(aEnv) )
             == STL_SUCCESS );
    STL_ASSERT( sTimeout == STL_FALSE );
    sStateLatch = STL_TRUE;

    /**
     * Hash Element 에 대한 동시성 정보 설정
     */

    aHashElement->mVersionInfo.mDropTransID  = aTransID;
    aHashElement->mVersionInfo.mDropFlag = STL_TRUE;

    /**
     * X Latch 해제 
     */

    sStateLatch = STL_FALSE;
    STL_TRY( knlReleaseLatch( & aDictHash->mBucket[sBucketNO].mInsDelLatch,
                              KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Drop Flag 에 대한 Pre-ROLLBACK Action 을 등록
     */

    sSetFlagPendArg.mVersionInfo  = & aHashElement->mVersionInfo;
    sSetFlagPendArg.mDropFlag = STL_FALSE;
    
    elgAddPreRollbackAction( ELG_PRE_ROLLBACK_SET_FLAG,
                             sSetFlagPendBuffer1,
                             & sSetFlagPendArg,
                             STL_SIZEOF(eldcSetFlagRollbackArg),
                             aEnv );

    /**
     * Statement/Transaction ROLLBACK 을 위한 Pending Operation 등록
     */

    sRollbackPendArg.mElement = aHashElement;

    elgAddStmtRollbackPendOP( ELG_PEND_ROLLBACK_DELETE_HASH_ELEMENT,
                              sRollbackPendBuffer,
                              & sRollbackPendArg,
                              STL_SIZEOF(eldcElementPendArg),
                              aEnv );
    
    /**
     * Statement 정상 종료 후 Transaction COMMIT 을 위한 Pending Operation 등록
     * - Hash Element 의 연결을 해제한다.
     * - 모든 Object Descriptor 는 메모리 해제 대상임 
     */

    sCommitPendArg.mDictHash = aDictHash;
    sCommitPendArg.mBucketNO = sBucketNO;
    sCommitPendArg.mElement  = aHashElement;

    /* STL_TRY( knlLogMsg( NULL, */
    /*                     KNL_ENV(aEnv), */
    /*                     KNL_LOG_LEVEL_INFO, */
    /*                     "[EL eldcDeleteHashElement()] [Alloc Event Mem]  %p", */
    /*                     sAgingEventBuffer ) */
    /*          == STL_SUCCESS ); */
    
    elgAddStmtSuccessPendOP( ELG_PEND_COMMIT_DELETE_HASH_ELEMENT,
                             sCommitPendBuffer,
                             & sCommitPendArg,
                             STL_SIZEOF(eldcBucketElementPendArg),
                             sAgingEventBuffer,
                             aEnv );
    
    return STL_SUCCESS;

    STL_FINISH;

    if ( sAgingEventBuffer != NULL )
    {
        (void) knlFreeEnvEventMem( sAgingEventBuffer, KNL_ENV(aEnv) );
        sAgingEventBuffer = NULL;
    }
    
    if ( sStateLatch == STL_TRUE )
    {
        (void) knlReleaseLatch( & aDictHash->mBucket[sBucketNO].mInsDelLatch,
                                KNL_ENV(aEnv) );
        sStateLatch = STL_FALSE;
    }

    if ( sSetFlagPendBuffer1 != NULL )
    {
        (void) eldClearPendingMem( sSetFlagPendBuffer1, ELL_ENV(aEnv) );
        sSetFlagPendBuffer1 = NULL;
    }

    if ( sRollbackPendBuffer != NULL )
    {
        (void) eldClearPendingMem( sRollbackPendBuffer, ELL_ENV(aEnv) );
        sRollbackPendBuffer = NULL;
    }

    if ( sCommitPendBuffer != NULL )
    {
        (void) eldClearPendingMem( sCommitPendBuffer, ELL_ENV(aEnv) );
        sCommitPendBuffer = NULL;
    }
    
    return STL_FAILURE;
}

/**
 * @brief Into Hash Element 에 Related Object 정보를 추가
 * @param[in] aTransID           Transaction ID
 * @param[in] aIntoHashElement   Into Hash Element
 * @param[in] aRelatedList       Related List 의 시작 위치 
 * @param[in] aObjectHashElement 추가할 객체의 Hash Element
 * @param[in] aObjectType        Object Type
 * @param[in] aEnv               Environment
 * @remarks
 * 관련 Object 에 CREATE DDL 발생시 이를 추가함.
 */
stlStatus eldcAddHashRelated( smlTransId                aTransID,
                              eldcHashElement         * aIntoHashElement,
                              eldcHashRelatedObjList ** aRelatedList,
                              eldcHashElement         * aObjectHashElement,
                              ellObjectType             aObjectType,
                              ellEnv                  * aEnv )
{
    ellSessObjectMgr  * sObjMgr = ELL_SESS_OBJ(aEnv);
    
    stlBool    sStateLatch = STL_FALSE;
    stlBool    sTimeout = STL_FALSE;

    stlInt64   sNextObjectID = ELL_DICT_OBJECT_ID_NA;
    stlInt64   sHashObjectID = ELL_DICT_OBJECT_ID_NA;
    
    eldcHashRelatedObjList * sPrevRelated = NULL;
    eldcHashRelatedObjList * sHashRelated = NULL;

    eldcRelatedTransList   * sRelatedTrans = NULL;
    eldcRelatedTransList   * sCurRelatedTrans = NULL;
    
    eldcLocalCache         * sLocalCache    = NULL;
    eldcLocalCache         * sNewLocalCache = NULL;

    eldcSetFlagRollbackArg             sSetFlagPendArg;
    eldcHashElementHashRelatedPendArg  sRollbackPendArg;
    eldcHashRelatedPendArg             sCommitPendArg;

    elgPendOP * sSetFlagPendBuffer  = NULL;
    elgPendOP * sRollbackPendBuffer = NULL;
    elgPendOP * sCommitPendBuffer   = NULL;
    
    stlMemset( & sSetFlagPendArg, 0x00, STL_SIZEOF(eldcSetFlagRollbackArg) );
    stlMemset( & sRollbackPendArg, 0x00, STL_SIZEOF(eldcHashElementHashRelatedPendArg) );
    stlMemset( & sCommitPendArg, 0x00, STL_SIZEOF(eldcHashRelatedPendArg) );


    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aIntoHashElement != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIntoHashElement->mObjectDesc != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRelatedList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aObjectHashElement != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aObjectHashElement->mObjectDesc != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * New Local Cache 공간 할당
     */
    
    STL_TRY( knlAllocDynamicMem( & sObjMgr->mMemSessObj,
                                 STL_SIZEOF(eldcLocalCache),
                                 (void **) & sNewLocalCache,
                                 KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Pending Operation 을 위한 Buffer 확보
     */

    STL_TRY( eldAllocPendingMem( (void **) & sSetFlagPendBuffer,
                                 STL_SIZEOF(eldcSetFlagRollbackArg),
                                 aEnv )
             == STL_SUCCESS );

    STL_TRY( eldAllocPendingMem( (void **) & sRollbackPendBuffer,
                                 STL_SIZEOF(eldcHashElementHashRelatedPendArg),
                                 aEnv )
             == STL_SUCCESS );

    STL_TRY( eldAllocPendingMem( (void **) & sCommitPendBuffer,
                                 STL_SIZEOF(eldcHashRelatedPendArg),
                                 aEnv )
             == STL_SUCCESS );
    
    /**
     * Related Transaction List 공간을 할당
     */

    STL_TRY( knlAllocDynamicMem( ELL_MEM_RELATED_TRANS,
                                 STL_SIZEOF(eldcRelatedTransList),
                                 (void**)& sRelatedTrans,
                                 KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * Hash Related Object 공간을 할당
     */

    STL_TRY( eldAllocHashRelatedMem( & sHashRelated, aEnv ) == STL_SUCCESS );

    /**
     * Hash Related Object 설정
     */

    sHashRelated->mVersionInfo.mCreateTransID    = aTransID;
    sHashRelated->mVersionInfo.mCreateCommitSCN  = SML_INFINITE_SCN;
    sHashRelated->mVersionInfo.mDropTransID      = SML_INVALID_TRANSID;
    sHashRelated->mVersionInfo.mDropCommitSCN    = SML_INFINITE_SCN;
    sHashRelated->mVersionInfo.mDropFlag         = STL_FALSE;
    sHashRelated->mVersionInfo.mLastModifiedSCN  = 0;
    sHashRelated->mVersionInfo.mModifyingCnt     = 0;

    ellSetDictHandle( aTransID,
                      0,          /* aViewSCN */
                      & sHashRelated->mRelatedHandle,
                      aObjectType,
                      (void*)aObjectHashElement,
                      STL_FALSE,  /* aRecentCheckable */
                      aEnv );
    
    sHashRelated->mNext = NULL;

    /**
     * X Latch 획득
     */

    STL_TRY( knlAcquireLatch( & aIntoHashElement->mWriteLatch,
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,   /* interval */
                              & sTimeout,
                              KNL_ENV(aEnv) )
             == STL_SUCCESS );
    STL_ASSERT( sTimeout == STL_FALSE );
    sStateLatch = STL_TRUE;

    /**
     * Related Object List 연결
     */

    sHashObjectID = ellGetDictObjectID( & sHashRelated->mRelatedHandle );
    
    sPrevRelated = *aRelatedList;
    if ( sPrevRelated != NULL )
    {
        /**
         * test case DIFF 방지를 위해 넣을 위치를 순서대로 결정
         */

        if ( ellGetDictObjectID( & sPrevRelated->mRelatedHandle ) > sHashObjectID )
        {
            sHashRelated->mNext = sPrevRelated;
            *aRelatedList = sHashRelated;
        }
        else
        {
            while ( sPrevRelated->mNext != NULL )
            {
                sNextObjectID = ellGetDictObjectID( & sPrevRelated->mNext->mRelatedHandle );
                
                if ( sNextObjectID > sHashObjectID )
                {
                    break;
                }
                sPrevRelated = sPrevRelated->mNext;
            }

            sHashRelated->mNext = sPrevRelated->mNext;
            sPrevRelated->mNext = sHashRelated;
        }
    }
    else
    {
        *aRelatedList = sHashRelated;
    }

    /**
     * Base Hash Element 에 변경이 발생하고 있음을 기록
     */

    aIntoHashElement->mVersionInfo.mModifyingCnt++;

    /**
     * Related Transaction List에 연결
     */
    
    sCurRelatedTrans = aIntoHashElement->mRelatedTransList;
    
    while( sCurRelatedTrans != NULL )
    {
        /**
         * My Transaction이라면
         */
        if( sCurRelatedTrans->mTransId == aTransID )
        {
            break;
        }

        sCurRelatedTrans = sCurRelatedTrans->mNext;
    }

    if( sCurRelatedTrans == NULL )
    {
        sRelatedTrans->mRefCount = 1;
        sRelatedTrans->mTransId = aTransID;
        sRelatedTrans->mNext = aIntoHashElement->mRelatedTransList;
        aIntoHashElement->mRelatedTransList = sRelatedTrans;
    }
    else
    {
        /**
         * 기 할당된 메모리는 삭제한다.
         */
        
        STL_TRY( knlFreeDynamicMem( ELL_MEM_RELATED_TRANS,
                                    (void *) sRelatedTrans,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        STL_DASSERT( sCurRelatedTrans->mRefCount > 0 );
        sCurRelatedTrans->mRefCount += 1;
    }
    
    /**
     * X Latch 해제 
     */

    sStateLatch = STL_FALSE;
    STL_TRY( knlReleaseLatch( & aIntoHashElement->mWriteLatch,
                              KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Insert Local Cache
     */

    ELL_SESS_ENV(aEnv)->mLocalModifySeq += 1;
    
    eldcSearchLocalCache( aIntoHashElement, &sLocalCache, aEnv );

    if( sLocalCache == NULL )
    {
        eldcInsertLocalCache( aIntoHashElement, sNewLocalCache, aEnv );
    }
    else
    {
        sLocalCache->mModifySeq = ELL_SESS_ENV(aEnv)->mLocalModifySeq;

        STL_TRY( knlFreeDynamicMem( & sObjMgr->mMemSessObj,
                                    (void *) sNewLocalCache,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        sNewLocalCache = NULL;
    }

    /**
     * Drop Flag 에 대한 Pre-ROLLBACK Action 을 등록
     */

    sSetFlagPendArg.mVersionInfo  = & sHashRelated->mVersionInfo;
    sSetFlagPendArg.mDropFlag = STL_TRUE;
    
    elgAddPreRollbackAction( ELG_PRE_ROLLBACK_SET_FLAG,
                             sSetFlagPendBuffer,
                             & sSetFlagPendArg,
                             STL_SIZEOF(eldcSetFlagRollbackArg),
                             aEnv );

    /**
     * Statement/Transaction ROLLBACK 을 위한 Pending Operation 등록
     * - Hash Related 의 연결을 해제한다.
     */

    sRollbackPendArg.mHashElement  = aIntoHashElement;
    sRollbackPendArg.mStartRelated = aRelatedList;
    sRollbackPendArg.mHashRelated  = sHashRelated;

    elgAddStmtRollbackPendOP( ELG_PEND_ROLLBACK_ADD_HASH_RELATED,
                              sRollbackPendBuffer,
                              & sRollbackPendArg,
                              STL_SIZEOF(eldcHashElementHashRelatedPendArg),
                              aEnv );

    /**
     * Statement 정상 종료후 Transaction COMMIT 을 위한 Pending Operation 등록
     * - Hash Related 의 Create SCN 을 설정한다.
     */

    sCommitPendArg.mHashElement = aIntoHashElement;
    sCommitPendArg.mHashRelated = sHashRelated;

    (void) elgAddStmtSuccessPendOP( ELG_PEND_COMMIT_ADD_HASH_RELATED,
                                    sCommitPendBuffer,
                                    & sCommitPendArg,
                                    STL_SIZEOF(eldcHashRelatedPendArg),
                                    NULL, /* aAgingEventBuffer */
                                    aEnv );

    return STL_SUCCESS;

    STL_FINISH;

    if ( sStateLatch == STL_TRUE )
    {
        (void) knlReleaseLatch( & aIntoHashElement->mWriteLatch,
                                KNL_ENV(aEnv) );
        sStateLatch = STL_FALSE;
    }

    if ( sNewLocalCache != NULL )
    {
        (void) knlFreeDynamicMem( & sObjMgr->mMemSessObj,
                                  (void *) sNewLocalCache,
                                  KNL_ENV(aEnv) );
        sNewLocalCache = NULL;
    }
            
    if ( sSetFlagPendBuffer != NULL )
    {
        (void) eldClearPendingMem( sSetFlagPendBuffer, ELL_ENV(aEnv) );
        sSetFlagPendBuffer = NULL;
    }

    if ( sRollbackPendBuffer != NULL )
    {
        (void) eldClearPendingMem( sRollbackPendBuffer, ELL_ENV(aEnv) );
        sRollbackPendBuffer = NULL;
    }

    if ( sCommitPendBuffer != NULL )
    {
        (void) eldClearPendingMem( sCommitPendBuffer, ELL_ENV(aEnv) );
        sCommitPendBuffer = NULL;
    }
    
    if ( sRelatedTrans != NULL )
    {
        (void) knlFreeDynamicMem( ELL_MEM_RELATED_TRANS,
                                  (void *) sRelatedTrans,
                                  KNL_ENV(aEnv) );
        sRelatedTrans = NULL;
    }
    
    return STL_FAILURE;
}


/**
 * @brief Into Hash Element 에서 Related Object 정보를 제거 
 * @param[in] aTransID           Transaction ID
 * @param[in] aFromHashElement   From Hash Element
 * @param[in] aRelatedList       Related List 의 시작 위치 
 * @param[in] aObjectHashElement 제거할 객체의 Hash Element
 * @param[in] aEnv               Environment
 * @remarks
 * 관련 Object 에 DROP DDL 발생시 이를 추가함.
 */
stlStatus eldcDelHashRelated( smlTransId                aTransID,
                              eldcHashElement         * aFromHashElement,
                              eldcHashRelatedObjList ** aRelatedList,
                              eldcHashElement         * aObjectHashElement,
                              ellEnv                  * aEnv )
{
    ellSessObjectMgr  * sObjMgr = ELL_SESS_OBJ(aEnv);
    
    stlBool    sStateLatch = STL_FALSE;
    stlBool    sTimeout    = STL_FALSE;

    eldcHashRelatedObjList * sHashRelated = NULL;
    
    eldcRelatedTransList   * sRelatedTrans = NULL;
    eldcRelatedTransList   * sCurRelatedTrans = NULL;
    eldcLocalCache         * sLocalCache = NULL;
    eldcLocalCache         * sNewLocalCache = NULL;

    eldcSetFlagRollbackArg            sSetFlagPendArg;
    eldcHashRelatedPendArg            sRollbackPendArg;
    eldcHashElementHashRelatedPendArg sCommitPendArg;

    elgPendOP * sSetFlagPendBuffer  = NULL;
    elgPendOP * sRollbackPendBuffer = NULL;
    elgPendOP * sCommitPendBuffer   = NULL;
    
    void * sAgingEventBuffer = NULL;

    stlMemset( & sSetFlagPendArg, 0x00, STL_SIZEOF(eldcSetFlagRollbackArg) );
    stlMemset( & sRollbackPendArg, 0x00, STL_SIZEOF(eldcHashRelatedPendArg) );
    stlMemset( & sCommitPendArg, 0x00, STL_SIZEOF(eldcHashElementHashRelatedPendArg) );
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aFromHashElement != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aFromHashElement->mObjectDesc != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRelatedList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( *aRelatedList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aObjectHashElement != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aObjectHashElement->mObjectDesc != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * New Local Cache 공간 할당
     */
    
    STL_TRY( knlAllocDynamicMem( & sObjMgr->mMemSessObj,
                                 STL_SIZEOF(eldcLocalCache),
                                 (void **) & sNewLocalCache,
                                 KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * Aging Event Buffer 확보
     */
    
    STL_TRY( knlAllocEnvEventMem( STL_SIZEOF(eldMemFreeAgingEvent),
                                  & sAgingEventBuffer,
                                  KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * Pending Operation 을 위한 Buffer 확보
     */

    STL_TRY( eldAllocPendingMem( (void **) & sSetFlagPendBuffer,
                                 STL_SIZEOF(eldcSetFlagRollbackArg),
                                 aEnv )
             == STL_SUCCESS );

    STL_TRY( eldAllocPendingMem( (void **) & sRollbackPendBuffer,
                                 STL_SIZEOF(eldcHashRelatedPendArg),
                                 aEnv )
             == STL_SUCCESS );

    STL_TRY( eldAllocPendingMem( (void **) & sCommitPendBuffer,
                                 STL_SIZEOF(eldcHashElementHashRelatedPendArg),
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Related Transaction List 공간을 할당
     */

    STL_TRY( knlAllocDynamicMem( ELL_MEM_RELATED_TRANS,
                                 STL_SIZEOF(eldcRelatedTransList),
                                 (void**)& sRelatedTrans,
                                 KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * Latch 설정
     */
    
    STL_TRY( knlAcquireLatch( & aFromHashElement->mWriteLatch,
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,   /* interval */
                              & sTimeout,
                              KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    STL_ASSERT( sTimeout == STL_FALSE );
    sStateLatch = STL_TRUE;

    /**
     * Object Hash Element 검색
     */

    for ( sHashRelated = *aRelatedList;
          sHashRelated != NULL;
          sHashRelated = sHashRelated->mNext )
    {
        if ( sHashRelated->mRelatedHandle.mObjHashElement == aObjectHashElement )
        {
            break;
        }
        else
        {
            continue;
        }
    }
    
    STL_ASSERT( sHashRelated != NULL );

    /**
     * 동시성 정보 설정
     */

    sHashRelated->mVersionInfo.mDropTransID  = aTransID;
    sHashRelated->mVersionInfo.mDropFlag = STL_TRUE;

    /**
     * Base Hash Element 에 변경이 발생하고 있음을 기록
     */

    aFromHashElement->mVersionInfo.mModifyingCnt++;

    /**
     * Related Transaction List에 연결
     */
    
    sCurRelatedTrans = aFromHashElement->mRelatedTransList;
    
    while( sCurRelatedTrans != NULL )
    {
        /**
         * My Transaction이라면
         */
        if( sCurRelatedTrans->mTransId == aTransID )
        {
            break;
        }
        
        sCurRelatedTrans = sCurRelatedTrans->mNext;
    }

    if( sCurRelatedTrans == NULL )
    {
        sRelatedTrans->mRefCount = 1;
        sRelatedTrans->mTransId = aTransID;
        sRelatedTrans->mNext = aFromHashElement->mRelatedTransList;
        aFromHashElement->mRelatedTransList = sRelatedTrans;
    }
    else
    {
        /**
         * 기 할당된 메모리는 삭제한다.
         */
        
        STL_TRY( knlFreeDynamicMem( ELL_MEM_RELATED_TRANS,
                                    (void *) sRelatedTrans,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        STL_DASSERT( sCurRelatedTrans->mRefCount > 0 );
        sCurRelatedTrans->mRefCount += 1;
    }
    
    sStateLatch = STL_FALSE;
    STL_TRY( knlReleaseLatch( & aFromHashElement->mWriteLatch,
                              KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Delete Local Cache
     */

    ELL_SESS_ENV(aEnv)->mLocalModifySeq += 1;
    
    eldcSearchLocalCache( aFromHashElement, &sLocalCache, aEnv );

    if( sLocalCache == NULL )
    {
        eldcInsertLocalCache( aFromHashElement, sNewLocalCache, aEnv );
    }
    else
    {
        STL_TRY( knlFreeDynamicMem( & sObjMgr->mMemSessObj,
                                    (void *) sNewLocalCache,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        sNewLocalCache = NULL;
        
        sLocalCache->mModifySeq = ELL_SESS_ENV(aEnv)->mLocalModifySeq;
    }

    /**
     * Drop Flag 에 대한 Pre-ROLLBACK Action 을 등록
     */

    sSetFlagPendArg.mVersionInfo  = & sHashRelated->mVersionInfo;
    sSetFlagPendArg.mDropFlag = STL_FALSE;
    
    elgAddPreRollbackAction( ELG_PRE_ROLLBACK_SET_FLAG,
                             sSetFlagPendBuffer,
                             & sSetFlagPendArg,
                             STL_SIZEOF(eldcSetFlagRollbackArg),
                             aEnv );

    /**
     * Statement/Transaction ROLLBACK 을 위한 Pending Operation 등록
     * - Dropping Flag 을 원복한다.
     */

    sRollbackPendArg.mHashElement = aFromHashElement;
    sRollbackPendArg.mHashRelated = sHashRelated;

    elgAddStmtRollbackPendOP( ELG_PEND_ROLLBACK_DEL_HASH_RELATED,
                              sRollbackPendBuffer,
                              & sRollbackPendArg,
                              STL_SIZEOF(eldcHashRelatedPendArg),
                              aEnv );
    
    /**
     * Statement 정상 종료 후 Transaction COMMIT 을 위한 Pending Operation 등록
     * - Hash Related 를 연결에서 제거한다.
     */
    
    sCommitPendArg.mHashElement  = aFromHashElement;
    sCommitPendArg.mStartRelated = aRelatedList;
    sCommitPendArg.mHashRelated  = sHashRelated;

    /* STL_TRY( knlLogMsg( NULL, */
    /*                     KNL_ENV(aEnv), */
    /*                     KNL_LOG_LEVEL_INFO, */
    /*                     "[EL eldcDelHashRelated()] [Alloc Event Mem]  %p", */
    /*                     sAgingEventBuffer ) */
    /*          == STL_SUCCESS ); */
    
    elgAddStmtSuccessPendOP( ELG_PEND_COMMIT_DEL_HASH_RELATED,
                             sCommitPendBuffer,
                             & sCommitPendArg,
                             STL_SIZEOF(eldcHashElementHashRelatedPendArg),
                             sAgingEventBuffer,
                             aEnv );

    return STL_SUCCESS;

    STL_FINISH;

    if ( sStateLatch == STL_TRUE )
    {
        (void) knlReleaseLatch( & aFromHashElement->mWriteLatch,
                                KNL_ENV(aEnv) );
        sStateLatch = STL_FALSE;
    }
    
    if ( sNewLocalCache != NULL )
    {
        (void) knlFreeDynamicMem( & sObjMgr->mMemSessObj,
                                  (void *) sNewLocalCache,
                                  KNL_ENV(aEnv) );
        sNewLocalCache = NULL;
    }
    
    if ( sAgingEventBuffer != NULL )
    {
        (void) knlFreeEnvEventMem( sAgingEventBuffer, KNL_ENV(aEnv) );
        sAgingEventBuffer = NULL;
    }
    
    if ( sSetFlagPendBuffer != NULL )
    {
        (void) eldClearPendingMem( sSetFlagPendBuffer, ELL_ENV(aEnv) );
        sSetFlagPendBuffer = NULL;
    }

    if ( sRollbackPendBuffer != NULL )
    {
        (void) eldClearPendingMem( sRollbackPendBuffer, ELL_ENV(aEnv) );
        sRollbackPendBuffer = NULL;
    }

    if ( sCommitPendBuffer != NULL )
    {
        (void) eldClearPendingMem( sCommitPendBuffer, ELL_ENV(aEnv) );
        sCommitPendBuffer = NULL;
    }
    
    if ( sRelatedTrans != NULL )
    {
        (void) knlFreeDynamicMem( ELL_MEM_RELATED_TRANS,
                                  (void *) sRelatedTrans,
                                  KNL_ENV(aEnv) );
        sRelatedTrans = NULL;
    }
    
    return STL_FAILURE;
}



/**
 * @brief ROLLBACK 시 rollback log 작성 전에 수행할 pre-rollback action
 * @param[in] aTransID           Transaction ID
 * @param[in] aPendArgs          Memory Free Pending Action의 Argument
 * @param[in] aAgingEventBuffer  Aging Event Buffer (should null)
 * @param[in] aEnv               Environment 포인터
 * @remarks
 */
stlStatus eldcPreRollbackSetFlag( smlTransId     aTransID,
                                  void         * aPendArgs,
                                  void         * aAgingEventBuffer,
                                  ellEnv       * aEnv )
{
    eldcSetFlagRollbackArg * sPendArg;

    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aPendArgs != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAgingEventBuffer == NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Flag 값을 설정
     */
    
    sPendArg = (eldcSetFlagRollbackArg *) aPendArgs;

    if( sPendArg->mDropFlag == STL_TRUE )
    {
        /**
         * 객체 생성이 실패한 경우
         * - 삭제된 상태로 표시한다.
         */
        sPendArg->mVersionInfo->mDropFlag = STL_TRUE;
        sPendArg->mVersionInfo->mDropTransID = aTransID;
    }
    else
    {
        /**
         * 객체 삭제가 실패한 경우
         * - 삭제하기 이전 상태로 되돌린다.
         */
        sPendArg->mVersionInfo->mDropFlag = STL_FALSE;
        sPendArg->mVersionInfo->mDropTransID = SML_INVALID_TRANSID;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief Dictionary Cache 구성을 위해 할당한 메모리를 Rollback 시 해제
 * @param[in] aTransID           Transaction ID
 * @param[in] aPendArgs          Memory Free Pending Action의 Argument
 * @param[in] aAgingEventBuffer  Aging Event Buffer
 * @param[in] aEnv               Environment 포인터
 * @remarks
 */
stlStatus eldcPendRollbackFreeDictMem( smlTransId     aTransID,
                                       void         * aPendArgs,
                                       void         * aAgingEventBuffer,
                                       ellEnv       * aEnv )
{
    eldMemFreePendArg    * sPendArg;
    eldMemFreeAgingEvent   sMemAgingEvent;
    
    stlMemset( & sMemAgingEvent, 0x00, STL_SIZEOF(eldMemFreeAgingEvent) );
    
    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aPendArgs != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAgingEventBuffer != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * 메모리 해제 Pending Argument
     */
    
    sPendArg = (eldMemFreePendArg *) aPendArgs;

    /**
     * 해제할 메모리를 Aging Event 에 추가
     */

    sMemAgingEvent.mEventType = ELD_AGING_FREE_DICTIONARY_MEMORY;
    sMemAgingEvent.mArg1      = sPendArg->mMemMgr;
    sMemAgingEvent.mArg2      = sPendArg->mAddr;
    sMemAgingEvent.mArg3      = NULL;
    
    /**
     * Aging Event 를 등록
     * - Dictionary Cache Memory 는 Aging 되어야 함.
     */

    STL_TRY( eldAddAgingEventDictMem( aTransID,
                                      (void *) & sMemAgingEvent,
                                      aAgingEventBuffer,
                                      aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief Insert Hash Element 에 대한 ROLLBACK Pending 을 수행 
 * @param[in] aTransID           Transaction Identifier
 * @param[in] aPendArgs          Pending Argument
 * @param[in] aAgingEventBuffer  Aging Event Buffer (should null)
 * @param[in] aEnv               Environment 포인터
 * @remarks
 */
stlStatus eldcPendRollbackInsertHashElement( smlTransId     aTransID,
                                             void         * aPendArgs,
                                             void         * aAgingEventBuffer,
                                             ellEnv       * aEnv )
{
    eldcBucketElementPendArg * sPendArg;

    stlBool    sStateLatch = STL_FALSE;
    
    stlUInt32  sBucketNO = 0;
    stlBool    sTimeout  = STL_FALSE;

    eldcHashElement * sCurrElement = NULL;
    eldcHashElement * sPrevElement = NULL;
    smlScn            sSCN;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aPendArgs != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAgingEventBuffer == NULL, ELL_ERROR_STACK(aEnv) );

    sPendArg = (eldcBucketElementPendArg *) aPendArgs;

    /**
     * Bucket Number
     */

    STL_ASSERT( sPendArg->mBucketNO < sPendArg->mDictHash->mBucketCnt );
    
    sBucketNO = sPendArg->mBucketNO;

    
    /**
     * X Latch 획득 
     */

    STL_TRY( knlAcquireLatch( & sPendArg->mDictHash->mBucket[sBucketNO].mInsDelLatch,
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,   /* interval */
                              & sTimeout,
                              KNL_ENV(aEnv) )
             == STL_SUCCESS );
    STL_ASSERT( sTimeout == STL_FALSE );
    sStateLatch = STL_TRUE;
    
    
    /**
     * 동일한 Hash Element 검색 
     */

    sPrevElement = NULL;

    for ( sCurrElement = sPendArg->mDictHash->mBucket[sBucketNO].mElement;
          sCurrElement != NULL;
          sCurrElement = sCurrElement->mNext )
    {
        if ( sCurrElement == sPendArg->mElement )
        {
            break;
        }
        
        sPrevElement = sCurrElement;
    }
    STL_ASSERT( sCurrElement != NULL );

    /**
     * Commit SCN 획득
     */

    STL_TRY( smlGetCommitScn( aTransID, & sSCN, SML_ENV(aEnv) ) == STL_SUCCESS );

    sCurrElement->mVersionInfo.mCreateCommitSCN = SML_INVISIBLE_CREATE_SCN;
    stlMemBarrier();
    sCurrElement->mVersionInfo.mDropCommitSCN = sSCN;

    /**
     * 연결 해제
     */
    
    if ( sPrevElement == NULL )
    {
        sPendArg->mDictHash->mBucket[sBucketNO].mElement = sCurrElement->mNext;
    }
    else
    {
        sPrevElement->mNext = sCurrElement->mNext;
    }

    /**
     * X Latch 해제 
     */

    sStateLatch = STL_FALSE;
    STL_TRY( knlReleaseLatch( & sPendArg->mDictHash->mBucket[sBucketNO].mInsDelLatch,
                              KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    if ( sStateLatch == STL_TRUE )
    {
        (void) knlReleaseLatch( & sPendArg->mDictHash->mBucket[sBucketNO].mInsDelLatch,
                                KNL_ENV(aEnv) );
        sStateLatch = STL_FALSE;
    }
    
    return STL_FAILURE;
}


/**
 * @brief Insert Hash Element 에 대한 COMMIT pending 을 수행 
 * @param[in] aTransID           Transaction ID
 * @param[in] aPendArgs          Pending Argument
 * @param[in] aAgingEventBuffer  Aging Event Buffer (should null)
 * @param[in] aEnv               Environment 포인터
 * @remarks
 */
stlStatus eldcPendCommitInsertHashElement( smlTransId     aTransID,
                                           void         * aPendArgs,
                                           void         * aAgingEventBuffer,
                                           ellEnv       * aEnv )
{
    eldcElementPendArg * sPendArg;
    smlScn               sSCN = SML_INFINITE_SCN;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aPendArgs != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAgingEventBuffer == NULL, ELL_ERROR_STACK(aEnv) );

    sPendArg = (eldcElementPendArg *) aPendArgs;

    /**
     * Commit SCN 획득
     */

    STL_TRY( smlGetCommitScn( aTransID, & sSCN, SML_ENV(aEnv) ) == STL_SUCCESS );

    /**
     * Hash Element 에 대한 동시성 정보 설정
     */
    
    sPendArg->mElement->mVersionInfo.mCreateCommitSCN = sSCN;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Delete Hash Element 에 대한 ROLLBACK Pending 을 수행 
 * @param[in] aTransNA    N/A
 * @param[in] aPendArgs   Free Hash Element Pending 의 Argument
 * @param[in] aAgingEventBuffer  Aging Event Buffer (should null)
 * @param[in] aEnv        Environment 포인터
 * @remarks
 */
stlStatus eldcPendRollbackDeleteHashElement( smlTransId     aTransNA,
                                             void         * aPendArgs,
                                             void         * aAgingEventBuffer,
                                             ellEnv       * aEnv )
{
    eldcElementPendArg * sPendArg;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aPendArgs != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAgingEventBuffer == NULL, ELL_ERROR_STACK(aEnv) );

    sPendArg = (eldcElementPendArg *) aPendArgs;

    /**
     * Hash Element 에 대한 동시성 정보 설정
     */
    
    sPendArg->mElement->mVersionInfo.mDropCommitSCN = SML_INFINITE_SCN;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Delete Hash Element 에 대한 COMMIT Pending 을 수행 
 * @param[in] aTransID           Transaction ID
 * @param[in] aPendArgs          Pending Argument
 * @param[in] aAgingEventBuffer  Aging Event Buffer
 * @param[in] aEnv               Environment 포인터
 * @remarks
 */
stlStatus eldcPendCommitDeleteHashElement( smlTransId     aTransID,
                                           void         * aPendArgs,
                                           void         * aAgingEventBuffer,
                                           ellEnv       * aEnv )
{
    eldcBucketElementPendArg * sPendArg;
    eldMemFreeAgingEvent       sMemAgingEvent;
    
    stlBool    sStateLatch = STL_FALSE;
    stlUInt32  sBucketNO = 0;
    stlBool    sTimeout  = STL_FALSE;
    smlScn     sSCN;

    stlMemset( & sMemAgingEvent, 0x00, STL_SIZEOF(eldMemFreeAgingEvent) );
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aPendArgs != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAgingEventBuffer != NULL, ELL_ERROR_STACK(aEnv) );

    sPendArg = (eldcBucketElementPendArg *) aPendArgs;

    /**
     * Bucket Number
     */

    STL_ASSERT( sPendArg->mBucketNO < sPendArg->mDictHash->mBucketCnt );
    
    sBucketNO = sPendArg->mBucketNO;

    
    /**
     * X Latch 획득 
     */

    STL_TRY( knlAcquireLatch( & sPendArg->mDictHash->mBucket[sBucketNO].mInsDelLatch,
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,   /* interval */
                              & sTimeout,
                              KNL_ENV(aEnv) )
             == STL_SUCCESS );
    STL_ASSERT( sTimeout == STL_FALSE );
    sStateLatch = STL_TRUE;
    
    /**
     * Commit SCN 획득
     */

    STL_TRY( smlGetCommitScn( aTransID, & sSCN, SML_ENV(aEnv) ) == STL_SUCCESS );

    /**
     * Hash Element 에 대한 동시성 정보 설정
     */

    sPendArg->mElement->mVersionInfo.mDropCommitSCN = sSCN;

    /**
     * Hash Element 메모리를 Aging Event 에 추가
     */

    sMemAgingEvent.mEventType = ELD_AGING_UNLINK_HASH_ELEMENT;
    sMemAgingEvent.mArg1      = & sPendArg->mDictHash->mBucket[sBucketNO];
    sMemAgingEvent.mArg2      = sPendArg->mElement;
    sMemAgingEvent.mArg3      = NULL;
    
    /**
     * Aging Event 를 등록
     */

    STL_TRY( eldAddAgingEventDictMem( aTransID,
                                      (void *) & sMemAgingEvent,
                                      aAgingEventBuffer,
                                      aEnv )
             == STL_SUCCESS );
    
    /**
     * X Latch 해제 
     */

    sStateLatch = STL_FALSE;
    STL_TRY( knlReleaseLatch( & sPendArg->mDictHash->mBucket[sBucketNO].mInsDelLatch,
                              KNL_ENV(aEnv) )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    if ( sStateLatch == STL_TRUE )
    {
        (void) knlReleaseLatch( & sPendArg->mDictHash->mBucket[sBucketNO].mInsDelLatch,
                                KNL_ENV(aEnv) );
        sStateLatch = STL_FALSE;
    }
    
    return STL_FAILURE;
}


/**
 * @brief Add Hash Related 에 대한 ROLLBACK Pending Operation 을 수행 
 * @param[in] aTransID           Transaction Identifier
 * @param[in] aPendArgs          Pending Argument
 * @param[in] aAgingEventBuffer  Aging Event Buffer (should null)
 * @param[in] aEnv               Environment 포인터
 * @remarks
 */
stlStatus eldcPendRollbackAddHashRelated( smlTransId     aTransID,
                                          void         * aPendArgs,
                                          void         * aAgingEventBuffer,
                                          ellEnv       * aEnv )
{
    eldcHashElementHashRelatedPendArg * sPendArg = NULL;

    stlBool    sStateLatch = STL_FALSE;
    stlBool    sTimeout  = STL_FALSE;
    smlScn     sSCN;

    eldcHashRelatedObjList * sCurrRelated = NULL;
    eldcHashRelatedObjList * sPrevRelated = NULL;
    ellVersionInfo         * sVersionInfo = NULL;
    eldcRelatedTransList   * sCurRelatedTrans = NULL;
    eldcRelatedTransList   * sPrvRelatedTrans = NULL;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aPendArgs != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAgingEventBuffer == NULL, ELL_ERROR_STACK(aEnv) );

    sPendArg = (eldcHashElementHashRelatedPendArg *) aPendArgs;

    /**
     * X Latch 획득 
     */

    STL_TRY( knlAcquireLatch( & sPendArg->mHashElement->mWriteLatch,
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,   /* interval */
                              & sTimeout,
                              KNL_ENV(aEnv) )
             == STL_SUCCESS );
    STL_ASSERT( sTimeout == STL_FALSE );
    sStateLatch = STL_TRUE;

    /**
     * Hash Element 에 Related 객체 변경이 종료되었음을 기록한다.
     */

    sVersionInfo = &(sPendArg->mHashElement->mVersionInfo);
    sVersionInfo->mTryModifyCnt++;
    sVersionInfo->mModifyingCnt--;
    
    STL_DASSERT( sVersionInfo->mModifyingCnt >= 0 );
    
    /**
     * 동일한 Related 검색 
     */

    sPrevRelated = NULL;

    for ( sCurrRelated = *(sPendArg->mStartRelated);
          sCurrRelated != NULL;
          sCurrRelated = sCurrRelated->mNext )
    {
        if ( sCurrRelated == sPendArg->mHashRelated )
        {
            break;
        }
        
        sPrevRelated = sCurrRelated;
    }
    STL_ASSERT( sCurrRelated != NULL );

    /**
     * Commit SCN 획득
     */

    STL_TRY( smlGetCommitScn( aTransID, & sSCN, SML_ENV(aEnv) ) == STL_SUCCESS );

    /**
     * Hash Element 에 대한 동시성 정보 설정
     */

    sCurrRelated->mVersionInfo.mCreateCommitSCN = SML_INVISIBLE_CREATE_SCN;
    stlMemBarrier();
    sCurrRelated->mVersionInfo.mDropCommitSCN = sSCN;

    /**
     * 연결 해제
     */
    
    if ( sPrevRelated == NULL )
    {
        *(sPendArg->mStartRelated) = sCurrRelated->mNext;
    }
    else
    {
        sPrevRelated->mNext = sCurrRelated->mNext;
    }

    /**
     * Related Transaction List에서 연결 해제
     */
    
    sCurRelatedTrans = sPendArg->mHashElement->mRelatedTransList;
    sPrvRelatedTrans = NULL;
    
    while( sCurRelatedTrans != NULL )
    {
        /**
         * My Transaction이라면
         */
        if( sCurRelatedTrans->mTransId == aTransID )
        {
            sCurRelatedTrans->mRefCount -= 1;
            
            if( sCurRelatedTrans->mRefCount == 0 )
            {
                if( sPrvRelatedTrans == NULL )
                {
                    sPendArg->mHashElement->mRelatedTransList = NULL;
                }
                else
                {
                    sPrvRelatedTrans->mNext = sCurRelatedTrans->mNext;
                }
                
                STL_TRY( knlFreeDynamicMem( ELL_MEM_RELATED_TRANS,
                                            (void *) sCurRelatedTrans,
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );
            }
            
            break;
        }

        sPrvRelatedTrans = sCurRelatedTrans;
        sCurRelatedTrans = sCurRelatedTrans->mNext;
    }

    /**
     * X Latch 해제 
     */

    sStateLatch = STL_FALSE;
    STL_TRY( knlReleaseLatch( & sPendArg->mHashElement->mWriteLatch,
                              KNL_ENV(aEnv) )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    if ( sStateLatch == STL_TRUE )
    {
        (void) knlReleaseLatch( & sPendArg->mHashElement->mWriteLatch,
                                KNL_ENV(aEnv) );
        sStateLatch = STL_FALSE;
    }
    
    return STL_FAILURE;
}


/**
 * @brief Add Hash Related 에 대해 COMMIT Pending Operation 을 수행 
 * @param[in] aTransID           Transaction ID
 * @param[in] aPendArgs          Free Hash Element Pending 의 Argument
 * @param[in] aAgingEventBuffer  Aging Event Buffer (should null)
 * @param[in] aEnv               Environment 포인터
 * @remarks
 */
stlStatus eldcPendCommitAddHashRelated( smlTransId     aTransID,
                                        void         * aPendArgs,
                                        void         * aAgingEventBuffer,
                                        ellEnv       * aEnv )
{
    stlBool    sStateLatch = STL_FALSE;
    stlBool    sTimeout  = STL_FALSE;
    smlScn     sSCN = SML_INFINITE_SCN;

    eldcHashRelatedPendArg * sPendArg = NULL;
    ellVersionInfo         * sVersionInfo = NULL;
    eldcRelatedTransList   * sCurRelatedTrans = NULL;
    eldcRelatedTransList   * sPrvRelatedTrans = NULL;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aPendArgs != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAgingEventBuffer == NULL, ELL_ERROR_STACK(aEnv) );

    sPendArg = (eldcHashRelatedPendArg *) aPendArgs;

    /**
     * Commit SCN 획득
     */

    STL_TRY( smlGetCommitScn( aTransID, & sSCN, SML_ENV(aEnv) ) == STL_SUCCESS );

    /**
     * X Latch 획득 
     */

    STL_TRY( knlAcquireLatch( & sPendArg->mHashElement->mWriteLatch,
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,   /* interval */
                              & sTimeout,
                              KNL_ENV(aEnv) )
             == STL_SUCCESS );
    STL_ASSERT( sTimeout == STL_FALSE );
    sStateLatch = STL_TRUE;
    
    /**
     * Hash Related 에 대한 동시성 정보 설정
     */
    
    sPendArg->mHashRelated->mVersionInfo.mCreateCommitSCN = sSCN;

    /**
     * Hash Element 에 Related 객체의 변경이 발생되었음을 기록하고,
     * Related 객체의 변경이 종료되었음을 설정한다.
     */

    sVersionInfo = &(sPendArg->mHashElement->mVersionInfo);
    sVersionInfo->mLastModifiedSCN = STL_MAX( sVersionInfo->mLastModifiedSCN, sSCN );
    sVersionInfo->mTryModifyCnt++;
    sVersionInfo->mModifyingCnt--;
    
    STL_DASSERT( sPendArg->mHashElement->mVersionInfo.mModifyingCnt >= 0 );
    
    /**
     * Related Transaction List에서 연결 해제
     */
    
    sCurRelatedTrans = sPendArg->mHashElement->mRelatedTransList;
    sPrvRelatedTrans = NULL;
    
    while( sCurRelatedTrans != NULL )
    {
        /**
         * My Transaction이라면
         */
        if( sCurRelatedTrans->mTransId == aTransID )
        {
            sCurRelatedTrans->mRefCount -= 1;
            
            if( sCurRelatedTrans->mRefCount == 0 )
            {
                if( sPrvRelatedTrans == NULL )
                {
                    sPendArg->mHashElement->mRelatedTransList = NULL;
                }
                else
                {
                    sPrvRelatedTrans->mNext = sCurRelatedTrans->mNext;
                }
                
                STL_TRY( knlFreeDynamicMem( ELL_MEM_RELATED_TRANS,
                                            (void *) sCurRelatedTrans,
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );
            }
            
            break;
        }

        sPrvRelatedTrans = sCurRelatedTrans;
        sCurRelatedTrans = sCurRelatedTrans->mNext;
    }

    /**
     * X Latch 해제 
     */

    sStateLatch = STL_FALSE;
    STL_TRY( knlReleaseLatch( & sPendArg->mHashElement->mWriteLatch,
                              KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    if ( sStateLatch == STL_TRUE )
    {
        (void) knlReleaseLatch( & sPendArg->mHashElement->mWriteLatch,
                                KNL_ENV(aEnv) );
        sStateLatch = STL_FALSE;
    }
    
    return STL_FAILURE;
}


/**
 * @brief Delete Hash Related 에 대해 ROLLBACK Pending Operation 을 수행 
 * @param[in] aTransId           Transaction Identifier
 * @param[in] aPendArgs          Free Hash Element Pending 의 Argument
 * @param[in] aAgingEventBuffer  Aging Event Buffer (should null)
 * @param[in] aEnv               Environment 포인터
 * @remarks
 */
stlStatus eldcPendRollbackDelHashRelated( smlTransId     aTransId,
                                          void         * aPendArgs,
                                          void         * aAgingEventBuffer,
                                          ellEnv       * aEnv )
{
    stlBool    sStateLatch = STL_FALSE;
    stlBool    sTimeout  = STL_FALSE;
    
    eldcHashRelatedPendArg * sPendArg = NULL;
    eldcRelatedTransList   * sCurRelatedTrans = NULL;
    eldcRelatedTransList   * sPrvRelatedTrans = NULL;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aPendArgs != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAgingEventBuffer == NULL, ELL_ERROR_STACK(aEnv) );

    sPendArg = (eldcHashRelatedPendArg *) aPendArgs;
    
    /**
     * X Latch 획득 
     */

    STL_TRY( knlAcquireLatch( & sPendArg->mHashElement->mWriteLatch,
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,   /* interval */
                              & sTimeout,
                              KNL_ENV(aEnv) )
             == STL_SUCCESS );
    STL_ASSERT( sTimeout == STL_FALSE );
    sStateLatch = STL_TRUE;

    /**
     * Hash Element 에 Related 객체 변경이 종료되었음을 기록한다.
     */
    
    sPendArg->mHashElement->mVersionInfo.mTryModifyCnt++;
    sPendArg->mHashElement->mVersionInfo.mModifyingCnt--;
    
    STL_DASSERT( sPendArg->mHashElement->mVersionInfo.mModifyingCnt >= 0 );
    
    /**
     * Hash Related 에 대한 동시성 정보 설정 
     */

    sPendArg->mHashRelated->mVersionInfo.mDropCommitSCN = SML_INFINITE_SCN;

    /**
     * Related Transaction List에서 연결 해제
     */
    
    sCurRelatedTrans = sPendArg->mHashElement->mRelatedTransList;
    sPrvRelatedTrans = NULL;
    
    while( sCurRelatedTrans != NULL )
    {
        /**
         * My Transaction이라면
         */
        if( sCurRelatedTrans->mTransId == aTransId )
        {
            sCurRelatedTrans->mRefCount -= 1;
            
            if( sCurRelatedTrans->mRefCount == 0 )
            {
                if( sPrvRelatedTrans == NULL )
                {
                    sPendArg->mHashElement->mRelatedTransList = NULL;
                }
                else
                {
                    sPrvRelatedTrans->mNext = sCurRelatedTrans->mNext;
                }
                
                STL_TRY( knlFreeDynamicMem( ELL_MEM_RELATED_TRANS,
                                            (void *) sCurRelatedTrans,
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );
            }
            
            break;
        }

        sPrvRelatedTrans = sCurRelatedTrans;
        sCurRelatedTrans = sCurRelatedTrans->mNext;
    }

    /**
     * X Latch 해제 
     */

    sStateLatch = STL_FALSE;
    STL_TRY( knlReleaseLatch( & sPendArg->mHashElement->mWriteLatch,
                              KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    if ( sStateLatch == STL_TRUE )
    {
        (void) knlReleaseLatch( & sPendArg->mHashElement->mWriteLatch,
                                KNL_ENV(aEnv) );
        sStateLatch = STL_FALSE;
    }
    
    return STL_FAILURE;
}


/**
 * @brief Del Hash Related 에 대해 COMMIT Pending Operation 을 수행 
 * @param[in] aTransID           Transaction ID
 * @param[in] aPendArgs          Pending Argument
 * @param[in] aAgingEventBuffer  Aging Event Buffer
 * @param[in] aEnv               Environment 포인터
 * @remarks
 * 다음 연산의 로직은 메모리 관리를 제외하고 동일하다.
 * - Add Hash Related 에 대한 ROLLBACK
 * - Del Hash Related 에 대한 COMMIT
 */
stlStatus eldcPendCommitDelHashRelated( smlTransId     aTransID,
                                        void         * aPendArgs,
                                        void         * aAgingEventBuffer,
                                        ellEnv       * aEnv )
{
    eldcHashElementHashRelatedPendArg * sPendArg = NULL;
    eldMemFreeAgingEvent                sMemAgingEvent;
    ellVersionInfo                    * sVersionInfo = NULL;
    eldcRelatedTransList              * sCurRelatedTrans = NULL;
    eldcRelatedTransList              * sPrvRelatedTrans = NULL;

    stlBool    sStateLatch = STL_FALSE;
    stlBool    sTimeout  = STL_FALSE;
    smlScn     sSCN = SML_INFINITE_SCN;

    stlMemset( & sMemAgingEvent, 0x00, STL_SIZEOF(eldMemFreeAgingEvent) );
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aPendArgs != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAgingEventBuffer != NULL, ELL_ERROR_STACK(aEnv) );

    sPendArg = (eldcHashElementHashRelatedPendArg *) aPendArgs;

    /**
     * X Latch 획득 
     */

    STL_TRY( knlAcquireLatch( & sPendArg->mHashElement->mWriteLatch,
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,   /* interval */
                              & sTimeout,
                              KNL_ENV(aEnv) )
             == STL_SUCCESS );
    STL_ASSERT( sTimeout == STL_FALSE );
    sStateLatch = STL_TRUE;
    
    /**
     * Related Object에 SCN 설정
     */
    
    STL_TRY( smlGetCommitScn( aTransID,
                              & sSCN,
                              SML_ENV(aEnv) ) == STL_SUCCESS );
    
    sPendArg->mHashRelated->mVersionInfo.mDropCommitSCN = sSCN;
    
    /**
     * Hash Related 메모리를 Aging Event 에 추가
     */

    sMemAgingEvent.mEventType = ELD_AGING_UNLINK_HASH_RELATED;
    sMemAgingEvent.mArg1      = sPendArg->mHashElement;
    sMemAgingEvent.mArg2      = sPendArg->mStartRelated;
    sMemAgingEvent.mArg3      = sPendArg->mHashRelated;
    
    /**
     * Aging Event 를 등록
     */

    STL_TRY( eldAddAgingEventDictMem( aTransID,
                                      (void *) & sMemAgingEvent,
                                      aAgingEventBuffer,
                                      aEnv )
             == STL_SUCCESS );
    
    /**
     * Hash Element 에 Related 객체의 변경이 발생되었음을 기록하고,
     * Related 객체의 변경이 종료되었음을 설정한다.
     */
    
    sVersionInfo = &(sPendArg->mHashElement->mVersionInfo);
    sVersionInfo->mLastModifiedSCN = STL_MAX( sVersionInfo->mLastModifiedSCN, sSCN );
    sVersionInfo->mTryModifyCnt++;
    sVersionInfo->mModifyingCnt--;
    
    STL_DASSERT( sPendArg->mHashElement->mVersionInfo.mModifyingCnt >= 0 );
    
    /**
     * Related Transaction List에서 연결 해제
     */
    
    sCurRelatedTrans = sPendArg->mHashElement->mRelatedTransList;
    sPrvRelatedTrans = NULL;
    
    while( sCurRelatedTrans != NULL )
    {
        /**
         * My Transaction이라면
         */
        if( sCurRelatedTrans->mTransId == aTransID )
        {
            sCurRelatedTrans->mRefCount -= 1;
            
            if( sCurRelatedTrans->mRefCount == 0 )
            {
                if( sPrvRelatedTrans == NULL )
                {
                    sPendArg->mHashElement->mRelatedTransList = NULL;
                }
                else
                {
                    sPrvRelatedTrans->mNext = sCurRelatedTrans->mNext;
                }
                
                STL_TRY( knlFreeDynamicMem( ELL_MEM_RELATED_TRANS,
                                            (void *) sCurRelatedTrans,
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );
            }
            
            break;
        }

        sPrvRelatedTrans = sCurRelatedTrans;
        sCurRelatedTrans = sCurRelatedTrans->mNext;
    }

    /**
     * X Latch 해제 
     */

    sStateLatch = STL_FALSE;
    STL_TRY( knlReleaseLatch( & sPendArg->mHashElement->mWriteLatch,
                              KNL_ENV(aEnv) )
             == STL_SUCCESS );

    KNL_BREAKPOINT( KNL_BREAKPOINT_ELDC_PEND_COMMIT_DELETE_HASH_RELATED_COMPLETE, KNL_ENV(aEnv) );
    
    return STL_SUCCESS;

    STL_FINISH;

    if ( sStateLatch == STL_TRUE )
    {
        (void) knlReleaseLatch( & sPendArg->mHashElement->mWriteLatch,
                                KNL_ENV(aEnv) );
        sStateLatch = STL_FALSE;
    }
    
    return STL_FAILURE;
}

/**
 * @brief Local Cache에서 주어지 Object를 검색한다.
 * @param[in] aObject        대상 Object
 * @param[out] aLocalCache   찾은 Local Cache
 * @param[in] aEnv           Environment 포인터
 */
void eldcSearchLocalCache( void            * aObject,
                           eldcLocalCache ** aLocalCache,
                           ellEnv          * aEnv )
{
    eldcLocalCache * sLocalCache = NULL;
    stlBool          sFound = STL_FALSE;
    
    *aLocalCache = NULL;

    STL_RING_FOREACH_ENTRY( & ELL_SESS_ENV(aEnv)->mLocalCache, sLocalCache )
    {
        if( sLocalCache->mObject == aObject )
        {
            sFound = STL_TRUE;
            break;
        }
    }

    if( sFound == STL_TRUE )
    {
        *aLocalCache = sLocalCache;
    }
}


/**
 * @brief 새로운 Local Cache를 삽입한다.
 * @param[in] aObject        대상 Object
 * @param[in] aLocalCache   Local Cache
 * @param[in] aEnv           Environment 포인터
 */
void eldcInsertLocalCache( void            * aObject,
                           eldcLocalCache  * aLocalCache,
                           ellEnv          * aEnv )
{
    STL_RING_INIT_DATALINK( aLocalCache, STL_OFFSETOF(eldcLocalCache, mCacheLink) );
    
    aLocalCache->mObject = aObject;
    aLocalCache->mDropFlag = STL_FALSE;
    aLocalCache->mModifySeq = ELL_SESS_ENV(aEnv)->mLocalModifySeq;

    STL_RING_ADD_LAST( & ELL_SESS_ENV(aEnv)->mLocalCache, aLocalCache );
}


/**
 * @brief 주어진 Local Cache에 삭제 표시를 한다.
 * @param[in] aLocalCache    Local Cache
 * @param[in] aEnv           Environment 포인터
 */
stlStatus eldcDeleteLocalCache( eldcLocalCache * aLocalCache,
                                ellEnv         * aEnv )
{
    aLocalCache->mDropFlag = STL_TRUE;
    aLocalCache->mModifySeq = ELL_SESS_ENV(aEnv)->mLocalModifySeq;
    
    return STL_SUCCESS;
}

/**
 * @brief 모든 Local Cache를 물리적으로 삭제한다.
 * @param[in] aEnv  Environment 포인터
 */
stlStatus eldcFreeLocalCaches( ellEnv * aEnv )
{
    ellSessObjectMgr * sObjMgr = NULL;
    eldcLocalCache   * sLocalCache = NULL;
    eldcLocalCache   * sNextLocalCache = NULL;
    
    sObjMgr = ELL_SESS_OBJ(aEnv);

    STL_RING_FOREACH_ENTRY_SAFE( & ELL_SESS_ENV(aEnv)->mLocalCache, sLocalCache, sNextLocalCache )
    {
        STL_RING_UNLINK( & ELL_SESS_ENV(aEnv)->mLocalCache, sLocalCache );

        STL_TRY( knlFreeDynamicMem( & sObjMgr->mMemSessObj,
                                    (void *) sLocalCache,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} eldcDictHash */
