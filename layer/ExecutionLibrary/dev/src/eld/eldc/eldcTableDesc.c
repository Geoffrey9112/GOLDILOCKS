/*******************************************************************************
 * eldcTableDesc.c
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
 * @file eldcTableDesc.c
 * @brief Cache for Table(View) descriptor
 */

#include <ell.h>
#include <eldt.h>                               
#include <eldc.h>

#include <eldDictionary.h>
#include <elgPendOp.h>



/**
 * @addtogroup eldcTableDesc
 * @{
 */


/**
 * @brief Hash Key 로부터 Hash Value 생성을 위한 함수 포인터
 * @param[in] aHashKey     Hash Key(Table ID)
 * @remarks
 */
stlUInt32  eldcHashFuncTableID( void * aHashKey )
{
    stlInt64  sTableID = 0;
    
    stlUInt32 sHashValue = 0;

    /**
     * Table ID 로부터 Hash Value 생성 
     */
    
    sTableID = *(stlInt64 *) aHashKey;
    
    sHashValue = ellGetHashValueOneID( sTableID );

    return sHashValue;
}

/**
 * @brief Hash Key 와 Hash Element의 비교를 위한 함수 포인터
 * @param[in] aHashKey        Hash Key(Table ID)
 * @param[in] aHashElement    Hash Element
 * @remarks
 * Hash Value 가 동일할 경우 Key 를 검사 
 */
stlBool    eldcCompareFuncTableID( void * aHashKey, eldcHashElement * aHashElement )
{
    stlInt64              sTableID = 0;
    
    eldcHashDataTableID * sHashData = NULL;

    /**
     * 입력된 Table ID 와 Hash Element 의 Table ID 를 비교 
     */
    
    sTableID  = *(stlInt64 *) aHashKey;
    
    sHashData    = (eldcHashDataTableID *) aHashElement->mHashData;

    if ( sHashData->mKeyTableID == sTableID )
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
 * @param[in] aHashKey     Hash Key (Schema ID, Table Name)
 * @remarks
 */
stlUInt32  eldcHashFuncTableName( void * aHashKey )
{
    eldcHashKeyTableName * sKeyData = NULL;
    
    stlUInt32 sHashValue = 0;

    /**
     * Hash Key 로부터 Hash Value 생성 
     */
    
    sKeyData = (eldcHashKeyTableName *) aHashKey;
    
    sHashValue = ellGetHashValueNameAndID( sKeyData->mSchemaID, sKeyData->mTableName );

    return sHashValue;
}

/**
 * @brief Hash Key 와 Hash Element의 비교를 위한 함수 포인터
 * @param[in] aHashKey        Hash Key (Schema ID, Table Name )
 * @param[in] aHashElement    Hash Element
 * @remarks
 * Hash Value 가 동일할 경우 Key 를 검사 
 */
stlBool    eldcCompareFuncTableName( void * aHashKey, eldcHashElement * aHashElement )
{
    eldcHashKeyTableName  * sHashKey = NULL;
    
    eldcHashDataTableName * sHashData    = NULL;

    /**
     * 입력된 Hash Key 와 Hash Element 의 Key 를 비교 
     */
    
    sHashKey  = (eldcHashKeyTableName *) aHashKey;
    
    sHashData    = (eldcHashDataTableName *) aHashElement->mHashData;

    if ( ( sHashData->mKeySchemaID == sHashKey->mSchemaID ) &&
         ( stlStrcmp( sHashData->mKeyTableName, sHashKey->mTableName ) == 0 ) )
    {
        return STL_TRUE;
    }
    else
    {
        return STL_FALSE;
    }
}






/**
 * @brief Table Descriptor 를 위한 공간을 계산한다.
 * @param[in] aTableNameLen    Table Name 의 길이
 * @param[in] aColumnCnt       Column 의 개수
 * @param[in] aViewColumnLen   View 정의 구문의 길이
 * @param[in] aViewQueryLen    View Query 구문의 길이
 * @remarks
 */
stlInt32  eldcCalSizeTableDesc( stlInt32     aTableNameLen,
                                stlInt32     aColumnCnt,
                                stlInt32     aViewColumnLen,
                                stlInt32     aViewQueryLen )
{
    stlInt32   sSize = 0;

    sSize = STL_ALIGN_DEFAULT( STL_SIZEOF(ellTableDesc) );
    
    if ( aTableNameLen > 0 )
    {
        sSize = sSize + STL_ALIGN_DEFAULT(aTableNameLen + 1);
    }

    if ( aColumnCnt > 0 )
    {
        sSize = sSize + STL_ALIGN_DEFAULT( STL_SIZEOF(ellDictHandle) * aColumnCnt );
    }

    if ( aViewColumnLen > 0 )
    {
        sSize = sSize + STL_ALIGN_DEFAULT(aViewColumnLen + 1);
    }

    if ( aViewQueryLen > 0 )
    {
        sSize = sSize + STL_ALIGN_DEFAULT(aViewQueryLen + 1);
    }
    
    return sSize;
}


/**
 * @brief Table Descriptor 로부터 Column Name 의 Pointer 위치를 계산
 * @param[in] aTableDesc   Table Descriptor
 * @remarks
 */
stlChar * eldcGetTableNamePtr( ellTableDesc * aTableDesc )
{
    return (stlChar *) aTableDesc + STL_ALIGN_DEFAULT( STL_SIZEOF(ellTableDesc) );
}

/**
 * @brief Table Desciptor 로부터 Column Hash Element 의 Pointer 위치를 계산
 * @param[in] aTableDesc    Table Descriptor
 * @param[in] aTableNameLen Table Name 의 길이 
 * @remarks
 */
ellDictHandle * eldcGetTableColumnDictHandlePtr( ellTableDesc * aTableDesc,
                                                 stlInt32       aTableNameLen )
{
    return (ellDictHandle *) ( (stlChar *) aTableDesc
                               + STL_ALIGN_DEFAULT( STL_SIZEOF(ellTableDesc) )
                               + STL_ALIGN_DEFAULT( aTableNameLen + 1 ) );
}


/**
 * @brief Table Desciptor 로부터 View Column List 의 Pointer 위치를 계산
 * @param[in] aTableDesc    Table Descriptor
 * @param[in] aTableNameLen Table Name 의 길이
 * @remarks
 */
stlChar * eldcGetViewColDefPtr( ellTableDesc * aTableDesc,
                                stlInt32       aTableNameLen )
{
    return (stlChar *) aTableDesc
        + STL_ALIGN_DEFAULT( STL_SIZEOF(ellTableDesc) )
        + STL_ALIGN_DEFAULT( aTableNameLen + 1 );
}

/**
 * @brief Table Desciptor 로부터 View Phrase 의 Pointer 위치를 계산
 * @param[in] aTableDesc      Table Descriptor
 * @param[in] aTableNameLen   Table Name 의 길이
 * @param[in] aViewColDefLen  View Column List 의 길이
 * @remarks
 */
stlChar * eldcGetViewSelectPtr( ellTableDesc * aTableDesc,
                                stlInt32       aTableNameLen,
                                stlInt32       aViewColDefLen )
{
    return (stlChar *) aTableDesc
        + STL_ALIGN_DEFAULT( STL_SIZEOF(ellTableDesc) )
        + STL_ALIGN_DEFAULT( aTableNameLen + 1 )
        + STL_ALIGN_DEFAULT( aViewColDefLen + 1 );
}

/**
 * @brief Table ID  Hash Data 를 위한 공간을 계산한다.
 * @remarks
 */
stlInt32  eldcCalSizeHashDataTableID()
{
    stlInt32   sSize = 0;

    sSize = STL_ALIGN_DEFAULT( STL_SIZEOF( eldcHashDataTableID ) );

    return sSize;
}

/**
 * @brief Table Name Hash Data 를 위한 공간을 계산한다.
 * @param[in]    aTableNameLen    Table Name 의 길이 
 * @remarks
 */
stlInt32  eldcCalSizeHashDataTableName( stlInt32 aTableNameLen )
{
    stlInt32   sSize = 0;

    sSize = STL_ALIGN_DEFAULT( STL_SIZEOF(eldcHashDataTableName) );

    if ( aTableNameLen > 0 )
    {
        sSize = sSize + STL_ALIGN_DEFAULT(aTableNameLen + 1);
    }
    
    return sSize;
}


/**
 * @brief Table Name Hash Data 로부터 Table Name 의 Pointer 위치를 계산
 * @param[in] aHashDataTableName   Table Name 의 Hash Data
 * @remarks
 */
stlChar * eldcGetHashDataTableNamePtr( eldcHashDataTableName * aHashDataTableName )
{
    return (stlChar *) aHashDataTableName
        + STL_ALIGN_DEFAULT( STL_SIZEOF(eldcHashDataTableName) );
}


/**
 * @brief Table Descriptor 의 String 출력을 생성하는 함수 포인터 
 * @param[in]  aTableDesc    Table Descriptor
 * @param[out] aStringBuffer 출력 정보를 위한 String Buffer 공간
 * @remarks
 */
void eldcPrintTableDesc( void * aTableDesc, stlChar * aStringBuffer )
{
    stlInt32       i = 0;
    stlChar      * sString = NULL;

    stlInt32          sSize = 0;
    ellTableDesc    * sTableDesc = (ellTableDesc *)aTableDesc;
    ellDictHandle   * sDictHandle = NULL;
    
    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "Table Descriptor( " );

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sOWNER_ID: %ld, ",
                 aStringBuffer,
                 sTableDesc->mOwnerID );

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sSCHEMA_ID: %ld, ",
                 aStringBuffer,
                 sTableDesc->mSchemaID );
    
    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sTABLE_ID: %ld, ",
                 aStringBuffer,
                 sTableDesc->mTableID );

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sTABLESPACE_ID: %ld, ",
                 aStringBuffer,
                 sTableDesc->mTablespaceID );

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sPHYSICAL_ID: %ld, ",
                 aStringBuffer,
                 sTableDesc->mPhysicalID );
    
    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sTABLE_NAME: %s, ",
                 aStringBuffer,
                 sTableDesc->mTableName );

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sTABLE_TYPE: %s, ",
                 aStringBuffer,
                 gEllTableTypeString[sTableDesc->mTableType] );
    
    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sCOLUMN_CNT: %ld, ",
                 aStringBuffer,
                 sTableDesc->mColumnCnt );

    /**
     * Column 개수만큼 컬럼 정보를 출력해야 함
     */

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sCOLUMN_ID[",
                 aStringBuffer );
    
    for ( i = 0; i < sTableDesc->mColumnCnt; i++ )
    {
        sDictHandle = & sTableDesc->mColumnDictHandle[i];
        stlSnprintf( aStringBuffer,
                     ELDC_DUMP_PRINT_BUFFER_SIZE,
                     "%s %ld",
                     aStringBuffer,
                     ellGetColumnID( sDictHandle ) );
    }

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%s ], ",
                 aStringBuffer );
    
    switch ( sTableDesc->mTableType )
    {
        case ELL_TABLE_TYPE_BASE_TABLE:
            {
                sSize = stlSnprintf( aStringBuffer,
                                     ELDC_DUMP_PRINT_BUFFER_SIZE,
                                     "%sBASE_TABLE[] )",
                                     aStringBuffer );

                break;
            }
        case ELL_TABLE_TYPE_VIEW:
            {
                sSize = stlSnprintf( aStringBuffer,
                                     ELDC_DUMP_PRINT_BUFFER_SIZE,
                                     "%sVIEWED_TABLE[%s, %s] )",
                                     aStringBuffer,
                                     sTableDesc->mTableTypeDesc.mView.mViewColumnString,
                                     sTableDesc->mTableTypeDesc.mView.mViewQueryString );
                break;
            }

        case ELL_TABLE_TYPE_GLOBAL_TEMPORARY:
        case ELL_TABLE_TYPE_LOCAL_TEMPORARY:
        case ELL_TABLE_TYPE_SYSTEM_VERSIONED:
            {
                sSize = stlSnprintf( aStringBuffer,
                                     ELDC_DUMP_PRINT_BUFFER_SIZE,
                                     "%sINVALID_TABLE[] )",
                                     aStringBuffer );
                break;
            }
        case ELL_TABLE_TYPE_SEQUENCE:
        case ELL_TABLE_TYPE_SYNONYM:
            {
                /**
                 * Table Descriptor 에 존재하면 안됨
                 */
                STL_DASSERT(0);
                break;
            }
        case ELL_TABLE_TYPE_FIXED_TABLE:
            {
                sString = (stlChar *)
                    gPhaseString[sTableDesc->mTableTypeDesc.mFixedTable.mStartupPhase];
                
                sSize = stlSnprintf( aStringBuffer,
                                     ELDC_DUMP_PRINT_BUFFER_SIZE,
                                     "%sFIXED_TABLE[%s] )",
                                     aStringBuffer,
                                     sString );
                break;
            }
        case ELL_TABLE_TYPE_DUMP_TABLE:
            {
                sString = (stlChar *)
                    gPhaseString[sTableDesc->mTableTypeDesc.mFixedTable.mStartupPhase];
                
                sSize = stlSnprintf( aStringBuffer,
                                     ELDC_DUMP_PRINT_BUFFER_SIZE,
                                     "%sDUMP_TABLE[%s] )",
                                     aStringBuffer,
                                     sString );
                break;
            }
        default:
            {
                sSize = stlSnprintf( aStringBuffer,
                                     ELDC_DUMP_PRINT_BUFFER_SIZE,
                                     "%sINVALID_TABLE[] )",
                                     aStringBuffer );
                break;
            }
    }
    
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
 * @brief Table ID Cache 의 Hash Data 의 String 출력을 생성하는 함수 포인터 
 * @param[in]  aHashDataTableID    Table ID 의 Hash Data
 * @param[out] aStringBuffer       출력 정보를 위한 String Buffer 공간 
 * @remarks
 */
void eldcPrintHashDataTableID( void * aHashDataTableID, stlChar * aStringBuffer )
{
    stlInt32                 sSize = 0;
    eldcHashDataTableID    * sHashData = (eldcHashDataTableID *) aHashDataTableID;
    eldcHashRelatedObjList * sRelList = NULL;

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "Table ID Hash Data( " );

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sKEY_TABLE_ID: %ld",
                 aStringBuffer,
                 sHashData->mKeyTableID );

    /**
     * CHECK NOT NULL 의 Constraint ID 정보 출력
     */

    if ( sHashData->mCheckNotNull == NULL )
    {
        /* nothing to do */
    }
    else
    {
        stlSnprintf( aStringBuffer,
                     ELDC_DUMP_PRINT_BUFFER_SIZE,
                     "%s, CHECK_NOT_NULL[",
                     aStringBuffer );
    
        for ( sRelList = sHashData->mCheckNotNull;
              sRelList != NULL;
              sRelList = sRelList->mNext )
        {
            stlSnprintf( aStringBuffer,
                         ELDC_DUMP_PRINT_BUFFER_SIZE,
                         "%s %s",
                         aStringBuffer,
                         ellGetConstraintName( & sRelList->mRelatedHandle ) );
            
            stlSnprintf( aStringBuffer,
                         ELDC_DUMP_PRINT_BUFFER_SIZE,
                         "%s(CrtID:%ld,CrtSCN:%ld,DrpID:%ld,DrpSCN:%ld,DrpFlag:%d,ModCnt:%ld,GModCnt:%ld,MemUse:%ld)",
                         aStringBuffer,
                         sRelList->mVersionInfo.mCreateTransID,
                         sRelList->mVersionInfo.mCreateCommitSCN,
                         sRelList->mVersionInfo.mDropTransID,
                         sRelList->mVersionInfo.mDropCommitSCN,
                         sRelList->mVersionInfo.mDropFlag,
                         sRelList->mVersionInfo.mModifyingCnt,
                         sRelList->mVersionInfo.mTryModifyCnt,
                         sRelList->mVersionInfo.mMemUseCnt );
        }
        
        stlSnprintf( aStringBuffer,
                     ELDC_DUMP_PRINT_BUFFER_SIZE,
                     "%s]",
                     aStringBuffer );
    }
    
    /**
     * CHECK CLAUSE 정보를 출력
     */

    if ( sHashData->mCheckClause == NULL )
    {
        /* nothing to do */
    }
    else
    {
        stlSnprintf( aStringBuffer,
                     ELDC_DUMP_PRINT_BUFFER_SIZE,
                     "%s, CHECK_CLAUSE[",
                     aStringBuffer );
        
        for ( sRelList = sHashData->mCheckClause;
              sRelList != NULL;
              sRelList = sRelList->mNext )
        {
            stlSnprintf( aStringBuffer,
                         ELDC_DUMP_PRINT_BUFFER_SIZE,
                         "%s %s",
                         aStringBuffer,
                         ellGetConstraintName( & sRelList->mRelatedHandle ) );

            stlSnprintf( aStringBuffer,
                         ELDC_DUMP_PRINT_BUFFER_SIZE,
                         "%s(CrtID:%ld,CrtSCN:%ld,DrpID:%ld,DrpSCN:%ld,DrpFlag:%d,ModCnt:%ld,GModCnt:%ld,MemUse:%ld)",
                         aStringBuffer,
                         sRelList->mVersionInfo.mCreateTransID,
                         sRelList->mVersionInfo.mCreateCommitSCN,
                         sRelList->mVersionInfo.mDropTransID,
                         sRelList->mVersionInfo.mDropCommitSCN,
                         sRelList->mVersionInfo.mDropFlag,
                         sRelList->mVersionInfo.mModifyingCnt,
                         sRelList->mVersionInfo.mTryModifyCnt,
                         sRelList->mVersionInfo.mMemUseCnt );
        }

        stlSnprintf( aStringBuffer,
                     ELDC_DUMP_PRINT_BUFFER_SIZE,
                     "%s]",
                     aStringBuffer );
    }

    /**
     * PRIMARY KEY 정보를 출력
     */
    
    if ( sHashData->mPrimaryKey == NULL )
    {
        /* nothing to do */
    }
    else
    {
        stlSnprintf( aStringBuffer,
                     ELDC_DUMP_PRINT_BUFFER_SIZE,
                     "%s, PRIMARY_KEY[",
                     aStringBuffer );
    
        for ( sRelList = sHashData->mPrimaryKey;
              sRelList != NULL;
              sRelList = sRelList->mNext )
        {
            stlSnprintf( aStringBuffer,
                         ELDC_DUMP_PRINT_BUFFER_SIZE,
                         "%s %s",
                         aStringBuffer,
                         ellGetConstraintName( & sRelList->mRelatedHandle ) );

            stlSnprintf( aStringBuffer,
                         ELDC_DUMP_PRINT_BUFFER_SIZE,
                         "%s(CrtID:%ld,CrtSCN:%ld,DrpID:%ld,DrpSCN:%ld,DrpFlag:%d,ModCnt:%ld,GModCnt:%ld,MemUse:%ld)",
                         aStringBuffer,
                         sRelList->mVersionInfo.mCreateTransID,
                         sRelList->mVersionInfo.mCreateCommitSCN,
                         sRelList->mVersionInfo.mDropTransID,
                         sRelList->mVersionInfo.mDropCommitSCN,
                         sRelList->mVersionInfo.mDropFlag,
                         sRelList->mVersionInfo.mModifyingCnt,
                         sRelList->mVersionInfo.mTryModifyCnt,
                         sRelList->mVersionInfo.mMemUseCnt );
        }

        stlSnprintf( aStringBuffer,
                     ELDC_DUMP_PRINT_BUFFER_SIZE,
                     "%s]",
                     aStringBuffer );
    }

    
    /**
     * UNIQUE KEY 정보를 출력
     */
    
    if ( sHashData->mUniqueKey == NULL )
    {
        /* nothing to do */
    }
    else
    {
        stlSnprintf( aStringBuffer,
                     ELDC_DUMP_PRINT_BUFFER_SIZE,
                     "%s, UNIQUE_KEY[",
                     aStringBuffer );
    
        for ( sRelList = sHashData->mUniqueKey;
              sRelList != NULL;
              sRelList = sRelList->mNext )
        {
            stlSnprintf( aStringBuffer,
                         ELDC_DUMP_PRINT_BUFFER_SIZE,
                         "%s %s",
                         aStringBuffer,
                         ellGetConstraintName( & sRelList->mRelatedHandle ) );

            stlSnprintf( aStringBuffer,
                         ELDC_DUMP_PRINT_BUFFER_SIZE,
                         "%s(CrtID:%ld,CrtSCN:%ld,DrpID:%ld,DrpSCN:%ld,DrpFlag:%d,ModCnt:%ld,GModCnt:%ld,MemUse:%ld)",
                         aStringBuffer,
                         sRelList->mVersionInfo.mCreateTransID,
                         sRelList->mVersionInfo.mCreateCommitSCN,
                         sRelList->mVersionInfo.mDropTransID,
                         sRelList->mVersionInfo.mDropCommitSCN,
                         sRelList->mVersionInfo.mDropFlag,
                         sRelList->mVersionInfo.mModifyingCnt,
                         sRelList->mVersionInfo.mTryModifyCnt,
                         sRelList->mVersionInfo.mMemUseCnt );
        }

        stlSnprintf( aStringBuffer,
                     ELDC_DUMP_PRINT_BUFFER_SIZE,
                     "%s]",
                     aStringBuffer );
    }

    /**
     * FOREIGN KEY 정보를 출력
     */
    
    if ( sHashData->mForeignKey == NULL )
    {
        /* nothing to do */
    }
    else
    {
        stlSnprintf( aStringBuffer,
                     ELDC_DUMP_PRINT_BUFFER_SIZE,
                     "%s, FOREIGN_KEY[",
                     aStringBuffer );
    
        for ( sRelList = sHashData->mForeignKey;
              sRelList != NULL;
              sRelList = sRelList->mNext )
        {
            stlSnprintf( aStringBuffer,
                         ELDC_DUMP_PRINT_BUFFER_SIZE,
                         "%s %s",
                         aStringBuffer,
                         ellGetConstraintName( & sRelList->mRelatedHandle ) );

            stlSnprintf( aStringBuffer,
                         ELDC_DUMP_PRINT_BUFFER_SIZE,
                         "%s(CrtID:%ld,CrtSCN:%ld,DrpID:%ld,DrpSCN:%ld,DrpFlag:%d,ModCnt:%ld,GModCnt:%ld,MemUse:%ld)",
                         aStringBuffer,
                         sRelList->mVersionInfo.mCreateTransID,
                         sRelList->mVersionInfo.mCreateCommitSCN,
                         sRelList->mVersionInfo.mDropTransID,
                         sRelList->mVersionInfo.mDropCommitSCN,
                         sRelList->mVersionInfo.mDropFlag,
                         sRelList->mVersionInfo.mModifyingCnt,
                         sRelList->mVersionInfo.mTryModifyCnt,
                         sRelList->mVersionInfo.mMemUseCnt );
        }

        stlSnprintf( aStringBuffer,
                     ELDC_DUMP_PRINT_BUFFER_SIZE,
                     "%s]",
                     aStringBuffer );
    }

    /**
     * UNIQUE INDEX 정보를 출력
     */
    
    if ( sHashData->mUniqueIndex == NULL )
    {
        /* nothing to do */
    }
    else
    {
        stlSnprintf( aStringBuffer,
                     ELDC_DUMP_PRINT_BUFFER_SIZE,
                     "%s, UNIQUE_INDEX[",
                     aStringBuffer );
    
        for ( sRelList = sHashData->mUniqueIndex;
              sRelList != NULL;
              sRelList = sRelList->mNext )
        {
            stlSnprintf( aStringBuffer,
                         ELDC_DUMP_PRINT_BUFFER_SIZE,
                         "%s %s",
                         aStringBuffer,
                         ellGetIndexName( & sRelList->mRelatedHandle ) );

            stlSnprintf( aStringBuffer,
                         ELDC_DUMP_PRINT_BUFFER_SIZE,
                         "%s(CrtID:%ld,CrtSCN:%ld,DrpID:%ld,DrpSCN:%ld,DrpFlag:%d,ModCnt:%ld,GModCnt:%ld,MemUse:%ld)",
                         aStringBuffer,
                         sRelList->mVersionInfo.mCreateTransID,
                         sRelList->mVersionInfo.mCreateCommitSCN,
                         sRelList->mVersionInfo.mDropTransID,
                         sRelList->mVersionInfo.mDropCommitSCN,
                         sRelList->mVersionInfo.mDropFlag,
                         sRelList->mVersionInfo.mModifyingCnt,
                         sRelList->mVersionInfo.mTryModifyCnt,
                         sRelList->mVersionInfo.mMemUseCnt );
        }

        stlSnprintf( aStringBuffer,
                     ELDC_DUMP_PRINT_BUFFER_SIZE,
                     "%s]",
                     aStringBuffer );
    }
    
    /**
     * NON-UNIQUE INDEX 정보를 출력
     */

    
    if ( sHashData->mNonUniqueIndex == NULL )
    {
        /* nothing to do */
    }
    else
    {
        stlSnprintf( aStringBuffer,
                     ELDC_DUMP_PRINT_BUFFER_SIZE,
                     "%s, NON_UNIQUE_INDEX[",
                     aStringBuffer );
    
        for ( sRelList = sHashData->mNonUniqueIndex;
              sRelList != NULL;
              sRelList = sRelList->mNext )
        {
            stlSnprintf( aStringBuffer,
                         ELDC_DUMP_PRINT_BUFFER_SIZE,
                         "%s %s",
                         aStringBuffer,
                         ellGetIndexName( & sRelList->mRelatedHandle ) );

            stlSnprintf( aStringBuffer,
                         ELDC_DUMP_PRINT_BUFFER_SIZE,
                         "%s(CrtID:%ld,CrtSCN:%ld,DrpID:%ld,DrpSCN:%ld,DrpFlag:%d,ModCnt:%ld,GModCnt:%ld,MemUse:%ld)",
                         aStringBuffer,
                         sRelList->mVersionInfo.mCreateTransID,
                         sRelList->mVersionInfo.mCreateCommitSCN,
                         sRelList->mVersionInfo.mDropTransID,
                         sRelList->mVersionInfo.mDropCommitSCN,
                         sRelList->mVersionInfo.mDropFlag,
                         sRelList->mVersionInfo.mModifyingCnt,
                         sRelList->mVersionInfo.mTryModifyCnt,
                         sRelList->mVersionInfo.mMemUseCnt );
        }

        stlSnprintf( aStringBuffer,
                     ELDC_DUMP_PRINT_BUFFER_SIZE,
                     "%s]",
                     aStringBuffer );
    }
    

    /**
     * 나를 참조하는 Child Foreign Key Constraint 정보를 출력
     */

    if ( sHashData->mChildForeignKey == NULL )
    {
        /* nothing to do */
    }
    else
    {
    
        stlSnprintf( aStringBuffer,
                     ELDC_DUMP_PRINT_BUFFER_SIZE,
                     "%s, CHILD_FOREIGN_KEY[",
                     aStringBuffer );
    
        for ( sRelList = sHashData->mChildForeignKey;
              sRelList != NULL;
              sRelList = sRelList->mNext )
        {
            stlSnprintf( aStringBuffer,
                         ELDC_DUMP_PRINT_BUFFER_SIZE,
                         "%s %s",
                         aStringBuffer,
                         ellGetConstraintName( & sRelList->mRelatedHandle ) );

            stlSnprintf( aStringBuffer,
                         ELDC_DUMP_PRINT_BUFFER_SIZE,
                         "%s(CrtID:%ld,CrtSCN:%ld,DrpID:%ld,DrpSCN:%ld,DrpFlag:%d,ModCnt:%ld,GModCnt:%ld,MemUse:%ld)",
                         aStringBuffer,
                         sRelList->mVersionInfo.mCreateTransID,
                         sRelList->mVersionInfo.mCreateCommitSCN,
                         sRelList->mVersionInfo.mDropTransID,
                         sRelList->mVersionInfo.mDropCommitSCN,
                         sRelList->mVersionInfo.mDropFlag,
                         sRelList->mVersionInfo.mModifyingCnt,
                         sRelList->mVersionInfo.mTryModifyCnt,
                         sRelList->mVersionInfo.mMemUseCnt );
        }

        sSize = stlSnprintf( aStringBuffer,
                             ELDC_DUMP_PRINT_BUFFER_SIZE,
                             "%s]",
                             aStringBuffer );
    }

    sSize = stlSnprintf( aStringBuffer,
                         ELDC_DUMP_PRINT_BUFFER_SIZE,
                         "%s)",
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
 * @brief Table Name Cache 의 Hash Data 의 String 출력을 생성하는 함수 포인터 
 * @param[in]  aHashDataTableName  Table Name 의 Hash Data
 * @param[out] aStringBuffer        출력 정보를 위한 String Buffer 공간 
 * @remarks
 */
void eldcPrintHashDataTableName( void * aHashDataTableName, stlChar * aStringBuffer )
{
    stlInt32   sSize = 0;
    eldcHashDataTableName * sHashData = (eldcHashDataTableName *) aHashDataTableName;

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "Table Name Hash Data( " );

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sKEY_SCHEMA_ID: %ld, ",
                 aStringBuffer,
                 sHashData->mKeySchemaID );
    
    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sKEY_TABLE_NAME: %s, ",
                 aStringBuffer,
                 sHashData->mKeyTableName );

    sSize = stlSnprintf( aStringBuffer,
                         ELDC_DUMP_PRINT_BUFFER_SIZE,
                         "%sDATA_TABLE_ID: %ld )",
                         aStringBuffer,
                         sHashData->mDataTableID );

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
 * @brief SQL-Table Cache 를 위한 Dump Handle을 설정
 * @remarks
 */

void eldcSetSQLTableDumpHandle()
{
    /*
     * ELDC_OBJECTCACHE_TABLE_ID
     */

    gEldcObjectDump[ELDC_OBJECTCACHE_TABLE_ID].mDictHash
        = ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_TABLE_ID );
    gEldcObjectDump[ELDC_OBJECTCACHE_TABLE_ID].mPrintHashData
        = eldcPrintHashDataTableID;
    gEldcObjectDump[ELDC_OBJECTCACHE_TABLE_ID].mPrintObjectDesc
        = eldcPrintTableDesc;
    
    /*
     * ELDC_OBJECTCACHE_TABLE_NAME
     */

    gEldcObjectDump[ELDC_OBJECTCACHE_TABLE_NAME].mDictHash
        = ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_TABLE_NAME );
    gEldcObjectDump[ELDC_OBJECTCACHE_TABLE_NAME].mPrintHashData
        = eldcPrintHashDataTableName;
    gEldcObjectDump[ELDC_OBJECTCACHE_TABLE_NAME].mPrintObjectDesc
        = eldcPrintINVALID;
}


/**
 * @brief SQL-Table 객체를 위한 Cache 를 구축한다.
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aEnv      Environment
 * @remarks
 */
stlStatus eldcBuildTableCache( smlTransId     aTransID,
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
                                      ELDT_TABLE_ID_TABLES,
                                      & sBucketCnt,
                                      aEnv )
             == STL_SUCCESS );
    
    /**
     * 소수에 해당하는 Hash Bucket 개수의 조정
     */

    sBucketCnt = ellGetPrimeBucketCnt( sBucketCnt );
    
    /**
     * Table ID 를 위한 Cache 초기화 
     */

    STL_TRY( eldcCreateDictHash( ELL_OBJECT_CACHE_ADDR( ELDC_OBJECTCACHE_TABLE_ID ),
                                 sBucketCnt,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Table Name 을 위한 Cache 초기화 
     */
    
    STL_TRY( eldcCreateDictHash( ELL_OBJECT_CACHE_ADDR( ELDC_OBJECTCACHE_TABLE_NAME ),
                                 sBucketCnt,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Dump Handle 설정
     */
    
    eldcSetSQLTableDumpHandle();
    
    /**
     * SQL-Table Cache 구축 
     */

    STL_TRY( eldcBuildCacheSQLTable( aTransID,
                                     aStmt,
                                     aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief SQL-Table 을 위한 Cache 를 구축한다.
 * @param[in]  aTransID  Transaction ID
 * @param[in]  aStmt     Statement
 * @param[in]  aEnv      Environment
 * @remarks
 */
stlStatus eldcBuildCacheSQLTable( smlTransId       aTransID,
                                  smlStatement   * aStmt,
                                  ellEnv         * aEnv )
{
    stlBool             sBeginSelect = STL_FALSE;
    
    void              * sTableHandle = NULL;
    knlValueBlockList * sTableValueList = NULL;

    void                * sIterator  = NULL;
    smlIteratorProperty   sIteratorProperty;

    ellTableType        sTableType = ELL_TABLE_TYPE_NA;

    dtlDataValue      * sDataValue = NULL;

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
    
    /**
     * DEFINITION_SCHEMA.TABLES 테이블의 table handle 을 획득
     */
    
    sTableHandle = gEldTablePhysicalHandle[ELDT_TABLE_ID_TABLES];

    /**
     * DEFINITION_SCHEMA.TABLES 테이블의 Value List와 Row Block 할당
     */

    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_TABLES,
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
         * SEQ.NEXTVAL 등의 정의가
         * Sequence 의 Nextval 일 수도 있고,
         * Table SEQ 의 NEXTVAL 컬럼일 수도 있다.
         * 이러한, Naming Resolution을 위해 Sequence Name 도 TABLES 에서 관리된다.
         */

        sDataValue = eldFindDataValue( sTableValueList,
                                       ELDT_TABLE_ID_TABLES,
                                       ELDT_Tables_ColumnOrder_TABLE_TYPE_ID );
        if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
        {
            STL_ASSERT(0);
        }
        else
        {
            stlMemcpy( & sTableType,
                       sDataValue->mValue,
                       sDataValue->mLength );
            STL_DASSERT( STL_SIZEOF(sTableType) == sDataValue->mLength );
        }

        if ( (sTableType == ELL_TABLE_TYPE_SEQUENCE) ||
             (sTableType == ELL_TABLE_TYPE_SYNONYM) )
        {
            /**
             * Sequence, Synonym 등 Namiming Resolution을 위해 TABLE_ID 를 사용하는 객체는
             * Table Cache 에 추가하지 않는다.
             */
        }
        else
        {
            /**
             * Cache 정보를 추가
             */
            
            STL_TRY( eldcInsertCacheTable( aTransID,
                                           aStmt,
                                           sTableValueList,
                                           aEnv )
                     == STL_SUCCESS );
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
 * @brief Table ID 를 이용해 Table Handle 을 획득한다.
 * @param[in]  aTransID           Transaction ID
 * @param[in]  aViewSCN           View SCN
 * @param[in]  aTableID           Table ID
 * @param[out] aTableDictHandle   Table Dictionary Handle
 * @param[out] aExist             존재 여부 
 * @param[in]  aEnv               Environment
 * @remarks
 */
stlStatus eldcGetTableHandleByID( smlTransId           aTransID,
                                  smlScn               aViewSCN,
                                  stlInt64             aTableID,
                                  ellDictHandle      * aTableDictHandle,
                                  stlBool            * aExist,
                                  ellEnv             * aEnv )
{
    eldcHashElement * sHashElement = NULL;

    stlBool sIsValid = STL_FALSE;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aTableID != ELL_DICT_OBJECT_ID_NA,
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExist != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * ELDC_OBJECTCACHE_TABLE_ID Hash 로부터 검색
     */

    STL_TRY( eldcFindHashElement( aTransID,
                                  aViewSCN,
                                  ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_TABLE_ID ),
                                  & aTableID,
                                  eldcHashFuncTableID,
                                  eldcCompareFuncTableID,
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
             * Table Handle 설정
             */

            ellSetDictHandle( aTransID,
                              aViewSCN,
                              aTableDictHandle,
                              ELL_OBJECT_TABLE,
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
 * @brief Table Name 을 이용해 Table Handle 을 획득한다.
 * @param[in]  aTransID           Transaction ID
 * @param[in]  aViewSCN           View SCN
 * @param[in]  aSchemaID          Schema ID
 * @param[in]  aTableName         Table Name
 * @param[out] aTableDictHandle   Table Descriptor
 * @param[out] aExist             존재 여부 
 * @param[in]  aEnv               Envirionment
 * @remarks
 */
stlStatus eldcGetTableHandleByName( smlTransId           aTransID,
                                    smlScn               aViewSCN,
                                    stlInt64             aSchemaID,
                                    stlChar            * aTableName,
                                    ellDictHandle      * aTableDictHandle,
                                    stlBool            * aExist,
                                    ellEnv             * aEnv )
{
    eldcHashElement        * sHashElement = NULL;

    eldcHashDataTableName  * sHashData = NULL;

    stlBool sIsValid = STL_FALSE;
    
    eldcHashKeyTableName  sHashKey;
    stlMemset( & sHashKey, 0x00, STL_SIZEOF(eldcHashKeyTableName) );
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aSchemaID > ELL_DICT_OBJECT_ID_NA,
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableName != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExist != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * ELDC_OBJECTCACHE_TABLE_NAME Hash 로부터 검색
     */

    sHashKey.mSchemaID  = aSchemaID;
    sHashKey.mTableName = aTableName;
    
    STL_TRY( eldcFindHashElement( aTransID,
                                  aViewSCN,
                                  ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_TABLE_NAME ),
                                  & sHashKey,
                                  eldcHashFuncTableName,
                                  eldcCompareFuncTableName,
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
        sHashData = (eldcHashDataTableName *) sHashElement->mHashData;
        STL_TRY( eldcIsVisibleCache( aTransID,
                                     aViewSCN,
                                     & sHashData->mDataHashElement->mVersionInfo,
                                     & sIsValid,
                                     aEnv )
                 == STL_SUCCESS );
        
        if ( sIsValid == STL_TRUE )
        {
            /**
             * Table Handle 설정
             */

            KNL_BREAKPOINT(
                KNL_BREAKPOINT_ELDCGETTABLEHANDLEBYNAME_BEFORE_SET_DICT_HANDLE,
                KNL_ENV(aEnv) );
            
            ellSetDictHandle( aTransID,
                              aViewSCN,
                              aTableDictHandle,
                              ELL_OBJECT_TABLE,
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







/**
 * @brief Table 과 관련된 Object Dictionary Handle Array 를 얻는다.
 * @param[in]  aTransID             Transaction ID
 * @param[in]  aViewSCN             View SCN
 * @param[in]  aTableHandle         Table Handle
 * @param[in]  aRelatedType         Related Object 의 유형
 * @param[in]  aRelatedCnt          Related Object 의 갯수
 * @param[out] aRelatedHandleArray  Related Object Dictionary Handle Array
 * @param[in]  aRegionMem           Regione Memory
 * @param[in]  aEnv                 Environment
 * @remarks
 */
stlStatus eldcGetTableRelatedObjectHandle( smlTransId               aTransID,
                                           smlScn                   aViewSCN,
                                           ellDictHandle          * aTableHandle,
                                           ellTableRelatedObject    aRelatedType,
                                           stlInt32               * aRelatedCnt,
                                           ellDictHandle         ** aRelatedHandleArray,
                                           knlRegionMem           * aRegionMem,
                                           ellEnv                 * aEnv )
{
    stlInt32        sObjectCnt = 0;
    stlInt32        sIdx = 0;
    ellDictHandle * sObjectHandleArray = NULL;
    
    stlBool        sIsValid = STL_FALSE;
    
    eldcHashElement     * sHashElement = NULL;
    eldcHashDataTableID * sHashData = NULL;

    eldcHashRelatedObjList * sHashRelated = NULL;

    ellObjectList * sObjectList = NULL;
    
    ellObjectItem   * sObjectItem = NULL;
    ellDictHandle   * sObjectHandle = NULL;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aTableHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aRelatedType > ELL_TABLE_RELATED_NA) &&
                        (aRelatedType < ELL_TABLE_RELATED_MAX),
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRelatedCnt > 0, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRelatedHandleArray != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Related Data 획득
     */

    sHashElement = (eldcHashElement*) aTableHandle->mObjHashElement;
    
    sHashData = (eldcHashDataTableID *) sHashElement->mHashData;

    /**
     * 유효한 Related Object 의 Handle 을 구성함.
     */

    switch (aRelatedType)
    {
        case ELL_TABLE_RELATED_PRIMARY_KEY:         
            sHashRelated = sHashData->mPrimaryKey;
            break;
        case ELL_TABLE_RELATED_UNIQUE_KEY:
            sHashRelated = sHashData->mUniqueKey;
            break;
        case ELL_TABLE_RELATED_FOREIGN_KEY:
            sHashRelated = sHashData->mForeignKey;
            break;
        case ELL_TABLE_RELATED_CHILD_FOREIGN_KEY:
            sHashRelated = sHashData->mChildForeignKey;
            break;
        case ELL_TABLE_RELATED_UNIQUE_INDEX:
            sHashRelated = sHashData->mUniqueIndex;
            break;
        case ELL_TABLE_RELATED_NON_UNIQUE_INDEX:
            sHashRelated = sHashData->mNonUniqueIndex;
            break;
        case ELL_TABLE_RELATED_CHECK_NOT_NULL:
            sHashRelated = sHashData->mCheckNotNull;
            break;
        case ELL_TABLE_RELATED_CHECK_CLAUSE:
            sHashRelated = sHashData->mCheckClause;
            break;
        default:
            sHashRelated = NULL;
            break;
    }

    STL_TRY( ellInitObjectList( & sObjectList, aRegionMem, aEnv ) == STL_SUCCESS );
    
    for ( ; sHashRelated != NULL; sHashRelated = sHashRelated->mNext )
    {
        STL_TRY( eldcIsVisibleCache( aTransID,
                                     aViewSCN,
                                     & sHashRelated->mVersionInfo,
                                     & sIsValid,
                                     aEnv )
                 == STL_SUCCESS );
        
        if ( sIsValid == STL_TRUE )
        {
            /**
             * Object Handle 을 추가
             */
            STL_TRY( ellAppendObjectItem( sObjectList,
                                          & sHashRelated->mRelatedHandle,
                                          aRegionMem,
                                          aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            /* Non Visible Object 임 */
        }
    }

    /**
     * Array Handle 을 구성함
     */

    sObjectCnt = sObjectList->mCount;

    if ( sObjectCnt > 0 )
    {
        STL_TRY( knlAllocRegionMem( aRegionMem,
                                    STL_SIZEOF(ellDictHandle) * sObjectCnt,
                                    (void **) & sObjectHandleArray,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    else
    {
        sObjectHandleArray = NULL;
    }
        

    sIdx = 0;
    STL_RING_FOREACH_ENTRY( & sObjectList->mHeadList, sObjectItem )
    {
        sObjectHandle = & sObjectItem->mObjectHandle;

        stlMemcpy( & sObjectHandleArray[sIdx], sObjectHandle, STL_SIZEOF(ellDictHandle) );
        sIdx++;
    }

    /**
     * Output 설정 
     */
    
    *aRelatedCnt = sObjectCnt;
    *aRelatedHandleArray = sObjectHandleArray;
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}














/**
 * @brief SQL Table 을 위한 Cache 정보를 추가한다.
 * @param[in] aTransID          Transaction ID
 * @param[in] aStmt             Statement
 * @param[in] aValueList        Data Value List
 * @param[in] aEnv              Environment
 * @remarks
 */
stlStatus eldcInsertCacheTable( smlTransId          aTransID,
                                smlStatement      * aStmt,
                                knlValueBlockList * aValueList,
                                ellEnv            * aEnv )
{
    ellSessObjectMgr  * sObjMgr = ELL_SESS_OBJ(aEnv);
    
    ellTableDesc   * sTableDesc = NULL;
    
    eldcHashElement * sHashElementTableID = NULL;
    eldcHashElement * sHashElementTableName = NULL;
    eldcLocalCache  * sNewLocalCache = NULL;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aValueList != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * New Local Cache 공간 할당
     */
    
    STL_TRY( knlAllocDynamicMem( & sObjMgr->mMemSessObj,
                                 STL_SIZEOF(eldcLocalCache),
                                 (void **) & sNewLocalCache,
                                 KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * Table ID를 위한 Table Descriptor 구성
     */
    
    STL_TRY( eldcMakeTableDesc( aTransID,
                                aStmt,
                                aValueList,
                                & sTableDesc,
                                aEnv )
             == STL_SUCCESS );
    
    /**
     * Table ID 를 위한 Hash Element 구성
     */
    
    STL_TRY( eldcMakeHashElementTableID( & sHashElementTableID,
                                         aTransID,
                                         aStmt,
                                         sTableDesc,
                                         aEnv )
             == STL_SUCCESS );
                                                  
    /**
     * ELDC_OBJECTCACHE_TABLE_ID Hash 에 추가 
     */
    
    STL_TRY( eldcInsertHashElement( ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_TABLE_ID ),
                                    sHashElementTableID,
                                    aEnv )
             == STL_SUCCESS );

    /**
     * Table Name 을 위한 Hash Element 구성
     */

    STL_TRY( eldcMakeHashElementTableName( & sHashElementTableName,
                                           aTransID,
                                           sTableDesc,
                                           sHashElementTableID,
                                           aEnv )
             == STL_SUCCESS );
    
    /**
     * ELDC_OBJECTCACHE_TABLE_NAME 에 추가 
     */

    STL_TRY( eldcInsertHashElement( ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_TABLE_NAME ),
                                    sHashElementTableName,
                                    aEnv )
             == STL_SUCCESS );
    
    /**
     * 객체가 변경되고 있음을 세션에 기록한다.
     */
    
    ELL_SESS_ENV(aEnv)->mLocalModifySeq += 1;
    
    eldcInsertLocalCache( sHashElementTableID, sNewLocalCache, aEnv );

    return STL_SUCCESS;

    STL_FINISH;

    if ( sNewLocalCache != NULL )
    {
        (void) knlFreeDynamicMem( & sObjMgr->mMemSessObj,
                                  (void *) sNewLocalCache,
                                  KNL_ENV(aEnv) );
        sNewLocalCache = NULL;
    }
    
    return STL_FAILURE;
}




/**
 * @brief Non-Service Performance View 를  위한 Cache 정보를 추가한다.
 * @param[in] aValueList         Data Value List
 * @param[in] aViewColumnString  View Column Definition
 * @param[in] aViewQueryString   View Query Definition
 * @param[in] aEnv               Environment
 * @remarks
 */
stlStatus eldcInsertCacheNonServicePerfView( knlValueBlockList * aValueList,
                                             stlChar           * aViewColumnString,
                                             stlChar           * aViewQueryString,
                                             ellEnv            * aEnv )
{
    ellTableDesc   * sTableDesc = NULL;
    
    eldcHashElement * sHashElementTableID = NULL;
    eldcHashElement * sHashElementTableName = NULL;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aValueList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aViewColumnString != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aViewQueryString != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Table ID를 위한 Table Descriptor 구성
     */

    STL_TRY( eldcMakeNonServicePerfViewDesc( aValueList,
                                             aViewColumnString,
                                             aViewQueryString,
                                             & sTableDesc,
                                             aEnv )
             == STL_SUCCESS );
        
    /**
     * Table ID 를 위한 Hash Element 구성
     */
    
    STL_TRY( eldcMakeHashElementTableID( & sHashElementTableID,
                                         SML_INVALID_TRANSID,
                                         NULL, /* aStmt */
                                         sTableDesc,
                                         aEnv )
             == STL_SUCCESS );

    /* Hash Element 의 Version 정보 변경 */
    eldcSetStableVersionInfo( & sHashElementTableID->mVersionInfo );

    /**
     * ELDC_OBJECTCACHE_TABLE_ID Hash 에 추가 
     */
    
    STL_TRY( eldcInsertHashElement( ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_TABLE_ID ),
                                    sHashElementTableID,
                                    aEnv )
             == STL_SUCCESS );

    /**
     * Table Name 을 위한 Hash Element 구성
     */

    STL_TRY( eldcMakeHashElementTableName( & sHashElementTableName,
                                           SML_INVALID_TRANSID,
                                           sTableDesc,
                                           sHashElementTableID,
                                           aEnv )
             == STL_SUCCESS );

    /* Hash Element 의 Version 정보 변경 */
    eldcSetStableVersionInfo( & sHashElementTableName->mVersionInfo );

    /**
     * ELDC_OBJECTCACHE_TABLE_NAME 에 추가 
     */

    STL_TRY( eldcInsertHashElement( ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_TABLE_NAME ),
                                    sHashElementTableName,
                                    aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief Table ID를 기준으로 SQL-Table Cache 를 추가한다.
 * @param[in]  aTransID      Transaction ID
 * @param[in]  aStmt         Statement
 * @param[in]  aTableID      Table ID
 * @param[in]  aEnv          Environment
 * @remarks
 */
stlStatus eldcInsertCacheTableByTableID( smlTransId          aTransID,
                                         smlStatement      * aStmt,
                                         stlInt64            aTableID,
                                         ellEnv            * aEnv )
{
    smlRid              sRowRid;
    smlScn              sRowScn;
    smlRowBlock         sRowBlock;
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
     * DEFINITION_SCHEMA.TABLES 테이블의 table handle 을 획득
     */
    
    sTableHandle = gEldTablePhysicalHandle[ELDT_TABLE_ID_TABLES];

    /**
     * DEFINITION_SCHEMA.TABLES 테이블의 Value List와 Row Block 할당
     */

    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_TABLES,
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
                                            ELDT_TABLE_ID_TABLES,
                                            ELDT_Tables_ColumnOrder_TABLE_ID );

    /**
     * Key Range 생성
     */

    STL_TRY( eldMakeOneEquiRange( aTransID,
                                  aStmt,
                                  ELDT_TABLE_ID_TABLES,
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
     * SQL-Table Cache 정보를 추가한다.
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
            STL_TRY( eldcInsertCacheTable( aTransID,
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
 * @brief SQL Table 을 위한 Cache 정보를 삭제한다.
 * @param[in] aTransID          Transaction ID
 * @param[in] aTableHandle      Table Dictionary Handle
 * @param[in] aEnv              Environment
 * @remarks
 */
stlStatus eldcDeleteCacheTable( smlTransId          aTransID,
                                ellDictHandle     * aTableHandle,
                                ellEnv            * aEnv )
{
    ellSessObjectMgr  * sObjMgr = ELL_SESS_OBJ(aEnv);
    
    eldcHashElement * sHashElementID = NULL;
    eldcHashElement * sHashElementName = NULL;
    eldcLocalCache  * sLocalCache;
    eldcLocalCache  * sNewLocalCache = NULL;

    stlInt64   sTableID = ELL_DICT_OBJECT_ID_NA;
    
    eldcHashKeyTableName  sHashKey;
    stlMemset( & sHashKey, 0x00, STL_SIZEOF(eldcHashKeyTableName) );
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aTableHandle != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * New Local Cache 공간 할당
     */
    
    STL_TRY( knlAllocDynamicMem( & sObjMgr->mMemSessObj,
                                 STL_SIZEOF(eldcLocalCache),
                                 (void **) & sNewLocalCache,
                                 KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * ELDC_OBJECTCACHE_TABLE_ID Hash 에서 삭제 
     */

    sTableID = ellGetTableID( aTableHandle );
    
    STL_TRY( eldcFindHashElement( aTransID,
                                  SML_MAXIMUM_STABLE_SCN,
                                  ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_TABLE_ID ),
                                  & sTableID,
                                  eldcHashFuncTableID,
                                  eldcCompareFuncTableID,
                                  & sHashElementID,
                                  aEnv )
             == STL_SUCCESS );

    STL_TRY( eldcDeleteHashElement( aTransID,
                                    ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_TABLE_ID ),
                                    sHashElementID,
                                    aEnv )
             == STL_SUCCESS );
                                    
    /**
     * ELDC_OBJECTCACHE_TABLE_NAME Hash 에서 삭제 
     */

    sHashKey.mSchemaID    = ellGetTableSchemaID( aTableHandle );
    sHashKey.mTableName   = ellGetTableName( aTableHandle );
    
    STL_TRY( eldcFindHashElement( aTransID,
                                  SML_MAXIMUM_STABLE_SCN,
                                  ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_TABLE_NAME ),
                                  & sHashKey,
                                  eldcHashFuncTableName,
                                  eldcCompareFuncTableName,
                                  & sHashElementName,
                                  aEnv )
             == STL_SUCCESS );

    STL_TRY( eldcDeleteHashElement( aTransID,
                                    ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_TABLE_NAME ),
                                    sHashElementName,
                                    aEnv )
             == STL_SUCCESS );
    
    /**
     * 객체가 변경되고 있음을 세션에 기록한다.
     */
    
    ELL_SESS_ENV(aEnv)->mLocalModifySeq += 1;
    
    eldcSearchLocalCache( sHashElementID, &sLocalCache, aEnv );

    if( sLocalCache == NULL )
    {
        eldcInsertLocalCache( sHashElementID, sNewLocalCache, aEnv );
        
        STL_TRY( eldcDeleteLocalCache( sNewLocalCache, aEnv ) == STL_SUCCESS );
    }
    else
    {
        
        STL_TRY( eldcDeleteLocalCache( sLocalCache, aEnv ) == STL_SUCCESS );
        
        STL_TRY( knlFreeDynamicMem( & sObjMgr->mMemSessObj,
                                    (void *) sNewLocalCache,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        sNewLocalCache = NULL;
    }

    return STL_SUCCESS;

    STL_FINISH;

    if ( sNewLocalCache != NULL )
    {
        (void) knlFreeDynamicMem( & sObjMgr->mMemSessObj,
                                  (void *) sNewLocalCache,
                                  KNL_ENV(aEnv) );
        sNewLocalCache = NULL;
    }
    
    return STL_FAILURE;
}















/**
 * @brief 읽은 Value List 로부터 SQL Table 의 Table Descriptor 를 생성한다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[out] aTableDesc       Table Descriptor
 * @param[in]  aValueList       Value List
 * @param[in]  aEnv             Environment 포인터
 * @remarks
 */
stlStatus eldcMakeTableDesc( smlTransId          aTransID,
                             smlStatement      * aStmt,
                             knlValueBlockList * aValueList,
                             ellTableDesc     ** aTableDesc,
                             ellEnv            * aEnv )
{
    stlInt64           sTableID   = ELL_DICT_OBJECT_ID_NA;
    ellTableDesc     * sTableDesc = NULL;
    ellTableType       sTableType = ELL_TABLE_TYPE_NA;

    dtlDataValue     * sDataValue = NULL;

    stlInt32           sTableDescSize = 0;
    
    stlInt32           sViewColDefLen = 0;
    stlInt32           sViewSelectLen = 0;

    stlInt32           sColumnCnt = 0;

    stlInt32           sTableNameLen = 0;

    ellBaseTableDepDesc     * sBaseTableDesc = NULL;
    ellViewedTableDepDesc   * sViewDesc = NULL;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aTableDesc != NULL,        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aValueList != NULL      , ELL_ERROR_STACK(aEnv) );

    /**
     * Table ID 획득
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_TABLES,
                                   ELDT_Tables_ColumnOrder_TABLE_ID );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( & sTableID,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sTableID) == sDataValue->mLength );
    }

    /**
     * Table Descriptor 를 위한 공간의 크기 계산
     */

    /* Table Name 의 길이 */
    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_TABLES,
                                   ELDT_Tables_ColumnOrder_TABLE_NAME );
    sTableNameLen = sDataValue->mLength;

    /* Table Type */
    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_TABLES,
                                   ELDT_Tables_ColumnOrder_TABLE_TYPE_ID );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( & sTableType,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sTableType) == sDataValue->mLength );
    }

    /* View 구문의 길이 */
    if ( sTableType == ELL_TABLE_TYPE_VIEW )
    {
        /**
         * View 의 정보를 획득
         */

        STL_TRY( eldcGetViewStringLength( aTransID,
                                          aStmt,
                                          sTableID,
                                          & sViewColDefLen,
                                          & sViewSelectLen,
                                          aEnv )
                 == STL_SUCCESS );

        /**
         * View Descriptor 는 Column Descriptor 를 별도로 갖지 않는다.
         */
        
        sColumnCnt = 0;
    }
    else
    {
        /**
         * Table 에 소속된 컬럼의 개수를 얻는다.
         */
        
        STL_TRY( eldcGetColumnCount( aTransID,
                                     aStmt,
                                     sTableID,
                                     & sColumnCnt,
                                     aEnv )
                 == STL_SUCCESS );
    
        sViewColDefLen = 0;
        sViewSelectLen = 0;
    }
    
    sTableDescSize = eldcCalSizeTableDesc( sTableNameLen,
                                           sColumnCnt,
                                           sViewColDefLen,
                                           sViewSelectLen );

    /**
     * Table Descriptor 를 위한 공간 할당
     */
    
    STL_TRY( eldAllocCacheMem( (void **) & sTableDesc,
                               sTableDescSize,
                               aEnv )
             == STL_SUCCESS );
    
    /**
     * Table Descriptor 의 각 정보를 설정한다.
     */

    /*
     * mOwnerID
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_TABLES,
                                   ELDT_Tables_ColumnOrder_OWNER_ID );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( & sTableDesc->mOwnerID,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sTableDesc->mOwnerID) == sDataValue->mLength );
    }

    /*
     * mSchemaID
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_TABLES,
                                   ELDT_Tables_ColumnOrder_SCHEMA_ID );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( & sTableDesc->mSchemaID,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sTableDesc->mSchemaID) == sDataValue->mLength );
    }
    
    /*
     * mTableID
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_TABLES,
                                   ELDT_Tables_ColumnOrder_TABLE_ID );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( & sTableDesc->mTableID,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sTableDesc->mTableID) == sDataValue->mLength );
    }
    
    /*
     * mTablespaceID
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_TABLES,
                                   ELDT_Tables_ColumnOrder_TABLESPACE_ID );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        /* viewed table 등은 물리적 정보가 없다. */
        sTableDesc->mTablespaceID = ELL_DICT_TABLESPACE_ID_NA;
    }
    else
    {
        stlMemcpy( & sTableDesc->mTablespaceID,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sTableDesc->mTablespaceID) == sDataValue->mLength );
    }

    /*
     * mPhysicalID
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_TABLES,
                                   ELDT_Tables_ColumnOrder_PHYSICAL_ID );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        /* viewed table 등은 물리적 정보가 없다. */
        sTableDesc->mPhysicalID = ELL_DICT_OBJECT_ID_NA;
    }
    else
    {
        stlMemcpy( & sTableDesc->mPhysicalID,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sTableDesc->mPhysicalID) == sDataValue->mLength );
    }
    
    /*
     * mTableName
     * - 메모리 공간 내에서의 위치 계산
     */

    sTableDesc->mTableName = eldcGetTableNamePtr( sTableDesc );

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_TABLES,
                                   ELDT_Tables_ColumnOrder_TABLE_NAME );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( sTableDesc->mTableName,
                   sDataValue->mValue,
                   sDataValue->mLength );
        sTableDesc->mTableName[sDataValue->mLength] = '\0';
    }

    /*
     * mTableType
     */

    sTableDesc->mTableType = sTableType;

    /*
     * mIsSuppPK
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_TABLES,
                                   ELDT_Tables_ColumnOrder_IS_SET_SUPPLOG_PK );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( & sTableDesc->mIsSuppPK,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sTableDesc->mIsSuppPK) == sDataValue->mLength );
    }
    
    /*
     * mTableHandle
     */

    if ( sTableDesc->mPhysicalID == ELL_DICT_OBJECT_ID_NA )
    {
        sTableDesc->mTableHandle = NULL;
    }
    else
    {
        STL_TRY( smlGetRelationHandle( sTableDesc->mPhysicalID,
                                       & sTableDesc->mTableHandle,
                                       SML_ENV( aEnv ) )
                 == STL_SUCCESS );
        STL_ASSERT( sTableDesc->mTableHandle != NULL );
    }
    
    /*
     * mColumnCnt
     * mColumnDictHandle
     */

    if ( sTableType == ELL_TABLE_TYPE_VIEW )
    {
        /**
         * View 는 Column Handle 을 별도로 소유하지 않는다.
         * - View Column List 를 이용해 Syntax Replacement 함.
         */

        sTableDesc->mColumnCnt = 0;
        sTableDesc->mColumnDictHandle = NULL;
    }
    else
    {
        sTableDesc->mColumnCnt = sColumnCnt;
        
        /*
         * Table 에 속한 Column Dictionary Handle 들을 설정함.
         */
        
        sTableDesc->mColumnDictHandle = 
            eldcGetTableColumnDictHandlePtr( sTableDesc, sTableNameLen );
        
        STL_TRY( eldcSetColumnDictHandle( aTransID,
                                          aStmt,
                                          sTableDesc,
                                          aEnv )
                 == STL_SUCCESS );
    }
                                         
    /**
     * Table Type 에 따라 종속적인 descriptor 정보를 설정한다.
     */

    switch (sTableType)
    {
        case ELL_TABLE_TYPE_BASE_TABLE:
            {
                sBaseTableDesc = (ellBaseTableDepDesc *) & sTableDesc->mTableTypeDesc.mBaseTable;
                sBaseTableDesc->mDummy = sTableID;
                break;
            }
        case ELL_TABLE_TYPE_VIEW:
            {
                sViewDesc = (ellViewedTableDepDesc *) & sTableDesc->mTableTypeDesc.mView;

                sViewDesc->mViewColumnString = eldcGetViewColDefPtr( sTableDesc,
                                                                     sTableNameLen );
                
                sViewDesc->mViewQueryString = eldcGetViewSelectPtr( sTableDesc,
                                                                    sTableNameLen,
                                                                    sViewColDefLen );

                STL_TRY( eldcSetViewStringDesc( aTransID,
                                                aStmt,
                                                sTableID,
                                                sViewDesc->mViewColumnString,
                                                sViewColDefLen,
                                                sViewDesc->mViewQueryString,
                                                sViewSelectLen,
                                                aEnv )
                         == STL_SUCCESS );
                                                
                break;
            }

        case ELL_TABLE_TYPE_GLOBAL_TEMPORARY:
        case ELL_TABLE_TYPE_LOCAL_TEMPORARY:
        case ELL_TABLE_TYPE_SYSTEM_VERSIONED:
        case ELL_TABLE_TYPE_SEQUENCE:
        case ELL_TABLE_TYPE_SYNONYM:
            {
                /* 지원하지 않는 Table 유형임 */
                STL_ASSERT(0);
                break;
            }
        case ELL_TABLE_TYPE_FIXED_TABLE:
        case ELL_TABLE_TYPE_DUMP_TABLE:
            {
                /* Fixed Table 은 별도의 로직으로 처리하므로 여기에 들어오면 안됨 */
                STL_ASSERT(0);
                break;
            }
        default:
            {
                /* 지원하지 않는 Table 유형임 */
                STL_ASSERT(0);
                break;
            }
    }
    
    /**
     * return 값 설정
     */
    
    *aTableDesc = sTableDesc;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Non-Service Performance View 를  위한 Cache Descriptor 를 구성
 * @param[in]  aValueList         Data Value List
 * @param[in]  aViewColumnString  View Column Definition
 * @param[in]  aViewQueryString   View Query Definition
 * @param[out] aTableDesc         Table Descriptor
 * @param[in]  aEnv               Environment
 * @remarks
 */
stlStatus eldcMakeNonServicePerfViewDesc( knlValueBlockList * aValueList,
                                          stlChar           * aViewColumnString,
                                          stlChar           * aViewQueryString,
                                          ellTableDesc     ** aTableDesc,
                                          ellEnv            * aEnv )
{
    stlInt64           sTableID   = ELL_DICT_OBJECT_ID_NA;
    ellTableDesc     * sTableDesc = NULL;
    ellTableType       sTableType = ELL_TABLE_TYPE_NA;

    dtlDataValue     * sDataValue = NULL;

    stlInt32           sTableDescSize = 0;
    
    stlInt32           sViewColDefLen = 0;
    stlInt32           sViewSelectLen = 0;

    stlInt32           sColumnCnt = 0;

    stlInt32           sTableNameLen = 0;

    ellViewedTableDepDesc   * sViewDesc = NULL;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aTableDesc != NULL,        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aViewColumnString != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aViewQueryString != NULL,  ELL_ERROR_STACK(aEnv) );

    /**
     * Table ID 획득
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_TABLES,
                                   ELDT_Tables_ColumnOrder_TABLE_ID );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( & sTableID,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sTableID) == sDataValue->mLength );
    }

    /**
     * Table Descriptor 를 위한 공간의 크기 계산
     */

    /* Table Name 의 길이 */
    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_TABLES,
                                   ELDT_Tables_ColumnOrder_TABLE_NAME );
    sTableNameLen = sDataValue->mLength;

    /* Table Type */
    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_TABLES,
                                   ELDT_Tables_ColumnOrder_TABLE_TYPE_ID );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( & sTableType,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sTableType) == sDataValue->mLength );
    }

    STL_DASSERT ( sTableType == ELL_TABLE_TYPE_VIEW );

    sViewColDefLen = stlStrlen( aViewColumnString );
    sViewSelectLen = stlStrlen( aViewQueryString );
        
    sColumnCnt = 0;
    
    sTableDescSize = eldcCalSizeTableDesc( sTableNameLen,
                                           sColumnCnt,
                                           sViewColDefLen,
                                           sViewSelectLen );

    /**
     * Table Descriptor 를 위한 공간 할당
     */
    
    STL_TRY( eldAllocCacheMem( (void **) & sTableDesc,
                               sTableDescSize,
                               aEnv )
             == STL_SUCCESS );
    
    /**
     * Table Descriptor 의 각 정보를 설정한다.
     */

    /*
     * mOwnerID
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_TABLES,
                                   ELDT_Tables_ColumnOrder_OWNER_ID );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( & sTableDesc->mOwnerID,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sTableDesc->mOwnerID) == sDataValue->mLength );
    }

    /*
     * mSchemaID
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_TABLES,
                                   ELDT_Tables_ColumnOrder_SCHEMA_ID );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( & sTableDesc->mSchemaID,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sTableDesc->mSchemaID) == sDataValue->mLength );
    }
    
    /*
     * mTableID
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_TABLES,
                                   ELDT_Tables_ColumnOrder_TABLE_ID );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( & sTableDesc->mTableID,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sTableDesc->mTableID) == sDataValue->mLength );
    }
    
    /*
     * mTablespaceID
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_TABLES,
                                   ELDT_Tables_ColumnOrder_TABLESPACE_ID );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        /* viewed table 등은 물리적 정보가 없다. */
        sTableDesc->mTablespaceID = ELL_DICT_TABLESPACE_ID_NA;
    }
    else
    {
        stlMemcpy( & sTableDesc->mTablespaceID,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sTableDesc->mTablespaceID) == sDataValue->mLength );
    }

    /*
     * mPhysicalID
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_TABLES,
                                   ELDT_Tables_ColumnOrder_PHYSICAL_ID );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        /* viewed table 등은 물리적 정보가 없다. */
        sTableDesc->mPhysicalID = ELL_DICT_OBJECT_ID_NA;
    }
    else
    {
        stlMemcpy( & sTableDesc->mPhysicalID,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sTableDesc->mPhysicalID) == sDataValue->mLength );
    }
    
    /*
     * mTableName
     * - 메모리 공간 내에서의 위치 계산
     */

    sTableDesc->mTableName = eldcGetTableNamePtr( sTableDesc );

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_TABLES,
                                   ELDT_Tables_ColumnOrder_TABLE_NAME );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( sTableDesc->mTableName,
                   sDataValue->mValue,
                   sDataValue->mLength );
        sTableDesc->mTableName[sDataValue->mLength] = '\0';
    }

    /*
     * mTableType
     */

    sTableDesc->mTableType = sTableType;

    /*
     * mIsSuppPK
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_TABLES,
                                   ELDT_Tables_ColumnOrder_IS_SET_SUPPLOG_PK );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( & sTableDesc->mIsSuppPK,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sTableDesc->mIsSuppPK) == sDataValue->mLength );
    }
    
    /*
     * mTableHandle
     */

    if ( sTableDesc->mPhysicalID == ELL_DICT_OBJECT_ID_NA )
    {
        sTableDesc->mTableHandle = NULL;
    }
    else
    {
        STL_TRY( smlGetRelationHandle( sTableDesc->mPhysicalID,
                                       & sTableDesc->mTableHandle,
                                       SML_ENV( aEnv ) )
                 == STL_SUCCESS );
        STL_ASSERT( sTableDesc->mTableHandle != NULL );
    }
    
    /*
     * mColumnCnt
     * mColumnDictHandle
     */

    sTableDesc->mColumnCnt = 0;
    sTableDesc->mColumnDictHandle = NULL;
                                         
    /**
     * Performance View 에 종속적인 descriptor 정보를 설정한다.
     */

    sViewDesc = (ellViewedTableDepDesc *) & sTableDesc->mTableTypeDesc.mView;
    
    sViewDesc->mViewColumnString = eldcGetViewColDefPtr( sTableDesc,
                                                         sTableNameLen );
    
    sViewDesc->mViewQueryString = eldcGetViewSelectPtr( sTableDesc,
                                                        sTableNameLen,
                                                        sViewColDefLen );

    stlStrcpy( sViewDesc->mViewColumnString, aViewColumnString );
    stlStrcpy( sViewDesc->mViewQueryString, aViewQueryString );
    
    /**
     * Output 설정
     */
    
    *aTableDesc = sTableDesc;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Table ID 를 위한 Hash Element 를 구성한다.
 * @param[out] aHashElement    Hash Element
 * @param[in]  aTransID        Transaction ID
 * @param[in]  aStmt           Statement
 * @param[in]  aTableDesc      Table Descriptor
 * @param[in]  aEnv            Environment
 * @remarks
 */ 
stlStatus eldcMakeHashElementTableID( eldcHashElement   ** aHashElement,
                                      smlTransId           aTransID,
                                      smlStatement       * aStmt,
                                      ellTableDesc       * aTableDesc,
                                      ellEnv             * aEnv )
{
    stlInt32 sHashDataSize = 0;

    stlUInt32              sHashValue = 0;
    eldcHashDataTableID  * sHashDataTableID = NULL;
    eldcHashElement      * sHashElement      = NULL;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aHashElement != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableDesc != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Hash Data 생성
     */

    sHashDataSize = eldcCalSizeHashDataTableID();
    
    /**
     * Shared Cache 메모리 관리자로부터 할당
     */
    
    STL_TRY( eldAllocCacheMem( (void **) & sHashDataTableID,
                               sHashDataSize,
                               aEnv )
             == STL_SUCCESS );

    stlMemset( sHashDataTableID, 0x00, sHashDataSize );
    
    sHashDataTableID->mKeyTableID = aTableDesc->mTableID;
    sHashDataTableID->mCheckNotNull = NULL;
    sHashDataTableID->mCheckClause = NULL;
    sHashDataTableID->mPrimaryKey = NULL;
    sHashDataTableID->mUniqueKey = NULL;
    sHashDataTableID->mForeignKey = NULL;
    sHashDataTableID->mChildForeignKey = NULL;
    sHashDataTableID->mUniqueIndex = NULL;
    sHashDataTableID->mNonUniqueIndex = NULL;

    /**
     * Hash Value 생성
     */
    
    sHashValue = eldcHashFuncTableID( & aTableDesc->mTableID );
    
    /**
     * Hash Element 생성
     */

    STL_TRY( eldcMakeHashElement( & sHashElement,
                                  aTransID,
                                  sHashDataTableID,
                                  sHashValue,
                                  aTableDesc,
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
 * @brief Table Name 을 위한 Hash Element 를 구성한다.
 * @param[out] aHashElement    Hash Element
 * @param[in]  aTransID        Transaction ID
 * @param[in]  aTableDesc      Table Descriptor
 * @param[in]  aHashElementID  Table ID로부터 구축된 Hash Element
 * @param[in]  aEnv            Environment 
 * @remarks
 */ 
stlStatus eldcMakeHashElementTableName( eldcHashElement   ** aHashElement,
                                        smlTransId           aTransID,
                                        ellTableDesc       * aTableDesc,
                                        eldcHashElement    * aHashElementID,
                                        ellEnv             * aEnv )
{
    stlInt32 sHashDataSize = 0;

    stlUInt32                sHashValue = 0;
    eldcHashDataTableName  * sHashDataTableName = NULL;
    eldcHashElement        * sHashElement        = NULL;

    eldcHashKeyTableName     sHashKey;
    stlMemset( & sHashKey, 0x00, STL_SIZEOF(eldcHashKeyTableName) );
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aHashElement != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableDesc != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Hash Data 생성
     */

    sHashDataSize = eldcCalSizeHashDataTableName( stlStrlen(aTableDesc->mTableName) );
    
    /**
     * Shared Cache 메모리 관리자로부터 할당
     */
    
    STL_TRY( eldAllocCacheMem( (void **) & sHashDataTableName,
                               sHashDataSize,
                               aEnv )
             == STL_SUCCESS );

    stlMemset( sHashDataTableName, 0x00, sHashDataSize );
    
    sHashDataTableName->mKeySchemaID = aTableDesc->mSchemaID;
    
    /* Table Name 을 복사할 위치 지정 */
    sHashDataTableName->mKeyTableName = eldcGetHashDataTableNamePtr( sHashDataTableName );
    
    stlStrcpy( sHashDataTableName->mKeyTableName,
               aTableDesc->mTableName );

    sHashDataTableName->mDataTableID = aTableDesc->mTableID;
    sHashDataTableName->mDataHashElement = aHashElementID;

    /**
     * Hash Value 생성
     */

    sHashKey.mSchemaID = sHashDataTableName->mKeySchemaID;
    sHashKey.mTableName = sHashDataTableName->mKeyTableName;
    sHashValue = eldcHashFuncTableName( & sHashKey );
    
    /**
     * Hash Element 생성
     */

    STL_TRY( eldcMakeHashElement( & sHashElement,
                                  aTransID,
                                  sHashDataTableName,
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
 * @brief Table 의 Column 개수를 얻는다.
 * @param[in]  aTransID      Transaction ID
 * @param[in]  aStmt         Statement
 * @param[in]  aTableID      Table ID
 * @param[out] aColumnCnt    Column 개수 
 * @param[in]  aEnv          Environment
 * @remarks
 */
stlStatus eldcGetColumnCount( smlTransId           aTransID,
                              smlStatement       * aStmt,
                              stlInt64             aTableID,
                              stlInt32           * aColumnCnt,
                              ellEnv             * aEnv )
{
    stlInt32            sColumnCnt = 0;
    
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

    smlRid              sRowRid;
    smlScn              sRowScn;
    smlRowBlock         sRowBlock;
    eldMemMark          sMemMark;

    smlFetchInfo        sFetchInfo;
    smlFetchRecordInfo  sFetchRecordInfo;
    
    stlInt32 sState = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableID > ELL_DICT_OBJECT_ID_NA,ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColumnCnt != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * DEFINITION_SCHEMA.COLUMNS 테이블의 table handle 을 획득
     */
    
    sTableHandle = gEldTablePhysicalHandle[ELDT_TABLE_ID_COLUMNS];

    /**
     * DEFINITION_SCHEMA.COLUMNS 테이블의 Value List와 Row Block 할당
     */

    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_COLUMNS,
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
                                            ELDT_TABLE_ID_COLUMNS,
                                            ELDT_Columns_ColumnOrder_TABLE_ID );
    
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
                                        aTableID,
                                        & sKeyCompList,
                                        & sIndexValueList,
                                        & sRangePred,
                                        & sRangeContext,
                                        aEnv )
                 == STL_SUCCESS );
        
        sIndexHandle = gEldNonCacheIndexPhyHandle4ColTableID;
    }
    else
    {
        /**
         * Key Range 생성 
         */

        STL_TRY( eldMakeOneEquiRange( aTransID,
                                      aStmt,
                                      ELDT_TABLE_ID_COLUMNS,
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
     * Column 갯수를 얻는다.
     */

    sColumnCnt = 0;
    
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
            sColumnCnt++;
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

    *aColumnCnt = sColumnCnt;
    
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
 * @brief Table Descriptor 에 column dictionary handle 정보를 채운다.
 * @param[in] aTransID         Transaction ID
 * @param[in] aStmt            Statement
 * @param[in] aTableDesc       Table Descriptor
 * @param[in] aEnv             Environment
 * @remarks
 */

stlStatus eldcSetColumnDictHandle( smlTransId           aTransID,
                                   smlStatement       * aStmt,
                                   ellTableDesc       * aTableDesc,
                                   ellEnv             * aEnv )
{
    stlBool             sExist = STL_FALSE;

    stlInt64            sColumnID = 0;
    stlInt32            sColumnIdx = 0;
    
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
    
    dtlDataValue  * sDataValue = NULL;

    smlFetchInfo        sFetchInfo;
    smlFetchRecordInfo  sFetchRecordInfo;
    eldMemMark          sMemMark;

    stlInt32            sColumnCount = 0;

    stlInt32  sState = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableDesc != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * DEFINITION_SCHEMA.COLUMNS 테이블의 table handle 을 획득
     */
    
    sTableHandle = gEldTablePhysicalHandle[ELDT_TABLE_ID_COLUMNS];

    /**
     * DEFINITION_SCHEMA.COLUMNS 테이블의 Value List와 Row Block 할당
     */

    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_COLUMNS,
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
                                            ELDT_TABLE_ID_COLUMNS,
                                            ELDT_Columns_ColumnOrder_TABLE_ID );
    
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
                                        aTableDesc->mTableID,
                                        & sKeyCompList,
                                        & sIndexValueList,
                                        & sRangePred,
                                        & sRangeContext,
                                        aEnv )
                 == STL_SUCCESS );
        
        sIndexHandle = gEldNonCacheIndexPhyHandle4ColTableID;
    }
    else
    {
        /**
         * Key Range 생성
         */

        STL_TRY( eldMakeOneEquiRange( aTransID,
                                      aStmt,
                                      ELDT_TABLE_ID_COLUMNS,
                                      sFilterColumn,
                                      & aTableDesc->mTableID,
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
     * Column Hash Element 를 table descriptor 에 추가한다.
     */

    sColumnCount = 0;
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
        else
        {
            sColumnCount++;
            
            /*
             * Column ID
             */

            sDataValue = eldFindDataValue( sTableValueList,
                                           ELDT_TABLE_ID_COLUMNS,
                                           ELDT_Columns_ColumnOrder_COLUMN_ID );
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

            /*
             * Ordinal Position
             */
            
            sDataValue = eldFindDataValue( sTableValueList,
                                           ELDT_TABLE_ID_COLUMNS,
                                           ELDT_Columns_ColumnOrder_PHYSICAL_ORDINAL_POSITION );
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
            
            STL_TRY( eldcGetColumnHandleByID( aTransID,
                                              SML_MAXIMUM_STABLE_SCN,
                                              sColumnID,
                                              & aTableDesc->mColumnDictHandle[sColumnIdx],
                                              & sExist,
                                              aEnv )
                     == STL_SUCCESS );

            STL_ASSERT( sExist == STL_TRUE );
            STL_ASSERT( sColumnIdx < aTableDesc->mColumnCnt );
        }
    }

    STL_DASSERT( sColumnCount == aTableDesc->mColumnCnt );
    
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
 * @brief Table ID(View ID)로부터 View 관련 정보를 획득한다.
 * @param[in]  aTransID      Transaction ID
 * @param[in]  aStmt         Statement
 * @param[in]  aViewID       Table ID (View ID)
 * @param[out] aColDefLen    Column List 구문의 길이
 * @param[out] aSelectLen    Select 구문의 길이
 * @param[in]  aEnv          Environment
 * @remarks
 */
stlStatus eldcGetViewStringLength( smlTransId      aTransID,
                                   smlStatement  * aStmt,
                                   stlInt64        aViewID,
                                   stlInt32      * aColDefLen,
                                   stlInt32      * aSelectLen,
                                   ellEnv        * aEnv )
{
    smlRid              sRowRid;
    smlScn              sRowScn;
    smlRowBlock         sRowBlock;
    
    stlBool             sBeginSelect = STL_FALSE;

    dtlDataValue      * sDataValue = NULL;
    
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

    smlFetchInfo        sFetchInfo;
    smlFetchRecordInfo  sFetchRecordInfo;
    eldMemMark          sMemMark;

    stlInt32 sState = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aViewID > ELL_DICT_OBJECT_ID_NA, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColDefLen != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSelectLen != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * DEFINITION_SCHEMA.VIEWS 테이블의 table handle 을 획득
     */
    
    sTableHandle = gEldTablePhysicalHandle[ELDT_TABLE_ID_VIEWS];

    /**
     * DEFINITION_SCHEMA.VIEWS 테이블의 Value List와 Row Block 할당
     */

    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_VIEWS,
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
     * WHERE TABLE_ID = aViewID
     */

    sFilterColumn = ellFindColumnValueList( sTableValueList,
                                            ELDT_TABLE_ID_VIEWS,
                                            ELDT_Views_ColumnOrder_TABLE_ID );
    
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
                                        aViewID,
                                        & sKeyCompList,
                                        & sIndexValueList,
                                        & sRangePred,
                                        & sRangeContext,
                                        aEnv )
                 == STL_SUCCESS );
        
        sIndexHandle = gEldNonCacheIndexPhyHandle4ViewedTableID;
    }
    else
    {
        /**
         * Key Range 생성
         */

        STL_TRY( eldMakeOneEquiRange( aTransID,
                                      aStmt,
                                      ELDT_TABLE_ID_VIEWS,
                                      sFilterColumn,
                                      & aViewID,
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
     * View 정보를 읽는다.
     */

    STL_TRY( eldFetchNext( sIterator,
                           & sFetchInfo, 
                           aEnv )
             == STL_SUCCESS );

    STL_ASSERT( sFetchInfo.mIsEndOfScan == STL_FALSE );

    /**
     * View Columns 과 View Columns String 의 길이 
     */
    
    sDataValue = eldFindDataValue( sTableValueList,
                                   ELDT_TABLE_ID_VIEWS,
                                   ELDT_Views_ColumnOrder_VIEW_COLUMNS );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        *aColDefLen   = (stlInt32)  sDataValue->mLength;
    }
    
    /**
     * View String 과 View String 의 길이 
     */
    
    sDataValue = eldFindDataValue( sTableValueList,
                                   ELDT_TABLE_ID_VIEWS,
                                   ELDT_Views_ColumnOrder_VIEW_DEFINITION );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        *aSelectLen    = (stlInt32)  sDataValue->mLength;
    }

    /**
     * 한 건만 존재해야 함
     */

    STL_TRY( eldFetchNext( sIterator,
                           & sFetchInfo, 
                           aEnv )
             == STL_SUCCESS );

    STL_ASSERT( sFetchInfo.mIsEndOfScan == STL_TRUE );

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
 * @brief Table ID(View ID)로부터 View Desc 의 String 관련 정보를 획득한다.
 * @param[in]  aTransID      Transaction ID
 * @param[in]  aStmt         Statement
 * @param[in]  aViewID       Table ID (View ID)
 * @param[in]  aViewColumns  View Column List 구문
 * @param[in]  aColDefLen    Column List 구문의 길이
 * @param[in]  aViewSelect   View 의 Select 구문
 * @param[in]  aSelectLen    Select 구문의 길이
 * @param[in]  aEnv          Environment
 * @remarks
 */
stlStatus eldcSetViewStringDesc( smlTransId      aTransID,
                                 smlStatement  * aStmt,
                                 stlInt64        aViewID,
                                 stlChar       * aViewColumns,
                                 stlInt32        aColDefLen,
                                 stlChar       * aViewSelect,
                                 stlInt32        aSelectLen,
                                 ellEnv        * aEnv )
{
    smlRid              sRowRid;
    smlScn              sRowScn;
    smlRowBlock         sRowBlock;
    
    stlBool             sBeginSelect = STL_FALSE;

    dtlDataValue      * sDataValue = NULL;
    
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

    smlFetchInfo        sFetchInfo;
    smlFetchRecordInfo  sFetchRecordInfo;
    eldMemMark          sMemMark;

    stlInt32  sState = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aViewID > ELL_DICT_OBJECT_ID_NA, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aViewColumns != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aViewSelect != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * DEFINITION_SCHEMA.VIEWS 테이블의 table handle 을 획득
     */
    
    sTableHandle = gEldTablePhysicalHandle[ELDT_TABLE_ID_VIEWS];

    /**
     * DEFINITION_SCHEMA.VIEWS 테이블의 Value List와 Row Block 할당
     */

    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_VIEWS,
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
     * WHERE TABLE_ID = aViewID
     */

    sFilterColumn = ellFindColumnValueList( sTableValueList,
                                            ELDT_TABLE_ID_VIEWS,
                                            ELDT_Views_ColumnOrder_TABLE_ID );
    
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
                                        aViewID,
                                        & sKeyCompList,
                                        & sIndexValueList,
                                        & sRangePred,
                                        & sRangeContext,
                                        aEnv )
                 == STL_SUCCESS );
        
        sIndexHandle = gEldNonCacheIndexPhyHandle4ViewedTableID;
    }
    else
    {
        /**
         * Key Range 생성
         */

        STL_TRY( eldMakeOneEquiRange( aTransID,
                                      aStmt,
                                      ELDT_TABLE_ID_VIEWS,
                                      sFilterColumn,
                                      & aViewID,
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
     * View 정보를 읽는다.
     */

    STL_TRY( eldFetchNext( sIterator,
                           & sFetchInfo, 
                           aEnv )
             == STL_SUCCESS );

    STL_ASSERT( sFetchInfo.mIsEndOfScan == STL_FALSE );

    /**
     * View Columns 정보 설정
     */
    
    sDataValue = eldFindDataValue( sTableValueList,
                                   ELDT_TABLE_ID_VIEWS,
                                   ELDT_Views_ColumnOrder_VIEW_COLUMNS );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        STL_DASSERT( sDataValue->mLength == aColDefLen );
        stlMemcpy( aViewColumns,
                   sDataValue->mValue,
                   sDataValue->mLength );
        aViewColumns[aColDefLen] = '\0';
    }
    
    /**
     * View String 정보 설정
     */
    
    sDataValue = eldFindDataValue( sTableValueList,
                                   ELDT_TABLE_ID_VIEWS,
                                   ELDT_Views_ColumnOrder_VIEW_DEFINITION );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        STL_DASSERT( sDataValue->mLength == aSelectLen );
        stlMemcpy( aViewSelect,
                   sDataValue->mValue,
                   sDataValue->mLength );
        aViewSelect[aSelectLen] = '\0';
    }

    /**
     * 한 건만 존재해야 함
     */

    STL_TRY( eldFetchNext( sIterator,
                           & sFetchInfo, 
                           aEnv )
             == STL_SUCCESS );

    STL_ASSERT( sFetchInfo.mIsEndOfScan == STL_TRUE );

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

/** @} eldcTableDesc */
