/*******************************************************************************
 * smdmpfScan.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: smdmpfScan.c 13901 2014-11-13 01:31:33Z mkkim $
 *
 * NOTES
 *
 *
 ******************************************************************************/


#include <stl.h>
#include <dtl.h>
#include <knl.h>
#include <scl.h>
#include <smlDef.h>
#include <smDef.h>
#include <sms.h>
#include <smxm.h>
#include <smxl.h>
#include <smo.h>
#include <smp.h>
#include <sme.h>
#include <sma.h>
#include <smdDef.h>
#include <smdmpfRowDef.h>
#include <smdmpfDef.h>
#include <smdmpf.h>


/**
 * @file smdmpfScan.c
 * @brief Storage Manager Layer Memory Heap Table Scanning Routines
 */

/**
 * @addtogroup smd
 * @{
 */


/**
 * @brief 주어진 Iterator를 초기화한다
 * @param[in,out] aIterator 초기화할 iterator
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smdmpfInitIterator ( void   * aIterator,
                               smlEnv * aEnv )
{
    smdmpfIterator * sIterator = (smdmpfIterator*)aIterator;
    void           * sSegHandle;

    sSegHandle = SME_GET_SEGMENT_HANDLE( sIterator->mCommon.mRelationHandle );
    
    sIterator->mRowRid.mTbsId = smsGetTbsId( sSegHandle );
    sIterator->mRowRid.mPageId = SMP_NULL_PID;
    sIterator->mRowRid.mOffset = -1;
    sIterator->mPageBufPid = SMP_NULL_PID;
    sIterator->mAgableViewScn = smxlGetAgableViewScn( aEnv );
    
    /**
     * Table Mutating 조건의 위배 여부를 검사한다.
     */
    STL_TRY( smaCheckMutating( sIterator->mCommon.mStatement,
                               sIterator->mCommon.mRelationHandle,
                               aEnv )
             == STL_SUCCESS );
    
    STL_TRY( smsAllocSegIterator( sIterator->mCommon.mStatement,
                                  sSegHandle,
                                  &sIterator->mSegIterator,
                                  aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 주어진 Iterator를 다시 초기화한다
 * @param[in,out] aIterator         초기화할 iterator
 * @param[in]     aEnv              Storage Manager Environment
 */
stlStatus smdmpfResetIterator( void   * aIterator,
                               smlEnv * aEnv )
{
    smdmpfIterator * sIterator = (smdmpfIterator*)aIterator;
    void           * sSegHandle;

    sSegHandle = SME_GET_SEGMENT_HANDLE( sIterator->mCommon.mRelationHandle );
    
    sIterator->mRowRid.mTbsId = smsGetTbsId( sSegHandle );
    sIterator->mRowRid.mPageId = SMP_NULL_PID;
    sIterator->mRowRid.mOffset = -1;
    sIterator->mPageBufPid = SMP_NULL_PID;

    return STL_SUCCESS;
}


/**
 * @brief 주어진 Row Rid로 Iterator의 위치를 이동시킨다
 * @param[in,out] aIterator 조사할 iterator
 * @param[in] aRowRid 이동시킬 위치
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smdmpfMoveToRowRid( void   * aIterator,
                              smlRid * aRowRid,
                              smlEnv * aEnv )
{
    return STL_SUCCESS;
}

/** 
 * @brief 주어진 position number로 Iterator의 위치를 이동시킨다
 * @param[in,out] aIterator 조사할 iterator
 * @param[in] aPosNum 이동시킬 위치
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smdmpfMoveToPosNum( void   * aIterator,
                              stlInt64 aPosNum,
                              smlEnv * aEnv )
{   
    return STL_SUCCESS;
}

/**
 * @brief 주어진 undo record를 이용하여 column list의 value들을 rollback 한다
 * @param[in] aRowInfo Validation 할 row와 현재 Tx 정보
 * @param[in] aUndoRid 적용할 undo record의 위치
 * @param[out] aRecHdrBuf 이전 버전의 row header 정보를 담을 버퍼
 * @param[in,out] aColList 레코드의 value를 담고있는 리스트
 * @param[in] aBlockIdx aColList에서 사용할 block idx
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smdmpfMakePrevVersion( smdRowInfo         * aRowInfo,
                                 smlRid             * aUndoRid,
                                 stlChar            * aRecHdrBuf,
                                 knlValueBlockList  * aColList,
                                 stlInt32             aBlockIdx,
                                 smlEnv             * aEnv )
{
    stlInt16            i;
    knlValueBlockList * sCurCol;
    stlInt16            sColLen;
    stlUInt8            sColLenSize;
    stlInt16            sUpdateColCnt;
    stlInt32            sFirstColOrd;
    stlInt32            sLogColOrd;
    stlUInt8            sLogColOrdIdx;
    stlInt16            sUndoType;
    stlInt8             sUndoClass;
    stlChar           * sUndoRec;
    stlInt16            sUndoRecSize;
    stlChar           * sLogPtr;
    smpHandle           sPageHandle;
    stlInt16            sState = 0;
    stlInt32            sBufColOrd;
    stlUInt16           sSlotHdrSize;
    dtlDataValue      * sDataValue;
    stlBool             sIsZero;
    stlBool             sIsContCol;
    
    STL_TRY( smpFix( aUndoRid->mTbsId,
                     aUndoRid->mPageId,
                     &sPageHandle,
                     aEnv ) == STL_SUCCESS );
    sState = 1;

    STL_TRY( smxlGetUndoRecord( &sPageHandle,
                                aUndoRid,
                                &sUndoType,
                                &sUndoClass,
                                &sUndoRec,
                                &sUndoRecSize ) == STL_SUCCESS );

    STL_DASSERT( sUndoClass == SMG_COMPONENT_TABLE );

    switch( sUndoType )
    {   
        case SMD_UNDO_LOG_MEMORY_HEAP_UPDATE:
            {
                STL_TRY( smdmpfMakePrevHeader( sUndoRec,
                                               aRecHdrBuf,
                                               aEnv )
                         == STL_SUCCESS);
                
                if( aColList != NULL ) /* row header만 돌리는 경우가 아니면 */
                {
                    sSlotHdrSize = SMDMPF_ROW_HDR_LEN;
                    sLogPtr = sUndoRec + sSlotHdrSize;
                    STL_WRITE_INT32( &sFirstColOrd, sLogPtr );
                    sLogPtr += STL_SIZEOF(stlInt32);
                    STL_WRITE_INT16( &sUpdateColCnt, sLogPtr );
                    sLogPtr += STL_SIZEOF(stlInt16);

                    if( sUpdateColCnt > 0 )
                    {
                        sCurCol = aColList;
                        sBufColOrd = KNL_GET_BLOCK_COLUMN_ORDER(sCurCol);

                        for( i = 0; i < sUpdateColCnt; i++ )
                        {
                            STL_WRITE_INT8( &sLogColOrdIdx, sLogPtr );
                            sLogColOrd = sLogColOrdIdx + sFirstColOrd;
                            sLogPtr += STL_SIZEOF(stlInt8);

                            while( STL_TRUE )
                            {
                                if( sBufColOrd >= sLogColOrd )
                                {
                                    break;
                                }
                                else
                                {
                                    sCurCol = sCurCol->mNext;
                                    if( sCurCol == NULL )
                                    {
                                        break;
                                    }
                                    sBufColOrd = KNL_GET_BLOCK_COLUMN_ORDER(sCurCol);
                                }
                            } /* while */

                            SMP_GET_FIXED_COLUMN_LEN( sLogPtr,
                                                      &sColLenSize,
                                                      &sColLen,
                                                      sIsContCol,
                                                      sIsZero);

                            if( sBufColOrd == sLogColOrd )
                            {
                                if( KNL_GET_BLOCK_USED_CNT(sCurCol) <= aBlockIdx )
                                {
                                    /* copy할 컬럼임 */
                                    if( sIsContCol == STL_TRUE )
                                    {
                                        /**
                                         * continuous column의 경우 next piece를 읽어야 함
                                         */
                                        STL_TRY( smdmpfReadContCols( aRowInfo->mTbsId,
                                                                     sCurCol,
                                                                     aBlockIdx,
                                                                     sLogPtr,
                                                                     aEnv ) == STL_SUCCESS );
                                    }
                                    else
                                    {
                                        sLogPtr += sColLenSize;
                                        sDataValue = KNL_GET_BLOCK_SEP_DATA_VALUE(sCurCol, aBlockIdx);

                                        /**
                                         * table의 column 사이즈가 변경된 경우를 고려해야 한다.
                                         */

                                        if( sIsZero == STL_FALSE )
                                        {
                                            if( sDataValue->mBufferSize >= sColLen )
                                            {
                                                stlMemcpy( sDataValue->mValue, sLogPtr, sColLen );
                                                sDataValue->mLength = sColLen;
                                                KNL_VALIDATE_DATA_VALUE( sCurCol, aBlockIdx, aEnv );
                                            }

                                            sLogPtr += sColLen;
                                            sCurCol->mColumnInRow.mValue  = sDataValue->mValue;
                                            sCurCol->mColumnInRow.mLength = sColLen;
                                        }
                                        else
                                        {
                                            DTL_NUMERIC_SET_ZERO( sDataValue->mValue,
                                                                  sDataValue->mLength );
                                            sCurCol->mColumnInRow.mValue  = sDataValue->mValue;
                                            sCurCol->mColumnInRow.mLength = 1;
                                        }
                                    }
                                }
                                else
                                {
                                    /* column in row 정보만 설정하고 copy할 컬럼이 아님 */
                                    sLogPtr += sColLenSize + sColLen;

                                    sDataValue = KNL_GET_BLOCK_SEP_DATA_VALUE(sCurCol, aBlockIdx);
                                    sCurCol->mColumnInRow.mValue  = sDataValue->mValue;
                                    sCurCol->mColumnInRow.mLength = sDataValue->mLength;
                                }

                                sCurCol = sCurCol->mNext;
                                
                                if( sCurCol != NULL )
                                {
                                    sBufColOrd = KNL_GET_BLOCK_COLUMN_ORDER(sCurCol);
                                }
                            }
                            else
                            {
                                /* copy할 컬럼이 아님 */
                                sLogPtr += sColLenSize + sColLen;
                            }

                            if( sCurCol == NULL )
                            {
                                break;
                            }
                        } /* for */
                    }
                }

                
                break;
            }
        case SMD_UNDO_LOG_MEMORY_HEAP_APPEND:
            {
                STL_DASSERT( STL_FALSE );
                
                break;
            }
        case SMD_UNDO_LOG_MEMORY_HEAP_UPDATE_LINK:
        case SMD_UNDO_LOG_MEMORY_HEAP_DELETE:
            {
                STL_TRY( smdmpfMakePrevHeader( sUndoRec,
                                               aRecHdrBuf,
                                               aEnv )
                         == STL_SUCCESS);
                break;
            }
        case SMD_UNDO_LOG_MEMORY_HEAP_MIGRATE:
            {
                STL_DASSERT( STL_FALSE );

                break;
            }
        default:
            {
                STL_THROW( RAMP_ERR_INTERNAL );
            }
    }

    sState = 0;
    STL_TRY( smpUnfix( &sPageHandle, aEnv ) == STL_SUCCESS ); 

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INTERNAL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_INTERNAL,
                      NULL,
                      KNL_ERROR_STACK( aEnv ),
                      "smdmpfMakePrevVersion()",
                      0 );
        
        KNL_SYSTEM_FATAL_DASSERT( STL_FALSE, KNL_ENV(aEnv) );
    }
    
    STL_FINISH;
    
    if( sState == 1 )
    {
        smpUnfix( &sPageHandle, aEnv );
    }

    return STL_FAILURE;
}

#if 0
/**
 * @brief 주어진 undo record를 이용하여 column list의 value들을 rollback 한다
 * @param[in] aRowInfo Validation 할 row와 현재 Tx 정보
 * @param[in] aUndoRid 적용할 undo record의 위치
 * @param[in] aRowHdr 이전 버전의 row header 정보를 담을 버퍼
 * @param[in,out] aColList 레코드의 value를 담고있는 리스트
 * @param[in,out] aLastCol Fetch aColList중 현재 row piece에 있는 마지막 컬럼
 *                MIGRATE ROW의 rollback시만 갱신됨
 * @param[in] aBlockIdx aColList에서 사용할 block idx
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smdmpfMakePrevRowPiece( smdRowInfo         * aRowInfo,
                                  smlRid             * aUndoRid,
                                  stlChar            * aRowHdr,
                                  knlValueBlockList  * aColList,
                                  knlValueBlockList ** aLastCol,
                                  stlInt32             aBlockIdx,
                                  smlEnv             * aEnv )
{
    stlInt16            i;
    knlValueBlockList * sCurCol;
    knlValueBlockList * sLastCol = NULL;
    stlInt64            sColLen;
    stlUInt8            sColLenSize;
    stlInt16            sUpdateColCnt;
    stlInt32            sFirstColOrd;
    stlInt32            sLogColOrd;
    stlUInt8            sLogColOrdIdx;
    stlInt16            sUndoType;
    stlInt8             sUndoClass;
    stlChar           * sUndoRec;
    stlInt16            sUndoRecSize;
    stlChar           * sLogPtr;
    smpHandle           sPageHandle;
    stlInt16            sState = 0;
    stlInt32            sBufColOrd;
    stlInt16            sColCnt;
    stlUInt16           sSlotHdrSize = SMDMPF_ROW_HDR_LEN;
    dtlDataValue      * sDataValue;
    stlBool             sIsZero;
    stlBool             sIsContCol;

    STL_DASSERT( aColList != NULL );
    
    STL_TRY( smpFix( aUndoRid->mTbsId,
                     aUndoRid->mPageId,
                     &sPageHandle,
                     aEnv ) == STL_SUCCESS );
    sState = 1;

    STL_TRY( smxlGetUndoRecord( &sPageHandle,
                                aUndoRid,
                                &sUndoType,
                                &sUndoClass,
                                &sUndoRec,
                                &sUndoRecSize ) == STL_SUCCESS );

    STL_DASSERT( sUndoClass == SMG_COMPONENT_TABLE );

    switch( sUndoType )
    {   
        case SMD_UNDO_LOG_MEMORY_HEAP_UPDATE:
            {
                sLogPtr = sUndoRec + sSlotHdrSize;
                STL_WRITE_INT32( &sFirstColOrd, sLogPtr );
                sLogPtr += STL_SIZEOF(stlInt32);
                STL_WRITE_INT16( &sUpdateColCnt, sLogPtr );
                sLogPtr += STL_SIZEOF(stlInt16);

                if( sUpdateColCnt > 0 )
                {
                    sCurCol = aColList;
                    sBufColOrd = KNL_GET_BLOCK_COLUMN_ORDER(sCurCol);

                    for( i = 0; i < sUpdateColCnt; i++ )
                    {
                        STL_WRITE_INT8( &sLogColOrdIdx, sLogPtr );
                        sLogColOrd = sLogColOrdIdx + sFirstColOrd;
                        sLogPtr += STL_SIZEOF(stlInt8);
                        while( STL_TRUE )
                        {
                            if( sBufColOrd >= sLogColOrd )
                            {
                                break;
                            }
                            else
                            {
                                sCurCol = sCurCol->mNext;
                                if( sCurCol == NULL )
                                {
                                    break;
                                }
                                sBufColOrd = KNL_GET_BLOCK_COLUMN_ORDER(sCurCol);
                            }
                        } /* while */

                        if( sBufColOrd == sLogColOrd )
                        {
                            if( KNL_GET_BLOCK_USED_CNT(sCurCol) <= aBlockIdx )
                            {
                                /* copy할 컬럼임 */
                                SMP_GET_FIXED_COLUMN_LEN( sLogPtr,
                                                          &sColLenSize,
                                                          &sColLen,
                                                          sIsContCol,
                                                          sIsZero);

                                if( sIsContCol == STL_TRUE )
                                {
                                    /**
                                     * continuous column의 경우 next piece를 읽어야 함
                                     */
                                    STL_TRY( smdmpfReadContCols( aRowInfo->mTbsId,
                                                                 sCurCol,
                                                                 aBlockIdx,
                                                                 sLogPtr,
                                                                 &sColLen,
                                                                 aEnv ) == STL_SUCCESS );
                                }
                                else
                                {
                                    sLogPtr += sColLenSize;
                                    
                                    sDataValue = KNL_GET_BLOCK_SEP_DATA_VALUE(sCurCol, aBlockIdx);

                                    /**
                                     * table의 column 사이즈가 변경된 경우를 고려해야 한다.
                                     */

                                    if( sIsZero == STL_FALSE )
                                    {
                                        if( sDataValue->mBufferSize >= sColLen )
                                        {
                                            stlMemcpy( sDataValue->mValue, sLogPtr, sColLen );
                                            sDataValue->mLength = sColLen;
                                            KNL_VALIDATE_DATA_VALUE( sCurCol, aBlockIdx, aEnv );
                                        }

                                        sLogPtr += sColLen;
                                        sCurCol->mColumnInRow.mValue  = sDataValue->mValue;
                                        sCurCol->mColumnInRow.mLength = sColLen;
                                    }
                                    else
                                    {
                                        DTL_NUMERIC_SET_ZERO( sDataValue->mValue,
                                                              sDataValue->mLength );
                                        sCurCol->mColumnInRow.mValue  = sDataValue->mValue;
                                        sCurCol->mColumnInRow.mLength = 1;
                                    }
                                }
                            }
                            else
                            {
                                /* column in row 정보만 설정하고 copy할 컬럼이 아님 */
                                SMP_GET_COLUMN_LEN( sLogPtr, &sColLenSize, &sColLen, sIsZero );
                                sLogPtr += sColLenSize + sColLen;
                                    
                                sDataValue = KNL_GET_BLOCK_SEP_DATA_VALUE(sCurCol, aBlockIdx);
                                sCurCol->mColumnInRow.mValue  = sDataValue->mValue;
                                sCurCol->mColumnInRow.mLength = sDataValue->mLength;
                            }

                            *aLastColOffset = sColLen;
                            sCurCol = sCurCol->mNext;
                                
                            if( sCurCol != NULL )
                            {
                                sBufColOrd = KNL_GET_BLOCK_COLUMN_ORDER(sCurCol);
                            }
                        }
                        else
                        {
                            /* copy할 컬럼이 아님 */
                            SMP_GET_COLUMN_LEN( sLogPtr, &sColLenSize, &sColLen, sIsZero );
                            sLogPtr += sColLenSize + sColLen;
                        }

                        if( sCurCol == NULL )
                        {
                            break;
                        }
                    } /* for */
                }
                break;
            }
        case SMD_UNDO_LOG_MEMORY_HEAP_APPEND:
            {
                STL_TRY( smdmpfMakePrevHeader( sUndoRec,
                                               aRecHdrBuf,
                                               aEnv )
                         == STL_SUCCESS);

                sLogPtr = sUndoRec + SMDMPF_ROW_HDR_LEN + STL_SIZEOF(stlInt16);
                STL_WRITE_INT32( &sFirstColOrd, sLogPtr );
                sLogPtr += STL_SIZEOF(stlInt32);
                STL_WRITE_INT16( &sUpdateColCnt, sLogPtr );

                if( aColList != NULL )
                {
                    sCurCol = aColList;

                    while( sCurCol != NULL )
                    {
                        for( i = 0; i < sUpdateColCnt; i++ )
                        {
                            if( (i + sFirstColOrd) == KNL_GET_BLOCK_COLUMN_ORDER(sCurCol) )
                            {
                                if( KNL_GET_BLOCK_USED_CNT(sCurCol) <= aBlockIdx )
                                {
                                    sDataValue = KNL_GET_BLOCK_SEP_DATA_VALUE(sCurCol, aBlockIdx);
                                    sDataValue->mLength = 0;
                                }
                            
                                sCurCol->mColumnInRow.mLength = 0;
                            }
                        }

                        sLastCol = sCurCol;
                        sCurCol = sCurCol->mNext;
                    }

                    *aLastCol = sLastCol;
                }
                
                break;
            }
        case SMD_UNDO_LOG_MEMORY_HEAP_UPDATE_LINK:
        case SMD_UNDO_LOG_MEMORY_HEAP_DELETE:
            {
                STL_TRY( smdmpfMakePrevHeader( sUndoRec,
                                               aRecHdrBuf,
                                               aEnv )
                         == STL_SUCCESS);
                break;
            }
        case SMD_UNDO_LOG_MEMORY_HEAP_MIGRATE:
            {
                STL_TRY( smdmpfMakePrevHeader( sUndoRec,
                                               aRecHdrBuf,
                                               aEnv )
                         == STL_SUCCESS);
                
                if( aColList != NULL ) /* row header만 돌리는 경우가 아니면 */
                {
                    sFirstColOrd = 0;
                    sSlotHdrSize = SMDMPF_ROW_HDR_LEN;
                    /* SMDMPF_GET_ROW_HDR_SIZE( sUndoRec, &sSlotHdrSize ); */
                    sLogPtr = sUndoRec + sSlotHdrSize;
 
                    sCurCol = aColList;
                    sBufColOrd = KNL_GET_BLOCK_COLUMN_ORDER(sCurCol);
                    SMDMPF_GET_COLUMN_CNT(aRecHdrBuf, &sColCnt);

                    for( i = 0; i < sColCnt; i++ )
                    {
                        while( STL_TRUE )
                        {
                            if( sBufColOrd >= i )
                            {
                                break;
                            }
                            else
                            {
                                sCurCol = sCurCol->mNext;
                                if( sCurCol == NULL )
                                {
                                    break;
                                }
                                sBufColOrd = KNL_GET_BLOCK_COLUMN_ORDER(sCurCol);
                            }
                        } /* while */

                        if( sBufColOrd == i )
                        {
                            if( KNL_GET_BLOCK_USED_CNT(sCurCol) <= aBlockIdx )
                            {
                                /* copy할 컬럼임 */
                                SMP_GET_COLUMN_LEN( sLogPtr, &sColLenSize, &sColLen, sIsZero);
                                sLogPtr += sColLenSize;

                                sDataValue = KNL_GET_BLOCK_SEP_DATA_VALUE(sCurCol, aBlockIdx);

                                /**
                                 * table의 column 사이즈가 변경된 경우를 고려해야 한다.
                                 */

                                if( sIsZero == STL_FALSE )
                                {
                                    if( sDataValue->mBufferSize >= sColLen )
                                    {
                                        stlMemcpy( sDataValue->mValue, sLogPtr, sColLen );
                                        sDataValue->mLength = sColLen;
                                        KNL_VALIDATE_DATA_VALUE( sCurCol, aBlockIdx, aEnv );
                                    }

                                    sLogPtr += sColLen;
                                    sCurCol->mColumnInRow.mValue  = sDataValue->mValue;
                                    sCurCol->mColumnInRow.mLength = sColLen;
                                }
                                else
                                {
                                    DTL_NUMERIC_SET_ZERO( sDataValue->mValue,
                                                          sDataValue->mLength );
                                    sCurCol->mColumnInRow.mValue  = sDataValue->mValue;
                                    sCurCol->mColumnInRow.mLength = 1;
                                }
                            }
                            else
                            {
                                /* column in row 정보만 설정하고 copy하지 않음 */
                                SMP_GET_COLUMN_LEN( sLogPtr, &sColLenSize, &sColLen, sIsZero);
                                sLogPtr += sColLenSize + sColLen;

                                sDataValue = KNL_GET_BLOCK_SEP_DATA_VALUE(sCurCol, aBlockIdx);
                                sCurCol->mColumnInRow.mValue  = sDataValue->mValue;
                                sCurCol->mColumnInRow.mLength = sDataValue->mLength;
                            }

                            sLastCol = sCurCol;
                            sCurCol = sCurCol->mNext;
                            if( sCurCol != NULL )
                            {
                                sBufColOrd = KNL_GET_BLOCK_COLUMN_ORDER(sCurCol);
                            }
                        }
                        else
                        {
                            /* copy할 컬럼이 아님 */
                            SMP_GET_COLUMN_LEN( sLogPtr, &sColLenSize, &sColLen, sIsZero );
                            sLogPtr += sColLenSize + sColLen;
                        }

                        *aLastColOffset = sColLen;
                        
                        if( sCurCol == NULL )
                        {
                            break;
                        }
                    } /* for */
                    /* Migrate Row의 경우 컬럼의 총 개수가 변하므로 마지막 컬럼을 갱신을 해주어야 한다 */
                    *aLastCol = sLastCol;
                }

                break;
            }
        default:
            {
                STL_THROW( RAMP_ERR_INTERNAL );
            }
    }

    sState = 0;
    STL_TRY( smpUnfix( &sPageHandle, aEnv ) == STL_SUCCESS ); 

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INTERNAL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_INTERNAL,
                      NULL,
                      KNL_ERROR_STACK( aEnv ),
                      "smdmpfMakePrevVersion()",
                      0 );
        
        KNL_SYSTEM_FATAL_DASSERT( STL_FALSE, KNL_ENV(aEnv) );
    }
    
    STL_FINISH;
    
    if( sState == 1 )
    {
        smpUnfix( &sPageHandle, aEnv );
    }

    return STL_FAILURE;
}
#endif

stlStatus smdmpfReadNthColValue( smdRowInfo         * aRowInfo,
                                 smpHandle          * aPageHandle,
                                 stlBool              aNeedPrevVersion,
                                 stlChar            * aUndoLogPtr,
                                 stlInt16             aSlotSeq,
                                 stlInt16             aUndoType,
                                 stlInt32             aFirstColOrd,
                                 knlValueBlockList ** aCurCol,
                                 knlValueBlockList ** aLastCol,
                                 stlInt32             aBlockIdx,
                                 smlEnv             * aEnv )
{
    stlInt32             sColOrder;
    knlColumnInReadRow * sCurInReadRow;
    dtlDataValue       * sDataValue;
    stlUInt8             sLogColOrdIdx;
    stlUInt8             sColLenSize;
    stlInt16             sColLen;
    stlBool              sIsContCol;
    stlBool              sIsZero;
    stlChar            * sColPtr;
    stlBool              sUseUndoLog = STL_FALSE;

    sColOrder  = KNL_GET_BLOCK_COLUMN_ORDER( *aCurCol );
    sDataValue = KNL_GET_BLOCK_SEP_DATA_VALUE( *aCurCol, aBlockIdx );

    if( aNeedPrevVersion == STL_TRUE )
    {
        STL_WRITE_INT8( &sLogColOrdIdx, aUndoLogPtr );

        if( (aFirstColOrd + sLogColOrdIdx) == sColOrder )
        {
            aUndoLogPtr += STL_SIZEOF(stlInt8);
            sColPtr = aUndoLogPtr;
            sUseUndoLog = STL_TRUE;
        }
    }

    if( sUseUndoLog == STL_FALSE )
    {
        sColPtr = smdmpfGetNthColValue( aPageHandle,
                                        aSlotSeq,
                                        sColOrder );

        STL_TRY_THROW( sColPtr != NULL, RAMP_READ_DONE );
    }

    sCurInReadRow = KNL_GET_BLOCK_COLUMN_IN_ROW( *aCurCol );

    SMP_GET_FIXED_COLUMN_LEN( sColPtr,
                              &sColLenSize,
                              &sColLen,
                              sIsContCol,
                              sIsZero);

    if( sIsContCol == STL_TRUE )
    {
        /**
         * continuous column의 경우 next piece를 읽어야 함
         */
        STL_TRY( smdmpfReadContCols( aRowInfo->mTbsId,
                                     *aCurCol,
                                     aBlockIdx,
                                     sColPtr,
                                     aEnv ) == STL_SUCCESS );

        SMP_GET_CONT_COLUMN_LEN_SIZE( &sColLenSize );

        sColPtr += (sColLen + sColLenSize);

        sCurInReadRow->mValue  = sDataValue->mValue;
        sCurInReadRow->mLength = sDataValue->mLength;
    }
    else
    {
        sColPtr += sColLenSize;

        if( sIsZero == STL_FALSE )
        {
            if( sDataValue->mBufferSize >= sColLen )
            {
                stlMemcpy( sDataValue->mValue, sColPtr, sColLen );
                sDataValue->mLength = sColLen;
                KNL_VALIDATE_DATA_VALUE( *aCurCol, aBlockIdx, aEnv );
            }

            sColPtr += sColLen;
            (*aCurCol)->mColumnInRow.mValue  = sDataValue->mValue;
            (*aCurCol)->mColumnInRow.mLength = sColLen;

            sCurInReadRow->mValue  = sDataValue->mValue;
            sCurInReadRow->mLength = sDataValue->mLength;
        }
        else
        {
            DTL_NUMERIC_SET_ZERO( sDataValue->mValue,
                                  sDataValue->mLength );
            (*aCurCol)->mColumnInRow.mValue  = sDataValue->mValue;
            (*aCurCol)->mColumnInRow.mLength = 1;

            sCurInReadRow->mValue  = sDataValue->mValue;
            sCurInReadRow->mLength = 1;
        }
    }

    if( sUseUndoLog == STL_TRUE )
    {
        aUndoLogPtr += (sColLen + sColLenSize);
    }

    *aLastCol = (*aCurCol);
    (*aCurCol) = (*aCurCol)->mNext;

    STL_RAMP( RAMP_READ_DONE )

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Data page에 저장된 최신 버전의 column value를 읽는다.
 * @param[in] aPageHandle 최신 버전의 data value를 읽은 page handle
 * @param[in] aRowInfo Validation 할 row와 현재 Tx 정보
 * @param[in] aSlotSeq Data column value가 저장된 page내 slot sequence
 * @param[in] aColCount aPageHandle에 기록된 column의 수
 * @param[in,out] aStartCol Data column을 담고 있는 리스트
 * @param[in] aBlockIdx aStartCol에서 사용할 block idx
 * @param[in,out] aLastCol aPageHandle에서 읽은 마지막 column
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smdmpfReadRecentColValues( smpHandle          * aPageHandle,
                                     smdRowInfo         * aRowInfo,
                                     stlInt16             aSlotSeq,
                                     stlInt16             aColCount,
                                     knlValueBlockList  * aStartCol,
                                     stlInt32             aBlockIdx,
                                     knlValueBlockList ** aLastCol,
                                     smlEnv             * aEnv )
{
    stlInt16            i;
    stlChar           * sColPtr;
    stlInt16            sColLen;
    stlUInt8            sColLenSize;
    knlValueBlockList * sCurCol = aStartCol;
    knlValueBlockList * sLastCol = NULL;
    dtlDataValue      * sDataValue;
    stlBool             sIsZero;
    stlBool             sIsContCol;
    stlInt32            sColOrder;

    for( i = 0; i < aColCount; i++ )
    {
        if( sCurCol == NULL )
        {
            break;
        }

        sColOrder = KNL_GET_BLOCK_COLUMN_ORDER(sCurCol);

        sColPtr = smdmpfGetNthColValue( aPageHandle,
                                        aSlotSeq,
                                        sColOrder );

        if( sColPtr == NULL )
        {
            break;
        }

        sDataValue = KNL_GET_BLOCK_SEP_DATA_VALUE(sCurCol, aBlockIdx);

        if( KNL_GET_BLOCK_USED_CNT(sCurCol) <= aBlockIdx )
        {
            /* Page에 저장된 column의 continous column여부까지 확인하여 length를 분석한다. */
            SMP_GET_FIXED_COLUMN_LEN( sColPtr,
                                      &sColLenSize,
                                      &sColLen,
                                      sIsContCol,
                                      sIsZero );

            sDataValue = KNL_GET_BLOCK_SEP_DATA_VALUE(sCurCol, aBlockIdx);

            if( sIsContCol == STL_TRUE )
            {
                /**
                 * continuous column의 경우 next piece를 읽어야 함
                 */
                STL_TRY( smdmpfReadContCols( aRowInfo->mTbsId,
                                             sCurCol,
                                             aBlockIdx,
                                             sColPtr,
                                             aEnv ) == STL_SUCCESS );

                sCurCol->mColumnInRow.mValue  = sDataValue->mValue;
                sCurCol->mColumnInRow.mLength = sDataValue->mLength;
            }
            else
            {
                if( sIsZero == STL_FALSE )
                {
                    if( sDataValue->mBufferSize >= sColLen )
                    {
                        stlMemcpy( sDataValue->mValue, sColPtr + sColLenSize, sColLen );
                        sDataValue->mLength = sColLen;
                    }

                    sCurCol->mColumnInRow.mValue  = sDataValue->mValue;
                    sCurCol->mColumnInRow.mLength = sDataValue->mLength;
                }
                else
                {
                    DTL_NUMERIC_SET_ZERO( sDataValue->mValue,
                                          sDataValue->mLength );

                    sCurCol->mColumnInRow.mValue  = sDataValue->mValue;
                    sCurCol->mColumnInRow.mLength = 1;
                }
            }
        }
        else
        {
            sCurCol->mColumnInRow.mValue  = sDataValue->mValue;
            sCurCol->mColumnInRow.mLength = sDataValue->mLength;
        }
            
        sLastCol = sCurCol;
        sCurCol = sCurCol->mNext;
    }

    *aLastCol = sLastCol;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 주어진 Row에서 읽을 수 있는 버전을 구한다
 * @param[in] aRelationHandle  Relation Handle
 * @param[in] aRowRid  현재 레코드의 smlRid
 * @param[in] aRowInfo Validation 할 row와 현재 Tx 정보
 * @param[in] aPhysicalFilter Physical Filter
 * @param[in] aLogicalFilterEvalInfo Logical Filter Evaluation Info
 * @param[out] aCommitScn 반환되는 버전의 commit SCN
 * @param[out] aRowTcn 반환되는 버전의 TCN
 * @param[out] aWaitTransId 반환되는 버전이 commit되지 않아서 기다려야 되는 transaction id
 * @param[out] aHasValidVersion 읽을 수 있는 버전이 존재하는지 여부
 * @param[out] aIsMatched Physical Filter 조건에 만족하는 버전이 존재하는지 여부
 * @param[out] aLatchReleased aPageHandle의 Latch 해제 여부
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smdmpfGetValidVersion( void                * aRelationHandle,
                                 smlRid              * aRowRid,
                                 smdRowInfo          * aRowInfo,
                                 knlPhysicalFilter   * aPhysicalFilter, 
                                 knlExprEvalInfo     * aLogicalFilterEvalInfo,
                                 smlScn              * aCommitScn,
                                 smlTcn              * aRowTcn,
                                 smxlTransId         * aWaitTransId,
                                 stlBool             * aHasValidVersion,
                                 stlBool             * aIsMatched,
                                 stlBool             * aLatchReleased,
                                 smlEnv              * aEnv )
{
    knlValueBlockList * sCurCol;
    knlValueBlockList * sLastCol= NULL;
    stlChar           * sColPtr;
    smlRid              sRollbackRid;
    smlRid              sNextRid;
    smlTcn              sTcn;
    stlInt16            sColCount = 0;
    stlInt16            sCurColCount = 0;
    stlChar             sRowHdrBuf[SMDMPF_ROW_HDR_LEN];
    stlBool             sAlreadyCopyCols = STL_FALSE;
    stlInt32            sStartColOrd = 0;
    smpHandle           sPageHandle = *aRowInfo->mPageHandle;
    stlInt32            sState = 0;
    stlBool             sIsMyTrans = STL_FALSE;
    smlScn              sCommitScn;
    stlBool             sIsFirstPiece = STL_TRUE;
    stlChar           * sRowHdr = aRowInfo->mRow;
    knlValueBlockList * sColList = aRowInfo->mColList;
    stlInt32            sBlockIdx = aRowInfo->mBlockIdx;
    stlUInt8            sColLenSize = 0;
    stlInt64            sColLen = 0;
    knlPhysicalFilter * sPhysicalFilter;
    knlColumnInReadRow  * sCurInReadRow;
    knlValueBlockList * sTempCurCol;
    dtlDataValue      * sDataValue;
    stlInt32            sLastColOrder = 0;
    stlBool             sIsZero;
    stlBool             sIsContCol;
    stlInt32            sColOrder;
    smlPid              sChainedPageId;

    STL_ASSERT( SMDMPF_IS_MASTER_ROW( sRowHdr ) == STL_TRUE );

    *aRowTcn = 0;
    *aLatchReleased = STL_FALSE;
    *aHasValidVersion = STL_FALSE;

    sCurCol = sColList;
    *aIsMatched = STL_FALSE;
    sPhysicalFilter = aPhysicalFilter;
    sChainedPageId = SMDMPF_GET_NEXT_CHAINED_PAGE_ID(SMP_FRAME(&sPageHandle));
    
    while( STL_TRUE )  /* 모든 row piece들에 대해 */
    {
        sIsFirstPiece = SMDMPF_IS_MASTER_ROW( sRowHdr );
        sAlreadyCopyCols = STL_FALSE;
        sColCount = 0;
        *aHasValidVersion = STL_FALSE;

        while( STL_TRUE )  /* 한 row piece의 모든 버전들에 대해 */
        {
            STL_TRY( smdmpfGetCommitScn( aRowInfo->mTbsId,
                                         &sPageHandle,
                                         sRowHdr,
                                         &sCommitScn,
                                         aWaitTransId,
                                         aEnv ) == STL_SUCCESS );

            if( sIsFirstPiece == STL_TRUE )
            {
                *aCommitScn = sCommitScn;
            }

            SMDMPF_GET_COLUMN_CNT( sRowHdr, &sCurColCount );
            
            /**
             * DDL( ex. add column ) 이전의 view을 봐야 하는 경우라면
             * trailing null도 읽은 column으로 설정해야 한다.
             */
            sColCount = STL_MAX( sColCount, sCurColCount );

            /* 1. 내 tx인지 체크 */
            if( SMDMPF_IS_SAME_TRANS( aRowInfo->mTransId,
                                      aRowInfo->mCommitScn,
                                      *aWaitTransId,
                                      sCommitScn ) == STL_TRUE )
            {
                sIsMyTrans = STL_TRUE;
                SMDMPF_GET_TCN( sRowHdr, &sTcn );
                
                if( sIsFirstPiece == STL_TRUE )
                {
                    *aRowTcn = sTcn;
                }
                
                if( (aRowInfo->mTcn == SML_INFINITE_TCN) || (sTcn < aRowInfo->mTcn) )
                {
                    /* 내 Tx의 읽을 수 있는 버전을 찾아냄 */
                    *aHasValidVersion = STL_TRUE;
                    break;
                }
            }
            else
            {
                sIsMyTrans = STL_FALSE;
                /* 2. 내 tx가 아닌 경우 */
                /* SML_TRM_COMMITTED인 경우 aMyViewScn이 infinite라서 커밋된 모든 레코드를 자동으로 보게 됨 */
                if( (sCommitScn <= aRowInfo->mViewScn) &&
                    (sCommitScn != SML_INFINITE_SCN) )
                {
                    *aHasValidVersion = ( !SMDMPF_IS_DELETED( sRowHdr ) );
                    break;
                }
            }

            sRollbackRid.mTbsId = SML_MEMORY_UNDO_SYSTEM_TBS_ID;
            SMDMPF_GET_ROLLBACK_PID( sRowHdr, &sRollbackRid.mPageId );
            SMDMPF_GET_ROLLBACK_OFFSET( sRowHdr, &sRollbackRid.mOffset );

            /* 읽을 수 있는 버전이 아님. 이전 버전으로 되돌림 */
            if( sRollbackRid.mPageId == SMP_NULL_PID )
            {
                *aHasValidVersion = STL_FALSE;
                break;
            }

            if( (sAlreadyCopyCols == STL_FALSE) && (sCurCol != NULL) )
            {
                /* 이전버전을 읽으면 값이 바뀌게 되므로
                 * 여기서는 physical filter를 처리하지 않는다.
                 * 현재 row piece에 읽을 컬럼이 없다면 read하지 않는다. */
                if( (sStartColOrd <= KNL_GET_BLOCK_COLUMN_ORDER(sCurCol)) &&
                    (sStartColOrd + sColCount > KNL_GET_BLOCK_COLUMN_ORDER(sCurCol)) )
                {
                    STL_TRY( smdmpfReadRecentColValues( &sPageHandle,
                                                        aRowInfo,
                                                        aRowRid->mOffset,
                                                        sColCount,
                                                        sCurCol,
                                                        sBlockIdx,
                                                        &sLastCol,
                                                        aEnv )
                             == STL_SUCCESS );
                }
                sAlreadyCopyCols = STL_TRUE;
            }

            if( sRowHdr != sRowHdrBuf )
            {
                SMDMPF_COPY_ROW_HDR( sRowHdrBuf, sRowHdr );
            }

            /* 해당 row piece의 이전 버전을 구해온다 */
            STL_TRY( smdmpfMakePrevVersion( aRowInfo,
                                            &sRollbackRid,
                                            sRowHdrBuf,
                                            sCurCol,
                                            sBlockIdx,
                                            aEnv ) == STL_SUCCESS );

            sRowHdr = sRowHdrBuf;
        } /* while */

        sIsFirstPiece = STL_FALSE;
        
        if( (SMDMPF_IS_DELETED( sRowHdr ) == STL_TRUE) &&
            ((aRowInfo->mViewScn != SML_INFINITE_SCN) || (sIsMyTrans == STL_TRUE)) &&
            (*aHasValidVersion == STL_TRUE) )
        {
            *aHasValidVersion = STL_FALSE;
        }

        if( *aHasValidVersion != STL_TRUE )
        {
            /* valid version을 못찾음 */
            break;
        }

        STL_DASSERT( sColCount > 0 );
            
        if( sCurCol == NULL )
        {
            /**
             * 더 이상 Copy할 Column이 없다면,
             * 평가되지 않은 Physical Filter도 없어야 한다.
             */
            STL_DASSERT( sPhysicalFilter == NULL );
            *aIsMatched = STL_TRUE;
            break;
        }

        if( sAlreadyCopyCols == STL_FALSE )
        {
            /* 현재 row piece에 평가할 컬럼이 없다면 다음 row piece로 */
            if( (sStartColOrd > KNL_GET_BLOCK_COLUMN_ORDER(sCurCol)) ||
                (sStartColOrd + sColCount <= KNL_GET_BLOCK_COLUMN_ORDER(sCurCol)) )
            {
                sStartColOrd += sColCount;
                STL_THROW( RAMP_READ_NEXT_ROW_PIECE );
            }

            sLastColOrder = sStartColOrd + sColCount - 1;
            sTempCurCol   = sCurCol;
            sLastCol      = sCurCol;

            while( sTempCurCol != NULL )
            {
                sColOrder = KNL_GET_BLOCK_COLUMN_ORDER(sTempCurCol);

                if( sColOrder > sLastColOrder )
                {
                    break;
                }

                sColPtr = smdmpfGetNthColValue( &sPageHandle,
                                                aRowRid->mOffset,
                                                sColOrder );

                STL_DASSERT( sColPtr != NULL );

                SMP_GET_FIXED_COLUMN_LEN( sColPtr,
                                          &sColLenSize,
                                          &sColLen,
                                          sIsContCol,
                                          sIsZero );

                sDataValue = KNL_GET_BLOCK_SEP_DATA_VALUE(sTempCurCol, sBlockIdx);

                if( sIsContCol == STL_TRUE )
                {
                    /**
                     * continuous column의 경우 next piece를 읽어야 함
                     */
                    STL_TRY( smdmpfReadContCols( aRowInfo->mTbsId,
                                                 sTempCurCol,
                                                 sBlockIdx,
                                                 sColPtr,
                                                 aEnv ) == STL_SUCCESS );

                    sTempCurCol->mColumnInRow.mValue  = sDataValue->mValue;
                    sTempCurCol->mColumnInRow.mLength = sDataValue->mLength;
                }
                else
                {
                    if( sIsZero == STL_FALSE )
                    {
                        if( sDataValue->mBufferSize >= sColLen )
                        {
                            stlMemcpy( sDataValue->mValue, sColPtr + sColLenSize, sColLen );
                            sDataValue->mLength = sColLen;
                        }

                        sTempCurCol->mColumnInRow.mValue  = sDataValue->mValue;
                        sTempCurCol->mColumnInRow.mLength = sDataValue->mLength;
                    }
                    else
                    {
                        DTL_NUMERIC_SET_ZERO( sDataValue->mValue,
                                              sDataValue->mLength );

                        sTempCurCol->mColumnInRow.mValue  = sDataValue->mValue;
                        sTempCurCol->mColumnInRow.mLength = 1;
                    }
                }

                sTempCurCol = sTempCurCol->mNext;
                sLastCol    = sTempCurCol;

                while( sPhysicalFilter != NULL )
                {
                    if( sPhysicalFilter->mColIdx1 <= sColOrder )
                    {
                        /* 위 sCurInReadRow의 셋팅에서 이미 physical filter가
                         * 가리키는 column value의 위치를 저장했다. */
                        if( sPhysicalFilter->mFunc( sPhysicalFilter->mColVal1->mValue,
                                                    sPhysicalFilter->mColVal1->mLength,
                                                    sPhysicalFilter->mColVal2->mValue,
                                                    sPhysicalFilter->mColVal2->mLength )
                            == STL_FALSE )
                        {
                            STL_THROW( RAMP_FINISH );
                        }
                    }
                    else
                    {
                        break;
                    }

                    sPhysicalFilter = sPhysicalFilter->mNext;
                }
            }

            /* 현재 row 에 포함된 column들을 value block 에 복사 */
            while( sCurCol != sLastCol )
            {
                sCurInReadRow = KNL_GET_BLOCK_COLUMN_IN_ROW( sCurCol );
                sDataValue    = KNL_GET_BLOCK_SEP_DATA_VALUE( sCurCol, sBlockIdx );

                DTL_COPY_VALUE( sDataValue,
                                sCurInReadRow->mValue,
                                sCurInReadRow->mLength );
                sDataValue->mLength = sCurInReadRow->mLength;

                sCurCol = sCurCol->mNext;
            }
        }
        else
        {
            sLastColOrder = sStartColOrd + sColCount - 1;

            if( sLastCol == NULL )
            {
                /* Do Nothing */
            }
            else
            {
                while( sPhysicalFilter != NULL )
                {
                    if( sPhysicalFilter->mColIdx1 <= sLastColOrder )
                    {
                        /* 위 sCurInReadRow의 셋팅에서 이미 physical filter가
                         * 가리키는 column value의 위치를 저장했다. */
                        if( sPhysicalFilter->mFunc( sPhysicalFilter->mColVal1->mValue,
                                                    sPhysicalFilter->mColVal1->mLength,
                                                    sPhysicalFilter->mColVal2->mValue,
                                                    sPhysicalFilter->mColVal2->mLength )
                            == STL_FALSE )
                        {
                            STL_THROW( RAMP_FINISH );
                        }
                    }
                    else
                    {
                        break;
                    }

                    sPhysicalFilter = sPhysicalFilter->mNext;
                }
                    
                /**
                 * DDL( ex. add column ) 이전의 view을 봐야 하는 경우라면
                 * Last Column을 증가시키지 않는다.
                 */
                while( sLastColOrder >= KNL_GET_BLOCK_COLUMN_ORDER(sLastCol) )
                {
                    sLastCol = sLastCol->mNext;
                    sCurCol  = sLastCol;

                    if( sCurCol == NULL )
                    {
                        break;
                    }
                }
            }
        }

        sStartColOrd = sLastColOrder + 1;
        
        STL_RAMP( RAMP_READ_NEXT_ROW_PIECE );

        /**
         * 다음 Row를 읽을지 여부 판단
         */
        if( sCurCol == NULL )
        {
            /* 더 이상 row piece를 fetch할 필요가 없다 */
            while( sPhysicalFilter != NULL )
            {
                if( sPhysicalFilter->mFunc( sPhysicalFilter->mColVal1->mValue,
                                            sPhysicalFilter->mColVal1->mLength,
                                            sPhysicalFilter->mColVal2->mValue,
                                            sPhysicalFilter->mColVal2->mLength )
                    == STL_FALSE )
                {
                    STL_THROW( RAMP_FINISH );
                }
                else
                {       
                    sPhysicalFilter = sPhysicalFilter->mNext;
                }
            }

            *aIsMatched = STL_TRUE;
            break;
        }
        else
        {
            if( sChainedPageId != SMP_NULL_PID )
            {
                /* next member page가 있으면 */
                sNextRid.mPageId = sChainedPageId;
                sNextRid.mOffset = aRowRid->mOffset;
            }
            else
            {
                /**
                 * Trailing null
                 */

                while( sCurCol != NULL )
                {
                    /**
                     * if i2 and i3 are trailing null
                     * i3의 filter 검사시 i2의 값이 사용될수 있기 때문에
                     * i2의 길이를 설정한다.
                     * 
                     * 예) select ... where i3 = i2;
                     */
                    
                    while( sPhysicalFilter != NULL )
                    {
                        sPhysicalFilter->mColVal1->mLength = 0;
                    
                        if( sPhysicalFilter->mColIdx1 <= KNL_GET_BLOCK_COLUMN_ORDER(sCurCol) )
                        {
                            if( sPhysicalFilter->mFunc( sPhysicalFilter->mColVal1->mValue,
                                                        sPhysicalFilter->mColVal1->mLength,
                                                        sPhysicalFilter->mColVal2->mValue,
                                                        sPhysicalFilter->mColVal2->mLength )
                                == STL_FALSE )
                            {
                                STL_THROW( RAMP_FINISH );
                            }
                        }
                        else
                        {
                            break;
                        }
                        
                        sPhysicalFilter = sPhysicalFilter->mNext;
                    }
                    
                    sDataValue = KNL_GET_BLOCK_SEP_DATA_VALUE( sCurCol, sBlockIdx );
                    sDataValue->mLength = 0;
                    sCurCol = sCurCol->mNext;
                }
                
                *aIsMatched = STL_TRUE;
                break;
            }
        }
            
        /* next piece를 읽을 준비를 함 */
        if( SMP_IS_DUMMY_HANDLE( &sPageHandle ) != STL_TRUE )
        {
            sState = 0;
            STL_TRY( smpRelease( &sPageHandle, aEnv ) == STL_SUCCESS );
            *aLatchReleased = STL_TRUE;
        }
        
        STL_TRY( smpAcquire( NULL,
                             aRowInfo->mTbsId,
                             sNextRid.mPageId,
                             KNL_LATCH_MODE_SHARED,
                             &sPageHandle,
                             aEnv ) == STL_SUCCESS );
        
        sState = 1;

        sChainedPageId = SMDMPF_GET_NEXT_CHAINED_PAGE_ID(SMP_FRAME(&sPageHandle));
        sRowHdr = SMDMPF_GET_NTH_ROW_HDR( SMP_FRAME(&sPageHandle), sNextRid.mOffset );
    } /* while */

    if( ( *aHasValidVersion == STL_TRUE ) &&
        ( *aIsMatched == STL_TRUE ) )
    {
        /**
         * RowId Column을 참조하는 경우, RowId Column Block에 값을 만든다. 
         */
        if( aRowInfo->mRowIdColList != NULL )
        {
            STL_TRY( dtlRowIdSetValueFromRowIdInfo(
                         aRowInfo->mTableLogicalId,
                         aRowRid->mTbsId,
                         aRowRid->mPageId,
                         aRowRid->mOffset,
                         KNL_GET_BLOCK_SEP_DATA_VALUE( aRowInfo->mRowIdColList,
                                                       sBlockIdx ),
                         KNL_ERROR_STACK(aEnv) )
                     == STL_SUCCESS );            
        }
        
        /**
         * Logical Filter 평가
         */
        if( aLogicalFilterEvalInfo != NULL )
        {
            aLogicalFilterEvalInfo->mBlockIdx = sBlockIdx;

            STL_TRY( knlEvaluateOneExpression( aLogicalFilterEvalInfo,
                                               KNL_ENV(aEnv) )
                     == STL_SUCCESS );
            
            STL_DASSERT( KNL_GET_BLOCK_IS_SEP_BUFF(
                             aLogicalFilterEvalInfo->mResultBlock )
                         == STL_TRUE );
            
            if( !DTL_BOOLEAN_IS_TRUE( KNL_GET_BLOCK_SEP_DATA_VALUE(
                                          aLogicalFilterEvalInfo->mResultBlock,
                                          sBlockIdx ) ) )
            {
                *aIsMatched = STL_FALSE;
                STL_THROW( RAMP_FINISH );
            }
        }    
    }
    
    STL_RAMP( RAMP_FINISH );
    
    if( sState == 1 )
    {
        sState = 0;
        STL_TRY( smpRelease( &sPageHandle, aEnv ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smpRelease( &sPageHandle, aEnv );
    }

    return STL_FAILURE;
}


/**
 * @brief 주어진 Row에서 읽을 수 있는 버전을 구한다
 * @param[in] aRelationHandle  Relation Handle
 * @param[in] aRowRid  현재 레코드의 smlRid
 * @param[in] aRowInfo Validation 할 row와 현재 Tx 정보
 * @param[in] aPhysicalFilter Physical Filter
 * @param[in] aLogicalFilterEvalInfo Logical Filter Evaluation Info
 * @param[out] aCommitScn 반환되는 버전의 commit SCN
 * @param[out] aWaitTransId 반환되는 버전이 commit되지 않아서 기다려야 되는 transaction id
 * @param[out] aHasValidVersion 읽을 수 있는 버전이 존재하는지 여부
 * @param[out] aIsMatched Physical Filter 조건에 만족하는 버전이 존재하는지 여부
 * @param[out] aLatchReleased aPageHandle의 Latch 해제 여부
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smdmpfGetValidVersionSimplePage( void                * aRelationHandle,
                                           smlRid              * aRowRid,
                                           smdRowInfo          * aRowInfo,
                                           knlPhysicalFilter   * aPhysicalFilter, 
                                           knlExprEvalInfo     * aLogicalFilterEvalInfo,
                                           smlScn              * aCommitScn,
                                           smxlTransId         * aWaitTransId,
                                           stlBool             * aHasValidVersion,
                                           stlBool             * aIsMatched,
                                           stlBool             * aLatchReleased,
                                           smlEnv              * aEnv )
{
    knlValueBlockList  * sCurCol;
    knlValueBlockList  * sLastCol= NULL;
    stlInt32             sBlockIdx = aRowInfo->mBlockIdx;
    stlUInt8             sColLenSize = 0;
    stlInt64             sColLen = 0;
    knlPhysicalFilter  * sPhysicalFilter;
    knlColumnInReadRow * sCurInReadRow;
    knlValueBlockList  * sTempCurCol;
    dtlDataValue       * sDataValue;
    stlBool              sIsZero;
    stlInt32             sColOrder;
    stlChar            * sColPtr;

    STL_DASSERT( SMDMPF_IS_MASTER_ROW( aRowInfo->mRow ) == STL_TRUE );
    STL_DASSERT( SMDMPF_IS_DELETED( aRowInfo->mRow ) == STL_FALSE );

    *aLatchReleased = STL_FALSE;
    *aIsMatched = STL_FALSE;
    *aHasValidVersion = STL_TRUE;

    sCurCol = aRowInfo->mColList;
    sPhysicalFilter = aPhysicalFilter;
        
    /**
     * Physical Filter 평가
     */

    sTempCurCol = sCurCol;
    sLastCol    = sCurCol;

    if( sTempCurCol != NULL )
    {
        if( sPhysicalFilter != NULL )
        {
            while( sTempCurCol != NULL )
            {
                sColOrder = KNL_GET_BLOCK_COLUMN_ORDER(sTempCurCol);

                sColPtr = smdmpfGetNthColValue( aRowInfo->mPageHandle,
                                                aRowRid->mOffset,
                                                sColOrder );

                STL_DASSERT( sColPtr != NULL );

                SMP_GET_COLUMN_LEN( sColPtr,
                                    &sColLenSize,
                                    &sColLen,
                                    sIsZero );

                sCurInReadRow = KNL_GET_BLOCK_COLUMN_IN_ROW( sTempCurCol );
                        
                if( sIsZero == STL_FALSE )
                {
                    sCurInReadRow->mValue  = sColPtr + sColLenSize;
                    sCurInReadRow->mLength = sColLen;
                }
                else
                {
                    sDataValue = KNL_GET_BLOCK_SEP_DATA_VALUE( sTempCurCol, sBlockIdx );
                            
                    DTL_NUMERIC_SET_ZERO( sDataValue->mValue,
                                          sDataValue->mLength );
                            
                    sCurInReadRow->mValue  = sDataValue->mValue;
                    sCurInReadRow->mLength = 1;
                }

                sTempCurCol = sTempCurCol->mNext;
                sLastCol    = sTempCurCol;
                            
                /* 현재 column에 해당하는 filter 체크
                 * (filter 결과가 false이면 invalid 체크후 break */
                while( sPhysicalFilter != NULL )
                {
                    if( sPhysicalFilter->mColIdx1 <= sColOrder )
                    {
                        /* 위 sCurInReadRow의 셋팅에서 이미 physical filter가
                         * 가리키는 column value의 위치를 저장했다. */
                        if( sPhysicalFilter->mFunc( sPhysicalFilter->mColVal1->mValue,
                                                    sPhysicalFilter->mColVal1->mLength,
                                                    sPhysicalFilter->mColVal2->mValue,
                                                    sPhysicalFilter->mColVal2->mLength )
                            == STL_FALSE )
                        {
                            STL_THROW( RAMP_FINISH );
                        }
                    }
                    else
                    {
                        break;
                    }
                    sPhysicalFilter = sPhysicalFilter->mNext;
                }
            }

            /* 현재 row 에 포함된 column들을 value block 에 복사 */
            while( sCurCol != sLastCol )
            {
                sCurInReadRow = KNL_GET_BLOCK_COLUMN_IN_ROW( sCurCol );
                sDataValue    = KNL_GET_BLOCK_SEP_DATA_VALUE( sCurCol, sBlockIdx );
                
                sDataValue->mLength = sCurInReadRow->mLength;

                DTL_COPY_VALUE( sDataValue,
                                sCurInReadRow->mValue,
                                sCurInReadRow->mLength );

                sCurCol = sCurCol->mNext;
            }
        }
        else
        {
            while( sCurCol != NULL )
            {
                sColOrder = KNL_GET_BLOCK_COLUMN_ORDER(sCurCol);

                sColPtr = smdmpfGetNthColValue( aRowInfo->mPageHandle,
                                                aRowRid->mOffset,
                                                sColOrder );

                STL_DASSERT( sColPtr != NULL );

                SMP_GET_COLUMN_LEN( sColPtr,
                                    &sColLenSize,
                                    &sColLen,
                                    sIsZero );

                sColPtr += sColLenSize;
                sDataValue = KNL_GET_BLOCK_SEP_DATA_VALUE( sCurCol, sBlockIdx );
                        
                if( sIsZero == STL_TRUE )
                {
                    DTL_NUMERIC_SET_ZERO( sDataValue->mValue, sDataValue->mLength );
                }
                else
                {
                    sDataValue->mLength = sColLen;
                    DTL_COPY_VALUE( sDataValue, sColPtr, sColLen );
                }
                    
                sCurCol = sCurCol->mNext;
            }
        }
    }

    /**
     * Trailing null
     */
        
    while( sCurCol != NULL )
    {
        while( sPhysicalFilter != NULL )
        {
            sPhysicalFilter->mColVal1->mLength = 0;
                            
            if( sPhysicalFilter->mColIdx1 <= KNL_GET_BLOCK_COLUMN_ORDER(sCurCol) )
            {
                if( sPhysicalFilter->mFunc( sPhysicalFilter->mColVal1->mValue,
                                            sPhysicalFilter->mColVal1->mLength,
                                            sPhysicalFilter->mColVal2->mValue,
                                            sPhysicalFilter->mColVal2->mLength )
                    == STL_FALSE )
                {
                    STL_THROW( RAMP_FINISH );
                }
            }
            else
            {
                break;
            }

            sPhysicalFilter = sPhysicalFilter->mNext;
        }
                        
        sDataValue = KNL_GET_BLOCK_SEP_DATA_VALUE( sCurCol, sBlockIdx );
        sDataValue->mLength = 0;
        sCurCol = sCurCol->mNext;
    }
                    
    /**
     * RowId Column을 참조하는 경우, RowId Column Block에 값을 만든다. 
     */
    if( aRowInfo->mRowIdColList != NULL )
    {
        STL_TRY( dtlRowIdSetValueFromRowIdInfo(
                     aRowInfo->mTableLogicalId,
                     aRowRid->mTbsId,
                     aRowRid->mPageId,
                     aRowRid->mOffset,
                     KNL_GET_BLOCK_SEP_DATA_VALUE( aRowInfo->mRowIdColList,
                                                   sBlockIdx ),
                     KNL_ERROR_STACK(aEnv) )
                 == STL_SUCCESS );            
    }

    /**
     * Logical Filter 평가
     */
    if( aLogicalFilterEvalInfo != NULL )
    {
        aLogicalFilterEvalInfo->mBlockIdx = sBlockIdx;

        STL_TRY( knlEvaluateOneExpression( aLogicalFilterEvalInfo,
                                           KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        STL_DASSERT( KNL_GET_BLOCK_IS_SEP_BUFF(
                         aLogicalFilterEvalInfo->mResultBlock )
                     == STL_TRUE );

        if( !DTL_BOOLEAN_IS_TRUE( KNL_GET_BLOCK_SEP_DATA_VALUE(
                                      aLogicalFilterEvalInfo->mResultBlock,
                                      sBlockIdx ) ) )
        {
            STL_THROW( RAMP_FINISH );
        }
    }

    *aIsMatched = STL_TRUE;
    
    STL_RAMP( RAMP_FINISH );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 주어진 Row에서 읽을 수 있는 버전을 구한다
 * @param[in] aRowInfo Validation 할 row와 현재 Tx 정보
 * @param[in] aSlotSeq Row Slot의 순번
 * @param[out] aCommitScn 반환되는 버전의 commit SCN
 * @param[out] aWaitTransId 반환되는 버전이 commit되지 않아서 기다려야 되는 transaction id
 * @param[out] aIsMatch 주어진 Key 값과 일치하는지 여부
 * @param[out] aLatchReleased aPageHandle의 Latch 해제 여부
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smdmpfGetValidVersionAndCompare( smdRowInfo          * aRowInfo,
                                           stlInt16              aSlotSeq,
                                           smlScn              * aCommitScn,
                                           smxlTransId         * aWaitTransId,
                                           stlBool             * aIsMatch,
                                           stlBool             * aLatchReleased,
                                           smlEnv              * aEnv )
{
    knlValueBlockList * sCurCol;
    knlValueBlockList * sTempCurCol;
    knlValueBlockList * sLastCol= NULL;
    smlRid              sRollbackRid;
    smlRid              sNextRid;
    smlTcn              sTcn;
    stlInt16            sColCount = 0;
    stlInt16            sCurColCount = 0;
    stlChar             sRowHdrBuf[SMDMPF_ROW_HDR_LEN];
    stlInt32            sStartColOrd = 0;
    smpHandle           sPageHandle = *aRowInfo->mPageHandle;
    stlInt32            sState = 0;
    stlBool             sIsMyTrans = STL_FALSE;
    smlScn              sCommitScn;
    stlBool             sIsFirstPiece = STL_TRUE;
    stlChar           * sRowHdr = aRowInfo->mRow;
    knlValueBlockList * sColList = aRowInfo->mKeyCompList->mValueList;
    stlInt32            sBlockIdx = aRowInfo->mBlockIdx;
    stlBool             sHasValidVersion = STL_FALSE;
    stlUInt16           sKeyCompCnt = 0;
    stlBool             sResult = STL_TRUE;
    stlUInt8            sColLenSize;
    stlInt64            sColLen;
    dtlDataValue      * sDataValue;
    stlBool             sIsZero;
    smlPid              sChainedPageId;
    stlBool             sAlreadyCopyCols = STL_FALSE;
    stlBool             sIsContCol;
    stlInt32            sLastColOrder = 0;
    stlInt32            sColOrder;
    stlChar           * sColPtr;

    STL_ASSERT( SMDMPF_IS_MASTER_ROW( sRowHdr ) == STL_TRUE );

    *aLatchReleased = STL_FALSE;
    *aIsMatch = STL_TRUE;
    sCurCol = sColList;
    sChainedPageId = SMDMPF_GET_NEXT_CHAINED_PAGE_ID(SMP_FRAME(&sPageHandle));

    while( STL_TRUE )  /* 모든 row piece들에 대해 */
    {
        sIsFirstPiece = SMDMPF_IS_MASTER_ROW( sRowHdr );
        sHasValidVersion = STL_FALSE;
        sColCount = 0;
        
        while( STL_TRUE )  /* 한 row piece의 모든 버전들에 대해 */
        {
            STL_TRY( smdmpfGetCommitScn( aRowInfo->mTbsId,
                                         &sPageHandle,
                                         sRowHdr,
                                         &sCommitScn,
                                         aWaitTransId,
                                         aEnv ) == STL_SUCCESS );

            if( sIsFirstPiece == STL_TRUE )
            {
                *aCommitScn = sCommitScn;
            }

            SMDMPF_GET_COLUMN_CNT( sRowHdr, &sCurColCount );
            
            /**
             * DDL( ex. add column ) 이전의 view을 봐야 하는 경우라면
             * trailing null도 읽은 column으로 설정해야 한다.
             */
            sColCount = STL_MAX( sColCount, sCurColCount );

            /* 1. 내 tx인지 체크 */
            if( SMDMPF_IS_SAME_TRANS( aRowInfo->mTransId,
                                      aRowInfo->mCommitScn,
                                      *aWaitTransId,
                                      sCommitScn ) == STL_TRUE )
            {
                sIsMyTrans = STL_TRUE;
                SMDMPF_GET_TCN( sRowHdr, &sTcn );
                if( (aRowInfo->mTcn == SML_INFINITE_TCN) || (sTcn < aRowInfo->mTcn) )
                {
                    /* 내 Tx의 읽을 수 있는 버전을 찾아냄 */
                    sHasValidVersion = STL_TRUE;
                    break;
                }
            }
            else
            {
                sIsMyTrans = STL_FALSE;
                /* 2. 내 tx가 아닌 경우 */
                /* SML_TRM_COMMITTED인 경우 aMyViewScn이 infinite라서 커밋된 모든 레코드를 자동으로 보게 됨 */
                if( (sCommitScn <= aRowInfo->mViewScn) &&
                    (sCommitScn != SML_INFINITE_SCN) )
                {
                    sHasValidVersion = ( !SMDMPF_IS_DELETED( sRowHdr ) );
                    break;
                }
            }

            STL_DASSERT( sHasValidVersion == STL_FALSE );

            sRollbackRid.mTbsId =  SML_MEMORY_UNDO_SYSTEM_TBS_ID;
            SMDMPF_GET_ROLLBACK_PID( sRowHdr, &sRollbackRid.mPageId );
            SMDMPF_GET_ROLLBACK_OFFSET( sRowHdr, &sRollbackRid.mOffset );

            /* 읽을 수 있는 버전이 아님. 이전 버전으로 되돌림 */
            if( sRollbackRid.mPageId == SMP_NULL_PID )
            {
                sHasValidVersion = STL_FALSE;
                break;
            }

            if( (sAlreadyCopyCols == STL_FALSE) && (sCurCol != NULL) )
            {
                /* 이전버전을 읽으면 값이 바뀌게 되므로
                 * 여기서는 physical filter를 처리하지 않는다.
                 * 현재 row piece에 읽을 컬럼이 없다면 read하지 않는다. */
                if( (sStartColOrd <= KNL_GET_BLOCK_COLUMN_ORDER(sCurCol)) &&
                    (sStartColOrd + sColCount > KNL_GET_BLOCK_COLUMN_ORDER(sCurCol)) )
                {
                    STL_TRY( smdmpfReadRecentColValues( &sPageHandle,
                                                        aRowInfo,
                                                        aSlotSeq,
                                                        sColCount,
                                                        sCurCol,
                                                        sBlockIdx,
                                                        &sLastCol,
                                                        aEnv )
                             == STL_SUCCESS );
                }
                sAlreadyCopyCols = STL_TRUE;
            }

            if( sRowHdr != sRowHdrBuf )
            {
                SMDMPF_COPY_ROW_HDR( sRowHdrBuf, sRowHdr );
            }

            /* 해당 row piece의 이전 버전을 구해온다 */
            STL_TRY( smdmpfMakePrevVersion( aRowInfo,
                                            &sRollbackRid,
                                            sRowHdrBuf,
                                            sCurCol,
                                            sBlockIdx,
                                            aEnv ) == STL_SUCCESS );

            sRowHdr = sRowHdrBuf;
        } /* while */

        sIsFirstPiece = STL_FALSE;
        
        if( (SMDMPF_IS_DELETED( sRowHdr ) == STL_TRUE) &&
            ((aRowInfo->mViewScn != SML_INFINITE_SCN) || (sIsMyTrans == STL_TRUE)) &&
            (sHasValidVersion == STL_TRUE) )
        {
            sHasValidVersion = STL_FALSE;
        }

        if( sHasValidVersion == STL_FALSE )
        {
            /* valid version을 못찾음 */
            *aIsMatch = STL_FALSE;
            break;
        }

        if( sCurCol == NULL )
        {
            /* 더 이상 row piece를 fetch할 필요가 없다 */
            break;
        }

        if( (sStartColOrd <= KNL_GET_BLOCK_COLUMN_ORDER(sCurCol)) &&
            (sStartColOrd + sColCount > KNL_GET_BLOCK_COLUMN_ORDER(sCurCol)) )
        {
            if( sAlreadyCopyCols == STL_FALSE )
            {
                sLastColOrder = sStartColOrd + sColCount - 1;
                sTempCurCol   = sCurCol;
                sLastCol      = sCurCol;

                while( sTempCurCol != NULL )
                {
                    sColOrder = KNL_GET_BLOCK_COLUMN_ORDER(sTempCurCol);

                    if( sColOrder > sLastColOrder )
                    {
                        break;
                    }

                    sColPtr = smdmpfGetNthColValue( &sPageHandle,
                                                    aSlotSeq,
                                                    sColOrder );

                    STL_DASSERT( sColPtr != NULL );

                    SMP_GET_FIXED_COLUMN_LEN( sColPtr,
                                              &sColLenSize,
                                              &sColLen,
                                              sIsContCol,
                                              sIsZero );

                    STL_DASSERT( sIsContCol != STL_TRUE );

                    if( sIsZero == STL_TRUE )
                    {
                        sDataValue = KNL_GET_BLOCK_FIRST_DATA_VALUE(sCurCol);
                        DTL_NUMERIC_SET_ZERO( sDataValue->mValue, sDataValue->mLength );

                        if( aRowInfo->mKeyCompList->mIndexColLen[sKeyCompCnt] != 0 )
                        {
                            sResult = aRowInfo->mKeyCompList->mCompFunc[sKeyCompCnt]
                                ( sDataValue->mValue,
                                  sDataValue->mLength,
                                  aRowInfo->mKeyCompList->mIndexColVal[sKeyCompCnt],
                                  aRowInfo->mKeyCompList->mIndexColLen[sKeyCompCnt] );

                            if( sResult != STL_TRUE )
                            {
                                /* column 값이 다름 */
                                *aIsMatch = STL_FALSE;
                                break;
                            }
                        }
                        else
                        {
                            /* column 값이 다름 */
                            *aIsMatch = STL_FALSE;
                            break;
                        }
                    }
                    else
                    {
                        if( (aRowInfo->mKeyCompList->mIndexColLen[sKeyCompCnt] != 0) &&
                            (sColLen != 0 ) )
                        {
                            sResult = aRowInfo->mKeyCompList->mCompFunc[sKeyCompCnt]
                                ( sColPtr + sColLenSize,
                                  sColLen,
                                  aRowInfo->mKeyCompList->mIndexColVal[sKeyCompCnt],
                                  aRowInfo->mKeyCompList->mIndexColLen[sKeyCompCnt] );

                            if( sResult != STL_TRUE )
                            {
                                /* column 값이 다름 */
                                *aIsMatch = STL_FALSE;
                                break;
                            }
                        }
                        else
                        {
                            /* column 값이 다름 */
                            *aIsMatch = STL_FALSE;
                            break;
                        }
                    }

                    sKeyCompCnt++;
                    sLastCol = sCurCol;
                    sCurCol = sCurCol->mNext;
                    if( sCurCol == NULL )
                    {
                        break;
                    }
                }
            }
            else
            {
                /* buffered compare */
                while( sCurCol != NULL )
                {
                    sColOrder = KNL_GET_BLOCK_COLUMN_ORDER(sCurCol);

                    if( (sStartColOrd + sColCount) <= sColOrder )
                    {
                        break;
                    }

                    if( (aRowInfo->mKeyCompList->mIndexColLen[sKeyCompCnt] != 0) &&
                        (KNL_GET_BLOCK_DATA_LENGTH(sCurCol, 0) != 0 ) )
                    {
                        sDataValue = KNL_GET_BLOCK_FIRST_DATA_VALUE(sCurCol);

                        sResult = aRowInfo->mKeyCompList->mCompFunc[sKeyCompCnt]
                            ( sDataValue->mValue,
                              sDataValue->mLength,
                              aRowInfo->mKeyCompList->mIndexColVal[sKeyCompCnt],
                              aRowInfo->mKeyCompList->mIndexColLen[sKeyCompCnt] );

                        if( sResult != STL_TRUE )
                        {
                            /* column 값이 다름 */
                            *aIsMatch = STL_FALSE;
                            break;
                        }
                    }
                    else
                    {
                        /* column 값이 다름 */
                        *aIsMatch = STL_FALSE;
                        break;
                    }

                    sKeyCompCnt++;
                    sLastCol = sCurCol;
                    sCurCol = sCurCol->mNext;
                }
            }
        }

        if( *aIsMatch == STL_FALSE )
        {
            break;
        }

        if( sCurCol == NULL )
        {
            break;
        }
        else
        {
            if( sChainedPageId != SMP_NULL_PID )
            {
                sStartColOrd += sColCount;
                /* next member page가 있으면 */
                sNextRid.mPageId = sChainedPageId;
                sNextRid.mOffset = aSlotSeq;
            }
            else
            {
                /**
                 * Trailing null
                 */

                while( sCurCol != NULL )
                {
                    sResult = ( aRowInfo->mKeyCompList->mIndexColLen[sKeyCompCnt] == 0 );
                                
                    if( sResult != STL_TRUE )
                    {
                        /* column 값이 다름 */
                        *aIsMatch = STL_FALSE;
                        break;
                    }
                        
                    sKeyCompCnt++;
                    sCurCol = sCurCol->mNext;
                }
                break;
            }
        }
        
        /* next piece를 읽을 준비를 함 */
        if( SMP_IS_DUMMY_HANDLE( &sPageHandle ) != STL_TRUE )
        {
            sState = 0;
            STL_TRY( smpRelease( &sPageHandle, aEnv ) == STL_SUCCESS );
            *aLatchReleased = STL_TRUE;
        }
        
        STL_TRY( smpAcquire( NULL,
                             aRowInfo->mTbsId,
                             sNextRid.mPageId,
                             KNL_LATCH_MODE_SHARED,
                             &sPageHandle,
                             aEnv ) == STL_SUCCESS );
        
        sState = 1;

        sChainedPageId = SMDMPF_GET_NEXT_CHAINED_PAGE_ID(SMP_FRAME(&sPageHandle));
        sRowHdr = SMDMPF_GET_NTH_ROW_HDR( SMP_FRAME(&sPageHandle), sNextRid.mOffset );
        sAlreadyCopyCols = STL_FALSE;
    } /* while */

    if( sState == 1 )
    {
        sState = 0;
        STL_TRY( smpRelease( &sPageHandle, aEnv ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smpRelease( &sPageHandle, aEnv );
    }

    return STL_FAILURE;
}

stlStatus smdmpfGetValidVersions( smdmpfIterator    * aIterator,
                                  stlChar           * aPageFrame,
                                  smdRowInfo        * aRowInfo,
                                  smlFetchInfo      * aFetchInfo,
                                  stlInt16            aSlotSeq,
                                  stlBool           * aEndOfSlot,
                                  smlEnv            * aEnv )
{
    knlValueBlockList * sCurCol;
    knlValueBlockList * sLastCol= NULL;
    stlChar           * sColPtr;
    smlRid              sRollbackRid;
    smlRid              sNextRid;
    smlTcn              sTcn;
    stlInt16            sColCount;
    stlInt16            sCurColCount;
    stlChar             sRowHdrBuf[SMDMPF_ROW_HDR_LEN];
    stlBool             sAlreadyCopyCols = STL_FALSE;
    stlInt32            sStartColOrd = 0;
    smpHandle           sPageHandle = *aRowInfo->mPageHandle;
    stlInt32            sState = 0;
    stlBool             sIsMyTrans = STL_FALSE;
    smlScn              sCommitScn;
    stlChar           * sRowHdr;
    stlInt32            sBlockIdx = 0;
    stlBool             sHasValidVersion;
    smlScn              sMasterCommitScn = 0;
    smxlTransId         sWaitTransId;
    stlInt16            sSlotSeq;
    stlInt64            sBlockSize = SML_BLOCK_SIZE( aFetchInfo->mRowBlock );

    stlUInt8             sColLenSize = 0;
    stlInt64             sColLen = 0;
    knlPhysicalFilter  * sPhysicalFilter;
    knlColumnInReadRow * sCurInReadRow;
    knlValueBlockList  * sTempCurCol;
    dtlDataValue       * sDataValue;
    smpRowItemIterator * sSlotIterator;
    
    stlInt32            sLastColOrder = 0;
    stlInt32            sAggrRowCount = 0;
    stlBool             sIsZero;
    stlBool             sIsContCol;
    stlInt32            sColOrder;
    smlPid              sFirstSlavePageId;
    smlPid              sChainedPageId;

    /**
     * Continuous column 때문에 page handle은 dummy 여야 한다.
     */
    STL_DASSERT( SMP_IS_DUMMY_HANDLE( aRowInfo->mPageHandle ) == STL_TRUE );

    sSlotIterator = &aIterator->mCommon.mSlotIterator;
    *aEndOfSlot = STL_TRUE;
    
    if( aFetchInfo->mAggrFetchInfo != NULL )
    {
        sBlockIdx = 0;
    }
    else
    {
        sBlockIdx = SML_USED_BLOCK_SIZE( aFetchInfo->mRowBlock );
    }

    sPageHandle.mFrame = aPageFrame;

    if( aSlotSeq == -1 )
    {
        /* init slot iterator */
        SMDMPF_INIT_ROWITEM_ITERATOR( SMP_FRAME(&sPageHandle), sSlotIterator );
    }

    sFirstSlavePageId = SMDMPF_GET_NEXT_CHAINED_PAGE_ID(SMP_FRAME(&sPageHandle));

    SMDMPF_FOREACH_ROWITEM_ITERATOR( sSlotIterator )
    {
        SMP_MAKE_DUMMY_HANDLE( &sPageHandle, aPageFrame );
        sRowHdr = (stlChar*)sSlotIterator->mCurSlot;
        sSlotSeq = SMP_GET_CURRENT_ROWITEM_SEQ( sSlotIterator );
        
        if( SMDMPF_IS_IN_USE( sRowHdr ) != STL_TRUE )
        {
            continue;
        }
        
        sCurCol = aRowInfo->mColList;
        sStartColOrd = 0;
        sLastCol= NULL;
        sPhysicalFilter = aFetchInfo->mPhysicalFilter;
        sChainedPageId = sFirstSlavePageId;

        do  /* 모든 row piece들에 대해 */
        {
            sColCount = 0;
            sHasValidVersion = STL_FALSE;
            sAlreadyCopyCols = STL_FALSE;

            do  /* 한 row piece의 모든 버전들에 대해 */
            {
                /* Master page의 row header에서 commit scn을 구한다. */
                STL_TRY( smdmpfGetCommitScn( aRowInfo->mTbsId,
                                             &sPageHandle,
                                             sRowHdr,
                                             &sCommitScn,
                                             &sWaitTransId,
                                             aEnv ) == STL_SUCCESS );

                SMDMPF_GET_COLUMN_CNT( sRowHdr, &sCurColCount );

                sColCount = STL_MAX( sColCount, sCurColCount );

                /* 1. 내 tx인지 체크 */
                if( SMDMPF_IS_SAME_TRANS( aRowInfo->mTransId,
                                          aRowInfo->mCommitScn,
                                          sWaitTransId,
                                          sCommitScn ) == STL_TRUE )
                {
                    sIsMyTrans = STL_TRUE;
                    SMDMPF_GET_TCN( sRowHdr, &sTcn );
                    if( (aRowInfo->mTcn == SML_INFINITE_TCN) || (sTcn < aRowInfo->mTcn) )
                    {
                        /* 내 Tx의 읽을 수 있는 버전을 찾아냄 */
                        sHasValidVersion = STL_TRUE;
                        break;
                    }
                }
                else
                {
                    sIsMyTrans = STL_FALSE;
                    /* 2. 내 tx가 아닌 경우 */
                    /* SML_TRM_COMMITTED인 경우 aMyViewScn이 infinite라서 커밋된 모든 레코드를 자동으로 보게 됨 */
                    if( (sCommitScn <= aRowInfo->mViewScn) &&
                        (sCommitScn != SML_INFINITE_SCN) )
                    {
                        sHasValidVersion = ( !SMDMPF_IS_DELETED( sRowHdr ) );
                        break;
                    }
                }

                sRollbackRid.mTbsId = SML_MEMORY_UNDO_SYSTEM_TBS_ID;
                SMDMPF_GET_ROLLBACK_PID( sRowHdr, &sRollbackRid.mPageId );
                SMDMPF_GET_ROLLBACK_OFFSET( sRowHdr, &sRollbackRid.mOffset );

                /* 읽을 수 있는 버전이 아님. 이전 버전으로 되돌림 */
                if( sRollbackRid.mPageId == SMP_NULL_PID )
                {
                    sHasValidVersion = STL_FALSE;
                    break;
                }

                if( (sAlreadyCopyCols == STL_FALSE) && (sCurCol != NULL) )
                {
                    /* 이전버전을 읽으면 값이 바뀌게 되므로
                     * 여기서는 physical filter를 처리하지 않는다.
                     * 현재 row piece에 읽을 컬럼이 없다면 read하지 않는다. */
                    if( (sStartColOrd <= KNL_GET_BLOCK_COLUMN_ORDER(sCurCol)) &&
                        (sStartColOrd + sColCount > KNL_GET_BLOCK_COLUMN_ORDER(sCurCol)) )
                    {
                        STL_TRY( smdmpfReadRecentColValues( &sPageHandle,
                                                            aRowInfo,
                                                            sSlotSeq,
                                                            sColCount,
                                                            sCurCol,
                                                            sBlockIdx,
                                                            &sLastCol,
                                                            aEnv )
                                 == STL_SUCCESS );
                    }
                    sAlreadyCopyCols = STL_TRUE;
                }

                if( sRowHdr != sRowHdrBuf )
                {
                    SMDMPF_COPY_ROW_HDR( sRowHdrBuf, sRowHdr );
                }

                /* 해당 row piece의 이전 버전을 구해온다 */
                STL_TRY( smdmpfMakePrevVersion( aRowInfo,
                                                &sRollbackRid,
                                                sRowHdrBuf,
                                                sCurCol,
                                                sBlockIdx,
                                                aEnv ) == STL_SUCCESS );

                sRowHdr = sRowHdrBuf;
            } while( STL_TRUE );

            if( (SMDMPF_IS_DELETED( sRowHdr ) == STL_TRUE) &&
                ((aRowInfo->mViewScn != SML_INFINITE_SCN) || (sIsMyTrans == STL_TRUE)) &&
                (sHasValidVersion == STL_TRUE) )
            {
                sHasValidVersion = STL_FALSE;
            }

            if( sHasValidVersion == STL_TRUE )
            {
                /**
                 * 현재 row에서 읽을 column이 존재하지 않으면 filter를 평가하지 않는다.
                 */
                STL_TRY_THROW( sColCount > 0, RAMP_READ_NEXT_ROW );

                if( sCurCol == NULL )
                {
                    /**
                     * 더 이상 Copy할 Column이 없다면,
                     * 평가되지 않은 Physical Filter도 없어야 한다.
                     */
                    STL_DASSERT( sPhysicalFilter == NULL );
                    break;
                }

                if( sAlreadyCopyCols == STL_FALSE )
                {
                    /* 현재 row piece에 평가할 컬럼이 없다면 다음 row piece로 */
                    if( (sStartColOrd > KNL_GET_BLOCK_COLUMN_ORDER(sCurCol)) ||
                        (sStartColOrd + sColCount <= KNL_GET_BLOCK_COLUMN_ORDER(sCurCol)) )
                    {
                        sStartColOrd += sColCount;
                        STL_THROW( RAMP_READ_NEXT_ROW );
                    }

                    sLastColOrder = sStartColOrd + sColCount - 1;
                    sTempCurCol   = sCurCol;
                    sLastCol      = sCurCol;

                    while( sTempCurCol != NULL )
                    {
                        sColOrder = KNL_GET_BLOCK_COLUMN_ORDER( sTempCurCol );

                        if( sColOrder > sLastColOrder )
                        {
                            break;
                        }

                        sCurInReadRow = KNL_GET_BLOCK_COLUMN_IN_ROW( sTempCurCol );
                        sColPtr = smdmpfGetNthColValue( &sPageHandle,
                                                        sSlotSeq,
                                                        sColOrder );

                        STL_DASSERT( sColPtr != NULL );

                        SMP_GET_FIXED_COLUMN_LEN( sColPtr,
                                                  &sColLenSize,
                                                  &sColLen,
                                                  sIsContCol,
                                                  sIsZero );

                        if( sIsContCol == STL_TRUE )
                        {
                            /**
                             * continuous column의 경우 next piece를 읽어야 함
                             */
                            STL_TRY( smdmpfReadContCols( aRowInfo->mTbsId,
                                                         sTempCurCol,
                                                         sBlockIdx,
                                                         sColPtr,
                                                         aEnv ) == STL_SUCCESS );

                            sDataValue = KNL_GET_BLOCK_SEP_DATA_VALUE(sTempCurCol, sBlockIdx);

                            sCurInReadRow->mValue  = sDataValue->mValue;
                            sCurInReadRow->mLength = sDataValue->mLength;
                        }
                        else
                        {
                            if( sIsZero == STL_FALSE )
                            {
                                sCurInReadRow->mValue  = sColPtr + sColLenSize;
                                sCurInReadRow->mLength = sColLen;
                            }
                            else
                            {
                                sDataValue = KNL_GET_BLOCK_SEP_DATA_VALUE(sTempCurCol, sBlockIdx);

                                DTL_NUMERIC_SET_ZERO( sDataValue->mValue,
                                                      sDataValue->mLength );

                                sCurInReadRow->mValue  = sDataValue->mValue;
                                sCurInReadRow->mLength = 1;
                            }
                        }

                        sTempCurCol = sTempCurCol->mNext;
                        sLastCol    = sTempCurCol;

                        while( sPhysicalFilter != NULL )
                        {
                            if( sPhysicalFilter->mColIdx1 <= sColOrder )
                            {
                                /* 위 sCurInReadRow의 셋팅에서 이미 physical filter가
                                 * 가리키는 column value의 위치를 저장했다. */
                                if( sPhysicalFilter->mFunc( sPhysicalFilter->mColVal1->mValue,
                                                            sPhysicalFilter->mColVal1->mLength,
                                                            sPhysicalFilter->mColVal2->mValue,
                                                            sPhysicalFilter->mColVal2->mLength )
                                    == STL_FALSE )
                                {
                                    sHasValidVersion = STL_FALSE;
                                    STL_THROW( RAMP_FINISH_READ_ROWS );
                                }
                            }
                            else
                            {
                                break;
                            }

                            sPhysicalFilter = sPhysicalFilter->mNext;
                        }
                    }

                    /* 현재 row 에 포함된 column들을 value block 에 복사 */
                    while( sCurCol != sLastCol )
                    {
                        sCurInReadRow = KNL_GET_BLOCK_COLUMN_IN_ROW( sCurCol );
                        sDataValue    = KNL_GET_BLOCK_SEP_DATA_VALUE( sCurCol, sBlockIdx );

                        DTL_COPY_VALUE( sDataValue,
                                        sCurInReadRow->mValue,
                                        sCurInReadRow->mLength );
                        sDataValue->mLength = sCurInReadRow->mLength;

                        sCurCol = sCurCol->mNext;
                    }
                }
                else
                {
                    sLastColOrder = sStartColOrd + sColCount - 1;

                    if( sLastCol != NULL )
                    {
                        while( sPhysicalFilter != NULL )
                        {
                            if( sPhysicalFilter->mColIdx1 <= sLastColOrder )
                            {
                                /* 위 sCurInReadRow의 셋팅에서 이미 physical filter가
                                 * 가리키는 column value의 위치를 저장했다. */
                                if( sPhysicalFilter->mFunc( sPhysicalFilter->mColVal1->mValue,
                                                            sPhysicalFilter->mColVal1->mLength,
                                                            sPhysicalFilter->mColVal2->mValue,
                                                            sPhysicalFilter->mColVal2->mLength )
                                    == STL_FALSE )
                                {
                                    sHasValidVersion = STL_FALSE;
                                    STL_THROW( RAMP_FINISH_READ_ROWS );
                                }
                            }
                            else
                            {
                                break;
                            }

                            sPhysicalFilter = sPhysicalFilter->mNext;
                        }
                    
                        /**
                         * DDL( ex. add column ) 이전의 view을 봐야 하는 경우라면
                         * Last Column을 증가시키지 않는다.
                         */
                        while( sLastColOrder >= KNL_GET_BLOCK_COLUMN_ORDER(sLastCol) )
                        {
                            sLastCol = sLastCol->mNext;
                            sCurCol  = sLastCol;

                            if( sCurCol == NULL )
                            {
                                break;
                            }
                        }
                    }
                }

                sStartColOrd = sLastColOrder + 1;
            }
            else
            {
                /* valid version을 못찾음 */
                break;
            }

            STL_RAMP( RAMP_READ_NEXT_ROW );

            /**
             * 다음 Row를 읽을지 여부 판단
             */
            if( sCurCol == NULL )
            {
                /* 더 이상 row piece를 fetch할 필요가 없다 */
                break;
            }
            else
            {
                if( sChainedPageId != SMP_NULL_PID )
                {
                    /* next member page가 있으면 */
                    sNextRid.mPageId = sChainedPageId;
                    sNextRid.mOffset = sSlotSeq;
                }
                else
                {
                    /**
                     * Trailing null
                     */
                    
                    while( sCurCol != NULL )
                    {
                        while( sPhysicalFilter != NULL )
                        {
                            sPhysicalFilter->mColVal1->mLength = 0;
                            
                            if( sPhysicalFilter->mColIdx1 <= KNL_GET_BLOCK_COLUMN_ORDER(sCurCol) )
                            {
                                if( sPhysicalFilter->mFunc( sPhysicalFilter->mColVal1->mValue,
                                                            sPhysicalFilter->mColVal1->mLength,
                                                            sPhysicalFilter->mColVal2->mValue,
                                                            sPhysicalFilter->mColVal2->mLength )
                                    == STL_FALSE )
                                {
                                    sHasValidVersion = STL_FALSE;
                                    STL_THROW( RAMP_FINISH_READ_ROWS );
                                }
                            }
                            else
                            {
                                break;
                            }

                            sPhysicalFilter = sPhysicalFilter->mNext;
                        }
                        
                        sDataValue = KNL_GET_BLOCK_SEP_DATA_VALUE( sCurCol, sBlockIdx );
                        sDataValue->mLength = 0;
                        sCurCol = sCurCol->mNext;
                    }
                    
                    break;
                }
            }

            /* next piece를 읽을 준비를 함 */
            if( SMP_IS_DUMMY_HANDLE( &sPageHandle ) != STL_TRUE )
            {
                sState = 0;
                STL_TRY( smpRelease( &sPageHandle, aEnv ) == STL_SUCCESS );
            }
        
            STL_TRY( smpAcquire( NULL,
                                 aRowInfo->mTbsId,
                                 sNextRid.mPageId,
                                 KNL_LATCH_MODE_SHARED,
                                 &sPageHandle,
                                 aEnv ) == STL_SUCCESS );
        
            sState = 1;

            sChainedPageId = SMDMPF_GET_NEXT_CHAINED_PAGE_ID(SMP_FRAME(&sPageHandle));
            sRowHdr = SMDMPF_GET_NTH_ROW_HDR( SMP_FRAME(&sPageHandle), sNextRid.mOffset );
        } while( STL_TRUE ); /* while */
        
        STL_RAMP( RAMP_FINISH_READ_ROWS );

        if( sState == 1 )
        {
            sState = 0;
            STL_TRY( smpRelease( &sPageHandle, aEnv ) == STL_SUCCESS );
        }

        if( sHasValidVersion == STL_FALSE )
        {
            continue;
        }

        aIterator->mRowRid.mPageId = aIterator->mPageBufPid;
        aIterator->mRowRid.mOffset = sSlotSeq;
        
        STL_DASSERT( aRowInfo->mViewScn == SMI_GET_VIEW_SCN( aIterator ) );
        
        /**
         * DDL statement는 현재 commit scn보다 작은 view scn을 가질수도 있다.
         * - SCN_VALUE는 version conflict를 확인하기 위해서 사용되므로 MAX값을 반환해야 한다. 
         */
        if( sMasterCommitScn == SML_INFINITE_SCN )
        {
            sMasterCommitScn = SMI_GET_VIEW_SCN( aIterator );
        }
        else
        {
            sMasterCommitScn = STL_MAX( sMasterCommitScn, SMI_GET_VIEW_SCN( aIterator ) );
        }
                 
        SML_SET_RID_VALUE( aFetchInfo->mRowBlock, sBlockIdx, aIterator->mRowRid );
        SML_SET_SCN_VALUE( aFetchInfo->mRowBlock, sBlockIdx, sMasterCommitScn );

        /**
         * RowId Column을 참조하는 경우, RowId Column Block에 값을 만든다. 
         */
        if( aRowInfo->mRowIdColList != NULL )
        {
            STL_TRY( dtlRowIdSetValueFromRowIdInfo(
                         aRowInfo->mTableLogicalId,
                         aIterator->mRowRid.mTbsId,
                         aIterator->mRowRid.mPageId,
                         aIterator->mRowRid.mOffset,
                         KNL_GET_BLOCK_SEP_DATA_VALUE( aRowInfo->mRowIdColList,
                                                       sBlockIdx ),
                         KNL_ERROR_STACK(aEnv) )
                     == STL_SUCCESS );            
        }
        
        /**
         * Logical Filter 평가
         */
        if( aFetchInfo->mFilterEvalInfo != NULL )
        {
            aFetchInfo->mFilterEvalInfo->mBlockIdx = sBlockIdx;

            STL_TRY( knlEvaluateOneExpression( aFetchInfo->mFilterEvalInfo,
                                               KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            STL_DASSERT( KNL_GET_BLOCK_IS_SEP_BUFF(
                             aFetchInfo->mFilterEvalInfo->mResultBlock )
                         == STL_TRUE );

            if( !DTL_BOOLEAN_IS_TRUE( KNL_GET_BLOCK_SEP_DATA_VALUE( 
                                          aFetchInfo->mFilterEvalInfo->mResultBlock,
                                          sBlockIdx ) ) )
            {
                continue;
            }
        }


        /**
         * evaluate ggregation
         */
        
        if( aFetchInfo->mAggrFetchInfo != NULL )
        {
            sBlockIdx++;
            
            if( sBlockIdx == sBlockSize )
            {
                STL_TRY( smoBlockBuildAggrFunc( aFetchInfo->mAggrFetchInfo,
                                                0, /* aStartBlockIdx */
                                                sBlockSize,
                                                aEnv )
                         == STL_SUCCESS );
                sBlockIdx = 0;
                sAggrRowCount = 1;
            }
        }
        else
        {
            if( aFetchInfo->mSkipCnt > 0 )
            {
                (aFetchInfo->mSkipCnt)--;
                continue;
            }
            else
            {
                (aFetchInfo->mFetchCnt)--;
                sBlockIdx++;
            }
        
            if( (aFetchInfo->mFetchCnt == 0) ||
                (sBlockIdx == sBlockSize) )
            {
                *aEndOfSlot = STL_FALSE;
                break;
            }
        }
    }

    if( aFetchInfo->mAggrFetchInfo == NULL )
    {
        if( aRowInfo->mColList != NULL )
        {
            KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( aRowInfo->mColList, 0, sBlockIdx );
        }

        if( aRowInfo->mRowIdColList != NULL )
        {
            KNL_SET_ONE_BLOCK_SKIP_CNT( aRowInfo->mRowIdColList, 0 );
            KNL_SET_ONE_BLOCK_USED_CNT( aRowInfo->mRowIdColList, sBlockIdx );
        }
    
        SML_SET_USED_BLOCK_SIZE( aFetchInfo->mRowBlock, sBlockIdx );
    }
    else
    {
        if( sBlockIdx > 0 )
        {
            STL_TRY( smoBlockBuildAggrFunc( aFetchInfo->mAggrFetchInfo,
                                            0, /* aStartBlockIdx */
                                            sBlockIdx,
                                            aEnv )
                     == STL_SUCCESS );
            sAggrRowCount = 1;
        }

        SML_SET_USED_BLOCK_SIZE( aFetchInfo->mRowBlock, sAggrRowCount );
    }

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smpRelease( &sPageHandle, aEnv );
    }

    return STL_FAILURE;
}

stlStatus smdmpfGetValidVersionsSimplePage( smdmpfIterator    * aIterator,
                                            stlChar           * aPageFrame,
                                            smdRowInfo        * aRowInfo,
                                            smlFetchInfo      * aFetchInfo,
                                            stlInt16            aSlotSeq,
                                            stlBool           * aEndOfSlot,
                                            smlEnv            * aEnv )
{
    knlValueBlockList * sCurCol;
    knlValueBlockList * sLastCol= NULL;
    stlInt16            sColCount;
    stlChar           * sRow;
    stlInt32            sBlockIdx = 0;
    stlBool             sHasValidVersion;
    stlInt16            sSlotSeq;
    stlInt64            sBlockSize = SML_BLOCK_SIZE( aFetchInfo->mRowBlock );

    stlUInt8             sColLenSize = 0;
    stlInt64             sColLen = 0;
    knlPhysicalFilter  * sPhysicalFilter;
    knlColumnInReadRow * sCurInReadRow;
    knlValueBlockList  * sTempCurCol;
    smpRowItemIterator * sSlotIterator;
    
    dtlDataValue      * sDataValue;
    stlInt32            sAggrRowCount = 0;
    stlBool             sIsZero;
    stlChar           * sColPtr;
    stlInt32            sColOrder;
    smpHandle           sPageHandle;
    
    sSlotIterator = &aIterator->mCommon.mSlotIterator;
    *aEndOfSlot = STL_TRUE;
    
    if( aFetchInfo->mAggrFetchInfo != NULL )
    {
        sBlockIdx = 0;
    }
    else
    {
        sBlockIdx = SML_USED_BLOCK_SIZE( aFetchInfo->mRowBlock );
    }

    sPageHandle.mFrame = aPageFrame;

    if( aSlotSeq == -1 )
    {
        /* init slot iterator */
        SMDMPF_INIT_ROWITEM_ITERATOR( SMP_FRAME(&sPageHandle), sSlotIterator );
    }

    SMDMPF_FOREACH_ROWITEM_ITERATOR( sSlotIterator )
    {
        SMP_MAKE_DUMMY_HANDLE( &sPageHandle, aPageFrame );
        sRow = (stlChar*)sSlotIterator->mCurSlot;
        sSlotSeq = SMP_GET_CURRENT_ROWITEM_SEQ( sSlotIterator );
        
        sHasValidVersion = STL_TRUE;
        sCurCol = aRowInfo->mColList;
        sLastCol= NULL;
        sPhysicalFilter = NULL;

        if( SMDMPF_IS_IN_USE( sRow ) != STL_TRUE )
        {
            sHasValidVersion = STL_FALSE;
            continue;
        }

        if( SMDMPF_IS_DELETED( sRow ) == STL_TRUE )
        {
            sHasValidVersion = STL_FALSE;
            continue;
        }

        /**
         * Physical Filter 평가
         */
                   
        if( sCurCol != NULL )
        {
            SMDMPF_GET_COLUMN_CNT( sRow, &sColCount );

            sTempCurCol   = sCurCol;
            sLastCol      = sCurCol;
            sPhysicalFilter = aFetchInfo->mPhysicalFilter;

            if( sPhysicalFilter != NULL )
            {
                while( sTempCurCol != NULL )
                {
                    sColOrder = KNL_GET_BLOCK_COLUMN_ORDER(sTempCurCol);

                    sColPtr = smdmpfGetNthColValue( &sPageHandle,
                                                    sSlotIterator->mCurSeq,
                                                    sColOrder );

                    STL_DASSERT( sColPtr != NULL );

                    SMP_GET_COLUMN_LEN( sColPtr,
                                        &sColLenSize,
                                        &sColLen,
                                        sIsZero );

                    sCurInReadRow = KNL_GET_BLOCK_COLUMN_IN_ROW( sTempCurCol );
                        
                    if( sIsZero == STL_FALSE )
                    {
                        sCurInReadRow->mValue  = sColPtr + sColLenSize;
                        sCurInReadRow->mLength = sColLen;
                    }
                    else
                    {
                        sDataValue = KNL_GET_BLOCK_SEP_DATA_VALUE( sTempCurCol, sBlockIdx );
                            
                        DTL_NUMERIC_SET_ZERO( sDataValue->mValue,
                                              sDataValue->mLength );
                            
                        sCurInReadRow->mValue  = sDataValue->mValue;
                        sCurInReadRow->mLength = 1;
                    }
                        
                    sTempCurCol = sTempCurCol->mNext;
                    sLastCol    = sTempCurCol;
                            
                    /* 현재 column에 해당하는 filter 체크
                     * (filter 결과가 false이면 invalid 체크후 break */
                    while( sPhysicalFilter != NULL )
                    {
                        if( sPhysicalFilter->mColIdx1 <= sColOrder )
                        {
                            /* 위 sCurInReadRow의 셋팅에서 이미 physical filter가
                             * 가리키는 column value의 위치를 저장했다. */
                            if( sPhysicalFilter->mFunc( sPhysicalFilter->mColVal1->mValue,
                                                        sPhysicalFilter->mColVal1->mLength,
                                                        sPhysicalFilter->mColVal2->mValue,
                                                        sPhysicalFilter->mColVal2->mLength )
                                == STL_FALSE )
                            {
                                sHasValidVersion = STL_FALSE;
                                STL_THROW( RAMP_READ_NEXT_ROW );
                            }
                        }
                        else
                        {
                            break;
                        }
                        sPhysicalFilter = sPhysicalFilter->mNext;
                    }
                }

                /* 현재 row 에 포함된 column들을 value block 에 복사 */
                while( sCurCol != sLastCol )
                {
                    sCurInReadRow = KNL_GET_BLOCK_COLUMN_IN_ROW( sCurCol );
                    sDataValue    = KNL_GET_BLOCK_SEP_DATA_VALUE( sCurCol, sBlockIdx );

                    DTL_COPY_VALUE( sDataValue,
                                    sCurInReadRow->mValue,
                                    sCurInReadRow->mLength );
                    sDataValue->mLength = sCurInReadRow->mLength;

                    sCurCol = sCurCol->mNext;
                }
            }
            else
            {
                while( sCurCol != NULL )
                {
                    sColOrder = KNL_GET_BLOCK_COLUMN_ORDER(sCurCol);

                    sColPtr = smdmpfGetNthColValue( &sPageHandle,
                                                    sSlotIterator->mCurSeq,
                                                    sColOrder );

                    STL_DASSERT( sColPtr != NULL );

                    SMP_GET_COLUMN_LEN( sColPtr,
                                        &sColLenSize,
                                        &sColLen,
                                        sIsZero );

                    sColPtr += sColLenSize;
                    sDataValue = KNL_GET_BLOCK_SEP_DATA_VALUE( sCurCol, sBlockIdx );
                        
                    if( sIsZero == STL_TRUE )
                    {
                        DTL_NUMERIC_SET_ZERO( sDataValue->mValue, sDataValue->mLength );
                    }
                    else
                    {
                        sDataValue->mLength = sColLen;
                        DTL_COPY_VALUE( sDataValue, sColPtr, sColLen );
                    }
                    
                    sCurCol = sCurCol->mNext;
                }
            }
        }

        STL_RAMP( RAMP_READ_NEXT_ROW );

        if( sHasValidVersion == STL_FALSE )
        {
            continue;
        }
        
        /**
         * Trailing null
         */
        
        while( sCurCol != NULL )
        {
            while( sPhysicalFilter != NULL )
            {
                sPhysicalFilter->mColVal1->mLength = 0;
                            
                if( sPhysicalFilter->mColIdx1 <= KNL_GET_BLOCK_COLUMN_ORDER(sCurCol) )
                {
                    if( sPhysicalFilter->mFunc( sPhysicalFilter->mColVal1->mValue,
                                                sPhysicalFilter->mColVal1->mLength,
                                                sPhysicalFilter->mColVal2->mValue,
                                                sPhysicalFilter->mColVal2->mLength )
                        == STL_FALSE )
                    {
                        sHasValidVersion = STL_FALSE;
                        STL_THROW( RAMP_READ_NEXT_ROW );
                    }
                }
                else
                {
                    break;
                }

                sPhysicalFilter = sPhysicalFilter->mNext;
            }
                        
            sDataValue = KNL_GET_BLOCK_SEP_DATA_VALUE( sCurCol, sBlockIdx );
            sDataValue->mLength = 0;
            sCurCol = sCurCol->mNext;
        }
                    
        aIterator->mRowRid.mPageId = aIterator->mPageBufPid;
        aIterator->mRowRid.mOffset = sSlotSeq;
        
        /**
         * RowId Column을 참조하는 경우, RowId Column Block에 값을 만든다. 
         */
        if( aRowInfo->mRowIdColList != NULL )
        {
            STL_TRY( dtlRowIdSetValueFromRowIdInfo(
                         aRowInfo->mTableLogicalId,
                         aIterator->mRowRid.mTbsId,
                         aIterator->mRowRid.mPageId,
                         aIterator->mRowRid.mOffset,
                         KNL_GET_BLOCK_SEP_DATA_VALUE( aRowInfo->mRowIdColList,
                                                       sBlockIdx ),
                         KNL_ERROR_STACK(aEnv) )
                     == STL_SUCCESS );            
        }

        /**
         * Logical Filter 평가
         */
        if( aFetchInfo->mFilterEvalInfo != NULL )
        {
            aFetchInfo->mFilterEvalInfo->mBlockIdx = sBlockIdx;

            STL_TRY( knlEvaluateOneExpression( aFetchInfo->mFilterEvalInfo,
                                               KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            STL_DASSERT( KNL_GET_BLOCK_IS_SEP_BUFF(
                             aFetchInfo->mFilterEvalInfo->mResultBlock )
                         == STL_TRUE );

            if( !DTL_BOOLEAN_IS_TRUE( KNL_GET_BLOCK_SEP_DATA_VALUE(
                                          aFetchInfo->mFilterEvalInfo->mResultBlock,
                                          sBlockIdx ) ) )
            {
                continue;
            }
        }

        /**
         * evaluate ggregation
         */
        if( aFetchInfo->mAggrFetchInfo != NULL )
        {
            sBlockIdx++;
            
            if( sBlockIdx == sBlockSize )
            {
                STL_TRY( smoBlockBuildAggrFunc( aFetchInfo->mAggrFetchInfo,
                                                0, /* aStartBlockIdx */
                                                sBlockSize,
                                                aEnv )
                         == STL_SUCCESS );
                sBlockIdx = 0;
                sAggrRowCount = 1;
            }
        }
        else
        {
            SML_SET_RID_VALUE( aFetchInfo->mRowBlock, sBlockIdx, aIterator->mRowRid );
            SML_SET_SCN_VALUE( aFetchInfo->mRowBlock, sBlockIdx, SMI_GET_VIEW_SCN( aIterator ) );

            if( aFetchInfo->mSkipCnt > 0 )
            {
                (aFetchInfo->mSkipCnt)--;
                continue;
            }
            else
            {
                (aFetchInfo->mFetchCnt)--;
                sBlockIdx++;
            }
        
            if( (aFetchInfo->mFetchCnt == 0) ||
                (sBlockIdx == sBlockSize) )
            {
                *aEndOfSlot = STL_FALSE;
                break;
            }
        }
    }

    if( aFetchInfo->mAggrFetchInfo == NULL )
    {
        if( aRowInfo->mColList != NULL )
        {
            KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( aRowInfo->mColList, 0, sBlockIdx );
        }

        if( aRowInfo->mRowIdColList != NULL )
        {
            KNL_SET_ONE_BLOCK_SKIP_CNT( aRowInfo->mRowIdColList, 0 );
            KNL_SET_ONE_BLOCK_USED_CNT( aRowInfo->mRowIdColList, sBlockIdx );
        }
    
        SML_SET_USED_BLOCK_SIZE( aFetchInfo->mRowBlock, sBlockIdx );
    }
    else
    {
        if( sBlockIdx > 0 )
        {
            STL_TRY( smoBlockBuildAggrFunc( aFetchInfo->mAggrFetchInfo,
                                            0, /* aStartBlockIdx */
                                            sBlockIdx,
                                            aEnv )
                     == STL_SUCCESS );
            sAggrRowCount = 1;
        }
        
        SML_SET_USED_BLOCK_SIZE( aFetchInfo->mRowBlock, sAggrRowCount );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smdmpfMakePrevHeader( void   * aUndoRecHeader,
                                void   * aOrgRecHeader,
                                smlEnv * aEnv )
{
    stlMemcpy( aOrgRecHeader, aUndoRecHeader, SMDMPF_ROW_HDR_LEN );

    return STL_SUCCESS;
}
                                

/**
 * @brief 일반 Scan을 위해 열린 Iterator에서 다음 row를 반환한다
 * @param[in]       aIterator    이전에 읽은 위치를 가진 iterator
 * @param[in,out]   aFetchInfo   Fetch 정보
 * @param[in]       aEnv         Storage Manager Environment
 */
stlStatus smdmpfFetchNext( void          * aIterator,
                           smlFetchInfo  * aFetchInfo,
                           smlEnv        * aEnv )
{
    smdmpfIterator       * sIterator = (smdmpfIterator*)aIterator;
    smpHandle              sPageHandle;
    smlPid                 sPid = sIterator->mPageBufPid;
    stlInt16               sCurSlotSeq = sIterator->mRowRid.mOffset;
    stlInt32               sState = 0;
    smpHandle              sDummyHandle;
    void                 * sSegHandle = SME_GET_SEGMENT_HANDLE(sIterator->mCommon.mRelationHandle);
    smdRowInfo             sRowInfo;
    stlBool                sEndOfSlot;

    STL_PARAM_VALIDATE( aFetchInfo->mRowBlock != NULL, KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aFetchInfo->mAggrFetchInfo == NULL, KNL_ERROR_STACK(aEnv) );
    
    SML_RESET_USED_BLOCK( aFetchInfo->mRowBlock );
    
    aFetchInfo->mIsEndOfScan = STL_TRUE;

    sDummyHandle.mFrame = sIterator->mPageBuf;
    sDummyHandle.mPch = NULL;

    sRowInfo.mPageHandle = &sDummyHandle;
    sRowInfo.mTbsId = sIterator->mRowRid.mTbsId;
    sRowInfo.mTransId = SMA_GET_TRANS_ID(sIterator->mCommon.mStatement);
    sRowInfo.mViewScn = sIterator->mCommon.mViewScn;
    sRowInfo.mCommitScn = SMA_GET_COMMIT_SCN(sIterator->mCommon.mStatement);
    sRowInfo.mTcn = sIterator->mCommon.mViewTcn;
    sRowInfo.mColList = aFetchInfo->mColList;
    sRowInfo.mRowIdColList = aFetchInfo->mRowIdColList;
    sRowInfo.mTableLogicalId = aFetchInfo->mTableLogicalId;    

    if( sPid == SMP_NULL_PID )
    {
        /* 아직 pagebuf에 page가 적재되지 않았음 */
        STL_TRY( smsGetFirstPage( sSegHandle,
                                  sIterator->mSegIterator,
                                  &sPid,
                                  aEnv ) == STL_SUCCESS );

        while( STL_TRUE )
        {
            if( sPid == SMP_NULL_PID )
            {
                /**
                 * Value Block 에 채워진 Data 가 없음을 설정
                 */
                if( aFetchInfo->mColList != NULL )
                {
                    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( aFetchInfo->mColList, 0, 0 );
                }

                if( aFetchInfo->mRowIdColList != NULL )
                {
                    KNL_SET_ONE_BLOCK_SKIP_CNT( aFetchInfo->mRowIdColList, 0 );
                    KNL_SET_ONE_BLOCK_USED_CNT( aFetchInfo->mRowIdColList, 0 );
                }

                break;
            }

            STL_TRY( smpFix( sIterator->mRowRid.mTbsId,
                             sPid,
                             &sPageHandle,
                             aEnv ) == STL_SUCCESS );
            sState = 1;

            if( SMDMPF_GET_MEMBER_PAGE_TYPE(SMP_FRAME(&sPageHandle)) == SMDMPF_MEMBER_PAGE_TYPE_MASTER )
            {
                STL_TRY( smpSnapshot( &sPageHandle,
                                      (stlChar*)sIterator->mPageBuf,
                                      aEnv )
                         == STL_SUCCESS );
            
                sState = 0;
                STL_TRY( smpUnfix( &sPageHandle, aEnv ) == STL_SUCCESS );
                sIterator->mPageBufPid = sPid;
                sCurSlotSeq = -1;
                break;
            }

            sState = 0;
            STL_TRY( smpUnfix( &sPageHandle, aEnv ) == STL_SUCCESS );

            /**
             * Master Page가 아니면 skip
             */
            STL_TRY( smsGetNextPage( SME_GET_SEGMENT_HANDLE(sIterator->mCommon.mRelationHandle),
                                     sIterator->mSegIterator,
                                     &sPid,
                                     aEnv ) == STL_SUCCESS );
        }
    }

    while( sPid != SMP_NULL_PID ) /* 이전에 cache한 페이지가 있으면 */
    {
        if( SMXL_IS_AGABLE_VIEW_SCN(sIterator->mAgableViewScn, smpGetMaxViewScn(&sDummyHandle)) &&
            smdmpfIsSimplePage(&sDummyHandle) )
        {
            STL_TRY( smdmpfGetValidVersionsSimplePage( sIterator,
                                                       (stlChar*)sIterator->mPageBuf,
                                                       &sRowInfo,
                                                       aFetchInfo,
                                                       sCurSlotSeq,
                                                       &sEndOfSlot,
                                                       aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            STL_TRY( smdmpfGetValidVersions( sIterator,
                                             (stlChar*)sIterator->mPageBuf,
                                             &sRowInfo,
                                             aFetchInfo,
                                             sCurSlotSeq,
                                             &sEndOfSlot,
                                             aEnv )
                     == STL_SUCCESS );
        }

        if( aFetchInfo->mFetchCnt == 0 )
        {
            aFetchInfo->mIsEndOfScan = STL_FALSE;
            break;
        }
        
        if( sEndOfSlot == STL_TRUE )
        {
            while( STL_TRUE )
            {
                /* 현재 페이지에서 적당한 row를 찾지 못했기 때문에 다음 페이지로 이동함 */
                STL_TRY( smsGetNextPage( SME_GET_SEGMENT_HANDLE(sIterator->mCommon.mRelationHandle),
                                         sIterator->mSegIterator,
                                         &sPid,
                                         aEnv ) == STL_SUCCESS );

                if( sPid != SMP_NULL_PID )
                {
                    /* 다음 페이지가 존재하므로 caching함 */
                    STL_TRY( smpFix( smsGetTbsId(SME_GET_SEGMENT_HANDLE(sIterator->mCommon.mRelationHandle)),
                                     sPid,
                                     &sPageHandle,
                                     aEnv ) == STL_SUCCESS );
                    sState = 1;

                    /**
                     * Master Page가 아니면 skip
                     */
                    if( SMDMPF_GET_MEMBER_PAGE_TYPE(SMP_FRAME(&sPageHandle)) != SMDMPF_MEMBER_PAGE_TYPE_MASTER )
                    {
                        sState = 0;
                        STL_TRY( smpUnfix( &sPageHandle, aEnv ) == STL_SUCCESS );
                        continue;
                    }

                    STL_TRY( smpSnapshot( &sPageHandle,
                                          (stlChar*)sIterator->mPageBuf,
                                          aEnv )
                             == STL_SUCCESS );
            
                    sState = 0;
                    STL_TRY( smpUnfix( &sPageHandle, aEnv ) == STL_SUCCESS );
                    sIterator->mPageBufPid = sPid;
                    sIterator->mRowRid.mOffset = -1;
                    sCurSlotSeq = -1;
                    break;
                }
                else
                {
                    /* 다음 페이지가 없으므로 EOF를 리턴 */
                    sIterator->mPageBufPid = sPid;
                    sIterator->mRowRid.mTbsId = 0;
                    sIterator->mRowRid.mPageId = SMP_NULL_PID;
                    sIterator->mRowRid.mOffset = -1;
                    aFetchInfo->mIsEndOfScan = STL_TRUE;
                    STL_THROW( RAMP_END );
                }
            }
        }

        if( SML_USED_BLOCK_SIZE( aFetchInfo->mRowBlock ) == SML_BLOCK_SIZE( aFetchInfo->mRowBlock ) )
        {
            aFetchInfo->mIsEndOfScan = STL_FALSE;
            break;
        }
    } /* while( sPid != SMP_NULL_PID ) */

    STL_RAMP( RAMP_END );

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smpUnfix( &sPageHandle, aEnv );
    }

    return STL_FAILURE;
}

/**
 * @brief 일반 Scan을 위해 열린 Iterator에서 이전 row를 반환한다
 * @param[in]       aIterator    이전에 읽은 위치를 가진 iterator
 * @param[in,out]   aFetchInfo   Fetch 정보
 * @param[in]       aEnv         Storage Manager Environment
 */
stlStatus smdmpfFetchPrev( void          * aIterator,
                           smlFetchInfo  * aFetchInfo,
                           smlEnv        * aEnv )
{
    return STL_SUCCESS;
}


/**
 * @brief Aggregation을 위한 Scan을 한다.
 * @param[in]       aIterator    이전에 읽은 위치를 가진 iterator
 * @param[in,out]   aFetchInfo   Fetch 정보
 * @param[in]       aEnv         Storage Manager Environment
 */
stlStatus smdmpfFetchAggr( void          * aIterator,
                           smlFetchInfo  * aFetchInfo,
                           smlEnv        * aEnv )
{
    smdmpfIterator       * sIterator = (smdmpfIterator*)aIterator;
    smpHandle              sPageHandle;
    stlInt32               sState = 0;
    void                 * sSegHandle = SME_GET_SEGMENT_HANDLE(sIterator->mCommon.mRelationHandle);
    smdRowInfo             sRowInfo;
    stlBool                sEndOfSlot;
    smlPid                 sPid;

    STL_PARAM_VALIDATE( aFetchInfo->mAggrFetchInfo != NULL, KNL_ERROR_STACK(aEnv) );
    
    SML_RESET_USED_BLOCK( aFetchInfo->mRowBlock );
    
    aFetchInfo->mIsEndOfScan = STL_TRUE;

    sRowInfo.mPageHandle = &sPageHandle;
    sRowInfo.mTbsId = sIterator->mRowRid.mTbsId;
    sRowInfo.mTransId = SMA_GET_TRANS_ID(sIterator->mCommon.mStatement);
    sRowInfo.mViewScn = sIterator->mCommon.mViewScn;
    sRowInfo.mCommitScn = SMA_GET_COMMIT_SCN(sIterator->mCommon.mStatement);
    sRowInfo.mTcn = sIterator->mCommon.mViewTcn;
    sRowInfo.mColList = aFetchInfo->mColList;
    sRowInfo.mRowIdColList = aFetchInfo->mRowIdColList;
    sRowInfo.mTableLogicalId = aFetchInfo->mTableLogicalId;
        
    /* 아직 pagebuf에 page가 적재되지 않았음 */
    STL_TRY( smsGetFirstPage( sSegHandle,
                              sIterator->mSegIterator,
                              &sPid,
                              aEnv ) == STL_SUCCESS );

    while( sPid != SMP_NULL_PID )
    {
        /* Segment에 단 한페이지도 할당되어 있지 않음 */
        STL_TRY( smpFix( sIterator->mRowRid.mTbsId,
                         sPid,
                         &sPageHandle,
                         aEnv ) == STL_SUCCESS );
        sState = 1;

        /**
         * Master Page가 아니면 skip
         */
        if( SMDMPF_GET_MEMBER_PAGE_TYPE(SMP_FRAME(&sPageHandle)) == SMDMPF_MEMBER_PAGE_TYPE_MASTER )
        {
            sState = 0;
            STL_TRY( smpUnfix( &sPageHandle, aEnv ) == STL_SUCCESS );

            sIterator->mPageBufPid = sPid;
            break;
        }
            
        sState = 0;
        STL_TRY( smpUnfix( &sPageHandle, aEnv ) == STL_SUCCESS );

        STL_TRY( smsGetNextPage( SME_GET_SEGMENT_HANDLE(sIterator->mCommon.mRelationHandle),
                                 sIterator->mSegIterator,
                                 &sPid,
                                 aEnv ) == STL_SUCCESS );
    }

    while( sPid != SMP_NULL_PID )
    {
        STL_TRY( smpAcquire( NULL,
                             sIterator->mRowRid.mTbsId,
                             sPid,
                             KNL_LATCH_MODE_SHARED,
                             &sPageHandle,
                             aEnv ) == STL_SUCCESS );
        sState = 1;
            
        /* Segment에 단 한페이지도 할당되어 있지 않음 */
        if( SMXL_IS_AGABLE_VIEW_SCN(sIterator->mAgableViewScn, smpGetMaxViewScn(&sPageHandle)) &&
            smdmpfIsSimplePage(&sPageHandle) )
        {
            STL_TRY( smdmpfGetValidVersionsSimplePage( sIterator,
                                                       SMP_FRAME( &sPageHandle ),
                                                       &sRowInfo,
                                                       aFetchInfo,
                                                       -1,  /* aFirstSlotSeq */
                                                       &sEndOfSlot,
                                                       aEnv )
                     == STL_SUCCESS );

            sState = 0;
            STL_TRY( smpRelease( &sPageHandle, aEnv ) == STL_SUCCESS );
        }
        else
        {
            stlMemcpy( sIterator->mPageBuf, SMP_FRAME( &sPageHandle ), SMP_PAGE_SIZE );
            
            sState = 0;
            STL_TRY( smpRelease( &sPageHandle, aEnv ) == STL_SUCCESS );

            SMP_MAKE_DUMMY_HANDLE( &sPageHandle, sIterator->mPageBuf );

            STL_TRY( smdmpfGetValidVersions( sIterator,
                                             (stlChar*)sIterator->mPageBuf,
                                             &sRowInfo,
                                             aFetchInfo,
                                             -1,  /* aFirstSlotSeq */
                                             &sEndOfSlot,
                                             aEnv )
                     == STL_SUCCESS );
        }

        STL_DASSERT( sEndOfSlot == STL_TRUE );

        while( sPid != SMP_NULL_PID )
        {
            /* 현재 페이지에서 적당한 row를 찾지 못했기 때문에 다음 페이지로 이동함 */
            STL_TRY( smsGetNextPage( SME_GET_SEGMENT_HANDLE(sIterator->mCommon.mRelationHandle),
                                     sIterator->mSegIterator,
                                     &sPid,
                                     aEnv ) == STL_SUCCESS );

            /* 다음 페이지가 존재하므로 caching함 */
            STL_TRY( smpFix( smsGetTbsId(SME_GET_SEGMENT_HANDLE(sIterator->mCommon.mRelationHandle)),
                             sPid,
                             &sPageHandle,
                             aEnv ) == STL_SUCCESS );
            sState = 1;

            /**
             * Master Page가 아니면 skip
             */
            if( SMDMPF_GET_MEMBER_PAGE_TYPE(SMP_FRAME(&sPageHandle)) == SMDMPF_MEMBER_PAGE_TYPE_MASTER )
            {
                sState = 0;
                STL_TRY( smpUnfix( &sPageHandle, aEnv ) == STL_SUCCESS );

                sIterator->mPageBufPid = sPid;
                sIterator->mRowRid.mOffset = -1;
                break;
            }

            sState = 0;
            STL_TRY( smpUnfix( &sPageHandle, aEnv ) == STL_SUCCESS );
        }
    }
            
    
    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smpRelease( &sPageHandle, aEnv );
    }

    return STL_FAILURE;
}

inline stlChar * smdmpfGetNthColValue( smpHandle * aPageHandle,
                                       stlInt16    aRowSeq,
                                       stlInt16    aColOrd )
{
    stlUInt8             sColLenSize;
    stlChar            * sColPtr;
    stlInt16             sColOrd;
    stlInt16             sStartPos;
    stlInt16             i;
    stlUInt16          * sColLengthArray = (stlUInt16 *)SMP_GET_COLUMN_LENGTH( aPageHandle );
    stlInt64             sColLen;
    smdmpfLogicalHdr   * sLogicalHdr = SMDMPF_GET_LOGICAL_HEADER( SMP_FRAME(aPageHandle) );
    stlInt16             sRowCount = sLogicalHdr->mRowCount;

    sColPtr = NULL;

    STL_TRY_THROW( aColOrd >= sLogicalHdr->mFromOrd, RAMP_SKIP );
    STL_TRY_THROW( aColOrd <= sLogicalHdr->mToOrd, RAMP_SKIP );

    sColOrd = aColOrd - sLogicalHdr->mFromOrd;
    sStartPos = SMDMPF_ROW_HDR_LEN * sRowCount;

    for( i = 0; i < sColOrd; i++ )
    {
        sColLen = sColLengthArray[i];

        if( SMP_IS_FIXED_SLOT_CONTINUOUS_COLUMN( sColLen ) == STL_TRUE )
        {
            sColLen = SMP_GET_FIXED_SLOT_CONTINUOUS_COLUMN_LENGTH( sColLen );
            SMP_GET_CONT_COLUMN_LEN_SIZE( &sColLenSize );
        }
        else
        {
            SMP_GET_COLUMN_LEN_SIZE( sColLen, &sColLenSize );
        }

        sStartPos += sRowCount * (sColLen + sColLenSize);
    }

    sColLen = sColLengthArray[sColOrd];

    if( SMP_IS_FIXED_SLOT_CONTINUOUS_COLUMN( sColLen ) == STL_TRUE )
    {
        sColLen = SMP_GET_FIXED_SLOT_CONTINUOUS_COLUMN_LENGTH( sColLen );
        SMP_GET_CONT_COLUMN_LEN_SIZE( &sColLenSize );
    }
    else
    {
        SMP_GET_COLUMN_LEN_SIZE( sColLen, &sColLenSize );
    }

    sColPtr = SMDMPF_GET_PAGE_BODY(SMP_FRAME(aPageHandle)) + sStartPos + (sColLen + sColLenSize) * aRowSeq;

    STL_RAMP( RAMP_SKIP );

    return sColPtr;
}

stlStatus smdmpfReadContCols( smlTbsId             aTbsId,
                              knlValueBlockList  * aCurCol,
                              stlInt32             aBlockIdx,
                              stlChar            * aColPtr,
                              smlEnv             * aEnv )
{
    smlPid           sContColPid;
    dtlDataValue   * sDataValue;
    stlChar        * sValuePtr;
    stlInt16         sSubColLen;
    stlInt64         sTotalColLen = 0;
    stlInt32         sState = 0;
    stlChar        * sRowHdr;
    stlChar        * sContColPtr;
    smpHandle        sPageHandle;
    stlInt64         sAllocSize;

    sDataValue = KNL_GET_BLOCK_SEP_DATA_VALUE( aCurCol, aBlockIdx );

    SMP_READ_CONT_COLUMN_PID( aColPtr, &sContColPid );
    SMP_GET_CONT_COLUMN_VALUE_PTR( aColPtr, sValuePtr );
    SMP_GET_CONT_COLUMN_LEN( aColPtr, &sSubColLen );

    sTotalColLen += sSubColLen;

    stlMemcpy( (stlChar*)sDataValue->mValue,
               sValuePtr,
               sSubColLen );

    sDataValue->mLength = sTotalColLen;

    while( sContColPid != SMP_NULL_PID )
    {
        STL_TRY( smpAcquire( NULL,
                             aTbsId,
                             sContColPid,
                             KNL_LATCH_MODE_SHARED,
                             &sPageHandle,
                             aEnv ) == STL_SUCCESS );
        sState = 1;

        sRowHdr = (stlChar *)SMDMPF_GET_START_POS( SMP_FRAME(&sPageHandle) );
        sContColPtr = sRowHdr + SMDMPF_ROW_HDR_LEN;

        SMP_READ_CONT_COLUMN_PID( sContColPtr, &sContColPid );
        SMP_GET_CONT_COLUMN_VALUE_PTR( sContColPtr, sValuePtr );
        SMP_GET_CONT_COLUMN_LEN( sContColPtr, &sSubColLen );

        /**
         * Long Varying Type에 대해서 Memory 확장
         */
        if( sDataValue->mBufferSize < (sTotalColLen + sSubColLen) )
        {
            STL_DASSERT( (sDataValue->mType == DTL_TYPE_LONGVARCHAR) ||
                         (sDataValue->mType == DTL_TYPE_LONGVARBINARY) );
                            
            if( sDataValue->mType == DTL_TYPE_LONGVARCHAR )
            {
                sAllocSize = STL_MIN( (sDataValue->mBufferSize + (sTotalColLen + sSubColLen)) * 2,
                                      DTL_LONGVARCHAR_MAX_PRECISION );
            }
            else
            {
                sAllocSize = STL_MIN( (sDataValue->mBufferSize + (sTotalColLen + sSubColLen)) * 2,
                                      DTL_LONGVARBINARY_MAX_PRECISION );
            }
                            
            STL_TRY( knlReallocAndMoveLongVaryingMem( sDataValue,
                                                      sAllocSize,
                                                      KNL_ENV(aEnv) )
                     == STL_SUCCESS );
        }

        stlMemcpy( (stlChar*)sDataValue->mValue + sTotalColLen,
                   sValuePtr,
                   sSubColLen );

        sTotalColLen += sSubColLen;
        sDataValue->mLength = sTotalColLen;

        sState = 0;
        STL_TRY( smpRelease( &sPageHandle, aEnv ) == STL_SUCCESS );
    }

    sDataValue->mLength = sTotalColLen;

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smpRelease( &sPageHandle, aEnv );
    }

    return STL_FAILURE;
}
                                       
/** @} */
