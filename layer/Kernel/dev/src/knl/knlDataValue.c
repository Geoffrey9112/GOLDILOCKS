/*******************************************************************************
 * knlDataValue.c
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
 * @file knlDataValue.c
 * @brief Block Read를 위한 Data Value 관리 루틴
 */

#include <knl.h>



/**
 * @addtogroup knlDataValue
 * @{
 */

/**
 * @brief Data Value Array의 mValue를 할당 받는다.
 * @param[in,out]  aDataValue         Data Value
 * @param[in]      aDBType            DB Data Type
 * @param[in]      aPrecision         Precision
 * @param[in]      aStringLengthUnit  string length unit (dtlStringLengthUnit 참조)
 * @param[in]      aArraySize         Data Value Array 개수
 * @param[in]      aRegionMem         Region Memory
 * @param[out]     aBufferSize        Allocated Buffer Size For Each Value (not aligned)
 * @param[in]      aEnv               Environment
 */
stlStatus knlAllocDataValueArray( dtlDataValue           * aDataValue,
                                  dtlDataType              aDBType,
                                  stlInt64                 aPrecision,
                                  dtlStringLengthUnit      aStringLengthUnit,
                                  stlInt32                 aArraySize,
                                  knlRegionMem           * aRegionMem,
                                  stlInt64               * aBufferSize,
                                  knlEnv                 * aEnv )
{
    stlInt64        sLoop           = 0;
    stlInt64        sLength         = 0;
    stlInt64        sAlignSize      = 0;
    void          * sDataBuffer     = NULL;
    

    /*
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aDataValue != NULL, KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( ( aDBType >= 0 ) && ( aDBType < DTL_TYPE_MAX ),
                        KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aArraySize > 0, KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, KNL_ERROR_STACK(aEnv) );


    /**
     * Value Buffer Size 얻기
     */
    
    STL_TRY( dtlGetDataValueBufferSize( aDBType,
                                        aPrecision,
                                        aStringLengthUnit,
                                        & sLength,
                                        KNL_DT_VECTOR(aEnv),
                                        aEnv,
                                        KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );

    STL_DASSERT( sLength > 0 );

    sAlignSize = STL_ALIGN_DEFAULT( sLength );


    if( (aDBType == DTL_TYPE_LONGVARCHAR) ||
        (aDBType == DTL_TYPE_LONGVARBINARY) )
    {
        for( sLoop = 0 ; sLoop < aArraySize ; sLoop++ )
        {
            STL_TRY( knlAllocLongVaryingMem( sAlignSize,
                                             &sDataBuffer,
                                             aEnv )
                     == STL_SUCCESS );
            
            aDataValue[ sLoop ].mType       = aDBType;
            aDataValue[ sLoop ].mLength     = 0;
            aDataValue[ sLoop ].mBufferSize = sAlignSize;
            aDataValue[ sLoop ].mValue      = sDataBuffer;
        }
    }
    else
    {
        /**
         * Value Buffer 공간 할당
         */

        STL_TRY( knlAllocRegionMem( aRegionMem,
                                    sAlignSize * aArraySize,
                                    & sDataBuffer,
                                    aEnv )
                 == STL_SUCCESS );
    

        /**
         * Data Value 정보 설정
         */
    
        for( sLoop = 0 ; sLoop < aArraySize ; sLoop++ )
        {
            aDataValue[ sLoop ].mType       = aDBType;
            aDataValue[ sLoop ].mLength     = 0;
            aDataValue[ sLoop ].mBufferSize = sAlignSize;
            aDataValue[ sLoop ].mValue      = sDataBuffer;
            sDataBuffer += sAlignSize;
        }
    }


    /**
     * OUTPUT 설정
     */

    *aBufferSize = sLength;
    
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    if( (aDBType == DTL_TYPE_LONGVARCHAR) ||
        (aDBType == DTL_TYPE_LONGVARBINARY) )
    {
        for( sLoop--; sLoop >= 0 ; sLoop-- )
        {
            (void) knlFreeLongVaryingMem( aDataValue[ sLoop ].mValue, aEnv );
            
            aDataValue[ sLoop ].mType       = 0;
            aDataValue[ sLoop ].mLength     = 0;
            aDataValue[ sLoop ].mBufferSize = 0;
            aDataValue[ sLoop ].mValue      = NULL;
        }
    }
    
    return STL_FAILURE;
}


/**
 * @brief Data Value Array의 mValue를 할당 받는다.
 * @param[in,out]  aDataValue         Data Value
 * @param[in]      aDBType            DB Data Type
 * @param[in]      aPrecision         Precision
 * @param[in]      aStringLengthUnit  string length unit (dtlStringLengthUnit 참조)
 * @param[in]      aArraySize         Data Value Array 개수
 * @param[in]      aDynamicMem        Dynamic Memory
 * @param[out]     aBufferSize        Allocated Buffer Size For Each Value (not aligned)
 * @param[in]      aEnv               Environment
 */
stlStatus knlAllocDataValueArrayDynamic( dtlDataValue           * aDataValue,
                                         dtlDataType              aDBType,
                                         stlInt64                 aPrecision,
                                         dtlStringLengthUnit      aStringLengthUnit,
                                         stlInt32                 aArraySize,
                                         knlDynamicMem          * aDynamicMem,
                                         stlInt64               * aBufferSize,
                                         knlEnv                 * aEnv )
{
    stlInt64        sLoop           = 0;
    stlInt64        sLength         = 0;
    stlInt64        sAlignSize      = 0;
    void          * sDataBuffer     = NULL;
    

    /*
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aDataValue != NULL, KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( ( aDBType >= 0 ) && ( aDBType < DTL_TYPE_MAX ),
                        KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aArraySize > 0, KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDynamicMem != NULL, KNL_ERROR_STACK(aEnv) );


    /**
     * Value Buffer Size 얻기
     */
    
    STL_TRY( dtlGetDataValueBufferSize( aDBType,
                                        aPrecision,
                                        aStringLengthUnit,
                                        & sLength,
                                        KNL_DT_VECTOR(aEnv),
                                        aEnv,
                                        KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );

    STL_DASSERT( sLength > 0 );

    sAlignSize = STL_ALIGN_DEFAULT( sLength );


    if( (aDBType == DTL_TYPE_LONGVARCHAR) ||
        (aDBType == DTL_TYPE_LONGVARBINARY) )
    {
        for( sLoop = 0 ; sLoop < aArraySize ; sLoop++ )
        {
            STL_TRY( knlAllocLongVaryingMem( sAlignSize,
                                             &sDataBuffer,
                                             aEnv )
                     == STL_SUCCESS );
            
            aDataValue[ sLoop ].mType       = aDBType;
            aDataValue[ sLoop ].mLength     = 0;
            aDataValue[ sLoop ].mBufferSize = sAlignSize;
            aDataValue[ sLoop ].mValue      = sDataBuffer;
        }
    }
    else
    {
        /**
         * Value Buffer 공간 할당
         */

        STL_TRY( knlAllocDynamicMem( aDynamicMem,
                                     sAlignSize * aArraySize,
                                     & sDataBuffer,
                                     aEnv )
                 == STL_SUCCESS );
    

        /**
         * Data Value 정보 설정
         */
    
        for( sLoop = 0 ; sLoop < aArraySize ; sLoop++ )
        {
            aDataValue[ sLoop ].mType       = aDBType;
            aDataValue[ sLoop ].mLength     = 0;
            aDataValue[ sLoop ].mBufferSize = sAlignSize;
            aDataValue[ sLoop ].mValue      = sDataBuffer;
            sDataBuffer += sAlignSize;
        }
    }


    /**
     * OUTPUT 설정
     */

    *aBufferSize = sLength;
    
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    if( (aDBType == DTL_TYPE_LONGVARCHAR) ||
        (aDBType == DTL_TYPE_LONGVARBINARY) )
    {
        for( sLoop--; sLoop >= 0 ; sLoop-- )
        {
            (void) knlFreeLongVaryingMem( aDataValue[ sLoop ].mValue, aEnv );
            
            aDataValue[ sLoop ].mType       = 0;
            aDataValue[ sLoop ].mLength     = 0;
            aDataValue[ sLoop ].mBufferSize = 0;
            aDataValue[ sLoop ].mValue      = NULL;
        }
    }
    
    return STL_FAILURE;
}

/**
 * @brief Input String을 분석하여 Data Value Array의 mValue를 할당 받는다.
 * @param[in,out]  aDataValue         Data Value
 * @param[in]      aDBType            DB Data Type
 * @param[in]      aPrecision         Precision
 * @param[in]      aScale             Scale
 * @param[in]      aStringLengthUnit  string length unit (dtlStringLengthUnit 참조)
 * @param[in]      aString            Input String 
 * @param[in]      aStringLength      Input String Length
 * @param[in]      aArraySize         Data Value Array 개수
 * @param[in]      aRegionMem         Region Memory
 * @param[out]     aBufferSize        Allocated Buffer Size For Each Value (not aligned)
 * @param[in]      aEnv               Environment
 */
stlStatus knlAllocDataValueArrayFromString( dtlDataValue           * aDataValue,
                                            dtlDataType              aDBType,
                                            stlInt64                 aPrecision,
                                            stlInt64                 aScale,
                                            dtlStringLengthUnit      aStringLengthUnit,
                                            stlChar                * aString,
                                            stlInt64                 aStringLength,
                                            stlInt32                 aArraySize,
                                            knlRegionMem           * aRegionMem,
                                            stlInt64               * aBufferSize,
                                            knlEnv                 * aEnv )
{
    stlInt64        sLoop           = 0;
    stlInt64        sLength         = 0;
    stlInt64        sAlignSize      = 0;
    void          * sDataBuffer     = NULL;
    

    /*
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aDataValue != NULL, KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( ( aDBType >= 0 ) && ( aDBType < DTL_TYPE_MAX ),
                        KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aString != NULL, KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aArraySize > 0, KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, KNL_ERROR_STACK(aEnv) );


    /**
     * Value Buffer Size 얻기
     */
    
    STL_TRY( dtlGetDataValueBufferSizeFromString( aDBType,
                                                  aPrecision,
                                                  aScale,
                                                  aStringLengthUnit,
                                                  aString,
                                                  aStringLength,
                                                  & sLength,
                                                  KNL_DT_VECTOR(aEnv),
                                                  aEnv,
                                                  KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );

    STL_DASSERT( sLength > 0 );

    sAlignSize = STL_ALIGN_DEFAULT( sLength );


    if( (aDBType == DTL_TYPE_LONGVARCHAR) ||
        (aDBType == DTL_TYPE_LONGVARBINARY) )
    {
        for( sLoop = 0 ; sLoop < aArraySize ; sLoop++ )
        {
            /**
             * Value Buffer 공간 할당
             */

            STL_TRY( knlAllocLongVaryingMem( sAlignSize,
                                             &sDataBuffer,
                                             aEnv )
                     == STL_SUCCESS );
            
            /**
             * Data Value 정보 설정
             */

            aDataValue[ sLoop ].mType       = aDBType;
            aDataValue[ sLoop ].mLength     = 0;
            aDataValue[ sLoop ].mBufferSize = sAlignSize;
            aDataValue[ sLoop ].mValue      = sDataBuffer;
        }
    }
    else
    {
        /**
         * Value Buffer 공간 할당
         */

        STL_TRY( knlAllocRegionMem( aRegionMem,
                                    sAlignSize * aArraySize,
                                    & sDataBuffer,
                                    aEnv )
                 == STL_SUCCESS );
        
        stlMemset( sDataBuffer, 0x00, sAlignSize * aArraySize );
        
        /**
         * Data Value 정보 설정
         */

        for( sLoop = 0 ; sLoop < aArraySize ; sLoop++ )
        {
            aDataValue[ sLoop ].mType       = aDBType;
            aDataValue[ sLoop ].mBufferSize = sAlignSize;
            aDataValue[ sLoop ].mLength     = 0;
            aDataValue[ sLoop ].mValue      = sDataBuffer;
            sDataBuffer += sAlignSize;
        }
    }

    /**
     * OUTPUT 설정
     */

    *aBufferSize = sLength;
    
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    if( (aDBType == DTL_TYPE_LONGVARCHAR) ||
        (aDBType == DTL_TYPE_LONGVARBINARY) )
    {
        for( sLoop--; sLoop >= 0 ; sLoop-- )
        {
            (void) knlFreeLongVaryingMem( aDataValue[ sLoop ].mValue, aEnv );
            
            aDataValue[ sLoop ].mType       = 0;
            aDataValue[ sLoop ].mLength     = 0;
            aDataValue[ sLoop ].mBufferSize = 0;
            aDataValue[ sLoop ].mValue      = NULL;
        }
    }
    
    return STL_FAILURE;
}


/**
 * @brief Data Value 에 할당된 모든 Long Varying Memory를 해재한다.
 * @param[in]      aDataValue         Data Value
 * @param[in]      aArraySize         Data Value Array 개수
 * @param[in]      aEnv               Environment
 */
stlStatus knlFreeLongDataValueArray( dtlDataValue * aDataValue,
                                     stlInt32       aArraySize,
                                     knlEnv       * aEnv )
{
    stlInt64  sLoop = 0;
    
    if( (aDataValue[0].mType == DTL_TYPE_LONGVARCHAR) ||
        (aDataValue[0].mType == DTL_TYPE_LONGVARBINARY) )
    {
        for( sLoop = 0; sLoop < aArraySize ; sLoop++ )
        {
            STL_TRY( knlFreeLongVaryingMem( aDataValue[ sLoop ].mValue,
                                            aEnv )
                     == STL_SUCCESS );
            
            aDataValue[ sLoop ].mType       = 0;
            aDataValue[ sLoop ].mLength     = 0;
            aDataValue[ sLoop ].mBufferSize = 0;
            aDataValue[ sLoop ].mValue      = NULL;
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}    



/**
 * @brief dtlDataType의 내용 복사
 * @param[in]      aSrcValue      copy 대상이 되는 dtlDataType ( dtlDataValue * )
 * @param[in,out]  aDstValue      copy되는 내용이 들어갈 dtlDataType ( dtlDataValue * )
 * @param[in]      aEnv           Environment
 */
stlStatus knlCopyDataValue( dtlDataValue * aSrcValue,
                            dtlDataValue * aDstValue,
                            knlEnv       * aEnv )
{
    if( aSrcValue != aDstValue )
    {
        STL_DASSERT( (aSrcValue->mLength == 0) || (aSrcValue->mValue != NULL) );
        STL_DASSERT( (aDstValue->mLength == 0) || (aDstValue->mValue != NULL) );
        
        aDstValue->mLength = aSrcValue->mLength;
        STL_DASSERT( ( aDstValue->mType == aSrcValue->mType ) ||
                     ( ( dtlDataTypeGroup[aDstValue->mType] == DTL_GROUP_NUMBER ) &&
                       ( dtlDataTypeGroup[aSrcValue->mType] == DTL_GROUP_NUMBER ) ) );
        
        if( aDstValue->mValue != aSrcValue->mValue )
        {
            /**
             * 추가적인 공간이 필요한 경우라면
             */
            
            if( aDstValue->mBufferSize < aSrcValue->mLength )
            {
                STL_DASSERT( (aDstValue->mType == DTL_TYPE_LONGVARCHAR) ||
                             (aDstValue->mType == DTL_TYPE_LONGVARBINARY) );
                STL_DASSERT( (aSrcValue->mType == DTL_TYPE_LONGVARCHAR) ||
                             (aSrcValue->mType == DTL_TYPE_LONGVARBINARY) );

                STL_TRY( knlReallocLongVaryingMem( aSrcValue->mLength,
                                                   (void**)&aDstValue->mValue,
                                                   aEnv )
                         == STL_SUCCESS );
                
                aDstValue->mBufferSize = aSrcValue->mLength;
            }
            
            DTL_COPY_VALUE( aDstValue,
                            aSrcValue->mValue,
                            aSrcValue->mLength );
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Long Varying Memory를 할당한다.
 * @param[in]     aAllocSize     할당할 공간의 크기
 * @param[out]    aAddr          할당 받은 공간의 메모리 주소
 * @param[in]     aEnv           Environment
 */
stlStatus knlAllocLongVaryingMem( stlInt32     aAllocSize,
                                  void      ** aAddr,
                                  knlEnv     * aEnv )
{
    knlDynamicMem * sDynamicMem;
    
    sDynamicMem = KNL_GET_LONG_VARYING_DYNAMIC_MEM( KNL_SESS_ENV(aEnv) );

    STL_TRY( knlAllocDynamicMem( sDynamicMem,
                                 aAllocSize,
                                 aAddr,
                                 aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
                            

/**
 * @brief Long Varying Memory를 해제한다.
 * @param[in]    aAddr          해제할 메모리 주소
 * @param[in]    aEnv           Environment
 */
stlStatus knlFreeLongVaryingMem( void   * aAddr,
                                 knlEnv * aEnv )
{
    knlDynamicMem * sDynamicMem;
    
    sDynamicMem = KNL_GET_LONG_VARYING_DYNAMIC_MEM( KNL_SESS_ENV(aEnv) );

    STL_TRY( knlFreeDynamicMem( sDynamicMem,
                                aAddr,
                                aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
                            

/**
 * @brief Long Varying Memory를 재할당한다.
 * @param[in]     aAllocSize     할당할 공간의 크기
 * @param[out]    aAddr          할당 받은 공간의 메모리 주소
 * @param[in]     aEnv           Environment
 */
stlStatus knlReallocLongVaryingMem( stlInt32     aAllocSize,
                                    void      ** aAddr,
                                    void       * aEnv )
{
    knlDynamicMem * sDynamicMem;
    knlEnv        * sEnv = (knlEnv*)aEnv;
    void          * sAddr = NULL;
    
    sDynamicMem = KNL_GET_LONG_VARYING_DYNAMIC_MEM( KNL_SESS_ENV(sEnv) );
    sAddr = *aAddr;

    if( aAllocSize > DTL_LONGVARYING_INIT_STRING_SIZE )
    {
        STL_TRY( knlAllocDynamicMem( sDynamicMem,
                                     aAllocSize,
                                     aAddr,
                                     sEnv )
                 == STL_SUCCESS );

        STL_TRY( knlFreeDynamicMem( sDynamicMem,
                                    sAddr,
                                    sEnv )
                 == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
                            

/**
 * @brief Long Varying Memory를 재할당하고 내용을 복사한다.
 * @param[out]    aDataValue     대상 dtlDataValue
 * @param[in]     aAllocSize     할당할 공간의 크기
 * @param[in]     aEnv           Environment
 */
stlStatus knlReallocAndMoveLongVaryingMem( dtlDataValue  * aDataValue,
                                           stlInt32        aAllocSize,
                                           knlEnv        * aEnv )
{
    knlDynamicMem * sDynamicMem;
    void          * sSrcAddr = aDataValue->mValue;
    void          * sDstAddr;
    
    sDynamicMem = KNL_GET_LONG_VARYING_DYNAMIC_MEM( KNL_SESS_ENV(aEnv) );

    if( aAllocSize > DTL_LONGVARYING_INIT_STRING_SIZE )
    {
        STL_TRY( knlAllocDynamicMem( sDynamicMem,
                                     aAllocSize,
                                     &sDstAddr,
                                     aEnv )
                 == STL_SUCCESS );

        stlMemcpy( sDstAddr, sSrcAddr, aDataValue->mLength );
        
        STL_TRY( knlFreeDynamicMem( sDynamicMem,
                                    sSrcAddr,
                                    aEnv )
                 == STL_SUCCESS );

        aDataValue->mValue = sDstAddr;
        aDataValue->mBufferSize = aAllocSize;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
                            

/**
 * @brief (DEBUG 용도) Write Block List 의 Data Value 가 유효한 값인지 검증한다.
 * @param[in] aWriteBlockList   Write Block List
 * @param[in] aSkipCnt          Skip Count
 * @param[in] aUsedCnt          Used Count
 * @param[in] aEnv              Environment
 * @remarks
 * DEBUG mode 에서는 CORE
 */
void knlVerifyWriteDataValue( knlValueBlockList * aWriteBlockList,
                              stlInt64            aSkipCnt,
                              stlInt64            aUsedCnt,
                              knlEnv            * aEnv )
{
    knlValueBlockList * sCurrList = NULL;
    dtlDataValue      * sCurrValue = NULL;
    
    stlInt32            i = 0;
    stlBool             sResult = STL_FALSE;

    for ( sCurrList = aWriteBlockList; sCurrList != NULL; sCurrList = sCurrList->mNext )
    {
        for ( i = aSkipCnt; i < aUsedCnt; i++ )
        {
            sCurrValue = KNL_GET_BLOCK_DATA_VALUE( sCurrList, i );

            sResult = dtlIsValidDataValue( sCurrValue );

            if ( sResult == STL_TRUE )
            {
                /* ok */
            }
            else
            {
                STL_DASSERT(0);
            }
        }
    }
}

/** @} knlDataValue */
