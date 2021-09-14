/*******************************************************************************
 * ellDictSynonym.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: 
 *
 * NOTES
 *    
 *
 ******************************************************************************/


/**
 * @file ellDictSynonym.c
 * @brief Synonym Dictionary 관리 모듈
 */

#include <ell.h>
#include <elDef.h>

#include <eldt.h>
#include <eldc.h>
#include <eldDictionary.h>

#include <eldAuthorization.h>
#include <eldSynonym.h>

/**
 * @addtogroup ellObjectSynonym
 * @{
 */

/****************************************************************
 * DDL Lock 함수 
 ****************************************************************/

/**
 * @brief CREATE SYNONYM 구문을 위한 DDL Lock 을 획득한다.
 * @param[in]  aTransID        Transaction ID
 * @param[in]  aStmt           Statement
 * @param[in]  aOwnerHandle    Synonym 의 Owner Handle
 * @param[in]  aSchemaHandle   Synonym 의 Schema Handle
 * @param[out] aLockSuccess    Lock 획득 여부 
 * @param[in]  aEnv            Environment
 * @remarks
 */
stlStatus ellLock4CreateSynonym( smlTransId      aTransID,
                                 smlStatement  * aStmt,
                                 ellDictHandle * aOwnerHandle,
                                 ellDictHandle * aSchemaHandle,
                                 stlBool       * aLockSuccess,
                                 ellEnv        * aEnv )
{
    stlBool sLocked = STL_TRUE;
    
    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOwnerHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSchemaHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aLockSuccess != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * Owner Record 에 S lock 
     */
        
    STL_TRY( eldLockRowByObjectHandle( aTransID,
                                       aStmt,
                                       ELDT_TABLE_ID_AUTHORIZATIONS,
                                       ELDT_Auth_ColumnOrder_AUTH_ID,
                                       aOwnerHandle,
                                       SML_LOCK_S,
                                       & sLocked,
                                       aEnv )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_FINISH );
    
    /**
     * Schema Record 에 S lock
     */

    STL_TRY( eldLockRowByObjectHandle( aTransID,
                                       aStmt,
                                       ELDT_TABLE_ID_SCHEMATA,
                                       ELDT_Schemata_ColumnOrder_SCHEMA_ID,
                                       aSchemaHandle,
                                       SML_LOCK_S,
                                       & sLocked,
                                       aEnv )
             == STL_SUCCESS );
        
    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_FINISH );

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
 * @brief DROP SYNONYM 구문을 위한 DDL Lock 을 획득한다.
 * @param[in]  aTransID             Transaction ID
 * @param[in]  aStmt                Statement
 * @param[in]  aSynonymDictHandle   Synonym Dictionary Handle
 * @param[out] aLockSuccess         Lock 획득 여부 
 * @param[in]  aEnv                 Environment
 * @remarks
 */
stlStatus ellLock4DropSynonym( smlTransId      aTransID,
                               smlStatement  * aStmt,
                               ellDictHandle * aSynonymDictHandle,
                               stlBool       * aLockSuccess,
                               ellEnv        * aEnv )
{
    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSynonymDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aLockSuccess != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Synonym 레코드에 X lock 
     */
    
    STL_TRY( eldLockRowByObjectHandle( aTransID,
                                       aStmt,
                                       ELDT_TABLE_ID_SYNONYMS,
                                       ELDT_Synonyms_ColumnOrder_SYNONYM_ID,
                                       aSynonymDictHandle,
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
 * @brief DDL 수행시 관련 Synonym List 에 대한 Lock 을 획득한다.
 * @param[in]  aTransID      Transaction ID
 * @param[in]  aStmt         Statement
 * @param[in]  aSynonymList  Synonym Object List
 * @param[out] aLockSuccess  Lock 획득 여부 
 * @param[in]  aEnv          Environment
 * @remarks
 */
stlStatus ellLockRelatedSynonymList4DDL( smlTransId      aTransID,
                                         smlStatement  * aStmt,
                                         ellObjectList * aSynonymList,
                                         stlBool       * aLockSuccess,
                                         ellEnv        * aEnv )
{
    stlRingHead  * sHeadPtr = NULL;
    stlRingEntry * sDataPtr = NULL;

    ellObjectItem * sObjectItem = NULL;
    ellDictHandle * sObjectHandle = NULL;

    stlBool   sLocked = STL_TRUE;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSynonymList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aLockSuccess != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * Synonym 목록을 순회하며 lock 을 획득 
     */

    sHeadPtr = & aSynonymList->mHeadList;
    sDataPtr = NULL;

    STL_RING_FOREACH_ENTRY( sHeadPtr, sDataPtr )
    {
        sObjectItem   = (ellObjectItem *) sDataPtr;
        sObjectHandle = & sObjectItem->mObjectHandle;

        STL_TRY( ellLock4DropSynonym( aTransID,
                                      aStmt,
                                      sObjectHandle,
                                      & sLocked,
                                      aEnv )
             == STL_SUCCESS );

        STL_TRY_THROW( sLocked == STL_TRUE, RAMP_FINISH );
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

/****************************************************************
 * Dictionary 변경 함수 
 ****************************************************************/

/**
 * @brief Synonym Descriptor를 Dictionary에 추가한다. 
 * @param[in]  aTransID          Transaction ID
 * @param[in]  aStmt             Statement
 * @param[in]  aOwnerID          Synonym 의 Owner ID
 * @param[in]  aSchemaID         Synonym 의 Schema ID
 * @param[out] aSynonymID        SynonymID
 * @param[in]  aSynonymTableID   Synonym Table ID
 * @param[in]  aSynonymName      Synonym Name
 * @param[in]  aObjectSchemaName Object Schema Name
 * @param[in]  aObjectName       Object Name 
 * @param[in]  aEnv              Environment
 * @remarks
 */
stlStatus ellInsertSynonymDesc( smlTransId           aTransID,
                                smlStatement       * aStmt,
                                stlInt64             aOwnerID,
                                stlInt64             aSchemaID,
                                stlInt64             aSynonymID,
                                stlInt64             aSynonymTableID,
                                stlChar            * aSynonymName,
                                stlChar            * aObjectSchemaName,
                                stlChar            * aObjectName,
                                ellEnv             * aEnv )
{
    /**
     * Synonym Dictionary Record 추가
     */

    STL_TRY( eldInsertSynonymDesc( aTransID,
                                   aStmt,
                                   aOwnerID,
                                   aSchemaID,
                                   aSynonymID,
                                   aSynonymTableID,
                                   aSynonymName,
                                   aObjectSchemaName,
                                   aObjectName,
                                   aEnv )
        == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}

/**
 * @brief DROP SYNONYM 를 위한 Dictionary Row 제거 
 * @param[in]  aTransID             Transaction ID
 * @param[in]  aStmt                Statement
 * @param[in]  aSynonymDictHandle   Synonym Dictionary Handle
 * @param[in]  aEnv                 Environment
 * @remarks
 */
stlStatus ellRemoveDict4DropSynonym( smlTransId        aTransID,
                                     smlStatement    * aStmt,
                                     ellDictHandle   * aSynonymDictHandle,
                                     ellEnv          * aEnv )
{
    /**
     * Dictionary Record 제거
     */
    
    STL_TRY( eldDeleteDictSynonym4Drop( aTransID,
                                        aStmt,
                                        aSynonymDictHandle,
                                        aEnv )
             == STL_SUCCESS );

    
    /**
     * Operation Memory 정리
     */
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}

/**
 * @brief CREATE OR REPLACE SYNONYM 를 위한 Dictionary Row 제거 
 * @param[in]  aTransID             Transaction ID
 * @param[in]  aStmt                Statement
 * @param[in]  aSynonymDictHandle   Synonym Dictionary Handle
 * @param[in]  aEnv                 Environment
 * @remarks
 */
stlStatus ellRemoveDict4ReplaceSynonym( smlTransId        aTransID,
                                        smlStatement    * aStmt,
                                        ellDictHandle   * aSynonymDictHandle,
                                        ellEnv          * aEnv )
{
    /**
     * Dictionary Record 제거
     */
    
    STL_TRY( eldDeleteDictSynonym4Replace( aTransID,
                                           aStmt,
                                           aSynonymDictHandle,
                                           aEnv )
             == STL_SUCCESS );

    /**
     * Operation Memory 정리
     */
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}

/**
 * @brief Synonym List 의 Dictionary Record 를 삭제한다.
 * @param[in] aTransID              aTransaction ID
 * @param[in] aStmt                 Statement
 * @param[in] aSynonymObjectList    Synonym Object List
 * @param[in] aEnv                  Environemnt
 * @remarks
 */
stlStatus ellRemoveDictSynonymByObjectList( smlTransId        aTransID,
                                            smlStatement    * aStmt,
                                            ellObjectList   * aSynonymObjectList,
                                            ellEnv          * aEnv )
{
    stlRingHead   * sHeadPtr = NULL;
    stlRingEntry  * sDataPtr = NULL;
    
    ellObjectItem * sObjectItem = NULL;
    ellDictHandle * sObjectHandle = NULL;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSynonymObjectList != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Synonym Object List 를 순회하며, Dictionary Record 를 삭제한다.
     */

    sHeadPtr = & aSynonymObjectList->mHeadList;
    sDataPtr = NULL;

    STL_RING_FOREACH_ENTRY( sHeadPtr, sDataPtr )
    {
        sObjectItem   = (ellObjectItem *) sDataPtr;
        sObjectHandle = & sObjectItem->mObjectHandle;

        STL_TRY( ellRemoveDict4DropSynonym( aTransID,
                                            aStmt,
                                            sObjectHandle,
                                            aEnv )
                 == STL_SUCCESS );
    }

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
 * @brief CREATE SYNONYM 를 위한 Cache 재구축 
 * @param[in]  aTransID     Transaction ID
 * @param[in]  aStmt        Statement
 * @param[in]  aSynonymID   Synonym ID
 * @param[in]  aEnv         Envirionment 
 * @remarks
 * 필요한 정보들이 Dictionary Table 에 저장되어 있어야 함.
 */
stlStatus ellRefineCache4CreateSynonym( smlTransId           aTransID,
                                        smlStatement       * aStmt,
                                        stlInt64             aSynonymID,
                                        ellEnv             * aEnv )
{
    STL_TRY( eldRefineCache4AddSynonym( aTransID,
                                        aStmt,
                                        aSynonymID,
                                        aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}

/**
 * @brief DROP SYNONYM 를 위한 Cache 재구성 
 * @param[in]  aTransID             Transaction ID
 * @param[in]  aStmt                Statement
 * @param[in]  aSynonymDictHandle   Synonym Dictionary Handle
 * @param[in]  aEnv                 Environment
 * @remarks
 */
stlStatus ellRefineCache4DropSynonym( smlTransId        aTransID,
                                      smlStatement    * aStmt,
                                      ellDictHandle   * aSynonymDictHandle,
                                      ellEnv          * aEnv )
{
    /**
     * SQL-Synonym Cache 를 삭제
     */

    STL_TRY( eldcDeleteCacheSynonym( aTransID,
                                     aStmt,
                                     aSynonymDictHandle,
                                     aEnv )
             == STL_SUCCESS );

    /**
     * Operation Memory 정리
     */
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/**
 * @brief Synonym List 의 Dictionary Cache 를 삭제한다.
 * @param[in] aTransID              Transaction ID
 * @param[in] aStmt                 Statement
 * @param[in] aSynonymObjectList    Synonym Object List
 * @param[in] aEnv                  Environemnt
 * @remarks
 */
stlStatus ellRefineCacheDropSynonymByObjectList( smlTransId      aTransID,
                                                 smlStatement  * aStmt,
                                                 ellObjectList * aSynonymObjectList,
                                                 ellEnv        * aEnv )
{
    stlRingHead   * sHeadPtr = NULL;
    stlRingEntry  * sDataPtr = NULL;
    
    ellObjectItem * sObjectItem = NULL;
    ellDictHandle * sObjectHandle = NULL;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSynonymObjectList != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Synonym Object List 를 순회하며, Dictionary Record 를 삭제한다.
     */

    sHeadPtr = & aSynonymObjectList->mHeadList;
    sDataPtr = NULL;

    STL_RING_FOREACH_ENTRY( sHeadPtr, sDataPtr )
    {
        sObjectItem   = (ellObjectItem *) sDataPtr;
        sObjectHandle = & sObjectItem->mObjectHandle;

        STL_TRY( ellRefineCache4DropSynonym( aTransID,
                                             aStmt,
                                             sObjectHandle,
                                             aEnv )
                 == STL_SUCCESS );
    }

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
 * @brief Schema Handle 과 Synonym Name 을 이용해 Synonym Dictionary Handle 을 얻는다.
 * @param[in]  aTransID             Transaction ID
 * @param[in]  aViewSCN             View SCN
 * @param[in]  aSchemaDictHandle    Schema Dictionary Handle
 * @param[in]  aSynonymName         Synonym Name
 * @param[out] aSynonymDictHandle   Synonym Dictionary Handle
 * @param[out] aExist               존재 여부 
 * @param[in]  aEnv                 Envirionment 
 * @remarks
 */
stlStatus ellGetSynonymDictHandleWithSchema( smlTransId           aTransID,
                                             smlScn               aViewSCN,
                                             ellDictHandle      * aSchemaDictHandle,
                                             stlChar            * aSynonymName,
                                             ellDictHandle      * aSynonymDictHandle,
                                             stlBool            * aExist,
                                             ellEnv             * aEnv )
{
    ellSchemaDesc * sSchemaDesc = NULL;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aSchemaDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSynonymName != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSynonymDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExist != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Schema Descriptor
     */

    sSchemaDesc = (ellSchemaDesc *) ellGetObjectDesc( aSchemaDictHandle );

    /**
     * Synonym Dictionary Handle 획득
     */

    ellInitDictHandle( aSynonymDictHandle );
    
    STL_TRY( eldGetSynonymHandleByName( aTransID,
                                        aViewSCN,
                                        sSchemaDesc->mSchemaID,
                                        aSynonymName,
                                        aSynonymDictHandle,
                                        aExist,
                                        aEnv )
             == STL_SUCCESS );

    if ( *aExist == STL_TRUE )
    {
        aSynonymDictHandle->mFullyQualifiedName = STL_TRUE;
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Auth Handle 과 Synonym Name 을 이용해 Synonym Dictionary Handle 을 얻는다.
 * @param[in]  aTransID             Transaction ID
 * @param[in]  aViewSCN             View SCN
 * @param[in]  aAuthDictHandle      Authorization Dictionary Handle
 * @param[in]  aSynonymName         Synonym Name
 * @param[out] aSynonymDictHandle   Synonym Dictionary Handle
 * @param[out] aExist               존재 여부 
 * @param[in]  aEnv                 Envirionment 
 * @remarks
 */
stlStatus ellGetSynonymDictHandleWithAuth( smlTransId           aTransID,
                                           smlScn               aViewSCN,
                                           ellDictHandle      * aAuthDictHandle,
                                           stlChar            * aSynonymName,
                                           ellDictHandle      * aSynonymDictHandle,
                                           stlBool            * aExist,
                                           ellEnv             * aEnv )
{
    stlInt32   i;
    
    ellAuthDesc   * sAuthDesc = NULL;

    ellDictHandle   sPublicHandle;
    ellAuthDesc   * sPublicDesc = NULL;
    
    stlBool         sExist = STL_FALSE;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aAuthDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSynonymName != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSynonymDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExist != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Authorization Descriptor
     */

    sAuthDesc = (ellAuthDesc *) ellGetObjectDesc( aAuthDictHandle );

    /**
     * Synonym Dictionary Handle 을 획득
     */
    
    ellInitDictHandle( aSynonymDictHandle );
    
    while(1)
    {
        /**
         * 사용자 Schema Path 를 이용해 검색
         */
        for ( i = 0; i < sAuthDesc->mSchemaPathCnt; i++ )
        {
            STL_TRY( eldGetSynonymHandleByName( aTransID,
                                                aViewSCN,
                                                sAuthDesc->mSchemaIDArray[i],
                                                aSynonymName,
                                                aSynonymDictHandle,
                                                & sExist,
                                                aEnv )
                     == STL_SUCCESS );
            if ( sExist == STL_TRUE )
            {
                break;
            }
            else
            {
                continue;
            }
        }

        if ( sExist == STL_TRUE )
        {
            break;
        }
        else
        {

            /**
             * PUBLIC Auth Handle 획득
             */
            
            STL_TRY( eldGetAuthHandleByID( aTransID,
                                           aViewSCN,
                                           ellGetAuthIdPUBLIC(),
                                           & sPublicHandle,
                                           & sExist,
                                           aEnv )
                     == STL_SUCCESS );
            
            STL_TRY_THROW( sExist == STL_TRUE, RAMP_ERR_OBJECT_MODIFIED );
            
            sExist = STL_FALSE;

            sPublicDesc = (ellAuthDesc *) ellGetObjectDesc( & sPublicHandle );
            
            /**
             * PUBLIC Schema Path 를 이용해 검색
             */

            for ( i = 0; i < sPublicDesc->mSchemaPathCnt; i++ )
            {
                STL_TRY( eldGetSynonymHandleByName( aTransID,
                                                    aViewSCN,
                                                    sPublicDesc->mSchemaIDArray[i],
                                                    aSynonymName,
                                                    aSynonymDictHandle,
                                                    & sExist,
                                                    aEnv )
                         == STL_SUCCESS );
                if ( sExist == STL_TRUE )
                {
                    break;
                }
                else
                {
                    continue;
                }
            }
        }

        break;
    }

    /**
     * Output 설정
     */

    *aExist = sExist;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_OBJECT_MODIFIED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ELL_ERRCODE_INVALID_OBJECT,
                      NULL,
                      ELL_ERROR_STACK(aEnv) );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}


/*********************************************************
 * 정보 획득 함수 
 *********************************************************/

/**
 * @brief New Synonym ID 획득 
 * @param[out] aSynonymID    Synonym ID 
 * @param[in]  aEnv          Environment  
 * @remarks
 */
stlStatus ellGetNewSynonymID( stlInt64 * aSynonymID,
                              ellEnv   * aEnv )
{
    STL_TRY( eldGetNewSynonymID( aSynonymID,
                                 aEnv )
             == STL_SUCCESS );
        
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief Synonym ID 획득 
 * @param[in] aSynonymDictHandle Synonym Dictionary Handle
 * @return Synonym ID
 * @remarks
 */
stlInt64 ellGetSynonymID( ellDictHandle * aSynonymDictHandle )
{
    ellSynonymDesc * sSynonymDesc = NULL;

    STL_DASSERT( aSynonymDictHandle->mObjectType == ELL_OBJECT_SYNONYM );
    
    sSynonymDesc = (ellSynonymDesc *) ellGetObjectDesc( aSynonymDictHandle );

    return sSynonymDesc->mSynonymID;
}

/**
 * @brief Synonym 의 Table ID 획득 
 * @param[in] aSynonymDictHandle Synonym Dictionary Handle
 * @return Synonym ID
 * @remarks
 * Table 과 Naming Resultion 을 위한 ID 임.
 */
stlInt64 ellGetSynonymTableID( ellDictHandle * aSynonymDictHandle )
{
    ellSynonymDesc * sSynonymDesc = NULL;

    STL_DASSERT( aSynonymDictHandle->mObjectType == ELL_OBJECT_SYNONYM );
    
    sSynonymDesc = (ellSynonymDesc *) ellGetObjectDesc( aSynonymDictHandle );

    return sSynonymDesc->mSynonymTableID;
}

/**
 * @brief Synonym 의 Owner ID 획득 
 * @param[in] aSynonymDictHandle Synonym Dictionary Handle
 * @return Synonym Owner ID
 * @remarks
 */
stlInt64 ellGetSynonymOwnerID( ellDictHandle * aSynonymDictHandle )
{
    ellSynonymDesc * sSynonymDesc = NULL;

    STL_DASSERT( aSynonymDictHandle->mObjectType == ELL_OBJECT_SYNONYM );
    
    sSynonymDesc = (ellSynonymDesc *) ellGetObjectDesc( aSynonymDictHandle );

    return sSynonymDesc->mOwnerID;
}

/**
 * @brief Synonym 의 Schema ID 획득 
 * @param[in] aSynonymDictHandle Synonym Dictionary Handle
 * @return Synonym Schema ID
 * @remarks
 */
stlInt64 ellGetSynonymSchemaID( ellDictHandle * aSynonymDictHandle )
{
    ellSynonymDesc * sSynonymDesc = NULL;

    STL_DASSERT( aSynonymDictHandle->mObjectType == ELL_OBJECT_SYNONYM );
    
    sSynonymDesc = (ellSynonymDesc *) ellGetObjectDesc( aSynonymDictHandle );

    return sSynonymDesc->mSchemaID;
}


/**
 * @brief Synonym Name 획득 
 * @param[in] aSynonymDictHandle Synonym Dictionary Handle
 * @return Synonym Name
 * @remarks
 */
stlChar * ellGetSynonymName( ellDictHandle * aSynonymDictHandle )
{
    ellSynonymDesc * sSynonymDesc = NULL;

    STL_DASSERT( aSynonymDictHandle->mObjectType == ELL_OBJECT_SYNONYM );
    
    sSynonymDesc = (ellSynonymDesc *) ellGetObjectDesc( aSynonymDictHandle );

    return sSynonymDesc->mSynonymName;
}

/**
 * @brief Synonym Object Schema Name 획득 
 * @param[in] aSynonymDictHandle Synonym Dictionary Handle
 * @return Synonym Object Schema Name
 * @remarks
 */
stlChar * ellGetSynonymObjectSchemaName( ellDictHandle * aSynonymDictHandle )
{
    ellSynonymDesc * sSynonymDesc = NULL;

    STL_DASSERT( aSynonymDictHandle->mObjectType == ELL_OBJECT_SYNONYM );
    
    sSynonymDesc = (ellSynonymDesc *) ellGetObjectDesc( aSynonymDictHandle );

    return sSynonymDesc->mObjectSchemaName;
}

/**
 * @brief Synonym Object Name 획득 
 * @param[in] aSynonymDictHandle Synonym Dictionary Handle
 * @return Synonym Object Schema Name
 * @remarks
 */
stlChar * ellGetSynonymObjectName( ellDictHandle * aSynonymDictHandle )
{
    ellSynonymDesc * sSynonymDesc = NULL;

    STL_DASSERT( aSynonymDictHandle->mObjectType == ELL_OBJECT_SYNONYM );
    
    sSynonymDesc = (ellSynonymDesc *) ellGetObjectDesc( aSynonymDictHandle );

    return sSynonymDesc->mObjectName;
}

/** @} ellObjectSynonym */
