/*******************************************************************************
 * knsSegment.h
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


#ifndef _KNSSEGMENT_H_
#define _KNSSEGMENT_H_ 1

/**
 * @file knsSegment.h
 * @brief Kernel Layer Shared Memory Segment Internal Routines
 */

/**
 * @defgroup knsSegment Shared Memory Segment
 * @ingroup knInternal
 * @internal
 * @{
 */

/**
 * @brief 공유메모리 세그먼트 타입
 */
typedef enum
{
    KNS_SHM_SEGMENT_TYPE_STATIC = 0, /**< static 공유 메모리 세그먼트 */
    KNS_SHM_SEGMENT_TYPE_DATABASE,   /**< database 공유 메모리 세그먼트 */
    KNS_SHM_SEGMENT_TYPE_DYNAMIC,    /**< dynamic 공유 메모리 세그먼트 */
    KNS_SHM_SEGMENT_TYPE_INVALID
} knsShmSemgnetType;

stlStatus knsInitializeAddrTable( void   * aStaticArea,
                                  knlEnv * aEnv );
stlStatus knsFinalizeAddrTable( knlEnv * aEnv );

/*
 * static area
 */
stlBool   knsValidateStaticArea();
stlStatus knsCreateStaticArea( knlEnv * aEnv );
stlStatus knsAttachStaticArea( knlEnv * aEnv );
stlStatus knsDetachStaticArea( knlEnv * aEnv );
stlStatus knsDestroyStaticArea( knlEnv * aEnv );

stlStatus knsInitVariableStaticArea( void    * aStartAddr,
                                     stlSize   aSize,
                                     knlEnv  * aEnv );

stlStatus knsAllocVariableStaticArea( stlSize   aSize,
                                      void   ** aAddr,
                                      knlEnv  * aEnv );

stlStatus knsFreeVariableStaticArea( void   * aAddr,
                                     knlEnv * aEnv );

stlSize knsGetVariableStaticAreaTotalSize();
stlSize knsGetVariableStaticAreaFragSize();

/*
 * shared memory event handler
 */
stlStatus knsAttachShmEventHandler( void      * aData,
                                    stlUInt32   aDataSize,
                                    stlBool   * aDone,
                                    void      * aEnv );
stlStatus knsDetachShmEventHandler( void      * aData,
                                    stlUInt32   aDataSize,
                                    stlBool   * aDone,
                                    void      * aEnv );

/*
 * shared memory segment 관리
 */
stlStatus knsCreateSegment( knsShmSemgnetType    aType,
                            stlChar            * aName,
                            stlUInt16          * aIdx,
                            stlSize              aReqSize,
                            knlEnv             * aEnv );

stlStatus knsAttachSegment( stlUInt16  aIdx,
                            stlBool    aDoLatch,
                            knlEnv   * aEnv );

stlStatus knsAttachAllSegment( knlEnv * aEnv );
stlStatus knsDetachAllSegment( knlEnv * aEnv );

stlStatus knsDetachSegment( stlUInt16   aIdx,
                            knlEnv    * aEnv );

stlStatus knsReattachSegment( stlUInt16   aIdx,
                              knlEnv    * aEnv );
stlStatus knsDestroySegment( stlUInt16   aIdx,
                             knlEnv    * aEnv );

stlStatus knsDestroyAllSegment( knlEnv * aEnv );

/*
 * logical address
 */
stlStatus knsGetLogicalAddr( void           * aPysicalAddr,
                             knlLogicalAddr * aLogicalAddr,
                             knlEnv         * aEnv );

/*
 * Get
 */
stlStatus knsGetSegmentIdxByName( stlChar    * aName,
                                  stlUInt16  * aIdx,
                                  knlEnv     * aEnv );

/*
 * X$LATCHES : ShmManager Latch
 */
stlStatus knsOpenShmManagerLatch();
stlStatus knsCloseShmManagerLatch();
stlStatus knsGetNextShmManagerLatch( knlLatch ** aLatch,
                                     stlChar   * aBuf,
                                     void      * aPrevPosition,
                                     knlEnv    * aErrorStack );
/*
 * X$SHM_SEGMENT
 */
stlStatus openFxShmCallback( void   * aStmt,
                             void   * aDumpObjHandle,
                             void   * aPathCtrl,
                             knlEnv * aEnv );

stlStatus closeFxShmCallback( void   * aDumpObjHandle,
                              void   * aPathCtrl,
                              knlEnv * aEnv );

stlStatus buildRecordFxShmCallback( void              * aDumpObjHandle,
                                    void              * aPathCtrl,
                                    knlValueBlockList * aValueList,
                                    stlInt32            aBlockIdx,
                                    stlBool           * aTupleExist,
                                    knlEnv            * aEnv );
/** @} */
    
#endif /* _KNSSEGMENT_H_ */
