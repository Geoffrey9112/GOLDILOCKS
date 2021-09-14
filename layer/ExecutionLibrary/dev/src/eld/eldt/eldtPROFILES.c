/*******************************************************************************
 * eldtPROFILES.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: eldtPROFILES.c 14729 2015-04-06 03:16:14Z leekmo $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


/**
 * @file eldtPROFILES.c
 * @brief DEFINITION_SCHEMA.PROFILES 정의 명세  
 *
 */

#include <ell.h>
#include <eldt.h>

/**
 * @addtogroup eldtPROFILES
 * @{
 */

/**
 * @brief DEFINITION_SCHEMA.PROFILES 의 컬럼 정의
 */
eldtDefinitionColumnDesc  gEldtColumnDescPROFILES[ELDT_Profile_ColumnOrder_MAX] =
{
    {
        ELDT_TABLE_ID_PROFILES,                 /**< Table ID */
        "PROFILE_ID",                           /**< 컬럼의 이름  */
        ELDT_Profile_ColumnOrder_PROFILE_ID,    /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SERIAL_NUMBER,              /**< 컬럼의 Domain */
        ELDT_NULLABLE_PK_NOT_NULL,              /**< 컬럼의 Nullable 여부 */
        "profile identifier"        
    },
    {
        ELDT_TABLE_ID_PROFILES,                 /**< Table ID */
        "PROFILE_NAME",                         /**< 컬럼의 이름  */
        ELDT_Profile_ColumnOrder_PROFILE_NAME,  /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_SQL_IDENTIFIER,             /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "profile name"        
    },
    {
        ELDT_TABLE_ID_PROFILES,                 /**< Table ID */
        "IS_BUILTIN",                           /**< 컬럼의 이름  */
        ELDT_Profile_ColumnOrder_IS_BUILTIN,    /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_YES_OR_NO,                  /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "is built-in object or not"        
    },
    
    {
        ELDT_TABLE_ID_PROFILES,                 /**< Table ID */
        "CREATED_TIME",                         /**< 컬럼의 이름  */
        ELDT_Profile_ColumnOrder_CREATED_TIME,  /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_TIME_STAMP,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "created time of the profile"        
    },
    {
        ELDT_TABLE_ID_PROFILES,                 /**< Table ID */
        "MODIFIED_TIME",                        /**< 컬럼의 이름  */
        ELDT_Profile_ColumnOrder_MODIFIED_TIME, /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_TIME_STAMP,                 /**< 컬럼의 Domain */
        ELDT_NULLABLE_COLUMN_NOT_NULL,          /**< 컬럼의 Nullable 여부 */
        "last modified time of the profile"        
    },
    {
        ELDT_TABLE_ID_PROFILES,                 /**< Table ID */
        "COMMENTS",                             /**< 컬럼의 이름  */
        ELDT_Profile_ColumnOrder_COMMENTS,      /**< 컬럼의 순서 위치 */
        ELDT_DOMAIN_LONG_DESC,                  /**< 컬럼의 Domain */
        ELDT_NULLABLE_YES,                      /**< 컬럼의 Nullable 여부 */
        "comments of the profile"        
    }
};


/**
 * @brief DEFINITION_SCHEMA.PROFILES 의 테이블 정의
 */
eldtDefinitionTableDesc gEldtTableDescPROFILES =
{
    ELDT_TABLE_ID_PROFILES,                  /**< Table ID */
    "PROFILES",                              /**< 테이블의 이름  */
    "The PROFILES table has one row for each profile. "
};


/**
 * @brief DEFINITION_SCHEMA.PROFILES 의 KEY 제약조건 정의
 */
eldtDefinitionKeyConstDesc  gEldtKeyConstDescPROFILES[ELDT_Profile_Const_MAX] =
{
    {
        ELDT_TABLE_ID_PROFILES,                      /**< Table ID */
        ELDT_Profile_Const_PRIMARY_KEY,              /**< Table 내 Constraint 번호 */
        ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY,       /**< Table Constraint 유형 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                                /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_Profile_ColumnOrder_PROFILE_ID,
        },
        ELL_DICT_OBJECT_ID_NA,                       /**< 참조 제약이 참조하는 Table ID */
        ELL_DICT_OBJECT_ID_NA,                       /**< 참조 제약이 참조하는 Unique의 번호 */
    },
    {
        ELDT_TABLE_ID_PROFILES,                      /**< Table ID */
        ELDT_Profile_Const_UNIQUE_PROFILE_NAME,      /**< Table 내 Constraint 번호 */
        ELL_TABLE_CONSTRAINT_TYPE_UNIQUE_KEY,        /**< Table Constraint 유형 */
        1,                                           /**< 키를 구성하는 컬럼의 개수 */
        {                                /**< 키를 구성하는 컬럼의 테이블내 Ordinal Position */
            ELDT_Profile_ColumnOrder_PROFILE_NAME,
        },
        ELL_DICT_OBJECT_ID_NA,                       /**< 참조 제약이 참조하는 Table ID */
        ELL_DICT_OBJECT_ID_NA,                       /**< 참조 제약이 참조하는 Unique의 번호 */
    }
};




/**
 * @brief DEFINITION_SCHEMA 의 built-in profile, DEFAULT
 */
eldtBuiltInProfileDesc gEldtBuiltInProfileDEFAULT = 
{
    ELDT_PROFILE_ID_DEFAULT,                        /**< Profile ID */
    "DEFAULT",                                      /**< Profile Name */
    "The DEFAULT profile is a built-in profile.",   /**< Profile 의 주석 */
    {
        {
            ELL_PROFILE_PARAM_NA,
            0,
            NULL,
        },
        {
            ELL_PROFILE_PARAM_FAILED_LOGIN_ATTEMPTS,
            ELDT_DEFAULT_LIMIT_FAILED_LOGIN_ATTEMPTS,
            ELDT_DEFAULT_LIMIT_FAILED_LOGIN_ATTEMPTS_STRING,
        },
        {
            ELL_PROFILE_PARAM_PASSWORD_LOCK_TIME,
            ELDT_DEFAULT_LIMIT_PASSWORD_LOCK_TIME,
            ELDT_DEFAULT_LIMIT_PASSWORD_LOCK_TIME_STRING
        },
        {
            ELL_PROFILE_PARAM_PASSWORD_LIFE_TIME,
            ELDT_DEFAULT_LIMIT_PASSWORD_LIFE_TIME,
            ELDT_DEFAULT_LIMIT_PASSWORD_LIFE_TIME_STRING
        },
        {
            ELL_PROFILE_PARAM_PASSWORD_GRACE_TIME,
            ELDT_DEFAULT_LIMIT_PASSWORD_GRACE_TIME,
            ELDT_DEFAULT_LIMIT_PASSWORD_GRACE_TIME_STRING
        },
        {
            ELL_PROFILE_PARAM_PASSWORD_REUSE_MAX,
            ELDT_DEFAULT_LIMIT_PASSWORD_REUSE_MAX,
            ELDT_DEFAULT_LIMIT_PASSWORD_REUSE_MAX_STRING
        },
        {
            ELL_PROFILE_PARAM_PASSWORD_REUSE_TIME,
            ELDT_DEFAULT_LIMIT_PASSWORD_REUSE_TIME,
            ELDT_DEFAULT_LIMIT_PASSWORD_REUSE_TIME_STRING
        },
        {
            ELL_PROFILE_PARAM_PASSWORD_VERIFY_FUNCTION,
            ELDT_DEFAULT_LIMIT_PASSWORD_VERIFY_FUNCITON,
            ELDT_DEFAULT_LIMIT_PASSWORD_VERIFY_FUNCITON_STRING
        }
    }
};

/**
 * @brief DEFINITION_SCHEMA 의 built-in profile 들
 */

eldtBuiltInProfileDesc * gEldtBuiltInProfileDesc[] =
{
    & gEldtBuiltInProfileDEFAULT,
    NULL
};


/**
 * @brief Built-In Profile Descriptor 를 추가한다.
 * @param[in]  aTransID          Transaction ID
 * @param[in]  aStmt             Statement
 * @param[in]  aEnv              Execution Library Environment
 * @remarks
 */
stlStatus eldtInsertBuiltInProfileDesc( smlTransId     aTransID,
                                        smlStatement * aStmt,
                                        ellEnv       * aEnv )
{
    stlInt32               i = 0;
    stlInt32               j = 0;
    
    stlInt64                  sProfileID   = 0;
    eldtBuiltInProfileDesc  * sProfileDesc = NULL;
    eldtBuiltInProfileParam * sProfileParam = NULL;

    dtlDataValue sNumberValue;
    stlChar      sNumberBuffer[DTL_NUMERIC_MAX_SIZE];
    stlBool      sSuccessWithInfo = STL_FALSE;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Number Value 초기화
     */
    
    sNumberValue.mType       = DTL_TYPE_NUMBER;
    sNumberValue.mBufferSize = DTL_NUMERIC_MAX_SIZE;
    sNumberValue.mLength     = 0;
    sNumberValue.mValue      = sNumberBuffer;
    
    /**
     * built-in profile 정보 추가 
     */
    
    for ( i = 0; gEldtBuiltInProfileDesc[i] != NULL; i++ )
    {
        /***************************************************************
         * built-in profile descriptor 를 추가함
         ***************************************************************/
    
        sProfileDesc = gEldtBuiltInProfileDesc[i];
        
        STL_TRY( ellInsertProfileDesc( aTransID,
                                       aStmt,
                                       & sProfileID,                   /* Profile ID */
                                       sProfileDesc->mProfileName,     /* Profile Name */
                                       sProfileDesc->mProfileComment,  /* Profile Comment */
                                       aEnv )
                 == STL_SUCCESS );

        /*
         * Enumeration 정의 순서와 DATABASE 정보가 일치하는 지 검증 
         */ 
        STL_DASSERT( sProfileID == sProfileDesc->mProfileID );

        /***************************************************************
         * built-in profile password parameter 를 추가함
         ***************************************************************/

        for ( j = ELL_PROFILE_PARAM_NA + 1; j < ELL_PROFILE_PARAM_MAX; j++ )
        {
            sProfileParam = & gEldtBuiltInProfileDesc[i]->mPasswordParam[j];
            STL_DASSERT( sProfileParam != NULL );

            switch( sProfileParam->mParamID )
            {
                case ELL_PROFILE_PARAM_FAILED_LOGIN_ATTEMPTS:
                case ELL_PROFILE_PARAM_PASSWORD_REUSE_MAX:
                    {
                        STL_TRY( ellInsertProfileParamInteger( aTransID,
                                                               aStmt,
                                                               sProfileID,
                                                               sProfileParam->mParamID,
                                                               STL_FALSE, /* aIsDefault */
                                                               sProfileParam->mLimitValue,
                                                               sProfileParam->mLimitString,
                                                               aEnv )
                                 == STL_SUCCESS );
                        break;
                    }
                case ELL_PROFILE_PARAM_PASSWORD_LOCK_TIME:
                case ELL_PROFILE_PARAM_PASSWORD_LIFE_TIME:
                case ELL_PROFILE_PARAM_PASSWORD_GRACE_TIME:
                case ELL_PROFILE_PARAM_PASSWORD_REUSE_TIME:
                    {
                        if ( sProfileParam->mLimitValue == ELL_PROFILE_LIMIT_UNLIMITED )
                        {
                            DTL_SET_NULL( & sNumberValue );
                        }
                        else
                        {
                            STL_TRY( dtlSetValueFromInteger( sNumberValue.mType,
                                                             sProfileParam->mLimitValue,
                                                             DTL_NUMERIC_MAX_PRECISION, 
                                                             DTL_SCALE_NA,
                                                             DTL_STRING_LENGTH_UNIT_NA,
                                                             DTL_INTERVAL_INDICATOR_NA,
                                                             sNumberValue.mBufferSize, 
                                                             & sNumberValue,
                                                             & sSuccessWithInfo,
                                                             KNL_DT_VECTOR(aEnv),
                                                             aEnv,
                                                             ELL_ERROR_STACK(aEnv) )
                                     == STL_SUCCESS );
                        }

                        STL_TRY( ellInsertProfileParamIntervalNumber( aTransID,
                                                                      aStmt,
                                                                      sProfileID,
                                                                      sProfileParam->mParamID,
                                                                      STL_FALSE, /* aIsDefault */
                                                                      & sNumberValue,
                                                                      sProfileParam->mLimitString,
                                                                      aEnv )
                                 == STL_SUCCESS );
                        
                        break;
                    }
                case ELL_PROFILE_PARAM_PASSWORD_VERIFY_FUNCTION:
                    {
                        STL_TRY( ellInsertProfileParamVerifyFunction( aTransID,
                                                                      aStmt,
                                                                      sProfileID,
                                                                      STL_FALSE, /* aIsDefault */
                                                                      sProfileParam->mLimitValue,
                                                                      sProfileParam->mLimitString,
                                                                      aEnv )
                                 == STL_SUCCESS );
                        break;
                    }
                default:
                    {
                        STL_DASSERT(0);
                        break;
                    }
            }
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/** @} eldtPROFILES */

