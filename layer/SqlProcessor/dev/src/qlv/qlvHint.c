/*******************************************************************************
 * qlvHint.c
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
 * @file qlvHint.c
 * @brief SQL Processor Layer HINT statement Validation
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlv
 * @{
 */


/**
 * @brief Hint Info 생성
 * @param[out]  aHintInfo   생성된 Hint Info
 * @param[in]   aTableCount Table Count
 * @param[in]   aRegionMem  Region Memory
 * @param[in]   aEnv        Environment
 */
stlStatus qlvCreateHintInfo( qlvHintInfo   ** aHintInfo,
                             stlInt32         aTableCount,
                             knlRegionMem   * aRegionMem,
                             qllEnv         * aEnv )
{
    stlInt32              i;
    qlvHintInfo         * sHintInfo             = NULL;
    qlvInitNode        ** sTableNodePtrArr      = NULL;
    qlvAccessPathHint   * sAccPathHintArr       = NULL;
    qlvJoinOrderHint    * sJoinOrderArr         = NULL;


    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aHintInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableCount > 0, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * Hint Info 공간 할당
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlvHintInfo ),
                                (void **) & sHintInfo,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );


    /**
     * Table Node Ptr 공간 할당
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlvInitNode* ) * aTableCount,
                                (void **) & sTableNodePtrArr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );


    /**
     * Access Path Hint 공간 할당
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlvAccessPathHint ) * aTableCount,
                                (void **) & sAccPathHintArr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );


    /**
     * Join Order Hint 공간 할당
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlvJoinOrderHint ) * aTableCount,
                                (void **) & sJoinOrderArr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );


    /**
     * Hint Info 정보 초기화
     */

    for( i = 0; i < aTableCount; i++ )
    {
        /* Table Node Ptr Array */
        sTableNodePtrArr[i] = NULL;

        /* Access Path Array */
        sAccPathHintArr[i].mScanMethod = QLV_HINT_SCAN_METHOD_DEFAULT;
        sAccPathHintArr[i].mUseIndexScanList.mCount = 0;
        sAccPathHintArr[i].mUseIndexScanList.mHead = NULL;
        sAccPathHintArr[i].mUseIndexScanList.mTail = NULL;
        sAccPathHintArr[i].mNoUseIndexScanList.mCount = 0;
        sAccPathHintArr[i].mNoUseIndexScanList.mHead = NULL;
        sAccPathHintArr[i].mNoUseIndexScanList.mTail = NULL;
    }


    /**
     * Hint Info 정보 연결
     */

    sHintInfo->mTransID         = SML_INVALID_TRANSID;
    sHintInfo->mTableCount      = aTableCount;
    sHintInfo->mHasHintError    = STL_FALSE;
    sHintInfo->mTableNodePtrArr = sTableNodePtrArr;
    sHintInfo->mAccPathHintArr  = sAccPathHintArr;
    sHintInfo->mJoinOrderCount  = 0;
    sHintInfo->mJoinOrderArr    = sJoinOrderArr;
    sHintInfo->mJoinOperList.mCount = 0;
    sHintInfo->mJoinOperList.mHead  = NULL;
    sHintInfo->mJoinOperList.mTail  = NULL;
    sHintInfo->mQueryTransformHint.mQueryTransformType = QLV_HINT_QUERY_TRANSFORM_TYPE_DEFAULT;
    sHintInfo->mQueryTransformHint.mSubQueryUnnestType = QLV_HINT_SUBQUERY_UNNEST_TYPE_DEFAULT;
    sHintInfo->mQueryTransformHint.mUnnestOperType = QLV_HINT_UNNEST_OPER_TYPE_DEFAULT;
    sHintInfo->mOtherHint.mPushSubq = QLV_HINT_DEFAULT;
    sHintInfo->mOtherHint.mUseDistinctHash = QLV_HINT_DEFAULT;
    sHintInfo->mOtherHint.mUseGroupHash = QLV_HINT_DEFAULT;


    /**
     * OUTPUT 설정
     */
    
    *aHintInfo = sHintInfo;

    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief HINT 구문을 Validation 한다.
 * @param[in]   aStmtInfo           Stmt Information
 * @param[in]   aCalledFromSubQuery Subquery로부터 이 함수가 호출되었는지 여부
 * @param[in]   aHasGroupBy         Group By를 갖는지 여부
 * @param[in]   aHasDistinct        Distinct를 갖는지 여부
 * @param[in]   aTableNode          Table Node
 * @param[in]   aTableCount         Table Count
 * @param[in]   aHintSource         Hint Source
 * @param[in]   aHintInfo           Hint Info
 * @param[in]   aEnv                Environment
 */
stlStatus qlvValidateHintInfo( qlvStmtInfo  * aStmtInfo,
                               stlBool        aCalledFromSubQuery,
                               stlBool        aHasGroupBy,
                               stlBool        aHasDistinct,
                               qlvInitNode  * aTableNode,
                               stlInt32       aTableCount,
                               qlpList      * aHintSource,
                               qlvHintInfo  * aHintInfo,
                               qllEnv       * aEnv )
{
    qlpListElement          * sHintListElem     = NULL;
    qlpHint                 * sHint             = NULL;

    stlInt32                  sIdx              = 0;
    qlvInitNode            ** sTableNodePtrArr  = NULL;

    stlBool                   sEnabledHintError = STL_FALSE;
    stlBool                   sExistJoinOrder   = STL_FALSE;


    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableCount > 0, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aHintInfo != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * TransID 설정
     */

    aHintInfo->mTransID = aStmtInfo->mTransID;


    /**
     * Table Node List 분석
     */

    {
        qlvInitNode     * sTmpTableNode;
        qlvInitNode    ** sNodeStack;
        stlInt32          sNodePos = 0;

        /* Node Stack 생성 */
        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                    STL_SIZEOF( qlvInitNode* ) * aHintInfo->mTableCount,
                                    (void **) &sNodeStack,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        sIdx = 0;
        sTableNodePtrArr = aHintInfo->mTableNodePtrArr;
        sTmpTableNode = aTableNode;

        while( sTmpTableNode != NULL )
        {
            switch( sTmpTableNode->mType )
            {
                case QLV_NODE_TYPE_BASE_TABLE:
                    sTableNodePtrArr[sIdx] = sTmpTableNode;
                    sIdx++;
                    if( sNodePos > 0 )
                    {
                        sNodePos--;
                        sTmpTableNode = ((qlvInitJoinTableNode*)sNodeStack[sNodePos])->mRightTableNode;
                    }
                    else
                    {
                        sTmpTableNode = NULL;
                    }
                    break;

                case QLV_NODE_TYPE_SUB_TABLE:
                    sTableNodePtrArr[sIdx] = sTmpTableNode;
                    sIdx++;
                    if( sNodePos > 0 )
                    {
                        sNodePos--;
                        sTmpTableNode = ((qlvInitJoinTableNode*)sNodeStack[sNodePos])->mRightTableNode;
                    }
                    else
                    {
                        sTmpTableNode = NULL;
                    }
                    break;

                case QLV_NODE_TYPE_JOIN_TABLE:
                    sNodeStack[sNodePos] = sTmpTableNode;
                    sNodePos++;
                    sTmpTableNode = ((qlvInitJoinTableNode*)sTmpTableNode)->mLeftTableNode;
                    break;
                default:
                    STL_DASSERT( 0 );
                    break;
            }
        }

        STL_DASSERT( sNodePos == 0 );
        STL_DASSERT( sIdx == aTableCount );
    }


    /**
     * Hint 분석
     */

    if( aHintSource != NULL )
    {
        /* Hint Error 처리 여부 설정 얻기 */
        STL_TRY( knlGetPropertyValueByID( KNL_PROPERTY_HINT_ERROR,
                                          &sEnabledHintError,
                                          KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        /* Hint 분석 */
        QLP_LIST_FOR_EACH( aHintSource, sHintListElem )
        {
            sHint = (qlpHint*)QLP_LIST_GET_POINTER_VALUE( sHintListElem );
            STL_DASSERT( sHint->mType == QLL_HINT_TYPE );

            switch( sHint->mHintType )
            {
                case QLP_HINT_JOIN_ORDER:
                    {
                        if( sExistJoinOrder == STL_TRUE )
                        {
                            /* 이미 Join Order Hint가 있었음 */
                            aHintInfo->mHasHintError = STL_TRUE;
                            
                            STL_TRY_THROW( sEnabledHintError == STL_FALSE,
                                           RAMP_ERR_NOT_APPLICABLE_HINT );
                        }
                        else
                        {
                            STL_TRY( qlvValidateHintJoinOrder( aStmtInfo,
                                                               sTableNodePtrArr,
                                                               aTableCount,
                                                               sEnabledHintError,
                                                               (qlpHintJoinOrder*)sHint,
                                                               aHintInfo->mJoinOrderArr,
                                                               &aHintInfo->mJoinOrderCount,
                                                               aEnv )
                                     == STL_SUCCESS );

                            sExistJoinOrder = STL_TRUE;
                        }

                        break;
                    }
                case QLP_HINT_JOIN_OPER:
                    {
                        STL_TRY( qlvValidateHintJoinOper( aStmtInfo,
                                                          sTableNodePtrArr,
                                                          aTableCount,
                                                          sEnabledHintError,
                                                          (qlpHintJoinOper*)sHint,
                                                          &(aHintInfo->mJoinOperList),
                                                          aEnv )
                                 == STL_SUCCESS );

                        break;
                    }
                case QLP_HINT_INDEX:
                    {
                        STL_TRY( qlvValidateHintIndex( aStmtInfo,
                                                       sTableNodePtrArr,
                                                       aTableCount,
                                                       sEnabledHintError,
                                                       (qlpHintIndex*)sHint,
                                                       aHintInfo->mAccPathHintArr,
                                                       aEnv )
                                 == STL_SUCCESS );

                        break;
                    }
                case QLP_HINT_TABLE:
                    {
                        STL_TRY( qlvValidateHintTable( aStmtInfo,
                                                       sTableNodePtrArr,
                                                       aTableCount,
                                                       sEnabledHintError,
                                                       (qlpHintTable*)sHint,
                                                       aHintInfo->mAccPathHintArr,
                                                       aEnv )
                                 == STL_SUCCESS );

                        break;
                    }
                case QLP_HINT_QUERY_TRANSFORM:
                    {
                        STL_TRY( qlvValidateHintQueryTransform( aStmtInfo,
                                                                sEnabledHintError,
                                                                aCalledFromSubQuery,
                                                                (qlpHintQueryTransform*)sHint,
                                                                &(aHintInfo->mQueryTransformHint),
                                                                aEnv )
                                 == STL_SUCCESS );

                        break;
                    }
                case QLP_HINT_OTHER:
                    {
                        STL_TRY( qlvValidateHintOther( aStmtInfo,
                                                       sEnabledHintError,
                                                       aCalledFromSubQuery,
                                                       aHasGroupBy,
                                                       aHasDistinct,
                                                       (qlpHintOther*)sHint,
                                                       &(aHintInfo->mOtherHint),
                                                       aEnv )
                                 == STL_SUCCESS );
                        break;
                    }
                default:
                    {
                        STL_DASSERT( 0 );
                        break;
                    }
            }
        }
    }

    if( aHintInfo->mHasHintError == STL_TRUE )
    {
        aStmtInfo->mHasHintError = STL_TRUE;
    }
    

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_APPLICABLE_HINT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_APPLICABLE_HINT,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           sHint->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Index 힌트를 Validation 한다.
 * @param[in]       aStmtInfo           Stmt Information
 * @param[in]       aTableNodePtrArr    Table Node Pointer Array
 * @param[in]       aTableCount         Table Count
 * @param[in]       aEnabledHintError   Hint Error 처리 여부
 * @param[in]       aHintIndex          Index Scan Hint Parse Tree
 * @param[in,out]   aAccPathHintArr     Access Path Hint Array
 * @param[in]       aEnv                Environment
 * @remarks
 */
stlStatus qlvValidateHintIndex( qlvStmtInfo         * aStmtInfo,
                                qlvInitNode        ** aTableNodePtrArr,
                                stlInt32              aTableCount,
                                stlBool               aEnabledHintError,
                                qlpHintIndex        * aHintIndex,
                                qlvAccessPathHint   * aAccPathHintArr,
                                qllEnv              * aEnv )
{
    stlBool                   sFound            = STL_FALSE;
    stlInt32                  sIdx              = 0;
    stlInt32                  i;

    qlvInitBaseTableNode    * sBaseTableNode    = NULL;
    qlvInitSubTableNode     * sSubTableNode     = NULL;

    qlpListElement          * sListElem         = NULL;
    qlpIndexScan            * sIndexScan        = NULL;

    ellDictHandle             sSchemaHandle;
    ellDictHandle             sIndexHandle;

    stlInt32                  sIndexCount;
    ellDictHandle           * sIndexHandleList  = NULL;

    qlvIndexScanItem        * sIndexScanItem    = NULL;
    qlvIndexScanList        * sIndexScanList    = NULL;
    qlvHintFlag               sAsc              = QLV_HINT_DEFAULT;


    ellInitDictHandle( &sSchemaHandle );
    ellInitDictHandle( &sIndexHandle );


    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableNodePtrArr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableCount > 0, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aHintIndex != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAccPathHintArr != NULL, QLL_ERROR_STACK(aEnv) );


#define _SET_INDEX_SCAN_ITEM( _aIndexHandle, _aAsc )                            \
    {                                                                           \
        /* Index Scan Item 할당*/                                               \
        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),       \
                                    STL_SIZEOF( qlvIndexScanItem ),             \
                                    (void **) & sIndexScanItem,                 \
                                    KNL_ENV(aEnv) )                             \
                 == STL_SUCCESS );                                              \
                                                                                \
        /* Index Scan Item 설정 */                                              \
        stlMemcpy( &(sIndexScanItem->mIndexHandle),                             \
                   (_aIndexHandle),                                             \
                   STL_SIZEOF( ellDictHandle ) );                               \
                                                                                \
        sIndexScanItem->mAsc = (_aAsc);                                         \
        sIndexScanItem->mNext = NULL;                                           \
                                                                                \
        /* Index Scan List에 추가 */                                            \
        if( sIndexScanList->mCount == 0 )                                       \
        {                                                                       \
            sIndexScanList->mHead = sIndexScanItem;                             \
            sIndexScanList->mTail = sIndexScanItem;                             \
        }                                                                       \
        else                                                                    \
        {                                                                       \
            sIndexScanList->mTail->mNext = sIndexScanItem;                      \
            sIndexScanList->mTail = sIndexScanItem;                             \
        }                                                                       \
        sIndexScanList->mCount++;                                               \
    }


    /**********************************************************
     * Hint 의 Table 정보 획득 
     **********************************************************/

    /**
     * Object Name에 일치하는 Table 찾기
     */

    for( sIdx = 0; sIdx < aTableCount; sIdx++ )
    {
        if( aTableNodePtrArr[sIdx]->mType == QLV_NODE_TYPE_BASE_TABLE )
        {
            sBaseTableNode = (qlvInitBaseTableNode*)aTableNodePtrArr[sIdx];
            sFound = qlvIsSameTable( sBaseTableNode->mRelationName,
                                     NULL, /* aHintIndex->mObjectName->mCatalog */
                                     aHintIndex->mObjectName->mSchema,
                                     aHintIndex->mObjectName->mObject );
        }
        else
        {
            STL_DASSERT( aTableNodePtrArr[sIdx]->mType == QLV_NODE_TYPE_SUB_TABLE );
            sSubTableNode = (qlvInitSubTableNode*)aTableNodePtrArr[sIdx];
            sFound = qlvIsSameTable( sSubTableNode->mRelationName,
                                     NULL, /* aHintIndex->mObjectName->mCatalog */
                                     aHintIndex->mObjectName->mSchema,
                                     aHintIndex->mObjectName->mObject );

            /* Sub Table에는 Index Hint가 올 수 없다. */
            if( sFound == STL_TRUE )
            {
                aStmtInfo->mHasHintError = STL_TRUE;

                STL_TRY_THROW( aEnabledHintError == STL_FALSE,
                               RAMP_ERR_NOT_APPLICABLE_HINT );
                STL_THROW( RAMP_FINISH );
            }
        }

        if( sFound == STL_TRUE )
        {
            break;
        }
    }

    if( sFound == STL_TRUE )
    {
        if( aAccPathHintArr[sIdx].mScanMethod != QLV_HINT_SCAN_METHOD_DEFAULT )
        {
            /* Index Hint를 줄 수 없다. */
            aStmtInfo->mHasHintError = STL_TRUE;

            STL_TRY_THROW( aEnabledHintError == STL_FALSE,
                           RAMP_ERR_NOT_APPLICABLE_HINT );

            STL_THROW( RAMP_FINISH );
        }
    }
    else
    {
        /* Index Hint의 table을 찾지 못함 */
        aStmtInfo->mHasHintError = STL_TRUE;
                            
        STL_TRY_THROW( aEnabledHintError == STL_FALSE,
                       RAMP_ERR_NOT_APPLICABLE_HINT );

        STL_THROW( RAMP_FINISH );
    }


    /**********************************************************
     * Hint 의 Index 정보 획득 
     **********************************************************/

    /* Index Hint Type 분석 */
    switch( aHintIndex->mIndexHintType )
    {
        case QLP_INDEX_HINT_INDEX:
            sIndexScanList = &(aAccPathHintArr[sIdx].mUseIndexScanList);
            aAccPathHintArr[sIdx].mScanMethod = QLV_HINT_SCAN_METHOD_USE_INDEX;
            sAsc = QLV_HINT_DEFAULT;
            break;
        case QLP_INDEX_HINT_INDEX_ASC:
            sIndexScanList = &(aAccPathHintArr[sIdx].mUseIndexScanList);
            aAccPathHintArr[sIdx].mScanMethod = QLV_HINT_SCAN_METHOD_USE_INDEX;
            sAsc = QLV_HINT_YES;
            break;
        case QLP_INDEX_HINT_INDEX_DESC:
            sIndexScanList = &(aAccPathHintArr[sIdx].mUseIndexScanList);
            aAccPathHintArr[sIdx].mScanMethod = QLV_HINT_SCAN_METHOD_USE_INDEX;
            sAsc = QLV_HINT_NO;
            break;
        case QLP_INDEX_HINT_NO_INDEX:
            sIndexScanList = &(aAccPathHintArr[sIdx].mNoUseIndexScanList);
            aAccPathHintArr[sIdx].mScanMethod = QLV_HINT_SCAN_METHOD_NO_USE_INDEX;
            sAsc = QLV_HINT_DEFAULT;
            break;
        case QLP_INDEX_HINT_INDEX_COMBINE:
            sIndexScanList = &(aAccPathHintArr[sIdx].mUseIndexScanList);
            aAccPathHintArr[sIdx].mScanMethod = QLV_HINT_SCAN_METHOD_INDEX_COMBINE;
            sAsc = QLV_HINT_DEFAULT;
            break;
        case QLP_INDEX_HINT_IN_KEY_RANGE:
            sIndexScanList = &(aAccPathHintArr[sIdx].mUseIndexScanList);
            aAccPathHintArr[sIdx].mScanMethod = QLV_HINT_SCAN_METHOD_IN_KEY_RANGE;
            sAsc = QLV_HINT_DEFAULT;
            break;
        default:
            STL_DASSERT( 0 );
            break;
    }

    if( aHintIndex->mIndexScanList == NULL )
    {
        /* Primary Key 정보 */
        STL_TRY( ellGetTablePrimaryKeyDictHandle( aStmtInfo->mTransID,
                                                  aStmtInfo->mSQLStmt->mViewSCN,
                                                  &QLL_VALIDATE_MEM( aEnv ),
                                                  &(sBaseTableNode->mTableHandle),
                                                  &sIndexCount,
                                                  &sIndexHandleList,
                                                  ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        for( i = 0; i < sIndexCount; i++ )
        {
            /* Index Scan Item 등록 */
            _SET_INDEX_SCAN_ITEM( ellGetConstraintIndexDictHandle( &sIndexHandleList[i] ),
                                  sAsc );
        }

        /* Unique Key 정보 */
        STL_TRY( ellGetTableUniqueKeyDictHandle( aStmtInfo->mTransID,
                                                 aStmtInfo->mSQLStmt->mViewSCN,
                                                 &QLL_VALIDATE_MEM( aEnv ),
                                                 &(sBaseTableNode->mTableHandle),
                                                 &sIndexCount,
                                                 &sIndexHandleList,
                                                 ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        for( i = 0; i < sIndexCount; i++ )
        {
            /* Index Scan Item 등록 */
            _SET_INDEX_SCAN_ITEM( ellGetConstraintIndexDictHandle( &sIndexHandleList[i] ),
                                  sAsc );
        }

        /* Foreign Key 정보 */
        STL_TRY( ellGetTableForeignKeyDictHandle( aStmtInfo->mTransID,
                                                  aStmtInfo->mSQLStmt->mViewSCN,
                                                  &QLL_VALIDATE_MEM( aEnv ),
                                                  &(sBaseTableNode->mTableHandle),
                                                  &sIndexCount,
                                                  &sIndexHandleList,
                                                  ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        for( i = 0; i < sIndexCount; i++ )
        {
            /**
             * @remark Foreign Key 구현시 진입할 코드임
             */
            
            /* Index Scan Item 등록 */
            _SET_INDEX_SCAN_ITEM( ellGetConstraintIndexDictHandle( &sIndexHandleList[i] ),
                                  sAsc );
        }

        /* Unique Index 정보 */
        STL_TRY( ellGetTableUniqueIndexDictHandle( aStmtInfo->mTransID,
                                                   aStmtInfo->mSQLStmt->mViewSCN,
                                                   &QLL_VALIDATE_MEM( aEnv ),
                                                   &(sBaseTableNode->mTableHandle),
                                                   &sIndexCount,
                                                   &sIndexHandleList,
                                                   ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        for( i = 0; i < sIndexCount; i++ )
        {
            /* Index Scan Item 등록 */
            _SET_INDEX_SCAN_ITEM( &sIndexHandleList[i],
                                  sAsc );
        }

        /* Non Unique Index 정보 */
        STL_TRY( ellGetTableNonUniqueIndexDictHandle( aStmtInfo->mTransID,
                                                      aStmtInfo->mSQLStmt->mViewSCN,
                                                      &QLL_VALIDATE_MEM( aEnv ),
                                                      &(sBaseTableNode->mTableHandle),
                                                      &sIndexCount,
                                                      &sIndexHandleList,
                                                      ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        for( i = 0; i < sIndexCount; i++ )
        {
            /* Index Scan Item 등록 */
            _SET_INDEX_SCAN_ITEM( &sIndexHandleList[i],
                                  sAsc );
        }
    }
    else
    {
        QLP_LIST_FOR_EACH( aHintIndex->mIndexScanList, sListElem )
        {
            sIndexScan = (qlpIndexScan *)QLP_LIST_GET_POINTER_VALUE( sListElem );

            if( sIndexScan->mName->mSchema == NULL )
            {
                /**
                 * Schema Name 이 명시되지 않은 경우
                 */

                sFound = STL_FALSE;
                STL_TRY( ellGetIndexDictHandleWithAuth( aStmtInfo->mTransID,
                                                        aStmtInfo->mSQLStmt->mViewSCN,
                                                        ellGetCurrentUserHandle( ELL_ENV(aEnv) ),
                                                        sIndexScan->mName->mObject,
                                                        & sIndexHandle,
                                                        & sFound,
                                                        ELL_ENV(aEnv) )
                         == STL_SUCCESS );
            }
            else
            {
                /**
                 * Schema Name 이 명시된 경우
                 */

                sFound = STL_FALSE;
                STL_TRY( ellGetSchemaDictHandle( aStmtInfo->mTransID,
                                                 aStmtInfo->mSQLStmt->mViewSCN,
                                                 sIndexScan->mName->mSchema,
                                                 & sSchemaHandle,
                                                 & sFound,
                                                 ELL_ENV(aEnv) )
                         == STL_SUCCESS );

                if( sFound == STL_TRUE )
                {
                    /**
                     * Index 존재 여부 확인
                     */

                    sFound = STL_FALSE;
                    STL_TRY( ellGetIndexDictHandleWithSchema( aStmtInfo->mTransID,
                                                              aStmtInfo->mSQLStmt->mViewSCN,
                                                              & sSchemaHandle,
                                                              sIndexScan->mName->mObject,
                                                              & sIndexHandle,
                                                              & sFound,
                                                              ELL_ENV(aEnv) )
                             == STL_SUCCESS );
                }
                else
                {
                    aStmtInfo->mHasHintError = STL_TRUE;
                            
                    STL_TRY_THROW( aEnabledHintError == STL_FALSE,
                                   RAMP_ERR_NOT_APPLICABLE_HINT );
                }
            }

            if( sFound == STL_TRUE )
            {
                if( ellGetTableID( & sBaseTableNode->mTableHandle )
                    == ellGetIndexTableID( &sIndexHandle ) )
                {
                    /* 기존에 등록된 index인지 확인 */
                    sFound = STL_FALSE;
                    sIndexScanItem = sIndexScanList->mHead;
                    while( sIndexScanItem != NULL )
                    {
                        if( ellGetIndexID( &sIndexScanItem->mIndexHandle )
                            == ellGetIndexID( &sIndexHandle ) )
                        {
                            /* 기존에 등록되어 있음 */
                            sFound = STL_TRUE;
                            break;
                        }

                        sIndexScanItem = sIndexScanItem->mNext;
                    }

                    if( sFound == STL_TRUE )
                    {
                        /* 기존에 등록되어 있음 */
                        aStmtInfo->mHasHintError = STL_TRUE;
                            
                        STL_TRY_THROW( aEnabledHintError == STL_FALSE,
                                       RAMP_ERR_NOT_APPLICABLE_HINT );

                        continue;
                    }

                    /* Index Scan Item 등록 */
                    _SET_INDEX_SCAN_ITEM( &sIndexHandle, sAsc );
                }
                else
                {
                    aStmtInfo->mHasHintError = STL_TRUE;
                            
                    STL_TRY_THROW( aEnabledHintError == STL_FALSE,
                                   RAMP_ERR_NOT_APPLICABLE_HINT );
                }
            }
            else
            {
                aStmtInfo->mHasHintError = STL_TRUE;
                            
                STL_TRY_THROW( aEnabledHintError == STL_FALSE,
                               RAMP_ERR_NOT_APPLICABLE_HINT );
            }
        }
    }

    /* Index 정보를 하나도 못찾은 경우 */
    if( sIndexScanList->mCount == 0 )
    {
        aStmtInfo->mHasHintError = STL_TRUE;
                            
        STL_TRY_THROW( aEnabledHintError == STL_FALSE,
                       RAMP_ERR_NOT_APPLICABLE_HINT );

        /* scan method를 default로 복구 */
        aAccPathHintArr[sIdx].mScanMethod = QLV_HINT_SCAN_METHOD_DEFAULT;
    }


    STL_RAMP( RAMP_FINISH );

    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_APPLICABLE_HINT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_APPLICABLE_HINT,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aHintIndex->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief DML 구문을 위한 Table Scan 힌트를 Validation 한다.
 * @param[in]       aStmtInfo           Stmt Information
 * @param[in]       aTableNodePtrArr    Table Node Pointer Array
 * @param[in]       aTableCount         Table Count
 * @param[in]       aEnabledHintError   Hint Error 처리 여부
 * @param[in]       aHintTable          Table Hint Parse Tree
 * @param[in,out]   aAccPathHintArr     Access Path Hint Array
 * @param[in]       aEnv                Environment
 * @remarks
 */
stlStatus qlvValidateHintTable( qlvStmtInfo         * aStmtInfo,
                                qlvInitNode        ** aTableNodePtrArr,
                                stlInt32              aTableCount,
                                stlBool               aEnabledHintError,
                                qlpHintTable        * aHintTable,
                                qlvAccessPathHint   * aAccPathHintArr,
                                qllEnv              * aEnv )
{
    stlBool                   sFound            = STL_FALSE;
    stlInt32                  sIdx              = 0;
    qlvInitBaseTableNode    * sBaseTableNode    = NULL;
    qlvInitSubTableNode     * sSubTableNode     = NULL;


    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableNodePtrArr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aHintTable != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAccPathHintArr != NULL, QLL_ERROR_STACK(aEnv) );    


    /**********************************************************
     * Hint 의 Table 정보 획득 
     **********************************************************/

    /**
     * Object Name에 일치하는 Table 찾기
     */

    for( sIdx = 0; sIdx < aTableCount; sIdx++ )
    {
        if( aTableNodePtrArr[sIdx]->mType == QLV_NODE_TYPE_BASE_TABLE )
        {
            sBaseTableNode = (qlvInitBaseTableNode*)aTableNodePtrArr[sIdx];
            sFound = qlvIsSameTable( sBaseTableNode->mRelationName,
                                     NULL, /* aHintTable->mObjectName->mCatalog */
                                     aHintTable->mObjectName->mSchema,
                                     aHintTable->mObjectName->mObject );
        }
        else
        {
            STL_DASSERT( aTableNodePtrArr[sIdx]->mType == QLV_NODE_TYPE_SUB_TABLE );
            sSubTableNode = (qlvInitSubTableNode*)aTableNodePtrArr[sIdx];
            sFound = qlvIsSameTable( sSubTableNode->mRelationName,
                                     NULL, /* aHintTable->mObjectName->mCatalog */
                                     aHintTable->mObjectName->mSchema,
                                     aHintTable->mObjectName->mObject );
        }

        if( sFound == STL_TRUE )
        {
            switch( aHintTable->mTableHintType )
            {
                case QLP_TABLE_HINT_FULL:
                    if( aAccPathHintArr[sIdx].mScanMethod != QLV_HINT_SCAN_METHOD_DEFAULT )
                    {
                        aStmtInfo->mHasHintError = STL_TRUE;
                            
                        STL_TRY_THROW( aEnabledHintError == STL_FALSE,
                                       RAMP_ERR_NOT_APPLICABLE_HINT );
                        /* 이미 다른 hint가 셋팅되어 있음 */
                        /* Do Nothing */
                    }
                    else
                    {
                        aAccPathHintArr[sIdx].mScanMethod = QLV_HINT_SCAN_METHOD_TABLE;
                    }
                    break;
                case QLP_TABLE_HINT_HASH:
                    aStmtInfo->mHasHintError = STL_TRUE;
                            
                    STL_TRY_THROW( aEnabledHintError == STL_FALSE,
                                   RAMP_ERR_NOT_IMPLEMENTED_HINT );
                    break;
                case QLP_TABLE_HINT_ROWID:
                    if( aAccPathHintArr[sIdx].mScanMethod != QLV_HINT_SCAN_METHOD_DEFAULT )
                    {
                        aStmtInfo->mHasHintError = STL_TRUE;
                            
                        STL_TRY_THROW( aEnabledHintError == STL_FALSE,
                                       RAMP_ERR_NOT_APPLICABLE_HINT );
                        /* 이미 다른 hint가 셋팅되어 있음 */
                        /* Do Nothing */
                    }
                    else
                    {
                        if( (aTableNodePtrArr[sIdx]->mType == QLV_NODE_TYPE_SUB_TABLE) ||
                            (((qlvInitBaseTableNode*)(aTableNodePtrArr[sIdx]))->mRefRowIdColumn == NULL) )
                        {
                            aStmtInfo->mHasHintError = STL_TRUE;
                            
                            STL_TRY_THROW( aEnabledHintError == STL_FALSE,
                                           RAMP_ERR_NOT_APPLICABLE_HINT );
                            /* Rowid 관련 조건이 없음 */
                            /* Do Nothing */
                        }
                        else
                        {
                            aAccPathHintArr[sIdx].mScanMethod = QLV_HINT_SCAN_METHOD_ROWID;
                        }
                    }
                    break;
                default:
                    STL_DASSERT( 0 );
                    break;
            }

            break;
        }
    }

    if( (aEnabledHintError == STL_TRUE) && (sFound == STL_FALSE) )
    {
        aStmtInfo->mHasHintError = STL_TRUE;
                            
        STL_THROW( RAMP_ERR_NOT_APPLICABLE_HINT );
    }

    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED_HINT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_IMPLEMENTED,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aHintTable->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      "qlvValidateHintTable()" );
    }

    STL_CATCH( RAMP_ERR_NOT_APPLICABLE_HINT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_APPLICABLE_HINT,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aHintTable->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Join Order 힌트를 Validation 한다.
 * @param[in]       aStmtInfo           Stmt Information
 * @param[in]       aTableNodePtrArr    Table Node Pointer Array
 * @param[in]       aTableCount         Table Count
 * @param[in]       aEnabledHintError   Hint Error 처리 여부
 * @param[in]       aHintJoinOrder      Join Order Hint Parse Tree
 * @param[in,out]   aJoinOrderArr       Join Order Array
 * @param[out]      aJoinOrderCount     Join Order Count
 * @param[in]       aEnv                Environment
 * @remarks
 */
stlStatus qlvValidateHintJoinOrder( qlvStmtInfo         * aStmtInfo,
                                    qlvInitNode        ** aTableNodePtrArr,
                                    stlInt32              aTableCount,
                                    stlBool               aEnabledHintError,
                                    qlpHintJoinOrder    * aHintJoinOrder,
                                    qlvJoinOrderHint    * aJoinOrderArr,
                                    stlInt32            * aJoinOrderCount,
                                    qllEnv              * aEnv )
{
    stlInt32                  sIdx              = 0;
    stlInt32                  sJoinOrderCount   = 0;
    stlInt32                  sTableIdx         = 0;

    stlBool                   sFound            = STL_FALSE;
    stlBool                   sIsFixedOrder     = STL_FALSE;
    stlBool                   sIsFixedPosition  = STL_FALSE;
    stlBool                   sIsLeft           = STL_FALSE;

    qlpListElement          * sListElem         = NULL;
    qlpObjectName           * sObjectName       = NULL;
    qlpOrdering             * sOrdering         = NULL;

    qlvInitBaseTableNode    * sBaseTableNode    = NULL;
    qlvInitSubTableNode     * sSubTableNode     = NULL;


    /**********************************************************
     * Parameter Validation
     **********************************************************/

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableNodePtrArr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableCount > 0, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aHintJoinOrder != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aJoinOrderArr != NULL, QLL_ERROR_STACK(aEnv) );    


    /**********************************************************
     * Join Order 분석
     **********************************************************/

    if( aHintJoinOrder->mJoinOrderHintType == QLP_JOIN_ORDER_HINT_ORDERED )
    {
        for( sIdx = 0; sIdx < aTableCount; sIdx++ )
        {
            aJoinOrderArr[sIdx].mJoinOrderIdx = sIdx;
            aJoinOrderArr[sIdx].mIsFixedOrder = STL_TRUE;
            aJoinOrderArr[sIdx].mIsFixedPosition = STL_FALSE;
            aJoinOrderArr[sIdx].mIsLeft = STL_FALSE;
        }

        *aJoinOrderCount = aTableCount;
    }
    else
    {
        sJoinOrderCount = 0;
        QLP_LIST_FOR_EACH( aHintJoinOrder->mObjectNameList, sListElem )
        {
            if( aHintJoinOrder->mJoinOrderHintType == QLP_JOIN_ORDER_HINT_LEADING )
            {
                sObjectName = (qlpObjectName *)QLP_LIST_GET_POINTER_VALUE( sListElem );
                sIsFixedOrder = STL_TRUE;
                sIsFixedPosition = STL_FALSE;
                sIsLeft = STL_FALSE;
            }
            else
            {
                sOrdering = (qlpOrdering *)QLP_LIST_GET_POINTER_VALUE( sListElem );
                sObjectName = sOrdering->mObjectName;
                sIsFixedOrder = STL_TRUE;
                if( sOrdering->mIsFixedPosition == STL_TRUE )
                {
                    sIsFixedPosition = STL_TRUE;
                    if( sOrdering->mIsLeft == STL_TRUE )
                    {
                        sIsLeft = STL_TRUE;
                    }
                    else
                    {
                        sIsLeft = STL_FALSE;
                    }
                }
                else
                {
                    sIsFixedPosition = STL_FALSE;
                    sIsLeft = STL_FALSE;
                }
            }

            /**
             * Object Name에 일치하는 Table 찾기
             */

            for( sIdx = 0; sIdx < aTableCount; sIdx++ )
            {
                if( aTableNodePtrArr[sIdx]->mType == QLV_NODE_TYPE_BASE_TABLE )
                {
                    sBaseTableNode = (qlvInitBaseTableNode*)aTableNodePtrArr[sIdx];
                    sFound = qlvIsSameTable( sBaseTableNode->mRelationName,
                                             NULL, /* aHintTable->mObjectName->mCatalog */
                                             sObjectName->mSchema,
                                             sObjectName->mObject );
                }
                else
                {
                    STL_DASSERT( aTableNodePtrArr[sIdx]->mType == QLV_NODE_TYPE_SUB_TABLE );
                    sSubTableNode = (qlvInitSubTableNode*)aTableNodePtrArr[sIdx];
                    sFound = qlvIsSameTable( sSubTableNode->mRelationName,
                                             NULL, /* aHintTable->mObjectName->mCatalog */
                                             sObjectName->mSchema,
                                             sObjectName->mObject );
                }

                if( sFound == STL_TRUE )
                {
                    sTableIdx = sIdx;
                    break;
                }
            }

            if( sFound == STL_TRUE )
            {
                /* 이미 등록한 table인지 확인 */
                sFound = STL_FALSE;
                for( sIdx = 0; sIdx < sJoinOrderCount; sIdx++ )
                {
                    if( aJoinOrderArr[sIdx].mJoinOrderIdx == sTableIdx )
                    {
                        sFound = STL_TRUE;
                        break;
                    }
                }

                if( sFound == STL_TRUE )
                {
                    /* 이미 존재함 */
                    aStmtInfo->mHasHintError = STL_TRUE;
                            
                    STL_TRY_THROW( aEnabledHintError == STL_FALSE,
                                   RAMP_ERR_NOT_APPLICABLE_HINT );
                }
                else
                {
                    /* ORDERING 힌트에서 처음 2 테이블에는 position을 지정할 수 없음 */
                    if( (sIsFixedPosition == STL_TRUE) &&
                        (sJoinOrderCount < 2) )
                    {
                        aStmtInfo->mHasHintError = STL_TRUE;
                            
                        STL_TRY_THROW( aEnabledHintError == STL_FALSE,
                                       RAMP_ERR_NOT_APPLICABLE_HINT );
                    }

                    aJoinOrderArr[sJoinOrderCount].mJoinOrderIdx = sTableIdx;
                    aJoinOrderArr[sJoinOrderCount].mIsFixedOrder = sIsFixedOrder;
                    aJoinOrderArr[sJoinOrderCount].mIsFixedPosition = sIsFixedPosition;
                    aJoinOrderArr[sJoinOrderCount].mIsLeft = sIsLeft;
                    sJoinOrderCount++;
                }
            }
            else
            {
                aStmtInfo->mHasHintError = STL_TRUE;
                            
                STL_TRY_THROW( aEnabledHintError == STL_FALSE,
                               RAMP_ERR_NOT_APPLICABLE_HINT );
            }
        }

        *aJoinOrderCount = sJoinOrderCount;
    }


    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_APPLICABLE_HINT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_APPLICABLE_HINT,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aHintJoinOrder->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Join Operation 힌트를 Validation 한다.
 * @param[in]       aStmtInfo           Stmt Information
 * @param[in]       aTableNodePtrArr    Table Node Pointer Array
 * @param[in]       aTableCount         Table Count
 * @param[in]       aEnabledHintError   Hint Error 처리 여부
 * @param[in]       aHintJoinOper       Join Operation Hint Parse Tree
 * @param[in,out]   aJoinOperList       Join Operation List
 * @param[in]       aEnv                Environment
 * @remarks
 */
stlStatus qlvValidateHintJoinOper( qlvStmtInfo      * aStmtInfo,
                                   qlvInitNode     ** aTableNodePtrArr,
                                   stlInt32           aTableCount,
                                   stlBool            aEnabledHintError,
                                   qlpHintJoinOper  * aHintJoinOper,
                                   qlvJoinOperList  * aJoinOperList,
                                   qllEnv           * aEnv )
{
    stlInt32                  sIdx              = 0;
    stlInt32                  sJoinTableCount   = 0;
    stlInt32                  sTableIdx         = 0;

    stlBool                   sIsExistHint      = STL_FALSE;
    stlBool                   sFound            = STL_FALSE;

    qlvJoinOperType           sJoinOperType     = QLV_HINT_JOIN_OPER_TYPE_DEFAULT;
    qlvJoinOperItem         * sJoinOperItem     = NULL;
    qlvJoinOperItem         * sTmpJoinOperItem  = NULL;

    qlpListElement          * sListElem         = NULL;
    qlpObjectName           * sObjectName       = NULL;

    qlvInitBaseTableNode    * sBaseTableNode    = NULL;
    qlvInitSubTableNode     * sSubTableNode     = NULL;


    /**********************************************************
     * Parameter Validation
     **********************************************************/

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableNodePtrArr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableCount > 0, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aHintJoinOper != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aJoinOperList != NULL, QLL_ERROR_STACK(aEnv) );    


    /**********************************************************
     * Join Operation 분석
     **********************************************************/

    /* Join Operation Type 분석 */
    switch( aHintJoinOper->mJoinOperHintType )
    {
        case QLP_JOIN_OPER_HINT_NL:
            if( aHintJoinOper->mIsNoUse == STL_FALSE )
            {
                sJoinOperType = QLV_HINT_JOIN_OPER_TYPE_USE_NL;
            }
            else
            {
                sJoinOperType = QLV_HINT_JOIN_OPER_TYPE_NO_USE_NL;
            }
            break;
        case QLP_JOIN_OPER_HINT_INL:
            if( aHintJoinOper->mIsNoUse == STL_FALSE )
            {
                sJoinOperType = QLV_HINT_JOIN_OPER_TYPE_USE_INL;
            }
            else
            {
                sJoinOperType = QLV_HINT_JOIN_OPER_TYPE_NO_USE_INL;
            }
            break;
        case QLP_JOIN_OPER_HINT_MERGE:
            if( aHintJoinOper->mIsNoUse == STL_FALSE )
            {
                sJoinOperType = QLV_HINT_JOIN_OPER_TYPE_USE_MERGE;
            }
            else
            {
                sJoinOperType = QLV_HINT_JOIN_OPER_TYPE_NO_USE_MERGE;
            }
            break;
        case QLP_JOIN_OPER_HINT_HASH:
            if( aHintJoinOper->mIsNoUse == STL_FALSE )
            {
                sJoinOperType = QLV_HINT_JOIN_OPER_TYPE_USE_HASH;
            }
            else
            {
                sJoinOperType = QLV_HINT_JOIN_OPER_TYPE_NO_USE_HASH;
            }
            break;
        default:
            STL_DASSERT( 0 );
            break;
    }

    /* 해당 Join Hint가 이미 존재하는지 체크 */
    /* 해당 Join Hint가 있으면 계속 덧붙인다. */
    sJoinOperItem = aJoinOperList->mHead;
    while( sJoinOperItem != NULL )
    {
        if( sJoinOperItem->mJoinOperType == sJoinOperType )
        {
            break;
        }

        sJoinOperItem = sJoinOperItem->mNext;
    }

    if( sJoinOperItem == NULL )
    {
        /* Join Operation Item 할당*/
        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                    STL_SIZEOF( qlvJoinOperItem ),
                                    (void **) & sJoinOperItem,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        sJoinOperItem->mJoinOperType = sJoinOperType;
        sJoinOperItem->mJoinTableCount = 0;
        sJoinOperItem->mNext = NULL;

        /* Join Table Array 할당*/
        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                    STL_SIZEOF( stlInt32 ) * aTableCount,
                                    (void **) & sJoinOperItem->mJoinTableArr,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        sJoinTableCount = 0;
        sIsExistHint = STL_FALSE;
    }
    else
    {
        sJoinTableCount = sJoinOperItem->mJoinTableCount;
        sIsExistHint = STL_TRUE;
    }

    /* Join Table 분석 */
    QLP_LIST_FOR_EACH( aHintJoinOper->mObjectNameList, sListElem )
    {
        sObjectName = (qlpObjectName *)QLP_LIST_GET_POINTER_VALUE( sListElem );

        /**
         * Object Name에 일치하는 Table 찾기
         */

        for( sIdx = 0; sIdx < aTableCount; sIdx++ )
        {
            if( aTableNodePtrArr[sIdx]->mType == QLV_NODE_TYPE_BASE_TABLE )
            {
                sBaseTableNode = (qlvInitBaseTableNode*)aTableNodePtrArr[sIdx];
                sFound = qlvIsSameTable( sBaseTableNode->mRelationName,
                                         NULL, /* aHintTable->mObjectName->mCatalog */
                                         sObjectName->mSchema,
                                         sObjectName->mObject );
            }
            else
            {
                STL_DASSERT( aTableNodePtrArr[sIdx]->mType == QLV_NODE_TYPE_SUB_TABLE );
                sSubTableNode = (qlvInitSubTableNode*)aTableNodePtrArr[sIdx];
                if( sSubTableNode->mRelationName->mTable != NULL )
                {
                    sFound = qlvIsSameTable( sSubTableNode->mRelationName,
                                             NULL, /* aHintTable->mObjectName->mCatalog */
                                             sObjectName->mSchema,
                                             sObjectName->mObject );
                }
                else
                {
                    sFound = STL_FALSE;
                }
            }

            if( sFound == STL_TRUE )
            {
                sTableIdx = sIdx;
                break;
            }
        }

        if( sFound == STL_TRUE )
        {
            /* 현재 Join Operation에 이미 등록한 table인지 확인 */
            sFound = STL_FALSE;
            for( sIdx = 0; sIdx < sJoinTableCount; sIdx++ )
            {
                if( sJoinOperItem->mJoinTableArr[sIdx] == sTableIdx )
                {
                    sFound = STL_TRUE;
                    break;
                }
            }

            if( sFound == STL_TRUE )
            {
                /* 이미 존재함 */
                aStmtInfo->mHasHintError = STL_TRUE;
                            
                STL_TRY_THROW( aEnabledHintError == STL_FALSE,
                               RAMP_ERR_NOT_APPLICABLE_HINT );
            }
            else
            {
                /* 다른 Join Operation에 이미 등록한 table인지 확인 */
                sFound = STL_FALSE;
                sTmpJoinOperItem = aJoinOperList->mHead;
                while( (sTmpJoinOperItem != NULL) && (sFound == STL_FALSE) )
                {
                    if( sTmpJoinOperItem->mJoinOperType != sJoinOperType )
                    {
                        for( sIdx = 0; sIdx < sTmpJoinOperItem->mJoinTableCount; sIdx++ )
                        {
                            if( sTmpJoinOperItem->mJoinTableArr[sIdx] == sTableIdx )
                            {
                                sFound = STL_TRUE;
                                break;
                            }
                        }
                    }

                    sTmpJoinOperItem = sTmpJoinOperItem->mNext;
                }

                if( sFound == STL_TRUE )
                {
                    /* 이미 존재함 */
                    aStmtInfo->mHasHintError = STL_TRUE;
                            
                    STL_TRY_THROW( aEnabledHintError == STL_FALSE,
                                   RAMP_ERR_NOT_APPLICABLE_HINT );
                }

                sJoinOperItem->mJoinTableArr[sJoinTableCount] = sTableIdx;
                sJoinTableCount++;
            }
        }
        else
        {
            aStmtInfo->mHasHintError = STL_TRUE;
                            
            STL_TRY_THROW( aEnabledHintError == STL_FALSE,
                           RAMP_ERR_NOT_APPLICABLE_HINT );
        }
    }

    sJoinOperItem->mJoinTableCount = sJoinTableCount;

    /* Join Operation List에 등록 */
    if( sIsExistHint == STL_FALSE )
    {
        if( aJoinOperList->mCount == 0 )
        {
            aJoinOperList->mHead = sJoinOperItem;
            aJoinOperList->mTail = sJoinOperItem;
        }
        else
        {
            aJoinOperList->mTail->mNext = sJoinOperItem;
            aJoinOperList->mTail = sJoinOperItem;
        }
        aJoinOperList->mCount++;
    }


    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_APPLICABLE_HINT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_APPLICABLE_HINT,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aHintJoinOper->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Query Transformation 힌트를 Validation 한다.
 * @param[in]       aStmtInfo           Stmt Information
 * @param[in]       aEnabledHintError   Hint Error 처리 여부
 * @param[in]       aCalledFromSubQuery Subquery로부터 이 함수가 호출되었는지 여부
 * @param[in]       aHintQueryTransform Query Transform Hint
 * @param[in,out]   aValidateQTHint     Query Transform Validate Hint
 * @param[in]       aEnv                Environment
 * @remarks
 */
stlStatus qlvValidateHintQueryTransform( qlvStmtInfo            * aStmtInfo,
                                         stlBool                  aEnabledHintError,
                                         stlBool                  aCalledFromSubQuery,
                                         qlpHintQueryTransform  * aHintQueryTransform,
                                         qlvQueryTransformHint  * aValidateQTHint,
                                         qllEnv                 * aEnv )
{
    /**********************************************************
     * Parameter Validation
     **********************************************************/

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aHintQueryTransform != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aValidateQTHint != NULL, QLL_ERROR_STACK(aEnv) );


    /**********************************************************
     * Query Transformation 분석
     **********************************************************/

    if( aCalledFromSubQuery == STL_FALSE )
    {
        aStmtInfo->mHasHintError = STL_TRUE;
                            
        STL_TRY_THROW( aEnabledHintError == STL_FALSE,
                       RAMP_ERR_NOT_APPLICABLE_HINT );
    }

    /* Query Transformation Type 분석 */
    switch( aHintQueryTransform->mQueryTransformHintType )
    {
        case QLP_QUERY_TRANSFORM_HINT_UNNEST:
            if( aValidateQTHint->mSubQueryUnnestType == QLV_HINT_SUBQUERY_UNNEST_TYPE_DEFAULT )
            {
                aValidateQTHint->mSubQueryUnnestType = QLV_HINT_SUBQUERY_UNNEST_TYPE_ALLOW;
            }
            else
            {
                aStmtInfo->mHasHintError = STL_TRUE;
                            
                STL_TRY_THROW( aEnabledHintError == STL_FALSE,
                               RAMP_ERR_NOT_APPLICABLE_HINT );
            }
            break;
        case QLP_QUERY_TRANSFORM_HINT_NO_UNNEST:
            if( aValidateQTHint->mSubQueryUnnestType == QLV_HINT_SUBQUERY_UNNEST_TYPE_DEFAULT )
            {
                aValidateQTHint->mSubQueryUnnestType = QLV_HINT_SUBQUERY_UNNEST_TYPE_FORBID;
            }
            else
            {
                aStmtInfo->mHasHintError = STL_TRUE;
                            
                STL_TRY_THROW( aEnabledHintError == STL_FALSE,
                               RAMP_ERR_NOT_APPLICABLE_HINT );
            }
            break;
        case QLP_QUERY_TRANSFORM_HINT_NL_ISJ:
            if( aValidateQTHint->mUnnestOperType == QLV_HINT_UNNEST_OPER_TYPE_DEFAULT )
            {
                aValidateQTHint->mUnnestOperType = QLV_HINT_UNNEST_OPER_TYPE_NL_ISJ;
            }
            else
            {
                aStmtInfo->mHasHintError = STL_TRUE;
                            
                STL_TRY_THROW( aEnabledHintError == STL_FALSE,
                               RAMP_ERR_NOT_APPLICABLE_HINT );
            }
            break;
        case QLP_QUERY_TRANSFORM_HINT_NL_SJ:
            if( aValidateQTHint->mUnnestOperType == QLV_HINT_UNNEST_OPER_TYPE_DEFAULT )
            {
                aValidateQTHint->mUnnestOperType = QLV_HINT_UNNEST_OPER_TYPE_NL_SJ;
            }
            else
            {
                aStmtInfo->mHasHintError = STL_TRUE;
                            
                STL_TRY_THROW( aEnabledHintError == STL_FALSE,
                               RAMP_ERR_NOT_APPLICABLE_HINT );
            }
            break;
        case QLP_QUERY_TRANSFORM_HINT_NL_AJ:
            if( aValidateQTHint->mUnnestOperType == QLV_HINT_UNNEST_OPER_TYPE_DEFAULT )
            {
                aValidateQTHint->mUnnestOperType = QLV_HINT_UNNEST_OPER_TYPE_NL_AJ;
            }
            else
            {
                aStmtInfo->mHasHintError = STL_TRUE;
                            
                STL_TRY_THROW( aEnabledHintError == STL_FALSE,
                               RAMP_ERR_NOT_APPLICABLE_HINT );
            }
            break;
        case QLP_QUERY_TRANSFORM_HINT_MERGE_SJ:
            if( aValidateQTHint->mUnnestOperType == QLV_HINT_UNNEST_OPER_TYPE_DEFAULT )
            {
                aValidateQTHint->mUnnestOperType = QLV_HINT_UNNEST_OPER_TYPE_MERGE_SJ;
            }
            else
            {
                aStmtInfo->mHasHintError = STL_TRUE;
                            
                STL_TRY_THROW( aEnabledHintError == STL_FALSE,
                               RAMP_ERR_NOT_APPLICABLE_HINT );
            }
            break;
        case QLP_QUERY_TRANSFORM_HINT_MERGE_AJ:
            if( aValidateQTHint->mUnnestOperType == QLV_HINT_UNNEST_OPER_TYPE_DEFAULT )
            {
                aValidateQTHint->mUnnestOperType = QLV_HINT_UNNEST_OPER_TYPE_MERGE_AJ;
            }
            else
            {
                aStmtInfo->mHasHintError = STL_TRUE;
                            
                STL_TRY_THROW( aEnabledHintError == STL_FALSE,
                               RAMP_ERR_NOT_APPLICABLE_HINT );
            }
            break;
        case QLP_QUERY_TRANSFORM_HINT_HASH_ISJ:
            if( aValidateQTHint->mUnnestOperType == QLV_HINT_UNNEST_OPER_TYPE_DEFAULT )
            {
                aValidateQTHint->mUnnestOperType = QLV_HINT_UNNEST_OPER_TYPE_HASH_ISJ;
            }
            else
            {
                aStmtInfo->mHasHintError = STL_TRUE;
                            
                STL_TRY_THROW( aEnabledHintError == STL_FALSE,
                               RAMP_ERR_NOT_APPLICABLE_HINT );
            }
            break;
        case QLP_QUERY_TRANSFORM_HINT_HASH_SJ:
            if( aValidateQTHint->mUnnestOperType == QLV_HINT_UNNEST_OPER_TYPE_DEFAULT )
            {
                aValidateQTHint->mUnnestOperType = QLV_HINT_UNNEST_OPER_TYPE_HASH_SJ;
            }
            else
            {
                aStmtInfo->mHasHintError = STL_TRUE;
                            
                STL_TRY_THROW( aEnabledHintError == STL_FALSE,
                               RAMP_ERR_NOT_APPLICABLE_HINT );
            }
            break;
        case QLP_QUERY_TRANSFORM_HINT_HASH_AJ:
            if( aValidateQTHint->mUnnestOperType == QLV_HINT_UNNEST_OPER_TYPE_DEFAULT )
            {
                aValidateQTHint->mUnnestOperType = QLV_HINT_UNNEST_OPER_TYPE_HASH_AJ;
            }
            else
            {
                aStmtInfo->mHasHintError = STL_TRUE;
                            
                STL_TRY_THROW( aEnabledHintError == STL_FALSE,
                               RAMP_ERR_NOT_APPLICABLE_HINT );
            }
            break;
        case QLP_QUERY_TRANSFORM_HINT_NO_QUERY_TRANSFORMATION:
            if( aValidateQTHint->mQueryTransformType == QLV_HINT_QUERY_TRANSFORM_TYPE_DEFAULT )
            {
                aValidateQTHint->mQueryTransformType = QLV_HINT_QUERY_TRANSFORM_TYPE_FORBID;
            }
            else
            {
                aStmtInfo->mHasHintError = STL_TRUE;
                            
                STL_TRY_THROW( aEnabledHintError == STL_FALSE,
                               RAMP_ERR_NOT_APPLICABLE_HINT );
            }
            break;
        default:
            aStmtInfo->mHasHintError = STL_TRUE;
                            
            STL_TRY_THROW( aEnabledHintError == STL_FALSE,
                           RAMP_ERR_NOT_APPLICABLE_HINT );
            break;
    }


    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_APPLICABLE_HINT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_APPLICABLE_HINT,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aHintQueryTransform->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Other Hint를 Validation 한다.
 * @param[in]       aStmtInfo           Stmt Information
 * @param[in]       aEnabledHintError   Hint Error 처리 여부
 * @param[in]       aCalledFromSubQuery Subquery로부터 이 함수가 호출되었는지 여부
 * @param[in]       aHasGroupBy         Group By를 갖는지 여부
 * @param[in]       aHasDistinct        Distinct를 갖는지 여부
 * @param[in]       aHintOther          Other Hint
 * @param[in,out]   aValidateOtherHint  Other Validate Hint
 * @param[in]       aEnv                Environment
 * @remarks
 */
stlStatus qlvValidateHintOther( qlvStmtInfo     * aStmtInfo,
                                stlBool           aEnabledHintError,
                                stlBool           aCalledFromSubQuery,
                                stlBool           aHasGroupBy,
                                stlBool           aHasDistinct,
                                qlpHintOther    * aHintOther,
                                qlvOtherHint    * aValidateOtherHint,
                                qllEnv          * aEnv )
{
    /**********************************************************
     * Parameter Validation
     **********************************************************/

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aHintOther != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aValidateOtherHint != NULL, QLL_ERROR_STACK(aEnv) );


    /**********************************************************
     * Query Transformation 분석
     **********************************************************/

    if( ( (aHintOther->mOtherHintType == QLP_OTHER_HINT_PUSH_PRED) ||
          (aHintOther->mOtherHintType == QLP_OTHER_HINT_PUSH_SUBQ) ) &&
        ( aCalledFromSubQuery == STL_FALSE ) )
    {
        aStmtInfo->mHasHintError = STL_TRUE;
                            
        STL_TRY_THROW( aEnabledHintError == STL_FALSE,
                       RAMP_ERR_NOT_APPLICABLE_HINT );
    }

    /* Other Hint Type 분석 */
    switch( aHintOther->mOtherHintType )
    {
        case QLP_OTHER_HINT_PUSH_PRED:
            aStmtInfo->mHasHintError = STL_TRUE;
                            
            STL_TRY_THROW( aEnabledHintError == STL_FALSE,
                           RAMP_ERR_NOT_APPLICABLE_HINT );
            break;
        case QLP_OTHER_HINT_PUSH_SUBQ:
            if( aValidateOtherHint->mPushSubq == QLV_HINT_DEFAULT )
            {
                if( aHintOther->mIsNoUse == STL_TRUE )
                {
                    aValidateOtherHint->mPushSubq = QLV_HINT_NO;
                }
                else
                {
                    aValidateOtherHint->mPushSubq = QLV_HINT_YES;
                }
            }
            else
            {
                aStmtInfo->mHasHintError = STL_TRUE;
                            
                STL_TRY_THROW( aEnabledHintError == STL_FALSE,
                               RAMP_ERR_NOT_APPLICABLE_HINT );
            }
            break;
        case QLP_OTHER_HINT_USE_DISTINCT_HASH:
            if( aValidateOtherHint->mUseDistinctHash == QLV_HINT_DEFAULT )
            {
                STL_DASSERT( aHintOther->mIsNoUse == STL_FALSE );
                if( aHasDistinct == STL_TRUE )
                {
                    aValidateOtherHint->mUseDistinctHash = QLV_HINT_YES;
                }
                else
                {
                    aStmtInfo->mHasHintError = STL_TRUE;
                            
                    STL_TRY_THROW( aEnabledHintError == STL_FALSE,
                                   RAMP_ERR_NOT_APPLICABLE_HINT );
                }
            }
            else
            {
                aStmtInfo->mHasHintError = STL_TRUE;
                            
                STL_TRY_THROW( aEnabledHintError == STL_FALSE,
                               RAMP_ERR_NOT_APPLICABLE_HINT );
            }

            break;

        case QLP_OTHER_HINT_USE_GROUP_HASH:
            if( aValidateOtherHint->mUseGroupHash == QLV_HINT_DEFAULT )
            {
                STL_DASSERT( aHintOther->mIsNoUse == STL_FALSE );
                if( aHasGroupBy == STL_TRUE )
                {
                    aValidateOtherHint->mUseGroupHash = QLV_HINT_YES;
                }
                else
                {
                    aStmtInfo->mHasHintError = STL_TRUE;
                            
                    STL_TRY_THROW( aEnabledHintError == STL_FALSE,
                                   RAMP_ERR_NOT_APPLICABLE_HINT );
                }
            }
            else
            {
                aStmtInfo->mHasHintError = STL_TRUE;
                            
                STL_TRY_THROW( aEnabledHintError == STL_FALSE,
                               RAMP_ERR_NOT_APPLICABLE_HINT );
            }

            break;
        default:
            STL_DASSERT( 0 );
            break;
    }


    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_APPLICABLE_HINT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_APPLICABLE_HINT,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aHintOther->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_FINISH;

    return STL_FAILURE;

}


/** @} qlv */

