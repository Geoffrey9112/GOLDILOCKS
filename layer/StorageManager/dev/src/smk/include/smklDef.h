/*******************************************************************************
 * smklDef.h
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


#ifndef _SMKLDEF_H_
#define _SMKLDEF_H_ 1

/**
 * @file smklDef.h
 * @brief Storage Manager Layer Local Lock Component Internal Definition
 */

/**
 * @defgroup smklInternal Component Internal Routines
 * @ingroup smkl
 * @{
 */

typedef struct smklItemCore smklItemCore;
/**
 * @brief 로컬 락 노드 구조체
 */
typedef struct smklNode
{
    stlUInt32       mLockMode;    /**< 락 모드 @see smlLockMode */
    smxlTransId     mTransId;
    smklItemCore  * mLockItem;
    stlRingEntry    mTransLink;   /**< 트랜잭션 링크 */
    stlRingEntry    mNodeLink;    /**< 락 노드 링크 */
    void          * mEnv;
} smklNode;

#define SMKL_RELATION_LATCH_COUNT (8)  /**< Relation Latch count */

struct smklItemCore
{
    stlUInt32     mGrantedMode;      /**< 대표락 모드 @see smlLockMode */
    stlBool       mIsPessimistic;    /**< Pessimistic Mode로 동작하고 있는지 설정 */
    stlBool       mBeFree;           /**< Free 될 Lock Handle 여부 */
    stlInt16      mItemType;         /**< Lock Item Type ( Relation or Record )*/
    stlRingHead   mHoldList;         /**< Exclusive 락을 획득한 노드 리스트 */
    stlRingHead   mRequestList;      /**< 락 획득에 대기하는 노드 리스트 */
    stlChar       mPadding2[8];      /**< padding for cache align */
};

/**
 * @brief 로컬 락 릴레이션 아이템 구조체
 * @note @a mIntentHoldArray는 Lock Item에 래치를 획득하지 않고 잠금 획득을 표시하기 위해서 사용된다.
 * <BR> mIntentHoldArray는 트랜잭션 테이블 사이즈만큼 크기가 할당되고, 트랜잭션마다 고유한 배열원소를
 * <BR> 배제적으로 사용한다. mHoldList가 테이블에 Exclusive 잠금을 획득하는 경우에 사용되는 반면
 * <BR> mIntentHoldArray는 IX나 IS 잠금을 획득하는 경우에 사용한다.
 * <BR> 데이터베이스 연산의 대부분은 IX를 획득하는 경우이며, mIntentHoldArray는 IX 획득 성능을 높이기
 * <BR> 위한 방법이다.
 */
typedef struct smklItemRelation
{
    stlUInt32     mGrantedMode;                       /**< 대표락 모드 @see smlLockMode */
    stlBool       mIsPessimistic;                     /**< Pessimistic Mode로 동작하고 있는지 설정 */
    stlBool       mBeFree;                            /**< Free 될 Lock Handle 여부 */
    stlInt16      mItemType;                          /**< Lock Item Type ( Relation or Record )*/
    stlRingHead   mHoldList;                          /**< Exclusive 락을 획득한 노드 리스트 */
    stlRingHead   mRequestList;                       /**< 락 획득에 대기하는 노드 리스트 */
    stlChar       mPadding2[8];                       /**< padding for cache align */
    knlLatch      mLatch[SMKL_RELATION_LATCH_COUNT];  /**< 락 아이템 보호를 위한 래치 */
    stlChar     * mIntentHoldArray;                   /**< Intention 락을 획득한 트랜잭션들의 배열 */
    void        * mRelationHandle;                    /**< relation handle */
    stlChar       mPadding3[44];                      /**< align cache line */
} smklItemRelation;

#define SMKL_LOCK_USAGE_DDL   ( 0 )
#define SMKL_LOCK_USAGE_DML   ( 1 )

typedef struct smklItemRecord
{
    stlUInt32     mGrantedMode;      /**< 대표락 모드 @see smlLockMode */
    stlBool       mIsPessimistic;    /**< Pessimistic Mode로 동작하고 있는지 설정 */
    stlBool       mBeFree;           /**< Free 될 Lock Handle 여부 */
    stlInt16      mItemType;         /**< Lock Item Type ( Relation or Record )*/
    stlRingHead   mHoldList;         /**< Exclusive 락을 획득한 노드 리스트 */
    stlRingHead   mRequestList;      /**< 락 획득에 대기하는 노드 리스트 */
    stlRingEntry  mItemLink;         /**< Hash Bucket에 연결되는 Item Link */
    stlInt16      mUsageType;        /**< usage type ( normal record for update or meta record for ddl */
    smlRid        mRid;              /**< Record Identifier */
} smklItemRecord;

typedef struct smklHashBucket
{
    knlLatch        mLatch;
    stlRingHead     mItemList;
    knlDynamicMem   mShmMem;
    stlChar         mPadding[16];
} smklHashBucket;

#define SMKL_PARENT_SHMMEM_CHUNK_SIZE     ( 1024 * 100 )
#define SMKL_SHMMEM_CHUNK_SIZE            ( 1024 )

typedef struct smklLockSlot
{
    stlRingHead mLockNodeList;
    stlRingHead mRelationList;
    knlCondVar  mTransLock;           /**< Transaction 대기를 위한 Conditional Variable */
    stlChar     mPadding[48];         /**< Cache Alignment */
} smklLockSlot;

typedef struct smklRelation
{
    void         * mRelationHandle;
    stlRingEntry   mRelationLink;
} smklRelation;

/**
 * @brief Process Warmup을 위한 Entry 구조체
 */
typedef struct smklWarmupEntry
{
    knlLatch         mDeadlockLatch;         /**< Deadlock을 검출하기 위한 래치 */
    smklLockSlot   * mLockTable;             /**< Warmup시 Lock 테이블 구성을 위한 Entry Point */
    smklHashBucket * mLockHashTable;         /**< Warmup시 Lock Hash 테이블 구성을 위한 Entry Point */
    stlChar        * mLockWaitTable;         /**< Warmup시 Lock Wait Table 구성을 위한 Entry Point */
    stlUInt32        mLockTableSize;         /**< Warmup시 Lock 테이블 사이즈 구성을 위한 Entry Point */
    stlUInt32        mLockWaitTableSize;     /**< Warmup시 Lock Wait 테이블 사이즈 구성을 위한 Entry Point */
    stlUInt32        mLockHashTableSize;     /**< Warmup시 Lock 해시 테이블 사이즈 구성을 위한 Entry Point */
    knlDynamicMem    mShmMemForLock;         /**< Parent allocator for lock tables */

    /**
     * Shared Request Group에 속한 Worker들의 개수
     */
    stlInt32         mGroupWorkerCount[KNL_PROPERTY_MAX(KNL_PROPERTY_SHARED_REQUEST_QUEUE_COUNT)];
} smklWarmupEntry;

extern smklWarmupEntry * gSmklWarmupEntry;

extern smklLockSlot   * gSmklLockTable;
extern smklHashBucket * gSmklHashTable;
extern stlChar        * gSmklWaitTable;
extern stlInt32         gSmklLockTableSize;
extern stlInt32         gSmklLockWaitTableSize;
extern stlInt32         gSmklHashTableSize;

#define SMKL_LOCK_TABLE( aSlotId )       ( gSmklLockTable[(aSlotId)] )
#define SMKL_LOCK_HASH_TABLE( aSlotId )  ( gSmklHashTable[(aSlotId)] )
#define SMKL_LOCK_WAIT_TABLE( aSlotId )  ( gSmklWaitTable[(aSlotId)] )
#define SMKL_LOCK_TABLE_SIZE             ( gSmklLockTableSize )
#define SMKL_LOCK_WAIT_TABLE_SIZE        ( gSmklLockWaitTableSize )
#define SMKL_LOCK_HASH_BUCKET_COUNT      ( gSmklHashTableSize )

/**
 * @brief aLockSlotIdB에 대기할 aLockSlotIdA를 설정한다.
 * @remark (aLockSlotIdA) --> (aLockSlotIdB)
 */
#define SMKL_ADD_EDGE( aLockSlotIdA, aLockSlotIdB )                                             \
    {                                                                                           \
        SMKL_LOCK_WAIT_TABLE( (aLockSlotIdB * SMKL_LOCK_WAIT_TABLE_SIZE) + aLockSlotIdA ) = 1;  \
    }

/**
 * @brief 아직도 대기하고 있는지 여부
 */
#define SMKL_HAS_EDGE( aLockSlotIdA, aLockSlotIdB )                             \
    ( (SMKL_LOCK_WAIT_TABLE( ((aLockSlotIdB) * SMKL_LOCK_WAIT_TABLE_SIZE) +     \
                             (aLockSlotIdA) ) == 1) ? STL_TRUE : STL_FALSE )

/**
 * @brief Deadlock Edge를 설정한다.
 */
#define SMKL_SET_DEADLOCK_EDGE( aLockSlotIdA, aLockSlotIdB )                                    \
    {                                                                                           \
        SMKL_LOCK_WAIT_TABLE( (aLockSlotIdB * SMKL_LOCK_WAIT_TABLE_SIZE) + aLockSlotIdA ) = 2;  \
    }

/**
 * @brief aLockSlotIdB에 aLockSlotIdA에 대한 대기 정보를 삭제한다.
 * @remark (aLockSlotIdA) --> (aLockSlotIdB)
 */
#define SMKL_REMOVE_EDGE( aLockSlotIdA, aLockSlotIdB )                                          \
    {                                                                                           \
        SMKL_LOCK_WAIT_TABLE( (aLockSlotIdB * SMKL_LOCK_WAIT_TABLE_SIZE) + aLockSlotIdA ) = 0;  \
    }

#define SMKL_GROUP_WORKER_COUNT( aRequestGroupId )              \
    gSmklWarmupEntry->mGroupWorkerCount[(aRequestGroupId)]

typedef struct smklFxLockWait
{
    stlInt16 mGrantedTransSlotId;
    stlInt16 mRequestTransSlotId;
} smklFxLockWait;

typedef struct smklFxPathCtrl
{
    stlInt16 mX;
    stlInt16 mY;
} smklFxPathCtrl;

typedef struct smklDeadlockLink smklDeadlockLink;

#define SMKL_DEADLOCK_TRUE  (1)
#define SMKL_DEADLOCK_FALSE (2)
#define SMKL_DEADLOCK_EXIT  (3)

struct smklDeadlockLink
{
    stlInt16           mSlotId;
    smklDeadlockLink * mPrev;
    
};

/** @} */
    
#endif /* _SMKLDEF_H_ */
