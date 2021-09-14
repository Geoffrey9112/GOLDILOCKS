/*******************************************************************************
 * ellDictTablespace.c
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
 * @file ellDictTablespace.c
 * @brief Tablespace Dictionary 관리 모듈
 */

#include <ell.h>
#include <elDef.h>

#include <eldt.h>
#include <eldc.h>

#include <eldDictionary.h>
#include <eldTablespace.h>


/**
 * @addtogroup ellObjectTablespace
 * @{
 */

/**
 * @brief Tablespace Media Type 에 대한 String 상수
 */
const stlChar * const gEllTablespaceMediaTypeString[ELL_SPACE_MEDIA_TYPE_MAX] =
{
    "N/A",         /**< ELL_SPACE_MEDIA_TYPE_NOT_AVAILABLE */
    
    "DISK",        /**< ELL_SPACE_MEDIA_TYPE_DISK */
    "MEMORY",      /**< ELL_SPACE_MEDIA_TYPE_MEMORY */
    "SSD",         /**< ELL_SPACE_MEDIA_TYPE_SSD */
};


/**
 * @brief Tablespace Usage Type 에 대한 String 상수
 */
const stlChar * const gEllTablespaceUsageTypeString[ELL_SPACE_USAGE_TYPE_MAX] =
{
    "N/A",         /**< ELL_SPACE_MEDIA_TYPE_NOT_AVAILABLE */
    
    "DICTIONARY",  /**< ELL_SPACE_USAGE_TYPE_SYSTEM */
    "UNDO",        /**< ELL_SPACE_USAGE_TYPE_UNDO */
    "DATA",        /**< ELL_SPACE_USAGE_TYPE_DATA */
    "TEMPORARY",   /**< ELL_SPACE_USAGE_TYPE_TEMPORARY */
};



/*********************************************************
 * DDL 관련 Object 획득 함수
 *********************************************************/

/**
 * @brief DROP TABLESPACE 에 의해 영향을 받는 Object Handle List 를 획득
 * @param[in]  aTransID               Transaction ID
 * @param[in]  aStmt                  Statement
 * @param[in]  aSpaceHandle           Tablespace Dictionary Handle
 * @param[in]  aRegionMem             Region Memory
 * @param[out] aInnerTableList                 Space 내부의 table list
 * @param[out] aOuterTableInnerPrimaryKeyList  Space 내부의 PK 이나 table 이 Space 외부 
 * @param[out] aOuterTableInnerUniqueKeyList   Space 내부의 UK 이나 table 이 Space 외부 
 * @param[out] aOuterTableInnerIndexList       Space 내부의 index 이나 table 이 Space 외부 
 * @param[out] aInnerForeignKeyList            Space 내부의 Foreign Key list
 * @param[out] aOuterChildForeignKeyList       Space 외부의 Child Foreign key이나 내부 관련 객체를 참조 
 * @param[in]  aEnv                   Environment
 * @remarks
 */
stlStatus ellGetObjectList4DropSpace( smlTransId       aTransID,
                                      smlStatement   * aStmt,
                                      ellDictHandle  * aSpaceHandle,
                                      knlRegionMem   * aRegionMem,
                                      ellObjectList ** aInnerTableList,
                                      ellObjectList ** aOuterTableInnerPrimaryKeyList,
                                      ellObjectList ** aOuterTableInnerUniqueKeyList,
                                      ellObjectList ** aOuterTableInnerIndexList,
                                      ellObjectList ** aInnerForeignKeyList,
                                      ellObjectList ** aOuterChildForeignKeyList,
                                      ellEnv         * aEnv )
{
    stlInt64   sSpaceID = ELL_DICT_OBJECT_ID_NA;

    ellObjectList * sInnerTableList = NULL;
    ellObjectList * sInnerTableUniqueList = NULL;
    
    ellObjectList * sInnerIndexOrientedList = NULL;
    ellObjectList * sOuterTableInnerIndexOrientedList = NULL;
    
    ellObjectList * sOuterTableInnerUniqueList = NULL;
    ellObjectList * sOuterTableInnerPKList = NULL;
    ellObjectList * sOuterTableInnerUKList = NULL;
    ellObjectList * sOuterTableInnerIndexList = NULL;

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
    STL_PARAM_VALIDATE( aSpaceHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInnerTableList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOuterTableInnerPrimaryKeyList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOuterTableInnerUniqueKeyList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOuterTableInnerIndexList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInnerForeignKeyList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOuterChildForeignKeyList != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */

    sSpaceID = ellGetTablespaceID( aSpaceHandle );
    
    /*********************************************
     * sInnerTableList
     *********************************************/
    
    /**
     * - sInnerTableList : Space 내부에 존재하는 Table 목록
     *
     * <BR> CREATE VIEW AS VIEW_INNER_TABLE_LIST AS
     * <BR>
     * <BR> SELECT t.TABLE_ID
     * <BR>   FROM TABLES t
     * <BR>  WHERE t.TABLESPACE_ID = space_id
     */

    STL_TRY( ellInitObjectList( & sInnerTableList, aRegionMem, aEnv ) == STL_SUCCESS );
    
    STL_TRY( eldGetObjectList( aTransID,
                               aStmt,
                               ELDT_TABLE_ID_TABLES,
                               ELL_OBJECT_TABLE,
                               ELDT_Tables_ColumnOrder_TABLE_ID,
                               ELDT_Tables_ColumnOrder_TABLESPACE_ID,
                               sSpaceID,
                               aRegionMem,
                               sInnerTableList,
                               aEnv )
             == STL_SUCCESS );

    /*********************************************
     * sInnerTableUniqueList
     *********************************************/

    /**
     * - sInnerTableUniqueList : Space 내부에 존재하는 Table 들이 가진 Unique 목록
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
     * - sInnerIndexOrientedList : Space 내부에 존재하는 index-oriented object list
     *
     * <BR> CREATE VIEW AS VIEW_INNER_INDEX_ORIENTED_LIST AS
     * <BR>
     * <BR> SELECT n.INDEX_ID
     * <BR>   FROM INDEXES n
     * <BR>  WHERE n.TABLESPACE_ID = space_id
     */

    STL_TRY( ellInitObjectList( & sInnerIndexOrientedList, aRegionMem, aEnv )
             == STL_SUCCESS );

    STL_TRY( eldGetObjectList( aTransID,
                               aStmt,
                               ELDT_TABLE_ID_INDEXES,
                               ELL_OBJECT_INDEX,
                               ELDT_Indexes_ColumnOrder_INDEX_ID,
                               ELDT_Indexes_ColumnOrder_TABLESPACE_ID,
                               sSpaceID,
                               aRegionMem,
                               sInnerIndexOrientedList,
                               aEnv )
             == STL_SUCCESS );
    
    /*********************************************
     * sOuterTableInnerIndexOrientedList
     *********************************************/

    /**
     * - sOuterTableInnerIndexOrientedList
     *   : Space 내부에 존재하는 index-oriented object 이나 Outer Table 에 속한 Object
     * 
     * <BR> CREATE VIEW AS VIEW_OUTER_TABLE_INNER_INDEX_ORIENTED_LIST AS
     * <BR>
     * <BR> SELECT n.INDEX_ID
     * <BR>   FROM VIEW_INNER_INDEX_ORIENTED_LIST viiol, INDEX_KEY_TABLE_USAGE k
     * <BR>  WHERE viiol.INDEX_ID = k.INDEX_ID
     * <BR>    AND k.TABLE_ID
     * <BR>        NOT IN ( SELECT TABLE_ID FROM VIEW_INNER_TABLE_LIST )
     */

    STL_TRY( ellInitObjectList( & sOuterTableInnerIndexOrientedList, aRegionMem, aEnv )
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

            STL_TRY( ellAddNewObjectItem( sOuterTableInnerIndexOrientedList,
                                          sObjectHandle,
                                          NULL,
                                          aRegionMem,
                                          aEnv )
                     == STL_SUCCESS );
        }
    }
    
    /*********************************************
     * sOuterTableInnerIndexOrientedList 로부터 다음을 분류 
     * - sOuterTableInnerUniqueList
     * - sOuterTableInnerPKList
     * - sOuterTableInnerUKList
     * - sOuterTableInnerIndexList
     *********************************************/

    /**
     * - sOuterTableInnerIndexOrientedList 를 순회하며 분류 
     */

    STL_TRY( ellInitObjectList( & sOuterTableInnerUniqueList, aRegionMem, aEnv )
             == STL_SUCCESS );
    STL_TRY( ellInitObjectList( & sOuterTableInnerPKList, aRegionMem, aEnv )
             == STL_SUCCESS );
    STL_TRY( ellInitObjectList( & sOuterTableInnerUKList, aRegionMem, aEnv )
             == STL_SUCCESS );
    STL_TRY( ellInitObjectList( & sOuterTableInnerIndexList, aRegionMem, aEnv )
             == STL_SUCCESS );

    STL_RING_FOREACH_ENTRY( & sOuterTableInnerIndexOrientedList->mHeadList, sObjectItem )
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
                    {
                        STL_TRY( ellAddNewObjectItem( sOuterTableInnerUniqueList,
                                                      sKeyObjHandle,
                                                      NULL,
                                                      aRegionMem,
                                                      aEnv )
                                 == STL_SUCCESS );
                        
                        STL_TRY( ellAddNewObjectItem( sOuterTableInnerPKList,
                                                      sKeyObjHandle,
                                                      NULL,
                                                      aRegionMem,
                                                      aEnv )
                                 == STL_SUCCESS );
                        break;
                    }
                case ELL_TABLE_CONSTRAINT_TYPE_UNIQUE_KEY:
                    {
                        STL_TRY( ellAddNewObjectItem( sOuterTableInnerUniqueList,
                                                      sKeyObjHandle,
                                                      NULL,
                                                      aRegionMem,
                                                      aEnv )
                                 == STL_SUCCESS );
                        
                        STL_TRY( ellAddNewObjectItem( sOuterTableInnerUKList,
                                                      sKeyObjHandle,
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
             * (Unique or Non-Unique) Index 임
             */
            
            STL_TRY( ellAddNewObjectItem( sOuterTableInnerIndexList,
                                          sObjectHandle,
                                          NULL,
                                          aRegionMem,
                                          aEnv )
                     == STL_SUCCESS );
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
     * - sChildForeignKeyList : Space 와 관련된 모든 Child Foreign Key List
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
     * - sOuterChildForeignKeyList : Space 외부에 존재하는 Child Foreign Key 이나 내부 객체와 관련 있는 list
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
             * Space 내부의 Child Foreign Key 임
             * - nothing to do 
             */
        }
        else
        {
            /**
             * Space 외부의 Child Foreign Key 이면서, Space 내부 객체를 참조하는 Child Foreign Key 임 
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

    *aInnerTableList                = sInnerTableList;
    *aOuterTableInnerPrimaryKeyList = sOuterTableInnerPKList;
    *aOuterTableInnerUniqueKeyList  = sOuterTableInnerUKList;
    *aOuterTableInnerIndexList      = sOuterTableInnerIndexList;
    *aInnerForeignKeyList           = sInnerForeignKeyList;
    *aOuterChildForeignKeyList      = sOuterChildForeignKeyList;
    
    
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
 * @brief Tablespace 에 포함된 index-oriented object 와 해당 table list 를 획득한다.
 * @param[in]  aTransID               Transaction ID
 * @param[in]  aStmt                  Statement
 * @param[in]  aSpaceHandle           Tablespace Dictionary Handle
 * @param[in]  aRegionMem             Region Memory
 * @param[out] aInnerTableIndexObjList  Space 내부의 Table 에 종속된 index-oriented object list
 * @param[in]  aEnv                   Environment
 * @remarks
 */
stlStatus ellGetIndexObjectList4SpaceOnline( smlTransId       aTransID,
                                             smlStatement   * aStmt,
                                             ellDictHandle  * aSpaceHandle,
                                             knlRegionMem   * aRegionMem,
                                             ellObjectList ** aInnerTableIndexObjList,
                                             ellEnv         * aEnv )
{
    stlInt64   sSpaceID = ELL_DICT_OBJECT_ID_NA;

    ellObjectList * sObjectList = NULL;
    
    ellObjectList * sInnerTableList = NULL;
    ellObjectList * sTableIndexList = NULL;

    ellObjectItem * sObjectItem = NULL;
    ellDictHandle * sObjectHandle = NULL;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSpaceHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInnerTableIndexObjList != NULL, ELL_ERROR_STACK(aEnv) );


    /**
     * 기본 정보 획득
     */

    sSpaceID = ellGetTablespaceID( aSpaceHandle );


    /**
     * - sInnerTableList : Space 내부에 존재하는 Table 목록
     *
     * <BR> CREATE VIEW AS VIEW_INNER_TABLE_LIST AS
     * <BR>
     * <BR> SELECT t.TABLE_ID
     * <BR>   FROM TABLES t
     * <BR>  WHERE t.TABLESPACE_ID = space_id
     */

    STL_TRY( ellInitObjectList( & sInnerTableList, aRegionMem, aEnv ) == STL_SUCCESS );
    
    STL_TRY( eldGetObjectList( aTransID,
                               aStmt,
                               ELDT_TABLE_ID_TABLES,
                               ELL_OBJECT_TABLE,
                               ELDT_Tables_ColumnOrder_TABLE_ID,
                               ELDT_Tables_ColumnOrder_TABLESPACE_ID,
                               sSpaceID,
                               aRegionMem,
                               sInnerTableList,
                               aEnv )
             == STL_SUCCESS );

    /**
     * Inner Table List 로부터 Index List 획득
     */

    STL_TRY( ellInitObjectList( & sObjectList, aRegionMem, aEnv ) == STL_SUCCESS );
    
    STL_RING_FOREACH_ENTRY( & sInnerTableList->mHeadList, sObjectItem )
    {
        sObjectHandle = & sObjectItem->mObjectHandle;

        /**
         * Table 과 관련된 Index 정보 획득
         * - SELECT INDEX_ID
         * - FROM DEFINITION_SCHEMA.INDEX_KEY_TABLE_USAGE
         * - WHERE TABLE_ID = sTableID
         */
        
        STL_TRY( ellInitObjectList( & sTableIndexList, aRegionMem, aEnv ) == STL_SUCCESS );
        
        STL_TRY( eldGetObjectList( aTransID,
                                   aStmt,
                                   ELDT_TABLE_ID_INDEX_KEY_TABLE_USAGE,
                                   ELL_OBJECT_INDEX,
                                   ELDT_IndexTable_ColumnOrder_INDEX_ID,
                                   ELDT_IndexTable_ColumnOrder_TABLE_ID,
                                   ellGetTableID( sObjectHandle ),
                                   aRegionMem,
                                   sTableIndexList,
                                   aEnv )
                 == STL_SUCCESS );

        /**
         * Index List 를 Merge
         */

        ellMergeObjectList( sObjectList, sTableIndexList );
    }

    /**
     * Output 설정
     */

    *aInnerTableIndexObjList = sObjectList;
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}




/*********************************************************
 * DDL Lock 함수 
 *********************************************************/




/**
 * @brief CREATE TABLESPACE 구문을 위한 DDL Lock 을 획득한다.
 * @param[in]  aTransID          Transaction ID
 * @param[in]  aStmt             Statement
 * @param[in]  aCreatorHandle    Tablespace 의 Creator Handle
 * @param[out] aLockSuccess      Lock 획득 여부 
 * @param[in]  aEnv              Environment
 * @remarks
 */
stlStatus ellLock4CreateTablespace( smlTransId      aTransID,
                                    smlStatement  * aStmt,
                                    ellDictHandle * aCreatorHandle,
                                    stlBool       * aLockSuccess,
                                    ellEnv        * aEnv )
{
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCreatorHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aLockSuccess != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * Creator 가 변경되지 않아야 함.
     */
    
    STL_TRY( eldLockRowByObjectHandle( aTransID,
                                       aStmt,
                                       ELDT_TABLE_ID_AUTHORIZATIONS,
                                       ELDT_Auth_ColumnOrder_AUTH_ID,
                                       aCreatorHandle,
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
 * @brief DROP TABLESPACE 구문을 위한 DDL Lock 을 획득한다.
 * @param[in]  aTransID          Transaction ID
 * @param[in]  aStmt             Statement
 * @param[in]  aSpaceHandle      Tablespace Dictionary Handle
 * @param[out] aLockSuccess      Lock 획득 여부 
 * @param[in]  aEnv              Environment
 * @remarks
 */
stlStatus ellLock4DropTablespace( smlTransId      aTransID,
                                  smlStatement  * aStmt,
                                  ellDictHandle * aSpaceHandle,
                                  stlBool       * aLockSuccess,
                                  ellEnv        * aEnv )
{
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSpaceHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aLockSuccess != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * Tablespace 가 변경되지 않아야 함.
     */
    
    STL_TRY( eldLockRowByObjectHandle( aTransID,
                                       aStmt,
                                       ELDT_TABLE_ID_TABLESPACES,
                                       ELDT_Space_ColumnOrder_TABLESPACE_ID,
                                       aSpaceHandle,
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
 * @brief ALTER TABLESPACE 구문을 위한 DDL Lock 을 획득한다.
 * @param[in]  aTransID          Transaction ID
 * @param[in]  aStmt             Statement
 * @param[in]  aSpaceHandle      Tablespace Dictionary Handle
 * @param[out] aLockSuccess      Lock 획득 여부 
 * @param[in]  aEnv              Environment
 * @remarks
 */
stlStatus ellLock4AlterTablespace( smlTransId      aTransID,
                                   smlStatement  * aStmt,
                                   ellDictHandle * aSpaceHandle,
                                   stlBool       * aLockSuccess,
                                   ellEnv        * aEnv )
{
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSpaceHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aLockSuccess != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * Tablespace 가 변경되지 않아야 함.
     */
    
    STL_TRY( eldLockRowByObjectHandle( aTransID,
                                       aStmt,
                                       ELDT_TABLE_ID_TABLESPACES,
                                       ELDT_Space_ColumnOrder_TABLESPACE_ID,
                                       aSpaceHandle,
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
 * @brief ALTER TABLESPACE 구문의 수행시간을 설정한다.
 * @param[in]  aTransID          Transaction ID
 * @param[in]  aStmt             Statement
 * @param[in]  aTablespaceID     Tablespace ID
 * @param[in]  aEnv              Environment
 * @remarks
 */
stlStatus ellSetTimeAlterTablespace( smlTransId      aTransID,
                                     smlStatement  * aStmt,
                                     stlInt64        aTablespaceID,
                                     ellEnv        * aEnv )
{
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTablespaceID > ELL_DICT_TABLESPACE_ID_NA,
                        ELL_ERROR_STACK(aEnv) );
    
    /**
     * Tablespace Dictionary Record 가 이전에 변경되지 않아야 함.
     */
    
    STL_TRY( eldModifyDictDesc( aTransID,
                                aStmt,
                                ELDT_TABLE_ID_TABLESPACES,
                                ELDT_Space_ColumnOrder_TABLESPACE_ID,
                                aTablespaceID,
                                ELDT_Space_ColumnOrder_MODIFIED_TIME,
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
 * @brief Tablespace Descriptor 를 Dictionary 에 추가한다.
 * @param[in]  aTransID             Transaction ID
 * @param[in]  aStmt                Statement
 * @param[in]  aTablespaceID        Tablespace 의 ID (SM에서 tablespace 생성후 획득한 ID)
 * @param[in]  aCreatorID           Tablespace를 생성한 사용자의 Authorization ID
 * @param[in]  aTablespaceName      Tablespace Name
 * @param[in]  aMediaType           Tablespace 의 Media Type
 * @param[in]  aUsageType           Tablespace 의 Usage Type
 * @param[in]  aTablespaceComment   Tablespace Comment (nullable)
 * @param[in]  aEnv                 Execution Library Environment
 * @remarks
 */
stlStatus ellInsertTablespaceDesc( smlTransId               aTransID,
                                   smlStatement           * aStmt,
                                   stlInt64                 aTablespaceID,
                                   stlInt64                 aCreatorID,
                                   stlChar                * aTablespaceName,
                                   ellTablespaceMediaType   aMediaType,
                                   ellTablespaceUsageType   aUsageType,
                                   stlChar                * aTablespaceComment,
                                   ellEnv                 * aEnv )
{
    /**
     * Tablespace Dictionary Record 추가
     */
    
    STL_TRY( eldInsertTablespaceDesc( aTransID,
                                      aStmt,
                                      aTablespaceID,
                                      aCreatorID,
                                      aTablespaceName,
                                      aMediaType,
                                      aUsageType,
                                      aTablespaceComment,
                                      aEnv )
             == STL_SUCCESS );

    /**
     * Tablespace Creator 에 대해 CREATE OBJECT ON TABLESPACE Privilege Cache 추가
     */

    STL_TRY( ellInsertTablespacePrivDesc( aTransID,
                                          aStmt,
                                          ELDT_AUTH_ID_SYSTEM,  /* Grantor */
                                          aCreatorID,           /* Grantee */
                                          aTablespaceID,
                                          ELL_SPACE_PRIV_ACTION_CREATE_OBJECT,
                                          STL_FALSE, /* aGrantable */
                                          aEnv )
             == STL_SUCCESS );
                                          
                                          
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/**
 * @brief RENAME TABLESPACE 를 위한 Dictionary Record 를 변경한다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aSpaceID         Tablespace 의 ID 
 * @param[in]  aNewName         Tablespace 의 New Name
 * @param[in]  aEnv             Execution Library Environment
 * @remarks
 */
stlStatus ellModifySpaceName( smlTransId     aTransID,
                              smlStatement * aStmt,
                              stlInt64       aSpaceID,
                              stlChar      * aNewName,
                              ellEnv       * aEnv )
{
    STL_TRY( eldModifyDictDesc( aTransID,
                                aStmt,
                                ELDT_TABLE_ID_TABLESPACES,
                                ELDT_Space_ColumnOrder_TABLESPACE_ID,
                                aSpaceID,
                                ELDT_Space_ColumnOrder_TABLESPACE_NAME,
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
 * @brief DROP TABLESPACE 를 위한 Dictionary Record 를 삭제한다.
 * @param[in] aTransID      aTransaction ID
 * @param[in] aStmt         Statement
 * @param[in] aSpaceHandle  Tablespace Handle
 * @param[in] aEnv          Environemnt
 * @remarks
 */
stlStatus ellDeleteDict4DropSpace( smlTransId        aTransID,
                                   smlStatement    * aStmt,
                                   ellDictHandle   * aSpaceHandle,
                                   ellEnv          * aEnv )
{
    STL_TRY( eldDeleteDictSpace( aTransID,
                                 aStmt,
                                 aSpaceHandle,
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
 * @param[in]  aSpaceHandle     Tablespace Dictionary Handle
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus ellRebuildSpaceCache( smlTransId        aTransID,
                                smlStatement    * aStmt,
                                ellDictHandle   * aSpaceHandle,
                                ellEnv          * aEnv )
{
    stlInt64  sSpaceID = ELL_DICT_TABLESPACE_ID_NA;
    stlInt64  sCreatorID = ELL_DICT_OBJECT_ID_NA;

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSpaceHandle != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Tablespace ID 획득
     */
    
    sSpaceID = ellGetTablespaceID( aSpaceHandle );
    sCreatorID = ellGetTablespaceCreatorID( aSpaceHandle );

    /**
     * 기존 Cache 를 제거
     */

    STL_TRY( eldcDeleteCacheTablespace( aTransID,
                                        aStmt,
                                        aSpaceHandle,
                                        aEnv )
             == STL_SUCCESS );
    
    /**
     * 새로운 Cache 를 구축 
     */

    STL_TRY( eldRefineCache4AddTablespace( aTransID,
                                           aStmt,
                                           sCreatorID,
                                           sSpaceID,
                                           aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/**
 * @brief SQL-Tablespace Cache 를 추가한다.
 * @param[in]  aTransID       Transaction ID
 * @param[in]  aStmt          Statement
 * @param[in]  aAuthID        Authroization ID
 * @param[in]  aTablespaceID  Tablespace ID
 * @param[in]  aEnv           Envirionment 
 * @remarks
 * 필요한 정보들이 Dictionary Table 에 저장되어 있어야 함.
 */
stlStatus ellRefineCache4AddTablespace( smlTransId           aTransID,
                                        smlStatement       * aStmt,
                                        stlInt64             aAuthID,
                                        stlInt64             aTablespaceID,
                                        ellEnv             * aEnv )
{
    STL_TRY( eldRefineCache4AddTablespace( aTransID,
                                           aStmt,
                                           aAuthID,
                                           aTablespaceID,
                                           aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}



/**
 * @brief DROP TABLESPACE 를 위한 Cache 재구성  
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aSpaceHandle     Tablespace Dictionary Handle
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus ellRefineCache4DropSpace( smlTransId        aTransID,
                                    smlStatement    * aStmt,
                                    ellDictHandle   * aSpaceHandle,
                                    ellEnv          * aEnv )
{
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSpaceHandle != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * SQL-Tablespace Cache 정보를 삭제
     */
    
    STL_TRY( eldcDeleteCacheTablespace( aTransID,
                                        aStmt,
                                        aSpaceHandle,
                                        aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/**
 * @brief ALTER TABLESPACE .. RENAME TO .. 에 대한 Cache 를 재구성
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aSpaceHandle     Tablespace Dictionary Handle
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus ellRefineCache4RenameSpace( smlTransId        aTransID,
                                      smlStatement    * aStmt,
                                      ellDictHandle   * aSpaceHandle,
                                      ellEnv          * aEnv )
{
    return ellRebuildSpaceCache( aTransID,
                                 aStmt,
                                 aSpaceHandle,
                                 aEnv );
}


/*********************************************************
 * Handle 획득 함수
 *********************************************************/




/**
 * @brief Tablespace Name 을 이용해 Tablespace Dictionary Handle 을 얻는다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aViewSCN         View SCN
 * @param[in]  aTablespaceName  Tablespace Name
 * @param[out] aSpaceDictHandle Tablespace Dictionary Handle
 * @param[out] aExist           존재 여부 
 * @param[in]  aEnv             Envirionment 
 * @remarks
 */
stlStatus ellGetTablespaceDictHandle( smlTransId           aTransID,
                                      smlScn               aViewSCN,
                                      stlChar            * aTablespaceName,
                                      ellDictHandle      * aSpaceDictHandle,
                                      stlBool            * aExist,
                                      ellEnv             * aEnv )
{
    ellInitDictHandle( aSpaceDictHandle );
    
    return eldGetTablespaceHandleByName( aTransID,
                                         aViewSCN,
                                         aTablespaceName,
                                         aSpaceDictHandle,
                                         aExist,
                                         aEnv );
}


/**
 * @brief Tablespace ID 를 이용해 Tablespace Dictionary Handle 을 얻는다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aViewSCN         View SCN
 * @param[in]  aTablespaceID    Tablespace ID
 * @param[out] aSpaceDictHandle Tablespace Dictionary Handle
 * @param[out] aExist           존재 여부 
 * @param[in]  aEnv             Envirionment 
 * @remarks
 */
stlStatus ellGetTablespaceDictHandleByID( smlTransId           aTransID,
                                          smlScn               aViewSCN,
                                          stlInt64             aTablespaceID,
                                          ellDictHandle      * aSpaceDictHandle,
                                          stlBool            * aExist,
                                          ellEnv             * aEnv )
{
    ellInitDictHandle( aSpaceDictHandle );
    
    return eldGetTablespaceHandleByID( aTransID,
                                       aViewSCN,
                                       aTablespaceID,
                                       aSpaceDictHandle,
                                       aExist,
                                       aEnv );
}



/*********************************************************
 * 정보 획득 함수
 *********************************************************/


/**
 * @brief Tablespace ID 정보 
 * @param[in] aSpaceDictHandle  Tablespace Dictionary Handle
 * @return Tablespace ID 
 * @remarks
 * 물리적인 ID 로 Storage Manager 로부터 다양한 정보 획득이 가능하다.
 */
stlInt64 ellGetTablespaceID( ellDictHandle * aSpaceDictHandle )
{
    ellTablespaceDesc * sSpaceDesc;

    STL_DASSERT( aSpaceDictHandle->mObjectType == ELL_OBJECT_TABLESPACE );
    
    sSpaceDesc = (ellTablespaceDesc *) ellGetObjectDesc( aSpaceDictHandle );

    return sSpaceDesc->mTablespaceID;
}

/**
 * @brief Tablespace Name 정보
 * @param[in] aSpaceDictHandle  Tablespace Dictionary Handle
 * @return Tablespace Name
 * @remarks
 */
stlChar * ellGetTablespaceName( ellDictHandle * aSpaceDictHandle )
{
    ellTablespaceDesc * sSpaceDesc;

    STL_DASSERT( aSpaceDictHandle->mObjectType == ELL_OBJECT_TABLESPACE );
    
    sSpaceDesc = (ellTablespaceDesc *) ellGetObjectDesc( aSpaceDictHandle );

    return sSpaceDesc->mTablespaceName;
}


/**
 * @brief Tablespace 의 매체 유형 (MEMORY, DISK, ...)
 * @param[in] aSpaceDictHandle  Tablespace Dictionary Handle
 * @return ellTablespaceMediaType
 * @remarks
 */
ellTablespaceMediaType  ellGetTablespaceMediaType( ellDictHandle * aSpaceDictHandle )
{
    ellTablespaceDesc * sSpaceDesc;

    STL_DASSERT( aSpaceDictHandle->mObjectType == ELL_OBJECT_TABLESPACE );
    
    sSpaceDesc = (ellTablespaceDesc *) ellGetObjectDesc( aSpaceDictHandle );

    return sSpaceDesc->mMediaType;
}

/**
 * @brief Tablespace 의 사용 유형 (DATA, TEMP, UNDO, ...)
 * @param[in] aSpaceDictHandle  Tablespace Dictionary Handle
 * @return ellTablespaceUsageType
 * @remarks
 */
ellTablespaceUsageType  ellGetTablespaceUsageType( ellDictHandle * aSpaceDictHandle )
{
    ellTablespaceDesc * sSpaceDesc;

    STL_DASSERT( aSpaceDictHandle->mObjectType == ELL_OBJECT_TABLESPACE );
    
    sSpaceDesc = (ellTablespaceDesc *) ellGetObjectDesc( aSpaceDictHandle );

    return sSpaceDesc->mUsageType;
}


/**
 * @brief Tablespace 객체가 Built-In Object 인지의 여부
 * @param[in]  aSpaceDictHandle  Dictionary Handle
 * @remarks
 */
stlBool  ellIsBuiltInTablespace( ellDictHandle * aSpaceDictHandle )
{
    ellTablespaceDesc * sSpaceDesc;

    STL_DASSERT( aSpaceDictHandle->mObjectType == ELL_OBJECT_TABLESPACE );
    
    sSpaceDesc = (ellTablespaceDesc *) ellGetObjectDesc( aSpaceDictHandle );

    if ( sSpaceDesc->mCreatorID == ELDT_AUTH_ID_SYSTEM )
    {
        return STL_TRUE;
    }
    else
    {
        return STL_FALSE;
    }
}

stlInt64 ellGetTablespaceCreatorID( ellDictHandle * aSpaceDictHandle )
{
    ellTablespaceDesc * sSpaceDesc;

    STL_DASSERT( aSpaceDictHandle->mObjectType == ELL_OBJECT_TABLESPACE );
    
    sSpaceDesc = (ellTablespaceDesc *) ellGetObjectDesc( aSpaceDictHandle );

    return sSpaceDesc->mCreatorID;
}

/** @} ellObjectTablespace */
