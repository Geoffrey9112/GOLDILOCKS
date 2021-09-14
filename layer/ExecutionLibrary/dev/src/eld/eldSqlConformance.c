/*******************************************************************************
 * eldSqlConformance.c
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
 * @file eldSqlConformance.c
 * @brief SQL CONFORMANCE Dictionary 관리 루틴 
 */

#include <ell.h>
#include <eldt.h>
#include <eldc.h>

#include <eldIntegrity.h>
#include <eldDictionary.h>
#include <eldSqlConformance.h>

/**
 * @addtogroup eldSqlConformance
 * @{
 */

/**
 * @brief SQL CONFORMANCE Descriptor 를 Dictionary 에 추가한다.
 * @remarks
 * 함수 ellInsertSqlConformanceDesc() 주석 참조 
 */
stlStatus eldInsertSqlConformanceDesc( smlTransId     aTransID,
                                       smlStatement * aStmt,
                                       stlChar      * aType,
                                       stlChar      * aID,
                                       stlChar      * aSubID,
                                       stlChar      * aName,
                                       stlChar      * aSubName,
                                       stlBool        aIsSupported,
                                       stlBool        aIsMandatory,
                                       stlChar      * aIsVerifiedBy,
                                       stlChar      * aComments,
                                       ellEnv       * aEnv )

{
    knlValueBlockList        * sDataValueList  = NULL;

    smlRid               sRowRid;
    smlScn               sRowScn;
    smlRowBlock          sRowBlock;
    eldMemMark           sMemMark;

    stlInt32  sState = 0;
    
    /*
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aType != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aName != NULL, ELL_ERROR_STACK(aEnv) );

    
    /**
     * - Data Value List와 Row Block을 위한 공간 할당 및 초기화
     */
    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_SQL_CONFORMANCE,
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
     * - SQL CONFORMANCE descriptor의 정보를 구성한다.
     */

    /*
     * TYPE
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_SQL_CONFORMANCE,
                                ELDT_SqlConformance_ColumnOrder_TYPE,
                                sDataValueList,
                                aType,
                                aEnv )
             == STL_SUCCESS );

    /*
     * ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_SQL_CONFORMANCE,
                                ELDT_SqlConformance_ColumnOrder_ID,
                                sDataValueList,
                                aID,
                                aEnv )
             == STL_SUCCESS );
    
    /*
     * SUB_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_SQL_CONFORMANCE,
                                ELDT_SqlConformance_ColumnOrder_SUB_ID,
                                sDataValueList,
                                aSubID,
                                aEnv )
             == STL_SUCCESS );
    
    /*
     * NAME
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_SQL_CONFORMANCE,
                                ELDT_SqlConformance_ColumnOrder_NAME,
                                sDataValueList,
                                aName,
                                aEnv )
             == STL_SUCCESS );
    
    /*
     * SUB_NAME
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_SQL_CONFORMANCE,
                                ELDT_SqlConformance_ColumnOrder_SUB_NAME,
                                sDataValueList,
                                aSubName,
                                aEnv )
             == STL_SUCCESS );
    
    /*
     * IS_SUPPORTED
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_SQL_CONFORMANCE,
                                ELDT_SqlConformance_ColumnOrder_IS_SUPPORTED,
                                sDataValueList,
                                & aIsSupported,
                                aEnv )
             == STL_SUCCESS );
    
    /*
     * IS_MANDATORY
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_SQL_CONFORMANCE,
                                ELDT_SqlConformance_ColumnOrder_IS_MANDATORY,
                                sDataValueList,
                                & aIsMandatory,
                                aEnv )
             == STL_SUCCESS );
    

    /*
     * IS_VERTIFIED_BY
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_SQL_CONFORMANCE,
                                ELDT_SqlConformance_ColumnOrder_IS_VERIFIED_BY,
                                sDataValueList,
                                aIsVerifiedBy,
                                aEnv )
             == STL_SUCCESS );
    
    /*
     * COMMENTS
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_SQL_CONFORMANCE,
                                ELDT_SqlConformance_ColumnOrder_COMMENTS,
                                sDataValueList,
                                aComments,
                                aEnv )
             == STL_SUCCESS );

    
    /**
     * - 레코드를 넣는다.
     */

    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sDataValueList, 0, 1 );
    
    STL_TRY( smlInsertRecord( aStmt,
                              gEldTablePhysicalHandle[ELDT_TABLE_ID_SQL_CONFORMANCE],
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
                                              ELDT_TABLE_ID_SQL_CONFORMANCE,
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


/** @} eldSqlConformance */
