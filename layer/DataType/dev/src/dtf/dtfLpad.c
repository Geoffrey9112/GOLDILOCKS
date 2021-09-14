/*******************************************************************************
 * dtfLpad.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id$
 *
 * NOTES
 * 
 ******************************************************************************/

/**
 * @file dtfLpad.c
 * @brief String Function DataType Library Layer
 */

#include <dtl.h>
#include <dtDef.h>
#include <dtlDef.h>
#include <dtsCharacterSet.h>
#include <dtfNumeric.h>

/*******************************************************************************
 *   Lpad
 *   Lpad ==> [ P, O, M]
 * ex) Lpad( 'aa',5,'b' ) => bbbaa
 *******************************************************************************/

/**
 * @brief Lpad str의 왼쪽부터 문자열 fill 문자를 추가해 len의 길이를 채운 string을 반환한다. 
 *        <BR> Lpad(str, length len, [, fill])
 *        <BR>   Lpad => [ P, O, M ]
 *        <BR> ex) Lpad( '',2,'' )  =>  null
 *        <BR> ex) Lpad( 'aa',0,'b' )  =>  null
 *        <BR> ex) Lpad( 'aa',5,'b' )  =>  bbbaa
 *        <BR> ex) Lpad( 'aa',5 )  =>     aa
 *        
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> Lpad( VARCHAR, INTEGER, VARCHAR )
 * @param[out] aResultValue      연산 결과 (결과타입 VARCHAR)
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfStringLpad( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         dtlFuncVector  * aVectorFunc,
                         void           * aVectorArg,
                         void           * aEnv )
{
    /**
     * 논의대상.
     * Multibyte character 처리. 현재는 PostgreSQL를 따라가고 있음.
     *
     *   SELECT LPAD('a',5,'가') FROM DUAL;
     *
     *    Oracle     => [ 가가a     ] : '가' 2byte
     *    Postgrel   => [ 가가가가a ] : '가' 1byte
     *    Mysql      => [ 가 �a     ] : '가' 3byte
     **/
    
    dtlDataValue    * sValue1;
    dtlDataValue    * sValue2;
    dtlDataValue    * sValue3;
    dtlDataValue    * sResultValue;
    stlErrorStack   * sErrorStack;
    dtlCharacterSet   sCharSetID;
    
    dtlVarcharType    sResult;
    dtlVarcharType    sHeadStr;
    dtlVarcharType    sTailStr;
    stlInt64          sHeadStrLen;
    stlInt64          sTailStrLen;
    stlInt32          sTotalLen;

    stlInt8           sStrMbLen;
    stlInt64          sStrLen;
    stlInt64          sFillLen;
    stlInt64          sTailFillLen;
    stlInt64          sResultLen   = 0;
    stlInt64          sStrCountLen = 0;

    stlInt64          sTailChrCount = 0;
    stlInt64          sTailLenCount = 0;
    
    DTL_PARAM_VALIDATE( ( aInputArgumentCnt == 2 ) || ( aInputArgumentCnt == 3 ) , (stlErrorStack *)aEnv );

    sValue1      = aInputArgument[0].mValue.mDataValue;
    sValue2      = aInputArgument[1].mValue.mDataValue;
    sValue3      = NULL;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack  = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE2( sValue1,
                          DTL_TYPE_CHAR,
                          DTL_TYPE_VARCHAR,
                          sErrorStack );
    
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_NATIVE_INTEGER, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_VARCHAR, sErrorStack );

    if( aInputArgumentCnt == 3 )
    {
        sValue3 = aInputArgument[2].mValue.mDataValue;
        DTF_CHECK_DTL_VALUE2( sValue3,
                              DTL_TYPE_CHAR,
                              DTL_TYPE_VARCHAR,
                              sErrorStack );
    }
    else
    {
        /* Do Nothing */
    }
    
    sCharSetID   = aVectorFunc->mGetCharSetIDFunc( aVectorArg );
    sHeadStr     = DTF_VARCHAR( sValue1 );
    sTotalLen    = DTF_INTEGER( sValue2 );
    sResult      = DTF_VARCHAR( sResultValue );

    /* varchar overflow check */
    STL_TRY_THROW( sTotalLen <= DTL_VARCHAR_MAX_PRECISION, ERROR_OUT_OF_RANGE );
    
    /* sTotalLen의 음수처리 */
    if( sTotalLen < 0 )
    {
        sResultLen = 0;
    }
    else
    {
        /**
         * sHeadStr의 sStrLen(character length)를 구한 후
         * sTotalLen - sStrLen = sFillLen를 통해 sTailStr를 넣을 공간을 확인한다.
         * sFillLen < 0 : sHeadStr이 sTotalLen보다 큰경우 sTotalLen만큼 sStr을 복사한다.
         * sFillLen = 0 : sHeadStr과 sTotalLen이 같으므로 바로 복사해주면된다.
         * sFillLen > 0 : sFillLen만큼 sTailStr Character를 복사한다.
         **/
        sHeadStrLen = sValue1->mLength;
        
        STL_TRY( dtsGetMbstrlenWithLenFuncList[ sCharSetID ]( sHeadStr,
                                                              sHeadStrLen,
                                                              &sStrLen,
                                                              aVectorFunc,
                                                              aVectorArg,
                                                              sErrorStack )
                 == STL_SUCCESS );                
        
        sFillLen = sTotalLen - sStrLen;
        
        if( sFillLen == 0 )
        {
            /**
             * ex ) SELECT LPAD( 'aa', 2, 'b' ) FROM DUAL;
             *
             * result
             * -------
             *   aa
             **/            
            stlMemcpy( sResult,
                       sHeadStr,
                       sHeadStrLen );

            sResultLen = sHeadStrLen;            
        }
        else if( sFillLen < 0 )
        {
            /**
             * sHeadStr을 sTotalLen만큼 sResult에 복사한다.
             *
             *  ex ) SELECT LPAD( 'aaa', 2, 'b' ) FROM DUAL;
             *
             * result
             * -------
             *   aa
             **/                
            while( sTotalLen > 0 )
            {
                STL_TRY( dtsGetMbLengthFuncList[sCharSetID]( sHeadStr,
                                                             &sStrMbLen,
                                                             sErrorStack )
                         == STL_SUCCESS );

                sHeadStr   += sStrMbLen;
                sResultLen += sStrMbLen;
                sTotalLen --;
            }            
            stlMemcpy( sResult,
                       sValue1->mValue,
                       sResultLen );
        }
        else
        {
            /**
             * sTailStr가 생략된 경우,
             * SELECT LPAD( 'a', 3 ) FROM DUAL  => [   ]a
             * sTailStr대신 FillLen만큼 WHITE SPACE 를 채워준다.
             **/
            if( aInputArgumentCnt == 2 )
            {
                stlMemset( sResult,
                           ' ',
                           sFillLen );

                sResult    += sFillLen;
                sResultLen = sFillLen;
            }
            else
            {
                /**
                 * sFillLen만큼 sTailStr를 sResult에 복사한다.
                 * 그리고 나머지 sHeadStr를 sResult에 복사한다.
                 **/
                sTailStr = DTF_VARCHAR( sValue3 );
                sTailStrLen = sValue3->mLength;
                
                STL_TRY( dtsGetMbstrlenWithLenFuncList[ sCharSetID ]( sTailStr,
                                                                      sTailStrLen,
                                                                      &sStrLen,
                                                                      aVectorFunc,
                                                                      aVectorArg,
                                                                      sErrorStack )
                         == STL_SUCCESS );

                /* sFillLen / sStrLen로 TailStr이 반복하며 채워야 할 공간 확인*/
                sTailFillLen = sFillLen / sStrLen;

                if( sTailFillLen > 0 )
                {
                    /*
                     * sTailFillLen이 1번 이상이므로 복사를 위해 미리 한번 수행한다
                     */
                    sResultLen = ( sTailStrLen * sTailFillLen );

                    stlMemcpy( sResult,
                               sTailStr,
                               (stlSize)sTailStrLen );
                    sTailLenCount += sTailStrLen;
                    sTailFillLen --;
                    sTailChrCount ++;
                    
                    while( sTailFillLen > 0 )
                    {
                        if( sTailChrCount <= sTailFillLen )
                        {
                            /*
                             * sTailStr을 sResultVal에 반복하여 넣는데,
                             * 만약 이전에 넣은 값의 두배를 추가로 넣어도 sTailFillLen을 넘지 않으면
                             * 2의 배수로 계속 넣는다. 그렇지 않으면 남은 값은 반복하여 넣는다
                             *
                             * => a + a + aa + aaaa + aaaaaaaa ...
                             *  
                             */
                            
                            stlMemcpy( ( sResult + sTailLenCount ),
                                       sResult,
                                       (stlSize)sTailLenCount );

                            sTailFillLen -= sTailChrCount;
                            sTailLenCount *= 2;
                            sTailChrCount *= 2;
                        }
                        else
                        {
                            /*
                             * 남은 부분은 횟수만큼 넣는다 
                             * 
                             * => a + a + aa + aaaa + aaaaaaaa + [ aaaa ]
                             *    ^........... 16 .............^ [ ^ 4 ^ ]
                             */
                            
                            stlMemcpy( ( sResult + sTailLenCount ),
                                       sResult,
                                       (stlSize)( sTailStrLen * sTailFillLen ) );
                            sTailFillLen = 0;
                        }
                    }                    
                    sResult += sResultLen;
                }
                else
                {
                    /* Do Nothing */
                }
                
                /* sFillLen % sStrLen로 TailStr의 일부분으로만 채워야 할 공간 확인 */
                sTailFillLen = sFillLen % sStrLen;
                
                while( sTailFillLen > 0 )
                {
                    /**
                     * 만약 TailStr에 일부분만 넣어야한다면 나머지만큼 잘라 넣는다.
                     * ex ) SELECT LPAD( 'aa', 7, 'bc' ) FROM DUAL;
                     *
                     *  result
                     * ---------
                     *  bcbcbaa    (bcbc반복시, 나머지b 입력과정)
                     *
                     *  ex ) SELECT LPAD( 'a', 5, 'abcdef' ) FROM DUAL;
                     *
                     *  result
                     * ---------
                     *  abcda     (abcde중 TailFillLen만큼의 길이로 잘라 넣는다.) 
                     **/
                    STL_TRY( dtsGetMbLengthFuncList[sCharSetID]( sTailStr,
                                                                 &sStrMbLen,
                                                                 sErrorStack )
                             == STL_SUCCESS );

                    sTailStr     += sStrMbLen;
                    sStrCountLen += sStrMbLen;
                    sTailFillLen --;
                }
                stlMemcpy( sResult,
                           sValue3->mValue,
                           sStrCountLen );
                    
                sResult    += sStrCountLen;
                sResultLen += sStrCountLen;
                
            }
            /**
             * ex1 ) SELECT LPAD( 'aa', 5, 'b' ) FROM DUAL;
             *
             *  result
             * ---------
             *  bbb(aa)
             *
             **/
            stlMemcpy( sResult,
                       sHeadStr,
                       sHeadStrLen );

            sResultLen += sHeadStrLen;
        }
    }

    sResultValue->mLength = sResultLen;
    
    return STL_SUCCESS;

    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_STRING_CHARACTER_LENGTH_GREATER_THAN_COLUMN_LENGTH,
                      NULL,
                      sErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/*******************************************************************************
 *   Lpad
 *   Lpad ==> [ P, O, M]
 * ex) Lpad( 'aa',5,'b' ) => bbbaa
 *******************************************************************************/

/**
 * @brief Lpad str의 왼쪽부터 문자열 fill 문자를 추가해 len의 길이를 채운 string을 반환한다. 
 *        <BR> Lpad(str, length len, [, fill])
 *        <BR>   Lpad => [ P, O, M ]
 *        <BR> ex) Lpad( '',2,'' )  =>  null
 *        <BR> ex) Lpad( 'aa',0,'b' )  =>  null
 *        <BR> ex) Lpad( 'aa',5,'b' )  =>  bbbaa
 *        <BR> ex) Lpad( 'aa',5 )  =>     aa
 *        
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> Lpad( LONGVARCHAR_GROUP | CHAR_GROUP, INTEGER,
 *                                          LONGVARCHAR_GROUP | CHAR_GROUP )
 * @param[out] aResultValue      연산 결과 (결과타입 LONGVARCHAR)
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfLongvarcharStringLpad( stlUInt16        aInputArgumentCnt,
                                    dtlValueEntry  * aInputArgument,
                                    void           * aResultValue,
                                    dtlFuncVector  * aVectorFunc,
                                    void           * aVectorArg,
                                    void           * aEnv )
{
    /**
     * 논의대상.
     * Multibyte character 처리. 현재는 PostgreSQL를 따라가고 있음.
     *
     *   SELECT LPAD('a',5,'가') FROM DUAL;
     *
     *    Oracle     => [ 가가a     ] : '가' 2byte
     *    Postgrel   => [ 가가가가a ] : '가' 1byte
     *    Mysql      => [ 가 �a     ] : '가' 3byte
     **/
    
    dtlDataValue    * sValue1;
    dtlDataValue    * sValue2;
    dtlDataValue    * sValue3;
    dtlDataValue    * sResultValue;
    stlErrorStack   * sErrorStack;
    dtlCharacterSet   sCharSetID;
    
    dtlVarcharType    sResult;
    dtlVarcharType    sHeadStr;
    dtlVarcharType    sTailStr;
    stlInt64          sHeadStrLen;
    stlInt64          sTailStrLen;
    stlInt32          sTotalLen;

    stlInt8           sStrMbLen;
    stlInt64          sStrLen;
    stlInt64          sFillLen;
    stlInt64          sTailFillLen;
    stlInt64          sResultLen   = 0;
    stlInt64          sStrCountLen = 0;

    stlInt64          sTailChrCount = 0;
    stlInt64          sTailLenCount = 0;
    
    DTL_PARAM_VALIDATE( ( aInputArgumentCnt == 2 ) || ( aInputArgumentCnt == 3 ) , (stlErrorStack *)aEnv );

    sValue1      = aInputArgument[0].mValue.mDataValue;
    sValue2      = aInputArgument[1].mValue.mDataValue;
    sValue3      = NULL;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack  = (stlErrorStack *)aEnv;
    
    DTF_CHECK_DTL_GROUP2( sValue1,
                          DTL_GROUP_CHAR_STRING,
                          DTL_GROUP_LONGVARCHAR_STRING,
                          sErrorStack );
    
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_NATIVE_INTEGER, sErrorStack );
    
    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_LONGVARCHAR, sErrorStack );
    
    if( aInputArgumentCnt == 3 )
    {
        sValue3 = aInputArgument[2].mValue.mDataValue;
        DTF_CHECK_DTL_GROUP2( sValue3,
                              DTL_GROUP_CHAR_STRING,
                              DTL_GROUP_LONGVARCHAR_STRING,
                              sErrorStack );
    }
    else
    {
        /* Do Nothing */
    }
    
    sCharSetID   = aVectorFunc->mGetCharSetIDFunc( aVectorArg );
    sHeadStr     = DTF_VARCHAR( sValue1 );
    sTotalLen    = DTF_INTEGER( sValue2 );
    sResult      = DTF_VARCHAR( sResultValue );

    /* long varchar overflow check */
    STL_TRY_THROW( sTotalLen <= DTL_LONGVARCHAR_MAX_PRECISION, ERROR_OUT_OF_RANGE );
    
    /* sTotalLen의 음수처리 */
    if( sTotalLen < 0 )
    {
        sResultLen = 0;
    }
    else
    {
        /**
         * sHeadStr의 sStrLen(character length)를 구한 후
         * sTotalLen - sStrLen = sFillLen를 통해 sTailStr를 넣을 공간을 확인한다.
         * sFillLen < 0 : sHeadStr이 sTotalLen보다 큰경우 sTotalLen만큼 sStr을 복사한다.
         * sFillLen = 0 : sHeadStr과 sTotalLen이 같으므로 바로 복사해주면된다.
         * sFillLen > 0 : sFillLen만큼 sTailStr Character를 복사한다.
         **/
        sHeadStrLen = sValue1->mLength;
        
        STL_TRY( dtsGetMbstrlenWithLenFuncList[ sCharSetID ]( sHeadStr,
                                                              sHeadStrLen,
                                                              &sStrLen,
                                                              aVectorFunc,
                                                              aVectorArg,
                                                              sErrorStack )
                 == STL_SUCCESS );                
        
        sFillLen = sTotalLen - sStrLen;
        
        if( sFillLen == 0 )
        {
            /**
             * ex ) SELECT LPAD( 'aa', 2, 'b' ) FROM DUAL;
             *
             * result
             * -------
             *   aa
             **/            
            stlMemcpy( sResult,
                       sHeadStr,
                       sHeadStrLen );

            sResultLen = sHeadStrLen;
        }
        else if( sFillLen < 0 )
        {
            /**
             * sHeadStr을 sTotalLen만큼 sResult에 복사한다.
             *
             *  ex ) SELECT LPAD( 'aaa', 2, 'b' ) FROM DUAL;
             *
             * result
             * -------
             *   aa
             **/                
            while( sTotalLen > 0 )
            {
                STL_TRY( dtsGetMbLengthFuncList[sCharSetID]( sHeadStr,
                                                             &sStrMbLen,
                                                             sErrorStack )
                         == STL_SUCCESS );

                sHeadStr   += sStrMbLen;
                sResultLen += sStrMbLen;
                sTotalLen --;
            }            
            stlMemcpy( sResult,
                       sValue1->mValue,
                       sResultLen );
        }
        else
        {
            /**
             * sTailStr가 생략된 경우,
             * SELECT LPAD( 'a', 3 ) FROM DUAL  => [   ]a
             * sTailStr대신 FillLen만큼 WHITE SPACE 를 채워준다.
             **/
            if( aInputArgumentCnt == 2 )
            {
                STL_TRY_THROW( sFillLen <= DTL_LONGVARCHAR_MAX_PRECISION,
                               ERROR_OUT_OF_RANGE );
                
                stlMemset( sResult,
                           ' ',
                           sFillLen );

                sResult    += sFillLen;
                sResultLen = sFillLen;
            }
            else
            {
                /**
                 * sFillLen만큼 sTailStr를 sResult에 복사한다.
                 * 그리고 나머지 sHeadStr를 sResult에 복사한다.
                 **/
                sTailStr = DTF_VARCHAR( sValue3 );
                sTailStrLen = sValue3->mLength;
                
                STL_TRY( dtsGetMbstrlenWithLenFuncList[ sCharSetID ]( sTailStr,
                                                                      sTailStrLen,
                                                                      &sStrLen,
                                                                      aVectorFunc,
                                                                      aVectorArg,
                                                                      sErrorStack )
                         == STL_SUCCESS );
                
                /* sFillLen / sStrLen로 TailStr이 반복하며 채워야 할 공간 확인*/
                sTailFillLen = sFillLen / sStrLen;

                if( sTailFillLen > 0 )
                {
                    /*
                     * sTailFillLen이 1번 이상이므로 복사를 위해 미리 한번 수행한다
                     */
                    sResultLen = ( sTailStrLen * sTailFillLen );

                    STL_TRY_THROW( sResultLen <= DTL_LONGVARCHAR_MAX_PRECISION,
                                   ERROR_OUT_OF_RANGE );

                    stlMemcpy( sResult,
                               sTailStr,
                               (stlSize)sTailStrLen );
                    sTailLenCount += sTailStrLen;
                    sTailFillLen --;
                    sTailChrCount ++;
                    
                    while( sTailFillLen > 0 )
                    {
                        if( sTailChrCount <= sTailFillLen )
                        {
                            /*
                             * sTailStr을 sResultVal에 반복하여 넣는데,
                             * 만약 이전에 넣은 값의 두배를 추가로 넣어도 sTailFillLen을 넘지 않으면
                             * 2의 배수로 계속 넣는다. 그렇지 않으면 남은 값은 반복하여 넣는다
                             *
                             * => a + a + aa + aaaa + aaaaaaaa ...
                             *  
                             */
                            
                            stlMemcpy( ( sResult + sTailLenCount ),
                                       sResult,
                                       (stlSize)sTailLenCount );

                            sTailFillLen -= sTailChrCount;
                            sTailLenCount *= 2;
                            sTailChrCount *= 2;
                        }
                        else
                        {
                            /*
                             * 남은 부분은 횟수만큼 넣는다 
                             * 
                             * => a + a + aa + aaaa + aaaaaaaa + [ aaaa ]
                             *    ^........... 16 .............^ [ ^ 4 ^ ]
                             */
                            
                            stlMemcpy( ( sResult + sTailLenCount ),
                                       sResult,
                                       (stlSize)( sTailStrLen * sTailFillLen ) );
                            sTailFillLen = 0;
                        }
                    }                    
                    sResult += sResultLen;
                }
                else
                {
                    /* Do Nothing */
                }
                
                /* sFillLen % sStrLen로 TailStr의 일부분으로만 채워야 할 공간 확인 */
                sTailFillLen = sFillLen % sStrLen;
                
                while( sTailFillLen > 0 )
                {
                    /**
                     * 만약 TailStr에 일부분만 넣어야한다면 나머지만큼 잘라 넣는다.
                     * ex ) SELECT LPAD( 'aa', 7, 'bc' ) FROM DUAL;
                     *
                     *  result
                     * ---------
                     *  bcbcbaa    (bcbc반복시, 나머지b 입력과정)
                     *
                     *  ex ) SELECT LPAD( 'a', 5, 'abcdef' ) FROM DUAL;
                     *
                     *  result
                     * ---------
                     *  abcda     (abcde중 TailFillLen만큼의 길이로 잘라 넣는다.) 
                     **/
                    STL_TRY( dtsGetMbLengthFuncList[sCharSetID]( sTailStr,
                                                                 &sStrMbLen,
                                                                 sErrorStack )
                             == STL_SUCCESS );

                    sTailStr     += sStrMbLen;
                    sStrCountLen += sStrMbLen;
                    sTailFillLen --;
                }

                STL_TRY_THROW( sResultLen + sStrCountLen <= DTL_LONGVARCHAR_MAX_PRECISION,
                               ERROR_OUT_OF_RANGE );
                
                stlMemcpy( sResult,
                           sValue3->mValue,
                           sStrCountLen );
                    
                sResult    += sStrCountLen;
                sResultLen += sStrCountLen;
                
            }

            /**
             * ex1 ) SELECT LPAD( 'aa', 5, 'b' ) FROM DUAL;
             *
             *  result
             * ---------
             *  bbb(aa)
             *
             **/
            
            STL_TRY_THROW( sResultLen + sHeadStrLen <= DTL_LONGVARCHAR_MAX_PRECISION,
                           ERROR_OUT_OF_RANGE );
            stlMemcpy( sResult,
                       sHeadStr,
                       sHeadStrLen );

            sResultLen += sHeadStrLen;
        }
    }

    sResultValue->mLength = sResultLen;
    
    return STL_SUCCESS;

    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_STRING_CHARACTER_LENGTH_GREATER_THAN_COLUMN_LENGTH,
                      NULL,
                      sErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Lpad str의 왼쪽부터 문자열 fill 문자를 추가해 len의 길이를 채운 binary string을 반환한다. 
 *        <BR> Lpad(str, length len, [, fill])
 *        <BR>   Lpad => [ P, O, M ]
 *        <BR> ex) Lpad( '',2,'' )  =>  null
 *        <BR> ex) Lpad( 'aa',0,'b' )  =>  null
 *        <BR> ex) Lpad( 'aa',5,'b' )  =>  bbbaa
 *        <BR> ex) Lpad( 'aa',5 )  =>     aa
 *        
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> Lpad( LONGVARBINARY_GROUP | BINARY_GROUP, INTEGER,
 *                                          LONGVARBINARY_GROUP | BINARY_GROUP )
 * @param[in]  aMaxPrecision     Max Precision
 * @param[out] aResultValue      연산 결과 (결과타입 VARBINARY)
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfBinaryLpad( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         stlInt64         aMaxPrecision,
                         void           * aResultValue,
                         dtlFuncVector  * aVectorFunc,
                         void           * aVectorArg,
                         void           * aEnv )
{
    dtlDataValue    * sValue1;
    dtlDataValue    * sValue2;
    dtlDataValue    * sValue3;
    dtlDataValue    * sResultValue;
    stlErrorStack   * sErrorStack;

    dtlVarbinaryType  sResult;
    dtlVarbinaryType  sHeadStr;
    dtlVarbinaryType  sTailStr;
    stlInt64          sHeadStrLen;
    stlInt64          sTailStrLen;

    stlInt32          sTotalLen;
    stlInt64          sFillLen;
    stlInt64          sTailFillLen;
    stlInt64          sResultLen   = 0;

    stlInt64          sTailChrCount = 0;
    stlInt64          sTailLenCount = 0;

    DTL_PARAM_VALIDATE( ( aInputArgumentCnt == 2 ) || ( aInputArgumentCnt == 3 ), (stlErrorStack *)aEnv );

    sValue1      = aInputArgument[0].mValue.mDataValue;
    sValue2      = aInputArgument[1].mValue.mDataValue;
    sValue3      = NULL;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack  = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_GROUP2( sValue1,
                          DTL_GROUP_BINARY_STRING,
                          DTL_GROUP_LONGVARBINARY_STRING,
                          sErrorStack );

    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_NATIVE_INTEGER, sErrorStack );
    
    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT2( sResultValue,
                           DTL_TYPE_VARBINARY,
                           DTL_TYPE_LONGVARBINARY,
                           sErrorStack );
    
    if( aInputArgumentCnt == 3 )
    {
        sValue3 = aInputArgument[2].mValue.mDataValue;
        DTF_CHECK_DTL_GROUP2( sValue3,
                              DTL_GROUP_BINARY_STRING,
                              DTL_GROUP_LONGVARBINARY_STRING,
                              sErrorStack );
    }
    else
    {
        /* Do Nothing */
    }

    sHeadStr    = DTF_VARBINARY( sValue1 );
    sResult     = DTF_VARBINARY( sResultValue );
    sTotalLen   = DTF_INTEGER( sValue2 );
    
    /* binary type overflow check */
    STL_TRY_THROW( sTotalLen <= aMaxPrecision, ERROR_OUT_OF_RANGE );
    
    /* sTotalLen 의 음수처리 */
    if( sTotalLen < 0 )
    {
        sResultLen = 0;
    }
    else
    {
        sHeadStrLen = sValue1->mLength;

        if( sTotalLen <= sHeadStrLen )
        {
            /**
             * sTotalLen을 sHeadStr로 전부 표현할 수 있는 경우.
             * ex ) SELECT LPAD( X'aa', 1, X'bb' ) FROM DUAL;
             *
             * result
             * -------
             *   AA
             **/            
            stlMemcpy( sResult,
                       sHeadStr,
                       sTotalLen );

            sResultLen = sTotalLen;
        }
        else
        {
            sFillLen = sTotalLen - sHeadStrLen;
            
            /**
             * sTailStr가 생략된 경우,
             * SELECT LPAD( X'aa', 3 ) FROM DUAL;  => 0000AA
             * sTailStr대신 FillLen만큼 0x00으로 채워준다.
             **/
            if( aInputArgumentCnt == 2 )
            {
                stlMemset( sResult,
                           0,
                           sFillLen );

                sResult    += sFillLen;
                sResultLen  = sFillLen;
            }
            else
            {
                /**
                 * sFillLen만큼 sTailStr를 sResult에 복사한다.
                 * 그리고 나머지 sHeadStr를 sResult에 복사한다.
                 **/
                sTailStr = DTF_VARBINARY( sValue3 );
                sTailStrLen = sValue3->mLength;

                /* sFillLen / sStrLen로 TailStr이 반복하며 채워야 할 공간 확인*/
                sTailFillLen = sFillLen / sTailStrLen;

                if( sTailFillLen > 0 )
                {
                    sResultLen = ( sTailStrLen * sTailFillLen );

                    stlMemcpy( sResult,
                               sTailStr,
                               (stlSize)sTailStrLen );

                    sTailLenCount += sTailStrLen;
                    sTailFillLen --;
                    sTailChrCount ++;

                    while( sTailFillLen > 0 )
                    {
                        if( sTailChrCount <= sTailFillLen )
                        {
                            /*
                             * sTailStr을 sResultVal에 반복하여 넣는데,
                             * 만약 이전에 넣은 값의 두배를 추가로 넣어도 sTailFillLen을 넘지 않으면
                             * 2의 배수로 계속 넣는다. 그렇지 않으면 남은 값은 반복하여 넣는다
                             *
                             * => a + a + aa + aaaa + aaaaaaaa ...
                             *  
                             */

                            stlMemcpy( ( sResult + sTailLenCount ),
                                       sResult,
                                       (stlSize)sTailLenCount );
                            sTailFillLen -= sTailChrCount;
                            sTailLenCount *= 2;
                            sTailChrCount *= 2;
                        }
                        else
                        {
                            /*
                             * 남은 부분은 횟수만큼 넣는다 
                             * 
                             * => a + a + aa + aaaa + aaaaaaaa + [ aaaa ]
                             *    ^........... 16 .............^ [ ^ 4 ^ ]
                             */

                            stlMemcpy( ( sResult + sTailLenCount ),
                                       sResult,
                                       (stlSize)( sTailStrLen * sTailFillLen ));

                            sTailFillLen = 0;
                        }
                    }
                    sResult += sResultLen;
                }
                else
                {
                    /* Do Nothing */
                }
                
                /* sFillLen % sStrLen로 TailStr의 일부분으로만 채워야 할 공간 확인*/
                sTailFillLen = sFillLen % sTailStrLen;

                if( sTailFillLen > 0 )
                {
                     /**
                     * 만약 TailStr에 일부분만 넣어야한다면 나머지만큼 잘라 넣는다.
                     * ex ) SELECT LPAD( X'aa', 4, X'bcde' ) FROM DUAL;
                     *
                     *  result
                     * ---------
                     *  BCDEBCAA    (BCDE반복시, 나머지 BC입력과정)
                     *
                     *  ex ) SELECT LPAD( X'aa', 3, X'ABCDEF' ) FROM DUAL;
                     *
                     *  result
                     * ---------
                     *  ABCDAA     (ABCDEF중 TailFillLen만큼의 길이로 잘라 넣는다.) 
                     **/
                    stlMemcpy( sResult,
                               sTailStr,
                               sTailFillLen );

                    sResult    += sTailFillLen;
                    sResultLen += sTailFillLen;
                }
                else
                {
                    /* Do Nothing */
                } 
            }
            /**
             * ex1 ) SELECT LPAD( X'AA', 3, X'BB' ) FROM DUAL;
             *
             *  result
             * ---------
             *  BBBB(AA)
             *
             **/
            stlMemcpy( sResult,
                       sHeadStr,
                       sHeadStrLen );

            sResultLen += sHeadStrLen;
        }
    }

    sResultValue->mLength = sResultLen;
    
    return STL_SUCCESS;
    
    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_STRING_CHARACTER_LENGTH_GREATER_THAN_COLUMN_LENGTH,
                      NULL,
                      sErrorStack );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/** @} */
