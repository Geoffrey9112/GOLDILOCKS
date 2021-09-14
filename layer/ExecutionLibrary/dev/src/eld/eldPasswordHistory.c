/*******************************************************************************
 * eldPasswordHistory.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: eldPasswordHistory.c 13496 2014-08-29 05:38:43Z leekmo $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


/**
 * @file eldPasswordHistory.c
 * @brief User Schema Path Dictionary 관리 루틴 
 */

#include <ell.h>
#include <eldt.h>
#include <eldc.h>

#include <eldIntegrity.h>
#include <eldDictionary.h>
#include <eldPasswordHistory.h>

/**
 * @addtogroup eldPasswordHistory
 * @{
 */






/**
 * @brief ALTER DATABASE CLEAR PASSWORD HISTORY 를 수행
 * @param[in] aTransID             Transaction ID
 * @param[in] aStmt                Statement
 * @param[in] aEnv                 Environment
 * @remarks
 */
stlStatus eldClearUserPasswordHistory( smlTransId       aTransID,
                                       smlStatement   * aStmt,
                                       ellEnv         * aEnv )
{
    stlBool             sBeginSelect = STL_FALSE;
    
    void              * sTableHandle = NULL;
    knlValueBlockList * sTableValueList = NULL;

    void                * sIterator  = NULL;
    smlIteratorProperty   sIteratorProperty;

    smlRid              sRowRid;
    smlScn              sRowScn;
    smlRowBlock         sRowBlock;
    eldMemMark          sMemMark;

    smlFetchInfo          sFetchInfo;

    stlBool  sVersionConflict = STL_FALSE;
    stlBool  sSkipped = STL_FALSE;

    stlInt32 sState = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * DEFINITION_SCHEMA.USER_PASSWORD_HISTORY 테이블의 table handle 을 획득
     */
    
    sTableHandle = gEldTablePhysicalHandle[ELDT_TABLE_ID_USER_PASSWORD_HISTORY];

    /**
     * DEFINITION_SCHEMA.USER_PASSWORD_HISTORY 테이블의 Value List와 Row Block 할당
     */

    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_USER_PASSWORD_HISTORY,
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
     * RETRY
     */
    
    STL_RAMP( RAMP_RETRY );

    if ( sVersionConflict == STL_TRUE )
    {
        sBeginSelect = STL_FALSE;
        STL_TRY( eldEndForQuery( sIterator,
                                 aEnv )
                 == STL_SUCCESS );
        
        STL_TRY( smlResetStatement( aStmt,
                                    STL_FALSE, /* aDoRollback */
                                    SML_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    
    /**
     * SELECT 준비
     */

    STL_TRY( eldBeginForTableAccessQuery( aStmt,
                                          sTableHandle,
                                          & sIteratorProperty,
                                          & sRowBlock,
                                          sTableValueList,
                                          NULL,
                                          NULL,
                                          & sFetchInfo,
                                          NULL,
                                          & sIterator,         
                                          aEnv )
             == STL_SUCCESS );

    sBeginSelect = STL_TRUE;

    /**
     * Column Descriptor 를 Cache 에 추가한다.
     */

    while ( 1 )
    {
        /**
         * 레코드를 읽는다.
         */
        
        STL_TRY( eldFetchNext( sIterator,
                               & sFetchInfo,
                               aEnv )
                 == STL_SUCCESS );

        if( sFetchInfo.mIsEndOfScan == STL_TRUE )
        {
            break;
        }

        /**
         * 레코드 삭제 
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
        
        if ( sSkipped == STL_TRUE )
        {
            continue;
        }
        
        if ( sVersionConflict == STL_TRUE )
        {
            STL_THROW( RAMP_RETRY );
        }
        
        /**
         * 인덱스 갱신 및 무결성 검증
         */
        
        STL_TRY( eldRefineIntegrityRowDelete( aTransID,
                                              aStmt,
                                              STL_FALSE,   /* UPDATE 를 위한 삭제 여부 */
                                              ELDT_TABLE_ID_USER_PASSWORD_HISTORY,
                                              & sRowBlock,
                                              sTableValueList,
                                              aEnv )
                 == STL_SUCCESS );
        
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
stlStatus eldInsertPasswordHistoryDesc( smlTransId             aTransID,
                                        smlStatement         * aStmt,
                                        stlInt64               aAuthID,
                                        stlChar              * aEncryptedPassword,
                                        stlInt64               aPassChangeNO,
                                        stlTime                aPassChangeTime,
                                        ellEnv               * aEnv )

{
    knlValueBlockList        * sDataValueList  = NULL;

    smlRid                 sRowRid;
    smlScn                 sRowScn;
    smlRowBlock            sRowBlock;
    eldMemMark             sMemMark;

    stlInt32   sState = 0;
    
    /*
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAuthID > ELL_DICT_OBJECT_ID_NA, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aEncryptedPassword != 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPassChangeNO >= 0, ELL_ERROR_STACK(aEnv) );
    
    /**
     * - Data Value List와 Row Block을 위한 공간 할당 및 초기화
     */
    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_USER_PASSWORD_HISTORY,
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
     * - user schema path descriptor의 정보를 구성한다.
     */

    /*
     * AUTH_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_USER_PASSWORD_HISTORY,
                                ELDT_PassHist_ColumnOrder_AUTH_ID,
                                sDataValueList,
                                & aAuthID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * ENCRYPTED_PASSWORD
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_USER_PASSWORD_HISTORY,
                                ELDT_PassHist_ColumnOrder_ENCRYPTED_PASSWORD,
                                sDataValueList,
                                aEncryptedPassword,
                                aEnv )
             == STL_SUCCESS );

    /*
     * PASSWORD_CHANGE_NO
     */
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_USER_PASSWORD_HISTORY,
                                ELDT_PassHist_ColumnOrder_PASSWORD_CHANGE_NO,
                                sDataValueList,
                                & aPassChangeNO,
                                aEnv )
             == STL_SUCCESS );

    /*
     * PASSWORD_CHANGE_TIME
     */
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_USER_PASSWORD_HISTORY,
                                ELDT_PassHist_ColumnOrder_PASSWORD_CHANGE_TIME,
                                sDataValueList,
                                & aPassChangeTime,
                                aEnv )
             == STL_SUCCESS );
    
    /**
     * - 레코드를 넣는다.
     */

    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sDataValueList, 0, 1 );
    
    STL_TRY( smlInsertRecord( aStmt,
                              gEldTablePhysicalHandle[ELDT_TABLE_ID_USER_PASSWORD_HISTORY],
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
                                              ELDT_TABLE_ID_USER_PASSWORD_HISTORY,
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
 * password 가 동일하고 다음 두 조건 중 하나를 만족하면 재사용할 수 없는 password 임
 * 조건 1) >= aMinChangeNO 
 * 조건 2) >= MinChangeTime 
 */
stlStatus eldFindDisableReusePassword( smlTransId             aTransID,
                                       smlStatement         * aStmt,
                                       ellDictHandle        * aUserHandle,
                                       stlChar              * aEncryptedPassword,
                                       stlInt64               aMinChangeNO,
                                       stlTime                aMinChangeTime,
                                       stlBool              * aExist,
                                       ellEnv               * aEnv )
{
    stlBool             sBeginSelect = STL_FALSE;
    
    void              * sTableHandle = NULL;
    knlValueBlockList * sTableValueList = NULL;

    void                * sIterator  = NULL;
    smlIteratorProperty   sIteratorProperty;

    knlValueBlockList   * sFilterColumn = NULL;

    void              * sIndexHandle = NULL;
    knlValueBlockList * sIndexValueList = NULL;
    knlKeyCompareList * sKeyCompList = NULL;
    
    knlPredicateList    * sRangePred = NULL;
    knlExprContextInfo  * sRangeContext = NULL;
    
    smlRid       sRowRid;
    smlScn       sRowScn;
    smlRowBlock  sRowBlock;

    smlFetchInfo        sFetchInfo;
    smlFetchRecordInfo  sFetchRecordInfo;
    eldMemMark          sMemMark;

    dtlDataValue * sDataValue = NULL;
    
    stlInt64 sUserID = ELL_DICT_OBJECT_ID_NA;

    stlChar   sPassword[STL_MAX_SQL_IDENTIFIER_LENGTH] = {0,};
    stlInt64  sChangeNO = 0;
    stlTime   sChangeTime = 0;
    
    stlBool  sFind = STL_FALSE;

    stlInt32 sState = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aUserHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aEncryptedPassword != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aMinChangeNO >= 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExist != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * DEFINITION_SCHEMA.USER_PASSWORD_HISTORY 테이블의 table handle 을 획득
     */
    
    sTableHandle = gEldTablePhysicalHandle[ELDT_TABLE_ID_USER_PASSWORD_HISTORY];

    /**
     * DEFINITION_SCHEMA.USER_PASSWORD_HISTORY 테이블의 Value List와 Row Block 할당
     */

    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_USER_PASSWORD_HISTORY,
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
     * WHERE AUTH_ID = aAuthID
     */

    sUserID = ellGetAuthID( aUserHandle );
    
    sFilterColumn = ellFindColumnValueList( sTableValueList,
                                            ELDT_TABLE_ID_USER_PASSWORD_HISTORY,
                                            ELDT_PassHist_ColumnOrder_AUTH_ID );

    /**
     * Key Range 생성
     */
    
    STL_TRY( eldMakeOneEquiRange( aTransID,
                                  aStmt,
                                  ELDT_TABLE_ID_USER_PASSWORD_HISTORY,
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
    
    sBeginSelect = STL_TRUE;

    /**
     * REUSE 조건을 위배하는 password 가 존재하는 지 검사
     */

    sFind = STL_FALSE;
    
    while ( 1 )
    {
        /**
         * 레코드를 읽는다.
         */

        STL_TRY( eldFetchNext( sIterator,
                               & sFetchInfo, 
                               aEnv )
                 == STL_SUCCESS );

        if( sFetchInfo.mIsEndOfScan == STL_TRUE )
        {
            break;
        }
        else
        {
            /**
             * ENCRYPTED_PASSWORD
             */
            
            sDataValue = eldFindDataValue( sTableValueList,
                                           ELDT_TABLE_ID_USER_PASSWORD_HISTORY,
                                           ELDT_PassHist_ColumnOrder_ENCRYPTED_PASSWORD );
            
            if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
            {
                STL_DASSERT(0);
            }
            else
            {
                stlMemcpy( sPassword,
                           sDataValue->mValue,
                           sDataValue->mLength );
                sPassword[sDataValue->mLength] = '\0';
            }

            if ( stlStrcmp( sPassword, aEncryptedPassword ) == 0 )
            {
                /* password 가 동일함 */
            }
            else
            {
                /* password 가 다름 */
                continue;
            }
            
            /**
             * PASSWORD_CHANGE_NO
             */

            sDataValue = eldFindDataValue( sTableValueList,
                                           ELDT_TABLE_ID_USER_PASSWORD_HISTORY,
                                           ELDT_PassHist_ColumnOrder_PASSWORD_CHANGE_NO );

            if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
            {
                STL_DASSERT(0);
            }
            else
            {
                stlMemcpy( & sChangeNO,
                           sDataValue->mValue,
                           sDataValue->mLength );
                STL_DASSERT( STL_SIZEOF(sChangeNO) == sDataValue->mLength );
            }

            /**
             * PASSWORD_CHANGE_TIME
             */

            sDataValue = eldFindDataValue( sTableValueList,
                                           ELDT_TABLE_ID_USER_PASSWORD_HISTORY,
                                           ELDT_PassHist_ColumnOrder_PASSWORD_CHANGE_TIME );
            
            if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
            {
                STL_DASSERT(0);
            }
            else
            {
                STL_TRY( dtlFromTimestampToStlTime( (dtlTimestampType*) sDataValue->mValue,
                                                    & sChangeTime,
                                                    KNL_DT_VECTOR(aEnv),
                                                    aEnv,
                                                    ELL_ERROR_STACK(aEnv) )
                         == STL_SUCCESS );
            }

            /**
             * REUSE 불가 여부 판단
             */
            if ( (sChangeNO < aMinChangeNO) && ( sChangeTime < aMinChangeTime ) )
            {
                continue;
            }
            else
            {
                /* reuse 불가 범위내에 있음 */
                sFind = STL_TRUE;
                break;
            }
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
    
    /**
     * output 설정
     */

    *aExist = sFind;
    
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



/**
 * @brief User Password History 에서 제거 가능한 old password 를 제거한다.
 * @param[in] aTransID             Transaction ID
 * @param[in] aStmt                Statement
 * @param[in] aUserHandle          User Handle
 * @param[in] aMinChangeNO         Min Change Number
 * @param[in] aMinChangeTime       Min Change Time
 * @param[in] aEnv                 Environment
 * @remarks
 * MinChangeNO 보다 작고 MinChangeTime 보다 작은 old password 를 제거한다.
 */
stlStatus eldRemoveOldPassword( smlTransId             aTransID,
                                smlStatement         * aStmt,
                                ellDictHandle        * aUserHandle,
                                stlInt64               aMinChangeNO,
                                stlTime                aMinChangeTime,
                                ellEnv               * aEnv )
{
    stlBool             sBeginSelect = STL_FALSE;
    
    void              * sTableHandle = NULL;
    knlValueBlockList * sTableValueList = NULL;

    void                * sIterator  = NULL;
    smlIteratorProperty   sIteratorProperty;

    knlValueBlockList   * sFilterColumn = NULL;

    void              * sIndexHandle = NULL;
    knlValueBlockList * sIndexValueList = NULL;
    knlKeyCompareList * sKeyCompList = NULL;
    
    knlPredicateList    * sRangePred = NULL;
    knlExprContextInfo  * sRangeContext = NULL;
    
    smlRid       sRowRid;
    smlScn       sRowScn;
    smlRowBlock  sRowBlock;

    smlFetchInfo        sFetchInfo;
    smlFetchRecordInfo  sFetchRecordInfo;
    eldMemMark          sMemMark;

    dtlDataValue * sDataValue = NULL;

    stlBool  sVersionConflict = STL_FALSE;
    stlBool  sSkipped = STL_FALSE;
    
    stlInt64 sUserID = ELL_DICT_OBJECT_ID_NA;

    stlInt64  sChangeNO = 0;
    stlTime   sChangeTime = 0;

    stlInt32 sState = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aUserHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aMinChangeNO >= 0, ELL_ERROR_STACK(aEnv) );
    
    /**
     * DEFINITION_SCHEMA.USER_PASSWORD_HISTORY 테이블의 table handle 을 획득
     */
    
    sTableHandle = gEldTablePhysicalHandle[ELDT_TABLE_ID_USER_PASSWORD_HISTORY];

    /**
     * DEFINITION_SCHEMA.USER_PASSWORD_HISTORY 테이블의 Value List와 Row Block 할당
     */

    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_USER_PASSWORD_HISTORY,
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
     * WHERE AUTH_ID = aAuthID
     */

    sUserID = ellGetAuthID( aUserHandle );
    
    sFilterColumn = ellFindColumnValueList( sTableValueList,
                                            ELDT_TABLE_ID_USER_PASSWORD_HISTORY,
                                            ELDT_PassHist_ColumnOrder_AUTH_ID );

    STL_RAMP( RAMP_RETRY );

    if ( sVersionConflict == STL_TRUE )
    {
        sBeginSelect = STL_FALSE;
        STL_TRY( eldEndForQuery( sIterator,
                                 aEnv )
                 == STL_SUCCESS );
        
        STL_TRY( smlResetStatement( aStmt,
                                    STL_FALSE, /* aDoRollback */
                                    SML_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    
    /**
     * Key Range 생성
     */
    
    STL_TRY( eldMakeOneEquiRange( aTransID,
                                  aStmt,
                                  ELDT_TABLE_ID_USER_PASSWORD_HISTORY,
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
    
    sBeginSelect = STL_TRUE;

    /**
     * REUSE 조건을 위배하는 password 가 존재하는 지 검사
     */

    while ( 1 )
    {
        /**
         * 레코드를 읽는다.
         */

        STL_TRY( eldFetchNext( sIterator,
                               & sFetchInfo, 
                               aEnv )
                 == STL_SUCCESS );

        if( sFetchInfo.mIsEndOfScan == STL_TRUE )
        {
            break;
        }
        else
        {
            /*******************************************************
             * 제거 가능한 지 검사
             *******************************************************/
            
            /**
             * PASSWORD_CHANGE_NO
             */

            sDataValue = eldFindDataValue( sTableValueList,
                                           ELDT_TABLE_ID_USER_PASSWORD_HISTORY,
                                           ELDT_PassHist_ColumnOrder_PASSWORD_CHANGE_NO );

            if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
            {
                STL_DASSERT(0);
            }
            else
            {
                stlMemcpy( & sChangeNO,
                           sDataValue->mValue,
                           sDataValue->mLength );
                STL_DASSERT( STL_SIZEOF(sChangeNO) == sDataValue->mLength );
            }

            /**
             * PASSWORD_CHANGE_TIME
             */

            sDataValue = eldFindDataValue( sTableValueList,
                                           ELDT_TABLE_ID_USER_PASSWORD_HISTORY,
                                           ELDT_PassHist_ColumnOrder_PASSWORD_CHANGE_TIME );
            
            if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
            {
                STL_DASSERT(0);
            }
            else
            {
                STL_TRY( dtlFromTimestampToStlTime( (dtlTimestampType*) sDataValue->mValue,
                                                    & sChangeTime,
                                                    KNL_DT_VECTOR(aEnv),
                                                    aEnv,
                                                    ELL_ERROR_STACK(aEnv) )
                         == STL_SUCCESS );
            }

            if ( (sChangeNO < aMinChangeNO) && (sChangeTime < aMinChangeTime) )
            {
                /* 제거 가능한 old password 임 */
            }
            else
            {
                /* reuse 검사가 필요한 범위임 */
                continue;
            }

            /*******************************************************
             * Old Password 삭제
             *******************************************************/
            
            /**
             * 레코드 삭제 
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

            if ( sSkipped == STL_TRUE )
            {
                continue;
            }

            if ( sVersionConflict == STL_TRUE )
            {
                STL_THROW( RAMP_RETRY );
            }
            
            /**
             * 인덱스 갱신 및 무결성 검증
             */

            STL_TRY( eldRefineIntegrityRowDelete( aTransID,
                                                  aStmt,
                                                  STL_FALSE,   /* UPDATE 를 위한 삭제 여부 */
                                                  ELDT_TABLE_ID_USER_PASSWORD_HISTORY,
                                                  & sRowBlock,
                                                  sTableValueList,
                                                  aEnv )
                     == STL_SUCCESS );
            
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

/** @} eldPasswordHistory */
