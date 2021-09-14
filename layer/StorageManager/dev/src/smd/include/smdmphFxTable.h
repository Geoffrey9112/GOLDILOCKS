/*******************************************************************************
 * smdmphFxTable.h
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


#ifndef _SMDMPHFXTABLE_H_
#define _SMDMPHFXTABLE_H_ 1

/**
 * @file smdmphFxTable.h
 * @brief Storage Manager Layer Memory Heap Table Fixed Table Routines
 */

/**
 * @defgroup smdmphFxTable Memory Heap Fixed Table
 * @ingroup smdInternal
 * @{
 */
stlStatus smdmphGetDumpObjectCallback( stlInt64   aTransID,
                                       void     * aStmt,
                                       stlChar  * aDumpObjName,
                                       void    ** aDumpObjHandle,
                                       knlEnv   * aEnv );
stlStatus smdmphOpenOffsetTblHdrCallback( void   * aStmt,
                                          void   * aDumpObjHandle,
                                          void   * aPathCtrl,
                                          knlEnv * aEnv );
stlStatus smdmphCloseOffsetTblHdrCallback( void   * aDumpObjHandle,
                                           void   * aPathCtrl,
                                           knlEnv * aEnv );
stlStatus smdmphBuildRecordOffsetTblHdrCallback( void              * aDumpObjHandle,
                                                 void              * aPathCtrl,
                                                 knlValueBlockList * aValueList,
                                                 stlInt32            aBockIdx,
                                                 stlBool           * aTupleExist,
                                                 knlEnv            * aEnv );

stlStatus smdmphOpenRowCallback( void   * aStmt,
                                 void   * aDumpObjHandle,
                                 void   * aPathCtrl,
                                 knlEnv * aEnv );

stlStatus smdmphCloseRowCallback( void   * aDumpObjHandle,
                                  void   * aPathCtrl,
                                  knlEnv * aEnv );

stlStatus smdmphBuildRecordRowCallback( void              * aDumpObjHandle,
                                        void              * aPathCtrl,
                                        knlValueBlockList * aValueList,
                                        stlInt32            aBlockIdx,
                                        stlBool           * aTupleExist,
                                        knlEnv            * aEnv );


/** @} */
    
#endif /* _SMDMPHFXTABLE_H_ */
