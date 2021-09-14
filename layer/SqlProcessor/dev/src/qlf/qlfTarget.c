/*******************************************************************************
 * qlfTarget.c
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
 * @file qlfTarget.c
 * @brief Target Basic Information for ODBC, JDBC
 */

#include <qll.h>
#include <qlDef.h>

#include <qlfTarget.h>

/**
 * @addtogroup qlf
 * @{
 */


/**
 * @brief Query 의 Target Information 을 생성한다.
 * @param[in]  aSQLStmt      SQL Statement
 * @param[in]  aRegionMem    Region Memory
 * @param[out] aTargetList   Target Information List
 * @param[in]  aEnv          Environment
 * @remarks
 */
stlStatus qlfMakeQueryTarget( qllStatement   * aSQLStmt,
                              knlRegionMem   * aRegionMem,
                              qllTarget     ** aTargetList,
                              qllEnv         * aEnv )
{
    qllTarget              * sTargetInfo  = NULL;
    stlInt32                 sTargetCount = 0;

    ellDictHandle * sBaseColumnHandle = NULL;
    ellDictHandle * sColumnHandle = NULL;
    ellDictHandle * sTableHandle = NULL;
    ellDictHandle * sSchemaHandle = NULL;
    
    stlInt32 i = 0;

    /* Query Node in SELECT --> Query Spec  */
    /*                      |               */
    /*                      --> Query Set   */
    
    qlvInitSelect         * sInitSelect    = NULL;
    qlvInitNode           * sInitQueryNode = NULL;
    qlvInitQuerySetNode   * sInitQuerySet  = NULL;
    qlvInitQuerySpecNode  * sInitQuerySpec = NULL;

    qllOptimizationNode  * sOptNode       = NULL;
    qlnoSelectStmt       * sOptSelect     = NULL;

    qlvInitInsertReturnInto * sInitInsertReturn = NULL;
    qlnoInsertStmt          * sOptInsert    = NULL;
    
    qlvInitDelete        * sInitDelete   = NULL;
    qlnoDeleteStmt       * sOptDelete    = NULL;
    
    qlvInitUpdate        * sInitUpdate   = NULL;
    qlnoUpdateStmt       * sOptUpdate    = NULL;

    qlrInitCreateTable   * sCreateTableAsSelect = NULL;
    qlvInitNode          * sSubquery = NULL;

    qlrInitCreateView    * sCreateView = NULL;
    qlrInitAlterView     * sAlterView  = NULL;
    
    qlvInitTarget        * sTargetList = NULL;
    qlvInitTarget        * sTarget     = NULL;

    qlvInitExpression    * sTargetExpr = NULL;

    qlvInitBaseTableNode * sBaseTableNode = NULL;   
    qlvInitSubTableNode  * sSubTableNode = NULL;
    
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTargetList != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * Query 유형에 따른 기본 정보 획득
     */

    switch( aSQLStmt->mStmtType )
    {
        case QLL_STMT_SELECT_TYPE:
        case QLL_STMT_SELECT_FOR_UPDATE_TYPE:
            {
                sInitSelect = (qlvInitSelect *) qllGetInitPlan( aSQLStmt );
                sOptNode  = (qllOptimizationNode *) aSQLStmt->mCodePlan;

                if( sOptNode == NULL )
                {
                    /**
                     * Optimization 이전
                     */
                    
                    sInitQueryNode = sInitSelect->mQueryNode;
                    switch( sInitQueryNode->mType )
                    {
                        case QLV_NODE_TYPE_QUERY_SET :
                            {
                                sInitQuerySet = (qlvInitQuerySetNode*) sInitQueryNode;
                                sTargetCount  = sInitQuerySet->mSetTargetCount;
                                sTargetList   = sInitQuerySet->mSetTargets;
                                break;
                            }
                        case QLV_NODE_TYPE_QUERY_SPEC :
                            {
                                sInitQuerySpec = (qlvInitQuerySpecNode*) sInitQueryNode;
                                sTargetCount  = sInitQuerySpec->mTargetCount;
                                sTargetList   = sInitQuerySpec->mTargets;
                                break;
                            }
                        default :
                            {
                                STL_DASSERT( 0 );
                                break;
                            }
                    }
                }
                else
                {
                    /**
                     * Optimization 이후 
                     */
                    
                    sInitQueryNode = sInitSelect->mQueryNode;
                    sOptSelect = (qlnoSelectStmt *) sOptNode->mOptNode;

                    sTargetCount = sOptSelect->mTargetCnt;
                    sTargetList  = sOptSelect->mTargets;
                }
                break;
            }
        case QLL_STMT_INSERT_RETURNING_QUERY_TYPE:
            {
                sInitInsertReturn = (qlvInitInsertReturnInto *) qllGetInitPlan( aSQLStmt );
                sOptNode          = (qllOptimizationNode *) aSQLStmt->mCodePlan;

                if ( sOptNode == NULL )
                {
                    if ( sInitInsertReturn->mIsInsertValues == STL_TRUE )
                    {
                        sTargetCount = sInitInsertReturn->mInitInsertValues->mReturnTargetCnt;
                        sTargetList  = sInitInsertReturn->mInitInsertValues->mReturnTargetArray;
                    }
                    else
                    {
                        sTargetCount = sInitInsertReturn->mInitInsertSelect->mReturnTargetCnt;
                        sTargetList  = sInitInsertReturn->mInitInsertSelect->mReturnTargetArray;
                    }
                }
                else
                {
                    sOptInsert = sOptNode->mOptNode;

                    sTargetCount  = sOptInsert->mReturnExprCnt;
                    sTargetList   = sOptInsert->mReturnTarget;
                }
                
                break;
            }
        case QLL_STMT_DELETE_RETURNING_QUERY_TYPE:
            {
                sInitDelete = (qlvInitDelete *) qllGetInitPlan( aSQLStmt );
                sOptNode    = (qllOptimizationNode *) aSQLStmt->mCodePlan;

                if ( sOptNode == NULL )
                {
                    sTargetCount  = sInitDelete->mReturnTargetCnt;
                    sTargetList   = sInitDelete->mReturnTargetArray;
                }
                else
                {
                    sOptDelete = sOptNode->mOptNode;
                    
                    sTargetCount  = sOptDelete->mReturnExprCnt;
                    sTargetList   = sOptDelete->mReturnTarget;
                }
                break;
            }
        case QLL_STMT_UPDATE_RETURNING_QUERY_TYPE:
            {
                sInitUpdate = (qlvInitUpdate *) qllGetInitPlan( aSQLStmt );
                sOptNode    = (qllOptimizationNode *) aSQLStmt->mCodePlan;

                if ( sOptNode == NULL )
                {
                    sTargetCount  = sInitUpdate->mReturnTargetCnt;
                    sTargetList   = sInitUpdate->mReturnTargetArray;
                }
                else
                {
                    sOptUpdate = sOptNode->mOptNode;
                    
                    sTargetCount  = sOptUpdate->mReturnExprCnt;
                    sTargetList   = sOptUpdate->mReturnTarget;
                }
                break;
            }
        case QLL_STMT_CREATE_TABLE_AS_SELECT_TYPE:
            {   
                sCreateTableAsSelect = (qlrInitCreateTable *) qllGetInitPlan( aSQLStmt );
                sSubquery            = sCreateTableAsSelect->mSubQueryInitNode;
                
                switch( sCreateTableAsSelect->mSubQueryInitNode->mType )
                {
                    case QLV_NODE_TYPE_QUERY_SET :
                        {
                            sTargetCount = ((qlvInitQuerySetNode*) sSubquery)->mSetTargetCount;
                            sTargetList  = ((qlvInitQuerySetNode*) sSubquery)->mSetTargets;
                            break;
                        }
                    case QLV_NODE_TYPE_QUERY_SPEC :
                        {
                            sTargetCount = ((qlvInitQuerySpecNode*) sSubquery)->mTargetCount;
                            sTargetList  = ((qlvInitQuerySpecNode*) sSubquery)->mTargets;
                            break;
                        }
                    default :
                        {
                            STL_DASSERT( 0 );
                            break;
                        }
                }

                break;
            }
        case QLL_STMT_CREATE_VIEW_TYPE :
            {
                sCreateView = (qlrInitCreateView *) qllGetInitPlan( aSQLStmt );
                sSubquery   = sCreateView->mInitQueryNode;
                
                switch( sCreateView->mInitQueryNode->mType )
                {
                    case QLV_NODE_TYPE_QUERY_SET :
                        {
                            sTargetCount = ((qlvInitQuerySetNode*) sSubquery)->mSetTargetCount;
                            sTargetList  = ((qlvInitQuerySetNode*) sSubquery)->mSetTargets;
                            break;
                        }
                    case QLV_NODE_TYPE_QUERY_SPEC :
                        {
                            sTargetCount = ((qlvInitQuerySpecNode*) sSubquery)->mTargetCount;
                            sTargetList  = ((qlvInitQuerySpecNode*) sSubquery)->mTargets;
                            break;
                        }
                    default :
                        {
                            STL_DASSERT( 0 );
                            break;
                        }
                }
                
                break;
            }
        case QLL_STMT_ALTER_VIEW_TYPE :
            {
                sAlterView = (qlrInitAlterView *) qllGetInitPlan( aSQLStmt );
                sSubquery   = sAlterView->mInitQueryNode;
                
                switch( sAlterView->mInitQueryNode->mType )
                {
                    case QLV_NODE_TYPE_QUERY_SET :
                        {
                            sTargetCount = ((qlvInitQuerySetNode*) sSubquery)->mSetTargetCount;
                            sTargetList  = ((qlvInitQuerySetNode*) sSubquery)->mSetTargets;
                            break;
                        }
                    case QLV_NODE_TYPE_QUERY_SPEC :
                        {
                            sTargetCount = ((qlvInitQuerySpecNode*) sSubquery)->mTargetCount;
                            sTargetList  = ((qlvInitQuerySpecNode*) sSubquery)->mTargets;
                            break;
                        }
                    default :
                        {
                            STL_DASSERT( 0 );
                            break;
                        }
                }
                
                break;
            }
        default:
            {
                sTargetInfo = NULL;
                STL_THROW( RAMP_FINISH );
                break;
            }
    }
    
    /**
     * Target Information 배열 확보
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF(qllTarget) * sTargetCount,
                                (void **) & sTargetInfo,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    for ( i = 0; i < sTargetCount; i++ )
    {
        /**
         * Target 정보 초기화
         */

        qlfInitTarget( & sTargetInfo[i] );

        /**
         * 연결 정보 설정
         */
        
        if ( (i + 1) == sTargetCount )
        {
            sTargetInfo[i].mNext = NULL;
        }
        else
        {
            sTargetInfo[i].mNext = & sTargetInfo[i+1];
        }
    }
                 

    /**
     * Target 유형에 따른 Target Information 생성
     */

    for( i = 0; i < sTargetCount; i++ )
    {
        sTarget = & sTargetList[i];

        /* Target Expression은 Inner Column으로 구성된다. */
        sTargetExpr = sTarget->mExpr;
            
        STL_RAMP( GET_TARGET_INFO );
            
        switch( sTargetExpr->mType )
        {
            case QLV_EXPR_TYPE_COLUMN:
                {
                    sBaseTableNode = (qlvInitBaseTableNode *)sTargetExpr->mExpr.mColumn->mRelationNode;
                    sTableHandle   = & sBaseTableNode->mTableHandle;
                    sSchemaHandle  = & sBaseTableNode->mSchemaHandle;
                    sColumnHandle  = sTargetExpr->mExpr.mColumn->mColumnHandle;
                    break;
                }
            case QLV_EXPR_TYPE_INNER_COLUMN:
                {
                    sInitQueryNode = sTargetExpr->mExpr.mInnerColumn->mRelationNode;
                    sTableHandle   = NULL;
                    sSchemaHandle  = NULL;
                    sColumnHandle  = NULL;
                        
                    if ( sInitQueryNode->mType == QLV_NODE_TYPE_QUERY_SPEC )
                    {
                        STL_DASSERT( ((qlvInitQuerySpecNode *) sInitQueryNode)->mTableNode != NULL );

                        sInitQueryNode = ((qlvInitQuerySpecNode *) sInitQueryNode)->mTableNode;
                           
                        if ( sInitQueryNode->mType == QLV_NODE_TYPE_BASE_TABLE )
                        {
                            sTargetExpr = sTargetExpr->mExpr.mInnerColumn->mOrgExpr;
                                
                            STL_THROW( GET_TARGET_INFO );
                        }
                        else if ( sInitQueryNode->mType == QLV_NODE_TYPE_SUB_TABLE )
                        {
                            sSubTableNode = (qlvInitSubTableNode *) sInitQueryNode;
                            
                            if ( sSubTableNode->mHasHandle == STL_TRUE )
                            {
                                /**
                                 * Created View 인 경우
                                 */

                                sTableHandle = & sSubTableNode->mViewHandle;
                                sSchemaHandle = & sSubTableNode->mSchemaHandle;
                            }
                            else
                            {
                                /**
                                 * In-line View 인 경우
                                 */

                                /* Do Nothing */
                            }
                        }
                    }
                    break;
                }
            case QLV_EXPR_TYPE_ROWID_COLUMN:
                {
                    sBaseTableNode = (qlvInitBaseTableNode *)sTargetExpr->mExpr.mRowIdColumn->mRelationNode;
                    sTableHandle   = & sBaseTableNode->mTableHandle;
                    sSchemaHandle  = & sBaseTableNode->mSchemaHandle;
                    sColumnHandle  = NULL;
                    break;
                }
            default:
                {
                    sTableHandle  = NULL;
                    sSchemaHandle = NULL;
                    sColumnHandle = NULL;
                    break;
                }
        }

        
        /**
         * Base Column Handle 얻기
         */

        if( sTargetList[i].mDataTypeInfo.mIsBaseColumn == STL_TRUE )
        {
            sTargetExpr = sTarget->mExpr;
            
            while( 1 )
            {
                switch( sTargetExpr->mType )
                {
                    case QLV_EXPR_TYPE_COLUMN :
                        {
                            sBaseColumnHandle = sTargetExpr->mExpr.mColumn->mColumnHandle;
                            break;
                        }
                    case QLV_EXPR_TYPE_INNER_COLUMN :
                        {
                            sTargetExpr = sTargetExpr->mExpr.mInnerColumn->mOrgExpr;
                            continue;
                        }
                    case QLV_EXPR_TYPE_SUB_QUERY :
                        {
                            break;
                        }
                    default :
                        {
                            STL_DASSERT( 0 );
                            break;
                        }
                }
                break;
            }
        }
        
        
        /**
         * Target Info 설정
         */

        STL_TRY( qlfMakeTargetInfo( & sTargetList[i],
                                    sSchemaHandle,
                                    sTableHandle,
                                    sColumnHandle,
                                    sBaseColumnHandle,
                                    aRegionMem,
                                    & sTargetInfo[i],
                                    aEnv )
                 == STL_SUCCESS );
    }

    STL_RAMP( RAMP_FINISH );

    
    /**
     * Output 설정
     */

    *aTargetList = sTargetInfo;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Target Information 을 초기화한다.
 */
void qlfInitTarget( qllTarget * aTarget )
{
    stlMemset( aTarget, 0x00, STL_SIZEOF(qllTarget) );

    /*
     * Target Name 관련 정보
     */
    
    aTarget->mCatalogName = NULL;
    aTarget->mSchemaName = NULL;
    
    aTarget->mTableName = NULL;
    aTarget->mBaseTableName = NULL;

    /*
     * Target 의 관계모델 속성
     */
    
    aTarget->mIsAliasUnnamed = STL_FALSE;
    aTarget->mColumnAlias = NULL;
    aTarget->mColumnLabel = NULL;
    aTarget->mBaseColumnName = NULL;

    aTarget->mIsNullable   = STL_TRUE;
    aTarget->mIsUpdatable  = STL_FALSE;
    aTarget->mIsAutoUnique = STL_FALSE;
    aTarget->mIsRowVersion = STL_FALSE;
    aTarget->mIsAbleLike   = STL_FALSE;

    /*
     * Target 의 출력 공간
     */
    
    aTarget->mDisplaySize  = 0;

    /*
     * Target 의 Type 정보
     */
    
    aTarget->mDBType = DTL_TYPE_NA;
    aTarget->mConciseType = DTL_TYPE_NA;
    aTarget->mIntervalCode = DTL_INTERVAL_INDICATOR_NA;

    aTarget->mTypeName = NULL;
    aTarget->mLocalTypeName = NULL;

    /*
     * Target Type 의 Date/Time/Interval Precision 관련 정보
     */
    
    aTarget->mDateTimeIntervalPrec = 0;

    /*
     * Target Type 의 숫자 관련 정보
     */

    aTarget->mNumPrecRadix = DTL_NUMERIC_PREC_RADIX_NA;
    aTarget->mUnsigned = STL_TRUE;
    aTarget->mIsExactNumeric = STL_FALSE;
    aTarget->mPrecision = 0;
    aTarget->mScale = 0;

    /*
     * Target Type 의 문자 관련 Type 정보
     */
    
    aTarget->mIsCaseSensitive = STL_FALSE;

    aTarget->mCharacterLength = 0;
    aTarget->mOctetLength = 0;

    aTarget->mLiteralPrefix = NULL;
    aTarget->mLiteralSuffix = NULL;

    /*
     * 연결 정보
     */
    
    aTarget->mNext = NULL;
}


/**
 * @brief Target Information 을 복사한다.
 * @param[in]  aSrcTargetInfo   Source Information List
 * @param[out] aDstTargetInfo   Target Information List
 * @param[in]  aRegionMem       Region Memory
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus qlfCopyTarget( qllTarget      * aSrcTargetInfo,
                         qllTarget     ** aDstTargetInfo,
                         knlRegionMem   * aRegionMem,
                         qllEnv         * aEnv )
{
    qllTarget  * sOrgTargetInfo  = NULL;
    qllTarget  * sCurTargetInfo  = NULL;
    qllTarget  * sPrevTargetInfo = NULL;

    stlInt32     sStrLen;

    
#define QLF_ALLOC_AND_COPY_STRING( _aDstStr, _aSrcStr )         \
    {                                                           \
        if( (_aSrcStr) == NULL )                                \
        {                                                       \
            (_aDstStr) = NULL;                                  \
        }                                                       \
        else                                                    \
        {                                                       \
            sStrLen = stlStrlen( (_aSrcStr) );                  \
            STL_TRY( knlAllocRegionMem( aRegionMem,             \
                                        sStrLen + 1,            \
                                        (void**) &(_aDstStr),   \
                                        KNL_ENV( aEnv ) )       \
                     == STL_SUCCESS );                          \
            stlMemcpy( (_aDstStr), (_aSrcStr), sStrLen );       \
            (_aDstStr)[ sStrLen ] = '\0';                       \
        }                                                       \
    }

    
    /**
     * Paramter Validation
     */
    
    STL_PARAM_VALIDATE( aSrcTargetInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDstTargetInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * Target Info List 구성
     */
    
    sOrgTargetInfo = aSrcTargetInfo;

    while( sOrgTargetInfo != NULL )
    {
        /**
         * Target Info 공간 할당
         */

        STL_TRY( knlAllocRegionMem( aRegionMem,
                                    STL_SIZEOF( qllTarget ),
                                    (void **) & sCurTargetInfo,
                                    KNL_ENV( aEnv ) )
                 == STL_SUCCESS );


        /**
         * Target Name 관련 정보
         */

        QLF_ALLOC_AND_COPY_STRING( sCurTargetInfo->mCatalogName,
                                   sOrgTargetInfo->mCatalogName );

        QLF_ALLOC_AND_COPY_STRING( sCurTargetInfo->mSchemaName,
                                   sOrgTargetInfo->mSchemaName );

        QLF_ALLOC_AND_COPY_STRING( sCurTargetInfo->mTableName,
                                   sOrgTargetInfo->mTableName );

        QLF_ALLOC_AND_COPY_STRING( sCurTargetInfo->mBaseTableName,
                                   sOrgTargetInfo->mBaseTableName );

        sCurTargetInfo->mIsAliasUnnamed = sOrgTargetInfo->mIsAliasUnnamed;

        QLF_ALLOC_AND_COPY_STRING( sCurTargetInfo->mColumnAlias,
                                   sOrgTargetInfo->mColumnAlias );

        QLF_ALLOC_AND_COPY_STRING( sCurTargetInfo->mColumnLabel,
                                   sOrgTargetInfo->mColumnLabel );

        QLF_ALLOC_AND_COPY_STRING( sCurTargetInfo->mBaseColumnName,
                                   sOrgTargetInfo->mBaseColumnName );
    
        /**
         * Target 의 관계모델 속성
         */

        sCurTargetInfo->mIsNullable   = sOrgTargetInfo->mIsNullable;
        sCurTargetInfo->mIsUpdatable  = sOrgTargetInfo->mIsUpdatable;
        sCurTargetInfo->mIsAutoUnique = sOrgTargetInfo->mIsAutoUnique;
        sCurTargetInfo->mIsRowVersion = sOrgTargetInfo->mIsRowVersion;
        sCurTargetInfo->mIsAbleLike   = sOrgTargetInfo->mIsAbleLike;

        
        /**
         * Target 의 출력 공간
         */

        sCurTargetInfo->mDisplaySize = sOrgTargetInfo->mDisplaySize;


        /**
         * Target 의 Type 정보
         */

        sCurTargetInfo->mDBType       = sOrgTargetInfo->mDBType;
        sCurTargetInfo->mConciseType  = sOrgTargetInfo->mConciseType;
        sCurTargetInfo->mIntervalCode = sOrgTargetInfo->mIntervalCode;

        QLF_ALLOC_AND_COPY_STRING( sCurTargetInfo->mTypeName,
                                   sOrgTargetInfo->mTypeName );

        QLF_ALLOC_AND_COPY_STRING( sCurTargetInfo->mLocalTypeName,
                                   sOrgTargetInfo->mLocalTypeName );

        
        /**
         * Target Type 의 Date/Time/Interval Precision 관련 정보
         */

        sCurTargetInfo->mDateTimeIntervalPrec = sOrgTargetInfo->mDateTimeIntervalPrec;


        /**
         * Target Type 의 숫자 관련 정보
         */

        sCurTargetInfo->mNumPrecRadix   = sOrgTargetInfo->mNumPrecRadix;
        sCurTargetInfo->mUnsigned       = sOrgTargetInfo->mUnsigned;
        sCurTargetInfo->mIsExactNumeric = sOrgTargetInfo->mIsExactNumeric;
        sCurTargetInfo->mPrecision      = sOrgTargetInfo->mPrecision;
        sCurTargetInfo->mScale          = sOrgTargetInfo->mScale;


        /**
         * Target Type 의 문자 관련 Type 정보
         */

        sCurTargetInfo->mIsCaseSensitive  = sOrgTargetInfo->mIsCaseSensitive;
        sCurTargetInfo->mStringLengthUnit = sOrgTargetInfo->mStringLengthUnit;
        sCurTargetInfo->mCharacterLength  = sOrgTargetInfo->mCharacterLength;
        sCurTargetInfo->mOctetLength      = sOrgTargetInfo->mOctetLength;

        QLF_ALLOC_AND_COPY_STRING( sCurTargetInfo->mLiteralPrefix,
                                   sOrgTargetInfo->mLiteralPrefix );

        QLF_ALLOC_AND_COPY_STRING( sCurTargetInfo->mLiteralSuffix,
                                   sOrgTargetInfo->mLiteralSuffix );


        /**
         * Target Info Link
         */

        sCurTargetInfo->mNext = NULL;

        if( sPrevTargetInfo == NULL )
        {
            *aDstTargetInfo = sCurTargetInfo;
        }
        else
        {
            sPrevTargetInfo->mNext = sCurTargetInfo;
        }

        sPrevTargetInfo = sCurTargetInfo;


        /**
         * 다음 Target Info로 이동
         */
        
        sOrgTargetInfo = sOrgTargetInfo->mNext;
    }
    
            
    /**
     * Output 설정
     */

    STL_DASSERT( *aDstTargetInfo != NULL );
    
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Target 의 Information 을 생성
 * @param[in]  aCodeTarget       Code Target
 * @param[in]  aSchemaHandle     Schema Dictionary Handle
 * @param[in]  aTableHandle      Table Dictionary Handle
 * @param[in]  aColumnHandle     Column Dictionary Handle
 * @param[in]  aBaseColumnHandle Base Table Column Dictionary Handle
 * @param[in]  aRegionMem        Region Memory
 * @param[out] aTarget           Target Information
 * @param[in]  aEnv              Environment
 * @note Recompile 될 경우 Pointer 정보는 유효하지 않다.  Name 에 대한 공간을 확보해야 한다.
 * @remarks
 * - DB Type 에 대응하는 적합한 정보만을 설정한다.
 * - 이 외의 정보는 초기값으로 설정되어 있다.
 */
stlStatus qlfMakeTargetInfo( qlvInitTarget       * aCodeTarget,
                             ellDictHandle       * aSchemaHandle,
                             ellDictHandle       * aTableHandle,
                             ellDictHandle       * aColumnHandle,
                             ellDictHandle       * aBaseColumnHandle,
                             knlRegionMem        * aRegionMem,
                             qllTarget           * aTarget,
                             qllEnv              * aEnv )
{
    qlvInitDataTypeInfo  * sDataTypeDefInfo = NULL;
    ellColumnDesc        * sColumnDesc      = NULL;
    stlChar              * sName            = NULL;
    stlInt32               sNameLen         = 0;
    dtlDataType            sDataType        = DTL_TYPE_NA;

    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aCodeTarget != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTarget != NULL, QLL_ERROR_STACK(aEnv) );
    

    /**
     * 기본 정보 획득
     */

    sDataTypeDefInfo = & aCodeTarget->mDataTypeInfo;
    sDataType = sDataTypeDefInfo->mDataType;
    

    /********************************************************
     * Name 관련 정보 획득
     ********************************************************/
    
    /**
     * Catalog Name 설정
     */

    sName = ellGetDbCatalogName();
    STL_TRY( knlAllocRegionMem( aRegionMem,
                                stlStrlen( sName ) + 1,
                                (void **) & aTarget->mCatalogName,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlStrcpy( aTarget->mCatalogName, sName );

    
    /**
     * Object Name 설정
     */

    if( aTableHandle == NULL )
    {
        aTarget->mSchemaName = NULL;
        aTarget->mTableName = NULL;
        aTarget->mBaseTableName = NULL;
    }
    else
    {
        /**
         * Table Handle 이 존재하는 경우 (Base Table or Viewed Table)
         */

        STL_DASSERT( aSchemaHandle != NULL );

        sName = ellGetSchemaName( aSchemaHandle );
        STL_TRY( knlAllocRegionMem( aRegionMem,
                                    stlStrlen( sName ) + 1,
                                    (void **) & aTarget->mSchemaName,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        stlStrcpy( aTarget->mSchemaName, sName );
        
        sName = ellGetTableName( aTableHandle );
        STL_TRY( knlAllocRegionMem( aRegionMem,
                                    stlStrlen( sName ) + 1,
                                    (void **) & aTarget->mTableName,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        stlStrcpy( aTarget->mTableName, sName );
        
        aTarget->mBaseTableName = aTarget->mTableName;
    }


    /**
     * Target 의 Display Name (Column Label)
     */
    
    sName = aCodeTarget->mDisplayName;
    STL_TRY( knlAllocRegionMem( aRegionMem,
                                stlStrlen( sName ) + 1,
                                (void **) & aTarget->mColumnLabel,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlStrcpy( aTarget->mColumnLabel, sName );

    
    /**
     * Target 의 Base Column Name
     */

    if( aCodeTarget->mExpr->mColumnName == NULL )
    {
        aTarget->mBaseColumnName = NULL;
    }
    else
    {
        sName = aCodeTarget->mExpr->mColumnName;
        STL_TRY( knlAllocRegionMem( aRegionMem,
                                    stlStrlen( sName ) + 1,
                                    (void **) & aTarget->mBaseColumnName,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        stlStrcpy( aTarget->mBaseColumnName, sName );
    }
    

    /**
     * Target 의 Alias Name
     * - 순수 컬럼은 반드시 NAMED 이다. 
     */

    if( aColumnHandle == NULL )
    {
        if( aCodeTarget->mAliasName == NULL )
        {
            /* View 의 명시된 컬럼인 경우 */
            sName = aTarget->mBaseColumnName;
        }
        else
        {
            sName = aCodeTarget->mAliasName;
        }

        if( sName != NULL )
        {
            aTarget->mIsAliasUnnamed = STL_FALSE;
        
            STL_TRY( knlAllocRegionMem( aRegionMem,
                                        stlStrlen( sName ) + 1,
                                        (void **) & aTarget->mColumnAlias,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );
            stlStrcpy( aTarget->mColumnAlias, sName );
        }
        else
        {
            aTarget->mIsAliasUnnamed = STL_TRUE;
            aTarget->mColumnAlias    = NULL;
        }
    }
    else
    {
        if( aCodeTarget->mAliasName == NULL )
        {
            if( aTarget->mBaseColumnName == NULL )
            {
                sName = ellGetColumnName( aColumnHandle );
            }
            else
            {
                sName = aTarget->mBaseColumnName;
            }
        }
        else
        {
            sName = aCodeTarget->mAliasName;
        }

        aTarget->mIsAliasUnnamed = STL_FALSE;

        STL_TRY( knlAllocRegionMem( aRegionMem,
                                    stlStrlen( sName ) + 1,
                                    (void **) & aTarget->mColumnAlias,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        stlStrcpy( aTarget->mColumnAlias, sName );

    }

    
    /**
     * Column Handle for Base Table 
     */
    
    aTarget->mBaseColumnHandle = aBaseColumnHandle;
    
    
    /********************************************************
     * Target 의 관계모델 속성
     ********************************************************/

    aTarget->mIsNullable  = sDataTypeDefInfo->mNullable;
    aTarget->mIsUpdatable = sDataTypeDefInfo->mUpdatable;
    
    if( aColumnHandle == NULL )
    {
        aTarget->mIsAutoUnique = STL_FALSE;
    }
    else
    {
        if( ellGetColumnIdentityHandle( aColumnHandle ) == NULL )
        {
            aTarget->mIsAutoUnique = STL_FALSE;
        }
        else
        {
            aTarget->mIsAutoUnique = STL_TRUE;
        }
    }

    aTarget->mIsRowVersion = STL_FALSE;
    
    switch( sDataType )
    {
        case DTL_TYPE_CHAR:
        case DTL_TYPE_VARCHAR:
        case DTL_TYPE_LONGVARCHAR:
        case DTL_TYPE_CLOB:
            {
                aTarget->mIsAbleLike = STL_TRUE;
                break;
            }
        default:
            {
                aTarget->mIsAbleLike = STL_FALSE;
                break;
            }
    }
    

    /********************************************************
     * Target 의 출력 공간
     ********************************************************/
    
    aTarget->mDisplaySize = dtlGetDisplaySize( sDataType,
                                               sDataTypeDefInfo->mArgNum1,
                                               sDataTypeDefInfo->mArgNum2,
                                               sDataTypeDefInfo->mIntervalIndicator,
                                               KNL_DT_VECTOR(aEnv),
                                               aEnv );

    
    /********************************************************
     * Target 의 Type 정보
     ********************************************************/

    aTarget->mDBType       = sDataType;
    aTarget->mConciseType  = sDataType;
    aTarget->mIntervalCode = sDataTypeDefInfo->mIntervalIndicator;

    sName = gDataTypeDefinition[ sDataType ].mSqlNormalTypeName;
    STL_TRY( knlAllocRegionMem( aRegionMem,
                                stlStrlen( sName ) + 1,
                                (void **) & aTarget->mTypeName,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlStrcpy( aTarget->mTypeName, sName );
    
    aTarget->mLocalTypeName = aTarget->mTypeName;

    
    /********************************************************
     * Target Type 별 관련 정보
     * - Data Type 에 따른 관련 값만을 설정하고
     * - 이 외의 값은 초기화에 의한 값을 따른다.
     ********************************************************/

    switch( sDataType )
    {
        case DTL_TYPE_BOOLEAN:
            /* nothing to do */
            break;
        case DTL_TYPE_NATIVE_SMALLINT:
        case DTL_TYPE_NATIVE_INTEGER:
        case DTL_TYPE_NATIVE_BIGINT:
            {
                aTarget->mNumPrecRadix = DTL_NUMERIC_PREC_RADIX_BINARY;
                aTarget->mUnsigned = STL_FALSE;
                aTarget->mIsExactNumeric = STL_TRUE;
                aTarget->mPrecision = sDataTypeDefInfo->mArgNum1;
                aTarget->mScale = 0;
                break;
            }
        case DTL_TYPE_NATIVE_REAL:
        case DTL_TYPE_NATIVE_DOUBLE:
            {
                aTarget->mNumPrecRadix = DTL_NUMERIC_PREC_RADIX_BINARY;
                aTarget->mUnsigned = STL_FALSE;
                aTarget->mIsExactNumeric = STL_FALSE;
                aTarget->mPrecision = sDataTypeDefInfo->mArgNum1;
                aTarget->mScale = 0;
                break;
            }
        case DTL_TYPE_FLOAT:
            {
                aTarget->mUnsigned = STL_FALSE;
                if( aColumnHandle == NULL )
                {
                    aTarget->mNumPrecRadix = DTL_NUMERIC_PREC_RADIX_BINARY;
                    aTarget->mPrecision = sDataTypeDefInfo->mArgNum1;
                }
                else
                {
                    sColumnDesc = ellGetColumnDesc( aColumnHandle );
                    
                    aTarget->mNumPrecRadix = sColumnDesc->mColumnType.mDecimalNum.mRadix;
                    aTarget->mPrecision = sColumnDesc->mColumnType.mDecimalNum.mPrec;
                }
                
                aTarget->mScale = sDataTypeDefInfo->mArgNum2;
                aTarget->mIsExactNumeric = STL_FALSE;
                
                break;
            }
        case DTL_TYPE_NUMBER:
            {
                aTarget->mNumPrecRadix = DTL_NUMERIC_PREC_RADIX_DECIMAL;
                aTarget->mUnsigned = STL_FALSE;
                aTarget->mPrecision = sDataTypeDefInfo->mArgNum1;
                aTarget->mScale = sDataTypeDefInfo->mArgNum2;

                if( aTarget->mScale == DTL_SCALE_NA )
                {
                    /**
                     * NUMBER 임
                     */
                        
                    aTarget->mIsExactNumeric = STL_FALSE;
                }
                else
                {
                    /**
                     * NUMBER(p,s) 임
                     */
                        
                    aTarget->mIsExactNumeric = STL_TRUE;
                }
                
                break;
            }
            
        case DTL_TYPE_DECIMAL:
            STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );
            break;
    
        case DTL_TYPE_CHAR:
        case DTL_TYPE_VARCHAR:
        case DTL_TYPE_LONGVARCHAR:
            {
                aTarget->mIsCaseSensitive  = STL_TRUE;
                aTarget->mStringLengthUnit = sDataTypeDefInfo->mStringLengthUnit;
                aTarget->mCharacterLength  = sDataTypeDefInfo->mArgNum1;
                if ( aTarget->mStringLengthUnit == DTL_STRING_LENGTH_UNIT_CHARACTERS )
                {
                    aTarget->mOctetLength =
                        sDataTypeDefInfo->mArgNum1 * dtlGetMbMaxLength( ellGetDbCharacterSet() );
                }
                else
                {
                    aTarget->mOctetLength = sDataTypeDefInfo->mArgNum1;
                }
                
                aTarget->mLiteralPrefix    = NULL;
                aTarget->mLiteralSuffix    = NULL;
                break;
            }
        case DTL_TYPE_CLOB:
            STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );
            break;
    
        case DTL_TYPE_BINARY:
        case DTL_TYPE_VARBINARY:
        case DTL_TYPE_LONGVARBINARY:
            {
                aTarget->mIsCaseSensitive  = STL_FALSE;
                aTarget->mStringLengthUnit = sDataTypeDefInfo->mStringLengthUnit;
                aTarget->mCharacterLength  = sDataTypeDefInfo->mArgNum1;
                aTarget->mOctetLength      = sDataTypeDefInfo->mArgNum1;
                aTarget->mLiteralPrefix = NULL;
                aTarget->mLiteralSuffix = NULL;
                break;
            }
        case DTL_TYPE_BLOB:
            STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );
            break;

        case DTL_TYPE_DATE:
            {
                break;
            }
        case DTL_TYPE_TIME:
        case DTL_TYPE_TIMESTAMP:
        case DTL_TYPE_TIME_WITH_TIME_ZONE:
        case DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE:
            {
                aTarget->mPrecision = sDataTypeDefInfo->mArgNum1;
                break;
            }

        case DTL_TYPE_INTERVAL_YEAR_TO_MONTH:
        case DTL_TYPE_INTERVAL_DAY_TO_SECOND:
            {
                aTarget->mDateTimeIntervalPrec = sDataTypeDefInfo->mArgNum1;
                aTarget->mPrecision = sDataTypeDefInfo->mArgNum2;

                sNameLen =
                    stlStrlen( "INTERVAL " ) + stlStrlen( gDtlIntervalIndicatorString[ aTarget->mIntervalCode ] );

                STL_TRY( knlAllocRegionMem( aRegionMem,
                                            sNameLen + 1,
                                            (void **) & aTarget->mTypeName,
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                stlSnprintf( aTarget->mTypeName,
                             sNameLen + 1,
                             "INTERVAL %s",
                             gDtlIntervalIndicatorString[ aTarget->mIntervalCode ] );
                aTarget->mLocalTypeName = aTarget->mTypeName;
                
                break;
            }
        case DTL_TYPE_ROWID:
            {
                break;
            }
            
        default:
            STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );
            break;
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_IMPLEMENTED,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      "qlfMakeTargetInfo()" );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}


/** @} qlf */
