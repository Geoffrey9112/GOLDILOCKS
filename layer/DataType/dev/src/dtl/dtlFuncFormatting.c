/*******************************************************************************
 * dtlFuncFormatting.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id:$
 *
 * NOTES
 *
 *
 ******************************************************************************/

/**
 * @file dtlFuncFormatting.c
 * @brief Data Type Layer datetime formatting 함수 
 */

#include <dtl.h>
#include <dtDef.h>
#include <dtfFormatting.h>

/**
 * @ingroup dtlFunction Function관련 함수
 * @{
 */

/*******************************************************************************
 * TO_DATETIME FORMAT FUNCTION
 *******************************************************************************/

/*
 * 대표 함수의 정보를 정의한다.
 * mValidArgList는 함수를 결정하는데 사용되는 arg index의 list이다.
 * 함수를 결정하는데 사용되지 않는 arg는 무조건 함수의 원형 type으로
 * 리턴한다.
 */
const dtlFuncInfo dtlFuncToDateTimeFormatInfo =
{
    { STL_FALSE, STL_TRUE  }      /* 함수를 결정하는데 판단이 되는 argument index list */
};

/*
 * 각 함수별 arg type, result type을 정의한다.
 * 주의: 빠르게 처리 가능한 함수를 먼저 정의해야 한다.
 */
const dtlFuncArgInfo dtlFuncToDateTimeFormatList[] =
{
    { dtlToDateTimeFormat,
      { DTL_TYPE_NUMBER, DTL_TYPE_VARCHAR },
      DTL_TYPE_VARBINARY
    },
    { dtlToDateTimeFormat,
      { DTL_TYPE_NUMBER, DTL_TYPE_LONGVARCHAR },
      DTL_TYPE_VARBINARY
    },

    { NULL,
      { DTL_TYPE_NA, },
      DTL_TYPE_NA
    }
};    


/**
 * @brief To_datetime format function에 대한 정보 얻기
 * @param[in]  aDataTypeArrayCount         aDataTypeArrary의 갯수
 * @param[in]  aIsConstantData             constant value인지 여부
 * @param[in]  aDataTypeArray              function을 수행할 data type
 * @param[in]  aDataTypeDefInfoArray       aDataTypeArray에 대응되는 dtlDataTypeDefInfo정보 
 * @param[in]  aFuncArgDataTypeArrayCount  function의 input argument count
 * @param[out] aFuncArgDataTypeArray       function 수행시 수행 argument type
 * @param[out] aFuncArgDataTypeDefInfoArray aFuncArgDataTypeArray에 대응되는 dtlDataTypeDefInfo정보
 * @param[out] aFuncResultDataType         function 수행시 return type
 * @param[out] aFuncResultDataTypeDefInfo  aFuncResultDataType에 대응하는 dtlDataTypeDefInfo정보
 * @param[out] aFuncPtrIdx                 함수 수행을 위한 function pointer 얻기 위한 식별 정보
 * @param[in]  aVectorFunc                 Function Vector
 * @param[in]  aVectorArg                  Vector Argument 
 * @param[out] aErrorStack                 에러 스택
 */
stlStatus dtlGetFuncInfoToDateTimeFormat( stlUInt16               aDataTypeArrayCount,
                                          stlBool               * aIsConstantData,
                                          dtlDataType           * aDataTypeArray,
                                          dtlDataTypeDefInfo    * aDataTypeDefInfoArray,
                                          stlUInt16               aFuncArgDataTypeArrayCount,
                                          dtlDataType           * aFuncArgDataTypeArray,
                                          dtlDataTypeDefInfo    * aFuncArgDataTypeDefInfoArray,
                                          dtlDataType           * aFuncResultDataType,
                                          dtlDataTypeDefInfo    * aFuncResultDataTypeDefInfo,
                                          stlUInt32             * aFuncPtrIdx,
                                          dtlFuncVector         * aVectorFunc,
                                          void                  * aVectorArg,
                                          stlErrorStack         * aErrorStack )
{
    DTL_PARAM_VALIDATE( ( (aDataTypeArrayCount == 1) ||
                          (aDataTypeArrayCount == 2) ),
                        aErrorStack );
    DTL_PARAM_VALIDATE( ( (aFuncArgDataTypeArrayCount == 1) ||
                          ( aFuncArgDataTypeArrayCount == 2) ),
                        aErrorStack );

    STL_TRY( dtlGetFuncInfoCommon( &dtlFuncToDateTimeFormatInfo,
                                   dtlFuncToDateTimeFormatList,
                                   aDataTypeArrayCount,
                                   aIsConstantData,
                                   aDataTypeArray,
                                   aDataTypeDefInfoArray,
                                   aFuncArgDataTypeArrayCount,
                                   aFuncArgDataTypeArray,
                                   aFuncArgDataTypeDefInfoArray,
                                   aFuncResultDataType,
                                   aFuncResultDataTypeDefInfo,
                                   aFuncPtrIdx,
                                   aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief To_datetime format function pointer 얻기
 * @param[in]  aFuncPtrIdx       function pointer 얻기 위한 식별자
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus dtlGetFuncPtrToDateTimeFormat( stlUInt32             aFuncPtrIdx,
                                         dtlBuiltInFuncPtr   * aFuncPtr,
                                         stlErrorStack       * aErrorStack )
{
    /**
     * output 설정
     */
    
    *aFuncPtr = dtlFuncToDateTimeFormatList[aFuncPtrIdx].mFuncPtr;    
    
    return STL_SUCCESS;
}


stlStatus dtlToDateTimeFormat( stlUInt16        aInputArgumentCnt,
                               dtlValueEntry  * aInputArgument,
                               void           * aResultValue,
                               void           * aInfo,
                               dtlFuncVector  * aVectorFunc,
                               void           * aVectorArg,
                               void           * aEnv )
{
    dtlDataType     sType = DTL_TYPE_MAX;
    dtlDataValue  * sValue1;
    dtlDataValue  * sValue2;
    dtlDataValue  * sResultValue;
    stlUInt8      * sNumericDataPtr;    
    stlErrorStack * sErrorStack;
    dtlDateTimeFormatInfo * sToDateTimeFormatInfo = NULL;
    
    DTL_PARAM_VALIDATE( (aInputArgumentCnt == 1) || (aInputArgumentCnt == 2),
                        (stlErrorStack *)aEnv );
    
    sValue1 = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;
    
    if( DTL_NUMERIC_GET_DIGIT_COUNT( sValue1->mLength ) == 1 )
    {
        sNumericDataPtr = DTL_NUMERIC_GET_DIGIT_PTR( DTF_NUMERIC(sValue1) );
        sType = DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sNumericDataPtr );
    }
    else
    {
        STL_DASSERT( STL_FALSE );
    }

    STL_DASSERT( (sType == DTL_TYPE_DATE) ||
                 (sType == DTL_TYPE_TIME) ||
                 (sType == DTL_TYPE_TIMESTAMP) ||
                 (sType == DTL_TYPE_TIME_WITH_TIME_ZONE) ||
                 (sType == DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE) );
    
    if( aInputArgumentCnt == 1 )
    {
        switch( sType )
        {
            case DTL_TYPE_DATE:
                sToDateTimeFormatInfo =
                    aVectorFunc->mGetDateFormatInfoFunc( aVectorArg );
                break;
            case DTL_TYPE_TIMESTAMP:
                sToDateTimeFormatInfo =
                    aVectorFunc->mGetTimestampFormatInfoFunc( aVectorArg );
                break;
            case DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE:
                sToDateTimeFormatInfo =
                    aVectorFunc->mGetTimestampWithTimeZoneFormatInfoFunc( aVectorArg );
                break;
            case DTL_TYPE_TIME:
                sToDateTimeFormatInfo =
                    aVectorFunc->mGetTimeFormatInfoFunc( aVectorArg );
                break;
            case DTL_TYPE_TIME_WITH_TIME_ZONE:
                sToDateTimeFormatInfo =
                    aVectorFunc->mGetTimeWithTimeZoneFormatInfoFunc( aVectorArg );
                break;
            default:
                STL_DASSERT( STL_FALSE );
                break;
        }
        
        stlMemcpy( sResultValue->mValue,
                   sToDateTimeFormatInfo,
                   sToDateTimeFormatInfo->mSize );
    }
    else
    {
        sValue2 = aInputArgument[1].mValue.mDataValue;
        
        STL_TRY( dtlGetDateTimeFormatInfoForToDateTime(
                     sType,
                     (stlChar *)(sValue2->mValue),
                     sValue2->mLength,
                     STL_FALSE, /* aIsSaveToCharMaxResultLen */
                     sResultValue->mValue,
                     sResultValue->mBufferSize,
                     aVectorFunc,
                     aVectorArg,
                     sErrorStack )
                 == STL_SUCCESS );

        sToDateTimeFormatInfo = (dtlDateTimeFormatInfo *)(sResultValue->mValue);
    }
    
    sResultValue->mLength = sToDateTimeFormatInfo->mSize;

    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief to_datefime 함수에서 format string 으로 formatNode (format info)를 구성한다.
 * @param[in]   aDataType                        dtlDataType
 * @param[in]   aFormatString                    format string
 * @param[in]   aFormatStringLen                 format string length
 * @param[in]   aIsSaveToCharMaxResultLen        aToDateTimeFormatInfoBuffer 의 mToCharMaxResultLen의 유지정보.
 * @param[in,out]  aToDateTimeFormatInfoBuffer   to_datetime format info를 위한 공간
 * @param[in]   aToDateTimeFormatInfoBufferSize  to_datetime format info를 위한 공간 사이즈
 * @param[in]   aVectorFunc                      aVectorArg
 * @param[in]   aVectorArg                       aVectorArg
 * @param[in]   aErrorStack                      에러 스택
 */
stlStatus dtlGetDateTimeFormatInfoForToDateTime( dtlDataType      aDataType,
                                                 stlChar        * aFormatString,
                                                 stlInt64         aFormatStringLen,
                                                 stlBool          aIsSaveToCharMaxResultLen,
                                                 void           * aToDateTimeFormatInfoBuffer,
                                                 stlInt32         aToDateTimeFormatInfoBufferSize,
                                                 dtlFuncVector  * aVectorFunc,
                                                 void           * aVectorArg,
                                                 stlErrorStack  * aErrorStack )
{
    stlInt8                             sPosition = 0;
    stlInt8                             sLen      = 0;
    dtlDateTimeFormatInfo             * sToDateTimeFormatInfo = NULL;
    dtlDateTimeFormatStrInfo          * sFormatStrInfo        = NULL;
    const dtlDateTimeFormatKeyWord    * sKeyWord              = NULL;
    stlChar                           * sFormatString;
    stlChar                           * sFormatStringEnd;
    dtlDateTimeFormatNode             * sFormatNode;
    dtlToDateTimeFormatElementSetInfo * sFormatElementSetInfo;
    dtfDateSetInfo            sDateSetInfo;
    stlBool                   sFormatElementCheck[DTF_DATETIME_FMT_MAX];
    dtlAllocInfo              sAllocInfo;
    stlBool                   sSpecifiedSignedYear = STL_FALSE;
    stlUInt16                 sToCharMaxResultLen = 0;

    /**
     * format string 길이 검사 & 초기화 
     */
    
    /* format string의 길이 검사 */
    STL_TRY_THROW( aFormatStringLen <= DTL_DATETIME_FORMAT_MAX_SIZE,
                   ERROR_DATE_FORMAT_LENGTH );


    /*
     * aToDateTimeFormatInfoBuffer 초기화
     */
    
    if( aIsSaveToCharMaxResultLen == STL_FALSE )
    {
        stlMemset( aToDateTimeFormatInfoBuffer, 0x00, aToDateTimeFormatInfoBufferSize );        
    }
    else
    {
        /*
         * ((dtlDateTimeFormatInfo *)(aToDateTimeFormatInfoBuffer))->mToCharMaxResultLen 값은 유지한다.
         * session format info 정보구축시, to_char에서 구축해 놓은 정보를 유지하기 위해서. 
         */
        sToCharMaxResultLen = ((dtlDateTimeFormatInfo *)(aToDateTimeFormatInfoBuffer))->mToCharMaxResultLen;
        stlMemset( aToDateTimeFormatInfoBuffer, 0x00, aToDateTimeFormatInfoBufferSize );
        ((dtlDateTimeFormatInfo *)(aToDateTimeFormatInfoBuffer))->mToCharMaxResultLen = sToCharMaxResultLen;
    }
    
    /* 지역변수 sAllocInfo 초기화 */
    DTL_ALLOC_INFO_INIT( &sAllocInfo,
                         aToDateTimeFormatInfoBuffer,
                         aToDateTimeFormatInfoBufferSize );
    
    /**
     *  dtlDateTimeFormatInfo 공간 확보
     */
    
    STL_TRY_THROW( dtlAllocMem( &sAllocInfo,
                                STL_SIZEOF( dtlDateTimeFormatInfo ),
                                (void **) &sToDateTimeFormatInfo )
                   == STL_SUCCESS,
                   ERROR_NOT_ENOUGH_BUFFER );
    
    /**
     *  dtlFormatStrInfo 공간 확보
     */
    
    STL_TRY_THROW( dtlAllocMem( &sAllocInfo,
                                STL_SIZEOF( dtlDateTimeFormatStrInfo ) ,
                                (void **) &(sToDateTimeFormatInfo->mFormatStrInfo) )
                   == STL_SUCCESS,
                   ERROR_NOT_ENOUGH_BUFFER);

    /**
     *  dtlFormatStrInfo 멤버 공간 확보
     */
    
    sFormatStrInfo = sToDateTimeFormatInfo->mFormatStrInfo;

    STL_TRY_THROW( dtlAllocMem( &sAllocInfo,
                                aFormatStringLen + 1,
                                (void **) &(sFormatStrInfo->mStr) )
                   == STL_SUCCESS,
                   ERROR_NOT_ENOUGH_BUFFER );
    
    /* format string 복사 */
    stlMemcpy( sFormatStrInfo->mStr,
               aFormatString,
               aFormatStringLen );
    sFormatStrInfo->mStr[aFormatStringLen] = '\0';
    /* '\0'을 제외한 mStr length */
    sFormatStrInfo->mStrLen = aFormatStringLen;

    sFormatString = sFormatStrInfo->mStr;
    sFormatStringEnd = sFormatString + aFormatStringLen;
    sPosition = 0;

    sFormatElementSetInfo = &(sToDateTimeFormatInfo->mFormatElementSetInfo);
    DTF_INIT_TO_DATETIME_FORMAT_ELEMENT_SET_INFO( sFormatElementSetInfo );

    DTF_INIT_DATE_SET_INFO(sDateSetInfo);
    
    /* 지역변수 sFormatElementCheck 초기화 */
    stlMemset( sFormatElementCheck, 0x00, STL_SIZEOF( sFormatElementCheck ) );
    
    /**
     * format key word 를 찾아 정보를 구축한다.
     */

    STL_TRY_THROW( dtlAllocMem( &sAllocInfo,
                                STL_SIZEOF(dtlDateTimeFormatNode),
                                (void **) &(sFormatNode) )
                   == STL_SUCCESS,
                   ERROR_NOT_ENOUGH_BUFFER );
    
    sFormatStrInfo->mFormat = sFormatNode;
    
    while( sFormatString < sFormatStringEnd )
    {
        sKeyWord = dtfGetDateTimeFormatKeyWord( sFormatString );
        
        if( sKeyWord != NULL )
        {
            /* TO_DATETIME FUNCTION 에 사용할 수 있는 element 인지 검사. */ 
            STL_TRY_THROW( sKeyWord->mUsableToDateTimeFunc == STL_TRUE,
                           ERROR_CANNOT_APPEAR_IN_DATE_INPUT_FORMAT );
            
            switch( sKeyWord->mTypeMode )
            {
                case DTL_FORMAT_TYPE_DATE:
                    STL_TRY_THROW( (aDataType == DTL_TYPE_DATE) ||
                                   (aDataType == DTL_TYPE_TIMESTAMP) ||
                                   (aDataType == DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE ),
                                   ERROR_DATE_FORMAT_STR );
                    break;
                case DTL_FORMAT_TYPE_TIME:
                    STL_TRY_THROW( (aDataType == DTL_TYPE_DATE) ||
                                   (aDataType == DTL_TYPE_TIME) ||
                                   (aDataType == DTL_TYPE_TIME_WITH_TIME_ZONE) ||
                                   (aDataType == DTL_TYPE_TIMESTAMP) ||
                                   (aDataType == DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE),
                                   ERROR_DATE_FORMAT_STR );
                    break;
                case DTL_FORMAT_TYPE_WITH_TIME_ZONE:
                    STL_TRY_THROW( (aDataType == DTL_TYPE_TIME_WITH_TIME_ZONE) ||
                                   (aDataType == DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE),
                                   ERROR_DATE_FORMAT_STR );
                    break;
                default:
                    STL_DASSERT( STL_FALSE );
                    break;
            }

            sFormatNode->mType   = DTF_FORMAT_NODE_TYPE_KEYWORD;
            sFormatNode->mKey    = sKeyWord;
            sFormatNode->mPos    = sPosition;
            sFormatNode->mPosLen = sKeyWord->mLen;

            sPosition += sFormatNode->mPosLen;
            sFormatString += sFormatNode->mPosLen;

            STL_TRY_THROW( sFormatElementCheck[sFormatNode->mKey->mId] == STL_FALSE,
                           ERROR_DATE_FORMAT_APPEAR_TWICE );

            switch( sFormatNode->mKey->mId )
            {
                case DTF_DATETIME_FMT_YYYY:
                case DTF_DATETIME_FMT_Y_YYY:                    
                    {
                        /*
                         * yyyy
                         */                          
                        
                        STL_TRY_THROW( sFormatElementSetInfo->mYear == STL_FALSE,
                                       ERROR_DATE_FORMAT_YEAR_SPECIFIED_ONCE );
                        
                        sFormatElementSetInfo->mYear = STL_TRUE;
                        sDateSetInfo.mYear = STL_TRUE;
                    }
                    break;
                case DTF_DATETIME_FMT_YYY:
                case DTF_DATETIME_FMT_YY:
                case DTF_DATETIME_FMT_Y:
                    {
                        STL_TRY_THROW( sFormatElementSetInfo->mYear == STL_FALSE,
                                       ERROR_DATE_FORMAT_YEAR_SPECIFIED_ONCE );
                        
                        sFormatElementSetInfo->mYear = STL_TRUE;
                    }
                    break;
                case DTF_DATETIME_FMT_RRRR:
                case DTF_DATETIME_FMT_RR:
                    {
                        STL_TRY_THROW( sFormatElementSetInfo->mYear == STL_FALSE,
                                       ERROR_DATE_FORMAT_YEAR_SPECIFIED_ONCE );
                        
                        sFormatElementSetInfo->mYear = STL_TRUE;                        
                    }                    
                    break;
                case DTF_DATETIME_FMT_SYYYY:
                    {
                        STL_TRY_THROW( sFormatElementSetInfo->mYear == STL_FALSE,
                                       ERROR_DATE_FORMAT_YEAR_SPECIFIED_ONCE );

                        STL_TRY_THROW( sFormatElementSetInfo->mBc == STL_FALSE,
                                       ERROR_SIGNED_YEAR_PRECLUDES_USE_OF_BC_AD );

                        sSpecifiedSignedYear = STL_TRUE;
                        
                        sFormatElementSetInfo->mYear = STL_TRUE;
                        sDateSetInfo.mYear = STL_TRUE;
                    }
                    break;
                case DTF_DATETIME_FMT_MM:                    
                case DTF_DATETIME_FMT_MONTH:
                case DTF_DATETIME_FMT_Month:
                case DTF_DATETIME_FMT_month:
                case DTF_DATETIME_FMT_MON:
                case DTF_DATETIME_FMT_Mon:
                case DTF_DATETIME_FMT_mon:
                case DTF_DATETIME_FMT_RM:
                case DTF_DATETIME_FMT_Rm:
                case DTF_DATETIME_FMT_rm:
                    {
                        /*
                         * mm
                         */ 
                        STL_TRY_THROW( sFormatElementSetInfo->mMonth == STL_FALSE,
                                       ERROR_DATE_FORMAT_MONTH_SPECIFIED_ONCE );
                        
                        sFormatElementSetInfo->mMonth = STL_TRUE;
                        sDateSetInfo.mMonth = STL_TRUE;
                    }
                    break;
                case DTF_DATETIME_FMT_DD:
                    {
                        /*
                         * dd
                         */                          
                        /* STL_TRY_THROW( sFormatElementSetInfo->mDay == STL_FALSE, */
                        /*                ERROR_DATE_FORMAT_DAY_SPECIFIED_ONCE ); */

                        sFormatElementSetInfo->mDay = STL_TRUE;
                        sDateSetInfo.mDay = STL_TRUE;
                    }
                    break;
                case DTF_DATETIME_FMT_HH:
                case DTF_DATETIME_FMT_HH12:
                case DTF_DATETIME_FMT_HH24:
                    {
                        /*
                         * hh
                         */ 
                        STL_TRY_THROW( sFormatElementSetInfo->mHour == STL_FALSE,
                                       ERROR_DATE_FORMAT_HOUR_SPECIFIED_ONCE );

                        if( sFormatNode->mKey->mId == DTF_DATETIME_FMT_HH24 )
                        {
                            STL_TRY_THROW( sFormatElementSetInfo->mAmPm == STL_FALSE,
                                           ERROR_HH24_PRECLUDES_USE_OF_MERIDIAN_INDICATOR );
                        }
                        else
                        {
                            /* Do Nothing */
                        }

                        sFormatElementSetInfo->mHour = STL_TRUE;
                    }
                    break;
                case DTF_DATETIME_FMT_MI:
                    {
                        /*
                         * mi
                         */ 
                        sFormatElementSetInfo->mMinute = STL_TRUE;
                    }
                    break;
                case DTF_DATETIME_FMT_SS:
                    {
                        /*
                         * ss
                         */                          
                        sFormatElementSetInfo->mSecond = STL_TRUE;
                    }
                    break;
                case DTF_DATETIME_FMT_FF:
                case DTF_DATETIME_FMT_FF6:                
                case DTF_DATETIME_FMT_FF1:                
                case DTF_DATETIME_FMT_FF2:
                case DTF_DATETIME_FMT_FF3:
                case DTF_DATETIME_FMT_FF4:
                case DTF_DATETIME_FMT_FF5:
                    {
                        /*
                         * ff
                         */

                        /* date type에는 fractional second가 없다. */
                        STL_TRY_THROW( (aDataType != DTL_TYPE_DATE),
                                       ERROR_DATE_FORMAT_STR );                        
                        
                        STL_TRY_THROW( sFormatElementSetInfo->mFractionalSecond == STL_FALSE,
                                       ERROR_DATE_FORMAT_FRACTIONAL_SECOND_SPECIFIED_ONCE );
                        
                        sFormatElementSetInfo->mFractionalSecond = STL_TRUE;
                    }
                    break;
                case DTF_DATETIME_FMT_A_M:
                case DTF_DATETIME_FMT_a_m:                    
                case DTF_DATETIME_FMT_P_M:
                case DTF_DATETIME_FMT_p_m:
                    {
                        STL_TRY_THROW( (sFormatElementCheck[DTF_DATETIME_FMT_A_M] == STL_FALSE) &&
                                       (sFormatElementCheck[DTF_DATETIME_FMT_a_m] == STL_FALSE) &&
                                       (sFormatElementCheck[DTF_DATETIME_FMT_P_M] == STL_FALSE) &&
                                       (sFormatElementCheck[DTF_DATETIME_FMT_p_m] == STL_FALSE),
                                       ERROR_DATE_FORMAT_APPEAR_TWICE );

                        STL_TRY_THROW( (sFormatElementSetInfo->mAmPm == STL_FALSE) &&
                                       ((sFormatElementCheck[DTF_DATETIME_FMT_AM] == STL_FALSE) &&
                                        (sFormatElementCheck[DTF_DATETIME_FMT_Am] == STL_FALSE) &&
                                        (sFormatElementCheck[DTF_DATETIME_FMT_am] == STL_FALSE) &&
                                        (sFormatElementCheck[DTF_DATETIME_FMT_PM] == STL_FALSE) &&
                                        (sFormatElementCheck[DTF_DATETIME_FMT_Pm] == STL_FALSE) &&
                                        (sFormatElementCheck[DTF_DATETIME_FMT_pm] == STL_FALSE)),
                                       ERROR_DATE_FORMAT_AM_PM_CONFLICT_A_M_P_M );
                    }                    
                case DTF_DATETIME_FMT_AM:
                case DTF_DATETIME_FMT_PM:
                case DTF_DATETIME_FMT_Am:
                case DTF_DATETIME_FMT_Pm:
                case DTF_DATETIME_FMT_am:
                case DTF_DATETIME_FMT_pm:
                    {
                        STL_TRY_THROW( (sFormatElementCheck[DTF_DATETIME_FMT_AM] == STL_FALSE) &&
                                       (sFormatElementCheck[DTF_DATETIME_FMT_Am] == STL_FALSE) &&
                                       (sFormatElementCheck[DTF_DATETIME_FMT_am] == STL_FALSE) &&
                                       (sFormatElementCheck[DTF_DATETIME_FMT_PM] == STL_FALSE) &&
                                       (sFormatElementCheck[DTF_DATETIME_FMT_Pm] == STL_FALSE) &&
                                       (sFormatElementCheck[DTF_DATETIME_FMT_pm] == STL_FALSE),
                                       ERROR_DATE_FORMAT_APPEAR_TWICE );

                        STL_TRY_THROW( (sFormatElementSetInfo->mAmPm == STL_FALSE) &&
                                       ((sFormatElementCheck[DTF_DATETIME_FMT_A_M] == STL_FALSE) &&
                                        (sFormatElementCheck[DTF_DATETIME_FMT_a_m] == STL_FALSE) &&
                                        (sFormatElementCheck[DTF_DATETIME_FMT_P_M] == STL_FALSE) &&
                                        (sFormatElementCheck[DTF_DATETIME_FMT_p_m] == STL_FALSE)),
                                       ERROR_DATE_FORMAT_AM_PM_CONFLICT_A_M_P_M );
                        
                        STL_TRY_THROW( sFormatElementCheck[DTF_DATETIME_FMT_HH24] == STL_FALSE,
                                       ERROR_HH24_PRECLUDES_USE_OF_MERIDIAN_INDICATOR );
                        
                        sFormatElementSetInfo->mAmPm = STL_TRUE;
                    }
                    break;
                case DTF_DATETIME_FMT_SSSSS:
                    {
                        /*
                         * sssss
                         */ 
                        sFormatElementSetInfo->mSssss = STL_TRUE;
                    }
                    break;
                case DTF_DATETIME_FMT_TZH:
                    {
                        /*
                         * tzh
                         */                          
                        sFormatElementSetInfo->mTimeZoneHour = STL_TRUE;
                    }
                    break;
                case DTF_DATETIME_FMT_TZM:
                    {
                        /*
                         * tzm
                         */ 
                        sFormatElementSetInfo->mTimeZoneMinute = STL_TRUE;
                    }
                    break;
                case DTF_DATETIME_FMT_J:
                    {
                        /*
                         * yyyy-mm-dd
                         */ 
                        STL_TRY_THROW( sFormatElementSetInfo->mYearDay == STL_FALSE,
                                       ERROR_JULIAN_DATE_PRECLUDES_USE_OF_DAY_OF_YEAR );
                        
                        sFormatElementSetInfo->mJulianDay = STL_TRUE;
                        
                        sDateSetInfo.mYear = STL_TRUE;
                        sDateSetInfo.mMonth = STL_TRUE;
                        sDateSetInfo.mDay = STL_TRUE;
                    }
                    break;
                case DTF_DATETIME_FMT_DDD:
                    {
                        /*
                         * mm-dd
                         */ 
                        
                        STL_TRY_THROW( sFormatElementSetInfo->mJulianDay == STL_FALSE,
                                       ERROR_JULIAN_DATE_PRECLUDES_USE_OF_DAY_OF_YEAR );
                        
                        sFormatElementSetInfo->mYearDay = STL_TRUE;
                        
                        sDateSetInfo.mMonth = STL_TRUE;
                        sDateSetInfo.mDay   = STL_TRUE;
                    }
                    break;
                case DTF_DATETIME_FMT_D:
                case DTF_DATETIME_FMT_DAY:
                case DTF_DATETIME_FMT_Day:
                case DTF_DATETIME_FMT_day:
                case DTF_DATETIME_FMT_DY:
                case DTF_DATETIME_FMT_Dy:
                case DTF_DATETIME_FMT_dy:    
                    {
                        STL_TRY_THROW( sFormatElementSetInfo->mWeekDay == STL_FALSE,
                                       ERROR_DATE_FORMAT_DAY_OF_WEEK_SPECIFIED_ONCE ); 
                        
                        sFormatElementSetInfo->mWeekDay = STL_TRUE;
                    }
                    break;
                case DTF_DATETIME_FMT_A_D:
                case DTF_DATETIME_FMT_a_d:
                case DTF_DATETIME_FMT_B_C:
                case DTF_DATETIME_FMT_b_c:
                    {
                        STL_TRY_THROW( (sFormatElementCheck[DTF_DATETIME_FMT_A_D] == STL_FALSE) &&
                                       (sFormatElementCheck[DTF_DATETIME_FMT_a_d] == STL_FALSE) &&
                                       (sFormatElementCheck[DTF_DATETIME_FMT_B_C] == STL_FALSE) &&
                                       (sFormatElementCheck[DTF_DATETIME_FMT_b_c] == STL_FALSE),
                                       ERROR_DATE_FORMAT_APPEAR_TWICE );
                        
                        STL_TRY_THROW( (sFormatElementSetInfo->mBc == STL_FALSE) &&
                                       ((sFormatElementCheck[DTF_DATETIME_FMT_AD] == STL_FALSE) &&
                                        (sFormatElementCheck[DTF_DATETIME_FMT_ad] == STL_FALSE) &&
                                        (sFormatElementCheck[DTF_DATETIME_FMT_BC] == STL_FALSE) &&
                                        (sFormatElementCheck[DTF_DATETIME_FMT_bc] == STL_FALSE)),
                                       ERROR_DATE_FORMAT_BC_AD_CONFLICT_B_C_A_D );
                    }
                case DTF_DATETIME_FMT_AD:
                case DTF_DATETIME_FMT_ad:
                case DTF_DATETIME_FMT_BC:
                case DTF_DATETIME_FMT_bc:
                    {
                        STL_TRY_THROW( (sFormatElementCheck[DTF_DATETIME_FMT_AD] == STL_FALSE) &&
                                       (sFormatElementCheck[DTF_DATETIME_FMT_ad] == STL_FALSE) &&
                                       (sFormatElementCheck[DTF_DATETIME_FMT_BC] == STL_FALSE) &&
                                       (sFormatElementCheck[DTF_DATETIME_FMT_bc] == STL_FALSE),
                                       ERROR_DATE_FORMAT_APPEAR_TWICE );

                        STL_TRY_THROW( (sFormatElementSetInfo->mBc == STL_FALSE) &&
                                       ((sFormatElementCheck[DTF_DATETIME_FMT_A_D] == STL_FALSE) &&
                                        (sFormatElementCheck[DTF_DATETIME_FMT_a_d] == STL_FALSE) &&
                                        (sFormatElementCheck[DTF_DATETIME_FMT_B_C] == STL_FALSE) &&
                                        (sFormatElementCheck[DTF_DATETIME_FMT_b_c] == STL_FALSE)),
                                       ERROR_DATE_FORMAT_BC_AD_CONFLICT_B_C_A_D );

                        STL_TRY_THROW( sSpecifiedSignedYear == STL_FALSE,
                                       ERROR_SIGNED_YEAR_PRECLUDES_USE_OF_BC_AD );
                        
                        sFormatElementSetInfo->mBc = STL_TRUE;                        
                    }
                    break;
                default:
                    STL_DASSERT( STL_FALSE );
                    break;
            }

            sFormatElementCheck[sFormatNode->mKey->mId] = STL_TRUE;
        }
        else
        {
            if( DTF_DATETIME_FORMAT_SPECIAL_CHAR_INDEX[(stlUInt8)*(sFormatString)] == 0 )
            {
                sFormatNode->mType   = DTF_FORMAT_NODE_TYPE_CHAR;
                sFormatNode->mKey    = NULL;
                sFormatNode->mPos    = sPosition;
                sFormatNode->mPosLen = 1;
                
                sPosition += sFormatNode->mPosLen;
                sFormatString += sFormatNode->mPosLen;
            }
            else if( *sFormatString == '"' )
            {
                sLen = 0;
                sPosition++;
                sFormatString++;
                
                while( sFormatString < sFormatStringEnd )
                {
                    if( *sFormatString == '"' )
                    {
                        break;
                    }
                    else
                    {
                        sLen++;
                    }

                    sFormatString++;
                }
                
                if( *sFormatString == '"' )
                {
                    sFormatNode->mType   = DTF_FORMAT_NODE_TYPE_CHAR;
                    sFormatNode->mKey    = NULL;
                    sFormatNode->mPos    = sPosition;
                    sFormatNode->mPosLen = sLen;
                    
                    sPosition += sFormatNode->mPosLen + 1;
                    
                    sFormatString++;
                }
                else
                {
                    STL_THROW( ERROR_DATE_FORMAT_STR );
                }
            }
            else
            {
                STL_THROW( ERROR_DATE_FORMAT_STR );
            }
        }

        STL_TRY_THROW( dtlAllocMem( &sAllocInfo,
                                    STL_SIZEOF(dtlDateTimeFormatNode),
                                    (void **) &(sFormatNode) )
                       == STL_SUCCESS,
                       ERROR_NOT_ENOUGH_BUFFER );
    }
    
    if( (aDataType == DTL_TYPE_DATE) ||
        (aDataType == DTL_TYPE_TIMESTAMP) ||
        (aDataType == DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE) )
    {
        if( (sDateSetInfo.mYear == STL_TRUE) &&
            (sDateSetInfo.mMonth == STL_TRUE) &&
            (sDateSetInfo.mDay == STL_TRUE) )
        {
            sToDateTimeFormatInfo->mNeedCurrentDate = STL_FALSE;
        }
        else
        {
            sToDateTimeFormatInfo->mNeedCurrentDate = STL_TRUE;
        }
    }
    else
    {
        sToDateTimeFormatInfo->mNeedCurrentDate = STL_FALSE;
    }
    
    if( (aDataType == DTL_TYPE_TIME_WITH_TIME_ZONE) ||
        (aDataType == DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE) )
    {
        if( (sFormatElementSetInfo->mTimeZoneHour == STL_TRUE) &&
            (sFormatElementSetInfo->mTimeZoneMinute == STL_TRUE) )
        {
            sToDateTimeFormatInfo->mNeedCurrentTimeZoneOffset = STL_FALSE;
        }
        else
        {
            sToDateTimeFormatInfo->mNeedCurrentTimeZoneOffset = STL_TRUE;            
        }
    }
    else
    {
        sToDateTimeFormatInfo->mNeedCurrentTimeZoneOffset = STL_FALSE;
    }
    
    sFormatNode->mType = DTF_FORMAT_NODE_TYPE_END;

    sToDateTimeFormatInfo->mSize = DTL_ALLOC_INFO_USED_SIZE( &sAllocInfo );
    
    return STL_SUCCESS;

    STL_CATCH( ERROR_DATE_FORMAT_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_DATE_FORMAT_LENGTH,
                      NULL,
                      aErrorStack,
                      DTL_DATETIME_FORMAT_MAX_SIZE );
    }

    STL_CATCH( ERROR_DATE_FORMAT_STR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_DATE_FORMAT_NOT_RECOGNIZED,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( ERROR_DATE_FORMAT_APPEAR_TWICE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_FORMAT_CODE_APPEARS_TWICE,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( ERROR_CANNOT_APPEAR_IN_DATE_INPUT_FORMAT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_CANNOT_APPEAR_IN_DATE_INPUT_FORMAT,
                      NULL,
                      aErrorStack );
    }
    
    STL_CATCH( ERROR_DATE_FORMAT_YEAR_SPECIFIED_ONCE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_DATE_FORMAT_SPECIFIED_ONCE,
                      NULL,
                      aErrorStack,
                      DTF_DATE_YEAR_STRING );
    }

    STL_CATCH( ERROR_DATE_FORMAT_MONTH_SPECIFIED_ONCE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_DATE_FORMAT_SPECIFIED_ONCE,
                      NULL,
                      aErrorStack,
                      DTF_DATE_MONTH_STRING );
    }  

    STL_CATCH( ERROR_DATE_FORMAT_DAY_OF_WEEK_SPECIFIED_ONCE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_DATE_FORMAT_SPECIFIED_ONCE,
                      NULL,
                      aErrorStack,
                      DTF_DATE_DAY_OF_WEEK_STRING );
    }  
    
    STL_CATCH( ERROR_JULIAN_DATE_PRECLUDES_USE_OF_DAY_OF_YEAR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_DATE_FORMAT_JULIAN_DATE_PRECLUDES_USE_OF_DAY_OF_YEAR,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( ERROR_DATE_FORMAT_BC_AD_CONFLICT_B_C_A_D )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_DATE_FORMAT_CODE_CONFLICT_WITH_USE_OF,
                      NULL,
                      aErrorStack,
                      DTF_DATE_BC_AD_STRING,
                      DTF_DATE_LONG_BC_AD_STRING );
    }

    STL_CATCH( ERROR_NOT_ENOUGH_BUFFER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_NOT_ENOUGH_FORMAT_INFO_BUFFER,
                      NULL,
                      aErrorStack );
    }
    
    STL_CATCH( ERROR_DATE_FORMAT_HOUR_SPECIFIED_ONCE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_DATE_FORMAT_SPECIFIED_ONCE,
                      NULL,
                      aErrorStack,
                      DTF_DATE_HOUR_STRING );
    }  

    STL_CATCH( ERROR_HH24_PRECLUDES_USE_OF_MERIDIAN_INDICATOR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_DATE_FORMAT_HH24_PRECLUDES_USE_OF_MERIDIAN_INDICATOR,
                      NULL,
                      aErrorStack );
    }
    
    STL_CATCH( ERROR_DATE_FORMAT_AM_PM_CONFLICT_A_M_P_M )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_DATE_FORMAT_CODE_CONFLICT_WITH_USE_OF,
                      NULL,
                      aErrorStack,
                      DTF_DATE_AM_PM_STRING,
                      DTF_DATE_LONG_AM_PM_STRING );
    }

    STL_CATCH( ERROR_DATE_FORMAT_FRACTIONAL_SECOND_SPECIFIED_ONCE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_DATE_FORMAT_SPECIFIED_ONCE,
                      NULL,
                      aErrorStack,
                      DTF_DATE_FRACTIONAL_SECOND_STRING );
    }

    STL_CATCH( ERROR_SIGNED_YEAR_PRECLUDES_USE_OF_BC_AD )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_DATE_FORMAT_SIGNED_YEAR_PRECLUDES_USE_OF_BC_AD,
                      NULL,
                      aErrorStack );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}



/*******************************************************************************
 * TO_NUMBER_FORMAT        FUNCTION
 * TO_NATIVE_REAL_FORMAT   FUNCTION
 * TO_NATIVE_DOUBLE_FORMAT FUNCTION
 *******************************************************************************/

/*
 * 대표 함수의 정보를 정의한다.
 * mValidArgList는 함수를 결정하는데 사용되는 arg index의 list이다.
 * 함수를 결정하는데 사용되지 않는 arg는 무조건 함수의 원형 type으로
 * 리턴한다.
 */
const dtlFuncInfo dtlFuncToNumberFormatInfo =
{
    { STL_TRUE }      /* 함수를 결정하는데 판단이 되는 argument index list */
};

/*
 * 각 함수별 arg type, result type을 정의한다.
 * 주의: 빠르게 처리 가능한 함수를 먼저 정의해야 한다.
 */
const dtlFuncArgInfo dtlFuncToNumberFormatList[] =
{
    { dtlToNumberFormat,
      { DTL_TYPE_VARCHAR },
      DTL_TYPE_VARBINARY
    },
    { dtlToNumberFormat,
      { DTL_TYPE_LONGVARCHAR },
      DTL_TYPE_VARBINARY
    },
    
    { NULL,
      { DTL_TYPE_NA, },
      DTL_TYPE_NA
    }
};    


/**
 * @brief to_number to_native_real to_native_double 의 format function에 대한 정보 얻기
 * @param[in]  aDataTypeArrayCount         aDataTypeArrary의 갯수
 * @param[in]  aIsConstantData             constant value인지 여부
 * @param[in]  aDataTypeArray              function을 수행할 data type
 * @param[in]  aDataTypeDefInfoArray       aDataTypeArray에 대응되는 dtlDataTypeDefInfo정보 
 * @param[in]  aFuncArgDataTypeArrayCount  function의 input argument count
 * @param[out] aFuncArgDataTypeArray       function 수행시 수행 argument type
 * @param[out] aFuncArgDataTypeDefInfoArray aFuncArgDataTypeArray에 대응되는 dtlDataTypeDefInfo정보
 * @param[out] aFuncResultDataType         function 수행시 return type
 * @param[out] aFuncResultDataTypeDefInfo  aFuncResultDataType에 대응하는 dtlDataTypeDefInfo정보
 * @param[out] aFuncPtrIdx                 함수 수행을 위한 function pointer 얻기 위한 식별 정보
 * @param[in]  aVectorFunc                 Function Vector
 * @param[in]  aVectorArg                  Vector Argument 
 * @param[out] aErrorStack                 에러 스택
 */
stlStatus dtlGetFuncInfoToNumberFormat( stlUInt16               aDataTypeArrayCount,
                                        stlBool               * aIsConstantData,
                                        dtlDataType           * aDataTypeArray,
                                        dtlDataTypeDefInfo    * aDataTypeDefInfoArray,
                                        stlUInt16               aFuncArgDataTypeArrayCount,
                                        dtlDataType           * aFuncArgDataTypeArray,
                                        dtlDataTypeDefInfo    * aFuncArgDataTypeDefInfoArray,
                                        dtlDataType           * aFuncResultDataType,
                                        dtlDataTypeDefInfo    * aFuncResultDataTypeDefInfo,
                                        stlUInt32             * aFuncPtrIdx,
                                        dtlFuncVector         * aVectorFunc,
                                        void                  * aVectorArg,
                                        stlErrorStack         * aErrorStack )
{
    DTL_PARAM_VALIDATE( aDataTypeArrayCount == 1, aErrorStack );
    DTL_PARAM_VALIDATE( aFuncArgDataTypeArrayCount == 1, aErrorStack );

    STL_TRY( dtlGetFuncInfoCommon( &dtlFuncToNumberFormatInfo,
                                   dtlFuncToNumberFormatList,
                                   aDataTypeArrayCount,
                                   aIsConstantData,
                                   aDataTypeArray,
                                   aDataTypeDefInfoArray,
                                   aFuncArgDataTypeArrayCount,
                                   aFuncArgDataTypeArray,
                                   aFuncArgDataTypeDefInfoArray,
                                   aFuncResultDataType,
                                   aFuncResultDataTypeDefInfo,
                                   aFuncPtrIdx,
                                   aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief to_number to_native_real to_native_double format function pointer 얻기
 * @param[in]  aFuncPtrIdx       function pointer 얻기 위한 식별자
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus dtlGetFuncPtrToNumberFormat( stlUInt32             aFuncPtrIdx,
                                       dtlBuiltInFuncPtr   * aFuncPtr,
                                       stlErrorStack       * aErrorStack )
{
    /**
     * output 설정
     */
    
    *aFuncPtr = dtlFuncToNumberFormatList[aFuncPtrIdx].mFuncPtr;    
    
    return STL_SUCCESS;
}


stlStatus dtlToNumberFormat( stlUInt16        aInputArgumentCnt,
                             dtlValueEntry  * aInputArgument,
                             void           * aResultValue,
                             void           * aInfo,
                             dtlFuncVector  * aVectorFunc,
                             void           * aVectorArg,
                             void           * aEnv )
{
    dtlDataValue        * sValue1 = NULL;
    dtlDataValue        * sResultValue = NULL;
    stlErrorStack       * sErrorStack;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv );
    
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;
    
    sValue1 = aInputArgument[0].mValue.mDataValue;

    STL_TRY( dtfGetNumberFormatInfoForToNumber( sValue1->mType,
                                                (stlChar *)(sValue1->mValue),
                                                sValue1->mLength,
                                                sResultValue->mValue,
                                                sResultValue->mBufferSize,
                                                aVectorFunc,
                                                aVectorArg,
                                                sErrorStack )
             == STL_SUCCESS );
    
    sResultValue->mLength = STL_SIZEOF( dtfNumberFormatInfo );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/*******************************************************************************
 * TO_CHAR FORMAT FUNCTION
 *******************************************************************************/

/*
 * 대표 함수의 정보를 정의한다.
 * mValidArgList는 함수를 결정하는데 사용되는 arg index의 list이다.
 * 함수를 결정하는데 사용되지 않는 arg는 무조건 함수의 원형 type으로
 * 리턴한다.
 */
const dtlFuncInfo dtlFuncToCharFormatInfo =
{
    { STL_TRUE }      /* 함수를 결정하는데 판단이 되는 argument index list */
};

/*
 * 각 함수별 arg type, result type을 정의한다.
 * 주의: 빠르게 처리 가능한 함수를 먼저 정의해야 한다.
 */
const dtlFuncArgInfo dtlFuncToCharFormatList[] =
{
    { dtlDateToCharFormat,
      { DTL_TYPE_VARCHAR },
      DTL_TYPE_VARBINARY
    },

    { dtlDateToCharFormat,
      { DTL_TYPE_LONGVARCHAR },
      DTL_TYPE_VARBINARY
    },
    
    { NULL,
      { DTL_TYPE_NA, },
      DTL_TYPE_NA
    }
};    



dtlBuiltInFuncPtr  gDtlToCharFormatFunc[ DTL_TYPE_MAX ] =
{
    NULL,                                 /**< BOOLEAN */
    NULL,                                 /**< NATIVE_SMALLINT */
    NULL,                                 /**< NATIVE_INTEGER */
    NULL,                                 /**< NATIVE_BIGINT */

    dtlRealToCharFormat,                  /**< NATIVE_REAL */
    dtlDoubleToCharFormat,                /**< NATIVE_DOUBLE */

    dtlNumericToCharFormat,               /**< FLOAT */
    dtlNumericToCharFormat,               /**< NUMBER */
    NULL,                                 /**< unsupported feature, DECIMAL */
    
    dtlCharStringToCharFormat,            /**< CHARACTER */
    dtlCharStringToCharFormat,            /**< CHARACTER VARYING */
    dtlCharStringToCharFormat,            /**< CHARACTER LONG VARYING */
    NULL,                                 /**< unsupported feature, CHARACTER LARGE OBJECT */
    
    NULL,                                 /**< BINARY */
    NULL,                                 /**< BINARY VARYING */
    NULL,                                 /**< BINARY LONG VARYING */
    NULL,                                 /**< unsupported feature, BINARY LARGE OBJECT */

    dtlDateToCharFormat,                  /**< DATE */
    dtlTimeToCharFormat,                  /**< TIME WITHOUT TIME ZONE */
    dtlTimestampToCharFormat,             /**< TIMESTAMP WITHOUT TIME ZONE */
    dtlTimeWithTimeZoneToCharFormat,      /**< TIME WITH TIME ZONE */
    dtlTimestampWithTimeZoneToCharFormat, /**< TIMESTAMP WITH TIME ZONE */

    dtlIntervalYearToMonthToCharFormat,   /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
    dtlIntervalDayToSecondToCharFormat,   /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */

    NULL                                  /**< DTL_TYPE_ROWID */
};



/**
 * @brief To_char format function에 대한 정보 얻기
 * @param[in]  aDataTypeArrayCount         aDataTypeArrary의 갯수
 * @param[in]  aIsConstantData             constant value인지 여부
 * @param[in]  aDataTypeArray              function을 수행할 data type
 * @param[in]  aDataTypeDefInfoArray       aDataTypeArray에 대응되는 dtlDataTypeDefInfo정보 
 * @param[in]  aFuncArgDataTypeArrayCount  function의 input argument count
 * @param[out] aFuncArgDataTypeArray       function 수행시 수행 argument type
 * @param[out] aFuncArgDataTypeDefInfoArray aFuncArgDataTypeArray에 대응되는 dtlDataTypeDefInfo정보
 * @param[out] aFuncResultDataType         function 수행시 return type
 * @param[out] aFuncResultDataTypeDefInfo  aFuncResultDataType에 대응하는 dtlDataTypeDefInfo정보
 * @param[out] aFuncPtrIdx                 함수 수행을 위한 function pointer 얻기 위한 식별 정보
 * @param[in]  aVectorFunc                 Function Vector
 * @param[in]  aVectorArg                  Vector Argument
 * @param[out] aErrorStack                 에러 스택
 */
stlStatus dtlGetFuncInfoToCharFormat( stlUInt16               aDataTypeArrayCount,
                                      stlBool               * aIsConstantData,
                                      dtlDataType           * aDataTypeArray,
                                      dtlDataTypeDefInfo    * aDataTypeDefInfoArray,
                                      stlUInt16               aFuncArgDataTypeArrayCount,
                                      dtlDataType           * aFuncArgDataTypeArray,
                                      dtlDataTypeDefInfo    * aFuncArgDataTypeDefInfoArray,
                                      dtlDataType           * aFuncResultDataType,
                                      dtlDataTypeDefInfo    * aFuncResultDataTypeDefInfo,
                                      stlUInt32             * aFuncPtrIdx,
                                      dtlFuncVector         * aVectorFunc,
                                      void                  * aVectorArg,                                     
                                      stlErrorStack         * aErrorStack )
{
    DTL_PARAM_VALIDATE( (aDataTypeArrayCount == 0) || (aDataTypeArrayCount == 1),
                        aErrorStack );
    DTL_PARAM_VALIDATE( (aFuncArgDataTypeArrayCount == 0) ||
                        (aFuncArgDataTypeArrayCount == 1),
                        aErrorStack );
    
    STL_TRY( dtlGetFuncInfoCommon( &dtlFuncToCharFormatInfo,
                                   dtlFuncToCharFormatList,
                                   aDataTypeArrayCount,
                                   aIsConstantData,
                                   aDataTypeArray,
                                   aDataTypeDefInfoArray,
                                   aFuncArgDataTypeArrayCount,
                                   aFuncArgDataTypeArray,
                                   aFuncArgDataTypeDefInfoArray,
                                   aFuncResultDataType,
                                   aFuncResultDataTypeDefInfo,
                                   aFuncPtrIdx,
                                   aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief To_char format function idx 를 얻는다.
 * @param[in]  aDataType      to_char 를 수행할 data type
 * @param[out] aFuncPtrIdx    함수 수행을 위한 function pointer 얻기 위한 식별 정보
 * @param[out] aErrorStack    에러 스택
 */
stlStatus dtlGetFuncPtrIdxToCharFormat( dtlDataType      aDataType,
                                        stlUInt32      * aFuncPtrIdx,
                                        stlErrorStack  * aErrorStack )
{
    STL_DASSERT( gDtlToCharFormatFunc[ aDataType ] != NULL );
    
    *aFuncPtrIdx = aDataType;
    
    return STL_SUCCESS;    
}


/**
 * @brief To_char format function pointer 얻기
 * @param[in]  aFuncPtrIdx       function pointer 얻기 위한 식별자
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus dtlGetFuncPtrToCharFormat( stlUInt32             aFuncPtrIdx,
                                     dtlBuiltInFuncPtr   * aFuncPtr,
                                     stlErrorStack       * aErrorStack )
{
    /**
     * output 설정
     */
    
    *aFuncPtr = gDtlToCharFormatFunc[ aFuncPtrIdx ];
    
    return STL_SUCCESS;
}


stlStatus dtlCharStringToCharFormat( stlUInt16        aInputArgumentCnt,
                                     dtlValueEntry  * aInputArgument,
                                     void           * aResultValue,
                                     void           * aInfo,
                                     dtlFuncVector  * aVectorFunc,
                                     void           * aVectorArg,
                                     void           * aEnv )
{
    /**
     * 1. TO_CHAR( str )
     * 
     * 2. null 처리 용도로 사용됨.
     *   ex) TO_CHAR( NULL, format_str )
     */
    
    dtlDataValue        * sResultValue;
    
    
    DTL_PARAM_VALIDATE( (aInputArgumentCnt == 0) || (aInputArgumentCnt == 1),
                        (stlErrorStack *)aEnv );
    
    sResultValue = (dtlDataValue *)aResultValue;

    /* aToCharFormatInfoBuffer 초기화 */
    DTF_INIT_NUMBER_FORMAT_INFO( (dtfNumberFormatInfo *)(sResultValue->mValue) );
    
    if( aInputArgumentCnt == 0 )
    {
        /* aToCharFormatInfoBuffer 초기화 */
        DTF_INIT_NUMBER_FORMAT_INFO( (dtfNumberFormatInfo *)(sResultValue->mValue) );
    }
    else
    {
        /* to_char( NULL, format_str ) 과 같이 두번째인자가 있었음을 표시만 함. */
        ((dtfNumberFormatInfo *)(sResultValue->mValue))->mStrLen = STL_UINT8_MAX;
    }
    
    sResultValue->mLength = STL_SIZEOF( dtfNumberFormatInfo );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus dtlDateToCharFormat( stlUInt16        aInputArgumentCnt,
                               dtlValueEntry  * aInputArgument,
                               void           * aResultValue,
                               void           * aInfo,
                               dtlFuncVector  * aVectorFunc,
                               void           * aVectorArg,
                               void           * aEnv )
{
    dtlDataValue  * sValue1 = NULL;
    dtlDataValue  * sResultValue;
    stlErrorStack * sErrorStack;
    dtlDateTimeFormatInfo * sToCharFormatInfo = NULL;
    
    DTL_PARAM_VALIDATE( (aInputArgumentCnt == 0) || (aInputArgumentCnt == 1),
                        (stlErrorStack *)aEnv );

    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;
    
    if( aInputArgumentCnt == 0 )
    {
        sToCharFormatInfo =
            aVectorFunc->mGetDateFormatInfoFunc( aVectorArg );

        stlMemcpy( sResultValue->mValue,
                   sToCharFormatInfo,
                   sToCharFormatInfo->mSize );
    }
    else
    {
        sValue1 = aInputArgument[0].mValue.mDataValue;
        
        STL_TRY( dtlGetDateTimeFormatInfoForToChar(
                     DTL_TYPE_DATE,
                     (stlChar *)(sValue1->mValue),
                     sValue1->mLength,
                     sResultValue->mValue,
                     sResultValue->mBufferSize,
                     aVectorFunc,
                     aVectorArg,
                     sErrorStack )
                 == STL_SUCCESS );
        
        sToCharFormatInfo = (dtlDateTimeFormatInfo *)(sResultValue->mValue);
    }
    
    sResultValue->mLength = sToCharFormatInfo->mSize;
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus dtlTimeToCharFormat( stlUInt16        aInputArgumentCnt,
                               dtlValueEntry  * aInputArgument,
                               void           * aResultValue,
                               void           * aInfo,
                               dtlFuncVector  * aVectorFunc,
                               void           * aVectorArg,
                               void           * aEnv )
{
    dtlDataValue  * sValue1 = NULL;
    dtlDataValue  * sResultValue;
    stlErrorStack * sErrorStack;
    dtlDateTimeFormatInfo * sToCharFormatInfo = NULL;
    
    DTL_PARAM_VALIDATE( (aInputArgumentCnt == 0) || (aInputArgumentCnt == 1),
                        (stlErrorStack *)aEnv );

    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;
    
    if( aInputArgumentCnt == 0 )
    {
        sToCharFormatInfo =
            aVectorFunc->mGetTimeFormatInfoFunc( aVectorArg );

        stlMemcpy( sResultValue->mValue,
                   sToCharFormatInfo,
                   sToCharFormatInfo->mSize );
    }
    else
    {
        sValue1 = aInputArgument[0].mValue.mDataValue;
        
        STL_TRY( dtlGetDateTimeFormatInfoForToChar(
                     DTL_TYPE_TIME,
                     (stlChar *)(sValue1->mValue),
                     sValue1->mLength,
                     sResultValue->mValue,
                     sResultValue->mBufferSize,
                     aVectorFunc,
                     aVectorArg,
                     sErrorStack )
                 == STL_SUCCESS );
        
        sToCharFormatInfo = (dtlDateTimeFormatInfo *)(sResultValue->mValue);
    }
    
    sResultValue->mLength = sToCharFormatInfo->mSize;
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus dtlTimeWithTimeZoneToCharFormat( stlUInt16        aInputArgumentCnt,
                                           dtlValueEntry  * aInputArgument,
                                           void           * aResultValue,
                                           void           * aInfo,
                                           dtlFuncVector  * aVectorFunc,
                                           void           * aVectorArg,
                                           void           * aEnv )
{
    dtlDataValue  * sValue1 = NULL;
    dtlDataValue  * sResultValue;
    stlErrorStack * sErrorStack;
    dtlDateTimeFormatInfo * sToCharFormatInfo = NULL;
    
    DTL_PARAM_VALIDATE( (aInputArgumentCnt == 0) || (aInputArgumentCnt == 1),
                        (stlErrorStack *)aEnv );

    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;
    
    if( aInputArgumentCnt == 0 )
    {
        sToCharFormatInfo =
            aVectorFunc->mGetTimeWithTimeZoneFormatInfoFunc( aVectorArg );

        stlMemcpy( sResultValue->mValue,
                   sToCharFormatInfo,
                   sToCharFormatInfo->mSize );
    }
    else
    {
        sValue1 = aInputArgument[0].mValue.mDataValue;
        
        STL_TRY( dtlGetDateTimeFormatInfoForToChar(
                     DTL_TYPE_TIME_WITH_TIME_ZONE,
                     (stlChar *)(sValue1->mValue),
                     sValue1->mLength,
                     sResultValue->mValue,
                     sResultValue->mBufferSize,
                     aVectorFunc,
                     aVectorArg,
                     sErrorStack )
                 == STL_SUCCESS );
        
        sToCharFormatInfo = (dtlDateTimeFormatInfo *)(sResultValue->mValue);
    }
    
    sResultValue->mLength = sToCharFormatInfo->mSize;
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus dtlTimestampToCharFormat( stlUInt16        aInputArgumentCnt,
                                    dtlValueEntry  * aInputArgument,
                                    void           * aResultValue,
                                    void           * aInfo,
                                    dtlFuncVector  * aVectorFunc,
                                    void           * aVectorArg,
                                    void           * aEnv )
{
    dtlDataValue  * sValue1 = NULL;
    dtlDataValue  * sResultValue;
    stlErrorStack * sErrorStack;
    dtlDateTimeFormatInfo * sToCharFormatInfo = NULL;
    
    DTL_PARAM_VALIDATE( (aInputArgumentCnt == 0) || (aInputArgumentCnt == 1),
                        (stlErrorStack *)aEnv );

    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;
    
    if( aInputArgumentCnt == 0 )
    {
        sToCharFormatInfo =
            aVectorFunc->mGetTimestampFormatInfoFunc( aVectorArg );

        stlMemcpy( sResultValue->mValue,
                   sToCharFormatInfo,
                   sToCharFormatInfo->mSize );
    }
    else
    {
        sValue1 = aInputArgument[0].mValue.mDataValue;
        
        STL_TRY( dtlGetDateTimeFormatInfoForToChar(
                     DTL_TYPE_TIMESTAMP,
                     (stlChar *)(sValue1->mValue),
                     sValue1->mLength,
                     sResultValue->mValue,
                     sResultValue->mBufferSize,
                     aVectorFunc,
                     aVectorArg,
                     sErrorStack )
                 == STL_SUCCESS );
        
        sToCharFormatInfo = (dtlDateTimeFormatInfo *)(sResultValue->mValue);
    }
    
    sResultValue->mLength = sToCharFormatInfo->mSize;
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus dtlTimestampWithTimeZoneToCharFormat( stlUInt16        aInputArgumentCnt,
                                                dtlValueEntry  * aInputArgument,
                                                void           * aResultValue,
                                                void           * aInfo,
                                                dtlFuncVector  * aVectorFunc,
                                                void           * aVectorArg,
                                                void           * aEnv )
{
    dtlDataValue  * sValue1 = NULL;
    dtlDataValue  * sResultValue;
    stlErrorStack * sErrorStack;
    dtlDateTimeFormatInfo * sToCharFormatInfo = NULL;
    
    DTL_PARAM_VALIDATE( (aInputArgumentCnt == 0) || (aInputArgumentCnt == 1),
                        (stlErrorStack *)aEnv );

    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;
    
    if( aInputArgumentCnt == 0 )
    {
        sToCharFormatInfo =
            aVectorFunc->mGetTimestampWithTimeZoneFormatInfoFunc( aVectorArg );

        stlMemcpy( sResultValue->mValue,
                   sToCharFormatInfo,
                   sToCharFormatInfo->mSize );
    }
    else
    {
        sValue1 = aInputArgument[0].mValue.mDataValue;
        
        STL_TRY( dtlGetDateTimeFormatInfoForToChar(
                     DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,
                     (stlChar *)(sValue1->mValue),
                     sValue1->mLength,
                     sResultValue->mValue,
                     sResultValue->mBufferSize,
                     aVectorFunc,
                     aVectorArg,
                     sErrorStack )
                 == STL_SUCCESS );
        
        sToCharFormatInfo = (dtlDateTimeFormatInfo *)(sResultValue->mValue);
    }
    
    sResultValue->mLength = sToCharFormatInfo->mSize;
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus dtlIntervalYearToMonthToCharFormat( stlUInt16        aInputArgumentCnt,
                                              dtlValueEntry  * aInputArgument,
                                              void           * aResultValue,
                                              void           * aInfo,
                                              dtlFuncVector  * aVectorFunc,
                                              void           * aVectorArg,
                                              void           * aEnv )
{
    dtlDataValue  * sResultValue;
    
    DTL_PARAM_VALIDATE( (aInputArgumentCnt == 0) || (aInputArgumentCnt == 1),
                        (stlErrorStack *)aEnv );

    sResultValue = (dtlDataValue *)aResultValue;

    /*
     * interval year to month, interval day to second인 경우는,
     * fmt는 적용하지 않고 ( 무시하고 ),
     * interval value를 varchar로 변환해서 결과를 낸다.
     * sResultValue를 null로 처리할 수 없으므로( to_char의 결과가 null이 되므로 ),
     * sResultValue->mLength를 STL_SIZEOF(dtlToCharFormatInfo)로 설정한다.
     * 이 sResultValue는 이후 참조되지 않는다.
     */
        
    sResultValue->mLength = STL_SIZEOF( dtlDateTimeFormatInfo );

    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus dtlIntervalDayToSecondToCharFormat( stlUInt16        aInputArgumentCnt,
                                              dtlValueEntry  * aInputArgument,
                                              void           * aResultValue,
                                              void           * aInfo,
                                              dtlFuncVector  * aVectorFunc,
                                              void           * aVectorArg,
                                              void           * aEnv )
{
    dtlDataValue  * sResultValue;
    
    DTL_PARAM_VALIDATE( (aInputArgumentCnt == 0) || (aInputArgumentCnt == 1),
                        (stlErrorStack *)aEnv );

    sResultValue = (dtlDataValue *)aResultValue;

    /*
     * interval year to month, interval day to second인 경우는,
     * fmt는 적용하지 않고 ( 무시하고 ),
     * interval value를 varchar로 변환해서 결과를 낸다.
     * sResultValue를 null로 처리할 수 없으므로( to_char의 결과가 null이 되므로 ),
     * sResultValue->mLength를 STL_SIZEOF(dtlToCharFormatInfo)로 설정한다.
     * 이 sResultValue는 이후 참조되지 않는다.
     */
        
    sResultValue->mLength = STL_SIZEOF( dtlDateTimeFormatInfo );

    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus dtlNumericToCharFormat( stlUInt16        aInputArgumentCnt,
                                  dtlValueEntry  * aInputArgument,
                                  void           * aResultValue,
                                  void           * aInfo,
                                  dtlFuncVector  * aVectorFunc,
                                  void           * aVectorArg,
                                  void           * aEnv )
{
    dtlDataValue        * sValue1 = NULL;
    dtlDataValue        * sResultValue;
    stlErrorStack       * sErrorStack;
    
    DTL_PARAM_VALIDATE( (aInputArgumentCnt == 0) || (aInputArgumentCnt == 1),
                        (stlErrorStack *)aEnv );

    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;
    
    if( aInputArgumentCnt == 0 )
    {
        /* aToCharFormatInfoBuffer 초기화 */
        DTF_INIT_NUMBER_FORMAT_INFO( (dtfNumberFormatInfo *)(sResultValue->mValue) );
    }
    else
    {
        sValue1 = aInputArgument[0].mValue.mDataValue;
        
        STL_TRY( dtfGetNumberFormatInfoForToChar( DTL_TYPE_NUMBER,
                                                  (stlChar *)(sValue1->mValue),
                                                  sValue1->mLength,
                                                  sResultValue->mValue,
                                                  sResultValue->mBufferSize,
                                                  aVectorFunc,
                                                  aVectorArg,
                                                  sErrorStack )
                 == STL_SUCCESS );
    }
    
    sResultValue->mLength = STL_SIZEOF( dtfNumberFormatInfo );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


stlStatus dtlDoubleToCharFormat( stlUInt16        aInputArgumentCnt,
                                 dtlValueEntry  * aInputArgument,
                                 void           * aResultValue,
                                 void           * aInfo,
                                 dtlFuncVector  * aVectorFunc,
                                 void           * aVectorArg,
                                 void           * aEnv )
{
    dtlDataValue        * sValue1 = NULL;
    dtlDataValue        * sResultValue;
    stlErrorStack       * sErrorStack;
    
    DTL_PARAM_VALIDATE( (aInputArgumentCnt == 0) || (aInputArgumentCnt == 1),
                        (stlErrorStack *)aEnv );

    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;
    
    if( aInputArgumentCnt == 0 )
    {
        /* aToCharFormatInfoBuffer 초기화 */
        DTF_INIT_NUMBER_FORMAT_INFO( (dtfNumberFormatInfo *)(sResultValue->mValue) );
    }
    else
    {
        sValue1 = aInputArgument[0].mValue.mDataValue;
        
        STL_TRY( dtfGetNumberFormatInfoForToChar( DTL_TYPE_NATIVE_DOUBLE,
                                                  (stlChar *)(sValue1->mValue),
                                                  sValue1->mLength,
                                                  sResultValue->mValue,
                                                  sResultValue->mBufferSize,
                                                  aVectorFunc,
                                                  aVectorArg,
                                                  sErrorStack )
                 == STL_SUCCESS );
    }
    
    sResultValue->mLength = STL_SIZEOF( dtfNumberFormatInfo );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus dtlRealToCharFormat( stlUInt16        aInputArgumentCnt,
                               dtlValueEntry  * aInputArgument,
                               void           * aResultValue,
                               void           * aInfo,
                               dtlFuncVector  * aVectorFunc,
                               void           * aVectorArg,
                               void           * aEnv )
{
    dtlDataValue        * sValue1 = NULL;
    dtlDataValue        * sResultValue;
    stlErrorStack       * sErrorStack;
    
    DTL_PARAM_VALIDATE( (aInputArgumentCnt == 0) || (aInputArgumentCnt == 1),
                        (stlErrorStack *)aEnv );

    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;
    
    if( aInputArgumentCnt == 0 )
    {
        /* aToCharFormatInfoBuffer 초기화 */
        DTF_INIT_NUMBER_FORMAT_INFO( (dtfNumberFormatInfo *)(sResultValue->mValue) );
    }
    else
    {
        sValue1 = aInputArgument[0].mValue.mDataValue;
        
        STL_TRY( dtfGetNumberFormatInfoForToChar( DTL_TYPE_NATIVE_REAL,
                                                  (stlChar *)(sValue1->mValue),
                                                  sValue1->mLength,
                                                  sResultValue->mValue,
                                                  sResultValue->mBufferSize,
                                                  aVectorFunc,
                                                  aVectorArg,
                                                  sErrorStack )
                 == STL_SUCCESS );
    }
    
    sResultValue->mLength = STL_SIZEOF( dtfNumberFormatInfo );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief format string 으로 formatNode (format info)를 구성한다.
 * @param[in]   aDataType                        dtlDataType
 * @param[in]   aFormatString                    format string
 * @param[in]   aFormatStringLen                 format string length
 * @param[in,out]  aToCharFormatInfoBuffer       to_char format info를 위한 공간
 * @param[in]   aToCharFormatInfoBufferSize      to_char format info를 위한 공간 사이즈
 * @param[in]   aVectorFunc                      aVectorArg
 * @param[in]   aVectorArg                       aVectorArg
 * @param[in]   aErrorStack                      에러 스택
 */
stlStatus dtlGetDateTimeFormatInfoForToChar( dtlDataType                aDataType,
                                             stlChar                  * aFormatString,
                                             stlInt64                   aFormatStringLen,
                                             void                     * aToCharFormatInfoBuffer,
                                             stlInt32                   aToCharFormatInfoBufferSize,
                                             dtlFuncVector            * aVectorFunc,
                                             void                     * aVectorArg,
                                             stlErrorStack            * aErrorStack )
{
    stlInt8                           sPosition   = 0;
    stlInt8                           sLen        = 0;
    dtlDateTimeFormatInfo           * sToCharFormatInfo = NULL;
    dtlDateTimeFormatStrInfo        * sFormatStrInfo    = NULL;    
    const dtlDateTimeFormatKeyWord  * sKeyWord          = NULL;
    stlChar                         * sFormatString;
    stlChar                         * sFormatStringEnd;
    dtlDateTimeFormatNode           * sFormatNode;
    dtlAllocInfo                      sAllocInfo;
    stlUInt16                         sToCharResultLen = 0;
    
    /**
     * format string 길이 검사 & 초기화 
     */
    
    /* format string의 길이 검사 */
    STL_TRY_THROW( aFormatStringLen <= DTL_DATETIME_FORMAT_MAX_SIZE,
                   ERROR_DATE_FORMAT_LENGTH );
    
    /* aToCharFormatInfoBuffer 초기화 */
    stlMemset( aToCharFormatInfoBuffer, 0x00, aToCharFormatInfoBufferSize );

    /* 지역변수 sAllocInfo 초기화 */
    DTL_ALLOC_INFO_INIT( &sAllocInfo,
                         aToCharFormatInfoBuffer,
                         aToCharFormatInfoBufferSize );

    /**
     *  dtlDateTimeFormatInfo 공간 확보
     */
    
    STL_TRY_THROW( dtlAllocMem( &sAllocInfo,
                                STL_SIZEOF( dtlDateTimeFormatInfo ),
                                (void **) &sToCharFormatInfo )
                   == STL_SUCCESS,
                   ERROR_NOT_ENOUGH_BUFFER );

    /**
     *  dtlFormatStrInfo 공간 확보
     */
    
    STL_TRY_THROW( dtlAllocMem( &sAllocInfo,
                                STL_SIZEOF( dtlDateTimeFormatStrInfo ) ,
                                (void **) &(sToCharFormatInfo->mFormatStrInfo) )
                   == STL_SUCCESS,
                   ERROR_NOT_ENOUGH_BUFFER);

    /**
     *  dtlFormatStrInfo 멤버 공간 확보
     */
    
    sFormatStrInfo = sToCharFormatInfo->mFormatStrInfo;

    STL_TRY_THROW( dtlAllocMem( &sAllocInfo,
                                aFormatStringLen + 1,
                                (void **) &(sFormatStrInfo->mStr) )
                   == STL_SUCCESS,
                   ERROR_NOT_ENOUGH_BUFFER );
    
    /* format string 복사 */
    stlMemcpy( sFormatStrInfo->mStr,
               aFormatString,
               aFormatStringLen );
    sFormatStrInfo->mStr[aFormatStringLen] = '\0';
    /* '\0'을 제외한 mStr length */
    sFormatStrInfo->mStrLen = aFormatStringLen;

    sFormatString = sFormatStrInfo->mStr;
    sFormatStringEnd = sFormatString + aFormatStringLen;
    sPosition = 0;
    
    /**
     * format key word 를 찾아 정보를 구축한다.
     */

    STL_TRY_THROW( dtlAllocMem( &sAllocInfo,
                                STL_SIZEOF(dtlDateTimeFormatNode),
                                (void **) &(sFormatNode) )
                   == STL_SUCCESS,
                   ERROR_NOT_ENOUGH_BUFFER );
    
    sFormatStrInfo->mFormat = sFormatNode;

    while( sFormatString < sFormatStringEnd )
    {
        sKeyWord = dtfGetDateTimeFormatKeyWord( sFormatString );

        if( sKeyWord != NULL )
        {
            switch( sKeyWord->mTypeMode )
            {
                case DTL_FORMAT_TYPE_DATE:
                    STL_TRY_THROW( (aDataType == DTL_TYPE_DATE) ||
                                   (aDataType == DTL_TYPE_TIMESTAMP) ||
                                   (aDataType == DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE ),
                                   ERROR_DATE_FORMAT_STR );
                    break;
                case DTL_FORMAT_TYPE_TIME:
                    STL_TRY_THROW( (aDataType == DTL_TYPE_DATE) ||
                                   (aDataType == DTL_TYPE_TIME) ||
                                   (aDataType == DTL_TYPE_TIME_WITH_TIME_ZONE) ||
                                   (aDataType == DTL_TYPE_TIMESTAMP) ||
                                   (aDataType == DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE),
                                   ERROR_DATE_FORMAT_STR );

                    if( aDataType == DTL_TYPE_DATE )
                    {
                        /* date type에는 fractional second가 없다. */
                        STL_TRY_THROW( ( sKeyWord->mId != DTF_DATETIME_FMT_FF ) &&
                                       ( sKeyWord->mId != DTF_DATETIME_FMT_FF6 ) &&
                                       ( sKeyWord->mId != DTF_DATETIME_FMT_FF1 ) &&
                                       ( sKeyWord->mId != DTF_DATETIME_FMT_FF2 ) &&
                                       ( sKeyWord->mId != DTF_DATETIME_FMT_FF3 ) &&
                                       ( sKeyWord->mId != DTF_DATETIME_FMT_FF4 ) &&
                                       ( sKeyWord->mId != DTF_DATETIME_FMT_FF5 ),
                                       ERROR_DATE_FORMAT_STR );
                    }
                    else
                    {
                        /* Do Nothing */
                    }
                    
                    break;
                case DTL_FORMAT_TYPE_WITH_TIME_ZONE:
                    STL_TRY_THROW( (aDataType == DTL_TYPE_TIME_WITH_TIME_ZONE) ||
                                   (aDataType == DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE),
                                   ERROR_DATE_FORMAT_STR );
                    break;
                default:
                    STL_DASSERT( STL_FALSE );
                    break;
            }
            
            sFormatNode->mType   = DTF_FORMAT_NODE_TYPE_KEYWORD;
            sFormatNode->mKey    = sKeyWord;
            sFormatNode->mPos    = sPosition;
            sFormatNode->mPosLen = sKeyWord->mLen;

            sPosition += sFormatNode->mPosLen;
            sFormatString += sFormatNode->mPosLen;

            if( ( sKeyWord->mId == DTF_DATETIME_FMT_TZH )  ||
                ( sKeyWord->mId == DTF_DATETIME_FMT_SYYYY ) )
            {
                /* +, - 부호를 위한 한자리를 더한다. */
                sToCharResultLen += sKeyWord->mValueMaxLen + 1;
            }
            else
            {
                sToCharResultLen += sKeyWord->mValueMaxLen;
            }
        }
        else
        {
            if( DTF_DATETIME_FORMAT_SPECIAL_CHAR_INDEX[(stlUInt8)*(sFormatString )] == 0 )
            {
                /* 
                 * 특수문자처리
                 * 예: to_char( cast( '2001-01-01 as date ), 'YYYY  MM  DD )
                 *     => 2001  01  01
                 */
                
                sLen = 1;
                sFormatString++;
                
                while( sFormatString < sFormatStringEnd )
                {
                    if( DTF_DATETIME_FORMAT_SPECIAL_CHAR_INDEX[(stlUInt8)*(sFormatString )] == 0 )
                    {
                        sLen++;
                    }
                    else
                    {
                        break;
                    }

                    sFormatString++;
                }
                
                sFormatNode->mType   = DTF_FORMAT_NODE_TYPE_CHAR;
                sFormatNode->mKey    = NULL;
                sFormatNode->mPos    = sPosition;
                sFormatNode->mPosLen = sLen;

                sToCharResultLen += sLen;
                sPosition += sFormatNode->mPosLen;
            }
            else if( *sFormatString == '"' )
            {
                /* 예: to_char( cast( '2001-01-01 as date ), 'YYYY""MM""DD )
                 *     => 20010101
                 */

                sLen = 0;
                sPosition++;
                sFormatString++;
                
                while( sFormatString < sFormatStringEnd )
                {
                    if( *sFormatString == '"' )
                    {
                        break;
                    }
                    else
                    {
                        sLen++;
                    }

                    sFormatString++;
                }

                if( *sFormatString == '"' )
                {
                    sFormatNode->mType   = DTF_FORMAT_NODE_TYPE_CHAR;
                    sFormatNode->mKey    = NULL;
                    sFormatNode->mPos    = sPosition;
                    sFormatNode->mPosLen = sLen;

                    sToCharResultLen += sLen;
                    sPosition += sFormatNode->mPosLen + 1;

                    sFormatString++;
                }
                else
                {
                    STL_THROW( ERROR_DATE_FORMAT_STR );
                }
            }
            else
            {
                STL_THROW( ERROR_DATE_FORMAT_STR );
            }
        }

        STL_TRY_THROW( dtlAllocMem( &sAllocInfo,
                                    STL_SIZEOF(dtlDateTimeFormatNode),
                                    (void **) &(sFormatNode) )
                       == STL_SUCCESS,
                       ERROR_NOT_ENOUGH_BUFFER );
    }
    
    sFormatNode->mType = DTF_FORMAT_NODE_TYPE_END;

    sToCharFormatInfo->mToCharMaxResultLen = sToCharResultLen;
    sToCharFormatInfo->mSize = DTL_ALLOC_INFO_USED_SIZE( &sAllocInfo );    
    
    return STL_SUCCESS;

    STL_CATCH( ERROR_DATE_FORMAT_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_DATE_FORMAT_LENGTH,
                      NULL,
                      aErrorStack,
                      DTL_DATETIME_FORMAT_MAX_SIZE );
    }

    STL_CATCH( ERROR_DATE_FORMAT_STR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_DATE_FORMAT_NOT_RECOGNIZED,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( ERROR_NOT_ENOUGH_BUFFER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_NOT_ENOUGH_FORMAT_INFO_BUFFER,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief datetime string 에 {+|-}tzh:tzm 형식의 문자열이 존재하는지 여부 반환
 *   <BR> 단지, {+|-}tzh:tzm 형식의 문자열이 존재하는지 여부만 체크하고,
 *   <BR>       validate 검사는 하지 않는다.
 * @param[in] aDateTimeString          datetime string
 * @param[in] aDateTimeStringLength    datetime string length
 * @param[in] aIsTimestampStr          년-월-일을 포함한 timestamp string인지 여부
 */
stlBool dtlIsExistTimeZoneStr( stlChar   * aDateTimeString,
                               stlInt64    aDateTimeStringLength,
                               stlBool     aIsTimestampStr )
{
    return dtfIsExistTimeZoneStr( aDateTimeString,
                                  aDateTimeStringLength,
                                  aIsTimestampStr );
}


/**
 * @brief gsql에서 호출함. <BR>
 *        datetime type에 대해서 session format에 대한 strng을 얻는다.
 *        
 *  
 */
stlStatus dtlGetToCharDateTime( dtlDataType             aDataType,
                                dtlExpTime            * aDtlExpTime,
                                dtlFractionalSecond     aFractionalSecond,
                                stlInt32              * aTimeZone,
                                dtlDateTimeFormatInfo * aToCharFormatInfo,
                                stlInt64                aAvailableSize,
                                stlChar               * aResult,
                                stlInt64              * aLength,
                                stlErrorStack         * aErrorStack )
{
    STL_TRY( dtfDateTimeToChar( aToCharFormatInfo,
                                aDtlExpTime,
                                aFractionalSecond,
                                aTimeZone,
                                aAvailableSize,
                                aResult,
                                aLength,
                                NULL,
                                NULL,
                                aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/*******************************************************************************
 * ODBC / CDC 를 위한 DEFAULT DATETIME FORMAT FUNCTION 을 구축한다.
 * ODBC에서는 datetime의 input 은 session format을 따르고,
 *            datetime의 output string을 출력할 경우, 아래와 같은 format을 따른다.
 *******************************************************************************/

stlInt64   dtlNLSDateFormatInfoBufForODBC[DTL_DATETIME_FORMAT_INFO_MAX_BUFFER_SIZE / STL_SIZEOF(stlInt64)];
stlInt64   dtlNLSTimeFormatInfoBufForODBC[DTL_DATETIME_FORMAT_INFO_MAX_BUFFER_SIZE / STL_SIZEOF(stlInt64)];
stlInt64   dtlNLSTimeWithTimeZoneFormatInfoBufForODBC[DTL_DATETIME_FORMAT_INFO_MAX_BUFFER_SIZE / STL_SIZEOF(stlInt64)];
stlInt64   dtlNLSTimestampFormatInfoBufForODBC[DTL_DATETIME_FORMAT_INFO_MAX_BUFFER_SIZE / STL_SIZEOF(stlInt64)];
stlInt64   dtlNLSTimestampWithTimeZonwFormatInfoBufForODBC[DTL_DATETIME_FORMAT_INFO_MAX_BUFFER_SIZE / STL_SIZEOF(stlInt64)];

dtlDateTimeFormatInfo  * gDtlNLSDateFormatInfoForODBC = (dtlDateTimeFormatInfo*)dtlNLSDateFormatInfoBufForODBC;
dtlDateTimeFormatInfo  * gDtlNLSTimeFormatInfoForODBC = (dtlDateTimeFormatInfo*)dtlNLSTimeFormatInfoBufForODBC;
dtlDateTimeFormatInfo  * gDtlNLSTimeWithTimeZoneFormatInfoForODBC = (dtlDateTimeFormatInfo*)dtlNLSTimeWithTimeZoneFormatInfoBufForODBC;
dtlDateTimeFormatInfo  * gDtlNLSTimestampFormatInfoForODBC = (dtlDateTimeFormatInfo*)dtlNLSTimestampFormatInfoBufForODBC;
dtlDateTimeFormatInfo  * gDtlNLSTimestampWithTimeZoneFormatInfoForODBC = (dtlDateTimeFormatInfo*)dtlNLSTimestampWithTimeZonwFormatInfoBufForODBC;


stlStatus dtlBuildNLSDateTimeFormatInfoForODBC( stlErrorStack * aErrorStack )
{
    /**************************************
     * NLS_DATE_FORMAT ( "SYYYY-MM-DD" )
     **************************************/ 
    
    STL_TRY( dtlGetDateTimeFormatInfoForToChar( DTL_TYPE_DATE,
                                                DTL_DATE_FORMAT_FOR_ODBC,
                                                stlStrlen(DTL_DATE_FORMAT_FOR_ODBC),
                                                gDtlNLSDateFormatInfoForODBC,
                                                DTL_DATETIME_FORMAT_INFO_MAX_BUFFER_SIZE,
                                                NULL, // aVectorFunc
                                                NULL, // aVectorArg
                                                aErrorStack )
             == STL_SUCCESS );

    STL_TRY( dtlGetDateTimeFormatInfoForToDateTime( DTL_TYPE_DATE,
                                                    DTL_DATE_FORMAT_FOR_ODBC,
                                                    stlStrlen(DTL_DATE_FORMAT_FOR_ODBC),
                                                    STL_TRUE, /* aIsSaveToCharMaxResultLen */
                                                    gDtlNLSDateFormatInfoForODBC,
                                                    DTL_DATETIME_FORMAT_INFO_MAX_BUFFER_SIZE,
                                                    NULL, // aVectorFunc
                                                    NULL, // aVectorArg
                                                    aErrorStack )
             == STL_SUCCESS );
    
    /**************************************
     * NLS_TIME_FORMAT ( "HH24:MI:SS.FF6" )
     **************************************/ 
    
    STL_TRY( dtlGetDateTimeFormatInfoForToChar( DTL_TYPE_TIME,
                                                DTL_TIME_FORMAT_FOR_ODBC,
                                                stlStrlen(DTL_TIME_FORMAT_FOR_ODBC),
                                                gDtlNLSTimeFormatInfoForODBC,
                                                DTL_DATETIME_FORMAT_INFO_MAX_BUFFER_SIZE,
                                                NULL, // aVectorFunc
                                                NULL, // aVectorArg
                                                aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY( dtlGetDateTimeFormatInfoForToDateTime( DTL_TYPE_TIME,
                                                    DTL_TIME_FORMAT_FOR_ODBC,
                                                    stlStrlen(DTL_TIME_FORMAT_FOR_ODBC),
                                                    STL_TRUE, /* aIsSaveToCharMaxResultLen */
                                                    gDtlNLSTimeFormatInfoForODBC,
                                                    DTL_DATETIME_FORMAT_INFO_MAX_BUFFER_SIZE,
                                                    NULL, // aVectorFunc
                                                    NULL, // aVectorArg
                                                    aErrorStack )
             == STL_SUCCESS );

    /**************************************
     * NLS_TIME_WITH_TIME_ZONE_FORMAT ( "HH24:MI:SS.FF6 TZH:TZM" )
     **************************************/ 
    
    STL_TRY( dtlGetDateTimeFormatInfoForToChar( DTL_TYPE_TIME_WITH_TIME_ZONE,
                                                DTL_TIME_WITH_TIME_ZONE_FORMAT_FOR_ODBC,
                                                stlStrlen(DTL_TIME_WITH_TIME_ZONE_FORMAT_FOR_ODBC),
                                                gDtlNLSTimeWithTimeZoneFormatInfoForODBC,
                                                DTL_DATETIME_FORMAT_INFO_MAX_BUFFER_SIZE,
                                                NULL, // aVectorFunc
                                                NULL, // aVectorArg
                                                aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY( dtlGetDateTimeFormatInfoForToDateTime( DTL_TYPE_TIME_WITH_TIME_ZONE,
                                                    DTL_TIME_WITH_TIME_ZONE_FORMAT_FOR_ODBC,
                                                    stlStrlen(DTL_TIME_WITH_TIME_ZONE_FORMAT_FOR_ODBC),
                                                    STL_TRUE, /* aIsSaveToCharMaxResultLen */
                                                    gDtlNLSTimeWithTimeZoneFormatInfoForODBC,
                                                    DTL_DATETIME_FORMAT_INFO_MAX_BUFFER_SIZE,
                                                    NULL, // aVectorFunc
                                                    NULL, // aVectorArg
                                                    aErrorStack )
             == STL_SUCCESS );

    /**************************************
     * NLS_TIMESTAMP_FORMAT ( "SYYYY-MM-DD HH24:MI:SS.FF6" )
     **************************************/ 
    
    STL_TRY( dtlGetDateTimeFormatInfoForToChar( DTL_TYPE_TIMESTAMP,
                                                DTL_TIMESTAMP_FORMAT_FOR_ODBC,
                                                stlStrlen(DTL_TIMESTAMP_FORMAT_FOR_ODBC),
                                                gDtlNLSTimestampFormatInfoForODBC,
                                                DTL_DATETIME_FORMAT_INFO_MAX_BUFFER_SIZE,
                                                NULL, // aVectorFunc
                                                NULL, // aVectorArg
                                                aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY( dtlGetDateTimeFormatInfoForToDateTime( DTL_TYPE_TIMESTAMP,
                                                    DTL_TIMESTAMP_FORMAT_FOR_ODBC,
                                                    stlStrlen(DTL_TIMESTAMP_FORMAT_FOR_ODBC),
                                                    STL_TRUE, /* aIsSaveToCharMaxResultLen */
                                                    gDtlNLSTimestampFormatInfoForODBC,
                                                    DTL_DATETIME_FORMAT_INFO_MAX_BUFFER_SIZE,
                                                    NULL, // aVectorFunc
                                                    NULL, // aVectorArg
                                                    aErrorStack )
             == STL_SUCCESS );

    /**************************************
     * NLS_TIMESTAMP_WITH_TIME_ZONE_FORMAT ( "SYYYY-MM-DD HH24:MI:SS.FF6 TZH:TZM" )
     **************************************/ 
    
    STL_TRY( dtlGetDateTimeFormatInfoForToChar( DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,
                                                DTL_TIMESTAMP_WITH_TIME_ZONE_FORMAT_FOR_ODBC,
                                                stlStrlen(DTL_TIMESTAMP_WITH_TIME_ZONE_FORMAT_FOR_ODBC),
                                                gDtlNLSTimestampWithTimeZoneFormatInfoForODBC,
                                                DTL_DATETIME_FORMAT_INFO_MAX_BUFFER_SIZE,
                                                NULL, // aVectorFunc
                                                NULL, // aVectorArg
                                                aErrorStack )
             == STL_SUCCESS );

    STL_TRY( dtlGetDateTimeFormatInfoForToDateTime( DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,
                                                    DTL_TIMESTAMP_WITH_TIME_ZONE_FORMAT_FOR_ODBC,
                                                    stlStrlen(DTL_TIMESTAMP_WITH_TIME_ZONE_FORMAT_FOR_ODBC),
                                                    STL_TRUE, /* aIsSaveToCharMaxResultLen */
                                                    gDtlNLSTimestampWithTimeZoneFormatInfoForODBC,
                                                    DTL_DATETIME_FORMAT_INFO_MAX_BUFFER_SIZE,
                                                    NULL, // aVectorFunc
                                                    NULL, // aVectorArg
                                                    aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;    
}




/** @} */
