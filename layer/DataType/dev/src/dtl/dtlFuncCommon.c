/*******************************************************************************
 * dtlFuncCommon.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: dtlFuncCommon.c 1841 2011-09-07 07:14:06Z jhkim $
 *
 * NOTES
 *  찾는 함수 없을 경우 error 리턴함.
 *
 ******************************************************************************/

/**
 * @file dtlFuncCommon.c
 * @brief DataType Layer Function관련 함수 
 */

#include <dtl.h>
#include <dtDef.h>
#include <dtlFuncCommon.h>

/**
 * @ingroup dtlFunction Function관련 함수
 * @{
 */

/*
 * dtlGroup별 변환 가능을 정의한다.
 * target type으로 변환 가능한 source type list를 정의한다.
 * mSourceList 는 마지막을 알기위해 마지막은 DTL_GROUP_MAX를 넣는다.
 */
const dtlGroupCast dtlFuncArgGroupCast[] =
{
    { DTL_GROUP_BOOLEAN,
      { DTL_GROUP_BOOLEAN,
        DTL_GROUP_CHAR_STRING,
        DTL_GROUP_LONGVARCHAR_STRING,
        DTL_GROUP_MAX } }, 
    { DTL_GROUP_BINARY_INTEGER,
      { DTL_GROUP_BINARY_INTEGER,
        DTL_GROUP_MAX } },    
    { DTL_GROUP_NUMBER,
      { DTL_GROUP_NUMBER,
        DTL_GROUP_BINARY_INTEGER,
        DTL_GROUP_CHAR_STRING,
        DTL_GROUP_LONGVARCHAR_STRING,
        DTL_GROUP_MAX } },
    { DTL_GROUP_BINARY_REAL,
      { DTL_GROUP_BINARY_REAL,
        DTL_GROUP_BINARY_INTEGER,
        DTL_GROUP_CHAR_STRING,
        DTL_GROUP_LONGVARCHAR_STRING,
        DTL_GROUP_NUMBER,
        DTL_GROUP_MAX } },    
    { DTL_GROUP_CHAR_STRING,
      { DTL_GROUP_CHAR_STRING,
        DTL_GROUP_NUMBER,
        DTL_GROUP_BINARY_REAL,
        DTL_GROUP_BINARY_INTEGER,
        DTL_GROUP_BOOLEAN,
        DTL_GROUP_DATE,
        DTL_GROUP_TIME,
        DTL_GROUP_TIMESTAMP,
        DTL_GROUP_TIME_WITH_TIME_ZONE,
        DTL_GROUP_TIMESTAMP_WITH_TIME_ZONE,
        DTL_GROUP_INTERVAL_YEAR_TO_MONTH,
        DTL_GROUP_INTERVAL_DAY_TO_SECOND,        
        DTL_GROUP_ROWID,
        DTL_GROUP_MAX } },
    { DTL_GROUP_LONGVARCHAR_STRING,
      { DTL_GROUP_CHAR_STRING,
        DTL_GROUP_LONGVARCHAR_STRING,
        DTL_GROUP_NUMBER,
        DTL_GROUP_BINARY_REAL,
        DTL_GROUP_BINARY_INTEGER,
        DTL_GROUP_BOOLEAN,
        DTL_GROUP_DATE,
        DTL_GROUP_TIME,
        DTL_GROUP_TIMESTAMP,
        DTL_GROUP_TIME_WITH_TIME_ZONE,
        DTL_GROUP_TIMESTAMP_WITH_TIME_ZONE,
        DTL_GROUP_INTERVAL_YEAR_TO_MONTH,
        DTL_GROUP_INTERVAL_DAY_TO_SECOND,                
        DTL_GROUP_ROWID,
        DTL_GROUP_MAX } },
    { DTL_GROUP_BINARY_STRING,
      { DTL_GROUP_BINARY_STRING,
        DTL_GROUP_CHAR_STRING,          /* UNTYPED NULL을 위한 변환 */
        DTL_GROUP_MAX } },
    { DTL_GROUP_LONGVARBINARY_STRING,
      { DTL_GROUP_BINARY_STRING,
        DTL_GROUP_LONGVARBINARY_STRING,
        DTL_GROUP_CHAR_STRING,          /* UNTYPED NULL을 위한 변환 */
        DTL_GROUP_MAX } },
    { DTL_GROUP_DATE,
      { DTL_GROUP_DATE,
        DTL_GROUP_CHAR_STRING,
        DTL_GROUP_LONGVARCHAR_STRING,
        DTL_GROUP_MAX } },
    { DTL_GROUP_TIME,
      { DTL_GROUP_TIME,
        DTL_GROUP_CHAR_STRING,
        DTL_GROUP_LONGVARCHAR_STRING,
        DTL_GROUP_MAX } },
    { DTL_GROUP_TIMESTAMP,
      { DTL_GROUP_TIMESTAMP,
        DTL_GROUP_CHAR_STRING,
        DTL_GROUP_LONGVARCHAR_STRING,
        DTL_GROUP_DATE,
        DTL_GROUP_MAX } },
    { DTL_GROUP_TIME_WITH_TIME_ZONE,
      { DTL_GROUP_TIME_WITH_TIME_ZONE,
        DTL_GROUP_CHAR_STRING,
        DTL_GROUP_LONGVARCHAR_STRING,
        DTL_GROUP_TIME,
        DTL_GROUP_MAX } },
    { DTL_GROUP_TIMESTAMP_WITH_TIME_ZONE,
      { DTL_GROUP_TIMESTAMP_WITH_TIME_ZONE,
        DTL_GROUP_CHAR_STRING,
        DTL_GROUP_LONGVARCHAR_STRING,
        DTL_GROUP_DATE,
        DTL_GROUP_TIMESTAMP,
        DTL_GROUP_MAX } },
    { DTL_GROUP_INTERVAL_YEAR_TO_MONTH,
      { DTL_GROUP_INTERVAL_YEAR_TO_MONTH,
        DTL_GROUP_CHAR_STRING,
        DTL_GROUP_LONGVARCHAR_STRING,
        DTL_GROUP_MAX } },
    { DTL_GROUP_INTERVAL_DAY_TO_SECOND,
      { DTL_GROUP_INTERVAL_DAY_TO_SECOND,
        DTL_GROUP_CHAR_STRING,
        DTL_GROUP_LONGVARCHAR_STRING,
        DTL_GROUP_MAX } },
    { DTL_GROUP_ROWID,
      { DTL_GROUP_ROWID,
        DTL_GROUP_CHAR_STRING,
        DTL_GROUP_LONGVARCHAR_STRING,
        DTL_GROUP_MAX } }    
};


/**
 * @brief common function에 대한 정보 얻기
 * @param[in]  aFuncInfo                   대표 func의 공통 정보
 * @param[in]  aFuncList                   arg별 func list 정의 구조체
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
 * @param[out] aErrorStack                 에러 스택
 */
stlStatus dtlGetFuncInfoCommon( const dtlFuncInfo     * aFuncInfo,
                                const dtlFuncArgInfo  * aFuncList,
                                stlUInt16               aDataTypeArrayCount,
                                stlBool               * aIsConstantData,
                                dtlDataType           * aDataTypeArray,
                                dtlDataTypeDefInfo    * aDataTypeDefInfoArray,
                                stlUInt16               aFuncArgDataTypeArrayCount,
                                dtlDataType           * aFuncArgDataTypeArray,
                                dtlDataTypeDefInfo    * aFuncArgDataTypeDefInfoArray,
                                dtlDataType           * aFuncResultDataType,
                                dtlDataTypeDefInfo    * aFuncResultDataTypeDefInfo,
                                stlUInt32             * aFuncPtrIdx,
                                stlErrorStack         * aErrorStack )
{
    dtlGroup            sInputGroup[DTL_MAX_FUNC_ARG_CNT];
    stlInt32            sFuncIdx;
    dtlGroup            sArgGroupFunc;
    dtlGroup            sArgGroupInput;
    stlChar             sErrMsg[128];
    stlInt32            i;
    stlInt32            j;

    DTL_PARAM_VALIDATE( aFuncInfo != NULL, aErrorStack );
    /*
     * 아래 validate에서 걸릴경우 DTL_MAX_FUNC_ARG_CNT를 늘려줘야 함.
     */
    DTL_PARAM_VALIDATE( STL_SIZEOF( aFuncInfo->mValidArgList ) ==
                        STL_SIZEOF( aFuncInfo->mValidArgList[0] ) * DTL_MAX_FUNC_ARG_CNT,
                        aErrorStack );

    DTL_PARAM_VALIDATE( aFuncList != NULL, aErrorStack );

    sFuncIdx = 0;

    /*
     * input arg를 group으로 분류한다.
     */
    for( i = 0; i < aDataTypeArrayCount; i++ )
    {
        if( aFuncInfo->mValidArgList[i] == STL_TRUE )
        {
            sInputGroup[i] = dtlDataTypeGroup[aDataTypeArray[i]];
        }
    }

    /*
     * dtlBucketFuncList의 함수별로 input으로 들어온
     * arg처리가 가능한지 확인한다.
     */
    while( aFuncList->mFuncPtr != NULL )
    {
        /*
         * 현재 aArgTypeList->mFuncPtr 이 함수에 대해서 검증한다.
         * aArgTypeList->mFuncPtr함수의 파라미터가
         * 요청 arg를 처리할수 있는지 확인한다.
         *
         * arg 하나씩 체크한다.
         */

        for( i = 0; i < aDataTypeArrayCount; i++ )
        {
            /*
             * 함수의 i번째 arg에 대해서 사용 가능한지 확인한다.
             * j는 target arg에 대한 source arg list index임
             * sArgGroupFunc : 현재 func의 i번째 arg의 GroupType
             * sArgGroupInput : group Type으로 변환 가능한 group type list
             */

            if( aFuncInfo->mValidArgList[i] == STL_FALSE )
            {
                continue;
            }
            else
            {
                /* Do Nothing */
            }


            sArgGroupFunc = dtlDataTypeGroup[aFuncList->mArgType[i]];
            sArgGroupInput = dtlFuncArgGroupCast[sArgGroupFunc].mSourceList[0];
            j = 0;
            while( sArgGroupInput != DTL_GROUP_MAX )
            {
                /*
                 * sArgTypeList->mArgType[i] 는 함수의 arg type을 나타냄.
                 * dtlCast[sArgTypeList->mArgType[i]] 는 그 arg type의
                 */
                if ( sArgGroupInput == sInputGroup[i] )
                {
                    /* 현재 함수에 input arg 사용 가능함. */
                    break;
                }
                else
                {
                    j++;
                    sArgGroupInput = dtlFuncArgGroupCast[sArgGroupFunc].mSourceList[j];
                }
            }

            /*
             * 함수의 arg가 input arg를 처리할수 있으면 다음 arg체크로 넘어감.
             * input arg를 처리할수 없으면 다음 함수로 넘어감.
             */
            if ( sArgGroupInput == DTL_GROUP_MAX )
            {
                /*
                 * 함수가 input arg type을 처리할수 없는 경우임.
                 * 다음 함수 체크로 넘어감.
                 */
                break;
            }
            else
            {
                /*
                 * Do Nothing
                 * 다음 arg 체크로 넘어감.
                 */
            }
        }

        /*
         * arg를 모두 check했으면 성공.
         * 중간에 빠져 나온건 다른 함수 체크해야 함.
         */
        if ( i == aDataTypeArrayCount )
        {
            /*
             * arg를 모두 체크한 경우임.
             * 처리할 함수를 찾은 경우임.
             */
            break;
        }
        else
        {
            /*
             * 처리할 함수를 못찾았음으로 다음 함수 체크함.
             */
            /* Do Nothing */
        }

        aFuncList++;
        sFuncIdx++;
    }


    if ( aFuncList->mFuncPtr == NULL )
    {
        /*
         * 사용 가능한 함수를 못찾은 경우임.
         */
        STL_THROW( ERROR_NOT_APPLICABLE );

    }
    else
    {
        /*
         * 함수 찾았으면 찾은 함수의 arg type을 output.
         *
         * function의 argument type의 info가 저장되는 부분 조정.
         */
        
        for( i = 0; i < aFuncArgDataTypeArrayCount; i++ )
        {
            sArgGroupFunc = dtlDataTypeGroup[aFuncList->mArgType[i]];
            sArgGroupInput = dtlDataTypeGroup[aDataTypeArray[i]];
            
            if( ( ( sArgGroupFunc == DTL_GROUP_CHAR_STRING ) &&
                  ( sArgGroupInput == DTL_GROUP_CHAR_STRING ) )
                ||
                ( ( sArgGroupFunc == DTL_GROUP_LONGVARCHAR_STRING ) &&
                  ( ( sArgGroupInput == DTL_GROUP_CHAR_STRING ) ||
                    ( sArgGroupInput == DTL_GROUP_LONGVARCHAR_STRING ) ) )
                ||
                ( ( sArgGroupFunc == DTL_GROUP_BINARY_STRING ) &&
                  ( sArgGroupInput == DTL_GROUP_BINARY_STRING ) )
                ||
                ( ( sArgGroupFunc == DTL_GROUP_LONGVARBINARY_STRING ) &&
                  ( ( sArgGroupInput == DTL_GROUP_BINARY_STRING ) ||
                    ( sArgGroupInput == DTL_GROUP_LONGVARBINARY_STRING ) ) ) )
            {
                /*
                 * string 계열간에는 굳이 conversion을 발생하지 않게 한다.
                 * 예1) . char    -> longvarchar
                 *      . varchar -> longvarchar
                 *      . longvarchar -> longvarchar
                 *
                 * 예2) . char    -> varchar
                 *      . varchar -> varchar
                 */
                
                aFuncArgDataTypeArray[i] = aDataTypeArray[i];
                aFuncArgDataTypeDefInfoArray[i] = aDataTypeDefInfoArray[i];
            }
            else
            {
                /*
                 * longvarchar로 conversion이 발생하는 경우 중,
                 * string 계열이 아닌 type들은 varchar로 conversion 되도록 한다.
                 *
                 * longvarbinary로 conversion이 발생하는 경우 중,
                 * binary 계열이 아닌 type들은 varbinary로 conversion 되도록 한다.
                 */
                
                if( aFuncList->mArgType[i] == DTL_TYPE_LONGVARCHAR )
                {
                    STL_DASSERT( (sArgGroupInput != DTL_GROUP_CHAR_STRING) &&
                                 (sArgGroupInput != DTL_GROUP_LONGVARCHAR_STRING) );
                    
                    aFuncArgDataTypeArray[i] = DTL_TYPE_VARCHAR;
                }
                else if( aFuncList->mArgType[i] == DTL_TYPE_LONGVARBINARY )
                {
                    STL_DASSERT( (sArgGroupInput != DTL_GROUP_BINARY_STRING) &&
                                 (sArgGroupInput != DTL_GROUP_LONGVARBINARY_STRING) );

                    aFuncArgDataTypeArray[i] = DTL_TYPE_VARBINARY;
                }
                else
                {
                    aFuncArgDataTypeArray[i] = aFuncList->mArgType[i];
                }

                if( ( (aFuncArgDataTypeArray[i] == DTL_TYPE_INTERVAL_YEAR_TO_MONTH) &&
                      (aDataTypeArray[i] == DTL_TYPE_INTERVAL_YEAR_TO_MONTH) )
                    ||
                    ( (aFuncArgDataTypeArray[i] == DTL_TYPE_INTERVAL_DAY_TO_SECOND) &&
                      (aDataTypeArray[i] == DTL_TYPE_INTERVAL_DAY_TO_SECOND) ) )
                {
                    /* function argument datatype과 input datatype이 같은 interval type인 경우,
                     * input datatype의 info 정보로 지정한다. */
                    aFuncArgDataTypeDefInfoArray[i] = aDataTypeDefInfoArray[i];
                }
                else
                {
                    if( ( ( sArgGroupInput != DTL_GROUP_CHAR_STRING ) &&
                          ( sArgGroupInput != DTL_GROUP_LONGVARCHAR_STRING ) ) &&
                        ( aFuncArgDataTypeArray[i] == DTL_TYPE_VARCHAR ) )
                    {
                        /*
                         * string 계열이 아닌 type이 varchar 로 변환되어야 하는 경우로,
                         * aDataTypeArray[i]가 string으로 변환되었을때의 size로 precision 조정.
                         */
                        
                        STL_DASSERT( ( sArgGroupInput != DTL_GROUP_CHAR_STRING ) &&
                                     ( sArgGroupInput != DTL_GROUP_LONGVARCHAR_STRING ) );
                        
                        aFuncArgDataTypeDefInfoArray[i].mArgNum1 =
                            ( gResultDataTypeDef[aDataTypeArray[i]].mStringBufferSize - 1 );
                    }
                    else
                    {
                        aFuncArgDataTypeDefInfoArray[i].mArgNum1 =
                            gResultDataTypeDef[aFuncArgDataTypeArray[i]].mArgNum1;
                    }
                    
                    aFuncArgDataTypeDefInfoArray[i].mArgNum2 =
                        gResultDataTypeDef[aFuncArgDataTypeArray[i]].mArgNum2;
                    aFuncArgDataTypeDefInfoArray[i].mStringLengthUnit =
                        gResultDataTypeDef[aFuncArgDataTypeArray[i]].mStringLengthUnit;                    
                    aFuncArgDataTypeDefInfoArray[i].mIntervalIndicator =
                        gResultDataTypeDef[aFuncArgDataTypeArray[i]].mIntervalIndicator;
                }
            }
        }
        
        *aFuncResultDataType = aFuncList->mResultType;

        aFuncResultDataTypeDefInfo->mArgNum1 = gResultDataTypeDef[*aFuncResultDataType].mArgNum1;
        aFuncResultDataTypeDefInfo->mArgNum2 = gResultDataTypeDef[*aFuncResultDataType].mArgNum2;
        aFuncResultDataTypeDefInfo->mStringLengthUnit =
            gResultDataTypeDef[*aFuncResultDataType].mStringLengthUnit;
        aFuncResultDataTypeDefInfo->mIntervalIndicator =
            gResultDataTypeDef[*aFuncResultDataType].mIntervalIndicator;

        *aFuncPtrIdx = sFuncIdx;
    }


    return STL_SUCCESS;

    STL_CATCH( ERROR_NOT_APPLICABLE )
    {
        sErrMsg[0] = 0x00;
        for( i = 0; i < aDataTypeArrayCount; i++ )
        {
            stlSnprintf( sErrMsg + stlStrlen( sErrMsg ),
                         STL_SIZEOF( sErrMsg ) - stlStrlen( sErrMsg ),
                         "%s, ",
                         dtlDataTypeName[aDataTypeArray[i]]);
        }
        /*
         * 마지막 ',' 삭제.
         */
        if ( aDataTypeArrayCount > 0 )
        {
            sErrMsg[stlStrlen( sErrMsg ) - 2] = 0x00;
        }
        else
        {
            /* Do Nothing */
        }

        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INCONSISTENT_DATATYPE_FUNCTION_PARAMETERS,
                      NULL,
                      aErrorStack,
                      sErrMsg );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief column간의 conversion정보를 얻고 대표 Type을 선정한다.
 *
 * @param[in]  aDataTypeArray               input data type
 * @param[in]  aFuncArgDataTypeArrayCount   input data type array count
 * @param[out] aReturnFuncGroupType         반환 될 Return datatype group
 * @param[out] aErrorStack                  에러스택
 */

stlStatus dtlGetDataTypesConversionInfo( dtlDataType   * aDataTypeArray,
                                         stlUInt16       aFuncArgDataTypeArrayCount,
                                         dtlGroup      * aReturnFuncGroupType,
                                         stlErrorStack * aErrorStack )
{
    dtlDataType   sType;
    dtlGroup      sGroup;
    stlUInt16     i;

    /**
     * N 대 N 연산시,
     * Conversion 가능 여부와 동일한 Conversion Return Type을 찾기.
     * 두 type의 Group(ConversionGroup)을 검사하여 나온 Return Group을
     * 다른 Type의 Group과 비교해 봄으로써 서로 Conversion의 가능여부를 찾는다.
     *
     *  Integer  =   Char   =  Interval Day
     *    ^           ^
     *    -- Integer --            ^
     *         ^                   .         => ( O )
     *         ------ Interval -----
     * 
     */
    
    STL_PARAM_VALIDATE( ( aDataTypeArray[0] >= DTL_TYPE_BOOLEAN ) &&
                        ( aDataTypeArray[0] < DTL_TYPE_MAX ),
                        aErrorStack );

    sGroup = dtlDataTypeGroup[aDataTypeArray[0]];
    
    for( i = 1; i < aFuncArgDataTypeArrayCount ; i ++ )
    {
        STL_PARAM_VALIDATE( ( aDataTypeArray[i] >= DTL_TYPE_BOOLEAN ) &&
                            ( aDataTypeArray[i] < DTL_TYPE_MAX ),
                            aErrorStack );
        
        sType = aDataTypeArray[i];

        /* 두 Group의 Conversion 검사 및 Return Group Type */
        sGroup = dtlConversionGroup[sGroup][dtlDataTypeGroup[sType]];
        
        STL_TRY_THROW( (sGroup != DTL_GROUP_MAX), ERROR_NOT_APPLICABLE );
    }

    *aReturnFuncGroupType = sGroup;
    
    return STL_SUCCESS;
    
    STL_CATCH( ERROR_NOT_APPLICABLE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_COMPARISON_NOT_APPLICABLE,
                      NULL,
                      aErrorStack,
                      dtlDataTypeName[ aDataTypeArray[i-1] ],
                      dtlDataTypeName[ aDataTypeArray[i] ] );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief conversion정보로 얻은 대표 Type에 대한 Function Argument Data type을 얻는다.
 *
 * @param[in]  aDataTypeArray               input data type
 * @param[in]  aFuncArgDataTypeArrayCount   input data type array count
 * @param[in]  aReturnGroupType             input datat ype group
 * @param[out] aFuncArgDataTypeArray        반환 될 Func arg data type
 * @param[out] aErrorStack                  에러스택
 */

stlStatus dtlSetFuncArgDataTypeInfo( dtlDataType   * aDataTypeArray,
                                     stlUInt16       aFuncArgDataTypeArrayCount,
                                     dtlGroup        aReturnGroupType,
                                     dtlDataType   * aFuncArgDataTypeArray,
                                     stlErrorStack * aErrorStack )
{
    stlUInt16    i;

    for( i = 0; i < aFuncArgDataTypeArrayCount; i++ )
    {
        /**
         * 같은 Group내의 Type 연산 시,
         * Type의 변형없이 선언한 Type을 그대로 따라야 하는 경우.
         * 
         * 1) Char = VarChar 와 같은 경우 (Binary = VarBinary도)
         *    Type 변환      : char '0' = varchar '0'  =>  (conversion) varchar '0' == varchar '0'  (틀림)
         *    선언 Type 연산 : char '0' = varchar '0'   =>  (not conversion)char '0' != varchar '0'  (맞음)
         * --------------------------------------------------
         *      Group      |         Type
         * --------------------------------------------------   
         *  Binary_Integer =   SmallInt, Integer, BigInt
         *  Binary_Real    =   Real, Double, (Float)
         *    Numeric      =   Numeric, ( Decimal)
         *  Char_String    =   Char, Varchar, Long Varchar, (character object)
         *  Binary_String  =   Binary, VarBinary, Long VarBinary,...
         * 
         * --------------------------------------------------
         */
        
        if( aReturnGroupType == dtlDataTypeGroup[aDataTypeArray[i]] )
        {
            aFuncArgDataTypeArray[i] = aDataTypeArray[i];
        }
        else
        {
            /**
             * 2) Integer = Real 와 같은 경우, Type간의 연산이 존재하므로 Numeric으로 변환할 필요없음.
             *    Type 변환      : Integer '0' = Real '0'      =>  Numeric '0' = Numeric '0' 
             *    선언 Type 연산 : Integer '0' = Real '0'      =>  Integer '0' = Real '0'
             *
             *    But, Char Type의 경우에는 숫자와 연산시 숫자 타입으로 Conversion해줘야 하므로
             *         Char Type은 Conversion Type을 따른다. 
             *    
             * --------------------------------------------------
             *      Group      |         Type
             * --------------------------------------------------   
             *  Binary_Integer =   SmallInt, Integer, BigInt
             *  Binary_Real    =   Real, Double, (Float)
             *    Numeric      =   Numeric, ( Decimal)
             *  [ Char_String  =>    숫자 Type ]  
             * --------------------------------------------------
             */

            if( ( aReturnGroupType == DTL_GROUP_BINARY_INTEGER ) ||
                ( aReturnGroupType == DTL_GROUP_BINARY_REAL ) ||
                ( aReturnGroupType == DTL_GROUP_NUMBER ) )
            {
                if( ( dtlDataTypeGroup[aDataTypeArray[i]] == DTL_GROUP_BINARY_INTEGER ) ||
                    ( dtlDataTypeGroup[aDataTypeArray[i]] == DTL_GROUP_BINARY_REAL ) ||
                    ( dtlDataTypeGroup[aDataTypeArray[i]] == DTL_GROUP_NUMBER ) )
                {
                    aFuncArgDataTypeArray[i] = aDataTypeArray[i];
                }
                else
                {
                    /**
                     *  DTL_GROUP_CHAR_STRING,
                     *  DTL_GROUP_LONGVARCHAR_STRING,
                     */ 
                    aFuncArgDataTypeArray[i] = DTL_TYPE_NUMBER;
                }
            }
            else if( ( aReturnGroupType == DTL_GROUP_CHAR_STRING ) ||
                     ( aReturnGroupType == DTL_GROUP_LONGVARCHAR_STRING ) )
            {
                if( ( dtlDataTypeGroup[aDataTypeArray[i]] == DTL_GROUP_CHAR_STRING ) ||
                    ( dtlDataTypeGroup[aDataTypeArray[i]] == DTL_GROUP_LONGVARCHAR_STRING ) )
                {
                    aFuncArgDataTypeArray[i] = aDataTypeArray[i];
                }
                else
                {
                    /**
                     *  DTL_GROUP_NUMBER,
                     *  DTL_GROUP_BINARY_REAL,
                     *  DTL_GROUP_BINARY_INTEGER,
                     *  DTL_GROUP_ROWID,
                     */ 
                    aFuncArgDataTypeArray[i] = DTL_TYPE_VARCHAR;
                }
            }
            else if( ( aReturnGroupType == DTL_GROUP_BINARY_STRING ) ||
                     ( aReturnGroupType == DTL_GROUP_LONGVARBINARY_STRING ) )
            {
                if( ( dtlDataTypeGroup[aDataTypeArray[i]] == DTL_GROUP_BINARY_STRING ) ||
                    ( dtlDataTypeGroup[aDataTypeArray[i]] == DTL_GROUP_LONGVARBINARY_STRING ) )
                {
                    aFuncArgDataTypeArray[i] = aDataTypeArray[i];
                }
                else
                {
                    aFuncArgDataTypeArray[i] = dtlGroupRepresentType[aReturnGroupType];
                }
            }
            else
            {
                aFuncArgDataTypeArray[i] = dtlGroupRepresentType[aReturnGroupType];
            }
        }
    }

    return STL_SUCCESS;
}


/**
 * @brief Interval type에 대해 Indicator를 구한다.
 *
 * @param[in]  aDataTypeDefInfoArray         input data type info array 
 * @param[in]  aFuncArgDataTypeArrayCount    input data type array count
 * @param[out] aFuncArgDataTypeDefInfoArray  반환 될 data type def info array
 * @param[out] aFuncArgDataTypeArray         반환 될 Func Arg Data Type 
 * @param[out] aErrorStack                   에러스택
 */
stlStatus dtlSetIntervalTypeIndicator( dtlDataTypeDefInfo  * aDataTypeDefInfoArray,
                                       stlUInt16             aFuncArgDataTypeArrayCount,
                                       dtlDataTypeDefInfo  * aFuncArgDataTypeDefInfoArray,
                                       dtlDataType         * aFuncArgDataTypeArray,
                                       stlErrorStack       * aErrorStack )
{
    
    /**
     * @todo INTERVAL Indicator정리
     */
    
    stlUInt16  i;
    dtlIntervalIndicator sIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;
    
    /**
     * 함수의 argument 타입이 INTERVAL 타입인 경우,
     * conversion이 필요한 argument에 intervalIndicator 재설정.
     * 예) interval year = varchar 인 경우,
     *     varchar가 interval year로 변환될 수 있도록 intervalIndicator를 지정한다.
     *
     *     Interval 연산은 single field를 기준으로 varchar와 integer이 conversion 될 수 있다.
     *
     *     Interval Type의 경우에는 모든 Type들(Integer, CharString)의 Conversion Type이 Interval이다.     
     *     그러므로 Return Type인 aFuncArgDataTypeArray[0]을 하나만 검사하면 된다. 
     */
    
    if( ( aFuncArgDataTypeArray[0] == DTL_TYPE_INTERVAL_YEAR_TO_MONTH) ||
        ( aFuncArgDataTypeArray[0] == DTL_TYPE_INTERVAL_DAY_TO_SECOND) )
    {
        
        STL_TRY( dtlGetIntervalTypeIndicator( aDataTypeDefInfoArray,
                                              aFuncArgDataTypeArrayCount,
                                              & sIntervalIndicator,
                                              aErrorStack )
                 == STL_SUCCESS );
        
        for( i = 0; i < aFuncArgDataTypeArrayCount; i++ )
        {
            /*
             * 모든 Type의 Indicator적용,
             * 모든 Type들을 검사하여 최종 Indicator를 찾았다.
             * 모든 Type들의 Indicator를 통일하기 위해 모든 Type에 Indicator를 적용한다.
             * */
            aFuncArgDataTypeDefInfoArray[i].mIntervalIndicator = sIntervalIndicator;
        }
    }
    else
    {
        // Do Nothing
    }

    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;    
}


/**
 * @brief Interval type에 대해 Indicator를 얻는다.
 *
 * @param[in]  aDataTypeDefInfoArray    input data type info array 
 * @param[in]  aDataTypeArrayCount      input data type array count
 * @param[out] aIntervalIndicator       반환 될 Interval Indicator
 * @param[out] aErrorStack              에러스택
 */
stlStatus dtlGetIntervalTypeIndicator( dtlDataTypeDefInfo    * aDataTypeDefInfoArray,
                                       stlUInt16               aDataTypeArrayCount,
                                       dtlIntervalIndicator  * aIntervalIndicator,
                                       stlErrorStack         * aErrorStack )
{
    stlUInt16   i;
    dtlIntervalIndicator sIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;

    /**
     * 함수의 argument 타입이 INTERVAL 타입인 경우,
     * conversion이 필요한 argument에 intervalIndicator 재설정.
     * 예) interval year = varchar 인 경우,
     *     varchar가 interval year로 변환될 수 있도록 intervalIndicator를 지정한다.
     *
     *     Interval 연산은 single field를 기준으로 varchar와 integer이 conversion 될 수 있다.
     *
     *     Interval Type의 경우에는 모든 Type들(Integer, CharString)의 Conversion Type이 Interval이다.     
     *     그러므로 Return Type인 aFuncArgDataTypeArray[0]을 하나만 검사하면 된다. 
     */
    
    for( i = 0 ; i < aDataTypeArrayCount; i ++ )
    {           
        if( sIntervalIndicator == DTL_INTERVAL_INDICATOR_NA )
        {
            /*
             * 최초 IntervalIndicator 찾기,
             * sIntervalIndicator가 INDICATOR_NA일 경우는 Interval Type이 아닌,
             * VarChar, Integer Type만 Check한 상태로 Indicator를 알 수 없는 상태이다.
             * 이때는 Interval Type을 찾아 Indicator를 찾아줘야 한다.
             */ 
            sIntervalIndicator = aDataTypeDefInfoArray[i].mIntervalIndicator;
        }
        else
        {
            if( aDataTypeDefInfoArray[i].mIntervalIndicator != DTL_INTERVAL_INDICATOR_NA )
            {
                /* 최종 IntervalIndicator 찾기,
                 * 해당 범위는 하나 이상의 Interval type의 Indicator를 알고 있는 상태이다.
                 * 만약 중복되는 Interval Type이 있을 경우,
                 * Interval type간의 비교를 통해 최종 Indicator를 찾아야 한다.
                 * 
                 * 다음과 같은 경우가 있으므로 최종 Indicator를 찾는다.
                 *
                 * '10' Interval day = '10' Interval Second   =>   Indicator : Interval Day To Second
                 *       (Indicator)         (Indicator) 
                 */
                    
                sIntervalIndicator = 
                    dtlIntervalIndicatorContainArgIndicator 
                    [sIntervalIndicator][aDataTypeDefInfoArray[i].mIntervalIndicator];
            }
            else
            {
                /* Do Nothing */
            }
        }
    }

    *aIntervalIndicator = sIntervalIndicator;

    return STL_SUCCESS;
}


/** @} */
