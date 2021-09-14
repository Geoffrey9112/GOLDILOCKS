/*******************************************************************************
 * eldUser.h
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


#ifndef _ELD_USER_H_
#define _ELD_USER_H_ 1

/**
 * @file eldUser.h
 * @brief User Dictionary 관리 루틴 
 */


/**
 * @defgroup eldUser User Descriptor 관리 루틴 
 * @internal 
 * @ingroup eldDesc
 * @{
 */

stlStatus eldInsertUserDesc( smlTransId             aTransID,
                             smlStatement         * aStmt,
                             stlInt64               aAuthID,
                             stlInt64               aProfileID,
                             stlChar              * aEncryptedPassword,
                             stlInt64               aDataTablespaceID,
                             stlInt64               aTempTablespaceID,
                             ellUserLockedStatus    aLockedStatus,
                             stlTime                aLockedTime,
                             ellUserExpiryStatus    aExpiryStatus,
                             stlTime                aExpiryTime,
                             ellEnv               * aEnv );

stlStatus eldDeleteAuthen4LoginModify( smlTransId               aTransID,
                                       smlStatement           * aStmt,
                                       ellDictHandle          * aUserHandle,
                                       eldMemMark             * aMemMark,
                                       knlValueBlockList     ** aDeletedValueList,
                                       stlBool                * aIsConflict,
                                       ellEnv                 * aEnv );

stlStatus eldSetAuthenInfo( ellAuthenInfo      * aAuthenInfo,
                            knlValueBlockList  * aValueList,
                            ellEnv             * aEnv );

stlStatus eldGetUserAuthenticationInfo( smlTransId           aTransID,
                                        smlStatement       * aStmt,
                                        ellDictHandle      * aUserHandle,
                                        ellAuthenInfo      * aAuthenInfo,
                                        ellEnv             * aEnv );

/** @} eldUser */

#endif /* _ELD_USER_H_ */
