/*******************************************************************************
 * ellDictPublicSynonym.c
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
 * @file ellDictPublicSynonym.c
 * @brief Synonym Dictionary 관리 모듈
 */

#include <ell.h>
#include <elDef.h>

#include <eldt.h>
#include <eldc.h>
#include <eldDictionary.h>

#include <eldAuthorization.h>
#include <eldPublicSynonym.h>

/**
 * @addtogroup ellObjectPublicSynonym
 * @{
 */

/****************************************************************
 * DDL Lock 함수 
 ****************************************************************/

/**
 * @brief CREATE PUBLIC SYNONYM 구문을 위한 DDL Lock 을 획득한다.
 * @param[in]  aTransID        Transaction ID
 * @param[in]  aStmt           Statement
 * @param[in]  aCreatorHandle  Public Synonym 의 Creator Handle
 * @param[out] aLockSuccess    Lock 획득 여부 
 * @param[in]  aEnv            Environment
 * @remarks
 */
stlStatus ellLock4CreatePublicSynonym( smlTransId      aTransID,
                                       smlStatement  * aStmt,
                                       ellDictHandle * aCreatorHandle,
                                       stlBool       * aLockSuccess,
                                       ellEnv        * aEnv )
{
    stlBool sLocked = STL_TRUE;
    
    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCreatorHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aLockSuccess != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * Creator Record 에 S lock 
     */
        
    STL_TRY( eldLockRowByObjectHandle( aTransID,
                                       aStmt,
                                       ELDT_TABLE_ID_AUTHORIZATIONS,
                                       ELDT_Auth_ColumnOrder_AUTH_ID,
                                       aCreatorHandle,
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
 * @brief DROP PUBLIC SYNONYM 구문을 위한 DDL Lock 을 획득한다.
 * @param[in]  aTransID             Transaction ID
 * @param[in]  aStmt                Statement
 * @param[in]  aSynonymDictHandle   Synonym Dictionary Handle
 * @param[out] aLockSuccess         Lock 획득 여부 
 * @param[in]  aEnv                 Environment
 * @remarks
 */
stlStatus ellLock4DropPublicSynonym( smlTransId      aTransID,
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
     * Public Synonym 레코드에 X lock 
     */
    
    STL_TRY( eldLockRowByObjectHandle( aTransID,
                                       aStmt,
                                       ELDT_TABLE_ID_PUBLIC_SYNONYMS,
                                       ELDT_Public_Synonyms_ColumnOrder_SYNONYM_ID,
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


/****************************************************************
 * Dictionary 변경 함수 
 ****************************************************************/

/**
 * @brief Public Synonym Descriptor를 Dictionary에 추가한다. 
 * @param[in]  aTransID          Transaction ID
 * @param[in]  aStmt             Statement
 * @param[out] aSynonymID        SynonymID
 * @param[in]  aSynonymName      Synonym Name
 * @param[in]  aCreatorID        Creator ID
 * @param[in]  aObjectSchemaName Object Schema Name
 * @param[in]  aObjectName       Object Name 
 * @param[in]  aEnv              Environment
 * @remarks
 */
stlStatus ellInsertPublicSynonymDesc( smlTransId           aTransID,
                                      smlStatement       * aStmt,
                                      stlInt64             aSynonymID,
                                      stlChar            * aSynonymName,
                                      stlInt64             aCreatorID,
                                      stlChar            * aObjectSchemaName,
                                      stlChar            * aObjectName,
                                      ellEnv             * aEnv )
{
    /**
     * Synonym Dictionary Record 추가
     */
    
    STL_TRY( eldInsertPublicSynonymDesc( aTransID,
                                         aStmt,
                                         aSynonymID,
                                         aSynonymName,
                                         aCreatorID,
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
 * @brief DROP PUBLIC SYNONYM 를 위한 Dictionary Row 제거 
 * @param[in]  aTransID             Transaction ID
 * @param[in]  aStmt                Statement
 * @param[in]  aSynonymDictHandle   Synonym Dictionary Handle
 * @param[in]  aEnv                 Environment
 * @remarks
 */
stlStatus ellRemoveDict4DropPublicSynonym( smlTransId        aTransID,
                                           smlStatement    * aStmt,
                                           ellDictHandle   * aSynonymDictHandle,
                                           ellEnv          * aEnv )
{
    /**
     * Dictionary Record 제거
     */
              
    STL_TRY( eldDeleteDictPublicSynonym( aTransID,
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

/*********************************************************
 * Cache 재구축 함수 
 *********************************************************/

/**
 * @brief CREATE PUBLIC SYNONYM 를 위한 Cache 재구축 
 * @param[in]  aTransID     Transaction ID
 * @param[in]  aStmt        Statement
 * @param[in]  aSynonymID   Synonym ID
 * @param[in]  aEnv         Envirionment 
 * @remarks
 * 필요한 정보들이 Dictionary Table 에 저장되어 있어야 함.
 */
stlStatus ellRefineCache4CreatePublicSynonym( smlTransId           aTransID,
                                              smlStatement       * aStmt,
                                              stlInt64             aSynonymID,
                                              ellEnv             * aEnv )
{
    STL_TRY( eldRefineCache4AddPublicSynonym( aTransID,
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
 * @brief DROP PUBLIC SYNONYM 를 위한 Cache 재구성 
 * @param[in]  aTransID             Transaction ID
 * @param[in]  aStmt                Statement
 * @param[in]  aSynonymDictHandle   Synonym Dictionary Handle
 * @param[in]  aEnv                 Environment
 * @remarks
 */       
stlStatus ellRefineCache4DropPublicSynonym( smlTransId        aTransID,
                                            smlStatement    * aStmt,
                                            ellDictHandle   * aSynonymDictHandle,
                                            ellEnv          * aEnv )
{
    /**
     * SQL-PublicSynonym Cache 를 삭제
     */

    STL_TRY( eldcDeleteCachePublicSynonym( aTransID,
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


/*********************************************************
 * Handle 획득 함수 
 *********************************************************/

/**
 * @brief Synonym Name 을 이용해 Public Synonym Dictionary Handle 을 획득 
 * @param[in]  aTransID            Transaction ID
 * @param[in]  aViewSCN            View SCN
 * @param[in]  aSynonymName        Public Synonym Name
 * @param[out] aSynonymDictHandle  Public Synonym Dictionary Handle
 * @param[out] aExist              존재 여부 
 * @param[in]  aEnv                Envirionment 
 * @remarks
 */
stlStatus ellGetPublicSynonymDictHandle( smlTransId           aTransID,
                                         smlScn               aViewSCN,
                                         stlChar            * aSynonymName,
                                         ellDictHandle      * aSynonymDictHandle,
                                         stlBool            * aExist,
                                         ellEnv             * aEnv )
{
   
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aSynonymName != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSynonymDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExist != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Public Synonym Dictionary Handle 획득
     */

    ellInitDictHandle( aSynonymDictHandle );
    
    STL_TRY( eldGetPublicSynonymHandleByName( aTransID,
                                              aViewSCN,
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

/*********************************************************
 * 정보 획득 함수 
 *********************************************************/

  /**
 * @brief New Public Synonym ID 획득 
 * @param[out] aSynonymID    Synonym ID 
 * @param[in]  aEnv          Environment   
 * @remarks
 */
stlStatus ellGetNewPublicSynonymID( stlInt64 * aSynonymID,
                                    ellEnv   * aEnv )
{
    STL_TRY( eldGetNewPublicSynonymID( aSynonymID,
                                       aEnv )
             == STL_SUCCESS );
        
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief Public Synonym ID 획득 
 * @param[in] aPublicSynonymDictHandle Public Synonym Dictionary Handle
 * @return Synonym ID
 * @remarks
 */
stlInt64 ellGetPublicSynonymID( ellDictHandle * aPublicSynonymDictHandle )
{
    ellPublicSynonymDesc * sPublicSynonymDesc = NULL;

    STL_DASSERT( aPublicSynonymDictHandle->mObjectType == ELL_OBJECT_PUBLIC_SYNONYM );
    
    sPublicSynonymDesc = (ellPublicSynonymDesc *) ellGetObjectDesc( aPublicSynonymDictHandle );

    return sPublicSynonymDesc->mSynonymID;
}

/**
 * @brief Public Synonym Name 획득 
 * @param[in] aPublicSynonymDictHandle Synonym Dictionary Handle
 * @return Public Synonym Name
 * @remarks
 */
stlChar * ellGetPublicSynonymName( ellDictHandle * aPublicSynonymDictHandle )
{
    ellPublicSynonymDesc * sPublicSynonymDesc = NULL;

    STL_DASSERT( aPublicSynonymDictHandle->mObjectType == ELL_OBJECT_PUBLIC_SYNONYM );
    
    sPublicSynonymDesc = (ellPublicSynonymDesc *) ellGetObjectDesc( aPublicSynonymDictHandle );

    return sPublicSynonymDesc->mSynonymName;
}

/**
 * @brief Public Synonym Object Schema Name 획득 
 * @param[in] aSynonymDictHandle Synonym Dictionary Handle
 * @return Public Synonym Object Schema Name
 * @remarks
 */
stlChar * ellGetPublicSynonymObjectSchemaName( ellDictHandle * aSynonymDictHandle )
{
    ellPublicSynonymDesc * sPublicSynonymDesc = NULL;

    STL_DASSERT( aSynonymDictHandle->mObjectType == ELL_OBJECT_PUBLIC_SYNONYM );
    
    sPublicSynonymDesc = (ellPublicSynonymDesc *) ellGetObjectDesc( aSynonymDictHandle );

    return sPublicSynonymDesc->mObjectSchemaName;
}

/**
 * @brief Public Synonym Object Name 획득 
 * @param[in] aSynonymDictHandle Synonym Dictionary Handle
 * @return Public Synonym Object Schema Name
 * @remarks
 */
stlChar * ellGetPublicSynonymObjectName( ellDictHandle * aSynonymDictHandle )
{
    ellPublicSynonymDesc * sPublicSynonymDesc = NULL;

    STL_DASSERT( aSynonymDictHandle->mObjectType == ELL_OBJECT_PUBLIC_SYNONYM );
    
    sPublicSynonymDesc = (ellPublicSynonymDesc *) ellGetObjectDesc( aSynonymDictHandle );

    return sPublicSynonymDesc->mObjectName;
}


/** @} ellObjectPublicSynonym */
