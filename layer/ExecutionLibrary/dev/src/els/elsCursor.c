/*******************************************************************************
 * elsCursor.c
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
 * @file elsCursor.c
 * @brief Execution Library Layer Cursor
 */


#include <ell.h>
#include <elsCursor.h>

/**
 * @addtogroup elsCursorDeclDesc
 * @{
 */

/**
 * @brief Cursor Key 로부터 Hash Bucket 획득 
 * @param[in] aHashElement Hash Element Data
 * @param[in] aBucketCount Bucket Count
 * @remarks
 */
stlUInt32  elsCursorKeyGetHashValue( void * aHashElement, stlUInt32 aBucketCount )
{
    stlUInt32   sHashValue = 0;
    
    ellCursorHashKey * sCursorKey = (ellCursorHashKey *) aHashElement;

    sHashValue = ellGetHashValueNameAndID( sCursorKey->mProcID,
                                           sCursorKey->mCursorName );
    
    return sHashValue % aBucketCount;
}

/**
 * @brief Hash Key 와 Declaration Cursor 가 동일한지 검사 
 * @param[in] aSearchKey    검색하려는  Hash Key 
 * @param[in] aBaseKey      비교 대상이 되는 Hash Key 
 * @remarks
 * Hash Value 가 동일할 경우 Key 를 검사 
 */
stlInt32 elsCursorKeyCompare( void * aSearchKey, void * aBaseKey )
{
    ellCursorHashKey  * sNewKey;
    ellCursorHashKey  * sOrgKey;
    
    /**
     * 입력된 Hash Key 와 Hash Element 의 Key 를 비교 
     */
    
    sNewKey  = (ellCursorHashKey *) aSearchKey;
    sOrgKey  = (ellCursorHashKey *) aBaseKey;
    

    if ( ( sNewKey->mProcID == sOrgKey->mProcID ) &&
         ( stlStrcmp( sNewKey->mCursorName, sOrgKey->mCursorName ) == 0 ) )
    {
        return 0;
    }
    else
    {
        return 1;
    }
}


/**
 * @brief Cursor Declaration Descriptor 를 초기화한다.
 * @param[in]  aDeclDesc    Cursor Declaration Descriptor
 * @remarks
 */
void elsInitCursorDeclDesc( ellCursorDeclDesc * aDeclDesc )
{
    STL_RING_INIT_DATALINK( aDeclDesc, STL_OFFSETOF(ellCursorDeclDesc, mLink) );

    aDeclDesc->mCursorKey.mCursorName = NULL;
    aDeclDesc->mCursorKey.mProcID = ELL_DICT_OBJECT_ID_NA;

    aDeclDesc->mOriginType = ELL_CURSOR_ORIGIN_NA;
    aDeclDesc->mCursorOrigin = NULL;

    aDeclDesc->mCursorProperty.mStandardType  = ELL_CURSOR_STANDARD_NA;
    
    aDeclDesc->mCursorProperty.mSensitivity   = ELL_CURSOR_SENSITIVITY_NA;
    aDeclDesc->mCursorProperty.mScrollability = ELL_CURSOR_SCROLLABILITY_NA;
    aDeclDesc->mCursorProperty.mHoldability   = ELL_CURSOR_HOLDABILITY_NA;
    aDeclDesc->mCursorProperty.mUpdatability  = ELL_CURSOR_UPDATABILITY_NA;
    aDeclDesc->mCursorProperty.mReturnability = ELL_CURSOR_RETURNABILITY_NA;
}


/**
 * @brief Cursor Declaration Descriptor 를 생성하고 Hash Pool 에 추가한다.
 * @param[in]  aCursorName     Cursor Name
 * @param[in]  aProcID         Procedure ID (Procedure가 아니라면 ELL_DICT_OBJECT_ID_NA)
 * @param[in]  aOriginType     Cursor Origin Type
 * @param[in]  aCursorOrigin   Cursor Origin Text (nullable)
 * @param[in]  aCursorProperty Cursor Property 
 * @param[out] aCursorDeclDesc 생성된 Cursor Declaration Descriptor
 * @param[in]  aEnv            Environment
 * @remarks
 */
stlStatus elsInsertCursorDeclDesc( stlChar                 * aCursorName,
                                   stlInt64                  aProcID,
                                   ellCursorOriginType       aOriginType,
                                   stlChar                 * aCursorOrigin,
                                   ellCursorProperty       * aCursorProperty,
                                   ellCursorDeclDesc      ** aCursorDeclDesc,
                                   ellEnv                  * aEnv )
{
    stlInt32   sStrLen = 0;
    
    ellSessObjectMgr  * sObjMgr = NULL;
    
    ellCursorDeclDesc * sDeclDesc = NULL;
    ellNamedCursorSlot * sCursorSlot = NULL;
    
    stlInt32 sSlotID = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aCursorName != NULL, ELL_ERROR_STACK(aEnv) );
    /* STL_PARAM_VALIDATE( (aOriginType > ELL_CURSOR_ORIGIN_NA) && */
    /*                     (aOriginType < ELL_CURSOR_ORIGIN_MAX), */
    /*                     ELL_ERROR_STACK(aEnv) ); */
    /* STL_PARAM_VALIDATE( aCursorProperty != NULL, ELL_ERROR_STACK(aEnv) ); */
    STL_PARAM_VALIDATE( aCursorDeclDesc != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 설정
     */

    sObjMgr = ELL_SESS_OBJ(aEnv);

    /**
     * Named Cursor Slot 확보
     */

    STL_TRY_THROW( sObjMgr->mUsedCursorCount < sObjMgr->mMaxCursorCount,
                   RAMP_ERR_LIMIT_NAMED_CURSOR_COUNT );

    for( sSlotID = sObjMgr->mNextCursorSlot; sSlotID < sObjMgr->mMaxCursorCount; sSlotID++ )
    {
        if( sObjMgr->mCursorSlot[sSlotID] == NULL )
        {
            break;
        }
        else
        {
            continue;
        }
    }

    if( sSlotID == sObjMgr->mMaxCursorCount )
    {
        for( sSlotID = 0; sSlotID < sObjMgr->mNextCursorSlot; sSlotID++ )
        {
            if( sObjMgr->mCursorSlot[sSlotID] == NULL )
            {
                break;
            }
            else
            {
                continue;
            }
        }
    }

    STL_ASSERT( sSlotID < sObjMgr->mMaxCursorCount );

    /***********************************************************************
     * Declaration Cursor 정보 설정
     ***********************************************************************/
    
    /**
     * Declaration Cursor 공간 확보
     */

    STL_TRY( knlAllocDynamicMem( & sObjMgr->mMemSessObj,
                                 STL_SIZEOF(ellCursorDeclDesc),
                                 (void **) & sDeclDesc,
                                 KNL_ENV(aEnv) )
             == STL_SUCCESS );

    elsInitCursorDeclDesc( sDeclDesc );

    /**
     * Cursor Key 설정
     */

    sStrLen = stlStrlen(aCursorName);
    STL_TRY( knlAllocDynamicMem( & sObjMgr->mMemSessObj,
                                 sStrLen + 1,
                                 (void **) & sDeclDesc->mCursorKey.mCursorName,
                                 KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemcpy( sDeclDesc->mCursorKey.mCursorName, aCursorName, sStrLen );
    sDeclDesc->mCursorKey.mCursorName[sStrLen] = '\0';
    
    sDeclDesc->mCursorKey.mProcID = aProcID;
    
    /**
     * Cursor Origin 정보 설정 
     */

    sDeclDesc->mOriginType = aOriginType;
    
    switch (aOriginType)
    {
        case ELL_CURSOR_ORIGIN_DBC_CURSOR:
            /**
             * ODBC 에서 커서 선언시에는 Origin 이 없을 수 있음.
             */
            STL_DASSERT( aCursorOrigin == NULL );
            break;
        case ELL_CURSOR_ORIGIN_STANDING_CURSOR:
            /**
             * Standing Cursor 의 Origin 은 SQL text 임.
             */
            STL_DASSERT( aCursorOrigin != NULL );
            break;
        case ELL_CURSOR_ORIGIN_DECLARED_DYNAMIC_CURSOR:
        case ELL_CURSOR_ORIGIN_EXTENDED_STATEMENT_CURSOR:
        case ELL_CURSOR_ORIGIN_RECEIVED_CURSOR:
            /**
             * Cursor Origin 이 SQL Text 가 아님 
             */
            STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );
            break;
            
        default:
            STL_ASSERT(0);
            break;
    }

    if ( aCursorOrigin != NULL )
    {
        sStrLen = stlStrlen( aCursorOrigin );
        STL_TRY( knlAllocDynamicMem( & sObjMgr->mMemSessObj,
                                     sStrLen + 1,
                                     (void **) & sDeclDesc->mCursorOrigin,
                                     KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        stlMemcpy( sDeclDesc->mCursorOrigin, aCursorOrigin, sStrLen );
        sDeclDesc->mCursorOrigin[sStrLen] = '\0';
    }
    else
    {
        sDeclDesc->mCursorOrigin = NULL;
    }

    /**
     * Cursor Property 정보 설정 
     */

    if ( aCursorProperty != NULL )
    {
        /**
         * SQL Cursor
         */
        stlMemcpy( & sDeclDesc->mCursorProperty,
                   aCursorProperty,
                   STL_SIZEOF(ellCursorProperty) );
    }
    else
    {
        /**
         * DBC Cursor
         */
        
        sDeclDesc->mCursorProperty.mStandardType  = ELL_CURSOR_STANDARD_NA;
    
        sDeclDesc->mCursorProperty.mSensitivity   = ELL_CURSOR_SENSITIVITY_NA;
        sDeclDesc->mCursorProperty.mScrollability = ELL_CURSOR_SCROLLABILITY_NA;
        sDeclDesc->mCursorProperty.mHoldability   = ELL_CURSOR_HOLDABILITY_NA;
        sDeclDesc->mCursorProperty.mUpdatability  = ELL_CURSOR_UPDATABILITY_NA;
        sDeclDesc->mCursorProperty.mReturnability = ELL_CURSOR_RETURNABILITY_NA;
    }

    /***********************************************************************
     * Cursor Slot 정보 설정
     ***********************************************************************/
    
    /**
     * Named Cursor Slot 에 정보 설정
     */

    STL_TRY( knlAllocDynamicMem( & sObjMgr->mMemCursorSlot,
                                 STL_SIZEOF(ellNamedCursorSlot),
                                 (void **) & sCursorSlot,
                                 KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sCursorSlot, 0x00, STL_SIZEOF(ellNamedCursorSlot) );
    
    sCursorSlot->mIsOpen   = STL_FALSE;
    
    stlStrncpy( sCursorSlot->mCursorName,
                sDeclDesc->mCursorKey.mCursorName,
                STL_MAX_SQL_IDENTIFIER_LENGTH );
    
    sCursorSlot->mOpenTime = 0;
    sCursorSlot->mViewScn  = SML_INFINITE_SCN;
    
    stlMemcpy( & sCursorSlot->mCursorProperty, & sDeclDesc->mCursorProperty, STL_SIZEOF(ellCursorProperty) );
    
    if ( aCursorOrigin == NULL )
    {
        sCursorSlot->mCursorQuery[0] = '\0';
    }
    else
    {
        stlStrncpy( sCursorSlot->mCursorQuery, aCursorOrigin, ELL_MAX_NAMED_CURSOR_QUERY_SIZE );
    }

    /***********************************************************************
     * Memory 할당 실패가 발생할 수 있으므로 Link 연결을 마지막에 한다.
     ***********************************************************************/

    /**
     * Cursor Declaration Pool 에 추가
     */

    STL_TRY( knlInsertStaticHash( sObjMgr->mHashDeclCursor,
                                  elsCursorKeyGetHashValue,
                                  sDeclDesc,
                                  KNL_ENV(aEnv) )
             == STL_SUCCESS );


    /**
     * Named Cursor Slot 에 추가 
     */
    
    sObjMgr->mCursorSlot[sSlotID] = sCursorSlot;
    
    sObjMgr->mNextCursorSlot = sSlotID + 1;
    sObjMgr->mUsedCursorCount++;
    
    sDeclDesc->mSlotID = sSlotID;
    
    /**
     * Latest Declaration Cursor 정보를 추가
     */

    sObjMgr->mLatestDeclCursor = sDeclDesc;

    /**
     * Output 설정
     */

    *aCursorDeclDesc = sDeclDesc;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_LIMIT_NAMED_CURSOR_COUNT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ELL_ERRCODE_LIMIT_ON_THE_NUMBER_OF_NAMED_CURSOR_EXCEEDED,
                      NULL,
                      ELL_ERROR_STACK(aEnv),
                      sObjMgr->mMaxCursorCount );
    }
    
    STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ELL_ERRCODE_NOT_IMPLEMENTED,
                      NULL,
                      ELL_ERROR_STACK(aEnv) );
    }
    
    STL_FINISH;

    if ( sDeclDesc != NULL )
    {
        if ( sDeclDesc->mCursorOrigin != NULL )
        {
            (void) knlFreeDynamicMem( & sObjMgr->mMemSessObj,
                                      sDeclDesc->mCursorOrigin,
                                      KNL_ENV(aEnv) );
            sDeclDesc->mCursorOrigin = NULL;
        }

        if ( sDeclDesc->mCursorKey.mCursorName != NULL )
        {
            (void) knlFreeDynamicMem( & sObjMgr->mMemSessObj,
                                      sDeclDesc->mCursorKey.mCursorName,
                                      KNL_ENV(aEnv) );

            sDeclDesc->mCursorKey.mCursorName = NULL;
        }

        (void) knlFreeDynamicMem( & sObjMgr->mMemSessObj,
                                  sDeclDesc,
                                  KNL_ENV(aEnv) );
        sDeclDesc = NULL;
    }

    if ( sCursorSlot != NULL )
    {
        (void) knlFreeDynamicMem( & sObjMgr->mMemCursorSlot,
                                  sCursorSlot,
                                  KNL_ENV(aEnv) );
        sCursorSlot = NULL;
    }
    
    return STL_FAILURE;
}


/**
 * @brief Cursor Declaration Descriptor 를 검색한다.
 * @param[in]  aCursorName      Cursor Name
 * @param[in]  aProcID          Procedure ID
 * @param[out] aCursorDeclDesc  Cursor Declaration Descriptor
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus elsFindCursorDeclDesc( stlChar            * aCursorName,
                                 stlInt64             aProcID,
                                 ellCursorDeclDesc ** aCursorDeclDesc,
                                 ellEnv             * aEnv )
{
    ellSessObjectMgr  * sObjMgr = NULL;

    ellCursorDeclDesc * sDeclDesc = NULL;
    ellCursorHashKey    sHashKey;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aCursorName != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCursorDeclDesc != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 설정
     */

    sObjMgr = ELL_SESS_OBJ(aEnv);
    
    /**
     * Hash Key 구성
     */
    
    sHashKey.mCursorName = aCursorName;
    sHashKey.mProcID     = aProcID;

    /**
     * 가장 최근에 접근한 Declaration Cursor 와 비교
     */
    
    if ( sObjMgr->mLatestDeclCursor != NULL )
    {
        if ( elsCursorKeyCompare( (void *) & sHashKey,
                                  (void *) & sObjMgr->mLatestDeclCursor->mCursorKey )
             == 0 )
        {
            sDeclDesc = sObjMgr->mLatestDeclCursor;
        }
        else
        {
            sDeclDesc = NULL;
        }
    }

    /**
     * 최근 Cursor 와 다르다면 Cursor Declaration Pool 에서 검색 
     */
    
    if ( sDeclDesc == NULL )
    {
        STL_TRY( knlFindStaticHash( sObjMgr->mHashDeclCursor,
                                    elsCursorKeyGetHashValue,
                                    elsCursorKeyCompare,
                                    (void *) & sHashKey,
                                    (void **) & sDeclDesc,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    else
    {
        /* lastest cursor 임 */
    }
    
    /**
     * Latest Declaration Cursor 정보를 변경 
     */

    if ( sDeclDesc != NULL )
    {
        sObjMgr->mLatestDeclCursor = sDeclDesc;
    }

    /**
     * Output 설정
     */

    *aCursorDeclDesc = sDeclDesc;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Cursor Declaration Desciptor 를 Pool 에서 제거
 * @param[in] aCursorDeclDesc Cursor Declaration Descriptor
 * @param[in] aEnv            Environment
 * @remarks
 */
stlStatus elsRemoveCursorDeclDesc( ellCursorDeclDesc * aCursorDeclDesc,
                                   ellEnv            * aEnv )
{
    ellSessObjectMgr  * sObjMgr = NULL;
    
    ellCursorDeclDesc  * sDeclDesc = NULL;
    ellNamedCursorSlot * sCursorSlot = NULL;

    stlInt32   sSlotID = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aCursorDeclDesc != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 설정
     */

    sObjMgr = ELL_SESS_OBJ(aEnv);
    sDeclDesc = aCursorDeclDesc;

    /**
     * Named Cursor Slot 에 정보 설정
     */

    sSlotID = sDeclDesc->mSlotID;

    sObjMgr->mUsedCursorCount--;
    
    sCursorSlot = sObjMgr->mCursorSlot[sSlotID];
    STL_ASSERT( sCursorSlot != NULL );
    
    sObjMgr->mCursorSlot[sSlotID] = NULL;

    STL_TRY( knlFreeDynamicMem( & sObjMgr->mMemCursorSlot,
                                sCursorSlot,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * Latest Declaration Cursor 정보를 제거
     */

    if ( sObjMgr->mLatestDeclCursor == sDeclDesc )
    {
        sObjMgr->mLatestDeclCursor = NULL;
    }
    else
    {
        /* 가장 최근에 검색된 Cursor 가 아님 */
    }

    /**
     * Cursor Declaration Pool 에서 제거 
     */

    STL_TRY( knlRemoveStaticHash( sObjMgr->mHashDeclCursor,
                                  elsCursorKeyGetHashValue,
                                  (void *) aCursorDeclDesc,
                                  KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * 메모리 공간 해제
     */

    if ( sDeclDesc->mCursorKey.mCursorName != NULL )
    {
        STL_TRY( knlFreeDynamicMem( & sObjMgr->mMemSessObj,
                                    sDeclDesc->mCursorKey.mCursorName,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    
    sDeclDesc->mCursorKey.mCursorName = NULL;
    
    if ( sDeclDesc->mCursorOrigin != NULL )
    {
        STL_TRY( knlFreeDynamicMem( & sObjMgr->mMemSessObj,
                                    sDeclDesc->mCursorOrigin,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    sDeclDesc->mCursorOrigin = NULL;

    STL_TRY( knlFreeDynamicMem( & sObjMgr->mMemSessObj,
                                sDeclDesc,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sDeclDesc = NULL;

    return STL_SUCCESS;

    STL_FINISH;

    /**
     * Memory 해제 및 연결 햬제가 실패해서는 안됨
     */

    STL_DASSERT(0);
    
    return STL_FAILURE;
}

/** @} elsCursorDeclDesc */










/**
 * @addtogroup elsCursorInstDesc
 * @{
 */



/**
 * @brief Cursor Instance Descriptor 를 초기화한다.
 * @param[in]  aInstDesc    Cursor Instance Descriptor
 * @remarks
 */
void elsInitCursorInstDesc( ellCursorInstDesc * aInstDesc )
{
    STL_RING_INIT_DATALINK( aInstDesc, STL_OFFSETOF(ellCursorInstDesc, mHashLink) );
    STL_RING_INIT_DATALINK( aInstDesc, STL_OFFSETOF(ellCursorInstDesc, mListLink) );
    
    aInstDesc->mCursorKey.mCursorName = NULL;
    aInstDesc->mCursorKey.mProcID = ELL_DICT_OBJECT_ID_NA;

    aInstDesc->mDeclDesc = NULL;
    aInstDesc->mIsOpen   = STL_FALSE;

    aInstDesc->mCursorStmt    = NULL;
    aInstDesc->mCursorDBCStmt = NULL;
    aInstDesc->mCursorSQLStmt = NULL;
}


/**
 * @brief Cursor Instance Descriptor 를 생성하고 Hash Pool 에 추가한다.
 * @param[in]  aDeclDesc           Cursor Declaration Descriptor
 * @param[in]  aCursorDBCStmtSize  Cursor DBC Statement 의 크기
 * @param[in]  aCursorSQLStmtSize  Cursor SQL Statement 의 크기
 * @param[out] aInstDesc           Cursor Instance Descriptor
 * @param[in]  aEnv                Environment
 * @remarks
 */
stlStatus elsInsertCursorInstDesc( ellCursorDeclDesc  * aDeclDesc,
                                   stlInt32             aCursorDBCStmtSize,
                                   stlInt32             aCursorSQLStmtSize,
                                   ellCursorInstDesc ** aInstDesc,
                                   ellEnv             * aEnv )
{
    ellSessObjectMgr  * sObjMgr = NULL;
    ellCursorInstDesc * sInstDesc = NULL;

    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDeclDesc != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInstDesc != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 설정
     */

    sObjMgr = ELL_SESS_OBJ(aEnv);

    /**
     * Instance Cursor 공간 확보
     */

    STL_TRY( knlAllocDynamicMem( & sObjMgr->mMemSessObj,
                                 STL_SIZEOF(ellCursorInstDesc),
                                 (void **) & sInstDesc,
                                 KNL_ENV(aEnv) )
             == STL_SUCCESS );

    elsInitCursorInstDesc( sInstDesc );

    /**
     * Instance Cursor 정보 설정 
     */

    sInstDesc->mCursorKey.mCursorName = aDeclDesc->mCursorKey.mCursorName;
    sInstDesc->mCursorKey.mProcID = aDeclDesc->mCursorKey.mProcID;
    sInstDesc->mDeclDesc = aDeclDesc;

    /**
     * Cursor DBC Statement 할당
     */

    if ( aCursorDBCStmtSize > 0 )
    {
        STL_TRY( knlAllocDynamicMem( & sObjMgr->mMemSessObj,
                                     aCursorDBCStmtSize,
                                     (void **) & sInstDesc->mCursorDBCStmt,
                                     KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    else
    {
        sInstDesc->mCursorDBCStmt = NULL;
    }

    /**
     * Cursor SQL Statement 할당
     */

    if ( aCursorSQLStmtSize > 0 )
    {
        STL_TRY( knlAllocDynamicMem( & sObjMgr->mMemSessObj,
                                     aCursorSQLStmtSize,
                                     (void **) & sInstDesc->mCursorSQLStmt,
                                     KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    else
    {
        sInstDesc->mCursorSQLStmt = NULL;
    }
    
    /**
     * Cursor Instance Pool 에 추가
     */

    STL_TRY( knlInsertStaticHash( sObjMgr->mHashInstCursor,
                                  elsCursorKeyGetHashValue,
                                  sInstDesc,
                                  KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Latest Instance Cursor 정보를 추가
     */

    sObjMgr->mLatestInstCursor = sInstDesc;

    /**
     * Output 설정
     */

    *aInstDesc = sInstDesc;
    
    return STL_SUCCESS;

    STL_FINISH;

    if ( sInstDesc != NULL )
    {
        if ( sInstDesc->mCursorDBCStmt != NULL )
        {
            (void) knlFreeDynamicMem( & sObjMgr->mMemSessObj,
                                      sInstDesc->mCursorDBCStmt,
                                      KNL_ENV(aEnv) );
            
            sInstDesc->mCursorDBCStmt = NULL;
        }
            
        if ( sInstDesc->mCursorSQLStmt != NULL )
        {
            (void) knlFreeDynamicMem( & sObjMgr->mMemSessObj,
                                      sInstDesc->mCursorSQLStmt,
                                      KNL_ENV(aEnv) );
            
            sInstDesc->mCursorSQLStmt = NULL;
        }
            
        (void) knlFreeDynamicMem( & sObjMgr->mMemSessObj,
                                  sInstDesc,
                                  KNL_ENV(aEnv) );

        sInstDesc = NULL;
    }
    
    return STL_FAILURE;
}


/**
 * @brief Cursor Instance Descriptor 를 검색한다.
 * @param[in]  aCursorName      Cursor Name
 * @param[in]  aProcID          Procedure ID
 * @param[out] aCursorInstDesc  Cursor Instance Descriptor
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus elsFindCursorInstDesc( stlChar            * aCursorName,
                                 stlInt64             aProcID,
                                 ellCursorInstDesc ** aCursorInstDesc,
                                 ellEnv             * aEnv )
{
    ellSessObjectMgr  * sObjMgr = NULL;

    ellCursorInstDesc * sInstDesc = NULL;
    ellCursorHashKey    sHashKey;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aCursorName != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCursorInstDesc != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 설정
     */

    sObjMgr = ELL_SESS_OBJ(aEnv);
    
    /**
     * Hash Key 구성
     */
    
    sHashKey.mCursorName = aCursorName;
    sHashKey.mProcID     = aProcID;

    /**
     * 가장 최근에 접근한 Instance Cursor 와 비교
     */
    
    if ( sObjMgr->mLatestInstCursor != NULL )
    {
        if ( elsCursorKeyCompare( (void *) & sHashKey,
                                  (void *) & sObjMgr->mLatestInstCursor->mCursorKey )
             == 0 )
        {
            sInstDesc = sObjMgr->mLatestInstCursor;
        }
        else
        {
            sInstDesc = NULL;
        }
    }

    /**
     * 최근 Cursor 와 다르다면 Cursor Instance Pool 에서 검색 
     */
    
    if ( sInstDesc == NULL )
    {
        STL_TRY( knlFindStaticHash( sObjMgr->mHashInstCursor,
                                    elsCursorKeyGetHashValue,
                                    elsCursorKeyCompare,
                                    (void *) & sHashKey,
                                    (void **) & sInstDesc,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    else
    {
        /* lastest cursor 임 */
    }
    
    /**
     * Latest Instance Cursor 정보를 변경 
     */

    if ( sInstDesc != NULL )
    {
        sObjMgr->mLatestInstCursor = sInstDesc;
    }

    /**
     * Output 설정
     */

    *aCursorInstDesc = sInstDesc;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Cursor Instance Desciptor 를 Pool 에서 제거
 * @param[in] aCursorInstDesc Cursor Instance Descriptor
 * @param[in] aEnv            Environment
 * @remarks
 */
stlStatus elsRemoveCursorInstDesc( ellCursorInstDesc * aCursorInstDesc,
                                   ellEnv            * aEnv )
{
    ellSessObjectMgr  * sObjMgr = NULL;
    ellCursorInstDesc * sInstDesc = NULL;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aCursorInstDesc != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 설정
     */

    sObjMgr = ELL_SESS_OBJ(aEnv);
    sInstDesc = aCursorInstDesc;
    
    /**
     * Latest Instance Cursor 정보를 제거
     */

    if ( sObjMgr->mLatestInstCursor == aCursorInstDesc )
    {
        sObjMgr->mLatestInstCursor = NULL;
    }
    else
    {
        /* 가장 최근에 검색된 Cursor 가 아님 */
    }

    /**
     * Cursor Instance Pool 에서 제거 
     */

    STL_TRY( knlRemoveStaticHash( sObjMgr->mHashInstCursor,
                                  elsCursorKeyGetHashValue,
                                  (void *) aCursorInstDesc,
                                  KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * 메모리 공간 해제
     */

    if ( ellCursorHasOrigin( sInstDesc ) == STL_TRUE )
    {
        if ( sInstDesc->mCursorSQLStmt != NULL )
        {
            STL_TRY( knlFreeDynamicMem( & sObjMgr->mMemSessObj,
                                        sInstDesc->mCursorSQLStmt,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );
        }
        sInstDesc->mCursorSQLStmt = NULL;

        if ( sInstDesc->mCursorDBCStmt != NULL )
        {
            STL_TRY( knlFreeDynamicMem( & sObjMgr->mMemSessObj,
                                        sInstDesc->mCursorDBCStmt,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );
        }
        sInstDesc->mCursorDBCStmt = NULL;
    }
        
    STL_TRY( knlFreeDynamicMem( & sObjMgr->mMemSessObj,
                                sInstDesc,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    sInstDesc = NULL;
    
    return STL_SUCCESS;

    STL_FINISH;

    if ( sInstDesc != NULL )
    {
        if ( ellCursorHasOrigin( sInstDesc ) == STL_TRUE )
        {
            if ( sInstDesc->mCursorSQLStmt != NULL )
            {
                (void) knlFreeDynamicMem( & sObjMgr->mMemSessObj,
                                          sInstDesc->mCursorSQLStmt,
                                          KNL_ENV(aEnv) );
            }
            sInstDesc->mCursorSQLStmt = NULL;
            
            if ( sInstDesc->mCursorDBCStmt != NULL )
            {
                (void) knlFreeDynamicMem( & sObjMgr->mMemSessObj,
                                          sInstDesc->mCursorDBCStmt,
                                          KNL_ENV(aEnv) );
            }
            sInstDesc->mCursorDBCStmt = NULL;
        }

        (void) knlFreeDynamicMem( & sObjMgr->mMemSessObj,
                                  sInstDesc,
                                  KNL_ENV(aEnv) );
        
        sInstDesc = NULL;
        
    }
        
    return STL_FAILURE;
}


/** @} elsCursorInstDesc */







