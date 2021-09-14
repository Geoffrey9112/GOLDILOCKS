/*******************************************************************************
 * ztcd.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _ZTCD_H_
#define _ZTCD_H_ 1

/**
 * @file ztcd.h
 * @brief GlieseTool Cyclone Connection Routines
 */


/**
 * @defgroup ztcd Cyclone ODBC Routines
 * @{
 */

stlStatus ztcdAllocEnvHandle( SQLHENV       * aEnvHandle,
                              stlErrorStack * aErrorStack );
                             
stlStatus ztcdCreateConnection( SQLHDBC       * aDbcHandle,
                                SQLHENV         aEnvHandle,
                                stlBool         aIsAutoCommit,
                                stlErrorStack * aErrorStack );

stlStatus ztcdCreateConnection4Syncher( SQLHDBC       * aDbcHandle,
                                        SQLHENV         aEnvHandle,
                                        stlBool         aIsAutoCommit,
                                        stlErrorStack * aErrorStack );

stlStatus ztcdFreeEnvHandle( SQLHENV         aEnvHandle,
                             stlErrorStack * aErrorStack );

stlStatus ztcdDestroyConnection( SQLHDBC         aDbcHandle,
                                 stlErrorStack * aErrorStack );

stlStatus ztcdPrintDiagnostic( SQLSMALLINT     aHandleType,
                               SQLHANDLE       aHandle,
                               stlErrorStack * aErrorStack );

stlStatus ztcdGetDiagnosticMsg( SQLSMALLINT     aHandleType,
                                SQLHANDLE       aHandle,
                                SQLSMALLINT     aRecNumber,
                                stlChar       * aMsg,
                                stlErrorStack * aErrorStack );

stlStatus ztcdSQLAllocHandle( SQLHANDLE       aInputHandle,
                              SQLHANDLE     * aOutputHandlePtr,
                              stlErrorStack * aErrorStack );

stlStatus ztcdSQLFreeHandle( SQLHANDLE       aHandle,
                             stlErrorStack * aErrorStack );

stlStatus ztcdSQLEndTran( SQLSMALLINT     aHandleType,
                          SQLHANDLE       aHandle,
                          SQLSMALLINT     aCompletionType,
                          stlBool         aIgnoreFailure,
                          stlErrorStack * aErrorStack );

stlStatus ztcdSQLPrepare( SQLHSTMT        aStmtHandle,
                          SQLCHAR       * aStmtText,
                          SQLINTEGER      aTextLength,
                          stlErrorStack * aErrorStack );

stlStatus ztcdSQLBindCol( SQLHSTMT        aStmtHandle,
                          SQLUSMALLINT    aColumnNumber,
                          SQLSMALLINT     aTargetType,
                          SQLPOINTER      aTargetValuePtr,
                          SQLLEN          aBufferLength,
                          SQLLEN        * aIndicator,
                          stlErrorStack * aErrorStack );

stlStatus ztcdSQLBindParameter( SQLHSTMT        aStmtHandle,
                                SQLUSMALLINT    aParameterNumber,
                                SQLSMALLINT     aInputOutputType,
                                SQLSMALLINT     aValueType,
                                SQLSMALLINT     aParameterType,
                                SQLSMALLINT     aCharLengthUnit,
                                SQLULEN         aColumnSize,
                                SQLSMALLINT     aDecimalDigits,
                                SQLPOINTER      aParameterValuePtr,
                                SQLLEN          aBufferLength,
                                SQLLEN        * aIndicator,
                                stlErrorStack * aErrorStack );

stlStatus ztcdSQLExecute( SQLHSTMT        aStmtHandle,
                          SQLRETURN     * aReturn,
                          stlErrorStack * aErrorStack );


stlStatus ztcdSQLExecDirect( SQLHSTMT        aStmtHandle,
                             SQLCHAR       * aStmtText,
                             SQLINTEGER      aTextLength,
                             SQLRETURN     * aReturn,
                             stlErrorStack * aErrorStack );

stlStatus ztcdSQLFetch( SQLHSTMT        aStmtHandle,
                        SQLRETURN     * aReturn,
                        stlErrorStack * aErrorStack );

stlStatus ztcdSQLGetData( SQLHSTMT        aStmtHandle,
                          SQLUSMALLINT    aCol_or_Param_Num,
                          SQLSMALLINT     aTargetType,
                          SQLPOINTER      aTargetValuePtr,
                          SQLLEN          aBufferLength,
                          SQLLEN        * aStrLen_or_IndPtr,  
                          SQLRETURN     * aReturn,
                          stlErrorStack * aErrorStack );

stlStatus ztcdSQLCloseCursor( SQLHSTMT        aStmtHandle,
                              SQLRETURN     * aReturn,
                              stlErrorStack * aErrorStack );

stlStatus ztcdSQLFreeStmt( SQLHSTMT        aStmtHandle,
                           SQLUSMALLINT    aOption,
                           SQLRETURN     * aReturn,
                           stlErrorStack * aErrorStack );

stlStatus ztcdSQLGetStmtAttr( SQLHSTMT        aStmtHandle,
                              SQLINTEGER      aAttribute,
                              SQLPOINTER      aValuePtr,
                              SQLINTEGER      aBufferLength, 
                              SQLINTEGER    * aStringLength,
                              stlErrorStack * aErrorStack );

stlStatus ztcdSQLGetDescField( SQLHDESC        aDescriptorHandle,
                               SQLSMALLINT     aRecNumber,
                               SQLSMALLINT     aFieldIdentifier,
                               SQLPOINTER      aValuePtr,
                               SQLINTEGER      aBufferLength,
                               SQLINTEGER    * aStringLengthPtr,
                               stlErrorStack * aErrorStack );

void ztcdWriteDiagnostic( SQLSMALLINT     aHandleType,
                          SQLHANDLE       aHandle,
                          stlErrorStack * aErrorStack );

stlStatus ztcdBuildLogFileInfo( SQLHDBC         aDbcHandle,
                                stlErrorStack * aErrorStack );

stlStatus ztcdGetTablePhysicalIDFromDB( SQLHDBC         aDbcHandle,
                                        stlChar       * aSchemaName,
                                        stlChar       * aTableName,
                                        stlInt64      * aPhysicalId,
                                        stlErrorStack * aErrorStack );

stlStatus ztcdGetTableIDFromSlaveMeta( SQLHDBC         aDbcHandle,
                                       stlChar       * aSchemaName,
                                       stlChar       * aTableName,
                                       stlInt32      * aTableId,
                                       stlBool       * aFound,
                                       stlErrorStack * aErrorStack );

stlStatus ztcdGetColumnId( SQLHDBC         aDbcHandle,
                           stlInt64        aSchemaId,
                           stlInt64        aTableId,
                           stlChar       * aColumnName,
                           stlInt64      * aColumnId,
                           stlErrorStack * aErrorStack );

stlStatus ztcdGetColumnTypeId( SQLHDBC         aDbcHandle,
                               stlInt64        aTableId, 
                               stlInt64        aColumnId,
                               stlInt64      * aDataTypeId,
                               stlInt64      * aIntervalTypeId,
                               stlErrorStack * aErrorStack );
                                  
stlStatus ztcdGetColumnConstraint( SQLHDBC         aDbcHandle,
                                   stlInt64        aSchemaId, 
                                   stlInt64        aTableId,
                                   stlInt64        aColumnId,
                                   stlInt64        aConstraintId,
                                   stlBool       * aIskey,
                                   stlErrorStack * aErrorStack );
                                  
stlStatus ztcdGetPrimaryConstraintId( SQLHDBC         aDbcHandle,
                                      stlInt64        aSchemaId, 
                                      stlInt64        aTableId,
                                      stlInt64      * aConstraintId,
                                      stlBool       * aFound,
                                      stlErrorStack * aErrorStack );
                                     
stlStatus ztcdGetColumnUniqueConstraint( SQLHDBC         aDbcHandle,
                                         stlInt64        aSchemaId, 
                                         stlInt64        aTableId,
                                         stlInt64        aColumnId,
                                         stlBool       * aIsUnique,
                                         stlErrorStack * aErrorStack );
                            
stlStatus ztcdGetSlaveColumnInfo( SQLHDBC             aDbcHandle,
                                  ztcSlaveTableInfo * aTableInfo,
                                  stlErrorStack     * aErrorStack );

stlStatus ztcdGetSchemaIDFromDB( SQLHDBC         aDbcHandle,
                                 stlChar       * aSchemaName,
                                 stlInt64      * aSchemaId,
                                 stlErrorStack * aErrorStack );

stlStatus ztcdGetTableIDFromDB( SQLHDBC         aDbcHandle,
                                stlInt64        aSchemaId,
                                stlChar       * aTableName,
                                stlInt64      * aTableId,
                                stlErrorStack * aErrorStack );

stlStatus ztcdSetTransTableSize( SQLHDBC         aDbcHandle,
                                 stlErrorStack * aErrorStack );

stlStatus ztcdInsertTableSlaveMeta( SQLHDBC             aDbcHandle,
                                    ztcSlaveTableInfo * aTableInfo,
                                    stlErrorStack     * aErrorStack );

stlStatus ztcdBuildTableInfoMeta( SQLHDBC               aDbcHandle,
                                  stlAllocator        * aAllocator,
                                  ztcMasterTableMeta  * aTableMeta,
                                  stlErrorStack       * aErrorStack );

stlStatus ztcdGetColumnDataTypeStr( SQLHDBC         aDbcHandle,
                                    stlInt64        aTableId,
                                    stlInt64        aColumnId,
                                    stlChar       * aDataTypeStr,
                                    stlErrorStack * aErrorStack );

stlStatus ztcdGetCurrentLogBufferInfo( SQLHDBC         aDbcHandle,
                                       stlInt16      * aGroupId,
                                       stlInt32      * aBlockSeq,
                                       stlInt64      * aFileSeqNo,
                                       stlErrorStack * aErrorStack );

stlStatus ztcdGetCharacterOctetLength( SQLHDBC         aDbcHandle,
                                       stlInt64        aTableId,
                                       stlInt64        aColumnId,
                                       stlInt64      * aOctetLength,
                                       stlErrorStack * aErrorStack );
                                      
stlStatus ztcdGetNumericInfo( SQLHDBC         aDbcHandle,
                              stlInt64        aTableId,
                              stlInt64        aColumnId,
                              stlInt32      * aPrecision,
                              stlInt32      * aScale,
                              stlErrorStack * aErrorStack );
                             
stlStatus ztcdGetIntervalTypeInfo( SQLHDBC             aDbcHandle,
                                   ztcSlaveTableInfo * aTableInfo,
                                   stlErrorStack     * aErrorStack );

stlStatus ztcdGetCharacterMaxLength( SQLHDBC         aDbcHandle,
                                     stlInt64        aTableId,
                                     stlInt64        aColumnId,
                                     stlInt64      * aCharLength,
                                     stlErrorStack * aErrorStack );
                                    
stlStatus ztcdGetCharacterLengthUnit( SQLHDBC         aDbcHandle,
                                      stlInt64        aTableId,
                                      stlInt64        aColumnId,
                                      stlInt16      * aCharLengthUnit,
                                      stlErrorStack * aErrorStack );
                                    
stlStatus ztcdGetMbMaxLength( SQLHDBC         aDbcHandle,
                              stlInt8       * aLength,
                              stlErrorStack * aErrorStack );

stlStatus ztcdGetPrimaryKeyIndexName( SQLHDBC         aDbcHandle,
                                      stlInt64        aSchemaId,
                                      stlInt64        aTableId,
                                      stlChar       * aIndexName,
                                      stlErrorStack * aErrorStack );

stlStatus ztcdResetSlaveMeta( SQLHDBC         aDbcHandle,
                              stlErrorStack * aErrorStack );

stlStatus ztcdInsertCommitState( ztcApplierMgr  * aApplierMgr,
                                 SQLHDBC          aDbcHandle,
                                 SQLHSTMT         aStmtHandle,
                                 ztcCaptureInfo * aCaptureInfo,
                                 stlErrorStack  * aErrorStack );
                                
stlStatus ztcdSelectCommitState( stlErrorStack * aErrorStack );
                                
stlStatus ztcdInsertBlankCommitState( SQLHDBC          aDbcHandle,
                                      ztcCaptureInfo * aRestartInfo,
                                      stlErrorStack  * aErrorStack );
                              
stlStatus ztcdDoInsertNExecute( ztcApplierMgr        * aApplierMgr,
                                ztcApplierTableInfo  * aTableInfo,
                                stlErrorStack        * aErrorStack );

stlStatus ztcdDoDeleteNExecute( ztcApplierMgr        * aApplierMgr,
                                ztcApplierTableInfo  * aTableInfo,
                                stlErrorStack        * aErrorStack );

stlStatus ztcdDoUpdateNExecute( ztcApplierMgr        * aApplierMgr,
                                ztcApplierTableInfo  * aTableInfo,
                                stlErrorStack        * aErrorStack );
                               
stlStatus ztcdInitializeApplierStatement( ztcApplierMgr * aApplierMgr,
                                          ztcStmtInfo   * aStmtInfo,
                                          stlErrorStack * aErrorStack );

stlStatus ztcdFinalizeApplierStatement( ztcStmtInfo   * aStmtInfo,
                                        stlErrorStack * aErrorStack );

stlStatus ztcdGetColumnParamInfo( SQLHDBC          aDbcHandle,
                                  stlInt64         aTableId,
                                  dtlFuncVector  * aDTVectorFunc,
                                  ztcColumnInfo  * aColumnInfo,
                                  stlErrorStack  * aErrorStack );

stlStatus ztcdCreateMetaTable4Slave( stlErrorStack * aErrorStack );

stlStatus ztcdGetSupplLogTableValue( SQLHDBC         aDbcHandle,
                                     stlChar       * aSchemaName,
                                     stlChar       * aTableName, 
                                     stlBool       * aValue,
                                     stlErrorStack * aErrorStack );

stlStatus ztcdSetDTFuncValue( SQLHDBC           aDbcHandle,
                              dtlCharacterSet * aCharacterSet,
                              SQLINTEGER      * aTimezone,
                              stlErrorStack   * aErrorStack );

stlStatus ztcdSetPropagateMode( SQLHDBC         aDbcHandle,
                                stlErrorStack * aErrorStack );
                                
stlStatus ztcdGetArchiveLogInfo( SQLHDBC         aDbcHandle,
                                 stlChar       * aPath,
                                 stlChar       * aFilePrefix,
                                 stlBool       * aIsArchiveMode,
                                 stlErrorStack * aErrorStack );

stlStatus ztcdGetLogFileStatus( SQLHDBC         aDbcHandle,
                                stlInt16      * aGroupId,
                                stlInt64      * aFileSeqNo,
                                stlInt32      * aBlockSeq,
                                stlErrorStack * aErrorStack );

/** ztcdSync.c */
stlStatus ztcdGetSyncColumnInfo( SQLHDBC         aDbcHandle,
                                 stlChar       * aSchemaName,
                                 stlChar       * aTableName,
                                 dtlFuncVector * aDTVector,
                                 ztcColumnInfo * aColumnInfo,
                                 stlErrorStack * aErrorStack );

stlStatus ztcdSyncMgrSQLBindCol( SQLHSTMT            aStmt,
                                 ztcSyncBufferInfo * aBufferInfo,
                                 stlErrorStack     * aErrorStack );

stlStatus ztcdDeleteSlaveRecord( SQLHSTMT           aStmtHandle,
                                 ztcSyncTableInfo * aTableInfo,
                                 stlErrorStack    * aErrorStack );

stlStatus ztcdSyncMgrSQLExecDirect( SQLHSTMT           aStmt,
                                    ztcSyncTableInfo * aTableInfo,
                                    stlErrorStack    * aErrorStack );

stlStatus ztcdSyncMgrGetDiagSQLState( SQLHSTMT        aStmtHandle,
                                      SQLCHAR       * aSqlState,
                                      stlErrorStack * aErrorStack );

stlStatus ztcdSyncMgrSQLGetData( SQLHSTMT              aStmt,
                                 stlDynamicAllocator * aDynamicMem,
                                 ztcSyncBufferInfo   * aBufferInfo,
                                 stlInt64              aFetchedSize,
                                 stlErrorStack       * aErrorStack );

stlStatus ztcdSyncherSQLBindParameter( SQLHSTMT        aStmtHandle,
                                       ztcSyncher    * aSyncher,
                                       stlErrorStack * aErrorStack );

stlStatus ztcdSyncherSQLPrepare( SQLHSTMT           aStmtHandle,
                                 ztcSyncTableInfo * aTableInfo,
                                 stlErrorStack    * aErrorStack );

stlStatus ztcdGetMinBeginLsn( SQLHDBC         aDbcHandle,
                              stlInt64      * aBeginLsn,
                              stlErrorStack * aErrorStack );

/** @} */


/** @} */

#endif /* _ZTCD_H_ */
