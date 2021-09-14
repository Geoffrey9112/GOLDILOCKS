/*******************************************************************************
 * dtfUnixTime.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id:$
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file dtfUnixTime.c
 * @brief unixtime Function DataType Library Layer
 */

#include <dtl.h>
#include <dtDef.h>
#include <dtfUnixTime.h>

/**
 * @ingroup dtfDateTime
 * @internal
 * @{
 */


/*******************************************************************************
 * FROM_UNIXTIME
 *   FROM_UNIXTIME( NUMBER )
 *     FROM_UNIXTIME => [ Mysql ]
 * ex) FROM_UNIXTIME( 1397462891279607 ) => 2014-04-14 17:08:11.279607
 *******************************************************************************/

/**
 * @brief FROM_UNIXTIME
 *        <BR> stlNow()값을( 1970년 1월 1일부터 계산된 microsecond값 ) 을 timestamp 로 변환
 *        <BR> FROM_UNIXTIME
 *        <BR> FROM_UNIXTIME( NUMBER )
 *        <BR>   FROM_UNIXTIME => [ Mysql ]
 *        <BR> ex) FROM_UNIXTIME( 1397462891279607 ) => 2014-04-14 17:08:11.279607
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> FROM_UNIXTIME( number )
 * @param[out] aResultValue      결과 (결과타입 TIMESTAMP)
 * @param[in]  aInfo             function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       aVectorArg
 * @param[in]  aVectorArg        aVectorArg
 * @param[in]  aEnv              environment (stlErrorStack)
 */
stlStatus dtfFromUnixTime( stlUInt16        aInputArgumentCnt,
                           dtlValueEntry  * aInputArgument,
                           void           * aResultValue,
                           void           * aInfo,
                           dtlFuncVector  * aVectorFunc,
                           void           * aVectorArg,
                           void           * aEnv )                  
{
    dtlDataValue   * sValue1 = NULL;
    dtlDataValue   * sResultValue = NULL;
    stlErrorStack  * sErrorStack = NULL;

    stlTime          sStlTime  = 0;
    stlBool          sIsTruncated = STL_FALSE;
    stlUInt8         sCarry    = 0;
    stlInt32         sExponent = 0;
    stlInt32         sDigitLen = 0;
    dtlNumericType * sNumeric  = NULL;
    stlUInt8       * sDigit    = NULL;


    sValue1 = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, sErrorStack );    
    
    DTF_CHECK_DTL_VALUE2( sValue1,
                          DTL_TYPE_FLOAT,
                          DTL_TYPE_NUMBER,
                          sErrorStack );
    
    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_TIMESTAMP, sErrorStack );        

    sNumeric = DTF_NUMERIC( sValue1 );

    /* 양수이어야 한다. */
    STL_TRY_THROW( DTL_NUMERIC_IS_POSITIVE( sNumeric ) == STL_TRUE,
                   ERROR_ARGUMENT_OUT_OF_RANGE );

    if( DTL_NUMERIC_IS_ZERO( sNumeric, sValue1->mLength ) == STL_TRUE )
    {
        sStlTime = 0;
        STL_THROW( RAMP_NUMBER_TO_STLTIME_END );
    }
    else
    {
        /* Do Nothing */
    }
        
    /* round 적용되지 않음. */
    STL_TRY( dtlNumericToUInt64( sValue1,
                                 & sStlTime,
                                 & sIsTruncated,
                                 sErrorStack )
             == STL_SUCCESS );

    /* round 계산 , stlUInt64 범위이내어야 한다. */
    sDigitLen = DTL_NUMERIC_GET_DIGIT_COUNT( sValue1->mLength );
    sExponent = DTL_NUMERIC_GET_EXPONENT( sNumeric );
    sDigit    = & sNumeric->mData[1];
        
    if( sExponent < 0 )
    {
        if( DTD_NUMERIC_GET_POS_DIGIT_STRING( *sDigit )[0] > '4' )
        {
            sCarry = 1;
        }
        else
        {
            /* Do Nothing */
        }
    }
    else if( sDigitLen > sExponent + 1 )
    {
        if( DTD_NUMERIC_GET_POS_DIGIT_STRING( sDigit[sExponent + 1] )[0] > '4' )
        {
            sCarry = 1;
        }
        else
        {
            /* Do Nothing */
        }
    }
    else
    {
        /* Do Nothing */
    }

    if( sStlTime <= STL_UINT64_MAX )
    {
        if( sCarry == 1 )
        {
            if( sStlTime < STL_UINT64_MAX )
            {
                sStlTime++;
            }
            else
            {
                STL_THROW( ERROR_ARGUMENT_OUT_OF_RANGE );
            }
        }
        else
        {
            /* Do Noting */
        }
    }
    else
    {
        STL_THROW( ERROR_ARGUMENT_OUT_OF_RANGE );
    }
    
    STL_TRY_THROW( sStlTime <= DTF_FROM_UNIXTIME_MAX_STLTIME,
                   ERROR_ARGUMENT_OUT_OF_RANGE );
    
    STL_RAMP( RAMP_NUMBER_TO_STLTIME_END );
    
    /*
     * 현재 time zone 을 적용한 timestamp 값으로 계산.
     */
    
    STL_TRY( dtlTimestampSetValueFromStlTime( sStlTime,
                                              DTL_TIMESTAMP_MAX_PRECISION,
                                              DTL_SCALE_NA,
                                              DTL_TIMESTAMP_SIZE,
                                              sResultValue,
                                              aVectorFunc,
                                              aVectorArg,
                                              aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;
    
    STL_CATCH( ERROR_ARGUMENT_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_SQL_STRING_OUTSIDE_RANGE_DATA_TYPE_NUMBER_CONVERTED,
                      NULL,
                      sErrorStack );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;    
}


/**
 * @brief timestamp 값을 stlTime 값으로 변환한 값을 반환한다.
 * @param[in]  aTimestamp       Timestamp Value
 * @param[out] aStlTime         변환된 stlTime Value
 * @param[in]  aVectorFunc      Function Vector
 * @param[in]  aVectorArg       Vector Argument
 * @param[out] aErrorStack      에러 스택
 */
stlStatus dtfFromTimestampToStlTime( dtlTimestampType  * aTimestamp,
                                     stlTime           * aStlTime,
                                     dtlFuncVector     * aVectorFunc,
                                     void              * aVectorArg,
                                     stlErrorStack     * aErrorStack )
{
    dtlTimestampType   sTimestamp = 0;
    stlInt32           sGMTOffset;
    
    sTimestamp = *aTimestamp;
    
    /**
     * UTC TimeZone 값으로 변경
     */
    
    sGMTOffset = aVectorFunc->mGetGMTOffsetFunc(aVectorArg);
    sTimestamp = sTimestamp - (sGMTOffset * DTL_USECS_PER_SEC);
    
    /**
     * Julian Time 을 Unix Time 으로 변경
     */
    
    sTimestamp = sTimestamp + ((DTL_EPOCH_JDATE - DTL_UNIX_EPOCH_JDATE) * DTL_USECS_PER_DAY );
    
    STL_DASSERT( (sTimestamp >= DTF_FROM_UNIXTIME_MIN_STLTIME ) &&
                 (sTimestamp <= DTF_FROM_UNIXTIME_MAX_STLTIME ) );
    
    *aStlTime = sTimestamp;
    
    return STL_SUCCESS;
    
    /* STL_FINISH; */
    
    /* return STL_FAILURE; */
}

/** @} */
