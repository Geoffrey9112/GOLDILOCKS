/*******************************************************************************
 * dtfTrim.c
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
 * @file dtfTrim.c
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

stlStatus dtfTrimLeadingForBinaryCompare( stlChar          * aString,
                                          stlInt64           aStringLen,
                                          stlChar          * aTrimChar,
                                          stlInt8            aTrimCharByte,
                                          stlChar         ** aRemainString,
                                          stlInt64         * aRemainStringLen,
                                          stlErrorStack    * aErrorStack );

stlStatus dtfTrimLeadingForUhc( stlChar          * aString,
                                stlInt64           aStringLen,
                                stlChar          * aTrimChar,
                                stlInt8            aTrimCharByte,
                                stlChar         ** aRemainString,
                                stlInt64         * aRemainStringLen,
                                stlErrorStack    * aErrorStack );

stlStatus dtfTrimLeadingForGb18030( stlChar          * aString,
                                    stlInt64           aStringLen,
                                    stlChar          * aTrimChar,
                                    stlInt8            aTrimCharByte,
                                    stlChar         ** aRemainString,
                                    stlInt64         * aRemainStringLen,
                                    stlErrorStack    * aErrorStack );

stlStatus dtfTrimTrailingForAscii( stlChar          * aString,
                                   stlInt64           aStringLen,
                                   stlChar          * aTrimChar,
                                   stlInt8            aTrimCharByte,
                                   stlInt64         * aRemainStringLen,
                                   stlErrorStack    * aErrorStack );

stlStatus dtfTrimTrailingForUtf8( stlChar          * aString,
                                  stlInt64           aStringLen,
                                  stlChar          * aTrimChar,
                                  stlInt8            aTrimCharByte,
                                  stlInt64         * aRemainStringLen,
                                  stlErrorStack    * aErrorStack );

stlStatus dtfTrimTrailingForUhc( stlChar          * aString,
                                 stlInt64           aStringLen,
                                 stlChar          * aTrimChar,
                                 stlInt8            aTrimCharByte,
                                 stlInt64         * aRemainStringLen,
                                 stlErrorStack    * aErrorStack );

stlStatus dtfTrimTrailingForGb18030( stlChar          * aString,
                                     stlInt64           aStringLen,
                                     stlChar          * aTrimChar,
                                     stlInt8            aTrimCharByte,
                                     stlInt64         * aRemainStringLen,
                                     stlErrorStack    * aErrorStack );

dtfTrimLeadingFunc dtfTrimLeadingFuncList[ DTL_CHARACTERSET_MAX + 1 ] =
{
    dtfTrimLeadingForBinaryCompare,
    dtfTrimLeadingForBinaryCompare,
    dtfTrimLeadingForUhc,
    dtfTrimLeadingForGb18030,
    
    NULL
};


dtfTrimTrailingFunc dtfTrimTrailingFuncList[ DTL_CHARACTERSET_MAX + 1 ] =
{
    dtfTrimTrailingForAscii,
    dtfTrimTrailingForUtf8,
    dtfTrimTrailingForUhc,
    dtfTrimTrailingForGb18030,
    
    NULL
};


/*******************************************************************************
 * trim([leading | trailing | both] [characters] from string)
 *   trim([leading | trailing | both] [characters] from string) => [ P, S, M, O ]
 * ex) trim(both 'x' from 'xTomxx')  =>  Tom
 *******************************************************************************/

/**
 * @brief trim([leading | trailing | both] [characters] from string)
 *        <BR> trim([leading | trailing | both] [characters] from string)
 *        <BR>   trim([leading | trailing | both] [characters] from string) => [ P, S, M, O ]
 *        <BR> ex) trim(both 'x' from 'xTomxx')  =>  Tom
 *        <BR> ex) trim(both null from 'xTomxx')  =>  null
 *        <BR> ex) trim(both 'x' from null)  =>  null
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> trim(VARCHAR, VARCHAR)
 *                               <BR> trim(string, characters list)
 * @param[out] aResultValue      연산 결과 (결과타입 VARCHAR)
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfStringTrim( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         dtlFuncVector  * aVectorFunc,
                         void           * aVectorArg,
                         void           * aEnv )
{
    dtlDataValue    * sValue1;
    dtlDataValue    * sValue2;
    dtlDataValue    * sResultValue;
    stlErrorStack   * sErrorStack;
    stlBool           sLTrim = STL_FALSE;
    stlBool           sRTrim = STL_FALSE;
    stlChar         * sString;
    stlInt64          sStringLen;
    stlInt8           sTrimByte;    /* 1 char 의 length */

    stlChar         * sTrimStr;
    stlChar           sDefaultTrimChar[1];
    
    dtlCharacterSet   sCharSetID;
    
    /**
     * Parameter Validation
     */
    
    DTL_PARAM_VALIDATE( ( aInputArgumentCnt >= 2 ) &&
                        ( aInputArgumentCnt <= 3 ),
                        (stlErrorStack *)aEnv );

    /* character set이 추가될때 그에 따른 function list가 누락되는 일이 없도록 하기 위한 검사 코드 */
    DTF_CHECK_CHAR_SET_FUNC_LIST( dtfTrimLeadingFuncList );
    DTF_CHECK_CHAR_SET_FUNC_LIST( dtfTrimTrailingFuncList );
    
    
    /*
     * TRIM( [ [ LEADING | TRAILING | BOTH ] [trim_character] FROM ] trim_source ) 
     *                                                              ^^^^^^^^^^^^^
     *
     * 함수의 인자로 아래와 같이 내려옴.
     * ( [leading|trailing|both] ), trim_source, [ trim_charactor ] )
     *   ^^^^^^^^^^^^
     */

    /*
     * trim specification
     */
    STL_DASSERT( DTL_IS_NULL( aInputArgument[0].mValue.mDataValue ) != STL_TRUE );    
    if( DTF_BIGINT( aInputArgument[0].mValue.mDataValue ) == DTL_FUNC_TRIM_LTRIM )
    {
        sLTrim = STL_TRUE;
    }
    else if( DTF_BIGINT( aInputArgument[0].mValue.mDataValue ) == DTL_FUNC_TRIM_RTRIM )
    {
        sRTrim = STL_TRUE;
    }
    else
    {
        sLTrim = STL_TRUE;
        sRTrim = STL_TRUE;
    }
    
    /* trim_source */
    sValue1 = aInputArgument[1].mValue.mDataValue;

    sResultValue = (dtlDataValue *)aResultValue;        
    sCharSetID = aVectorFunc->mGetCharSetIDFunc(aVectorArg);    
    sErrorStack = (stlErrorStack *)aEnv;
    
    DTF_CHECK_DTL_GROUP2( sValue1,
                          DTL_GROUP_CHAR_STRING,
                          DTL_GROUP_LONGVARCHAR_STRING,
                          sErrorStack );
    /* NULL value는 들어오지 않는다. */
    STL_DASSERT( DTL_IS_NULL( sValue1 ) != STL_TRUE );
    
    
    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT2( sResultValue,
                           DTL_TYPE_VARCHAR,
                           DTL_TYPE_LONGVARCHAR,
                           sErrorStack );
    
    if( aInputArgumentCnt == 2 )
    {
        /*
         * trim_character가 생략되었을 경우, ' '으로 처리됨.
         * 예) TRIM( BOTH FROM 'ABCDE' ) => TRIM( BOTH ' ' FROM 'abcde' )
         */
        
        /*
         * @todo
         * ' ' 는 character set에 맞는 code로 가져와야 한다.
         * character set에 맞는 CharLen, ByteLen을 구해야 함.
         */
        sDefaultTrimChar[0] = ' ';
        
        sTrimStr = sDefaultTrimChar;
        sTrimByte = 1;        
    }
    else
    {
        /* trim_character */
        sValue2 = aInputArgument[2].mValue.mDataValue;
        
        DTF_CHECK_DTL_GROUP2( sValue2,
                              DTL_GROUP_CHAR_STRING,
                              DTL_GROUP_LONGVARCHAR_STRING,
                              sErrorStack );
        /* NULL value는 들어오지 않는다. */
        STL_DASSERT( DTL_IS_NULL( sValue2 ) != STL_TRUE );
        
        /**
         * Trim Character 가 한 글자인지 확인한다.
         */
        
        sTrimStr = DTF_VARCHAR( sValue2 );
        
        STL_TRY( dtsGetMbLengthFuncList[sCharSetID]( sTrimStr,
                                                     &sTrimByte,
                                                     sErrorStack )
                 == STL_SUCCESS );
        
        STL_TRY_THROW( (sTrimByte == sValue2->mLength), RAMP_ERR_TRIM_ONE_CHARACTER );        
    }
    
    /*
     * sString : target data
     * sStringLen : target data의 길이
     */
    sString = sValue1->mValue;
    sStringLen = sValue1->mLength;
    
    if( sLTrim == STL_TRUE )
    {
        STL_TRY( dtfTrimLeadingFuncList[sCharSetID]( sString,
                                                     sStringLen,
                                                     sTrimStr,
                                                     sTrimByte,
                                                     & sString,
                                                     & sStringLen,
                                                     sErrorStack )
                 == STL_SUCCESS );
    }
    else
    {
        /* Do Nothing */
    }
    
    if( sRTrim == STL_TRUE )
    {
        STL_TRY( dtfTrimTrailingFuncList[sCharSetID]( sString,
                                                      sStringLen,
                                                      sTrimStr,
                                                      sTrimByte,
                                                      & sStringLen,
                                                      sErrorStack )
                 == STL_SUCCESS );
    }
    else
    {
        /* Do Nothing */
    }

    stlMemcpy( DTF_VARCHAR( sResultValue ), sString, sStringLen );
    sResultValue->mLength = sStringLen;

    STL_DASSERT( sResultValue->mLength <= sResultValue->mBufferSize );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_TRIM_ONE_CHARACTER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_TRIM_SET_SHOULD_ONLY_ONE_CHARACTER,
                      NULL,
                      sErrorStack );
    }

    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief trim([leading | trailing | both] [characters] from string)
 *        <BR> trim([leading | trailing | both] [characters] from string)
 *        <BR>   trim([leading | trailing | both] [characters] from string) => [ P, S, M, O ]
 *        <BR> ex) trim(both 'x' from 'xTomxx')  =>  Tom
 *        <BR> ex) trim(both null from 'xTomxx')  =>  null
 *        <BR> ex) trim(both 'x' from null)  =>  null
 *        <BR> @todo 이 함수를 이용해,  LTRIM, RTRIM, TRIM 도 구현 
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> trim(VARBINARY, VARBINARY)
 *                               <BR> trim(string, characters list)
 * @param[out] aResultValue      연산 결과 (결과타입 VARBINARY)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfBinaryStringTrim( stlUInt16        aInputArgumentCnt,
                               dtlValueEntry  * aInputArgument,
                               void           * aResultValue,
                               void           * aEnv )
{
    dtlDataValue    * sValue1;
    dtlDataValue    * sValue2;
    dtlDataValue    * sResultValue;
    stlBool           sLTrim = STL_FALSE;
    stlBool           sRTrim = STL_FALSE;
    stlChar         * sString;
    stlInt64          sStringLen;
    stlChar         * sTrimStr;
    stlInt64          sTrimLen;
    stlChar           sDefaultTrimChar[1];    

    DTL_PARAM_VALIDATE( ( aInputArgumentCnt >= 2 ) &&
                        ( aInputArgumentCnt <= 3 ),
                        (stlErrorStack *)aEnv );
    
    /*
     * TRIM( [ [ LEADING | TRAILING | BOTH ] [trim_character] FROM ] trim_source ) 
     *                                                              ^^^^^^^^^^^^^
     *
     * 함수의 인자로 아래와 같이 내려옴.
     * ( [leading|trailing|both] ), trim_source, [ trim_charactor ] )
     *   ^^^^^^^^^^^^
     */

    /*
     * trim specification
     */
    STL_DASSERT( DTL_IS_NULL( aInputArgument[0].mValue.mDataValue ) != STL_TRUE );        
    if( DTF_BIGINT( aInputArgument[0].mValue.mDataValue ) == DTL_FUNC_TRIM_LTRIM )
    {
        sLTrim = STL_TRUE;
    }
    else if( DTF_BIGINT( aInputArgument[0].mValue.mDataValue ) == DTL_FUNC_TRIM_RTRIM )
    {
        sRTrim = STL_TRUE;
    }
    else
    {
        sLTrim = STL_TRUE;
        sRTrim = STL_TRUE;
    }
    
    /* trim_source */    
    sValue1 = aInputArgument[1].mValue.mDataValue;

    sResultValue = (dtlDataValue *)aResultValue;

    DTF_CHECK_DTL_GROUP2( sValue1,
                          DTL_GROUP_BINARY_STRING,
                          DTL_GROUP_LONGVARBINARY_STRING,
                          (stlErrorStack *)aEnv );
    /* NULL value는 들어오지 않는다. */
    STL_DASSERT( DTL_IS_NULL( sValue1 ) != STL_TRUE );
    
    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT2( sResultValue,
                           DTL_TYPE_VARBINARY,
                           DTL_TYPE_LONGVARBINARY,
                           (stlErrorStack *)aEnv );

    if( aInputArgumentCnt == 2 )
    {
        /*
         * trim_character가 생략되었을 경우, X'00'으로 처리됨.
         * 예) TRIM( BOTH FROM X'AABBCC' ) => TRIM( BOTH X'00' FROM X'AABBCC' )
         */

        *(stlInt8 *)sDefaultTrimChar = 0;
        
        sTrimStr = sDefaultTrimChar;
        sTrimLen = 1;
    }
    else
    {
        /* trim_character */
        sValue2 = aInputArgument[2].mValue.mDataValue;
        
        DTF_CHECK_DTL_GROUP2( sValue2,
                              DTL_GROUP_BINARY_STRING,
                              DTL_GROUP_LONGVARBINARY_STRING,
                              (stlErrorStack *)aEnv );
        
        /* NULL value는 들어오지 않는다. */
        STL_DASSERT( DTL_IS_NULL( sValue2 ) != STL_TRUE );
        
        sTrimStr = DTF_VARBINARY( sValue2 );
        sTrimLen = sValue2->mLength;
    }
    

    /**
     * Trim Character 가 한 글자인지 확인한다.
     */

    STL_TRY_THROW( sTrimLen == 1, RAMP_ERR_TRIM_ONE_CHARACTER );
    
    
    /*
     * sString : target data
     * sStringLen : target data의 길이
     */
    sString = sValue1->mValue;
    sStringLen = sValue1->mLength;

    if( sLTrim == STL_TRUE )
    {
        while( sStringLen > 0 )
        {
            if ( *sString == sTrimStr[0] )
            {
                /**
                 * Match
                 */
                
                sString++;
                sStringLen--;
                
                continue;
            }
            else
            {
                /**
                 * No Match
                 */
                
                break;  
            }
        }
    }
    else
    {
        /* Do Nothing */
    }

    if( sRTrim == STL_TRUE )
    {
        while( sStringLen > 0 )
        {
            if( sString[sStringLen - 1] == sTrimStr[0] )
            {
                /**
                 * Match
                 */
                
                sStringLen--;
                continue;
            }
            else
            {
                /**
                 * No Match
                 */
                
                break;  
            }
        }
    }
    else
    {
        /* Do Nothing */
    }

    stlMemcpy( DTF_VARBINARY( sResultValue ), sString, sStringLen );
    sResultValue->mLength = sStringLen;

    STL_DASSERT( sResultValue->mLength <= sResultValue->mBufferSize );    

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_TRIM_ONE_CHARACTER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_TRIM_SET_SHOULD_ONLY_ONE_CHARACTER,
                      NULL,
                      (stlErrorStack *) aEnv );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Binary Compare를 할 수 있는 character set의 trim LEADING 처리
 * @param[in]  aString           trim_source string
 * @param[in]  aStringLen        trim_source string byte length
 * @param[in]  aTrimChar         trim_character string 
 * @param[in]  aTrimCharByte     trim_character string byte length
 * @param[out] aRemainString     trim 처리후 남은 trim_source string ptr
 * @param[out] aRemainStringLen  trim 처리후 남은 trim_source string length
 * @param[out] aErrorStack       stlErrorStack
 */
stlStatus dtfTrimLeadingForBinaryCompare( stlChar          * aString,
                                          stlInt64           aStringLen,
                                          stlChar          * aTrimChar,
                                          stlInt8            aTrimCharByte,
                                          stlChar         ** aRemainString,
                                          stlInt64         * aRemainStringLen,
                                          stlErrorStack    * aErrorStack )
{
    stlChar   * sString = aString;
    stlInt64    sStringLen = aStringLen;
    
    while( sStringLen >= aTrimCharByte )
    {
        if( aTrimCharByte == 1 )
        {
            if( sString[0] == aTrimChar[0] )
            {
                /**
                 * Match
                 */
            }
            else
            {
                /**
                 * No Match
                 */                    
                break;
            }
        }
        else if( aTrimCharByte == 2 )
        {
            if( ( sString[0] == aTrimChar[0] ) &&
                ( sString[1] == aTrimChar[1] ) )
            {
                /**
                 * Match
                 */
            }
            else
            {
                /**
                 * No Match
                 */                    
                break;
            }
        }
        else if( aTrimCharByte == 3 )
        {
            if( ( sString[0] == aTrimChar[0] ) &&
                ( sString[1] == aTrimChar[1] ) &&
                ( sString[2] == aTrimChar[2] ) )
            {
                /**
                 * Match
                 */
            }
            else
            {
                /**
                 * No Match
                 */                    
                break;
            }            
        }
        else if( aTrimCharByte == 4 )
        {
            if( ( sString[0] == aTrimChar[0] ) &&
                ( sString[1] == aTrimChar[1] ) &&
                ( sString[2] == aTrimChar[2] ) &&
                ( sString[3] == aTrimChar[3] ) )
            {
                /**
                 * Match
                 */
            }
            else
            {
                /**
                 * No Match
                 */                    
                break;
            }                        
        }
        else
        {
            STL_DASSERT( STL_FALSE );
        }
            
        sString += aTrimCharByte;
        sStringLen -= aTrimCharByte;
    }
    
    *aRemainString = sString;
    *aRemainStringLen = sStringLen;
    
    return STL_SUCCESS;
}


/**
 * @brief UHC character set의 trim LEADING 처리
 * @param[in]  aString           trim_source string
 * @param[in]  aStringLen        trim_source string byte length
 * @param[in]  aTrimChar         trim_character string 
 * @param[in]  aTrimCharByte     trim_character string byte length
 * @param[out] aRemainString     trim 처리후 남은 trim_source string ptr
 * @param[out] aRemainStringLen  trim 처리후 남은 trim_source string length
 * @param[out] aErrorStack       stlErrorStack
 */
stlStatus dtfTrimLeadingForUhc( stlChar          * aString,
                                stlInt64           aStringLen,
                                stlChar          * aTrimChar,
                                stlInt8            aTrimCharByte,
                                stlChar         ** aRemainString,
                                stlInt64         * aRemainStringLen,
                                stlErrorStack    * aErrorStack )
{
    stlChar   * sString = aString;
    stlInt64    sStringLen = aStringLen;
    stlInt8     sSourceLen = 0;
    
    while( sStringLen >= aTrimCharByte )
    {
        /**
         * 체크할 sString의 첫번째 하나의 char의 length를 구한다.
         */
        
        DTS_GET_UHC_MBLENGTH( sString,
                              & sSourceLen );
        
        if( sSourceLen == aTrimCharByte )
        {
            if( aTrimCharByte == 1 )
            {
                if( sString[0] == aTrimChar[0] )
                {
                    /**
                     * Match
                     */
                }
                else
                {
                    /**
                     * No Match
                     */                    
                    break;
                }
            }
            else if( aTrimCharByte == 2 )
            {
                if( ( sString[0] == aTrimChar[0] ) &&
                    ( sString[1] == aTrimChar[1] ) )
                {
                    /**
                     * Match
                     */
                }
                else
                {
                    /**
                     * No Match
                     */
                    break;
                }
            }
            else
            {
                STL_DASSERT( STL_FALSE );
            }
            
            sString += aTrimCharByte;
            sStringLen -= aTrimCharByte;
        }
        else
        {
            /**
             * No Match
             */
            
            break;
        }
    }
    
    *aRemainString = sString;
    *aRemainStringLen = sStringLen;
    
    return STL_SUCCESS;
}

/**
 * @brief GB18030 character set의 trim LEADING 처리
 * @param[in]  aString           trim_source string
 * @param[in]  aStringLen        trim_source string byte length
 * @param[in]  aTrimChar         trim_character string 
 * @param[in]  aTrimCharByte     trim_character string byte length
 * @param[out] aRemainString     trim 처리후 남은 trim_source string ptr
 * @param[out] aRemainStringLen  trim 처리후 남은 trim_source string length
 * @param[out] aErrorStack       stlErrorStack
 */
stlStatus dtfTrimLeadingForGb18030( stlChar          * aString,
                                    stlInt64           aStringLen,
                                    stlChar          * aTrimChar,
                                    stlInt8            aTrimCharByte,
                                    stlChar         ** aRemainString,
                                    stlInt64         * aRemainStringLen,
                                    stlErrorStack    * aErrorStack )
{
    stlChar   * sString = aString;
    stlInt64    sStringLen = aStringLen;
    stlInt8     sSourceLen = 0;

    while( sStringLen >= aTrimCharByte )
    {
        /**
         * 체크할 sString의 첫번째 하나의 char의 length를 구한다.
         */
    
        DTS_GET_GB18030_MBLENGTH( sString,
                                  & sSourceLen );
    
        if( sSourceLen == aTrimCharByte )
        {
            if( aTrimCharByte == 1 )
            {
                if( sString[0] == aTrimChar[0] )
                {
                    /**
                     * Match
                     */
                }
                else
                {
                    /**
                     * No Match
                     */                    
                    break;
                }
            }
            else if( aTrimCharByte == 2 )
            {
                if( ( sString[0] == aTrimChar[0] ) &&
                    ( sString[1] == aTrimChar[1] ) )
                {
                    /**
                     * Match
                     */
                }
                else
                {
                    /**
                     * No Match
                     */
                    break;
                }
            }
            else if( aTrimCharByte == 4 )
            {
                if( ( sString[0] == aTrimChar[0] ) &&
                    ( sString[1] == aTrimChar[1] ) &&
                    ( sString[2] == aTrimChar[2] ) &&
                    ( sString[3] == aTrimChar[3] ) )
                {
                    /**
                     * Match
                     */
                }
                else
                {
                    /**
                     * No Match
                     */
                    break;
                }
            }
            else
            {
                STL_DASSERT( STL_FALSE );
            }
        
            sString += aTrimCharByte;
            sStringLen -= aTrimCharByte;
        }
        else
        {
            /**
             * No Match
             */
        
            break;
        }
    }

    *aRemainString = sString;
    *aRemainStringLen = sStringLen;

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

/**
 * @brief ASCII character set의 trim TRAILING 처리
 * @param[in]  aString           trim_source string
 * @param[in]  aStringLen        trim_source string byte length
 * @param[in]  aTrimChar         trim_character string 
 * @param[in]  aTrimCharByte     trim_character string byte length
 * @param[out] aRemainStringLen  trim 처리후 남은 trim_source string length
 * @param[out] aErrorStack       stlErrorStack
 */
stlStatus dtfTrimTrailingForAscii( stlChar          * aString,
                                   stlInt64           aStringLen,
                                   stlChar          * aTrimChar,
                                   stlInt8            aTrimCharByte,
                                   stlInt64         * aRemainStringLen,
                                   stlErrorStack    * aErrorStack )
{
    stlChar   * sString = aString + aStringLen;
    stlInt64    sStringByteLen = aStringLen;
    stlInt8     sMaxCharLen = DTS_GET_ASCII_MB_MAX_LENGTH;
    stlInt8     i = 0;
    stlInt8     sStrByte = 0;
    stlInt8     sTempLen = 0;
    stlBool     sVerify = STL_FALSE;
    stlBool     sIsEqual = STL_FALSE;
    
    while( sStringByteLen > 0  )
    {
        /**
         * 체크할 sString의 마지막 하나의 char의 length를 구한다.
         */
        
        if( sStringByteLen < sMaxCharLen )
        {
            sMaxCharLen = sStringByteLen;
        }
        else
        {
            /* Do Noghint */
        }
        
        for( i = sMaxCharLen; i > 0; i-- )
        {
            DTS_IS_VERIFY_ASCII( &sTempLen,
                                 &sVerify );
            
            if ( sVerify == STL_TRUE )
            {
                sStrByte = sTempLen;
                /*
                 * char를 찾았음으로 1Char만큼 건너뛰도록.
                 */
                i = i - sStrByte + 1;
            }
            else
            {
                /* Do Nothing */
            }
            /*
             * 마지막에 찾은 char의 length의 길이가 sTempLen에 저장된다.
             */
        }
        
        if( sStrByte == aTrimCharByte )
        {
            DTS_IS_EQUAL_SAME_BYTELEN_ONE_CHAR( ( sString - sStrByte ),
                                                aTrimChar,
                                                aTrimCharByte,
                                                & sIsEqual );
            if( sIsEqual == STL_TRUE )
            {
                /* Match */
                sString -= aTrimCharByte;
                sStringByteLen -= aTrimCharByte;
                
                continue;
            }
            else
            {
                /* No Match */
                break;
            }
        }
        else
        {
            /* No Match */
            break;
        }
    }
    
    STL_DASSERT( aString <= sString );
    STL_DASSERT( sStringByteLen >= 0 );
    
    *aRemainStringLen = sStringByteLen;
    
    return STL_SUCCESS;
}

/**
 * @brief UTF8 character set의 trim TRAILING 처리
 * @param[in]  aString           trim_source string
 * @param[in]  aStringLen        trim_source string byte length
 * @param[in]  aTrimChar         trim_character string 
 * @param[in]  aTrimCharByte     trim_character string byte length
 * @param[out] aRemainStringLen  trim 처리후 남은 trim_source string length
 * @param[out] aErrorStack       stlErrorStack
 */
stlStatus dtfTrimTrailingForUtf8( stlChar          * aString,
                                  stlInt64           aStringLen,
                                  stlChar          * aTrimChar,
                                  stlInt8            aTrimCharByte,
                                  stlInt64         * aRemainStringLen,
                                  stlErrorStack    * aErrorStack )
{
    stlChar   * sString = aString + aStringLen;
    stlInt64    sStringByteLen = aStringLen;
    stlInt8     sMaxCharLen = DTS_GET_UTF8_MB_MAX_LENGTH;
    stlInt8     i = 0;
    stlInt8     sStrByte = 0;
    stlInt8     sTempLen = 0;
    stlBool     sVerify = STL_FALSE;
    stlBool     sIsEqual = STL_FALSE;
    
    while( sStringByteLen > 0  )
    {
        /**
         * 체크할 sString의 마지막 하나의 char의 length를 구한다.
         */
        
        if( sStringByteLen < sMaxCharLen )
        {
            sMaxCharLen = sStringByteLen;
        }
        else
        {
            /* Do Noghint */
        }
        
        for( i = sMaxCharLen; i > 0; i-- )
        {
            DTS_IS_VERIFY_UTF8( sString - i,
                                i,
                                &sTempLen,
                                &sVerify );
            
            if ( sVerify == STL_TRUE )
            {
                sStrByte = sTempLen;
                /*
                 * char를 찾았음으로 1Char만큼 건너뛰도록.
                 */
                i = i - sStrByte + 1;
            }
            else
            {
                /* Do Nothing */
            }
            /*
             * 마지막에 찾은 char의 length의 길이가 sTempLen에 저장된다.
             */
        }
        
        if( sStrByte == aTrimCharByte )
        {
            DTS_IS_EQUAL_SAME_BYTELEN_ONE_CHAR( ( sString - sStrByte ),
                                                aTrimChar,
                                                aTrimCharByte,
                                                & sIsEqual );
            if( sIsEqual == STL_TRUE )
            {
                /* Match */
                sString -= aTrimCharByte;
                sStringByteLen -= aTrimCharByte;
                
                continue;
            }
            else
            {
                /* No Match */
                break;
            }
        }
        else
        {
            /* No Match */
            break;
        }
    }
    
    STL_DASSERT( aString <= sString );
    STL_DASSERT( sStringByteLen >= 0 );
    
    *aRemainStringLen = sStringByteLen;
    
    return STL_SUCCESS;
}

/**
 * @brief UHC character set의 trim TRAILING 처리
 * @param[in]  aString           trim_source string
 * @param[in]  aStringLen        trim_source string byte length
 * @param[in]  aTrimChar         trim_character string 
 * @param[in]  aTrimCharByte     trim_character string byte length
 * @param[out] aRemainStringLen  trim 처리후 남은 trim_source string length
 * @param[out] aErrorStack       stlErrorStack
 */
stlStatus dtfTrimTrailingForUhc( stlChar          * aString,
                                 stlInt64           aStringLen,
                                 stlChar          * aTrimChar,
                                 stlInt8            aTrimCharByte,
                                 stlInt64         * aRemainStringLen,
                                 stlErrorStack    * aErrorStack )
{
    stlChar   * sString = aString + aStringLen;
    stlInt64    sStringByteLen = aStringLen;
    stlInt8     sMaxCharLen = DTS_GET_UHC_MB_MAX_LENGTH;
    stlInt8     i = 0;
    stlInt8     sStrByte = 0;
    stlInt8     sTempLen = 0;
    stlBool     sVerify = STL_FALSE;
    stlBool     sIsEqual = STL_FALSE;
    
    while( sStringByteLen > 0  )
    {
        /**
         * 체크할 sString의 마지막 하나의 char의 length를 구한다.
         */
        
        if( sStringByteLen < sMaxCharLen )
        {
            sMaxCharLen = sStringByteLen;
        }
        else
        {
            /* Do Noghint */
        }
        
        for( i = sMaxCharLen; i > 0; i-- )
        {
            DTS_IS_VERIFY_UHC( sString - i,
                               i,
                               &sTempLen,
                               &sVerify );
            
            if ( sVerify == STL_TRUE )
            {
                sStrByte = sTempLen;
                /*
                 * char를 찾았음으로 1Char만큼 건너뛰도록.
                 */
                i = i - sStrByte + 1;
            }
            else
            {
                /* Do Nothing */
            }
            /*
             * 마지막에 찾은 char의 length의 길이가 sTempLen에 저장된다.
             */
        }
        
        if( sStrByte == aTrimCharByte )
        {
            DTS_IS_EQUAL_SAME_BYTELEN_ONE_CHAR( ( sString - sStrByte ),
                                                aTrimChar,
                                                aTrimCharByte,
                                                & sIsEqual );
            if( sIsEqual == STL_TRUE )
            {
                /* Match */
                sString -= aTrimCharByte;
                sStringByteLen -= aTrimCharByte;
                
                continue;
            }
            else
            {
                /* No Match */
                break;
            }
        }
        else
        {
            /* No Match */
            break;
        }
    }
    
    STL_DASSERT( aString <= sString );
    STL_DASSERT( sStringByteLen >= 0 );
    
    *aRemainStringLen = sStringByteLen;
    
    return STL_SUCCESS;
}

/**
 * @brief GB18030 character set의 trim TRAILING 처리
 * @param[in]  aString           trim_source string
 * @param[in]  aStringLen        trim_source string byte length
 * @param[in]  aTrimChar         trim_character string 
 * @param[in]  aTrimCharByte     trim_character string byte length
 * @param[out] aRemainStringLen  trim 처리후 남은 trim_source string length
 * @param[out] aErrorStack       stlErrorStack
 */
stlStatus dtfTrimTrailingForGb18030( stlChar          * aString,
                                     stlInt64           aStringLen,
                                     stlChar          * aTrimChar,
                                     stlInt8            aTrimCharByte,
                                     stlInt64         * aRemainStringLen,
                                     stlErrorStack    * aErrorStack )
{
    stlChar   * sString = aString + aStringLen;
    stlInt64    sStringByteLen = aStringLen;
    stlInt8     sMaxCharLen = DTS_GET_GB18030_MB_MAX_LENGTH;
    stlInt8     i = 0;
    stlInt8     sStrByte = 0;
    stlInt8     sTempLen = 0;
    stlBool     sVerify = STL_FALSE;
    stlBool     sIsEqual = STL_FALSE;

    while( sStringByteLen > 0  )
    {
        /**
         * 체크할 sString의 마지막 하나의 char의 length를 구한다.
         */
    
        if( sStringByteLen < sMaxCharLen )
        {
            sMaxCharLen = sStringByteLen;
        }
        else
        {
            /* Do Noghint */
        }
    
        for( i = sMaxCharLen; i > 0; i-- )
        {
            DTS_IS_VERIFY_GB18030( sString - i,
                                   i,
                                   &sTempLen,
                                   &sVerify );
        
            if ( sVerify == STL_TRUE )
            {
                sStrByte = sTempLen;
                /*
                 * char를 찾았음으로 1Char만큼 건너뛰도록.
                 */
                i = i - sStrByte + 1;
            }
            else
            {
                /* Do Nothing */
            }
            /*
             * 마지막에 찾은 char의 length의 길이가 sTempLen에 저장된다.
             */
        }
    
        if( sStrByte == aTrimCharByte )
        {
            DTS_IS_EQUAL_SAME_BYTELEN_ONE_CHAR( ( sString - sStrByte ),
                                                aTrimChar,
                                                aTrimCharByte,
                                                & sIsEqual );
            if( sIsEqual == STL_TRUE )
            {
                /* Match */
                sString -= aTrimCharByte;
                sStringByteLen -= aTrimCharByte;
            
                continue;
            }
            else
            {
                /* No Match */
                break;
            }
        }
        else
        {
            /* No Match */
            break;
        }
    }

    STL_DASSERT( aString <= sString );
    STL_DASSERT( sStringByteLen >= 0 );

    *aRemainStringLen = sStringByteLen;

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
