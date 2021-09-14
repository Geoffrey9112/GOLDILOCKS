/*******************************************************************************
 * eldTablespace.c
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
 * @file eldTablespace.c
 * @brief Tablespace Dictionary 관리 루틴 
 */

#include <ell.h>
#include <eldt.h>
#include <eldc.h>

#include <eldIntegrity.h>
#include <eldDictionary.h>
#include <eldTablespace.h>

/**
 * @addtogroup eldTablespace
 * @{
 */

/**
 * @brief Tablespace Descriptor 를 Dictionary 에 추가한다.
 * @remarks
 * 함수 ellInsertTablespaceDesc() 주석 참조 
 */

stlStatus eldInsertTablespaceDesc( smlTransId               aTransID,
                                   smlStatement           * aStmt,
                                   stlInt64                 aTablespaceID,
                                   stlInt64                 aCreatorID,
                                   stlChar                * aTablespaceName,
                                   ellTablespaceMediaType   aMediaType,
                                   ellTablespaceUsageType   aUsageType,
                                   stlChar                * aTablespaceComment,
                                   ellEnv                 * aEnv )
{
    ellSessionEnv * sSessEnv = ELL_SESS_ENV(aEnv);

    knlValueBlockList         * sDataValueList  = NULL;

    smlRid               sRowRid;
    smlScn               sRowScn;
    smlRowBlock          sRowBlock;
    eldMemMark           sMemMark;

    stlInt32  sState = 0;
    
    /*
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTablespaceID >= 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCreatorID > 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTablespaceName != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aMediaType > ELL_SPACE_MEDIA_TYPE_NA) &&
                        (aMediaType < ELL_SPACE_MEDIA_TYPE_MAX),
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aUsageType > ELL_SPACE_USAGE_TYPE_NA) &&
                        (aUsageType < ELL_SPACE_USAGE_TYPE_MAX),
                        ELL_ERROR_STACK(aEnv) );

    
    /**
     * - Data Value List와 Row Block을 위한 공간 할당 및 초기화
     */
    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_TABLESPACES,
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
     * - tablespace descriptor의 정보를 구성한다.
     */

    /*
     * TABLESPACE_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLESPACES,
                                ELDT_Space_ColumnOrder_TABLESPACE_ID,
                                sDataValueList,
                                & aTablespaceID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * CREATOR_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLESPACES,
                                ELDT_Space_ColumnOrder_CREATOR_ID,
                                sDataValueList,
                                & aCreatorID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * TABLESPACE_NAME
     */
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLESPACES,
                                ELDT_Space_ColumnOrder_TABLESPACE_NAME,
                                sDataValueList,
                                aTablespaceName,
                                aEnv )
             == STL_SUCCESS );

    /*
     * MEDIA_TYPE
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLESPACES,
                                ELDT_Space_ColumnOrder_MEDIA_TYPE,
                                sDataValueList,
                                (void *) gEllTablespaceMediaTypeString[aMediaType],
                                aEnv )
             == STL_SUCCESS );

    /*
     * MEDIA_TYPE_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLESPACES,
                                ELDT_Space_ColumnOrder_MEDIA_TYPE_ID,
                                sDataValueList,
                                & aMediaType,
                                aEnv )
             == STL_SUCCESS );

    /*
     * USAGE_TYPE
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLESPACES,
                                ELDT_Space_ColumnOrder_USAGE_TYPE,
                                sDataValueList,
                                (void *) gEllTablespaceUsageTypeString[aUsageType],
                                aEnv )
             == STL_SUCCESS );

    /*
     * USAGE_TYPE_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLESPACES,
                                ELDT_Space_ColumnOrder_USAGE_TYPE_ID,
                                sDataValueList,
                                & aUsageType,
                                aEnv )
             == STL_SUCCESS );

    /**
     * IS_BUILTIN
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLESPACES,
                                ELDT_Space_ColumnOrder_IS_BUILTIN,
                                sDataValueList,
                                & gEllIsMetaBuilding,
                                aEnv )
             == STL_SUCCESS );
    
    /*
     * CREATED_TIME
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLESPACES,
                                ELDT_Space_ColumnOrder_CREATED_TIME,
                                sDataValueList,
                                & sSessEnv->mTimeDDL,
                                aEnv )
             == STL_SUCCESS );

    /*
     * MODIFIED_TIME
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLESPACES,
                                ELDT_Space_ColumnOrder_MODIFIED_TIME,
                                sDataValueList,
                                & sSessEnv->mTimeDDL,
                                aEnv )
             == STL_SUCCESS );

    /*
     * COMMENTS
     */
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_TABLESPACES,
                                ELDT_Space_ColumnOrder_COMMENTS,
                                sDataValueList,
                                aTablespaceComment,
                                aEnv )
             == STL_SUCCESS );
    
    /**
     * - 레코드를 넣는다.
     */

    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sDataValueList, 0, 1 );
    
    STL_TRY( smlInsertRecord( aStmt,
                              gEldTablePhysicalHandle[ELDT_TABLE_ID_TABLESPACES],
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
                                              ELDT_TABLE_ID_TABLESPACES,
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
 * @brief Tablespace Dictionary 정보만을 삭제한다.
 * @param[in] aTransID         Transaction ID
 * @param[in] aStmt            Statement
 * @param[in] aSpaceHandle     Tablespace Dictionary Handle
 * @param[in] aEnv             Environment
 * @remarks
 */
stlStatus eldDeleteDictSpace( smlTransId       aTransID,
                              smlStatement   * aStmt,
                              ellDictHandle  * aSpaceHandle,
                              ellEnv         * aEnv )
{
    stlInt64 sSpaceID = ELL_DICT_TABLESPACE_ID_NA;

    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSpaceHandle != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Tablespace ID 획득
     */
    
    sSpaceID = ellGetTablespaceID( aSpaceHandle );

    /**
     * Tablespace Privilege Record 를 삭제
     */
    
    /*
     * DELETE FROM DEFINITION_SCHEMA.TABLESPACE_PRIVILEGES
     *  WHERE TABLESPACE_ID = sSpaceID;
     */
    
    STL_TRY( eldDeletePrivRowsAndCache( aTransID,
                                        aStmt,
                                        ELDT_TABLE_ID_TABLESPACE_PRIVILEGES,
                                        ELDT_SpacePriv_ColumnOrder_TABLESPACE_ID,
                                        sSpaceID,
                                        aEnv )
             == STL_SUCCESS );
    
    /**
     * Tablespace 관련 Dictionary Record 정보를 삭제
     */
    
    /*
     * DELETE FROM DEFINITION_SCHEMA.TABLESPACES
     *  WHERE TABLESPACE_ID = sSpaceID;
     */
    
    STL_TRY( eldDeleteObjectRows( aTransID,
                                  aStmt,
                                  ELDT_TABLE_ID_TABLESPACES,
                                  ELDT_Space_ColumnOrder_TABLESPACE_ID,
                                  sSpaceID,
                                  aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}




/**
 * @brief SQL-Tablespace Cache 를 추가한다.
 * @param[in]  aTransID       Transaction ID
 * @param[in]  aStmt          Statement
 * @param[in]  aAuthID        Authorization ID
 * @param[in]  aTablespaceID  Tablespace ID
 * @param[in]  aEnv           Environment
 * @remarks
 */
stlStatus eldRefineCache4AddTablespace( smlTransId           aTransID,
                                        smlStatement       * aStmt,
                                        stlInt64             aAuthID,
                                        stlInt64             aTablespaceID,
                                        ellEnv             * aEnv )
{


    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAuthID > ELL_DICT_OBJECT_ID_NA,
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTablespaceID > ELL_DICT_TABLESPACE_ID_NA,
                        ELL_ERROR_STACK(aEnv) );


    
    /**
     * SQL-Tablespace Cache 를 추가 
     */

    STL_TRY( eldcInsertCacheTablespaceByTablespaceID( aTransID,
                                                      aStmt,
                                                      aTablespaceID,
                                                      aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Tablespace ID 를 이용해 Tablespace Handle 을 얻는다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aViewSCN         View SCN
 * @param[in]  aTablespaceID    Tablespace ID
 * @param[out] aSpaceDictHandle Tablespace Dictionary Handle
 * @param[out] aExist           존재 여부
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus eldGetTablespaceHandleByID( smlTransId           aTransID,
                                      smlScn               aViewSCN,
                                      stlInt64             aTablespaceID,
                                      ellDictHandle      * aSpaceDictHandle,
                                      stlBool            * aExist,
                                      ellEnv             * aEnv )
{
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aTablespaceID > ELL_DICT_TABLESPACE_ID_NA,
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSpaceDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExist != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Tablespace ID 에 대한 Handle 검색
     */

    STL_TRY( eldcGetTablespaceHandleByID( aTransID,
                                          aViewSCN,
                                          aTablespaceID,
                                          aSpaceDictHandle,
                                          aExist,
                                          aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Tablespace Name 를 이용해 Tablespace Handle 을 얻는다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aViewSCN         View SCN
 * @param[in]  aTablespaceName  Tablespace Name
 * @param[out] aSpaceDictHandle Tablespace Dictionary Handle
 * @param[out] aExist           존재 여부
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus eldGetTablespaceHandleByName( smlTransId           aTransID,
                                        smlScn               aViewSCN,
                                        stlChar            * aTablespaceName,
                                        ellDictHandle      * aSpaceDictHandle,
                                        stlBool            * aExist,
                                        ellEnv             * aEnv )
{
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aTablespaceName != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSpaceDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExist != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Tablespace Name 에 대한 handle 검색
     */
        
    STL_TRY( eldcGetTablespaceHandleByName( aTransID,
                                            aViewSCN,
                                            aTablespaceName,
                                            aSpaceDictHandle,
                                            aExist,
                                            aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/** @} eldTablespace */
