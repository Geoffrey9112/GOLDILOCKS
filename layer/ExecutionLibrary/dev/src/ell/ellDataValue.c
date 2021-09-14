/*******************************************************************************
 * ellDataValue.c
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
 * @file ellDataValue.c
 * @brief Data Value Management
 */

#include <ell.h>



/**
 * @addtogroup ellTableValueList
 * @{
 */


/**
 * @brief Table Value List 에서 Column Idx 에 해당하는 Value List 를 찾는다.
 * @param[in] aTableValueList  Table Value Block List
 * @param[in] aTableID         Table ID
 * @param[in] aColumnIdx       Column Order (0-base)
 * @return
 * - Column 의 Value Block List
 * - 존재하지 않을 경우, NULL 리턴 
 * @remarks
 * - Dictionary Table에 대한 Table ID는 KNL_ANONYMOUS_TABLE_ID로 가정한다.
 */
knlValueBlockList * ellFindColumnValueList( knlValueBlockList * aTableValueList,
                                            stlInt64            aTableID,
                                            stlInt32            aColumnIdx )
{
    knlValueBlockList * sValueList = NULL;

    for ( sValueList = aTableValueList;
          sValueList != NULL;
          sValueList = sValueList->mNext )
    {
        if ( ( KNL_GET_BLOCK_TABLE_ID(sValueList) == aTableID ) &&
             ( KNL_GET_BLOCK_COLUMN_ORDER(sValueList) == aColumnIdx ) )
        {
            break;
        }
        else
        {
            continue;
        }
    }

    return sValueList;
}


/* /\** */
/*  * @brief Table Value List 에서 Column Idx 에 해당하는 Value List 를 찾는다. */
/*  * @param[in] aTableValueList  Table Value Block List */
/*  * @param[in] aTableID         Table ID */
/*  * @param[in] aColumnIdx       Column Order (0-base) */
/*  * @return */
/*  * - Column 의 Value Block List */
/*  * - 존재하지 않을 경우, NULL 리턴  */
/*  * @remarks */
/*  *\/ */
/* knlValueBlockList * ellFindColumnValueListUsingTableID( knlValueBlockList * aTableValueList, */
/*                                                         stlInt64            aTableID, */
/*                                                         stlInt32            aColumnIdx ) */
                                                        
/* { */
/*     knlValueBlockList * sValueList = NULL; */

/*     for ( sValueList = aTableValueList; */
/*           sValueList != NULL; */
/*           sValueList = sValueList->mNext ) */
/*     { */
/*         if ( KNL_GET_BLOCK_COLUMN_ORDER(sValueList) == aColumnIdx ) */
/*         { */
/*             break; */
/*         } */
/*         else */
/*         { */
/*             continue; */
/*         } */
/*     } */

/*     return sValueList; */
/* } */




/** @} ellTableValueList */













/**
 * @addtogroup ellIndexValueList
 * @{
 */

/**
 * @brief CREATE [UNIQUE] INDEX 구문을 위한 Index Insert Value List를 할당받는다.
 * @param[in]  aTransID               Transaction ID
 * @param[in]  aViewSCN               View SCN
 * @param[in]  aKeyColumnCnt          Key Column 의 개수
 * @param[in]  aKeyArrayColumnID      Key Column ID 의 Array(nullable, Cache 구축시 사용)
 * @param[in]  aNewKeyColumnPos       Key Column 의 Position (nullable, Cache 미구축시 사용) 
 * @param[in]  aNewColumnDesc         New Column Desc (nullable, Cache 미구축시 사용) 
 * @param[in]  aRegionMem             Region Memory
 * @param[out] aIndexInsertValueList  Index Insert Key Value List
 * @param[in]  aEnv                   Environment
 * @remarks
 * 컬럼 Cache 가 존재하는 경우
 * - aKeyArrayColumnID 
 * 컬럼 Cache 가 존재하지 않는 경우
 * - aNewKeyColumnPos
 * - aNewColumnDesc
 */
stlStatus ellAllocIndexValueListForCREATE( smlTransId           aTransID,
                                           smlScn               aViewSCN,
                                           stlInt32             aKeyColumnCnt,
                                           stlInt64           * aKeyArrayColumnID,
                                           stlInt32           * aNewKeyColumnPos,
                                           ellAddColumnDesc   * aNewColumnDesc,
                                           knlRegionMem       * aRegionMem,
                                           knlValueBlockList ** aIndexInsertValueList,
                                           ellEnv             * aEnv )
{
    stlInt32    i = 0;

    knlValueBlockList * sValueList = NULL;
    knlValueBlockList * sPrevValueList = NULL;
    knlValueBlockList * sIndexValueList = NULL;

    stlInt32        sColumnPos = 0;
    
    stlInt64        sColumnID = ELL_DICT_OBJECT_ID_NA;
    stlBool         sColumnExist = STL_FALSE;
    ellDictHandle   sColumnHandle;
    dtlDataType     sColumnType = DTL_TYPE_NA;
    
    stlMemset( & sColumnHandle, 0x00, STL_SIZEOF(ellDictHandle) );

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aKeyColumnCnt > 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIndexInsertValueList != NULL, ELL_ERROR_STACK(aEnv) );

    if ( aKeyArrayColumnID == NULL )
    {
        STL_PARAM_VALIDATE( aNewKeyColumnPos != NULL, ELL_ERROR_STACK(aEnv) );
        STL_PARAM_VALIDATE( aNewColumnDesc != NULL, ELL_ERROR_STACK(aEnv) );
    }
    else
    {
        STL_PARAM_VALIDATE( aNewKeyColumnPos == NULL, ELL_ERROR_STACK(aEnv) );
        STL_PARAM_VALIDATE( aNewColumnDesc == NULL, ELL_ERROR_STACK(aEnv) );
    }
    
    /**
     * Key Column Cnt 만큼 Index Value Block List 를 생성 
     */

    for ( i = 0; i < aKeyColumnCnt; i++ )
    {
        /**
         * Column Descriptor 획득
         */

        if ( aNewColumnDesc == NULL )
        {
            sColumnID = aKeyArrayColumnID[i];
            
            STL_TRY( ellGetColumnDictHandleByID( aTransID,
                                                 aViewSCN,
                                                 sColumnID,
                                                 & sColumnHandle,
                                                 & sColumnExist,
                                                 aEnv )
                     == STL_SUCCESS );
            STL_ASSERT( sColumnExist == STL_TRUE );
            
            /**
             * Value Block 을 생성 
             */

            sColumnType = ellGetColumnDBType( &sColumnHandle );
            
            STL_DASSERT( (sColumnType != DTL_TYPE_LONGVARCHAR) &&
                         (sColumnType != DTL_TYPE_CLOB) &&
                         (sColumnType != DTL_TYPE_LONGVARBINARY) &&
                         (sColumnType != DTL_TYPE_BLOB) );
            
            STL_TRY( knlInitBlock( & sValueList,
                                   KNL_NO_BLOCK_READ_CNT,
                                   STL_TRUE, /* 컬럼임 */
                                   STL_LAYER_EXECUTION_LIBRARY,
                                   ellGetColumnTableID( &sColumnHandle ),
                                   ellGetColumnIdx( &sColumnHandle ),
                                   sColumnType,
                                   ellGetColumnPrecision( &sColumnHandle ),
                                   ellGetColumnScale( &sColumnHandle ),
                                   ellGetColumnStringLengthUnit( &sColumnHandle ),
                                   ellGetColumnIntervalIndicator( &sColumnHandle ),
                                   aRegionMem,
                                   KNL_ENV(aEnv) )
                     == STL_SUCCESS );
        }
        else
        {
            sColumnPos = aNewKeyColumnPos[i];
            
            /**
             * Value Block 을 생성 
             */

            sColumnType = aNewColumnDesc[sColumnPos].mDataType;
            
            STL_DASSERT( (sColumnType != DTL_TYPE_LONGVARCHAR) &&
                         (sColumnType != DTL_TYPE_CLOB) &&
                         (sColumnType != DTL_TYPE_LONGVARBINARY) &&
                         (sColumnType != DTL_TYPE_BLOB) );
            
            STL_TRY( knlInitBlock( & sValueList,
                                   KNL_NO_BLOCK_READ_CNT,
                                   STL_TRUE, /* 컬럼임 */
                                   STL_LAYER_EXECUTION_LIBRARY,
                                   aNewColumnDesc[sColumnPos].mTableID,
                                   aNewColumnDesc[sColumnPos].mOrdinalPosition,
                                   aNewColumnDesc[sColumnPos].mDataType,
                                   aNewColumnDesc[sColumnPos].mArgNum1,
                                   aNewColumnDesc[sColumnPos].mArgNum2,
                                   aNewColumnDesc[sColumnPos].mStringLengthUnit,
                                   aNewColumnDesc[sColumnPos].mIntervalIndicator,
                                   aRegionMem,
                                   KNL_ENV(aEnv) )
                     == STL_SUCCESS );
            
        }

        /**
         * Link 설정
         */

        if ( sIndexValueList == NULL )
        {
            sIndexValueList = sValueList;
        }
        else
        {
            KNL_LINK_BLOCK_LIST( sPrevValueList, sValueList );
        }

        sPrevValueList = sValueList;
    }
    
    /**
     * Output 설정
     */

    *aIndexInsertValueList = sIndexValueList;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief DML 에 대한 Index Insert/Delete Key Value List 를 할당받는다.
 * @param[in]  aTableReadValueList      Table Read Value List (nullable)
 * @param[in]  aTableWriteValueList     Table Write Value List (nullable)
 * @param[in]  aIndexDictHandle         Index Dictionary Handle
 * @param[in]  aRegionMem               Region Memory 
 * @param[in]  aIndexDeleteValueList    Index Delete Key 를 위한 Value List (nullable)
 * @param[in]  aIndexInsertValueList    Index Insert Key 를 위한 Value List (nullable)
 * @param[in]  aEnv                     Environment
 * @remarks
 * DML 에 따라 다음과 같은 구성이 된다.
 * - INSERT 구문
 *  - Table Read Value List  : N/A
 *  - Table Write Value List : Exist
 *  - Index Delete Key Value List : N/A
 *  - Index Insert Key Value List : Exist
 * - DELETE 구문
 *  - Table Read Value List : Exist
 *  - Table Write Value List : N/A
 *  - Index Delete Key Value List : Exist
 *  - Index Insert Key Value List : N/A
 * - UPDATE 구문
 *  - Table Read Value List : Exist
 *  - Table Write Value List : Exist
 *  - Index Delete Key Value List : Exist
 *  - Index Insert Key Value List : Exist
 */
stlStatus ellShareIndexValueList( knlValueBlockList  * aTableReadValueList,
                                  knlValueBlockList  * aTableWriteValueList,
                                  ellDictHandle      * aIndexDictHandle,
                                  knlRegionMem       * aRegionMem,
                                  knlValueBlockList ** aIndexDeleteValueList,
                                  knlValueBlockList ** aIndexInsertValueList,
                                  ellEnv             * aEnv )
{
    stlInt32  i = 0;

    ellDictHandle   * sColumnHandle = NULL;
    
    stlInt32          sIndexKeyCnt = 0;
    ellIndexKeyDesc * sIndexKeyDesc = NULL;

    knlValueBlockList * sDeleteValueList = NULL;
    knlValueBlockList * sInsertValueList = NULL;

    knlValueBlockList * sValueList     = NULL;
    knlValueBlockList * sPrevValueList = NULL;

    knlValueBlockList * sColumnValueList = NULL;
    
    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aIndexDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Table 과 Index 정보 획득 
     */

    sIndexKeyCnt  = ellGetIndexKeyCount( aIndexDictHandle );
    sIndexKeyDesc = ellGetIndexKeyDesc( aIndexDictHandle );
    
    /**
     * Index Delete Key Value List 생성
     */

    if ( aIndexDeleteValueList == NULL )
    {
        sDeleteValueList = NULL;
    }
    else
    {
        /**
         * Index Delete Key 에 해당하는 Table Value List 를 찾아 연결함.
         */
        
        for ( i = 0; i < sIndexKeyCnt; i++ )
        {
            sColumnHandle = &sIndexKeyDesc[i].mKeyColumnHandle;
            
            /**
             * Index Insert Key 를 위한 Value Block 생성
             */
            
            sColumnValueList = ellFindColumnValueList( aTableReadValueList,
                                                       ellGetColumnTableID(sColumnHandle),
                                                       ellGetColumnIdx(sColumnHandle) );
            
            STL_TRY( knlInitShareBlock( & sValueList,
                                        sColumnValueList,
                                        aRegionMem,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            /**
             * Link 연결
             */

            if ( sDeleteValueList == NULL )
            {
                sDeleteValueList = sValueList;
            }
            else
            {
                KNL_LINK_BLOCK_LIST( sPrevValueList, sValueList );
            }

            sPrevValueList = sValueList;
        }
    }

    /**
     * Index Insert Key Value List 생성
     */

    if ( aIndexInsertValueList == NULL )
    {
        sInsertValueList = NULL;
    }
    else
    {
        /**
         * Index Insert Key 에 해당하는 Table Value List 를 찾아 연결함.
         */
        
        for ( i = 0; i < sIndexKeyCnt; i++ )
        {
            /**
             * Write Value List 에서 검색하고, 존재하지 않으면 Read Value List 에서 검색 
             */

            sColumnHandle = &sIndexKeyDesc[i].mKeyColumnHandle;
            sColumnValueList = ellFindColumnValueList( aTableWriteValueList,
                                                       ellGetColumnTableID(sColumnHandle),
                                                       ellGetColumnIdx(sColumnHandle) );
            if ( sColumnValueList == NULL )
            {
                /**
                 * Read Value List 에서 검색 
                 */
                
                sColumnValueList = ellFindColumnValueList( aTableReadValueList,
                                                           ellGetColumnTableID(sColumnHandle),
                                                           ellGetColumnIdx(sColumnHandle));
            }

            /**
             * Value Block 을 공유
             */
            
            STL_TRY( knlInitShareBlock( & sValueList,
                                        sColumnValueList,
                                        aRegionMem,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );
            
            /**
             * Link 연결
             */

            if ( sInsertValueList == NULL )
            {
                sInsertValueList = sValueList;
            }
            else
            {
                KNL_LINK_BLOCK_LIST( sPrevValueList, sValueList );
            }

            sPrevValueList = sValueList;
        }
    }

    
    /**
     * Output 설정
     */

    if ( aIndexDeleteValueList == NULL )
    {
        /* Output 없음 */
    }
    else
    {
        *aIndexDeleteValueList = sDeleteValueList;
    }

    if ( aIndexInsertValueList == NULL )
    {
        /* Output 없음 */
    }
    else
    {
        *aIndexInsertValueList = sInsertValueList;
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
 
/**
 * @brief INSERT 구문을 위한 Index Insert Key Value List 를 할당한다.
 * @param[in]  aTableDictHandle         Table Dictionary Handle
 * @param[in]  aTableWriteValueList     Table Write Value List
 * @param[in]  aIndexDictHandle         Index Dictionary Handle
 * @param[in]  aRegionMem               Region Memory 
 * @param[in]  aIndexInsertValueList    Index Insert Key 를 위한 Value List
 * @param[in]  aEnv                     Environment
 * @remarks
 * 함수 ellShareIndexValueList() 를 호출함.
 */
stlStatus ellShareIndexValueListForINSERT( ellDictHandle      * aTableDictHandle,
                                           knlValueBlockList  * aTableWriteValueList,
                                           ellDictHandle      * aIndexDictHandle,
                                           knlRegionMem       * aRegionMem,
                                           knlValueBlockList ** aIndexInsertValueList,
                                           ellEnv             * aEnv )
{
    return ellShareIndexValueList( NULL,
                                   aTableWriteValueList,
                                   aIndexDictHandle,
                                   aRegionMem,
                                   NULL,
                                   aIndexInsertValueList,
                                   aEnv );
}




/**
 * @brief DELETE 구문을 위한 Index Delete Key Value List 를 할당한다.
 * @param[in]  aTableDictHandle         Table Dictionary Handle
 * @param[in]  aTableReadValueList      Table Read Value List
 * @param[in]  aIndexDictHandle         Index Dictionary Handle
 * @param[in]  aRegionMem               Region Memory 
 * @param[in]  aIndexDeleteValueList    Index Delete Key 를 위한 Value List
 * @param[in]  aEnv                     Environment
 * @remarks
 * 함수 ellShareIndexValueList() 를 호출함.
 */
stlStatus ellShareIndexValueListForDELETE( ellDictHandle      * aTableDictHandle,
                                           knlValueBlockList  * aTableReadValueList,
                                           ellDictHandle      * aIndexDictHandle,
                                           knlRegionMem       * aRegionMem,
                                           knlValueBlockList ** aIndexDeleteValueList,
                                           ellEnv             * aEnv )
{
    return ellShareIndexValueList( aTableReadValueList,
                                   NULL,
                                   aIndexDictHandle,
                                   aRegionMem,
                                   aIndexDeleteValueList,
                                   NULL,
                                   aEnv );
}



/**
 * @brief SELECT 구문을 위한 Index Read Value List 를 할당한다.
 * @param[in]  aIndexDictHandle       Index Dictionary Handle
 * @param[in]  aUseKeyCnt             사용할 Key 의 개수 
 * @param[in]  aRegionMem             Region Memory
 * @param[in]  aBlockCnt              Block Read Count
 * @param[out] aIndexSelectValueList  Index Select Data Value List
 * @param[in]  aEnv                   Environment
 * @remarks
 * Key Range 생성등을 위한 Value List 공간으로 독자적인 Buffer 공간을 확보한다.
 */
stlStatus ellAllocIndexValueListForSELECT( ellDictHandle      * aIndexDictHandle,
                                           stlInt32             aUseKeyCnt,
                                           knlRegionMem       * aRegionMem,
                                           stlInt32             aBlockCnt,
                                           knlValueBlockList ** aIndexSelectValueList,
                                           ellEnv             * aEnv )
{
    stlInt32   i = 0;

    knlValueBlockList * sIndexValueList = NULL;
    knlValueBlockList * sValueList = NULL;
    knlValueBlockList * sPrevValueList = NULL;

    ellDictHandle   * sColumnHandle = NULL;
    ellIndexKeyDesc * sKeyDesc = NULL;
    dtlDataType       sColumnType = DTL_TYPE_NA;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aIndexDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aUseKeyCnt > 0) &&
                        (aUseKeyCnt <= ellGetIndexKeyCount(aIndexDictHandle)),
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aBlockCnt > 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIndexSelectValueList != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Key Column 정보 획득
     */

    sKeyDesc = ellGetIndexKeyDesc( aIndexDictHandle );
    
    /**
     * Table Column 들에 대한 Data Value List 설정
     */

    for ( i = 0; i < aUseKeyCnt; i++ )
    {
        /**
         * Value Block 생성 
         */

        sColumnHandle = & sKeyDesc[i].mKeyColumnHandle;

        sColumnType = ellGetColumnDBType( sColumnHandle );
        
        STL_DASSERT( (sColumnType != DTL_TYPE_LONGVARCHAR) &&
                     (sColumnType != DTL_TYPE_CLOB) &&
                     (sColumnType != DTL_TYPE_LONGVARBINARY) &&
                     (sColumnType != DTL_TYPE_BLOB) );
        
        STL_TRY( knlInitBlock( & sValueList,
                               aBlockCnt,
                               STL_TRUE, /* 컬럼임 */
                               STL_LAYER_EXECUTION_LIBRARY,
                               ellGetColumnTableID( sColumnHandle ),
                               ellGetColumnIdx( sColumnHandle ),
                               sColumnType,
                               ellGetColumnPrecision( sColumnHandle ),
                               ellGetColumnScale( sColumnHandle ),
                               ellGetColumnStringLengthUnit( sColumnHandle ),
                               ellGetColumnIntervalIndicator( sColumnHandle ),
                               aRegionMem,
                               KNL_ENV(aEnv) )
                 == STL_SUCCESS );
                                    
        /**
         * Link 설정 
         */

        if ( sIndexValueList == NULL )
        {
            sIndexValueList = sValueList;
        }
        else
        {
            KNL_LINK_BLOCK_LIST( sPrevValueList, sValueList );
        }

        sPrevValueList = sValueList;
    }
    
    /**
     * Output 설정
     */

    *aIndexSelectValueList = sIndexValueList;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}







/**
 * @brief Value Block에 Table Column Order 대신 Index Column Order를 설정한다.
 * @param[in,out]    aValueBlock      Table Column Order가 부여된 Value Block List
 * @param[in]        aIndexHandle     Index Handle
 * @param[in]        aEnv             Environment
 */
stlStatus ellSetIndexColumnOrder( knlValueBlockList   * aValueBlock,
                                  ellDictHandle       * aIndexHandle,
                                  ellEnv              * aEnv )
{
    knlValueBlockList  * sValueBlock = aValueBlock;
    ellIndexKeyDesc    * sIndexKeyDesc;
    stlInt32             sIndexKeyCnt;
    stlInt32             sLoop;
    stlInt64             sTableID;
    
    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aIndexHandle != NULL, ELL_ERROR_STACK(aEnv) );


    /**
     * Index 정보 획득
     */
    
    sIndexKeyCnt  = ellGetIndexKeyCount( aIndexHandle );
    sIndexKeyDesc = ellGetIndexKeyDesc( aIndexHandle );
    sTableID      = ellGetIndexTableID( aIndexHandle );

    /**
     * Index Column Order 설정
     */
    
    while( sValueBlock != NULL )
    {
        for( sLoop = 0; sLoop < sIndexKeyCnt ; sLoop++ )
        {
            if( ellGetColumnIdx( & sIndexKeyDesc[ sLoop ].mKeyColumnHandle ) ==
                KNL_GET_BLOCK_COLUMN_ORDER( sValueBlock ) )
            {
                sValueBlock->mTableID = sTableID;
                sValueBlock->mColumnOrder = sLoop;
                break;
            }
        }

        STL_DASSERT( sLoop < sIndexKeyCnt );
        
        sValueBlock = sValueBlock->mNext;
    }

    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

/** @} ellIndexValueList */



