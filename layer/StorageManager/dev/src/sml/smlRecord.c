/*******************************************************************************
 * smlRecord.c
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

#include <stl.h>
#include <knl.h>
#include <scl.h>
#include <dtl.h>
#include <sml.h>
#include <smDef.h>
#include <sme.h>
#include <sma.h>
#include <smd.h>
#include <sms.h>
#include <smf.h>
#include <smkl.h>
#include <smxl.h>
#include <smdmifDef.h>
#include <smdmifFixedPart.h>
#include <smdmifTable.h>

/**
 * @file smlRecord.c
 * @brief Storage Manager Layer Record Routines
 */

/**
 * @addtogroup smlRecord
 * @{
 */

/**
 * @brief 레코드를 block단위로 삽입한다.
 * @param[in] aStatement Statement 구조체
 * @param[in] aRelHandle Insert할 테이블의 Relation Handle
 * @param[in] aInsertCols 삽입될 Record 들의 컬럼값 리스트의 배열
 * @param[out] aUniqueViolation 삽입할 record가 unique 체크에 걸려 insert되지 않았는지 여부
 * @param[out] aRowBlock 삽입된 레코드의 RID/SCN 배열
 * @param[in,out] aEnv Environment 구조체
 * @remark
 *     @a aColumnValueList : 모든 컬럼은 컬럼 아이디순에 의해서 정렬되어 있어야 한다.
 */
stlStatus smlInsertRecord( smlStatement      * aStatement,
                           void              * aRelHandle,
                           knlValueBlockList * aInsertCols,
                           knlValueBlockList * aUniqueViolation,
                           smlRowBlock       * aRowBlock,
                           smlEnv            * aEnv )
{
    smlRelationType   sRelType = SME_GET_RELATION_TYPE( aRelHandle );
    smdTableModule  * sModule = smdGetTableModule(SML_GET_TABLE_TYPE(sRelType));
    smlRid            sRid;

    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    /**
     * Validate relation
     */

    STL_TRY( smlValidateRelation( aRelHandle,
                                  SME_VALIDATION_BEHAVIOR_UPDATE,
                                  aEnv )
             == STL_SUCCESS );

    /**
     * Query Timeout 검사
     */
    STL_TRY( knlCheckQueryTimeout( KNL_ENV(aEnv) ) == STL_SUCCESS );
    
    if( SML_RELATION_IS_INSTANT_TABLE( sRelType ) == STL_FALSE )
    {
        /* instant table에 대해서는 아래 작업을 하지 않아야 한다. */
        STL_TRY( smxlPrepareTransRecord( SMA_GET_TRANS_ID(aStatement),
                                         aEnv )
                 == STL_SUCCESS );
    }

#ifdef STL_DEBUG
    if ( knlGetPropertyBigIntValueByID( KNL_PROPERTY_DATABASE_TEST_OPTION, KNL_ENV(aEnv) ) > 0 )
    {
        knlVerifyWriteDataValue( aInsertCols,
                                 KNL_GET_BLOCK_SKIP_CNT( aInsertCols ),
                                 KNL_GET_BLOCK_USED_CNT( aInsertCols ),
                                 KNL_ENV(aEnv) );
    }
#endif
    
    if( ( KNL_GET_BLOCK_USED_CNT( aInsertCols ) -
          KNL_GET_BLOCK_SKIP_CNT( aInsertCols ) ) > 1 )
    {
        KNL_BREAKPOINT( KNL_BREAKPOINT_SMLINSERTRECORD_BEFORE_INSERT,
                        KNL_ENV(aEnv) );

        STL_TRY( sModule->mBlockInsert( aStatement,
                                        aRelHandle,
                                        aInsertCols,
                                        aUniqueViolation,
                                        aRowBlock,
                                        aEnv ) == STL_SUCCESS );
    }
    else
    {
        STL_TRY( sModule->mInsert( aStatement,
                                   aRelHandle,
                                   KNL_GET_BLOCK_SKIP_CNT(aInsertCols),
                                   aInsertCols,
                                   aUniqueViolation,
                                   &sRid,
                                   aEnv ) == STL_SUCCESS );
        
        SML_SET_RID_VALUE( aRowBlock, KNL_GET_BLOCK_SKIP_CNT(aInsertCols), sRid );
        SML_SET_USED_BLOCK_SIZE( aRowBlock, KNL_GET_BLOCK_SKIP_CNT(aInsertCols) + 1 );
    }
    
    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief 레코드를 한 건 삭제한다.
 * @param[in] aStatement Statement 구조체
 * @param[in] aRelHandle Delete할 테이블의 Relation Handle
 * @param[in] aRowRid 삭제할 레코드 아이디
 * @param[in] aRowScn 레코드 검색 당시의 SCN
 * @param[in] aValueIdx primary key가 사용하고 있는 block idx
 * @param[in] aPrimaryKey 해당 레코드의 primary key
 * @param[out] aVersionConflict delete 시도 도중에 version conflict 상황을 만났는지 여부
 * @param[out] aSkipped 이미 삭제된 Row인지 여부
 * @param[in,out] aEnv Environment 구조체
 * @remarks Instant Hash Table에 사용할 경우 @a aPrimaryKey는 삭제할 키 컬럼 리스트이고, <BR>
 *          삭제할 대상 키는 1개로 제한한다. <BR>
 * @todo 향후 Instant Hash Table 관련해서는 IOT 전용 인터페이스를 만들 필요가 있다.
 */
stlStatus smlDeleteRecord( smlStatement      * aStatement,
                           void              * aRelHandle,
                           smlRid            * aRowRid,
                           smlScn              aRowScn,
                           stlInt32            aValueIdx,
                           knlValueBlockList * aPrimaryKey,
                           stlBool           * aVersionConflict,
                           stlBool           * aSkipped,
                           smlEnv            * aEnv )
{
    smlRelationType   sRelType = SME_GET_RELATION_TYPE( aRelHandle );
    smdTableModule  * sModule = smdGetTableModule(SML_GET_TABLE_TYPE(sRelType));
    stlBool           sVersionConflict = STL_FALSE;
    smxlTransId       sTransId = SMA_GET_TRANS_ID(aStatement);

    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    /**
     * Validate relation
     */

    STL_TRY( smlValidateRelation( aRelHandle,
                                  SME_VALIDATION_BEHAVIOR_UPDATE,
                                  aEnv )
             == STL_SUCCESS );

    /**
     * SQL layer 에서 Block 단위로 Query Timeout 검사함.
     */

    if( SML_RELATION_IS_INSTANT_TABLE( sRelType ) == STL_FALSE )
    {
        STL_TRY( smxlPrepareTransRecord( sTransId,
                                         aEnv )
                 == STL_SUCCESS );
    }

    KNL_BREAKPOINT( KNL_BREAKPOINT_SMLDELETERECORD_BEFORE_MODULEDELETE,
                    KNL_ENV(aEnv) );

    STL_TRY( sModule->mDelete( aStatement,
                               aRelHandle,
                               aRowRid,
                               aRowScn,
                               aValueIdx,
                               aPrimaryKey,
                               &sVersionConflict,
                               aSkipped,
                               aEnv ) == STL_SUCCESS );

    if( smxlGetIsolationLevel( sTransId ) == SML_TIL_SERIALIZABLE )
    {
        STL_TRY_THROW( sVersionConflict == STL_FALSE, RAMP_ERR_SERIALIZE_TRANSACTION );
    }

    if( aVersionConflict != NULL )
    {
        *aVersionConflict = sVersionConflict;
    }

    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_SERIALIZE_TRANSACTION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_SERIALIZE_TRANSACTION,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }
    
    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief 레코드를 한 건 변경한다.
 * @param[in]     aStatement             Statement 구조체
 * @param[in]     aRelHandle             Update할 테이블의 Relation Handle
 * @param[in]     aRowRid                갱신할 레코드 아이디
 * @param[in]     aRowScn                레코드 검색 당시의 SCN
 * @param[in]     aValueIdx              현재 갱신 대상 row가 사용하고 있는 block idx
 * @param[in]     aAfterCols             변경될 Record의 컬럼값 리스트
 * @param[out]    aBeforeCols            변경될 Record의 이전 값을 저장하기 위한 버퍼
 * @param[in]     aPrimaryKey            해당 레코드의 primary key
 * @param[out]    aVersionConflict       update 시도 도중에 version conflict 상황을 만났는지 여부
 * @param[out]    aSkipped               이미 갱신된 Row인지 여부
 * @param[in,out] aEnv                   Environment 구조체
 * @remark
 *     @a aAfterCols : 변경될 모든 컬럼은 컬럼 아이디순에 의해서 정렬되어 있어야 한다.
 *     @a aBeforerCols : QP에서 채워진 값이 아니라 SM에서 채워야 한다.
 */
stlStatus smlUpdateRecord( smlStatement      * aStatement,
                           void              * aRelHandle,
                           smlRid            * aRowRid,
                           smlScn              aRowScn,
                           stlInt32            aValueIdx,
                           knlValueBlockList * aAfterCols,
                           knlValueBlockList * aBeforeCols,
                           knlValueBlockList * aPrimaryKey,
                           stlBool           * aVersionConflict,
                           stlBool           * aSkipped,
                           smlEnv            * aEnv )
{
    smlRelationType   sRelType = SME_GET_RELATION_TYPE( aRelHandle );
    smdTableModule  * sModule = smdGetTableModule(SML_GET_TABLE_TYPE(sRelType));
    stlBool           sVersionConflict = STL_FALSE;
    smxlTransId       sTransId = SMA_GET_TRANS_ID(aStatement);

    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    /**
     * Validate relation
     */

    STL_TRY( smlValidateRelation( aRelHandle,
                                  SME_VALIDATION_BEHAVIOR_UPDATE,
                                  aEnv )
             == STL_SUCCESS );

    /**
     * SQL layer 에서 Block 단위로 Query Timeout 검사함.
     */

    if( SML_RELATION_IS_INSTANT_TABLE( sRelType ) == STL_FALSE )
    {
        STL_TRY( smxlPrepareTransRecord( sTransId,
                                         aEnv )
                 == STL_SUCCESS );
    }
    
    KNL_BREAKPOINT( KNL_BREAKPOINT_SMLUPDATERECORD_BEFORE_MODULEUPDATE,
                    KNL_ENV(aEnv) );

#ifdef STL_DEBUG
    if ( knlGetPropertyBigIntValueByID( KNL_PROPERTY_DATABASE_TEST_OPTION, KNL_ENV(aEnv) ) > 0 )
    {
        knlVerifyWriteDataValue( aAfterCols,
                                 aValueIdx,
                                 aValueIdx + 1,
                                 KNL_ENV(aEnv) );
    }
#endif
    
    STL_TRY( sModule->mUpdate( aStatement,
                               aRelHandle,
                               aRowRid,
                               aRowScn,
                               aValueIdx,
                               aAfterCols,
                               aBeforeCols,
                               aPrimaryKey,
                               &sVersionConflict,
                               aSkipped,
                               aEnv ) == STL_SUCCESS );

    if( smxlGetIsolationLevel( sTransId ) == SML_TIL_SERIALIZABLE )
    {
        STL_TRY_THROW( sVersionConflict == STL_FALSE, RAMP_ERR_SERIALIZE_TRANSACTION );
    }

    if( aVersionConflict != NULL )
    {
        *aVersionConflict = sVersionConflict;
    }

    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_SERIALIZE_TRANSACTION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_SERIALIZE_TRANSACTION,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }
    
    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}


/**
 * @brief 기존 레코드가 존재하면 합병하고, 없다면 삽입한다.
 * @param[in] aStatement Statement 구조체
 * @param[in] aRelHandle Insert할 테이블의 Relation Handle
 * @param[in] aInsertCols 삽입될 Record 들의 컬럼값 리스트의 배열
 * @param[in] aMergeReocordInfo 합병에 필요한 정보들
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smlMergeRecord( smlStatement       * aStatement,
                          void               * aRelHandle,
                          knlValueBlockList  * aInsertCols,
                          smlMergeRecordInfo * aMergeReocordInfo,
                          smlEnv             * aEnv )
{
    smlRelationType   sRelType = SME_GET_RELATION_TYPE( aRelHandle );
    smdTableModule  * sModule = smdGetTableModule(SML_GET_TABLE_TYPE(sRelType));
    smxlTransId       sTransId = SMA_GET_TRANS_ID(aStatement);

    STL_PARAM_VALIDATE( aMergeReocordInfo != NULL, KNL_ERROR_STACK( aEnv ) );
    
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    /**
     * Validate relation
     */

    STL_TRY( smlValidateRelation( aRelHandle,
                                  SME_VALIDATION_BEHAVIOR_UPDATE,
                                  aEnv )
             == STL_SUCCESS );

    /**
     * SQL layer 에서 Block 단위로 Query Timeout 검사함.
     */

    if( SML_RELATION_IS_INSTANT_TABLE( sRelType ) == STL_FALSE )
    {
        STL_TRY( smxlPrepareTransRecord( sTransId,
                                         aEnv )
                 == STL_SUCCESS );
    }
    
    STL_TRY( sModule->mMerge( aStatement,
                              aRelHandle,
                              aInsertCols,
                              aMergeReocordInfo,
                              aEnv )
             == STL_SUCCESS );

    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}


/**
 * @brief 레코드 한 건을 Fetch한다.
 * @param[in]  aStatement        Statement
 * @param[in]  aFetchRecordInfo  Fetch Record 정보
 * @param[in]  aRowRid           검색할 레코드 아이디
 * @param[in]  aBlockIdx         현재 갱신 대상 row가 사용하고 있는 block idx
 * @param[out] aIsMatched        Physical Filter에 부합하는 레코드 존재 여부
 * @param[out] aDeleted          레코드 삭제 여부
 * @param[out] aUpdated          레코드 갱신 여부
 * @param[in,out] aEnv           Environment 구조체
 * @remark
 *     @a aAfterCols : 변경될 모든 컬럼은 컬럼 아이디순에 의해서 정렬되어 있어야 한다.
 * @note TransReadMode가 SML_TRM_SNAPSHOT일 경우는 aDeleted를 NULL로 설정해야 한다.
 * @par Example:
 * <table>
 *     <caption align="TOP"> SQL에서 Read Mode의 사용 </caption>
 *     <tr>
 *         <td>  </td>
 *         <th>  SML_TRM_SNAPSHOT  </th>
 *         <th>  SML_TRM_RECENT  </th>
 *     </tr>
 *     <tr>
 *         <th>  SML_SRM_SNAPSHOT </th>
 *         <td>  SELECT </td>
 *         <td>  DML </td>
 *     </tr>
 *     <tr>
 *         <th>  SML_SRM_RECENT </th>
 *         <td>  X </td>
 *         <td>  DDL, FOREIGN CONSTRAINT 검사 </td>
 *     </tr>
 * </table>
 */

stlStatus smlFetchRecord( smlStatement        * aStatement,
                          smlFetchRecordInfo  * aFetchRecordInfo,
                          smlRid              * aRowRid,
                          stlInt32              aBlockIdx,
                          stlBool             * aIsMatched,
                          stlBool             * aDeleted,
                          stlBool             * aUpdated,
                          smlEnv              * aEnv )
{
    smlRelationType   sRelType = SME_GET_RELATION_TYPE( aFetchRecordInfo->mRelationHandle );
    smdTableModule  * sModule  = smdGetTableModule(SML_GET_TABLE_TYPE(sRelType));
    stlBool           sDeleted = STL_FALSE;
    smlScn            sRowScn;

    STL_PARAM_VALIDATE( aStatement != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aFetchRecordInfo != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aFetchRecordInfo->mRelationHandle != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aRowRid != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( sModule != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aIsMatched != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aFetchRecordInfo->mScnBlock != NULL, KNL_ERROR_STACK( aEnv ) );

    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    /**
     * Validate relation
     */

    STL_TRY( smlValidateRelation( aFetchRecordInfo->mRelationHandle,
                                  SME_VALIDATION_BEHAVIOR_READ,
                                  aEnv )
             == STL_SUCCESS );

    if( aDeleted != NULL )
    {
        *aDeleted = STL_FALSE;
    }
    
    if( aFetchRecordInfo->mTransReadMode == SML_TRM_SNAPSHOT )
    {
        aFetchRecordInfo->mViewScn = aStatement->mScn;
    }
    else
    {
        /**
         * committed read의 경우 view scn을 무한대로 설정하면 안된다.
         * - version traversing 동안 잘못된 버전을 fetch하는 경우가 발생한다.
         *
         * ----------------------------------------------------------------------> scn
         *             |                   |          |         |        |
         *      t1:update(1->2)      t1:update(2->3)  |     t1:commit    |
         *                                            |                  |
         *                                       t2:fetch(3)       t2:fetch(2)
         *                                       
         *   t2:fetch(3) 은 t1이 commit되지 않았기 때문에 이전버전을 fetch해야 한다.
         *   문제는 t1이 commit된 이후에 t2:fetch(2)를 하면 visibility가 true로 된다.
         *   t1가 fetch한 2 값은 t1 의 최종값이 아니다.
         *   따라서, view scn은 system scn으로 설정되어야 한다.
         */
        
        aFetchRecordInfo->mViewScn = smxlGetSystemScn();
    }

    if( aFetchRecordInfo->mStmtReadMode == SML_SRM_SNAPSHOT )
    {
        aFetchRecordInfo->mViewTcn = aStatement->mTcn;
    }
    else
    {
        aFetchRecordInfo->mViewTcn = SML_INFINITE_TCN;
    }

    sRowScn = aFetchRecordInfo->mScnBlock[ aBlockIdx ];
    STL_TRY( sModule->mFetch( aStatement,
                              aFetchRecordInfo,
                              aRowRid,
                              aBlockIdx,
                              aIsMatched,
                              & sDeleted,
                              aUpdated,
                              aEnv ) == STL_SUCCESS );

    if( sDeleted == STL_TRUE )
    {
        /**
         * 삭제된 레코드를 만날수 있는 경우는 READ COMMITTED VIEW를 원하는 경우여야 한다.
         */
        KNL_ASSERT( (aFetchRecordInfo->mTransReadMode == SML_TRM_COMMITTED) ||
                    (aFetchRecordInfo->mStmtReadMode == SML_SRM_RECENT),
                    KNL_ENV(aEnv),
                    ( "VIEW_SCN(%ld), ITERATOR ROW_SCN(%ld), FETCH ROW SCN(%ld)\n",
                      aFetchRecordInfo->mViewScn,
                      sRowScn,
                      aFetchRecordInfo->mScnBlock[ aBlockIdx ] ) );
    }
    
    if( aDeleted != NULL )
    {
        *aDeleted = sDeleted;
    }
    
    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}


/**
 * @brief ROWID에 해당하는 레코드 한 건을 Fetch한다.
 * @param[in]  aStatement        Statement
 * @param[in]  aFetchRecordInfo  Fetch Record 정보
 * @param[in]  aRowRid           검색할 레코드 아이디
 * @param[in]  aBlockIdx         현재 갱신 대상 row가 사용하고 있는 block idx
 * @param[out] aIsMatched        Physical Filter에 부합하는 레코드 존재 여부
 * @param[out] aDeleted          레코드 삭제 여부
 * @param[in,out] aEnv           Environment 구조체
 * @remark
 *     @a aAfterCols : 변경될 모든 컬럼은 컬럼 아이디순에 의해서 정렬되어 있어야 한다.
 * @note TransReadMode가 SML_TRM_SNAPSHOT일 경우는 aDeleted를 NULL로 설정해야 한다.
 * @par Example:
 * <table>
 *     <caption align="TOP"> SQL에서 Read Mode의 사용 </caption>
 *     <tr>
 *         <td>  </td>
 *         <th>  SML_TRM_SNAPSHOT  </th>
 *         <th>  SML_TRM_RECENT  </th>
 *     </tr>
 *     <tr>
 *         <th>  SML_SRM_SNAPSHOT </th>
 *         <td>  SELECT </td>
 *         <td>  DML </td>
 *     </tr>
 *     <tr>
 *         <th>  SML_SRM_RECENT </th>
 *         <td>  X </td>
 *         <td>  DDL, FOREIGN CONSTRAINT 검사 </td>
 *     </tr>
 * </table>
 */
stlStatus smlFetchRecordWithRowid( smlStatement        * aStatement,
                                   smlFetchRecordInfo  * aFetchRecordInfo,
                                   smlRid              * aRowRid,
                                   stlInt32              aBlockIdx,
                                   stlBool             * aIsMatched,
                                   stlBool             * aDeleted,
                                   smlEnv              * aEnv )
{
    smlRelationType   sRelType = SME_GET_RELATION_TYPE( aFetchRecordInfo->mRelationHandle );
    smdTableModule  * sModule  = smdGetTableModule(SML_GET_TABLE_TYPE(sRelType));
    stlBool           sDeleted = STL_FALSE;
    stlInt64          sSegmentId;
    smlRid            sSegmentRid;
    void            * sSegmentHandle;

    STL_PARAM_VALIDATE( aStatement != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aFetchRecordInfo != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aFetchRecordInfo->mRelationHandle != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aRowRid != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( sModule != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aIsMatched != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aFetchRecordInfo->mScnBlock != NULL, KNL_ERROR_STACK( aEnv ) );

    STL_DASSERT( aFetchRecordInfo->mTransReadMode == SML_TRM_SNAPSHOT );
    STL_DASSERT( aFetchRecordInfo->mStmtReadMode == SML_SRM_SNAPSHOT );

    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    /**
     * Validate relation
     */

    STL_TRY( smlValidateRelation( aFetchRecordInfo->mRelationHandle,
                                  SME_VALIDATION_BEHAVIOR_READ,
                                  aEnv )
             == STL_SUCCESS );
    
    sSegmentHandle = SME_GET_SEGMENT_HANDLE( aFetchRecordInfo->mRelationHandle );

    /**
     * Check invaild rowid
     */
    
    sSegmentRid = smsGetSegRid( sSegmentHandle );
    SMS_MAKE_SEGMENT_ID( &sSegmentId, &sSegmentRid );

    if( aDeleted != NULL )
    {
        *aDeleted = STL_FALSE;
    }
    
    aFetchRecordInfo->mViewScn = aStatement->mScn;
    aFetchRecordInfo->mViewTcn = aStatement->mTcn;

    STL_TRY( sModule->mFetchWithRowid( aStatement,
                                       aFetchRecordInfo,
                                       sSegmentId,
                                       smsGetValidSeqFromHandle( sSegmentHandle ),
                                       aRowRid,
                                       aBlockIdx,
                                       aIsMatched,
                                       & sDeleted,
                                       aEnv ) == STL_SUCCESS );

    if( aDeleted != NULL )
    {
        *aDeleted = sDeleted;
    }
    
    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Row Block을 초기화 한다.
 * @param[out] aRowBlock   Row Block 구조체
 * @param[in]  aBlockSize  Row Block의 크기(배열 크기)
 * @param[in]  aRegionMem  Region Memory
 * @param[out] aEnv        Environment 포인터
 */
stlStatus smlInitRowBlock( smlRowBlock  * aRowBlock,
                           stlInt32       aBlockSize,
                           knlRegionMem * aRegionMem,
                           smlEnv       * aEnv )
{
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    aRowBlock->mBlockSize = aBlockSize;
    aRowBlock->mUsedBlockSize = 0;

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF(smlScn) * aBlockSize,
                                (void **)&aRowBlock->mScnBlock,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF(smlRid) * aBlockSize,
                                (void **)&aRowBlock->mRidBlock,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief 한 개의 원소를 갖는 Row Block을 초기화 한다.
 * @param[out] aRowBlock Row Block 구조체
 * @param[in]  aRowRid   Single Row의 RID 포인터 
 * @param[in]  aRowScn   Single Row의 SCN 포인터 
 * @param[out] aEnv      Environment 포인터
 */
stlStatus smlInitSingleRowBlock( smlRowBlock  * aRowBlock,
                                 smlRid       * aRowRid,
                                 smlScn       * aRowScn,
                                 smlEnv       * aEnv )
{
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    aRowBlock->mBlockSize = 1;
    aRowBlock->mUsedBlockSize = 0;
    aRowBlock->mRidBlock = aRowRid;
    aRowBlock->mScnBlock = aRowScn;

    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;
}

/**
 * @brief instant table의 delete mark를 모두 clear한다.
 * @param[in] aRelationHandle 인스턴트 테이블 핸들
 * @param[out] aEnv      Environment 포인터
 */
stlStatus smlInstantTableUnsetDeletion( void   * aRelationHandle,
                                        smlEnv * aEnv )
{
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    /**
     * Validate relation
     */

    STL_TRY( knlCheckQueryTimeout( KNL_ENV(aEnv) ) == STL_SUCCESS );

    STL_TRY( smlValidateRelation( aRelationHandle,
                                  SME_VALIDATION_BEHAVIOR_UPDATE,
                                  aEnv )
             == STL_SUCCESS );

    STL_TRY( smdmifTableUnsetDeletion( aRelationHandle,
                                       aEnv ) == STL_SUCCESS );

    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/** @} */
