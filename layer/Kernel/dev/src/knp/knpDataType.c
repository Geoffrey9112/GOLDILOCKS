/******************************************************************************* 
 * knpDataType.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id$
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file knpDataType.c
 * @brief Kernel Layer Property Internal Routines
 */

#include <knl.h>
#include <knDef.h>
#include <knpDef.h>
#include <knpPropertyManager.h>
#include <knpDataType.h>

/**
 * @addtogroup knpProperty
 * @{
 */


/**
 * @brief 추가할 값이 정상적인지를 확인한다.
 * @param[in]     aContent      knpPropertyContent
 * @param[in]     aValue        추가할 값
 * @param[in,out] aKnlEnv       Kernel Environment
 * @par Error Codes:
 * @code
 * [KNL_ERRCODE_PROPERTY_INVALID_VALUE]
 * 해당 Property의 값이 정상적이지 않습니다.
 * - 해당 값을 확인합니다.
 * @endcode
 * @remarks
 */

stlStatus knpVarcharVerifyValue( knpPropertyContent     * aContent, 
                                 void                   * aValue,
                                 knlEnv                 * aKnlEnv )
{
    stlUInt32     sLen = 0;
    
    /**
     * Parameter Validation
     */
    STL_PARAM_VALIDATE( aContent   != NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( aValue     != NULL, KNL_ERROR_STACK(aKnlEnv) );
    
    sLen = stlStrlen((stlChar*)aValue);
    
    /**
     * RANGE_CHECK에 대한 체크를 하지 않고 무조건 길이에 대한 Validation을 체크한다.
     * 문자의 길이가 정상적인지 확인한다. (최대값 최소값)
     * NULL 문자를 포함하기 위해서 +1 을 해준다.
     */
    STL_TRY_THROW( sLen > aContent->mMin, RAMP_ERR_INVALID_VALUE );
    
    STL_TRY_THROW( sLen + 1 <= aContent->mMax, RAMP_ERR_INVALID_VALUE );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_PROPERTY_INVALID_VALUE, 
                      NULL,
                      KNL_ERROR_STACK(aKnlEnv), 
                      aContent->mName );
    }    
    STL_FINISH;

    return STL_FAILURE;    
}


/**
 * @brief 추가할 값이 정상적인지를 확인한다.
 * @param[in]     aContent      knpPropertyContent
 * @param[in]     aValue        추가할 값
 * @param[in,out] aKnlEnv       Kernel Environment
 * @par Error Codes:
 * @code
 * [KNL_ERRCODE_PROPERTY_INVALID_VALUE]
 * 해당 Property의 값이 정상적이지 않습니다.
 * - 해당 값을 확인합니다.
 * @endcode
 * @remarks
 */

stlStatus knpBooleanVerifyValue( knpPropertyContent     * aContent, 
                                 void                   * aValue,
                                 knlEnv                 * aKnlEnv )
{
    /**
     * Parameter Validation
     */
    STL_PARAM_VALIDATE( aContent   != NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( aValue     != NULL, KNL_ERROR_STACK(aKnlEnv) );
    
    /**
     * TRUE/FALSE 인지를 확인해야 한다.
     */
    STL_TRY_THROW( ( *(stlBool*)aValue == STL_TRUE ||
                     *(stlBool*)aValue == STL_FALSE ) == STL_TRUE, RAMP_ERR_INVALID_VALUE );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_PROPERTY_INVALID_VALUE, 
                      NULL,
                      KNL_ERROR_STACK(aKnlEnv),
                      aContent->mName );
    }
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief 추가할 값이 정상적인지를 확인한다.
 * @param[in]     aContent      knpPropertyContent
 * @param[in]     aValue        추가할 값
 * @param[in,out] aKnlEnv       Kernel Environment
 * @par Error Codes:
 * @code
 * [KNL_ERRCODE_PROPERTY_INVALID_VALUE]
 * 해당 Property의 값이 정상적이지 않습니다.
 * - 해당 값을 확인합니다.
 * @endcode
 * @remarks
 */

stlStatus knpBigIntVerifyValue( knpPropertyContent     * aContent, 
                                void                   * aValue,
                                knlEnv                 * aKnlEnv )
{
    stlBool  sIsCheck = STL_FALSE;
    
    /**
     * Parameter Validation
     */
    STL_PARAM_VALIDATE( aContent   != NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( aValue     != NULL, KNL_ERROR_STACK(aKnlEnv) );
    
    STL_TRY( knpGetPropertyCheckRange( aContent,
                                       &sIsCheck,
                                       aKnlEnv ) == STL_SUCCESS );
    /**
     * 범위 검사를 한다.
     */
    if( sIsCheck == STL_TRUE )
    {
        STL_TRY_THROW( *(stlInt64*)aValue >= aContent->mMin, RAMP_ERR_INVALID_VALUE );
        STL_TRY_THROW( *(stlInt64*)aValue <= aContent->mMax, RAMP_ERR_INVALID_VALUE );
    }

    /**
     * 증가 여부는 KNL_STARTUP_PHASE_NONE 상태에서는 체크하지 않는다.
     * (gcreatedb(KNL_STARTUP_PHASE_NONE) 에서 default값에서 줄이는것 가능해야 함.)
     */
    if( knlGetCurrStartupPhase() != KNL_STARTUP_PHASE_NONE )
    {
        STL_TRY( knpGetPropertyCheckIncrease( aContent,
                                              &sIsCheck,
                                              aKnlEnv ) == STL_SUCCESS );
        /**
         * 값의 증가 검사를 한다.
         * 기존 값보다 같거나 커야 함
         */
        if( sIsCheck == STL_TRUE )
        {
            STL_TRY_THROW( *(stlInt64*)aValue >= *(stlInt64*)(aContent->mValue), RAMP_ERR_INVALID_VALUE );
        }
    }

    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_PROPERTY_INVALID_VALUE, 
                      NULL,
                      KNL_ERROR_STACK(aKnlEnv),
                      aContent->mName );
    }
    STL_FINISH;

    return STL_FAILURE;   
}


/**
 * @brief 문자를 얻어온다.
 * @param[in]     aValue        입력한 문자
 * @param[out]    aResult       반환할 결과
 * @param[in,out] aKnlEnv       Kernel Environment
 * @remarks
 */

stlStatus knpVarcharConvertFromString( stlChar   * aValue,
                                       void     ** aResult,
                                       knlEnv    * aKnlEnv )
{
    /**
     * Parameter Validation
     */
    STL_PARAM_VALIDATE(  aValue  != NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( *aResult == NULL, KNL_ERROR_STACK(aKnlEnv) );

    /**
     * 임시로 사용되는 메모리이며 STRING MAX 값으로 Alloc 한다.
     */ 
    STL_TRY( knlAllocRegionMem( &(gKnpPropertyMgr->mHeapMem),
                                KNL_PROPERTY_STRING_MAX_LENGTH,
                                aResult,
                                aKnlEnv ) == STL_SUCCESS );    
    
    /**
     * 문자를 복사한다.
     */
    stlStrncpy( *aResult, 
                 aValue, 
                 stlStrlen(aValue) + 1 );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;    
}


/**
 * @brief 문자를 Boolean형으로 변환한다.
 * @param[in]     aValue        입력한 문자
 * @param[out]    aResult       반환할 결과
 * @param[in,out] aKnlEnv       Kernel Environment
 * @par Error Codes:
 * @code
 * [KNL_ERRCODE_PROPERTY_INVALID_VALUE]
 * 해당 Property의 값이 정상적이지 않습니다.
 * - 해당 값을 확인합니다.
 * @endcode
 * @remarks
 */

stlStatus knpBooleanConvertFromString( stlChar   * aValue,
                                       void     ** aResult,
                                       knlEnv    * aKnlEnv )
{
    stlInt32    sLen      = 0;
    stlBool     sRetValue = STL_FALSE;
    
    /**
     * Parameter Validation
     */
    STL_PARAM_VALIDATE(  aValue  != NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( *aResult == NULL, KNL_ERROR_STACK(aKnlEnv) );
    
    STL_TRY( knlAllocRegionMem( &(gKnpPropertyMgr->mHeapMem),
                                STL_SIZEOF(stlInt64),
                                aResult,
                                aKnlEnv ) == STL_SUCCESS );
    
    /**
     * 0/1, ON/OFF, TRUE/FALSE, YES/NO, ENABLE/DISABLE 의 문자를 해당 값으로 변경한다.
     */
    
    sLen = stlStrlen( aValue );
    
    if( sLen == 1 )
    {
        if( stlStrncasecmp( (stlChar*)aValue, "1", 1 ) == 0 )
        {
            sRetValue = STL_TRUE;
        }
        else if( stlStrncasecmp( (stlChar*)aValue, "0", 1 ) == 0 )
        {
            sRetValue = STL_FALSE;
        }
        else
        {
            STL_THROW( RAMP_ERR_INVALID_VALUE );
        }
    }
    else if( sLen == 2 )
    {
        if( stlStrncasecmp( (stlChar*)aValue, "ON", 2 ) == 0 )
        {
            sRetValue = STL_TRUE;
        }
        else if( stlStrncasecmp( (stlChar*)aValue, "NO", 2 ) == 0 )
        {
            sRetValue = STL_FALSE;
        }
        else
        {
            STL_THROW( RAMP_ERR_INVALID_VALUE );
        }
    }
    else if( sLen == 3 )
    {
        if( stlStrncasecmp( (stlChar*)aValue, "YES", 3 ) == 0 )
        {
            sRetValue = STL_TRUE;
        }
        else if( stlStrncasecmp( (stlChar*)aValue, "OFF", 3 ) == 0 )
        {
            sRetValue = STL_FALSE;   
        }
        else
        {
            STL_THROW( RAMP_ERR_INVALID_VALUE );
        }
    }
    else if( sLen == 4 )
    {
        if( stlStrncasecmp( (stlChar*)aValue, "TRUE", 4 ) == 0 )
        {
            sRetValue = STL_TRUE;
        }
        else
        {
            STL_THROW( RAMP_ERR_INVALID_VALUE );
        }
    }
    else if( sLen == 5 )
    {
        if( stlStrncasecmp( (stlChar*)aValue, "FALSE", 5 ) == 0 )
        {
            sRetValue = STL_FALSE;
        }
        else
        {
            STL_THROW( RAMP_ERR_INVALID_VALUE );
        }
    }
    else if( sLen == 6 )
    {
        if( stlStrncasecmp( (stlChar*)aValue, "ENABLE", 6 ) == 0 )
        {
            sRetValue = STL_TRUE;
        }
        else
        {
            STL_THROW( RAMP_ERR_INVALID_VALUE );
        }
    }
    else if( sLen == 7 )
    {
        if( stlStrncasecmp( (stlChar*)aValue, "DISABLE", 7 ) == 0 )
        {
            sRetValue = STL_FALSE;
        }
        else
        {
            STL_THROW( RAMP_ERR_INVALID_VALUE );
        }
    }
    else
    {
        STL_THROW( RAMP_ERR_INVALID_VALUE );
    }
    
    *(stlBool*)(*aResult) = sRetValue;
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_PROPERTY_INVALID_VALUE, 
                      NULL,
                      KNL_ERROR_STACK(aKnlEnv),
                      NULL );
    }
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 문자를 숫자형으로 변환한다.
 * @param[in]     aValue        입력한 문자
 * @param[out]    aResult       반환할 결과
 * @param[in,out] aKnlEnv       Kernel Environment
 * @par Error Codes:
 * @code
 * [KNL_ERRCODE_PROPERTY_INVALID_VALUE]
 * 해당 Property의 값이 정상적이지 않습니다.
 * - 해당 값을 확인합니다.
 * @endcode 
 * @remarks
 */

stlStatus knpBigIntConvertFromString( stlChar   * aValue,
                                      void     ** aResult,
                                      knlEnv    * aKnlEnv )
{
    stlChar     *sEndPtr;
    
    STL_PARAM_VALIDATE( aValue   != NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( *aResult == NULL, KNL_ERROR_STACK(aKnlEnv) );
  
    STL_TRY( knlAllocRegionMem( &(gKnpPropertyMgr->mHeapMem),
                                STL_SIZEOF(stlInt64),
                                aResult,
                                aKnlEnv ) == STL_SUCCESS );
    
    /**
     * 10진수 숫자형으로 변환한다.
     */
    STL_TRY( stlStrToInt64( aValue,
                            STL_NTS,
                            &sEndPtr,
                            10,
                            *aResult, 
                            KNL_ERROR_STACK(aKnlEnv) ) == STL_SUCCESS );
    
    /** 입력된 String이 정상적으로 변환이 되었을 경우에는 EndPtr는 0 된다. */
    if ( *sEndPtr != 0 )
    {
        STL_THROW( RAMP_ERR_INVALID_VALUE );
    }
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_PROPERTY_INVALID_VALUE, 
                      NULL,
                      KNL_ERROR_STACK(aKnlEnv),
                      NULL );
    }
    STL_FINISH;

    return STL_FAILURE;
}

/** @} */

