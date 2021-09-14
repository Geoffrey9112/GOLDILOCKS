/*******************************************************************************
 * ellDictAuthorization.c
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
 * @file ellDictAuthorization.c
 * @brief Authorization Dictionary 관리 모듈
 */

#include <ell.h>
#include <elDef.h>

#include <eldt.h>
#include <eldc.h>
#include <eldDictionary.h>

#include <eldTable.h>
#include <eldSchema.h>
#include <eldAuthorization.h>


/**
 * @addtogroup ellObjectAuth
 * @{
 */

const stlChar * const gEllAuthorizationTypeString[ELL_AUTHORIZATION_TYPE_MAX] =
{
    "N/A",    /* ELL_AUTHORIZATION_TYPE_NOT_AVAILABLE */
    
    "USER",   /* ELL_AUTHORIZATION_TYPE_USER */
    "ROLE",   /* ELL_AUTHORIZATION_TYPE_ROLE */
    "SYSTEM", /* ELL_AUTHORIZATION_TYPE_SYSTEM */
    "PUBLIC"  /* ELL_AUTHORIZATION_TYPE_ALL_USER */
};



/*********************************************************
 * DDL 관련 Object 획득 함수
 *********************************************************/

/**
 * @brief User 가 소유한 Schema List 획득
 * @param[in]  aTransID               Transaction ID
 * @param[in]  aStmt                  Statement
 * @param[in]  aUserHandle            User Dictionary Handle
 * @param[in]  aRegionMem             Region Memory
 * @param[out] aSchemaList            Schema List
 * @param[in]  aEnv                   Environment
 * @remarks
 */
stlStatus ellGetUserOwnedSchemaList( smlTransId       aTransID,
                                     smlStatement   * aStmt,
                                     ellDictHandle  * aUserHandle,
                                     knlRegionMem   * aRegionMem,
                                     ellObjectList ** aSchemaList,
                                     ellEnv         * aEnv )
{
    stlInt64   sUserID = ELL_DICT_OBJECT_ID_NA;

    ellObjectList * sSchemaList = NULL;

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aUserHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSchemaList != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */

    sUserID = ellGetAuthID( aUserHandle );

    /*********************************************
     * sSchemaList
     *********************************************/

    /**
     * - sSchemaList : User 가 소유한 Schema 목록
     *
     * <BR> SELECT sch.SCHEMA_ID
     * <BR>   FROM SCHEMATA sch
     * <BR>  WHERE sch.OWNER_ID = sUserID
     */

    STL_TRY( ellInitObjectList( & sSchemaList, aRegionMem, aEnv ) == STL_SUCCESS );

    STL_TRY( eldGetObjectList( aTransID,
                               aStmt,
                               ELDT_TABLE_ID_SCHEMATA,
                               ELL_OBJECT_SCHEMA,
                               ELDT_Schemata_ColumnOrder_SCHEMA_ID,
                               ELDT_Schemata_ColumnOrder_OWNER_ID,
                               sUserID,
                               aRegionMem,
                               sSchemaList,
                               aEnv )
             == STL_SUCCESS );

    /**
     * Output 설정
     */

    *aSchemaList = sSchemaList;
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}



/**
 * @brief DROP USER 에 의해 영향을 받는 Object Handle List 를 획득
 * @param[in]  aTransID               Transaction ID
 * @param[in]  aStmt                  Statement
 * @param[in]  aUserHandle            User Dictionary Handle
 * @param[in]  aRegionMem             Region Memory
 * @param[out] aInnerSeqList                   User 가 소유한 sequence list
 * @param[out] aInnerSynonymList               User 가 소유한 synonym list
 * @param[out] aInnerTableList                 User 가 소유한 table list
 * @param[out] aOuterTableInnerPrimaryKeyList  User 가 소유한 PK 이나 table 이 다른 User 의 소유 
 * @param[out] aOuterTableInnerUniqueKeyList   User 가 소유한 UK 이나 table 이 다른 User 의 소유 
 * @param[out] aOuterTableInnerIndexList       User 가 소유한 index 이나 table 이 다른 User 의 소유 
 * @param[out] aOuterTableInnerNotNullList     User 가 소유한 not null 이나 table 이 다른 User 의 소유 
 * @param[out] aOuterTableInnerCheckClauseList User 가 소유한 check 이나 table 이 다른 User 의 소유 
 * @param[out] aInnerForeignKeyList            User 가 소유한 Foreign Key list
 * @param[out] aOuterChildForeignKeyList       다른 User 가 소유한 Child Foreign key이나 User 가 소유한 객체를 참조
 * @param[out] aPrivObjectList                 User Privilege 가 포함된 객체 목록
 * @param[in]  aEnv                   Environment
 * @remarks
 */
stlStatus ellGetObjectList4DropUser( smlTransId       aTransID,
                                     smlStatement   * aStmt,
                                     ellDictHandle  * aUserHandle,
                                     knlRegionMem   * aRegionMem,
                                     ellObjectList ** aInnerSeqList,
                                     ellObjectList ** aInnerSynonymList,
                                     ellObjectList ** aInnerTableList,
                                     ellObjectList ** aOuterTableInnerPrimaryKeyList,
                                     ellObjectList ** aOuterTableInnerUniqueKeyList,
                                     ellObjectList ** aOuterTableInnerIndexList,
                                     ellObjectList ** aOuterTableInnerNotNullList,
                                     ellObjectList ** aOuterTableInnerCheckClauseList,
                                     ellObjectList ** aInnerForeignKeyList,
                                     ellObjectList ** aOuterChildForeignKeyList,
                                     ellObjectList ** aPrivObjectList,
                                     ellEnv         * aEnv )
{
    stlInt64   sUserID = ELL_DICT_OBJECT_ID_NA;

    ellObjectList * sInnerSeqList = NULL;
    ellObjectList * sInnerSynonymList = NULL;
    
    ellObjectList * sInnerTableList = NULL;
    ellObjectList * sInnerTableUniqueList = NULL;
    
    ellObjectList * sInnerIndexOrientedList = NULL;
    ellObjectList * sOuterTableInnerIndexList = NULL;
    
    ellObjectList * sInnerConstList = NULL;
    ellObjectList * sOuterTableInnerUniqueList = NULL;
    ellObjectList * sOuterTableInnerPKList = NULL;
    ellObjectList * sOuterTableInnerUKList = NULL;
    ellObjectList * sOuterTableInnerNotNullList = NULL;
    ellObjectList * sOuterTableInnerCheckClauseList = NULL;

    ellObjectList * sInnerForeignKeyList = NULL;
    ellObjectList * sChildForeignKeyList    = NULL;
    ellObjectList * sOuterChildForeignKeyList = NULL;


    ellObjectList * sPrivObjectList = NULL;
    
    ellObjectList * sObjectList = NULL;
    
    ellObjectItem   * sObjectItem = NULL;
    ellDictHandle   * sObjectHandle = NULL;
    ellDictHandle   * sKeyObjHandle = NULL;
    ellDictHandle   * sIndexHandle = NULL;

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aUserHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInnerSeqList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInnerSynonymList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInnerTableList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOuterTableInnerPrimaryKeyList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOuterTableInnerUniqueKeyList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOuterTableInnerIndexList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOuterTableInnerNotNullList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOuterTableInnerCheckClauseList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInnerForeignKeyList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOuterChildForeignKeyList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPrivObjectList != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */

    sUserID = ellGetAuthID( aUserHandle );

    /*********************************************
     * sInnerSeqList
     *********************************************/
    
    /**
     * - sInnerSeqList : User 가 소유한 Sequence 목록
     *
     * <BR> CREATE VIEW AS VIEW_INNER_SEQ_LIST AS
     * <BR>
     * <BR> SELECT s.SEQUENCE_ID
     * <BR>   FROM SEQUENCES s
     * <BR>  WHERE s.OWNER_ID = sUserID
     */

    STL_TRY( ellInitObjectList( & sInnerSeqList, aRegionMem, aEnv ) == STL_SUCCESS );
    
    STL_TRY( eldGetObjectList( aTransID,
                               aStmt,
                               ELDT_TABLE_ID_SEQUENCES,
                               ELL_OBJECT_SEQUENCE,
                               ELDT_Sequences_ColumnOrder_SEQUENCE_ID,
                               ELDT_Sequences_ColumnOrder_OWNER_ID,
                               sUserID,
                               aRegionMem,
                               sInnerSeqList,
                               aEnv )
             == STL_SUCCESS );

     /*********************************************
     * sInnerSynonymList
     *********************************************/
    
    /**
     * - sInnerSynonymList : User 가 소유한 Synonym 목록
     *
     * <BR> CREATE VIEW AS VIEW_INNER_SYNONYM_LIST AS
     * <BR>
     * <BR> SELECT s.SYNONYM_ID
     * <BR>   FROM SYNONYMS s
     * <BR>  WHERE s.OWNER_ID = sUserID
     */

    STL_TRY( ellInitObjectList( & sInnerSynonymList, aRegionMem, aEnv ) == STL_SUCCESS );
    
    STL_TRY( eldGetObjectList( aTransID,
                               aStmt,
                               ELDT_TABLE_ID_SYNONYMS,
                               ELL_OBJECT_SYNONYM,
                               ELDT_Synonyms_ColumnOrder_SYNONYM_ID,
                               ELDT_Synonyms_ColumnOrder_OWNER_ID,
                               sUserID,
                               aRegionMem,
                               sInnerSynonymList,
                               aEnv )
             == STL_SUCCESS );
    
    /*********************************************
     * sInnerTableList
     *********************************************/
    
    /**
     * - sInnerTableList : User 가 소유한 Table 목록
     *
     * <BR> CREATE VIEW AS VIEW_INNER_TABLE_LIST AS
     * <BR>
     * <BR> SELECT t.TABLE_ID
     * <BR>   FROM TABLES t
     * <BR>  WHERE t.OWNER_ID = sUserID
     */

    STL_TRY( ellInitObjectList( & sInnerTableList, aRegionMem, aEnv ) == STL_SUCCESS );
    
    STL_TRY( eldGetObjectList( aTransID,
                               aStmt,
                               ELDT_TABLE_ID_TABLES,
                               ELL_OBJECT_TABLE,
                               ELDT_Tables_ColumnOrder_TABLE_ID,
                               ELDT_Tables_ColumnOrder_OWNER_ID,
                               sUserID,
                               aRegionMem,
                               sInnerTableList,
                               aEnv )
             == STL_SUCCESS );

    /*********************************************
     * sInnerTableUniqueList
     *********************************************/

    /**
     * - sInnerTableUniqueList : User 가 소유한 Table 들이 가진 Unique 목록
     *
     * <BR> CREATE VIEW AS VIEW_INNER_TABLE_UNIQUE_LIST AS
     * <BR>
     * <BR> SELECT c.CONSTRAINT_ID
     * <BR>   FROM VIEW_INNER_TABLE_LIST vitl, TABLE_CONSTRAINTS c
     * <BR>  WHERE vitl.TABLE_ID = c.TABLE_ID
     * <BR>    AND c.CONSTRAINT_TYPE IN ( 'PRIMARY KEY', 'UNIQUE' )
     */

    STL_TRY( ellInitObjectList( & sInnerTableUniqueList, aRegionMem, aEnv )
             == STL_SUCCESS );

    /**
     * Inner Table List 를 순회하며, Primary Key 와 Unique Key 들을 Merge 함
     */
    
    STL_RING_FOREACH_ENTRY( & sInnerTableList->mHeadList, sObjectItem )
    {
        sObjectHandle = & sObjectItem->mObjectHandle;

        /**
         * Primary Key 정보를 Merge
         */

        STL_TRY( ellInitObjectList( & sObjectList, aRegionMem, aEnv )
                 == STL_SUCCESS );

        STL_TRY( ellGetTableConstTypeList( aTransID,
                                           aStmt,
                                           ellGetTableID( sObjectHandle ),
                                           ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY,
                                           aRegionMem,
                                           & sObjectList,
                                           aEnv )
                 == STL_SUCCESS );
        
        ellMergeObjectList( sInnerTableUniqueList, sObjectList );
        
        /**
         * Unique Key 정보를 Merge
         */

        STL_TRY( ellInitObjectList( & sObjectList, aRegionMem, aEnv )
                 == STL_SUCCESS );

        STL_TRY( ellGetTableConstTypeList( aTransID,
                                           aStmt,
                                           ellGetTableID( sObjectHandle ),
                                           ELL_TABLE_CONSTRAINT_TYPE_UNIQUE_KEY,
                                           aRegionMem,
                                           & sObjectList,
                                           aEnv )
                 == STL_SUCCESS );

        ellMergeObjectList( sInnerTableUniqueList, sObjectList );
    }
    
    
    
    /*********************************************
     * sInnerIndexOrientedList
     *********************************************/

    /**
     * - sInnerIndexOrientedList : User 가 소유한 index-oriented object list
     *
     * <BR> CREATE VIEW AS VIEW_INNER_INDEX_ORIENTED_LIST AS
     * <BR>
     * <BR> SELECT n.INDEX_ID
     * <BR>   FROM INDEXES n
     * <BR>  WHERE n.OWNER_ID = sUserID
     */

    STL_TRY( ellInitObjectList( & sInnerIndexOrientedList, aRegionMem, aEnv )
             == STL_SUCCESS );

    STL_TRY( eldGetObjectList( aTransID,
                               aStmt,
                               ELDT_TABLE_ID_INDEXES,
                               ELL_OBJECT_INDEX,
                               ELDT_Indexes_ColumnOrder_INDEX_ID,
                               ELDT_Indexes_ColumnOrder_OWNER_ID,
                               sUserID,
                               aRegionMem,
                               sInnerIndexOrientedList,
                               aEnv )
             == STL_SUCCESS );
    
    /*********************************************
     * sOuterTableInnerIndexList
     *********************************************/

    /**
     * - sOuterTableInnerIndexList
     *   : User 가 소유한 index-oriented object 이나 다른 User 의 Table 에 속한 Object
     * 
     * <BR> CREATE VIEW AS VIEW_OUTER_TABLE_INNER_INDEX_ORIENTED_LIST AS
     * <BR>
     * <BR> SELECT n.INDEX_ID
     * <BR>   FROM VIEW_INNER_INDEX_ORIENTED_LIST viiol, INDEX_KEY_TABLE_USAGE k
     * <BR>  WHERE viiol.INDEX_ID = k.INDEX_ID
     * <BR>    AND k.BY_CONSTRAINT = FALSE
     * <BR>    AND k.TABLE_ID
     * <BR>        NOT IN ( SELECT TABLE_ID FROM VIEW_INNER_TABLE_LIST )
     */

    STL_TRY( ellInitObjectList( & sOuterTableInnerIndexList, aRegionMem, aEnv )
             == STL_SUCCESS );
    
    /**
     * Inner Index-Oriented Ojbect List 를 순회하며, Outer Table 에 해당하는 목록을 추출 
     */
    
    STL_RING_FOREACH_ENTRY( & sInnerIndexOrientedList->mHeadList, sObjectItem )
    {
        sObjectHandle = & sObjectItem->mObjectHandle;

        if ( ellObjectExistInObjectList( ELL_OBJECT_TABLE,
                                         ellGetIndexTableID( sObjectHandle ),
                                         sInnerTableList )
             == STL_TRUE )
        {
            /**
             * Inner Table 의 index-oriented object
             * - nothing to do 
             */
        }
        else
        {
            /**
             * Outer Table 의 index-oriented object
             */
            
            if ( ellGetIndexIsByConstraint( sObjectHandle ) == STL_TRUE )
            {
                /**
                 * Key Constraint 를 위한 Index 임
                 */
            }
            else
            {
                /**
                 * Normal Index 임
                 */
                
                STL_TRY( ellAddNewObjectItem( sOuterTableInnerIndexList,
                                              sObjectHandle,
                                              NULL,
                                              aRegionMem,
                                              aEnv )
                         == STL_SUCCESS );
            }
        }
    }
    
    /*********************************************
     * sInnerConstList
     *********************************************/
    
    /**
     * - sInnerConstList : User 가 소유한 Constraint 목록
     *
     * <BR> CREATE VIEW AS VIEW_INNER_CONST_LIST AS
     * <BR>
     * <BR> SELECT t.CONSTRAINT_ID
     * <BR>   FROM TABLE_CONSTRAINTS tc
     * <BR>  WHERE tc.CONSTRAINT_OWNER_ID = sUserID
     */

    STL_TRY( ellInitObjectList( & sInnerConstList, aRegionMem, aEnv ) == STL_SUCCESS );
    
    STL_TRY( eldGetObjectList( aTransID,
                               aStmt,
                               ELDT_TABLE_ID_TABLE_CONSTRAINTS,
                               ELL_OBJECT_CONSTRAINT,
                               ELDT_TableConst_ColumnOrder_CONSTRAINT_ID,
                               ELDT_TableConst_ColumnOrder_CONSTRAINT_OWNER_ID,
                               sUserID,
                               aRegionMem,
                               sInnerConstList,
                               aEnv )
             == STL_SUCCESS );

    /*********************************************
     * sInnerConstList 로부터 다음을 분류
     * - sOuterTableInnerUniqueList
     * - sOuterTableInnerPKList
     * - sOuterTableInnerUKList
     * - sOuterTableInnerNotNullList
     * - sOuterTableInnerCheckClauseList
     *********************************************/

    /**
     * - sInnerConstList 를 순회하며 분류 
     */
    
    STL_TRY( ellInitObjectList( & sOuterTableInnerUniqueList, aRegionMem, aEnv )
             == STL_SUCCESS );
    STL_TRY( ellInitObjectList( & sOuterTableInnerPKList, aRegionMem, aEnv )
             == STL_SUCCESS );
    STL_TRY( ellInitObjectList( & sOuterTableInnerUKList, aRegionMem, aEnv )
             == STL_SUCCESS );
    STL_TRY( ellInitObjectList( & sOuterTableInnerNotNullList, aRegionMem, aEnv )
             == STL_SUCCESS );
    STL_TRY( ellInitObjectList( & sOuterTableInnerCheckClauseList, aRegionMem, aEnv )
             == STL_SUCCESS );

    STL_RING_FOREACH_ENTRY( & sInnerConstList->mHeadList, sObjectItem )
    {
        sObjectHandle = & sObjectItem->mObjectHandle;

        if ( ellGetConstraintTableOwnerID( sObjectHandle ) == sUserID )
        {
            /**
             * Inner Table 의 Constraint 임 
             */
        }
        else
        {
            /**
             * Outer Table 의 Inner Constraint 임 
             */
            switch( ellGetConstraintType( sObjectHandle ) )
            {
                case ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY:
                    {
                        STL_TRY( ellAddNewObjectItem( sOuterTableInnerUniqueList,
                                                      sObjectHandle,
                                                      NULL,
                                                      aRegionMem,
                                                      aEnv )
                                 == STL_SUCCESS );
                        
                        STL_TRY( ellAddNewObjectItem( sOuterTableInnerPKList,
                                                      sObjectHandle,
                                                      NULL,
                                                      aRegionMem,
                                                      aEnv )
                                 == STL_SUCCESS );
                        
                        break;
                    }
                case ELL_TABLE_CONSTRAINT_TYPE_UNIQUE_KEY:
                    {
                        STL_TRY( ellAddNewObjectItem( sOuterTableInnerUniqueList,
                                                      sObjectHandle,
                                                      NULL,
                                                      aRegionMem,
                                                      aEnv )
                                 == STL_SUCCESS );
                        
                        STL_TRY( ellAddNewObjectItem( sOuterTableInnerUKList,
                                                      sObjectHandle,
                                                      NULL,
                                                      aRegionMem,
                                                      aEnv )
                                 == STL_SUCCESS );
                        break;
                    }
                case ELL_TABLE_CONSTRAINT_TYPE_FOREIGN_KEY:
                    {
                        /**
                         * nothing to do
                         */
                        break;
                    }
                case ELL_TABLE_CONSTRAINT_TYPE_CHECK_NOT_NULL:
                    {
                        /**
                         * sOuterTableInnerNotNullList 에 추가
                         */

                        STL_TRY( ellAddNewObjectItem( sOuterTableInnerNotNullList,
                                                      sObjectHandle,
                                                      NULL,
                                                      aRegionMem,
                                                      aEnv )
                                 == STL_SUCCESS );
                        
                        break;
                    }
                case ELL_TABLE_CONSTRAINT_TYPE_CHECK_CLAUSE:
                    {
                        /**
                         * sOuterTableInnerCheckClauseList 에 추가
                         */
                        
                        STL_TRY( ellAddNewObjectItem( sOuterTableInnerCheckClauseList,
                                                      sObjectHandle,
                                                      NULL,
                                                      aRegionMem,
                                                      aEnv )
                                 == STL_SUCCESS );
                        break;
                    }
                default:
                    {
                        STL_ASSERT(0);
                        break;
                    }
            }
        }
    }
    
    /*********************************************
     * sInnerForeignKeyList
     *********************************************/

    /**
     * <BR> SELECT c.CONSTRAINT_ID
     * <BR>   FROM VIEW_INNER_INDEX_ORIENTED_LIST viiol, TABLE_CONSTRAINTS c
     * <BR>  WHERE viiol.BY_CONSTRAINT = true
     * <BR>    AND viiol.INDEX_ID = c.ASSOCIATED_INDEX_ID
     * <BR>    AND c.CONSTRAINT_TYPE IN ( 'FOREIGN KEY' )
     */
    
    STL_TRY( ellInitObjectList( & sInnerForeignKeyList, aRegionMem, aEnv )
             == STL_SUCCESS );

    /**
     * Inner Index-Oriented Ojbect List 를 순회하며, Inner Foreign Key 에 해당하는 목록을 추출 
     */
    
    STL_RING_FOREACH_ENTRY( & sInnerIndexOrientedList->mHeadList, sObjectItem )
    {
        sObjectHandle = & sObjectItem->mObjectHandle;

        if ( ellGetIndexIsByConstraint( sObjectHandle ) == STL_TRUE )
        {
            /**
             * Key Index 임
             */

            STL_TRY( ellInitObjectList( & sObjectList, aRegionMem, aEnv )
                     == STL_SUCCESS );
            
            STL_TRY( eldGetObjectList( aTransID,
                                       aStmt,
                                       ELDT_TABLE_ID_TABLE_CONSTRAINTS,
                                       ELL_OBJECT_CONSTRAINT,
                                       ELDT_TableConst_ColumnOrder_CONSTRAINT_ID,
                                       ELDT_TableConst_ColumnOrder_ASSOCIATED_INDEX_ID,
                                       ellGetIndexID(sObjectHandle),
                                       aRegionMem,
                                       sObjectList,
                                       aEnv )
                     == STL_SUCCESS );

            /* 1 개의 key constraint 만 존재함 */
            sKeyObjHandle = ellGetNthObjectHandle( sObjectList, 0 );

            STL_TRY_THROW( sKeyObjHandle != NULL, RAMP_ERR_OBJECT_MODIFIED );
            
            switch( ellGetConstraintType( sKeyObjHandle ) )
            {
                case ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY:
                case ELL_TABLE_CONSTRAINT_TYPE_UNIQUE_KEY:
                    {
                        /**
                         * nothing to do
                         */
                        break;
                    }
                case ELL_TABLE_CONSTRAINT_TYPE_FOREIGN_KEY:
                    {
                        STL_TRY( ellAddNewObjectItem( sInnerForeignKeyList,
                                                      sKeyObjHandle,
                                                      NULL,
                                                      aRegionMem,
                                                      aEnv )
                                 == STL_SUCCESS );
                        
                        break;
                    }
                default:
                    {
                        STL_ASSERT(0);
                        break;
                    }
            }
        }
        else
        {
            /**
             * Normal (Unique or Non-Unique) Index 임
             * - nothing to do
             */
        }
    }
    
    /*********************************************
     * sChildForeignKeyList
     *********************************************/

    /**
     * - sChildForeignKeyList : User 와 관련된 모든 Child Foreign Key List
     *
     * <BR> CREATE VIEW VIEW_CHILD_FOREIGN_KEY_LIST AS
     * <BR>
     * <BR> SELECT r.CONSTRAINT_ID
     * <BR>   FROM VIEW_INNER_TABLE_UNIQUE_LIST vitul, REFERENTIAL_CONSTRAINTS r
     * <BR>  WHERE vitul.CONSTRAINT_ID = r.UNIQUE_CONSTRAINT_ID
     * <BR> UNION ALL
     * <BR> SELECT r.CONSTRAINT_ID
     * <BR>   FROM VIEW_OUTER_TABLE_INNER_UNIQUE_LIST votiul, REFERENTIAL_CONSTRAINTS r
     * <BR>  WHERE votiul.CONSTRAINT_ID = r.UNIQUE_CONSTRAINT_ID
     */

    STL_TRY( ellInitObjectList( & sChildForeignKeyList, aRegionMem, aEnv )
             == STL_SUCCESS );

    /**
     * Inner Table Unique List 로부터 Child Foreign Key 병합
     */
    
    sObjectList = NULL;
    STL_TRY( ellGetChildForeignKeyListByUniqueList( aTransID,
                                                    aStmt,
                                                    sInnerTableUniqueList,
                                                    aRegionMem,
                                                    & sObjectList,
                                                    aEnv )
             == STL_SUCCESS );
    ellMergeObjectList( sChildForeignKeyList, sObjectList );

    /**
     * Outer Table Inner Unique List 로부터 Child Foreign Key 병합
     */
    
    sObjectList = NULL;
    STL_TRY( ellGetChildForeignKeyListByUniqueList( aTransID,
                                                    aStmt,
                                                    sOuterTableInnerUniqueList,
                                                    aRegionMem,
                                                    & sObjectList,
                                                    aEnv )
             == STL_SUCCESS );
    ellMergeObjectList( sChildForeignKeyList, sObjectList );
    
    
    /*********************************************
     * sOuterChildForeignKeyList
     *********************************************/

    /**
     * - sOuterChildForeignKeyList : 다른 User 가 소유한 Child Foreign Key 이나 User 가 소유한 객체와 관련 있는 list
     *
     * <BR> SELECT vrkl.CONSTRAINT_ID
     * <BR>   FROM VIEW_CHILD_FOREIGN_KEY_LIST vrkl
     * <BR>  WHERE vrkl.ASSOCIATED_INDEX_ID
     * <BR>        NOT IN ( SELECT INDEX_ID FROM VIEW_INNER_INDEX_ORIENTED_LIST )
     */
    
    STL_TRY( ellInitObjectList( & sOuterChildForeignKeyList, aRegionMem, aEnv )
             == STL_SUCCESS );

    /**
     * sChildForeignKeyList 를 순회하며, Outer Child Foreign Key 에 해당하는 목록을 추출 
     */
    
    STL_RING_FOREACH_ENTRY( & sChildForeignKeyList->mHeadList, sObjectItem )
    {
        sObjectHandle = & sObjectItem->mObjectHandle;

        sIndexHandle = ellGetConstraintIndexDictHandle( sObjectHandle );
        
        if ( ellObjectExistInObjectList( ELL_OBJECT_INDEX,
                                         ellGetIndexID( sIndexHandle ),
                                         sInnerIndexOrientedList )
             == STL_TRUE )
        {
            /**
             * User 가 소유한 Child Foreign Key 임
             * - nothing to do 
             */
        }
        else
        {
            /**
             * User 외부의 Child Foreign Key 이면서, User 내부 객체를 참조하는 Child Foreign Key 임 
             */

            STL_TRY( ellAddNewObjectItem( sOuterChildForeignKeyList,
                                          sObjectHandle,
                                          NULL,
                                          aRegionMem,
                                          aEnv )
                     == STL_SUCCESS );
        }
    }

    /*********************************************
     * sPrivObjectList
     *********************************************/

    STL_TRY( ellInitObjectList( & sPrivObjectList, aRegionMem, aEnv )
             == STL_SUCCESS );

    /**
     * User 가 Grantor/Grantee 인 Tablespace Object 목록 획득
     */
    
    STL_TRY( eldGetObjectList( aTransID,
                               aStmt,
                               ELDT_TABLE_ID_TABLESPACE_PRIVILEGES,
                               ELL_OBJECT_TABLESPACE,
                               ELDT_SpacePriv_ColumnOrder_TABLESPACE_ID,
                               ELDT_SpacePriv_ColumnOrder_GRANTOR_ID,
                               sUserID,
                               aRegionMem,
                               sPrivObjectList,
                               aEnv )
             == STL_SUCCESS );

    STL_TRY( eldGetObjectList( aTransID,
                               aStmt,
                               ELDT_TABLE_ID_TABLESPACE_PRIVILEGES,
                               ELL_OBJECT_TABLESPACE,
                               ELDT_SpacePriv_ColumnOrder_TABLESPACE_ID,
                               ELDT_SpacePriv_ColumnOrder_GRANTEE_ID,
                               sUserID,
                               aRegionMem,
                               sPrivObjectList,
                               aEnv )
             == STL_SUCCESS );

    /**
     * User 가 Grantor/Grantee 인 Schema Object 목록 획득
     */
    
    STL_TRY( eldGetObjectList( aTransID,
                               aStmt,
                               ELDT_TABLE_ID_SCHEMA_PRIVILEGES,
                               ELL_OBJECT_SCHEMA,
                               ELDT_SchemaPriv_ColumnOrder_SCHEMA_ID,
                               ELDT_SchemaPriv_ColumnOrder_GRANTOR_ID,
                               sUserID,
                               aRegionMem,
                               sPrivObjectList,
                               aEnv )
             == STL_SUCCESS );

    STL_TRY( eldGetObjectList( aTransID,
                               aStmt,
                               ELDT_TABLE_ID_SCHEMA_PRIVILEGES,
                               ELL_OBJECT_SCHEMA,
                               ELDT_SchemaPriv_ColumnOrder_SCHEMA_ID,
                               ELDT_SchemaPriv_ColumnOrder_GRANTEE_ID,
                               sUserID,
                               aRegionMem,
                               sPrivObjectList,
                               aEnv )
             == STL_SUCCESS );

    /**
     * User 가 Grantor/Grantee 인 Table Object 목록 획득
     */
    
    STL_TRY( eldGetObjectList( aTransID,
                               aStmt,
                               ELDT_TABLE_ID_TABLE_PRIVILEGES,
                               ELL_OBJECT_TABLE,
                               ELDT_TablePriv_ColumnOrder_TABLE_ID,
                               ELDT_TablePriv_ColumnOrder_GRANTOR_ID,
                               sUserID,
                               aRegionMem,
                               sPrivObjectList,
                               aEnv )
             == STL_SUCCESS );

    STL_TRY( eldGetObjectList( aTransID,
                               aStmt,
                               ELDT_TABLE_ID_TABLE_PRIVILEGES,
                               ELL_OBJECT_TABLE,
                               ELDT_TablePriv_ColumnOrder_TABLE_ID,
                               ELDT_TablePriv_ColumnOrder_GRANTEE_ID,
                               sUserID,
                               aRegionMem,
                               sPrivObjectList,
                               aEnv )
             == STL_SUCCESS );

    /**
     * User 가 Grantor/Grantee 인 Column 의 Table Object 목록 획득
     */
    
    STL_TRY( eldGetObjectList( aTransID,
                               aStmt,
                               ELDT_TABLE_ID_COLUMN_PRIVILEGES,
                               ELL_OBJECT_TABLE,
                               ELDT_ColumnPriv_ColumnOrder_TABLE_ID,
                               ELDT_ColumnPriv_ColumnOrder_GRANTOR_ID,
                               sUserID,
                               aRegionMem,
                               sPrivObjectList,
                               aEnv )
             == STL_SUCCESS );

    STL_TRY( eldGetObjectList( aTransID,
                               aStmt,
                               ELDT_TABLE_ID_COLUMN_PRIVILEGES,
                               ELL_OBJECT_TABLE,
                               ELDT_ColumnPriv_ColumnOrder_TABLE_ID,
                               ELDT_ColumnPriv_ColumnOrder_GRANTEE_ID,
                               sUserID,
                               aRegionMem,
                               sPrivObjectList,
                               aEnv )
             == STL_SUCCESS );
    
    /**
     * User 가 Grantor/Grantee 인 Column 의 Sequence Object 목록 획득
     */
    
    /**
     * @todo
     * usage privilege 객체로 UDT(User Defined Type), Domain 등이 구현될 경우
     * 해당 로직은 변경되어야 함.
     */
    
    STL_TRY( eldGetObjectList( aTransID,
                               aStmt,
                               ELDT_TABLE_ID_USAGE_PRIVILEGES,
                               ELL_OBJECT_SEQUENCE,
                               ELDT_UsagePriv_ColumnOrder_OBJECT_ID,
                               ELDT_UsagePriv_ColumnOrder_GRANTOR_ID,
                               sUserID,
                               aRegionMem,
                               sPrivObjectList,
                               aEnv )
             == STL_SUCCESS );

    STL_TRY( eldGetObjectList( aTransID,
                               aStmt,
                               ELDT_TABLE_ID_USAGE_PRIVILEGES,
                               ELL_OBJECT_SEQUENCE,
                               ELDT_UsagePriv_ColumnOrder_OBJECT_ID,
                               ELDT_UsagePriv_ColumnOrder_GRANTEE_ID,
                               sUserID,
                               aRegionMem,
                               sPrivObjectList,
                               aEnv )
             == STL_SUCCESS );
    
    /*********************************************
     * Output 설정
     *********************************************/

    *aInnerSeqList                   = sInnerSeqList;
    *aInnerSynonymList               = sInnerSynonymList;
    *aInnerTableList                 = sInnerTableList;
    *aOuterTableInnerPrimaryKeyList  = sOuterTableInnerPKList;
    *aOuterTableInnerUniqueKeyList   = sOuterTableInnerUKList;
    *aOuterTableInnerIndexList       = sOuterTableInnerIndexList;
    *aOuterTableInnerNotNullList     = sOuterTableInnerNotNullList;
    *aOuterTableInnerCheckClauseList = sOuterTableInnerCheckClauseList;
    *aInnerForeignKeyList            = sInnerForeignKeyList;
    *aOuterChildForeignKeyList       = sOuterChildForeignKeyList;
    *aPrivObjectList                 = sPrivObjectList;
    
    
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



/*********************************************************
 * DDL Lock 함수 
 *********************************************************/


/**
 * @brief CREATE USER 구문을 위한 DDL Lock 을 획득한다.
 * @param[in]  aTransID          Transaction ID
 * @param[in]  aStmt             Statement
 * @param[in]  aCreatorHandle    Creator Handle
 * @param[in]  aDataSpaceHandle  Data Space Handle
 * @param[in]  aTempSpaceHandle  Temp Space Handle
 * @param[in]  aProfileHandle    Profile Handle (nullable)
 * @param[out] aLockSuccess      Lock 획득 여부 
 * @param[in]  aEnv              Environment
 * @remarks
 */
stlStatus ellLock4CreateUser( smlTransId      aTransID,
                              smlStatement  * aStmt,
                              ellDictHandle * aCreatorHandle,
                              ellDictHandle * aDataSpaceHandle,
                              ellDictHandle * aTempSpaceHandle,
                              ellDictHandle * aProfileHandle,
                              stlBool       * aLockSuccess,
                              ellEnv        * aEnv )
{
    stlBool  sLocked = STL_TRUE;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCreatorHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataSpaceHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTempSpaceHandle != NULL, ELL_ERROR_STACK(aEnv) );
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
     * Data Tablespace Record 에 S lock
     */

    STL_TRY( eldLockRowByObjectHandle( aTransID,
                                       aStmt,
                                       ELDT_TABLE_ID_TABLESPACES,
                                       ELDT_Space_ColumnOrder_TABLESPACE_ID,
                                       aDataSpaceHandle,
                                       SML_LOCK_S,
                                       & sLocked,
                                       aEnv )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_FINISH );
    
    /**
     * Temp Tablespace Record 에 S lock
     */

    STL_TRY( eldLockRowByObjectHandle( aTransID,
                                       aStmt,
                                       ELDT_TABLE_ID_TABLESPACES,
                                       ELDT_Space_ColumnOrder_TABLESPACE_ID,
                                       aTempSpaceHandle,
                                       SML_LOCK_S,
                                       & sLocked,
                                       aEnv )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_FINISH );

    /**
     * Profile Record 에 S lock
     */

    if ( aProfileHandle == NULL )
    {
        sLocked = STL_TRUE;
    }
    else
    {
        STL_TRY( eldLockRowByObjectHandle( aTransID,
                                           aStmt,
                                           ELDT_TABLE_ID_PROFILES,
                                           ELDT_Profile_ColumnOrder_PROFILE_ID,
                                           aProfileHandle,
                                           SML_LOCK_S,
                                           & sLocked,
                                           aEnv )
                 == STL_SUCCESS );
    }

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
 * @brief CREATE ROLE 구문을 위한 DDL Lock 을 획득한다.
 * @todo implement
 * @remarks
 */
stlStatus ellLock4CreateRole()
{
    return STL_FAILURE;
}


/**
 * @brief DROP USER 구문을 위한 DDL Lock 을 획득한다.
 * @param[in]  aTransID          Transaction ID
 * @param[in]  aStmt             Statement
 * @param[in]  aUserHandle       User Handle
 * @param[out] aLockSuccess      Lock 획득 여부 
 * @param[in]  aEnv              Environment
 * @remarks
 */
stlStatus ellLock4DropUser( smlTransId      aTransID,
                            smlStatement  * aStmt,
                            ellDictHandle * aUserHandle,
                            stlBool       * aLockSuccess,
                            ellEnv        * aEnv )
{
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aUserHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aLockSuccess != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * User 가 변경되지 않아야 함.
     */
    
    STL_TRY( eldLockRowByObjectHandle( aTransID,
                                       aStmt,
                                       ELDT_TABLE_ID_AUTHORIZATIONS,
                                       ELDT_Auth_ColumnOrder_AUTH_ID,
                                       aUserHandle,
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
 * @brief ALTER USER 구문을 위한 DDL Lock 을 획득한다.
 * @param[in]  aTransID          Transaction ID
 * @param[in]  aStmt             Statement
 * @param[in]  aUserHandle       User Handle
 * @param[out] aLockSuccess      Lock 획득 여부 
 * @param[in]  aEnv              Environment
 * @remarks
 */
stlStatus ellLock4AlterUser( smlTransId      aTransID,
                             smlStatement  * aStmt,
                             ellDictHandle * aUserHandle,
                             stlBool       * aLockSuccess,
                             ellEnv        * aEnv )
{
    return ellLock4DropUser( aTransID,
                             aStmt,
                             aUserHandle,
                             aLockSuccess,
                             aEnv );
}



/**
 * @brief GRANT PRIVILEGE 구문을 위한 DDL Lock 을 획득한다.
 * @param[in]  aTransID          Transaction ID
 * @param[in]  aStmt             Statement
 * @param[in]  aGrantorHandle    Grantor Handle
 * @param[in]  aGranteeList      Grantee List
 * @param[in]  aPrivObject       Privilege Object 유형
 * @param[in]  aObjectHandle     Object Handle
 * @param[out] aLockSuccess      Lock 획득 여부 
 * @param[in]  aEnv              Environment
 * @remarks
 */
stlStatus ellLock4GrantPriv( smlTransId      aTransID,
                             smlStatement  * aStmt,
                             ellDictHandle * aGrantorHandle,
                             ellObjectList * aGranteeList,
                             ellPrivObject   aPrivObject,
                             ellDictHandle * aObjectHandle,
                             stlBool       * aLockSuccess,
                             ellEnv        * aEnv )
{
    ellObjectItem * sObjectItem = NULL;
    ellDictHandle * sObjectHandle = NULL;

    stlBool   sLocked = STL_TRUE;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aGrantorHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aGranteeList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aPrivObject > ELL_PRIV_NA) && (aPrivObject < ELL_PRIV_OWNER), ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aLockSuccess != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * Grantor Record 에 S lock
     */
    
    STL_TRY( eldLockRowByObjectHandle( aTransID,
                                       aStmt,
                                       ELDT_TABLE_ID_AUTHORIZATIONS,
                                       ELDT_Auth_ColumnOrder_AUTH_ID,
                                       aGrantorHandle,
                                       SML_LOCK_S,
                                       & sLocked,
                                       aEnv )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_FINISH );

    /**
     * Grantee List 목록을 순회하며 S lock 
     */

    STL_RING_FOREACH_ENTRY( & aGranteeList->mHeadList, sObjectItem )
    {
        sObjectHandle = & sObjectItem->mObjectHandle;

        STL_TRY( eldLockRowByObjectHandle( aTransID,
                                           aStmt,
                                           ELDT_TABLE_ID_AUTHORIZATIONS,
                                           ELDT_Auth_ColumnOrder_AUTH_ID,
                                           sObjectHandle,
                                           SML_LOCK_S,
                                           & sLocked,
                                           aEnv )
                 == STL_SUCCESS );

        STL_TRY_THROW( sLocked == STL_TRUE, RAMP_FINISH );
    }

    /**
     * Privilege Object 가 구조는 변경되지 않고 DML 은 수행할 수 있도록 lock 을 획득
     */

    switch ( aPrivObject )
    {
        case ELL_PRIV_DATABASE:
            {
                /**
                 * nothing to do
                 * - @todo DROP DATABASE 구문이 지원되면 고민해야 할 지도 
                 */
                break;
            }
        case ELL_PRIV_SPACE:
            {
                /**
                 * Tablespace Dictionary Row 에 S row lock 획득
                 */
                
                STL_TRY( eldLockRowByObjectHandle( aTransID,
                                                   aStmt,
                                                   ELDT_TABLE_ID_TABLESPACES,
                                                   ELDT_Space_ColumnOrder_TABLESPACE_ID,
                                                   aObjectHandle,
                                                   SML_LOCK_S,
                                                   & sLocked,
                                                   aEnv )
                         == STL_SUCCESS );
                
                STL_TRY_THROW( sLocked == STL_TRUE, RAMP_FINISH );
                
                break;
            }
        case ELL_PRIV_SCHEMA:
            {
                /**
                 * Schema Dictionary Row 에 S row lock 획득
                 */
                
                STL_TRY( eldLockRowByObjectHandle( aTransID,
                                                   aStmt,
                                                   ELDT_TABLE_ID_SCHEMATA,
                                                   ELDT_Schemata_ColumnOrder_SCHEMA_ID,
                                                   aObjectHandle,
                                                   SML_LOCK_S,
                                                   & sLocked,
                                                   aEnv )
                         == STL_SUCCESS );

                STL_TRY_THROW( sLocked == STL_TRUE, RAMP_FINISH );
                
                break;
            }
        case ELL_PRIV_TABLE:
            {
                /**
                 * Table 객체에 IS lock
                 * - DDL 은 방지하더라도, DML 은 수행될 수 있어야 함.
                 */
                
                STL_TRY( eldLockTable4DDL( aTransID,
                                           aStmt,
                                           SML_LOCK_IS,
                                           aObjectHandle,
                                           & sLocked,
                                           aEnv )
                         == STL_SUCCESS );

                STL_TRY_THROW( sLocked == STL_TRUE, RAMP_FINISH );
                
                break;
            }
        case ELL_PRIV_USAGE:
            {
                /**
                 * Sequence Dictionary Row 에 S row lock 획득
                 */
                
                STL_TRY( eldLockRowByObjectHandle( aTransID,
                                                   aStmt,
                                                   ELDT_TABLE_ID_SEQUENCES,
                                                   ELDT_Sequences_ColumnOrder_SEQUENCE_ID,
                                                   aObjectHandle,
                                                   SML_LOCK_S,
                                                   & sLocked,
                                                   aEnv )
                         == STL_SUCCESS );

                STL_TRY_THROW( sLocked == STL_TRUE, RAMP_FINISH );
                
                break;
            }
        default:
            {
                STL_ASSERT(0);
                break;
            }
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
 * @brief REVOKE PRIVILEGE 구문을 위한 DDL Lock 을 획득한다.
 * @param[in]  aTransID          Transaction ID
 * @param[in]  aStmt             Statement
 * @param[in]  aRevokerHandle    Revoker Handle
 * @param[in]  aRevokeeList      Revokee List
 * @param[in]  aPrivObject       Privilege Object 유형
 * @param[in]  aObjectHandle     Object Handle
 * @param[out] aLockSuccess      Lock 획득 여부 
 * @param[in]  aEnv              Environment
 * @remarks
 */
stlStatus ellLock4RevokePriv( smlTransId      aTransID,
                              smlStatement  * aStmt,
                              ellDictHandle * aRevokerHandle,
                              ellObjectList * aRevokeeList,
                              ellPrivObject   aPrivObject,
                              ellDictHandle * aObjectHandle,
                              stlBool       * aLockSuccess,
                              ellEnv        * aEnv )
{
    return ellLock4GrantPriv( aTransID,
                              aStmt,
                              aRevokerHandle,
                              aRevokeeList,
                              aPrivObject,
                              aObjectHandle,
                              aLockSuccess,
                              aEnv );
}


/**
 * @brief DROP USER/ROLE 등을 위해 Privilege Object List 에 대해 S 또는 IS lock 을 획득한다.
 * @param[in]  aTransID          Transaction ID
 * @param[in]  aStmt             Statement
 * @param[in]  aPrivObjectList   Privilege Object List
 * @param[out] aLockSuccess      Lock 획득 여부 
 * @param[in]  aEnv              Environment
 * @remarks
 */
stlStatus ellLock4PrivObjectList( smlTransId      aTransID,
                                  smlStatement  * aStmt,
                                  ellObjectList * aPrivObjectList,
                                  stlBool       * aLockSuccess,
                                  ellEnv        * aEnv )
{
    ellObjectItem * sObjectItem = NULL;
    ellDictHandle * sObjectHandle = NULL;

    stlBool  sLocked = STL_TRUE;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPrivObjectList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aLockSuccess != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * Privilege Object List 목록을 순회하며 IS lock 
     */

    STL_RING_FOREACH_ENTRY( & aPrivObjectList->mHeadList, sObjectItem )
    {
        sObjectHandle = & sObjectItem->mObjectHandle;

        switch (sObjectHandle->mObjectType)
        {
            case ELL_OBJECT_COLUMN:
                {
                    STL_DASSERT(0);
                    break;
                }
            case ELL_OBJECT_TABLE:
                {
                    STL_TRY( eldLockTable4DDL( aTransID,
                                               aStmt,
                                               SML_LOCK_IS,
                                               sObjectHandle,
                                               & sLocked,
                                               aEnv )
                             == STL_SUCCESS );
                    
                    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_FINISH );
                    
                    break;
                }
            case ELL_OBJECT_CONSTRAINT:
                {
                    STL_DASSERT(0);
                    break;
                }
            case ELL_OBJECT_INDEX:
                {
                    STL_DASSERT(0);
                    break;
                }
            case ELL_OBJECT_SEQUENCE:
                {
                    STL_TRY( eldLockRowByObjectHandle( aTransID,
                                                       aStmt,
                                                       ELDT_TABLE_ID_SEQUENCES,
                                                       ELDT_Sequences_ColumnOrder_SEQUENCE_ID,
                                                       sObjectHandle,
                                                       SML_LOCK_S,
                                                       & sLocked,
                                                       aEnv )
                             == STL_SUCCESS );
                    
                    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_FINISH );
                    
                    break;
                }
            case ELL_OBJECT_SCHEMA:
                {
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
                    
                    break;
                }
            case ELL_OBJECT_TABLESPACE:
                {
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
                    
                    break;
                }
            case ELL_OBJECT_AUTHORIZATION:
                {
                    STL_DASSERT(0);
                    break;
                }
            case ELL_OBJECT_CATALOG:
                {
                    STL_DASSERT(0);
                    break;
                }
            default:
                {
                    STL_DASSERT(0);
                    break;
                }
        }
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
 * Dictionary 변경 함수 
 *********************************************************/



/**
 * @brief Authorization Descriptor 를 Dictionary 에 추가한다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[out] aAuthID          Authorization 의 ID 
 * @param[in]  aAuthName        Authorization Name
 * @param[in]  aAuthType        Authorization Type (User or Role)
 * @param[in]  aAuthComment     Authorization Comment (nullable)
 * @param[in]  aEnv             Execution Library Environment
 * @remarks
 */

stlStatus ellInsertAuthDesc( smlTransId             aTransID,
                             smlStatement         * aStmt,
                             stlInt64             * aAuthID,
                             stlChar              * aAuthName,
                             ellAuthorizationType   aAuthType,
                             stlChar              * aAuthComment,
                             ellEnv               * aEnv )
{
    STL_TRY( eldInsertAuthDesc( aTransID,
                                aStmt,
                                aAuthID,
                                aAuthName,
                                aAuthType,
                                aAuthComment,
                                aEnv )
             == STL_SUCCESS );

    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}



/**
 * @brief DROP USER 를 위한 Dictionary Row 를 제거 
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aUserHandle      User Dictionary Handle
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus ellRemoveDict4DropUser( smlTransId        aTransID,
                                  smlStatement    * aStmt,
                                  ellDictHandle   * aUserHandle,
                                  ellEnv          * aEnv )
{
    stlInt64 sUserID = ELL_DICT_OBJECT_ID_NA;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aUserHandle != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */

    sUserID = ellGetAuthID( aUserHandle );
    
    /**
     * User Schema Path 제거 
     */

    /*
     * DELETE FROM DEFINITION_SCHEMA.USER_SCHEMA_PATH
     *  WHERE AUTH_ID = sUserID;
     */
    
    STL_TRY( eldDeleteObjectRows( aTransID,
                                  aStmt,
                                  ELDT_TABLE_ID_USER_SCHEMA_PATH,
                                  ELDT_UserPath_ColumnOrder_AUTH_ID,
                                  sUserID,
                                  aEnv )
             == STL_SUCCESS );

    /*
     * DELETE FROM DEFINITION_SCHEMA.USER_PASSWORD_HISTORY
     *  WHERE AUTH_ID = sUserID;
     */
    
    STL_TRY( eldDeleteObjectRows( aTransID,
                                  aStmt,
                                  ELDT_TABLE_ID_USER_PASSWORD_HISTORY,
                                  ELDT_PassHist_ColumnOrder_AUTH_ID,
                                  sUserID,
                                  aEnv )
             == STL_SUCCESS );
    
    /**
     * User Descriptor 제거 
     */

    /*
     * DELETE FROM DEFINITION_SCHEMA.USERS
     *  WHERE AUTH_ID = sUserID;
     */
    
    STL_TRY( eldDeleteObjectRows( aTransID,
                                  aStmt,
                                  ELDT_TABLE_ID_USERS,
                                  ELDT_User_ColumnOrder_AUTH_ID,
                                  sUserID,
                                  aEnv )
             == STL_SUCCESS );

    /**
     * Authorization Descriptor 제거 
     */

    /*
     * DELETE FROM DEFINITION_SCHEMA.AUTHORIZATIONS
     *  WHERE AUTH_ID = sUserID;
     */
    
    STL_TRY( eldDeleteObjectRows( aTransID,
                                  aStmt,
                                  ELDT_TABLE_ID_AUTHORIZATIONS,
                                  ELDT_Auth_ColumnOrder_AUTH_ID,
                                  sUserID,
                                  aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/**
 * @brief ALTER USER/ROLE 구문의 수행시간을 설정한다.
 * @param[in]  aTransID          Transaction ID
 * @param[in]  aStmt             Statement
 * @param[in]  aAuthID           Authorization ID
 * @param[in]  aEnv              Environment
 * @remarks
 */
stlStatus ellSetTimeAlterAuth( smlTransId      aTransID,
                               smlStatement  * aStmt,
                               stlInt64        aAuthID,
                               ellEnv        * aEnv )
{
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAuthID > ELL_DICT_OBJECT_ID_NA,
                        ELL_ERROR_STACK(aEnv) );
    
    /**
     * Authorization Record 가 이전에 변경되지 않아야 함.
     */
    
    STL_TRY( eldModifyDictDesc( aTransID,
                                aStmt,
                                ELDT_TABLE_ID_AUTHORIZATIONS,
                                ELDT_Auth_ColumnOrder_AUTH_ID,
                                aAuthID,
                                ELDT_Auth_ColumnOrder_MODIFIED_TIME,
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
 * @brief User 의 Default Data Tablespace 를 변경한다.
 * @param[in]  aTransID          Transaction ID
 * @param[in]  aStmt             Statement
 * @param[in]  aUserHandle       User Handle
 * @param[in]  aSpaceID          Tablespace ID
 * @param[in]  aEnv              Environment
 * @remarks
 */
stlStatus ellModifyUserDataSpace( smlTransId      aTransID,
                                  smlStatement  * aStmt,
                                  ellDictHandle * aUserHandle,
                                  stlInt64        aSpaceID,
                                  ellEnv        * aEnv )
{
    stlInt64 sUserID = ELL_DICT_OBJECT_ID_NA;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aUserHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSpaceID > ELL_DICT_TABLESPACE_ID_NA, ELL_ERROR_STACK(aEnv) );
    
    /**
     * 기본 정보 획득
     */

    sUserID = ellGetAuthID(aUserHandle);
    
    /**
     * User 의 Password 변경 
     */
    
    STL_TRY( eldModifyDictDesc( aTransID,
                                aStmt,
                                ELDT_TABLE_ID_USERS,
                                ELDT_User_ColumnOrder_AUTH_ID,
                                sUserID,
                                ELDT_User_ColumnOrder_DEFAULT_DATA_TABLESPACE_ID,
                                & aSpaceID,  
                                aEnv )
             == STL_SUCCESS );

    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/**
 * @brief User 의 Default Temp Tablespace 를 변경한다.
 * @param[in]  aTransID          Transaction ID
 * @param[in]  aStmt             Statement
 * @param[in]  aUserHandle       User Handle
 * @param[in]  aSpaceID          Tablespace ID
 * @param[in]  aEnv              Environment
 * @remarks
 */
stlStatus ellModifyUserTempSpace( smlTransId      aTransID,
                                  smlStatement  * aStmt,
                                  ellDictHandle * aUserHandle,
                                  stlInt64        aSpaceID,
                                  ellEnv        * aEnv )
{
    stlInt64 sUserID = ELL_DICT_OBJECT_ID_NA;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aUserHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSpaceID > ELL_DICT_TABLESPACE_ID_NA, ELL_ERROR_STACK(aEnv) );
    
    /**
     * 기본 정보 획득
     */

    sUserID = ellGetAuthID(aUserHandle);
    
    /**
     * User 의 Password 변경 
     */
    
    STL_TRY( eldModifyDictDesc( aTransID,
                                aStmt,
                                ELDT_TABLE_ID_USERS,
                                ELDT_User_ColumnOrder_AUTH_ID,
                                sUserID,
                                ELDT_User_ColumnOrder_DEFAULT_TEMP_TABLESPACE_ID,
                                & aSpaceID,  
                                aEnv )
             == STL_SUCCESS );

    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}



/**
 * @brief Authorization 의 Schema Path 를 변경한다.
 * @param[in]  aTransID            Transaction ID
 * @param[in]  aStmt               Statement
 * @param[in]  aAuthHandle         Authorization Handle
 * @param[in]  aSchemaCnt          Schema Count
 * @param[in]  aSchemaHandleArray  Schema Handle Array
 * @param[in]  aEnv                Environment
 * @remarks
 */
stlStatus ellModifyAuthSchemaPath( smlTransId      aTransID,
                                   smlStatement  * aStmt,
                                   ellDictHandle * aAuthHandle,
                                   stlInt32        aSchemaCnt,
                                   ellDictHandle * aSchemaHandleArray,
                                   ellEnv        * aEnv )
{
    stlInt64 sAuthID = ELL_DICT_OBJECT_ID_NA;

    stlInt32 i = 0;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAuthHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSchemaCnt > 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSchemaHandleArray != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * 기본 정보 획득
     */

    sAuthID = ellGetAuthID(aAuthHandle);
    
    /**
     * Authorization 의 Schema Path 제거 
     */
    
    STL_TRY( eldDeleteObjectRows( aTransID,
                                  aStmt,
                                  ELDT_TABLE_ID_USER_SCHEMA_PATH,
                                  ELDT_UserPath_ColumnOrder_AUTH_ID,
                                  sAuthID,
                                  aEnv )
             == STL_SUCCESS );

    /**
     * 새로운 Schema Path 추가 
     */
    
    for ( i = 0; i < aSchemaCnt; i++ )
    {
        STL_TRY( ellInsertUserSchemaPathDesc( aTransID,
                                              aStmt,
                                              sAuthID,
                                              ellGetSchemaID( & aSchemaHandleArray[i] ),
                                              i,
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
 * @brief SQL-User Cache 를 추가한다.
 * @param[in]  aTransID       Transaction ID
 * @param[in]  aStmt          Statement
 * @param[in]  aUserID        User Authorization ID
 * @param[in]  aEnv           Envirionment 
 * @remarks
 * SQL-Authorization Cache 를 추가한다.
 */
stlStatus ellRefineCache4AddUser( smlTransId           aTransID,
                                  smlStatement       * aStmt,
                                  stlInt64             aUserID,
                                  ellEnv             * aEnv )
{
    STL_TRY( eldRefineCache4AddAuth( aTransID,
                                     aStmt,
                                     aUserID,
                                     aEnv )
             == STL_SUCCESS );

    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/**
 * @brief SQL-Role Cache 를 추가한다.
 * @param[in]  aTransID       Transaction ID
 * @param[in]  aStmt          Statement
 * @param[in]  aRoleID        Role Authorization ID
 * @param[in]  aEnv           Envirionment 
 * @remarks
 * SQL-Authorization Cache 를 추가한다.
 */
stlStatus ellRefineCache4AddRole( smlTransId           aTransID,
                                  smlStatement       * aStmt,
                                  stlInt64             aRoleID,
                                  ellEnv             * aEnv )
{
    STL_TRY( eldRefineCache4AddAuth( aTransID,
                                     aStmt,
                                     aRoleID,
                                     aEnv )
             == STL_SUCCESS );

    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}

/**
 * @brief DROP USER 를 위한 Cache 재구성  
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aUserHandle      User Dictionary Handle
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus ellRefineCache4DropUser( smlTransId        aTransID,
                                   ellDictHandle   * aUserHandle,
                                   ellEnv          * aEnv )
{
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aUserHandle != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * SQL-Authorization Cache 정보를 삭제
     */
    
    STL_TRY( eldcDeleteCacheAuth( aTransID,
                                  aUserHandle,
                                  aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}



/**
 * @brief ALTER USER 를 위한 Cache 재구성  
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aUserHandle      User Dictionary Handle
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus ellRefineCache4AlterUser( smlTransId        aTransID,
                                    smlStatement    * aStmt,
                                    ellDictHandle   * aUserHandle,
                                    ellEnv          * aEnv )
{
    return ellRebuildAuthCache( aTransID,
                                aStmt,
                                aUserHandle,
                                aEnv );
}

/**
 * @brief Authorization Cache 재구축 
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aAuthHandle      Authorization Dictionary Handle
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus ellRebuildAuthCache( smlTransId        aTransID,
                               smlStatement    * aStmt,
                               ellDictHandle   * aAuthHandle,
                               ellEnv          * aEnv )
{
    stlInt64  sAuthID = ELL_DICT_OBJECT_ID_NA;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAuthHandle != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */
    
    sAuthID = ellGetAuthID( aAuthHandle );
    
    /**
     * SQL-Authorization Cache 정보를 삭제
     */
    
    STL_TRY( eldcDeleteCacheAuth( aTransID,
                                  aAuthHandle,
                                  aEnv )
             == STL_SUCCESS );

    /**
     * SQL-Authorization Cache 정보를 추가 
     */
    
    STL_TRY( eldRefineCache4AddAuth( aTransID,
                                     aStmt,
                                     sAuthID,
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
 * @brief Authorization Dictionary Handle 을 얻는다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aViewSCN         View SCN
 * @param[in]  aAuthName        Auth Name
 * @param[out] aAuthDictHandle  Authorization Dictionary Handle
 * @param[out] aExist           존재 여부 
 * @param[in]  aEnv             Envirionment 
 * @remarks
 */
stlStatus ellGetAuthDictHandle( smlTransId           aTransID,
                                smlScn               aViewSCN,
                                stlChar            * aAuthName,
                                ellDictHandle      * aAuthDictHandle,
                                stlBool            * aExist,
                                ellEnv             * aEnv )
{
    ellInitDictHandle( aAuthDictHandle );
    
    return eldGetAuthHandleByName( aTransID,
                                   aViewSCN,
                                   aAuthName,
                                   aAuthDictHandle,
                                   aExist,
                                   aEnv );

    
}


/**
 * @brief Authorization Dictionary Handle 을 얻는다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aViewSCN         View SCN
 * @param[in]  aAuthID          Auth ID
 * @param[out] aAuthDictHandle  Authorization Dictionary Handle
 * @param[out] aExist           존재 여부 
 * @param[in]  aEnv             Envirionment 
 * @remarks
 */
stlStatus ellGetAuthDictHandleByID( smlTransId           aTransID,
                                    smlScn               aViewSCN,
                                    stlInt64             aAuthID,
                                    ellDictHandle      * aAuthDictHandle,
                                    stlBool            * aExist,
                                    ellEnv             * aEnv )
{
    ellInitDictHandle( aAuthDictHandle );

    STL_TRY( eldGetAuthHandleByID( aTransID,
                                   aViewSCN,
                                   aAuthID,
                                   aAuthDictHandle,
                                   aExist,
                                   aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief User 의 Schema Path 에서 첫번째 Schema Dictionary Handle 을 얻는다.
 * @param[in]  aTransID          Transaction ID
 * @param[in]  aViewSCN          View SCN
 * @param[in]  aAuthDictHandle   Authorization Dictionary Handle
 * @param[out] aSchemaDictHandle Schema Dictionary Handle
 * @param[in]  aEnv              Environment
 * @remarks
 */
stlStatus ellGetAuthFirstSchemaDictHandle( smlTransId           aTransID,
                                           smlScn               aViewSCN,
                                           ellDictHandle      * aAuthDictHandle,
                                           ellDictHandle      * aSchemaDictHandle,
                                           ellEnv             * aEnv )
{
    ellAuthDesc * sAuthDesc = NULL;

    stlBool sSchemaExist = STL_FALSE;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aAuthDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSchemaDictHandle != NULL, ELL_ERROR_STACK(aEnv) );

    ellInitDictHandle( aSchemaDictHandle );
    
    /**
     * Authorization Descriptor
     */

    sAuthDesc = (ellAuthDesc *) ellGetObjectDesc( aAuthDictHandle );

    /**
     * 사용자 Schema Path 를 이용해 검색
     */
    
    STL_TRY( eldGetSchemaHandleByID( aTransID,
                                     aViewSCN,
                                     sAuthDesc->mSchemaIDArray[0],
                                     aSchemaDictHandle,
                                     & sSchemaExist,
                                     aEnv )
             == STL_SUCCESS );
    STL_TRY_THROW( sSchemaExist == STL_TRUE, RAMP_ERR_DROP_SCHEMA );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_DROP_SCHEMA )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ELL_ERRCODE_USER_DEFAULT_SCHEMA_DROPPED,
                      NULL,
                      ELL_ERROR_STACK(aEnv) );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Authorization 의 Default Data Tablespace Handle 획득
 * @param[in]  aTransID      Transaction ID
 * @param[in]  aViewSCN      View SCN
 * @param[in]  aAuthHandle   Authorization Handle
 * @param[out] aSpaceHandle  Default Data Space Handle
 * @param[in]  aEnv          Environment
 * @remarks
 */
stlStatus ellGetAuthDataSpaceHandle( smlTransId       aTransID,
                                     smlScn           aViewSCN,
                                     ellDictHandle  * aAuthHandle,
                                     ellDictHandle  * aSpaceHandle,
                                     ellEnv         * aEnv )
{
    ellAuthDesc * sAuthDesc = NULL;

    stlBool sSpaceExist = STL_FALSE;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aAuthHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSpaceHandle != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Authorization Descriptor
     */
    
    ellInitDictHandle( aSpaceHandle );
    
    sAuthDesc = (ellAuthDesc *) ellGetObjectDesc( aAuthHandle );

    /**
     * 사용자 Default Space ID 를 이용해 검색
     */
    
    STL_TRY( ellGetTablespaceDictHandleByID( aTransID,
                                             aViewSCN,
                                             sAuthDesc->mDataSpaceID,
                                             aSpaceHandle,
                                             & sSpaceExist,
                                             aEnv )
             == STL_SUCCESS );
    STL_TRY_THROW( sSpaceExist == STL_TRUE, RAMP_ERR_DROP_SPACE );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_DROP_SPACE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ELL_ERRCODE_USER_DEFAULT_DATA_TABLESPACE_DROPPED,
                      NULL,
                      ELL_ERROR_STACK(aEnv) );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Authorization 의 Default Temporary Tablespace Handle 획득
 * @param[in]  aTransID      Transaction ID
 * @param[in]  aViewSCN      View SCN
 * @param[in]  aAuthHandle   Authorization Handle
 * @param[out] aSpaceHandle  Temporary Space Handle
 * @param[in]  aEnv          Environment
 * @remarks
 */
stlStatus ellGetAuthTempSpaceHandle( smlTransId       aTransID,
                                     smlScn           aViewSCN,
                                     ellDictHandle  * aAuthHandle,
                                     ellDictHandle  * aSpaceHandle,
                                     ellEnv         * aEnv )
{
    ellAuthDesc * sAuthDesc = NULL;

    stlBool sSpaceExist = STL_FALSE;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aAuthHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSpaceHandle != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Authorization Descriptor
     */
    
    ellInitDictHandle( aSpaceHandle );
    
    sAuthDesc = (ellAuthDesc *) ellGetObjectDesc( aAuthHandle );

    /**
     * 사용자 Default Space ID 를 이용해 검색
     */
    
    STL_TRY( ellGetTablespaceDictHandleByID( aTransID,
                                             aViewSCN,
                                             sAuthDesc->mTempSpaceID,
                                             aSpaceHandle,
                                             & sSpaceExist,
                                             aEnv )
             == STL_SUCCESS );
    STL_TRY_THROW( sSpaceExist == STL_TRUE, RAMP_ERR_DROP_SPACE );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_DROP_SPACE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ELL_ERRCODE_USER_DEFAULT_TEMPORARY_TABLESPACE_DROPPED,
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
 * @brief Authorization ID 정보
 * @param[in] aAuthDictHandle  Authorization Dictionary Handle
 * @return Authorization ID
 * @remarks
 */
stlInt64  ellGetAuthID( ellDictHandle * aAuthDictHandle )
{
    ellAuthDesc * sAuthDesc = NULL;

    STL_DASSERT( aAuthDictHandle->mObjectType == ELL_OBJECT_AUTHORIZATION );
    
    sAuthDesc = (ellAuthDesc *) ellGetObjectDesc( aAuthDictHandle );

    return sAuthDesc->mAuthID;
}


/**
 * @brief Authorization Name 정보
 * @param[in] aAuthDictHandle  Authorization Dictionary Handle
 * @return Authorization Name
 * @remarks
 */
stlChar * ellGetAuthName( ellDictHandle * aAuthDictHandle )
{
    ellAuthDesc * sAuthDesc = NULL;

    STL_DASSERT( aAuthDictHandle->mObjectType == ELL_OBJECT_AUTHORIZATION );
    
    sAuthDesc = (ellAuthDesc *) ellGetObjectDesc( aAuthDictHandle );

    return sAuthDesc->mAuthName;
}


/**
 * @brief Authorization Type 정보
 * @param[in] aAuthDictHandle  Authorization Dictionary Handle
 * @return Authorization Type ( USER, ROLE, PUBLIC, ... )
 * @remarks
 */
ellAuthorizationType ellGetAuthType( ellDictHandle * aAuthDictHandle )
{
    ellAuthDesc * sAuthDesc = NULL;

    STL_DASSERT( aAuthDictHandle->mObjectType == ELL_OBJECT_AUTHORIZATION );
    
    sAuthDesc = (ellAuthDesc *) ellGetObjectDesc( aAuthDictHandle );

    return sAuthDesc->mAuthType;
}
    

/**
 * @brief Authorization 이 Login 가능한지의 여부 
 * @param[in] aAuthDictHandle  Authorization Dictionary Handle
 * @return
 * - STL_TRUE : Login 가능한 Authorization 임.
 * - STL_FALSE: Login 할 수 없는 Authorization 임. (ROLE, PUBLIC, SYSTEM 등)
 * @remarks
 * 사용자의 ELL_DB_PRIV_TYPE_CREATE_SESSION 권한 검사와는 별도로
 * _SYSTEM, PUBLIC 등의 특수 계정으로 접근하는 것을 방지하기 위한 API 임.
 */
stlBool ellGetAuthAbleLogin( ellDictHandle * aAuthDictHandle )
{
    ellAuthDesc * sAuthDesc = NULL;

    STL_DASSERT( aAuthDictHandle->mObjectType == ELL_OBJECT_AUTHORIZATION );
    
    sAuthDesc = (ellAuthDesc *) ellGetObjectDesc( aAuthDictHandle );

    /**
     * USER 만이 login 할 수 있다.
     */
    
    if ( sAuthDesc->mAuthType == ELL_AUTHORIZATION_TYPE_USER )
    {
        return STL_TRUE;
    }
    else
    {
        return STL_FALSE;
    }
}

/**
 * @brief Authorization 의 Schema Path Count
 * @param[in] aAuthDictHandle  Authorization Dictionary Handle
 * @return
 * Schema Count
 */
stlInt32 ellGetAuthSchemaPathCnt( ellDictHandle * aAuthDictHandle )
{
    ellAuthDesc * sAuthDesc = NULL;

    STL_DASSERT( aAuthDictHandle->mObjectType == ELL_OBJECT_AUTHORIZATION );
    
    sAuthDesc = (ellAuthDesc *) ellGetObjectDesc( aAuthDictHandle );

    return sAuthDesc->mSchemaPathCnt;
}

/**
 * @brief Authorization 의 Schema Path ID Array
 * @param[in] aAuthDictHandle  Authorization Dictionary Handle
 * @return
 * Schema ID Array Pointer
 */
stlInt64 * ellGetAuthSchemaPathArray( ellDictHandle * aAuthDictHandle )
{
    ellAuthDesc * sAuthDesc = NULL;

    STL_DASSERT( aAuthDictHandle->mObjectType == ELL_OBJECT_AUTHORIZATION );
    
    sAuthDesc = (ellAuthDesc *) ellGetObjectDesc( aAuthDictHandle );

    return sAuthDesc->mSchemaIDArray;
}

/**
 * @brief PUBLIC(전체 사용자)의 authorization ID를 얻는다.
 * @remarks
 * Gliese 에서 User 및 Schema 를 이름 기반으로 접근하지 않을 경우를 위해
 * special authorization id 인 PUBLIC 을 사용하도록 한다.
 */
stlInt64 ellGetAuthIdPUBLIC()
{
    return ELDT_AUTH_ID_PUBLIC;
}

/**
 * @brief SYSTEM 사용자의 authorization ID를 얻는다.
 * @remarks
 */
stlInt64 ellGetAuthIdSYSTEM()
{
    return ELDT_AUTH_ID_SYSTEM;
}


/**
 * @brief SYS 사용자의 authorization ID를 얻는다.
 * @remarks
 */
stlInt64 ellGetAuthIdSYS()
{
    return ELDT_AUTH_ID_SYS;
}

/**
 * @brief SYSDBA Role 의 authorization ID를 얻는다.
 * @remarks
 */
stlInt64 ellGetAuthIdSYSDBA()
{
    return ELDT_AUTH_ID_SYSDBA;
}

/**
 * @brief Authorization 객체가 Built-In Object 인지의 여부
 * @param[in]  aAuthDictHandle  Dictionary Handle
 * @remarks
 */
stlBool  ellIsBuiltInAuth( ellDictHandle * aAuthDictHandle )
{
    ellAuthDesc * sAuthDesc = NULL;

    STL_DASSERT( aAuthDictHandle->mObjectType == ELL_OBJECT_AUTHORIZATION );
    
    sAuthDesc = (ellAuthDesc *) ellGetObjectDesc( aAuthDictHandle );

    if ( sAuthDesc->mAuthID < ELDT_AUTH_BUILT_IN_ID_MAX )
    {
        return STL_TRUE;
    }
    else
    {
        return STL_FALSE;
    }
}


/**
 * @brief Authorization 객체가 Privilege 를 변경가능한 Object 인지의 여부
 * @param[in]  aAuthDictHandle  Dictionary Handle
 * @remarks
 */
stlBool  ellIsUpdatablePrivAuth( ellDictHandle * aAuthDictHandle )
{
    ellAuthDesc * sAuthDesc = NULL;

    STL_DASSERT( aAuthDictHandle->mObjectType == ELL_OBJECT_AUTHORIZATION );
    
    sAuthDesc = (ellAuthDesc *) ellGetObjectDesc( aAuthDictHandle );

    if ( sAuthDesc->mAuthID < ELDT_AUTH_NOT_UPDATABLE_PRIVILEGE_BUILT_IN_ID_MAX )
    {
        return STL_FALSE;
    }
    else
    {
        return STL_TRUE;
    }
}

/** @} ellObjectAuth */
