/*******************************************************************************
 * qllTarget.h
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

#ifndef _QLL_TARGET_H_
#define _QLL_TARGET_H_ 1

/**
 * @file qllTarget.h
 * @brief Fetch Target Information
 * ODBC, JDBC 등에서 필요로 하는 Target 관련 기초 정보를 획득한다.
 */

stlInt32  qllGetTargetCount( qllStatement * aSQLStmt );

stlStatus qllGetTargetInfo( qllStatement  * aSQLStmt,
                            knlRegionMem  * aRegionMem,
                            qllTarget    ** aTargetInfo,
                            qllEnv        * aEnv );
                            
#endif /* _QLL_TARGET_H_ */



