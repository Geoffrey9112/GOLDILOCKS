/*******************************************************************************
 * eldUser.c
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
 * @file eldUser.c
 * @brief User Dictionary 관리 루틴 
 */

#include <ell.h>
#include <eldt.h>
#include <eldc.h>

#include <eldIntegrity.h>
#include <eldDictionary.h>
#include <eldUser.h>

/**
 * @addtogroup eldUser
 * @{
 */

/**
 * @brief User Descriptor 를 Dictionary 에 추가한다.
 * @remarks
 * 함수 ellInsertUserDesc() 주석 참조 
 */
stlStatus eldInsertUserDesc( smlTransId             aTransID,
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
    knlValueBlockList        * sDataValueList  = NULL;

    smlRid               sRowRid;
    smlScn               sRowScn;
    smlRowBlock          sRowBlock;
    eldMemMark           sMemMark;

    stlInt64    sInt64Value = 0;

    stlInt32  sState = 0;
    
    /*
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAuthID > ELL_DICT_OBJECT_ID_NA, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aProfileID >= ELL_DICT_OBJECT_ID_NA, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aEncryptedPassword > 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aLockedStatus > ELL_USER_LOCKED_STATUS_NA) &&
                        (aLockedStatus < ELL_USER_LOCKED_STATUS_MAX)
                        , ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aExpiryStatus > ELL_USER_EXPIRY_STATUS_NA) &&
                        (aExpiryStatus < ELL_USER_EXPIRY_STATUS_MAX)
                        , ELL_ERROR_STACK(aEnv) );
    

    
    /**
     * - Data Value List와 Row Block을 위한 공간 할당 및 초기화
     */
    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_USERS,
                                     & sMemMark,
                                     & sDataValueList,
                                     aEnv )
             == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( smlInitSingleRowBlock( &sRowBlock,
                                    &sRowRid,
                                    &sRowScn,
                                    SML_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * - user descriptor의 정보를 구성한다.
     */

    /*
     * AUTH_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_USERS,
                                ELDT_User_ColumnOrder_AUTH_ID,
                                sDataValueList,
                                & aAuthID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * PROFILE_ID
     */

    if ( aProfileID == ELL_DICT_OBJECT_ID_NA )
    {
        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_USERS,
                                    ELDT_User_ColumnOrder_PROFILE_ID,
                                    sDataValueList,
                                    NULL,
                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_USERS,
                                    ELDT_User_ColumnOrder_PROFILE_ID,
                                    sDataValueList,
                                    & aProfileID,
                                    aEnv )
                 == STL_SUCCESS );
    }
    
    /*
     * ENCRYPTED_PASSWORD
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_USERS,
                                ELDT_User_ColumnOrder_ENCRYPTED_PASSWORD,
                                sDataValueList,
                                aEncryptedPassword,
                                aEnv )
             == STL_SUCCESS );
    
    /*
     * DEFAULT_DATA_TABLESPACE_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_USERS,
                                ELDT_User_ColumnOrder_DEFAULT_DATA_TABLESPACE_ID,
                                sDataValueList,
                                & aDataTablespaceID,
                                aEnv )
             == STL_SUCCESS );
    
    /*
     * DEFAULT_TEMP_TABLESPACE_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_USERS,
                                ELDT_User_ColumnOrder_DEFAULT_TEMP_TABLESPACE_ID,
                                sDataValueList,
                                & aTempTablespaceID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * LOCKED_STATUS
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_USERS,
                                ELDT_User_ColumnOrder_LOCKED_STATUS,
                                sDataValueList,
                                (void *) gEllUserLockedString[aLockedStatus],
                                aEnv )
             == STL_SUCCESS );
    
    /*
     * LOCKED_STATUS_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_USERS,
                                ELDT_User_ColumnOrder_LOCKED_STATUS_ID,
                                sDataValueList,
                                & aLockedStatus,
                                aEnv )
             == STL_SUCCESS );
    
    /*
     * LOCKED_TIME
     */

    if ( aLockedTime == ELL_DICT_STLTIME_NA )
    {
        STL_DASSERT( aLockedStatus == ELL_USER_LOCKED_STATUS_OPEN );

        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_USERS,
                                    ELDT_User_ColumnOrder_LOCKED_TIME,
                                    sDataValueList,
                                    NULL,
                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_DASSERT( aLockedStatus == ELL_USER_LOCKED_STATUS_LOCKED );

        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_USERS,
                                    ELDT_User_ColumnOrder_LOCKED_TIME,
                                    sDataValueList,
                                    & aLockedTime,
                                    aEnv )
                 == STL_SUCCESS );
    }
    
    /*
     * FAILED_LOGIN_ATTEMPTS
     */

    sInt64Value = 0;
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_USERS,
                                ELDT_User_ColumnOrder_FAILED_LOGIN_ATTEMPTS,
                                sDataValueList,
                                & sInt64Value,
                                aEnv )
             == STL_SUCCESS );
    
    /*
     * EXPIRY_STATUS
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_USERS,
                                ELDT_User_ColumnOrder_EXPIRY_STATUS,
                                sDataValueList,
                                (void *) gEllUserExpiryString[aExpiryStatus],
                                aEnv )
             == STL_SUCCESS );
    
    /*
     * EXPIRY_STATUS_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_USERS,
                                ELDT_User_ColumnOrder_EXPIRY_STATUS_ID,
                                sDataValueList,
                                & aExpiryStatus,
                                aEnv )
             == STL_SUCCESS );
    
    /*
     * EXPIRY_TIME
     */

    if ( aExpiryTime == ELL_DICT_STLTIME_NA )
    {
        STL_DASSERT( aExpiryStatus == ELL_USER_EXPIRY_STATUS_OPEN );

        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_USERS,
                                    ELDT_User_ColumnOrder_EXPIRY_TIME,
                                    sDataValueList,
                                    NULL,
                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        /* PASSWORD_LIFE_TIME 이 존재하는 경우 or PASSWORD EXPIRE 를 명시한 경우 */
        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_USERS,
                                    ELDT_User_ColumnOrder_EXPIRY_TIME,
                                    sDataValueList,
                                    & aExpiryTime,
                                    aEnv )
                 == STL_SUCCESS );
    }
    
    /*
     * PASSWORD_CHANGE_COUNT
     */

    sInt64Value = 0;
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_USERS,
                                ELDT_User_ColumnOrder_PASSWORD_CHANGE_COUNT,
                                sDataValueList,
                                & sInt64Value,
                                aEnv )
             == STL_SUCCESS );
    
    
    /**
     * - 레코드를 넣는다.
     */

    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sDataValueList, 0, 1 );
    
    STL_TRY( smlInsertRecord( aStmt,
                              gEldTablePhysicalHandle[ELDT_TABLE_ID_USERS],
                              sDataValueList,
                              NULL, /* unique violation */
                              & sRowBlock,
                              SML_ENV( aEnv ) )
             == STL_SUCCESS );
                              
    /**
     * Dictionary 무결성 유지 
     */

    if ( gEllIsMetaBuilding == STL_TRUE )
    {
        /**
         * 모두 구축후 무결성 검사함.
         */
    }
    else
    {
        STL_TRY( eldRefineIntegrityRowInsert( aTransID,
                                              aStmt,
                                              ELDT_TABLE_ID_USERS,
                                              & sRowBlock,
                                              sDataValueList,
                                              aEnv )
                 == STL_SUCCESS );
    }
    
    /**
     * 사용한 메모리를 해제한다.
     */

    sState = 0;
    STL_TRY( eldFreeTableValueList( & sMemMark, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    switch (sState)
    {
        case 1:
            (void) eldFreeTableValueList( & sMemMark, aEnv );
        default:
            break;
    }
    
    return STL_FAILURE;
}




/**
 * @brief Login 과정에서 Authentication 정보를 변경하기 위해 DELETE 한다.
 * @param[in]  aTransID           Transaction ID
 * @param[in]  aStmt              Statement
 * @param[in]  aUserHandle        User Handle
 * @param[out] aMemMark           Memory Marker
 * @param[out] aDeletedValueList  삭제된 Row 의 Value List
 * @param[out] aIsConflict        Conflict 발생 여부 
 * @param[in]  aEnv               Environment
 * @remarks
 */
stlStatus eldDeleteAuthen4LoginModify( smlTransId               aTransID,
                                       smlStatement           * aStmt,
                                       ellDictHandle          * aUserHandle,
                                       eldMemMark             * aMemMark,
                                       knlValueBlockList     ** aDeletedValueList,
                                       stlBool                * aIsConflict,
                                       ellEnv                 * aEnv )
{
    stlBool             sBeginSelect = STL_FALSE;

    void              * sTableHandle = NULL;
    knlValueBlockList * sTableValueList = NULL;

    knlValueBlockList * sFilterColumn = NULL;
    
    void              * sIndexHandle = NULL;
    knlValueBlockList * sIndexValueList = NULL;
    knlKeyCompareList * sKeyCompList = NULL;
    
    
    void              * sIterator  = NULL;
    smlIteratorProperty sIteratorProperty;

    knlPredicateList    * sRangePred = NULL;
    knlExprContextInfo  * sRangeContext = NULL;

    smlRowBlock         sRowBlock;
    smlRid              sRowRid;
    smlScn              sRowScn;
    
    smlFetchInfo        sFetchInfo;
    smlFetchRecordInfo  sFetchRecordInfo;

    stlBool  sVersionConflict = STL_FALSE;
    stlBool  sSkipped = STL_FALSE;
    stlBool  sHasConflict = STL_FALSE;
    
    stlInt64   sUserID = ELL_DICT_OBJECT_ID_NA;

    stlInt32 sState = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aUserHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aMemMark != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDeletedValueList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIsConflict != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * DEFINITION_SCHEMA.USERS 테이블의 Row 정보 구성 
     */
    
    sTableHandle = gEldTablePhysicalHandle[ELDT_TABLE_ID_USERS];

    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_USERS,
                                     aMemMark,
                                     & sTableValueList,
                                     aEnv )
             == STL_SUCCESS );
    sState = 1;

    STL_TRY( smlInitSingleRowBlock( &sRowBlock,
                                    &sRowRid,
                                    &sRowScn,
                                    SML_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * 조건 생성
     *  FROM DEFINITION_SCHEMA.USERS
     * WHERE AUTH_ID = sUserID
     */

    sUserID = ellGetAuthID( aUserHandle );
    
    sFilterColumn = ellFindColumnValueList( sTableValueList,
                                            ELDT_TABLE_ID_USERS,
                                            ELDT_User_ColumnOrder_AUTH_ID );

    STL_TRY( eldMakeOneEquiRange( aTransID,
                                  aStmt,
                                  ELDT_TABLE_ID_USERS,
                                  sFilterColumn,
                                  & sUserID,
                                  STL_SIZEOF(stlInt64),
                                  & sIndexHandle,
                                  & sKeyCompList,
                                  & sIndexValueList,
                                  & sRangePred,
                                  & sRangeContext,
                                  aEnv )
             == STL_SUCCESS );

    /**
     * Dictionary Read 전용 fetch record 정보 설정
     */

    STL_TRY( eldBeginForIndexAccessQuery( aStmt,
                                          sTableHandle,
                                          sIndexHandle,
                                          sKeyCompList,
                                          & sIteratorProperty,
                                          & sRowBlock,
                                          sTableValueList,
                                          sIndexValueList,
                                          sRangePred,
                                          sRangeContext,
                                          & sFetchInfo,
                                          & sFetchRecordInfo,
                                          & sIterator,
                                          aEnv )                    
             == STL_SUCCESS );
    
    sBeginSelect = STL_TRUE;

    /**
     * 레코드를 읽는다.
     */
    
    STL_TRY( eldFetchNext( sIterator,
                           & sFetchInfo, 
                           aEnv )
             == STL_SUCCESS );


    if( sFetchInfo.mIsEndOfScan == STL_TRUE )
    {
        sHasConflict = STL_TRUE;
        STL_THROW(RAMP_FINISH);
    }
    
    /**
     * Row Delete
     */

    STL_TRY( smlDeleteRecord( aStmt,
                              sTableHandle,
                              & sRowRid,
                              sRowScn,
                              0,     /* aValueIdx */
                              NULL,  /* aPrimaryKey */
                              & sVersionConflict,
                              & sSkipped,
                              SML_ENV(aEnv) )
             == STL_SUCCESS );

    if( (sSkipped == STL_TRUE) || (sVersionConflict == STL_TRUE) )
    {
        sHasConflict = STL_TRUE;
        STL_THROW(RAMP_FINISH);
    }
    
    /**
     * Row 삭제에 대한 인덱스 갱신 및 무결성 검증
     */

    STL_TRY( eldRefineIntegrityRowDelete( aTransID,
                                          aStmt,
                                          STL_TRUE,   /* UPDATE 를 위한 삭제 여부 */
                                          ELDT_TABLE_ID_USERS,
                                          & sRowBlock,
                                          sTableValueList,
                                          aEnv )
             == STL_SUCCESS );

    STL_RAMP( RAMP_FINISH );
    
    /**
     * Iterator 종료
     */
    
    sBeginSelect = STL_FALSE;
    STL_TRY( eldEndForQuery( sIterator,
                             aEnv )
             == STL_SUCCESS );

    
    /**
     * Output 설정
     */

    *aDeletedValueList = sTableValueList;
    *aIsConflict = sHasConflict;
    
    return STL_SUCCESS;

    STL_FINISH;

    if ( sBeginSelect == STL_TRUE )
    {
        (void) eldEndForQuery( sIterator,
                               aEnv );
    }

    switch (sState)
    {
        case 1:
            (void) eldFreeTableValueList( aMemMark, aEnv );
        default:
            break;
    }
    
    return STL_FAILURE;
}




/**
 * @brief Authentication Information 을 Value List 에 설정한다.
 * @param[in]  aAuthenInfo   User Authentication Information
 * @param[in]  aValueList    Value List
 * @param[in]  aEnv          Environment
 * @remarks
 */
stlStatus eldSetAuthenInfo( ellAuthenInfo      * aAuthenInfo,
                            knlValueBlockList  * aValueList,
                            ellEnv             * aEnv )
{
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aAuthenInfo != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aValueList != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * ENCRYPTED_PASSWORD
     */
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_USERS,
                                ELDT_User_ColumnOrder_ENCRYPTED_PASSWORD,
                                aValueList,
                                aAuthenInfo->mEncryptPassword,
                                aEnv )
             == STL_SUCCESS );

    /**
     * LOCKED_STATUS
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_USERS,
                                ELDT_User_ColumnOrder_LOCKED_STATUS,
                                aValueList,
                                (void *) gEllUserLockedString[aAuthenInfo->mLockedStatus],
                                aEnv )
             == STL_SUCCESS );
    
    /**
     * LOCKED_STATUS_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_USERS,
                                ELDT_User_ColumnOrder_LOCKED_STATUS_ID,
                                aValueList,
                                & aAuthenInfo->mLockedStatus,
                                aEnv )
             == STL_SUCCESS );
    
    /**
     * LOCKED_TIME
     */

    if ( aAuthenInfo->mLockedTime == ELL_DICT_STLTIME_NA )
    {
        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_USERS,
                                    ELDT_User_ColumnOrder_LOCKED_TIME,
                                    aValueList,
                                    NULL,
                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_USERS,
                                    ELDT_User_ColumnOrder_LOCKED_TIME,
                                    aValueList,
                                    & aAuthenInfo->mLockedTime,
                                    aEnv )
                 == STL_SUCCESS );
    }
    
    /**
     * FAILED_LOGIN_ATTEMPTS
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_USERS,
                                ELDT_User_ColumnOrder_FAILED_LOGIN_ATTEMPTS,
                                aValueList,
                                & aAuthenInfo->mFailedLoginAttempt,
                                aEnv )
             == STL_SUCCESS );

    /**
     * EXPIRY_STATUS
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_USERS,
                                ELDT_User_ColumnOrder_EXPIRY_STATUS,
                                aValueList,
                                (void *) gEllUserExpiryString[aAuthenInfo->mExpiryStatus],
                                aEnv )
             == STL_SUCCESS );
    
    /**
     * EXPIRY_STATUS_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_USERS,
                                ELDT_User_ColumnOrder_EXPIRY_STATUS_ID,
                                aValueList,
                                & aAuthenInfo->mExpiryStatus,
                                aEnv )
             == STL_SUCCESS );

    /**
     * EXPIRY_TIME
     */

    if ( aAuthenInfo->mExpiryTime == ELL_DICT_STLTIME_NA )
    {
        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_USERS,
                                    ELDT_User_ColumnOrder_EXPIRY_TIME,
                                    aValueList,
                                    NULL,
                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_USERS,
                                    ELDT_User_ColumnOrder_EXPIRY_TIME,
                                    aValueList,
                                    & aAuthenInfo->mExpiryTime,
                                    aEnv )
                 == STL_SUCCESS );
    }

    /**
     * PASSWORD_CHANGE_COUNT
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_USERS,
                                ELDT_User_ColumnOrder_PASSWORD_CHANGE_COUNT,
                                aValueList,
                                & aAuthenInfo->mPassChangeCount,
                                aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

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
stlStatus eldGetUserAuthenticationInfo( smlTransId           aTransID,
                                        smlStatement       * aStmt,
                                        ellDictHandle      * aUserHandle,
                                        ellAuthenInfo      * aAuthenInfo,
                                        ellEnv             * aEnv )
{
    smlRowBlock         sRowBlock;
    smlRid              sRowRid;
    smlScn              sRowScn;
    
    stlBool             sBeginSelect = STL_FALSE;

    void              * sTableHandle = NULL;
    knlValueBlockList * sTableValueList = NULL;

    knlValueBlockList   * sFilterColumn = NULL;

    void              * sIndexHandle = NULL;
    knlValueBlockList * sIndexValueList = NULL;
    knlKeyCompareList * sKeyCompList = NULL;
    
    void              * sIterator  = NULL;
    smlIteratorProperty sIteratorProperty;

    knlPredicateList    * sRangePred = NULL;
    knlExprContextInfo  * sRangeContext = NULL;
    
    dtlDataValue      * sDataValue = NULL;

    smlFetchInfo        sFetchInfo;
    smlFetchRecordInfo  sFetchRecordInfo;
    eldMemMark          sMemMark;

    stlInt64            sUserID = ELL_DICT_OBJECT_ID_NA;

    stlInt32  sState = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aUserHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAuthenInfo != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * DEFINITION_SCHEMA.USERS 테이블의 table handle 을 획득
     */
    
    sTableHandle = gEldTablePhysicalHandle[ELDT_TABLE_ID_USERS];

    /**
     * DEFINITION_SCHEMA.USERS 테이블의 Value List와 Row Block 할당
     */

    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_USERS,
                                     & sMemMark,
                                     & sTableValueList,
                                     aEnv )
             == STL_SUCCESS );
    sState = 1;

    STL_TRY( smlInitSingleRowBlock( &sRowBlock,
                                    &sRowRid,
                                    &sRowScn,
                                    SML_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * 조건 생성
     * WHERE AUTH_ID = sUserID
     */

    STL_DASSERT( ellGetAuthType( aUserHandle ) == ELL_AUTHORIZATION_TYPE_USER );
    
    sUserID = ellGetAuthID( aUserHandle );
    sFilterColumn = ellFindColumnValueList( sTableValueList,
                                            ELDT_TABLE_ID_USERS,
                                            ELDT_User_ColumnOrder_AUTH_ID );

    /**
     * Key Range 생성
     */
    
    STL_TRY( eldMakeOneEquiRange( aTransID,
                                  aStmt,
                                  ELDT_TABLE_ID_USERS,
                                  sFilterColumn,
                                  & sUserID,
                                  STL_SIZEOF(stlInt64),
                                  & sIndexHandle,
                                  & sKeyCompList,
                                  & sIndexValueList,
                                  & sRangePred,
                                  & sRangeContext,
                                  aEnv )
             == STL_SUCCESS );
    
    /**
     * SELECT 준비
     */
    
    STL_TRY( eldBeginForIndexAccessQuery( aStmt,
                                          sTableHandle,
                                          sIndexHandle,
                                          sKeyCompList,
                                          & sIteratorProperty,
                                          & sRowBlock,
                                          sTableValueList,
                                          sIndexValueList,
                                          sRangePred,
                                          sRangeContext,
                                          & sFetchInfo,
                                          & sFetchRecordInfo,
                                          & sIterator,
                                          aEnv )                    
             == STL_SUCCESS );
    
    /**
     * User 의 Authentication 정보를 획득한다.
     */

    stlMemset( aAuthenInfo, 0x00, STL_SIZEOF(ellAuthenInfo) );
    
    sBeginSelect = STL_TRUE;
    
    while ( 1 )
    {
        STL_TRY( eldFetchNext( sIterator,
                               & sFetchInfo,
                               aEnv )
                 == STL_SUCCESS );
        
        if( sFetchInfo.mIsEndOfScan == STL_TRUE )
        {
            break;
        }

        /*******************************************************************
         * Account Lock 관련 정보 
         *******************************************************************/
        
        /**
         * LOCKED_STATUS_ID
         */

        sDataValue = eldFindDataValue( sTableValueList,
                                       ELDT_TABLE_ID_USERS,
                                       ELDT_User_ColumnOrder_LOCKED_STATUS_ID );

        if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
        {
            STL_DASSERT(0);
        }
        else
        {
            stlMemcpy( & aAuthenInfo->mLockedStatus,
                       sDataValue->mValue,
                       sDataValue->mLength );
            STL_DASSERT( STL_SIZEOF(aAuthenInfo->mLockedStatus) == sDataValue->mLength );
        }

        /**
         * LOCKED_TIME
         */

        sDataValue = eldFindDataValue( sTableValueList,
                                       ELDT_TABLE_ID_USERS,
                                       ELDT_User_ColumnOrder_LOCKED_TIME );

        if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
        {
            aAuthenInfo->mLockedTime = ELL_DICT_STLTIME_NA;
        }
        else
        {
            STL_TRY( dtlFromTimestampToStlTime( (dtlTimestampType*) sDataValue->mValue,
                                                & aAuthenInfo->mLockedTime,
                                                KNL_DT_VECTOR(aEnv),
                                                aEnv,
                                                ELL_ERROR_STACK(aEnv) )
                     == STL_SUCCESS );
        }

        /**
         * FAILED_LOGIN_ATTEMPTS
         */

        sDataValue = eldFindDataValue( sTableValueList,
                                       ELDT_TABLE_ID_USERS,
                                       ELDT_User_ColumnOrder_FAILED_LOGIN_ATTEMPTS );

        if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
        {
            STL_DASSERT(0);
        }
        else
        {
            stlMemcpy( & aAuthenInfo->mFailedLoginAttempt,
                       sDataValue->mValue,
                       sDataValue->mLength );
            STL_DASSERT( STL_SIZEOF(aAuthenInfo->mFailedLoginAttempt) == sDataValue->mLength );
        }

        /*******************************************************************
         * Password 관련 정보 
         *******************************************************************/
        
        /**
         * EXPIRY_STATUS_ID
         */

        sDataValue = eldFindDataValue( sTableValueList,
                                       ELDT_TABLE_ID_USERS,
                                       ELDT_User_ColumnOrder_EXPIRY_STATUS_ID );

        if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
        {
            STL_DASSERT(0);
        }
        else
        {
            stlMemcpy( & aAuthenInfo->mExpiryStatus,
                       sDataValue->mValue,
                       sDataValue->mLength );
            STL_DASSERT( STL_SIZEOF(aAuthenInfo->mExpiryStatus) == sDataValue->mLength );
        }
        
        /**
         * EXPIRY_TIME
         */

        sDataValue = eldFindDataValue( sTableValueList,
                                       ELDT_TABLE_ID_USERS,
                                       ELDT_User_ColumnOrder_EXPIRY_TIME );

        if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
        {
            aAuthenInfo->mExpiryTime = ELL_DICT_STLTIME_NA;
        }
        else
        {
            STL_TRY( dtlFromTimestampToStlTime( (dtlTimestampType*) sDataValue->mValue,
                                                & aAuthenInfo->mExpiryTime,
                                                KNL_DT_VECTOR(aEnv),
                                                aEnv,
                                                ELL_ERROR_STACK(aEnv) )
                     == STL_SUCCESS );
        }

        /**
         * ENCRYPTED_PASSWORD
         */

        sDataValue = eldFindDataValue( sTableValueList,
                                       ELDT_TABLE_ID_USERS,
                                       ELDT_User_ColumnOrder_ENCRYPTED_PASSWORD );

        if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
        {
            STL_DASSERT(0);
        }
        else
        {
            stlMemcpy( & aAuthenInfo->mEncryptPassword,
                       sDataValue->mValue,
                       sDataValue->mLength );
            aAuthenInfo->mEncryptPassword[sDataValue->mLength] = '\0';
        }
        
        /**
         * PASSWORD_CHANGE_COUNT
         */

        sDataValue = eldFindDataValue( sTableValueList,
                                       ELDT_TABLE_ID_USERS,
                                       ELDT_User_ColumnOrder_PASSWORD_CHANGE_COUNT );

        if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
        {
            STL_DASSERT(0);
        }
        else
        {
            stlMemcpy( & aAuthenInfo->mPassChangeCount,
                       sDataValue->mValue,
                       sDataValue->mLength );
            STL_DASSERT( STL_SIZEOF(aAuthenInfo->mPassChangeCount) == sDataValue->mLength );
        }
    }

    /**
     * SELECT 종료
     */
    
    sBeginSelect = STL_FALSE;
    STL_TRY( eldEndForQuery( sIterator,
                             aEnv )
             == STL_SUCCESS );

    /**
     * 사용한 메모리를 해제한다.
     */

    sState = 0;
    STL_TRY( eldFreeTableValueList( & sMemMark, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    if ( sBeginSelect == STL_TRUE )
    {
        (void) eldEndForQuery( sIterator,
                               aEnv );
    }

    switch (sState)
    {
        case 1:
            (void) eldFreeTableValueList( & sMemMark, aEnv );
        default:
            break;
    }
    
    return STL_FAILURE;
}



/** @} eldUser */
