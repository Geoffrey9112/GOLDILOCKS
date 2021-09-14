/*******************************************************************************
 * ellDictView.c
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
 * @file ellDictView.c
 * @brief View Dictionary 관리 모듈
 */

#include <ell.h>
#include <elDef.h>

#include <eldt.h>
#include <eldc.h>
#include <eldDictionary.h>

#include <eldTable.h>
#include <eldView.h>
#include <elgPendOp.h>


/**
 * @addtogroup ellObjectView
 * @{
 */


/*********************************************************
 * DDL 관련 Object 획득 함수
 *********************************************************/

/**
 * @brief View 에 소속된 Column List 를 획득한다.
 * @param[in]  aTransID               Transaction ID
 * @param[in]  aStmt                  Statement
 * @param[in]  aViewHandle            View Dictionary Handle
 * @param[in]  aRegionMem             Region Memory
 * @param[out] aViewColumnList        View Column List
 * @param[in]  aEnv                   Environment
 * @remarks
 * View 의 Table Cache 와 Column Cache 는 연결 관계가 존재하지 않음.
 */
stlStatus ellGetViewColumnList( smlTransId          aTransID,
                                smlStatement      * aStmt,
                                ellDictHandle     * aViewHandle,
                                knlRegionMem      * aRegionMem,
                                ellObjectList    ** aViewColumnList,
                                ellEnv            * aEnv )
{
    ellObjectList * sColumnList = NULL;

    stlInt64        sTableID = ELL_DICT_OBJECT_ID_NA;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aViewHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aViewColumnList != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득 
     */

    sTableID = ellGetTableID( aViewHandle );

    /**********************************************
     * View 의 Column List 획득 
     **********************************************/

    STL_TRY( ellInitObjectList( & sColumnList, aRegionMem, aEnv ) == STL_SUCCESS );

    /**
     * View 에 속한 Column 목록 획득
     * - SELECT COLUMN_ID
     * - FROM DEFINITION_SCHEMA.COLUMNS
     * - WHERE TABLE_ID = sTableID
     */
    
    STL_TRY( eldGetObjectList( aTransID,
                               aStmt,
                               ELDT_TABLE_ID_COLUMNS,
                               ELL_OBJECT_COLUMN,
                               ELDT_Columns_ColumnOrder_COLUMN_ID,
                               ELDT_Columns_ColumnOrder_TABLE_ID,
                               sTableID,
                               aRegionMem,
                               sColumnList,
                               aEnv )
             == STL_SUCCESS );

    /**
     * Output 설정
     */

    *aViewColumnList = sColumnList;
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/**
 * @brief Relation List 에 소속된 View Column List 를 획득한다.
 * @param[in]  aTransID               Transaction ID
 * @param[in]  aStmt                  Statement
 * @param[in]  aRelationList          Relation(Table or View) List
 * @param[in]  aRegionMem             Region Memory
 * @param[out] aViewColumnList        View Column List
 * @param[in]  aEnv                   Environment
 * @remarks
 * View 의 Table Cache 와 Column Cache 는 연결 관계가 존재하지 않음.
 */
stlStatus ellGetViewColumnListByRelationList( smlTransId          aTransID,
                                              smlStatement      * aStmt,
                                              ellObjectList     * aRelationList,
                                              knlRegionMem      * aRegionMem,
                                              ellObjectList    ** aViewColumnList,
                                              ellEnv            * aEnv )
{
    ellObjectList * sObjectList = NULL;
    ellObjectList * sColumnList = NULL;

    ellObjectItem   * sRelationItem = NULL;
    ellDictHandle   * sRelationHandle = NULL;
    
    ellObjectItem   * sObjectItem = NULL;
    ellDictHandle   * sObjectHandle = NULL;

    stlBool    sDuplicate;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRelationList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aViewColumnList != NULL, ELL_ERROR_STACK(aEnv) );

    STL_TRY( ellInitObjectList( & sColumnList, aRegionMem, aEnv ) == STL_SUCCESS );
    
    /**********************************************
     * Relation List 에 속한 View Column List 획득
     **********************************************/

    STL_RING_FOREACH_ENTRY( & aRelationList->mHeadList, sRelationItem )
    {
        sRelationHandle = & sRelationItem->mObjectHandle;

        sObjectList = NULL;
        if ( ellGetTableType( sRelationHandle ) == ELL_TABLE_TYPE_VIEW )
        {
            /**
             * View 에 속한 Column 목록 획득
             */
            
            STL_TRY( ellGetViewColumnList( aTransID,
                                           aStmt,
                                           sRelationHandle,
                                           aRegionMem,
                                           & sObjectList,
                                           aEnv )
                     == STL_SUCCESS );
        
            /**
             * Column List 의 중복 제거 
             */
            
            STL_RING_FOREACH_ENTRY( & sObjectList->mHeadList, sObjectItem )
            {
                sObjectHandle = & sObjectItem->mObjectHandle;
                
                /**
                 * 중복을 제거한 Column List 에 추가 
                 */
                STL_TRY( ellAddNewObjectItem( sColumnList,
                                              sObjectHandle,
                                              & sDuplicate,
                                              aRegionMem,
                                              aEnv )
                         == STL_SUCCESS );
            }
        }
        else
        {
            /**
             * View 가 아님
             */
        }
    }
        
    /**
     * Output 설정
     */

    *aViewColumnList = sColumnList;
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}

/**
 * @brief Relation 변경에 의해 영향을 받는 View Handle List 를 획득
 * @param[in]  aTransID               Transaction ID
 * @param[in]  aStmt                  Statement
 * @param[in]  aRelationHandle        Relation(Table or View) Dictionary Handle
 * @param[in]  aRegionMem             Region Memory
 * @param[out] aAffectedViewList      Affected View List
 * @param[in]  aEnv                   Environment
 * @remarks
 */
stlStatus ellGetAffectedViewListByRelation( smlTransId          aTransID,
                                            smlStatement      * aStmt,
                                            ellDictHandle     * aRelationHandle,
                                            knlRegionMem      * aRegionMem,
                                            ellObjectList    ** aAffectedViewList,
                                            ellEnv            * aEnv )
{
    ellObjectList * sViewList = NULL;

    stlInt64        sTableID = ELL_DICT_OBJECT_ID_NA;

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRelationHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAffectedViewList != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득 
     */

    sTableID = ellGetTableID( aRelationHandle );

    /**********************************************
     * Relation 의 영향을 받는 View List 획득 
     **********************************************/

    STL_TRY( ellInitObjectList( & sViewList, aRegionMem, aEnv ) == STL_SUCCESS );

    /**
     * Relation 을 사용하는 View 정보 획득
     * - SELECT VIEW_ID
     * - FROM DEFINITION_SCHEMA.VIEW_TABLE_USAGE
     * - WHERE TABLE_ID = sTableID
     */
    
    STL_TRY( eldGetObjectList( aTransID,
                               aStmt,
                               ELDT_TABLE_ID_VIEW_TABLE_USAGE,
                               ELL_OBJECT_TABLE,
                               ELDT_ViewTable_ColumnOrder_VIEW_ID,
                               ELDT_ViewTable_ColumnOrder_TABLE_ID,
                               sTableID,
                               aRegionMem,
                               sViewList,
                               aEnv )
             == STL_SUCCESS );

    /**
     * Output 설정
     */

    *aAffectedViewList    = sViewList;
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}



/**
 * @brief DROP non-shema object 로 인해 영향을 받는 Outer View Handle List 를 획득
 * @param[in]  aTransID               Transaction ID
 * @param[in]  aStmt                  Statement
 * @param[in]  aRelationList          Relation(Table or View) List
 * @param[in]  aRegionMem             Region Memory
 * @param[out] aAffectedViewList      Affected View List
 * @param[in]  aEnv                   Environment
 * @remarks
 */
stlStatus ellGetAffectedOuterViewList4DropNonSchema( smlTransId          aTransID,
                                                     smlStatement      * aStmt,
                                                     ellObjectList     * aRelationList,
                                                     knlRegionMem      * aRegionMem,
                                                     ellObjectList    ** aAffectedViewList,
                                                     ellEnv            * aEnv )
{
    ellObjectList * sViewList = NULL;
    ellObjectList * sObjectList = NULL;

    ellObjectItem   * sRelationItem = NULL;
    ellDictHandle   * sRelationHandle = NULL;
    
    ellObjectItem   * sObjectItem = NULL;
    ellDictHandle   * sObjectHandle = NULL;

    stlBool    sDuplicate;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRelationList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAffectedViewList != NULL, ELL_ERROR_STACK(aEnv) );

    STL_TRY( ellInitObjectList( & sViewList, aRegionMem, aEnv ) == STL_SUCCESS );
    
    /**********************************************
     * Relation List 의 영향을 받는 View Object List 획득 
     **********************************************/

    STL_RING_FOREACH_ENTRY( & aRelationList->mHeadList, sRelationItem )
    {
        sRelationHandle = & sRelationItem->mObjectHandle;

        /**
         * Relation 을 사용하는 View 정보 획득
         */
        
        sObjectList = NULL;
        STL_TRY( ellGetAffectedViewListByRelation( aTransID,
                                                   aStmt,
                                                   sRelationHandle,
                                                   aRegionMem,
                                                   & sObjectList,
                                                   aEnv )
                 == STL_SUCCESS );

        /**
         * View Object List 의 중복 제거 
         */
        
        STL_RING_FOREACH_ENTRY( & sObjectList->mHeadList, sObjectItem )
        {
            sObjectHandle = & sObjectItem->mObjectHandle;

            if ( ellObjectExistInObjectList( ELL_OBJECT_TABLE,
                                             ellGetTableID( sObjectHandle ),
                                             aRelationList )
                 == STL_TRUE )
            {
                /**
                 * aRelationList 에 존재하는 Inner View 임
                 */
            }
            else
            {
                /**
                 * 중복을 제거한 View List 에 추가 
                 */
                STL_TRY( ellAddNewObjectItem( sViewList,
                                              sObjectHandle,
                                              & sDuplicate,
                                              aRegionMem,
                                              aEnv )
                         == STL_SUCCESS );
            }
        }
    }
        
    /**
     * Output 설정
     */

    *aAffectedViewList    = sViewList;
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/*********************************************************
 * View 에 대한 DDL Lock 함수 
 *********************************************************/


/**
 * @brief CREATE VIEW 구문을 위한 DDL Lock 을 획득한다.
 * @param[in]  aTransID        Transaction ID
 * @param[in]  aStmt           Statement
 * @param[in]  aOwnerHandle    View 의 Owner Handle
 * @param[in]  aSchemaHandle   View 의 Schema Handle
 * @param[out] aLockSuccess    Lock 획득 여부 
 * @param[in]  aEnv            Environment
 * @remarks
 */
stlStatus ellLock4CreateView( smlTransId      aTransID,
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
    
    /**
     * Owner 가 변경되지 않아야 함.
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
     * Schema 가 변경되지 않아야 함.
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
 * @brief DROP VIEW 구문을 위한 DDL Lock 을 획득한다.
 * @param[in]  aTransID      Transaction ID
 * @param[in]  aStmt         Statement
 * @param[in]  aViewHandle   View Dictionary Handle
 * @param[out] aLockSuccess  Lock 획득 여부 
 * @param[in]  aEnv          Environment
 * @remarks
 */
stlStatus ellLock4DropView( smlTransId      aTransID,
                            smlStatement  * aStmt,
                            ellDictHandle * aViewHandle,
                            stlBool       * aLockSuccess,
                            ellEnv        * aEnv )
{
    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aViewHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aLockSuccess != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * View Record 에 X lock
     * - Table 객체와 달리 객체가 존재하지 않음
     */
    
    STL_TRY( eldLockRowByObjectHandle( aTransID,
                                       aStmt,
                                       ELDT_TABLE_ID_TABLES,
                                       ELDT_Tables_ColumnOrder_TABLE_ID,
                                       aViewHandle,
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
 * @brief ALTER VIEW 구문을 위한 DDL Lock 을 획득한다.
 * @param[in]  aTransID      Transaction ID
 * @param[in]  aStmt         Statement
 * @param[in]  aViewHandle   View Dictionary Handle
 * @param[out] aLockSuccess  Lock 획득 여부 
 * @param[in]  aEnv          Environment
 * @remarks
 */
stlStatus ellLock4AlterView( smlTransId      aTransID,
                             smlStatement  * aStmt,
                             ellDictHandle * aViewHandle,
                             stlBool       * aLockSuccess,
                             ellEnv        * aEnv )
{
    return ellLock4DropView( aTransID,
                             aStmt,
                             aViewHandle,
                             aLockSuccess,
                             aEnv );
}


/*********************************************************
 * Dictionary 변경 함수 
 *********************************************************/



/**
 * @brief View Descriptor 를 Dictionary 에 추가한다.
 * @todo Performance View 등과 같이 fixed table을 포함한 View를 구별할 필요가 있다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aOwnerID         Owner의 ID
 * @param[in]  aSchemaID        Schema ID
 * @param[in]  aViewColumns     View Column List (nullable)
 * @param[in]  aViewedTableID   Viewed Table 의 ID (table descriptor 추가후 얻은 Table ID)
 * @param[in]  aViewString      CREATE VIEW 구문내에서 SELECT 구문만을 의미함.
 * @param[in]  aIsCompiled      Compile 성공 여부 
 * @param[in]  aIsAffected      Underlying Object 에 의해 영향을 받았는지의 여부
 * @param[in]  aEnv             Execution Library Environment
 * @remarks
 */

stlStatus ellInsertViewDesc( smlTransId     aTransID,
                             smlStatement * aStmt,
                             stlInt64       aOwnerID,
                             stlInt64       aSchemaID,
                             stlInt64       aViewedTableID,
                             stlChar      * aViewColumns,
                             stlChar      * aViewString,
                             stlBool        aIsCompiled,
                             stlBool        aIsAffected,
                             ellEnv       * aEnv )
{
    STL_TRY( eldInsertViewDesc( aTransID,
                                aStmt,
                                aOwnerID,
                                aSchemaID,
                                aViewedTableID,
                                aViewColumns,
                                aViewString,
                                aIsCompiled,
                                aIsAffected,
                                aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/**
 * @brief  DROP VIEW 를 위한 Dictionary Record 를 삭제한다.
 * @param[in]  aTransID        Transaction ID
 * @param[in]  aStmt           Statement
 * @param[in]  aViewHandle     View Handle
 * @param[in]  aEnv            Envirionment 
 * @remarks
 */
stlStatus ellRemoveDict4DropView( smlTransId        aTransID,
                                  smlStatement    * aStmt,
                                  ellDictHandle   * aViewHandle,
                                  ellEnv          * aEnv )
{
    stlInt64   sViewID = ELL_DICT_OBJECT_ID_NA;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aViewHandle != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */

    sViewID = ellGetTableID( aViewHandle );
    
    /***************************************************************
     * View Column 정보 제거
     ***************************************************************/

    /**
     * Column Privilege Record
     * - View 의 Column Privilege 는 존재하지 않음 
     */
    
    /**
     * Column Dictionary Record 를 삭제 
     */
    
    /*
     * DELETE FROM DEFINITION_SCHEMA.DATA_TYPE_DESCRIPTOR
     *  WHERE TABLE_ID = sViewID;
     */
    
    STL_TRY( eldDeleteObjectRows( aTransID,
                                  aStmt,
                                  ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                  ELDT_DTDesc_ColumnOrder_TABLE_ID,
                                  sViewID,
                                  aEnv )
             == STL_SUCCESS );

    /*
     * DELETE FROM DEFINITION_SCHEMA.COLUMNS
     *  WHERE TABLE_ID = sViewID;
     */
    
    STL_TRY( eldDeleteObjectRows( aTransID,
                                  aStmt,
                                  ELDT_TABLE_ID_COLUMNS,
                                  ELDT_Columns_ColumnOrder_TABLE_ID,
                                  sViewID,
                                  aEnv )
             == STL_SUCCESS );
    
    /*********************************************************
     * VIEW USAGE 정보를 삭제
     *********************************************************/
    
    /*
     * DELETE FROM DEFINITION_SCHEMA.VIEW_TABLE_USAGE
     *  WHERE VIEW_ID = sViewID;
     */
    
    STL_TRY( eldDeleteObjectRows( aTransID,
                                  aStmt,
                                  ELDT_TABLE_ID_VIEW_TABLE_USAGE,
                                  ELDT_ViewTable_ColumnOrder_VIEW_ID,
                                  sViewID,
                                  aEnv )
             == STL_SUCCESS );

    /*********************************************************
     * VIEW 정보를 삭제
     *********************************************************/
    
    /*
     * DELETE FROM DEFINITION_SCHEMA.VIEWS
     *  WHERE TABLE_ID = sViewID;
     */
    
    STL_TRY( eldDeleteObjectRows( aTransID,
                                  aStmt,
                                  ELDT_TABLE_ID_VIEWS,
                                  ELDT_Views_ColumnOrder_TABLE_ID,
                                  sViewID,
                                  aEnv )
             == STL_SUCCESS );

    /*********************************************************
     * Table 정보를 삭제
     *********************************************************/
    
    /*
     * DELETE FROM DEFINITION_SCHEMA.TABLE_PRIVILEGES
     *  WHERE TABLE_ID = sViewID;
     */
    
    STL_TRY( eldDeletePrivRowsAndCache( aTransID,
                                        aStmt,
                                        ELDT_TABLE_ID_TABLE_PRIVILEGES,
                                        ELDT_TablePriv_ColumnOrder_TABLE_ID,
                                        sViewID,
                                        aEnv )
             == STL_SUCCESS );
    
    /*
     * DELETE FROM DEFINITION_SCHEMA.TABLES
     *  WHERE TABLE_ID = sViewID;
     */
    
    STL_TRY( eldDeleteObjectRows( aTransID,
                                  aStmt,
                                  ELDT_TABLE_ID_TABLES,
                                  ELDT_Tables_ColumnOrder_TABLE_ID,
                                  sViewID,
                                  aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}



/**
 * @brief  ALTER VIEW 를 위한 Dictionary Record 를 삭제한다.
 * @param[in]  aTransID        Transaction ID
 * @param[in]  aStmt           Statement
 * @param[in]  aViewHandle     View Handle
 * @param[in]  aEnv            Envirionment 
 * @remarks
 */
stlStatus ellRemoveDict4AlterView( smlTransId        aTransID,
                                   smlStatement    * aStmt,
                                   ellDictHandle   * aViewHandle,
                                   ellEnv          * aEnv )
{
    stlInt64   sViewID = ELL_DICT_OBJECT_ID_NA;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aViewHandle != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */

    sViewID = ellGetTableID( aViewHandle );
    
    /***************************************************************
     * View Column 정보 제거
     ***************************************************************/

    /**
     * Column Privilege Record
     * - View 의 Column Privilege 는 존재하지 않음 
     */
    
    /**
     * Column Dictionary Record 를 삭제 
     */
    
    /*
     * DELETE FROM DEFINITION_SCHEMA.DATA_TYPE_DESCRIPTOR
     *  WHERE TABLE_ID = sViewID;
     */
    
    STL_TRY( eldDeleteObjectRows( aTransID,
                                  aStmt,
                                  ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                  ELDT_DTDesc_ColumnOrder_TABLE_ID,
                                  sViewID,
                                  aEnv )
             == STL_SUCCESS );

    /*
     * DELETE FROM DEFINITION_SCHEMA.COLUMNS
     *  WHERE TABLE_ID = sViewID;
     */
    
    STL_TRY( eldDeleteObjectRows( aTransID,
                                  aStmt,
                                  ELDT_TABLE_ID_COLUMNS,
                                  ELDT_Columns_ColumnOrder_TABLE_ID,
                                  sViewID,
                                  aEnv )
             == STL_SUCCESS );
    
    /*********************************************************
     * VIEW USAGE 정보를 삭제
     *********************************************************/
    
    /*
     * DELETE FROM DEFINITION_SCHEMA.VIEW_TABLE_USAGE
     *  WHERE VIEW_ID = sViewID;
     */
    
    STL_TRY( eldDeleteObjectRows( aTransID,
                                  aStmt,
                                  ELDT_TABLE_ID_VIEW_TABLE_USAGE,
                                  ELDT_ViewTable_ColumnOrder_VIEW_ID,
                                  sViewID,
                                  aEnv )
             == STL_SUCCESS );

    /*********************************************************
     * VIEW 정보를 삭제
     *********************************************************/
    
    /*
     * DELETE FROM DEFINITION_SCHEMA.VIEWS
     *  WHERE TABLE_ID = sViewID;
     */
    
    STL_TRY( eldDeleteObjectRows( aTransID,
                                  aStmt,
                                  ELDT_TABLE_ID_VIEWS,
                                  ELDT_Views_ColumnOrder_TABLE_ID,
                                  sViewID,
                                  aEnv )
             == STL_SUCCESS );

    /*********************************************************
     * Table 정보
     *********************************************************/
    
    /**
     * Table Privilege 정보는 유지한다.
     */

    /**
     * Table 정보는 유지한다.
     */
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}



/**
 * @brief  CREATE OR REPLACE VIEW 를 위한 Dictionary Record 를 삭제한다.
 * @param[in]  aTransID        Transaction ID
 * @param[in]  aStmt           Statement
 * @param[in]  aViewHandle     View Handle
 * @param[in]  aEnv            Envirionment 
 * @remarks
 */
stlStatus ellRemoveDict4ReplaceView( smlTransId        aTransID,
                                     smlStatement    * aStmt,
                                     ellDictHandle   * aViewHandle,
                                     ellEnv          * aEnv )
{
    return ellRemoveDict4AlterView( aTransID,
                                    aStmt,
                                    aViewHandle,
                                    aEnv );
    
}


/**
 * @brief View List 가 영향을 받았음을 설정한다.
 * @param[in]  aTransID           Transaction ID
 * @param[in]  aStmt              Statement
 * @param[in]  aAffectedViewList  Affected View List
 * @param[in]  aEnv               Envirionment 
 * @remarks
 */
stlStatus ellSetAffectedViewList( smlTransId          aTransID,
                                  smlStatement      * aStmt,
                                  ellObjectList     * aAffectedViewList,
                                  ellEnv            * aEnv )
{
    ellObjectItem * sObjectItem = NULL;
    ellDictHandle * sObjectHandle = NULL;

    stlInt64        sTableID = ELL_DICT_OBJECT_ID_NA;
    
    stlBool      sAffected = STL_TRUE;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAffectedViewList != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * View 목록을 순회하며 IS_AFFECTED 를 TRUE 로 설정
     */

    STL_RING_FOREACH_ENTRY( & aAffectedViewList->mHeadList, sObjectItem )
    {
        sObjectHandle = & sObjectItem->mObjectHandle;

        sTableID = ellGetTableID( sObjectHandle );
        
        STL_TRY( eldModifyDictDesc( aTransID,
                                    aStmt,
                                    ELDT_TABLE_ID_VIEWS,
                                    ELDT_Views_ColumnOrder_TABLE_ID,
                                    sTableID,
                                    ELDT_Views_ColumnOrder_IS_AFFECTED,
                                    & sAffected,  
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
 * @brief CREATE VIEW 를 위한 SQL-Table Cache 를 추가한다.
 * @param[in]  aTransID        Transaction ID
 * @param[in]  aStmt           Statement
 * @param[in]  aViewedTableID  Viewed Table ID
 * @param[in]  aEnv            Envirionment 
 * @remarks
 */
stlStatus ellRefineCache4CreateView( smlTransId           aTransID,
                                     smlStatement       * aStmt,
                                     stlInt64             aViewedTableID,
                                     ellEnv             * aEnv )
{
    /**
     * 새로운 Cache 를 구축 
     */

    STL_TRY( eldBuildViewCache( aTransID,
                                aStmt,
                                aViewedTableID,
                                aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}



/**
 * @brief VIEW 의 Column Cache 를 제거
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aViewColumnList  View Column List
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus ellRefineCache4RemoveViewColumns( smlTransId        aTransID,
                                            smlStatement    * aStmt,
                                            ellObjectList   * aViewColumnList,
                                            ellEnv          * aEnv )
{
    ellObjectItem * sObjectItem = NULL;
    ellDictHandle * sObjectHandle = NULL;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aViewColumnList != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Column 목록을 순회하며 Column Cache 를 삭제 
     */

    STL_RING_FOREACH_ENTRY( & aViewColumnList->mHeadList, sObjectItem )
    {
        sObjectHandle = & sObjectItem->mObjectHandle;

        STL_TRY( eldcDeleteCacheColumn( aTransID,
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
 * @brief DROP VIEW 에 대한 Cache 를 재구성
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aViewHandle      View Dictionary Handle
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus ellRefineCache4DropView( smlTransId        aTransID,
                                   ellDictHandle   * aViewHandle,
                                   ellEnv          * aEnv )
{
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aViewHandle != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * SQL-Table Cache 를 삭제 
     */
    
    STL_TRY( eldcDeleteCacheTable( aTransID,
                                   aViewHandle,
                                   aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/**
 * @brief CREATE OR REPLACE VIEW 에 대한 Cache 를 재구성
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aViewHandle      View Dictionary Handle
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus ellRefineCache4ReplaceView( smlTransId        aTransID,
                                      smlStatement    * aStmt,
                                      ellDictHandle   * aViewHandle,
                                      ellEnv          * aEnv )
{
    stlInt64   sViewID       = ELL_DICT_OBJECT_ID_NA;

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aViewHandle != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * 기본 정보 획득
     */

    sViewID = ellGetTableID( aViewHandle );
    
    /**
     * 기존 Dictionary Cache 를 제거
     */

    STL_TRY( ellRefineCache4DropView( aTransID,
                                      aViewHandle,
                                      aEnv )
             == STL_SUCCESS );
    
    /**
     * 새로운 Dictionary Cache 를 구축
     */
    
    STL_TRY( ellRefineCache4CreateView( aTransID,
                                        aStmt,
                                        sViewID,
                                        aEnv )
             == STL_SUCCESS );

    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}



/**
 * @brief ALTER VIEW .. COMPILE 에 대한 Cache 를 재구성
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aViewHandle      View Dictionary Handle
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus ellRefineCache4AlterViewCompile( smlTransId        aTransID,
                                           smlStatement    * aStmt,
                                           ellDictHandle   * aViewHandle,
                                           ellEnv          * aEnv )
{
    return ellRefineCache4ReplaceView( aTransID,
                                       aStmt,
                                       aViewHandle,
                                       aEnv );
}


/**
 * @brief Non-Service Performance View 에 대한 Cache 를 구성
 * @param[in]  aOwnerID           Owner의 ID
 * @param[in]  aSchemaID          Schema ID
 * @param[in]  aViewName          View Name
 * @param[in]  aViewColumnString  View Column Definition
 * @param[in]  aViewQueryString   View Query Definition
 * @param[in]  aEnv               Environment
 * @remarks
 */
stlStatus ellAddNonServicePerfViewCache( stlInt64       aOwnerID,
                                         stlInt64       aSchemaID,
                                         stlChar      * aViewName,
                                         stlChar      * aViewColumnString,
                                         stlChar      * aViewQueryString,
                                         ellEnv       * aEnv )
{
    stlInt64 sViewID = 0;

    eldMemMark sMemMark;
    
    knlValueBlockList * sDataValueList  = NULL;

    stlInt32 sState = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aViewName != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aViewColumnString != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aViewQueryString != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * 새로운 Table ID 를 획득
     */

    gEllIsMetaBuilding = STL_TRUE;
    
    STL_TRY( eldGetNewTableID( & sViewID, aEnv ) == STL_SUCCESS );

    /**
     * - Data Value List 공간 할당
     */
    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_TABLES,
                                     & sMemMark,
                                     & sDataValueList,
                                     aEnv )
             == STL_SUCCESS );
    sState = 1;
    
    /**
     * - Table descriptor의 정보를 구성한다.
     */
    
    STL_TRY( eldMakeValueList4TableDesc( sDataValueList,
                                         aOwnerID,
                                         aSchemaID,
                                         sViewID,
                                         ELL_DICT_TABLESPACE_ID_NA,  /* aTablespaceID */
                                         ELL_DICT_OBJECT_ID_NA,      /* aPhysicalID */
                                         aViewName,
                                         ELL_TABLE_TYPE_VIEW,
                                         NULL,                       /* aTableComment */
                                         aEnv )
             == STL_SUCCESS );

    /**
     * - Non-Service Performance View 를 Table Cache 에 추가
     */

    STL_TRY( eldcInsertCacheNonServicePerfView( sDataValueList,
                                                aViewColumnString,
                                                aViewQueryString,
                                                aEnv )
             == STL_SUCCESS );
    
    /**
     * 사용한 메모리를 해제한다.
     */

    sState = 0;
    STL_TRY( eldFreeTableValueList( & sMemMark, aEnv ) == STL_SUCCESS );

    /**
     * Pending Operation Reset
     */

    STL_TRY( elgResetSessionPendOp4NotOpenPhase( ELL_ENV(aEnv) ) == STL_SUCCESS );
    
    gEllIsMetaBuilding = STL_FALSE;
    
    return STL_SUCCESS;

    STL_FINISH;

    switch (sState)
    {
        case 1:
            (void) eldFreeTableValueList( & sMemMark, aEnv );
        default:
            break;
    }
    
    gEllIsMetaBuilding = STL_FALSE;
    
    return STL_FAILURE;
}

/*********************************************************
 * 정보 획득 함수 
 *********************************************************/



/**
 * @brief View 의 View Column List 문장을 얻는다.
 * @param[in] aTableDictHandle Table Dictionary Handle
 * @return View Column List String
 * @remarks
 */
stlChar * ellGetViewColumnString( ellDictHandle * aTableDictHandle )
{
    ellTableDesc * sTableDesc = (ellTableDesc *) ellGetObjectDesc( aTableDictHandle );
    STL_DASSERT( sTableDesc->mTableType == ELL_TABLE_TYPE_VIEW );

    return sTableDesc->mTableTypeDesc.mView.mViewColumnString;
}


/**
 * @brief View 의 SELECT 문장을 얻는다.
 * @param[in] aTableDictHandle Table Dictionary Handle
 * @return View Select String
 * @remarks
 */
stlChar * ellGetViewQueryString( ellDictHandle * aTableDictHandle )
{
    ellTableDesc * sTableDesc = (ellTableDesc *) ellGetObjectDesc( aTableDictHandle );
    STL_DASSERT( sTableDesc->mTableType == ELL_TABLE_TYPE_VIEW );

    return sTableDesc->mTableTypeDesc.mView.mViewQueryString;
}



/** @} ellObjectView */
