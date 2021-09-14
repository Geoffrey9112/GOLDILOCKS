/*******************************************************************************
 * dtfUnixTime.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _DTF_UNIX_TIME_H_
#define _DTF_UNIX_TIME_H_ 1

/**
 * @file dtfUnixTime.h
 * @brief unix time for DataType Layer
 */

/**
 * @internal
 * @{
 */

/**
 * @brief UTC 1970-01-01 00:00:00.000000 의 stlTime
 */
#define DTF_FROM_UNIXTIME_MIN_STLTIME   ( STL_INT64_C(0) )

/**
 * @brief UTC 9999-12-31 23:59:59.999999 의 stlTime
 */
#define DTF_FROM_UNIXTIME_MAX_STLTIME   ( STL_INT64_C(253402300799999999) )

/**
 * @brief stlTime 을 1970-01-01 00:00:00.000000 의 timestamp type으로 변환했을때의 값
 */
#define DTF_FROM_UNIXTIME_MIN_TIMESTAMP ( STL_INT64_C(-946684800000000) )

/**
 * @brief stlTime 을 9999-12-31 23:59:59.999999 의 timestamp type으로 변환했을때의 값
 */
#define DTF_FROM_UNIXTIME_MAX_TIMESTAMP ( STL_INT64_C(252455615999999999) )


/*************************************************
 * FROM_UNIXTIME
 *************************************************/ 

stlStatus dtfFromUnixTime( stlUInt16        aInputArgumentCnt,
                           dtlValueEntry  * aInputArgument,
                           void           * aResultValue,
                           void           * aInfo,
                           dtlFuncVector  * aVectorFunc,
                           void           * aVectorArg,
                           void           * aEnv );


stlStatus dtfFromTimestampToStlTime( dtlTimestampType  * aTimestamp,
                                     stlTime           * aStlTime,
                                     dtlFuncVector     * aVectorFunc,
                                     void              * aVectorArg,
                                     stlErrorStack     * aErrorStack );

/** @} */
    
#endif /* _DTF_UNIX_TIME_H_ */
