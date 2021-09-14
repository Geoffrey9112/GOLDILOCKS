/*******************************************************************************
 * qlpParse.h
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

#ifndef _QLPPARSE_H_
#define _QLPPARSE_H_ 1


/**
 * @file qlpParse.h
 * @brief SQL Processor Layer Parser
 */

#include <qll.h>
#include <qlDef.h>

/**
 * @addtogroup qlpParser
 * @{
 */

/******************************************************************************* 
 * FUNCTIONS for NODE : qlpParse.c
 ******************************************************************************/


inline stlChar * qlpMakeBuffer( stlParseParam  * aParam,
                                stlInt32         aSize );

inline stlInt64 qlpToIntegerFromString( stlParseParam  * aParam,
                                        stlChar        * aStr );

inline qlpAlterDatabaseAddLogfileGroup * qlpMakeAlterDatabaseAddLogfileGroup(
    stlParseParam    * aParam,
    stlInt32           aNodePos,
    qllNodeType        aNodeType,
    qlpValue         * aLogGroupId,
    qlpList          * aLogfileList,
    qlpSize          * aLogfileSize,
    qlpValue         * aLogfileReuse );

inline qlpAlterDatabaseAddLogfileMember * qlpMakeAlterDatabaseAddLogfileMember(
    stlParseParam    * aParam,
    stlInt32           aNodePos,
    qllNodeType        aNodeType,
    qlpList          * aMemberList,
    qlpValue         * aGroupId );

inline qlpAlterDatabaseDropLogfileGroup * qlpMakeAlterDatabaseDropLogfileGroup(
    stlParseParam    * aParam,
    stlInt32           aNodePos,
    qllNodeType        aNodeType,
    qlpValue         * aLogGroupId );

inline qlpAlterDatabaseDropLogfileMember * qlpMakeAlterDatabaseDropLogfileMember(
    stlParseParam    * aParam,
    stlInt32           aNodePos,
    qllNodeType        aNodeType,
    qlpList          * aMemberList );

inline qlpAlterDatabaseRenameLogfile * qlpMakeAlterDatabaseRenameLogfile(
    stlParseParam    * aParam,
    stlInt32           aNodePos,
    qllNodeType        aNodeType,
    qlpList          * aFromFileList,
    qlpList          * aToFileList );

inline qlpAlterDatabaseArchivelogMode * qlpMakeAlterDatabaseArchivelogMode(
    stlParseParam    * aParam,
    stlInt32           aNodePos,
    stlInt32           aMode );

inline qlpAlterDatabaseBackup * qlpMakeAlterDatabaseBackup(
    stlParseParam  * aParam,
    stlInt32         aNodePos,
    qllNodeType      aNodeType,
    qlpValue       * aCommand,
    stlUInt8         aBackupType,
    qlpValue       * aOption );

inline qlpAlterDatabaseDeleteBackupList * qlpMakeDeleteBackupList(
    stlParseParam  * aParam,
    stlInt32         aNodePos,
    qllNodeType      aNodeType,
    qlpValue       * aTarget,
    stlBool          aIsIncluding );

inline qlpIncrementalBackupOption * qlpMakeIncrementalBackupOption(
    stlParseParam  * aParam,
    qlpValue       * aLevel,
    qlpValue       * aOption );

inline qlpBackupCtrlfile * qlpMakeBackupCtrlfile( stlParseParam  * aParam,
                                                  stlInt32         aNodePos,
                                                  qlpValue       * aTarget );

inline qlpRestoreCtrlfile * qlpMakeRestoreCtrlfile( stlParseParam  * aParam,
                                                    stlInt32         aNodePos,
                                                    qlpValue       * aTarget );

inline qlpAlterDatabaseIrrecoverable * qlpMakeAlterDatabaseIrrecoverable(
    stlParseParam  * aParam,
    stlInt32         aNodePos,
    qlpList        * aSegmentList );

inline qlpAlterDatabaseRecover * qlpMakeAlterDatabaseRecover(
    stlParseParam  * aParam,
    stlInt32         aNodePos,
    qlpValue       * aRecoveryOption );

inline qlpImrOption * qlpMakeImrOption( stlParseParam  * aParam,
                                        stlUInt8         aImrOption,
                                        stlUInt8         aImrCondition,
                                        qlpValue       * aConditionValue,
                                        stlBool          aBackupCtrlfile );

inline qlpInteractiveImrCondition * qlpMakeInteractiveImrCondition( stlParseParam  * aParam,
                                                                    stlUInt8         aImrCondition,
                                                                    qlpValue       * aLogfileName );

inline qlpRestoreOption * qlpMakeUntilOption( stlParseParam  * aParam,
                                              stlUInt8         aUntilType,
                                              qlpValue       * aUntilValue );

inline qlpAlterDatabaseRecoverTablespace * qlpMakeAlterDatabaseRecoverTablespace(
    stlParseParam  * aParam,
    stlInt32         aNodePos,
    qlpValue       * aName );

inline qlpAlterDatabaseRecoverDatafile * qlpMakeAlterDatabaseRecoverDatafile(
    stlParseParam * aParam,
    stlInt32        aNodePos,
    qlpList       * aDatafileList );

inline qlpDatafileList * qlpMakeRecoverDatafileTarget( stlParseParam * aParam,
                                                       qlpValue      * aDatafileName,
                                                       qlpValue      * aBackup,
                                                       stlBool         aForCorruption );

inline qlpAlterDatabaseRestore * qlpMakeAlterDatabaseRestore( stlParseParam  * aParam,
                                                              stlInt32         aNodePos,
                                                              qllNodeType      aNodeType,
                                                              qlpValue       * aRestoreOption );

inline qlpAlterDatabaseRestoreTablespace * qlpMakeAlterDatabaseRestoreTablespace(
    stlParseParam  * aParam,
    stlInt32         aNodePos,
    qllNodeType      aNodeType,
    qlpValue       * aTbsName );

inline qlpAlterDatabaseClearPassHistory * qlpMakeAlterDatabaseClearPassHistory( stlParseParam  * aParam,
                                                                                stlInt32         aNodePos );

/* statement node */


/********************************************************************************
 * Authorization DDL
 ********************************************************************************/

inline qlpUserDef * qlpMakeUserDef( stlParseParam      * aParam,
                                    stlInt32             aNodePos,
                                    qlpValue           * aUserName,
                                    qlpValue           * aPassword,
                                    qlpValue           * aDefaultSpace,
                                    qlpValue           * aTempSpace,
                                    qlpValue           * aSchemaName,
                                    qlpValue           * aProfileName,
                                    stlBool              aPasswordExpire,
                                    stlBool              aAccountLock );

inline qlpDropUser * qlpMakeDropUser( stlParseParam    * aParam,
                                      stlInt32           aNodePos,
                                      stlBool            aIfExists,
                                      qlpValue         * aUserName,
                                      stlBool            aIsCascade );

inline qlpAlterUser * qlpMakeAlterUser( stlParseParam      * aParam,
                                        stlInt32             aNodePos,
                                        qlpAlterUserAction   aAlterAction,
                                        qlpValue           * aUserName,
                                        qlpValue           * aNewPassword,
                                        qlpValue           * aOrgPassword,
                                        qlpValue           * aSpaceName,
                                        qlpList            * aSchemaList,
                                        qlpValue           * aProfileName );

inline void qlpCheckDuplicatePasswordParameter( stlParseParam        * aParam,
                                                qlpList              * aList,
                                                qlpPasswordParameter * aParameter);

inline qlpPasswordParameter * qlpMakePasswordParameter( stlParseParam   * aParam,
                                                        stlInt32          aNodePos,
                                                        ellProfileParam   aParameterType,
                                                        stlBool           aIsDEFAULT,
                                                        stlBool           aIsUNLIMITED,
                                                        qlpValue        * aValue,
                                                        stlInt32          aValueStrLen );

inline qlpProfileDef * qlpMakeProfileDef( stlParseParam        * aParam,
                                          stlInt32               aNodePos,
                                          qlpValue             * aName,
                                          qlpList              * aPasswordParameters );

inline qlpDropProfile * qlpMakeDropProfile( stlParseParam    * aParam,
                                            stlInt32           aNodePos,
                                            stlBool            aIfExists,
                                            qlpValue         * aName,
                                            stlBool            aIsCascade );

inline qlpAlterProfile * qlpMakeAlterProfile( stlParseParam        * aParam,
                                              stlInt32               aNodePos,
                                              qlpValue             * aName,
                                              qlpList              * aPasswordParameters );

inline qlpPrivAction * qlpMakePrivAction( stlParseParam    * aParam,
                                          stlInt32           aNodePos,
                                          ellPrivObject      aPrivObject,
                                          stlInt32           aPrivAction,
                                          qlpList          * aColumnList );


inline qlpPrivObject * qlpMakePrivObject( stlParseParam    * aParam,
                                          stlInt32           aNodePos,
                                          ellPrivObject      aPrivObject,
                                          qlpValue         * aNonSchemaObjectName,
                                          qlpObjectName    * aSchemaObjectName );

inline qlpPrivilege * qlpMakePrivilege( stlParseParam    * aParam,
                                        stlInt32           aNodePos,
                                        qlpPrivObject    * aPrivObject,
                                        qlpList          * aPrivActionList );

inline qlpGrantPriv * qlpMakeGrantPriv( stlParseParam    * aParam,
                                        stlInt32           aNodePos,
                                        qlpPrivilege     * aPrivilege,
                                        qlpList          * aGranteeList,
                                        stlBool            aWithGrant );

inline qlpRevokePriv * qlpMakeRevokePriv( stlParseParam    * aParam,
                                          stlInt32           aNodePos,
                                          qlpPrivilege     * aPrivilege,
                                          qlpList          * aRevokeeList,
                                          stlBool            aGrantOption,
                                          qlpValue         * aRevokeBehavior );

/********************************************************************************
 * Tablespace DDL
 ********************************************************************************/

inline qlpTablespaceDef * qlpMakeTablespaceDef( stlParseParam      * aParam,
                                                stlInt32             aNodePos,
                                                qllNodeType          aNodeType,
                                                qlpSpaceMediaType    aMediaType,
                                                qlpSpaceUsageType    aUsageType,
                                                qlpValue           * aName,
                                                qlpList            * aDataFileSpecs,
                                                qlpTablespaceAttr  * aTbsAttrs );

inline qlpDropTablespace * qlpMakeDropTablespace( stlParseParam    * aParam,
                                                  stlInt32           aNodePos,
                                                  stlBool            aIfExists,
                                                  qlpValue         * aTbsName,
                                                  qlpValue         * aIsIncluding,
                                                  qlpValue         * aDropDatafiles,
                                                  qlpValue         * aIsCascade );


inline qlpAlterSpaceAddFile * qlpMakeAlterSpaceAddFile( stlParseParam    * aParam,
                                                        stlInt32           aNodePos,
                                                        qllNodeType        aNodeType,
                                                        qlpValue         * aName,
                                                        stlInt32           aUsageTypePos,
                                                        qlpSpaceMediaType  aMediaType,
                                                        qlpSpaceUsageType  aUsageType,
                                                        qlpList          * aFileSpecs );

inline qlpAlterTablespaceBackup * qlpMakeAlterTablespaceBackup( stlParseParam    * aParam,
                                                                stlInt32           aNodePos,
                                                                qllNodeType      aNodeType,
                                                                qlpValue       * aName,
                                                                qlpValue       * aCommand,
                                                                stlUInt8         aBackupType,
                                                                qlpValue       * aOption );

inline qlpAlterSpaceDropFile * qlpMakeAlterSpaceDropFile( stlParseParam    * aParam,
                                                          stlInt32           aNodePos,
                                                          qllNodeType        aNodeType,
                                                          qlpValue         * aName,
                                                          stlInt32           aUsageTypePos,
                                                          qlpSpaceMediaType  aMediaType,
                                                          qlpSpaceUsageType  aUsageType,
                                                          qlpValue         * aFileName );

inline qlpAlterSpaceOnline * qlpMakeAlterSpaceOnline( stlParseParam  * aParam,
                                                      stlInt32         aNodePos,
                                                      qllNodeType      aNodeType,
                                                      qlpValue       * aName );

inline qlpAlterSpaceOffline * qlpMakeAlterSpaceOffline( stlParseParam  * aParam,
                                                        stlInt32         aNodePos,
                                                        qllNodeType      aNodeType,
                                                        qlpValue       * aName,
                                                        qlpValue       * aOfflineBehavior );

inline qlpAlterSpaceRenameFile * qlpMakeAlterSpaceRenameFile( stlParseParam    * aParam,
                                                              stlInt32           aNodePos,
                                                              qllNodeType        aNodeType,
                                                              qlpValue         * aName,
                                                              stlInt32           aUsageTypePos,
                                                              qlpSpaceMediaType  aMediaType,
                                                              qlpSpaceUsageType  aUsageType,
                                                              qlpList          * aFromFileList,
                                                              qlpList          * aToFileList );

inline qlpAlterSpaceRename * qlpMakeAlterSpaceRename( stlParseParam    * aParam,
                                                      stlInt32           aNodePos,
                                                      qllNodeType        aNodeType,
                                                      qlpValue         * aName,
                                                      qlpValue         * aNewName );


/********************************************************************************
 * Schema DDL
 ********************************************************************************/

inline qlpSchemaDef * qlpMakeSchemaDef( stlParseParam  * aParam,
                                        stlInt32         aNodePos,
                                        qlpValue       * aSchemaName,
                                        qlpValue       * aOwnerName,
                                        qlpList        * aSchemaElemList );

inline qlpDropSchema * qlpMakeDropSchema( stlParseParam    * aParam,
                                          stlInt32           aNodePos,
                                          stlBool            aIfExists,
                                          qlpValue         * aSchemaName,
                                          stlBool            aIsCascade );


/********************************************************************************
 * Table DDL
 ********************************************************************************/

inline qlpTableDef * qlpMakeTableDef( stlParseParam  * aParam,
                                      stlInt32         aNodePos,
                                      qlpList        * aRelName,
                                      stlInt32         aRelPosition,
                                      qlpList        * aTableElems,
                                      qlpList        * aRelationAttrs,
                                      qlpList        * aColumnNameList,
                                      qlpQueryExpr   * aSubquery,
                                      stlBool          aWithData );


inline qlpDropTable * qlpMakeDropTable( stlParseParam    * aParam,
                                        stlInt32           aNodePos,
                                        stlBool            aIfExists,
                                        qlpList          * aName,
                                        stlBool            aIsCascade );

inline qlpTruncateTable * qlpMakeTruncateTable( stlParseParam         * aParam,
                                                stlInt32                aNodePos,
                                                qlpList               * aName,
                                                qlpValue              * aIdentityRestart,
                                                qlpValue              * aDropStorage );


inline qlpAlterTableAddColumn * qlpMakeAlterTableAddColumn( stlParseParam * aParam,
                                                            stlInt32        aNodePos,
                                                            qlpList       * aTableName,
                                                            qlpList       * aColDefList );

inline qlpAlterTableDropColumn * qlpMakeAlterTableDropColumn( stlParseParam       * aParam,
                                                              stlInt32              aNodePos,
                                                              qlpList             * aTableName,
                                                              stlInt32              aDropActionPos,
                                                              qlpDropColumnAction   aDropAction,
                                                              qlpList             * aColNameList,
                                                              stlBool               aIsCascade );

inline qlpAlterIdentity * qlpMakeAlterIdentitySpec( stlParseParam       * aParam,
                                                    qlpIdentityType       aIdentType,
                                                    qlpSequenceOption   * aIdentSeqOptions );
                                                    
inline qlpAlterTableAlterColumn * qlpMakeAlterTableAlterColumn( stlParseParam       * aParam,
                                                                stlInt32              aNodePos,
                                                                qlpList             * aTableName,
                                                                qlpValue            * aColumnName,
                                                                stlInt32              aAlterActionPos,
                                                                qlpAlterColumnAction  aAlterAction,
                                                                qlpList             * aNotNullName,
                                                                qlpConstraintAttr   * aNotNullConstAttr,
                                                                qllNode             * aAlterOption );

inline qlpAlterTableAddConstraint * qlpMakeAlterTableAddConstraint( stlParseParam      * aParam,
                                                                    stlInt32             aNodePos,
                                                                    qlpList            * aTableName,
                                                                    qlpList            * aConstDef );

inline qlpAlterTableDropConstraint * qlpMakeAlterTableDropConstraint( stlParseParam      * aParam,
                                                                      stlInt32             aNodePos,
                                                                      qlpList            * aTableName,
                                                                      qlpDropConstObject * aConstObject,
                                                                      stlBool              aKeepIndex,
                                                                      stlBool              aIsCascade );

inline qlpAlterTableAlterConstraint * qlpMakeAlterTableAlterConstraint( stlParseParam      * aParam,
                                                                        stlInt32             aNodePos,
                                                                        qlpList            * aTableName,
                                                                        qlpDropConstObject * aConstObject,
                                                                        qlpConstraintAttr  * aConstAttr );

inline qlpDropConstObject * qlpMakeDropConstObject( stlParseParam    * aParam,
                                                    stlInt32           aNodePos,
                                                    qlpDropConstType   aDropConstType,
                                                    qlpList          * aConstraintName,
                                                    qlpList          * aUniqueColumnList );

inline qlpAlterTablePhysicalAttr * qlpMakeAlterTablePhysicalAttr( stlParseParam * aParam,
                                                                  stlInt32        aNodePos,
                                                                  qlpList       * aTableName,
                                                                  qlpList       * aRelaionAttrs );

inline qlpAlterTableRenameColumn * qlpMakeAlterTableRenameColumn( stlParseParam * aParam,
                                                                  stlInt32        aNodePos,
                                                                  qlpList       * aTableName,
                                                                  qlpValue      * aOrgName,
                                                                  qlpValue      * aNewName );

inline qlpAlterTableRenameTable * qlpMakeAlterTableRenameTable( stlParseParam * aParam,
                                                                stlInt32        aNodePos,
                                                                qlpList       * aTableName,
                                                                qlpValue      * aNewName );

inline qlpAlterTableSuppLog * qlpMakeAlterTableSuppLog( stlParseParam * aParam,
                                                        stlInt32        aNodePos,
                                                        qlpList       * aTableName,
                                                        stlBool         aIsAdd );


/********************************************************************************
 * Index DDL
 ********************************************************************************/

inline void qlpSetIndexAttr( stlParseParam  * aParam,
                             qlpList        * aAttrList,
                             qlpIndexAttr   * aIndexAttr );

inline qlpIndexDef * qlpMakeIndexDef( stlParseParam  * aParam,
                                      stlInt32         aNodePos,
                                      stlBool          aIsUnique,
                                      stlInt32         aIsUniquePos,
                                      stlBool          aIsPrimary,
                                      stlBool          aIsFromConstr,
                                      qlpList        * aIndexName,
                                      qlpList        * aRelName,
                                      stlInt32         aRelPosition,
                                      qlpList        * aElems,
                                      qlpList        * aIndexAttrs,
                                      qlpValue       * aTbsName );
                                      
inline qlpDropIndex * qlpMakeDropIndex( stlParseParam    * aParam,
                                        stlInt32           aNodePos,
                                        stlBool            aIfExists,
                                        qlpList          * aName );

inline qlpAlterIndexPhysicalAttr * qlpMakeAlterIndexPhysicalAttr( stlParseParam * aParam,
                                                                  stlInt32        aNodePos,
                                                                  qlpList       * aIndexName,
                                                                  qlpList       * aPhysicalAttrs );


/********************************************************************************
 * View DDL
 ********************************************************************************/

inline qlpViewDef * qlpMakeViewDef( stlParseParam      * aParam,
                                    stlInt32             aNodePos,
                                    stlBool              aOrReplace,
                                    stlBool              aForce,
                                    qlpList            * aViewName,
                                    qlpList            * aColumnList,
                                    qlpQueryExpr       * aViewQuery,
                                    stlInt32             aViewColStart,
                                    stlInt32             aViewColLen,
                                    stlInt32             aViewQueryStart,
                                    stlInt32             aViewQueryLen );

inline qlpDropView * qlpMakeDropView( stlParseParam    * aParam,
                                      stlInt32           aNodePos,
                                      stlBool            aIfExists,
                                      qlpList          * aViewName );

inline qlpAlterView * qlpMakeAlterView( stlParseParam    * aParam,
                                        stlInt32           aNodePos,
                                        qlpList          * aViewName,
                                        qlpValue         * aAlterAction );

/********************************************************************************
 * Sequence DDL
 ********************************************************************************/


inline qlpSequenceDef * qlpMakeSequenceDef( stlParseParam      * aParam,
                                            stlInt32             aNodePos,
                                            qlpList            * aName,
                                            qlpSequenceOption  * aIdentSeqOptions );

inline qlpDropSequence * qlpMakeDropSequence( stlParseParam    * aParam,
                                              stlInt32           aNodePos,
                                              stlBool            aIfExists,
                                              qlpList          * aName );

inline qlpAlterSequence * qlpMakeAlterSequence( stlParseParam      * aParam,
                                                stlInt32             aNodePos,
                                                qlpList            * aName,
                                                qlpSequenceOption  * aIdentSeqOptions );

/********************************************************************************
 * Synonym DDL
 ********************************************************************************/


inline qlpSynonymDef * qlpMakeSynonymDef( stlParseParam      * aParam,
                                          stlInt32             aNodePos,
                                          stlBool              aOrReplace,
                                          stlBool              aIsPublic,  
                                          qlpList            * aSynonymName,
                                          qlpList            * aObjectName );

inline qlpDropSynonym * qlpMakeDropSynonym( stlParseParam      * aParam,
                                            stlInt32             aNodePos,
                                            stlBool              aIsPublic,
                                            stlBool              aIfExists,
                                            qlpList            * aSynonymName );

/********************************************************************************
 * Query & DML
 ********************************************************************************/

inline qlpSelect * qlpMakeSelect( stlParseParam      * aParam,
                                  stlInt32             aNodePos,
                                  qllNode            * aQueryNode,
                                  qlpUpdatableClause * aUpdatability );

inline qlpUpdatableClause * qlpMakeUpdatableClause( stlParseParam        * aParam,
                                                    stlInt32               aNodePos,
                                                    qlpUpdatableMode       aUpdateMode,
                                                    qlpList              * aUpdateColList,
                                                    qlpForUpdateLockWait * aLockWait );

inline qlpForUpdateLockWait * qlpMakeForUpdateLockWait( stlParseParam            * aParam,
                                                        stlInt32                   aNodePos,
                                                        qlpForUpdateLockWaitMode   aLockWaitMode,
                                                        qlpValue                 * aWaitSecond );

inline qlpSelect * qlpMakeSelectInto( stlParseParam      * aParam,
                                      stlInt32             aNodePos,
                                      qllNode            * aQueryNode,
                                      qlpIntoClause      * aInto,
                                      qlpUpdatableClause * aUpdatability );

inline qlpIntoClause * qlpMakeIntoClause( stlParseParam    * aParam,
                                          stlInt32           aNodePos,
                                          qlpList          * mParameters );

inline qlpInsDelReturn * qlpMakeInsDelReturn( stlParseParam  * aParam,
                                              stlInt32         aNodePos,
                                              qlpList        * aReturnTarget,
                                              qlpIntoClause  * aIntoTarget );

inline qlpUpdateReturn * qlpMakeUpdateReturn( stlParseParam  * aParam,
                                              stlInt32         aNodePos,
                                              stlBool          aIsReturnNew,
                                              qlpList        * aReturnTarget,
                                              qlpIntoClause  * aIntoTarget );

inline qlpInsert * qlpMakeInsert( stlParseParam    * aParam,
                                  stlInt32           aNodePos,
                                  qlpRelInfo       * aRelation,
                                  qlpInsertSource  * aSource,
                                  qlpInsDelReturn  * aInsertReturn );

inline qlpDelete * qlpMakeDelete( stlParseParam    * aParam,
                                  stlInt32           aNodePos,
                                  qlpRelInfo       * aRelation,
                                  qlpList          * aHints,
                                  qllNode          * aWhere,
                                  qllNode          * aResultSkip,
                                  qllNode          * aResultLimit,
                                  qlpInsDelReturn  * aDeleteReturn );

inline qlpDelete * qlpMakeDeletePositionedCursor( stlParseParam    * aParam,
                                                  stlInt32           aNodePos,
                                                  qlpList          * aHints,
                                                  qlpRelInfo       * aRelation,
                                                  qlpValue         * aCursorName );

inline qlpUpdate * qlpMakeUpdate( stlParseParam    * aParam,
                                  stlInt32           aNodePos,
                                  qlpRelInfo       * aRelation,
                                  qlpList          * aHints,
                                  qlpList          * aSet,
                                  qllNode          * aWhere,
                                  qllNode          * aResultSkip,
                                  qllNode          * aResultLimit,
                                  qlpUpdateReturn  * aUpdateReturn );

inline qlpUpdate * qlpMakeUpdatePositionedCursor( stlParseParam    * aParam,
                                                  stlInt32           aNodePos,
                                                  qlpList          * aHints,
                                                  qlpRelInfo       * aRelation,
                                                  qlpList          * aSet,
                                                  qlpValue         * aCursorName );

inline qlpSavepoint * qlpMakeSavepoint( stlParseParam  * aParam,
                                        stlInt32         aNodePos,
                                        stlChar        * aName,
                                        stlInt32         aNamePos );

inline qlpSavepoint * qlpMakeReleaseSavepoint( stlParseParam  * aParam,
                                               stlInt32         aNodePos,
                                               stlChar        * aName,
                                               stlInt32         aNamePos );

inline qlpCommit * qlpMakeCommit( stlParseParam  * aParam,
                                  stlInt32         aNodePos,
                                  qlpValue       * aForce,
                                  qlpValue       * aWriteMode,
                                  qlpValue       * aComment );

inline qlpRollback * qlpMakeRollback( stlParseParam  * aParam,
                                      stlInt32         aNodePos,
                                      qlpValue       * aForce,
                                      qlpValue       * aComment,
                                      stlChar        * aName,
                                      stlInt32         aNamePos );

inline qlpLockTable * qlpMakeLockTable( stlParseParam  * aParam,
                                        stlInt32         aNodePos,
                                        qlpList        * aRelations,
                                        qlpValue       * aLockMode,
                                        stlInt32         aLockModePos,
                                        qlpValue       * aWaitTime );


/* primitive */
inline qllNode * qlpAddQueryNodeOption( stlParseParam    * aParam,
                                        stlInt32           aNodePos,
                                        qllNode          * aQueryNode,
                                        void             * aWith,
                                        qlpList          * aOrderBy,
                                        stlInt32           aOrderByNodePos,
                                        qllNode          * aResultSkip,
                                        qllNode          * aResultLimit );

inline qllNode * qlpMakeQuerySetNode( stlParseParam     * aParam,
                                      stlInt32            aNodePos,
                                      qlpSetType          aSetType,
                                      qlpSetQuantifier    aSetQuantifier,
                                      qllNode           * aLeftQueryNode,
                                      qllNode           * aRightQueryNode,
                                      void              * aCorrespondingSpec );

inline qllNode * qlpMakeQuerySpecNode( stlParseParam  * aParam,
                                       stlInt32         aNodePos,
                                       qlpList        * aHints,
                                       qlpSetQuantifier aSetQuantifier,
                                       qlpList        * aTargets,
                                       qllNode        * aFrom,
                                       qllNode        * aWhere,
                                       qllNode        * aGroupBy,
                                       qllNode        * aHaving,
                                       qllNode        * aWindow );

inline qlpJoinTypeInfo * qlpMakeJoinTypeInfo( stlParseParam         * aParam,
                                              stlInt32                aNodePos,
                                              qlpJoinType             aJoinType );

inline qlpJoinSpecification * qlpMakeJoinSpecification( stlParseParam         * aParam,
                                                        stlInt32                aNodePos,
                                                        qlpList               * aJoinCondition,
                                                        qlpList               * aNamedColumns );

inline qllNode * qlpMakeBaseTableNode( stlParseParam  * aParam,
                                       stlInt32         aNodePos,
                                       qlpList        * aRelName,
                                       qlpValue       * aDumpOpt,
                                       qlpAlias       * aAlias );

inline qllNode * qlpMakeSubTableNode( stlParseParam  * aParam,
                                      stlInt32         aNodePos,
                                      qllNode        * aQueryNode,
                                      qlpAlias       * aAlias,
                                      qlpList        * aColAlias );

inline qllNode * qlpMakeJoinTableNode( stlParseParam         * aParam,
                                       stlInt32                aNodePos,
                                       stlBool                 aIsNatural,
                                       qlpJoinType             aJoinType,
                                       qllNode               * aLeftTableNode,
                                       qllNode               * aRightTableNode,
                                       qlpJoinSpecification  * aJoinSpec );

inline qlpRelInfo * qlpMakeRelInfo( stlParseParam  * aParam,
                                    stlInt32         aNodePos,
                                    qlpList        * aRelName,
                                    qlpAlias       * aAlias );

inline qlpAlias * qlpMakeAlias( stlParseParam  * aParam,
                                stlInt32         aNodePos,
                                qlpValue       * aAliasName );

inline qlpParameter * qlpMakeParameter( stlParseParam  * aParam,
                                        stlInt32         aNodePos,
                                        qlpParamType     aParamType,
                                        knlBindType      aInOutType,
                                        stlInt32         aParamTypePos,
                                        qlpList        * aParamName );

inline qlpFunction * qlpMakeFunction( stlParseParam   * aParam,
                                      knlBuiltInFunc    aFuncId,
                                      stlInt32          aNodePos,
                                      stlInt32          aNodeLen,
                                      qlpList         * aArgs );

inline qlpList * qlpGetFunctionArgList( qlpFunction * aFuncNode );

inline qllNode * qlpMakeOrderBy( stlParseParam   * aParam,
                                 stlInt32          aNodePos,
                                 qllNode         * aSortKey,
                                 qlpValue        * aIsAsc,
                                 qlpValue        * aIsNullsFirst );

inline qllNode * qlpMakeCaseExprForSearchedCase( stlParseParam   * aParam,
                                                 stlInt32          aNodePos,
                                                 stlInt32          aNodeLen,
                                                 qllNodeType       aNodeType,
                                                 knlBuiltInFunc    aFuncId,
                                                 qlpList         * aWhenClause,
                                                 qllNode         * aDefResult );

inline qlpFunction * qlpMakeFunctionArgs1ForCaseWhenClause( stlParseParam   * aParam,
                                                            qllNode         * aArgument,
                                                            stlInt32          aNodePos,
                                                            knlBuiltInFunc    aFunctionId );

inline qlpFunction * qlpMakeFunctionArgs2ForCaseWhenClause( stlParseParam   * aParam,
                                                            qllNode         * aLeftArgument,
                                                            qllNode         * aRightArgument,
                                                            stlInt32          aNodePos,
                                                            knlBuiltInFunc    aFunctionId );

inline qlpListFunc * qlpMakeListFunctionForCaseWhenCaluse( stlParseParam   * aParam,
                                                           qlpList         * aLeftList,
                                                           qlpList         * aRightList,
                                                           knlBuiltInFunc    aFunctionId );

inline qllNode * qlpMakeCaseExprForSimpleCase( stlParseParam   * aParam,
                                               stlInt32          aNodePos,
                                               stlInt32          aNodeLen,
                                               qllNodeType       aNodeType,
                                               knlBuiltInFunc    aFuncId,
                                               qlpList         * aExpr,
                                               qlpList         * aWhenClause,
                                               qllNode         * aDefResult );

inline qllNode * qlpMakeCaseExprForNullif( stlParseParam   * aParam,
                                           stlInt32          aNodePos,
                                           stlInt32          aNodeLen,
                                           qllNodeType       aNodeType,
                                           knlBuiltInFunc    aFuncId,
                                           qlpList         * aExpr );

inline qllNode * qlpMakeCaseExprForCoalesce( stlParseParam   * aParam,
                                             stlInt32          aNodePos,
                                             stlInt32          aNodeLen,
                                             qllNodeType       aNodeType,
                                             knlBuiltInFunc    aFuncId,
                                             qlpList         * aExpr );

inline qllNode * qlpMakeCaseExprForDecode( stlParseParam   * aParam,
                                           stlInt32          aNodePos,
                                           stlInt32          aNodeLen,
                                           qllNodeType       aNodeType,
                                           knlBuiltInFunc    aFuncId,
                                           qlpList         * aExpr );

inline qllNode * qlpMakeCaseExprForNvl( stlParseParam   * aParam,
                                        stlInt32          aNodePos,
                                        stlInt32          aNodeLen,
                                        qllNodeType       aNodeType,
                                        knlBuiltInFunc    aFuncId,
                                        qlpList         * aExpr );

inline qllNode * qlpMakeCaseExprForCase2( stlParseParam   * aParam,
                                          stlInt32          aNodePos,
                                          stlInt32          aNodeLen,
                                          qllNodeType       aNodeType,
                                          knlBuiltInFunc    aFuncId,
                                          qlpList         * aExpr );

inline qllNode * qlpMakeCaseWhenClause( stlParseParam   * aParam,
                                        qllNode         * aWhen,
                                        qllNode         * aThen );

inline qllNode * qlpMakeGroupBy( stlParseParam   * aParam,
                                 stlInt32          aNodePos,
                                 qlpList         * aGroupingElemList );

inline qllNode * qlpMakeGroupingElem( stlParseParam   * aParam,
                                      stlInt32          aNodePos,
                                      qlpGroupingType   aGroupingType,
                                      qlpList         * aGroupingElem );

inline qllNode * qlpMakeHaving( stlParseParam   * aParam,
                                stlInt32          aNodePos,
                                qllNode         * aCondition );

inline qlpListCol * qlpMakeListColumn( stlParseParam       * aParam,
                                       stlInt32              aNodePos,
                                       knlBuiltInFunc        aFunc,
                                       dtlListPredicate      aPredicate,
                                       qlpList             * aList  );

inline qlpListFunc * qlpMakeListFunction( stlParseParam       * aParam,
                                          stlInt32              aNodePos,
                                          stlInt32              aNodeLen,
                                          stlInt32              aOperatorPos,
                                          knlBuiltInFunc        aFunc,
                                          qlpList             * aList );

inline qlpRowExpr * qlpMakeRowExpr( stlParseParam       * aParam,
                                    stlInt32              aNodePos,
                                    qlpList             * aList  );

inline qlpValue * qlpMakeQuantifier( stlParseParam       * aParam,
                                     stlInt32              aNodePos,
                                     knlBuiltInFunc        aFuncId,
                                     qlpQuantifier         aQuantifier  );
/* parse tree node */
inline qlpColumnDef * qlpMakeColumnDef( stlParseParam    * aParam,
                                        stlInt32           aNodePos,
                                        stlChar          * aColumnName,
                                        stlInt32           aColumnNamePos,
                                        qlpTypeName      * aTypeName,
                                        qlpList          * aConstraints,
                                        qlpColDefSecond  * aColDefSecond );

inline qlpConstraint * qlpMakeNullConst( stlParseParam  * aParam,
                                         stlInt32         aNodePos,
                                         stlInt32         aConstTypePos );

inline qlpConstraint * qlpMakeNotNullConst( stlParseParam  * aParam,
                                            stlInt32         aNodePos,
                                            stlInt32         aConstTypePos );

inline qlpConstraint * qlpMakeUniqueConst( stlParseParam      * aParam,
                                           stlInt32             aNodePos,
                                           stlInt32             aConstTypePos,
                                           qlpList            * aUniqueFields,
                                           qlpValue           * aIndexName,
                                           qlpList            * aIndexAttrList,
                                           qlpObjectName      * aIndexSpace );

inline qlpConstraint * qlpMakePKConst( stlParseParam      * aParam,
                                       stlInt32             aNodePos,
                                       stlInt32             aConstTypePos,
                                       qlpList            * aPKFields,
                                       qlpValue           * aIndexName,
                                       qlpList            * aIndexAttrList,
                                       qlpObjectName      * aIndexSpace );

inline qlpConstraint * qlpMakeFKConst( stlParseParam      * aParam,
                                       stlInt32             aNodePos,
                                       stlInt32             aConstTypePos,
                                       qlpRelInfo         * aPKTable,
                                       qlpList            * aPKFields,
                                       qlpList            * aFKFields,
                                       qlpFKMatchType       aFKMatchType,
                                       stlInt32             aFKMatchTypePos,
                                       qlpFKAction          aFKUpdateAction,
                                       stlInt32             aFKUpdateActionPos,
                                       qlpFKAction          aFKDeleteAction,
                                       stlInt32             aFKDeleteActionPos );

inline qlpConstraint * qlpMakeCheckConst( stlParseParam      * aParam,
                                          stlInt32             aNodePos,
                                          stlInt32             aConstTypePos,
                                          qllNode            * aExpr );

inline qlpTypeName * qlpMakeStaticTypeName( stlParseParam  * aParam,
                                            stlInt32         aNodePos,
                                            stlInt32         aNodeLen,
                                            dtlDataType      aDBType,
                                            stlInt32         aDBTypePos );

inline qlpTypeName * qlpMakeCharTypeName( stlParseParam        * aParam,
                                          stlInt32               aNodePos,
                                          stlInt32               aNodeLen,
                                          dtlDataType            aDBType,
                                          stlInt32               aDBTypePos,
                                          qlpCharLength        * aStrLen,
                                          stlChar              * aCharSet,
                                          stlInt32               aCharSetPos );

inline qlpTypeName * qlpMakeBinaryTypeName( stlParseParam  * aParam,
                                            stlInt32         aNodePos,
                                            stlInt32         aNodeLen,
                                            dtlDataType      aDBType,
                                            stlInt32         aDBTypePos,
                                            qlpValue       * aBinaryLen );

inline qlpTypeName * qlpMakeNumericTypeName( stlParseParam  * aParam,
                                             stlInt32         aNodePos,
                                             stlInt32         aNodeLen,
                                             dtlDataType      aDBType,
                                             stlInt32         aDBTypePos,
                                             stlInt32         aRadix,
                                             qlpValue       * aPrecision,
                                             qlpValue       * aScale );

inline qlpTypeName * qlpMakeDateTimeTypeName( stlParseParam  * aParam,
                                              stlInt32         aNodePos,
                                              stlInt32         aNodeLen,
                                              dtlDataType      aDBType,
                                              stlInt32         aDBTypePos,
                                              qlpValue       * aSecondPrec );

inline qlpTypeName * qlpMakeIntervalTypeName( stlParseParam  * aParam,
                                              stlInt32         aNodePos,
                                              stlInt32         aNodeLen,
                                              stlInt32         aDBTypePos,
                                              qllNode        * aQualifier );

inline qlpValue * qlpMakeFieldStringConstant( stlParseParam              * aParam,
                                              stlInt32                     aNodePos,
                                              stlChar                    * aStringBuf,
                                              stlChar                    * aLowStringBuf,
                                              stlChar                    * aResultBuf );

inline qlpQualifier * qlpMakeQualifier( stlParseParam         * aParam,
                                        stlInt32                aNodePos,
                                        qlpIntvPriFieldDesc   * aStartFieldDesc,
                                        qlpIntvPriFieldDesc   * aEndFieldDesc,
                                        qlpIntvPriFieldDesc   * aSingleFieldDesc );

inline qlpIntvPriFieldDesc * qlpMakeIntvPriFieldDesc( stlParseParam   * aParam,
                                                      stlInt32          aNodePos,
                                                      qlpValue        * aPrimaryFieldType,
                                                      qlpValue        * aLeadingFieldPrecision,
                                                      qlpValue        * aFractionalSecondsPrecision );

inline qlpConstantValue * qlpMakeConstant( stlParseParam  * aParam,
                                           stlInt32         aNodePos,
                                           qlpValue       * aValue );

inline qlpTypeCast * qlpMakeTypeCast( stlParseParam  * aParam,
                                      stlInt32         aNodePos,
                                      qllNode        * aExpr,
                                      qlpTypeName    * aTypename );
                                           
inline qlpDefElem * qlpMakeDefElem( stlParseParam  * aParam,
                                    stlInt32         aNodePos,
                                    stlInt32         aElementType,
                                    qllNode        * aArgument );

inline qllNode * qlpGetElemArgument( stlParseParam  * aParam,
                                     qlpDefElem     * aElement );

inline qlpTarget * qlpMakeTarget( stlParseParam  * aParam,
                                  stlInt32         aNodePos,
                                  stlChar        * aName,
                                  stlInt32         aNamePos,
                                  qlpList        * aDestSubscripts,
                                  qllNode        * aExpr,
                                  qlpColumnRef   * aUpdateColumnRef );

inline qlpTarget * qlpMakeTargetAsterisk( stlParseParam  * aParam,
                                          stlInt32         aNodePos,
                                          stlChar        * aName,
                                          stlInt32         aNamePos,
                                          qlpList        * aDestSubscripts,
                                          qlpList        * aRelName,
                                          qlpList        * aColumnNameList,
                                          qllNode        * aExpr );

inline qlpColumnRef * qlpMakeColumnRef( stlParseParam  * aParam,
                                        stlInt32         aNodePos,
                                        qlpList        * aFields,
                                        stlBool          aIsSetOuterMark );

inline qlpIndexScan * qlpMakeIndexScan( stlParseParam  * aParam,
                                        stlInt32         aNodePos,
                                        qlpList        * aName );

inline qlpIndexElement * qlpMakeIndexElement( stlParseParam  * aParam,
                                              stlInt32         aNodePos,
                                              qlpValue       * aName,
                                              qlpValue       * aIsAsc,
                                              qlpValue       * aIsNullsFirst,
                                              qlpValue       * aIsNullable );

    
/* value node */
inline qlpValue * qlpMakeIntParamConstant( stlParseParam  * aParam,
                                           stlInt32         aNodePos,
                                           stlInt64         aInteger );

inline qlpValue * qlpMakeStrConstant( stlParseParam  * aParam,
                                      stlInt32         aNodePos,
                                      stlChar        * aString );

inline qlpValue * qlpMakeSensitiveString( stlParseParam  * aParam,
                                          qlpValue       * aStringValue );

inline qlpValue * qlpMakeNumberConstant( stlParseParam  * aParam,
                                         stlInt32         aNodePos,
                                         stlChar        * aNumericStr );

inline qlpValue * qlpMakeBitStrConstant( stlParseParam  * aParam,
                                         stlInt32         aNodePos,
                                         stlChar        * aBitStr );

inline qlpValue * qlpMakeNullConstant( stlParseParam  * aParam,
                                       stlInt32         aNodePos );

inline qlpValue * qlpMakeDateTimeConstant( stlParseParam  * aParam,
                                           stlInt32         aNodePos,
                                           qllNodeType      aConstantType,
                                           stlChar        * aString );

inline void qlpCheckIntervalLiteralEmptyString( stlParseParam  * aParam,
                                                stlInt32         aNodePos,
                                                stlChar        * aString );
    
/* attr info */

inline qlpColDefSecond * qlpMakeColDefSecond( stlParseParam      * aParam,
                                              stlInt32             aNodePos,
                                              qllNode            * aRawDefault,
                                              qlpIdentityType      aIdentType,
                                              stlInt32             aIdentTypePos,
                                              qlpSequenceOption  * aIdentSeqOptions,
                                              qllNode            * aGenerationExpr );

inline qlpDatafile * qlpMakeDatafile( stlParseParam  * aParam,
                                      stlInt32         aNodePos,
                                      qlpValue       * aFilePath,
                                      qlpSize        * aSize,
                                      qlpValue       * aReuse,
                                      qlpAutoExtend  * aAutoExtend );

inline qlpAutoExtend * qlpMakeAutoExtend( stlParseParam    * aParam,
                                          stlInt32           aNodePos,
                                          qlpValue         * aIsAutoOn,
                                          qlpSize          * aNextSize,
                                          qlpSize          * aMaxSize );

inline void qlpSetTablespaceAttr( stlParseParam      * aParam,
                                  qlpTablespaceAttr  * aTbsAttr,
                                  qlpList            * aValue );

inline qlpSequenceOption * qlpMakeSequenceOption( stlParseParam   * aParam,
                                                  qlpList         * aOptions );

inline qlpTablespaceAttr * qlpMakeTablespaceAttr( stlParseParam      * aParam,
                                                  stlInt32             aNodePos );

/* set parameter list */
inline void qlpAddParamList( stlParseParam    * aParam,
                             qlpParameter     * aParameter );

/* set alias list */
inline void qlpAddAliasList( stlParseParam  * aParam,
                             qlpAlias       * aAlias );

/* get name : internal function */
inline qlpObjectName * qlpMakeObjectName( stlParseParam  * aParam,
                                          qlpList        * aName );

inline qlpOrdering * qlpMakeOrdering( stlParseParam * aParam,
                                      stlInt32        aNodePos,
                                      qlpObjectName * aObjectName,
                                      stlBool         aIsFixedPosition,
                                      stlBool         aIsLeft );

inline qlpColumnName * qlpMakeColumnName( stlParseParam  * aParam,
                                          qlpList        * aName );

inline qlpInsertSource * qlpMakeInsertSource( stlParseParam  * aParam,
                                              stlInt32         aNodePos,
                                              stlBool          aIsAllColumn,
                                              stlBool          aIsDefault,
                                              qlpList        * aColumns,
                                              qlpList        * aValues,
                                              qllNode        * aSubquery );

inline qlpList * qlpMakeMultipleSetColumn( stlParseParam    * aParam,
                                           stlInt32           aNodePos,
                                           qlpList          * aColumns,
                                           qlpList          * aValues );
                                           
inline qlpDefault * qlpMakeDefault( stlParseParam  * aParam,
                                    stlInt32         aNodePos,
                                    stlBool          aIsRowValue,
                                    stlInt32         aIsRowValuePos );

inline qlpRefTriggerAction * qlpMakeRefTriggerAction( stlParseParam  * aParam,
                                                      stlInt32         aNodePos,
                                                      qlpValue       * aUpdateAction,
                                                      qlpValue       * aDeleteAction );

inline qlpCharLength * qlpMakeCharLength( stlParseParam        * aParam,
                                          stlInt32               aNodePos,
                                          dtlStringLengthUnit    aLengthUnit,
                                          stlInt32               aLengthUnitPos,
                                          qlpValue             * aLength );


inline qlpConstraintAttr * qlpMakeConstraintAttr( stlParseParam        * aParam,
                                                  stlInt32               aNodePos,
                                                  qlpValue             * aInitDeferred,
                                                  qlpValue             * aDeferrable,
                                                  qlpValue             * aEnforce );

inline qlpConstraintAttr * qlpAppendConstraintAttr( stlParseParam     * aParam,
                                                    qlpConstraintAttr * aFirstConstAttr,
                                                    qlpConstraintAttr * aNextConstAttr );

inline qlpHintTable * qlpMakeHintTable( stlParseParam     * aParam,
                                        stlInt32            aNodePos,
                                        qlpTableHintType    aTableHintType,
                                        qlpObjectName     * aObjectName );

inline qlpHintIndex * qlpMakeHintIndex( stlParseParam     * aParam,
                                        stlInt32            aNodePos,
                                        qlpIndexHintType    aIndexHintType,
                                        qlpObjectName     * aObjectName,
                                        qlpList           * aIndexScanList );

inline qlpHintJoinOrder * qlpMakeHintJoinOrder( stlParseParam         * aParam,
                                                stlInt32                aNodePos,
                                                qlpJoinOrderHintType    aJoinOrderHintType,
                                                qlpList               * aObjectNameList );

inline qlpHintJoinOper * qlpMakeHintJoinOper( stlParseParam       * aParam,
                                              stlInt32              aNodePos,
                                              qlpJoinOperHintType   aJoinOperHintType,
                                              stlBool               aIsNoUse,
                                              qlpList             * aObjectNameList );

inline qlpHintQueryTransform * qlpMakeHintQueryTransform( stlParseParam             * aParam,
                                                          stlInt32                    aNodePos,
                                                          qlpQueryTransformHintType   aQueryTransformHintType );

inline qlpHintOther * qlpMakeHintOther( stlParseParam       * aParam,
                                        stlInt32              aNodePos,
                                        stlBool               aIsNoUse,
                                        qlpObjectName       * aObjectName,
                                        qlpOtherHintType      aOtherHintType );

inline qlpValueExpr * qlpMakeValueExpr( stlParseParam  * aParam,
                                        stlInt32         aNodePos,
                                        stlInt32         aNodeLen,
                                        qllNode        * aExpr );

inline void qlpCheckIdentifierLength( stlParseParam   * aParam,
                                      qlpValue        * aStrValue );

inline void qlpCheckDuplicateOptions( stlParseParam   * aParam,
                                      qlpList         * aList,
                                      qlpDefElem      * aOption );

inline void qlpCheckDuplicateConstraints( stlParseParam   * aParam,
                                          qlpList         * aList,
                                          qlpConstraint   * aConstr );

inline void qlpCheckNodeType( stlParseParam   * aParam,
                              qllNode         * aNode,
                              stlInt32          aType );

inline void qlpAddConstraintCharacteristic( stlParseParam     * aParam,
                                            qlpConstraint     * aConstr,
                                            qlpConstraintAttr * aConstAttr );

inline qlpSize * qlpMakeSize( stlParseParam  * aParam,
                              stlInt32         aNodePos,
                              stlBool          aIsUnlimited,
                              qlpValue       * aSizeUnit,
                              qlpValue       * aSize );

inline qlpSetParamAttr * qlpMakeSetParamAttr( stlParseParam * aParam,
                                              stlInt32        aNodePos,
                                              stlBool         aIsDeferred,
                                              qlpValue      * aScope );

inline qlpSetParam * qlpMakeSetParameter( stlParseParam   * aParam,
                                          stlInt32          aNodePos,
                                          qlpValue        * aPropertyName,
                                          qlpValue        * aSetValue,
                                          qlpSetParamAttr * aAtrr );

inline qlpAlterSystemSessName * qlpMakeAlterSystemSessName( stlParseParam       * aParam,
                                                            stlInt32              aNodePos,
                                                            qlpValue            * aSessName );

inline qlpAlterSystem * qlpMakeAlterSystemSet( stlParseParam  * aParam,
                                               qllNodeType      aNodeType,
                                               stlInt32         aNodePos,
                                               qlpSetParam    * aSetStruct );

inline qlpAlterSystem * qlpMakeAlterSystemReset( stlParseParam  * aParam,
                                                 qllNodeType      aNodeType,
                                                 stlInt32         aNodePos,
                                                 qlpValue       * aPropertyName );

inline qlpBreakpointAction * qlpMakeBreakpointAction( stlParseParam           * aParam,
                                                      stlInt32                  aNodePos,
                                                      knlBreakPointAction       aActionType,
                                                      knlBreakPointPostAction   aPostActionType,
                                                      qlpValue                * aArgument );

inline qlpBreakpointOption * qlpMakeBreakpointOption( stlParseParam       * aParam,
                                                      stlInt32              aNodePos,
                                                      qlpValue            * aSkipCount,
                                                      qlpBreakpointAction * aAction );

inline qlpAlterSystemBreakpoint * qlpMakeAlterSystemBreakpoint( stlParseParam         * aParam,
                                                                stlInt32                aNodePos,
                                                                knlBreakPointSessType   aSessType,
                                                                qlpValue              * aBreakpointName,
                                                                qlpValue              * aOwnerSessName,
                                                                qlpBreakpointOption   * aBreakpointOption );

inline qlpAlterSystemAger * qlpMakeAlterSystemAger( stlParseParam  * aParam,
                                                    stlInt32         aNodePos,
                                                    qlpAgerBehavior  aBehavior );

inline qlpAlterSystemStartupDatabase * qlpMakeAlterSystemStartupDatabase( stlParseParam   * aParam,
                                                                          stlInt32          aNodePos,
                                                                          knlStartupPhase   aStartupPhase,
                                                                          qlpValue        * aOpenOption,
                                                                          qlpValue        * aLogOption );

inline qlpAlterSystemShutdownDatabase * qlpMakeAlterSystemShutdownDatabase( stlParseParam    * aParam,
                                                                            stlInt32           aNodePos,
                                                                            knlShutdownPhase   aShutdownPhase,
                                                                            qlpValue         * aCloseOption );

inline qlpAlterSystemSwitchLogfile * qlpMakeAlterSystemSwitchLogfile( stlParseParam    * aParam,
                                                                      stlInt32           aNodePos,
                                                                      qllNodeType        aNodeType );

inline qlpAlterSystemChkpt * qlpMakeAlterSystemChkpt( stlParseParam  * aParam,
                                                      stlInt32         aNodePos );

inline qlpAlterSystemFlushLog * qlpMakeAlterSystemFlushLog( stlParseParam       * aParam,
                                                            stlInt32              aNodePos,
                                                            qlpFlushLogBehavior   aBehavior );

inline qlpAlterSystemCleanupPlan * qlpMakeAlterSystemCleanupPlan( stlParseParam  * aParam,
                                                                  stlInt32         aNodePos );

inline qlpAlterSystemCleanupSession * qlpMakeAlterSystemCleanupSession( stlParseParam  * aParam,
                                                                        stlInt32         aNodePos );

inline qlpAlterSystemDisconnSession * qlpMakeAlterSystemDisconnectSession( stlParseParam  * aParam,
                                                                           stlInt32         aNodePos,
                                                                           qlpValue       * aSessionID,
                                                                           qlpValue       * aSerialNum,
                                                                           stlBool          aIsPostTransaction,
                                                                           qlpValue       * aOption );

inline qlpAlterSystemKillSession * qlpMakeAlterSystemKillSession( stlParseParam  * aParam,
                                                                  stlInt32         aNodePos,
                                                                  qlpValue       * aSessionID,
                                                                  qlpValue       * aSerialNum );

inline qlpAlterSession * qlpMakeAlterSession( stlParseParam  * aParam,
                                              qllNodeType      aNodeType,
                                              stlInt32         aNodePos,
                                              qlpValue       * aPropertyName,
                                              qlpValue       * aSetValue );

inline qlpPseudoCol * qlpMakePseudoColumn( stlParseParam   * aParam,
                                           knlPseudoCol      aPseudoColId,
                                           stlInt32          aNodePos,
                                           stlInt32          aNodeLen,
                                           qlpList         * aArgs );

inline qlpAggregation * qlpMakeAggregation( stlParseParam       * aParam,
                                            stlInt32              aNodePos,
                                            knlBuiltInAggrFunc    aAggrId,
                                            qlpSetQuantifier      aSetQuantifier,
                                            qlpList             * aArgs,
                                            qllNode             * aFilter );

inline qlpCommentObject * qlpMakeCommentObject( stlParseParam * aParam,
                                                ellObjectType   aObjectType,
                                                stlInt32        aNodePos,
                                                qlpValue      * aNonSchemaObjectName,
                                                qlpList       * aSchemaObjectName,
                                                qlpList       * aColumnObjectName );

inline qlpCommentOnStmt * qlpMakeCommentOnStmt( stlParseParam    * aParam,
                                                stlInt32           aNodePos,
                                                qlpCommentObject * aCommentObject,
                                                qlpValue         * aCommentString );

inline qlpSetConstraint * qlpMakeSetConstraint( stlParseParam        * aParam,
                                                stlInt32               aNodePos,
                                                qllSetConstraintMode   aConstMode,
                                                qlpList              * aConstNameList );

inline qlpTransactionMode * qlpMakeSetTransactionMode( stlParseParam      * aParam,
                                                       stlInt32             aNodePos,
                                                       qlpTransactionAttr   aMode,   
                                                       qlpValue           * aValue );

inline qlpSetTransaction * qlpMakeSetTransaction( stlParseParam      * aParam,
                                                  stlInt32             aNodePos,
                                                  qlpTransactionMode * aTransactionMode );

inline qlpSetSessionCharacteristics *
qlpMakeSetSessionCharacteristics( stlParseParam      * aParam,
                                  stlInt32             aNodePos,
                                  qlpTransactionMode * aTransactionMode );

inline qlpSetSessionAuth * qlpMakeSetSessionAuth( stlParseParam * aParam,
                                                  stlInt32        aNodePos,
                                                  qlpValue      * aUserName );

inline qlpSetTimeZone * qlpMakeSetTimeZone( stlParseParam * aParam,
                                            stlInt32        aNodePos,
                                            qlpValue      * aValue );

inline qlpDeclareCursor * qlpMakeDeclareCursorISO( stlParseParam * aParam,
                                                   stlInt32        aNodePos,
                                                   qlpValue      * aCursorName,
                                                   qlpValue      * aSensitivity,
                                                   qlpValue      * aScrollability,
                                                   qlpValue      * aHoldability,
                                                   qllNode       * aCursorQuery );

inline qlpDeclareCursor * qlpMakeDeclareCursorODBC( stlParseParam * aParam,
                                                    stlInt32        aNodePos,
                                                    qlpValue      * aCursorName,
                                                    qlpValue      * aStandardType,
                                                    qlpValue      * aHoldability,
                                                    qllNode       * aCursorQuery );

inline qlpOpenCursor * qlpMakeOpenCursor( stlParseParam * aParam,
                                          stlInt32        aNodePos,
                                          qlpValue      * aCursorName );

inline qlpCloseCursor * qlpMakeCloseCursor( stlParseParam * aParam,
                                            stlInt32        aNodePos,
                                            qlpValue      * aCursorName );

inline qlpFetchCursor * qlpMakeFetchCursor( stlParseParam       * aParam,
                                            stlInt32              aNodePos,
                                            qlpValue            * aCursorName,
                                            qlpFetchOrientation * aFetchOrient,
                                            qlpIntoClause       * aIntoList );

inline qlpFetchOrientation * qlpMakeFetchOrientation( stlParseParam       * aParam,
                                                      stlInt32              aNodePos,
                                                      qllFetchOrientation   aFetchOrient,
                                                      qllNode             * aFetchPosition );

inline qlpBetween * qlpMakeBetween( stlParseParam   * aParam,
                                    qlpListElement  * aExpr1,
                                    qlpListElement  * aExpr2,
                                    qlpBetweenType    aType );



/********************************************************************************
 * PHRASE - for syntax replacement
 ********************************************************************************/


inline qlpPhraseViewedTable * qlpMakePharseViewedTable( stlParseParam      * aParam,
                                                        stlInt32             aNodePos,
                                                        qlpList            * aViewName,
                                                        qlpList            * aColumnList );

inline qlpPhraseDefaultExpr * qlpMakePhraseDefaultExpr( stlParseParam      * aParam,
                                                        stlInt32             aNodePos,
                                                        qlpValueExpr       * aDefaultExpr );

inline void qlpCheckSyntaxForHint( stlParseParam  * aParam );



/********************************************************************************
 * for rewrite
 ********************************************************************************/

inline stlBool qlpIsExistsFunc( void  * aNode );
    

/** @} */

#endif /* _QLPPARSE_H_ */




