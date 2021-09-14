/*******************************************************************************
 * smdmpfFxTable.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: smdmpfFxTable.h 13901 2014-11-13 01:31:33Z mkkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _SMDMPFFXTABLE_H_
#define _SMDMPFFXTABLE_H_ 1

/**
 * @file smdmpfFxTable.h
 * @brief Storage Manager Layer Memory Heap Table Fixed Table Routines
 */

/**
 * @defgroup smdmpfFxTable Memory Heap Fixed Table
 * @ingroup smdInternal
 * @{
 */
stlStatus smdmpfGetDumpObjectCallback( stlInt64   aTransID,
                                       void     * aStmt,
                                       stlChar  * aDumpObjName,
                                       void    ** aDumpObjHandle,
                                       knlEnv   * aEnv );
stlStatus smdmpfOpenOffsetTblHdrCallback( void   * aStmt,
                                          void   * aDumpObjHandle,
                                          void   * aPathCtrl,
                                          knlEnv * aEnv );
stlStatus smdmpfCloseOffsetTblHdrCallback( void   * aDumpObjHandle,
                                           void   * aPathCtrl,
                                           knlEnv * aEnv );
stlStatus smdmpfBuildRecordOffsetTblHdrCallback( void              * aDumpObjHandle,
                                                 void              * aPathCtrl,
                                                 knlValueBlockList * aValueList,
                                                 stlInt32            aBockIdx,
                                                 stlBool           * aTupleExist,
                                                 knlEnv            * aEnv );

stlStatus smdmpfOpenRowCallback( void   * aStmt,
                                 void   * aDumpObjHandle,
                                 void   * aPathCtrl,
                                 knlEnv * aEnv );

stlStatus smdmpfCloseRowCallback( void   * aDumpObjHandle,
                                  void   * aPathCtrl,
                                  knlEnv * aEnv );

stlStatus smdmpfBuildRecordRowCallback( void              * aDumpObjHandle,
                                        void              * aPathCtrl,
                                        knlValueBlockList * aValueList,
                                        stlInt32            aBlockIdx,
                                        stlBool           * aTupleExist,
                                        knlEnv            * aEnv );


/** @} */
    
#endif /* _SMDMPFFXTABLE_H_ */
