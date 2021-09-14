/*******************************************************************************
 * kniProcess.h
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


#ifndef _KNIPROCESS_H_
#define _KNIPROCESS_H_ 1

/**
 * @file kniProcess.h
 * @brief Kernel Layer Process Internal Routines
 */

/**
 * @defgroup kniProcess Process
 * @ingroup knInternal
 * @internal
 * @{
 */

/**
 * @brief Process Table 정보를 초기화한다.
 */
#define KNI_INIT_PROCESS_TABLE_ID()   gProcessTableID  = -1

stlStatus kniSetCurrentProcessInfo( knlEnv * aEnv );
stlStatus kniCreateProcessTable( stlInt32   aMaxProcessCount,
                                 knlEnv   * aEnv );
stlStatus kniAttachProcessTable( knlEnv * aEnv );
stlStatus kniAllocProcessTable( knlEnv * aEnv );
stlStatus kniDetachProcessTable( knlEnv * aEnv );
stlStatus kniFreeProcessTable( knlEnv * aEnv );


stlStatus openFxProcessInfoCallback( void   * aStmt,
                                     void   * aDumpObjHandle,
                                     void   * aPathCtrl,
                                     knlEnv * aEnv );
stlStatus closeFxProcessInfoCallback( void   * aDumpObjHandle,
                                      void   * aPathCtrl,
                                      knlEnv * aEnv );
stlStatus buildRecordFxProcessInfoCallback( void              * aDumpObjHandle,
                                            void              * aPathCtrl,
                                            knlValueBlockList * aValueList,
                                            stlInt32            aBlockIdx,
                                            stlBool           * aTupleExist,
                                            knlEnv            * aEnv );

/** @} */
    
#endif /* _KNIPROCESS_H_ */
