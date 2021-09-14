/*******************************************************************************
 * dtfInitcap.c
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
 * @file dtfInitcap.c
 * @brief String Function DataType Library Layer
 */

#include <dtl.h>
#include <dtDef.h>
#include <dtsCharacterSet.h>

/*******************************************************************************
 *   Initcap
 *   Initcap ==> [ P, O ]
 * ex) Initcap( 'hi GLIESE' ) => Hi Gliese
 *******************************************************************************/

/**
 * @brief Initcap 문자열내의 첫번째 문자을 대문자로 반환
 *        <BR> Initcap(str)
 *        <BR>   Initcap => [ P, O ]
 *        <BR> ex) Initcap( 'hi GLIESE' )  =>  Hi Gliese
 *        <BR> ex) Initcap( null )  =>  null
 *        <BR> ex) Initcap( '' )  =>  null
 *        <BR> ex) Initcap( '0' )  =>  0
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> Initcap(CHARACTER or VARCHAR or LONGVARCHAR)
 * @param[out] aResultValue      연산 결과 (결과타입 CHAR or VARCHAR or LONGVARCHAR)
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */

stlStatus dtfInitcap( stlUInt16        aInputArgumentCnt,
                      dtlValueEntry  * aInputArgument,
                      void           * aResultValue,
                      dtlFuncVector  * aVectorFunc,
                      void           * aVectorArg,
                      void           * aEnv )
{
    /**
     * 논의대상.
     * 문자열의 구분 기준,
     * PostgreSQL은 White space 또는 기호문자 이후는 대문자 표시.
     * 오라클은 White space 또는 알파벳, 숫자가 아닌 문자 이후에 대문자 표시.
     * 현재는 오라클을 따라가고 있음.
     *
     *   SELECT INITCAP('가r__r*r r1r aa') FROM DUAL;
     *
     *    Oracle     => [ 가R__R*R R1r Aa] 
     *    Postgrel   => [ 가r__R*R R1r Aa]
     **/
    
    dtlDataValue    * sValue1;
    dtlDataValue    * sResultValue;
    stlErrorStack   * sErrorStack;
    dtlCharacterSet   sCharSetID;
    stlChar         * sChrVal;
    stlChar         * sResultVal;  
    stlBool           sIsInitcap;
    stlInt8           sByteLen;
    stlInt64          sStrLen;
    
    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv );

    sValue1      = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack  = (stlErrorStack *)aEnv;

    /**
     * aInputArgument type은 DTL_TYPE_CHAR 또는 DTL_TYPE_VARCHAR 또는 DTL_TYPE_LONGVARCHAR 이어야 한다.
     * aResultValue type은 DTL_TYPE_CHAR 또는 DTL_TYPE_VARCHAR 이어야 한다.
     * 예) initcap( DTL_TYPE_CHAR )    ==> result : DTL_TYPE_CHAR
     *     initcap( DTL_TYPE_VARCHAR ) ==> result : DTL_TYPE_VARCHAR
     *     initcap( DTL_TYPE_LONGVARCHAR ) ==> result: DTL_TYPE_LONGVARCHAR
     */
    DTF_CHECK_DTL_GROUP2( sValue1,
                          DTL_GROUP_CHAR_STRING,
                          DTL_GROUP_LONGVARCHAR_STRING,
                          sErrorStack );

    DTF_CHECK_DTL_RESULT3( sValue1,
                           DTL_TYPE_CHAR,
                           DTL_TYPE_VARCHAR,
                           DTL_TYPE_LONGVARCHAR,
                           sErrorStack );
    
    sCharSetID = aVectorFunc->mGetCharSetIDFunc( aVectorArg );
    sChrVal    = sValue1->mValue;
    sResultVal = sResultValue->mValue;    
    sStrLen    = sValue1->mLength;
    sIsInitcap = STL_TRUE;

    STL_DASSERT( sValue1->mLength <= sResultValue->mBufferSize );
 
    /* Character 수를 계산한다. */
    while( sStrLen > 0 )
    {
         /**
          * Multibyte(ex: 가)같은 경우에는 byte length를 구하여 따로 처리한다.
          * sIsInitcap가 STL_TRUE이면 next 문자는 Initcap라는 의미이다
          **/
        STL_TRY( dtsGetMbLengthFuncList[sCharSetID]( sChrVal,
                                                     &sByteLen,
                                                     sErrorStack )
                 == STL_SUCCESS );

        if( sByteLen == 1 )
        {
            /**
             * 문자열의 구분 기준을 오라클과 같이 함에 따라
             * 숫자 혹은 알파벳이 아닌 문자, 또는 White space로 구분을 해준다.
             **/
            if( stlIsalnum( *sChrVal ) == STL_TRUE )
            {
                /**
                 * 해당 구간은 숫자 및 알파벳만 적용되므로 Multibyte 검사가 필요없다.
                 * sIsInitcap가 STL_TRUE이면 구분자 이후의 문자이므로 대문자를 return 한다.
                 * 나머지 문자들은 소문자로 return 한다.
                 * ( ex: Hi [g]LIESE -> Hi Gliese )
                 **/
                if( sIsInitcap == STL_TRUE )
                {
                    /* 대문자로 변환 */
                    *sResultVal = stlToupper( *sChrVal );
                }
                else
                {
                    /* 소문자로 변환 */
                    *sResultVal = stlTolower( *sChrVal );
                }
                sIsInitcap = STL_FALSE;
            }
            else
            {
                *sResultVal = *sChrVal;
                sIsInitcap = STL_TRUE;
            }
        }
        else
        {
            /* Multibyte의 경우이므로 문자를 복사한 후 Initcap 을 TRUE로 한다 */
            stlMemcpy( sResultVal,
                       sChrVal,
                       sByteLen );

            sIsInitcap = STL_TRUE;
        }
        sChrVal    += sByteLen;
        sResultVal += sByteLen;
        sStrLen    -= sByteLen;
    }
    
    sResultValue->mLength = sValue1->mLength;
   
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
