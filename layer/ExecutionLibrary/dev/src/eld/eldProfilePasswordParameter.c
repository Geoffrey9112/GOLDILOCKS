/*******************************************************************************
 * eldProfilePasswordParameter.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: eldProfilePasswordParameter.c 14729 2015-04-06 03:16:14Z leekmo $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


/**
 * @file eldProfilePasswordParameter.c
 * @brief Profile Dictionary 관리 루틴 
 */

#include <ell.h>
#include <eldt.h>
#include <eldc.h>

#include <eldIntegrity.h>
#include <eldDictionary.h>
#include <eldProfilePasswordParameter.h>

/**
 * @addtogroup eldProfilePasswordParameter
 * @{
 */

/**
 * @brief Profile Password Parameter Descriptor 를 Dictionary 에 추가한다.
 * @param[in]  aTransID               Transaction ID
 * @param[in]  aStmt                  Statement
 * @param[in]  aProfileID             Profile ID
 * @param[in]  aProfileParam          Profile Parameter ID
 * @param[in]  aIsDefault             LIMIT parameter DEFAULT 여부 
 * @param[in]  aLimitValue            LIMIT value
 * @param[in]  aLimitString           LIMIT string
 * @param[in]  aEnv                   Environment
 * @remarks
 */
stlStatus eldInsertProfilePassParamDesc( smlTransId             aTransID,
                                         smlStatement         * aStmt,
                                         stlInt64               aProfileID,
                                         ellProfileParam        aProfileParam,
                                         stlBool                aIsDefault,
                                         dtlDataValue         * aLimitValue,
                                         stlChar              * aLimitString,
                                         ellEnv               * aEnv )
{
    knlValueBlockList  * sDataValueList  = NULL;

    smlRid      sRowRid;
    smlScn      sRowScn;
    smlRowBlock sRowBlock;
    eldMemMark  sMemMark;

    stlInt32   sState = 0;
    
    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aProfileID > ELL_DICT_OBJECT_ID_NA, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aProfileParam > ELL_PROFILE_PARAM_NA) && (aProfileParam < ELL_PROFILE_PARAM_MAX)
                        , ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aLimitValue != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aLimitValue->mType == DTL_TYPE_NUMBER, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aLimitString != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * - Data Value List와 Row Block을 위한 공간 할당 및 초기화
     */
    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_PROFILE_PASSWORD_PARAMETER,
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
     * - profile descriptor의 정보를 구성한다.
     */

    /*
     * PROFILE_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_PROFILE_PASSWORD_PARAMETER,
                                ELDT_PassParam_ColumnOrder_PROFILE_ID,
                                sDataValueList,
                                & aProfileID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * PARAMETER_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_PROFILE_PASSWORD_PARAMETER,
                                ELDT_PassParam_ColumnOrder_PARAMETER_ID,
                                sDataValueList,
                                & aProfileParam,
                                aEnv )
             == STL_SUCCESS );

    /*
     * PARAMETER_NAME
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_PROFILE_PASSWORD_PARAMETER,
                                ELDT_PassParam_ColumnOrder_PARAMETER_NAME,
                                sDataValueList,
                                (void *) gEllProfileParamString[aProfileParam],
                                aEnv )
             == STL_SUCCESS );
    
    /*
     * IS_DEFAULT
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_PROFILE_PASSWORD_PARAMETER,
                                ELDT_PassParam_ColumnOrder_IS_DEFAULT,
                                sDataValueList,
                                & aIsDefault,
                                aEnv )
             == STL_SUCCESS );

    /*
     * PARAMETER_VALUE
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_PROFILE_PASSWORD_PARAMETER,
                                ELDT_PassParam_ColumnOrder_PARAMETER_VALUE,
                                sDataValueList,
                                aLimitValue,
                                aEnv )
             == STL_SUCCESS );

    /*
     * PARAMETER_STRING
     */
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_PROFILE_PASSWORD_PARAMETER,
                                ELDT_PassParam_ColumnOrder_PARAMETER_STRING,
                                sDataValueList,
                                aLimitString,
                                aEnv )
             == STL_SUCCESS );
    
    /**
     * - 레코드를 넣는다.
     */

    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sDataValueList, 0, 1 );

    STL_TRY( smlInsertRecord( aStmt,
                              gEldTablePhysicalHandle[ELDT_TABLE_ID_PROFILE_PASSWORD_PARAMETER],
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
                                              ELDT_TABLE_ID_PROFILE_PASSWORD_PARAMETER,
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
 * @brief Profile 의 Password Parameter 를 제거한다.
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aProfileID   Profile ID
 * @param[in] aProfileParam  Profile Parameter
 * @param[in] aEnv           Environent
 * @remarks
 */ 
stlStatus eldDeleteProfilePassParam( smlTransId             aTransID,
                                     smlStatement         * aStmt,
                                     stlInt64               aProfileID,
                                     ellProfileParam        aProfileParam,
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

    ellProfileParam  sParamID = ELL_PROFILE_PARAM_NA;

    stlInt32 sState = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aProfileID > ELL_DICT_OBJECT_ID_NA, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aProfileParam > ELL_PROFILE_PARAM_NA) &&
                        (aProfileParam < ELL_PROFILE_PARAM_MAX),
                        ELL_ERROR_STACK(aEnv) );
    
    /**
     * DEFINITION_SCHEMA.PROFILE_PASSWORD_PARAMETER 테이블의 table handle 을 획득
     */
    
    sTableHandle = gEldTablePhysicalHandle[ELDT_TABLE_ID_PROFILE_PASSWORD_PARAMETER];

    /**
     * DEFINITION_SCHEMA.PROFILE_PASSWORD_PARAMETER 테이블의 Value List와 Row Block 할당
     */

    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_PROFILE_PASSWORD_PARAMETER,
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
     * WHERE PROFILE_ID = aProfileID
     */

    sFilterColumn = ellFindColumnValueList( sTableValueList,
                                            ELDT_TABLE_ID_PROFILE_PASSWORD_PARAMETER,
                                            ELDT_PassParam_ColumnOrder_PROFILE_ID );

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
                                  ELDT_TABLE_ID_PROFILE_PASSWORD_PARAMETER,
                                  sFilterColumn,
                                  & aProfileID,
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
             * PARAMETER_ID 에 해당하는 Row 제거 
             *******************************************************/
            
            /**
             * PARAMETER_ID
             */

            sDataValue = eldFindDataValue( sTableValueList,
                                           ELDT_TABLE_ID_PROFILE_PASSWORD_PARAMETER,
                                           ELDT_PassParam_ColumnOrder_PARAMETER_ID );

            if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
            {
                STL_DASSERT(0);
            }
            else
            {
                stlMemcpy( & sParamID,
                           sDataValue->mValue,
                           sDataValue->mLength );
                STL_DASSERT( STL_SIZEOF(sParamID) == sDataValue->mLength );
            }

            if ( sParamID == aProfileParam )
            {
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
                                                      ELDT_TABLE_ID_PROFILE_PASSWORD_PARAMETER,
                                                      & sRowBlock,
                                                      sTableValueList,
                                                      aEnv )
                         == STL_SUCCESS );

                break;
            }
            else
            {
                continue;
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


/** @} eldProfilePasswordParameter */
