/*******************************************************************************
 * ellDictTable.c
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
 * @file ellDictTable.c
 * @brief Table Dictionary 관리 모듈
 */

#include <ell.h>
#include <elDef.h>

#include <eldt.h>
#include <eldc.h>
#include <eldDictionary.h>

#include <eldTable.h>
#include <eldAuthorization.h>


/**
 * @addtogroup ellObjectTable
 * @{
 */

/**
 * @brief Table Type 에 대한 String 상수
 */
const stlChar * const gEllTableTypeString[ELL_TABLE_TYPE_MAX] =
{
    "N/A",               /* ELL_TABLE_TYPE_NOT_AVAILABLE */
    
    "BASE TABLE",        /* ELL_TABLE_TYPE_BASE_TABLE */
    "VIEW",              /* ELL_TABLE_TYPE_VIEW */
    "GLOBAL TEMPORARY",  /* ELL_TABLE_TYPE_GLOBAL_TEMPORARY */
    "LOCAL TEMPORARY",   /* ELL_TABLE_TYPE_LOCAL_TEMPORARY */
    "SYSTEM VERSIONED",  /* ELL_TABLE_TYPE_SYSTEM_VERSIONED */
    "SEQUENCE",          /* ELL_TABLE_TYPE_SEQUENCE */
    "FIXED TABLE",       /* ELL_TABLE_TYPE_FIXED_TABLE */
    "DUMP TABLE",        /* ELL_TABLE_TYPE_FIXED_TABLE */
    "SYNONYM",           /* ELL_TABLE_TYPE_SYNONYM */
};




/*********************************************************
 * DDL 관련 Object 획득 함수
 *********************************************************/


/**
 * @brief DROP TABLE 에 의해 영향을 받는 Object Handle List 를 획득
 * @param[in]  aTransID                   Transaction ID
 * @param[in]  aStmt                      Statement
 * @param[in]  aTableDictHandle           Table Dictionary Handle
 * @param[in]  aRegionMem                 Region Memory
 * @param[out] aInnerForeignKeyList       Inner Foreign Key List
 * @param[out] aOuterChildForeignKeyList  Outer Child Foreign Key List
 * @param[in]  aEnv                       Environment
 * @remarks
 */
stlStatus ellGetObjectList4DropTable( smlTransId          aTransID,
                                      smlStatement      * aStmt,
                                      ellDictHandle     * aTableDictHandle,
                                      knlRegionMem      * aRegionMem,
                                      ellObjectList    ** aInnerForeignKeyList,
                                      ellObjectList    ** aOuterChildForeignKeyList,
                                      ellEnv            * aEnv )
{
    ellObjectList * sForeignKeyList = NULL;

    ellObjectList * sParentUniqueList = NULL;
    
    ellObjectList * sPKList = NULL;
    ellObjectList * sUKList = NULL;
    
    ellObjectList * sOuterChildForeignKeyList = NULL;
    ellObjectList * sChildForeignKeyList = NULL;
    
    stlInt64        sTableID = ELL_DICT_OBJECT_ID_NA;

    ellObjectItem   * sObjectItem = NULL;
    ellDictHandle   * sObjectHandle = NULL;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInnerForeignKeyList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOuterChildForeignKeyList != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득 
     */

    sTableID = ellGetTableID( aTableDictHandle );

    /**********************************************
     * Table 의 Foreign Key List 획득 
     **********************************************/

    STL_TRY( ellInitObjectList( & sForeignKeyList, aRegionMem, aEnv ) == STL_SUCCESS );

    STL_TRY( ellGetTableConstTypeList( aTransID,
                                       aStmt,
                                       sTableID,
                                       ELL_TABLE_CONSTRAINT_TYPE_FOREIGN_KEY,
                                       aRegionMem,
                                       & sForeignKeyList,
                                       aEnv )
             == STL_SUCCESS );
    
    /**********************************************
     * Table 을 참조하는 Outer Child Foreign Key 획득 
     **********************************************/

    STL_TRY( ellInitObjectList( & sParentUniqueList, aRegionMem, aEnv ) == STL_SUCCESS );

    /**
     * Primary Key List 획득 
     */

    STL_TRY( ellGetTableConstTypeList( aTransID,
                                       aStmt,
                                       sTableID,
                                       ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY,
                                       aRegionMem,
                                       & sPKList,
                                       aEnv )
             == STL_SUCCESS );

    ellMergeObjectList( sParentUniqueList, sPKList );
    
    /**
     * Unique Key List 획득 
     */

    STL_TRY( ellGetTableConstTypeList( aTransID,
                                       aStmt,
                                       sTableID,
                                       ELL_TABLE_CONSTRAINT_TYPE_UNIQUE_KEY,
                                       aRegionMem,
                                       & sUKList,
                                       aEnv )
             == STL_SUCCESS );
    
    ellMergeObjectList( sParentUniqueList, sUKList );

    /**
     * Parent Unique List 로부터 Child Foreign Key List 획득
     */

    STL_TRY( ellGetChildForeignKeyListByUniqueList( aTransID,
                                                    aStmt,
                                                    sParentUniqueList,
                                                    aRegionMem,
                                                    & sChildForeignKeyList,
                                                    aEnv )
             == STL_SUCCESS );

    /**
     * Child Foreign Key List 로부터 Outer Child Foreign Key 획득 
     */

    STL_TRY( ellInitObjectList( & sOuterChildForeignKeyList, aRegionMem, aEnv ) == STL_SUCCESS );
    
    STL_RING_FOREACH_ENTRY( & sChildForeignKeyList->mHeadList, sObjectItem )
    {
        sObjectHandle = & sObjectItem->mObjectHandle;

        if ( ellGetConstraintTableID( sObjectHandle ) == sTableID )
        {
            /**
             * Self Child Foreign Key 임
             * - nothing to do
             */
        }
        else
        {
            /**
             * Outer Child Foreign Key 임
             */
            STL_TRY( ellAddNewObjectItem( sOuterChildForeignKeyList,
                                          sObjectHandle,
                                          NULL,
                                          aRegionMem,
                                          aEnv )
                     == STL_SUCCESS );
        }
    }
                                           
    /**
     * Output 설정
     */

    *aInnerForeignKeyList      = sForeignKeyList;
    *aOuterChildForeignKeyList = sOuterChildForeignKeyList;
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/**
 * @brief TRUNCATE TABLE 에 의해 영향을 받는 Object Handle List 를 획득
 * @param[in]  aTransID                    Transaction ID
 * @param[in]  aStmt                       Statement
 * @param[in]  aTableDictHandle            Table Dictionary Handle
 * @param[in]  aRegionMem                       Region Memory
 * @param[out] aEnableOuterChildForeignKeyList  Enabled Outer Child Foreign Key List
 * @param[in]  aEnv                             Environment
 * @remarks
 * DROP TABLE 과 달리 Eabled Outer Child Foreign Key 목록(하나만 구해도 됨)을 구해야 함.
 * @todo CASCADE 옵션이 있으므로 다시 고민해야 함.
 */
stlStatus ellGetObjectList4TruncateTable( smlTransId          aTransID,
                                          smlStatement      * aStmt,
                                          ellDictHandle     * aTableDictHandle,
                                          knlRegionMem      * aRegionMem,
                                          ellObjectList    ** aEnableOuterChildForeignKeyList,
                                          ellEnv            * aEnv )

{
    ellObjectList * sForeignKeyList = NULL;
    

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aEnableOuterChildForeignKeyList != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * @todo
     * Table 을 참조하는 Enabled Outer Child Foreign Key List 를 획득한다.
     */

    STL_TRY( ellInitObjectList( & sForeignKeyList, aRegionMem, aEnv ) == STL_SUCCESS );

    /**
     * Output 설정
     */

    *aEnableOuterChildForeignKeyList  = sForeignKeyList;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}





/*********************************************************
 * Table 에 대한 DDL Lock 함수 
 *********************************************************/

/**
 * @brief CREATE TABLE 구문을 위한 DDL Lock 을 획득한다.
 * @param[in]  aTransID        Transaction ID
 * @param[in]  aStmt           Statement
 * @param[in]  aOwnerHandle    Owner Dictionary Handle
 * @param[in]  aSchemaHandle   Schema Dictionary Handle
 * @param[in]  aSpaceHandle    Tablespace Dictionary Handle
 * @param[in]  aConstSchemaList  Constraint Schema Handle List
 * @param[in]  aConstSpaceList   Constraint Tablespace Handle List
 * @param[out] aLockSuccess    Lock 획득 여부 
 * @param[in]  aEnv            Environment
 * @remarks
 */
stlStatus ellLock4CreateTable( smlTransId      aTransID,
                               smlStatement  * aStmt,
                               ellDictHandle * aOwnerHandle,
                               ellDictHandle * aSchemaHandle,
                               ellDictHandle * aSpaceHandle,
                               ellObjectList * aConstSchemaList,
                               ellObjectList * aConstSpaceList,
                               stlBool       * aLockSuccess,
                               ellEnv        * aEnv )
{
    ellObjectItem   * sObjectItem = NULL;
    ellDictHandle   * sObjectHandle = NULL;

    stlBool   sLocked = STL_TRUE;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOwnerHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSchemaHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSpaceHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstSchemaList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstSpaceList != NULL, ELL_ERROR_STACK(aEnv) );
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
     * Tablespace Record 에 S lock
     */
    
    STL_TRY( eldLockRowByObjectHandle( aTransID,
                                       aStmt,
                                       ELDT_TABLE_ID_TABLESPACES,
                                       ELDT_Space_ColumnOrder_TABLESPACE_ID,
                                       aSpaceHandle,
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
     * Constraint Schema List 에 S lock
     */
    
    STL_RING_FOREACH_ENTRY( & aConstSchemaList->mHeadList, sObjectItem )
    {
        sObjectHandle = & sObjectItem->mObjectHandle;

        STL_TRY( eldLockRowByObjectHandle( aTransID,
                                           aStmt,
                                           ELDT_TABLE_ID_SCHEMATA,
                                           ELDT_Schemata_ColumnOrder_SCHEMA_ID,
                                           sObjectHandle,
                                           SML_LOCK_S,
                                           & sLocked,
                                           aEnv )
             == STL_SUCCESS );
        
        STL_TRY_THROW( sLocked == STL_TRUE, RAMP_FINISH );
    }

    /**
     * Constraint TableSpace List 에 S lock
     */

    STL_RING_FOREACH_ENTRY( & aConstSpaceList->mHeadList, sObjectItem )
    {
        sObjectHandle = & sObjectItem->mObjectHandle;

        STL_TRY( eldLockRowByObjectHandle( aTransID,
                                           aStmt,
                                           ELDT_TABLE_ID_TABLESPACES,
                                           ELDT_Space_ColumnOrder_TABLESPACE_ID,
                                           sObjectHandle,
                                           SML_LOCK_S,
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

/**
 * @brief DROP TABLE 구문을 위한 DDL Lock 을 획득한다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aTableDictHandle Table Dictionary Handle
 * @param[out] aLockSuccess     Lock 획득 여부 
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus ellLock4DropTable( smlTransId      aTransID,
                             smlStatement  * aStmt,
                             ellDictHandle * aTableDictHandle,
                             stlBool       * aLockSuccess,
                             ellEnv        * aEnv )
{
    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aTableDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aLockSuccess != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Table 객체에 X lock
     */
    
    STL_TRY( eldLockTable4DDL( aTransID,
                               aStmt,
                               SML_LOCK_X,
                               aTableDictHandle,
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
 * @brief TRUNCATE TABLE 구문을 위한 DDL Lock 을 획득한다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aTableDictHandle Table Dictionary Handle
 * @param[out] aLockSuccess     Lock 획득 여부 
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus ellLock4TruncateTable( smlTransId      aTransID,
                                 smlStatement  * aStmt,
                                 ellDictHandle * aTableDictHandle,
                                 stlBool       * aLockSuccess,
                                 ellEnv        * aEnv )
{
    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aLockSuccess != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Table 객체에 X lock
     */
    
    STL_TRY( eldLockTable4DDL( aTransID,
                               aStmt,
                               SML_LOCK_X,
                               aTableDictHandle,
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
 * @brief ALTER TABLE .. RENAME TO .. 구문을 위한 DDL Lock 을 획득한다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aTableDictHandle Table Dictionary Handle
 * @param[out] aLockSuccess     Lock 획득 여부 
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus ellLock4RenameTable( smlTransId      aTransID,
                               smlStatement  * aStmt,
                               ellDictHandle * aTableDictHandle,
                               stlBool       * aLockSuccess,
                               ellEnv        * aEnv )
{
    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aTableDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aLockSuccess != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Table 객체에 X lock
     */
    
    STL_TRY( eldLockTable4DDL( aTransID,
                               aStmt,
                               SML_LOCK_X,
                               aTableDictHandle,
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
 * @brief ALTER TABLE .. STORAGE .. 구문을 위한 DDL Lock 을 획득한다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aTableDictHandle Table Dictionary Handle
 * @param[out] aLockSuccess     Lock 획득 여부 
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus ellLock4AlterPhysicalAttr( smlTransId      aTransID,
                                     smlStatement  * aStmt,
                                     ellDictHandle * aTableDictHandle,
                                     stlBool       * aLockSuccess,
                                     ellEnv        * aEnv )
{
    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aTableDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aLockSuccess != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Table 객체에 X lock
     */
    
    STL_TRY( eldLockTable4DDL( aTransID,
                               aStmt,
                               SML_LOCK_X,
                               aTableDictHandle,
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
 * @brief ALTER TABLE .. ADD/DROP SUPPLEMENTAL LOG 구문을 위한 DDL Lock 을 획득한다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aTableDictHandle Table Dictionary Handle
 * @param[out] aLockSuccess     Lock 획득 여부 
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus ellLock4TableSuppLog( smlTransId      aTransID,
                                smlStatement  * aStmt,
                                ellDictHandle * aTableDictHandle,
                                stlBool       * aLockSuccess,
                                ellEnv        * aEnv )
{
    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aTableDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aLockSuccess != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Table 객체에 X lock
     */
    
    STL_TRY( eldLockTable4DDL( aTransID,
                               aStmt,
                               SML_LOCK_X,
                               aTableDictHandle,
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
 * @brief DDL 수행시 관련 Table List 에 대한 Lock 을 획득한다.
 * @param[in]  aTransID       Transaction ID
 * @param[in]  aStmt          Statement
 * @param[in]  aLockMode      SML_LOCK_X or SML_LOCK_S
 * @param[in]  aTableList     Table Object List
 * @param[out] aLockSuccess   Lock 획득 여부 
 * @param[in]  aEnv           Environment
 * @remarks
 */
stlStatus ellLockRelatedTableList4DDL( smlTransId      aTransID,
                                       smlStatement  * aStmt,
                                       stlInt32        aLockMode,
                                       ellObjectList * aTableList,
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
    STL_PARAM_VALIDATE( aTableList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aLockSuccess != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * Table 목록을 순회하며 lock 을 획득 
     */

    STL_RING_FOREACH_ENTRY( & aTableList->mHeadList, sObjectItem )
    {
        sObjectHandle = & sObjectItem->mObjectHandle;

        STL_TRY( eldLockTable4DDL( aTransID,
                                   aStmt,
                                   aLockMode,
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



/**
 * @brief View DDL 수행시 관련 Underlying Table List 에 대한 Lock 을 획득한다.
 * @param[in]  aTransID       Transaction ID
 * @param[in]  aStmt          Statement
 * @param[in]  aLockMode      SML_LOCK_X or SML_LOCK_S
 * @param[in]  aTableList     Table Object List
 * @param[out] aLockSuccess   Lock 획득 여부 
 * @param[in]  aEnv           Environment
 * @remarks
 */
stlStatus ellLockUnderlyingTableList4ViewDDL( smlTransId      aTransID,
                                              smlStatement  * aStmt,
                                              stlInt32        aLockMode,
                                              ellObjectList * aTableList,
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
    STL_PARAM_VALIDATE( aTableList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aLockSuccess != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * Table 목록을 순회하며 lock 을 획득 
     */

    STL_RING_FOREACH_ENTRY( & aTableList->mHeadList, sObjectItem )
    {
        sObjectHandle = & sObjectItem->mObjectHandle;

        if ( (ellGetTableType( sObjectHandle ) == ELL_TABLE_TYPE_BASE_TABLE) ||
             (ellGetTableType( sObjectHandle ) == ELL_TABLE_TYPE_VIEW) )
        {
            STL_TRY( eldLockTable4DDL( aTransID,
                                       aStmt,
                                       aLockMode,
                                       sObjectHandle,
                                       & sLocked,
                                       aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            /**
             * Fixed Table 이나 Dump Table 인 경우
             */

            STL_DASSERT( ( ellGetTableType( sObjectHandle ) == ELL_TABLE_TYPE_FIXED_TABLE ) ||
                         ( ellGetTableType( sObjectHandle ) == ELL_TABLE_TYPE_DUMP_TABLE ) );
                
            sLocked = STL_TRUE;
        }
        
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


/*********************************************************
 * DDL 관련 Index 획득 함수
 *********************************************************/


/**
 * @brief DROP TABLE 에 의해 영향을 받는 Prime Index Object List 를 획득
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aTableDictHandle Table Dictionary Handle
 * @param[in]  aRegionMem       Region Memory
 * @param[in]  aIndexObjectList 영향을 받는 Index Object Handle List
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus ellGetPrimeIndexList4DropTable( smlTransId          aTransID,
                                          smlStatement      * aStmt,
                                          ellDictHandle     * aTableDictHandle,
                                          knlRegionMem      * aRegionMem,
                                          ellObjectList    ** aIndexObjectList,
                                          ellEnv            * aEnv )
{
    ellObjectList    * sPrimeIndexList = NULL;

    ellObjectList    * sIndexList = NULL;
    ellObjectList    * sForeignKeyList = NULL;
    
    stlInt64           sTableID = ELL_DICT_OBJECT_ID_NA;

    stlBool            sIsForeignKeyIndex = STL_FALSE;
    
    ellObjectItem * sIndexItem = NULL;
    ellDictHandle * sIndexHandle = NULL;

    ellObjectItem * sKeyItem = NULL;
    ellDictHandle * sKeyHandle = NULL;
    ellDictHandle * sKeyIndexHandle = NULL;
    
    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIndexObjectList != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Table ID 획득
     */
    
    sTableID = ellGetTableID( aTableDictHandle );

    /**
     * Table 과 관련된 Index 정보 획득
     * - SELECT INDEX_ID
     * - FROM DEFINITION_SCHEMA.INDEX_KEY_TABLE_USAGE
     * - WHERE TABLE_ID = sTableID
     */

    STL_TRY( ellInitObjectList( & sIndexList, aRegionMem, aEnv ) == STL_SUCCESS );
    
    STL_TRY( eldGetObjectList( aTransID,
                               aStmt,
                               ELDT_TABLE_ID_INDEX_KEY_TABLE_USAGE,
                               ELL_OBJECT_INDEX,
                               ELDT_IndexTable_ColumnOrder_INDEX_ID,
                               ELDT_IndexTable_ColumnOrder_TABLE_ID,
                               sTableID,
                               aRegionMem,
                               sIndexList,
                               aEnv )
             == STL_SUCCESS );

    /**
     * Table 과 관련된 Foreign Key 정보 획득 
     */

    STL_TRY( ellGetTableConstTypeList( aTransID,
                                       aStmt,
                                       sTableID,
                                       ELL_TABLE_CONSTRAINT_TYPE_FOREIGN_KEY,
                                       aRegionMem,
                                       & sForeignKeyList,
                                       aEnv )
             == STL_SUCCESS );

    /**
     * Index List 에서 Foreign Key 와 관련된 Index 는 제거
     */

    STL_TRY( ellInitObjectList( & sPrimeIndexList, aRegionMem, aEnv ) == STL_SUCCESS );
    
    STL_RING_FOREACH_ENTRY( & sIndexList->mHeadList, sIndexItem )
    {
        sIndexHandle = & sIndexItem->mObjectHandle;

        /**
         * Foreign Key 용 Index 인지 검사
         */
        
        sIsForeignKeyIndex = STL_FALSE;

        STL_RING_FOREACH_ENTRY( & sForeignKeyList->mHeadList, sKeyItem )
        {
            sKeyHandle = & sKeyItem->mObjectHandle;

            sKeyIndexHandle = ellGetConstraintIndexDictHandle(sKeyHandle);

            if ( ellGetIndexID( sIndexHandle ) == ellGetIndexID( sKeyIndexHandle ) )
            {
                /**
                 * 해당 Index 가 Foreign Key 인덱스임
                 */

                sIsForeignKeyIndex = STL_TRUE;
                break;
            }
            else
            {
                continue;
            }
        }

        /**
         * Foreign Key Index 는 목록에 포함하지 않음
         */
        
        if ( sIsForeignKeyIndex == STL_TRUE )
        {
            /**
             * Foreign Key Index 임
             * - nothing to do 
             */
        }
        else
        {
            /**
             * Prime Index 임, (Normal Index 또는 PK/UK Index 임)
             */
 
            STL_TRY( ellAddNewObjectItem( sPrimeIndexList,
                                          sIndexHandle,
                                          NULL,
                                          aRegionMem,
                                          aEnv )
                     == STL_SUCCESS );
        }
    }
    
    /**
     * Output 설정
     */

    *aIndexObjectList = sPrimeIndexList;
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}



/**
 * @brief TRUNCATE TABLE 에 의해 영향을 받는 Index Object List 를 획득
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aTableDictHandle Table Dictionary Handle
 * @param[in]  aRegionMem       Region Memory
 * @param[in]  aIndexObjectList 영향을 받는 Index Object Handle List
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus ellGetIndexList4TruncateTable( smlTransId          aTransID,
                                         smlStatement      * aStmt,
                                         ellDictHandle     * aTableDictHandle,
                                         knlRegionMem      * aRegionMem,
                                         ellObjectList    ** aIndexObjectList,
                                         ellEnv            * aEnv )
{
    ellObjectList    * sObjectList = NULL;

    stlInt64           sTableID = ELL_DICT_OBJECT_ID_NA;

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIndexObjectList != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Table ID 획득
     */
    
    sTableID = ellGetTableID( aTableDictHandle );

    /**
     * Table 과 관련된 Index 정보 획득
     * - SELECT INDEX_ID
     * - FROM DEFINITION_SCHEMA.INDEX_KEY_TABLE_USAGE
     * - WHERE TABLE_ID = sTableID
     */

    STL_TRY( ellInitObjectList( & sObjectList, aRegionMem, aEnv ) == STL_SUCCESS );
    
    STL_TRY( eldGetObjectList( aTransID,
                               aStmt,
                               ELDT_TABLE_ID_INDEX_KEY_TABLE_USAGE,
                               ELL_OBJECT_INDEX,
                               ELDT_IndexTable_ColumnOrder_INDEX_ID,
                               ELDT_IndexTable_ColumnOrder_TABLE_ID,
                               sTableID,
                               aRegionMem,
                               sObjectList,
                               aEnv )
             == STL_SUCCESS );

    /**
     * Output 설정
     */

    *aIndexObjectList = sObjectList;
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/*********************************************************
 * Dictionary 변경 함수 
 *********************************************************/

/**
 * @brief Table 의 Dictionary Record 를 변경하지 않는 경우, Table 변경 시간을 설정한다.
 * @param[in] aTransID         Transaction ID
 * @param[in] aStmt            Statement
 * @param[in] aTableDictHandle Table Dictionary Handle
 * @param[in] aEnv             Environment
 * @remarks
 */
stlStatus ellSetTime4ModifyTableElement( smlTransId      aTransID,
                                         smlStatement  * aStmt,
                                         ellDictHandle * aTableDictHandle,
                                         ellEnv        * aEnv )
{
    stlInt64 sTableID = ELL_DICT_OBJECT_ID_NA;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableDictHandle != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */

    sTableID = ellGetTableID( aTableDictHandle );

    /**
     * 테이블 변경 시간 설정
     */

    STL_TRY( eldModifyDictDesc( aTransID,
                                aStmt,
                                ELDT_TABLE_ID_TABLES,
                                ELDT_Tables_ColumnOrder_TABLE_ID,
                                sTableID,
                                ELDT_Tables_ColumnOrder_MODIFIED_TIME,
                                NULL,  /* aModifyData */
                                aEnv )
             == STL_SUCCESS );

    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}

/**
 * @brief Table Descriptor 를 Dictionary 에 추가한다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aOwnerID         Owner의 ID
 * @param[in]  aSchemaID        Schema ID
 * @param[out] aTableID         Table 의 ID 
 * @param[in]  aTablespaceID    Tablespace ID
 *                         <BR> Base Table 이 아니라면, ELL_DICT_OBJECT_ID_NA
 * @param[in]  aPhysicalID      Table 의 Physical ID
 *                         <BR> Base Table 이 아니라면, ELL_DICT_OBJECT_ID_NA
 * @param[in]  aTableName       Table Name
 * @param[in]  aTableType       Table Type 
 * @param[in]  aTableComment    Table Comment (nullable)
 * @param[in]  aEnv             Execution Library Environment
 * @remarks
 */

stlStatus ellInsertTableDesc( smlTransId     aTransID,
                              smlStatement * aStmt,
                              stlInt64       aOwnerID,
                              stlInt64       aSchemaID,
                              stlInt64     * aTableID,
                              stlInt64       aTablespaceID,
                              stlInt64       aPhysicalID,
                              stlChar      * aTableName,
                              ellTableType   aTableType,
                              stlChar      * aTableComment,
                              ellEnv       * aEnv )
{
    /**
     * Table Owner 가 소유하는 Table Privilege 들
     * - CONTROL privilege 외의 모든 Table Privilege를 갖는다.
     */
    
    ellTablePrivAction   sTableOwnerPriv[] =
        {
            ELL_TABLE_PRIV_ACTION_SELECT,              /**< SELECT table privilege */
            ELL_TABLE_PRIV_ACTION_INSERT,              /**< INSERT table privilege */
            ELL_TABLE_PRIV_ACTION_UPDATE,              /**< UPDATE table privilege */
            ELL_TABLE_PRIV_ACTION_DELETE,              /**< DELETE table privilege */
            ELL_TABLE_PRIV_ACTION_TRIGGER,             /**< TRIGGER table privilege */
            ELL_TABLE_PRIV_ACTION_REFERENCES,          /**< REFERENCES table privilege */
            
            ELL_TABLE_PRIV_ACTION_LOCK,                /**< LOCK table privilege */
            ELL_TABLE_PRIV_ACTION_INDEX,               /**< INDEX table privilege */
            ELL_TABLE_PRIV_ACTION_ALTER,               /**< ALTER table privilege */
            
            ELL_TABLE_PRIV_ACTION_NA
        };
    stlBool  sTableOwnerGrant[] = 
        {
            STL_TRUE,              /**< SELECT table privilege */
            STL_TRUE,              /**< INSERT table privilege */
            STL_TRUE,              /**< UPDATE table privilege */
            STL_TRUE,              /**< DELETE table privilege */
            STL_TRUE,              /**< TRIGGER table privilege */
            STL_TRUE,              /**< REFERENCES table privilege */
            
            STL_TRUE,              /**< LOCK table privilege */
            STL_TRUE,              /**< INDEX table privilege */
            STL_TRUE,              /**< ALTER table privilege */
            
            STL_FALSE,             /* N/A */
        };

    /**
     * View Owner 가 소유하는 Table Privilege 들
     */
    
    ellTablePrivAction   sViewOwnerPriv[] =
        {
            ELL_TABLE_PRIV_ACTION_SELECT,              /**< SELECT table privilege */
            ELL_TABLE_PRIV_ACTION_INSERT,              /**< INSERT table privilege */
            ELL_TABLE_PRIV_ACTION_UPDATE,              /**< UPDATE table privilege */
            ELL_TABLE_PRIV_ACTION_DELETE,              /**< DELETE table privilege */
            ELL_TABLE_PRIV_ACTION_TRIGGER,             /**< TRIGGER table privilege */
            
            ELL_TABLE_PRIV_ACTION_LOCK,                /**< LOCK table privilege */
            ELL_TABLE_PRIV_ACTION_ALTER,               /**< ALTER table privilege */
            
            ELL_TABLE_PRIV_ACTION_NA
        };
    
    stlBool  sViewOwnerGrant[] = 
        {
            STL_TRUE,              /**< SELECT table privilege */
            STL_TRUE,              /**< INSERT table privilege */
            STL_TRUE,              /**< UPDATE table privilege */
            STL_TRUE,              /**< DELETE table privilege */
            STL_FALSE,             /**< TRIGGER table privilege, WITH GRANT OPTION 이 없음 */
            STL_TRUE,              /**< REFERENCES table privilege */
            
            STL_TRUE,              /**< LOCK table privilege */
            STL_TRUE,              /**< ALTER table privilege */
            
            STL_FALSE,             /* N/A */
        };
    
    stlInt32   i = 0;
    ellTablePrivAction * sOwnerPriv = NULL;
    stlBool            * sOwnerGrant = NULL;
    
    /**
     * Table Dictionary Record 추가
     */
    
    STL_TRY( eldInsertTableDesc( aTransID,
                                 aStmt,
                                 aOwnerID,
                                 aSchemaID,
                                 aTableID,
                                 aTablespaceID,
                                 aPhysicalID,
                                 aTableName,
                                 aTableType,
                                 aTableComment,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Table Owner Privilege 추가
     */

    switch ( aTableType )
    {
        case ELL_TABLE_TYPE_BASE_TABLE:
            sOwnerPriv  = sTableOwnerPriv;
            sOwnerGrant = sTableOwnerGrant;
            break;
        case ELL_TABLE_TYPE_VIEW:
            sOwnerPriv  = sViewOwnerPriv;
            sOwnerGrant = sViewOwnerGrant;
            break;
        case ELL_TABLE_TYPE_GLOBAL_TEMPORARY:
        case ELL_TABLE_TYPE_LOCAL_TEMPORARY:
        case ELL_TABLE_TYPE_SYSTEM_VERSIONED:
            /**
             * @todo new feature
             */
            break;
        case ELL_TABLE_TYPE_SEQUENCE:
        case ELL_TABLE_TYPE_SYNONYM:
            /**
             * Sequence Naming Resolution 을 위해 추가되는 경우임.
             * - nothing to do
             */
            
            sOwnerPriv = NULL;
            sOwnerGrant = NULL;
            break;

        case ELL_TABLE_TYPE_FIXED_TABLE:
        case ELL_TABLE_TYPE_DUMP_TABLE:
            /**
             * 이런 경우가 발생하면 안됨
             */
            STL_DASSERT(0);
            sOwnerPriv = NULL;
            sOwnerGrant = NULL;
            break;
        default:
            /**
             * 이런 경우가 발생하면 안됨
             */
            STL_DASSERT(0);
            sOwnerPriv = NULL;
            sOwnerGrant = NULL;
            break;
    }
    
    i = 0;
    
    while ( sOwnerPriv != NULL )
    {
        if ( sOwnerPriv[i] == ELL_TABLE_PRIV_ACTION_NA )
        {
            break;
        }
        else
        {
            STL_TRY( ellInsertTablePrivDesc( aTransID,
                                             aStmt,
                                             ELDT_AUTH_ID_SYSTEM,  /* Grantor */
                                             aOwnerID,             /* Grantee */
                                             aSchemaID,
                                             *aTableID,
                                             sOwnerPriv[i],        /* Priv Action */
                                             sOwnerGrant[i],       /* sWithGrant */
                                             STL_FALSE,            /* sWithHierarchy */
                                             aEnv )
                     == STL_SUCCESS );
        }
        
        i++;
    }
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/**
 * @brief DROP TABLE 을 위한 Prime Element 의 Dictionary Row 를 제거 
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aTableDictHandle Table Dictionary Handle
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus ellRemoveDict4DropTablePrimeElement( smlTransId        aTransID,
                                               smlStatement    * aStmt,
                                               ellDictHandle   * aTableDictHandle,
                                               ellEnv          * aEnv )
{
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableDictHandle != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Table 과 관련된 기본 Dictionary 정보 제거
     */

    STL_TRY( eldDeleteDict4DropTablePrimeElement( aTransID,
                                                  aStmt,
                                                  aTableDictHandle,
                                                  aEnv )
             == STL_SUCCESS );

    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}



/**
 * @brief Table List 로부터 Table Prime Element 의 Dictionary Row 를 제거 
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aTableList       Table List
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus ellRemoveDictTablePrimeElement4TableList( smlTransId        aTransID,
                                                    smlStatement    * aStmt,
                                                    ellObjectList   * aTableList,
                                                    ellEnv          * aEnv )
{
    ellObjectItem   * sObjectItem = NULL;
    ellDictHandle   * sObjectHandle = NULL;

    ellTableType      sTableType = ELL_TABLE_TYPE_NA;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableList != NULL, ELL_ERROR_STACK(aEnv) );

    
    /**
     * Table List 를 순회하며 기본 Dictionary 정보 제거
     */

    STL_RING_FOREACH_ENTRY( & aTableList->mHeadList, sObjectItem )
    {
        sObjectHandle = & sObjectItem->mObjectHandle;

        sTableType = ellGetTableType( sObjectHandle );

        switch ( sTableType )
        {
            case ELL_TABLE_TYPE_BASE_TABLE:
                STL_TRY( ellRemoveDict4DropTablePrimeElement( aTransID,
                                                              aStmt,
                                                              sObjectHandle,
                                                              aEnv )
                         == STL_SUCCESS );
                break;
            case ELL_TABLE_TYPE_VIEW:
                STL_TRY( ellRemoveDict4DropView( aTransID,
                                                 aStmt,
                                                 sObjectHandle,
                                                 aEnv )
                         == STL_SUCCESS );
                break;

            case ELL_TABLE_TYPE_GLOBAL_TEMPORARY:
            case ELL_TABLE_TYPE_LOCAL_TEMPORARY:
            case ELL_TABLE_TYPE_SYSTEM_VERSIONED:
                /*
                 * not implemented
                 */
                STL_DASSERT(0);
                break;

            case ELL_TABLE_TYPE_SEQUENCE:
            case ELL_TABLE_TYPE_FIXED_TABLE:
            case ELL_TABLE_TYPE_DUMP_TABLE:
            case ELL_TABLE_TYPE_SYNONYM:
            default:
                /*
                 * invalid use
                 */
                STL_DASSERT(0);
                break;
        }
    }
        
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Table Dictionary 의 Physical ID 를 변경한다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aTableID         Table 의 ID 
 * @param[in]  aPhysicalID      Table 의 Physical ID
 * @param[in]  aEnv             Execution Library Environment
 * @remarks
 */
stlStatus ellModifyTablePhysicalID( smlTransId     aTransID,
                                    smlStatement * aStmt,
                                    stlInt64       aTableID,
                                    stlInt64       aPhysicalID,
                                    ellEnv       * aEnv )
{
    STL_TRY( eldModifyDictDesc( aTransID,
                                aStmt,
                                ELDT_TABLE_ID_TABLES,
                                ELDT_Tables_ColumnOrder_TABLE_ID,
                                aTableID,
                                ELDT_Tables_ColumnOrder_PHYSICAL_ID,
                                (void *) & aPhysicalID,
                                aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/**
 * @brief Table Dictionary 의 Table Name 을 변경한다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aTableID         Table 의 ID 
 * @param[in]  aNewName         Table 의 New Name
 * @param[in]  aEnv             Execution Library Environment
 * @remarks
 */
stlStatus ellModifyTableName( smlTransId     aTransID,
                              smlStatement * aStmt,
                              stlInt64       aTableID,
                              stlChar      * aNewName,
                              ellEnv       * aEnv )
{
    STL_TRY( eldModifyDictDesc( aTransID,
                                aStmt,
                                ELDT_TABLE_ID_TABLES,
                                ELDT_Tables_ColumnOrder_TABLE_ID,
                                aTableID,
                                ELDT_Tables_ColumnOrder_TABLE_NAME,
                                (void *) aNewName,
                                aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/**
 * @brief Table Dictionary 의 Supp Log(PK) 설정을 변경한다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aTableID         Table 의 ID 
 * @param[in]  aOnOff           ON(STL_TRUE) or OFF(STL_FALSE)
 * @param[in]  aEnv             Execution Library Environment
 * @remarks
 */
stlStatus ellModifyTableSuppLog( smlTransId     aTransID,
                                 smlStatement * aStmt,
                                 stlInt64       aTableID,
                                 stlBool        aOnOff,
                                 ellEnv       * aEnv )
{
    STL_TRY( eldModifyDictDesc( aTransID,
                                aStmt,
                                ELDT_TABLE_ID_TABLES,
                                ELDT_Tables_ColumnOrder_TABLE_ID,
                                aTableID,
                                ELDT_Tables_ColumnOrder_IS_SET_SUPPLOG_PK,
                                (void *) & aOnOff,
                                aEnv )
             == STL_SUCCESS );
    
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
 * @brief Table Cache 를 재구성
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aTableDictHandle Table Dictionary Handle
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus ellRebuildTableCache( smlTransId        aTransID,
                                smlStatement    * aStmt,
                                ellDictHandle   * aTableDictHandle,
                                ellEnv          * aEnv )
{
    stlInt64  sTableID = ELL_DICT_OBJECT_ID_NA;

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableDictHandle != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Table ID 획득
     */
    
    sTableID = ellGetTableID( aTableDictHandle );

    /**
     * 기존 Cache 를 제거
     */

    STL_TRY( eldRemoveTableAllCache( aTransID,
                                     aStmt,
                                     aTableDictHandle,
                                     aEnv )
             == STL_SUCCESS );
    
    /**
     * 새로운 Cache 를 구축 
     */

    STL_TRY( eldBuildTableCache( aTransID,
                                 aStmt,
                                 sTableID,
                                 aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/**
 * @brief CREATE TABLE 을 위한 Cache 를 재구성
 * @param[in]  aTransID     Transaction ID
 * @param[in]  aStmt        Statement
 * @param[in]  aTableID     Table ID
 * @param[in]  aEnv         Envirionment 
 * @remarks
 * 필요한 정보들이 Dictionary Table 에 저장되어 있어야 함.
 */
stlStatus ellRefineCache4CreateTable( smlTransId           aTransID,
                                      smlStatement       * aStmt,
                                      stlInt64             aTableID,
                                      ellEnv             * aEnv )
{
    /**
     * 새로운 Cache 를 구축 
     */

    STL_TRY( eldBuildTableCache( aTransID,
                                 aStmt,
                                 aTableID,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * DDL Supplemental Logging 기록
     */

    STL_TRY( eldWriteSuppLogTableDDL( aTransID,
                                      aTableID,
                                      ELL_SUPP_LOG_CREATE_TABLE,
                                      aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}




/**
 * @brief DROP TABLE 에 대한 Table Prime Element 에 대한 Cache 를 재구성
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aTableDictHandle Table Dictionary Handle
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus ellRefineCache4DropTablePrimeElement( smlTransId        aTransID,
                                                smlStatement    * aStmt,
                                                ellDictHandle   * aTableDictHandle,
                                                ellEnv          * aEnv )
{
    stlInt64  sTableID = ELL_DICT_OBJECT_ID_NA;

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableDictHandle != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Table ID 획득
     */
    
    sTableID = ellGetTableID( aTableDictHandle );

    /**
     * DDL Supplemental Logging 기록
     */

    STL_TRY( eldWriteSuppLogTableDDL( aTransID,
                                      sTableID,
                                      ELL_SUPP_LOG_DROP_TABLE,
                                      aEnv )
             == STL_SUCCESS );

    /**
     * Table Prime Element 의 Cache 를 제거
     */

    STL_TRY( eldRemoveTablePrimeElementCache( aTransID,
                                              aTableDictHandle,
                                              aEnv )
             == STL_SUCCESS );

    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}



/**
 * @brief Table List 에 대한 DROP Table Prime Element 에 대한 Cache 를 재구성
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aTableList       Table List
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus ellRefineCacheDropTablePrime4TableList( smlTransId        aTransID,
                                                  smlStatement    * aStmt,
                                                  ellObjectList   * aTableList,
                                                  ellEnv          * aEnv )
{
    ellObjectItem   * sObjectItem = NULL;
    ellDictHandle   * sObjectHandle = NULL;

    ellTableType      sTableType = ELL_TABLE_TYPE_NA;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableList != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Table List 를 순회하며 Prime Element Cache 를 제거
     */
    
    STL_RING_FOREACH_ENTRY( & aTableList->mHeadList, sObjectItem )
    {
        sObjectHandle = & sObjectItem->mObjectHandle;

        sTableType = ellGetTableType( sObjectHandle );

        switch ( sTableType )
        {
            case ELL_TABLE_TYPE_BASE_TABLE:
                STL_TRY( ellRefineCache4DropTablePrimeElement( aTransID,
                                                               aStmt,
                                                               sObjectHandle,
                                                               aEnv )
                         == STL_SUCCESS );
                break;
            case ELL_TABLE_TYPE_VIEW:
                STL_TRY( ellRefineCache4DropView( aTransID,
                                                  sObjectHandle,
                                                  aEnv )
                         == STL_SUCCESS );
                break;

            case ELL_TABLE_TYPE_GLOBAL_TEMPORARY:
            case ELL_TABLE_TYPE_LOCAL_TEMPORARY:
            case ELL_TABLE_TYPE_SYSTEM_VERSIONED:
                /*
                 * not implemented
                 */
                STL_DASSERT(0);
                break;

            case ELL_TABLE_TYPE_SEQUENCE:
            case ELL_TABLE_TYPE_FIXED_TABLE:
            case ELL_TABLE_TYPE_DUMP_TABLE:
            case ELL_TABLE_TYPE_SYNONYM:
            default:
                /*
                 * invalid use
                 */
                STL_DASSERT(0);
                break;
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief TRUNCATE TABLE 에 대한 Cache 를 재구성
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aNewPhysicalID   New Table Physical ID
 * @param[in]  aTableDictHandle Table Dictionary Handle
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus ellRefineCache4TruncateTable( smlTransId        aTransID,
                                        smlStatement    * aStmt,
                                        stlInt64          aNewPhysicalID,
                                        ellDictHandle   * aTableDictHandle,
                                        ellEnv          * aEnv )
{
    stlInt64  sTableID = ELL_DICT_OBJECT_ID_NA;

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableDictHandle != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Table ID 획득
     */
    
    sTableID = ellGetTableID( aTableDictHandle );

    /**
     * DDL Supplemental Logging 기록
     */

    STL_TRY( eldWriteSuppLogTable4Truncate( aTransID,
                                            sTableID,
                                            aNewPhysicalID,
                                            ELL_SUPP_LOG_TRUNCATE_TABLE,
                                            aEnv )
             == STL_SUCCESS );
    
    /**
     * Table Cache 를 재구축 
     */

    STL_TRY( ellRebuildTableCache( aTransID,
                                   aStmt,
                                   aTableDictHandle,
                                   aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/**
 * @brief ALTER TABLE .. STORAGE .. 에 대한 Cache 를 재구성
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aTableDictHandle Table Dictionary Handle
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus ellRefineCache4AlterPhysicalAttr( smlTransId        aTransID,
                                            ellDictHandle   * aTableDictHandle,
                                            ellEnv          * aEnv )
{
    stlInt64  sTableID = ELL_DICT_OBJECT_ID_NA;

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aTableDictHandle != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Table ID 획득
     */
    
    sTableID = ellGetTableID( aTableDictHandle );

    /**
     * DDL Supplemental Logging 기록
     */

    STL_TRY( eldWriteSuppLogTableDDL( aTransID,
                                      sTableID,
                                      ELL_SUPP_LOG_ALTER_TABLE_PHYSICAL_ATTR,
                                      aEnv )
             == STL_SUCCESS );

    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}

/**
 * @brief ALTER TABLE .. RENAME TO .. 에 대한 Cache 를 재구성
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aTableDictHandle Table Dictionary Handle
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus ellRefineCache4RenameTable( smlTransId        aTransID,
                                      smlStatement    * aStmt,
                                      ellDictHandle   * aTableDictHandle,
                                      ellEnv          * aEnv )
{
    stlInt64  sTableID = ELL_DICT_OBJECT_ID_NA;

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableDictHandle != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Table ID 획득
     */
    
    sTableID = ellGetTableID( aTableDictHandle );

    /**
     * DDL Supplemental Logging 기록
     */

    STL_TRY( eldWriteSuppLogTableDDL( aTransID,
                                      sTableID,
                                      ELL_SUPP_LOG_ALTER_TABLE_RENAME_TABLE,
                                      aEnv )
             == STL_SUCCESS );
    
    /**
     * Cache 를 재구축 
     */

    STL_TRY( ellRebuildTableCache( aTransID,
                                   aStmt,
                                   aTableDictHandle,
                                   aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/**
 * @brief ALTER TABLE .. ADD/DROP SUPPLEMENTAL LOG 에 대한 Cache 를 재구성
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aTableDictHandle Table Dictionary Handle
 * @param[in]  aIsAdd           ADD or DROP
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus ellRefineCache4SuppLogTable( smlTransId        aTransID,
                                       smlStatement    * aStmt,
                                       ellDictHandle   * aTableDictHandle,
                                       stlBool           aIsAdd,
                                       ellEnv          * aEnv )
{
    stlInt64  sTableID = ELL_DICT_OBJECT_ID_NA;

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableDictHandle != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Table ID 획득
     */
    
    sTableID = ellGetTableID( aTableDictHandle );

    /**
     * DDL Supplemental Logging 기록
     */

    if ( aIsAdd == STL_TRUE )
    {
        STL_TRY( eldWriteSuppLogTableDDL( aTransID,
                                          sTableID,
                                          ELL_SUPP_LOG_ALTER_TABLE_ADD_SUPP_LOG,
                                          aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( eldWriteSuppLogTableDDL( aTransID,
                                          sTableID,
                                          ELL_SUPP_LOG_ALTER_TABLE_DROP_SUPP_LOG,
                                          aEnv )
                 == STL_SUCCESS );
    }
    
    /**
     * Cache 를 재구축 
     */

    STL_TRY( ellRebuildTableCache( aTransID,
                                   aStmt,
                                   aTableDictHandle,
                                   aEnv )
             == STL_SUCCESS );
    
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
 * @brief Schema Handle 과 Table Name 을 이용해 Table Dictionary Handle 을 얻는다.
 * @param[in]  aTransID             Transaction ID
 * @param[in]  aViewSCN             aViewSCN
 * @param[in]  aSchemaDictHandle    Schema Dictionary Handle
 * @param[in]  aTableName           Table Name
 * @param[out] aTableDictHandle     Table Dictionary Handle
 * @param[out] aExist               존재 여부 
 * @param[in]  aEnv                 Envirionment 
 * @remarks
 */
stlStatus ellGetTableDictHandleWithSchema( smlTransId           aTransID,
                                           smlScn               aViewSCN,
                                           ellDictHandle      * aSchemaDictHandle,
                                           stlChar            * aTableName,
                                           ellDictHandle      * aTableDictHandle,
                                           stlBool            * aExist,
                                           ellEnv             * aEnv )
{
    ellSchemaDesc * sSchemaDesc = NULL;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aSchemaDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableName != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExist != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Schema Descriptor
     */

    sSchemaDesc = (ellSchemaDesc *) ellGetObjectDesc( aSchemaDictHandle );

    /**
     * Table Dictionary Handle 획득
     */

    ellInitDictHandle( aTableDictHandle );
    
    STL_TRY( eldGetTableHandleByName( aTransID,
                                      aViewSCN,
                                      sSchemaDesc->mSchemaID,
                                      aTableName,
                                      aTableDictHandle,
                                      aExist,
                                      aEnv )
             == STL_SUCCESS );

    if ( *aExist == STL_TRUE )
    {
        aTableDictHandle->mFullyQualifiedName = STL_TRUE;
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Auth Handle 과 Table Name 을 이용해 Table Dictionary Handle 을 얻는다.
 * @param[in]  aTransID             Transaction ID
 * @param[in]  aViewSCN             aViewSCN
 * @param[in]  aAuthDictHandle      Authorization Dictionary Handle
 * @param[in]  aTableName           Table Name
 * @param[out] aTableDictHandle     Table Dictionary Handle
 * @param[out] aExist               존재 여부 
 * @param[in]  aEnv                 Envirionment 
 * @remarks
 */
stlStatus ellGetTableDictHandleWithAuth( smlTransId           aTransID,
                                         smlScn               aViewSCN,
                                         ellDictHandle      * aAuthDictHandle,
                                         stlChar            * aTableName,
                                         ellDictHandle      * aTableDictHandle,
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
    STL_PARAM_VALIDATE( aTableName != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExist != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Authorization Descriptor
     */

    sAuthDesc = (ellAuthDesc *) ellGetObjectDesc( aAuthDictHandle );

    /**
     * Table Dictionary Handle 을 획득
     */
    
    ellInitDictHandle( aTableDictHandle );
    
    while(1)
    {
        /**
         * 사용자 Schema Path 를 이용해 검색
         */
        for ( i = 0; i < sAuthDesc->mSchemaPathCnt; i++ )
        {
            STL_TRY( eldGetTableHandleByName( aTransID,
                                              aViewSCN,
                                              sAuthDesc->mSchemaIDArray[i],
                                              aTableName,
                                              aTableDictHandle,
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
                STL_TRY( eldGetTableHandleByName( aTransID,
                                                  aViewSCN,
                                                  sPublicDesc->mSchemaIDArray[i],
                                                  aTableName,
                                                  aTableDictHandle,
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
 * @brief Table ID 를 이용해 Table Dictionary Handle 을 얻는다.
 * @param[in]  aTransID           Transaction ID
 * @param[in]  aViewSCN           View SCN
 * @param[in]  aTableID           Table ID
 * @param[out] aTableDictHandle   Table Dictionary Handle
 * @param[out] aExist             존재 여부 
 * @param[in]  aEnv               Envirionment 
 * @remarks
 */
stlStatus ellGetTableDictHandleByID( smlTransId           aTransID,
                                     smlScn               aViewSCN,
                                     stlInt64             aTableID,
                                     ellDictHandle      * aTableDictHandle,
                                     stlBool            * aExist,
                                     ellEnv             * aEnv )
{
    return eldGetTableHandleByID( aTransID,
                                  aViewSCN,
                                  aTableID,
                                  aTableDictHandle,
                                  aExist,
                                  aEnv );
}

/**
 * @brief PENDING_TRANS table 의 physical handle 을 획득
 * @param[out] aPhyHandle  PENDING_TRANS table 의 physical handle
 * @param[in]  aViewSCN    View SCN
 * @param[in]  aEnv        environment
 */
stlStatus  ellGetPhyHandle4PendingTransTable( void   ** aPhyHandle,
                                              smlScn    aViewSCN,
                                              ellEnv  * aEnv )
{
    smlTransId    sTransID  = SML_INVALID_TRANSID;
    stlBool       sObjectExist = STL_FALSE;
    ellDictHandle sTableHandle;
    
    STL_PARAM_VALIDATE( aPhyHandle != NULL, ELL_ERROR_STACK(aEnv) );

    STL_TRY( ellGetTableDictHandleByID( sTransID,
                                        aViewSCN,
                                        ELDT_TABLE_ID_PENDING_TRANS,
                                        & sTableHandle,
                                        & sObjectExist,
                                        aEnv )
             == STL_SUCCESS );
    STL_DASSERT( sObjectExist == STL_TRUE );

    /**
     * Output 설정
     */

    *aPhyHandle = ellGetTableHandle( & sTableHandle );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*********************************************************
 * 정보 획득 함수 
 *********************************************************/


/**
 * @brief Table ID 정보
 * @param[in] aTableDictHandle Table Dictionary Handle
 * @return Table ID
 * @remarks
 */
stlInt64 ellGetTableID( ellDictHandle * aTableDictHandle )
{
    ellTableDesc * sTableDesc = NULL;

    STL_DASSERT( aTableDictHandle->mObjectType == ELL_OBJECT_TABLE );
    
    sTableDesc = (ellTableDesc *) ellGetObjectDesc( aTableDictHandle );

    return sTableDesc->mTableID;
}

/**
 * @brief Table Owner ID 정보
 * @param[in] aTableDictHandle Table Dictionary Handle
 * @return Table Owner ID
 * @remarks
 */
stlInt64 ellGetTableOwnerID( ellDictHandle * aTableDictHandle )
{
    ellTableDesc * sTableDesc = NULL;

    STL_DASSERT( aTableDictHandle->mObjectType == ELL_OBJECT_TABLE );
    
    sTableDesc = (ellTableDesc *) ellGetObjectDesc( aTableDictHandle );

    return sTableDesc->mOwnerID;
}

/**
 * @brief Table Schema ID 정보
 * @param[in] aTableDictHandle Table Dictionary Handle
 * @return Table Schema ID
 * @remarks
 */
stlInt64 ellGetTableSchemaID( ellDictHandle * aTableDictHandle )
{
    ellTableDesc * sTableDesc = NULL;

    STL_DASSERT( aTableDictHandle->mObjectType == ELL_OBJECT_TABLE );
    
    sTableDesc = (ellTableDesc *) ellGetObjectDesc( aTableDictHandle );

    return sTableDesc->mSchemaID;
}

/**
 * @brief Table 의 Tablespace ID 정보
 * @param[in] aTableDictHandle Table Dictionary Handle
 * @return Table 의 Tablespace ID
 * @remarks
 */
stlInt64 ellGetTableTablespaceID( ellDictHandle * aTableDictHandle )
{
    ellTableDesc * sTableDesc = NULL;

    STL_DASSERT( aTableDictHandle->mObjectType == ELL_OBJECT_TABLE );
    
    sTableDesc = (ellTableDesc *) ellGetObjectDesc( aTableDictHandle );

    return sTableDesc->mTablespaceID;
}

/**
 * @brief Table Name 정보를 얻는다.
 * @param[in] aTableDictHandle Table Dictionary Handle
 * @return Table Name
 * @remarks
 */
stlChar * ellGetTableName( ellDictHandle * aTableDictHandle )
{
    ellTableDesc * sTableDesc = NULL;

    STL_DASSERT( aTableDictHandle->mObjectType == ELL_OBJECT_TABLE );
    
    sTableDesc = (ellTableDesc *) ellGetObjectDesc( aTableDictHandle );

    return sTableDesc->mTableName;
}


/**
 * @brief Table Type 정보를 얻는다.
 * @param[in] aTableDictHandle Table Dictionary Handle
 * @return Table Type (ellTableType)
 * @remarks
 */
ellTableType ellGetTableType( ellDictHandle * aTableDictHandle )
{
    ellTableDesc * sTableDesc = NULL;

    STL_DASSERT( aTableDictHandle->mObjectType == ELL_OBJECT_TABLE );
    
    sTableDesc = (ellTableDesc *) ellGetObjectDesc( aTableDictHandle );

    return sTableDesc->mTableType;
}


/**
 * @brief Table Handle 정보를 얻는다.
 * @param[in] aTableDictHandle Table Dictionary Handle
 * @return Table Handle
 * @remarks
 * Table 의 물리적 Handle 정보이다.
 */
void * ellGetTableHandle( ellDictHandle * aTableDictHandle )
{
    ellTableDesc * sTableDesc = NULL;
    void         * sHandle = NULL;

    STL_DASSERT( aTableDictHandle->mObjectType == ELL_OBJECT_TABLE );
    
    sTableDesc = (ellTableDesc *) ellGetObjectDesc( aTableDictHandle );

    switch ( sTableDesc->mTableType )
    {
        case ELL_TABLE_TYPE_BASE_TABLE:
            sHandle = sTableDesc->mTableHandle;
            break;
        case ELL_TABLE_TYPE_VIEW:
            sHandle = NULL;
            break;
        case ELL_TABLE_TYPE_GLOBAL_TEMPORARY:
        case ELL_TABLE_TYPE_LOCAL_TEMPORARY:
        case ELL_TABLE_TYPE_SYSTEM_VERSIONED:
            /* not implemented */
            sHandle = NULL;
            STL_DASSERT(0);
            break;
        case ELL_TABLE_TYPE_SEQUENCE:
        case ELL_TABLE_TYPE_SYNONYM:
            /* 호출되어서는 안됨 */
            sHandle = NULL;
            STL_DASSERT(0);
            break;

        case ELL_TABLE_TYPE_FIXED_TABLE:
        case ELL_TABLE_TYPE_DUMP_TABLE:
            sHandle = sTableDesc->mTableHandle;
            break;
        default:
            sHandle = NULL;
            STL_DASSERT(0);
            break;
    }
    
    return sHandle;
}


/**
 * @brief Table 의 Column 개수 including unused columns
 * @param[in] aTableDictHandle Table Dictionary Handle
 * @return Column Count
 * @remarks
 */
stlInt32     ellGetTableColumnCnt( ellDictHandle * aTableDictHandle )
{
    ellTableDesc * sTableDesc = (ellTableDesc *) ellGetObjectDesc( aTableDictHandle );

    STL_DASSERT( aTableDictHandle->mObjectType == ELL_OBJECT_TABLE );
    
    /* view 는 syntax replacement 함 */
    STL_DASSERT( sTableDesc->mTableType != ELL_TABLE_TYPE_VIEW );
    
    return sTableDesc->mColumnCnt;
}

/**
 * @brief Table 의 Column 개수 w/o unused columns
 * @param[in] aTableDictHandle Table Dictionary Handle
 * @return Column Count
 * @remarks
 */
stlInt32     ellGetTableUsedColumnCnt( ellDictHandle * aTableDictHandle )
{
    stlInt32  sColCnt = 0;
    stlInt32  i = 0;
    
    ellTableDesc * sTableDesc = NULL;

    STL_DASSERT( aTableDictHandle->mObjectType == ELL_OBJECT_TABLE );
    
    sTableDesc = (ellTableDesc *) ellGetObjectDesc( aTableDictHandle );

    /* view 는 syntax replacement 함 */
    STL_DASSERT( sTableDesc->mTableType != ELL_TABLE_TYPE_VIEW );

    sColCnt = sTableDesc->mColumnCnt;

    for ( i = 0; i < sTableDesc->mColumnCnt; i++ )
    {
        if ( ellGetColumnUnused( & sTableDesc->mColumnDictHandle[i] ) == STL_TRUE )
        {
            /* Unused Column 임 */
            sColCnt--;
        }
        else
        {
            /* Used Column 임 */
        }
    }

    STL_DASSERT( sColCnt > 0 );
    
    return sColCnt;
}

/**
 * @brief Table 의 Column Dictionary Handle Array 
 * @param[in] aTableDictHandle Table Dictionary Handle
 * @return Column Dictionary Handle Array Pointer
 * @remarks
 */
ellDictHandle * ellGetTableColumnDictHandle( ellDictHandle * aTableDictHandle )
{
    ellTableDesc * sTableDesc = NULL;

    STL_DASSERT( aTableDictHandle->mObjectType == ELL_OBJECT_TABLE );
    
    sTableDesc = (ellTableDesc *) ellGetObjectDesc( aTableDictHandle );

    /* view 는 syntax replacement 함 */
    STL_DASSERT( sTableDesc->mTableType != ELL_TABLE_TYPE_VIEW );
    
    return sTableDesc->mColumnDictHandle;
}


/**
 * @brief Table 객체가 Built-In Object 인지의 여부
 * @param[in]  aTableDictHandle  Dictionary Handle
 * @remarks
 */
stlBool ellIsBuiltInTable( ellDictHandle * aTableDictHandle )
{
    ellTableDesc * sTableDesc = NULL;

    STL_DASSERT( aTableDictHandle->mObjectType == ELL_OBJECT_TABLE );
    
    sTableDesc = (ellTableDesc *) ellGetObjectDesc( aTableDictHandle );

    if ( sTableDesc->mOwnerID == ELDT_AUTH_ID_SYSTEM )
    {
        return STL_TRUE;
    }
    else
    {
        return STL_FALSE;
    }
}


/**
 * @brief Table 객체가 Updatable Table 인지의 여부
 * @param[in]  aTableDictHandle  Dictionary Handle
 * @remarks
 */
stlBool ellIsUpdatableTable( ellDictHandle * aTableDictHandle )
{
    stlBool sResult = STL_FALSE;
    
    STL_DASSERT( aTableDictHandle->mObjectType == ELL_OBJECT_TABLE );

    if ( ellIsBuiltInTable(aTableDictHandle) == STL_TRUE )
    {
        sResult = STL_FALSE;
    }
    else
    {
        switch ( ellGetTableType(aTableDictHandle) )
        {
            case ELL_TABLE_TYPE_BASE_TABLE:
                sResult = STL_TRUE;
                break;
                
            case ELL_TABLE_TYPE_VIEW:
                /**
                 * @todo updatable view 가 구현되면 변경되어야 함
                 */
                sResult = STL_FALSE;
                break;

            case ELL_TABLE_TYPE_GLOBAL_TEMPORARY:
            case ELL_TABLE_TYPE_LOCAL_TEMPORARY:
            case ELL_TABLE_TYPE_SYSTEM_VERSIONED:
                sResult = STL_FALSE;
                break;

            case ELL_TABLE_TYPE_SEQUENCE:
            case ELL_TABLE_TYPE_SYNONYM:
                sResult = STL_FALSE;
                break;
            case ELL_TABLE_TYPE_FIXED_TABLE:
            case ELL_TABLE_TYPE_DUMP_TABLE:
                sResult = STL_FALSE;
                break;

            default:
                sResult = STL_FALSE;
                break;
        }
    }

    return sResult;
}


/**
 * @brief Table 객체의 Supplemental Log Data Priamry Key 의 설정 여부 
 * @param[in]  aTableDictHandle  Dictionary Handle
 * @remarks
 */
stlBool ellIsTableSuppLogPK( ellDictHandle * aTableDictHandle )
{
    ellTableDesc * sTableDesc = NULL;

    STL_DASSERT( aTableDictHandle->mObjectType == ELL_OBJECT_TABLE );
    
    sTableDesc = (ellTableDesc *) ellGetObjectDesc( aTableDictHandle );

    return sTableDesc->mIsSuppPK;
}

/**
 * @brief 해당 Table 이 Supplemental Logging 이 필요한지 여부 
 * @param[in]  aTableDictHandle Table Dictionary Handle
 * @param[in]  aEnv             Environment
 * @remarks
 * - STL_TRUE : Primary Key Value 를 구축해야 함.
 * - STL_FALSE : 
 */
stlBool  ellGetTableNeedSupplePK( ellDictHandle * aTableDictHandle,
                                  ellEnv        * aEnv )
{
    stlBool        sResult = STL_FALSE;
    ellTableDesc * sTableDesc = NULL;

    STL_DASSERT( aTableDictHandle->mObjectType == ELL_OBJECT_TABLE );
    
    sTableDesc = (ellTableDesc *) ellGetObjectDesc( aTableDictHandle );

    if ( sTableDesc->mIsSuppPK == STL_TRUE )
    {
        sResult = STL_TRUE;
    }
    else
    {
        if ( knlGetPropertyBoolValueByID( KNL_PROPERTY_SUPPLEMENTAL_LOG_DATA_PRIMARY_KEY,
                                          KNL_ENV(aEnv) ) == STL_TRUE )
        {
            /**
             * DB level 의 Supplemental Logging
             */
            
            sResult = STL_TRUE;
        }
        else
        {
            sResult = STL_FALSE;
        }
    }
    
    return sResult;
}

/**
 * @brief Table Physical ID 정보
 * @param[in] aTableDictHandle Table Dictionary Handle
 * @return Table Physical ID
 * @remarks
 */
stlInt64 ellGetTablePhysicalID( ellDictHandle * aTableDictHandle )
{
    ellTableDesc * sTableDesc = NULL;

    STL_DASSERT( aTableDictHandle->mObjectType == ELL_OBJECT_TABLE );
    
    sTableDesc = (ellTableDesc *) ellGetObjectDesc( aTableDictHandle );

    return sTableDesc->mPhysicalID;
}


/** @} ellObjectTable */
