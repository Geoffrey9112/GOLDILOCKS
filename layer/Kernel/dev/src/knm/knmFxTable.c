/*******************************************************************************
 * knmFxTable.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: knmFxTable.c 5797 2012-09-27 02:32:51Z jhkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file knmFxTable.c
 * @brief Kernel Layer Memory Fixed Table Internal Routines
 */

#include <stl.h>
#include <dtl.h>
#include <knl.h>
#include <knlDef.h>
#include <knDef.h>
#include <knmDynamic.h>

extern knsEntryPoint    * gKnEntryPoint;
extern knlAllocatorDesc   gKnmAllocatorDesc[];

/**
 * @addtogroup knmFxTable
 * @{
 */

/**
 * @brief D$DUMP_MEMORY Table 정의
 */
knlFxColumnDesc gKnDumpMemoryColumnDesc[] =
{
    {
        "START_ADDR",
        "start address",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( knmDumpMemoryFxRecord, mStartAddr ),
        KNM_ADDR_LENGTH,
        STL_FALSE  /* nullable */
    },
    {
        "BIN_DATA",
        "dumped memory data",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( knmDumpMemoryFxRecord, mBinData ),
        KNM_DUMP_DATA_LENGTH * 2,
        STL_FALSE  /* nullable */
    },
    {
        "TEXT_DATA",
        "dumped memory data",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( knmDumpMemoryFxRecord, mTextData ),
        KNM_DUMP_DATA_LENGTH,
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

stlStatus knmGetDumpObjectCallback( stlInt64   aTransID,
                                    void     * aStmt,
                                    stlChar  * aDumpObjName,
                                    void    ** aDumpObjHandle,
                                    knlEnv   * aEnv )
{
    /* argument 전달을 위해 메모리를 할당받기가 뭐해서 */
    /* open때 파싱하기 위해 그대로 전달함              */
    *aDumpObjHandle = (void*)aDumpObjName;

    return STL_SUCCESS;
}


/**
 * @brief D$DUMP_MEMORY Table의 open 함수
 * @param[in] aStmt Statement Pointer
 * @param[in] aDumpObjHandle Dump Object의 Handle
 * @param[in] aPathCtrl Path Controller
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus knmOpenFxDumpMemoryCallback( void   * aStmt,
                                       void   * aDumpObjHandle,
                                       void   * aPathCtrl,
                                       knlEnv * aEnv )
{
    stlChar  * sToken1;
    stlChar  * sToken2;
    stlChar  * sToken3;
    stlInt32   sTokenCount;
    stlInt32   sStrLen;
    stlInt64   sStartAddr;
    stlInt32   i;
    stlChar  * sCurPtr = (stlChar*)aDumpObjHandle;
    stlChar  * sEndPtr;

    knmDumpMemoryPathCtrl * sPathCtrl = (knmDumpMemoryPathCtrl*)aPathCtrl;

    STL_TRY( knlTokenizeNextDumpOption( &sCurPtr,
                                        &sToken1,
                                        &sToken2,
                                        &sToken3,
                                        &sTokenCount,
                                        aEnv ) == STL_SUCCESS );

    STL_TRY_THROW( (sTokenCount == 1) && (sToken1[0] == '0') && (sToken1[1] == 'x'),
                   RAMP_ERR_INVALID_ARGUMENT );

    sStrLen = stlStrlen(sToken1);
    sStartAddr = 0;

    for( i = 2; i < sStrLen; i++ )
    {
        sStartAddr *= 16;
        if( (sToken1[i] >= '0') && (sToken1[i] <= '9') )
        {
            sStartAddr += sToken1[i] - '0';
        }
        else if( (sToken1[i] >= 'a') && (sToken1[i] <= 'f') )
        {
            sStartAddr += 10 + sToken1[i] - 'a';
        }
        else if( (sToken1[i] >= 'A') && (sToken1[i] <= 'F') )
        {
            sStartAddr += 10 + sToken1[i] - 'A';
        }
        else
        {
            STL_THROW( RAMP_ERR_INVALID_ARGUMENT );
        }
    }

    sPathCtrl->mStartAddr = (void*)sStartAddr;

    STL_TRY( knlTokenizeNextDumpOption( &sCurPtr,
                                        &sToken1,
                                        &sToken2,
                                        &sToken3,
                                        &sTokenCount,
                                        aEnv ) == STL_SUCCESS );

    STL_TRY_THROW( sTokenCount == 1, RAMP_ERR_INVALID_ARGUMENT );

    STL_TRY( stlStrToUInt64( sToken1,
                             stlStrlen(sToken1),
                             &sEndPtr,
                             10,
                             &(sPathCtrl->mSize),
                             KNL_ERROR_STACK(aEnv) ) == STL_SUCCESS );

    sPathCtrl->mCurAddr = (stlChar*)(sPathCtrl->mStartAddr);
    sPathCtrl->mRemainSize = sPathCtrl->mSize;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_ARGUMENT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_INVALID_ARGUMENT,
                      NULL,
                      KNL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief D$DUMP_MEMORY Table의 close 함수
 * @param[in] aDumpObjHandle Dump Object의 Handle
 * @param[in] aPathCtrl Path Controller
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus knmCloseFxDumpMemoryCallback( void   * aDumpObjHandle,
                                        void   * aPathCtrl,
                                        knlEnv * aEnv )
{
    return STL_SUCCESS;
}

/**
 * @brief D$DUMP_MEMORY Table의 레코드 구축 함수
 * @param[in] aDumpObjHandle Dump Object Handle
 * @param[in] aPathCtrl Path Controller
 * @param[in,out] aValueList 값을 담을 공간
 * @param[in]  aBlockIdx   Block Idx
 * @param[out] aTupleExist 레코드 존재 여부
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus knmBuildRecordFxDumpMemoryCallback( void              * aDumpObjHandle,
                                              void              * aPathCtrl,
                                              knlValueBlockList * aValueList,
                                              stlInt32            aBlockIdx,
                                              stlBool           * aTupleExist,
                                              knlEnv            * aEnv )
{
    knmDumpMemoryPathCtrl * sPathCtrl = (knmDumpMemoryPathCtrl*)aPathCtrl;
    knmDumpMemoryFxRecord   sRec;
    stlInt64                sPrintSize;
    stlInt32                sAddrSize;

    *aTupleExist = STL_FALSE;

    if( sPathCtrl->mRemainSize > 0 )
    {
        if( sPathCtrl->mRemainSize >= KNM_DUMP_DATA_LENGTH )
        {
            sPrintSize = KNM_DUMP_DATA_LENGTH;
        }
        else
        {
            sPrintSize = sPathCtrl->mRemainSize;
        }

        sAddrSize = stlSnprintf( sRec.mStartAddr,
                                 KNM_ADDR_LENGTH,
                                 "0x%p",
                                 sPathCtrl->mCurAddr );
        sRec.mStartAddr[sAddrSize] = '\0';


        STL_TRY( stlBinary2HexString( (stlUInt8*)(sPathCtrl->mCurAddr),
                                      sPrintSize,
                                      sRec.mBinData,
                                      (KNM_DUMP_DATA_LENGTH * 2) + 1,
                                      KNL_ERROR_STACK(aEnv) ) == STL_SUCCESS );

        STL_TRY( stlBinary2HexString( (stlUInt8*)(sPathCtrl->mCurAddr),
                                      sPrintSize,
                                      sRec.mTextData,
                                      KNM_DUMP_DATA_LENGTH + 1,
                                      KNL_ERROR_STACK(aEnv) ) == STL_SUCCESS );
        
        STL_TRY( knlBuildFxRecord( gKnDumpMemoryColumnDesc,
                                   &sRec,
                                   aValueList,
                                   aBlockIdx,
                                   aEnv )
                 == STL_SUCCESS );

        *aTupleExist = STL_TRUE;

        sPathCtrl->mCurAddr += sPrintSize;
        sPathCtrl->mRemainSize -= sPrintSize;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief D$DUMP_MEMORY Table 정보
 */
knlFxTableDesc gKnDumpMemoryTableDesc =
{
    KNL_FXTABLE_USAGE_DUMP_TABLE,
    KNL_STARTUP_PHASE_NO_MOUNT,
    knmGetDumpObjectCallback,
    knmOpenFxDumpMemoryCallback,
    knmCloseFxDumpMemoryCallback,
    knmBuildRecordFxDumpMemoryCallback,
    STL_SIZEOF( knmDumpMemoryPathCtrl ),
    "D$DUMP_MEMORY",
    "memory dump table",
    gKnDumpMemoryColumnDesc
};


/**
 * @brief X$DYNAMIC_MEM Table 정의
 */
knlFxColumnDesc gKnDynamicMemColumnDesc[] =
{
    {
        "SEQ_ID",
        "sequence id of database level allocators",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( knmFxDynamicMem, mSeqId ),
        STL_SIZEOF(stlInt64),
        STL_FALSE  /* nullable */
    },
    {
        "ALLOCATOR_ID",
        "allocator id",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( knmFxDynamicMem, mAllocatorId ),
        STL_SIZEOF(stlInt64),
        STL_FALSE  /* nullable */
    },
    {
        "ADDRESS",
        "allocator address",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( knmFxDynamicMem, mAddr ),
        STL_SIZEOF(stlInt64),
        STL_FALSE  /* nullable */
    },
    {
        "INIT_SIZE",
        "initial size",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( knmFxDynamicMem, mInitSize ),
        STL_SIZEOF(stlInt64),
        STL_FALSE  /* nullable */
    },
    {
        "NEXT_SIZE",
        "next size",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( knmFxDynamicMem, mNextSize ),
        STL_SIZEOF(stlInt64),
        STL_FALSE  /* nullable */
    },
    {
        "MIN_FRAG_SIZE",
        "minimum fragment size",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( knmFxDynamicMem, mMinFragSize ),
        STL_SIZEOF(stlInt64),
        STL_FALSE  /* nullable */
    },
    {
        "TOTAL_SIZE",
        "total size",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( knmFxDynamicMem, mTotalSize ),
        STL_SIZEOF(stlInt64),
        STL_FALSE  /* nullable */
    },
    {
        "USED_SIZE",
        "used size",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( knmFxDynamicMem, mUsedSize ),
        STL_SIZEOF(stlInt64),
        STL_FALSE  /* nullable */
    },
    {
        "DESC",
        "description",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( knmFxDynamicMem, mDesc ),
        KNL_ALLOCATOR_MAX_DESC_SIZE + 1,
        STL_TRUE  /* nullable */
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


/**
 * @brief X$DYNAMIC_MEM Table의 open 함수
 * @param[in] aStmt Statement Pointer
 * @param[in] aDumpObjHandle Dump Object의 Handle
 * @param[in] aPathCtrl Path Controller
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus knmOpenFxDynamicMemCallback( void   * aStmt,
                                       void   * aDumpObjHandle,
                                       void   * aPathCtrl,
                                       knlEnv * aEnv )
{
    stlInt32 sArrayIdx = 0;
    
    stlMemcpy( aPathCtrl, &sArrayIdx, STL_SIZEOF(stlInt32) );

    return STL_SUCCESS;
}

/**
 * @brief X$DYNAMIC_MEM Table의 close 함수
 * @param[in] aDumpObjHandle Dump Object의 Handle
 * @param[in] aPathCtrl Path Controller
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus knmCloseFxDynamicMemCallback( void   * aDumpObjHandle,
                                        void   * aPathCtrl,
                                        knlEnv * aEnv )
{
    return STL_SUCCESS;
}

/**
 * @brief X$DYNAMIC_MEM Table의 레코드 구축 함수
 * @param[in] aDumpObjHandle Dump Object Handle
 * @param[in] aPathCtrl Path Controller
 * @param[in,out] aValueList 값을 담을 공간
 * @param[in]  aBlockIdx   Block Idx
 * @param[out] aTupleExist 레코드 존재 여부
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus knmBuildRecordFxDynamicMemCallback( void              * aDumpObjHandle,
                                              void              * aPathCtrl,
                                              knlValueBlockList * aValueList,
                                              stlInt32            aBlockIdx,
                                              stlBool           * aTupleExist,
                                              knlEnv            * aEnv )
{
    stlInt32          sArrayIdx;
    knmFxDynamicMem   sFxRecord;
    knlDynamicMem   * sDynamicMem;

    *aTupleExist = STL_FALSE;

    stlMemcpy( &sArrayIdx, aPathCtrl, STL_SIZEOF(stlInt32) );

    if( sArrayIdx < gKnEntryPoint->mDynamicArrayCount )
    {
        sDynamicMem = gKnEntryPoint->mDynamicMemArray[sArrayIdx];

        sFxRecord.mAddr = (stlUInt64)sDynamicMem;
        sFxRecord.mSeqId = sArrayIdx;
        sFxRecord.mAllocatorId = sDynamicMem->mAllocatorId;
        sFxRecord.mInitSize = sDynamicMem->mInitSize;
        sFxRecord.mNextSize = sDynamicMem->mNextSize;
        sFxRecord.mMinFragSize = sDynamicMem->mMinFragSize;

        STL_TRY( knmAnalyzeDynamicMem( sDynamicMem,
                                       &sFxRecord.mTotalSize,
                                       &sFxRecord.mUsedSize,
                                       aEnv )
                 == STL_SUCCESS );

        stlStrncpy( sFxRecord.mDesc,
                    gKnmAllocatorDesc[sDynamicMem->mAllocatorId].mDescription,
                    KNL_ALLOCATOR_MAX_DESC_SIZE );
        
        *aTupleExist = STL_TRUE;

        STL_TRY( knlBuildFxRecord( gKnDynamicMemColumnDesc,
                                   &sFxRecord,
                                   aValueList,
                                   aBlockIdx,
                                   aEnv )
                 == STL_SUCCESS );

        sArrayIdx += 1;
        stlMemcpy( aPathCtrl, &sArrayIdx, STL_SIZEOF(stlInt32) );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief X$DYNAMIC_MEM Table 정보
 */
knlFxTableDesc gKnDynamicMemTableDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,
    KNL_STARTUP_PHASE_NO_MOUNT,
    NULL,
    knmOpenFxDynamicMemCallback,
    knmCloseFxDynamicMemCallback,
    knmBuildRecordFxDynamicMemCallback,
    STL_SIZEOF( stlInt32 ),
    "X$DYNAMIC_MEM",
    "show dynamic memory allocators",
    gKnDynamicMemColumnDesc
};


/**
 * @brief X$ALLOCATOR Table 정의
 */
knlFxColumnDesc gKnAllocatorColumnDesc[] =
{
    {
        "ALLOCATOR_ID",
        "allocator id",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( knmFxAllocatorDesc, mId ),
        STL_SIZEOF(stlInt64),
        STL_FALSE  /* nullable */
    },
    {
        "ALLOCATOR_TYPE",
        "allocator type",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( knmFxAllocatorDesc, mAllocatorType ),
        STL_SIZEOF(stlInt32),
        STL_FALSE  /* nullable */
    },
    {
        "STORAGE_TYPE",
        "storage type of allocator",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( knmFxAllocatorDesc, mStorageType ),
        STL_SIZEOF(stlInt32),
        STL_FALSE  /* nullable */
    },
    {
        "ATTRIBUTE",
        "attribute of allocator",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( knmFxAllocatorDesc, mMemAttr ),
        STL_SIZEOF(stlInt32),
        STL_FALSE  /* nullable */
    },
    {
        "UNMARK_TYPE",
        "unmark type of allocator",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( knmFxAllocatorDesc, mUnmarkType ),
        STL_SIZEOF(stlInt32),
        STL_FALSE  /* nullable */
    },
    {
        "ALLOCATOR_LEVEL",
        "allocator level",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( knmFxAllocatorDesc, mAllocatorLevel ),
        STL_SIZEOF(stlInt32),
        STL_FALSE  /* nullable */
    },
    {
        "ALLOCATOR_CATEGORY",
        "allocator category",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( knmFxAllocatorDesc, mAllocatorCategory ),
        STL_SIZEOF(stlInt32),
        STL_FALSE  /* nullable */
    },
    {
        "MINIMUM_FRAGMENT_SIZE",
        "minimum fragment size",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( knmFxAllocatorDesc, mMinFragSize ),
        STL_SIZEOF(stlInt32),
        STL_FALSE  /* nullable */
    },
    {
        "ALLOCATOR_LAYER",
        "top layer of allocator",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( knmFxAllocatorDesc, mLayer ),
        STL_SIZEOF(stlInt32),
        STL_FALSE  /* nullable */
    },
    {
        "DESC",
        "description",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( knmFxAllocatorDesc, mDesc ),
        KNL_ALLOCATOR_MAX_DESC_SIZE + 1,
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


/**
 * @brief X$ALLOCATOR Table의 open 함수
 * @param[in] aStmt Statement Pointer
 * @param[in] aDumpObjHandle Dump Object의 Handle
 * @param[in] aPathCtrl Path Controller
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus knmOpenFxAllocatorCallback( void   * aStmt,
                                      void   * aDumpObjHandle,
                                      void   * aPathCtrl,
                                      knlEnv * aEnv )
{
    stlInt32 sArrayIdx = 0;
    
    stlMemcpy( aPathCtrl, &sArrayIdx, STL_SIZEOF(stlInt32) );

    return STL_SUCCESS;
}

/**
 * @brief X$ALLOCATOR Table의 close 함수
 * @param[in] aDumpObjHandle Dump Object의 Handle
 * @param[in] aPathCtrl Path Controller
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus knmCloseFxAllocatorCallback( void   * aDumpObjHandle,
                                       void   * aPathCtrl,
                                       knlEnv * aEnv )
{
    return STL_SUCCESS;
}

/**
 * @brief X$ALLOCATOR Table의 레코드 구축 함수
 * @param[in] aDumpObjHandle Dump Object Handle
 * @param[in] aPathCtrl Path Controller
 * @param[in,out] aValueList 값을 담을 공간
 * @param[in]  aBlockIdx   Block Idx
 * @param[out] aTupleExist 레코드 존재 여부
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus knmBuildRecordFxAllocatorCallback( void              * aDumpObjHandle,
                                             void              * aPathCtrl,
                                             knlValueBlockList * aValueList,
                                             stlInt32            aBlockIdx,
                                             stlBool           * aTupleExist,
                                             knlEnv            * aEnv )
{
    stlInt32             sArrayIdx;
    knmFxAllocatorDesc   sFxRecord;
    knlAllocatorDesc   * sAllocatorDesc;

    *aTupleExist = STL_FALSE;

    stlMemcpy( &sArrayIdx, aPathCtrl, STL_SIZEOF(stlInt32) );

    if( gKnmAllocatorDesc[sArrayIdx].mDescription != NULL )
    {
        sAllocatorDesc = &(gKnmAllocatorDesc[sArrayIdx]);

        STL_DASSERT( sArrayIdx == sAllocatorDesc->mAllocatorId );
        
        sFxRecord.mId = sAllocatorDesc->mAllocatorId;
        sFxRecord.mAllocatorType = sAllocatorDesc->mAllocatorType;
        sFxRecord.mStorageType = sAllocatorDesc->mStorageType;
        sFxRecord.mMemAttr = sAllocatorDesc->mMemAttr;
        sFxRecord.mUnmarkType = sAllocatorDesc->mUnmarkType;
        sFxRecord.mAllocatorLevel = sAllocatorDesc->mAllocatorLevel;
        sFxRecord.mAllocatorCategory = sAllocatorDesc->mAllocatorCategory;
        sFxRecord.mMinFragSize = sAllocatorDesc->mMinFragSize;
        sFxRecord.mLayer = sAllocatorDesc->mLayer;
        
        stlStrncpy( sFxRecord.mDesc,
                    sAllocatorDesc->mDescription,
                    KNL_ALLOCATOR_MAX_DESC_SIZE );
        
        *aTupleExist = STL_TRUE;

        STL_TRY( knlBuildFxRecord( gKnAllocatorColumnDesc,
                                   &sFxRecord,
                                   aValueList,
                                   aBlockIdx,
                                   aEnv )
                 == STL_SUCCESS );

        sArrayIdx += 1;
        stlMemcpy( aPathCtrl, &sArrayIdx, STL_SIZEOF(stlInt32) );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief X$ALLOCATOR Table 정보
 */
knlFxTableDesc gKnAllocatorTableDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,
    KNL_STARTUP_PHASE_NO_MOUNT,
    NULL,
    knmOpenFxAllocatorCallback,
    knmCloseFxAllocatorCallback,
    knmBuildRecordFxAllocatorCallback,
    STL_SIZEOF( stlInt32 ),
    "X$ALLOCATOR",
    "show description of allocators",
    gKnAllocatorColumnDesc
};


/**
 * @brief D$DYNAMIC_MEM Table 정의
 */
knlFxColumnDesc gKnDumpDynamicMemColumnDesc[] =
{
    {
        "CHUNK_ID",
        "chunk identifier",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( knmFxDumpDynamicMem, mChunkId ),
        STL_SIZEOF(stlInt64),
        STL_FALSE  /* nullable */
    },
    {
        "BLOCK_ID",
        "block identifier",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( knmFxDumpDynamicMem, mBlockId ),
        STL_SIZEOF(stlInt64),
        STL_FALSE  /* nullable */
    },
    {
        "BLOCK_ADDR",
        "block address",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( knmFxDumpDynamicMem, mBlockAddr ),
        KNM_ADDR_LENGTH,
        STL_FALSE  /* nullable */
    },
    {
        "SIZE",
        "block size",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( knmFxDumpDynamicMem, mSize ),
        STL_SIZEOF(stlInt64),
        STL_FALSE  /* nullable */
    },
    {
        "IS_USED",
        "is used",
        KNL_FXTABLE_DATATYPE_BOOL,
        STL_OFFSETOF( knmFxDumpDynamicMem, mIsUsed ),
        STL_SIZEOF(stlBool),
        STL_FALSE  /* nullable */
    },
    {
        "REQUESTOR_ADDRESS",
        "address of request allocator",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( knmFxDumpDynamicMem, mRequestorAddress ),
        STL_SIZEOF(stlInt64),
        STL_FALSE  /* nullable */
    },
    {
        "REQUESTOR_ID",
        "identifier of request allocator",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( knmFxDumpDynamicMem, mRequestorId ),
        STL_SIZEOF(stlInt32),
        STL_FALSE  /* nullable */
    },
    {
        "REQUESTOR_SESSION_ID",
        "session identifier of request allocator",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( knmFxDumpDynamicMem, mRequestorSessionId ),
        STL_SIZEOF(stlInt32),
        STL_FALSE  /* nullable */
    },
    {
        "REQUESTOR_TOTAL_SIZE",
        "total used size of request allocator",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( knmFxDumpDynamicMem, mRequestorTotalSize ),
        STL_SIZEOF(stlInt64),
        STL_FALSE  /* nullable */
    },
    {
        "CALLSTACK",
        "callstack",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( knmFxDumpDynamicMem, mCallstack ),
        KNM_CALLSTCK_STRING_SIZE,
        STL_TRUE  /* nullable */
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

stlStatus knmGetDumpDynamicMemObjectCallback( stlInt64   aTransID,
                                              void     * aStmt,
                                              stlChar  * aDumpObjName,
                                              void    ** aDumpObjHandle,
                                              knlEnv   * aEnv )
{
    stlChar  * sToken1;
    stlChar  * sToken2;
    stlChar  * sToken3;
    stlInt32   sTokenCount;
    stlChar  * sEndPtr;
    stlInt64   sArrayIdx = 0;

    *aDumpObjHandle = NULL;

    STL_TRY( knlTokenizeNextDumpOption( &aDumpObjName,
                                        &sToken1,
                                        &sToken2,
                                        &sToken3,
                                        &sTokenCount,
                                        aEnv ) == STL_SUCCESS );

    if( sTokenCount == 1 )
    {
        if( stlStrToInt64( sToken1,
                           STL_NTS,
                           &sEndPtr,
                           10,
                           &sArrayIdx,
                           KNL_ERROR_STACK(aEnv) )
            == STL_FAILURE )
        {
            stlPopError( KNL_ERROR_STACK(aEnv) );
            STL_THROW( RAMP_FINISH );
        }

        STL_TRY_THROW( gKnEntryPoint->mDynamicArrayCount > sArrayIdx, RAMP_FINISH );
        
        *aDumpObjHandle = (void*)(gKnEntryPoint->mDynamicMemArray[sArrayIdx]);
    }

    STL_RAMP( RAMP_FINISH );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief D$DYNAMIC_MEM Table의 open 함수
 * @param[in] aStmt Statement Pointer
 * @param[in] aDumpObjHandle Dump Object의 Handle
 * @param[in] aPathCtrl Path Controller
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus knmOpenFxDumpDynamicMemCallback( void   * aStmt,
                                           void   * aDumpObjHandle,
                                           void   * aPathCtrl,
                                           knlEnv * aEnv )
{
    knlDynamicMem            * sDynamicMem = (knlDynamicMem*)aDumpObjHandle;
    stlBool                    sIsTimedout;
    knmFxDumpDynamicPathCtrl * sPathCtrl = (knmFxDumpDynamicPathCtrl *)aPathCtrl;
    stlInt32                   sState = 0;

    STL_TRY( knlAcquireLatch( &(sDynamicMem->mLatch),
                              KNL_LATCH_MODE_SHARED,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sIsTimedout,
                              aEnv ) == STL_SUCCESS );
    sState = 1;

    sPathCtrl->mCurrChunk = STL_RING_GET_FIRST_DATA( &(sDynamicMem->mChunkRing) );
    sPathCtrl->mCurrBlock = STL_RING_GET_FIRST_DATA( &(sPathCtrl->mCurrChunk->mAllocRing) );
    sPathCtrl->mCurrChunkId = 0;
    sPathCtrl->mCurrBlockId = 0;
    sPathCtrl->mValidSeq = knlGetExclLockSeq( &(sDynamicMem->mLatch) );

    sState = 0;
    STL_TRY( knlReleaseLatch( &(sDynamicMem->mLatch),
                              aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void) knlReleaseLatch( &(sDynamicMem->mLatch), aEnv );
    }

    return STL_FAILURE;
}

/**
 * @brief D$DYNAMIC_MEM Table의 close 함수
 * @param[in] aDumpObjHandle Dump Object의 Handle
 * @param[in] aPathCtrl Path Controller
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus knmCloseFxDumpDynamicMemCallback( void   * aDumpObjHandle,
                                            void   * aPathCtrl,
                                            knlEnv * aEnv )
{
    return STL_SUCCESS;
}

/**
 * @brief D$DYNAMIC_MEM Table의 레코드 구축 함수
 * @param[in] aDumpObjHandle Dump Object Handle
 * @param[in] aPathCtrl Path Controller
 * @param[in,out] aValueList 값을 담을 공간
 * @param[in]  aBlockIdx   Block Idx
 * @param[out] aTupleExist 레코드 존재 여부
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus knmBuildRecordFxDumpDynamicMemCallback( void              * aDumpObjHandle,
                                                  void              * aPathCtrl,
                                                  knlValueBlockList * aValueList,
                                                  stlInt32            aBlockIdx,
                                                  stlBool           * aTupleExist,
                                                  knlEnv            * aEnv )
{
    knlDynamicMem            * sDynamicMem = (knlDynamicMem*)aDumpObjHandle;
    knmFxDumpDynamicPathCtrl * sPathCtrl = (knmFxDumpDynamicPathCtrl *)aPathCtrl;
    knmFxDumpDynamicMem        sFxRecord;
    knmBlockHeader           * sBlockHeader;
    stlInt32                   sState = 0;
    stlBool                    sIsTimedout;
    void                     * sRequestAllocator;

    *aTupleExist = STL_FALSE;

    STL_TRY( knlAcquireLatch( &(sDynamicMem->mLatch),
                              KNL_LATCH_MODE_SHARED,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,
                              &sIsTimedout,
                              aEnv ) == STL_SUCCESS );
    sState = 1;

    /**
     * Memory allocator의 변경사항 검사
     */
    STL_TRY_THROW( sPathCtrl->mValidSeq == knlGetExclLockSeq( &(sDynamicMem->mLatch) ),
                   RAMP_ERR_MODIFIED_OBJECT );
    
    /**
     * search block
     */
    while( STL_RING_IS_HEADLINK( &(sPathCtrl->mCurrChunk->mAllocRing),
                                 sPathCtrl->mCurrBlock )
           == STL_TRUE )
    {
        sPathCtrl->mCurrChunk = STL_RING_GET_NEXT_DATA( &(sDynamicMem->mChunkRing),
                                                        sPathCtrl->mCurrChunk );

        STL_TRY_THROW( STL_RING_IS_HEADLINK( &(sDynamicMem->mChunkRing),
                                             sPathCtrl->mCurrChunk ) == STL_FALSE,
                       RAMP_FINISH );
        sPathCtrl->mCurrChunkId += 1;

        sPathCtrl->mCurrBlock = STL_RING_GET_FIRST_DATA( &(sPathCtrl->mCurrChunk->mAllocRing) );
        sPathCtrl->mCurrBlockId = 0;
    }

    /**
     * build record
     */
    sBlockHeader = sPathCtrl->mCurrBlock;
    
    sFxRecord.mChunkId = sPathCtrl->mCurrChunkId;
    sFxRecord.mBlockId = sPathCtrl->mCurrBlockId;
    sFxRecord.mSize = sBlockHeader->mSize;
    sFxRecord.mIsUsed = sBlockHeader->mIsUsed;
    sFxRecord.mCallstack[0] = '\0';

    sRequestAllocator = sBlockHeader->mRequestorAddress;
    
    sFxRecord.mRequestorAddress = (stlUInt64)(sRequestAllocator);
    sFxRecord.mRequestorId = (stlUInt64)(sBlockHeader->mRequestorId);
    sFxRecord.mRequestorSessionId = sBlockHeader->mRequestorSessionId;
    sFxRecord.mRequestorTotalSize = 0;

    if( knlIsValidStaticAddress( sRequestAllocator,
                                 STL_MAX(STL_SIZEOF(knlRegionMem),
                                         STL_SIZEOF(knlDynamicMem)) )
        == STL_TRUE )
    {
        if( (sFxRecord.mRequestorId >= 0) &&
            (sFxRecord.mRequestorId < KNL_ALLOCATOR_MAX) )
        {
            if( gKnmAllocatorDesc[sFxRecord.mRequestorId].mAllocatorType ==
                KNL_ALLOCATOR_TYPE_REGION )
            {
                sFxRecord.mRequestorTotalSize =
                    ((knlRegionMem*)(sRequestAllocator))->mTotalSize;
            }
            else
            {
                if( gKnmAllocatorDesc[sFxRecord.mRequestorId].mAllocatorType ==
                    KNL_ALLOCATOR_TYPE_DYNAMIC )
                {
                    sFxRecord.mRequestorTotalSize =
                        ((knlDynamicMem*)(sRequestAllocator))->mTotalSize;
                }
            }
        }
    }

    stlSnprintf( sFxRecord.mBlockAddr,
                 KNM_ADDR_LENGTH,
                 "%p",
                 sBlockHeader );
    
    /**
     * print callstack
     */
    if( (sBlockHeader->mIsUsed == STL_TRUE) &&
        (KNM_CHECK_MEMORY_BOUNDARY(aEnv) == STL_TRUE) )
    {
        if( ((knmHeadFence*)(sBlockHeader + 1))->mCallstackCount > 0 )
        {
            stlBacktraceSymbolsToStr( ((knmHeadFence*)(sBlockHeader + 1))->mCallstack,
                                      sFxRecord.mCallstack,
                                      ((knmHeadFence*)(sBlockHeader + 1))->mCallstackCount,
                                      KNM_CALLSTCK_STRING_SIZE );
        }
    }

    STL_TRY( knlBuildFxRecord( gKnDumpDynamicMemColumnDesc,
                               &sFxRecord,
                               aValueList,
                               aBlockIdx,
                               aEnv )
             == STL_SUCCESS );

    /**
     * move to next
     */
    sPathCtrl->mCurrBlock = STL_RING_GET_NEXT_DATA( &(sPathCtrl->mCurrChunk->mAllocRing),
                                                    sBlockHeader );
    sPathCtrl->mCurrBlockId += 1;

    *aTupleExist = STL_TRUE;
    
    STL_RAMP( RAMP_FINISH );

    sState = 0;
    STL_TRY( knlReleaseLatch( &(sDynamicMem->mLatch),
                              aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_MODIFIED_OBJECT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_MODIFIED_DUMP_OBJECT,
                      NULL,
                      KNL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    if( sState == 1 )
    {
        (void) knlReleaseLatch( &(sDynamicMem->mLatch), aEnv );
    }

    return STL_FAILURE;
}

/**
 * @brief D$DYNAMIC_MEM Table 정보
 */
knlFxTableDesc gKnDumpDynamicMemTableDesc =
{
    KNL_FXTABLE_USAGE_DUMP_TABLE,
    KNL_STARTUP_PHASE_NO_MOUNT,
    knmGetDumpDynamicMemObjectCallback,
    knmOpenFxDumpDynamicMemCallback,
    knmCloseFxDumpDynamicMemCallback,
    knmBuildRecordFxDumpDynamicMemCallback,
    STL_SIZEOF( knmFxDumpDynamicPathCtrl ),
    "D$DYNAMIC_MEM",
    "dump dynamic allocator",
    gKnDumpDynamicMemColumnDesc
};


/** @} */
