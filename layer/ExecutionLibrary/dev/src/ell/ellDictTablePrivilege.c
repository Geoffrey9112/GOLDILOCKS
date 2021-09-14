/*******************************************************************************
 * ellDictTablePrivilege.c
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


/**
 * @file ellDictTablePrivilege.c
 * @brief Table Privilege Dictionary 관리 모듈
 */

#include <ell.h>
#include <elDef.h>

#include <eldt.h>
#include <eldc.h>
#include <eldDictionary.h>
#include <eldIntegrity.h>


/**
 * @addtogroup ellPrivTable
 * @{
 */

/**
 * @brief Table Privilge 의 String 상수
 */
const stlChar * const gEllTablePrivActionString[ELL_TABLE_PRIV_ACTION_MAX] =
{
    "N/A",            /**< ELL_TABLE_PRIV_ACTION_NOT_AVAILABLE */

    "CONTROL TABLE",  /**< ELL_TABLE_PRIV_ACTION_CONTROL_TABLE */
    "SELECT",         /**< ELL_TABLE_PRIV_ACTION_SELECT */
    "INSERT",         /**< ELL_TABLE_PRIV_ACTION_INSERT */
    "UPDATE",         /**< ELL_TABLE_PRIV_ACTION_UPDATE */
    "DELETE",         /**< ELL_TABLE_PRIV_ACTION_DELETE */
    "TRIGGER",        /**< ELL_TABLE_PRIV_ACTION_TRIGGER */
    "REFERENCES",     /**< ELL_TABLE_PRIV_ACTION_REFERENCES */

    "LOCK",           /**< ELL_TABLE_PRIV_ACTION_LOCK */
    "INDEX",          /**< ELL_TABLE_PRIV_ACTION_INDEX */
    "ALTER",          /**< ELL_TABLE_PRIV_ACTION_ALTER */
};


/**
 * @brief Table Privilege Descriptor 를 Dictionary 에 추가한다.
 * @param[in]  aTransID                 Transaction ID
 * @param[in]  aStmt                    Statement
 * @param[in]  aGrantorID               Grantor ID
 * @param[in]  aGranteeID               Grantee ID
 * @param[in]  aSchemaID                Table 의 Schema ID
 * @param[in]  aTableID                 Table 의 Table ID
 * @param[in]  aTablePrivAction         Table Privilege Action
 * @param[in]  aIsGrantable             IS_GRANTABLE 여부 
 * @param[in]  aWithHierarchy           WITH_HIERARCHY  여부 
 * @param[in]  aEnv                     Execution Library Environment
 * @remarks
 */

stlStatus ellInsertTablePrivDesc( smlTransId           aTransID,
                                  smlStatement       * aStmt,
                                  stlInt64             aGrantorID,
                                  stlInt64             aGranteeID,
                                  stlInt64             aSchemaID,
                                  stlInt64             aTableID,
                                  ellTablePrivAction   aTablePrivAction,
                                  stlBool              aIsGrantable,
                                  stlBool              aWithHierarchy,
                                  ellEnv             * aEnv )
{
    knlValueBlockList        * sDataValueList  = NULL;

    smlRid              sRowRid;
    smlScn              sRowScn;
    smlRowBlock         sRowBlock;
    eldMemMark          sMemMark;

    stlInt32  sState = 0;
    
    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aGranteeID > 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aGrantorID > 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSchemaID > 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableID > 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aTablePrivAction > ELL_TABLE_PRIV_ACTION_NA) &&
                        (aTablePrivAction < ELL_TABLE_PRIV_ACTION_MAX),
                        ELL_ERROR_STACK(aEnv) );

    /**************************************************************
     * Privilege Record 추가
     **************************************************************/
    
    /**
     * - Data Value List와 Row Block을 위한 공간 할당 및 초기화
     */
    
    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_TABLE_PRIVILEGES,
                                     & sMemMark,
                                     & sDataValueList,
                                     aEnv )
             == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( smlInitSingleRowBlock( &sRowBlock,
                                    &sRowRid,
                                    &sRowScn,
                                    SML_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * - database privilege descriptor의 정보를 구성한다.
     */

    /*
     * GRANTOR_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLE_PRIVILEGES,
                                ELDT_TablePriv_ColumnOrder_GRANTOR_ID,
                                sDataValueList,
                                & aGrantorID,
                                aEnv )
             == STL_SUCCESS );
    
    /*
     * GRANTEE_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLE_PRIVILEGES,
                                ELDT_TablePriv_ColumnOrder_GRANTEE_ID,
                                sDataValueList,
                                & aGranteeID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * SCHEMA_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLE_PRIVILEGES,
                                ELDT_TablePriv_ColumnOrder_SCHEMA_ID,
                                sDataValueList,
                                & aSchemaID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * TABLE_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLE_PRIVILEGES,
                                ELDT_TablePriv_ColumnOrder_TABLE_ID,
                                sDataValueList,
                                & aTableID,
                                aEnv )
             == STL_SUCCESS );
    
    /*
     * PRIVILEGE_TYPE
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLE_PRIVILEGES,
                                ELDT_TablePriv_ColumnOrder_PRIVILEGE_TYPE,
                                sDataValueList,
                                (void *) gEllTablePrivActionString[aTablePrivAction],
                                aEnv )
             == STL_SUCCESS );

    /*
     * PRIVILEGE_TYPE_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLE_PRIVILEGES,
                                ELDT_TablePriv_ColumnOrder_PRIVILEGE_TYPE_ID,
                                sDataValueList,
                                & aTablePrivAction,
                                aEnv )
             == STL_SUCCESS );
    
    /*
     * IS_GRANTABLE
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLE_PRIVILEGES,
                                ELDT_TablePriv_ColumnOrder_IS_GRANTABLE,
                                sDataValueList,
                                & aIsGrantable,
                                aEnv )
             == STL_SUCCESS );

    /*
     * WITH_HIERARCHY
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLE_PRIVILEGES,
                                ELDT_TablePriv_ColumnOrder_WITH_HIERARCHY,
                                sDataValueList,
                                & aWithHierarchy,  
                                aEnv )
             == STL_SUCCESS );

    /**
     * IS_BUILTIN
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLE_PRIVILEGES,
                                ELDT_TablePriv_ColumnOrder_IS_BUILTIN,
                                sDataValueList,
                                & gEllIsMetaBuilding,
                                aEnv )
             == STL_SUCCESS );
    
    
    /**
     * - 레코드를 넣는다.
     */

    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sDataValueList, 0, 1 );
    
    STL_TRY( smlInsertRecord( aStmt,
                              gEldTablePhysicalHandle[ELDT_TABLE_ID_TABLE_PRIVILEGES],
                              sDataValueList,
                              NULL, /* unique violation */
                              & sRowBlock,
                              SML_ENV( aEnv ) )
             == STL_SUCCESS );
                              
    /**
     * Dictionary 무결성 유지 
     */

    if ( gEllIsMetaBuilding == STL_TRUE )
    {
        /**
         * 모두 구축후 무결성 검사함.
         */
    }
    else
    {
        STL_TRY( eldRefineIntegrityRowInsert( aTransID,
                                              aStmt,
                                              ELDT_TABLE_ID_TABLE_PRIVILEGES,
                                              & sRowBlock,
                                              sDataValueList,
                                              aEnv )
                 == STL_SUCCESS );
    }
    
    /**************************************************************
     * Privilege Cache 추가
     **************************************************************/

    STL_TRY( eldcInsertCachePriv( aTransID,
                                  aStmt,
                                  ELDC_PRIVCACHE_TABLE,
                                  sDataValueList,
                                  aEnv )
             == STL_SUCCESS );

    /**
     * 사용한 메모리를 해제한다.
     */
    
    sState = 0;
    STL_TRY( eldFreeTableValueList( & sMemMark, aEnv ) == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    switch (sState)
    {
        case 1:
            (void) eldFreeTableValueList( & sMemMark, aEnv );
        default:
            break;
    }
    
    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/**
 * @brief Current User 의 Grantable Prvilege for ALL PRIVILEGES ON TABLE 목록을 획득한다.
 * @param[in]  aTransID                 Transaction ID
 * @param[in]  aStmt                    Statement
 * @param[in]  aTableHandle             Table Handle
 * @param[out] aPrivList                Grantable Privilege List
 * @param[in]  aRegionMem               Region Memory
 * @param[in]  aEnv                     Execution Library Environment
 * @remarks
 */
stlStatus ellGetGrantableAllTablePriv( smlTransId          aTransID,
                                       smlStatement      * aStmt,
                                       ellDictHandle     * aTableHandle,
                                       ellPrivList      ** aPrivList,
                                       knlRegionMem      * aRegionMem,
                                       ellEnv            * aEnv )
{
    ellDictHandle * sSessionUser = NULL;
    stlInt64        sGranteeID = ELL_DICT_OBJECT_ID_NA;
    stlInt64        sTableID = ELL_DICT_OBJECT_ID_NA;

    ellPrivList * sAllPrivList = NULL;
    ellPrivList * sGrantableList = NULL;

    ellPrivItem   * sPrivItem = NULL;

    stlBool   sDuplicate = STL_FALSE;

    stlInt32        i = 0;
    stlInt32        sColCount = 0;
    ellDictHandle * sColHandle = NULL;

    stlBool             sNeedColPriv = STL_FALSE;
    ellPrivDesc         sColPrivDesc;
    ellColumnPrivAction sColPrivAction = ELL_COLUMN_PRIV_ACTION_NA;

    ellTableType    sTableType = ELL_TABLE_TYPE_NA;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPrivList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */

    sSessionUser = ellGetSessionUserHandle( aEnv );
    sGranteeID = ellGetAuthID( sSessionUser );
    sTableID = ellGetTableID( aTableHandle );
    sTableType = ellGetTableType( aTableHandle );

    /**
     * Base Table 만이 Column Privilege 가 존재한다.
     */
    if ( sTableType == ELL_TABLE_TYPE_BASE_TABLE )
    {
        sColCount = ellGetTableColumnCnt( aTableHandle );
        sColHandle = ellGetTableColumnDictHandle( aTableHandle );
    }
    else
    {
        sColCount  = 0;
        sColHandle = NULL;
    }

    /*********************************************************
     * ALL TABLE PRIVILEGES
     *********************************************************/
    
    /**
     * Table ID 가 동일한 모든 TABLE PRIVILEGE 획득
     */

    STL_TRY( ellInitPrivList( & sAllPrivList,
                              STL_TRUE,  /* aForRevoke, 모든 Privilege 목록을 얻기 위함 */
                              aRegionMem,
                              aEnv )
             == STL_SUCCESS );

    STL_TRY( eldGetPrivList( aTransID,
                             aStmt,
                             ELL_PRIV_TABLE,
                             ELDT_TablePriv_ColumnOrder_TABLE_ID,
                             sTableID,
                             aRegionMem,
                             sAllPrivList,
                             aEnv )
             == STL_SUCCESS );

    /**
     * 동일한 Grantee 와 WITH GRANT OPTION 을 가진 Privilege 만 중복 없이 추출
     */

    STL_TRY( ellInitPrivList( & sGrantableList,
                              STL_FALSE,  /* aForRevoke */
                              aRegionMem,
                              aEnv )
             == STL_SUCCESS );
    
    STL_RING_FOREACH_ENTRY( & sAllPrivList->mHeadList, sPrivItem )
    {
        if ( (sPrivItem->mPrivDesc.mWithGrant == STL_TRUE) &&
             (sPrivItem->mPrivDesc.mGranteeID == sGranteeID) )
        {
            /**
             * 동일한 GRANTEE 와 WITH GRANT OPTION 을 가지고 있음
             */

            STL_TRY( ellAddNewPrivItem( sGrantableList,
                                        sPrivItem->mPrivDesc.mPrivObject,
                                        & sPrivItem->mPrivHandle,
                                        NULL,   /* aPrivDesc */
                                        & sDuplicate,
                                        aRegionMem,
                                        aEnv )
                     == STL_SUCCESS );

            /**
             * 중복이 발생할 수 있다.
             *
             * u1 --> grantee WITH GRANT OPTION
             * u2 --> grantee WITh GRANT OPTION
             */

            /**
             * 다음 TABLE PRIVILEGE Action 은 모든 Column 에 대한 COLUMN PRIVILEGE Action 을 추가한다.
             */

            switch ( sPrivItem->mPrivDesc.mPrivAction.mTableAction )
            {
                case ELL_TABLE_PRIV_ACTION_SELECT:
                    sNeedColPriv = STL_TRUE;
                    sColPrivAction = ELL_COLUMN_PRIV_ACTION_SELECT;
                    break;
                case ELL_TABLE_PRIV_ACTION_INSERT:
                    sNeedColPriv = STL_TRUE;
                    sColPrivAction = ELL_COLUMN_PRIV_ACTION_INSERT;
                    break;
                case ELL_TABLE_PRIV_ACTION_UPDATE:
                    sNeedColPriv = STL_TRUE;
                    sColPrivAction = ELL_COLUMN_PRIV_ACTION_UPDATE;
                    break;
                case ELL_TABLE_PRIV_ACTION_REFERENCES:
                    sNeedColPriv = STL_TRUE;
                    sColPrivAction = ELL_COLUMN_PRIV_ACTION_REFERENCES;
                    break;
                default:
                    /**
                     * nothing to do
                     */
                    sNeedColPriv = STL_FALSE;
                    sColPrivAction = ELL_COLUMN_PRIV_ACTION_NA;
                    break;
            }

            if ( sNeedColPriv == STL_TRUE )
            {
                /**
                 * COLUMN PRIVILEGE ACTION 을 추가해야 함.
                 */

                for ( i = 0; i < sColCount; i++ )
                {
                    if ( ellGetColumnUnused( & sColHandle[i] ) == STL_TRUE )
                    {
                        /**
                         * Unused Column 임
                         */
                        continue;
                    }
                    
                    stlMemcpy( & sColPrivDesc, & sPrivItem->mPrivDesc, STL_SIZEOF(ellPrivDesc) );
                    sColPrivDesc.mObjectID = ellGetColumnID( & sColHandle[i] );
                    sColPrivDesc.mPrivObject = ELL_PRIV_COLUMN;
                    sColPrivDesc.mPrivAction.mColumnAction = sColPrivAction;

                    STL_TRY( ellAddNewPrivItem( sGrantableList,
                                                sPrivItem->mPrivDesc.mPrivObject,
                                                NULL,             /* aPrivHandle */
                                                & sColPrivDesc,   /* aPrivDesc */
                                                & sDuplicate,
                                                aRegionMem,
                                                aEnv )
                             == STL_SUCCESS );
                }
            }
            else
            {
                /**
                 * nothing to ddo
                 */
            }
                
        }
        else
        {
            /**
             * WITH GRANT OPTION 이 없음
             */
        }
    }

    /*********************************************************
     * ALL COLUMN PRIVILEGES
     *********************************************************/
    
    /**
     * Table ID 가 동일한 모든 COLUMN PRIVILEGE 획득
     */

    STL_TRY( ellInitPrivList( & sAllPrivList,
                              STL_TRUE,  /* aForRevoke, 모든 Privilege 목록을 얻기 위함 */
                              aRegionMem,
                              aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldGetPrivList( aTransID,
                             aStmt,
                             ELL_PRIV_COLUMN,
                             ELDT_ColumnPriv_ColumnOrder_TABLE_ID,
                             sTableID,
                             aRegionMem,
                             sAllPrivList,
                             aEnv )
             == STL_SUCCESS );

    /**
     * 동일한 Grantee 와 WITH GRANT OPTION 을 가진 Privilege 만 중복 없이 추출
     */

    STL_RING_FOREACH_ENTRY( & sAllPrivList->mHeadList, sPrivItem )
    {
        if ( (sPrivItem->mPrivDesc.mWithGrant == STL_TRUE) &&
             (sPrivItem->mPrivDesc.mGranteeID == sGranteeID) )
        {
            /**
             * 동일한 GRANTEE 와 WITH GRANT OPTION 을 가지고 있음
             */

            STL_TRY( ellAddNewPrivItem( sGrantableList,
                                        sPrivItem->mPrivDesc.mPrivObject,
                                        & sPrivItem->mPrivHandle,
                                        NULL,   /* aPrivDesc */
                                        & sDuplicate,
                                        aRegionMem,
                                        aEnv )
                     == STL_SUCCESS );

            /**
             * 중복이 발생할 수 있다.
             */
        }
        else
        {
            /**
             * WITH GRANT OPTION 이 없음
             */
        }
    }
    
    /**
     * Output 설정
     */

    *aPrivList = sGrantableList;

    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}



/**
 * @brief Current User 가 Revokee 에게 부여한 Revokable Prvilege for ALL PRIVILEGES ON TABLE 목록을 획득한다.
 * @param[in]  aTransID                 Transaction ID
 * @param[in]  aStmt                    Statement
 * @param[in]  aTableHandle             Table Handle
 * @param[in]  aRevokeeID               Revokee ID
 * @param[in]  aAccessControl           ACCESS CONTROL 권한 소유 여부 
 * @param[in]  aGrantOption             GRANT OPTION FOR
 * @param[out] aPrivList                Revokable Privilege List
 * @param[in]  aRegionMem               Region Memory
 * @param[in]  aEnv                     Execution Library Environment
 * @remarks
 */
stlStatus ellGetRevokableAllTablePriv( smlTransId          aTransID,
                                       smlStatement      * aStmt,
                                       ellDictHandle     * aTableHandle,
                                       stlInt64            aRevokeeID,
                                       stlBool             aAccessControl,
                                       stlBool             aGrantOption,
                                       ellPrivList      ** aPrivList,
                                       knlRegionMem      * aRegionMem,
                                       ellEnv            * aEnv )
{
    ellDictHandle * sSessionUser = NULL;
    stlInt64        sTableID = ELL_DICT_OBJECT_ID_NA;
    stlInt64        sGrantorID = ELL_DICT_OBJECT_ID_NA;
    
    ellPrivList * sAllTabPrivList = NULL;
    ellPrivList * sAllColPrivList = NULL;
    ellPrivList * sRevokableList = NULL;
    ellPrivList * sFinalPrivList = NULL;

    ellPrivItem   * sPrivItem = NULL;

    stlBool   sDuplicate = STL_FALSE;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPrivList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */

    sSessionUser = ellGetSessionUserHandle( aEnv );
    sGrantorID = ellGetAuthID( sSessionUser );
    sTableID = ellGetTableID( aTableHandle );

    /******************************************************
     * Table Privilege 추출
     ******************************************************/
    
    /**
     * Table ID 가 동일한 모든 TABLE PRIVILEGE 획득
     */

    STL_TRY( ellInitPrivList( & sAllTabPrivList,
                              STL_TRUE,  /* aForRevoke, 모든 Privilege 목록을 얻기 위함 */
                              aRegionMem,
                              aEnv )
             == STL_SUCCESS );

    STL_TRY( eldGetPrivList( aTransID,
                             aStmt,
                             ELL_PRIV_TABLE,
                             ELDT_TablePriv_ColumnOrder_TABLE_ID,
                             sTableID,
                             aRegionMem,
                             sAllTabPrivList,
                             aEnv )
             == STL_SUCCESS );
    
    /**
     * Revoker 와 Revokee 가 동일한 Privilege 만 중복 없이 추출
     */

    STL_TRY( ellInitPrivList( & sRevokableList,
                              STL_TRUE,  /* aForRevoke */
                              aRegionMem,
                              aEnv )
             == STL_SUCCESS );
    
    STL_RING_FOREACH_ENTRY( & sAllTabPrivList->mHeadList, sPrivItem )
    {
        /**
         * Revoker 와 Revokee 가 동일한 Privilege 를 추출
         */

        if ( sPrivItem->mPrivDesc.mGranteeID == aRevokeeID )
        {
            if ( (sPrivItem->mPrivDesc.mGrantorID == sGrantorID) ||
                 ((aAccessControl == STL_TRUE) && (sPrivItem->mPrivDesc.mGrantorID != ELDT_AUTH_ID_SYSTEM)) )
            {
                STL_TRY( ellAddNewPrivItem( sRevokableList,
                                            sPrivItem->mPrivDesc.mPrivObject,
                                            & sPrivItem->mPrivHandle,
                                            NULL,   /* aPrivDesc */
                                            & sDuplicate,
                                            aRegionMem,
                                            aEnv )
                         == STL_SUCCESS );
                
                /**
                 * 중복이 발생하면 안됨
                 */
                
                STL_ASSERT( sDuplicate == STL_FALSE );
            }
            else
            {
                /**
                 * Grantor 가 아님 
                 */
            }
        }
        else
        {
            /**
             * 다른 Object 임
             */
        }
    }

    /*********************************************************
     * Column Privilege 추출
     *********************************************************/
    
    /**
     * Table ID 가 동일한 모든 COLUMN PRIVILEGE 획득
     */

    STL_TRY( ellInitPrivList( & sAllColPrivList,
                              STL_TRUE,  /* aForRevoke, 모든 Privilege 목록을 얻기 위함 */
                              aRegionMem,
                              aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldGetPrivList( aTransID,
                             aStmt,
                             ELL_PRIV_COLUMN,
                             ELDT_ColumnPriv_ColumnOrder_TABLE_ID,
                             sTableID,
                             aRegionMem,
                             sAllColPrivList,
                             aEnv )
             == STL_SUCCESS );

    /**
     * Revoker 와 Revokee 가 동일한 Privilege 만 중복 없이 추출
     */

    STL_RING_FOREACH_ENTRY( & sAllColPrivList->mHeadList, sPrivItem )
    {
        /**
         * Revoker 와 Revokee 가 동일한 Privilege 를 추출
         */

        if ( sPrivItem->mPrivDesc.mGranteeID == aRevokeeID )
        {
            if ( (sPrivItem->mPrivDesc.mGrantorID == sGrantorID) ||
                 ((aAccessControl == STL_TRUE) && (sPrivItem->mPrivDesc.mGrantorID != ELDT_AUTH_ID_SYSTEM)) )
            {
                STL_TRY( ellAddNewPrivItem( sRevokableList,
                                            sPrivItem->mPrivDesc.mPrivObject,
                                            & sPrivItem->mPrivHandle,
                                            NULL,   /* aPrivDesc */
                                            & sDuplicate,
                                            aRegionMem,
                                            aEnv )
                         == STL_SUCCESS );
                
                /**
                 * 중복이 발생하면 안됨
                 */
                
                STL_ASSERT( sDuplicate == STL_FALSE );
            }
            else
            {
                /**
                 * Grantor 가 아님 
                 */
            }
        }
        else
        {
            /**
             * 다른 Object 임
             */
        }
    }
    
    /*********************************************************
     * GRANT OPTION FOR 인 경우 With Grant 만 추출 
     *********************************************************/

    if ( aGrantOption == STL_FALSE )
    {
        sFinalPrivList = sRevokableList;
    }
    else
    {
        STL_TRY( ellInitPrivList( & sFinalPrivList,
                                  STL_TRUE,  /* aForRevoke */
                                  aRegionMem,
                                  aEnv )
             == STL_SUCCESS );
    
        STL_RING_FOREACH_ENTRY( & sRevokableList->mHeadList, sPrivItem )
        {
            if ( sPrivItem->mPrivDesc.mWithGrant == STL_TRUE )
            {
                /**
                 * WITH GRANT OPTION 을 가지고 있음
                 */

                
                STL_TRY( ellAddNewPrivItem( sFinalPrivList,
                                            sPrivItem->mPrivDesc.mPrivObject,
                                            & sPrivItem->mPrivHandle,
                                            NULL,   /* aPrivDesc */
                                            & sDuplicate,
                                            aRegionMem,
                                            aEnv )
                         == STL_SUCCESS );

                /**
                 * 중복이 존재할 수 없음
                 */

                STL_ASSERT( sDuplicate == STL_FALSE );
            }
            else
            {
                /**
                 * WITH GRANT OPTION 이 없음
                 */
            }
        }
    }

    
    /**
     * Output 설정
     */

    *aPrivList = sFinalPrivList;

    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}

/** @} ellPrivTable */
