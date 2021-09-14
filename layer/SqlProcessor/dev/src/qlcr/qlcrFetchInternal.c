/*******************************************************************************
 * qlcrFetchInternal.c
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
 * @file qlcrFetchInternal.c
 * @brief CURSOR FETCH 처리 루틴 
 */

#include <qll.h>

#include <qlDef.h>
#include <qlcrFetchInternal.h>


/**
 * @addtogroup qlcrFetchInternal
 * @{
 */

qlcrFetchFunc gFetchInternalFunc[QLL_FETCH_MAX] =
{
    qlcrCursorInternalFetchNA,        /**< QLL_FETCH_NA */
    
    qlcrCursorInternalFetchNext,      /**< QLL_FETCH_NEXT */
    qlcrCursorInternalFetchPrior,     /**< QLL_FETCH_PRIOR */
    qlcrCursorInternalFetchFirst,     /**< QLL_FETCH_FIRST */
    qlcrCursorInternalFetchLast,      /**< QLL_FETCH_LAST */
    qlcrCursorInternalFetchCurrent,   /**< QLL_FETCH_CURRENT */
    qlcrCursorInternalFetchAbsolute,  /**< QLL_FETCH_ABSOLUTE */
    qlcrCursorInternalFetchRelative   /**< QLL_FETCH_RELATIVE */
};


/**
 * @brief Invalid FETCH
 * @param[in]  aTransID         Trasaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aDBCStmt         DBC Statement
 * @param[in]  aResultSetDesc   Result Set Descriptor
 * @param[in]  aFetchPosition   Fetch Position
 * @param[out] aRowStatus       Cursor Row 의 상태 변경 여부 
 * @param[out] aValidBlockIdx   Target Block 에서의 현재 Row 위치 (결과가 없는 경우 음수임)
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus qlcrCursorInternalFetchNA( smlTransId               aTransID,
                                     smlStatement           * aStmt,
                                     qllDBCStmt             * aDBCStmt,
                                     qllResultSetDesc       * aResultSetDesc,
                                     stlInt64                 aFetchPosition,
                                     qllCursorRowStatus     * aRowStatus,
                                     stlInt32               * aValidBlockIdx,
                                     qllEnv                 * aEnv )
{
    STL_DASSERT(0);
    
    stlPushError( STL_ERROR_LEVEL_ABORT,
                  QLL_ERRCODE_NOT_IMPLEMENTED,
                  NULL,
                  QLL_ERROR_STACK(aEnv),
                  "Invalid Function Sequence: qlcrCursorInternalFetchNA()" );
    
    return STL_FAILURE;
}






/**
 * @brief Named Cursor 의 FETCH NEXT 를 수행한다.
 * @param[in]  aTransID         Trasaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aDBCStmt         DBC Statement
 * @param[in]  aResultSetDesc   Result Set Descriptor
 * @param[in]  aFetchPosition   Fetch Position
 * @param[out] aRowStatus       Cursor Row 의 상태 변경 여부 
 * @param[out] aValidBlockIdx   Target Block 에서의 현재 Row 위치 (결과가 없는 경우 음수임)
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus qlcrCursorInternalFetchNext( smlTransId               aTransID,
                                       smlStatement           * aStmt,
                                       qllDBCStmt             * aDBCStmt,
                                       qllResultSetDesc       * aResultSetDesc,
                                       stlInt64                 aFetchPosition,
                                       qllCursorRowStatus     * aRowStatus,
                                       stlInt32               * aValidBlockIdx,
                                       qllEnv                 * aEnv )
{
    return qlcrCursorInternalFetchRelative( aTransID,
                                            aStmt,
                                            aDBCStmt,
                                            aResultSetDesc,
                                            1,
                                            aRowStatus,
                                            aValidBlockIdx,
                                            aEnv );
}



/**
 * @brief Named Cursor 의 FETCH PRIOR 를 수행한다.
 * @param[in]  aTransID         Trasaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aDBCStmt         DBC Statement
 * @param[in]  aResultSetDesc   Result Set Descriptor
 * @param[in]  aFetchPosition   Fetch Position
 * @param[out] aRowStatus       Cursor Row 의 상태 변경 여부 
 * @param[out] aValidBlockIdx   Target Block 에서의 현재 Row 위치 (결과가 없는 경우 음수임)
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus qlcrCursorInternalFetchPrior( smlTransId               aTransID,
                                        smlStatement           * aStmt,
                                        qllDBCStmt             * aDBCStmt,
                                        qllResultSetDesc       * aResultSetDesc,
                                        stlInt64                 aFetchPosition,
                                        qllCursorRowStatus     * aRowStatus,
                                        stlInt32               * aValidBlockIdx,
                                        qllEnv                 * aEnv )
{
    return qlcrCursorInternalFetchRelative( aTransID,
                                            aStmt,
                                            aDBCStmt,
                                            aResultSetDesc,
                                            -1,
                                            aRowStatus,
                                            aValidBlockIdx,
                                            aEnv );
}



/**
 * @brief Named Cursor 의 FETCH FIRST 를 수행한다.
 * @param[in]  aTransID         Trasaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aDBCStmt         DBC Statement
 * @param[in]  aResultSetDesc   Result Set Descriptor
 * @param[in]  aFetchPosition   Fetch Position
 * @param[out] aRowStatus       Cursor Row 의 상태 변경 여부 
 * @param[out] aValidBlockIdx   Target Block 에서의 현재 Row 위치 (결과가 없는 경우 음수임)
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus qlcrCursorInternalFetchFirst( smlTransId               aTransID,
                                        smlStatement           * aStmt,
                                        qllDBCStmt             * aDBCStmt,
                                        qllResultSetDesc       * aResultSetDesc,
                                        stlInt64                 aFetchPosition,
                                        qllCursorRowStatus     * aRowStatus,
                                        stlInt32               * aValidBlockIdx,
                                        qllEnv                 * aEnv )
{
    return qlcrCursorInternalFetchAbsolute( aTransID,
                                            aStmt,
                                            aDBCStmt,
                                            aResultSetDesc,
                                            1,
                                            aRowStatus,
                                            aValidBlockIdx,
                                            aEnv );
}



/**
 * @brief Named Cursor 의 FETCH LAST 를 수행한다.
 * @param[in]  aTransID         Trasaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aDBCStmt         DBC Statement
 * @param[in]  aResultSetDesc   Result Set Descriptor
 * @param[in]  aFetchPosition   Fetch Position
 * @param[out] aRowStatus       Cursor Row 의 상태 변경 여부 
 * @param[out] aValidBlockIdx   Target Block 에서의 현재 Row 위치 (결과가 없는 경우 음수임)
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus qlcrCursorInternalFetchLast( smlTransId               aTransID,
                                       smlStatement           * aStmt,
                                       qllDBCStmt             * aDBCStmt,
                                       qllResultSetDesc       * aResultSetDesc,
                                       stlInt64                 aFetchPosition,
                                       qllCursorRowStatus     * aRowStatus,
                                       stlInt32               * aValidBlockIdx,
                                       qllEnv                 * aEnv )
{
    return qlcrCursorInternalFetchAbsolute( aTransID,
                                            aStmt,
                                            aDBCStmt,
                                            aResultSetDesc,
                                            -1,
                                            aRowStatus,
                                            aValidBlockIdx,
                                            aEnv );
}


/**
 * @brief Named Cursor 의 FETCH CURRENT 를 수행한다.
 * @param[in]  aTransID         Trasaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aDBCStmt         DBC Statement
 * @param[in]  aResultSetDesc   Result Set Descriptor
 * @param[in]  aFetchPosition   Fetch Position
 * @param[out] aRowStatus       Cursor Row 의 상태 변경 여부 
 * @param[out] aValidBlockIdx   Target Block 에서의 현재 Row 위치 (결과가 없는 경우 음수임)
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus qlcrCursorInternalFetchCurrent( smlTransId               aTransID,
                                          smlStatement           * aStmt,
                                          qllDBCStmt             * aDBCStmt,
                                          qllResultSetDesc       * aResultSetDesc,
                                          stlInt64                 aFetchPosition,
                                          qllCursorRowStatus     * aRowStatus,
                                          stlInt32               * aValidBlockIdx,
                                          qllEnv                 * aEnv )
{
    return qlcrCursorInternalFetchRelative( aTransID,
                                            aStmt,
                                            aDBCStmt,
                                            aResultSetDesc,
                                            0,
                                            aRowStatus,
                                            aValidBlockIdx,
                                            aEnv );
}


/**
 * @brief Named Cursor 의 FETCH ABSOLUTE 를 수행한다.
 * @param[in]  aTransID         Trasaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aDBCStmt         DBC Statement
 * @param[in]  aResultSetDesc   Result Set Descriptor
 * @param[in]  aFetchPosition   Fetch Position
 * @param[out] aRowStatus       Cursor Row 의 상태 변경 여부 
 * @param[out] aValidBlockIdx   Target Block 에서의 현재 Row 위치 (결과가 없는 경우 음수임)
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus qlcrCursorInternalFetchAbsolute( smlTransId               aTransID,
                                           smlStatement           * aStmt,
                                           qllDBCStmt             * aDBCStmt,
                                           qllResultSetDesc       * aResultSetDesc,
                                           stlInt64                 aFetchPosition,
                                           qllCursorRowStatus     * aRowStatus,
                                           stlInt32               * aValidBlockIdx,
                                           qllEnv                 * aEnv )
{
    stlInt64  sAbsPosIdx = 0;
    
    stlInt64  sStartPosIdx = 0;

    stlInt32  sBlockRowIdx = 0;
    
    stlBool   sNeedBlockRead  = STL_FALSE;
    stlBool   sIsValid = STL_FALSE;

    qlvInitPlan * sInitPlan = NULL;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aResultSetDesc != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aValidBlockIdx != NULL, QLL_ERROR_STACK(aEnv) );

    /**************************************************
     * Block Read Position 결정 
     **************************************************/

    sNeedBlockRead = STL_FALSE;
    
    if ( aFetchPosition == 0 )
    {
        /**
         * FETCH ABSOLUTE 0
         * - before the first row 상태가 됨  
         */
        
        aResultSetDesc->mPositionType   = QLL_CURSOR_POSITION_BEFORE_THE_FIRST_ROW;
        aResultSetDesc->mPositionRowIdx = 0;
        aResultSetDesc->mBlockRowIdx    = -1;
        
        sNeedBlockRead = STL_FALSE;
    }
    else
    {
        if ( aFetchPosition > 0 )
        {
            /**
             * FETCH ABSOLUTE positive number
             */

            /**
             * 현재 Block 내에서 읽을 수 있는지 확인
             */

            if ( (aFetchPosition >= aResultSetDesc->mBlockStartPositionIdx) &&
                 (aFetchPosition < aResultSetDesc->mBlockEOFPositionIdx) )
            {
                /**
                 * 현재 Block 내에 존재함.
                 */

                aResultSetDesc->mPositionType   = QLL_CURSOR_POSITION_ON_A_CERTAIN_ROW;
                aResultSetDesc->mPositionRowIdx = aFetchPosition;
                aResultSetDesc->mBlockRowIdx    = (aFetchPosition - aResultSetDesc->mBlockStartPositionIdx);

                sNeedBlockRead = STL_FALSE;
            }
            else
            {
                /**
                 * 현재 Block 내에 존재하지 않음 
                 */

                if ( aResultSetDesc->mKnownLastPosition == STL_TRUE )
                {
                    /**
                     * 전체 Result Set 이 결정됨
                     */
                    
                    if ( aFetchPosition > aResultSetDesc->mLastPositionIdx )
                    {
                        /**
                         * 저장된 위치를 벗어나는 위치임
                         * - Position 정보를 변경함
                         */
                        
                        aResultSetDesc->mPositionType   = QLL_CURSOR_POSITION_AFTER_THE_LAST_ROW;
                        aResultSetDesc->mPositionRowIdx = 0;
                        aResultSetDesc->mBlockRowIdx    = -1;

                        sNeedBlockRead = STL_FALSE;
                    }
                    else
                    {
                        sStartPosIdx = aFetchPosition;

                        /**
                         * Block Read 가 필요함.
                         */
                        
                        sNeedBlockRead = STL_TRUE;
                    }
                }
                else
                {
                    /**
                     * 전체 Result Set 을 알 수 없음 
                     */
                        
                    sStartPosIdx = aFetchPosition;

                    /**
                     * Block Read 가 필요함.
                     */
                    
                    sNeedBlockRead = STL_TRUE;
                }
            }
        }
        else
        {
            /**
             * FETCH ABSOLUTE negative number
             */

            if ( aResultSetDesc->mKnownLastPosition == STL_TRUE )
            {
                /**
                 * 현재 Block 내에서 읽을 수 있는지 확인
                 */
                
                sAbsPosIdx = aResultSetDesc->mLastPositionIdx + aFetchPosition + 1;

                if ( sAbsPosIdx < 1 )
                {
                    /**
                     * 읽을 수 없는 위치에 있음.
                     */

                    aResultSetDesc->mPositionType   = QLL_CURSOR_POSITION_BEFORE_THE_FIRST_ROW;
                    aResultSetDesc->mPositionRowIdx = 0;
                    aResultSetDesc->mBlockRowIdx    = -1;
                    
                    sNeedBlockRead = STL_FALSE;
                }
                else
                {
                    if ( (sAbsPosIdx >= aResultSetDesc->mBlockStartPositionIdx) &&
                         (sAbsPosIdx < aResultSetDesc->mBlockEOFPositionIdx) )
                    {
                        /**
                         * 현재 Block 내에 존재함.
                         */
                        
                        aResultSetDesc->mPositionType   = QLL_CURSOR_POSITION_ON_A_CERTAIN_ROW;
                        aResultSetDesc->mPositionRowIdx = sAbsPosIdx;
                        aResultSetDesc->mBlockRowIdx    = sAbsPosIdx - aResultSetDesc->mBlockStartPositionIdx;
                        
                        sNeedBlockRead = STL_FALSE;
                    }
                    else
                    {
                        sStartPosIdx = sAbsPosIdx;
                        
                        /**
                         * Block Read 가 필요함.
                         */
                        
                        sNeedBlockRead = STL_TRUE;
                    }
                }
            }
            else
            {
                /**
                 * 음수 값을 그대로 넘김
                 */
                
                sStartPosIdx = aFetchPosition;
                
                /**
                 * Block Read 가 필요함.
                 */
                
                sNeedBlockRead = STL_TRUE;
            }
        }
    }

    /**************************************************
     * Block Read 
     **************************************************/
    
    /**
     * Fetch Block
     */

    if ( sNeedBlockRead == STL_TRUE )
    {
        aResultSetDesc->mInstantFetchInfo.mIsEndOfScan = STL_FALSE;
            
        if( (aResultSetDesc->mCursorProperty.mScrollability == ELL_CURSOR_SCROLLABILITY_SCROLL ) ||
            (aResultSetDesc->mCursorProperty.mUpdatability == ELL_CURSOR_UPDATABILITY_FOR_UPDATE ) )
             
        {
            STL_TRY( qlcrFetchCursorMaterializedBlock( aTransID,
                                                       aStmt,
                                                       aDBCStmt,
                                                       aResultSetDesc,
                                                       sStartPosIdx,
                                                       & aResultSetDesc->mInstantFetchInfo.mIsEndOfScan,
                                                       aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            STL_TRY( qllFetchCursorForwardBlock( aTransID,
                                                 aStmt,
                                                 aDBCStmt,
                                                 aResultSetDesc,
                                                 & aResultSetDesc->mInstantFetchInfo.mIsEndOfScan,
                                                 aEnv )
                     == STL_SUCCESS );
        }

        /************************
         * Position 정보 설정
         ************************/
        
        /**
         * 절대값 다시 계산
         */

        if ( aFetchPosition > 0 )
        {
            sAbsPosIdx = aFetchPosition;
        }
        else
        {
            STL_ASSERT( aResultSetDesc->mKnownLastPosition == STL_TRUE );
            sAbsPosIdx = aResultSetDesc->mLastPositionIdx + aFetchPosition + 1;
        }

        /**
         * Cursor Position 설정
         */

        if ( (sAbsPosIdx >= aResultSetDesc->mBlockStartPositionIdx) &&
             (sAbsPosIdx < aResultSetDesc->mBlockEOFPositionIdx) )
        {
            /**
             * 새로이 읽은 Block 내에 포함된 경우
             */
                
            aResultSetDesc->mPositionType   = QLL_CURSOR_POSITION_ON_A_CERTAIN_ROW;
            aResultSetDesc->mPositionRowIdx = sAbsPosIdx;
            aResultSetDesc->mBlockRowIdx    = sAbsPosIdx - aResultSetDesc->mBlockStartPositionIdx;
        }
        else
        {
            /**
             * 새로 읽은 마지막 Block 의 범위도 벗어난 경우
             */

            STL_ASSERT( aResultSetDesc->mKnownLastPosition == STL_TRUE );

            if ( sAbsPosIdx >= aResultSetDesc->mBlockEOFPositionIdx )
            {
                aResultSetDesc->mPositionType   = QLL_CURSOR_POSITION_AFTER_THE_LAST_ROW;
                aResultSetDesc->mPositionRowIdx = 0;
                aResultSetDesc->mBlockRowIdx    = -1;
            }
            else
            {
                STL_DASSERT( sAbsPosIdx < 0 );
                aResultSetDesc->mPositionType   = QLL_CURSOR_POSITION_BEFORE_THE_FIRST_ROW;
                aResultSetDesc->mPositionRowIdx = 0;
                aResultSetDesc->mBlockRowIdx    = -1;
            }
        }
    }
    else
    {
        /* nothing to do */
    }

    /**************************************************
     * Sensitive Read
     **************************************************/

    /**
     * Sensitive Read 로 deleted row 에 의해 Cursor Position 이 바뀌지 않도록 함
     */
    sBlockRowIdx = aResultSetDesc->mBlockRowIdx;
    
    if ( sBlockRowIdx >= 0 )
    {
        /**
         * Sensitivity 처리
         */
        
        if( aResultSetDesc->mCursorProperty.mSensitivity == ELL_CURSOR_SENSITIVITY_SENSITIVE )
        {
            /**
             * Table 구조가 변경되었는지 검사
             * - SELECT Query 만 SENSTIVIE 가 가능함.
             * - DML Query 는 SENSITIVE 할 수 없으며, IX lock 으로 인해 Table 구조가 변경되지 않음.
             */
            
            sInitPlan = (qlvInitPlan *) qllGetInitPlan( aResultSetDesc->mQueryStmt );

            STL_TRY( qllIsRecentAccessObject( aTransID,
                                              sInitPlan->mValidationObjList,
                                              NULL, /* Table 구조 변경 여부만 검사(page 개수 변화 무시) */
                                              & sIsValid,
                                              aEnv )
                     == STL_SUCCESS );
            STL_TRY_THROW( sIsValid == STL_TRUE, RAMP_OBJECT_MODIFIED );
            
            /**
             * RID 를 이용해 새로운 결과값을 얻는다.
             */
            
            STL_TRY ( qlcrCursorSensitiveFetchRID( aStmt,
                                                   aResultSetDesc,
                                                   aRowStatus,
                                                   sBlockRowIdx,
                                                   aEnv )
                      == STL_SUCCESS );
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
     * Output 설정
     */

    *aValidBlockIdx = sBlockRowIdx;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_OBJECT_MODIFIED )
    {
        (void) qlcrCloseResultSet( aResultSetDesc->mQueryStmt, aEnv );

        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INVALID_OBJECT_BY_CONCURRENT_DDL,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief Named Cursor 의 FETCH RELATIVE 를 수행한다.
 * @param[in]  aTransID         Trasaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aDBCStmt         DBC Statement
 * @param[in]  aResultSetDesc   Result Set Descriptor
 * @param[in]  aFetchPosition   Fetch Position
 * @param[out] aRowStatus       Cursor Row 의 상태 변경 여부 
 * @param[out] aValidBlockIdx   Target Block 에서의 현재 Row 위치 (결과가 없는 경우 음수임)
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus qlcrCursorInternalFetchRelative( smlTransId               aTransID,
                                           smlStatement           * aStmt,
                                           qllDBCStmt             * aDBCStmt,
                                           qllResultSetDesc       * aResultSetDesc,
                                           stlInt64                 aFetchPosition,
                                           qllCursorRowStatus     * aRowStatus,
                                           stlInt32               * aValidBlockIdx,
                                           qllEnv                 * aEnv )
{
    stlInt64  sAbsPosIdx = 0;

    stlBool   sNeedFetch = STL_FALSE;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aResultSetDesc != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aValidBlockIdx != NULL, QLL_ERROR_STACK(aEnv) );

    if ( aFetchPosition != 1 )
    {
        STL_PARAM_VALIDATE( aResultSetDesc->mCursorProperty.mScrollability == ELL_CURSOR_SCROLLABILITY_SCROLL,
                            QLL_ERROR_STACK(aEnv) );
    }

    /**
     * 절대값 위치 계산
     */

    switch ( aResultSetDesc->mPositionType )
    {
        case QLL_CURSOR_POSITION_BEFORE_THE_FIRST_ROW:
            {
                if ( aFetchPosition > 0 )
                {
                    sAbsPosIdx = aFetchPosition;
                    sNeedFetch = STL_TRUE;
                }
                else
                {
                    /**
                     * 읽을 수 없는 위치임
                     */
                    
                    aResultSetDesc->mPositionType   = QLL_CURSOR_POSITION_BEFORE_THE_FIRST_ROW;
                    aResultSetDesc->mPositionRowIdx = 0;
                    aResultSetDesc->mBlockRowIdx    = -1;
                    
                    sNeedFetch = STL_FALSE;
                }
                break;
            }
        case QLL_CURSOR_POSITION_ON_A_CERTAIN_ROW:
            {
                sAbsPosIdx = aResultSetDesc->mPositionRowIdx + aFetchPosition;

                if ( sAbsPosIdx > 0 )
                {
                    sNeedFetch = STL_TRUE;
                }
                else
                {
                    /**
                     * 읽을 수 없는 위치임
                     */
                    
                    aResultSetDesc->mPositionType   = QLL_CURSOR_POSITION_BEFORE_THE_FIRST_ROW;
                    aResultSetDesc->mPositionRowIdx = 0;
                    aResultSetDesc->mBlockRowIdx    = -1;
                    
                    sNeedFetch = STL_FALSE;
                }
                
                break;
            }
        case QLL_CURSOR_POSITION_AFTER_THE_LAST_ROW:
            {
                if ( aFetchPosition >= 0 )
                {
                    /**
                     * 읽을 수 없는 위치임
                     */
                    
                    aResultSetDesc->mPositionType   = QLL_CURSOR_POSITION_AFTER_THE_LAST_ROW;
                    aResultSetDesc->mPositionRowIdx = 0;
                    aResultSetDesc->mBlockRowIdx    = -1;
                    
                    sNeedFetch = STL_FALSE;
                }
                else
                {
                    if ( aResultSetDesc->mKnownLastPosition == STL_TRUE )
                    {
                        sAbsPosIdx = aResultSetDesc->mLastPositionIdx + aFetchPosition + 1;
                        
                        if ( sAbsPosIdx > 0 )
                        {
                            sNeedFetch = STL_TRUE;
                        }
                        else
                        {
                            /**
                             * 읽을 수 없는 위치임
                             */
                            
                            aResultSetDesc->mPositionType   = QLL_CURSOR_POSITION_BEFORE_THE_FIRST_ROW;
                            aResultSetDesc->mPositionRowIdx = 0;
                            aResultSetDesc->mBlockRowIdx    = -1;
                            
                            sNeedFetch = STL_FALSE;
                        }
                    }
                    else
                    {
                        sAbsPosIdx = aFetchPosition;
                        sNeedFetch = STL_TRUE;
                    }
                }
                break;
            }
        default:
            {
                STL_ASSERT(0);
                break;
            }
    }

    /**
     * 절대값 Fetch 수행
     */

    if ( sNeedFetch == STL_TRUE )
    {
        STL_TRY( qlcrCursorInternalFetchAbsolute( aTransID,
                                                  aStmt,
                                                  aDBCStmt,
                                                  aResultSetDesc,
                                                  sAbsPosIdx,
                                                  aRowStatus,
                                                  aValidBlockIdx,
                                                  aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        *aValidBlockIdx = aResultSetDesc->mBlockRowIdx;
    }
    
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief RID 를 이용해 Sensitive Fetch를 수행한다.
 * @param[in]     aStmt            Statement
 * @param[in]     aResultSetDesc   Result Set Descriptor
 * @param[out]    aRowStatus       Cursor Row 의 상태 변경 여부 
 * @param[in]     aValidBlockIdx   Target Block 에서의 현재 Row 위치
 * @param[in]     aEnv             Environment
 * @remarks
 */
stlStatus qlcrCursorSensitiveFetchRID( smlStatement           * aStmt,
                                       qllResultSetDesc       * aResultSetDesc,
                                       qllCursorRowStatus     * aRowStatus,
                                       stlInt32                 aValidBlockIdx,
                                       qllEnv                 * aEnv )
{
    stlInt32            sBlockRowIdx = 0;
    qllCursorScanItem * sScanItem = NULL;
    stlBool             sDeleted = STL_FALSE;
    stlBool             sUpdated = STL_FALSE;
    stlBool             sMatched = STL_FALSE;

    knlExprEvalInfo     sTargetEvalInfo;
    knlValueBlockList * sTargetBlock = NULL;
    stlInt32            sTargetIdx = 0;
    
    qllCursorRowStatus  sCursorRowStatus = QLL_CURSOR_ROW_STATUS_NA;
    smlScn              sOrgRowScn;
    /* smlScn              sNewRowScn; */

    /**
     * Paramater Validation
     */

    STL_PARAM_VALIDATE( aResultSetDesc != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRowStatus != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aValidBlockIdx >= 0, QLL_ERROR_STACK(aEnv) );
    
    /**
     * RID 를 이용해 Base Table 들의 새로운 Row 를 읽는다.
     */
    
    sBlockRowIdx = aValidBlockIdx;

    sCursorRowStatus = QLL_CURSOR_ROW_STATUS_NO_CHANGE;

    for ( sScanItem = aResultSetDesc->mScanItemList;
          sScanItem != NULL;
          sScanItem = sScanItem->mNext )
    {
        sDeleted = STL_FALSE;
        sMatched = STL_TRUE;

        sOrgRowScn = sScanItem->mRIDFetchInfo.mScnBlock[sBlockRowIdx];

        /**
         * value block에 대한 프로토콜을 맞추기 위해 USED_COUNT 값을 변경함
         */
        KNL_SET_ALL_BLOCK_USED_CNT( sScanItem->mRIDFetchInfo.mColList, sBlockRowIdx );
        
        STL_TRY( smlFetchRecord(
                     aStmt,
                     & sScanItem->mRIDFetchInfo,
                     & sScanItem->mScanRowBlock->mRidBlock[sBlockRowIdx],
                     sBlockRowIdx,
                     & sMatched,
                     & sDeleted,
                     & sUpdated,
                     SML_ENV(aEnv) )
                 == STL_SUCCESS );

        /* sNewRowScn = sScanItem->mRIDFetchInfo.mScnBlock[sBlockRowIdx]; */
        sScanItem->mRIDFetchInfo.mScnBlock[sBlockRowIdx] = sOrgRowScn;
        
        if ( sDeleted == STL_TRUE )
        {
            STL_DASSERT( sUpdated == STL_FALSE );
                
            /**
             * 삭제된 Row 인 경우
             */
            
            sBlockRowIdx = -1;
            sCursorRowStatus = QLL_CURSOR_ROW_STATUS_DELETED;
            break;
        }
        else
        {
            if( sUpdated == STL_TRUE )
            {
                sCursorRowStatus = QLL_CURSOR_ROW_STATUS_UPDATED;
            }
            else
            {
                /* no change */
            }
        }
    }

    /**
     * 새로운 Row 기준으로 Target Expression 을 수행한다.
     */
    
    if ( sBlockRowIdx >= 0 )
    {
        for ( sTargetBlock = aResultSetDesc->mTargetBlock,
                  sTargetIdx = 0;
              sTargetBlock != NULL;
              sTargetBlock = sTargetBlock->mNext,
                  sTargetIdx++ )
        {
            if( aResultSetDesc->mTargetCodeStack[sTargetIdx].mEntryCount > 0 )
            {
                /**
                 * Evaluation Information 구성
                 */

                sTargetEvalInfo.mExprStack =
                    & aResultSetDesc->mTargetCodeStack[sTargetIdx];
                sTargetEvalInfo.mContext     = aResultSetDesc->mTargetDataContext;
                sTargetEvalInfo.mResultBlock = sTargetBlock;
                sTargetEvalInfo.mBlockIdx    = sBlockRowIdx;
                sTargetEvalInfo.mBlockCount  = 1;

            
                /**
                 * Targeting Expression 수행
                 */
                
                STL_TRY( knlEvaluateOneExpression( & sTargetEvalInfo,
                                                   KNL_ENV(aEnv) )
                         == STL_SUCCESS );
            }
        }
    }
    else
    {
        /**
         * Row 가 삭제됨
         */
    }
    
    /**
     * Output 설정
     */

    *aRowStatus = sCursorRowStatus;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Cursor Result Set Desciptor 를 이용해 Materialized Block Fetch 를 수행한다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aDBCStmt         DBC Statement
 * @param[in]  aResultSetDesc   Cursor Result Set Desc
 * @param[in]  aStartPosition   Start Position ( 음수일 경우는 Result Set 의 END 로부터의 위치 )
 * @param[out] aEOF             End Of File
 * @param[in]  aEnv             Environment
 * @remarks
 * Scroll Cursor 용 Block Read
 * Forward Only Cursor 인 경우도 이전 block 을 읽는 경우가 있다.
 * - C/S 환경에서
 *  - SELECT .. FOR UPDATE 구문으로 client result 에 여러 block 을 fetch
 *  - 이후 SQLFetch() 후 SQLSetPos() 으로 이미 지나간 block 의 위치를 지정하는 경우 
 */
stlStatus qlcrFetchCursorMaterializedBlock( smlTransId               aTransID,
                                            smlStatement           * aStmt,
                                            qllDBCStmt             * aDBCStmt,
                                            qllResultSetDesc       * aResultSetDesc,
                                            stlInt64                 aStartPosition,
                                            stlBool                * aEOF,
                                            qllEnv                 * aEnv )
{
    stlInt64 sBeginPosIdx = 0;
    
    stlInt64 sBeginZeroPos = 0;
    stlInt64 sEndZeroPos   = 0;

    stlInt32  sBlockAllocCnt = 0;
    
    qlnInstantTable * sInstantTable = NULL;
    
    stlBool  sSelectEOF = STL_FALSE;
    stlBool  sHasData   = STL_FALSE;
    
    stlInt64 sSkipCnt = 0;
    stlInt64 sUsedCnt = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aResultSetDesc != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aResultSetDesc->mMaterialized == STL_TRUE, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aStartPosition != 0, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aEOF != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * Fixed Table 정보 누적
     */

    qllAddCallCntCursorFetch( aEnv );

    STL_TRY_THROW( aResultSetDesc->mIsOpen == STL_TRUE, RAMP_RESULT_SET_NOT_EXIST );
    
    sInstantTable = (qlnInstantTable *) aResultSetDesc->mInstantTable;

    /**************************************************
     * 절대값 조정 
     **************************************************/

    if ( aStartPosition > 0 )
    {
        sBeginPosIdx = aStartPosition;
    }
    else
    {
        if ( aResultSetDesc->mKnownLastPosition == STL_TRUE )
        {
            sBeginPosIdx = aResultSetDesc->mLastPositionIdx + aStartPosition + 1;

            if ( sBeginPosIdx <= 0 )
            {
                sHasData = STL_FALSE;
                STL_THROW( RAMP_FINISH );
            }
        }
        else
        {
            /**
             * 알 수 없음
             */
            
            sBeginPosIdx = aStartPosition;
        }
    }
    
    /**************************************************
     * 방향성을 고려한 Block Read Position 조정 
     **************************************************/

    sBlockAllocCnt = KNL_GET_BLOCK_ALLOC_CNT( aResultSetDesc->mTargetBlock );
    
    if ( sBeginPosIdx > 0 )
    {
        if ( sBeginPosIdx <= sBlockAllocCnt )
        {
            /**
             * Result Set 의 첫번째 Block 에 포함되는 경우
             */

            sBeginPosIdx = 1;
        }
        else
        {
            if ( (sBeginPosIdx >= aResultSetDesc->mBlockStartPositionIdx - sBlockAllocCnt) &&
                 (sBeginPosIdx < aResultSetDesc->mBlockStartPositionIdx) )
            {
                /**
                 * 최근 Read Block 의 바로 이전 Block 에 포함되는 경우
                 */

                sBeginPosIdx = aResultSetDesc->mBlockStartPositionIdx - sBlockAllocCnt;
            }
            else
            {
                if ( (sBeginPosIdx >= aResultSetDesc->mBlockEOFPositionIdx) &&
                     (sBeginPosIdx < aResultSetDesc->mBlockEOFPositionIdx + sBlockAllocCnt) )
                {
                    /**
                     * 최근 Read Block 의 바로 다음 Block 에 포함되는 경우
                     */

                    sBeginPosIdx = aResultSetDesc->mBlockEOFPositionIdx;
                }
                else
                {
                     /* nohting to do */
                }

                if ( (aResultSetDesc->mKnownLastPosition == STL_TRUE) &&
                     (sBeginPosIdx > aResultSetDesc->mLastPositionIdx - sBlockAllocCnt) &&
                     (sBeginPosIdx <= aResultSetDesc->mLastPositionIdx) )
                {
                    /**
                     * Result Set 의 마지막 Block 에 포함되는 경우
                     */

                    sBeginPosIdx = aResultSetDesc->mLastPositionIdx - sBlockAllocCnt + 1;
                }
                else
                {
                    /* nothing to do */
                }
                        
            }
        }
    }
    else
    {
        /* noting to do */
    }
    
    /*********************************************************************
     * 읽을 Position 구간을 결정 
     *********************************************************************/


    if ( sBeginPosIdx > 0 )
    {
        sBeginZeroPos = sBeginPosIdx - 1;
        sEndZeroPos   = sBeginZeroPos + sBlockAllocCnt;

        if ( aResultSetDesc->mKnownLastPosition == STL_TRUE )
        {
            /**
             * nothing to do
             */
        }
        else
        {
            /**
             * EOF 를 만나거나, End 위치보다 더 많이 저장할때까지 수행
             */

            sSelectEOF = STL_FALSE;
            while ( aResultSetDesc->mLastPositionIdx < sEndZeroPos )
            {
                /**
                 * Fetch Select
                 */
                
                STL_TRY( qlfFetchSelect( aTransID,
                                         aStmt,
                                         aDBCStmt,
                                         aResultSetDesc->mQueryStmt,
                                         & sSelectEOF,
                                         aEnv )
                     == STL_SUCCESS );

                /**
                 * Instant Table 에 저장
                 */
                
                sSkipCnt = KNL_GET_BLOCK_SKIP_CNT( aResultSetDesc->mTargetBlock );
                sUsedCnt = KNL_GET_BLOCK_USED_CNT( aResultSetDesc->mTargetBlock );

                if ( sUsedCnt > 0 )
                {
                    /* target block 과 store block 은 공유됨 */
                    STL_TRY( qlnxAddMeetBlockIntoInstantTable( aResultSetDesc->mInstantTable,
                                                               aResultSetDesc->mInstantFetchInfo.mRowBlock,
                                                               aResultSetDesc->mStoreBlockList,
                                                               NULL, /* aMeetArray */
                                                               sSkipCnt,
                                                               sUsedCnt,
                                                               aEnv )
                             == STL_SUCCESS );
                    
                    aResultSetDesc->mLastPositionIdx += (sUsedCnt - sSkipCnt);
                }
                else
                {
                    STL_DASSERT( sSelectEOF == STL_TRUE );
                }

                if ( sSelectEOF == STL_TRUE )
                {
                    aResultSetDesc->mKnownLastPosition = STL_TRUE;
                    aResultSetDesc->mLastPositionIdx   = sInstantTable->mRecCnt;
                    break;
                }                
            }
        }
    }
    else
    {
        STL_ASSERT( aResultSetDesc->mKnownLastPosition == STL_FALSE );
        
        /**
         * EOF 를 만날때까지 Fetch 
         */

        sSelectEOF = STL_FALSE;
        while ( 1 )
        {
            /**
             * Fetch Select
             */
                
            STL_TRY( qlfFetchSelect( aTransID,
                                     aStmt,
                                     aDBCStmt,
                                     aResultSetDesc->mQueryStmt,
                                     & sSelectEOF,
                                     aEnv )
                     == STL_SUCCESS );

            /**
             * Instant Table 에 저장
             */
                
            sSkipCnt = KNL_GET_BLOCK_SKIP_CNT( aResultSetDesc->mTargetBlock );
            sUsedCnt = KNL_GET_BLOCK_USED_CNT( aResultSetDesc->mTargetBlock );

            if ( sUsedCnt > 0 )
            {
                
                /* target block 과 store block 은 공유됨 */
                STL_TRY( qlnxAddMeetBlockIntoInstantTable( aResultSetDesc->mInstantTable,
                                                           aResultSetDesc->mInstantFetchInfo.mRowBlock,
                                                           aResultSetDesc->mStoreBlockList,
                                                           NULL, /* aMeetArray */
                                                           sSkipCnt,
                                                           sUsedCnt,
                                                           aEnv )
                         == STL_SUCCESS );
            }
            else
            {
                STL_DASSERT( sSelectEOF == STL_TRUE );
            }

            if ( sSelectEOF == STL_TRUE )
            {
                break;
            }            
        }

        aResultSetDesc->mKnownLastPosition = STL_TRUE;
        aResultSetDesc->mLastPositionIdx   = sInstantTable->mRecCnt;

        sBeginZeroPos = aResultSetDesc->mLastPositionIdx + aStartPosition;
        sEndZeroPos   = sBeginZeroPos + KNL_GET_BLOCK_ALLOC_CNT( aResultSetDesc->mTargetBlock );
    }

    /****************************************************************
     * 유효한 범위의 위치인지 확인 
     ****************************************************************/

    if( (sBeginZeroPos >= 0) && (sBeginZeroPos < aResultSetDesc->mLastPositionIdx) )
    {
        /**
         * Instant Table 에 저장되어 있는 범위임
         */
        sHasData = STL_TRUE;
    }
    else
    {
        /**
         * Instant Table 에 저장되어 있는 범위가 아님
         */
        
        sHasData = STL_FALSE;
        STL_THROW( RAMP_FINISH );
    }
                   
    /****************************************************************
     * Instant Table 에서 Fetch 
     ****************************************************************/

    /**
     * Iterator 를 시작 Position 으로 이동
     */
    
    STL_TRY( smlMoveToPosNum( sInstantTable->mIterator,
                              sBeginZeroPos,
                              SML_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Iterator 를 통해 읽음 
     */
    
    STL_TRY( qlcrFetchBlockFromMaterialResult( aTransID,
                                               aResultSetDesc,
                                               & sHasData,
                                               aEnv )
             == STL_SUCCESS );
    STL_DASSERT( sHasData == STL_TRUE );
    
    /*************************************************************
     * Block Position 설정
     *************************************************************/

    STL_RAMP( RAMP_FINISH );

    if ( sHasData == STL_TRUE )
    {
        sSkipCnt = KNL_GET_BLOCK_SKIP_CNT( aResultSetDesc->mTargetBlock );
        sUsedCnt = KNL_GET_BLOCK_USED_CNT( aResultSetDesc->mTargetBlock );
        
        /* aResultSetDesc->mKnownLastPosition = STL_FALSE; */
        /* aResultSetDesc->mLastPositionIdx   += (sUsedCnt - sSkipCnt); */
        
        aResultSetDesc->mBlockStartIdx = sSkipCnt;
        aResultSetDesc->mBlockEOFIdx   = sUsedCnt;
        
        aResultSetDesc->mBlockStartPositionIdx = sBeginZeroPos + 1;
        aResultSetDesc->mBlockEOFPositionIdx   = aResultSetDesc->mBlockStartPositionIdx + (sUsedCnt - sSkipCnt);

        /* data 가 존재하고 끝까지 읽은 경우 */
        if( aResultSetDesc->mKnownLastPosition == STL_TRUE )
        {
            if ( aResultSetDesc->mBlockEOFPositionIdx > aResultSetDesc->mLastPositionIdx )
            {
                *aEOF = STL_TRUE;
            }
            else
            {
                *aEOF = STL_FALSE;
            }
        }
        else
        {
            *aEOF = STL_FALSE;
        }
    }
    else
    {
        aResultSetDesc->mBlockStartIdx = -1;
        aResultSetDesc->mBlockEOFIdx   = 0;
        
        aResultSetDesc->mBlockStartPositionIdx = 0;
        aResultSetDesc->mBlockEOFPositionIdx   = 1;

        *aEOF = STL_TRUE;
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_RESULT_SET_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_RESULT_SET_NOT_EXIST,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_FINISH;

    /**
     * fetch 시 정상적인 에러가 발생할 수 있으므로 SQL Statement를 초기화 하면 안된다.
     */
    
    (void) qlcrCloseResultSet( aResultSetDesc->mQueryStmt, aEnv );
    
    return STL_FAILURE;
}



/** @} qlcrFetchInternal */
