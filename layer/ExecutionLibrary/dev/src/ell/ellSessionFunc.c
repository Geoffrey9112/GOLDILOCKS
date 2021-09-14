/*******************************************************************************
 * ellSessionFunc.c
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
 * @file ellSessionFunc.c
 * @brief Execution Library Layer Session Function Routines
 */

#include <ell.h>


/**
 * @ingroup ellSessionFunc
 * @{
 */


stlInt64 smlGetTransID( smlEnv* aEnv )
{
    return ((smlSessionEnv*) ELL_SESS_ENV( aEnv ) )->mTransId;
}

/* Current Schema */
const dtlFuncInfo ellFuncCurrentSchemaInfo =
{
    { STL_TRUE, }      /* 함수를 결정하는데 판단이 되는 argument index list */
};

const dtlFuncArgInfo ellFuncCurrentSchemaList[] =
{
    { ellFuncCurrentSchema,
      { DTL_TYPE_NATIVE_BIGINT },
      DTL_TYPE_VARCHAR
    },
     
    { NULL,
      { DTL_TYPE_NA, },
      DTL_TYPE_NA
    }
};


/* Transaction Date */
const dtlFuncInfo ellFuncTransactionDateInfo =
{
    { STL_TRUE, }      /* 함수를 결정하는데 판단이 되는 argument index list */
};

const dtlFuncArgInfo ellFuncTransactionDateList[] =
{
    { ellFuncTransactionDate,
      { DTL_TYPE_NATIVE_BIGINT },
      DTL_TYPE_DATE
    },
     
    { NULL,
      { DTL_TYPE_NA, },
      DTL_TYPE_NA
    }
};


/* Transaction Time */
const dtlFuncInfo ellFuncTransactionTimeInfo =
{
    { STL_TRUE, }      /* 함수를 결정하는데 판단이 되는 argument index list */
};

const dtlFuncArgInfo ellFuncTransactionTimeList[] =
{
    { ellFuncTransactionTime,
      { DTL_TYPE_NATIVE_BIGINT },
      DTL_TYPE_TIME_WITH_TIME_ZONE
    },
     
    { NULL,
      { DTL_TYPE_NA, },
      DTL_TYPE_NA
    }
};


/* Transaction Timestamp */
const dtlFuncInfo ellFuncTransactionTimestampInfo =
{
    { STL_TRUE, }      /* 함수를 결정하는데 판단이 되는 argument index list */
};

const dtlFuncArgInfo ellFuncTransactionTimestampList[] =
{
    { ellFuncTransactionTimestamp,
      { DTL_TYPE_NATIVE_BIGINT },
      DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE
    },
     
    { NULL,
      { DTL_TYPE_NA, },
      DTL_TYPE_NA
    }
};


/* Transaction LocalTime */
const dtlFuncInfo ellFuncTransactionLocalTimeInfo =
{
    { STL_TRUE, }      /* 함수를 결정하는데 판단이 되는 argument index list */
};

const dtlFuncArgInfo ellFuncTransactionLocalTimeList[] =
{
    { ellFuncTransactionLocalTime,
      { DTL_TYPE_NATIVE_BIGINT },
      DTL_TYPE_TIME
    },
     
    { NULL,
      { DTL_TYPE_NA, },
      DTL_TYPE_NA
    }
};

/* Transaction LocalTimestamp */
const dtlFuncInfo ellFuncTransactionLocalTimestampInfo =
{
    { STL_TRUE, }      /* 함수를 결정하는데 판단이 되는 argument index list */
};

const dtlFuncArgInfo ellFuncTransactionLocalTimestampList[] =
{
    { ellFuncTransactionLocalTimestamp,
      { DTL_TYPE_NATIVE_BIGINT },
      DTL_TYPE_TIMESTAMP
    },
     
    { NULL,
      { DTL_TYPE_NA, },
      DTL_TYPE_NA
    }
};

/* sysdate */
const dtlFuncInfo ellFuncSysDateInfo =
{
    { STL_TRUE, }      /* 함수를 결정하는데 판단이 되는 argument index list */
};

const dtlFuncArgInfo ellFuncSysDateList[] =
{
    { ellFuncSysDate,
      { DTL_TYPE_NATIVE_BIGINT },
      DTL_TYPE_DATE,
    },
     
    { NULL,
      { DTL_TYPE_NA, },
      DTL_TYPE_NA
    }
};

/* systime */
const dtlFuncInfo ellFuncSysTimeInfo =
{
    { STL_TRUE, }      /* 함수를 결정하는데 판단이 되는 argument index list */
};

const dtlFuncArgInfo ellFuncSysTimeList[] =
{
    { ellFuncSysTime,
      { DTL_TYPE_NATIVE_BIGINT },
      DTL_TYPE_TIME_WITH_TIME_ZONE,
    },
    
    { NULL,
      { DTL_TYPE_NA, },
      DTL_TYPE_NA
    }
};

/* systimestamp */
const dtlFuncInfo ellFuncSysTimestampInfo =
{
    { STL_TRUE, }      /* 함수를 결정하는데 판단이 되는 argument index list */
};

const dtlFuncArgInfo ellFuncSysTimestampList[] =
{
    { ellFuncSysTimestamp,
      { DTL_TYPE_NATIVE_BIGINT },
      DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,
    },
    
    { NULL,
      { DTL_TYPE_NA, },
      DTL_TYPE_NA
    }
};


/* statement view scn */
const dtlFuncInfo ellFuncStatementViewScnInfo =
{
    { STL_TRUE, }      /* 함수를 결정하는데 판단이 되는 argument index list */
};

const dtlFuncArgInfo ellFuncStatementViewScnList[] =
{
    { ellFuncStatementViewScn,
      { DTL_TYPE_NUMBER },
      DTL_TYPE_NUMBER
    },
     
    { NULL,
      { DTL_TYPE_NA, },
      DTL_TYPE_NA
    }
};


/*************************************************
 * SYSTEM
 ************************************************/

/**
 * @brief CATALOG_NAME() function에 대한 정보 얻기
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
stlStatus ellGetFuncInfoCatalogName( stlUInt16               aDataTypeArrayCount,
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
    STL_PARAM_VALIDATE( aDataTypeArrayCount == 0, aErrorStack );
    STL_PARAM_VALIDATE( aFuncArgDataTypeArrayCount == 0, aErrorStack );

    *aFuncResultDataType = DTL_TYPE_VARCHAR;

    aFuncResultDataTypeDefInfo->mArgNum1           = STL_MAX_SQL_IDENTIFIER_LENGTH;
    aFuncResultDataTypeDefInfo->mArgNum2           = DTL_SCALE_NA;
    aFuncResultDataTypeDefInfo->mStringLengthUnit  = DTL_STRING_LENGTH_UNIT_OCTETS;
    aFuncResultDataTypeDefInfo->mIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;

    *aFuncPtrIdx = 0;

    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief CATALOG_NAME() function pointer 얻기
 * @param[in]  aFuncPtrIdx       function pointer 얻기 위한 식별자
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus ellGetFuncPtrCatalogName( stlUInt32             aFuncPtrIdx,
                                    dtlBuiltInFuncPtr   * aFuncPtr,
                                    stlErrorStack       * aErrorStack )
{
    STL_PARAM_VALIDATE( aFuncPtrIdx == 0, aErrorStack );
    STL_PARAM_VALIDATE( aFuncPtr != NULL, aErrorStack );

    
    /**
     * output 설정
     */
    
    *aFuncPtr = ellFuncCatalogName;

        
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief CATALOG_NAME() 을 반환하는 함수
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> input argument가 없음.
 * @param[out] aResultValue      연산 결과 (결과타입 VARCHAR)
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus ellFuncCatalogName( stlUInt16        aInputArgumentCnt,
                              dtlValueEntry  * aInputArgument,
                              void           * aResultValue,
                              void           * aInfo,
                              dtlFuncVector  * aVectorFunc,
                              void           * aVectorArg,
                              void           * aEnv )
{
    dtlDataValue       * sResultValue;
    stlChar            * sDBName;
    stlBool              sSucceessWithInfo;
    dtlValueBlockList  * sValueBlock;
    

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 0, (stlErrorStack *)aEnv );
    DTL_PARAM_VALIDATE( aInfo != NULL, (stlErrorStack *)aEnv );

    sResultValue = (dtlDataValue *)aResultValue;
    sValueBlock  = (knlValueBlockList *)aInfo;
    

    /**
     * Result 생성 
     */

    if( DTL_IS_NULL( KNL_GET_BLOCK_FIRST_DATA_VALUE( sValueBlock ) ) == STL_TRUE )
    {
        sDBName = ellGetDbCatalogName();

        STL_TRY( dtlSetValueFromString( DTL_TYPE_VARCHAR,
                                        sDBName,
                                        stlStrlen( sDBName ),
                                        STL_MAX_SQL_IDENTIFIER_LENGTH,
                                        DTL_SCALE_NA,
                                        DTL_STRING_LENGTH_UNIT_OCTETS,
                                        DTL_INTERVAL_INDICATOR_NA,
                                        STL_MAX_SQL_IDENTIFIER_LENGTH,
                                        sResultValue,
                                        & sSucceessWithInfo,
                                        KNL_DT_VECTOR( aEnv ),
                                        aEnv,
                                        KNL_DT_VECTOR( aEnv ),
                                        aEnv,
                                        ELL_ERROR_STACK( aEnv ) )
                 == STL_SUCCESS );

        STL_TRY( knlCopyDataValue( sResultValue,
                                   KNL_GET_BLOCK_FIRST_DATA_VALUE( sValueBlock ),
                                   aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( knlCopyDataValue( KNL_GET_BLOCK_FIRST_DATA_VALUE( sValueBlock ),
                                   sResultValue,
                                   aEnv )
                 == STL_SUCCESS );
    }

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief VERSION() function에 대한 정보 얻기
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
stlStatus ellGetFuncInfoVersion( stlUInt16               aDataTypeArrayCount,
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
    STL_PARAM_VALIDATE( aDataTypeArrayCount == 0, aErrorStack );
    STL_PARAM_VALIDATE( aFuncArgDataTypeArrayCount == 0, aErrorStack );

    *aFuncResultDataType = DTL_TYPE_VARCHAR;

    aFuncResultDataTypeDefInfo->mArgNum1           = STL_MAX_SQL_IDENTIFIER_LENGTH;
    aFuncResultDataTypeDefInfo->mArgNum2           = DTL_SCALE_NA;
    aFuncResultDataTypeDefInfo->mStringLengthUnit  = DTL_STRING_LENGTH_UNIT_OCTETS;
    aFuncResultDataTypeDefInfo->mIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;

    *aFuncPtrIdx = 0;

    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief VERSION() function pointer 얻기
 * @param[in]  aFuncPtrIdx       function pointer 얻기 위한 식별자
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus ellGetFuncPtrVersion( stlUInt32             aFuncPtrIdx,
                                dtlBuiltInFuncPtr   * aFuncPtr,
                                stlErrorStack       * aErrorStack )
{
    STL_PARAM_VALIDATE( aFuncPtrIdx == 0, aErrorStack );
    STL_PARAM_VALIDATE( aFuncPtr != NULL, aErrorStack );

    
    /**
     * output 설정
     */
    
    *aFuncPtr = ellFuncVersion;

        
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief VERSION() 을 반환하는 함수
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> input argument가 없음.
 * @param[out] aResultValue      연산 결과 (결과타입 VARCHAR)
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus ellFuncVersion( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aInfo,
                          dtlFuncVector  * aVectorFunc,
                          void           * aVectorArg,
                          void           * aEnv )
{
    dtlDataValue       * sResultValue;
    stlBool              sSucceessWithInfo;
    stlChar              sVersion[ STL_MAX_SQL_IDENTIFIER_LENGTH ] = {0,};
    dtlValueBlockList  * sValueBlock;
    

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 0, (stlErrorStack *)aEnv );
    DTL_PARAM_VALIDATE( aInfo != NULL, (stlErrorStack *)aEnv );

    sResultValue = (dtlDataValue *)aResultValue;
    sValueBlock  = (knlValueBlockList *)aInfo;


    /**
     * Result 생성 
     */
    
    if( DTL_IS_NULL( KNL_GET_BLOCK_FIRST_DATA_VALUE( sValueBlock ) ) == STL_TRUE )
    {
        stlGetVersionString( sVersion, STL_MAX_SQL_IDENTIFIER_LENGTH );

        STL_TRY( dtlSetValueFromString( DTL_TYPE_VARCHAR,
                                        sVersion,
                                        stlStrlen( sVersion ),
                                        STL_MAX_SQL_IDENTIFIER_LENGTH,
                                        DTL_SCALE_NA,
                                        DTL_STRING_LENGTH_UNIT_OCTETS,
                                        DTL_INTERVAL_INDICATOR_NA,
                                        STL_MAX_SQL_IDENTIFIER_LENGTH,
                                        sResultValue,
                                        & sSucceessWithInfo,
                                        KNL_DT_VECTOR( aEnv ),
                                        aEnv,
                                        KNL_DT_VECTOR( aEnv ),
                                        aEnv,
                                        ELL_ERROR_STACK( aEnv ) )
                 == STL_SUCCESS );

        STL_TRY( knlCopyDataValue( sResultValue,
                                   KNL_GET_BLOCK_FIRST_DATA_VALUE( sValueBlock ),
                                   aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( knlCopyDataValue( KNL_GET_BLOCK_FIRST_DATA_VALUE( sValueBlock ),
                                   sResultValue,
                                   aEnv )
                 == STL_SUCCESS );
    }

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/*************************************************
 * SESSION
 ************************************************/

/**
 * @brief SESSION_ID() function에 대한 정보 얻기
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
stlStatus ellGetFuncInfoSessionID( stlUInt16               aDataTypeArrayCount,
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
    STL_PARAM_VALIDATE( aDataTypeArrayCount == 0, aErrorStack );
    STL_PARAM_VALIDATE( aFuncArgDataTypeArrayCount == 0, aErrorStack );

    *aFuncResultDataType = DTL_TYPE_NATIVE_BIGINT;

    aFuncResultDataTypeDefInfo->mArgNum1           = DTL_PRECISION_NA;
    aFuncResultDataTypeDefInfo->mArgNum2           = DTL_SCALE_NA;
    aFuncResultDataTypeDefInfo->mStringLengthUnit  = DTL_STRING_LENGTH_UNIT_NA;
    aFuncResultDataTypeDefInfo->mIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;

    *aFuncPtrIdx = 0;

    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief SESSION_ID() function pointer 얻기
 * @param[in]  aFuncPtrIdx       function pointer 얻기 위한 식별자
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus ellGetFuncPtrSessionID( stlUInt32             aFuncPtrIdx,
                                  dtlBuiltInFuncPtr   * aFuncPtr,
                                  stlErrorStack       * aErrorStack )
{
    STL_PARAM_VALIDATE( aFuncPtrIdx == 0, aErrorStack );
    STL_PARAM_VALIDATE( aFuncPtr != NULL, aErrorStack );

    
    /**
     * output 설정
     */
    
    *aFuncPtr = ellFuncSessionID;

        
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief SESSION_ID() 을 반환하는 함수
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> input argument가 없음.
 * @param[out] aResultValue      연산 결과 (결과타입 NATIVE_BIGINT)
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus ellFuncSessionID( stlUInt16        aInputArgumentCnt,
                            dtlValueEntry  * aInputArgument,
                            void           * aResultValue,
                            void           * aInfo,
                            dtlFuncVector  * aVectorFunc,
                            void           * aVectorArg,
                            void           * aEnv )
{
    dtlDataValue       * sResultValue;
    stlBool              sSucceessWithInfo;
    stlUInt32            sSessionID;
    dtlValueBlockList  * sValueBlock;
    
    
    DTL_PARAM_VALIDATE( aInputArgumentCnt == 0, (stlErrorStack *)aEnv );
    DTL_PARAM_VALIDATE( aInfo != NULL, (stlErrorStack *)aEnv );

    sResultValue = (dtlDataValue *)aResultValue;
    sValueBlock  = (knlValueBlockList *)aInfo;


    /**
     * Result 생성 
     */
    
    if( DTL_IS_NULL( KNL_GET_BLOCK_FIRST_DATA_VALUE( sValueBlock ) ) == STL_TRUE )
    {
        STL_TRY( knlGetSessionEnvId( (void*) ELL_SESS_ENV(aEnv),
                                     & sSessionID,
                                     aEnv )
                 == STL_SUCCESS );

        STL_TRY( dtlSetValueFromInteger( DTL_TYPE_NATIVE_BIGINT,
                                         sSessionID,
                                         DTL_PRECISION_NA,
                                         DTL_SCALE_NA,
                                         DTL_STRING_LENGTH_UNIT_NA,
                                         DTL_INTERVAL_INDICATOR_NA,
                                         DTL_NATIVE_BIGINT_SIZE,
                                         sResultValue,
                                         & sSucceessWithInfo,
                                         KNL_DT_VECTOR( aEnv ),
                                         aEnv,
                                         ELL_ERROR_STACK( aEnv ) )
                 == STL_SUCCESS );

        STL_TRY( knlCopyDataValue( sResultValue,
                                   KNL_GET_BLOCK_FIRST_DATA_VALUE( sValueBlock ),
                                   aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( knlCopyDataValue( KNL_GET_BLOCK_FIRST_DATA_VALUE( sValueBlock ),
                                   sResultValue,
                                   aEnv )
                 == STL_SUCCESS );
    }

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief SESSION_SERIAL() function에 대한 정보 얻기
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
stlStatus ellGetFuncInfoSessionSerial( stlUInt16               aDataTypeArrayCount,
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
    STL_PARAM_VALIDATE( aDataTypeArrayCount == 0, aErrorStack );
    STL_PARAM_VALIDATE( aFuncArgDataTypeArrayCount == 0, aErrorStack );

    *aFuncResultDataType = DTL_TYPE_NATIVE_BIGINT;

    aFuncResultDataTypeDefInfo->mArgNum1           = DTL_PRECISION_NA;
    aFuncResultDataTypeDefInfo->mArgNum2           = DTL_SCALE_NA;
    aFuncResultDataTypeDefInfo->mStringLengthUnit  = DTL_STRING_LENGTH_UNIT_NA;
    aFuncResultDataTypeDefInfo->mIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;

    *aFuncPtrIdx = 0;

    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief SESSION_SERIAL() function pointer 얻기
 * @param[in]  aFuncPtrIdx       function pointer 얻기 위한 식별자
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus ellGetFuncPtrSessionSerial( stlUInt32             aFuncPtrIdx,
                                      dtlBuiltInFuncPtr   * aFuncPtr,
                                      stlErrorStack       * aErrorStack )
{
    STL_PARAM_VALIDATE( aFuncPtrIdx == 0, aErrorStack );
    STL_PARAM_VALIDATE( aFuncPtr != NULL, aErrorStack );

    
    /**
     * output 설정
     */
    
    *aFuncPtr = ellFuncSessionSerial;

        
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief SESSION_SERIAL() 을 반환하는 함수
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> input argument가 없음.
 * @param[out] aResultValue      연산 결과 (결과타입 NATIVE_BIGINT)
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus ellFuncSessionSerial( stlUInt16        aInputArgumentCnt,
                                dtlValueEntry  * aInputArgument,
                                void           * aResultValue,
                                void           * aInfo,
                                dtlFuncVector  * aVectorFunc,
                                void           * aVectorArg,
                                void           * aEnv )
{
    dtlDataValue       * sResultValue;
    stlBool              sSucceessWithInfo;
    stlInt64             sSessionSerial = 0;
    dtlValueBlockList  * sValueBlock;
    
    
    DTL_PARAM_VALIDATE( aInputArgumentCnt == 0, (stlErrorStack *)aEnv );
    DTL_PARAM_VALIDATE( aInfo != NULL, (stlErrorStack *)aEnv );

    sResultValue = (dtlDataValue *)aResultValue;
    sValueBlock  = (knlValueBlockList *)aInfo;


    /**
     * Result 생성 
     */
    
    if( DTL_IS_NULL( KNL_GET_BLOCK_FIRST_DATA_VALUE( sValueBlock ) ) == STL_TRUE )
    {
        sSessionSerial = KNL_GET_SESSION_SEQ( ELL_SESS_ENV(aEnv) );

        STL_TRY( dtlSetValueFromInteger( DTL_TYPE_NATIVE_BIGINT,
                                         sSessionSerial,
                                         DTL_PRECISION_NA,
                                         DTL_SCALE_NA,
                                         DTL_STRING_LENGTH_UNIT_NA,
                                         DTL_INTERVAL_INDICATOR_NA,
                                         DTL_NATIVE_BIGINT_SIZE,
                                         sResultValue,
                                         & sSucceessWithInfo,
                                         KNL_DT_VECTOR( aEnv ),
                                         aEnv,
                                         ELL_ERROR_STACK( aEnv ) )
                 == STL_SUCCESS );

        STL_TRY( knlCopyDataValue( sResultValue,
                                   KNL_GET_BLOCK_FIRST_DATA_VALUE( sValueBlock ),
                                   aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( knlCopyDataValue( KNL_GET_BLOCK_FIRST_DATA_VALUE( sValueBlock ),
                                   sResultValue,
                                   aEnv )
                 == STL_SUCCESS );
    }

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief USER_ID() function에 대한 정보 얻기
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
stlStatus ellGetFuncInfoUserID( stlUInt16               aDataTypeArrayCount,
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
    STL_PARAM_VALIDATE( aDataTypeArrayCount == 0, aErrorStack );
    STL_PARAM_VALIDATE( aFuncArgDataTypeArrayCount == 0, aErrorStack );

    *aFuncResultDataType = DTL_TYPE_NATIVE_BIGINT;

    aFuncResultDataTypeDefInfo->mArgNum1           = DTL_PRECISION_NA;
    aFuncResultDataTypeDefInfo->mArgNum2           = DTL_SCALE_NA;
    aFuncResultDataTypeDefInfo->mStringLengthUnit  = DTL_STRING_LENGTH_UNIT_NA;
    aFuncResultDataTypeDefInfo->mIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;

    *aFuncPtrIdx = 0;

    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief USER_ID() function pointer 얻기
 * @param[in]  aFuncPtrIdx       function pointer 얻기 위한 식별자
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus ellGetFuncPtrUserID( stlUInt32             aFuncPtrIdx,
                               dtlBuiltInFuncPtr   * aFuncPtr,
                               stlErrorStack       * aErrorStack )
{
    STL_PARAM_VALIDATE( aFuncPtrIdx == 0, aErrorStack );
    STL_PARAM_VALIDATE( aFuncPtr != NULL, aErrorStack );

    
    /**
     * output 설정
     */
    
    *aFuncPtr = ellFuncUserID;

        
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief USER_ID() 을 반환하는 함수
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> input argument가 없음.
 * @param[out] aResultValue      연산 결과 (결과타입 NATIVE_BIGINT)
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus ellFuncUserID( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aInfo,
                         dtlFuncVector  * aVectorFunc,
                         void           * aVectorArg,
                         void           * aEnv )
{
    dtlDataValue       * sResultValue;
    stlBool              sSucceessWithInfo;
    ellDictHandle      * sAuthHandle;
    stlInt64             sUserID;
    dtlValueBlockList  * sValueBlock;
    
    
    DTL_PARAM_VALIDATE( aInputArgumentCnt == 0, (stlErrorStack *)aEnv );
    DTL_PARAM_VALIDATE( aInfo != NULL, (stlErrorStack *)aEnv );

    sResultValue = (dtlDataValue *)aResultValue;
    sValueBlock  = (knlValueBlockList *)aInfo;
    

    /**
     * Result 생성 
     */
    
    if( DTL_IS_NULL( KNL_GET_BLOCK_FIRST_DATA_VALUE( sValueBlock ) ) == STL_TRUE )
    {
        sAuthHandle = ellGetCurrentUserHandle( aEnv );
        sUserID = ellGetAuthID(sAuthHandle);

        STL_TRY( dtlSetValueFromInteger( DTL_TYPE_NATIVE_BIGINT,
                                         sUserID,
                                         DTL_PRECISION_NA,
                                         DTL_SCALE_NA,
                                         DTL_STRING_LENGTH_UNIT_NA,
                                         DTL_INTERVAL_INDICATOR_NA,
                                         DTL_NATIVE_BIGINT_SIZE,
                                         sResultValue,
                                         & sSucceessWithInfo,
                                         KNL_DT_VECTOR( aEnv ),
                                         aEnv,
                                         ELL_ERROR_STACK( aEnv ) )
                 == STL_SUCCESS );

        STL_TRY( knlCopyDataValue( sResultValue,
                                   KNL_GET_BLOCK_FIRST_DATA_VALUE( sValueBlock ),
                                   aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( knlCopyDataValue( KNL_GET_BLOCK_FIRST_DATA_VALUE( sValueBlock ),
                                   sResultValue,
                                   aEnv )
                 == STL_SUCCESS );
    }

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief CURRENT_USER() function에 대한 정보 얻기
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
stlStatus ellGetFuncInfoCurrentUser( stlUInt16               aDataTypeArrayCount,
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
    STL_PARAM_VALIDATE( aDataTypeArrayCount == 0, aErrorStack );
    STL_PARAM_VALIDATE( aFuncArgDataTypeArrayCount == 0, aErrorStack );

    *aFuncResultDataType = DTL_TYPE_VARCHAR;

    aFuncResultDataTypeDefInfo->mArgNum1           = STL_MAX_SQL_IDENTIFIER_LENGTH;
    aFuncResultDataTypeDefInfo->mArgNum2           = DTL_SCALE_NA;
    aFuncResultDataTypeDefInfo->mStringLengthUnit  = DTL_STRING_LENGTH_UNIT_OCTETS;
    aFuncResultDataTypeDefInfo->mIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;

    *aFuncPtrIdx = 0;

    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief CURRENT_USER() function pointer 얻기
 * @param[in]  aFuncPtrIdx       function pointer 얻기 위한 식별자
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus ellGetFuncPtrCurrentUser( stlUInt32             aFuncPtrIdx,
                                    dtlBuiltInFuncPtr   * aFuncPtr,
                                    stlErrorStack       * aErrorStack )
{
    STL_PARAM_VALIDATE( aFuncPtrIdx == 0, aErrorStack );
    STL_PARAM_VALIDATE( aFuncPtr != NULL, aErrorStack );

    
    /**
     * output 설정
     */
    
    *aFuncPtr = ellFuncCurrentUser;

        
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief CURRENT_USER() 을 반환하는 함수
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> input argument가 없음.
 * @param[out] aResultValue      연산 결과 (결과타입 VARCHAR)
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus ellFuncCurrentUser( stlUInt16        aInputArgumentCnt,
                              dtlValueEntry  * aInputArgument,
                              void           * aResultValue,
                              void           * aInfo,
                              dtlFuncVector  * aVectorFunc,
                              void           * aVectorArg,
                              void           * aEnv )
{
    dtlDataValue       * sResultValue;
    stlBool              sSucceessWithInfo;
    ellDictHandle      * sAuthHandle;
    stlChar            * sUserName;
    dtlValueBlockList  * sValueBlock;
    
    
    DTL_PARAM_VALIDATE( aInputArgumentCnt == 0, (stlErrorStack *)aEnv );
    DTL_PARAM_VALIDATE( aInfo != NULL, (stlErrorStack *)aEnv );

    sResultValue = (dtlDataValue *)aResultValue;
    sValueBlock  = (knlValueBlockList *)aInfo;
    

    /**
     * Result 생성 
     */
    
    if( DTL_IS_NULL( KNL_GET_BLOCK_FIRST_DATA_VALUE( sValueBlock ) ) == STL_TRUE )
    {
        sAuthHandle = ellGetCurrentUserHandle( aEnv );
        sUserName = ellGetAuthName(sAuthHandle);

        STL_TRY( dtlSetValueFromString( DTL_TYPE_VARCHAR,
                                        sUserName,
                                        stlStrlen( sUserName ),
                                        STL_MAX_SQL_IDENTIFIER_LENGTH,
                                        DTL_SCALE_NA,
                                        DTL_STRING_LENGTH_UNIT_OCTETS,
                                        DTL_INTERVAL_INDICATOR_NA,
                                        STL_MAX_SQL_IDENTIFIER_LENGTH,
                                        sResultValue,
                                        & sSucceessWithInfo,
                                        KNL_DT_VECTOR( aEnv ),
                                        aEnv,
                                        KNL_DT_VECTOR( aEnv ),
                                        aEnv,
                                        ELL_ERROR_STACK( aEnv ) )
                 == STL_SUCCESS );

        STL_TRY( knlCopyDataValue( sResultValue,
                                   KNL_GET_BLOCK_FIRST_DATA_VALUE( sValueBlock ),
                                   aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( knlCopyDataValue( KNL_GET_BLOCK_FIRST_DATA_VALUE( sValueBlock ),
                                   sResultValue,
                                   aEnv )
                 == STL_SUCCESS );
    }

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief SESSION_USER() function에 대한 정보 얻기
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
stlStatus ellGetFuncInfoSessionUser( stlUInt16               aDataTypeArrayCount,
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
    STL_PARAM_VALIDATE( aDataTypeArrayCount == 0, aErrorStack );
    STL_PARAM_VALIDATE( aFuncArgDataTypeArrayCount == 0, aErrorStack );

    *aFuncResultDataType = DTL_TYPE_VARCHAR;

    aFuncResultDataTypeDefInfo->mArgNum1           = STL_MAX_SQL_IDENTIFIER_LENGTH;
    aFuncResultDataTypeDefInfo->mArgNum2           = DTL_SCALE_NA;
    aFuncResultDataTypeDefInfo->mStringLengthUnit  = DTL_STRING_LENGTH_UNIT_OCTETS;
    aFuncResultDataTypeDefInfo->mIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;

    *aFuncPtrIdx = 0;

    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief SESSION_USER() function pointer 얻기
 * @param[in]  aFuncPtrIdx       function pointer 얻기 위한 식별자
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus ellGetFuncPtrSessionUser( stlUInt32             aFuncPtrIdx,
                                    dtlBuiltInFuncPtr   * aFuncPtr,
                                    stlErrorStack       * aErrorStack )
{
    STL_PARAM_VALIDATE( aFuncPtrIdx == 0, aErrorStack );
    STL_PARAM_VALIDATE( aFuncPtr != NULL, aErrorStack );

    
    /**
     * output 설정
     */
    
    *aFuncPtr = ellFuncSessionUser;

        
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief SESSION_USER() 을 반환하는 함수
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> input argument가 없음.
 * @param[out] aResultValue      연산 결과 (결과타입 VARCHAR)
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus ellFuncSessionUser( stlUInt16        aInputArgumentCnt,
                              dtlValueEntry  * aInputArgument,
                              void           * aResultValue,
                              void           * aInfo,
                              dtlFuncVector  * aVectorFunc,
                              void           * aVectorArg,
                              void           * aEnv )
{
    dtlDataValue       * sResultValue;
    stlBool              sSucceessWithInfo;
    ellDictHandle      * sAuthHandle;
    stlChar            * sUserName;
    dtlValueBlockList  * sValueBlock;

    
    DTL_PARAM_VALIDATE( aInputArgumentCnt == 0, (stlErrorStack *)aEnv );
    DTL_PARAM_VALIDATE( aInfo != NULL, (stlErrorStack *)aEnv );

    sResultValue = (dtlDataValue *)aResultValue;
    sValueBlock  = (knlValueBlockList *)aInfo;


    /**
     * Result 생성 
     */
    
    if( DTL_IS_NULL( KNL_GET_BLOCK_FIRST_DATA_VALUE( sValueBlock ) ) == STL_TRUE )
    {
        sAuthHandle = ellGetSessionUserHandle( aEnv );
        sUserName = ellGetAuthName(sAuthHandle);

        STL_TRY( dtlSetValueFromString( DTL_TYPE_VARCHAR,
                                        sUserName,
                                        stlStrlen( sUserName ),
                                        STL_MAX_SQL_IDENTIFIER_LENGTH,
                                        DTL_SCALE_NA,
                                        DTL_STRING_LENGTH_UNIT_OCTETS,
                                        DTL_INTERVAL_INDICATOR_NA,
                                        STL_MAX_SQL_IDENTIFIER_LENGTH,
                                        sResultValue,
                                        & sSucceessWithInfo,
                                        KNL_DT_VECTOR( aEnv ),
                                        aEnv,
                                        KNL_DT_VECTOR( aEnv ),
                                        aEnv,
                                        ELL_ERROR_STACK( aEnv ) )
                 == STL_SUCCESS );

        STL_TRY( knlCopyDataValue( sResultValue,
                                   KNL_GET_BLOCK_FIRST_DATA_VALUE( sValueBlock ),
                                   aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( knlCopyDataValue( KNL_GET_BLOCK_FIRST_DATA_VALUE( sValueBlock ),
                                   sResultValue,
                                   aEnv )
                 == STL_SUCCESS );
    }

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief LOGON_USER() function에 대한 정보 얻기
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
stlStatus ellGetFuncInfoLogonUser( stlUInt16               aDataTypeArrayCount,
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
    STL_PARAM_VALIDATE( aDataTypeArrayCount == 0, aErrorStack );
    STL_PARAM_VALIDATE( aFuncArgDataTypeArrayCount == 0, aErrorStack );

    *aFuncResultDataType = DTL_TYPE_VARCHAR;

    aFuncResultDataTypeDefInfo->mArgNum1           = STL_MAX_SQL_IDENTIFIER_LENGTH;
    aFuncResultDataTypeDefInfo->mArgNum2           = DTL_SCALE_NA;
    aFuncResultDataTypeDefInfo->mStringLengthUnit  = DTL_STRING_LENGTH_UNIT_OCTETS;
    aFuncResultDataTypeDefInfo->mIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;

    *aFuncPtrIdx = 0;

    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief LOGON_USER() function pointer 얻기
 * @param[in]  aFuncPtrIdx       function pointer 얻기 위한 식별자
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus ellGetFuncPtrLogonUser( stlUInt32             aFuncPtrIdx,
                                  dtlBuiltInFuncPtr   * aFuncPtr,
                                  stlErrorStack       * aErrorStack )
{
    STL_PARAM_VALIDATE( aFuncPtrIdx == 0, aErrorStack );
    STL_PARAM_VALIDATE( aFuncPtr != NULL, aErrorStack );

    
    /**
     * output 설정
     */
    
    *aFuncPtr = ellFuncLogonUser;

        
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief LOGON_USER() 을 반환하는 함수
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> input argument가 없음.
 * @param[out] aResultValue      연산 결과 (결과타입 VARCHAR)
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus ellFuncLogonUser( stlUInt16        aInputArgumentCnt,
                            dtlValueEntry  * aInputArgument,
                            void           * aResultValue,
                            void           * aInfo,
                            dtlFuncVector  * aVectorFunc,
                            void           * aVectorArg,
                            void           * aEnv )
{
    dtlDataValue       * sResultValue;
    stlBool              sSucceessWithInfo;
    ellDictHandle      * sAuthHandle;
    stlChar            * sUserName;
    dtlValueBlockList  * sValueBlock;

    
    DTL_PARAM_VALIDATE( aInputArgumentCnt == 0, (stlErrorStack *)aEnv );
    DTL_PARAM_VALIDATE( aInfo != NULL, (stlErrorStack *)aEnv );

    sResultValue = (dtlDataValue *)aResultValue;
    sValueBlock  = (knlValueBlockList *)aInfo;


    /**
     * Result 생성 
     */
    
    if( DTL_IS_NULL( KNL_GET_BLOCK_FIRST_DATA_VALUE( sValueBlock ) ) == STL_TRUE )
    {
        sAuthHandle = ellGetLoginUserHandle( aEnv );
        sUserName = ellGetAuthName(sAuthHandle);

        STL_TRY( dtlSetValueFromString( DTL_TYPE_VARCHAR,
                                        sUserName,
                                        stlStrlen( sUserName ),
                                        STL_MAX_SQL_IDENTIFIER_LENGTH,
                                        DTL_SCALE_NA,
                                        DTL_STRING_LENGTH_UNIT_OCTETS,
                                        DTL_INTERVAL_INDICATOR_NA,
                                        STL_MAX_SQL_IDENTIFIER_LENGTH,
                                        sResultValue,
                                        & sSucceessWithInfo,
                                        KNL_DT_VECTOR( aEnv ),
                                        aEnv,
                                        KNL_DT_VECTOR( aEnv ),
                                        aEnv,
                                        ELL_ERROR_STACK( aEnv ) )
                 == STL_SUCCESS );

        STL_TRY( knlCopyDataValue( sResultValue,
                                   KNL_GET_BLOCK_FIRST_DATA_VALUE( sValueBlock ),
                                   aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( knlCopyDataValue( KNL_GET_BLOCK_FIRST_DATA_VALUE( sValueBlock ),
                                   sResultValue,
                                   aEnv )
                 == STL_SUCCESS );
    }

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief CURRENT_SCHEMA() function에 대한 정보 얻기
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
stlStatus ellGetFuncInfoCurrentSchema( stlUInt16               aDataTypeArrayCount,
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
    STL_PARAM_VALIDATE( aDataTypeArrayCount == 0, aErrorStack );
    STL_PARAM_VALIDATE( aFuncArgDataTypeArrayCount == 0, aErrorStack );

    *aFuncResultDataType = DTL_TYPE_VARCHAR;

    aFuncResultDataTypeDefInfo->mArgNum1           = STL_MAX_SQL_IDENTIFIER_LENGTH;
    aFuncResultDataTypeDefInfo->mArgNum2           = DTL_SCALE_NA;
    aFuncResultDataTypeDefInfo->mStringLengthUnit  = DTL_STRING_LENGTH_UNIT_OCTETS;
    aFuncResultDataTypeDefInfo->mIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;

    *aFuncPtrIdx = 0;

    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief CURRENT_SCHEMA() function pointer 얻기
 * @param[in]  aFuncPtrIdx       function pointer 얻기 위한 식별자
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus ellGetFuncPtrCurrentSchema( stlUInt32             aFuncPtrIdx,
                                      dtlBuiltInFuncPtr   * aFuncPtr,
                                      stlErrorStack       * aErrorStack )
{
    STL_PARAM_VALIDATE( aFuncPtrIdx == 0, aErrorStack );
    STL_PARAM_VALIDATE( aFuncPtr != NULL, aErrorStack );

    
    /**
     * output 설정
     */
    
    *aFuncPtr = ellFuncCurrentSchema;

        
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief CURRENT_SCHEMA() 을 반환하는 함수
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> input argument가 없음.
 * @param[out] aResultValue      연산 결과 (결과타입 NATIVE_BIGINT)
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus ellFuncCurrentSchema( stlUInt16        aInputArgumentCnt,
                                dtlValueEntry  * aInputArgument,
                                void           * aResultValue,
                                void           * aInfo,
                                dtlFuncVector  * aVectorFunc,
                                void           * aVectorArg,
                                void           * aEnv )
{
    dtlDataValue       * sResultValue;
    stlBool              sSucceessWithInfo;
    ellDictHandle      * sAuthHandle;
    stlChar            * sSchemaName;
    ellDictHandle        sSchemaHandle;
    dtlValueBlockList  * sValueBlock;

    
    DTL_PARAM_VALIDATE( aInputArgumentCnt == 0, (stlErrorStack *)aEnv );
    DTL_PARAM_VALIDATE( aInfo != NULL, (stlErrorStack *)aEnv );

    sResultValue = (dtlDataValue *)aResultValue;
    sValueBlock  = (knlValueBlockList *)aInfo;


    /**
     * Result 생성 
     */
    
    if( DTL_IS_NULL( KNL_GET_BLOCK_FIRST_DATA_VALUE( sValueBlock ) ) == STL_TRUE )
    {
        ellInitDictHandle( & sSchemaHandle );
        sAuthHandle = ellGetCurrentUserHandle( aEnv );
    
        STL_TRY( ellGetAuthFirstSchemaDictHandle( smlGetTransID( SML_ENV( aEnv ) ),
                                                  SML_MAXIMUM_STABLE_SCN,
                                                  sAuthHandle,
                                                  & sSchemaHandle,
                                                  aEnv )
                 == STL_SUCCESS );

        sSchemaName = ellGetSchemaName( & sSchemaHandle );

        STL_TRY( dtlSetValueFromString( DTL_TYPE_VARCHAR,
                                        sSchemaName,
                                        stlStrlen( sSchemaName ),
                                        STL_MAX_SQL_IDENTIFIER_LENGTH,
                                        DTL_SCALE_NA,
                                        DTL_STRING_LENGTH_UNIT_OCTETS,
                                        DTL_INTERVAL_INDICATOR_NA,
                                        STL_MAX_SQL_IDENTIFIER_LENGTH,
                                        sResultValue,
                                        & sSucceessWithInfo,
                                        KNL_DT_VECTOR( aEnv ),
                                        aEnv,
                                        KNL_DT_VECTOR( aEnv ),
                                        aEnv,
                                        ELL_ERROR_STACK( aEnv ) )
                 == STL_SUCCESS );

        STL_TRY( knlCopyDataValue( sResultValue,
                                   KNL_GET_BLOCK_FIRST_DATA_VALUE( sValueBlock ),
                                   aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( knlCopyDataValue( KNL_GET_BLOCK_FIRST_DATA_VALUE( sValueBlock ),
                                   sResultValue,
                                   aEnv )
                 == STL_SUCCESS );
    }

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/*************************************************
 * DATE & TIME
 ************************************************/

/**
 * @brief CLOCK_DATE(), ROW_DATE() function에 대한 정보 얻기
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
 *
 * @remark ROW_DATE() alias는 제거되어야 함.
 */
stlStatus ellGetFuncInfoClockDate( stlUInt16               aDataTypeArrayCount,
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
    STL_PARAM_VALIDATE( aDataTypeArrayCount == 0, aErrorStack );
    STL_PARAM_VALIDATE( aFuncArgDataTypeArrayCount == 0, aErrorStack );

    *aFuncResultDataType = DTL_TYPE_DATE;

    aFuncResultDataTypeDefInfo->mArgNum1           = DTL_PRECISION_NA;
    aFuncResultDataTypeDefInfo->mArgNum2           = DTL_SCALE_NA;
    aFuncResultDataTypeDefInfo->mStringLengthUnit  = DTL_STRING_LENGTH_UNIT_NA;
    aFuncResultDataTypeDefInfo->mIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;

    *aFuncPtrIdx = 0;

    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief CLOCK_DATE(), ROW_DATE() function pointer 얻기
 * @param[in]  aFuncPtrIdx       function pointer 얻기 위한 식별자
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 *
 * @remark ROW_DATE() alias는 제거되어야 함.
 */
stlStatus ellGetFuncPtrClockDate( stlUInt32             aFuncPtrIdx,
                                  dtlBuiltInFuncPtr   * aFuncPtr,
                                  stlErrorStack       * aErrorStack )
{
    STL_PARAM_VALIDATE( aFuncPtrIdx == 0, aErrorStack );
    STL_PARAM_VALIDATE( aFuncPtr != NULL, aErrorStack );

    
    /**
     * output 설정
     */
    
    *aFuncPtr = ellFuncClockDate;

        
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief CLOCK_DATE(), ROW_DATE() 을 반환하는 함수
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> input argument가 없음.
 * @param[out] aResultValue      연산 결과 (결과타입 DATE)
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 *
 * @remark ROW_DATE() alias는 제거되어야 함.
 */
stlStatus ellFuncClockDate( stlUInt16        aInputArgumentCnt,
                            dtlValueEntry  * aInputArgument,
                            void           * aResultValue,
                            void           * aInfo,
                            dtlFuncVector  * aVectorFunc,
                            void           * aVectorArg,
                            void           * aEnv )
{
    dtlDataValue   * sResultValue;

    
    DTL_PARAM_VALIDATE( aInputArgumentCnt == 0, (stlErrorStack *)aEnv );

    sResultValue = (dtlDataValue *)aResultValue;


    /**
     * Result 생성 
     */
    
    STL_TRY( dtlDateSetValueFromStlTime( stlNow(),
                                         DTL_PRECISION_NA,
                                         DTL_SCALE_NA,
                                         DTL_DATE_SIZE,
                                         sResultValue,
                                         KNL_DT_VECTOR( aEnv ),
                                         aEnv,
                                         ELL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief STATEMENT_DATE(), CURRENT_DATE() function에 대한 정보 얻기
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
stlStatus ellGetFuncInfoStatementDate( stlUInt16               aDataTypeArrayCount,
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
    STL_PARAM_VALIDATE( aDataTypeArrayCount == 0, aErrorStack );
    STL_PARAM_VALIDATE( aFuncArgDataTypeArrayCount == 0, aErrorStack );

    *aFuncResultDataType = DTL_TYPE_DATE;

    aFuncResultDataTypeDefInfo->mArgNum1           = DTL_PRECISION_NA;
    aFuncResultDataTypeDefInfo->mArgNum2           = DTL_SCALE_NA;
    aFuncResultDataTypeDefInfo->mStringLengthUnit  = DTL_STRING_LENGTH_UNIT_NA;
    aFuncResultDataTypeDefInfo->mIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;

    *aFuncPtrIdx = 0;

    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief STATEMENT_DATE(), CURRENT_DATE() function pointer 얻기
 * @param[in]  aFuncPtrIdx       function pointer 얻기 위한 식별자
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus ellGetFuncPtrStatementDate( stlUInt32             aFuncPtrIdx,
                                      dtlBuiltInFuncPtr   * aFuncPtr,
                                      stlErrorStack       * aErrorStack )
{
    STL_PARAM_VALIDATE( aFuncPtrIdx == 0, aErrorStack );
    STL_PARAM_VALIDATE( aFuncPtr != NULL, aErrorStack );

    
    /**
     * output 설정
     */
    
    *aFuncPtr = ellFuncStatementDate;

        
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief STATEMENT_DATE(), CURRENT_DATE() 을 반환하는 함수
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> input argument가 없음.
 * @param[out] aResultValue      연산 결과 (결과타입 DATE)
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus ellFuncStatementDate( stlUInt16        aInputArgumentCnt,
                                dtlValueEntry  * aInputArgument,
                                void           * aResultValue,
                                void           * aInfo,
                                dtlFuncVector  * aVectorFunc,
                                void           * aVectorArg,
                                void           * aEnv )
{
    dtlDataValue       * sResultValue;
    
    dtlValueBlockList  * sStmtTimestampTzBlock;
    dtlTimestampTzType * sStmtTimestampTz;
    
    
    DTL_PARAM_VALIDATE( aInputArgumentCnt == 0, (stlErrorStack *)aEnv );
    DTL_PARAM_VALIDATE( aInfo != NULL, (stlErrorStack *)aEnv );
    
    sResultValue = (dtlDataValue *)aResultValue;
    
    sStmtTimestampTzBlock = (knlValueBlockList *)aInfo;
    sStmtTimestampTz = (dtlTimestampTzType *)((KNL_GET_BLOCK_FIRST_DATA_VALUE(sStmtTimestampTzBlock))->mValue);
    
    /**
     * Result 생성 
     */
    
    STL_TRY( dtlTimestampTzToDate( sStmtTimestampTz,
                                   (dtlDateType *)(sResultValue->mValue),
                                   ELL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );
    
    sResultValue->mLength = DTL_DATE_SIZE;    
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief TRANSACTION DATE() function에 대한 정보 얻기
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
stlStatus ellGetFuncInfoTransactionDate( stlUInt16               aDataTypeArrayCount,
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
    STL_PARAM_VALIDATE( aDataTypeArrayCount == 0, aErrorStack );
    STL_PARAM_VALIDATE( aFuncArgDataTypeArrayCount == 0, aErrorStack );

    *aFuncResultDataType = DTL_TYPE_DATE;

    aFuncResultDataTypeDefInfo->mArgNum1           = DTL_PRECISION_NA;
    aFuncResultDataTypeDefInfo->mArgNum2           = DTL_SCALE_NA;
    aFuncResultDataTypeDefInfo->mStringLengthUnit  = DTL_STRING_LENGTH_UNIT_NA;
    aFuncResultDataTypeDefInfo->mIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;

    *aFuncPtrIdx = 0;

    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief TRANSACTION_DATE() function pointer 얻기
 * @param[in]  aFuncPtrIdx       function pointer 얻기 위한 식별자
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus ellGetFuncPtrTransactionDate( stlUInt32             aFuncPtrIdx,
                                        dtlBuiltInFuncPtr   * aFuncPtr,
                                        stlErrorStack       * aErrorStack )
{
    STL_PARAM_VALIDATE( aFuncPtrIdx == 0, aErrorStack );
    STL_PARAM_VALIDATE( aFuncPtr != NULL, aErrorStack );

    
    /**
     * output 설정
     */
    
    *aFuncPtr = ellFuncTransactionDate;

        
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief TRANSACTION_DATE() 을 반환하는 함수
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> input argument가 없음.
 * @param[out] aResultValue      연산 결과 (결과타입 DATE)
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus ellFuncTransactionDate( stlUInt16        aInputArgumentCnt,
                                  dtlValueEntry  * aInputArgument,
                                  void           * aResultValue,
                                  void           * aInfo,
                                  dtlFuncVector  * aVectorFunc,
                                  void           * aVectorArg,
                                  void           * aEnv )
{
    dtlDataValue       * sResultValue;
    
    dtlValueBlockList  * sTransactionTimestampTzBlock;
    dtlTimestampTzType * sTransactionTimestampTz;
    
    
    DTL_PARAM_VALIDATE( aInputArgumentCnt == 0, (stlErrorStack *)aEnv );
    DTL_PARAM_VALIDATE( aInfo != NULL, (stlErrorStack *)aEnv );
    
    sResultValue = (dtlDataValue *)aResultValue;
    
    sTransactionTimestampTzBlock = (knlValueBlockList *)aInfo;
    sTransactionTimestampTz = (dtlTimestampTzType *)((KNL_GET_BLOCK_FIRST_DATA_VALUE(sTransactionTimestampTzBlock))->mValue);
    
    /**
     * Result 생성 
     */
    
    STL_TRY( dtlTimestampTzToDate( sTransactionTimestampTz,
                                   (dtlDateType *)(sResultValue->mValue),
                                   ELL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );
    
    sResultValue->mLength = DTL_DATE_SIZE;    
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief CLOCK_TIME(), ROW_TIME() function에 대한 정보 얻기
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
 *
 * @remark ROW_TIME() alias는 제거되어야 함.
 */
stlStatus ellGetFuncInfoClockTime( stlUInt16               aDataTypeArrayCount,
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
    STL_PARAM_VALIDATE( aDataTypeArrayCount == 0, aErrorStack );
    STL_PARAM_VALIDATE( aFuncArgDataTypeArrayCount == 0, aErrorStack );

    *aFuncResultDataType = DTL_TYPE_TIME_WITH_TIME_ZONE;

    aFuncResultDataTypeDefInfo->mArgNum1           = DTL_TIMETZ_MAX_PRECISION;
    aFuncResultDataTypeDefInfo->mArgNum2           = DTL_SCALE_NA;
    aFuncResultDataTypeDefInfo->mStringLengthUnit  = DTL_STRING_LENGTH_UNIT_NA;
    aFuncResultDataTypeDefInfo->mIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;

    *aFuncPtrIdx = 0;

    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief CLOCK_TIME(), ROW_TIME() function pointer 얻기
 * @param[in]  aFuncPtrIdx       function pointer 얻기 위한 식별자
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 *
 * @remark ROW_TIME() alias는 제거되어야 함.
 */
stlStatus ellGetFuncPtrClockTime( stlUInt32             aFuncPtrIdx,
                                  dtlBuiltInFuncPtr   * aFuncPtr,
                                  stlErrorStack       * aErrorStack )
{
    STL_PARAM_VALIDATE( aFuncPtrIdx == 0, aErrorStack );
    STL_PARAM_VALIDATE( aFuncPtr != NULL, aErrorStack );

    
    /**
     * output 설정
     */
    
    *aFuncPtr = ellFuncClockTime;

        
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief CLOCK_TIME(), ROW_TIME() 을 반환하는 함수
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> input argument가 없음.
 * @param[out] aResultValue      연산 결과 (결과타입 TIME WITH TIME ZONE)
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 *
 * @remark ROW_TIME() alias는 제거되어야 함.
 */
stlStatus ellFuncClockTime( stlUInt16        aInputArgumentCnt,
                            dtlValueEntry  * aInputArgument,
                            void           * aResultValue,
                            void           * aInfo,
                            dtlFuncVector  * aVectorFunc,
                            void           * aVectorArg,
                            void           * aEnv )
{
    dtlDataValue   * sResultValue;

    
    DTL_PARAM_VALIDATE( aInputArgumentCnt == 0, (stlErrorStack *)aEnv );

    sResultValue = (dtlDataValue *)aResultValue;


    /**
     * Result 생성 
     */
    
    STL_TRY( dtlTimeTzSetValueFromStlTime( stlNow(),
                                           DTL_TIMETZ_MAX_PRECISION,
                                           DTL_SCALE_NA,
                                           DTL_TIMETZ_SIZE,
                                           sResultValue,
                                           KNL_DT_VECTOR( aEnv ),
                                           aEnv,
                                           ELL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief STATEMENT_TIME(), CURRENT_TIME() function에 대한 정보 얻기
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
stlStatus ellGetFuncInfoStatementTime( stlUInt16               aDataTypeArrayCount,
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
    STL_PARAM_VALIDATE( aDataTypeArrayCount == 0, aErrorStack );
    STL_PARAM_VALIDATE( aFuncArgDataTypeArrayCount == 0, aErrorStack );

    *aFuncResultDataType = DTL_TYPE_TIME_WITH_TIME_ZONE;

    aFuncResultDataTypeDefInfo->mArgNum1           = DTL_TIMETZ_MAX_PRECISION;
    aFuncResultDataTypeDefInfo->mArgNum2           = DTL_SCALE_NA;
    aFuncResultDataTypeDefInfo->mStringLengthUnit  = DTL_STRING_LENGTH_UNIT_NA;
    aFuncResultDataTypeDefInfo->mIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;

    *aFuncPtrIdx = 0;

    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief STATEMENT_TIME(), CURRENT_TIME() function pointer 얻기
 * @param[in]  aFuncPtrIdx       function pointer 얻기 위한 식별자
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus ellGetFuncPtrStatementTime( stlUInt32             aFuncPtrIdx,
                                      dtlBuiltInFuncPtr   * aFuncPtr,
                                      stlErrorStack       * aErrorStack )
{
    STL_PARAM_VALIDATE( aFuncPtrIdx == 0, aErrorStack );
    STL_PARAM_VALIDATE( aFuncPtr != NULL, aErrorStack );

    
    /**
     * output 설정
     */
    
    *aFuncPtr = ellFuncStatementTime;

        
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief STATEMENT_TIME(), CURRENT_TIME() 을 반환하는 함수
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> input argument가 없음.
 * @param[out] aResultValue      연산 결과 (결과타입 TIME WITH TIME ZONE)
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus ellFuncStatementTime( stlUInt16        aInputArgumentCnt,
                                dtlValueEntry  * aInputArgument,
                                void           * aResultValue,
                                void           * aInfo,
                                dtlFuncVector  * aVectorFunc,
                                void           * aVectorArg,
                                void           * aEnv )
{
    dtlDataValue       * sResultValue;

    dtlValueBlockList  * sStmtTimestampTzBlock;
    dtlTimestampTzType * sStmtTimestampTz;
    
    DTL_PARAM_VALIDATE( aInputArgumentCnt == 0, (stlErrorStack *)aEnv );
    DTL_PARAM_VALIDATE( aInfo != NULL, (stlErrorStack *)aEnv );

    sResultValue = (dtlDataValue *)aResultValue;

    sStmtTimestampTzBlock = (knlValueBlockList *)aInfo;
    sStmtTimestampTz = (dtlTimestampTzType *)((KNL_GET_BLOCK_FIRST_DATA_VALUE(sStmtTimestampTzBlock))->mValue);

    /**
     * Result 생성 
     */

    STL_TRY( dtlTimestampTzToTimeTz( sStmtTimestampTz,
                                     DTL_TIMETZ_MAX_PRECISION,
                                     (dtlTimeTzType *)(sResultValue->mValue),
                                     ELL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );

    sResultValue->mLength = DTL_TIMETZ_SIZE;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief TRANSACTION_TIME() function에 대한 정보 얻기
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
stlStatus ellGetFuncInfoTransactionTime( stlUInt16               aDataTypeArrayCount,
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
    STL_PARAM_VALIDATE( aDataTypeArrayCount == 0, aErrorStack );
    STL_PARAM_VALIDATE( aFuncArgDataTypeArrayCount == 0, aErrorStack );

    *aFuncResultDataType = DTL_TYPE_TIME_WITH_TIME_ZONE;

    aFuncResultDataTypeDefInfo->mArgNum1           = DTL_TIMETZ_MAX_PRECISION;
    aFuncResultDataTypeDefInfo->mArgNum2           = DTL_SCALE_NA;
    aFuncResultDataTypeDefInfo->mStringLengthUnit  = DTL_STRING_LENGTH_UNIT_NA;
    aFuncResultDataTypeDefInfo->mIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;

    *aFuncPtrIdx = 0;

    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief TRANSACTION_TIME() function pointer 얻기
 * @param[in]  aFuncPtrIdx       function pointer 얻기 위한 식별자
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus ellGetFuncPtrTransactionTime( stlUInt32             aFuncPtrIdx,
                                        dtlBuiltInFuncPtr   * aFuncPtr,
                                        stlErrorStack       * aErrorStack )
{
    STL_PARAM_VALIDATE( aFuncPtrIdx == 0, aErrorStack );
    STL_PARAM_VALIDATE( aFuncPtr != NULL, aErrorStack );

    
    /**
     * output 설정
     */
    
    *aFuncPtr = ellFuncTransactionTime;

        
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief TRANSACTION_TIME() 을 반환하는 함수
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> input argument가 없음.
 * @param[out] aResultValue      연산 결과 (결과타입 TIME WITH TIME ZONE)
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus ellFuncTransactionTime( stlUInt16        aInputArgumentCnt,
                                  dtlValueEntry  * aInputArgument,
                                  void           * aResultValue,
                                  void           * aInfo,
                                  dtlFuncVector  * aVectorFunc,
                                  void           * aVectorArg,
                                  void           * aEnv )
{
    dtlDataValue       * sResultValue;

    dtlValueBlockList  * sTransactionTimestampTzBlock;
    dtlTimestampTzType * sTransactionTimestampTz;
    
    DTL_PARAM_VALIDATE( aInputArgumentCnt == 0, (stlErrorStack *)aEnv );
    DTL_PARAM_VALIDATE( aInfo != NULL, (stlErrorStack *)aEnv );

    sResultValue = (dtlDataValue *)aResultValue;

    sTransactionTimestampTzBlock = (knlValueBlockList *)aInfo;
    sTransactionTimestampTz = (dtlTimestampTzType *)((KNL_GET_BLOCK_FIRST_DATA_VALUE(sTransactionTimestampTzBlock))->mValue);
    
    /**
     * Result 생성 
     */

    STL_TRY( dtlTimestampTzToTimeTz( sTransactionTimestampTz,
                                     DTL_TIMETZ_MAX_PRECISION,
                                     (dtlTimeTzType *)(sResultValue->mValue),
                                     ELL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );

    sResultValue->mLength = DTL_TIMETZ_SIZE;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief CLOCK_TIMESTAMP(), ROW_TIMESTAMP() function에 대한 정보 얻기
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
 *
 * @remark ROW_TIMESTAMP() alias는 제거되어야 함.
 */
stlStatus ellGetFuncInfoClockTimestamp( stlUInt16               aDataTypeArrayCount,
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
    STL_PARAM_VALIDATE( aDataTypeArrayCount == 0, aErrorStack );
    STL_PARAM_VALIDATE( aFuncArgDataTypeArrayCount == 0, aErrorStack );

    *aFuncResultDataType = DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE;

    aFuncResultDataTypeDefInfo->mArgNum1           = DTL_TIMESTAMPTZ_MAX_PRECISION;
    aFuncResultDataTypeDefInfo->mArgNum2           = DTL_SCALE_NA;
    aFuncResultDataTypeDefInfo->mStringLengthUnit  = DTL_STRING_LENGTH_UNIT_NA;
    aFuncResultDataTypeDefInfo->mIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;

    *aFuncPtrIdx = 0;

    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief CLOCK_TIMESTAMP(), ROW_TIMESTAMP() function pointer 얻기
 * @param[in]  aFuncPtrIdx       function pointer 얻기 위한 식별자
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 *
 * @remark ROW_TIMESTAMP() alias는 제거되어야 함.
 */
stlStatus ellGetFuncPtrClockTimestamp( stlUInt32             aFuncPtrIdx,
                                       dtlBuiltInFuncPtr   * aFuncPtr,
                                       stlErrorStack       * aErrorStack )
{
    STL_PARAM_VALIDATE( aFuncPtrIdx == 0, aErrorStack );
    STL_PARAM_VALIDATE( aFuncPtr != NULL, aErrorStack );

    
    /**
     * output 설정
     */
    
    *aFuncPtr = ellFuncClockTimestamp;

        
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief CLOCK_TIMESTAMP(), ROW_TIMESTAMP() 을 반환하는 함수
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> input argument가 없음.
 * @param[out] aResultValue      연산 결과 (결과타입 TIMESTAMP WITH TIME ZONE)
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 *
 * @remark ROW_TIMESTAMP() alias는 제거되어야 함.
 */
stlStatus ellFuncClockTimestamp( stlUInt16        aInputArgumentCnt,
                                 dtlValueEntry  * aInputArgument,
                                 void           * aResultValue,
                                 void           * aInfo,
                                 dtlFuncVector  * aVectorFunc,
                                 void           * aVectorArg,
                                 void           * aEnv )
{
    dtlDataValue   * sResultValue;

    
    DTL_PARAM_VALIDATE( aInputArgumentCnt == 0, (stlErrorStack *)aEnv );

    sResultValue = (dtlDataValue *)aResultValue;


    /**
     * Result 생성 
     */
    
    STL_TRY( dtlTimestampTzSetValueFromStlTime( stlNow(),
                                                DTL_TIMESTAMPTZ_MAX_PRECISION,
                                                DTL_SCALE_NA,
                                                DTL_TIMESTAMPTZ_SIZE,
                                                sResultValue,
                                                KNL_DT_VECTOR( aEnv ),
                                                aEnv,
                                                ELL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief STATEMENT_TIMESTAMP(), CURRENT_TIMESTAMP() function에 대한 정보 얻기
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
stlStatus ellGetFuncInfoStatementTimestamp( stlUInt16               aDataTypeArrayCount,
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
    STL_PARAM_VALIDATE( aDataTypeArrayCount == 0, aErrorStack );
    STL_PARAM_VALIDATE( aFuncArgDataTypeArrayCount == 0, aErrorStack );

    *aFuncResultDataType = DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE;

    aFuncResultDataTypeDefInfo->mArgNum1           = DTL_TIMESTAMPTZ_MAX_PRECISION;
    aFuncResultDataTypeDefInfo->mArgNum2           = DTL_SCALE_NA;
    aFuncResultDataTypeDefInfo->mStringLengthUnit  = DTL_STRING_LENGTH_UNIT_NA;
    aFuncResultDataTypeDefInfo->mIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;

    *aFuncPtrIdx = 0;

    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief STATEMENT_TIMESTAMP(), CURRENT_TIMESTAMP() function pointer 얻기
 * @param[in]  aFuncPtrIdx       function pointer 얻기 위한 식별자
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus ellGetFuncPtrStatementTimestamp( stlUInt32             aFuncPtrIdx,
                                           dtlBuiltInFuncPtr   * aFuncPtr,
                                           stlErrorStack       * aErrorStack )
{
    STL_PARAM_VALIDATE( aFuncPtrIdx == 0, aErrorStack );
    STL_PARAM_VALIDATE( aFuncPtr != NULL, aErrorStack );

    
    /**
     * output 설정
     */
    
    *aFuncPtr = ellFuncStatementTimestamp;

        
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief STATEMENT_TIMESTAMP(), CURRENT_TIMESTAMP() 을 반환하는 함수
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> input argument가 없음.
 * @param[out] aResultValue      연산 결과 (결과타입 TIMESTAMP WITH TIME ZONE)
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus ellFuncStatementTimestamp( stlUInt16        aInputArgumentCnt,
                                     dtlValueEntry  * aInputArgument,
                                     void           * aResultValue,
                                     void           * aInfo,
                                     dtlFuncVector  * aVectorFunc,
                                     void           * aVectorArg,
                                     void           * aEnv )
{
    dtlDataValue       * sResultValue;

    dtlValueBlockList  * sStmtTimestampTzBlock;
    dtlTimestampTzType * sStmtTimestampTz;
    
    
    DTL_PARAM_VALIDATE( aInputArgumentCnt == 0, (stlErrorStack *)aEnv );
    DTL_PARAM_VALIDATE( aInfo != NULL, (stlErrorStack *)aEnv );

    sResultValue = (dtlDataValue *)aResultValue;

    sStmtTimestampTzBlock = (knlValueBlockList *)aInfo;
    sStmtTimestampTz = (dtlTimestampTzType *)((KNL_GET_BLOCK_FIRST_DATA_VALUE(sStmtTimestampTzBlock))->mValue);

    /**
     * Result 생성 
     */
    
    ((dtlTimestampTzType *)(sResultValue->mValue))->mTimestamp = sStmtTimestampTz->mTimestamp;
    ((dtlTimestampTzType *)(sResultValue->mValue))->mTimeZone = sStmtTimestampTz->mTimeZone;
    
    sResultValue->mLength = DTL_TIMESTAMPTZ_SIZE;
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief TRANSACTION_TIMESTAMP() function에 대한 정보 얻기
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
stlStatus ellGetFuncInfoTransactionTimestamp( stlUInt16               aDataTypeArrayCount,
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
    STL_PARAM_VALIDATE( aDataTypeArrayCount == 0, aErrorStack );
    STL_PARAM_VALIDATE( aFuncArgDataTypeArrayCount == 0, aErrorStack );

    *aFuncResultDataType = DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE;

    aFuncResultDataTypeDefInfo->mArgNum1           = DTL_TIMESTAMPTZ_MAX_PRECISION;
    aFuncResultDataTypeDefInfo->mArgNum2           = DTL_SCALE_NA;
    aFuncResultDataTypeDefInfo->mStringLengthUnit  = DTL_STRING_LENGTH_UNIT_NA;
    aFuncResultDataTypeDefInfo->mIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;

    *aFuncPtrIdx = 0;

    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief TRANSACTION_TIMESTAMP() function pointer 얻기
 * @param[in]  aFuncPtrIdx       function pointer 얻기 위한 식별자
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus ellGetFuncPtrTransactionTimestamp( stlUInt32             aFuncPtrIdx,
                                             dtlBuiltInFuncPtr   * aFuncPtr,
                                             stlErrorStack       * aErrorStack )
{
    STL_PARAM_VALIDATE( aFuncPtrIdx == 0, aErrorStack );
    STL_PARAM_VALIDATE( aFuncPtr != NULL, aErrorStack );

    
    /**
     * output 설정
     */
    
    *aFuncPtr = ellFuncTransactionTimestamp;

        
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief TRANSACTION_TIMESTAMP() 을 반환하는 함수
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> input argument가 없음.
 * @param[out] aResultValue      연산 결과 (결과타입 TIMESTAMP WITH TIME ZONE)
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus ellFuncTransactionTimestamp( stlUInt16        aInputArgumentCnt,
                                       dtlValueEntry  * aInputArgument,
                                       void           * aResultValue,
                                       void           * aInfo,
                                       dtlFuncVector  * aVectorFunc,
                                       void           * aVectorArg,
                                       void           * aEnv )
{
    dtlDataValue       * sResultValue;

    dtlValueBlockList  * sTransactionTimestampTzBlock;
    dtlTimestampTzType * sTransactionTimestampTz;

    
    DTL_PARAM_VALIDATE( aInputArgumentCnt == 0, (stlErrorStack *)aEnv );
    DTL_PARAM_VALIDATE( aInfo != NULL, (stlErrorStack *)aEnv );

    sResultValue = (dtlDataValue *)aResultValue;

    sTransactionTimestampTzBlock = (knlValueBlockList *)aInfo;
    sTransactionTimestampTz = (dtlTimestampTzType *)((KNL_GET_BLOCK_FIRST_DATA_VALUE(sTransactionTimestampTzBlock))->mValue);

    /**
     * Result 생성 
     */
    
    ((dtlTimestampTzType *)(sResultValue->mValue))->mTimestamp = sTransactionTimestampTz->mTimestamp;
    ((dtlTimestampTzType *)(sResultValue->mValue))->mTimeZone = sTransactionTimestampTz->mTimeZone;
    
    sResultValue->mLength = DTL_TIMESTAMPTZ_SIZE;
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief CLOCK_LOCALTIME(), ROW_LOCALTIME() function에 대한 정보 얻기
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
 *
 * @remark ROW_LOCALTIME() alias는 제거되어야 함.
 */
stlStatus ellGetFuncInfoClockLocalTime( stlUInt16               aDataTypeArrayCount,
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
    STL_PARAM_VALIDATE( aDataTypeArrayCount == 0, aErrorStack );
    STL_PARAM_VALIDATE( aFuncArgDataTypeArrayCount == 0, aErrorStack );

    *aFuncResultDataType = DTL_TYPE_TIME;

    aFuncResultDataTypeDefInfo->mArgNum1           = DTL_TIME_MAX_PRECISION;
    aFuncResultDataTypeDefInfo->mArgNum2           = DTL_SCALE_NA;
    aFuncResultDataTypeDefInfo->mStringLengthUnit  = DTL_STRING_LENGTH_UNIT_NA;
    aFuncResultDataTypeDefInfo->mIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;

    *aFuncPtrIdx = 0;

    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief CLOCK_LOCALTIME(), ROW_LOCALTIME() function pointer 얻기
 * @param[in]  aFuncPtrIdx       function pointer 얻기 위한 식별자
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 *
 * @remark ROW_LOCALTIME() alias는 제거되어야 함.
 */
stlStatus ellGetFuncPtrClockLocalTime( stlUInt32             aFuncPtrIdx,
                                       dtlBuiltInFuncPtr   * aFuncPtr,
                                       stlErrorStack       * aErrorStack )
{
    STL_PARAM_VALIDATE( aFuncPtrIdx == 0, aErrorStack );
    STL_PARAM_VALIDATE( aFuncPtr != NULL, aErrorStack );

    
    /**
     * output 설정
     */
    
    *aFuncPtr = ellFuncClockLocalTime;

        
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief CLOCK_LOCALTIME(), ROW_LOCALTIME() 을 반환하는 함수
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> input argument가 없음.
 * @param[out] aResultValue      연산 결과 (결과타입 TIME)
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 *
 * @remark ROW_LOCALTIME() alias는 제거되어야 함.
 */
stlStatus ellFuncClockLocalTime( stlUInt16        aInputArgumentCnt,
                                 dtlValueEntry  * aInputArgument,
                                 void           * aResultValue,
                                 void           * aInfo,
                                 dtlFuncVector  * aVectorFunc,
                                 void           * aVectorArg,
                                 void           * aEnv )
{
    dtlDataValue   * sResultValue;

    
    DTL_PARAM_VALIDATE( aInputArgumentCnt == 0, (stlErrorStack *)aEnv );

    sResultValue = (dtlDataValue *)aResultValue;


    /**
     * Result 생성 
     */
    
    STL_TRY( dtlTimeSetValueFromStlTime( stlNow(),
                                         DTL_TIME_MAX_PRECISION,
                                         DTL_SCALE_NA,
                                         DTL_TIME_SIZE,
                                         sResultValue,
                                         KNL_DT_VECTOR( aEnv ),
                                         aEnv,
                                         ELL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief STATEMENT_LOCALTIME(), LOCALTIME() function에 대한 정보 얻기
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
stlStatus ellGetFuncInfoStatementLocalTime( stlUInt16               aDataTypeArrayCount,
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
    STL_PARAM_VALIDATE( aDataTypeArrayCount == 0, aErrorStack );
    STL_PARAM_VALIDATE( aFuncArgDataTypeArrayCount == 0, aErrorStack );

    *aFuncResultDataType = DTL_TYPE_TIME;

    aFuncResultDataTypeDefInfo->mArgNum1           = DTL_TIME_MAX_PRECISION;
    aFuncResultDataTypeDefInfo->mArgNum2           = DTL_SCALE_NA;
    aFuncResultDataTypeDefInfo->mStringLengthUnit  = DTL_STRING_LENGTH_UNIT_NA;
    aFuncResultDataTypeDefInfo->mIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;

    *aFuncPtrIdx = 0;

    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief STATEMENT_LOCALTIME(), LOCALTIME() function pointer 얻기
 * @param[in]  aFuncPtrIdx       function pointer 얻기 위한 식별자
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus ellGetFuncPtrStatementLocalTime( stlUInt32             aFuncPtrIdx,
                                           dtlBuiltInFuncPtr   * aFuncPtr,
                                           stlErrorStack       * aErrorStack )
{
    STL_PARAM_VALIDATE( aFuncPtrIdx == 0, aErrorStack );
    STL_PARAM_VALIDATE( aFuncPtr != NULL, aErrorStack );

    
    /**
     * output 설정
     */
    
    *aFuncPtr = ellFuncStatementLocalTime;

        
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief STATEMENT_LOCALTIME(), LOCALTIME() 을 반환하는 함수
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> input argument가 없음.
 * @param[out] aResultValue      연산 결과 (결과타입 TIME)
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus ellFuncStatementLocalTime( stlUInt16        aInputArgumentCnt,
                                     dtlValueEntry  * aInputArgument,
                                     void           * aResultValue,
                                     void           * aInfo,
                                     dtlFuncVector  * aVectorFunc,
                                     void           * aVectorArg,
                                     void           * aEnv )
{
    dtlDataValue       * sResultValue;

    dtlValueBlockList  * sStmtTimestampTzBlock;
    dtlTimestampTzType * sStmtTimestampTz;
    
    DTL_PARAM_VALIDATE( aInputArgumentCnt == 0, (stlErrorStack *)aEnv );
    DTL_PARAM_VALIDATE( aInfo != NULL, (stlErrorStack *)aEnv );

    sResultValue = (dtlDataValue *)aResultValue;

    sStmtTimestampTzBlock = (knlValueBlockList *)aInfo;
    sStmtTimestampTz = (dtlTimestampTzType *)((KNL_GET_BLOCK_FIRST_DATA_VALUE(sStmtTimestampTzBlock))->mValue);

    /**
     * Result 생성 
     */

    STL_TRY( dtlTimestampTzToTime( sStmtTimestampTz,
                                   DTL_TIME_MAX_PRECISION,
                                   (dtlTimeType *)(sResultValue->mValue),
                                   ELL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );
    
    sResultValue->mLength = DTL_TIME_SIZE;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief TRANSACTION_LOCALTIME() function에 대한 정보 얻기
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
stlStatus ellGetFuncInfoTransactionLocalTime( stlUInt16               aDataTypeArrayCount,
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
    STL_PARAM_VALIDATE( aDataTypeArrayCount == 0, aErrorStack );
    STL_PARAM_VALIDATE( aFuncArgDataTypeArrayCount == 0, aErrorStack );

    *aFuncResultDataType = DTL_TYPE_TIME;

    aFuncResultDataTypeDefInfo->mArgNum1           = DTL_TIME_MAX_PRECISION;
    aFuncResultDataTypeDefInfo->mArgNum2           = DTL_SCALE_NA;
    aFuncResultDataTypeDefInfo->mStringLengthUnit  = DTL_STRING_LENGTH_UNIT_NA;
    aFuncResultDataTypeDefInfo->mIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;

    *aFuncPtrIdx = 0;

    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief TRANSACTION_LOCALTIME() function pointer 얻기
 * @param[in]  aFuncPtrIdx       function pointer 얻기 위한 식별자
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus ellGetFuncPtrTransactionLocalTime( stlUInt32             aFuncPtrIdx,
                                             dtlBuiltInFuncPtr   * aFuncPtr,
                                             stlErrorStack       * aErrorStack )
{
    STL_PARAM_VALIDATE( aFuncPtrIdx == 0, aErrorStack );
    STL_PARAM_VALIDATE( aFuncPtr != NULL, aErrorStack );

    
    /**
     * output 설정
     */
    
    *aFuncPtr = ellFuncTransactionLocalTime;

        
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief TRANSACTION_LOCALTIME() 을 반환하는 함수
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> input argument가 없음.
 * @param[out] aResultValue      연산 결과 (결과타입 TIME)
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus ellFuncTransactionLocalTime( stlUInt16        aInputArgumentCnt,
                                       dtlValueEntry  * aInputArgument,
                                       void           * aResultValue,
                                       void           * aInfo,
                                       dtlFuncVector  * aVectorFunc,
                                       void           * aVectorArg,
                                       void           * aEnv )
{
    dtlDataValue       * sResultValue;

    dtlValueBlockList  * sTransactionTimestampTzBlock;
    dtlTimestampTzType * sTransactionTimestampTz;

    
    DTL_PARAM_VALIDATE( aInputArgumentCnt == 0, (stlErrorStack *)aEnv );
    DTL_PARAM_VALIDATE( aInfo != NULL, (stlErrorStack *)aEnv );

    sResultValue = (dtlDataValue *)aResultValue;

    sTransactionTimestampTzBlock = (knlValueBlockList *)aInfo;
    sTransactionTimestampTz = (dtlTimestampTzType *)((KNL_GET_BLOCK_FIRST_DATA_VALUE(sTransactionTimestampTzBlock))->mValue);

    /**
     * Result 생성 
     */

    STL_TRY( dtlTimestampTzToTime( sTransactionTimestampTz,
                                   DTL_TIME_MAX_PRECISION,
                                   (dtlTimeType *)(sResultValue->mValue),
                                   ELL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );
    
    sResultValue->mLength = DTL_TIME_SIZE;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief CLOCK_LOCALTIMESTAMP(), ROW_LOCALTIMESTAMP() function에 대한 정보 얻기
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
 *
 * @remark ROW_LOCALTIMESTAMP() alias는 제거되어야 함.
 */
stlStatus ellGetFuncInfoClockLocalTimestamp( stlUInt16               aDataTypeArrayCount,
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
    STL_PARAM_VALIDATE( aDataTypeArrayCount == 0, aErrorStack );
    STL_PARAM_VALIDATE( aFuncArgDataTypeArrayCount == 0, aErrorStack );

    *aFuncResultDataType = DTL_TYPE_TIMESTAMP;

    aFuncResultDataTypeDefInfo->mArgNum1           = DTL_TIMESTAMP_MAX_PRECISION;
    aFuncResultDataTypeDefInfo->mArgNum2           = DTL_SCALE_NA;
    aFuncResultDataTypeDefInfo->mStringLengthUnit  = DTL_STRING_LENGTH_UNIT_NA;
    aFuncResultDataTypeDefInfo->mIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;

    *aFuncPtrIdx = 0;

    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief CLOCK_LOCALTIMESTAMP(), ROW_LOCALTIMESTAMP() function pointer 얻기
 * @param[in]  aFuncPtrIdx       function pointer 얻기 위한 식별자
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 *
 * @remark ROW_LOCALTIMESTAMP() alias는 제거되어야 함.
 */
stlStatus ellGetFuncPtrClockLocalTimestamp( stlUInt32             aFuncPtrIdx,
                                            dtlBuiltInFuncPtr   * aFuncPtr,
                                            stlErrorStack       * aErrorStack )
{
    STL_PARAM_VALIDATE( aFuncPtrIdx == 0, aErrorStack );
    STL_PARAM_VALIDATE( aFuncPtr != NULL, aErrorStack );

    
    /**
     * output 설정
     */
    
    *aFuncPtr = ellFuncClockLocalTimestamp;

        
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief CLOCK_LOCALTIMESTAMP(), ROW_LOCALTIMESTAMP() 을 반환하는 함수
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> input argument가 없음.
 * @param[out] aResultValue      연산 결과 (결과타입 TIMESTAMP)
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 *
 * @remark ROW_LOCALTIMESTAMP() alias는 제거되어야 함.
 */
stlStatus ellFuncClockLocalTimestamp( stlUInt16        aInputArgumentCnt,
                                      dtlValueEntry  * aInputArgument,
                                      void           * aResultValue,
                                      void           * aInfo,
                                      dtlFuncVector  * aVectorFunc,
                                      void           * aVectorArg,
                                      void           * aEnv )
{
    dtlDataValue   * sResultValue;

    
    DTL_PARAM_VALIDATE( aInputArgumentCnt == 0, (stlErrorStack *)aEnv );

    sResultValue = (dtlDataValue *)aResultValue;


    /**
     * Result 생성 
     */
    
    STL_TRY( dtlTimestampSetValueFromStlTime( stlNow(),
                                              DTL_TIMESTAMP_MAX_PRECISION,
                                              DTL_SCALE_NA,
                                              DTL_TIMESTAMP_SIZE,
                                              sResultValue,
                                              KNL_DT_VECTOR( aEnv ),
                                              aEnv,
                                              ELL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief STATEMENT_LOCALTIMESTAMP(), LOCALTIMESTAMP() function에 대한 정보 얻기
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
stlStatus ellGetFuncInfoStatementLocalTimestamp( stlUInt16               aDataTypeArrayCount,
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
    STL_PARAM_VALIDATE( aDataTypeArrayCount == 0, aErrorStack );
    STL_PARAM_VALIDATE( aFuncArgDataTypeArrayCount == 0, aErrorStack );

    *aFuncResultDataType = DTL_TYPE_TIMESTAMP;

    aFuncResultDataTypeDefInfo->mArgNum1           = DTL_TIMESTAMP_MAX_PRECISION;
    aFuncResultDataTypeDefInfo->mArgNum2           = DTL_SCALE_NA;
    aFuncResultDataTypeDefInfo->mStringLengthUnit  = DTL_STRING_LENGTH_UNIT_NA;
    aFuncResultDataTypeDefInfo->mIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;

    *aFuncPtrIdx = 0;

    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief STATEMENT_LOCALTIMESTAMP(), LOCALTIMESTAMP() function pointer 얻기
 * @param[in]  aFuncPtrIdx       function pointer 얻기 위한 식별자
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus ellGetFuncPtrStatementLocalTimestamp( stlUInt32             aFuncPtrIdx,
                                                dtlBuiltInFuncPtr   * aFuncPtr,
                                                stlErrorStack       * aErrorStack )
{
    STL_PARAM_VALIDATE( aFuncPtrIdx == 0, aErrorStack );
    STL_PARAM_VALIDATE( aFuncPtr != NULL, aErrorStack );

    
    /**
     * output 설정
     */
    
    *aFuncPtr = ellFuncStatementLocalTimestamp;

        
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief STATEMENT_LOCALTIMESTAMP(), LOCALTIMESTAMP() 을 반환하는 함수
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> input argument가 없음.
 * @param[out] aResultValue      연산 결과 (결과타입 TIMESTAMP)
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus ellFuncStatementLocalTimestamp( stlUInt16        aInputArgumentCnt,
                                          dtlValueEntry  * aInputArgument,
                                          void           * aResultValue,
                                          void           * aInfo,
                                          dtlFuncVector  * aVectorFunc,
                                          void           * aVectorArg,
                                          void           * aEnv )
{
    dtlDataValue       * sResultValue;

    dtlValueBlockList  * sStmtTimestampTzBlock;
    dtlTimestampTzType * sStmtTimestampTz;
    
    DTL_PARAM_VALIDATE( aInputArgumentCnt == 0, (stlErrorStack *)aEnv );
    DTL_PARAM_VALIDATE( aInfo != NULL, (stlErrorStack *)aEnv );

    sResultValue = (dtlDataValue *)aResultValue;
    
    sStmtTimestampTzBlock = (knlValueBlockList *)aInfo;
    sStmtTimestampTz = (dtlTimestampTzType *)((KNL_GET_BLOCK_FIRST_DATA_VALUE(sStmtTimestampTzBlock))->mValue);
    
    /**
     * Result 생성 
     */
    
    STL_TRY( dtlTimestampTzToTimestamp( sStmtTimestampTz,
                                        DTL_TIMESTAMP_MAX_PRECISION,
                                        (dtlTimestampType *)(sResultValue->mValue),
                                        ELL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );
    
    sResultValue->mLength = DTL_TIMESTAMP_SIZE;
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief TRANSACTION_LOCALTIMESTAMP() function에 대한 정보 얻기
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
stlStatus ellGetFuncInfoTransactionLocalTimestamp( stlUInt16               aDataTypeArrayCount,
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
    STL_PARAM_VALIDATE( aDataTypeArrayCount == 0, aErrorStack );
    STL_PARAM_VALIDATE( aFuncArgDataTypeArrayCount == 0, aErrorStack );

    *aFuncResultDataType = DTL_TYPE_TIMESTAMP;

    aFuncResultDataTypeDefInfo->mArgNum1           = DTL_TIMESTAMP_MAX_PRECISION;
    aFuncResultDataTypeDefInfo->mArgNum2           = DTL_SCALE_NA;
    aFuncResultDataTypeDefInfo->mStringLengthUnit  = DTL_STRING_LENGTH_UNIT_NA;
    aFuncResultDataTypeDefInfo->mIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;

    *aFuncPtrIdx = 0;

    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief TRANSACTION_LOCALTIMESTAMP() function pointer 얻기
 * @param[in]  aFuncPtrIdx       function pointer 얻기 위한 식별자
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus ellGetFuncPtrTransactionLocalTimestamp( stlUInt32             aFuncPtrIdx,
                                                  dtlBuiltInFuncPtr   * aFuncPtr,
                                                  stlErrorStack       * aErrorStack )
{
    STL_PARAM_VALIDATE( aFuncPtrIdx == 0, aErrorStack );
    STL_PARAM_VALIDATE( aFuncPtr != NULL, aErrorStack );

    
    /**
     * output 설정
     */
    
    *aFuncPtr = ellFuncTransactionLocalTimestamp;

        
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief TRANSACTION_LOCALTIMESTAMP() 을 반환하는 함수
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> input argument가 없음.
 * @param[out] aResultValue      연산 결과 (결과타입 TIMESTAMP)
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus ellFuncTransactionLocalTimestamp( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aInfo,
                                            dtlFuncVector  * aVectorFunc,
                                            void           * aVectorArg,
                                            void           * aEnv )
{
    dtlDataValue       * sResultValue;

    dtlValueBlockList  * sTransactionTimestampTzBlock;
    dtlTimestampTzType * sTransactionTimestampTz;
    
    
    DTL_PARAM_VALIDATE( aInputArgumentCnt == 0, (stlErrorStack *)aEnv );
    DTL_PARAM_VALIDATE( aInfo != NULL, (stlErrorStack *)aEnv );

    sResultValue = (dtlDataValue *)aResultValue;

    sTransactionTimestampTzBlock = (knlValueBlockList *)aInfo;
    sTransactionTimestampTz = (dtlTimestampTzType *)((KNL_GET_BLOCK_FIRST_DATA_VALUE(sTransactionTimestampTzBlock))->mValue);

    /**
     * Result 생성 
     */
    
    STL_TRY( dtlTimestampTzToTimestamp( sTransactionTimestampTz,
                                        DTL_TIMESTAMP_MAX_PRECISION,
                                        (dtlTimestampType *)(sResultValue->mValue),
                                        ELL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );
    
    sResultValue->mLength = DTL_TIMESTAMP_SIZE;
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief SYSDATE() function에 대한 정보 얻기
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
stlStatus ellGetFuncInfoSysDate( stlUInt16               aDataTypeArrayCount,
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
    STL_PARAM_VALIDATE( aDataTypeArrayCount == 0, aErrorStack );
    STL_PARAM_VALIDATE( aFuncArgDataTypeArrayCount == 0, aErrorStack );
    
    *aFuncResultDataType = DTL_TYPE_DATE;

    aFuncResultDataTypeDefInfo->mArgNum1           = DTL_PRECISION_NA;
    aFuncResultDataTypeDefInfo->mArgNum2           = DTL_SCALE_NA;
    aFuncResultDataTypeDefInfo->mStringLengthUnit  = DTL_STRING_LENGTH_UNIT_NA;
    aFuncResultDataTypeDefInfo->mIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;

    *aFuncPtrIdx = 0;

    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief SYSDATE() function pointer 얻기
 * @param[in]  aFuncPtrIdx       function pointer 얻기 위한 식별자
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus ellGetFuncPtrSysDate( stlUInt32             aFuncPtrIdx,
                                dtlBuiltInFuncPtr   * aFuncPtr,
                                stlErrorStack       * aErrorStack )
{
    STL_PARAM_VALIDATE( aFuncPtrIdx == 0, aErrorStack );
    STL_PARAM_VALIDATE( aFuncPtr != NULL, aErrorStack );
    
    
    /**
     * output 설정
     */
    
    *aFuncPtr = ellFuncSysDate;

        
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief SYSDATE() 을 반환하는 함수
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> input argument가 없음.
 * @param[out] aResultValue      연산 결과 (결과타입 DATE)
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus ellFuncSysDate( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aInfo,
                          dtlFuncVector  * aVectorFunc,
                          void           * aVectorArg,
                          void           * aEnv )
{
    dtlDataValue       * sResultValue;
    
    dtlValueBlockList  * sSysTimestampTzBlock;
    dtlTimestampTzType * sSysTimestampTz;
    
    DTL_PARAM_VALIDATE( aInputArgumentCnt == 0, (stlErrorStack *)aEnv );
    DTL_PARAM_VALIDATE( aInfo != NULL, (stlErrorStack *)aEnv );
    
    sResultValue = (dtlDataValue *)aResultValue;
    
    sSysTimestampTzBlock = (knlValueBlockList *)aInfo;
    sSysTimestampTz = (dtlTimestampTzType *)((KNL_GET_BLOCK_FIRST_DATA_VALUE(sSysTimestampTzBlock))->mValue);
    
    /**
     * Result 생성 
     */
    
    STL_TRY( dtlTimestampTzToDate( sSysTimestampTz,
                                   (dtlDateType *)(sResultValue->mValue),
                                   ELL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );
    
    sResultValue->mLength = DTL_DATE_SIZE;    
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief SYSTIME() function에 대한 정보 얻기
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
stlStatus ellGetFuncInfoSysTime( stlUInt16               aDataTypeArrayCount,
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
    STL_PARAM_VALIDATE( aDataTypeArrayCount == 0, aErrorStack );
    STL_PARAM_VALIDATE( aFuncArgDataTypeArrayCount == 0, aErrorStack );
    
    *aFuncResultDataType = DTL_TYPE_TIME_WITH_TIME_ZONE;

    aFuncResultDataTypeDefInfo->mArgNum1           = DTL_TIMETZ_MAX_PRECISION;
    aFuncResultDataTypeDefInfo->mArgNum2           = DTL_SCALE_NA;
    aFuncResultDataTypeDefInfo->mStringLengthUnit  = DTL_STRING_LENGTH_UNIT_NA;
    aFuncResultDataTypeDefInfo->mIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;

    *aFuncPtrIdx = 0;

    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief SYSTIME() function pointer 얻기
 * @param[in]  aFuncPtrIdx       function pointer 얻기 위한 식별자
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus ellGetFuncPtrSysTime( stlUInt32             aFuncPtrIdx,
                                dtlBuiltInFuncPtr   * aFuncPtr,
                                stlErrorStack       * aErrorStack )
{
    STL_PARAM_VALIDATE( aFuncPtrIdx == 0, aErrorStack );
    STL_PARAM_VALIDATE( aFuncPtr != NULL, aErrorStack );
    
    
    /**
     * output 설정
     */
    
    *aFuncPtr = ellFuncSysTime;
    
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief SYSTIME() 을 반환하는 함수
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> input argument가 없음.
 * @param[out] aResultValue      연산 결과 (결과타입 TIME WITH TIME ZONE)
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus ellFuncSysTime( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aInfo,
                          dtlFuncVector  * aVectorFunc,
                          void           * aVectorArg,
                          void           * aEnv )
{
    dtlDataValue       * sResultValue;
    
    dtlValueBlockList  * sSysTimestampTzBlock;
    dtlTimestampTzType * sSysTimestampTz;
    
    
    DTL_PARAM_VALIDATE( aInputArgumentCnt == 0, (stlErrorStack *)aEnv );
    DTL_PARAM_VALIDATE( aInfo != NULL, (stlErrorStack *)aEnv );
    
    sResultValue = (dtlDataValue *)aResultValue;
    
    sSysTimestampTzBlock = (knlValueBlockList *)aInfo;
    sSysTimestampTz = (dtlTimestampTzType *)((KNL_GET_BLOCK_FIRST_DATA_VALUE(sSysTimestampTzBlock))->mValue);
    
    /**
     * Result 생성 
     */

    STL_TRY( dtlTimestampTzToTimeTz( sSysTimestampTz,
                                     DTL_TIMETZ_MAX_PRECISION,
                                     (dtlTimeTzType *)(sResultValue->mValue),
                                     ELL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );
    
    sResultValue->mLength = DTL_TIMETZ_SIZE;
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief SYSTIMESTAMP() function에 대한 정보 얻기
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
stlStatus ellGetFuncInfoSysTimestamp( stlUInt16               aDataTypeArrayCount,
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
    STL_PARAM_VALIDATE( aDataTypeArrayCount == 0, aErrorStack );
    STL_PARAM_VALIDATE( aFuncArgDataTypeArrayCount == 0, aErrorStack );
    
    *aFuncResultDataType = DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE;

    aFuncResultDataTypeDefInfo->mArgNum1           = DTL_TIMESTAMPTZ_MAX_PRECISION;
    aFuncResultDataTypeDefInfo->mArgNum2           = DTL_SCALE_NA;
    aFuncResultDataTypeDefInfo->mStringLengthUnit  = DTL_STRING_LENGTH_UNIT_NA;
    aFuncResultDataTypeDefInfo->mIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;

    *aFuncPtrIdx = 0;

    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief SYSTIMESTAMP() function pointer 얻기
 * @param[in]  aFuncPtrIdx       function pointer 얻기 위한 식별자
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus ellGetFuncPtrSysTimestamp( stlUInt32             aFuncPtrIdx,
                                     dtlBuiltInFuncPtr   * aFuncPtr,
                                     stlErrorStack       * aErrorStack )
{
    STL_PARAM_VALIDATE( aFuncPtrIdx == 0, aErrorStack );
    STL_PARAM_VALIDATE( aFuncPtr != NULL, aErrorStack );
    
    
    /**
     * output 설정
     */
    
    *aFuncPtr = ellFuncSysTimestamp;

        
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief SYSTIMESTAMP() 을 반환하는 함수
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> input argument가 없음.
 * @param[out] aResultValue      연산 결과 (결과타입 TIMESTAMP WITH TIME ZONE)
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus ellFuncSysTimestamp( stlUInt16        aInputArgumentCnt,
                               dtlValueEntry  * aInputArgument,
                               void           * aResultValue,
                               void           * aInfo,
                               dtlFuncVector  * aVectorFunc,
                               void           * aVectorArg,
                               void           * aEnv )
{
    dtlDataValue       * sResultValue;
    
    dtlValueBlockList  * sSysTimestampTzBlock;
    dtlTimestampTzType * sSysTimestampTz;
    
    DTL_PARAM_VALIDATE( aInputArgumentCnt == 0, (stlErrorStack *)aEnv );
    DTL_PARAM_VALIDATE( aInfo != NULL, (stlErrorStack *)aEnv );
    
    sResultValue = (dtlDataValue *)aResultValue;
    
    sSysTimestampTzBlock = (knlValueBlockList *)aInfo;
    sSysTimestampTz = (dtlTimestampTzType *)((KNL_GET_BLOCK_FIRST_DATA_VALUE(sSysTimestampTzBlock))->mValue);
    
    /**
     * Result 생성 
     */

    ((dtlTimestampTzType *)(sResultValue->mValue))->mTimestamp = sSysTimestampTz->mTimestamp;
    ((dtlTimestampTzType *)(sResultValue->mValue))->mTimeZone = sSysTimestampTz->mTimeZone;
    
    sResultValue->mLength = DTL_TIMESTAMPTZ_SIZE;
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief STATEMENT_VIEW_SCN() function에 대한 정보 얻기
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
stlStatus ellGetFuncInfoStatementViewScn( stlUInt16               aDataTypeArrayCount,
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
    STL_PARAM_VALIDATE( aDataTypeArrayCount == 0, aErrorStack );
    STL_PARAM_VALIDATE( aFuncArgDataTypeArrayCount == 0, aErrorStack );

    *aFuncResultDataType = DTL_TYPE_NUMBER;

    aFuncResultDataTypeDefInfo->mArgNum1           = DTL_NUMERIC_MAX_PRECISION;
    aFuncResultDataTypeDefInfo->mArgNum2           = DTL_SCALE_NA;
    aFuncResultDataTypeDefInfo->mStringLengthUnit  = DTL_STRING_LENGTH_UNIT_NA;
    aFuncResultDataTypeDefInfo->mIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;

    *aFuncPtrIdx = 0;

    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief STATEMENT_VIEW_SCN() function pointer 얻기
 * @param[in]  aFuncPtrIdx       function pointer 얻기 위한 식별자
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus ellGetFuncPtrStatementViewScn( stlUInt32             aFuncPtrIdx,
                                         dtlBuiltInFuncPtr   * aFuncPtr,
                                         stlErrorStack       * aErrorStack )
{
    STL_PARAM_VALIDATE( aFuncPtrIdx == 0, aErrorStack );
    STL_PARAM_VALIDATE( aFuncPtr != NULL, aErrorStack );

    
    /**
     * output 설정
     */
    
    *aFuncPtr = ellFuncStatementViewScn;

        
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief STATEMENT_VIEW_SCN() 을 반환하는 함수
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> input argument가 없음.
 * @param[out] aResultValue      연산 결과 (결과타입 TIMESTAMP)
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus ellFuncStatementViewScn( stlUInt16        aInputArgumentCnt,
                                   dtlValueEntry  * aInputArgument,
                                   void           * aResultValue,
                                   void           * aInfo,
                                   dtlFuncVector  * aVectorFunc,
                                   void           * aVectorArg,
                                   void           * aEnv )
{
    dtlDataValue   * sResultValue = NULL;
    stlBool          sSuccessWithInfo = STL_FALSE;
    smlScn           sStatementViewScn = *((smlScn *)aInfo);
    
    DTL_PARAM_VALIDATE( aInputArgumentCnt == 0, (stlErrorStack *)aEnv );
    
    sResultValue = (dtlDataValue *)aResultValue;
    

    
    /**
     * Result 생성 
     */
    
    STL_TRY( dtlNumericSetValueFromUInt64Integer( (stlUInt64)sStatementViewScn,
                                                  DTL_NUMERIC_MAX_PRECISION,
                                                  DTL_SCALE_NA,
                                                  DTL_STRING_LENGTH_UNIT_NA,
                                                  DTL_INTERVAL_INDICATOR_NA,
                                                  DTL_NUMERIC_MAX_SIZE,
                                                  sResultValue,
                                                  &sSuccessWithInfo,
                                                  KNL_DT_VECTOR( aEnv ),
                                                  aEnv,
                                                  ELL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


/** @} ellSessionFunc */
 
