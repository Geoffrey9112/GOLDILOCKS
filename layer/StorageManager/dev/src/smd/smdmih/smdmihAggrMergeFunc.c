/*******************************************************************************
 * smdmihTable.c
 *
 * Copyright (c) 2012, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: smdmihTable.c 6322 2012-11-13 05:16:47Z egon $
 *
 * NOTES
 *
 *
 ******************************************************************************/

/**
 * @file smdmihTable.c
 * @brief Storage Manager Layer Memory Instant Hash Table Routines
 */

#include <sml.h>
#include <smDef.h>
#include <smg.h>
#include <sma.h>
#include <smd.h>
#include <smo.h>
#include <smsDef.h>
#include <smsManager.h>
#include <smdDef.h>
#include <smdmifTable.h>
#include <smdmih.h>
#include <smdmifVarPart.h>

smlMergeRecordAggrFunc gSmlAggrMergeFuncInfoArr[KNL_BUILTIN_AGGREGATION_UNARY_MAX][2] =
{
    {
        NULL,
        NULL,
    },
    {
        (smlMergeRecordAggrFunc)smdmihMergeRecordSum,
        (smlMergeRecordAggrFunc)smdmihMergeRecordSumWithExpr
    },
    {
        (smlMergeRecordAggrFunc)smdmihMergeRecordCount,
        (smlMergeRecordAggrFunc)smdmihMergeRecordCountWithExpr
    },
    {
        (smlMergeRecordAggrFunc)smdmihMergeRecordCountAst,
        (smlMergeRecordAggrFunc)smdmihMergeRecordCountAst
    },
    {
        (smlMergeRecordAggrFunc)smdmihMergeRecordMin,
        (smlMergeRecordAggrFunc)smdmihMergeRecordMin
    },
    {
        (smlMergeRecordAggrFunc)smdmihMergeRecordMax,
        (smlMergeRecordAggrFunc)smdmihMergeRecordMax
    }
};


stlStatus smdmihMergeRecordCountAst( smdmihTableHeader  * aTableHeader,
                                     stlChar            * aRow,
                                     smlAggrFuncInfo    * aAggrFuncInfo,
                                     stlInt32             aStartIdx,
                                     stlInt32             aEndIdx,
                                     knlValueBlockList  * aReadBlock,
                                     knlValueBlockList  * aUpdateBlock,
                                     smlEnv             * aEnv )
{
    smdmifDefColumn   * sColumn;
    stlChar           * sColumnValue;
    stlInt64            sCount;

    /**
     * merge aggregation
     */

    sColumn = &aTableHeader->mColumns[aReadBlock->mColumnOrder];
    sColumnValue = aRow + sColumn->mOffset + 1;
    
    STL_WRITE_INT64( &sCount, sColumnValue );
    sCount += aEndIdx - aStartIdx;
    STL_WRITE_INT64( sColumnValue, &sCount );
    
    return STL_SUCCESS;
}

stlStatus smdmihMergeRecordCount( smdmihTableHeader  * aTableHeader,
                                  stlChar            * aRow,
                                  smlAggrFuncInfo    * aAggrFuncInfo,
                                  stlInt32             aStartIdx,
                                  stlInt32             aEndIdx,
                                  knlValueBlockList  * aReadBlock,
                                  knlValueBlockList  * aUpdateBlock,
                                  smlEnv             * aEnv )
{
    smdmifDefColumn   * sColumn;
    stlChar           * sColumnValue;
    dtlDataValue      * sDataValue;
    dtlDataValue      * sFence;
    stlInt64            sCount;

    /**
     * merge aggregation
     */

    if( KNL_GET_BLOCK_IS_SEP_BUFF( aAggrFuncInfo->mInputArgList ) == STL_TRUE )
    {
        sDataValue = KNL_GET_BLOCK_DATA_VALUE( aAggrFuncInfo->mInputArgList, aStartIdx );
        sFence = sDataValue + aEndIdx - aStartIdx;

        sColumn = &aTableHeader->mColumns[aReadBlock->mColumnOrder];
        sColumnValue = aRow + sColumn->mOffset + 1;
        STL_WRITE_INT64( &sCount, sColumnValue );

        while( sDataValue != sFence )
        {
            sCount += ( DTL_IS_NULL( sDataValue ) == STL_FALSE );
            sDataValue++;
        }

        STL_WRITE_INT64( sColumnValue, &sCount );
    }
    else
    {
        sDataValue = KNL_GET_BLOCK_FIRST_DATA_VALUE( aAggrFuncInfo->mInputArgList );

        if( DTL_IS_NULL( sDataValue ) == STL_FALSE )
        {
            sColumn = &aTableHeader->mColumns[aReadBlock->mColumnOrder];
            sColumnValue = aRow + sColumn->mOffset + 1;
            STL_WRITE_INT64( &sCount, sColumnValue );

            sCount += (aEndIdx - aStartIdx);
            STL_WRITE_INT64( sColumnValue, &sCount );
        }
    }

    return STL_SUCCESS;
}

stlStatus smdmihMergeRecordCountWithExpr( smdmihTableHeader  * aTableHeader,
                                          stlChar            * aRow,
                                          smlAggrFuncInfo    * aAggrFuncInfo,
                                          stlInt32             aStartIdx,
                                          stlInt32             aEndIdx,
                                          knlValueBlockList  * aReadBlock,
                                          knlValueBlockList  * aUpdateBlock,
                                          smlEnv             * aEnv )
{
    smdmifDefColumn   * sColumn;
    stlChar           * sColumnValue;
    dtlDataValue      * sDataValue;
    dtlDataValue      * sFence;
    stlInt64            sCount;
    knlExprEvalInfo   * sEvalInfo;

    /**
     * Aggregation내 Expression Stack 수행
     */

    sEvalInfo = aAggrFuncInfo->mEvalInfo[0];
    
    sEvalInfo->mBlockCount = aEndIdx - aStartIdx;
    sEvalInfo->mBlockIdx = aStartIdx;
        
    STL_TRY( knlEvaluateBlockExpression( sEvalInfo,
                                         KNL_ENV(aEnv) )
             == STL_SUCCESS );

    
    /**
     * merge aggregation
     */

    if( KNL_GET_BLOCK_IS_SEP_BUFF( aAggrFuncInfo->mInputArgList ) == STL_TRUE )
    {
        sDataValue = KNL_GET_BLOCK_DATA_VALUE( aAggrFuncInfo->mInputArgList, aStartIdx );
        sFence = sDataValue + aEndIdx - aStartIdx;

        sColumn = &aTableHeader->mColumns[aReadBlock->mColumnOrder];
        sColumnValue = aRow + sColumn->mOffset + 1;
        STL_WRITE_INT64( &sCount, sColumnValue );

        while( sDataValue != sFence )
        {
            sCount += ( DTL_IS_NULL( sDataValue ) == STL_FALSE );
            sDataValue++;
        }

        STL_WRITE_INT64( sColumnValue, &sCount );
    }
    else
    {
        sDataValue = KNL_GET_BLOCK_FIRST_DATA_VALUE( aAggrFuncInfo->mInputArgList );

        if( DTL_IS_NULL( sDataValue ) == STL_FALSE )
        {
            sColumn = &aTableHeader->mColumns[aReadBlock->mColumnOrder];
            sColumnValue = aRow + sColumn->mOffset + 1;
            STL_WRITE_INT64( &sCount, sColumnValue );

            sCount += (aEndIdx - aStartIdx);
            STL_WRITE_INT64( sColumnValue, &sCount );
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smdmihMergeRecordSum( smdmihTableHeader  * aTableHeader,
                                stlChar            * aRow,
                                smlAggrFuncInfo    * aAggrFuncInfo,
                                stlInt32             aStartIdx,
                                stlInt32             aEndIdx,
                                knlValueBlockList  * aReadBlock,
                                knlValueBlockList  * aUpdateBlock,
                                smlEnv             * aEnv )
{
    smdmifDefColumn   * sColumn;
    stlInt32            sBlockIdx;
    dtlDataValue        sDstValue;
    dtlDataValue      * sSrcValue;
    
    sDstValue.mType = KNL_GET_BLOCK_DB_TYPE( aAggrFuncInfo->mAggrValueList );

    /**
     * sDstValue.mType은 sum 의 결과타입은 DTL_TYPE_NUMBER임.
     * 예: SUM( float_type )  => DTL_TYPE_NUMBER
     *     SUM( number_type ) => DTL_TYPE_NUMBER
     */
    STL_TRY_THROW( dtlDataTypeGroup[ DTL_GET_BLOCK_DB_TYPE( aAggrFuncInfo->mInputArgList ) ]
                   == DTL_GROUP_NUMBER,
                   RAMP_NON_NUMERIC );

    STL_DASSERT( sDstValue.mType == DTL_TYPE_NUMBER );

    sColumn = &aTableHeader->mColumns[aReadBlock->mColumnOrder];
    sDstValue.mValue = aRow + sColumn->mOffset + 1;
    sDstValue.mBufferSize = sColumn->mAllocSize;
    sDstValue.mLength = *(aRow + sColumn->mOffset);

    if( ( ( aEndIdx - aStartIdx ) > 1 ) &&
        ( DTL_GET_BLOCK_ARG_NUM2( aAggrFuncInfo->mInputArgList ) == 0 ) &&
        ( DTL_GET_BLOCK_ARG_NUM1( aAggrFuncInfo->mInputArgList ) <= DTL_DECIMAL_INTEGER_DEFAULT_PRECISION ) )
    {
        STL_TRY( knlAddNumericForInteger( aAggrFuncInfo->mInputArgList,
                                          &sDstValue,
                                          aStartIdx,
                                          aEndIdx,
                                          KNL_ENV( aEnv ) )
                 == STL_SUCCESS );
    }
    else
    {
        for( sBlockIdx = aStartIdx; sBlockIdx < aEndIdx; sBlockIdx++ )
        {
            sSrcValue = KNL_GET_BLOCK_DATA_VALUE( aAggrFuncInfo->mInputArgList, sBlockIdx );
    
            if( DTL_IS_NULL( sSrcValue ) )
            {
                /* Do Nothing */
            }
            else
            {
                if( DTL_IS_NULL( &sDstValue ) )
                {
                    sDstValue.mLength = sSrcValue->mLength;
                    stlMemcpy( sDstValue.mValue, sSrcValue->mValue, sDstValue.mLength );
                }
                else
                {
                    STL_TRY( dtlNumericSum( &sDstValue,
                                            sSrcValue,
                                            KNL_ERROR_STACK( aEnv ) )
                             == STL_SUCCESS );
                }
            }
        }
    }
    
    *(aRow + sColumn->mOffset) = sDstValue.mLength;
    
    
    return STL_SUCCESS;

    STL_RAMP( RAMP_NON_NUMERIC );

    STL_TRY( smdmihMergeRecordAggr( aTableHeader,
                                    aRow,
                                    aAggrFuncInfo,
                                    aStartIdx,
                                    aEndIdx,
                                    aReadBlock,
                                    aUpdateBlock,
                                    aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smdmihMergeRecordSumWithExpr( smdmihTableHeader  * aTableHeader,
                                        stlChar            * aRow,
                                        smlAggrFuncInfo    * aAggrFuncInfo,
                                        stlInt32             aStartIdx,
                                        stlInt32             aEndIdx,
                                        knlValueBlockList  * aReadBlock,
                                        knlValueBlockList  * aUpdateBlock,
                                        smlEnv             * aEnv )
{
    smdmifDefColumn   * sColumn;
    stlInt64            sBlockCount = aEndIdx - aStartIdx;
    dtlDataValue        sDstValue;
    dtlDataValue      * sSrcValue;
    knlExprEvalInfo   * sEvalInfo;
    stlInt32            sBlockIdx;
    
    sDstValue.mType = KNL_GET_BLOCK_DB_TYPE( aAggrFuncInfo->mAggrValueList );

    /**
     * sDstValue.mType은 sum 의 결과타입은 DTL_TYPE_NUMBER임.
     * 예: SUM( float_type )  => DTL_TYPE_NUMBER
     *     SUM( number_type ) => DTL_TYPE_NUMBER
     */
    STL_TRY_THROW( dtlDataTypeGroup[ DTL_GET_BLOCK_DB_TYPE( aAggrFuncInfo->mInputArgList ) ]
                   == DTL_GROUP_NUMBER,
                   RAMP_NON_NUMERIC );

    STL_DASSERT( sDstValue.mType == DTL_TYPE_NUMBER );
    
    sColumn = &aTableHeader->mColumns[aReadBlock->mColumnOrder];
    sDstValue.mValue = aRow + sColumn->mOffset + 1;
    sDstValue.mBufferSize = sColumn->mAllocSize;
    sDstValue.mLength = *(aRow + sColumn->mOffset);

    sEvalInfo = aAggrFuncInfo->mEvalInfo[0];
    
    if( sBlockCount == 1 )
    {
        sEvalInfo->mBlockIdx = aStartIdx;
        
        STL_TRY( knlEvaluateOneExpression( sEvalInfo,
                                           KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        sSrcValue = KNL_GET_BLOCK_DATA_VALUE( aAggrFuncInfo->mInputArgList, aStartIdx );
    
        if( DTL_IS_NULL( sSrcValue ) )
        {
            /* Do Nothing */
        }
        else
        {
            if( DTL_IS_NULL( &sDstValue ) )
            {
                sDstValue.mLength = sSrcValue->mLength;
                stlMemcpy( sDstValue.mValue, sSrcValue->mValue, sDstValue.mLength );
            }
            else
            {
                STL_TRY( dtlNumericSum( &sDstValue,
                                        sSrcValue,
                                        KNL_ERROR_STACK( aEnv ) )
                         == STL_SUCCESS );
            }
        }
    }
    else
    {
        sEvalInfo->mBlockCount = sBlockCount;
        sEvalInfo->mBlockIdx = aStartIdx;
        
        STL_TRY( knlEvaluateBlockExpression( sEvalInfo,
                                             KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        if( ( ( aEndIdx - aStartIdx ) > 1 ) &&
            ( DTL_GET_BLOCK_ARG_NUM2( aAggrFuncInfo->mInputArgList ) == 0 ) &&
            ( DTL_GET_BLOCK_ARG_NUM1( aAggrFuncInfo->mInputArgList ) <= DTL_DECIMAL_INTEGER_DEFAULT_PRECISION ) )
        {
            STL_TRY( knlAddNumericForInteger( aAggrFuncInfo->mInputArgList,
                                              &sDstValue,
                                              aStartIdx,
                                              aEndIdx,
                                              KNL_ENV( aEnv ) )
                     == STL_SUCCESS );
        }
        else
        {
            for( sBlockIdx = aStartIdx; sBlockIdx < aEndIdx; sBlockIdx++ )
            {
                sSrcValue = KNL_GET_BLOCK_DATA_VALUE( aAggrFuncInfo->mInputArgList, sBlockIdx );
    
                if( DTL_IS_NULL( sSrcValue ) )
                {
                    /* Do Nothing */
                }
                else
                {
                    if( DTL_IS_NULL( &sDstValue ) )
                    {
                        sDstValue.mLength = sSrcValue->mLength;
                        stlMemcpy( sDstValue.mValue, sSrcValue->mValue, sDstValue.mLength );
                    }
                    else
                    {
                        STL_TRY( dtlNumericSum( &sDstValue,
                                                sSrcValue,
                                                KNL_ERROR_STACK( aEnv ) )
                                 == STL_SUCCESS );
                    }
                }
            }
        }
    }
    
    *(aRow + sColumn->mOffset) = sDstValue.mLength;
    
    return STL_SUCCESS;

    STL_RAMP( RAMP_NON_NUMERIC );

    STL_TRY( smdmihMergeRecordAggr( aTableHeader,
                                    aRow,
                                    aAggrFuncInfo,
                                    aStartIdx,
                                    aEndIdx,
                                    aReadBlock,
                                    aUpdateBlock,
                                    aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smdmihMergeRecordMin( smdmihTableHeader  * aTableHeader,
                                stlChar            * aRow,
                                smlAggrFuncInfo    * aAggrFuncInfo,
                                stlInt32             aStartIdx,
                                stlInt32             aEndIdx,
                                knlValueBlockList  * aReadBlock,
                                knlValueBlockList  * aUpdateBlock,
                                smlEnv             * aEnv )
{
    smdmifDefColumn   * sColumn;
    stlInt64            sBlockCount = aEndIdx - aStartIdx;
    dtlDataValue        sDstValue;
    dtlDataValue      * sSrcValue;
    dtlDataValue      * sOrgDstValue;
    /* dtlDataType         sDataType; */
    dtlBooleanType      sBoolean    = STL_FALSE;
    dtlDataValue        sTempValue  = { DTL_TYPE_BOOLEAN, STL_SIZEOF(dtlBooleanType), 0, & sBoolean };
    dtlDataValue      * sCompResult = & sTempValue;
    dtlValueEntry       sValueEntry[ 2 ];
    knlExprEvalInfo   * sEvalInfo;
    stlInt32            sLoop;
            

    /* sDataType = KNL_GET_BLOCK_DB_TYPE( aAggrFuncInfo->mAggrValueList ); */

    /**
     * non-numeric min aggregation 튜닝은 나중에
     */
    
    STL_TRY_THROW( dtlDataTypeGroup[ DTL_GET_BLOCK_DB_TYPE( aAggrFuncInfo->mInputArgList ) ]
                   == DTL_GROUP_NUMBER,
                   RAMP_NON_NUMERIC );

    /**
     * numeric min aggregation
     */
    
    sColumn = &aTableHeader->mColumns[aReadBlock->mColumnOrder];
    
    sDstValue.mValue = aRow + sColumn->mOffset + 1;
    sDstValue.mBufferSize = sColumn->mAllocSize;
    sDstValue.mLength = *(aRow + sColumn->mOffset);
    sDstValue.mType = KNL_GET_BLOCK_DB_TYPE( aAggrFuncInfo->mAggrValueList );

    sOrgDstValue = aAggrFuncInfo->mAggrValueList->mValueBlock->mDataValue;
    aAggrFuncInfo->mAggrValueList->mValueBlock->mDataValue = &sDstValue;
    
    sSrcValue = KNL_GET_BLOCK_DATA_VALUE( aAggrFuncInfo->mInputArgList, aStartIdx );
    sEvalInfo = aAggrFuncInfo->mEvalInfo[0];
    
    /**
     * merge aggregation
     */

    if( sBlockCount == 1 )
    {
        /**
         * Aggregation내 Expression Stack 수행
         */

        if( sEvalInfo != NULL )
        {
            sEvalInfo->mBlockIdx = aStartIdx;
        
            STL_TRY( knlEvaluateOneExpression( sEvalInfo,
                                               KNL_ENV(aEnv) )
                     == STL_SUCCESS );
        }
        
        /**
         * decide minimum value
         */
        
        if( !DTL_IS_NULL( sSrcValue ) )
        {
            if( DTL_IS_NULL( &sDstValue ) )
            {
                DTL_COPY_DATA_VALUE( sDstValue.mBufferSize,
                                     sSrcValue,
                                     &sDstValue );
            }
            else
            {
                sValueEntry[0].mValue.mDataValue = sSrcValue;
                sValueEntry[1].mValue.mDataValue = &sDstValue;
                
                STL_TRY( dtlOperIsLessThanNumericToNumeric( 2,
                                                            sValueEntry,
                                                            sCompResult,
                                                            NULL,
                                                            NULL,
                                                            NULL,
                                                            aEnv )
                         == STL_SUCCESS );

                if( DTL_BOOLEAN_IS_TRUE( sCompResult ) )
                {
                    DTL_COPY_DATA_VALUE( sDstValue.mBufferSize,
                                         sSrcValue,
                                         &sDstValue );
                }
            }
        }
    }
    else
    {
        /**
         * Aggregation내 Expression Stack 수행
         */

        if( sEvalInfo != NULL )
        {
            sEvalInfo->mBlockCount = sBlockCount;
            sEvalInfo->mBlockIdx = aStartIdx;
        
            STL_TRY( knlEvaluateBlockExpression( sEvalInfo,
                                                 KNL_ENV(aEnv) )
                     == STL_SUCCESS );
        }
        
        /**
         * decide minimum value
         */
        
        if( KNL_GET_BLOCK_IS_SEP_BUFF( aAggrFuncInfo->mInputArgList ) == STL_TRUE )
        {
            sValueEntry[1].mValue.mDataValue = &sDstValue;
            
            for( sLoop = 0 ; sLoop < sBlockCount ; sLoop++ )
            {
                if( !DTL_IS_NULL( sSrcValue ) )
                {
                    if( DTL_IS_NULL( &sDstValue ) )
                    {
                        DTL_COPY_DATA_VALUE( sDstValue.mBufferSize,
                                             sSrcValue,
                                             &sDstValue );
                    }
                    else
                    {
                        sValueEntry[0].mValue.mDataValue = sSrcValue;
                
                        STL_TRY( dtlOperIsLessThanNumericToNumeric( 2,
                                                                    sValueEntry,
                                                                    sCompResult,
                                                                    NULL,
                                                                    NULL,
                                                                    NULL,
                                                                    aEnv )
                                 == STL_SUCCESS );

                        if( DTL_BOOLEAN_IS_TRUE( sCompResult ) )
                        {
                            DTL_COPY_DATA_VALUE( sDstValue.mBufferSize,
                                                 sSrcValue,
                                                 &sDstValue );
                        }
                    }
                }
                
                sSrcValue++;
            }
        }
        else
        {
            if( !DTL_IS_NULL( sSrcValue ) )
            {
                if( DTL_IS_NULL( &sDstValue ) )
                {
                    DTL_COPY_DATA_VALUE( sDstValue.mBufferSize,
                                         sSrcValue,
                                         &sDstValue );
                }
                else
                {
                    sValueEntry[0].mValue.mDataValue = sSrcValue;
                    sValueEntry[1].mValue.mDataValue = &sDstValue;

                    STL_TRY( dtlOperIsLessThanNumericToNumeric( 2,
                                                                sValueEntry,
                                                                sCompResult,
                                                                NULL,
                                                                NULL,
                                                                NULL,
                                                                aEnv )
                             == STL_SUCCESS );

                    if( DTL_BOOLEAN_IS_TRUE( sCompResult ) )
                    {
                        DTL_COPY_DATA_VALUE( sDstValue.mBufferSize,
                                             sSrcValue,
                                             &sDstValue );
                    }
                }
            }
        }
    }
    
    aAggrFuncInfo->mAggrValueList->mValueBlock->mDataValue = sOrgDstValue;
    *(aRow + sColumn->mOffset) = sDstValue.mLength;

    return STL_SUCCESS;

    STL_RAMP( RAMP_NON_NUMERIC );

    STL_TRY( smdmihMergeRecordAggr( aTableHeader,
                                    aRow,
                                    aAggrFuncInfo,
                                    aStartIdx,
                                    aEndIdx,
                                    aReadBlock,
                                    aUpdateBlock,
                                    aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smdmihMergeRecordMax( smdmihTableHeader  * aTableHeader,
                                stlChar            * aRow,
                                smlAggrFuncInfo    * aAggrFuncInfo,
                                stlInt32             aStartIdx,
                                stlInt32             aEndIdx,
                                knlValueBlockList  * aReadBlock,
                                knlValueBlockList  * aUpdateBlock,
                                smlEnv             * aEnv )
{
    smdmifDefColumn   * sColumn;
    stlInt64            sBlockCount = aEndIdx - aStartIdx;
    dtlDataValue        sDstValue;
    dtlDataValue      * sOrgDstValue;
    /* dtlDataType         sDataType; */
    dtlDataValue      * sSrcValue;
    dtlBooleanType      sBoolean    = STL_FALSE;
    dtlDataValue        sTempValue  = { DTL_TYPE_BOOLEAN, STL_SIZEOF(dtlBooleanType), 0, & sBoolean };
    dtlDataValue      * sCompResult = & sTempValue;
    dtlValueEntry       sValueEntry[ 2 ];
    knlExprEvalInfo   * sEvalInfo;
    stlInt32            sLoop;
    

    /* sDataType = KNL_GET_BLOCK_DB_TYPE( aAggrFuncInfo->mAggrValueList ); */

    /**
     * non-numeric min aggregation 튜닝은 나중에
     */
    
    STL_TRY_THROW( dtlDataTypeGroup[ DTL_GET_BLOCK_DB_TYPE( aAggrFuncInfo->mInputArgList ) ]
                   == DTL_GROUP_NUMBER,
                   RAMP_NON_NUMERIC );

    /**
     * numeric max aggregation
     */
    
    sColumn = &aTableHeader->mColumns[aReadBlock->mColumnOrder];
    
    sDstValue.mValue = aRow + sColumn->mOffset + 1;
    sDstValue.mBufferSize = sColumn->mAllocSize;
    sDstValue.mLength = *(aRow + sColumn->mOffset);
    sDstValue.mType = KNL_GET_BLOCK_DB_TYPE( aAggrFuncInfo->mAggrValueList );

    sOrgDstValue = aAggrFuncInfo->mAggrValueList->mValueBlock->mDataValue;
    aAggrFuncInfo->mAggrValueList->mValueBlock->mDataValue = &sDstValue;
    
    sSrcValue = KNL_GET_BLOCK_DATA_VALUE( aAggrFuncInfo->mInputArgList, aStartIdx );
    sEvalInfo = aAggrFuncInfo->mEvalInfo[0];
        
    /**
     * merge aggregation
     */

    if( sBlockCount == 1 )
    {
        /**
         * Aggregation내 Expression Stack 수행
         */

        if( sEvalInfo != NULL )
        {
            sEvalInfo->mBlockIdx = aStartIdx;
        
            STL_TRY( knlEvaluateOneExpression( sEvalInfo,
                                               KNL_ENV(aEnv) )
                     == STL_SUCCESS );
        }

        /**
         * decide maximum value
         */
        
        if( !DTL_IS_NULL( sSrcValue ) )
        {
            if( DTL_IS_NULL( &sDstValue ) )
            {
                DTL_COPY_DATA_VALUE( sDstValue.mBufferSize,
                                     sSrcValue,
                                     &sDstValue );
            }
            else
            {
                sValueEntry[0].mValue.mDataValue = sSrcValue;
                sValueEntry[1].mValue.mDataValue = &sDstValue;
                
                STL_TRY( dtlOperIsGreaterThanNumericToNumeric( 2,
                                                               sValueEntry,
                                                               sCompResult,
                                                               NULL,
                                                               NULL,
                                                               NULL,
                                                               aEnv )
                         == STL_SUCCESS );

                if( DTL_BOOLEAN_IS_TRUE( sCompResult ) )
                {
                    DTL_COPY_DATA_VALUE( sDstValue.mBufferSize,
                                         sSrcValue,
                                         &sDstValue );
                }
            }
        }
    }
    else
    {
        /**
         * Aggregation내 Expression Stack 수행
         */

        if( sEvalInfo != NULL )
        {
            sEvalInfo->mBlockCount = sBlockCount;
            sEvalInfo->mBlockIdx = aStartIdx;
        
            STL_TRY( knlEvaluateBlockExpression( sEvalInfo,
                                                 KNL_ENV(aEnv) )
                     == STL_SUCCESS );
        }
        
        /**
         * decide maximum value
         */
        
        if( KNL_GET_BLOCK_IS_SEP_BUFF( aAggrFuncInfo->mInputArgList ) == STL_TRUE )
        {
            sValueEntry[1].mValue.mDataValue = &sDstValue;
            
            for( sLoop = 0 ; sLoop < sBlockCount ; sLoop++ )
            {
                if( !DTL_IS_NULL( sSrcValue ) )
                {
                    if( DTL_IS_NULL( &sDstValue ) )
                    {
                        DTL_COPY_DATA_VALUE( sDstValue.mBufferSize,
                                             sSrcValue,
                                             &sDstValue );
                    }
                    else
                    {
                        sValueEntry[0].mValue.mDataValue = sSrcValue;
                
                        STL_TRY( dtlOperIsGreaterThanNumericToNumeric( 2,
                                                                       sValueEntry,
                                                                       sCompResult,
                                                                       NULL,
                                                                       NULL,
                                                                       NULL,
                                                                       aEnv )
                                 == STL_SUCCESS );

                        if( DTL_BOOLEAN_IS_TRUE( sCompResult ) )
                        {
                            DTL_COPY_DATA_VALUE( sDstValue.mBufferSize,
                                                 sSrcValue,
                                                 &sDstValue );
                        }
                    }
                }
                
                sSrcValue++;
            }
        }
        else
        {
            if( !DTL_IS_NULL( sSrcValue ) )
            {
                if( DTL_IS_NULL( &sDstValue ) )
                {
                    DTL_COPY_DATA_VALUE( sDstValue.mBufferSize,
                                         sSrcValue,
                                         &sDstValue );
                }
                else
                {
                    sValueEntry[0].mValue.mDataValue = sSrcValue;
                    sValueEntry[1].mValue.mDataValue = &sDstValue;

                    STL_TRY( dtlOperIsGreaterThanNumericToNumeric( 2,
                                                                   sValueEntry,
                                                                   sCompResult,
                                                                   NULL,
                                                                   NULL,
                                                                   NULL,
                                                                   aEnv )
                             == STL_SUCCESS );

                    if( DTL_BOOLEAN_IS_TRUE( sCompResult ) )
                    {
                        DTL_COPY_DATA_VALUE( sDstValue.mBufferSize,
                                             sSrcValue,
                                             &sDstValue );
                    }
                }
            }
        }
    }
    
    aAggrFuncInfo->mAggrValueList->mValueBlock->mDataValue = sOrgDstValue;
    *(aRow + sColumn->mOffset) = sDstValue.mLength;

    return STL_SUCCESS;

    STL_RAMP( RAMP_NON_NUMERIC );

    STL_TRY( smdmihMergeRecordAggr( aTableHeader,
                                    aRow,
                                    aAggrFuncInfo,
                                    aStartIdx,
                                    aEndIdx,
                                    aReadBlock,
                                    aUpdateBlock,
                                    aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smdmihMergeRecordAggr( smdmihTableHeader  * aTableHeader,
                                 stlChar            * aRow,
                                 smlAggrFuncInfo    * aAggrFuncInfo,
                                 stlInt32             aStartIdx,
                                 stlInt32             aEndIdx,
                                 knlValueBlockList  * aReadBlock,
                                 knlValueBlockList  * aUpdateBlock,
                                 smlEnv             * aEnv )
{
    smdmifDefColumn   * sColumn;
    stlInt32            sColLenSize;
    stlInt64            sLength;
    stlUInt16           sColLen16;
    stlChar           * sKeyColumn = NULL;
    knlLogicalAddr      sVarColLa;
    stlInt64            sColLen64;
    stlChar           * sVarCol = NULL;
    dtlDataValue      * sDataValue = NULL;

    /**
     * fill read column list
     */

    sColumn = &aTableHeader->mColumns[aReadBlock->mColumnOrder];
    sColLenSize = aTableHeader->mColumnInfo[aReadBlock->mColumnOrder].mColLenSize;
    sDataValue = KNL_GET_BLOCK_FIRST_DATA_VALUE( aReadBlock );

    if( sColumn->mIsFixedPart == STL_TRUE )
    {
        sKeyColumn = aRow + sColumn->mOffset;
        if( sColLenSize == 1 )
        {
            sColLen16 = *(sKeyColumn);
        }
        else
        {
            STL_WRITE_INT16( &sColLen16, sKeyColumn );
        }

        sLength = sColLen16;
    }
    else
    {
        STL_WRITE_INT64( &sVarColLa, (aRow + sColumn->mOffset) );

        if( sVarColLa == KNL_LOGICAL_ADDR_NULL )
        {
            sLength = 0;
        }
        else
        {
            sVarCol = (stlChar*)KNL_TO_PHYSICAL_ADDR( sVarColLa );
            
            if( SMDMIF_VAR_COL_HAS_NEXT( sVarCol ) == STL_TRUE )
            {
                smdmifVarPartReadCol( sVarCol,
                                      aTableHeader->mColCombineMem1,
                                      &sColLen64 );
                sKeyColumn = aTableHeader->mColCombineMem1;
                sLength = sColLen64;
            }
            else
            {
                sKeyColumn = SMDMIF_VAR_COL_VALUE_POS( sVarCol );
                sLength = SMDMIF_VAR_COL_GET_LEN( sVarCol );
            }
            sColLenSize = 0;
        }
    }
        
    stlMemcpy( sDataValue->mValue,
               sKeyColumn + sColLenSize,
               sLength );
    sDataValue->mLength = sLength;

    if( (aEndIdx - aStartIdx) == 1 )
    {
        STL_TRY( aAggrFuncInfo->mAggrFuncModule->mBuildFuncPtr( aAggrFuncInfo,
                                                                aStartIdx,
                                                                aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( aAggrFuncInfo->mAggrFuncModule->mBlockBuildFuncPtr( aAggrFuncInfo,
                                                                     aStartIdx,
                                                                     aEndIdx - aStartIdx,
                                                                     aEnv )
                 == STL_SUCCESS );
    }
    
    /**
     * write columns to row
     */
    
    sColumn = &aTableHeader->mColumns[aUpdateBlock->mColumnOrder];
    sColLenSize = aTableHeader->mColumnInfo[aUpdateBlock->mColumnOrder].mColLenSize;
    sDataValue = KNL_GET_BLOCK_FIRST_DATA_VALUE( aUpdateBlock );

    if( sColumn->mIsFixedPart == STL_TRUE )
    {
        smdmihFixedRowWriteCol( aRow + sColumn->mOffset,
                                sColLenSize,
                                sDataValue->mValue,
                                sDataValue->mLength );
    }
    else
    {
        if( sDataValue->mLength == 0 )
        {
            sVarColLa = KNL_LOGICAL_ADDR_NULL;
        }
        else
        {
            STL_TRY( smdmifVarPartWriteCol( &aTableHeader->mBaseHeader,
                                            SMDMIF_TABLE_GET_CUR_VAR_PART( &aTableHeader->mBaseHeader ),
                                            &sVarColLa,
                                            sDataValue->mValue,
                                            sDataValue->mLength,
                                            aTableHeader->mBaseHeader.mVarColPieceMinLen,
                                            smdmifTablePrepareNextVarBlock,
                                            aEnv ) == STL_SUCCESS );
        }

        STL_WRITE_INT64( aRow + sColumn->mOffset, &sVarColLa );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} */

