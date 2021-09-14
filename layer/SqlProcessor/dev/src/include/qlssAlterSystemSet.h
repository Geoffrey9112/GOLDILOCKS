/*******************************************************************************
 * qlssAlterSystemSet.h
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

#ifndef _QLSS_ALTER_SYSTEM_SET_H_
#define _QLSS_ALTER_SYSTEM_SET_H_ 1

/**
 * @file qlssAlterSystemSet.h
 * @brief System Control Language
 */


/**
 * @brief ALTER SYSTEM SET Init Plan
 */
typedef struct qlssInitSystemSet
{
    knlPropertyID            mPropertyId;   /**< Property Identifier */
    stlChar                * mParamName;    /**< Parameter Name. mProperyId == KNL_PROPERTY_ID_MAX일때 */
    stlBool                  mIsDeferred;   /**< DEFERRED */
    knlPropertyModifyScope   mScope;        /**< SCOPE */
    
    stlChar                * mStringValue;  /**< String Property Value */
    stlInt64                 mIntegerValue; /**< Integer Property Value */
    stlBool                  mBoolValue;    /**< Boolean Property Value */
    stlBool                  mDefaultValue; /**< default Value */
} qlssInitSystemSet;




stlStatus qlssValidateSystemSet( smlTransId      aTransID,
                                 qllDBCStmt    * aDBCStmt,
                                 qllStatement  * aSQLStmt,
                                 stlChar       * aSQLString,
                                 qllNode       * aParseTree,
                                 qllEnv        * aEnv );

stlStatus qlssOptCodeSystemSet( smlTransId      aTransID,
                                qllDBCStmt    * aDBCStmt,
                                qllStatement  * aSQLStmt,
                                qllEnv        * aEnv );

stlStatus qlssOptDataSystemSet( smlTransId      aTransID,
                                qllDBCStmt    * aDBCStmt,
                                qllStatement  * aSQLStmt,
                                qllEnv        * aEnv );

stlStatus qlssExecuteSystemSet( smlTransId      aTransID,
                                smlStatement  * aStmt,
                                qllDBCStmt    * aDBCStmt,
                                qllStatement  * aSQLStmt,
                                qllEnv        * aEnv );


#endif /* _QLSS_ALTER_SYSTEM_SET_H_ */

