/*******************************************************************************
 * ellCursor.c
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
 * @file ellCursor.c
 * @brief Execution Library Layer Cursor
 */


#include <ell.h>
#include <elsCursor.h>
#include <ellCursor.h>

/**
 * @addtogroup ellCursorDeclDesc
 * @{
 */


/**
 * @brief Session Object Manager 의 Cursor Slot 을 초기화한다.
 * @param[in]  aSessEnv   Session Environment
 * @param[in]  aEnv       Environment
 */
stlStatus ellInitNamedCursorSlot( ellSessionEnv * aSessEnv, ellEnv * aEnv )
{
    stlBool            sAlloc      = STL_FALSE;
    
    /**
     * Named Cursor Slot 할당
     */

    /* session 초기화시 호출되므로 Session Environemnt 로부터 property 값을 얻어옴. */
    aSessEnv->mSessObjMgr.mMaxCursorCount =
        knlGetPropertyBigIntValue4SessEnvById( KNL_PROPERTY_MAXIMUM_NAMED_CURSOR_COUNT, (knlSessionEnv*) aSessEnv );
    
    STL_TRY( knlAllocDynamicMem( & aSessEnv->mSessObjMgr.mMemCursorSlot,
                                 STL_SIZEOF( ellNamedCursorSlot *) * aSessEnv->mSessObjMgr.mMaxCursorCount,
                                 (void**) & aSessEnv->mSessObjMgr.mCursorSlot,
                                 KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    sAlloc = STL_TRUE;
    
    stlMemset( aSessEnv->mSessObjMgr.mCursorSlot,
               0x00,
               STL_SIZEOF( ellNamedCursorSlot *) * aSessEnv->mSessObjMgr.mMaxCursorCount );

    aSessEnv->mSessObjMgr.mUsedCursorCount = 0;
    aSessEnv->mSessObjMgr.mNextCursorSlot = 0;
    
    return STL_SUCCESS;

    STL_FINISH;

    if ( sAlloc == STL_TRUE )
    {
        (void) knlFreeDynamicMem( & aSessEnv->mSessObjMgr.mMemCursorSlot,
                                  aSessEnv->mSessObjMgr.mCursorSlot,
                                  KNL_ENV(aEnv) );
    }

    aSessEnv->mSessObjMgr.mMaxCursorCount = 0;
    aSessEnv->mSessObjMgr.mUsedCursorCount = 0;
    aSessEnv->mSessObjMgr.mNextCursorSlot = 0;
    aSessEnv->mSessObjMgr.mCursorSlot = NULL;
    
    return STL_FAILURE;
}

/**
 * @brief Session Object Manager 의 Cursor Slot 을 종료한다.
 * @param[in]  aSessEnv   Session Environment
 * @param[in]  aEnv       Environment
 */
stlStatus ellFiniNamedCursorSlot( ellSessionEnv * aSessEnv, ellEnv * aEnv )
{
    STL_TRY( knlFreeDynamicMem( & aSessEnv->mSessObjMgr.mMemCursorSlot,
                                aSessEnv->mSessObjMgr.mCursorSlot,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    aSessEnv->mSessObjMgr.mMaxCursorCount = 0;
    aSessEnv->mSessObjMgr.mUsedCursorCount = 0;
    aSessEnv->mSessObjMgr.mNextCursorSlot = 0;
    aSessEnv->mSessObjMgr.mCursorSlot = NULL;

    return STL_SUCCESS;
    
    STL_FINISH;

    STL_DASSERT(0);
    
    return STL_FAILURE;
}


/**
 * @brief SQL Standing Cursor Declaration Descriptor 를 생성하고 Hash Pool 에 추가한다.
 * @param[in]  aCursorName         Cursor Name
 * @param[in]  aProcID             Procedure ID (Procedure가 아니라면 ELL_DICT_OBJECT_ID_NA)
 * @param[in]  aCursorOrigin       Cursor Origin Text
 * @param[in]  aCursorProperty     Cursor Property
 * @param[out] aCursorDeclDesc     생성된 Cursor Declaration Descriptor
 * @param[in]  aEnv                Environment
 * @remarks
 */
stlStatus ellInsertStandingCursorDeclDesc( stlChar                 * aCursorName,
                                           stlInt64                  aProcID,
                                           stlChar                 * aCursorOrigin,
                                           ellCursorProperty       * aCursorProperty,
                                           ellCursorDeclDesc      ** aCursorDeclDesc,
                                           ellEnv                  * aEnv )
{
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aCursorName != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCursorOrigin != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCursorProperty != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCursorDeclDesc != NULL, ELL_ERROR_STACK(aEnv) );
                        
    /**
     * Standing Cursor 를 추가
     */
    
    STL_TRY( elsInsertCursorDeclDesc( aCursorName,
                                      aProcID,
                                      ELL_CURSOR_ORIGIN_STANDING_CURSOR,
                                      aCursorOrigin,
                                      aCursorProperty,
                                      aCursorDeclDesc,
                                      aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief ODBC/JDBC Cursor Declaration Descriptor 를 생성하고 Hash Pool 에 추가한다.
 * @param[in]  aCursorName         Cursor Name
 * @param[out] aCursorDeclDesc     생성된 Cursor Declaration Descriptor
 * @param[in]  aEnv                Environment
 * @remarks
 */
stlStatus ellInsertDBCCursorDeclDesc( stlChar                 * aCursorName,
                                      ellCursorDeclDesc      ** aCursorDeclDesc,
                                      ellEnv                  * aEnv )
{
    return elsInsertCursorDeclDesc( aCursorName,
                                    ELL_DICT_OBJECT_ID_NA,
                                    ELL_CURSOR_ORIGIN_DBC_CURSOR,
                                    NULL, /* aCursorOrigin */
                                    NULL, /* aCursorProperty */
                                    aCursorDeclDesc,
                                    aEnv );
}

/**
 * @brief Cursor Declaration Descriptor 를 검색한다.
 * @param[in]  aCursorName      Cursor Name
 * @param[in]  aProcID          Procedure ID
 * @param[out] aCursorDeclDesc  Cursor Declaration Descriptor
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus ellFindCursorDeclDesc( stlChar            * aCursorName,
                                 stlInt64             aProcID,
                                 ellCursorDeclDesc ** aCursorDeclDesc,
                                 ellEnv             * aEnv )
{
    return elsFindCursorDeclDesc( aCursorName,
                                  aProcID,
                                  aCursorDeclDesc,
                                  aEnv );
}


/**
 * @brief Cursor Declaration Desciptor 를 Pool 에서 제거
 * @param[in] aCursorDeclDesc Cursor Declaration Descriptor
 * @param[in] aEnv            Environment
 * @remarks
 */
stlStatus ellRemoveCursorDeclDesc( ellCursorDeclDesc * aCursorDeclDesc,
                                   ellEnv            * aEnv )
{
    return elsRemoveCursorDeclDesc( aCursorDeclDesc,
                                    aEnv );
}



/** @} ellCursorDeclDesc */




/**
 * @addtogroup ellCursorInstDesc
 * @{
 */


/**
 * @brief Cursor Instance Descriptor 를 생성하고 Hash Pool 에 추가한다.
 * @param[in]  aDeclDesc           Cursor Declaration Descriptor
 * @param[in]  aCursorDBCStmtSize  Cursor DBC Statement 의 크기
 * @param[in]  aCursorSQLStmtSize  Cursor SQL Statement 의 크기
 * @param[out] aInstDesc           Cursor Instance Descriptor
 * @param[in]  aEnv                Environment
 * @remarks
 */
stlStatus ellInsertCursorInstDesc( ellCursorDeclDesc  * aDeclDesc,
                                   stlInt32             aCursorDBCStmtSize,
                                   stlInt32             aCursorSQLStmtSize,
                                   ellCursorInstDesc ** aInstDesc,
                                   ellEnv             * aEnv )
{
    return elsInsertCursorInstDesc( aDeclDesc,
                                    aCursorDBCStmtSize,
                                    aCursorSQLStmtSize,
                                    aInstDesc,
                                    aEnv );
}

/**
 * @brief Cursor Instance Descriptor 를 검색한다.
 * @param[in]  aCursorName      Cursor Name
 * @param[in]  aProcID          Procedure ID
 * @param[out] aCursorInstDesc  Cursor Instance Descriptor
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus ellFindCursorInstDesc( stlChar            * aCursorName,
                                 stlInt64             aProcID,
                                 ellCursorInstDesc ** aCursorInstDesc,
                                 ellEnv             * aEnv )
{
    return elsFindCursorInstDesc( aCursorName,
                                  aProcID,
                                  aCursorInstDesc,
                                  aEnv );
}

/**
 * @brief Cursor Instance Desciptor 를 Pool 에서 제거
 * @param[in] aCursorInstDesc Cursor Instance Descriptor
 * @param[in] aEnv            Environment
 * @remarks
 */
stlStatus ellRemoveCursorInstDesc( ellCursorInstDesc * aCursorInstDesc,
                                   ellEnv            * aEnv )
{
    return elsRemoveCursorInstDesc( aCursorInstDesc,
                                    aEnv );
}

/**
 * @brief Open Cursor List 를 획득
 * @param[in] aEnv            Environment
 * @return stlRingEntry *
 * @remarks
 */
stlRingHead * ellGetCursorOpenList( ellEnv * aEnv )
{
    return & (ELL_SESS_OBJ(aEnv)->mOpenCursorList);
}


/**
 * @brief 첫번째 SQL-Cursor 를 획득
 * @param[out]  aFirstBucketSeq   Bucket Sequence
 * @param[out]  aCursorInstDesc   Cursor Descriptor
 * @param[in]   aEnv              Environment
 * @remarks
 */
stlStatus ellGetFirstSQLCursor( stlUInt32           * aFirstBucketSeq,
                                ellCursorInstDesc  ** aCursorInstDesc,
                                ellEnv              * aEnv )
{
    ellSessObjectMgr  * sObjMgr = NULL;
    
    stlUInt32           sBucketSeq = 0;
    ellCursorInstDesc * sInstDesc  = NULL;

    /**
     * Parameter Validation
     */

    STL_ASSERT( aCursorInstDesc != NULL );
    
    /**
     * 기본 정보 설정
     */

    sObjMgr = ELL_SESS_OBJ(aEnv);

    /**
     * SQL Cursor 획득
     */

    /* 초기화 과정의 Session 인 경우, Hash 가 존재하지 않는다. */
    STL_TRY_THROW( sObjMgr->mHashInstCursor != NULL, RAMP_FINISH );
    
    STL_TRY( knlGetFirstStaticHashItem( sObjMgr->mHashInstCursor,
                                        & sBucketSeq,
                                        (void **) & sInstDesc,
                                        KNL_ENV(aEnv) )
             == STL_SUCCESS );

    while ( sInstDesc != NULL )
    {
        if ( sInstDesc->mDeclDesc->mOriginType == ELL_CURSOR_ORIGIN_DBC_CURSOR )
        {
            /**
             * ODBC/JDBC 로부터 연결된 Cursor 임
             */
            
            STL_TRY( knlGetNextStaticHashItem( sObjMgr->mHashInstCursor,
                                               & sBucketSeq,
                                               (void **) & sInstDesc,
                                               KNL_ENV(aEnv) )
                     == STL_SUCCESS );
            
        }
        else
        {
            /**
             * SQL-Cursor 임
             */
            
            break;
        }
    }

    /**
     * Ouput 설정
     */

    STL_RAMP( RAMP_FINISH );
    
    *aCursorInstDesc = sInstDesc;
    *aFirstBucketSeq = sBucketSeq;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief 다음  SQL-Cursor 를 획득
 * @param[in, out]  aBucketSeq        Bucket Sequence
 * @param[out]      aCursorInstDesc   Cursor Descriptor
 * @param[in]       aEnv              Environment
 * @remarks
 */
stlStatus ellGetNextSQLCursor( stlUInt32           * aBucketSeq,
                               ellCursorInstDesc  ** aCursorInstDesc,
                               ellEnv              * aEnv )
{
    ellSessObjectMgr  * sObjMgr = NULL;
    
    stlUInt32           sBucketSeq = 0;
    ellCursorInstDesc * sInstDesc  = NULL;

    /**
     * Parameter Validation
     */

    STL_ASSERT( aBucketSeq != NULL );
    STL_ASSERT( aCursorInstDesc != NULL );
    
    /**
     * 기본 정보 설정
     */

    sObjMgr = ELL_SESS_OBJ(aEnv);
    
    sBucketSeq = *aBucketSeq;
    sInstDesc  = *aCursorInstDesc;
    
    /**
     * SQL Cursor 획득
     */

    STL_TRY( knlGetNextStaticHashItem( sObjMgr->mHashInstCursor,
                                       & sBucketSeq,
                                       (void **) & sInstDesc,
                                       KNL_ENV(aEnv) )
             == STL_SUCCESS );

    while ( sInstDesc != NULL )
    {
        if ( sInstDesc->mDeclDesc->mOriginType == ELL_CURSOR_ORIGIN_DBC_CURSOR )
        {
            /**
             * ODBC/JDBC 로부터 연결된 Cursor 임
             */
            
            STL_TRY( knlGetNextStaticHashItem( sObjMgr->mHashInstCursor,
                                               & sBucketSeq,
                                               (void **) & sInstDesc,
                                               KNL_ENV(aEnv) )
                     == STL_SUCCESS );
            
        }
        else
        {
            /**
             * SQL-Cursor 임
             */
            
            break;
        }
    }

    /**
     * Ouput 설정
     */

    *aCursorInstDesc = sInstDesc;
    *aBucketSeq = sBucketSeq;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Cursor 를 OPEN 상태로 설정
 * @param[in] aCursorInstDesc Cursor Instance Descriptor
 * @param[in] aCursorStmt     Cursor 의 Statement
 * @param[in] aCursorDBCStmt  Cursor 의 DBC Statement
 * @param[in] aCursorSQLStmt  Cursor 의 SQL Statement
 * @param[in] aResultProperty Result Set 의 Cursor Property
 * @param[in] aCursorQuery    Cursor Query 
 * @param[in] aEnv            Environment
 */
void ellCursorSetOpen( ellCursorInstDesc      * aCursorInstDesc,
                       smlStatement           * aCursorStmt,
                       void                   * aCursorDBCStmt,
                       void                   * aCursorSQLStmt,
                       ellCursorProperty      * aResultProperty,
                       stlChar                * aCursorQuery,
                       ellEnv                 * aEnv )
{
    ellSessObjectMgr  * sObjMgr = NULL;
    
    ellNamedCursorSlot * sCursorSlot = NULL;

    /**
     * Parameter Validation
     */

    STL_DASSERT( aCursorInstDesc != NULL );
    STL_DASSERT( aCursorStmt != NULL );
    STL_DASSERT( aCursorDBCStmt != NULL );
    STL_DASSERT( aCursorSQLStmt != NULL );
    STL_DASSERT( aResultProperty != NULL );
    STL_DASSERT( aCursorQuery != NULL );
    
    /**
     * 기본 정보 설정
     */

    sObjMgr = ELL_SESS_OBJ(aEnv);

    /**
     * OPEN 상태
     */

    aCursorInstDesc->mIsOpen        = STL_TRUE;
    aCursorInstDesc->mCursorStmt    = aCursorStmt;

    if ( ellCursorHasOrigin( aCursorInstDesc ) == STL_TRUE )    
    {
        STL_DASSERT( aCursorInstDesc->mCursorDBCStmt == aCursorDBCStmt );
        STL_DASSERT( aCursorInstDesc->mCursorSQLStmt == aCursorSQLStmt );
    }
    else
    {
        /**
         * Origin 이 없는 DBC Cursor 인 경우
         */
        
        aCursorInstDesc->mCursorDBCStmt = aCursorDBCStmt;
        aCursorInstDesc->mCursorSQLStmt = aCursorSQLStmt;
    }
    
    /**
     * Open Cursor List 에 추가 
     */

    STL_RING_ADD_LAST( & sObjMgr->mOpenCursorList, aCursorInstDesc );

    /**
     * Named Cursor Slot 에 정보 설정
     */

    sCursorSlot = sObjMgr->mCursorSlot[aCursorInstDesc->mDeclDesc->mSlotID];
    
    sCursorSlot->mIsOpen   = STL_TRUE;
    sCursorSlot->mOpenTime = knlGetSystemTime();
    sCursorSlot->mViewScn  = aCursorStmt->mScn;

    stlMemcpy( & sCursorSlot->mCursorProperty,
               aResultProperty,
               STL_SIZEOF(ellCursorProperty) );

    stlStrncpy( sCursorSlot->mCursorQuery, aCursorQuery, ELL_MAX_NAMED_CURSOR_QUERY_SIZE );
}

/**
 * @brief Cursor 를 CLOSED 상태로 설정
 * @param[in] aCursorInstDesc Cursor Instance Descriptor
 * @param[in] aEnv            Environment
 */
void ellCursorSetClosed( ellCursorInstDesc * aCursorInstDesc,
                         ellEnv            * aEnv )
{
    ellSessObjectMgr  * sObjMgr = NULL;

    ellNamedCursorSlot * sCursorSlot = NULL;
    
    /**
     * Parameter Validation
     */

    STL_ASSERT( aCursorInstDesc != NULL );
    
    /**
     * 기본 정보 설정
     */

    sObjMgr = ELL_SESS_OBJ(aEnv);

    /**
     * CLOSE 상태
     */

    aCursorInstDesc->mIsOpen = STL_FALSE;

    aCursorInstDesc->mCursorStmt    = NULL;
    if ( ellCursorHasOrigin( aCursorInstDesc ) == STL_TRUE )    
    {
        /* nothing to do */
    }
    else
    {
        /**
         * Origin 이 없는 DBC Cursor 인 경우
         */
        
        aCursorInstDesc->mCursorDBCStmt = NULL;
        aCursorInstDesc->mCursorSQLStmt = NULL;
    }
    
    /**
     * Open Cursor List 에서 제거
     */

    STL_RING_UNLINK( & sObjMgr->mOpenCursorList, aCursorInstDesc );

    /**
     * Named Cursor Slot 에 정보 설정
     */

    sCursorSlot = sObjMgr->mCursorSlot[aCursorInstDesc->mDeclDesc->mSlotID];
    
    sCursorSlot->mIsOpen   = STL_FALSE;
    sCursorSlot->mOpenTime = 0;
    sCursorSlot->mViewScn  = SML_INFINITE_SCN;

    stlMemcpy( & sCursorSlot->mCursorProperty,
               & aCursorInstDesc->mDeclDesc->mCursorProperty,
               STL_SIZEOF(ellCursorProperty) );
    
    if ( aCursorInstDesc->mDeclDesc->mCursorOrigin == NULL )
    {
        /**
         * ODBC/JDBC named cursor 인 경우
         */
        
        sCursorSlot->mCursorQuery[0] = '\0';
    }
    else
    {
        /* nothing to do */
    }
}


/**
 * @brief Cursor 의 OPEN 여부
 * @param[in] aCursorInstDesc Cursor Instance Descriptor
 * @return stlBool
 * @remarks
 */ 
stlBool ellCursorIsOpen( ellCursorInstDesc * aCursorInstDesc )
{
    return aCursorInstDesc->mIsOpen;
}

/**
 * @brief Cursor 가 Cursor Origin 을 가지고 있는지의 여부 
 * @param[in] aCursorInstDesc Cursor Instance Descriptor
 * @return stlBool
 * @remarks
 * DECLARE 구문으로 선언한 커서의 경우 Cursor Origin 을 갖는다.
 * ODBC 등의 SQLSetCursorName() 등으로 선언한 경우 Cursor Origin 이 없다.
 */ 
stlBool ellCursorHasOrigin( ellCursorInstDesc * aCursorInstDesc )
{
    return ( aCursorInstDesc->mDeclDesc->mCursorOrigin == NULL ) ? STL_FALSE : STL_TRUE;
}

/** @} ellCursorInstDesc */




