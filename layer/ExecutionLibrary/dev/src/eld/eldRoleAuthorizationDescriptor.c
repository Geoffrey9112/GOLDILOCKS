/*******************************************************************************
 * eldRoleAuthorizationDescriptor.c
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
 * @file eldRoleAuthorizationDescriptor.c
 * @brief Role Authorization Descriptor Dictionary 관리 루틴 
 */

#include <ell.h>
#include <eldt.h>
#include <eldc.h>

#include <eldIntegrity.h>
#include <eldDictionary.h>
#include <eldRoleAuthorizationDescriptor.h>

/**
 * @addtogroup eldRoleAuthorizationDescriptor
 * @{
 */

/**
 * @brief Role Authorization Descriptor 를 Dictionary 에 추가한다.
 * @remarks
 * 함수 ellInsertRoleAuthDesc() 주석 참조 
 */

stlStatus eldInsertRoleAuthDesc( smlTransId             aTransID,
                                 smlStatement         * aStmt,
                                 stlInt64               aRoleID,
                                 stlInt64               aGranteeID,
                                 stlInt64               aGrantorID,
                                 stlBool                aIsGrantable,
                                 ellEnv               * aEnv )
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
    STL_PARAM_VALIDATE( aRoleID > 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aGranteeID > 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aGrantorID > 0, ELL_ERROR_STACK(aEnv) );
    
    /**
     * - Data Value List와 Row Block을 위한 공간 할당 및 초기화
     */
    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_ROLE_AUTHORIZATION_DESCRIPTORS,
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
     * ROLE_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_ROLE_AUTHORIZATION_DESCRIPTORS,
                                ELDT_RoleDesc_ColumnOrder_ROLE_ID,
                                sDataValueList,
                                & aRoleID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * GRANTEE_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_ROLE_AUTHORIZATION_DESCRIPTORS,
                                ELDT_RoleDesc_ColumnOrder_GRANTEE_ID,
                                sDataValueList,
                                & aGranteeID,
                                aEnv )
             == STL_SUCCESS );
    
    /*
     * GRANTOR_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_ROLE_AUTHORIZATION_DESCRIPTORS,
                                ELDT_RoleDesc_ColumnOrder_GRANTOR_ID,
                                sDataValueList,
                                & aGrantorID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * IS_GRANTABLE
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_ROLE_AUTHORIZATION_DESCRIPTORS,
                                ELDT_RoleDesc_ColumnOrder_IS_GRANTABLE,
                                sDataValueList,
                                & aIsGrantable,
                                aEnv )
             == STL_SUCCESS );
    
    /**
     * - 레코드를 넣는다.
     */

    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sDataValueList, 0, 1 );
    
    STL_TRY( smlInsertRecord( aStmt,
                              gEldTablePhysicalHandle[ELDT_TABLE_ID_ROLE_AUTHORIZATION_DESCRIPTORS],
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
                                              ELDT_TABLE_ID_ROLE_AUTHORIZATION_DESCRIPTORS,
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


/** @} eldRoleAuthorizationDescriptor */
