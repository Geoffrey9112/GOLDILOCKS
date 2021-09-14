/*******************************************************************************
 * smdmifTable.c
 *
 * Copyright (c) 2012, SUNJESOFT Inc.
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
 * @file smdmifTable.c
 * @brief Storage Manager Layer Memory Instant Flat Table Routines
 */

#include <sml.h>
#include <smDef.h>
#include <smg.h>
#include <smd.h>
#include <smdDef.h>
#include <smsDef.h>
#include <sms.h>
#include <smsmfDef.h>
#include <smxm.h>
#include <smp.h>
#include <smf.h>
#include <smbDef.h>
#include <smdmifTable.h>
#include <smdmifFixedPart.h>
#include <smdmifVarPart.h>
#include <smdmifBlockDir.h>
#include <smsManager.h>

/**
 * @addtogroup smdmifTable
 * @{
 */


smdTableModule gSmdmifTableModule =
{
    smdmifTableCreate,
    NULL, /* CreateIotFunc */
    NULL, /* CreateUndoFunc */
    smdmifTableDrop,
    NULL, /* CopyFunc */
    NULL, /* AddColumnFunc */
    NULL, /* DropColumnFunc */
    NULL, /* AlterTableAttrFunc */
    NULL, /* MergeTableFunc */
    NULL, /* CheckExistRowFunc */
    NULL, /* CreateForTruncateFunc */
    NULL, /* DropAgingFunc */
    NULL, /* InitializeFunc */
    NULL, /* FinalizeFunc */
    smdmifTableInsert,
    smdmifTableBlockInsert,
    smdmifTableUpdate,
    smdmifTableDelete,
    NULL, /* MergeFunc */
    NULL, /* RowCountFunc */
    smdmifTableFetch,
    NULL, /* FetchWithRowidFunc */
    NULL, /* LockRowFunc */
    NULL, /* CompareKeyValueFunc */
    NULL, /* ExtractKeyValueFunc */
    NULL, /* ExtractValidKeyValueFunc */
    NULL, /* SortFunc */
    NULL, /* GetSlotBodyFunc */
    smdmifTableTruncate,
    smdmifTableSetFlag,
    smdmifTableCleanup,
    NULL,  /* BuildCacheFunc */
    NULL   /* Fetch4Index */
};


inline stlInt32 smdmifGetBlockSize( smlTbsId aTbsId )
{
    stlInt32 sPageCount = (stlInt32)smsGetPageCountInTempTbsExt( aTbsId );

    return sPageCount * SMP_PAGE_SIZE - STL_SIZEOF( smpPhyHdr ) - STL_SIZEOF( smsmfExtHdr );
}

/* local function declaration */
static stlStatus smdmifTableAllocBlockDir( void                *aRelationHandle,
                                           void               **aBlockDir,
                                           knlLogicalAddr       aParentDirAddr,
                                           stlInt16             aLevel,
                                           smlEnv              *aEnv );

stlStatus smdmifTableInitInstantMode( smdmifTableHeader *aHeader,
                                      smlEnv            *aEnv )
{
    if( aHeader->mInstMemMaxCount > 0 )
    {
        STL_TRY( smgAllocSessShmMem( STL_SIZEOF( void * ) * aHeader->mInstMemMaxCount,
                                     (void**)&aHeader->mInstMemList,
                                     aEnv ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

static void smdmifTableResetHeaderForInsert( smdmifTableHeader *aHeader,
                                             smlEnv            *aEnv );

static stlStatus smdmifTableAllocBlockFromInstMem( smdmifTableHeader  *aHeader,
                                                   void              **aBlock,
                                                   knlLogicalAddr     *aBlockAddr,
                                                   smlEnv             *aEnv );

static stlStatus smdmifTableAllocBlockFromTbs( smdmifTableHeader  *aHeader,
                                               void              **aBlock,
                                               knlLogicalAddr     *aBlockAddr,
                                               smlEnv             *aEnv );

static stlStatus smdmifTablePrepareNextFixedBlock( smdmifTableHeader      *aHeader,
                                                   void                  **aFixedPart,
                                                   smlEnv                 *aEnv )
{
    void            *sOldFixedPart;
    void            *sNewFixedPart;
    void            *sBlockDir;
 
    /* 이전 fixed part를 구한다. */
    sOldFixedPart = SMDMIF_TABLE_GET_CUR_FIXED_PART( aHeader );

    /* fixed part로 사용할 block을 할당한다. */
    STL_TRY( smdmifTableAllocBlock( aHeader,
                                    SMDMIF_DEF_BLOCK_FIXED_PART,
                                    &sNewFixedPart,
                                    aEnv ) == STL_SUCCESS );

    /* 새로 할당받은 fixed part를 초기화한다. */
    smdmifFixedPartInit( sNewFixedPart,
                         sOldFixedPart,
                         aHeader->mBlockSize,
                         aHeader->mFixedRowLen,
                         aHeader->mFixedRowAlign );

    if( sOldFixedPart == NULL )
    {
        /* 처음 fixed part가 할당된 경우이므로 first를 세팅한다. */
        aHeader->mFirstFixedPartAddr = SMDMIF_DEF_BLOCK_ADDR( sNewFixedPart );
    }
    else
    {
        if( aHeader->mBuildBlockDir == STL_TRUE )
        {
            /*
             * 새로 생성된 block에 대해 directory를 구성한다.
             * fixed part block이 하나만 있을 경우엔 block directory를 만들지 않는다.
             * 2개째 block이 생성될 때부터 만들기 시작한다.
             */
            if( aHeader->mCurBlockDirAddr == KNL_LOGICAL_ADDR_NULL )
            {
                STL_TRY( smdmifTableAllocBlockDir( aHeader,
                                                   &sBlockDir,
                                                   KNL_LOGICAL_ADDR_NULL, /* parent block dir addr */
                                                   1,
                                                   aEnv ) == STL_SUCCESS );

                /* block directory에 처음 넣을 경우,
                   first fixed part를 먼저 dir key로 넣어야 한다. */
                STL_TRY( smdmifBlockDirInsertKey( aHeader,
                                                  sBlockDir,
                                                  aHeader->mFirstFixedPartAddr,
                                                  smdmifTableAllocBlockDir,
                                                  aEnv ) == STL_SUCCESS );
            }
            else
            {
                sBlockDir = KNL_TO_PHYSICAL_ADDR( aHeader->mCurBlockDirAddr );
            }

            STL_TRY( smdmifBlockDirInsertKey( aHeader,
                                              sBlockDir,
                                              SMDMIF_DEF_BLOCK_ADDR( sNewFixedPart ),
                                              smdmifTableAllocBlockDir,
                                              aEnv ) == STL_SUCCESS );
        }
    }

    /* current fixed part를 갱신한다. */
    aHeader->mCurFixedPartAddr = SMDMIF_DEF_BLOCK_ADDR( sNewFixedPart );

    *aFixedPart = sNewFixedPart;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

static stlStatus smdmifTableAllocBlockDir( void                *aRelationHandle,
                                           void               **aBlockDir,
                                           knlLogicalAddr       aParentDirAddr,
                                           stlInt16             aLevel,
                                           smlEnv              *aEnv )
{
    smdmifTableHeader *sHeader = (smdmifTableHeader *)aRelationHandle;
 
    STL_TRY( smdmifTableAllocBlock( sHeader,
                                    SMDMIF_DEF_BLOCK_DIR,
                                    aBlockDir,
                                    aEnv ) == STL_SUCCESS );

    smdmifBlockDirInit( *aBlockDir,
                        aParentDirAddr,
                        aLevel,
                        sHeader->mBlockSize );

    if( aLevel == 1 )
    {
        sHeader->mCurBlockDirAddr = SMDMIF_DEF_BLOCK_ADDR(*aBlockDir);
    }

    if( aParentDirAddr == KNL_LOGICAL_ADDR_NULL )
    {
        sHeader->mRootBlockDirAddr = SMDMIF_DEF_BLOCK_ADDR(*aBlockDir);
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
 
stlStatus smdmifTablePrepareNextVarBlock( void                 *aHeader,
                                          void                **aVarPart,
                                          smlEnv               *aEnv )
{
    void            *sBlock;

    /* var part로 사용할 block을 할당한다. */
    STL_TRY( smdmifTableAllocBlock( (smdmifTableHeader *)aHeader,
                                    SMDMIF_DEF_BLOCK_VAR_PART,
                                    &sBlock,
                                    aEnv ) == STL_SUCCESS );

    /* 새로 할당받은 var part header를 초기화한다. */
    SMDMIF_VAR_PART_INIT_HEADER( sBlock,
                                 ((smdmifTableHeader *)aHeader)->mBlockSize );

    /* current var part를 갱신한다. */
    ((smdmifTableHeader *)aHeader)->mCurVarPartAddr = SMDMIF_DEF_BLOCK_ADDR(sBlock);

    *aVarPart = sBlock;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

static stlStatus smdmifTableSetColInfo( smdmifDefColumn     *aCols,
                                        stlInt32             aColCount,
                                        knlValueBlockList   *aColList,
                                        stlInt32            *aFixedRowLen,
                                        stlInt32            *aFixedRowAlign,
                                        stlInt32            *aVarPartColCount,
                                        stlInt32            *aLargestColSize,
                                        smlEnv              *aEnv )
{
    knlValueBlockList *sValueBlock;
    stlInt32           i;
    stlInt32           sFixedRowLen = 0;    /* align 고려된 정확한 fixed part의 총 길이 */
    stlInt16           sFixedRowAlign = 0;
    stlInt32           sFixedColMaxLen;

    sFixedColMaxLen = (stlInt32)knlGetPropertyBigIntValueByID( KNL_PROPERTY_INST_TABLE_FIXED_COL_MAX_LEN,
                                                               KNL_ENV( aEnv ) );

    /* fixed row의 맨 처음에 해더가 들어간다. header는 align에 상관없다. */
    sFixedRowLen = SMDMIF_FIXED_ROW_HEADER_SIZE;

    sValueBlock = aColList;
    *aVarPartColCount = 0;

    for( i = 0; i < aColCount; i++ )
    {
        /* 로직엔 사용하지 않지만, D$를 위해 두 정보를 저장함 */
        aCols[i].mDataType = KNL_GET_BLOCK_DB_TYPE( sValueBlock );
        aCols[i].mAllocSize = KNL_GET_BLOCK_DATA_BUFFER_SIZE( sValueBlock );
        aCols[i].mMaxActualLen = 0; /* insert될 때 최대값이 갱신된다. */
        aCols[i].mHasNull = STL_FALSE; /* insert 과정에서 null이 한번이라도 들어가면 true로 세팅된다. */
        aCols[i].mIsFixedPart = ( aCols[i].mAllocSize <= sFixedColMaxLen ? STL_TRUE : STL_FALSE );

        if( (aCols[i].mDataType == DTL_TYPE_LONGVARCHAR) ||
            (aCols[i].mDataType == DTL_TYPE_LONGVARBINARY) )
        {
            /**
             * Long varying type은 variable part에 저장한다.
             */
            aCols[i].mIsFixedPart = STL_FALSE;
        }

        *aLargestColSize = STL_MAX( *aLargestColSize, aCols[i].mAllocSize );

        if( aCols[i].mIsFixedPart == STL_FALSE )
        {
            /* var column들이 앞쪽에 온다. var col ptr이 8바이트이기 때문이다. */
            aCols[i].mOffset = sFixedRowLen;
            sFixedRowLen += STL_SIZEOF( knlLogicalAddr );
            sFixedRowAlign = STL_SIZEOF( knlLogicalAddr );

            (*aVarPartColCount)++;
        }
        sValueBlock = sValueBlock->mNext;
    }

    /* fixed part column들의 offset을 설정한다. */
    for( i = 0; i < aColCount; i++ )
    {
        if( aCols[i].mIsFixedPart == STL_TRUE )
        {
            aCols[i].mOffset = STL_ALIGN( sFixedRowLen - SMDMIF_FIXED_ROW_HEADER_SIZE, SMDMIF_FIXED_COL_ALIGN ) +
                               SMDMIF_FIXED_ROW_HEADER_SIZE;
            sFixedRowLen = aCols[i].mOffset + SMDMIF_FIXED_COL_SIZE(aCols[i].mAllocSize);
            sFixedRowAlign = STL_MAX(sFixedRowAlign, SMDMIF_FIXED_COL_ALIGN);
        }
    }

    *aFixedRowLen    = STL_ALIGN( sFixedRowLen, sFixedRowAlign );
    *aFixedRowAlign  = sFixedRowAlign;

    return STL_SUCCESS;
}

/**
 * @brief 인스턴트 테이블을 생성한다.
 * @param[in] aStatement Statement 객체
 * @param[in] aTbsId 생성할 인스턴트 테이블이 속할 Tablespace ID
 * @param[in] aAttr 생성할 테이블 속성을 지정한 구조체
 * @param[in] aLoggingUndo undo 로깅 여부. 사용하지 않음
 * @param[out] aSegmentId 생성된 테이블의 세그먼트 ID
 * @param[out] aRelationHandle 생성된 인스턴트 테이블을 조작할 때 사용할 메모리 핸들
 * @param[in] aEnv Environment 구조체
 */
stlStatus smdmifTableCreate( smlStatement  *aStatement,
                             smlTbsId       aTbsId,
                             smlTableAttr  *aAttr,
                             stlBool        aLoggingUndo,
                             stlInt64      *aSegmentId,
                             void         **aRelationHandle,
                             smlEnv        *aEnv)
{
    smdmifTableHeader *sHeader = NULL;
    stlInt32           sState = 0;

    STL_TRY( smgAllocSessShmMem( STL_SIZEOF( smdmifTableHeader ),
                                 (void**)&sHeader,
                                 aEnv ) == STL_SUCCESS );
    sState = 1;

    if( (aAttr->mValidFlags & SML_TABLE_SCROLLABLE_MASK) == SML_TABLE_SCROLLABLE_NO )
    {
        aAttr->mScrollable = STL_FALSE;
    }

    STL_TRY( smdmifTableInitHeader( sHeader,
                                    aTbsId,
                                    SML_MEMORY_INSTANT_FLAT_TABLE,
                                    SMDMIF_TABLE_TYPE_FLAT,
                                    aAttr->mScrollable,
                                    STL_SIZEOF( smdmifTableHeader ),
                                    aEnv ) == STL_SUCCESS );

    STL_TRY( smdAddHandleToSession( aStatement,
                                    (smeRelationHeader*)sHeader,
                                    aEnv ) == STL_SUCCESS );
    sState = 2;
    
    *aRelationHandle = sHeader;

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void) smdmifTableDrop( aStatement, (void*)sHeader, aEnv );
            break;
        case 1:
            (void) smgFreeSessShmMem( sHeader, aEnv );
        default:
            break;
    }

    return STL_FAILURE;
}

stlStatus smdmifTableInitHeader( smdmifTableHeader *aHeader,
                                 smlTbsId           aTbsId,
                                 smlRelationType    aRelType,
                                 smdmifTableType    aInstType,
                                 stlBool            aScrollable,
                                 stlInt32           aHeaderSize,
                                 smlEnv            *aEnv )
{
    aHeader->mRelHeader.mRelationType = aRelType;
    aHeader->mRelHeader.mRelHeaderSize = aHeaderSize;
    aHeader->mRelHeader.mCreateTransId = SML_INVALID_TRANSID;
    aHeader->mRelHeader.mDropTransId = SML_INVALID_TRANSID;
    aHeader->mRelHeader.mCreateScn = 0;
    aHeader->mRelHeader.mDropScn = 0;

    SME_SET_RELATION_STATE((void*)&aHeader->mRelHeader, SME_RELATION_STATE_ACTIVE);
    SME_SET_OBJECT_SCN((void*)&aHeader->mRelHeader, SML_INFINITE_SCN);
    SME_SET_SEGMENT_HANDLE((void*)&aHeader->mRelHeader, NULL);

    aHeader->mInstType             = aInstType;
    aHeader->mTbsId                = aTbsId;
    aHeader->mCols                 = NULL;
    aHeader->mColCount             = 0;
    aHeader->mSegHandle            = NULL;
    aHeader->mInstMemList          = NULL;

    aHeader->mFixedRowLen          = 0;
    aHeader->mFixedRowAlign        = 0;
    aHeader->mVarPartColCount      = 0;
    aHeader->mInstMemAllocCount    = 0;
    aHeader->mLargestColSize       = 0;

    aHeader->mBlockSize            = smdmifGetBlockSize( aTbsId );
    aHeader->mInstMemMaxCount      = (stlInt32)knlGetPropertyBigIntValueByID( KNL_PROPERTY_INST_TABLE_INST_MEM_MAX,
                                                                              KNL_ENV( aEnv ) );
    aHeader->mVarColPieceMinLen    = (stlInt32)knlGetPropertyBigIntValueByID( KNL_PROPERTY_INST_TABLE_VAR_COL_PIECE_MIN_LEN,
                                                                              KNL_ENV( aEnv ) );
    aHeader->mBuildBlockDir        = aScrollable;

    smdmifTableResetHeaderForInsert( aHeader,
                                     aEnv );

    return STL_SUCCESS;
}

static void smdmifTableResetHeaderForInsert( smdmifTableHeader *aHeader,
                                             smlEnv            *aEnv )
{
    aHeader->mInstMemIndex         = 0;

    aHeader->mRootBlockDirAddr     = KNL_LOGICAL_ADDR_NULL;
    aHeader->mCurBlockDirAddr      = KNL_LOGICAL_ADDR_NULL;
    aHeader->mFirstFixedPartAddr   = KNL_LOGICAL_ADDR_NULL;
    aHeader->mCurFixedPartAddr     = KNL_LOGICAL_ADDR_NULL;
    aHeader->mCurVarPartAddr       = KNL_LOGICAL_ADDR_NULL;

    aHeader->mHasDeletedOnce       = STL_FALSE;
}

stlStatus smdmifTableInitColumnInfo( smdmifTableHeader *aHeader,
                                     knlValueBlockList *aColList,
                                     smlEnv            *aEnv )
{
    stlInt32           sColCount = 0;
    knlValueBlockList *sColList = aColList;

    /* 우선 column 개수를 count한다. */
    while( sColList != NULL )
    {
        sColCount++;
        sColList = sColList->mNext;
    }

    STL_TRY( smgAllocSessShmMem( STL_SIZEOF( smdmifDefColumn ) * sColCount,
                                 (void**)&aHeader->mCols,
                                 aEnv ) == STL_SUCCESS );

    aHeader->mColCount = sColCount;

    STL_TRY( smdmifTableSetColInfo( aHeader->mCols,      
                                    sColCount,
                                    aColList,
                                    &aHeader->mFixedRowLen,
                                    &aHeader->mFixedRowAlign,
                                    &aHeader->mVarPartColCount,
                                    &aHeader->mLargestColSize,
                                    aEnv ) == STL_SUCCESS );
    
    STL_TRY_THROW( aHeader->mFixedRowLen <= aHeader->mBlockSize - SMDMIF_FIRST_FIXED_ROW_OFFSET( aHeader->mFixedRowAlign ),
                   RAMP_ERR_FIXED_ROW_TOO_LARGE );

    STL_TRY( smdmifTableInitInstantMode( aHeader, aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_FIXED_ROW_TOO_LARGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_INSTANT_TABLE_ROW_TOO_LARGE,
                      NULL,
                      KNL_ERROR_STACK( aEnv ),
                      gPhaseString[KNL_STARTUP_PHASE_MOUNT] );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 인스턴트 테이블을 drop한다. 메모리 자원을 모두 해제한다.
 * @param[in] aStatement Statement 객체
 * @param[in] aRelationHandle 인스턴트 테이블 핸들
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smdmifTableDrop( smlStatement *aStatement,
                           void         *aRelationHandle,
                           smlEnv       *aEnv )
{
    smdmifTableHeader *sHeader = (smdmifTableHeader *)aRelationHandle;

    STL_TRY( smdmifTableFiniHeader( sHeader,
                                    aEnv ) == STL_SUCCESS );

    STL_TRY( smdRemoveHandleFromSession( aStatement,
                                         (smeRelationHeader*)sHeader,
                                         aEnv ) == STL_SUCCESS );

    STL_TRY( smgFreeSessShmMem( sHeader,
                                aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 인스턴트 테이블을 clean up한다. gmaster에 의해 수행된다.
 * @param[in] aRelationHandle 인스턴트 테이블 핸들
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smdmifTableCleanup( void    *aRelationHandle,
                              smlEnv  *aEnv )
{
    smdmifTableHeader *sHeader = (smdmifTableHeader *)aRelationHandle;

    /* cleanup시 segment를 반납한다.
     * 각종 메모리 해제들은 smlCleanupSessionEnv()에 의해 수행되기 때문에
     * 여기서 메모리 해제는 관여하지 않는다.
     * aEnv는 원본 session env가 아니라 system env임에 유의하라!
     * 따라서 이 함수내에서 aEnv의 session env에 접근해서는 안된다.
     * 현재 segment 하위 레이어에서는 env는 error stack의 용도로만 사용되고 있다. 
     */
    if( sHeader->mSegHandle != NULL )
    {
        STL_TRY( smsDrop( SML_INVALID_TRANSID,
                          sHeader->mSegHandle,
                          STL_FALSE, /*on start up*/
                          aEnv ) == STL_SUCCESS );
        sHeader->mSegHandle = NULL;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 인스턴트 테이블을 truncate한다. 사용한 instant memory와 extent들을 모두 해제한다.
 * @param[in] aStatement Statement 객체
 * @param[in] aRelationHandle 인스턴트 테이블 핸들
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smdmifTableTruncate( smlStatement *aStatement,
                               void         *aRelationHandle,
                               smlEnv       *aEnv )
{
    stlInt32           i;
    smdmifTableHeader *sHeader = (smdmifTableHeader *)aRelationHandle;

    if( sHeader->mSegHandle != NULL )
    {
        STL_TRY( smsDrop( SML_INVALID_TRANSID,
                          sHeader->mSegHandle,
                          STL_FALSE, /*on start up*/
                          aEnv ) == STL_SUCCESS );
        sHeader->mSegHandle = NULL; 
    }

    smdmifTableResetHeaderForInsert( sHeader,
                                     aEnv );

    /* 컬럼 정보중 동적 정보들을 reset한다. */
    for( i = 0; i < sHeader->mColCount; i++ )
    {
        sHeader->mCols[i].mHasNull = STL_FALSE;
        sHeader->mCols[i].mMaxActualLen = 0;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smdmifTableSetFlag( smlStatement      *aStatement,
                              void              *aRelationHandle,
                              smlRowBlock       *aRids,
                              knlValueBlockList *aFlags,
                              smlEnv            *aEnv )
{
    stlInt32           sBlockIdx;
    stlChar           *sFlagPos;
    smdmifFixedRow     sFixedRow;
    stlUInt64          sRowAddr;
    smlRid             sRowRid;

    for( sBlockIdx = 0;
         sBlockIdx < SML_USED_BLOCK_SIZE( aRids );
         sBlockIdx++ )
    {
        sFlagPos = KNL_GET_BLOCK_DATA_PTR( aFlags, sBlockIdx );

        if( *sFlagPos == SML_INSTANT_TABLE_ROW_FLAG_DONTCARE )
        {
            continue;
        }

        sRowRid = SML_GET_RID_VALUE( aRids, sBlockIdx );

        STL_DASSERT( STL_SIZEOF( smlRid ) == STL_SIZEOF( stlUInt64 ) );

        /* sRowRid가 8바이트 align된다는 보장이 없기 때문에 sRowAddr 변수로 옮긴다.
           sRowAddr은 8바이트 align이 보장된다. */
        stlMemcpy( &sRowAddr, &sRowRid, STL_SIZEOF( smlRid ) );

        sFixedRow = SMDMIF_GET_ROW_BY_ROW_ADDR( sRowAddr );

        STL_TRY_THROW( sFixedRow != NULL, RAMP_ERR_NO_ROW );

        if( *sFlagPos == SML_INSTANT_TABLE_ROW_FLAG_SET )
        {
            SMDMIF_FIXED_ROW_SET_FLAG( sFixedRow );
        }
        else
        {
            STL_DASSERT( *sFlagPos == SML_INSTANT_TABLE_ROW_FLAG_UNSET );

            SMDMIF_FIXED_ROW_UNSET_FLAG( sFixedRow );
        }
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NO_ROW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_INTERNAL,
                      "- invalid instant table rid",
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smdmifTableFiniHeader( smdmifTableHeader *aHeader,
                                 smlEnv            *aEnv )
{
    stlInt32 i;

    if( aHeader->mSegHandle != NULL )
    {
        STL_TRY( smsDrop( SML_INVALID_TRANSID,
                          aHeader->mSegHandle,
                          STL_FALSE, /*on start up*/
                          aEnv ) == STL_SUCCESS );
        aHeader->mSegHandle = NULL; 
    }   
    
    if( aHeader->mInstMemList != NULL )
    {
        for( i = 0; i < aHeader->mInstMemAllocCount; i++ )
        {
            if( aHeader->mInstMemList[i] != NULL )
            {
                STL_TRY( smgFreeSessShmMem( aHeader->mInstMemList[i],
                                            aEnv ) == STL_SUCCESS );
                aHeader->mInstMemList[i] = NULL;
            }
        }

        STL_TRY( smgFreeSessShmMem( aHeader->mInstMemList,
                                    aEnv ) == STL_SUCCESS );

        aHeader->mInstMemAllocCount = 0;
        aHeader->mInstMemList = NULL;    
    }   

    if( aHeader->mCols != NULL )
    {
        STL_TRY( smgFreeSessShmMem( aHeader->mCols,
                                     aEnv ) == STL_SUCCESS );
        aHeader->mCols = NULL;
    }

    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus smdmifTableGetInsertableFixedPart( smdmifTableHeader   *aHeader,
                                             void               **aFixedPart,
                                             smdmifFixedRow      *aRow,
                                             knlLogicalAddr      *aRowAddr,
                                             smlEnv              *aEnv )
{
    void *sFixedPart = SMDMIF_TABLE_GET_CUR_FIXED_PART( aHeader );

    /* fixed part insertable check */
    if( sFixedPart == NULL || SMDMIF_FIXED_PART_IS_INSERTABLE( sFixedPart ) == STL_FALSE )
    {
        STL_TRY( smdmifTablePrepareNextFixedBlock( aHeader,
                                                   &sFixedPart,
                                                   aEnv ) == STL_SUCCESS );
    }

    SMDMIF_FIXED_PART_ALLOC_ROW( sFixedPart, *aRow, *aRowAddr );

    *aFixedPart = sFixedPart;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smdmifTableUnsetDeletion( void   *aRelationHandle,
                                    smlEnv *aEnv )
{
    smdmifTableHeader *sHeader = (smdmifTableHeader *)aRelationHandle;
    void              *sFixedPart = SMDMIF_TABLE_GET_FIRST_FIXED_PART( sHeader );

    while( sFixedPart != NULL )
    {
        smdmifFixedPartUnsetDeletion( sFixedPart );
        sFixedPart = SMDMIF_FIXED_PART_GET_NEXT_FIXED_PART( sFixedPart );
    }

    return STL_SUCCESS;
}

/**
 * @brief 주어진 relation에 새 row를 삽입한다
 * @param[in] aStatement 사용중인 statement
 * @param[in] aRelationHandle row를 삽입할 Relation의 handle
 * @param[in] aValueIdx 삽입할 row의 value block내에서의 순번
 * @param[in] aInsertCols 삽입할 row의 value block
 * @param[out] aUniqueViolation 삽입할 row가 unique check에 걸려 insert되지 않았는지의 여부; flat table은 사용하지 않음.
 * @param[out] aRowRid 삽입한 row의 위치
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smdmifTableInsert( smlStatement      *aStatement,
                             void              *aRelationHandle,
                             stlInt32           aValueIdx,
                             knlValueBlockList *aInsertCols,
                             knlValueBlockList *aUniqueViolation,
                             smlRid            *aRowRid,
                             smlEnv            *aEnv )
{
    smdmifTableHeader *sHeader = (smdmifTableHeader *)aRelationHandle;

    if( sHeader->mCols == NULL )
    {
        /* 처음 insert될 때, column 정보를 세팅한다. */

        STL_TRY( smdmifTableInitColumnInfo( sHeader,
                                            aInsertCols,
                                            aEnv ) == STL_SUCCESS );
    }

    STL_TRY( smdmifTableInsertInternal( sHeader,
                                        aInsertCols,
                                        aUniqueViolation,
                                        aValueIdx,
                                        aRowRid,
                                        aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 인스턴트 테이블에 레코드를 삽입한다.
 * @param[in] aStatement 사용중인 statement
 * @param[in] aRelationHandle 인스턴트 테이블 핸들
 * @param[in] aInsertCols 삽입될 레코드들의 컬럼값 리스트
 * @param[out] aUniqueViolation 삽입할 row가 unique check에 걸려 insert되지 않았는지의 여부; flat table은 사용하지 않음.
 * @param[in] aRowBlock 삽입된 레코드의 RID 배열 (SCN은 N/A)
 * @param[in] aEnv Environment 구조체
 */
stlStatus smdmifTableBlockInsert( smlStatement      *aStatement,
                                  void              *aRelationHandle,
                                  knlValueBlockList *aInsertCols,
                                  knlValueBlockList *aUniqueViolation,
                                  smlRowBlock       *aRowBlock,
                                  smlEnv            *aEnv )
{
    smdmifTableHeader     *sHeader = (smdmifTableHeader *)aRelationHandle;
    stlInt32               sBlockIdx;

    if( sHeader->mCols == NULL )
    {
        /* 처음 insert될 때, column 정보를 세팅한다. */

        STL_TRY( smdmifTableInitColumnInfo( sHeader,
                                            aInsertCols,
                                            aEnv ) == STL_SUCCESS );
    }

    for( sBlockIdx = KNL_GET_BLOCK_SKIP_CNT( aInsertCols );
         sBlockIdx < KNL_GET_BLOCK_USED_CNT( aInsertCols );
         sBlockIdx++ )
    {
        STL_TRY( smdmifTableInsertInternal( sHeader,
                                            aInsertCols,
                                            aUniqueViolation,
                                            sBlockIdx,
                                            &aRowBlock->mRidBlock[sBlockIdx],
                                            aEnv ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smdmifTableInsertInternal( smdmifTableHeader *aHeader,
                                     knlValueBlockList *aColList,
                                     knlValueBlockList *aUniqueViolation,
                                     stlInt32           aBlockIdx,
                                     smlRid            *aRid,
                                     smlEnv            *aEnv )
{
    knlValueBlockList *sInsertCols = aColList;
    smdmifDefColumn   *sCols = aHeader->mCols;
    void              *sFixedPart;
    smdmifFixedRow     sFixedRow;
    knlLogicalAddr     sFixedRowAddr;
    stlChar           *sColPos;
    stlInt32           sColLen;
    
    /* fixed part insert할 곳을 얻어온다. */
    STL_TRY( smdmifTableGetInsertableFixedPart( aHeader,
                                                &sFixedPart,
                                                &sFixedRow,
                                                &sFixedRowAddr,
                                                aEnv ) == STL_SUCCESS );

    /* fixed row의 header를 기록한다. */
    SMDMIF_FIXED_ROW_WRITE_HEADER( sFixedRow );

    /* write columns */
    while( sInsertCols != NULL )
    {
        /* 컬럼이 위에서 잘 내려왔는지 체크한다. */
        STL_DASSERT( KNL_GET_BLOCK_USED_CNT( sInsertCols ) > aBlockIdx );
 
        sColLen = KNL_GET_BLOCK_DATA_LENGTH(sInsertCols, aBlockIdx);
        sColPos = KNL_GET_BLOCK_DATA_PTR(sInsertCols, aBlockIdx);

        if( sCols->mIsFixedPart == STL_TRUE )
        {
            smdmifFixedRowWriteCol( sFixedRow,
                                    sCols,
                                    sColPos,
                                    sColLen );
        }
        else
        {
            STL_TRY( smdmifVarPartWriteCol( aHeader,    /* 내부적으로 var part block을 새로 할당받기 위해 */
                                            SMDMIF_TABLE_GET_CUR_VAR_PART( aHeader ),
                                            SMDMIF_FIXED_ROW_GET_VAR_COL_POS( sFixedRow, sCols ),
                                            sColPos,
                                            sColLen,
                                            aHeader->mVarColPieceMinLen,
                                            smdmifTablePrepareNextVarBlock,
                                            aEnv ) == STL_SUCCESS );
        }

        if( sColLen == 0 )
        {
            /* null 유무 세팅 */
            sCols->mHasNull = STL_TRUE;
        }
        else if( sCols->mMaxActualLen < sColLen )
        {
            /* 컬럼 최대값 갱신 */
            sCols->mMaxActualLen = sColLen;
        }

        /* 다음 컬럼으로 진행 */
        sInsertCols = sInsertCols->mNext;
        sCols++;
    }

    /* RID 세팅 */
    STL_WRITE_INT64( aRid, &sFixedRowAddr );

    return STL_SUCCESS;                
    
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus smdmifTableFindRow( smdmifTableHeader  *aHeader,
                              stlInt64            aRowSequenceNumber,
                              stlChar           **aRowPos,
                              smlEnv             *aEnv )
{
    void      *sFixedPart;
    stlInt64   sRowNum = aRowSequenceNumber;

    smdmifTableFindFixedPart( aHeader,
                              &sRowNum,
                              &sFixedPart,
                              aEnv );

    /* 찾으려는 row가 없다. */
    if( sFixedPart == NULL )
    {
        *aRowPos = NULL;
    }
    else
    {
        *aRowPos = SMDMIF_FIXED_PART_GET_ROW( sFixedPart,
                                              sRowNum );
    }

    return STL_SUCCESS;
}

void smdmifTableFindFixedPart( smdmifTableHeader  *aHeader,
                               stlInt64           *aRowPositionNumber,
                               void              **aFixedPart,
                               smlEnv             *aEnv )
{
    STL_DASSERT( aHeader->mBuildBlockDir == STL_TRUE );

    if( SMDMIF_TABLE_IS_ONE_BLOCK_TABLE( aHeader ) == STL_TRUE )
    {
        *aFixedPart = SMDMIF_TABLE_GET_FIRST_FIXED_PART( aHeader );
    }
    else
    {
        STL_DASSERT( aHeader->mRootBlockDirAddr != KNL_LOGICAL_ADDR_NULL );

        *aFixedPart = smdmifBlockDirFindItem( aHeader->mRootBlockDirAddr,
                                              SMDMIF_FIXED_PART_GET_MAX_ROW_COUNT(
                                                  SMDMIF_TABLE_GET_FIRST_FIXED_PART( aHeader ) ),
                                              aRowPositionNumber,
                                              aEnv );
    }
}

/**
 * @brief 인스턴트 테이블에 레코드를 갱신한다.
 * @param[in] aStatement Statement 객체
 * @param[in] aRelationHandle 인스턴트 테이블 핸들
 * @param[in] aRowRid 갱신할 row의 row rid
 * @param[in] aRowScn 레코드 검색 당시의 SCN. 사용하지 않음
 * @param[in] aValueIdx Value block에서 현재 처리할 row의 순서
 * @param[in] aAfterCols 갱신할 컬럼의 이후 이미지 정보들
 * @param[out] aBeforeCols 갱신할 컬럼의 이전 이미지를 기록할 버퍼. 사용하지 않음
 * @param[in] aPrimaryKey 해당 레코드의 primary key. 사용하지 않음
 * @param[out] aVersionConflict 항상 STL_FALSE를 리턴
 * @param[out] aSkipped 항상 STL_FALSE를 리턴
 * @param[in] aEnv Environment 구조체
 */
stlStatus smdmifTableUpdate( smlStatement      *aStatement,
                             void              *aRelationHandle,
                             smlRid            *aRowRid,
                             smlScn             aRowScn,
                             stlInt32           aValueIdx,
                             knlValueBlockList *aAfterCols,
                             knlValueBlockList *aBeforeCols,
                             knlValueBlockList *aPrimaryKey,
                             stlBool           *aVersionConflict,
                             stlBool           *aSkipped,
                             smlEnv            *aEnv )
{
    smdmifTableHeader   *sHeader = (smdmifTableHeader *)aRelationHandle;
    knlValueBlockList   *sUpdateCols = aAfterCols;
    smdmifFixedRow       sFixedRow;
    stlInt32             sSourceColLen;
    stlChar             *sSourceColPos;
    smdmifDefColumn     *sColInfo;
    knlLogicalAddr       sRowAddr;

    SMDMIF_RID_TO_ROW_ADDR( &sRowAddr, aRowRid );

    sFixedRow = SMDMIF_GET_ROW_BY_ROW_ADDR( sRowAddr );

    /* 찾으려는 row가 없다. */
    STL_TRY_THROW( sFixedRow != NULL, RAMP_ERR_NO_ROW );

    while( sUpdateCols != NULL )
    {
        sSourceColLen = KNL_GET_BLOCK_DATA_LENGTH(sUpdateCols, aValueIdx);
        sSourceColPos = KNL_GET_BLOCK_DATA_PTR(sUpdateCols, aValueIdx);

        sColInfo = &sHeader->mCols[sUpdateCols->mColumnOrder];

        if( sColInfo->mIsFixedPart == STL_TRUE )
        {
            smdmifFixedRowUpdateCol( sFixedRow,
                                     sColInfo,
                                     sSourceColPos,
                                     sSourceColLen );
        }
        else
        {
            STL_TRY( smdmifVarPartUpdateCol( sHeader,
                                             SMDMIF_TABLE_GET_CUR_VAR_PART( sHeader ),
                                             SMDMIF_FIXED_ROW_GET_VAR_COL_POS( sFixedRow, sColInfo ),
                                             sSourceColPos,
                                             sSourceColLen,
                                             sHeader->mVarColPieceMinLen,
                                             smdmifTablePrepareNextVarBlock,
                                             aEnv ) == STL_SUCCESS );
        }

        sUpdateCols = sUpdateCols->mNext;
    }

    *aVersionConflict = STL_FALSE;
    *aSkipped = STL_FALSE;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NO_ROW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_INTERNAL,
                      "- invalid instant table rid",
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 인스턴트 테이블에 레코드를 삭제한다.
 * @param[in] aStatement Statement 객체
 * @param[in] aRelationHandle 인스턴트 테이블 핸들
 * @param[in] aRowRid 삭제할 row의 rid
 * @param[in] aRowScn 삭제할 row의 scn. 사용하지 않음
 * @param[in] aValueIdx 사용하지 않음
 * @param[in] aPrimaryKey 사용하지 않음
 * @param[out] aVersionConflict 항상 STL_FALSE를 리턴
 * @param[out] aSkipped 항상 STL_FALSE를 리턴
 * @param[in] aEnv Environment 구조체
 */
stlStatus smdmifTableDelete( smlStatement      *aStatement,
                             void              *aRelationHandle,
                             smlRid            *aRowRid,
                             smlScn             aRowScn,
                             stlInt32           aValueIdx,
                             knlValueBlockList *aPrimaryKey,
                             stlBool           *aVersionConflict,
                             stlBool           *aSkipped,
                             smlEnv            *aEnv )
{
    smdmifTableHeader  *sHeader = (smdmifTableHeader *)aRelationHandle;
    smdmifFixedRow      sFixedRow;

    sFixedRow = SMDMIF_GET_ROW_BY_ROW_ADDR( *aRowRid );

    STL_TRY_THROW( sFixedRow != NULL, RAMP_ERR_NO_ROW );

    SMDMIF_FIXED_ROW_DELETE( sFixedRow );

    /* 한번이라도 delete가 불리면 mHasDeletedOnce를 true로 세팅한다. */
    sHeader->mHasDeletedOnce = STL_TRUE;

    *aVersionConflict = STL_FALSE;
    *aSkipped = STL_FALSE;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NO_ROW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_INTERNAL,
                      "- invalid instant table rid",
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 인스턴트 테이블에 특정 레코드를 얻어온다.
 * @param[in] aStatement  Statement
 * @param[in] aFetchRecordInfo  fetch할 테이블, 컬럼들의 정보
 * @param[in] aRowRid 찾을 row의 position number
 * @param[in] aBlockIdx value block list에서 해당 index
 * @param[out] aIsMatched 찾았는지의 여부
 * @param[out] aDeleted 찾은 row의 삭제 여부
 * @param[out] aUpdated 찾은 row의 갱신 여부
 * @param[in] aEnv Environment 구조체
 */
stlStatus smdmifTableFetch( smlStatement        *aStatement,
                            smlFetchRecordInfo  *aFetchRecordInfo,
                            smlRid              *aRowRid,
                            stlInt32             aBlockIdx,
                            stlBool             *aIsMatched,
                            stlBool             *aDeleted,
                            stlBool             *aUpdated,
                            smlEnv              *aEnv )
{
    smdmifTableHeader   *sHeader = (smdmifTableHeader *)aFetchRecordInfo->mRelationHandle;
    smdmifFixedRow       sFixedRow;
    knlLogicalAddr       sRowAddr;

    SMDMIF_RID_TO_ROW_ADDR( &sRowAddr, aRowRid );

    sFixedRow = SMDMIF_GET_ROW_BY_ROW_ADDR( *aRowRid );

    *aIsMatched = STL_FALSE;

    if( sFixedRow == NULL )
    {
        STL_THROW( RAMP_FINISH );
    }

    STL_TRY( smdmifTableFetchOneRow( sHeader,
                                     sFixedRow,
                                     NULL, /* key physical filter */
                                     NULL, /* key logical filter */
                                     NULL, /* key fetch columns */
                                     aFetchRecordInfo->mPhysicalFilter,
                                     aFetchRecordInfo->mLogicalFilterEvalInfo,
                                     aFetchRecordInfo->mColList,
                                     aBlockIdx,
                                     aIsMatched,
                                     aEnv ) == STL_SUCCESS );

    *aDeleted = SMDMIF_FIXED_ROW_IS_DELETED( sFixedRow );

    /**
     * Flat table은 Record Visibility를 지원하지 않는다.
     */
    
    STL_DASSERT( aUpdated == NULL );

    STL_RAMP( RAMP_FINISH );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

void smdmifTableGetColValueAndLen( smdmifFixedRow    aRow,
                                   smdmifDefColumn  *aCol,
                                   stlChar          *aTempMem,
                                   stlChar         **aColValue,
                                   stlInt64         *aColLen )
{
    smdmifFixedCol   sFixedCol;
    smdmifVarCol     sVarCol;
    stlInt64         sColLen;

    if( aCol->mIsFixedPart == STL_TRUE )
    {
        sFixedCol = SMDMIF_FIXED_ROW_GET_COL( aRow, aCol );
        *aColValue = SMDMIF_FIXED_COL_GET_VALUE( sFixedCol );
        *aColLen   = SMDMIF_FIXED_COL_GET_LEN( sFixedCol );
    }
    else
    {
        /* var part column인 경우이다. */
        sVarCol = (smdmifVarCol)SMDMIF_FIXED_ROW_GET_VAR_COL( aRow, aCol );

        if( SMDMIF_VAR_COL_HAS_NEXT( sVarCol ) == STL_TRUE )
        {
            /* var col이 쪼개져 있는 경우 aTempMem에 복사한다. */
            smdmifVarPartReadCol( sVarCol,
                                  aTempMem,
                                  &sColLen );
            *aColValue = aTempMem;
            *aColLen   = sColLen;
        }
        else
        {
            *aColValue = SMDMIF_VAR_COL_VALUE_POS( sVarCol );
            *aColLen   = SMDMIF_VAR_COL_GET_LEN( sVarCol );
        }
    }
}

stlStatus smdmifTableGetLongVaryingValueAndLen( smdmifFixedRow     aRow,
                                                smdmifDefColumn  * aCol,
                                                dtlDataValue     * aDataValue,
                                                stlChar         ** aColValue,
                                                stlInt64         * aColLen,
                                                smlEnv           * aEnv )
{
    smdmifVarCol  sVarCol;

    STL_DASSERT( aCol->mIsFixedPart == STL_FALSE );
    
    /* var part column인 경우이다. */
    sVarCol = (smdmifVarCol)SMDMIF_FIXED_ROW_GET_VAR_COL( aRow, aCol );
    
    if( SMDMIF_VAR_COL_HAS_NEXT( sVarCol ) == STL_TRUE )
    {
        /* var col이 쪼개져 있는 경우 aDataValue에 복사한다. */

        STL_TRY( smdmifVarPartReadDataValue( sVarCol,
                                             aDataValue,
                                             aEnv )
                 == STL_SUCCESS );
        
        *aColValue = aDataValue->mValue;
        *aColLen   = aDataValue->mLength;
    }
    else
    {
        *aColValue = SMDMIF_VAR_COL_VALUE_POS( sVarCol );
        *aColLen   = SMDMIF_VAR_COL_GET_LEN( sVarCol );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smdmifTableCopyColValue( smdmifFixedRow     aRow,
                                   smdmifDefColumn  * aCol,
                                   dtlDataValue     * aDataValue,
                                   smlEnv           * aEnv )
{
    smdmifFixedCol   sFixedCol;
    smdmifVarCol     sVarCol;

    if( aCol->mIsFixedPart == STL_TRUE )
    {
        sFixedCol = SMDMIF_FIXED_ROW_GET_COL( aRow, aCol );
        aDataValue->mLength = SMDMIF_FIXED_COL_GET_LEN( sFixedCol );

        stlMemcpy( aDataValue->mValue,
                   SMDMIF_FIXED_COL_GET_VALUE( sFixedCol ),
                   aDataValue->mLength );
    }
    else
    {
        /* var part column인 경우이다. */
        sVarCol = (smdmifVarCol)SMDMIF_FIXED_ROW_GET_VAR_COL( aRow, aCol );

        STL_TRY( smdmifVarPartReadDataValue( sVarCol,
                                             aDataValue,
                                             aEnv )
                 == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 인스턴트 테이블에 특정 레코드를 얻어온다.
 * @param[in] aHeader 테이블 헤더
 * @param[in] aFixedRow 읽을 row
 * @param[in] aKeyPhysicalFilter physical filter
 * @param[in] aKeyLogicalFilterInfo logical filter
 * @param[in] aKeyFetchCol 읽을 컬럼 리스트
 * @param[in] aRowPhysicalFilter physical filter
 * @param[in] aRowLogicalFilterInfo logical filter
 * @param[in] aRowFetchCol 읽을 컬럼 리스트
 * @param[in] aBlockIdx value block list에서 해당 index
 * @param[out] aFetched row를 읽었는지의 여부
 * @param[in] aEnv Environment 구조체
 */
stlStatus smdmifTableFetchOneRow( smdmifTableHeader *aHeader,
                                  void              *aFixedRow,
                                  knlPhysicalFilter *aKeyPhysicalFilter,
                                  knlExprEvalInfo   *aKeyLogicalFilterInfo,
                                  knlValueBlockList *aKeyFetchCol,
                                  knlPhysicalFilter *aRowPhysicalFilter,
                                  knlExprEvalInfo   *aRowLogicalFilterInfo,
                                  knlValueBlockList *aRowFetchCol,
                                  stlInt32           aBlockIdx,
                                  stlBool           *aFetched,
                                  smlEnv            *aEnv )
{
    knlValueBlockList   *sFetchCol;
    smdmifFixedRow       sFixedRow = (smdmifFixedRow)aFixedRow;
    smdmifVarCol         sVarCol;
    smdmifDefColumn     *sColInfo;
    knlColumnInReadRow  *sColInRead;
    dtlDataValue        *sValueCol;
    stlBool              sColAlreadyCopied;
    stlBool              sPhysicalFiltered;
    knlPhysicalFilter   *sPhysicalFilter;
    knlExprEvalInfo     *sLogicalFilterInfo;
    knlValueBlockList   *sColList;
    stlBool              sRowFiltered = STL_FALSE;
    knlColumnInReadRow  *sCurInReadRow = NULL;

    *aFetched = STL_FALSE;

    sPhysicalFilter    = aKeyPhysicalFilter;
    sLogicalFilterInfo = aKeyLogicalFilterInfo;
    sColList           = aKeyFetchCol;
    while( STL_TRUE )
    {
        sPhysicalFiltered = STL_FALSE;
        if( sPhysicalFilter != NULL )
        {
            sPhysicalFiltered = STL_TRUE;
            sFetchCol = sColList;
            while( sFetchCol != NULL )
            {
                sColInfo = &aHeader->mCols[sFetchCol->mColumnOrder];
                sColInRead = KNL_GET_BLOCK_COLUMN_IN_ROW( sFetchCol );
                sValueCol = KNL_GET_BLOCK_DATA_VALUE( sFetchCol, aBlockIdx );

                if( (KNL_GET_BLOCK_DB_TYPE(sFetchCol) == DTL_TYPE_LONGVARCHAR) ||
                    (KNL_GET_BLOCK_DB_TYPE(sFetchCol) == DTL_TYPE_LONGVARBINARY) )
                {
                    STL_TRY( smdmifTableGetLongVaryingValueAndLen( sFixedRow,
                                                                   sColInfo,
                                                                   sValueCol,
                                                                   (stlChar**)&sColInRead->mValue,
                                                                   &sColInRead->mLength,
                                                                   aEnv )
                             == STL_SUCCESS );
                }
                else
                {
                    smdmifTableGetColValueAndLen( sFixedRow,
                                                  sColInfo,
                                                  sValueCol->mValue,
                                                  (stlChar**)&sColInRead->mValue,
                                                  &sColInRead->mLength );
                }

                sValueCol->mLength = sColInRead->mLength;

                sCurInReadRow = KNL_GET_BLOCK_COLUMN_IN_ROW( sFetchCol );

                sCurInReadRow->mValue  = sColInRead->mValue;
                sCurInReadRow->mLength = sColInRead->mLength;

                while( sPhysicalFilter != NULL )
                {
                    if( sPhysicalFilter->mColIdx1 <= sFetchCol->mColumnOrder )
                    {
                        if( sPhysicalFilter->mFunc( sPhysicalFilter->mColVal1->mValue,
                                                    sPhysicalFilter->mColVal1->mLength,
                                                    sPhysicalFilter->mColVal2->mValue,
                                                    sPhysicalFilter->mColVal2->mLength ) == STL_FALSE )
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

                sFetchCol = sFetchCol->mNext;
            }
        }

        /* physical filter를 만족했으므로, value block에 column value를 복사한다. */
        sFetchCol = sColList;
        while( sFetchCol != NULL )
        {
            sColInfo = &aHeader->mCols[sFetchCol->mColumnOrder];
            sValueCol = KNL_GET_BLOCK_DATA_VALUE( sFetchCol, aBlockIdx );

            sColAlreadyCopied = STL_FALSE;
            if( sColInfo->mIsFixedPart == STL_FALSE )
            {
                sVarCol = (smdmifVarCol)SMDMIF_FIXED_ROW_GET_VAR_COL( sFixedRow, sColInfo );
                if( (sPhysicalFiltered == STL_TRUE) && (SMDMIF_VAR_COL_HAS_NEXT( sVarCol ) == STL_TRUE ) )
                {
                    /* physical filter가 적용됐고, 컬럼이 쪼개져 있는 경우, 이미 value block에 복사되어 있다.*/
                    sColAlreadyCopied = STL_TRUE;
                }
            }

            if( sColAlreadyCopied == STL_FALSE )
            {
                STL_TRY( smdmifTableCopyColValue( sFixedRow,
                                                  sColInfo,
                                                  sValueCol,
                                                  aEnv )
                         == STL_SUCCESS );
            }

            sFetchCol = sFetchCol->mNext;
        }

        /**
         * Logical Filter 평가
         */
        if( sLogicalFilterInfo != NULL )
        {
            sLogicalFilterInfo->mBlockIdx = aBlockIdx;
            
            STL_TRY( knlEvaluateOneExpression( sLogicalFilterInfo,
                                               KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            STL_DASSERT( KNL_GET_BLOCK_IS_SEP_BUFF(
                             sLogicalFilterInfo->mResultBlock )
                         == STL_TRUE );
                
            if( !DTL_BOOLEAN_IS_TRUE( KNL_GET_BLOCK_SEP_DATA_VALUE(
                                          sLogicalFilterInfo->mResultBlock,
                                          aBlockIdx ) ) )
            {
                STL_THROW( RAMP_FINISH );
            }
        }

        if( sRowFiltered == STL_FALSE )
        {
            sPhysicalFilter    = aRowPhysicalFilter;
            sLogicalFilterInfo = aRowLogicalFilterInfo;
            sColList           = aRowFetchCol;
            sRowFiltered = STL_TRUE;
        }
        else
        {
            break;
        }
    }

    *aFetched = STL_TRUE;

    STL_RAMP( RAMP_FINISH );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 인스턴트 테이블에 특정 레코드를 얻어온다.
 * @param[in] aHeader 테이블 헤더
 * @param[in] aFixedRow 읽을 row
 * @param[in] aRowPhysicalFilter physical filter
 * @param[in] aRowLogicalFilterInfo logical filter
 * @param[in] aRowFetchCol 읽을 컬럼 리스트
 * @param[in] aBlockIdx value block list에서 해당 index
 * @param[out] aFetched row를 읽었는지의 여부
 * @param[in] aEnv Environment 구조체
 */
stlStatus smdmifTableFetchOneRowWithRowFilter( smdmifTableHeader *aHeader,
                                               void              *aFixedRow,
                                               knlPhysicalFilter *aRowPhysicalFilter,
                                               knlExprEvalInfo   *aRowLogicalFilterInfo,
                                               knlValueBlockList *aRowFetchCol,
                                               stlInt32           aBlockIdx,
                                               stlBool           *aFetched,
                                               smlEnv            *aEnv )
{
    knlValueBlockList   *sFetchCol;
    smdmifFixedRow       sFixedRow = (smdmifFixedRow)aFixedRow;
    smdmifVarCol         sVarCol;
    smdmifDefColumn     *sColInfo;
    knlColumnInReadRow  *sColInRead;
    dtlDataValue        *sValueCol;
    knlPhysicalFilter   *sPhysicalFilter;
    knlExprEvalInfo     *sLogicalFilterInfo;
    knlValueBlockList   *sColList;
    smdmifFixedCol       sFixedCol;
    stlInt64             sColLen;

    *aFetched = STL_FALSE;

    sPhysicalFilter    = aRowPhysicalFilter;
    sLogicalFilterInfo = aRowLogicalFilterInfo;
    sColList           = aRowFetchCol;
    
    if( sPhysicalFilter != NULL )
    {
        sFetchCol = sColList;
        
        while( sFetchCol != NULL )
        {
            sColInfo = &aHeader->mCols[sFetchCol->mColumnOrder];
            sColInRead = KNL_GET_BLOCK_COLUMN_IN_ROW( sFetchCol );
            sValueCol = KNL_GET_BLOCK_DATA_VALUE( sFetchCol, aBlockIdx );

            if( (KNL_GET_BLOCK_DB_TYPE(sFetchCol) == DTL_TYPE_LONGVARCHAR) ||
                (KNL_GET_BLOCK_DB_TYPE(sFetchCol) == DTL_TYPE_LONGVARBINARY) )
            {
                STL_TRY( smdmifTableGetLongVaryingValueAndLen( sFixedRow,
                                                               sColInfo,
                                                               sValueCol,
                                                               (stlChar**)&sColInRead->mValue,
                                                               &sColInRead->mLength,
                                                               aEnv )
                         == STL_SUCCESS );
            }
            else
            {
                smdmifTableGetColValueAndLen( sFixedRow,
                                              sColInfo,
                                              sValueCol->mValue,
                                              (stlChar**)&sColInRead->mValue,
                                              &sColInRead->mLength );
            }

            sValueCol->mLength = sColInRead->mLength;

            while( sPhysicalFilter != NULL )
            {
                if( sPhysicalFilter->mColIdx1 <= sFetchCol->mColumnOrder )
                {
                    if( sPhysicalFilter->mFunc( sPhysicalFilter->mColVal1->mValue,
                                                sPhysicalFilter->mColVal1->mLength,
                                                sPhysicalFilter->mColVal2->mValue,
                                                sPhysicalFilter->mColVal2->mLength ) == STL_FALSE )
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

            /* 컬럼이 쪼개져 있는 경우, 이미 value block에 복사되어 있다.*/

            if( sColInfo->mIsFixedPart == STL_TRUE )
            {
                sFixedCol = SMDMIF_FIXED_ROW_GET_COL( sFixedRow, sColInfo );
                sColLen   = SMDMIF_FIXED_COL_GET_LEN( sFixedCol );

                DTL_COPY_VALUE( sValueCol,
                                SMDMIF_FIXED_COL_GET_VALUE( sFixedCol ),
                                sColLen );
            }
            else
            {
                sVarCol = (smdmifVarCol)SMDMIF_FIXED_ROW_GET_VAR_COL( sFixedRow, sColInfo );
            
                if( SMDMIF_VAR_COL_HAS_NEXT( sVarCol ) == STL_FALSE )
                {
                    STL_TRY( smdmifVarPartReadDataValue( sVarCol,
                                                         sValueCol,
                                                         aEnv )
                             == STL_SUCCESS );
                }
            }

            sFetchCol = sFetchCol->mNext;
        }
    }
    else
    {
        sFetchCol = sColList;
        
        while( sFetchCol != NULL )
        {
            sColInfo = &aHeader->mCols[sFetchCol->mColumnOrder];
            sValueCol = KNL_GET_BLOCK_DATA_VALUE( sFetchCol, aBlockIdx );
            
            if( sColInfo->mIsFixedPart == STL_TRUE )
            {
                sFixedCol = SMDMIF_FIXED_ROW_GET_COL( sFixedRow, sColInfo );
                sColLen   = SMDMIF_FIXED_COL_GET_LEN( sFixedCol );

                DTL_COPY_VALUE( sValueCol,
                                SMDMIF_FIXED_COL_GET_VALUE( sFixedCol ),
                                sColLen );
                
                sValueCol->mLength = sColLen;
            }
            else
            {
                /* var part column인 경우이다. */
                sVarCol = (smdmifVarCol)SMDMIF_FIXED_ROW_GET_VAR_COL( sFixedRow, sColInfo );

                STL_TRY( smdmifVarPartReadDataValue( sVarCol,
                                                     sValueCol,
                                                     aEnv )
                         == STL_SUCCESS );
            }

            sFetchCol = sFetchCol->mNext;
        }
    }

    /**
     * Logical Filter 평가
     */
    if( sLogicalFilterInfo != NULL )
    {
        sLogicalFilterInfo->mBlockIdx = aBlockIdx;
            
        STL_TRY( knlEvaluateOneExpression( sLogicalFilterInfo,
                                           KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        STL_DASSERT( KNL_GET_BLOCK_IS_SEP_BUFF(
                         sLogicalFilterInfo->mResultBlock )
                     == STL_TRUE );
                
        if( !DTL_BOOLEAN_IS_TRUE( KNL_GET_BLOCK_SEP_DATA_VALUE(
                                      sLogicalFilterInfo->mResultBlock,
                                      aBlockIdx ) ) )
        {
            STL_THROW( RAMP_FINISH );
        }
    }
    
    *aFetched = STL_TRUE;

    STL_RAMP( RAMP_FINISH );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 인스턴트 테이블을 위해 block 하나를 할당한다.
 * @param[in] aHeader 인스턴트 테이블 헤더
 * @param[in] aBlockType 할당할 block의 용도
 * @param[out] aBlock block의 시작 주소
 * @param[in,out] aEnv Environment 구조체
 * @note block은 instant table이 사용하는 데이터 할당 단위이다. extent가 될 수도 있고, page가 될 수도 있다.
 *       현재 block은 extent 단위인데, page로 바꾸려면 이 함수만 바꾸면 된다. 
 */
stlStatus smdmifTableAllocBlock( smdmifTableHeader  *aHeader,
                                 smdmifDefBlockType  aBlockType,
                                 void              **aBlock,
                                 smlEnv             *aEnv )
{
    knlLogicalAddr sBlockAddr;

    /*
     * instant table의 block을 할당할 때 query timeout을 체크하도록 했다.
     */
    STL_TRY( knlCheckQueryTimeout( KNL_ENV(aEnv) ) == STL_SUCCESS );

    if( aHeader->mInstMemIndex < aHeader->mInstMemMaxCount )
    {
        STL_TRY( smdmifTableAllocBlockFromInstMem( aHeader,
                                                   aBlock,
                                                   &sBlockAddr,
                                                   aEnv ) == STL_SUCCESS );
    }
    else
    {
        STL_TRY( smdmifTableAllocBlockFromTbs( aHeader,
                                               aBlock,
                                               &sBlockAddr,
                                               aEnv ) == STL_SUCCESS );
    }

    SMDMIF_DEF_BLOCK_INIT( *aBlock, sBlockAddr, aBlockType );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

static stlStatus smdmifTableAllocBlockFromInstMem( smdmifTableHeader  *aHeader,
                                                   void              **aBlock,
                                                   knlLogicalAddr     *aBlockAddr,
                                                   smlEnv             *aEnv )
{
    STL_DASSERT( aHeader->mInstMemIndex <= aHeader->mInstMemAllocCount );

    if( aHeader->mInstMemIndex == aHeader->mInstMemAllocCount )
    {
        STL_TRY( smgAllocSessShmMem( aHeader->mBlockSize,
                                     (void**)&aHeader->mInstMemList[aHeader->mInstMemAllocCount],
                                     aEnv ) == STL_SUCCESS );
 
        aHeader->mInstMemAllocCount++;
    }

    *aBlock = aHeader->mInstMemList[aHeader->mInstMemIndex];

    STL_TRY( knlGetLogicalAddr( *aBlock,
                                aBlockAddr,
                                KNL_ENV( aEnv ) ) == STL_SUCCESS );

    aHeader->mInstMemIndex++;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

static stlStatus smdmifTableAllocBlockFromTbs( smdmifTableHeader  *aHeader,
                                               void              **aBlock,
                                               knlLogicalAddr     *aBlockAddr,
                                               smlEnv             *aEnv )
{
    smxmTrans      sMiniTrans;
    smlPid         sPid;
    smpHandle      sPageHandle;
    smsmfExtHdr   *sExtHdr;
    smlRid         sDummySegRid;
    stlInt32       sState = 0;

    if( aHeader->mSegHandle == NULL )
    {
        STL_TRY( smxmBegin( &sMiniTrans,
                            SML_INVALID_TRANSID,
                            SML_INVALID_RID,
                            SMXM_ATTR_NOLOG_DIRTY | SMXM_ATTR_NO_VALIDATE_PAGE,
                            aEnv ) == STL_SUCCESS );
        sState = 1;

        /* flat segment를 할당한다. */
        STL_TRY( smsAllocSegHdr( &sMiniTrans,
                                 aHeader->mTbsId,
                                 SML_SEG_TYPE_MEMORY_FLAT,
                                 NULL,      /*aAttr*/
                                 STL_FALSE, /*aHasRelHdr*/
                                 &sDummySegRid,
                                 &aHeader->mSegHandle,
                                 aEnv ) == STL_SUCCESS );

        STL_TRY( smsCreate( SML_INVALID_TRANSID,
                            aHeader->mTbsId,
                            SML_SEG_TYPE_MEMORY_FLAT,
                            aHeader->mSegHandle,
                            0, /*initial extent: not used*/
                            aEnv ) == STL_SUCCESS );
        sState = 0;
        STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );
    }

    STL_TRY( smsAllocExtent( SML_INVALID_TRANSID,
                             aHeader->mSegHandle,
                             SMP_PAGE_TYPE_TABLE_DATA, /*not used*/
                             &sPid,
                             aEnv ) == STL_SUCCESS );

    STL_TRY( smpFix( aHeader->mTbsId,
                     sPid,
                     &sPageHandle,
                     aEnv ) == STL_SUCCESS );

    sExtHdr = (smsmfExtHdr *)smpGetBody( SMP_FRAME( &sPageHandle ) );

    STL_TRY( smpUnfix( &sPageHandle,
                       aEnv ) == STL_SUCCESS );

    /* sExtHdr의 위치가 정말 smpPhyHdr 바로 다음에 오는지 검사한다.
       이 assert에서 실패하면 smdmifGetBlockSize 함수를 수정해야 한다. */
    STL_DASSERT( ( (stlChar*)sExtHdr - (stlChar*)( SMP_FRAME(&sPageHandle) ) ) == STL_SIZEOF( smpPhyHdr ) );

    /* block의 시작 주소는 extent header 바로 다음이다. */
    *aBlock = (void *)(sExtHdr + 1);

    STL_TRY( knlGetLogicalAddr( *aBlock,
                                aBlockAddr,
                                KNL_ENV( aEnv ) ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smxmRollback( &sMiniTrans, aEnv );
    }

    return STL_FAILURE;
}

stlStatus smdmifTableVerify( smdmifTableHeader *aHeader )
{
    stlInt32    i;

    /**
     * TBS_ID 체크
     */
    STL_DASSERT( SML_TBS_IS_MEMORY_TEMPORARY( SMF_GET_TBS_ATTR(aHeader->mTbsId) ) == STL_TRUE );

    /**
     * 컬럼 체크
     */
    STL_DASSERT( aHeader->mColCount > 0 );

    for( i = 0; i < aHeader->mColCount; i++ )
    {
        STL_DASSERT( aHeader->mCols[i].mOffset > 0 &&
                     aHeader->mCols[i].mOffset < aHeader->mFixedRowLen );
    }

    /**
     * fixed part 순회 체크
     */
    STL_DASSERT( smdmifFixedPartVerify( SMDMIF_TABLE_GET_FIRST_FIXED_PART( aHeader ),
                                        aHeader->mCols,
                                        aHeader->mColCount,
                                        smdmifVarPartVerifyCol ) == STL_SUCCESS );

    /**
     * block directory 체크
     */
    if( aHeader->mBuildBlockDir == STL_TRUE )
    {
        if( aHeader->mRootBlockDirAddr != KNL_LOGICAL_ADDR_NULL )
        {
            STL_DASSERT( smdmifBlockDirVerify( KNL_TO_PHYSICAL_ADDR(aHeader->mRootBlockDirAddr) )
                         == STL_SUCCESS );
        }
    }

    return STL_SUCCESS;
}

/** @} */
