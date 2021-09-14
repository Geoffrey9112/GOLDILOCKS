/*******************************************************************************
 * ellDictColumn.c
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
 * @file ellDictColumn.c
 * @brief Column Dictionary 관리 모듈
 */

#include <ell.h>
#include <elDef.h>

#include <eldt.h>
#include <eldc.h>
#include <eldDictionary.h>

#include <eldColumn.h>
#include <eldTable.h>



/**
 * @addtogroup ellColumn
 * @{
 */

/**
 * @brief Identity Generation Option의 String 상수 
 */
const stlChar * const gEllIdentityGenOptionString[ELL_IDENTITY_GENERATION_MAX] =
{
    "N/A",        /**< ELL_IDENTITY_GENERATION_NOT_AVAILABLE */
    
    "ALWAYS",     /**< ELL_IDENTITY_GENERATION_ALWAYS */
    "BY DEFAULT"  /**< ELL_IDENTITY_GENERATION_BY_DEFAULT */
};




/************************************************************
 * DDL behavior 관련 Object 획득 함수
 ************************************************************/


/**
 * @brief ALTER TABLE .. DROP COLUMN 에 의해 영향을 받는 Object Handle List 를 획득
 * @param[in]  aTransID             Transaction ID
 * @param[in]  aStmt                Statement
 * @param[in]  aColumnList          Drop Column List
 * @param[in]  aRegionMem           Region Memory
 * @param[out] aUniqueKeyList       Unique(PK/UK) Key List
 * @param[out] aForeignKeyList      Foreign Key List
 * @param[out] aChildForeignKeyList Child Foreign Key List
 * @param[out] aCheckConstList      Check Constraint(Not Null/Check Clause) List
 * @param[out] aNonKeyIndexList     Non-Key Index List
 * @param[out] aSetNullColumnList   NULLABLE 로 변경할 Column List
 * @param[in]  aEnv                 Environment
 * @remarks
 */
stlStatus ellGetObjectList4DropColumn( smlTransId       aTransID,
                                       smlStatement   * aStmt,
                                       ellObjectList  * aColumnList,
                                       knlRegionMem   * aRegionMem,
                                       ellObjectList ** aUniqueKeyList,
                                       ellObjectList ** aForeignKeyList,
                                       ellObjectList ** aChildForeignKeyList,
                                       ellObjectList ** aCheckConstList,
                                       ellObjectList ** aNonKeyIndexList,
                                       ellObjectList ** aSetNullColumnList,
                                       ellEnv         * aEnv )
{
    ellObjectList * sConstList = NULL;
    ellObjectList * sIndexList = NULL;

    ellTableConstraintType sConstType = ELL_TABLE_CONSTRAINT_TYPE_NA;
    
    ellObjectList * sUniqueKeyList     = NULL;
    ellObjectList * sForeignKeyList    = NULL;
    ellObjectList * sCheckConstList    = NULL;
    ellObjectList * sNonKeyIndexList   = NULL;
    ellObjectList * sSetNullColumnList = NULL;

    ellObjectList * sChildForeignKeyList  = NULL;
    ellObjectList * sTableNotNullList  = NULL;
    
    ellObjectItem   * sObjectItem = NULL;
    ellDictHandle   * sObjectHandle = NULL;

    ellObjectItem   * sTempObjectItem = NULL;
    ellDictHandle   * sTempObjectHandle = NULL;

    ellDictHandle   * sPriKeyHandle = NULL;
    stlInt32          sPriKeyCnt = 0;
    ellDictHandle   * sPriColHandle = NULL;

    stlInt32  i = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColumnList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aUniqueKeyList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aForeignKeyList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aChildForeignKeyList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCheckConstList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNonKeyIndexList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSetNullColumnList != NULL, ELL_ERROR_STACK(aEnv) );


    /**
     * Object List 초기화
     */

    STL_TRY( ellInitObjectList( & sUniqueKeyList,   aRegionMem, aEnv ) == STL_SUCCESS );
    STL_TRY( ellInitObjectList( & sForeignKeyList,  aRegionMem, aEnv ) == STL_SUCCESS );
    STL_TRY( ellInitObjectList( & sCheckConstList,  aRegionMem, aEnv ) == STL_SUCCESS );
    STL_TRY( ellInitObjectList( & sNonKeyIndexList, aRegionMem, aEnv ) == STL_SUCCESS );
    STL_TRY( ellInitObjectList( & sSetNullColumnList, aRegionMem, aEnv ) == STL_SUCCESS );

    /**
     * 대상 컬럼과 관련된 Constraint 정보 획득
     */

    STL_RING_FOREACH_ENTRY( & aColumnList->mHeadList, sObjectItem )
    {
        sObjectHandle = & sObjectItem->mObjectHandle;

        /**
         * Column 과 관련된 Key Constraint 정보 획득
         * - SELECT CONSTRAINT_ID
         * - FROM DEFINITION_SCHEMA.KEY_COLUMN_USAGE
         * - WHERE COLUMN_ID = ellGetColumnID(sObjectHandle)
         */

        STL_TRY( ellInitObjectList( & sConstList, aRegionMem, aEnv ) == STL_SUCCESS );
        
        STL_TRY( eldGetObjectList( aTransID,
                                   aStmt,
                                   ELDT_TABLE_ID_KEY_COLUMN_USAGE,
                                   ELL_OBJECT_CONSTRAINT,
                                   ELDT_KeyColumn_ColumnOrder_CONSTRAINT_ID,
                                   ELDT_KeyColumn_ColumnOrder_COLUMN_ID,
                                   ellGetColumnID(sObjectHandle),
                                   aRegionMem,
                                   sConstList,
                                   aEnv )
                 == STL_SUCCESS );

        /**
         * Key Constraint 를 유형별로 분류
         */

        STL_RING_FOREACH_ENTRY( & sConstList->mHeadList, sTempObjectItem )
        {
            sTempObjectHandle = & sTempObjectItem->mObjectHandle;

            sConstType = ellGetConstraintType( sTempObjectHandle );
            switch( sConstType )
            {
                case ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY:
                case ELL_TABLE_CONSTRAINT_TYPE_UNIQUE_KEY:
                    {
                        if ( sConstType == ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY )
                        {
                            sPriKeyHandle = sTempObjectHandle;
                        }
                        else
                        {
                            /* nothing to do */
                        }
                        
                        STL_TRY( ellAddNewObjectItem( sUniqueKeyList,
                                                      sTempObjectHandle,
                                                      NULL,
                                                      aRegionMem,
                                                      aEnv )
                                 == STL_SUCCESS );
                        break;
                    }
                case ELL_TABLE_CONSTRAINT_TYPE_FOREIGN_KEY:
                    {
                        STL_TRY( ellAddNewObjectItem( sForeignKeyList,
                                                      sTempObjectHandle,
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

        /**
         * Column 과 관련된 Check Constraint(Not Null, Check Clause) 정보 획득
         * - SELECT CONSTRAINT_ID
         * - FROM DEFINITION_SCHEMA.CHECK_COLUMN_USAGE
         * - WHERE COLUMN_ID = ellGetColumnID(sObjectHandle)
         */

        STL_TRY( ellInitObjectList( & sConstList, aRegionMem, aEnv ) == STL_SUCCESS );
        
        STL_TRY( eldGetObjectList( aTransID,
                                   aStmt,
                                   ELDT_TABLE_ID_CHECK_COLUMN_USAGE,
                                   ELL_OBJECT_CONSTRAINT,
                                   ELDT_CheckColumn_ColumnOrder_CONSTRAINT_ID,
                                   ELDT_CheckColumn_ColumnOrder_COLUMN_ID,
                                   ellGetColumnID(sObjectHandle),
                                   aRegionMem,
                                   sConstList,
                                   aEnv )
                 == STL_SUCCESS );

        /**
         * Check Constraint 를 추가 
         */

        STL_RING_FOREACH_ENTRY( & sConstList->mHeadList, sTempObjectItem )
        {
            sTempObjectHandle = & sTempObjectItem->mObjectHandle;

            STL_TRY( ellAddNewObjectItem( sCheckConstList,
                                          sTempObjectHandle,
                                          NULL,
                                          aRegionMem,
                                          aEnv )
                     == STL_SUCCESS );
        }

        
        
        /**
         * Column 과 관련된 Index 정보 획득
         * - SELECT INDEX_ID
         * - FROM DEFINITION_SCHEMA.INDEX_KEY_COLUMN_USAGE
         * - WHERE COLUMN_ID = ellGetColumnID(sObjectHandle)
         */

        STL_TRY( ellInitObjectList( & sIndexList, aRegionMem, aEnv ) == STL_SUCCESS );
        
        STL_TRY( eldGetObjectList( aTransID,
                                   aStmt,
                                   ELDT_TABLE_ID_INDEX_KEY_COLUMN_USAGE,
                                   ELL_OBJECT_INDEX,
                                   ELDT_IndexKey_ColumnOrder_INDEX_ID,
                                   ELDT_IndexKey_ColumnOrder_COLUMN_ID,
                                   ellGetColumnID(sObjectHandle),
                                   aRegionMem,
                                   sIndexList,
                                   aEnv )
                 == STL_SUCCESS );

        /**
         * Key 와 관련되지 않은 Index 정보만 추출
         */

        STL_RING_FOREACH_ENTRY( & sIndexList->mHeadList, sTempObjectItem )
        {
            sTempObjectHandle = & sTempObjectItem->mObjectHandle;

            if ( ellGetIndexIsByConstraint( sTempObjectHandle ) == STL_TRUE )
            {
                /**
                 * Key 와 관련된 index 임
                 */
            }
            else
            {
                /**
                 * Key 와 무관한 index 임
                 */

                STL_TRY( ellAddNewObjectItem( sNonKeyIndexList,
                                              sTempObjectHandle,
                                              NULL,
                                              aRegionMem,
                                              aEnv )
                         == STL_SUCCESS );
            }
        }
    }

    /**
     * Unique Key 를 참조하는 Child Foreign Key 구성 
     */

    STL_TRY( ellGetChildForeignKeyListByUniqueList( aTransID,
                                                    aStmt,
                                                    sUniqueKeyList,
                                                    aRegionMem,
                                                    & sChildForeignKeyList,
                                                    aEnv )
             == STL_SUCCESS );

    /**
     * Nullable 로 설정할 Column List 구성
     */

    if ( sPriKeyHandle != NULL )
    {
        STL_TRY( ellGetTableConstTypeList( aTransID,
                                           aStmt,
                                           ellGetConstraintTableID( sPriKeyHandle ),
                                           ELL_TABLE_CONSTRAINT_TYPE_CHECK_NOT_NULL,
                                           aRegionMem,
                                           & sTableNotNullList,
                                           aEnv )
                 == STL_SUCCESS );
        
        sPriKeyCnt    = ellGetPrimaryKeyColumnCount( sPriKeyHandle );
        sPriColHandle = ellGetPrimaryKeyColumnHandle( sPriKeyHandle );

        for ( i = 0; i < sPriKeyCnt; i++ )
        {
            /**
             * DROP COLUMN 대상이 아니어야 함.
             */

            if ( ellObjectExistInObjectList( ELL_OBJECT_COLUMN,
                                             ellGetColumnID( & sPriColHandle[i] ),
                                             aColumnList )
                 == STL_TRUE )
            {
                /**
                 * DROP Column 대상임
                 */
            }
            else
            {
                /**
                 * Column 을 포함하는 NOT NULL Constraint 가 없어야 함.
                 */
                
                if ( ellIsColumnInsideNotNullList( & sPriColHandle[i],
                                                   sTableNotNullList,
                                                   aEnv )
                     == STL_TRUE )
                {
                    /**
                     * NOT NULL Constraint 가 존재함
                     */
                }
                else
                {
                    /**
                     * NOT NULL 제약 조건이 없으므로 Nullable 컬럼으로 설정해야 함.
                     */
                    
                    STL_TRY( ellAddNewObjectItem( sSetNullColumnList,
                                                  & sPriColHandle[i],
                                                  NULL,
                                                  aRegionMem,
                                                  aEnv )
                             == STL_SUCCESS );
                }
            }
        }
    }
    else
    {
        /**
         * Nothing To Do
         */
    }
    
    /**
     * Output 설정
     */

    *aUniqueKeyList        = sUniqueKeyList;
    *aForeignKeyList       = sForeignKeyList;
    *aChildForeignKeyList  = sChildForeignKeyList;
    *aCheckConstList       = sCheckConstList;
    *aNonKeyIndexList      = sNonKeyIndexList;
    *aSetNullColumnList    = sSetNullColumnList;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief Column 이 Not Null List 에 존재하는 지 검사 
 * @param[in] aColumnHandle     Column Handle
 * @param[in] aNotNullList      NOT NULL Object List
 * @param[in] aEnv              Environment
 * @return
 * stlBool
 * @remarks
 */
stlBool ellIsColumnInsideNotNullList( ellDictHandle    * aColumnHandle,
                                      ellObjectList    * aNotNullList,
                                      ellEnv           * aEnv )
{
    stlBool  sExist = STL_FALSE;
    
    ellObjectItem   * sObjectItem   = NULL;
    ellDictHandle   * sObjectHandle = NULL;

    ellDictHandle   * sNotNullColumnHandle = NULL;
    
    /**
     * Parameter Validation
     */

    STL_ASSERT( aColumnHandle != NULL );
    STL_ASSERT( aNotNullList != NULL );
    
    /**
     * Key Column 을 포함하고 있는 NOT NULL Constraint 가 존재하는 지 확인
     */

    sExist = STL_FALSE;
    
    STL_RING_FOREACH_ENTRY( & aNotNullList->mHeadList, sObjectItem )
    {
        sObjectHandle = & sObjectItem->mObjectHandle;
        
        sNotNullColumnHandle = ellGetCheckNotNullColumnHandle( sObjectHandle );
        
        if ( ellGetColumnID(aColumnHandle) == ellGetColumnID(sNotNullColumnHandle) )
        {
            sExist = STL_TRUE;
            break;
        }
        else
        {
            continue;
        }
    }
    
    return sExist;
}


/**
 * @brief ALTER TABLE .. RENAME COLUMN 에 의해 영향을 받는 Object Handle List 를 획득
 * @param[in]  aTransID          Transaction ID
 * @param[in]  aStmt             Statement
 * @param[in]  aColumnDictHandle Column Dictionary Handle
 * @param[in]  aRegionMem        Region Memory
 * @param[out] aCheckList        Check Constraint Clause Object List
 * @param[in]  aEnv              Environment
 */
stlStatus ellGetObjectList4RenameColumn( smlTransId          aTransID,
                                         smlStatement      * aStmt,
                                         ellDictHandle     * aColumnDictHandle,
                                         knlRegionMem      * aRegionMem,
                                         ellObjectList    ** aCheckList,
                                         ellEnv            * aEnv )
{
    ellObjectList * sCheckObjectList = NULL;
    
    stlInt64        sColumnID = ELL_DICT_OBJECT_ID_NA;

    ellObjectList * sObjectList = NULL;
    
    ellObjectItem * sObjectItem = NULL;
    ellDictHandle * sObjectHandle = NULL;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColumnDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCheckList != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득 
     */
    
    sColumnID = ellGetColumnID( aColumnDictHandle );

    /************************************************
     * CHECK CONSTRAINT 정보 획득 
     ************************************************/
    
    /**
     * Column 과 관련된 Check Constraint 정보 획득
     * - SELECT CONSTRAINT_ID
     * - FROM DEFINITION_SCHEMA.CHECK_COLUMN_USAGE
     * - WHERE COLUMN_ID = sColumnID
     */

    STL_TRY( ellInitObjectList( & sObjectList, aRegionMem, aEnv ) == STL_SUCCESS );

    STL_TRY( eldGetObjectList( aTransID,
                               aStmt,
                               ELDT_TABLE_ID_CHECK_COLUMN_USAGE,
                               ELL_OBJECT_CONSTRAINT,
                               ELDT_CheckColumn_ColumnOrder_CONSTRAINT_ID,
                               ELDT_CheckColumn_ColumnOrder_COLUMN_ID,
                               sColumnID,
                               aRegionMem,
                               sObjectList,
                               aEnv )
             == STL_SUCCESS );

    /**
     * CHECK CONSTRAINT 만 추출 
     */

    STL_TRY( ellInitObjectList( & sCheckObjectList, aRegionMem, aEnv ) == STL_SUCCESS );
    
    STL_RING_FOREACH_ENTRY( & sObjectList->mHeadList, sObjectItem )
    {
        sObjectHandle = & sObjectItem->mObjectHandle;

        if ( ellGetConstraintType( sObjectHandle )
             == ELL_TABLE_CONSTRAINT_TYPE_CHECK_CLAUSE )
        {
            /**
             * 원하는 Constraint 임
             */
            
            STL_TRY( ellAddNewObjectItem( sCheckObjectList,
                                          sObjectHandle,
                                          NULL,
                                          aRegionMem,
                                          aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            /**
             * Check Clause 가 아님
             * - nothing to do
             */
        }
    }
    
    /**
     * Output 설정
     */

    *aCheckList = sCheckObjectList;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}





/**
 * @brief Column 이 포함된 Index List 획득 
 * @param[in]  aTransID             Transaction ID
 * @param[in]  aStmt                Statement
 * @param[in]  aColumnHandle        ColumnHandle
 * @param[in]  aRegionMem           Region Memory
 * @param[out] aIndexList           Index List
 * @param[in]  aEnv                 Environment
 * @remarks
 */
stlStatus ellGetIndexListByColumn( smlTransId       aTransID,
                                   smlStatement   * aStmt,
                                   ellDictHandle  * aColumnHandle,
                                   knlRegionMem   * aRegionMem,
                                   ellObjectList ** aIndexList,
                                   ellEnv         * aEnv )
{
    ellObjectList * sIndexList = NULL;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColumnHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIndexList != NULL, ELL_ERROR_STACK(aEnv) );


    /**
     * Column 과 관련된 Index 정보 획득
     * - SELECT INDEX_ID
     * - FROM DEFINITION_SCHEMA.INDEX_KEY_COLUMN_USAGE
     * - WHERE COLUMN_ID = ellGetColumnID(sObjectHandle)
     */
    
    STL_TRY( ellInitObjectList( & sIndexList, aRegionMem, aEnv ) == STL_SUCCESS );
    
    STL_TRY( eldGetObjectList( aTransID,
                               aStmt,
                               ELDT_TABLE_ID_INDEX_KEY_COLUMN_USAGE,
                               ELL_OBJECT_INDEX,
                               ELDT_IndexKey_ColumnOrder_INDEX_ID,
                               ELDT_IndexKey_ColumnOrder_COLUMN_ID,
                               ellGetColumnID(aColumnHandle),
                               aRegionMem,
                               sIndexList,
                               aEnv )
             == STL_SUCCESS );

    /**
     * Output 설정
     */

    *aIndexList = sIndexList;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}




/************************************************************
 * Column 에 대한 DDL Lock 함수 
 ************************************************************/

/**
 * @brief ALTER TABLE .. ADD COLUMN 구문을 위한 DDL Lock 을 획득한다.
 * @param[in]  aTransID          Transaction ID
 * @param[in]  aStmt             Statement
 * @param[in]  aTableDictHandle  Table Dictionary Handle
 * @param[in]  aConstSchemaList  Constraint Schema Handle List
 * @param[in]  aConstSpaceList   Constraint Tablespace Handle List
 * @param[out] aLockSuccess      Lock 획득 여부 
 * @param[in]  aEnv              Environment
 * @remarks
 */
stlStatus ellLock4AddColumn( smlTransId      aTransID,
                             smlStatement  * aStmt,
                             ellDictHandle * aTableDictHandle,
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
    
    STL_PARAM_VALIDATE( aTableDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstSchemaList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstSpaceList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aLockSuccess != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Table 객체에 X lock
     */
    
    STL_TRY( eldLockTable4DDL( aTransID,
                               aStmt,
                               SML_LOCK_X,
                               aTableDictHandle,
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
 * @brief ALTER TABLE .. DROP COLUMN 구문을 위한 DDL Lock 을 획득한다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aTableDictHandle Table Dictionary Handle
 * @param[out] aLockSuccess     Lock 획득 여부 
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus ellLock4DropColumn( smlTransId      aTransID,
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
 * @brief ALTER TABLE .. ALTER COLUMN 구문을 위한 DDL Lock 을 획득한다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aTableDictHandle Table Dictionary Handle
 * @param[out] aLockSuccess     Lock 획득 여부 
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus ellLock4AlterColumn( smlTransId      aTransID,
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
 * @brief ALTER TABLE .. RENAME COLUMN 구문을 위한 DDL Lock 을 획득한다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aTableDictHandle Table Dictionary Handle
 * @param[out] aLockSuccess     Lock 획득 여부 
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus ellLock4RenameColumn( smlTransId      aTransID,
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



/************************************************************
 * Column 관련 객체에 대한 DDL Lock 함수 
 ************************************************************/




/************************************************************
 * DDL 관련 Index 획득 함수
 ************************************************************/





/************************************************************
 * Dictionary 변경 함수 
 ************************************************************/


/**
 * @brief Column Descriptor 를 Dictionary 에 추가한다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aOwnerID         Column 이 속한 Table 의 Owner ID
 * @param[in]  aSchemaID        Column 이 속한 Schema 의 ID
 * @param[in]  aTableID         Column 이 속한 Table 의 ID
 * @param[in]  aTableType       Table Type
 * @param[out] aColumnID        Column 의 ID 
 * @param[in]  aColumnName      Column Name (null-terminated)
 * @param[in]  aPhysicalOrdinalPosition Table 내의 Column 순서 w/ unused column (0부터 시작, DB에는 1부터 저장됨)
 * @param[in]  aLogicalOrdinalPosition  Table 내의 Column 순서 w/o unused column (0부터 시작, DB에는 1부터 저장됨)
 * @param[in]  aColumnDefault   Column Default 값 String (nullable)
 * @param[in]  aIsNullable      NULL 값 가능 여부 
 * @param[in]  aIsIdentity      IDENTITY 컬럼 여부
 *                         <BR> STL_FALSE일 경우 다음 입력 인자들은 무시됨
 *                         <BR> - aIdentityGenOption
 *                         <BR> - aIdentityStart
 *                         <BR> - aIdentityIncrement
 *                         <BR> - aIdentityMaximum
 *                         <BR> - aIdentityMinimum
 *                         <BR> - aIdentityCycle
 *                         <BR> - aIdentityTablespaceID
 *                         <BR> - aIdentityPhysicalID
 *                         <BR> - aIdentityCacheSize
 * @param[in]  aIdentityGenOption IDENTITY 컬럼의 생성 옵션
 *                         <BR> - ELL_IDENTITY_GENERATION_ALWAYS
 *                         <BR> - ELL_IDENTITY_GENERATION_BY_DEFAULT
 *                         <BR> - 사용자 입력값이 없다면, ELL_IDENTITY_GENERATION_NOT_AVAILABLE
 * @param[in]  aIdentityStart     IDENTITY 컬럼을 위해 생성한 Sequence 객체의 시작값
 *                         <BR> Sequence 객체와 관련 정보의 관리 주체는 Storage Manager이다.
 *                         <BR> 사용자 입력값이 없더라도 Sequence생성시 이러한 값들이 관리 주체에 의해
 *                         <BR> 설정되며, 설정된 값을 획득하여 입력 인자로 주어야 한다.
 *                         <BR> 정보의 관리 주체가 Dictionary가 아닌 경우,
 *                         <BR> Dictionary Table에서 그 정보를 관리하지 않고 있으나,
 *                         <BR> 표준에서 정의한 컬럼에 대한 값을 굳이 NULL로 설정할 필요가 없어
 *                         <BR> 예외적으로 입력을 받는다.
 *                         <BR> - aIdentityStart
 *                         <BR> - aIdentityIncrement
 *                         <BR> - aIdentityMaximum
 *                         <BR> - aIdentityMinimum
 *                         <BR> - aIdentityCycle
 *                         <BR> - aIdentityCacheSize
 * @param[in]  aIdentityIncrement IDENTITY 컬럼을 위해 생성한 Sequence 객체의 증가값
 * @param[in]  aIdentityMaximum   IDENTITY 컬럼을 위해 생성한 Sequence 객체의 최대값
 * @param[in]  aIdentityMinimum   IDENTITY 컬럼을 위해 생성한 Sequence 객체의 최소값 
 * @param[in]  aIdentityCycle     IDENTITY 컬럼을 위해 생성한 Sequence 객체의 Cycle 허용 여부
 * @param[in]  aIdentityTablespaceID  IDENTITY 객체의 Tablespace ID
 * @param[in]  aIdentityPhysicalID    IDENTITY 객체의 Physical ID
 * @param[in]  aIdentityCacheSize IDENTITY 컬럼을 위해 생성한 Sequence 객체의 Cache Size
 * @param[in]  aIsUpdatable       컬럼의 갱신 가능 여부 
 * @param[in]  aColumnComment     Column Comment (nullable)
 * @param[in]  aEnv               Execution Library Environment
 * @remarks
 */

stlStatus ellInsertColumnDesc( smlTransId           aTransID,
                               smlStatement       * aStmt,
                               stlInt64             aOwnerID,
                               stlInt64             aSchemaID,
                               stlInt64             aTableID,
                               ellTableType         aTableType,
                               stlInt64           * aColumnID,
                               stlChar            * aColumnName,
                               stlInt32             aPhysicalOrdinalPosition,
                               stlInt32             aLogicalOrdinalPosition,
                               stlChar            * aColumnDefault,
                               stlBool              aIsNullable,
                               stlBool              aIsIdentity,
                               ellIdentityGenOption aIdentityGenOption,
                               stlInt64             aIdentityStart,
                               stlInt64             aIdentityIncrement,
                               stlInt64             aIdentityMaximum,
                               stlInt64             aIdentityMinimum,
                               stlBool              aIdentityCycle,
                               stlInt64             aIdentityTablespaceID,
                               stlInt64             aIdentityPhysicalID,
                               stlInt64             aIdentityCacheSize,
                               stlBool              aIsUpdatable,
                               stlChar            * aColumnComment,
                               ellEnv             * aEnv )
{
    /**
     * Table Owner 가 소유하는 Column Privilege 들
     */
    
    ellColumnPrivAction   sOwnerPriv[] =
        {
            ELL_COLUMN_PRIV_ACTION_SELECT,             /**< SELECT column privilege */
            ELL_COLUMN_PRIV_ACTION_INSERT,             /**< INSERT column privilege */
            ELL_COLUMN_PRIV_ACTION_UPDATE,             /**< UPDATE column privilege */
            ELL_COLUMN_PRIV_ACTION_REFERENCES,         /**< REFERENCES column privilege */
            
            ELL_COLUMN_PRIV_ACTION_NA
        };

    stlInt32   i = 0;

    /**
     * Column Dictionary Record 추가
     */
    
    STL_TRY( eldInsertColumnDesc( aTransID,
                                  aStmt,
                                  aOwnerID,
                                  aSchemaID,
                                  aTableID,
                                  aColumnID,
                                  aColumnName,
                                  aPhysicalOrdinalPosition,
                                  aLogicalOrdinalPosition,
                                  aColumnDefault,
                                  aIsNullable,
                                  aIsIdentity,
                                  aIdentityGenOption,
                                  aIdentityStart,
                                  aIdentityIncrement,
                                  aIdentityMaximum,
                                  aIdentityMinimum,
                                  aIdentityCycle,
                                  aIdentityTablespaceID,
                                  aIdentityPhysicalID,
                                  aIdentityCacheSize,
                                  aIsUpdatable,
                                  aColumnComment,
                                  aEnv )
             == STL_SUCCESS );

    /**
     * Column Privilege 추가
     */

    if ( aTableType == ELL_TABLE_TYPE_BASE_TABLE )
    {
        i = 0;
        
        while ( 1 )
        {
            if ( sOwnerPriv[i] == ELL_COLUMN_PRIV_ACTION_NA )
            {
                break;
            }
            else
            {
                STL_TRY( ellInsertColumnPrivDesc( aTransID,
                                                  aStmt,
                                                  ELDT_AUTH_ID_SYSTEM,  /* Grantor */
                                                  aOwnerID,             /* Grantee */
                                                  aSchemaID,
                                                  aTableID,
                                                  *aColumnID,
                                                  sOwnerPriv[i],        /* Priv Action */
                                                  STL_TRUE,             /* sWithGrant */
                                                  aEnv )
                         == STL_SUCCESS );
            }
            
            i++;
        }
    }
    else
    {
        /**
         * Column Privilege 는 BASE TABLE 에만 적용할 수 있다.
         */
    }
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}



/**
 * @brief Column Dictionary 의 Column Name 을 변경한다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aColumnID        Column 의 ID 
 * @param[in]  aNewName         Column 의 New Name
 * @param[in]  aEnv             Execution Library Environment
 * @remarks
 */
stlStatus ellModifyColumnName( smlTransId     aTransID,
                               smlStatement * aStmt,
                               stlInt64       aColumnID,
                               stlChar      * aNewName,
                               ellEnv       * aEnv )
{
    /**
     * Column Name 변경 
     */
    
    STL_TRY( eldModifyDictDesc( aTransID,
                                aStmt,
                                ELDT_TABLE_ID_COLUMNS,
                                ELDT_Columns_ColumnOrder_COLUMN_ID,
                                aColumnID,
                                ELDT_Columns_ColumnOrder_COLUMN_NAME,
                                aNewName,  
                                aEnv )
             == STL_SUCCESS );

    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}



/**
 * @brief Column 의 DEFAULT 를 변경한다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aColumnID        Column 의 ID
 * @param[in]  aDefaultString   DEFAULT string (nullable)
 * @param[in]  aEnv             Execution Library Environment
 * @remarks
 */
stlStatus ellModifyColumnDefault( smlTransId     aTransID,
                                  smlStatement * aStmt,
                                  stlInt64       aColumnID,
                                  stlChar      * aDefaultString,
                                  ellEnv       * aEnv )
{
    /**
     * Column 의 DEFAULT 변경 
     */

    STL_TRY( eldModifyDictDesc( aTransID,
                                aStmt,
                                ELDT_TABLE_ID_COLUMNS,
                                ELDT_Columns_ColumnOrder_COLUMN_ID,
                                aColumnID,
                                ELDT_Columns_ColumnOrder_COLUMN_DEFAULT,
                                aDefaultString,
                                aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}

/**
 * @brief Column 을 Nullable 로 설정한다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aColumnID        Column 의 ID
 * @param[in]  aIsNullable      IS NULLABLE
 * @param[in]  aEnv             Execution Library Environment
 * @remarks
 */
stlStatus ellModifyColumnNullable( smlTransId     aTransID,
                                   smlStatement * aStmt,
                                   stlInt64       aColumnID,
                                   stlBool        aIsNullable,
                                   ellEnv       * aEnv )
{
    /**
     * Column 의 NULLABLE 여부 변경 
     */

    STL_TRY( eldModifyDictDesc( aTransID,
                                aStmt,
                                ELDT_TABLE_ID_COLUMNS,
                                ELDT_Columns_ColumnOrder_COLUMN_ID,
                                aColumnID,
                                ELDT_Columns_ColumnOrder_IS_NULLABLE,
                                & aIsNullable,
                                aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}





/**
 * @brief Column 의 Dictionary Record 를 UNUSED 로 설정한다.
 * @param[in] aTransID          aTransaction ID
 * @param[in] aStmt             Statement
 * @param[in] aColumnHandle     Column Dictionary Handle
 * @param[in] aEnv              Environemnt
 * @remarks
 */
stlStatus ellModifyColumnSetUnused( smlTransId        aTransID,
                                    smlStatement    * aStmt,
                                    ellDictHandle   * aColumnHandle,
                                    ellEnv          * aEnv )
{
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColumnHandle != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Column Privilege Record 를 삭제
     */
    
    /*
     * DELETE FROM DEFINITION_SCHEMA.COLUMN_PRIVILEGES
     *  WHERE COLUMN_ID = sTableID;
     */
    
    STL_TRY( eldDeletePrivRowsAndCache( aTransID,
                                        aStmt,
                                        ELDT_TABLE_ID_COLUMN_PRIVILEGES,
                                        ELDT_ColumnPriv_ColumnOrder_COLUMN_ID,
                                        ellGetColumnID(aColumnHandle),
                                        aEnv )
             == STL_SUCCESS );
        
    /**
     * Column Dictionary Record 를 UNUSED 상태로 변경
     * - Column Name 을 NULL 로 설정
     * - IS_UNUSED 를 TRUE 로 설정
     * - LOGICAL_ORDINARY_POSITION 을 NULL 로 설정
     */
    
    STL_TRY( eldModifyColumnSetUnused( aTransID,
                                       aStmt,
                                       ellGetColumnID(aColumnHandle),
                                       aEnv )
             == STL_SUCCESS );

    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
    
}


/**
 * @brief Column 의 Logical Ordinal Position 을 변경한다.
 * @param[in] aTransID          aTransaction ID
 * @param[in] aStmt             Statement
 * @param[in] aColumnHandle     Column Dictionary Handle
 * @param[in] aLogicalOrdinalPosition  Logical Ordinal Position
 * @param[in] aSetNullable      Column 을 Nullable 로 변경할 지 여부 
 * @param[in] aEnv              Environemnt
 * @remarks
 */
stlStatus ellModifyColumnLogicalIdx( smlTransId        aTransID,
                                     smlStatement    * aStmt,
                                     ellDictHandle   * aColumnHandle,
                                     stlInt32          aLogicalOrdinalPosition,
                                     stlBool           aSetNullable,
                                     ellEnv          * aEnv )
{
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColumnHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aLogicalOrdinalPosition >= 0, ELL_ERROR_STACK(aEnv) );

    /**
     * Column 의 Logical Ordinal Position 변경 
     */

    STL_TRY( eldModifyColumnLogicalIdx( aTransID,
                                        aStmt,
                                        ellGetColumnID( aColumnHandle ),
                                        aLogicalOrdinalPosition,
                                        aSetNullable,
                                        aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
    
}


/**
 * @brief Column 의 Dictionary Record 에서 Identity 속성을 제거한다.
 * @param[in] aTransID          aTransaction ID
 * @param[in] aStmt             Statement
 * @param[in] aColumnID         Column ID
 * @param[in] aIsNullable       Nullable 여부
 * @param[in] aEnv              Environment
 */
stlStatus ellModifyColumnDropIdentity( smlTransId     aTransID,
                                       smlStatement * aStmt,
                                       stlInt64       aColumnID,
                                       stlBool        aIsNullable,
                                       ellEnv       * aEnv )
{
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColumnID > ELL_DICT_OBJECT_ID_NA, ELL_ERROR_STACK(aEnv) );

    /**
     * Column 의 Identity Property 제거 
     */

    STL_TRY( eldRemoveColumnIdentity( aTransID,
                                      aStmt,
                                      aColumnID,
                                      aIsNullable,
                                      aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/**
 * @brief Column 의 Dictionary Record 에서 Identity 속성을 변경한다.
 * @param[in]  aTransID               aTransaction ID
 * @param[in]  aStmt                  Statement
 * @param[in]  aColumnID              Column ID
 * @param[in]  aIdentityGenOption     (nullable) IDENTITY 컬럼의 생성 옵션 
 * @param[in]  aIdentityStart         (nullable) IDENTITY 컬럼을 위해 생성한 Sequence 객체의 시작값
 * @param[in]  aIdentityIncrement     (nullable) IDENTITY 컬럼을 위해 생성한 Sequence 객체의 증가값
 * @param[in]  aIdentityMaximum       (nullable) IDENTITY 컬럼을 위해 생성한 Sequence 객체의 최대값
 * @param[in]  aIdentityMinimum       (nullable) IDENTITY 컬럼을 위해 생성한 Sequence 객체의 최소값 
 * @param[in]  aIdentityCycle         (nullable) IDENTITY 컬럼을 위해 생성한 Sequence 객체의 Cycle 허용 여부
 * @param[in]  aIdentityPhysicalID    IDENTITY 객체의 Physical ID
 * @param[in]  aIdentityCacheSize     (nullable) IDENTITY 컬럼을 위해 생성한 Sequence 객체의 Cache Size
 * @param[in]  aEnv                   Execution Library Environment
 * @remarks
 */
stlStatus ellModifyColumnAlterIdentity( smlTransId             aTransID,
                                        smlStatement         * aStmt,
                                        stlInt64               aColumnID,
                                        ellIdentityGenOption * aIdentityGenOption,
                                        stlInt64             * aIdentityStart,
                                        stlInt64             * aIdentityIncrement,
                                        stlInt64             * aIdentityMaximum,
                                        stlInt64             * aIdentityMinimum,
                                        stlBool              * aIdentityCycle,
                                        stlInt64             * aIdentityPhysicalID,
                                        stlInt64             * aIdentityCacheSize,
                                        ellEnv               * aEnv )
{
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColumnID > ELL_DICT_OBJECT_ID_NA, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIdentityPhysicalID != NULL, ELL_ERROR_STACK(aEnv) );


    /**
     * Column Identity 속성을 변경
     */

    STL_TRY( eldModifyColumnIdentity( aTransID,
                                      aStmt,
                                      aColumnID,
                                      aIdentityGenOption,
                                      aIdentityStart,
                                      aIdentityIncrement,
                                      aIdentityMaximum,
                                      aIdentityMinimum,
                                      aIdentityCycle,
                                      aIdentityPhysicalID,
                                      aIdentityCacheSize,
                                      aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/************************************************************
 * Cache 재구축 함수
 ************************************************************/

/**
 * @brief ALTER TABLE .. ADD COLUMN 에 대한 Cache 를 재구성
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aTableDictHandle Table Dictionary Handle
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus ellRefineCache4AddColumn( smlTransId        aTransID,
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
                                      ELL_SUPP_LOG_ADD_COLUMN,
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


stlStatus ellRefineCache4DropColumn( smlTransId        aTransID,
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
                                      ELL_SUPP_LOG_DROP_COLUMN,
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
 * @brief ALTER TABLE .. ALTER COLUMN 에 대한 Cache 를 재구성
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aTableDictHandle Table Dictionary Handle
 * @param[in]  aAlterColumnSuppLog  Alter Column Supplemental DDL Log
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus ellRefineCache4AlterColumn( smlTransId           aTransID,
                                      smlStatement       * aStmt,
                                      ellDictHandle      * aTableDictHandle,
                                      ellSuppLogTableDDL   aAlterColumnSuppLog,
                                      ellEnv             * aEnv )
{
    stlInt64  sTableID = ELL_DICT_OBJECT_ID_NA;

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aAlterColumnSuppLog >= ELL_SUPP_LOG_ALTER_COLUMN_SET_DEFAULT) &&
                        (aAlterColumnSuppLog <= ELL_SUPP_LOG_ALTER_COLUMN_DROP_IDENTITY),
                        ELL_ERROR_STACK(aEnv) );
    
    /**
     * Table ID 획득
     */
    
    sTableID = ellGetTableID( aTableDictHandle );

    /**
     * DDL Supplemental Logging 기록
     */

    STL_TRY( eldWriteSuppLogTableDDL( aTransID,
                                      sTableID,
                                      aAlterColumnSuppLog,
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
 * @brief ALTER TABLE .. RENAME COLUMN 에 대한 Cache 를 재구성
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aTableDictHandle Table Dictionary Handle
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus ellRefineCache4RenameColumn( smlTransId        aTransID,
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
                                      ELL_SUPP_LOG_RENAME_COLUMN,
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



/************************************************************
 * Handle 획득 함수 
 ************************************************************/


/**
 * @brief Column Dictionary Handle 을 얻는다.
 * @param[in]  aTransID           Transaction ID
 * @param[in]  aViewSCN           View SCN
 * @param[in]  aTableDictHandle   Table Dictionary Handle
 * @param[in]  aColumnName        Column Name
 * @param[out] aColumnDictHandle  Column Dictionary Handle
 * @param[out] aExist             존재 여부 
 * @param[in]  aEnv               Envirionment 
 * @remarks
 */
stlStatus ellGetColumnDictHandle( smlTransId           aTransID,
                                  smlScn               aViewSCN,
                                  ellDictHandle      * aTableDictHandle,
                                  stlChar            * aColumnName,
                                  ellDictHandle      * aColumnDictHandle,
                                  stlBool            * aExist,
                                  ellEnv             * aEnv )
{
    ellTableDesc * sTableDesc = NULL;

    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aTableDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColumnName != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColumnDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExist != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Table Descriptor
     */

    sTableDesc = (ellTableDesc *) ellGetObjectDesc( aTableDictHandle );

    /**
     * Column Handle 획득
     */

    ellInitDictHandle( aColumnDictHandle );
    
    STL_TRY( eldGetColumnHandleByName( aTransID,
                                       aViewSCN,
                                       sTableDesc->mTableID,
                                       aColumnName,
                                       aColumnDictHandle,
                                       aExist,
                                       aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Column ID 를 이용해 Column Dictionary Handle 을 얻는다.
 * @param[in]  aTransID           Transaction ID
 * @param[in]  aViewSCN           View SCN
 * @param[in]  aColumnID          Column ID
 * @param[out] aColumnDictHandle  Column Dictionary Handle
 * @param[out] aExist             존재 여부 
 * @param[in]  aEnv               Envirionment 
 * @remarks
 */
stlStatus ellGetColumnDictHandleByID( smlTransId           aTransID,
                                      smlScn               aViewSCN,
                                      stlInt64             aColumnID,
                                      ellDictHandle      * aColumnDictHandle,
                                      stlBool            * aExist,
                                      ellEnv             * aEnv )
{
    return eldGetColumnHandleByID( aTransID,
                                   aViewSCN,
                                   aColumnID,
                                   aColumnDictHandle,
                                   aExist,
                                   aEnv );
}




/************************************************************
 * 정보 획득 함수 
 ************************************************************/

                                 


/**
 * @brief Column ID 정보
 * @param[in] aColumnDictHandle  Column Dictionary Handle
 * @return Column ID
 * @remarks
 */
stlInt64 ellGetColumnID( ellDictHandle * aColumnDictHandle )
{
    ellColumnDesc * sColumnDesc = NULL;

    STL_DASSERT( aColumnDictHandle->mObjectType == ELL_OBJECT_COLUMN );
    
    sColumnDesc = (ellColumnDesc *) ellGetObjectDesc( aColumnDictHandle );

    return sColumnDesc->mColumnID;
}

/**
 * @brief Column Owner ID 정보
 * @param[in] aColumnDictHandle  Column Dictionary Handle
 * @return Column Owner ID
 * @remarks
 */
stlInt64 ellGetColumnOwnerID( ellDictHandle * aColumnDictHandle )
{
    ellColumnDesc * sColumnDesc = NULL;

    STL_DASSERT( aColumnDictHandle->mObjectType == ELL_OBJECT_COLUMN );
    
    sColumnDesc = (ellColumnDesc *) ellGetObjectDesc( aColumnDictHandle );

    return sColumnDesc->mOwnerID;
}


/**
 * @brief Column Schema ID 정보
 * @param[in] aColumnDictHandle  Column Dictionary Handle
 * @return Column Schema ID
 * @remarks
 */
stlInt64 ellGetColumnSchemaID( ellDictHandle * aColumnDictHandle )
{
    ellColumnDesc * sColumnDesc = NULL;

    STL_DASSERT( aColumnDictHandle->mObjectType == ELL_OBJECT_COLUMN );
    
    sColumnDesc = (ellColumnDesc *) ellGetObjectDesc( aColumnDictHandle );

    return sColumnDesc->mSchemaID;
}

/**
 * @brief Column 의 Table ID 정보
 * @param[in] aColumnDictHandle  Column Dictionary Handle
 * @return Column Table ID
 * @remarks
 */
stlInt64 ellGetColumnTableID( ellDictHandle * aColumnDictHandle )
{
    ellColumnDesc * sColumnDesc = NULL;

    STL_DASSERT( aColumnDictHandle->mObjectType == ELL_OBJECT_COLUMN );
    
    sColumnDesc = (ellColumnDesc *) ellGetObjectDesc( aColumnDictHandle );

    return sColumnDesc->mTableID;
}

/**
 * @brief Column Name 정보
 * @param[in] aColumnDictHandle  Column Dictionary Handle
 * @return Column Name
 * @remarks
 */
stlChar * ellGetColumnName( ellDictHandle * aColumnDictHandle )
{
    ellColumnDesc * sColumnDesc = NULL;

    STL_DASSERT( aColumnDictHandle->mObjectType == ELL_OBJECT_COLUMN );
    
    sColumnDesc = (ellColumnDesc *) ellGetObjectDesc( aColumnDictHandle );

    return sColumnDesc->mColumnName;
}

/**
 * @brief Column 위치 정보
 * @param[in] aColumnDictHandle  Column Dictionary Handle
 * @return Column Position (0 부터 시작)
 * @remarks
 */
stlInt32  ellGetColumnIdx( ellDictHandle * aColumnDictHandle )
{
    ellColumnDesc * sColumnDesc = NULL;

    STL_DASSERT( aColumnDictHandle->mObjectType == ELL_OBJECT_COLUMN );
    
    sColumnDesc = (ellColumnDesc *) ellGetObjectDesc( aColumnDictHandle );

    return sColumnDesc->mOrdinalPosition;
}


/**
 * @brief Column 의 Default Value String 정보 
 * @param[in] aColumnDictHandle  Column Dictionary Handle
 * @return Default Value String
 * @remarks
 */
stlChar * ellGetColumnDefaultValue( ellDictHandle * aColumnDictHandle )
{
    ellColumnDesc * sColumnDesc = NULL;

    STL_DASSERT( aColumnDictHandle->mObjectType == ELL_OBJECT_COLUMN );
    
    sColumnDesc = (ellColumnDesc *) ellGetObjectDesc( aColumnDictHandle );

    return sColumnDesc->mDefaultString;
}


/**
 * @brief Column 의 Nullable 여부 
 * @param[in] aColumnDictHandle  Column Dictionary Handle
 * @return Boolean [Nullable : TRUE, Not Null : FALSE]
 * @remarks
 */
stlBool   ellGetColumnNullable( ellDictHandle * aColumnDictHandle )
{
    ellColumnDesc * sColumnDesc = NULL;

    STL_DASSERT( aColumnDictHandle->mObjectType == ELL_OBJECT_COLUMN );
    
    sColumnDesc = (ellColumnDesc *) ellGetObjectDesc( aColumnDictHandle );

    return sColumnDesc->mNullable;
}

/**
 * @brief Column 의 Updatable 여부 
 * @param[in] aColumnDictHandle  Column Dictionary Handle
 * @return stlBool
 * @remarks
 */
stlBool   ellGetColumnUpdatable( ellDictHandle * aColumnDictHandle )
{
    ellColumnDesc * sColumnDesc = NULL;

    STL_DASSERT( aColumnDictHandle->mObjectType == ELL_OBJECT_COLUMN );
    
    sColumnDesc = (ellColumnDesc *) ellGetObjectDesc( aColumnDictHandle );

    return sColumnDesc->mUpdatable;
}


/**
 * @brief Column 의 Unused 여부 
 * @param[in] aColumnDictHandle  Column Dictionary Handle
 * @return stlBool
 * @remarks
 */
stlBool   ellGetColumnUnused( ellDictHandle * aColumnDictHandle )
{
    ellColumnDesc * sColumnDesc = NULL;

    STL_DASSERT( aColumnDictHandle->mObjectType == ELL_OBJECT_COLUMN );
    
    sColumnDesc = (ellColumnDesc *) ellGetObjectDesc( aColumnDictHandle );

    return sColumnDesc->mUnused;
}


/**
 * @brief Column 의 Idnentity Handle
 * @param[in] aColumnDictHandle  Column Dictionary Handle
 * @return IF Identity Column THEN identity handle ELSE NULL
 * @remarks
 */
void * ellGetColumnIdentityHandle( ellDictHandle * aColumnDictHandle )
{
    ellColumnDesc * sColumnDesc = NULL;

    STL_DASSERT( aColumnDictHandle->mObjectType == ELL_OBJECT_COLUMN );
    
    sColumnDesc = (ellColumnDesc *) ellGetObjectDesc( aColumnDictHandle );

    return sColumnDesc->mIdentityHandle;
}

/**
 * @brief Column 의 Idnentity Handle 의 Physical ID
 * @param[in] aColumnDictHandle  Column Dictionary Handle
 * @return Identity Column 의 Physical ID
 * @remarks
 * Idnentity Column 이어야 함
 */
stlInt64 ellGetColumnIdentityPhysicalID( ellDictHandle * aColumnDictHandle )
{
    ellColumnDesc * sColumnDesc = NULL;

    STL_DASSERT( aColumnDictHandle->mObjectType == ELL_OBJECT_COLUMN );
    
    sColumnDesc = (ellColumnDesc *) ellGetObjectDesc( aColumnDictHandle );

    return sColumnDesc->mIdentityPhyID;
}

/**
 * @brief Column 의 Idnentity Generation Option
 * @param[in] aColumnDictHandle  Column Dictionary Handle
 * @return ellIdentityGenOption
 * @remarks
 */
ellIdentityGenOption ellGetColumnIdentityGenOption( ellDictHandle * aColumnDictHandle )
{
    ellColumnDesc * sColumnDesc = NULL;

    STL_DASSERT( aColumnDictHandle->mObjectType == ELL_OBJECT_COLUMN );
    
    sColumnDesc = (ellColumnDesc *) ellGetObjectDesc( aColumnDictHandle );

    return sColumnDesc->mIdentityGenOption;
}

/**
 * @brief Column 의 DB Data Type
 * @param[in] aColumnDictHandle  Column Dictionary Handle
 * @return dtlDataType 
 * @remarks
 */
dtlDataType ellGetColumnDBType( ellDictHandle * aColumnDictHandle )
{
    ellColumnDesc * sColumnDesc = NULL;

    STL_DASSERT( aColumnDictHandle->mObjectType == ELL_OBJECT_COLUMN );
    
    sColumnDesc = (ellColumnDesc *) ellGetObjectDesc( aColumnDictHandle );

    return sColumnDesc->mDataType;
}
    

/**
 * @brief Column Dictinoary Handle 로부터 Precision 정보 획득
 * @param[in]   aColumnDictHandle  Column Dictionary Handle
 * @return
 * Column Precision(or Length)
 * @remarks
 */
stlInt64  ellGetColumnPrecision( ellDictHandle * aColumnDictHandle )
{
    stlInt64 sPrecision = 0;
    
    ellColumnDesc * sColumnDesc = NULL;
    
    STL_DASSERT( aColumnDictHandle->mObjectType == ELL_OBJECT_COLUMN );

    /**
     * Data Type 별 적절한 Precision 정보와 Scale 정보 획득
     */

    sColumnDesc = (ellColumnDesc *) ellGetObjectDesc( aColumnDictHandle );
    
    switch( sColumnDesc->mDataType )
    {
        case DTL_TYPE_BOOLEAN:
            {
                sPrecision = 0;
                break;
            }
        case DTL_TYPE_NATIVE_SMALLINT:
            {
                sPrecision = sColumnDesc->mColumnType.mBinaryNum.mBinaryPrec;
                break;
            }
        case DTL_TYPE_NATIVE_INTEGER:
            {
                sPrecision = sColumnDesc->mColumnType.mBinaryNum.mBinaryPrec;
                break;
            }
        case DTL_TYPE_NATIVE_BIGINT:
            {
                sPrecision = sColumnDesc->mColumnType.mBinaryNum.mBinaryPrec;
                break;
            }
        case DTL_TYPE_NATIVE_REAL:
            {
                sPrecision = sColumnDesc->mColumnType.mBinaryNum.mBinaryPrec;
                break;
            }
        case DTL_TYPE_NATIVE_DOUBLE:
            {
                sPrecision = sColumnDesc->mColumnType.mBinaryNum.mBinaryPrec;
                break;
            }
        case DTL_TYPE_FLOAT:
            {
                sPrecision = sColumnDesc->mColumnType.mDecimalNum.mPrec;
                break;
            }
        case DTL_TYPE_NUMBER:
            {
                sPrecision = sColumnDesc->mColumnType.mDecimalNum.mPrec;
                break;
            }
        case DTL_TYPE_DECIMAL:
            {
                STL_ASSERT( 0 );
                break;
            }
        case DTL_TYPE_CHAR:
            {
                sPrecision = sColumnDesc->mColumnType.mString.mCharLength;
                break;
            }
        case DTL_TYPE_VARCHAR:
        case DTL_TYPE_LONGVARCHAR:
            {
                sPrecision = sColumnDesc->mColumnType.mString.mCharLength;
                break;
            }
        case DTL_TYPE_CLOB:
            {
                STL_ASSERT( 0 );
                break;
            }
        case DTL_TYPE_BINARY:
            {
                sPrecision = sColumnDesc->mColumnType.mBinary.mByteLength;
                break;
            }
        case DTL_TYPE_VARBINARY:
        case DTL_TYPE_LONGVARBINARY:
            {
                sPrecision = sColumnDesc->mColumnType.mBinary.mByteLength;
                break;
            }
        case DTL_TYPE_BLOB:
            {
                STL_ASSERT( 0 );
                break;
            }
        case DTL_TYPE_DATE:
            {
                sPrecision = 0;
                break;
            }
        case DTL_TYPE_TIME:
            {
                sPrecision = sColumnDesc->mColumnType.mDateTime.mFracSecPrec;
                break;
            }
        case DTL_TYPE_TIMESTAMP:
            {
                sPrecision = sColumnDesc->mColumnType.mDateTime.mFracSecPrec;
                break;
            }
        case DTL_TYPE_TIME_WITH_TIME_ZONE:
            {
                sPrecision = sColumnDesc->mColumnType.mDateTime.mFracSecPrec;
                break;
            }
        case DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE:
            {
                sPrecision = sColumnDesc->mColumnType.mDateTime.mFracSecPrec;
                break;
            }
        case DTL_TYPE_INTERVAL_YEAR_TO_MONTH:
            {
                sPrecision = sColumnDesc->mColumnType.mInterval.mStartPrec;
                break;
            }
        case DTL_TYPE_INTERVAL_DAY_TO_SECOND:
            {
                sPrecision = sColumnDesc->mColumnType.mInterval.mStartPrec;
                break;
            }
        case DTL_TYPE_ROWID:
            {
                sPrecision = 0;
                break;
            }
        default:
            {
                STL_ASSERT( 0 );
                break;
            }
    }

    /**
     * Output 설정
     */

    return sPrecision;
}


/**
 * @brief Column Dictinoary Handle 로부터 Scale 정보 획득
 * @param[in]   aColumnDictHandle  Column Dictionary Handle
 * @return
 * Column Scale
 * @remarks
 */
stlInt64  ellGetColumnScale( ellDictHandle * aColumnDictHandle )
{
    stlInt64 sScale = 0;

    ellColumnDesc * sColumnDesc = NULL;
    
    STL_DASSERT( aColumnDictHandle->mObjectType == ELL_OBJECT_COLUMN );

    /**
     * Data Type 별 적절한 Precision 정보와 Scale 정보 획득
     */

    sColumnDesc = (ellColumnDesc *) ellGetObjectDesc( aColumnDictHandle );
    
    switch( sColumnDesc->mDataType )
    {
        case DTL_TYPE_BOOLEAN:
            {
                sScale = 0;
                break;
            }
        case DTL_TYPE_NATIVE_SMALLINT:
            {
                sScale = 0;
                break;
            }
        case DTL_TYPE_NATIVE_INTEGER:
            {
                sScale = 0;
                break;
            }
        case DTL_TYPE_NATIVE_BIGINT:
            {
                sScale = 0;
                break;
            }
        case DTL_TYPE_NATIVE_REAL:
            {
                sScale = 0;
                break;
            }
        case DTL_TYPE_NATIVE_DOUBLE:
            {
                sScale = 0;
                break;
            }
        case DTL_TYPE_FLOAT:
        case DTL_TYPE_NUMBER:
            {
                sScale = sColumnDesc->mColumnType.mDecimalNum.mScale;
                break;
            }
        case DTL_TYPE_DECIMAL:
            {
                STL_ASSERT( 0 );
                break;
            }
        case DTL_TYPE_CHAR:
            {
                sScale = 0;
                break;
            }
        case DTL_TYPE_VARCHAR:
            {
                sScale = 0;
                break;
            }
        case DTL_TYPE_LONGVARCHAR:
            {
                sScale = 0;
                break;
            }
        case DTL_TYPE_CLOB:
            {
                sScale = 0;
                break;
            }
        case DTL_TYPE_BINARY:
            {
                sScale = 0;
                break;
            }
        case DTL_TYPE_VARBINARY:
            {
                sScale = 0;
                break;
            }
        case DTL_TYPE_LONGVARBINARY:
            {
                sScale = 0;
                break;
            }
        case DTL_TYPE_BLOB:
            {
                STL_ASSERT( 0 );
                break;
            }
        case DTL_TYPE_DATE:
            {
                sScale = 0;
                break;
            }
        case DTL_TYPE_TIME:
            {
                sScale = 0;
                break;
            }
        case DTL_TYPE_TIMESTAMP:
            {
                sScale = 0;
                break;
            }
        case DTL_TYPE_TIME_WITH_TIME_ZONE:
            {
                sScale = 0;
                break;
            }
        case DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE:
            {
                sScale = 0;
                break;
            }
        case DTL_TYPE_INTERVAL_YEAR_TO_MONTH:
            {
                sScale = sColumnDesc->mColumnType.mInterval.mEndPrec;
                break;
            }
        case DTL_TYPE_INTERVAL_DAY_TO_SECOND:
            {
                sScale = sColumnDesc->mColumnType.mInterval.mEndPrec;
                break;
            }
        case DTL_TYPE_ROWID:
            {
                sScale = 0;
                break;
            }
        default:
            {
                STL_ASSERT( 0 );
                break;
            }
    }

    /**
     * Output 설정
     */

    return sScale;
}

/**
 * @brief Column Dictinoary Handle 로부터 String length unit 정보 획득
 * @param[in]   aColumnDictHandle  Column Dictionary Handle
 * @return Column String length unit
 * @remarks
 */
dtlStringLengthUnit ellGetColumnStringLengthUnit( ellDictHandle * aColumnDictHandle )
{
    dtlStringLengthUnit  sStringLengthUnit = DTL_STRING_LENGTH_UNIT_NA;

    ellColumnDesc * sColumnDesc = NULL;
    
    STL_DASSERT( aColumnDictHandle->mObjectType == ELL_OBJECT_COLUMN );

    /**
     * Data Type 별 적절한 String length unit 정보 획득
     */

    sColumnDesc = (ellColumnDesc *) ellGetObjectDesc( aColumnDictHandle );
    
    switch( sColumnDesc->mDataType )
    {
        case DTL_TYPE_BOOLEAN:
            {
                sStringLengthUnit = DTL_STRING_LENGTH_UNIT_NA;
                break;
            }
        case DTL_TYPE_NATIVE_SMALLINT:
            {
                sStringLengthUnit = DTL_STRING_LENGTH_UNIT_NA;
                break;
            }
        case DTL_TYPE_NATIVE_INTEGER:
            {
                sStringLengthUnit = DTL_STRING_LENGTH_UNIT_NA;
                break;
            }
        case DTL_TYPE_NATIVE_BIGINT:
            {
                sStringLengthUnit = DTL_STRING_LENGTH_UNIT_NA;
                break;
            }
        case DTL_TYPE_NATIVE_REAL:
            {
                sStringLengthUnit = DTL_STRING_LENGTH_UNIT_NA;
                break;
            }
        case DTL_TYPE_NATIVE_DOUBLE:
            {
                sStringLengthUnit = DTL_STRING_LENGTH_UNIT_NA;
                break;
            }
        case DTL_TYPE_FLOAT:
        case DTL_TYPE_NUMBER:
            {
                sStringLengthUnit = DTL_STRING_LENGTH_UNIT_NA;
                break;
            }
        case DTL_TYPE_DECIMAL:
            {
                STL_ASSERT( 0 );
                break;
            }
        case DTL_TYPE_CHAR:
        case DTL_TYPE_VARCHAR:
        case DTL_TYPE_LONGVARCHAR:
        case DTL_TYPE_CLOB:
            {
                sStringLengthUnit = sColumnDesc->mColumnType.mString.mLengthUnit;
                break;
            }
        case DTL_TYPE_BINARY:
        case DTL_TYPE_VARBINARY:
        case DTL_TYPE_LONGVARBINARY:
        case DTL_TYPE_BLOB:
            {
                sStringLengthUnit = DTL_STRING_LENGTH_UNIT_OCTETS;
                break;
            }
        case DTL_TYPE_DATE:
            {
                sStringLengthUnit = DTL_STRING_LENGTH_UNIT_NA;
                break;
            }
        case DTL_TYPE_TIME:
            {
                sStringLengthUnit = DTL_STRING_LENGTH_UNIT_NA;
                break;
            }
        case DTL_TYPE_TIMESTAMP:
            {
                sStringLengthUnit = DTL_STRING_LENGTH_UNIT_NA;
                break;
            }
        case DTL_TYPE_TIME_WITH_TIME_ZONE:
            {
                sStringLengthUnit = DTL_STRING_LENGTH_UNIT_NA;
                break;
            }
        case DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE:
            {
                sStringLengthUnit = DTL_STRING_LENGTH_UNIT_NA;
                break;
            }
        case DTL_TYPE_INTERVAL_YEAR_TO_MONTH:
            {
                sStringLengthUnit = DTL_STRING_LENGTH_UNIT_NA;
                break;
            }
        case DTL_TYPE_INTERVAL_DAY_TO_SECOND:
            {
                sStringLengthUnit = DTL_STRING_LENGTH_UNIT_NA;
                break;
            }
        case DTL_TYPE_ROWID:
            {
                sStringLengthUnit = DTL_STRING_LENGTH_UNIT_NA;
                break;               
            }
        default:
            {
                STL_ASSERT( 0 );
                break;
            }
    }

    /**
     * Output 설정
     */

    return sStringLengthUnit;
}


/**
 * @brief Column Dictinoary Handle 로부터 interval type indicator 정보 획득
 * @param[in]   aColumnDictHandle  Column Dictionary Handle
 * @return
 * Column interval type indicator
 * @remarks
 */
dtlIntervalIndicator ellGetColumnIntervalIndicator( ellDictHandle * aColumnDictHandle )
{
    dtlIntervalIndicator sIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;

    ellColumnDesc * sColumnDesc = NULL;
    
    STL_DASSERT( aColumnDictHandle->mObjectType == ELL_OBJECT_COLUMN );

    /**
     * Data Type 별 적절한 interval type indicator 정보 획득
     */

    sColumnDesc = (ellColumnDesc *) ellGetObjectDesc( aColumnDictHandle );
    
    switch( sColumnDesc->mDataType )
    {
        case DTL_TYPE_BOOLEAN:
            {
                sIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;
                break;
            }
        case DTL_TYPE_NATIVE_SMALLINT:
            {
                sIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;
                break;
            }
        case DTL_TYPE_NATIVE_INTEGER:
            {
                sIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;
                break;
            }
        case DTL_TYPE_NATIVE_BIGINT:
            {
                sIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;
                break;
            }
        case DTL_TYPE_NATIVE_REAL:
            {
                sIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;
                break;
            }
        case DTL_TYPE_NATIVE_DOUBLE:
            {
                sIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;
                break;
            }
        case DTL_TYPE_FLOAT:
        case DTL_TYPE_NUMBER:
            {
                sIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;
                break;
            }
        case DTL_TYPE_DECIMAL:
            {
                STL_ASSERT( 0 );
                break;
            }
        case DTL_TYPE_CHAR:
            {
                sIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;
                break;
            }
        case DTL_TYPE_VARCHAR:
            {
                sIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;
                break;
            }
        case DTL_TYPE_LONGVARCHAR:
            {
                sIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;
                break;
            }
        case DTL_TYPE_CLOB:
            {
                STL_ASSERT( 0 );
                break;
            }
        case DTL_TYPE_BINARY:
            {
                sIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;
                break;
            }
        case DTL_TYPE_VARBINARY:
            {
                sIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;
                break;
            }
        case DTL_TYPE_LONGVARBINARY:
            {
                sIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;
                break;
            }
        case DTL_TYPE_BLOB:
            {
                STL_ASSERT( 0 );
                break;
            }
        case DTL_TYPE_DATE:
            {
                sIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;
                break;
            }
        case DTL_TYPE_TIME:
            {
                sIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;
                break;
            }
        case DTL_TYPE_TIMESTAMP:
            {
                sIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;
                break;
            }
        case DTL_TYPE_TIME_WITH_TIME_ZONE:
            {
                sIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;
                break;
            }
        case DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE:
            {
                sIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;
                break;
            }
        case DTL_TYPE_INTERVAL_YEAR_TO_MONTH:
            {
                sIntervalIndicator = sColumnDesc->mColumnType.mInterval.mIntervalType;
                break;
            }
        case DTL_TYPE_INTERVAL_DAY_TO_SECOND:
            {
                sIntervalIndicator = sColumnDesc->mColumnType.mInterval.mIntervalType;
                break;
            }
        case DTL_TYPE_ROWID:
            {
                sIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;
                break;
            }
        default:
            {
                STL_ASSERT( 0 );
                break;
            }
    }

    /**
     * Output 설정
     */

    return sIntervalIndicator;
}




/**
 * @brief Column Dictinoary Handle 로부터 interval type indicator 정보 획득
 * @param[in]   aColumnDictHandle  Column Dictionary Handle
 * @return
 * Column Descriptor
 * @remarks
 */
ellColumnDesc * ellGetColumnDesc( ellDictHandle * aColumnDictHandle )
{
    ellColumnDesc * sColumnDesc = NULL;

    STL_DASSERT( aColumnDictHandle->mObjectType == ELL_OBJECT_COLUMN );

    sColumnDesc = (ellColumnDesc *) ellGetObjectDesc( aColumnDictHandle );

    return sColumnDesc;
}

/** @} ellColumn */
