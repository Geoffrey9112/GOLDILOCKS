/*******************************************************************************
 * eldcPrivDesc.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: eldcPrivDesc.c 1462 2011-07-19 10:46:21Z leekmo $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


/**
 * @file eldcPrivDesc.c
 * @brief Cache for Privilege descriptor
 */

#include <ell.h>
#include <eldt.h>                               
#include <eldc.h>

#include <eldDictionary.h>



/**
 * @addtogroup eldcPrivDesc
 * @{
 */

/**********************************************************************
 * Privilege 공통 Hash 함수 
 **********************************************************************/

/**
 * @brief Privilege Key 로부터 Hash Value 를 얻는다.
 * @param[in] aHashKey   Hash Key 
 * @remarks
 * from postgres
 */
stlUInt32  ellGetHashValuePrivKey( void * aHashKey )
{
    ellPrivDesc * sKeyPriv = NULL;
    
    stlUInt32   sHashValue = 0;
    stlUInt32   sOneValue;

    sKeyPriv = (ellPrivDesc *) aHashKey;
    
    sOneValue = ellHashInt64( sKeyPriv->mGranteeID );

    sHashValue ^= sOneValue << 16;
    sHashValue ^= sOneValue >> 16;

    sOneValue = ellHashInt64( sKeyPriv->mObjectID );
    
    sHashValue ^= sOneValue << 8;
    sHashValue ^= sOneValue >> 24;

    sOneValue = ellHashInt64( sKeyPriv->mPrivAction.mAction );

    sHashValue ^= sOneValue;
    
    return sHashValue;
}


/**
 * @brief Grant Privilege 를 위한 Privilege Hash Key 와 Hash Element의 비교를 위한 함수 포인터
 * @param[in] aHashKey        Hash Key
 * @param[in] aHashElement    Hash Element
 * @remarks
 * Hash Value 가 동일할 경우 Key 를 검사 
 */
stlBool eldcCompareFuncPrivWithoutGrant( void * aHashKey, eldcHashElement * aHashElement )
{
    ellPrivDesc * sHashData = NULL;
    ellPrivDesc * sHashKey = NULL;

    /**
     * 입력된 Privilege Key 와 Hash Element 의 Privilege Key 를 비교 
     */
    
    sHashKey  = (ellPrivDesc *) aHashKey;
    sHashData = (ellPrivDesc *) aHashElement->mHashData;

    if ( (sHashKey->mGranteeID          == sHashData->mGranteeID) &&
         (sHashKey->mObjectID           == sHashData->mObjectID) &&
         (sHashKey->mPrivAction.mAction == sHashData->mPrivAction.mAction) )
    {
        return STL_TRUE;
    }
    else
    {
        return STL_FALSE;
    }
}

/**
 * @brief Grant Privilege 를 위한 Privilege Hash Key 와 Hash Element의 비교를 위한 함수 포인터
 * @param[in] aHashKey        Hash Key
 * @param[in] aHashElement    Hash Element
 * @remarks
 * Hash Value 가 동일할 경우 Key 를 검사 
 */
stlBool eldcCompareFuncPrivWithGrant( void * aHashKey, eldcHashElement * aHashElement )
{
    ellPrivDesc * sHashData = NULL;
    ellPrivDesc * sHashKey = NULL;

    /**
     * 입력된 Privilege Key 와 Hash Element 의 Privilege Key 를 비교 
     */
    
    sHashKey  = (ellPrivDesc *) aHashKey;
    sHashData = (ellPrivDesc *) aHashElement->mHashData;

    if ( (sHashKey->mGranteeID          == sHashData->mGranteeID) &&
         (sHashKey->mObjectID           == sHashData->mObjectID) &&
         (sHashKey->mPrivAction.mAction == sHashData->mPrivAction.mAction) &&
         (sHashKey->mWithGrant          == sHashData->mWithGrant) )
    {
        return STL_TRUE;
    }
    else
    {
        return STL_FALSE;
    }
}

/**
 * @brief Revoke Privilege 를 위한 Privilege Hash Key 와 Hash Element의 비교를 위한 함수 포인터
 * @param[in] aHashKey        Hash Key
 * @param[in] aHashElement    Hash Element
 * @remarks
 * Hash Value 가 동일할 경우 Key 를 검사 
 */
stlBool eldcCompareFuncPrivRevoke( void * aHashKey, eldcHashElement * aHashElement )
{
    ellPrivDesc * sHashData = NULL;
    ellPrivDesc * sHashKey = NULL;

    /**
     * 입력된 Privilege Key 와 Hash Element 의 Privilege Key 를 비교 
     */
    
    sHashKey  = (ellPrivDesc *) aHashKey;
    sHashData = (ellPrivDesc *) aHashElement->mHashData;

    if ( (sHashKey->mGrantorID          == sHashData->mGrantorID) &&
         (sHashKey->mGranteeID          == sHashData->mGranteeID) &&
         (sHashKey->mObjectID           == sHashData->mObjectID) &&
         (sHashKey->mPrivAction.mAction == sHashData->mPrivAction.mAction) )
    {
        return STL_TRUE;
    }
    else
    {
        return STL_FALSE;
    }
}

/**********************************************************************
 * Privilege 공통 Dump Information
 **********************************************************************/

/**
 * @brief Privilege 의 String 출력을 생성하는 함수 포인터 
 * @param[in]  aPrivDesc    Privilege Descriptor
 * @param[out] aStringBuffer 출력 정보를 위한 String Buffer 공간
 * @remarks
 */
void eldcPrintHashDataPrivDesc( void * aPrivDesc, stlChar * aStringBuffer )
{
    ellPrivDesc * sPrivDesc = (ellPrivDesc *) aPrivDesc;
    
    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "Privilege Descriptor( " );

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sGRANTOR_ID: %ld, ",
                 aStringBuffer,
                 sPrivDesc->mGrantorID );
    
    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sGRANTEE_ID: %ld, ",
                 aStringBuffer,
                 sPrivDesc->mGranteeID );

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sOBJECT_ID: %ld, ",
                 aStringBuffer,
                 sPrivDesc->mObjectID );
    
    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sPRIV_ACTION: %d ) ",
                 aStringBuffer,
                 sPrivDesc->mPrivAction.mAction );
}

/**********************************************************************
 * Privilege 공통 초기화, 마무리 함수
 **********************************************************************/


/**
 * @brief Privilege Dump Handle 정보 설정
 * @param[in] aPrivCacheNO Privilege Cache Number 
 * @remarks
 */
void eldcSetPrivDumpHandle( eldcPrivCacheNO aPrivCacheNO )
{
    STL_ASSERT( (aPrivCacheNO >= ELDC_PRIVCACHE_DATABASE) &&
                (aPrivCacheNO < ELDC_PRIVCACHE_MAX) );

    gEldcPrivDump[aPrivCacheNO].mDictHash        = ELL_PRIV_CACHE( aPrivCacheNO );
    gEldcPrivDump[aPrivCacheNO].mPrintHashData   = eldcPrintHashDataPrivDesc;
    gEldcPrivDump[aPrivCacheNO].mPrintObjectDesc = eldcPrintINVALID;
}



/**
 * @brief Privilege 위한 Cache 를 구축한다.
 * @param[in] aTransID      Transaction ID
 * @param[in] aStmt         Statement
 * @param[in] aPrivCacheNO  Privilege Cache Number 
 * @param[in] aEnv          Environment
 * @remarks
 */
stlStatus eldcBuildCachePriv( smlTransId        aTransID,
                              smlStatement    * aStmt,
                              eldcPrivCacheNO   aPrivCacheNO,
                              ellEnv          * aEnv )
{
    stlBool             sBeginSelect = STL_FALSE;

    stlInt64            sDictTableID = ELL_DICT_OBJECT_ID_NA;
    
    void              * sTableHandle = NULL;
    knlValueBlockList * sTableValueList = NULL;

    void                * sIterator  = NULL;
    smlIteratorProperty   sIteratorProperty;

    smlRid              sRowRid;
    smlScn              sRowScn;
    smlRowBlock         sRowBlock;
    eldMemMark          sMemMark;

    smlFetchInfo          sFetchInfo;

    stlInt32 sState = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aPrivCacheNO >= ELDC_PRIVCACHE_DATABASE) &&
                        (aPrivCacheNO < ELDC_PRIVCACHE_MAX),
                        ELL_ERROR_STACK(aEnv) );

    /**
     * Dictionary Table ID 획득
     */

    switch (aPrivCacheNO)
    {
        case ELDC_PRIVCACHE_DATABASE:
            sDictTableID = ELDT_TABLE_ID_DATABASE_PRIVILEGES;
            break;
        case ELDC_PRIVCACHE_SPACE:
            sDictTableID = ELDT_TABLE_ID_TABLESPACE_PRIVILEGES;
            break;
        case ELDC_PRIVCACHE_SCHEMA:
            sDictTableID = ELDT_TABLE_ID_SCHEMA_PRIVILEGES;
            break;
        case ELDC_PRIVCACHE_TABLE:
            sDictTableID = ELDT_TABLE_ID_TABLE_PRIVILEGES;
            break;
        case ELDC_PRIVCACHE_USAGE:
            sDictTableID = ELDT_TABLE_ID_USAGE_PRIVILEGES;
            break;
        case ELDC_PRIVCACHE_COLUMN:
            sDictTableID = ELDT_TABLE_ID_COLUMN_PRIVILEGES;
            break;
        default:
            STL_ASSERT(0);
            break;
    }

    /**
     * Dictionary 테이블의 table handle 을 획득
     */
    
    sTableHandle = gEldTablePhysicalHandle[sDictTableID];

    /**
     * Dictionary 테이블의 Value List와 Row Block 할당
     */

    STL_TRY( eldAllocTableValueList( sDictTableID,
                                     & sMemMark,
                                     & sTableValueList,
                                     aEnv )
             == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( smlInitSingleRowBlock( &sRowBlock,
                                    &sRowRid,
                                    &sRowScn,
                                    SML_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * SELECT 준비
     */

    STL_TRY( eldBeginForTableAccessQuery( aStmt,
                                          sTableHandle,
                                          & sIteratorProperty,
                                          & sRowBlock,
                                          sTableValueList,
                                          NULL,
                                          NULL,
                                          & sFetchInfo,
                                          NULL,
                                          & sIterator,         
                                          aEnv )
             == STL_SUCCESS );

    sBeginSelect = STL_TRUE;

    /**
     * Column Descriptor 를 Cache 에 추가한다.
     */

    while ( 1 )
    {
        /**
         * 레코드를 읽는다.
         */
        
        STL_TRY( eldFetchNext( sIterator,
                               & sFetchInfo,
                               aEnv )
                 == STL_SUCCESS );

        if( sFetchInfo.mIsEndOfScan == STL_TRUE )
        {
            break;
        }

        /**
         * Privilege Cache 정보를 추가
         */
        
        STL_TRY( eldcInsertCachePriv( aTransID,
                                      aStmt,
                                      aPrivCacheNO,
                                      sTableValueList,
                                      aEnv )
                 == STL_SUCCESS );

        /**
         * Start-Up 단계에서 Pending Memory 가 증가하지 않도록 Pending Operation 들을 수행한다.
         */
        
        STL_TRY( eldcRunPendOP4BuildCache( aTransID, aStmt, aEnv ) == STL_SUCCESS );
    }
    
    /**
     * SELECT 종료
     */

    sBeginSelect = STL_FALSE;
    STL_TRY( eldEndForQuery( sIterator,
                             aEnv )
             == STL_SUCCESS );

    /**
     * 사용한 메모리를 해제한다.
     */

    sState = 0;
    STL_TRY( eldFreeTableValueList( & sMemMark, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    if ( sBeginSelect == STL_TRUE )
    {
        (void) eldEndForQuery( sIterator,
                               aEnv );
    }

    switch (sState)
    {
        case 1:
            (void) eldFreeTableValueList( & sMemMark, aEnv );
        default:
            break;
    }
    
    return STL_FAILURE;
}    


/**********************************************************************
 * Privilege 공통 제어 함수
 **********************************************************************/


/**
 * @brief Privilege 를 위한 Cache 정보를 추가한다.
 * @param[in] aTransID      Transaction ID
 * @param[in] aStmt         Statement
 * @param[in] aPrivCacheNO  Privilege Cache Number 
 * @param[in] aValueList    Data Value List
 * @param[in] aEnv          Environment
 * @remarks
 */
stlStatus eldcInsertCachePriv( smlTransId          aTransID,
                               smlStatement      * aStmt,
                               eldcPrivCacheNO     aPrivCacheNO,
                               knlValueBlockList * aValueList,
                               ellEnv            * aEnv )
{
    ellPrivDesc * sPrivDesc = NULL;
    
    eldcHashElement * sHashElement = NULL;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aPrivCacheNO >= ELDC_PRIVCACHE_DATABASE) &&
                        (aPrivCacheNO < ELDC_PRIVCACHE_MAX),
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aValueList != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Meta Building 과정이라면, Cache 를 구축하지 않는다.
     */
    
    STL_TRY_THROW ( gEllIsMetaBuilding == STL_FALSE, RAMP_SUCCESS );
    
    /**
     * Privilege Descriptor 구성
     */
    
    STL_TRY( eldcMakePrivDesc( aTransID,
                               aStmt,
                               aPrivCacheNO,
                               aValueList,
                               & sPrivDesc,
                               aEnv )
             == STL_SUCCESS );
    
    /**
     * Privilege를 위한 Hash Element 구성
     */

    STL_TRY( eldcMakeHashElementPriv( & sHashElement,
                                      aTransID,
                                      sPrivDesc,
                                      aEnv )
             == STL_SUCCESS );
    
    /**
     * Privilege Cache  에 추가 
     */

    STL_TRY( eldcInsertHashElement( ELL_PRIV_CACHE( aPrivCacheNO ),
                                    sHashElement,
                                    aEnv )
             == STL_SUCCESS );

    STL_RAMP( RAMP_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}





/**
 * @brief 조건에 부합하는 Privilege Cache 을 제거한다.
 * @param[in]  aTransID           Transaction ID
 * @param[in]  aPrivObject        Privilege Object
 * @param[in]  aGrantorID         Grantor ID
 * @param[in]  aGranteeID         Grantee ID
 * @param[in]  aObjectID          Object ID
 * @param[in]  aPrivAction        Privilege Action
 * @param[in]  aEnv               Envirionment
 * @remarks
 */
stlStatus eldcDeletePrivCache( smlTransId           aTransID,
                               ellPrivObject        aPrivObject,
                               stlInt64             aGrantorID,
                               stlInt64             aGranteeID,
                               stlInt64             aObjectID,
                               stlInt32             aPrivAction,
                               ellEnv             * aEnv )
{
    eldcPrivCacheNO sPrivCacheNO = -1;

    eldcHashElement * sHashElement = NULL;
    
    ellPrivDesc sHashKey;
    stlMemset( & sHashKey, 0x00, STL_SIZEOF(ellPrivDesc) );
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( (aPrivObject > ELL_PRIV_NA) && (aPrivObject < ELL_PRIV_OWNER),
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aGrantorID > ELL_DICT_OBJECT_ID_NA, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aGranteeID > ELL_DICT_OBJECT_ID_NA, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPrivAction > 0, ELL_ERROR_STACK(aEnv) );

    /**
     * Hash Key 구성
     */

    sHashKey.mGrantorID          = aGrantorID;
    sHashKey.mGranteeID          = aGranteeID;
    sHashKey.mObjectID           = aObjectID;
    sHashKey.mPrivObject         = aPrivObject;
    sHashKey.mPrivAction.mAction = aPrivAction;

    /**
     * Privilege Cache 선정
     */

    switch ( aPrivObject )
    {
        case ELL_PRIV_DATABASE:
            sPrivCacheNO = ELDC_PRIVCACHE_DATABASE;
            break;
        case ELL_PRIV_SPACE:
            sPrivCacheNO = ELDC_PRIVCACHE_SPACE;
            break;
        case ELL_PRIV_SCHEMA:
            sPrivCacheNO = ELDC_PRIVCACHE_SCHEMA;
            break;
        case ELL_PRIV_TABLE:
            sPrivCacheNO = ELDC_PRIVCACHE_TABLE;
            break;
        case ELL_PRIV_USAGE:
            sPrivCacheNO = ELDC_PRIVCACHE_USAGE;
            break;
        case ELL_PRIV_COLUMN:
            sPrivCacheNO = ELDC_PRIVCACHE_COLUMN;
            break;
        default:
            STL_ASSERT(0);
            break;
    }

    /**
     * Privilege Hash Element 검색
     */
    
    STL_TRY( eldcFindHashElement( aTransID,
                                  SML_MAXIMUM_STABLE_SCN,
                                  ELL_PRIV_CACHE( sPrivCacheNO ),
                                  & sHashKey,
                                  ellGetHashValuePrivKey,
                                  eldcCompareFuncPrivRevoke,
                                  & sHashElement,
                                  aEnv )
             == STL_SUCCESS );

    STL_DASSERT( sHashElement != NULL );

    /**
     * Privilege Hash Element 삭제
     */

    STL_TRY( eldcDeleteHashElement( aTransID,
                                    ELL_PRIV_CACHE( sPrivCacheNO ),
                                    sHashElement,
                                    aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**********************************************************************
 * Privilege 공통 멤버 함수 
 **********************************************************************/


/**
 * @brief 읽은 Value List 로부터 Privilege Descriptor 를 생성한다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aPrivCacheNO     Privilege Cache Number 
 * @param[in]  aValueList       Value List
 * @param[out] aPrivDesc        Privilege Descriptor
 * @param[in]  aEnv             Environment 포인터
 * @remarks
 */
stlStatus eldcMakePrivDesc( smlTransId           aTransID,
                            smlStatement       * aStmt,
                            eldcPrivCacheNO      aPrivCacheNO,
                            knlValueBlockList  * aValueList,
                            ellPrivDesc       ** aPrivDesc,
                            ellEnv             * aEnv )
{
    ellPrivDesc      * sPrivDesc = NULL;

    stlInt64           sDictTableID   = ELL_DICT_OBJECT_ID_NA;
    ellPrivObject      sPrivObject    = ELL_PRIV_NA;
    stlInt32           sDictGrantorColumnNO = 0;
    stlInt32           sDictGranteeColumnNO = 0;
    stlInt32           sDictObjectColumnNO = 0;
    stlInt32           sDictPrivColumnNO = 0;
    stlInt32           sDictGrantColumnNO = 0;
    
    dtlDataValue     * sDataValue = NULL;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aPrivCacheNO >= ELDC_PRIVCACHE_DATABASE) &&
                        (aPrivCacheNO < ELDC_PRIVCACHE_MAX),
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aValueList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPrivDesc != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Privilege Descriptor 를 위한 공간 할당
     */
    
    STL_TRY( eldAllocCacheMem( (void **) & sPrivDesc,
                               STL_SIZEOF(ellPrivDesc),
                               aEnv )
             == STL_SUCCESS );

    /**
     * Dictionary Table 과 Column 정보 획득
     */

    switch (aPrivCacheNO)
    {
        case ELDC_PRIVCACHE_DATABASE:
            sDictTableID         = ELDT_TABLE_ID_DATABASE_PRIVILEGES;
            sPrivObject          = ELL_PRIV_DATABASE;
            sDictGrantorColumnNO = ELDT_DBPriv_ColumnOrder_GRANTOR_ID;
            sDictGranteeColumnNO = ELDT_DBPriv_ColumnOrder_GRANTEE_ID;
            sDictObjectColumnNO  = 0;
            sDictPrivColumnNO    = ELDT_DBPriv_ColumnOrder_PRIVILEGE_TYPE_ID;
            sDictGrantColumnNO   = ELDT_DBPriv_ColumnOrder_IS_GRANTABLE;
            break;
        case ELDC_PRIVCACHE_SPACE:
            sDictTableID         = ELDT_TABLE_ID_TABLESPACE_PRIVILEGES;
            sPrivObject          = ELL_PRIV_SPACE;
            sDictGrantorColumnNO = ELDT_SpacePriv_ColumnOrder_GRANTOR_ID;
            sDictGranteeColumnNO = ELDT_SpacePriv_ColumnOrder_GRANTEE_ID;
            sDictObjectColumnNO  = ELDT_SpacePriv_ColumnOrder_TABLESPACE_ID;
            sDictPrivColumnNO    = ELDT_SpacePriv_ColumnOrder_PRIVILEGE_TYPE_ID;
            sDictGrantColumnNO   = ELDT_SpacePriv_ColumnOrder_IS_GRANTABLE;
            break;
        case ELDC_PRIVCACHE_SCHEMA:
            sDictTableID         = ELDT_TABLE_ID_SCHEMA_PRIVILEGES;
            sPrivObject          = ELL_PRIV_SCHEMA;
            sDictGrantorColumnNO = ELDT_SchemaPriv_ColumnOrder_GRANTOR_ID;
            sDictGranteeColumnNO = ELDT_SchemaPriv_ColumnOrder_GRANTEE_ID;
            sDictObjectColumnNO  = ELDT_SchemaPriv_ColumnOrder_SCHEMA_ID;
            sDictPrivColumnNO    = ELDT_SchemaPriv_ColumnOrder_PRIVILEGE_TYPE_ID;
            sDictGrantColumnNO   = ELDT_SchemaPriv_ColumnOrder_IS_GRANTABLE;
            break;
        case ELDC_PRIVCACHE_TABLE:
            sDictTableID         = ELDT_TABLE_ID_TABLE_PRIVILEGES;
            sPrivObject          = ELL_PRIV_TABLE;
            sDictGrantorColumnNO = ELDT_TablePriv_ColumnOrder_GRANTOR_ID;
            sDictGranteeColumnNO = ELDT_TablePriv_ColumnOrder_GRANTEE_ID;
            sDictObjectColumnNO  = ELDT_TablePriv_ColumnOrder_TABLE_ID;
            sDictPrivColumnNO    = ELDT_TablePriv_ColumnOrder_PRIVILEGE_TYPE_ID;
            sDictGrantColumnNO   = ELDT_TablePriv_ColumnOrder_IS_GRANTABLE;
            break;
        case ELDC_PRIVCACHE_USAGE:
            sDictTableID         = ELDT_TABLE_ID_USAGE_PRIVILEGES;
            sPrivObject          = ELL_PRIV_USAGE;
            sDictGrantorColumnNO = ELDT_UsagePriv_ColumnOrder_GRANTOR_ID;
            sDictGranteeColumnNO = ELDT_UsagePriv_ColumnOrder_GRANTEE_ID;
            sDictObjectColumnNO  = ELDT_UsagePriv_ColumnOrder_OBJECT_ID;
            sDictPrivColumnNO    = ELDT_UsagePriv_ColumnOrder_OBJECT_TYPE_ID;
            sDictGrantColumnNO   = ELDT_UsagePriv_ColumnOrder_IS_GRANTABLE;
            break;
        case ELDC_PRIVCACHE_COLUMN:
            sDictTableID         = ELDT_TABLE_ID_COLUMN_PRIVILEGES;
            sPrivObject          = ELL_PRIV_COLUMN;
            sDictGrantorColumnNO = ELDT_ColumnPriv_ColumnOrder_GRANTOR_ID;
            sDictGranteeColumnNO = ELDT_ColumnPriv_ColumnOrder_GRANTEE_ID;
            sDictObjectColumnNO  = ELDT_ColumnPriv_ColumnOrder_COLUMN_ID;
            sDictPrivColumnNO    = ELDT_ColumnPriv_ColumnOrder_PRIVILEGE_TYPE_ID;
            sDictGrantColumnNO   = ELDT_ColumnPriv_ColumnOrder_IS_GRANTABLE;
            break;
        default:
            STL_ASSERT(0);
            break;
    }
    
    /**
     * Privilege Descriptor 의 각 정보를 설정한다.
     */

    /*
     * mGrantorID
     */

    sDataValue = eldFindDataValue( aValueList,
                                   sDictTableID,
                                   sDictGrantorColumnNO );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( & sPrivDesc->mGrantorID,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sPrivDesc->mGrantorID) == sDataValue->mLength );
    }
    
    /*
     * mGranteeID
     */

    sDataValue = eldFindDataValue( aValueList,
                                   sDictTableID,
                                   sDictGranteeColumnNO );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( & sPrivDesc->mGranteeID,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sPrivDesc->mGranteeID) == sDataValue->mLength );
    }

    /*
     * mObjectID
     */

    if ( aPrivCacheNO == ELDC_PRIVCACHE_DATABASE )
    {
        sPrivDesc->mObjectID = ellGetDbCatalogID();
    }
    else
    {
        sDataValue = eldFindDataValue( aValueList,
                                       sDictTableID,
                                       sDictObjectColumnNO );
        if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
        {
            STL_ASSERT(0);
        }
        else
        {
            stlMemcpy( & sPrivDesc->mObjectID,
                       sDataValue->mValue,
                       sDataValue->mLength );
            STL_DASSERT( STL_SIZEOF(sPrivDesc->mObjectID) == sDataValue->mLength );
        }
    }

    /*
     * mPrivObject
     */

    sPrivDesc->mPrivObject = sPrivObject;
    
    /*
     * mPrivAction
     */

    sDataValue = eldFindDataValue( aValueList,
                                   sDictTableID,
                                   sDictPrivColumnNO );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( & sPrivDesc->mPrivAction.mAction,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sPrivDesc->mPrivAction.mAction) == sDataValue->mLength );
    }

    /*
     * mWithGrant
     */

    sDataValue = eldFindDataValue( aValueList,
                                   sDictTableID,
                                   sDictGrantColumnNO );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( & sPrivDesc->mWithGrant,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sPrivDesc->mWithGrant) == sDataValue->mLength );
    }
    
    /**
     * return 값 설정
     */
    
    *aPrivDesc = sPrivDesc;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Privilege를 위한 Hash Element 를 구성한다.
 * @param[out] aHashElement    Hash Element
 * @param[in]  aTransID        Transaction ID
 * @param[in]  aPrivDesc       Privilege Descriptor
 * @param[in]  aEnv            Environment 
 * @remarks
 */ 
stlStatus eldcMakeHashElementPriv( eldcHashElement ** aHashElement,
                                   smlTransId         aTransID,
                                   ellPrivDesc      * aPrivDesc,
                                   ellEnv           * aEnv )
{
    stlUInt32                sHashValue = 0;
    eldcHashElement        * sHashElement = NULL;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aHashElement != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPrivDesc != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Hash Value 생성
     */

    sHashValue = ellGetHashValuePrivKey( aPrivDesc );
    
    /**
     * Hash Element 생성
     */

    STL_TRY( eldcMakeHashElement( & sHashElement,
                                  aTransID,
                                  aPrivDesc,
                                  sHashValue,
                                  NULL,   /* Object Descriptor 가 없음 */
                                  aEnv )
             == STL_SUCCESS );

    /**
     * Output Parameter 설정 
     */
    
    *aHashElement = sHashElement;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**********************************************************************
 * Privilege 공통 조회 함수 
 **********************************************************************/


/**
 * @brief 조건에 부합하는 Grant Privilege Handle 을 획득한다.
 * @param[in]  aTransID           Transaction ID
 * @param[in]  aViewSCN           View SCN
 * @param[in]  aPrivObject        Privilege Object
 * @param[in]  aGranteeID         Grantee ID
 * @param[in]  aObjectID          Object ID
 * @param[in]  aPrivAction        Privilege Action
 * @param[in]  aWithGrant         WITH GRANT OPTION 필요 여부 
 * @param[out] aPrivHandle        Privilege Handle
 * @param[out] aExist             존재 여부 
 * @param[in]  aEnv               Envirionment
 * @remarks
 */
stlStatus eldcGetGrantPrivHandle( smlTransId           aTransID,
                                  smlScn               aViewSCN,
                                  ellPrivObject        aPrivObject,
                                  stlInt64             aGranteeID,
                                  stlInt64             aObjectID,
                                  stlInt32             aPrivAction,
                                  stlBool              aWithGrant,
                                  ellDictHandle      * aPrivHandle,
                                  stlBool            * aExist,
                                  ellEnv             * aEnv )
{
    eldcPrivCacheNO sPrivCacheNO = -1;

    eldcHashElement * sHashElement = NULL;
    
    ellPrivDesc sHashKey;
    stlMemset( & sHashKey, 0x00, STL_SIZEOF(ellPrivDesc) );
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( (aPrivObject > ELL_PRIV_NA) && (aPrivObject < ELL_PRIV_OWNER),
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aGranteeID > ELL_DICT_OBJECT_ID_NA, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPrivAction > 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPrivHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExist != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Hash Key 구성
     */

    sHashKey.mGranteeID          = aGranteeID;
    sHashKey.mObjectID           = aObjectID;
    sHashKey.mPrivObject         = aPrivObject;
    sHashKey.mPrivAction.mAction = aPrivAction;
    sHashKey.mWithGrant          = aWithGrant;

    /**
     * Privilege Cache 선정
     */

    switch ( aPrivObject )
    {
        case ELL_PRIV_DATABASE:
            sPrivCacheNO = ELDC_PRIVCACHE_DATABASE;
            break;
        case ELL_PRIV_SPACE:
            sPrivCacheNO = ELDC_PRIVCACHE_SPACE;
            break;
        case ELL_PRIV_SCHEMA:
            sPrivCacheNO = ELDC_PRIVCACHE_SCHEMA;
            break;
        case ELL_PRIV_TABLE:
            sPrivCacheNO = ELDC_PRIVCACHE_TABLE;
            break;
        case ELL_PRIV_USAGE:
            sPrivCacheNO = ELDC_PRIVCACHE_USAGE;
            break;
        case ELL_PRIV_COLUMN:
            sPrivCacheNO = ELDC_PRIVCACHE_COLUMN;
            break;
        default:
            STL_ASSERT(0);
            break;
    }

    /**
     * Privilege Hash Element 검색
     */

    if ( aWithGrant == STL_TRUE )
    {
        STL_TRY( eldcFindHashElement( aTransID,
                                      aViewSCN,
                                      ELL_PRIV_CACHE( sPrivCacheNO ),
                                      & sHashKey,
                                      ellGetHashValuePrivKey,
                                      eldcCompareFuncPrivWithGrant,
                                      & sHashElement,
                                      aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( eldcFindHashElement( aTransID,
                                      aViewSCN,
                                      ELL_PRIV_CACHE( sPrivCacheNO ),
                                      & sHashKey,
                                      ellGetHashValuePrivKey,
                                      eldcCompareFuncPrivWithoutGrant,
                                      & sHashElement,
                                      aEnv )
                 == STL_SUCCESS );
    }

    /**
     * Output 설정
     */
    
    if ( sHashElement == NULL )
    {
        /**
         * 존재하지 않음
         */
        
        *aExist = STL_FALSE;
    }
    else
    {
        ellSetPrivHandle( aTransID,
                          aViewSCN,
                          aPrivHandle,
                          aPrivObject,
                          sHashElement,
                          aEnv );
            
        *aExist = STL_TRUE;
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 조건에 부합하는 Revoke Privilege Handle 을 획득한다.
 * @param[in]  aTransID           Transaction ID
 * @param[in]  aViewSCN           View SCN
 * @param[in]  aPrivObject        Privilege Object
 * @param[in]  aGrantorID         Grantor ID
 * @param[in]  aGranteeID         Grantee ID
 * @param[in]  aObjectID          Object ID
 * @param[in]  aPrivAction        Privilege Action
 * @param[out] aPrivHandle        Privilege Handle
 * @param[out] aExist             존재 여부 
 * @param[in]  aEnv               Envirionment
 * @remarks
 */
stlStatus eldcGetRevokePrivHandle( smlTransId           aTransID,
                                   smlScn               aViewSCN,
                                   ellPrivObject        aPrivObject,
                                   stlInt64             aGrantorID,
                                   stlInt64             aGranteeID,
                                   stlInt64             aObjectID,
                                   stlInt32             aPrivAction,
                                   ellDictHandle      * aPrivHandle,
                                   stlBool            * aExist,
                                   ellEnv             * aEnv )
{
    eldcPrivCacheNO sPrivCacheNO = -1;

    eldcHashElement * sHashElement = NULL;
    
    ellPrivDesc sHashKey;
    stlMemset( & sHashKey, 0x00, STL_SIZEOF(ellPrivDesc) );
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( (aPrivObject > ELL_PRIV_NA) && (aPrivObject < ELL_PRIV_OWNER),
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aGrantorID > ELL_DICT_OBJECT_ID_NA, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aGranteeID > ELL_DICT_OBJECT_ID_NA, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPrivAction > 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPrivHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExist != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Hash Key 구성
     */

    sHashKey.mGrantorID          = aGrantorID;
    sHashKey.mGranteeID          = aGranteeID;
    sHashKey.mObjectID           = aObjectID;
    sHashKey.mPrivObject         = aPrivObject;
    sHashKey.mPrivAction.mAction = aPrivAction;

    /**
     * Privilege Cache 선정
     */

    switch ( aPrivObject )
    {
        case ELL_PRIV_DATABASE:
            sPrivCacheNO = ELDC_PRIVCACHE_DATABASE;
            break;
        case ELL_PRIV_SPACE:
            sPrivCacheNO = ELDC_PRIVCACHE_SPACE;
            break;
        case ELL_PRIV_SCHEMA:
            sPrivCacheNO = ELDC_PRIVCACHE_SCHEMA;
            break;
        case ELL_PRIV_TABLE:
            sPrivCacheNO = ELDC_PRIVCACHE_TABLE;
            break;
        case ELL_PRIV_USAGE:
            sPrivCacheNO = ELDC_PRIVCACHE_USAGE;
            break;
        case ELL_PRIV_COLUMN:
            sPrivCacheNO = ELDC_PRIVCACHE_COLUMN;
            break;
        default:
            STL_ASSERT(0);
            break;
    }

    /**
     * Privilege Hash Element 검색
     */
    
    STL_TRY( eldcFindHashElement( aTransID,
                                  aViewSCN,
                                  ELL_PRIV_CACHE( sPrivCacheNO ),
                                  & sHashKey,
                                  ellGetHashValuePrivKey,
                                  eldcCompareFuncPrivRevoke,
                                  & sHashElement,
                                  aEnv )
             == STL_SUCCESS );

    /**
     * Output 설정
     */
    
    if ( sHashElement == NULL )
    {
        /**
         * 존재하지 않음
         */
        
        *aExist = STL_FALSE;
    }
    else
    {
        ellSetPrivHandle( aTransID,
                          aViewSCN,
                          aPrivHandle,
                          aPrivObject,
                          sHashElement,
                          aEnv );
            
        *aExist = STL_TRUE;
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/** @} eldcPrivDesc */
