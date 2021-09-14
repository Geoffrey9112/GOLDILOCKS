/*******************************************************************************
 * eldIntegrity.c
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
 * @file eldIntegrity.c
 * @brief Dictionary 의 Integrity 관리 루틴 
 */

#include <ell.h>
#include <eldt.h>
#include <eldc.h>

#include <eldDictionary.h>
#include <eldIntegrity.h>

/**
 * @addtogroup eldIntegrity
 * @{
 */

/**
 * @brief DEFINITION_SCHEMA 구축후 테이블 무결성을 검증한다.
 * @param[in]  aEnv    Environment
 * @remarks
 * 최초 DB 생성시에만 사용되는 함수임.
 */
stlStatus eldFinishCheckAllConstraint( ellEnv * aEnv )
{
    stlInt32 sState = 0;
    
    smlTransId          sTransID  = SML_INVALID_TRANSID;
    smlStatement      * sStmt     = NULL; 

    stlInt64            sTableID    = ELL_DICT_OBJECT_ID_NA;
    stlBool             sTableExist = STL_FALSE;
    ellDictHandle       sTableHandle;
    ellInitDictHandle( & sTableHandle );
    
    /**
     * Storage Component 할당
     */
    
    /* Statement */
    STL_TRY( smlAllocStatement( sTransID,
                                NULL, /* aUpdateRelHandle */
                                SML_STMT_ATTR_READONLY,
                                SML_LOCK_TIMEOUT_INVALID,
                                STL_FALSE, /* aNeedSnapshotIterator */
                                & sStmt,
                                SML_ENV( aEnv ) )
             == STL_SUCCESS );
    sState = 1;

    /**
     * DEFINITION_SCHEMA 의 모든 테이블에 대하여 무결성 검사
     */

    for ( sTableID = ELDT_TABLE_ID_NA + 1;
          sTableID < ELDT_TABLE_ID_MAX;
          sTableID++ )
    {
        /**
         * Table Handle 획득
         */
        
        STL_TRY( eldcGetTableHandleByID( sTransID,
                                         SML_MAXIMUM_STABLE_SCN,
                                         sTableID,
                                         & sTableHandle,
                                         & sTableExist,
                                         aEnv )
                 == STL_SUCCESS );
        STL_ASSERT( sTableExist == STL_TRUE );
        
        /**
         * Table Integrity 검사 
         */                             

        STL_TRY( eldFinishCheckTableIntegrity( sTransID,
                                               sStmt,
                                               & sTableHandle,
                                               aEnv )
                 == STL_SUCCESS );
        STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    }
    
    /**
     * Storage Component 해제 
     */

    /* Statement */
    sState = 0;
    STL_TRY( smlFreeStatement( sStmt, SML_ENV( aEnv ) )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            (void) smlFreeStatement( sStmt, SML_ENV( aEnv ) );
        default:
            break;
    }
    
    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/**
 * @brief 하나의 Dictionary Table 의 무결성을 검사한다.
 * @param[in]  aTransID          Transaction ID
 * @param[in]  aStmt             Statement
 * @param[in]  aTableDictHandle  Table Dictionary Handle
 * @param[in]  aEnv              Environment
 * @remarks
 * 무결성 검사 대상
 * - Check Constraint
 *  - [A] Check Not Null : 검사해야 함
 *  - Check Clause   : 생성하지 않음 
 * - Key Constraint
 *  - [B] Primary Key : Unique 검사는 완료됨. Not Null 검사만 하면 됨
 *  - Unique Key  : Unique Index 생성으로 검사됨
 *  - [C] Foreign Key : 검사해야 함.
 */

stlStatus eldFinishCheckTableIntegrity( smlTransId           aTransID,
                                        smlStatement       * aStmt,
                                        ellDictHandle      * aTableDictHandle,
                                        ellEnv             * aEnv )
{
    stlInt32            i = 0;
    
    stlBool             sBeginSelect = STL_FALSE;

    void              * sTableHandle = NULL;
    knlValueBlockList * sTableValueList = NULL;

    void                * sIterator  = NULL;
    smlIteratorProperty   sIteratorProperty;

    stlInt32            sPrimaryKeyCnt = 0;
    ellDictHandle     * sPrimaryHandle = NULL;

    stlInt32            sNotNullCnt = 0;
    ellDictHandle     * sNotNullHandle = NULL;

    stlInt32             sForeignKeyCnt = 0;
    ellDictHandle      * sForeignHandle = NULL;

    ellDictHandle        * sParentKeyHandle         = NULL;
    ellDictHandle        * sParentIndexHandle       = NULL;
    knlPredicateList    ** sParentIndexRange        = NULL;
    knlExprContextInfo  ** sParentIndexRangeContext = NULL;
    knlValueBlockList   ** sParentIndexValueList    = NULL;
    stlBool                sParentKeyExist          = STL_FALSE;

    smlRid          sRowRid;
    smlScn          sRowScn;
    smlRowBlock     sRowBlock;

    ellDictHandle * sViolatedDictHandle = NULL;

    smlFetchInfo    sFetchInfo;
    
    eldMemMark      sMemMark;
    stlInt32        sState = 0;

    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableDictHandle != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Table Handle 획득
     */

    sTableHandle = ellGetTableHandle( aTableDictHandle );
    
    /**
     * Table Value List 공간 할당
     */

    STL_TRY( eldAllocTableValueList( ellGetTableID( aTableDictHandle ),
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
     * Primary Key Constraint 정보 획득
     */

    STL_TRY( ellGetTablePrimaryKeyDictHandle( aTransID,
                                              ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                              & aEnv->mMemDictOP,
                                              aTableDictHandle,
                                              & sPrimaryKeyCnt,
                                              & sPrimaryHandle,
                                              aEnv )
             == STL_SUCCESS );

    /**
     * Check Not Null Constraint 정보 획득
     */

    STL_TRY( ellGetTableCheckNotNullDictHandle( aTransID,
                                                ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                                & aEnv->mMemDictOP,
                                                aTableDictHandle,
                                                & sNotNullCnt,
                                                & sNotNullHandle,
                                                aEnv )
             == STL_SUCCESS );

    /**
     * Foreign Key Constraint 정보 획득
     */

    STL_TRY( ellGetTableForeignKeyDictHandle( aTransID,
                                              ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                              & aEnv->mMemDictOP,
                                              aTableDictHandle,
                                              & sForeignKeyCnt,
                                              & sForeignHandle,
                                              aEnv )
             == STL_SUCCESS );

    if ( sForeignKeyCnt > 0 )
    {
        /**
         * Parent Array 정보 초기화
         */
        
        STL_TRY( knlAllocRegionMem( & aEnv->mMemDictOP,
                                    STL_SIZEOF( knlPredicateList *) * sForeignKeyCnt,
                                    (void **) & sParentIndexRange,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        stlMemset( sParentIndexRange,
                   0x00,
                   STL_SIZEOF( knlPredicateList *) * sForeignKeyCnt );

        STL_TRY( knlAllocRegionMem( & aEnv->mMemDictOP,
                                    STL_SIZEOF( knlExprContextInfo *) * sForeignKeyCnt,
                                    (void **) & sParentIndexRangeContext,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        stlMemset( sParentIndexRangeContext,
                   0x00,
                   STL_SIZEOF( knlExprContextInfo *) * sForeignKeyCnt );

        STL_TRY( knlAllocRegionMem( & aEnv->mMemDictOP,
                                    STL_SIZEOF( knlValueBlockList *) * sForeignKeyCnt,
                                    (void **) & sParentIndexValueList,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        stlMemset( sParentIndexValueList,
                   0x00,
                   STL_SIZEOF( knlValueBlockList *) * sForeignKeyCnt );
        
        for ( i = 0; i < sForeignKeyCnt; i++ )
        {
            /**
             * Parent Index 의 Key Range 생성
             */

            STL_TRY( eldMakeParentRange( aTableDictHandle,
                                         & sForeignHandle[i],
                                         sTableValueList,
                                         & aEnv->mMemDictOP,
                                         & sParentIndexRange[i],
                                         & sParentIndexRangeContext[i],
                                         & sParentIndexValueList[i],
                                         aEnv )
                     == STL_SUCCESS );
        }
    }
    else
    {
        sParentIndexRange = NULL;
        sParentIndexRangeContext = NULL;
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
     * 각 Row 에 대한 무결성을 검사한다.
     */

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

        /**
         * Primary Key 의 Not Null 검사
         */

        for ( i = 0; i < sPrimaryKeyCnt; i++ )
        {
            STL_TRY( eldCheckRowPrimaryNotNull( aTableDictHandle,
                                                & sPrimaryHandle[i],
                                                sTableValueList,
                                                aEnv )
                     == STL_SUCCESS );
        }
                                         
        /**
         * Check Not Null 검사
         */

        for ( i = 0; i < sNotNullCnt; i++ )
        {
            STL_TRY( eldCheckRowCheckNotNull( aTableDictHandle,
                                              & sNotNullHandle[i],
                                              sTableValueList,
                                              aEnv )
                     == STL_SUCCESS );
        }
        
        /**
         * Foreign Key 검사
         */

        for ( i = 0; i < sForeignKeyCnt; i++ )
        {
            /**
             * Parent Key 에 Key 가 존재하는 지 검사
             */
            
            sParentKeyHandle = ellGetForeignKeyParentUniqueKeyHandle( & sForeignHandle[i] );
            sParentIndexHandle = ellGetConstraintIndexDictHandle( sParentKeyHandle );
            
            /* for debugging */
            sParentKeyExist = STL_TRUE;
            
            STL_TRY( eldCheckIndexKeyExist( aTransID,
                                            aStmt,
                                            ellGetTableID( aTableDictHandle ),
                                            sParentIndexHandle,
                                            sParentIndexRange[i],
                                            sParentIndexRangeContext[i],
                                            sParentIndexValueList[i],
                                            & sParentKeyExist,
                                            aEnv )
                     == STL_SUCCESS );

            /**
             * Key 가 존재해야 함.
             */
            
            if ( sParentKeyExist == STL_TRUE )
            {
                /* OK */
            }
            else
            {
                sViolatedDictHandle = & sForeignHandle[i];

#ifdef STL_DEBUG
                knlSystemFatal( "eldFinishCheckTableIntegrity()", KNL_ENV(aEnv) );
#endif
                
                STL_THROW( RAMP_ERROR_CHECK_FOREIGN_KEY );
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

    STL_CATCH( RAMP_ERROR_CHECK_FOREIGN_KEY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ELL_ERRCODE_INTEGRITY_VIOLATION,
                      NULL,
                      ELL_ERROR_STACK(aEnv),
                      ellGetConstraintName(sViolatedDictHandle) );
    }
    
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
 * @brief Dictionary Table 에 Row 추가시 인덱스 키 추가 및 무결성을 보장한다.
 * @param[in]  aTransID          Transaction ID
 * @param[in]  aStmt             Statement
 * @param[in]  aDictTableID      Dictionary Table 의 ID
 * @param[in]  aRowBlock         SM Row Block
 * @param[in]  aInsertValueList  Insert Row 의 Value List
 * @param[in]  aEnv              Environment
 */
stlStatus eldRefineIntegrityRowInsert( smlTransId              aTransID,
                                       smlStatement          * aStmt,
                                       eldtDefinitionTableID   aDictTableID,
                                       smlRowBlock           * aRowBlock,
                                       knlValueBlockList     * aInsertValueList,
                                       ellEnv                * aEnv )
{
    stlInt32 i = 0;
    
    stlBool              sObjectExist = STL_FALSE;
    ellDictHandle        sTableHandle;
    stlInt32             sViolatedCnt = 0;

    stlInt32             sNotNullCnt = 0;
    ellDictHandle      * sNotNullHandle = NULL;
    
    stlInt32             sPrimaryKeyCnt = 0;
    ellDictHandle      * sPrimaryHandle = NULL;

    stlInt32             sUniqueKeyCnt = 0;
    ellDictHandle      * sUniqueHandle = NULL;

    stlInt32             sUniqueIndexCnt = 0;
    ellDictHandle      * sUniqueIndexHandle = NULL;
    
    stlInt32             sNonUniqueIndexCnt = 0;
    ellDictHandle      * sNonUniqueIndexHandle = NULL;
    
    stlInt32             sForeignKeyCnt = 0;
    ellDictHandle      * sForeignHandle = NULL;
    
    ellDictHandle      * sParentKeyHandle         = NULL;
    ellDictHandle      * sParentIndexHandle       = NULL;
    knlPredicateList   * sParentIndexRange        = NULL;
    knlExprContextInfo * sParentIndexRangeContext = NULL;
    knlValueBlockList  * sParentIndexValueList    = NULL;
    stlBool              sParentKeyExist          = STL_FALSE;

    ellDictHandle     * sIndexDictHandle = NULL;
    knlValueBlockList * sIndexValueList = NULL;

    ellDictHandle     * sViolatedDictHandle = NULL;

    smlUniqueViolationScope sUniqueViolationScope;

    ellInitDictHandle( & sTableHandle );
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aDictTableID > ELDT_TABLE_ID_NA) &&
                        (aDictTableID < ELDT_TABLE_ID_MAX),
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRowBlock != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInsertValueList != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * 테이블 정보 획득
     */

    STL_TRY( eldcGetTableHandleByID( aTransID,
                                     ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                     aDictTableID,
                                     & sTableHandle,
                                     & sObjectExist,
                                     aEnv )
             == STL_SUCCESS );
    STL_ASSERT( sObjectExist == STL_TRUE );

    /**
     * Not Null 무결성 검사  
     */

    STL_TRY( ellGetTableCheckNotNullDictHandle( aTransID,
                                                ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                                & aEnv->mMemDictOP,
                                                & sTableHandle,
                                                & sNotNullCnt,
                                                & sNotNullHandle,
                                                aEnv )
             == STL_SUCCESS );

    for ( i = 0; i < sNotNullCnt; i++ )
    {
        STL_TRY( eldCheckRowCheckNotNull( & sTableHandle,
                                          & sNotNullHandle[i],
                                          aInsertValueList,
                                          aEnv )
                 == STL_SUCCESS );
    }
                                                
    /**
     * Primary Key 무결성 보장 
     */

    STL_TRY( ellGetTablePrimaryKeyDictHandle( aTransID,
                                              ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                              & aEnv->mMemDictOP,
                                              & sTableHandle,
                                              & sPrimaryKeyCnt,
                                              & sPrimaryHandle,
                                              aEnv )
             == STL_SUCCESS );

    for ( i = 0; i < sPrimaryKeyCnt; i++ )
    {
        /**
         * Primary Key 의 Not Null 검사
         */

        STL_TRY( eldCheckRowPrimaryNotNull( & sTableHandle,
                                            & sPrimaryHandle[i],
                                            aInsertValueList,
                                            aEnv )
                 == STL_SUCCESS );
        
        /**
         * Primary Key 의 Unique 검사
         */

        sIndexDictHandle = ellGetConstraintIndexDictHandle( & sPrimaryHandle[i] );

        STL_TRY( ellShareIndexValueListForINSERT( & sTableHandle,
                                                  aInsertValueList,
                                                  sIndexDictHandle,
                                                  & aEnv->mMemDictOP,
                                                  & sIndexValueList,
                                                  aEnv )
                 == STL_SUCCESS );

        STL_TRY( smlInsertKeys( aStmt,
                                ellGetIndexHandle( sIndexDictHandle ),
                                sIndexValueList,
                                aRowBlock,
                                NULL,      /* aBlockFilter */
                                STL_TRUE,  /* aIsDeferred */
                                & sViolatedCnt,
                                & sUniqueViolationScope,
                                SML_ENV(aEnv) )
                 == STL_SUCCESS );
        
        if ( sUniqueViolationScope != SML_UNIQUE_VIOLATION_SCOPE_NONE )
        {
            sViolatedDictHandle = & sPrimaryHandle[i];

            STL_THROW( RAMP_ERROR_UNIQUE_VIOLATION );
        }
        else
        {
            /* 무결성 검증됨 */
        }
    }
                                              
    /**
     * Unique Key 무결성 보장 
     */

    STL_TRY( ellGetTableUniqueKeyDictHandle( aTransID,
                                             ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                             & aEnv->mMemDictOP,
                                             & sTableHandle,
                                             & sUniqueKeyCnt,
                                             & sUniqueHandle,
                                             aEnv )
             == STL_SUCCESS );

    for ( i = 0; i < sUniqueKeyCnt; i++ )
    {
        /**
         * Unique Key 의 Unique 검사
         */

        sIndexDictHandle = ellGetConstraintIndexDictHandle( & sUniqueHandle[i] );

        STL_TRY( ellShareIndexValueListForINSERT( & sTableHandle,
                                                  aInsertValueList,
                                                  sIndexDictHandle,
                                                  & aEnv->mMemDictOP,
                                                  & sIndexValueList,
                                                  aEnv )
                 == STL_SUCCESS );
        
        STL_TRY( smlInsertKeys( aStmt,
                                ellGetIndexHandle( sIndexDictHandle ),
                                sIndexValueList,
                                aRowBlock,
                                NULL,      /* aBlockFilter */
                                STL_TRUE,  /* aIsDeferred */
                                & sViolatedCnt,
                                & sUniqueViolationScope,
                                SML_ENV(aEnv) )
                 == STL_SUCCESS );
        
        if ( sUniqueViolationScope != SML_UNIQUE_VIOLATION_SCOPE_NONE )
        {
            sViolatedDictHandle = & sUniqueHandle[i];

            STL_THROW( RAMP_ERROR_UNIQUE_VIOLATION );
        }
        else
        {
            /* 무결성 검증됨 */
        }
    }
    
    /**
     * Foreign Key 무결성 보장 
     */

    STL_TRY( ellGetTableForeignKeyDictHandle( aTransID,
                                              ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                              & aEnv->mMemDictOP,
                                              & sTableHandle,
                                              & sForeignKeyCnt,
                                              & sForeignHandle,
                                              aEnv )
             == STL_SUCCESS );

    for ( i = 0; i < sForeignKeyCnt; i++ )
    {
        /**
         * Parent Index 의 Key Range 생성
         */
        
        STL_TRY( eldMakeParentRange( & sTableHandle,
                                     & sForeignHandle[i],
                                     aInsertValueList,
                                     & aEnv->mMemDictOP,
                                     & sParentIndexRange,
                                     & sParentIndexRangeContext,
                                     & sParentIndexValueList,
                                     aEnv )
                 == STL_SUCCESS );
        
        /**
         * Foreign Key Constraint 검사 
         * Parent Key 에 Key 가 존재하는 지 검사
         */

        sParentKeyHandle = ellGetForeignKeyParentUniqueKeyHandle( & sForeignHandle[i] );
        sParentIndexHandle = ellGetConstraintIndexDictHandle( sParentKeyHandle );
        
        /* for debugging */
        sParentKeyExist = STL_TRUE;
        
        STL_TRY( eldCheckIndexKeyExist( aTransID,
                                        aStmt,
                                        ellGetConstraintTableID( sParentKeyHandle ),
                                        sParentIndexHandle,
                                        sParentIndexRange,
                                        sParentIndexRangeContext,
                                        sParentIndexValueList,
                                        & sParentKeyExist,
                                        aEnv )
                 == STL_SUCCESS );
        
        /**
         * Key 가 존재해야 함.
         */

        if ( sParentKeyExist == STL_TRUE )
        {
            /* OK */
        }
        else
        {
            sViolatedDictHandle = & sForeignHandle[i];

            /**
             * Dictionary 의 Foreign Key 무결성이 유지되어야 함
             */

#ifdef STL_DEBUG
            knlSystemFatal( "eldRefineIntegrityRowInsert()", KNL_ENV(aEnv) );
#endif
            
            STL_THROW( RAMP_ERROR_CHECK_FOREIGN_KEY );
        }
        
        /**
         * Foreign Key Index 에 Index Key 추가 
         */

        sIndexDictHandle = ellGetConstraintIndexDictHandle( & sForeignHandle[i] );

        STL_TRY( ellShareIndexValueListForINSERT( & sTableHandle,
                                                  aInsertValueList,
                                                  sIndexDictHandle,
                                                  & aEnv->mMemDictOP,
                                                  & sIndexValueList,
                                                  aEnv )
                 == STL_SUCCESS );

        STL_TRY( smlInsertKeys( aStmt,
                                ellGetIndexHandle( sIndexDictHandle ),
                                sIndexValueList,
                                aRowBlock,
                                NULL,      /* aBlockFilter */
                                STL_TRUE,  /* aIsDeferred */
                                & sViolatedCnt,
                                & sUniqueViolationScope,
                                SML_ENV(aEnv) )
                 == STL_SUCCESS );

        STL_ASSERT( sUniqueViolationScope == SML_UNIQUE_VIOLATION_SCOPE_NONE );
    }

    /**
     * Unique Index 무결성 보장 
     */

    STL_TRY( ellGetTableUniqueIndexDictHandle( aTransID,
                                               ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                               & aEnv->mMemDictOP,
                                               & sTableHandle,
                                               & sUniqueIndexCnt,
                                               & sUniqueIndexHandle,
                                               aEnv )
             == STL_SUCCESS );

    for ( i = 0; i < sUniqueIndexCnt; i++ )
    {
        /**
         * Unique Index 의 Unique 검사
         */

        sIndexDictHandle = & sUniqueIndexHandle[i];

        STL_TRY( ellShareIndexValueListForINSERT( & sTableHandle,
                                                  aInsertValueList,
                                                  sIndexDictHandle,
                                                  & aEnv->mMemDictOP,
                                                  & sIndexValueList,
                                                  aEnv )
                 == STL_SUCCESS );
        
        STL_TRY( smlInsertKeys( aStmt,
                                ellGetIndexHandle( sIndexDictHandle ),
                                sIndexValueList,
                                aRowBlock,
                                NULL,      /* aBlockFilter */
                                STL_TRUE,  /* aIsDeferred */
                                & sViolatedCnt,
                                & sUniqueViolationScope,
                                SML_ENV(aEnv) )
                 == STL_SUCCESS );

        if ( sUniqueViolationScope != SML_UNIQUE_VIOLATION_SCOPE_NONE )
        {
            sViolatedDictHandle = & sUniqueIndexHandle[i];

            STL_THROW( RAMP_ERROR_UNIQUE_INDEX_VIOLATION );
        }
        else
        {
            /* 무결성 검증됨 */
        }
    }
    
    /**
     * Non-Unique Index 키 추가 
     */

    STL_TRY( ellGetTableNonUniqueIndexDictHandle( aTransID,
                                                  ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                                  & aEnv->mMemDictOP,
                                                  & sTableHandle,
                                                  & sNonUniqueIndexCnt,
                                                  & sNonUniqueIndexHandle,
                                                  aEnv )
             == STL_SUCCESS );

    for ( i = 0; i < sNonUniqueIndexCnt; i++ )
    {
        /**
         * Non-Unique Index 에 Key 추가
         */
        
        sIndexDictHandle = & sNonUniqueIndexHandle[i];

        STL_TRY( ellShareIndexValueListForINSERT( & sTableHandle,
                                                  aInsertValueList,
                                                  sIndexDictHandle,
                                                  & aEnv->mMemDictOP,
                                                  & sIndexValueList,
                                                  aEnv )
                 == STL_SUCCESS );
        
        STL_TRY( smlInsertKeys( aStmt,
                                ellGetIndexHandle( sIndexDictHandle ),
                                sIndexValueList,
                                aRowBlock,
                                NULL,      /* aBlockFilter */
                                STL_TRUE,  /* aIsDeferred */
                                & sViolatedCnt,
                                & sUniqueViolationScope,
                                SML_ENV(aEnv) )
                 == STL_SUCCESS );

        STL_ASSERT( sUniqueViolationScope == SML_UNIQUE_VIOLATION_SCOPE_NONE );
    }
                                     
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERROR_UNIQUE_VIOLATION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ELL_ERRCODE_INTEGRITY_VIOLATION,
                      NULL,
                      ELL_ERROR_STACK(aEnv),
                      ellGetConstraintName(sViolatedDictHandle) );
    }

    STL_CATCH( RAMP_ERROR_CHECK_FOREIGN_KEY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ELL_ERRCODE_INTEGRITY_VIOLATION,
                      NULL,
                      ELL_ERROR_STACK(aEnv),
                      ellGetConstraintName(sViolatedDictHandle) );
    }

    STL_CATCH( RAMP_ERROR_UNIQUE_INDEX_VIOLATION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ELL_ERRCODE_INTEGRITY_VIOLATION,
                      NULL,
                      ELL_ERROR_STACK(aEnv),
                      ellGetIndexName(sViolatedDictHandle) );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief Dictionary Table 에서 Row 삭제 시 인덱스 키 제거 및 무결성을 보장한다.
 * @param[in]  aTransID          Transaction ID
 * @param[in]  aStmt             Statement
 * @param[in]  aForUpdate        Dictionary UPDATE 를 위한 삭제인지의 여부 
 * @param[in]  aDictTableID      Dictionary Table 의 ID
 * @param[in]  aRowBlock         SM Row Block
 * @param[in]  aDeleteValueList  Delete Row 의 Value List
 * @param[in]  aEnv              Environment
 */
stlStatus eldRefineIntegrityRowDelete( smlTransId              aTransID,
                                       smlStatement          * aStmt,
                                       stlBool                 aForUpdate,
                                       eldtDefinitionTableID   aDictTableID,
                                       smlRowBlock           * aRowBlock,
                                       knlValueBlockList     * aDeleteValueList,
                                       ellEnv                * aEnv )
{
    stlInt32 i = 0;
    
    stlBool              sObjectExist = STL_FALSE;

    ellDictHandle        sTableHandle;
    stlInt32             sCollisionCnt = 0;

    stlInt32             sPrimaryKeyCnt = 0;
    ellDictHandle      * sPrimaryHandle = NULL;

    stlInt32             sUniqueKeyCnt = 0;
    ellDictHandle      * sUniqueHandle = NULL;

    stlInt32             sUniqueIndexCnt = 0;
    ellDictHandle      * sUniqueIndexHandle = NULL;
    
    stlInt32             sNonUniqueIndexCnt = 0;
    ellDictHandle      * sNonUniqueIndexHandle = NULL;
    
    stlInt32             sForeignKeyCnt = 0;
    ellDictHandle      * sForeignHandle = NULL;

    stlInt32             sChildForeignKeyCnt = 0;
    ellDictHandle      * sChildForeignKeyHandle = NULL;

    ellDictHandle        sChildTableHandle;
    
    ellDictHandle      * sChildIndexHandle       = NULL;
    knlPredicateList   * sChildIndexRange        = NULL;
    knlExprContextInfo * sChildIndexRangeContext = NULL;
    knlValueBlockList  * sChildIndexValueList    = NULL;
    stlBool              sChildForeignKeyExist      = STL_FALSE;

    ellDictHandle      * sIndexDictHandle = NULL;
    knlValueBlockList  * sIndexValueList = NULL;

    ellDictHandle     * sViolatedDictHandle = NULL;

    smlUniqueViolationScope sUniqueViolationScope;
    
    ellInitDictHandle( & sTableHandle );
    ellInitDictHandle( & sChildTableHandle );
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aDictTableID > ELDT_TABLE_ID_NA) &&
                        (aDictTableID < ELDT_TABLE_ID_MAX),
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRowBlock != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDeleteValueList != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * 테이블 정보 획득
     */

    STL_TRY( eldcGetTableHandleByID( aTransID,
                                     ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                     aDictTableID,
                                     & sTableHandle,
                                     & sObjectExist,
                                     aEnv )
             == STL_SUCCESS );
    STL_ASSERT( sObjectExist == STL_TRUE );

    /**
     * Primary Key 무결성 보장 
     */

    STL_TRY( ellGetTablePrimaryKeyDictHandle( aTransID,
                                              ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                              & aEnv->mMemDictOP,
                                              & sTableHandle,
                                              & sPrimaryKeyCnt,
                                              & sPrimaryHandle,
                                              aEnv )
             == STL_SUCCESS );

    for ( i = 0; i < sPrimaryKeyCnt; i++ )
    {
        /**
         * Primary Key Index 의 Key 삭제
         */

        sIndexDictHandle = ellGetConstraintIndexDictHandle( & sPrimaryHandle[i] );

        STL_TRY( ellShareIndexValueListForDELETE( & sTableHandle,
                                                  aDeleteValueList,
                                                  sIndexDictHandle,
                                                  & aEnv->mMemDictOP,
                                                  & sIndexValueList,
                                                  aEnv )
                 == STL_SUCCESS );
        
        STL_TRY( smlDeleteKeys( aStmt,
                                ellGetIndexHandle( sIndexDictHandle ),
                                sIndexValueList,
                                aRowBlock,
                                NULL,   /* aBlockFilter */
                                & sCollisionCnt,
                                & sUniqueViolationScope,
                                SML_ENV(aEnv) )
                 == STL_SUCCESS );

        STL_ASSERT( sUniqueViolationScope != SML_UNIQUE_VIOLATION_SCOPE_RESOLVED );
    }
                                              
    /**
     * Unique Key 무결성 보장 
     */

    STL_TRY( ellGetTableUniqueKeyDictHandle( aTransID,
                                             ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                             & aEnv->mMemDictOP,
                                             & sTableHandle,
                                             & sUniqueKeyCnt,
                                             & sUniqueHandle,
                                             aEnv )
             == STL_SUCCESS );

    for ( i = 0; i < sUniqueKeyCnt; i++ )
    {
        /**
         * Unique Key Index 의 Key 삭제 
         */

        sIndexDictHandle = ellGetConstraintIndexDictHandle( & sUniqueHandle[i] );

        STL_TRY( ellShareIndexValueListForDELETE( & sTableHandle,
                                                  aDeleteValueList,
                                                  sIndexDictHandle,
                                                  & aEnv->mMemDictOP,
                                                  & sIndexValueList,
                                                  aEnv )
                 == STL_SUCCESS );
        
        STL_TRY( smlDeleteKeys( aStmt,
                                ellGetIndexHandle( sIndexDictHandle ),
                                sIndexValueList,
                                aRowBlock,
                                NULL,   /* aBlockFilter */
                                & sCollisionCnt,
                                & sUniqueViolationScope,
                                SML_ENV(aEnv) )
                 == STL_SUCCESS );

        STL_ASSERT( sUniqueViolationScope != SML_UNIQUE_VIOLATION_SCOPE_RESOLVED );
    }
                                              
    
    /**
     * Foreign Key 무결성 보장 
     */

    STL_TRY( ellGetTableForeignKeyDictHandle( aTransID,
                                              ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                              & aEnv->mMemDictOP,
                                              & sTableHandle,
                                              & sForeignKeyCnt,
                                              & sForeignHandle,
                                              aEnv )
             == STL_SUCCESS );

    for ( i = 0; i < sForeignKeyCnt; i++ )
    {
        /**
         * Foreign Key Index 의 Key 삭제 
         */

        sIndexDictHandle = ellGetConstraintIndexDictHandle( & sForeignHandle[i] );

        STL_TRY( ellShareIndexValueListForDELETE( & sTableHandle,
                                                  aDeleteValueList,
                                                  sIndexDictHandle,
                                                  & aEnv->mMemDictOP,
                                                  & sIndexValueList,
                                                  aEnv )
                 == STL_SUCCESS );
        
        STL_TRY( smlDeleteKeys( aStmt,
                                ellGetIndexHandle( sIndexDictHandle ),
                                sIndexValueList,
                                aRowBlock,
                                NULL,   /* aBlockFilter */
                                & sCollisionCnt,
                                & sUniqueViolationScope,
                                SML_ENV(aEnv) )
                 == STL_SUCCESS );

        STL_ASSERT( sUniqueViolationScope != SML_UNIQUE_VIOLATION_SCOPE_RESOLVED );
    }

    /**
     * Unique Index 무결성 보장 
     */

    STL_TRY( ellGetTableUniqueIndexDictHandle( aTransID,
                                               ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                               & aEnv->mMemDictOP,
                                               & sTableHandle,
                                               & sUniqueIndexCnt,
                                               & sUniqueIndexHandle,
                                               aEnv )
             == STL_SUCCESS );

    for ( i = 0; i < sUniqueIndexCnt; i++ )
    {
        /**
         * Unique Index 의 Key 삭제 
         */

        sIndexDictHandle = & sUniqueIndexHandle[i];

        STL_TRY( ellShareIndexValueListForDELETE( & sTableHandle,
                                                  aDeleteValueList,
                                                  sIndexDictHandle,
                                                  & aEnv->mMemDictOP,
                                                  & sIndexValueList,
                                                  aEnv )
                 == STL_SUCCESS );
        
        STL_TRY( smlDeleteKeys( aStmt,
                                ellGetIndexHandle( sIndexDictHandle ),
                                sIndexValueList,
                                aRowBlock,
                                NULL,   /* aBlockFilter */
                                & sCollisionCnt,
                                & sUniqueViolationScope,
                                SML_ENV(aEnv) )
                 == STL_SUCCESS );

        STL_ASSERT( sUniqueViolationScope != SML_UNIQUE_VIOLATION_SCOPE_RESOLVED );
    }
    
    /**
     * Non-Unique Index 키 삭제  
     */

    STL_TRY( ellGetTableNonUniqueIndexDictHandle( aTransID,
                                                  ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                                  & aEnv->mMemDictOP,
                                                  & sTableHandle,
                                                  & sNonUniqueIndexCnt,
                                                  & sNonUniqueIndexHandle,
                                                  aEnv )
             == STL_SUCCESS );

    for ( i = 0; i < sNonUniqueIndexCnt; i++ )
    {
        /**
         * Non-Unique Index 의 Key 삭제 
         */
        
        sIndexDictHandle = & sNonUniqueIndexHandle[i];

        STL_TRY( ellShareIndexValueListForDELETE( & sTableHandle,
                                                  aDeleteValueList,
                                                  sIndexDictHandle,
                                                  & aEnv->mMemDictOP,
                                                  & sIndexValueList,
                                                  aEnv )
                 == STL_SUCCESS );
        
        STL_TRY( smlDeleteKeys( aStmt,
                                ellGetIndexHandle( sIndexDictHandle ),
                                sIndexValueList,
                                aRowBlock,
                                NULL,   /* aBlockFilter */
                                & sCollisionCnt,
                                & sUniqueViolationScope,
                                SML_ENV(aEnv) )
                 == STL_SUCCESS );

        STL_ASSERT( sUniqueViolationScope != SML_UNIQUE_VIOLATION_SCOPE_RESOLVED );
    }

    /**
     * Child Foreign Key 의 무결성 검증
     */

    if ( aForUpdate == STL_TRUE )
    {
        /**
         * nothing to do
         * Dictionary 를 UPDATE 를 위한 삭제임
         */
    }
    else
    {
        STL_TRY( ellGetTableChildForeignKeyDictHandle( aTransID,
                                                       ELL_DICT_CACHE_VIEW_SCN( aStmt ),
                                                       & aEnv->mMemDictOP,
                                                       & sTableHandle,
                                                       & sChildForeignKeyCnt,
                                                       & sChildForeignKeyHandle,
                                                       aEnv )
                 == STL_SUCCESS );
                                                
        for ( i = 0; i < sChildForeignKeyCnt; i++ )
        {
            /**
             * Child Index 의 Key Range 생성
             */
        
            STL_TRY( eldMakeChildRange( & sTableHandle,
                                        & sChildForeignKeyHandle[i],
                                        aDeleteValueList,
                                        & aEnv->mMemDictOP,
                                        & sChildIndexRange,
                                        & sChildIndexRangeContext,
                                        & sChildIndexValueList,
                                        aEnv )
                     == STL_SUCCESS );
        
            /**
             * Child Index 에 Key 가 존재하는 지 검사
             */
        
            sChildIndexHandle =
                ellGetConstraintIndexDictHandle( & sChildForeignKeyHandle[i] );

            /* for debugging */
            sChildForeignKeyExist = STL_FALSE;
            
            STL_TRY( eldCheckIndexKeyExist( aTransID,
                                            aStmt,
                                            ellGetConstraintTableID( & sChildForeignKeyHandle[i] ),
                                            sChildIndexHandle,
                                            sChildIndexRange,
                                            sChildIndexRangeContext,
                                            sChildIndexValueList,
                                            & sChildForeignKeyExist,
                                            aEnv )
                     == STL_SUCCESS );
        
            /**
             * Key 가 존재하지 않아야 함.
             */
        
            if ( sChildForeignKeyExist == STL_TRUE )
            {
                sViolatedDictHandle = & sChildForeignKeyHandle[i];

                /**
                 * Dictionary 의 Foreign Key 무결성이 유지되어야 함
                 */

#ifdef STL_DEBUG
                knlSystemFatal( "eldRefineIntegrityRowDelete()", KNL_ENV(aEnv) );
#endif
                
                STL_THROW( RAMP_ERROR_CHECK_REFERENCE_KEY );
            }
            else
            {
                /* OK */
            }
        }
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERROR_CHECK_REFERENCE_KEY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ELL_ERRCODE_INTEGRITY_VIOLATION,
                      NULL,
                      ELL_ERROR_STACK(aEnv),
                      ellGetConstraintName(sViolatedDictHandle) );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}




/**
 * @brief Primary Key 제약조건의 Not Null 조건을 만족하는 지 검사한다.
 * @param[in]  aTableDictHandle  Table Dictionary Handle
 * @param[in]  aPrimaryKeyHandle Primary Key Dictionary Handle
 * @param[in]  aTableValueList   Table Value list
 * @param[in]  aEnv              Environment
 */
stlStatus eldCheckRowPrimaryNotNull( ellDictHandle     * aTableDictHandle,
                                     ellDictHandle     * aPrimaryKeyHandle,
                                     knlValueBlockList * aTableValueList,
                                     ellEnv            * aEnv )
{
    stlInt32        i = 0;
    
    stlInt32        sKeyColumnCnt = 0;
    ellDictHandle * sKeyColumnHandle = NULL;

    knlValueBlockList * sColumnValue = NULL;
    dtlDataValue      * sDataValue   = NULL;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aTableDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPrimaryKeyHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableValueList != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Primary Key 정보 획득
     */

    sKeyColumnCnt    = ellGetPrimaryKeyColumnCount(aPrimaryKeyHandle);
    sKeyColumnHandle = ellGetPrimaryKeyColumnHandle(aPrimaryKeyHandle);

    /**
     * Key Column 의 Not Null 검사
     */
    
    for ( i = 0; i < sKeyColumnCnt; i++ )
    {
        sColumnValue = ellFindColumnValueList( aTableValueList,
                                               ellGetColumnTableID( &sKeyColumnHandle[i] ),
                                               ellGetColumnIdx( &sKeyColumnHandle[i] ) );
        sDataValue = KNL_GET_BLOCK_DATA_VALUE( sColumnValue, 0 );

        STL_TRY_THROW( DTL_IS_NULL( sDataValue ) != STL_TRUE, RAMP_ERROR_PRIMARY_NOT_NULL );
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERROR_PRIMARY_NOT_NULL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ELL_ERRCODE_INTEGRITY_VIOLATION,
                      NULL,
                      ELL_ERROR_STACK(aEnv),
                      ellGetConstraintName(aPrimaryKeyHandle) );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Check Not Null 제약조건의 Not Null 조건을 만족하는 지 검사한다.
 * @param[in]  aTableDictHandle  Table Dictionary Handle
 * @param[in]  aNotNullHandle    Check Not Null Dictionary Handle
 * @param[in]  aTableValueList   Table Value list
 * @param[in]  aEnv              Environment
 */
stlStatus eldCheckRowCheckNotNull( ellDictHandle     * aTableDictHandle,
                                   ellDictHandle     * aNotNullHandle,
                                   knlValueBlockList * aTableValueList,
                                   ellEnv            * aEnv )
{
    ellDictHandle * sColumnHandle = NULL;

    knlValueBlockList * sColumnValue = NULL;
    dtlDataValue      * sDataValue   = NULL;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aTableDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNotNullHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableValueList != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Not Null 컬럼 정보 획득
     */

    sColumnHandle = ellGetCheckNotNullColumnHandle(aNotNullHandle);

    /**
     * Column 의 Not Null 검사
     */
    
    sColumnValue = ellFindColumnValueList( aTableValueList,
                                           ellGetColumnTableID( sColumnHandle ),
                                           ellGetColumnIdx( sColumnHandle ) );
    sDataValue = KNL_GET_BLOCK_DATA_VALUE( sColumnValue, 0 );

    STL_TRY_THROW( DTL_IS_NULL( sDataValue ) != STL_TRUE, RAMP_ERROR_CHECK_NOT_NULL );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERROR_CHECK_NOT_NULL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ELL_ERRCODE_INTEGRITY_VIOLATION,
                      NULL,
                      ELL_ERROR_STACK(aEnv),
                      ellGetConstraintName(aNotNullHandle) );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief Foreign Key 검사를 위해 Index 에 Key 가 존재하는 지 검사한다.
 * @param[in]  aTransID            Transaction ID
 * @param[in]  aStmt               Statement
 * @param[in]  aDictTableID        Dictionary Table ID
 * @param[in]  aIndexDictHandle    Index Dictionary Handle
 * @param[in]  aIndexRange         Index Key Range
 * @param[in]  aIndexRangeContext  Index Key Range Context
 * @param[in]  aIndexValueList     Index Value List
 * @param[out] aKeyExist           Key 존재 여부 
 * @param[in]  aEnv                Environment
 */
stlStatus eldCheckIndexKeyExist( smlTransId              aTransID,
                                 smlStatement          * aStmt,
                                 eldtDefinitionTableID   aDictTableID,
                                 ellDictHandle         * aIndexDictHandle,
                                 knlPredicateList      * aIndexRange,
                                 knlExprContextInfo    * aIndexRangeContext,
                                 knlValueBlockList     * aIndexValueList,
                                 stlBool               * aKeyExist,
                                 ellEnv                * aEnv )
{
    stlBool             sBeginSelect = STL_FALSE;

    void              * sIterator  = NULL;
    smlIteratorProperty sIteratorProperty;

    smlRid               sRowRid;
    smlScn               sRowScn;
    smlRowBlock          sRowBlock;

    void   * sTableHandle = NULL;
    void   * sIndexHandle = NULL;

    knlKeyCompareList  * sKeyCompList = NULL;
    
    smlFetchInfo        sFetchInfo;
    smlFetchRecordInfo  sFetchRecordInfo;

    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aDictTableID > ELDT_TABLE_ID_NA) &&
                        (aDictTableID < ELDT_TABLE_ID_MAX),
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIndexDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIndexRange != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIndexValueList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aKeyExist != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * - Row Block 초기화
     */
    
    STL_TRY( smlInitSingleRowBlock( &sRowBlock,
                                    &sRowRid,
                                    &sRowScn,
                                    SML_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * Index Dictionary Handle 로부터 Table Handle 과 Index Handle 을 획득 
     */

    sTableHandle = gEldTablePhysicalHandle[aDictTableID];
    sIndexHandle = ellGetIndexHandle( aIndexDictHandle );

    /**
     * Index Key Compare List 획득
     */

    STL_TRY( ellGetKeyCompareList( aIndexDictHandle,
                                   & aEnv->mMemDictOP,
                                   & sKeyCompList,
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
                                          NULL,
                                          aIndexValueList,
                                          aIndexRange,
                                          aIndexRangeContext,
                                          & sFetchInfo,
                                          & sFetchRecordInfo,
                                          & sIterator,
                                          aEnv )                    
             == STL_SUCCESS );

    sBeginSelect = STL_TRUE;

    /**
     * Key Range 에 부합하는 레코드가 존재하는지 검사 
     */

#ifdef STL_DEBUG
    if ( *aKeyExist == STL_FALSE )
    {
        /* Data 가 존재할 경우 Index Fetch 시 비정상 종료할 수 있도록 함 */
        sFetchInfo.mRowBlock->mRidBlock = NULL;
        sFetchInfo.mRowBlock->mScnBlock = NULL;
    }
#endif
    
    sFetchInfo.mIsEndOfScan = STL_TRUE;
    STL_TRY( eldFetchNext( sIterator,
                           & sFetchInfo, 
                           aEnv )
             == STL_SUCCESS );
    
    /**
     * SELECT 종료
     */

    sBeginSelect = STL_FALSE;
    STL_TRY( eldEndForQuery( sIterator,
                             aEnv )
             == STL_SUCCESS );

    /**
     * Output 설정
     */

    if( SML_USED_BLOCK_SIZE( sFetchInfo.mRowBlock ) == 0 )
    {
        STL_DASSERT( sFetchInfo.mIsEndOfScan == STL_TRUE );
        *aKeyExist = STL_FALSE;
    }
    else
    {
        *aKeyExist = STL_TRUE;
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    if ( sBeginSelect == STL_TRUE )
    {
        (void) eldEndForQuery( sIterator,
                               aEnv );
    }
    
    return STL_FAILURE;
}

/**
 * @brief Foreign Key 검사를 위해 Parent Key 에 대한 Key Range 를 생성한다.
 * @param[in]  aChildTableHandle     Child Table 의 Dictionary Handle
 * @param[in]  aChildForeignHandle   Child Table 의 Foreign Key Handle
 * @param[in]  aChildTableValueList  Child Table 의 Table Value List 공간
 * @param[in]  aRegionMem            Region Memory
 * @param[out] aParentKeyRange       Parent Index 에 대한 Key Range
 * @param[out] aParentContextInfo    Parent Index 에 대한 Key Range Context 정보
 * @param[out] aParentIndexValueList Parent Index 의 Value List 공간 
 * @param[in]  aEnv                  Environment
 * @remarks
 * Child Table 의 Row 추가 시 Parent Table 에 대한 Foreign Key 검사를 위해
 * Parent Key(Unique or Primary) Index 의 Key Range 를 생성한다.
 */
stlStatus eldMakeParentRange( ellDictHandle        * aChildTableHandle,
                              ellDictHandle        * aChildForeignHandle,
                              knlValueBlockList    * aChildTableValueList,
                              knlRegionMem         * aRegionMem,
                              knlPredicateList    ** aParentKeyRange,
                              knlExprContextInfo  ** aParentContextInfo,
                              knlValueBlockList   ** aParentIndexValueList,
                              ellEnv               * aEnv )
{
    stlInt32             i = 0;
    
    stlInt32             sForeignColumnCnt = 0;
    stlInt32           * sForeignMatchIdx = NULL;
    stlInt32             sMatchIdx = 0;
    ellDictHandle      * sForeignColumnHandle = NULL;
    
    ellDictHandle      * sParentKeyHandle = NULL;
    ellDictHandle      * sParentIndexHandle = NULL;
    ellIndexKeyDesc    * sParentIndexKeyDesc = NULL;
    knlValueBlockList  * sParentIndexValueList = NULL;
    knlValueBlockList  * sParentIndexColumnValue = NULL;
    stlInt32             sParentIndexColumnIdx = 0;

    knlPredicateList   * sKeyRange = NULL;
    knlValueBlockList  * sKeyColumn = NULL;
    knlValueBlockList  * sKeyValue = NULL;
    knlValueBlockList  * sKeyConst = NULL;
    knlExprContextInfo * sRangeContext = NULL;

    stlInt32             sKeyColCnt = 0;

    ellIndexKeyDesc    * sKeyColDesc = NULL;
    knlExprContext     * sContexts   = NULL;
    stlInt32             sContextCnt = 0;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aChildTableHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aChildForeignHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aChildTableValueList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParentIndexValueList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParentKeyRange != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Foreign Key 정보 획득
     */

    sForeignColumnCnt = ellGetForeignKeyColumnCount( aChildForeignHandle );
    sForeignMatchIdx = ellGetForeignKeyMatchIdxInParent( aChildForeignHandle );
    sForeignColumnHandle = ellGetForeignKeyColumnHandle( aChildForeignHandle );
    
    /**
     * Parent Key 정보 획득
     */

    sParentKeyHandle = ellGetForeignKeyParentUniqueKeyHandle( aChildForeignHandle );

    /**
     * Parent Key 의 Index 정보 획득
     */
    
    sParentIndexHandle = ellGetConstraintIndexDictHandle( sParentKeyHandle );
    sParentIndexKeyDesc = ellGetIndexKeyDesc( sParentIndexHandle );

    STL_ASSERT( sForeignColumnCnt == ellGetIndexKeyCount( sParentIndexHandle ) );

    /**
     * Parent Index Value List 할당
     */
    
    STL_TRY( ellAllocIndexValueListForSELECT(
                 sParentIndexHandle,
                 ellGetIndexKeyCount( sParentIndexHandle ),
                 aRegionMem,
                 KNL_NO_BLOCK_READ_CNT,
                 & sParentIndexValueList,
                 aEnv )
             == STL_SUCCESS );

    /**
     * Key Range 생성
     */

    sKeyColCnt = ellGetIndexKeyCount(sParentIndexHandle);
    
    STL_TRY( knlCreateRangePred( sKeyColCnt,
                                 STL_TRUE,
                                 & sKeyRange,
                                 & sRangeContext,
                                 aRegionMem,
                                 KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    
    sContexts = sRangeContext->mContexts;
        
    for ( i = 0 ; i < sKeyColCnt ; i++ )
    {
        sKeyColumn = NULL;
        sKeyConst = NULL;
        
        /**
         * Key Column 설정 
         */

        sParentIndexColumnIdx =
            ellGetColumnIdx( & sParentIndexKeyDesc[i].mKeyColumnHandle );
        sParentIndexColumnValue = ellFindColumnValueList( sParentIndexValueList,
                                                          ellGetColumnTableID( & sParentIndexKeyDesc[i].mKeyColumnHandle ),
                                                          sParentIndexColumnIdx );
        
        STL_TRY( knlInitShareBlock( & sKeyColumn,
                                    sParentIndexColumnValue,
                                    aRegionMem,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
                                                        
        /**
         * Key Value 설정
         * - Parent Unique Index ( i1, i2 )
         * - Child Foreign Key (c1, c2) references ( i2, i1 )
         * 위와 같은 설정을 찾기 위해 Match Idx 를 찾아야 한다.
         */

        for ( sMatchIdx = 0; sMatchIdx < sForeignColumnCnt; sMatchIdx++ )
        {
            if ( sForeignMatchIdx[sMatchIdx] == i )
            {
                break;
            }
        }
        
        sKeyValue =
            ellFindColumnValueList( aChildTableValueList,
                                    ellGetColumnTableID( & sForeignColumnHandle[sMatchIdx] ),
                                    ellGetColumnIdx( & sForeignColumnHandle[sMatchIdx] ) );

        STL_TRY( knlInitShareBlock( & sKeyConst,
                                    sKeyValue,
                                    aRegionMem,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        /**
         * Equal Key Range 생성
         */
        
        sKeyColDesc = & sParentIndexKeyDesc[i];

        /* context 설정 */
        STL_TRY( knlAddRange( STL_TRUE,
                              sContextCnt,
                              sContextCnt + 1,
                              STL_TRUE,
                              ( sKeyColDesc->mKeyOrdering == ELL_INDEX_COLUMN_ASCENDING ),
                              ( sKeyColDesc->mKeyNullOrdering == ELL_INDEX_COLUMN_NULLS_FIRST ),
                              STL_TRUE, /* Null Always STOP */
                              sKeyRange,
                              KNL_ENV( aEnv ) )
                 == STL_SUCCESS );

        STL_TRY( knlAddRange( STL_FALSE,
                              sContextCnt,
                              sContextCnt + 1,
                              STL_TRUE,
                              ( sKeyColDesc->mKeyOrdering == ELL_INDEX_COLUMN_ASCENDING ),
                              ( sKeyColDesc->mKeyNullOrdering == ELL_INDEX_COLUMN_NULLS_FIRST ),
                              STL_TRUE, /* Null Always STOP */
                              sKeyRange,
                              KNL_ENV( aEnv ) )
                 == STL_SUCCESS );

        sContexts[ sContextCnt++ ].mInfo.mValueInfo = sKeyColumn->mValueBlock;
        sContexts[ sContextCnt++ ].mInfo.mValueInfo = sKeyConst->mValueBlock;
    }


    /**
     * Index Column Order 설정
     */

    STL_TRY( ellSetIndexColumnOrder( sParentIndexValueList,
                                     sParentIndexHandle,
                                     aEnv )
             == STL_SUCCESS );
                 
    /**
     * Output 설정
     */

    *aParentIndexValueList = sParentIndexValueList;
    *aParentKeyRange = sKeyRange;
    *aParentContextInfo = sRangeContext;
        
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief Foreign Key 검사를 위해 Child Key 에 대한 Key Range 를 생성한다.
 * @param[in]  aParentTableHandle    Parent Table 의 Dictionary Handle
 * @param[in]  aChildForeignHandle   Child Table 의 Foreign Key Handle
 * @param[in]  aParentTableValueList Parent Table 의 Table Value List 공간
 * @param[in]  aRegionMem            Region Memory
 * @param[out] aChildKeyRange        Child Index 에 대한 Key Range
 * @param[out] aChildContextInfo     Child Index 에 대한 Key Range Context
 * @param[out] aChildIndexValueList  Child Index 의 Value List 공간 
 * @param[in]  aEnv                  Environment
 * @remarks
 * Parent Table 의 Row 삭제시 Child Table 들의 Foreign Key 검사를 위해
 * Foreign Key Index 의 Key Range 를 생성한다.
 */
stlStatus eldMakeChildRange( ellDictHandle        * aParentTableHandle,
                             ellDictHandle        * aChildForeignHandle,
                             knlValueBlockList    * aParentTableValueList,
                             knlRegionMem         * aRegionMem,
                             knlPredicateList    ** aChildKeyRange,
                             knlExprContextInfo  ** aChildContextInfo,
                             knlValueBlockList   ** aChildIndexValueList,
                             ellEnv               * aEnv )
{
    stlInt32             i = 0;
    
    stlInt32             sForeignColumnCnt = 0;
    stlInt32           * sForeignMatchIdx = NULL;
    stlInt32             sMatchIdx = 0;
    
    ellDictHandle      * sParentKeyHandle = NULL;
    ellDictHandle      * sParentIndexHandle = NULL;
    ellIndexKeyDesc    * sParentIndexKeyDesc = NULL;
    stlInt32             sParentIndexColumnIdx = 0;
    
    
    ellDictHandle      * sChildIndexHandle = NULL;
    ellIndexKeyDesc    * sChildIndexKeyDesc = NULL;
    knlValueBlockList  * sChildIndexValueList = NULL;
    knlValueBlockList  * sChildIndexColumnValue = NULL;
    stlInt32             sChildIndexColumnIdx = 0;

    knlPredicateList   * sKeyRange = NULL;
    knlExprContextInfo * sChildContextInfo = NULL;
    knlValueBlockList  * sKeyColumn = NULL;
    knlValueBlockList  * sKeyValue = NULL;
    knlValueBlockList  * sKeyConst = NULL;

    stlInt32             sKeyColCnt = 0;

    ellIndexKeyDesc    * sKeyColDesc = NULL;
    knlExprContext     * sContexts   = NULL;
    stlInt32             sContextCnt = 0;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aParentTableHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aChildForeignHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParentTableValueList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aChildKeyRange != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aChildContextInfo != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aChildIndexValueList != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Foreign Key 정보 획득
     */

    sForeignColumnCnt = ellGetForeignKeyColumnCount( aChildForeignHandle );
    sForeignMatchIdx = ellGetForeignKeyMatchIdxInParent( aChildForeignHandle );
    /* sForeignColumnHandle = ellGetForeignKeyColumnHandle( aChildForeignHandle ); */
    
    /**
     * Child Key 의 Index 정보 획득
     */
    
    sChildIndexHandle = ellGetConstraintIndexDictHandle( aChildForeignHandle );
    sChildIndexKeyDesc = ellGetIndexKeyDesc( sChildIndexHandle );

    /**
     * Parent Unique/Primary Key 정보 획득
     */

    sParentKeyHandle   = ellGetForeignKeyParentUniqueKeyHandle( aChildForeignHandle );
    sParentIndexHandle = ellGetConstraintIndexDictHandle( sParentKeyHandle );
    sParentIndexKeyDesc = ellGetIndexKeyDesc( sParentIndexHandle );
    
    STL_ASSERT( sForeignColumnCnt == ellGetIndexKeyCount( sParentIndexHandle ) );

    /**
     * Child Index Value List 할당
     */
    
    STL_TRY( ellAllocIndexValueListForSELECT(
                 sChildIndexHandle,
                 ellGetIndexKeyCount( sChildIndexHandle ),
                 aRegionMem,
                 KNL_NO_BLOCK_READ_CNT,
                 & sChildIndexValueList,
                 aEnv )
             == STL_SUCCESS );

    /**
     * Key Range 생성
     */

    sKeyColCnt = ellGetIndexKeyCount(sParentIndexHandle);
    
    STL_TRY( knlCreateRangePred( sKeyColCnt,
                                 STL_TRUE,
                                 & sKeyRange,
                                 & sChildContextInfo,
                                 aRegionMem,
                                 KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    sContexts = sChildContextInfo->mContexts;
    
    for ( i = 0 ; i < sKeyColCnt ; i++ )
    {
        sKeyColumn = NULL;
        sKeyConst = NULL;
        
        /**
         * Key Column 설정 
         */

        sChildIndexColumnIdx = ellGetColumnIdx( & sChildIndexKeyDesc[i].mKeyColumnHandle );
        sChildIndexColumnValue =
            ellFindColumnValueList( sChildIndexValueList,
                                    ellGetColumnTableID( & sChildIndexKeyDesc[i].mKeyColumnHandle ),
                                    sChildIndexColumnIdx );
        
        STL_TRY( knlInitShareBlock( & sKeyColumn,
                                    sChildIndexColumnValue,
                                    aRegionMem,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
                                                        
        /**
         * Key Value 설정
         * - Parent Unique Index ( i1, i2 )
         * - Child Foreign Key (c1, c2) references ( i2, i1 )
         * 위와 같은 설정을 찾기 위해 Match Idx 를 찾아야 한다.
         */

        sMatchIdx = sForeignMatchIdx[i];
        sParentIndexColumnIdx =
            ellGetColumnIdx( & sParentIndexKeyDesc[sMatchIdx].mKeyColumnHandle );
        
        sKeyValue =
            ellFindColumnValueList( aParentTableValueList,
                                    ellGetColumnTableID( & sParentIndexKeyDesc[sMatchIdx].mKeyColumnHandle ),
                                    sParentIndexColumnIdx );

        STL_TRY( knlInitShareBlock( & sKeyConst,
                                    sKeyValue,
                                    aRegionMem,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        /**
         * Equal Key Range 생성
         */

        sKeyColDesc = & sChildIndexKeyDesc[i];
            
        /* context 설정 */
        STL_TRY( knlAddRange( STL_TRUE,
                              sContextCnt,
                              sContextCnt + 1,
                              STL_TRUE,
                              ( sKeyColDesc->mKeyOrdering == ELL_INDEX_COLUMN_ASCENDING ),
                              ( sKeyColDesc->mKeyNullOrdering == ELL_INDEX_COLUMN_NULLS_FIRST ),
                              STL_TRUE, /* Null Always STOP */
                              sKeyRange,
                              KNL_ENV( aEnv ) )
                 == STL_SUCCESS );

        STL_TRY( knlAddRange( STL_FALSE,
                              sContextCnt,
                              sContextCnt + 1,
                              STL_TRUE,
                              ( sKeyColDesc->mKeyOrdering == ELL_INDEX_COLUMN_ASCENDING ),
                              ( sKeyColDesc->mKeyNullOrdering == ELL_INDEX_COLUMN_NULLS_FIRST ),
                              STL_TRUE, /* Null Always STOP */
                              sKeyRange,
                              KNL_ENV( aEnv ) )
                 == STL_SUCCESS );

        sContexts[ sContextCnt++ ].mInfo.mValueInfo = sKeyColumn->mValueBlock;
        sContexts[ sContextCnt++ ].mInfo.mValueInfo = sKeyConst->mValueBlock;
    }

    /**
     * Index Column Order 설정
     */
    
    STL_TRY( ellSetIndexColumnOrder( sChildIndexValueList,
                                     sChildIndexHandle,
                                     aEnv )
             == STL_SUCCESS );

    
    /**
     * Output 설정
     */

    *aChildIndexValueList = sChildIndexValueList;
    *aChildKeyRange = sKeyRange;
    *aChildContextInfo = sChildContextInfo;
        
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} eldIntegrity */
