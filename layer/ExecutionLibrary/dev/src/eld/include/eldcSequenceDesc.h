/*******************************************************************************
 * eldcSequenceDesc.h
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


#ifndef _ELDC_SEQUENCE_DESC_H_
#define _ELDC_SEQUENCE_DESC_H_ 1

/**
 * @file eldcSequenceDesc.h
 * @brief Cache for Sequence descriptor
 */


/**
 * @defgroup eldcSequenceDesc Cache for Sequence descriptor
 * @ingroup eldcObject
 * @{
 */

/**
 * @brief Sequence 의 Name Hash Data
 */
typedef struct eldcHashDataSequenceName
{
    stlInt64          mKeySchemaID;      /**< Name Hash 의 Key : Schema ID */
    stlChar         * mKeySequenceName;  /**< Name Hash 의 Key : Sequence Name */
    stlInt64          mDataSequenceID;   /**< Name Hash 의 Data: Sequence ID */
    eldcHashElement * mDataHashElement;  /**< Name Hash 의 Data: Seq ID 의 Hash Element */
} eldcHashDataSequenceName;

/**
 * @brief Sequence 의 ID Hash Data
 */
typedef struct eldcHashDataSequenceID
{
    stlInt64                 mKeySequenceID;    /**< ID Hash 의 Key : Sequence ID */
} eldcHashDataSequenceID;

/**
 * @brief Sequence Name 검색을 위한 Hash Key
 */
typedef struct eldcHashKeySequenceName
{
    stlInt64     mSchemaID;      /**< Schema ID */
    stlChar    * mSequenceName;  /**< Sequence Name */
} eldcHashKeySequenceName;


/*
 * Hash 함수 
 */

stlUInt32  eldcHashFuncSequenceID( void * aHashKey );
stlBool    eldcCompareFuncSequenceID( void * aHashKey, eldcHashElement * aHashElement );

stlUInt32  eldcHashFuncSequenceName( void * aHashKey );
stlBool    eldcCompareFuncSequenceName( void * aHashKey, eldcHashElement * aHashElement );


/*
 * Buffer Size 계산
 */

stlInt32  eldcCalSizeSequenceDesc( stlInt32 aSeqNameLen );
stlChar * eldcGetSequenceNamePtr( ellSequenceDesc * aSeqDesc );

stlInt32  eldcCalSizeHashDataSequenceID();

stlInt32  eldcCalSizeHashDataSequenceName( stlInt32 aSeqNameLen );
stlChar * eldcGetHashDataSequenceNamePtr( eldcHashDataSequenceName * aHashDataSeqName );


/*
 * Dump Information
 */

void eldcPrintSequenceDesc( void * aSeqDesc, stlChar * aStringBuffer );
void eldcPrintHashDataSequenceID( void * aHashDataSeqID, stlChar * aStringBuffer );
void eldcPrintHashDataSequenceName( void * aHashDataSeqName, stlChar * aStringBuffer );


/*
 * 초기화, 마무리 함수
 */

void eldcSetSQLSequenceDumpHandle();

stlStatus eldcBuildSequenceCache( smlTransId     aTransID,
                                  smlStatement * aStmt,
                                  ellEnv       * aEnv );

stlStatus eldcBuildCacheSQLSequence( smlTransId       aTransID,
                                     smlStatement   * aStmt,
                                     ellEnv         * aEnv );

/*
 * 조회 함수 
 */

stlStatus eldcGetSequenceHandleByID( smlTransId           aTransID,
                                     smlScn               aViewSCN,
                                     stlInt64             aSequenceID,
                                     ellDictHandle      * aSequenceDictHandle,
                                     stlBool            * aExist,
                                     ellEnv             * aEnv );

stlStatus eldcGetSequenceHandleByName( smlTransId           aTransID,
                                       smlScn               aViewSCN,
                                       stlInt64             aSchemaID,
                                       stlChar            * aSequenceName,
                                       ellDictHandle      * aSequenceDictHandle,
                                       stlBool            * aExist,
                                       ellEnv             * aEnv );

/*
 * 제어 함수
 */

stlStatus eldcInsertCacheSequence( smlTransId          aTransID,
                                   smlStatement      * aStmt,
                                   knlValueBlockList * aValueArray,
                                   ellEnv            * aEnv );

stlStatus eldcInsertCacheSequenceBySequenceID( smlTransId          aTransID,
                                               smlStatement      * aStmt,
                                               stlInt64            aSeqID,
                                               ellEnv            * aEnv );

stlStatus eldcDeleteCacheSequence( smlTransId          aTransID,
                                   smlStatement      * aStmt,
                                   ellDictHandle     * aSeqHandle,
                                   ellEnv            * aEnv );

/*
 * 멤버 함수 
 */

stlStatus eldcMakeSequenceDesc( smlTransId          aTransID,
                                smlStatement      * aStmt,
                                knlValueBlockList * aValueArray,
                                ellSequenceDesc  ** aSeqDesc,
                                ellEnv            * aEnv );

stlStatus eldcMakeHashElementSequenceID( eldcHashElement   ** aHashElement,
                                         smlTransId           aTransID,
                                         ellSequenceDesc    * aSeqDesc,
                                         ellEnv             * aEnv );


stlStatus eldcMakeHashElementSequenceName( eldcHashElement   ** aHashElement,
                                           smlTransId           aTransID,
                                           ellSequenceDesc    * aSeqDesc,
                                           eldcHashElement    * aHashElementID,
                                           ellEnv             * aEnv );

/** @} eldcSequenceDesc */


#endif /* _ELDC_SEQUENCE_DESC_H_ */
