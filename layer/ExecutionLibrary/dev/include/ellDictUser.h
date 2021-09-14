/*******************************************************************************
 * ellDictUser.h
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


#ifndef _ELL_DICT_USER_H_
#define _ELL_DICT_USER_H_ 1

/**
 * @file ellDictUser.h
 * @brief User Dictionary Management
 */

/**
 * @addtogroup ellObjectAuth
 * @{
 */

stlStatus ellClearUserPasswordHistory( smlTransId       aTransID,
                                       smlStatement   * aStmt,
                                       ellEnv         * aEnv );


stlStatus ellInsertUserDesc( smlTransId             aTransID,
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

stlStatus ellModifyUserProfile( smlTransId      aTransID,
                                smlStatement  * aStmt,
                                ellDictHandle * aUserHandle,
                                stlInt64        aProfileID,
                                ellAuthenInfo * aAuthenInfo,
                                ellEnv        * aEnv );

stlStatus ellModifyUserAuthentication4Login( smlTransId      aTransID,
                                             smlStatement  * aStmt,
                                             ellDictHandle * aUserHandle,
                                             ellAuthenInfo * aAuthenInfo,
                                             stlBool       * aHasConflict,
                                             ellEnv        * aEnv );

stlStatus ellModifyUserAuthentication4DDL( smlTransId      aTransID,
                                           smlStatement  * aStmt,
                                           ellDictHandle * aUserHandle,
                                           ellAuthenInfo * aAuthenInfo,
                                           ellEnv        * aEnv );

    
stlStatus ellGetUserAuthenticationInfo( smlTransId           aTransID,
                                        smlStatement       * aStmt,
                                        ellDictHandle      * aUserHandle,
                                        ellAuthenInfo      * aAuthenInfo,
                                        ellEnv             * aEnv );
    
stlStatus ellInsertPasswordHistoryDesc( smlTransId             aTransID,
                                        smlStatement         * aStmt,
                                        stlInt64               aAuthID,
                                        stlChar              * aEncryptedPassword,
                                        stlInt64               aPassChangeNO,
                                        stlTime                aPassChangeTime,
                                        ellEnv               * aEnv );

stlStatus ellFindDisableReusePassword( smlTransId             aTransID,
                                      smlStatement         * aStmt,
                                       ellDictHandle        * aUserHandle,
                                       stlChar              * aEncryptedPassword,
                                       stlInt64               aMinChangeNO,
                                       stlTime                aMinChangeTime,
                                       stlBool              * aExist,
                                       ellEnv               * aEnv );

stlStatus ellRemoveOldPassword( smlTransId             aTransID,
                                smlStatement         * aStmt,
                                ellDictHandle        * aUserHandle,
                                stlInt64               aMinChangeNO,
                                stlTime                aMinChangeTime,
                                ellEnv               * aEnv );


stlInt64    ellGetUserProfileID( ellDictHandle * aUserHandle );


/** @} ellObjectAuth */



#endif /* _ELL_DICT_USER_H_ */
