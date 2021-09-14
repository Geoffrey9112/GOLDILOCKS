/*******************************************************************************
 * qloRange.c
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
 * @file qloRange.c
 * @brief SQL Processor Layer RANGE statement optimization
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlv
 * @{
 */


/*******************************************************************************
 * Key-Range
 ******************************************************************************/


/**
 * @brief 
 * @param[in]     aSQLString       SQL String
 * @param[in]     aIndexHandle     Index Handle
 * @param[in]     aInitNode        Init Node
 * @param[in]     aBindContext     Bind Context 
 * @param[in]     aInitExprCode    Parse/Validation 단계의 Code Range
 * @param[out]    aIndexScanInfo   Range & Filter 구성 정보 
 * @param[in]     aRegionMem       Region Memory
 * @param[in]     aEnv             Environment
 *
 * @remark 기 구축된 ellRWColumnList를 분석하여 index가 선택되었음을 가정. 
 */
stlStatus qloGetKeyRange( stlChar             * aSQLString,
                          ellDictHandle       * aIndexHandle,
                          qlvInitNode         * aInitNode,
                          knlBindContext      * aBindContext,
                          qlvInitExpression   * aInitExprCode,
                          qloIndexScanInfo   ** aIndexScanInfo,
                          knlRegionMem        * aRegionMem,
                          qllEnv              * aEnv )
{
    qlvRefExprList    * sIndexColExprList = NULL;
    qlvRefExprList    * sTableColExprList = NULL;
    qloIndexScanInfo  * sIndexScanInfo    = NULL;
    stlInt32            sLoop             = 0;
    qlpValue            sExprSource;
    qlvStmtInfo         sStmtInfo;
    stlInt32            sKeyColCount;

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLString != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );

    stlMemset( & sStmtInfo, 0x00, STL_SIZEOF( qlvStmtInfo ) );
    
    
    /**
     * range/filter 구성을 위한 expression 분류 공간 할당 및 초기화 
     */

    /* Index Scan 공간 할당 */
    STL_TRY( knlAllocRegionMem(
                 aRegionMem,
                 STL_SIZEOF( qloIndexScanInfo ),
                 (void **) & sIndexScanInfo,
                 KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sIndexScanInfo, 0x00, STL_SIZEOF( qloIndexScanInfo ) );

    
    /**
     * INDEX SCAN METHOD 구분
     */

    switch( aInitNode->mType )
    {
        case QLV_NODE_TYPE_BASE_TABLE :
            sKeyColCount = ellGetIndexKeyCount( aIndexHandle );
            break;
        case QLV_NODE_TYPE_SORT_INSTANT :
        case QLV_NODE_TYPE_GROUP_SORT_INSTANT :
        case QLV_NODE_TYPE_SORT_JOIN_INSTANT :
            sKeyColCount = ((qlvInitInstantNode *) aInitNode)->mKeyColCnt;
            break;
        default :
            sKeyColCount = 0;
            STL_DASSERT( 0 );
            break;
    }
    
    
    if( aInitExprCode == NULL )
    {
        /* index full scan */
        sIndexScanInfo->mIsExist     = STL_FALSE;
        sIndexScanInfo->mEmptyValue  = STL_TRUE;

        sIndexScanInfo->mKeyColCount = sKeyColCount;
        sIndexScanInfo->mReadKeyCnt  = 0;

        sIndexScanInfo->mMinRangeExpr  = NULL;
        sIndexScanInfo->mMaxRangeExpr  = NULL;
        
        sIndexScanInfo->mLogicalKeyFilterExprList   = NULL;
        sIndexScanInfo->mLogicalTableFilterExprList = NULL;

        sIndexScanInfo->mPhysicalKeyFilterExprList   = NULL;
        sIndexScanInfo->mPhysicalTableFilterExprList = NULL;

        sIndexScanInfo->mLogicalKeyFilter   = NULL;
        sIndexScanInfo->mLogicalTableFilter = NULL;

        sIndexScanInfo->mNullValueExpr  = NULL;
        sIndexScanInfo->mFalseValueExpr = NULL;
    }
    else  /* index range scan */
    {
        /**
         * Expression을 Range / Filter 요소로 구분하기
         * @todo OR를 지원하는 경우 변경 필요
         */


        /**
         * Index Scan Info 초기화
         */
        
        sIndexScanInfo->mIsExist     = STL_TRUE;
        sIndexScanInfo->mEmptyValue  = STL_FALSE;
        sIndexScanInfo->mKeyColCount = sKeyColCount;
        sIndexScanInfo->mReadKeyCnt  = 0;

        /* Min-Range 공간 할당 */
        STL_TRY( knlAllocRegionMem(
                     aRegionMem,
                     STL_SIZEOF( qlvRefExprList * ) * sIndexScanInfo->mKeyColCount,
                     (void **) & sIndexScanInfo->mMinRangeExpr,
                     KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        stlMemset( sIndexScanInfo->mMinRangeExpr,
                   0x00,
                   STL_SIZEOF( qlvRefExprList * ) * sIndexScanInfo->mKeyColCount );

        for( sLoop = 0 ; sLoop < sIndexScanInfo->mKeyColCount ; sLoop++ )
        {
            STL_TRY( qlvCreateRefExprList( & sIndexScanInfo->mMinRangeExpr[ sLoop ],
                                           aRegionMem,
                                           aEnv )
                     == STL_SUCCESS );
        }

        /* Max-Range 공간 할당 */
        STL_TRY( knlAllocRegionMem(
                     aRegionMem,
                     STL_SIZEOF( qlvRefExprList * ) * sIndexScanInfo->mKeyColCount,
                     (void **) & sIndexScanInfo->mMaxRangeExpr,
                     KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        stlMemset( sIndexScanInfo->mMaxRangeExpr,
                   0x00,
                   STL_SIZEOF( qlvRefExprList * ) * sIndexScanInfo->mKeyColCount );

        for( sLoop = 0 ; sLoop < sIndexScanInfo->mKeyColCount ; sLoop++ )
        {
            STL_TRY( qlvCreateRefExprList( & sIndexScanInfo->mMaxRangeExpr[ sLoop ],
                                           aRegionMem,
                                           aEnv )
                     == STL_SUCCESS );
        }

        /* logical key filter 공간 할당 */
        STL_TRY( qlvCreateRefExprList( & sIndexScanInfo->mLogicalKeyFilterExprList,
                                        aRegionMem,
                                        aEnv )
                 == STL_SUCCESS );
        
        /* logical table filter 공간 할당 */
        STL_TRY( qlvCreateRefExprList( & sIndexScanInfo->mLogicalTableFilterExprList,
                                        aRegionMem,
                                        aEnv )
                 == STL_SUCCESS );

        /* physical key filter 공간 할당 */
        STL_TRY( qlvCreateRefExprList( & sIndexScanInfo->mPhysicalKeyFilterExprList,
                                        aRegionMem,
                                        aEnv )
                 == STL_SUCCESS );
        
        /* physical table filter 공간 할당 */
        STL_TRY( qlvCreateRefExprList( & sIndexScanInfo->mPhysicalTableFilterExprList,
                                        aRegionMem,
                                        aEnv )
                 == STL_SUCCESS );

        /* Index Column Expression List */
        STL_TRY( qlvCreateRefExprList( & sIndexColExprList,
                                        aRegionMem,
                                        aEnv )
                 == STL_SUCCESS );
        
        /* Table Column Expression List */
        STL_TRY( qlvCreateRefExprList( & sTableColExprList,
                                        aRegionMem,
                                        aEnv )
                 == STL_SUCCESS );

        /* Predicate 분류 */
        STL_TRY( qloClassifyPredExpr( aSQLString,
                                      aIndexHandle,
                                      aInitNode,
                                      aBindContext,
                                      aInitExprCode,
                                      sIndexScanInfo,
                                      sIndexColExprList,
                                      sTableColExprList,
                                      aRegionMem,
                                      aEnv )
                 == STL_SUCCESS );

        
        /**
         * Range & Key-Filter 조정하기
         */

        STL_TRY( qloExtractRange( aIndexHandle,
                                  aInitNode,
                                  sIndexScanInfo,
                                  aRegionMem,
                                  aEnv )
                 == STL_SUCCESS );
        
        if( sIndexScanInfo->mIsExist == STL_FALSE )
        {
            /**
             * scan 하지 않음
             */

            sIndexScanInfo->mReadKeyCnt  = 0;
            
            sIndexScanInfo->mMinRangeExpr = NULL;
            sIndexScanInfo->mMaxRangeExpr = NULL;

            sIndexScanInfo->mLogicalKeyFilterExprList   = NULL;
            sIndexScanInfo->mLogicalTableFilterExprList = NULL;
            
            sIndexScanInfo->mPhysicalKeyFilterExprList   = NULL;
            sIndexScanInfo->mPhysicalTableFilterExprList = NULL;

            sIndexScanInfo->mLogicalKeyFilter   = NULL;
            sIndexScanInfo->mLogicalTableFilter = NULL;

            sIndexScanInfo->mNullValueExpr  = NULL;
            sIndexScanInfo->mFalseValueExpr = NULL;
        }
        else
        {
            /* logical key filter expression 구성 */
            STL_TRY( qloLinkRangeExpr( & sIndexScanInfo->mLogicalKeyFilter,
                                       sIndexScanInfo->mLogicalKeyFilterExprList,
                                       aRegionMem,
                                       aEnv )
                     == STL_SUCCESS );

            /* logical table filter expression 구성 */
            STL_TRY( qloLinkRangeExpr( & sIndexScanInfo->mLogicalTableFilter,
                                       sIndexScanInfo->mLogicalTableFilterExprList,
                                       aRegionMem,
                                       aEnv )
                     == STL_SUCCESS );
    
            /* NULL Value Expression 생성 */
            sExprSource.mType    = QLL_BNF_NULL_CONSTANT_TYPE;
            sExprSource.mNodePos = 0;
            STL_TRY( qlvValidateNullBNF( & sStmtInfo,
                                         & sExprSource,
                                         & sIndexScanInfo->mNullValueExpr,
                                         KNL_BUILTIN_FUNC_INVALID,
                                         DTL_ITERATION_TIME_NONE,
                                         DTL_ITERATION_TIME_NONE,
                                         aRegionMem,
                                         aEnv )
                     == STL_SUCCESS );

            /* FALSE Value Expression 생성 */
            sExprSource.mType    = QLL_BNF_INTPARAM_CONSTANT_TYPE;
            sExprSource.mNodePos = 0;
            sExprSource.mValue.mInteger = STL_FALSE;
            STL_TRY( qlvValidateIntParamBNF( & sStmtInfo,
                                             & sExprSource,
                                             & sIndexScanInfo->mFalseValueExpr,
                                             KNL_BUILTIN_FUNC_INVALID,
                                             DTL_ITERATION_TIME_NONE,
                                             DTL_ITERATION_TIME_NONE,
                                             aRegionMem,
                                             aEnv )
                     == STL_SUCCESS );
        }
    }


    /**
     * OUTPUT 설정
     */
    
    *aIndexScanInfo = sIndexScanInfo;

    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Expression을 range/filter 대상으로 구분하기
 * @param[in]     aSQLString          SQL String
 * @param[in]     aIndexHandle        Index Handle
 * @param[in]     aInitNode           Init Node
 * @param[in]     aBindContext        Bind Context 
 * @param[in]     aInitExprCode       Parse/Validation 단계의 Code Range
 * @param[in,out] aIndexScanInfo      Range/Filter Expression 분류 정보
 * @param[in,out] aIndexColExprList   Index Column List 
 * @param[in,out] aTableColExprList   Table Column List 
 * @param[in]     aRegionMem          Region Memory
 * @param[in]     aEnv                Environment
 *
 * @remark 기 구축된 ellRWColumnList를 분석하여 index가 선택되었음을 가정. 
 */
stlStatus qloClassifyPredExpr( stlChar             * aSQLString,
                               ellDictHandle       * aIndexHandle,
                               qlvInitNode         * aInitNode,
                               knlBindContext      * aBindContext,
                               qlvInitExpression   * aInitExprCode,
                               qloIndexScanInfo    * aIndexScanInfo,
                               qlvRefExprList      * aIndexColExprList,
                               qlvRefExprList      * aTableColExprList,
                               knlRegionMem        * aRegionMem,
                               qllEnv              * aEnv )
{
    qlvInitExpression     * sInitExprCode    = NULL;
    qlvInitExpression     * sNewExpr         = NULL;
    qlvInitExpression     * sTmpExpr         = NULL;
    qlvInitFunction       * sInitFunction    = NULL;
    qlvInitListFunc       * sListFunc        = NULL;
    qlvInitListCol        * sListColLeft     = NULL;
    qlvInitListCol        * sListColRight    = NULL;
    qlvInitRowExpr        * sRowExpr         = NULL;
    stlInt32                sLastColIdx      = 0;
    stlInt32                sColIdx          = 0;
    stlInt32                sLoop            = 0;

    stlBool                 sHasRowIdCol     = STL_FALSE;
    stlBool                 sIsIncludeKeyCol = STL_FALSE;
    stlBool                 sIsRange         = STL_FALSE;
    stlBool                 sIsTableFilter   = STL_FALSE;
    stlBool                 sIsLogicalFilter = STL_FALSE;
    stlBool                 sIsSameColumn    = STL_FALSE;
    stlBool                 sHasRangeFunc    = STL_FALSE;

    dtlDataType             sDataType1       = DTL_TYPE_NA;
    dtlDataType             sDataType2       = DTL_TYPE_NA;
    dtlPhysicalFunc         sPhysicalFuncPtr = NULL;
    
    ellIndexKeyDesc       * sIndexKeyDesc    = NULL;
    stlInt32                sKeyColCnt       = 0;

    qlvRefExprList          sTableColExprList;
    qlvRefExprList          sIndexColExprList;
    const dtlCompareType  * sCompareType     = NULL;
    stlInt32                i;

    /**
     * MACRO (코드 단순화 등을 위해 반복 사용되는 부분을 macro로 만듬)
     */


#define ADD_PHYSICAL_KEY_FILTER( aExpr )                                                \
    {                                                                                   \
        STL_TRY( qlvAddExprToRefExprList( aIndexScanInfo->mPhysicalKeyFilterExprList,   \
                                          (aExpr),                                      \
                                          STL_FALSE,                                    \
                                          aRegionMem,                                   \
                                          aEnv )                                        \
                 == STL_SUCCESS );                                                      \
    }

#define ADD_LOGICAL_KEY_FILTER( aExpr )                                                 \
    {                                                                                   \
        STL_TRY( qlvAddExprToRefExprList( aIndexScanInfo->mLogicalKeyFilterExprList,    \
                                          (aExpr),                                      \
                                          STL_FALSE,                                    \
                                          aRegionMem,                                   \
                                          aEnv )                                        \
                 == STL_SUCCESS );                                                      \
    }

#define ADD_PHYSICAL_TABLE_FILTER( aExpr )                                              \
    {                                                                                   \
        STL_TRY( qlvAddExprToRefExprList( aIndexScanInfo->mPhysicalTableFilterExprList, \
                                          (aExpr),                                      \
                                          STL_FALSE,                                    \
                                          aRegionMem,                                   \
                                          aEnv )                                        \
                 == STL_SUCCESS );                                                      \
    }

#define ADD_LOGICAL_TABLE_FILTER( aExpr )                                               \
    {                                                                                   \
        STL_TRY( qlvAddExprToRefExprList( aIndexScanInfo->mLogicalTableFilterExprList,  \
                                          (aExpr),                                      \
                                          STL_FALSE,                                    \
                                          aRegionMem,                                   \
                                          aEnv )                                        \
                 == STL_SUCCESS );                                                      \
    }

#define ADD_MIN_RANGE( aExpr, aIdx )                                                    \
    {                                                                                   \
        STL_TRY( qlvAddExprToRefExprList( aIndexScanInfo->mMinRangeExpr[ (aIdx) ],      \
                                          (aExpr),                                      \
                                          STL_FALSE,                                    \
                                          aRegionMem,                                   \
                                          aEnv )                                        \
                 == STL_SUCCESS );                                                      \
        sLastColIdx = ( sLastColIdx > (aIdx) + 1 ) ? sLastColIdx : (aIdx) + 1;          \
    }

#define GET_INDEX_IDX( aExpr, aIdx )                                                    \
    {                                                                                   \
        if( aInitNode->mType == QLV_NODE_TYPE_BASE_TABLE )                              \
        {                                                                               \
            for( (aIdx) = 0 ; (aIdx) < sKeyColCnt ; (aIdx)++ )                          \
            {                                                                           \
                if( ellGetColumnIdx( (aExpr)->mExpr.mColumn->mColumnHandle ) ==         \
                    ellGetColumnIdx( & sIndexKeyDesc[ (aIdx) ].mKeyColumnHandle ) )     \
                {                                                                       \
                    break;                                                              \
                }                                                                       \
            }                                                                           \
            STL_DASSERT( (aIdx) < sKeyColCnt );                                         \
        }                                                                               \
        else                                                                            \
        {                                                                               \
            STL_DASSERT( (aExpr)->mType == QLV_EXPR_TYPE_INNER_COLUMN );                \
            (aIdx) = *((aExpr)->mExpr.mInnerColumn->mIdx);                              \
        }                                                                               \
    }
    
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLString != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitExprCode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIndexScanInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIndexColExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableColExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    

    /**
     * Iteration Time이 Each Expression이 아닌 경우는 Column을 포함하지 않은 경우이다.
     */
    
    sLastColIdx = aIndexScanInfo->mReadKeyCnt;
    if( aInitExprCode->mIterationTime != DTL_ITERATION_TIME_FOR_EACH_EXPR )
    {
        /**
         * constant value만 포함된 조건은 Fetch 이전의 Execution에서 수행하여야 함.
         */

        /**
         * @todo 해당 Expression을 Fetch 단위 Constant Expression으로 등록하고,
         *  <BR> Constant Expression은 Logical Key Filter에 추가한다.
         *  <BR> 
         *  <BR> 구현 이전까지는 해당 Expression을 logical filter로 분류한다. 
         */

        /* Add to Key Logical Filter */
        ADD_LOGICAL_KEY_FILTER( aInitExprCode );

        STL_THROW( RAMP_FINISH );
    }
    else
    {
        /* Do Nothing */
    }


    /**
     * Expression 분류
     */
    
    if( aInitExprCode->mType != QLV_EXPR_TYPE_FUNCTION )
    {
        /**
         * Function이 아닌 Expression에 대한 Filter 분류
         */

        STL_TRY( qloGetRefColListForFilter( aIndexHandle,
                                            aInitNode,
                                            aInitExprCode,
                                            aIndexColExprList,
                                            aTableColExprList,
                                            & sHasRowIdCol,
                                            aRegionMem,
                                            aEnv )
                 == STL_SUCCESS );

        if( aInitExprCode->mType != QLV_EXPR_TYPE_LIST_FUNCTION )
        {
            if( aTableColExprList->mCount == 0 )
            {
                /* Add to Key Logical Filter */
                ADD_LOGICAL_KEY_FILTER( aInitExprCode );
            }
            else
            {
                /* Add to Table Logical Filter */
                ADD_LOGICAL_TABLE_FILTER( aInitExprCode );
            }
            STL_THROW( RAMP_FINISH );
        }
        else
        {
            sInitExprCode = aInitExprCode;
            sListFunc     = sInitExprCode->mExpr.mListFunc;

            STL_DASSERT( ( sListFunc->mArgs[0]->mType == QLV_EXPR_TYPE_LIST_COLUMN ) &&
                         ( sListFunc->mArgCount == 2 ) );
            
            sListColLeft  = sListFunc->mArgs[1]->mExpr.mListCol;
            sListColRight  = sListFunc->mArgs[0]->mExpr.mListCol;
            STL_DASSERT( ( sListColLeft->mArgs[0]->mType == QLV_EXPR_TYPE_ROW_EXPR ) &&
                         ( sListColRight->mArgs[0]->mType == QLV_EXPR_TYPE_ROW_EXPR ) );

            sRowExpr  = sListColLeft->mArgs[0]->mExpr.mRowExpr;
             
            if( ( sRowExpr->mArgCount == 1 ) &&
                ( ( sListFunc->mFuncId >= KNL_LIST_FUNC_START_ID ) &&
                  ( sListFunc->mFuncId <= KNL_LIST_FUNC_END_ID ) ) )
            {
                if( sListFunc->mArgs[0]->mIterationTime != DTL_ITERATION_TIME_FOR_EACH_EXPR )
                {
                    if( ( sRowExpr->mArgs[0]->mType == QLV_EXPR_TYPE_COLUMN ) ||
                        ( sRowExpr->mArgs[0]->mType == QLV_EXPR_TYPE_INNER_COLUMN ) )
                    {
                        /* physical function이 없으면 Logical Filter로 분류 */

                        for( i = 0; i < sListColLeft->mArgCount; i ++ )
                        {
                            STL_TRY( qloGetDataType( aSQLString,
                                                     sListColLeft->mArgs[i]->mExpr.mRowExpr->mArgs[0],
                                                     aBindContext,
                                                     & sDataType1,
                                                     aRegionMem,
                                                     aEnv )
                                     == STL_SUCCESS );

                            STL_TRY( qloGetDataType( aSQLString,
                                                     sListColRight->mArgs[i]->mExpr.mRowExpr->mArgs[0],
                                                     aBindContext,
                                                     & sDataType2,
                                                     aRegionMem,
                                                     aEnv )
                                     == STL_SUCCESS );

                            STL_TRY( qlnoGetCompareType( aSQLString,
                                                         sDataType1,
                                                         sDataType2,
                                                         STL_FALSE,
                                                         STL_TRUE,
                                                         aInitExprCode->mPosition,
                                                         &sCompareType,
                                                         aEnv )
                                     == STL_SUCCESS );

                            if( sCompareType->mLeftType == sDataType1 )
                            {
                                qloGetPhysicalFuncPtr( KNL_BUILTIN_FUNC_IS_EQUAL,
                                                       sDataType1,
                                                       sDataType1,
                                                       & sPhysicalFuncPtr,
                                                       STL_TRUE );
                            }
                            else
                            {
                                qloGetPhysicalFuncPtr( KNL_BUILTIN_FUNC_IS_EQUAL,
                                                       sDataType1,
                                                       sDataType2,
                                                       & sPhysicalFuncPtr,
                                                       STL_TRUE );
                            }
                            
                            if( sPhysicalFuncPtr == NULL )
                            {
                                break;
                            }
                        }
                    }
                }
            }
        }
        if( sPhysicalFuncPtr != NULL )
        {
            if( aTableColExprList->mCount == 0 )
            {
                /* Add to Key Logical Filter */
                ADD_PHYSICAL_KEY_FILTER( aInitExprCode );
            }
            else
            {
                /* Add to Table Logical Filter */
                ADD_PHYSICAL_TABLE_FILTER( aInitExprCode );
            }
        }
        else
        {
            if( aTableColExprList->mCount == 0 )
            {
                /* Add to Key Logical Filter */
                ADD_LOGICAL_KEY_FILTER( aInitExprCode );
            }
            else
            {
                /* Add to Table Logical Filter */
                ADD_LOGICAL_TABLE_FILTER( aInitExprCode );
            }
        }
        STL_THROW( RAMP_FINISH );
    }
    else
    {
        /* Do Nothing */
    }

    
    /**
     * Function Expression 처리
     */
    
    sInitExprCode = aInitExprCode;
    sInitFunction = sInitExprCode->mExpr.mFunction;

    if( aInitNode->mType == QLV_NODE_TYPE_BASE_TABLE )
    {
        sKeyColCnt    = ellGetIndexKeyCount( aIndexHandle );
        sIndexKeyDesc = ellGetIndexKeyDesc( aIndexHandle );
    }
    else
    {
        /* Do Nothing */
    }

    if( sInitFunction->mFuncId == KNL_BUILTIN_FUNC_AND )
    {
        STL_PARAM_VALIDATE( sInitFunction->mArgCount >= 2, QLL_ERROR_STACK(aEnv) );

        for( i = 0; i < sInitFunction->mArgCount; i++ )
        {
            STL_TRY( qloClassifyPredExpr( aSQLString,
                                          aIndexHandle,
                                          aInitNode,
                                          aBindContext,
                                          sInitFunction->mArgs[i],
                                          aIndexScanInfo,
                                          aIndexColExprList,
                                          aTableColExprList,
                                          aRegionMem,
                                          aEnv )
                     == STL_SUCCESS );
        }

        sLastColIdx = aIndexScanInfo->mReadKeyCnt;
        STL_THROW( RAMP_FINISH );
    }
    else
    {
        /**
         * Expression에 포함된 Index 및 Table Column List 구성
         */

        sHasRowIdCol     = STL_FALSE;
        sIsIncludeKeyCol = STL_FALSE;
        sIsRange         = STL_TRUE;
        sIsTableFilter   = STL_FALSE;
        sIsLogicalFilter = STL_FALSE;
        sIsSameColumn    = STL_FALSE;

        aIndexColExprList->mCount = 0;
        aIndexColExprList->mHead  = NULL;
        aIndexColExprList->mTail  = NULL;
        
        aTableColExprList->mCount = 0;
        aTableColExprList->mHead  = NULL;
        aTableColExprList->mTail  = NULL;
        
        for( sLoop = 0 ; sLoop < sInitFunction->mArgCount ; sLoop++ )
        {
            switch( sInitFunction->mArgs[ sLoop ]->mType )
            {
                case QLV_EXPR_TYPE_FUNCTION :
                    {
                        if( sInitFunction->mArgs[ sLoop ]->mExpr.mFunction->mFuncId ==
                            KNL_BUILTIN_FUNC_LIKE_PATTERN )
                        {
                            if( aIndexColExprList->mCount == 1 )
                            {
                                sTableColExprList.mCount = 0;
                                sTableColExprList.mHead  = NULL;
                                sTableColExprList.mTail  = NULL;

                                sIndexColExprList.mCount = 0;
                                sIndexColExprList.mHead  = NULL;
                                sIndexColExprList.mTail  = NULL;

                                /* argument list 내의 참조 Column List 구성 */
                                STL_TRY( qloGetRefColListForFilter( aIndexHandle,
                                                                    aInitNode,
                                                                    sInitFunction->mArgs[ sLoop ],
                                                                    & sIndexColExprList,
                                                                    & sTableColExprList,
                                                                    & sHasRowIdCol,
                                                                    aRegionMem,
                                                                    aEnv )
                                         == STL_SUCCESS );

                                if( sTableColExprList.mCount > 0 )
                                {
                                    sIsRange = STL_FALSE;
                                    sIsLogicalFilter = STL_TRUE;
                                    sIsTableFilter = STL_TRUE;
                                }
                                else
                                {
                                    if( sIndexColExprList.mCount > 0 )
                                    {
                                        sIsRange = STL_FALSE;
                                        sIsLogicalFilter = STL_TRUE;
                                    }
                                }
                                break;
                            }
                        }
                    }
                case QLV_EXPR_TYPE_CAST :
                case QLV_EXPR_TYPE_CASE_EXPR :
                case QLV_EXPR_TYPE_LIST_FUNCTION :
                    {
                        sTableColExprList.mCount = 0;
                        sTableColExprList.mHead  = NULL;
                        sTableColExprList.mTail  = NULL;
                        
                        STL_TRY( qloGetRefColListForFilter( aIndexHandle,
                                                            aInitNode,
                                                            sInitFunction->mArgs[ sLoop ],
                                                            aIndexColExprList,
                                                            & sTableColExprList,
                                                            & sHasRowIdCol,
                                                            aRegionMem,
                                                            aEnv )
                                 == STL_SUCCESS );

                        if( sTableColExprList.mCount > 0 )
                        {
                            sIsRange = STL_FALSE;
                            sIsLogicalFilter = STL_TRUE;
                            sIsTableFilter = STL_TRUE;
                        }
                        else if( aIndexColExprList->mCount > 1 )
                        {
                            sIsRange = STL_FALSE;
                            sIsLogicalFilter = STL_TRUE;
                        }
                        else
                        {
                            sIndexColExprList.mCount = 0;
                            sIndexColExprList.mHead  = NULL;
                            sIndexColExprList.mTail  = NULL;

                            /* argument list 내의 참조 Column List 구성 */
                            STL_TRY( qloGetRefColListForFilter( aIndexHandle,
                                                                aInitNode,
                                                                sInitFunction->mArgs[ sLoop ],
                                                                & sIndexColExprList,
                                                                aTableColExprList,
                                                                & sHasRowIdCol,
                                                                aRegionMem,
                                                                aEnv )
                                     == STL_SUCCESS );

                            if( sIndexColExprList.mCount > 0 )
                            {
                                sIsRange = STL_FALSE;
                                sIsLogicalFilter = STL_TRUE;
                            }
                        }
                        break;
                    }
                case QLV_EXPR_TYPE_COLUMN :
                case QLV_EXPR_TYPE_INNER_COLUMN :
                    {
                        /* 참조 Column이 존재 */
                        if( aIndexColExprList->mCount > 0 )
                        {
                            STL_TRY( qloGetRefColListForFilter( aIndexHandle,
                                                                aInitNode,
                                                                sInitFunction->mArgs[ sLoop ],
                                                                aIndexColExprList,
                                                                aTableColExprList,
                                                                & sHasRowIdCol,
                                                                aRegionMem,
                                                                aEnv )
                                     == STL_SUCCESS );

                            sIsSameColumn = ( ( aIndexColExprList->mCount == 1 ) &&
                                              ( aTableColExprList->mCount == 0 ) );
                        }
                        else
                        {
                            STL_TRY( qloGetRefColListForFilter( aIndexHandle,
                                                                aInitNode,
                                                                sInitFunction->mArgs[ sLoop ],
                                                                aIndexColExprList,
                                                                aTableColExprList,
                                                                & sHasRowIdCol,
                                                                aRegionMem,
                                                                aEnv )
                                     == STL_SUCCESS );

                            sIsIncludeKeyCol = ( aIndexColExprList->mCount == 1 );
                        }
                                
                        sIsTableFilter = ( aTableColExprList->mCount > 0 );
                        break;
                    }
                case QLV_EXPR_TYPE_ROWID_COLUMN :
                    {
                        sIsLogicalFilter = STL_TRUE;
                        break;
                    }
                default :
                    {
                        /* QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT */

                        /**
                         * @todo 최종적으로는 Constant Expression만 와야 한다.
                         */
                        
                        /* Do Nothing */
                        break;
                    }
            }

            if( sIsTableFilter == STL_TRUE )
            {
                sIsRange = STL_FALSE;
            }

            if( sHasRowIdCol == STL_TRUE )
            {
                sIsRange = STL_FALSE;
                sIsLogicalFilter = STL_TRUE;
            }

            if( ( sIsLogicalFilter == STL_TRUE ) && ( sIsTableFilter == STL_TRUE ) )
            {
                /* logical table filter expression 리스트에 추가 */
                ADD_LOGICAL_TABLE_FILTER( sInitExprCode );
                STL_THROW( RAMP_FINISH );
            }
            else
            {
                /* Do Nothing */
            }
        }
        
        
        /**
         * 항상 동일한 값을 반환하는 Expr인지 확인
         */

        if( sIsIncludeKeyCol == STL_FALSE )
        {
            sIsRange = STL_FALSE;
        }

        if( ( aIndexColExprList->mCount == 0 ) &&
            ( aTableColExprList->mCount == 0 ) )
        {
            sIsLogicalFilter = STL_TRUE;
            sIsTableFilter = STL_FALSE;
        }
        
        if( ( sIsRange == STL_TRUE ) && ( sIsSameColumn == STL_TRUE ) )
        {
            switch( sInitFunction->mFuncId )
            {
                /* Equal 조건을 포함하는 2-operand 비교 연산자 */
                case KNL_BUILTIN_FUNC_IS_EQUAL :
                case KNL_BUILTIN_FUNC_IS_LESS_THAN_EQUAL :
                case KNL_BUILTIN_FUNC_IS_GREATER_THAN_EQUAL :
                    {
                        /**
                         * @todo 값이 NULL인 경우가 있으므로 항상 TRUE 일 수는 없다.
                         * 
                         *       1. NOT NULL constraint가 있으면 TRUE 설정 여부 검사
                         *       2. NOT NULL constraint가 없으면 IS NOT NULL 조건을 Range로 설정
                         */

                        /* MIN-range에 IS_NOT_NULL 조건 추가 */
                        STL_TRY( qlvCopyExpr( sInitExprCode,
                                              & sNewExpr,
                                              aRegionMem,
                                              aEnv )
                                 == STL_SUCCESS );

                        sNewExpr->mExpr.mFunction->mFuncId   = KNL_BUILTIN_FUNC_IS_NOT_NULL;
                        sNewExpr->mExpr.mFunction->mArgCount = 1;

                        GET_INDEX_IDX( aIndexColExprList->mHead->mExprPtr, sColIdx );
                        ADD_MIN_RANGE( sNewExpr, sColIdx );
                        
                        STL_THROW( RAMP_FINISH );
                    }
                    
                /* Equal 조건을 포함하지 않는 2-operand 비교 연산자 */
                case KNL_BUILTIN_FUNC_IS_NOT_EQUAL :
                case KNL_BUILTIN_FUNC_IS_LESS_THAN :
                case KNL_BUILTIN_FUNC_IS_GREATER_THAN :
                    {
                        /* 항상 FALSE인 조건 */
                        aIndexScanInfo->mIsExist    = STL_FALSE;
                        aIndexScanInfo->mEmptyValue = STL_FALSE;
                        STL_THROW( RAMP_FINISH );
                    }
                default :
                    {
                        break;
                    }
            }
        }
        else
        {
            /* Do Nothing */
        }


        /**
         * Table Filter 구성
         */
        
        if( sIsTableFilter == STL_TRUE )
        {
            STL_DASSERT( sIsRange == STL_FALSE );
            STL_DASSERT( sIsLogicalFilter == STL_FALSE );
            
            /* Physical vs Logical Filter 구분 */
            switch( sInitFunction->mFuncId )
            {
                /* 1-operand 비교 연산자 */
                case KNL_BUILTIN_FUNC_IS_NULL :
                case KNL_BUILTIN_FUNC_IS_NOT_NULL :
                    {
                        /* physical table filter expression 리스트에 추가 */
                        ADD_PHYSICAL_TABLE_FILTER( sInitExprCode );
                        break;
                    }
                    /* 2-operand 비교 연산자 */
                case KNL_BUILTIN_FUNC_IS_EQUAL :
                case KNL_BUILTIN_FUNC_IS_NOT_EQUAL :
                case KNL_BUILTIN_FUNC_IS_LESS_THAN :
                case KNL_BUILTIN_FUNC_IS_LESS_THAN_EQUAL :
                case KNL_BUILTIN_FUNC_IS_GREATER_THAN :
                case KNL_BUILTIN_FUNC_IS_GREATER_THAN_EQUAL :
                    {
                        /**
                         * Physical & Logical Filter 구분
                         */
                    
                        STL_TRY( qloGetDataType( aSQLString,
                                                 sInitFunction->mArgs[0],
                                                 aBindContext,
                                                 & sDataType1,
                                                 aRegionMem,
                                                 aEnv )
                                 == STL_SUCCESS );

                        STL_TRY( qloGetDataType( aSQLString,
                                                 sInitFunction->mArgs[1],
                                                 aBindContext,
                                                 & sDataType2,
                                                 aRegionMem,
                                                 aEnv )
                                 == STL_SUCCESS );

                        if( ( sInitFunction->mArgs[0]->mType == QLV_EXPR_TYPE_COLUMN ) ||
                            ( sInitFunction->mArgs[0]->mType == QLV_EXPR_TYPE_INNER_COLUMN ) )
                        {
                            if( ( sInitFunction->mArgs[1]->mType == QLV_EXPR_TYPE_COLUMN ) ||
                                ( sInitFunction->mArgs[1]->mType == QLV_EXPR_TYPE_INNER_COLUMN ) )
                            {
                                STL_TRY( qlnoGetCompareType( aSQLString,
                                                             sDataType1,
                                                             sDataType2,
                                                             STL_FALSE,
                                                             STL_FALSE,
                                                             sInitExprCode->mPosition,
                                                             &sCompareType,
                                                             aEnv )
                                         == STL_SUCCESS );

                                if( ( sCompareType->mLeftType == sDataType1 ) &&
                                    ( sCompareType->mRightType == sDataType2 ) )
                                {
                                    /* physical table filter expression 리스트에 추가 */
                                    ADD_PHYSICAL_TABLE_FILTER( sInitExprCode );
                                }
                                else
                                {
                                    /* logical table filter expression 리스트에 추가 */
                                    ADD_LOGICAL_TABLE_FILTER( sInitExprCode );
                                }
                            }
                            else
                            {
                                STL_TRY( qlnoGetCompareType( aSQLString,
                                                             sDataType1,
                                                             sDataType2,
                                                             STL_FALSE,
                                                             STL_TRUE,
                                                             sInitExprCode->mPosition,
                                                             &sCompareType,
                                                             aEnv )
                                         == STL_SUCCESS );
                                    
                                if( sCompareType->mLeftType == sDataType1 )
                                {
                                    /* physical table filter expression 리스트에 추가 */
                                    ADD_PHYSICAL_TABLE_FILTER( sInitExprCode );
                                }
                                else
                                {
                                    /* logical table filter expression 리스트에 추가 */
                                    ADD_LOGICAL_TABLE_FILTER( sInitExprCode );
                                }
                            }
                        }
                        else
                        {
                            STL_DASSERT( ( sInitFunction->mArgs[1]->mType == QLV_EXPR_TYPE_COLUMN ) ||
                                         ( sInitFunction->mArgs[1]->mType == QLV_EXPR_TYPE_INNER_COLUMN ) );

                            STL_TRY( qlnoGetCompareType( aSQLString,
                                                         sDataType1,
                                                         sDataType2,
                                                         STL_TRUE,
                                                         STL_FALSE,
                                                         sInitExprCode->mPosition,
                                                         &sCompareType,
                                                         aEnv )
                                     == STL_SUCCESS );
                                    
                            if( sCompareType->mRightType == sDataType2 )
                            {
                                /* physical table filter expression 리스트에 추가 */
                                ADD_PHYSICAL_TABLE_FILTER( sInitExprCode );
                            }
                            else
                            {
                                /* logical table filter expression 리스트에 추가 */
                                ADD_LOGICAL_TABLE_FILTER( sInitExprCode );
                            }
                        }
                                
                        break;
                    }
                    /* 1-operand 만 평가하는 연산자 */
                case KNL_BUILTIN_FUNC_NOT :
                case KNL_BUILTIN_FUNC_IS_UNKNOWN :
                case KNL_BUILTIN_FUNC_IS_NOT_UNKNOWN :
                case KNL_BUILTIN_FUNC_IS :
                case KNL_BUILTIN_FUNC_IS_NOT :
                    {
                        /**
                         * Physical & Logical Filter 구분
                         */
                    
                        STL_DASSERT( ( sInitFunction->mArgs[0]->mType == QLV_EXPR_TYPE_COLUMN ) ||
                                     ( sInitFunction->mArgs[0]->mType == QLV_EXPR_TYPE_INNER_COLUMN ) );
                        
                        STL_TRY( qloGetDataType( aSQLString,
                                                 sInitFunction->mArgs[0],
                                                 aBindContext,
                                                 & sDataType1,
                                                 aRegionMem,
                                                 aEnv )
                                 == STL_SUCCESS );
                                
                        if( sDataType1 == DTL_TYPE_BOOLEAN )
                        {
                            /* physical table filter expression 리스트에 추가 */
                            ADD_PHYSICAL_TABLE_FILTER( sInitExprCode );
                        }
                        else
                        {
                            /* logical table filter expression 리스트에 추가 */
                            ADD_LOGICAL_TABLE_FILTER( sInitExprCode );
                        }

                        break;
                    }
                case KNL_BUILTIN_FUNC_LIKE :
                default :
                    {
                        /* logical table filter expression 리스트에 추가 */
                        ADD_LOGICAL_TABLE_FILTER( sInitExprCode );
                        break;
                    }
            }

            STL_THROW( RAMP_FINISH );
        }
        else
        {
            if( sIsRange == STL_FALSE )
            {
                /* Logical Key Filter 인지 구분 */
                STL_DASSERT( sIsLogicalFilter == STL_TRUE );

                ADD_LOGICAL_KEY_FILTER( sInitExprCode );

                STL_THROW( RAMP_FINISH );
            }
        }
        

        /**
         * FUNCTION 종류별 Range & Key Filter 구성
         */
        
        switch( sInitFunction->mFuncId )
        {
            /* 1-operand 비교 연산자 */
            case KNL_BUILTIN_FUNC_IS_NULL :
            case KNL_BUILTIN_FUNC_IS_NOT_NULL :
                {   
                    /**
                     * Range 추가
                     */

                    /* MIN-range 추가 */
                    GET_INDEX_IDX( aIndexColExprList->mHead->mExprPtr, sColIdx );
                    ADD_MIN_RANGE( sInitExprCode, sColIdx );

                    /*
                     * min-range에만 expression 추가하고,
                     * range 추출 단계(qloExtractRange)에서
                     * min/max range 및 key filter를 재조정함
                     */
                    
                    break;
                }
            case KNL_BUILTIN_FUNC_IS_UNKNOWN :
            case KNL_BUILTIN_FUNC_IS_NOT_UNKNOWN :
                {
                    /**
                     * Range & Key-Filter 구분
                     */
                    
                    STL_TRY( qloGetDataType( aSQLString,
                                             sInitFunction->mArgs[0],
                                             aBindContext,
                                             & sDataType1,
                                             aRegionMem,
                                             aEnv )
                             == STL_SUCCESS );

                    if( sDataType1 == DTL_TYPE_BOOLEAN )
                    {
                        /* MIN-range 추가 */
                        GET_INDEX_IDX( aIndexColExprList->mHead->mExprPtr, sColIdx );
                        ADD_MIN_RANGE( sInitExprCode, sColIdx );
                    }
                    else
                    {
                        /* logical table filter expression 리스트에 추가 */
                        ADD_LOGICAL_KEY_FILTER( sInitExprCode );
                    }
                    break;
                }

            /* 2-operand 비교 연산자 */
            case KNL_BUILTIN_FUNC_IS_EQUAL :
            case KNL_BUILTIN_FUNC_IS_NOT_EQUAL :
            case KNL_BUILTIN_FUNC_IS_LESS_THAN :
            case KNL_BUILTIN_FUNC_IS_LESS_THAN_EQUAL :
            case KNL_BUILTIN_FUNC_IS_GREATER_THAN :
            case KNL_BUILTIN_FUNC_IS_GREATER_THAN_EQUAL :
                {
                    /**
                     * Range & Key-Filter 구분
                     */
                    
                    STL_TRY( qloGetDataType( aSQLString,
                                             sInitFunction->mArgs[0],
                                             aBindContext,
                                             & sDataType1,
                                             aRegionMem,
                                             aEnv )
                             == STL_SUCCESS );

                    STL_TRY( qloGetDataType( aSQLString,
                                             sInitFunction->mArgs[1],
                                             aBindContext,
                                             & sDataType2,
                                             aRegionMem,
                                             aEnv )
                             == STL_SUCCESS );

                    if( aIndexColExprList->mCount == 1 )
                    {
                        /* column의 원본 type과 range 후보 값의 type 간의 compare 연산 유무 체크 */
                        if( ( sInitFunction->mArgs[0]->mType == QLV_EXPR_TYPE_COLUMN ) ||
                            ( sInitFunction->mArgs[0]->mType == QLV_EXPR_TYPE_INNER_COLUMN ) )
                        {
                            STL_TRY( qlnoGetCompareType( aSQLString,
                                                         sDataType1,
                                                         sDataType2,
                                                         STL_FALSE,
                                                         STL_TRUE,
                                                         sInitExprCode->mPosition,
                                                         &sCompareType,
                                                         aEnv )
                                     == STL_SUCCESS );

                            if( sCompareType->mLeftType == sDataType1 )
                            {
                                sHasRangeFunc =
                                    ( dtlPhysicalCompareFuncList[ sCompareType->mLeftType ][ sCompareType->mRightType ] != NULL );

                                STL_DASSERT( sHasRangeFunc == STL_TRUE );
                            }
                            else
                            {
                                sHasRangeFunc = STL_FALSE;
                                sDataType2 = sCompareType->mRightType;
                            }
                        }
                        else
                        {
                            STL_TRY( qlnoGetCompareType( aSQLString,
                                                         sDataType1,
                                                         sDataType2,
                                                         STL_TRUE,
                                                         STL_FALSE,
                                                         sInitExprCode->mPosition,
                                                         &sCompareType,
                                                         aEnv )
                                     == STL_SUCCESS );

                            if( sCompareType->mRightType == sDataType2 )
                            {
                                sHasRangeFunc =
                                    ( dtlPhysicalCompareFuncList[ sCompareType->mLeftType ][ sCompareType->mRightType ] != NULL );
                                
                                STL_DASSERT( sHasRangeFunc == STL_TRUE );
                            }
                            else
                            {
                                sHasRangeFunc = STL_FALSE;
                                sDataType1 = sCompareType->mLeftType;
                            }
                        }
                    }
                    else
                    {
                        /* 예) C1 = C2 */
                        sHasRangeFunc = STL_FALSE;
                    }
                    
                    
                    /**
                     * Range & Key-Filter List 구성
                     */

                    if( sHasRangeFunc == STL_FALSE )
                    {
                        /**
                         * Key Filter 추가
                         */

                        qloGetPhysicalFuncPtr( sInitFunction->mFuncId,
                                               sDataType1,
                                               sDataType2,
                                               & sPhysicalFuncPtr,
                                               STL_FALSE );

                        if( sPhysicalFuncPtr == NULL )
                        {
                            ADD_LOGICAL_KEY_FILTER( sInitExprCode );
                        }
                        else
                        {
                            ADD_PHYSICAL_KEY_FILTER( sInitExprCode );
                        }
                    }
                    else
                    {
                        /**
                         * Range 추가
                         */
                            
                        /* column comp value 순으로 위치하도록 comparison oper 변경 */
                        if( ( sInitFunction->mArgs[1]->mType == QLV_EXPR_TYPE_COLUMN ) ||
                            ( sInitFunction->mArgs[1]->mType == QLV_EXPR_TYPE_INNER_COLUMN ) )
                        {
                            /* Copy 하여 반대되는 Function ID로 변경 및
                             * Index를 사용하는 Column이 Function의 0번째 Args로 오도록 함 */
                            switch( sInitFunction->mFuncId )
                            {
                                case KNL_BUILTIN_FUNC_IS_EQUAL :
                                    {
                                        STL_TRY( qlvCopyExpr( sInitExprCode,
                                                              & sNewExpr,
                                                              aRegionMem,
                                                              aEnv )
                                                 == STL_SUCCESS );

                                        sTmpExpr = sNewExpr->mExpr.mFunction->mArgs[0];
                                        sNewExpr->mExpr.mFunction->mArgs[0] =
                                            sNewExpr->mExpr.mFunction->mArgs[1];
                                        sNewExpr->mExpr.mFunction->mArgs[1] = sTmpExpr;

                                        break;
                                    }
                                case KNL_BUILTIN_FUNC_IS_LESS_THAN :
                                    {
                                        STL_TRY( qlvCopyExpr( sInitExprCode,
                                                              & sNewExpr,
                                                              aRegionMem,
                                                              aEnv )
                                                 == STL_SUCCESS );

                                        sNewExpr->mExpr.mFunction->mFuncId =
                                            KNL_BUILTIN_FUNC_IS_GREATER_THAN;

                                        sTmpExpr = sNewExpr->mExpr.mFunction->mArgs[0];
                                        sNewExpr->mExpr.mFunction->mArgs[0] =
                                            sNewExpr->mExpr.mFunction->mArgs[1];
                                        sNewExpr->mExpr.mFunction->mArgs[1] = sTmpExpr;

                                        break;
                                    }
                                case KNL_BUILTIN_FUNC_IS_LESS_THAN_EQUAL :
                                    {
                                        STL_TRY( qlvCopyExpr( sInitExprCode,
                                                              & sNewExpr,
                                                              aRegionMem,
                                                              aEnv )
                                                 == STL_SUCCESS );

                                        sNewExpr->mExpr.mFunction->mFuncId =
                                            KNL_BUILTIN_FUNC_IS_GREATER_THAN_EQUAL;

                                        sTmpExpr = sNewExpr->mExpr.mFunction->mArgs[0];
                                        sNewExpr->mExpr.mFunction->mArgs[0] =
                                            sNewExpr->mExpr.mFunction->mArgs[1];
                                        sNewExpr->mExpr.mFunction->mArgs[1] = sTmpExpr;

                                        break;
                                    }
                                case KNL_BUILTIN_FUNC_IS_GREATER_THAN :
                                    {
                                        STL_TRY( qlvCopyExpr( sInitExprCode,
                                                              & sNewExpr,
                                                              aRegionMem,
                                                              aEnv )
                                                 == STL_SUCCESS );

                                        sNewExpr->mExpr.mFunction->mFuncId =
                                            KNL_BUILTIN_FUNC_IS_LESS_THAN;

                                        sTmpExpr = sNewExpr->mExpr.mFunction->mArgs[0];
                                        sNewExpr->mExpr.mFunction->mArgs[0] =
                                            sNewExpr->mExpr.mFunction->mArgs[1];
                                        sNewExpr->mExpr.mFunction->mArgs[1] = sTmpExpr;

                                        break;
                                    }
                                case KNL_BUILTIN_FUNC_IS_GREATER_THAN_EQUAL :
                                    {
                                        STL_TRY( qlvCopyExpr( sInitExprCode,
                                                              & sNewExpr,
                                                              aRegionMem,
                                                              aEnv )
                                                 == STL_SUCCESS );

                                        sNewExpr->mExpr.mFunction->mFuncId =
                                            KNL_BUILTIN_FUNC_IS_LESS_THAN_EQUAL;

                                        sTmpExpr = sNewExpr->mExpr.mFunction->mArgs[0];
                                        sNewExpr->mExpr.mFunction->mArgs[0] =
                                            sNewExpr->mExpr.mFunction->mArgs[1];
                                        sNewExpr->mExpr.mFunction->mArgs[1] = sTmpExpr;

                                        break;
                                    }
                                default :
                                    {
                                        sNewExpr = sInitExprCode;
                                        break;
                                    }
                            }

                            sInitExprCode = sNewExpr;
                        }
                        else
                        {
                            /* Do Nothing */
                        }

                        /* MIN-range 추가 */
                        GET_INDEX_IDX( aIndexColExprList->mHead->mExprPtr, sColIdx );
                        ADD_MIN_RANGE( sInitExprCode, sColIdx );

                        /*
                         * min-range에만 expression 추가하고,
                         * range 추출 단계(qloExtractRange)에서
                         * min/max range 및 key filter를 재조정함
                         */
                    }
                    
                    break;
                }
            case KNL_BUILTIN_FUNC_LIKE :
                {
                    /**
                     * Range & Key-Filter 구분
                     */
                    
                    STL_TRY( qloGetDataType( aSQLString,
                                             sInitFunction->mArgs[0],
                                             aBindContext,
                                             & sDataType1,
                                             aRegionMem,
                                             aEnv )
                             == STL_SUCCESS );

                    STL_TRY( qloGetDataType( aSQLString,
                                             sInitFunction->mArgs[1],
                                             aBindContext,
                                             & sDataType2,
                                             aRegionMem,
                                             aEnv )
                             == STL_SUCCESS );

                    STL_DASSERT( aIndexColExprList->mCount == 1 );
                    
                    /* column의 원본 type과 range 후보 값의 type 간의 compare 연산 유무 체크 */
                    STL_DASSERT( ( sInitFunction->mArgs[0]->mType == QLV_EXPR_TYPE_COLUMN ) ||
                                 ( sInitFunction->mArgs[0]->mType == QLV_EXPR_TYPE_INNER_COLUMN ) );
                    
                    if( ( dtlDataTypeGroup[ sDataType1 ] == DTL_GROUP_CHAR_STRING ) || 	 	 
                        ( dtlDataTypeGroup[ sDataType1 ] == DTL_GROUP_LONGVARCHAR_STRING ) ) 	 	 
                    { 	 	 
                        sHasRangeFunc = STL_TRUE;
                    } 	 	 
                    else 	 	 
                    { 	 	 
                        sHasRangeFunc = STL_FALSE; 	 	 
                    } 

                    
                    /**
                     * Key Filter 추가
                     */

                    ADD_LOGICAL_KEY_FILTER( sInitExprCode );

                    
                    /**
                     * Range & Key-Filter List 구성
                     */
                    
                    /**
                     * Range 추가
                     */

                    if( sHasRangeFunc == STL_TRUE )
                    {
                        /* MIN-range 추가 */
                        GET_INDEX_IDX( aIndexColExprList->mHead->mExprPtr, sColIdx );
                        ADD_MIN_RANGE( sInitExprCode, sColIdx );
                    }
                    else
                    {
                        /* Do Nothing */
                    }

                    /*
                     * min-range에만 expression 추가하고,
                     * range 추출 단계(qloExtractRange)에서
                     * min/max range 및 key filter를 재조정함
                     */
                    
                    break;
                }

            case KNL_BUILTIN_FUNC_IS :
                {
                    /**
                     * Key Physical Filter와 Table Physical Filter 구분
                     */
                    
                    STL_DASSERT( ( sInitFunction->mArgs[0]->mType == QLV_EXPR_TYPE_COLUMN ) ||
                                 ( sInitFunction->mArgs[0]->mType == QLV_EXPR_TYPE_INNER_COLUMN ) );
                    
                    STL_TRY( qloGetDataType( aSQLString,
                                             sInitFunction->mArgs[0],
                                             aBindContext,
                                             & sDataType1,
                                             aRegionMem,
                                             aEnv )
                             == STL_SUCCESS );

                    if( sDataType1 == DTL_TYPE_BOOLEAN )
                    {
                        /* MIN-range 추가 */
                        GET_INDEX_IDX( aIndexColExprList->mHead->mExprPtr, sColIdx );
                        ADD_MIN_RANGE( sInitExprCode, sColIdx );
                    }
                    else
                    {
                        ADD_LOGICAL_KEY_FILTER( sInitExprCode );                            
                    }
                                                
                    break;
                }

            case KNL_BUILTIN_FUNC_NOT :
            case KNL_BUILTIN_FUNC_IS_NOT :
                {
                    /**
                     * Key Physical Filter와 Table Physical Filter 구분
                     */
                    
                    STL_DASSERT( ( sInitFunction->mArgs[0]->mType == QLV_EXPR_TYPE_COLUMN ) ||
                                 ( sInitFunction->mArgs[0]->mType == QLV_EXPR_TYPE_INNER_COLUMN ) );
                    
                    STL_TRY( qloGetDataType( aSQLString,
                                             sInitFunction->mArgs[0],
                                             aBindContext,
                                             & sDataType1,
                                             aRegionMem,
                                             aEnv )
                             == STL_SUCCESS );

                    if( sDataType1 == DTL_TYPE_BOOLEAN )
                    {
                        ADD_PHYSICAL_KEY_FILTER( sInitExprCode );
                    }
                    else
                    {
                        ADD_LOGICAL_KEY_FILTER( sInitExprCode );                            
                    }
                                                
                    break;
                }

            default :
                {
                    /**
                     * Key Filter 추가
                     */
                    
                    ADD_LOGICAL_KEY_FILTER( sInitExprCode );
                    break;
                }
        }
    }

    
    STL_RAMP( RAMP_FINISH );
    
    
    /**
     * OUTPUT 설정
     */
    
    aIndexScanInfo->mReadKeyCnt = sLastColIdx;

    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief range/filter 대상으로 구분하기
 * @param[in]       aIndexHandle     Index Handle
 * @param[in]       aInitNode        Init Node
 * @param[in,out]   aIndexScanInfo   Range/Filter Expression 분류 정보
 * @param[in]       aRegionMem       Region Memory
 * @param[in]       aEnv             Environment
 *
 * @remark index가 선택되었음을 가정
 */
stlStatus qloExtractRange( ellDictHandle       * aIndexHandle,
                           qlvInitNode         * aInitNode,
                           qloIndexScanInfo    * aIndexScanInfo,
                           knlRegionMem        * aRegionMem,
                           qllEnv              * aEnv )
{
    qlvRefExprList       sRangeExprList;
    qlvRefExprList     * sExprList             = NULL;
    qlvRefExprList     * sMinRangeExprList     = NULL;
    qlvRefExprList     * sMaxRangeExprList     = NULL;
    qlvRefExprList     * sPhyKeyFilterExprList = NULL;

    qlvRefExprItem     * sCurRangeListItem     = NULL;
    qlvRefExprItem     * sPrevRangeListItem    = NULL;

    qlvInitExpression  * sRangeExpr            = NULL;
    qlvInitExpression  * sNewExpr              = NULL;
    qlvInitFunction    * sFuncCode             = NULL;
    ellIndexKeyDesc    * sIndexKeyDesc         = NULL;
    
    stlInt32             sKeyColCnt            = 0;
    stlInt32             sLoop                 = 0;
    stlInt32             sLastColIdx           = 0;

    stlBool              sIsExistFixedPred     = STL_FALSE;
    stlBool              sExistIsNull          = STL_FALSE;
    stlBool              sExistIsNotNull       = STL_FALSE;
    stlBool              sIsAsc                = STL_FALSE;
    stlBool              sIsNullsFirst         = STL_FALSE;

    stlUInt8           * sKeyFlags             = NULL;

#define ADD_EXPR( aExpr, aList )                        \
    {                                                   \
        STL_TRY( qlvAddExprToRefExprList( (aList),      \
                                          (aExpr),      \
                                          STL_FALSE,    \
                                          aRegionMem,   \
                                          aEnv )        \
                 == STL_SUCCESS );                      \
    }

#define REMOVE_LIST_ITEM( aCurItem, aPrevItem, aList )  \
    {                                                   \
        if( (aCurItem) == (aList)->mHead )              \
        {                                               \
            (aList)->mHead = (aCurItem)->mNext;         \
        }                                               \
        if( (aCurItem) == (aList)->mTail )              \
        {                                               \
            (aList)->mTail = (aPrevItem);               \
        }                                               \
        if( (aPrevItem) != NULL )                       \
        {                                               \
            (aPrevItem)->mNext = (aCurItem)->mNext;     \
        }                                               \
        (aList)->mCount--;                              \
        (aCurItem)->mNext = NULL;                       \
    }
    
        
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aIndexScanInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    
    
    /**
     * range & key-filter 추출
     */

    sKeyColCnt            = aIndexScanInfo->mReadKeyCnt;
    sIsExistFixedPred     = STL_TRUE;

    if( aInitNode->mType == QLV_NODE_TYPE_BASE_TABLE )
    {
        sIndexKeyDesc = ellGetIndexKeyDesc( aIndexHandle );
    }
    else
    {
        STL_DASSERT( ( aInitNode->mType == QLV_NODE_TYPE_SORT_INSTANT ) ||
                     ( aInitNode->mType == QLV_NODE_TYPE_GROUP_SORT_INSTANT ) ||
                     ( aInitNode->mType == QLV_NODE_TYPE_SORT_JOIN_INSTANT ) );
        
        sKeyFlags = ((qlvInitInstantNode *) aInitNode)->mKeyFlags;
    }
    
    sPhyKeyFilterExprList = aIndexScanInfo->mPhysicalKeyFilterExprList;

    for( sLoop = 0 ; sLoop < sKeyColCnt ; sLoop++ )
    {
        /* 분석할 range 정보 설정 */
        stlMemcpy( & sRangeExprList,
                   aIndexScanInfo->mMinRangeExpr[ sLoop ],
                   STL_SIZEOF( qlvRefExprList ) );

        sMinRangeExprList = aIndexScanInfo->mMinRangeExpr[ sLoop ];
        sMaxRangeExprList = aIndexScanInfo->mMaxRangeExpr[ sLoop ];

        sMinRangeExprList->mCount = 0;
        sMinRangeExprList->mHead  = NULL;
        sMinRangeExprList->mTail  = NULL;
        
        sMaxRangeExprList->mCount = 0;
        sMaxRangeExprList->mHead  = NULL;
        sMaxRangeExprList->mTail  = NULL;

        if( sRangeExprList.mCount == 0 )
        {
            sIsExistFixedPred = STL_FALSE;
        }
        else
        {
            /* Do Nothing */
        }

        /* range 분석 가능 여부 판단 */ 
        if( sIsExistFixedPred == STL_TRUE )
        {
            sLastColIdx = sLoop + 1;
        }
        else
        {
            /**
             * 더 이상 Range를 만들 수 없는 경우
             */

            if( sRangeExprList.mCount > 0 )
            {
                /* Like 연산을 List에서 제거 */
                sCurRangeListItem  = sRangeExprList.mHead;
                sPrevRangeListItem = NULL;

                while( sCurRangeListItem != NULL )
                {
                    sRangeExpr = sCurRangeListItem->mExprPtr;

                    STL_PARAM_VALIDATE( sRangeExpr->mType == QLV_EXPR_TYPE_FUNCTION,
                                        QLL_ERROR_STACK( aEnv ) );

                    sFuncCode = sRangeExpr->mExpr.mFunction;
   
                    if( sFuncCode->mFuncId == KNL_BUILTIN_FUNC_LIKE )
                    {
                        if( sPrevRangeListItem == NULL )
                        {
                            sRangeExprList.mHead = sCurRangeListItem->mNext;
                        }
                        else
                        {
                            sPrevRangeListItem->mNext = sCurRangeListItem->mNext;
                        }
                        sRangeExprList.mCount--;
                    }
                    else
                    {
                        sPrevRangeListItem = sCurRangeListItem;
                    }

                    sCurRangeListItem = sCurRangeListItem->mNext;
                }
                
                /* 나머지 부분들을 모두 physical key-filter로 내림 */
                if( sPhyKeyFilterExprList->mCount == 0 )
                {
                    sPhyKeyFilterExprList->mHead  = sRangeExprList.mHead;
                    sPhyKeyFilterExprList->mTail  = sRangeExprList.mTail;
                    sPhyKeyFilterExprList->mCount = sRangeExprList.mCount;
                    
                }
                else
                {
                    sPhyKeyFilterExprList->mTail->mNext = sRangeExprList.mHead;
                    sPhyKeyFilterExprList->mTail        = sRangeExprList.mTail;
                    sPhyKeyFilterExprList->mCount      += sRangeExprList.mCount;
                }
            }
            else
            {
                /* Do Nothing */
            }
            
            continue;
        }


        /**
         * Function 종류에 따라 Range & Filter 분류
         */

        /* IS NULL 과 IS NOT NULL이 같이 있으면 Range는 항상 FALSE 값 리턴 */

        sExistIsNull    = STL_FALSE;
        sExistIsNotNull = STL_FALSE;

        sCurRangeListItem  = sRangeExprList.mHead;
        
        while( sCurRangeListItem != NULL )
        {
            sRangeExpr = sCurRangeListItem->mExprPtr;
            
            STL_PARAM_VALIDATE( sRangeExpr->mType == QLV_EXPR_TYPE_FUNCTION,
                                QLL_ERROR_STACK( aEnv ) );

            sFuncCode = sRangeExpr->mExpr.mFunction;
            
            switch( sFuncCode->mFuncId )
            {
                case KNL_BUILTIN_FUNC_IS_NULL :
                case KNL_BUILTIN_FUNC_IS_UNKNOWN :
                    {
                        if( sExistIsNotNull == STL_TRUE )
                        {
                            aIndexScanInfo->mIsExist    = STL_FALSE;
                            aIndexScanInfo->mEmptyValue = STL_FALSE;
                            
                            STL_THROW( RAMP_CANNOT_MAKE_RANGE );
                        }
                        else
                        {
                            if( sExistIsNull == STL_TRUE )
                            {
                                /* 동일한 조건이 존재 */
                                /* Do Nothing */
                            }
                            else
                            {
                                /* MIN & MAX 동일한 조건 가짐 */

                                /* add to MIN-range list */
                                ADD_EXPR( sRangeExpr, sMinRangeExprList );

                                /* add to MAX-range list */
                                ADD_EXPR( sRangeExpr, sMaxRangeExprList );
                            }

                            /* Fixed Range 존재 */
                            /* Do Nothing */

                            /* IS NULL */
                            sExistIsNull = STL_TRUE;
                        }

                        break;
                    }

                case KNL_BUILTIN_FUNC_IS_EQUAL :
                case KNL_BUILTIN_FUNC_LIKE :
                    {
                        if( sExistIsNull == STL_TRUE )
                        {
                            aIndexScanInfo->mIsExist    = STL_FALSE;
                            aIndexScanInfo->mEmptyValue = STL_FALSE;

                            STL_THROW( RAMP_CANNOT_MAKE_RANGE );
                        }
                        else
                        {
                            /* MIN & MAX 동일한 조건 가짐 */

                            /* add to MIN-range list */
                            ADD_EXPR( sRangeExpr, sMinRangeExprList );

                            /* add to MAX-range list */
                            ADD_EXPR( sRangeExpr, sMaxRangeExprList );
                            
                            /* Fixed Range 존재 */
                            /* Do Nothing */
                        
                            /* IS NOT NULL */
                            sExistIsNotNull = STL_TRUE;
                        }

                        if( sFuncCode->mFuncId == KNL_BUILTIN_FUNC_LIKE )
                        {
                            /* Fixed Range 존재하지 않음 */
                            sIsExistFixedPred = STL_FALSE;
                        }
                        
                        break;
                    }
                    
                case KNL_BUILTIN_FUNC_IS :
                    {
                        if( sExistIsNull == STL_TRUE )
                        {
                            aIndexScanInfo->mIsExist    = STL_FALSE;
                            aIndexScanInfo->mEmptyValue = STL_FALSE;

                            STL_THROW( RAMP_CANNOT_MAKE_RANGE );
                        }
                        else
                        {
                            /* MIN & MAX 동일한 조건 가짐 */
                            STL_TRY( qlvCopyExpr( sRangeExpr,
                                                  & sNewExpr,
                                                  aRegionMem,
                                                  aEnv )
                                     == STL_SUCCESS );

                            sFuncCode = sNewExpr->mExpr.mFunction;
                            sFuncCode->mFuncId = KNL_BUILTIN_FUNC_IS_EQUAL;

                            /* add to MIN-range list */
                            ADD_EXPR( sNewExpr, sMinRangeExprList );

                            /* add to MAX-range list */
                            ADD_EXPR( sNewExpr, sMaxRangeExprList );
                            
                            /* Fixed Range 존재 */
                            /* Do Nothing */
                        
                            /* IS NOT NULL */
                            sExistIsNotNull = STL_TRUE;
                        }
                        
                        break;
                    }

                case KNL_BUILTIN_FUNC_IS_LESS_THAN :
                case KNL_BUILTIN_FUNC_IS_LESS_THAN_EQUAL :
                    {
                        if( sExistIsNull == STL_TRUE )
                        {
                            aIndexScanInfo->mIsExist    = STL_FALSE;
                            aIndexScanInfo->mEmptyValue = STL_FALSE;

                            STL_THROW( RAMP_CANNOT_MAKE_RANGE );
                        }
                        else
                        {
                            /* MIN : IS NOT NULL 추가 */
                            if( sExistIsNotNull == STL_TRUE )
                            {
                                /* 동일한 조건이 존재 */
                                /* Do Nothing */
                            }
                            else
                            {
                                /* MIN-range에 IS_NOT_NULL 조건 추가 */
                                STL_TRY( qlvCopyExpr( sRangeExpr,
                                                      & sNewExpr,
                                                      aRegionMem,
                                                      aEnv )
                                         == STL_SUCCESS );

                                sFuncCode = sNewExpr->mExpr.mFunction;

                                sFuncCode->mFuncId   = KNL_BUILTIN_FUNC_IS_NOT_NULL;
                                sFuncCode->mArgCount = 1;
                                
                                STL_DASSERT( ( sFuncCode->mArgs[0]->mType == QLV_EXPR_TYPE_COLUMN ) ||
                                             ( sFuncCode->mArgs[0]->mType == QLV_EXPR_TYPE_INNER_COLUMN ) );

                                ADD_EXPR( sNewExpr, sMinRangeExprList );
                            }

                            /* MAX 조건 가짐 */
                            ADD_EXPR( sRangeExpr, sMaxRangeExprList );
                            
                            /* Is Not NULL */
                            sExistIsNotNull = STL_TRUE;
                        }

                        /* Fixed Range 존재하지 않음 */
                        sIsExistFixedPred = STL_FALSE;

                        break;
                    }

                case KNL_BUILTIN_FUNC_IS_GREATER_THAN :
                case KNL_BUILTIN_FUNC_IS_GREATER_THAN_EQUAL :
                    {
                        if( sExistIsNull == STL_TRUE )
                        {
                            aIndexScanInfo->mIsExist    = STL_FALSE;
                            aIndexScanInfo->mEmptyValue = STL_FALSE;

                            STL_THROW( RAMP_CANNOT_MAKE_RANGE );
                        }
                        else
                        {
                            /* MIN 조건 가짐 */
                            ADD_EXPR( sRangeExpr, sMinRangeExprList );
                            
                            /* MAX : IS NOT NULL 추가 */
                            if( sExistIsNotNull == STL_TRUE )
                            {
                                /* 동일한 조건이 존재하므로 추가하지 않음 */
                                /* Do Nohting */ 
                            }
                            else
                            {
                                /* MAX-range에 IS_NOT_NULL 조건 추가 */
                                STL_TRY( qlvCopyExpr( sRangeExpr,
                                                      & sNewExpr,
                                                      aRegionMem,
                                                      aEnv )
                                         == STL_SUCCESS );

                                sFuncCode = sNewExpr->mExpr.mFunction;

                                sFuncCode->mFuncId   = KNL_BUILTIN_FUNC_IS_NOT_NULL;
                                sFuncCode->mArgCount = 1;
                                
                                STL_DASSERT( ( sFuncCode->mArgs[0]->mType == QLV_EXPR_TYPE_COLUMN ) ||
                                             ( sFuncCode->mArgs[0]->mType == QLV_EXPR_TYPE_INNER_COLUMN ) );

                                ADD_EXPR( sNewExpr, sMaxRangeExprList );
                            }
                            
                            /* Fixed Range 존재하지 않음 */
                            /* Do Nothing */
                        
                            /* Is Not NULL */
                            sExistIsNotNull = STL_TRUE;
                        }

                        /* Fixed Range 존재하지 않음 */
                        sIsExistFixedPred = STL_FALSE;

                        break;
                    }

                case KNL_BUILTIN_FUNC_IS_NOT_EQUAL :
                    {
                        if( sExistIsNull == STL_TRUE )
                        {
                            aIndexScanInfo->mIsExist    = STL_FALSE;
                            aIndexScanInfo->mEmptyValue = STL_FALSE;

                            STL_THROW( RAMP_CANNOT_MAKE_RANGE );
                        }
                        else
                        {
                            /* MIN & MAX 모두 없음 => Key Filter로 내림 */
                            ADD_EXPR( sRangeExpr, sPhyKeyFilterExprList );

                            if( sExistIsNotNull == STL_TRUE )
                            {
                                /* 동일한 조건이 존재하므로 추가하지 않음 */
                                /* Do Nohting */ 
                            }
                            else
                            {
                                /* MIN & MAX : IS NOT NULL 추가 */

                                /* IS_NOT_NULL 함수로 변경하여 range에 추가 */
                                STL_TRY( qlvCopyExpr( sRangeExpr,
                                                      & sNewExpr,
                                                      aRegionMem,
                                                      aEnv )
                                         == STL_SUCCESS );

                                sFuncCode = sNewExpr->mExpr.mFunction;

                                sFuncCode->mFuncId   = KNL_BUILTIN_FUNC_IS_NOT_NULL;
                                sFuncCode->mArgCount = 1;
                                if( ( sFuncCode->mArgs[0]->mType == QLV_EXPR_TYPE_COLUMN ) ||
                                    ( sFuncCode->mArgs[0]->mType == QLV_EXPR_TYPE_INNER_COLUMN ) )
                                {
                                    /* Do Nothing */
                                }
                                else
                                {
                                    sFuncCode->mArgs[0] = sFuncCode->mArgs[1];
                                }

                                /* add to MIN-range list */
                                ADD_EXPR( sNewExpr, sMinRangeExprList );
                                /* add to MAX-range list */
                                ADD_EXPR( sNewExpr, sMaxRangeExprList );
                            }

                            /* Fixed Range 존재하지 않음 */
                            sIsExistFixedPred = STL_FALSE;

                            /* Is Not NULL */
                            sExistIsNotNull = STL_TRUE;
                        }
                        
                        break;
                    }

                case KNL_BUILTIN_FUNC_IS_NOT_NULL :
                    {
                        if( sExistIsNull == STL_TRUE )
                        {
                            aIndexScanInfo->mIsExist    = STL_FALSE;
                            aIndexScanInfo->mEmptyValue = STL_FALSE;

                            STL_THROW( RAMP_CANNOT_MAKE_RANGE );
                        }
                        else
                        {
                            if( sExistIsNotNull == STL_TRUE )
                            {
                                /* 동일한 조건이 존재하므로 추가하지 않음 */
                                /* Do Nohting */ 
                            }
                            else
                            {
                                /* MIN & MAX 동일한 조건 가짐 */

                                /* add to MIN-range list */
                                ADD_EXPR( sRangeExpr, sMinRangeExprList );
                                /* add to MAX-range list */
                                ADD_EXPR( sRangeExpr, sMaxRangeExprList );
                            }

                            /* Is Not NULL */
                            sExistIsNotNull = STL_TRUE;
                        }
                        
                        /* Fixed Range 존재하지 않음 */
                        sIsExistFixedPred = STL_FALSE;
                        
                        break;
                    }
                    
                case KNL_BUILTIN_FUNC_IS_NOT_UNKNOWN :
                    {
                        if( sExistIsNull == STL_TRUE )
                        {
                            aIndexScanInfo->mIsExist    = STL_FALSE;
                            aIndexScanInfo->mEmptyValue = STL_FALSE;

                            STL_THROW( RAMP_CANNOT_MAKE_RANGE );
                        }
                        else
                        {
                            if( sExistIsNotNull == STL_TRUE )
                            {
                                /* 동일한 조건이 존재하므로 추가하지 않음 */
                                /* Do Nohting */ 
                            }
                            else
                            {
                                /* MIN & MAX 동일한 조건 가짐 */
                                
                                /* IS_NOT_NULL 함수로 변경하여 range에 추가 */
                                STL_TRY( qlvCopyExpr( sRangeExpr,
                                                      & sNewExpr,
                                                      aRegionMem,
                                                      aEnv )
                                         == STL_SUCCESS );

                                sFuncCode = sNewExpr->mExpr.mFunction;
                                sFuncCode->mFuncId = KNL_BUILTIN_FUNC_IS_NOT_NULL;

                                /* add to MIN-range list */
                                ADD_EXPR( sNewExpr, sMinRangeExprList );
                                /* add to MAX-range list */
                                ADD_EXPR( sNewExpr, sMaxRangeExprList );
                            }

                            /* Is Not NULL */
                            sExistIsNotNull = STL_TRUE;
                        }
                        
                        /* Fixed Range 존재하지 않음 */
                        sIsExistFixedPred = STL_FALSE;
                        
                        break;
                    }

                case KNL_BUILTIN_FUNC_NOT :
                    {
                        if( sExistIsNull == STL_TRUE )
                        {
                            aIndexScanInfo->mIsExist    = STL_FALSE;
                            aIndexScanInfo->mEmptyValue = STL_FALSE;

                            STL_THROW( RAMP_CANNOT_MAKE_RANGE );
                        }
                        else
                        {
                            if( sExistIsNotNull == STL_TRUE )
                            {
                                /* 동일한 조건이 존재하므로 추가하지 않음 */
                                /* Do Nohting */ 
                            }
                            else
                            {
                                /* IS_NOT_NULL 함수로 변경하여 range에 추가 */
                                STL_TRY( qlvCopyExpr( sRangeExpr,
                                                      & sNewExpr,
                                                      aRegionMem,
                                                      aEnv )
                                         == STL_SUCCESS );

                                sFuncCode = sNewExpr->mExpr.mFunction;
                                sFuncCode->mFuncId = KNL_BUILTIN_FUNC_IS_NOT_NULL;

                                /* add to MIN-range list */
                                ADD_EXPR( sNewExpr, sMinRangeExprList );

                                /* add to MAX-range list */
                                ADD_EXPR( sNewExpr, sMaxRangeExprList );

                            }

                            /* MIN & MAX 동일한 조건 가짐 */
            
                            /* add to MIN-range list */
                            ADD_EXPR( sRangeExpr, sMinRangeExprList );
                            /* add to MAX-range list */
                            ADD_EXPR( sRangeExpr, sMaxRangeExprList );

                            /* Is Not NULL */
                            sExistIsNotNull = STL_TRUE;
                        }
                        
                        /* Fixed Range 존재하지 않음 */
                        sIsExistFixedPred = STL_FALSE;
                        
                        break;
                    }

                default :
                    {
                        STL_DASSERT( 0 );
                        break;
                    }
            }
            
            sCurRangeListItem  = sCurRangeListItem->mNext;
            
        } /* while */
        
    } /* for */
    

    /**
     * Range 대상인 Last Key Column들의 Min-Range 재조정
     */

    for( sLoop = 0 ; sLoop < sLastColIdx ; sLoop++ )
    {
        if( aInitNode->mType == QLV_NODE_TYPE_BASE_TABLE )
        {
            sIsAsc = ( sIndexKeyDesc[ sLoop ].mKeyOrdering ==
                       ELL_INDEX_COLUMN_ASCENDING );
        
            sIsNullsFirst = ( sIndexKeyDesc[ sLoop ].mKeyNullOrdering ==
                              ELL_INDEX_COLUMN_NULLS_FIRST );
        }
        else
        {
            sIsAsc = ( DTL_GET_KEY_FLAG_SORT_ORDER( sKeyFlags[ sLoop ] ) ==
                       DTL_KEYCOLUMN_INDEX_ORDER_ASC );
        
            sIsNullsFirst = ( DTL_GET_KEY_FLAG_NULL_ORDER( sKeyFlags[ sLoop ] ) ==
                              DTL_KEYCOLUMN_INDEX_NULL_ORDER_FIRST );
        }
        
        sMinRangeExprList = aIndexScanInfo->mMinRangeExpr[ sLoop ];

        STL_DASSERT( sMinRangeExprList->mCount > 0 );

        sPrevRangeListItem = NULL;
        sCurRangeListItem  = sMinRangeExprList->mHead;
        sIsExistFixedPred  = STL_FALSE;
        
        while( sCurRangeListItem != NULL )
        {
            sRangeExpr = sCurRangeListItem->mExprPtr;
            sFuncCode  = sRangeExpr->mExpr.mFunction;
            
            switch( sFuncCode->mFuncId )
            {
                case KNL_BUILTIN_FUNC_IS_EQUAL :
                case KNL_BUILTIN_FUNC_IS_NULL :
                case KNL_BUILTIN_FUNC_IS_UNKNOWN :
                case KNL_BUILTIN_FUNC_IS_GREATER_THAN :
                case KNL_BUILTIN_FUNC_IS_GREATER_THAN_EQUAL :
                case KNL_BUILTIN_FUNC_LIKE :
                case KNL_BUILTIN_FUNC_NOT :
                    {
                        /* 그대로 남겨두기 */
                        break;
                    }
                case KNL_BUILTIN_FUNC_IS_NOT_NULL :
                    {
                        if( sIsAsc == sIsNullsFirst )
                        {
                            if( sCurRangeListItem == sMinRangeExprList->mHead )
                            {
                                /* 그대로 남겨두기 */
                            }
                            else
                            {
                                /* 현재 Item을 List에서 제거 */
                                REMOVE_LIST_ITEM( sCurRangeListItem,
                                                  sPrevRangeListItem,
                                                  sMinRangeExprList );
                            }
                        }
                        else
                        {
                            /* 현재 Item을 List에서 제거 */
                            REMOVE_LIST_ITEM( sCurRangeListItem,
                                              sPrevRangeListItem,
                                              sMinRangeExprList );
                        }
                        break;
                    }
                default :
                    {
                        STL_DASSERT( 0 );
                        break;
                    }
            }

            sPrevRangeListItem = sCurRangeListItem;
            sCurRangeListItem  = sCurRangeListItem->mNext;
            
        } /* while */

    } /* for */
    

    /**
     * Range 대상인 Last Key Column들의 Max-Range 재조정
     */

    for( sLoop = 0 ; sLoop < sLastColIdx ; sLoop++ )
    {
        if( aInitNode->mType == QLV_NODE_TYPE_BASE_TABLE )
        {
            sIsAsc = ( sIndexKeyDesc[ sLoop ].mKeyOrdering ==
                       ELL_INDEX_COLUMN_ASCENDING );
        
            sIsNullsFirst = ( sIndexKeyDesc[ sLoop ].mKeyNullOrdering ==
                              ELL_INDEX_COLUMN_NULLS_FIRST );
        }
        else
        {
            sIsAsc = ( DTL_GET_KEY_FLAG_SORT_ORDER( sKeyFlags[ sLoop ] ) ==
                       DTL_KEYCOLUMN_INDEX_ORDER_ASC );
        
            sIsNullsFirst = ( DTL_GET_KEY_FLAG_NULL_ORDER( sKeyFlags[ sLoop ] ) ==
                              DTL_KEYCOLUMN_INDEX_NULL_ORDER_FIRST );
        }

        sMaxRangeExprList = aIndexScanInfo->mMaxRangeExpr[ sLoop ];

        STL_DASSERT( sMaxRangeExprList->mCount > 0 );

        sPrevRangeListItem = NULL;
        sCurRangeListItem  = sMaxRangeExprList->mHead;
        sIsExistFixedPred  = STL_FALSE;

        while( sCurRangeListItem != NULL )
        {
            sRangeExpr = sCurRangeListItem->mExprPtr;
            sFuncCode  = sRangeExpr->mExpr.mFunction;
            
            switch( sFuncCode->mFuncId )
            {
                case KNL_BUILTIN_FUNC_IS_EQUAL :
                case KNL_BUILTIN_FUNC_IS_NULL :
                case KNL_BUILTIN_FUNC_IS_UNKNOWN :
                case KNL_BUILTIN_FUNC_IS_LESS_THAN :
                case KNL_BUILTIN_FUNC_IS_LESS_THAN_EQUAL :
                case KNL_BUILTIN_FUNC_LIKE :
                case KNL_BUILTIN_FUNC_NOT :
                    {
                        /* 그대로 남겨두기 */
                        break;
                    }
                case KNL_BUILTIN_FUNC_IS_NOT_NULL :
                    {
                        if( sIsAsc != sIsNullsFirst )
                        {
                            if( sCurRangeListItem == sMaxRangeExprList->mHead )
                            {
                                /* 그대로 남겨두기 */
                            }
                            else
                            {
                                /* 현재 Item을 List에서 제거 */
                                REMOVE_LIST_ITEM( sCurRangeListItem,
                                                  sPrevRangeListItem,
                                                  sMaxRangeExprList );
                            }
                        }
                        else
                        {
                            /* 현재 Item을 List에서 제거 */
                            REMOVE_LIST_ITEM( sCurRangeListItem,
                                              sPrevRangeListItem,
                                              sMaxRangeExprList );
                        }
                        break;
                    }
                default :
                    {
                        STL_DASSERT( 0 );
                        break;
                    }
            }
            
            sPrevRangeListItem = sCurRangeListItem;
            sCurRangeListItem  = sCurRangeListItem->mNext;
            
        } /* while */

    } /* for */


    /**
     * sort order에 따라 MIN/MAX 값 치환
     */

    for( sLoop = 0 ; sLoop < sLastColIdx ; sLoop++ )
    {
        if( aInitNode->mType == QLV_NODE_TYPE_BASE_TABLE )
        {
            sIsAsc = ( sIndexKeyDesc[ sLoop ].mKeyOrdering ==
                       ELL_INDEX_COLUMN_ASCENDING );
        }
        else
        {
            sIsAsc = ( DTL_GET_KEY_FLAG_SORT_ORDER( sKeyFlags[ sLoop ] ) ==
                       DTL_KEYCOLUMN_INDEX_ORDER_ASC );
        }
        
        if( sIsAsc == STL_TRUE )
        {
            /* Do Nothing */
        }
        else
        {
            sExprList = aIndexScanInfo->mMinRangeExpr[ sLoop ];
            aIndexScanInfo->mMinRangeExpr[ sLoop ] = aIndexScanInfo->mMaxRangeExpr[ sLoop ];
            aIndexScanInfo->mMaxRangeExpr[ sLoop ] = sExprList;
        }
    }

    aIndexScanInfo->mKeyColCount = sLastColIdx;
    
    STL_RAMP( RAMP_CANNOT_MAKE_RANGE );

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief AND 연산을 이용하여 Expression List의 Expression들을 연결
 * @param[out]    aExpr            Expression
 * @param[in]     aExprList        Expression List
 * @param[in]     aRegionMem       Region Memory
 * @param[in]     aEnv             Environment
 */
stlStatus qloLinkRangeExpr( qlvInitExpression  ** aExpr,
                            qlvRefExprList      * aExprList,
                            knlRegionMem        * aRegionMem,
                            qllEnv              * aEnv )
{
    qlvInitExpression  * sResultExpr = NULL;
    qlvInitExpression ** sArgArr     = NULL;

    qlvRefExprItem     * sCurItem    = NULL;
    qlvRefExprItem     * sNextItem   = NULL;

    stlInt32             i;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );

    
    /**
     * AND Function을 이용하여 Range Expression을 묶음
     */

    if( aExprList->mCount == 0 )
    {
        sResultExpr = NULL;
    }
    else if( aExprList->mCount == 1 )
    {
        sResultExpr = aExprList->mHead->mExprPtr;
    }
    else
    {
        STL_TRY( knlAllocRegionMem( aRegionMem,
                                    STL_SIZEOF( qlvInitExpression ) * aExprList->mCount,
                                    (void **) & sArgArr,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        sCurItem = aExprList->mHead;
        i = 0;
        while( sCurItem != NULL )
        {
            sNextItem = sCurItem->mNext;

            sArgArr[i] = sCurItem->mExprPtr;

            sCurItem = sNextItem;
            i++;
        }
                
        STL_TRY( qlvMakeFuncAndNArg( aRegionMem,
                                     aExprList->mHead->mExprPtr->mPosition,
                                     i,
                                     sArgArr,
                                     &sResultExpr,
                                     aEnv )
                 == STL_SUCCESS );
    }

    /**
     * OUTPUT 설정
     */

    *aExpr = sResultExpr;

    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief 하나의 expression(comparison function)에 대한 range 구성 정보 얻기
 * @param[in]     aExpr              Expression
 * @param[in]     aNullOffset        NULL Value's Context Offset
 * @param[in]     aFalseOffset       FALSE Value's Context Offset
 * @param[out]    aKeyColOffset      Key Column's Context Offset 
 * @param[out]    aRangeValOffset    Range Value's Context Offset 
 * @param[out]    aIsIncludeEqual    Equal 조건을 포함하는지 여부
 * @param[out]    aIsNullAlwaysStop  NULL 상수에 대해 항상 FALSE 인 Key Range 여부  
 * @param[out]    aIsLikeFunc        해당 Range와 관련된 Function이 Like Function인지 여부
 * @param[in]     aEnv               Environment
 */
stlStatus qloGetRangeFromExpr( qlvInitExpression  * aExpr, 
                               stlUInt32            aNullOffset,
                               stlUInt32            aFalseOffset,
                               stlUInt32          * aKeyColOffset,
                               stlUInt32          * aRangeValOffset,
                               stlBool            * aIsIncludeEqual,
                               stlBool            * aIsNullAlwaysStop,
                               stlBool            * aIsLikeFunc,
                               qllEnv             * aEnv )
{
    qlvInitExpression  * sFuncExpr = NULL;
    qlvInitFunction    * sFuncCode = NULL;

    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExpr->mType == QLV_EXPR_TYPE_FUNCTION,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aKeyColOffset != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRangeValOffset != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIsIncludeEqual != NULL, QLL_ERROR_STACK(aEnv) );

    sFuncExpr = aExpr;
    sFuncCode = sFuncExpr->mExpr.mFunction;

    *aIsLikeFunc = STL_FALSE;

    switch( sFuncCode->mFuncId )
    {
        case KNL_BUILTIN_FUNC_IS_EQUAL :
        case KNL_BUILTIN_FUNC_IS_LESS_THAN_EQUAL :
        case KNL_BUILTIN_FUNC_IS_GREATER_THAN_EQUAL :
            {
                STL_DASSERT( ( sFuncCode->mArgs[0]->mType == QLV_EXPR_TYPE_COLUMN ) ||
                             ( sFuncCode->mArgs[0]->mType == QLV_EXPR_TYPE_INNER_COLUMN ) );
                
                /* STL_PARAM_VALIDATE( ( sFuncCode->mArgs[1]->mType == */
                /*                       QLV_EXPR_TYPE_VALUE ) || */
                /*                     ( sFuncCode->mArgs[1]->mType == */
                /*                       QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT ) || */
                /*                     ( sFuncCode->mArgs[1]->mType == */
                /*                       QLV_EXPR_TYPE_OUTER_COLUMN ) || */
                /*                     ( sFuncCode->mArgs[1]->mType == */
                /*                       QLV_EXPR_TYPE_REFERENCE ) || */
                /*                     ( sFuncCode->mArgs[1]->mType == */
                /*                       QLV_EXPR_TYPE_BIND_PARAM ), */
                /*                     QLL_ERROR_STACK(aEnv) ); */

                *aKeyColOffset   = sFuncCode->mArgs[0]->mOffset;
                *aRangeValOffset = sFuncCode->mArgs[1]->mOffset;
                
                *aIsIncludeEqual = STL_TRUE;
                *aIsNullAlwaysStop = STL_TRUE;
                break;
            }
        case KNL_BUILTIN_FUNC_IS_LESS_THAN :
        case KNL_BUILTIN_FUNC_IS_GREATER_THAN :
            {
                STL_DASSERT( ( sFuncCode->mArgs[0]->mType == QLV_EXPR_TYPE_COLUMN ) ||
                             ( sFuncCode->mArgs[0]->mType == QLV_EXPR_TYPE_INNER_COLUMN ) );
                
                /* STL_PARAM_VALIDATE( ( sFuncCode->mArgs[1]->mType == */
                /*                       QLV_EXPR_TYPE_VALUE ) || */
                /*                     ( sFuncCode->mArgs[1]->mType == */
                /*                       QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT ) || */
                /*                     ( sFuncCode->mArgs[1]->mType == */
                /*                       QLV_EXPR_TYPE_OUTER_COLUMN ) || */
                /*                     ( sFuncCode->mArgs[1]->mType == */
                /*                       QLV_EXPR_TYPE_REFERENCE ) || */
                /*                     ( sFuncCode->mArgs[1]->mType == */
                /*                       QLV_EXPR_TYPE_BIND_PARAM ), */
                /*                     QLL_ERROR_STACK(aEnv) ); */

                *aKeyColOffset   = sFuncCode->mArgs[0]->mOffset;
                *aRangeValOffset = sFuncCode->mArgs[1]->mOffset;
                
                *aIsIncludeEqual = STL_FALSE;
                *aIsNullAlwaysStop = STL_TRUE;
                break;
            }
        case KNL_BUILTIN_FUNC_IS_NULL :
        case KNL_BUILTIN_FUNC_IS_UNKNOWN :
            {
                *aKeyColOffset   = sFuncCode->mArgs[0]->mOffset;
                *aRangeValOffset = aNullOffset;
                *aIsIncludeEqual = STL_TRUE;
                *aIsNullAlwaysStop = STL_FALSE;
                break;
            }
        case KNL_BUILTIN_FUNC_IS_NOT_NULL :
        case KNL_BUILTIN_FUNC_IS_NOT_UNKNOWN :
            {
                *aKeyColOffset   = sFuncCode->mArgs[0]->mOffset;
                *aRangeValOffset = aNullOffset;
                *aIsIncludeEqual = STL_FALSE;
                *aIsNullAlwaysStop = STL_FALSE;
                break;
            }
        case KNL_BUILTIN_FUNC_NOT :
            {
                *aKeyColOffset   = sFuncCode->mArgs[0]->mOffset;
                *aRangeValOffset = aFalseOffset;
                *aIsIncludeEqual = STL_TRUE;
                *aIsNullAlwaysStop = STL_TRUE;
                break;
            }
        case KNL_BUILTIN_FUNC_LIKE :
            {
                *aKeyColOffset   = sFuncCode->mArgs[0]->mOffset;
                *aRangeValOffset = sFuncCode->mArgs[1]->mOffset;
                *aIsIncludeEqual = STL_TRUE;
                *aIsNullAlwaysStop = STL_TRUE;
                *aIsLikeFunc = STL_TRUE;
                break;
            }
        default :
            {
                break;
            }
    }

    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Expression을 분석하여 Index Scan와 Table Scan시 읽어야할 Column List를 구성한다.
 * @param[in]     aIndexHandle          Index Handle
 * @param[in]     aInitNode             Init Node
 * @param[in]     aInitExprCode         Index Scan 관련 Column들만 Column Expression으로 재구성한 Expression
 * @param[in,out] aIndexColExprList     Expression이 참조하고 있는 Index Column List
 * @param[in,out] aTableColExprList     Expression이 참조하고 있는 Table Column List
 * @param[in,out] aHasRowIdCol          Expression이 참조하고 있는 Row Id Column이 존재하는지 여부
 * @param[in]     aRegionMem            Region Memory
 * @param[in]     aEnv                  Environment
 *
 * @remark 1. 해당 Expression 처리를 위해
 *    <BR>    Index Scan 참조할 컬럼 리스트를 구성하고,
 *    <BR>    Table Scan (Fetch Record) 참조할 컬럼 리스트를 구성한다.
 *    <BR> 2. Expression이 참조하는 하위 Expression에 대한 검사는 수행하지 않는다.
 *    <BR>    (Bottom-up 방식으로 해당 함수를 호출한다.)
 */
stlStatus qloGetRefColListForFilter( ellDictHandle       * aIndexHandle,
                                     qlvInitNode         * aInitNode,
                                     qlvInitExpression   * aInitExprCode,
                                     qlvRefExprList      * aIndexColExprList,
                                     qlvRefExprList      * aTableColExprList,
                                     stlBool             * aHasRowIdCol,
                                     knlRegionMem        * aRegionMem,
                                     qllEnv              * aEnv )
{
    qlvInitExpression  * sInitExprCode = NULL;
    ellDictHandle      * sColumnHandle = NULL;
    ellDictHandle      * sIdxColHandle = NULL;
    ellIndexKeyDesc    * sIndexKeyDesc = NULL;
    stlInt32             sIndexKeyCnt  = 0;
    stlInt32             sLoop         = 0;

    qlvInitFunction    * sInitFunction = NULL;
    qlvInitTypeCast    * sInitTypeCast = NULL;
    qlvInitCaseExpr    * sInitCaseExpr = NULL;
    qlvInitListFunc    * sInitListFunc = NULL;
    qlvInitListCol     * sInitListCol  = NULL;
    qlvInitRowExpr     * sInitRowExpr  = NULL;
    
    qlvInitInstantNode * sInstantNode  = NULL;
    
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aInitExprCode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIndexColExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableColExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );


    sInitExprCode = aInitExprCode;
    STL_DASSERT( sInitExprCode->mType < QLV_EXPR_TYPE_MAX );

    
    /**
     * Column List 구성
     */
    
    switch( sInitExprCode->mType )
    {
        case QLV_EXPR_TYPE_COLUMN :
            {
                /**
                 * Expression이 Column인 경우 Column List를 구성
                 */

                STL_DASSERT( aIndexHandle != NULL );

                if( sInitExprCode->mExpr.mColumn->mRelationNode !=
                    aInitNode )
                {
                    break;
                }
                
                sColumnHandle = sInitExprCode->mExpr.mColumn->mColumnHandle;

                
                /**
                 * Index Key Column인지 검사
                 */

                sIndexKeyCnt  = ellGetIndexKeyCount( aIndexHandle );
                sIndexKeyDesc = ellGetIndexKeyDesc( aIndexHandle );

                STL_DASSERT( ellGetColumnTableID( sColumnHandle ) ==
                             ellGetColumnTableID( & sIndexKeyDesc[0].mKeyColumnHandle ) );

                for( sLoop = 0 ; sLoop < sIndexKeyCnt ; sLoop++ )
                {
                    sIdxColHandle = & sIndexKeyDesc[ sLoop ].mKeyColumnHandle;

                    if( ellGetColumnIdx( sColumnHandle ) == ellGetColumnIdx( sIdxColHandle ) )
                    {
                        break;
                    }
                    else
                    {
                        /* Do Nothing */
                    }
                }


                /**
                 * Column List 구성
                 */

                if( sLoop == sIndexKeyCnt )
                {
                    /**
                     * Table Scan Column List에 Column 추가
                     */

                    STL_DASSERT( aInitExprCode->mType == QLV_EXPR_TYPE_COLUMN );
                    STL_TRY( qlvAddExprToRefColExprList( aTableColExprList,
                                                         aInitExprCode,
                                                         aRegionMem,
                                                         aEnv )
                             == STL_SUCCESS );
        
                }
                else
                {
                    /**
                     * Index Scan Column List에 Column 추가
                     */
        
                    STL_DASSERT( aInitExprCode->mType == QLV_EXPR_TYPE_COLUMN );
                    STL_TRY( qlvAddExprToRefColExprList( aIndexColExprList,
                                                         aInitExprCode,
                                                         aRegionMem,
                                                         aEnv )
                             == STL_SUCCESS );
                }
                
                break;
            }
            
        case QLV_EXPR_TYPE_INNER_COLUMN :
            {
                /**
                 * Expression이 Inner Column인 경우 Column List를 구성
                 */

                if( sInitExprCode->mExpr.mInnerColumn->mRelationNode !=
                    aInitNode )
                {
                    break;
                }

                switch( sInitExprCode->mExpr.mInnerColumn->mRelationNode->mType )
                {
                    case QLV_NODE_TYPE_SORT_INSTANT :
                    case QLV_NODE_TYPE_HASH_INSTANT :
                    case QLV_NODE_TYPE_GROUP_HASH_INSTANT :
                    case QLV_NODE_TYPE_GROUP_SORT_INSTANT :
                    case QLV_NODE_TYPE_SORT_JOIN_INSTANT :
                    case QLV_NODE_TYPE_HASH_JOIN_INSTANT :
                        {
                            /**
                             * Sort Key Column인지 검사
                             */
                            
                            sInstantNode = (qlvInitInstantNode *) sInitExprCode->mExpr.mInnerColumn->mRelationNode;
                            
                            if( *sInitExprCode->mExpr.mInnerColumn->mIdx <
                                sInstantNode->mKeyColCnt )
                            {
                                STL_TRY( qlvAddExprToRefColExprList( aIndexColExprList,
                                                                     aInitExprCode,
                                                                     aRegionMem,
                                                                     aEnv )
                                         == STL_SUCCESS );
                            }
                            else
                            {
                                STL_TRY( qlvAddExprToRefColExprList( aTableColExprList,
                                                                     aInitExprCode,
                                                                     aRegionMem,
                                                                     aEnv )
                                         == STL_SUCCESS );
                            }
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

        case QLV_EXPR_TYPE_FUNCTION :
            {
                sInitFunction = sInitExprCode->mExpr.mFunction;
                
                for( sLoop = 0 ; sLoop < sInitFunction->mArgCount ; sLoop++ )
                {
                    STL_TRY( qloGetRefColListForFilter( aIndexHandle,
                                                        aInitNode,
                                                        sInitFunction->mArgs[ sLoop ],
                                                        aIndexColExprList,
                                                        aTableColExprList,
                                                        aHasRowIdCol,
                                                        aRegionMem,
                                                        aEnv )
                             == STL_SUCCESS );
                }
                
                break;
            }
            
        case QLV_EXPR_TYPE_CAST :
            {
                sInitTypeCast = sInitExprCode->mExpr.mTypeCast;
                
                for( sLoop = 0 ; sLoop < DTL_CAST_INPUT_ARG_CNT ; sLoop++ )
                {
                    STL_TRY( qloGetRefColListForFilter( aIndexHandle,
                                                        aInitNode,
                                                        sInitTypeCast->mArgs[ sLoop ],
                                                        aIndexColExprList,
                                                        aTableColExprList,
                                                        aHasRowIdCol,
                                                        aRegionMem,
                                                        aEnv )
                             == STL_SUCCESS );
                }
                
                break;
            }

        case QLV_EXPR_TYPE_CASE_EXPR :
            {
                sInitCaseExpr = sInitExprCode->mExpr.mCaseExpr;
                
                for( sLoop = 0 ; sLoop < sInitCaseExpr->mCount ; sLoop++ )
                {
                    STL_TRY( qloGetRefColListForFilter( aIndexHandle,
                                                        aInitNode,
                                                        sInitCaseExpr->mWhenArr[ sLoop ] ,
                                                        aIndexColExprList,
                                                        aTableColExprList,
                                                        aHasRowIdCol,
                                                        aRegionMem,
                                                        aEnv )
                             == STL_SUCCESS );

                    STL_TRY( qloGetRefColListForFilter( aIndexHandle,
                                                        aInitNode,
                                                        sInitCaseExpr->mThenArr[ sLoop ] ,
                                                        aIndexColExprList,
                                                        aTableColExprList,
                                                        aHasRowIdCol,
                                                        aRegionMem,
                                                        aEnv )
                             == STL_SUCCESS );
                }

                STL_TRY( qloGetRefColListForFilter( aIndexHandle,
                                                    aInitNode,
                                                    sInitCaseExpr->mDefResult ,
                                                    aIndexColExprList,
                                                    aTableColExprList,
                                                    aHasRowIdCol,
                                                    aRegionMem,
                                                    aEnv )
                         == STL_SUCCESS );
                
                break;
            }

        case QLV_EXPR_TYPE_LIST_FUNCTION :
            {
                sInitListFunc = sInitExprCode->mExpr.mListFunc;
                
                for( sLoop = 0 ; sLoop < sInitListFunc->mArgCount ; sLoop++ )
                {
                    STL_TRY( qloGetRefColListForFilter( aIndexHandle,
                                                        aInitNode,
                                                        sInitListFunc->mArgs[ sLoop ],
                                                        aIndexColExprList,
                                                        aTableColExprList,
                                                        aHasRowIdCol,
                                                        aRegionMem,
                                                        aEnv )
                             == STL_SUCCESS );
                }
                
                break;
            }
        case QLV_EXPR_TYPE_LIST_COLUMN :
            {
                sInitListCol = sInitExprCode->mExpr.mListCol;
                
                for( sLoop = 0 ; sLoop < sInitListCol->mArgCount ; sLoop++ )
                {
                    STL_TRY( qloGetRefColListForFilter( aIndexHandle,
                                                        aInitNode,
                                                        sInitListCol->mArgs[ sLoop ],
                                                        aIndexColExprList,
                                                        aTableColExprList,
                                                        aHasRowIdCol,
                                                        aRegionMem,
                                                        aEnv )
                             == STL_SUCCESS );
                }
                
                break;
            }
        case QLV_EXPR_TYPE_ROW_EXPR :
            {
                sInitRowExpr = sInitExprCode->mExpr.mRowExpr;
                
                for( sLoop = 0 ; sLoop < sInitRowExpr->mArgCount ; sLoop++ )
                {
                    STL_TRY( qloGetRefColListForFilter( aIndexHandle,
                                                        aInitNode,
                                                        sInitRowExpr->mArgs[ sLoop ],
                                                        aIndexColExprList,
                                                        aTableColExprList,
                                                        aHasRowIdCol,
                                                        aRegionMem,
                                                        aEnv )
                             == STL_SUCCESS );
                }
                
                break;
            }
        case QLV_EXPR_TYPE_ROWID_COLUMN :
            {
                *aHasRowIdCol = STL_TRUE;
                break;
            }
        /* 나머지 Expression들은 내부에 Column Expr을 포함할 수 없다 */
        default :   
            {
                /* Do Nothing */
                break;
            }
    }
    
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/*******************************************************************************
 * Key-Hash
 ******************************************************************************/

/**
 * @brief 
 * @param[in]     aSQLString            SQL String
 * @param[in]     aIndexHandle          Index Handle
 * @param[in]     aInitNode             Init Node
 * @param[in]     aBindContext          Bind Context 
 * @param[in]     aHashKeyExprCode      Hash Key에 대한 Expression
 * @param[in]     aNonHashKeyExprCode   Non Hash Key에 대한 Expression
 * @param[out]    aHashScanInfo         Hash Filter 구성 정보 
 * @param[in]     aRegionMem            Region Memory
 * @param[in]     aEnv                  Environment
 */
stlStatus qloGetKeyHash( stlChar            * aSQLString,
                         ellDictHandle      * aIndexHandle,
                         qlvInitNode        * aInitNode,
                         knlBindContext     * aBindContext,
                         qlvInitExpression  * aHashKeyExprCode,
                         qlvInitExpression  * aNonHashKeyExprCode,
                         qloHashScanInfo   ** aHashScanInfo,
                         knlRegionMem       * aRegionMem,
                         qllEnv             * aEnv )
{
    stlInt32            i;
    qlvRefExprList    * sHashColExprList  = NULL;
    qlvRefExprList    * sTableColExprList = NULL;
    qloHashScanInfo   * sHashScanInfo     = NULL;
    qlvStmtInfo         sStmtInfo;


    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLString != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( ((aInitNode->mType == QLV_NODE_TYPE_HASH_INSTANT) ||
                         (aInitNode->mType == QLV_NODE_TYPE_HASH_JOIN_INSTANT)), QLL_ERROR_STACK(aEnv) );

    stlMemset( & sStmtInfo, 0x00, STL_SIZEOF( qlvStmtInfo ) );
    
    
    /**
     * range/filter 구성을 위한 expression 분류 공간 할당 및 초기화 
     */

    /* Hash Scan 공간 할당 */
    STL_TRY( knlAllocRegionMem(
                 aRegionMem,
                 STL_SIZEOF( qloHashScanInfo ),
                 (void **) & sHashScanInfo,
                 KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sHashScanInfo, 0x00, STL_SIZEOF( qloHashScanInfo ) );

    
    /**
     * HASH SCAN METHOD 구분
     */

    STL_DASSERT( aHashKeyExprCode != NULL );

    /**
     * Expression을 Range / Filter 요소로 구분하기
     * @todo OR를 지원하는 경우 변경 필요
     */


    /**
     * Hash Scan Info 초기화
     */

    sHashScanInfo->mHashColCount = ((qlvInitInstantNode*)aInitNode)->mKeyColCnt;

    /* hash filter 공간 할당 */
    STL_TRY( knlAllocRegionMem(
                 aRegionMem,
                 STL_SIZEOF( qlvRefExprList * ) * sHashScanInfo->mHashColCount,
                 (void **) & sHashScanInfo->mHashFilterExpr,
                 KNL_ENV(aEnv) )
             == STL_SUCCESS );

    for( i = 0; i < sHashScanInfo->mHashColCount; i++ )
    {
        STL_TRY( qlvCreateRefExprList( & sHashScanInfo->mHashFilterExpr[i],
                                       aRegionMem,
                                       aEnv )
                 == STL_SUCCESS );
    }

    /* logical table filter 공간 할당 */
    STL_TRY( qlvCreateRefExprList( & sHashScanInfo->mLogicalTableFilterExprList,
                                   aRegionMem,
                                   aEnv )
             == STL_SUCCESS );

    /* physical table filter 공간 할당 */
    STL_TRY( qlvCreateRefExprList( & sHashScanInfo->mPhysicalTableFilterExprList,
                                   aRegionMem,
                                   aEnv )
             == STL_SUCCESS );

    /* Hash Column Expression List */
    STL_TRY( qlvCreateRefExprList( & sHashColExprList,
                                   aRegionMem,
                                   aEnv )
             == STL_SUCCESS );

    /* Table Column Expression List */
    STL_TRY( qlvCreateRefExprList( & sTableColExprList,
                                   aRegionMem,
                                   aEnv )
             == STL_SUCCESS );

    /* Hash Key에 대한 Predicate 분류 */
    STL_TRY( qloClassifyHashPredExpr( aSQLString,
                                      aInitNode,
                                      aBindContext,
                                      aHashKeyExprCode,
                                      STL_TRUE,
                                      sHashScanInfo,
                                      sHashColExprList,
                                      sTableColExprList,
                                      aRegionMem,
                                      aEnv )
             == STL_SUCCESS );

    /* Non Hash Key에 대한 Predicate 분류 */
    if( aNonHashKeyExprCode != NULL )
    {
        STL_TRY( qloClassifyHashPredExpr( aSQLString,
                                          aInitNode,
                                          aBindContext,
                                          aNonHashKeyExprCode,
                                          STL_FALSE,
                                          sHashScanInfo,
                                          sHashColExprList,
                                          sTableColExprList,
                                          aRegionMem,
                                          aEnv )
                 == STL_SUCCESS );
    }

    /* logical table filter expression 구성 */
    STL_TRY( qloLinkRangeExpr( & sHashScanInfo->mLogicalTableFilter,
                               sHashScanInfo->mLogicalTableFilterExprList,
                               aRegionMem,
                               aEnv )
             == STL_SUCCESS );


    /**
     * OUTPUT 설정
     */
    
    *aHashScanInfo = sHashScanInfo;

    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Expression을 hash/filter 대상으로 구분하기
 * @param[in]     aSQLString          SQL String
 * @param[in]     aInitNode           Init Node
 * @param[in]     aBindContext        Bind Context 
 * @param[in]     aInitExprCode       Parse/Validation 단계의 Code Range
 * @param[in]     aIsHashKeyFilter    Hash Key Filter인지 여부
 * @param[in,out] aHashScanInfo       Hash/Filter Expression 분류 정보
 * @param[in,out] aHashColExprList    Hash Column List 
 * @param[in,out] aTableColExprList   Table Column List 
 * @param[in]     aRegionMem          Region Memory
 * @param[in]     aEnv                Environment
 */
stlStatus qloClassifyHashPredExpr( stlChar              * aSQLString,
                                   qlvInitNode          * aInitNode,
                                   knlBindContext       * aBindContext,
                                   qlvInitExpression    * aInitExprCode,
                                   stlBool                aIsHashKeyFilter,
                                   qloHashScanInfo      * aHashScanInfo,
                                   qlvRefExprList       * aHashColExprList,
                                   qlvRefExprList       * aTableColExprList,
                                   knlRegionMem         * aRegionMem,
                                   qllEnv               * aEnv )
{
    qlvInitExpression     * sInitExprCode    = NULL;
    qlvInitFunction       * sInitFunction    = NULL;
    qlvInitListFunc       * sListFunc        = NULL;
    qlvInitListCol        * sListColLeft     = NULL;
    qlvInitListCol        * sListColRight    = NULL;
    qlvInitRowExpr        * sRowExpr         = NULL;
    stlInt32                sColIdx          = 0;
    stlInt32                sLoop            = 0;
    qlvRefExprItem        * sRefExprItem     = NULL;

    stlBool                 sHasRowIdCol     = STL_FALSE;
    stlBool                 sIsHashKey       = STL_FALSE;
    stlBool                 sIsTableFilter   = STL_FALSE;
    stlBool                 sIsLogicalFilter = STL_FALSE;
    stlBool                 sIsSameColumn    = STL_FALSE;
    stlBool                 sIsIncludeFunc   = STL_FALSE;

    dtlDataType             sDataType1       = DTL_TYPE_NA;
    dtlDataType             sDataType2       = DTL_TYPE_NA;
    dtlPhysicalFunc         sPhysicalFuncPtr = NULL;

    qlvRefExprList          sTableColExprList;
    qlvRefExprList          sHashColExprList;
    const dtlCompareType  * sCompareType     = NULL;

    
    /**
     * MACRO (코드 단순화 등을 위해 반복 사용되는 부분을 macro로 만듬)
     */


#define ADD_HASH_FILTER( aExpr )                                                        \
    {                                                                                   \
        sColIdx = 0;                                                                    \
        sRefExprItem = ((qlvInitInstantNode*)aInitNode)->mKeyColList->mHead;            \
        while( sRefExprItem != NULL )                                                   \
        {                                                                               \
            if( qlvIsSameExpression( aHashColExprList->mHead->mExprPtr,                 \
                                     sRefExprItem->mExprPtr )                           \
                == STL_TRUE )                                                           \
            {                                                                           \
                break;                                                                  \
            }                                                                           \
                                                                                        \
            sColIdx++;                                                                  \
            sRefExprItem = sRefExprItem->mNext;                                         \
        }                                                                               \
                                                                                        \
        STL_TRY( qlvAddExprToRefExprList( aHashScanInfo->mHashFilterExpr[sColIdx],      \
                                          (aExpr),                                      \
                                          STL_FALSE,                                    \
                                          aRegionMem,                                   \
                                          aEnv )                                        \
                 == STL_SUCCESS );                                                      \
    }

#define ADD_HASH_PHYSICAL_TABLE_FILTER( aExpr )                                         \
    {                                                                                   \
        STL_TRY( qlvAddExprToRefExprList( aHashScanInfo->mPhysicalTableFilterExprList,  \
                                          (aExpr),                                      \
                                          STL_FALSE,                                    \
                                          aRegionMem,                                   \
                                          aEnv )                                        \
                 == STL_SUCCESS );                                                      \
    }

#define ADD_HASH_LOGICAL_TABLE_FILTER( aExpr )                                          \
    {                                                                                   \
        STL_TRY( qlvAddExprToRefExprList( aHashScanInfo->mLogicalTableFilterExprList,   \
                                          (aExpr),                                      \
                                          STL_FALSE,                                    \
                                          aRegionMem,                                   \
                                          aEnv )                                        \
                 == STL_SUCCESS );                                                      \
    }


    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLString != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitExprCode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aHashScanInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aHashColExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableColExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    

    /**
     * Iteration Time이 Each Expression이 아닌 경우는 Column을 포함하지 않은 경우이다.
     */
    
    if( aInitExprCode->mIterationTime != DTL_ITERATION_TIME_FOR_EACH_EXPR )
    {
        /**
         * constant value만 포함된 조건은 Fetch 이전의 Execution에서 수행하여야 함.
         */

        /**
         * @todo 해당 Expression을 Fetch 단위 Constant Expression으로 등록하고,
         *  <BR> Constant Expression은 Logical Key Filter에 추가한다.
         *  <BR> 
         *  <BR> 구현 이전까지는 해당 Expression을 logical filter로 분류한다. 
         */

        /* Add to Logical Filter */
        ADD_HASH_LOGICAL_TABLE_FILTER( aInitExprCode );
        
        STL_THROW( RAMP_FINISH );
    }
    else
    {
        if( aInitExprCode->mType == QLV_EXPR_TYPE_LIST_FUNCTION )
        {
            sInitExprCode = aInitExprCode;
            sListFunc     = sInitExprCode->mExpr.mListFunc;

            STL_DASSERT( ( sListFunc->mArgs[0]->mType == QLV_EXPR_TYPE_LIST_COLUMN ) &&
                         ( sListFunc->mArgCount == 2 ) );
            
            sListColLeft  = sListFunc->mArgs[1]->mExpr.mListCol;
            sListColRight  = sListFunc->mArgs[0]->mExpr.mListCol;
            STL_DASSERT( ( sListColLeft->mArgs[0]->mType == QLV_EXPR_TYPE_ROW_EXPR ) &&
                         ( sListColRight->mArgs[0]->mType == QLV_EXPR_TYPE_ROW_EXPR ) );

            sRowExpr  = sListColLeft->mArgs[0]->mExpr.mRowExpr;

            if( ( sRowExpr->mArgCount == 1 ) &&
                ( ( sListFunc->mFuncId >= KNL_LIST_FUNC_START_ID ) &&
                  ( sListFunc->mFuncId <= KNL_LIST_FUNC_END_ID ) ) )
            {
                if( sListFunc->mArgs[0]->mIterationTime != DTL_ITERATION_TIME_FOR_EACH_EXPR )
                {
                    if( ( sRowExpr->mArgs[0]->mType == QLV_EXPR_TYPE_COLUMN ) ||
                        ( sRowExpr->mArgs[0]->mType == QLV_EXPR_TYPE_INNER_COLUMN ) )
                    {
                        /* physical function이 없으면 Logical Filter로 분류 */

                        for( sLoop = 0; sLoop < sListColLeft->mArgCount; sLoop ++ )
                        {
                            STL_TRY( qloGetDataType( aSQLString,
                                                     sListColLeft->mArgs[sLoop]->mExpr.mRowExpr->mArgs[0],
                                                     aBindContext,
                                                     & sDataType1,
                                                     aRegionMem,
                                                     aEnv )
                                     == STL_SUCCESS );

                            STL_TRY( qloGetDataType( aSQLString,
                                                     sListColRight->mArgs[sLoop]->mExpr.mRowExpr->mArgs[0],
                                                     aBindContext,
                                                     & sDataType2,
                                                     aRegionMem,
                                                     aEnv )
                                     == STL_SUCCESS );

                            STL_TRY( qlnoGetCompareType( aSQLString,
                                                         sDataType1,
                                                         sDataType2,
                                                         STL_FALSE,
                                                         STL_TRUE,
                                                         sInitExprCode->mPosition,
                                                         &sCompareType,
                                                         aEnv )
                                     == STL_SUCCESS );

                            if( sCompareType->mLeftType == sDataType1 )
                            {
                                qloGetPhysicalFuncPtr( KNL_BUILTIN_FUNC_IS_EQUAL,
                                                       sDataType1,
                                                       sDataType1,
                                                       & sPhysicalFuncPtr,
                                                       STL_TRUE );
                            }
                            else
                            {
                                qloGetPhysicalFuncPtr( KNL_BUILTIN_FUNC_IS_EQUAL,
                                                       sDataType1,
                                                       sDataType2,
                                                       & sPhysicalFuncPtr,
                                                       STL_TRUE );
                            }

                            if( sPhysicalFuncPtr == NULL )
                            {
                                ADD_HASH_LOGICAL_TABLE_FILTER( sInitExprCode );
                                break;
                            }
                        }
                        if( sPhysicalFuncPtr != NULL )
                        {
                            ADD_HASH_PHYSICAL_TABLE_FILTER( sInitExprCode );
                        } 
                    }
                    else
                    {
                        ADD_HASH_LOGICAL_TABLE_FILTER( sInitExprCode );
                    }
                }
                else
                {
                    ADD_HASH_LOGICAL_TABLE_FILTER( sInitExprCode );
                }
            }
            else
            {
                ADD_HASH_LOGICAL_TABLE_FILTER( sInitExprCode );
            }
            STL_THROW( RAMP_FINISH );
        }
    
        if( aInitExprCode->mType != QLV_EXPR_TYPE_FUNCTION )
        {
            /* Add to Logical Filter */
            ADD_HASH_LOGICAL_TABLE_FILTER( aInitExprCode );

            STL_THROW( RAMP_FINISH );
        }

        /* Do Nothing */
    }

    /**
     * Function Expression 처리
     */
    
    STL_DASSERT( aInitExprCode->mType == QLV_EXPR_TYPE_FUNCTION );

    sInitExprCode = aInitExprCode;
    sInitFunction = sInitExprCode->mExpr.mFunction;

    STL_DASSERT( aInitNode->mType != QLV_NODE_TYPE_BASE_TABLE );

    if( sInitFunction->mFuncId == KNL_BUILTIN_FUNC_AND )
    {
        /**
         * @todo AND Expression의 Argument Count를 가변 변경
         */
                
        STL_PARAM_VALIDATE( sInitFunction->mArgCount >= 2, QLL_ERROR_STACK(aEnv) );

        for( sLoop = 0; sLoop < sInitFunction->mArgCount; sLoop++ )
        {
            STL_TRY( qloClassifyHashPredExpr( aSQLString,
                                              aInitNode,
                                              aBindContext,
                                              sInitFunction->mArgs[sLoop],
                                              aIsHashKeyFilter,
                                              aHashScanInfo,
                                              aHashColExprList,
                                              aTableColExprList,
                                              aRegionMem,
                                              aEnv )
                     == STL_SUCCESS );
        }

        STL_THROW( RAMP_FINISH );
    }
    else if( sInitFunction->mFuncId == KNL_BUILTIN_FUNC_OR )
    {
        ADD_HASH_LOGICAL_TABLE_FILTER( sInitExprCode );

        STL_THROW( RAMP_FINISH );
    }
    else
    {
        /**
         * Expression에 포함된 Index 및 Table Column List 구성
         */
        
        sHasRowIdCol     = STL_FALSE;
        sIsHashKey       = aIsHashKeyFilter;
        sIsTableFilter   = STL_FALSE;
        sIsLogicalFilter = STL_FALSE;
        sIsSameColumn    = STL_FALSE;
        sIsIncludeFunc   = STL_FALSE;

        aHashColExprList->mCount = 0;
        aHashColExprList->mHead  = NULL;
        aHashColExprList->mTail  = NULL;
        
        aTableColExprList->mCount = 0;
        aTableColExprList->mHead  = NULL;
        aTableColExprList->mTail  = NULL;
 
        if( sInitFunction->mFuncId != KNL_BUILTIN_FUNC_IS_EQUAL )
        {
            /* Equal Function이 아니면 Table Filter로 분류 */
            sIsHashKey = STL_FALSE;
        }

        for( sLoop = 0 ; sLoop < sInitFunction->mArgCount ; sLoop++ )
        {
            switch( sInitFunction->mArgs[ sLoop ]->mType )
            {
                case QLV_EXPR_TYPE_FUNCTION :
                    {
                        if( sInitFunction->mArgs[ sLoop ]->mExpr.mFunction->mFuncId ==
                            KNL_BUILTIN_FUNC_LIKE_PATTERN )
                        {
                            if( aHashColExprList->mCount == 1 )
                            {
                                sTableColExprList.mCount = 0;
                                sTableColExprList.mHead  = NULL;
                                sTableColExprList.mTail  = NULL;

                                sHashColExprList.mCount = 0;
                                sHashColExprList.mHead  = NULL;
                                sHashColExprList.mTail  = NULL;

                                /* argument list 내의 참조 Column List 구성 */
                                STL_TRY( qloGetRefColListForFilter( NULL,
                                                                    aInitNode,
                                                                    sInitFunction->mArgs[ sLoop ],
                                                                    & sHashColExprList,
                                                                    & sTableColExprList,
                                                                    & sHasRowIdCol,
                                                                    aRegionMem,
                                                                    aEnv )
                                         == STL_SUCCESS );
                                
                                if( sTableColExprList.mCount > 0 )
                                {
                                    sIsHashKey = STL_FALSE;
                                    sIsTableFilter = STL_TRUE;
                                    sIsLogicalFilter = STL_TRUE;
                                }
                                break;
                            }
                        }
                    }
                case QLV_EXPR_TYPE_CAST :
                case QLV_EXPR_TYPE_CASE_EXPR :
                case QLV_EXPR_TYPE_LIST_FUNCTION :
                    {
                        sTableColExprList.mCount = 0;
                        sTableColExprList.mHead  = NULL;
                        sTableColExprList.mTail  = NULL;
                        
                        STL_TRY( qloGetRefColListForFilter( NULL,
                                                            aInitNode,
                                                            sInitFunction->mArgs[ sLoop ],
                                                            aHashColExprList,
                                                            & sTableColExprList,
                                                            & sHasRowIdCol,
                                                            aRegionMem,
                                                            aEnv )
                                 == STL_SUCCESS );

                        if( (sTableColExprList.mCount > 0) || (aHashColExprList->mCount > 1) )
                        {
                            sIsHashKey = STL_FALSE;
                            sIsLogicalFilter = STL_TRUE;
                            sIsTableFilter = STL_TRUE;
                        }
                        else
                        {
                            sHashColExprList.mCount = 0;
                            sHashColExprList.mHead  = NULL;
                            sHashColExprList.mTail  = NULL;

                            /* argument list 내의 참조 Column List 구성 */
                            STL_TRY( qloGetRefColListForFilter( NULL,
                                                                aInitNode,
                                                                sInitFunction->mArgs[ sLoop ],
                                                                & sHashColExprList,
                                                                aTableColExprList,
                                                                & sHasRowIdCol,
                                                                aRegionMem,
                                                                aEnv )
                                     == STL_SUCCESS );

                            if( sHashColExprList.mCount > 0 )
                            {
                                sIsHashKey = STL_FALSE;
                                sIsTableFilter = STL_TRUE;
                                sIsLogicalFilter = STL_TRUE;
                            }
                        }
                        break;
                    }
                case QLV_EXPR_TYPE_COLUMN :
                case QLV_EXPR_TYPE_INNER_COLUMN :
                    {
                        /* 참조 Column이 존재 */
                        if( aHashColExprList->mCount > 0 )
                        {
                            STL_TRY( qloGetRefColListForFilter( NULL,
                                                                aInitNode,
                                                                sInitFunction->mArgs[ sLoop ],
                                                                aHashColExprList,
                                                                aTableColExprList,
                                                                & sHasRowIdCol,
                                                                aRegionMem,
                                                                aEnv )
                                     == STL_SUCCESS );

                            sIsSameColumn = ( ( aHashColExprList->mCount == 1 ) &&
                                              ( aTableColExprList->mCount == 0 ) );
                        }
                        else
                        {
                            STL_TRY( qloGetRefColListForFilter( NULL,
                                                                aInitNode,
                                                                sInitFunction->mArgs[ sLoop ],
                                                                aHashColExprList,
                                                                aTableColExprList,
                                                                & sHasRowIdCol,
                                                                aRegionMem,
                                                                aEnv )
                                     == STL_SUCCESS );
                        }
                                
                        if( aTableColExprList->mCount > 0 )
                        {
                            sIsTableFilter = STL_TRUE;
                        }
                        break;
                    }
                case QLV_EXPR_TYPE_ROWID_COLUMN :
                    {
                        /* Record Column으로 구성되어 있기 때문에 Rowid Column 이 올 수 없다. */
                        STL_DASSERT( 0 );
                        break;
                    }
                default :
                    {
                        /* QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT */

                        /**
                         * @todo 최종적으로는 Constant Expression만 와야 한다.
                         */
                        
                        /* Do Nothing */
                        break;
                    }
            }

            if( sIsTableFilter == STL_TRUE )
            {
                sIsHashKey = STL_FALSE;
            }

            STL_DASSERT( sHasRowIdCol == STL_FALSE );

            if( ( sIsLogicalFilter == STL_TRUE ) && ( sIsTableFilter == STL_TRUE ) )
            {
                /* logical table filter expression 리스트에 추가 */
                ADD_HASH_LOGICAL_TABLE_FILTER( sInitExprCode );
                STL_THROW( RAMP_FINISH );
            }
            else
            {
                /* Do Nothing */
            }
        }

        if( aHashColExprList->mCount == 0 )
        {
            STL_DASSERT( sIsLogicalFilter == STL_FALSE );
            
            if( aTableColExprList->mCount == 0 )
            {
                /* logical table filter expression 리스트에 추가 */
                ADD_HASH_LOGICAL_TABLE_FILTER( sInitExprCode );
                STL_THROW( RAMP_FINISH );
            }
            else
            {
                /* Do Nothing */
            }
        }
        
        
        /**
         * 항상 동일한 값을 반환하는 Expr인지 확인
         */

        if( ( sIsIncludeFunc == STL_FALSE ) && ( sIsSameColumn == STL_TRUE ) )
        {
            switch( sInitFunction->mFuncId )
            {
                /* Equal 조건을 포함하는 2-operand 비교 연산자 */
                case KNL_BUILTIN_FUNC_IS_EQUAL :
                    {
                        ADD_HASH_LOGICAL_TABLE_FILTER( sInitExprCode );
                        
                        STL_THROW( RAMP_FINISH );
                    }
                    
                default :
                    {
                        break;
                    }
            }
        }
        else
        {
            /* Do Nothing */
        }

        /**
         * 동일 Key Column에 Filter가 등록되어 있는 경우 Table Filter로 분리
         */

        if( sIsHashKey == STL_TRUE )
        {
            sColIdx = 0;
            sRefExprItem = ((qlvInitInstantNode*)aInitNode)->mKeyColList->mHead;
            while( sRefExprItem != NULL )
            {
                if( qlvIsSameExpression( aHashColExprList->mHead->mExprPtr,
                                         sRefExprItem->mExprPtr )
                    == STL_TRUE )
                {
                    break;
                }

                sColIdx++;
                sRefExprItem = sRefExprItem->mNext;
            }

            if( aHashScanInfo->mHashFilterExpr[sColIdx]->mCount > 0 )
            {
                sIsHashKey = STL_FALSE;
            }
        }
        

        /**
         * Table Filter 구성
         */

        if( ( sIsHashKey == STL_FALSE ) || ( sIsTableFilter == STL_TRUE ) )
        {
            STL_DASSERT( sIsLogicalFilter == STL_FALSE );
            
            /* Physical vs Logical Filter 구분 */
            switch( sInitFunction->mFuncId )
            {
                /* 1-operand 비교 연산자 */
                case KNL_BUILTIN_FUNC_IS_NULL :
                case KNL_BUILTIN_FUNC_IS_NOT_NULL :
                    {
                        /* physical table filter expression 리스트에 추가 */
                        ADD_HASH_PHYSICAL_TABLE_FILTER( sInitExprCode );
                        break;
                    }
                case KNL_BUILTIN_FUNC_IS_UNKNOWN :
                case KNL_BUILTIN_FUNC_IS_NOT_UNKNOWN :
                case KNL_BUILTIN_FUNC_NOT :
                case KNL_BUILTIN_FUNC_IS :
                case KNL_BUILTIN_FUNC_IS_NOT :
                    {
                        /**
                         * Physical & Logical Filter 구분
                         */
                    
                        STL_DASSERT( ( sInitFunction->mArgs[0]->mType == QLV_EXPR_TYPE_COLUMN ) ||
                                     ( sInitFunction->mArgs[0]->mType == QLV_EXPR_TYPE_INNER_COLUMN ) );
                        
                        STL_TRY( qloGetDataType( aSQLString,
                                                 sInitFunction->mArgs[0],
                                                 aBindContext,
                                                 & sDataType1,
                                                 aRegionMem,
                                                 aEnv )
                                 == STL_SUCCESS );
                                
                        if( sDataType1 == DTL_TYPE_BOOLEAN )
                        {
                            /* physical table filter expression 리스트에 추가 */
                            ADD_HASH_PHYSICAL_TABLE_FILTER( sInitExprCode );
                        }
                        else
                        {
                            /* logical table filter expression 리스트에 추가 */
                            ADD_HASH_LOGICAL_TABLE_FILTER( sInitExprCode );
                        }
                        break;
                    }
                        
                    /* 2-operand 비교 연산자 */
                case KNL_BUILTIN_FUNC_IS_EQUAL :
                case KNL_BUILTIN_FUNC_IS_NOT_EQUAL :
                case KNL_BUILTIN_FUNC_IS_LESS_THAN :
                case KNL_BUILTIN_FUNC_IS_LESS_THAN_EQUAL :
                case KNL_BUILTIN_FUNC_IS_GREATER_THAN :
                case KNL_BUILTIN_FUNC_IS_GREATER_THAN_EQUAL :
                    {
                        /**
                         * Physical & Logical Filter 구분
                         */
                    
                        STL_TRY( qloGetDataType( aSQLString,
                                                 sInitFunction->mArgs[0],
                                                 aBindContext,
                                                 & sDataType1,
                                                 aRegionMem,
                                                 aEnv )
                                 == STL_SUCCESS );

                        STL_TRY( qloGetDataType( aSQLString,
                                                 sInitFunction->mArgs[1],
                                                 aBindContext,
                                                 & sDataType2,
                                                 aRegionMem,
                                                 aEnv )
                                 == STL_SUCCESS );

                        if( ( sInitFunction->mArgs[0]->mType == QLV_EXPR_TYPE_COLUMN ) ||
                            ( sInitFunction->mArgs[0]->mType == QLV_EXPR_TYPE_INNER_COLUMN ) )
                        {
                            if( ( sInitFunction->mArgs[1]->mType == QLV_EXPR_TYPE_COLUMN ) ||
                                ( sInitFunction->mArgs[1]->mType == QLV_EXPR_TYPE_INNER_COLUMN ) )
                            {
                                STL_TRY( qlnoGetCompareType( aSQLString,
                                                             sDataType1,
                                                             sDataType2,
                                                             STL_FALSE,
                                                             STL_FALSE,
                                                             sInitExprCode->mPosition,
                                                             &sCompareType,
                                                             aEnv )
                                         == STL_SUCCESS );

                                if( ( sCompareType->mLeftType == sDataType1 ) &&
                                    ( sCompareType->mRightType == sDataType2 ) )
                                {
                                    /* physical table filter expression 리스트에 추가 */
                                    ADD_HASH_PHYSICAL_TABLE_FILTER( sInitExprCode );
                                }
                                else
                                {
                                    /* logical table filter expression 리스트에 추가 */
                                    ADD_HASH_LOGICAL_TABLE_FILTER( sInitExprCode );
                                }
                            }
                            else
                            {
                                STL_TRY( qlnoGetCompareType( aSQLString,
                                                             sDataType1,
                                                             sDataType2,
                                                             STL_FALSE,
                                                             STL_TRUE,
                                                             sInitExprCode->mPosition,
                                                             &sCompareType,
                                                             aEnv )
                                         == STL_SUCCESS );
                                    
                                if( sCompareType->mLeftType == sDataType1 )
                                {
                                    /* physical table filter expression 리스트에 추가 */
                                    ADD_HASH_PHYSICAL_TABLE_FILTER( sInitExprCode );
                                }
                                else
                                {
                                    /* logical table filter expression 리스트에 추가 */
                                    ADD_HASH_LOGICAL_TABLE_FILTER( sInitExprCode );
                                }
                            }
                        }
                        else
                        {
                            STL_DASSERT( ( sInitFunction->mArgs[1]->mType == QLV_EXPR_TYPE_COLUMN ) ||
                                         ( sInitFunction->mArgs[1]->mType == QLV_EXPR_TYPE_INNER_COLUMN ) );

                            STL_TRY( qlnoGetCompareType( aSQLString,
                                                         sDataType1,
                                                         sDataType2,
                                                         STL_TRUE,
                                                         STL_FALSE,
                                                         sInitExprCode->mPosition,
                                                         &sCompareType,
                                                         aEnv )
                                     == STL_SUCCESS );
                                    
                            if( sCompareType->mRightType == sDataType2 )
                            {
                                /* physical table filter expression 리스트에 추가 */
                                ADD_HASH_PHYSICAL_TABLE_FILTER( sInitExprCode );
                            }
                            else
                            {
                                /* logical table filter expression 리스트에 추가 */
                                ADD_HASH_LOGICAL_TABLE_FILTER( sInitExprCode );
                            }
                        }
                        break;
                    }
                default :
                    {
                        /* logical table filter expression 리스트에 추가 */
                        ADD_HASH_LOGICAL_TABLE_FILTER( sInitExprCode );
                        break;
                    }
            }

            STL_THROW( RAMP_FINISH );
        }
        

        /**
         * FUNCTION 종류별 Hash Filter 구성
         */
        
        switch( sInitFunction->mFuncId )
        {
            /* 1-operand 비교 연산자 */
            case KNL_BUILTIN_FUNC_IS_NULL :
            case KNL_BUILTIN_FUNC_IS_NOT_NULL :
            case KNL_BUILTIN_FUNC_IS_UNKNOWN :
            case KNL_BUILTIN_FUNC_IS_NOT_UNKNOWN :
            case KNL_BUILTIN_FUNC_NOT :
                {
                    STL_DASSERT( 0 );
                    break;
                }

            /* 2-operand 비교 연산자 */
            case KNL_BUILTIN_FUNC_IS_NOT_EQUAL :
            case KNL_BUILTIN_FUNC_IS_LESS_THAN :
            case KNL_BUILTIN_FUNC_IS_LESS_THAN_EQUAL :
            case KNL_BUILTIN_FUNC_IS_GREATER_THAN :
            case KNL_BUILTIN_FUNC_IS_GREATER_THAN_EQUAL :
            case KNL_BUILTIN_FUNC_IS :
            case KNL_BUILTIN_FUNC_IS_NOT :
                {
                    STL_DASSERT( 0 );
                    break;
                }

            case KNL_BUILTIN_FUNC_IS_EQUAL :
                {
                    /**
                     * Hash Filter List 구성
                     */

                    STL_DASSERT( aHashColExprList->mCount == 1 );
                    ADD_HASH_FILTER( sInitExprCode );

                    break;
                }
                
            default :
                {
                    STL_DASSERT( 0 );

                    break;
                }
        }
    }

    
    STL_RAMP( RAMP_FINISH );


    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/** @} qlo */

