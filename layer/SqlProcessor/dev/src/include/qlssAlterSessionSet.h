/*******************************************************************************
 * qlssAlterSessionSet.h
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

#ifndef _QLSS_ALTER_SESSION_SET_H_
#define _QLSS_ALTER_SESSION_SET_H_ 1

/**
 * @file qlssAlterSessionSet.h
 * @brief Session Control Language
 */


/**
 * @brief ALTER SESSION SET Init Plan
 */
typedef struct qlssInitSessionSet
{
    knlPropertyID   mPropertyId;         /**< Property Identifier */
    stlChar       * mParamName;          /**< Parameter Name. mProperyId == KNL_PROPERTY_ID_MAX일때 */
    stlChar       * mStringValue;        /**< String Property Value */
    stlInt64        mIntegerValue;       /**< Integer Property Value */
    stlBool         mBoolValue;          /**< Boolean Property Value */
    stlBool         mDefaultValue;       /**< default Value */
} qlssInitSessionSet;



stlStatus qlssValidateSessionSet( smlTransId      aTransID,
                                  qllDBCStmt    * aDBCStmt,
                                  qllStatement  * aSQLStmt,
                                  stlChar       * aSQLString,
                                  qllNode       * aParseTree,
                                  qllEnv        * aEnv );

stlStatus qlssOptCodeSessionSet( smlTransId      aTransID,
                                 qllDBCStmt    * aDBCStmt,
                                 qllStatement  * aSQLStmt,
                                 qllEnv        * aEnv );

stlStatus qlssOptDataSessionSet( smlTransId      aTransID,
                                 qllDBCStmt    * aDBCStmt,
                                 qllStatement  * aSQLStmt,
                                 qllEnv        * aEnv );

stlStatus qlssExecuteSessionSet( smlTransId      aTransID,
                                 smlStatement  * aStmt,
                                 qllDBCStmt    * aDBCStmt,
                                 qllStatement  * aSQLStmt,
                                 qllEnv        * aEnv );

stlStatus qlssCheckPropFormatString( knlPropertyID   aPropID,
                                     stlChar       * aFormatString,
                                     qllEnv        * aEnv );
                               


#endif /* _QLSS_ALTER_SESSION_SET_H_ */

