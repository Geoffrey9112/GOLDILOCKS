/*******************************************************************************
 * qlgCallPhase.h
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


#ifndef _QLG_CALL_PHASE_H_
#define _QLG_CALL_PHASE_H_ 1

/**
 * @file qlgCallPhase.h
 * @brief SQL Processor Layer API Call Phase Internal Routines
 */

/*************************************************************************
 * API 단계별 수행 함수 획득
 *************************************************************************/

void qlgSetQueryPhaseMemMgr( qllDBCStmt    * aDBCStmt,
                             qllQueryPhase   aPhase,
                             qllEnv        * aEnv );

void qlgDisableQueryPhaseMemMgr( qllDBCStmt    * aDBCStmt,
                                 qllEnv        * aEnv );

qllValidateFunc    qlgGetValidateFuncSQL( qllNodeType aType ); 
qllCodeOptFunc     qlgGetCodeOptimizeFuncSQL( qllNodeType aType );  
qllDataOptFunc     qlgGetDataOptimizeFuncSQL( qllNodeType aType );   
qllExecuteFunc     qlgGetExecuteFuncSQL( qllNodeType aType );  

stlStatus          qlgCheckExecutableStatement( qllNodeType aType, qllEnv * aEnv );  

/*************************************************************************
 * API 단계별 수행 정보 누적
 *************************************************************************/

void qlgAddCallCntParse( qllEnv * aEnv );
void qlgAddCallCntValidate( qllEnv * aEnv );

void qlgAddCallCntBindContext( qllEnv * aEnv );

void qlgAddCallCntOptCode( qllEnv * aEnv );
void qlgAddCallCntOptData( qllEnv * aEnv );

void qlgAddCallCntRecheck( qllEnv * aEnv );
void qlgAddCallCntExecute( qllStatement * aSQLStmt,
                           qllEnv       * aEnv );
void qlgAddCallCntRecompile( qllEnv * aEnv );



/*************************************************************************
 * 단계별 주요 함수 
 *************************************************************************/

stlChar * qlgMakeErrPosString( stlChar * aSQLString,
                               stlInt32  aPosition,
                               qllEnv  * aEnv );
                               
stlStatus qlgParseSQL( qllDBCStmt    * aDBCStmt,
                       qllStatement  * aSQLStmt,
                       qllQueryPhase   aQueryPhase,
                       stlChar       * aSQLString,
                       stlBool         aIsRecompile,
                       stlBool         aSearchPlanCache,
                       qllNode      ** aParseTree,
                       qllEnv        * aEnv );

stlStatus qlgParseInternal( qllDBCStmt    * aDBCStmt,
                            stlChar       * aSQLString,
                            qllNode      ** aParseTree,
                            stlInt32      * aBindCount,
                            qllEnv        * aEnv );

stlStatus qlgValidateSQL( smlTransId      aTransID,
                          qllDBCStmt    * aDBCStmt,
                          qllStatement  * aSQLStmt,
                          qllQueryPhase   aQueryPhase,
                          stlChar       * aSQLString,
                          qllNode       * aParseTree,
                          qllEnv        * aEnv );

stlStatus qlgOptimizeCodeSQL( smlTransId      aTransID,
                              qllDBCStmt    * aDBCStmt,
                              qllStatement  * aSQLStmt,
                              qllQueryPhase   aQueryPhase,
                              qllEnv        * aEnv );

stlStatus qlgOptimizeDataSQL( smlTransId      aTransID,
                              qllDBCStmt    * aDBCStmt,
                              qllStatement  * aSQLStmt,
                              qllQueryPhase   aQueryPhase,
                              qllEnv        * aEnv );

stlStatus qlgInit4RecompileSQL( qllDBCStmt   * aDBCStmt,
                                qllStatement * aSQLStmt,
                                qllEnv       * aEnv );

stlStatus qlgRecompileSQL( smlTransId     aTransID,
                           qllDBCStmt   * aDBCStmt,
                           qllStatement * aSQLStmt,
                           stlBool        aSearchPlanCache,
                           qllQueryPhase  aQueryPhase,
                           qllEnv       * aEnv );

stlStatus qlgRecheckPrivSQL( smlTransId      aTransID,
                             smlStatement  * aStmt,
                             qllDBCStmt    * aDBCStmt,
                             qllStatement  * aSQLStmt,
                             qllQueryPhase   aQueryPhase,
                             qllEnv        * aEnv );

stlStatus qlgExecuteSQL( smlTransId      aTransID,
                         smlStatement  * aStmt,
                         qllDBCStmt    * aDBCStmt,
                         qllStatement  * aSQLStmt,
                         qllQueryPhase   aQueryPhase,
                         qllEnv        * aEnv );

/*************************************************************************
 * 정보 획득 함수 
 *************************************************************************/

stlBool qlgHasInsufficientPlanCacheMemoryError( qllEnv * aEnv );

stlStatus qlgGetValidCacheInitPlan( smlTransId      aTransID,
                                    qllStatement  * aSQLStmt,
                                    void          * aSqlHandle,
                                    void         ** aInitPlan,
                                    qllEnv        * aEnv );

stlStatus qlgGetValidCacheCodePlan( qllStatement  * aSQLStmt,
                                    void          * aSqlHandle,
                                    void         ** aCodePlan,
                                    qllEnv        * aEnv );

stlStatus qlgGetValidPlan4DDL( smlTransId       aTransID,
                               qllDBCStmt     * aDBCStmt,
                               qllStatement   * aSQLStmt,
                               qllQueryPhase    aQueryPhase,
                               qllEnv         * aEnv );

stlBool qlgComparePlan( knlPlanSqlInfo * aSqlInfo,
                        void           * aCodePlanA,
                        void           * aCodePlanB );

stlStatus qlgValidateBindParamDataType( stlInt32          aBindCount,
                                        knlBindContext  * aBindContext,
                                        qllEnv          * aEnv );

stlStatus qlgValidateBindParamDataValue( stlInt32          aBindCount,
                                         knlBindContext  * aBindContext,
                                         qllEnv          * aEnv );

#endif /* _QLG_CALL_PHASE_H_ */
