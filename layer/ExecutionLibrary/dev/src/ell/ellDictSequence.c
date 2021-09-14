/*******************************************************************************
 * ellDictSequence.c
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
 * @file ellDictSequence.c
 * @brief Sequence Dictionary 관리 모듈
 */

#include <ell.h>
#include <elDef.h>

#include <eldt.h>
#include <eldc.h>
#include <eldDictionary.h>

#include <eldAuthorization.h>
#include <eldSequence.h>



/**
 * @addtogroup ellObjectSequence
 * @{
 */


/****************************************************************
 * DDL Lock 함수 
 ****************************************************************/

/**
 * @brief CREATE SEQUENCE 구문을 위한 DDL Lock 을 획득한다.
 * @param[in]  aTransID        Transaction ID
 * @param[in]  aStmt           Statement
 * @param[in]  aOwnerHandle    Sequence 의 Owner Handle
 * @param[in]  aSchemaHandle   Sequence 의 Schema Handle
 * @param[out] aLockSuccess    Lock 획득 여부 
 * @param[in]  aEnv            Environment
 * @remarks
 * 현재 Sequence 는 별도의 tablespace 가 아닌 SYSTEM tablespace 에 생성되고 있다.
 */
stlStatus ellLock4CreateSequence( smlTransId      aTransID,
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
 * @brief DROP SEQUENCE 구문을 위한 DDL Lock 을 획득한다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aSeqDictHandle   Sequence Dictionary Handle
 * @param[out] aLockSuccess     Lock 획득 여부 
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus ellLock4DropSequence( smlTransId      aTransID,
                                smlStatement  * aStmt,
                                ellDictHandle * aSeqDictHandle,
                                stlBool       * aLockSuccess,
                                ellEnv        * aEnv )
{
    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSeqDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aLockSuccess != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Sequence 레코드에 X lock 
     */
    
    STL_TRY( eldLockRowByObjectHandle( aTransID,
                                       aStmt,
                                       ELDT_TABLE_ID_SEQUENCES,
                                       ELDT_Sequences_ColumnOrder_SEQUENCE_ID,
                                       aSeqDictHandle,
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
 * @brief ALTER SEQUENCE 구문을 위한 DDL Lock 을 획득한다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aSeqDictHandle   Sequence Dictionary Handle
 * @param[out] aLockSuccess     Lock 획득 여부 
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus ellLock4AlterSequence( smlTransId      aTransID,
                                 smlStatement  * aStmt,
                                 ellDictHandle * aSeqDictHandle,
                                 stlBool       * aLockSuccess,
                                 ellEnv        * aEnv )
{
    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSeqDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aLockSuccess != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Sequence 레코드에 X lock 
     */
    
    STL_TRY( eldLockRowByObjectHandle( aTransID,
                                       aStmt,
                                       ELDT_TABLE_ID_SEQUENCES,
                                       ELDT_Sequences_ColumnOrder_SEQUENCE_ID,
                                       aSeqDictHandle,
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
 * @brief DDL 수행시 관련 Sequence List 에 대한 Lock 을 획득한다.
 * @param[in]  aTransID      Transaction ID
 * @param[in]  aStmt         Statement
 * @param[in]  aSeqList      Sequence Object List
 * @param[out] aLockSuccess  Lock 획득 여부 
 * @param[in]  aEnv          Environment
 * @remarks
 */
stlStatus ellLockRelatedSeqList4DDL( smlTransId      aTransID,
                                     smlStatement  * aStmt,
                                     ellObjectList * aSeqList,
                                     stlBool       * aLockSuccess,
                                     ellEnv        * aEnv )
{
    ellObjectItem * sObjectItem = NULL;
    ellDictHandle * sObjectHandle = NULL;

    stlBool   sLocked = STL_TRUE;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSeqList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aLockSuccess != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * Sequence 목록을 순회하며 lock 을 획득 
     */

    STL_RING_FOREACH_ENTRY( & aSeqList->mHeadList, sObjectItem )
    {
        sObjectHandle = & sObjectItem->mObjectHandle;

        STL_TRY( ellLock4DropSequence( aTransID,
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
 * @brief Sequence Descriptor 를 Dictionary 에 추가한다.
 * @param[in]  aTransID       Transaction ID
 * @param[in]  aStmt          Statement
 * @param[in]  aOwnerID       Sequence 의 Owner ID
 * @param[in]  aSchemaID      Sequence 의 Schema 의 ID
 * @param[out] aSequenceID    Sequence ID
 * @param[in]  aSeqTableID    Naming Resolution 을 위한 Sequence 의 Table ID
 * @param[in]  aTablespaceID  Sequence 를 생성할 Tablespace ID
 * @param[in]  aPhysicalID    Sequence 의 물리적 ID
 * @param[in]  aSequenceName  Sequence Name (null-terminated)
 * @param[in]  aStart         생성한 Sequence의 시작값
 *                       <BR> Sequence 객체와 관련 정보의 관리 주체는 Storage Manager이다.
 *                       <BR> 사용자 입력값이 없더라도 Sequence생성시 이러한 값들이 관리 주체에 의해
 *                       <BR> 설정되며, 설정된 값을 획득하여 입력 인자로 주어야 한다.
 *                       <BR> 정보의 관리 주체가 Dictionary가 아닌 경우,
 *                       <BR> Dictionary Table에서 그 정보를 관리하지 않고 있으나,
 *                       <BR> 표준에서 정의한 컬럼에 대한 값을 굳이 NULL로 설정할 필요가 없어
 *                       <BR> 예외적으로 입력을 받는다.
 *                       <BR> - aStart
 *                       <BR> - aMinimum
 *                       <BR> - aMaximum
 *                       <BR> - aIncrement
 *                       <BR> - aIsCycle
 *                       <BR> - aCacheSize
 * @param[in]  aMinimum       생성한 Sequence의 최소값 
 * @param[in]  aMaximum       생성한 Sequence의 최대값
 * @param[in]  aIncrement     생성한 Sequence의 증가값
 * @param[in]  aIsCycle       생성한 Sequence의 Cycle 허용 여부
 * @param[in]  aCacheSize     생성한 Sequence의 Cache Size
 * @param[in]  aComment       Sequence Comment (nullable)
 * @param[in]  aEnv           Execution Library Environment
 * @remarks
 */

stlStatus ellInsertSequenceDesc( smlTransId           aTransID,
                                 smlStatement       * aStmt,
                                 stlInt64             aOwnerID,
                                 stlInt64             aSchemaID,
                                 stlInt64           * aSequenceID,
                                 stlInt64             aSeqTableID,
                                 stlInt64             aTablespaceID,
                                 stlInt64             aPhysicalID,
                                 stlChar            * aSequenceName,
                                 stlInt64             aStart,
                                 stlInt64             aMinimum,
                                 stlInt64             aMaximum,
                                 stlInt64             aIncrement,
                                 stlBool              aIsCycle,
                                 stlInt32             aCacheSize,
                                 stlChar            * aComment,
                                 ellEnv             * aEnv )
{
    /**
     * Sequence Dictionary Record 추가
     */
    
    STL_TRY( eldInsertSequenceDesc( aTransID,
                                    aStmt,
                                    aOwnerID,
                                    aSchemaID,
                                    aSequenceID,
                                    aSeqTableID,
                                    aTablespaceID,
                                    aPhysicalID,
                                    aSequenceName,
                                    aStart,
                                    aMinimum,
                                    aMaximum,
                                    aIncrement,
                                    aIsCycle,
                                    aCacheSize,
                                    aComment,
                                    aEnv )
             == STL_SUCCESS );

    /**
     * Sequence Owner 가 소유할 usage descriptor 를 추가한다.
     */

    STL_TRY( ellInsertUsagePrivDesc( aTransID,
                                     aStmt,
                                     ELDT_AUTH_ID_SYSTEM,  /* Grantor */
                                     aOwnerID,             /* Grantee */
                                     aSchemaID,
                                     *aSequenceID,
                                     ELL_USAGE_OBJECT_TYPE_SEQUENCE,  /* Sequence Object */
                                     STL_TRUE,                        /* Grantable */  
                                     aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/**
 * @brief DROP SEQUENCE 를 위한 Dictionary Row 제거 
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aSeqDictHandle   Sequence Dictionary Handle
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus ellRemoveDict4DropSequence( smlTransId        aTransID,
                                      smlStatement    * aStmt,
                                      ellDictHandle   * aSeqDictHandle,
                                      ellEnv          * aEnv )
{
    /**
     * Dictionary Record 제거
     */
    
    STL_TRY( eldDeleteDictSequence( aTransID,
                                    aStmt,
                                    aSeqDictHandle,
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
 * @brief Sequence List 의 Dictionary Record 를 삭제한다.
 * @param[in] aTransID          aTransaction ID
 * @param[in] aStmt             Statement
 * @param[in] aSeqObjectList    Sequence Object List
 * @param[in] aEnv              Environemnt
 * @remarks
 */
stlStatus ellRemoveDictSeqByObjectList( smlTransId        aTransID,
                                        smlStatement    * aStmt,
                                        ellObjectList   * aSeqObjectList,
                                        ellEnv          * aEnv )
{
    ellObjectItem   * sObjectItem = NULL;
    ellDictHandle   * sObjectHandle = NULL;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSeqObjectList != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Sequence Object List 를 순회하며, Dictionary Record 를 삭제한다.
     */

    STL_RING_FOREACH_ENTRY( & aSeqObjectList->mHeadList, sObjectItem )
    {
        sObjectHandle = & sObjectItem->mObjectHandle;

        STL_TRY( ellRemoveDict4DropSequence( aTransID,
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


/**
 * @brief Sequence Option 을 Dictionary 에서 변경한다.
 * @param[in]  aTransID       Transaction ID
 * @param[in]  aStmt          Statement
 * @param[in]  aSequenceID    Sequence ID
 * @param[in]  aPhysicalID    Sequence 의 물리적 ID
 * @param[in]  aStart         변경할 Sequence의 시작값
 * @param[in]  aMinimum       변경할 Sequence의 최소값 
 * @param[in]  aMaximum       변경할 Sequence의 최대값
 * @param[in]  aIncrement     변경할 Sequence의 증가값
 * @param[in]  aIsCycle       변경할 Sequence의 Cycle 허용 여부
 * @param[in]  aCacheSize     변경할 Sequence의 Cache Size
 * @param[in]  aEnv           Execution Library Environment
 * @remarks
 */
stlStatus ellModifySequenceOption( smlTransId             aTransID,
                                   smlStatement         * aStmt,
                                   stlInt64               aSequenceID,
                                   stlInt64               aPhysicalID,
                                   stlInt64               aStart,
                                   stlInt64               aMinimum,
                                   stlInt64               aMaximum,
                                   stlInt64               aIncrement,
                                   stlBool                aIsCycle,
                                   stlInt32               aCacheSize,
                                   ellEnv               * aEnv )
{
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aSequenceID > ELL_DICT_OBJECT_ID_NA, ELL_ERROR_STACK(aEnv) );
    
    /**
     * Dictionary Record 변경
     */
    
    STL_TRY( eldModifySequenceOption( aTransID,
                                      aStmt,
                                      aSequenceID,
                                      aPhysicalID,
                                      aStart,
                                      aMinimum,
                                      aMaximum,
                                      aIncrement,
                                      aIsCycle,
                                      aCacheSize,
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

/****************************************************************
 * Cache 재구축 함수
 ****************************************************************/

/**
 * @brief CREATE SEQUENCE 를 위한 Cache 재구축 
 * @param[in]  aTransID     Transaction ID
 * @param[in]  aStmt        Statement
 * @param[in]  aAuthID      Authorization ID
 * @param[in]  aSequenceID  Sequence ID
 * @param[in]  aEnv         Envirionment 
 * @remarks
 * 필요한 정보들이 Dictionary Table 에 저장되어 있어야 함.
 */
stlStatus ellRefineCache4CreateSequence( smlTransId           aTransID,
                                         smlStatement       * aStmt,
                                         stlInt64             aAuthID,
                                         stlInt64             aSequenceID,
                                         ellEnv             * aEnv )
{
    STL_TRY( eldRefineCache4AddSequence( aTransID,
                                         aStmt,
                                         aAuthID,
                                         aSequenceID,
                                         aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/**
 * @brief DROP SEQUENCE 를 위한 Cache 재구성 
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aSeqDictHandle   Sequence Dictionary Handle
 * @param[in]  aEnv             Environment
 * @remarks
 * Drop Behavior 별 동작의 차이가 없다.
 */
stlStatus ellRefineCache4DropSequence( smlTransId        aTransID,
                                       smlStatement    * aStmt,
                                       ellDictHandle   * aSeqDictHandle,
                                       ellEnv          * aEnv )
{
    /**
     * SQL-Sequence Cache 를 삭제
     */

    STL_TRY( eldcDeleteCacheSequence( aTransID,
                                      aStmt,
                                      aSeqDictHandle,
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
 * @brief Sequence List 의 Dictionary Cache 를 삭제한다.
 * @param[in] aTransID          aTransaction ID
 * @param[in] aStmt             Statement
 * @param[in] aSeqObjectList    Sequence Object List
 * @param[in] aEnv              Environemnt
 * @remarks
 */
stlStatus ellRefineCacheDropSeqByObjectList( smlTransId        aTransID,
                                             smlStatement    * aStmt,
                                             ellObjectList   * aSeqObjectList,
                                             ellEnv          * aEnv )
{
    ellObjectItem   * sObjectItem = NULL;
    ellDictHandle   * sObjectHandle = NULL;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSeqObjectList != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Sequence Object List 를 순회하며, Dictionary Record 를 삭제한다.
     */

    STL_RING_FOREACH_ENTRY( & aSeqObjectList->mHeadList, sObjectItem )
    {
        sObjectHandle = & sObjectItem->mObjectHandle;

        STL_TRY( ellRefineCache4DropSequence( aTransID,
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


/**
 * @brief ALTER SEQUENCE 를 위한 Cache 재구성 
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aSeqDictHandle   Sequence Dictionary Handle
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus ellRefineCache4AlterSequence( smlTransId        aTransID,
                                        smlStatement    * aStmt,
                                        ellDictHandle   * aSeqDictHandle,
                                        ellEnv          * aEnv )
{
    stlInt64 sSequenceID = ELL_DICT_OBJECT_ID_NA;
    stlInt64 sOwnerID = ELL_DICT_OBJECT_ID_NA;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSeqDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * Sequence ID 획득
     */

    sSequenceID = ellGetSequenceID( aSeqDictHandle );
    sOwnerID = ellGetSequenceOwnerID( aSeqDictHandle );
    
    /**
     * SQL-Sequence Cache 를 삭제
     */

    STL_TRY( eldcDeleteCacheSequence( aTransID,
                                      aStmt,
                                      aSeqDictHandle,
                                      aEnv )
             == STL_SUCCESS );

    /**
     * SQL-Sequence Cache 를 추가
     */
    
    STL_TRY( eldRefineCache4AddSequence( aTransID,
                                         aStmt,
                                         sOwnerID,
                                         sSequenceID,
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


/****************************************************************
 * Handle 획득 함수 
 ****************************************************************/




/**
 * @brief Schema Handle 과 Sequence Name 을 이용해 Sequence Dictionary Handle 을 획득 
 * @param[in]  aTransID             Transaction ID
 * @param[in]  aViewSCN             View SCN
 * @param[in]  aSchemaDictHandle    Schema Dictionary Handle
 * @param[in]  aSequenceName        Sequence Name
 * @param[out] aSequenceDictHandle  Sequence Dictionary Handle
 * @param[out] aExist               존재 여부 
 * @param[in]  aEnv                 Envirionment 
 * @remarks
 */
stlStatus ellGetSequenceDictHandleWithSchema( smlTransId           aTransID,
                                              smlScn               aViewSCN,
                                              ellDictHandle      * aSchemaDictHandle,
                                              stlChar            * aSequenceName,
                                              ellDictHandle      * aSequenceDictHandle,
                                              stlBool            * aExist,
                                              ellEnv             * aEnv )
{
    ellSchemaDesc * sSchemaDesc = NULL;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aSchemaDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSequenceName != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSequenceDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExist != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Schema Descriptor
     */

    sSchemaDesc = (ellSchemaDesc *) ellGetObjectDesc( aSchemaDictHandle );

    /**
     * Sequence Dictionary Handle 획득
     */

    ellInitDictHandle( aSequenceDictHandle );
    
    STL_TRY( eldGetSequenceHandleByName( aTransID,
                                         aViewSCN,
                                         sSchemaDesc->mSchemaID,
                                         aSequenceName,
                                         aSequenceDictHandle,
                                         aExist,
                                         aEnv )
             == STL_SUCCESS );

    if ( *aExist == STL_TRUE )
    {
        aSequenceDictHandle->mFullyQualifiedName = STL_TRUE;
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief Auth Handle 과 Sequence Name 을 이용해 Sequence Dictionary Handle 을 얻는다.
 * @param[in]  aTransID             Transaction ID
 * @param[in]  aViewSCN             View SCN
 * @param[in]  aAuthDictHandle      Authorization Dictionary Handle
 * @param[in]  aSequenceName        Sequence Name
 * @param[out] aSequenceDictHandle  Sequence Dictionary Handle
 * @param[out] aExist               존재 여부 
 * @param[in]  aEnv                 Envirionment 
 * @remarks
 */
stlStatus ellGetSequenceDictHandleWithAuth( smlTransId           aTransID,
                                            smlScn               aViewSCN,
                                            ellDictHandle      * aAuthDictHandle,
                                            stlChar            * aSequenceName,
                                            ellDictHandle      * aSequenceDictHandle,
                                            stlBool            * aExist,
                                            ellEnv             * aEnv )
{
    stlInt32   i;
    
    ellAuthDesc * sAuthDesc = NULL;

    ellDictHandle   sPublicHandle;
    ellAuthDesc   * sPublicDesc = NULL;
    
    stlBool       sExist = STL_FALSE;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aAuthDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSequenceName != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSequenceDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExist != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Authorization Descriptor
     */

    sAuthDesc = (ellAuthDesc *) ellGetObjectDesc( aAuthDictHandle );

    /**
     * Sequence Dictionary Handle 을 획득
     */
    
    ellInitDictHandle( aSequenceDictHandle );
    
    while(1)
    {
        /**
         * 사용자 Schema Path 를 이용해 검색
         */
        for ( i = 0; i < sAuthDesc->mSchemaPathCnt; i++ )
        {
            STL_TRY( eldGetSequenceHandleByName( aTransID,
                                                 aViewSCN,
                                                 sAuthDesc->mSchemaIDArray[i],
                                                 aSequenceName,
                                                 aSequenceDictHandle,
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
                STL_TRY( eldGetSequenceHandleByName( aTransID,
                                                     aViewSCN,
                                                     sPublicDesc->mSchemaIDArray[i],
                                                     aSequenceName,
                                                     aSequenceDictHandle,
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



/**
 * @brief Auth Handle 과 Sequence Name 을 이용해 Sequence Dictionary Handle 을 얻는다.
 * @param[in]  aTransID             Transaction ID
 * @param[in]  aViewSCN             View SCN
 * @param[in]  aSequenceID          Sequence ID
 * @param[out] aSequenceDictHandle  Sequence Dictionary Handle
 * @param[out] aExist               존재 여부 
 * @param[in]  aEnv                 Envirionment 
 * @remarks
 */
stlStatus ellGetSequenceDictHandleByID( smlTransId           aTransID,
                                        smlScn               aViewSCN,
                                        stlInt64             aSequenceID,
                                        ellDictHandle      * aSequenceDictHandle,
                                        stlBool            * aExist,
                                        ellEnv             * aEnv )
{
    return eldGetSequenceHandleByID( aTransID,
                                     aViewSCN,
                                     aSequenceID,
                                     aSequenceDictHandle,
                                     aExist,
                                     aEnv );
}



/**********************************************************
 * 정보 획득 함수
 **********************************************************/




/**
 * @brief Sequence ID 획득 
 * @param[in] aSequenceDictHandle Sequence Dictionary Handle
 * @return Sequence ID
 * @remarks
 */
stlInt64     ellGetSequenceID( ellDictHandle * aSequenceDictHandle )
{
    ellSequenceDesc * sSequenceDesc = NULL;

    STL_DASSERT( aSequenceDictHandle->mObjectType == ELL_OBJECT_SEQUENCE );
    
    sSequenceDesc = (ellSequenceDesc *) ellGetObjectDesc( aSequenceDictHandle );

    return sSequenceDesc->mSequenceID;
}

/**
 * @brief Sequence 의 Table ID 획득 
 * @param[in] aSequenceDictHandle Sequence Dictionary Handle
 * @return Sequence ID
 * @remarks
 * Table 과 Naming Resultion 을 위한 ID 임.
 */
stlInt64     ellGetSeqTableID( ellDictHandle * aSequenceDictHandle )
{
    ellSequenceDesc * sSequenceDesc = NULL;

    STL_DASSERT( aSequenceDictHandle->mObjectType == ELL_OBJECT_SEQUENCE );
    
    sSequenceDesc = (ellSequenceDesc *) ellGetObjectDesc( aSequenceDictHandle );

    return sSequenceDesc->mSeqTableID;
}

/**
 * @brief Sequence 의 Owner ID 획득 
 * @param[in] aSequenceDictHandle Sequence Dictionary Handle
 * @return Sequence Owner ID
 * @remarks
 */
stlInt64     ellGetSequenceOwnerID( ellDictHandle * aSequenceDictHandle )
{
    ellSequenceDesc * sSequenceDesc = NULL;

    STL_DASSERT( aSequenceDictHandle->mObjectType == ELL_OBJECT_SEQUENCE );
    
    sSequenceDesc = (ellSequenceDesc *) ellGetObjectDesc( aSequenceDictHandle );

    return sSequenceDesc->mOwnerID;
}


/**
 * @brief Sequence 의 Schema ID 획득 
 * @param[in] aSequenceDictHandle Sequence Dictionary Handle
 * @return Sequence Schema ID
 * @remarks
 */
stlInt64     ellGetSequenceSchemaID( ellDictHandle * aSequenceDictHandle )
{
    ellSequenceDesc * sSequenceDesc = NULL;

    STL_DASSERT( aSequenceDictHandle->mObjectType == ELL_OBJECT_SEQUENCE );
    
    sSequenceDesc = (ellSequenceDesc *) ellGetObjectDesc( aSequenceDictHandle );

    return sSequenceDesc->mSchemaID;
}


/**
 * @brief Sequence Name 정보
 * @param[in] aSequenceDictHandle Sequence Dictionary Handle
 * @return Sequence Name
 * @remarks
 */
stlChar * ellGetSequenceName( ellDictHandle * aSequenceDictHandle )
{
    ellSequenceDesc * sSequenceDesc = NULL;

    STL_DASSERT( aSequenceDictHandle->mObjectType == ELL_OBJECT_SEQUENCE );
    
    sSequenceDesc = (ellSequenceDesc *) ellGetObjectDesc( aSequenceDictHandle );

    return sSequenceDesc->mSequenceName;
}


/**
 * @brief Sequence Handle 정보
 * @param[in] aSequenceDictHandle Sequence Dictionary Handle
 * @return 물리적 Sequence Handle
 * @remarks
 */
void * ellGetSequenceHandle( ellDictHandle * aSequenceDictHandle )
{
    ellSequenceDesc * sSequenceDesc = NULL;

    STL_DASSERT( aSequenceDictHandle->mObjectType == ELL_OBJECT_SEQUENCE );
    
    sSequenceDesc = (ellSequenceDesc *) ellGetObjectDesc( aSequenceDictHandle );

    return sSequenceDesc->mSequenceHandle;
}


/**
 * @brief Sequence 객체가 Built-In Object 인지의 여부
 * @param[in]  aSequenceDictHandle  Dictionary Handle
 * @remarks
 */
stlBool ellIsBuiltInSequence( ellDictHandle * aSequenceDictHandle )
{
    ellSequenceDesc * sSequenceDesc = NULL;

    STL_DASSERT( aSequenceDictHandle->mObjectType == ELL_OBJECT_SEQUENCE );
    
    sSequenceDesc = (ellSequenceDesc *) ellGetObjectDesc( aSequenceDictHandle );

    if ( sSequenceDesc->mOwnerID == ELDT_AUTH_ID_SYSTEM )
    {
        return STL_TRUE;
    }
    else
    {
        return STL_FALSE;
    }
}

/** @} ellObjectSequence */
