/*******************************************************************************
 * ellDictRoleAuthorizationDescriptor.h
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


#ifndef _ELL_DICT_ROLE_AUTHORIZATION_DESCRIPTOR_H_
#define _ELL_DICT_ROLE_AUTHORIZATION_DESCRIPTOR_H_ 1

/**
 * @file ellDictRoleAuthorizationDescriptor.h
 * @brief Role Authorization Descriptor Dictionary Management
 */

/**
 * @addtogroup ellObjectAuth
 * @{
 */

stlStatus ellInsertRoleAuthDescForCreateRole( smlTransId             aTransID,
                                              smlStatement         * aStmt,
                                              stlInt64               aRoleID,
                                              stlInt64               aUserID,
                                              stlBool                aIsGrantable,
                                              ellEnv               * aEnv );

stlStatus ellInsertRoleAuthDescForGrantRole( smlTransId             aTransID,
                                             smlStatement         * aStmt,
                                             stlInt64               aRoleID,
                                             stlInt64               aGranteeID,
                                             stlInt64               aGrantorID,
                                             stlBool                aIsGrantable,
                                             ellEnv               * aEnv );


    
/** @} ellObjectAuth */



#endif /* _ELL_DICT_ROLE_AUTHORIZATION_DESCRIPTOR_H_ */
