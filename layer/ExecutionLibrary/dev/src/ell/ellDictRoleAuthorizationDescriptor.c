/*******************************************************************************
 * ellDictRoleAuthorizationDescriptor.c
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
 * @file ellDictRoleAuthorizationDescriptor.c
 * @brief RoleAuthorizationDescriptor Dictionary 관리 모듈
 */

#include <ell.h>
#include <elDef.h>

#include <eldt.h>
#include <eldc.h>
#include <eldDictionary.h>

#include <eldRoleAuthorizationDescriptor.h>


/**
 * @addtogroup ellObjectAuth
 * @{
 */

/**
 * @brief GRANT ROLE을 위한 Role Authorization Descriptor 를 Dictionary 에 추가한다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aRoleID          Role ID
 * @param[in]  aGranteeID       Grantee ID
 * @param[in]  aGrantorID       Grantor ID
 * @param[in]  aIsGrantable     GRANT 가능 여부 
 * @param[in]  aEnv             Execution Library Environment
 * @remarks
 * GRANT ROLE 구문을 수행한 Role Authorization Descriptor 는 다음과 같이 구성된다.
 * - orderer> GRANT ROLE role_id TO user_id
 *  - Grantor : orderer
 *  - Grantee : user_id
 *  - Role    : role_id
 */

stlStatus ellInsertRoleAuthDescForGrantRole( smlTransId             aTransID,
                                             smlStatement         * aStmt,
                                             stlInt64               aRoleID,
                                             stlInt64               aGranteeID,
                                             stlInt64               aGrantorID,
                                             stlBool                aIsGrantable,
                                             ellEnv               * aEnv )
{
    STL_TRY( eldInsertRoleAuthDesc( aTransID,
                                    aStmt,
                                    aRoleID,
                                    aGranteeID,
                                    aGrantorID,
                                    aIsGrantable,
                                    aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}

/**
 * @brief CREATE ROLE을 위한 Role Authorization Descriptor 를 Dictionary 에 추가한다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aRoleID          Role ID
 * @param[in]  aUserID          CREATE ROLE 구문을 수행한 User의 ID
 * @param[in]  aIsGrantable     GRANT 가능 여부 
 * @param[in]  aEnv             Execution Library Environment
 * @remarks
 * CREATE ROLE 에 대한 Grantor 와 Grantee 는 다음과 같이 설정된다.
 * - orderer> CREATE ROLE role_id
 *  - Grantor : _SYSTEM
 *  - Grantee : orderer
 *  - Role    : role_id
 */

stlStatus ellInsertRoleAuthDescForCreateRole( smlTransId             aTransID,
                                              smlStatement         * aStmt,
                                              stlInt64               aRoleID,
                                              stlInt64               aUserID,
                                              stlBool                aIsGrantable,
                                              ellEnv               * aEnv )
{
    STL_TRY( eldInsertRoleAuthDesc( aTransID,
                                    aStmt,
                                    aRoleID,
                                    aUserID,
                                    ELDT_AUTH_ID_SYSTEM,
                                    aIsGrantable,
                                    aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}

/** @} ellObjectAuth */
