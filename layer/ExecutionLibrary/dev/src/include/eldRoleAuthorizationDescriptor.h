/*******************************************************************************
 * eldRoleAuthorizationDescriptor.h
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


#ifndef _ELD_ROLE_AUTHORIZATION_DESCRIPTOR_H_
#define _ELD_ROLE_AUTHORIZATION_DESCRIPTOR_H_ 1

/**
 * @file eldRoleAuthorizationDescriptor.h
 * @brief Role Authorization Descriptor Dictionary 관리 루틴 
 */


/**
 * @defgroup eldRoleAuthorizationDescriptor Role Authorization Descriptor Descriptor 관리 루틴 
 * @internal 
 * @ingroup eldDesc
 * @{
 */

stlStatus eldInsertRoleAuthDesc( smlTransId             aTransID,
                                 smlStatement         * aStmt,
                                 stlInt64               aRoleID,
                                 stlInt64               aGranteeID,
                                 stlInt64               aGrantorID,
                                 stlBool                aIsGrantable,
                                 ellEnv               * aEnv );


/** @} eldRoleAuthorizationDescriptor */

#endif /* _ELD_ROLE_AUTHORIZATION_DESCRIPTOR_H_ */
