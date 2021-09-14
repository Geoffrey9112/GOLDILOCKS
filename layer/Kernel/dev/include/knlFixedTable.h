/*******************************************************************************
 * knlFixedTable.h
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


#ifndef _KNLFIXEDTABLE_H_
#define _KNLFIXEDTABLE_H_ 1

/**
 * @file knlFixedTable.h
 * @brief Kernel Layer Fixed Table 관리 루틴
 */

/**
 * @defgroup knlFixedTable Fixed Table
 * @ingroup knExternal
 * @see Fixed Table 생성 예제: knxBuiltInFixedTable.c
 *      <BR> X$TABLES 
 *      <BR> X$COLUMNS
 * @remarks
 * <BR> Fixed Table은 시스템 내부의 자료 구조를 Relation 형태로 표현하기 위한 루틴이다.
 * <BR> Fixed Table을 통해 내부 자료 구조를 SQL을 통해 모니터링 할 수 있는 편의를 제공한다.
 * <BR> Fixed Table이라는 용어는 Oracle에서 차용한 것으로,
 * <BR> DBA를 포함한 어떤 유저도 스키마 정보를 변경하거나 데이터를 변경할 수 없다는 의미로 
 * <BR> 정의된 용어이다.
 * - Fixed Table의 용도에 따른 구분
 *  - Fixed Table 은 용도에 따라 다음과 같이 구분한다.
 *   - Basic Fixed Table
 *    - 테이블 이름만으로 자료구조를 식별가능한 Fixed Table
 *    - 일반적으로 말하는 fixed table
 *   - Dump Fixed Table
 *    - 자료구조는 동일하나, 다양한 객체들이 존재하여 Identifier가 필요한 Fixed Table
 *    - dump table이라고 간략히 명명.
 *  - 이러한 구분은 Query Processor가
 *    normal table, fixed table, dump table을 구별하기 위해 필요하다.
 *  <table>
 *     <tr>
 *       <td> 구분 </td>
 *       <td> Fixed Table </td>
 *       <td> Dump Table </td>
 *     </tr>
 *     <tr>
 *       <td> Naming Rule </td>
 *       <td> X$table_name </td>
 *       <td> D$structure_name(identifier_name) </td>
 *     </tr>
 *     <tr>
 *       <td> 예시 </td>
 *       <td> SELECT * FROM X$SESSION </td>
 *       <td> SELECT * FROM D$MEMORY_INDEX(t1_index) </td>
 *     </tr>
 *  </table>
 * @note
 * - Fixed Table은 과도한 메모리 사용이나, 메모리 Fragementation 방지를 위해
 * materialization 방식이 아닌 pipelining 방식으로 구현하자.
 * - Fixed Table의 dictionary 정보를 dictionary table 또는 dictionary cache 상에 관리하여
 *   동일한 방식으로 관리하고 싶었음.
 *  - 그러나, 다음과 같은 치명적인 난제가 있음.
 *   - Fixed Table에 대한 정의는 프로세스마다 다를 수 있음.
 *    - Gliese .vs. Goldilocks
 *    - Debugging을 Fixed Table 추가 또는 변경 등등.
 *   - 이를 Dictionary Table 또는 Dictionary Cache에서 관리할 때,
 *   <BR> Process가 attach 될때마다 검증하는 것도 어려운 요소이나,
 *   <BR> 동일한 Fixed Table에 대하여 Process간에 서로 다른 정의가 되어 있을 때 해결할 방법이 없음.
 *  - 하나의 Process에서 접근 가능한 테이블 종류
 *  <BR> = Dictionary Table상에 존재하는 Normal Table + Process 상에 존재하는 Fixed Table
 *  <BR> 로 정의하고 Execution Library Layer 단에서 접근 방식을 일원화하여
 *  <BR> 접근 방식의 일관성을 어느 정도 유지하고
 *  <BR> 구분이 반드시 필요한 곳에서만 Fixed Table과 Normal Table을 구분하도록 해야겠음.
 * - Data Type Layer는 Kernel layer의 상위에 위치하지 않는다.
 * @{
 */

stlStatus knlInitializeFxTableMgr( knlEnv * aKnlEnv );

stlStatus knlFinalizeFxTableMgr( knlEnv * aKnlEnv );

void knlSetDumpTableNameHelper( knlDumpNameHelper * aDumpHelper );

stlStatus knlRegisterFxTable( knlFxTableDesc * aFxTableDesc,
                              knlEnv         * aKnlEnv );

stlStatus knlBuildFxRecord( knlFxColumnDesc   * aFxColumnDesc,
                            void              * aStructData,
                            knlValueBlockList * aValueList,
                            stlInt32            aBlockIdx,
                            knlEnv            * aKnlEnv );

stlStatus knlGetFixedTableCount( stlInt32 * aFxTableCount,
                                 knlEnv   * aKnlEnv );

stlStatus knlGetFixedTableUsageType( void                * aFxTableHeapDesc,
                                     knlFxTableUsageType * aFxUsageType,
                                     knlEnv              * aKnlEnv );

stlStatus knlGetFixedTableUsablePhase( void            * aFxTableHandle,
                                       knlStartupPhase * aUsableStartupPhase,
                                       knlEnv          * aKnlEnv );

stlStatus knlGetFixedTablePathCtrlSize( void     * aFxTableHandle,
                                        stlInt32 * aFxPathCtrlSize,
                                        knlEnv   * aKnlEnv );

stlStatus knlGetFixedTableName( void     * aFxTableHeapDesc,
                                stlChar ** aFxTableName,
                                knlEnv   * aKnlEnv );

stlStatus knlGetFixedTableComment( void     * aFxTableHeapDesc,
                                   stlChar ** aFxTableComment,
                                   knlEnv   * aKnlEnv );

stlStatus knlGetFixedColumnCount( void             * aFxTableHeapDesc,
                                  stlInt32         * aFxColumnCount,
                                  knlEnv           * aKnlEnv );

stlStatus knlGetFixedColumnDesc( void             * aFxTableHeapDesc,
                                 knlFxColumnDesc ** aFxColumnDesc,
                                 knlEnv           * aKnlEnv );

stlStatus knlGetDataTypeIDOfFixedColumn( knlFxColumnDesc * aFxTableHeapDesc,
                                         dtlDataType     * aDtlDataTypeID,
                                         knlEnv          * aKnlEnv );

stlStatus knlGetDumpObjHandleCallback( void                     * aFxTableHandle,
                                       knlGetDumpObjectCallback * aGetDumpCallback,
                                       knlEnv                   * aKnlEnv );

stlStatus knlGetFixedTableOpenCallback( void                    * aFxTableHandle,
                                        knlOpenFxTableCallback  * aOpenCallback,
                                        knlEnv                  * aKnlEnv );

stlStatus knlGetFixedTableCloseCallback( void                    * aFxTableHandle,
                                         knlCloseFxTableCallback * aCloseCallback,
                                         knlEnv                  * aKnlEnv );

stlStatus knlGetFixedTableBuildRecordCallback( void                     * aFxTableHandle,
                                               knlBuildFxRecordCallback * aBuildCallback,
                                               knlEnv                   * aKnlEnv );

stlStatus knlTokenizeNextDumpOption( stlChar  ** aDumpOptionStr,
                                     stlChar  ** aToken1,
                                     stlChar  ** aToken2,
                                     stlChar  ** aToken3,
                                     stlInt32  * aTokenCount,
                                     knlEnv    * aEnv );

void * knlGetFxRelationHandle( stlInt32 aHandleIdx );
void * knlGetFxTableHeapDesc( void * aFxRelationHandle );


stlStatus knlBuildHeapFixedTableDescMap( knlEnv * aEnv );
stlStatus knlBuildSharedFixedRelationHeader( knlEnv * aEnv );

/** @} */
    

#endif /* _KNLFIXEDTABLE_H_ */
