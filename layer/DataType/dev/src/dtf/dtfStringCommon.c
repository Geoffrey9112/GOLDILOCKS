/*******************************************************************************
 * dtfStringCommon.c
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
 * @file dtfStringCommon.c
 * @brief String Function DataType Library Layer
 */

#include <dtl.h>
#include <dtDef.h>
#include <dtfString.h>
#include <dtsCharacterSet.h>

/**
 * @ingroup dtfString
 * @internal
 * @{
 */

dtfMbLength2BinaryLengthFunc dtfMbLength2BinaryLengthFuncList[ DTL_CHARACTERSET_MAX + 1 ] =
{
    dtfMbLength2BinaryLengthForAscii,
    dtfMbLength2BinaryLengthForUtf8,
    dtfMbLength2BinaryLengthForUhc,
    dtfMbLength2BinaryLengthForGb18030,
    
    NULL
};


/**
 * @brief 각 characterSet의 문자에 대한 Length를 binary Length로 변경
 *           <br>aMbLength가 음수인 경우 오른쪽 부터 length이고
 *           <br>aBinLength도 음수가 된다.
 * @param[in]   aSource
 * @param[in]   aLength aSource의 Length
 * @param[in]   aMbLength character set에 의한 Length
 * @param[out]  aBinLength 결과: Binary의 Length
 * @param[out]  aIsOver 결과가 aLength범위에 있는지 여부
 * @param[in]   aVectorFunc   Function Vector
 * @param[in]   aVectorArg    Vector Argument
 * @param[out]  aErrorStack
 * @remark      변환한 BinaryPosition의 범위에 따른 결과는 아래와 같다.
 *              BinaryPosition < -aLength :
 *              => aIsOver=>STL_TRUE, aBinLength=> 음수(범위내 최소값 == -aLength)
 *              -aLength < BinaryPosition < 0 :
 *              => aIsOver=>STL_FALSE, aBinLength=> 음수
 *              BinaryPosition == 0 :
 *              => aIsOver=>STL_FALSE, aBinLength=> 0
 *              0 < BinaryPosition <= aLength:
 *              => aIsOver=>STL_FALSE, aBinLength=> 양수
 *              aLength < BinaryPosition:
 *              => aIsOver=>STL_TRUE, aBinLength=> 양수(범위내 최대값 == aLength)
 */

stlStatus dtfMbLength2BinaryLength( stlChar       * aSource,
                                    stlInt64        aLength,
                                    stlInt64        aMbLength,
                                    stlInt64      * aBinLength,
                                    stlBool       * aIsOver,
                                    dtlFuncVector * aVectorFunc,
                                    void          * aVectorArg,
                                    stlErrorStack * aErrorStack )
{
    stlChar   * sStr;
    stlInt64    sIdx;
    stlInt8     sTempLen  = 0;
    stlInt8     sTempLen2 = 0;
    stlBool     sIsOver;
    stlStatus   sVerify;
    stlInt8     sMaxCharLen;
    stlInt8     i;

    dtlCharacterSet  sCharSetID;

    /**
     * Parameter Validation
     */
    
    DTL_PARAM_VALIDATE( aSource != NULL, aErrorStack );
    DTL_PARAM_VALIDATE( aBinLength != NULL, aErrorStack );

    sIdx = 0;

    sCharSetID = aVectorFunc->mGetCharSetIDFunc(aVectorArg);
    
    /*
     * 일단 범위 내에 있다고 설정하고
     * 범위를 넘어가는 경우만 체크한다.
     */
    sIsOver = STL_FALSE;

    if( aMbLength == 0 )
    {
        *aBinLength = 0;
    }
    else if( aMbLength < 0 )
    {
        sMaxCharLen = dtsGetMbMaxLengthFuncList[sCharSetID]();
        
        /*
         * sStr의 오른쪽 부터 찾는다.
         */
        sStr = aSource + aLength;

        while( aMbLength < 0 )
        {
            if ( sIdx >= aLength )
            {
                /* 범위를 넘어 간 경우다 */
                sIsOver = STL_TRUE;
                break;
            }
            else
            {
                /* Do Nothing */
            }

            /*
             * character set기준 1글짜씩 넘어간다.
             *
             * sStr[length - 1CharMaxLen] 부터 1byte씩 찾아서
             * 마지막에 찾은것이 맨 오른쪽의 1char 이다.
             */

            /*
             * 남아 있는 byte가 sMaxCharLen보다 작으면 남은 byte만 체크한다.
             */
            if ( aLength - sIdx  < sMaxCharLen )
            {
                sMaxCharLen = aLength - sIdx;
            }
            else
            {
                /* Do Nothing */
            }

            /*
             * 'sStr - MAX char length'로 부터 char을 체크하여 마지막 char을 찾는다.
             */
            for( i = sMaxCharLen; i > 0; i-- )
            {
                sVerify = dtsMbVerifyFuncList[sCharSetID]( sStr - i,
                                                           i,
                                                           &sTempLen2,
                                                           aErrorStack );
                if ( sVerify == STL_SUCCESS )
                {
                    sTempLen = sTempLen2;
                    /*
                     * char를 찾았음으로 1Char만큼 건너뛰도록.
                     */
                    i = i - sTempLen + 1;
                }
                else
                {
                    /*
                     * aErrorStack에 쌓이지 않도록 함.
                     */
                    stlPopError( aErrorStack );
                }
                /*
                 * 마지막에 찾은 char의 length의 길이가 sTempLen에 저장된다.
                 */
            }

            sIdx += sTempLen;
            sStr -= sTempLen;
            aMbLength++;
        }

        *aBinLength = -sIdx;
    }
    else
    {
       sStr = aSource;
       while( aMbLength > 0 )
       {
           if ( sIdx >= aLength )
           {
               /* 범위를 넘어 간 경우다 */
               sIsOver = STL_TRUE;
               break;
           }
           else
           {
               /* Do Nothing */
           }
           /*
            * character set기준 1글짜씩 넘어간다.
            */
           STL_TRY( dtsGetMbLengthFuncList[sCharSetID]( sStr,
                                                        &sTempLen,
                                                        aErrorStack )
                    == STL_SUCCESS );

           sIdx += sTempLen;
           sStr += sTempLen;
           aMbLength--;
       }

       *aBinLength = sIdx;
    }

    if ( aIsOver != NULL )
    {
        *aIsOver = sIsOver;
    }
    else
    {
        /* Do Nothing */
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus dtfMbLength2BinaryLengthForAscii( stlChar       * aSource,
                                            stlInt64        aLength,
                                            stlInt64        aMbLength,
                                            stlInt64      * aBinLength,
                                            stlBool       * aIsOver,
                                            dtlFuncVector * aVectorFunc,
                                            void          * aVectorArg,
                                            stlErrorStack * aErrorStack )
{
    stlChar   * sStr;
    stlInt64    sIdx;
    stlInt8     sTempLen  = 0;
    stlInt8     sTempLen2 = 0;
    stlBool     sIsOver;
    stlBool     sVerify;
    stlInt8     sMaxCharLen;
    stlInt8     i;
    
    /**
     * Parameter Validation
     */
    
    DTL_PARAM_VALIDATE( aSource != NULL, aErrorStack );
    DTL_PARAM_VALIDATE( aBinLength != NULL, aErrorStack );
    DTL_PARAM_VALIDATE( aIsOver != NULL, aErrorStack );    
    
    sIdx = 0;
    
    /*
     * 일단 범위 내에 있다고 설정하고
     * 범위를 넘어가는 경우만 체크한다.
     */
    sIsOver = STL_FALSE;

    if( aMbLength == 0 )
    {
        *aBinLength = 0;
    }
    else if( aMbLength < 0 )
    {
        sMaxCharLen = DTS_GET_ASCII_MB_MAX_LENGTH;
        
        /*
         * sStr의 오른쪽 부터 찾는다.
         */
        sStr = aSource + aLength;
        
        while( aMbLength < 0 )
        {
            if ( sIdx >= aLength )
            {
                /* 범위를 넘어 간 경우다 */
                sIsOver = STL_TRUE;
                break;
            }
            else
            {
                /* Do Nothing */
            }
            
            /*
             * character set기준 1글짜씩 넘어간다.
             *
             * sStr[length - 1CharMaxLen] 부터 1byte씩 찾아서
             * 마지막에 찾은것이 맨 오른쪽의 1char 이다.
             */

            /*
             * 남아 있는 byte가 sMaxCharLen보다 작으면 남은 byte만 체크한다.
             */
            if ( aLength - sIdx  < sMaxCharLen )
            {
                sMaxCharLen = aLength - sIdx;
            }
            else
            {
                /* Do Nothing */
            }

            /*
             * 'sStr - MAX char length'로 부터 char을 체크하여 마지막 char을 찾는다.
             */
            for( i = sMaxCharLen; i > 0; i-- )
            {
                DTS_IS_VERIFY_ASCII( &sTempLen2,
                                     &sVerify );
                if ( sVerify == STL_TRUE )
                {
                    sTempLen = sTempLen2;
                    /*
                     * char를 찾았음으로 1Char만큼 건너뛰도록.
                     */
                    i = i - sTempLen + 1;
                }
                else
                {
                    /* Do Nothing */
                }
                /*
                 * 마지막에 찾은 char의 length의 길이가 sTempLen에 저장된다.
                 */
            }

            sIdx += sTempLen;
            sStr -= sTempLen;
            aMbLength++;
        }

        *aBinLength = -sIdx;
    }
    else
    {
        sStr = aSource;
        while( aMbLength > 0 )
        {
            if ( sIdx >= aLength )
            {
                /* 범위를 넘어 간 경우다 */
                sIsOver = STL_TRUE;
                break;
            }
            else
            {
                /* Do Nothing */
            }
            /*
             * character set기준 1글짜씩 넘어간다.
             */
            DTS_GET_ASCII_MBLENGTH( sStr,
                                    &sTempLen );
            
            sIdx += sTempLen;
            sStr += sTempLen;
            aMbLength--;
        }
        
        *aBinLength = sIdx;
    }
    
    *aIsOver = sIsOver;
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus dtfMbLength2BinaryLengthForUtf8( stlChar       * aSource,
                                           stlInt64        aLength,
                                           stlInt64        aMbLength,
                                           stlInt64      * aBinLength,
                                           stlBool       * aIsOver,
                                           dtlFuncVector * aVectorFunc,
                                           void          * aVectorArg,
                                           stlErrorStack * aErrorStack )
{
    stlChar   * sStr;
    stlInt64    sIdx;
    stlInt8     sTempLen  = 0;
    stlInt8     sTempLen2 = 0;
    stlBool     sIsOver;
    stlBool     sVerify;
    stlInt8     sMaxCharLen;
    stlInt8     i;
    
    /**
     * Parameter Validation
     */
    
    DTL_PARAM_VALIDATE( aSource != NULL, aErrorStack );
    DTL_PARAM_VALIDATE( aBinLength != NULL, aErrorStack );
    DTL_PARAM_VALIDATE( aIsOver != NULL, aErrorStack );    
    
    sIdx = 0;
    
    /*
     * 일단 범위 내에 있다고 설정하고
     * 범위를 넘어가는 경우만 체크한다.
     */
    sIsOver = STL_FALSE;

    if( aMbLength == 0 )
    {
        *aBinLength = 0;
    }
    else if( aMbLength < 0 )
    {
        sMaxCharLen = DTS_GET_UTF8_MB_MAX_LENGTH;
        
        /*
         * sStr의 오른쪽 부터 찾는다.
         */
        sStr = aSource + aLength;

        while( aMbLength < 0 )
        {
            if ( sIdx >= aLength )
            {
                /* 범위를 넘어 간 경우다 */
                sIsOver = STL_TRUE;
                break;
            }
            else
            {
                /* Do Nothing */
            }

            /*
             * character set기준 1글짜씩 넘어간다.
             *
             * sStr[length - 1CharMaxLen] 부터 1byte씩 찾아서
             * 마지막에 찾은것이 맨 오른쪽의 1char 이다.
             */

            /*
             * 남아 있는 byte가 sMaxCharLen보다 작으면 남은 byte만 체크한다.
             */
            if ( aLength - sIdx  < sMaxCharLen )
            {
                sMaxCharLen = aLength - sIdx;
            }
            else
            {
                /* Do Nothing */
            }
            
            /*
             * 'sStr - MAX char length'로 부터 char을 체크하여 마지막 char을 찾는다.
             */
            for( i = sMaxCharLen; i > 0; i-- )
            {
                DTS_IS_VERIFY_UTF8( sStr - i,
                                    i,
                                    &sTempLen2,
                                    &sVerify );
                if ( sVerify == STL_TRUE )
                {
                    sTempLen = sTempLen2;
                    /*
                     * char를 찾았음으로 1Char만큼 건너뛰도록.
                     */
                    i = i - sTempLen + 1;
                }
                else
                {
                    /* Do Nothing */
                }
                /*
                 * 마지막에 찾은 char의 length의 길이가 sTempLen에 저장된다.
                 */
            }

            sIdx += sTempLen;
            sStr -= sTempLen;
            aMbLength++;
        }

        *aBinLength = -sIdx;
    }
    else
    {
       sStr = aSource;
       while( aMbLength > 0 )
       {
           if ( sIdx >= aLength )
           {
               /* 범위를 넘어 간 경우다 */
               sIsOver = STL_TRUE;
               break;
           }
           else
           {
               /* Do Nothing */
           }
           /*
            * character set기준 1글짜씩 넘어간다.
            */
           DTS_GET_UTF8_MBLENGTH( sStr,
                                  &sTempLen );
           
           sIdx += sTempLen;
           sStr += sTempLen;
           aMbLength--;
       }
       
       *aBinLength = sIdx;
    }

    *aIsOver = sIsOver;    
    
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


stlStatus dtfMbLength2BinaryLengthForUhc( stlChar       * aSource,
                                          stlInt64        aLength,
                                          stlInt64        aMbLength,
                                          stlInt64      * aBinLength,
                                          stlBool       * aIsOver,
                                          dtlFuncVector * aVectorFunc,
                                          void          * aVectorArg,
                                          stlErrorStack * aErrorStack )
{
    stlChar   * sStr;
    stlInt64    sIdx;
    stlInt8     sTempLen  = 0;
    stlInt8     sTempLen2 = 0;
    stlBool     sIsOver;
    stlBool     sVerify;
    stlInt8     sMaxCharLen;
    stlInt8     i;
    
    /**
     * Parameter Validation
     */
    
    DTL_PARAM_VALIDATE( aSource != NULL, aErrorStack );
    DTL_PARAM_VALIDATE( aBinLength != NULL, aErrorStack );
    DTL_PARAM_VALIDATE( aIsOver != NULL, aErrorStack );    
    
    sIdx = 0;
    
    /*
     * 일단 범위 내에 있다고 설정하고
     * 범위를 넘어가는 경우만 체크한다.
     */
    sIsOver = STL_FALSE;

    if( aMbLength == 0 )
    {
        *aBinLength = 0;
    }
    else if( aMbLength < 0 )
    {
        sMaxCharLen = DTS_GET_UHC_MB_MAX_LENGTH;
        
        /*
         * sStr의 오른쪽 부터 찾는다.
         */
        sStr = aSource + aLength;

        while( aMbLength < 0 )
        {
            if ( sIdx >= aLength )
            {
                /* 범위를 넘어 간 경우다 */
                sIsOver = STL_TRUE;
                break;
            }
            else
            {
                /* Do Nothing */
            }

            /*
             * character set기준 1글짜씩 넘어간다.
             *
             * sStr[length - 1CharMaxLen] 부터 1byte씩 찾아서
             * 마지막에 찾은것이 맨 오른쪽의 1char 이다.
             */

            /*
             * 남아 있는 byte가 sMaxCharLen보다 작으면 남은 byte만 체크한다.
             */
            if ( aLength - sIdx  < sMaxCharLen )
            {
                sMaxCharLen = aLength - sIdx;
            }
            else
            {
                /* Do Nothing */
            }

            /*
             * 'sStr - MAX char length'로 부터 char을 체크하여 마지막 char을 찾는다.
             */
            for( i = sMaxCharLen; i > 0; i-- )
            {
                DTS_IS_VERIFY_UHC( sStr - i,
                                   i,
                                   &sTempLen2,
                                   &sVerify );
                if ( sVerify == STL_TRUE )
                {
                    sTempLen = sTempLen2;
                    /*
                     * char를 찾았음으로 1Char만큼 건너뛰도록.
                     */
                    i = i - sTempLen + 1;
                }
                else
                {
                    /* Do Nothing */
                }
                /*
                 * 마지막에 찾은 char의 length의 길이가 sTempLen에 저장된다.
                 */
            }

            sIdx += sTempLen;
            sStr -= sTempLen;
            aMbLength++;
        }

        *aBinLength = -sIdx;
    }
    else
    {
       sStr = aSource;
       while( aMbLength > 0 )
       {
           if ( sIdx >= aLength )
           {
               /* 범위를 넘어 간 경우다 */
               sIsOver = STL_TRUE;
               break;
           }
           else
           {
               /* Do Nothing */
           }
           /*
            * character set기준 1글짜씩 넘어간다.
            */
           DTS_GET_UHC_MBLENGTH( sStr,
                                 &sTempLen );
           
           sIdx += sTempLen;
           sStr += sTempLen;
           aMbLength--;
       }
       
       *aBinLength = sIdx;
    }
    
    *aIsOver = sIsOver;
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}




stlStatus dtfMbLength2BinaryLengthForGb18030( stlChar       * aSource,
                                              stlInt64        aLength,
                                              stlInt64        aMbLength,
                                              stlInt64      * aBinLength,
                                              stlBool       * aIsOver,
                                              dtlFuncVector * aVectorFunc,
                                              void          * aVectorArg,
                                              stlErrorStack * aErrorStack )
{
    stlChar   * sStr;
    stlInt64    sIdx;
    stlInt8     sTempLen  = 0;
    stlInt8     sTempLen2 = 0;
    stlBool     sIsOver;
    stlBool     sVerify;
    stlInt8     sMaxCharLen;
    stlInt8     i;
    
    /**
     * Parameter Validation
     */
    
    DTL_PARAM_VALIDATE( aSource != NULL, aErrorStack );
    DTL_PARAM_VALIDATE( aBinLength != NULL, aErrorStack );
    DTL_PARAM_VALIDATE( aIsOver != NULL, aErrorStack );    
    
    sIdx = 0;
    
    /*
     * 일단 범위 내에 있다고 설정하고
     * 범위를 넘어가는 경우만 체크한다.
     */
    sIsOver = STL_FALSE;

    if( aMbLength == 0 )
    {
        *aBinLength = 0;
    }
    else if( aMbLength < 0 )
    {
        sMaxCharLen = DTS_GET_GB18030_MB_MAX_LENGTH;
        
        /*
         * sStr의 오른쪽 부터 찾는다.
         */
        sStr = aSource + aLength;

        while( aMbLength < 0 )
        {
            if ( sIdx >= aLength )
            {
                /* 범위를 넘어 간 경우다 */
                sIsOver = STL_TRUE;
                break;
            }
            else
            {
                /* Do Nothing */
            }

            /*
             * character set기준 1글짜씩 넘어간다.
             *
             * sStr[length - 1CharMaxLen] 부터 1byte씩 찾아서
             * 마지막에 찾은것이 맨 오른쪽의 1char 이다.
             */

            /*
             * 남아 있는 byte가 sMaxCharLen보다 작으면 남은 byte만 체크한다.
             */
            if ( aLength - sIdx  < sMaxCharLen )
            {
                sMaxCharLen = aLength - sIdx;
            }
            else
            {
                /* Do Nothing */
            }

            /*
             * 'sStr - MAX char length'로 부터 char을 체크하여 마지막 char을 찾는다.
             */
            for( i = sMaxCharLen; i > 0; i-- )
            {
                DTS_IS_VERIFY_GB18030( sStr - i,
                                       i,
                                       &sTempLen2,
                                       &sVerify );
                if ( sVerify == STL_TRUE )
                {
                    sTempLen = sTempLen2;
                    /*
                     * char를 찾았음으로 1Char만큼 건너뛰도록.
                     */
                    i = i - sTempLen + 1;
                }
                else
                {
                    /* Do Nothing */
                }
                /*
                 * 마지막에 찾은 char의 length의 길이가 sTempLen에 저장된다.
                 */
            }

            sIdx += sTempLen;
            sStr -= sTempLen;
            aMbLength++;
        }

        *aBinLength = -sIdx;
    }
    else
    {
       sStr = aSource;
       while( aMbLength > 0 )
       {
           if ( sIdx >= aLength )
           {
               /* 범위를 넘어 간 경우다 */
               sIsOver = STL_TRUE;
               break;
           }
           else
           {
               /* Do Nothing */
           }
           /*
            * character set기준 1글짜씩 넘어간다.
            */
           DTS_GET_GB18030_MBLENGTH( sStr,
                                     &sTempLen );
           
           sIdx += sTempLen;
           sStr += sTempLen;
           aMbLength--;
       }
       
       *aBinLength = sIdx;
    }
    
    *aIsOver = sIsOver;
    
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
 * @brief 각 characterSet의 문자에 대한 Position를 binary Position로 변경
 *           결과인 aBinPosition는 0 or 양의 정수만 나온다.
 * @param[in]   aCharSetID          
 * @param[in]   aSource
 * @param[in]   aLength aSource의 Length
 * @param[in]   aMbPosition character set에 의한 Position
 * @param[out]  aBinPosition 결과: Binary의 Position
 * @param[out]  aIsOver 결과가 aLength범위에 있는지 여부
 * @param[in]   aVectorFunc  Function Vector
 * @param[in]   aVectorArg   Vector Argument
 * @param[out]  aErrorStack
 * @remark      변환한 BinaryPosition의 범위에 따른 결과는 아래와 같다.
 *              BinaryPosition < -aLength :
 *              => aIsOver=>STL_TRUE, aBinPosition=> 0
 *              -aLength <= BinaryPosition < 0 :
 *              => aIsOver=>STL_FALSE, aBinPosition=> 양수(-aLength)
 *              BinaryPosition == 0:
 *              => aIsOver=>STL_FALSE, aBinPosition=> 1 (oracle의 subString 정책)
 *              0 < BinaryPosition <= aLength:
 *              => aIsOver=>STL_FALSE, aBinPosition=> 양수(aLength)
 *              aLength < BinaryPosition:
 *              => aIsOver=>STL_TRUE, aBinPosition=> 0
 */
stlStatus dtfMbPosition2BinaryPosition( dtlCharacterSet aCharSetID,
                                        stlChar       * aSource,
                                        stlInt64        aLength,
                                        stlInt64        aMbPosition,
                                        stlInt64      * aBinPosition,
                                        stlBool       * aIsOver,
                                        dtlFuncVector * aVectorFunc,
                                        void          * aVectorArg,
                                        stlErrorStack * aErrorStack )
{
    stlBool     sIsOver;
    
    DTL_PARAM_VALIDATE( aSource != NULL, aErrorStack );
    DTL_PARAM_VALIDATE( aLength > 0, aErrorStack );
    DTL_PARAM_VALIDATE( aBinPosition != NULL, aErrorStack );
    DTL_PARAM_VALIDATE( aIsOver != NULL, aErrorStack );

    /* character set이 추가될때 그에 따른 function list가 누락되는 일이 없도록 하기 위한 검사 코드 */
    DTF_CHECK_CHAR_SET_FUNC_LIST( dtfMbLength2BinaryLengthFuncList );    
    
    /*
     * 일단 범위 내에 있다고 설정하고
     * 범위를 넘어가는 경우만 체크한다.
     */
    sIsOver = STL_FALSE;
    
    /*
     * position의 시작은 1인 가정에서 처리됨. (0 이 아님)
     * 예) SUBSTR( 'ABC', 1, 1 ) => 'A'
     */
    STL_DASSERT( aMbPosition != 0 );
    
    if( aMbPosition == 1 )
    {
        /* aMbPosition이 1이면 무조건 1 이다. */
        *aBinPosition = 1;
    }
    else if( aMbPosition > 1 )
    {
        /*
         * position은 length(position-1)+1 로 계산한다.
         * position은 이전 글자까지의 길이 + 1 이기 때문임.
         */
        STL_TRY( dtfMbLength2BinaryLengthFuncList[ aCharSetID ]( aSource,
                                                                 aLength,
                                                                 aMbPosition - 1,
                                                                 aBinPosition,
                                                                 &sIsOver,
                                                                 aVectorFunc,
                                                                 aVectorArg,
                                                                 aErrorStack )
                 == STL_SUCCESS );
        if ( sIsOver == STL_TRUE )
        {
            /* 범위를 벗어난 경우 */
            *aBinPosition = 0;
        }
        else
        {
            (*aBinPosition)++;

            /*
             * 여기서 ++를 하기 때문에 dtfMbLength2BinaryLength()에서
             * 범위를 벗어나는지 체크하지 못해서 여기서 다시 체크 한다.
             */
            if ( *aBinPosition > aLength )
            {
                *aBinPosition = 0;
                sIsOver = STL_TRUE;
            }
            else
            {
                /* Do Nothing */
            }
        }
    }
    else    /* aMbPosition < 0 */
    {
        /*
         * aMbPosition이 음수면 오른쪽 부터 계산한다
         * 음수 position은 전체 length - length(position) + 1 으로 계산한다.
         */

        STL_TRY( dtfMbLength2BinaryLengthFuncList[ aCharSetID ]( aSource,
                                                                 aLength,
                                                                 aMbPosition,
                                                                 aBinPosition,
                                                                 &sIsOver,
                                                                 aVectorFunc,
                                                                 aVectorArg,
                                                                 aErrorStack )
                 == STL_SUCCESS );
        
        
        if ( sIsOver == STL_TRUE )
        {
            *aBinPosition = 0;
        }
        else
        {
            /*
             * *aBinPosition 자체가 음수임으로 +를 해야 함.
             */
            *aBinPosition = aLength + *aBinPosition + 1;
        }
    }

    *aIsOver = sIsOver;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} */
