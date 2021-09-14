/*******************************************************************************
 * qlssAlterSessionSet.c
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
 * @file qlssAlterSessionSet.c
 * @brief ALTER SESSION SET 처리 루틴 
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @defgroup qlssSessionAlterSessionSet ALTER SESSION SET property 
 * @ingroup qlssSession
 * @{
 */

/**
 * @brief ALTER SESSION SET 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlssValidateSessionSet( smlTransId      aTransID,
                                  qllDBCStmt    * aDBCStmt,
                                  qllStatement  * aSQLStmt,
                                  stlChar       * aSQLString,
                                  qllNode       * aParseTree,
                                  qllEnv        * aEnv )
{
    qlssInitSessionSet  * sInitPlan = NULL;
    qlpAlterSession     * sParseTree = NULL;
    knlPropertyID         sPropertyId = KNL_PROPERTY_ID_MAX;
    qlpValue            * sSetValue;
    qlpValue            * sNameValue;
    knlPropertyDataType   sPropertyType;
    stlInt64              sIntSetValue;
    stlChar             * sPtrSetValue;
    stlChar             * sPropertyName = NULL;
    stlBool               sExist = STL_FALSE;
    stlInt64              sByteUnit = 1;

    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );

    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_SESSION_SET_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_ALTER_SESSION_SET_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득 
     */
    
    sParseTree = (qlpAlterSession *) aParseTree;

    STL_PARAM_VALIDATE( sParseTree->mPropertyName != NULL, QLL_ERROR_STACK(aEnv) );
    
    
    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qlssInitSessionSet),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qlssInitSessionSet) );

    /**
     * Property 유효성 검사
     */

    sNameValue    = sParseTree->mPropertyName;
    sPropertyName = QLP_GET_PTR_VALUE( sNameValue );

    STL_TRY( knlExistPropertyByName( sPropertyName,
                                     &sExist,
                                     KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    STL_TRY_THROW( sExist == STL_TRUE, RAMP_ERR_NAME );

    /**
     * Init Plan 구성
     */

    STL_TRY( knlGetPropertyIDByName( sPropertyName,
                                     &sPropertyId,
                                     KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    sInitPlan->mPropertyId = sPropertyId;

    sSetValue = sParseTree->mSetValue;

    if( sSetValue != NULL )
    {
        /*
         * SET parameter_name = value
         */

        sInitPlan->mDefaultValue = STL_FALSE;

        STL_TRY( knlGetPropertyDataTypeByID( sPropertyId,
                                             &sPropertyType,
                                             KNL_ENV( aEnv ) )
                 == STL_SUCCESS );

        if( sSetValue->mType == QLL_SIZE_TYPE )
        {
            sByteUnit = QLP_GET_BYTES_FROM_SIZE_UNIT( QLP_GET_INT_VALUE( ((qlpSize*)sSetValue)->mSizeUnit ) );
            sSetValue = ((qlpSize*)sSetValue)->mSize;
        }
        else
        {
            /* Do Nothing */
        }

        sIntSetValue = QLP_GET_INT_VALUE( sSetValue );
        sPtrSetValue = QLP_GET_PTR_VALUE( sSetValue );

        /**
         * Parameter Value 를 Property Type 에 맞도록 변환 
         */

        switch ( sSetValue->mType )
        {
            case QLL_BNF_INTPARAM_CONSTANT_TYPE:
                {
                    /**
                     * INTPARAM을 BOOL Type으로 변환
                     * - ON/OFF, 0/1, DISABLE/ENABLE, ...
                     */

                    STL_TRY_THROW( sPropertyType == KNL_PROPERTY_TYPE_BOOL,
                                   RAMP_ERR_DATATYPE );

                    sInitPlan->mBoolValue = (stlBool)sIntSetValue;
                    break;
                }
            case QLL_BNF_NUMBER_CONSTANT_TYPE:
                {
                    /**
                     * NUMBER는 String 형태로 오며, 이를 stlInt64로 변환
                     */

                    STL_TRY_THROW( dtlGetIntegerFromString( sPtrSetValue,
                                                            stlStrlen( sPtrSetValue ),
                                                            & sInitPlan->mIntegerValue,
                                                            QLL_ERROR_STACK(aEnv) )
                                   == STL_SUCCESS, RAMP_ERR_DATATYPE );

                    /**
                     * Property Type이 BOOL이라면 Value는 0 또는 1의 값을 가져야 한다.
                     */
                    if( sPropertyType == KNL_PROPERTY_TYPE_BOOL )
                    {
                        STL_TRY_THROW( (sInitPlan->mIntegerValue == 0) ||
                                       (sInitPlan->mIntegerValue == 1),
                                       RAMP_ERR_DATATYPE );

                        sInitPlan->mBoolValue = (stlBool)sInitPlan->mIntegerValue;
                    }
                    else
                    {
                        STL_TRY_THROW( sPropertyType == KNL_PROPERTY_TYPE_BIGINT,
                                       RAMP_ERR_DATATYPE );

                        sInitPlan->mIntegerValue *= sByteUnit;
                    }

                    break;
                }
            case QLL_BNF_STRING_CONSTANT_TYPE:
                {
                    /**
                     * String 의 경우 Property Type 에 따라 적절한 변환이 필요하다.
                     */

                    switch ( sPropertyType )
                    {
                        case KNL_PROPERTY_TYPE_VARCHAR:
                            {
                                /**
                                 * STRING은 Property Varchar로 변환
                                 */

                                STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                                            stlStrlen( sPtrSetValue ) + 1,
                                                            (void **) & sInitPlan->mStringValue,
                                                            KNL_ENV(aEnv) )
                                         == STL_SUCCESS );

                                stlStrcpy( sInitPlan->mStringValue, sPtrSetValue );

                                /**
                                 * Property Format Value 의 유효성 검사
                                 */

                                STL_TRY( qlssCheckPropFormatString( sInitPlan->mPropertyId,
                                                                    sInitPlan->mStringValue,
                                                                    aEnv )
                                         == STL_SUCCESS );
                                
                                break;
                            }
                        case KNL_PROPERTY_TYPE_BIGINT:
                            {
                                STL_TRY( knlConvertUnitToValue( sPropertyId,
                                                                sPtrSetValue,
                                                                & sInitPlan->mIntegerValue,
                                                                KNL_ENV(aEnv) )
                                         == STL_SUCCESS );

                                sInitPlan->mIntegerValue *= sByteUnit;
                                break;
                            }
                        default:
                            {
                                STL_THROW( RAMP_ERR_DATATYPE );
                                break;
                            }
                    }

                    break;
                }
            default:
                {
                    STL_THROW( RAMP_ERR_DATATYPE );
                    break;
                }
        }
    }
    else
    {
        /*
         * SET parameter_name TO DEFAULT
         */

        sInitPlan->mDefaultValue = STL_TRUE;
    }

    /**
     * 권한 검사가 없음
     */
    
    /**
     * Init Plan 연결 
     */

    aSQLStmt->mInitPlan = (void *) sInitPlan;
        
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_DATATYPE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_PROPERTY_INVALID_DATATYPE,
                      qlgMakeErrPosString( aSQLString,
                                           sSetValue->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_NAME )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_PROPERTY_NOT_EXIST,
                      qlgMakeErrPosString( aSQLString,
                                           sNameValue->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK( aEnv ),
                      sPropertyName );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief ALTER SESSION SET 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlssOptCodeSessionSet( smlTransId      aTransID,
                                 qllDBCStmt    * aDBCStmt,
                                 qllStatement  * aSQLStmt,
                                 qllEnv        * aEnv )
{
    /**
     * nothing to do
     */
    
    return STL_SUCCESS;
}

/**
 * @brief ALTER SESSION SET 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlssOptDataSessionSet( smlTransId      aTransID,
                                 qllDBCStmt    * aDBCStmt,
                                 qllStatement  * aSQLStmt,
                                 qllEnv        * aEnv )
{
    /**
     * nothing to do
     */
    
    return STL_SUCCESS;
}


/**
 * @brief ALTER SESSION SET 을 수행한다
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aDBCStmt  DBC Statement
 * @param[in] aSQLStmt  SQL Statement
 * @param[in] aEnv      Environment
 */
stlStatus qlssExecuteSessionSet( smlTransId      aTransID,
                                 smlStatement  * aStmt,
                                 qllDBCStmt    * aDBCStmt,
                                 qllStatement  * aSQLStmt,
                                 qllEnv        * aEnv )
{
    qlssInitSessionSet  * sInitPlan = NULL;
    knlPropertyDataType   sPropertyType;
    void                * sPropertyValue = NULL;

    /**
     * Init Plan 획득 
     */

    sInitPlan = (qlssInitSessionSet *) aSQLStmt->mInitPlan;

    STL_TRY( knlGetPropertyDataTypeByID( sInitPlan->mPropertyId,
                                         &sPropertyType,
                                         KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    if( sInitPlan->mDefaultValue == STL_FALSE )
    {
        /**
         * Proptery 갱신
         */

        switch( sPropertyType )
        {
            case KNL_PROPERTY_TYPE_BOOL:
                sPropertyValue = (void*)&sInitPlan->mBoolValue;
                break;
            case KNL_PROPERTY_TYPE_BIGINT:
                sPropertyValue = (void*)&sInitPlan->mIntegerValue;
                break;
            case KNL_PROPERTY_TYPE_VARCHAR:
                sPropertyValue = (void*)sInitPlan->mStringValue;
                break;
            default:
                STL_ASSERT( STL_FALSE );
                break;
        }

        STL_TRY( knlUpdatePropertyValueByID( sInitPlan->mPropertyId,
                                             sPropertyValue,
                                             KNL_PROPERTY_MODE_SES,
                                             STL_FALSE,
                                             KNL_PROPERTY_SCOPE_MEMORY,
                                             KNL_ENV( aEnv ) )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( knlInitPropertyValueByID( sInitPlan->mPropertyId,
                                           KNL_PROPERTY_MODE_SES,
                                           STL_FALSE,
                                           KNL_PROPERTY_SCOPE_MEMORY,
                                           KNL_ENV( aEnv ) )
                 == STL_SUCCESS );
    }

    /**
     * Session NLS Propery 일 경우, Session NLS 정보 변경
     */

    switch ( sInitPlan->mPropertyId )
    {
        case KNL_PROPERTY_NLS_DATE_FORMAT:
        case KNL_PROPERTY_NLS_TIME_FORMAT:
        case KNL_PROPERTY_NLS_TIME_WITH_TIME_ZONE_FORMAT:
        case KNL_PROPERTY_NLS_TIMESTAMP_FORMAT:
        case KNL_PROPERTY_NLS_TIMESTAMP_WITH_TIME_ZONE_FORMAT:
            {
                STL_TRY( qllInitSessNLSFromProperty( aEnv ) == STL_SUCCESS );
                break;
            }
        default:
            {
                /* nothing to do */
                break;
            }
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Property Format String 값의 유효성을 검사
 * @param[in] aPropID        Property ID
 * @param[in] aFormatString  Property Format String
 * @param[in] aEnv           Environment
 * @remarks
 * 잘못된 Format String 이 저장될 경우, DB 구동이 실패한다.
 */
stlStatus qlssCheckPropFormatString( knlPropertyID   aPropID,
                                     stlChar       * aFormatString,
                                     qllEnv        * aEnv )
{
    stlInt32 sFormatLen = 0;
    
    /* TO_CHAR() TO_DATETIME() FORMAT 용 임시버퍼 */        
    stlInt64  sFormatInfoBuf[DTL_DATETIME_FORMAT_INFO_MAX_BUFFER_SIZE / STL_SIZEOF(stlInt64)];
    
    dtlDateTimeFormatInfo  * sFormatInfo = (dtlDateTimeFormatInfo *)sFormatInfoBuf;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( (aPropID >= 0) && ( aPropID < KNL_PROPERTY_ID_MAX), QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aFormatString != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * Property 유형별 Format String 의 유효성 검사
     */

    sFormatLen = stlStrlen( aFormatString );    
    
    switch ( aPropID )
    {
        case KNL_PROPERTY_NLS_DATE_FORMAT:
            {
                /**
                 * TO_CHAR 용 Format 정보 구축
                 */
                
                STL_TRY( dtlGetDateTimeFormatInfoForToChar(
                             DTL_TYPE_DATE,  /* DATE format 용임 */
                             aFormatString,
                             sFormatLen,
                             sFormatInfo,
                             DTL_DATETIME_FORMAT_INFO_MAX_BUFFER_SIZE,
                             KNL_DT_VECTOR(aEnv),
                             aEnv,
                             QLL_ERROR_STACK(aEnv) )
                         == STL_SUCCESS );

                /**
                 * TO_DATE 용 Format 정보 구축
                 */
                
                STL_TRY( dtlGetDateTimeFormatInfoForToDateTime(
                             DTL_TYPE_DATE,
                             aFormatString,
                             sFormatLen,
                             STL_TRUE, /* aIsSaveToCharMaxResultLen */
                             sFormatInfo,
                             DTL_DATETIME_FORMAT_INFO_MAX_BUFFER_SIZE,
                             KNL_DT_VECTOR(aEnv),
                             aEnv,
                             QLL_ERROR_STACK(aEnv) )
                         == STL_SUCCESS );
                
                break;
            }
        case KNL_PROPERTY_NLS_TIME_FORMAT:
            {
                /**
                 * TO_CHAR 용 Format 정보 구축
                 */
                
                STL_TRY( dtlGetDateTimeFormatInfoForToChar(
                             DTL_TYPE_TIME,  /* TIME format 용임 */
                             aFormatString,
                             sFormatLen,
                             sFormatInfo,
                             DTL_DATETIME_FORMAT_INFO_MAX_BUFFER_SIZE,
                             KNL_DT_VECTOR(aEnv),
                             aEnv,
                             QLL_ERROR_STACK(aEnv) )
                         == STL_SUCCESS );
                
                /**
                 * TO_DATE 용 Format 정보 구축
                 */
                
                STL_TRY( dtlGetDateTimeFormatInfoForToDateTime(
                             DTL_TYPE_TIME,
                             aFormatString,
                             sFormatLen,
                             STL_TRUE, /* aIsSaveToCharMaxResultLen */
                             sFormatInfo,
                             DTL_DATETIME_FORMAT_INFO_MAX_BUFFER_SIZE,
                             KNL_DT_VECTOR(aEnv),
                             aEnv,
                             QLL_ERROR_STACK(aEnv) )
                         == STL_SUCCESS );
                
                break;
            }
        case KNL_PROPERTY_NLS_TIME_WITH_TIME_ZONE_FORMAT:
            {
                /**
                 * TO_CHAR 용 Format 정보 구축
                 */
                
                STL_TRY( dtlGetDateTimeFormatInfoForToChar(
                             DTL_TYPE_TIME_WITH_TIME_ZONE,  /* TIME WITH TIME ZONE format 용임 */
                             aFormatString,
                             sFormatLen,
                             sFormatInfo,
                             DTL_DATETIME_FORMAT_INFO_MAX_BUFFER_SIZE,
                             KNL_DT_VECTOR(aEnv),
                             aEnv,
                             QLL_ERROR_STACK(aEnv) )
                         == STL_SUCCESS );
                
                /**
                 * TO_DATE 용 Format 정보 구축
                 */
                
                STL_TRY( dtlGetDateTimeFormatInfoForToDateTime(
                             DTL_TYPE_TIME_WITH_TIME_ZONE,
                             aFormatString,
                             sFormatLen,
                             STL_TRUE, /* aIsSaveToCharMaxResultLen */
                             sFormatInfo,
                             DTL_DATETIME_FORMAT_INFO_MAX_BUFFER_SIZE,
                             KNL_DT_VECTOR(aEnv),
                             aEnv,
                             QLL_ERROR_STACK(aEnv) )
                         == STL_SUCCESS );
                
                break;
            }
        case KNL_PROPERTY_NLS_TIMESTAMP_FORMAT:
            {
                /**
                 * TO_CHAR 용 Format 정보 구축
                 */
                
                STL_TRY( dtlGetDateTimeFormatInfoForToChar(
                             DTL_TYPE_TIMESTAMP,  /* TIMESTAMP format 용임 */
                             aFormatString,
                             sFormatLen,
                             sFormatInfo,
                             DTL_DATETIME_FORMAT_INFO_MAX_BUFFER_SIZE,
                             KNL_DT_VECTOR(aEnv),
                             aEnv,
                             QLL_ERROR_STACK(aEnv) )
                         == STL_SUCCESS );
                
                /**
                 * TO_DATE 용 Format 정보 구축
                 */
                
                STL_TRY( dtlGetDateTimeFormatInfoForToDateTime(
                             DTL_TYPE_TIMESTAMP,
                             aFormatString,
                             sFormatLen,
                             STL_TRUE, /* aIsSaveToCharMaxResultLen */
                             sFormatInfo,
                             DTL_DATETIME_FORMAT_INFO_MAX_BUFFER_SIZE,
                             KNL_DT_VECTOR(aEnv),
                             aEnv,
                             QLL_ERROR_STACK(aEnv) )
                         == STL_SUCCESS );
                
                break;
            }
        case KNL_PROPERTY_NLS_TIMESTAMP_WITH_TIME_ZONE_FORMAT:
            {
                /**
                 * TO_CHAR 용 Format 정보 구축
                 */
                
                STL_TRY( dtlGetDateTimeFormatInfoForToChar(
                             DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,  /* TIMESTAMP WITH TIME ZONE format 용임 */
                             aFormatString,
                             sFormatLen,
                             sFormatInfo,
                             DTL_DATETIME_FORMAT_INFO_MAX_BUFFER_SIZE,
                             KNL_DT_VECTOR(aEnv),
                             aEnv,
                             QLL_ERROR_STACK(aEnv) )
                         == STL_SUCCESS );
                
                /**
                 * TO_DATE 용 Format 정보 구축
                 */
                
                STL_TRY( dtlGetDateTimeFormatInfoForToDateTime(
                             DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,
                             aFormatString,
                             sFormatLen,
                             STL_TRUE, /* aIsSaveToCharMaxResultLen */
                             sFormatInfo,
                             DTL_DATETIME_FORMAT_INFO_MAX_BUFFER_SIZE,
                             KNL_DT_VECTOR(aEnv),
                             aEnv,
                             QLL_ERROR_STACK(aEnv) )
                         == STL_SUCCESS );
                
                break;
            }
        default:
            {
                /* nothing to do */
                break;
            }
    }    
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} qlssSessionAlterSessionSet */
