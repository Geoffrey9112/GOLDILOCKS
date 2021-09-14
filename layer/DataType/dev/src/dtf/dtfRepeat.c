/*******************************************************************************
 * dtfRepeat.c
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
 * @file dtfRepeat.c
 * @brief String Function DataType Library Layer
 */

#include <dtl.h>
#include <dtDef.h>
#include <dtsCharacterSet.h>

/*******************************************************************************
 *   Repeat
 *   Repeat ==> [ P, M ]
 * ex) Repeat( 'Pg', 3 ) => PgPgPg
 *******************************************************************************/

/**
 * @brief Repeat 지정된 수만큼 string반복
 *        <BR> Repeat(str, num)
 *        <BR>   Repeat => [ P, M ]
 *        <BR> ex) Repeat( 'Pg', 3 )  =>  PgPgPg
 *        <BR> ex) Repeat( null, 1 )  =>  null
 *        <BR> ex) Repeat( 'a', 0 )  =>  null
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> Repeat(VARCHAR, INTEGER)
 * @param[out] aResultValue      연산 결과 (결과타입 VARCHAR)
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfStringRepeat( stlUInt16        aInputArgumentCnt,
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
    
    dtlVarcharType    sChrVal;
    dtlVarcharType    sResultVal;
    stlInt32          sRepeatNum;
    stlInt64          sStrLen;
    stlInt64          sChrLen;
    stlInt64          sResultLen = 0;
    stlInt64          sLenCount = 0;
    stlInt64          sChrCount = 0;
    
    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );

    sValue1      = aInputArgument[0].mValue.mDataValue;
    sValue2      = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack  = (stlErrorStack *)aEnv;
        
    DTF_CHECK_DTL_VALUE2( sValue1,
                          DTL_TYPE_CHAR,
                          DTL_TYPE_VARCHAR,
                          sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_NATIVE_INTEGER, sErrorStack ); 
    
    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_VARCHAR, sErrorStack );

    /**
     * @bug dtlCharacterSet를 이용해야 한다.
     */
    /* sCharSetID = aVectorFunc->mGetCharSetIDFunc( aVectorArg ); */

    sChrVal    = DTF_VARCHAR( sValue1 );
    sRepeatNum  = DTF_INTEGER( sValue2 );
    sResultVal = DTF_VARCHAR( sResultValue );
    sStrLen    = sValue1->mLength;
    
    /* sRepeatNum 의 음수처리 */
    if( sRepeatNum <= 0 )
    {
        /*
         *  repeat횟수가 적절하지 않으므로 null
         *           *  REPEAT( 'a', -1 ); || REPEAT( 'a', 0 ); 
         *
         *   결과 : null
         */
        
        sResultLen = 0;
    }
    else
    {
        /* sChrVal의 Character Length를 구하여 Repeat시의 VARCHAR_MAX_PRESICION을 검사 */
        STL_TRY( dtsGetMbstrlenWithLenFuncList[ aVectorFunc->mGetCharSetIDFunc(aVectorArg) ](
                     sChrVal,
                     sStrLen,
                     &sChrLen,
                     aVectorFunc,
                     aVectorArg,
                     sErrorStack )
                 == STL_SUCCESS );
        
        /* VARCHAR OVERFLOW CHECK : Character Length * 반복할 수를 통해 최대 길이를 구함*/
        STL_TRY_THROW( ( sChrLen * sRepeatNum ) <= DTL_VARCHAR_MAX_PRECISION, ERROR_OUT_OF_RANGE );

        sResultLen = sStrLen * sRepeatNum;

        /* 이 부분에 해당하는 경우는 sRepeatNum > 0 이므로 한번은 무조건 해준다 */
        stlMemcpy( sResultVal,
                   sChrVal,
                   (stlSize)sStrLen );
        sLenCount += sStrLen;
        sChrCount ++;
        sRepeatNum --;

        while( 0 < sRepeatNum )
        {
            /*
             * sString을 sResultVal에 반복하여 넣는데,
             * 만약 이전에 넣은 값의 두배를 추가로 넣어도 sRepeatNum을 넘지 않으면
             * 2의 배수로 계속 넣는다. 그렇지 않으면 남은 값은 반복하여 넣는다
             *
             * REPEAT('a', 20);
             *
             * => a + a + aa + aaaa + aaaaaaaa ...
             *  
             */
            
            if( sChrCount <= sRepeatNum )
            {
                stlMemcpy( ( sResultVal + sLenCount ),
                           sResultVal,
                           (stlSize)sLenCount );
                
                sRepeatNum -= sChrCount;
                sLenCount *= 2;
                sChrCount *= 2;
            }
            else
            {
               /*
                * 남은 부분은 횟수만큼 넣는다 
                * REPEAT('a', 20);
                *
                * => a + a + aa + aaaa + aaaaaaaa + [ aaaa ]
                *    ^........... 16 .............^ [ ^ 4 ^ ]
                */
                stlMemcpy( ( sResultVal + sLenCount ),
                           sResultVal,
                           (stlSize)( sStrLen * sRepeatNum ));
                sRepeatNum = 0;

                STL_ASSERT( ( sLenCount + ( sStrLen * sRepeatNum ) ) != sResultLen );
            }
        }
    }
    
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
 * @brief Repeat 지정된 수만큼 long varchar string반복
 *        <BR> Repeat(str, num)
 *        <BR>   Repeat => [ P, M ]
 *        <BR> ex) Repeat( 'Pg', 3 )  =>  PgPgPg
 *        <BR> ex) Repeat( null, 1 )  =>  null
 *        <BR> ex) Repeat( 'a', 0 )  =>  null
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> Repeat(LONGVARCHAR , INTEGER)
 * @param[out] aResultValue      연산 결과 (결과타입 LONGVARCHAR)
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfLongvarcharStringRepeat( stlUInt16        aInputArgumentCnt,
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
    
    dtlVarcharType    sChrVal;
    dtlVarcharType    sResultVal;
    stlInt32          sRepeatNum;
    stlInt64          sStrLen;
    stlInt64          sResultLen = 0;
    stlInt64          sLenCount = 0;
    stlInt64          sChrCount = 0;
    
    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );

    sValue1      = aInputArgument[0].mValue.mDataValue;
    sValue2      = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack  = (stlErrorStack *)aEnv;
        
    DTF_CHECK_DTL_GROUP2( sValue1,
                          DTL_GROUP_CHAR_STRING,
                          DTL_GROUP_LONGVARCHAR_STRING,
                          sErrorStack );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_NATIVE_INTEGER, sErrorStack ); 
    
    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_LONGVARCHAR, sErrorStack );

    /**
     * @bug dtlCharacterSet를 이용해야 한다.
     */
    /* sCharSetID = aVectorFunc->mGetCharSetIDFunc( aVectorArg ); */

    sChrVal    = DTF_VARCHAR( sValue1 );
    sRepeatNum  = DTF_INTEGER( sValue2 );
    sResultVal = DTF_VARCHAR( sResultValue );
    sStrLen    = sValue1->mLength;
    
    /* sRepeatNum 의 음수처리 */
    if( sRepeatNum <= 0 )
    {
        /*
         *  repeat횟수가 적절하지 않으므로 null
         *           *  REPEAT( 'a', -1 ); || REPEAT( 'a', 0 ); 
         *
         *   결과 : null
         */
        
        sResultLen = 0;
    }
    else
    {
        /* LONGVARCHAR OVERFLOW CHECK : Character Length * 반복할 수를 통해 최대 길이를 구함*/
        STL_TRY_THROW( ( sStrLen * sRepeatNum ) <= DTL_LONGVARCHAR_MAX_PRECISION,
                       ERROR_OUT_OF_RANGE );

        sResultLen = sStrLen * sRepeatNum;

        /* 이 부분에 해당하는 경우는 sRepeatNum > 0 이므로 한번은 무조건 해준다 */
        stlMemcpy( sResultVal,
                   sChrVal,
                   (stlSize)sStrLen );
        sLenCount += sStrLen;
        sChrCount ++;
        sRepeatNum --;

        while( 0 < sRepeatNum )
        {
            /*
             * sString을 sResultVal에 반복하여 넣는데,
             * 만약 이전에 넣은 값의 두배를 추가로 넣어도 sRepeatNum을 넘지 않으면
             * 2의 배수로 계속 넣는다. 그렇지 않으면 남은 값은 반복하여 넣는다
             *
             * REPEAT('a', 20);
             *
             * => a + a + aa + aaaa + aaaaaaaa ...
             *  
             */
            
            if( sChrCount <= sRepeatNum )
            {
                stlMemcpy( ( sResultVal + sLenCount ),
                           sResultVal,
                           (stlSize)sLenCount );
                
                sRepeatNum -= sChrCount;
                sLenCount *= 2;
                sChrCount *= 2;
            }
            else
            {
               /*
                * 남은 부분은 횟수만큼 넣는다 
                * REPEAT('a', 20);
                *
                * => a + a + aa + aaaa + aaaaaaaa + [ aaaa ]
                *    ^........... 16 .............^ [ ^ 4 ^ ]
                */
                stlMemcpy( ( sResultVal + sLenCount ),
                           sResultVal,
                           (stlSize)( sStrLen * sRepeatNum ));
                sRepeatNum = 0;

                STL_ASSERT( ( sLenCount + ( sStrLen * sRepeatNum ) ) != sResultLen );
            }
        }
    }
    
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
 * @brief Repeat 지정된 수만큼 string반복
 *        <BR> Repeat(str, num)
 *        <BR>   Repeat => [ P, M ]
 *        <BR> ex) Repeat( X'ab', 3 )  =>  ababab
 *        <BR> ex) Repeat( null, 1 )  =>  null
 *        <BR> ex) Repeat( X'aa', 0 )  =>  null
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> Repeat(VARBINARY | LONGVARBINARY, INTEGER)
 * @param[in]  aMaxPrecision     Maximum Precision
 * @param[out] aResultValue      연산 결과 (결과타입 VARBINARY | LONGVARBINARY)
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfBinaryRepeat( stlUInt16        aInputArgumentCnt,
                           dtlValueEntry  * aInputArgument,
                           stlInt64         aMaxPrecision,
                           void           * aResultValue,
                           dtlFuncVector  * aVectorFunc,
                           void           * aVectorArg,
                           void           * aEnv )
{    
    dtlDataValue    * sValue1;
    dtlDataValue    * sValue2;
    dtlDataValue    * sResultValue;
    stlErrorStack   * sErrorStack;
    
    dtlVarbinaryType  sChrVal;
    dtlVarbinaryType  sResultVal;
    stlInt32          sRepeatNum;
    stlInt64          sStrLen;
    stlInt64          sResultLen = 0;
    stlInt64          sLenCount = 0;
    stlInt64          sChrCount = 0;
    
    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );

    sValue1      = aInputArgument[0].mValue.mDataValue;
    sValue2      = aInputArgument[1].mValue.mDataValue;
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

    sChrVal     = DTF_VARBINARY( sValue1 );
    sRepeatNum  = DTF_INTEGER( sValue2 );
    sResultVal  = DTF_VARBINARY( sResultValue );
    sStrLen     = sValue1->mLength;
    
    /* sRepeatNum 의 음수처리 */
    if( sRepeatNum <= 0 )
    {
        /*
         *  repeat횟수가 적절하지 않으므로 null
         *  
         *  REPEAT( 'a', -1 ); || REPEAT( 'a', 0 ); 
         *
         *   결과 : null
         */
        sResultLen = 0;
    }
    else
    {        
        /* VARCHAR OVERFLOW CHECK : Character Length * 반복할 수를 통해 최대 길이를 구함*/
        STL_TRY_THROW( ( sStrLen * sRepeatNum ) <= aMaxPrecision, ERROR_OUT_OF_RANGE );

        sResultLen = sStrLen * sRepeatNum;
        
        /* 이 부분에 해당하는 경우는 sRepeatNum > 0 이므로 한번은 무조건 해준다 */
        stlMemcpy( sResultVal,
                   sChrVal,
                   (stlSize)sStrLen );
        
        sLenCount += sStrLen;
        sChrCount ++;
        sRepeatNum --;

        while( 0 < sRepeatNum )
        {
            /*
             * sString을 sResultVal에 반복하여 넣는데,
             * 만약 이전에 넣은 값의 두배를 추가로 넣어도 sRepeatNum을 넘지 않으면
             * 2의 배수로 계속 넣는다. 그렇지 않으면 남은 값은 반복하여 넣는다
             *
             * REPEAT('a', 20);
             *
             * => a + a + aa + aaaa + aaaaaaaa...
             *  
             */
            
            if( sChrCount <= sRepeatNum )
            {
                stlMemcpy( ( sResultVal + sLenCount ),
                           sResultVal,
                           (stlSize)sLenCount );
                
                sRepeatNum -= sChrCount;
                sLenCount  *= 2;
                sChrCount  *= 2;
            }
            else
            {
                /*
                 * 남은 부분은 횟수만큼 넣는다 
                 * REPEAT('a', 20);
                 *
                 * => a + a + aa + aaaa + aaaaaaaa + [ aaaa  ]
                 *    ^........... 16 .............^ [ ^ 4 ^ ]
                 */
                stlMemcpy( ( sResultVal + sLenCount ),
                           sResultVal,
                           (stlSize)( sStrLen * sRepeatNum ));
                sRepeatNum = 0;
                
                STL_ASSERT( ( sLenCount + ( sStrLen * sRepeatNum ) ) != sResultLen );
            }
        }
    }
    
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

/** @} */
