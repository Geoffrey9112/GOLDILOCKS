/*******************************************************************************
 * eldUserSchemaPath.c
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
 * @file eldUserSchemaPath.c
 * @brief User Schema Path Dictionary 관리 루틴 
 */

#include <ell.h>
#include <eldt.h>
#include <eldc.h>

#include <eldIntegrity.h>
#include <eldDictionary.h>
#include <eldUserSchemaPath.h>

/**
 * @addtogroup eldUserSchemaPath
 * @{
 */

/**
 * @brief User Schema Path Descriptor 를 Dictionary 에 추가한다.
 * @remarks
 * 함수 ellInsertUserSchemaPathDesc() 주석 참조 
 */
stlStatus eldInsertUserSchemaPathDesc( smlTransId             aTransID,
                                       smlStatement         * aStmt,
                                       stlInt64               aAuthID,
                                       stlInt64               aSchemaID,
                                       stlInt32               aSearchOrder,
                                       ellEnv               * aEnv )

{
    stlInt32    sOrder = 0;
    
    knlValueBlockList        * sDataValueList  = NULL;

    smlRid                 sRowRid;
    smlScn                 sRowScn;
    smlRowBlock            sRowBlock;
    eldMemMark             sMemMark;

    stlInt32  sState = 0;
    
    /*
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAuthID > 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSchemaID > 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSearchOrder >= 0, ELL_ERROR_STACK(aEnv) );
    
    /**
     * - Data Value List와 Row Block을 위한 공간 할당 및 초기화
     */
    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_USER_SCHEMA_PATH,
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
     * - user schema path descriptor의 정보를 구성한다.
     */

    /*
     * AUTH_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_USER_SCHEMA_PATH,
                                ELDT_UserPath_ColumnOrder_AUTH_ID,
                                sDataValueList,
                                & aAuthID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * SCHEMA_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_USER_SCHEMA_PATH,
                                ELDT_UserPath_ColumnOrder_SCHEMA_ID,
                                sDataValueList,
                                & aSchemaID,
                                aEnv )
             == STL_SUCCESS );
    
    /*
     * SEARCH_ORDER
     */

    sOrder = ELD_ORDINAL_POSITION_TO_DATABASE(aSearchOrder);
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_USER_SCHEMA_PATH,
                                ELDT_UserPath_ColumnOrder_SEARCH_ORDER,
                                sDataValueList,
                                & sOrder,
                                aEnv )
             == STL_SUCCESS );
    
    /**
     * - 레코드를 넣는다.
     */

    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sDataValueList, 0, 1 );
    
    STL_TRY( smlInsertRecord( aStmt,
                              gEldTablePhysicalHandle[ELDT_TABLE_ID_USER_SCHEMA_PATH],
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
                                              ELDT_TABLE_ID_USER_SCHEMA_PATH,
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


/** @} eldUserSchemaPath */
