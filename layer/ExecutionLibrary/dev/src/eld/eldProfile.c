/*******************************************************************************
 * eldProfile.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: eldProfile.c 14729 2015-04-06 03:16:14Z leekmo $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


/**
 * @file eldProfile.c
 * @brief Profile Dictionary 관리 루틴 
 */

#include <ell.h>
#include <eldt.h>
#include <eldc.h>

#include <eldIntegrity.h>
#include <eldDictionary.h>
#include <eldProfile.h>

/**
 * @addtogroup eldProfile
 * @{
 */

/**
 * @brief Profile Descriptor 를 Dictionary 에 추가한다.
 * @remarks
 * 함수 ellInsertProfileDesc() 주석 참조 
 */
stlStatus eldInsertProfileDesc( smlTransId             aTransID,
                                smlStatement         * aStmt,
                                stlInt64             * aProfileID,
                                stlChar              * aProfileName,
                                stlChar              * aProfileComment,
                                ellEnv               * aEnv )
{
    ellSessionEnv * sSessEnv = ELL_SESS_ENV(aEnv);
    
    stlInt64   sProfileID = 0;
    
    knlValueBlockList  * sDataValueList  = NULL;

    smlRid      sRowRid;
    smlScn      sRowScn;
    smlRowBlock sRowBlock;
    eldMemMark  sMemMark;

    stlInt32 sState = 0;
    
    /*
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aProfileID != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aProfileName != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * - Data Value List와 Row Block을 위한 공간 할당 및 초기화
     */
    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_PROFILES,
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

    if ( gEllIsMetaBuilding == STL_TRUE )
    {
        /* Database Dictionary 를 초기화하는 단계 */
        gEldtIdentityValue[ELDT_TABLE_ID_PROFILES] += 1;
        sProfileID = gEldtIdentityValue[ELDT_TABLE_ID_PROFILES];
    }
    else
    {
        /* Database가 이미 구축되어 있는 단계 */
        
        /**
         * Identity Column으로부터 Profile ID를 획득
         */
        STL_TRY( smlGetNextSequenceVal( gEldIdentityColumnHandle[ELDT_TABLE_ID_PROFILES],
                                        & sProfileID,
                                        SML_ENV( aEnv ) )
                 == STL_SUCCESS );
    }
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_PROFILES,
                                ELDT_Profile_ColumnOrder_PROFILE_ID,
                                sDataValueList,
                                & sProfileID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * PROFILE_NAME
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_PROFILES,
                                ELDT_Profile_ColumnOrder_PROFILE_NAME,
                                sDataValueList,
                                aProfileName,
                                aEnv )
             == STL_SUCCESS );

    /**
     * IS_BUILTIN
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_PROFILES,
                                ELDT_Profile_ColumnOrder_IS_BUILTIN,
                                sDataValueList,
                                & gEllIsMetaBuilding,
                                aEnv )
             == STL_SUCCESS );
    
    
    /*
     * CREATED_TIME
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_PROFILES,
                                ELDT_Profile_ColumnOrder_CREATED_TIME,
                                sDataValueList,
                                & sSessEnv->mTimeDDL,
                                aEnv )
             == STL_SUCCESS );

    /*
     * MODIFIED_TIME
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_PROFILES,
                                ELDT_Profile_ColumnOrder_MODIFIED_TIME,
                                sDataValueList,
                                & sSessEnv->mTimeDDL,
                                aEnv )
             == STL_SUCCESS );

    /*
     * COMMENTS
     */
    
    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_PROFILES,
                                ELDT_Profile_ColumnOrder_COMMENTS,
                                sDataValueList,
                                aProfileComment,
                                aEnv )
             == STL_SUCCESS );
    
    /**
     * - 레코드를 넣는다.
     */

    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sDataValueList, 0, 1 );

    STL_TRY( smlInsertRecord( aStmt,
                              gEldTablePhysicalHandle[ELDT_TABLE_ID_PROFILES],
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
                                              ELDT_TABLE_ID_PROFILES,
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
    
    /**
     * Output 설정
     */
    
    *aProfileID = sProfileID;
    
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



/** @} eldProfile */
