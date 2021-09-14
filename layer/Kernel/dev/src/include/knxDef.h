/*******************************************************************************
 * knxDef.h
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


#ifndef _KNX_DEF_H_
#define _KNX_DEF_H_ 1

/**
 * @file knxDef.h
 * @brief Kernel Layer 의 Fixed Table 을 위한 내부 자료 구조 정의
 */

/**
 * @defgroup KNX Fixed Table
 * @ingroup knInternal
 * @internal
 * @{
 */
/** @} */

/**
 * @defgroup KNXDEF Fixed Table 내부 자료 구조
 * @ingroup KNX
 * @internal
 * @{
 */

/**
 * @brief 테스트 용도의 버퍼 공간의 크기
 */
#define KNX_TESTPRINT_BUFFER_SIZE   (4096)


/**
 * @brief Shared 영역에 구축된 Fixed Table 의 Relation Header
 * @remarks
 * Multiplexing 등을 위해 Fixed Table 의 Relation Header 는 공유 메모리상에 위치해야 함.
 */
typedef struct knxFxTableRelationHeader
{
    /*
     * Storage Manager (smeRelationHeader) 와 약속된 영역
     */
      
    stlUInt16    mRelationType;    /**< Relation Type */
    stlUInt16    mRelHeaderSize;   /**< 사용 안함, 0으로 설정 */
    stlUInt16    mRelState;        /**< 사용 안함 */
    stlChar      mPadding[2];
    stlUInt64    mCreateTransId;
    stlUInt64    mDropTransId;
    stlUInt64    mCreateScn;
    stlUInt64    mDropScn;
    stlUInt64    mObjectScn;       /**< 0으로 초기화 */
    stlUInt64    mSegmentHandle;   /**< 사용 안함 */

    /*
     * Fixed Table Descriptor 의 ID
     */

    stlInt32     mFxHeapDescID;    /**< Fixed Table 의 Heap Descriptor ID */
    
} knxFxTableRelationHeader;

/**
 * @brief Heap 영역에 구축된 Fixed Table Descriptor
 * @remarks
 */
typedef struct knxFxTableHeapDesc
{
    /*
     * Fixed Table Descriptor 
     */

    knlFxTableUsageType          mUsageType;       /**< Fixed Table 의 사용 방식 */
    knlStartupPhase              mStartupPhase;    /**< 사용가능한 Startup 단계 */

    knlGetDumpObjectCallback     mGetDumpObjectFunc; /**< Dump Object의 Handle 획득 함수 */
    knlOpenFxTableCallback       mOpenFunc;          /**< Fixed Table 의 열기 함수 */
    knlCloseFxTableCallback      mCloseFunc;         /**< Fixed Table 의 닫기 함수 */
    knlBuildFxRecordCallback     mBuildRecordFunc;   /**< 레코드 생성 함수 */

    stlInt32                     mPathCtrlSize;
                                 /**< 사용자가 정의한 Path Controller 정보의 크기 */
    
    stlChar                    * mTableName;    /**< Fixed Table 이름 */
    stlChar                    * mTableComment; /**< Fixed Table 에 대한 주석*/

    stlInt32                     mColumnCount;  /**< 컬럼의 갯수 */
    knlFxColumnDesc            * mColumnDesc;   /**< 컬럼 정의 정보 Array */

    stlRingEntry                 mLink;            /**< 연결 정보 */
} knxFxTableHeapDesc;


/**
 * @brief Fixed Table Manager
 * @remarks
 * <BR> Process내에 정의된 Fixed Table을 관리한다.
 * <BR> 하나의 Process당 하나씩 존재한다.
 */
typedef struct knxFxTableMgr
{
    knlRegionMem          mMemMgr;        /**< Fixed Table List 관리를 위한 Process 메모리 관리자 */
    stlInt32              mFxTableCount;  /**< 등록된 Fixed Table 갯수 */
    stlRingHead           mFxTableList;   /**< 등록된 Fixed Table List */
    knxFxTableHeapDesc ** mFxHeapDescMap; /**< 등록된 Fixed Table 의 Heap Descriptor Map */
} knxFxTableMgr;

extern knxFxTableMgr gKnxFxTableMgr;



/** @} */

#endif /* _KNX_DEF_H_ */
