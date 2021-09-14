/*******************************************************************************
 * smlTable.c
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
#include <smDef.h>
#include <sme.h>
#include <smd.h>
#include <smf.h>
#include <sms.h>
#include <sma.h>
#include <smn.h>
#include <smxl.h>
#include <smdDef.h>
#include <smdmifTable.h>
#include <smdmih.h>

/**
 * @file smlTable.c
 * @brief Storage Manager Layer Table Routines
 */

/**
 * @addtogroup smlTable
 * @{
 */

/**
 * @brief 테이블을 생성한다.
 * @param[in] aStatement Statement 객체
 * @param[in] aTbsId 생성할 테이블이 속할 Tablespace ID
 * @param[in] aRelationType 생성할 릴레이션의 타입
 * @param[in] aAttr 생성할 테이블 속성을 지정한 구조체
 * @param[out] aRelationId 생성된 테이블의 물리적 아이디
 * @param[out] aRelationHandle 생성된 테이블을 조작할때 사용할 메모리 핸들
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smlCreateTable( smlStatement     * aStatement,
                          smlTbsId           aTbsId,
                          smlRelationType    aRelationType,
                          smlTableAttr     * aAttr,
                          stlInt64         * aRelationId,
                          void            ** aRelationHandle,
                          smlEnv           * aEnv )
{
    smdTableModule * sModule = smdGetTableModule(SML_GET_TABLE_TYPE(aRelationType));

    STL_ASSERT( sModule != NULL );

    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    if( SML_RELATION_IS_INSTANT_TABLE( aRelationType ) == STL_FALSE )
    {
        /**
         * Query Timeout 검사
         */
    
        STL_TRY( knlCheckQueryTimeout( KNL_ENV(aEnv) ) == STL_SUCCESS );
 
        /**
         * Validate tablespace
         */

        STL_TRY( smlValidateTablespace( aTbsId,
                                        SME_VALIDATION_BEHAVIOR_UPDATE,
                                        aEnv )
                 == STL_SUCCESS );

        STL_TRY( smxlInsertTransRecord( SMA_GET_TRANS_ID(aStatement),
                                        aEnv )
                 == STL_SUCCESS );
    }
    
    STL_TRY( sModule->mCreate( aStatement,
                               aTbsId,
                               aAttr,
                               STL_TRUE,  /* aUndoLogging */
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
 * @brief IO(Index Organized) 테이블을 생성한다.
 * @param[in] aStatement Statement 객체
 * @param[in] aTbsId 생성할 테이블이 속할 Tablespace ID
 * @param[in] aRelationType 생성할 릴레이션의 타입
 * @param[in] aTableAttr 생성할 테이블 속성을 지정한 구조체
 * @param[in] aIndexAttr 생성할 인덱스 속성을 지정한 구조체
 * @param[in] aColList 각 column들의 list. Fetch를 위한 Buffer도 있음
 * @param[in] aKeyColCount key column의 개수
 * @param[in] aKeyColFlags key column들의 flag 배열
 * @param[out] aRelationId 생성된 테이블의 물리적 아이디
 * @param[out] aRelationHandle 생성된 테이블을 조작할때 사용할 메모리 핸들
 * @param[in,out] aEnv Environment 구조체
 *
 * IOT는 아래의 경우에서 사용한다.
 * 
 * 1. group by, distinct
 *  - uniqueness 옵션이 필요함.
 * 2. semi join, anti semi join
 *  - uniqueness 옵션이 필요함(성능 개선이슈)
 * 3. set operator
 *  - delete 연산을 지원해야 함.
 *
 * Hash Instant Index는 아래의 경우에서 사용된다.
 * 
 * 1. inner/outer join
 *  - join function을 지원하지 않음(향후 IOT에도 지원 가능)
 * 2. semi join, anti semi join
 *  - uniqueness를 사용할수 없는 경우(Record column의 filter가 존재)
 */
stlStatus smlCreateIoTable( smlStatement      * aStatement,
                            smlTbsId            aTbsId,
                            smlRelationType     aRelationType,
                            smlTableAttr      * aTableAttr,
                            smlIndexAttr      * aIndexAttr,
                            knlValueBlockList * aColList,
                            stlUInt16           aKeyColCount,
                            stlUInt8          * aKeyColFlags,
                            stlInt64          * aRelationId,
                            void             ** aRelationHandle,
                            smlEnv            * aEnv )
{
    smdTableModule * sModule = smdGetTableModule(SML_GET_TABLE_TYPE(aRelationType));

    STL_ASSERT( sModule != NULL );

    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    if( SML_RELATION_IS_INSTANT_TABLE( aRelationType ) == STL_FALSE )
    {
        /**
         * Query Timeout 검사
         */
    
        STL_TRY( knlCheckQueryTimeout( KNL_ENV(aEnv) ) == STL_SUCCESS );
 
        /**
         * Validate tablespace
         */

        STL_TRY( smlValidateTablespace( aTbsId,
                                        SME_VALIDATION_BEHAVIOR_UPDATE,
                                        aEnv )
                 == STL_SUCCESS );

        STL_TRY( smxlInsertTransRecord( SMA_GET_TRANS_ID(aStatement),
                                        aEnv )
                 == STL_SUCCESS );
    }
    
    STL_TRY( sModule->mCreateIot( aStatement,
                                  aTbsId,
                                  aTableAttr,
                                  aIndexAttr,
                                  aColList,
                                  aKeyColCount,
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
 * @brief Sort instant table을 생성한다.
 * @param[in] aStatement Statement 객체
 * @param[in] aTbsId 생성할 Sort instant table이 속할 Tablespace ID
 * @param[in] aAttr 생성할 Sort instant table의 종류 및 속성
 * @param[in] aBaseTableHandle Secondary sort instant table일 경우 base table의 핸들
 * @param[in] aKeyColCount key column의 개수
 * @param[in] aKeyColList 각 key column들의 list. Fetch를 위한 Buffer도 있음
 * @param[in] aKeyColFlags key column들의 flag 배열
 * @param[out] aRelationId 생성된 sort table의 물리적 아이디
 * @param[out] aRelationHandle 생성된 sort table을 조작할때 사용할 메모리 핸들
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smlCreateSortTable( smlStatement       * aStatement,
                              smlTbsId             aTbsId,
                              smlSortTableAttr   * aAttr,
                              void               * aBaseTableHandle,
                              stlUInt16            aKeyColCount,
                              knlValueBlockList  * aKeyColList,
                              stlUInt8           * aKeyColFlags,
                              stlInt64           * aRelationId,
                              void              ** aRelationHandle,
                              smlEnv             * aEnv )
{
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    STL_TRY( knlCheckQueryTimeout( KNL_ENV(aEnv) ) == STL_SUCCESS );

    STL_TRY( smdmisTableCreate( aStatement,
                                aTbsId,
                                aAttr,
                                aBaseTableHandle,
                                aKeyColCount,
                                aKeyColList,
                                aKeyColFlags,
                                aRelationId,
                                aRelationHandle,
                                aEnv ) == STL_SUCCESS );

    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief 테이블을 삭제한다.
 * @param[in] aStatement Statement 객체
 * @param[in] aRelationHandle 삭제할 테이블의 Relation Handle
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smlDropTable( smlStatement * aStatement,
                        void         * aRelationHandle,
                        smlEnv       * aEnv )
{
    smdTableModule * sModule;
    stlUInt16        sRelationType;

    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    sRelationType = SME_GET_RELATION_TYPE( aRelationHandle );
    sModule = smdGetTableModule(SML_GET_TABLE_TYPE(sRelationType));

    STL_ASSERT( sModule != NULL );

    if( SML_RELATION_IS_INSTANT_TABLE( sRelationType ) == STL_FALSE )
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
 * @brief 테이블에 새로운 컬럼을 추가한다.
 * @param[in] aStatement Statement 객체
 * @param[in] aRelationHandle 변경할 테이블의 Relation Handle
 * @param[in] aAddColumns 추가할 Column List
 * @param[in] aRowBlock Row Block
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smlAlterTableAddColumns( smlStatement      * aStatement,
                                   void              * aRelationHandle,
                                   knlValueBlockList * aAddColumns,
                                   smlRowBlock       * aRowBlock,
                                   smlEnv            * aEnv )
{
    smdTableModule * sModule;
    stlUInt16        sRelationType;

    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    sRelationType = SME_GET_RELATION_TYPE( aRelationHandle );
    sModule = smdGetTableModule( SML_GET_TABLE_TYPE(sRelationType) );

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
    
    STL_TRY( sModule->mAddColumns( aStatement,
                                   aRelationHandle,
                                   aAddColumns,
                                   aRowBlock,
                                   aEnv )
             == STL_SUCCESS );
    
    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief 테이블에서 기존 컬럼을 삭제한다.
 * @param[in] aStatement Statement 객체
 * @param[in] aRelationHandle 변경할 테이블의 Relation Handle
 * @param[in] aDropColumns 삭제할 Column List
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smlAlterTableDropColumns( smlStatement      * aStatement,
                                    void              * aRelationHandle,
                                    knlValueBlockList * aDropColumns,
                                    smlEnv            * aEnv )
{
    smdTableModule * sModule;
    stlUInt16        sRelationType;

    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    sRelationType = SME_GET_RELATION_TYPE( aRelationHandle );
    sModule = smdGetTableModule( SML_GET_TABLE_TYPE(sRelationType) );

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
    
    STL_TRY( sModule->mDropColumns( aStatement,
                                    aRelationHandle,
                                    aDropColumns,
                                    aEnv )
             == STL_SUCCESS );
    
    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief 기존 테이블의 물리적 속성을 변경한다.
 * @param[in] aStatement Statement 객체
 * @param[in] aRelationHandle 검사할 테이블의 Relation Handle
 * @param[in] aTableAttr 변경할 물리적 속성
 * @param[in] aIndexAttr 변경할 물리적 속성 ( nullable )
 * @param[in,out] aEnv Environment 구조체
 * @remarks aIndexAttr은 Index Organized Table에만 해당 된다.
 */
stlStatus smlAlterTableAttr( smlStatement * aStatement,
                             void         * aRelationHandle,
                             smlTableAttr * aTableAttr,
                             smlIndexAttr * aIndexAttr,
                             smlEnv       * aEnv )
{
    smdTableModule * sModule;
    stlUInt16        sRelationType;

    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    sRelationType = SME_GET_RELATION_TYPE( aRelationHandle );
    sModule = smdGetTableModule( SML_GET_TABLE_TYPE(sRelationType) );

    STL_DASSERT( sModule != NULL );

    if( SML_RELATION_IS_INSTANT_TABLE( sRelationType ) == STL_FALSE )
    {
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
    }
    
    STL_TRY( sModule->mAlterTableAttr( aStatement,
                                       aRelationHandle,
                                       aTableAttr,
                                       aIndexAttr,
                                       aEnv )
             == STL_SUCCESS );
    
    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief 기존 테이블 레코드를 새로운 세그먼트이 재할당한다.
 * @param[in] aStatement Statement 객체
 * @param[in] aOrgRelHandle Truncate 이전의 원본 Relation Handle
 * @param[in] aTbsId 새로운 세그먼트가 저장될 Tablespace Identifier
 * @param[in] aTableAttr 변경할 물리적 속성
 * @param[in] aColumnList 이동될 레코드를 저장할 컬럼들의 공간
 * @param[out] aNewRelHandle 생성된 Relation Handle
 * @param[out] aNewRelationId 생성된 Relation의 Identifier
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smlAlterTableMove( smlStatement       * aStatement,
                             void               * aOrgRelHandle,
                             smlTbsId             aTbsId,
                             smlTableAttr       * aTableAttr,
                             knlValueBlockList  * aColumnList,
                             void              ** aNewRelHandle,
                             stlInt64           * aNewRelationId,
                             smlEnv             * aEnv )
{
    smdTableModule * sModule;
    stlUInt16        sRelationType;
    smlTableAttr     sTableAttr;
    
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    sTableAttr = *aTableAttr;
    sRelationType = SME_GET_RELATION_TYPE( aOrgRelHandle );
    sModule = smdGetTableModule( SML_GET_TABLE_TYPE(sRelationType) );

    STL_DASSERT( sModule != NULL );

    /**
     * Query Timeout 검사
     */
    STL_TRY( knlCheckQueryTimeout( KNL_ENV(aEnv) ) == STL_SUCCESS );
    
    /**
     * Validate relation
     */

    STL_TRY( smlValidateRelation( aOrgRelHandle,
                                  SME_VALIDATION_BEHAVIOR_UPDATE,
                                  aEnv )
             == STL_SUCCESS );

    STL_TRY( smxlInsertTransRecord( SMA_GET_TRANS_ID(aStatement),
                                    aEnv )
             == STL_SUCCESS );
    
    STL_TRY( sModule->mMergeTableAttr( aOrgRelHandle,
                                       &sTableAttr,
                                       aEnv )
             == STL_SUCCESS );

    STL_TRY( sModule->mCreate( aStatement,
                               aTbsId,
                               &sTableAttr,
                               STL_TRUE,  /* aUndoLogging */
                               aNewRelationId,
                               aNewRelHandle,
                               aEnv )
             == STL_SUCCESS );
    
    STL_TRY( sModule->mCopy( aStatement,
                             aOrgRelHandle,
                             *aNewRelHandle,
                             aColumnList,
                             aEnv )
             == STL_SUCCESS );

    STL_TRY( sModule->mDrop( aStatement,
                             aOrgRelHandle,
                             aEnv )
             == STL_SUCCESS );
    
    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief 테이블에서 Row가 존재하는 검사한다.
 * @param[in] aStatement Statement 객체
 * @param[in] aRelationHandle 검사할 테이블의 Relation Handle
 * @param[out] aExistRow Row의 존재 여부
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smlCheckTableExistRow( smlStatement * aStatement,
                                 void         * aRelationHandle,
                                 stlBool      * aExistRow,
                                 smlEnv       * aEnv )
{
    smdTableModule * sModule;
    stlUInt16        sRelationType;

    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    sRelationType = SME_GET_RELATION_TYPE( aRelationHandle );
    sModule = smdGetTableModule( SML_GET_TABLE_TYPE(sRelationType) );

    STL_DASSERT( sModule != NULL );

    /**
     * Query Timeout 검사
     */
    STL_TRY( knlCheckQueryTimeout( KNL_ENV(aEnv) ) == STL_SUCCESS );
    
    STL_TRY( sModule->mCheckExistRow( aStatement,
                                      aRelationHandle,
                                      aExistRow,
                                      aEnv )
             == STL_SUCCESS );
    
    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief 테이블을 Truncate한다.
 * @param[in] aStatement Statement 객체
 * @param[in] aOrgRelHandle Truncate 이전의 원본 Relation Handle
 * @param[in] aDropStorageOption extent의 drop 방법 지정
 * @param[out] aNewRelHandle 생성된 Relation Handle
 * @param[out] aNewRelationId 생성된 Relation의 Identifier
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smlTruncateTable( smlStatement          * aStatement,
                            void                  * aOrgRelHandle,
                            smlDropStorageOption    aDropStorageOption,
                            void                 ** aNewRelHandle,
                            stlInt64              * aNewRelationId,
                            smlEnv                * aEnv )
{
    smdTableModule * sModule;
    stlUInt16        sRelationType;

    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    sRelationType = SME_GET_RELATION_TYPE( aOrgRelHandle );
    sModule = smdGetTableModule(SML_GET_TABLE_TYPE(sRelationType));

    STL_ASSERT( sModule != NULL );

    if( SML_RELATION_IS_INSTANT_TABLE( sRelationType ) == STL_FALSE )
    {
        /**
         * Query Timeout 검사
         */
        STL_TRY( knlCheckQueryTimeout( KNL_ENV(aEnv) ) == STL_SUCCESS );
    
        /**
         * Validate relation
         */

        STL_TRY( smlValidateRelation( aOrgRelHandle,
                                      SME_VALIDATION_BEHAVIOR_UPDATE,
                                      aEnv )
                 == STL_SUCCESS );

        STL_TRY( smxlInsertTransRecord( SMA_GET_TRANS_ID(aStatement),
                                        aEnv )
                 == STL_SUCCESS );

        STL_TRY( sModule->mCreateForTruncate( aStatement,
                                              aOrgRelHandle,
                                              aDropStorageOption,
                                              aNewRelHandle,
                                              aNewRelationId,
                                              aEnv )
                 == STL_SUCCESS );

        STL_TRY( sModule->mDrop( aStatement,
                                 aOrgRelHandle,
                                 aEnv) == STL_SUCCESS );
    }
    else
    {
        STL_TRY( sModule->mTruncate( aStatement,
                                     aOrgRelHandle,
                                     aEnv ) == STL_SUCCESS );

        *aNewRelHandle = aOrgRelHandle;
    }

    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief Undo Segment의 크기를 줄인다.
 * @param[in,out] aEnv  Environment 구조체
 */
stlStatus smlShrinkUndoSegments( smlEnv * aEnv )
{
    smxlTransId  sTransId;
    stlUInt64    sTransSeq;
    stlInt32     sState = 0;
    
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );

    /**
     * Open phase에서만 동작한다.
     */
    STL_TRY_THROW( knlGetCurrStartupPhase() == KNL_STARTUP_PHASE_OPEN, RAMP_FINISH );

    STL_TRY( smxlAllocTrans( SML_TIL_READ_COMMITTED,
                             STL_FALSE, /* aIsGlobalTrans */
                             &sTransId,
                             &sTransSeq,
                             aEnv )
             == STL_SUCCESS );
    sState = 1;

    STL_TRY( smxlShrinkTo( sTransId,
                           aEnv )
             == STL_SUCCESS );

    sState = 0;
    STL_TRY( smxlCommit( sTransId,
                         NULL, /* aComment */
                         SML_TRANSACTION_CWM_WAIT,
                         aEnv ) == STL_SUCCESS );
    STL_TRY( smxlFreeTrans( sTransId,
                            SML_PEND_ACTION_COMMIT,
                            STL_FALSE, /* aCleanup */
                            SML_TRANSACTION_CWM_WAIT,
                            aEnv ) == STL_SUCCESS );

    STL_RAMP( RAMP_FINISH );
        
    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    if( sState == 1 )
    {
        (void)smxlRollback( sTransId,
                            SML_TRANSACTION_CWM_NO_WAIT,
                            STL_FALSE, /* aCleanup */
                            aEnv );
        (void)smxlFreeTrans( sTransId,
                             SML_PEND_ACTION_ROLLBACK,
                             STL_FALSE, /* aCleanup */
                             SML_TRANSACTION_CWM_NO_WAIT,
                             aEnv );
    }
    
    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );

    STL_TRY( knlLogMsg( NULL,
                        KNL_ENV( aEnv ),
                        KNL_LOG_LEVEL_WARNING,
                        "[AGER] failed to attempt to shrink undo segments\n" )
             == STL_SUCCESS );

    /**
     * 실패해도 실패를 반환하지 않는다.
     * - 해당 함수는 ager thread에서 호출함.
     */
    
    STL_INIT_ERROR_STACK( KNL_ERROR_STACK(aEnv) );

    return STL_SUCCESS;
}

/**
 * @brief 테이블을 생성한다.
 * @param[in] aStatement Statement 객체
 * @param[in] aRelationHandle 생성된 테이블을 조작할때 사용할 메모리 핸들
 * @param[in] aRids RID들이 저장된 value block
 * @param[in] aFlags flag를 설정할 row를 표시한 value block
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smlSetFlag( smlStatement      * aStatement,
                      void             ** aRelationHandle,
                      smlRowBlock       * aRids,
                      knlValueBlockList * aFlags,
                      smlEnv            * aEnv )
{
    smdTableModule * sModule;
    stlUInt16        sRelationType;

    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );

    sRelationType = SME_GET_RELATION_TYPE( aRelationHandle );
    sModule = smdGetTableModule( SML_GET_TABLE_TYPE(sRelationType) );

    STL_ASSERT( sModule != NULL );

    if( SML_RELATION_IS_INSTANT_TABLE( sRelationType ) == STL_TRUE )
    {
        STL_TRY( sModule->mSetFlag( aStatement,
                                    aRelationHandle,
                                    aRids,
                                    aFlags,
                                    aEnv) == STL_SUCCESS );
    }

    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief 주어진 Relation에 저장된 총 레코드 개수를 얻는다.
 * @param[in]     aRelationHandle   Relation Handle
 * @param[out]    aRowCount         저장된 총 레코드 개수
 * @param[in,out] aEnv              Environment 구조체
 */
stlStatus smlGetTotalRecordCount( void      * aRelationHandle,
                                  stlInt64  * aRowCount,
                                  smlEnv    * aEnv )
{
    smdTableModule * sModule;
    stlUInt16        sRelationType;

    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    sRelationType = SME_GET_RELATION_TYPE( aRelationHandle );
    sModule = smdGetTableModule( SML_GET_TABLE_TYPE(sRelationType) );

    STL_DASSERT( sModule != NULL );

    STL_TRY( sModule->mRowCount( aRelationHandle,
                                 aRowCount,
                                 aEnv )
             == STL_SUCCESS );
    
    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief 주어진 Relation Identifier로 부터 Relation Handle을 얻는다.
 * @param[in] aRelationId Relation Identifier
 * @param[out] aRelationHandle 설정될 Relation Handle
 * @param[in,out] aEnv Environment 구조체
 * @note Restrict Server 단계 이상에서만 사용가능하다.
 */
stlStatus smlGetRelationHandle( stlInt64    aRelationId,
                                void     ** aRelationHandle,
                                smlEnv    * aEnv )
{
    smlRid aRelationRid;

    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    SMG_WRITE_RID( &aRelationRid, &aRelationId );
    
    STL_TRY( smeGetRelationHandle( aRelationRid,
                                   aRelationHandle,
                                   aEnv ) == STL_SUCCESS );

    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief 주어진 Relation의 유효성을 검사한다.
 * @param[in] aRelationHandle 검사할 Relation Handle
 * @param[in] aValidateBehavior Validation Behavior
 * @param[in,out] aEnv Environment 구조체
 */

stlStatus smlValidateRelation( void     * aRelationHandle,
                               stlInt32   aValidateBehavior,
                               smlEnv   * aEnv )
{
    smlTbsId   sTbsId;
    void     * sSegmentHandle;
    
    sSegmentHandle = SME_GET_SEGMENT_HANDLE( aRelationHandle );

    if( sSegmentHandle != NULL )
    {
        sTbsId = smsGetTbsId( sSegmentHandle );
        
        STL_TRY( smlValidateTablespace( sTbsId,
                                        aValidateBehavior,
                                        aEnv )
                 == STL_SUCCESS );
        
        STL_TRY( smsValidateSegment( sSegmentHandle, aEnv ) == STL_SUCCESS );

        if( aValidateBehavior & SME_VALIDATION_BEHAVIOR_UPDATE )
        {
            KNL_SYSTEM_FATAL_DASSERT( (SME_GET_RELATION_STATE( aRelationHandle ) ==
                                       SME_RELATION_STATE_ACTIVE),
                                      aEnv );
        }
    }
    else
    {
        /**
         * Segment Handle을 갖지 않는 Instant Relation은
         * validation을 skip한다.
         */
    }

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 주어진 Segment Id의 유효성을 검사한다.
 * @param[in] aSegmentId 검사할 Segment의 Physical Identifier
 */
stlBool smlIsValidSegmentId( stlInt64 aSegmentId )
{
    return smsIsValidSegmentId( aSegmentId );
}

/**
 * @brief Relation의 Type을 구한다.
 * @param[in] aRelationHandle Relation Handle
 */
smlRelationType smlGetRelationType( void * aRelationHandle )
{
    return SME_GET_RELATION_TYPE( aRelationHandle );
}

/**
 * @brief ALTER TABLE이 가능 테이블인지 체크한다.
 * @param[in] aTableHandle Table Handle
 */
stlBool smlIsAlterableTable( void * aTableHandle )
{
    smlRelationType   sRelType;

    sRelType = SME_GET_RELATION_TYPE( aTableHandle );

    return (sRelType == SML_MEMORY_PERSISTENT_HEAP_ROW_TABLE);
}

/**
 * @brief Segment에 할당 가능한 최대 크기(byte)를 반환한다.
 * @param[in] aTbsId Tablespace identifier
 */
stlInt64 smlSegmentMaxSize( smlTbsId aTbsId )
{
    return SMS_MAXSIZE_DEFAULT( SMF_GET_PAGE_COUNT_IN_EXTENT( aTbsId ) );
}

/** @} */
