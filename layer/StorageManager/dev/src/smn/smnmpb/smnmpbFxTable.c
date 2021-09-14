/*******************************************************************************
 * smnmpbFxTable.c
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

#include <stl.h>
#include <dtl.h>
#include <knl.h>
#include <scl.h>
#include <smlDef.h>
#include <smDef.h>
#include <smt.h>
#include <smxm.h>
#include <smxl.h>
#include <smp.h>
#include <smg.h>
#include <smp.h>
#include <smn.h>
#include <sms.h>
#include <sme.h>
#include <smnmpb.h>


/**
 * @file smnmpbFxTable.c
 * @brief Storage Manager Layer Memory B-Tree Fixed Table Internal Routines
 */

/**
 * @addtogroup smnmpbFxTable
 * @{
 */


knlFxColumnDesc gSmnmpbKeyColumnDesc[] =
{
    {
        "CURRENT_PAGE_ID",
        "page identifier",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smnmpbFxKey, mCurPageId ),
        STL_SIZEOF( smlPid ),
        STL_FALSE  /* nullable */
    },
    {
        "LEVEL",
        "level",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smnmpbFxKey, mLevel ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "PARENT_PAGE_ID",
        "page identifier",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smnmpbFxKey, mParentPageId ),
        STL_SIZEOF( smlPid ),
        STL_FALSE  /* nullable */
    },
    {
        "KEY_SEQUENCE",
        "sequence",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smnmpbFxKey, mSeq ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "KEY_STATUS_FLAGS",
        "key status flag",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smnmpbFxKey, mKeyStatusFlags ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "CHILD_PAGE_ID",
        "page identifier",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smnmpbFxKey, mChildPageId ),
        STL_SIZEOF( smlPid ),
        STL_FALSE  /* nullable */
    },
    {
        "ROW_PAGE_ID",
        "page identifier",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smnmpbFxKey, mRowPageId ),
        STL_SIZEOF( smlPid ),
        STL_FALSE  /* nullable */
    },
    {
        "ROW_OFFSET",
        "offset",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smnmpbFxKey, mRowOffset ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "INSERT_RTS_SEQ",
        "insert tx rts sequence",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smnmpbFxKey, mInsertRtsSeq ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "INSERT_RTS_VER_NO",
        "insert tx rts version number",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smnmpbFxKey, mInsertRtsVerNo ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "INSERT_TCN",
        "insert tx change number",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smnmpbFxKey, mInsertTcn ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "INSERT_TRANS_ID",
        "insert trans id",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smnmpbFxKey, mInsertTransId ),
        STL_SIZEOF( smxlTransId ),
        STL_FALSE  /* nullable */
    },
    {
        "INSERT_SCN",
        "insert view/commit Scn",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smnmpbFxKey, mInsertScn ),
        STL_SIZEOF( smlScn ),
        STL_FALSE  /* nullable */
    },
    {
        "DELETE_RTS_SEQ",
        "sequence",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smnmpbFxKey, mDeleteRtsSeq ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "DELETE_RTS_VER_NO",
        "delete tx rts version number",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smnmpbFxKey, mDeleteRtsVerNo ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "DELETE_TCN",
        "delete tx change number",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smnmpbFxKey, mDeleteTcn ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "DELETE_TRANS_ID",
        "delete trans id",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smnmpbFxKey, mInsertTransId ),
        STL_SIZEOF( smxlTransId ),
        STL_FALSE  /* nullable */
    },
    {
        "DELETE_SCN",
        "delete view/commit Scn",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smnmpbFxKey, mDeleteScn ),
        STL_SIZEOF( smlScn ),
        STL_FALSE  /* nullable */
    },
    {
        "KEY_COLUMN",
        "column",
        KNL_FXTABLE_DATATYPE_LONGVARCHAR,
        STL_OFFSETOF( smnmpbFxKey, mColumn ),
        SMP_PAGE_SIZE ,
        STL_TRUE  /* nullable */
    },
    {
        "KEY_COLUMN_SEQ",
        "column sequence",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smnmpbFxKey, mColumnSeq ),
        STL_SIZEOF( stlInt16 ) ,
        STL_FALSE  /* nullable */
    },
    {
        NULL,
        NULL,
        0,
        0,
        0,
        STL_FALSE  /* nullable */
    }
};

stlStatus smnmpbOpenKeyCallback( void   * aStmt,
                                 void   * aDumpObjHandle,
                                 void   * aPathCtrl,
                                 knlEnv * aEnv )
{
    smnmpbFxKeyIterator * sIterator;
    smpHandle             sPageHandle;
    stlInt16              sState = 0;
    stlInt32              i;

    STL_TRY( smsValidateSegment( SME_GET_SEGMENT_HANDLE(aDumpObjHandle),
                                 SML_ENV(aEnv) )
             == STL_SUCCESS );
    
    sIterator = (smnmpbFxKeyIterator*)aPathCtrl;
    sIterator->mTbsId = smsGetTbsId(SME_GET_SEGMENT_HANDLE(aDumpObjHandle));

    sIterator->mPathStack.mDepth = 0;
    sIterator->mPathStack.mStack[sIterator->mPathStack.mDepth].mPid = 
        SMN_GET_ROOT_PAGE_ID(aDumpObjHandle);
    sIterator->mPathStack.mStack[sIterator->mPathStack.mDepth].mSeq = -1;

    for( i = 0; i < SMNMPB_MAX_INDEX_DEPTH; i++ )
    {
        sIterator->mPrinted[i] = STL_FALSE;
    }

    STL_TRY( smpAcquire( NULL,
                         sIterator->mTbsId,
                         sIterator->mPathStack.mStack[sIterator->mPathStack.mDepth].mPid,
                         KNL_LATCH_MODE_SHARED,
                         &sPageHandle,
                         (smlEnv*)aEnv ) == STL_SUCCESS );
    sState = 1;

    stlMemcpy( sIterator->mPageBuf, SMP_FRAME(&sPageHandle), SMP_PAGE_SIZE );
    sIterator->mLevel = SMNMPB_GET_LEVEL(&sPageHandle);
    sIterator->mTotalKeyCount = SMP_GET_PAGE_ROW_ITEM_COUNT(&sPageHandle);
    sIterator->mTotalColumnCount = SMNMPB_GET_KEY_COLUMN_COUNT(&sPageHandle);
    sIterator->mNextColumnIdx = 0;

    sState = 0;
    STL_TRY( smpRelease( &sPageHandle, (smlEnv*)aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        smpRelease( &sPageHandle, (smlEnv*)aEnv );
    }

    return STL_FAILURE;
}

stlStatus smnmpbCloseKeyCallback( void   * aDumpObjHandle,
                                  void   * aPathCtrl,
                                  knlEnv * aEnv )
{
    return STL_SUCCESS;
}

stlStatus smnmpbGetDumpObjectCallback( stlInt64   aTransID,
                                       void     * aStmt,
                                       stlChar  * aDumpObjName,
                                       void    ** aDumpObjHandle,
                                       knlEnv   * aEnv )
{
    stlChar  * sToken1;
    stlChar  * sToken2;
    stlChar  * sToken3;
    stlInt32   sTokenCount;
    stlBool    sExist;

    *aDumpObjHandle = NULL;

    STL_TRY( knlTokenizeNextDumpOption( &aDumpObjName,
                                        &sToken1,
                                        &sToken2,
                                        &sToken3,
                                        &sTokenCount,
                                        aEnv ) == STL_SUCCESS );

    if( sTokenCount == 1 )
    {
        STL_TRY( gKnlDumpNameHelper.mGetIndexHandle( aTransID,
                                                     ((smlStatement*) aStmt)->mScn,
                                                     NULL,
                                                     sToken1,
                                                     NULL,
                                                     aDumpObjHandle,
                                                     &sExist,
                                                     aEnv ) == STL_SUCCESS );
    }
    else if( sTokenCount == 2 )
    {
        STL_TRY( gKnlDumpNameHelper.mGetIndexHandle( aTransID,
                                                     ((smlStatement*) aStmt)->mScn,
                                                     sToken1,
                                                     sToken2,
                                                     NULL,
                                                     aDumpObjHandle,
                                                     &sExist,
                                                     aEnv ) == STL_SUCCESS );
    }
    else
    {
        STL_THROW( RAMP_FINISH );
    }

    STL_TRY_THROW( sExist == STL_TRUE, RAMP_FINISH );

    STL_RAMP( RAMP_FINISH );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus  smnmpbMakeKeyString( stlChar   * aColPtr,
                                stlUInt16   aColIdx,
                                stlUInt16   aColCount,
                                stlUInt8  * aColTypes,
                                stlChar   * aColStrBuf,
                                knlEnv    * aEnv )
{
    stlChar      * sPtr = aColStrBuf;
    stlInt16       i;
    dtlDataValue   sColDesc;
    stlUInt8       sColLenSize;
    stlInt64       sColLen;
    stlInt64       sStrLen;
    knlRegionMark  sRegionMark;
    stlInt32       sState = 0;

    KNL_INIT_REGION_MARK( &sRegionMark );
    
    if( aColPtr == NULL )
    {
        stlSnprintf( aColStrBuf, SMP_PAGE_SIZE, "infinite");
    }
    else
    {
        for( i = 0; i < aColCount; i++ )
        {
            SMP_GET_COLUMN_LEN_ZERO_INSENS(aColPtr, &sColLenSize, &sColLen);

            if( i == aColIdx )
            {
                STL_TRY( knlMarkRegionMem( &(SML_ENV(aEnv))->mOperationHeapMem,
                                           &sRegionMark,
                                           aEnv )
                         == STL_SUCCESS );
                sState = 1;
    
                sColDesc.mType = aColTypes[i];
                sColDesc.mLength = sColLen;
                sColDesc.mBufferSize = sColLen;

                if( sColLen == 0 )
                {
                    stlSnprintf( aColStrBuf, SMP_PAGE_SIZE, "null ");
                }
                else
                {
                    STL_TRY( knlAllocRegionMem( &(SML_ENV(aEnv))->mOperationHeapMem,
                                                sColDesc.mLength,
                                                &sColDesc.mValue,
                                                aEnv )
                             == STL_SUCCESS );

                    stlMemcpy( sColDesc.mValue, aColPtr + sColLenSize, sColDesc.mLength );
                    
                    STL_TRY( dtlToStringDataValue( &sColDesc,
                                                   DTL_PRECISION_NA,
                                                   DTL_SCALE_NA,
                                                   SMP_PAGE_SIZE,
                                                   sPtr,
                                                   &sStrLen,
                                                   KNL_DT_VECTOR(aEnv),
                                                   aEnv,
                                                   KNL_ERROR_STACK(aEnv) )
                             == STL_SUCCESS );
                
                    sPtr[sStrLen] = '\0';
                }

                sState = 0;
                STL_TRY( knlFreeUnmarkedRegionMem( &(SML_ENV(aEnv))->mOperationHeapMem,
                                                   &sRegionMark,
                                                   STL_FALSE, /* aFreeChunk */
                                                   aEnv )
                         == STL_SUCCESS );
                
                break;
            }
            
            aColPtr += sColLenSize + sColLen;
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void) knlFreeUnmarkedRegionMem( &(SML_ENV(aEnv))->mOperationHeapMem,
                                         &sRegionMark,
                                         STL_FALSE, /* aFreeChunk */
                                         aEnv );
    }
    
    return STL_FAILURE;
}


stlStatus smnmpbBuildRecordKeyCallback( void              * aDumpObjHandle,
                                        void              * aPathCtrl,
                                        knlValueBlockList * aValueList,
                                        stlInt32            aBlockIdx,
                                        stlBool           * aTupleExist,
                                        knlEnv            * aEnv )
{
    smnmpbFxKeyIterator * sIterator;
    smnmpbFxKey         * sFxKey;
    smpHandle             sPageHandle;
    smlPid                sPageId;
    stlInt16              sState = 0;
    stlChar             * sKeySlot;
    stlChar             * sColPtr = NULL;
    stlUInt8              sTmpRtsSeq;
    stlUInt8              sTmpRtsVerNo;
    stlUInt8              sTcn;

    sIterator = (smnmpbFxKeyIterator*)aPathCtrl;
    sFxKey = &sIterator->mKeyRecord;

    if( sIterator->mNextColumnIdx > 0 )
    {
        SMP_MAKE_DUMMY_HANDLE( &sPageHandle, sIterator->mPageBuf );
        sKeySlot = smpGetNthRowItem( &sPageHandle,
                                     sIterator->mPathStack.mStack[sIterator->mPathStack.mDepth].mSeq );
        
        if( sIterator->mLevel == 0 )
        {
            /* Leaf Node */
            if( SMNMPB_IS_TBK( sKeySlot ) != STL_TRUE )
            {
                sColPtr = sKeySlot + SMNMPB_RBK_HDR_SIZE;
            }
            else
            {
                sColPtr = sKeySlot + SMNMPB_TBK_HDR_SIZE;
            }
        }
        else
        {
            /* Internal Node */
            if( (sIterator->mPathStack.mStack[sIterator->mPathStack.mDepth].mSeq 
                 == sIterator->mTotalKeyCount - 1) &&
                (SMNMPB_IS_LAST_SIBLING(&sPageHandle) == STL_TRUE) )
            {
                sColPtr = NULL;
            }
            else
            {
                sColPtr = sKeySlot + SMNMPB_INTL_SLOT_HDR_SIZE;
            }
        }
        
        STL_THROW( RAMP_SKIP_FETCH );
    }

    while( STL_TRUE )
    {
        STL_ASSERT( sIterator->mPathStack.mDepth < SMNMPB_MAX_INDEX_DEPTH );
        if( sIterator->mPathStack.mStack[sIterator->mPathStack.mDepth].mSeq + 1 >=
            sIterator->mTotalKeyCount )
        {
            if( sIterator->mPrinted[sIterator->mPathStack.mDepth] == STL_FALSE )
            {
                /* 현재 노드 출력 완료 */
                sIterator->mPrinted[sIterator->mPathStack.mDepth] = STL_TRUE;
                sIterator->mPathStack.mStack[sIterator->mPathStack.mDepth].mSeq = 0;
                if( sIterator->mLevel > 0 )
                {
                    /* internal node 이면 아래로 */
                    STL_TRY( smpAcquire( NULL,
                                         sIterator->mTbsId,
                                         sIterator->mPathStack.mStack[sIterator->mPathStack.mDepth].mPid,
                                         KNL_LATCH_MODE_SHARED,
                                         &sPageHandle,
                                         (smlEnv*)aEnv ) == STL_SUCCESS );
                    sState = 1;
                    if( smpGetFreeness( &sPageHandle ) == SMP_FREENESS_FREE )
                    {
                        sState = 0;
                        STL_TRY( smpRelease( &sPageHandle, (smlEnv*)aEnv ) == STL_SUCCESS );
                        *aTupleExist = STL_FALSE;
                        STL_THROW( ERR_MUTATING_OBJECT );
                    }
                    sKeySlot = smpGetNthRowItem( &sPageHandle, 0 );
                    SMNMPB_INTL_SLOT_GET_CHILD_PID( sKeySlot, &sPageId );
                    sState = 0;
                    STL_TRY( smpRelease( &sPageHandle, (smlEnv*)aEnv ) == STL_SUCCESS );
                    sIterator->mPathStack.mStack[sIterator->mPathStack.mDepth + 1].mPid = sPageId;
                    sIterator->mPathStack.mStack[sIterator->mPathStack.mDepth + 1].mSeq = -1;
                    sIterator->mPrinted[sIterator->mPathStack.mDepth + 1] = STL_FALSE;
                    sIterator->mPathStack.mDepth++;
                }
                else
                {
                    /* leaf node 이면 위로 */
                    sIterator->mPathStack.mDepth--;
                    if( sIterator->mPathStack.mDepth == -1 )
                    {
                        *aTupleExist = STL_FALSE;
                        STL_THROW( RAMP_SUCCESS );
                    }
                }

                STL_TRY( smpAcquire( NULL,
                                     sIterator->mTbsId,
                                     sIterator->mPathStack.mStack[sIterator->mPathStack.mDepth].mPid,
                                     KNL_LATCH_MODE_SHARED,
                                     &sPageHandle,
                                     (smlEnv*)aEnv ) == STL_SUCCESS );
                sState = 1;
                if( smpGetFreeness( &sPageHandle ) == SMP_FREENESS_FREE )
                {
                    sState = 0;
                    STL_TRY( smpRelease( &sPageHandle, (smlEnv*)aEnv ) == STL_SUCCESS );
                    *aTupleExist = STL_FALSE;
                    STL_THROW( ERR_MUTATING_OBJECT );
                }
                stlMemcpy( sIterator->mPageBuf, SMP_FRAME(&sPageHandle), SMP_PAGE_SIZE );
                sIterator->mLevel = SMNMPB_GET_LEVEL(&sPageHandle);
                sIterator->mTotalKeyCount = SMP_GET_PAGE_ROW_ITEM_COUNT(&sPageHandle);
                sState = 0;
                STL_TRY( smpRelease( &sPageHandle, (smlEnv*)aEnv ) == STL_SUCCESS );
            }
            else
            {
                /* 현재 페이지를 모두 반환하였으면 상위 페이지로 */
                STL_ASSERT( sIterator->mLevel > 0 );
                sIterator->mPathStack.mDepth--;
                if( sIterator->mPathStack.mDepth == -1 )
                {
                    *aTupleExist = STL_FALSE;
                    STL_THROW( RAMP_SUCCESS );
                }

                STL_TRY( smpAcquire( NULL,
                                     sIterator->mTbsId,
                                     sIterator->mPathStack.mStack[sIterator->mPathStack.mDepth].mPid,
                                     KNL_LATCH_MODE_SHARED,
                                     &sPageHandle,
                                     (smlEnv*)aEnv ) == STL_SUCCESS );
                sState = 1;
                sIterator->mLevel = SMNMPB_GET_LEVEL(&sPageHandle);
                sIterator->mTotalKeyCount = SMP_GET_PAGE_ROW_ITEM_COUNT(&sPageHandle);
                if( smpGetFreeness( &sPageHandle ) == SMP_FREENESS_FREE )
                {
                    sState = 0;
                    STL_TRY( smpRelease( &sPageHandle, (smlEnv*)aEnv ) == STL_SUCCESS );
                    *aTupleExist = STL_FALSE;
                    STL_THROW( ERR_MUTATING_OBJECT );
                }
                stlMemcpy( sIterator->mPageBuf, SMP_FRAME(&sPageHandle), SMP_PAGE_SIZE );
                sState = 0;
                STL_TRY( smpRelease( &sPageHandle, (smlEnv*)aEnv ) == STL_SUCCESS );
            }
            continue;
        }
        else
        {
            if( sIterator->mPrinted[sIterator->mPathStack.mDepth] == STL_TRUE )
            {
                STL_ASSERT( sIterator->mLevel > 0 );
                /* internal node 이고 아래로 */
                sIterator->mPathStack.mStack[sIterator->mPathStack.mDepth].mSeq++;
                STL_TRY( smpAcquire( NULL,
                                     sIterator->mTbsId,
                                     sIterator->mPathStack.mStack[sIterator->mPathStack.mDepth].mPid,
                                     KNL_LATCH_MODE_SHARED,
                                     &sPageHandle,
                                     (smlEnv*)aEnv ) == STL_SUCCESS );
                sState = 1;
                if( (smpGetFreeness( &sPageHandle ) == SMP_FREENESS_FREE) ||
                    (sIterator->mPathStack.mStack[sIterator->mPathStack.mDepth].mSeq
                     >= SMP_GET_PAGE_ROW_ITEM_COUNT(&sPageHandle)) )
                {
                    sState = 0;
                    STL_TRY( smpRelease( &sPageHandle, (smlEnv*)aEnv ) == STL_SUCCESS );
                    *aTupleExist = STL_FALSE;
                    STL_THROW( ERR_MUTATING_OBJECT );
                }
                sKeySlot = smpGetNthRowItem( &sPageHandle, 
                                             sIterator->mPathStack.mStack[sIterator->mPathStack.mDepth].mSeq );
                SMNMPB_INTL_SLOT_GET_CHILD_PID( sKeySlot, &sPageId );
                sState = 0;
                STL_TRY( smpRelease( &sPageHandle, (smlEnv*)aEnv ) == STL_SUCCESS );
                sIterator->mPathStack.mStack[sIterator->mPathStack.mDepth + 1].mPid = sPageId;
                sIterator->mPathStack.mStack[sIterator->mPathStack.mDepth + 1].mSeq = -1;
                sIterator->mPrinted[sIterator->mPathStack.mDepth + 1] = STL_FALSE;
                sIterator->mPathStack.mDepth++;
 
                STL_TRY( smpAcquire( NULL,
                                     sIterator->mTbsId,
                                     sIterator->mPathStack.mStack[sIterator->mPathStack.mDepth].mPid,
                                     KNL_LATCH_MODE_SHARED,
                                     &sPageHandle,
                                     (smlEnv*)aEnv ) == STL_SUCCESS );
                sState = 1;
                sIterator->mLevel = SMNMPB_GET_LEVEL(&sPageHandle);
                sIterator->mTotalKeyCount = SMP_GET_PAGE_ROW_ITEM_COUNT(&sPageHandle);
                if( smpGetFreeness( &sPageHandle ) == SMP_FREENESS_FREE )
                {
                    sState = 0;
                    STL_TRY( smpRelease( &sPageHandle, (smlEnv*)aEnv ) == STL_SUCCESS );
                    *aTupleExist = STL_FALSE;
                    STL_THROW( ERR_MUTATING_OBJECT );
                }
                stlMemcpy( sIterator->mPageBuf, SMP_FRAME(&sPageHandle), SMP_PAGE_SIZE );
                sState = 0;
                STL_TRY( smpRelease( &sPageHandle, (smlEnv*)aEnv ) == STL_SUCCESS );
            }
            break;
        } /* if all done */
    } /* while */

    (sIterator->mPathStack.mStack[sIterator->mPathStack.mDepth].mSeq)++;

    SMP_MAKE_DUMMY_HANDLE( &sPageHandle, sIterator->mPageBuf );
    sKeySlot = smpGetNthRowItem( &sPageHandle,
                                 sIterator->mPathStack.mStack[sIterator->mPathStack.mDepth].mSeq );
    sFxKey->mCurPageId = sIterator->mPathStack.mStack[sIterator->mPathStack.mDepth].mPid;
    sFxKey->mLevel = sIterator->mLevel;
    sFxKey->mParentPageId = (sIterator->mPathStack.mDepth == 0) ? 0 :
        sIterator->mPathStack.mStack[sIterator->mPathStack.mDepth - 1].mPid;
    sFxKey->mSeq = sIterator->mPathStack.mStack[sIterator->mPathStack.mDepth].mSeq;

    if( sIterator->mLevel == 0 )
    {
        /* Leaf Node */
        sFxKey->mKeyStatusFlags = *(stlUInt8*)sKeySlot;
        sFxKey->mChildPageId = 0;
        SMNMPB_GET_ROW_PID( sKeySlot, &sFxKey->mRowPageId );
        SMNMPB_GET_ROW_SEQ( sKeySlot, &sFxKey->mRowOffset );

        SMNMPB_GET_INS_RTS_INFO( sKeySlot, &sTmpRtsSeq, &sTmpRtsVerNo );
        SMNMPB_GET_INS_TCN( sKeySlot, &sTcn );
        sFxKey->mInsertRtsSeq = sTmpRtsSeq; 
        sFxKey->mInsertRtsVerNo = sTmpRtsVerNo;
        sFxKey->mInsertTcn = sTcn;
        SMNMPB_GET_DEL_RTS_INFO( sKeySlot, &sTmpRtsSeq, &sTmpRtsVerNo );
        SMNMPB_GET_DEL_TCN( sKeySlot, &sTcn );
        sFxKey->mDeleteRtsSeq = sTmpRtsSeq; 
        sFxKey->mDeleteRtsVerNo = sTmpRtsVerNo; 
        sFxKey->mDeleteTcn = sTcn;

        if( SMNMPB_IS_TBK( sKeySlot ) != STL_TRUE )
        {
            sFxKey->mInsertTransId = SML_INVALID_TRANSID;
            sFxKey->mInsertScn = SML_INFINITE_SCN;
            sFxKey->mDeleteTransId = SML_INVALID_TRANSID;
            sFxKey->mDeleteScn = SML_INFINITE_SCN;
            sColPtr = sKeySlot + SMNMPB_RBK_HDR_SIZE;
        }
        else
        {
            SMNMPB_GET_TBK_INS_TRANS_ID( sKeySlot, &sFxKey->mInsertTransId );
            SMNMPB_GET_TBK_INS_SCN( sKeySlot, &sFxKey->mInsertScn );
            SMNMPB_GET_TBK_DEL_TRANS_ID( sKeySlot, &sFxKey->mDeleteTransId );
            SMNMPB_GET_TBK_DEL_SCN( sKeySlot, &sFxKey->mDeleteScn );
            sColPtr = sKeySlot + SMNMPB_TBK_HDR_SIZE;
        }
    }
    else
    {
        /* Internal Node */
        sFxKey->mKeyStatusFlags = 0;
        STL_WRITE_INT32(&sFxKey->mChildPageId, sKeySlot);
        if( (sIterator->mPathStack.mStack[sIterator->mPathStack.mDepth].mSeq 
             == sIterator->mTotalKeyCount - 1) &&
            (SMNMPB_IS_LAST_SIBLING(&sPageHandle) == STL_TRUE) )
        {
            sFxKey->mRowPageId = SMP_NULL_PID;
            sFxKey->mRowOffset = -1;
            sFxKey->mInsertRtsSeq = SMP_RTS_SEQ_NULL;
            sFxKey->mInsertRtsVerNo = 0;
            sFxKey->mInsertTcn = SMNMPB_INVALID_TCN;
            sFxKey->mInsertTransId = SML_INVALID_TRANSID;
            sFxKey->mInsertScn = SML_INFINITE_SCN;
            sFxKey->mDeleteRtsSeq = SMP_RTS_SEQ_NULL;
            sFxKey->mDeleteRtsVerNo = 0;
            sFxKey->mDeleteTcn = SMNMPB_INVALID_TCN;
            sFxKey->mDeleteTransId = SML_INVALID_TRANSID;
            sFxKey->mDeleteScn = SML_INFINITE_SCN;
            sColPtr = NULL;
        }
        else
        {
            STL_WRITE_INT32(&sFxKey->mRowPageId, sKeySlot + STL_SIZEOF(smlPid));
            STL_WRITE_INT16(&sFxKey->mRowOffset, sKeySlot + STL_SIZEOF(smlPid) + STL_SIZEOF(smlPid));
            sFxKey->mInsertRtsSeq = SMP_RTS_SEQ_NULL;
            sFxKey->mInsertRtsVerNo = 0;
            sFxKey->mInsertTcn = SMNMPB_INVALID_TCN;
            sFxKey->mInsertTransId = SML_INVALID_TRANSID;
            sFxKey->mInsertScn = SML_INFINITE_SCN;
            sFxKey->mDeleteRtsSeq = SMP_RTS_SEQ_NULL;
            sFxKey->mDeleteRtsVerNo = 0;
            sFxKey->mDeleteTcn = SMNMPB_INVALID_TCN;
            sFxKey->mDeleteTransId = SML_INVALID_TRANSID;
            sFxKey->mDeleteScn = SML_INFINITE_SCN;
            sColPtr = sKeySlot + SMNMPB_INTL_SLOT_HDR_SIZE;
        }
    }

    STL_RAMP( RAMP_SKIP_FETCH );

    sFxKey->mColumnSeq = sIterator->mNextColumnIdx;
    
    stlMemset(sFxKey->mColumn, 0x00, SMP_PAGE_SIZE);
    STL_TRY( smnmpbMakeKeyString( sColPtr,
                                  sIterator->mNextColumnIdx,
                                  SMNMPB_GET_KEY_COLUMN_COUNT(&sPageHandle),
                                  SMNMPB_GET_KEY_COLUMN_TYPES(&sPageHandle),
                                  (stlChar*)sFxKey->mColumn,
                                  aEnv )
             == STL_SUCCESS );

    STL_TRY( knlBuildFxRecord( gSmnmpbKeyColumnDesc,
                               sFxKey,
                               aValueList,
                               aBlockIdx,
                               aEnv )
             == STL_SUCCESS );

    sIterator->mNextColumnIdx++;
    
    if( sIterator->mTotalColumnCount <= sIterator->mNextColumnIdx )
    {
        sIterator->mNextColumnIdx = 0;
    }
    
    *aTupleExist = STL_TRUE;

    STL_RAMP( RAMP_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( ERR_MUTATING_OBJECT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_MUTATING_DUMP_OBJECT,
                      NULL,
                      KNL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smpRelease( &sPageHandle, (smlEnv*)aEnv );
    }

    return STL_FAILURE;
}



knlFxTableDesc gSmnmpbKeyTableDesc =
{
    KNL_FXTABLE_USAGE_DUMP_TABLE,
    KNL_STARTUP_PHASE_OPEN,
    smnmpbGetDumpObjectCallback,
    smnmpbOpenKeyCallback,
    smnmpbCloseKeyCallback,
    smnmpbBuildRecordKeyCallback,
    STL_SIZEOF( smnmpbFxKeyIterator ),
    "D$MEMORY_BTREE_KEY",
    "memory B-Tree key",
    gSmnmpbKeyColumnDesc
};



knlFxColumnDesc gSmnmpbHdrColumnDesc[] =
{
    {
        "CURRENT_PAGE_ID",
        "current page identifier",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smnmpbFxHdr, mCurPageId ),
        STL_SIZEOF( smlPid ),
        STL_FALSE  /* nullable */
    },
    {
        "PARENT_PAGE_ID",
        "parent page identifier",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smnmpbFxHdr, mParentPageId ),
        STL_SIZEOF( smlPid ),
        STL_FALSE  /* nullable */
    },
    {
        "MINIMUM_RTS_COUNT",
        "minimum rts count",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smnmpbFxHdr, mMinRtsCount ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "MAXIMUM_RTS_COUNT",
        "maximum rts count",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smnmpbFxHdr, mMaxRtsCount ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "CURRENT_RTS_COUNT",
        "current rts count",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smnmpbFxHdr, mCurRtsCount ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "HIGH_WATER_MARK",
        "high water mark",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smnmpbFxHdr, mHighWaterMark ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "LOW_WATER_MARK",
        "low water mark",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smnmpbFxHdr, mLowWaterMark ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "TOTAL_KEY_COUNT",
        "total key count",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smnmpbFxHdr, mTotalKeyCount ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "PREV_PAGE_ID",
        "previous page identifier",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smnmpbFxHdr, mPrevPageId ),
        STL_SIZEOF( smlPid ),
        STL_FALSE  /* nullable */
    },
    {
        "NEXT_PAGE_ID",
        "next page identifier",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smnmpbFxHdr, mNextPageId ),
        STL_SIZEOF( smlPid ),
        STL_FALSE  /* nullable */
    },
    {
        "NEXT_FREE_PAGE_ID",
        "next free page identifier",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smnmpbFxHdr, mNextFreePageId ),
        STL_SIZEOF( smlPid ),
        STL_FALSE  /* nullable */
    },
    {
        "LEVEL",
        "level of node",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smnmpbFxHdr, mLevel ),
        STL_SIZEOF( stlUInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "ACTIVE_KEY_COUNT",
        "active(not deleted) key count",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smnmpbFxHdr, mActiveKeyCount ),
        STL_SIZEOF( stlUInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "KEY_COLUMN_COUNT",
        "key column count",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smnmpbFxHdr, mKeyColCount ),
        STL_SIZEOF( stlUInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "SMO_NO",
        "node smo number",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smnmpbFxHdr, mSmoNo ),
        STL_SIZEOF( stlUInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "FLAGS",
        "node flag",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smnmpbFxHdr, mFlags ),
        STL_SIZEOF( stlUInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        NULL,
        NULL,
        0,
        0,
        0,
        STL_FALSE  /* nullable */
    }
};


stlStatus smnmpbOpenHdrCallback( void   * aStmt,
                                 void   * aDumpObjHandle,
                                 void   * aPathCtrl,
                                 knlEnv * aEnv )
{
    smnmpbFxHdrIterator * sIterator;
    smpHandle             sPageHandle;
    stlInt16              sState = 0;
    stlInt32              i;

    STL_TRY( smsValidateSegment( SME_GET_SEGMENT_HANDLE(aDumpObjHandle),
                                 SML_ENV(aEnv) )
             == STL_SUCCESS );
    
    sIterator = (smnmpbFxHdrIterator*)aPathCtrl;
    sIterator->mTbsId = smsGetTbsId(SME_GET_SEGMENT_HANDLE(aDumpObjHandle));

    sIterator->mPathStack.mDepth = 0;
    sIterator->mPathStack.mStack[sIterator->mPathStack.mDepth].mPid = 
        SMN_GET_ROOT_PAGE_ID(aDumpObjHandle);
    sIterator->mPathStack.mStack[sIterator->mPathStack.mDepth].mSeq = -1;

    for( i = 0; i < SMNMPB_MAX_INDEX_DEPTH; i++ )
    {
        sIterator->mPrinted[i] = STL_FALSE;
    }

    STL_TRY( smpAcquire( NULL,
                         sIterator->mTbsId,
                         sIterator->mPathStack.mStack[sIterator->mPathStack.mDepth].mPid,
                         KNL_LATCH_MODE_SHARED,
                         &sPageHandle,
                         (smlEnv*)aEnv ) == STL_SUCCESS );
    sState = 1;

    stlMemcpy( sIterator->mPageBuf, SMP_FRAME(&sPageHandle), SMP_PAGE_SIZE );
    sIterator->mLevel = SMNMPB_GET_LEVEL(&sPageHandle);
    sIterator->mTotalKeyCount = SMP_GET_PAGE_ROW_ITEM_COUNT(&sPageHandle);

    sState = 0;
    STL_TRY( smpRelease( &sPageHandle, (smlEnv*)aEnv ) == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        smpRelease( &sPageHandle, (smlEnv*)aEnv );
    }

    return STL_FAILURE;
}

stlStatus smnmpbCloseHdrCallback( void   * aDumpObjHandle,
                                  void   * aPathCtrl,
                                  knlEnv * aEnv )
{
    return STL_SUCCESS;
}


stlStatus smnmpbBuildRecordHdrCallback( void              * aDumpObjHandle,
                                        void              * aPathCtrl,
                                        knlValueBlockList * aValueList,
                                        stlInt32            aBlockIdx,
                                        stlBool           * aTupleExist,
                                        knlEnv            * aEnv )
{
    smnmpbFxHdrIterator * sIterator;
    smnmpbFxHdr         * sFxHdr;
    smpHandle             sPageHandle;
    stlChar             * sKeySlot;
    smlPid                sPageId;
    stlInt16              sState = 0;

    sIterator = (smnmpbFxHdrIterator*)aPathCtrl;
    sFxHdr = &sIterator->mHdrRecord;

    while( STL_TRUE )
    {
        STL_ASSERT( sIterator->mPathStack.mDepth < SMNMPB_MAX_INDEX_DEPTH );
        if( sIterator->mPathStack.mStack[sIterator->mPathStack.mDepth].mSeq + 1 >=
            sIterator->mTotalKeyCount )
        {
            if( sIterator->mPrinted[sIterator->mPathStack.mDepth] == STL_FALSE )
            {
                /* 현재 노드 출력 완료 */
                sIterator->mPrinted[sIterator->mPathStack.mDepth] = STL_TRUE;
                sIterator->mPathStack.mStack[sIterator->mPathStack.mDepth].mSeq = 0;
                if( sIterator->mLevel > 0 )
                {
                    /* internal node 이면 아래로 */
                    STL_TRY( smpAcquire( NULL,
                                         sIterator->mTbsId,
                                         sIterator->mPathStack.mStack[sIterator->mPathStack.mDepth].mPid,
                                         KNL_LATCH_MODE_SHARED,
                                         &sPageHandle,
                                         (smlEnv*)aEnv ) == STL_SUCCESS );
                    sState = 1;
                    if( smpGetFreeness( &sPageHandle ) == SMP_FREENESS_FREE )
                    {
                        sState = 0;
                        STL_TRY( smpRelease( &sPageHandle, (smlEnv*)aEnv ) == STL_SUCCESS );
                        *aTupleExist = STL_FALSE;
                        STL_THROW( ERR_MUTATING_OBJECT );
                    }
                    sKeySlot = smpGetNthRowItem( &sPageHandle, 0 );
                    SMNMPB_INTL_SLOT_GET_CHILD_PID( sKeySlot, &sPageId );
                    sState = 0;
                    STL_TRY( smpRelease( &sPageHandle, (smlEnv*)aEnv ) == STL_SUCCESS );
                    sIterator->mPathStack.mStack[sIterator->mPathStack.mDepth + 1].mPid = sPageId;
                    sIterator->mPathStack.mStack[sIterator->mPathStack.mDepth + 1].mSeq = -1;
                    sIterator->mPrinted[sIterator->mPathStack.mDepth + 1] = STL_FALSE;
                    sIterator->mPathStack.mDepth++;
                }
                else
                {
                    /* leaf node 이면 위로 */
                    sIterator->mPathStack.mDepth--;
                    if( sIterator->mPathStack.mDepth == -1 )
                    {
                        *aTupleExist = STL_FALSE;
                        STL_THROW( RAMP_SUCCESS );
                    }
                }

                STL_TRY( smpAcquire( NULL,
                                     sIterator->mTbsId,
                                     sIterator->mPathStack.mStack[sIterator->mPathStack.mDepth].mPid,
                                     KNL_LATCH_MODE_SHARED,
                                     &sPageHandle,
                                     (smlEnv*)aEnv ) == STL_SUCCESS );
                sState = 1;
                if( smpGetFreeness( &sPageHandle ) == SMP_FREENESS_FREE )
                {
                    sState = 0;
                    STL_TRY( smpRelease( &sPageHandle, (smlEnv*)aEnv ) == STL_SUCCESS );
                    *aTupleExist = STL_FALSE;
                    STL_THROW( ERR_MUTATING_OBJECT );
                }
                stlMemcpy( sIterator->mPageBuf, SMP_FRAME(&sPageHandle), SMP_PAGE_SIZE );
                sIterator->mLevel = SMNMPB_GET_LEVEL(&sPageHandle);
                sIterator->mTotalKeyCount = SMP_GET_PAGE_ROW_ITEM_COUNT(&sPageHandle);
                sState = 0;
                STL_TRY( smpRelease( &sPageHandle, (smlEnv*)aEnv ) == STL_SUCCESS );
            }
            else
            {
                /* 현재 페이지를 모두 반환하였으면 상위 페이지로 */
                STL_ASSERT( sIterator->mLevel > 0 );
                sIterator->mPathStack.mDepth--;
                if( sIterator->mPathStack.mDepth == -1 )
                {
                    *aTupleExist = STL_FALSE;
                    STL_THROW( RAMP_SUCCESS );
                }

                STL_TRY( smpAcquire( NULL,
                                     sIterator->mTbsId,
                                     sIterator->mPathStack.mStack[sIterator->mPathStack.mDepth].mPid,
                                     KNL_LATCH_MODE_SHARED,
                                     &sPageHandle,
                                     (smlEnv*)aEnv ) == STL_SUCCESS );
                sState = 1;
                sIterator->mLevel = SMNMPB_GET_LEVEL(&sPageHandle);
                sIterator->mTotalKeyCount = SMP_GET_PAGE_ROW_ITEM_COUNT(&sPageHandle);
                if( smpGetFreeness( &sPageHandle ) == SMP_FREENESS_FREE )
                {
                    sState = 0;
                    STL_TRY( smpRelease( &sPageHandle, (smlEnv*)aEnv ) == STL_SUCCESS );
                    *aTupleExist = STL_FALSE;
                    STL_THROW( ERR_MUTATING_OBJECT );
                }
                stlMemcpy( sIterator->mPageBuf, SMP_FRAME(&sPageHandle), SMP_PAGE_SIZE );
                sState = 0;
                STL_TRY( smpRelease( &sPageHandle, (smlEnv*)aEnv ) == STL_SUCCESS );
            }
            continue;
        }
        else
        {
            if( sIterator->mPrinted[sIterator->mPathStack.mDepth] == STL_TRUE )
            {
                STL_ASSERT( sIterator->mLevel > 0 );
                /* internal node 이고 아래로 */
                sIterator->mPathStack.mStack[sIterator->mPathStack.mDepth].mSeq++;
                STL_TRY( smpAcquire( NULL,
                                     sIterator->mTbsId,
                                     sIterator->mPathStack.mStack[sIterator->mPathStack.mDepth].mPid,
                                     KNL_LATCH_MODE_SHARED,
                                     &sPageHandle,
                                     (smlEnv*)aEnv ) == STL_SUCCESS );
                sState = 1;
                if( (smpGetFreeness( &sPageHandle ) == SMP_FREENESS_FREE) ||
                    (sIterator->mPathStack.mStack[sIterator->mPathStack.mDepth].mSeq
                     >= SMP_GET_PAGE_ROW_ITEM_COUNT(&sPageHandle)) )
                {
                    sState = 0;
                    STL_TRY( smpRelease( &sPageHandle, (smlEnv*)aEnv ) == STL_SUCCESS );
                    *aTupleExist = STL_FALSE;
                    STL_THROW( ERR_MUTATING_OBJECT );
                }
                sKeySlot = smpGetNthRowItem( &sPageHandle, 
                                             sIterator->mPathStack.mStack[sIterator->mPathStack.mDepth].mSeq );
                SMNMPB_INTL_SLOT_GET_CHILD_PID( sKeySlot, &sPageId );
                sState = 0;
                STL_TRY( smpRelease( &sPageHandle, (smlEnv*)aEnv ) == STL_SUCCESS );
                sIterator->mPathStack.mStack[sIterator->mPathStack.mDepth + 1].mPid = sPageId;
                sIterator->mPathStack.mStack[sIterator->mPathStack.mDepth + 1].mSeq = -1;
                sIterator->mPrinted[sIterator->mPathStack.mDepth + 1] = STL_FALSE;
                sIterator->mPathStack.mDepth++;
 
                STL_TRY( smpAcquire( NULL,
                                     sIterator->mTbsId,
                                     sIterator->mPathStack.mStack[sIterator->mPathStack.mDepth].mPid,
                                     KNL_LATCH_MODE_SHARED,
                                     &sPageHandle,
                                     (smlEnv*)aEnv ) == STL_SUCCESS );
                sState = 1;
                sIterator->mLevel = SMNMPB_GET_LEVEL(&sPageHandle);
                sIterator->mTotalKeyCount = SMP_GET_PAGE_ROW_ITEM_COUNT(&sPageHandle);
                if( smpGetFreeness( &sPageHandle ) == SMP_FREENESS_FREE )
                {
                    sState = 0;
                    STL_TRY( smpRelease( &sPageHandle, (smlEnv*)aEnv ) == STL_SUCCESS );
                    *aTupleExist = STL_FALSE;
                    STL_THROW( ERR_MUTATING_OBJECT );
                }
                stlMemcpy( sIterator->mPageBuf, SMP_FRAME(&sPageHandle), SMP_PAGE_SIZE );
                sState = 0;
                STL_TRY( smpRelease( &sPageHandle, (smlEnv*)aEnv ) == STL_SUCCESS );
            }
            break;
        } /* if all done */
    } /* while */

    sIterator->mPathStack.mStack[sIterator->mPathStack.mDepth].mSeq = sIterator->mTotalKeyCount;

    SMP_MAKE_DUMMY_HANDLE( &sPageHandle, sIterator->mPageBuf );

    sFxHdr->mCurPageId = sIterator->mPathStack.mStack[sIterator->mPathStack.mDepth].mPid;
    sFxHdr->mParentPageId = (sIterator->mPathStack.mDepth == 0) ? 0 :
        sIterator->mPathStack.mStack[sIterator->mPathStack.mDepth - 1].mPid;
    sFxHdr->mMinRtsCount = smpGetMinRtsCount(&sPageHandle);
    sFxHdr->mMaxRtsCount = smpGetMaxRtsCount(&sPageHandle);
    sFxHdr->mCurRtsCount = SMP_GET_CURRENT_RTS_COUNT( SMP_FRAME(&sPageHandle) );
    sFxHdr->mHighWaterMark = smpGetHighWaterMark(&sPageHandle);
    sFxHdr->mLowWaterMark = smpGetLowWaterMark(&sPageHandle);
    sFxHdr->mTotalKeyCount = SMP_GET_PAGE_ROW_ITEM_COUNT(&sPageHandle);
    sFxHdr->mPrevPageId = SMNMPB_GET_PREV_PAGE(&sPageHandle);
    sFxHdr->mNextPageId = SMNMPB_GET_NEXT_PAGE(&sPageHandle);
    sFxHdr->mNextFreePageId = SMNMPB_GET_NEXT_FREE_PAGE(&sPageHandle);
    sFxHdr->mLevel = SMNMPB_GET_LEVEL(&sPageHandle);
    sFxHdr->mActiveKeyCount = SMP_GET_ACTIVE_SLOT_COUNT(SMP_FRAME(&sPageHandle));
    sFxHdr->mKeyColCount = SMNMPB_GET_KEY_COLUMN_COUNT(&sPageHandle);
    sFxHdr->mSmoNo = *(stlUInt64*)smpGetVolatileArea(&sPageHandle);
    sFxHdr->mFlags = SMNMPB_GET_NODE_FLAG(&sPageHandle);

    STL_TRY( knlBuildFxRecord( gSmnmpbHdrColumnDesc,
                               sFxHdr,
                               aValueList,
                               aBlockIdx,
                               aEnv )
             == STL_SUCCESS );

    *aTupleExist = STL_TRUE;

    STL_RAMP( RAMP_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( ERR_MUTATING_OBJECT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_MUTATING_DUMP_OBJECT,
                      NULL,
                      KNL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smpRelease( &sPageHandle, (smlEnv*)aEnv );
    }

    return STL_FAILURE;
}

knlFxTableDesc gSmnmpbHdrTableDesc =
{
    KNL_FXTABLE_USAGE_DUMP_TABLE,
    KNL_STARTUP_PHASE_OPEN,
    smnmpbGetDumpObjectCallback,
    smnmpbOpenHdrCallback,
    smnmpbCloseHdrCallback,
    smnmpbBuildRecordHdrCallback,
    STL_SIZEOF( smnmpbFxHdrIterator ),
    "D$MEMORY_BTREE_HEADER",
    "memory b-tree header information",
    gSmnmpbHdrColumnDesc
};


knlFxColumnDesc gSmnmpbStatisticsColumnDesc[] =
{
    {
        "PHYSICAL_ID",
        "segment physical indentifier",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smnmpbFxStatistics, mSegmentId ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "SPLIT_COUNT",
        "split count",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smnmpbFxStatistics, mSplitCount ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "COMPACT_COUNT",
        "compact count",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smnmpbFxStatistics, mCompactCount ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "LINK_TRAVERSE_COUNT",
        "link traverse count",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smnmpbFxStatistics, mLinkTraverseCount ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "EMPTY_NODE_COUNT",
        "empty node count",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smnmpbFxStatistics, mEmptyNodeCount ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "SORT_ELAPSED_TIME",
        "elpased time to sort keys",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smnmpbFxStatistics, mSortElapsedTime ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "MERGE_ELAPSED_TIME",
        "elpased time to merge runs",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smnmpbFxStatistics, mMergeElapsedTime ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "BUILD_ELAPSED_TIME",
        "elpased time to build tree",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smnmpbFxStatistics, mBuildElapsedTime ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        NULL,
        NULL,
        0,
        0,
        0,
        STL_FALSE  /* nullable */
    }
};


stlStatus smnmpbOpenStatisticsCallback( void   * aStmt,
                                        void   * aDumpObjHandle,
                                        void   * aPathCtrl,
                                        knlEnv * aEnv )
{
    smnmpbFxStatisticsIterator * sIterator;

    sIterator = (smnmpbFxStatisticsIterator*)aPathCtrl;

    STL_TRY( smsAllocSegMapIterator( (smlStatement*)aStmt,
                                     &sIterator->mSegIterator,
                                     SML_ENV( aEnv ) )
             == STL_SUCCESS );

    sIterator->mFetchStarted = STL_FALSE;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smnmpbCloseStatisticsCallback( void   * aDumpObjHandle,
                                         void   * aPathCtrl,
                                         knlEnv * aEnv )
{
    smnmpbFxStatisticsIterator * sIterator;

    sIterator = (smnmpbFxStatisticsIterator*)aPathCtrl;

    STL_TRY( smsFreeSegMapIterator( sIterator->mSegIterator,
                                    SML_ENV( aEnv ) )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smnmpbBuildRecordStatisticsCallback( void              * aDumpObjHandle,
                                               void              * aPathCtrl,
                                               knlValueBlockList * aValueList,
                                               stlInt32            aBlockIdx,
                                               stlBool           * aTupleExist,
                                               knlEnv            * aEnv )
{
    smnmpbFxStatisticsIterator * sIterator;
    smpHandle                    sPageHandle;
    stlInt16                     sState = 0;
    smnmpbStatistics           * sStatistics;
    smnmpbFxStatistics           sFxStatistics;
    smlRid                       sSegmentRid;
    void                       * sSegmentHandle;
    void                       * sRelationHandle;

    *aTupleExist = STL_FALSE;
    sIterator = (smnmpbFxStatisticsIterator*)aPathCtrl;

    while( 1 )
    {
        if( sIterator->mFetchStarted == STL_FALSE )
        {
            STL_TRY( smsGetFirstSegment( sIterator->mSegIterator,
                                         &sSegmentHandle,
                                         &sSegmentRid,
                                         SML_ENV( aEnv ) )
                     == STL_SUCCESS);
            
            sIterator->mFetchStarted = STL_TRUE;
        }
        else
        {
            STL_TRY( smsGetNextSegment( sIterator->mSegIterator,
                                        &sSegmentHandle,
                                        &sSegmentRid,
                                        SML_ENV( aEnv ) )
                     == STL_SUCCESS);
        }

        if( sSegmentHandle == NULL )
        {
            break;
        }

        if( smsHasRelHdr( sSegmentHandle ) == STL_FALSE )
        {
            continue;
        }
        
        STL_TRY( smpAcquire( NULL,
                             sSegmentRid.mTbsId,
                             sSegmentRid.mPageId,
                             KNL_LATCH_MODE_SHARED,
                             &sPageHandle,
                             SML_ENV(aEnv) ) == STL_SUCCESS );
        sState = 1;

        STL_TRY( smeGetRelationHandle( sSegmentRid,
                                       &sRelationHandle,
                                       SML_ENV(aEnv) ) == STL_SUCCESS );

        if( (sRelationHandle == NULL ) ||
            (SME_GET_RELATION_TYPE( sRelationHandle ) != SML_MEMORY_PERSISTENT_BTREE_INDEX) )
        {
            sState = 0;
            STL_TRY( smpRelease( &sPageHandle, SML_ENV(aEnv) ) == STL_SUCCESS );
            continue;
        }

        sStatistics = SMNMPB_GET_MODULE_SPECIFIC_DATA( sRelationHandle );

        if( sStatistics == NULL )
        {
            sState = 0;
            STL_TRY( smpRelease( &sPageHandle, SML_ENV(aEnv) ) == STL_SUCCESS );
            continue;
        }

        sFxStatistics.mSegmentId = smsGetSegmentId( sSegmentHandle );
        sFxStatistics.mSplitCount = sStatistics->mSplitCount;
        sFxStatistics.mCompactCount = sStatistics->mCompactCount;
        sFxStatistics.mLinkTraverseCount = sStatistics->mLinkTraverseCount;
        sFxStatistics.mEmptyNodeCount = ((smnIndexHeader*)sRelationHandle)->mEmptyNodeCount;
        sFxStatistics.mSortElapsedTime = sStatistics->mSortElapsedTime;
        sFxStatistics.mMergeElapsedTime = sStatistics->mMergeElapsedTime;
        sFxStatistics.mBuildElapsedTime = sStatistics->mBuildElapsedTime;

        sState = 0;
        STL_TRY( smpRelease( &sPageHandle, SML_ENV(aEnv) ) == STL_SUCCESS );

        STL_TRY( knlBuildFxRecord( gSmnmpbStatisticsColumnDesc,
                                   &sFxStatistics,
                                   aValueList,
                                   aBlockIdx,
                                   aEnv )
                 == STL_SUCCESS );

        *aTupleExist = STL_TRUE;
        break;
    }

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smpRelease( &sPageHandle, (smlEnv*)aEnv );
    }

    return STL_FAILURE;
}

knlFxTableDesc gSmnmpbStatisticsTableDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,
    KNL_STARTUP_PHASE_OPEN,
    NULL,
    smnmpbOpenStatisticsCallback,
    smnmpbCloseStatisticsCallback,
    smnmpbBuildRecordStatisticsCallback,
    STL_SIZEOF( smnmpbFxHdrIterator ),
    "X$MEMORY_BTREE_STATISTICS",
    "memory b-tree statistics",
    gSmnmpbStatisticsColumnDesc
};

knlFxColumnDesc gSmnmpbVerifyOrderColumnDesc[] =
{
    {
        "PHYSICAL_ID",
        "segment physical indentifier",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smnmpbFxVerifyOrder, mPhysicalId ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "RESULT",
        "page indentifier",
        KNL_FXTABLE_DATATYPE_BOOL,
        STL_OFFSETOF( smnmpbFxVerifyOrder, mResult ),
        STL_SIZEOF( stlBool ),
        STL_FALSE  /* nullable */
    },
    {
        NULL,
        NULL,
        0,
        0,
        0,
        STL_FALSE
    }
};

stlStatus smnmpbOpenVerifyOrderCallback( void   * aStmt,
                                         void   * aDumpObjHandle,
                                         void   * aPathCtrl,
                                         knlEnv * aEnv )
{
    smnmpbFxVerifyOrderIterator * sIterator;
    stlInt32                      i;
    smnIndexHeader              * sIndexHeader;
    dtlDataType                   sDataType;

    sIterator = (smnmpbFxVerifyOrderIterator*)aPathCtrl;
    sIndexHeader = (smnIndexHeader*)aDumpObjHandle;

    stlMemset( sIterator, 0x00, STL_SIZEOF(smnmpbFxVerifyOrderIterator) );
    
    sIterator->mTbsId = smsGetTbsId( SME_GET_SEGMENT_HANDLE(aDumpObjHandle) );
    sIterator->mCompareInfo.mUniqueViolated = 0;
    sIterator->mCompareInfo.mKeyColCount = sIndexHeader->mKeyColCount;
    sIterator->mCompareInfo.mKeyColFlags = sIndexHeader->mKeyColFlags;
    sIterator->mCompareInfo.mKeyColOrder = sIterator->mKeyColOrder;

    for( i = 0; i < sIndexHeader->mKeyColCount; i++ )
    {
        if( (sIndexHeader->mKeyColFlags[i] & DTL_KEYCOLUMN_INDEX_ORDER_MASK)
            == DTL_KEYCOLUMN_INDEX_ORDER_ASC )
        {
            sIterator->mCompareInfo.mKeyColOrder[i] = 1;
        }
        else
        {
            sIterator->mCompareInfo.mKeyColOrder[i] = -1;
        }
        
        sDataType = sIndexHeader->mKeyColTypes[i];
        sIterator->mCompareInfo.mPhysicalCompare[i] =
            dtlPhysicalCompareFuncList[sDataType][sDataType];
    }

    sIterator->mWorkDone = STL_FALSE;
    
    return STL_SUCCESS;
}

stlStatus smnmpbCloseVerifyOrderCallback( void   * aDumpObjHandle,
                                          void   * aPathCtrl,
                                          knlEnv * aEnv )
{
    return STL_SUCCESS;
}


stlStatus smnmpbBuildRecordVerifyOrderCallback( void              * aDumpObjHandle,
                                                void              * aPathCtrl,
                                                knlValueBlockList * aValueList,
                                                stlInt32            aBlockIdx,
                                                stlBool           * aTupleExist,
                                                knlEnv            * aEnv )
{
    smnmpbFxVerifyOrderIterator * sIterator;
    smlPid                        sPageId;
    smnmpbFxVerifyOrder           sRecord;

    *aTupleExist = STL_FALSE;
    
    sIterator = (smnmpbFxVerifyOrderIterator*)aPathCtrl;

    STL_TRY_THROW( sIterator->mWorkDone == STL_FALSE, RAMP_FINISH );

    sPageId = SMN_GET_ROOT_PAGE_ID(aDumpObjHandle);

    sRecord.mPhysicalId = smsGetSegmentId( SME_GET_SEGMENT_HANDLE(aDumpObjHandle) );
    sRecord.mResult = STL_TRUE;
    
    STL_TRY( smnmpbVerifyOrderTree( sIterator,
                                    sPageId,
                                    0,
                                    NULL,  /* aKeyPa */
                                    NULL,  /* aKeyPb */
                                    &sRecord.mResult,
                                    SML_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( knlBuildFxRecord( gSmnmpbVerifyOrderColumnDesc,
                               &sRecord,
                               aValueList,
                               aBlockIdx,
                               aEnv )
             == STL_SUCCESS );
    
    *aTupleExist = STL_TRUE;
    sIterator->mWorkDone = STL_TRUE;

    STL_RAMP( RAMP_FINISH );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * 인덱스 키 정렬상태를 검증을 위해서 아래의 2가지 조건을 검사한다.
 * 
 * P : 부모 노드
 * C : 자식 노드
 * i : C를 가리키고 있는 부모 키
 * j : C에 있는 임의의 키
 * 
 * (1) P(i-1) <= C(all) <= P(i)
 * (2) C(j) <= C(j+1)
 */

knlFxTableDesc gSmnmpbVerifyOrderTableDesc =
{
    KNL_FXTABLE_USAGE_DUMP_TABLE,
    KNL_STARTUP_PHASE_OPEN,
    smnmpbGetDumpObjectCallback,
    smnmpbOpenVerifyOrderCallback,
    smnmpbCloseVerifyOrderCallback,
    smnmpbBuildRecordVerifyOrderCallback,
    STL_SIZEOF( smnmpbFxVerifyOrderIterator ),
    "D$INDEX_VERIFY_ORDER",
    "verify index key ordering",
    gSmnmpbVerifyOrderColumnDesc
};

stlStatus smnmpbVerifyOrderTree( smnmpbFxVerifyOrderIterator * aIterator,
                                 smlPid                        aPageId,
                                 stlUInt16                     aDepth,
                                 stlChar                     * aKeyPa,
                                 stlChar                     * aKeyPb,
                                 stlBool                     * aResult,
                                 smlEnv                      * aEnv )
{
    smlPid                      sPageId;
    smpHandle                   sPageHandle;
    stlChar                   * sKeyCa;
    stlChar                   * sKeyCb;
    smnmpbFxVerifyOrderItem   * sStackItem;
    stlUInt16                   sKeySize = 0;
    stlBool                     sIsLeaf;
    stlUInt16                   sKeyCount;
    stlInt32                    i;
    stlInt32                    sState = 0;
    stlBool                     sIsMaxKey = STL_FALSE;
        
    STL_TRY( knlCheckQueryTimeout( KNL_ENV(aEnv) ) == STL_SUCCESS );
    
    sStackItem = &(aIterator->mStack[aDepth]);
    
    STL_TRY( smpAcquire( NULL,
                         aIterator->mTbsId,
                         aPageId,
                         KNL_LATCH_MODE_SHARED,
                         &sPageHandle,
                         SML_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 1;
    
    if( smpGetFreeness( &sPageHandle ) == SMP_FREENESS_FREE )
    {
        /* 현재 index tree가 SMO로 변경 중이라서 정확한 validation을 할 수 없다 */
        sState = 0;
        STL_TRY( smpRelease( &sPageHandle, aEnv ) == STL_SUCCESS );
        *aResult = STL_FALSE;
        STL_THROW( ERR_MUTATING_OBJECT );
    }

    stlMemcpy( sStackItem->mPageBuf,
               SMP_FRAME( &sPageHandle ),
               SMP_PAGE_SIZE );

    sStackItem->mPageId = aPageId;

    sState = 0;
    STL_TRY( smpRelease( &sPageHandle, aEnv ) == STL_SUCCESS );

    SMP_MAKE_DUMMY_HANDLE( &sPageHandle, sStackItem->mPageBuf );
    
    sKeyCount = SMP_GET_PAGE_ROW_ITEM_COUNT( &sPageHandle );
    sIsLeaf = SMNMPB_IS_LEAF( &sPageHandle );

    for( i = 0; i < sKeyCount; i++ )
    {
        sKeyCa = sStackItem->mPrevKey;
        sKeyCb = smpGetNthRowItem( &sPageHandle, i );

        /**
         * filtering max key
         */

        if( (SMNMPB_IS_LAST_SIBLING(&sPageHandle) == STL_TRUE) &&
            ((i + 1) == sKeyCount) )
        {
            sIsMaxKey = STL_TRUE;
        }

        if( sIsMaxKey != STL_TRUE )
        {
            if( sKeyCa != NULL )
            {
                /**
                 * compare to sibling key
                 */
                if( smnmpbCompareFxKey( aIterator,
                                        sIsLeaf,
                                        sIsLeaf,
                                        sKeyCa,
                                        sKeyCb ) == DTL_COMPARISON_GREATER )
                {
                    STL_TRY( knlLogMsg( NULL,
                                        KNL_ENV(aEnv),
                                        KNL_LOG_LEVEL_INFO,
                                        "CORRUPTED KEY [ KEY_ID(%d,%d,%d), CAUSE(greater than sibling key) ]\n",
                                        aIterator->mTbsId,
                                        sStackItem->mPageId,
                                        i )
                             == STL_SUCCESS );
                    *aResult = STL_FALSE;
                }
            }

            if( aKeyPa != NULL )
            {
                /**
                 * compare to parent key Pa
                 */
            
                if( smnmpbCompareFxKey( aIterator,
                                        STL_FALSE,  /* aIsLeafA */
                                        sIsLeaf,
                                        aKeyPa,
                                        sKeyCb ) == DTL_COMPARISON_GREATER )
                {
                    STL_TRY( knlLogMsg( NULL,
                                        KNL_ENV(aEnv),
                                        KNL_LOG_LEVEL_INFO,
                                        "CORRUPTED KEY [ KEY_ID(%d,%d,%d), CAUSE(greater than parent key) ]\n",
                                        aIterator->mTbsId,
                                        sStackItem->mPageId,
                                        i )
                             == STL_SUCCESS );
                    *aResult = STL_FALSE;
                }
            }
        
            if( aKeyPb != NULL )
            {
                /**
                 * compare to parent key Pb
                 */
            
                if( smnmpbCompareFxKey( aIterator,
                                        STL_FALSE,  /* aIsLeafA */
                                        sIsLeaf,
                                        aKeyPb,
                                        sKeyCb ) == DTL_COMPARISON_LESS )
                {
                    STL_TRY( knlLogMsg( NULL,
                                        KNL_ENV(aEnv),
                                        KNL_LOG_LEVEL_INFO,
                                        "CORRUPTED KEY [ KEY_ID(%d,%d,%d), CAUSE(less than parent key) ]\n",
                                        aIterator->mTbsId,
                                        sStackItem->mPageId,
                                        i )
                             == STL_SUCCESS );
                    *aResult = STL_FALSE;
                }
            }
        }
        
        if( sIsLeaf == STL_FALSE )
        {
            SMNMPB_INTL_SLOT_GET_CHILD_PID( sKeyCb, &sPageId );

            STL_TRY( smnmpbVerifyOrderTree( aIterator,
                                            sPageId,
                                            aDepth + 1,
                                            sKeyCa,   /* C(j) */
                                            (sIsMaxKey == STL_TRUE) ? NULL : sKeyCb, /* C(j+1) */
                                            aResult,
                                            aEnv )
                     == STL_SUCCESS );

            if( sIsMaxKey == STL_FALSE )
            {
                sKeySize = smnmpbGetIntlKeySize( sKeyCb,
                                                 aIterator->mCompareInfo.mKeyColCount );
            }
        }
        else
        {
            sKeySize = smnmpbGetLeafKeySize( sKeyCb,
                                             aIterator->mCompareInfo.mKeyColCount );
        }

        if( sStackItem->mPrevKey == NULL )
        {
            sStackItem->mPrevKey = sStackItem->mPrevKeyBuf;
        }

        if( sIsMaxKey == STL_FALSE )
        {
            stlMemcpy( sStackItem->mPrevKey, sKeyCb, sKeySize );
        }
    }

    return STL_SUCCESS;

    STL_CATCH( ERR_MUTATING_OBJECT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_MUTATING_DUMP_OBJECT,
                      NULL,
                      KNL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smpRelease( &sPageHandle, aEnv );
    }
    
    return STL_FAILURE;
}

stlInt32 smnmpbCompareFxKey( smnmpbFxVerifyOrderIterator * aIterator,
                             stlBool                       aIsLeafA,
                             stlBool                       aIsLeafB,
                             void                        * aValueA,
                             void                        * aValueB )
{
    dtlCompareResult   sCompareResult = DTL_COMPARISON_EQUAL;
    void             * sKeyBodyA;
    void             * sKeyBodyB;
    stlUInt16          sKeyHdrSizeA;
    stlUInt16          sKeyHdrSizeB;
    smlPid             sRowPidA;
    smlPid             sRowPidB;
    stlInt16           sRowOffsetA;
    stlInt16           sRowOffsetB;
    
    if( aIsLeafA == STL_TRUE )
    {
        SMNMPB_GET_LEAF_KEY_HDR_SIZE( aValueA, &sKeyHdrSizeA );
        
        sKeyBodyA = aValueA + sKeyHdrSizeA;
        
        SMNMPB_GET_ROW_PID( aValueA, &sRowPidA );
        SMNMPB_GET_ROW_SEQ( aValueA, &sRowOffsetA );
    }
    else
    {
        sKeyBodyA = aValueA + SMNMPB_INTL_SLOT_HDR_SIZE;
        
        SMNMPB_INTL_SLOT_GET_ROW_PID( aValueA, &sRowPidA );
        SMNMPB_INTL_SLOT_GET_ROW_OFFSET( aValueA, &sRowOffsetA );
        
    }

    if( aIsLeafB == STL_TRUE )
    {
        SMNMPB_GET_LEAF_KEY_HDR_SIZE( aValueB, &sKeyHdrSizeB );
        
        sKeyBodyB = aValueB + sKeyHdrSizeB;
        
        SMNMPB_GET_ROW_PID( aValueB, &sRowPidB );
        SMNMPB_GET_ROW_SEQ( aValueB, &sRowOffsetB );
    }
    else
    {
        sKeyBodyB = aValueB + SMNMPB_INTL_SLOT_HDR_SIZE;
        
        SMNMPB_INTL_SLOT_GET_ROW_PID( aValueB, &sRowPidB);
        SMNMPB_INTL_SLOT_GET_ROW_OFFSET( aValueB, &sRowOffsetB);
    }
    
    sCompareResult = smnmpbCompare4MergeTree( &aIterator->mCompareInfo,
                                              sKeyBodyA,
                                              sKeyBodyB );

    if( sCompareResult == DTL_COMPARISON_EQUAL )
    {
        if( sRowPidA > sRowPidB )
        {
            sCompareResult = DTL_COMPARISON_GREATER;
        }
        else if( sRowPidA < sRowPidB )
        {
            sCompareResult = DTL_COMPARISON_LESS;
        }
        else
        {
            if( sRowOffsetA > sRowOffsetB )
            {
                sCompareResult = DTL_COMPARISON_GREATER;
            }
            else if( sRowOffsetA < sRowOffsetB )
            {
                sCompareResult = DTL_COMPARISON_LESS;
            }
            else
            {
                sCompareResult = DTL_COMPARISON_EQUAL;
            }
        }
    }

    return sCompareResult;
}

knlFxColumnDesc gSmnmpbVerifyStructureColumnDesc[] =
{
    {
        "PHYSICAL_ID",
        "segment physical indentifier",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smnmpbFxVerifyStructure, mPhysicalId ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "RESULT",
        "page indentifier",
        KNL_FXTABLE_DATATYPE_BOOL,
        STL_OFFSETOF( smnmpbFxVerifyStructure, mResult ),
        STL_SIZEOF( stlBool ),
        STL_FALSE  /* nullable */
    },
    {
        NULL,
        NULL,
        0,
        0,
        0,
        STL_FALSE
    }
};

stlStatus smnmpbOpenVerifyStructureCallback( void   * aStmt,
                                             void   * aDumpObjHandle,
                                             void   * aPathCtrl,
                                             knlEnv * aEnv )
{
    smnmpbFxVerifyStructureIterator * sIterator;

    sIterator = (smnmpbFxVerifyStructureIterator*)aPathCtrl;

    stlMemset( sIterator, 0x00, STL_SIZEOF(smnmpbFxVerifyStructureIterator) );
    
    sIterator->mTbsId = smsGetTbsId( SME_GET_SEGMENT_HANDLE(aDumpObjHandle) );
    sIterator->mWorkDone = STL_FALSE;

    return STL_SUCCESS;
}

stlStatus smnmpbCloseVerifyStructureCallback( void   * aDumpObjHandle,
                                              void   * aPathCtrl,
                                              knlEnv * aEnv )
{
    return STL_SUCCESS;
}


stlStatus smnmpbBuildRecordVerifyStructureCallback( void              * aDumpObjHandle,
                                                    void              * aPathCtrl,
                                                    knlValueBlockList * aValueList,
                                                    stlInt32            aBlockIdx,
                                                    stlBool           * aTupleExist,
                                                    knlEnv            * aEnv )
{
    smnmpbFxVerifyStructureIterator * sIterator;
    smlPid                            sPageId;
    smnmpbFxVerifyStructure           sRecord;

    *aTupleExist = STL_FALSE;
    
    sIterator = (smnmpbFxVerifyStructureIterator*)aPathCtrl;

    STL_TRY_THROW( sIterator->mWorkDone == STL_FALSE, RAMP_FINISH );

    sPageId = SMN_GET_ROOT_PAGE_ID(aDumpObjHandle);

    sRecord.mPhysicalId = smsGetSegmentId( SME_GET_SEGMENT_HANDLE(aDumpObjHandle) );
    sRecord.mResult = STL_TRUE;
    
    STL_TRY( smnmpbVerifyStructureTree( sIterator,
                                        sPageId,
                                        0,
                                        SMP_NULL_PID,  /* aPidPa */
                                        NULL,          /* aPidCn */
                                        &sRecord.mResult,
                                        SML_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( knlBuildFxRecord( gSmnmpbVerifyStructureColumnDesc,
                               &sRecord,
                               aValueList,
                               aBlockIdx,
                               aEnv )
             == STL_SUCCESS );
    
    *aTupleExist = STL_TRUE;
    sIterator->mWorkDone = STL_TRUE;

    STL_RAMP( RAMP_FINISH );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * 인덱스 구조 검증을 위해서 아래의 2가지 조건을 검사한다.
 * 
 * P : 부모 노드
 * C : 자식 노드
 * n : Next Page Identifier
 * p : Previous Page Identifier
 * 
 * (1) C(n) == P(i+1)
 * (2) C(p) == P(i-1)
 */

knlFxTableDesc gSmnmpbVerifyStructureTableDesc =
{
    KNL_FXTABLE_USAGE_DUMP_TABLE,
    KNL_STARTUP_PHASE_OPEN,
    smnmpbGetDumpObjectCallback,
    smnmpbOpenVerifyStructureCallback,
    smnmpbCloseVerifyStructureCallback,
    smnmpbBuildRecordVerifyStructureCallback,
    STL_SIZEOF( smnmpbFxVerifyStructureIterator ),
    "D$INDEX_VERIFY_STRUCTURE",
    "verify index structure",
    gSmnmpbVerifyStructureColumnDesc
};

stlStatus smnmpbVerifyStructureTree( smnmpbFxVerifyStructureIterator * aIterator,
                                     smlPid                            aPageId,
                                     stlUInt16                         aDepth,
                                     smlPid                            aPidPa,
                                     smlPid                          * aPidCn,
                                     stlBool                         * aResult,
                                     smlEnv                          * aEnv )
{
    smlPid                        sPageId;
    smpHandle                     sPageHandle;
    smnmpbFxVerifyStructureItem * sStackItem;
    smlPid                        sPrevPid;
    smlPid                        sNextPid;
    stlBool                       sIsLeaf;
    stlBool                       sIsSemiLeaf;
    stlInt32                      i;
    stlChar                     * sKey;
    stlInt16                      sKeyCount;
    stlInt32                      sState = 0;
        
    STL_TRY( knlCheckQueryTimeout( KNL_ENV(aEnv) ) == STL_SUCCESS );
    
    sStackItem = &(aIterator->mStack[aDepth]);
    
    STL_TRY( smpAcquire( NULL,
                         aIterator->mTbsId,
                         aPageId,
                         KNL_LATCH_MODE_SHARED,
                         &sPageHandle,
                         SML_ENV(aEnv) )
             == STL_SUCCESS );
    sState = 1;

    if( smpGetFreeness( &sPageHandle ) == SMP_FREENESS_FREE )
    {
        /* 현재 index tree가 SMO로 변경 중이라서 정확한 validation을 할 수 없다 */
        sState = 0;
        STL_TRY( smpRelease( &sPageHandle, aEnv ) == STL_SUCCESS );
        *aResult = STL_FALSE;
        STL_THROW( ERR_MUTATING_OBJECT );
    }

    stlMemcpy( sStackItem->mPageBuf,
               SMP_FRAME( &sPageHandle ),
               SMP_PAGE_SIZE );

    sState = 0;
    STL_TRY( smpRelease( &sPageHandle, aEnv ) == STL_SUCCESS );

    SMP_MAKE_DUMMY_HANDLE( &sPageHandle, sStackItem->mPageBuf );
    
    sKeyCount = SMP_GET_PAGE_ROW_ITEM_COUNT( &sPageHandle );
    sIsLeaf = SMNMPB_IS_LEAF( &sPageHandle );
    sIsSemiLeaf = SMNMPB_IS_SEMI_LEAF( &sPageHandle );

    sPrevPid = SMNMPB_GET_PREV_PAGE( &sPageHandle );
    sNextPid = SMNMPB_GET_NEXT_PAGE( &sPageHandle );

    if( sIsLeaf == STL_TRUE )
    {
        if( aPidPa != sPrevPid )
        {
            STL_TRY( knlLogMsg( NULL,
                                KNL_ENV(aEnv),
                                KNL_LOG_LEVEL_INFO,
                                "CORRUPTED PAGE [ CORRUPTED_PID(%d), EXPECTED_PREV_PID(%d), "
                                "ACTUAL_PREV_PID(%d), CAUSE(invalid previous page identifier) ]\n",
                                aPageId,
                                aPidPa,
                                sPrevPid )
                     == STL_SUCCESS );
            
            *aResult = STL_FALSE;
        }

        if( aPidCn != NULL )
        {
            *aPidCn = sNextPid;
        }
    }
    else
    {
        for( i = 0; i < sKeyCount; i++ )
        {
            sKey = smpGetNthRowItem( &sPageHandle, i );

            SMNMPB_INTL_SLOT_GET_CHILD_PID( sKey, &sPageId );
            
            if( (sIsSemiLeaf == STL_TRUE) &&
                (sStackItem->mPrevCn != SMP_NULL_PID) &&
                (sStackItem->mPrevCn != sPageId) )
            {
                STL_TRY( knlLogMsg( NULL,
                                    KNL_ENV(aEnv),
                                    KNL_LOG_LEVEL_INFO,
                                    "CORRUPTED PAGE [ CORRUPTED_PID(%d), EXPECTED_NEXT_PID(%d), "
                                    "ACTUAL_NEXT_PID(%d), CAUSE(invalid next page identifier) ]\n",
                                    sStackItem->mPrevC,
                                    sPageId,
                                    sStackItem->mPrevCn )
                         == STL_SUCCESS );
            
                *aResult = STL_FALSE;
            }
            
            STL_TRY( smnmpbVerifyStructureTree( aIterator,
                                                sPageId,
                                                aDepth + 1,
                                                sStackItem->mPrevC,     /* P(a) */
                                                &sStackItem->mPrevCn,   /* C(n) */
                                                aResult,
                                                aEnv )
                     == STL_SUCCESS );

            if( (SMNMPB_IS_LAST_SIBLING(&sPageHandle) == STL_TRUE) &&
                ((i + 1) == sKeyCount) &&
                (sStackItem->mPrevCn != SMP_NULL_PID) )
            {
                STL_TRY( knlLogMsg( NULL,
                                    KNL_ENV(aEnv),
                                    KNL_LOG_LEVEL_INFO,
                                    "CORRUPTED PAGE [ CORRUPTED_PID(%d), EXPECTED_NEXT_PID(NULL), "
                                    "ACTUAL_NEXT_PID(%d), CAUSE(invalid max page) ]\n",
                                    sPageId,
                                    sStackItem->mPrevCn )
                         == STL_SUCCESS );
            
                *aResult = STL_FALSE;
            }

            sStackItem->mPrevC = sPageId;
        }
    }

    return STL_SUCCESS;

    STL_CATCH( ERR_MUTATING_OBJECT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_MUTATING_DUMP_OBJECT,
                      NULL,
                      KNL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smpRelease( &sPageHandle, aEnv );
    }
    
    return STL_FAILURE;
}

/** @} */
