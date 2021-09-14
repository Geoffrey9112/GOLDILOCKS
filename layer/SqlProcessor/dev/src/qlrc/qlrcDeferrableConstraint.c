/*******************************************************************************
 * qlrcDeferrableConstraint.c
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
 * @file qlrcDeferrableConstraint.c
 * @brief SQL Processor Layer Deferrable Constraint
 */

#include <qll.h>
#include <qlDef.h>

/**
 * @defgroup qlrcDeferrableConstraint Deferrable Constraint 내부 함수 
 * @ingroup qlInternal
 * @{
 */


/*********************************************************
 * Initialize & Finalize
 *********************************************************/

/**
 * @brief Deferrable Constraint Information 을 초기화한다.
 * @param[in]  aSessEnv  Session Environment
 */
void qlrcInitDeferConstraintInfo( qllSessionEnv * aSessEnv )
{
    aSessEnv->mDeferConstInfo.mSetModeHistory = NULL;
    
    aSessEnv->mDeferConstInfo.mTransCollisionHistory = NULL;
    aSessEnv->mDeferConstInfo.mTransCollisionSummary = NULL;

    aSessEnv->mDeferConstInfo.mStmtCollisionList   = NULL;
    aSessEnv->mDeferConstInfo.mNewCollisionSummary = NULL;
}




/**
 * @brief Statement 종료시 Deferred Constarint 의 Collision 정보를 설정한다.
 * @param[in]  aEnv               Environment
 * @remarks
 * Collision History 와 Collision Summary 에 정보를 추가한다.
 * Statement 정상 종료시 수행되므로 Memory 할당이 없어야 함.
 */
void qlrcStmtEndDeferConstraint( qllEnv * aEnv )
{
    qllSessionEnv * sSessEnv = QLL_SESS_ENV(aEnv);

    qllCollisionItem * sTransItem = NULL;
    qllCollisionItem * sFoundItem = NULL;
    
    qllCollisionItem * sStmtItem = NULL;
    qllCollisionItem * sStmtItemNext = NULL;

    /**
     * Statement Collision 이 없다면 통과
     */
    
    STL_TRY_THROW( sSessEnv->mDeferConstInfo.mStmtCollisionList != NULL, RAMP_FINISH );
    
    /********************************************************************
     * New Collision Summary 를 Transaction Collision Summary 에 추가
     ********************************************************************/

    if ( sSessEnv->mDeferConstInfo.mNewCollisionSummary == NULL )
    {
        /**
         * 새로 발생한 collision summary 가 없음
         */
    }
    else
    {
        /**
         * 새로 발생한 collision summary 를 transaction collision summary 에 연결
         */
        
        sStmtItem = sSessEnv->mDeferConstInfo.mNewCollisionSummary;
        while ( sStmtItem->mNext != NULL )
        {
            sStmtItem = sStmtItem->mNext;
        }

        sStmtItem->mNext = sSessEnv->mDeferConstInfo.mTransCollisionSummary;
        sSessEnv->mDeferConstInfo.mTransCollisionSummary = sSessEnv->mDeferConstInfo.mNewCollisionSummary;
        sSessEnv->mDeferConstInfo.mNewCollisionSummary   = NULL;
    }

    /********************************************************************
     * Statement Collision 정보를 Transaction Collision 정보에 추가
     ********************************************************************/
    
    sStmtItem = sSessEnv->mDeferConstInfo.mStmtCollisionList;

    while ( sStmtItem != NULL )
    {
        sStmtItemNext = sStmtItem->mNext;

        /********************************************************************
         * Statement Collision List 를 Transaction Collision Summmary 에 누적
         ********************************************************************/

        /**
         * Transaction Collision Summary 검색
         */
        
        sFoundItem = NULL;
        for ( sTransItem = sSessEnv->mDeferConstInfo.mTransCollisionSummary;
              sTransItem != NULL;
              sTransItem = sTransItem->mNext )
        {
            if ( qlrcIsSameCollisionItem( sStmtItem, sTransItem ) == STL_TRUE )
            {
                sFoundItem = sTransItem;
                break;
            }
        }

        /**
         * Statement Collision 을 Transaction Collision Summary 에 누적
         */

        STL_DASSERT( sFoundItem != NULL );

        sFoundItem->mCollisionCount += sStmtItem->mCollisionCount;
        
        /********************************************************************
         * Statement Collision List 를 Transaction Collision History 에 누적
         ********************************************************************/
        
        /**
         * Savepoint 이후 시점에 발생한 동일한 Transaction Collision History 에 존재하는 지 검사
         */

        sFoundItem = NULL;
        for ( sTransItem = sSessEnv->mDeferConstInfo.mTransCollisionHistory;
              sTransItem != NULL;
              sTransItem = sTransItem->mNext )
        {
            if ( sTransItem->mItemType == QLL_COLL_ITEM_SAVEPOINT )
            {
                /**
                 * Savepoint 가 존재함
                 */
                break;
            }
            else
            {
                if ( qlrcIsSameCollisionItem( sStmtItem, sTransItem ) == STL_TRUE )
                {
                    sFoundItem = sTransItem;
                    break;
                }
            }
        }

        /**
         * Statement Collision 을 Transaction Collision History 에 누적
         */
        
        if ( sFoundItem == NULL )
        {
            /**
             * 새로운 Collision History 인 경우
             */
            
            sStmtItem->mNext = sSessEnv->mDeferConstInfo.mTransCollisionHistory;
            sSessEnv->mDeferConstInfo.mTransCollisionHistory = sStmtItem;
        }
        else
        {
            /**
             * 이미 존재하는 Collision 인 경우 값을 누적하고 메모리 반납
             */
            
            sFoundItem->mCollisionCount += sStmtItem->mCollisionCount;

            if ( knlFreeDynamicMem( & QLL_COLLISION_MEM(aEnv), sStmtItem, KNL_ENV(aEnv) ) == STL_SUCCESS )
            {
                /* ok */
            }
            else
            {
                STL_DASSERT(0);
            }
        }

        /**
         * 다음 Collision
         */
        
        sStmtItem = sStmtItemNext;
    }
    
    STL_RAMP( RAMP_FINISH );

    sSessEnv->mDeferConstInfo.mStmtCollisionList   = NULL;
    sSessEnv->mDeferConstInfo.mNewCollisionSummary = NULL;
}


/**
 * @brief Statement 종료 실패시 Deferred Constarint 의 Statement Collision 정보를 정리한다.
 * @param[in]  aEnv               Environment
 * @remarks
 */
void qlrcStmtRollbackDeferConstraint( qllEnv * aEnv )
{
    qllSessionEnv * sSessEnv = QLL_SESS_ENV(aEnv);
    
    qllCollisionItem * sItem     = NULL;
    qllCollisionItem * sItemNext = NULL;

    /**
     * Statement Collision List 반납
     */
    
    sItem = sSessEnv->mDeferConstInfo.mStmtCollisionList;

    while ( sItem != NULL )
    {
        sItemNext = sItem->mNext;

        if ( knlFreeDynamicMem( & QLL_COLLISION_MEM(aEnv), sItem, KNL_ENV(aEnv) ) == STL_SUCCESS )
        {
            /* ok */
        }
        else
        {
            STL_DASSERT(0);
        }
        
        sItem = sItemNext;
    }

    /**
     * Statement New Collision Summary 반납
     */
    
    sItem = sSessEnv->mDeferConstInfo.mNewCollisionSummary;

    while ( sItem != NULL )
    {
        sItemNext = sItem->mNext;

        if ( knlFreeDynamicMem( & QLL_COLLISION_MEM(aEnv), sItem, KNL_ENV(aEnv) ) == STL_SUCCESS )
        {
            /* ok */
        }
        else
        {
            STL_DASSERT(0);
        }
        
        sItem = sItemNext;
    }
    
    sSessEnv->mDeferConstInfo.mStmtCollisionList   = NULL;
    sSessEnv->mDeferConstInfo.mNewCollisionSummary = NULL;
}


/**
 * @brief Transaction 종료 시 Deferrable Constraint Information 을 Reset 한다.
 * @param[in]  aEnv  Environment
 * @remarks
 */
void qlrcTransEndDeferConstraint( qllEnv * aEnv )
{
    qlrcResetDeferConstraint( aEnv );
}

/**
 * @brief Deferrable Constraint Information 을 Reset 한다.
 * @param[in]  aEnv  Environment
 * @remarks  Transaction COMMIT/ROLLBACK 시 호출
 */
void qlrcResetDeferConstraint( qllEnv * aEnv )
{
    qllSessionEnv * sSessEnv = QLL_SESS_ENV(aEnv);

    qllSetConstraintModeItem * sMode  = NULL;
    qllSetConstraintModeItem * sModeNext  = NULL;
    
    qllCollisionItem * sItem     = NULL;
    qllCollisionItem * sItemNext = NULL;

    /**
     * Statement level 의 collision 정보가 남아있지 않아야 함
     */
    
    STL_DASSERT( sSessEnv->mDeferConstInfo.mStmtCollisionList == NULL );
    STL_DASSERT( sSessEnv->mDeferConstInfo.mNewCollisionSummary == NULL );
    
    /**
     * SET CONSTRAINT list 반납
     */
    
    sMode = sSessEnv->mDeferConstInfo.mSetModeHistory;

    while ( sMode != NULL )
    {
        sModeNext = sMode->mNext;

        if ( knlFreeDynamicMem( & QLL_COLLISION_MEM(aEnv), sMode, KNL_ENV(aEnv) ) == STL_SUCCESS )
        {
            /* ok */
        }
        else
        {
            STL_DASSERT(0);
        }
        
        sMode = sModeNext;
    }

    
    /**
     * Transaction Collision List 반납
     */
    
    sItem = sSessEnv->mDeferConstInfo.mTransCollisionHistory;

    while ( sItem != NULL )
    {
        sItemNext = sItem->mNext;

        if ( knlFreeDynamicMem( & QLL_COLLISION_MEM(aEnv), sItem, KNL_ENV(aEnv) ) == STL_SUCCESS )
        {
            /* ok */
        }
        else
        {
            STL_DASSERT(0);
        }
        
        sItem = sItemNext;
    }

    /**
     * Transaction Collision Summary 반납
     */
    
    sItem = sSessEnv->mDeferConstInfo.mTransCollisionSummary;

    while ( sItem != NULL )
    {
        sItemNext = sItem->mNext;

        if ( knlFreeDynamicMem( & QLL_COLLISION_MEM(aEnv), sItem, KNL_ENV(aEnv) ) == STL_SUCCESS )
        {
            /* ok */
        }
        else
        {
            STL_DASSERT(0);
        }
        
        sItem = sItemNext;
    }

    /**
     * Deferred Constraint 정보 초기화
     */
    
    qlrcInitDeferConstraintInfo( sSessEnv );
}

/**
 * @brief Savepoint 정보를 Deferred Constraint 정보에 추가한다.
 * @param[in] aSavepointTimestamp Savepoint Timestamp
 * @param[in] aEnv                Environment
 */

stlStatus qlrcAddSavepointIntoDeferConstraint( stlInt64     aSavepointTimestamp,
                                               qllEnv     * aEnv )
{
    qllSessionEnv * sSessEnv = QLL_SESS_ENV(aEnv);

    qllSetConstraintModeItem * sMode = NULL;
    qllCollisionItem         * sItem = NULL;

    /**
     * 공간 할당
     */

    STL_TRY( knlAllocDynamicMem( & QLL_COLLISION_MEM(aEnv),
                                 STL_SIZEOF(qllSetConstraintModeItem),
                                 (void **) & sMode,
                                 KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sMode, 0x00, STL_SIZEOF(qllSetConstraintModeItem) );

    STL_TRY( knlAllocDynamicMem( & QLL_COLLISION_MEM(aEnv),
                                 STL_SIZEOF(qllCollisionItem),
                                 (void **) & sItem,
                                 KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sItem, 0x00, STL_SIZEOF(qllCollisionItem) );

    /**
     * Set Mode 정보 추가 
     */

    sMode->mSetMode = QLL_SET_CONSTRAINT_MODE_SAVEPOINT;
    sMode->mConstID = ELL_DICT_OBJECT_ID_NA;
    sMode->mSaveTimestamp = aSavepointTimestamp;
    
    sMode->mNext = sSessEnv->mDeferConstInfo.mSetModeHistory;
    sSessEnv->mDeferConstInfo.mSetModeHistory = sMode;

    /**
     * Collision 정보 추가 
     */

    sItem->mItemType = QLL_COLL_ITEM_SAVEPOINT;
    ellInitDictHandle( & sItem->mObjectHandle );
    sItem->mCollisionCount = 0;
    sItem->mSaveTimestamp  = aSavepointTimestamp;

    sItem->mNext = sSessEnv->mDeferConstInfo.mTransCollisionHistory;
    sSessEnv->mDeferConstInfo.mTransCollisionHistory = sItem;
    
    return STL_SUCCESS;

    STL_FINISH;

    if ( sMode != NULL )
    {
        (void) knlFreeDynamicMem( & QLL_COLLISION_MEM(aEnv), sMode, KNL_ENV(aEnv) );
    }

    if ( sItem != NULL )
    {
        (void) knlFreeDynamicMem( & QLL_COLLISION_MEM(aEnv), sItem, KNL_ENV(aEnv) );
    }
    
    return STL_FAILURE;
}

/**
 * @brief ROLLBACK TO SAVEPOINT 에 대해 Deferrable Constraint 정보를 rollback 한다.
 * @param[in]  aSavepointTimestamp  savepoint 의 timestamp
 * @param[in]  aEnv                 Environment
 * @remarks  ROLLBACK TO SAVEPOINT 시 호출
 */
void qlrcRollbackToSavepointDeferConstraint( stlInt64     aSavepointTimestamp,
                                             qllEnv     * aEnv )
{
    qllSessionEnv * sSessEnv = QLL_SESS_ENV(aEnv);

    qllSetConstraintModeItem * sMode = NULL;
    qllSetConstraintModeItem * sModeNext = NULL;

    qllCollisionItem * sItem = NULL;
    qllCollisionItem * sItemNext = NULL;

    qllCollisionItem * sSummaryItem = NULL;
    qllCollisionItem * sFoundItem = NULL;
    
    /*************************************************************
     * SET CONSTRAINT 정보 Rollback 
     *************************************************************/
    
    /**
     * Savepoint 이후에 설정된 SET CONSTRAINT 정보를 제거한다.
     */

    sMode = sSessEnv->mDeferConstInfo.mSetModeHistory;

    while ( sMode != NULL )
    {
        sModeNext = sMode->mNext;

        /**
         * 해당 SAVEPOINT 인지 검사
         */
        
        if ( (sMode->mSetMode       == QLL_SET_CONSTRAINT_MODE_SAVEPOINT) &&
             (sMode->mSaveTimestamp == aSavepointTimestamp) )
        {
            break;
        }

        /**
         * Set Mode History 에서 제거
         */
        
        sSessEnv->mDeferConstInfo.mSetModeHistory = sModeNext;
        
        /**
         * 메모리 해제
         */
        
        if ( knlFreeDynamicMem( & QLL_COLLISION_MEM(aEnv), sMode, KNL_ENV(aEnv) ) == STL_SUCCESS )
        {
            /* ok */
        }
        else
        {
            STL_DASSERT(0);
        }
        
        /**
         * 다음 Mode
         */
        
        sMode = sModeNext;
    }


    /*************************************************************
     * Deferred Constraint 의 Collision 정보 Rollback 
     *************************************************************/

    /**
     * Savepoint 이후에 설정된 Collision History 정보를 제거하면서, Summary 정보에서 차감한다.
     */

    sItem = sSessEnv->mDeferConstInfo.mTransCollisionHistory;

    while ( sItem != NULL )
    {
        sItemNext = sItem->mNext;

        /**
         * 해당 SAVEPOINT 인지 검사
         */
        
        if ( (sItem->mItemType      == QLL_COLL_ITEM_SAVEPOINT) &&
             (sItem->mSaveTimestamp == aSavepointTimestamp) )
        {
            break;
        }

        /**
         * Transaction Collision History 에서 제거
         */
        
        sSessEnv->mDeferConstInfo.mTransCollisionHistory = sItemNext;
        
        /**
         * Collision Summary 에서 차감
         */

        if ( sItem->mItemType == QLL_COLL_ITEM_SAVEPOINT )
        {
            /* nothing to do */
        }
        else
        {
            sFoundItem = NULL;
            
            for ( sSummaryItem = sSessEnv->mDeferConstInfo.mTransCollisionSummary;
                  sSummaryItem != NULL;
                  sSummaryItem = sSummaryItem->mNext )
            {
                if ( qlrcIsSameCollisionItem( sItem, sSummaryItem ) == STL_TRUE )
                {
                    sFoundItem = sSummaryItem;
                    break;
                }
            }

            STL_DASSERT( sFoundItem != NULL );

            sFoundItem->mCollisionCount -= sItem->mCollisionCount;
        }
        
        /**
         * 메모리 해제
         */

        if ( knlFreeDynamicMem( & QLL_COLLISION_MEM(aEnv), sItem, KNL_ENV(aEnv) ) == STL_SUCCESS )
        {
            /* ok */
        }
        else
        {
            STL_DASSERT(0);
        }
        
        /**
         * 다음 Item
         */

        sItem = sItemNext;
    }
}


/*********************************************************
 * Constraint Mode
 *********************************************************/


/**
 * @brief SET CONSTRAINT 정보를 추가한다.
 * @param[in] aSetMode           SET CONSTRAINT mode
 * @param[in] aConstraintHandle  Constraint Handle
 * @param[in] aEnv               Environment
 * @remarks
 * 
 */
stlStatus qlrcAddSetConstraintMode( qllSetConstraintMode   aSetMode,
                                    ellDictHandle        * aConstraintHandle,
                                    qllEnv               * aEnv )
{
    qllSessionEnv * sSessEnv = QLL_SESS_ENV(aEnv);

    qllSetConstraintModeItem * sMode = NULL;

    /**
     * Parameter Validation
     */
    
    if ( aConstraintHandle == NULL )
    {
        STL_PARAM_VALIDATE( (aSetMode == QLL_SET_CONSTRAINT_MODE_ALL_IMMEDIATE) ||
                            (aSetMode == QLL_SET_CONSTRAINT_MODE_ALL_DEFERRED),
                            QLL_ERROR_STACK(aEnv) );
    }
    else
    {
        STL_PARAM_VALIDATE( (aSetMode == QLL_SET_CONSTRAINT_MODE_CONSTRAINT_IMMEDIATE) ||
                            (aSetMode == QLL_SET_CONSTRAINT_MODE_CONSTRAINT_DEFERRED),
                            QLL_ERROR_STACK(aEnv) );
        
    }

    /**
     * 공간 할당
     */

    STL_TRY( knlAllocDynamicMem( & QLL_COLLISION_MEM(aEnv),
                                 STL_SIZEOF(qllSetConstraintModeItem),
                                 (void **) & sMode,
                                 KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sMode, 0x00, STL_SIZEOF(qllSetConstraintModeItem) );
    
    /**
     * Constraint Mode Item 정보 설정
     */

    sMode->mSetMode = aSetMode;
    
    switch ( aSetMode )
    {
        case QLL_SET_CONSTRAINT_MODE_ALL_IMMEDIATE:
            sMode->mConstID = ELL_DICT_OBJECT_ID_NA;
            break;
        case QLL_SET_CONSTRAINT_MODE_ALL_DEFERRED:
            sMode->mConstID = ELL_DICT_OBJECT_ID_NA;
            break;
        case QLL_SET_CONSTRAINT_MODE_CONSTRAINT_IMMEDIATE:
            sMode->mConstID = ellGetConstraintID( aConstraintHandle );
            break;
        case QLL_SET_CONSTRAINT_MODE_CONSTRAINT_DEFERRED:
            sMode->mConstID = ellGetConstraintID( aConstraintHandle );
            break;
        default:
            STL_DASSERT(0);
            break;
    }
    
    sMode->mSaveTimestamp = 0;

    /**
     * Set Mode History 에 추가
     * 최신을 앞에 추가된다.
     */

    sMode->mNext = sSessEnv->mDeferConstInfo.mSetModeHistory;
    sSessEnv->mDeferConstInfo.mSetModeHistory = sMode;
    
    return STL_SUCCESS;

    STL_FINISH;

    if ( sMode != NULL )
    {
        (void) knlFreeDynamicMem( & QLL_COLLISION_MEM(aEnv), sMode, KNL_ENV(aEnv) );
    }
    
    return STL_FAILURE;
}

/**
 * @brief Constraint 의 Check Time 이 deferred 인지 immediate 인지의 정보
 * @param[in] aConstraintHandle  Constraint Handle
 * @param[in] aEnv               Environment
 * @remarks
 * Execution 시점에 Constraint 의 check time 이 결정된다.
 */
stlBool  qlrcIsCheckTimeDeferred( ellDictHandle * aConstraintHandle,
                                  qllEnv        * aEnv )
{
    stlBool   sIsDeferred = STL_FALSE;
    
    qllSessionEnv * sSessEnv = QLL_SESS_ENV(aEnv);

    qllSetConstraintModeItem * sMode = NULL;
    qllSetConstraintModeItem * sFoundMode = NULL;
    
    
    /**
     * Set Mode History 를 순회하며, Constraint Handle 이 SET CONSTRAINT 의 영향을 받는지 검사한다.
     */

    sFoundMode = NULL;
    
    if ( ellGetConstraintAttrIsDeferrable( aConstraintHandle ) == STL_TRUE )
    {
        /**
         * DEFERRABLE constraint 인 경우
         */

        for ( sMode = sSessEnv->mDeferConstInfo.mSetModeHistory;
              sMode != NULL;
              sMode = sMode->mNext )
        {
            switch ( sMode->mSetMode )
            {
                case QLL_SET_CONSTRAINT_MODE_ALL_IMMEDIATE:
                case QLL_SET_CONSTRAINT_MODE_ALL_DEFERRED:
                    {
                        /**
                         * ALL 인 경우
                         */
                        
                        sFoundMode = sMode;
                        break;
                    }
                case QLL_SET_CONSTRAINT_MODE_CONSTRAINT_IMMEDIATE:
                case QLL_SET_CONSTRAINT_MODE_CONSTRAINT_DEFERRED:
                    {
                        /**
                         * 지정 Contraint 인 경우
                         */
                        
                        if ( ellGetConstraintID( aConstraintHandle ) == sMode->mConstID )
                        {
                            /**
                             * 같은 Constraint
                             */
                            sFoundMode = sMode;
                        }
                        else
                        {
                            /**
                             * 다른 Constraint
                             */
                            sFoundMode = NULL;
                        }
                        
                        break;
                    }

                case QLL_SET_CONSTRAINT_MODE_SAVEPOINT:
                    {
                        sFoundMode = NULL;
                        break;
                    }
                default:
                    {
                        STL_DASSERT(0);
                        break;
                    }
            }

            if ( sFoundMode != NULL )
            {
                break;
            }
        }
    }
    else
    {
        /**
         * NOT DEFERRABLE constraint 인 경우
         */

        sFoundMode = NULL;
    }

    /**
     * Deferred 여부를 설정한다
     */
    
    if ( sFoundMode == NULL )
    {
        /**
         * SET CONSTRAINT 로 설정되어 있지 않은 경우
         */
        
        sIsDeferred = ellGetConstraintAttrIsInitDeferred( aConstraintHandle );
    }
    else
    {
        /**
         * SET CONSTRAINT 로 설정되어 있는 경우
         */
        
        switch ( sFoundMode->mSetMode )
        {
            case QLL_SET_CONSTRAINT_MODE_ALL_IMMEDIATE:
                sIsDeferred = STL_FALSE;
                break;
            case QLL_SET_CONSTRAINT_MODE_ALL_DEFERRED:
                sIsDeferred = STL_TRUE;
                break;
            case QLL_SET_CONSTRAINT_MODE_CONSTRAINT_IMMEDIATE:
                sIsDeferred = STL_FALSE;
                break;
            case QLL_SET_CONSTRAINT_MODE_CONSTRAINT_DEFERRED:
                sIsDeferred = STL_TRUE;
                break;
            default:
                STL_DASSERT(0);
                break;
        }
    }

    return sIsDeferred;
}


/*********************************************************
 * Constraint Collision
 *********************************************************/

/**
 * @brief 동일한 Collision Item 인지 검사
 * @param[in] aItem1   Collision Item
 * @param[in] aItem2   Collision Item
 * @return stlBool
 */
stlBool  qlrcIsSameCollisionItem( qllCollisionItem * aItem1,
                                  qllCollisionItem * aItem2 )
{
    stlBool sResult = STL_FALSE;
    
    if ( aItem1->mItemType == aItem2->mItemType )
    {
        switch ( aItem1->mItemType )
        {
            case QLL_COLL_ITEM_CONSTRAINT:
                {
                    if ( ellGetConstraintID( & aItem1->mObjectHandle )
                         == ellGetConstraintID( & aItem2->mObjectHandle ) )
                    {
                        sResult = STL_TRUE;
                    }
                    else
                    {
                        sResult = STL_FALSE;
                    }
                    
                    break;
                }
            case QLL_COLL_ITEM_UNIQUE_INDEX:
                {
                    /**
                     * CDC_DEFERRED_UNIQUE 가 설정된 경우임
                     */
                    
                    if ( ellGetIndexID( & aItem1->mObjectHandle )
                         == ellGetIndexID( & aItem2->mObjectHandle ) )
                    {
                        sResult = STL_TRUE;
                    }
                    else
                    {
                        sResult = STL_FALSE;
                    }
                    
                    break;
                }
            default:
                {
                    sResult = STL_FALSE;
                    STL_DASSERT(0);
                    break;
                }
        }
    }
    else
    {
        sResult = STL_FALSE;
    }

    return sResult;
}
    

/**
 * @brief Deferred Constarint 의 Collision 정보를 설정한다.
 * @param[in]  aCollItemType      Collision Item Type
 * @param[in]  aObjectHandle      Object Handle
 * @param[in]  aCollisionCount    Collision Count
 * @param[in]  aEnv               Environment
 * @remarks Collision History 와 Collision Summary 에 정보를 추가한다.
 */
stlStatus qlrcSetDeferredCollision( qllCollItemType   aCollItemType,
                                    ellDictHandle   * aObjectHandle,
                                    stlInt64          aCollisionCount,
                                    qllEnv          * aEnv )
{
    qllSessionEnv * sSessEnv = QLL_SESS_ENV(aEnv);

    qllCollisionItem * sItem = NULL;
    qllCollisionItem * sFoundItem = NULL;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aObjectHandle != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCollisionCount != 0, QLL_ERROR_STACK(aEnv) );

    /********************************************************************
     * Statement Collision List 에 추가
     ********************************************************************/

    /**
     * Collision Item 정보 설정
     */

    STL_TRY( knlAllocDynamicMem( & QLL_COLLISION_MEM(aEnv),
                                 STL_SIZEOF(qllCollisionItem),
                                 (void **) & sItem,
                                 KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sItem, 0x00, STL_SIZEOF(qllCollisionItem) );

    sItem->mItemType = aCollItemType;
    stlMemcpy( & sItem->mObjectHandle, aObjectHandle, STL_SIZEOF(ellDictHandle) );
    sItem->mCollisionCount = aCollisionCount;
    sItem->mSaveTimestamp  = 0;

    /**
     * Collision History 에 추가
     * 최신을 앞에 추가된다.
     */

    sItem->mNext = sSessEnv->mDeferConstInfo.mStmtCollisionList;
    sSessEnv->mDeferConstInfo.mStmtCollisionList = sItem;

    /********************************************************************
     * Statement Collision Summary 노드를 생성
     ********************************************************************/

    /**
     * 기존 Transaction Collision Summary 에 존재하는 지 검사 
     */

    for ( sFoundItem = sSessEnv->mDeferConstInfo.mTransCollisionSummary;
          sFoundItem != NULL;
          sFoundItem = sFoundItem->mNext )
    {
        if ( qlrcIsSameCollisionItem( sItem, sFoundItem ) == STL_TRUE )
        {
            break;
        }
    }

    /**
     * Collision New Summary 공간 할당
     */

    if ( sFoundItem == NULL )
    {
        /**
         * 처음 위반한 Constraint 를 추가 
         */
        
        STL_TRY( knlAllocDynamicMem( & QLL_COLLISION_MEM(aEnv),
                                     STL_SIZEOF(qllCollisionItem),
                                     (void **) & sItem,
                                     KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        stlMemset( sItem, 0x00, STL_SIZEOF(qllCollisionItem) );

        sItem->mItemType = aCollItemType;
        stlMemcpy( & sItem->mObjectHandle, aObjectHandle, STL_SIZEOF(ellDictHandle) );
        /**
         * summary 는 statement 종료시 누적
         */
        sItem->mCollisionCount = 0;
        sItem->mSaveTimestamp  = 0;

        /* 새로 추가되는 collision 은 NOVALIDATE 이라 하더라도 양수여야 함 */
        STL_DASSERT( aCollisionCount > 0 );
        
        /**
         * Collision New Summary 에 추가
         */

        sItem->mNext = sSessEnv->mDeferConstInfo.mNewCollisionSummary;
        sSessEnv->mDeferConstInfo.mNewCollisionSummary = sItem;
    }
    else
    {
        /**
         * 이미 존재하는 Collision Summary 
         */
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Deferred Constraint 를 위배했는지 여부
 * @param[in]  aEnv       Environment
 */
stlBool qlrcHasViolatedDeferredConstraint( qllEnv * aEnv )
{
    qllSessionEnv * sSessEnv = QLL_SESS_ENV(aEnv);

    stlBool sIsViolated = STL_FALSE;

    qllCollisionItem * sItem = NULL;
    
    /**
     * Transaction Collision Summary 를 순회하며 Violated Constraint 가 존재하는 지 확인
     */

    sIsViolated = STL_FALSE;

    for ( sItem = sSessEnv->mDeferConstInfo.mTransCollisionSummary;
          sItem != NULL;
          sItem = sItem->mNext )
    {
        if ( sItem->mCollisionCount > 0 )
        {
            /**
             * Violation Constraint 가 존재함
             */

            sIsViolated = STL_TRUE;
            break;
        }
        else
        {
            STL_DASSERT( sItem->mCollisionCount == 0 );
            /**
             * Violation 이 해소된 constraint 임
             */

            continue;
        }
    }
    
    return sIsViolated;
}

/**
 * @brief XA context 에 deferrable constraint 정보를 위배하고 있는지 검사
 * @param[in] aXaContext  XA Context
 * @remarks
 */
stlBool qlrcHasXaViolatedConstraint( knlXaContext * aXaContext )
{
    qllDeferrableConstraintInfo * sXaDeferInfo = NULL;
    qllCollisionItem * sItem = NULL;

    stlBool sIsViolated = STL_FALSE;
    
    /**
     * Parameter Validation
     */

    STL_DASSERT( aXaContext != NULL );
    sXaDeferInfo = (qllDeferrableConstraintInfo *) aXaContext->mDeferContext;
    
    /**
     * Transaction Collision Summary 를 순회하며 Violated Constraint 가 존재하는 지 확인
     */

    sIsViolated = STL_FALSE;

    if ( sXaDeferInfo == NULL )
    {
        sIsViolated = STL_FALSE;
    }
    else
    {
        for ( sItem = sXaDeferInfo->mTransCollisionSummary;
              sItem != NULL;
              sItem = sItem->mNext )
        {
            if ( sItem->mCollisionCount > 0 )
            {
                /**
                 * Violation Constraint 가 존재함
                 */
                
                sIsViolated = STL_TRUE;
                break;
            }
            else
            {
                STL_DASSERT( sItem->mCollisionCount == 0 );
                /**
                 * Violation 이 해소된 constraint 임
                 */
                
                continue;
            }
        }
    }
    
    return sIsViolated;
}

/**
 * @brief 해당 Constraint 가 이미 Violated Constraint 인지를 검사
 * @param[in]  aConstHandle
 * @param[in]  aEnv       Environment
 */
stlBool qlrcIsViolatedConstraint( ellDictHandle * aConstHandle, qllEnv * aEnv )
{
    qllSessionEnv * sSessEnv = QLL_SESS_ENV(aEnv);

    stlBool sIsViolated = STL_FALSE;

    qllCollisionItem * sItem = NULL;
    
    /**
     * Transaction Collision Summary 를 순회하며 Violated Constraint 가 존재하는 지 확인
     */

    sIsViolated = STL_FALSE;

    for ( sItem = sSessEnv->mDeferConstInfo.mTransCollisionSummary;
          sItem != NULL;
          sItem = sItem->mNext )
    {
        if ( sItem->mItemType == QLL_COLL_ITEM_CONSTRAINT )
        {
            if ( sItem->mCollisionCount > 0 )
            {
                if ( ellGetConstraintID( & sItem->mObjectHandle ) == ellGetConstraintID( aConstHandle ) )
                {
                    sIsViolated = STL_TRUE;
                    break;
                }
            }
            else
            {
                continue;
            }
        }
        else
        {
            continue;
        }
    }
    
    return sIsViolated;
}

/**
 * @brief Deferred Constraint 를 위배 정보를 메시지로 구성함.
 * @param[in]  aTransID       Transaction ID
 * @param[out] aBuffer        Message Buffer
 * @param[in]  aBufferLength  Buffer Length
 * @param[in]  aConstHandle   Const Handle ( nullable: ALL )
 * @param[in]  aEnv           Environment
 */
stlStatus qlrcSetViolatedHandleMessage( smlTransId      aTransID,
                                        stlChar       * aBuffer,
                                        stlInt32        aBufferLength,
                                        ellDictHandle * aConstHandle,
                                        qllEnv        * aEnv )
{
    qllSessionEnv * sSessEnv = QLL_SESS_ENV(aEnv);
    qllCollisionItem * sItem = NULL;
    
    ellDictHandle  sSchemaHandle;
    stlBool        sObjectExist;
    stlBool        sIsFirstError = STL_TRUE;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aBuffer != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aBufferLength > 0, QLL_ERROR_STACK(aEnv) );

    /**
     * Collision Summary 를 순회하며 Violated Constraint 정보를 메시지로 구성
     */

    aBuffer[0] = '\0';

    for ( sItem = sSessEnv->mDeferConstInfo.mTransCollisionSummary;
          sItem != NULL;
          sItem = sItem->mNext )
    {
        if ( sItem->mCollisionCount > 0 )
        {
            if ( aConstHandle == NULL )
            {
                /* 모든 위배 정보를 출력 */
            }
            else
            {
                /**
                 * 해당 Constraint 인 경우만 출력
                 */
                if ( ( sItem->mItemType == QLL_COLL_ITEM_CONSTRAINT ) &&
                     ( ellGetConstraintID( & sItem->mObjectHandle ) == ellGetConstraintID( aConstHandle ) ) )
                {
                    /* ok */
                }
                else
                {
                    continue;
                }
            }
            
            /**
             * Violation Constraint 가 존재함
             */

            switch ( sItem->mItemType )
            {
                case QLL_COLL_ITEM_CONSTRAINT:
                    {
                        STL_TRY( ellGetSchemaDictHandleByID( aTransID,
                                                             smlGetSystemScn(), 
                                                             ellGetConstraintSchemaID(& sItem->mObjectHandle),
                                                             & sSchemaHandle,
                                                             & sObjectExist,
                                                             ELL_ENV(aEnv) )
                                 == STL_SUCCESS );
                        /* DML 수행후이기 때문에 관련 객체가 DROP 되지 않는다 */
                        STL_DASSERT( sObjectExist == STL_TRUE );

                        if ( sIsFirstError == STL_TRUE )
                        {
                            stlSnprintf( aBuffer,
                                         aBufferLength,
                                         "%s%s.%s(%d)",
                                         aBuffer,
                                         ellGetSchemaName( & sSchemaHandle ),
                                         ellGetConstraintName( & sItem->mObjectHandle ),
                                         sItem->mCollisionCount );

                            sIsFirstError = STL_FALSE;
                        }
                        else
                        {
                            stlSnprintf( aBuffer,
                                         aBufferLength,
                                         "%s, %s.%s(%d)",
                                         aBuffer,
                                         ellGetSchemaName( & sSchemaHandle ),
                                         ellGetConstraintName( & sItem->mObjectHandle ),
                                         sItem->mCollisionCount );
                        }
                        
                        break;
                    }
                case QLL_COLL_ITEM_UNIQUE_INDEX:
                    {
                        STL_TRY( ellGetSchemaDictHandleByID( aTransID,
                                                             smlGetSystemScn(), 
                                                             ellGetIndexSchemaID(& sItem->mObjectHandle),
                                                             & sSchemaHandle,
                                                             & sObjectExist,
                                                             ELL_ENV(aEnv) )
                                 == STL_SUCCESS );
                        /* DML 수행후이기 때문에 관련 객체가 DROP 되지 않는다 */
                        STL_DASSERT( sObjectExist == STL_TRUE );
                        
                        if ( sIsFirstError == STL_TRUE )
                        {
                            stlSnprintf( aBuffer,
                                         aBufferLength,
                                         "%s%s.%s(%d)",
                                         aBuffer,
                                         ellGetSchemaName( & sSchemaHandle ),
                                         ellGetIndexName( & sItem->mObjectHandle ),
                                         sItem->mCollisionCount );
                            sIsFirstError = STL_FALSE;
                        }
                        else
                        {
                            stlSnprintf( aBuffer,
                                         aBufferLength,
                                         "%s, %s.%s(%d)",
                                         aBuffer,
                                         ellGetSchemaName( & sSchemaHandle ),
                                         ellGetIndexName( & sItem->mObjectHandle ),
                                         sItem->mCollisionCount );
                        }
                        
                        break;
                    }
                default:
                    {
                        STL_DASSERT(0);
                        break;
                    }
            }
        }
        else 
        {
            /**
             * Violation 이 해소된 constraint 임
             */
            
            STL_DASSERT( sItem->mCollisionCount == 0 );
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}





/** @} qlrcDeferrableConstraint */


