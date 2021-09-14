/*******************************************************************************
 * dtfSplitPart.c
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
 * @file dtfSplitPart.c
 * @brief String Function DataType Library Layer
 */

#include <dtl.h>
#include <dtDef.h>
#include <dtsCharacterSet.h>

/*******************************************************************************
 * split_part(string, delimiter, field)
 *   split_part(string, delimiter, field) => [ P ]
 * <BR>    : string을 delimiter로 구분해,  그결과로부터 지정된 field를 반환 
 * <BR>  ex) split_part('abc~@~def~@~ghi', '~@~', 2)  =>  def
 *******************************************************************************/

/**
 * @brief  split_part(string, delimiter, field)
 *         <BR> string을 delimiter로 구분해,  그결과로부터 지정된 field를 반환
 *         <BR> split_part(string, delimiter, field)
 *         <BR>   split_part(string, delimiter, field) => [ P ]
 *         <BR> ex) split_part('abc~@~def~@~ghi', '~@~', 2)    =>  def
 *         <BR> ex) split_part('abc~@~def~@~ghi', '~@~', 0)    =>  error
 *         <BR> ex) split_part('abc~@~def~@~ghi', '~@~', '2')  =>  def
 *         <BR> ex) split_part('abc~@~def~@~ghi', '', 1 )     =>   null
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> Split_part(CHAR_STRING | LONGVARCHAR_STRING,
 *                               <BR>            CHAR_STRING | LONGVARCHAR_STRING,
 *                               <BR>            INTEGER)
 * @param[out] aResultValue      연산 결과 (결과타입 VARCHAR | LONGVARCHAR)
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfSplitPart( stlUInt16        aInputArgumentCnt,
                        dtlValueEntry  * aInputArgument,
                        void           * aResultValue,
                        dtlFuncVector  * aVectorFunc,
                        void           * aVectorArg,
                        void           * aEnv )
{             
    dtlDataValue    * sValue1;
    dtlDataValue    * sValue2;
    dtlDataValue    * sValue3;
    dtlDataValue    * sResultValue;
    dtlCharacterSet   sCharSetID;
    stlErrorStack   * sErrorStack;
    
    dtlVarcharType    sString;
    dtlVarcharType    sDelimiter;
    dtlIntegerType    sFieldNum;
    dtlVarcharType    sResultStr;
    dtlVarcharType    sPartString;
    stlInt64          sResultLen = 0;
    stlInt64          sStringLen;
    stlInt64          sDelimiterChrLen;
    stlInt64          sStringChr;
    stlInt64          sPartStringLen = 0;
    stlInt8           sStrMbLen;
    stlBool           sIsSplit = STL_FALSE;
    stlInt64          sSplitNum = 0;
    
    DTL_PARAM_VALIDATE( aInputArgumentCnt == 3 , (stlErrorStack *)aEnv );

    sValue1      = aInputArgument[0].mValue.mDataValue;
    sValue2      = aInputArgument[1].mValue.mDataValue;
    sValue3      = aInputArgument[2].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack  = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_GROUP2( sValue1,
                          DTL_GROUP_CHAR_STRING,
                          DTL_GROUP_LONGVARCHAR_STRING,
                          sErrorStack );
    DTF_CHECK_DTL_GROUP2( sValue2,
                          DTL_GROUP_CHAR_STRING,
                          DTL_GROUP_LONGVARCHAR_STRING,
                          sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue3, DTL_TYPE_NATIVE_INTEGER, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT2( sResultValue,
                           DTL_TYPE_VARCHAR,
                           DTL_TYPE_LONGVARCHAR,
                           sErrorStack );
    
    sCharSetID  = aVectorFunc->mGetCharSetIDFunc( aVectorArg );
    sString     = DTF_VARCHAR( sValue1 );
    sDelimiter  = DTF_VARCHAR( sValue2 );
    sFieldNum   = DTF_INTEGER( sValue3 );
    sResultStr  = DTF_VARCHAR( sResultValue );
    sStringLen  = sValue1->mLength;

    if( sStringLen < sValue2->mLength )
    {
        if( sFieldNum == 1 )
        {
            /*
             * sString보다 sDelimiter이 클경우,
             * sString에는 sDelimiter로 구분 할 수 없기 때문에 비교하지 않고 반환함 
             *
             *  SPLIT_PART( 'ABBC', 'ABCDEF', 1);
             *
             *  Result : ABBC
             */
        
            stlMemcpy( sResultStr,
                       sString,
                       sStringLen );
        
            sResultLen = sStringLen;
        }
        else
        {
            DTL_SET_NULL( sResultValue );
        }
        STL_THROW( SUCCESS_END );
    }
    else
    {
        /* Do Nothing */
    }

    /* 0 및 음수 처리 */
    if( sFieldNum <= 0 )
    {
        STL_THROW( FIELD_INVALID_NUMBER );
    }
    else
    {
        /* Do Nothing */
    }
    
    sPartString = DTF_VARCHAR( sValue1 );
    
    STL_TRY( dtsGetMbstrlenWithLenFuncList[ sCharSetID ]( sDelimiter,
                                                          sValue2->mLength,
                                                          &sDelimiterChrLen,
                                                          aVectorFunc,
                                                          aVectorArg,
                                                          sErrorStack )
             == STL_SUCCESS );
    
    while( sStringLen >= sValue2->mLength )
    {
        if( stlMemcmp( sString, sDelimiter, sValue2->mLength ) == 0 )
        {
            /*
             * sString을 sDelimiter범위로 검사하여 동일한 값인지 검사
             * sString과 sDelimiter의 byte Length만큼의 단어가 동일하다.
             *
             *  SPLIT_PART( 'AAAbbbCCC','bbb',1 );
             *
             *  string : AAA[bbb]CCC     delimiter: [bbb]
             *                 ^                       ^
             */
                
            STL_TRY( dtsGetMbstrlenWithLenFuncList[ sCharSetID ]( sString,
                                                                  sValue2->mLength,
                                                                  &sStringChr,
                                                                  aVectorFunc,
                                                                  aVectorArg,
                                                                  sErrorStack )
                     == STL_SUCCESS );
            
            if( sDelimiterChrLen == sStringChr )
            {
                /*
                 * sChracter Length 가 동일함므로 두 값은 동일함을 알 수 있다
                 *
                 * SPLIT_PART( 'aa bb cc dd', ' ', 3);
                 *
                 * reult :  cc 
                 * 
                 */

                sSplitNum ++;
                sString += sValue2->mLength;
                sStringLen -= sValue2->mLength;
                
                if( sFieldNum == sSplitNum )
                {
                    /*
                     *  sFieldNum과 sSplitNum이 같은 경우는,
                     *  Split가 되고 나서 sFieldNum의 값을 구할 수 있는 경우이다.
                     *  현재 가르키고 있는 sDelimiterChrLen의 이전 값을 복사하면 된다.
                     *
                     *  SPLIT_PART( 'AAAbbbCCC','bbb',1 );
                     *
                     *  string : AAA[bbb]CCC     delimiter: [bbb]
                     *                 ^                       ^
                     *  result : AAA 
                     */
                    
                    stlMemcpy( sResultStr,
                               sPartString,
                               sPartStringLen );
                    
                    sResultLen = sPartStringLen;
                    break;
                }
                else
                {
                    /*
                     *  sFieldNum 과 sSplitNum이 같지 않는 경우는,
                     *  아직 sFieldNum에 만족할 만한 split가 이루어지지 않았으므로 더 검사해봐야 한다.
                     *  또한, Split로 인해 나누어진 부분은 반환하지 않으므로 Copy할 String은
                     *  초기화한다. 
                     *
                     *  SPLIT_PART( 'AA BB CC DD', ' ', 3 );
                     *
                     *  string : AA BB CC DD
                     *             ^   
                     *  result : CC 
                     */
                    
                    sPartString += ( sPartStringLen + sValue2->mLength );
                    sPartStringLen = 0;                    
                }
                sIsSplit = STL_TRUE;                    
            }
            else
            {
                /* sDelimiterChrLen != sStringChr */
            }
        }
        else
        {
            /* stlMemcmp( sString, sDelimiter ) != 0 */
        }

        if( sIsSplit == STL_FALSE )
        {
            /*
             * String을 Delimiter의 길이로 검사를 했을 때, 두 부분이 같지 않으므로
             * sString을 1 Character 증가시켜 다음을 검사한다. 
             *
             *  SPLIT_PART( 'ABCbbABC', 'bb', 2);
             *
             *  string : A[BC]bbABC      delimiter : [bb]
             *           ^                            ^
             */
            
            STL_TRY( dtsGetMbLengthFuncList[sCharSetID]( sString,
                                                         &sStrMbLen,
                                                         sErrorStack )
                     == STL_SUCCESS );

            sString += sStrMbLen;
            sStringLen -= sStrMbLen;
            sPartStringLen += sStrMbLen;
        }
        else
        {
            sIsSplit = STL_FALSE;
        }
    }

    sSplitNum ++;
    sPartStringLen += sStringLen;
    
    if( sSplitNum  == sFieldNum )
    {
        /*
         *  sSplitNum +1 == sFieldNum 경우는,
         *  이전의 작업으로 인해 split가 되었고 split 이후의 값이 필요한 경우이다.
         *
         *  SPLIT_PART( 'AA BB CC',' ',3 );
         *
         *  string : AA BB [CC]
         *                 
         *  result : CC
         */
    
        stlMemcpy( sResultStr,
                   sPartString,
                   sPartStringLen );

        sResultLen = sPartStringLen;
    }
    else
    {
        /* Do Nothing */
    }
       
    STL_RAMP( SUCCESS_END );
    
    sResultValue->mLength = sResultLen;

    STL_DASSERT( sResultValue->mLength <= sResultValue->mBufferSize );
   
    return STL_SUCCESS;
 
    STL_CATCH( FIELD_INVALID_NUMBER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INVALID_NTH_ARGUMENT_VALUE,
                      NULL,
                      sErrorStack,
                      3,
                      "SPLIT_PART" );        
    }

    STL_FINISH;
    
    return STL_FAILURE;
}

/** @} */
