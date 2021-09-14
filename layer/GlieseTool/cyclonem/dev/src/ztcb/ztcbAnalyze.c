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

extern ztcSlaveMgr       * gSlaveMgr;

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
    
    stlDebug("ztcbAnalyzeInsert %s\n", aTableInfo->mSlaveTable);

    /**
     * Row Header 읽기
     */
    ZTC_READ64_BYTEORDER_OFFSET( gSlaveMgr->mLogByteOrder, &sTransId, aData, sOffset );
    ZTC_READ64_BYTEORDER_OFFSET( gSlaveMgr->mLogByteOrder, &sScn, aData, sOffset );
    ZTC_READ32_BYTEORDER_OFFSET( gSlaveMgr->mLogByteOrder, &sFirstColOrd, aData, sOffset );
    sPtr = aData + sOffset;
    
    ZTC_GET_ROW_HDR_SIZE( sPtr, &sSlotHdrSize );
    stlDebug("ztcbAnalyzeInsert : sSlotHdrSize %d \n", sSlotHdrSize);
    ZTC_GET_COLUMN_CNT( sPtr, &sColCount );
    ZTC_ENDIAN16_TOMASTER( gSlaveMgr->mLogByteOrder, sColCount );
    stlDebug("ztcbAnalyzeInsert : sColCount %d \n", sColCount);

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
                ZTC_READ_COLUMN_VALUE_LEN_AND_MOVE_PTR( gSlaveMgr->mLogByteOrder, &sPtr, &sColValueLen );
                stlDebug("ztcbAnalyzeInsert (PK) %s: sColValueLen %d \n", sColumnInfo->mColumnName, sColValueLen);

                STL_TRY_THROW( sColValueLen <= sColumnInfo->mValueMaxSize &&
                               sColValueLen >= 0, RAMP_ERR_INVALID_VALUE );

                if( sColValueLen != 0 )
                {
                    sHashValue += dtlHash32Any( (stlUInt8*)sPtr, sColValueLen );
                    //stlPrintf("INSERT HASHVALUE = %u[Length:%d]\n", gDtlHash32[sColumnInfo->mDataTypeId]( (stlUInt8*)sPtr, sColValueLen ),sColValueLen  );
                    stlDebug("ztcbAnalyzeInsert : sHashValue %ld \n", sHashValue);
                }
                
                sPtr += sColValueLen;
            }
            else
            {
                ZTC_READ_COLUMN_VALUE_LEN_AND_MOVE_PTR( gSlaveMgr->mLogByteOrder, &sPtr, &sColValueLen );
                stlDebug("ztcbAnalyzeInsert %s: sColValueLen %ld, mDataTypeId %ld \n",
                         sColumnInfo->mColumnName,
                         sColValueLen,
                         sColumnInfo->mDataTypeId);

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
    
//    stlDebug("ztcbAnalyzeInsert : sHashValue %x \n", sHashValue);

    *aHashValue += sHashValue;
//    stlDebug("ztcbAnalyzeInsert : aHashValue %x \n", *aHashValue);
    
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
    
    stlDebug("ztcbAnalyzeSuppLog %s\n", aTableInfo->mSlaveTable);

    ZTC_READ8_BYTEORDER_OFFSET( gSlaveMgr->mLogByteOrder, &sUpdatePk, aData, sOffset );
    ZTC_READ16_BYTEORDER_OFFSET( gSlaveMgr->mLogByteOrder, &sColCount, aData, sOffset );
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
            ZTC_READ_COLUMN_VALUE_LEN_AND_MOVE_PTR( gSlaveMgr->mLogByteOrder, &sPtr, &sColValueLen );
            
            STL_TRY_THROW( sColValueLen <= sColumnInfo->mValueMaxSize &&
                           sColValueLen >= 0, RAMP_ERR_INVALID_VALUE );

            stlDebug("ztcbAnalyzeSuppLog %s: DataType %ld \n",
                     sColumnInfo->mColumnName,
                     sColumnInfo->mDataTypeId);

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
    
    stlDebug("table %s\n", aTableInfo->mSlaveTable);

    ZTC_READ16_BYTEORDER_OFFSET( gSlaveMgr->mLogByteOrder, &sColCount, aData, sOffset );
    
    STL_TRY_THROW( sColCount > 0, RAMP_SUCCESS );
    
    ZTC_READ32_BYTEORDER_OFFSET( gSlaveMgr->mLogByteOrder, &sColOrd, aData, sOffset );
    
    STL_RING_FOREACH_ENTRY( &aTableInfo->mColumnList, sColumnInfo )
    {
        if( sColOrd == sColIdx )
        {
            stlDebug("column name %s: DataType %ld \n",
                     sColumnInfo->mColumnName,
                     sColumnInfo->mDataTypeId);

            if( sColumnInfo->mIsPrimary == STL_TRUE )
            {
                sKeyChanged = STL_TRUE;
                break;
            }
            
            if( ++sReadCol == sColCount )
            {
                break;
            }
            
            ZTC_READ32_BYTEORDER_OFFSET( gSlaveMgr->mLogByteOrder, &sColOrd, aData, sOffset );
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
    
    stlDebug("table %s\n", aTableInfo->mSlaveTable);

    /**
     * Row Header 읽기
     */
    ZTC_READ64_BYTEORDER_OFFSET( gSlaveMgr->mLogByteOrder, &sTransId, aData, sOffset );
    ZTC_READ64_BYTEORDER_OFFSET( gSlaveMgr->mLogByteOrder, &sScn, aData, sOffset );
    sPtr = aData + sOffset;
    
    ZTC_GET_ROW_HDR_SIZE( sPtr, &sSlotHdrSize );
    ZTC_GET_COLUMN_CNT( sPtr, &sRowColCount );
    ZTC_ENDIAN16_TOMASTER( gSlaveMgr->mLogByteOrder, sRowColCount );

    sPtr += sSlotHdrSize;
    
    sOffset = 0;
    ZTC_READ32_BYTEORDER_OFFSET( gSlaveMgr->mLogByteOrder, &sFirstColOrd, sPtr, sOffset );
    ZTC_READ16_BYTEORDER_OFFSET( gSlaveMgr->mLogByteOrder, &sColCount, sPtr, sOffset );
    sPtr += sOffset;
    
    STL_TRY_THROW( sColCount > 0, RAMP_SUCCESS );
    
    sOffset = 0;
    ZTC_READ8_BYTEORDER_OFFSET( gSlaveMgr->mLogByteOrder, &sColOrd, sPtr, sOffset );
    sPtr += sOffset;
    
    sColOrd += sFirstColOrd;
    
    STL_RING_FOREACH_ENTRY( &aTableInfo->mColumnList, sColumnInfo )
    {
        if( sColOrd == sColIdx )
        {
            if( sColumnInfo->mIsPrimary == STL_TRUE )
            {
                stlDebug("column name %s: DataType %ld \n",
                         sColumnInfo->mColumnName,
                         sColumnInfo->mDataTypeId);

                ZTC_READ_COLUMN_VALUE_LEN_AND_MOVE_PTR( gSlaveMgr->mLogByteOrder, &sPtr, &sColValueLen );
                
                STL_TRY_THROW( sColValueLen <= sColumnInfo->mValueMaxSize, RAMP_ERR_INVALID_VALUE );
                
                stlDebug("ztcbAnalyzeUpdate : mDataTypeId %ld \n", sColumnInfo->mDataTypeId);

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
                ZTC_SKIP_COLUMN_AND_MOVE_PTR( gSlaveMgr->mLogByteOrder, &sPtr );
            }
            
            if( ++sReadCol == sColCount )
            {
                break;   
            }
            
            sOffset = 0;
            ZTC_READ8_BYTEORDER_OFFSET( gSlaveMgr->mLogByteOrder, &sColOrd, sPtr, sOffset );
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
    
    stlDebug("table %s\n", aTableInfo->mSlaveTable);

    /**
     * Row Header 읽기
     */
    ZTC_READ64_BYTEORDER_OFFSET( gSlaveMgr->mLogByteOrder, &sTransId, aData, sOffset );
    ZTC_READ64_BYTEORDER_OFFSET( gSlaveMgr->mLogByteOrder, &sScn, aData, sOffset );
    ZTC_READ32_BYTEORDER_OFFSET( gSlaveMgr->mLogByteOrder, &sFirstColOrd, aData, sOffset );
    sPtr = aData + sOffset;
    
    ZTC_GET_ROW_HDR_SIZE( sPtr, &sSlotHdrSize );
    ZTC_GET_COLUMN_CNT( sPtr, &sColCount );
    ZTC_ENDIAN16_TOMASTER( gSlaveMgr->mLogByteOrder, sColCount );

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
            stlDebug("column name %s: DataType %ld \n",
                     sColumnInfo->mColumnName,
                     sColumnInfo->mDataTypeId);

            ZTC_READ_COLUMN_VALUE_LEN_AND_MOVE_PTR( gSlaveMgr->mLogByteOrder, &sPtr, &sColValueLen );
            
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
            ZTC_SKIP_COLUMN_AND_MOVE_PTR( gSlaveMgr->mLogByteOrder, &sPtr );
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
