
/**
 * @file smdmifDef.h
 * @brief Storage Manager Layer Memory Instant Flat Table Definitions
 */

/**
 * @defgroup smdmif Memory Instant Flat Table
 * @ingroup smInternal
 * @{
 */

#ifndef _SMDMIFDEF_H_
#define _SMDMIFDEF_H_ 1

/**
 * @brief Instant table의 컬럼 정보 구조체
 */
typedef struct smdmifDefColumn
{
    stlInt32      mOffset;       /**< variablePart일 땐 컬럼 순번 */
    dtlDataType   mDataType;
    stlInt32      mAllocSize;
    stlBool       mIsFixedPart;  /**< fixed part에 저장될 컬럼인지의 여부*/

    /* 아래 정보는 동적 정보이다. truncate시 reset이 필요하다.*/
    stlBool       mHasNull;      /**< 이 컬럼에 null이 들어간 적이 있는지의 여부 */
    stlUInt16     mMaxActualLen; /**< table에 insert될 때 최대 크기를 저장한다. 이 정보는 공간 효율을 위해 mAllocSize대신 사용된다. */
} smdmifDefColumn;

typedef enum
{
    SMDMIF_DEF_BLOCK_FIXED_PART = 1,
    SMDMIF_DEF_BLOCK_VAR_PART,
    SMDMIF_DEF_BLOCK_DIR,
    SMDMIF_DEF_BLOCK_INDEX_NODE
} smdmifDefBlockType;

typedef struct smdmifDefBlockHeader
{
    knlLogicalAddr      mMyAddr;
    smdmifDefBlockType  mType;
    stlUInt32           mItemCount;
} smdmifDefBlockHeader;

#define SMDMIF_DEF_BLOCK_ADDR(aHeader)     \
    ( ((smdmifDefBlockHeader *)(aHeader))->mMyAddr )

#define SMDMIF_DEF_BLOCK_TYPE(aHeader)     \
    ( ((smdmifDefBlockHeader *)(aHeader))->mType )

#define SMDMIF_DEF_BLOCK_ITEM_COUNT(aHeader) \
    ( ((smdmifDefBlockHeader *)(aHeader))->mItemCount )

#define SMDMIF_DEF_BLOCK_INIT(aHeader, aHeaderAddr, aBlockType)    \
    ((smdmifDefBlockHeader *)(aHeader))->mMyAddr = (aHeaderAddr);  \
    ((smdmifDefBlockHeader *)(aHeader))->mType = (aBlockType);     \
    ((smdmifDefBlockHeader *)(aHeader))->mItemCount = 0

#endif   /* _SMDMIFDEF_H_ */


/** @} */

