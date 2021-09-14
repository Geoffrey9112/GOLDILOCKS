/*******************************************************************************
 * smlIndex.c
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

#include <dtl.h>
#include <sml.h>
#include <sme.h>
#include <smn.h>
#include <sma.h>
#include <smd.h>
#include <smxl.h>
#include <smf.h>
#include <sms.h>

/**
 * @file smlIndex.c
 * @brief Storage Manager Layer Index Routines
 */

/**
 * @addtogroup smlIndex
 * @{
 */

/**
 * @brief Index를 생성한다.
 * @param[in] aStatement Statement 객체
 * @param[in] aTbsId 생성할 Index가 속할 Tablespace ID
 * @param[in] aIndexType 생성할 Index의 타입
 * @param[in] aAttr 생성할 Index의 속성을 지정한 구조체
 * @param[in] aBaseTableHandle Index를 생성할 base Table의 handle
 * @param[in] aKeyColCount Index가 가지는 key column의 개수
 * @param[in] aIndexColList 각 key column들의 list. Fetch를 위한 Buffer도 있음
 * @param[in] aKeyColFlags Index가 가지는 key column들의 flag 배열
 * @param[out] aRelationId 생성된 Index의 물리적 아이디
 * @param[out] aRelationHandle 생성된 Index를 조작할때 사용할 메모리 핸들
 * @param[in,out] aEnv Environment 구조체
 */

stlStatus smlCreateIndex( smlStatement       * aStatement,
                          smlTbsId             aTbsId,
                          smlIndexType         aIndexType,
                          smlIndexAttr       * aAttr,
                          void               * aBaseTableHandle,
                          stlUInt16            aKeyColCount,
                          knlValueBlockList  * aIndexColList,
                          stlUInt8           * aKeyColFlags,
                          stlInt64           * aRelationId,
                          void              ** aRelationHandle,
                          smlEnv             * aEnv )
{
    smnIndexModule * sModule = smnGetIndexModule(aIndexType);

    STL_ASSERT( sModule != NULL );

    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    if( SML_INDEX_IS_INSTANT( aIndexType ) == STL_FALSE )
    {
        /**
         * Validate tablespace
         */

        STL_TRY( smlValidateTablespace( aTbsId,
                                        SME_VALIDATION_BEHAVIOR_UPDATE,
                                        aEnv )
                 == STL_SUCCESS );
    
        /**
         * Query Timeout 검사
         */
    
        STL_TRY( knlCheckQueryTimeout( KNL_ENV(aEnv) ) == STL_SUCCESS );
    
        STL_TRY( smxlInsertTransRecord( SMA_GET_TRANS_ID(aStatement),
                                        aEnv )
                 == STL_SUCCESS );
    }
    
    /**
     * Create index
     */
    
    STL_TRY( sModule->mCreate( aStatement,
                               aTbsId,
                               aAttr,
                               aBaseTableHandle,
                               aKeyColCount,
                               aIndexColList,
                               aKeyColFlags,
                               aRelationId,
                               aRelationHandle,
                               aEnv) == STL_SUCCESS );

    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief 인덱스를 삭제한다.
 * @param[in] aStatement Statement 객체
 * @param[in] aRelationHandle Index relation handle
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smlDropIndex( smlStatement * aStatement,
                        void         * aRelationHandle,
                        smlEnv       * aEnv )
{
    smnIndexModule * sModule;
    smlIndexType     sIndexType;
    smlRelationType  sRelType = SME_GET_RELATION_TYPE( aRelationHandle );

    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    sIndexType = SML_GET_INDEX_TYPE( SME_GET_RELATION_TYPE(aRelationHandle) );
    
    sModule = smnGetIndexModule( sIndexType );
    STL_ASSERT( sModule != NULL );

    if( SML_RELATION_IS_INSTANT_INDEX( sRelType ) == STL_FALSE )
    {
        /**
         * Query Timeout 검사
         */
        STL_TRY( knlCheckQueryTimeout( KNL_ENV(aEnv) ) == STL_SUCCESS );

        KNL_SYSTEM_FATAL_DASSERT( (SME_GET_RELATION_STATE( aRelationHandle ) ==
                                   SME_RELATION_STATE_ACTIVE),
                                  aEnv );

        STL_TRY( smxlInsertTransRecord( SMA_GET_TRANS_ID(aStatement),
                                        aEnv )
                 == STL_SUCCESS );
    }
    
    STL_TRY( sModule->mDrop( aStatement,
                             aRelationHandle,
                             aEnv) == STL_SUCCESS );

    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief 인덱스를 Truncate한다.
 * @param[in] aStatement Statement 객체
 * @param[in] aOrgIndexHandle Truncate 이전의 원본 Index Handle
 * @param[in] aNewBaseTableHandle Truncate을 위해 새로 생성된 Base Table의 Relation Handle
 * @param[in] aDropStorageOption extent의 drop 방법 지정
 * @param[out] aNewIndexHandle 생성된 인덱스의 Relation Handle
 * @param[out] aNewIndexRelationId 생성된 인덱스의 Relation의 Identifier
 * @param[in,out] aEnv Environment 구조체
 * @note Instant Hash Index는 In-place truncate을 수행한다.
 * 또한 Instant Table Truncation없이 Index만 Truncate되는 경우는 Page Leak이 발생할수 있다.
 */
stlStatus smlTruncateIndex( smlStatement          * aStatement,
                            void                  * aOrgIndexHandle,
                            void                  * aNewBaseTableHandle,
                            smlDropStorageOption    aDropStorageOption,
                            void                 ** aNewIndexHandle,
                            stlInt64              * aNewIndexRelationId,
                            smlEnv                * aEnv )
{
    smnIndexModule * sModule;
    smlIndexType     sIndexType;
    smlRelationType  sRelType = SME_GET_RELATION_TYPE( aOrgIndexHandle );

    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    sIndexType = SML_GET_INDEX_TYPE( SME_GET_RELATION_TYPE(aOrgIndexHandle) );
    
    sModule = smnGetIndexModule( sIndexType );
    STL_ASSERT( sModule != NULL );

    if( SML_RELATION_IS_INSTANT_INDEX( sRelType ) == STL_FALSE )
    {
        /**
         * Query Timeout 검사
         */
        STL_TRY( knlCheckQueryTimeout( KNL_ENV(aEnv) ) == STL_SUCCESS );
    
        /**
         * Validate relation
         */

        STL_TRY( smlValidateRelation( aOrgIndexHandle,
                                      SME_VALIDATION_BEHAVIOR_UPDATE,
                                      aEnv )
                 == STL_SUCCESS );

        STL_TRY( smxlInsertTransRecord( SMA_GET_TRANS_ID(aStatement),
                                        aEnv )
                 == STL_SUCCESS );
    
        STL_TRY( sModule->mCreateForTruncate( aStatement,
                                              aOrgIndexHandle,
                                              aNewBaseTableHandle,
                                              aDropStorageOption,
                                              aNewIndexHandle,
                                              aNewIndexRelationId,
                                              aEnv )
                 == STL_SUCCESS );

        STL_TRY( sModule->mBuildForTruncate( aStatement,
                                             aOrgIndexHandle,
                                             (*aNewIndexHandle),
                                             aEnv) == STL_SUCCESS );

        STL_TRY( sModule->mDrop( aStatement,
                                 aOrgIndexHandle,
                                 aEnv) == STL_SUCCESS );
    }
    else
    {
        STL_TRY( sModule->mTruncate( aStatement,
                                     aOrgIndexHandle,
                                     aEnv ) == STL_SUCCESS );

        *aNewIndexHandle = aOrgIndexHandle;
    }

    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}


/**
 * @brief Index를 Build한다.
 * @param[in] aStatement Statement 객체
 * @param[in] aIndexHandle enable할 Index의 handle
 * @param[in] aBaseTableHandle Index의 base Table의 handle
 * @param[in] aKeyColCount Index가 가지는 key column의 개수
 * @param[in] aIndexColList 각 key column들의 list. Fetch를 위한 Buffer와 type, base table order도 있음
 * @param[in] aKeyColFlags Index가 가지는 key column들의 flag 배열
 * @param[in] aParallelFactor 작업에 참여하는 thread 수
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smlBuildIndex( smlStatement      * aStatement,
                         void              * aIndexHandle,
                         void              * aBaseTableHandle,
                         stlUInt16           aKeyColCount,
                         knlValueBlockList * aIndexColList,
                         stlUInt8          * aKeyColFlags,
                         stlUInt16           aParallelFactor,
                         smlEnv            * aEnv )
{
    smlRelationType  sRelType = SME_GET_RELATION_TYPE( aIndexHandle );
    smlIndexType     sIndexType = SML_GET_INDEX_TYPE( sRelType );
    smnIndexModule * sModule = smnGetIndexModule(sIndexType);
    smlTbsId         sTbsId;

    STL_ASSERT( sModule != NULL );

    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    /**
     * Validate tablespace
     */

    if( SML_RELATION_IS_INSTANT_INDEX( sRelType ) == STL_FALSE )
    {
        /**
         * Query Timeout 검사
         */
        STL_TRY( knlCheckQueryTimeout( KNL_ENV(aEnv) ) == STL_SUCCESS );
    
        /**
         * Validate tablespace
         */

        sTbsId = smsGetTbsId( SME_GET_SEGMENT_HANDLE( aIndexHandle ) );
        STL_TRY( smlValidateTablespace( sTbsId,
                                        SME_VALIDATION_BEHAVIOR_UPDATE,
                                        aEnv )
                 == STL_SUCCESS );
        
        /**
         * Validate relation
         */

        KNL_SYSTEM_FATAL_DASSERT( (SME_GET_RELATION_STATE( aIndexHandle ) ==
                                   SME_RELATION_STATE_ACTIVE),
                                  aEnv );
                                  
        STL_TRY( smxlInsertTransRecord( SMA_GET_TRANS_ID(aStatement),
                                        aEnv )
                 == STL_SUCCESS );
    }

    STL_TRY( sModule->mBuild( aStatement,
                              aIndexHandle,
                              aBaseTableHandle,
                              aKeyColCount,
                              aIndexColList,
                              aKeyColFlags,
                              aParallelFactor,
                              aEnv) == STL_SUCCESS );

    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief Index를 Unusable에서 usable상태로 build한다.
 * @param[in] aStatement Statement 객체
 * @param[in] aIndexHandle enable할 Index의 handle
 * @param[in,out] aEnv Environment 구조체
 * @remarks 이미 Usable 상태의 인덱스는 무시한다.
 */
stlStatus smlBuildUnusableIndex( smlStatement      * aStatement,
                                 void              * aIndexHandle,
                                 smlEnv            * aEnv )
{
    smlIndexType     sIndexType = SML_GET_INDEX_TYPE(((smnIndexHeader*)aIndexHandle)->mRelHeader.mRelationType);
    smnIndexModule * sModule = smnGetIndexModule(sIndexType);
    smlTbsId         sTbsId;

    STL_ASSERT( sModule != NULL );

    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    /**
     * Validate tablespace
     */

    sTbsId = smsGetTbsId( SME_GET_SEGMENT_HANDLE( aIndexHandle ) );
    STL_TRY( smlValidateTablespace( sTbsId,
                                    SME_VALIDATION_BEHAVIOR_UPDATE,
                                    aEnv )
             == STL_SUCCESS );

    /**
     * 이미 Usable Index는 무시한다.
     */
    STL_TRY_THROW( smsIsUsableSegment( SME_GET_SEGMENT_HANDLE(aIndexHandle) )
                   == STL_FALSE, RAMP_SUCCESS );

    STL_TRY( smxlInsertTransRecord( SMA_GET_TRANS_ID(aStatement),
                                    aEnv )
             == STL_SUCCESS );

    STL_TRY( sModule->mBuildUnusable( aStatement,
                                      aIndexHandle,
                                      aEnv)
             == STL_SUCCESS );

    STL_RAMP( RAMP_SUCCESS );

    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;

}

/**
 * @brief 인덱스 컬럼의 타입을 변경한다.
 * @param[in] aStatement Statement 객체
 * @param[in] aIndexHandle 변경할 인덱스의 Relation Handle
 * @param[in] aColumnOrder Target Column Order
 * @param[in] aDataType Target Column Type 
 * @param[in] aColumnSize Target Column Size
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smlAlterDataType( smlStatement * aStatement,
                            void         * aIndexHandle,
                            stlInt32       aColumnOrder,
                            dtlDataType    aDataType,
                            stlInt64       aColumnSize,
                            smlEnv       * aEnv )
{
    smlIndexType     sIndexType = SML_GET_INDEX_TYPE(((smnIndexHeader*)aIndexHandle)->mRelHeader.mRelationType);
    smnIndexModule * sModule = smnGetIndexModule(sIndexType);

    STL_ASSERT( sModule != NULL );

    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    /**
     * Validate relation
     */

    STL_TRY( smlValidateRelation( aIndexHandle,
                                  SME_VALIDATION_BEHAVIOR_UPDATE,
                                  aEnv )
             == STL_SUCCESS );

    STL_TRY( smxlInsertTransRecord( SMA_GET_TRANS_ID(aStatement),
                                    aEnv )
             == STL_SUCCESS );

    STL_TRY( sModule->mAlterDataType( aStatement,
                                      aIndexHandle,
                                      aColumnOrder,
                                      aDataType,
                                      aColumnSize,
                                      aEnv)
             == STL_SUCCESS );

    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;

}

/**
 * @brief 기존 인덱스의 물리적 속성을 변경한다.
 * @param[in] aStatement Statement 객체
 * @param[in] aRelationHandle 검사할 인덱스의 Relation Handle
 * @param[in] aIndexAttr 변경할 물리적 속성
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smlAlterIndexAttr( smlStatement * aStatement,
                             void         * aRelationHandle,
                             smlIndexAttr * aIndexAttr,
                             smlEnv       * aEnv )
{
    smlIndexType     sIndexType;
    smnIndexModule * sModule;

    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    sIndexType = SML_GET_INDEX_TYPE(((smnIndexHeader*)aRelationHandle)->mRelHeader.mRelationType);
    sModule = smnGetIndexModule( sIndexType );

    STL_DASSERT( sModule != NULL );

    /**
     * Query Timeout 검사
     */
    STL_TRY( knlCheckQueryTimeout( KNL_ENV(aEnv) ) == STL_SUCCESS );
    
    /**
     * Validate relation
     */

    STL_TRY( smlValidateRelation( aRelationHandle,
                                  SME_VALIDATION_BEHAVIOR_UPDATE,
                                  aEnv )
             == STL_SUCCESS );

    STL_TRY( smxlInsertTransRecord( SMA_GET_TRANS_ID(aStatement),
                                    aEnv )
             == STL_SUCCESS );
    
    STL_TRY( sModule->mAlterIndexAttr( aStatement,
                                       aRelationHandle,
                                       aIndexAttr,
                                       aEnv )
             == STL_SUCCESS );
    
    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/** @} */
