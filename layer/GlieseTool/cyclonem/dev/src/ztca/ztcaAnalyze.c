/*******************************************************************************
 * ztcaAnalyze.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: ztcaAnalyze.c 5800 2012-09-27 05:01:28Z bsyou $
 *
 * NOTES
 *
 *
 ******************************************************************************/

/**
 * @file ztcaAnalyze.c
 * @brief GlieseTool Cyclone Applier Routines 
 */

#include <goldilocks.h>
#include <ztc.h>

extern ztcSlaveMgr  * gSlaveMgr;

stlStatus ztcaIsMasterRow( stlChar       * aData,
                           stlBool       * aIsMasterRow,
                           stlInt32      * aFirstColOrd,
                           stlErrorStack * aErrorStack )
{
    stlChar     * sPtr = (stlChar*)aData;
    stlBool       sIsMasterRow;
    stlInt32      sOffset      = 0;
    
    stlInt64      sTransId;
    stlUInt64     sScn;
    stlInt32      sFirstColOrd = 0;
    
    /**
     * Row Header 읽기
     */
    ZTC_READ64_BYTEORDER_OFFSET( gSlaveMgr->mLogByteOrder, &sTransId, aData, sOffset );
    ZTC_READ64_BYTEORDER_OFFSET( gSlaveMgr->mLogByteOrder, &sScn, aData, sOffset );
    ZTC_READ32_BYTEORDER_OFFSET( gSlaveMgr->mLogByteOrder, &sFirstColOrd, aData, sOffset );
    sPtr = aData + sOffset;
    
    sIsMasterRow = ZTC_IS_MASTER_ROW( sPtr );
    
    *aIsMasterRow = sIsMasterRow;
    *aFirstColOrd = sFirstColOrd;
    
    return STL_SUCCESS;
}

stlStatus ztcaAnalyzeInsert( ztcApplierMgr       * aApplierMgr,
                             ztcApplierTableInfo * aTableInfo,
                             stlChar             * aData,
                             stlInt64              aDataSize,
                             stlInt64              aArrIdx,
                             stlInt32              aColIdx,
                             stlErrorStack       * aErrorStack )
{
    stlChar        * sPtr          = (stlChar*)aData;
    stlChar        * sNewValuePtr  = NULL;
    ztcColumnValue * sColumnValue  = NULL;
    stlInt32         sOffset       = 0;
    stlInt32         sColIdx       = 0;
    stlInt16         sReadCol      = 0;
    
    stlInt32         sFirstColOrd  = 0;
    stlInt16         sColCount     = 0;
    stlUInt16        sSlotHdrSize  = 0;
    stlInt64         sColValueLen  = 0;
    stlInt64         sTransId;
    stlUInt64        sScn;
    
    SQL_LONG_VARIABLE_LENGTH_STRUCT * sVariableStr = NULL;
    stlDebug("ztcaAnalyzeInsert \n");
    /**
     * Row Header 읽기
     */
    ZTC_READ64_BYTEORDER_OFFSET( gSlaveMgr->mLogByteOrder, &sTransId, aData, sOffset );
    ZTC_READ64_BYTEORDER_OFFSET( gSlaveMgr->mLogByteOrder, &sScn, aData, sOffset );
    ZTC_READ32_BYTEORDER_OFFSET( gSlaveMgr->mLogByteOrder, &sFirstColOrd, aData, sOffset );
    sPtr = aData + sOffset;
    stlDebug("ztcaAnalyzeInsert : sTransId %ld \n", sTransId);
    stlDebug("ztcaAnalyzeInsert : sScn %ld \n", sScn);
    stlDebug("ztcaAnalyzeInsert : sFirstColOrd %d \n", sFirstColOrd);

    ZTC_GET_ROW_HDR_SIZE( sPtr, &sSlotHdrSize );
    stlDebug("ztcaAnalyzeInsert : sSlotHdrSize %d \n", sSlotHdrSize);
    ZTC_GET_COLUMN_CNT( sPtr, &sColCount );
    ZTC_ENDIAN16_TOMASTER( gSlaveMgr->mLogByteOrder, sColCount );
    stlDebug("ztcaAnalyzeInsert : sColCount %d \n", sColCount);
    sPtr += sSlotHdrSize;
    
    STL_TRY_THROW( sColCount > 0, RAMP_SUCCESS );
    
    STL_RING_FOREACH_ENTRY( &aTableInfo->mColumnList, sColumnValue )
    {
        if( sColIdx >= aColIdx )
        {
            ZTC_READ_COLUMN_VALUE_LEN( gSlaveMgr->mLogByteOrder, &sPtr, &sColValueLen );
            stlDebug("ztcaAnalyzeInsert : mDataTypeId %ld \n", sColumnValue->mInfoPtr->mDataTypeId);

            if( sColumnValue->mInfoPtr->mDataTypeId == DTL_TYPE_LONGVARCHAR ||
                sColumnValue->mInfoPtr->mDataTypeId == DTL_TYPE_LONGVARBINARY )
            {
                sVariableStr = (SQL_LONG_VARIABLE_LENGTH_STRUCT *)&sColumnValue->mValue[ STL_SIZEOF( SQL_LONG_VARIABLE_LENGTH_STRUCT ) * aArrIdx ];
                
                if( sVariableStr->len == 0 )
                {
                    STL_TRY( stlAllocDynamicMem( &(aApplierMgr->mLongVaryingMem),
                                                 ZTC_LONG_VARYING_MEMORY_INIT_SIZE,
                                                 (void**)&sVariableStr->arr,
                                                 aErrorStack ) == STL_SUCCESS );
                    
                    sVariableStr->len = ZTC_LONG_VARYING_MEMORY_INIT_SIZE;
                }
                else
                {
                    if( sVariableStr->len < sColValueLen + sColumnValue->mValueSize[ aArrIdx ] )
                    {
                        /**
                         * 할당 받은 메모리보다 많은 메모리가 필요할 경우 새로운 메모리를 할당 받아야 한다.
                         */
                        STL_TRY( stlAllocDynamicMem( &(aApplierMgr->mLongVaryingMem),
                                                     ZTC_LONG_VARYING_MEMORY_INIT_SIZE * ( ( sVariableStr->len / ZTC_LONG_VARYING_MEMORY_INIT_SIZE ) + 1 ), 
                                                     (void**)&sNewValuePtr,
                                                     aErrorStack ) == STL_SUCCESS );
                        
                        stlMemmove( &sNewValuePtr[ sColValueLen ],                  //Destination 
                                    sVariableStr->arr,                              //Source
                                    sColumnValue->mValueSize[ aArrIdx ] );
                        
                        STL_TRY( stlFreeDynamicMem( &(aApplierMgr->mLongVaryingMem),
                                                    sVariableStr->arr,
                                                    aErrorStack ) == STL_SUCCESS );
                        
                        sVariableStr->arr = (void*)sNewValuePtr;
                        sVariableStr->len = ZTC_LONG_VARYING_MEMORY_INIT_SIZE * ( ( sVariableStr->len / ZTC_LONG_VARYING_MEMORY_INIT_SIZE ) + 1 );
                    }
                    else
                    {
                        stlMemmove( &sVariableStr->arr[ sColValueLen ],                //Destination 
                                    sVariableStr->arr,                                 //Source
                                    sColumnValue->mValueSize[ aArrIdx ] );
                    }
                }
                
                ZTC_READ_COLUMN_AND_MOVE_PTR( gSlaveMgr->mLogByteOrder,
                                              &sPtr,
                                              sVariableStr->arr,
                                              &sColValueLen );
            
                sColumnValue->mValueSize[ aArrIdx ] += sColValueLen;
            }
            else
            {
                if( sColumnValue->mValueSize[ aArrIdx ] > 0 )
                {
                    /**
                     * Continuous Column이므로 ColumnValue를 재구성해야 한다.
                     */
                    stlMemmove( &sColumnValue->mValue[ (sColumnValue->mInfoPtr->mValueMaxSize * aArrIdx) + sColValueLen ],       //Destination
                                &sColumnValue->mValue[ sColumnValue->mInfoPtr->mValueMaxSize * aArrIdx ],                        //Source
                                sColumnValue->mValueSize[ aArrIdx ] );
                }
            
                STL_TRY_THROW( sColValueLen <= sColumnValue->mInfoPtr->mValueMaxSize, RAMP_ERR_INVALID_VALUE );
                
                ZTC_READ_COLUMN_AND_MOVE_PTR( gSlaveMgr->mLogByteOrder,
                                              &sPtr,
                                              &sColumnValue->mValue[ sColumnValue->mInfoPtr->mValueMaxSize * aArrIdx ],
                                              &sColValueLen );
//                stlDebug("ztcaAnalyzeInsert : sColValueLen %ld \n", sColValueLen);

                sColumnValue->mValueSize[ aArrIdx ] += sColValueLen;
            }
            
            if( ++sReadCol == sColCount )
            {
                break;
            }
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
                      "INSERT" );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus ztcaAnalyzeSuppLog( ztcApplierTableInfo * aTableInfo,
                              stlChar             * aData,
                              stlInt64              aDataSize,
                              stlInt64              aArrIdx,
                              stlErrorStack       * aErrorStack )
{
    stlChar        * sPtr         = (stlChar*)aData;
    ztcColumnValue * sColumnValue = NULL;
    stlInt16         sColIdx      = 0;
    stlInt32         sOffset      = 0;
    stlInt16         sColCount    = 0;
    stlInt64         sColValueLen = 0;
    stlBool          sUpdatePk    = STL_FALSE;
    
    ZTC_READ8_BYTEORDER_OFFSET( gSlaveMgr->mLogByteOrder, &sUpdatePk, aData, sOffset );
    ZTC_READ16_BYTEORDER_OFFSET( gSlaveMgr->mLogByteOrder, &sColCount, aData, sOffset );
    sPtr = aData + sOffset;
    
    STL_TRY_THROW( sColCount > 0, RAMP_SUCCESS );
    
    /**
     * Primary Key를 얻어온다.
     * Update/Delete 시에 사용된다.
     */
    STL_RING_FOREACH_ENTRY( &aTableInfo->mColumnList, sColumnValue )
    {
        if( sColumnValue->mInfoPtr->mIsPrimary == STL_TRUE )
        {
            ZTC_READ_COLUMN_VALUE_LEN( gSlaveMgr->mLogByteOrder, &sPtr, &sColValueLen );
            
            /**
             * VALUE가 0 일 경우에는 sColValueLen은 0이 될 수 있음.
             */
            STL_TRY_THROW( sColValueLen <= sColumnValue->mInfoPtr->mValueMaxSize &&
                           sColValueLen >= 0, RAMP_ERR_INVALID_VALUE );
            
            ZTC_READ_COLUMN_AND_MOVE_PTR( gSlaveMgr->mLogByteOrder,
                                          &sPtr,
                                          &sColumnValue->mKeyValue[ sColumnValue->mInfoPtr->mValueMaxSize * aArrIdx ],
                                          &sColumnValue->mKeyValueSize[ aArrIdx ] );
            
            if( ++sColIdx == sColCount )
            {
                break;
            }
        }
    }
    
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


stlStatus ztcaAnalyzeSuppUpdateColLog( ztcApplierTableInfo * aTableInfo,
                                       stlChar             * aData,
                                       stlInt64              aDataSize,
                                       stlBool             * aNeedPrepare,
                                       stlErrorStack       * aErrorStack )
{
    ztcColumnValue * sColumnValue  = NULL;
    stlInt32         sOffset       = 0;
    stlInt32         sColOrd       = 0;
    stlInt32         sColIdx       = 0;
    stlInt16         sColCount     = 0;    
    stlInt16         sReadCol      = 0;
    stlBool          sNeedPrepare  = STL_FALSE;
    
    ZTC_READ16_BYTEORDER_OFFSET( gSlaveMgr->mLogByteOrder, &sColCount, aData, sOffset );
    
    STL_TRY_THROW( sColCount > 0, RAMP_SUCCESS );
    stlDebug("Column sColCount %d \n", sColCount);
    
    /**
     * SUPPLEMENTAL UPDATE COLUMN의 ColumnOrder는 4Byte이다.
     */
    ZTC_READ32_BYTEORDER_OFFSET( gSlaveMgr->mLogByteOrder, &sColOrd, aData, sOffset );
    
    stlDebug("Column sColOrd %d \n", sColOrd);

    STL_RING_FOREACH_ENTRY( &aTableInfo->mColumnList, sColumnValue )
    {
        stlDebug("Column sColIdx %d, %s \n", sColIdx, sColumnValue->mInfoPtr->mColumnName);
        if( sColOrd == sColIdx )
        {
            /**
             * 바로 전에 Update되지 않았고, 이번에 Update가 되는 Column이라면 Prepare가 필요하다.
             */
            if( sColumnValue->mIsUpdated != STL_TRUE )
            {
                sNeedPrepare = STL_TRUE;
            }
            
            sColumnValue->mIsUpdated = STL_TRUE;
            sReadCol++;
            
            if( sReadCol < sColCount )
            {
                ZTC_READ32_BYTEORDER_OFFSET( gSlaveMgr->mLogByteOrder, &sColOrd, aData, sOffset );
            }
        }
        else
        {
            /**
             * 바로 전에 Update되었지만 이번에 Update가 되지 않지 않은 Column이라면 Prepare가 필요하다.
             */
            if( sColumnValue->mIsUpdated == STL_TRUE )
            {
                sNeedPrepare = STL_TRUE;
            }
            sColumnValue->mIsUpdated = STL_FALSE;
        }
        
        sColIdx++;
    }
    
    *aNeedPrepare = sNeedPrepare;
    
    STL_RAMP( RAMP_SUCCESS );
    
    return STL_SUCCESS;
}

stlStatus ztcaAnalyzeSuppUpdateBefColLog( ztcApplierMgr       * aApplierMgr,
                                          ztcApplierTableInfo * aTableInfo,
                                          stlChar             * aData,
                                          stlInt64              aDataSize,
                                          stlInt64              aArrIdx,
                                          stlErrorStack       * aErrorStack )
{
    ztcColumnValue * sColumnValue  = NULL;
    stlInt32         sOffset       = 0;
    stlInt32         sColOrd       = 0;
    stlInt32         sColIdx       = 0;
    stlInt16         sColCount     = 0;
    stlInt16         sReadCol      = 0;

    stlInt64         sColValueLen  = 0;
    stlChar        * sPtr         = (stlChar*)aData;
    stlChar        * sNewValuePtr = NULL;

    SQL_LONG_VARIABLE_LENGTH_STRUCT * sVariableStr = NULL;

    STL_TRY_THROW( ztcmGetUpdateApplyMode( aErrorStack ) >= 1, RAMP_SUCCESS );   //해당 옵션을 사용하는지 확인하는 과정..

    ZTC_READ16_BYTEORDER_OFFSET( gSlaveMgr->mLogByteOrder, &sColCount, sPtr, sOffset );

    STL_TRY_THROW( sColCount > 0, RAMP_SUCCESS );

    ZTC_READ32_BYTEORDER_OFFSET( gSlaveMgr->mLogByteOrder, &sColOrd, sPtr, sOffset );
    sPtr += sOffset;

    STL_RING_FOREACH_ENTRY( &aTableInfo->mColumnList, sColumnValue )
    {
        if( sColOrd == sColIdx )
        {
            /**
             * Primary Key는 Supplemental Log를 사용하여 이미 세팅되었기 때문에 skip 한다.
             */
            if( sColumnValue->mInfoPtr->mIsPrimary != STL_TRUE )
            {
                STL_TRY_THROW( sColumnValue->mIsUpdated == STL_TRUE, RAMP_ERR_UPDATED_COLUMN );

                ZTC_READ_COLUMN_VALUE_LEN( gSlaveMgr->mLogByteOrder, &sPtr, &sColValueLen );

                stlDebug("ztcaAnalyzeSuppUpdateBefColLog : mDataTypeId %ld \n", sColumnValue->mInfoPtr->mDataTypeId);

                if( sColumnValue->mInfoPtr->mDataTypeId == DTL_TYPE_LONGVARCHAR ||
                    sColumnValue->mInfoPtr->mDataTypeId == DTL_TYPE_LONGVARBINARY )
                {
                    STL_DASSERT( aTableInfo->mUpdateLongVariableCol == STL_TRUE );

                    sVariableStr = (SQL_LONG_VARIABLE_LENGTH_STRUCT *)&sColumnValue->mBefValue[ STL_SIZEOF( SQL_LONG_VARIABLE_LENGTH_STRUCT ) * aArrIdx ];

                    if( sVariableStr->len == 0 )
                    {
                        STL_TRY( stlAllocDynamicMem( &(aApplierMgr->mLongVaryingMem),
                                                     ZTC_LONG_VARYING_MEMORY_INIT_SIZE,
                                                     (void**)&sVariableStr->arr,
                                                     aErrorStack ) == STL_SUCCESS );

                        sVariableStr->len = ZTC_LONG_VARYING_MEMORY_INIT_SIZE;
                    }
                    else
                    {
                        if( sVariableStr->len < sColValueLen + sColumnValue->mBefValueSize[ aArrIdx ] )
                        {
                            STL_TRY( stlAllocDynamicMem( &(aApplierMgr->mLongVaryingMem),
                                                         ZTC_LONG_VARYING_MEMORY_INIT_SIZE * ( ( sVariableStr->len / ZTC_LONG_VARYING_MEMORY_INIT_SIZE ) + 1 ),
                                                         (void**)&sNewValuePtr,
                                                         aErrorStack ) == STL_SUCCESS );

                            stlMemcpy( sNewValuePtr,
                                       sVariableStr->arr,
                                       sColumnValue->mBefValueSize[ aArrIdx ] );

                            STL_TRY( stlFreeDynamicMem( &(aApplierMgr->mLongVaryingMem),
                                                        sVariableStr->arr,
                                                        aErrorStack ) == STL_SUCCESS );

                            sVariableStr->arr = (void*)sNewValuePtr;
                            sVariableStr->len = ZTC_LONG_VARYING_MEMORY_INIT_SIZE * ( ( sVariableStr->len / ZTC_LONG_VARYING_MEMORY_INIT_SIZE ) + 1 );
                        }
                    }

                    ZTC_READ_COLUMN_AND_MOVE_PTR( gSlaveMgr->mLogByteOrder,
                                                  &sPtr,
                                                  sVariableStr->arr + sColumnValue->mBefValueSize[ aArrIdx ],
                                                  &sColValueLen );

                    sColumnValue->mBefValueSize[ aArrIdx ] += sColValueLen;
                }
                else
                {
                    if( sColumnValue->mBefValueSize[ aArrIdx ] > 0 )
                    {
                        /**
                         * Continuous Column인 경우...
                         */
                        STL_TRY_THROW( sColValueLen + sColumnValue->mBefValueSize[ aArrIdx ] <= sColumnValue->mInfoPtr->mValueMaxSize, RAMP_ERR_INVALID_VALUE );

                        /**
                         * UPDATE BEFORE COLUMN의 로그는 순서대로 쌓인다. (역순으로 저장되지 않는다....ex.. insert, insert4update )
                         * data : ABCDE --> A | BCD | E
                         */
                        ZTC_READ_COLUMN_AND_MOVE_PTR( gSlaveMgr->mLogByteOrder,
                                                      &sPtr,
                                                      &sColumnValue->mBefValue[ (sColumnValue->mInfoPtr->mValueMaxSize * aArrIdx) + sColumnValue->mBefValueSize[ aArrIdx ] ],
                                                      &sColValueLen );
                    }
                    else
                    {
                        STL_TRY_THROW( sColValueLen <= sColumnValue->mInfoPtr->mValueMaxSize, RAMP_ERR_INVALID_VALUE );

                        ZTC_READ_COLUMN_AND_MOVE_PTR( gSlaveMgr->mLogByteOrder,
                                                      &sPtr,
                                                      &sColumnValue->mBefValue[ sColumnValue->mInfoPtr->mValueMaxSize * aArrIdx ],
                                                      &sColValueLen );
                    }

                    sColumnValue->mBefValueSize[ aArrIdx ] += sColValueLen;
                }
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
            ZTC_READ32_BYTEORDER_OFFSET( gSlaveMgr->mLogByteOrder, &sColOrd, sPtr, sOffset );
            sPtr += sOffset;
        }

        STL_TRY_THROW( sColIdx < sColOrd, RAMP_ERR_INVALID_COLUMN_COUNT );
        sColIdx++;
    }

    STL_RAMP( RAMP_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_COLUMN_COUNT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_FAIL_ANALYZE_LOG,
                      NULL,
                      aErrorStack,
                      "Column Count. Supplemental Update Before Column Log" );
    }
    STL_CATCH( RAMP_ERR_INVALID_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_FAIL_ANALYZE_LOG,
                      NULL,
                      aErrorStack,
                      "Update. Supplemental Update Before Column Log" );
    }
    STL_CATCH( RAMP_ERR_UPDATED_COLUMN )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_FAIL_ANALYZE_LOG,
                      NULL,
                      aErrorStack,
                      "Updated Flag. Supplemental Update Before Column Log" );
    }
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztcaValidateSuppUpdateColLog( ztcApplierTableInfo * aTableInfo,
                                        stlChar             * aData,
                                        stlInt64              aDataSize,
                                        stlBool             * aNeedPrepare,
                                        stlErrorStack       * aErrorStack )
{
    ztcColumnValue * sColumnValue = NULL;
    stlInt32         sOffset      = 0;
    stlInt32         sColOrd      = 0;
    stlInt32         sColIdx      = 0;
    stlInt16         sColCount    = 0;    
    stlInt16         sReadCol     = 0;
    stlBool          sNeedPrepare = STL_FALSE;
    
    ZTC_READ16_BYTEORDER_OFFSET( gSlaveMgr->mLogByteOrder, &sColCount, aData, sOffset );
    
    STL_TRY_THROW( sColCount > 0, RAMP_SUCCESS );
    
    ZTC_READ32_BYTEORDER_OFFSET( gSlaveMgr->mLogByteOrder, &sColOrd, aData, sOffset );
    
    STL_RING_FOREACH_ENTRY( &aTableInfo->mColumnList, sColumnValue )
    {
        if( sColOrd == sColIdx )
        {
            /**
             * 바로 전에 Update되지 않았고, 이번에 Update가 되는 Column이라면 Prepare가 필요하다.
             */
            if( sColumnValue->mIsUpdated != STL_TRUE )
            {
                sNeedPrepare = STL_TRUE;
                break;
            }
            
            sReadCol++;
            
            if( sReadCol < sColCount )
            {
                ZTC_READ32_BYTEORDER_OFFSET( gSlaveMgr->mLogByteOrder, &sColOrd, aData, sOffset );
            }
        }
        else
        {
            /**
             * 바로 전에 Update되었지만 이번에 Update가 되지 않지 않은 Column이라면 Prepare가 필요하다.
             */
            if( sColumnValue->mIsUpdated == STL_TRUE )
            {
                sNeedPrepare = STL_TRUE;
                break;
            }
        }
        
        sColIdx++;
    }
    
    *aNeedPrepare = sNeedPrepare;
    
    STL_RAMP( RAMP_SUCCESS );
    
    return STL_SUCCESS;
}

stlStatus ztcaAnalyzeUpdate( ztcApplierMgr       * aApplierMgr,
                             ztcApplierTableInfo * aTableInfo,
                             stlChar             * aData,
                             stlInt64              aDataSize,
                             stlInt64              aArrIdx,
                             stlErrorStack       * aErrorStack )
{
    stlChar        * sPtr         = (stlChar*)aData;
    ztcColumnValue * sColumnValue = NULL;
    
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
    
    SQL_LONG_VARIABLE_LENGTH_STRUCT * sVariableStr = NULL;
    
    /**
     * Row Header 읽기
     */
    ZTC_READ64_BYTEORDER_OFFSET( gSlaveMgr->mLogByteOrder, &sTransId, aData, sOffset );
    ZTC_READ64_BYTEORDER_OFFSET( gSlaveMgr->mLogByteOrder, &sScn, aData, sOffset );
    sPtr = aData + sOffset;
    
    ZTC_GET_ROW_HDR_SIZE( sPtr, &sSlotHdrSize );
    ZTC_GET_COLUMN_CNT( sPtr, &sRowColCount );
    ZTC_ENDIAN16_TOMASTER( gSlaveMgr->mLogByteOrder, sColCount );
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
    
    STL_RING_FOREACH_ENTRY( &aTableInfo->mColumnList, sColumnValue )
    {
        if( sColOrd == sColIdx )
        {
            /**
             * Update Column에서 Update된다고 세팅되어 있어야 함!!
             *
             * Continuous Column이 올수 없다.
             * Merge하는 과정이 필요없음. 
             */
            STL_TRY_THROW( sColumnValue->mIsUpdated == STL_TRUE, RAMP_ERR_UPDATED_COLUMN );
            
            ZTC_READ_COLUMN_VALUE_LEN( gSlaveMgr->mLogByteOrder, &sPtr, &sColValueLen );
            
            stlDebug("ztcaAnalyzeUpdate : %s, mDataTypeId %ld \n",
                     sColumnValue->mInfoPtr->mColumnName,
                     sColumnValue->mInfoPtr->mDataTypeId);

            if( sColumnValue->mInfoPtr->mDataTypeId == DTL_TYPE_LONGVARCHAR ||
                sColumnValue->mInfoPtr->mDataTypeId == DTL_TYPE_LONGVARBINARY )
            {
                sVariableStr = (SQL_LONG_VARIABLE_LENGTH_STRUCT *)&sColumnValue->mValue[ STL_SIZEOF( SQL_LONG_VARIABLE_LENGTH_STRUCT ) * aArrIdx ];
                STL_TRY_THROW( sVariableStr->len == 0, RAMP_ERR_INVALID_LONG_VARIABLE );
                
                STL_TRY( stlAllocDynamicMem( &(aApplierMgr->mLongVaryingMem),
                                             ZTC_LONG_VARYING_MEMORY_INIT_SIZE,
                                             (void**)&sVariableStr->arr,
                                             aErrorStack ) == STL_SUCCESS );
                    
                sVariableStr->len = ZTC_LONG_VARYING_MEMORY_INIT_SIZE;
                
                ZTC_READ_COLUMN_AND_MOVE_PTR( gSlaveMgr->mLogByteOrder,
                                              &sPtr,
                                              sVariableStr->arr,
                                              &sColumnValue->mValueSize[ aArrIdx ] );

                /**
                 * Long Variable Column을 나중에 Free해 주기 위해서 세팅한다.
                 */
                aTableInfo->mUpdateLongVariableCol = STL_TRUE;
            }
            else
            {
                STL_TRY_THROW( sColValueLen <= sColumnValue->mInfoPtr->mValueMaxSize, RAMP_ERR_INVALID_VALUE );
            
                ZTC_READ_COLUMN_AND_MOVE_PTR( gSlaveMgr->mLogByteOrder,
                                              &sPtr,
                                              &sColumnValue->mValue[ sColumnValue->mInfoPtr->mValueMaxSize * aArrIdx ],
                                              &sColumnValue->mValueSize[ aArrIdx ] );
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
        
        STL_TRY_THROW( sColIdx < sColOrd, RAMP_ERR_INVALID_COLUMN_COUNT );
        sColIdx++;
    }
    
    STL_RAMP( RAMP_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_LONG_VARIABLE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_FAIL_ANALYZE_LOG,
                      NULL,
                      aErrorStack,
                      "Update - Long Variable Type" );
    }
    STL_CATCH( RAMP_ERR_INVALID_COLUMN_COUNT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_FAIL_ANALYZE_LOG,
                      NULL,
                      aErrorStack,
                      "Column Count" );
    }
    STL_CATCH( RAMP_ERR_INVALID_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_FAIL_ANALYZE_LOG,
                      NULL,
                      aErrorStack,
                      "Update" );
    }
    STL_CATCH( RAMP_ERR_UPDATED_COLUMN )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_FAIL_ANALYZE_LOG,
                      NULL,
                      aErrorStack,
                      "Updated Flag" );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus ztcaAnalyzeInsert4Update( ztcApplierMgr       * aApplierMgr,
                                    ztcApplierTableInfo * aTableInfo,
                                    stlChar             * aData,
                                    stlInt64              aDataSize,
                                    stlInt64              aArrIdx,
                                    stlErrorStack       * aErrorStack )
{
    stlChar        * sPtr         = (stlChar*)aData;
    stlChar        * sNewValuePtr = NULL;
    ztcColumnValue * sColumnValue = NULL;
    
    stlInt32        sOffset       = 0;
    stlInt32        sColIdx       = 0;
    stlInt16        sReadCol      = 0;
    
    stlInt32        sFirstColOrd  = 0;
    stlInt16        sColCount     = 0;
    stlUInt16       sSlotHdrSize  = 0;
    stlInt64        sColValueLen  = 0;
    stlInt64        sTransId;
    stlUInt64       sScn;
    
    SQL_LONG_VARIABLE_LENGTH_STRUCT * sVariableStr = NULL;
    
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
    
    STL_RING_FOREACH_ENTRY( &aTableInfo->mColumnList, sColumnValue )
    {
        if( sColIdx < sFirstColOrd )
        {
            sColIdx++;
            continue;   
        }
        
        if( sColumnValue->mIsUpdated == STL_TRUE ) 
        {
            ZTC_READ_COLUMN_VALUE_LEN( gSlaveMgr->mLogByteOrder, &sPtr, &sColValueLen );
            
            stlDebug("ztcaAnalyzeInsert4Update : mDataTypeId %ld \n", sColumnValue->mInfoPtr->mDataTypeId);

            if( sColumnValue->mInfoPtr->mDataTypeId == DTL_TYPE_LONGVARCHAR || 
                sColumnValue->mInfoPtr->mDataTypeId == DTL_TYPE_LONGVARBINARY )
            {
                sVariableStr = (SQL_LONG_VARIABLE_LENGTH_STRUCT *)&sColumnValue->mValue[ STL_SIZEOF( SQL_LONG_VARIABLE_LENGTH_STRUCT ) * aArrIdx ];
               
                if( sVariableStr->len == 0 )
                {
                    STL_TRY( stlAllocDynamicMem( &(aApplierMgr->mLongVaryingMem),
                                                 ZTC_LONG_VARYING_MEMORY_INIT_SIZE,
                                                 (void**)&sVariableStr->arr,
                                                 aErrorStack ) == STL_SUCCESS );
                    
                    sVariableStr->len = ZTC_LONG_VARYING_MEMORY_INIT_SIZE;
                }
                else
                {
                    if( sVariableStr->len < sColValueLen + sColumnValue->mValueSize[ aArrIdx ] )
                    {
                        /**
                         * 할당 받은 메모리보다 많은 메모리가 필요할 경우 새로운 메모리를 할당 받아야 한다.
                         */
                        STL_TRY( stlAllocDynamicMem( &(aApplierMgr->mLongVaryingMem),
                                                     ZTC_LONG_VARYING_MEMORY_INIT_SIZE * ( ( sVariableStr->len / ZTC_LONG_VARYING_MEMORY_INIT_SIZE ) + 1 ), 
                                                     (void**)&sNewValuePtr,
                                                     aErrorStack ) == STL_SUCCESS );
                        
                        stlMemmove( &sNewValuePtr[ sColValueLen ],                  //Destination 
                                    sVariableStr->arr,                              //Source
                                    sColumnValue->mValueSize[ aArrIdx ] );
                        
                        STL_TRY( stlFreeDynamicMem( &(aApplierMgr->mLongVaryingMem),
                                                    sVariableStr->arr,
                                                    aErrorStack ) == STL_SUCCESS );
                        
                        sVariableStr->arr = (void*)sNewValuePtr;
                        sVariableStr->len = ZTC_LONG_VARYING_MEMORY_INIT_SIZE * ( ( sVariableStr->len / ZTC_LONG_VARYING_MEMORY_INIT_SIZE ) + 1 );
                    }
                    else
                    {
                        stlMemmove( &sVariableStr->arr[ sColValueLen ],                //Destination 
                                    sVariableStr->arr,                                 //Source
                                    sColumnValue->mValueSize[ aArrIdx ] );
                    }
                }
                
                ZTC_READ_COLUMN_AND_MOVE_PTR( gSlaveMgr->mLogByteOrder,
                                              &sPtr,
                                              sVariableStr->arr,
                                              &sColValueLen );
            
                sColumnValue->mValueSize[ aArrIdx ] += sColValueLen;
                
                /**
                 * Long Variable Column을 나중에 Free해 주기 위해서 세팅한다.
                 */
                aTableInfo->mUpdateLongVariableCol = STL_TRUE;
            }
            else
            {
                if( sColumnValue->mValueSize[ aArrIdx ] > 0 )
                {
                    /**
                     * Continuous Column이므로 ColumnValue를 재구성해야 한다.
                     */
                    stlMemmove( &sColumnValue->mValue[ (sColumnValue->mInfoPtr->mValueMaxSize * aArrIdx) + sColValueLen ],       //Destination
                                &sColumnValue->mValue[ sColumnValue->mInfoPtr->mValueMaxSize * aArrIdx ],                        //Source
                                sColumnValue->mValueSize[ aArrIdx ] );
                }
            
                STL_TRY_THROW( sColValueLen <= sColumnValue->mInfoPtr->mValueMaxSize, RAMP_ERR_INVALID_VALUE );
            
                ZTC_READ_COLUMN_AND_MOVE_PTR( gSlaveMgr->mLogByteOrder,
                                              &sPtr,
                                              &sColumnValue->mValue[ sColumnValue->mInfoPtr->mValueMaxSize * aArrIdx ],
                                              &sColValueLen );
                
                sColumnValue->mValueSize[ aArrIdx ] += sColValueLen;    
            }
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

stlStatus ztcaAnalyzeAppend4Update( ztcApplierMgr       * aApplierMgr,
                                    ztcApplierTableInfo * aTableInfo,
                                    stlChar             * aData,
                                    stlInt64              aDataSize,
                                    stlInt64              aArrIdx,
                                    stlErrorStack       * aErrorStack )
{
    stlChar        * sPtr         = (stlChar*)aData;
    ztcColumnValue * sColumnValue = NULL;
    
    stlInt32        sOffset       = 0;
    stlInt32        sColIdx       = 0;
    stlInt16        sReadCol      = 0;
    stlInt32        sFirstColOrd  = 0;
    stlInt64        sColValueLen  = 0;
    
    stlInt16        sAppendPieceColCount = 0;
    stlInt16        sAppendPieceBodySize = 0;
    stlInt16        sTmpDstPieceHdrSize;
    stlInt64        sTransId;
    stlUInt64       sScn;
    
    SQL_LONG_VARIABLE_LENGTH_STRUCT * sVariableStr = NULL;
    
    /**
     * Row Header 읽기
     */
    ZTC_READ64_BYTEORDER_OFFSET( gSlaveMgr->mLogByteOrder, &sTransId, aData, sOffset );
    ZTC_READ64_BYTEORDER_OFFSET( gSlaveMgr->mLogByteOrder, &sScn, aData, sOffset );
    ZTC_READ16_BYTEORDER_OFFSET( gSlaveMgr->mLogByteOrder, &sTmpDstPieceHdrSize, aData, sOffset );
    sOffset += sTmpDstPieceHdrSize;
    
    ZTC_READ32_BYTEORDER_OFFSET( gSlaveMgr->mLogByteOrder, &sFirstColOrd, aData, sOffset );
    ZTC_READ16_BYTEORDER_OFFSET( gSlaveMgr->mLogByteOrder, &sAppendPieceColCount, aData, sOffset );
    ZTC_READ16_BYTEORDER_OFFSET( gSlaveMgr->mLogByteOrder, &sAppendPieceBodySize, aData, sOffset );
    sPtr = aData + sOffset;
    
    STL_RING_FOREACH_ENTRY( &aTableInfo->mColumnList, sColumnValue )
    {
        if( sColIdx < sFirstColOrd )
        {
            sColIdx++;
            continue;   
        }
        
        if( sColumnValue->mIsUpdated == STL_TRUE )
        {
            /**
             * Continuous Column이 올수 없다.
             * Merge하는 과정이 필요없음. 
             */
            ZTC_READ_COLUMN_VALUE_LEN( gSlaveMgr->mLogByteOrder, &sPtr, &sColValueLen );
            
            stlDebug("ztcaAnalyzeAppend4Update : mDataTypeId %ld \n", sColumnValue->mInfoPtr->mDataTypeId);

            if( sColumnValue->mInfoPtr->mDataTypeId == DTL_TYPE_LONGVARCHAR ||
                sColumnValue->mInfoPtr->mDataTypeId == DTL_TYPE_LONGVARBINARY )
            {
                sVariableStr = (SQL_LONG_VARIABLE_LENGTH_STRUCT *)&sColumnValue->mValue[ STL_SIZEOF( SQL_LONG_VARIABLE_LENGTH_STRUCT ) * aArrIdx ];
                
                STL_TRY_THROW( sVariableStr->len == 0, RAMP_ERR_INVALID_LONG_VARIABLE );
                
                STL_TRY( stlAllocDynamicMem( &(aApplierMgr->mLongVaryingMem),
                                             ZTC_LONG_VARYING_MEMORY_INIT_SIZE,
                                             (void**)&sVariableStr->arr,
                                             aErrorStack ) == STL_SUCCESS );
                    
                sVariableStr->len = ZTC_LONG_VARYING_MEMORY_INIT_SIZE;
                
                ZTC_READ_COLUMN_AND_MOVE_PTR( gSlaveMgr->mLogByteOrder,
                                              &sPtr,
                                              sVariableStr->arr,
                                              &sColumnValue->mValueSize[ aArrIdx ] );
                
                /**
                 * Long Variable Column을 나중에 Free해 주기 위해서 세팅한다.
                 */
                aTableInfo->mUpdateLongVariableCol = STL_TRUE;
            }
            else
            {
                STL_TRY_THROW( sColValueLen <= sColumnValue->mInfoPtr->mValueMaxSize, RAMP_ERR_INVALID_VALUE );
            
                ZTC_READ_COLUMN_AND_MOVE_PTR( gSlaveMgr->mLogByteOrder,
                                              &sPtr,
                                              &sColumnValue->mValue[ sColumnValue->mInfoPtr->mValueMaxSize * aArrIdx ],
                                              &sColumnValue->mValueSize[ aArrIdx ] );
            }
        }
        else
        {
            ZTC_SKIP_COLUMN_AND_MOVE_PTR( gSlaveMgr->mLogByteOrder, &sPtr );
        }
        
        if( ++sReadCol == sAppendPieceColCount )
        {
            break;   
        }
        
        sColIdx++;
    }
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_LONG_VARIABLE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_FAIL_ANALYZE_LOG,
                      NULL,
                      aErrorStack,
                      "Append For Update - Long Variable Type" );
    }
    STL_CATCH( RAMP_ERR_INVALID_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTC_ERRCODE_FAIL_ANALYZE_LOG,
                      NULL,
                      aErrorStack,
                      "Append For Update" );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus ztcaMovePrimaryValue( ztcApplierTableInfo * aTableInfo,
                                stlInt64              aIdx,
                                stlErrorStack       * aErrorStack )
{
    ztcColumnValue * sColumnValue = NULL;
    
    STL_RING_FOREACH_ENTRY( &aTableInfo->mColumnList, sColumnValue )
    {
        if( sColumnValue->mInfoPtr->mIsPrimary == STL_TRUE )
        {
            stlMemcpy( &sColumnValue->mKeyValue[0], 
                       &sColumnValue->mKeyValue[ sColumnValue->mInfoPtr->mValueMaxSize * aIdx ], 
                       sColumnValue->mKeyValueSize[ aIdx ] );
            
            sColumnValue->mKeyValueSize[0] = sColumnValue->mKeyValueSize[ aIdx ];
        }
    }
    
    return STL_SUCCESS;
}

/** @} */
