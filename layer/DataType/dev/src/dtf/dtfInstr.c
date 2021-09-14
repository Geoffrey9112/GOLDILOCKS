/*******************************************************************************
 * dtfInstr.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: $
 *
 * NOTES
 *
 ******************************************************************************/

/**
 * @file dtfInstr.c
 * @brief String Function DataType Library Layer
 */

#include <dtl.h>
#include <dtDef.h>
#include <dtfString.h>
#include <dtsCharacterSet.h>

/**
 * @ingroup dtf
 * @internal
 * @{
 */

stlStatus dtfInstrGetAsciiPosition( dtlDataValue   * aStrValue,
                                    dtlDataValue   * aSubstrValue,
                                    stlInt64         aPosition,
                                    stlInt64         aPositionByteLen,
                                    stlInt64         aOccurrence,
                                    stlInt64       * aResultPosition,
                                    stlErrorStack  * aErrorStack );

stlStatus dtfInstrGetUtf8Position( dtlDataValue   * aStrValue,
                                   dtlDataValue   * aSubstrValue,
                                   stlInt64         aPosition,
                                   stlInt64         aPositionByteLen,
                                   stlInt64         aOccurrence,
                                   stlInt64       * aResultPosition,
                                   stlErrorStack  * aErrorStack );

stlStatus dtfInstrGetUhcPosition( dtlDataValue   * aStrValue,
                                  dtlDataValue   * aSubstrValue,
                                  stlInt64         aPosition,
                                  stlInt64         aPositionByteLen,
                                  stlInt64         aOccurrence,
                                  stlInt64       * aResultPosition,
                                  stlErrorStack  * aErrorStack );

stlStatus dtfInstrGetGb18030Position( dtlDataValue   * aStrValue,
                                      dtlDataValue   * aSubstrValue,
                                      stlInt64         aPosition,
                                      stlInt64         aPositionByteLen,
                                      stlInt64         aOccurrence,
                                      stlInt64       * aResultPosition,
                                      stlErrorStack  * aErrorStack );


dtfInstrGetPositionFunc dtfInstrGetPositionFuncList[ DTL_CHARACTERSET_MAX + 1 ] =
{
    dtfInstrGetAsciiPosition,
    dtfInstrGetUtf8Position,
    dtfInstrGetUhcPosition,
    dtfInstrGetGb18030Position,
    
    NULL
};


/**
 * @brief INSTR( string, substring[, position[, occurrence] ] )
 *        <BR> string의 특정한 substring의 위치를 반환
 *        <BR> INSTR( string, substring[, position[, occurrence] ] )
 *        <BR>   INSTR( string, substring[, position[, occurrence] ] ) => [ O ]
 *        <BR> ex) instr('Thomas', 'om')  =>  3
 *        <BR> ex) instr('ThomasThomas', 'om', 5 )  =>  9
 *        <BR> ex) instr('ThomasThomas', 'om', 2, 2 )  =>  9
 *   
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> instr(VARCHAR, VARCHAR, NATIVE_BIGINT, NATIVE_BIGINT)
 *                               <BR> instr(LONGVARCHAR, LONGVARCHAR, NATIVE_BIGINT, NATIVE_BIGINT)
 * @param[out] aResultValue      연산 결과 (결과타입 NUMBER)
 *                                  n: 1 base index
 *                                  0: not found
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfStringInstr( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          dtlFuncVector  * aVectorFunc,
                          void           * aVectorArg,
                          void           * aEnv )
{
    dtlDataValue  * sValue1;
    dtlDataValue  * sValue2;
    dtlDataValue  * sValue3;
    dtlDataValue  * sValue4;
    dtlDataValue  * sResultValue;
    stlErrorStack * sErrorStack;

    stlInt64        sPosition = 0;
    stlInt64        sOccurrence = 0;
    stlInt64        sBytePosition = 0;
    stlInt64        sResultPosition = 0;

    stlBool         sIsOver = STL_FALSE;

    dtlCharacterSet sCharSetID = DTL_CHARACTERSET_MAX;
    
    DTL_PARAM_VALIDATE( ( aInputArgumentCnt >= 2 ) && ( aInputArgumentCnt <= 4 ),
                        (stlErrorStack *)aEnv );

    /* character set이 추가될때 그에 따른 function list가 누락되는 일이 없도록 하기 위한 검사 코드 */
    DTF_CHECK_CHAR_SET_FUNC_LIST( dtfInstrGetPositionFuncList );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;

    sCharSetID = aVectorFunc->mGetCharSetIDFunc(aVectorArg);
    
    DTF_CHECK_DTL_GROUP2( sValue1,
                          DTL_GROUP_CHAR_STRING,
                          DTL_GROUP_LONGVARCHAR_STRING,
                          sErrorStack );
    DTF_CHECK_DTL_GROUP2( sValue2,
                          DTL_GROUP_CHAR_STRING,
                          DTL_GROUP_LONGVARCHAR_STRING,
                          sErrorStack );
    
    STL_DASSERT( sValue1->mLength > 0 );
    STL_DASSERT( sValue2->mLength > 0 );
    
    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT2( sResultValue, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );

    /*
     * INSTR( string, substring[, position[, occurrence] ] )
     * occurrence 정보
     */
    
    if( aInputArgumentCnt == 4 )
    {
        sValue4 = aInputArgument[3].mValue.mDataValue;
        DTF_CHECK_DTL_VALUE( sValue4, DTL_TYPE_NATIVE_BIGINT, sErrorStack );

        sOccurrence = DTF_BIGINT( sValue4 );

        STL_TRY_THROW( sOccurrence > 0, ERROR_ARGUMENT_OUT_OF_RANGE );
    }
    else
    {
        /* position이 생략되었을 경우, default = 1 */
        sOccurrence = 1;
    }
    
    /*
     * INSTR( string, substring[, position[, occurrence] ] )
     * position 정보
     */
    
    if( aInputArgumentCnt >= 3 )
    {
        sValue3 = aInputArgument[2].mValue.mDataValue;
        DTF_CHECK_DTL_VALUE( sValue3, DTL_TYPE_NATIVE_BIGINT, sErrorStack );

        sPosition = DTF_BIGINT( sValue3 );

        if( sPosition == 0 )
        {
            /*
             * position 의 값이 0 이면, 0 리턴 
             * INSTR( 'CORPORATE FLOOR', '0R', 0 ) => 0
             */ 
            DTL_NUMERIC_SET_ZERO( DTF_NUMERIC( sResultValue ), sResultValue->mLength );
            STL_THROW( SUCCESS_END );
        }
        else
        {
            /*
             * position > string length 이면, 0 리턴
             * INSTR( 'CORPORATE FLOOR', '0R', 16 ) => 0
             *
             * ( position + string length ) < 0 이며, 0 리턴
             * instr( 'CORPORATE FLOOR', 'C', -16, 1 ) =>  0
             */
            if( ( sPosition > sValue1->mLength ) || ( sPosition + sValue1->mLength < 0 ) )
            {
                DTL_NUMERIC_SET_ZERO( DTF_NUMERIC( sResultValue ), sResultValue->mLength );
                STL_THROW( SUCCESS_END );
            }
            else
            {
                /* Do Nothing */
            }
        }
    }
    else
    {
        /* position이 생략되었을 경우, default = 1 */
        sPosition = 1;
    }

    /*
     * position에 대한 byte 위치를 찾는다.
     */

    STL_DASSERT( sPosition != 0 );
    
    STL_TRY( dtfMbPosition2BinaryPosition( sCharSetID,
                                           DTF_VARCHAR( sValue1 ),
                                           sValue1->mLength,
                                           sPosition,
                                           &sBytePosition,
                                           &sIsOver,
                                           aVectorFunc,
                                           aVectorArg,
                                           sErrorStack )
             == STL_SUCCESS );
        
//    STL_DASSERT( sIsOver == STL_FALSE );

    if( sIsOver == STL_TRUE )
    {
        sResultPosition = 0;
        STL_THROW( RAMP_FINISH_GET_RESULT_POSITION );
    }
    else
    {
        /* Do Nothing */
    }
    
    /*
     * 각 characterset에 맞게 position에 대한 substring 위치를 찾는다.
     */
    
    STL_TRY( dtfInstrGetPositionFuncList[ sCharSetID ]( sValue1,
                                                        sValue2,
                                                        sPosition,
                                                        sBytePosition,
                                                        sOccurrence,
                                                        & sResultPosition,
                                                        sErrorStack )
             == STL_SUCCESS );
    
    /*
     * result value 구성.
     */
    
    STL_RAMP( RAMP_FINISH_GET_RESULT_POSITION );
    
    if( sResultPosition == 0 )
    {
        DTL_NUMERIC_SET_ZERO( DTF_NUMERIC( sResultValue ), sResultValue->mLength );
    }
    else
    {
        STL_TRY( dtdToNumericFromInt64( sResultPosition,
                                        gResultDataTypeDef[DTL_TYPE_NUMBER].mArgNum1,
                                        gResultDataTypeDef[DTL_TYPE_NUMBER].mArgNum2,
                                        sResultValue,
                                        sErrorStack )
                 == STL_SUCCESS );
        
    }
    
    STL_RAMP( SUCCESS_END );
    
    return STL_SUCCESS;
    
    STL_CATCH( ERROR_ARGUMENT_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_ARGUMENT_OUT_OF_RANGE,
                      NULL,
                      sErrorStack,
                      sOccurrence );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief INSTR( string, substring[, position[, occurrence] ] )
 *        <BR> string의 특정한 substring의 위치를 반환
 *        <BR> INSTR( string, substring[, position[, occurrence] ] )
 *        <BR>   INSTR( string, substring[, position[, occurrence] ] ) => [ O ]
 *        <BR> ex) instr( X'AABBCCAABBCCAABBCC', X'BBCC', 3, 2 ) => 8
 *   
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> instr(VARBINARY, VARBINARY, NATIVE_BIGINT, NATIVE_BIGINT)
 *                               <BR> instr(LONGVARBINARY, LONGVARBINARY, NATIVE_BIGINT, NATIVE_BIGINT)
 * @param[out] aResultValue      연산 결과 (결과타입 NUMBER)
 *                                  n: 1 base index
 *                                  0: not found
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfBinaryStringInstr( stlUInt16        aInputArgumentCnt,
                                dtlValueEntry  * aInputArgument,
                                void           * aResultValue,
                                dtlFuncVector  * aVectorFunc,
                                void           * aVectorArg,
                                void           * aEnv )
{
    dtlDataValue  * sValue1;
    dtlDataValue  * sValue2;
    dtlDataValue  * sValue3;
    dtlDataValue  * sValue4;
    dtlDataValue  * sResultValue;
    stlErrorStack * sErrorStack;

    stlInt64        sPosition = 0;
    stlInt64        sOccurrence = 0;
    stlInt64        sResultPosition = 0;
    stlInt64        sCmpSuccessCnt = 0;
    stlInt32        sCmpRtn = 0;
    stlInt64        sIdx = 0;
    stlInt64        sMaxIdx = 0;
    stlChar       * sString = NULL;
    stlChar       * sSubString = NULL;
    
    DTL_PARAM_VALIDATE( ( aInputArgumentCnt >= 2 ) && ( aInputArgumentCnt <= 4 ),
                        (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_GROUP2( sValue1,
                          DTL_GROUP_BINARY_STRING,
                          DTL_GROUP_LONGVARBINARY_STRING,
                          sErrorStack );
    DTF_CHECK_DTL_GROUP2( sValue2,
                          DTL_GROUP_BINARY_STRING,
                          DTL_GROUP_LONGVARBINARY_STRING,
                          sErrorStack );
    
    STL_DASSERT( sValue1->mLength > 0 );
    STL_DASSERT( sValue2->mLength > 0 );
    
    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT2( sResultValue, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );

    /*
     * INSTR( string, substring[, position[, occurrence] ] )
     * occurrence 정보
     */
    
    if( aInputArgumentCnt == 4 )
    {
        sValue4 = aInputArgument[3].mValue.mDataValue;
        DTF_CHECK_DTL_VALUE( sValue4, DTL_TYPE_NATIVE_BIGINT, sErrorStack );

        sOccurrence = DTF_BIGINT( sValue4 );

        STL_TRY_THROW( sOccurrence > 0, ERROR_ARGUMENT_OUT_OF_RANGE );
    }
    else
    {
        /* position이 생략되었을 경우, default = 1 */
        sOccurrence = 1;
    }
    
    /*
     * INSTR( string, substring[, position[, occurrence] ] )
     * position 정보
     */
    
    if( aInputArgumentCnt >= 3 )
    {
        sValue3 = aInputArgument[2].mValue.mDataValue;
        DTF_CHECK_DTL_VALUE( sValue3, DTL_TYPE_NATIVE_BIGINT, sErrorStack );

        sPosition = DTF_BIGINT( sValue3 );

        if( sPosition == 0 )
        {
            /*
             * position 의 값이 0 이면, 0 리턴 
             * INSTR( 'CORPORATE FLOOR', '0R', 0 ) => 0
             */ 
            DTL_NUMERIC_SET_ZERO( DTF_NUMERIC( sResultValue ), sResultValue->mLength );
            STL_THROW( SUCCESS_END );
        }
        else
        {
            /*
             * position > string length 이면, 0 리턴
             * INSTR( 'CORPORATE FLOOR', '0R', 16 ) => 0
             *
             * ( position + string length ) < 0 이며, 0 리턴
             * instr( 'CORPORATE FLOOR', 'C', -16, 1 ) =>  0
             */
            if( ( sPosition > sValue1->mLength ) || ( sPosition + sValue1->mLength < 0 ) )
            {
                DTL_NUMERIC_SET_ZERO( DTF_NUMERIC( sResultValue ), sResultValue->mLength );
                STL_THROW( SUCCESS_END );
            }
            else
            {
                /* Do Nothing */
            }
        }
    }
    else
    {
        /* position이 생략되었을 경우, default = 1 */
        sPosition = 1;
    }
    
    STL_DASSERT( sPosition != 0 );
    
    /*
     * position에 대한 substring 위치를 찾는다.
     */ 
    
    if( sPosition > 0 )
    {
        /*
         * substring에 대한 위치를 찾는다.
         */
        
        /* position 이 양수인 경우로, string의 left에서부터 검색 */
        /* sub string 찾는다. */
        sIdx = sPosition - 1;
        sMaxIdx = sValue1->mLength - sValue2->mLength + 1;
        sString = DTF_VARBINARY( sValue1 );
        sSubString = DTF_VARBINARY( sValue2 );
        
        while( sIdx < sMaxIdx )
        {
            /* 속도를 빠르게 하기 위해서 1byte만 먼저 비교한다. */
            if( sString[sIdx] == sSubString[0] )
            {
                sCmpRtn = stlMemcmp( sString + sIdx,
                                     sSubString,
                                     sValue2->mLength );
                
                if( sCmpRtn == 0 )
                {
                    sCmpSuccessCnt++;
                    
                    if( sCmpSuccessCnt == sOccurrence )
                    {
                        /* found it.. */
                        sResultPosition = sIdx + 1;
                        break;
                    }
                    else
                    {
                        /* Do Nothing */
                    }
                }
                else
                {
                    /* not found.. continue.. */
                    /* Do Nothing */
                }
            }
            else
            {
                /* not found.. continue.. */
                /* Do Nothing */
            }

            sIdx++;
        }
    }
    else
    {
        /* position 이 음수인 경우로, string의 right에서부터 검색 */
        /* sub string 찾는다. */
        sIdx = sValue1->mLength + sPosition;
        sString = DTF_VARBINARY( sValue1 );
        sSubString = DTF_VARBINARY( sValue2 );
        
        while( sIdx >= 0 )
        {
            /* 속도를 빠르게 하기 위해서 1byte만 먼저 비교한다. */
            if( sString[sIdx] == sSubString[0] )
            {
                sCmpRtn = stlMemcmp( sString + sIdx,
                                     sSubString,
                                     sValue2->mLength );
                
                if( sCmpRtn == 0 )
                {
                    sCmpSuccessCnt++;
                    
                    if( sCmpSuccessCnt == sOccurrence )
                    {
                        /* found it.. */
                        
                        sResultPosition = sIdx + 1;
                        break;
                    }
                    else
                    {
                        /* Do Nothing */
                    }
                }
                else
                {
                    /* not found.. continue.. */
                    /* Do Nothing */
                }
            }
            else
            {
                /* not found.. continue.. */
                /* Do Nothing */
            }
            
            sIdx--;
        }
    }
    
    
    /*
     * result value 구성.
     */
    
    if( sResultPosition == 0 )
    {
        DTL_NUMERIC_SET_ZERO( DTF_NUMERIC( sResultValue ), sResultValue->mLength );
    }
    else
    {
        STL_TRY( dtdToNumericFromInt64( sResultPosition,
                                        gResultDataTypeDef[DTL_TYPE_NUMBER].mArgNum1,
                                        gResultDataTypeDef[DTL_TYPE_NUMBER].mArgNum2,
                                        sResultValue,
                                        sErrorStack )
                 == STL_SUCCESS );
        
    }
    
    STL_RAMP( SUCCESS_END );
    
    return STL_SUCCESS;
    
    STL_CATCH( ERROR_ARGUMENT_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_ARGUMENT_OUT_OF_RANGE,
                      NULL,
                      sErrorStack,
                      sOccurrence );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief ascii characterset에 맞게 position에 대한 substring 위치를 찾는다.
 * @param[in]  aStrValue         source string
 * @param[in]  aSubstrValue      찾고자 하는 substring
 * @param[in]  aPosition         substring을 검색할 처음 위치 ( char length )
 * @param[in]  aPositionByteLen  substring을 검색할 처음 위치 ( byte length )
 * @param[in]  aOccurrence       substring 검색 횟수
 * @param[out] aResultPosition   substring 위치
 * @param[in]  aErrorStack       stlErrorStack
 */
stlStatus dtfInstrGetAsciiPosition( dtlDataValue   * aStrValue,
                                    dtlDataValue   * aSubstrValue,
                                    stlInt64         aPosition,
                                    stlInt64         aPositionByteLen,
                                    stlInt64         aOccurrence,
                                    stlInt64       * aResultPosition,
                                    stlErrorStack  * aErrorStack )
{
    stlInt64        sPosition = aPosition;
    stlInt64        sBytePosition = aPositionByteLen;
    stlInt64        sResultPosition = 0;
    stlInt64        sCmpSuccessCnt = 0;
    stlInt32        sCmpRtn = 0;
    stlInt64        sIdx = 0;
    stlInt64        sMaxIdx = 0;
    stlInt64        sCharCnt = 0;
    stlInt8         sMaxCharLen = 0;
    stlInt8         sTempLen = 0;   /* 1char 의 length */
    stlInt8         i = 0;    
    stlChar       * sString = NULL;
    stlChar       * sSubString = NULL;
    stlBool         sVerify = STL_FALSE;
    
    /*
     * position에 대한 substring 위치를 찾는다.
     */ 

    if( sPosition > 0 )
    {
        /*
         * substring에 대한 위치를 찾는다.
         */
        
        /* position 이 양수인 경우로, string의 left에서부터 검색 */
        /* sub string 찾는다. */
        sIdx = sBytePosition - 1;
        sMaxIdx = aStrValue->mLength - aSubstrValue->mLength + 1;
        sCharCnt = 0;
        sString = DTF_VARCHAR( aStrValue ) + sIdx;
        sSubString = DTF_VARCHAR( aSubstrValue );
        
        while( sIdx < sMaxIdx )
        {
            /* 속도를 빠르게 하기 위해서 1byte만 먼저 비교한다. */
            if( sString[0] == sSubString[0] )
            {
                sCmpRtn = stlMemcmp( sString,
                                     sSubString,
                                     aSubstrValue->mLength );
                
                if( sCmpRtn == 0 )
                {
                    sCmpSuccessCnt++;
                    
                    if( sCmpSuccessCnt == aOccurrence )
                    {
                        /* found it.. */
                        sResultPosition = sPosition + sCharCnt;
                        break;
                    }
                    else
                    {
                        /* Do Nothing */
                    }
                }
                else
                {
                    /* not found.. continue.. */
                    /* Do Nothing */
                }
            }
            else
            {
                /* not found.. continue.. */
                /* Do Nothing */
            }

            /*
             * character set에 따른 1 char에 대한 length를 구하고
             * 그 만큼씩 넘어가면서 비교한다.
             */
            
            DTS_GET_ASCII_MBLENGTH( sString,
                                    & sTempLen );
            
            sCharCnt++;
            sIdx += sTempLen;
            sString += sTempLen;
        }
    }
    else
    {
        /* position 이 음수인 경우로, string의 right에서부터 검색 */
        /* sub string 찾는다. */
        sIdx = sBytePosition - 1;
        sString = DTF_VARCHAR( aStrValue );
        sSubString = DTF_VARCHAR( aSubstrValue );
        sMaxCharLen = DTS_GET_ASCII_MB_MAX_LENGTH;
        
        while( sIdx >= 0 )
        {
            /* 속도를 빠르게 하기 위해서 1byte만 먼저 비교한다. */
            if( sString[sIdx] == sSubString[0] )
            {
                sCmpRtn = stlMemcmp( sString + sIdx,
                                     sSubString,
                                     aSubstrValue->mLength );
                
                if( sCmpRtn == 0 )
                {
                    sCmpSuccessCnt++;
                    
                    if( sCmpSuccessCnt == aOccurrence )
                    {
                        /* found it.. */
                        
                        sPosition = 0;
                        sBytePosition = 0;
                        
                        while( sBytePosition < sIdx )
                        {
                            DTS_GET_ASCII_MBLENGTH( sString + sBytePosition,
                                                    &sTempLen );
                            
                            sBytePosition += sTempLen;
                            sPosition++;
                        }
                        
                        sResultPosition = sPosition + 1;
                        break;
                    }
                    else
                    {
                        /* Do Nothing */
                    }
                }
                else
                {
                    /* not found.. continue.. */
                    /* Do Nothing */                    
                }
            }
            else
            {
                /* not found.. continue.. */
                /* Do Nothing */
            }
            
            /*
             * character set에 따른 1 char에 대한 length를 구하고
             * 그 만큼씩 넘어가면서 비교한다.
             */
            
            i = ( sIdx - sMaxCharLen > 0 ) ? sMaxCharLen : sIdx;
            sTempLen = 1;
            for( ; i > 0; i-- )
            {
                DTS_IS_VERIFY_ASCII( &sTempLen,
                                     &sVerify );
                if( sVerify == STL_TRUE )
                {
                    /*
                     * char를 찾았음으로 1Char만큼 건너뛰도록.
                     */
                    
                    i = i - sTempLen + 1;
                }
                else
                {
                    /* Do Nothing */
                }
            }
            
            sIdx -= sTempLen;
        }
    }
    
    *aResultPosition = sResultPosition;
    
    return STL_SUCCESS;
    
//    STL_FINISH;
    
//    return STL_FAILURE;    
}


/**
 * @brief utf8 characterset에 맞게 position에 대한 substring 위치를 찾는다.
 * @param[in]  aStrValue         source string
 * @param[in]  aSubstrValue      찾고자 하는 substring
 * @param[in]  aPosition         substring을 검색할 처음 위치 ( char length )
 * @param[in]  aPositionByteLen  substring을 검색할 처음 위치 ( byte length )
 * @param[in]  aOccurrence       substring 검색 횟수
 * @param[out] aResultPosition   substring 위치
 * @param[in]  aErrorStack       stlErrorStack
 */
stlStatus dtfInstrGetUtf8Position( dtlDataValue   * aStrValue,
                                   dtlDataValue   * aSubstrValue,
                                   stlInt64         aPosition,
                                   stlInt64         aPositionByteLen,
                                   stlInt64         aOccurrence,
                                   stlInt64       * aResultPosition,
                                   stlErrorStack  * aErrorStack )
{
    stlInt64        sPosition = aPosition;
    stlInt64        sBytePosition = aPositionByteLen;
    stlInt64        sResultPosition = 0;
    stlInt64        sCmpSuccessCnt = 0;
    stlInt32        sCmpRtn = 0;
    stlInt64        sIdx = 0;
    stlInt64        sMaxIdx = 0;
    stlInt64        sCharCnt = 0;
    stlInt8         sMaxCharLen = 0;
    stlInt8         sTempLen = 0;   /* 1char 의 length */
    stlInt8         i = 0;    
    stlChar       * sString = NULL;
    stlChar       * sSubString = NULL;
    stlChar       * sTempStr = NULL;
    stlBool         sVerify = STL_FALSE;
    
    /*
     * position에 대한 substring 위치를 찾는다.
     */ 
    
    if( sPosition > 0 )
    {
        /*
         * substring에 대한 위치를 찾는다.
         */
        
        /* position 이 양수인 경우로, string의 left에서부터 검색 */
        /* sub string 찾는다. */
        sIdx = sBytePosition - 1;
        sMaxIdx = aStrValue->mLength - aSubstrValue->mLength + 1;
        sCharCnt = 0;
        sString = DTF_VARCHAR( aStrValue ) + sIdx;
        sSubString = DTF_VARCHAR( aSubstrValue );
        
        while( sIdx < sMaxIdx )
        {
            /* 속도를 빠르게 하기 위해서 1byte만 먼저 비교한다. */
            if( sString[0] == sSubString[0] )
            {
                sCmpRtn = stlMemcmp( sString,
                                     sSubString,
                                     aSubstrValue->mLength );
                
                if( sCmpRtn == 0 )
                {
                    sCmpSuccessCnt++;
                    
                    if( sCmpSuccessCnt == aOccurrence )
                    {
                        /* found it.. */
                        sResultPosition = sPosition + sCharCnt;
                        break;
                    }
                    else
                    {
                        /* Do Nothing */
                    }
                }
                else
                {
                    /* not found.. continue.. */
                    /* Do Nothing */
                }
            }
            else
            {
                /* not found.. continue.. */
                /* Do Nothing */
            }

            /*
             * character set에 따른 1 char에 대한 length를 구하고
             * 그 만큼씩 넘어가면서 비교한다.
             */
            
            DTS_GET_UTF8_MBLENGTH( sString,
                                   & sTempLen );
            
            sCharCnt++;
            sIdx += sTempLen;
            sString += sTempLen;
        }
    }
    else
    {
        /* position 이 음수인 경우로, string의 right에서부터 검색 */
        /* sub string 찾는다. */
        sIdx = sBytePosition - 1;
        sString = DTF_VARCHAR( aStrValue );
        sSubString = DTF_VARCHAR( aSubstrValue );
        sMaxCharLen = DTS_GET_UTF8_MB_MAX_LENGTH;
        
        while( sIdx >= 0 )
        {
            /* 속도를 빠르게 하기 위해서 1byte만 먼저 비교한다. */
            if( sString[sIdx] == sSubString[0] )
            {
                sCmpRtn = stlMemcmp( sString + sIdx,
                                     sSubString,
                                     aSubstrValue->mLength );
                
                if( sCmpRtn == 0 )
                {
                    sCmpSuccessCnt++;
                    
                    if( sCmpSuccessCnt == aOccurrence )
                    {
                        /* found it.. */
                        
                        sPosition = 0;
                        sBytePosition = 0;
                        
                        while( sBytePosition < sIdx )
                        {
                            DTS_GET_UTF8_MBLENGTH( sString + sBytePosition,
                                                   &sTempLen );
                            
                            sBytePosition += sTempLen;
                            sPosition++;
                        }
                        
                        sResultPosition = sPosition + 1;
                        break;
                    }
                    else
                    {
                        /* Do Nothing */
                    }
                }
                else
                {
                    /* not found.. continue.. */
                    /* Do Nothing */                    
                }
            }
            else
            {
                /* not found.. continue.. */
                /* Do Nothing */
            }
            
            /*
             * character set에 따른 1 char에 대한 length를 구하고
             * 그 만큼씩 넘어가면서 비교한다.
             */
            
            sTempStr = sString + sIdx;
            
            i = ( sIdx - sMaxCharLen > 0 ) ? sMaxCharLen : sIdx;
            sTempLen = 1;
            for( ; i > 0; i-- )
            {
                DTS_IS_VERIFY_UTF8( sTempStr - i,
                                    i,
                                    &sTempLen,
                                    &sVerify );
                if( sVerify == STL_TRUE )
                {
                    /*
                     * char를 찾았음으로 1Char만큼 건너뛰도록.
                     */
                    
                    i = i - sTempLen + 1;
                }
                else
                {
                    /* Do Nothing */
                }
            }
            
            sIdx -= sTempLen;
        }
    }

    *aResultPosition = sResultPosition;

    return STL_SUCCESS;
    
    STL_CATCH( ERROR_INVALID_CHARACTER_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INVALID_CHARACTER_IN_CHARSET_REPERTOIRE,
                      NULL,
                      aErrorStack );  
    }
    
    STL_FINISH;

    return STL_FAILURE;    
}

/**
 * @brief uhc characterset에 맞게 position에 대한 substring 위치를 찾는다.
 * @param[in]  aStrValue         source string
 * @param[in]  aSubstrValue      찾고자 하는 substring
 * @param[in]  aPosition         substring을 검색할 처음 위치 ( char length )
 * @param[in]  aPositionByteLen  substring을 검색할 처음 위치 ( byte length )
 * @param[in]  aOccurrence       substring 검색 횟수
 * @param[out] aResultPosition   substring 위치
 * @param[in]  aErrorStack       stlErrorStack
 */
stlStatus dtfInstrGetUhcPosition( dtlDataValue   * aStrValue,
                                  dtlDataValue   * aSubstrValue,
                                  stlInt64         aPosition,
                                  stlInt64         aPositionByteLen,
                                  stlInt64         aOccurrence,
                                  stlInt64       * aResultPosition,
                                  stlErrorStack  * aErrorStack )
{
    stlInt64        sPosition = aPosition;
    stlInt64        sBytePosition = aPositionByteLen;
    stlInt64        sResultPosition = 0;
    stlInt64        sCmpSuccessCnt = 0;
    stlInt32        sCmpRtn = 0;
    stlInt64        sIdx = 0;
    stlInt64        sMaxIdx = 0;
    stlInt64        sCharCnt = 0;
    stlInt8         sMaxCharLen = 0;
    stlInt8         sTempLen = 0;   /* 1char 의 length */
    stlInt8         i = 0;    
    stlChar       * sString = NULL;
    stlChar       * sSubString = NULL;
    stlChar       * sTempStr = NULL;
    stlBool         sVerify = STL_FALSE;
    
    /*
     * position에 대한 substring 위치를 찾는다.
     */ 
    
    if( sPosition > 0 )
    {
        /*
         * substring에 대한 위치를 찾는다.
         */
        
        /* position 이 양수인 경우로, string의 left에서부터 검색 */
        /* sub string 찾는다. */
        sIdx = sBytePosition - 1;
        sMaxIdx = aStrValue->mLength - aSubstrValue->mLength + 1;
        sCharCnt = 0;
        sString = DTF_VARCHAR( aStrValue ) + sIdx;
        sSubString = DTF_VARCHAR( aSubstrValue );
        
        while( sIdx < sMaxIdx )
        {
            /* 속도를 빠르게 하기 위해서 1byte만 먼저 비교한다. */
            if( sString[0] == sSubString[0] )
            {
                sCmpRtn = stlMemcmp( sString,
                                     sSubString,
                                     aSubstrValue->mLength );
                
                if( sCmpRtn == 0 )
                {
                    sCmpSuccessCnt++;
                    
                    if( sCmpSuccessCnt == aOccurrence )
                    {
                        /* found it.. */
                        sResultPosition = sPosition + sCharCnt;
                        break;
                    }
                    else
                    {
                        /* Do Nothing */
                    }
                }
                else
                {
                    /* not found.. continue.. */
                    /* Do Nothing */
                }
            }
            else
            {
                /* not found.. continue.. */
                /* Do Nothing */
            }

            /*
             * character set에 따른 1 char에 대한 length를 구하고
             * 그 만큼씩 넘어가면서 비교한다.
             */
            
            DTS_GET_UHC_MBLENGTH( sString,
                                  & sTempLen );
            
            sCharCnt++;
            sIdx += sTempLen;
            sString += sTempLen;
        }
    }
    else
    {
        /* position 이 음수인 경우로, string의 right에서부터 검색 */
        /* sub string 찾는다. */
        sIdx = sBytePosition - 1;
        sString = DTF_VARCHAR( aStrValue );
        sSubString = DTF_VARCHAR( aSubstrValue );
        sMaxCharLen = DTS_GET_UHC_MB_MAX_LENGTH;
        
        while( sIdx >= 0 )
        {
            /* 속도를 빠르게 하기 위해서 1byte만 먼저 비교한다. */
            if( sString[sIdx] == sSubString[0] )
            {
                sCmpRtn = stlMemcmp( sString + sIdx,
                                     sSubString,
                                     aSubstrValue->mLength );
                
                if( sCmpRtn == 0 )
                {
                    sCmpSuccessCnt++;
                    
                    if( sCmpSuccessCnt == aOccurrence )
                    {
                        /* found it.. */
                        
                        sPosition = 0;
                        sBytePosition = 0;
                        
                        while( sBytePosition < sIdx )
                        {
                            DTS_GET_UHC_MBLENGTH( sString + sBytePosition,
                                                  &sTempLen );
                            
                            sBytePosition += sTempLen;
                            sPosition++;
                        }
                        
                        sResultPosition = sPosition + 1;
                        break;
                    }
                    else
                    {
                        /* Do Nothing */
                    }
                }
                else
                {
                    /* not found.. continue.. */
                    /* Do Nothing */                    
                }
            }
            else
            {
                /* not found.. continue.. */
                /* Do Nothing */
            }
            
            /*
             * character set에 따른 1 char에 대한 length를 구하고
             * 그 만큼씩 넘어가면서 비교한다.
             */
            
            sTempStr = sString + sIdx;
            
            i = ( sIdx - sMaxCharLen > 0 ) ? sMaxCharLen : sIdx;
            sTempLen = 1;
            for( ; i > 0; i-- )
            {
                DTS_IS_VERIFY_UHC( sTempStr - i,
                                   i,
                                   &sTempLen,
                                   &sVerify );
                if( sVerify == STL_TRUE )
                {
                    /*
                     * char를 찾았음으로 1Char만큼 건너뛰도록.
                     */
                    
                    i = i - sTempLen + 1;
                }
                else
                {
                    /* Do Nothing */
                }
            }
            
            sIdx -= sTempLen;
        }
    }
    
    *aResultPosition = sResultPosition;
    
    return STL_SUCCESS;
    
//    STL_FINISH;

//    return STL_FAILURE;    
}

/**
 * @brief gb18030 characterset에 맞게 position에 대한 substring 위치를 찾는다.
 * @param[in]  aStrValue         source string
 * @param[in]  aSubstrValue      찾고자 하는 substring
 * @param[in]  aPosition         substring을 검색할 처음 위치 ( char length )
 * @param[in]  aPositionByteLen  substring을 검색할 처음 위치 ( byte length )
 * @param[in]  aOccurrence       substring 검색 횟수
 * @param[out] aResultPosition   substring 위치
 * @param[in]  aErrorStack       stlErrorStack
 */
stlStatus dtfInstrGetGb18030Position( dtlDataValue   * aStrValue,
                                      dtlDataValue   * aSubstrValue,
                                      stlInt64         aPosition,
                                      stlInt64         aPositionByteLen,
                                      stlInt64         aOccurrence,
                                      stlInt64       * aResultPosition,
                                      stlErrorStack  * aErrorStack )
{
    stlInt64        sPosition = aPosition;
    stlInt64        sBytePosition = aPositionByteLen;
    stlInt64        sResultPosition = 0;
    stlInt64        sCmpSuccessCnt = 0;
    stlInt32        sCmpRtn = 0;
    stlInt64        sIdx = 0;
    stlInt64        sMaxIdx = 0;
    stlInt64        sCharCnt = 0;
    stlInt8         sMaxCharLen = 0;
    stlInt8         sTempLen = 0;   /* 1char 의 length */
    stlInt8         i = 0;    
    stlChar       * sString = NULL;
    stlChar       * sSubString = NULL;
    stlChar       * sTempStr = NULL;
    stlBool         sVerify = STL_FALSE;
    
    /*
     * position에 대한 substring 위치를 찾는다.
     */ 
    
    if( sPosition > 0 )
    {
        /*
         * substring에 대한 위치를 찾는다.
         */
        
        /* position 이 양수인 경우로, string의 left에서부터 검색 */
        /* sub string 찾는다. */
        sIdx = sBytePosition - 1;
        sMaxIdx = aStrValue->mLength - aSubstrValue->mLength + 1;
        sCharCnt = 0;
        sString = DTF_VARCHAR( aStrValue ) + sIdx;
        sSubString = DTF_VARCHAR( aSubstrValue );
        
        while( sIdx < sMaxIdx )
        {
            /* 속도를 빠르게 하기 위해서 1byte만 먼저 비교한다. */
            if( sString[0] == sSubString[0] )
            {
                sCmpRtn = stlMemcmp( sString,
                                     sSubString,
                                     aSubstrValue->mLength );
                
                if( sCmpRtn == 0 )
                {
                    sCmpSuccessCnt++;
                    
                    if( sCmpSuccessCnt == aOccurrence )
                    {
                        /* found it.. */
                        sResultPosition = sPosition + sCharCnt;
                        break;
                    }
                    else
                    {
                        /* Do Nothing */
                    }
                }
                else
                {
                    /* not found.. continue.. */
                    /* Do Nothing */
                }
            }
            else
            {
                /* not found.. continue.. */
                /* Do Nothing */
            }

            /*
             * character set에 따른 1 char에 대한 length를 구하고
             * 그 만큼씩 넘어가면서 비교한다.
             */
            
            DTS_GET_GB18030_MBLENGTH( sString,
                                      & sTempLen );
            
            sCharCnt++;
            sIdx += sTempLen;
            sString += sTempLen;
        }
    }
    else
    {
        /* position 이 음수인 경우로, string의 right에서부터 검색 */
        /* sub string 찾는다. */
        sIdx = sBytePosition - 1;
        sString = DTF_VARCHAR( aStrValue );
        sSubString = DTF_VARCHAR( aSubstrValue );
        sMaxCharLen = DTS_GET_GB18030_MB_MAX_LENGTH;
        
        while( sIdx >= 0 )
        {
            /* 속도를 빠르게 하기 위해서 1byte만 먼저 비교한다. */
            if( sString[sIdx] == sSubString[0] )
            {
                sCmpRtn = stlMemcmp( sString + sIdx,
                                     sSubString,
                                     aSubstrValue->mLength );
                
                if( sCmpRtn == 0 )
                {
                    sCmpSuccessCnt++;
                    
                    if( sCmpSuccessCnt == aOccurrence )
                    {
                        /* found it.. */
                        
                        sPosition = 0;
                        sBytePosition = 0;
                        
                        while( sBytePosition < sIdx )
                        {
                            DTS_GET_GB18030_MBLENGTH( sString + sBytePosition,
                                                      &sTempLen );
                            
                            sBytePosition += sTempLen;
                            sPosition++;
                        }
                        
                        sResultPosition = sPosition + 1;
                        break;
                    }
                    else
                    {
                        /* Do Nothing */
                    }
                }
                else
                {
                    /* not found.. continue.. */
                    /* Do Nothing */                    
                }
            }
            else
            {
                /* not found.. continue.. */
                /* Do Nothing */
            }
            
            /*
             * character set에 따른 1 char에 대한 length를 구하고
             * 그 만큼씩 넘어가면서 비교한다.
             */
            
            sTempStr = sString + sIdx;
            
            i = ( sIdx - sMaxCharLen > 0 ) ? sMaxCharLen : sIdx;
            sTempLen = 1;
            for( ; i > 0; i-- )
            {
                DTS_IS_VERIFY_GB18030( sTempStr - i,
                                       i,
                                       &sTempLen,
                                       &sVerify );
                if( sVerify == STL_TRUE )
                {
                    /*
                     * char를 찾았음으로 1Char만큼 건너뛰도록.
                     */
                    
                    i = i - sTempLen + 1;
                }
                else
                {
                    /* Do Nothing */
                }
            }
            
            sIdx -= sTempLen;
        }
    }
    
    *aResultPosition = sResultPosition;
    
    return STL_SUCCESS;

    STL_CATCH( ERROR_CHARACTER_NOT_IN_REPERTOIRE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INVALID_CHARACTER_IN_CHARSET_REPERTOIRE,
                      NULL,
                      aErrorStack );  
    }

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
