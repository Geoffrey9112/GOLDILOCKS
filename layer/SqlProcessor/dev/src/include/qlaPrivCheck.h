/*******************************************************************************
 * qlaPrivCheck.h
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

#ifndef _QLA_PRIV_CHECK_H_
#define _QLA_PRIV_CHECK_H_ 1

/**
 * @file qlaPrivCheck.h
 * @brief Privilege Check Logic
 */


/*****************************************************************************
 * privilege for COMMENT ON object
 *****************************************************************************/

stlStatus qlaCheckPrivCommentObject( smlTransId              aTransID,
                                     qllDBCStmt            * aDBCStmt,
                                     qllStatement          * aSQLStmt,
                                     ellObjectType           aObjectType,
                                     ellDictHandle         * aObjectHandle,
                                     qllEnv                * aEnv );

/*****************************************************************************
 * privilege for INDEX object
 *****************************************************************************/

stlStatus qlaCheckPrivDropIndex( smlTransId              aTransID,
                                 qllDBCStmt            * aDBCStmt,
                                 qllStatement          * aSQLStmt,
                                 ellDictHandle         * aIndexHandle,
                                 qllEnv                * aEnv );

stlStatus qlaCheckPrivAlterIndex( smlTransId              aTransID,
                                  qllDBCStmt            * aDBCStmt,
                                  qllStatement          * aSQLStmt,
                                  ellDictHandle         * aIndexHandle,
                                  qllEnv                * aEnv );

/*****************************************************************************
 * privilege for SEQUENCE object
 *****************************************************************************/

stlStatus qlaCheckPrivDropSequence( smlTransId              aTransID,
                                    qllDBCStmt            * aDBCStmt,
                                    qllStatement          * aSQLStmt,
                                    ellDictHandle         * aSeqHandle,
                                    qllEnv                * aEnv );

stlStatus qlaCheckPrivAlterSequence( smlTransId              aTransID,
                                     qllDBCStmt            * aDBCStmt,
                                     qllStatement          * aSQLStmt,
                                     ellDictHandle         * aSeqHandle,
                                     qllEnv                * aEnv );

/*****************************************************************************
 * privilege for DROP object
 *****************************************************************************/

stlStatus qlaCheckPrivDropUser( smlTransId              aTransID,
                                qllDBCStmt            * aDBCStmt,
                                qllStatement          * aSQLStmt,
                                qllEnv                * aEnv );

stlStatus qlaCheckPrivDropSpace( smlTransId              aTransID,
                                 qllDBCStmt            * aDBCStmt,
                                 qllStatement          * aSQLStmt,
                                 qllEnv                * aEnv );

stlStatus qlaCheckPrivDropSchema( smlTransId              aTransID,
                                  qllDBCStmt            * aDBCStmt,
                                  qllStatement          * aSQLStmt,
                                  ellDictHandle         * aSchemaHandle,
                                  qllEnv                * aEnv );

stlStatus qlaCheckPrivDropTable( smlTransId              aTransID,
                                 qllDBCStmt            * aDBCStmt,
                                 qllStatement          * aSQLStmt,
                                 ellDictHandle         * aTableHandle,
                                 qllEnv                * aEnv );

stlStatus qlaCheckPrivDropView( smlTransId              aTransID,
                                qllDBCStmt            * aDBCStmt,
                                qllStatement          * aSQLStmt,
                                ellDictHandle         * aViewHandle,
                                qllEnv                * aEnv );

stlStatus qlaCheckPrivDropSynonym( smlTransId              aTransID,
                                   qllDBCStmt            * aDBCStmt,
                                   qllStatement          * aSQLStmt,
                                   ellDictHandle         * aSynonymHandle,
                                   qllEnv                * aEnv );

/*****************************************************************************
 * privilege ON DATABASE
 *****************************************************************************/

stlStatus qlaCheckDatabasePriv( smlTransId              aTransID,
                                qllDBCStmt            * aDBCStmt,
                                qllStatement          * aSQLStmt,
                                ellDatabasePrivAction   aDatabaseAction,
                                qllEnv                * aEnv );

/*****************************************************************************
 * privilege ON TABLESPACE
 *****************************************************************************/

stlStatus qlaCheckSpacePriv( smlTransId            aTransID,
                             qllDBCStmt          * aDBCStmt,
                             qllStatement        * aSQLStmt,
                             ellSpacePrivAction    aSpaceAction,
                             ellDictHandle       * aSpaceHandle,
                             qllEnv              * aEnv );

/*****************************************************************************
 * privilege ON SCHEMA
 *****************************************************************************/

stlStatus qlaCheckSchemaPriv( smlTransId            aTransID,
                              qllDBCStmt          * aDBCStmt,
                              qllStatement        * aSQLStmt,
                              ellSchemaPrivAction   aSchemaAction,
                              ellDictHandle       * aSchemaHandle,
                              qllEnv              * aEnv );

/*****************************************************************************
 * privilege ON TABLE
 *****************************************************************************/

stlStatus qlaCheckTablePriv( smlTransId           aTransID,
                             qllDBCStmt         * aDBCStmt,
                             qllStatement       * aSQLStmt,
                             ellTablePrivAction   aTableAction,
                             ellDictHandle      * aTableHandle,
                             qlvRefExprList     * aColumnList,
                             qllEnv             * aEnv );

stlStatus qlaCheckColumnPriv( smlTransId           aTransID,
                              qllDBCStmt         * aDBCStmt,
                              qllStatement       * aSQLStmt,
                              ellDictHandle      * aTableHandle,
                              ellColumnPrivAction  aColumnAction,
                              qlvRefExprList     * aColumnList,
                              qllEnv             * aEnv );


/*****************************************************************************
 * USAGE ON object
 *****************************************************************************/

stlStatus qlaCheckSeqPriv( smlTransId      aTransID,
                           qllDBCStmt    * aDBCStmt,
                           qllStatement  * aSQLStmt,
                           ellDictHandle * aSeqHandle,
                           qllEnv        * aEnv );

/*****************************************************************************
 * Query Spec 
 *****************************************************************************/

stlStatus qlaCheckPrivQuerySpec( smlTransId      aTransID,
                                 qllDBCStmt    * aDBCStmt,
                                 qllStatement  * aSQLStmt,
                                 qlvInitNode   * aRelationTree,
                                 qllEnv        * aEnv );

stlStatus qlaCheckPrivBaseTable( smlTransId               aTransID,
                                 qllDBCStmt             * aDBCStmt,
                                 qllStatement           * aSQLStmt,
                                 qlvInitBaseTableNode   * aBaseTable,
                                 qllEnv                 * aEnv );

stlStatus qlaCheckPrivJoinedTable( smlTransId               aTransID,
                                   qllDBCStmt             * aDBCStmt,
                                   qllStatement           * aSQLStmt,
                                   qlvInitJoinTableNode   * aJoinTable,
                                   qllEnv                 * aEnv );

stlStatus qlaCheckPrivSubTable( smlTransId      aTransID,
                                qllDBCStmt    * aDBCStmt,
                                qllStatement  * aSQLStmt,
                                qlvInitNode   * aSubTable,
                                qllEnv        * aEnv );

#endif /* _QLA_PRIV_CHECK_H_ */
