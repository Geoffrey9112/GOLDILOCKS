/*******************************************************************************
 * eldAuthorization.c
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
 * @file eldAuthorization.c
 * @brief Authorization Dictionary 관리 루틴 
 */

#include <ell.h>
#include <eldt.h>
#include <eldc.h>

#include <eldIntegrity.h>
#include <eldDictionary.h>
#include <eldAuthorization.h>

/**
 * @addtogroup eldAuthorization
 * @{
 */

/**
 * @brief Authorization Descriptor 를 Dictionary 에 추가한다.
 * @remarks
 * 함수 ellInsertAuthDesc() 주석 참조 
 */

stlStatus eldInsertAuthDesc( smlTransId             aTransID,
                             smlStatement         * aStmt,
                             stlInt64             * aAuthID,
                             stlChar              * aAuthName,
                             ellAuthorizationType   aAuthType,
                             stlChar              * aAuthComment,
                             ellEnv               * aEnv )
{
    ellSessionEnv * sSessEnv = ELL_SESS_ENV(aEnv);
    
    stlInt64   sAuthID = 0;
    
    knlValueBlockList        * sDataValueList  = NULL;

    smlRid      sRowRid;
    smlScn      sRowScn;
    smlRowBlock sRowBlock;
    eldMemMark  sMemMark;

    stlInt32 sState = 0;
    
    /*
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAuthID != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAuthName != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aAuthType > ELL_AUTHORIZATION_TYPE_NA) &&
                        (aAuthType < ELL_AUTHORIZATION_TYPE_MAX),
                        ELL_ERROR_STACK(aEnv) );

    
    /**
     * - Data Value List와 Row Block을 위한 공간 할당 및 초기화
     */
    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_AUTHORIZATIONS,
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
     * - authorization descriptor의 정보를 구성한다.
     */

    /*
     * AUTH_ID
     */

    if ( gEllIsMetaBuilding == STL_TRUE )
    {
        /* Database Dictionary 를 초기화하는 단계 */
        gEldtIdentityValue[ELDT_TABLE_ID_AUTHORIZATIONS] += 1;
        sAuthID = gEldtIdentityValue[ELDT_TABLE_ID_AUTHORIZATIONS];
    }
    else
    {
        /* Database가 이미 구축되어 있는 단계 */
        
        /**
         * Identity Column으로부터 Authorization ID를 획득
         */
        STL_TRY( smlGetNextSequenceVal( gEldIdentityColumnHandle[ELDT_TABLE_ID_AUTHORIZATIONS],
                                        & sAuthID,
                                        SML_ENV( aEnv ) )
                 == STL_SUCCESS );
    }
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_AUTHORIZATIONS,
                                ELDT_Auth_ColumnOrder_AUTH_ID,
                                sDataValueList,
                                & sAuthID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * AUTHORIZATION_NAME
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_AUTHORIZATIONS,
                                ELDT_Auth_ColumnOrder_AUTHORIZATION_NAME,
                                sDataValueList,
                                aAuthName,
                                aEnv )
             == STL_SUCCESS );

    /*
     * AUTHORIZATION_TYPE
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_AUTHORIZATIONS,
                                ELDT_Auth_ColumnOrder_AUTHORIZATION_TYPE,
                                sDataValueList,
                                (void *) gEllAuthorizationTypeString[aAuthType],
                                aEnv )
             == STL_SUCCESS );

    /*
     * AUTHORIZATION_TYPE_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_AUTHORIZATIONS,
                                ELDT_Auth_ColumnOrder_AUTHORIZATION_TYPE_ID,
                                sDataValueList,
                                (void *) & aAuthType,
                                aEnv )
             == STL_SUCCESS );

    /**
     * IS_BUILTIN
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_AUTHORIZATIONS,
                                ELDT_Auth_ColumnOrder_IS_BUILTIN,
                                sDataValueList,
                                & gEllIsMetaBuilding,
                                aEnv )
             == STL_SUCCESS );
    
    
    /*
     * CREATED_TIME
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_AUTHORIZATIONS,
                                ELDT_Auth_ColumnOrder_CREATED_TIME,
                                sDataValueList,
                                & sSessEnv->mTimeDDL,
                                aEnv )
             == STL_SUCCESS );

    /*
     * MODIFIED_TIME
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_AUTHORIZATIONS,
                                ELDT_Auth_ColumnOrder_MODIFIED_TIME,
                                sDataValueList,
                                & sSessEnv->mTimeDDL,
                                aEnv )
             == STL_SUCCESS );

    /*
     * COMMENTS
     */
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_AUTHORIZATIONS,
                                ELDT_Auth_ColumnOrder_COMMENTS,
                                sDataValueList,
                                aAuthComment,
                                aEnv )
             == STL_SUCCESS );
    
    /**
     * - 레코드를 넣는다.
     */

    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sDataValueList, 0, 1 );

    STL_TRY( smlInsertRecord( aStmt,
                              gEldTablePhysicalHandle[ELDT_TABLE_ID_AUTHORIZATIONS],
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
                                              ELDT_TABLE_ID_AUTHORIZATIONS,
                                              & sRowBlock,
                                              sDataValueList,
                                              aEnv )
                 == STL_SUCCESS );
    }

    /**
     * 사용한 메모리를 해제한다.
     */

    sState = 0;
    STL_TRY( eldFreeTableValueList( & sMemMark, aEnv ) == STL_SUCCESS );
    
    /**
     * Output 설정
     */
    
    *aAuthID = sAuthID;
    
    return STL_SUCCESS;

    STL_FINISH;

    switch (sState)
    {
        case 1:
            (void) eldFreeTableValueList( & sMemMark, aEnv );
        default:
            break;
    }
    
    return STL_FAILURE;
}


/**
 * @brief SQL-Authorization Cache 를 추가한다.
 * @param[in]  aTransID       Transaction ID
 * @param[in]  aStmt          Statement
 * @param[in]  aAuthID        Authorization ID
 * @param[in]  aEnv           Envirionment 
 * @remarks
 */
stlStatus eldRefineCache4AddAuth( smlTransId           aTransID,
                                  smlStatement       * aStmt,
                                  stlInt64             aAuthID,
                                  ellEnv             * aEnv )
{
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAuthID > ELL_DICT_OBJECT_ID_NA,
                        ELL_ERROR_STACK(aEnv) );

    /**
     * SQL-Authorization Cache 를 추가
     */

    STL_TRY( eldcInsertCacheAuthByAuthID( aTransID,
                                          aStmt,
                                          aAuthID,
                                          aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}








/**
 * @brief Auth ID 를 이용해 Auth Dictionary Handle 을 획득
 * @param[in]  aTransID      Transaction ID
 * @param[in]  aViewSCN      View SCN
 * @param[in]  aAuthID       Authorization ID
 * @param[out] aAuthHandle   Authorization Dictionary Handle
 * @param[out] aExist        존재 여부
 * @param[in]  aEnv          Environment
 * @remarks
 */
stlStatus eldGetAuthHandleByID( smlTransId           aTransID,
                                smlScn               aViewSCN,
                                stlInt64             aAuthID,
                                ellDictHandle      * aAuthHandle,
                                stlBool            * aExist,
                                ellEnv             * aEnv )
{
    return eldcGetAuthHandleByID( aTransID,
                                  aViewSCN,
                                  aAuthID,
                                  aAuthHandle,
                                  aExist,
                                  aEnv );
}

/**
 * @brief Auth Name 를 이용해 Auth Dictionary Handle 을 획득
 * @param[in]  aTransID      Transaction ID
 * @param[in]  aViewSCN      View SCN
 * @param[in]  aAuthName     Authorization Name
 * @param[out] aAuthHandle   Authorization Dictionary Handle
 * @param[out] aExist        존재 여부
 * @param[in]  aEnv          Environment
 * @remarks
 */
stlStatus eldGetAuthHandleByName( smlTransId           aTransID,
                                  smlScn               aViewSCN,
                                  stlChar            * aAuthName,
                                  ellDictHandle      * aAuthHandle,
                                  stlBool            * aExist,
                                  ellEnv             * aEnv )
{
    return eldcGetAuthHandleByName( aTransID,
                                    aViewSCN,
                                    aAuthName,
                                    aAuthHandle,
                                    aExist,
                                    aEnv );
}




/** @} eldAuthorization */
