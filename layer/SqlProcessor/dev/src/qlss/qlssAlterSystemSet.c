/*******************************************************************************
 * qlssAlterSystemSet.c
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
 * @file qlssAlterSystemSet.c
 * @brief ALTER SYSTEM SET 처리 루틴 
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @defgroup qlssSystemAlterSystemSet ALTER SYSTEM SET property
 * @ingroup qlssSystem
 * @{
 */

/**
 * @brief ALTER SYSTEM SET 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlssValidateSystemSet( smlTransId      aTransID,
                                 qllDBCStmt    * aDBCStmt,
                                 qllStatement  * aSQLStmt,
                                 stlChar       * aSQLString,
                                 qllNode       * aParseTree,
                                 qllEnv        * aEnv )
{
    qlssInitSystemSet   * sInitPlan   = NULL;
    qlpAlterSystem      * sParseTree  = NULL;
    knlPropertyID         sPropertyId = KNL_PROPERTY_ID_MAX;
    qlpValue            * sSetValue;
    qlpValue            * sNameValue;
    knlPropertyDataType   sPropertyType;
    stlInt64              sIntSetValue;
    stlChar             * sPtrSetValue;
    stlChar             * sPropertyName = NULL;
    stlBool               sExist = STL_FALSE;
    stlInt64              sByteUnit = 1;
    stlInt64              sDatabaseTestOption = 0;

    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );

    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_SYSTEM_SET_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_ALTER_SYSTEM_SET_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득 
     */
    
    sParseTree = (qlpAlterSystem *) aParseTree;

    STL_PARAM_VALIDATE( sParseTree->mPropertyName != NULL,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( ( sParseTree->mScope >= KNL_PROPERTY_SCOPE_MEMORY ) &&
                        ( sParseTree->mScope < KNL_PROPERTY_SCOPE_MAX ),
                        QLL_ERROR_STACK(aEnv) );
    
    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qlssInitSystemSet),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qlssInitSystemSet) );


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

    STL_TRY( knlGetPropertyIDByName( sPropertyName,
                                     &sPropertyId,
                                     KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    sInitPlan->mPropertyId = sPropertyId;
    sInitPlan->mIsDeferred = sParseTree->mIsDeferred;
    sInitPlan->mScope      = sParseTree->mScope;

    sDatabaseTestOption = knlGetPropertyBigIntValueByID( KNL_PROPERTY_DATABASE_TEST_OPTION,
                                                         KNL_ENV(aEnv) );

    if( sDatabaseTestOption == 1 )
    {
        if( sPropertyId == KNL_PROPERTY_QUERY_TIMEOUT )
        {
            STL_THROW( RAMP_ERR_DISALLOW_STATEMENT );
        }
    }

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
     * ALTER SYSTEM ON DATABASE 권한 검사
     */

    STL_TRY( qlaCheckDatabasePriv( aTransID,
                                   aDBCStmt,
                                   aSQLStmt,
                                   ELL_DB_PRIV_ACTION_ALTER_SYSTEM,
                                   aEnv )
             == STL_SUCCESS );
    
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

    STL_CATCH( RAMP_ERR_DISALLOW_STATEMENT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_DISALLOW_STATEMENT,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief ALTER SYSTEM SET 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlssOptCodeSystemSet( smlTransId      aTransID,
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
 * @brief ALTER SYSTEM SET 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlssOptDataSystemSet( smlTransId      aTransID,
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
 * @brief ALTER SYSTEM SET 을 수행한다
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aDBCStmt  DBC Statement
 * @param[in] aSQLStmt  SQL Statement
 * @param[in] aEnv      Environment
 */
stlStatus qlssExecuteSystemSet( smlTransId      aTransID,
                                smlStatement  * aStmt,
                                qllDBCStmt    * aDBCStmt,
                                qllStatement  * aSQLStmt,
                                qllEnv        * aEnv )
{
    qlssInitSystemSet   * sInitPlan = NULL;
    knlPropertyDataType   sPropertyType;
    void                * sPropertyValue = NULL;

    /**
     * Init Plan 획득 
     */

    sInitPlan = (qlssInitSystemSet *) aSQLStmt->mInitPlan;

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
                                             KNL_PROPERTY_MODE_SYS,
                                             sInitPlan->mIsDeferred,
                                             sInitPlan->mScope,
                                             KNL_ENV( aEnv ) )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( knlInitPropertyValueByID( sInitPlan->mPropertyId,
                                           KNL_PROPERTY_MODE_SYS,
                                           sInitPlan->mIsDeferred,
                                           sInitPlan->mScope,
                                           KNL_ENV( aEnv ) )
                 == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} qlssSystemAlterSystemSet */


