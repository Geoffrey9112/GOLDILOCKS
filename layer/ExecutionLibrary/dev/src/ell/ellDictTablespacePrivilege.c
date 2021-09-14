/*******************************************************************************
 * ellDictTablespacePrivilege.c
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
 * @file ellDictTablespacePrivilege.c
 * @brief Tablespace Privilege Dictionary 관리 모듈
 */

#include <ell.h>
#include <elDef.h>

#include <eldt.h>
#include <eldc.h>
#include <eldDictionary.h>
#include <eldIntegrity.h>


/**
 * @addtogroup ellPrivTablespace
 * @{
 */

/**
 * @brief Tablespace Privilge 의 String 상수
 */
const stlChar * const gEllSpacePrivActionString[ELL_SPACE_PRIV_ACTION_MAX] =
{
    "N/A",             /**< ELL_SPACE_PRIV_ACTION_NOT_AVAILABLE */

    "CREATE OBJECT",   /**< ELL_SPACE_PRIV_ACTION_CREATE_OBJECT */
};


/**
 * @brief Tablespace Privilege Descriptor 를 Dictionary 에 추가한다.
 * @param[in]  aTransID                 Transaction ID
 * @param[in]  aStmt                    Statement
 * @param[in]  aGrantorID               Grantor ID
 * @param[in]  aGranteeID               Grantee ID
 * @param[in]  aTablespaceID            Tablespace ID
 * @param[in]  aSpacePrivAction         Tablespace Privilege Action
 * @param[in]  aIsGrantable             GRANT 가능 여부 
 * @param[in]  aEnv                     Execution Library Environment
 * @remarks
 */
stlStatus ellInsertTablespacePrivDesc( smlTransId              aTransID,
                                       smlStatement          * aStmt,
                                       stlInt64                aGrantorID,
                                       stlInt64                aGranteeID,
                                       stlInt64                aTablespaceID,
                                       ellSpacePrivAction      aSpacePrivAction,
                                       stlBool                 aIsGrantable,
                                       ellEnv                * aEnv )
{
    knlValueBlockList        * sDataValueList  = NULL;

    smlRid               sRowRid;
    smlScn               sRowScn;
    smlRowBlock          sRowBlock;
    eldMemMark           sMemMark;

    stlInt32  sState = 0;
    
    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aGranteeID > 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aGrantorID > 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aSpacePrivAction > ELL_SPACE_PRIV_ACTION_NA) &&
                        (aSpacePrivAction < ELL_SPACE_PRIV_ACTION_MAX),
                        ELL_ERROR_STACK(aEnv) );

    /**************************************************************
     * Privilege Record 추가
     **************************************************************/
    
    /**
     * - Data Value List와 Row Block을 위한 공간 할당 및 초기화
     */
    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_TABLESPACE_PRIVILEGES,
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

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLESPACE_PRIVILEGES,
                                ELDT_SpacePriv_ColumnOrder_GRANTOR_ID,
                                sDataValueList,
                                & aGrantorID,
                                aEnv )
             == STL_SUCCESS );
    
    /*
     * GRANTEE_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLESPACE_PRIVILEGES,
                                ELDT_SpacePriv_ColumnOrder_GRANTEE_ID,
                                sDataValueList,
                                & aGranteeID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * TABLESPACE_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLESPACE_PRIVILEGES,
                                ELDT_SpacePriv_ColumnOrder_TABLESPACE_ID,
                                sDataValueList,
                                & aTablespaceID,
                                aEnv )
             == STL_SUCCESS );
    
    /*
     * PRIVILEGE_TYPE
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLESPACE_PRIVILEGES,
                                ELDT_SpacePriv_ColumnOrder_PRIVILEGE_TYPE,
                                sDataValueList,
                                (void *) gEllSpacePrivActionString[aSpacePrivAction],
                                aEnv )
             == STL_SUCCESS );

    /*
     * PRIVILEGE_TYPE_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLESPACE_PRIVILEGES,
                                ELDT_SpacePriv_ColumnOrder_PRIVILEGE_TYPE_ID,
                                sDataValueList,
                                & aSpacePrivAction,
                                aEnv )
             == STL_SUCCESS );
    
    /*
     * IS_GRANTABLE
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLESPACE_PRIVILEGES,
                                ELDT_SpacePriv_ColumnOrder_IS_GRANTABLE,
                                sDataValueList,
                                & aIsGrantable,
                                aEnv )
             == STL_SUCCESS );

    /**
     * IS_BUILTIN
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLESPACE_PRIVILEGES,
                                ELDT_SpacePriv_ColumnOrder_IS_BUILTIN,
                                sDataValueList,
                                & gEllIsMetaBuilding,
                                aEnv )
             == STL_SUCCESS );
    
    /**
     * - 레코드를 넣는다.
     */

    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sDataValueList, 0, 1 );
    
    STL_TRY( smlInsertRecord( aStmt,
                              gEldTablePhysicalHandle[ELDT_TABLE_ID_TABLESPACE_PRIVILEGES],
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
                                              ELDT_TABLE_ID_TABLESPACE_PRIVILEGES,
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
                                  ELDC_PRIVCACHE_SPACE,
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
 * @brief Current User 의 Grantable Prvilege for ALL PRIVILEGES ON TABLESAPCE 목록을 획득한다.
 * @param[in]  aTransID                 Transaction ID
 * @param[in]  aStmt                    Statement
 * @param[in]  aSpaceHandle             Tablespace Handle
 * @param[out] aPrivList                Grantable Privilege List
 * @param[in]  aRegionMem               Region Memory
 * @param[in]  aEnv                     Execution Library Environment
 * @remarks
 */
stlStatus ellGetGrantableAllSpacePriv( smlTransId          aTransID,
                                       smlStatement      * aStmt,
                                       ellDictHandle     * aSpaceHandle,
                                       ellPrivList      ** aPrivList,
                                       knlRegionMem      * aRegionMem,
                                       ellEnv            * aEnv )
{
    ellDictHandle * sSessionUser = NULL;
    stlInt64        sGranteeID = ELL_DICT_OBJECT_ID_NA;
    stlInt64        sSpaceID = ELL_DICT_OBJECT_ID_NA;

    ellPrivList * sAllPrivList = NULL;
    ellPrivList * sGrantableList = NULL;

    ellPrivItem   * sPrivItem = NULL;

    stlBool   sDuplicate = STL_FALSE;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSpaceHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPrivList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */

    sSessionUser = ellGetSessionUserHandle( aEnv );
    sGranteeID = ellGetAuthID( sSessionUser );
    sSpaceID = ellGetTablespaceID( aSpaceHandle );
    
    /**
     * Current User 가 Grantee 인 모든 Privilege 획득
     */

    STL_TRY( ellInitPrivList( & sAllPrivList,
                              STL_TRUE,  /* aForRevoke, 모든 Privilege 목록을 얻기 위함 */
                              aRegionMem,
                              aEnv )
             == STL_SUCCESS );

    STL_TRY( eldGetPrivList( aTransID,
                             aStmt,
                             ELL_PRIV_SPACE,
                             ELDT_SpacePriv_ColumnOrder_GRANTEE_ID,
                             sGranteeID,
                             aRegionMem,
                             sAllPrivList,
                             aEnv )
             == STL_SUCCESS );

    /**
     * 동일한 SPACE 와 WITH GRANT OPTION 을 가진 Privilege 만 중복 없이 추출
     */

    STL_TRY( ellInitPrivList( & sGrantableList,
                              STL_FALSE,  /* aForRevoke */
                              aRegionMem,
                              aEnv )
             == STL_SUCCESS );
    
    STL_RING_FOREACH_ENTRY( & sAllPrivList->mHeadList, sPrivItem )
    {
        if ( (sPrivItem->mPrivDesc.mWithGrant == STL_TRUE) &&
             (sPrivItem->mPrivDesc.mObjectID == sSpaceID) )
        {
            /**
             * 동일한 SPACE_ID 와 WITH GRANT OPTION 을 가지고 있음
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
 * @brief Current User 가 Revokee 에게 부여한 Revokable Prvilege for ALL PRIVILEGES ON TABLESPACE 목록을 획득한다.
 * @param[in]  aTransID                 Transaction ID
 * @param[in]  aStmt                    Statement
 * @param[in]  aSpaceHandle             Tablespace Handle
 * @param[in]  aRevokeeID               Revokee ID
 * @param[in]  aAccessControl           ACCESS CONTROL 권한 소유 여부 
 * @param[in]  aGrantOption             GRANT OPTION FOR
 * @param[out] aPrivList                Revokable Privilege List
 * @param[in]  aRegionMem               Region Memory
 * @param[in]  aEnv                     Execution Library Environment
 * @remarks
 */
stlStatus ellGetRevokableAllSpacePriv( smlTransId          aTransID,
                                       smlStatement      * aStmt,
                                       ellDictHandle     * aSpaceHandle,
                                       stlInt64            aRevokeeID,
                                       stlBool             aAccessControl,
                                       stlBool             aGrantOption,
                                       ellPrivList      ** aPrivList,
                                       knlRegionMem      * aRegionMem,
                                       ellEnv            * aEnv )
{
    ellDictHandle * sSessionUser = NULL;
    stlInt64        sSpaceID = ELL_DICT_TABLESPACE_ID_NA;
    stlInt64        sGrantorID = ELL_DICT_OBJECT_ID_NA;
    
    ellPrivList * sAllPrivList = NULL;
    ellPrivList * sRevokableList = NULL;
    ellPrivList * sFinalPrivList = NULL;

    ellPrivItem   * sPrivItem = NULL;

    stlBool   sDuplicate = STL_FALSE;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSpaceHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPrivList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */

    sSessionUser = ellGetSessionUserHandle( aEnv );
    sGrantorID = ellGetAuthID( sSessionUser );
    sSpaceID = ellGetTablespaceID( aSpaceHandle );
    
    /**
     * Revokee 의 모든 Privilege 획득
     */

    STL_TRY( ellInitPrivList( & sAllPrivList,
                              STL_TRUE,  /* aForRevoke, 모든 Privilege 목록을 얻기 위함 */
                              aRegionMem,
                              aEnv )
             == STL_SUCCESS );

    STL_TRY( eldGetPrivList( aTransID,
                             aStmt,
                             ELL_PRIV_SPACE,
                             ELDT_SpacePriv_ColumnOrder_GRANTEE_ID,
                             aRevokeeID,
                             aRegionMem,
                             sAllPrivList,
                             aEnv )
             == STL_SUCCESS );

    /**
     * Grantor 와 Space ID 가 동일한 Privilege 만 중복 없이 추출
     */

    STL_TRY( ellInitPrivList( & sRevokableList,
                              STL_TRUE,  /* aForRevoke */
                              aRegionMem,
                              aEnv )
             == STL_SUCCESS );
    
    STL_RING_FOREACH_ENTRY( & sAllPrivList->mHeadList, sPrivItem )
    {
        /**
         * Grantor 와 Space ID가 동일한 Privilege 를 추출
         */

        if ( sPrivItem->mPrivDesc.mObjectID == sSpaceID )
        {
            if ( (sPrivItem->mPrivDesc.mGrantorID == sGrantorID) ||
                 (aAccessControl == STL_TRUE ) )
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

    /**
     * GRANT OPTION FOR 인 경우 With Grant 만 추출 
     */

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




/** @} ellPrivTablespace */
