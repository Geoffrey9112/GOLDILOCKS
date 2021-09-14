/*******************************************************************************
 * smdmifTable.h
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
 * @file smdmifTable.h
 * @brief Storage Manager Layer Memory Instant Flat Table Routines
 */

/**
 * @defgroup smdmifTable Memory Instant Flat Table
 * @ingroup smdInternal
 * @{
 */

#ifndef _SMDMIF_TABLE_H_
#define _SMDMIF_TABLE_H_ 1

#include <smdmifDef.h>
#include <smdmifFixedPart.h>

typedef enum
{
    SMDMIF_TABLE_TYPE_FLAT = 1,
    SMDMIF_TABLE_TYPE_MERGE_SORT,
    SMDMIF_TABLE_TYPE_ORDERED_SORT,
    SMDMIF_TABLE_TYPE_VOLATILE_SORT,
    SMDMIF_TABLE_TYPE_INDEX_SORT
} smdmifTableType;

typedef struct smdmifTableHeader smdmifTableHeader;

typedef stlStatus (*smdmifAfterBlockInsert)( smdmifTableHeader *aHeader,
                                             smlEnv            *aEnv );

typedef stlStatus (*smdmifBeforeFirstInsert)( smdmifTableHeader *aHeader,
                                              smlEnv            *aEnv );

/**
 * @brief Instant table 헤더 구조체. handle로 사용되기도 함
 * @note instant table은 두가지 모드로 동작한다.
 *       instant mode: 처음에 instant mode로 동작한다. row가 적을 때 shared memory로부터 할당받아 사용한다.
 *       extent mode: row가 instant memory에 가득차면, temp tablespace로부터 메모리를 할당받아
 *                   대용량의 테이블을 준비한다.
 */
struct smdmifTableHeader
{
    smeRelationHeader   mRelHeader;           /**< 모든 테이블 해더의 공통 해더 */

    smdmifTableType     mInstType;            /**< instant table 종류 */
    smlTbsId            mTbsId;
    stlInt32            mColCount;
    smdmifDefColumn    *mCols;
    void               *mSegHandle;           /**< flat segment handle */

    stlInt32            mFixedRowLen;
    stlInt32            mFixedRowAlign;
    stlInt32            mVarPartColCount;
    stlInt32            mLargestColSize;      /**< insert된 column 중 가장 큰 크기; combine시 메모리 크기로 사용됨 */
    stlInt32            mBlockSize;

    void              **mInstMemList;         /**< instant mode시 할당받은 메모리 포인터들의 리스트 */
    stlInt32            mInstMemAllocCount;   /**< instant mode시 할당받은 메모리 block의 개수 */
    stlInt32            mInstMemMaxCount;     /**< instant mode시 할당받을 수 있는 최대 메모리 수; mInstMemList의 길이이기도 함 */

    stlInt32            mVarColPieceMinLen;   /**< var column의 최소 piece 크기.var part block에 이보다 작은 공간은 버린다. */
    stlBool             mBuildBlockDir;       /**< block dir을 만들지 결정. */

    /*
     * 여기 아래부터는 동적인 정보들이다. truncate때 reset해줘야 한다.
     */
    knlLogicalAddr      mRootBlockDirAddr;    /**< 맨 상위 directory block */
    knlLogicalAddr      mFirstFixedPartAddr;  /**< 맨 첫번 fixed part block */
    knlLogicalAddr      mCurBlockDirAddr;     /**< 현재 insert 중인 directory block */
    knlLogicalAddr      mCurFixedPartAddr;    /**< 현재 insert 중인 fixed part block의 논리 주소 */
    knlLogicalAddr      mCurVarPartAddr;      /**< 현재 insert 중인 var part block의 논리 주소 */

    stlInt32            mInstMemIndex;        /**< instant mode시 현재 block의 index; mInstMemAllocCount 이하여야 한다. */
    stlBool             mHasDeletedOnce;      /**< delete된 적이 있는가, 즉 delete된 row가 하나라도 있는가 */
};

#define SMDMIF_TABLE_IS_ONE_BLOCK_TABLE(aHeader) \
    ( (aHeader)->mFirstFixedPartAddr == (aHeader)->mCurFixedPartAddr ? STL_TRUE : STL_FALSE )

#define SMDMIF_TABLE_GET_CUR_FIXED_PART(aHeader) \
    ( knlToPhysicalAddr((aHeader)->mCurFixedPartAddr) )

#define SMDMIF_TABLE_GET_CUR_VAR_PART(aHeader) \
    ( knlToPhysicalAddr((aHeader)->mCurVarPartAddr) )

#define SMDMIF_TABLE_GET_FIRST_FIXED_PART(aHeader)             \
    ( knlToPhysicalAddr((aHeader)->mFirstFixedPartAddr) )

#define SMDMIF_TABLE_GET_FIRST_FIXED_PART_ADDR(aHeader)        \
    ( (aHeader)->mFirstFixedPartAddr )

#define SMDMIF_TABLE_FIRST_INSERTED(aHeader)  \
    ( (aHeader)->mFirstFixedPartAddr == KNL_LOGICAL_ADDR_NULL ? STL_FALSE : STL_TRUE )

#define SMDMIF_TABLE_GET_TOTAL_ROW_COUNT(aHeader)                   \
    ( ( SMDMIF_TABLE_GET_CUR_FIXED_PART( aHeader ) == NULL ) ? 0 :  \
          SMDMIF_FIXED_PART_GET_TOTAL_ROW_COUNT_SO_FAR( SMDMIF_TABLE_GET_CUR_FIXED_PART( aHeader ) ) )


#define SMDMIF_GET_ROW_BY_ROW_ADDR(aRowAddr)    \
    ( knlToPhysicalAddr( *(stlUInt64 *)(&(aRowAddr)) ) )

#define SMDMIF_RID_TO_ROW_ADDR(aRowAddrPtr, aRidPtr)   \
    ( STL_WRITE_INT64( (aRowAddrPtr), aRidPtr ) )

inline stlInt32 smdmifGetBlockSize( smlTbsId aTbsId );

stlStatus smdmifTableCreate( smlStatement  *aStatement,
                             smlTbsId       aTbsId,
                             smlTableAttr  *aAttr,
                             stlBool        aLoggingUndo,
                             stlInt64      *aSegmentId,
                             void         **aRelationHandle,
                             smlEnv        *aEnv);

stlStatus smdmifTableInitHeader( smdmifTableHeader *aHeader,
                                 smlTbsId           aTbsId,
                                 smlRelationType    aRelType,
                                 smdmifTableType    aInstType,
                                 stlBool            aScrollable,
                                 stlInt32           aHeaderSize,
                                 smlEnv            *aEnv );

stlStatus smdmifTableInitColumnInfo( smdmifTableHeader *aHeader,
                                     knlValueBlockList *aColList,
                                     smlEnv            *aEnv );

stlStatus smdmifTableDrop( smlStatement *aStatement,
                           void         *aRelationHandle,
                           smlEnv       *aEnv );

stlStatus smdmifTableCleanup( void    *aRelationHandle,
                              smlEnv  *aEnv );

stlStatus smdmifTableTruncate( smlStatement *aStatement,
                               void         *aRelationHandle,
                               smlEnv       *aEnv );

stlStatus smdmifTableSetFlag( smlStatement      *aStatement,
                              void              *aRelationHandle,
                              smlRowBlock       *aRids,
                              knlValueBlockList *aFlags,
                              smlEnv            *aEnv );

stlStatus smdmifTableFiniHeader( smdmifTableHeader *aHeader,
                                 smlEnv            *aEnv );

stlStatus smdmifTableUnsetDeletion( void   *aRelationHandle,
                                    smlEnv *aEnv );

stlStatus smdmifTableInsert( smlStatement      *aStatement,
                             void              *aRelationHandle,
                             stlInt32           aValueIdx,
                             knlValueBlockList *aInsertCols,
                             knlValueBlockList *aUniqueViolation,
                             smlRid            *aRowRid,
                             smlEnv            *aEnv );

stlStatus smdmifTableBlockInsert( smlStatement      *aStatement,
                                  void              *aRelationHandle,
                                  knlValueBlockList *aInsertCols,
                                  knlValueBlockList *aUniqueViolation,
                                  smlRowBlock       *aRowBlock,
                                  smlEnv            *aEnv );

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
                             smlEnv            *aEnv );

stlStatus smdmifTableDelete( smlStatement      *aStatement,
                             void              *aRelationHandle,
                             smlRid            *aRowRid,
                             smlScn             aRowScn,
                             stlInt32           aValueIdx,
                             knlValueBlockList *aPrimaryKey,
                             stlBool           *aVersionConflict,
                             stlBool           *aSkipped,
                             smlEnv            *aEnv );

stlStatus smdmifTableFetch( smlStatement        *aStatement,
                            smlFetchRecordInfo  *aFetchRecordInfo,
                            smlRid              *aRowRid,
                            stlInt32             aBlockIdx,
                            stlBool             *aIsMatched,
                            stlBool             *aDeleted,
                            stlBool             *aUpdated,
                            smlEnv              *aEnv );

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
                                  smlEnv            *aEnv );

stlStatus smdmifTableFetchOneRowWithRowFilter( smdmifTableHeader *aHeader,
                                               void              *aFixedRow,
                                               knlPhysicalFilter *aRowPhysicalFilter,
                                               knlExprEvalInfo   *aRowLogicalFilterInfo,
                                               knlValueBlockList *aRowFetchCol,
                                               stlInt32           aBlockIdx,
                                               stlBool           *aFetched,
                                               smlEnv            *aEnv );

stlStatus smdmifTableInsertInternal( smdmifTableHeader *aHeader,
                                     knlValueBlockList *aColList,
                                     knlValueBlockList *aUniqueViolation,
                                     stlInt32           aBlockIdx,
                                     smlRid            *aRid,
                                     smlEnv            *aEnv );

stlStatus smdmifTableGetFirstFixedPartAddr( smdmifTableHeader *aHeader,
                                            knlLogicalAddr    *aRetAddr,
                                            smlEnv            *aEnv );

stlStatus smdmifTableGetInsertableFixedPart( smdmifTableHeader   *aHeader,
                                             void               **aFixedPart,
                                             smdmifFixedRow      *aRow,
                                             knlLogicalAddr      *aRowAddr,
                                             smlEnv              *aEnv );

void smdmifTableFindFixedPart( smdmifTableHeader  *aHeader,
                               stlInt64           *aRowPositionNumber,
                               void              **aFixedPart,
                               smlEnv             *aEnv );

void smdmifTableGetColValueAndLen( smdmifFixedRow    aRow,
                                   smdmifDefColumn  *aCol,
                                   stlChar          *aTempMem,
                                   stlChar         **aColValue,
                                   stlInt64         *aColLen );

stlStatus smdmifTableCopyColValue( smdmifFixedRow     aRow,
                                   smdmifDefColumn  * aCol,
                                   dtlDataValue     * aDataValue,
                                   smlEnv           * aEnv );

stlStatus smdmifTableGetLongVaryingValueAndLen( smdmifFixedRow     aRow,
                                                smdmifDefColumn  * aCol,
                                                dtlDataValue     * aDataValue,
                                                stlChar         ** aColValue,
                                                stlInt64         * aColLen,
                                                smlEnv           * aEnv );

stlStatus smdmifTableAllocBlock( smdmifTableHeader  *aHeader,
                                 smdmifDefBlockType  aBlockType,
                                 void              **aBlock,
                                 smlEnv             *aEnv );

stlStatus smdmifTableInitInstantMode( smdmifTableHeader *aHeader,
                                      smlEnv            *aEnv );

stlStatus smdmifTableVerify( smdmifTableHeader *aHeader );

#endif /* _SMDMIF_TABLE_ */

/** @} */
