/*******************************************************************************
 * ellDictIndex.c
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
 * @file ellDictIndex.c
 * @brief Index Dictionary 관리 모듈
 */

#include <ell.h>
#include <elDef.h>

#include <eldt.h>
#include <eldc.h>

#include <eldDictionary.h>
#include <eldTable.h>
#include <eldTableConstraint.h>
#include <eldIndex.h>
#include <eldAuthorization.h>

#include <ellDictIndex.h>

/**
 * @addtogroup ellObjectIndex
 * @{
 */

/**
 * @brief Index Type 에 대한 String 상수
 */
const stlChar * const gEllIndexTypeString[ELL_INDEX_TYPE_MAX] = 
{
    "N/A",                   /* ELL_INDEX_TYPE_NOT_AVAILABLE */
    
    "BTREE",                 /**< ELL_INDEX_TYPE_BTREE */
    "HASH",                  /**< ELL_INDEX_TYPE_HASH */
    "BITMAP",                /**< ELL_INDEX_TYPE_BITMAP */
    "RTREE",                 /**< ELL_INDEX_TYPE_RTREE */

    "FUNCTION-BASED BTREE",  /**< ELL_INDEX_TYPE_FUNCTION_BTREE */
    "FUNCTION-BASED HASH",   /**< ELL_INDEX_TYPE_FUNCTION_HASH */
    "FUNCTION-BASED BITMAP", /**< ELL_INDEX_TYPE_FUNCTION_BITMAP */
};


/**
 * @brief Index Column Ordering 에 대한 String 상수
 */
const stlChar * const gEllIndexColumnOrderString[] = 
{
    "DESC",  /**< ELL_INDEX_COLUMN_DESCENDING */
    "ASC"    /**< ELL_INDEX_COLUMN_ASCENDING */
};

/**
 * @brief Index Column Nulls Ordering 에 대한 String 상수
 */
const stlChar * const gEllIndexColumnNullsOrderString[] = 
{
    "NULLS_LAST",   /**< ELL_INDEX_COLUMN_NULLS_LAST */
    "NULLS_FIRST",  /**< ELL_INDEX_COLUMN_NULLS_FIRST */
};



/**********************************************************
 * DDL 관련 Object 획득 함수
 **********************************************************/


/**
 * @brief Index 목록에서 Table List 를 획득 
 * @param[in]  aTransID               Transaction ID
 * @param[in]  aViewSCN               View SCN
 * @param[in]  aIndexList             Index List
 * @param[in]  aRegionMem             Region Memory
 * @param[out] aTableList             Table List
 * @param[in]  aEnv                   Environment
 * @remarks
 */
stlStatus ellGetTableList4IndexList( smlTransId          aTransID,
                                     smlScn              aViewSCN,
                                     ellObjectList     * aIndexList,
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

    STL_PARAM_VALIDATE( aIndexList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableList != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Index List 를 순회하며, Table List 를 구성한다.
     */

    STL_TRY( ellInitObjectList( & sTableList, aRegionMem, aEnv ) == STL_SUCCESS );
    
    STL_RING_FOREACH_ENTRY( & aIndexList->mHeadList, sObjectItem )
    {
        sObjectHandle = & sObjectItem->mObjectHandle;

        /**
         * Table ID 획득
         */
        
        sTableID = ellGetIndexTableID( sObjectHandle );

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


/**********************************************************
 * DDL Lock 함수 
 **********************************************************/

/**
 * @brief CREATE INDEX 구문을 위한 DDL Lock 을 획득한다.
 * @param[in]  aTransID          Transaction ID
 * @param[in]  aStmt             Statement
 * @param[in]  aOwnerHandle      Index 의 Owner Handle
 * @param[in]  aSpaceHandle      Index 의 Tablespace Handle
 * @param[in]  aSchemaHandle     Index 의 Schema Handle
 * @param[in]  aTableDictHandle  Index 를 생성하는 Table 의 Dictionary Handle
 * @param[out] aLockSuccess      Lock 획득 여부 
 * @param[in]  aEnv              Environment
 * @remarks
 */
stlStatus ellLock4CreateIndex( smlTransId      aTransID,
                               smlStatement  * aStmt,
                               ellDictHandle * aOwnerHandle,
                               ellDictHandle * aSpaceHandle,
                               ellDictHandle * aSchemaHandle,
                               ellDictHandle * aTableDictHandle,
                               stlBool       * aLockSuccess,
                               ellEnv        * aEnv )
{
    stlBool  sLocked = STL_TRUE;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOwnerHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSpaceHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSchemaHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
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
     * Table 객체에 S lock
     */

    STL_TRY( eldLockTable4DDL( aTransID,
                               aStmt,
                               SML_LOCK_S,
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
 * @brief DROP INDEX 구문을 위한 DDL Lock 을 획득한다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aTableDictHandle Table Dictionary Handle
 * @param[in]  aIndexDictHandle Index Dictionary Handle
 * @param[out] aLockSuccess     Lock 획득 여부 
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus ellLock4DropIndex( smlTransId      aTransID,
                             smlStatement  * aStmt,
                             ellDictHandle * aTableDictHandle,
                             ellDictHandle * aIndexDictHandle,
                             stlBool       * aLockSuccess,
                             ellEnv        * aEnv )
{
    stlBool  sLocked = STL_TRUE;
    
    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIndexDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aLockSuccess != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Table 객체에 S lock
     */
    
    STL_TRY( eldLockTable4DDL( aTransID,
                               aStmt,
                               SML_LOCK_S,
                               aTableDictHandle,
                               & sLocked,
                               aEnv )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_FINISH );
    
    /**
     * Index Record 에 X lock 을 획득한다.
     */

    STL_TRY( eldLockRowByObjectHandle( aTransID,
                                       aStmt,
                                       ELDT_TABLE_ID_INDEXES,
                                       ELDT_Indexes_ColumnOrder_INDEX_ID,
                                       aIndexDictHandle,
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
 * @brief DROP INDEX 구문을 위한 DDL Lock 을 획득한다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aTableDictHandle Table Dictionary Handle
 * @param[in]  aIndexDictHandle Index Dictionary Handle
 * @param[out] aLockSuccess     Lock 획득 여부 
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus ellLock4AlterIndexPhysicalAttr( smlTransId      aTransID,
                                          smlStatement  * aStmt,
                                          ellDictHandle * aTableDictHandle,
                                          ellDictHandle * aIndexDictHandle,
                                          stlBool       * aLockSuccess,
                                          ellEnv        * aEnv )
{
    return ellLock4DropIndex( aTransID,
                              aStmt,
                              aTableDictHandle,
                              aIndexDictHandle,
                              aLockSuccess,
                              aEnv );
}



/**
 * @brief DDL 수행시 관련 Index List 에 대한 Lock 을 획득한다.
 * @param[in]  aTransID    Transaction ID
 * @param[in]  aStmt       Statement
 * @param[in]  aIndexList  Index Object List
 * @param[out] aLockSuccess     Lock 획득 여부 
 * @param[in]  aEnv        Environment
 * @remarks
 */
stlStatus ellLock4IndexList( smlTransId      aTransID,
                             smlStatement  * aStmt,
                             ellObjectList * aIndexList,
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
    STL_PARAM_VALIDATE( aIndexList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aLockSuccess != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * Table 목록을 순회하며 lock 을 획득 
     */

    STL_RING_FOREACH_ENTRY( & aIndexList->mHeadList, sObjectItem )
    {
        sObjectHandle = & sObjectItem->mObjectHandle;

        /**
         * Index Record 에 X lock 을 획득한다.
         */
        
        STL_TRY( eldLockRowByObjectHandle( aTransID,
                                           aStmt,
                                           ELDT_TABLE_ID_INDEXES,
                                           ELDT_Indexes_ColumnOrder_INDEX_ID,
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



/**********************************************************
 * Dictionary 변경 함수 
 **********************************************************/


/**
 * @brief Index Descriptor 를 Dictionary 에 추가한다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aOwnerID         Owner의 ID
 * @param[in]  aSchemaID        Schema ID
 * @param[out] aIndexID         Index 의 ID
 * @param[in]  aTablespaceID    Tablespace ID
 * @param[in]  aPhysicalID      인덱스의 Physical ID
 * @param[in]  aIndexName       Index Name
 * @param[in]  aIndexType       Index Type
 * @param[in]  aUnique          Unique Index 여부 
 * @param[in]  aInvalid         Index의 Invalid 여부 
 * @param[in]  aByConstraint    Index가 Key Constraint 에 의해서 생성되었는지의 여부 
 * @param[in]  aIndexComment    Index Comment (nullable)
 * @param[in]  aEnv             Execution Library Environment
 * @remarks
 */

stlStatus ellInsertIndexDesc( smlTransId     aTransID,
                              smlStatement * aStmt,
                              stlInt64       aOwnerID,
                              stlInt64       aSchemaID,
                              stlInt64     * aIndexID,
                              stlInt64       aTablespaceID,
                              stlInt64       aPhysicalID,
                              stlChar      * aIndexName,
                              ellIndexType   aIndexType,
                              stlBool        aUnique,
                              stlBool        aInvalid,
                              stlBool        aByConstraint,
                              stlChar      * aIndexComment,
                              ellEnv       * aEnv )
{
    /**
     * Index Dictionary Record 추가
     */
    
    
    STL_TRY( eldInsertIndexDesc( aTransID,
                                 aStmt,
                                 aOwnerID,
                                 aSchemaID,
                                 aIndexID,
                                 aTablespaceID,
                                 aPhysicalID,
                                 aIndexName,
                                 aIndexType,
                                 aUnique,
                                 aInvalid,
                                 aByConstraint,
                                 aIndexComment,
                                 aEnv )
             == STL_SUCCESS );

    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}




/**
 * @brief DROP INDEX 를 위한 Dictionary Row 제거 
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aIndexHandle     Index Dictionary Handle
 * @param[in]  aEnv             Environment
 * @remarks
 * Unique Index 의 경우, Unique Key Constraint 도 제거해야 한다.
 */
stlStatus ellDeleteDict4DropIndex( smlTransId        aTransID,
                                   smlStatement    * aStmt,
                                   ellDictHandle   * aIndexHandle,
                                   ellEnv          * aEnv )
{
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    
    STL_PARAM_VALIDATE( aIndexHandle != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * SQL-Index Dictionary 정보 삭제
     */
    
    STL_TRY( eldDeleteDictIndex( aTransID,
                                 aStmt,
                                 aIndexHandle,
                                 aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/**
 * @brief Index List 의 Dictionary Record 를 삭제한다.
 * @param[in] aTransID          aTransaction ID
 * @param[in] aStmt             Statement
 * @param[in] aIndexObjectList  Index Object List
 * @param[in] aEnv              Environemnt
 * @remarks
 */
stlStatus ellDeleteDictIndexByObjectList( smlTransId        aTransID,
                                          smlStatement    * aStmt,
                                          ellObjectList   * aIndexObjectList,
                                          ellEnv          * aEnv )
{
    ellObjectItem   * sObjectItem = NULL;
    ellDictHandle   * sObjectHandle = NULL;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIndexObjectList != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Index Object List 를 순회하며, Dictionary Record 를 삭제한다.
     */

    STL_RING_FOREACH_ENTRY( & aIndexObjectList->mHeadList, sObjectItem )
    {
        sObjectHandle = & sObjectItem->mObjectHandle;

        STL_TRY( ellDeleteDict4DropIndex( aTransID,
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
 * @brief ALTER INDEX 구문의 수행시간을 설정한다.
 * @param[in]  aTransID          Transaction ID
 * @param[in]  aStmt             Statement
 * @param[in]  aIndexID          Index ID
 * @param[in]  aEnv              Environment
 * @remarks
 */
stlStatus ellSetTimeAlterIndex( smlTransId      aTransID,
                                smlStatement  * aStmt,
                                stlInt64        aIndexID,
                                ellEnv        * aEnv )
{
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIndexID > ELL_DICT_OBJECT_ID_NA, ELL_ERROR_STACK(aEnv) );
    
    /**
     * Index Dictionary Record 가 이전에 변경되지 않아야 함.
     */
    
    STL_TRY( eldModifyDictDesc( aTransID,
                                aStmt,
                                ELDT_TABLE_ID_INDEXES,
                                ELDT_Indexes_ColumnOrder_INDEX_ID,
                                aIndexID,
                                ELDT_Indexes_ColumnOrder_MODIFIED_TIME,
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
 * @brief Index Dictionary 의 Physical ID 를 변경한다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aIndexID         Index 의 ID 
 * @param[in]  aPhysicalID      Index 의 Physical ID
 * @param[in]  aEnv             Execution Library Environment
 * @remarks
 */
stlStatus ellModifyIndexPhysicalID( smlTransId     aTransID,
                                    smlStatement * aStmt,
                                    stlInt64       aIndexID,
                                    stlInt64       aPhysicalID,
                                    ellEnv       * aEnv )
{
    STL_TRY( eldModifyDictDesc( aTransID,
                                aStmt,
                                ELDT_TABLE_ID_INDEXES,
                                ELDT_Indexes_ColumnOrder_INDEX_ID,
                                aIndexID,
                                ELDT_Indexes_ColumnOrder_PHYSICAL_ID,
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
 * @brief Index Dictionary 의 Key Index 를 User Index 로 변경한다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aIndexHandle     Index Dictionary Handle
 * @param[in]  aEnv             Execution Library Environment
 * @remarks
 */
stlStatus ellModifyKeyIndex2UserIndex( smlTransId      aTransID,
                                       smlStatement  * aStmt,
                                       ellDictHandle * aIndexHandle,
                                       ellEnv        * aEnv )
{
    stlBool   sByConstraint = STL_FALSE;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIndexHandle != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * KEY INDEX => USER INDEX 로 변경 
     */

    STL_TRY( eldModifyDictDesc( aTransID,
                                aStmt,
                                ELDT_TABLE_ID_INDEXES,
                                ELDT_Indexes_ColumnOrder_INDEX_ID,
                                ellGetIndexID(aIndexHandle),
                                ELDT_Indexes_ColumnOrder_BY_CONSTRAINT,
                                (void *) & sByConstraint,
                                aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/**
 * @brief Key Index List 를 User Index 로 변경한다.
 * @param[in] aTransID          aTransaction ID
 * @param[in] aStmt             Statement
 * @param[in] aIndexObjectList  Index Object List
 * @param[in] aEnv              Environemnt
 * @remarks
 */
stlStatus ellModifyKeyIndex2UserIndexByObjectList( smlTransId        aTransID,
                                                   smlStatement    * aStmt,
                                                   ellObjectList   * aIndexObjectList,
                                                   ellEnv          * aEnv )
{
    ellObjectItem   * sObjectItem = NULL;
    ellDictHandle   * sObjectHandle = NULL;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIndexObjectList != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Constraint Object List 를 순회하며, Dictionary Record 를 삭제한다.
     */

    STL_RING_FOREACH_ENTRY( & aIndexObjectList->mHeadList, sObjectItem )
    {
        sObjectHandle = & sObjectItem->mObjectHandle;

        STL_TRY( ellModifyKeyIndex2UserIndex( aTransID,
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

/**********************************************************
 * Cache 재구축 함수
 **********************************************************/


/**
 * @brief Index Cache 를 재구성
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aIndexDictHandle Index Dictionary Handle
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus ellRebuildIndexCache( smlTransId        aTransID,
                                smlStatement    * aStmt,
                                ellDictHandle   * aIndexDictHandle,
                                ellEnv          * aEnv )
{
    stlInt64  sIndexID = ELL_DICT_OBJECT_ID_NA;

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIndexDictHandle != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Index ID 획득
     */
    
    sIndexID = ellGetIndexID( aIndexDictHandle );

    /**
     * 기존 Cache 를 제거
     */

    STL_TRY( eldcDeleteCacheIndex( aTransID,
                                   aIndexDictHandle,
                                   aEnv )
             == STL_SUCCESS );
    
    /**
     * 새로운 Cache 를 구축 
     */

    STL_TRY( eldcInsertCacheIndexByIndexID( aTransID,
                                            aStmt,
                                            sIndexID,
                                            aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/**
 * @brief CREATE INDEX 를 위한 Cache 재구성 
 * @param[in]  aTransID     Transaction ID
 * @param[in]  aStmt        Statement
 * @param[in]  aAuthID      Authorization ID
 * @param[in]  aIndexID     Index ID
 * @param[in]  aEnv         Envirionment 
 * @remarks
 */
stlStatus ellRefineCache4CreateIndex( smlTransId           aTransID,
                                      smlStatement       * aStmt,
                                      stlInt64             aAuthID,
                                      stlInt64             aIndexID,
                                      ellEnv             * aEnv )
{
    STL_TRY( eldRefineCache4AddIndex( aTransID,
                                      aStmt,
                                      aAuthID,
                                      aIndexID,
                                      aEnv )
             == STL_SUCCESS );

    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}



/**
 * @brief DROP INDEX 를 위한 Cache 재구성  
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aTableHandle     Table Dictionary Handle
 * @param[in]  aIndexHandle     Index Dictionary Handle
 * @param[in]  aEnv             Environment
 * @remarks
 * Unique Index 의 경우, Unique Key Constraint 도 제거해야 한다.
 */
stlStatus ellRefineCache4DropIndex( smlTransId        aTransID,
                                    ellDictHandle   * aTableHandle,
                                    ellDictHandle   * aIndexHandle,
                                    ellEnv          * aEnv )
{
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aTableHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIndexHandle != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Unique Index 또는 Non-Unique Index 에 따라 구별하여 처리한다.
     */
    
    if ( ellGetIndexIsUnique(aIndexHandle) == STL_TRUE )
    {
        /******************************************************************
         * Unique Index 의 제거 
         ******************************************************************/

        /**
         * SQL-Table Cache 에서 Unique Index 정보 제거 
         */

        STL_TRY( eldcDelUniqueIndexFromTable( aTransID,
                                              aTableHandle,
                                              aIndexHandle,
                                              aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        /******************************************************************
         * Non-Unique Index 의 제거 
         ******************************************************************/

        /**
         * SQL-Table Cache 에서 Non-Unique Index 정보 제거 
         */

        STL_TRY( eldcDelNonUniqueIndexFromTable( aTransID,
                                                 aTableHandle,
                                                 aIndexHandle,
                                                 aEnv )
                 == STL_SUCCESS );
    }

    /**
     * SQL-Index Cache 정보를 삭제
     */
    
    STL_TRY( eldcDeleteCacheIndex( aTransID,
                                   aIndexHandle,
                                   aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/**
 * @brief DROP CONSTRAINT DROP INDEX 를 위한 Cache 재구성  
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aIndexHandle     Index Dictionary Handle
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus ellRefineCache4DropKeyIndex( smlTransId        aTransID,
                                       ellDictHandle   * aIndexHandle,
                                       ellEnv          * aEnv )
{
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aIndexHandle != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * SQL-Index Cache 정보를 삭제
     */
    
    STL_TRY( eldcDeleteCacheIndex( aTransID,
                                   aIndexHandle,
                                   aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/**
 * @brief DROP CONSTRAINT KEEP INDEX 를 위한 Cache 재구성  
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aTableHandle     Table Dictionary Handle
 * @param[in]  aIndexHandle     Index Dictionary Handle
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus ellRefineCache4KeepKeyIndex( smlTransId        aTransID,
                                       smlStatement    * aStmt,
                                       ellDictHandle   * aTableHandle,
                                       ellDictHandle   * aIndexHandle,
                                       ellEnv          * aEnv )
{
    stlInt64 sIndexID = ELL_DICT_OBJECT_ID_NA;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIndexHandle != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */

    sIndexID = ellGetIndexID(aIndexHandle);

    /**
     * Index Cache 재구성
     */

    STL_TRY( ellRebuildIndexCache( aTransID,
                                   aStmt,
                                   aIndexHandle,
                                   aEnv )
             == STL_SUCCESS );

    /**
     * Unique Index 또는 Non-Unique Index 에 따라 구별하여 처리한다.
     */
    
    if ( ellGetIndexIsUnique(aIndexHandle) == STL_TRUE )
    {
        /******************************************************************
         * Unique Index 정보에 추가 
         ******************************************************************/

        /**
         * SQL-Table Cache 에 Unique Index 정보 추가  
         */

        STL_TRY( eldcAddUniqueIndexIntoTable( aTransID,
                                              sIndexID,
                                              aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        /******************************************************************
         * Non-Unique Index 정보에 추가 
         ******************************************************************/

        /**
         * SQL-Table Cache 에 Non-Unique Index 정보 추가 
         */

        STL_TRY( eldcAddNonUniqueIndexIntoTable( aTransID,
                                                 sIndexID,
                                                 aEnv )
                 == STL_SUCCESS );
    }

    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/**********************************************************
 * Index DDL 관련 RunTime 검증 함수 
 **********************************************************/


/**
 * @brief Primary/Unique Key 추가시 SQL-Table Cache 에
 *        동일한 Key Column 을 갖는 SQL-Index Cache 가 존재하는 지 확인
 * @param[in]  aTransID       Transaction ID
 * @param[in]  aTableHandle   Table Dictionary Handle
 * @param[in]  aIndexHandle   Index Dictionary Handle
 * @param[out] aExist         동일한 Key 를 갖는 인덱스 존재 여부 
 * @param[in]  aEnv           Environment
 * @note Runtime Validation 으로 다른 Tx 의 종료 여부에 관계없이 검사한다.
 * @remarks
 * 다음과 같은 범위에서 검사한다.
 * - Primary Key
 * - Unique Key 
 * 다음과 같은 사항을 검사한다.
 * - Key Column 의 동일 여부
 */
stlStatus ellRuntimeCheckSameKey4UniqueKey( smlTransId      aTransID,
                                            ellDictHandle * aTableHandle,
                                            ellDictHandle * aIndexHandle,
                                            stlBool       * aExist,
                                            ellEnv        * aEnv )
{
    return eldcRuntimeCheckSameKey4UniqueKey( aTransID,
                                              aTableHandle,
                                              aIndexHandle,
                                              aExist,
                                              aEnv );
}

/**
 * @brief Foreign Key 추가시 SQL-Table Cache 에
 *        동일한 Key Column 을 갖는 SQL-Index Cache 가 존재하는 지 확인
 * @param[in]  aTransID       Transaction ID
 * @param[in]  aTableHandle   Table Dictionary Handle
 * @param[in]  aIndexHandle   Index Dictionary Handle
 * @param[out] aExist         동일한 Key 를 갖는 인덱스 존재 여부 
 * @param[in]  aEnv           Environment
 * @note Runtime Validation 으로 다른 Tx 의 종료 여부에 관계없이 검사한다.
 * @remarks
 * 다음과 같은 범위에서 검사한다.
 * - Foreign Key
 * 다음과 같은 사항을 검사한다.
 * - Key Column 의 동일 여부
 * 본 함수 호출 후 추가적으로 다음 사항을 검사해야 한다.
 * - Parent Key 가 동일한지의 여부 
 */
stlStatus ellRuntimeCheckSameKey4ForeignKey( smlTransId      aTransID,
                                             ellDictHandle * aTableHandle,
                                             ellDictHandle * aIndexHandle,
                                             stlBool       * aExist,
                                             ellEnv        * aEnv )
{
    return eldcRuntimeCheckSameKey4ForeignKey( aTransID,
                                               aTableHandle,
                                               aIndexHandle,
                                               aExist,
                                               aEnv );
}


/**
 * @brief Index 생성시 SQL-Table Cache 에
 *        동일한 Key Column 을 갖는 SQL-Index Cache 가 존재하는 지 확인
 * @param[in]  aTransID       Transaction ID
 * @param[in]  aTableHandle   Table Dictionary Handle
 * @param[in]  aIndexHandle   Index Dictionary Handle
 * @param[out] aExist         동일한 Key 를 갖는 인덱스 존재 여부 
 * @param[in]  aEnv           Environment
 * @note Runtime Validation 으로 다른 Tx 의 종료 여부에 관계없이 검사한다.
 * @remarks
 * 다음과 같은 범위에서 검사한다.
 * - Unique Index
 * - Non-Unique Index
 * 다음과 같은 사항을 검사한다.
 * - Key Column 의 동일 여부
 * - Key Order(Asc/Desc) 의 동일 여부
 * - Null Order(First/Last) 의 동일 여부
 */
stlStatus ellRuntimeCheckSameKey4Index( smlTransId      aTransID,
                                        ellDictHandle * aTableHandle,
                                        ellDictHandle * aIndexHandle,
                                        stlBool       * aExist,
                                        ellEnv        * aEnv )
{
    return eldcRuntimeCheckSameKey4Index( aTransID,
                                          aTableHandle,
                                          aIndexHandle,
                                          aExist,
                                          aEnv );
}





/**********************************************************
 * Handle 획득 함수 
 **********************************************************/





/**
 * @brief Schema Handle 과 Index Name 을 이용해 Index Dictionary Handle 을 획득 
 * @param[in]  aTransID           Transaction ID
 * @param[in]  aViewSCN           View SCN
 * @param[in]  aSchemaDictHandle  Schema Dictionary Handle
 * @param[in]  aIndexName         Index Name
 * @param[out] aIndexDictHandle   Index Dictionary Handle
 * @param[out] aExist             존재 여부 
 * @param[in]  aEnv               Envirionment 
 * @remarks
 */
stlStatus ellGetIndexDictHandleWithSchema( smlTransId           aTransID,
                                           smlScn               aViewSCN,
                                           ellDictHandle      * aSchemaDictHandle,
                                           stlChar            * aIndexName,
                                           ellDictHandle      * aIndexDictHandle,
                                           stlBool            * aExist,
                                           ellEnv             * aEnv )
{
    ellSchemaDesc * sSchemaDesc = NULL;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aSchemaDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIndexName != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIndexDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExist != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Schema Descriptor
     */

    sSchemaDesc = (ellSchemaDesc *) ellGetObjectDesc( aSchemaDictHandle );

    /**
     * Index Dictionary Handle 획득
     */
    
    ellInitDictHandle( aIndexDictHandle );
    
    STL_TRY( eldGetIndexHandleByName( aTransID,
                                      aViewSCN,
                                      sSchemaDesc->mSchemaID,
                                      aIndexName,
                                      aIndexDictHandle,
                                      aExist,
                                      aEnv )
             == STL_SUCCESS );

    if ( *aExist == STL_TRUE )
    {
        aIndexDictHandle->mFullyQualifiedName = STL_TRUE;
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief Auth Handle 과 Index Name 을 이용해 Index Dictionary Handle 을 얻는다.
 * @param[in]  aTransID             Transaction ID
 * @param[in]  aViewSCN             View SCN
 * @param[in]  aAuthDictHandle      Authorization Dictionary Handle
 * @param[in]  aIndexName           Index Name
 * @param[out] aIndexDictHandle     Index Dictionary Handle
 * @param[out] aExist               존재 여부 
 * @param[in]  aEnv                 Envirionment 
 * @remarks
 */
stlStatus ellGetIndexDictHandleWithAuth( smlTransId           aTransID,
                                         smlScn               aViewSCN,
                                         ellDictHandle      * aAuthDictHandle,
                                         stlChar            * aIndexName,
                                         ellDictHandle      * aIndexDictHandle,
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
    STL_PARAM_VALIDATE( aIndexName != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIndexDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExist != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Authorization Descriptor
     */

    sAuthDesc = (ellAuthDesc *) ellGetObjectDesc( aAuthDictHandle );

    /**
     * Index Dictionary Handle 획득
     */
    
    ellInitDictHandle( aIndexDictHandle );
    
    while(1)
    {
        /**
         * 사용자 Schema Path 를 이용해 검색
         */
        for ( i = 0; i < sAuthDesc->mSchemaPathCnt; i++ )
        {
            STL_TRY( eldGetIndexHandleByName( aTransID,
                                              aViewSCN,
                                              sAuthDesc->mSchemaIDArray[i],
                                              aIndexName,
                                              aIndexDictHandle,
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
                STL_TRY( eldGetIndexHandleByName( aTransID,
                                                  aViewSCN,
                                                  sPublicDesc->mSchemaIDArray[i],
                                                  aIndexName,
                                                  aIndexDictHandle,
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
 * @brief Index ID 를 이용해 Index Dictionary Handle 을 얻는다.
 * @param[in]  aTransID          Transaction ID
 * @param[in]  aViewSCN          View SCN
 * @param[in]  aIndexID          Index Dictionary ID
 * @param[out] aIndexDictHandle  Index Dictionary Handle
 * @param[in]  aExist            존재 여부
 * @param[in]  aEnv              Environment
 * @remarks
 */
stlStatus ellGetIndexDictHandleByID( smlTransId           aTransID,
                                     smlScn               aViewSCN,
                                     stlInt64             aIndexID,
                                     ellDictHandle      * aIndexDictHandle,
                                     stlBool            * aExist,
                                     ellEnv             * aEnv )
{
    return eldGetIndexHandleByID( aTransID,
                                  aViewSCN,
                                  aIndexID,
                                  aIndexDictHandle,
                                  aExist,
                                  aEnv );    
}





/**
 * @brief Table 과 관련된 Unique Index 개수와 Index Dictionary Handle Array 를 얻는다.
 * @param[in]  aTransID               Transaction ID
 * @param[in]  aViewSCN               View SCN
 * @param[in]  aRegionMem             Index Dictionary Handle Array 를 할당받기 위한 메모리 관리자 
 * @param[in]  aTableDictHandle       Table Dictionary Handle
 * @param[out] aUniqueIndexCnt        Unique Index 개수
 * @param[out] aIndexDictHandleArray  Unique Index Dictionary Handle Array
 * @param[in]  aEnv                   Environment
 * @remarks
 */
stlStatus ellGetTableUniqueIndexDictHandle( smlTransId       aTransID,
                                            smlScn           aViewSCN,
                                            knlRegionMem   * aRegionMem,
                                            ellDictHandle  * aTableDictHandle,
                                            stlInt32       * aUniqueIndexCnt,
                                            ellDictHandle ** aIndexDictHandleArray,
                                            ellEnv         * aEnv )
{
    return eldGetTableRelatedDictHandle( aTransID,
                                         aViewSCN,
                                         aRegionMem,
                                         aTableDictHandle,
                                         ELL_TABLE_RELATED_UNIQUE_INDEX,
                                         aUniqueIndexCnt,
                                         aIndexDictHandleArray,
                                         aEnv );
}

/**
 * @brief Table 과 관련된 Non-Unique Index 개수와 Index Dictionary Handle Array 를 얻는다.
 * @param[in]  aTransID               Transaction ID
 * @param[in]  aViewSCN               View SCN
 * @param[in]  aRegionMem       Index Dictionary Handle Array 를 할당받기 위한 메모리 관리자 
 * @param[in]  aTableDictHandle       Table Dictionary Handle
 * @param[out] aNonUniqueIndexCnt     Non-Unique Index 개수
 * @param[out] aIndexDictHandleArray  Non-Unique Index Dictionary Handle Array
 * @param[in]  aEnv                   Environment
 * @remarks
 */
stlStatus ellGetTableNonUniqueIndexDictHandle( smlTransId       aTransID,
                                               smlScn           aViewSCN,
                                               knlRegionMem   * aRegionMem,
                                               ellDictHandle  * aTableDictHandle,
                                               stlInt32       * aNonUniqueIndexCnt,
                                               ellDictHandle ** aIndexDictHandleArray,
                                               ellEnv         * aEnv )
{
    return eldGetTableRelatedDictHandle( aTransID,
                                         aViewSCN,
                                         aRegionMem,
                                         aTableDictHandle,
                                         ELL_TABLE_RELATED_NON_UNIQUE_INDEX,
                                         aNonUniqueIndexCnt,
                                         aIndexDictHandleArray,
                                         aEnv );
}



/**********************************************************
 * 정보 획득 함수
 **********************************************************/






/**
 * @brief Index ID 정보 
 * @param[in] aIndexDictHandle Index Dictionary Handle
 * @return Index ID
 * @remarks
 */
stlInt64     ellGetIndexID( ellDictHandle * aIndexDictHandle )
{
    ellIndexDesc * sIndexDesc = NULL;

    STL_DASSERT( aIndexDictHandle->mObjectType == ELL_OBJECT_INDEX );
    
    sIndexDesc = (ellIndexDesc *) ellGetObjectDesc( aIndexDictHandle );

    return sIndexDesc->mIndexID;
}

/**
 * @brief Index Owner ID 정보
 * @param[in] aIndexDictHandle Index Dictionary Handle
 * @return Index Owner ID
 * @remarks
 */
stlInt64 ellGetIndexOwnerID( ellDictHandle * aIndexDictHandle )
{
    ellIndexDesc * sIndexDesc = NULL;

    STL_DASSERT( aIndexDictHandle->mObjectType == ELL_OBJECT_INDEX );
    
    sIndexDesc = (ellIndexDesc *) ellGetObjectDesc( aIndexDictHandle );

    return sIndexDesc->mOwnerID;
}

/**
 * @brief Index Schema ID 정보
 * @param[in] aIndexDictHandle Index Dictionary Handle
 * @return Index Schema ID
 * @remarks
 */
stlInt64 ellGetIndexSchemaID( ellDictHandle * aIndexDictHandle )
{
    ellIndexDesc * sIndexDesc = NULL;

    STL_DASSERT( aIndexDictHandle->mObjectType == ELL_OBJECT_INDEX );
    
    sIndexDesc = (ellIndexDesc *) ellGetObjectDesc( aIndexDictHandle );

    return sIndexDesc->mSchemaID;
}


/**
 * @brief Index 와 관계된 Table ID 정보
 * @param[in] aIndexDictHandle Index Dictionary Handle
 * @return Index 와 관련된 Table ID
 * @remarks
 * INDEX 는 TABLE 에 종속되지 않는다.
 * @todo 현재는 문제 소지가 없지만, Joined Index 등이 구현되면 문제소지가 있다.
 */
stlInt64 ellGetIndexTableID( ellDictHandle * aIndexDictHandle )
{
    stlInt64       sTableID = 0;
    ellIndexDesc * sIndexDesc = NULL;

    STL_DASSERT( aIndexDictHandle->mObjectType == ELL_OBJECT_INDEX );
    
    sIndexDesc = (ellIndexDesc *) ellGetObjectDesc( aIndexDictHandle );

    sTableID = ellGetColumnTableID( & sIndexDesc->mKeyDesc[0].mKeyColumnHandle );

    STL_DASSERT( sTableID == sIndexDesc->mDebugTableID );
    
    return sTableID;
}

/**
 * @brief Index 의 Tablespace ID 정보
 * @param[in] aIndexDictHandle Index Dictionary Handle
 * @return Index 의 Tablespace ID
 * @remarks
 */
stlInt64 ellGetIndexTablespaceID( ellDictHandle * aIndexDictHandle )
{
    ellIndexDesc * sIndexDesc = NULL;

    STL_DASSERT( aIndexDictHandle->mObjectType == ELL_OBJECT_INDEX );
    
    sIndexDesc = (ellIndexDesc *) ellGetObjectDesc( aIndexDictHandle );

    return sIndexDesc->mTablespaceID;
}

/**
 * @brief Index Name 정보 
 * @param[in] aIndexDictHandle Index Dictionary Handle
 * @return Index Name
 * @remarks
 */
stlChar * ellGetIndexName( ellDictHandle * aIndexDictHandle )
{
    ellIndexDesc * sIndexDesc = NULL;

    STL_DASSERT( aIndexDictHandle->mObjectType == ELL_OBJECT_INDEX );
    
    sIndexDesc = (ellIndexDesc *) ellGetObjectDesc( aIndexDictHandle );

    return sIndexDesc->mIndexName;
}


/**
 * @brief Unique Index 여부 
 * @param[in] aIndexDictHandle Index Dictionary Handle
 * @return 
 * - STL_TRUE : unique index
 * - STL_FALSE : non-unique index
 * @remarks
 */
stlBool ellGetIndexIsUnique( ellDictHandle * aIndexDictHandle )
{
    ellIndexDesc * sIndexDesc = NULL;

    STL_DASSERT( aIndexDictHandle->mObjectType == ELL_OBJECT_INDEX );
    
    sIndexDesc = (ellIndexDesc *) ellGetObjectDesc( aIndexDictHandle );

    return sIndexDesc->mIsUnique;
}


/**
 * @brief Index 의 Invalid 여부 
 * @param[in] aIndexDictHandle Index Dictionary Handle
 * @return 
 * - STL_TRUE  : Invalid Index
 * - STL_FALSE : Valid Index
 * @remarks
 */
stlBool ellGetIndexIsInvalid( ellDictHandle * aIndexDictHandle )
{
    ellIndexDesc * sIndexDesc = NULL;

    STL_DASSERT( aIndexDictHandle->mObjectType == ELL_OBJECT_INDEX );
    
    sIndexDesc = (ellIndexDesc *) ellGetObjectDesc( aIndexDictHandle );

    return sIndexDesc->mInvalid;
}


/**
 * @brief Index 가 제약조건에 의해 생성되었는지의 여부 
 * @param[in] aIndexDictHandle Index Dictionary Handle
 * @return 
 * - STL_TRUE  : 제약조건 생성에 의해 자동으로 생성된 인덱스 
 * - STL_FALSE : CREATE [UNIQUE] INDEX 구문으로 생성된 인덱스 
 * @remarks
 */
stlBool ellGetIndexIsByConstraint( ellDictHandle * aIndexDictHandle )
{
    ellIndexDesc * sIndexDesc = NULL;

    STL_DASSERT( aIndexDictHandle->mObjectType == ELL_OBJECT_INDEX );
    
    sIndexDesc = (ellIndexDesc *) ellGetObjectDesc( aIndexDictHandle );

    return sIndexDesc->mByConstraint;
}

/**
 * @brief Index Handle 
 * @param[in] aIndexDictHandle Index Dictionary Handle
 * @return
 * 물리적 Index Handle
 * @remarks
 */
void  * ellGetIndexHandle( ellDictHandle * aIndexDictHandle )
{
    ellIndexDesc * sIndexDesc = NULL;

    STL_DASSERT( aIndexDictHandle->mObjectType == ELL_OBJECT_INDEX );
    
    sIndexDesc = (ellIndexDesc *) ellGetObjectDesc( aIndexDictHandle );

    return sIndexDesc->mIndexHandle;
}


/**
 * @brief Index 의 Key Column Count
 * @param[in] aIndexDictHandle Index Dictionary Handle
 * @return
 * Key Column 개수 
 * @remarks
 */
stlInt32  ellGetIndexKeyCount( ellDictHandle * aIndexDictHandle )
{
    ellIndexDesc * sIndexDesc = NULL;

    STL_DASSERT( aIndexDictHandle->mObjectType == ELL_OBJECT_INDEX );
    
    sIndexDesc = (ellIndexDesc *) ellGetObjectDesc( aIndexDictHandle );

    return sIndexDesc->mKeyColumnCnt;
}



/**
 * @brief Index 의 Key Column Descriptor Array 
 * @param[in] aIndexDictHandle Index Dictionary Handle
 * @return
 * Index Key Column Descriptor Array (ellIndexKeyDesc)
 * @remarks
 */
ellIndexKeyDesc * ellGetIndexKeyDesc( ellDictHandle * aIndexDictHandle )
{
    ellIndexDesc * sIndexDesc = NULL;

    STL_DASSERT( aIndexDictHandle->mObjectType == ELL_OBJECT_INDEX );
    
    sIndexDesc = (ellIndexDesc *) ellGetObjectDesc( aIndexDictHandle );

    return sIndexDesc->mKeyDesc;
}



/**
 * @brief Index 객체가 Built-In Object 인지의 여부
 * @param[in]  aIndexDictHandle  Dictionary Handle
 * @remarks
 */
stlBool ellIsBuiltInIndex( ellDictHandle * aIndexDictHandle )
{
    ellIndexDesc * sIndexDesc = NULL;

    STL_DASSERT( aIndexDictHandle->mObjectType == ELL_OBJECT_INDEX );
    
    sIndexDesc = (ellIndexDesc *) ellGetObjectDesc( aIndexDictHandle );

    if ( sIndexDesc->mOwnerID == ELDT_AUTH_ID_SYSTEM )
    {
        return STL_TRUE;
    }
    else
    {
        return STL_FALSE;
    }
}

/** @} ellObjectIndex */
