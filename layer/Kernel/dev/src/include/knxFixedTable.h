/*******************************************************************************
 * knxFixedTable.h
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


#ifndef _KNX_FIXEDTABLE_H_
#define _KNX_FIXEDTABLE_H_ 1

/**
 * @file knxFixedTable.h
 * @brief Kernel Layer 의 Fixed Table 을 위한 내부 처리 루틴
 */

/**
 * @defgroup knxFixedTable Fixed Table 내부 처리 루틴
 * @ingroup KNX
 * @internal
 * @{
 */

#define KNX_FXTABLE_MEM_INIT_SIZE       ( 8 * 1024 )
#define KNX_FXTABLE_MEM_NEXT_SIZE       ( 8 * 1024 )

stlStatus knxFxTableMemInitialize( knxFxTableMgr * aFxMgr,
                                   knlEnv        * aEnv );

stlStatus knxFxTableMemFinalize( knxFxTableMgr * aFxMgr,
                                 knlEnv        * aEnv );

stlStatus knxFxTableMemAlloc ( knxFxTableMgr   * aFxMgr,
                               void           ** aMemory,
                               stlInt32          aSize,
                               knlEnv          * aEnv );


stlStatus knxInitializeFxTableMgr( knlEnv * aKnlEnv );
stlStatus knxFinalizeFxTableMgr( knlEnv * aKnlEnv );

stlStatus knxRegisterFixedTable( knlFxTableDesc * aFxTableDesc,
                                 knlEnv         * aKnlEnv );

stlStatus knxBuildFxRecord( knlFxColumnDesc   * aFxColumnDesc,
                            void              * aStructData,
                            knlValueBlockList * aValueList,
                            stlInt32            aBlockIdx,
                            knlEnv            * aKnlEnv );

stlStatus knxGetFixedTableCount( stlInt32 * aFxTableCount,
                                 knlEnv   * aKnlEnv );

stlStatus knxGetNthFixedTableDesc( void    ** aFxTableHeapDesc,
                                   stlInt32   aIdx,
                                   knlEnv   * aKnlEnv );

stlStatus knxGetFixedTableUsageType( void                * aFxTableHeapDesc,
                                     knlFxTableUsageType * aFxUsageType,
                                     knlEnv              * aKnlEnv );

stlStatus knxGetFixedTableUsablePhase( void            * aFxTableHeapDesc,
                                       knlStartupPhase * aUsableStartupPhase,
                                       knlEnv          * aKnlEnv );

stlStatus knxGetFixedTablePathCtrlSize( void     * aFxTableHeapDesc,
                                        stlInt32 * aFxPathCtrlSize,
                                        knlEnv   * aKnlEnv );
    
stlStatus knxGetFixedTableName( void     * aFxTableHeapDesc,
                                stlChar ** aFxTableName,
                                knlEnv   * aKnlEnv );

stlStatus knxGetFixedTableComment( void     * aFxTableHeapDesc,
                                   stlChar ** aFxTableComment,
                                   knlEnv   * aKnlEnv );

stlStatus knxGetFixedColumnCount( void             * aFxTableHeapDesc,
                                  stlInt32         * aFxColumnCount,
                                  knlEnv           * aKnlEnv );

stlStatus knxGetFixedColumnDesc( void             * aFxTableHeapDesc,
                                 knlFxColumnDesc ** aFxColumnDesc,
                                 knlEnv           * aKnlEnv );

stlStatus knxGetDataTypeIDOfFixedColumn( knlFxColumnDesc * aFxColumnDesc,
                                         dtlDataType     * aDtlDataTypeID,
                                         knlEnv          * aKnlEnv );

stlStatus knxGetDumpObjHandleCallback( void                     * aFxTableHeapDesc,
                                       knlGetDumpObjectCallback * aGetDumpCallback,
                                       knlEnv                   * aKnlEnv );

stlStatus knxGetFixedTableOpenCallback( void                   * aFxTableHeapDesc,
                                        knlOpenFxTableCallback * aOpenCallback,
                                        knlEnv                 * aKnlEnv );

stlStatus knxGetFixedTableCloseCallback( void                    * aFxTableHeapDesc,
                                         knlCloseFxTableCallback * aCloseCallback,
                                         knlEnv                  * aKnlEnv );

stlStatus knxGetFixedTableBuildRecordCallback( void                     * aFxTableHeapDesc,
                                               knlBuildFxRecordCallback * aBuildCallback,
                                               knlEnv                   * aKnlEnv );

stlStatus knxTokenizeNextDumpOption( stlChar  ** aDumpOptionStr,
                                     stlChar  ** aToken1,
                                     stlChar  ** aToken2,
                                     stlChar  ** aToken3,
                                     stlInt32  * aTokenCount,
                                     knlEnv    * aEnv );
                                     
/** @} */

#endif /* _KNX_FIXEDTABLE_H_ */
