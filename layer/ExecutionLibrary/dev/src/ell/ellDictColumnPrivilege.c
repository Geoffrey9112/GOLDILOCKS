/*******************************************************************************
 * ellDictColumnPrivilege.c
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
 * @file ellDictColumnPrivilege.c
 * @brief Column Privilege Dictionary 관리 모듈
 */

#include <ell.h>
#include <elDef.h>

#include <eldt.h>
#include <eldc.h>
#include <eldDictionary.h>
#include <eldIntegrity.h>


/**
 * @addtogroup ellPrivColumn
 * @{
 */

/**
 * @brief Column Privilge 의 String 상수
 */
const stlChar * const gEllColumnPrivActionString[ELL_COLUMN_PRIV_ACTION_MAX] =
{
    "N/A",         /**< ELL_COLUMN_PRIV_ACTION_NOT_AVAILABLE */
    
    "SELECT",      /**< ELL_COLUMN_PRIV_ACTION_SELECT */
    "INSERT",      /**< ELL_COLUMN_PRIV_ACTION_INSERT */
    "UPDATE",      /**< ELL_COLUMN_PRIV_ACTION_UPDATE */
    "REFERENCES"   /**< ELL_COLUMN_PRIV_ACTION_REFERENCES */
};

/**
 * @brief Column Privilege Descriptor 를 Dictionary 에 추가한다.
 * @param[in]  aTransID                 Transaction ID
 * @param[in]  aStmt                    Statement
 * @param[in]  aGrantorID               Grantor ID
 * @param[in]  aGranteeID               Grantee ID
 * @param[in]  aSchemaID                Column 의 Schema ID
 * @param[in]  aTableID                 Column 의 Table ID
 * @param[in]  aColumnID                Column ID
 * @param[in]  aColumnPrivAction        Column Privilege Action
 * @param[in]  aIsGrantable             GRANT 가능 여부 
 * @param[in]  aEnv                     Execution Library Environment
 * @remarks
 */

stlStatus ellInsertColumnPrivDesc( smlTransId            aTransID,
                                   smlStatement        * aStmt,
                                   stlInt64              aGrantorID,
                                   stlInt64              aGranteeID,
                                   stlInt64              aSchemaID,
                                   stlInt64              aTableID,
                                   stlInt64              aColumnID,
                                   ellColumnPrivAction   aColumnPrivAction,
                                   stlBool               aIsGrantable,
                                   ellEnv              * aEnv )
{
    knlValueBlockList        * sDataValueList  = NULL;

    smlRid               sRowRid;
    smlScn               sRowScn;
    smlRowBlock          sRowBlock;
    eldMemMark           sMemMark;

    stlInt32 sState = 0;
    
    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aGranteeID > 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aGrantorID > 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSchemaID > 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableID > 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColumnID > 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aColumnPrivAction > ELL_COLUMN_PRIV_ACTION_NA) &&
                        (aColumnPrivAction < ELL_COLUMN_PRIV_ACTION_MAX),
                        ELL_ERROR_STACK(aEnv) );

    /**************************************************************
     * Privilege Record 추가
     **************************************************************/
    
    /**
     * - Data Value List와 Row Block을 위한 공간 할당 및 초기화
     */
    
    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_COLUMN_PRIVILEGES,
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

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMN_PRIVILEGES,
                                ELDT_ColumnPriv_ColumnOrder_GRANTOR_ID,
                                sDataValueList,
                                & aGrantorID,
                                aEnv )
             == STL_SUCCESS );
    
    /*
     * GRANTEE_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMN_PRIVILEGES,
                                ELDT_ColumnPriv_ColumnOrder_GRANTEE_ID,
                                sDataValueList,
                                & aGranteeID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * SCHEMA_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMN_PRIVILEGES,
                                ELDT_ColumnPriv_ColumnOrder_SCHEMA_ID,
                                sDataValueList,
                                & aSchemaID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * TABLE_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMN_PRIVILEGES,
                                ELDT_ColumnPriv_ColumnOrder_TABLE_ID,
                                sDataValueList,
                                & aTableID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * COLUMN_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMN_PRIVILEGES,
                                ELDT_ColumnPriv_ColumnOrder_COLUMN_ID,
                                sDataValueList,
                                & aColumnID,
                                aEnv )
             == STL_SUCCESS );
    
    /*
     * PRIVILEGE_TYPE
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMN_PRIVILEGES,
                                ELDT_ColumnPriv_ColumnOrder_PRIVILEGE_TYPE,
                                sDataValueList,
                                (void *) gEllColumnPrivActionString[aColumnPrivAction],
                                aEnv )
             == STL_SUCCESS );

    /*
     * PRIVILEGE_TYPE_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMN_PRIVILEGES,
                                ELDT_ColumnPriv_ColumnOrder_PRIVILEGE_TYPE_ID,
                                sDataValueList,
                                & aColumnPrivAction,
                                aEnv )
             == STL_SUCCESS );
    
    /*
     * IS_GRANTABLE
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMN_PRIVILEGES,
                                ELDT_ColumnPriv_ColumnOrder_IS_GRANTABLE,
                                sDataValueList,
                                & aIsGrantable,
                                aEnv )
             == STL_SUCCESS );

    /**
     * IS_BUILTIN
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_COLUMN_PRIVILEGES,
                                ELDT_ColumnPriv_ColumnOrder_IS_BUILTIN,
                                sDataValueList,
                                & gEllIsMetaBuilding,
                                aEnv )
             == STL_SUCCESS );
    
    
    /**
     * - 레코드를 넣는다.
     */

    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sDataValueList, 0, 1 );
    
    STL_TRY( smlInsertRecord( aStmt,
                              gEldTablePhysicalHandle[ELDT_TABLE_ID_COLUMN_PRIVILEGES],
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
                                              ELDT_TABLE_ID_COLUMN_PRIVILEGES,
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
                                  ELDC_PRIVCACHE_COLUMN,
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
 * @brief ACCESS CONTROL 권한을 통해 revokable privilege action 을 구한다.
 * @param[in]  aTransID                 Transaction ID
 * @param[in]  aStmt                    Statement
 * @param[in]  aColumnHandle            Column Handle
 * @param[in]  aRevokeeID               Revokee ID
 * @param[in]  aColumnPrivAction        column privilege action
 * @param[out] aPrivExist               Privilege 존재 여부
 * @param[out] aDuplicate               Duplicate Privilege 존재 여부 
 * @param[in,out] aPrivList             Revokable Privilege List
 * @param[in]  aRegionMem               Region Memory
 * @param[in]  aEnv                     Execution Library Environment
 */
stlStatus ellGetRevokableColumnPrivByAccessControl( smlTransId           aTransID,
                                                    smlStatement       * aStmt,
                                                    ellDictHandle      * aColumnHandle,
                                                    stlInt64             aRevokeeID,
                                                    ellColumnPrivAction  aColumnPrivAction,
                                                    stlBool            * aPrivExist,
                                                    stlBool            * aDuplicate,
                                                    ellPrivList        * aPrivList,
                                                    knlRegionMem       * aRegionMem,
                                                    ellEnv             * aEnv )
{
    ellDictHandle * sSessionUser = NULL;
    stlInt64        sColumnID = ELL_DICT_OBJECT_ID_NA;
    stlInt64        sGrantorID = ELL_DICT_OBJECT_ID_NA;
    
    ellPrivList  * sAllPrivList = NULL;
    ellPrivList  * sRevokableList = NULL;
    
    ellPrivItem   * sPrivItem = NULL;

    stlBool   sPrivExist = STL_FALSE;
    stlBool   sDuplicate = STL_FALSE;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColumnHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRevokeeID > ELL_DICT_OBJECT_ID_NA, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aColumnPrivAction > ELL_COLUMN_PRIV_ACTION_NA) &&
                        (aColumnPrivAction < ELL_COLUMN_PRIV_ACTION_MAX),
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPrivExist != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDuplicate != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPrivList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */

    sSessionUser = ellGetSessionUserHandle( aEnv );
    sGrantorID = ellGetAuthID( sSessionUser );
    sColumnID = ellGetColumnID( aColumnHandle );

    /*********************************************************
     * Column Privilege Action 이 동일한 모든 COLUMN PRIVILEGE 획득
     *********************************************************/
    
    /**
     * Column ID 가 동일한 모든 COLUMN PRIVILEGE 획득
     */

    STL_TRY( ellInitPrivList( & sAllPrivList,
                              STL_TRUE,  /* aForRevoke, 모든 Privilege 목록을 얻기 위함 */
                              aRegionMem,
                              aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldGetPrivList( aTransID,
                             aStmt,
                             ELL_PRIV_COLUMN,
                             ELDT_ColumnPriv_ColumnOrder_COLUMN_ID,
                             sColumnID,
                             aRegionMem,
                             sAllPrivList,
                             aEnv )
             == STL_SUCCESS );

    /**
     * Revoker, Revokee, Column Privilege Action 이 동일한 Privilege 만 중복 없이 추출
     */

    STL_TRY( ellInitPrivList( & sRevokableList,
                              STL_TRUE,  /* aForRevoke */
                              aRegionMem,
                              aEnv )
             == STL_SUCCESS );
    
    sPrivExist = STL_FALSE;
    *aDuplicate = STL_FALSE;
    
    STL_RING_FOREACH_ENTRY( & sAllPrivList->mHeadList, sPrivItem )
    {
        /**
         * Revoker 와 Revokee 가 동일한 Privilege 를 추출
         */

        if ( (sPrivItem->mPrivDesc.mGranteeID == aRevokeeID) &&
             (sPrivItem->mPrivDesc.mPrivAction.mColumnAction == aColumnPrivAction) )
        {
            if ( (sPrivItem->mPrivDesc.mGrantorID == sGrantorID) ||
                 (sPrivItem->mPrivDesc.mGrantorID != ELDT_AUTH_ID_SYSTEM) )
            {
                STL_TRY( ellAddNewPrivItem( sRevokableList,
                                            sPrivItem->mPrivDesc.mPrivObject,
                                            & sPrivItem->mPrivHandle,
                                            NULL,   /* aPrivDesc */
                                            & sDuplicate,
                                            aRegionMem,
                                            aEnv )
                         == STL_SUCCESS );

                sPrivExist = STL_TRUE;
                
                /**
                 * 중복이 존재하면 안됨
                 */
                
                STL_DASSERT( sDuplicate == STL_FALSE );
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
     * Revokable Privilege List 를 추가 
     *********************************************************/

    if ( ellHasPrivItem(sRevokableList) == STL_FALSE )
    {
        sPrivExist = STL_FALSE;
    }
    else
    {
        STL_RING_FOREACH_ENTRY( & sRevokableList->mHeadList, sPrivItem )
        {
            STL_TRY( ellAddNewPrivItem( aPrivList,
                                        sPrivItem->mPrivDesc.mPrivObject,
                                        & sPrivItem->mPrivHandle,
                                        NULL,   /* aPrivDesc */
                                        & sDuplicate,
                                        aRegionMem,
                                        aEnv )
                     == STL_SUCCESS );
            
            /**
             * 중복이 존재함
             */
            
            if ( sDuplicate == STL_TRUE )
            {
                *aDuplicate = sDuplicate;
            }
            else
            {
                /* 중복 여부를 누적함 */
            }
        }
    }
    
    /**
     * Output 설정
     */
    
    *aPrivExist = sPrivExist;
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}

/** @} ellPrivColumn */
