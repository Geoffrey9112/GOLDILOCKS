/*******************************************************************************
 * ellDictDatabase.c
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
 * @file ellDictDatabase.c
 * @brief Database Dictionary 관리 모듈
 */

#include <ell.h>
#include <elDef.h>

#include <eldt.h>
#include <eldc.h>

#include <eldDictionary.h>
#include <eldTable.h>
#include <eldCatalog.h>
#include <eldColumn.h>
#include <eldIndex.h>
#include <eldIntegrity.h>
#include <eldTableConstraint.h>

/**
 * @addtogroup ellObjectDatabase
 * @{
 */

/**
 * @brief Database를 위한 Dictionary Table 들을 생성한다.
 * @param[in]     aTransID                Transaction ID
 * @param[in]     aStmt                   Statement
 * @param[in]     aDatabaseName           Database 의 이름 (null-terminated)
 * @param[in]     aDatabaseComment        Database 의 주석 (nullable)
 * @param[in]     aFirstDictPhysicalID     최초 dictionary table(SCHEMA.TABLES)의 Physical ID
 * @param[in]     aFirstDictPhysicalHandle 최초 dictionary table(SCHEMA.TABLES)의 Physical Handle
 * @param[in]     aSystemTbsID            System Tablespace의 ID
 * @param[in]     aDefaultUserTbsID       User Default Tablespace의 ID
 * @param[in]     aDefaultUserTbsName     User Default Tablespace의 이름 (nullable)
 * @param[in]     aDefaultUndoTbsID       Undo Default Tablespace의 ID
 * @param[in]     aDefaultUndoTbsName     Undo Default Tablespace의 이름 (nullable)
 * @param[in]     aDefaultTempTbsID       Temp Default Tablespace의 ID
 * @param[in]     aDefaultTempTbsName     Temp Default Tablespace의 이름 (nullable) 
 * @param[in]     aCharacterSetID         Character Set의 ID
 * @param[in]     aCharLengthUnit         Char Legnth Units
 * @param[in]     aEncryptedSysPassword   SYS 사용자의 encrypted password (nullable)
 *                                   <BR> null 인 경우, "gliese" 에 대한 encrypted password
 * @param[in]     aTimeZoneIntervalString Time Zone Interval String
 *                                   <BR> ex) "INTERVAL '+09:00' HOUR TO MINUTE"
 * @param[in]     aEnv                    Execution Library Environment
 * @remarks
 * - Storage Manager가 정상 운영 가능한 상태인 SERVICE 단계에서 구축한다.
 * - 마무리 작업과 서비스 운영을 위해 트랜잭션 종료 후
 *   함수 ellFinishBuildDictionary() 를 호출해야 한다.
 */
stlStatus ellBuildDatabaseDictionary( smlTransId            aTransID,
                                      smlStatement        * aStmt,
                                      stlChar             * aDatabaseName,
                                      stlChar             * aDatabaseComment,
                                      stlInt64              aFirstDictPhysicalID,
                                      void                * aFirstDictPhysicalHandle,
                                      smlTbsId              aSystemTbsID,
                                      smlTbsId              aDefaultUserTbsID,
                                      stlChar             * aDefaultUserTbsName,
                                      smlTbsId              aDefaultUndoTbsID,
                                      stlChar             * aDefaultUndoTbsName,
                                      smlTbsId              aDefaultTempTbsID,
                                      stlChar             * aDefaultTempTbsName,
                                      dtlCharacterSet       aCharacterSetID,
                                      dtlStringLengthUnit   aCharLengthUnit,
                                      stlChar             * aEncryptedSysPassword,
                                      stlChar             * aTimeZoneIntervalString,
                                      ellEnv              * aEnv )
{
    STL_TRY( eldtBuildDEFINITION_SCHEMA( aTransID,
                                         aStmt,
                                         aDatabaseName,
                                         aDatabaseComment,
                                         aFirstDictPhysicalID,
                                         aFirstDictPhysicalHandle,
                                         aSystemTbsID,
                                         aDefaultUserTbsID,
                                         aDefaultUserTbsName,
                                         aDefaultUndoTbsID,
                                         aDefaultUndoTbsName,
                                         aDefaultTempTbsID,
                                         aDefaultTempTbsName,
                                         aCharacterSetID,
                                         aCharLengthUnit,
                                         aEncryptedSysPassword,
                                         aTimeZoneIntervalString,
                                         aEnv )
             == STL_SUCCESS );

    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}

/**
 * @brief Dictionary 구축후 운영 가능하도록 마무리 작업을 수행함.
 * @param[in]   aEnv    Environment
 * @remarks
 * Dictionary 구축 작업인 함수 ellBuildDatabaseDictionary() 를
 * 수행한 transaction 이 종료된 후 수행해야 함.
 */
stlStatus ellFinishBuildDictionary( ellEnv * aEnv )
{
    /**
     * Sequence 객체의 활성화
     * - 재구동시 사용할 Identity Column 의 현재값을 설정한다.
     */

    STL_TRY( eldFinishSetValueIdentityColumn( aEnv ) == STL_SUCCESS );
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    /**
     * disable index 를 활성화
     */

    STL_TRY( eldFinishBuildAllIndexData( aEnv ) == STL_SUCCESS );
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    /**
     * Dictionary Cache 구축 
     */

    STL_TRY( eldBuildDictionaryCache( aEnv ) == STL_SUCCESS );
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    /**
     * 제약조건을 만족하는 지 검사
     * - 모든 Dictionary Data 가 제약조건을 만족하는 지 검사한다.
     */

    STL_TRY( eldFinishCheckAllConstraint( aEnv ) == STL_SUCCESS );
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}

/**
 * @brief Fixed Object 를 위한 Plan Cache 를 구축하고 Dictionary Cache 구축이 완료됨
 * @param[in] aStartPhase    Start-Up Phase
 * @param[in] aEnv           Environment
 * @remarks
 */
stlStatus ellBuildFixedCacheAndCompleteCache( knlStartupPhase   aStartPhase,
                                              ellEnv          * aEnv )
{
    /**
     * Parameter Validation
     */
    
    STL_ASSERT( (aStartPhase == KNL_STARTUP_PHASE_NO_MOUNT) ||
                (aStartPhase == KNL_STARTUP_PHASE_MOUNT) ||
                (aStartPhase == KNL_STARTUP_PHASE_OPEN) );
    
    /**
     * Cache Build 를 위한 Cache Build Entry 설정
     */
        
    gEllCacheBuildEntry = & gEllSharedEntry->mPhaseCacheEntry[aStartPhase];
        
    /**
     * Fixed Table Cache 구축
     */
        
    STL_TRY( eldcBuildFixedCache( aEnv ) == STL_SUCCESS );
        
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
        
        
    /**
     * 해당 단계의 Dictionary Cache 구축이 완료됨
     */
        
    gEllCacheBuildEntry = NULL;
    gEllSharedEntry->mCurrCacheEntry = & gEllSharedEntry->mPhaseCacheEntry[aStartPhase];

    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );

    gEllCacheBuildEntry = NULL;
    
    return STL_FAILURE;
}



/**
 * @brief D$OBJECT_CACHE 를 위한 Dump Handle 을 설정한다.
 * @param[in] aEnv  Environment
 * @remarks
 * Start-Up 단계를 변경 구문을 수행한 gsql 의 정보를 변경해야 함.
 */
stlStatus ellSetDictionaryDumpHandle( ellEnv * aEnv )
{
    return eldSetDictionaryDumpHandle( aEnv );
}

/*************************************************************
 * 정보 변경 함수
 *************************************************************/



/**
 * @brief COMMENT ON 구문을 위한 Lock 을 획득한다.
 * @param[in]  aTransID       Transaction ID
 * @param[in]  aStmt          Statement
 * @param[in]  aObjectHandle  Comment 대상이 되는 Object Handle
 * @param[in]  aRelatedTableHandle   Table 종속적인 객체일 경우 Table Dictionary Handle (nullable)
 * @param[out] aLockSuccess   Lock 획득 여부 
 * @param[in]  aEnv           Environment
 * @remarks
 */
stlStatus ellLock4SetObjectComment( smlTransId              aTransID,
                                    smlStatement          * aStmt,
                                    ellDictHandle         * aObjectHandle,
                                    ellDictHandle         * aRelatedTableHandle,
                                    stlBool               * aLockSuccess,
                                    ellEnv                * aEnv )
{
    stlBool  sLocked = STL_TRUE;
    
    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aObjectHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aLockSuccess != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * 객체 유형에 따라 필요한 Lock 을 획득한다.
     */

    switch( aObjectHandle->mObjectType )
    {
        case ELL_OBJECT_COLUMN:
            {
                STL_DASSERT( aRelatedTableHandle != NULL );
                
                /**
                 * Table 객체에 S lock
                 */

                STL_TRY( eldLockTable4DDL( aTransID,
                                           aStmt,
                                           SML_LOCK_S,
                                           aRelatedTableHandle,
                                           & sLocked,
                                           aEnv )
                         == STL_SUCCESS );

                STL_TRY_THROW( sLocked == STL_TRUE, RAMP_FINISH );
                
                /**
                 * Column 에 대한 X lock 을 획득
                 */

                STL_TRY( eldLockRowByObjectHandle( aTransID,
                                                   aStmt,
                                                   ELDT_TABLE_ID_COLUMNS,
                                                   ELDT_Columns_ColumnOrder_COLUMN_ID,
                                                   aObjectHandle,
                                                   SML_LOCK_X,
                                                   & sLocked,
                                                   aEnv )
                         == STL_SUCCESS );

                STL_TRY_THROW( sLocked == STL_TRUE, RAMP_FINISH );
                
                break;
            }
        case ELL_OBJECT_TABLE:
            {
                /**
                 * Table 객체에 X lock
                 */

                STL_TRY( eldLockTable4DDL( aTransID,
                                           aStmt,
                                           SML_LOCK_X,
                                           aObjectHandle,
                                           & sLocked,
                                           aEnv )
                         == STL_SUCCESS );
                
                STL_TRY_THROW( sLocked == STL_TRUE, RAMP_FINISH );
                
                break;
            }
        case ELL_OBJECT_CONSTRAINT:
            {
                STL_DASSERT( aRelatedTableHandle != NULL );
                
                /**
                 * Table 객체에 S lock
                 */

                STL_TRY( eldLockTable4DDL( aTransID,
                                           aStmt,
                                           SML_LOCK_S,
                                           aRelatedTableHandle,
                                           & sLocked,
                                           aEnv )
                         == STL_SUCCESS );

                STL_TRY_THROW( sLocked == STL_TRUE, RAMP_FINISH );
                
                /**
                 * Constraint 에 대한 X lock 을 획득
                 */

                STL_TRY( eldLockRowByObjectHandle( aTransID,
                                                   aStmt,
                                                   ELDT_TABLE_ID_TABLE_CONSTRAINTS,
                                                   ELDT_TableConst_ColumnOrder_CONSTRAINT_ID,
                                                   aObjectHandle,
                                                   SML_LOCK_X,
                                                   & sLocked,
                                                   aEnv )
                         == STL_SUCCESS );

                STL_TRY_THROW( sLocked == STL_TRUE, RAMP_FINISH );
                
                break;
            }
        case ELL_OBJECT_INDEX:
            {
                STL_DASSERT( aRelatedTableHandle != NULL );

                /**
                 * Table 객체에 S lock
                 */

                STL_TRY( eldLockTable4DDL( aTransID,
                                           aStmt,
                                           SML_LOCK_S,
                                           aRelatedTableHandle,
                                           & sLocked,
                                           aEnv )
                         == STL_SUCCESS );

                STL_TRY_THROW( sLocked == STL_TRUE, RAMP_FINISH );
                
                /**
                 * Index 에 대한 X lock 을 획득
                 */

                STL_TRY( eldLockRowByObjectHandle( aTransID,
                                                   aStmt,
                                                   ELDT_TABLE_ID_INDEXES,
                                                   ELDT_Indexes_ColumnOrder_INDEX_ID,
                                                   aObjectHandle,
                                                   SML_LOCK_X,
                                                   & sLocked,
                                                   aEnv )
                         == STL_SUCCESS );

                STL_TRY_THROW( sLocked == STL_TRUE, RAMP_FINISH );
                
                break;
            }
        case ELL_OBJECT_SEQUENCE:
            {
                /**
                 * Sequence 에 대한 X lock 을 획득
                 */

                STL_TRY( eldLockRowByObjectHandle( aTransID,
                                                   aStmt,
                                                   ELDT_TABLE_ID_SEQUENCES,
                                                   ELDT_Sequences_ColumnOrder_SEQUENCE_ID,
                                                   aObjectHandle,
                                                   SML_LOCK_X,
                                                   & sLocked,
                                                   aEnv )
                         == STL_SUCCESS );

                STL_TRY_THROW( sLocked == STL_TRUE, RAMP_FINISH );
                
                break;
            }
        case ELL_OBJECT_SCHEMA:
            {
                /**
                 * Schema 에 대한 X lock 을 획득
                 */

                STL_TRY( eldLockRowByObjectHandle( aTransID,
                                                   aStmt,
                                                   ELDT_TABLE_ID_SCHEMATA,
                                                   ELDT_Schemata_ColumnOrder_SCHEMA_ID,
                                                   aObjectHandle,
                                                   SML_LOCK_X,
                                                   & sLocked,
                                                   aEnv )
                         == STL_SUCCESS );
                
                STL_TRY_THROW( sLocked == STL_TRUE, RAMP_FINISH );
                
                break;
            }
        case ELL_OBJECT_TABLESPACE:
            {
                /**
                 * Tablespace 에 대한 X lock 을 획득
                 */

                STL_TRY( eldLockRowByObjectHandle( aTransID,
                                                   aStmt,
                                                   ELDT_TABLE_ID_TABLESPACES,
                                                   ELDT_Space_ColumnOrder_TABLESPACE_ID,
                                                   aObjectHandle,
                                                   SML_LOCK_X,
                                                   & sLocked,
                                                   aEnv )
                         == STL_SUCCESS );

                STL_TRY_THROW( sLocked == STL_TRUE, RAMP_FINISH );
                
                break;
            }
        case ELL_OBJECT_AUTHORIZATION:
            {
                /**
                 * Authorization 에 대한 X lock 을 획득
                 */

                STL_TRY( eldLockRowByObjectHandle( aTransID,
                                                   aStmt,
                                                   ELDT_TABLE_ID_AUTHORIZATIONS,
                                                   ELDT_Auth_ColumnOrder_AUTH_ID,
                                                   aObjectHandle,
                                                   SML_LOCK_X,
                                                   & sLocked,
                                                   aEnv )
                         == STL_SUCCESS );

                STL_TRY_THROW( sLocked == STL_TRUE, RAMP_FINISH );
                
                break;
            }
        case ELL_OBJECT_PROFILE:
            {
                /**
                 * Authorization 에 대한 X lock 을 획득
                 */

                STL_TRY( eldLockRowByObjectHandle( aTransID,
                                                   aStmt,
                                                   ELDT_TABLE_ID_PROFILES,
                                                   ELDT_Profile_ColumnOrder_PROFILE_ID,
                                                   aObjectHandle,
                                                   SML_LOCK_X,
                                                   & sLocked,
                                                   aEnv )
                         == STL_SUCCESS );

                STL_TRY_THROW( sLocked == STL_TRUE, RAMP_FINISH );
                
                break;
            }
        case ELL_OBJECT_CATALOG:
            {
                /**
                 * Catalog 에 대한 X lock 을 획득
                 */

                STL_TRY( eldLockRowByObjectHandle( aTransID,
                                                   aStmt,
                                                   ELDT_TABLE_ID_CATALOG_NAME,
                                                   ELDT_Catalog_ColumnOrder_CATALOG_ID,
                                                   aObjectHandle,
                                                   SML_LOCK_X,
                                                   & sLocked,
                                                   aEnv )
                         == STL_SUCCESS );

                STL_TRY_THROW( sLocked == STL_TRUE, RAMP_FINISH );
                
                break;
            }
        default:
            STL_ASSERT(0);
            break;
    }

    /**
     * 작업 종료
     */

    STL_RAMP( RAMP_FINISH );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );

    *aLockSuccess = sLocked;
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}

/**
 * @brief Object 에 Comment 를 설정한다.
 * @param[in]  aTransID       Transaction ID
 * @param[in]  aStmt          Statement
 * @param[in]  aObjectHandle  Object Dictionary Handle
 * @param[in]  aCommentString Comment String
 * @param[in]  aEnv           Environment
 * @remarks
 */
stlStatus ellSetObjectComment( smlTransId              aTransID,
                               smlStatement          * aStmt,
                               ellDictHandle         * aObjectHandle,
                               stlChar               * aCommentString,
                               ellEnv                * aEnv )
{
    void * sObjectDesc = NULL;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aObjectHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aObjectHandle->mObjectType > ELL_OBJECT_NA) &&
                        (aObjectHandle->mObjectType < ELL_OBJECT_MAX),
                        ELL_ERROR_STACK(aEnv) );

    /**
     * 객체 유형에 따라 관련 Dictionary Table 의 Comment Column 을 변경한다.
     */

    sObjectDesc = ellGetObjectDesc( aObjectHandle );
    
    switch( aObjectHandle->mObjectType )
    {
        case ELL_OBJECT_COLUMN:
            STL_TRY( eldModifyDictDesc( aTransID,
                                        aStmt,
                                        ELDT_TABLE_ID_COLUMNS,
                                        ELDT_Columns_ColumnOrder_COLUMN_ID,
                                        ((ellColumnDesc*)sObjectDesc)->mColumnID,
                                        ELDT_Columns_ColumnOrder_COMMENTS,
                                        aCommentString,
                                        aEnv )
                     == STL_SUCCESS );
            break;
        case ELL_OBJECT_TABLE:
            STL_TRY( eldModifyDictDesc( aTransID,
                                        aStmt,
                                        ELDT_TABLE_ID_TABLES,
                                        ELDT_Tables_ColumnOrder_TABLE_ID,
                                        ((ellTableDesc*)sObjectDesc)->mTableID,
                                        ELDT_Tables_ColumnOrder_COMMENTS,
                                        aCommentString,
                                        aEnv )
                     == STL_SUCCESS );
            break;
        case ELL_OBJECT_CONSTRAINT:
            STL_TRY( eldModifyDictDesc( aTransID,
                                        aStmt,
                                        ELDT_TABLE_ID_TABLE_CONSTRAINTS,
                                        ELDT_TableConst_ColumnOrder_CONSTRAINT_ID,
                                        ((ellTableConstDesc*)sObjectDesc)->mConstID,
                                        ELDT_TableConst_ColumnOrder_COMMENTS,
                                        aCommentString,
                                        aEnv )
                     == STL_SUCCESS );
            break;
        case ELL_OBJECT_INDEX:
            STL_TRY( eldModifyDictDesc( aTransID,
                                        aStmt,
                                        ELDT_TABLE_ID_INDEXES,
                                        ELDT_Indexes_ColumnOrder_INDEX_ID,
                                        ((ellIndexDesc*)sObjectDesc)->mIndexID,
                                        ELDT_Indexes_ColumnOrder_COMMENTS,
                                        aCommentString,
                                        aEnv )
                     == STL_SUCCESS );
            break;
        case ELL_OBJECT_SEQUENCE:
            STL_TRY( eldModifyDictDesc( aTransID,
                                        aStmt,
                                        ELDT_TABLE_ID_SEQUENCES,
                                        ELDT_Sequences_ColumnOrder_SEQUENCE_ID,
                                        ((ellSequenceDesc*)sObjectDesc)->mSequenceID,
                                        ELDT_Sequences_ColumnOrder_COMMENTS,
                                        aCommentString,
                                        aEnv )
                     == STL_SUCCESS );
            break;
        case ELL_OBJECT_SCHEMA:
            STL_TRY( eldModifyDictDesc( aTransID,
                                        aStmt,
                                        ELDT_TABLE_ID_SCHEMATA,
                                        ELDT_Schemata_ColumnOrder_SCHEMA_ID,
                                        ((ellSchemaDesc*)sObjectDesc)->mSchemaID,
                                        ELDT_Schemata_ColumnOrder_COMMENTS,
                                        aCommentString,
                                        aEnv )
                     == STL_SUCCESS );
            break;
        case ELL_OBJECT_TABLESPACE:
            STL_TRY( eldModifyDictDesc( aTransID,
                                        aStmt,
                                        ELDT_TABLE_ID_TABLESPACES,
                                        ELDT_Space_ColumnOrder_TABLESPACE_ID,
                                        ((ellTablespaceDesc*)sObjectDesc)->mTablespaceID,
                                        ELDT_Space_ColumnOrder_COMMENTS,
                                        aCommentString,
                                        aEnv )
                     == STL_SUCCESS );
            break;
        case ELL_OBJECT_AUTHORIZATION:
            STL_TRY( eldModifyDictDesc( aTransID,
                                        aStmt,
                                        ELDT_TABLE_ID_AUTHORIZATIONS,
                                        ELDT_Auth_ColumnOrder_AUTH_ID,
                                        ((ellAuthDesc*)sObjectDesc)->mAuthID,
                                        ELDT_Auth_ColumnOrder_COMMENTS,
                                        aCommentString,
                                        aEnv )
                     == STL_SUCCESS );
            break;
        case ELL_OBJECT_PROFILE:
            STL_TRY( eldModifyDictDesc( aTransID,
                                        aStmt,
                                        ELDT_TABLE_ID_PROFILES,
                                        ELDT_Profile_ColumnOrder_PROFILE_ID,
                                        ((ellProfileDesc*)sObjectDesc)->mProfileID,
                                        ELDT_Profile_ColumnOrder_COMMENTS,
                                        aCommentString,
                                        aEnv )
                     == STL_SUCCESS );
            break;
        case ELL_OBJECT_CATALOG:
            STL_TRY( eldModifyDictDesc( aTransID,
                                        aStmt,
                                        ELDT_TABLE_ID_CATALOG_NAME,
                                        ELDT_Catalog_ColumnOrder_CATALOG_ID,
                                        ((ellCatalogDesc*)sObjectDesc)->mCatalogID,
                                        ELDT_Catalog_ColumnOrder_COMMENTS,
                                        aCommentString,
                                        aEnv )
                     == STL_SUCCESS );
            break;
        default:
            STL_ASSERT(0);
            break;
    }

    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/*************************************************************
 * Lock 획득 함수
 *************************************************************/

/**
 * @brief Object Handle 에 대해 논리적 Lock 을 획득한다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aObjectHandle    SQL-Object Handle
 * @param[in]  aLockMode        다음 중 하나의 값이어야 함.
 *                         <BR> - SML_LOCK_S
 *                         <BR> - SML_LOCK_X
 * @param[out] aLockSuccess     Lock 획득 여부                         
 * @param[in]  aEnv          Environment
 * @remarks
 */
stlStatus ellLogicalObjectLock( smlTransId      aTransID,
                                smlStatement  * aStmt,
                                ellDictHandle * aObjectHandle,
                                stlInt32        aLockMode,
                                stlBool       * aLockSuccess,
                                ellEnv        * aEnv )
{
    eldtDefinitionTableID   sDictTableID = ELDT_TABLE_ID_NA;
    stlInt32                sDictColumnIdx = 0;
    
    stlBool   sLocked = STL_FALSE;

    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aObjectHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aLockMode == SML_LOCK_S) || (aLockMode == SML_LOCK_X),
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aLockSuccess != NULL, ELL_ERROR_STACK(aEnv) );


    /**
     * Object 유형에 따른 Dictionary 정보 결정
     */

    switch ( aObjectHandle->mObjectType )
    {
        case ELL_OBJECT_COLUMN:
            {
                sDictTableID   = ELDT_TABLE_ID_COLUMNS;
                sDictColumnIdx = ELDT_Columns_ColumnOrder_COLUMN_ID;
                break;
            }
        case ELL_OBJECT_TABLE:
            {
                sDictTableID   = ELDT_TABLE_ID_TABLES;
                sDictColumnIdx = ELDT_Tables_ColumnOrder_TABLE_ID;
                break;
            }
        case ELL_OBJECT_CONSTRAINT:
            {
                sDictTableID   = ELDT_TABLE_ID_TABLE_CONSTRAINTS;
                sDictColumnIdx = ELDT_TableConst_ColumnOrder_CONSTRAINT_ID;
                break;
            }
        case ELL_OBJECT_INDEX:
            {
                sDictTableID   = ELDT_TABLE_ID_INDEXES;
                sDictColumnIdx = ELDT_Indexes_ColumnOrder_INDEX_ID;
                break;
            }
        case ELL_OBJECT_SEQUENCE:
            {
                sDictTableID   = ELDT_TABLE_ID_SEQUENCES;
                sDictColumnIdx = ELDT_Sequences_ColumnOrder_SEQUENCE_ID;
                break;
            }
        case ELL_OBJECT_SCHEMA:
            {
                sDictTableID   = ELDT_TABLE_ID_SCHEMATA;
                sDictColumnIdx = ELDT_Schemata_ColumnOrder_SCHEMA_ID;
                break;
            }
        case ELL_OBJECT_TABLESPACE:
            {
                sDictTableID   = ELDT_TABLE_ID_TABLESPACES;
                sDictColumnIdx = ELDT_Space_ColumnOrder_TABLESPACE_ID;
                break;
            }
        case ELL_OBJECT_AUTHORIZATION:
            {
                sDictTableID   = ELDT_TABLE_ID_AUTHORIZATIONS;
                sDictColumnIdx = ELDT_Auth_ColumnOrder_AUTH_ID;
                break;
            }
        case ELL_OBJECT_CATALOG:
            {
                sDictTableID   = ELDT_TABLE_ID_CATALOG_NAME;
                sDictColumnIdx = ELDT_Catalog_ColumnOrder_CATALOG_ID;
                break;
            }
        case ELL_OBJECT_SYNONYM:
            {
                sDictTableID   = ELDT_TABLE_ID_SYNONYMS;
                sDictColumnIdx = ELDT_Synonyms_ColumnOrder_SYNONYM_ID;
                break;
            }
        case ELL_OBJECT_PUBLIC_SYNONYM:
            {
                sDictTableID   = ELDT_TABLE_ID_PUBLIC_SYNONYMS;
                sDictColumnIdx = ELDT_Public_Synonyms_ColumnOrder_SYNONYM_ID;
                break;
            }
        case ELL_OBJECT_PROFILE:
            {
                sDictTableID   = ELDT_TABLE_ID_PROFILES;
                sDictColumnIdx = ELDT_Profile_ColumnOrder_PROFILE_ID;
                break;
            }
        default:
            {
                STL_DASSERT(0);
                STL_TRY(0);
                break;
            }
    }

    /**
     * Object Lock 설정
     */

    STL_TRY( eldLockRowByObjectHandle( aTransID,
                                       aStmt,
                                       sDictTableID,
                                       sDictColumnIdx,
                                       aObjectHandle,
                                       aLockMode,
                                       & sLocked,
                                       aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );

    /**
     * Output 설정
     */

    *aLockSuccess = sLocked;
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}

/**
 * @brief Object 생성(base table 이외) 이후 해당 객체에 대해 lock 을 획득한다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aObjectType      Object Type
 * @param[in]  aObjectID        Object ID
 * @param[in]  aEnv             Environment
 * @remarks
 * commit -> pending -> trans end (unlock) 과정 중에
 * 해당 객체에 대한 DROP, ALTER 가 수행될 경우를 방지하기 위해
 * 생성한 객체에 대한 lock 을 획득해야 함.
 */
stlStatus ellLockRowAfterCreateNonBaseTableObject( smlTransId       aTransID,
                                                   smlStatement   * aStmt,
                                                   ellObjectType    aObjectType,
                                                   stlInt64         aObjectID,
                                                   ellEnv         * aEnv )
{
    eldtDefinitionTableID   sDictTableID = ELDT_TABLE_ID_NA;
    stlInt32                sObjectColumnIdx = 0;

    stlBool   sLocked = STL_TRUE;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * 객체에 해당하는 Dictionary Table 과 Column Idx 정보 획득
     */

    switch( aObjectType )
    {
        case ELL_OBJECT_COLUMN:
            /**
             * CREATE COLUMN 은 존재하지 않으며, Table X Lock 대상임
             */
            STL_DASSERT(0);
            break;
        case ELL_OBJECT_TABLE:
            /**
             * Base Table 이 아니어야 함.
             */
            sDictTableID     = ELDT_TABLE_ID_TABLES;
            sObjectColumnIdx = ELDT_Tables_ColumnOrder_TABLE_ID;
            break;
        case ELL_OBJECT_CONSTRAINT:
            /**
             * 동시에 수행가능한 제약조건이 있으며,
             * Table S Lock 이므로 생성한 제약조건에 대한 X lock 을 잡아야 함.
             */
            sDictTableID     = ELDT_TABLE_ID_TABLE_CONSTRAINTS;
            sObjectColumnIdx = ELDT_TableConst_ColumnOrder_CONSTRAINT_ID;
            break;
        case ELL_OBJECT_INDEX:
            sDictTableID     = ELDT_TABLE_ID_INDEXES;
            sObjectColumnIdx = ELDT_Indexes_ColumnOrder_INDEX_ID;
            break;
        case ELL_OBJECT_SEQUENCE:
            sDictTableID     = ELDT_TABLE_ID_SEQUENCES;
            sObjectColumnIdx = ELDT_Sequences_ColumnOrder_SEQUENCE_ID;
            break;
        case ELL_OBJECT_SYNONYM:
            sDictTableID     = ELDT_TABLE_ID_SYNONYMS;
            sObjectColumnIdx = ELDT_Synonyms_ColumnOrder_SYNONYM_ID;
            break;
        case ELL_OBJECT_PUBLIC_SYNONYM:
            sDictTableID     = ELDT_TABLE_ID_PUBLIC_SYNONYMS;
            sObjectColumnIdx = ELDT_Public_Synonyms_ColumnOrder_SYNONYM_ID;
            break;
        case ELL_OBJECT_SCHEMA:
            sDictTableID     = ELDT_TABLE_ID_SCHEMATA;
            sObjectColumnIdx = ELDT_Schemata_ColumnOrder_SCHEMA_ID;
            break;
        case ELL_OBJECT_TABLESPACE:
            sDictTableID     = ELDT_TABLE_ID_TABLESPACES;
            sObjectColumnIdx = ELDT_Space_ColumnOrder_TABLESPACE_ID;
            break;
        case ELL_OBJECT_AUTHORIZATION:
            sDictTableID     = ELDT_TABLE_ID_AUTHORIZATIONS;
            sObjectColumnIdx = ELDT_Auth_ColumnOrder_AUTH_ID;
            break;
        case ELL_OBJECT_CATALOG:
            /**
             * CREATE DATABASE 구문은 존재하지 않음.
             */
            STL_DASSERT(0);
            break;
        case ELL_OBJECT_PROFILE:
            sDictTableID     = ELDT_TABLE_ID_PROFILES;
            sObjectColumnIdx = ELDT_Profile_ColumnOrder_PROFILE_ID;
            break;
        default:
            STL_DASSERT(0);
            break;
    }

    /**
     * 해당 Row 에 X Lock Row 
     */

    STL_TRY( eldLockRowByObjectID( aTransID,
                                   aStmt,
                                   sDictTableID,
                                   sObjectColumnIdx,
                                   aObjectID,
                                   SML_LOCK_X,
                                   & sLocked,
                                   aEnv )
             == STL_SUCCESS );

    STL_DASSERT( sLocked == STL_TRUE );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
                                                  

/*************************************************************
 * Handle 획득 함수
 *************************************************************/



/**
 * @brief Catalog Dictionary Handle 을 얻는다.
 * @param[in]  aTransID             Transaction ID
 * @param[out] aCatalogDictHandle   Catalog Dictionary Handle
 * @param[in]  aEnv                 Envirionment 
 * @remarks
 */
stlStatus ellGetCatalogDictHandle( smlTransId           aTransID,
                                   ellDictHandle      * aCatalogDictHandle,
                                   ellEnv             * aEnv )
{
    stlBool   sExist = STL_FALSE;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aCatalogDictHandle != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Catalog Dictionary Handle 획득
     */

    ellInitDictHandle( aCatalogDictHandle );
    
    STL_TRY( eldcGetCatalogHandle( aTransID,
                                   SML_MAXIMUM_STABLE_SCN,
                                   aCatalogDictHandle,
                                   & sExist,
                                   aEnv )
             == STL_SUCCESS );

    /* catalog handle 은 변경되지 않는다 */
    STL_DASSERT( sExist == STL_TRUE );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
    
}


/*************************************************************
 * 정보 획득 함수
 *************************************************************/

/**
 * @brief Database 의 Catalog ID 정보
 * @return
 * - Catalog ID
 * @remarks
 */
stlInt64 ellGetDbCatalogID()
{
    ellCatalogDesc * sCatalogDesc =
        (ellCatalogDesc *) ellGetObjectDesc( ELL_CATALOG_HANDLE_ADDR() );

    return sCatalogDesc->mCatalogID;
}

/**
 * @brief Database 의 Catalog Name 정보 
 * @return
 * - Catalog Name
 * @remarks
 */
stlChar * ellGetDbCatalogName()
{
    ellCatalogDesc * sCatalogDesc =
        (ellCatalogDesc *) ellGetObjectDesc( ELL_CATALOG_HANDLE_ADDR() );

    return sCatalogDesc->mCatalogName;
}

/**
 * @brief Database 의 Character Set 정보 
 * @return
 * - Character Set ID
 * @remarks
 */
dtlCharacterSet ellGetDbCharacterSet()
{
    ellCatalogDesc * sCatalogDesc =
        (ellCatalogDesc *) ellGetObjectDesc( ELL_CATALOG_HANDLE_ADDR() );

    return sCatalogDesc->mCharacterSetID;
}


/**
 * @brief Database 의 Char Length Unit 정보 
 * @return
 * - char length units
 * @remarks
 */
dtlStringLengthUnit  ellGetDbCharLengthUnit()
{
    ellCatalogDesc * sCatalogDesc =
        (ellCatalogDesc *) ellGetObjectDesc( ELL_CATALOG_HANDLE_ADDR() );

    return sCatalogDesc->mCharLengthUnit;
}


/**
 * @brief Database 의 Time Zone 정보 
 * @return
 * - Timezone Interval Value
 * @remarks
 */
dtlIntervalDayToSecondType ellGetDbTimezone()
{
    ellCatalogDesc * sCatalogDesc =
        (ellCatalogDesc *) ellGetObjectDesc( ELL_CATALOG_HANDLE_ADDR() );

    return sCatalogDesc->mTimeZoneInterval;
}


/**
 * @brief Database 의 Default Data Tablespace Handle 
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[out] aSpaceHandle     Tablespace Handle
 * @param[in]  aRegionMem       Region Memory
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus ellGetDbDataSpaceHandle( smlTransId      aTransID,
                                   smlStatement  * aStmt,
                                   ellDictHandle * aSpaceHandle,
                                   knlRegionMem  * aRegionMem,
                                   ellEnv        * aEnv )
{
    ellObjectList * sObjectList = NULL;

    ellCatalogDesc * sCatalogDesc =
        (ellCatalogDesc *) ellGetObjectDesc( ELL_CATALOG_HANDLE_ADDR() );

    ellDictHandle * sSpaceHandle = NULL;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSpaceHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * DB Default Data Tablespace ID 획득 
     */

    STL_TRY( ellInitObjectList( & sObjectList, aRegionMem, aEnv ) == STL_SUCCESS );

    /**
     * DB Default Data Tablespace ID 획득 
     * - SELECT DEFAULT_DATA_TABLESPACE_ID
     * - FROM DEFINITION_SCHEMA.CATALOG_NAME
     * - WHERE CATALOG_ID = mCatalogID
     */
    
    STL_TRY( eldGetObjectList( aTransID,
                               aStmt,
                               ELDT_TABLE_ID_CATALOG_NAME,
                               ELL_OBJECT_TABLESPACE,
                               ELDT_Catalog_ColumnOrder_DEFAULT_DATA_TABLESPACE_ID,
                               ELDT_Catalog_ColumnOrder_CATALOG_ID,
                               sCatalogDesc->mCatalogID,
                               aRegionMem,
                               sObjectList,
                               aEnv )
             == STL_SUCCESS );

    /**
     * Output 설정
     */

    sSpaceHandle = ellGetNthObjectHandle( sObjectList, 0 );
    STL_TRY_THROW( sSpaceHandle != NULL, RAMP_ERR_OBJECT_MODIFIED );
    
    stlMemcpy( aSpaceHandle, sSpaceHandle, STL_SIZEOF(ellDictHandle) );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_OBJECT_MODIFIED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ELL_ERRCODE_INVALID_OBJECT,
                      NULL,
                      ELL_ERROR_STACK(aEnv) );
    }
    
    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/**
 * @brief Database 의 Default Temp Tablespace ID 정보 
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[out] aSpaceHandle     Tablespace Handle
 * @param[in]  aRegionMem       Region Memory
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus ellGetDbTempSpaceHandle( smlTransId      aTransID,
                                   smlStatement  * aStmt,
                                   ellDictHandle * aSpaceHandle,
                                   knlRegionMem  * aRegionMem,
                                   ellEnv        * aEnv )
{
    ellObjectList * sObjectList = NULL;

    ellCatalogDesc * sCatalogDesc =
        (ellCatalogDesc *) ellGetObjectDesc( ELL_CATALOG_HANDLE_ADDR() );

    ellDictHandle * sSpaceHandle = NULL;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSpaceHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * DB Default Temp Tablespace ID 획득 
     */

    STL_TRY( ellInitObjectList( & sObjectList, aRegionMem, aEnv ) == STL_SUCCESS );

    /**
     * DB Default Data Tablespace ID 획득 
     * - SELECT DEFAULT_TEMP_TABLESPACE_ID
     * - FROM DEFINITION_SCHEMA.CATALOG_NAME
     * - WHERE CATALOG_ID = mCatalogID
     */
    
    STL_TRY( eldGetObjectList( aTransID,
                               aStmt,
                               ELDT_TABLE_ID_CATALOG_NAME,
                               ELL_OBJECT_TABLESPACE,
                               ELDT_Catalog_ColumnOrder_DEFAULT_TEMP_TABLESPACE_ID,
                               ELDT_Catalog_ColumnOrder_CATALOG_ID,
                               sCatalogDesc->mCatalogID,
                               aRegionMem,
                               sObjectList,
                               aEnv )
             == STL_SUCCESS );

    /**
     * Output 설정
     */

    sSpaceHandle = ellGetNthObjectHandle( sObjectList, 0 );
    STL_TRY_THROW( sSpaceHandle != NULL, RAMP_ERR_OBJECT_MODIFIED );
    
    stlMemcpy( aSpaceHandle, sSpaceHandle, STL_SIZEOF(ellDictHandle) );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_OBJECT_MODIFIED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ELL_ERRCODE_INVALID_OBJECT,
                      NULL,
                      ELL_ERROR_STACK(aEnv) );
    }
    
    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}



/** @} ellObjectDatabase */
