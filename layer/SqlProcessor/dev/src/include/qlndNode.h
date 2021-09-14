/*******************************************************************************
 * qlndNode.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: qlndNode.h 11012 2014-01-21 06:43:00Z jhkim $
 *
 * NOTES
 *
 *
 ******************************************************************************/

#ifndef _QLNDNODE_H_
#define _QLNDNODE_H_ 1

/**
 * @file qlndNode.h
 * @brief SQL Processor Layer Data Optimization Node
 */

#include <qlDef.h>

/**
 * @addtogroup qlnd
 * @{
 */


/*********************************************************
 * COMMON
 ********************************************************/

stlStatus qlndBuildRefBlockList( qllStatement          * aSQLStmt,
                                 qllDataArea           * aDataArea,
                                 qloInitStmtExprList   * aStmtExprList,
                                 qlvRefExprList        * aValueExprList,
                                 knlValueBlockList    ** aValueBlockList,
                                 knlRegionMem          * aRegionMem,
                                 qllEnv                * aEnv );

stlStatus qlndBuildTableColBlockList( qllDataArea         * aDataArea,
                                      qlvRefExprList      * aValueExprList,
                                      stlBool               aIsReadBlock,
                                      knlValueBlockList  ** aValueBlockList,
                                      knlRegionMem        * aRegionMem,
                                      qllEnv              * aEnv );

stlStatus qlndBuildTableRowIdColBlockList( qllDataArea         * aDataArea,
                                           qlvInitExpression   * aRowIdExpr,
                                           knlValueBlockList  ** aRowIdBlockList,
                                           knlRegionMem        * aRegionMem,
                                           qllEnv              * aEnv );

stlStatus qlndBuildIndexColBlockList( qllDataArea         * aDataArea,
                                      qlvRefExprList      * aIndexColExprList,
                                      ellDictHandle       * aIndexHandle,
                                      knlValueBlockList  ** aIndexColBlockList,
                                      knlRegionMem        * aRegionMem,
                                      qllEnv              * aEnv );

stlStatus qlndBuildPseudoColBlockList( qllDataArea           * aDataArea,
                                       qlvRefExprList        * aPseudoColExprList,
                                       knlValueBlockList    ** aValueBlockList,
                                       knlRegionMem          * aRegionMem,
                                       qllEnv                * aEnv );

stlStatus qlndSetMergeRecordInfo( qllStatement         * aSQLStmt,
                                  qlnfAggrOptInfo      * aAggrOptInfo,
                                  qlvRefExprList       * aAggrFuncList,
                                  qlvInitInstantNode   * aIniInstant,
                                  smlMergeRecordInfo  ** aMergeRecord,
                                  qllDataArea          * aDataArea,
                                  knlRegionMem         * aRegionMem,
                                  qllEnv               * aEnv );

stlStatus qlndSetConflictCheckInfo( qllStatement            * aSQLStmt,
                                    stlInt32                  aAggrDistFuncCnt,
                                    stlInt32                  aAggrFuncCnt,
                                    qlnfAggrOptInfo         * aAggrOptInfo,
                                    stlInt32                  aKeyColCnt,
                                    knlValueBlockList       * aKeyColList,
                                    smlMergeRecordInfo      * aMergeRecord,
                                    qllDataArea             * aDataArea,
                                    qlnxConflictCheckInfo  ** aConflictCheckInfo,
                                    knlRegionMem            * aRegionMem,
                                    qllEnv                  * aEnv );

stlStatus qlndSetOuterColumnValue( qllDataArea  * aDataArea,
                                   stlInt16       aNodeIdx,
                                   stlInt32       aBlockIdx,
                                   qllEnv       * aEnv );

stlStatus qlndBuildOuterColBlockList( qllDataArea         * aDataArea,
                                      qlvRefExprList      * aValueExprList,
                                      knlValueBlockList  ** aOuterColBlockList,
                                      knlValueBlockList  ** aOuterOrgColBlockList,
                                      knlRegionMem        * aRegionMem,
                                      qllEnv              * aEnv );

stlStatus qlndSetRowBlockOnJoinNode( knlRegionMem       * aRegionMem,
                                     stlInt32             aBlockAllocCount,
                                     qlnxRowBlockList   * aLeftRowBlockList,
                                     qlnxRowBlockList   * aRightRowBlockList,
                                     qlnxRowBlockList   * aResultRowBlockList,
                                     qllEnv             * aEnv );

stlStatus qlndMakePrepareStackEvalInfo( knlExprContextInfo  * aExprDataContext,
                                        knlExprStack        * aFilterStack,
                                        knlExprEvalInfo    ** aFilterEvalInfo,
                                        knlRegionMem        * aRegionMem,
                                        qllEnv              * aEnv );

stlStatus qlndAllocPseudoBlockList( qllStatement        * aSQLStmt,
                                    qlvRefExprList      * aPseudoColList,
                                    stlInt32              aBlockCnt,
                                    knlValueBlockList  ** aPseudoBlockList,
                                    knlRegionMem        * aRegionMem,
                                    qllEnv              * aEnv );

stlStatus qlndAllocRowIdColumnBlockList( qllStatement       * aSQLStmt,
                                         qloColumnBlockList * aRowIdColumnBlockList,
                                         qlvRefExprList     * aRowIdColumnExprList,
                                         stlInt32             aBlockCnt,
                                         knlRegionMem       * aRegionMem,
                                         qllEnv             * aEnv );

stlStatus qlndShareRowIdColumnBlockListForUpdate( qloColumnBlockList * aRowIdColumnBlockList,
                                                  qlvInitNode        * aReadTableNode,
                                                  qlvInitNode        * aWriteTableNode,
                                                  qllEnv             * aEnv );

stlStatus qlndMakePhysicalFilter( qlvInitNode          * aRelationNode,
                                  qlvInitExpression    * aFilterExpr,
                                  knlValueBlockList    * aReadValueBlocks,
                                  knlExprContextInfo   * aExprDataContext,
                                  knlRegionMem         * aRegionMem,
                                  knlPhysicalFilter   ** aPhysicalFilter,
                                  qllEnv               * aEnv );

stlStatus qlndDataOptIntoPhrase( qllStatement        * aSQLStmt,
                                 qloExprInfo         * aQueryExprInfo,
                                 knlBindContext      * aBindContext,
                                 stlInt32              aIntoCount,
                                 qlvRefExprList      * aTargetExprList,
                                 qlvInitExpression   * aInitIntoArray,
                                 qllDataArea         * aDataArea,
                                 knlRegionMem        * aRegionMem,
                                 qllEnv              * aEnv );

stlStatus qlndShareIndexValueListForDelete( qllDBCStmt          * aDBCStmt,
                                            qllDataArea         * aDataArea,
                                            qlvRefExprList      * aReadColList,
                                            ellDictHandle       * aIndexDictHandle,
                                            knlValueBlockList  ** aIndexDeleteValueList,
                                            qllEnv              * aEnv );

stlStatus qlndShareIndexValueListForInsert( qllDBCStmt          * aDBCStmt,
                                            qllDataArea         * aDataArea,
                                            qlvRefExprList      * aReadColList,
                                            knlValueBlockList   * aTableWriteBlock,
                                            ellDictHandle       * aIndexDictHandle,
                                            knlValueBlockList  ** aIndexInsertValueList,
                                            qllEnv              * aEnv );

stlStatus qlndShareSuppleValueList( qllDBCStmt          * aDBCStmt,
                                    qllDataArea         * aDataArea,
                                    qlvRefExprList      * aReadColList,
                                    ellDictHandle       * aPrimaryKeyHandle,
                                    knlValueBlockList  ** aSuppleValueList,
                                    qllEnv              * aEnv );

stlStatus qlndShareNotNullReadColumnValueList( qllDBCStmt          * aDBCStmt,
                                               qllDataArea         * aDataArea,
                                               qlvRefExprList      * aReadColList,
                                               ellDictHandle       * aColumnHandle,
                                               knlValueBlockList  ** aNotNullValueList,
                                               qllEnv              * aEnv );

/*******************************************************************************
 * Reference Expression List
 ******************************************************************************/

stlStatus qlndCreateColumnBlockList( qloColumnBlockList  ** aColumnBlockList,
                                     knlRegionMem         * aRegionMem,
                                     qllEnv               * aEnv );

stlStatus qlndAddColumnToColumnBlockList( qllStatement        * aSQLStmt,
                                          qloDataOptExprInfo  * aDataOptInfo,
                                          qloColumnBlockList  * aColumnBlockList,
                                          qlvInitExpression   * aInitColumnExpr,
                                          stlInt32              aBlockAllocCnt,
                                          qllEnv              * aEnv );

stlStatus qlndAddColumnListToColumnBlockList( qllStatement        * aSQLStmt,
                                              qloDataOptExprInfo  * aDataOptInfo,
                                              qloColumnBlockList  * aColumnBlockList,
                                              qlvRefExprList      * aInitColumnExprList,
                                              stlInt32              aBlockAllocCnt,
                                              qllEnv              * aEnv );

stlStatus qlndFindColumnBlockList( qllDataArea          * aDataArea,
                                   qloColumnBlockList   * aColumnBlockList,
                                   qlvInitExpression    * aInitExpr,
                                   knlRegionMem         * aRegionMem,
                                   knlValueBlockList   ** aValueBlock,
                                   qllEnv               * aEnv );

stlStatus qlndFindRowIdColumnBlockList( qloColumnBlockList   * aRowIdColumnBlockList,
                                        qlvInitExpression    * aInitExpr,
                                        knlValueBlockList   ** aValueBlock,
                                        qllEnv               * aEnv );

stlStatus qlndAddLongTypeValueList( qllLongTypeValueList  * aLongTypeValueList,
                                    stlInt32                aDataValueCnt,
                                    dtlDataValue          * aDataValueArr,
                                    knlRegionMem          * aRegionMem,
                                    qllEnv                * aEnv );

stlStatus qlndFreeLongTypeValues( qllLongTypeValueList  * aLongTypeValueList,
                                  qllEnv                * aEnv );

stlStatus qlndResetLongTypeValues( qllLongTypeValueList  * aLongTypeValueList,
                                   qllEnv                * aEnv );

stlStatus qlndInitSingleDataValue( qllLongTypeValueList   * aLongTypeValueList,
                                   dtlDataType              aDBType,
                                   dtlDataValue           * aDataValue,
                                   stlInt32                 aAllocSize,
                                   knlRegionMem           * aRegionMem,
                                   qllEnv                 * aEnv );

stlStatus qlndAllocDataValueArray( qllLongTypeValueList   * aLongTypeValueList,
                                   dtlDataValue           * aDataValue,
                                   dtlDataType              aDBType,
                                   stlInt64                 aPrecision,
                                   dtlStringLengthUnit      aStringLengthUnit,
                                   stlInt32                 aArraySize,
                                   knlRegionMem           * aRegionMem,
                                   stlInt64               * aBufferSize,
                                   qllEnv                 * aEnv );

stlStatus qlndAllocDataValueArrayFromString( qllLongTypeValueList   * aLongTypeValueList,
                                             dtlDataValue           * aDataValue,
                                             dtlDataType              aDBType,
                                             stlInt64                 aPrecision,
                                             stlInt64                 aScale,
                                             dtlStringLengthUnit      aStringLengthUnit,
                                             stlChar                * aString,
                                             stlInt64                 aStringLength,
                                             stlInt32                 aArraySize,
                                             knlRegionMem           * aRegionMem,
                                             stlInt64               * aBufferSize,
                                             qllEnv                 * aEnv );

stlStatus qlndCopyBlock( qllLongTypeValueList   * aLongTypeValueList,
                         knlValueBlockList     ** aValueBlockList,
                         stlLayerClass            aAllocLayer,
                         stlInt32                 aBlockCnt,
                         knlValueBlockList      * aSourceBlockList,
                         knlRegionMem           * aRegionMem,
                         qllEnv                 * aEnv );

stlStatus qlndInitBlockList( qllLongTypeValueList   * aLongTypeValueList,
                             knlValueBlockList     ** aValueBlockList,
                             stlInt32                 aBlockCnt,
                             stlBool                  aIsSepBuff,
                             stlLayerClass            aAllocLayer,
                             stlInt64                 aTableID,
                             stlInt32                 aColumnOrder,
                             dtlDataType              aDBType,
                             stlInt64                 aArgNum1,
                             stlInt64                 aArgNum2,
                             dtlStringLengthUnit      aStringLengthUnit,
                             dtlIntervalIndicator     aIntervalIndicator,
                             knlRegionMem           * aRegionMem,
                             qllEnv                 * aEnv );


/*********************************************************
 * INDEX ACCESS
 ********************************************************/

stlStatus qlndGetRangePredInfo( qllDBCStmt       * aDBCStmt,
                                qllStatement     * aSQLStmt,
                                qllDataArea      * aDataArea,
                                qlnoIndexAccess  * aOptIndexAccess,
                                qlnxIndexAccess  * aExeIndexAccess,
                                qllEnv           * aEnv );

stlStatus qlndMakePhysicalFilterFromExprList( qlvRefExprList       * aExprList,
                                              knlValueBlockList    * aTableValueBlocks,
                                              knlValueBlockList    * aIndexValueBlocks,
                                              knlExprContextInfo   * aExprDataContext,
                                              stlBool                aIsKeyFilter,
                                              ellDictHandle        * aIndexHandle,
                                              knlRegionMem         * aRegionMem,
                                              knlPhysicalFilter   ** aPhysicalFilter,
                                              qllEnv               * aEnv );

stlStatus qlndSetFetchInfoFromRangeExpr( qllDataArea      * aDataArea,
                                         qlnoIndexAccess  * aOptIndexAccess,
                                         qlnxIndexAccess  * aExeIndexAccess,
                                         knlRegionMem     * aRegionMem,
                                         qllEnv           * aEnv );

stlStatus qlndMakeInKeyRangeInfo( qllStatement              * aSQLStmt,
                                  qllDataArea               * aDataArea,
                                  qlnoIndexAccess           * aOptIndexAccess,
                                  qlnxInKeyRangeFetchInfo  ** aInKeyRangeFetchInfo,
                                  knlRegionMem              * aRegionMem,
                                  qllEnv                    * aEnv );


/*********************************************************
 * SORT INSTANT
 ********************************************************/

stlStatus qlndGetRangePredInfoForSortInstant( qllDBCStmt       * aDBCStmt,
                                              qllStatement     * aSQLStmt,
                                              qllDataArea      * aDataArea,
                                              qlnoInstant      * aOptSortInstant,
                                              qlnxInstant      * aExeSortInstant,
                                              qllEnv           * aEnv );

stlStatus qlndMakePhysicalFilterFromExprListForSortInstant( qlvRefExprList       * aExprList,
                                                            knlValueBlockList    * aReadValueBlocks,
                                                            knlExprContextInfo   * aExprDataContext,
                                                            stlBool                aIsKeyFilter,
                                                            qlnoInstant          * aOptSortInstant,
                                                            knlRegionMem         * aRegionMem,
                                                            knlPhysicalFilter   ** aPhysicalFilter,
                                                            qllEnv               * aEnv );

stlStatus qlndGetKeyCompareListForSortInstant( qllStatement        * aSQLStmt,
                                               qlnoInstant         * aOptSortInstant,
                                               knlRegionMem        * aRegionMem,
                                               knlKeyCompareList  ** aKeyCompList,
                                               qllEnv              * aEnv );

stlStatus qlndSetFetchInfoFromRangeExprForSortInstant( qllDataArea      * aDataArea,
                                                       qlnoInstant      * aOptSortInstant,
                                                       qlnxInstant      * aExeSortInstant,
                                                       knlRegionMem     * aRegionMem,
                                                       qllEnv           * aEnv );


/*********************************************************
 * HASH INSTANT
 ********************************************************/

stlStatus qlndGetHashInfoForHashInstant( qllDBCStmt     * aDBCStmt,
                                         qllStatement   * aSQLStmt,
                                         qllDataArea    * aDataArea,
                                         qlnoInstant    * aOptHashInstant,
                                         qlnxInstant    * aExeHashInstant,
                                         qllEnv         * aEnv );


/****************************************************************************
 * SORT MERGE JOIN
 ***************************************************************************/



/****************************************************************************
 * SUB QUERY FUNCTION
 ***************************************************************************/

stlStatus qlndGetListFuncInstantFetchInfo( qllStatement                     * aSQLStmt,
                                           qllDataArea                      * aDataArea,
                                           qlnoSubQueryFunc                 * aOptSubQueryFunc,
                                           stlBool                            aNeedResNullCheck,
                                           knlValueBlockList                * aLeftValueBlockList,
                                           knlValueBlockList                * aRightValueBlockList,
                                           qlnxListFuncInstantFetchInfo    ** aListFuncInstantFetchInfo,
                                           qlnxSubQueryFuncPtr              * aSubQueryFuncPtr,
                                           knlRegionMem                     * aRegionMem,
                                           qllEnv                           * aEnv );

stlStatus qlndGetListFuncArrayFetchInfo( qllStatement                   * aSQLStmt,
                                         qllDataArea                    * aDataArea,
                                         qlnoSubQueryFunc               * aOptSubQueryFunc,
                                         stlBool                          aNeedResNullCheck,
                                         knlValueBlockList              * aRightValueBlockList,
                                         qlnxListFuncArrayFetchInfo    ** aListFuncArrayFetchInfo,
                                         qlnxSubQueryFuncPtr            * aSubQueryFuncPtr,
                                         knlRegionMem                   * aRegionMem,
                                         qllEnv                         * aEnv );


/*********************************************************
 * DELETE STMT
 ********************************************************/

stlStatus qlndAllocRelObject4Delete( qllDBCStmt             * aDBCStmt,
                                     qllDataArea            * aDataArea,
                                     ellDictHandle          * aTableHandle,
                                     qlvRefExprList         * aReadColList,
                                     qlvInitDeleteRelObject * aIniRelObject,
                                     qlxExecDeleteRelObject * aExeRelObject,
                                     qllEnv                 * aEnv );

/*********************************************************
 * INSERT STMT
 ********************************************************/

stlStatus qlndAllocRelObject4Insert( smlTransId               aTransID,
                                     qllDBCStmt             * aDBCStmt,
                                     ellDictHandle          * aTableHandle,
                                     knlValueBlockList      * aTableWriteBlock,
                                     qlvInitInsertRelObject * aIniRelObject,
                                     qlxExecInsertRelObject * aExeRelObject,
                                     qllEnv                 * aEnv );


/*********************************************************
 * UPDATE STMT
 ********************************************************/

stlStatus qlndAllocRelObject4Update( qllDBCStmt             * aDBCStmt,
                                     qllStatement           * aSQLStmt,
                                     qllDataArea            * aDataArea,
                                     ellDictHandle          * aTableHandle,
                                     qlvRefExprList         * aReadColList,
                                     knlValueBlockList      * aTableWriteBlock,
                                     qlvInitUpdateRelObject * aIniRelObject,
                                     qlxExecUpdateRelObject * aExeRelObject,
                                     qllEnv                 * aEnv );


/** @} qlnd */

#endif /* _QLNDNODE_H_ */
