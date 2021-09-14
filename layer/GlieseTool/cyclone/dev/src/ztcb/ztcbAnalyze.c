/*******************************************************************************
 * ztcbAnalyze.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $ $
 *
 * NOTES
 *
 *
 ******************************************************************************/

/**
 * @file ztcbAnalyze.c
 * @brief GlieseTool Cyclone Distributor Analyze Routines
 */

#include <goldilocks.h>
#include <ztc.h>

stlStatus ztcbAnalyzeInsert( ztcSlaveTableInfo * aTableInfo,
                             stlUInt64         * aHashValue,
                             stlInt32            aColIdx,
                             stlChar           * aData,
                             stlErrorStack     * aErrorStack )
{
    stlChar        * sPtr          = (stlChar*)aData;
    ztcColumnInfo  * sColumnInfo   = NULL;
    stlInt32         sOffset       = 0;
    stlInt32         sColIdx       = 0;
    stlInt16         sReadCol      = 0;
    
    stlInt32         sFirstColOrd  = 0;
    stlInt16         sColCount     = 0;
    stlUInt16        sSlotHdrSize  = 0;
    stlUInt64        sHashValue    = 0;
    stlInt64         sColValueLen  = 0;
    stlInt64         sTransId;
    stlUInt64        sScn;
    
    /**
     * Row Header 읽기
     */
    STL_READ_MOVE_INT64( &sTransId, aData, sOffset );
    STL_READ_MOVE_INT64( &sScn, aData, sOffset );
    STL_READ_MOVE_INT32( &sFirstColOrd, aData, sOffset );
    sPtr = aData + sOffset;
    
    ZTC_GET_ROW_HDR_SIZE( sPtr, &sSlotHdrSize );
    ZTC_GET_COLUMN_CNT( sPtr, &sColCount );
    sPtr += sSlotHdrSize;
    
    STL_TRY_THROW( sColCount > 0, RAMP_SUCCESS );
    
    STL_RING_FOREACH_ENTRY( &aTableInfo->mColumnList, sColumnInfo )
    {
        if( sColIdx >= aColIdx )
        {
            if( sColumnInfo->mIsPrimary == STL_TRUE )
            {
                /**
                 * Primary Key는 Continuous Column으로 나눠지지 않는다.
                 */
                ZTC_READ_COLUMN_VALUE_LEN_AND_MOVE_PTR( &sPtr, &sColValueLen );
                
                STL_TRY_THROW( sColValueLen <= sColumnInfo->mValueMaxSize &&
                               sColValueLen >= 0, RAMP_ERR_INVALID_VALUE );
                
                if( sColValueLen != 0 )
                {
                    sHashValue += dtlHash32Any( (stlUInt8*)sPtr, sColValueLen );    
                    //stlPrintf("INSERT HASHVALUE = %u[Length:%d]\n", gDtlHash32[sColumnInfo->mDataTypeId]( (stlUInt8*)sPtr, sColValueLen ),sColValueLen  );
                }
                
                sPtr += sColValueLen;
            }
            else
            {
                ZTC_READ_COLUMN_VALUE_LEN_AND_MOVE_PTR( &sPtr, &sColValueLen );
                
                /**
                 * Long Varchar/Varbinary가 아닐때만 체크한다.
                 */
                if( sColumnInfo->mDataTypeId != DTL_TYPE_LONGVARCHAR &&
                    sColumnInfo->mDataTypeId != DTL_TYPE_LONGVARBINARY )
                {
                    STL_TRY_THROW( sColValueLen <= sColumnInfo->mValueMaxSize, RAMP_ERR_INVALID_VALUE );
                }
                
                sPtr += sColValueLen;
            }
            
            if( ++sReadCol == sColCount )
            {
                break;
            }
        }
        sColIdx++;
    }
    
    *aHashValue += sHashValue;
    
    STL_RAMP( RAMP_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_FAIL_ANALYZE_LOG,
                      NULL,
                      aErrorStack,
                      "INSERT" );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztcbAnalyzeSuppLog( ztcSlaveTableInfo * aTableInfo,
                              stlUInt64         * aHashValue,
                              stlChar           * aData,
                              stlErrorStack     * aErrorStack )
{
    stlChar        * sPtr         = (stlChar*)aData;
    ztcColumnInfo  * sColumnInfo  = NULL;
    stlInt16         sColIdx      = 0;
    stlInt32         sOffset      = 0;
    stlInt16         sColCount    = 0;
    stlInt64         sColValueLen = 0;
    stlUInt64        sHashValue   = 0;
    stlBool          sUpdatePk;
    
    STL_READ_MOVE_INT8( &sUpdatePk, aData, sOffset );
    STL_READ_MOVE_INT16( &sColCount, aData, sOffset );
    sPtr = aData + sOffset;
    
    STL_TRY_THROW( sColCount > 0, RAMP_SUCCESS );
    
    /**
     * Primary Key를 얻어온다.
     * Update/Delete 시에 사용된다.
     */
    STL_RING_FOREACH_ENTRY( &aTableInfo->mColumnList, sColumnInfo )
    {
        if( sColumnInfo->mIsPrimary == STL_TRUE )
        {
            ZTC_READ_COLUMN_VALUE_LEN_AND_MOVE_PTR( &sPtr, &sColValueLen );
            
            STL_TRY_THROW( sColValueLen <= sColumnInfo->mValueMaxSize &&
                           sColValueLen >= 0, RAMP_ERR_INVALID_VALUE );
            
            if( sColValueLen != 0 )
            {
                sColumnInfo->mHashValue = dtlHash32Any( (stlUInt8*)sPtr, sColValueLen );
            }
            else
            {
                sColumnInfo->mHashValue = 0;
            }
            sHashValue += sColumnInfo->mHashValue;
            
            sPtr += sColValueLen;
            
            if( ++sColIdx == sColCount )
            {
                break;
            }
        }
    }
    
    *aHashValue = sHashValue;
    
    STL_RAMP( RAMP_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_FAIL_ANALYZE_LOG,
                      NULL,
                      aErrorStack,
                      "SUPPLEMENTAL LOG" );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus ztcbAnalyzeSuppUpdateColLog( ztcSlaveTableInfo * aTableInfo,
                                       stlChar           * aData,
                                       stlInt64            aDataSize,
                                       stlBool           * aKeyChanged,
                                       stlErrorStack     * aErrorStack )
{
    ztcColumnInfo  * sColumnInfo   = NULL;
    stlInt32         sOffset       = 0;
    stlInt32         sColOrd       = 0;
    stlInt32         sColIdx       = 0;
    stlInt16         sColCount     = 0;    
    stlInt16         sReadCol      = 0;
    stlBool          sKeyChanged   = STL_FALSE;
    
    STL_READ_MOVE_INT16( &sColCount, aData, sOffset );
    
    STL_TRY_THROW( sColCount > 0, RAMP_SUCCESS );
    
    STL_READ_MOVE_INT32( &sColOrd, aData, sOffset );
    
    STL_RING_FOREACH_ENTRY( &aTableInfo->mColumnList, sColumnInfo )
    {
        if( sColOrd == sColIdx )
        {
            if( sColumnInfo->mIsPrimary == STL_TRUE )
            {
                sKeyChanged = STL_TRUE;
                break;
            }
            
            if( ++sReadCol == sColCount )
            {
                break;
            }
            
            STL_READ_MOVE_INT32( &sColOrd, aData, sOffset );
        }
        sColIdx++;
    }
    
    STL_RAMP( RAMP_SUCCESS );
    
    *aKeyChanged = sKeyChanged;
    
    return STL_SUCCESS;
}


stlStatus ztcbAnalyzeUpdate( ztcSlaveTableInfo * aTableInfo,
                             stlChar           * aData,
                             stlInt64            aDataSize,
                             stlErrorStack     * aErrorStack )
{
    stlChar        * sPtr         = (stlChar*)aData;
    ztcColumnInfo  * sColumnInfo  = NULL;
    
    stlInt32        sOffset       = 0;
    stlInt32        sColIdx       = 0;
    stlInt16        sReadCol      = 0;
    
    stlInt64        sTransId;
    stlUInt64       sScn;
    stlUInt16       sSlotHdrSize  = 0;
    stlInt16        sRowColCount  = 0;        //해당 Log에 포함된 전체 Count
    stlInt32        sFirstColOrd  = 0;
    stlInt16        sColCount     = 0;        //Update된 ColumnCount
    stlInt8         sColOrd       = 0;
    stlInt64        sColValueLen  = 0;
    
    /**
     * Row Header 읽기
     */
    STL_READ_MOVE_INT64( &sTransId, aData, sOffset );
    STL_READ_MOVE_INT64( &sScn, aData, sOffset );
    sPtr = aData + sOffset;
    
    ZTC_GET_ROW_HDR_SIZE( sPtr, &sSlotHdrSize );
    ZTC_GET_COLUMN_CNT( sPtr, &sRowColCount );
    sPtr += sSlotHdrSize;
    
    sOffset = 0;
    STL_READ_MOVE_INT32( &sFirstColOrd, sPtr, sOffset );
    STL_READ_MOVE_INT16( &sColCount, sPtr, sOffset );
    sPtr += sOffset;
    
    STL_TRY_THROW( sColCount > 0, RAMP_SUCCESS );
    
    sOffset = 0;
    STL_READ_MOVE_INT8( &sColOrd, sPtr, sOffset );
    sPtr += sOffset;
    
    sColOrd += sFirstColOrd;
    
    STL_RING_FOREACH_ENTRY( &aTableInfo->mColumnList, sColumnInfo )
    {
        if( sColOrd == sColIdx )
        {
            if( sColumnInfo->mIsPrimary == STL_TRUE )
            {
                ZTC_READ_COLUMN_VALUE_LEN_AND_MOVE_PTR( &sPtr, &sColValueLen );
                
                STL_TRY_THROW( sColValueLen <= sColumnInfo->mValueMaxSize, RAMP_ERR_INVALID_VALUE );
                
                if( sColValueLen != 0 )
                {
                    sColumnInfo->mHashValue = dtlHash32Any( (stlUInt8*)sPtr, sColValueLen );
                    //stlPrintf("UPDATE HASHVALUE = %u[Length:%d]\n", gDtlHash32[sColumnInfo->mDataTypeId]( (stlUInt8*)sPtr, sColValueLen ),sColValueLen  );
                }
                else
                {
                    sColumnInfo->mHashValue = 0;
                }
                
                sPtr += sColValueLen;
            }
            else
            {
                ZTC_SKIP_COLUMN_AND_MOVE_PTR( &sPtr );
            }
            
            if( ++sReadCol == sColCount )
            {
                break;   
            }
            
            sOffset = 0;
            STL_READ_MOVE_INT8( &sColOrd, sPtr, sOffset );
            sPtr += sOffset;
            sColOrd += sFirstColOrd;
        }
        
        sColIdx++;
    }
    
    STL_RAMP( RAMP_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_FAIL_ANALYZE_LOG,
                      NULL,
                      aErrorStack,
                      "Update" );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus ztcbAnalyzeInsert4Update( ztcSlaveTableInfo * aTableInfo,
                                    stlChar           * aData,
                                    stlInt64            aDataSize,
                                    stlErrorStack     * aErrorStack )
{
    stlChar        * sPtr         = (stlChar*)aData;
    ztcColumnInfo  * sColumnInfo  = NULL;
    
    stlInt32        sOffset       = 0;
    stlInt32        sColIdx       = 0;
    stlInt16        sReadCol      = 0;
    
    stlInt32        sFirstColOrd  = 0;
    stlInt16        sColCount     = 0;
    stlUInt16       sSlotHdrSize  = 0;
    stlInt64        sColValueLen  = 0;
    stlInt64        sTransId;
    stlUInt64       sScn;
    
    /**
     * Row Header 읽기
     */
    STL_READ_MOVE_INT64( &sTransId, aData, sOffset );
    STL_READ_MOVE_INT64( &sScn, aData, sOffset );
    STL_READ_MOVE_INT32( &sFirstColOrd, aData, sOffset );
    sPtr = aData + sOffset;
    
    ZTC_GET_ROW_HDR_SIZE( sPtr, &sSlotHdrSize );
    ZTC_GET_COLUMN_CNT( sPtr, &sColCount );
    sPtr += sSlotHdrSize;
    
    STL_TRY_THROW( sColCount > 0, RAMP_SUCCESS );
    
    STL_RING_FOREACH_ENTRY( &aTableInfo->mColumnList, sColumnInfo )
    {
        if( sColIdx < sFirstColOrd )
        {
            sColIdx++;
            continue;   
        }
        
        if( sColumnInfo->mIsPrimary == STL_TRUE )
        {
            ZTC_READ_COLUMN_VALUE_LEN_AND_MOVE_PTR( &sPtr, &sColValueLen );
            
            STL_TRY_THROW( sColValueLen <= sColumnInfo->mValueMaxSize, RAMP_ERR_INVALID_VALUE );
            
            if( sColValueLen != 0 )
            {
                sColumnInfo->mHashValue = dtlHash32Any( (stlUInt8*)sPtr, sColValueLen );
                //stlPrintf("INSERT4UPDATE HASHVALUE = %u[Length:%d]\n", gDtlHash32[sColumnInfo->mDataTypeId]( (stlUInt8*)sPtr, sColValueLen ),sColValueLen  );
            }
            else
            {
                sColumnInfo->mHashValue = 0;
            }
            
            sPtr += sColValueLen;
        }
        else
        {
            ZTC_SKIP_COLUMN_AND_MOVE_PTR( &sPtr );
        }
            
        if( ++sReadCol == sColCount )
        {
            break;   
        }
        
        sColIdx++;
    }
    
    STL_RAMP( RAMP_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_FAIL_ANALYZE_LOG,
                      NULL,
                      aErrorStack,
                      "Insert For Update" );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztcbBuildAfterHashValue( ztcSlaveTableInfo * aTableInfo,
                                   stlUInt64         * aHashValue,
                                   stlErrorStack     * aErrorStack )
{
    ztcColumnInfo  * sColumnInfo = NULL;
    stlUInt64        sHashValue  = 0;
    
    STL_RING_FOREACH_ENTRY( &aTableInfo->mColumnList, sColumnInfo )
    {
        if( sColumnInfo->mIsPrimary == STL_TRUE )
        {
            sHashValue += sColumnInfo->mHashValue;
        }
    }
    
    *aHashValue = sHashValue;
    
    return STL_SUCCESS;
}

/** @} */
