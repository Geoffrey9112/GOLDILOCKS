/*******************************************************************************
 * dtfTranslate.c
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
 * @file dtfTranslate.c
 * @brief String Function DataType Library Layer
 */

#include <dtl.h>
#include <dtDef.h>
#include <dtsCharacterSet.h>

#define DTF_TRANSLATE_INFO_ARR_MAX ( 500 )

typedef struct dtfTranslateStr
{

    stlInt8    mFromByte;
    stlInt8    mToByte;
    
    stlChar    mAlign1[2];   /**< 8byte align을 위한 padding */
#if (STL_SIZEOF_VOIDP == 8)
    stlUInt32  mAlign2;      /**< 8byte align을 위한 추가 padding */
#endif 

    stlChar  * mFromOffsetStr;
    stlChar  * mToOffsetStr;
    
#if (STL_SIZEOF_VOIDP == 4)
    stlUInt32  mAlign3;      /**< 8byte align을 위한 추가 padding */
#endif 

} dtfTranslateStr;
    

/*******************************************************************************
 *   Translate
 *   Translate ==> [ P, O ]
 * ex) Translate( 'ABCDE', 'AE', '15' ) => 1BCD5
 *******************************************************************************/

/**
 * @brief from에 지정된 문자와 일치하는 모든 문자를 to에 지정된 문자로 치환
 *        <BR> Translate(str, from, to)
 *        <BR>   Translate => [ P, O ]
 *        <BR> ex) Translate( '12345', '14', 'ax' )  =>  a23x5
 *        <BR> ex) Translate( '12345', '12345', 'ax' )  =>  ax
 *        <BR> ex) Translate( null, 'n', 'f' )  =>  null
 *        <BR> ex) Translate( 'hi gliese', null, 'hi' )  =>  null
 *        <BR> ex) Translate( 'hi gliese', 'hi', null )  =>  null
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> Translate(CHAR_STRING | LONGVARCHAR_STRING,
 *                               <BR>            CHAR_STRING | LONGVARCHAR_STRING,
 *                               <BR>            CHAR_STRING | LONGVARCHAR_STRING)
 * @param[out] aResultValue      연산 결과 (결과타입 VARCHAR | LONGVARCHAR)
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfTranslate( stlUInt16        aInputArgumentCnt,
                        dtlValueEntry  * aInputArgument,
                        void           * aResultValue,
                        dtlFuncVector  * aVectorFunc,
                        void           * aVectorArg,
                        void           * aEnv )
{
    /**
     * 논의대상
     *
     * 현 Gliese는 오라클과 같이 정의하고 있음
     *
     * 1. Null 또는 EMPTY STRING에 대한 처리 
     *  Oracle     : NULL 또는 ''(Empty string ) 모두 NULL 처리
     *             :  translate('abcde',NULL,'abc' ) => null ( NULL )
     *             :  translate('abcde','','abc')    => null ( EMPTY STRING )
     *             :  translate('abcde','abc','')    => null ( EMPTY STRING )
     *
     *  PostgreSQL : sFromStr이 ''(Empty string)에 대한 결과처리
     *             :  translate('abcde',NULL,'abc') => null  ( NULL )
     *             :  translate('abcde','','abc')   => abcde ( EMPTY STRING )
     *             :  translate('abcde','abc','')   => null  ( EMPTY STRING )
     **/
    
    dtlDataValue    * sValue1;
    dtlDataValue    * sValue2;
    dtlDataValue    * sValue3;
    dtlDataValue    * sResultValue;
    dtlCharacterSet   sCharSetID;
    stlErrorStack   * sErrorStack;
    
    dtlVarcharType    sString;
    dtlVarcharType    sFromStr;
    dtlVarcharType    sFromStrEnd;
    dtlVarcharType    sTempFromStr;    
    dtlVarcharType    sToStr;
    dtlVarcharType    sToStrEnd;
    dtlVarcharType    sTempToStr;    
    dtlVarcharType    sResultStr;
    
    stlInt64          sResultLen = 0;
    stlInt64          sStringLen;
    stlInt8           sStrMbLen;
    stlInt8           sFromMbLen = 0;
    stlInt8           sToMbLen = 0;
    
    dtfTranslateStr   sFromToMatch[ DTF_TRANSLATE_INFO_ARR_MAX ];
    stlInt64          sFromArrayNum = 0;
    stlInt64          sStrArrayNum = 0;
    stlBool           sIsTranslate = STL_FALSE;
    
    stlBool           sIsRemainFromStr = STL_FALSE;
    stlInt64          sBufferSize = 0;

    stlBool           sIsEqual = STL_FALSE;
    
    
    DTL_PARAM_VALIDATE( aInputArgumentCnt == 3 , (stlErrorStack *)aEnv );

    sValue1         = aInputArgument[0].mValue.mDataValue;
    sValue2         = aInputArgument[1].mValue.mDataValue;
    sValue3         = aInputArgument[2].mValue.mDataValue;
    sResultValue    = (dtlDataValue *)aResultValue;
    sErrorStack     = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_GROUP2( sValue1,
                          DTL_GROUP_CHAR_STRING,
                          DTL_GROUP_LONGVARCHAR_STRING,
                          sErrorStack );
    DTF_CHECK_DTL_GROUP2( sValue2,
                          DTL_GROUP_CHAR_STRING,
                          DTL_GROUP_LONGVARCHAR_STRING,
                          sErrorStack );
    DTF_CHECK_DTL_GROUP2( sValue3,
                          DTL_GROUP_CHAR_STRING,
                          DTL_GROUP_LONGVARCHAR_STRING,
                          sErrorStack );
    
    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT2( sResultValue,
                           DTL_TYPE_VARCHAR,
                           DTL_TYPE_LONGVARCHAR,
                           sErrorStack );
    
    sCharSetID  = aVectorFunc->mGetCharSetIDFunc( aVectorArg );
    sString     = DTF_VARCHAR( sValue1 );
    sFromStr    = DTF_VARCHAR( sValue2 );
    sFromStrEnd = sFromStr + sValue2->mLength;
    sToStr      = DTF_VARCHAR( sValue3 );
    sToStrEnd   = sToStr + sValue3->mLength;
    sResultStr  = DTF_VARCHAR( sResultValue );
    sStringLen  = sValue1->mLength;
    sBufferSize = sResultValue->mBufferSize;
    
    /*
     * translate의 from 과 to 에 대한 정보구축
     */

    STL_TRY( dtsGetMbLengthFuncList[sCharSetID]( sFromStr,
                                                 &sFromMbLen,
                                                 sErrorStack )
             == STL_SUCCESS );
    
    sFromToMatch[ sFromArrayNum ].mFromByte = sFromMbLen;
    sFromToMatch[ sFromArrayNum ].mFromOffsetStr = sFromStr;
    sFromStr += sFromToMatch[ sFromArrayNum ].mFromByte;

    STL_TRY( dtsGetMbLengthFuncList[sCharSetID]( sToStr,
                                                 &sToMbLen,
                                                 sErrorStack )
             == STL_SUCCESS );
    
    sFromToMatch[ sFromArrayNum ].mToByte = sToMbLen;
    sFromToMatch[ sFromArrayNum ].mToOffsetStr = sToStr;
    sToStr += sFromToMatch[ sFromArrayNum ].mToByte;
    
    sFromArrayNum++;
    
    while( ( sFromStr < sFromStrEnd ) && ( sFromArrayNum < DTF_TRANSLATE_INFO_ARR_MAX )  )
    {
        /*
         * sFromStr과 sToStr의 하나의 Character에 Byte length를 구해
         * dtfTranslateStr에 각 저장한다. 그리고 각 Character의 offset을 구해
         * 그 offset에 해당하는 point를 저장한다.  sString 이 sFromStr과 동일한 문자를 찾았을 때
         * 바로 sToStr을 불러오기 위함이다.
         *
         * TRANSLATE('abcde', 'abcde', '12345' );
         *                      ^        ^
         * dtfTranslate
         * [ mFromByte ][ mFromOffsetStr ][ mToByte ][ mToOffsetStr ]
         *       1              b             1            2
         */
        
        STL_TRY( dtsGetMbLengthFuncList[sCharSetID]( sFromStr,
                                                     &sFromMbLen,
                                                     sErrorStack )
                 == STL_SUCCESS );
        
        sFromToMatch[ sFromArrayNum ].mFromByte = sFromMbLen;
        sFromToMatch[ sFromArrayNum ].mFromOffsetStr = sFromStr;
        sFromStr += sFromMbLen;
        
        if( sToStr < sToStrEnd )
        {
            STL_DASSERT( sFromArrayNum < DTF_TRANSLATE_INFO_ARR_MAX );
            
            STL_TRY( dtsGetMbLengthFuncList[sCharSetID]( sToStr,
                                                         &sToMbLen,
                                                         sErrorStack )
                     == STL_SUCCESS );
            
            sFromToMatch[ sFromArrayNum ].mToByte = sToMbLen;
            sFromToMatch[ sFromArrayNum ].mToOffsetStr = sToStr;
            sToStr += sToMbLen;
        }
        else
        {
            /*
             * 해당 구간은 sFromStr과 매치되는 sToStr이 없는 경우이다.
             * 이때는 sToStr의 mToByte와 mToOffset을 0으로 한다.
             */
            
            sFromToMatch[ sFromArrayNum ].mToByte      = 0;
            sFromToMatch[ sFromArrayNum ].mToOffsetStr = NULL;
        }
        
        sFromArrayNum ++;
    }

    if( sFromStr < sFromStrEnd )
    {
        sIsRemainFromStr = STL_TRUE;
    }
    else
    {
        STL_DASSERT( sFromStr == sFromStrEnd );

        sIsRemainFromStr = STL_FALSE;
    }
    
    /*
     * translate
     */
    
    if( sIsRemainFromStr == STL_FALSE )
    {
        /*
         * ( sFromStr, sToStr에 대한 character <= DTF_TRANSLATE_INFO_ARR_MAX(500) ) 경우로,
         * dtfTranslateStr의 배열 DTF_TRANSLATE_INFO_ARR_MAX 안에 다 구축되는 경우
         */
        
        while( sStringLen > 0 )
        {
            /*
             * sString과 sFromStr을 1 Character 씩 비교한다.
             */
            
            STL_TRY( dtsGetMbLengthFuncList[sCharSetID]( sString,
                                                         &sStrMbLen,
                                                         sErrorStack )
                     == STL_SUCCESS );

            sIsTranslate = STL_FALSE;
            sStrArrayNum = 0;
            
            while( sStrArrayNum < sFromArrayNum )
            {
                /*
                 * sString의 1 Character를 sFromStr로 순차적으로 비교한다.
                 */ 
                if( sStrMbLen == ( sFromToMatch[ sStrArrayNum ].mFromByte ) )
                {
                    /*
                     * sString과 sFromStr의 1 Character의 Byte Length가 같은 경우,
                     *
                     *  TRANSLATE('ABCDE', 'ABCDE', '12345' )
                     *
                     *  sString : ABCDE         sFromStr : ABCDE
                     *             ^                       ^
                     */
                    DTS_IS_EQUAL_SAME_BYTELEN_ONE_CHAR( sString,
                                                        sFromToMatch[ sStrArrayNum ].mFromOffsetStr,
                                                        sStrMbLen,
                                                        & sIsEqual );
                    if( sIsEqual == STL_TRUE )
                    {
                        /*
                         * sString과 sFromStr의 1 Character의 동일한 Character일 경우,
                         *
                         *  TRANSLATE('ABCDE', 'ABCDE', '12345' )
                         *
                         *  sString : ABCDE         sFromStr : ABCDE
                         *             ^                        ^
                         */
                    
                        if( ( sFromToMatch[ sStrArrayNum ].mToByte ) > 0 )
                        {
                            /*
                             * sString과 sFromStr이 동일한 문자를 찾았다.
                             * sFromStr에 해당하는 sToStr이 있는 경우 ( mToByte > 0 경우 )
                             * ResultStr에 sToStr의 Character를 복사하여 Translate 시켜준다.
                             *
                             * TRANSLATE( 'ABCDE', 'ABCDE', '12345' );
                             *
                             * sString: ABCDE       sFromStr : ABCDE      sToStr : 12345
                             *           ^                      ^                   ^
                             *
                             * Result : 12345
                             *           ^
                             */           

                            sResultLen += sFromToMatch[ sStrArrayNum ].mToByte;
                            STL_TRY_THROW( sResultLen <= sBufferSize, ERROR_OUT_OF_RANGE );
                            
                            DTS_COPY_ONE_CHAR( sResultStr,
                                               sFromToMatch[ sStrArrayNum ].mToOffsetStr,
                                               sFromToMatch[ sStrArrayNum ].mToByte );
                            
                            sResultStr += sFromToMatch[ sStrArrayNum ].mToByte;
                        }
                        else
                        {
                            /*
                             * sString과 sFromStr이 동일한 Character를 찾았으나
                             * sFromStr에 해당하는 sToStr이 없는 경우로
                             * 매치된 aString의 Character는 sResultStr에 복사하지 않아 제거한다. 
                             *
                             * TRANSLATE( 'ABCDE', 'ABCDE', '123' );
                             *
                             * sString: ABCDE       sFromStr : ABCDE      sToStr : 123
                             *             ^                      ^                   ^
                             *
                             * Result : 123
                             *             ^
                             */           
                        }
                        sIsTranslate = STL_TRUE;
                        break;
                    }
                    else
                    {
                        /* sString과 sFromStr의 1 Character 가 다른 값일 경우 */
                    }
                }
                else
                {
                    /* sString과 sFromStr의 1 Character 의 Byte Length가 다른 경우 */
                }

                sStrArrayNum ++;
            }

            if( sIsTranslate == STL_TRUE )
            {
                /*
                 * sIsTranslate 가 STL_TRUE 일 경우,
                 * sString에 맞는 sFromStr을 찾아 미리 처리해 줬으므로
                 * sString을 Memcpy하지 않고 넘어간다.
                 */
                
                /* Do Nothing */
            }
            else
            {
                /*
                 * sString의 1Character과 sFromStr의 전체 Character를 검사했으나
                 * 동일한 Character가 없었던 경우.
                 * sString 의 1 Character를 복사한다.
                 */
                
                sResultLen += sStrMbLen;
                STL_TRY_THROW( sResultLen <= sBufferSize, ERROR_OUT_OF_RANGE );
                
                DTS_COPY_ONE_CHAR( sResultStr,
                                   sString,
                                   sStrMbLen );
                sResultStr += sStrMbLen;
            }

            sString += sStrMbLen;
            sStringLen -= sStrMbLen;
        }
    }
    else
    {
        /*
         * ( sFromStr, sToStr에 대한 character > DTF_TRANSLATE_INFO_ARR_MAX(500) ) 경우로,
         * dtfTranslateStr의 배열 DTF_TRANSLATE_INFO_ARR_MAX 안에 다 구축되지 못하는 경우
         */
        
        STL_DASSERT( sFromArrayNum == DTF_TRANSLATE_INFO_ARR_MAX );
        
        while( sStringLen > 0 )
        {
            /*
             * sString과 sFromStr을 1 Character 씩 비교한다.
             */
            
            STL_TRY( dtsGetMbLengthFuncList[sCharSetID]( sString,
                                                         &sStrMbLen,
                                                         sErrorStack )
                     == STL_SUCCESS );
            
            sIsTranslate = STL_FALSE;
            sStrArrayNum = 0;
            
            while( sStrArrayNum < sFromArrayNum )
            {
                /*
                 * sString의 1 Character를 sFromStr로 순차적으로 비교한다.
                 */ 
                if( sStrMbLen == ( sFromToMatch[ sStrArrayNum ].mFromByte ) )
                {
                    /*
                     * sString과 sFromStr의 1 Character의 Byte Length가 같은 경우,
                     *
                     *  TRANSLATE('ABCDE', 'ABCDE', '12345' )
                     *
                     *  sString : ABCDE         sFromStr : ABCDE
                     *             ^                       ^
                     */
                    DTS_IS_EQUAL_SAME_BYTELEN_ONE_CHAR( sString,
                                                        sFromToMatch[ sStrArrayNum ].mFromOffsetStr,
                                                        sStrMbLen,
                                                        & sIsEqual );
                    if( sIsEqual == STL_TRUE )
                    {
                        /*
                         * sString과 sFromStr의 1 Character의 동일한 Character일 경우,
                         *
                         *  TRANSLATE('ABCDE', 'ABCDE', '12345' )
                         *
                         *  sString : ABCDE         sFromStr : ABCDE
                         *             ^                        ^
                         */
                    
                        if( ( sFromToMatch[ sStrArrayNum ].mToByte ) > 0 )
                        {
                            /*
                             * sString과 sFromStr이 동일한 문자를 찾았다.
                             * sFromStr에 해당하는 sToStr이 있는 경우 ( mToByte > 0 경우 )
                             * ResultStr에 sToStr의 Character를 복사하여 Translate 시켜준다.
                             *
                             * TRANSLATE( 'ABCDE', 'ABCDE', '12345' );
                             *
                             * sString: ABCDE       sFromStr : ABCDE      sToStr : 12345
                             *           ^                      ^                   ^
                             *
                             * Result : 12345
                             *           ^
                             */           

                            sResultLen += sFromToMatch[ sStrArrayNum ].mToByte;
                            STL_TRY_THROW( sResultLen <= sBufferSize, ERROR_OUT_OF_RANGE );
                            
                            DTS_COPY_ONE_CHAR( sResultStr,
                                               sFromToMatch[ sStrArrayNum ].mToOffsetStr,
                                               sFromToMatch[ sStrArrayNum ].mToByte );
                            
                            sResultStr += sFromToMatch[ sStrArrayNum ].mToByte;
                        }
                        else
                        {
                            /*
                             * sString과 sFromStr이 동일한 Character를 찾았으나
                             * sFromStr에 해당하는 sToStr이 없는 경우로
                             * 매치된 aString의 Character는 sResultStr에 복사하지 않아 제거한다. 
                             *
                             * TRANSLATE( 'ABCDE', 'ABCDE', '123' );
                             *
                             * sString: ABCDE       sFromStr : ABCDE      sToStr : 123
                             *             ^                      ^                   ^
                             *
                             * Result : 123
                             *             ^
                             */           
                        }
                        sIsTranslate = STL_TRUE;
                        break;
                    }
                    else
                    {
                        /* sString과 sFromStr의 1 Character 가 다른 값일 경우 */
                    }
                }
                else
                {
                    /* sString과 sFromStr의 1 Character 의 Byte Length가 다른 경우 */
                }

                sStrArrayNum ++;
            }

            if( sIsTranslate == STL_TRUE )
            {
                /*
                 * sIsTranslate 가 STL_TRUE 일 경우,
                 * sString에 맞는 sFromStr을 찾아 미리 처리해 줬으므로
                 * sString을 Memcpy하지 않고 넘어간다.
                 */
                
                sString += sStrMbLen;
                sStringLen -= sStrMbLen;
                
                continue;
            }
            else
            {
                /*
                 * sString의 1Character과 sFromStr의 전체 Character를 검사했으나
                 * 동일한 Character가 없었던 경우.
                 * 배열에 포함되지 못한 나머지 sFromStr과 sToStr을 분석해서 처리한다.
                 */
                
                /* Do Nothing */
            }
            
            STL_DASSERT( sStrArrayNum == DTF_TRANSLATE_INFO_ARR_MAX );
            
            sTempFromStr =
                ( sFromToMatch[ sStrArrayNum - 1 ].mFromOffsetStr + sFromToMatch[ sStrArrayNum - 1 ].mFromByte );
            sTempToStr =
                ( sFromToMatch[ sStrArrayNum - 1 ].mToOffsetStr + sFromToMatch[ sStrArrayNum - 1 ].mToByte );
            
            while( sTempFromStr < sFromStrEnd )
            {
                /**
                 * 체크할 sFromStr 과 sToStr 의 첫번째 하나의 char의 length를 구한다.
                 */
                
                STL_TRY( dtsGetMbLengthFuncList[sCharSetID]( sTempFromStr,
                                                             &sFromMbLen,
                                                             sErrorStack )
                         == STL_SUCCESS );
                
                if( sToStr < sToStrEnd )
                {
                    STL_TRY( dtsGetMbLengthFuncList[sCharSetID]( sTempToStr,
                                                                 &sToMbLen,
                                                                 sErrorStack )
                             == STL_SUCCESS );
                }
                else
                {
                    sToMbLen = 0;
                }
                
                /*
                 * sString의 1 Character를 sFromStr로 순차적으로 비교한다.
                 */ 
                if( sStrMbLen == sFromMbLen )
                {
                    /*
                     * sString과 sFromStr의 1 Character의 Byte Length가 같은 경우,
                     *
                     *  TRANSLATE('ABCDE', 'ABCDE', '12345' )
                     *
                     *  sString : ABCDE         sFromStr : ABCDE
                     *             ^                       ^
                     */
                    DTS_IS_EQUAL_SAME_BYTELEN_ONE_CHAR( sString,
                                                        sTempFromStr,
                                                        sStrMbLen,
                                                        & sIsEqual );
                    if( sIsEqual == STL_TRUE )
                    {
                        /*
                         * sString과 sFromStr의 1 Character의 동일한 Character일 경우,
                         *
                         *  TRANSLATE('ABCDE', 'ABCDE', '12345' )
                         *
                         *  sString : ABCDE         sFromStr : ABCDE
                         *             ^                        ^
                         */
                        
                        if( sToMbLen > 0 )
                        {
                            /*
                             * sString과 sFromStr이 동일한 문자를 찾았다.
                             * sFromStr에 해당하는 sToStr이 있는 경우 ( mToByte > 0 경우 )
                             * ResultStr에 sToStr의 Character를 복사하여 Translate 시켜준다.
                             *
                             * TRANSLATE( 'ABCDE', 'ABCDE', '12345' );
                             *
                             * sString: ABCDE       sFromStr : ABCDE      sToStr : 12345
                             *           ^                      ^                   ^
                             *
                             * Result : 12345
                             *           ^
                             */           

                            sResultLen += sToMbLen;
                            STL_TRY_THROW( sResultLen <= sBufferSize, ERROR_OUT_OF_RANGE );
                            
                            DTS_COPY_ONE_CHAR( sResultStr,
                                               sTempToStr,
                                               sToMbLen );
                            
                            sResultStr += sToMbLen;
                        }
                        else
                        {
                            /*
                             * sString과 sFromStr이 동일한 Character를 찾았으나
                             * sFromStr에 해당하는 sToStr이 없는 경우로
                             * 매치된 aString의 Character는 sResultStr에 복사하지 않아 제거한다. 
                             *
                             * TRANSLATE( 'ABCDE', 'ABCDE', '123' );
                             *
                             * sString: ABCDE       sFromStr : ABCDE      sToStr : 123
                             *             ^                      ^                   ^
                             *
                             * Result : 123
                             *             ^
                             */           
                        }
                        sIsTranslate = STL_TRUE;
                        break;
                    }
                    else
                    {
                        /* sString과 sFromStr의 1 Character 가 다른 값일 경우 */
                    }
                }
                else
                {
                    /* sString과 sFromStr의 1 Character 의 Byte Length가 다른 경우 */
                }
                
                sTempFromStr += sFromMbLen;
                sTempToStr += sToMbLen;
                
            } // sTempFromStr while
            
            if( sIsTranslate == STL_TRUE )
            {
                /*
                 * sIsTranslate 가 STL_TRUE 일 경우,
                 * sString에 맞는 sFromStr을 찾아 미리 처리해 줬으므로
                 * sString을 Memcpy하지 않고 넘어간다.
                 */
                
                /* Do Nothing */
            }
            else
            {
                /*
                 * sString의 1Character과 sFromStr의 전체 Character를 검사했으나
                 * 동일한 Character가 없었던 경우.
                 * sString 의 1 Character를 복사한다.
                 */

                sResultLen += sStrMbLen;
                STL_TRY_THROW( sResultLen <= sBufferSize, ERROR_OUT_OF_RANGE );
                
                DTS_COPY_ONE_CHAR( sResultStr,
                                   sString,
                                   sStrMbLen );
                sResultStr += sStrMbLen;
            }
            
            sString += sStrMbLen;
            sStringLen -= sStrMbLen;
            
        } // sString while
    }
    
    /* STL_TRY_THROW( sResultLen <= sResultValue->mBufferSize, */
    /*                ERROR_OUT_OF_RANGE );         */
    
    STL_DASSERT( sResultLen <= sResultValue->mBufferSize );
    
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
