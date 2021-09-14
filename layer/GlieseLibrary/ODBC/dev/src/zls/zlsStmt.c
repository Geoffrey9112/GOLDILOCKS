/*******************************************************************************
 * zlsStmt.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: zlsStmt.c 12841 2014-06-13 08:06:28Z htkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#include <cml.h>
#include <goldilocks.h>
#include <zlDef.h>
#include <zle.h>
#include <zld.h>
#include <zlr.h>
#include <zlvCharacterset.h>

#include <zlas.h>
#include <zlcs.h>

/**
 * @file zlsStmt.c
 * @brief ODBC API Internal Statement Routines.
 */

/**
 * @addtogroup zlsStmt
 * @{
 */

#if defined( ODBC_ALL )
stlStatus (*gZlsAllocFunc[ZLC_PROTOCOL_TYPE_MAX]) ( zlcDbc        * aDbc,
                                                    zlsStmt       * aStmt,
                                                    stlErrorStack * aErrorStack ) =
{
    zlasAlloc,
    zlcsAlloc
};

stlStatus (*gZlsFreeFunc[ZLC_PROTOCOL_TYPE_MAX]) ( zlcDbc            * aDbc,
                                                   zlsStmt           * aStmt,
                                                   zllFreeStmtOption   aOption,
                                                   stlBool             aIsCleanup,
                                                   stlErrorStack     * aErrorStack ) =
{
    zlasFree,
    zlcsFree
};

stlStatus (*gZlsGetAttr[ZLC_PROTOCOL_TYPE_MAX]) ( zlcDbc            * aDbc,
                                                  zlsStmt           * aStmt,
                                                  zllStatementAttr    aAttr,
                                                  void              * aValue,
                                                  stlInt32            aBufferLen,
                                                  stlInt32          * aStrLen,
                                                  stlErrorStack     * aErrorStack ) =
{
    zlasGetAttr,
    zlcsGetAttr
};

stlStatus (*gZlsSetAttr[ZLC_PROTOCOL_TYPE_MAX]) ( zlcDbc           * aDbc,
                                                  zlsStmt          * aStmt,
                                                  zllStatementAttr   aAttr,
                                                  stlInt64           aIntValue,
                                                  stlChar          * aStrValue,
                                                  stlInt32           aStrLength,
                                                  stlErrorStack *    aErrorStack ) =
{
    zlasSetAttr,
    zlcsSetAttr
};
#endif

stlStatus zlsAlloc( zlcDbc         * aDbc,
                    void          ** aStmt,
                    stlErrorStack  * aErrorStack )
{
    zlsStmt   * sStmt  = NULL;
    stlInt32    sState = 0;
    zldDesc   * sArd   = NULL;
    zldDesc   * sApd   = NULL;
    zldDesc   * sIrd   = NULL;
    zldDesc   * sIpd   = NULL;

    STL_TRY( stlAllocHeap( (void**)&sStmt,
                           STL_SIZEOF( zlsStmt ),
                           aErrorStack ) == STL_SUCCESS );
    sState = 1;

    stlMemset( sStmt, 0x00, STL_SIZEOF( zlsStmt ) );

    STL_TRY( zldDiagInit( SQL_HANDLE_STMT, (void*)sStmt, aErrorStack ) == STL_SUCCESS );
    sState = 2;

    sStmt->mParentDbc            = aDbc;
    sStmt->mStmtId               = ZLS_INVALID_STATEMENT_ID;
    sStmt->mType                 = SQL_HANDLE_STMT;
    sStmt->mReadBlockCount4Imp   = 0;
    sStmt->mExplainPlanType      = SQL_EXPLAIN_PLAN_OFF;
    sStmt->mAtomicExecution      = SQL_ATOMIC_EXECUTION_OFF;
    sStmt->mParamChanged         = STL_FALSE;
    sStmt->mNeedResultDescriptor = STL_TRUE;
    sStmt->mTransition           = ZLS_TRANSITION_STATE_S1;
    sStmt->mMetadataId           = aDbc->mMetadataId;

    sStmt->mResult.mRowCount = ZLR_ROWCOUNT_UNKNOWN;

    sStmt->mStringLengthUnit = aDbc->mStmtStringLengthUnit;
    
    sStmt->mParamCount    = ZLS_PARAM_COUNT_UNKNOWN;
    sStmt->mOutParamCount = ZLS_PARAM_COUNT_UNKNOWN;
    sStmt->mHasParamIN    = STL_FALSE;
    sStmt->mHasParamOUT   = STL_FALSE;

    sStmt->mCurrentPutDataApdRec   = NULL;
    sStmt->mCurrentPutDataIpdRec   = NULL;
    sStmt->mCurrentPutDataIdx      = 0;
    sStmt->mCurrentPutDataArrayIdx = 0;
    sStmt->mNeedDataTransition     = ZLS_TRANSITION_STATE_S0;

    sStmt->mRowsetSize           = 1;
    sStmt->mRetrieveData         = STL_TRUE;
    sStmt->mPreFetchSize         = aDbc->mStmtPreFetchSize;
    sStmt->mUsedRowsetSize       = 0;
    sStmt->mMaxRows              = aDbc->mStmtAttrMaxRows;
    sStmt->mSetCursorName        = STL_FALSE;
    sStmt->mCursorConcurrency    = ZLS_CONCURRENCY_NA;
    sStmt->mCursorSensitivity    = SQL_INSENSITIVE;    /**< ODBC 표준은 SQL_UNSPECIFIED */
    sStmt->mCursorScrollable     = SQL_NONSCROLLABLE;
    sStmt->mCursorType           = SQL_CURSOR_FORWARD_ONLY;
    sStmt->mCursorHoldable       = ZLS_HOLDABILITY_NA;
    
    sStmt->mCursorConcurrencyOrg = sStmt->mCursorConcurrency;
    sStmt->mCursorSensitivityOrg = sStmt->mCursorSensitivity;
    sStmt->mCursorHoldableOrg    = sStmt->mCursorHoldable;

    sStmt->mPreparedCursorConcurrency = sStmt->mCursorConcurrency;
    sStmt->mPreparedCursorSensitivity = sStmt->mCursorSensitivity;
    sStmt->mPreparedCursorHoldable    = sStmt->mCursorHoldable;

    /*
     * ARD
     */

    sStmt->mArd = &sStmt->mImpArd;
    
    sArd = sStmt->mArd;
    stlMemset( sArd, 0x00, STL_SIZEOF( zldDesc ) );

    STL_TRY( zldDiagInit( SQL_HANDLE_DESC, (void*)sArd, aErrorStack ) == STL_SUCCESS );
    sState = 3;

    STL_RING_INIT_HEADLINK( &sArd->mDescRecRing,
                            STL_OFFSETOF( zldDescElement, mLink ) );

    sArd->mType             = SQL_HANDLE_DESC;
    sArd->mDescType         = ZLD_DESC_TYPE_ARD;
    sArd->mTransition       = ZLD_TRANSITION_STATE_D1i;
    sArd->mStmt             = sStmt;
    sArd->mAllocType        = SQL_DESC_ALLOC_AUTO;
    sArd->mCount            = 0;
    sArd->mBindType         = SQL_BIND_BY_COLUMN;
    sArd->mArraySize        = 1;
    sArd->mPutDataArraySize = 0;
    sArd->mArrayStatusPtr   = NULL;
    sArd->mBindOffsetPtr    = NULL;
    
    /*
     * APD
     */

    sStmt->mApd = &sStmt->mImpApd;
    
    sApd = sStmt->mApd;
    stlMemset( sApd, 0x00, STL_SIZEOF( zldDesc ) );

    STL_TRY( zldDiagInit( SQL_HANDLE_DESC, (void*)sApd, aErrorStack ) == STL_SUCCESS );
    sState = 4;

    STL_RING_INIT_HEADLINK( &sApd->mDescRecRing,
                            STL_OFFSETOF( zldDescElement, mLink ) );

    sApd->mType             = SQL_HANDLE_DESC;
    sApd->mDescType         = ZLD_DESC_TYPE_APD;
    sApd->mTransition       = ZLD_TRANSITION_STATE_D1i;
    sApd->mStmt             = sStmt;
    sApd->mAllocType        = SQL_DESC_ALLOC_AUTO;
    sApd->mCount            = 0;
    sApd->mBindType         = SQL_BIND_BY_COLUMN;
    sApd->mArraySize        = 1;
    sApd->mPutDataArraySize = 0;
    sApd->mArrayStatusPtr   = NULL;
    sApd->mBindOffsetPtr    = NULL;
    
    /*
     * IRD
     */

    sIrd = &sStmt->mIrd;
    stlMemset( sIrd, 0x00, STL_SIZEOF( zldDesc ) );

    STL_TRY( zldDiagInit( SQL_HANDLE_DESC, (void*)sIrd, aErrorStack ) == STL_SUCCESS );
    sState = 5;

    STL_RING_INIT_HEADLINK( &sIrd->mDescRecRing,
                            STL_OFFSETOF( zldDescElement, mLink ) );

    sIrd->mType             = SQL_HANDLE_DESC;
    sIrd->mDescType         = ZLD_DESC_TYPE_IRD;
    sIrd->mTransition       = ZLD_TRANSITION_STATE_D1i;
    sIrd->mStmt             = sStmt;
    sIrd->mAllocType        = SQL_DESC_ALLOC_AUTO;
    sIrd->mCount            = 0;
    sIrd->mArrayStatusPtr   = NULL;
    sIrd->mBindType         = SQL_BIND_BY_COLUMN;
    sIrd->mArraySize        = 1;
    sIrd->mPutDataArraySize = 0;
    sIrd->mBindOffsetPtr    = NULL;
    sIrd->mRowProcessed     = NULL;
    
    /*
     * IPD
     */

    sIpd = &sStmt->mIpd;
    stlMemset( sIpd, 0x00, STL_SIZEOF( zldDesc ) );

    STL_TRY( zldDiagInit( SQL_HANDLE_DESC, (void*)sIpd, aErrorStack ) == STL_SUCCESS );
    sState = 6;

    STL_RING_INIT_HEADLINK( &sIpd->mDescRecRing,
                            STL_OFFSETOF( zldDescElement, mLink ) );

    sIpd->mType             = SQL_HANDLE_DESC;
    sIpd->mDescType         = ZLD_DESC_TYPE_IPD;
    sIpd->mTransition       = ZLD_TRANSITION_STATE_D1i;
    sIpd->mStmt             = sStmt;
    sIpd->mAllocType        = SQL_DESC_ALLOC_AUTO;
    sIpd->mCount            = 0;
    sIpd->mArraySize        = 1;
    sIpd->mPutDataArraySize = 0;
    sIpd->mArrayStatusPtr   = NULL;
    sIpd->mRowProcessed     = NULL;

    /*
     * Long Varying Memory Allocator 생성
     */

    STL_TRY( stlCreateDynamicAllocator( &sStmt->mLongVaryingMem,
                                        ZLS_LONG_VARYING_MEMORY_INIT_SIZE,
                                        ZLS_LONG_VARYING_MEMORY_NEXT_SIZE,
                                        aErrorStack )
             == STL_SUCCESS );
    sState = 7;

    /*
     * Allocator 생성
     */
    STL_TRY( stlCreateRegionAllocator( &sStmt->mAllocator,
                                       ZL_REGION_INIT_SIZE,
                                       ZL_REGION_NEXT_SIZE,
                                       aErrorStack ) == STL_SUCCESS );
    sState = 8;

    /*
     * GetData Element
     */

    sStmt->mGetDataOffset = 0;
    sStmt->mGetDataIdx    = 0;

    stlMemset( &sStmt->mGetDataElement, 0x00, STL_SIZEOF( zldDescElement ) );

    switch( aDbc->mTransition )
    {
        case ZLC_TRANSITION_STATE_C4 :
            aDbc->mTransition = ZLC_TRANSITION_STATE_C5;
            break;
        case ZLC_TRANSITION_STATE_C5 :
        case ZLC_TRANSITION_STATE_C6 :
            break;
    }

    /*
     * Explain Plan
     */
    sStmt->mExplainPlanType = SQL_EXPLAIN_PLAN_OFF;

#if defined( ODBC_ALL )
    STL_TRY( gZlsAllocFunc[aDbc->mProtocolType]( aDbc,
                                                 sStmt,
                                                 aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_DA )
    STL_TRY( zlasAlloc( aDbc,
                        sStmt,
                        aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_CS )
    STL_TRY( zlcsAlloc( aDbc,
                        sStmt,
                        aErrorStack ) == STL_SUCCESS );
#else
    STL_ASSERT( STL_FALSE );
#endif

    STL_RING_INIT_DATALINK( sStmt, STL_OFFSETOF(zlsStmt, mLink) );
    STL_RING_ADD_LAST( &aDbc->mStmtRing, sStmt );

    *aStmt = (void*)sStmt;

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 8 :
            (void)stlDestroyRegionAllocator( &sStmt->mAllocator,
                                             aErrorStack );
        case 7 :
            (void)stlDestroyDynamicAllocator( &sStmt->mLongVaryingMem,
                                              aErrorStack );
        case 6 :
            (void)zldDiagFini( SQL_HANDLE_DESC,
                               (void*)sIpd,
                               aErrorStack );
        case 5 :
            (void)zldDiagFini( SQL_HANDLE_DESC,
                               (void*)sIrd,
                               aErrorStack );
        case 4 :
            (void)zldDiagFini( SQL_HANDLE_DESC,
                               (void*)sApd,
                               aErrorStack );
        case 3 :
            (void)zldDiagFini( SQL_HANDLE_DESC,
                               (void*)sArd,
                               aErrorStack );
        case 2 :
            (void)zldDiagFini( SQL_HANDLE_STMT,
                               (void*)sStmt,
                               aErrorStack );
        case 1 :
            stlFreeHeap( sStmt );
        default :
            break;
    }

    return STL_FAILURE;
}

stlStatus zlsFree( zlsStmt       * aStmt,
                   stlBool         aIsCleanup,
                   stlErrorStack * aErrorStack )
{
    zlcDbc * sDbc;

    sDbc = aStmt->mParentDbc;

#if defined( ODBC_ALL )
    (void)gZlsFreeFunc[sDbc->mProtocolType]( sDbc,
                                             aStmt,
                                             ZLL_FREE_STMT_OPTION_DROP,
                                             aIsCleanup,
                                             aErrorStack );
#elif defined( ODBC_DA )
    (void)zlasFree( sDbc,
                    aStmt,
                    ZLL_FREE_STMT_OPTION_DROP,
                    aIsCleanup,
                    aErrorStack );
#elif defined( ODBC_CS )
    (void)zlcsFree( sDbc,
                    aStmt,
                    ZLL_FREE_STMT_OPTION_DROP,
                    aIsCleanup,
                    aErrorStack );
#else
    STL_ASSERT( STL_FALSE );
#endif

    STL_RING_UNLINK( &sDbc->mStmtRing,
                     aStmt );

    if( aStmt->mSQLBufferSize > 0 )
    {
        stlFreeHeap( aStmt->mSQLBuffer );

        aStmt->mSQLBuffer     = NULL;
        aStmt->mSQLBufferSize = 0;
    }

    (void)zldDescFreeRecs( &aStmt->mImpArd, aErrorStack );
    (void)zldDescFreeRecs( &aStmt->mImpApd, aErrorStack );
    (void)zldDescFreeRecs( &aStmt->mIrd, aErrorStack );
    (void)zldDescFreeRecs( &aStmt->mIpd, aErrorStack );
    
    (void)zldDiagFini( SQL_HANDLE_DESC,
                       (void*)&aStmt->mImpArd,
                       aErrorStack );
    (void)zldDiagFini( SQL_HANDLE_DESC,
                       (void*)&aStmt->mImpApd,
                       aErrorStack );
    (void)zldDiagFini( SQL_HANDLE_DESC,
                       (void*)&aStmt->mIrd,
                       aErrorStack );
    (void)zldDiagFini( SQL_HANDLE_DESC,
                       (void*)&aStmt->mIpd,
                       aErrorStack );
    (void)zldDiagFini( SQL_HANDLE_STMT,
                       (void*)aStmt,
                       aErrorStack );

    (void)stlDestroyRegionAllocator( &aStmt->mAllocator, aErrorStack );
    (void)stlDestroyDynamicAllocator( &aStmt->mLongVaryingMem, aErrorStack );
    
    stlMemset( aStmt, 0x00, STL_SIZEOF( zlsStmt ) );
    stlFreeHeap( aStmt );

    /**
     * error를 무시한다.
     */
    STL_INIT_ERROR_STACK( aErrorStack );
    
    return STL_SUCCESS;
}

stlStatus zlsClear( zlsStmt       * aStmt,
                    stlUInt16       aOption,
                    stlErrorStack * aErrorStack )
{
    zlcDbc  * sDbc;
    zldDesc * sArd;
    zldDesc * sApd;
    zldDesc * sIpd;
    zlsStmt * sStmtElement;
    stlBool   sHasOpendCursor = STL_FALSE;

    sDbc = aStmt->mParentDbc;
    sArd = aStmt->mArd;
    sApd = aStmt->mApd;
    sIpd = &aStmt->mIpd;
    
    switch( aOption )
    {
        case SQL_CLOSE :
            STL_TRY( zlrClose( aStmt, aErrorStack ) == STL_SUCCESS );

            switch( aStmt->mTransition )
            {
                case ZLS_TRANSITION_STATE_S4 :
                    if( aStmt->mIsPrepared != STL_TRUE )
                    {
                        aStmt->mTransition = ZLS_TRANSITION_STATE_S1; 
                    }
                    else
                    {
                        aStmt->mTransition = ZLS_TRANSITION_STATE_S2; 
                    }
                    break;
                case ZLS_TRANSITION_STATE_S5 :
                case ZLS_TRANSITION_STATE_S6 :
                case ZLS_TRANSITION_STATE_S7 :
                    if( aStmt->mIsPrepared != STL_TRUE )
                    {
                        aStmt->mTransition = ZLS_TRANSITION_STATE_S1; 
                    }
                    else
                    {
                        aStmt->mTransition = ZLS_TRANSITION_STATE_S3; 
                    }
                    break;
                default :
                    break;
            }
            break;
        case SQL_UNBIND :
            STL_TRY( zldDescFreeRecs( sArd, aErrorStack ) == STL_SUCCESS );
            break;
        case SQL_RESET_PARAMS :
#if defined( ODBC_ALL )
            (void)gZlsFreeFunc[sDbc->mProtocolType]( sDbc,
                                                     aStmt,
                                                     ZLL_FREE_STMT_OPTION_RESET_PARAMS,
                                                     STL_FALSE,
                                                     aErrorStack );
#elif defined( ODBC_DA )
            (void)zlasFree( sDbc,
                            aStmt,
                            ZLL_FREE_STMT_OPTION_RESET_PARAMS,
                            STL_FALSE,
                            aErrorStack );
#elif defined( ODBC_CS )
            (void)zlcsFree( sDbc,
                            aStmt,
                            ZLL_FREE_STMT_OPTION_RESET_PARAMS,
                            STL_FALSE,
                            aErrorStack );
#else
            STL_ASSERT( STL_FALSE );
#endif
            STL_TRY( zldDescFreeRecs( sApd, aErrorStack ) == STL_SUCCESS );
            STL_TRY( zldDescFreeRecs( sIpd, aErrorStack ) == STL_SUCCESS );

            aStmt->mParamChanged = STL_TRUE;
            
            aStmt->mHasParamIN   = STL_FALSE;
            aStmt->mHasParamOUT  = STL_FALSE;
            
            break;
    }

    if( sDbc->mTransition == ZLC_TRANSITION_STATE_C6 )
    {
        if( sDbc->mAttrAutoCommit == STL_TRUE )
        {
            STL_RING_FOREACH_ENTRY( &sDbc->mStmtRing, sStmtElement )
            {
                if( ( sStmtElement->mTransition == ZLS_TRANSITION_STATE_S5 ) ||
                    ( sStmtElement->mTransition == ZLS_TRANSITION_STATE_S6 ) ||
                    ( sStmtElement->mTransition == ZLS_TRANSITION_STATE_S7 ) )
                {
                    sHasOpendCursor = STL_TRUE;
                    break;
                }
            }

            if( sHasOpendCursor != STL_TRUE )
            {
                sDbc->mTransition = ZLC_TRANSITION_STATE_C5;
            }
        }
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zlsGetAttr( zlsStmt       * aStmt,
                      stlInt32        aAttr,
                      void          * aValue,
                      stlInt32        aBufferLen,
                      stlInt32      * aStrLen,
                      stlErrorStack * aErrorStack )
{
    zlcDbc  * sDbc;
    zldDesc * sArd;
    zldDesc * sApd;
    zldDesc * sIrd;
    zldDesc * sIpd;

    stlInt64  sValue;

    sDbc = aStmt->mParentDbc;
    sArd = aStmt->mArd;
    sApd = aStmt->mApd;
    sIrd = &aStmt->mIrd;
    sIpd = &aStmt->mIpd;

    switch( aAttr )
    {
        case SQL_ATTR_CONCURRENCY :
            if( aStmt->mCursorConcurrency == ZLS_CONCURRENCY_NA )
            {
                *(SQLULEN*)aValue = SQL_CONCUR_READ_ONLY;
            }
            else
            {
                *(SQLULEN*)aValue = aStmt->mCursorConcurrency;
            }
            break;
        case SQL_ATTR_CURSOR_HOLDABLE:
            if( aStmt->mCursorHoldable == ZLS_HOLDABILITY_NA )
            {
                *(SQLULEN*)aValue = SQL_HOLDABLE;
            }
            else
            {
                *(SQLULEN*)aValue = aStmt->mCursorHoldable;
            }
            break;
        case SQL_ATTR_CURSOR_SCROLLABLE :
            *(SQLULEN*)aValue = aStmt->mCursorScrollable;
            break;
        case SQL_ATTR_CURSOR_SENSITIVITY :
            *(SQLULEN*)aValue = aStmt->mCursorSensitivity;
            break;
        case SQL_ATTR_CURSOR_TYPE :
            *(SQLULEN*)aValue = aStmt->mCursorType;
            break;
        case SQL_ATTR_MAX_ROWS :
            *(SQLULEN*)aValue = aStmt->mMaxRows;
            break;
        case SQL_ATTR_METADATA_ID :
            if( aStmt->mMetadataId == STL_TRUE )
            {
                *(SQLULEN*)aValue = SQL_TRUE;
            }
            else
            {
                *(SQLULEN*)aValue = SQL_FALSE;
            }
            break;
        case SQL_ATTR_APP_PARAM_DESC :
            *(zldDesc**)aValue = sApd;
            break;
        case SQL_ATTR_APP_ROW_DESC :
            *(zldDesc**)aValue = sArd;
            break;
        case SQL_ATTR_IMP_PARAM_DESC :
            *(zldDesc**)aValue = sIpd;
            break;
        case SQL_ATTR_IMP_ROW_DESC :
            *(zldDesc**)aValue = sIrd;
            break;
        case SQL_ATTR_PARAM_BIND_OFFSET_PTR :
            *(SQLULEN**)aValue = (SQLULEN*)sApd->mBindOffsetPtr;
            break;
        case SQL_ATTR_PARAM_BIND_TYPE :
            *(SQLULEN*)aValue = sApd->mBindType;
            break;
        case SQL_ATTR_PARAM_OPERATION_PTR :
            *(SQLUSMALLINT**)aValue = sApd->mArrayStatusPtr;
            break;
        case SQL_ATTR_PARAM_STATUS_PTR :
            *(SQLUSMALLINT**)aValue = sIpd->mArrayStatusPtr;
            break;
        case SQL_ATTR_PARAMS_PROCESSED_PTR :
            *(SQLULEN**)aValue = sIpd->mRowProcessed;
            break;
        case SQL_ATTR_PARAMSET_SIZE :
            *(SQLULEN*)aValue = sApd->mArraySize;
            break;
        case SQL_ATTR_QUERY_TIMEOUT :
#if defined( ODBC_ALL )
            STL_TRY( gZlsGetAttr[sDbc->mProtocolType]( sDbc,
                                                       aStmt,
                                                       ZLL_STATEMENT_ATTR_QUERY_TIMEOUT,
                                                       (void*)&sValue,
                                                       0,
                                                       aStrLen,
                                                       aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_DA )
            STL_TRY( zlasGetAttr( sDbc,
                                  aStmt,
                                  ZLL_STATEMENT_ATTR_QUERY_TIMEOUT,
                                  (void*)&sValue,
                                  0,
                                  aStrLen,
                                  aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_CS )
            STL_TRY( zlcsGetAttr( sDbc,
                                  aStmt,
                                  ZLL_STATEMENT_ATTR_QUERY_TIMEOUT,
                                  (void*)&sValue,
                                  0,
                                  aStrLen,
                                  aErrorStack ) == STL_SUCCESS );
#else
            STL_ASSERT( STL_FALSE );
#endif
            *(SQLULEN*)aValue = sValue;
            break;
        case SQL_ATTR_RETRIEVE_DATA :
            if( aStmt->mRetrieveData == STL_TRUE)
            {
                *(SQLULEN*)aValue = SQL_RD_ON;
            }
            else
            {
                *(SQLULEN*)aValue = SQL_RD_OFF;
            }
            break;
        case SQL_ATTR_ROW_ARRAY_SIZE :
            *(SQLULEN*)aValue = sArd->mArraySize;
            break;
        case SQL_ROWSET_SIZE :
            *(SQLULEN*)aValue = aStmt->mRowsetSize;
            break;
        case SQL_ATTR_ROW_BIND_OFFSET_PTR :
            *(SQLULEN**)aValue = (SQLULEN*)sArd->mBindOffsetPtr;
            break;
        case SQL_ATTR_ROW_BIND_TYPE :
            *(SQLULEN*)aValue = sArd->mBindType;
            break;
        case SQL_ATTR_ROW_STATUS_PTR :
            *(SQLUSMALLINT**)aValue = sIrd->mArrayStatusPtr;
            break;
        case SQL_ATTR_ROW_NUMBER :
            *(SQLULEN*)aValue = aStmt->mResult.mCurrentRowIdx;
            break;
        case SQL_ATTR_ROWS_FETCHED_PTR :
            *(SQLULEN**)aValue = sIrd->mRowProcessed;
            break;
        case SQL_ATTR_EXPLAIN_PLAN_OPTION :
            *(stlUInt16*)aValue = aStmt->mExplainPlanType;
            break;
        case SQL_ATTR_ATOMIC_EXECUTION :
            *(stlUInt16*)aValue = aStmt->mAtomicExecution;
            break;
        case SQL_ATTR_EXPLAIN_PLAN_TEXT :
#if defined( ODBC_ALL )
            STL_TRY( gZlsGetAttr[sDbc->mProtocolType]( sDbc,
                                                       aStmt,
                                                       ZLL_STATEMENT_ATTR_EXPLAIN_PLAN_TEXT,
                                                       aValue,
                                                       aBufferLen,
                                                       aStrLen,
                                                       aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_DA )
            STL_TRY( zlasGetAttr( sDbc,
                                  aStmt,
                                  ZLL_STATEMENT_ATTR_EXPLAIN_PLAN_TEXT,
                                  aValue,
                                  aBufferLen,
                                  aStrLen,
                                  aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_CS )
            STL_TRY( zlcsGetAttr( sDbc,
                                  aStmt,
                                  ZLL_STATEMENT_ATTR_EXPLAIN_PLAN_TEXT,
                                  aValue,
                                  aBufferLen,
                                  aStrLen,
                                  aErrorStack ) == STL_SUCCESS );
#else
            STL_ASSERT( STL_FALSE );
#endif
            break;
        case SQL_ATTR_PREFETCH_ROWS :
            *(SQLULEN*)aValue = aStmt->mPreFetchSize;
            break;
        default :
            STL_THROW( RAMP_ERR_INVALID_ATTRIBUTE_OPTION_IDENTIFIER );
            break;
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_ATTRIBUTE_OPTION_IDENTIFIER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_ATTRIBUTE_OPTION_IDENTIFIER,
                      "The value specified for the argument Attribute was not "
                      "valid for the version of ODBC supported by the driver.",
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zlsGetAttrW( zlsStmt       * aStmt,
                       stlInt32        aAttr,
                       void          * aValue,
                       stlInt32        aBufferLen,
                       stlInt32      * aStrLen,
                       stlErrorStack * aErrorStack )
{
    zlcDbc        * sDbc;
    stlChar       * sText = NULL;
    stlInt32        sTextBufferLength = 0;
    stlInt32        sTextLength = 0;

    stlInt64  sBufferLength = 0;
    stlInt64  sLength = 0;

    sDbc = aStmt->mParentDbc;
    
    switch( aAttr )
    {
        case SQL_ATTR_EXPLAIN_PLAN_TEXT :
            /*
             * PLAN의 전체 길이를 알아온 다음
             * 전체 길이만큼 메모리 할당 후 다시 PLAN을 얻어온다.
             */
            
            sTextBufferLength = 0;
            
#if defined( ODBC_ALL )
            STL_TRY( gZlsGetAttr[sDbc->mProtocolType]( sDbc,
                                                       aStmt,
                                                       ZLL_STATEMENT_ATTR_EXPLAIN_PLAN_TEXT,
                                                       (void*)sText,
                                                       sTextBufferLength,
                                                       &sTextLength,
                                                       aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_DA )
            STL_TRY( zlasGetAttr( sDbc,
                                  aStmt,
                                  ZLL_STATEMENT_ATTR_EXPLAIN_PLAN_TEXT,
                                  (void*)sText,
                                  sTextBufferLength,
                                  &sTextLength,
                                  aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_CS )
            STL_TRY( zlcsGetAttr( sDbc,
                                  aStmt,
                                  ZLL_STATEMENT_ATTR_EXPLAIN_PLAN_TEXT,
                                  (void*)sText,
                                  sTextBufferLength,
                                  &sTextLength,
                                  aErrorStack ) == STL_SUCCESS );
#else
            STL_ASSERT( STL_FALSE );
#endif
            sTextBufferLength = sTextLength + 1;

            STL_TRY( stlAllocRegionMem( &aStmt->mAllocator,
                                        sTextBufferLength,
                                        (void**)&sText,
                                        aErrorStack ) == STL_SUCCESS );

#if defined( ODBC_ALL )
            STL_TRY( gZlsGetAttr[sDbc->mProtocolType]( sDbc,
                                                       aStmt,
                                                       ZLL_STATEMENT_ATTR_EXPLAIN_PLAN_TEXT,
                                                       (void*)sText,
                                                       sTextBufferLength,
                                                       &sTextLength,
                                                       aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_DA )
            STL_TRY( zlasGetAttr( sDbc,
                                  aStmt,
                                  ZLL_STATEMENT_ATTR_EXPLAIN_PLAN_TEXT,
                                  (void*)sText,
                                  sTextBufferLength,
                                  &sTextLength,
                                  aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_CS )
            STL_TRY( zlcsGetAttr( sDbc,
                                  aStmt,
                                  ZLL_STATEMENT_ATTR_EXPLAIN_PLAN_TEXT,
                                  (void*)sText,
                                  sTextBufferLength,
                                  &sTextLength,
                                  aErrorStack ) == STL_SUCCESS );
#else
            STL_ASSERT( STL_FALSE );
#endif

            STL_DASSERT( sTextLength < sTextBufferLength );
            
            sBufferLength = aBufferLen / STL_SIZEOF(SQLWCHAR);

            STL_TRY( zlvCharToWchar( sDbc->mCharacterSet,
                                     (SQLCHAR*)sText,
                                     sTextLength,
                                     (SQLWCHAR*)aValue,
                                     sBufferLength,
                                     &sLength,
                                     aErrorStack ) == STL_SUCCESS );

            if( aStrLen != NULL )
            {
                *aStrLen = sLength * STL_SIZEOF(SQLWCHAR);
            }
            break;
        default :
            STL_TRY( zlsGetAttr( aStmt,
                                 aAttr,
                                 aValue,
                                 aBufferLen,
                                 aStrLen,
                                 aErrorStack ) == STL_SUCCESS );
            STL_THROW( RAMP_FINISH );
            break;
    }

    STL_RAMP( RAMP_FINISH );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zlsSetAttr( zlsStmt       * aStmt,
                      stlInt32        aAttr,
                      void          * aValue,
                      stlInt32        aStrLen,
                      stlBool       * aSuccessWithInfo,
                      stlErrorStack * aErrorStack )
{
    zlcDbc            * sDbc;
    zldDesc           * sArd;
    zldDesc           * sApd;
    zldDesc           * sIrd;
    zldDesc           * sIpd;
    zldDescElement    * sIpdRec;

    dtlValueBlockList * sBlockList;
    dtlDataValue      * sDataValue;

    stlVarInt           sVarInt;
    stlInt64            sIntValue;

    stlBool             sOptionChanged = STL_FALSE;
    stlInt32            i;

    stlStatus           sRet = STL_FAILURE;

    *aSuccessWithInfo = STL_FALSE;

    sDbc = aStmt->mParentDbc;
    sArd = aStmt->mArd;
    sApd = aStmt->mApd;
    sIrd = &aStmt->mIrd;
    sIpd = &aStmt->mIpd;

    switch( aAttr )
    {
        case SQL_ATTR_CONCURRENCY:
            sVarInt = STL_INT_FROM_POINTER(aValue);
            switch( sVarInt )
            {
                case SQL_CONCUR_ROWVER:
                case SQL_CONCUR_VALUES:
                    sOptionChanged = STL_TRUE;
                case SQL_CONCUR_READ_ONLY:
                    sIntValue = ZLL_CURSOR_UPDATABILITY_FOR_READ_ONLY;
                    break;
                case SQL_CONCUR_LOCK:
                    sIntValue = ZLL_CURSOR_UPDATABILITY_FOR_UPDATE;
                    break;
                default:
                    STL_THROW( RAMP_ERR_INVALID_ATTRIBUTE_VALUE );
                    break;
            }

#if defined( ODBC_ALL )
            STL_TRY( gZlsSetAttr[sDbc->mProtocolType]( sDbc,
                                                       aStmt,
                                                       ZLL_STATEMENT_ATTR_CONCURRENCY,
                                                       sIntValue,
                                                       NULL,
                                                       0,
                                                       aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_DA )
            STL_TRY( zlasSetAttr( sDbc,
                                  aStmt,
                                  ZLL_STATEMENT_ATTR_CONCURRENCY,
                                  sIntValue,
                                  NULL,
                                  0,
                                  aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_CS )
            STL_TRY( zlcsSetAttr( sDbc,
                                  aStmt,
                                  ZLL_STATEMENT_ATTR_CONCURRENCY,
                                  sIntValue,
                                  NULL,
                                  0,
                                  aErrorStack ) == STL_SUCCESS );
#else
            STL_ASSERT( STL_FALSE );
#endif

            aStmt->mCursorConcurrency = sVarInt;
            break;
        case SQL_ATTR_CURSOR_HOLDABLE:
            sVarInt = STL_INT_FROM_POINTER(aValue);
            switch( sVarInt )
            {
                case SQL_NONHOLDABLE:
                    sIntValue = ZLL_CURSOR_HOLDABILITY_WITHOUT_HOLD;
                    break;
                case SQL_HOLDABLE:
                    sIntValue = ZLL_CURSOR_HOLDABILITY_WITH_HOLD;
                    break;
                default:
                    STL_THROW( RAMP_ERR_INVALID_ATTRIBUTE_VALUE );
                    break;
            }

#if defined( ODBC_ALL )
            STL_TRY( gZlsSetAttr[sDbc->mProtocolType]( sDbc,
                                                       aStmt,
                                                       ZLL_STATEMENT_ATTR_CURSOR_HOLDABILITY,
                                                       sIntValue,
                                                       NULL,
                                                       0,
                                                       aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_DA )
            STL_TRY( zlasSetAttr( sDbc,
                                  aStmt,
                                  ZLL_STATEMENT_ATTR_CURSOR_HOLDABILITY,
                                  sIntValue,
                                  NULL,
                                  0,
                                  aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_CS )
            STL_TRY( zlcsSetAttr( sDbc,
                                  aStmt,
                                  ZLL_STATEMENT_ATTR_CURSOR_HOLDABILITY,
                                  sIntValue,
                                  NULL,
                                  0,
                                  aErrorStack ) == STL_SUCCESS );
#else
            STL_ASSERT( STL_FALSE );
#endif

            aStmt->mCursorHoldable = sVarInt;
            break;
        case SQL_ATTR_CURSOR_SCROLLABLE :
            sVarInt = STL_INT_FROM_POINTER(aValue);
            switch( sVarInt )
            {
                case SQL_NONSCROLLABLE:
                    sIntValue = ZLL_CURSOR_SCROLLABILITY_NO_SCROLL;
                    break;
                case SQL_SCROLLABLE:
                    switch( aStmt->mCursorType )
                    {
                        case SQL_CURSOR_FORWARD_ONLY:
                            sOptionChanged = STL_TRUE;
                            sIntValue = ZLL_CURSOR_SCROLLABILITY_NO_SCROLL;
                            break;
                        case SQL_CURSOR_ISO:
                            sIntValue = ZLL_CURSOR_SCROLLABILITY_SCROLL;
                            break;
                        case SQL_CURSOR_STATIC:
                            sIntValue = ZLL_CURSOR_SCROLLABILITY_SCROLL;
                            break;
                        case SQL_CURSOR_KEYSET_DRIVEN:
                            sIntValue = ZLL_CURSOR_SCROLLABILITY_SCROLL;
                            break;
                        case SQL_CURSOR_DYNAMIC:
                            sIntValue = ZLL_CURSOR_SCROLLABILITY_SCROLL;
                            break;
                        default :
                            STL_THROW( RAMP_ERR_INVALID_ATTRIBUTE_VALUE );
                            break;
                    }
                    break;
                default:
                    STL_THROW( RAMP_ERR_INVALID_ATTRIBUTE_VALUE );
                    break;
            }

#if defined( ODBC_ALL )
            STL_TRY( gZlsSetAttr[sDbc->mProtocolType]( sDbc,
                                                       aStmt,
                                                       ZLL_STATEMENT_ATTR_CURSOR_SCROLLABLE,
                                                       sIntValue,
                                                       NULL,
                                                       0,
                                                       aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_DA )
            STL_TRY( zlasSetAttr( sDbc,
                                  aStmt,
                                  ZLL_STATEMENT_ATTR_CURSOR_SCROLLABLE,
                                  sIntValue,
                                  NULL,
                                  0,
                                  aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_CS )
            STL_TRY( zlcsSetAttr( sDbc,
                                  aStmt,
                                  ZLL_STATEMENT_ATTR_CURSOR_SCROLLABLE,
                                  sIntValue,
                                  NULL,
                                  0,
                                  aErrorStack ) == STL_SUCCESS );
#else
            STL_ASSERT( STL_FALSE );
#endif

            aStmt->mCursorScrollable = sVarInt;
            break;
        case SQL_ATTR_CURSOR_SENSITIVITY :
            sVarInt = STL_INT_FROM_POINTER(aValue);
            switch( sVarInt )
            {
                case SQL_UNSPECIFIED:
                    sIntValue = ZLL_CURSOR_SENSITIVITY_ASENSITIVE;
                    aStmt->mPreFetchSize = sArd->mArraySize;
                    break;
                case SQL_INSENSITIVE:
                    sIntValue = ZLL_CURSOR_SENSITIVITY_INSENSITIVE;
                    break;
                case SQL_SENSITIVE:
                    sIntValue = ZLL_CURSOR_SENSITIVITY_SENSITIVE;
                    aStmt->mPreFetchSize = sArd->mArraySize;
                    break;
                default:
                    STL_THROW( RAMP_ERR_INVALID_ATTRIBUTE_VALUE );
                    break;
            }

#if defined( ODBC_ALL )
            STL_TRY( gZlsSetAttr[sDbc->mProtocolType]( sDbc,
                                                       aStmt,
                                                       ZLL_STATEMENT_ATTR_CURSOR_SENSITIVITY,
                                                       sIntValue,
                                                       NULL,
                                                       0,
                                                       aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_DA )
            STL_TRY( zlasSetAttr( sDbc,
                                  aStmt,
                                  ZLL_STATEMENT_ATTR_CURSOR_SENSITIVITY,
                                  sIntValue,
                                  NULL,
                                  0,
                                  aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_CS )
            STL_TRY( zlcsSetAttr( sDbc,
                                  aStmt,
                                  ZLL_STATEMENT_ATTR_CURSOR_SENSITIVITY,
                                  sIntValue,
                                  NULL,
                                  0,
                                  aErrorStack ) == STL_SUCCESS );
#else
            STL_ASSERT( STL_FALSE );
#endif

            aStmt->mCursorSensitivity = sVarInt;
            break;
        case SQL_ATTR_CURSOR_TYPE :
            /*===============================================================================================
             * Application sets attribute to | Other attributes set implicitly
             *===============================================================================================
             * SQL_ATTR_CURSOR_TYPE to       | SQL_ATTR_SCROLLABLE to SQL_SCROLLABLE.
             * SQL_CURSOR_DYNAMIC            | SQL_ATTR_CURSOR_SENSITIVITY to SQL_SENSITIVE. (But only if
             *                               | SQL_ATTR_CONCURRENCY is not equal to
             *                               | SQL_CONCUR_READ_ONLY. Updatable dynamic cursors are always
             *                               | sensitive to changes that were made in their own transaction.)
             *===============================================================================================
             * SQL_ATTR_CURSOR_TYPE to       | SQL_ATTR_CURSOR_SCROLLABLE to SQL_NONSCROLLABLE.
             * SQL_CURSOR_FORWARD_ONLY       | 
             *===============================================================================================
             * SQL_ATTR_CURSOR_TYPE to       | nothing.
             * SQL_CURSOR_ISO                | 
             *===============================================================================================
             * SQL_ATTR_CURSOR_TYPE to       | SQL_ATTR_SCROLLABLE to SQL_SCROLLABLE.
             * SQL_CURSOR_KEYSET_DRIVEN      | SQL_ATTR_SENSITIVITY to SQL_UNSPECIFIED or SQL_SENSITIVE
             *                               | (according to driver-defined criteria, if SQL_ATTR_CONCURRENCY is not
             *                               | SQL_CONCUR_READ_ONLY).
             *===============================================================================================
             * SQL_ATTR_CURSOR_TYPE to       | SQL_ATTR_SCROLLABLE to SQL_SCROLLABLE.
             * SQL_CURSOR_STATIC             | SQL_ATTR_SENSITIVITY to SQL_INSENSITIVE (if
             *                               | SQL_ATTR_CONCURRENCY is SQL_CONCUR_READ_ONLY).
             *                               | SQL_ATTR_SENSITIVITY to SQL_UNSPECIFIED or SQL_SENSITIVE (if
             *                               | SQL_ATTR_CONCURRENCY is not SQL_CONCUR_READ_ONLY).
             *===============================================================================================*/
            sVarInt = STL_INT_FROM_POINTER(aValue);
            switch( sVarInt )
            {
                case SQL_CURSOR_FORWARD_ONLY:
                    sIntValue = ZLL_CURSOR_STANDARD_ISO;
                    aStmt->mCursorScrollable = SQL_NONSCROLLABLE;
                    break;
                case SQL_CURSOR_ISO:
                    sIntValue = ZLL_CURSOR_STANDARD_ISO;
                    break;
                case SQL_CURSOR_DYNAMIC:
                    sOptionChanged = STL_TRUE;
                case SQL_CURSOR_KEYSET_DRIVEN:
                    sIntValue = ZLL_CURSOR_STANDARD_ODBC_KEYSET;
                    aStmt->mCursorScrollable = SQL_SCROLLABLE;
                    if( aStmt->mCursorConcurrency != SQL_CONCUR_READ_ONLY )
                    {
                        aStmt->mCursorSensitivity = SQL_UNSPECIFIED;
                    }
                    break;
                case SQL_CURSOR_STATIC:
                    sIntValue = ZLL_CURSOR_STANDARD_ODBC_STATIC;
                    aStmt->mCursorScrollable = SQL_SCROLLABLE;
                    if( aStmt->mCursorConcurrency == SQL_CONCUR_READ_ONLY )
                    {
                        aStmt->mCursorSensitivity = SQL_INSENSITIVE;
                    }
                    else
                    {
                        aStmt->mCursorSensitivity = SQL_UNSPECIFIED;
                    }
                    break;
                default:
                    STL_THROW( RAMP_ERR_INVALID_ATTRIBUTE_VALUE );
                    break;
            }

#if defined( ODBC_ALL )
            STL_TRY( gZlsSetAttr[sDbc->mProtocolType]( sDbc,
                                                       aStmt,
                                                       ZLL_STATEMENT_ATTR_CURSOR_TYPE,
                                                       sIntValue,
                                                       NULL,
                                                       0,
                                                       aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_DA )
            STL_TRY( zlasSetAttr( sDbc,
                                  aStmt,
                                  ZLL_STATEMENT_ATTR_CURSOR_TYPE,
                                  sIntValue,
                                  NULL,
                                  0,
                                  aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_CS )
            STL_TRY( zlcsSetAttr( sDbc,
                                  aStmt,
                                  ZLL_STATEMENT_ATTR_CURSOR_TYPE,
                                  sIntValue,
                                  NULL,
                                  0,
                                  aErrorStack ) == STL_SUCCESS );
#else
            STL_ASSERT( STL_FALSE );
#endif

            aStmt->mCursorType = sVarInt;

            switch( aStmt->mCursorScrollable )
            {
                case SQL_NONSCROLLABLE:
                    sIntValue = ZLL_CURSOR_SCROLLABILITY_NO_SCROLL;
                    break;
                case SQL_SCROLLABLE:
                    sIntValue = ZLL_CURSOR_SCROLLABILITY_SCROLL;
                    break;
                default:
                    STL_THROW( RAMP_ERR_INVALID_ATTRIBUTE_VALUE );
                    break;
            }

#if defined( ODBC_ALL )
            STL_TRY( gZlsSetAttr[sDbc->mProtocolType]( sDbc,
                                                       aStmt,
                                                       ZLL_STATEMENT_ATTR_CURSOR_SCROLLABLE,
                                                       sIntValue,
                                                       NULL,
                                                       0,
                                                       aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_DA )
            STL_TRY( zlasSetAttr( sDbc,
                                  aStmt,
                                  ZLL_STATEMENT_ATTR_CURSOR_SCROLLABLE,
                                  sIntValue,
                                  NULL,
                                  0,
                                  aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_CS )
            STL_TRY( zlcsSetAttr( sDbc,
                                  aStmt,
                                  ZLL_STATEMENT_ATTR_CURSOR_SCROLLABLE,
                                  sIntValue,
                                  NULL,
                                  0,
                                  aErrorStack ) == STL_SUCCESS );
#else
            STL_ASSERT( STL_FALSE );
#endif

            switch( aStmt->mCursorSensitivity )
            {
                case SQL_UNSPECIFIED:
                    sIntValue = ZLL_CURSOR_SENSITIVITY_ASENSITIVE;
                    aStmt->mPreFetchSize = sArd->mArraySize;
                    break;
                case SQL_INSENSITIVE:
                    sIntValue = ZLL_CURSOR_SENSITIVITY_INSENSITIVE;
                    break;
                case SQL_SENSITIVE:
                    sIntValue = ZLL_CURSOR_SENSITIVITY_SENSITIVE;
                    aStmt->mPreFetchSize = sArd->mArraySize;
                    break;
                default:
                    STL_THROW( RAMP_ERR_INVALID_ATTRIBUTE_VALUE );
                    break;
            }

#if defined( ODBC_ALL )
            STL_TRY( gZlsSetAttr[sDbc->mProtocolType]( sDbc,
                                                       aStmt,
                                                       ZLL_STATEMENT_ATTR_CURSOR_SENSITIVITY,
                                                       sIntValue,
                                                       NULL,
                                                       0,
                                                       aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_DA )
            STL_TRY( zlasSetAttr( sDbc,
                                  aStmt,
                                  ZLL_STATEMENT_ATTR_CURSOR_SENSITIVITY,
                                  sIntValue,
                                  NULL,
                                  0,
                                  aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_CS )
            STL_TRY( zlcsSetAttr( sDbc,
                                  aStmt,
                                  ZLL_STATEMENT_ATTR_CURSOR_SENSITIVITY,
                                  sIntValue,
                                  NULL,
                                  0,
                                  aErrorStack ) == STL_SUCCESS );
#else
            STL_ASSERT( STL_FALSE );
#endif
            break;
        case SQL_ATTR_MAX_ROWS :
            aStmt->mMaxRows = (SQLULEN)(stlVarInt)aValue;
            break;
        case SQL_ATTR_METADATA_ID :
            switch( STL_INT_FROM_POINTER(aValue) )
            {
                case SQL_TRUE:
                    aStmt->mMetadataId = STL_TRUE;
                    break;
                case SQL_FALSE:
                    aStmt->mMetadataId = STL_FALSE;
                    break;
                default:
                    STL_THROW( RAMP_ERR_INVALID_ATTRIBUTE_VALUE );
                    break;
            }
            break;
        case SQL_ATTR_PARAM_BIND_OFFSET_PTR :
            sApd->mBindOffsetPtr = (SQLLEN*)aValue;
            break;
        case SQL_ATTR_PARAM_BIND_TYPE :
            sApd->mBindType = (stlInt32)((stlVarInt)aValue);
            break;
        case SQL_ATTR_PARAM_OPERATION_PTR :
            sApd->mArrayStatusPtr = (stlUInt16*)aValue;
            break;
        case SQL_ATTR_PARAM_STATUS_PTR :
            sIpd->mArrayStatusPtr = (stlUInt16*)aValue;
            break;
        case SQL_ATTR_PARAMS_PROCESSED_PTR :
            sIpd->mRowProcessed = (SQLULEN*)aValue;
            break;
        case SQL_ATTR_PARAMSET_SIZE :
            STL_TRY_THROW( ((stlVarInt)aValue) > 0, RAMP_ERR_INVALID_ATTRIBUTE_VALUE );

            if( sApd->mArraySize != (stlVarInt)aValue )
            {
                if( aStmt->mAtomicExecution == SQL_ATOMIC_EXECUTION_ON )
                {
                    /**
                     * LONG VARCHAR, LONG VARBINARY인 경우 기존의 메모리를 해제하고 array size를 재설정한다.
                     */
                    STL_RING_FOREACH_ENTRY( &sIpd->mDescRecRing, sIpdRec )
                    {
                        if( (sIpdRec->mConsiceType == SQL_LONGVARCHAR) ||
                            (sIpdRec->mConsiceType == SQL_LONGVARBINARY) )
                        {
                            sBlockList = sIpdRec->mInputValueBlock;

                            if( (sBlockList != NULL ) && (sBlockList->mValueBlock != NULL ) )
                            {
                                sDataValue = sBlockList->mValueBlock->mDataValue;

                                for( i = 0; i < sIpd->mArraySize; i++ )
                                {
                                    STL_TRY( stlFreeDynamicMem( &aStmt->mLongVaryingMem,
                                                                sDataValue[i].mValue,
                                                                aErrorStack )
                                             == STL_SUCCESS );
                                }
                            }
                        }
                    }

                    sIntValue = (stlInt32)((stlVarInt)aValue);
                
#if defined( ODBC_ALL )
                    STL_TRY( gZlsSetAttr[sDbc->mProtocolType]( sDbc,
                                                               aStmt,
                                                               ZLL_STATEMENT_ATTR_PARAMSET_SIZE,
                                                               sIntValue,
                                                               NULL,
                                                               0,
                                                               aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_DA )
                    STL_TRY( zlasSetAttr( sDbc,
                                          aStmt,
                                          ZLL_STATEMENT_ATTR_PARAMSET_SIZE,
                                          sIntValue,
                                          NULL,
                                          0,
                                          aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_CS )
                    STL_TRY( zlcsSetAttr( sDbc,
                                          aStmt,
                                          ZLL_STATEMENT_ATTR_PARAMSET_SIZE,
                                          sIntValue,
                                          NULL,
                                          0,
                                          aErrorStack ) == STL_SUCCESS );
#else
                    STL_ASSERT( STL_FALSE );
#endif
                    sIpd->mArraySize = (SQLULEN)(stlVarInt)aValue;
                }
            
                sApd->mArraySize = (SQLULEN)(stlVarInt)aValue;
                sApd->mChanged = STL_TRUE;
                aStmt->mParamChanged = STL_TRUE;
            }
            break;
        case SQL_ATTR_QUERY_TIMEOUT :
            sIntValue = (stlInt32)((stlVarInt)aValue);

#if defined( ODBC_ALL )
            STL_TRY( gZlsSetAttr[sDbc->mProtocolType]( sDbc,
                                                       aStmt,
                                                       ZLL_STATEMENT_ATTR_QUERY_TIMEOUT,
                                                       sIntValue,
                                                       NULL,
                                                       0,
                                                       aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_DA )
            STL_TRY( zlasSetAttr( sDbc,
                                  aStmt,
                                  ZLL_STATEMENT_ATTR_QUERY_TIMEOUT,
                                  sIntValue,
                                  NULL,
                                  0,
                                  aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_CS )
            STL_TRY( zlcsSetAttr( sDbc,
                                  aStmt,
                                  ZLL_STATEMENT_ATTR_QUERY_TIMEOUT,
                                  sIntValue,
                                  NULL,
                                  0,
                                  aErrorStack ) == STL_SUCCESS );
#else
            STL_ASSERT( STL_FALSE );
#endif
            break;
        case SQL_ATTR_RETRIEVE_DATA :
            sVarInt = STL_INT_FROM_POINTER(aValue);
            switch( sVarInt )
            {
                case SQL_RD_ON :
                    aStmt->mRetrieveData = STL_TRUE;
                    break;
                case SQL_RD_OFF :
                    aStmt->mRetrieveData = STL_FALSE;
                    break;
                default :
                    STL_THROW( RAMP_ERR_INVALID_ATTRIBUTE_VALUE );
                    break;
            }
            break;
        case SQL_ATTR_ROW_ARRAY_SIZE :
            STL_TRY_THROW( ((stlVarInt)aValue) > 0, RAMP_ERR_INVALID_ATTRIBUTE_VALUE );
            sArd->mArraySize = (SQLULEN)(stlVarInt)aValue;
            break;
        case SQL_ROWSET_SIZE :
            STL_TRY_THROW( ((stlVarInt)aValue) > 0, RAMP_ERR_INVALID_ATTRIBUTE_VALUE );
            aStmt->mRowsetSize = (SQLULEN)(stlVarInt)aValue;
            break;
        case SQL_ATTR_ROW_BIND_OFFSET_PTR :
            sArd->mBindOffsetPtr = (SQLLEN*)aValue;
            break;
        case SQL_ATTR_ROW_BIND_TYPE :
            sArd->mBindType = (stlInt32)((stlVarInt)aValue);
            break;
        case SQL_ATTR_ROW_STATUS_PTR :
            sIrd->mArrayStatusPtr = (stlUInt16*)aValue;
            break;
        case SQL_ATTR_ROWS_FETCHED_PTR :
            sIrd->mRowProcessed = (SQLULEN*)aValue;
            break;
        case SQL_ATTR_EXPLAIN_PLAN_OPTION :
            sVarInt = STL_INT_FROM_POINTER(aValue);
            switch( sVarInt )
            {
                case SQL_EXPLAIN_PLAN_OFF  :
                    sIntValue = ZLL_EXPLAIN_PLAN_OFF;
                    break;
                case SQL_EXPLAIN_PLAN_ON   :
                    sIntValue = ZLL_EXPLAIN_PLAN_ON;
                    break;
                case SQL_EXPLAIN_PLAN_ON_VERBOSE :
                    sIntValue = ZLL_EXPLAIN_PLAN_ON_VERBOSE;
                    break;
                case SQL_EXPLAIN_PLAN_ONLY :
                    sIntValue = ZLL_EXPLAIN_PLAN_ONLY;
                    break;
                default :
                    STL_THROW( RAMP_ERR_INVALID_ATTRIBUTE_VALUE );
                    break;
            }

#if defined( ODBC_ALL )
            STL_TRY( gZlsSetAttr[sDbc->mProtocolType]( sDbc,
                                                       aStmt,
                                                       ZLL_STATEMENT_ATTR_EXPLAIN_PLAN_OPTION,
                                                       sIntValue,
                                                       NULL,
                                                       0,
                                                       aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_DA )
            STL_TRY( zlasSetAttr( sDbc,
                                  aStmt,
                                  ZLL_STATEMENT_ATTR_EXPLAIN_PLAN_OPTION,
                                  sIntValue,
                                  NULL,
                                  0,
                                  aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_CS )
            STL_TRY( zlcsSetAttr( sDbc,
                                  aStmt,
                                  ZLL_STATEMENT_ATTR_EXPLAIN_PLAN_OPTION,
                                  sIntValue,
                                  NULL,
                                  0,
                                  aErrorStack ) == STL_SUCCESS );
#else
            STL_ASSERT( STL_FALSE );
#endif
            
            aStmt->mExplainPlanType = (stlUInt16)sVarInt;
            break;
        case SQL_ATTR_ATOMIC_EXECUTION :
            sVarInt = STL_INT_FROM_POINTER(aValue);

            if( aStmt->mAtomicExecution != sVarInt )
            {
                switch( sVarInt )
                {
                    case SQL_ATOMIC_EXECUTION_ON  :
                        sIpd->mArraySize = sApd->mArraySize;
                        sIntValue = ZLL_ATOMIC_EXECUTION_ON;
                        break;
                    case SQL_ATOMIC_EXECUTION_OFF :
                        sIpd->mArraySize = 1;
                        sIntValue = ZLL_ATOMIC_EXECUTION_OFF;
                        break;
                    default :
                        STL_THROW( RAMP_ERR_INVALID_ATTRIBUTE_VALUE );
                        break;
                }
            
#if defined( ODBC_ALL )
                STL_TRY( gZlsSetAttr[sDbc->mProtocolType]( sDbc,
                                                           aStmt,
                                                           ZLL_STATEMENT_ATTR_ATOMIC_EXECUTION,
                                                           sIntValue,
                                                           NULL,
                                                           0,
                                                           aErrorStack ) == STL_SUCCESS );

                STL_TRY( gZlsSetAttr[sDbc->mProtocolType]( sDbc,
                                                           aStmt,
                                                           ZLL_STATEMENT_ATTR_PARAMSET_SIZE,
                                                           (stlInt32)sIpd->mArraySize,
                                                           NULL,
                                                           0,
                                                           aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_DA )
                STL_TRY( zlasSetAttr( sDbc,
                                      aStmt,
                                      ZLL_STATEMENT_ATTR_ATOMIC_EXECUTION,
                                      sIntValue,
                                      NULL,
                                      0,
                                      aErrorStack ) == STL_SUCCESS );

                STL_TRY( zlasSetAttr( sDbc,
                                      aStmt,
                                      ZLL_STATEMENT_ATTR_PARAMSET_SIZE,
                                      (stlInt32)sIpd->mArraySize,
                                      NULL,
                                      0,
                                      aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_CS )
                STL_TRY( zlcsSetAttr( sDbc,
                                      aStmt,
                                      ZLL_STATEMENT_ATTR_ATOMIC_EXECUTION,
                                      sIntValue,
                                      NULL,
                                      0,
                                      aErrorStack ) == STL_SUCCESS );

                STL_TRY( zlcsSetAttr( sDbc,
                                      aStmt,
                                      ZLL_STATEMENT_ATTR_PARAMSET_SIZE,
                                      (stlInt32)sIpd->mArraySize,
                                      NULL,
                                      0,
                                      aErrorStack ) == STL_SUCCESS );
#else
                STL_ASSERT( STL_FALSE );
#endif

                aStmt->mAtomicExecution = (stlUInt16)(sVarInt);
                aStmt->mParamChanged = STL_TRUE;
                sApd->mChanged = STL_TRUE;
            }
            break;
        case SQL_ATTR_PREFETCH_ROWS :
            aStmt->mPreFetchSize = (stlInt32)STL_INT_FROM_POINTER(aValue);
            break;
        default :
            STL_THROW( RAMP_ERR_INVALID_ATTRIBUTE_OPTION_IDENTIFIER );
            break;
    }

    STL_TRY_THROW( sOptionChanged == STL_FALSE, RAMP_WARNINGS_OPTION_VALUE_CHANGED );
    
    sRet = STL_SUCCESS;
    
    return sRet;

    STL_CATCH( RAMP_ERR_INVALID_ATTRIBUTE_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_ATTRIBUTE_VALUE,
                      "Given the specified Attribute value, "
                      "an invalid value was specified in ValuePtr.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_ATTRIBUTE_OPTION_IDENTIFIER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_ATTRIBUTE_OPTION_IDENTIFIER,
                      "The value specified for the argument Attribute was not "
                      "valid for the version of ODBC supported by the driver.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_WARNINGS_OPTION_VALUE_CHANGED )
    {
        sRet              = STL_SUCCESS;
        *aSuccessWithInfo = STL_TRUE;
    
        stlPushError( STL_ERROR_LEVEL_WARNING,
                      ZLE_ERRCODE_WARNING_OPTION_VALUE_CHANGED,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    return sRet;
}

/** @} */
