/*******************************************************************************
 * eldPasswordHistory.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: eldPasswordHistory.h 13496 2014-08-29 05:38:43Z leekmo $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _ELD_USER_SCHEMA_PATH_H_
#define _ELD_USER_SCHEMA_PATH_H_ 1

/**
 * @file eldPasswordHistory.h
 * @brief User Password History Dictionary 관리 루틴 
 */


/**
 * @defgroup eldPasswordHistory User Password History Descriptor 관리 루틴 
 * @internal 
 * @ingroup eldDesc
 * @{
 */

stlStatus eldClearUserPasswordHistory( smlTransId       aTransID,
                                       smlStatement   * aStmt,
                                       ellEnv         * aEnv );

stlStatus eldInsertPasswordHistoryDesc( smlTransId             aTransID,
                                        smlStatement         * aStmt,
                                        stlInt64               aAuthID,
                                        stlChar              * aEncryptedPassword,
                                        stlInt64               aPassChangeNO,
                                        stlTime                aPassChangeTime,
                                        ellEnv               * aEnv );

stlStatus eldFindDisableReusePassword( smlTransId             aTransID,
                                       smlStatement         * aStmt,
                                       ellDictHandle        * aUserHandle,
                                       stlChar              * aEncryptedPassword,
                                       stlInt64               aMinChangeNO,
                                       stlTime                aMinChangeTime,
                                       stlBool              * aExist,
                                       ellEnv               * aEnv );

stlStatus eldRemoveOldPassword( smlTransId             aTransID,
                                smlStatement         * aStmt,
                                ellDictHandle        * aUserHandle,
                                stlInt64               aMinChangeNO,
                                stlTime                aMinChangeTime,
                                ellEnv               * aEnv );


/** @} eldPasswordHistory */

#endif /* _ELD_USER_SCHEMA_PATH_H_ */
