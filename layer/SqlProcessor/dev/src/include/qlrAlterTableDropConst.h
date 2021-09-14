/*******************************************************************************
 * qlrAlterTableDropConst.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: qlrAlterTableDropConst.h 5803 2012-09-27 06:40:12Z jhkim $
 *
 * NOTES
 *
 *
 ******************************************************************************/

#ifndef _QLR_ALTER_TABLE_DROP_CONST_H_
#define _QLR_ALTER_TABLE_DROP_CONST_H_ 1

/**
 * @file qlrAlterTableDropConst.h
 * @brief ALTER TABLE .. DROP CONSTRAINT
 */

/**
 * @brief DROP CONSTRAINT 를 위한 Init Plan
 */
typedef struct qlrInitDropConst
{
    qlvInitPlan      mCommonInit;       /**< Common Init Plan */
    
    ellDictHandle      mSchemaHandle;     /**< Schema Handle */
    ellDictHandle      mTableHandle;      /**< Table Handle */
    ellDictHandle      mConstHandle;      /**< Constraint Handle */

    stlBool            mKeepIndex;        /**< KEEP INDEX or DROP INDEX */
    stlBool            mIsCascade;        /**< CASCADE CONSTRAINTS */
    
} qlrInitDropConst;




stlStatus qlrValidateDropConst( smlTransId      aTransID,
                                qllDBCStmt    * aDBCStmt,
                                qllStatement  * aSQLStmt,
                                stlChar       * aSQLString,
                                qllNode       * aParseTree,
                                qllEnv        * aEnv );

stlStatus qlrOptCodeDropConst( smlTransId      aTransID,
                               qllDBCStmt    * aDBCStmt,
                               qllStatement  * aSQLStmt,
                               qllEnv        * aEnv );

stlStatus qlrOptDataDropConst( smlTransId      aTransID,
                               qllDBCStmt    * aDBCStmt,
                               qllStatement  * aSQLStmt,
                               qllEnv        * aEnv );

stlStatus qlrExecuteDropConst( smlTransId      aTransID,
                               smlStatement  * aStmt,
                               qllDBCStmt    * aDBCStmt,
                               qllStatement  * aSQLStmt,
                               qllEnv        * aEnv );

/*
 * Validation 보조 함수
 */

stlStatus qlrGetDropConstHandle( smlTransId           aTransID,
                                 qllDBCStmt         * aDBCStmt,
                                 qllStatement       * aSQLStmt,
                                 stlChar            * aSQLString,
                                 ellDictHandle      * aTableHandle,
                                 qlpDropConstObject * aConstObject,
                                 ellDictHandle      * aConstHandle,
                                 qllEnv             * aEnv );

stlStatus qlrGetDropNameConstHandle( smlTransId           aTransID,
                                     qllDBCStmt         * aDBCStmt,
                                     qllStatement       * aSQLStmt,
                                     stlChar            * aSQLString,
                                     ellDictHandle      * aTableHandle,
                                     qlpObjectName      * aConstName,
                                     ellDictHandle      * aConstHandle,
                                     qllEnv             * aEnv );

stlStatus qlrGetDropPKHandle( smlTransId           aTransID,
                              qllDBCStmt         * aDBCStmt,
                              qllStatement       * aSQLStmt,
                              stlChar            * aSQLString,
                              ellDictHandle      * aTableHandle,
                              qlpDropConstObject * aConstObject,
                              ellDictHandle      * aConstHandle,
                              qllEnv             * aEnv );

stlStatus qlrGetDropUKHandle( smlTransId           aTransID,
                              qllDBCStmt         * aDBCStmt,
                              qllStatement       * aSQLStmt,
                              stlChar            * aSQLString,
                              ellDictHandle      * aTableHandle,
                              qlpDropConstObject * aConstObject,
                              ellDictHandle      * aConstHandle,
                              qllEnv             * aEnv );

/*
 * Execution 보조 함수
 */

stlStatus qlrSetColumnNullable4DropPK( smlTransId         aTransID,
                                       smlStatement     * aStmt,
                                       ellDictHandle    * aPrimaryKeyHandle,
                                       ellObjectList    * aNotNullList,
                                       qllEnv           * aEnv );

stlStatus qlrSetColumnNullable4DropPrimaryKeyList( smlTransId         aTransID,
                                                   smlStatement     * aStmt,
                                                   qllDBCStmt       * aDBCStmt,
                                                   ellObjectList    * aPrimaryKeyList,
                                                   qllEnv           * aEnv );

stlStatus qlrSetColumnNullable4DropNotNull( smlTransId         aTransID,
                                            smlStatement     * aStmt,
                                            ellDictHandle    * aNotNullHandle,
                                            ellObjectList    * aPrimaryKeyList,
                                            qllEnv           * aEnv );

stlStatus qlrRefineCache4DropKeyList( smlTransId         aTransID,
                                      smlStatement     * aStmt,
                                      ellObjectList    * aKeyList,
                                      stlBool            aKeepIndex,
                                      ellObjectList    * aKeyIndexList,
                                      ellObjectList    * aKeyTableList,
                                      qllEnv           * aEnv );

stlStatus qlrRefineParentTable4DropForeignKey( smlTransId         aTransID,
                                               smlStatement     * aStmt,
                                               ellDictHandle    * aForeignKeyHandle,
                                               ellObjectList    * aParentTableList,
                                               qllEnv           * aEnv );

stlStatus qlrRefineParentList4DropForeignKeyList( smlTransId         aTransID,
                                                  smlStatement     * aStmt,
                                                  ellObjectList    * aForeignKeyList,
                                                  ellObjectList    * aParentTableList,
                                                  qllEnv           * aEnv );

stlStatus qlrRefineCache4DropCheckClauseList( smlTransId         aTransID,
                                              smlStatement     * aStmt,
                                              ellObjectList    * aConstList,
                                              ellObjectList    * aConstTableList,
                                              qllEnv           * aEnv );

stlStatus qlrRefineCache4DropNotNullList( smlTransId         aTransID,
                                          smlStatement     * aStmt,
                                          ellObjectList    * aConstList,
                                          ellObjectList    * aConstTableList,
                                          qllEnv           * aEnv );

#endif /* _QLR_ALTER_TABLE_DROP_CONST_H_ */

