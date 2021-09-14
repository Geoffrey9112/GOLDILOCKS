/*******************************************************************************
 * ellDictProfile.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: ellDictProfile.c 15300 2015-06-29 02:41:26Z kja $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


/**
 * @file ellDictProfile.c
 * @brief Profile Dictionary 관리 모듈
 */

#include <ell.h>
#include <elDef.h>

#include <eldt.h>
#include <eldc.h>
#include <eldDictionary.h>
#include <eldProfile.h>
#include <eldProfilePasswordParameter.h>

/**
 * @addtogroup ellObjectProfile
 * @{
 */

const stlChar * const gEllProfileParamString[ELL_PROFILE_PARAM_MAX] =
{
    "N/A",                        /* ELL_PROFILE_PARAM_NA */
    
    "FAILED_LOGIN_ATTEMPTS",      /* ELL_PROFILE_PARAM_FAILED_LOGIN_ATTEMPTS */
    "PASSWORD_LOCK_TIME",         /* ELL_PROFILE_PARAM_PASSWORD_LOCK_TIME */
    "PASSWORD_LIFE_TIME",         /* ELL_PROFILE_PARAM_PASSWORD_LIFE_TIME */
    "PASSWORD_GRACE_TIME",        /* ELL_PROFILE_PARAM_PASSWORD_GRACE_TIME */
    "PASSWORD_REUSE_MAX",         /* ELL_PROFILE_PARAM_PASSWORD_REUSE_MAX */
    "PASSWORD_REUSE_TIME",        /* ELL_PROFILE_PARAM_PASSWORD_REUSE_TIME */
    "PASSWORD_VERIFY_FUNCTION",   /* ELL_PROFILE_PARAM_PASSWORD_VERIFY_FUNCTION */
    
};

/*********************************************************
 * DDL 관련 Object 획득 함수
 *********************************************************/


/**
 * @brief DROP PROFILE 을 위한 Profile 이 할당된 User List 획득
 * @param[in]  aTransID               Transaction ID
 * @param[in]  aStmt                  Statement
 * @param[in]  aProfileHandle         Profile Dictionary Handle
 * @param[in]  aRegionMem             Region Memory
 * @param[out] aUserList              User List
 * @param[in]  aEnv                   Environment
 * @remarks
 */
stlStatus ellGetUserList4DropProfile( smlTransId       aTransID,
                                      smlStatement   * aStmt,
                                      ellDictHandle  * aProfileHandle,
                                      knlRegionMem   * aRegionMem,
                                      ellObjectList ** aUserList,
                                      ellEnv         * aEnv )
{
    stlInt64   sProfileID = ELL_DICT_OBJECT_ID_NA;

    ellObjectList * sUserList = NULL;

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aProfileHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aUserList != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */

    sProfileID = ellGetProfileID( aProfileHandle );

    /*********************************************
     * sUserList
     *********************************************/

    /**
     * - sUserList : Profile 가 소유한 Schema 목록
     *
     * <BR> SELECT usr.PROFILE_ID
     * <BR>   FROM USERS usr
     * <BR>  WHERE usr.PROFILE_ID = sProfileID
     */

    STL_TRY( ellInitObjectList( & sUserList, aRegionMem, aEnv ) == STL_SUCCESS );

    STL_TRY( eldGetObjectList( aTransID,
                               aStmt,
                               ELDT_TABLE_ID_USERS,
                               ELL_OBJECT_AUTHORIZATION,
                               ELDT_User_ColumnOrder_AUTH_ID,
                               ELDT_User_ColumnOrder_PROFILE_ID,
                               sProfileID,
                               aRegionMem,
                               sUserList,
                               aEnv )
             == STL_SUCCESS );

    /**
     * Output 설정
     */

    *aUserList = sUserList;
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/**
 * @brief ALTER PROFILE 을 위한 Profile 이 할당된 User List 획득
 * @param[in]  aTransID               Transaction ID
 * @param[in]  aStmt                  Statement
 * @param[in]  aProfileHandle         Profile Dictionary Handle
 * @param[in]  aRegionMem             Region Memory
 * @param[out] aUserList              User List
 * @param[in]  aEnv                   Environment
 * @remarks
 */
stlStatus ellGetUserList4AlterProfile( smlTransId       aTransID,
                                       smlStatement   * aStmt,
                                       ellDictHandle  * aProfileHandle,
                                       knlRegionMem   * aRegionMem,
                                       ellObjectList ** aUserList,
                                       ellEnv         * aEnv )
{
    return ellGetUserList4DropProfile( aTransID,
                                       aStmt,
                                       aProfileHandle,
                                       aRegionMem,
                                       aUserList,
                                       aEnv );
}

/*********************************************************
 * DDL Lock 함수 
 *********************************************************/


/**
 * @brief DROP PROFILE 구문을 위한 DDL Lock 을 획득한다.
 * @param[in]  aTransID          Transaction ID
 * @param[in]  aStmt             Statement
 * @param[in]  aProfileHandle    Profile Handle
 * @param[out] aLockSuccess      Lock 획득 여부 
 * @param[in]  aEnv              Environment
 * @remarks
 */
stlStatus ellLock4DropProfile( smlTransId      aTransID,
                               smlStatement  * aStmt,
                               ellDictHandle * aProfileHandle,
                               stlBool       * aLockSuccess,
                               ellEnv        * aEnv )
{
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aProfileHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aLockSuccess != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * Profile 이 변경되지 않아야 함.
     */
    
    STL_TRY( eldLockRowByObjectHandle( aTransID,
                                       aStmt,
                                       ELDT_TABLE_ID_PROFILES,
                                       ELDT_Profile_ColumnOrder_PROFILE_ID,
                                       aProfileHandle,
                                       SML_LOCK_X,
                                       aLockSuccess,
                                       aEnv )
             == STL_SUCCESS );

    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}



/**
 * @brief ALTER PROFILE 구문을 위한 DDL Lock 을 획득한다.
 * @param[in]  aTransID          Transaction ID
 * @param[in]  aStmt             Statement
 * @param[in]  aProfileHandle    Profile Handle
 * @param[out] aLockSuccess      Lock 획득 여부 
 * @param[in]  aEnv              Environment
 * @remarks
 */
stlStatus ellLock4AlterProfile( smlTransId      aTransID,
                                smlStatement  * aStmt,
                                ellDictHandle * aProfileHandle,
                                stlBool       * aLockSuccess,
                                ellEnv        * aEnv )
{
    return ellLock4DropProfile( aTransID,
                                aStmt,
                                aProfileHandle,
                                aLockSuccess,
                                aEnv );
}

/*********************************************************
 * Dictionary 변경 함수 
 *********************************************************/


/**
 * @brief Profile Descriptor 를 추가
 * @param[in]  aTransID               Transaction ID
 * @param[in]  aStmt                  Statement
 * @param[out] aProfileID             Profile ID
 * @param[in]  aProfileName           Profile Name
 * @param[in]  aProfileComment        Profile Comment
 * @param[in]  aEnv                   Environment
 * @remarks
 */
stlStatus ellInsertProfileDesc( smlTransId             aTransID,
                                smlStatement         * aStmt,
                                stlInt64             * aProfileID,
                                stlChar              * aProfileName,
                                stlChar              * aProfileComment,
                                ellEnv               * aEnv )
{
    STL_TRY( eldInsertProfileDesc( aTransID,
                                   aStmt,
                                   aProfileID,
                                   aProfileName,
                                   aProfileComment,
                                   aEnv )
             == STL_SUCCESS );

    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}



/**
 * @brief DROP PRORFILE 를 위한 Dictionary Row 를 제거 
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aProfileHandle   Profile Dictionary Handle
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus ellRemoveDict4DropProfile( smlTransId        aTransID,
                                     smlStatement    * aStmt,
                                     ellDictHandle   * aProfileHandle,
                                     ellEnv          * aEnv )
{
    stlInt64 sProfileID = ELL_DICT_OBJECT_ID_NA;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aProfileHandle != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */

    sProfileID = ellGetProfileID( aProfileHandle );
    
    /**
     * Profile Password Parameter 제거 
     */

    /*
     * DELETE FROM DEFINITION_SCHEMA.PROFILE_PASSWORD_PARAMETER
     *  WHERE PROFILE_ID = sProfileID;
     */
    
    STL_TRY( eldDeleteObjectRows( aTransID,
                                  aStmt,
                                  ELDT_TABLE_ID_PROFILE_PASSWORD_PARAMETER,
                                  ELDT_PassParam_ColumnOrder_PROFILE_ID,
                                  sProfileID,
                                  aEnv )
             == STL_SUCCESS );

    /**
     * Profile Kernel Parameter 제거 
     */

    /*
     * DELETE FROM DEFINITION_SCHEMA.PROFILE_KERNEL_PARAMETER
     *  WHERE PROFILE_ID = sProfileID;
     */
    
    STL_TRY( eldDeleteObjectRows( aTransID,
                                  aStmt,
                                  ELDT_TABLE_ID_PROFILE_KERNEL_PARAMETER,
                                  ELDT_KernelParam_ColumnOrder_PROFILE_ID,
                                  sProfileID,
                                  aEnv )
             == STL_SUCCESS );

    
    /**
     * Profile Descriptor 제거 
     */

    /*
     * DELETE FROM DEFINITION_SCHEMA.PROFILES
     *  WHERE PROFILE_ID = sProfileID;
     */
    
    STL_TRY( eldDeleteObjectRows( aTransID,
                                  aStmt,
                                  ELDT_TABLE_ID_PROFILES,
                                  ELDT_Profile_ColumnOrder_PROFILE_ID,
                                  sProfileID,
                                  aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}

/**
 * @brief Integer 계열의 Profile Parameter Descriptor 를 추가
 * @param[in]  aTransID               Transaction ID
 * @param[in]  aStmt                  Statement
 * @param[in]  aProfileID             Profile ID
 * @param[in]  aProfileParam          Profile Parameter ID
 * @param[in]  aIsDefault             LIMIT parameter DEFAULT 여부 
 * @param[in]  aLimitIntValue         LIMIT Integer Value
 * @param[in]  aLimitString           LIMIT string
 * @param[in]  aEnv                   Environment
 * @remarks
 */
stlStatus ellInsertProfileParamInteger( smlTransId             aTransID,
                                        smlStatement         * aStmt,
                                        stlInt64               aProfileID,
                                        ellProfileParam        aProfileParam,
                                        stlBool                aIsDefault,
                                        stlInt64               aLimitIntValue,
                                        stlChar              * aLimitString,
                                        ellEnv               * aEnv )
{
    dtlDataValue sNumberValue;
    stlChar      sNumberBuffer[DTL_NUMERIC_MAX_SIZE];
    stlBool      sSuccessWithInfo = STL_FALSE;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( (aProfileParam == ELL_PROFILE_PARAM_FAILED_LOGIN_ATTEMPTS) ||
                        (aProfileParam == ELL_PROFILE_PARAM_PASSWORD_REUSE_MAX)
                        , ELL_ERROR_STACK(aEnv) );
    
    /**
     * Number Value 로 변환
     */

    sNumberValue.mType       = DTL_TYPE_NUMBER;
    sNumberValue.mBufferSize = DTL_NUMERIC_MAX_SIZE;
    sNumberValue.mLength     = 0;
    sNumberValue.mValue      = sNumberBuffer;
    
    if ( aIsDefault == STL_TRUE )
    {
        DTL_SET_NULL( & sNumberValue );
    }
    else
    {
        if ( aLimitIntValue == ELL_PROFILE_LIMIT_UNLIMITED )
        {
            DTL_SET_NULL( & sNumberValue );
        }
        else
        {
            STL_TRY( dtlSetValueFromInteger( sNumberValue.mType,
                                             aLimitIntValue,
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
    }

    /**
     * PROFILE_PASSWORD_PARAMETER 에 추가
     */

    STL_TRY( eldInsertProfilePassParamDesc( aTransID,
                                            aStmt,
                                            aProfileID,
                                            aProfileParam,
                                            aIsDefault,
                                            & sNumberValue,
                                            aLimitString,
                                            aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}



/**
 * @brief Interval NUMBER 계열의 Profile Parameter Descriptor 를 추가
 * @param[in]  aTransID               Transaction ID
 * @param[in]  aStmt                  Statement
 * @param[in]  aProfileID             Profile ID
 * @param[in]  aProfileParam          Profile Parameter ID
 * @param[in]  aIsDefault             LIMIT parameter DEFAULT 여부 
 * @param[in]  aLimitValue            LIMIT interval number value
 * @param[in]  aLimitString           LIMIT string
 * @param[in]  aEnv                   Environment
 * @remarks
 */
stlStatus ellInsertProfileParamIntervalNumber( smlTransId             aTransID,
                                               smlStatement         * aStmt,
                                               stlInt64               aProfileID,
                                               ellProfileParam        aProfileParam,
                                               stlBool                aIsDefault,
                                               dtlDataValue         * aLimitValue,
                                               stlChar              * aLimitString,
                                               ellEnv               * aEnv )
{
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( (aProfileParam == ELL_PROFILE_PARAM_PASSWORD_LOCK_TIME) ||
                        (aProfileParam == ELL_PROFILE_PARAM_PASSWORD_LIFE_TIME) ||
                        (aProfileParam == ELL_PROFILE_PARAM_PASSWORD_GRACE_TIME) ||
                        (aProfileParam == ELL_PROFILE_PARAM_PASSWORD_REUSE_TIME)
                        , ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aLimitValue != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * PROFILE_PASSWORD_PARAMETER 에 추가
     */
    
    STL_TRY( eldInsertProfilePassParamDesc( aTransID,
                                            aStmt,
                                            aProfileID,
                                            aProfileParam,
                                            aIsDefault,
                                            aLimitValue,
                                            aLimitString,
                                            aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/**
 * @brief Password Verify Function Parameter Descriptor 를 추가
 * @param[in]  aTransID               Transaction ID
 * @param[in]  aStmt                  Statement
 * @param[in]  aProfileID             Profile ID
 * @param[in]  aIsDefault             PASSWORD_VERIFY_FUNCTION DEFAULT 여부 
 * @param[in]  aFunctionID            stored function ID
 * @param[in]  aFunctionString        storing function string
 * @param[in]  aEnv                   Environment
 * @remarks
 */
stlStatus ellInsertProfileParamVerifyFunction( smlTransId             aTransID,
                                               smlStatement         * aStmt,
                                               stlInt64               aProfileID,
                                               stlBool                aIsDefault,
                                               stlInt64               aFunctionID,
                                               stlChar              * aFunctionString,
                                               ellEnv               * aEnv )
{
    dtlDataValue sNumberValue;
    stlChar      sNumberBuffer[DTL_NUMERIC_MAX_SIZE];
    stlBool      sSuccessWithInfo = STL_FALSE;

    /**
     * Parameter Validation
     */

    /* stored function 이 구현되어야 함 */
    STL_PARAM_VALIDATE( (aFunctionID <= ELL_DICT_OBJECT_ID_NA) &&
                        (aFunctionID > ELL_PROFILE_VERIFY_MIN),
                        ELL_ERROR_STACK(aEnv) );
    
    /**
     * Number Value 로 변환
     */

    sNumberValue.mType       = DTL_TYPE_NUMBER;
    sNumberValue.mBufferSize = DTL_NUMERIC_MAX_SIZE;
    sNumberValue.mLength     = 0;
    sNumberValue.mValue      = sNumberBuffer;
    
    if ( aIsDefault == STL_TRUE )
    {
        DTL_SET_NULL( & sNumberValue );
    }
    else
    {
        if ( aFunctionID == ELL_DICT_OBJECT_ID_NA )
        {
            DTL_SET_NULL( & sNumberValue );
        }
        else
        {
            STL_TRY( dtlSetValueFromInteger( sNumberValue.mType,
                                             aFunctionID,
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
    }

    /**
     * PROFILE_PASSWORD_PARAMETER 에 추가
     */

    STL_TRY( eldInsertProfilePassParamDesc( aTransID,
                                            aStmt,
                                            aProfileID,
                                            ELL_PROFILE_PARAM_PASSWORD_VERIFY_FUNCTION,
                                            aIsDefault,
                                            & sNumberValue,
                                            aFunctionString,
                                            aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/**
 * @brief Integer 계열의 Profile Parameter Descriptor 를 변경
 * @param[in]  aTransID               Transaction ID
 * @param[in]  aStmt                  Statement
 * @param[in]  aProfileHandle         Profile Handle
 * @param[in]  aProfileParam          Profile Parameter ID
 * @param[in]  aIsDefault             LIMIT parameter DEFAULT 여부 
 * @param[in]  aLimitIntValue         LIMIT Integer Value
 * @param[in]  aLimitString           LIMIT string
 * @param[in]  aEnv                   Environment
 * @remarks
 */
stlStatus ellModifyProfileParamInteger( smlTransId             aTransID,
                                        smlStatement         * aStmt,
                                        ellDictHandle        * aProfileHandle,
                                        ellProfileParam        aProfileParam,
                                        stlBool                aIsDefault,
                                        stlInt64               aLimitIntValue,
                                        stlChar              * aLimitString,
                                        ellEnv               * aEnv )
{
    stlInt64 sProfileID = ELL_DICT_OBJECT_ID_NA;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aProfileHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aProfileParam == ELL_PROFILE_PARAM_FAILED_LOGIN_ATTEMPTS) ||
                        (aProfileParam == ELL_PROFILE_PARAM_PASSWORD_REUSE_MAX)
                        , ELL_ERROR_STACK(aEnv) );
    

    /**
     * 기본 정보 획득
     */

    sProfileID = ellGetProfileID( aProfileHandle );

    /**
     * Profile Parameter 제거
     */

    STL_TRY( eldDeleteProfilePassParam( aTransID,
                                        aStmt,
                                        sProfileID,
                                        aProfileParam,
                                        aEnv )
             == STL_SUCCESS );
    
    /**
     * Profile Parameter 추가
     */
    
    STL_TRY( ellInsertProfileParamInteger( aTransID,
                                           aStmt,
                                           sProfileID,
                                           aProfileParam,
                                           aIsDefault,
                                           aLimitIntValue,
                                           aLimitString,
                                           aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}



/**
 * @brief Interval NUMBER 계열의 Profile Parameter 를 변경
 * @param[in]  aTransID               Transaction ID
 * @param[in]  aStmt                  Statement
 * @param[in]  aProfileHandle         Profile Handle
 * @param[in]  aProfileParam          Profile Parameter ID
 * @param[in]  aIsDefault             LIMIT parameter DEFAULT 여부 
 * @param[in]  aLimitValue            LIMIT interval number value
 * @param[in]  aLimitString           LIMIT string
 * @param[in]  aEnv                   Environment
 * @remarks
 */
stlStatus ellModifyProfileParamIntervalNumber( smlTransId             aTransID,
                                               smlStatement         * aStmt,
                                               ellDictHandle        * aProfileHandle,
                                               ellProfileParam        aProfileParam,
                                               stlBool                aIsDefault,
                                               dtlDataValue         * aLimitValue,
                                               stlChar              * aLimitString,
                                               ellEnv               * aEnv )
{
    stlInt64 sProfileID = ELL_DICT_OBJECT_ID_NA;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aProfileHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aProfileParam == ELL_PROFILE_PARAM_PASSWORD_LOCK_TIME) ||
                        (aProfileParam == ELL_PROFILE_PARAM_PASSWORD_LIFE_TIME) ||
                        (aProfileParam == ELL_PROFILE_PARAM_PASSWORD_GRACE_TIME) ||
                        (aProfileParam == ELL_PROFILE_PARAM_PASSWORD_REUSE_TIME)
                        , ELL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */

    sProfileID = ellGetProfileID( aProfileHandle );

    /**
     * Profile Parameter 제거
     */

    STL_TRY( eldDeleteProfilePassParam( aTransID,
                                        aStmt,
                                        sProfileID,
                                        aProfileParam,
                                        aEnv )
             == STL_SUCCESS );
    
    
    /**
     * Profile Parameter 추가
     */
    
    STL_TRY( ellInsertProfileParamIntervalNumber( aTransID,
                                                  aStmt,
                                                  sProfileID,
                                                  aProfileParam,
                                                  aIsDefault,
                                                  aLimitValue,
                                                  aLimitString,
                                                  aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/**
 * @brief Verify Function Parameter Descriptor 를 변경
 * @param[in]  aTransID               Transaction ID
 * @param[in]  aStmt                  Statement
 * @param[in]  aProfileHandle         Profile Handle
 * @param[in]  aIsDefault             LIMIT parameter DEFAULT 여부 
 * @param[in]  aFunctionID            Verify Function ID
 * @param[in]  aFunctionString        Verify FUnction String
 * @param[in]  aEnv                   Environment
 * @remarks
 */
stlStatus ellModifyProfileParamVerifyFunction( smlTransId             aTransID,
                                               smlStatement         * aStmt,
                                               ellDictHandle        * aProfileHandle,
                                               stlBool                aIsDefault,
                                               stlInt64               aFunctionID,
                                               stlChar              * aFunctionString,
                                               ellEnv               * aEnv )
{
    stlInt64 sProfileID = ELL_DICT_OBJECT_ID_NA;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aProfileHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aFunctionID <= ELL_DICT_OBJECT_ID_NA) &&
                        (aFunctionID > ELL_PROFILE_VERIFY_MIN),
                        ELL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */

    sProfileID = ellGetProfileID( aProfileHandle );

    /**
     * Profile Parameter 제거
     */

    STL_TRY( eldDeleteProfilePassParam( aTransID,
                                        aStmt,
                                        sProfileID,
                                        ELL_PROFILE_PARAM_PASSWORD_VERIFY_FUNCTION,
                                        aEnv )
             == STL_SUCCESS );
    
    
    /**
     * Profile Parameter 추가
     */
    
    STL_TRY( ellInsertProfileParamVerifyFunction( aTransID,
                                                  aStmt,
                                                  sProfileID,
                                                  aIsDefault,
                                                  aFunctionID,
                                                  aFunctionString,
                                                  aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}



/*********************************************************
 * Cache 재구축 함수
 *********************************************************/


/**
 * @brief SQL-Profile Cache 를 추가한다.
 * @param[in]  aTransID       Transaction ID
 * @param[in]  aStmt          Statement
 * @param[in]  aProfileID     Profile ID
 * @param[in]  aEnv           Envirionment 
 * @remarks
 */
stlStatus ellRefineCache4CreateProfile( smlTransId           aTransID,
                                        smlStatement       * aStmt,
                                        stlInt64             aProfileID,
                                        ellEnv             * aEnv )
{
    STL_TRY( eldcInsertCacheProfileByProfileID( aTransID,
                                                aStmt,
                                                aProfileID,
                                                aEnv )
             == STL_SUCCESS );

    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}



/**
 * @brief SQL-Profile Cache 를 제거한다.
 * @param[in]  aTransID       Transaction ID
 * @param[in]  aProfileHandle Profile Handle
 * @param[in]  aEnv           Envirionment 
 * @remarks
 */
stlStatus ellRefineCache4DropProfile( smlTransId        aTransID,
                                      ellDictHandle   * aProfileHandle,
                                      ellEnv          * aEnv )
{
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aProfileHandle != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * SQL-Profile Cache 정보를 삭제
     */
    
    STL_TRY( eldcDeleteCacheProfile( aTransID,
                                     aProfileHandle,
                                     aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}



/**
 * @brief Profile Cache 재구축 
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aProfileHandle   Profile Handle
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus ellRebuildProfileCache( smlTransId        aTransID,
                                  smlStatement    * aStmt,
                                  ellDictHandle   * aProfileHandle,
                                  ellEnv          * aEnv )
{
    stlInt64  sProfileID = ELL_DICT_OBJECT_ID_NA;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aProfileHandle != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */
    
    sProfileID = ellGetProfileID( aProfileHandle );
    
    /**
     * SQL-Profile Cache 정보를 삭제
     */
    
    STL_TRY( eldcDeleteCacheProfile( aTransID,
                                     aProfileHandle,
                                     aEnv )
             == STL_SUCCESS );

    /**
     * SQL-Profile Cache 정보를 추가 
     */

    STL_TRY( eldcInsertCacheProfileByProfileID( aTransID,
                                                aStmt,
                                                sProfileID,
                                                aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
    
}

/*********************************************************
 * Handle 획득 함수
 *********************************************************/

/**
 * @brief Profile Name 으로 Profile Dictionary Handle 을 얻는다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aViewSCN         View SCN
 * @param[in]  aProfileName     Profile Name
 * @param[out] aProfileHandle   Profile Dictionary Handle
 * @param[out] aExist           존재 여부 
 * @param[in]  aEnv             Envirionment 
 * @remarks
 */
stlStatus ellGetProfileDictHandleByName( smlTransId           aTransID,
                                         smlScn               aViewSCN,
                                         stlChar            * aProfileName,
                                         ellDictHandle      * aProfileHandle,
                                         stlBool            * aExist,
                                         ellEnv             * aEnv )
{
    ellInitDictHandle( aProfileHandle );
    
    STL_TRY( eldcGetProfileHandleByName( aTransID,
                                         aViewSCN,
                                         aProfileName,
                                         aProfileHandle,
                                         aExist,
                                         aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Profile ID 로 Profile Dictionary Handle 을 얻는다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aViewSCN         View SCN
 * @param[in]  aProfileID       Profile ID
 * @param[out] aProfileHandle   Profile Dictionary Handle
 * @param[out] aExist           존재 여부 
 * @param[in]  aEnv             Envirionment 
 * @remarks
 */
stlStatus ellGetProfileDictHandleByID( smlTransId           aTransID,
                                       smlScn               aViewSCN,
                                       stlInt64             aProfileID,
                                       ellDictHandle      * aProfileHandle,
                                       stlBool            * aExist,
                                       ellEnv             * aEnv )
{
    ellInitDictHandle( aProfileHandle );
    
    STL_TRY( eldcGetProfileHandleByID( aTransID,
                                       aViewSCN,
                                       aProfileID,
                                       aProfileHandle,
                                       aExist,
                                       aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

/*********************************************************
 * 정보 획득 함수
 *********************************************************/

/**
 * @brief Profile ID 를 획득
 * @param[in] aProfileHandle  Profile Handle
 */
stlInt64    ellGetProfileID( ellDictHandle * aProfileHandle )
{
    ellProfileDesc * sProfileDesc = NULL;

    STL_DASSERT( aProfileHandle->mObjectType == ELL_OBJECT_PROFILE );
    
    sProfileDesc = (ellProfileDesc *) ellGetObjectDesc( aProfileHandle );

    return sProfileDesc->mProfileID;
}


/**
 * @brief DEFAULT Profile 의 ID 획득
 */
stlInt64 ellGetDefaultProfileID()
{
    return (stlInt64) ELDT_PROFILE_ID_DEFAULT;
}

/**
 * @brief Profile Name 을 획득
 * @param[in] aProfileHandle  Profile Handle
 */
stlChar   * ellGetProfileName( ellDictHandle * aProfileHandle )
{
    ellProfileDesc * sProfileDesc = NULL;

    STL_DASSERT( aProfileHandle->mObjectType == ELL_OBJECT_PROFILE );
    
    sProfileDesc = (ellProfileDesc *) ellGetObjectDesc( aProfileHandle );

    return sProfileDesc->mProfileName;
}


/**
 * @brief Password Parameter 의 DEFAULT 여부 
 * @param[in] aProfileHandle  Profile Handle
 * @param[in] aPassParam      Password Parameter
 */
stlBool ellIsDefaultPassParam( ellDictHandle * aProfileHandle,
                               ellProfileParam aPassParam )
{
    stlBool sIsDefault = STL_FALSE;

    ellProfileDesc * sProfileDesc = NULL;

    STL_DASSERT( aProfileHandle->mObjectType == ELL_OBJECT_PROFILE );
    STL_DASSERT( (aPassParam > ELL_PROFILE_PARAM_NA) &&
                 (aPassParam < ELL_PROFILE_PARAM_MAX) );

    sProfileDesc = (ellProfileDesc *) ellGetObjectDesc( aProfileHandle );

    switch ( aPassParam )
    {
        case ELL_PROFILE_PARAM_FAILED_LOGIN_ATTEMPTS:
            {
                sIsDefault = sProfileDesc->mFailedLoginIsDefault;
                break;
            }
        case ELL_PROFILE_PARAM_PASSWORD_LOCK_TIME:
            {
                sIsDefault = sProfileDesc->mLockTimeIsDefault;
                break;
            }
        case ELL_PROFILE_PARAM_PASSWORD_LIFE_TIME:
            {
                sIsDefault = sProfileDesc->mLifeTimeIsDefault;
                break;
            }
        case ELL_PROFILE_PARAM_PASSWORD_GRACE_TIME:
            {
                sIsDefault = sProfileDesc->mGraceTimeIsDefault;
                break;
            }
        case ELL_PROFILE_PARAM_PASSWORD_REUSE_MAX:
            {
                sIsDefault = sProfileDesc->mReuseMaxIsDefault;
                break;
            }
        case ELL_PROFILE_PARAM_PASSWORD_REUSE_TIME:
            {
                sIsDefault = sProfileDesc->mReuseTimeIsDefault;
                break;
            }
        case ELL_PROFILE_PARAM_PASSWORD_VERIFY_FUNCTION:
            {
                sIsDefault = sProfileDesc->mVerifyFunctionIsDefault;
                break;
            }
        default:
            {
                STL_DASSERT(0);
                break;
            }
    }

    return sIsDefault;
}


/**
 * @brief Integer 계열의 Password Parameter 값 획득
 * @param[in] aProfileHandle  Profile Handle
 * @param[in] aPassParam      Password Parameter
 * @return
 * ELL_PROFILE_LIMIT_UNLIMITED(0) or
 * stlInt64 value
 */
stlInt64    ellGetProfileParamInteger( ellDictHandle * aProfileHandle,
                                       ellProfileParam aPassParam )
{
    stlInt64 sInt64Value = 0;

    ellProfileDesc * sProfileDesc = NULL;

    STL_DASSERT( aProfileHandle->mObjectType == ELL_OBJECT_PROFILE );
    STL_DASSERT( (aPassParam > ELL_PROFILE_PARAM_NA) &&
                 (aPassParam < ELL_PROFILE_PARAM_MAX) );

    sProfileDesc = (ellProfileDesc *) ellGetObjectDesc( aProfileHandle );

    switch ( aPassParam )
    {
        case ELL_PROFILE_PARAM_FAILED_LOGIN_ATTEMPTS:
            {
                STL_DASSERT( sProfileDesc->mFailedLoginIsDefault == STL_FALSE );
                sInt64Value = sProfileDesc->mFailedLoginAttempt;
                break;
            }
        case ELL_PROFILE_PARAM_PASSWORD_LOCK_TIME:
            {
                STL_DASSERT(0);
                break;
            }
        case ELL_PROFILE_PARAM_PASSWORD_LIFE_TIME:
            {
                STL_DASSERT(0);
                break;
            }
        case ELL_PROFILE_PARAM_PASSWORD_GRACE_TIME:
            {
                STL_DASSERT(0);
                break;
            }
        case ELL_PROFILE_PARAM_PASSWORD_REUSE_MAX:
            {
                STL_DASSERT( sProfileDesc->mReuseMaxIsDefault == STL_FALSE );
                sInt64Value = sProfileDesc->mReuseMax;
                break;
            }
        case ELL_PROFILE_PARAM_PASSWORD_REUSE_TIME:
            {
                STL_DASSERT(0);
                break;
            }
        case ELL_PROFILE_PARAM_PASSWORD_VERIFY_FUNCTION:
            {
                STL_DASSERT(0);
                break;
            }
        default:
            {
                STL_DASSERT(0);
                break;
            }
    }

    return sInt64Value;
}
   

/**
 * @brief Number Interval 계열의 Password Parameter 값 획득
 * @param[in] aProfileHandle  Profile Handle
 * @param[in] aPassParam      Password Parameter
 * @return
 * number value( null value is UNLIMITED )
 */
dtlDataValue * ellGetProfileParamNumberInterval( ellDictHandle * aProfileHandle,
                                                 ellProfileParam aPassParam )
{
    dtlDataValue * sNumberValue = NULL;

    ellProfileDesc * sProfileDesc = NULL;

    STL_DASSERT( aProfileHandle->mObjectType == ELL_OBJECT_PROFILE );
    STL_DASSERT( (aPassParam > ELL_PROFILE_PARAM_NA) &&
                 (aPassParam < ELL_PROFILE_PARAM_MAX) );

    sProfileDesc = (ellProfileDesc *) ellGetObjectDesc( aProfileHandle );

    switch ( aPassParam )
    {
        case ELL_PROFILE_PARAM_FAILED_LOGIN_ATTEMPTS:
            {
                STL_DASSERT(0);
                break;
            }
        case ELL_PROFILE_PARAM_PASSWORD_LOCK_TIME:
            {
                STL_DASSERT( sProfileDesc->mLockTimeIsDefault == STL_FALSE );
                sNumberValue = & sProfileDesc->mLockTimeNumber;
                break;
            }
        case ELL_PROFILE_PARAM_PASSWORD_LIFE_TIME:
            {
                STL_DASSERT( sProfileDesc->mLifeTimeIsDefault == STL_FALSE );
                sNumberValue = & sProfileDesc->mLifeTimeNumber;
                break;
            }
        case ELL_PROFILE_PARAM_PASSWORD_GRACE_TIME:
            {
                STL_DASSERT( sProfileDesc->mGraceTimeIsDefault == STL_FALSE );
                sNumberValue = & sProfileDesc->mGraceTimeNumber;
                break;
            }
        case ELL_PROFILE_PARAM_PASSWORD_REUSE_MAX:
            {
                STL_DASSERT(0);
                break;
            }
        case ELL_PROFILE_PARAM_PASSWORD_REUSE_TIME:
            {
                STL_DASSERT( sProfileDesc->mReuseTimeIsDefault == STL_FALSE );
                sNumberValue = & sProfileDesc->mReuseTimeNumber;
                break;
            }
        case ELL_PROFILE_PARAM_PASSWORD_VERIFY_FUNCTION:
            {
                STL_DASSERT(0);
                break;
            }
        default:
            {
                STL_DASSERT(0);
                break;
            }
    }

    return sNumberValue;
}

/**
 * @brief Password Verify Function ID 획득
 * @param[in] aProfileHandle  Profile Handle
 * @return
 * ELL_OBJECT_ID_NA or
 * function ID
 */
stlInt64    ellGetProfileParamVerifyFunction( ellDictHandle * aProfileHandle )
{
    ellProfileDesc * sProfileDesc = NULL;

    STL_DASSERT( aProfileHandle->mObjectType == ELL_OBJECT_PROFILE );

    sProfileDesc = (ellProfileDesc *) ellGetObjectDesc( aProfileHandle );

    STL_DASSERT( sProfileDesc->mVerifyFunctionIsDefault == STL_FALSE );
    
    return sProfileDesc->mVerifyFunctionID;
}


/**
 * @brief Profile Handle 로부터 Default Profile 과 조합하여 Profile Parameter 정보를 획득
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aProfileHandle   User 의 Profile Handle (nullable)
 * @param[out] aProfileParam    User Profile Paramter
 * @param[in]  aEnv             Environment
 * @remarks
 * user profile handle + default profile handle
 */
stlStatus ellGetUserProfileParam( smlTransId            aTransID,
                                  smlStatement        * aStmt,
                                  ellDictHandle       * aProfileHandle,
                                  ellUserProfileParam * aProfileParam,
                                  ellEnv              * aEnv )
{
    ellDictHandle sDefaultProfileHandle;
    stlBool       sObjectExist = STL_FALSE;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aProfileParam != NULL, ELL_ERROR_STACK(aEnv) );
    
    if ( aProfileHandle == NULL )
    {
        aProfileParam->mFailedLogin  = ELL_PROFILE_LIMIT_UNLIMITED;
        aProfileParam->mLockTime     = ellGetNullNumber();
        aProfileParam->mLifeTime     = ellGetNullNumber();
        aProfileParam->mGraceTime    = ellGetNullNumber();
        aProfileParam->mReuseMax     = ELL_PROFILE_LIMIT_UNLIMITED;
        aProfileParam->mReuseTime    = ellGetNullNumber();
        aProfileParam->mVerifyFuncID = ELL_DICT_OBJECT_ID_NA;
    }
    else
    {
        STL_TRY( ellGetProfileDictHandleByID( aTransID,
                                              aStmt->mScn,
                                              ellGetDefaultProfileID(),
                                              & sDefaultProfileHandle,
                                              & sObjectExist,
                                              aEnv )
                 == STL_SUCCESS );
        STL_DASSERT( sObjectExist == STL_TRUE );

        /**
         * FAILED_LOGIN_ATTEMPTS 정보 획득
         */
        
        if ( ellIsDefaultPassParam( aProfileHandle,
                                    ELL_PROFILE_PARAM_FAILED_LOGIN_ATTEMPTS )
             == STL_TRUE )
        {
            aProfileParam->mFailedLogin = ellGetProfileParamInteger( & sDefaultProfileHandle,
                                                                     ELL_PROFILE_PARAM_FAILED_LOGIN_ATTEMPTS );
        }
        else
        {
            aProfileParam->mFailedLogin = ellGetProfileParamInteger( aProfileHandle,
                                                                     ELL_PROFILE_PARAM_FAILED_LOGIN_ATTEMPTS );
        }
                
        /**
         * PASSWORD_LOCK_TIME 획득
         */
        
        if ( ellIsDefaultPassParam( aProfileHandle,
                                    ELL_PROFILE_PARAM_PASSWORD_LOCK_TIME )
             == STL_TRUE )
        {
            aProfileParam->mLockTime = ellGetProfileParamNumberInterval( & sDefaultProfileHandle,
                                                                         ELL_PROFILE_PARAM_PASSWORD_LOCK_TIME );
        }
        else
        {
            aProfileParam->mLockTime = ellGetProfileParamNumberInterval( aProfileHandle,
                                                                         ELL_PROFILE_PARAM_PASSWORD_LOCK_TIME );
        }

        /**
         * PASSWORD_LIFE_TIME 획득
         */
        
        if ( ellIsDefaultPassParam( aProfileHandle,
                                    ELL_PROFILE_PARAM_PASSWORD_LIFE_TIME )
             == STL_TRUE )
        {
            aProfileParam->mLifeTime = ellGetProfileParamNumberInterval( & sDefaultProfileHandle,
                                                                         ELL_PROFILE_PARAM_PASSWORD_LIFE_TIME );
        }
        else
        {
            aProfileParam->mLifeTime = ellGetProfileParamNumberInterval( aProfileHandle,
                                                                         ELL_PROFILE_PARAM_PASSWORD_LIFE_TIME );
        }

        /**
         * PASSWORD_GRACE_TIME 획득
         */
        
        if ( ellIsDefaultPassParam( aProfileHandle,
                                    ELL_PROFILE_PARAM_PASSWORD_GRACE_TIME )
             == STL_TRUE )
        {
            aProfileParam->mGraceTime = ellGetProfileParamNumberInterval( & sDefaultProfileHandle,
                                                                          ELL_PROFILE_PARAM_PASSWORD_GRACE_TIME );
        }
        else
        {
            aProfileParam->mGraceTime = ellGetProfileParamNumberInterval( aProfileHandle,
                                                                          ELL_PROFILE_PARAM_PASSWORD_GRACE_TIME );
        }

        /**
         * PASSWORD_REUSE_MAX 획득
         */
        
        if ( ellIsDefaultPassParam( aProfileHandle,
                                    ELL_PROFILE_PARAM_PASSWORD_REUSE_MAX )
             == STL_TRUE )
        {
            aProfileParam->mReuseMax = ellGetProfileParamInteger( & sDefaultProfileHandle,
                                                                  ELL_PROFILE_PARAM_PASSWORD_REUSE_MAX );
        }
        else
        {
            aProfileParam->mReuseMax = ellGetProfileParamInteger( aProfileHandle,
                                                                  ELL_PROFILE_PARAM_PASSWORD_REUSE_MAX );
        }

        /**
         * PASSWORD_REUSE_TIME 획득
         */
        
        if ( ellIsDefaultPassParam( aProfileHandle,
                                    ELL_PROFILE_PARAM_PASSWORD_REUSE_TIME )
             == STL_TRUE )
        {
            aProfileParam->mReuseTime = ellGetProfileParamNumberInterval( & sDefaultProfileHandle,
                                                                          ELL_PROFILE_PARAM_PASSWORD_REUSE_TIME );
        }
        else
        {
            aProfileParam->mReuseTime = ellGetProfileParamNumberInterval( aProfileHandle,
                                                                          ELL_PROFILE_PARAM_PASSWORD_REUSE_TIME );
        }
        
        /**
         * PASSWORD_REUSE_MAX 획득
         */
        
        if ( ellIsDefaultPassParam( aProfileHandle,
                                    ELL_PROFILE_PARAM_PASSWORD_REUSE_MAX )
             == STL_TRUE )
        {
            aProfileParam->mReuseMax = ellGetProfileParamInteger( & sDefaultProfileHandle,
                                                                  ELL_PROFILE_PARAM_PASSWORD_REUSE_MAX );
        }
        else
        {
            aProfileParam->mReuseMax = ellGetProfileParamInteger( aProfileHandle,
                                                                  ELL_PROFILE_PARAM_PASSWORD_REUSE_MAX );
        }
        
        /**
         * PASSWORD_VERIFY_FUNCTION 획득
         */
        
        if ( ellIsDefaultPassParam( aProfileHandle,
                                    ELL_PROFILE_PARAM_PASSWORD_VERIFY_FUNCTION )
             == STL_TRUE )
        {
            aProfileParam->mVerifyFuncID = ellGetProfileParamVerifyFunction( & sDefaultProfileHandle );
        }
        else
        {
            aProfileParam->mVerifyFuncID = ellGetProfileParamVerifyFunction( aProfileHandle );
        }
    }

    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/** @} ellObjectProfile */
