/*******************************************************************************
 * ellDictSchema.c
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
 * @file ellDictSchema.c
 * @brief Schema Dictionary 관리 모듈
 */

#include <ell.h>
#include <elDef.h>

#include <eldt.h>
#include <eldc.h>

#include <eldDictionary.h>
#include <eldSchema.h>

/**
 * @addtogroup ellObjectSchema
 * @{
 */


/*********************************************************
 * DDL 관련 Object 획득 함수
 *********************************************************/

/**
 * @brief DROP SCHEMA 에 의해 영향을 받는 Object Handle List 를 획득
 * @param[in]  aTransID               Transaction ID
 * @param[in]  aStmt                  Statement
 * @param[in]  aSchemaHandle          Schema Dictionary Handle
 * @param[in]  aRegionMem             Region Memory
 * @param[out] aInnerSeqList                   Schema 내부의 sequence list
 * @param[out] aInnerSynonymList               Schema 내부의 synonym list
 * @param[out] aInnerTableList                 Schema 내부의 table list
 * @param[out] aOuterTableInnerPrimaryKeyList  Schema 내부의 PK 이나 table 이 Schema 외부 
 * @param[out] aOuterTableInnerUniqueKeyList   Schema 내부의 UK 이나 table 이 Schema 외부 
 * @param[out] aOuterTableInnerIndexList       Schema 내부의 index 이나 table 이 Schema 외부 
 * @param[out] aOuterTableInnerNotNullList     Schema 내부의 not null 이나 table 이 Schema 외부 
 * @param[out] aOuterTableInnerCheckClauseList  Schema 내부의 check 이나 table 이 Schema 외부 
 * @param[out] aInnerForeignKeyList            Schema 내부의 Foreign Key list
 * @param[out] aOuterChildForeignKeyList       Schema 외부의 Child Foreign key이나 Schema 내부와 관련된 객체를 참조 
 * @param[in]  aEnv                   Environment
 * @remarks
 */
stlStatus ellGetObjectList4DropSchema( smlTransId       aTransID,
                                       smlStatement   * aStmt,
                                       ellDictHandle  * aSchemaHandle,
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
                                       ellEnv         * aEnv )
{
    stlInt64   sSchemaID = ELL_DICT_OBJECT_ID_NA;

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

    
    ellObjectList * sObjectList = NULL;
    
    ellObjectItem   * sObjectItem = NULL;
    ellDictHandle   * sObjectHandle = NULL;
    ellDictHandle   * sKeyObjHandle = NULL;
    ellDictHandle   * sIndexHandle = NULL;

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSchemaHandle != NULL, ELL_ERROR_STACK(aEnv) );
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

    /**
     * 기본 정보 획득
     */

    sSchemaID = ellGetSchemaID( aSchemaHandle );

    /*********************************************
     * sInnerSeqList
     *********************************************/
    
    /**
     * - sInnerSeqList : Schema 내부에 존재하는 Sequence 목록
     *
     * <BR> CREATE VIEW AS VIEW_INNER_SEQ_LIST AS
     * <BR>
     * <BR> SELECT s.SEQUENCE_ID
     * <BR>   FROM SEQUENCES s
     * <BR>  WHERE s.SCHEMA_ID = schema_id
     */

    STL_TRY( ellInitObjectList( & sInnerSeqList, aRegionMem, aEnv ) == STL_SUCCESS );
    
    STL_TRY( eldGetObjectList( aTransID,
                               aStmt,
                               ELDT_TABLE_ID_SEQUENCES,
                               ELL_OBJECT_SEQUENCE,
                               ELDT_Sequences_ColumnOrder_SEQUENCE_ID,
                               ELDT_Sequences_ColumnOrder_SCHEMA_ID,
                               sSchemaID,
                               aRegionMem,
                               sInnerSeqList,
                               aEnv )
             == STL_SUCCESS );

    /*********************************************
     * sInnerSynonymList
     *********************************************/
    
    /**
     * - sInnerSynonymList : Schema 내부에 존재하는 Synonym 목록
     *
     * <BR> CREATE VIEW AS VIEW_INNER_SYNONYM_LIST AS
     * <BR>
     * <BR> SELECT s.SYNONYM_ID
     * <BR>   FROM SYNONYMS s
     * <BR>  WHERE s.SCHEMA_ID = schema_id
     */

    STL_TRY( ellInitObjectList( & sInnerSynonymList, aRegionMem, aEnv ) == STL_SUCCESS );
    
    STL_TRY( eldGetObjectList( aTransID,
                               aStmt,
                               ELDT_TABLE_ID_SYNONYMS,
                               ELL_OBJECT_SYNONYM,
                               ELDT_Synonyms_ColumnOrder_SYNONYM_ID,
                               ELDT_Synonyms_ColumnOrder_SCHEMA_ID,
                               sSchemaID,
                               aRegionMem,
                               sInnerSynonymList,
                               aEnv )
             == STL_SUCCESS );
    
    /*********************************************
     * sInnerTableList
     *********************************************/
    
    /**
     * - sInnerTableList : Schema 내부에 존재하는 Table 목록
     *
     * <BR> CREATE VIEW AS VIEW_INNER_TABLE_LIST AS
     * <BR>
     * <BR> SELECT t.TABLE_ID
     * <BR>   FROM TABLES t
     * <BR>  WHERE t.SCHEMA_ID = schema_id
     */

    STL_TRY( ellInitObjectList( & sInnerTableList, aRegionMem, aEnv ) == STL_SUCCESS );
    
    STL_TRY( eldGetObjectList( aTransID,
                               aStmt,
                               ELDT_TABLE_ID_TABLES,
                               ELL_OBJECT_TABLE,
                               ELDT_Tables_ColumnOrder_TABLE_ID,
                               ELDT_Tables_ColumnOrder_SCHEMA_ID,
                               sSchemaID,
                               aRegionMem,
                               sInnerTableList,
                               aEnv )
             == STL_SUCCESS );

    /*********************************************
     * sInnerTableUniqueList
     *********************************************/

    /**
     * - sInnerTableUniqueList : Schema 내부에 존재하는 Table 들이 가진 Unique 목록
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
     * - sInnerIndexOrientedList : Schema 내부에 존재하는 index-oriented object list
     *
     * <BR> CREATE VIEW AS VIEW_INNER_INDEX_ORIENTED_LIST AS
     * <BR>
     * <BR> SELECT n.INDEX_ID
     * <BR>   FROM INDEXES n
     * <BR>  WHERE n.SCHEMA_ID = schema_id
     */

    STL_TRY( ellInitObjectList( & sInnerIndexOrientedList, aRegionMem, aEnv )
             == STL_SUCCESS );

    STL_TRY( eldGetObjectList( aTransID,
                               aStmt,
                               ELDT_TABLE_ID_INDEXES,
                               ELL_OBJECT_INDEX,
                               ELDT_Indexes_ColumnOrder_INDEX_ID,
                               ELDT_Indexes_ColumnOrder_SCHEMA_ID,
                               sSchemaID,
                               aRegionMem,
                               sInnerIndexOrientedList,
                               aEnv )
             == STL_SUCCESS );
    
    /*********************************************
     * sOuterTableInnerIndexList
     *********************************************/

    /**
     * - sOuterTableInnerIndexOrientedList
     *   : Schema 내부에 존재하는 index-oriented object 이나 Outer Table 에 속한 Object
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
     * - sInnerConstList : Schema 내부에 존재하는 Constraint 목록
     *
     * <BR> CREATE VIEW AS VIEW_INNER_CONST_LIST AS
     * <BR>
     * <BR> SELECT t.CONSTRAINT_ID
     * <BR>   FROM TABLE_CONSTRAINTS tc
     * <BR>  WHERE tc.CONSTRAINT_SCHEMA_ID = schema_id
     */

    STL_TRY( ellInitObjectList( & sInnerConstList, aRegionMem, aEnv ) == STL_SUCCESS );
    
    STL_TRY( eldGetObjectList( aTransID,
                               aStmt,
                               ELDT_TABLE_ID_TABLE_CONSTRAINTS,
                               ELL_OBJECT_CONSTRAINT,
                               ELDT_TableConst_ColumnOrder_CONSTRAINT_ID,
                               ELDT_TableConst_ColumnOrder_CONSTRAINT_SCHEMA_ID,
                               sSchemaID,
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

        if ( ellGetConstraintTableSchemaID( sObjectHandle ) == sSchemaID )
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
     * - sChildForeignKeyList : Schema 와 관련된 모든 Child Foreign Key List
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
     * - sOuterChildForeignKeyList : Schema 외부에 존재하는 Foreign Key 이나 내부 객체와 관련 있는 list
     *
     * <BR> SELECT vrkl.CONSTRAINT_ID
     * <BR>   FROM VIEW_REFERENCE_KEY_LIST vrkl
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
             * Schema 내부의 Foreign Key 임
             * - nothing to do 
             */
        }
        else
        {
            /**
             * Schema 외부의 Foreign Key 이면서, Schema 내부 객체를 참조하는 Child Foreign Key 임 
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
 * @brief CREATE SCHEMA 구문을 위한 DDL Lock 을 획득한다.
 * @param[in]  aTransID          Transaction ID
 * @param[in]  aStmt             Statement
 * @param[in]  aOwnerHandle      Schema 의 Owner Dictionary Handle
 * @param[out] aLockSuccess      Lock 획득 여부 
 * @param[in]  aEnv              Environment
 * @remarks
 */
stlStatus ellLock4CreateSchema( smlTransId      aTransID,
                                smlStatement  * aStmt,
                                ellDictHandle * aOwnerHandle,
                                stlBool       * aLockSuccess,
                                ellEnv        * aEnv )
{
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOwnerHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aLockSuccess != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * Owner 가 변경되지 않아야 함.
     */
        
    STL_TRY( eldLockRowByObjectHandle( aTransID,
                                       aStmt,
                                       ELDT_TABLE_ID_AUTHORIZATIONS,
                                       ELDT_Auth_ColumnOrder_AUTH_ID,
                                       aOwnerHandle,
                                       SML_LOCK_S,
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
 * @brief DROP SCHEMA 구문을 위한 DDL Lock 을 획득한다.
 * @param[in]  aTransID          Transaction ID
 * @param[in]  aStmt             Statement
 * @param[in]  aSchemaHandle     Schema Handle
 * @param[out] aLockSuccess      Lock 획득 여부 
 * @param[in]  aEnv              Environment
 * @remarks
 */
stlStatus ellLock4DropSchema( smlTransId      aTransID,
                              smlStatement  * aStmt,
                              ellDictHandle * aSchemaHandle,
                              stlBool       * aLockSuccess,
                              ellEnv        * aEnv )
{
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSchemaHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aLockSuccess != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * Schema 가 변경되지 않아야 함.
     */
    
    STL_TRY( eldLockRowByObjectHandle( aTransID,
                                       aStmt,
                                       ELDT_TABLE_ID_SCHEMATA,
                                       ELDT_Schemata_ColumnOrder_SCHEMA_ID,
                                       aSchemaHandle,
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


/*********************************************************
 * Dictionary 변경 함수 
 *********************************************************/

/**
 * @brief Schema Descriptor 를 Dictionary 에 추가한다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aOwnerID         Schema 의 Owner ID
 * @param[out] aSchemaID        Schema 의 ID 
 * @param[in]  aSchemaName      Schema Name
 * @param[in]  aSchemaComment   Schema Comment (nullable)
 * @param[in]  aEnv             Execution Library Environment
 * @remarks
 */

stlStatus ellInsertSchemaDesc( smlTransId     aTransID,
                               smlStatement * aStmt,
                               stlInt64       aOwnerID,
                               stlInt64     * aSchemaID,
                               stlChar      * aSchemaName,
                               stlChar      * aSchemaComment,
                               ellEnv       * aEnv )
{
    /**
     * Schema Dictionary Record 추가
     */
    
    STL_TRY( eldInsertSchemaDesc( aTransID,
                                  aStmt,
                                  aOwnerID,
                                  aSchemaID,
                                  aSchemaName,
                                  aSchemaComment,
                                  aEnv )
             == STL_SUCCESS );

    /**
     * Schema Owner 의 Privilege 는 부여하지 않음
     */
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/**
 * @brief DROP SCHEMA 를 Dictionary Row 를 제거 
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aSchemaHandle    Schema Dictionary Handle
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus ellRemoveDict4DropSchema( smlTransId        aTransID,
                                    smlStatement    * aStmt,
                                    ellDictHandle   * aSchemaHandle,
                                    ellEnv          * aEnv )
{
    stlInt64 sSchemaID = ELL_DICT_OBJECT_ID_NA;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSchemaHandle != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */

    sSchemaID = ellGetSchemaID( aSchemaHandle );
    
    /**
     * Schema 의 Dictionary 정보 제거
     */

    /*
     * DELETE FROM DEFINITION_SCHEMA.SCHEMA_PRIVILEGES
     *  WHERE SCHEMA_ID = sSchemaID;
     */
    
    STL_TRY( eldDeletePrivRowsAndCache( aTransID,
                                        aStmt,
                                        ELDT_TABLE_ID_SCHEMA_PRIVILEGES,
                                        ELDT_SchemaPriv_ColumnOrder_SCHEMA_ID,
                                        sSchemaID,
                                        aEnv )
             == STL_SUCCESS );
    
    /*
     * DELETE FROM DEFINITION_SCHEMA.SCHEMATA
     *  WHERE SCHEMA_ID = sSchemaID;
     */
    
    STL_TRY( eldDeleteObjectRows( aTransID,
                                  aStmt,
                                  ELDT_TABLE_ID_SCHEMATA,
                                  ELDT_Schemata_ColumnOrder_SCHEMA_ID,
                                  sSchemaID,
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
 * @brief SQL-Schema Cache 를 추가한다.
 * @param[in]  aTransID     Transaction ID
 * @param[in]  aStmt        Statement
 * @param[in]  aAuthID      Authroization ID
 * @param[in]  aSchemaID    Schema ID
 * @param[in]  aEnv         Envirionment 
 * @remarks
 * 필요한 정보들이 Dictionary Table 에 저장되어 있어야 함.
 */
stlStatus ellRefineCache4AddSchema( smlTransId           aTransID,
                                    smlStatement       * aStmt,
                                    stlInt64             aAuthID,
                                    stlInt64             aSchemaID,
                                    ellEnv             * aEnv )
{
    STL_TRY( eldRefineCache4AddSchema( aTransID,
                                       aStmt,
                                       aAuthID,
                                       aSchemaID,
                                       aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/**
 * @brief DROP SCHEMA 를 위한 Cache 재구성  
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aSchemaHandle    Schema Dictionary Handle
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus ellRefineCache4DropSchema( smlTransId        aTransID,
                                     ellDictHandle   * aSchemaHandle,
                                     ellEnv          * aEnv )
{
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aSchemaHandle != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * SQL-Schema Cache 정보를 삭제
     */
    
    STL_TRY( eldcDeleteCacheSchema( aTransID,
                                    aSchemaHandle,
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
 * @brief Schema Dictionary Handle 을 획득 
 * @param[in]  aTransID           Transaction ID
 * @param[in]  aViewSCN           View SCN
 * @param[in]  aSchemaName        Schema Name
 * @param[out] aSchemaDictHandle  Schema Dictionary Handle
 * @param[out] aExist             존재 여부 
 * @param[in]  aEnv               Envirionment 
 * @remarks
 */
stlStatus ellGetSchemaDictHandle( smlTransId           aTransID,
                                  smlScn               aViewSCN,
                                  stlChar            * aSchemaName,
                                  ellDictHandle      * aSchemaDictHandle,
                                  stlBool            * aExist,
                                  ellEnv             * aEnv )
{
    ellInitDictHandle( aSchemaDictHandle );
    
    return eldGetSchemaHandleByName( aTransID,
                                     aViewSCN,
                                     aSchemaName,
                                     aSchemaDictHandle,
                                     aExist,
                                     aEnv );
}


/**
 * @brief Schema ID 로부터 Schema Dictionary Handle 을 획득 
 * @param[in]  aTransID           Transaction ID
 * @param[in]  aViewSCN           View SCN
 * @param[in]  aSchemaID          Schema ID
 * @param[out] aSchemaDictHandle  Schema Dictionary Handle
 * @param[out] aExist             존재 여부 
 * @param[in]  aEnv               Envirionment 
 * @remarks
 */
stlStatus ellGetSchemaDictHandleByID( smlTransId           aTransID,
                                      smlScn               aViewSCN,
                                      stlInt64             aSchemaID,
                                      ellDictHandle      * aSchemaDictHandle,
                                      stlBool            * aExist,
                                      ellEnv             * aEnv )
{
    ellInitDictHandle( aSchemaDictHandle );
    
    return eldGetSchemaHandleByID( aTransID,
                                   aViewSCN,
                                   aSchemaID,
                                   aSchemaDictHandle,
                                   aExist,
                                   aEnv );
}




/**********************************************************
 * 정보 획득 함수
 **********************************************************/



/**
 * @brief Schema ID 정보 
 * @param[in] aSchemaDictHandle Schema Dictionary Handle
 * @return Schema ID
 * @remarks
 */
stlInt64 ellGetSchemaID( ellDictHandle * aSchemaDictHandle )
{
    ellSchemaDesc * sSchemaDesc = NULL;

    STL_DASSERT( aSchemaDictHandle->mObjectType == ELL_OBJECT_SCHEMA );
    
    sSchemaDesc = (ellSchemaDesc *) ellGetObjectDesc( aSchemaDictHandle );

    return sSchemaDesc->mSchemaID;
}


/**
 * @brief Schema Owner ID 정보 
 * @param[in] aSchemaDictHandle Schema Dictionary Handle
 * @return Schema Owner ID
 * @remarks
 */
stlInt64 ellGetSchemaOwnerID( ellDictHandle * aSchemaDictHandle )
{
    ellSchemaDesc * sSchemaDesc;

    STL_DASSERT( aSchemaDictHandle->mObjectType == ELL_OBJECT_SCHEMA );
    
    sSchemaDesc = (ellSchemaDesc *) ellGetObjectDesc( aSchemaDictHandle );

    return sSchemaDesc->mOwnerID;
}


/**
 * @brief PUBLIC schema 의 ID
 * @return Schema ID
 * @remarks
 */
stlInt64 ellGetSchemaIdPUBLIC()
{
    return ELDT_SCHEMA_ID_PUBLIC;
}

/**
 * @brief INFORMATION_SCHEMA schema 의 ID
 * @return Schema ID
 * @remarks
 */
stlInt64 ellGetSchemaIdINFORMATION_SCHEMA()
{
    return ELDT_SCHEMA_ID_INFORMATION_SCHEMA;
}

/**
 * @brief DICTIONARY_SCHEMA schema 의 ID
 * @return Schema ID
 * @remarks
 */
stlInt64 ellGetSchemaIdDICTIONARY_SCHEMA()
{
    return ELDT_SCHEMA_ID_DICTIONARY_SCHEMA;
}

/**
 * @brief Schema Name 정보
 * @param[in] aSchemaDictHandle Schema Dictionary Handle
 * @return Schema Name
 * @remarks
 */
stlChar * ellGetSchemaName( ellDictHandle * aSchemaDictHandle )
{
    ellSchemaDesc * sSchemaDesc = NULL;

    STL_DASSERT( aSchemaDictHandle->mObjectType == ELL_OBJECT_SCHEMA );
    
    sSchemaDesc = (ellSchemaDesc *) ellGetObjectDesc( aSchemaDictHandle );

    return sSchemaDesc->mSchemaName;
}


/**
 * @brief 쓰기 가능한 Schema 인지 검사 
 * @param[in] aSchemaDictHandle Schema Dictionary Handle
 * @return
 * - STL_TRUE  : 쓰기 가능
 * - STL_FALSE : 읽기 전용 
 * @remarks
 * DEFINITION_SCHEMA, FIXED_TABLE_SCHEMA 등의 built-in Schema 는 사용자가 Write 할 수 없다.
 */
stlBool ellGetSchemaWritable( ellDictHandle * aSchemaDictHandle )
{
    stlBool         sWritable = STL_FALSE;
    ellSchemaDesc * sSchemaDesc = NULL;

    STL_DASSERT( aSchemaDictHandle->mObjectType == ELL_OBJECT_SCHEMA );
    
    sSchemaDesc = (ellSchemaDesc *) ellGetObjectDesc( aSchemaDictHandle );

    if ( sSchemaDesc->mOwnerID == ELDT_AUTH_ID_SYSTEM )
    {
        /**
         * _SYSTEM 사용자가 만든 Schema
         */

        if ( (sSchemaDesc->mSchemaID == ELDT_SCHEMA_ID_PUBLIC) ||
             (sSchemaDesc->mSchemaID == ELDT_SCHEMA_ID_DICTIONARY_SCHEMA) ||
             (sSchemaDesc->mSchemaID == ELDT_SCHEMA_ID_INFORMATION_SCHEMA) ||
             (sSchemaDesc->mSchemaID == ELDT_SCHEMA_ID_PERFORMANCE_VIEW_SCHEMA) )
        {
            /**
             * 다음 Schema 는 Built-In Schema 이나 쓰기 가능하다.
             * - PUBLIC schema
             * - DICTIONARY_SCHEMA schema
             * - INFORMATION_SCHEMA schema
             * - PERFORMANCE_VIEW_SCHEMA schema
             */
            sWritable = STL_TRUE;
        }
        else
        {
            
            sWritable = STL_FALSE;
        }
    }
    else
    {
        /**
         * 일반 User 가 만든 Schema
         */
        sWritable = STL_TRUE;
    }
    
    return sWritable;
}

/**
 * @brief Schema 객체가 Built-In Object 인지의 여부
 * @param[in]  aSchemaDictHandle  Dictionary Handle
 * @remarks
 */
stlBool ellIsBuiltInSchema( ellDictHandle * aSchemaDictHandle )
{
    ellSchemaDesc * sSchemaDesc = NULL;

    STL_DASSERT( aSchemaDictHandle->mObjectType == ELL_OBJECT_SCHEMA );
    
    sSchemaDesc = (ellSchemaDesc *) ellGetObjectDesc( aSchemaDictHandle );

    if ( sSchemaDesc->mOwnerID == ELDT_AUTH_ID_SYSTEM )
    {
        return STL_TRUE;
    }
    else
    {
        return STL_FALSE;
    }
}

/** @} ellObjectSchema */
