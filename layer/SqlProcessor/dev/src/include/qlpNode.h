/*******************************************************************************
 * qlpNode.h
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

#ifndef _QLPNODE_H_
#define _QLPNODE_H_ 1

/**
 * @file qlpNode.h
 * @brief SQL Processor Layer Node Definition
 */


/**
 * @defgroup qlpNode Node Definition
 * @ingroup qlInternal
 * @{
 */


/*******************************************************************************
 * NODE STRUCTURES 
 ******************************************************************************/


/* executor */

/* plan */

/* plan state */

/* primitive */
typedef struct qlpQuerySetNode       qlpQuerySetNode;
typedef struct qlpQuerySpecNode      qlpQuerySpecNode;
typedef struct qlpJoinTypeInfo       qlpJoinTypeInfo;
typedef struct qlpJoinSpecification  qlpJoinSpecification;
typedef struct qlpBaseTableNode      qlpBaseTableNode;
typedef struct qlpSubTableNode       qlpSubTableNode;
typedef struct qlpJoinTableNode      qlpJoinTableNode;
typedef struct qlpRelInfo            qlpRelInfo;
typedef struct qlpAlias              qlpAlias;
typedef struct qlpParameter          qlpParameter;
typedef struct qlpIntoClause         qlpIntoClause;
typedef struct qlpInsDelReturn       qlpInsDelReturn;
typedef struct qlpUpdateReturn       qlpUpdateReturn;
typedef struct qlpFunction           qlpFunction;
typedef struct qlpBooleanExpr        qlpBooleanExpr;
typedef struct qlpBooleanTest        qlpBooleanTest;
typedef struct qlpDefault            qlpDefault;
typedef struct qlpForUpdateLockWait  qlpForUpdateLockWait;
typedef struct qlpUpdatableClause    qlpUpdatableClause;
typedef struct qlpPseudoCol          qlpPseudoCol;
typedef struct qlpAggregation        qlpAggregation;
typedef struct qlpCommentObject      qlpCommentObject;
typedef struct qlpIndexAttr          qlpIndexAttr;
typedef struct qlpDropConstObject    qlpDropConstObject;
typedef struct qlpOrderBy            qlpOrderBy;
typedef struct qlpGroupBy            qlpGroupBy;
typedef struct qlpGroupingElem       qlpGroupingElem;
typedef struct qlpHaving             qlpHaving;

typedef struct qlpListFunc           qlpListFunc;
typedef struct qlpListCol            qlpListCol;
typedef struct qlpRowExpr            qlpRowExpr;


/* expr state */

/* planner */

/* memory */

/* value */
typedef struct qlpValue              qlpValue;

/* list */ 

/* statement */

typedef struct qlpSchemaDef                qlpSchemaDef;
typedef struct qlpDropSchema               qlpDropSchema;

typedef struct qlpTableDef                 qlpTableDef;
typedef struct qlpDropTable                qlpDropTable;
typedef struct qlpTruncateTable            qlpTruncateTable;
typedef struct qlpAlterTableAddColumn      qlpAlterTableAddColumn;  
typedef struct qlpAlterTableDropColumn     qlpAlterTableDropColumn;  
typedef struct qlpAlterTableAlterColumn    qlpAlterTableAlterColumn;  
typedef struct qlpAlterTableAddConstraint  qlpAlterTableAddConstraint;
typedef struct qlpAlterTableDropConstraint qlpAlterTableDropConstraint;
typedef struct qlpAlterTableAlterConstraint qlpAlterTableAlterConstraint;
typedef struct qlpAlterTablePhysicalAttr   qlpAlterTablePhysicalAttr;  
typedef struct qlpAlterTableRenameColumn   qlpAlterTableRenameColumn;
typedef struct qlpAlterTableSuppLog        qlpAlterTableSuppLog;
typedef struct qlpAlterTableRenameTable    qlpAlterTableRenameTable;



typedef struct qlpAlterDatabaseAddLogfileGroup    qlpAlterDatabaseAddLogfileGroup;
typedef struct qlpAlterDatabaseAddLogfileMember   qlpAlterDatabaseAddLogfileMember;
typedef struct qlpAlterDatabaseDropLogfileGroup   qlpAlterDatabaseDropLogfileGroup;
typedef struct qlpAlterDatabaseDropLogfileMember  qlpAlterDatabaseDropLogfileMember;
typedef struct qlpAlterDatabaseRenameLogfile      qlpAlterDatabaseRenameLogfile;
typedef struct qlpAlterDatabaseArchivelogMode     qlpAlterDatabaseArchivelogMode;
typedef struct qlpAlterDatabaseBackup             qlpAlterDatabaseBackup;
typedef struct qlpAlterDatabaseDeleteBackupList   qlpAlterDatabaseDeleteBackupList;
typedef struct qlpIncrementalBackupOption         qlpIncrementalBackupOption;
typedef struct qlpBackupCtrlfile                  qlpBackupCtrlfile;
typedef struct qlpRestoreCtrlfile                 qlpRestoreCtrlfile;
typedef struct qlpAlterDatabaseIrrecoverable      qlpAlterDatabaseIrrecoverable;
typedef struct qlpAlterDatabaseRecover            qlpAlterDatabaseRecover;
typedef struct qlpAlterDatabaseRestore            qlpAlterDatabaseRestore;
typedef struct qlpAlterDatabaseRecoverTablespace  qlpAlterDatabaseRecoverTablespace;
typedef struct qlpAlterDatabaseRecoverDatafile    qlpAlterDatabaseRecoverDatafile;
typedef struct qlpAlterDatabaseRestoreTablespace  qlpAlterDatabaseRestoreTablespace;
typedef struct qlpAlterDatabaseClearPassHistory   qlpAlterDatabaseClearPassHistory;
typedef struct qlpImrOption                       qlpImrOption;
typedef struct qlpInteractiveImrCondition         qlpInteractiveImrCondition;
typedef struct qlpRestoreOption                   qlpRestoreOption;

typedef struct qlpDatafileList                    qlpDatafileList;

typedef struct qlpUserDef               qlpUserDef;
typedef struct qlpDropUser              qlpDropUser;
typedef struct qlpAlterUser             qlpAlterUser;

typedef struct qlpPasswordParameter     qlpPasswordParameter;
typedef struct qlpProfileDef            qlpProfileDef;
typedef struct qlpDropProfile           qlpDropProfile;
typedef struct qlpAlterProfile          qlpAlterProfile;

typedef struct qlpPrivAction            qlpPrivAction;
typedef struct qlpPrivObject            qlpPrivObject;
typedef struct qlpPrivilege             qlpPrivilege;
typedef struct qlpGrantPriv             qlpGrantPriv;
typedef struct qlpRevokePriv            qlpRevokePriv;



typedef struct qlpTablespaceDef         qlpTablespaceDef;
typedef struct qlpDropTablespace        qlpDropTablespace;
typedef struct qlpAlterSpaceAddFile     qlpAlterSpaceAddFile;
typedef struct qlpAlterTablespaceBackup qlpAlterTablespaceBackup;
typedef struct qlpAlterSpaceDropFile    qlpAlterSpaceDropFile;
typedef struct qlpAlterSpaceOnline      qlpAlterSpaceOnline;
typedef struct qlpAlterSpaceOffline     qlpAlterSpaceOffline;
typedef struct qlpAlterSpaceRenameFile  qlpAlterSpaceRenameFile;
typedef struct qlpAlterSpaceRename      qlpAlterSpaceRename;

typedef struct qlpIndexDef                 qlpIndexDef;
typedef struct qlpDropIndex                qlpDropIndex;
typedef struct qlpAlterIndexPhysicalAttr   qlpAlterIndexPhysicalAttr;  

typedef struct qlpViewDef               qlpViewDef;
typedef struct qlpDropView              qlpDropView;
typedef struct qlpAlterView             qlpAlterView;

typedef struct qlpSequenceDef           qlpSequenceDef;
typedef struct qlpDropSequence          qlpDropSequence;
typedef struct qlpAlterSequence         qlpAlterSequence;

typedef struct qlpSynonymDef            qlpSynonymDef;
typedef struct qlpDropSynonym           qlpDropSynonym;

typedef struct qlpSelect                qlpSelect;
typedef struct qlpQueryExpr             qlpQueryExpr;
typedef struct qlpSavepoint             qlpSavepoint;
typedef struct qlpCommit                qlpCommit;
typedef struct qlpRollback              qlpRollback;
typedef struct qlpInsert                qlpInsert;
typedef struct qlpUpdate                qlpUpdate;
typedef struct qlpDelete                qlpDelete;
typedef struct qlpLockTable             qlpLockTable;
typedef struct qlpSetParam              qlpSetParam;
typedef struct qlpSetParamAttr          qlpSetParamAttr;
typedef struct qlpAlterSystem           qlpAlterSystem;
typedef struct qlpAlterSystemAger       qlpAlterSystemAger;
typedef struct qlpAlterSystemBreakpoint qlpAlterSystemBreakpoint;
typedef struct qlpAlterSystemDatabase   qlpAlterSystemDatabase;
typedef struct qlpAlterSystemSwitchLogfile  qlpAlterSystemSwitchLogfile;
typedef struct qlpAlterSystemChkpt      qlpAlterSystemChkpt;
typedef struct qlpAlterSystemFlushLog   qlpAlterSystemFlushLog;
typedef struct qlpAlterSystemCleanupPlan      qlpAlterSystemCleanupPlan;
typedef struct qlpAlterSystemCleanupSession   qlpAlterSystemCleanupSession;
typedef struct qlpAlterSystemSessName   qlpAlterSystemSessName;
typedef struct qlpAlterSession          qlpAlterSession;
typedef struct qlpCommentOnStmt         qlpCommentOnStmt;
typedef struct qlpSetConstraint         qlpSetConstraint;
typedef struct qlpSetTransaction        qlpSetTransaction;
typedef struct qlpSetTimeZone           qlpSetTimeZone;
typedef struct qlpSetSessionAuth        qlpSetSessionAuth;

typedef struct qlpAlterSystemStartupDatabase  qlpAlterSystemStartupDatabase;
typedef struct qlpAlterSystemShutdownDatabase qlpAlterSystemShutdownDatabase;
typedef struct qlpAlterSystemDisconnSession   qlpAlterSystemDisconnSession;
typedef struct qlpAlterSystemKillSession      qlpAlterSystemKillSession;
typedef struct qlpSetSessionCharacteristics   qlpSetSessionCharacteristics;
typedef struct qlpDeclareCursor               qlpDeclareCursor;
typedef struct qlpOpenCursor                  qlpOpenCursor;
typedef struct qlpCloseCursor                 qlpCloseCursor;
typedef struct qlpFetchCursor                 qlpFetchCursor;

typedef struct qlpBetween             qlpBetween;

/* phrase for syntax replacement */
typedef struct qlpPhraseViewedTable   qlpPhraseViewedTable;
typedef struct qlpPhraseDefaultExpr   qlpPhraseDefaultExpr;

/* parse tree */
typedef struct qlpConstantValue       qlpConstantValue;
typedef struct qlpColumnDef           qlpColumnDef;
typedef struct qlpConstraint          qlpConstraint;
typedef struct qlpTypeName            qlpTypeName;
typedef struct qlpQualifier           qlpQualifier;
typedef struct qlpIntvPriFieldDesc    qlpIntvPriFieldDesc;
typedef struct qlpTypeCast            qlpTypeCast;
typedef struct qlpDefElem             qlpDefElem; 
typedef struct qlpTarget              qlpTarget;
typedef struct qlpRowTarget           qlpRowTarget;
typedef struct qlpAsterisk            qlpAsterisk;
typedef struct qlpHint                qlpHint;
typedef struct qlpColumnRef           qlpColumnRef;
typedef struct qlpIndexScan           qlpIndexScan;
typedef struct qlpIndexElement        qlpIndexElement;
typedef struct qlpFetchOrientation    qlpFetchOrientation;

/* etc */
typedef struct qlpList                qlpList;
typedef struct qlpParseContext        qlpParseContext; 

/* for parser */
typedef struct qlpObjectName          qlpObjectName;
typedef struct qlpOrdering            qlpOrdering;
typedef struct qlpColumnName          qlpColumnName;
typedef struct qlpCharLength          qlpCharLength;
typedef struct qlpColDefSecond        qlpColDefSecond;
typedef struct qlpAlterIdentity       qlpAlterIdentity;  
typedef struct qlpRefTriggerAction    qlpRefTriggerAction;
typedef struct qlpHintTable           qlpHintTable;
typedef struct qlpHintIndex           qlpHintIndex;
typedef struct qlpHintJoinOrder       qlpHintJoinOrder;
typedef struct qlpHintJoinOper        qlpHintJoinOper;
typedef struct qlpHintQueryTransform  qlpHintQueryTransform;
typedef struct qlpHintOther           qlpHintOther;
typedef struct qlpDatafile            qlpDatafile;
typedef struct qlpAutoExtend          qlpAutoExtend;
typedef struct qlpTablespaceAttr      qlpTablespaceAttr;
typedef struct qlpSequenceOption      qlpSequenceOption;
typedef struct qlpNOT                 qlpNOT;
typedef struct qlpInsertSource        qlpInsertSource;
typedef struct qlpConstraintAttr      qlpConstraintAttr;
typedef struct qlpValueExpr           qlpValueExpr;
typedef struct qlpSize                qlpSize;
typedef struct qlpTransactionMode     qlpTransactionMode;
typedef struct qlpBreakpointAction    qlpBreakpointAction;
typedef struct qlpBreakpointOption    qlpBreakpointOption;
typedef struct qlpCaseExpr            qlpCaseExpr;
typedef struct qlpCaseWhenClause      qlpCaseWhenClause;


/*******************************************************************************
 * FUNCTIONS
 ******************************************************************************/



/** @} */

#endif /* _QLPNODE_H_ */

