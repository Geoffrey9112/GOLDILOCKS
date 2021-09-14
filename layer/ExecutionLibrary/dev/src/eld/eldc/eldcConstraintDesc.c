/*******************************************************************************
 * eldcConstraintDesc.c
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
 * @file eldcConstraintDesc.c
 * @brief Cache for Constraint descriptor
 */

#include <ell.h>
#include <eldt.h>                               
#include <eldc.h>

#include <eldDictionary.h>
#include <eldIndex.h>
#include <elgPendOp.h>


/**
 * @addtogroup eldcConstraintDesc
 * @{
 */


/**
 * @brief Hash Key 로부터 Hash Value 생성을 위한 함수 포인터
 * @param[in] aHashKey     Hash Key(Constraint ID)
 * @remarks
 */
stlUInt32  eldcHashFuncConstID( void * aHashKey )
{
    stlInt64  sConstID = 0;
    
    stlUInt32 sHashValue = 0;

    /**
     * Const ID 로부터 Hash Value 생성 
     */
    
    sConstID = *(stlInt64 *) aHashKey;
    
    sHashValue = ellGetHashValueOneID( sConstID );

    return sHashValue;
}


/**
 * @brief Hash Key 와 Hash Element의 비교를 위한 함수 포인터
 * @param[in] aHashKey        Hash Key(Constraint ID)
 * @param[in] aHashElement    Hash Element
 * @remarks
 * Hash Value 가 동일할 경우 Key 를 검사 
 */
stlBool    eldcCompareFuncConstID( void * aHashKey, eldcHashElement * aHashElement )
{
    stlInt64              sConstID = 0;
    
    eldcHashDataConstID * sHashData = NULL;

    /**
     * 입력된 Constraint ID 와 Hash Element 의 Constraint ID 를 비교 
     */
    
    sConstID  = *(stlInt64 *) aHashKey;
    
    sHashData    = (eldcHashDataConstID *) aHashElement->mHashData;

    if ( sHashData->mKeyConstID == sConstID )
    {
        return STL_TRUE;
    }
    else
    {
        return STL_FALSE;
    }
}


/**
 * @brief Hash Key 로부터 Hash Value 생성을 위한 함수 포인터
 * @param[in] aHashKey     Hash Key (Schema ID, Constraint Name)
 * @remarks
 */
stlUInt32  eldcHashFuncConstName( void * aHashKey )
{
    eldcHashKeyConstName * sKeyData = NULL;
    
    stlUInt32 sHashValue = 0;

    /**
     * Hash Key 로부터 Hash Value 생성 
     */
    
    sKeyData = (eldcHashKeyConstName *) aHashKey;
    
    sHashValue = ellGetHashValueNameAndID( sKeyData->mSchemaID, sKeyData->mConstName );

    return sHashValue;
}


/**
 * @brief Hash Key 와 Hash Element의 비교를 위한 함수 포인터
 * @param[in] aHashKey        Hash Key (Schema ID, Constraint Name )
 * @param[in] aHashElement    Hash Element
 * @remarks
 * Hash Value 가 동일할 경우 Key 를 검사 
 */
stlBool    eldcCompareFuncConstName( void * aHashKey, eldcHashElement * aHashElement )
{
    eldcHashKeyConstName  * sHashKey = NULL;
    
    eldcHashDataConstName * sHashData    = NULL;

    /**
     * 입력된 Hash Key 와 Hash Element 의 Key 를 비교 
     */
    
    sHashKey  = (eldcHashKeyConstName *) aHashKey;
    
    sHashData    = (eldcHashDataConstName *) aHashElement->mHashData;

    if ( ( sHashData->mKeySchemaID == sHashKey->mSchemaID ) &&
         ( stlStrcmp( sHashData->mKeyConstName, sHashKey->mConstName ) == 0 ) )
    {
        return STL_TRUE;
    }
    else
    {
        return STL_FALSE;
    }
}





/**
 * @brief Const Descriptor 를 위한 공간을 계산한다.
 * @param[in] aConstNameLen    Constraint Name 의 길이
 * @param[in] aConstType       Constraint Type
 * @param[in] aCheckClauseLen  Check Clause 의 길이 (CHECK CLAUSE contraint 인 경우)
 * @remarks
 */
stlInt32  eldcCalSizeConstDesc( stlInt32               aConstNameLen,
                                ellTableConstraintType aConstType,
                                stlInt32               aCheckClauseLen )
{
    stlInt32   sSize = 0;

    sSize = STL_ALIGN_DEFAULT( STL_SIZEOF(ellTableConstDesc) );
    
    if ( aConstNameLen > 0 )
    {
        sSize = sSize + STL_ALIGN_DEFAULT(aConstNameLen + 1);
    }

    switch ( aConstType )
    {
        case ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY:
            sSize = sSize + STL_ALIGN_DEFAULT( STL_SIZEOF(ellPrimaryKeyDepDesc) );
            break;
        case ELL_TABLE_CONSTRAINT_TYPE_UNIQUE_KEY:
            sSize = sSize + STL_ALIGN_DEFAULT( STL_SIZEOF(ellUniqueKeyDepDesc) );
            break;
        case ELL_TABLE_CONSTRAINT_TYPE_FOREIGN_KEY:
            sSize = sSize + STL_ALIGN_DEFAULT( STL_SIZEOF(ellForeignKeyDepDesc) );
            break;
        case ELL_TABLE_CONSTRAINT_TYPE_CHECK_NOT_NULL:
            sSize = sSize + STL_ALIGN_DEFAULT( STL_SIZEOF(ellCheckNotNullDepDesc) );
            break;
        case ELL_TABLE_CONSTRAINT_TYPE_CHECK_CLAUSE:
            sSize = sSize + STL_ALIGN_DEFAULT( STL_SIZEOF(ellCheckClauseDepDesc) );
            sSize = sSize + STL_ALIGN_DEFAULT(aCheckClauseLen + 1);
            break;
        default:
            break;
    }
    
    return sSize;
}


/**
 * @brief Const Descriptor 로부터 Constraint Name 의 Pointer 위치를 계산
 * @param[in] aConstDesc   Constraint Descriptor
 * @remarks
 */
stlChar * eldcGetConstNamePtr( ellTableConstDesc * aConstDesc )
{
    return (stlChar *) aConstDesc + STL_ALIGN_DEFAULT( STL_SIZEOF(ellTableConstDesc) );
}

/**
 * @brief Const Descriptor 로부터 Constraint Type Descriptor 의 Pointer 위치를 계산
 * @param[in] aConstDesc     Constraint Descriptor
 * @param[in] aConstNameLen  Constraint Name Length
 * @remarks
 */
void * eldcGetConstTypePtr( ellTableConstDesc * aConstDesc,
                            stlInt32            aConstNameLen )
{
    return (void *)
        ( (stlChar *) aConstDesc
          + STL_ALIGN_DEFAULT( STL_SIZEOF(ellTableConstDesc) )
          + STL_ALIGN_DEFAULT(aConstNameLen + 1) );
}


/**
 * @brief Const Descriptor 로부터 Check Clause 의 Pointer 위치를 계산
 * @param[in] aConstDesc     Constraint Descriptor
 * @param[in] aConstNameLen  Constraint Name Length
 * @remarks
 */
stlChar * eldcGetCheckClausePtr( ellTableConstDesc * aConstDesc,
                                 stlInt32            aConstNameLen )
{
    return (void *)
        ( (stlChar *) aConstDesc
          + STL_ALIGN_DEFAULT( STL_SIZEOF(ellTableConstDesc) )
          + STL_ALIGN_DEFAULT(aConstNameLen + 1)
          + STL_ALIGN_DEFAULT( STL_SIZEOF(ellCheckClauseDepDesc) ) );
}




/**
 * @brief Constraint ID  Hash Data 를 위한 공간을 계산한다.
 * @remarks
 */
stlInt32  eldcCalSizeHashDataConstID()
{
    stlInt32   sSize = 0;

    sSize = STL_ALIGN_DEFAULT( STL_SIZEOF( eldcHashDataConstID ) );

    return sSize;
}


/**
 * @brief Constraint Name Hash Data 를 위한 공간을 계산한다.
 * @param[in]    aConstNameLen    Const Name 의 길이 
 * @remarks
 */
stlInt32  eldcCalSizeHashDataConstName( stlInt32 aConstNameLen )
{
    stlInt32   sSize = 0;

    sSize = STL_ALIGN_DEFAULT( STL_SIZEOF(eldcHashDataConstName) );

    if ( aConstNameLen > 0 )
    {
        sSize = sSize + STL_ALIGN_DEFAULT(aConstNameLen + 1);
    }
    
    return sSize;
}


/**
 * @brief Constraint Name Hash Data 로부터 Const Name 의 Pointer 위치를 계산
 * @param[in] aHashDataConstName   Constraint Name 의 Hash Data
 * @remarks
 */
stlChar * eldcGetHashDataConstNamePtr( eldcHashDataConstName * aHashDataConstName )
{
    return (stlChar *) aHashDataConstName
        + STL_ALIGN_DEFAULT( STL_SIZEOF(eldcHashDataConstName) );
}













/**
 * @brief Constraint Descriptor 의 String 출력을 생성하는 함수 포인터 
 * @param[in]  aConstDesc    Constraint Descriptor
 * @param[out] aStringBuffer 출력 정보를 위한 String Buffer 공간
 * @remarks
 */
void eldcPrintConstDesc( void * aConstDesc, stlChar * aStringBuffer )
{
    stlInt32       i = 0;

    stlInt32               sSize = 0;
    ellTableConstDesc    * sConstDesc = (ellTableConstDesc *)aConstDesc;

    /**
     * SQL 5대 제약조건
     */

    ellPrimaryKeyDepDesc   * sPrimaryKeyDesc = NULL;
    ellForeignKeyDepDesc   * sForeignKeyDesc = NULL;
    ellUniqueKeyDepDesc    * sUniqueKeyDesc = NULL;
    ellCheckNotNullDepDesc * sCheckNotNullDesc = NULL;
    ellCheckClauseDepDesc  * sCheckClauseDesc = NULL;

    
    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "Constraint Descriptor( " );

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sCONSTRAINT_OWNER_ID: %ld, ",
                 aStringBuffer,
                 sConstDesc->mConstOwnerID );

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sCONSTRAINT_SCHEMA_ID: %ld, ",
                 aStringBuffer,
                 sConstDesc->mConstSchemaID );
    
    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sCONSTRAINT_ID: %ld, ",
                 aStringBuffer,
                 sConstDesc->mConstID );

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sTABLE_SCHEMA_ID: %ld, ",
                 aStringBuffer,
                 sConstDesc->mTableSchemaID );

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sTABLE_ID: %ld, ",
                 aStringBuffer,
                 sConstDesc->mTableID );
    
    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sCONSTRAINT_NAME: %s, ",
                 aStringBuffer,
                 sConstDesc->mConstName );

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sCONSTRAINT_TYPE: %s, ",
                 aStringBuffer,
                 gEllTableConstraintTypeString[sConstDesc->mConstType] );
    
    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sDEFERRABLE: %d, ",
                 aStringBuffer,
                 sConstDesc->mDeferrable );

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sINITIALLY_DEFERRED: %d, ",
                 aStringBuffer,
                 sConstDesc->mInitDeferred );
    
    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sIS_ENFORCED: %d, ",
                 aStringBuffer,
                 sConstDesc->mEnforced );

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sIS_VALIDATED: %d, ",
                 aStringBuffer,
                 sConstDesc->mValidate );

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sHAS_INDEX: %d, ",
                 aStringBuffer,
                 (sConstDesc->mKeyIndexHandle.mObjHashElement != NULL) ? 1 : 0 );
    
    /**
     * Constraint 유형별로 정보를 출력해야 함.
     */

    switch (sConstDesc->mConstType)
    {
        case ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY:
            {
                sPrimaryKeyDesc =
                    (ellPrimaryKeyDepDesc *) sConstDesc->mConstTypeDepDesc;
                
                stlSnprintf( aStringBuffer,
                             ELDC_DUMP_PRINT_BUFFER_SIZE,
                             "%sPRIMARY_KEY[ ID: ",
                             aStringBuffer );
            
                for ( i = 0; i < sPrimaryKeyDesc->mKeyColumnCnt; i++ )
                {
                    stlSnprintf( aStringBuffer,
                                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                                 "%s%ld ",
                                 aStringBuffer,
                                 ellGetColumnID(&sPrimaryKeyDesc->mKeyColumnHandle[i]));
                }

                stlSnprintf( aStringBuffer,
                             ELDC_DUMP_PRINT_BUFFER_SIZE,
                             "%s] ",
                             aStringBuffer );
                
                break;
            }
            
        case ELL_TABLE_CONSTRAINT_TYPE_UNIQUE_KEY:
            {
                sUniqueKeyDesc =
                    (ellUniqueKeyDepDesc *) sConstDesc->mConstTypeDepDesc;
                
                stlSnprintf( aStringBuffer,
                             ELDC_DUMP_PRINT_BUFFER_SIZE,
                             "%sUNIQUE_KEY[ ID: ",
                             aStringBuffer );
            
                for ( i = 0; i < sUniqueKeyDesc->mKeyColumnCnt; i++ )
                {
                    stlSnprintf( aStringBuffer,
                                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                                 "%s%ld ",
                                 aStringBuffer,
                                 ellGetColumnID(&sUniqueKeyDesc->mKeyColumnHandle[i]));
                }

                stlSnprintf( aStringBuffer,
                             ELDC_DUMP_PRINT_BUFFER_SIZE,
                             "%s] ",
                             aStringBuffer );
                
                break;
            }
            
        case ELL_TABLE_CONSTRAINT_TYPE_FOREIGN_KEY:
            {
                sForeignKeyDesc =
                    (ellForeignKeyDepDesc *) sConstDesc->mConstTypeDepDesc;
                
                stlSnprintf( aStringBuffer,
                             ELDC_DUMP_PRINT_BUFFER_SIZE,
                             "%sFOREIGN_KEY[ ID: ",
                             aStringBuffer );
            
                for ( i = 0; i < sForeignKeyDesc->mKeyColumnCnt; i++ )
                {
                    stlSnprintf(
                        aStringBuffer,
                        ELDC_DUMP_PRINT_BUFFER_SIZE,
                        "%s%ld ",
                        aStringBuffer,
                        ellGetColumnID( & sForeignKeyDesc->mKeyColumnHandle[i] ) );
                }
                
                stlSnprintf(
                    aStringBuffer,
                    ELDC_DUMP_PRINT_BUFFER_SIZE,
                    "%s, MATCH: %s, UPDATE: %s, DELETE: %s, ",
                    aStringBuffer,
                    gEllReferentialMatchOptionString[sForeignKeyDesc->mMatchOption],
                    gEllReferentialActionRuleString[sForeignKeyDesc->mUpdateRule],
                    gEllReferentialActionRuleString[sForeignKeyDesc->mDeleteRule]
                    );

                stlSnprintf( aStringBuffer,
                             ELDC_DUMP_PRINT_BUFFER_SIZE,
                             "%sREFERENCES ",
                             aStringBuffer );
                
                switch ( ellGetConstraintType( & sForeignKeyDesc->mParentUniqueKeyHandle) )
                {
                    case ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY:
                        stlSnprintf(
                            aStringBuffer,
                            ELDC_DUMP_PRINT_BUFFER_SIZE,
                            "%sPRIMARY_KEY(%ld) ",
                            aStringBuffer,
                            ellGetConstraintID(& sForeignKeyDesc->mParentUniqueKeyHandle) );
                        break;
                    case ELL_TABLE_CONSTRAINT_TYPE_UNIQUE_KEY:
                        stlSnprintf(
                            aStringBuffer,
                            ELDC_DUMP_PRINT_BUFFER_SIZE,
                            "%sUNIQUE_KEY(%ld) ",
                            aStringBuffer,
                            ellGetConstraintID(& sForeignKeyDesc->mParentUniqueKeyHandle) );
                        break;
                    default:
                        break;
                }

                stlSnprintf( aStringBuffer,
                             ELDC_DUMP_PRINT_BUFFER_SIZE,
                             "%s] ",
                             aStringBuffer );
                
                break;
            }
        case ELL_TABLE_CONSTRAINT_TYPE_CHECK_NOT_NULL:
            {
                sCheckNotNullDesc =
                    (ellCheckNotNullDepDesc *) sConstDesc->mConstTypeDepDesc;
                
                stlSnprintf( aStringBuffer,
                             ELDC_DUMP_PRINT_BUFFER_SIZE,
                             "%sCHECK_NOT_NULL[ ID: %ld ] ",
                             aStringBuffer,
                             ellGetColumnID(& sCheckNotNullDesc->mColumnHandle) );
                break;
            }
        case ELL_TABLE_CONSTRAINT_TYPE_CHECK_CLAUSE:
            {
                sCheckClauseDesc =
                    (ellCheckClauseDepDesc *) sConstDesc->mConstTypeDepDesc;
                
                stlSnprintf( aStringBuffer,
                             ELDC_DUMP_PRINT_BUFFER_SIZE,
                             "%sCHECK_CLAUSE[ %s ] ",
                             aStringBuffer,
                             sCheckClauseDesc->mCheckClause );
                break;
            }
        default:
            break;
    }
            

    sSize = stlSnprintf( aStringBuffer,
                         ELDC_DUMP_PRINT_BUFFER_SIZE,
                         "%s) ",
                         aStringBuffer );
    
    /**
     * 문자열 끝 마무리 
     */
    
    if ( (sSize + 1) == ELDC_DUMP_PRINT_BUFFER_SIZE )
    {
        stlSnprintf( & aStringBuffer[ELDC_DUMP_PRINT_NULL_TERMINATE_POSITION],
                     ELDC_DUMP_PRINT_BUFFER_SIZE - ELDC_DUMP_PRINT_NULL_TERMINATE_POSITION,
                     ELDC_DUMP_PRINT_NULL_TERMINATE_STRING );
    }
    else
    {
        // nothing to do
    }
}




/**
 * @brief Constraint ID Cache 의 Hash Data 의 String 출력을 생성하는 함수 포인터 
 * @param[in]  aHashDataConstID    Constraint ID 의 Hash Data
 * @param[out] aStringBuffer       출력 정보를 위한 String Buffer 공간 
 * @remarks
 */
void eldcPrintHashDataConstID( void * aHashDataConstID, stlChar * aStringBuffer )
{
    stlInt32                 sSize = 0;
    eldcHashDataConstID    * sHashData = (eldcHashDataConstID *) aHashDataConstID;

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "Consraint ID Hash Data( " );

    sSize = stlSnprintf( aStringBuffer,
                         ELDC_DUMP_PRINT_BUFFER_SIZE,
                         "%sKEY_CONSTRAINT_ID: %ld )",
                         aStringBuffer,
                         sHashData->mKeyConstID );

    /**
     * 문자열 끝 마무리 
     */
    
    if ( (sSize + 1) == ELDC_DUMP_PRINT_BUFFER_SIZE )
    {
        stlSnprintf( & aStringBuffer[ELDC_DUMP_PRINT_NULL_TERMINATE_POSITION],
                     ELDC_DUMP_PRINT_BUFFER_SIZE - ELDC_DUMP_PRINT_NULL_TERMINATE_POSITION,
                     ELDC_DUMP_PRINT_NULL_TERMINATE_STRING );
    }
    else
    {
        // nothing to do
    }
}


/**
 * @brief Constraint Name Cache 의 Hash Data 의 String 출력을 생성하는 함수 포인터 
 * @param[in]  aHashDataConstName   Constraint Name 의 Hash Data
 * @param[out] aStringBuffer        출력 정보를 위한 String Buffer 공간 
 * @remarks
 */
void eldcPrintHashDataConstName( void * aHashDataConstName, stlChar * aStringBuffer )
{
    stlInt32   sSize = 0;
    eldcHashDataConstName * sHashData = (eldcHashDataConstName *) aHashDataConstName;

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "Constraint Name Hash Data( " );

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sKEY_SCHEMA_ID: %ld, ",
                 aStringBuffer,
                 sHashData->mKeySchemaID );
    
    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sKEY_CONSTRAINT_NAME: %s, ",
                 aStringBuffer,
                 sHashData->mKeyConstName );

    sSize = stlSnprintf( aStringBuffer,
                         ELDC_DUMP_PRINT_BUFFER_SIZE,
                         "%sDATA_CONSTRAINT_ID: %ld )",
                         aStringBuffer,
                         sHashData->mDataConstID );

    /**
     * 문자열 끝 마무리 
     */
    
    if ( (sSize + 1) == ELDC_DUMP_PRINT_BUFFER_SIZE )
    {
        stlSnprintf( & aStringBuffer[ELDC_DUMP_PRINT_NULL_TERMINATE_POSITION],
                     ELDC_DUMP_PRINT_BUFFER_SIZE - ELDC_DUMP_PRINT_NULL_TERMINATE_POSITION,
                     ELDC_DUMP_PRINT_NULL_TERMINATE_STRING );
    }
    else
    {
        // nothing to do
    }
}






/********************************************************
 * 초기화, 마무리 함수
 ********************************************************/


/**
 * @brief SQL-Constraint Cache 를 위한 Dump Handle을 설정
 * @remarks
 */

void eldcSetSQLConstDumpHandle()
{
    /*
     * ELDC_OBJECTCACHE_CONSTRAINT_ID
     */
    
    gEldcObjectDump[ELDC_OBJECTCACHE_CONSTRAINT_ID].mDictHash
        = ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_CONSTRAINT_ID );
    gEldcObjectDump[ELDC_OBJECTCACHE_CONSTRAINT_ID].mPrintHashData
        = eldcPrintHashDataConstID;
    gEldcObjectDump[ELDC_OBJECTCACHE_CONSTRAINT_ID].mPrintObjectDesc
        = eldcPrintConstDesc;

    /*
     * ELDC_OBJECTCACHE_CONSTRAINT_NAME
     */
    
    gEldcObjectDump[ELDC_OBJECTCACHE_CONSTRAINT_NAME].mDictHash
        = ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_CONSTRAINT_NAME );
    gEldcObjectDump[ELDC_OBJECTCACHE_CONSTRAINT_NAME].mPrintHashData
        = eldcPrintHashDataConstName;
    gEldcObjectDump[ELDC_OBJECTCACHE_CONSTRAINT_NAME].mPrintObjectDesc
        = eldcPrintINVALID;
}


/**
 * @brief SQL-Constraint 객체를 위한 Cache 를 구축한다.
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aEnv      Environment
 * @remarks
 */
stlStatus eldcBuildConstCache( smlTransId     aTransID,
                               smlStatement * aStmt,
                               ellEnv       * aEnv )
{
    stlInt64  sBucketCnt = 0;

    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * Hash 의 Bucket 개수를 결정하기 위한 레코드 개수 획득
     */

    STL_TRY( eldcGetTableRecordCount( aTransID,
                                      aStmt,
                                      ELDT_TABLE_ID_TABLE_CONSTRAINTS,
                                      & sBucketCnt,
                                      aEnv )
             == STL_SUCCESS );
    
    /**
     * 소수에 해당하는 Hash Bucket 개수의 조정
     */

    sBucketCnt = ellGetPrimeBucketCnt( sBucketCnt );
    
    /**
     * Constraint ID 를 위한 Cache 초기화 
     */

    STL_TRY( eldcCreateDictHash( ELL_OBJECT_CACHE_ADDR( ELDC_OBJECTCACHE_CONSTRAINT_ID ),
                                 sBucketCnt,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Constraint Name 을 위한 Cache 초기화 
     */
    
    STL_TRY( eldcCreateDictHash( ELL_OBJECT_CACHE_ADDR( ELDC_OBJECTCACHE_CONSTRAINT_NAME ),
                                 sBucketCnt,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Dump Handle 설정
     */
    
    eldcSetSQLConstDumpHandle();
    
    /**
     * SQL-Constraint Cache 구축 
     */

    STL_TRY( eldcBuildCacheSQLConst( aTransID,
                                     aStmt,
                                     aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief SQL-Const 을 위한 Cache 를 구축한다.
 * @param[in]  aTransID  Transaction ID
 * @param[in]  aStmt     Statement
 * @param[in]  aEnv      Environment
 * @remarks
 */
stlStatus eldcBuildCacheSQLConst( smlTransId       aTransID,
                                  smlStatement   * aStmt,
                                  ellEnv         * aEnv )
{
    stlBool             sBeginSelect = STL_FALSE;
    
    void              * sTableHandle = NULL;
    knlValueBlockList * sTableValueList = NULL;

    void                * sIterator  = NULL;
    smlIteratorProperty   sIteratorProperty;

    dtlDataValue      * sDataValue = NULL;
    
    stlInt32            i = 0;

    stlInt64               sConstID = ELL_DICT_OBJECT_ID_NA;
    ellTableConstraintType sConstType = ELL_TABLE_CONSTRAINT_TYPE_NA;

    smlRid       sRowRid;
    smlScn       sRowScn;
    smlRowBlock  sRowBlock;
    eldMemMark   sMemMark;

    smlFetchInfo          sFetchInfo;

    stlInt32 sState = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * DEFINITION_SCHEMA.TABLE_CONSTRAINTS 테이블의 table handle 을 획득
     */
    
    sTableHandle = gEldTablePhysicalHandle[ELDT_TABLE_ID_TABLE_CONSTRAINTS];

    /**
     * DEFINITION_SCHEMA.TABLE_CONSTRAINTS 테이블의 Value List와 Row Block 할당
     */

    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_TABLE_CONSTRAINTS,
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
     * Constraint 유형별로 Cache 를 구축한다.
     * Foreign Key 는 Primary Key 또는 Unique Key 를 참조하므로,
     * Cache 구축 순서를 다음과 같은 순서로 진행한다.
     * Primary Key => Unique Key => Foreign Key => Check Not Null => Check Clause
     */
    
    for ( i = ELL_TABLE_CONSTRAINT_TYPE_NA + 1;
          i < ELL_TABLE_CONSTRAINT_TYPE_MAX;
          i++ )
    {
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
         * Constraint Descriptor 를 Cache 에 추가한다.
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
             * Constraint 유형 검사
             */

            sDataValue = eldFindDataValue( sTableValueList,
                                           ELDT_TABLE_ID_TABLE_CONSTRAINTS,
                                           ELDT_TableConst_ColumnOrder_CONSTRAINT_TYPE_ID );
            if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
            {
                STL_ASSERT(0);
            }
            else
            {
                stlMemcpy( & sConstType,
                           sDataValue->mValue,
                           sDataValue->mLength );
                STL_DASSERT( STL_SIZEOF(sConstType) == sDataValue->mLength );
            }

            /**
             * Constraint Type 유형별로 Cache 정보를 추가
             */
            
            if ( sConstType == i )
            {
                /**
                 * Cache 정보를 추가
                 */
                STL_TRY( eldcInsertCacheConst( aTransID,
                                               aStmt,
                                               sTableValueList,
                                               aEnv )
                         == STL_SUCCESS );

                /**
                 * CONSTRAINT_ID 를 획득
                 */
        
                sDataValue = eldFindDataValue( sTableValueList,
                                               ELDT_TABLE_ID_TABLE_CONSTRAINTS,
                                               ELDT_TableConst_ColumnOrder_CONSTRAINT_ID );
                if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
                {
                    STL_ASSERT(0);
                }
                else
                {
                    stlMemcpy( & sConstID,
                               sDataValue->mValue,
                               sDataValue->mLength );
                    STL_DASSERT( STL_SIZEOF(sConstID) == sDataValue->mLength );
                }

                /**
                 * Constraint 정보를 SQL-Table Cache 에 추가
                 */
                
                STL_TRY( eldcAddConstraintIntoTable( aTransID,
                                                     sConstID,
                                                     aEnv )
                         == STL_SUCCESS );
            }
            else
            {
                continue;
            }

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
    }

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





/********************************************************
 * 조회 함수 
 ********************************************************/




/**
 * @brief Constraint ID 를 이용해 Constraint Dictionary Handle 을 획득 
 * @param[in]  aTransID           Transaction ID
 * @param[in]  aViewSCN           View SCN
 * @param[in]  aConstID           Constraint ID
 * @param[out] aConstDictHandle   Constraint Dictionary Handle
 * @param[out] aExist             존재 여부 
 * @param[in]  aEnv               Envirionment 
 * @remarks
 */
stlStatus eldcGetConstHandleByID( smlTransId           aTransID,
                                  smlScn               aViewSCN,
                                  stlInt64             aConstID,
                                  ellDictHandle      * aConstDictHandle,
                                  stlBool            * aExist,
                                  ellEnv             * aEnv )
{
    eldcHashElement * sHashElement = NULL;

    stlBool  sIsValid = STL_FALSE;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aConstID > ELL_DICT_OBJECT_ID_NA,
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExist != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * ELDC_OBJECTCACHE_CONSTRAINT_ID Hash 로부터 검색
     */

    STL_TRY( eldcFindHashElement( aTransID,
                                  aViewSCN,
                                  ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_CONSTRAINT_ID ),
                                  & aConstID,
                                  eldcHashFuncConstID,
                                  eldcCompareFuncConstID,
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
        /**
         * 유효한 Version 획득
         */
        
        STL_TRY( eldcIsVisibleCache( aTransID,
                                     aViewSCN,
                                     & sHashElement->mVersionInfo,
                                     & sIsValid,
                                     aEnv )
                 == STL_SUCCESS );
        
        if ( sIsValid == STL_TRUE )
        {
            /**
             * Constraint Handle 설정
             */

            ellSetDictHandle( aTransID,
                              aViewSCN,
                              aConstDictHandle,
                              ELL_OBJECT_CONSTRAINT,
                              (void*)sHashElement,
                              STL_TRUE,  /* aRecentCheckable */
                              aEnv );
            
            *aExist = STL_TRUE;
        }
        else
        {
            /**
             * 유효한 Version 이 없음
             */

            *aExist = STL_FALSE;
        }
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief Constraint Name을 이용해 Constraint Dictionary Handle 을 획득 
 * @param[in]  aTransID           Transaction ID
 * @param[in]  aViewSCN           View SCN
 * @param[in]  aSchemaID          Schema ID
 * @param[in]  aConstName         Constraint Name
 * @param[out] aConstDictHandle   Constraint Dictionary Handle
 * @param[out] aExist             존재 여부 
 * @param[in]  aEnv               Envirionment 
 * @remarks
 */
stlStatus eldcGetConstHandleByName( smlTransId           aTransID,
                                    smlScn               aViewSCN,
                                    stlInt64             aSchemaID,
                                    stlChar            * aConstName,
                                    ellDictHandle      * aConstDictHandle,
                                    stlBool            * aExist,
                                    ellEnv             * aEnv )
{
    eldcHashElement        * sHashElement = NULL;

    eldcHashDataConstName  * sHashData = NULL;

    stlBool sIsValid = STL_FALSE;
    
    eldcHashKeyConstName  sHashKey;
    stlMemset( & sHashKey, 0x00, STL_SIZEOF(eldcHashKeyConstName) );
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aSchemaID > ELL_DICT_OBJECT_ID_NA,
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExist != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * ELDC_OBJECTCACHE_CONSTRAINT_NAME Hash 로부터 검색
     */

    sHashKey.mSchemaID  = aSchemaID;
    sHashKey.mConstName = aConstName;
    
    STL_TRY( eldcFindHashElement( aTransID,
                                  aViewSCN,
                                  ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_CONSTRAINT_NAME ),
                                  & sHashKey,
                                  eldcHashFuncConstName,
                                  eldcCompareFuncConstName,
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
        sHashData = (eldcHashDataConstName *) sHashElement->mHashData;
        
        STL_TRY( eldcIsVisibleCache( aTransID,
                                     aViewSCN,
                                     & sHashData->mDataHashElement->mVersionInfo,
                                     & sIsValid,
                                     aEnv )
                 == STL_SUCCESS );
        
        if ( sIsValid == STL_TRUE )
        {
            /**
             * Constraint Handle 설정
             */

            ellSetDictHandle( aTransID,
                              aViewSCN,
                              aConstDictHandle,
                              ELL_OBJECT_CONSTRAINT,
                              (void*)sHashData->mDataHashElement,
                              STL_TRUE,  /* aRecentCheckable */
                              aEnv );
            
            *aExist = STL_TRUE;
        }
        else
        {
            /**
             * 유효한 Version 이 없음
             */

            *aExist = STL_FALSE;
        }
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}








/********************************************************
 * 제어 함수
 ********************************************************/





/**
 * @brief SQL Constraint 을 위한 Cache 정보를 추가한다.
 * @param[in] aTransID          Transaction ID
 * @param[in] aStmt             Statement
 * @param[in] aValueList        Data Value List
 * @param[in] aEnv              Environment
 * @remarks
 */
stlStatus eldcInsertCacheConst( smlTransId          aTransID,
                                smlStatement      * aStmt,
                                knlValueBlockList * aValueList,
                                ellEnv            * aEnv )
{
    ellTableConstDesc   * sConstDesc = NULL;
    
    eldcHashElement * sHashElementConstID = NULL;
    eldcHashElement * sHashElementConstName = NULL;

    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aValueList != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * Constraint ID를 위한 Const Descriptor 구성
     */
    
    STL_TRY( eldcMakeConstDesc( aTransID,
                                aStmt,
                                aValueList,
                                & sConstDesc,
                                aEnv )
             == STL_SUCCESS );
    
    /**
     * Constraint ID 를 위한 Hash Element 구성
     */
    
    STL_TRY( eldcMakeHashElementConstID( & sHashElementConstID,
                                         aTransID,
                                         sConstDesc,
                                         aEnv )
             == STL_SUCCESS );
                                                  
    /**
     * ELDC_OBJECTCACHE_CONSTRAINT_ID Hash 에 추가 
     */
    
    STL_TRY( eldcInsertHashElement( ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_CONSTRAINT_ID ),
                                    sHashElementConstID,
                                    aEnv )
             == STL_SUCCESS );

    /**
     * Constraint Name 을 위한 Hash Element 구성
     */

    STL_TRY( eldcMakeHashElementConstName( & sHashElementConstName,
                                           aTransID,
                                           sConstDesc,
                                           sHashElementConstID,
                                           aEnv )
             == STL_SUCCESS );
    
    /**
     * ELDC_OBJECTCACHE_CONSTRAINT_NAME 에 추가 
     */

    STL_TRY( eldcInsertHashElement( ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_CONSTRAINT_NAME ),
                                    sHashElementConstName,
                                    aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Constraint ID를 기준으로 SQL-Constraint Cache 를 추가한다.
 * @param[in]  aTransID      Transaction ID
 * @param[in]  aStmt         Statement
 * @param[in]  aConstID      Constraint ID
 * @param[in]  aEnv          Environment
 * @remarks
 */
stlStatus eldcInsertCacheConstByConstID( smlTransId          aTransID,
                                         smlStatement      * aStmt,
                                         stlInt64            aConstID,
                                         ellEnv            * aEnv )
{
    stlBool             sFetchOne = STL_FALSE;
    
    stlBool             sBeginSelect = STL_FALSE;

    void              * sTableHandle = NULL;
    knlValueBlockList * sTableValueList = NULL;

    knlValueBlockList * sFilterColumn = NULL;

    void              * sIndexHandle = NULL;
    knlValueBlockList * sIndexValueList = NULL;
    knlKeyCompareList * sKeyCompList = NULL;
    
    void              * sIterator  = NULL;
    smlIteratorProperty sIteratorProperty;

    knlPredicateList    * sRangePred = NULL;
    knlExprContextInfo  * sRangeContext = NULL;
    
    smlRid       sRowRid;
    smlScn       sRowScn;
    smlRowBlock  sRowBlock;
    eldMemMark   sMemMark;

    smlFetchInfo        sFetchInfo;
    smlFetchRecordInfo  sFetchRecordInfo;
        
    stlInt32 sState = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstID > ELL_DICT_OBJECT_ID_NA,
                        ELL_ERROR_STACK(aEnv) );
    
    /**
     * DEFINITION_SCHEMA.TABLE_CONSTRAINTS 테이블의 table handle 을 획득
     */
    
    sTableHandle = gEldTablePhysicalHandle[ELDT_TABLE_ID_TABLE_CONSTRAINTS];

    /**
     * DEFINITION_SCHEMA.TABLE_CONSTRAINTS 테이블의 Value List와 Row Block 할당
     */

    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_TABLE_CONSTRAINTS,
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
     * 조건 생성
     * WHERE CONSTRAINT_ID = aConstID
     */

    sFilterColumn = ellFindColumnValueList( sTableValueList,
                                            ELDT_TABLE_ID_TABLE_CONSTRAINTS,
                                            ELDT_TableConst_ColumnOrder_CONSTRAINT_ID );

    /**
     * Key Range 생성
     */
    
    STL_TRY( eldMakeOneEquiRange( aTransID,
                                  aStmt,
                                  ELDT_TABLE_ID_TABLE_CONSTRAINTS,
                                  sFilterColumn,
                                  & aConstID,
                                  STL_SIZEOF(stlInt64),
                                  & sIndexHandle,
                                  & sKeyCompList,
                                  & sIndexValueList,
                                  & sRangePred,
                                  & sRangeContext,
                                  aEnv )
             == STL_SUCCESS );


    /**
     * SELECT 준비
     */

    STL_TRY( eldBeginForIndexAccessQuery( aStmt,
                                          sTableHandle,
                                          sIndexHandle,
                                          sKeyCompList,
                                          & sIteratorProperty,
                                          & sRowBlock,
                                          sTableValueList,
                                          sIndexValueList,
                                          sRangePred,
                                          sRangeContext,
                                          & sFetchInfo,
                                          & sFetchRecordInfo,
                                          & sIterator,
                                          aEnv )                    
             == STL_SUCCESS );

    sBeginSelect = STL_TRUE;

    /**
     * SQL-Constraints Cache 정보를 추가한다.
     */

    sFetchOne = STL_FALSE;
    while (1)
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
        else
        {
            STL_TRY( eldcInsertCacheConst( aTransID,
                                           aStmt,
                                           sTableValueList,
                                           aEnv )
                     == STL_SUCCESS );

            /* 한 건만 존재해야 한다. */
            STL_ASSERT( sFetchOne == STL_FALSE );
            sFetchOne = STL_TRUE;
            
            continue;
        }
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


/**
 * @brief Table ID를 기준으로 SQL-Constraint Cache 를 추가한다.
 * @param[in]  aTransID      Transaction ID
 * @param[in]  aStmt         Statement
 * @param[in]  aTableID      Table ID
 * @param[in]  aEnv          Environment
 * @remarks
 */
stlStatus eldcInsertCacheConstByTableID( smlTransId          aTransID,
                                         smlStatement      * aStmt,
                                         stlInt64            aTableID,
                                         ellEnv            * aEnv )
{
    smlRid              sRowRid;
    smlScn              sRowScn;
    smlRowBlock         sRowBlock;
    
    stlBool             sBeginSelect = STL_FALSE;

    void              * sTableHandle = NULL;
    knlValueBlockList * sTableValueList = NULL;

    knlValueBlockList * sFilterColumn = NULL;

    void              * sIndexHandle = NULL;
    knlValueBlockList * sIndexValueList = NULL;
    knlKeyCompareList * sKeyCompList = NULL;
    
    void              * sIterator  = NULL;
    smlIteratorProperty sIteratorProperty;

    knlPredicateList    * sRangePred = NULL;
    knlExprContextInfo  * sRangeContext = NULL;

    stlInt32               i = 0;

    dtlDataValue         * sDataValue = NULL;
    
    stlInt64               sConstID = ELL_DICT_OBJECT_ID_NA;
    ellTableConstraintType sConstType = ELL_TABLE_CONSTRAINT_TYPE_NA;

    smlFetchInfo        sFetchInfo;
    smlFetchRecordInfo  sFetchRecordInfo;
    eldMemMark          sMemMark;

    stlInt32  sState = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableID > ELL_DICT_OBJECT_ID_NA,
                        ELL_ERROR_STACK(aEnv) );
    
    /**
     * DEFINITION_SCHEMA.TABLE_CONSTRAINTS 테이블의 table handle 을 획득
     */
    
    sTableHandle = gEldTablePhysicalHandle[ELDT_TABLE_ID_TABLE_CONSTRAINTS];

    /**
     * DEFINITION_SCHEMA.TABLE_CONSTRAINTS 테이블의 Value List와 Row Block 할당
     */

    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_TABLE_CONSTRAINTS,
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
     * 조건 생성
     * WHERE TABLE_ID = aTableID
     */

    sFilterColumn = ellFindColumnValueList( sTableValueList,
                                            ELDT_TABLE_ID_TABLE_CONSTRAINTS,
                                            ELDT_TableConst_ColumnOrder_TABLE_ID );
    
    /**
     * Key Range 생성
     */
    
    STL_TRY( eldMakeOneEquiRange( aTransID,
                                  aStmt,
                                  ELDT_TABLE_ID_TABLE_CONSTRAINTS,
                                  sFilterColumn,
                                  & aTableID,
                                  STL_SIZEOF(stlInt64),
                                  & sIndexHandle,
                                  & sKeyCompList,
                                  & sIndexValueList,
                                  & sRangePred,
                                  & sRangeContext,
                                  aEnv )
             == STL_SUCCESS );
    
    /**
     * Constraint 유형별로 Cache 를 구축한다.
     * Foreign Key 는 Primary Key 또는 Unique Key 를 참조하므로,
     * Cache 구축 순서를 다음과 같은 순서로 진행한다.
     * Primary Key => Unique Key => Foreign Key => Check Not Null => Check Clause
     */
    
    for ( i = ELL_TABLE_CONSTRAINT_TYPE_NA + 1;
          i < ELL_TABLE_CONSTRAINT_TYPE_MAX;
          i++ )
    {
        /**
         * SELECT 준비
         */
        
        STL_TRY( eldBeginForIndexAccessQuery( aStmt,
                                              sTableHandle,
                                              sIndexHandle,
                                              sKeyCompList,
                                              & sIteratorProperty,
                                              & sRowBlock,
                                              sTableValueList,
                                              sIndexValueList,
                                              sRangePred,
                                              sRangeContext,
                                              & sFetchInfo,
                                              & sFetchRecordInfo,
                                              & sIterator,
                                              aEnv )                    
                 == STL_SUCCESS );
        
        sBeginSelect = STL_TRUE;
        
        /**
         * Constraint Descriptor 를 Cache 에 추가한다.
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
             * Constraint 유형 검사
             */

            sDataValue = eldFindDataValue( sTableValueList,
                                           ELDT_TABLE_ID_TABLE_CONSTRAINTS,
                                           ELDT_TableConst_ColumnOrder_CONSTRAINT_TYPE_ID );
            if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
            {
                STL_ASSERT(0);
            }
            else
            {
                stlMemcpy( & sConstType,
                           sDataValue->mValue,
                           sDataValue->mLength );
                STL_DASSERT( STL_SIZEOF(sConstType) == sDataValue->mLength );
            }

            /**
             * Constraint Type 유형별로 Cache 정보를 추가
             */
            
            if ( sConstType == i )
            {
                /**
                 * Cache 정보를 추가
                 */
                STL_TRY( eldcInsertCacheConst( aTransID,
                                               aStmt,
                                               sTableValueList,
                                               aEnv )
                         == STL_SUCCESS );

                /**
                 * CONSTRAINT_ID 를 획득
                 */
        
                sDataValue = eldFindDataValue( sTableValueList,
                                               ELDT_TABLE_ID_TABLE_CONSTRAINTS,
                                               ELDT_TableConst_ColumnOrder_CONSTRAINT_ID );
                if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
                {
                    STL_ASSERT(0);
                }
                else
                {
                    stlMemcpy( & sConstID,
                               sDataValue->mValue,
                               sDataValue->mLength );
                    STL_DASSERT( STL_SIZEOF(sConstID) == sDataValue->mLength );
                }

                /**
                 * Constraint 정보를 SQL-Table Cache 에 추가
                 */
                
                STL_TRY( eldcAddConstraintIntoTable( aTransID,
                                                     sConstID,
                                                     aEnv )
                         == STL_SUCCESS );
            }
            else
            {
                continue;
            }
        }
        
        /**
         * SELECT 종료
         */
        
        sBeginSelect = STL_FALSE;
        STL_TRY( eldEndForQuery( sIterator,
                                 aEnv )
                 == STL_SUCCESS );
    }

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


/**
 * @brief SQL Constraint 를 위한 Cache 정보를 삭제한다.
 * @param[in] aTransID          Transaction ID
 * @param[in] aConstHandle      Constraint Dictionary Handle
 * @param[in] aEnv              Environment
 * @remarks
 */
stlStatus eldcDeleteCacheConst( smlTransId          aTransID,
                                ellDictHandle     * aConstHandle,
                                ellEnv            * aEnv )
{
    eldcHashElement * sHashElement = NULL;

    stlInt64   sConstID = ELL_DICT_OBJECT_ID_NA;
    
    eldcHashKeyConstName  sHashKey;
    stlMemset( & sHashKey, 0x00, STL_SIZEOF(eldcHashKeyConstName) );
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aConstHandle != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * ELDC_OBJECTCACHE_CONSTRAINT_ID Hash 에서 삭제 
     */

    sConstID = ellGetConstraintID( aConstHandle );
    
    STL_TRY( eldcFindHashElement( aTransID,
                                  SML_MAXIMUM_STABLE_SCN,
                                  ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_CONSTRAINT_ID ),
                                  & sConstID,
                                  eldcHashFuncIndexID,
                                  eldcCompareFuncIndexID,
                                  & sHashElement,
                                  aEnv )
             == STL_SUCCESS );

    STL_TRY( eldcDeleteHashElement( aTransID,
                                    ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_CONSTRAINT_ID ),
                                    sHashElement,
                                    aEnv )
             == STL_SUCCESS );
                                    
    /**
     * ELDC_OBJECTCACHE_CONSTRAINT_NAME Hash 에서 삭제 
     */

    sHashKey.mSchemaID  = ellGetConstraintSchemaID( aConstHandle );
    sHashKey.mConstName = ellGetConstraintName( aConstHandle );
    
    STL_TRY( eldcFindHashElement( aTransID,
                                  SML_MAXIMUM_STABLE_SCN,
                                  ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_CONSTRAINT_NAME ),
                                  & sHashKey,
                                  eldcHashFuncColumnName,
                                  eldcCompareFuncColumnName,
                                  & sHashElement,
                                  aEnv )
             == STL_SUCCESS );

    STL_TRY( eldcDeleteHashElement( aTransID,
                                    ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_CONSTRAINT_NAME ),
                                    sHashElement,
                                    aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief SQL-Table Element 에 Constraint Element 를 추가한다.
 * @param[in] aTransID       Transaction ID
 * @param[in] aConstID       Constraint ID
 * @param[in] aEnv           Environment
 * @remarks
 */ 
stlStatus eldcAddConstraintIntoTable( smlTransId     aTransID,
                                      stlInt64       aConstID,
                                      ellEnv       * aEnv )
{
    eldcHashElement     * sTableElement = NULL;
    eldcHashDataTableID * sTableHashData = NULL;
    

    stlBool               sConstExist = STL_FALSE;
    ellTableConstDesc   * sConstDesc = NULL;
    ellDictHandle         sConstHandle;
    stlMemset( & sConstHandle, 0x00, STL_SIZEOF(ellDictHandle) );
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aConstID > ELL_DICT_OBJECT_ID_NA, ELL_ERROR_STACK(aEnv) );

    /**
     * CONSTRAINT_ID 에 해당하는 Constraint Handle 획득
     */
                
    STL_TRY( eldcGetConstHandleByID( aTransID,
                                     SML_MAXIMUM_STABLE_SCN,
                                     aConstID,
                                     & sConstHandle,
                                     & sConstExist,
                                     aEnv )
             == STL_SUCCESS );
    STL_ASSERT( sConstExist == STL_TRUE );
    
    sConstDesc = (ellTableConstDesc *) ellGetObjectDesc( & sConstHandle );
    

    /**
     * ELDC_OBJECTCACHE_TABLE_ID Hash 로부터 검색
     */
    
    STL_TRY( eldcFindHashElement( aTransID,
                                  SML_MAXIMUM_STABLE_SCN,
                                  ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_TABLE_ID ),
                                  & sConstDesc->mTableID,
                                  eldcHashFuncTableID,
                                  eldcCompareFuncTableID,
                                  & sTableElement,
                                  aEnv )
             == STL_SUCCESS );
            
    sTableHashData = (eldcHashDataTableID *) sTableElement->mHashData;

    /**
     * Table Constraint 유형에 따라 Related Object 를 연결 
     */
    
    switch ( sConstDesc->mConstType )
    {
        case ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY:
            {
                /**
                 * Primary Key Constraint 정보를 추가
                 */
                
                STL_TRY( eldcAddHashRelated( aTransID,
                                             sTableElement,
                                             & sTableHashData->mPrimaryKey,
                                             (eldcHashElement*)sConstHandle.mObjHashElement,
                                             ELL_OBJECT_CONSTRAINT,
                                             aEnv )
                         == STL_SUCCESS );
                break;
            }
        case ELL_TABLE_CONSTRAINT_TYPE_UNIQUE_KEY:
            {
                /**
                 * Unique Key Constraint 정보를 추가
                 */
                
                STL_TRY( eldcAddHashRelated( aTransID,
                                             sTableElement,
                                             & sTableHashData->mUniqueKey,
                                             (eldcHashElement*)sConstHandle.mObjHashElement,
                                             ELL_OBJECT_CONSTRAINT,
                                             aEnv )
                         == STL_SUCCESS );
                break;
            }
        case ELL_TABLE_CONSTRAINT_TYPE_FOREIGN_KEY:
            {
                /**
                 * Foreign Key Constraint 정보를 추가
                 */
                        
                STL_TRY( eldcAddHashRelated( aTransID,
                                             sTableElement,
                                             & sTableHashData->mForeignKey,
                                             (eldcHashElement*)sConstHandle.mObjHashElement,
                                             ELL_OBJECT_CONSTRAINT,
                                             aEnv )
                         == STL_SUCCESS );

                /**
                 * Child Foreign Key Constraint 정보를 추가
                 */

                STL_TRY( eldcAddChildForeignKeyIntoTable( aTransID,
                                                          & sConstHandle,
                                                          aEnv )
                         == STL_SUCCESS );
                break;
            }
        case ELL_TABLE_CONSTRAINT_TYPE_CHECK_NOT_NULL:
            {
                /**
                 * Check Not Null Constraint 정보를 추가
                 */
                        
                STL_TRY( eldcAddHashRelated( aTransID,
                                             sTableElement,
                                             & sTableHashData->mCheckNotNull,
                                             (eldcHashElement*)sConstHandle.mObjHashElement,
                                             ELL_OBJECT_CONSTRAINT,
                                             aEnv )
                         == STL_SUCCESS );
                break;
            }
        case ELL_TABLE_CONSTRAINT_TYPE_CHECK_CLAUSE:
            {
                /**
                 * Check Clause Constraint 정보를 추가
                 */
                        
                STL_TRY( eldcAddHashRelated( aTransID,
                                             sTableElement,
                                             & sTableHashData->mCheckClause,
                                             (eldcHashElement*)sConstHandle.mObjHashElement,
                                             ELL_OBJECT_CONSTRAINT,
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

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief SQL-Table Element 에서 Constraint Element 를 삭제한다.
 * @param[in] aTransID       Transaction ID
 * @param[in] aTableHandle   Table Dictionary Handle
 * @param[in] aConstHandle   Constraint Dictionary Handle
 * @param[in] aEnv           Environment
 * @remarks
 */ 
stlStatus eldcDelConstraintFromTable( smlTransId      aTransID,
                                      ellDictHandle * aTableHandle,
                                      ellDictHandle * aConstHandle,
                                      ellEnv        * aEnv )
{
    eldcHashElement     * sTableElement = NULL;
    eldcHashDataTableID * sTableHashData = NULL;
    
    stlInt64               sTableID = ELL_DICT_OBJECT_ID_NA;
    ellTableConstraintType sConstType = ELL_TABLE_CONSTRAINT_TYPE_NA;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aTableHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstHandle != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * ELDC_OBJECTCACHE_TABLE_ID Hash 로부터 검색
     */

    sTableID = ellGetTableID( aTableHandle );
    
    STL_TRY( eldcFindHashElement( aTransID,
                                  SML_MAXIMUM_STABLE_SCN,
                                  ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_TABLE_ID ),
                                  & sTableID,
                                  eldcHashFuncTableID,
                                  eldcCompareFuncTableID,
                                  & sTableElement,
                                  aEnv )
             == STL_SUCCESS );
            
    sTableHashData = (eldcHashDataTableID *) sTableElement->mHashData;

    /**
     * Table Constraint 유형에 따라 Related Object 를 연결 
     */

    sConstType = ellGetConstraintType( aConstHandle );
    
    switch ( sConstType )
    {
        case ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY:
            {
                /**
                 * Primary Key Constraint 정보를 제거 
                 */
                
                STL_TRY( eldcDelHashRelated( aTransID,
                                             sTableElement,
                                             & sTableHashData->mPrimaryKey,
                                             (eldcHashElement*)aConstHandle->mObjHashElement,
                                             aEnv )
                         == STL_SUCCESS );
                break;
            }
        case ELL_TABLE_CONSTRAINT_TYPE_UNIQUE_KEY:
            {
                /**
                 * Unique Key Constraint 정보를 제거 
                 */

                STL_TRY( eldcDelHashRelated( aTransID,
                                             sTableElement,
                                             & sTableHashData->mUniqueKey,
                                             (eldcHashElement*)aConstHandle->mObjHashElement,
                                             aEnv )
                         == STL_SUCCESS );
                break;
            }
        case ELL_TABLE_CONSTRAINT_TYPE_FOREIGN_KEY:
            {
                /**
                 * Foreign Key Constraint 정보를 제거 
                 */
                        
                STL_TRY( eldcDelHashRelated( aTransID,
                                             sTableElement,
                                             & sTableHashData->mForeignKey,
                                             (eldcHashElement*)aConstHandle->mObjHashElement,
                                             aEnv )
                         == STL_SUCCESS );
                
                break;
            }
        case ELL_TABLE_CONSTRAINT_TYPE_CHECK_NOT_NULL:
            {
                /**
                 * Check Not Null Constraint 정보를 제거 
                 */
                        
                STL_TRY( eldcDelHashRelated( aTransID,
                                             sTableElement,
                                             & sTableHashData->mCheckNotNull,
                                             (eldcHashElement*)aConstHandle->mObjHashElement,
                                             aEnv )
                         == STL_SUCCESS );
                
                break;
            }
        case ELL_TABLE_CONSTRAINT_TYPE_CHECK_CLAUSE:
            {
                /**
                 * Check Clause Constraint 정보를 추가
                 */
                        
                STL_TRY( eldcDelHashRelated( aTransID,
                                             sTableElement,
                                             & sTableHashData->mCheckClause,
                                             (eldcHashElement*)aConstHandle->mObjHashElement,
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

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief SQL-Table Element 에 Child Foreign Key Element 를 추가한다.
 * @param[in] aTransID                Transaction ID
 * @param[in] aChildForeignKeyHandle  Child Foreign Key Handle
 * @param[in] aEnv                    Environment
 * @remarks
 */ 
stlStatus eldcAddChildForeignKeyIntoTable( smlTransId      aTransID,
                                           ellDictHandle * aChildForeignKeyHandle,
                                           ellEnv        * aEnv )
{
    eldcHashElement     * sTableElement = NULL;
    eldcHashDataTableID * sTableHashData = NULL;

    ellTableConstDesc   * sChildForeignKeyDesc = NULL;
    ellDictHandle       * sUniqueKeyHandle = NULL;
    stlInt64              sParentTableID = ELL_DICT_OBJECT_ID_NA;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aChildForeignKeyHandle != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Child Foreign Key 가 참조하는 Unique Key Constraint Descriptor 획득 
     */
                
    sChildForeignKeyDesc = (ellTableConstDesc *) ellGetObjectDesc( aChildForeignKeyHandle );
    
    STL_PARAM_VALIDATE( sChildForeignKeyDesc != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( sChildForeignKeyDesc->mConstType == ELL_TABLE_CONSTRAINT_TYPE_FOREIGN_KEY,
                        ELL_ERROR_STACK(aEnv) );

    sUniqueKeyHandle =
        & ((ellForeignKeyDepDesc *) sChildForeignKeyDesc->mConstTypeDepDesc)->mParentUniqueKeyHandle;

    /**
     * ELDC_OBJECTCACHE_TABLE_ID Hash 로부터 Table Hash Data 검색
     */

    sParentTableID = ellGetConstraintTableID( sUniqueKeyHandle );
    STL_TRY( eldcFindHashElement( aTransID,
                                  SML_MAXIMUM_STABLE_SCN,
                                  ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_TABLE_ID ),
                                  & sParentTableID,
                                  eldcHashFuncTableID,
                                  eldcCompareFuncTableID,
                                  & sTableElement,
                                  aEnv )
             == STL_SUCCESS );
            
    sTableHashData = (eldcHashDataTableID *) sTableElement->mHashData;

    /**
     * SQL-Table Cache 에 Child Foreign Key Constraint 정보를 추가
     */
    
    STL_TRY( eldcAddHashRelated( aTransID,
                                 sTableElement,
                                 & sTableHashData->mChildForeignKey,
                                 (eldcHashElement*) aChildForeignKeyHandle->mObjHashElement,
                                 ELL_OBJECT_CONSTRAINT,
                                 aEnv )
             == STL_SUCCESS );


    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief SQL-Table Element 에서 Child Foreign Key Element 를 제거한다.
 * @param[in] aTransID                Transaction ID
 * @param[in] aStmt                   Statement
 * @param[in] aTableHandle            Table Dictionary Handle
 * @param[in] aChildForeignKeyHandle  Child Foreign Key Handle
 * @param[in] aEnv                    Environment
 * @remarks
 */ 
stlStatus eldcDelChildForeignKeyFromTable( smlTransId      aTransID,
                                           smlStatement  * aStmt,
                                           ellDictHandle * aTableHandle,
                                           ellDictHandle * aChildForeignKeyHandle,
                                           ellEnv        * aEnv )
{
    eldcHashElement     * sTableElement = NULL;
    eldcHashDataTableID * sTableHashData = NULL;

    stlInt64  sTableID = ELL_DICT_OBJECT_ID_NA;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aChildForeignKeyHandle != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * ELDC_OBJECTCACHE_TABLE_ID Hash 로부터 Table Hash Data 검색
     */

    sTableID = ellGetTableID( aTableHandle );
    STL_TRY( eldcFindHashElement( aTransID,
                                  SML_MAXIMUM_STABLE_SCN,
                                  ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_TABLE_ID ),
                                  & sTableID,
                                  eldcHashFuncTableID,
                                  eldcCompareFuncTableID,
                                  & sTableElement,
                                  aEnv )
             == STL_SUCCESS );
            
    sTableHashData = (eldcHashDataTableID *) sTableElement->mHashData;

    /**
     * SQL-Table Cache 에서 Child Foreign Key 정보를 제거 
     */
    
    STL_TRY( eldcDelHashRelated( aTransID,
                                 sTableElement,
                                 & sTableHashData->mChildForeignKey,
                                 (eldcHashElement*) aChildForeignKeyHandle->mObjHashElement,
                                 aEnv )
             == STL_SUCCESS );


    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}










/********************************************************
 * 멤버 함수 
 ********************************************************/






/**
 * @brief 읽은 Value List 로부터 SQL Constraint 의 Constraint Descriptor 를 생성한다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[out] aConstDesc       Constraint Descriptor
 * @param[in]  aValueList      Value List
 * @param[in]  aEnv             Environment 포인터
 * @remarks
 */
stlStatus eldcMakeConstDesc( smlTransId           aTransID,
                             smlStatement       * aStmt,
                             knlValueBlockList  * aValueList,
                             ellTableConstDesc ** aConstDesc,
                             ellEnv             * aEnv )
{
    ellTableConstDesc * sConstDesc = NULL;

    dtlDataValue      * sDataValue = NULL;

    stlInt32           sConstDescSize = 0;
    
    stlInt32               sConstNameLen = 0;
    ellTableConstraintType sConstType = ELL_TABLE_CONSTRAINT_TYPE_NA;
    stlInt64               sConstID   = ELL_DICT_OBJECT_ID_NA;
    stlInt32               sCheckClauseLen = 0;

    stlInt64               sIndexID = ELL_DICT_OBJECT_ID_NA;
    stlBool                sIndexExist = STL_FALSE;
    ellDictHandle          sIndexHandle;
    stlMemset( & sIndexHandle, 0x00, STL_SIZEOF(ellDictHandle) );
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aConstDesc != NULL,        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aValueList != NULL      , ELL_ERROR_STACK(aEnv) );

    /**
     * Const Descriptor 를 위한 공간의 크기 계산
     */

    /* Const Name 의 길이 */
    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_TABLE_CONSTRAINTS,
                                   ELDT_TableConst_ColumnOrder_CONSTRAINT_NAME );
    sConstNameLen = sDataValue->mLength;

    /* Table Constraint Type */
    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_TABLE_CONSTRAINTS,
                                   ELDT_TableConst_ColumnOrder_CONSTRAINT_TYPE_ID );
    stlMemcpy( & sConstType,
               sDataValue->mValue,
               sDataValue->mLength );
    STL_DASSERT( STL_SIZEOF(sConstType) == sDataValue->mLength );

    /* Check Clause 구문의 길이 */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_TABLE_CONSTRAINTS,
                                   ELDT_TableConst_ColumnOrder_CONSTRAINT_ID );
    stlMemcpy( & sConstID,
               sDataValue->mValue,
               sDataValue->mLength );
    STL_DASSERT( STL_SIZEOF(sConstID) == sDataValue->mLength );
               
    if ( sConstType == ELL_TABLE_CONSTRAINT_TYPE_CHECK_CLAUSE )
    {
        STL_TRY( eldcGetCheckClauseLen( aTransID,
                                        aStmt,
                                        sConstID,
                                        & sCheckClauseLen,
                                        aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sCheckClauseLen = 0;
    }
    
    sConstDescSize = eldcCalSizeConstDesc( sConstNameLen,
                                           sConstType,
                                           sCheckClauseLen );

    /**
     * Const Descriptor 를 위한 공간 할당
     */
    
    STL_TRY( eldAllocCacheMem( (void **) & sConstDesc,
                               sConstDescSize,
                               aEnv )
             == STL_SUCCESS );
    
    /**
     * Const Descriptor 의 각 정보를 설정한다.
     */

    /*
     * mOwnerID
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_TABLE_CONSTRAINTS,
                                   ELDT_TableConst_ColumnOrder_CONSTRAINT_OWNER_ID );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( & sConstDesc->mConstOwnerID,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sConstDesc->mConstOwnerID) == sDataValue->mLength );
    }

    /*
     * mSchemaID
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_TABLE_CONSTRAINTS,
                                   ELDT_TableConst_ColumnOrder_CONSTRAINT_SCHEMA_ID );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( & sConstDesc->mConstSchemaID,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sConstDesc->mConstSchemaID) == sDataValue->mLength );
    }
    
    /*
     * mConstID
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_TABLE_CONSTRAINTS,
                                   ELDT_TableConst_ColumnOrder_CONSTRAINT_ID );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( & sConstDesc->mConstID,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sConstDesc->mConstID) == sDataValue->mLength );
    }

    /*
     * mTableOwnerID
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_TABLE_CONSTRAINTS,
                                   ELDT_TableConst_ColumnOrder_OWNER_ID );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( & sConstDesc->mTableOwnerID,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sConstDesc->mTableOwnerID) == sDataValue->mLength );
    }
    
    /*
     * mTableSchemaID
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_TABLE_CONSTRAINTS,
                                   ELDT_TableConst_ColumnOrder_SCHEMA_ID );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( & sConstDesc->mTableSchemaID,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sConstDesc->mTableSchemaID) == sDataValue->mLength );
    }
    
    /*
     * mTableID
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_TABLE_CONSTRAINTS,
                                   ELDT_TableConst_ColumnOrder_TABLE_ID );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( & sConstDesc->mTableID,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sConstDesc->mTableID) == sDataValue->mLength );
    }
    
    
    /*
     * mConstName
     * - 메모리 공간 내에서의 위치 계산
     */

    sConstDesc->mConstName = eldcGetConstNamePtr( sConstDesc );

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_TABLE_CONSTRAINTS,
                                   ELDT_TableConst_ColumnOrder_CONSTRAINT_NAME );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( sConstDesc->mConstName,
                   sDataValue->mValue,
                   sDataValue->mLength );
        sConstDesc->mConstName[sDataValue->mLength] = '\0';
    }

    /*
     * mConstType
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_TABLE_CONSTRAINTS,
                                   ELDT_TableConst_ColumnOrder_CONSTRAINT_TYPE_ID );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( & sConstDesc->mConstType,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sConstDesc->mConstType) == sDataValue->mLength );
    }

    /*
     * mDeferrable
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_TABLE_CONSTRAINTS,
                                   ELDT_TableConst_ColumnOrder_IS_DEFERRABLE );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( & sConstDesc->mDeferrable,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sConstDesc->mDeferrable) == sDataValue->mLength );
    }

    /*
     * mInitDeferred
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_TABLE_CONSTRAINTS,
                                   ELDT_TableConst_ColumnOrder_INITIALLY_DEFERRED );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( & sConstDesc->mInitDeferred,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sConstDesc->mInitDeferred) == sDataValue->mLength );
    }

    /*
     * mEnforced
     */ 

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_TABLE_CONSTRAINTS,
                                   ELDT_TableConst_ColumnOrder_ENFORCED );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( & sConstDesc->mEnforced,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sConstDesc->mEnforced) == sDataValue->mLength );
    }

    /*
     * mValidate
     */ 

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_TABLE_CONSTRAINTS,
                                   ELDT_TableConst_ColumnOrder_VALIDATE );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( & sConstDesc->mValidate,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sConstDesc->mValidate) == sDataValue->mLength );
    }

    /*
     * mKeyIndexHandle
     */ 

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_TABLE_CONSTRAINTS,
                                   ELDT_TableConst_ColumnOrder_ASSOCIATED_INDEX_ID );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        ellInitDictHandle( & sConstDesc->mKeyIndexHandle );
    }
    else
    {
        stlMemcpy( & sIndexID,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sIndexID) == sDataValue->mLength );

        STL_TRY( eldGetIndexHandleByID( aTransID,
                                        SML_MAXIMUM_STABLE_SCN,
                                        sIndexID,
                                        & sConstDesc->mKeyIndexHandle,
                                        & sIndexExist,
                                        aEnv )
                 == STL_SUCCESS );

        STL_ASSERT( sIndexExist == STL_TRUE );
    }

    /*
     * mConstTypeDepDesc
     */

    sConstDesc->mConstTypeDepDesc = eldcGetConstTypePtr( sConstDesc, sConstNameLen );
                                                         
    switch( sConstDesc->mConstType )
    {
        case ELL_TABLE_CONSTRAINT_TYPE_PRIMARY_KEY:
            {
                STL_TRY( eldcSetPrimaryKey( aTransID,
                                            aStmt,
                                            sConstDesc,
                                            aEnv )
                         == STL_SUCCESS );
                break;
            }
        case ELL_TABLE_CONSTRAINT_TYPE_UNIQUE_KEY:
            {
                STL_TRY( eldcSetUniqueKey( aTransID,
                                           aStmt,
                                           sConstDesc,
                                           aEnv )
                         == STL_SUCCESS );
                break;
            }
        case ELL_TABLE_CONSTRAINT_TYPE_FOREIGN_KEY:
            {
                STL_TRY( eldcSetForeignKey( aTransID,
                                            aStmt,
                                            sConstDesc,
                                            aEnv )
                         == STL_SUCCESS );
                break;
            }
        case ELL_TABLE_CONSTRAINT_TYPE_CHECK_NOT_NULL:
            {
                STL_TRY( eldcSetCheckNotNull( aTransID,
                                              aStmt,
                                              sConstDesc,
                                              aEnv )
                         == STL_SUCCESS );
                break;
            }
        case ELL_TABLE_CONSTRAINT_TYPE_CHECK_CLAUSE:
            {
                ((ellCheckClauseDepDesc*) sConstDesc->mConstTypeDepDesc)->mCheckClause
                    = eldcGetCheckClausePtr( sConstDesc, sConstNameLen );
                
                STL_TRY( eldcSetCheckClause( aTransID,
                                             aStmt,
                                             sConstDesc,
                                             aEnv )
                         == STL_SUCCESS );
                break;
            }
        default:
            STL_ASSERT(0);
            break;
    }
    
    /**
     * return 값 설정
     */
    
    *aConstDesc = sConstDesc;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Constraint ID 를 위한 Hash Element 를 구성한다.
 * @param[out] aHashElement    Hash Element
 * @param[in]  aTransID        Transaction ID
 * @param[in]  aConstDesc      Constraint Descriptor
 * @param[in]  aEnv            Environment
 * @remarks
 */ 
stlStatus eldcMakeHashElementConstID( eldcHashElement   ** aHashElement,
                                      smlTransId           aTransID,
                                      ellTableConstDesc  * aConstDesc,
                                      ellEnv             * aEnv )
{
    stlInt32 sHashDataSize = 0;

    stlUInt32              sHashValue = 0;
    eldcHashDataConstID  * sHashDataConstID = NULL;
    eldcHashElement      * sHashElement      = NULL;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aHashElement != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstDesc != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Hash Data 생성
     */

    sHashDataSize = eldcCalSizeHashDataConstID();
    
    /**
     * Shared Cache 메모리 관리자로부터 할당
     */
    
    STL_TRY( eldAllocCacheMem( (void **) & sHashDataConstID,
                               sHashDataSize,
                               aEnv )
             == STL_SUCCESS );

    stlMemset( sHashDataConstID, 0x00, sHashDataSize );
    
    sHashDataConstID->mKeyConstID = aConstDesc->mConstID;

    /**
     * Hash Value 생성
     */
    
    sHashValue = eldcHashFuncConstID( & aConstDesc->mConstID );
    
    /**
     * Hash Element 생성
     */

    STL_TRY( eldcMakeHashElement( & sHashElement,
                                  aTransID,
                                  sHashDataConstID,
                                  sHashValue,
                                  aConstDesc,
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


/**
 * @brief Constraint Name 을 위한 Hash Element 를 구성한다.
 * @param[out] aHashElement    Hash Element
 * @param[in]  aTransID        Transaction ID
 * @param[in]  aConstDesc      Constraint Descriptor
 * @param[in]  aHashElementID  Constraint ID로부터 구축된 Hash Element
 * @param[in]  aEnv            Environment 
 * @remarks
 */ 
stlStatus eldcMakeHashElementConstName( eldcHashElement   ** aHashElement,
                                        smlTransId           aTransID,
                                        ellTableConstDesc  * aConstDesc,
                                        eldcHashElement    * aHashElementID,
                                        ellEnv             * aEnv )
{
    stlInt32 sHashDataSize = 0;

    stlUInt32                sHashValue = 0;
    eldcHashDataConstName  * sHashDataConstName = NULL;
    eldcHashElement        * sHashElement        = NULL;

    eldcHashKeyConstName     sHashKey;
    stlMemset( & sHashKey, 0x00, STL_SIZEOF(eldcHashKeyConstName) );
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aHashElement != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstDesc != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aHashElementID != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Hash Data 생성
     */

    sHashDataSize = eldcCalSizeHashDataConstName( stlStrlen(aConstDesc->mConstName) );
    
    /**
     * Shared Cache 메모리 관리자로부터 할당
     */

    STL_TRY( eldAllocCacheMem( (void **) & sHashDataConstName,
                               sHashDataSize,
                               aEnv )
             == STL_SUCCESS );

    stlMemset( sHashDataConstName, 0x00, sHashDataSize );
    
    sHashDataConstName->mKeySchemaID = aConstDesc->mConstSchemaID;
    
    /* Const Name 을 복사할 위치 지정 */
    sHashDataConstName->mKeyConstName = eldcGetHashDataConstNamePtr( sHashDataConstName );
    
    stlStrcpy( sHashDataConstName->mKeyConstName,
               aConstDesc->mConstName );

    sHashDataConstName->mDataConstID = aConstDesc->mConstID;
    sHashDataConstName->mDataHashElement = aHashElementID;

    /**
     * Hash Value 생성
     */

    sHashKey.mSchemaID = sHashDataConstName->mKeySchemaID;
    sHashKey.mConstName = sHashDataConstName->mKeyConstName;
    sHashValue = eldcHashFuncConstName( & sHashKey );
    
    /**
     * Hash Element 생성
     */

    STL_TRY( eldcMakeHashElement( & sHashElement,
                                  aTransID,
                                  sHashDataConstName,
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


/**
 * @brief Check Clause 구문의 길이를 얻는다.
 * @param[in]  aTransID            Transaction ID
 * @param[in]  aStmt               Statement
 * @param[in]  aCheckClauseConstID Constraint ID
 * @param[out] aCheckClauseLen     Check Clause 구문의 길이
 * @param[in]  aEnv                Environment
 * @remarks
 */
stlStatus eldcGetCheckClauseLen( smlTransId           aTransID,
                                 smlStatement       * aStmt,
                                 stlInt64             aCheckClauseConstID,
                                 stlInt32           * aCheckClauseLen,
                                 ellEnv             * aEnv )
{
    smlRid              sRowRid;
    smlScn              sRowScn;
    smlRowBlock         sRowBlock;
    stlBool             sFetchOne = STL_FALSE;
    
    stlBool             sBeginSelect = STL_FALSE;

    void              * sTableHandle = NULL;
    knlValueBlockList * sTableValueList = NULL;

    void                * sIterator  = NULL;
    smlIteratorProperty   sIteratorProperty;

    knlValueBlockList   * sFilterColumn = NULL;

    void              * sIndexHandle = NULL;
    knlValueBlockList * sIndexValueList = NULL;
    knlKeyCompareList * sKeyCompList = NULL;
    
    knlPredicateList    * sRangePred = NULL;
    knlExprContextInfo  * sRangeContext = NULL;

    dtlDataValue      * sDataValue = NULL;
    stlInt64            sClauseLen = 0;

    smlFetchInfo        sFetchInfo;
    smlFetchRecordInfo  sFetchRecordInfo;
    eldMemMark          sMemMark;
    
    stlInt32 sState = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCheckClauseConstID > ELL_DICT_OBJECT_ID_NA,
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCheckClauseLen != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * DEFINITION_SCHEMA.CHECK_CONSTRAINTS 테이블의 table handle 을 획득
     */
    
    sTableHandle = gEldTablePhysicalHandle[ELDT_TABLE_ID_CHECK_CONSTRAINTS];

    /**
     * DEFINITION_SCHEMA.CHECK_CONSTRAINTS 테이블의 Value List와 Row Block 할당
     */

    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_CHECK_CONSTRAINTS,
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
     * 조건 생성
     * WHERE CONSTRAINT_ID = aCheckClauseConstID
     */

    sFilterColumn = ellFindColumnValueList( sTableValueList,
                                            ELDT_TABLE_ID_CHECK_CONSTRAINTS,
                                            ELDT_CheckConst_ColumnOrder_CONSTRAINT_ID );

    if ( gEldCacheBuilding == STL_TRUE )
    {
        /**
         * Start-Up 수행 시간이 View 개수의 제곱에 비례하여 증가하게 된다.
         * Index Cache 가 존재하지 않아 General 한 Key Range 방식이 아닌
         * hard-coded key range 로 start-up 성능 문제를 해결한다.
         */

        STL_TRY( eldcMakeNonCacheRange( aTransID,
                                        aStmt,
                                        sFilterColumn,
                                        aCheckClauseConstID,
                                        & sKeyCompList,
                                        & sIndexValueList,
                                        & sRangePred,
                                        & sRangeContext,
                                        aEnv )
                 == STL_SUCCESS );
        
        sIndexHandle = gEldNonCacheIndexPhyHandle4CheckConstID;
    }
    else
    {
        /**
         * Key Range 생성
         */

        STL_TRY( eldMakeOneEquiRange( aTransID,
                                      aStmt,
                                      ELDT_TABLE_ID_CHECK_CONSTRAINTS,
                                      sFilterColumn,
                                      & aCheckClauseConstID,
                                      STL_SIZEOF(stlInt64),
                                      & sIndexHandle,
                                      & sKeyCompList,
                                      & sIndexValueList,
                                      & sRangePred,
                                      & sRangeContext,
                                      aEnv )
                 == STL_SUCCESS );
    }

    /**
     * SELECT 준비
     */
    
    STL_TRY( eldBeginForIndexAccessQuery( aStmt,
                                          sTableHandle,
                                          sIndexHandle,
                                          sKeyCompList,
                                          & sIteratorProperty,
                                          & sRowBlock,
                                          sTableValueList,
                                          sIndexValueList,
                                          sRangePred,
                                          sRangeContext,
                                          & sFetchInfo,
                                          & sFetchRecordInfo,
                                          & sIterator,
                                          aEnv )                    
                 == STL_SUCCESS );
    
    sBeginSelect = STL_TRUE;

    /**
     * Check Clause 구문의 길이를 구한다.
     */

    sFetchOne = STL_FALSE;
    while (1)
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
        else
        {
            sDataValue = eldFindDataValue( sTableValueList,
                                           ELDT_TABLE_ID_CHECK_CONSTRAINTS,
                                           ELDT_CheckConst_ColumnOrder_CHECK_CLAUSE );
            if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
            {
                STL_ASSERT(0);
            }
            else
            {
                sClauseLen = sDataValue->mLength;
            }
            

            /* 한 건만 존재해야 한다. */
            STL_ASSERT( sFetchOne == STL_FALSE );
            sFetchOne = STL_TRUE;
            
            continue;
        }
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
    
    /**
     * Output 설정
     */

    *aCheckClauseLen = sClauseLen;
    
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


/**
 * @brief Check Not Null Constraint 정보를 설정한다.
 * @param[in]  aTransID    Transaction ID
 * @param[in]  aStmt       Statement
 * @param[in]  aConstDesc  Constraint Descriptor
 * @param[in]  aEnv        Environment
 * @remarks
 */
stlStatus eldcSetCheckNotNull( smlTransId           aTransID,
                               smlStatement       * aStmt,
                               ellTableConstDesc  * aConstDesc,
                               ellEnv             * aEnv )
{
    smlRid              sRowRid;
    smlScn              sRowScn;
    smlRowBlock         sRowBlock;
    stlBool             sFetchOne = STL_FALSE;
    
    stlBool             sBeginSelect = STL_FALSE;

    void              * sTableHandle = NULL;
    knlValueBlockList * sTableValueList = NULL;

    void                * sIterator  = NULL;
    smlIteratorProperty   sIteratorProperty;

    knlValueBlockList   * sFilterColumn = NULL;

    void              * sIndexHandle = NULL;
    knlValueBlockList * sIndexValueList = NULL;
    knlKeyCompareList * sKeyCompList = NULL;

    knlPredicateList    * sRangePred = NULL;
    knlExprContextInfo  * sRangeContext = NULL;
    
    dtlDataValue           * sDataValue = NULL;
    ellCheckNotNullDepDesc * sCheckNotNullDesc = NULL;

    stlBool     sColumnExist = STL_FALSE;
    stlInt64    sColumnID = ELL_DICT_OBJECT_ID_NA;

    smlFetchInfo        sFetchInfo;
    smlFetchRecordInfo  sFetchRecordInfo;
    eldMemMark          sMemMark;

    stlInt32 sState = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstDesc != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * DEFINITION_SCHEMA.CHECK_COLUMN_USAGE 테이블의 table handle 을 획득
     */
    
    sTableHandle = gEldTablePhysicalHandle[ELDT_TABLE_ID_CHECK_COLUMN_USAGE];

    /**
     * DEFINITION_SCHEMA.CHECK_COLUMN_USAGE 테이블의 Value List와 Row Block 할당
     */

    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_CHECK_COLUMN_USAGE,
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
     * 조건 생성
     * WHERE CONSTRAINT_ID = aConstDesc->mConstID
     */

    sFilterColumn = ellFindColumnValueList( sTableValueList,
                                            ELDT_TABLE_ID_CHECK_COLUMN_USAGE,
                                            ELDT_CheckColumn_ColumnOrder_CONSTRAINT_ID );

    if ( gEldCacheBuilding == STL_TRUE )
    {
        /**
         * Start-Up 수행 시간이 View 개수의 제곱에 비례하여 증가하게 된다.
         * Index Cache 가 존재하지 않아 General 한 Key Range 방식이 아닌
         * hard-coded key range 로 start-up 성능 문제를 해결한다.
         */

        STL_TRY( eldcMakeNonCacheRange( aTransID,
                                        aStmt,
                                        sFilterColumn,
                                        aConstDesc->mConstID,
                                        & sKeyCompList,
                                        & sIndexValueList,
                                        & sRangePred,
                                        & sRangeContext,
                                        aEnv )
                 == STL_SUCCESS );
        
        sIndexHandle = gEldNonCacheIndexPhyHandle4ChkColConstID;
        
    }
    else
    {
        /**
         * Key Range 생성
         */

        STL_TRY( eldMakeOneEquiRange( aTransID,
                                      aStmt,
                                      ELDT_TABLE_ID_CHECK_COLUMN_USAGE,
                                      sFilterColumn,
                                      & aConstDesc->mConstID,
                                      STL_SIZEOF(stlInt64),
                                      & sIndexHandle,
                                      & sKeyCompList,
                                      & sIndexValueList,
                                      & sRangePred,
                                      & sRangeContext,
                                      aEnv )
                 == STL_SUCCESS );
    }

    /**
     * SELECT 준비
     */
    
    STL_TRY( eldBeginForIndexAccessQuery( aStmt,
                                          sTableHandle,
                                          sIndexHandle,
                                          sKeyCompList,
                                          & sIteratorProperty,
                                          & sRowBlock,
                                          sTableValueList,
                                          sIndexValueList,
                                          sRangePred,
                                          sRangeContext,
                                          & sFetchInfo,
                                          & sFetchRecordInfo,
                                          & sIterator,
                                          aEnv )                    
             == STL_SUCCESS );
                                      
    sBeginSelect = STL_TRUE;

    /**
     * Check Not Null 의 대상 Column ID 를 설정한다.
     */

    sFetchOne = STL_FALSE;
    while (1)
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
        else
        {
            sDataValue = eldFindDataValue( sTableValueList,
                                           ELDT_TABLE_ID_CHECK_COLUMN_USAGE,
                                           ELDT_CheckColumn_ColumnOrder_COLUMN_ID );
            if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
            {
                STL_ASSERT(0);
            }
            else
            {
                sCheckNotNullDesc =
                    (ellCheckNotNullDepDesc *) aConstDesc->mConstTypeDepDesc;
                
                stlMemcpy( & sColumnID,
                           sDataValue->mValue,
                           sDataValue->mLength );
                STL_DASSERT( STL_SIZEOF(sColumnID) == sDataValue->mLength );
            }

            STL_TRY( ellGetColumnDictHandleByID( aTransID,
                                                 SML_MAXIMUM_STABLE_SCN,
                                                 sColumnID,
                                                 & sCheckNotNullDesc->mColumnHandle,
                                                 & sColumnExist,
                                                 aEnv )
                     == STL_SUCCESS );
            STL_ASSERT( sColumnExist == STL_TRUE );

            /* 한 건만 존재해야 한다. */
            STL_ASSERT( sFetchOne == STL_FALSE );
            sFetchOne = STL_TRUE;
            
            continue;
        }
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



/**
 * @brief Check Clause Constraint 정보를 설정한다.
 * @param[in]  aTransID    Transaction ID
 * @param[in]  aStmt       Statement
 * @param[in]  aConstDesc  Constraint Descriptor
 * @param[in]  aEnv        Environment
 * @remarks
 */
stlStatus eldcSetCheckClause( smlTransId           aTransID,
                              smlStatement       * aStmt,
                              ellTableConstDesc  * aConstDesc,
                              ellEnv             * aEnv )
{
    smlRid              sRowRid;
    smlScn              sRowScn;
    smlRowBlock         sRowBlock;
    stlBool             sFetchOne = STL_FALSE;
    
    stlBool             sBeginSelect = STL_FALSE;

    void              * sTableHandle = NULL;
    knlValueBlockList * sTableValueList = NULL;

    void                * sIterator  = NULL;
    smlIteratorProperty   sIteratorProperty;

    knlValueBlockList   * sFilterColumn = NULL;

    void              * sIndexHandle = NULL;
    knlValueBlockList * sIndexValueList = NULL;
    knlKeyCompareList * sKeyCompList = NULL;
    
    knlPredicateList    * sRangePred = NULL;
    knlExprContextInfo  * sRangeContext = NULL;
    
    dtlDataValue          * sDataValue = NULL;
    ellCheckClauseDepDesc * sCheckClauseDesc = NULL;

    smlFetchInfo        sFetchInfo;
    smlFetchRecordInfo  sFetchRecordInfo;
    eldMemMark          sMemMark;

    stlInt32 sState = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstDesc != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * DEFINITION_SCHEMA.CHECK_CONSTRAINTS 테이블의 table handle 을 획득
     */
    
    sTableHandle = gEldTablePhysicalHandle[ELDT_TABLE_ID_CHECK_CONSTRAINTS];

    /**
     * DEFINITION_SCHEMA.CHECK_CONSTRAINTS 테이블의 Value List와 Row Block 할당
     */

    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_CHECK_CONSTRAINTS,
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
     * 조건 생성
     * WHERE CONSTRAINT_ID = aConstDesc->mConstID
     */

    sFilterColumn = ellFindColumnValueList( sTableValueList,
                                            ELDT_TABLE_ID_CHECK_CONSTRAINTS,
                                            ELDT_CheckConst_ColumnOrder_CONSTRAINT_ID );

    if ( gEldCacheBuilding == STL_TRUE )
    {
        /**
         * Start-Up 수행 시간이 View 개수의 제곱에 비례하여 증가하게 된다.
         * Index Cache 가 존재하지 않아 General 한 Key Range 방식이 아닌
         * hard-coded key range 로 start-up 성능 문제를 해결한다.
         */

        STL_TRY( eldcMakeNonCacheRange( aTransID,
                                        aStmt,
                                        sFilterColumn,
                                        aConstDesc->mConstID,
                                        & sKeyCompList,
                                        & sIndexValueList,
                                        & sRangePred,
                                        & sRangeContext,
                                        aEnv )
                 == STL_SUCCESS );
        
        sIndexHandle = gEldNonCacheIndexPhyHandle4CheckConstID;
    }
    else
    {
        /**
         * Key Range 생성
         */

        STL_TRY( eldMakeOneEquiRange( aTransID,
                                      aStmt,
                                      ELDT_TABLE_ID_CHECK_CONSTRAINTS,
                                      sFilterColumn,
                                      & aConstDesc->mConstID,
                                      STL_SIZEOF(stlInt64),
                                      & sIndexHandle,
                                      & sKeyCompList,
                                      & sIndexValueList,
                                      & sRangePred,
                                      & sRangeContext,
                                      aEnv )
                 == STL_SUCCESS );
    }

    /**
     * SELECT 준비
     */

    STL_TRY( eldBeginForIndexAccessQuery( aStmt,
                                          sTableHandle,
                                          sIndexHandle,
                                          sKeyCompList,
                                          & sIteratorProperty,
                                          & sRowBlock,
                                          sTableValueList,
                                          sIndexValueList,
                                          sRangePred,
                                          sRangeContext,
                                          & sFetchInfo,
                                          & sFetchRecordInfo,
                                          & sIterator,
                                          aEnv )                    
             == STL_SUCCESS );
                                      
    sBeginSelect = STL_TRUE;

    /**
     * Check Clause 의 대상 구문 정보를 설정한다.
     */

    sFetchOne = STL_FALSE;
    while (1)
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
        else
        {
            sDataValue = eldFindDataValue( sTableValueList,
                                           ELDT_TABLE_ID_CHECK_CONSTRAINTS,
                                           ELDT_CheckConst_ColumnOrder_CHECK_CLAUSE );
            if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
            {
                STL_ASSERT(0);
            }
            else
            {
                sCheckClauseDesc =
                    (ellCheckClauseDepDesc *) aConstDesc->mConstTypeDepDesc;
                
                stlMemcpy( sCheckClauseDesc->mCheckClause,
                           sDataValue->mValue,
                           sDataValue->mLength );
            }
            
            /* 한 건만 존재해야 한다. */
            STL_ASSERT( sFetchOne == STL_FALSE );
            sFetchOne = STL_TRUE;
            
            continue;
        }
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



/**
 * @brief Primary Key Constraint 정보를 설정한다.
 * @param[in]  aTransID    Transaction ID
 * @param[in]  aStmt       Statement
 * @param[in]  aConstDesc  Constraint Descriptor
 * @param[in]  aEnv        Environment
 * @remarks
 */
stlStatus eldcSetPrimaryKey( smlTransId           aTransID,
                             smlStatement       * aStmt,
                             ellTableConstDesc  * aConstDesc,
                             ellEnv             * aEnv )
{
    smlRid              sRowRid;
    smlScn              sRowScn;
    smlRowBlock         sRowBlock;
    
    stlBool             sBeginSelect = STL_FALSE;

    void              * sTableHandle = NULL;
    knlValueBlockList * sTableValueList = NULL;

    void                * sIterator  = NULL;
    smlIteratorProperty   sIteratorProperty;

    knlValueBlockList   * sFilterColumn = NULL;

    void              * sIndexHandle = NULL;
    knlValueBlockList * sIndexValueList = NULL;
    knlKeyCompareList * sKeyCompList = NULL;
    
    knlPredicateList    * sRangePred = NULL;
    knlExprContextInfo  * sRangeContext = NULL;
    
    dtlDataValue          * sDataValue = NULL;
    ellPrimaryKeyDepDesc  * sPrimaryKeyDesc = NULL;
    stlInt32                sColumnIdx = 0;

    stlBool    sColumnExist = STL_FALSE;
    stlInt64   sColumnID = ELL_DICT_OBJECT_ID_NA;

    smlFetchInfo        sFetchInfo;
    smlFetchRecordInfo  sFetchRecordInfo;
    eldMemMark          sMemMark;

    stlInt32 sState = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstDesc != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * DEFINITION_SCHEMA.KEY_COLUMN_USAGE 테이블의 table handle 을 획득
     */
    
    sTableHandle = gEldTablePhysicalHandle[ELDT_TABLE_ID_KEY_COLUMN_USAGE];

    /**
     * DEFINITION_SCHEMA.KEY_COLUMN_USAGE 테이블의 Value List와 Row Block 할당
     */

    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_KEY_COLUMN_USAGE,
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
     * 조건 생성
     * WHERE CONSTRAINT_ID = aConstDesc->mConstID
     */

    sFilterColumn = ellFindColumnValueList( sTableValueList,
                                            ELDT_TABLE_ID_KEY_COLUMN_USAGE,
                                            ELDT_KeyColumn_ColumnOrder_CONSTRAINT_ID );

    if ( gEldCacheBuilding == STL_TRUE )
    {
        /**
         * Start-Up 수행 시간이 View 개수의 제곱에 비례하여 증가하게 된다.
         * Index Cache 가 존재하지 않아 General 한 Key Range 방식이 아닌
         * hard-coded key range 로 start-up 성능 문제를 해결한다.
         */

        STL_TRY( eldcMakeNonCacheRange( aTransID,
                                        aStmt,
                                        sFilterColumn,
                                        aConstDesc->mConstID,
                                        & sKeyCompList,
                                        & sIndexValueList,
                                        & sRangePred,
                                        & sRangeContext,
                                        aEnv )
                 == STL_SUCCESS );
        
        sIndexHandle = gEldNonCacheIndexPhyHandle4KeyColConstID;
    }
    else
    {
        /**
         * Key Range 생성
         */

        STL_TRY( eldMakeOneEquiRange( aTransID,
                                      aStmt,
                                      ELDT_TABLE_ID_KEY_COLUMN_USAGE,
                                      sFilterColumn,
                                      & aConstDesc->mConstID,
                                      STL_SIZEOF(stlInt64),
                                      & sIndexHandle,
                                      & sKeyCompList,
                                      & sIndexValueList,
                                      & sRangePred,
                                      & sRangeContext,
                                      aEnv )
                 == STL_SUCCESS );
    }

    /**
     * SELECT 준비
     */
    
    STL_TRY( eldBeginForIndexAccessQuery( aStmt,
                                          sTableHandle,
                                          sIndexHandle,
                                          sKeyCompList,
                                          & sIteratorProperty,
                                          & sRowBlock,
                                          sTableValueList,
                                          sIndexValueList,
                                          sRangePred,
                                          sRangeContext,
                                          & sFetchInfo,
                                          & sFetchRecordInfo,
                                          & sIterator,
                                          aEnv )                    
             == STL_SUCCESS );

    sBeginSelect = STL_TRUE;

    /**
     * Primary Key 의 정보를 설정한다.
     */

    sPrimaryKeyDesc = (ellPrimaryKeyDepDesc *) aConstDesc->mConstTypeDepDesc;
    sPrimaryKeyDesc->mKeyColumnCnt = 0;
    
    while (1)
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
        else
        {
            sDataValue = eldFindDataValue( sTableValueList,
                                           ELDT_TABLE_ID_KEY_COLUMN_USAGE,
                                           ELDT_KeyColumn_ColumnOrder_ORDINAL_POSITION );
            if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
            {
                STL_ASSERT(0);
            }
            else
            {
                stlMemcpy( & sColumnIdx,
                           sDataValue->mValue,
                           sDataValue->mLength );
                STL_DASSERT( STL_SIZEOF(sColumnIdx) == sDataValue->mLength );
                
                sColumnIdx = ELD_ORDINAL_POSITION_TO_PROCESS(sColumnIdx);
            }
            
            sDataValue = eldFindDataValue( sTableValueList,
                                           ELDT_TABLE_ID_KEY_COLUMN_USAGE,
                                           ELDT_KeyColumn_ColumnOrder_COLUMN_ID );
            if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
            {
                STL_ASSERT(0);
            }
            else
            {
                
                stlMemcpy( & sColumnID,
                           sDataValue->mValue,
                           sDataValue->mLength );
                STL_DASSERT( STL_SIZEOF(sColumnID) == sDataValue->mLength );
            }

            STL_TRY( ellGetColumnDictHandleByID(
                         aTransID,
                         SML_MAXIMUM_STABLE_SCN,
                         sColumnID,
                         & sPrimaryKeyDesc->mKeyColumnHandle[sColumnIdx],
                         & sColumnExist,
                         aEnv )
                     == STL_SUCCESS );
            STL_ASSERT( sColumnExist == STL_TRUE );

            sPrimaryKeyDesc->mKeyColumnCnt++;
            
            continue;
        }
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



/**
 * @brief Unique Key Constraint 정보를 설정한다.
 * @param[in]  aTransID    Transaction ID
 * @param[in]  aStmt       Statement
 * @param[in]  aConstDesc  Constraint Descriptor
 * @param[in]  aEnv        Environment
 * @remarks
 */
stlStatus eldcSetUniqueKey( smlTransId           aTransID,
                            smlStatement       * aStmt,
                            ellTableConstDesc  * aConstDesc,
                            ellEnv             * aEnv )
{
    smlRid              sRowRid;
    smlScn              sRowScn;
    smlRowBlock         sRowBlock;
    
    stlBool             sBeginSelect = STL_FALSE;

    void              * sTableHandle = NULL;
    knlValueBlockList * sTableValueList = NULL;

    void                * sIterator  = NULL;
    smlIteratorProperty   sIteratorProperty;

    knlValueBlockList   * sFilterColumn = NULL;

    void              * sIndexHandle = NULL;
    knlValueBlockList * sIndexValueList = NULL;
    knlKeyCompareList * sKeyCompList = NULL;
    
    knlPredicateList    * sRangePred = NULL;
    knlExprContextInfo  * sRangeContext = NULL;
    
    dtlDataValue          * sDataValue = NULL;
    ellUniqueKeyDepDesc   * sUniqueKeyDesc = NULL;
    stlInt32                sColumnIdx = 0;

    stlBool        sColumnExist = STL_FALSE;
    stlInt64       sColumnID = ELL_DICT_OBJECT_ID_NA;

    smlFetchInfo        sFetchInfo;
    smlFetchRecordInfo  sFetchRecordInfo;
    eldMemMark          sMemMark;

    stlInt32  sState = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstDesc != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * DEFINITION_SCHEMA.KEY_COLUMN_USAGE 테이블의 table handle 을 획득
     */
    
    sTableHandle = gEldTablePhysicalHandle[ELDT_TABLE_ID_KEY_COLUMN_USAGE];

    /**
     * DEFINITION_SCHEMA.KEY_COLUMN_USAGE 테이블의 Value List와 Row Block 할당
     */

    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_KEY_COLUMN_USAGE,
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
     * 조건 생성
     * WHERE CONSTRAINT_ID = aConstDesc->mConstID
     */

    sFilterColumn = ellFindColumnValueList( sTableValueList,
                                            ELDT_TABLE_ID_KEY_COLUMN_USAGE,
                                            ELDT_KeyColumn_ColumnOrder_CONSTRAINT_ID );

    if ( gEldCacheBuilding == STL_TRUE )
    {
        /**
         * Start-Up 수행 시간이 View 개수의 제곱에 비례하여 증가하게 된다.
         * Index Cache 가 존재하지 않아 General 한 Key Range 방식이 아닌
         * hard-coded key range 로 start-up 성능 문제를 해결한다.
         */

        STL_TRY( eldcMakeNonCacheRange( aTransID,
                                        aStmt,
                                        sFilterColumn,
                                        aConstDesc->mConstID,
                                        & sKeyCompList,
                                        & sIndexValueList,
                                        & sRangePred,
                                        & sRangeContext,
                                        aEnv )
                 == STL_SUCCESS );
        
        sIndexHandle = gEldNonCacheIndexPhyHandle4KeyColConstID;
    }
    else
    {
        /**
         * Key Range 생성
         */

        STL_TRY( eldMakeOneEquiRange( aTransID,
                                      aStmt,
                                      ELDT_TABLE_ID_KEY_COLUMN_USAGE,
                                      sFilterColumn,
                                      & aConstDesc->mConstID,
                                      STL_SIZEOF(stlInt64),
                                      & sIndexHandle,
                                      & sKeyCompList,
                                      & sIndexValueList,
                                      & sRangePred,
                                      & sRangeContext,
                                      aEnv )
                 == STL_SUCCESS );
    }
    
    /**
     * SELECT 준비
     */

    STL_TRY( eldBeginForIndexAccessQuery( aStmt,
                                          sTableHandle,
                                          sIndexHandle,
                                          sKeyCompList,
                                          & sIteratorProperty,
                                          & sRowBlock,
                                          sTableValueList,
                                          sIndexValueList,
                                          sRangePred,
                                          sRangeContext,
                                          & sFetchInfo,
                                          & sFetchRecordInfo,
                                          & sIterator,
                                          aEnv )                    
             == STL_SUCCESS );
    
    sBeginSelect = STL_TRUE;

    /**
     * Unique Key 의 정보를 설정한다.
     */

    sUniqueKeyDesc = (ellUniqueKeyDepDesc *) aConstDesc->mConstTypeDepDesc;
    sUniqueKeyDesc->mKeyColumnCnt = 0;
    
    while (1)
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
        else
        {
            sDataValue = eldFindDataValue( sTableValueList,
                                           ELDT_TABLE_ID_KEY_COLUMN_USAGE,
                                           ELDT_KeyColumn_ColumnOrder_ORDINAL_POSITION );
            if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
            {
                STL_ASSERT(0);
            }
            else
            {
                stlMemcpy( & sColumnIdx,
                           sDataValue->mValue,
                           sDataValue->mLength );
                STL_DASSERT( STL_SIZEOF(sColumnIdx) == sDataValue->mLength );
                
                sColumnIdx = ELD_ORDINAL_POSITION_TO_PROCESS(sColumnIdx);
            }
            
            sDataValue = eldFindDataValue( sTableValueList,
                                           ELDT_TABLE_ID_KEY_COLUMN_USAGE,
                                           ELDT_KeyColumn_ColumnOrder_COLUMN_ID );
            if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
            {
                STL_ASSERT(0);
            }
            else
            {
                
                stlMemcpy( & sColumnID,
                           sDataValue->mValue,
                           sDataValue->mLength );
                STL_DASSERT( STL_SIZEOF(sColumnID) == sDataValue->mLength );
            }

            STL_TRY( ellGetColumnDictHandleByID(
                         aTransID,
                         SML_MAXIMUM_STABLE_SCN,
                         sColumnID,
                         & sUniqueKeyDesc->mKeyColumnHandle[sColumnIdx],
                         & sColumnExist,
                         aEnv )
                     == STL_SUCCESS );
            STL_ASSERT( sColumnExist == STL_TRUE );

            sUniqueKeyDesc->mKeyColumnCnt++;
            
            continue;
        }
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



/**
 * @brief Foreign Key Constraint 정보를 설정한다.
 * @param[in]  aTransID    Transaction ID
 * @param[in]  aStmt       Statement
 * @param[in]  aConstDesc  Constraint Descriptor
 * @param[in]  aEnv        Environment
 * @remarks
 */
stlStatus eldcSetForeignKey( smlTransId           aTransID,
                             smlStatement       * aStmt,
                             ellTableConstDesc  * aConstDesc,
                             ellEnv             * aEnv )
{
    smlRid              sRowRid;
    smlScn              sRowScn;
    smlRowBlock         sRowBlock;
    stlBool             sFetchOne = STL_FALSE;
    
    stlBool             sBeginSelect = STL_FALSE;

    void              * sTableHandle = NULL;
    knlValueBlockList * sTableValueList = NULL;

    void                * sIterator  = NULL;
    smlIteratorProperty   sIteratorProperty;

    knlValueBlockList   * sFilterColumn = NULL;

    void              * sIndexHandle = NULL;
    knlValueBlockList * sIndexValueList = NULL;
    knlKeyCompareList * sKeyCompList = NULL;
    
    knlPredicateList    * sRangePred = NULL;
    knlExprContextInfo  * sRangeContext = NULL;

    dtlDataValue          * sDataValue = NULL;
    ellForeignKeyDepDesc  * sForeignKeyDesc = NULL;
    
    stlInt32                sColumnIdx = 0;
    stlInt32                sRefKeyIdx = 0;

    stlBool                 sObjectExist = STL_FALSE;
    stlInt64                sColumnID = ELL_DICT_OBJECT_ID_NA;
    stlInt64                sParentKeyID = ELL_DICT_OBJECT_ID_NA;

    smlFetchInfo        sFetchInfo;
    smlFetchRecordInfo  sFetchRecordInfo;
    eldMemMark          sMemMark;

    stlInt32 sState = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstDesc != NULL, ELL_ERROR_STACK(aEnv) );

    /*************************************************************
     * Foreign Key 의 Key 정보를 구축
     *************************************************************/
    
    /**
     * DEFINITION_SCHEMA.KEY_COLUMN_USAGE 테이블의 table handle 을 획득
     */
    
    sTableHandle = gEldTablePhysicalHandle[ELDT_TABLE_ID_KEY_COLUMN_USAGE];

    /**
     * DEFINITION_SCHEMA.KEY_COLUMN_USAGE 테이블의 Value List와 Row Block 할당
     */

    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_KEY_COLUMN_USAGE,
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
     * 조건 생성
     * WHERE CONSTRAINT_ID = aConstDesc->mConstID
     */

    sFilterColumn = ellFindColumnValueList( sTableValueList,
                                            ELDT_TABLE_ID_KEY_COLUMN_USAGE,
                                            ELDT_KeyColumn_ColumnOrder_CONSTRAINT_ID );

    if ( gEldCacheBuilding == STL_TRUE )
    {
        /**
         * Start-Up 수행 시간이 View 개수의 제곱에 비례하여 증가하게 된다.
         * Index Cache 가 존재하지 않아 General 한 Key Range 방식이 아닌
         * hard-coded key range 로 start-up 성능 문제를 해결한다.
         */

        STL_TRY( eldcMakeNonCacheRange( aTransID,
                                        aStmt,
                                        sFilterColumn,
                                        aConstDesc->mConstID,
                                        & sKeyCompList,
                                        & sIndexValueList,
                                        & sRangePred,
                                        & sRangeContext,
                                        aEnv )
                 == STL_SUCCESS );
        
        sIndexHandle = gEldNonCacheIndexPhyHandle4KeyColConstID;
    }
    else
    {
        /**
         * Key Range 생성
         */

        STL_TRY( eldMakeOneEquiRange( aTransID,
                                      aStmt,
                                      ELDT_TABLE_ID_KEY_COLUMN_USAGE,
                                      sFilterColumn,
                                      & aConstDesc->mConstID,
                                      STL_SIZEOF(stlInt64),
                                      & sIndexHandle,
                                      & sKeyCompList,
                                      & sIndexValueList,
                                      & sRangePred,
                                      & sRangeContext,
                                      aEnv )
                 == STL_SUCCESS );
    }

        
    /**
     * SELECT 준비
     */

    STL_TRY( eldBeginForIndexAccessQuery( aStmt,
                                          sTableHandle,
                                          sIndexHandle,
                                          sKeyCompList,
                                          & sIteratorProperty,
                                          & sRowBlock,
                                          sTableValueList,
                                          sIndexValueList,
                                          sRangePred,
                                          sRangeContext,
                                          & sFetchInfo,
                                          & sFetchRecordInfo,
                                          & sIterator,
                                          aEnv )                    
             == STL_SUCCESS );
    
    sBeginSelect = STL_TRUE;

    /**
     * Foreign Key 의 Key 정보를 설정한다.
     */

    sForeignKeyDesc = (ellForeignKeyDepDesc *) aConstDesc->mConstTypeDepDesc;
    sForeignKeyDesc->mKeyColumnCnt = 0;
    
    while (1)
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
        else
        {
            /*
             * Key Idx
             */

            sDataValue = eldFindDataValue( sTableValueList,
                                           ELDT_TABLE_ID_KEY_COLUMN_USAGE,
                                           ELDT_KeyColumn_ColumnOrder_ORDINAL_POSITION );
            if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
            {
                STL_ASSERT(0);
            }
            else
            {
                stlMemcpy( & sColumnIdx,
                           sDataValue->mValue,
                           sDataValue->mLength );
                STL_DASSERT( STL_SIZEOF(sColumnIdx) == sDataValue->mLength );
                
                sColumnIdx = ELD_ORDINAL_POSITION_TO_PROCESS(sColumnIdx);
            }

            /**
             * mKeyColumnHandle
             */
            
            sDataValue = eldFindDataValue( sTableValueList,
                                           ELDT_TABLE_ID_KEY_COLUMN_USAGE,
                                           ELDT_KeyColumn_ColumnOrder_COLUMN_ID );
            if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
            {
                STL_ASSERT(0);
            }
            else
            {
                
                stlMemcpy( & sColumnID,
                           sDataValue->mValue,
                           sDataValue->mLength );
                STL_DASSERT( STL_SIZEOF(sColumnID) == sDataValue->mLength );
            }

            STL_TRY( ellGetColumnDictHandleByID(
                         aTransID,
                         SML_MAXIMUM_STABLE_SCN,
                         sColumnID,
                         & sForeignKeyDesc->mKeyColumnHandle[sColumnIdx],
                         & sObjectExist,
                         aEnv )
                     == STL_SUCCESS );
            STL_ASSERT( sObjectExist == STL_TRUE );


            /**
             * mKeyColumnIdxInParent
             */
            
            sDataValue = eldFindDataValue( sTableValueList,
                                           ELDT_TABLE_ID_KEY_COLUMN_USAGE,
                                           ELDT_KeyColumn_ColumnOrder_POSITION_IN_UNIQUE_CONSTRAINT );
            if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
            {
                STL_ASSERT(0);
            }
            else
            {
                stlMemcpy( & sRefKeyIdx,
                           sDataValue->mValue,
                           sDataValue->mLength );
                STL_DASSERT( STL_SIZEOF(sRefKeyIdx) == sDataValue->mLength );
                
                sRefKeyIdx = ELD_ORDINAL_POSITION_TO_PROCESS(sRefKeyIdx);
                
                sForeignKeyDesc->mKeyMatchIdxInParent[sColumnIdx] = sRefKeyIdx;
            }

            sForeignKeyDesc->mKeyColumnCnt++;
            
            continue;
        }
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
    
    /*************************************************************
     * Foreign Key 의 Option 정보를 구축 
     *************************************************************/

    /**
     * DEFINITION_SCHEMA.REFERENTIAL_CONSTRAINTS 테이블의 table handle 을 획득
     */
    
    sTableHandle = gEldTablePhysicalHandle[ELDT_TABLE_ID_REFERENTIAL_CONSTRAINTS];

    /**
     * DEFINITION_SCHEMA.REFERENTIAL_CONSTRAINTS 테이블의 Value List와 Row Block 할당
     */

    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_REFERENTIAL_CONSTRAINTS,
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
     * 조건 생성
     * WHERE CONSTRAINT_ID = aConstDesc->mConstID
     */

    sFilterColumn = ellFindColumnValueList( sTableValueList,
                                            ELDT_TABLE_ID_REFERENTIAL_CONSTRAINTS,
                                            ELDT_Reference_ColumnOrder_CONSTRAINT_ID );

    if ( gEldCacheBuilding == STL_TRUE )
    {
        /**
         * Start-Up 수행 시간이 View 개수의 제곱에 비례하여 증가하게 된다.
         * Index Cache 가 존재하지 않아 General 한 Key Range 방식이 아닌
         * hard-coded key range 로 start-up 성능 문제를 해결한다.
         */

        STL_TRY( eldcMakeNonCacheRange( aTransID,
                                        aStmt,
                                        sFilterColumn,
                                        aConstDesc->mConstID,
                                        & sKeyCompList,
                                        & sIndexValueList,
                                        & sRangePred,
                                        & sRangeContext,
                                        aEnv )
                 == STL_SUCCESS );
        
        sIndexHandle = gEldNonCacheIndexPhyHandle4RefConstID;
    }
    else
    {
        /**
         * Key Range 생성
         */

        STL_TRY( eldMakeOneEquiRange( aTransID,
                                      aStmt,
                                      ELDT_TABLE_ID_REFERENTIAL_CONSTRAINTS,
                                      sFilterColumn,
                                      & aConstDesc->mConstID,
                                      STL_SIZEOF(stlInt64),
                                      & sIndexHandle,
                                      & sKeyCompList,
                                      & sIndexValueList,
                                      & sRangePred,
                                      & sRangeContext,
                                      aEnv )
                 == STL_SUCCESS );
    }

    /**
     * SELECT 준비
     */

    STL_TRY( eldBeginForIndexAccessQuery( aStmt,
                                          sTableHandle,
                                          sIndexHandle,
                                          sKeyCompList,
                                          & sIteratorProperty,
                                          & sRowBlock,
                                          sTableValueList,
                                          sIndexValueList,
                                          sRangePred,
                                          sRangeContext,
                                          & sFetchInfo,
                                          & sFetchRecordInfo,
                                          & sIterator,
                                          aEnv )                    
             == STL_SUCCESS );
                                      
    sBeginSelect = STL_TRUE;

    /**
     * Foreign Key 의 Option 정보를 설정한다.
     */

    sFetchOne = STL_FALSE;
    
    while (1)
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
        else
        {
            /**
             * mMatchOption
             */

            sDataValue = eldFindDataValue( sTableValueList,
                                           ELDT_TABLE_ID_REFERENTIAL_CONSTRAINTS,
                                           ELDT_Reference_ColumnOrder_MATCH_OPTION_ID );
            if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
            {
                STL_ASSERT(0);
            }
            else
            {
                
                stlMemcpy( & sForeignKeyDesc->mMatchOption,
                           sDataValue->mValue,
                           sDataValue->mLength );
                STL_DASSERT( STL_SIZEOF(sForeignKeyDesc->mMatchOption)
                             == sDataValue->mLength );
            }

            /**
             * mUpdateRule
             */
            
            sDataValue = eldFindDataValue( sTableValueList,
                                           ELDT_TABLE_ID_REFERENTIAL_CONSTRAINTS,
                                           ELDT_Reference_ColumnOrder_UPDATE_RULE_ID );
            if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
            {
                STL_ASSERT(0);
            }
            else
            {
                
                stlMemcpy( & sForeignKeyDesc->mUpdateRule,
                           sDataValue->mValue,
                           sDataValue->mLength );
                STL_DASSERT( STL_SIZEOF(sForeignKeyDesc->mUpdateRule)
                             == sDataValue->mLength );
            }
            
            /**
             * mDeleteRule
             */
            
            sDataValue = eldFindDataValue( sTableValueList,
                                           ELDT_TABLE_ID_REFERENTIAL_CONSTRAINTS,
                                           ELDT_Reference_ColumnOrder_DELETE_RULE_ID );
            if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
            {
                STL_ASSERT(0);
            }
            else
            {
                
                stlMemcpy( & sForeignKeyDesc->mDeleteRule,
                           sDataValue->mValue,
                           sDataValue->mLength );
                STL_DASSERT( STL_SIZEOF(sForeignKeyDesc->mDeleteRule)
                             == sDataValue->mLength );
            }

            /**
             * mParentKeyHandle
             */

            sDataValue = eldFindDataValue( sTableValueList,
                                           ELDT_TABLE_ID_REFERENTIAL_CONSTRAINTS,
                                           ELDT_Reference_ColumnOrder_UNIQUE_CONSTRAINT_ID );
            if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
            {
                STL_ASSERT(0);
            }
            else
            {
                stlMemcpy( & sParentKeyID,
                           sDataValue->mValue,
                           sDataValue->mLength );
                STL_DASSERT( STL_SIZEOF(sParentKeyID) == sDataValue->mLength );

                STL_TRY( eldcGetConstHandleByID( aTransID,
                                                 SML_MAXIMUM_STABLE_SCN,
                                                 sParentKeyID,
                                                 & sForeignKeyDesc->mParentUniqueKeyHandle,
                                                 & sObjectExist,
                                                 aEnv )
                         == STL_SUCCESS );
                STL_ASSERT( sObjectExist == STL_TRUE );
            }

            /* 한 건만 존재해야 한다. */
            STL_ASSERT( sFetchOne == STL_FALSE );
            sFetchOne = STL_TRUE;
            
            continue;
        }
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




/** @} eldcConstraintDesc */
