/*******************************************************************************
 * knlValueBlock.c
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
 * @file knlValueBlock.c
 * @brief Block Read를 위한 Value Block 관리 루틴
 */

#include <knl.h>



/**
 * @addtogroup knlValueBlock
 * @{
 */


/**
 * @brief Value Block List 를 초기화한다.
 * @param[out]  aValueBlockList    Value Block List
 * @param[in]   aBlockCnt          Block Read Count
 * @param[in]   aIsSepBuff         각 Value 마다 공간을 확보할 지의 여부          
 *                            <BR> - 컬럼인 경우, aBlockCnt 만큼 Data Buffer 공간을 확보
 *                            <BR> - 상수인 경우, aBlockCnt 와 관계없이 Data Buffer를 공유
 * @param[in]   aAllocLayer        Block이 할당되는 Layer
 * @param[in]   aTableID           Table ID (컬럼이 아닌경우, 무시됨)
 * @param[in]   aColumnOrder       컬럼의 Table 내 위치 (컬럼이 아닌경우, 무시됨)
 * @param[in]   aDBType            DB Data Type
 * @param[in]   aArgNum1           Precision (타입마다 용도가 다름)
 *                            <BR> - time,timetz,timestamp,timestamptz타입은
 *                            <BR>   fractional second precision 정보
                              <BR> - interval 타입은 leading precision 정보
                              <BR> - 그외 타입은 precision 정보
 * @param[in]   aArgNum2           scale (타입마다 용도가 다름)
 *                            <BR> - interval 타입은 fractional second precision 정보
                              <BR> - 그외 타입은 scale 정보 
 * @param[in]   aStringLengthUnit  string length unit (dtlStringLengthUnit 참조)
 * @param[in]   aIntervalIndicator interval indicator (dtlIntervalIndicator 참조) 
 * @param[in]   aRegionMem         Region Memory
 * @param[in]   aEnv               Environment
 * @remarks
 */
stlStatus knlInitBlock( knlValueBlockList ** aValueBlockList,
                        stlInt32             aBlockCnt,
                        stlBool              aIsSepBuff,
                        stlLayerClass        aAllocLayer,
                        stlInt64             aTableID,
                        stlInt32             aColumnOrder,
                        dtlDataType          aDBType,
                        stlInt64             aArgNum1,
                        stlInt64             aArgNum2,
                        dtlStringLengthUnit  aStringLengthUnit,
                        dtlIntervalIndicator aIntervalIndicator,
                        knlRegionMem       * aRegionMem,
                        knlEnv             * aEnv )
{
    knlValueBlockList * sBlockList  = NULL;
    knlValueBlock     * sValueBlock = NULL;
    dtlDataValue      * sDataValue  = NULL;

    stlInt64            sBufferSize = 0;
    stlInt32            sBlockCnt = 0;
    stlInt32            sState = 0;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aValueBlockList != NULL, KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aBlockCnt > 0, KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aDBType >= DTL_TYPE_BOOLEAN) &&
                        (aDBType < DTL_TYPE_MAX)
                        , KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, KNL_ERROR_STACK(aEnv) );

    /**
     * 상수 유형인지 변수 유형인지에 따른 Block 개수 조정
     */

    if ( aIsSepBuff == STL_TRUE )
    {
        sBlockCnt = aBlockCnt;
    }
    else
    {
        sBlockCnt = 1;
    }
    
    /**
     * Block Count 만큼 Data Value 정보 설정 
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF(dtlDataValue) * sBlockCnt,
                                (void **) & sDataValue,
                                aEnv )
             == STL_SUCCESS );

    /**
     * Data Buffer 공간 할당
     */

    STL_TRY( knlAllocDataValueArray( sDataValue,
                                     aDBType,
                                     aArgNum1,
                                     aStringLengthUnit,
                                     sBlockCnt,
                                     aRegionMem,
                                     & sBufferSize,
                                     KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    sState = 1;

    /**
     * Value Block 정보 설정
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF(knlValueBlock),
                                (void **) & sValueBlock,
                                aEnv )
             == STL_SUCCESS );
    stlMemset( sValueBlock, 0x00, STL_SIZEOF(knlValueBlock) );

    sValueBlock->mIsSepBuff         = aIsSepBuff;
    sValueBlock->mAllocBlockCnt     = aBlockCnt;  /* Original 값을 설정함 */
    sValueBlock->mAllocLayer        = aAllocLayer;
    sValueBlock->mUsedBlockCnt      = 0;
    sValueBlock->mSkipBlockCnt      = 0;
    sValueBlock->mArgNum1           = aArgNum1;
    sValueBlock->mArgNum2           = aArgNum2;
    sValueBlock->mStringLengthUnit  = aStringLengthUnit;
    sValueBlock->mIntervalIndicator = aIntervalIndicator;
    sValueBlock->mAllocBufferSize   = sBufferSize;
    sValueBlock->mDataValue         = sDataValue;

    /**
     * Block List 정보 설정
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF(knlValueBlockList),
                                (void **) & sBlockList,
                                aEnv )
             == STL_SUCCESS );
    stlMemset( sBlockList, 0x00, STL_SIZEOF(knlValueBlockList) );

    if ( aIsSepBuff == STL_TRUE )
    {
       sBlockList->mTableID     = aTableID;
       sBlockList->mColumnOrder = aColumnOrder;
    }
    else
    {
       sBlockList->mTableID     = KNL_ANONYMOUS_TABLE_ID;
       sBlockList->mColumnOrder = 0;
    }

    sBlockList->mValueBlock = sValueBlock;
    sBlockList->mNext       = NULL;
    
    /**
     * Output 설정
     */

    *aValueBlockList = sBlockList;
    
    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void) knlFreeLongDataValueArray( sDataValue, sBlockCnt, aEnv );
    }

    return STL_FAILURE;
}


/**
 * @brief Constant Data Value에 대한 Value Block List 를 초기화한다.
 * @param[out]  aValueBlockList    Value Block List
 * @param[in]   aDataValue         이미 구성된 Constant Data Value
 * @param[in]   aDBType            DB Data Type
 * @param[in]   aAllocLayer        Block이 할당되는 Layer
 * @param[in]   aArgNum1           Precision (타입마다 용도가 다름)
 *                            <BR> - time,timetz,timestamp,timestamptz타입은
 *                            <BR>   fractional second precision 정보
                              <BR> - interval 타입은 leading precision 정보
                              <BR> - 그외 타입은 precision 정보
 * @param[in]   aArgNum2           scale (타입마다 용도가 다름)
 *                            <BR> - interval 타입은 fractional second precision 정보
                              <BR> - 그외 타입은 scale 정보 
 * @param[in]   aStringLengthUnit  string length unit (dtlStringLengthUnit 참조)
 * @param[in]   aIntervalIndicator interval indicator (dtlIntervalIndicator 참조) 
 * @param[in]   aRegionMem         Region Memory
 * @param[in]   aEnv               Environment
 * @remarks
 */
stlStatus knlInitBlockWithDataValue( knlValueBlockList ** aValueBlockList,
                                     dtlDataValue       * aDataValue,
                                     dtlDataType          aDBType,
                                     stlLayerClass        aAllocLayer,
                                     stlInt64             aArgNum1,
                                     stlInt64             aArgNum2,
                                     dtlStringLengthUnit  aStringLengthUnit,
                                     dtlIntervalIndicator aIntervalIndicator,
                                     knlRegionMem       * aRegionMem,
                                     knlEnv             * aEnv )
{
    knlValueBlockList * sBlockList  = NULL;
    knlValueBlock     * sValueBlock = NULL;

    stlInt64    sLength     = 0;
    stlInt64    sAllocSize  = 0;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aValueBlockList != NULL, KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aDBType >= DTL_TYPE_BOOLEAN) &&
                        (aDBType < DTL_TYPE_MAX),
                        KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, KNL_ERROR_STACK(aEnv) );
    
    /**
     * Value Buffer Size 얻기
     * - 실제 할당은 받지 않음 
     */
    
    STL_TRY( dtlGetDataValueBufferSize( aDBType,
                                        aArgNum1,
                                        aStringLengthUnit,
                                        & sLength,
                                        KNL_DT_VECTOR(aEnv),
                                        aEnv,
                                        KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );

    STL_DASSERT( sLength > 0 );

    sAllocSize = STL_ALIGN_DEFAULT( sLength );
    
    /**
     * Value Block 정보 설정
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF(knlValueBlock),
                                (void **) & sValueBlock,
                                aEnv )
             == STL_SUCCESS );
    stlMemset( sValueBlock, 0x00, STL_SIZEOF(knlValueBlock) );

    sValueBlock->mIsSepBuff         = STL_FALSE;
    sValueBlock->mAllocLayer        = aAllocLayer;
    sValueBlock->mAllocBlockCnt     = 1;  /* Original 값을 설정함 */
    sValueBlock->mUsedBlockCnt      = 0;
    sValueBlock->mSkipBlockCnt      = 0;
    sValueBlock->mArgNum1           = aArgNum1;
    sValueBlock->mArgNum2           = aArgNum2;
    sValueBlock->mStringLengthUnit  = aStringLengthUnit;
    sValueBlock->mIntervalIndicator = aIntervalIndicator;
    sValueBlock->mAllocBufferSize   = sAllocSize;
    sValueBlock->mDataValue         = aDataValue;

    /**
     * Block List 정보 설정
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF(knlValueBlockList),
                                (void **) & sBlockList,
                                aEnv )
             == STL_SUCCESS );
    stlMemset( sBlockList, 0x00, STL_SIZEOF(knlValueBlockList) );

    sBlockList->mTableID     = KNL_ANONYMOUS_TABLE_ID;
    sBlockList->mColumnOrder = 0;
    sBlockList->mValueBlock  = sValueBlock;
    sBlockList->mNext        = NULL;
    
    /**
     * Output 설정
     */

    *aValueBlockList = sBlockList;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Value Block List 를 공유하여 초기화한한다.
 * @param[out]  aValueBlockList   Target Value Block List
 * @param[in]   aSourceBlockList  참조할 Source Value Block List
 * @param[in]   aRegionMem        Region Memory
 * @param[in]   aEnv              Environment
 * @remarks
 */
stlStatus knlInitShareBlock( knlValueBlockList ** aValueBlockList,
                             knlValueBlockList  * aSourceBlockList,
                             knlRegionMem       * aRegionMem,
                             knlEnv             * aEnv )
{
    knlValueBlockList * sBlockList  = NULL;

    /*
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aValueBlockList != NULL, KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSourceBlockList != NULL, KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, KNL_ERROR_STACK(aEnv) );

    /**
     * Block List 정보 설정
     * - Value Block 을 공유한다.
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF(knlValueBlockList),
                                (void **) & sBlockList,
                                aEnv )
             == STL_SUCCESS );
    stlMemset( sBlockList, 0x00, STL_SIZEOF(knlValueBlockList) );

    sBlockList->mTableID     = aSourceBlockList->mTableID;
    sBlockList->mColumnOrder = aSourceBlockList->mColumnOrder;
    sBlockList->mValueBlock  = aSourceBlockList->mValueBlock;
    sBlockList->mNext        = NULL;

    /**
     * Output 설정
     */

    *aValueBlockList = sBlockList;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Value Block을 공유하여 Value Block List를 초기화한한다.
 * @param[out]  aValueBlockList   Target Value Block List
 * @param[in]   aSourceBlock      참조할 Source Value Block
 * @param[in]   aRegionMem        Region Memory
 * @param[in]   aEnv              Environment
 * @remarks
 */
stlStatus knlInitShareBlockFromBlock( knlValueBlockList ** aValueBlockList,
                                      knlValueBlock      * aSourceBlock,
                                      knlRegionMem       * aRegionMem,
                                      knlEnv             * aEnv )
{
    knlValueBlockList * sBlockList  = NULL;

    /*
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aValueBlockList != NULL, KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSourceBlock != NULL, KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, KNL_ERROR_STACK(aEnv) );

    /**
     * Block List 정보 설정
     * - Value Block 을 공유한다.
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF(knlValueBlockList),
                                (void **) & sBlockList,
                                aEnv )
             == STL_SUCCESS );
    stlMemset( sBlockList, 0x00, STL_SIZEOF(knlValueBlockList) );

    sBlockList->mTableID     = KNL_ANONYMOUS_TABLE_ID;
    sBlockList->mColumnOrder = 0;
    sBlockList->mValueBlock  = aSourceBlock;
    sBlockList->mNext        = NULL;

    /**
     * Output 설정
     */

    *aValueBlockList = sBlockList;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Value Block List 정보를 이용해 새로운 Value Block을 초기화한한다.
 * @param[out]  aValueBlockList   Target Value Block List
 * @param[in]   aAllocLayer       Block이 할당되는 Layer
 * @param[in]   aBlockCnt         Block Read Count
 * @param[in]   aSourceBlockList  참조할 Source Value Block List
 * @param[in]   aRegionMem        Region Memory
 * @param[in]   aEnv              Environment
 * @remarks
 */
stlStatus knlInitCopyBlock( knlValueBlockList ** aValueBlockList,
                            stlLayerClass        aAllocLayer,
                            stlInt32             aBlockCnt,
                            knlValueBlockList  * aSourceBlockList,
                            knlRegionMem       * aRegionMem,
                            knlEnv             * aEnv )
{
    knlValueBlockList * sBlockList  = NULL;
    knlValueBlock     * sValueBlock = NULL;
    dtlDataValue      * sDataValue  = NULL;

    stlInt64            sBufferSize = 0;
    stlInt32            sState = 0;
    stlInt32            sBlockCnt = 0;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aValueBlockList != NULL, KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aBlockCnt > 0, KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSourceBlockList != NULL, KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, KNL_ERROR_STACK(aEnv) );

    /**
     * 상수 유형인지 변수 유형인지에 따른 Block 개수 조정
     */

    if ( KNL_GET_BLOCK_IS_SEP_BUFF(aSourceBlockList) == STL_TRUE )
    {
        sBlockCnt = aBlockCnt;
    }
    else
    {
        sBlockCnt = 1;
    }
    
    /**
     * Block Count 만큼 Data Value 정보 설정 
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF(dtlDataValue) * sBlockCnt,
                                (void **) & sDataValue,
                                aEnv )
             == STL_SUCCESS );
    stlMemset( sDataValue, 0x00, STL_SIZEOF(dtlDataValue) * sBlockCnt );

    /**
     * Data Buffer 공간 할당
     */

    STL_TRY( knlAllocDataValueArray( sDataValue,
                                     KNL_GET_BLOCK_DB_TYPE( aSourceBlockList ),
                                     KNL_GET_BLOCK_ARG_NUM1( aSourceBlockList ),
                                     KNL_GET_BLOCK_STRING_LENGTH_UNIT( aSourceBlockList ),
                                     sBlockCnt,
                                     aRegionMem,
                                     & sBufferSize,
                                     KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    sState = 1;

    /**
     * Value Block 정보 설정
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF(knlValueBlock),
                                (void **) & sValueBlock,
                                aEnv )
             == STL_SUCCESS );
    stlMemset( sValueBlock, 0x00, STL_SIZEOF(knlValueBlock) );

    STL_DASSERT( KNL_GET_BLOCK_ALLOC_LAYER( aSourceBlockList ) == aAllocLayer );
    
    sValueBlock->mIsSepBuff         = KNL_GET_BLOCK_IS_SEP_BUFF(aSourceBlockList);
    sValueBlock->mAllocLayer        = aAllocLayer;
    sValueBlock->mAllocBlockCnt     = aBlockCnt;  /* Original Block Cnt */
    sValueBlock->mUsedBlockCnt      = 0;
    sValueBlock->mSkipBlockCnt      = 0;
    sValueBlock->mArgNum1           = KNL_GET_BLOCK_ARG_NUM1(aSourceBlockList);
    sValueBlock->mArgNum2           = KNL_GET_BLOCK_ARG_NUM2(aSourceBlockList);  
    sValueBlock->mStringLengthUnit  = KNL_GET_BLOCK_STRING_LENGTH_UNIT(aSourceBlockList);
    sValueBlock->mIntervalIndicator = KNL_GET_BLOCK_INTERVAL_INDICATOR(aSourceBlockList);
    sValueBlock->mAllocBufferSize   = sBufferSize;
    sValueBlock->mDataValue         = sDataValue;    

    /**
     * Block List 정보 설정
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF(knlValueBlockList),
                                (void **) & sBlockList,
                                aEnv )
             == STL_SUCCESS );
    stlMemset( sBlockList, 0x00, STL_SIZEOF(knlValueBlockList) );

    if ( KNL_GET_BLOCK_IS_SEP_BUFF(aSourceBlockList) == STL_TRUE )
    {
        sBlockList->mTableID     = aSourceBlockList->mTableID;
        sBlockList->mColumnOrder = aSourceBlockList->mColumnOrder;
    }
    else
    {
        sBlockList->mTableID     = KNL_ANONYMOUS_TABLE_ID;
        sBlockList->mColumnOrder = 0;
    }

    sBlockList->mValueBlock  = sValueBlock;
    sBlockList->mNext        = NULL;
    
    /**
     * Output 설정
     */

    *aValueBlockList = sBlockList;
    
    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void) knlFreeLongDataValueArray( sDataValue, sBlockCnt, aEnv );
    }

    return STL_FAILURE;
}


/**
 * @brief Data Buffer 공간 없이 Value Block 을 생성한다.
 * @param[out]  aValueBlockList   Value Block List
 * @param[in]   aAllocLayer       Block이 할당되는 Layer
 * @param[in]   aBlockCnt         Block Count
 * @param[in]   aDBType           DB Type
 * @param[in]   aRegionMem        Region Memory
 * @param[in]   aEnv              Environment
 * @remarks
 * Storage Manager에서만 사용한다.
 */
stlStatus knlInitBlockNoBuffer( knlValueBlockList ** aValueBlockList,
                                stlLayerClass        aAllocLayer,
                                stlInt32             aBlockCnt,
                                dtlDataType          aDBType,
                                knlRegionMem       * aRegionMem,
                                knlEnv             * aEnv )
{
    stlInt32            i = 0;
    
    knlValueBlockList * sBlockList  = NULL;
    knlValueBlock     * sValueBlock = NULL;
    dtlDataValue      * sDataValue  = NULL;

    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aValueBlockList != NULL, KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aBlockCnt > 0, KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aDBType >= DTL_TYPE_BOOLEAN) &&
                        (aDBType < DTL_TYPE_MAX)
                        , KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aDBType != DTL_TYPE_LONGVARCHAR) &&
                        (aDBType != DTL_TYPE_LONGVARBINARY)
                        , KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, KNL_ERROR_STACK(aEnv) );
    
    /**
     * Block Count 만큼 Data Value 정보 설정 
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF(dtlDataValue) * aBlockCnt,
                                (void **) & sDataValue,
                                aEnv )
             == STL_SUCCESS );
    stlMemset( sDataValue, 0x00, STL_SIZEOF(dtlDataValue) * aBlockCnt );

    for ( i = 0; i < aBlockCnt; i++ )
    {
        sDataValue[i].mType = aDBType;
        sDataValue[i].mBufferSize = 0;
        sDataValue[i].mLength = 0;
        sDataValue[i].mValue = NULL;
    }

    /**
     * Value Block 정보 설정
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF(knlValueBlock),
                                (void **) & sValueBlock,
                                aEnv )
             == STL_SUCCESS );
    stlMemset( sValueBlock, 0x00, STL_SIZEOF(knlValueBlock) );

    sValueBlock->mIsSepBuff         = STL_TRUE;
    sValueBlock->mAllocLayer        = aAllocLayer;
    sValueBlock->mAllocBlockCnt     = aBlockCnt;
    sValueBlock->mUsedBlockCnt      = 0;
    sValueBlock->mSkipBlockCnt      = 0;
    sValueBlock->mArgNum1           = DTL_PRECISION_NA;
    sValueBlock->mArgNum2           = DTL_SCALE_NA;
    sValueBlock->mStringLengthUnit  = DTL_STRING_LENGTH_UNIT_NA;
    sValueBlock->mIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;
    sValueBlock->mAllocBufferSize   = STL_INT32_MAX;   /* 필요할때마다 공간을 할당받음 */
    sValueBlock->mDataValue         = sDataValue;

    /**
     * Block List 정보 설정
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF(knlValueBlockList),
                                (void **) & sBlockList,
                                aEnv )
             == STL_SUCCESS );
    stlMemset( sBlockList, 0x00, STL_SIZEOF(knlValueBlockList) );

    sBlockList->mTableID     = KNL_ANONYMOUS_TABLE_ID;
    sBlockList->mColumnOrder = 0;
    sBlockList->mValueBlock  = sValueBlock;
    sBlockList->mNext        = NULL;
    
    /**
     * Output 설정
     */

    *aValueBlockList = sBlockList;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief n-번째 Value Block List를 얻기
 * @param[in]   aSource  Source Value Block List 
 * @param[in]   aOrder   Block List 순서 
 * @param[out]  aResult  Result Value Block List
 * @param[in]   aEnv     Environment
 * @remarks
 */
stlStatus knlGetBlockList( knlValueBlockList   * aSource,
                           stlInt32              aOrder,
                           knlValueBlockList  ** aResult,
                           knlEnv              * aEnv )
{
    knlValueBlockList  * sResult = NULL;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aSource != NULL, KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aResult != NULL, KNL_ERROR_STACK(aEnv) );

    sResult = aSource;
    
    while( aOrder > 0 )
    {
        STL_PARAM_VALIDATE( sResult != NULL, KNL_ERROR_STACK(aEnv) );
        
        sResult = sResult->mNext;
        aOrder--;
    }

    *aResult = sResult;

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Join Result Relation 을 이용해 Join Result Block 을 설정한다.
 * @param[in] aRelationList    Join Relation List
 * @param[in] aResultBlockIdx  Result Block Idx
 * @param[in] aLeftBlockIdx    Left Block Idx
 * @param[in] aRightBlockIdx   Right Block Idx
 * @param[in] aEnv             Environment
 */
stlStatus  knlSetJoinResultBlock( knlBlockRelationList * aRelationList,
                                  stlInt32               aResultBlockIdx,
                                  stlInt32               aLeftBlockIdx,
                                  stlInt32               aRightBlockIdx,
                                  knlEnv               * aEnv )
{
    knlBlockRelationList * sList = NULL;
    dtlDataValue * sDstValue = NULL;
    dtlDataValue * sSrcValue = NULL;

    sList = aRelationList;

    while ( sList != NULL )
    {
        if ( sList->mIsLeftSrc == STL_TRUE )
        {
            sSrcValue = DTL_GET_VALUE_BLOCK_DATA_VALUE( sList->mSrcBlock, aLeftBlockIdx );
        }
        else
        {
            sSrcValue = DTL_GET_VALUE_BLOCK_DATA_VALUE( sList->mSrcBlock, aRightBlockIdx );
        }

        sDstValue = DTL_GET_VALUE_BLOCK_DATA_VALUE( sList->mDstBlock, aResultBlockIdx );

        if( (sList->mCastFuncInfo != NULL) && !DTL_IS_NULL( sSrcValue ) )
        {
            sList->mCastFuncInfo->mArgs[0].mValue.mDataValue = sSrcValue;
            STL_TRY( sList->mCastFuncInfo->mCastFuncPtr(
                         DTL_CAST_INPUT_ARG_CNT,
                         sList->mCastFuncInfo->mArgs,
                         (void*)sDstValue,
                         (void*)(sList->mCastFuncInfo->mSrcTypeInfo),
                         KNL_DT_VECTOR( aEnv ),
                         aEnv,
                         KNL_ERROR_STACK( aEnv ) )
                     == STL_SUCCESS );                     
        }
        else
        {
            STL_TRY( knlCopyDataValue( sSrcValue,
                                       sDstValue,
                                       aEnv )
                     == STL_SUCCESS );
        }

        sList = sList->mNext;
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief Join Result Relation 을 이용해 Left 가 Null 인 Join Result Block 을 설정한다.
 * @param[in] aRelationList    Join Relation List
 * @param[in] aResultBlockIdx  Result Block Idx
 * @param[in] aRightBlockIdx   Right Block Idx
 * @param[in] aEnv             Environment
 */
stlStatus  knlSetLeftNullPaddJoinResultBlock( knlBlockRelationList * aRelationList,
                                              stlInt32               aResultBlockIdx,
                                              stlInt32               aRightBlockIdx,
                                              knlEnv               * aEnv )
{
    knlBlockRelationList * sList = NULL;
    dtlDataValue * sDstValue = NULL;
    dtlDataValue * sSrcValue = NULL;

    sList = aRelationList;

    while ( sList != NULL )
    {
        sDstValue = DTL_GET_VALUE_BLOCK_DATA_VALUE( sList->mDstBlock, aResultBlockIdx );
        
        if ( sList->mIsLeftSrc == STL_TRUE )
        {
            DTL_SET_NULL( sDstValue );
        }
        else
        {
            sSrcValue = DTL_GET_VALUE_BLOCK_DATA_VALUE( sList->mSrcBlock, aRightBlockIdx );

            if( (sList->mCastFuncInfo != NULL) && !DTL_IS_NULL( sSrcValue ) )
            {
                sList->mCastFuncInfo->mArgs[0].mValue.mDataValue = sSrcValue;
                STL_TRY( sList->mCastFuncInfo->mCastFuncPtr(
                             DTL_CAST_INPUT_ARG_CNT,
                             sList->mCastFuncInfo->mArgs,
                             (void*)sDstValue,
                             (void*)(sList->mCastFuncInfo->mSrcTypeInfo),
                             KNL_DT_VECTOR( aEnv ),
                             aEnv,
                             KNL_ERROR_STACK( aEnv ) )
                         == STL_SUCCESS );                     
            }
            else
            {
                STL_TRY( knlCopyDataValue( sSrcValue,
                                           sDstValue,
                                           aEnv )
                         == STL_SUCCESS );
            }
        }

        sList = sList->mNext;
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Long Varchar/Varbinary DataValue의 Value 공간을 해제한다.
 * @param[in]  aValueBlockList   Value Block List
 * @param[in]  aEnv              Environment
 * @remarks
 * 실제로 free하지 않고, value만 Null로 초기화 한다.
 */
stlStatus knlFreeLongVaryingValues( knlValueBlockList * aValueBlockList,
                                    knlEnv            * aEnv )
{
    stlInt32        sLoop = 0;
    stlInt32        sBlockSize = 0;
    dtlDataValue  * sDataValue  = NULL;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aValueBlockList != NULL, KNL_ERROR_STACK(aEnv) );

    sDataValue = aValueBlockList->mValueBlock->mDataValue;
    sBlockSize = KNL_GET_BLOCK_ALLOC_CNT(aValueBlockList);
    
    STL_PARAM_VALIDATE( (sDataValue[0].mType == DTL_TYPE_LONGVARCHAR) ||
                        (sDataValue[0].mType == DTL_TYPE_LONGVARBINARY),
                        KNL_ERROR_STACK(aEnv) );
    
    if( KNL_GET_BLOCK_IS_SEP_BUFF( aValueBlockList ) == STL_TRUE )
    {
        for( sLoop = 0; sLoop < sBlockSize; sLoop++ )
        {
            if( sDataValue[ sLoop ].mValue != NULL )
            {
                STL_TRY( knlFreeLongVaryingMem( sDataValue[ sLoop ].mValue,
                                                aEnv )
                         == STL_SUCCESS );
                
                sDataValue[ sLoop ].mValue = NULL;
                sDataValue[ sLoop ].mLength = 0;
                sDataValue[ sLoop ].mBufferSize = 0;
            }
        }
    }
    else
    {
        if( sDataValue[ 0 ].mValue != NULL )
        {
            STL_TRY( knlFreeLongVaryingMem( sDataValue[ 0 ].mValue,
                                            aEnv )
                     == STL_SUCCESS );
            
            sDataValue[ 0 ].mValue = NULL;
            sDataValue[ 0 ].mLength = 0;
            sDataValue[ 0 ].mBufferSize = 0;
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 바인드할 파라미터를 생성 한다.
 * @param[in]      aDtlDataType       DB Data Type
 * @param[in]      aArg1              Parameter 속성 1
 * @param[in]      aArg2              Parameter 속성 2
 * @param[in]      aStringLengthUnit  string length unit (dtlStringLengthUnit 참조)
 * @param[in]      aIntervalIndicator INTERVAL 타입의 Indicator
 * @param[in]      aAllocLayer        Block을 할당한 Layer
 * @param[in]      aBlockCount        block count
 * @param[out]     aValueBlock        생성된 value block list
 * @param[in,out]  aEnv               Environment Pointer
 */
stlStatus knlAllocParamValues( dtlDataType             aDtlDataType,
                               stlInt64                aArg1,
                               stlInt64                aArg2,
                               dtlStringLengthUnit     aStringLengthUnit,
                               dtlIntervalIndicator    aIntervalIndicator,
                               stlLayerClass           aAllocLayer,
                               stlInt32                aBlockCount,
                               knlValueBlockList    ** aValueBlock,
                               knlEnv                * aEnv )
{
    knlDynamicMem     * sDynamicMem = NULL;
    dtlValueBlockList * sBlockList  = NULL;
    dtlValueBlock     * sValueBlock = NULL;
    dtlDataValue      * sDataValue  = NULL;
    stlInt64            sBufferSize = 0;
    stlUInt32           sSize = 0;
    stlInt32            sState = 0;

    sDynamicMem = KNL_GET_PARAMETER_DYNAMIC_MEM( KNL_SESS_ENV(aEnv) );
    
    /*
     * Block Count 만큼 Data Value 정보 설정 
     */

    sSize = STL_SIZEOF(dtlDataValue) * aBlockCount;
    
    STL_TRY( knlAllocDynamicMem( sDynamicMem,
                                 sSize,
                                 (void **)&sDataValue,
                                 aEnv ) == STL_SUCCESS );
    sState = 1;
    
    /*
     * Data Buffer 공간 할당
     */

    STL_TRY( knlAllocDataValueArrayDynamic( sDataValue,
                                            aDtlDataType,
                                            aArg1,
                                            aStringLengthUnit,
                                            aBlockCount,
                                            sDynamicMem,
                                            &sBufferSize,
                                            aEnv )
             == STL_SUCCESS );
    sState = 2;
    
    /*
     * Value Block 정보 설정
     */

    sSize = STL_SIZEOF(dtlValueBlock);
    
    STL_TRY( knlAllocDynamicMem( sDynamicMem,
                                 sSize,
                                 (void **)&sValueBlock,
                                 aEnv )
             == STL_SUCCESS );
    sState = 3;
    
    stlMemset( sValueBlock, 0x00, sSize );

    sValueBlock->mIsSepBuff         = STL_TRUE;
    sValueBlock->mAllocLayer        = aAllocLayer;
    sValueBlock->mAllocBlockCnt     = aBlockCount;
    sValueBlock->mUsedBlockCnt      = 0;
    sValueBlock->mSkipBlockCnt      = 0;
    sValueBlock->mArgNum1           = aArg1;
    sValueBlock->mArgNum2           = aArg2;
    sValueBlock->mStringLengthUnit  = aStringLengthUnit;
    sValueBlock->mIntervalIndicator = aIntervalIndicator;
    sValueBlock->mAllocBufferSize   = sBufferSize;
    sValueBlock->mDataValue         = sDataValue;

    /*
     * Block List 정보 설정
     */

    sSize = STL_SIZEOF(dtlValueBlockList);

    STL_TRY( knlAllocDynamicMem( sDynamicMem,
                                 sSize,
                                 (void **)&sBlockList,
                                 aEnv )
             == STL_SUCCESS );
    sState = 4;

    stlMemset( sBlockList, 0x00, sSize );

    sBlockList->mTableID     = 0;
    sBlockList->mColumnOrder = 0;
    sBlockList->mValueBlock  = sValueBlock;
    sBlockList->mNext        = NULL;

    if( aValueBlock != NULL )
    {
        *aValueBlock = sBlockList;
    }

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 4:
            (void) knlFreeDynamicMem( sDynamicMem, sBlockList, aEnv );
        case 3:
            (void) knlFreeDynamicMem( sDynamicMem, sValueBlock, aEnv );
        case 2:
            (void) knlFreeLongDataValueArray( sDataValue, aBlockCount, aEnv );
        case 1:
            (void) knlFreeDynamicMem( sDynamicMem, sDataValue, aEnv );
        default:
            break;
    }

    return STL_FAILURE;
}

/**
 * @brief 바인드된 파라미터를 삭제 한다.
 * @param[in]  aValueBlockList   Value Block List
 * @param[in]  aEnv              Environment
 */
stlStatus knlFreeParamValues( knlValueBlockList * aValueBlockList,
                              knlEnv            * aEnv )
{
    knlDynamicMem * sDynamicMem = NULL;
    dtlDataValue  * sDataValue  = NULL;
    stlInt32        sLoop = 0;
    dtlDataType     sDBType;
    stlInt32        sBlockSize = 0;

    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aValueBlockList != NULL, KNL_ERROR_STACK(aEnv) );

    sDynamicMem = KNL_GET_PARAMETER_DYNAMIC_MEM( KNL_SESS_ENV(aEnv) );
    
    sDBType    = KNL_GET_BLOCK_DB_TYPE( aValueBlockList );
    sBlockSize = KNL_GET_BLOCK_ALLOC_CNT( aValueBlockList );
    sDataValue = DTL_GET_BLOCK_FIRST_DATA_VALUE( aValueBlockList );

    STL_ASSERT( sDataValue != NULL );

    if( (sDBType == DTL_TYPE_LONGVARCHAR) ||
        (sDBType == DTL_TYPE_LONGVARBINARY) )
    {
        for( sLoop = 0; sLoop < sBlockSize; sLoop++ )
        {
            if( sDataValue[ sLoop ].mValue != NULL )
            {
                STL_TRY( knlFreeLongVaryingMem( sDataValue[ sLoop ].mValue,
                                                aEnv )
                         == STL_SUCCESS );
                
                sDataValue[ sLoop ].mValue      = NULL;
                sDataValue[ sLoop ].mLength     = 0;
                sDataValue[ sLoop ].mBufferSize = 0;
            }
        }
    }
    else
    {
        STL_TRY( knlFreeDynamicMem( sDynamicMem,
                                    sDataValue[0].mValue,
                                    aEnv )
                 == STL_SUCCESS );
    
        for( sLoop = 0 ; sLoop < sBlockSize ; sLoop++ )
        {
            sDataValue[ sLoop ].mValue      = NULL;
            sDataValue[ sLoop ].mLength     = 0;
            sDataValue[ sLoop ].mBufferSize = 0;
        }
    }

    STL_TRY( knlFreeDynamicMem( sDynamicMem,
                                sDataValue,
                                aEnv )
             == STL_SUCCESS );

    aValueBlockList->mValueBlock->mDataValue = NULL;

    STL_TRY( knlFreeDynamicMem( sDynamicMem,
                                aValueBlockList->mValueBlock,
                                aEnv )
             == STL_SUCCESS );
    
    aValueBlockList->mValueBlock = NULL;

    STL_TRY( knlFreeDynamicMem( sDynamicMem,
                                aValueBlockList,
                                aEnv )
             == STL_SUCCESS );

    aValueBlockList = NULL;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} knlValueBlock */
