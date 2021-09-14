/*******************************************************************************
 * dtfReplace.c
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
 * @file dtfReplace.c
 * @brief String Function DataType Library Layer
 */

#include <dtl.h>
#include <dtDef.h>
#include <dtsCharacterSet.h>

/*******************************************************************************
 *   Replace
 *   Replace ==> [ P, M, O ]
 * ex) Replace( 'HIHI GLIESE', 'HI', 'HELLO!' ) => HELLO!HELLO! GLIESE
 *******************************************************************************/

/**
 * @brief string내의 모든 from string을 to string으로 치환
 *        <BR> Replace(str, from [, to])
 *        <BR>   Replace => [ P, M, O ]
 *        <BR> ex) Replace( 'gliese hi', 'hi' )  =>  gliese 
 *        <BR> ex) Replace( 'abcdefabcdef', 'cd', 'XX' )  =>  abXXefabXXef
 *        <BR> ex) Replace( null, 'n', 'f' )  =>  null
 *        <BR> ex) Replace( 'hi gliese', null, 'hi' )  =>   hi gliese
 *        <BR> ex) Replace( 'hi gliese', 'hi', null )  =>   gliese
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> Replace(VARCHAR, VARCHAR, VARCHAR)
 * @param[out] aResultValue      연산 결과 (결과타입 VARCHAR)
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfReplace( stlUInt16        aInputArgumentCnt,
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
     * 1. argument에서 to string이 제외되는 경우.
     *  Oracle            : replace( 'abab', 'b' ) => aa
     *  PostgreSQL, MySQL : syntax error
     *
     * 2. NULL과 Empty Length 처리의 차이로 인한 구문차이.
     *  Oracle            : replace( 'abab', null, 'b' ) => abab
     *  PostgreSQL, MySQL : replace( 'abab', null, 'b' ) => null  ( NULL )
     *  PostgreSQL, MySQL : replace( 'abab', '', 'b' ) => abab    ( empty string length )
     *
     *  Oracle            : replace( 'a_a_', '_', null ) => aa
     *  PostgreSQL, MySQL : replace( 'a_a_', '_', null ) => null  ( NULL )
     *  PostgreSQL, MySQL : replace( 'a_a_', '_', '' ) => aa      ( empty string length )
     *  
     **/
    
    dtlDataValue    * sValue1;
    dtlDataValue    * sValue2;
    dtlDataValue    * sValue3;
    dtlDataValue    * sResultValue;
    dtlCharacterSet   sCharSetID;
    stlErrorStack   * sErrorStack;
    
    dtlVarcharType    sString;
    dtlVarcharType    sFromStr;
    dtlVarcharType    sToStr = NULL;
    dtlVarcharType    sResultStr;
    stlInt64          sResultLen = 0;
    stlInt64          sStringLen;
    stlInt64          sStrChrLen;
    stlInt64          sFromChrLen;
    stlInt64          sToChrLen = 0;
    stlInt8           sStrMbLen;
    stlInt64          sTotalChrLen = 0;
    stlInt64          sBufferSize = 0;
    stlBool           sIsReplace = STL_FALSE;

    DTL_PARAM_VALIDATE( ( aInputArgumentCnt == 2 ) || ( aInputArgumentCnt == 3 ), (stlErrorStack *)aEnv );

    sValue1         = aInputArgument[0].mValue.mDataValue;
    sValue2         = aInputArgument[1].mValue.mDataValue;
    sValue3         = NULL;
    sResultValue    = (dtlDataValue *)aResultValue;
    sErrorStack     = (stlErrorStack *)aEnv;

    /* str, from, to에 NULL이 들어올 수 있으므로 VALID CHECK하지 않는다 */
    /* DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_VARCHAR, sErrorStack ); */
    
    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_VARCHAR, sErrorStack );

    if( DTL_IS_NULL( sValue1 ) == STL_TRUE )
    {
        DTL_SET_NULL( sResultValue );

        STL_THROW( SUCCESS_END );
    }        
    else 
    {
        if( aInputArgumentCnt == 3 )
        {
            sValue3 = aInputArgument[2].mValue.mDataValue;
        }
        else
        {
            /* Do Nothing */
        }
    }

    sCharSetID  = aVectorFunc->mGetCharSetIDFunc( aVectorArg );
    sString     = DTF_VARCHAR( sValue1 );
    sFromStr    = DTF_VARCHAR( sValue2 );
    sResultStr  = DTF_VARCHAR( sResultValue );
    sStringLen  = sValue1->mLength;
    sBufferSize = sResultValue->mBufferSize;
    
    if( ( DTL_IS_NULL( sValue2 ) == STL_TRUE ) || ( sStringLen < ( sValue2->mLength ) ) )
    {
        /*
         * sFromStr이 NULL일 때, sFromStr을 찾을 수 없으므로 비교하지 않음.
         *
         *  REPLACE( 'ABAB', NULL, 'C' );
         *
         *             결과 : ABAB
         *             
         * sString보다 sFromStr이 클경우,
         * sString에는 sFromStr과 같은 str이 없으므로 비교하지 않음.
         *
         *  REPLACE( 'AB', 'ABC', 'C');
         *
         *             결과 : AB 
         */

        sResultLen = sStringLen;
        STL_TRY_THROW( sResultLen <= sBufferSize, ERROR_OUT_OF_RANGE );
        
        stlMemcpy( sResultStr,
                   sString,
                   sStringLen );

        STL_THROW( SUCCESS_END );
    }
    else
    {
        /* Do Nothing */
    }

    if( aInputArgumentCnt == 3 )
    {
        sToStr = DTF_VARCHAR( sValue3 );
        
        /* VARCHAR MAX PRECISION검사를 위해 sToStr의 Character Length를 구함  */
        STL_TRY( dtsGetMbstrlenWithLenFuncList[ sCharSetID ]( sToStr,
                                                              sValue3->mLength,
                                                              &sToChrLen,
                                                              aVectorFunc,
                                                              aVectorArg,
                                                              sErrorStack )
                 == STL_SUCCESS );
    }
    else
    {
        /* Do Nothing */
    }

    STL_TRY( dtsGetMbstrlenWithLenFuncList[ sCharSetID ]( sFromStr,
                                                          sValue2->mLength,
                                                          &sFromChrLen,
                                                          aVectorFunc,
                                                          aVectorArg,
                                                          sErrorStack )
             == STL_SUCCESS );
    
    while( sStringLen >= sValue2->mLength )
    {
        if( stlMemcmp( sString, sFromStr, sValue2->mLength ) == 0 )
        {
            /*
             *  sFromStr의 byte Length 만큼 sString을 stlMemcmp를 통해 동일한 값인지 검사    
             *  sString과 sFromStr의  byte  Length만큼의 단어가 동일함.
             *   
             *   REPLACE( 'ABCDE', 'ABC', 'F' ) ;
             *
             *   str : [ABC]DE
             *            ^   
             *   from: [ABC]
             */
            
            STL_TRY( dtsGetMbstrlenWithLenFuncList[ sCharSetID ]( sString,
                                                                  sValue2->mLength,
                                                                  &sStrChrLen,
                                                                  aVectorFunc,
                                                                  aVectorArg,
                                                                  sErrorStack )
                     == STL_SUCCESS );
            
            /* sString과 sFromStr의 Character Length도 동일한지 검사 */
            if( sStrChrLen == sFromChrLen )
            {                 
                sString += sValue2->mLength;
                sStringLen -= sValue2->mLength;

                if( aInputArgumentCnt == 3 )
                {
                    /*
                     * aInputArgumentCnt == 3일 경우,
                     * sString에서 sFromStr이 동일한 단어 범위 대신 sToStr로 대체함
                     * 
                     * REPLACE( 'ABCDE', 'ABC', 'F' ) ;
                     *
                     *   str : [ABC]DE
                     *            ^   
                     *   from: [ABC]      RESULT : FDE
                     *   
                     */
                    
                    sTotalChrLen += sToChrLen;
                    sResultLen += sValue3->mLength;
                    STL_TRY_THROW( ( sTotalChrLen <= DTL_VARCHAR_MAX_PRECISION ) &&
                                   ( sResultLen <= sBufferSize ),
                                   ERROR_OUT_OF_RANGE );
                    
                    stlMemcpy( sResultStr,
                               sToStr,
                               sValue3->mLength );
                                
                    sResultStr += sValue3->mLength;
                }
                else
                {
                    /*
                     *  aInputArgumentCnt == 2일 경우,  
                     *  sString에서 sFromStr이 동일한 단어 범위 만큼 제거함
                     *   
                     *   REPLACE( 'ABCDE', 'ABC' ) ;
                     *
                     *   str : [ABC]DE
                     *            ^   
                     *   from: [ABC]           RESULT : []DE
                     *   
                     */
                }
                sIsReplace = STL_TRUE;
                
            }
            else
            {
                /* sString과 sFromStr이 stlMemcmp는 같지만 두 Character length가 다른경우 */
            }
        }
        else
        {
            /*
             * sString과 sFromStr을 byte Length범위로 잘라 비교했으나 다른 값인 경우,
             * 
             *  REPLACE( 'CDEDE', 'ABC', 'F') ;
             *
             *    str : [CDE]DE    
             *             ^               
             *    from: [ABC]           
             *    
             */
        }
        
        if( sIsReplace == STL_FALSE )
        {
            /*
             * sIsReplace 작업을 하지 않았을 경우, 
             * sString과 sFromStr을 byte Length범위로 잘라 비교했으나 다르므로 
             * sString의 첫 1 Character의 Byte Length를 계산한 후 복사하고 다음 Character를 검사함
             *
             * 
             *  REPLACE( 'CDEDE', 'ABC' ) ;
             *
             *    str : [CDE]DE            str : C[DED]E 
             *             ^        =>           ^        
             *    from: [ABC]               from: [ABC]    
             *   
             */
            
            STL_TRY( dtsGetMbLengthFuncList[sCharSetID]( sString,
                                                         &sStrMbLen,
                                                         sErrorStack )
                     == STL_SUCCESS );

            sTotalChrLen ++;
            sResultLen += sStrMbLen;            
            STL_TRY_THROW( ( sTotalChrLen <= DTL_VARCHAR_MAX_PRECISION ) &&
                           ( sResultLen <= sBufferSize ),
                           ERROR_OUT_OF_RANGE );

            stlMemcpy( sResultStr,
                       sString,
                       sStrMbLen );
                
            sResultStr += sStrMbLen;
                
            sString += sStrMbLen;
            sStringLen -= sStrMbLen;
        }
        else
        {
            sIsReplace = STL_FALSE;
        }
    }

    if( sStringLen > 0 )
    {
        /*
         * sFromStr범위로 sString을 검사하기 때문에 마지막 sString이 남을 경우가 있다.
         *
         * sStringLen < sValue2->mLength 경우,
         * sFromStr의 범위보다 sString의 byte Length가  적은 경우이므로 비교할 필요가 없다. 
         *
         *     REPLACE( 'AAA', 'AA', 'F' );
         *
         *      str : AA[A]       sFrom : AA
         *               ^
         * 이때, VARCHAR_MAX_PRECISION을 체크해주기 위해 나머지 sString의 Character Length를 구한다.
         * 
         */
            
        STL_TRY( dtsGetMbstrlenWithLenFuncList[ sCharSetID ]( sString,
                                                              sStringLen,
                                                              &sStrChrLen,
                                                              aVectorFunc,
                                                              aVectorArg,
                                                              sErrorStack )
                 == STL_SUCCESS );
        
        sResultLen += sStringLen;
        
        STL_TRY_THROW( ( ( sTotalChrLen + sStrChrLen )  <= DTL_VARCHAR_MAX_PRECISION ) &&
                       ( sResultLen <= sBufferSize ),
                       ERROR_OUT_OF_RANGE );
             
        stlMemcpy( sResultStr,
                   sString,
                   sStringLen );

        sResultStr += sStringLen;
    }
    else
    {
        /* DO NOTHING */
    }
    
    STL_RAMP( SUCCESS_END );
    
    sResultValue->mLength = sResultLen;

    STL_DASSERT( sResultValue->mLength <= sResultValue->mBufferSize );
    
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
 * @brief long varchar string내의 모든 from string을 to string으로 치환
 *        <BR> Replace(str, from [, to])
 *        <BR>   Replace => [ P, M, O ]
 *        <BR> ex) Replace( 'gliese hi', 'hi' )  =>  gliese 
 *        <BR> ex) Replace( 'abcdefabcdef', 'cd', 'XX' )  =>  abXXefabXXef
 *        <BR> ex) Replace( null, 'n', 'f' )  =>  null
 *        <BR> ex) Replace( 'hi gliese', null, 'hi' )  =>   hi gliese
 *        <BR> ex) Replace( 'hi gliese', 'hi', null )  =>   gliese
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> Replace(LONGVARCHAR, LONGVARCHAR, LONGVARCHAR)
 * @param[out] aResultValue      연산 결과 (결과타입 LONGVARCHAR)
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfLongvarcharReplace( stlUInt16        aInputArgumentCnt,
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
     * 1. argument에서 to string이 제외되는 경우.
     *  Oracle            : replace( 'abab', 'b' ) => aa
     *  PostgreSQL, MySQL : syntax error
     *
     * 2. NULL과 Empty Length 처리의 차이로 인한 구문차이.
     *  Oracle            : replace( 'abab', null, 'b' ) => abab
     *  PostgreSQL, MySQL : replace( 'abab', null, 'b' ) => null  ( NULL )
     *  PostgreSQL, MySQL : replace( 'abab', '', 'b' ) => abab    ( empty string length )
     *
     *  Oracle            : replace( 'a_a_', '_', null ) => aa
     *  PostgreSQL, MySQL : replace( 'a_a_', '_', null ) => null  ( NULL )
     *  PostgreSQL, MySQL : replace( 'a_a_', '_', '' ) => aa      ( empty string length )
     *  
     **/
    
    dtlDataValue    * sValue1;
    dtlDataValue    * sValue2;
    dtlDataValue    * sValue3;
    dtlDataValue    * sResultValue;
    dtlCharacterSet   sCharSetID;
    stlErrorStack   * sErrorStack;
    
    dtlVarcharType    sString;
    dtlVarcharType    sFromStr;
    dtlVarcharType    sToStr = NULL;
    dtlVarcharType    sResultStr;
    stlInt64          sResultLen = 0;
    stlInt64          sStringLen;
    stlInt64          sStrChrLen;        
    stlInt64          sToStrLen = 0;
    stlInt64          sFromChrLen;
    stlInt64          sBufferSize;
    stlInt8           sStrMbLen;
    stlBool           sIsReplace = STL_FALSE;

    DTL_PARAM_VALIDATE( ( aInputArgumentCnt == 2 ) || ( aInputArgumentCnt == 3 ), (stlErrorStack *)aEnv );

    sValue1         = aInputArgument[0].mValue.mDataValue;
    sValue2         = aInputArgument[1].mValue.mDataValue;
    sValue3         = NULL;
    sResultValue    = (dtlDataValue *)aResultValue;
    sErrorStack     = (stlErrorStack *)aEnv;

    /* str, from, to에 NULL이 들어올 수 있으므로 VALID CHECK하지 않는다 */
    /* DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_VARCHAR, sErrorStack ); */
    
    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_LONGVARCHAR, sErrorStack );

    if( DTL_IS_NULL( sValue1 ) == STL_TRUE )
    {
        DTL_SET_NULL( sResultValue );

        STL_THROW( SUCCESS_END );
    }        
    else 
    {
        if( aInputArgumentCnt == 3 )
        {
            sValue3 = aInputArgument[2].mValue.mDataValue;
        }
        else
        {
            /* Do Nothing */
        }
    }

    sCharSetID  = aVectorFunc->mGetCharSetIDFunc( aVectorArg );
    sString     = DTF_VARCHAR( sValue1 );
    sFromStr    = DTF_VARCHAR( sValue2 );
    sResultStr  = DTF_VARCHAR( sResultValue );
    sStringLen  = sValue1->mLength;
    
    if( ( DTL_IS_NULL( sValue2 ) == STL_TRUE ) || ( sStringLen < ( sValue2->mLength ) ) )
    {
        /*
         * sFromStr이 NULL일 때, sFromStr을 찾을 수 없으므로 비교하지 않음.
         *
         *  REPLACE( 'ABAB', NULL, 'C' );
         *
         *             결과 : ABAB
         *             
         * sString보다 sFromStr이 클경우,
         * sString에는 sFromStr과 같은 str이 없으므로 비교하지 않음.
         *
         *  REPLACE( 'AB', 'ABC', 'C');
         *
         *             결과 : AB 
         */

        if( sStringLen > sResultValue->mBufferSize )
        {
            STL_TRY( aVectorFunc->mReallocLongVaryingMemFunc( aVectorArg,
                                                              sStringLen,
                                                              &sResultValue->mValue,
                                                              (stlErrorStack *)aEnv )
                     == STL_SUCCESS );
        
            sResultValue->mBufferSize = sStringLen;

            sResultStr = DTF_VARCHAR( sResultValue );
        }
        
        stlMemcpy( sResultStr,
                   sString,
                   sStringLen );

        sResultLen = sStringLen;

        STL_THROW( SUCCESS_END );
    }
    else
    {
        /* Do Nothing */
    }

    if( aInputArgumentCnt == 3 )
    {
        sToStr = DTF_VARCHAR( sValue3 );
        sToStrLen = sValue3->mLength;
    }
    else
    {
        /* Do Nothing */
    }

    STL_TRY( dtsGetMbstrlenWithLenFuncList[ sCharSetID ]( sFromStr,
                                                          sValue2->mLength,
                                                          &sFromChrLen,
                                                          aVectorFunc,
                                                          aVectorArg,
                                                          sErrorStack )
             == STL_SUCCESS );
    
    while( sStringLen >= sValue2->mLength )
    {
        if( stlMemcmp( sString, sFromStr, sValue2->mLength ) == 0 )
        {
            /*
             *  sFromStr의 byte Length 만큼 sString을 stlMemcmp를 통해 동일한 값인지 검사    
             *  sString과 sFromStr의  byte  Length만큼의 단어가 동일함.
             *   
             *   REPLACE( 'ABCDE', 'ABC', 'F' ) ;
             *
             *   str : [ABC]DE
             *            ^   
             *   from: [ABC]
             */
            
            STL_TRY( dtsGetMbstrlenWithLenFuncList[ sCharSetID ]( sString,
                                                                  sValue2->mLength,
                                                                  &sStrChrLen,
                                                                  aVectorFunc,
                                                                  aVectorArg,
                                                                  sErrorStack )
                     == STL_SUCCESS );
            
            /* sString과 sFromStr의 Character Length도 동일한지 검사 */
            if( sStrChrLen == sFromChrLen )
            {                 
                sString += sValue2->mLength;
                sStringLen -= sValue2->mLength;

                if( aInputArgumentCnt == 3 )
                {
                    /*
                     * aInputArgumentCnt == 3일 경우,
                     * sString에서 sFromStr이 동일한 단어 범위 대신 sToStr로 대체함
                     * 
                     * REPLACE( 'ABCDE', 'ABC', 'F' ) ;
                     *
                     *   str : [ABC]DE
                     *            ^   
                     *   from: [ABC]      RESULT : FDE
                     *   
                     */

                    STL_TRY_THROW( (sResultLen + sToStrLen) <= DTL_LONGVARCHAR_MAX_PRECISION,
                                   ERROR_OUT_OF_RANGE );
                            
                    if( (sResultLen + sToStrLen) > sResultValue->mBufferSize )
                    {
                        sBufferSize = STL_MIN( (sResultLen + sToStrLen) * 2, DTL_LONGVARCHAR_MAX_PRECISION );

                        sResultValue->mLength = sResultLen;
                        
                        STL_TRY( aVectorFunc->mReallocAndMoveLongVaryingMemFunc( aVectorArg,
                                                                                 sResultValue,
                                                                                 sBufferSize,
                                                                                 (stlErrorStack *)aEnv )
                                 == STL_SUCCESS );

                        sResultValue->mBufferSize = sBufferSize;

                        sResultStr = DTF_VARCHAR( sResultValue );
                        sResultStr += sResultLen;
                    }
        
                    stlMemcpy( sResultStr, sToStr, sToStrLen );
                                
                    sResultStr += sToStrLen;
                    sResultLen += sToStrLen;
                }
                else
                {
                    /*
                     *  aInputArgumentCnt == 2일 경우,  
                     *  sString에서 sFromStr이 동일한 단어 범위 만큼 제거함
                     *   
                     *   REPLACE( 'ABCDE', 'ABC' ) ;
                     *
                     *   str : [ABC]DE
                     *            ^   
                     *   from: [ABC]           RESULT : []DE
                     *   
                     */
                }
                sIsReplace = STL_TRUE;
                
            }
            else
            {
                /* sString과 sFromStr이 stlMemcmp는 같지만 두 Character length가 다른경우 */
            }
        }
        else
        {
            /*
             * sString과 sFromStr을 byte Length범위로 잘라 비교했으나 다른 값인 경우,
             * 
             *  REPLACE( 'CDEDE', 'ABC', 'F') ;
             *
             *    str : [CDE]DE    
             *             ^               
             *    from: [ABC]           
             *    
             */
        }
        
        if( sIsReplace == STL_FALSE )
        {
            /*
             * sIsReplace 작업을 하지 않았을 경우, 
             * sString과 sFromStr을 byte Length범위로 잘라 비교했으나 다르므로 
             * sString의 첫 1 Character의 Byte Length를 계산한 후 복사하고 다음 Character를 검사함
             *
             * 
             *  REPLACE( 'CDEDE', 'ABC' ) ;
             *
             *    str : [CDE]DE            str : C[DED]E 
             *             ^        =>           ^        
             *    from: [ABC]               from: [ABC]    
             *   
             */
            
            STL_TRY( dtsGetMbLengthFuncList[sCharSetID]( sString,
                                                         &sStrMbLen,
                                                         sErrorStack )
                     == STL_SUCCESS );

            STL_TRY_THROW( sResultLen + sStrMbLen <= DTL_LONGVARCHAR_MAX_PRECISION,
                           ERROR_OUT_OF_RANGE );

            if( (sResultLen + sStrMbLen) > sResultValue->mBufferSize )
            {
                sBufferSize = STL_MIN( (sResultLen + sStrMbLen) * 2, DTL_LONGVARCHAR_MAX_PRECISION );

                sResultValue->mLength = sResultLen;
                        
                STL_TRY( aVectorFunc->mReallocAndMoveLongVaryingMemFunc( aVectorArg,
                                                                         sResultValue,
                                                                         sBufferSize,
                                                                         (stlErrorStack *)aEnv )
                         == STL_SUCCESS );

                sResultValue->mBufferSize = sBufferSize;
                                        
                sResultStr = DTF_VARCHAR( sResultValue );
                sResultStr += sResultLen;
            }
        
            stlMemcpy( sResultStr,
                       sString,
                       sStrMbLen );
                
            sResultStr += sStrMbLen;
            sResultLen += sStrMbLen;
                
            sString += sStrMbLen;
            sStringLen -= sStrMbLen;
        }
        else
        {
            sIsReplace = STL_FALSE;
        }
    }

    if( sStringLen > 0 )
    {
        /*
         * sFromStr범위로 sString을 검사하기 때문에 마지막 sString이 남을 경우가 있다.
         *
         * sStringLen < sValue2->mLength 경우,
         * sFromStr의 범위보다 sString의 byte Length가  적은 경우이므로 비교할 필요가 없다. 
         *
         *     REPLACE( 'AAA', 'AA', 'F' );
         *
         *      str : AA[A]       sFrom : AA
         *               ^
         * 이때, LONGVARCHAR_MAX_PRECISION을 체크해주기 위해 나머지 sString의 Length를 구한다.
         * 
         */
            
        if( (sResultLen + sStringLen) > sResultValue->mBufferSize )
        {
            sBufferSize = STL_MIN( (sResultLen + sStringLen), DTL_LONGVARCHAR_MAX_PRECISION );

            sResultValue->mLength = sResultLen;
                        
            STL_TRY( aVectorFunc->mReallocAndMoveLongVaryingMemFunc( aVectorArg,
                                                                     sResultValue,
                                                                     sBufferSize,
                                                                     (stlErrorStack *)aEnv )
                     == STL_SUCCESS );

            sResultValue->mBufferSize = sBufferSize;

            sResultStr = DTF_VARCHAR( sResultValue );
            sResultStr += sResultLen;
        }
        
        stlMemcpy( sResultStr,
                   sString,
                   sStringLen );

        sResultStr += sStringLen;
        sResultLen += sStringLen;
    }
    else
    {
        /* DO NOTHING */
    }
    
    STL_RAMP( SUCCESS_END );
    
    sResultValue->mLength = sResultLen;

    STL_DASSERT( sResultValue->mLength <= sResultValue->mBufferSize );
   
    return STL_SUCCESS;

    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_STRING_BYTE_LENGTH_GREATER_THAN_COLUMN_LENGTH,
                      NULL,
                      sErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
