/*******************************************************************************
 * ellDictTableConstraint.c
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
 * @file ellDictTableConstraint.c
 * @brief Table Constraint Dictionary 관리 모듈
 */

#include <ell.h>
#include <elDef.h>

#include <eldt.h>
#include <eldc.h>
#include <eldDictionary.h>

#include <eldTable.h>
#include <eldAuthorization.h>
#include <eldTableConstraint.h>


/**
 * @addtogroup ellObjectConstraint
 * @{
 */

/**
 * @brief Table Constraint Type 의 String 상수 
 */

const stlChar * const gEllTableConstraintTypeString[ELL_TABLE_CONSTRAINT_TYPE_MAX] =
{
    "N/A",          /**< ELL_TABLE_CONSTRAINT_TYPE_NA */

    "PRIMARY KEY",  /**< ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY */
    "UNIQUE",       /**< ELL_TABLE_CONSTRAINT_TYPE_UNIQUE_KEY */
    "FOREIGN KEY",  /**< ELL_TABLE_CONSTRAINT_TYPE_FOREIGN_KEY */
    "NOT NULL",     /**< ELL_TABLE_CONSTRAINT_TYPE_CHECK_NOT_NULL */
    "CHECK"         /**< ELL_TABLE_CONSTRAINT_TYPE_CHECK_CLAUSE */
};


/*********************************************************
 * DDL 관련 Object 획득 함수
 *********************************************************/

/**
 * @brief DROP PRIMARY KEY 를 위한 관련 정보 획득
 * @param[in]  aTransID                    Transaction ID
 * @param[in]  aStmt                       Statement
 * @param[in]  aPrimaryKeyHandle           Primary Key Handle
 * @param[in]  aRegionMem                  Region Memory
 * @param[out] aChildForeignKeyList        Child Foreign Key List
 * @param[out] aHasOuterChildForeignKey    Outer Child Foreign Key 의 존재 여부 
 * @param[out] aNotNullList                Not Null Constraint List
 * @param[in]  aEnv                        Environment
 * @remarks
 */
stlStatus ellGetObjectList4DropPK( smlTransId          aTransID,
                                   smlStatement      * aStmt,
                                   ellDictHandle     * aPrimaryKeyHandle,
                                   knlRegionMem      * aRegionMem,
                                   ellObjectList    ** aChildForeignKeyList,
                                   stlBool           * aHasOuterChildForeignKey,
                                   ellObjectList    ** aNotNullList,
                                   ellEnv            * aEnv )
{
    stlInt64        sTableID = ELL_DICT_OBJECT_ID_NA;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPrimaryKeyHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( ellGetConstraintType(aPrimaryKeyHandle)
                        == ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY,
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aChildForeignKeyList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aHasOuterChildForeignKey != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNotNullList != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득 
     */

    sTableID = ellGetConstraintTableID( aPrimaryKeyHandle );
    
    /**
     * Child Foreign Key 정보 획득
     */

    STL_TRY( ellGetObjectList4DropUK( aTransID,
                                      aStmt,
                                      aPrimaryKeyHandle,
                                      aRegionMem,
                                      aChildForeignKeyList,
                                      aHasOuterChildForeignKey,
                                      aEnv )
             == STL_SUCCESS );

    /**
     * Not Null Constraint 정보 획득
     */

    STL_TRY( ellGetTableConstTypeList( aTransID,
                                       aStmt,
                                       sTableID,
                                       ELL_TABLE_CONSTRAINT_TYPE_CHECK_NOT_NULL,
                                       aRegionMem,
                                       aNotNullList,
                                       aEnv )
             == STL_SUCCESS );
                                       

    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/**
 * @brief DROP UNIQUE KEY 를 위한 관련 정보 획득
 * @param[in]  aTransID                  Transaction ID
 * @param[in]  aStmt                     Statement
 * @param[in]  aUniqueKeyHandle          Unique Key Handle
 * @param[in]  aRegionMem                Region Memory
 * @param[out] aChildForeignKeyList      Child Foreign Key List
 * @param[out] aHasOuterChildForeignKey  Outer Child Foreign Key 의 존재 여부 
 * @param[in]  aEnv                      Environment
 * @remarks
 */
stlStatus ellGetObjectList4DropUK( smlTransId          aTransID,
                                   smlStatement      * aStmt,
                                   ellDictHandle     * aUniqueKeyHandle,
                                   knlRegionMem      * aRegionMem,
                                   ellObjectList    ** aChildForeignKeyList,
                                   stlBool           * aHasOuterChildForeignKey,
                                   ellEnv            * aEnv )
{
    ellObjectList * sKeyList  = NULL;

    stlInt64        sConstID = ELL_DICT_OBJECT_ID_NA;
    stlInt64        sTableID = ELL_DICT_OBJECT_ID_NA;

    stlBool         sHasOuter = STL_FALSE;
    
    ellObjectItem   * sObjectItem = NULL;
    ellDictHandle   * sObjectHandle = NULL;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aUniqueKeyHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (ellGetConstraintType(aUniqueKeyHandle)
                         == ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY) ||
                        (ellGetConstraintType(aUniqueKeyHandle)
                         == ELL_TABLE_CONSTRAINT_TYPE_UNIQUE_KEY),
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aChildForeignKeyList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aHasOuterChildForeignKey != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득 
     */

    sConstID = ellGetConstraintID( aUniqueKeyHandle );
    sTableID = ellGetConstraintTableID( aUniqueKeyHandle );

    /**
     * Constraint 를 참조하는 Child Foreign Key List 를 획득한다.
     */

    STL_TRY( ellInitObjectList( & sKeyList, aRegionMem, aEnv ) == STL_SUCCESS );

    /**
     * Unique Key 와 관련된 Child Foreign Key 정보 획득
     * - SELECT CONSTRAINT_ID
     * - FROM DEFINITION_SCHEMA.REFERENTIAL_CONSTRAINTS
     * - WHERE UNIQUE_CONSTRAINT_ID = sConstID
     */
    
    STL_TRY( eldGetObjectList( aTransID,
                               aStmt,
                               ELDT_TABLE_ID_REFERENTIAL_CONSTRAINTS,
                               ELL_OBJECT_CONSTRAINT,
                               ELDT_Reference_ColumnOrder_CONSTRAINT_ID,
                               ELDT_Reference_ColumnOrder_UNIQUE_CONSTRAINT_ID,
                               sConstID,
                               aRegionMem,
                               sKeyList,
                               aEnv )
             == STL_SUCCESS );

    /**
     * Child Foreign Key 중 Outer Child Foreign Key 가 존재하는 지 검사 
     */

    sHasOuter = STL_FALSE;
    
    STL_RING_FOREACH_ENTRY( & sKeyList->mHeadList, sObjectItem )
    {
        sObjectHandle = & sObjectItem->mObjectHandle;

        if ( ellGetConstraintTableID( sObjectHandle ) == sTableID )
        {
            /**
             * Self Child Foreign Key
             */

            continue;
        }
        else
        {
            /**
             * Outer Child Foreign Key
             */

            sHasOuter = STL_TRUE;
            break;
        }
    }
    
    /**
     * Output 설정
     */

    *aChildForeignKeyList  = sKeyList;
    *aHasOuterChildForeignKey = sHasOuter;
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/**
 * @brief Foreign Key 가 참조하는 Parent Key 목록 획득 
 * @param[in]  aTransID               Transaction ID
 * @param[in]  aStmt                  Statement
 * @param[in]  aForeignKeyHandle      Foreign Key Constraint Dictionary Handle
 * @param[in]  aRegionMem             Region Memory
 * @param[out] aParentKeyList         Parent Unique Key List
 * @param[in]  aEnv                   Environment
 * @remarks
 */
stlStatus ellGetObjectList4DropFK( smlTransId          aTransID,
                                   smlStatement      * aStmt,
                                   ellDictHandle     * aForeignKeyHandle,
                                   knlRegionMem      * aRegionMem,
                                   ellObjectList    ** aParentKeyList,
                                   ellEnv            * aEnv )
{
    ellObjectList * sUniqueKeyList = NULL;

    stlInt64        sConstID = ELL_DICT_OBJECT_ID_NA;

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aForeignKeyHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( ellGetConstraintType( aForeignKeyHandle )
                        == ELL_TABLE_CONSTRAINT_TYPE_FOREIGN_KEY,
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParentKeyList != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득 
     */

    sConstID = ellGetConstraintID( aForeignKeyHandle );

    /**
     * Foreign Key 가 참조하는 Unique Key 를 획득한다.
     */

    STL_TRY( ellInitObjectList( & sUniqueKeyList, aRegionMem, aEnv ) == STL_SUCCESS );

    
    /**
     * Foreign 가 참조하는 Unique Key 정보 획득
     * - SELECT UNIQUE_CONSTRAINT_ID
     * - FROM DEFINITION_SCHEMA.REFERENTIAL_CONSTRAINTS
     * - WHERE CONSTRAINT_ID = sConstID
     */
    
    STL_TRY( eldGetObjectList( aTransID,
                               aStmt,
                               ELDT_TABLE_ID_REFERENTIAL_CONSTRAINTS,
                               ELL_OBJECT_CONSTRAINT,
                               ELDT_Reference_ColumnOrder_UNIQUE_CONSTRAINT_ID,
                               ELDT_Reference_ColumnOrder_CONSTRAINT_ID,
                               sConstID,
                               aRegionMem,
                               sUniqueKeyList,
                               aEnv )
             == STL_SUCCESS );

    /**
     * Output 설정
     */

    *aParentKeyList  = sUniqueKeyList;
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}

/**
 * @brief Primary/Unique Key List 로부터 Child Foreign Key List 획득
 * @param[in]  aTransID               Transaction ID
 * @param[in]  aStmt                  Statement
 * @param[in]  aUniqueList            Unique(PK/UK) List
 * @param[in]  aRegionMem             Region Memory
 * @param[out] aChildForeignKeyList   Child Foreign Key List
 * @param[in]  aEnv                   Environment
 * @remarks
 */
stlStatus ellGetChildForeignKeyListByUniqueList( smlTransId          aTransID,
                                                 smlStatement      * aStmt,
                                                 ellObjectList     * aUniqueList,
                                                 knlRegionMem      * aRegionMem,
                                                 ellObjectList    ** aChildForeignKeyList,
                                                 ellEnv            * aEnv )
{
    ellObjectList * sRefList = NULL;

    ellObjectItem   * sObjectItem = NULL;
    ellDictHandle   * sObjectHandle = NULL;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aUniqueList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aChildForeignKeyList != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Unique List 를 순회하며 Child Foreign Key List 를 구축
     */

    STL_TRY( ellInitObjectList( & sRefList, aRegionMem, aEnv ) == STL_SUCCESS );
    
    STL_RING_FOREACH_ENTRY( & aUniqueList->mHeadList, sObjectItem )
    {
        sObjectHandle = & sObjectItem->mObjectHandle;

        /**
         * Unique Key 와 관련된 Child Foreign Key 정보 획득
         * - SELECT CONSTRAINT_ID
         * - FROM DEFINITION_SCHEMA.REFERENTIAL_CONSTRAINTS
         * - WHERE UNIQUE_CONSTRAINT_ID = ellGetConstraintID(sObjectHandle)
         */
        
        STL_TRY( eldGetObjectList( aTransID,
                                   aStmt,
                                   ELDT_TABLE_ID_REFERENTIAL_CONSTRAINTS,
                                   ELL_OBJECT_CONSTRAINT,
                                   ELDT_Reference_ColumnOrder_CONSTRAINT_ID,
                                   ELDT_Reference_ColumnOrder_UNIQUE_CONSTRAINT_ID,
                                   ellGetConstraintID(sObjectHandle),
                                   aRegionMem,
                                   sRefList,
                                   aEnv )
                 == STL_SUCCESS );
    }

    /**
     * Output 설정
     */

    *aChildForeignKeyList = sRefList;
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/**
 * @brief DROP NOT NULL 를 위한 관련 정보 획득
 * @param[in]  aTransID           Transaction ID
 * @param[in]  aStmt              Statement
 * @param[in]  aNotNullHandle     NOT NULL constraint handle
 * @param[in]  aRegionMem         Region Memory
 * @param[out] aPrimaryKeyList    Primary Key List
 * @param[in]  aEnv               Environment
 * @remarks
 */
stlStatus ellGetObjectList4DropNotNull( smlTransId          aTransID,
                                        smlStatement      * aStmt,
                                        ellDictHandle     * aNotNullHandle,
                                        knlRegionMem      * aRegionMem,
                                        ellObjectList    ** aPrimaryKeyList,
                                        ellEnv            * aEnv )
{
    stlInt64        sTableID = ELL_DICT_OBJECT_ID_NA;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNotNullHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( ellGetConstraintType(aNotNullHandle)
                        == ELL_TABLE_CONSTRAINT_TYPE_CHECK_NOT_NULL,
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPrimaryKeyList != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득 
     */

    sTableID = ellGetConstraintTableID( aNotNullHandle );
    
    /**
     * PRIMARY KEY Constraint 정보 획득
     */

    STL_TRY( ellGetTableConstTypeList( aTransID,
                                       aStmt,
                                       sTableID,
                                       ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY,
                                       aRegionMem,
                                       aPrimaryKeyList,
                                       aEnv )
             == STL_SUCCESS );
                                       

    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/**
 * @brief Table 의 모든 Constraint List 를 얻는다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aTableID         Table ID
 * @param[in]  aRegionMem       Region Memory
 * @param[out] aConstraintList  Constraint List
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus ellGetTableAllConstraintList( smlTransId          aTransID,
                                        smlStatement      * aStmt,
                                        stlInt64            aTableID,
                                        knlRegionMem      * aRegionMem,
                                        ellObjectList    ** aConstraintList,
                                        ellEnv            * aEnv )
{
    ellObjectList * sObjectList = NULL;

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableID > ELL_DICT_OBJECT_ID_NA, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstraintList != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Table Constraint List 를 획득 
     */

    STL_TRY( ellInitObjectList( & sObjectList, aRegionMem, aEnv ) == STL_SUCCESS );

    /**
     * Unique Key 와 관련된 Child Foreign Key 정보 획득
     * - SELECT CONSTRAINT_ID
     * - FROM DEFINITION_SCHEMA.TABLE_CONSTRAINTS
     * - WHERE TABLE_ID = sTableID
     */
    
    STL_TRY( eldGetObjectList( aTransID,
                               aStmt,
                               ELDT_TABLE_ID_TABLE_CONSTRAINTS,
                               ELL_OBJECT_CONSTRAINT,
                               ELDT_TableConst_ColumnOrder_CONSTRAINT_ID,
                               ELDT_TableConst_ColumnOrder_TABLE_ID,
                               aTableID,
                               aRegionMem,
                               sObjectList,
                               aEnv )
             == STL_SUCCESS );

    /**
     * Output 설정
     */

    *aConstraintList = sObjectList;
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}

/**
 * @brief Table 의 Constraint 중 특정 Constraint Type List 를 얻는다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aTableID         Table ID
 * @param[in]  aConstType       Constraint Type
 * @param[in]  aRegionMem       Region Memory
 * @param[out] aConstList       Constraint Type 에 해당하는 Object List
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus ellGetTableConstTypeList( smlTransId                aTransID,
                                    smlStatement            * aStmt,
                                    stlInt64                  aTableID,
                                    ellTableConstraintType    aConstType,
                                    knlRegionMem            * aRegionMem,
                                    ellObjectList          ** aConstList,
                                    ellEnv                  * aEnv )
{
    ellObjectList * sConstObjectList = NULL;
    ellObjectList * sObjectList = NULL;

    ellObjectItem * sObjectItem = NULL;
    ellDictHandle * sObjectHandle = NULL;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableID > ELL_DICT_OBJECT_ID_NA, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aConstType > ELL_TABLE_CONSTRAINT_TYPE_NA) &&
                        (aConstType < ELL_TABLE_CONSTRAINT_TYPE_MAX),
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstList != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Table 의 Constraint 획득
     */

    STL_TRY( ellGetTableAllConstraintList( aTransID,
                                           aStmt,
                                           aTableID,
                                           aRegionMem,
                                           & sObjectList,
                                           aEnv )
             == STL_SUCCESS );

    /**
     * 특정 Constraint Type 만을 획득함 
     */

    STL_TRY( ellInitObjectList( & sConstObjectList, aRegionMem, aEnv ) == STL_SUCCESS );
    
    STL_RING_FOREACH_ENTRY( & sObjectList->mHeadList, sObjectItem )
    {
        sObjectHandle = & sObjectItem->mObjectHandle;

        if ( ellGetConstraintType( sObjectHandle ) == aConstType )
        {
            /**
             * 원하는 Constraint 임
             */
            
            STL_TRY( ellAddNewObjectItem( sConstObjectList,
                                          sObjectHandle,
                                          NULL,
                                          aRegionMem,
                                          aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            /**
             * 원하는 Constraint 가 아님
             * - nothing to do 
             */
        }
    }
    
    /**
     * Output 설정
     */

    *aConstList = sConstObjectList;
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}





/**
 * @brief Key 목록에서 Table List 를 획득 
 * @param[in]  aTransID               Transaction ID
 * @param[in]  aViewSCN               View SCN
 * @param[in]  aForeignKeyList        Foreign Key List
 * @param[in]  aRegionMem             Region Memory
 * @param[out] aParentTableList       Parent Table List
 * @param[in]  aEnv                   Environment
 * @remarks
 */
stlStatus ellGetParentTableList4ForeignKeyList( smlTransId          aTransID,
                                                smlScn              aViewSCN,
                                                ellObjectList     * aForeignKeyList,
                                                knlRegionMem      * aRegionMem,
                                                ellObjectList    ** aParentTableList,
                                                ellEnv            * aEnv )
{
    ellObjectList * sTableList = NULL;

    stlInt64      sTableID = ELL_DICT_OBJECT_ID_NA;
    ellDictHandle sTableHandle;
    stlBool       sObjectExist = STL_FALSE;

    ellObjectItem   * sObjectItem = NULL;
    ellDictHandle   * sObjectHandle = NULL;
    
    ellDictHandle   * sUniqueHandle = NULL;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aForeignKeyList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParentTableList != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Foreign Key List 를 순회하며, Parent Table List 를 구성한다.
     */

    STL_TRY( ellInitObjectList( & sTableList, aRegionMem, aEnv ) == STL_SUCCESS );
    
    STL_RING_FOREACH_ENTRY( & aForeignKeyList->mHeadList, sObjectItem )
    {
        sObjectHandle = & sObjectItem->mObjectHandle;

        /**
         * Parent Unique Key Handle 획득 
         */
        
        sUniqueHandle = ellGetForeignKeyParentUniqueKeyHandle( sObjectHandle );
        sTableID = ellGetConstraintTableID( sUniqueHandle );

        /**
         * Parent Unique Key 의 Table Handle 획득
         */

        STL_TRY( ellGetTableDictHandleByID( aTransID,
                                            aViewSCN,
                                            sTableID,
                                            & sTableHandle,
                                            & sObjectExist,
                                            aEnv )
                 == STL_SUCCESS );

        STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_OBJECT_NOT_EXIST );

        /**
         * Table Handle 을 List 에 추가
         */

        STL_TRY( ellAppendObjectItem( sTableList,
                                      & sTableHandle,
                                      aRegionMem,
                                      aEnv )
                 == STL_SUCCESS );
    }
    
    /**
     * Output 설정
     */

    *aParentTableList  = sTableList;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_OBJECT_NOT_EXIST )
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
 * @brief Constraint 목록에서 Table List 를 획득 
 * @param[in]  aTransID               Transaction ID
 * @param[in]  aViewSCN               View SCN
 * @param[in]  aConstList             Constraint List
 * @param[in]  aRegionMem             Region Memory
 * @param[out] aTableList             Table List
 * @param[in]  aEnv                   Environment
 * @remarks
 */
stlStatus ellGetTableList4ConstList( smlTransId          aTransID,
                                     smlScn              aViewSCN,
                                     ellObjectList     * aConstList,
                                     knlRegionMem      * aRegionMem,
                                     ellObjectList    ** aTableList,
                                     ellEnv            * aEnv )
{
    ellObjectList * sTableList = NULL;

    stlInt64      sTableID = ELL_DICT_OBJECT_ID_NA;
    ellDictHandle sTableHandle;
    stlBool       sObjectExist = STL_FALSE;

    ellObjectItem   * sObjectItem = NULL;
    ellDictHandle   * sObjectHandle = NULL;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aConstList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableList != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Constraint List 를 순회하며, Table List 를 구성한다.
     */

    STL_TRY( ellInitObjectList( & sTableList, aRegionMem, aEnv ) == STL_SUCCESS );
    
    STL_RING_FOREACH_ENTRY( & aConstList->mHeadList, sObjectItem )
    {
        sObjectHandle = & sObjectItem->mObjectHandle;

        /**
         * Table ID 획득
         */
        
        sTableID = ellGetConstraintTableID( sObjectHandle );

        /**
         * Table Handle 획득
         */

        STL_TRY( ellGetTableDictHandleByID( aTransID,
                                            aViewSCN,
                                            sTableID,
                                            & sTableHandle,
                                            & sObjectExist,
                                            aEnv )
                 == STL_SUCCESS );

        STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_OBJECT_NOT_EXIST );

        /**
         * Table Handle 을 List 에 추가
         */

        STL_TRY( ellAppendObjectItem( sTableList,
                                      & sTableHandle,
                                      aRegionMem,
                                      aEnv )
                 == STL_SUCCESS );
    }
    
    /**
     * Output 설정
     */

    *aTableList  = sTableList;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_OBJECT_NOT_EXIST )
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
 * @brief Key 목록에서 Index List 를 획득 
 * @param[in]  aTransID               Transaction ID
 * @param[in]  aKeyList               Key List
 * @param[in]  aRegionMem             Region Memory
 * @param[out] aIndexList             Index List
 * @param[in]  aEnv                   Environment
 * @remarks
 */
stlStatus ellGetIndexList4KeyList( smlTransId          aTransID,
                                   ellObjectList     * aKeyList,
                                   knlRegionMem      * aRegionMem,
                                   ellObjectList    ** aIndexList,
                                   ellEnv            * aEnv )
{
    ellObjectList * sIndexList = NULL;
    ellDictHandle * sIndexHandle;

    ellObjectItem   * sObjectItem = NULL;
    ellDictHandle   * sObjectHandle = NULL;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aKeyList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIndexList != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Key List 를 순회하며, Index List 를 구성한다.
     */

    STL_TRY( ellInitObjectList( & sIndexList, aRegionMem, aEnv ) == STL_SUCCESS );
    
    STL_RING_FOREACH_ENTRY( & aKeyList->mHeadList, sObjectItem )
    {
        sObjectHandle = & sObjectItem->mObjectHandle;

        /**
         * Index Handle 획득
         */

        
        sIndexHandle = ellGetConstraintIndexDictHandle( sObjectHandle );

        /**
         * Index Handle 을 List 에 추가
         */

        STL_TRY( ellAddNewObjectItem( sIndexList,
                                      sIndexHandle,
                                      NULL,
                                      aRegionMem,
                                      aEnv )
                 == STL_SUCCESS );
    }
    
    /**
     * Output 설정
     */

    *aIndexList  = sIndexList;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/*********************************************************
 * Constraint 에 대한 DDL Lock 함수 
 *********************************************************/


/**
 * @brief ALTER TABLE .. ADD CONSTRAINT 구문을 위한 DDL Lock 을 획득한다.
 * @param[in]  aTransID             Transaction ID
 * @param[in]  aStmt                Statement
 * @param[in]  aConstOwnerHandle    Constraint 의 Owner Handle
 * @param[in]  aConstSchemaHandle   Constraint 의 Schema Handle
 * @param[in]  aConstSpaceHandle    Constraint Index 의 Space Handle (nullable when NOT NULL 등) 
 * @param[in]  aTableDictHandle     Table Dictionary Handle
 * @param[in]  aConstType           Constraint Type
 * @param[out] aLockSuccess         Lock 획득 여부 
 * @param[in]  aEnv                 Environment
 * @remarks
 */
stlStatus ellLock4AddConst( smlTransId               aTransID,
                            smlStatement           * aStmt,
                            ellDictHandle          * aConstOwnerHandle,
                            ellDictHandle          * aConstSchemaHandle,
                            ellDictHandle          * aConstSpaceHandle,
                            ellDictHandle          * aTableDictHandle,
                            ellTableConstraintType   aConstType,
                            stlBool                * aLockSuccess,
                            ellEnv                 * aEnv )
{
    stlInt32  sLockMode = 0;

    stlBool   sLocked = STL_TRUE;
    
    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstOwnerHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstSchemaHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aConstType > ELL_TABLE_CONSTRAINT_TYPE_NA) &&
                        (aConstType < ELL_TABLE_CONSTRAINT_TYPE_MAX),
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aLockSuccess != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Lock 유형 결정
     */

    switch ( aConstType )
    {
        case ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY:
            sLockMode = SML_LOCK_X;
            break;
        case ELL_TABLE_CONSTRAINT_TYPE_UNIQUE_KEY:
            sLockMode = SML_LOCK_S;
            break;
        case ELL_TABLE_CONSTRAINT_TYPE_FOREIGN_KEY:
            sLockMode = SML_LOCK_S;
            break;
        case ELL_TABLE_CONSTRAINT_TYPE_CHECK_NOT_NULL:
            sLockMode = SML_LOCK_X;
            break;
        case ELL_TABLE_CONSTRAINT_TYPE_CHECK_CLAUSE:
            sLockMode = SML_LOCK_S;
            break;
        default:
            STL_ASSERT(0);
            break;
    }

    /**
     * Owner Record 에 S lock
     */
    
    STL_TRY( eldLockRowByObjectHandle( aTransID,
                                       aStmt,
                                       ELDT_TABLE_ID_AUTHORIZATIONS,
                                       ELDT_Auth_ColumnOrder_AUTH_ID,
                                       aConstOwnerHandle,
                                       SML_LOCK_S,
                                       & sLocked,
                                       aEnv )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_FINISH );
    
    /**
     * Schema Record 에 S lock 설정 
     */

    STL_TRY( eldLockRowByObjectHandle( aTransID,
                                       aStmt,
                                       ELDT_TABLE_ID_SCHEMATA,
                                       ELDT_Schemata_ColumnOrder_SCHEMA_ID,
                                       aConstSchemaHandle,
                                       SML_LOCK_S,
                                       & sLocked,
                                       aEnv )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_FINISH );

    /**
     * Tablespace Record 에 S lock 설정 
     */
    
    if ( aConstSpaceHandle != NULL )
    {
        STL_TRY( eldLockRowByObjectHandle( aTransID,
                                           aStmt,
                                           ELDT_TABLE_ID_TABLESPACES,
                                           ELDT_Space_ColumnOrder_TABLESPACE_ID,
                                           aConstSpaceHandle,
                                           SML_LOCK_S,
                                           & sLocked,
                                           aEnv )
             == STL_SUCCESS );
        
        STL_TRY_THROW( sLocked == STL_TRUE, RAMP_FINISH );
    }
    else
    {
        /* nothing to do */
    }
    
    /**
     * Table 객체에 lock 설정 
     */
    
    STL_TRY( eldLockTable4DDL( aTransID,
                               aStmt,
                               sLockMode,
                               aTableDictHandle,
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
 * @brief ALTER TABLE .. DROP CONSTRAINT 구문을 위한 DDL Lock 을 획득한다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aTableDictHandle Table Dictionary Handle
 * @param[in]  aConstDictHandle Constraint Dictionary Handle
 * @param[out] aLockSuccess     Lock 획득 여부 
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus ellLock4DropConst( smlTransId      aTransID,
                             smlStatement  * aStmt,
                             ellDictHandle * aTableDictHandle,
                             ellDictHandle * aConstDictHandle,
                             stlBool       * aLockSuccess,
                             ellEnv        * aEnv )
{
    stlInt32  sLockMode = 0;

    stlBool  sLocked = STL_TRUE;

    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aLockSuccess != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Lock 유형 결정
     */

    switch ( ellGetConstraintType( aConstDictHandle ) )
    {
        case ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY:
            sLockMode = SML_LOCK_X;
            break;
        case ELL_TABLE_CONSTRAINT_TYPE_UNIQUE_KEY:
            sLockMode = SML_LOCK_S;
            break;
        case ELL_TABLE_CONSTRAINT_TYPE_FOREIGN_KEY:
            sLockMode = SML_LOCK_S;
            break;
        case ELL_TABLE_CONSTRAINT_TYPE_CHECK_NOT_NULL:
            sLockMode = SML_LOCK_X;
            break;
        case ELL_TABLE_CONSTRAINT_TYPE_CHECK_CLAUSE:
            sLockMode = SML_LOCK_S;
            break;
        default:
            STL_ASSERT(0);
            break;
    }
    
    /**
     * Table 객체에 lock 설정 
     */
    
    STL_TRY( eldLockTable4DDL( aTransID,
                               aStmt,
                               sLockMode,
                               aTableDictHandle,
                               & sLocked,
                               aEnv )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_FINISH );
    
    /**
     * Constraint Record 에 X lock 을 획득한다.
     */

    STL_TRY( eldLockRowByObjectHandle( aTransID,
                                       aStmt,
                                       ELDT_TABLE_ID_TABLE_CONSTRAINTS,
                                       ELDT_TableConst_ColumnOrder_CONSTRAINT_ID,
                                       aConstDictHandle,
                                       SML_LOCK_X,
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
 * @brief ALTER TABLE .. ALTER CONSTRAINT 구문을 위한 DDL Lock 을 획득한다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aTableDictHandle Table Dictionary Handle
 * @param[in]  aConstDictHandle Constraint Dictionary Handle
 * @param[out] aLockSuccess     Lock 획득 여부 
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus ellLock4AlterConst( smlTransId      aTransID,
                              smlStatement  * aStmt,
                              ellDictHandle * aTableDictHandle,
                              ellDictHandle * aConstDictHandle,
                              stlBool       * aLockSuccess,
                              ellEnv        * aEnv )
{
    STL_TRY( ellLock4DropConst( aTransID,
                                aStmt,
                                aTableDictHandle,
                                aConstDictHandle,
                                aLockSuccess,
                                aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief DDL 수행시 관련 Constraint List 에 대한 Lock 을 획득한다.
 * @param[in]  aTransID       Transaction ID
 * @param[in]  aStmt          Statement
 * @param[in]  aConstList     Constraint Object List
 * @param[out] aLockSuccess   Lock 획득 여부 
 * @param[in]  aEnv           Environment
 * @remarks
 */
stlStatus ellLock4ConstraintList( smlTransId      aTransID,
                                  smlStatement  * aStmt,
                                  ellObjectList * aConstList,
                                  stlBool       * aLockSuccess,
                                  ellEnv        * aEnv )
{
    ellObjectItem * sObjectItem = NULL;
    ellDictHandle * sObjectHandle = NULL;

    stlBool  sLocked = STL_TRUE;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aLockSuccess != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * Table 목록을 순회하며 lock 을 획득 
     */

    STL_RING_FOREACH_ENTRY( & aConstList->mHeadList, sObjectItem )
    {
        sObjectHandle = & sObjectItem->mObjectHandle;

        /**
         * Constraint Record 에 X lock 을 획득한다.
         */
        
        STL_TRY( eldLockRowByObjectHandle( aTransID,
                                           aStmt,
                                           ELDT_TABLE_ID_TABLE_CONSTRAINTS,
                                           ELDT_TableConst_ColumnOrder_CONSTRAINT_ID,
                                           sObjectHandle,
                                           SML_LOCK_X,
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







/*********************************************************
 * Dictionary 변경 함수 
 *********************************************************/



/**
 * @brief Table Constraint Descriptor 를 Dictionary 에 추가한다.
 * @param[in]  aTransID             Transaction ID
 * @param[in]  aStmt                Statement
 * @param[in]  aConstOwnerID        Table Constraint 의 Owner ID
 * @param[in]  aConstSchemaID       Table Constraint 가 속한 Schema ID
 * @param[out] aConstID             Table Constraint 의 ID 
 * @param[in]  aTableOwnerID        Table Owner ID
 * @param[in]  aTableSchemaID       Table Schema ID
 * @param[in]  aTableID             Table ID
 * @param[in]  aConstName           Table Constraint 이름 
 * @param[in]  aConstType           Table Constraint 유형
 * @param[in]  aIsDeferrable        Deferrable 여부
 * @param[in]  aInitiallyDeferred   Initially Deferred 여부 
 * @param[in]  aEnforced            Enforced 여부 
 * @param[in]  aIndexID             Constraint 와 관련된 Index의 ID
 *             <BR> - 관련된 인덱스가 없다면, ELL_DICT_OBJECT_ID_NA
 * @param[in]  aConstComment        Table Constraint 의 Comment
 * @param[in]  aEnv                 Execution Library Environment
 * @remarks
 */

stlStatus ellInsertTableConstraintDesc( smlTransId               aTransID,
                                        smlStatement           * aStmt,
                                        stlInt64                 aConstOwnerID,
                                        stlInt64                 aConstSchemaID,
                                        stlInt64               * aConstID,
                                        stlInt64                 aTableOwnerID,
                                        stlInt64                 aTableSchemaID,
                                        stlInt64                 aTableID,
                                        stlChar                * aConstName,
                                        ellTableConstraintType   aConstType,
                                        stlBool                  aIsDeferrable,
                                        stlBool                  aInitiallyDeferred,
                                        stlBool                  aEnforced,
                                        stlInt64                 aIndexID,
                                        stlChar                * aConstComment,
                                        ellEnv                 * aEnv )
{
    STL_TRY( eldInsertTableConstraintDesc( aTransID,
                                           aStmt,
                                           aConstOwnerID,
                                           aConstSchemaID,
                                           aConstID,
                                           aTableOwnerID,
                                           aTableSchemaID,
                                           aTableID,
                                           aConstName,
                                           aConstType,
                                           aIsDeferrable,
                                           aInitiallyDeferred,
                                           aEnforced,
                                           aIndexID,
                                           aConstComment,
                                           aEnv )
             == STL_SUCCESS );
        
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}

/**
 * @brief DROP CONSTRAINT 를 위한 Dictionary Record 를 삭제한다.
 * @param[in] aTransID      aTransaction ID
 * @param[in] aStmt         Statement
 * @param[in] aConstHandle  Constraint Handle
 * @param[in] aEnv          Environemnt
 * @remarks
 */
stlStatus ellDeleteDict4DropConstraint( smlTransId        aTransID,
                                        smlStatement    * aStmt,
                                        ellDictHandle   * aConstHandle,
                                        ellEnv          * aEnv )
{
    STL_TRY( eldDeleteDictConst( aTransID,
                                 aStmt,
                                 aConstHandle,
                                 aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/**
 * @brief Constraint List 의 Dictionary Record 를 삭제한다.
 * @param[in] aTransID          aTransaction ID
 * @param[in] aStmt             Statement
 * @param[in] aConstObjectList  Constraint Object List
 * @param[in] aEnv              Environemnt
 * @remarks
 */
stlStatus ellDeleteDictConstraintByObjectList( smlTransId        aTransID,
                                               smlStatement    * aStmt,
                                               ellObjectList   * aConstObjectList,
                                               ellEnv          * aEnv )
{
    ellObjectItem   * sObjectItem = NULL;
    ellDictHandle   * sObjectHandle = NULL;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstObjectList != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Constraint Object List 를 순회하며, Dictionary Record 를 삭제한다.
     */

    STL_RING_FOREACH_ENTRY( & aConstObjectList->mHeadList, sObjectItem )
    {
        sObjectHandle = & sObjectItem->mObjectHandle;

        STL_TRY( ellDeleteDict4DropConstraint( aTransID,
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
 * @brief ALTER CONSTRAINT 를 위한 Constraint Characteristic Dictionary 정보를 변경한다.
 * @param[in] aTransID            Transaction ID
 * @param[in] aStmt               Statement
 * @param[in] aConstHandle        Constraint Dictionary Handle
 * @param[in] aIsDeferrable       DEFERRABLE / NOT DEFERRABLE
 * @param[in] aInitiallyDeferred  INITIALLY DEFERRED / INITIALLY IMMEDIATE
 * @param[in] aEnforced           ENFORCED / NOT ENFORCED
 * @param[in] aEnv                Environment
 * @remarks
 */
stlStatus ellModifyDict4AlterConstFeature( smlTransId       aTransID,
                                           smlStatement   * aStmt,
                                           ellDictHandle  * aConstHandle,
                                           stlBool          aIsDeferrable,
                                           stlBool          aInitiallyDeferred,
                                           stlBool          aEnforced,
                                           ellEnv         * aEnv )
{
    STL_TRY( eldModifyDictConstFeature( aTransID,
                                        aStmt,
                                        aConstHandle,
                                        aIsDeferrable,
                                        aInitiallyDeferred,
                                        aEnforced,
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
 * @brief SQL-Constraint Cache 를 추가한다.
 * @param[in]  aTransID     Transaction ID
 * @param[in]  aStmt        Statement
 * @param[in]  aTableHandle Table Dictionary Handle
 * @param[in]  aConstID     Constraint ID
 * @param[in]  aConstType   Constraint Type
 * @param[in]  aEnv         Envirionment 
 * @remarks
 */
stlStatus ellRefineCache4AddConstraint( smlTransId               aTransID,
                                        smlStatement           * aStmt,
                                        ellDictHandle          * aTableHandle,
                                        stlInt64                 aConstID,
                                        ellTableConstraintType   aConstType,
                                        ellEnv                 * aEnv )
{
    stlInt64  sTableID = ELL_DICT_OBJECT_ID_NA;
    
    /**
     * Parameter Valdation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableHandle > ELL_DICT_OBJECT_ID_NA, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstID > ELL_DICT_OBJECT_ID_NA, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aConstType > ELL_TABLE_CONSTRAINT_TYPE_NA) &&
                        (aConstType < ELL_TABLE_CONSTRAINT_TYPE_MAX),
                        ELL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */

    sTableID = ellGetTableID( aTableHandle );
    
    /**
     * SQL-Constraint Cache 를 추가 
     */
    
    STL_TRY( eldcInsertCacheConstByConstID( aTransID,
                                            aStmt,
                                            aConstID,
                                            aEnv )
             == STL_SUCCESS );
    
    /**
     * 관련 Table 에 Constraint 정보를 등록
     */
    
    STL_TRY( eldcAddConstraintIntoTable( aTransID,
                                         aConstID,
                                         aEnv )
             == STL_SUCCESS );

    switch ( aConstType )
    {
        case ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY:
            {
                /**
                 * DDL Supplemental Logging 기록
                 */
                
                STL_TRY( eldWriteSuppLogTableDDL( aTransID,
                                                  sTableID,
                                                  ELL_SUPP_LOG_ADD_CONSTRAINT_PRIMARY_KEY,
                                                  aEnv )
                         == STL_SUCCESS );
                
                /**
                 * Column 속성이 변경되므로 Table Cache 를 재구축한다.
                 */
                STL_TRY( ellRebuildTableCache( aTransID,
                                               aStmt,
                                               aTableHandle,
                                               aEnv )
                         == STL_SUCCESS );
                break;
            }
        case ELL_TABLE_CONSTRAINT_TYPE_UNIQUE_KEY:
            {
                /**
                 * DDL Supplemental Logging 기록
                 */
                
                STL_TRY( eldWriteSuppLogTableDDL( aTransID,
                                                  sTableID,
                                                  ELL_SUPP_LOG_ADD_CONSTRAINT_UNIQUE_KEY,
                                                  aEnv )
                         == STL_SUCCESS );

                break;
            }
        case ELL_TABLE_CONSTRAINT_TYPE_FOREIGN_KEY:
            {
                /**
                 * DDL Supplemental Logging 기록
                 */
                
                STL_TRY( eldWriteSuppLogTableDDL( aTransID,
                                                  sTableID,
                                                  ELL_SUPP_LOG_ADD_CONSTRAINT_FOREIGN_KEY,
                                                  aEnv )
                         == STL_SUCCESS );
                
                break;
            }
        case ELL_TABLE_CONSTRAINT_TYPE_CHECK_NOT_NULL:
            {
                /**
                 * DDL Supplemental Logging 기록
                 */
                
                STL_TRY( eldWriteSuppLogTableDDL( aTransID,
                                                  sTableID,
                                                  ELL_SUPP_LOG_ADD_CONSTRAINT_CHECK_NOT_NULL,
                                                  aEnv )
                         == STL_SUCCESS );
                
                /**
                 * Column 속성이 변경되므로 Table Cache 를 재구축한다.
                 */
                STL_TRY( ellRebuildTableCache( aTransID,
                                               aStmt,
                                               aTableHandle,
                                               aEnv )
                         == STL_SUCCESS );
                break;
            }
        case ELL_TABLE_CONSTRAINT_TYPE_CHECK_CLAUSE:
            {
                /**
                 * DDL Supplemental Logging 기록
                 */
                
                STL_TRY( eldWriteSuppLogTableDDL( aTransID,
                                                  sTableID,
                                                  ELL_SUPP_LOG_ADD_CONSTRAINT_CHECK_CLAUSE,
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
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/**
 * @brief DROP Constraint 를 위한 Cache 를 재구성
 * @param[in] aTransID          Transaction ID
 * @param[in] aStmt             Statement
 * @param[in] aTableHandle      Table Handle
 * @param[in] aConstHandle      Constraint Handle
 * @param[in] aEnv              Environment
 * @remarks
 */
stlStatus ellRefineCache4DropConstraint( smlTransId           aTransID,
                                         smlStatement       * aStmt,
                                         ellDictHandle      * aTableHandle,
                                         ellDictHandle      * aConstHandle,
                                         ellEnv             * aEnv )
{
    stlInt64  sTableID = ELL_DICT_OBJECT_ID_NA;
    ellTableConstraintType  sConstType = ELL_TABLE_CONSTRAINT_TYPE_NA;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstHandle != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */

    sConstType = ellGetConstraintType( aConstHandle );
    sTableID = ellGetTableID( aTableHandle );

    /**
     * SQL-Constraint Cache 를 삭제
     */
    
    STL_TRY( eldcDeleteCacheConst( aTransID,
                                   aConstHandle,
                                   aEnv )
             == STL_SUCCESS );
    
    /**
     * Table 에서 연결 정보를 삭제
     */
    
    STL_TRY( eldcDelConstraintFromTable( aTransID,
                                         aTableHandle,
                                         aConstHandle,
                                         aEnv )
             == STL_SUCCESS );
                
    
    switch ( sConstType )
    {
        case ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY:
            {
                /**
                 * DDL Supplemental Logging 기록
                 */
                
                STL_TRY( eldWriteSuppLogTableDDL( aTransID,
                                                  sTableID,
                                                  ELL_SUPP_LOG_DROP_CONSTRAINT_PRIMARY_KEY,
                                                  aEnv )
                         == STL_SUCCESS );

                /**
                 * Column 속성이 변경되므로 Table Cache 를 재구축한다.
                 */
                STL_TRY( ellRebuildTableCache( aTransID,
                                               aStmt,
                                               aTableHandle,
                                               aEnv )
                         == STL_SUCCESS );
                break;
            }
        case ELL_TABLE_CONSTRAINT_TYPE_UNIQUE_KEY:
            {
                /**
                 * DDL Supplemental Logging 기록
                 */
                
                STL_TRY( eldWriteSuppLogTableDDL( aTransID,
                                                  sTableID,
                                                  ELL_SUPP_LOG_DROP_CONSTRAINT_UNIQUE_KEY,
                                                  aEnv )
                         == STL_SUCCESS );

                break;
            }
        case ELL_TABLE_CONSTRAINT_TYPE_FOREIGN_KEY:
            {
                /**
                 * DDL Supplemental Logging 기록
                 */
                
                STL_TRY( eldWriteSuppLogTableDDL( aTransID,
                                                  sTableID,
                                                  ELL_SUPP_LOG_DROP_CONSTRAINT_FOREIGN_KEY,
                                                  aEnv )
                         == STL_SUCCESS );

                break;
            }
        case ELL_TABLE_CONSTRAINT_TYPE_CHECK_NOT_NULL:
            {
                /**
                 * DDL Supplemental Logging 기록
                 */
                
                STL_TRY( eldWriteSuppLogTableDDL( aTransID,
                                                  sTableID,
                                                  ELL_SUPP_LOG_DROP_CONSTRAINT_CHECK_NOT_NULL,
                                                  aEnv )
                         == STL_SUCCESS );

                /**
                 * Column 속성이 변경되므로 Table Cache 를 재구축한다.
                 */
                STL_TRY( ellRebuildTableCache( aTransID,
                                               aStmt,
                                               aTableHandle,
                                               aEnv )
                         == STL_SUCCESS );
                break;
            }
        case ELL_TABLE_CONSTRAINT_TYPE_CHECK_CLAUSE:
            {
                /**
                 * DDL Supplemental Logging 기록
                 */
                
                STL_TRY( eldWriteSuppLogTableDDL( aTransID,
                                                  sTableID,
                                                  ELL_SUPP_LOG_DROP_CONSTRAINT_CHECK_CLAUSE,
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
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}



/**
 * @brief Not Null List 삭제에 대한 Cache 를 재구성한다.
 * @param[in] aTransID           Transaction ID
 * @param[in] aStmt              Statement
 * @param[in] aConstList         Check Constraint List
 * @param[in] aConstTableList    Constraint Table List
 * @param[in] aDistinctTableList 중복 제거된 Table List
 * @param[in] aEnv               Environment
 * @remarks
 * NOT NULL constraint 의 변경은 table cache 를 재구성한다.
 * table cache 가 한 번만 재구성되도록 해야 함.
 */
stlStatus ellRefineCache4DropNotNullList( smlTransId         aTransID,
                                          smlStatement     * aStmt,
                                          ellObjectList    * aConstList,
                                          ellObjectList    * aConstTableList,
                                          ellObjectList    * aDistinctTableList,
                                          ellEnv           * aEnv )
{
    ellObjectItem   * sConstItem   = NULL;
    ellDictHandle   * sConstHandle = NULL;

    ellObjectItem   * sTableItem   = NULL;
    ellDictHandle   * sTableHandle = NULL;
    stlInt64          sTableID = ELL_DICT_OBJECT_ID_NA;

    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstTableList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDistinctTableList != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Constraint List 를 순회하며, Constraint 정보만을 Refine 한다.
     */
    
    sTableItem = STL_RING_GET_FIRST_DATA( & aConstTableList->mHeadList );
    
    STL_RING_FOREACH_ENTRY( & aConstList->mHeadList, sConstItem )
    {
        sConstHandle = & sConstItem->mObjectHandle;

        STL_DASSERT( ellGetConstraintType( sConstHandle ) == ELL_TABLE_CONSTRAINT_TYPE_CHECK_NOT_NULL );
        
        sTableHandle = & sTableItem->mObjectHandle;
        sTableID = ellGetTableID( sTableHandle );

        /**
         * SQL-Constraint Cache 를 삭제
         */
        
        STL_TRY( eldcDeleteCacheConst( aTransID,
                                       sConstHandle,
                                       aEnv )
                 == STL_SUCCESS );
        
        /**
         * Table 에서 연결 정보를 삭제
         */
        
        STL_TRY( eldcDelConstraintFromTable( aTransID,
                                             sTableHandle,
                                             sConstHandle,
                                             aEnv )
                 == STL_SUCCESS );

        /**
         * DDL Supplemental Logging 기록
         */
        
        STL_TRY( eldWriteSuppLogTableDDL( aTransID,
                                          sTableID,
                                          ELL_SUPP_LOG_DROP_CONSTRAINT_CHECK_NOT_NULL,
                                          aEnv )
                 == STL_SUCCESS );
        
        sTableItem = STL_RING_GET_NEXT_DATA( & aConstTableList->mHeadList, sTableItem );
    }

    /**
     * Table Cache 를 Refine Cache 한다.
     */

    STL_RING_FOREACH_ENTRY( & aDistinctTableList->mHeadList, sTableItem )
    {
        sTableHandle = & sTableItem->mObjectHandle;
    
        /**
         * Table Cache 를 재구축한다.
         */
        
        STL_TRY( ellRebuildTableCache( aTransID,
                                       aStmt,
                                       sTableHandle,
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
 * @brief ALTER CONSTRAINT 를 위한 Cache 를 재구성
 * @param[in] aTransID          Transaction ID
 * @param[in] aStmt             Statement
 * @param[in] aTableHandle      Table Handle
 * @param[in] aConstHandle      Constraint Handle
 * @param[in] aEnv              Environment
 * @remarks
 */
stlStatus ellRefineCache4AlterConstraint( smlTransId           aTransID,
                                          smlStatement       * aStmt,
                                          ellDictHandle      * aTableHandle,
                                          ellDictHandle      * aConstHandle,
                                          ellEnv             * aEnv )
{
    /* stlInt64  sTableID = ELL_DICT_OBJECT_ID_NA; */
    /* ellTableConstraintType  sConstType = ELL_TABLE_CONSTRAINT_TYPE_NA; */
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstHandle != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */

    /* sConstType = ellGetConstraintType( aConstHandle ); */
    /* sTableID = ellGetTableID( aTableHandle ); */

    /**
     * SQL-Constraint Cache 를 삭제
     */
    
    STL_TRY( eldcDeleteCacheConst( aTransID,
                                   aConstHandle,
                                   aEnv )
             == STL_SUCCESS );
    
    /**
     * Table 에서 연결 정보를 삭제
     */
    
    STL_TRY( eldcDelConstraintFromTable( aTransID,
                                         aTableHandle,
                                         aConstHandle,
                                         aEnv )
             == STL_SUCCESS );

    /**
     * SQL-Constraint Cache 를 추가 
     */
    
    STL_TRY( eldcInsertCacheConstByConstID( aTransID,
                                            aStmt,
                                            ellGetConstraintID(aConstHandle),
                                            aEnv )
             == STL_SUCCESS );
    
    /**
     * 관련 Table 에 Constraint 정보를 등록
     */
    
    STL_TRY( eldcAddConstraintIntoTable( aTransID,
                                         ellGetConstraintID(aConstHandle),
                                         aEnv )
             == STL_SUCCESS );
    
    /**
     * @todo NOT ENFORCED, NOVALIDATE 를 지원할 경우 DDL Supplemental Logging 기록해야 함.
     */

    /**
     * @todo (PRIMARY KEY, NOT NULL) 의 NOT ENFORCED 일 경우 Column 속성이 변경되므로 Table Cache 를 재구축한다.
     */
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/**
 * @brief SQL-Table Element 에서 Child Foreign Key Element 를 제거한다.
 * @param[in] aTransID                Transaction ID
 * @param[in] aStmt                   Statement
 * @param[in] aParentTableHandle      Parent Table Dictionary Handle
 * @param[in] aChildForeignKeyHandle  Child Foreign Key Handle
 * @param[in] aEnv                    Environment
 * @remarks
 */
stlStatus ellUnlinkChildForeignKeyFromParentTable( smlTransId      aTransID,
                                                   smlStatement  * aStmt,
                                                   ellDictHandle * aParentTableHandle,
                                                   ellDictHandle * aChildForeignKeyHandle,
                                                   ellEnv        * aEnv )
{
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParentTableHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aChildForeignKeyHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( ellGetConstraintType(aChildForeignKeyHandle)
                        == ELL_TABLE_CONSTRAINT_TYPE_FOREIGN_KEY,
                        ELL_ERROR_STACK(aEnv) );
    
    /**
     * Parent Table 로부터 Child Foreign Key 의 연결을 제거한다.
     */
    
    STL_TRY( eldcDelChildForeignKeyFromTable( aTransID,
                                              aStmt,
                                              aParentTableHandle,
                                              aChildForeignKeyHandle,
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
 * @brief Schema Handle 과 Constraint Name 을 이용해 Constraint Dictionary Handle 을 획득 
 * @param[in]  aTransID           Transaction ID
 * @param[in]  aViewSCN           View SCN
 * @param[in]  aSchemaDictHandle  Schema Dictionary Handle
 * @param[in]  aConstName         Constraint Name
 * @param[out] aConstDictHandle   Constraint Dictionary Handle
 * @param[out] aExist             존재 여부 
 * @param[in]  aEnv               Envirionment 
 * @remarks
 */
stlStatus ellGetConstraintDictHandleWithSchema( smlTransId           aTransID,
                                                smlScn               aViewSCN,
                                                ellDictHandle      * aSchemaDictHandle,
                                                stlChar            * aConstName,
                                                ellDictHandle      * aConstDictHandle,
                                                stlBool            * aExist,
                                                ellEnv             * aEnv )
{
    ellSchemaDesc * sSchemaDesc = NULL;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aSchemaDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstName != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExist != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Schema Descriptor
     */

    sSchemaDesc = (ellSchemaDesc *) ellGetObjectDesc( aSchemaDictHandle );

    /**
     * Const Dictionary Handle 획득
     */
    
    ellInitDictHandle( aConstDictHandle );
    
    STL_TRY( eldGetConstHandleByName( aTransID,
                                      aViewSCN,
                                      sSchemaDesc->mSchemaID,
                                      aConstName,
                                      aConstDictHandle,
                                      aExist,
                                      aEnv )
             == STL_SUCCESS );

    if ( *aExist == STL_TRUE )
    {
        aConstDictHandle->mFullyQualifiedName = STL_TRUE;
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief Auth Handle 과 Constraint Name 을 이용해 Const Dictionary Handle 을 얻는다.
 * @param[in]  aTransID             Transaction ID
 * @param[in]  aViewSCN             View SCN
 * @param[in]  aAuthDictHandle      Authorization Dictionary Handle
 * @param[in]  aConstName           Constraint Name
 * @param[out] aConstDictHandle     Constraint Dictionary Handle
 * @param[out] aExist               존재 여부 
 * @param[in]  aEnv                 Envirionment 
 * @remarks
 */
stlStatus ellGetConstraintDictHandleWithAuth( smlTransId           aTransID,
                                              smlScn               aViewSCN,
                                              ellDictHandle      * aAuthDictHandle,
                                              stlChar            * aConstName,
                                              ellDictHandle      * aConstDictHandle,
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
    STL_PARAM_VALIDATE( aConstName != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExist != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Authorization Descriptor
     */

    sAuthDesc = (ellAuthDesc *) ellGetObjectDesc( aAuthDictHandle );

    /**
     * Constraint Dictionary Handle 을 획득
     */
    
    ellInitDictHandle( aConstDictHandle );
    
    while(1)
    {
        /**
         * 사용자 Schema Path 를 이용해 검색
         */
        
        for ( i = 0; i < sAuthDesc->mSchemaPathCnt; i++ )
        {
            STL_TRY( eldGetConstHandleByName( aTransID,
                                              aViewSCN,
                                              sAuthDesc->mSchemaIDArray[i],
                                              aConstName,
                                              aConstDictHandle,
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
                STL_TRY( eldGetConstHandleByName( aTransID,
                                                  aViewSCN,
                                                  sPublicDesc->mSchemaIDArray[i],
                                                  aConstName,
                                                  aConstDictHandle,
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
 * @brief Table 과 관련된 Primary Key 개수와 Dictionary Handle Array 를 얻는다.
 * @param[in]  aTransID                    Transaction ID
 * @param[in]  aViewSCN                    View SCN
 * @param[in]  aRegionMem                  Handle Array 를 할당받기 위한 메모리 관리자 
 * @param[in]  aTableDictHandle            Table Dictionary Handle
 * @param[out] aPrimaryKeyCnt              Primary Key Constraint 개수
 * @param[out] aPrimaryKeyDictHandleArray  Primary Key Dictionary Handle Array
 * @param[in]  aEnv                        Environment
 * @remarks
 * Primary Key 의 개수는 한개 이하이다.
 */
stlStatus ellGetTablePrimaryKeyDictHandle( smlTransId       aTransID,
                                           smlScn           aViewSCN,
                                           knlRegionMem   * aRegionMem,
                                           ellDictHandle  * aTableDictHandle,
                                           stlInt32       * aPrimaryKeyCnt,
                                           ellDictHandle ** aPrimaryKeyDictHandleArray,
                                           ellEnv         * aEnv )
{
    return eldGetTableRelatedDictHandle( aTransID,
                                         aViewSCN,
                                         aRegionMem,
                                         aTableDictHandle,
                                         ELL_TABLE_RELATED_PRIMARY_KEY,
                                         aPrimaryKeyCnt,
                                         aPrimaryKeyDictHandleArray,
                                         aEnv );
}


/**
 * @brief Table 과 관련된 Unique Key 개수와 Dictionary Handle Array 를 얻는다.
 * @param[in]  aTransID                   Transaction ID
 * @param[in]  aViewSCN                   View SCN
 * @param[in]  aRegionMem                 Handle Array 를 할당받기 위한 메모리 관리자 
 * @param[in]  aTableDictHandle           Table Dictionary Handle
 * @param[out] aUniqueKeyCnt              Unique Key Constraint 개수
 * @param[out] aUniqueKeyDictHandleArray  Unique Key Dictionary Handle Array
 * @param[in]  aEnv                       Environment
 * @remarks
 */
stlStatus ellGetTableUniqueKeyDictHandle( smlTransId       aTransID,
                                          smlScn           aViewSCN,
                                          knlRegionMem   * aRegionMem,
                                          ellDictHandle  * aTableDictHandle,
                                          stlInt32       * aUniqueKeyCnt,
                                          ellDictHandle ** aUniqueKeyDictHandleArray,
                                          ellEnv         * aEnv )
{
    return eldGetTableRelatedDictHandle( aTransID,
                                         aViewSCN,
                                         aRegionMem,
                                         aTableDictHandle,
                                         ELL_TABLE_RELATED_UNIQUE_KEY,
                                         aUniqueKeyCnt,
                                         aUniqueKeyDictHandleArray,
                                         aEnv );
}


/**
 * @brief Table 과 관련된 Foreign Key 개수와 Dictionary Handle Array 를 얻는다.
 * @param[in]  aTransID                    Transaction ID
 * @param[in]  aViewSCN                    View SCN
 * @param[in]  aRegionMem                  Handle Array 를 할당받기 위한 메모리 관리자 
 * @param[in]  aTableDictHandle            Table Dictionary Handle
 * @param[out] aForeignKeyCnt              Foreign Key Constraint 개수
 * @param[out] aForeignKeyDictHandleArray  Foreign Key Dictionary Handle Array
 * @param[in]  aEnv                        Environment
 * @remarks
 */
stlStatus ellGetTableForeignKeyDictHandle( smlTransId       aTransID,
                                           smlScn           aViewSCN,
                                           knlRegionMem   * aRegionMem,
                                           ellDictHandle  * aTableDictHandle,
                                           stlInt32       * aForeignKeyCnt,
                                           ellDictHandle ** aForeignKeyDictHandleArray,
                                           ellEnv         * aEnv )
{
    return eldGetTableRelatedDictHandle( aTransID,
                                         aViewSCN,
                                         aRegionMem,
                                         aTableDictHandle,
                                         ELL_TABLE_RELATED_FOREIGN_KEY,
                                         aForeignKeyCnt,
                                         aForeignKeyDictHandleArray,
                                         aEnv );
}



/**
 * @brief Table 을 참조하고 있는 Child Foreign Key 개수와 Dictionary Handle Array 를 얻는다.
 * @param[in]  aTransID                Transaction ID
 * @param[in]  aViewSCN                View SCN
 * @param[in]  aRegionMem              Handle Array 를 할당받기 위한 메모리 관리자 
 * @param[in]  aTableDictHandle        Table Dictionary Handle
 * @param[out] aChildForeignKeyCnt     Child Foreign Key Constraint 개수
 * @param[out] aChildForeignKeyArray   Child Foreign Key Dictionary Handle Array
 * @param[in]  aEnv                    Environment
 * @remarks
 */
stlStatus ellGetTableChildForeignKeyDictHandle( smlTransId       aTransID,
                                                smlScn           aViewSCN,
                                                knlRegionMem   * aRegionMem,
                                                ellDictHandle  * aTableDictHandle,
                                                stlInt32       * aChildForeignKeyCnt,
                                                ellDictHandle ** aChildForeignKeyArray,
                                                ellEnv         * aEnv )
{
    return eldGetTableRelatedDictHandle( aTransID,
                                         aViewSCN,
                                         aRegionMem,
                                         aTableDictHandle,
                                         ELL_TABLE_RELATED_CHILD_FOREIGN_KEY,
                                         aChildForeignKeyCnt,
                                         aChildForeignKeyArray,
                                         aEnv );
}


/**
 * @brief Table 의 Check Not Null Constraint 개수와 Dictionary Handle Array 를 얻는다.
 * @param[in]  aTransID                      Transaction ID
 * @param[in]  aViewSCN                      View SCN
 * @param[in]  aRegionMem                    Handle Array 를 할당받기 위한 메모리 관리자 
 * @param[in]  aTableDictHandle              Table Dictionary Handle
 * @param[out] aCheckNotNullCnt              Check Not Null Constraint 개수
 * @param[out] aCheckNotNullDictHandleArray  Check Not Null Dictionary Handle Array
 * @param[in]  aEnv                          Environment
 * @remarks
 */
stlStatus ellGetTableCheckNotNullDictHandle( smlTransId       aTransID,
                                             smlScn           aViewSCN,
                                             knlRegionMem   * aRegionMem,
                                             ellDictHandle  * aTableDictHandle,
                                             stlInt32       * aCheckNotNullCnt,
                                             ellDictHandle ** aCheckNotNullDictHandleArray,
                                             ellEnv         * aEnv )
{
    return eldGetTableRelatedDictHandle( aTransID,
                                         aViewSCN,
                                         aRegionMem,
                                         aTableDictHandle,
                                         ELL_TABLE_RELATED_CHECK_NOT_NULL,
                                         aCheckNotNullCnt,
                                         aCheckNotNullDictHandleArray,
                                         aEnv );
}


/**
 * @brief Table 의 Check Not Null Constraint 개수와 Dictionary Handle Array 를 얻는다.
 * @param[in]  aTransID                      Transaction ID
 * @param[in]  aViewSCN                      View SCN
 * @param[in]  aRegionMem                    Handle Array 를 할당받기 위한 메모리 관리자 
 * @param[in]  aTableDictHandle              Table Dictionary Handle
 * @param[out] aCheckClauseCnt               Check Clause Constraint 개수
 * @param[out] aCheckClauseDictHandleArray   Check Clause Dictionary Handle Array
 * @param[in]  aEnv                          Environment
 * @remarks
 */
stlStatus ellGetTableCheckClauseDictHandle( smlTransId       aTransID,
                                            smlScn           aViewSCN,
                                            knlRegionMem   * aRegionMem,
                                            ellDictHandle  * aTableDictHandle,
                                            stlInt32       * aCheckClauseCnt,
                                            ellDictHandle ** aCheckClauseDictHandleArray,
                                            ellEnv         * aEnv )
{
    return eldGetTableRelatedDictHandle( aTransID,
                                         aViewSCN,
                                         aRegionMem,
                                         aTableDictHandle,
                                         ELL_TABLE_RELATED_CHECK_CLAUSE,
                                         aCheckClauseCnt,
                                         aCheckClauseDictHandleArray,
                                         aEnv );
}



/*********************************************************
 * 정보 생성 함수
 *********************************************************/


/**
 * @brief 임의의 Priamry Key Name 을 생성
 * @param[in]  aNameBuffer      이름을 생성할 Buffer
 * @param[in]  aNameBufferSize  Name Buffer 의 크기
 *                         <BR> STL_MAX_SQL_IDENTIFIER_LENGTH 보다 커야 함.
 * @param[in]  aTableName       Table Name
 * @remarks
 * 생성되는 이름 : aTableName || '_PRIMARY_KEY' 
 */
void ellMakePrimaryKeyName( stlChar  * aNameBuffer,
                            stlInt32   aNameBufferSize,
                            stlChar  * aTableName )
{
    /*
     * Parameter Validation
     */

    STL_ASSERT( aNameBuffer != NULL );
    STL_ASSERT( aNameBufferSize > STL_MAX_SQL_IDENTIFIER_LENGTH );
    STL_ASSERT( aTableName != NULL );

    /**
     * Primary Key Name 을 생성
     */
    
    stlSnprintf( aNameBuffer,
                 aNameBufferSize,
                 "%s_PRIMARY_KEY",
                 aTableName );
}


/**
 * @brief 임의의 Unique Key Name 을 생성
 * @param[in]  aNameBuffer      이름을 생성할 Buffer
 * @param[in]  aNameBufferSize  Name Buffer 의 크기
 *                         <BR> STL_MAX_SQL_IDENTIFIER_LENGTH 보다 커야 함.
 * @param[in]  aTableName       Table Name
 * @param[in]  aColumnNameList  Column Name List
 * @remarks
 * 생성되는 이름 : aTableName || '_UNIQUE' || '_' column name || '_' column name ...
 */
void ellMakeUniqueKeyName( stlChar     * aNameBuffer,
                           stlInt32      aNameBufferSize,
                           stlChar     * aTableName,
                           ellNameList * aColumnNameList )
{
    ellNameList * sNameList = NULL;

    /*
     * Parameter Validation
     */

    STL_ASSERT( aNameBuffer != NULL );
    STL_ASSERT( aNameBufferSize > STL_MAX_SQL_IDENTIFIER_LENGTH );
    STL_ASSERT( aTableName != NULL );
    STL_ASSERT( aColumnNameList != NULL );

    /**
     * Unique Key Name 을 생성
     */
    
    stlSnprintf( aNameBuffer,
                 aNameBufferSize,
                 "%s_UNIQUE",
                 aTableName );

    for ( sNameList = aColumnNameList; sNameList != NULL; sNameList = sNameList->mNext )
    {
        stlSnprintf( aNameBuffer,
                     aNameBufferSize,
                     "%s_%s",
                     aNameBuffer,
                     sNameList->mName );
    }
}


/**
 * @brief 임의의 Foreign Key Name 을 생성
 * @param[in]  aNameBuffer      이름을 생성할 Buffer
 * @param[in]  aNameBufferSize  Name Buffer 의 크기
 *                         <BR> STL_MAX_SQL_IDENTIFIER_LENGTH 보다 커야 함.
 * @param[in]  aMyTableName     My Table Name
 * @param[in]  aRefTableName    참조하는 Table Name
 * @remarks
 * 생성되는 이름 : aMyTableName || '_FOREIGN_KEY_' || aRefTableName
 */
void ellMakeForeignKeyName( stlChar     * aNameBuffer,
                            stlInt32      aNameBufferSize,
                            stlChar     * aMyTableName,
                            stlChar     * aRefTableName )
{
    /*
     * Parameter Validation
     */

    STL_ASSERT( aNameBuffer != NULL );
    STL_ASSERT( aNameBufferSize > STL_MAX_SQL_IDENTIFIER_LENGTH );
    STL_ASSERT( aMyTableName != NULL );
    STL_ASSERT( aRefTableName != NULL );

    /**
     * Foreign Key Name 을 생성
     */
    
    stlSnprintf( aNameBuffer,
                 aNameBufferSize,
                 "%s_FOREIGN_KEY_%s",
                 aMyTableName,
                 aRefTableName );
}



/**
 * @brief 임의의 Check Not Null Name 을 생성
 * @param[in]  aNameBuffer      이름을 생성할 Buffer
 * @param[in]  aNameBufferSize  Name Buffer 의 크기
 *                         <BR> STL_MAX_SQL_IDENTIFIER_LENGTH 보다 커야 함.
 * @param[in]  aTableName       Table Name
 * @param[in]  aColumnName      Column Name
 * @remarks
 * 생성되는 이름 : aTableName || '_' aColumnName || '_NOT_NULL'
 */
void ellMakeCheckNotNullName( stlChar     * aNameBuffer,
                              stlInt32      aNameBufferSize,
                              stlChar     * aTableName,
                              stlChar     * aColumnName )
{
    /*
     * Parameter Validation
     */

    STL_ASSERT( aNameBuffer != NULL );
    STL_ASSERT( aNameBufferSize > STL_MAX_SQL_IDENTIFIER_LENGTH );
    STL_ASSERT( aTableName != NULL );

    /**
     * Check Not Null Name 을 생성
     */
    
    stlSnprintf( aNameBuffer,
                 aNameBufferSize,
                 "%s_%s_NOT_NULL",
                 aTableName,
                 aColumnName );
}



/**
 * @brief 임의의 Check Clause Name 을 생성
 * @param[in]  aNameBuffer      이름을 생성할 Buffer
 * @param[in]  aNameBufferSize  Name Buffer 의 크기
 *                         <BR> STL_MAX_SQL_IDENTIFIER_LENGTH 보다 커야 함.
 * @param[in]  aTableName       Table Name
 * @param[in]  aColumnNameList  Column Name List (nullable)
 * @remarks
 * 생성되는 이름 : aTableName || '_CHECK' || '_' column name || '_' column name ...
 */
void ellMakeCheckClauseName( stlChar     * aNameBuffer,
                             stlInt32      aNameBufferSize,
                             stlChar     * aTableName,
                             ellNameList * aColumnNameList )
{
    ellNameList * sNameList = NULL;

    /*
     * Parameter Validation
     */

    STL_ASSERT( aNameBuffer != NULL );
    STL_ASSERT( aNameBufferSize > STL_MAX_SQL_IDENTIFIER_LENGTH );
    STL_ASSERT( aTableName != NULL );

    /**
     * Check Clause Name 을 생성
     */
    
    stlSnprintf( aNameBuffer,
                 aNameBufferSize,
                 "%s_CHECK",
                 aTableName );

    for ( sNameList = aColumnNameList; sNameList != NULL; sNameList = sNameList->mNext )
    {
        stlSnprintf( aNameBuffer,
                     aNameBufferSize,
                     "%s_%s",
                     aNameBuffer,
                     sNameList->mName );
    }
}


/**
 * @brief Key Constraint에 대한 임의의 Index Name 을 생성
 * @param[in]  aNameBuffer        이름을 생성할 Buffer
 * @param[in]  aNameBufferSize    Name Buffer 의 크기
 *                           <BR> STL_MAX_SQL_IDENTIFIER_LENGTH 보다 커야 함.
 * @param[in]  aKeyConstraintName Key Constraint Name
 * @remarks
 */
void ellMakeKeyIndexName( stlChar     * aNameBuffer,
                          stlInt32      aNameBufferSize,
                          stlChar     * aKeyConstraintName )
{
    /*
     * Parameter Validation
     */

    STL_ASSERT( aNameBuffer != NULL );
    STL_ASSERT( aNameBufferSize > STL_MAX_SQL_IDENTIFIER_LENGTH );
    STL_ASSERT( aKeyConstraintName != NULL );

    /**
     * Index Name 을 생성
     */
    
    stlSnprintf( aNameBuffer,
                 aNameBufferSize,
                 "%s_INDEX",
                 aKeyConstraintName );
}



/*********************************************************
 * 정보 획득 함수
 *********************************************************/



/**
 * @brief Constraint ID 정보 
 * @param[in] aConstDictHandle   Table Constraint Dictionary Handle
 * @return Constraint ID
 * @remarks
 */
stlInt64 ellGetConstraintID( ellDictHandle * aConstDictHandle )
{
    ellTableConstDesc * sConstDesc = NULL;

    STL_DASSERT( aConstDictHandle->mObjectType == ELL_OBJECT_CONSTRAINT );
    
    sConstDesc = (ellTableConstDesc *) ellGetObjectDesc( aConstDictHandle );

    return sConstDesc->mConstID;
}


/**
 * @brief Constraint Name 정보 
 * @param[in] aConstDictHandle   Table Constraint Dictionary Handle
 * @return Constraint Name
 * @remarks
 */
stlChar * ellGetConstraintName( ellDictHandle * aConstDictHandle )
{
    ellTableConstDesc * sConstDesc = NULL;

    STL_DASSERT( aConstDictHandle->mObjectType == ELL_OBJECT_CONSTRAINT );
    
    sConstDesc = (ellTableConstDesc *) ellGetObjectDesc( aConstDictHandle );

    return sConstDesc->mConstName;
}


/**
 * @brief Constraint Owner ID 정보 
 * @param[in] aConstDictHandle   Table Constraint Dictionary Handle
 * @return Constraint Owner ID
 * @remarks
 */
stlInt64 ellGetConstraintOwnerID( ellDictHandle * aConstDictHandle )
{
    ellTableConstDesc * sConstDesc = NULL;

    STL_DASSERT( aConstDictHandle->mObjectType == ELL_OBJECT_CONSTRAINT );
    
    sConstDesc = (ellTableConstDesc *) ellGetObjectDesc( aConstDictHandle );

    return sConstDesc->mConstOwnerID;
}

/**
 * @brief Constraint 가 속한 Schema ID 정보 
 * @param[in] aConstDictHandle   Table Constraint Dictionary Handle
 * @return Constraint 의 Schema ID
 * @remarks
 * Constraint 의 Schema ID 와 Constraint 가 걸린 Table 의 Schema ID 는 다를 수 있다.
 */
stlInt64 ellGetConstraintSchemaID( ellDictHandle * aConstDictHandle )
{
    ellTableConstDesc * sConstDesc = NULL;

    STL_DASSERT( aConstDictHandle->mObjectType == ELL_OBJECT_CONSTRAINT );
    
    sConstDesc = (ellTableConstDesc *) ellGetObjectDesc( aConstDictHandle );

    return sConstDesc->mConstSchemaID;
}


/**
 * @brief Constraint 가 걸려있는 Table 의 Owner ID 정보 
 * @param[in] aConstDictHandle   Table Constraint Dictionary Handle
 * @return Constraint Table 의 Schema ID
 * @remarks
 * Constraint 의 Owner ID 와 Constraint 가 걸린 Table 의 Owner ID 는 다를 수 있다.
 */
stlInt64 ellGetConstraintTableOwnerID( ellDictHandle * aConstDictHandle )
{
    ellTableConstDesc * sConstDesc = NULL;

    STL_DASSERT( aConstDictHandle->mObjectType == ELL_OBJECT_CONSTRAINT );
    
    sConstDesc = (ellTableConstDesc *) ellGetObjectDesc( aConstDictHandle );

    return sConstDesc->mTableOwnerID;
}


/**
 * @brief Constraint 가 걸려있는 Table 의 Schema ID 정보 
 * @param[in] aConstDictHandle   Table Constraint Dictionary Handle
 * @return Constraint Table 의 Schema ID
 * @remarks
 * Constraint 의 Schema ID 와 Constraint 가 걸린 Table 의 Schema ID 는 다를 수 있다.
 */
stlInt64 ellGetConstraintTableSchemaID( ellDictHandle * aConstDictHandle )
{
    ellTableConstDesc * sConstDesc = NULL;

    STL_DASSERT( aConstDictHandle->mObjectType == ELL_OBJECT_CONSTRAINT );
    
    sConstDesc = (ellTableConstDesc *) ellGetObjectDesc( aConstDictHandle );

    return sConstDesc->mTableSchemaID;
}


/**
 * @brief Constraint 가 걸려있는 Table ID 정보 
 * @param[in] aConstDictHandle   Table Constraint Dictionary Handle
 * @return Constraint Table ID
 * @remarks
 * Constraint 의 Schema ID 와 Constraint 가 걸린 Table 의 Schema ID 는 다를 수 있다.
 */
stlInt64 ellGetConstraintTableID( ellDictHandle * aConstDictHandle )
{
    ellTableConstDesc * sConstDesc = NULL;

    STL_DASSERT( aConstDictHandle->mObjectType == ELL_OBJECT_CONSTRAINT );
    
    sConstDesc = (ellTableConstDesc *) ellGetObjectDesc( aConstDictHandle );

    return sConstDesc->mTableID;
}


/**
 * @brief Constraint Type 정보 
 * @param[in] aConstDictHandle   Table Constraint Dictionary Handle
 * @return Constraint Type (ellTableConstraintType)
 * @remarks
 */
ellTableConstraintType ellGetConstraintType( ellDictHandle * aConstDictHandle )
{
    ellTableConstDesc * sConstDesc = NULL;

    STL_DASSERT( aConstDictHandle->mObjectType == ELL_OBJECT_CONSTRAINT );
    
    sConstDesc = (ellTableConstDesc *) ellGetObjectDesc( aConstDictHandle );

    return sConstDesc->mConstType;
}


/**
 * @brief Constraint 의 DEFERRABLE or NOT DEFERRABLE
 * @param[in] aConstDictHandle   Table Constraint Dictionary Handle
 * @return boolean
 * @remarks
 */
stlBool  ellGetConstraintAttrIsDeferrable( ellDictHandle * aConstDictHandle )
{
    ellTableConstDesc * sConstDesc = NULL;

    STL_DASSERT( aConstDictHandle->mObjectType == ELL_OBJECT_CONSTRAINT );
    
    sConstDesc = (ellTableConstDesc *) ellGetObjectDesc( aConstDictHandle );

    return sConstDesc->mDeferrable;
}

/**
 * @brief Constraint 의 INITIALLY DEFERRED or INITIALLY IMMEDIATE
 * @param[in] aConstDictHandle   Table Constraint Dictionary Handle
 * @return boolean
 * @remarks
 */
stlBool  ellGetConstraintAttrIsInitDeferred( ellDictHandle * aConstDictHandle )
{
    ellTableConstDesc * sConstDesc = NULL;

    STL_DASSERT( aConstDictHandle->mObjectType == ELL_OBJECT_CONSTRAINT );
    
    sConstDesc = (ellTableConstDesc *) ellGetObjectDesc( aConstDictHandle );

    return sConstDesc->mInitDeferred;
}


/**
 * @brief Constraint 의 ENFORCED or NOT ENFORCED
 * @param[in] aConstDictHandle   Table Constraint Dictionary Handle
 * @return boolean
 * @remarks
 */
stlBool  ellGetConstraintAttrIsEnforced( ellDictHandle * aConstDictHandle )
{
    ellTableConstDesc * sConstDesc = NULL;

    STL_DASSERT( aConstDictHandle->mObjectType == ELL_OBJECT_CONSTRAINT );
    
    sConstDesc = (ellTableConstDesc *) ellGetObjectDesc( aConstDictHandle );

    return sConstDesc->mEnforced;
}



/**
 * @brief Constraint 의 VALIDATE or NOVALIDATE
 * @param[in] aConstDictHandle   Table Constraint Dictionary Handle
 * @return boolean
 * @remarks
 */
stlBool  ellGetConstraintAttrIsValidate( ellDictHandle * aConstDictHandle )
{
    ellTableConstDesc * sConstDesc = NULL;

    STL_DASSERT( aConstDictHandle->mObjectType == ELL_OBJECT_CONSTRAINT );
    
    sConstDesc = (ellTableConstDesc *) ellGetObjectDesc( aConstDictHandle );

    return sConstDesc->mValidate;
}


/**
 * @brief Key Constraint 를 위한 Index Dictionary Handle 을 획득
 * @param[in]  aConstDictHandle Constraint Dictionary Handle
 * @return
 * Index Dictionary Handle
 * @remarks
 */
ellDictHandle * ellGetConstraintIndexDictHandle( ellDictHandle * aConstDictHandle )
{
    ellTableConstDesc * sConstDesc = NULL;
    ellDictHandle     * sIndexHandle = NULL;
    
    STL_DASSERT( aConstDictHandle->mObjectType == ELL_OBJECT_CONSTRAINT );
    
    /**
     * Key Constraint 의 Index Dictionary Handle 을 얻는다.
     */
    
    sConstDesc = (ellTableConstDesc *) ellGetObjectDesc( aConstDictHandle );
    
    switch ( sConstDesc->mConstType )
    {
        case ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY:
        case ELL_TABLE_CONSTRAINT_TYPE_UNIQUE_KEY:
        case ELL_TABLE_CONSTRAINT_TYPE_FOREIGN_KEY:
            {
                sIndexHandle = & sConstDesc->mKeyIndexHandle;
                break;
            }
        default:
            {
                sIndexHandle = NULL;
                break;
            }
    }

    return sIndexHandle;
}

/**
 * @brief Primary Key Constraint 의 Key Column Count 
 * @param[in] aPrimaryKeyHandle   Primary Key Dictionary Handle
 * @return Key Column Count (0 이라면, 구현 오류임)
 * @remarks
 */
stlInt32   ellGetPrimaryKeyColumnCount( ellDictHandle * aPrimaryKeyHandle )
{
    stlInt32  sKeyCnt = 0;
    
    ellTableConstDesc    * sConstDesc = NULL;
    ellPrimaryKeyDepDesc * sPrimaryKeyDesc = NULL;
    
    STL_DASSERT( aPrimaryKeyHandle->mObjectType == ELL_OBJECT_CONSTRAINT );
    
    sConstDesc = (ellTableConstDesc *) ellGetObjectDesc( aPrimaryKeyHandle );

    sPrimaryKeyDesc = (ellPrimaryKeyDepDesc *) sConstDesc->mConstTypeDepDesc;
    
    if ( sConstDesc->mConstType == ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY )
    {
        sKeyCnt = sPrimaryKeyDesc->mKeyColumnCnt;
    }
    else
    {
        sKeyCnt = 0;
    }

    return sKeyCnt;
}


/**
 * @brief Primary Key Constraint 의 Key Column Handle Array
 * @param[in] aPrimaryKeyHandle   Primary Key Dictionary Handle
 * @return Key Column Handle Array Pointer (NULL 이라면, 구현 오류임)
 * @remarks
 */
ellDictHandle * ellGetPrimaryKeyColumnHandle( ellDictHandle * aPrimaryKeyHandle )
{
    ellDictHandle * sKeyColumnHandle = NULL;
    
    ellTableConstDesc    * sConstDesc = NULL;
    ellPrimaryKeyDepDesc * sPrimaryKeyDesc = NULL;
    
    STL_DASSERT( aPrimaryKeyHandle->mObjectType == ELL_OBJECT_CONSTRAINT );
    
    sConstDesc = (ellTableConstDesc *) ellGetObjectDesc( aPrimaryKeyHandle );

    sPrimaryKeyDesc = (ellPrimaryKeyDepDesc *) sConstDesc->mConstTypeDepDesc;
    
    if ( sConstDesc->mConstType == ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY )
    {
        sKeyColumnHandle = sPrimaryKeyDesc->mKeyColumnHandle;
    }
    else
    {
        sKeyColumnHandle = NULL;
    }

    return sKeyColumnHandle;
}




/**
 * @brief Unique Key Constraint 의 Key Column Count 
 * @param[in] aUniqueKeyHandle   Unique Key Dictionary Handle
 * @return Key Column Count (0 이라면, 구현 오류임)
 * @remarks
 */
stlInt32   ellGetUniqueKeyColumnCount( ellDictHandle * aUniqueKeyHandle )
{
    stlInt32  sKeyCnt = 0;
    
    ellTableConstDesc   * sConstDesc = NULL;
    ellUniqueKeyDepDesc * sUniqueKeyDesc = NULL;
    
    STL_DASSERT( aUniqueKeyHandle->mObjectType == ELL_OBJECT_CONSTRAINT );
    
    sConstDesc = (ellTableConstDesc *) ellGetObjectDesc( aUniqueKeyHandle );

    sUniqueKeyDesc = (ellUniqueKeyDepDesc *) sConstDesc->mConstTypeDepDesc;
    
    if ( sConstDesc->mConstType == ELL_TABLE_CONSTRAINT_TYPE_UNIQUE_KEY )
    {
        sKeyCnt = sUniqueKeyDesc->mKeyColumnCnt;
    }
    else
    {
        sKeyCnt = 0;
    }

    return sKeyCnt;
}


/**
 * @brief Unique Key Constraint 의 Key Column Handle Array
 * @param[in] aUniqueKeyHandle   Unique Key Dictionary Handle
 * @return Key Column Handle Array Pointer (NULL 이라면, 구현 오류임)
 * @remarks
 */
ellDictHandle * ellGetUniqueKeyColumnHandle( ellDictHandle * aUniqueKeyHandle )
{
    ellDictHandle * sKeyColumnHandle = NULL;
    
    ellTableConstDesc   * sConstDesc = NULL;
    ellUniqueKeyDepDesc * sUniqueKeyDesc = NULL;
    
    STL_DASSERT( aUniqueKeyHandle->mObjectType == ELL_OBJECT_CONSTRAINT );
    
    sConstDesc = (ellTableConstDesc *) ellGetObjectDesc( aUniqueKeyHandle );

    sUniqueKeyDesc = (ellUniqueKeyDepDesc *) sConstDesc->mConstTypeDepDesc;
    
    if ( sConstDesc->mConstType == ELL_TABLE_CONSTRAINT_TYPE_UNIQUE_KEY )
    {
        sKeyColumnHandle = sUniqueKeyDesc->mKeyColumnHandle;
    }
    else
    {
        sKeyColumnHandle = NULL;
    }

    return sKeyColumnHandle;
}



/**
 * @brief Foreign Key Constraint 의 Key Column Count 
 * @param[in] aForeignKeyHandle   Foreign Key Dictionary Handle
 * @return Key Column Count (0 이라면, 구현 오류임)
 * @remarks
 */
stlInt32   ellGetForeignKeyColumnCount( ellDictHandle * aForeignKeyHandle )
{
    stlInt32  sKeyCnt = 0;
    
    ellTableConstDesc    * sConstDesc = NULL;
    ellForeignKeyDepDesc * sForeignKeyDesc = NULL;
    
    STL_DASSERT( aForeignKeyHandle->mObjectType == ELL_OBJECT_CONSTRAINT );
    
    sConstDesc = (ellTableConstDesc *) ellGetObjectDesc( aForeignKeyHandle );

    sForeignKeyDesc = (ellForeignKeyDepDesc *) sConstDesc->mConstTypeDepDesc;
    
    if ( sConstDesc->mConstType == ELL_TABLE_CONSTRAINT_TYPE_FOREIGN_KEY )
    {
        sKeyCnt = sForeignKeyDesc->mKeyColumnCnt;
    }
    else
    {
        sKeyCnt = 0;
    }

    return sKeyCnt;
}


/**
 * @brief Foreign Key Constraint 의 Key Column Handle Array
 * @param[in] aForeignKeyHandle   Foreign Key Dictionary Handle
 * @return Key Column Handle Array Pointer (NULL 이라면, 구현 오류임)
 * @remarks
 */
ellDictHandle * ellGetForeignKeyColumnHandle( ellDictHandle * aForeignKeyHandle )
{
    ellDictHandle * sKeyColumnHandle = NULL;
    
    ellTableConstDesc    * sConstDesc = NULL;
    ellForeignKeyDepDesc * sForeignKeyDesc = NULL;
    
    STL_DASSERT( aForeignKeyHandle->mObjectType == ELL_OBJECT_CONSTRAINT );
    
    sConstDesc = (ellTableConstDesc *) ellGetObjectDesc( aForeignKeyHandle );

    sForeignKeyDesc = (ellForeignKeyDepDesc *) sConstDesc->mConstTypeDepDesc;
    
    if ( sConstDesc->mConstType == ELL_TABLE_CONSTRAINT_TYPE_FOREIGN_KEY )
    {
        sKeyColumnHandle = sForeignKeyDesc->mKeyColumnHandle;
    }
    else
    {
        sKeyColumnHandle = NULL;
    }

    return sKeyColumnHandle;
}


/**
 * @brief Foreign Key Column 들의 Parent Key 내에서의 위치
 * @param[in] aForeignKeyHandle   Foreign Key Dictionary Handle
 * @return Foreign Key Column Idx Array Pointer In Parent Key(NULL 이라면, 구현 오류임)
 * @remarks
 * Match Idx 는 다음과 같은 의미를 갖는다.
 * - Child Foreign Key(i2, i3)
 * - Parent Unique Key(c2, c3)
 * - T1(i2, I3) REFERENCES Unique Key(c3, c2)
 * 위의 경우 Match Idx 는 다음과 같은 값을 갖는다 [1][0]
 */
stlInt32  * ellGetForeignKeyMatchIdxInParent( ellDictHandle * aForeignKeyHandle )
{
    stlInt32 * sKeyPtr = NULL;
    
    ellTableConstDesc    * sConstDesc = NULL;
    ellForeignKeyDepDesc * sForeignKeyDesc = NULL;
    
    STL_DASSERT( aForeignKeyHandle->mObjectType == ELL_OBJECT_CONSTRAINT );
    
    sConstDesc = (ellTableConstDesc *) ellGetObjectDesc( aForeignKeyHandle );

    sForeignKeyDesc = (ellForeignKeyDepDesc *) sConstDesc->mConstTypeDepDesc;
    
    if ( sConstDesc->mConstType == ELL_TABLE_CONSTRAINT_TYPE_FOREIGN_KEY )
    {
        sKeyPtr = sForeignKeyDesc->mKeyMatchIdxInParent;
    }
    else
    {
        sKeyPtr = NULL;
    }

    return sKeyPtr;
}



/**
 * @brief Foreign Key Constraint 의 Match Option
 * @param[in] aForeignKeyHandle   Foreign Key Dictionary Handle
 * @return
 * ellReferentialMatchOption (ELL_REFERENTIAL_MATCH_OPTION_NA 라면, 구현오류임)
 * @remarks
 */
ellReferentialMatchOption ellGetForeignKeyMatchOption( ellDictHandle * aForeignKeyHandle )
{
    ellReferentialMatchOption  sMatchOption = ELL_REFERENTIAL_MATCH_OPTION_NA;
    
    ellTableConstDesc    * sConstDesc = NULL;
    ellForeignKeyDepDesc * sForeignKeyDesc = NULL;
    
    STL_DASSERT( aForeignKeyHandle->mObjectType == ELL_OBJECT_CONSTRAINT );
    
    sConstDesc = (ellTableConstDesc *) ellGetObjectDesc( aForeignKeyHandle );

    sForeignKeyDesc = (ellForeignKeyDepDesc *) sConstDesc->mConstTypeDepDesc;
    
    if ( sConstDesc->mConstType == ELL_TABLE_CONSTRAINT_TYPE_FOREIGN_KEY )
    {
        sMatchOption = sForeignKeyDesc->mMatchOption;
    }
    else
    {
        sMatchOption = ELL_REFERENTIAL_MATCH_OPTION_NA;
    }

    return sMatchOption;
}



/**
 * @brief Foreign Key Constraint 의 Update Action Rule
 * @param[in] aForeignKeyHandle   Foreign Key Dictionary Handle
 * @return
 * ellReferentialActionRule (ELL_REFERENTIAL_ACTION_RULE_NA 라면, 구현오류임)
 * @remarks
 */
ellReferentialActionRule ellGetForeignKeyUpdateRule( ellDictHandle * aForeignKeyHandle )
{
    ellReferentialActionRule  sActionRule = ELL_REFERENTIAL_ACTION_RULE_NA;
    
    ellTableConstDesc    * sConstDesc = NULL;
    ellForeignKeyDepDesc * sForeignKeyDesc = NULL;
    
    STL_DASSERT( aForeignKeyHandle->mObjectType == ELL_OBJECT_CONSTRAINT );
    
    sConstDesc = (ellTableConstDesc *) ellGetObjectDesc( aForeignKeyHandle );

    sForeignKeyDesc = (ellForeignKeyDepDesc *) sConstDesc->mConstTypeDepDesc;
    
    if ( sConstDesc->mConstType == ELL_TABLE_CONSTRAINT_TYPE_FOREIGN_KEY )
    {
        sActionRule = sForeignKeyDesc->mUpdateRule;
    }
    else
    {
        sActionRule = ELL_REFERENTIAL_ACTION_RULE_NA;
    }

    return sActionRule;
}


/**
 * @brief Foreign Key Constraint 의 Delete Action Rule
 * @param[in] aForeignKeyHandle   Foreign Key Dictionary Handle
 * @return
 * ellReferentialActionRule (ELL_REFERENTIAL_ACTION_RULE_NA 라면, 구현오류임)
 * @remarks
 */
ellReferentialActionRule ellGetForeignKeyDeleteRule( ellDictHandle * aForeignKeyHandle )
{
    ellReferentialActionRule  sActionRule = ELL_REFERENTIAL_ACTION_RULE_NA;
    
    ellTableConstDesc    * sConstDesc = NULL;
    ellForeignKeyDepDesc * sForeignKeyDesc = NULL;
    
    STL_DASSERT( aForeignKeyHandle->mObjectType == ELL_OBJECT_CONSTRAINT );
    
    sConstDesc = (ellTableConstDesc *) ellGetObjectDesc( aForeignKeyHandle );

    sForeignKeyDesc = (ellForeignKeyDepDesc *) sConstDesc->mConstTypeDepDesc;
    
    if ( sConstDesc->mConstType == ELL_TABLE_CONSTRAINT_TYPE_FOREIGN_KEY )
    {
        sActionRule = sForeignKeyDesc->mDeleteRule;
    }
    else
    {
        sActionRule = ELL_REFERENTIAL_ACTION_RULE_NA;
    }

    return sActionRule;
}


/**
 * @brief Foreign Key Constraint 가 참조하고 있는
 *        Parent Unique Key(Primary Key) Dictionary Handle 을 획득
 * @param[in]  aForeignKeyHandle Foreign Key Dictionary Handle
 * @return
 * Parent Key Dictionary Handle
 * @remarks
 * Parent Unique Key Handle 은 Primary Key Handle 이거나 Unique Key Handle 이다.
 */
ellDictHandle * ellGetForeignKeyParentUniqueKeyHandle( ellDictHandle * aForeignKeyHandle )
{
    ellDictHandle * sKeyHandle = NULL;
    
    ellTableConstDesc    * sConstDesc = NULL;
    ellForeignKeyDepDesc * sForeignKeyDesc = NULL;

    STL_DASSERT( aForeignKeyHandle->mObjectType == ELL_OBJECT_CONSTRAINT );
    
    /**
     * Foreign Key Constraint 가 참조하는 Parent Key Constraint 를 얻는다.
     */
    
    sConstDesc = (ellTableConstDesc *) ellGetObjectDesc( aForeignKeyHandle );
    sForeignKeyDesc = (ellForeignKeyDepDesc *) sConstDesc->mConstTypeDepDesc;

    if ( sConstDesc->mConstType == ELL_TABLE_CONSTRAINT_TYPE_FOREIGN_KEY )
    {
        sKeyHandle = & sForeignKeyDesc->mParentUniqueKeyHandle;
    }
    else
    {
        sKeyHandle = NULL;
    }

    return sKeyHandle;
}



/**
 * @brief Check Not Null Constraint 의 Column Handle
 * @param[in] aCheckNotNullHandle   Check Not Null Dictionary Handle
 * @return Column Handle (NULL 이라면, 구현오류임)
 * @remarks
 */
ellDictHandle * ellGetCheckNotNullColumnHandle( ellDictHandle * aCheckNotNullHandle )
{
    ellDictHandle * sColumnHandle = NULL;
    
    ellTableConstDesc      * sConstDesc = NULL;
    ellCheckNotNullDepDesc * sCheckNotNullDesc = NULL;
    
    STL_DASSERT( aCheckNotNullHandle->mObjectType == ELL_OBJECT_CONSTRAINT );
    
    sConstDesc = (ellTableConstDesc *) ellGetObjectDesc( aCheckNotNullHandle );

    sCheckNotNullDesc = (ellCheckNotNullDepDesc *) sConstDesc->mConstTypeDepDesc;
    
    if ( sConstDesc->mConstType == ELL_TABLE_CONSTRAINT_TYPE_CHECK_NOT_NULL )
    {
        sColumnHandle = & sCheckNotNullDesc->mColumnHandle;
    }
    else
    {
        sColumnHandle = NULL;
    }

    return sColumnHandle;
}



/**
 * @brief Check Clause Constraint 의 구문 
 * @param[in] aCheckClauseHandle   Check Clause Dictionary Handle
 * @return Check Clause String (NULL 이라면, 구현오류임)
 * @remarks
 */
stlChar  * ellGetCheckClausePhrase( ellDictHandle * aCheckClauseHandle )
{
    stlChar * sCheckClause = NULL;
    
    ellTableConstDesc     * sConstDesc = NULL;
    ellCheckClauseDepDesc * sCheckClauseDesc = NULL;
    
    STL_DASSERT( aCheckClauseHandle->mObjectType == ELL_OBJECT_CONSTRAINT );
    
    sConstDesc = (ellTableConstDesc *) ellGetObjectDesc( aCheckClauseHandle );

    sCheckClauseDesc = (ellCheckClauseDepDesc *) sConstDesc->mConstTypeDepDesc;
    
    if ( sConstDesc->mConstType == ELL_TABLE_CONSTRAINT_TYPE_CHECK_NOT_NULL )
    {
        sCheckClause = sCheckClauseDesc->mCheckClause;
    }
    else
    {
        sCheckClause = NULL;
    }

    return sCheckClause;
}


/**
 * @brief Constraint 객체가 Built-In Object 인지의 여부
 * @param[in]  aConstDictHandle  Dictionary Handle
 * @remarks
 */
stlBool  ellIsBuiltInConstraint( ellDictHandle * aConstDictHandle )
{
    ellTableConstDesc * sConstDesc = NULL;

    STL_DASSERT( aConstDictHandle->mObjectType == ELL_OBJECT_CONSTRAINT );
    
    sConstDesc = ellGetObjectDesc( aConstDictHandle );

    if ( sConstDesc->mConstOwnerID == ELDT_AUTH_ID_SYSTEM )
    {
        return STL_TRUE;
    }
    else
    {
        return STL_FALSE;
    }
}

/** @} ellObjectConstraint */
