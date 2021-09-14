/*******************************************************************************
 * eldDictionary.h
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


#ifndef _ELD_DICTIONARY_H_
#define _ELD_DICTIONARY_H_ 1

/**
 * @file eldDictionary.h
 * @brief Execution Layer 의 Dictionary 관리 공통 모듈 
 */

/**
 * @defgroup eldMemory Dictionary 메모리 관리 
 * @ingroup eldDict
 * @{
 */

extern stlBool gEldCacheBuilding;

/**
 * @brief Column 의 Ordinal Position 값
 * @remarks
 * - DATABASE 는 표준과 동일하게 1부터 시작하도록 저장 
 * - PROCESS 상에서는 구현의 용이성을 위해 0부터 시작하도록 조정 
 */
#define ELD_ORDINAL_POSITION_TO_DATABASE(aPosition)  ( aPosition + 1 )
#define ELD_ORDINAL_POSITION_TO_PROCESS(aPosition)   ( aPosition - 1 )

extern void ** gEldTablePhysicalHandle;
extern void ** gEldIdentityColumnHandle;

extern void * gEldNonCacheIndexPhyHandle4DTD;
extern void * gEldNonCacheIndexPhyHandle4ColTableID;
extern void * gEldNonCacheIndexPhyHandle4ViewedTableID;
extern void * gEldNonCacheIndexPhyHandle4IndexKeyColIndexID;
extern void * gEldNonCacheIndexPhyHandle4CheckConstID;
extern void * gEldNonCacheIndexPhyHandle4KeyColConstID;
extern void * gEldNonCacheIndexPhyHandle4RefConstID;
extern void * gEldNonCacheIndexPhyHandle4ChkColConstID;

/**
 * @brief Dictionary Memory Free 를 위한 Pending Argument
 * @remarks
 */
typedef struct eldMemFreePendArg
{
    knlDynamicMem * mMemMgr;       /**< 메모리 관리자 */
    void          * mAddr;         /**< 제거할 메모리 주소 */
} eldMemFreePendArg;

typedef enum
{
    ELD_AGING_FREE_DICTIONARY_MEMORY = 1, /** mArg1: Memory Manager mArg2: Target Address      mArg3 : N/A */
    ELD_AGING_UNLINK_HASH_ELEMENT,        /** mArg1: Hash Bucket    mArg2: Hash Element        mArg3 : N/A */
    ELD_AGING_UNLINK_HASH_RELATED,        /** mArg1: Hash Element   mArg2: Hash Related Head   mArg3 : Hash Related */
    ELD_AGING_FREE_HASH_ELEMENT,          /** mArg1: N/A            mArg2: Hash Element        mArg3 : N/A */
    ELD_AGING_FREE_HASH_RELATED           /** mArg1: N/A            mArg2: N/A                 mArg3 : Hash Related */
} eldAgingEventType;

#define ELD_MAX_AGING_EVENT_COUNT   ( 4 )

typedef struct eldMemFreeAgingEvent
{
    smlScn              mSCN;
    eldAgingEventType   mEventType;
    void              * mArg1;      /**< Event Argument 1 */
    void              * mArg2;      /**< Event Argument 2 */
    void              * mArg3;      /**< Event Argument 3 */
} eldMemFreeAgingEvent;


typedef struct eldMemMark
{
    ellLongTypeBlock  * mLongTypeBlock;
    knlRegionMark       mMemMark;
} eldMemMark;

/*
 * Dictionary Memory 관리 함수
 */

stlStatus eldAllocOPMem( void ** aAddr, stlUInt32 aSize, ellEnv * aEnv );
stlStatus eldClearOPMem( ellEnv * aEnv );

stlStatus eldAllocPendingMem( void ** aAddr, stlUInt32 aArgSize, ellEnv * aEnv );
stlStatus eldClearPendingMem( void * aAddr, ellEnv * aEnv );

stlStatus eldAllocCacheMem( void ** aAddr, stlUInt32 aSize, ellEnv * aEnv );
stlStatus eldAllocHashElementMem( eldcHashElement ** aAddr, ellEnv * aEnv );
stlStatus eldAllocHashRelatedMem( eldcHashRelatedObjList ** aAddr, ellEnv * aEnv );

stlStatus eldAddAgingEventDictMem( smlTransId     aTransID,
                                   void         * aAgingEvent,
                                   void         * aEventBuffer,
                                   ellEnv       * aEnv );
stlStatus eldAgingDictMem( void * aData,stlUInt32 aDataSize,stlBool * aDone,void * aEnv );


/*
 * Dictionary 생성을 위한 Value List 관리 함수
 */

stlInt64 eldGetPrecisionFromDomain( eldtDefinitionDomainNO      aDomainNO );

dtlDataValue * eldFindDataValue( knlValueBlockList * aTableValueList,
                                 stlInt64            aTableID,
                                 stlInt32            aColumnOrder );

stlStatus eldAllocTableValueList( eldtDefinitionTableID     aDictTableID,
                                  eldMemMark              * aMemMark,
                                  knlValueBlockList      ** aTableValueList,
                                  ellEnv                  * aEnv );

stlStatus eldFreeTableValueList( eldMemMark  * aMemMark,
                                 ellEnv      * aEnv );

stlStatus eldShareKeyConstIndexValueList( eldtDefinitionKeyConstDesc  * aKeyConstDesc,
                                          knlValueBlockList           * aTableValueList,
                                          knlValueBlockList          ** aIndexValueList,
                                          ellEnv                      * aEnv );

stlStatus eldShareNormalIndexValueList( eldtDefinitionIndexDesc * aIndexDesc,
                                        knlValueBlockList       * aTableValueArray,
                                        knlValueBlockList      ** aIndexValueArray,
                                        ellEnv                  * aEnv );

stlStatus eldAllocFixedValueList( void                * aFxTableHeapDesc,
                                  eldMemMark          * aMemMark,
                                  knlValueBlockList  ** aValueArray,
                                  ellEnv              * aEnv );

stlStatus eldSetColumnValue( eldtDefinitionTableID      aDictTableID,
                             stlInt32                   aColumnOrder,
                             knlValueBlockList        * aValueList,
                             void                     * aInputData,
                             ellEnv                   * aEnv );

/** @} eldMemory */






/**
 * @defgroup eldInitFini Dictionary 초기화, 마무리 함수 
 * @ingroup eldDict
 * @{
 */

/*
 * 초기화, 마무리 함수
 */

stlStatus eldBuildDictionaryCache( ellEnv * aEnv );

stlStatus eldSetDictionaryHandle( ellEnv * aEnv );
    
stlStatus eldSetDictionaryTableHandle( smlTransId     aTransID,
                                       smlStatement * aStmt,
                                       ellEnv       * aEnv );

stlStatus eldSetDictionaryIdentityHandle( smlTransId     aTransID,
                                          smlStatement * aStmt,
                                          ellEnv       * aEnv );

stlStatus eldSetDictionaryDumpHandle( ellEnv * aEnv );

/** @} eldInitFini */








/**
 * @defgroup eldQuery Dictionary 질의 처리 보조 함수 
 * @ingroup eldDict
 * @{
 */
                             
stlStatus eldBeginForTableAccessQuery( smlStatement         * aStmt,
                                       void                 * aTableHandle,
                                       smlIteratorProperty  * aIteratorProperty,
                                       smlRowBlock          * aRowBlock,
                                       knlValueBlockList    * aTableReadColList,
                                       knlPredicateList     * aFilterPred,
                                       knlExprContextInfo   * aFilterContext,
                                       smlFetchInfo         * aFetchInfo,
                                       knlExprEvalInfo      * aFilterEvalInfo,
                                       void                ** aIterator,
                                       ellEnv               * aEnv );

stlStatus eldBeginForIndexAccessQuery( smlStatement         * aStmt,
                                       void                 * aTableHandle,
                                       void                 * aIndexHandle,
                                       knlKeyCompareList    * aKeyCompList,
                                       smlIteratorProperty  * aIteratorProperty,
                                       smlRowBlock          * aRowBlock,
                                       knlValueBlockList    * aTableReadColList,
                                       knlValueBlockList    * aIndexReadColList,
                                       knlPredicateList     * aRangePred,
                                       knlExprContextInfo   * aRangeContext,
                                       smlFetchInfo         * aFetchInfo,
                                       smlFetchRecordInfo   * aFetchRecordInfo,
                                       void                ** aIterator,
                                       ellEnv               * aEnv );

stlStatus eldEndForQuery( void    * aIterator,
                          ellEnv  * aEnv );


stlStatus eldMakeOneFilter( knlPredicateList    ** aFilterPred,
                            knlExprContextInfo  ** aFilterContext,
                            knlValueBlockList    * aColumnValue,
                            knlBuiltInFunc         aFuncID,
                            void                 * aConstant,
                            stlInt64               aConstantLength,
                            ellEnv               * aEnv );

stlStatus eldMakeOneEquiRange( smlTransId              aTransID,
                               smlStatement          * aStmt,
                               eldtDefinitionTableID   aTableID,
                               knlValueBlockList     * aColumnValue,
                               void                  * aConstant,
                               stlInt64                aConstantLength,
                               void                 ** aIndexHandle,
                               knlKeyCompareList    ** aKeyCompList,
                               knlValueBlockList    ** aIndexValueList,
                               knlPredicateList     ** aRangePred,
                               knlExprContextInfo   ** aRangeContext,
                               ellEnv                * aEnv );

stlStatus eldFetchNext( void          * aIterator,
                        smlFetchInfo  * aFetchInfo,
                        ellEnv        * aEnv );

stlStatus eldLockRowByObjectID( smlTransId              aTransID,
                                smlStatement          * aStmt,
                                eldtDefinitionTableID   aDictTableID,
                                stlInt32                aObjectColumnIdx,
                                stlInt64                aObjectID,
                                stlInt32                aLockMode,
                                stlBool               * aLockSuccess,
                                ellEnv                * aEnv );

stlStatus eldLockRowByObjectHandle( smlTransId              aTransID,
                                    smlStatement          * aStmt,
                                    eldtDefinitionTableID   aDictTableID,
                                    stlInt32                aObjectColumnIdx,
                                    ellDictHandle         * aObjectHandle,
                                    stlInt32                aLockMode,
                                    stlBool               * aLockSuccess,
                                    ellEnv                * aEnv );


stlStatus eldGetObjectList( smlTransId              aTransID,
                            smlStatement          * aStmt,
                            eldtDefinitionTableID   aTableID,
                            ellObjectType           aTargetObjectType,
                            stlInt32                aTargetColumnIdx,
                            stlInt32                aFilterColumnIdx,
                            stlInt64                aFilterValue,
                            knlRegionMem          * aRegionMem,
                            ellObjectList         * aDDLObjectList,
                            ellEnv                * aEnv );

stlStatus eldGetPrivList( smlTransId              aTransID,
                          smlStatement          * aStmt,
                          ellPrivObject           aPrivObject,
                          stlInt32                aFilterColumnIdx,
                          stlInt64                aFilterValue,
                          knlRegionMem          * aRegionMem,
                          ellPrivList           * aPrivList,
                          ellEnv                * aEnv );

stlStatus eldDeleteObjectRows( smlTransId              aTransID,
                               smlStatement          * aStmt,
                               eldtDefinitionTableID   aTableID,
                               stlInt32                aFilterColumnIdx,
                               stlInt64                aFilterValue,
                               ellEnv                * aEnv );

stlStatus eldDeletePrivRowsAndCache( smlTransId              aTransID,
                                     smlStatement          * aStmt,
                                     eldtDefinitionTableID   aTableID,
                                     stlInt32                aFilterColumnIdx,
                                     stlInt64                aFilterValue,
                                     ellEnv                * aEnv );

stlStatus eldDeletePrivRowAndCacheByPrivDesc( smlTransId     aTransID,
                                              smlStatement * aStmt,
                                              ellPrivDesc  * aPrivDesc,
                                              ellEnv       * aEnv );

stlStatus eldModifyDictDesc( smlTransId              aTransID,
                             smlStatement          * aStmt,
                             eldtDefinitionTableID   aDictTableID,
                             stlInt32                aObjectColumnIdx,
                             stlInt64                aObjectID,
                             stlInt32                aModifyColumnIdx,
                             void                  * aModifyData,
                             ellEnv                * aEnv );

stlStatus eldDeleteDictDesc4Modify( smlTransId               aTransID,
                                    smlStatement           * aStmt,
                                    eldtDefinitionTableID    aDictTableID,
                                    stlInt32                 aObjectColumnIdx,
                                    stlInt64                 aObjectID,
                                    eldMemMark             * aMemMark,
                                    knlValueBlockList     ** aDeletedValueList,
                                    ellEnv                 * aEnv );

stlStatus eldInsertDictDesc4Modify( smlTransId               aTransID,
                                    smlStatement           * aStmt,
                                    eldtDefinitionTableID    aDictTableID,
                                    eldMemMark             * aMemMark,
                                    knlValueBlockList      * aInsertValueList,
                                    ellEnv                 * aEnv );

/** @} eldQuery */



#endif /* _ELD_DICTIONARY_H_ */
