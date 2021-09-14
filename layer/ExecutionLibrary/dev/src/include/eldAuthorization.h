/*******************************************************************************
 * eldAuthorization.h
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


#ifndef _ELD_AUTHORIZATION_H_
#define _ELD_AUTHORIZATION_H_ 1

/**
 * @file eldAuthorization.h
 * @brief Authorization Dictionary 관리 루틴 
 */


/**
 * @defgroup eldAuthorization Authorization Descriptor 관리 루틴 
 * @internal 
 * @ingroup eldDesc
 * @{
 */

stlStatus eldInsertAuthDesc( smlTransId             aTransID,
                             smlStatement         * aStmt,
                             stlInt64             * aAuthID,
                             stlChar              * aAuthName,
                             ellAuthorizationType   aAuthType,
                             stlChar              * aAuthComment,
                             ellEnv               * aEnv );


stlStatus eldRefineCache4AddAuth( smlTransId           aTransID,
                                  smlStatement       * aStmt,
                                  stlInt64             aAuthID,
                                  ellEnv             * aEnv );


stlStatus eldGetAuthHandleByID( smlTransId           aTransID,
                                smlScn               aViewSCN,
                                stlInt64             aAuthID,
                                ellDictHandle      * aAuthHandle,
                                stlBool            * aExist,
                                ellEnv             * aEnv );

stlStatus eldGetAuthHandleByName( smlTransId           aTransID,
                                  smlScn               aViewSCN,
                                  stlChar            * aAuthName,
                                  ellDictHandle      * aAuthHandle,
                                  stlBool            * aExist,
                                  ellEnv             * aEnv );


/** @} eldAuthorization */

#endif /* _ELD_AUTHORIZATION_H_ */
