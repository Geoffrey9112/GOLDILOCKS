/*******************************************************************************
 * zlcDbc.c
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

#include <cml.h>
#include <goldilocks.h>

#if defined( ODBC_ALL ) || defined( ODBC_DA )
#include <ssl.h>
#endif

#include <zlDef.h>
#include <zle.h>
#include <zld.h>

#include <zlc.h>
#include <zls.h>

#include <zlvCharacterset.h>

#include <zlac.h>
#include <zlcc.h>

#include <zllDbcNameHash.h>

#ifdef WIN32
#include <zlcResource.h>
#endif

/**
 * @file zlcDbc.c
 * @brief ODBC API Internal Connect Routines.
 */

extern stlIni  gZlnUserDsn;
extern stlIni  gZlnSystemDsn;
extern stlBool gZlnExistUserDsn;
extern stlBool gZlnExistSystemDsn;

dtlCharacterSet gZlcClientCharacterSet = DTL_NOMOUNT_CHARSET_ID;
stlInt32        gZlcClientTimezone;

#ifdef WIN32
extern HINSTANCE gZllInstance;

typedef struct zlcDialogAttr
{
    stlChar       * mHost;
    stlInt32      * mPort;
    stlChar       * mUserName;
    stlInt16      * mUserNameLength;
    stlChar       * mPassword;
    stlInt16      * mPasswordLength;
    stlErrorStack * mErrorStack;
} zlcDialogAttr;
#endif

/**
 * @addtogroup zlcDbc
 * @{
 */

#if defined( ODBC_ALL )
stlStatus (*gZlcConnectFunc[ZLC_PROTOCOL_TYPE_MAX])(zlcDbc        * aDbc,
                                                    stlChar       * aHost,
                                                    stlInt32        aPort,
                                                    stlChar       * aCsMode,
                                                    stlChar       * aUser,
                                                    stlChar       * aEncryptedPassword,
                                                    stlChar       * aRole,
                                                    stlChar       * aOldPassword,
                                                    stlChar       * aNewPassword,
                                                    stlBool         aIsStartup,
                                                    stlProc         aProc,
                                                    stlChar       * aProgram,
                                                    stlBool       * aSuccessWithInfo,
                                                    stlErrorStack * aErrorStack ) =
{
    zlacConnect,
    zlccConnect
};

stlStatus (*gZlcDisconnectFunc[ZLC_PROTOCOL_TYPE_MAX]) ( zlcDbc        * aDbc,
                                                         stlErrorStack * aErrorStack ) =
{
    zlacDisconnect,
    zlccDisconnect
};

stlStatus (*gZlcSetAttrFunc[ZLC_PROTOCOL_TYPE_MAX]) ( zlcDbc        * aDbc,
                                                      stlInt32        aAttr,
                                                      stlInt32        aIntValue,
                                                      stlChar       * aStrValue,
                                                      stlErrorStack * aErrorStack ) =
{
    zlacSetAttr,
    zlccSetAttr
};
#endif

static dtlCharacterSet zlcGetCharSetIDFunc( void * aArgs )
{
    zlsStmt * sStmt = (zlsStmt*)aArgs;

    return sStmt->mParentDbc->mCharacterSet;
}

static dtlCharacterSet zlcGetNlsCharSetIDFunc( void * aArgs )
{
    zlsStmt * sStmt = (zlsStmt*)aArgs;

    return sStmt->mParentDbc->mNlsCharacterSet;
}

static stlInt32 zlcGetGMTOffsetFunc( void * aArgs )
{
    zlsStmt * sStmt = (zlsStmt*)aArgs;

    return (stlInt32)sStmt->mParentDbc->mTimezone;
}

static stlStatus zlcReallocLongVaryingMemFunc( void           * aArgs,
                                               stlInt32         aAllocSize,
                                               void          ** aAddr,
                                               stlErrorStack  * aErrorStack )
{
    zlsStmt * sStmt = (zlsStmt*)aArgs;
    void    * sOrgAddr = *aAddr;

    if( aAllocSize > DTL_LONGVARYING_INIT_STRING_SIZE )
    {
        STL_TRY( stlFreeDynamicMem( &sStmt->mLongVaryingMem,
                                    sOrgAddr,
                                    aErrorStack )
                 == STL_SUCCESS );

        STL_TRY( stlAllocDynamicMem( &sStmt->mLongVaryingMem,
                                     aAllocSize,
                                     aAddr,
                                     aErrorStack )
                 == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

static stlStatus zlcReallocAndMoveLongVaryingMemFunc( void           * aArgs,
                                                      dtlDataValue   * aDataValue,
                                                      stlInt32         aAllocSize,
                                                      stlErrorStack  * aErrorStack )
{
    zlsStmt * sStmt = (zlsStmt*)aArgs;
    void    * sSrcAddr = aDataValue->mValue;
    void    * sDstAddr;
    
    if( aAllocSize > DTL_LONGVARYING_INIT_STRING_SIZE )
    {
        STL_TRY( stlAllocDynamicMem( &sStmt->mLongVaryingMem,
                                     aAllocSize,
                                     &sDstAddr,
                                     aErrorStack )
                 == STL_SUCCESS );

        stlMemcpy( sDstAddr, sSrcAddr, aDataValue->mLength );
        
        STL_TRY( stlFreeDynamicMem( &sStmt->mLongVaryingMem,
                                    sSrcAddr,
                                    aErrorStack )
                 == STL_SUCCESS );

        aDataValue->mValue = sDstAddr;
        aDataValue->mBufferSize = aAllocSize;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

static stlChar * zlcGetDateFormatStringFunc( void * aArgs )
{
    zlsStmt * sStmt;
    zlcDbc  * sDbc;
    stlChar * sFormat = NULL;

    sStmt = (zlsStmt*)aArgs;
    sDbc  = sStmt->mParentDbc;

    if( sDbc->mDateFormat == NULL )
    {
        sFormat = DTL_DATE_FORMAT_FOR_ODBC;
    }
    else
    {
        STL_DASSERT( sDbc->mDateFormatInfo != NULL );

        sFormat = sDbc->mDateFormat;
    }

    return sFormat;
}

static dtlDateTimeFormatInfo * zlcGetDateFormatInfoFunc( void * aArgs )
{
    zlsStmt               * sStmt;
    zlcDbc                * sDbc;
    dtlDateTimeFormatInfo * sFormatInfo = NULL;

    sStmt = (zlsStmt*)aArgs;
    sDbc  = sStmt->mParentDbc;

    if( sDbc->mDateFormatInfo == NULL )
    {
        STL_DASSERT( sDbc->mDateFormat == NULL );
        
        sFormatInfo = gDtlNLSDateFormatInfoForODBC;
    }
    else
    {
        STL_DASSERT( sDbc->mDateFormat != NULL );
        
        sFormatInfo = (dtlDateTimeFormatInfo*)sDbc->mDateFormatInfo;
    }

    return sFormatInfo;
}

static stlChar * zlcGetTimeFormatStringFunc( void * aArgs )
{
    zlsStmt * sStmt;
    zlcDbc  * sDbc;
    stlChar * sFormat = NULL;

    sStmt = (zlsStmt*)aArgs;
    sDbc  = sStmt->mParentDbc;

    if( sDbc->mTimeFormat == NULL )
    {
        sFormat = DTL_TIME_FORMAT_FOR_ODBC;
    }
    else
    {
        STL_DASSERT( sDbc->mTimeFormatInfo != NULL );
        
        sFormat = sDbc->mTimeFormat;
    }

    return sFormat;
}

static dtlDateTimeFormatInfo * zlcGetTimeFormatInfoFunc( void * aArgs )
{
    zlsStmt               * sStmt;
    zlcDbc                * sDbc;
    dtlDateTimeFormatInfo * sFormatInfo = NULL;

    sStmt = (zlsStmt*)aArgs;
    sDbc  = sStmt->mParentDbc;

    if( sDbc->mTimeFormatInfo == NULL )
    {
        STL_DASSERT( sDbc->mTimeFormat == NULL );
        
        sFormatInfo = gDtlNLSTimeFormatInfoForODBC;
    }
    else
    {
        STL_DASSERT( sDbc->mTimeFormat != NULL );
        
        sFormatInfo = (dtlDateTimeFormatInfo*)sDbc->mTimeFormatInfo;
    }

    return sFormatInfo;
}

static stlChar * zlcGetTimeWithTimeZoneFormatStringFunc( void * aArgs )
{
    zlsStmt * sStmt;
    zlcDbc  * sDbc;
    stlChar * sFormat = NULL;

    sStmt = (zlsStmt*)aArgs;
    sDbc  = sStmt->mParentDbc;

    if( sDbc->mTimeWithTimezoneFormat == NULL )
    {
        sFormat = DTL_TIME_WITH_TIME_ZONE_FORMAT_FOR_ODBC;
    }
    else
    {
        STL_DASSERT( sDbc->mTimeWithTimezoneFormatInfo != NULL );
        
        sFormat = sDbc->mTimeWithTimezoneFormat;
    }

    return sFormat;
}

static dtlDateTimeFormatInfo * zlcGetTimeWithTimeZoneFormatInfoFunc( void * aArgs )
{
    zlsStmt               * sStmt;
    zlcDbc                * sDbc;
    dtlDateTimeFormatInfo * sFormatInfo = NULL;

    sStmt = (zlsStmt*)aArgs;
    sDbc  = sStmt->mParentDbc;

    if( sDbc->mTimeWithTimezoneFormatInfo == NULL )
    {
        STL_DASSERT( sDbc->mTimeWithTimezoneFormat == NULL );
        
        sFormatInfo = gDtlNLSTimeWithTimeZoneFormatInfoForODBC;
    }
    else
    {
        STL_DASSERT( sDbc->mTimeWithTimezoneFormat != NULL );
        
        sFormatInfo = (dtlDateTimeFormatInfo*)sDbc->mTimeWithTimezoneFormatInfo;
    }

    return sFormatInfo;
}

static stlChar * zlcGetTimestampFormatStringFunc( void * aArgs )
{
    zlsStmt * sStmt;
    zlcDbc  * sDbc;
    stlChar * sFormat = NULL;

    sStmt = (zlsStmt*)aArgs;
    sDbc  = sStmt->mParentDbc;

    if( sDbc->mTimestampFormat == NULL )
    {
        sFormat = DTL_TIMESTAMP_FORMAT_FOR_ODBC;
    }
    else
    {
        STL_DASSERT( sDbc->mTimestampFormatInfo != NULL );
        
        sFormat = sDbc->mTimestampFormat;
    }

    return sFormat;
}

static dtlDateTimeFormatInfo * zlcGetTimestampFormatInfoFunc( void * aArgs )
{
    zlsStmt               * sStmt;
    zlcDbc                * sDbc;
    dtlDateTimeFormatInfo * sFormatInfo = NULL;

    sStmt = (zlsStmt*)aArgs;
    sDbc  = sStmt->mParentDbc;

    if( sDbc->mTimestampFormatInfo == NULL )
    {
        STL_DASSERT( sDbc->mTimestampFormat == NULL );
        
        sFormatInfo = gDtlNLSTimestampFormatInfoForODBC;
    }
    else
    {
        STL_DASSERT( sDbc->mTimestampFormat != NULL );
        
        sFormatInfo = (dtlDateTimeFormatInfo*)sDbc->mTimestampFormatInfo;
    }

    return sFormatInfo;
}

static stlChar * zlcGetTimestampWithTimeZoneFormatStringFunc( void * aArgs )
{
    zlsStmt * sStmt;
    zlcDbc  * sDbc;
    stlChar * sFormat = NULL;

    sStmt = (zlsStmt*)aArgs;
    sDbc  = sStmt->mParentDbc;

    if( sDbc->mTimestampWithTimezoneFormat == NULL )
    {
        sFormat = DTL_TIMESTAMP_WITH_TIME_ZONE_FORMAT_FOR_ODBC;
    }
    else
    {
        STL_DASSERT( sDbc->mTimestampWithTimezoneFormatInfo != NULL );
        
        sFormat = sDbc->mTimestampWithTimezoneFormat;
    }

    return sFormat;
}

static dtlDateTimeFormatInfo * zlcGetTimestampWithTimeZoneFormatInfoFunc( void * aArgs )
{
    zlsStmt               * sStmt;
    zlcDbc                * sDbc;
    dtlDateTimeFormatInfo * sFormatInfo = NULL;

    sStmt = (zlsStmt*)aArgs;
    sDbc  = sStmt->mParentDbc;

    if( sDbc->mTimestampWithTimezoneFormatInfo == NULL )
    {
        STL_DASSERT( sDbc->mTimestampWithTimezoneFormat == NULL );
        
        sFormatInfo = gDtlNLSTimestampWithTimeZoneFormatInfoForODBC;
    }
    else
    {
        STL_DASSERT( sDbc->mTimestampWithTimezoneFormat != NULL );
        
        sFormatInfo = (dtlDateTimeFormatInfo*)sDbc->mTimestampWithTimezoneFormatInfo;
    }

    return sFormatInfo;
}


static stlStatus zlcSetDTFuncVector( zlcDbc * aDbc )
{
    dtlFuncVector * sVector = & aDbc->mDTVector;

    sVector->mGetCharSetIDFunc  = zlcGetCharSetIDFunc;
    sVector->mGetGMTOffsetFunc  = zlcGetGMTOffsetFunc;
    sVector->mReallocLongVaryingMemFunc = zlcReallocLongVaryingMemFunc;
    sVector->mReallocAndMoveLongVaryingMemFunc = zlcReallocAndMoveLongVaryingMemFunc;
    
    sVector->mGetDateFormatStringFunc = zlcGetDateFormatStringFunc;
    sVector->mGetDateFormatInfoFunc = zlcGetDateFormatInfoFunc;

    sVector->mGetTimeFormatStringFunc = zlcGetTimeFormatStringFunc;
    sVector->mGetTimeFormatInfoFunc = zlcGetTimeFormatInfoFunc;

    sVector->mGetTimeWithTimeZoneFormatStringFunc = zlcGetTimeWithTimeZoneFormatStringFunc;
    sVector->mGetTimeWithTimeZoneFormatInfoFunc = zlcGetTimeWithTimeZoneFormatInfoFunc;

    sVector->mGetTimestampFormatStringFunc = zlcGetTimestampFormatStringFunc;
    sVector->mGetTimestampFormatInfoFunc = zlcGetTimestampFormatInfoFunc;

    sVector->mGetTimestampWithTimeZoneFormatStringFunc = zlcGetTimestampWithTimeZoneFormatStringFunc;
    sVector->mGetTimestampWithTimeZoneFormatInfoFunc = zlcGetTimestampWithTimeZoneFormatInfoFunc;

    if( aDbc->mCharacterSet == aDbc->mNlsCharacterSet )
    {
        aDbc->mConvertSQLFunc    = zlvNonConvertSQL;
        aDbc->mConvertResultFunc = zlvNonConvertResultSet;;
        aDbc->mConvertErrorFunc  = zlvNonConvertError;;
    }
    else
    {
        aDbc->mConvertSQLFunc    = zlvConvertSQL;
        aDbc->mConvertResultFunc = zlvConvertResultSet;
        aDbc->mConvertErrorFunc  = zlvConvertError;
    }
    
    return STL_SUCCESS;
}

stlStatus zlcSetNlsDTFuncVector( zlcDbc * aDbc )
{
    dtlFuncVector * sVector = & aDbc->mNlsDTVector;

    sVector->mGetCharSetIDFunc  = zlcGetNlsCharSetIDFunc;
    sVector->mGetGMTOffsetFunc  = zlcGetGMTOffsetFunc;
    sVector->mReallocLongVaryingMemFunc = zlcReallocLongVaryingMemFunc;
    sVector->mReallocAndMoveLongVaryingMemFunc = zlcReallocAndMoveLongVaryingMemFunc;
    
    sVector->mGetDateFormatStringFunc = zlcGetDateFormatStringFunc;
    sVector->mGetDateFormatInfoFunc = zlcGetDateFormatInfoFunc;

    sVector->mGetTimeFormatStringFunc = zlcGetTimeFormatStringFunc;
    sVector->mGetTimeFormatInfoFunc = zlcGetTimeFormatInfoFunc;

    sVector->mGetTimeWithTimeZoneFormatStringFunc = zlcGetTimeWithTimeZoneFormatStringFunc;
    sVector->mGetTimeWithTimeZoneFormatInfoFunc = zlcGetTimeWithTimeZoneFormatInfoFunc;

    sVector->mGetTimestampFormatStringFunc = zlcGetTimestampFormatStringFunc;
    sVector->mGetTimestampFormatInfoFunc = zlcGetTimestampFormatInfoFunc;

    sVector->mGetTimestampWithTimeZoneFormatStringFunc = zlcGetTimestampWithTimeZoneFormatStringFunc;
    sVector->mGetTimestampWithTimeZoneFormatInfoFunc = zlcGetTimestampWithTimeZoneFormatInfoFunc;

    if( aDbc->mCharacterSet == aDbc->mNlsCharacterSet )
    {
        aDbc->mConvertSQLFunc    = zlvNonConvertSQL;
        aDbc->mConvertResultFunc = zlvNonConvertResultSet;;
        aDbc->mConvertErrorFunc  = zlvNonConvertError;;
    }
    else
    {
        aDbc->mConvertSQLFunc    = zlvConvertSQL;
        aDbc->mConvertResultFunc = zlvConvertResultSet;
        aDbc->mConvertErrorFunc  = zlvConvertError;
    }
    
    return STL_SUCCESS;
}

stlStatus zlcCheckSession( zlcDbc        * aDbc,
                           stlErrorStack * aErrorStack )
{
    STL_TRY_THROW( aDbc != NULL,
                   RAMP_ERR_COMMUNICATION_LINK_FAILURE );
    STL_TRY_THROW( aDbc->mSessionId != ZLC_INIT_SESSION_ID,
                   RAMP_ERR_COMMUNICATION_LINK_FAILURE );
    STL_TRY_THROW( aDbc->mBrokenConnection == STL_FALSE,
                   RAMP_ERR_COMMUNICATION_LINK_FAILURE);

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_COMMUNICATION_LINK_FAILURE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_COMMUNICATION_LINK_FAILURE,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

void zlcCheckSessionAndCleanup( zlcDbc * aDbc )
{
    if( aDbc == NULL )
    {
        STL_THROW( RAMP_IGNORE );
    }
    
    if( aDbc->mSessionId == ZLC_INIT_SESSION_ID )
    {
        STL_THROW( RAMP_IGNORE );
    }

    if( aDbc->mBrokenConnection == STL_TRUE )
    {
        if( STL_RING_IS_EMPTY( &aDbc->mStmtRing ) == STL_TRUE )
        {
            aDbc->mTransition = ZLC_TRANSITION_STATE_C4;
        }
        else
        {
            aDbc->mTransition = ZLC_TRANSITION_STATE_C5;
        }
    }

    STL_RAMP( RAMP_IGNORE );
}

stlStatus zlcAlloc( zlnEnv         * aEnv,
                    void          ** aDbc,
                    stlErrorStack  * aErrorStack )
{
    zlcDbc    * sDbc;
    stlInt32    sState = 0;
    stlUInt32   sOldValue;

    STL_TRY( stlAllocHeap( (void**)&sDbc,
                           STL_SIZEOF( zlcDbc ),
                           aErrorStack ) == STL_SUCCESS );
    stlMemset( sDbc, 0x00, STL_SIZEOF( zlcDbc ) );
    sState = 1;

    /*
     * DBC 기본 정보 설정
     */
    STL_TRY( zldDiagInit( SQL_HANDLE_DBC,
                          (void*)sDbc,
                          aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_RING_INIT_HEADLINK( &sDbc->mStmtRing,
                            STL_OFFSETOF(zlsStmt, mLink) );

    STL_RING_INIT_DATALINK( sDbc, STL_OFFSETOF( zlcDbc, mLink ) );

    /*
     * XA information 초기화
     */
    STL_RING_INIT_DATALINK( sDbc, STL_OFFSETOF( zlcDbc, mDbcNameLink ) );
    sDbc->mDbcName = NULL;
    sDbc->mRmId    = 0;

    while( 1 )
    {
        sOldValue = stlAtomicCas32( &aEnv->mAtomic, 1, 0 );

        if( sOldValue == 0 )
        {
            STL_RING_ADD_LAST( &aEnv->mDbcRing, sDbc );
            break;
        }

        stlYieldThread();
    }

    aEnv->mAtomic = 0;

    sDbc->mParentEnv            = aEnv;
    sDbc->mType                 = SQL_HANDLE_DBC;
    sDbc->mTransition           = ZLC_TRANSITION_STATE_C2;
    sDbc->mSessionId            = ZLC_INIT_SESSION_ID;
    sDbc->mSessionSeq           = ZLC_INIT_SESSION_SEQ;
    sDbc->mHasWithoutHoldCursor = STL_FALSE;    
    sDbc->mMetadataId           = STL_FALSE;
    sDbc->mCharacterSet         = DTL_CHARACTERSET_MAX;
    sDbc->mNlsCharacterSet      = DTL_CHARACTERSET_MAX;
    sDbc->mNlsNCharCharacterSet = DTL_CHARACTERSET_MAX;
    sDbc->mTimezone             = gZlcClientTimezone;
    sDbc->mDBTimezone           = 0;

    sDbc->mDateFormat                      = NULL;
    sDbc->mDateFormatInfo                  = NULL;
    sDbc->mTimeFormat                      = NULL;
    sDbc->mTimeFormatInfo                  = NULL;
    sDbc->mTimeWithTimezoneFormat          = NULL;
    sDbc->mTimeWithTimezoneFormatInfo      = NULL;
    sDbc->mTimestampFormat                 = NULL;
    sDbc->mTimestampFormatInfo             = NULL;
    sDbc->mTimestampWithTimezoneFormat     = NULL;
    sDbc->mTimestampWithTimezoneFormatInfo = NULL;

    sDbc->mAttrOldPwd           = NULL;
    sDbc->mAttrAutoCommit       = STL_TRUE;
    sDbc->mAttrAccessMode       = ZLL_ACCESS_MODE_UPDATABLE;
    sDbc->mAttrTxnIsolation     = ZLL_ISOLATION_LEVEL_READ_COMMITTED;
    sDbc->mAttrLoginTimeout     = 0;
    
    sDbc->mStmtAttrMaxRows      = 0;
    sDbc->mStmtPreFetchSize     = ZLS_DEFAULT_PREFETCH_SIZE;
    sDbc->mStmtStringLengthUnit = DTL_STRING_LENGTH_UNIT_CHARACTERS;

    STL_TRY( dtlGetSystemCharacterset( &sDbc->mCharacterSet, aErrorStack ) == STL_SUCCESS );

    STL_TRY( zlcSetDTFuncVector( sDbc ) == STL_SUCCESS );

    /*
     * Communication 설정
     */
    STL_TRY( cmlInitializeMemoryManager( &sDbc->mMemoryMgr,
                                         CML_HANDSHAKE_BUFFER_SIZE, /* PacketBufferSize */
                                         CML_BUFFER_SIZE,           /* OperationBufferSize */
                                         aErrorStack ) == STL_SUCCESS );
    sState = 3;

    STL_TRY( cmlInitializeHandle( &sDbc->mComm,
                                  &sDbc->mContext,
                                  STL_PLATFORM_ENDIAN,
                                  aErrorStack ) == STL_SUCCESS );
    sState = 4;

    STL_TRY( cmlInitializeProtocolSentence( &sDbc->mComm,
                                            &sDbc->mProtocolSentence,
                                            &sDbc->mMemoryMgr,
                                            aErrorStack ) == STL_SUCCESS );
    sState = 5;

    STL_TRY( stlCreateRegionAllocator( &sDbc->mAllocator,
                                       ZL_REGION_INIT_SIZE,
                                       ZL_REGION_NEXT_SIZE,
                                       aErrorStack ) == STL_SUCCESS );
    sState = 6;

    *aDbc = (void*)sDbc;

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 6:
            (void)stlDestroyRegionAllocator( &sDbc->mAllocator, aErrorStack );
        case 5:
            (void)cmlFinalizeProtocolSentence( &sDbc->mProtocolSentence, aErrorStack );
        case 4:
            (void)cmlFinalizeHandle( &sDbc->mComm, aErrorStack );
        case 3:
            (void)cmlFinalizeMemoryManager( &sDbc->mMemoryMgr, aErrorStack );
        case 2 :
            (void)zldDiagFini( SQL_HANDLE_DBC,
                               (void*)sDbc,
                               aErrorStack );
        case 1 :
            (void)stlFreeHeap( sDbc );
        default :
            break;
    }

    return STL_FAILURE;
}

stlStatus zlcFree( zlcDbc        * aDbc,
                   stlErrorStack * aErrorStack )
{
    zlnEnv    * sEnv;
    stlUInt32   sOldValue;

    sEnv = aDbc->mParentEnv;
    
    if( zlcCheckSession( aDbc, aErrorStack ) == STL_SUCCESS )
    {
        (void)zlcDisconnect( aDbc, aErrorStack );
    }

    while( 1 )
    {
        sOldValue = stlAtomicCas32( &sEnv->mAtomic, 1, 0 );

        if( sOldValue == 0 )
        {
            STL_RING_UNLINK( &sEnv->mDbcRing, aDbc );
            break;
        }

        stlYieldThread();
    }

    sEnv->mAtomic = 0;

    (void)stlDestroyRegionAllocator( &aDbc->mAllocator, aErrorStack );
    
    (void)zldDiagFini( SQL_HANDLE_DBC, (void*)aDbc, aErrorStack );

    (void)cmlFinalizeProtocolSentence( &aDbc->mProtocolSentence, aErrorStack );

    (void)cmlFinalizeHandle( &aDbc->mComm, aErrorStack );

    (void)cmlFinalizeMemoryManager( &aDbc->mMemoryMgr, aErrorStack );

    if( aDbc->mDbcName != NULL )
    {
        stlFreeHeap( aDbc->mDbcName );
    }

    if( aDbc->mServerName != NULL )
    {
        stlFreeHeap( aDbc->mServerName );
    }
    
    if( aDbc->mDateFormat != NULL )
    {
        stlFreeHeap( aDbc->mDateFormat );
    }
    
    if( aDbc->mDateFormatInfo != NULL )
    {
        stlFreeHeap( aDbc->mDateFormatInfo );
    }
    
    if( aDbc->mTimeFormat != NULL )
    {
        stlFreeHeap( aDbc->mTimeFormat );
    }
    
    if( aDbc->mTimeFormatInfo != NULL )
    {
        stlFreeHeap( aDbc->mTimeFormatInfo );
    }
    
    if( aDbc->mTimeWithTimezoneFormat != NULL )
    {
        stlFreeHeap( aDbc->mTimeWithTimezoneFormat );
    }
    
    if( aDbc->mTimeWithTimezoneFormatInfo != NULL )
    {
        stlFreeHeap( aDbc->mTimeWithTimezoneFormatInfo );
    }
    
    if( aDbc->mTimestampFormat != NULL )
    {
        stlFreeHeap( aDbc->mTimestampFormat );
    }
    
    if( aDbc->mTimestampFormatInfo != NULL )
    {
        stlFreeHeap( aDbc->mTimestampFormatInfo );
    }
    
    if( aDbc->mTimestampWithTimezoneFormat != NULL )
    {
        stlFreeHeap( aDbc->mTimestampWithTimezoneFormat );
    }
    
    if( aDbc->mTimestampWithTimezoneFormatInfo != NULL )
    {
        stlFreeHeap( aDbc->mTimestampWithTimezoneFormatInfo );
    }

    if( aDbc->mAttrOldPwd != NULL )
    {
        stlFreeHeap( aDbc->mAttrOldPwd );
    }
    
    stlMemset( aDbc, 0x00, STL_SIZEOF( zlcDbc ) );
    stlFreeHeap( aDbc );

    STL_INIT_ERROR_STACK( aErrorStack );
    
    return STL_SUCCESS;
}

static stlChar * zlcCheckAS( stlChar  * aAuthentication,
                             stlInt16   aLength,
                             stlInt16 * aPasswordLength,
                             stlBool  * aIsStartup )
{
    stlChar    sDelimeter[] = " ";
    stlChar  * sPos         = NULL;
    stlChar  * sSavePtr     = NULL;
    stlChar  * sRoleString  = NULL;
    stlChar  * sFence;

    *aIsStartup = STL_FALSE;

    sFence = aAuthentication + aLength;
    *aPasswordLength = aLength;    

    /*
     * 패스워드에서 AS 구문을 검사한다.
     */

    sPos = stlStrtok( aAuthentication, sDelimeter, &sSavePtr );

    if( sPos == NULL )
    {
        STL_THROW( RAMP_END );
    }

    if( stlStrcasecmp( sPos, "AS" ) == 0 )
    {
        aAuthentication[0] = '\0';
        *aPasswordLength = 0;
        
        sPos = stlStrtok( NULL, sDelimeter, &sSavePtr );

        if( sPos != NULL )
        {
            sRoleString = sPos;

            for( ; sPos < sFence ; sPos++ )
            {
                *sPos = stlToupper( *sPos );
            }
        }
            
        STL_THROW( RAMP_CHECK_WITH );
    }

    sPos = stlStrtok( NULL, sDelimeter, &sSavePtr );

    if( sPos == NULL )
    {
        STL_THROW( RAMP_END );
    }

    if( stlStrcasecmp( sPos, "AS" ) == 0 )
    {
        *aPasswordLength = stlStrlen(aAuthentication);

        sPos = stlStrtok( NULL, sDelimeter, &sSavePtr );

        if( sPos != NULL )
        {
            sRoleString = sPos;

            for( ; sPos < sFence ; sPos++ )
            {
                *sPos = stlToupper( *sPos );
            }
        }
    }

    STL_RAMP( RAMP_CHECK_WITH );

    if( sSavePtr[0] != '\0' )
    {
        sPos = stlStrtok( NULL, sDelimeter, &sSavePtr );
        if( sPos == NULL )
        {
            STL_THROW( RAMP_END );
        }

        if( stlStrcasecmp( sPos, "WITH" ) == 0 )
        {
            sPos = stlStrtok( NULL, sDelimeter, &sSavePtr );

            if( sPos != NULL )
            {
                if( stlStrcasecmp( sPos, "STARTUP" ) == 0 )
                {
                    *aIsStartup = STL_TRUE;
                }
            }
        }
    }

    STL_RAMP( RAMP_END );

    return sRoleString;
}

stlStatus zlcGetInfoFromDSN( zlcDbc          * aDbc,
                             stlChar         * aDSN,
                             stlChar         * aHost,
                             stlInt32        * aPort,
                             stlChar         * aUserName,
                             stlChar         * aPassword,
                             stlChar         * aCsMode,
                             zlcProtocolType * aProtocolType,
                             stlErrorStack   * aErrorStack )
{
    stlChar        sProperty[STL_MAX_INI_PROPERTY_VALUE];
    stlBool        sFoundDSN = STL_FALSE;
    stlInt64       sInt64Value;
    stlChar      * sEndPtr = NULL;

#ifndef WIN32
    stlBool        sFoundProperty = STL_FALSE;
    stlIniObject * sObject;

    aHost[0] = '\0';
    *aPort   = 0;

    /*
     * ODBC_USER_DSN
     */
    if( gZlnExistUserDsn == STL_TRUE )
    {
        sObject = NULL;
        
        STL_TRY( stlFindIniObject( &gZlnUserDsn,
                                   (stlChar*)aDSN,
                                   &sFoundDSN,
                                   &sObject,
                                   aErrorStack ) == STL_SUCCESS );

        if( sFoundDSN == STL_TRUE )
        {
#if defined( ODBC_ALL )
            STL_TRY( stlFindIniProperty( sObject,
                                         "PROTOCOL",
                                         sProperty,
                                         &sFoundProperty,
                                         aErrorStack ) == STL_SUCCESS );

            if( sFoundProperty == STL_TRUE )
            {
                if( stlStrcasecmp( sProperty, "DA" ) == 0 )
                {
                    *aProtocolType = ZLC_PROTOCOL_TYPE_DA;
                }
                else if( stlStrcasecmp( sProperty, "TCP" ) == 0 )
                {
                    *aProtocolType = ZLC_PROTOCOL_TYPE_TCP;
                }
            }
#endif
            
            STL_TRY( stlFindIniProperty( sObject,
                                         "HOST",
                                         aHost,
                                         &sFoundProperty,
                                         aErrorStack ) == STL_SUCCESS );

            STL_TRY( stlFindIniProperty( sObject,
                                         "PORT",
                                         sProperty,
                                         &sFoundProperty,
                                         aErrorStack ) == STL_SUCCESS );

            if( sFoundProperty == STL_TRUE )
            {
                STL_TRY( stlStrToInt64( sProperty,
                                        STL_MAX_INI_PROPERTY_VALUE,
                                        &sEndPtr,
                                        10,
                                        &sInt64Value,
                                        aErrorStack ) == STL_SUCCESS );

                *aPort = (stlInt32)sInt64Value;
            }

            STL_TRY( stlFindIniProperty( sObject,
                                         "UID",
                                         aUserName,
                                         &sFoundProperty,
                                         aErrorStack ) == STL_SUCCESS );

            STL_TRY( stlFindIniProperty( sObject,
                                         "PWD",
                                         aPassword,
                                         &sFoundProperty,
                                         aErrorStack ) == STL_SUCCESS );

            STL_TRY( stlFindIniProperty( sObject,
                                         "CS_MODE",
                                         aCsMode,
                                         &sFoundProperty,
                                         aErrorStack ) == STL_SUCCESS );

            STL_TRY( stlFindIniProperty( sObject,
                                         "MAX_ROWS",
                                         sProperty,
                                         &sFoundProperty,
                                         aErrorStack ) == STL_SUCCESS );

            if( sFoundProperty == STL_TRUE )
            {
                STL_TRY( stlStrToInt64( sProperty,
                                        STL_MAX_INI_PROPERTY_VALUE,
                                        &sEndPtr,
                                        10,
                                        &sInt64Value,
                                        aErrorStack ) == STL_SUCCESS );
                
                aDbc->mStmtAttrMaxRows = (SQLULEN)sInt64Value;
            }

            STL_TRY( stlFindIniProperty( sObject,
                                         "PREFETCH_ROWS",
                                         sProperty,
                                         &sFoundProperty,
                                         aErrorStack ) == STL_SUCCESS );

            if( sFoundProperty == STL_TRUE )
            {
                STL_TRY( stlStrToInt64( sProperty,
                                        STL_MAX_INI_PROPERTY_VALUE,
                                        &sEndPtr,
                                        10,
                                        &sInt64Value,
                                        aErrorStack ) == STL_SUCCESS );
                
                aDbc->mStmtPreFetchSize = (stlInt32)sInt64Value;
            }

            STL_TRY( stlFindIniProperty( sObject,
                                         "CHAR_LENGTH_UNITS",
                                         sProperty,
                                         &sFoundProperty,
                                         aErrorStack ) == STL_SUCCESS );

            if( sFoundProperty == STL_TRUE )
            {
                if( (stlStrcasecmp( sProperty, "BYTE" ) == 0) ||
                    (stlStrcasecmp( sProperty, "OCTETS" ) == 0) )
                {
                    aDbc->mStmtStringLengthUnit = DTL_STRING_LENGTH_UNIT_OCTETS;
                }
                else if( (stlStrcasecmp( sProperty, "CHAR" ) == 0) ||
                         (stlStrcasecmp( sProperty, "CHARACTERS" ) == 0) )
                {
                    aDbc->mStmtStringLengthUnit = DTL_STRING_LENGTH_UNIT_CHARACTERS;
                }
                else
                {
                    STL_THROW( RAMP_ERR_INVALID_ATTRIBUTE_VALUE );
                }
            }

            STL_TRY( stlFindIniProperty( sObject,
                                         "DATE_FORMAT",
                                         sProperty,
                                         &sFoundProperty,
                                         aErrorStack ) == STL_SUCCESS );

            if( sFoundProperty == STL_TRUE )
            {
                STL_TRY( zlcSetAttr( aDbc,
                                     SQL_ATTR_DATE_FORMAT,
                                     sProperty,
                                     stlStrlen(sProperty),
                                     aErrorStack ) == STL_SUCCESS );
            }

            STL_TRY( stlFindIniProperty( sObject,
                                         "TIME_FORMAT",
                                         sProperty,
                                         &sFoundProperty,
                                         aErrorStack ) == STL_SUCCESS );

            if( sFoundProperty == STL_TRUE )
            {
                STL_TRY( zlcSetAttr( aDbc,
                                     SQL_ATTR_TIME_FORMAT,
                                     sProperty,
                                     stlStrlen(sProperty),
                                     aErrorStack ) == STL_SUCCESS );
            }

            STL_TRY( stlFindIniProperty( sObject,
                                         "TIME_WITH_TIME_ZONE_FORMAT",
                                         sProperty,
                                         &sFoundProperty,
                                         aErrorStack ) == STL_SUCCESS );

            if( sFoundProperty == STL_TRUE )
            {
                STL_TRY( zlcSetAttr( aDbc,
                                     SQL_ATTR_TIME_WITH_TIMEZONE_FORMAT,
                                     sProperty,
                                     stlStrlen(sProperty),
                                     aErrorStack ) == STL_SUCCESS );
            }

            STL_TRY( stlFindIniProperty( sObject,
                                         "TIMESTAMP_FORMAT",
                                         sProperty,
                                         &sFoundProperty,
                                         aErrorStack ) == STL_SUCCESS );

            if( sFoundProperty == STL_TRUE )
            {
                STL_TRY( zlcSetAttr( aDbc,
                                     SQL_ATTR_TIMESTAMP_FORMAT,
                                     sProperty,
                                     stlStrlen(sProperty),
                                     aErrorStack ) == STL_SUCCESS );
            }

            STL_TRY( stlFindIniProperty( sObject,
                                         "TIMESTAMP_WITH_TIME_ZONE_FORMAT",
                                         sProperty,
                                         &sFoundProperty,
                                         aErrorStack ) == STL_SUCCESS );

            if( sFoundProperty == STL_TRUE )
            {
                STL_TRY( zlcSetAttr( aDbc,
                                     SQL_ATTR_TIMESTAMP_WITH_TIMEZONE_FORMAT,
                                     sProperty,
                                     stlStrlen(sProperty),
                                     aErrorStack ) == STL_SUCCESS );
            }
        }
    }

    /*
     * ODBC_SYSTEM_DSN
     */
    if( (sFoundDSN == STL_FALSE) && (gZlnExistSystemDsn == STL_TRUE) )
    {
        sObject = NULL;
        
        STL_TRY( stlFindIniObject( &gZlnSystemDsn,
                                   (stlChar*)aDSN,
                                   &sFoundDSN,
                                   &sObject,
                                   aErrorStack ) == STL_SUCCESS );

        if( sFoundDSN == STL_TRUE )
        {
#if defined( ODBC_ALL )
            STL_TRY( stlFindIniProperty( sObject,
                                         "PROTOCOL",
                                         sProperty,
                                         &sFoundProperty,
                                         aErrorStack ) == STL_SUCCESS );

            if( sFoundProperty == STL_TRUE )
            {
                if( stlStrcasecmp( sProperty, "DA" ) == 0 )
                {
                    *aProtocolType = ZLC_PROTOCOL_TYPE_DA;
                }
                else if( stlStrcasecmp( sProperty, "TCP" ) == 0 )
                {
                    *aProtocolType = ZLC_PROTOCOL_TYPE_TCP;
                }
            }
#endif
            
            STL_TRY( stlFindIniProperty( sObject,
                                         "HOST",
                                         aHost,
                                         &sFoundProperty,
                                         aErrorStack ) == STL_SUCCESS );

            STL_TRY( stlFindIniProperty( sObject,
                                         "PORT",
                                         sProperty,
                                         &sFoundProperty,
                                         aErrorStack ) == STL_SUCCESS );

            if( sFoundProperty == STL_TRUE )
            {
                STL_TRY( stlStrToInt64( sProperty,
                                        STL_MAX_INI_PROPERTY_VALUE,
                                        &sEndPtr,
                                        10,
                                        &sInt64Value,
                                        aErrorStack ) == STL_SUCCESS );

                *aPort = (stlInt32)sInt64Value;
            }

            STL_TRY( stlFindIniProperty( sObject,
                                         "UID",
                                         aUserName,
                                         &sFoundProperty,
                                         aErrorStack ) == STL_SUCCESS );

            STL_TRY( stlFindIniProperty( sObject,
                                         "PWD",
                                         aPassword,
                                         &sFoundProperty,
                                         aErrorStack ) == STL_SUCCESS );

            STL_TRY( stlFindIniProperty( sObject,
                                         "CS_MODE",
                                         aCsMode,
                                         &sFoundProperty,
                                         aErrorStack ) == STL_SUCCESS );

            STL_TRY( stlFindIniProperty( sObject,
                                         "MAX_ROWS",
                                         sProperty,
                                         &sFoundProperty,
                                         aErrorStack ) == STL_SUCCESS );

            if( sFoundProperty == STL_TRUE )
            {
                STL_TRY( stlStrToInt64( sProperty,
                                        STL_MAX_INI_PROPERTY_VALUE,
                                        &sEndPtr,
                                        10,
                                        &sInt64Value,
                                        aErrorStack ) == STL_SUCCESS );
                
                aDbc->mStmtAttrMaxRows = (SQLULEN)sInt64Value;
            }

            STL_TRY( stlFindIniProperty( sObject,
                                         "PREFETCH_ROWS",
                                         sProperty,
                                         &sFoundProperty,
                                         aErrorStack ) == STL_SUCCESS );

            if( sFoundProperty == STL_TRUE )
            {
                STL_TRY( stlStrToInt64( sProperty,
                                        STL_MAX_INI_PROPERTY_VALUE,
                                        &sEndPtr,
                                        10,
                                        &sInt64Value,
                                        aErrorStack ) == STL_SUCCESS );
                
                aDbc->mStmtPreFetchSize = (stlInt32)sInt64Value;
            }

            STL_TRY( stlFindIniProperty( sObject,
                                         "CHAR_LENGTH_UNITS",
                                         sProperty,
                                         &sFoundProperty,
                                         aErrorStack ) == STL_SUCCESS );

            if( sFoundProperty == STL_TRUE )
            {
                if( (stlStrcasecmp( sProperty, "BYTE" ) == 0) ||
                    (stlStrcasecmp( sProperty, "OCTETS" ) == 0) )
                {
                    aDbc->mStmtStringLengthUnit = DTL_STRING_LENGTH_UNIT_OCTETS;
                }
                else if( (stlStrcasecmp( sProperty, "CHAR" ) == 0) ||
                         (stlStrcasecmp( sProperty, "CHARACTERS" ) == 0) )
                {
                    aDbc->mStmtStringLengthUnit = DTL_STRING_LENGTH_UNIT_CHARACTERS;
                }
                else
                {
                    STL_THROW( RAMP_ERR_INVALID_ATTRIBUTE_VALUE );
                }
            }

            STL_TRY( stlFindIniProperty( sObject,
                                         "DATE_FORMAT",
                                         sProperty,
                                         &sFoundProperty,
                                         aErrorStack ) == STL_SUCCESS );

            if( sFoundProperty == STL_TRUE )
            {
                STL_TRY( zlcSetAttr( aDbc,
                                     SQL_ATTR_DATE_FORMAT,
                                     sProperty,
                                     stlStrlen(sProperty),
                                     aErrorStack ) == STL_SUCCESS );
            }

            STL_TRY( stlFindIniProperty( sObject,
                                         "TIME_FORMAT",
                                         sProperty,
                                         &sFoundProperty,
                                         aErrorStack ) == STL_SUCCESS );

            if( sFoundProperty == STL_TRUE )
            {
                STL_TRY( zlcSetAttr( aDbc,
                                     SQL_ATTR_TIME_FORMAT,
                                     sProperty,
                                     stlStrlen(sProperty),
                                     aErrorStack ) == STL_SUCCESS );
            }

            STL_TRY( stlFindIniProperty( sObject,
                                         "TIME_WITH_TIME_ZONE_FORMAT",
                                         sProperty,
                                         &sFoundProperty,
                                         aErrorStack ) == STL_SUCCESS );

            if( sFoundProperty == STL_TRUE )
            {
                STL_TRY( zlcSetAttr( aDbc,
                                     SQL_ATTR_TIME_WITH_TIMEZONE_FORMAT,
                                     sProperty,
                                     stlStrlen(sProperty),
                                     aErrorStack ) == STL_SUCCESS );
            }

            STL_TRY( stlFindIniProperty( sObject,
                                         "TIMESTAMP_FORMAT",
                                         sProperty,
                                         &sFoundProperty,
                                         aErrorStack ) == STL_SUCCESS );

            if( sFoundProperty == STL_TRUE )
            {
                STL_TRY( zlcSetAttr( aDbc,
                                     SQL_ATTR_TIMESTAMP_FORMAT,
                                     sProperty,
                                     stlStrlen(sProperty),
                                     aErrorStack ) == STL_SUCCESS );
            }

            STL_TRY( stlFindIniProperty( sObject,
                                         "TIMESTAMP_WITH_TIME_ZONE_FORMAT",
                                         sProperty,
                                         &sFoundProperty,
                                         aErrorStack ) == STL_SUCCESS );

            if( sFoundProperty == STL_TRUE )
            {
                STL_TRY( zlcSetAttr( aDbc,
                                     SQL_ATTR_TIMESTAMP_WITH_TIMEZONE_FORMAT,
                                     sProperty,
                                     stlStrlen(sProperty),
                                     aErrorStack ) == STL_SUCCESS );
            }
        }
    }
#else
    LONG    sResult;
    HKEY    sKey;
    DWORD   sType;
    DWORD   sByte;
    stlChar sSubKey[STL_MAX_INI_PROPERTY_VALUE];

    /*
     * ODBC_USER_DSN
     */

    stlSnprintf(sSubKey, STL_MAX_INI_PROPERTY_VALUE, "Software\\ODBC\\ODBC.INI\\%s", (stlChar*)aDSN);

    sResult = RegOpenKeyEx(HKEY_CURRENT_USER,
                           sSubKey,
                           0,
                           KEY_ALL_ACCESS,
                           &sKey);

    if (sResult == ERROR_SUCCESS)
    {
	sFoundDSN = STL_TRUE;

#if defined( ODBC_ALL )
        sByte = STL_MAX_INI_PROPERTY_VALUE;
	sResult = RegQueryValueEx( sKey, "PROTOCOL", 0, &sType, (LPBYTE)sProperty, &sByte);

	if(sResult == ERROR_SUCCESS)
	{
            if (stlStrcasecmp(sProperty, "DA") == 0)
            {
                *aProtocolType = ZLC_PROTOCOL_TYPE_DA;
            }
            else if (stlStrcasecmp(sProperty, "TCP") == 0)
            {
                *aProtocolType = ZLC_PROTOCOL_TYPE_TCP;
            }
	}
#endif

        sByte = STL_MAX_INI_PROPERTY_VALUE;
	sResult = RegQueryValueEx(sKey, "HOST", 0, &sType, (LPBYTE)aHost, &sByte);

        sByte = STL_MAX_INI_PROPERTY_VALUE;
	sResult = RegQueryValueEx(sKey, "PORT", 0, &sType, (LPBYTE)sProperty, &sByte);

	if (sResult == ERROR_SUCCESS)
	{
            STL_TRY(stlStrToInt64(sProperty,
                                  STL_MAX_INI_PROPERTY_VALUE,
                                  &sEndPtr,
                                  10,
                                  &sInt64Value,
                                  aErrorStack) == STL_SUCCESS);

            *aPort = (stlInt32)sInt64Value;
	}

        sByte = STL_MAX_INI_PROPERTY_VALUE;
	sResult = RegQueryValueEx(sKey, "UID", 0, &sType, (LPBYTE)aUserName, &sByte);

        sByte = STL_MAX_INI_PROPERTY_VALUE;
	sResult = RegQueryValueEx(sKey, "PWD", 0, &sType, (LPBYTE)aPassword, &sByte);

        sByte = STL_MAX_INI_PROPERTY_VALUE;
	sResult = RegQueryValueEx(sKey, "CS_MODE", 0, &sType, (LPBYTE)aCsMode, &sByte);

        sByte = STL_MAX_INI_PROPERTY_VALUE;
	sResult = RegQueryValueEx(sKey, "MAX_ROWS", 0, &sType, (LPBYTE)sProperty, &sByte);

	if (sResult == ERROR_SUCCESS)
	{
            STL_TRY(stlStrToInt64(sProperty,
                                  STL_MAX_INI_PROPERTY_VALUE,
                                  &sEndPtr,
                                  10,
                                  &sInt64Value,
                                  aErrorStack) == STL_SUCCESS);

            aDbc->mStmtAttrMaxRows = (SQLULEN)sInt64Value;
	}

        sByte = STL_MAX_INI_PROPERTY_VALUE;
	sResult = RegQueryValueEx(sKey, "PREFETCH_ROWS", 0, &sType, (LPBYTE)sProperty, &sByte);

	if (sResult == ERROR_SUCCESS)
	{
            STL_TRY(stlStrToInt64(sProperty,
                                  STL_MAX_INI_PROPERTY_VALUE,
                                  &sEndPtr,
                                  10,
                                  &sInt64Value,
                                  aErrorStack) == STL_SUCCESS);

            aDbc->mStmtPreFetchSize = (stlInt32)sInt64Value;
	}

        sByte = STL_MAX_INI_PROPERTY_VALUE;
	sResult = RegQueryValueEx(sKey, "CHAR_LENGTH_UNITS", 0, &sType, (LPBYTE)sProperty, &sByte);

	if (sResult == ERROR_SUCCESS)
	{
            if ((stlStrcasecmp(sProperty, "BYTE") == 0) ||
                (stlStrcasecmp(sProperty, "OCTETS") == 0))
            {
                aDbc->mStmtStringLengthUnit = DTL_STRING_LENGTH_UNIT_OCTETS;
            }
            else if ((stlStrcasecmp(sProperty, "CHAR") == 0) ||
                     (stlStrcasecmp(sProperty, "CHARACTERS") == 0))
            {
                aDbc->mStmtStringLengthUnit = DTL_STRING_LENGTH_UNIT_CHARACTERS;
            }
            else
            {
                STL_THROW(RAMP_ERR_INVALID_ATTRIBUTE_VALUE);
            }
	}

        sByte = STL_MAX_INI_PROPERTY_VALUE;
	sResult = RegQueryValueEx(sKey, "DATE_FORMAT", 0, &sType, (LPBYTE)sProperty, &sByte);

	if (sResult == ERROR_SUCCESS)
	{
            STL_TRY(zlcSetAttr(aDbc,
                               SQL_ATTR_DATE_FORMAT,
                               sProperty,
                               stlStrlen(sProperty),
                               aErrorStack) == STL_SUCCESS);
	}

        sByte = STL_MAX_INI_PROPERTY_VALUE;
	sResult = RegQueryValueEx(sKey, "TIME_FORMAT", 0, &sType, (LPBYTE)sProperty, &sByte);

	if (sResult == ERROR_SUCCESS)
	{
            STL_TRY(zlcSetAttr(aDbc,
                               SQL_ATTR_TIME_FORMAT,
                               sProperty,
                               stlStrlen(sProperty),
                               aErrorStack) == STL_SUCCESS);
	}

        sByte = STL_MAX_INI_PROPERTY_VALUE;
	sResult = RegQueryValueEx(sKey, "TIME_WITH_TIME_ZONE_FORMAT", 0, &sType, (LPBYTE)sProperty, &sByte);

	if (sResult == ERROR_SUCCESS)
	{
            STL_TRY(zlcSetAttr(aDbc,
                               SQL_ATTR_TIME_WITH_TIMEZONE_FORMAT,
                               sProperty,
                               stlStrlen(sProperty),
                               aErrorStack) == STL_SUCCESS);
	}

        sByte = STL_MAX_INI_PROPERTY_VALUE;
	sResult = RegQueryValueEx(sKey, "TIMESTAMP_FORMAT", 0, &sType, (LPBYTE)sProperty, &sByte);

	if (sResult == ERROR_SUCCESS)
	{
            STL_TRY(zlcSetAttr(aDbc,
                               SQL_ATTR_TIMESTAMP_FORMAT,
                               sProperty,
                               stlStrlen(sProperty),
                               aErrorStack) == STL_SUCCESS);
	}

        sByte = STL_MAX_INI_PROPERTY_VALUE;
	sResult = RegQueryValueEx(sKey, "TIMESTAMP_WITH_TIME_ZONE_FORMAT", 0, &sType, (LPBYTE)sProperty, &sByte);

	if (sResult == ERROR_SUCCESS)
	{
            STL_TRY(zlcSetAttr(aDbc,
                               SQL_ATTR_TIMESTAMP_WITH_TIMEZONE_FORMAT,
                               sProperty,
                               stlStrlen(sProperty),
                               aErrorStack) == STL_SUCCESS);
	}
    }

    /*
     * ODBC_SYSTEM_DSN
     */

    if (sFoundDSN == STL_FALSE)
    {
#if (STL_SIZEOF_VOIDP == 4 )        
        stlSnprintf(sSubKey, STL_MAX_INI_PROPERTY_VALUE, "Software\\Wow6432Node\\ODBC\\ODBC.INI\\%s", (stlChar*)aDSN);
#else
        stlSnprintf(sSubKey, STL_MAX_INI_PROPERTY_VALUE, "Software\\ODBC\\ODBC.INI\\%s", (stlChar*)aDSN);
#endif

	sResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                               sSubKey,
                               0,
                               KEY_ALL_ACCESS,
                               &sKey);

	if (sResult == ERROR_SUCCESS)
	{
            sFoundDSN = STL_TRUE;

#if defined( ODBC_ALL )
            sByte = STL_MAX_INI_PROPERTY_VALUE;
            sResult = RegQueryValueEx(sKey, "PROTOCOL", 0, &sType, (LPBYTE)sProperty, &sByte);

            if (sResult == ERROR_SUCCESS)
            {
                if (stlStrcasecmp(sProperty, "DA") == 0)
                {
                    *aProtocolType = ZLC_PROTOCOL_TYPE_DA;
                }
                else if (stlStrcasecmp(sProperty, "TCP") == 0)
                {
                    *aProtocolType = ZLC_PROTOCOL_TYPE_TCP;
                }
            }
#endif

            sByte = STL_MAX_INI_PROPERTY_VALUE;
            sResult = RegQueryValueEx(sKey, "HOST", 0, &sType, (LPBYTE)aHost, &sByte);

            sByte = STL_MAX_INI_PROPERTY_VALUE;
            sResult = RegQueryValueEx(sKey, "PORT", 0, &sType, (LPBYTE)sProperty, &sByte);

            if (sResult == ERROR_SUCCESS)
            {
                STL_TRY(stlStrToInt64(sProperty,
                                      STL_MAX_INI_PROPERTY_VALUE,
                                      &sEndPtr,
                                      10,
                                      &sInt64Value,
                                      aErrorStack) == STL_SUCCESS);

                *aPort = (stlInt32)sInt64Value;
            }

            sByte = STL_MAX_INI_PROPERTY_VALUE;
            sResult = RegQueryValueEx(sKey, "UID", 0, &sType, (LPBYTE)aUserName, &sByte);

            sByte = STL_MAX_INI_PROPERTY_VALUE;
            sResult = RegQueryValueEx(sKey, "PWD", 0, &sType, (LPBYTE)aPassword, &sByte);

            sByte = STL_MAX_INI_PROPERTY_VALUE;
            sResult = RegQueryValueEx(sKey, "CS_MODE", 0, &sType, (LPBYTE)aCsMode, &sByte);

            sByte = STL_MAX_INI_PROPERTY_VALUE;
            sResult = RegQueryValueEx(sKey, "MAX_ROWS", 0, &sType, (LPBYTE)sProperty, &sByte);

            if (sResult == ERROR_SUCCESS)
            {
                STL_TRY(stlStrToInt64(sProperty,
                                      STL_MAX_INI_PROPERTY_VALUE,
                                      &sEndPtr,
                                      10,
                                      &sInt64Value,
                                      aErrorStack) == STL_SUCCESS);

                aDbc->mStmtAttrMaxRows = (SQLULEN)sInt64Value;
            }

            sByte = STL_MAX_INI_PROPERTY_VALUE;
            sResult = RegQueryValueEx(sKey, "PREFETCH_ROWS", 0, &sType, (LPBYTE)sProperty, &sByte);

            if (sResult == ERROR_SUCCESS)
            {
                STL_TRY(stlStrToInt64(sProperty,
                                      STL_MAX_INI_PROPERTY_VALUE,
                                      &sEndPtr,
                                      10,
                                      &sInt64Value,
                                      aErrorStack) == STL_SUCCESS);

                aDbc->mStmtPreFetchSize = (stlInt32)sInt64Value;
            }

            sByte = STL_MAX_INI_PROPERTY_VALUE;
            sResult = RegQueryValueEx(sKey, "CHAR_LENGTH_UNITS", 0, &sType, (LPBYTE)sProperty, &sByte);

            if (sResult == ERROR_SUCCESS)
            {
                if ((stlStrcasecmp(sProperty, "BYTE") == 0) ||
                    (stlStrcasecmp(sProperty, "OCTETS") == 0))
                {
                    aDbc->mStmtStringLengthUnit = DTL_STRING_LENGTH_UNIT_OCTETS;
                }
                else if ((stlStrcasecmp(sProperty, "CHAR") == 0) ||
                         (stlStrcasecmp(sProperty, "CHARACTERS") == 0))
                {
                    aDbc->mStmtStringLengthUnit = DTL_STRING_LENGTH_UNIT_CHARACTERS;
                }
                else
                {
                    STL_THROW(RAMP_ERR_INVALID_ATTRIBUTE_VALUE);
                }
            }

            sByte = STL_MAX_INI_PROPERTY_VALUE;
            sResult = RegQueryValueEx(sKey, "DATE_FORMAT", 0, &sType, (LPBYTE)sProperty, &sByte);

            if (sResult == ERROR_SUCCESS)
            {
                STL_TRY(zlcSetAttr(aDbc,
                                   SQL_ATTR_DATE_FORMAT,
                                   sProperty,
                                   stlStrlen(sProperty),
                                   aErrorStack) == STL_SUCCESS);
            }

            sByte = STL_MAX_INI_PROPERTY_VALUE;
            sResult = RegQueryValueEx(sKey, "TIME_FORMAT", 0, &sType, (LPBYTE)sProperty, &sByte);

            if (sResult == ERROR_SUCCESS)
            {
                STL_TRY(zlcSetAttr(aDbc,
                                   SQL_ATTR_TIME_FORMAT,
                                   sProperty,
                                   stlStrlen(sProperty),
                                   aErrorStack) == STL_SUCCESS);
            }

            sByte = STL_MAX_INI_PROPERTY_VALUE;
            sResult = RegQueryValueEx(sKey, "TIME_WITH_TIME_ZONE_FORMAT", 0, &sType, (LPBYTE)sProperty, &sByte);

            if (sResult == ERROR_SUCCESS)
            {
                STL_TRY(zlcSetAttr(aDbc,
                                   SQL_ATTR_TIME_WITH_TIMEZONE_FORMAT,
                                   sProperty,
                                   stlStrlen(sProperty),
                                   aErrorStack) == STL_SUCCESS);
            }

            sByte = STL_MAX_INI_PROPERTY_VALUE;
            sResult = RegQueryValueEx(sKey, "TIMESTAMP_FORMAT", 0, &sType, (LPBYTE)sProperty, &sByte);

            if (sResult == ERROR_SUCCESS)
            {
                STL_TRY(zlcSetAttr(aDbc,
                                   SQL_ATTR_TIMESTAMP_FORMAT,
                                   sProperty,
                                   stlStrlen(sProperty),
                                   aErrorStack) == STL_SUCCESS);
            }

            sByte = STL_MAX_INI_PROPERTY_VALUE;
            sResult = RegQueryValueEx(sKey, "TIMESTAMP_WITH_TIME_ZONE_FORMAT", 0, &sType, (LPBYTE)sProperty, &sByte);

            if (sResult == ERROR_SUCCESS)
            {
                STL_TRY(zlcSetAttr(aDbc,
                                   SQL_ATTR_TIMESTAMP_WITH_TIMEZONE_FORMAT,
                                   sProperty,
                                   stlStrlen(sProperty),
                                   aErrorStack) == STL_SUCCESS);
            }
	}
    }

#endif

    if( *aProtocolType == ZLC_PROTOCOL_TYPE_TCP )
    {
        STL_TRY_THROW( sFoundDSN == STL_TRUE, RAMP_ERR_CLIENT_UNABLE_TO_ESTABLISH_CONNECTION );
    }

    if( aDbc->mServerName != NULL )
    {
        stlFreeHeap( aDbc->mServerName );
        aDbc->mServerName = NULL;
    }

    if( sFoundDSN == STL_TRUE )
    {
        STL_TRY( stlAllocHeap( (void**)&aDbc->mServerName,
                               stlStrlen( aDSN ) + 1,
                               aErrorStack ) == STL_SUCCESS );

        stlStrcpy( aDbc->mServerName, aDSN );
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_CLIENT_UNABLE_TO_ESTABLISH_CONNECTION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_CLIENT_UNABLE_TO_ESTABLISH_CONNECTION,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_ATTRIBUTE_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_ATTRIBUTE_VALUE,
                      NULL,
                      aErrorStack );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zlcConnect( zlcDbc          * aDbc,
                      stlChar         * aHost,
                      stlInt32          aPort,
                      zlcProtocolType   aProtocolType,
                      stlChar         * aCsMode,
                      stlChar         * aUser,
                      stlInt16          aUserLen,
                      stlChar         * aPassword,
                      stlInt16          aPasswordLen,
                      stlBool         * aSuccessWithInfo,
                      stlErrorStack   * aErrorStack )
{
    stlChar         sUser[STL_MAX_SQL_IDENTIFIER_LENGTH + 1];
    stlChar         sEncryptedPassword[STL_MAX_SQL_IDENTIFIER_LENGTH + 1];
    stlChar         sRole[STL_MAX_SQL_IDENTIFIER_LENGTH + 1];
    stlProc         sProc;
    stlChar         sProgram[STL_PATH_MAX + 1];

    stlChar       * sRoleString;
    stlBool         sIsStartup = STL_FALSE;

    stlChar         sPasswordBuffer[STL_MAX_SQL_IDENTIFIER_LENGTH + 1];
    stlInt16        sPasswordLen = 0;
    
    stlChar       * sOldPassword = NULL;
    stlChar       * sNewPassword = NULL;

    stlUInt32       i;

    STL_PARAM_VALIDATE( (aProtocolType == ZLC_PROTOCOL_TYPE_DA ) ||
                        (aProtocolType == ZLC_PROTOCOL_TYPE_TCP )  , aErrorStack );
    
    STL_PARAM_VALIDATE( aUserLen <= STL_MAX_SQL_IDENTIFIER_LENGTH, aErrorStack );

    *aSuccessWithInfo = STL_FALSE;

    stlMemset( sUser, 0x00, STL_MAX_SQL_IDENTIFIER_LENGTH + 1 );
    stlMemset( sEncryptedPassword, 0x00, STL_MAX_SQL_IDENTIFIER_LENGTH + 1 );
    stlMemset( sRole, 0x00, STL_MAX_SQL_IDENTIFIER_LENGTH + 1 );

    STL_TRY( stlThreadSelf( &aDbc->mThread, aErrorStack ) == STL_SUCCESS );

    STL_TRY( stlGetCurrentProc( &sProc, aErrorStack ) == STL_SUCCESS );

    STL_TRY( stlGetProcName( &sProc,
                             sProgram,
                             STL_PATH_MAX,
                             aErrorStack ) == STL_SUCCESS );

    /*
     * ROLE
     */

    sRoleString = zlcCheckAS( aPassword, aPasswordLen, &aPasswordLen, &sIsStartup );

    if( sRoleString == NULL )
    {
        STL_TRY_THROW( aUserLen > 0, RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH );
    }
    else
    {
        STL_PARAM_VALIDATE( stlStrlen( sRoleString ) <= STL_MAX_SQL_IDENTIFIER_LENGTH, aErrorStack );

        stlStrcpy( sRole, sRoleString );
    }

    if( aUserLen > 0 )
    {
        if( ( aUser[0] == '\"' ) &&
            ( aUser[ aUserLen - 1 ] == '\"' ) )
        {
            for( i = 0; i < aUserLen - 2; i++ )
            {
                sUser[i] = aUser[ i + 1 ];
            }
        }
        else
        {
            for( i = 0; i < aUserLen; i++ )
            {
                sUser[i] = stlToupper( aUser[i] );
            }
        }
    }

    if( aDbc->mAttrOldPwd != NULL )
    {
        sPasswordLen = stlStrlen( aDbc->mAttrOldPwd );
        if( ( aDbc->mAttrOldPwd[0] == '\"' ) &&
            ( aDbc->mAttrOldPwd[ sPasswordLen - 1 ] == '\"' ) )
        {
            for( i = 0; i < sPasswordLen - 2; i++ )
            {
                sPasswordBuffer[i] = aDbc->mAttrOldPwd[ i + 1 ];
            }
            sPasswordLen = sPasswordLen - 2;
            sPasswordBuffer[sPasswordLen] = '\0';
        }
        else
        {
            stlMemcpy( sPasswordBuffer, aDbc->mAttrOldPwd, sPasswordLen );
            sPasswordBuffer[sPasswordLen] = '\0';
        }
        
        sOldPassword = sPasswordBuffer;
        sNewPassword = aPassword;
    }
    else
    {
        if( aPasswordLen > 0 )
        {
            if( ( aPassword[0] == '\"' ) &&
                ( aPassword[ aPasswordLen - 1 ] == '\"' ) )
            {
                for( i = 0; i < aPasswordLen - 2; i++ )
                {
                    sPasswordBuffer[i] = aPassword[ i + 1 ];
                }
                sPasswordLen = aPasswordLen - 2;
                sPasswordBuffer[sPasswordLen] = '\0';
            }
            else
            {
                stlMemcpy( sPasswordBuffer, aPassword, aPasswordLen );
                sPasswordLen = aPasswordLen;
                sPasswordBuffer[sPasswordLen] = '\0';
            }
            
            /*
             * Encrypted Password 생성
             */

            STL_TRY( stlMakeEncryptedPassword( sPasswordBuffer,
                                               sPasswordLen,
                                               sEncryptedPassword,
                                               STL_MAX_SQL_IDENTIFIER_LENGTH,
                                               aErrorStack ) == STL_SUCCESS );
        }
    }

#if defined( ODBC_ALL )
    STL_TRY( gZlcConnectFunc[aProtocolType]( aDbc,
                                             aHost,
                                             aPort,
                                             aCsMode,
                                             sUser,
                                             sEncryptedPassword,
                                             sRole,
                                             sOldPassword,
                                             sNewPassword,
                                             sIsStartup,
                                             sProc,
                                             sProgram,
                                             aSuccessWithInfo,
                                             aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_DA )
    STL_TRY( zlacConnect( aDbc,
                          aHost,
                          aPort,
                          aCsMode,
                          sUser,
                          sEncryptedPassword,
                          sRole,
                          sOldPassword,
                          sNewPassword,
                          sIsStartup,
                          sProc,
                          sProgram,
                          aSuccessWithInfo,
                          aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_CS )
    STL_TRY( zlccConnect( aDbc,
                          aHost,
                          aPort,
                          aCsMode,
                          sUser,
                          sEncryptedPassword,
                          sRole,
                          sOldPassword,
                          sNewPassword,
                          sIsStartup,
                          sProc,
                          sProgram,
                          aSuccessWithInfo,
                          aErrorStack ) == STL_SUCCESS );
#else
    STL_ASSERT( STL_FALSE );
#endif
    
    STL_TRY( zlcSetNlsDTFuncVector( aDbc ) == STL_SUCCESS );

    aDbc->mProtocolType     = aProtocolType;
    aDbc->mTransition       = ZLC_TRANSITION_STATE_C4;
    aDbc->mBrokenConnection = STL_FALSE;

    if( aDbc->mAttrOldPwd != NULL )
    {
        stlFreeHeap( aDbc->mAttrOldPwd );
        aDbc->mAttrOldPwd = NULL;
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_STRING_OR_BUFFER_LENGTH,
                      "The value specified for argument NameLength1, NameLength2, or "
                      "NameLength3 was less than 0 but not equal to SQL_NTS.",
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zlcGetConnName( stlChar         *aInConnectionString,
                          stlInt16         aInStringLen,
                          stlChar         *aConnName,
                          stlInt16        *aConnNameLen,
                          stlErrorStack   *aErrorStack )
{
    stlChar           sConnStrDelimeter[] = ";";
    stlChar         * sSaveStr = NULL;
    stlChar         * sPos = NULL;
    stlChar         * sAttr = NULL;
    stlChar         * sValue = NULL;

    stlChar           sConnString[MAXINFOSIZE];

    *aConnNameLen = 0;

    stlStrncpy( sConnString, aInConnectionString, aInStringLen + 1 );

    sPos = stlStrtok( sConnString, sConnStrDelimeter, &sSaveStr );

    STL_TRY_THROW( sPos != NULL, RAMP_ERR_INVALID_KEYWORD );

    while( sPos != NULL )
    {
        sAttr = sPos;
        sValue = stlStrchr( sPos, '=' );
        sValue[0] = '\0';
        sValue++;

        if( stlStrcasecmp( sAttr, "CONN_NAME" ) == 0 )
        {
            stlStrcpy( aConnName, sValue );
            *aConnNameLen = stlStrlen( aConnName );
        }

        sPos = stlStrtok( NULL, sConnStrDelimeter, &sSaveStr );
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_KEYWORD )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_DRIVER_KEYWORD_SYNTAX_ERROR,
                      NULL,
                      aErrorStack );
    }
    STL_FINISH;

    return STL_FAILURE;
}

#ifdef WIN32
INT_PTR CALLBACK zlcDriverConnectProc( HWND   aDialogHandle,
                                       UINT   aMsg,
                                       WPARAM aWParam,
                                       LPARAM aLParam )
{
    zlcDialogAttr * sDialogAttr;
    stlChar         sBuffer[STL_MAX_INI_PROPERTY_VALUE];
    stlBool         sFocus = STL_FALSE;
    stlInt64        sValue;
    stlChar       * sEndPtr = NULL;

    switch( aMsg )
    {
        case WM_INITDIALOG:
            sDialogAttr = (zlcDialogAttr*)aLParam;

            if( sDialogAttr->mHost[0] == '\0' )
            {
                SetFocus( GetDlgItem(aDialogHandle, IDC_EDIT_HOST) );
                sFocus = STL_TRUE;
            }
            else
            {
                SetDlgItemText( aDialogHandle, IDC_EDIT_HOST, sDialogAttr->mHost );
                EnableWindow( GetDlgItem(aDialogHandle, IDC_EDIT_HOST), FALSE );
            }

            if( *(sDialogAttr->mPort) == 0 )
            {
                if( sFocus == STL_FALSE )
                {
                    SetFocus( GetDlgItem(aDialogHandle, IDC_EDIT_PORT) );
                    sFocus = STL_TRUE;
                }
            }
            else
            {
                stlSnprintf( sBuffer, STL_MAX_INI_PROPERTY_VALUE, "%d", *(sDialogAttr->mPort) );
                SetDlgItemText( aDialogHandle, IDC_EDIT_PORT, sBuffer );
                EnableWindow( GetDlgItem(aDialogHandle, IDC_EDIT_PORT), FALSE );
            }

            if( sDialogAttr->mUserName[0] == '\0' )
            {
                if( sFocus == STL_FALSE )
                {
                    SetFocus( GetDlgItem(aDialogHandle, IDC_EDIT_UID) );
                    sFocus = STL_TRUE;
                }
            }
            else
            {
                SetDlgItemText( aDialogHandle, IDC_EDIT_UID, sDialogAttr->mUserName );
            }

            if( sDialogAttr->mPassword[0] == '\0' )
            {
                if( sFocus == STL_FALSE )
                {
                    SetFocus( GetDlgItem(aDialogHandle, IDC_EDIT_PWD) );
                    sFocus = STL_TRUE;
                }
            }
            else
            {
                SetDlgItemText( aDialogHandle, IDC_EDIT_PWD, sDialogAttr->mPassword );
            }

            SetWindowLongPtr( aDialogHandle, DWLP_USER, aLParam );
            break;

        case WM_COMMAND:
            switch( LOWORD(aWParam) )
            {
                case IDOK:
                    sDialogAttr = (zlcDialogAttr*)GetWindowLongPtr( aDialogHandle, DWLP_USER );

                    GetDlgItemText( aDialogHandle, IDC_EDIT_HOST, sDialogAttr->mHost, STL_MAX_INI_PROPERTY_VALUE);
                    GetDlgItemText( aDialogHandle, IDC_EDIT_PORT, sBuffer, STL_MAX_INI_PROPERTY_VALUE);

                    STL_TRY( stlStrToInt64(sBuffer,
                                           STL_NTS,
                                           &sEndPtr,
                                           10,
                                           &sValue,
                                           sDialogAttr->mErrorStack ) == STL_SUCCESS );
                    *(sDialogAttr->mPort) = (stlInt32)sValue;
                    
                    GetDlgItemText( aDialogHandle, IDC_EDIT_UID, sDialogAttr->mUserName, STL_MAX_INI_PROPERTY_VALUE);
                    *(sDialogAttr->mUserNameLength) = stlStrlen(sDialogAttr->mUserName);
                    
                    GetDlgItemText( aDialogHandle, IDC_EDIT_PWD, sDialogAttr->mPassword, STL_MAX_INI_PROPERTY_VALUE);
                    *(sDialogAttr->mPasswordLength) = stlStrlen(sDialogAttr->mPassword);
                    
                case IDCANCEL:
                    EndDialog(aDialogHandle, LOWORD(aWParam) == IDOK);
                    return TRUE;
            }
    }

    return FALSE;
	
    STL_FINISH;
	
    return FALSE;
}
#endif

stlStatus zlcDriverConnect( zlcDbc        * aDbc,
                            void          * aWindowsHandle,
                            stlChar       * aInConnectionString,
                            stlInt16        aInStringLen,
                            stlChar       * aOutConnectionString,
                            stlInt16        aBufferLen,
                            stlInt16      * aOutStringLen,
                            stlUInt16       aDriverCompletion,
                            stlBool       * aSuccessWithInfo,
                            stlBool       * aIsNoData,
                            stlErrorStack * aErrorStack )
{
    stlChar           sConnStrDelimeter[] = ";";
    stlChar         * sSaveStr = NULL;
    stlChar         * sPos = NULL;
    stlChar         * sAttr = NULL;
    stlChar         * sValue = NULL;
    stlChar         * sConnectionString = NULL;
    stlInt16          sOutStringLen = 0;

    stlChar           sHost[STL_MAX_INI_PROPERTY_VALUE];
    stlChar           sCsMode[STL_MAX_INI_PROPERTY_VALUE] = {0,};
    stlInt32          sPort = 0;
    zlcProtocolType   sProtocolType = ZLC_PROTOCOL_TYPE_MAX;

    stlChar           sUserName[STL_MAX_INI_PROPERTY_VALUE] = { 0, };
    stlInt16          sUserNameLength = 0;
    stlChar           sPassword[STL_MAX_INI_PROPERTY_VALUE] = { 0, };
    stlInt16          sPasswordLength = 0;

    stlChar         * sEndPtr = NULL;
    stlInt64          sInt64Value = 0;

    stlStatus         sReturn = STL_FAILURE;

#ifdef WIN32
    zlcDialogAttr     sDialogAttr;
    LRESULT           sDiagReturn = SQL_ERROR;
#endif

    *aIsNoData = STL_FALSE;

    stlMemset( sHost, 0x00, STL_MAX_INI_PROPERTY_VALUE );

#if defined(ODBC_ALL) || defined(ODBC_DA)
    sProtocolType = ZLC_PROTOCOL_TYPE_DA;
#else
    sProtocolType = ZLC_PROTOCOL_TYPE_TCP;
#endif

    *aSuccessWithInfo = STL_FALSE;
    
    STL_TRY( stlAllocRegionMem( &aDbc->mAllocator,
                                aInStringLen + 1,
                                (void**)&sConnectionString,
                                aErrorStack ) == STL_SUCCESS );

    stlStrcpy( sConnectionString, aInConnectionString );

    sPos = stlStrtok( sConnectionString, sConnStrDelimeter, &sSaveStr );

    STL_TRY_THROW( sPos != NULL, RAMP_ERR_INVALID_KEYWORD );

    while( sPos != NULL )
    {
        sAttr = sPos;
        sValue = stlStrchr( sPos, '=' );
        sValue[0] = '\0';
        sValue++;

        if( stlStrcasecmp( sAttr, "DSN" ) == 0 )
        {
            STL_TRY( zlcGetInfoFromDSN( aDbc,
                                        sValue,
                                        sHost,
                                        &sPort,
                                        sUserName,
                                        sPassword,
                                        sCsMode,
                                        &sProtocolType,
                                        aErrorStack ) == STL_SUCCESS );

            if (sUserName[0] != '\0')
            {
                sUserNameLength = stlStrlen(sUserName);
            }

            if (sPassword[0] != '\0')
            {
                sPasswordLength = stlStrlen(sPassword);
            }
        }
        else if( stlStrcasecmp( sAttr, "HOST" ) == 0 )
        {
            stlStrcpy( sHost, sValue );
        }
        else if( stlStrcasecmp( sAttr, "PORT" ) == 0 )
        {
            STL_TRY( stlStrToInt64( sValue,
                                    STL_NTS,
                                    &sEndPtr,
                                    10,
                                    &sInt64Value,
                                    aErrorStack ) == STL_SUCCESS );

            sPort = (stlInt32)sInt64Value;
        }
        else if( stlStrcasecmp( sAttr, "CS_MODE" ) == 0 )
        {
            stlStrcpy( sCsMode, sValue );
        }
        else if( stlStrcasecmp( sAttr, "UID" ) == 0 )
        {
            stlStrcpy( sUserName, sValue );
            sUserNameLength = stlStrlen( sUserName );
        }
        else if( stlStrcasecmp( sAttr, "PWD" ) == 0 )
        {
            stlStrcpy( sPassword, sValue );
            sPasswordLength = stlStrlen( sPassword );
        }
        else if( stlStrcasecmp( sAttr, "PROTOCOL" ) == 0 )
        {
#if defined( ODBC_ALL )
            if( stlStrcasecmp( sValue, "DA" ) == 0 )
            {
                sProtocolType = ZLC_PROTOCOL_TYPE_DA;
            }
            else if( stlStrcasecmp( sValue, "TCP" ) == 0 )
            {
                sProtocolType = ZLC_PROTOCOL_TYPE_TCP;
            }
            else
            {
                STL_THROW( RAMP_ERR_INVALID_KEYWORD );
            }
#endif
        }
        else if( stlStrcasecmp( sAttr, "MAX_ROWS" ) == 0 )
        {
            STL_TRY( stlStrToInt64( sValue,
                                    STL_NTS,
                                    &sEndPtr,
                                    10,
                                    &sInt64Value,
                                    aErrorStack ) == STL_SUCCESS );

            aDbc->mStmtAttrMaxRows = (SQLULEN)sInt64Value;
        }
        else if( stlStrcasecmp( sAttr, "PREFETCH_ROWS" ) == 0 )
        {
            STL_TRY( stlStrToInt64( sValue,
                                    STL_NTS,
                                    &sEndPtr,
                                    10,
                                    &sInt64Value,
                                    aErrorStack ) == STL_SUCCESS );

            aDbc->mStmtPreFetchSize = (stlInt32)sInt64Value;
        }
        else if( stlStrcasecmp( sAttr, "CHAR_LENGTH_UNITS" ) == 0 )
        {
            if( (stlStrcasecmp( sValue, "BYTE" ) == 0) ||
                (stlStrcasecmp( sValue, "OCTETS" ) == 0) )
            {
                aDbc->mStmtStringLengthUnit = DTL_STRING_LENGTH_UNIT_OCTETS;
            }
            else if( (stlStrcasecmp( sValue, "CHAR" ) == 0) ||
                     (stlStrcasecmp( sValue, "CHARACTERS" ) == 0) )
            {
                aDbc->mStmtStringLengthUnit = DTL_STRING_LENGTH_UNIT_CHARACTERS;
            }
            else
            {
                STL_THROW( RAMP_ERR_INVALID_ATTRIBUTE_VALUE );
            }
        }
        else if( stlStrcasecmp( sAttr, "DATE_FORMAT" ) == 0 )
        {
            STL_TRY( zlcSetAttr( aDbc,
                                 SQL_ATTR_DATE_FORMAT,
                                 sValue,
                                 stlStrlen(sValue),
                                 aErrorStack ) == STL_SUCCESS );
        }
        else if( stlStrcasecmp( sAttr, "TIME_FORMAT" ) == 0 )
        {
            STL_TRY( zlcSetAttr( aDbc,
                                 SQL_ATTR_TIME_FORMAT,
                                 sValue,
                                 stlStrlen(sValue),
                                 aErrorStack ) == STL_SUCCESS );
        }
        else if( stlStrcasecmp( sAttr, "TIME_WITH_TIME_ZONE_FORMAT" ) == 0 )
        {
            STL_TRY( zlcSetAttr( aDbc,
                                 SQL_ATTR_TIME_WITH_TIMEZONE_FORMAT,
                                 sValue,
                                 stlStrlen(sValue),
                                 aErrorStack ) == STL_SUCCESS );
        }
        else if( stlStrcasecmp( sAttr, "TIMESTAMP_FORMAT" ) == 0 )
        {
            STL_TRY( zlcSetAttr( aDbc,
                                 SQL_ATTR_TIMESTAMP_FORMAT,
                                 sValue,
                                 stlStrlen(sValue),
                                 aErrorStack ) == STL_SUCCESS );
        }
        else if( stlStrcasecmp( sAttr, "TIMESTAMP_WITH_TIME_ZONE_FORMAT" ) == 0 )
        {
            STL_TRY( zlcSetAttr( aDbc,
                                 SQL_ATTR_TIMESTAMP_WITH_TIMEZONE_FORMAT,
                                 sValue,
                                 stlStrlen(sValue),
                                 aErrorStack ) == STL_SUCCESS );
        }
        else if( stlStrcasecmp( sAttr, "CONN_NAME" ) == 0 )
        {
            /*
             * Do Nothing
             */
        }
        else
        {
            STL_THROW( RAMP_ERR_INVALID_KEYWORD );
        }

        sPos = stlStrtok( NULL, sConnStrDelimeter, &sSaveStr );
    }

    switch( aDriverCompletion )
    {
        case SQL_DRIVER_PROMPT :
        case SQL_DRIVER_COMPLETE_REQUIRED :
        case SQL_DRIVER_COMPLETE :
            if( (sHost[0] == '\0') || (sPort == 0) || (sUserName[0] == '\0') || (sPassword[0] == '\0') )
            {
#ifdef WIN32
                sDialogAttr.mHost = sHost;
                sDialogAttr.mPort = &sPort;
                sDialogAttr.mUserName = sUserName;
                sDialogAttr.mUserNameLength = &sUserNameLength;
                sDialogAttr.mPassword = sPassword;
                sDialogAttr.mPasswordLength = &sPasswordLength;
                sDialogAttr.mErrorStack = aErrorStack;

                sDiagReturn = DialogBoxParam( gZllInstance,
                                              MAKEINTRESOURCE(IDD_DIALOG_DRIVER_CONNECT),
                                              (HWND)aWindowsHandle,
                                              zlcDriverConnectProc,
                                              (LPARAM)&sDialogAttr);

                STL_TRY_THROW( (sDiagReturn != 0) && (sDiagReturn != -1), RAMP_NO_DATA );
#else
                STL_THROW( RAMP_ERR_OPTIONAL_FEATURE_NOT_IMPLEMENTED );
#endif
            }
            break;
        default :
            break;
    }

    STL_TRY( zlcConnect( aDbc,
                         sHost,
                         sPort,
                         sProtocolType,
                         sCsMode,
                         sUserName,
                         sUserNameLength,
                         sPassword,
                         sPasswordLength,
                         aSuccessWithInfo,
                         aErrorStack ) == STL_SUCCESS );

    if( aOutConnectionString != NULL )
    {
#if defined( ODBC_ALL )
        if( sProtocolType == ZLC_PROTOCOL_TYPE_TCP )
        {
            sOutStringLen += stlGetPrintfBufferSize( "PROTOCOL=TCP;" );
        }
        else
        {
            sOutStringLen += stlGetPrintfBufferSize( "PROTOCOL=DA;" );
        }
#endif        
        if( aDbc->mServerName != NULL )
        {
            sOutStringLen += stlGetPrintfBufferSize( "DSN=%s;", aDbc->mServerName );
        }
        
        if( sProtocolType == ZLC_PROTOCOL_TYPE_TCP )
        {
            sOutStringLen += stlGetPrintfBufferSize( "HOST=%s;PORT=%d;", sHost, sPort );
        }
        
        sOutStringLen += stlGetPrintfBufferSize( "UID=%s;PWD=%s;", sUserName, sPassword );

        if( aDbc->mServerName != NULL )
        {
            if( sProtocolType == ZLC_PROTOCOL_TYPE_TCP )
            {
                sOutStringLen = stlSnprintf( aOutConnectionString,
                                             aBufferLen,
#if defined( ODBC_ALL )
                                             "PROTOCOL=TCP;"
#endif
                                             "DSN=%s;"
                                             "HOST=%s;"
                                             "PORT=%d;"
                                             "UID=%s;"
                                             "PWD=%s;",
                                             aDbc->mServerName,
                                             sHost,
                                             sPort,
                                             sUserName,
                                             sPassword );
            }
            else
            {
                sOutStringLen = stlSnprintf( aOutConnectionString,
                                             aBufferLen,
#if defined( ODBC_ALL )
                                             "PROTOCOL=DA;"
#endif
                                             "DSN=%s;"
                                             "UID=%s;"
                                             "PWD=%s;",
                                             aDbc->mServerName,
                                             sUserName,
                                             sPassword );
            }
        }
        else
        {
            if( sProtocolType == ZLC_PROTOCOL_TYPE_TCP )
            {
                sOutStringLen = stlSnprintf( aOutConnectionString,
                                             aBufferLen,
#if defined( ODBC_ALL )
                                             "PROTOCOL=TCP;"
#endif
                                             "HOST=%s;"
                                             "PORT=%d;"
                                             "UID=%s;"
                                             "PWD=%s;",
                                             sHost,
                                             sPort,
                                             sUserName,
                                             sPassword );
            }
            else
            {
                sOutStringLen = stlSnprintf( aOutConnectionString,
                                             aBufferLen,
#if defined( ODBC_ALL )
                                             "PROTOCOL=DA;"
#endif
                                             "UID=%s;"
                                             "PWD=%s;",
                                             sUserName,
                                             sPassword );
            }
        }
    }

    if( aOutStringLen != NULL )
    {
        *aOutStringLen = sOutStringLen;
    }

    sReturn = STL_SUCCESS;
    
    return sReturn;

    STL_CATCH( RAMP_ERR_INVALID_KEYWORD )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_DRIVER_KEYWORD_SYNTAX_ERROR,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_ATTRIBUTE_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_ATTRIBUTE_VALUE,
                      NULL,
                      aErrorStack );
    }

#ifdef WIN32
    STL_CATCH( RAMP_NO_DATA )
    {
        sReturn    = STL_SUCCESS;
        *aIsNoData = STL_TRUE;
    }
#else
    STL_CATCH( RAMP_ERR_OPTIONAL_FEATURE_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_OPTIONAL_FEATURE_NOT_IMPLEMENTED,
                      "The driver does not support the version of ODBC behavior that the application requested.",
                      aErrorStack );
    }
#endif

    STL_FINISH;

    return sReturn;
}

stlStatus zlcDisconnect( zlcDbc        * aDbc,
                         stlErrorStack * aErrorStack )
{
    zlsStmt * sCurStmt;
    zlsStmt * sNextStmt;

    STL_RING_FOREACH_ENTRY_SAFE( &aDbc->mStmtRing, sCurStmt, sNextStmt )
    {
        STL_TRY( zlsFree( sCurStmt, STL_FALSE, aErrorStack ) == SQL_SUCCESS );
    }

    if( aDbc->mDbcName != NULL )
    {
        stlFreeHeap( aDbc->mDbcName );
        aDbc->mDbcName = NULL;
    }

    if( aDbc->mServerName != NULL )
    {
        stlFreeHeap( aDbc->mServerName );
        aDbc->mServerName = NULL;
    }

    if( aDbc->mBrokenConnection == STL_FALSE )
    {
#if defined( ODBC_ALL )        
        STL_TRY( gZlcDisconnectFunc[aDbc->mProtocolType]( aDbc,
                                                          aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_DA )
        STL_TRY( zlacDisconnect( aDbc, aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_CS )
        STL_TRY( zlccDisconnect( aDbc, aErrorStack ) == STL_SUCCESS );
#else
        STL_ASSERT( STL_FALSE );
#endif
    }

    /*
     * shutdown 인 경우는 aDbc->mBrokenConnection가 TRUE로 설정되어
     * zlccDisconnect가 호출되지 않기 때문에 여기서 stlFinalizeContext 한다.
     */
    if( aDbc->mProtocolType == ZLC_PROTOCOL_TYPE_TCP )
    {
        if( stlFinalizeContext( &aDbc->mContext, aErrorStack ) == STL_FAILURE )
        {
            (void)stlPopError( aErrorStack );
        }
    }

    aDbc->mSessionId        = ZLC_INIT_SESSION_ID;
    aDbc->mTransition       = ZLC_TRANSITION_STATE_C2;
    aDbc->mBrokenConnection = STL_TRUE;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zlcSetAttr( zlcDbc        * aDbc,
                      stlInt32        aAttr,
                      void          * aValue,
                      stlInt32        aStrLen,
                      stlErrorStack * aErrorStack )
{
    stlBool                      sAllocHeap = STL_FALSE;
    stlChar                    * sStrBuffer = NULL;
    dtlCharacterSet              sCharset;
    dtlDataValue                 sTimeZoneValue;
    dtlIntervalDayToSecondType   sTimeZoneInterval;
    stlBool                      sTimeZoneWithInfo = STL_FALSE;
    stlInt32                     sTimeZoneOffset = 0;

    stlChar                    * sFormat = NULL;
    stlInt64                   * sFormatInfo = NULL;

    stlInt32                     sIntValue;
    
    switch( aAttr )
    {
        case SQL_ATTR_ACCESS_MODE :
            switch( STL_INT_FROM_POINTER(aValue) )
            {
                case SQL_MODE_READ_ONLY :
                    /*
                     * ACCESS MODE를 READ ONLY로 설정해도 실제 UPDATE는 발생할 수 있다.
                     */
                    aDbc->mAttrAccessMode = ZLL_ACCESS_MODE_READ_ONLY;
                    break;
                case SQL_MODE_READ_WRITE :
                    aDbc->mAttrAccessMode = ZLL_ACCESS_MODE_UPDATABLE;
                    break;
                default :
                    STL_THROW( RAMP_ERR_INVALID_ATTRIBUTE_VALUE );
                    break;
            }
            break;
        case SQL_ATTR_AUTOCOMMIT :
            switch( STL_INT_FROM_POINTER(aValue) )
            {
                case SQL_AUTOCOMMIT_OFF :
                    aDbc->mAttrAutoCommit = STL_FALSE;
                    break;
                case SQL_AUTOCOMMIT_ON :
                    aDbc->mAttrAutoCommit = STL_TRUE;
                    break;
                default :
                    STL_THROW( RAMP_ERR_INVALID_ATTRIBUTE_VALUE );
                    break;
            }
            break;
        case SQL_ATTR_CURRENT_CATALOG :
            break;
        case SQL_ATTR_LOGIN_TIMEOUT :
            aDbc->mAttrLoginTimeout = STL_SET_SEC_TIME(STL_INT_FROM_POINTER(aValue));
            break;
        case SQL_ATTR_MAX_ROWS :
            aDbc->mStmtAttrMaxRows = (SQLULEN)(stlVarInt)aValue;
            break;
        case SQL_ATTR_METADATA_ID :
            switch( STL_INT_FROM_POINTER(aValue) )
            {
                case SQL_TRUE :
                    aDbc->mMetadataId = STL_TRUE;
                    break;
                case SQL_FALSE :
                    aDbc->mMetadataId = STL_FALSE;
                    break;
                default :
                    STL_THROW( RAMP_ERR_INVALID_ATTRIBUTE_VALUE );
                    break;
            }
            break;
        case SQL_ATTR_QUERY_TIMEOUT :
            /**
             * @todo stmt에 설정해야 한다.
             */
            break;
        case SQL_ATTR_TXN_ISOLATION :
            switch( STL_INT_FROM_POINTER(aValue) )
            {
                case SQL_TRANSACTION_READ_UNCOMMITTED :
                    sIntValue = ZLL_ISOLATION_LEVEL_READ_UNCOMMITTED;
                    break;
                case SQL_TRANSACTION_READ_COMMITTED :
                    sIntValue = ZLL_ISOLATION_LEVEL_READ_COMMITTED;
                    break;
                case SQL_TRANSACTION_SERIALIZABLE :
                    sIntValue = ZLL_ISOLATION_LEVEL_SERIALIZABLE;
                    break;
                case SQL_TRANSACTION_REPEATABLE_READ :
                    STL_THROW( RAMP_ERR_OPTIONAL_FEATURE_NOT_IMPLEMENTED );
                    break;
                default :
                    STL_THROW( RAMP_ERR_INVALID_ATTRIBUTE_VALUE );
                    break;
            }
#if defined( ODBC_ALL )
            STL_TRY( gZlcSetAttrFunc[aDbc->mProtocolType]( aDbc,
                                                           aAttr,
                                                           sIntValue,
                                                           NULL,
                                                           aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_DA )
            STL_TRY( zlacSetAttr( aDbc,
                                  aAttr,
                                  sIntValue,
                                  NULL,
                                  aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_CS )
            STL_TRY( zlccSetAttr( aDbc,
                                  aAttr,
                                  sIntValue,
                                  NULL,
                                  aErrorStack ) == STL_SUCCESS );
#else
            STL_ASSERT( STL_FALSE );
#endif

            aDbc->mAttrTxnIsolation = sIntValue;
            break;
        case SQL_ATTR_TIMEZONE:
            if( aStrLen != SQL_NTS )
            {
                STL_TRY( stlAllocHeap( (void**)&sStrBuffer,
                                       aStrLen + 1,
                                       aErrorStack ) == STL_SUCCESS );
                sAllocHeap = STL_TRUE;

                stlMemset( sStrBuffer, 0x00, aStrLen + 1 );
                stlMemcpy( sStrBuffer, aValue, aStrLen );
            }
            else
            {
                aStrLen    = stlStrlen( (stlChar*)aValue );
                sStrBuffer = (stlChar*)aValue;
            }

            /*
             * TimeZone Interval String 을 dtlInterval 로 변경
             */
            sTimeZoneValue.mValue = &sTimeZoneInterval;

            STL_TRY( dtlInitDataValue( DTL_TYPE_INTERVAL_DAY_TO_SECOND,
                                       STL_SIZEOF(dtlIntervalDayToSecondType),
                                       &sTimeZoneValue,
                                       aErrorStack )
                     == STL_SUCCESS );
                
            STL_TRY( dtlIntervalDayToSecondSetValueFromString(
                         sStrBuffer,
                         aStrLen,
                         DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION,
                         DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION,
                         DTL_STRING_LENGTH_UNIT_NA,
                         DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE,
                         DTL_INTERVAL_DAY_TO_SECOND_SIZE,
                         &sTimeZoneValue,
                         &sTimeZoneWithInfo,
                         &aDbc->mDTVector,
                         aDbc,
                         &aDbc->mDTVector,
                         aDbc,
                         aErrorStack ) == STL_SUCCESS );
                                                
            /*
             * dtlInterval 을 Timezone Offset 으로 변경 
             */

            STL_TRY( dtlTimeZoneToGMTOffset( &sTimeZoneInterval,
                                             &sTimeZoneOffset,
                                             aErrorStack )
                     == STL_SUCCESS );

#if defined( ODBC_ALL )
            STL_TRY( gZlcSetAttrFunc[aDbc->mProtocolType]( aDbc,
                                                           aAttr,
                                                           sTimeZoneOffset,
                                                           NULL,
                                                           aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_DA )
            STL_TRY( zlacSetAttr( aDbc,
                                  aAttr,
                                  sTimeZoneOffset,
                                  NULL,
                                  aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_CS )
            STL_TRY( zlccSetAttr( aDbc,
                                  aAttr,
                                  sTimeZoneOffset,
                                  NULL,
                                  aErrorStack ) == STL_SUCCESS );
#else
            STL_ASSERT( STL_FALSE );
#endif

            aDbc->mTimezone = sTimeZoneOffset;

            if( sAllocHeap == STL_TRUE )
            {
                sAllocHeap = STL_FALSE;
                stlFreeHeap( sStrBuffer );
                sStrBuffer = NULL;
            }
            break;
        case SQL_ATTR_CHARACTER_SET :
            if( aStrLen != SQL_NTS )
            {
                STL_TRY( stlAllocHeap( (void**)&sStrBuffer,
                                       aStrLen + 1,
                                       aErrorStack ) == STL_SUCCESS );
                sAllocHeap = STL_TRUE;

                stlMemset( sStrBuffer, 0x00, aStrLen + 1 );
                stlMemcpy( sStrBuffer, aValue, aStrLen );
            }
            else
            {
                sStrBuffer = (stlChar*)aValue;
            }

            STL_TRY( dtlGetCharacterSet( sStrBuffer,
                                         &sCharset,
                                         aErrorStack ) == STL_SUCCESS );

            aDbc->mCharacterSet = sCharset;

            if( sAllocHeap == STL_TRUE )
            {
                sAllocHeap = STL_FALSE;
                stlFreeHeap( sStrBuffer );
                sStrBuffer = NULL;
            }

            STL_TRY( zlcSetDTFuncVector( aDbc ) == STL_SUCCESS );
    
            break;
        case SQL_ATTR_DATE_FORMAT :
            if( aStrLen == SQL_NTS )
            {
                aStrLen = stlStrlen( (stlChar*)aValue );
            }

            if( aDbc->mDateFormat != NULL )
            {
                stlFreeHeap( aDbc->mDateFormat );
                aDbc->mDateFormat = NULL;
            }

            STL_TRY( stlAllocHeap( (void**)&sFormat,
                                   aStrLen + 1,
                                   aErrorStack ) == STL_SUCCESS );

            stlStrcpy( sFormat, (stlChar*)aValue );

            if( aDbc->mDateFormatInfo == NULL )
            {
                STL_TRY( stlAllocHeap( (void**)&sFormatInfo,
                                       DTL_DATETIME_FORMAT_INFO_MAX_BUFFER_SIZE,
                                       aErrorStack ) == STL_SUCCESS );
            }
            else
            {
                sFormatInfo = aDbc->mDateFormatInfo;
            }

            stlMemset( (void*)sFormatInfo,
                       0x00,
                       DTL_DATETIME_FORMAT_INFO_MAX_BUFFER_SIZE );

            STL_TRY( dtlGetDateTimeFormatInfoForToChar( DTL_TYPE_DATE,
                                                        sFormat,
                                                        aStrLen,
                                                        sFormatInfo,
                                                        DTL_DATETIME_FORMAT_INFO_MAX_BUFFER_SIZE,
                                                        NULL, // aVectorFunc
                                                        NULL, // aVectorArg
                                                        aErrorStack )
                     == STL_SUCCESS );
    
            STL_TRY( dtlGetDateTimeFormatInfoForToDateTime( DTL_TYPE_DATE,
                                                            sFormat,
                                                            aStrLen,
                                                            STL_TRUE, /* aIsSaveToCharMaxResultLen */
                                                            sFormatInfo,
                                                            DTL_DATETIME_FORMAT_INFO_MAX_BUFFER_SIZE,
                                                            NULL, // aVectorFunc
                                                            NULL, // aVectorArg
                                                            aErrorStack )
                     == STL_SUCCESS );

            aDbc->mDateFormat = sFormat;
            aDbc->mDateFormatInfo = sFormatInfo;
            break;
        case SQL_ATTR_TIME_FORMAT :
            if( aStrLen == SQL_NTS )
            {
                aStrLen = stlStrlen( (stlChar*)aValue );
            }

            if( aDbc->mTimeFormat != NULL )
            {
                stlFreeHeap( aDbc->mTimeFormat );
                aDbc->mTimeFormat = NULL;
            }

            STL_TRY( stlAllocHeap( (void**)&sFormat,
                                   aStrLen + 1,
                                   aErrorStack ) == STL_SUCCESS );

            stlStrcpy( sFormat, (stlChar*)aValue );

            if( aDbc->mTimeFormatInfo == NULL )
            {
                STL_TRY( stlAllocHeap( (void**)&sFormatInfo,
                                       DTL_DATETIME_FORMAT_INFO_MAX_BUFFER_SIZE,
                                       aErrorStack ) == STL_SUCCESS );
            }
            else
            {
                sFormatInfo = aDbc->mTimeFormatInfo;
            }

            stlMemset( (void*)sFormatInfo,
                       0x00,
                       DTL_DATETIME_FORMAT_INFO_MAX_BUFFER_SIZE );

            STL_TRY( dtlGetDateTimeFormatInfoForToChar( DTL_TYPE_TIME,
                                                        sFormat,
                                                        aStrLen,
                                                        sFormatInfo,
                                                        DTL_DATETIME_FORMAT_INFO_MAX_BUFFER_SIZE,
                                                        NULL, // aVectorFunc
                                                        NULL, // aVectorArg
                                                        aErrorStack )
                     == STL_SUCCESS );
    
            STL_TRY( dtlGetDateTimeFormatInfoForToDateTime( DTL_TYPE_TIME,
                                                            sFormat,
                                                            aStrLen,
                                                            STL_TRUE, /* aIsSaveToCharMaxResultLen */
                                                            sFormatInfo,
                                                            DTL_DATETIME_FORMAT_INFO_MAX_BUFFER_SIZE,
                                                            NULL, // aVectorFunc
                                                            NULL, // aVectorArg
                                                            aErrorStack )
                     == STL_SUCCESS );

            aDbc->mTimeFormat = sFormat;
            aDbc->mTimeFormatInfo = sFormatInfo;
            break;
        case SQL_ATTR_TIME_WITH_TIMEZONE_FORMAT :
            if( aStrLen == SQL_NTS )
            {
                aStrLen = stlStrlen( (stlChar*)aValue );
            }

            if( aDbc->mTimeWithTimezoneFormat != NULL )
            {
                stlFreeHeap( aDbc->mTimeWithTimezoneFormat );
                aDbc->mTimeWithTimezoneFormat = NULL;
            }

            STL_TRY( stlAllocHeap( (void**)&sFormat,
                                   aStrLen + 1,
                                   aErrorStack ) == STL_SUCCESS );

            stlStrcpy( sFormat, (stlChar*)aValue );

            if( aDbc->mTimeWithTimezoneFormatInfo == NULL )
            {
                STL_TRY( stlAllocHeap( (void**)&sFormatInfo,
                                       DTL_DATETIME_FORMAT_INFO_MAX_BUFFER_SIZE,
                                       aErrorStack ) == STL_SUCCESS );
            }
            else
            {
                sFormatInfo = aDbc->mTimeWithTimezoneFormatInfo;
            }

            stlMemset( (void*)sFormatInfo,
                       0x00,
                       DTL_DATETIME_FORMAT_INFO_MAX_BUFFER_SIZE );

            STL_TRY( dtlGetDateTimeFormatInfoForToChar( DTL_TYPE_TIME_WITH_TIME_ZONE,
                                                        sFormat,
                                                        aStrLen,
                                                        sFormatInfo,
                                                        DTL_DATETIME_FORMAT_INFO_MAX_BUFFER_SIZE,
                                                        NULL, // aVectorFunc
                                                        NULL, // aVectorArg
                                                        aErrorStack )
                     == STL_SUCCESS );
    
            STL_TRY( dtlGetDateTimeFormatInfoForToDateTime( DTL_TYPE_TIME_WITH_TIME_ZONE,
                                                            sFormat,
                                                            aStrLen,
                                                            STL_TRUE, /* aIsSaveToCharMaxResultLen */
                                                            sFormatInfo,
                                                            DTL_DATETIME_FORMAT_INFO_MAX_BUFFER_SIZE,
                                                            NULL, // aVectorFunc
                                                            NULL, // aVectorArg
                                                            aErrorStack )
                     == STL_SUCCESS );

            aDbc->mTimeWithTimezoneFormat = sFormat;
            aDbc->mTimeWithTimezoneFormatInfo = sFormatInfo;
            break;
        case SQL_ATTR_TIMESTAMP_FORMAT :
            if( aStrLen == SQL_NTS )
            {
                aStrLen = stlStrlen( (stlChar*)aValue );
            }

            if( aDbc->mTimestampFormat != NULL )
            {
                stlFreeHeap( aDbc->mTimestampFormat );
                aDbc->mTimestampFormat = NULL;
            }

            STL_TRY( stlAllocHeap( (void**)&sFormat,
                                   aStrLen + 1,
                                   aErrorStack ) == STL_SUCCESS );

            stlStrcpy( sFormat, (stlChar*)aValue );

            if( aDbc->mTimestampFormatInfo == NULL )
            {
                STL_TRY( stlAllocHeap( (void**)&sFormatInfo,
                                       DTL_DATETIME_FORMAT_INFO_MAX_BUFFER_SIZE,
                                       aErrorStack ) == STL_SUCCESS );
            }
            else
            {
                sFormatInfo = aDbc->mTimestampFormatInfo;
            }

            stlMemset( (void*)sFormatInfo,
                       0x00,
                       DTL_DATETIME_FORMAT_INFO_MAX_BUFFER_SIZE );

            STL_TRY( dtlGetDateTimeFormatInfoForToChar( DTL_TYPE_TIMESTAMP,
                                                        sFormat,
                                                        aStrLen,
                                                        sFormatInfo,
                                                        DTL_DATETIME_FORMAT_INFO_MAX_BUFFER_SIZE,
                                                        NULL, // aVectorFunc
                                                        NULL, // aVectorArg
                                                        aErrorStack )
                     == STL_SUCCESS );
    
            STL_TRY( dtlGetDateTimeFormatInfoForToDateTime( DTL_TYPE_TIMESTAMP,
                                                            sFormat,
                                                            aStrLen,
                                                            STL_TRUE, /* aIsSaveToCharMaxResultLen */
                                                            sFormatInfo,
                                                            DTL_DATETIME_FORMAT_INFO_MAX_BUFFER_SIZE,
                                                            NULL, // aVectorFunc
                                                            NULL, // aVectorArg
                                                            aErrorStack )
                     == STL_SUCCESS );

            aDbc->mTimestampFormat = sFormat;
            aDbc->mTimestampFormatInfo = sFormatInfo;
            break;
        case SQL_ATTR_TIMESTAMP_WITH_TIMEZONE_FORMAT :
            if( aStrLen == SQL_NTS )
            {
                aStrLen = stlStrlen( (stlChar*)aValue );
            }

            if( aDbc->mTimestampWithTimezoneFormat != NULL )
            {
                stlFreeHeap( aDbc->mTimestampWithTimezoneFormat );
                aDbc->mTimestampWithTimezoneFormat = NULL;
            }

            STL_TRY( stlAllocHeap( (void**)&sFormat,
                                   aStrLen + 1,
                                   aErrorStack ) == STL_SUCCESS );

            stlStrcpy( sFormat, (stlChar*)aValue );

            if( aDbc->mTimestampWithTimezoneFormatInfo == NULL )
            {
                STL_TRY( stlAllocHeap( (void**)&sFormatInfo,
                                       DTL_DATETIME_FORMAT_INFO_MAX_BUFFER_SIZE,
                                       aErrorStack ) == STL_SUCCESS );
            }
            else
            {
                sFormatInfo = aDbc->mTimestampWithTimezoneFormatInfo;
            }

            stlMemset( (void*)sFormatInfo,
                       0x00,
                       DTL_DATETIME_FORMAT_INFO_MAX_BUFFER_SIZE );

            STL_TRY( dtlGetDateTimeFormatInfoForToChar( DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,
                                                        sFormat,
                                                        aStrLen,
                                                        sFormatInfo,
                                                        DTL_DATETIME_FORMAT_INFO_MAX_BUFFER_SIZE,
                                                        NULL, // aVectorFunc
                                                        NULL, // aVectorArg
                                                        aErrorStack )
                     == STL_SUCCESS );
    
            STL_TRY( dtlGetDateTimeFormatInfoForToDateTime( DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,
                                                            sFormat,
                                                            aStrLen,
                                                            STL_TRUE, /* aIsSaveToCharMaxResultLen */
                                                            sFormatInfo,
                                                            DTL_DATETIME_FORMAT_INFO_MAX_BUFFER_SIZE,
                                                            NULL, // aVectorFunc
                                                            NULL, // aVectorArg
                                                            aErrorStack )
                     == STL_SUCCESS );

            aDbc->mTimestampWithTimezoneFormat = sFormat;
            aDbc->mTimestampWithTimezoneFormatInfo = sFormatInfo;
            break;
        case SQL_ATTR_OLDPWD :
            if( aStrLen == SQL_NTS )
            {
                aStrLen = stlStrlen( (stlChar*)aValue );
            }

            if( aDbc->mAttrOldPwd != NULL )
            {
                stlFreeHeap( aDbc->mAttrOldPwd );
                aDbc->mAttrOldPwd = NULL;
            }

            STL_TRY( stlAllocHeap( (void**)&aDbc->mAttrOldPwd,
                                   aStrLen + 1,
                                   aErrorStack ) == STL_SUCCESS );

            stlStrcpy( aDbc->mAttrOldPwd, (stlChar*)aValue );
            break;
        default:
            STL_THROW( RAMP_ERR_INVALID_ATTRIBUTE_OPTION_IDENTIFIER );
            break;
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_ATTRIBUTE_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_ATTRIBUTE_VALUE,
                      "Given the specified Attribute value, "
                      "an invalid value was specified in ValuePtr.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_OPTIONAL_FEATURE_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_OPTIONAL_FEATURE_NOT_IMPLEMENTED,
                      "The value specified for the argument Attribute was a valid "
                      "ODBC connection or statement attribute for the version of "
                      "ODBC supported by the driver but was not supported by the driver.",
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

    STL_FINISH;

    if( sAllocHeap == STL_TRUE )
    {
        stlFreeHeap( sStrBuffer );
    }

    if( sFormat != NULL )
    {
        stlFreeHeap( sFormat );
    }

    if( sFormatInfo != NULL )
    {
        stlFreeHeap( sFormatInfo );
    }

    return STL_FAILURE;
}

stlStatus zlcGetAttr( zlcDbc        * aDbc,
                      stlInt32        aAttr,
                      void          * aValue,
                      stlInt32        aBufferLen,
                      stlInt32      * aStringLen,
                      stlErrorStack * aErrorStack )
{
    stlChar    sTimezone[DTL_MAX_TIMEZONE_LEN];
    stlChar  * sCharacterSetString;
    stlChar  * sFormat;
    stlInt32   sStringLength;

    switch( aAttr )
    {
        case SQL_ATTR_ACCESS_MODE :
            if( aDbc->mAttrAccessMode == ZLL_ACCESS_MODE_READ_ONLY )
            {
                *(stlUInt32*)aValue = SQL_MODE_READ_ONLY;
            }
            else
            {
                *(stlUInt32*)aValue = SQL_MODE_READ_WRITE;
            }
            break;
        case SQL_ATTR_AUTOCOMMIT :
            if( aDbc->mAttrAutoCommit == STL_TRUE )
            {
                *(stlUInt32*)aValue = SQL_AUTOCOMMIT_ON;
            }
            else
            {
                *(stlUInt32*)aValue = SQL_AUTOCOMMIT_OFF;
            }
            break;
        case SQL_ATTR_CONNECTION_DEAD :
            *(stlUInt32*)aValue = SQL_CD_FALSE;
            break;            
        case SQL_ATTR_CURRENT_CATALOG :
            if( aStringLen != NULL )
            {
                *aStringLen = 0;
            }

            if( aValue != NULL )
            {
                ((stlChar*)aValue)[0] = '\0';
            }
            break;
        case SQL_ATTR_LOGIN_TIMEOUT :
            *(stlUInt32*)aValue = (stlUInt32)STL_GET_SEC_TIME(aDbc->mAttrLoginTimeout);
            break;
        case SQL_ATTR_MAX_ROWS :
            *(SQLULEN*)aValue = aDbc->mStmtAttrMaxRows;
            break;
        case SQL_ATTR_METADATA_ID :
            if( aDbc->mMetadataId == STL_TRUE )
            {
                *(stlUInt32*)aValue = SQL_TRUE;
            }
            else
            {
                *(stlUInt32*)aValue = SQL_FALSE;
            }
            break;
        case SQL_ATTR_QUERY_TIMEOUT :
            *(SQLULEN*)aValue = 0;
            break;
        case SQL_ATTR_TXN_ISOLATION :
            switch( aDbc->mAttrTxnIsolation )
            {
                case ZLL_ISOLATION_LEVEL_READ_UNCOMMITTED :
                    *(stlInt32*)aValue = SQL_TRANSACTION_READ_UNCOMMITTED;
                    break;
                case ZLL_ISOLATION_LEVEL_READ_COMMITTED :
                    *(stlInt32*)aValue = SQL_TRANSACTION_READ_COMMITTED;
                    break;
                case ZLL_ISOLATION_LEVEL_SERIALIZABLE :
                    *(stlInt32*)aValue = SQL_TRANSACTION_SERIALIZABLE;
                    break;
            }
            break;
        case SQL_ATTR_TIMEZONE:
            if( aDbc->mTimezone >= 0 )
            {
                stlSnprintf( sTimezone,
                             DTL_MAX_TIMEZONE_LEN,
                             "+%02d:%02d",
                             aDbc->mTimezone / DTL_SECS_PER_HOUR,
                             (aDbc->mTimezone % DTL_SECS_PER_HOUR) / DTL_SECS_PER_MINUTE );
            }
            else
            {
                stlSnprintf( sTimezone,
                             DTL_MAX_TIMEZONE_LEN,
                             "-%02d:%02d",
                             aDbc->mTimezone / DTL_SECS_PER_HOUR,
                             (aDbc->mTimezone % DTL_SECS_PER_HOUR) / DTL_SECS_PER_MINUTE );
            }

            sStringLength = stlStrlen( sTimezone );
            
            if( aStringLen != NULL )
            {
                *aStringLen = sStringLength;
            }

            if( aValue != NULL )
            {
                stlStrncpy( (stlChar*)aValue,
                            sTimezone,
                            aBufferLen );
            }
            break;
        case SQL_ATTR_CHARACTER_SET :
            sCharacterSetString = gDtlCharacterSetString[aDbc->mCharacterSet];
            sStringLength = stlStrlen( sCharacterSetString );
            
            if( aStringLen != NULL )
            {
                *aStringLen = sStringLength;
            }

            if( aValue != NULL )
            {
                stlStrncpy( (stlChar*)aValue,
                            sCharacterSetString,
                            aBufferLen );
            }
            break;
        case SQL_ATTR_DATABASE_CHARACTER_SET :
            sCharacterSetString = gDtlCharacterSetString[aDbc->mNlsCharacterSet];
            sStringLength = stlStrlen( sCharacterSetString );
            
            if( aStringLen != NULL )
            {
                *aStringLen = sStringLength;
            }

            if( aValue != NULL )
            {
                stlStrncpy( (stlChar*)aValue,
                            sCharacterSetString,
                            aBufferLen );
            }
            break;
        case SQL_ATTR_DATE_FORMAT :
            if( aDbc->mDateFormat == NULL )
            {
                sFormat = DTL_DATE_FORMAT_FOR_ODBC;
            }
            else
            {
                sFormat = aDbc->mDateFormat;
            }

            sStringLength = stlStrlen( sFormat );
            
            if( aStringLen != NULL )
            {
                *aStringLen = sStringLength;
            }

            if( aValue != NULL )
            {
                stlStrncpy( (stlChar*)aValue,
                            sFormat,
                            aBufferLen );
            }
            break;
        case SQL_ATTR_PROTOCOL :
            switch( aDbc->mProtocolType )
            {
                case ZLC_PROTOCOL_TYPE_DA :
                    *(stlUInt32*)aValue = SQL_DA;
                    break;
                case ZLC_PROTOCOL_TYPE_TCP :
                    *(stlUInt32*)aValue = SQL_TCP;
                    break;
                default :
                    STL_ASSERT( STL_FALSE );
                    break;
            }
            break;
        case SQL_ATTR_TIME_FORMAT :
            if( aDbc->mTimeFormat == NULL )
            {
                sFormat = DTL_TIME_FORMAT_FOR_ODBC;
            }
            else
            {
                sFormat = aDbc->mTimeFormat;
            }

            sStringLength = stlStrlen( sFormat );
            
            if( aStringLen != NULL )
            {
                *aStringLen = sStringLength;
            }

            if( aValue != NULL )
            {
                stlStrncpy( (stlChar*)aValue,
                            sFormat,
                            aBufferLen );
            }
            break;
        case SQL_ATTR_TIME_WITH_TIMEZONE_FORMAT :
            if( aDbc->mTimeWithTimezoneFormat == NULL )
            {
                sFormat = DTL_TIME_WITH_TIME_ZONE_FORMAT_FOR_ODBC;
            }
            else
            {
                sFormat = aDbc->mTimeWithTimezoneFormat;
            }

            sStringLength = stlStrlen( sFormat );
            
            if( aStringLen != NULL )
            {
                *aStringLen = sStringLength;
            }

            if( aValue != NULL )
            {
                stlStrncpy( (stlChar*)aValue,
                            sFormat,
                            aBufferLen );
            }
            break;
        case SQL_ATTR_TIMESTAMP_FORMAT :
            if( aDbc->mTimestampFormat == NULL )
            {
                sFormat = DTL_TIMESTAMP_FORMAT_FOR_ODBC;
            }
            else
            {
                sFormat = aDbc->mTimestampFormat;
            }

            sStringLength = stlStrlen( sFormat );
            
            if( aStringLen != NULL )
            {
                *aStringLen = sStringLength;
            }

            if( aValue != NULL )
            {
                stlStrncpy( (stlChar*)aValue,
                            sFormat,
                            aBufferLen );
            }
            break;
        case SQL_ATTR_TIMESTAMP_WITH_TIMEZONE_FORMAT :
            if( aDbc->mTimestampWithTimezoneFormat == NULL )
            {
                sFormat = DTL_TIMESTAMP_WITH_TIME_ZONE_FORMAT_FOR_ODBC;
            }
            else
            {
                sFormat = aDbc->mTimestampWithTimezoneFormat;
            }

            sStringLength = stlStrlen( sFormat );
            
            if( aStringLen != NULL )
            {
                *aStringLen = sStringLength;
            }

            if( aValue != NULL )
            {
                stlStrncpy( (stlChar*)aValue,
                            sFormat,
                            aBufferLen );
            }
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

/** @} */
