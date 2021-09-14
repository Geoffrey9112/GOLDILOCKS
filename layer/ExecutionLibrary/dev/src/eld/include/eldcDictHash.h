/*******************************************************************************
 * eldcDictHash.h
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


#ifndef _ELDC_DICT_HASH_H_
#define _ELDC_DICT_HASH_H_ 1

/**
 * @file eldcDictHash.h
 * @brief Dictionary Cache 를 위한 전용 Hash 
 */


/**
 * @addtogroup eldcDictHash
 * @{
 */

/**
 * @brief Pre-ROLLBACK 시 수행할 Pending Argument
 * @brief
 */
typedef struct eldcSetFlagRollbackArg
{
    ellVersionInfo * mVersionInfo;
    stlBool          mDropFlag;
} eldcSetFlagRollbackArg;

/**
 * @brief Dictionary Handle 에 COMMIT SCN 설정 Pending Argument
 * @brief
 */
typedef struct eldcSetHandleScnPendArg
{
    ellDictHandle * mHandle;
} eldcSetHandleScnPendArg;

/**
 * @brief Hash Element 을 제어를 위한 Pending Argument
 * @remarks
 */
typedef struct eldcElementPendArg
{
    eldcHashElement * mElement;       /**< Hash Element */
} eldcElementPendArg;

/**
 * @brief Hash Bucket, Hash Element 제어를 위한 Pending Argument
 * @remarks
 */
typedef struct eldcBucketElementPendArg
{
    eldcDictHash    * mDictHash;      /**< Dictionary Hash 관리자 */
    stlUInt32         mBucketNO;      /**< Bucket Number */
    eldcHashElement * mElement;       /**< 제거할 Hash Element */
} eldcBucketElementPendArg;

/**
 * @brief Hash Element 와 Hash Related Object 를 위한 Pending Argument
 */
typedef struct eldcHashElementHashRelatedPendArg
{
    eldcHashElement         * mHashElement;   /* Hash Element */
    eldcHashRelatedObjList ** mStartRelated;  /* Start Position of Related List */
    eldcHashRelatedObjList  * mHashRelated;   /* Related Object */
} eldcHashElementHashRelatedPendArg;


/**
 * @brief Hash Related Object 를 위한 Pending Argument
 */
typedef struct eldcHashRelatedPendArg
{
    eldcHashElement         * mHashElement;   /* Hash Element */
    eldcHashRelatedObjList  * mHashRelated;   /* Related Object */
} eldcHashRelatedPendArg;



/*
 * 조회 함수
 */

void eldcSetStableVersionInfo( ellVersionInfo * aVersionInfo );

stlStatus eldcIsRecentHandle( smlTransId      aMyTransID,
                              ellDictHandle * aDictHandle,
                              stlBool       * aIsValid,
                              ellEnv        * aEnv );

stlStatus eldcIsVisibleCache( smlTransId       aMyTransID,
                              smlScn           aViewSCN,
                              ellVersionInfo * aVersionInfo,
                              stlBool        * aIsValid,
                              ellEnv         * aEnv );

/*
 * 초기화, 마무리 함수 
 */

stlStatus eldcBuildFixedCache( ellEnv * aEnv );

stlStatus eldcRunPendOP4BuildCache( smlTransId     aTransID,
                                    smlStatement * aStmt,
                                    ellEnv       * aEnv );

stlStatus eldcBuildObjectCache( ellEnv * aEnv );

stlStatus eldcBuildPrivCache( ellEnv * aEnv );

stlStatus eldcSetIndexPhyHandle4NonCache( smlStatement        * aStmt,
                                          ellEnv              * aEnv );

stlStatus eldcMakeNonCacheRange( smlTransId              aTransID,
                                 smlStatement          * aStmt,
                                 knlValueBlockList     * aColumnValue,
                                 stlInt64                aID,
                                 knlKeyCompareList    ** aKeyCompList,
                                 knlValueBlockList    ** aIndexValueList,
                                 knlPredicateList     ** aRangePred,
                                 knlExprContextInfo   ** aRangeContext,
                                 ellEnv                * aEnv );


/*
 * Cache 공통 함수
 */

void       eldcPrintINVALID( void    * aObjectDesc,
                             stlChar * aStringBuffer );

stlStatus  eldcGetTableRecordCount( smlTransId            aTransID,
                                    smlStatement        * aStmt,
                                    eldtDefinitionTableID aTableID,
                                    stlInt64            * aRecordCnt,
                                    ellEnv              * aEnv );
    
stlStatus eldcMakeHashElement( eldcHashElement ** aHashElement,
                               smlTransId         aTransID,
                               void             * aHashData,
                               stlUInt32          aHashValue,
                               void             * aObjectDesc,
                               ellEnv           * aEnv );



/*
 * Hash 제어 함수 
 */

stlStatus eldcCreateDictHash( eldcDictHash ** aDictHash,
                              stlUInt32       aBucketCnt,
                              ellEnv        * aEnv );

stlStatus eldcFindHashElement( smlTransId         aTransID,
                               smlScn             aViewSCN,
                               eldcDictHash     * aDictHash,
                               void             * aHashKey,
                               eldcHashFunc       aHashFunc,
                               eldcKeyCompareFunc aCompareFunc,
                               eldcHashElement ** aHashElement,
                               ellEnv           * aEnv );

stlStatus eldcGetFirstHashElement( eldcDictHash     * aDictHash,
                                   stlUInt32        * aBucketNO,
                                   eldcHashElement ** aHashElement,
                                   ellEnv           * aEnv );

stlStatus eldcGetNextHashElement( eldcDictHash     * aDictHash,
                                  stlUInt32        * aBucketNO,
                                  eldcHashElement ** aHashElement,
                                  ellEnv           * aEnv );

stlStatus eldcInsertHashElement( eldcDictHash    * aDictHash,
                                 eldcHashElement * aHashElement,
                                 ellEnv          * aEnv );

stlStatus eldcDeleteHashElement( smlTransId         aTransID,
                                 eldcDictHash     * aDictHash,
                                 eldcHashElement  * aHashElement,
                                 ellEnv           * aEnv );


stlStatus eldcAddHashRelated( smlTransId                aTransID,
                              eldcHashElement         * aIntoHashElement,
                              eldcHashRelatedObjList ** aRelatedList,
                              eldcHashElement         * aObjectHashElement,
                              ellObjectType             aObjectType,
                              ellEnv                  * aEnv );

stlStatus eldcDelHashRelated( smlTransId                aTransID,
                              eldcHashElement         * aFromHashElement,
                              eldcHashRelatedObjList ** aRelatedList,
                              eldcHashElement         * aObjectHashElement,
                              ellEnv                  * aEnv );

/**
 * pre-rollback 함수
 */
stlStatus eldcPreRollbackSetFlag( smlTransId     aTransID,
                                  void         * aPendArgs,
                                  void         * aAgingEventBuffer,
                                  ellEnv       * aEnv );

/*
 * Pending Operation 함수 
 */

stlStatus eldcPendRollbackFreeDictMem( smlTransId     aTransID,
                                       void         * aPendArgs,
                                       void         * aAgingEventBuffer,
                                       ellEnv       * aEnv );

stlStatus eldcPendRollbackInsertHashElement( smlTransId     aTransNA,
                                             void         * aPendArgs,
                                             void         * aAgingEventBuffer,
                                             ellEnv       * aEnv );

stlStatus eldcPendCommitInsertHashElement( smlTransId     aTransID,
                                           void         * aPendArgs,
                                           void         * aAgingEventBuffer,
                                           ellEnv       * aEnv );

stlStatus eldcPendRollbackDeleteHashElement( smlTransId     aTransNA,
                                             void         * aPendArgs,
                                             void         * aAgingEventBuffer,
                                             ellEnv       * aEnv );

stlStatus eldcPendCommitDeleteHashElement( smlTransId     aTransID,
                                           void         * aPendArgs,
                                           void         * aAgingEventBuffer,
                                           ellEnv       * aEnv );


stlStatus eldcPendRollbackAddHashRelated( smlTransId     aTransNA,
                                          void         * aPendArgs,
                                          void         * aAgingEventBuffer,
                                          ellEnv       * aEnv );

stlStatus eldcPendCommitAddHashRelated( smlTransId     aTransID,
                                        void         * aPendArgs,
                                        void         * aAgingEventBuffer,
                                        ellEnv       * aEnv );

stlStatus eldcPendRollbackDelHashRelated( smlTransId     aTransNA,
                                          void         * aPendArgs,
                                          void         * aAgingEventBuffer,
                                          ellEnv       * aEnv );

stlStatus eldcPendCommitDelHashRelated( smlTransId     aTransID,
                                        void         * aPendArgs,
                                        void         * aAgingEventBuffer,
                                        ellEnv       * aEnv );

/*
 * local cache 함수
 */

void eldcSearchLocalCache( void            * aObject,
                           eldcLocalCache ** aLocalCache,
                           ellEnv          * aEnv );

void eldcInsertLocalCache( void            * aObject,
                           eldcLocalCache  * aLocalCache,
                           ellEnv          * aEnv );

stlStatus eldcDeleteLocalCache( eldcLocalCache * aLocalCache,
                                ellEnv         * aEnv );

stlStatus eldcFreeLocalCaches( ellEnv * aEnv );


/** @} eldcDictHash */


#endif /* _ELDC_DICT_HASH_H_ */
