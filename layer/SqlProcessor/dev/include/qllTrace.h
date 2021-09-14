/*******************************************************************************
 * qllTrace.h
 *
 * Copyright (c) 2013, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id$
 *
 * NOTES
 *
 *
 ******************************************************************************/

#ifndef _QLL_TRACE_H_
#define _QLL_TRACE_H_ 1

/**
 * @file qllTrace.h
 * @brief Query Process에서 Trace Log를 남기기 위한 함수들
 */


/****************************************************************
 * Trace Logger Macro
 ****************************************************************/

#define QLL_TRACE_LOG_NAME_PREFIX               "opt"

#define QLL_GET_TRACE_LOGGER(aEnv)              (&(QLL_SESS_ENV(aEnv)->mTraceLogger))
#define QLL_IS_ENABLE_TRACE_LOGGER(aEnv)        (QLL_SESS_ENV(aEnv)->mTraceLoggerEnabled == STL_TRUE)
#define QLL_IS_INIT_TRACE_LOGGER(aEnv)          (QLL_SESS_ENV(aEnv)->mTraceLoggerInited == STL_TRUE)

#define QLL_SET_ENABLE_TRACE_LOGGER(aEnv)       QLL_SESS_ENV(aEnv)->mTraceLoggerEnabled = STL_TRUE
#define QLL_SET_DISABLE_TRACE_LOGGER(aEnv)      QLL_SESS_ENV(aEnv)->mTraceLoggerEnabled = STL_FALSE

#define QLL_SET_INIT_TRACE_LOGGER(aEnv)         QLL_SESS_ENV(aEnv)->mTraceLoggerInited = STL_TRUE
#define QLL_SET_FINI_TRACE_LOGGER(aEnv)         QLL_SESS_ENV(aEnv)->mTraceLoggerInited = STL_FALSE

#define QLL_TRACE_OPTIMIZER_DUMP_TITLE         "SQL Statement Optimizer Dump"

/****************************************************************
 * Trace Logger Functions
 ****************************************************************/

stlStatus qllTraceCreateLogger( qllEnv  * aEnv );

stlStatus qllTraceDestroyLogger( qllEnv  * aEnv );

void qllTraceHead( stlChar  * aTitle,
                   qllEnv   * aEnv );

void qllTraceTail( stlChar  * aTitle,
                   qllEnv   * aEnv );

void qllTraceMidTitle( stlChar  * aTitle,
                       qllEnv   * aEnv );

stlStatus qllTraceBody( knlRegionMem    * aRegionMem,
                        qllEnv          * aEnv,
                        const stlChar   * aFormat,
                        ... );

void qllTraceString( stlChar    * aString,
                     qllEnv     * aEnv );

#endif /* _QLL_TRACE_H_ */

