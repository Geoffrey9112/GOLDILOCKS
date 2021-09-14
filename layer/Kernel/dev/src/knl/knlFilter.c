/*******************************************************************************
 * knlFilter.c
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
 * @file knlFilter.c
 * @brief Kernel Layer Filter Routines
 */

#include <knl.h>

/**
 * @addtogroup knlFilter
 * @{
 */


knlColumnInReadRow knlEmtpyColumnValue[1] = { { NULL, 0 } };


/*******************************************************************************
 * 최종 버전
 ******************************************************************************/

/**
 * @brief Key Range Predicate 초기화 (공간할당)
 * @param[in]      aMaxRangeCnt      Key Range 조건 최대 개수 
 * @param[in]      aIsCreateContext  Context 영역 생성 여부
 * @param[out]     aRangePredList    range predicate
 * @param[out]     aContextInfo      context for range predicate
 * @param[in]      aMemMgr           영역 기반 메모리 할당자 (knlRegionMem)
 * @param[in,out]  aEnv              Environment
 * @remarks
 */
stlStatus knlCreateRangePred( stlInt32               aMaxRangeCnt,
                              stlBool                aIsCreateContext,
                              knlPredicateList    ** aRangePredList,
                              knlExprContextInfo  ** aContextInfo,
                              knlRegionMem         * aMemMgr,
                              knlEnv               * aEnv )
{
    knlRangeElement     * sMinRangeValue = NULL;
    knlRangeElement     * sMaxRangeValue = NULL;
    
    
    /**
     * Parameter Validation
     */

    /* STL_PARAM_VALIDATE( aMaxRangeCnt > 0, KNL_ERROR_STACK(aEnv) ); */


    /**
     * min-range : expression stack 생성
     */

    /* Range Element 공간 할당 */
    STL_TRY( knlAllocRegionMem( aMemMgr,
                                STL_SIZEOF( knlRangeElement ),
                                (void**) & sMinRangeValue,
                                aEnv )
             == STL_SUCCESS );

    stlMemset( sMinRangeValue,
               0x00,
               STL_SIZEOF( knlRangeElement ) );

    if( aMaxRangeCnt == 0 )
    {
        sMinRangeValue->mToalSize         = 0;
        sMinRangeValue->mCount            = 0;
        sMinRangeValue->mKeyColumns       = NULL;
        sMinRangeValue->mRangeValues      = NULL;
        sMinRangeValue->mIsIncludeEqual   = NULL;
        sMinRangeValue->mIsAsc            = NULL;
        sMinRangeValue->mIsNullFirst      = NULL;
        sMinRangeValue->mIsNullAlwaysStop = NULL;
    }
    else
    {
        /* Range Element의 Key Column 공간 할당 */
        STL_TRY( knlAllocRegionMem( aMemMgr,
                                    STL_SIZEOF( stlUInt32 ) * aMaxRangeCnt,
                                    (void**) & sMinRangeValue->mKeyColumns,
                                    aEnv )
                 == STL_SUCCESS );

        stlMemset( sMinRangeValue->mKeyColumns,
                   0x00,
                   STL_SIZEOF( stlUInt32 ) * aMaxRangeCnt );

        /* Range Element의 Range Value 공간 할당 */
        STL_TRY( knlAllocRegionMem( aMemMgr,
                                    STL_SIZEOF( stlUInt32 ) * aMaxRangeCnt,
                                    (void**) & sMinRangeValue->mRangeValues,
                                    aEnv )
                 == STL_SUCCESS );

        stlMemset( sMinRangeValue->mRangeValues,
                   0x00,
                   STL_SIZEOF( stlUInt32 ) * aMaxRangeCnt );

        /* Range Element의 IsIncludeEqual 공간 할당 */
        STL_TRY( knlAllocRegionMem( aMemMgr,
                                    STL_SIZEOF( stlBool ) * aMaxRangeCnt,
                                    (void**) & sMinRangeValue->mIsIncludeEqual,
                                    aEnv )
                 == STL_SUCCESS );

        stlMemset( sMinRangeValue->mIsIncludeEqual,
                   0x00,
                   STL_SIZEOF( stlBool ) * aMaxRangeCnt );

        /* Range Element의 Is Ascend Order 공간 할당 */
        STL_TRY( knlAllocRegionMem( aMemMgr,
                                    STL_SIZEOF( stlBool ) * aMaxRangeCnt,
                                    (void**) & sMinRangeValue->mIsAsc,
                                    aEnv )
                 == STL_SUCCESS );

        stlMemset( sMinRangeValue->mIsAsc,
                   0x00,
                   STL_SIZEOF( stlBool ) * aMaxRangeCnt );

        /* Range Element의 Is Nulls First 공간 할당 */
        STL_TRY( knlAllocRegionMem( aMemMgr,
                                    STL_SIZEOF( stlBool ) * aMaxRangeCnt,
                                    (void**) & sMinRangeValue->mIsNullFirst,
                                    aEnv )
                 == STL_SUCCESS );

        stlMemset( sMinRangeValue->mIsNullFirst,
                   0x00,
                   STL_SIZEOF( stlBool ) * aMaxRangeCnt );

        /* Range Element의 Is Null Always Stop 공간 할당 */
        STL_TRY( knlAllocRegionMem( aMemMgr,
                                    STL_SIZEOF( stlBool ) * aMaxRangeCnt,
                                    (void**) & sMinRangeValue->mIsNullAlwaysStop,
                                    aEnv )
                 == STL_SUCCESS );

        stlMemset( sMinRangeValue->mIsNullAlwaysStop,
                   0x00,
                   STL_SIZEOF( stlBool ) * aMaxRangeCnt );

        /* 초기화 */
        sMinRangeValue->mToalSize    = aMaxRangeCnt;
        sMinRangeValue->mCount       = 0;
    }
    
    
    /**
     * max-range : expression stack 생성
     */

    STL_TRY( knlAllocRegionMem( aMemMgr,
                                STL_SIZEOF( knlRangeElement ),
                                (void**) & sMaxRangeValue,
                                aEnv )
             == STL_SUCCESS );

    stlMemset( sMaxRangeValue,
               0x00,
               STL_SIZEOF( knlRangeElement ) );

    if( aMaxRangeCnt == 0 )
    {
        sMaxRangeValue->mToalSize         = 0;
        sMaxRangeValue->mCount            = 0;
        sMaxRangeValue->mKeyColumns       = NULL;
        sMaxRangeValue->mRangeValues      = NULL;
        sMaxRangeValue->mIsIncludeEqual   = NULL;
        sMaxRangeValue->mIsAsc            = NULL;
        sMaxRangeValue->mIsNullFirst      = NULL;
        sMaxRangeValue->mIsNullAlwaysStop = NULL;
    }
    else
    {
        /* Range Element 공간 할당 */
        STL_TRY( knlAllocRegionMem( aMemMgr,
                                    STL_SIZEOF( knlRangeElement ),
                                    (void**) & sMaxRangeValue,
                                    aEnv )
                 == STL_SUCCESS );

        stlMemset( sMaxRangeValue,
                   0x00,
                   STL_SIZEOF( knlRangeElement ) );

        /* Range Element의 Key Column 공간 할당 */
        STL_TRY( knlAllocRegionMem( aMemMgr,
                                    STL_SIZEOF( stlUInt32 ) * aMaxRangeCnt,
                                    (void**) & sMaxRangeValue->mKeyColumns,
                                    aEnv )
                 == STL_SUCCESS );

        stlMemset( sMaxRangeValue->mKeyColumns,
                   0x00,
                   STL_SIZEOF( stlUInt32 ) * aMaxRangeCnt );

        /* Range Element의 Range Value 공간 할당 */
        STL_TRY( knlAllocRegionMem( aMemMgr,
                                    STL_SIZEOF( stlUInt32 ) * aMaxRangeCnt,
                                    (void**) & sMaxRangeValue->mRangeValues,
                                    aEnv )
                 == STL_SUCCESS );

        stlMemset( sMaxRangeValue->mRangeValues,
                   0x00,
                   STL_SIZEOF( stlUInt32 ) * aMaxRangeCnt );

        /* Range Element의 IsIncludeEqual 공간 할당 */
        STL_TRY( knlAllocRegionMem( aMemMgr,
                                    STL_SIZEOF( stlBool ) * aMaxRangeCnt,
                                    (void**) & sMaxRangeValue->mIsIncludeEqual,
                                    aEnv )
                 == STL_SUCCESS );

        stlMemset( sMaxRangeValue->mIsIncludeEqual,
                   0x00,
                   STL_SIZEOF( stlBool ) * aMaxRangeCnt );

        /* Range Element의 Is Ascend Order 공간 할당 */
        STL_TRY( knlAllocRegionMem( aMemMgr,
                                    STL_SIZEOF( stlBool ) * aMaxRangeCnt,
                                    (void**) & sMaxRangeValue->mIsAsc,
                                    aEnv )
                 == STL_SUCCESS );

        stlMemset( sMaxRangeValue->mIsAsc,
                   0x00,
                   STL_SIZEOF( stlBool ) * aMaxRangeCnt );

        /* Range Element의 Is Nulls First 공간 할당 */
        STL_TRY( knlAllocRegionMem( aMemMgr,
                                    STL_SIZEOF( stlBool ) * aMaxRangeCnt,
                                    (void**) & sMaxRangeValue->mIsNullFirst,
                                    aEnv )
                 == STL_SUCCESS );

        stlMemset( sMaxRangeValue->mIsNullFirst,
                   0x00,
                   STL_SIZEOF( stlBool ) * aMaxRangeCnt );
        
        /* Range Element의 Is Null Always Stop 공간 할당 */
        STL_TRY( knlAllocRegionMem( aMemMgr,
                                    STL_SIZEOF( stlBool ) * aMaxRangeCnt,
                                    (void**) & sMaxRangeValue->mIsNullAlwaysStop,
                                    aEnv )
                 == STL_SUCCESS );

        stlMemset( sMaxRangeValue->mIsNullAlwaysStop,
                   0x00,
                   STL_SIZEOF( stlBool ) * aMaxRangeCnt );

        /* 초기화 */
        sMaxRangeValue->mToalSize    = aMaxRangeCnt;
        sMaxRangeValue->mCount       = 0;
    }
    
    
    /**
     * Context Info 공간할당
     */

    if( ( aIsCreateContext == STL_TRUE ) && ( aMaxRangeCnt > 0 ) )
    {
        STL_TRY( knlCreateContextInfo( aMaxRangeCnt * 2,
                                       aContextInfo,
                                       aMemMgr,
                                       aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        *aContextInfo = NULL;
    }


    /**
     * range predicate list 설정
     */

    STL_TRY( knlMakeRange( sMinRangeValue,
                           sMaxRangeValue,
                           NULL,
                           aRangePredList,
                           aMemMgr,
                           aEnv )
             == STL_SUCCESS );

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Range Predicate에 range 정보 추가 
 * @param[in]      aIsMinRange        대상이 Min Range인지 여부 
 * @param[in]      aKeyColOffset      관련 Key Column의 context offset
 * @param[in]      aRangeValOffset    Constant Value의 context offset
 * @param[in]      aIsIncludeEqual    Equal 연산을 포함하는지 여부 
 * @param[in]      aIsAsc             Key Column Order가 Asc인지 여부
 * @param[in]      aIsNullFirst       Key Column이 Nulls First인지 여부
 * @param[in]      aIsNullAlwaysStop  NULL 상수에 대해 STOP 이 필요한 연산자인지의 여부 
 * @param[in,out]  aRangePredList     Range Predicate List (OR-list)
 * @param[out]     aEnv               커널 Environment
 *
 * @remark 반드시 range 수행 순서 역순으로 구성할 것!!!
 */
stlStatus knlAddRange( stlBool             aIsMinRange,
                       stlUInt32           aKeyColOffset,
                       stlUInt32           aRangeValOffset,
                       stlBool             aIsIncludeEqual,
                       stlBool             aIsAsc,
                       stlBool             aIsNullFirst,
                       stlBool             aIsNullAlwaysStop,
                       knlPredicateList  * aRangePredList,
                       knlEnv            * aEnv )
{
    knlRangeElement    * sRangeElem  = NULL;
    
    STL_PARAM_VALIDATE( aKeyColOffset >= 0, KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRangeValOffset >= 0, KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRangePredList != NULL, KNL_ERROR_STACK(aEnv) );

    if( aIsMinRange == STL_TRUE )
    {
        sRangeElem = KNL_PRED_GET_MIN_RANGE( aRangePredList );
    }
    else
    {
        sRangeElem = KNL_PRED_GET_MAX_RANGE( aRangePredList );
    }

    STL_PARAM_VALIDATE( sRangeElem->mToalSize > sRangeElem->mCount,
                        KNL_ERROR_STACK(aEnv) );

    /* Is Include Equal 설정 */
    sRangeElem->mKeyColumns[sRangeElem->mCount]       = aKeyColOffset;
    sRangeElem->mRangeValues[sRangeElem->mCount]      = aRangeValOffset;
    sRangeElem->mIsIncludeEqual[sRangeElem->mCount]   = aIsIncludeEqual;
    sRangeElem->mIsAsc[sRangeElem->mCount]            = aIsAsc;
    sRangeElem->mIsNullFirst[sRangeElem->mCount]      = aIsNullFirst;
    sRangeElem->mIsNullAlwaysStop[sRangeElem->mCount] = aIsNullAlwaysStop;


    /* 사용 element 개수 증가 */
    sRangeElem->mCount++;

    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Range Predicate 생성
 * @param[in]      aMinRangeValue   Min Range Expression Stack
 * @param[in]      aMaxRangeValue   Max Range Expression Stack
 * @param[in]      aKeyFilter       Key Filter Expression Stack
 * @param[out]     aRangePredList   Range Predicate List (OR-list)
 * @param[in]      aMemMgr          영역 기반 메모리 할당자 (knlRegionMem)
 * @param[out]     aEnv             커널 Environment
 */
stlStatus knlMakeRange( knlRangeElement      * aMinRangeValue,
                        knlRangeElement      * aMaxRangeValue,
                        knlExprStack         * aKeyFilter,
                        knlPredicateList    ** aRangePredList,
                        knlRegionMem         * aMemMgr,
                        knlEnv               * aEnv )
{
    knlPredicateList   * sPredList = NULL;
    knlPredicate       * sPred     = NULL;
    knlRangePredicate  * sRange    = NULL;

    /**
     * Range Predicate List 공간 할당
     */

    STL_TRY( knlAllocRegionMem( aMemMgr,
                                STL_SIZEOF( knlPredicateList ),
                                (void**) & sPredList,
                                aEnv )
             == STL_SUCCESS );

    stlMemset( sPredList,
               0x00,
               STL_SIZEOF( knlPredicateList ) );
    

    /**
     * Range 설정
     */
        
    STL_TRY( knlAllocRegionMem( aMemMgr,
                                STL_SIZEOF( knlRangePredicate ),
                                (void**) & sRange,
                                aEnv )
             == STL_SUCCESS );

    stlMemset( sRange,
               0x00,
               STL_SIZEOF( knlRangePredicate ) );

    sRange->mMinRange    = aMinRangeValue;
    sRange->mMaxRange    = aMaxRangeValue;
    sRange->mKeyFilter   = aKeyFilter;


    /**
     * Predicate 설정
     */

    sPred = & sPredList->mPredicate;
    sPred->mType = KNL_CONDITION_TYPE_RANGE;
    sPred->mCond.mRange = sRange;

    
    /**
     * Predicate List 설정
     */

    sPredList->mNext = NULL;
    

    /**
     * OUTPUT 설정
     */
    
    *aRangePredList = sPredList;


    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Filter Predicate 생성
 * @param[in]      aLogicalFilter   Logical Filter Expression Stack
 * @param[in,out]  aFilterPredList  Filter Predicate List
 * @param[in]      aMemMgr          영역 기반 메모리 할당자 (knlRegionMem)
 * @param[out]     aEnv             커널 Environment
 */
stlStatus knlMakeFilter( knlExprStack         * aLogicalFilter,
                         knlPredicateList    ** aFilterPredList,
                         knlRegionMem         * aMemMgr,  
                         knlEnv               * aEnv )
{
    knlPredicateList  * sPredList = NULL;
    knlPredicate      * sPred     = NULL;
    
    if( aLogicalFilter == NULL )
    {
        sPredList = NULL;
    }
    else
    {
        /**
         * Filter Predicate List 공간 할당
         */

        STL_TRY( knlAllocRegionMem( aMemMgr,
                                    STL_SIZEOF( knlPredicateList ),
                                    (void**) & sPredList,
                                    aEnv )
                 == STL_SUCCESS );

        stlMemset( sPredList,
                   0x00,
                   STL_SIZEOF( knlPredicateList ) );
    

        /**
         * Filter Predicate 설정
         */

        sPred = & sPredList->mPredicate;
        
        sPred->mType         = KNL_CONDITION_TYPE_FILTER;
        sPred->mCond.mFilter = aLogicalFilter;


        /**
         * Filter Predicate List 설정
         */

        sPredList->mNext = NULL;
    }
    

    /**
     * OUTPUT 설정
     */
    
    *aFilterPredList = sPredList;


    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Range Element로부터 Compare List 구성하기
 * @param[in]      aRangeElem     Range Element
 * @param[in]      aContextInfo   Context Info
 * @param[in]      aMemMgr        영역 기반 메모리 할당자 (knlRegionMem)
 * @param[out]     aCompList      Range Element에 대한 Compare List 
 * @param[in,out]  aEnv           커널 Environment
 */
stlStatus knlGetCompareListFromRangeElem( knlRangeElement      * aRangeElem,
                                          knlExprContextInfo   * aContextInfo,
                                          knlRegionMem         * aMemMgr,
                                          knlCompareList      ** aCompList,
                                          knlEnv               * aEnv )
{
    knlRangeElement  * sRangeElem = aRangeElem;
    knlCompareList   * sCompList;
    knlCompareList   * sTemp;
    knlValueBlock    * sValueBlock;
    dtlDataValue     * sColValuePtr;
    dtlDataValue     * sDataValue;
    stlInt32           sLoop;
    stlInt32           sColOrder;
    dtlDataType        sColDataType;

    if( sRangeElem == NULL )
    {
        sCompList = NULL;
    }
    else if( sRangeElem->mCount == 0 )
    {
        sCompList = NULL;
    }
    else
    {
        /**
         * Parameter Validation
         */
        
        STL_PARAM_VALIDATE( aContextInfo != NULL, KNL_ERROR_STACK( aEnv ) );
        STL_PARAM_VALIDATE( aMemMgr != NULL, KNL_ERROR_STACK( aEnv ) );
        STL_PARAM_VALIDATE( aCompList != NULL, KNL_ERROR_STACK( aEnv ) );


        /**
         * Compare List 공간 할당 
         */

        STL_TRY( knlAllocRegionMem( aMemMgr,
                                    STL_SIZEOF( knlCompareList ) * sRangeElem->mCount,
                                    (void**) & sCompList,
                                    aEnv )
                 == STL_SUCCESS );

        stlMemset( sCompList,
                   0x00,
                   STL_SIZEOF( knlCompareList ) * sRangeElem->mCount );

        sTemp = sCompList;

        
        /**
         * Compare List 설정
         */

        sColValuePtr = NULL;
        sColOrder = -1;
        
        for( sLoop = 0 ; sLoop < sRangeElem->mCount ; sLoop++ )
        {
            sValueBlock = aContextInfo->mContexts[ sRangeElem->mKeyColumns[ sLoop]  ].mInfo.mValueInfo;

            if( sColValuePtr != sValueBlock->mDataValue )
            {
                sColValuePtr = sValueBlock->mDataValue;
                sColOrder++;
            }
            else
            {
                /* Do Nothing */
            }

            sTemp->mColOrder       = sColOrder;
            sColDataType           = sValueBlock->mDataValue->mType;

            if( aContextInfo->mContexts[ sRangeElem->mRangeValues[ sLoop ] ].mCast == NULL )
            {
                sValueBlock = aContextInfo->mContexts[ sRangeElem->mRangeValues[ sLoop ] ].mInfo.mValueInfo;
                sDataValue  = & sValueBlock->mDataValue[0];
            }
            else
            {
                sDataValue = & aContextInfo->mContexts[ sRangeElem->mRangeValues[ sLoop ] ].mCast->mCastResultBuf;
            }
                
            sTemp->mRangeVal       = sDataValue->mValue;
            sTemp->mRangeLen       = sDataValue->mLength;
            sTemp->mConstVal       = sDataValue;
            sTemp->mCompFunc       = dtlPhysicalCompareFuncList[ sColDataType ][ sDataValue->mType ];
            sTemp->mIsLikeFunc     = STL_FALSE;
            sTemp->mCompNullStop   = sRangeElem->mIsNullAlwaysStop[ sLoop ];

            sTemp->mIsIncludeEqual = sRangeElem->mIsIncludeEqual[ sLoop ];
            sTemp->mIsAsc          = sRangeElem->mIsAsc[ sLoop ];
            sTemp->mIsNullFirst    = sRangeElem->mIsNullFirst[ sLoop ];
            sTemp->mIsDiffTypeCmp  = ( sColDataType != sDataValue->mType );

            sTemp->mNext = sTemp + 1;

            sTemp++;
        }

        sCompList[ sLoop - 1].mNext = NULL;
    }
  
    *aCompList = sCompList;

    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Key Compare List을 생성한다.
 * @param[in]      aIndexColCount    Index Key Column Count
 * @param[out]     aKeyCompareList   Key Compare List
 * @param[in]      aRegionMem        영역 기반 메모리 할당자 (knlRegionMem)
 * @param[in,out]  aEnv              Environment
 * @remarks
 */
stlStatus knlCreateKeyCompareList( stlInt32               aIndexColCount,
                                   knlKeyCompareList   ** aKeyCompareList,
                                   knlRegionMem         * aRegionMem,
                                   knlEnv               * aEnv )
{
    knlKeyCompareList   * sKeyCompList = NULL;

    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aIndexColCount > 0, KNL_ERROR_STACK( aEnv ) );


    /**
     * knlKeyCompareList 생성
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( knlKeyCompareList ),
                                (void **) & sKeyCompList,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );


    /**
     * Index Key Column Count 설정
     */
    
    sKeyCompList->mIndexColCount = aIndexColCount;
    

    /**
     * TableColOrder 공간 할당 및 초기화
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( stlInt32 ) * aIndexColCount,
                                (void **) & sKeyCompList->mTableColOrder,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sKeyCompList->mTableColOrder,
               0x00,
               STL_SIZEOF( stlInt32 ) * aIndexColCount );

    
    /**
     * IndexColOffset 공간 할당 및 초기화
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( stlInt32 ) * aIndexColCount,
                                (void **) & sKeyCompList->mIndexColOffset,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sKeyCompList->mIndexColOffset,
               0x00,
               STL_SIZEOF( stlInt32 ) * aIndexColCount );


    /**
     * IndexColVal 공간 할당 및 초기화
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( void* ) * aIndexColCount,
                                (void **) & sKeyCompList->mIndexColVal,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sKeyCompList->mIndexColVal,
               0x00,
               STL_SIZEOF( void* ) * aIndexColCount );


    /**
     * IndexColLen 공간 할당 및 초기화
     */
    
    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( stlInt64 ) * aIndexColCount,
                                (void **) & sKeyCompList->mIndexColLen,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sKeyCompList->mIndexColLen,
               0x00,
               STL_SIZEOF( stlInt64 ) * aIndexColCount );


    /**
     * CompFunc 공간 할당 및 초기화
     */
    
    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( dtlPhysicalFunc* ) * aIndexColCount,
                                (void **) & sKeyCompList->mCompFunc,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sKeyCompList->mCompFunc,
               0x00,
               STL_SIZEOF( dtlPhysicalFunc* ) * aIndexColCount );
    

    /**
     * ValueList 초기화
     */

    sKeyCompList->mValueList = NULL;
    

    /**
     * OUTPUT 설정
     */
    
    *aKeyCompareList = sKeyCompList;
    
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/** @} */



