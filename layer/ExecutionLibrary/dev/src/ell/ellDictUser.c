/*******************************************************************************
 * ellDictUser.c
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
 * @file ellDictUser.c
 * @brief User Dictionary 관리 모듈
 */

#include <ell.h>
#include <elDef.h>

#include <eldt.h>
#include <eldc.h>
#include <eldDictionary.h>

#include <eldUser.h>
#include <eldPasswordHistory.h>


/**
 * @addtogroup ellObjectAuth
 * @{
 */


const stlChar * const gEllUserLockedString[ELL_USER_LOCKED_STATUS_MAX] =
{
    "N/A",           /* ELL_USER_LOCKED_STATUS_NA */
    
    "OPEN",          /* ELL_USER_LOCKED_STATUS_OPEN */
    "LOCKED",        /* ELL_USER_LOCKED_STATUS_LOCKED */
    "LOCKED(TIMED)", /* ELL_USER_LOCKED_STATUS_LOCKED_TIMED */
};


const stlChar * const gEllUserExpiryString[ELL_USER_EXPIRY_STATUS_MAX] =
{
    "N/A",            /* ELL_USER_EXPIRY_STATUS_NA */
    
    "OPEN",           /* ELL_USER_EXPIRY_STATUS_OPEN */
    "EXPIRED",        /* ELL_USER_EXPIRY_STATUS_EXPIRED */
    "EXPIRED(GRACE)", /* ELL_USER_EXPIRY_STATUS_EXPIRED_GRACE */
};





stlStatus ellClearUserPasswordHistory( smlTransId       aTransID,
                                       smlStatement   * aStmt,
                                       ellEnv         * aEnv )
{
    /**
     * USER_PASSWORD_HISTORY 제거
     */

    STL_TRY( eldClearUserPasswordHistory( aTransID,
                                          aStmt,
                                          aEnv )
             == STL_SUCCESS );
             
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}

/**
 * @brief User Descriptor 를 Dictionary 에 추가한다.
 * @param[in]  aTransID            Transaction ID
 * @param[in]  aStmt               Statement
 * @param[in]  aAuthID             User 의 Authorization ID
 * @param[in]  aProfileID          User 의 Profile ID ( or ELL_DICT_OBJECT_ID_NA )
 * @param[in]  aEncryptedPassword  User 의 암호화된 password
 * @param[in]  aDataTablespaceID   User 의 default data tablespace ID
 * @param[in]  aTempTablespaceID   User 의 default temporary tablespace ID
 * @param[in]  aLockedStatus       Account Locked Status
 * @param[in]  aLockedTime         Account Locked Time ( or ELL_DICT_STLTIME_NA )
 * @param[in]  aExpiryStatus       Password Expiry Status
 * @param[in]  aExpiryTime         Password Expiry Time ( or ELL_DICT_STLTIME_NA )
 * @param[in]  aEnv                Execution Library Environment
 * @remarks
 */
stlStatus ellInsertUserDesc( smlTransId             aTransID,
                             smlStatement         * aStmt,
                             stlInt64               aAuthID,
                             stlInt64               aProfileID,
                             stlChar              * aEncryptedPassword,
                             stlInt64               aDataTablespaceID,
                             stlInt64               aTempTablespaceID,
                             ellUserLockedStatus    aLockedStatus,
                             stlTime                aLockedTime,
                             ellUserExpiryStatus    aExpiryStatus,
                             stlTime                aExpiryTime,
                             ellEnv               * aEnv )
{
    STL_TRY( eldInsertUserDesc( aTransID,
                                aStmt,
                                aAuthID,
                                aProfileID,
                                aEncryptedPassword,
                                aDataTablespaceID,
                                aTempTablespaceID,
                                aLockedStatus,
                                aLockedTime,
                                aExpiryStatus,
                                aExpiryTime,
                                aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}



/**
 * @brief User 의 Profile 을  변경한다.
 * @param[in]  aTransID          Transaction ID
 * @param[in]  aStmt             Statement
 * @param[in]  aUserHandle       User Handle
 * @param[in]  aProfileID        Profile ID (if null, ELL_DICT_OBJECT_ID_NA)
 * @param[in]  aAuthenInfo       User Authentication 
 * @param[in]  aEnv              Environment
 * @remarks
 */
stlStatus ellModifyUserProfile( smlTransId      aTransID,
                                smlStatement  * aStmt,
                                ellDictHandle * aUserHandle,
                                stlInt64        aProfileID,
                                ellAuthenInfo * aAuthenInfo,
                                ellEnv        * aEnv )
{
    knlValueBlockList * sRowValueList = NULL;

    eldMemMark          sMemMark;
    stlBool             sMemAlloc = STL_FALSE;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aUserHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aProfileID >= ELL_DICT_OBJECT_ID_NA, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAuthenInfo != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * 갱신을 위한 Record 삭제
     */

    STL_TRY( eldDeleteDictDesc4Modify( aTransID,
                                       aStmt,
                                       ELDT_TABLE_ID_USERS,
                                       ELDT_User_ColumnOrder_AUTH_ID,
                                       ellGetAuthID(aUserHandle),
                                       & sMemMark,
                                       & sRowValueList,
                                       aEnv )
             == STL_SUCCESS );
    sMemAlloc = STL_TRUE;

    /**
     * User 의 Authentication 정보 갱신
     */
    
    STL_TRY( eldSetAuthenInfo( aAuthenInfo, sRowValueList, aEnv ) == STL_SUCCESS );

    /**
     * PROFILE_ID
     */

    if ( aProfileID == ELL_DICT_OBJECT_ID_NA )
    {
        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_USERS,
                                    ELDT_User_ColumnOrder_PROFILE_ID,
                                    sRowValueList,
                                    NULL,
                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_USERS,
                                    ELDT_User_ColumnOrder_PROFILE_ID,
                                    sRowValueList,
                                    & aProfileID,
                                    aEnv )
                 == STL_SUCCESS );
    }
    
    /**
     * 갱신된 Row 추가 
     */

    sMemAlloc = STL_FALSE;
    STL_TRY( eldInsertDictDesc4Modify( aTransID,
                                       aStmt,
                                       ELDT_TABLE_ID_USERS,
                                       & sMemMark,
                                       sRowValueList,
                                       aEnv )
             == STL_SUCCESS );

    /**
     * 마무리 
     */
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    if ( sMemAlloc == STL_TRUE )
    {
        (void) eldFreeTableValueList( & sMemMark, aEnv );
    }
    
    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}




/**
 * @brief Login 을 통해 authentication 정보 변경
 * @param[in]  aTransID            Transaction ID
 * @param[in]  aStmt               Statement
 * @param[in]  aUserHandle         User Handle
 * @param[in]  aAuthenInfo         User Authentication 정보
 * @param[out] aVersionConflict    Version Conflict 여부 
 * @param[in]  aEnv                Execution Library Environment
 * @remarks
 * version conflict 시 login authentication 을 다시 수행해야 함.
 */
stlStatus ellModifyUserAuthentication4Login( smlTransId      aTransID,
                                             smlStatement  * aStmt,
                                             ellDictHandle * aUserHandle,
                                             ellAuthenInfo * aAuthenInfo,
                                             stlBool       * aVersionConflict,
                                             ellEnv        * aEnv )
{
    knlValueBlockList * sRowValueList = NULL;

    eldMemMark          sMemMark;
    stlBool             sMemAlloc = STL_FALSE;
    stlBool             sHasConflict = STL_FALSE;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aUserHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAuthenInfo != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aVersionConflict != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * 갱신을 위한 Record 삭제
     */

    STL_TRY( eldDeleteAuthen4LoginModify( aTransID,
                                          aStmt,
                                          aUserHandle,
                                          & sMemMark,
                                          & sRowValueList,
                                          & sHasConflict,
                                          aEnv )
             == STL_SUCCESS );
    sMemAlloc = STL_TRUE;

    if ( sHasConflict == STL_TRUE )
    {
        STL_THROW( RAMP_FINISH );
    }

    /**
     * User 의 Authentication 정보 갱신
     */
    
    STL_TRY( eldSetAuthenInfo( aAuthenInfo, sRowValueList, aEnv ) == STL_SUCCESS );
    
    /**
     * 갱신된 Row 추가 
     */

    sMemAlloc = STL_FALSE;
    STL_TRY( eldInsertDictDesc4Modify( aTransID,
                                       aStmt,
                                       ELDT_TABLE_ID_USERS,
                                       & sMemMark,
                                       sRowValueList,
                                       aEnv )
             == STL_SUCCESS );

    /**
     * 마무리 
     */
    
    STL_RAMP( RAMP_FINISH );

    if ( sMemAlloc == STL_TRUE )
    {
        sMemAlloc = STL_FALSE;
        STL_TRY( eldFreeTableValueList( & sMemMark, aEnv ) == STL_SUCCESS );
    }

    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    /**
     * output 설정
     */

    *aVersionConflict = sHasConflict;
    
    return STL_SUCCESS;

    STL_FINISH;

    if ( sMemAlloc == STL_TRUE )
    {
        (void) eldFreeTableValueList( & sMemMark, aEnv );
    }
    
    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/**
 * @brief DDL 을 통해 authentication 정보 변경
 * @param[in]  aTransID            Transaction ID
 * @param[in]  aStmt               Statement
 * @param[in]  aUserHandle         User Handle
 * @param[in]  aAuthenInfo         User Authentication 정보
 * @param[in]  aEnv                Execution Library Environment
 * @remarks
 * version conflict 를 내부에서 제어함
 */
stlStatus ellModifyUserAuthentication4DDL( smlTransId      aTransID,
                                           smlStatement  * aStmt,
                                           ellDictHandle * aUserHandle,
                                           ellAuthenInfo * aAuthenInfo,
                                           ellEnv        * aEnv )
{
    knlValueBlockList * sRowValueList = NULL;

    eldMemMark          sMemMark;
    stlBool             sMemAlloc = STL_FALSE;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aUserHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAuthenInfo != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * 갱신을 위한 Record 삭제
     */

    STL_TRY( eldDeleteDictDesc4Modify( aTransID,
                                       aStmt,
                                       ELDT_TABLE_ID_USERS,
                                       ELDT_User_ColumnOrder_AUTH_ID,
                                       ellGetAuthID(aUserHandle),
                                       & sMemMark,
                                       & sRowValueList,
                                       aEnv )
             == STL_SUCCESS );
    sMemAlloc = STL_TRUE;
    
    /**
     * User 의 Authentication 정보 갱신
     */
    
    STL_TRY( eldSetAuthenInfo( aAuthenInfo, sRowValueList, aEnv ) == STL_SUCCESS );
    
    /**
     * 갱신된 Row 추가 
     */

    sMemAlloc = STL_FALSE;
    STL_TRY( eldInsertDictDesc4Modify( aTransID,
                                       aStmt,
                                       ELDT_TABLE_ID_USERS,
                                       & sMemMark,
                                       sRowValueList,
                                       aEnv )
             == STL_SUCCESS );

    /**
     * 마무리 
     */
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    if ( sMemAlloc == STL_TRUE )
    {
        (void) eldFreeTableValueList( & sMemMark, aEnv );
    }
    
    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}



/**
 * @brief User 의 Authentication Information 을 획득한다.
 * @param[in]  aTransID            Transaction ID
 * @param[in]  aStmt               Statement
 * @param[in]  aUserHandle         User Dictionary Handle
 * @param[out] aAuthenInfo         Authentication Information
 * @param[in]  aEnv                Environment
 * @remarks
 */ 
stlStatus ellGetUserAuthenticationInfo( smlTransId           aTransID,
                                        smlStatement       * aStmt,
                                        ellDictHandle      * aUserHandle,
                                        ellAuthenInfo      * aAuthenInfo,
                                        ellEnv             * aEnv )
{
    STL_TRY( eldGetUserAuthenticationInfo( aTransID,
                                           aStmt,
                                           aUserHandle,
                                           aAuthenInfo,
                                           aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}



/**
 * @brief User Password History Descriptor 를 Dictionary 에 추가한다.
 * @param[in] aTransID             Transaction ID
 * @param[in] aStmt                Statement
 * @param[in] aAuthID              User ID
 * @param[in] aEncryptedPassword   Encrypted Password
 * @param[in] aPassChangeNO        Password Change Number
 * @param[in] aPassChangeTime      Password Change Time
 * @param[in] aEnv                 Environment
 * @remarks
 */
stlStatus ellInsertPasswordHistoryDesc( smlTransId             aTransID,
                                        smlStatement         * aStmt,
                                        stlInt64               aAuthID,
                                        stlChar              * aEncryptedPassword,
                                        stlInt64               aPassChangeNO,
                                        stlTime                aPassChangeTime,
                                        ellEnv               * aEnv )
{
    STL_TRY( eldInsertPasswordHistoryDesc( aTransID,
                                           aStmt,
                                           aAuthID,
                                           aEncryptedPassword,
                                           aPassChangeNO,
                                           aPassChangeTime,
                                           aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/**
 * @brief User Password History 에서 REUSE 불가능한 Password 가 존재하는 지 검사 
 * @param[in] aTransID             Transaction ID
 * @param[in] aStmt                Statement
 * @param[in] aUserHandle          User Handle
 * @param[in] aEncryptedPassword   Encrypted Password
 * @param[in] aMinChangeNO         Min Change Number
 * @param[in] aMinChangeTime       Min Change Time
 * @param[out] aExist              존재 여부 
 * @param[in] aEnv                 Environment
 * @remarks
 * password 가 동일하고 MinChangeNO 보다 크거나 같고 MinChangeTime 보다 크거나 같은
 * old password 가 있는지 검사
 */
stlStatus ellFindDisableReusePassword( smlTransId             aTransID,
                                       smlStatement         * aStmt,
                                       ellDictHandle        * aUserHandle,
                                       stlChar              * aEncryptedPassword,
                                       stlInt64               aMinChangeNO,
                                       stlTime                aMinChangeTime,
                                       stlBool              * aExist,
                                       ellEnv               * aEnv )
{
    STL_TRY( eldFindDisableReusePassword( aTransID,
                                          aStmt,
                                          aUserHandle,
                                          aEncryptedPassword,
                                          aMinChangeNO,
                                          aMinChangeTime,
                                          aExist,
                                          aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/**
 * @brief User Password History 에서 제거 가능한 old password 를 제거한다.
 * @param[in] aTransID             Transaction ID
 * @param[in] aStmt                Statement
 * @param[in] aUserHandle          User Handle
 * @param[in] aMinChangeNO         Min Change Number
 * @param[in] aMinChangeTime       Min Change Time
 * @param[in] aEnv                 Environment
 * @remarks
 * MinChangeNO 보다 자고 MinChangeTime 보다 작은 old password 를 제거한다.
 */
stlStatus ellRemoveOldPassword( smlTransId             aTransID,
                                smlStatement         * aStmt,
                                ellDictHandle        * aUserHandle,
                                stlInt64               aMinChangeNO,
                                stlTime                aMinChangeTime,
                                ellEnv               * aEnv )
{
    STL_TRY( eldRemoveOldPassword( aTransID,
                                   aStmt,
                                   aUserHandle,
                                   aMinChangeNO,
                                   aMinChangeTime,
                                   aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}

/**
 * @brief User 의 Profile ID 를 획득한다.
 * @param[in] aUserHandle User Handle
 */
stlInt64    ellGetUserProfileID( ellDictHandle * aUserHandle )
{
    ellAuthDesc * sUserDesc = NULL;

    STL_DASSERT( aUserHandle->mObjectType == ELL_OBJECT_AUTHORIZATION );
    
    sUserDesc = (ellAuthDesc *) ellGetObjectDesc( aUserHandle );
    STL_DASSERT( sUserDesc->mAuthType == ELL_AUTHORIZATION_TYPE_USER );

    return sUserDesc->mProfileID;
    
}

/** @} ellObjectAuth */
