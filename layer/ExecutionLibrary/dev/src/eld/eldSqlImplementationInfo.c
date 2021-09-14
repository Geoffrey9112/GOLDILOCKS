/*******************************************************************************
 * eldSqlImplementationInfo.c
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
 * @file eldSqlImplementationInfo.c
 * @brief SQL IMPLEMENTATION INFO Dictionary 관리 루틴 
 */

#include <ell.h>
#include <eldt.h>
#include <eldc.h>

#include <eldIntegrity.h>
#include <eldDictionary.h>
#include <eldSqlImplementationInfo.h>

/**
 * @addtogroup eldSqlImplementationInfo
 * @{
 */

/**
 * @brief SQL IMPLEMENTATION INFO Descriptor 를 Dictionary 에 추가한다.
 * @remarks
 * 함수 ellInsertSqlImplementationInfoDesc() 주석 참조 
 */
stlStatus eldInsertSqlImplementationInfoDesc( smlTransId     aTransID,
                                              smlStatement * aStmt,
                                              stlChar      * aID,
                                              stlChar      * aName,
                                              stlInt32       aIntegerValue,
                                              stlChar      * aCharacterValue,
                                              stlChar      * aComments,
                                              ellEnv       * aEnv )

{
    knlValueBlockList        * sDataValueList  = NULL;

    smlRid               sRowRid;
    smlScn               sRowScn;
    smlRowBlock          sRowBlock;
    eldMemMark           sMemMark;

    stlInt32   sState = 0;
    
    /*
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aName != NULL, ELL_ERROR_STACK(aEnv) );

    
    /**
     * - Data Value List와 Row Block을 위한 공간 할당 및 초기화
     */
    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_SQL_IMPLEMENTATION_INFO,
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
     * - SQL IMPLEMENTATION INFO descriptor의 정보를 구성한다.
     */

    /*
     * IMPLEMENTATION_INFO_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_SQL_IMPLEMENTATION_INFO,
                                ELDT_SqlImplementationInfo_ColumnOrder_IMPLEMENTATION_INFO_ID,
                                sDataValueList,
                                aID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * IMPLEMENTATION_INFO_NAME
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_SQL_IMPLEMENTATION_INFO,
                                ELDT_SqlImplementationInfo_ColumnOrder_IMPLEMENTATION_INFO_NAME,
                                sDataValueList,
                                aName,
                                aEnv )
             == STL_SUCCESS );
    
    /*
     * INTEGER_VALUE
     */

    if( aIntegerValue == STL_INT32_MIN )
    {
        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_SQL_IMPLEMENTATION_INFO,
                                    ELDT_SqlImplementationInfo_ColumnOrder_INTEGER_VALUE,
                                    sDataValueList,
                                    NULL,
                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_SQL_IMPLEMENTATION_INFO,
                                    ELDT_SqlImplementationInfo_ColumnOrder_INTEGER_VALUE,
                                    sDataValueList,
                                    & aIntegerValue,
                                    aEnv )
                 == STL_SUCCESS );
    }

    /*
     * CHARACTER_VALUE
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_SQL_IMPLEMENTATION_INFO,
                                ELDT_SqlImplementationInfo_ColumnOrder_CHARACTER_VALUE,
                                sDataValueList,
                                aCharacterValue,
                                aEnv )
             == STL_SUCCESS );

    /*
     * COMMENTS
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_SQL_IMPLEMENTATION_INFO,
                                ELDT_SqlImplementationInfo_ColumnOrder_COMMENTS,
                                sDataValueList,
                                aComments,
                                aEnv )
             == STL_SUCCESS );

    
    /**
     * - 레코드를 넣는다.
     */

    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sDataValueList, 0, 1 );
    
    STL_TRY( smlInsertRecord( aStmt,
                              gEldTablePhysicalHandle[ELDT_TABLE_ID_SQL_IMPLEMENTATION_INFO],
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
                                              ELDT_TABLE_ID_SQL_IMPLEMENTATION_INFO,
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


/** @} eldSqlImplementationInfo */
