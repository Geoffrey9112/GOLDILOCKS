/*******************************************************************************
 * qlfTarget.h
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

#ifndef _QLF_TARGET_H_
#define _QLF_TARGET_H_ 1

/**
 * @file qlfTarget.h
 * @brief Fetch Target Information
 * ODBC, JDBC 등에서 필요로 하는 Target 관련 기초 정보를 획득한다.
 */


stlStatus qlfMakeQueryTarget( qllStatement   * aSQLStmt,
                              knlRegionMem   * aRegionMem,
                              qllTarget     ** aTargetList,
                              qllEnv         * aEnv );
                                
void qlfInitTarget( qllTarget * aTarget );                                

stlStatus qlfCopyTarget( qllTarget      * aSrcTargetInfo,
                         qllTarget     ** aDstTargetInfo,
                         knlRegionMem   * aRegionMem,
                         qllEnv         * aEnv );

stlStatus qlfMakeTargetInfo( qlvInitTarget       * aCodeTarget,
                             ellDictHandle       * aSchemaHandle,
                             ellDictHandle       * aTableHandle,
                             ellDictHandle       * aColumnHandle,
                             ellDictHandle       * aBaseColumnHandle,
                             knlRegionMem        * aRegionMem,
                             qllTarget           * aTarget,
                             qllEnv              * aEnv );


#endif /* _QLF_TARGET_H_ */



