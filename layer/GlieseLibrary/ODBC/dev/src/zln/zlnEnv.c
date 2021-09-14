/*******************************************************************************
 * zlnEnv.c
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

#if defined( ODBC_ALL ) || defined( ODBC_DA )
#include <zlab.h>
#endif

#include <zllDbcNameHash.h>

/**
 * @file zlnEnv.c
 * @brief ODBC API Internal Environment Routines.
 */

/**
 * @addtogroup zlnEnv
 * @{
 */

zlnEnvList gZlnEnvList;
stlUInt32  gZlnCas = 0;

stlIni  gZlnUserDsn;
stlIni  gZlnSystemDsn;
stlBool gZlnExistUserDsn   = STL_FALSE;
stlBool gZlnExistSystemDsn = STL_FALSE;

extern stlErrorRecord gGlieseLibraryErrorTable[];
extern stlInt32       gZlcClientTimezone;

static stlStatus zlnInitialize( stlErrorStack * aErrorStack )
{
    stlUInt32  sOldValue;
    stlExpTime sExpTime; 

    while( 1 )
    {
        sOldValue = stlAtomicCas32( &gZlnCas, 1, 0 );

        if( sOldValue == 0 )
        {
            break;
        }

        stlYieldThread();
    }
    
    if( stlGetProcessInitialized( STL_LAYER_GLIESE_LIBRARY ) == STL_FALSE )
    {
#if defined( ODBC_ALL ) || defined( ODBC_DA )
        STL_TRY( sslInitialize() == STL_SUCCESS );
#else
        STL_TRY( cmlInitialize() == STL_SUCCESS );
#endif

        stlRegisterErrorTable( STL_ERROR_MODULE_GLIESE_LIBRARY_AODBC,
                               gGlieseLibraryErrorTable);

        stlSetProcessInitialized( STL_LAYER_GLIESE_LIBRARY );

        STL_RING_INIT_HEADLINK( &gZlnEnvList.mEnvRing,
                                STL_OFFSETOF(zlnEnv, mLink) );
        gZlnEnvList.mAtomic = 0;

        /*  	 	 
         * OS의 Default Timezone 설정  	 	 
         */  	 	 
        stlMakeExpTimeByLocalTz( &sExpTime, stlNow() );  	 	 

        gZlcClientTimezone = sExpTime.mGMTOffset;  	 	 
    }

    gZlnCas = 0;

    return STL_SUCCESS;

    STL_FINISH;

    gZlnCas = 0;

    return STL_FAILURE;
}

static stlStatus zlnOpenIni( stlErrorStack * aErrorStack )
{
#ifdef WIN32
    return STL_SUCCESS;
#else
    stlChar    sHome[STL_PATH_MAX];
    stlChar    sUserName[STL_PATH_MAX];
    stlChar    sFileName[STL_PATH_MAX];
    stlChar    sODBCINI[STL_PATH_MAX];
    stlChar    sODBCSYSINI[STL_PATH_MAX];
    stlBool    sFound = STL_FALSE;
    stlBool    sExist = STL_FALSE;
    stlUserID  sUserId;
    stlGroupID sGroupId;

    stlChar    sUserBuffer[STL_PATH_MAX];
    stlChar    sGroupBuffer[STL_PATH_MAX];

    /*
     * ODBC_USER_DSN
     *   - ODBCINI
     *   - ~/.odbc.ini
     *   - /home/.odbc.ini
     */

    sFileName[0] = '\0';

    STL_TRY( stlGetCurrentUserID( &sUserId,
                                  sUserBuffer,
                                  STL_PATH_MAX,
                                  &sGroupId,
                                  sGroupBuffer,
                                  STL_PATH_MAX,
                                  aErrorStack ) == STL_SUCCESS );

    STL_TRY( stlGetUserNameByUserID( sUserName,
                                     STL_PATH_MAX,
                                     sUserId,
                                     aErrorStack ) == STL_SUCCESS );

    if( stlGetHomeDirByUserName( sHome,
                                 STL_PATH_MAX,
                                 sUserName,
                                 aErrorStack ) != STL_SUCCESS )
    {
        STL_TRY( stlGetLastErrorCode( aErrorStack ) == STL_ERRCODE_NO_ENTRY );
        (void)stlPopError( aErrorStack );

        stlStrcpy( sHome, "/home" );
    }
    
    STL_TRY( stlGetEnv( sODBCINI,
                        STL_PATH_MAX,
                        "ODBCINI",
                        &sFound,
                        aErrorStack ) == STL_SUCCESS );

    if( sFound == STL_TRUE )
    {
        stlStrncpy( sFileName, sODBCINI, STL_PATH_MAX );
    }
    else
    {
        stlSnprintf( sFileName,
                     STL_PATH_MAX,
                     "%s"STL_PATH_SEPARATOR"%s",
                     sHome,
                     ".odbc.ini" );
    }
    
    STL_TRY( stlExistFile( sFileName,
                           &sExist,
                           aErrorStack ) == STL_SUCCESS );

    if( sExist == STL_TRUE )
    {
        STL_TRY( stlOpenIni( &gZlnUserDsn,
                             sFileName,
                             aErrorStack ) == STL_SUCCESS );

        gZlnExistUserDsn = STL_TRUE;
    }

    /*
     * ODBC_SYSTEM_DSN
     *   - $ODBCSYSINI/odbc.ini
     *   - /etc/odbc.ini
     */

    sFileName[0] = '\0';

    STL_TRY( stlGetEnv( sODBCSYSINI,
                        STL_PATH_MAX,
                        "ODBCSYSINI",
                        &sFound,
                        aErrorStack ) == STL_SUCCESS );

    if( sFound == STL_TRUE )
    {
        stlSnprintf( sFileName,
                     STL_PATH_MAX,
                     "%s"STL_PATH_SEPARATOR"%s",
                     sODBCSYSINI,
                     "odbc.ini" );
    }
    else
    {
        stlSnprintf( sFileName,
                     STL_PATH_MAX,
                     "%s"STL_PATH_SEPARATOR"%s",
                     "/etc",
                     "odbc.ini" );
    }

    STL_TRY( stlExistFile( sFileName,
                           &sExist,
                           aErrorStack ) == STL_SUCCESS );

    if( sExist == STL_TRUE )
    {
        STL_TRY( stlOpenIni( &gZlnSystemDsn,
                             sFileName,
                             aErrorStack ) == STL_SUCCESS );

        gZlnExistSystemDsn = STL_TRUE;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;  
#endif
}

static stlStatus zlnCloseIni( stlErrorStack * aErrorStack )
{
#ifdef WIN32
    return STL_SUCCESS;
#else
    if( gZlnExistUserDsn == STL_TRUE )
    {
        STL_TRY( stlCloseIni( &gZlnUserDsn,
                              aErrorStack ) == STL_SUCCESS );

        gZlnExistUserDsn = STL_FALSE;
    }

    if( gZlnExistSystemDsn == STL_TRUE )
    {
        STL_TRY( stlCloseIni( &gZlnSystemDsn,
                              aErrorStack ) == STL_SUCCESS );

        gZlnExistSystemDsn = STL_FALSE;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
#endif
}

stlStatus zlnAlloc( void          ** aEnv,
                    stlErrorStack  * aErrorStack )
{
    zlnEnv    * sEnv = NULL;
    stlInt32    sState = 0;
    stlUInt32   sOldValue;

    STL_TRY( zlnInitialize( aErrorStack ) == STL_SUCCESS );
    
    STL_TRY( stlAllocHeap( (void**)&sEnv,
                           STL_SIZEOF( zlnEnv ),
                           aErrorStack ) == STL_SUCCESS );
    stlMemset( sEnv, 0x00, STL_SIZEOF( zlnEnv ) );
    sState = 1;

    STL_TRY( zldDiagInit( SQL_HANDLE_ENV,
                          (void*)sEnv,
                          aErrorStack ) == STL_SUCCESS );
    sState = 2;

    STL_RING_INIT_HEADLINK( &sEnv->mDbcRing,
                            STL_OFFSETOF( zlcDbc, mLink ) );

    STL_RING_INIT_DATALINK( sEnv, STL_OFFSETOF( zlnEnv, mLink ) );

    while( 1 )
    {
        sOldValue = stlAtomicCas32( &gZlnEnvList.mAtomic, 1, 0 );

        if( sOldValue == 0 )
        {
            sState = 3;
            
            if( STL_RING_IS_EMPTY( &gZlnEnvList.mEnvRing ) == STL_TRUE )
            {
                STL_TRY( zlnOpenIni( aErrorStack ) == STL_SUCCESS );

                /* 첫번째 Env 할당시에 DBC name hash를 생성 */
                STL_TRY( zllInitializeDbcHash( aErrorStack ) == STL_SUCCESS );
            }

            STL_RING_ADD_LAST( &gZlnEnvList.mEnvRing, sEnv );
            break;
        }

        stlYieldThread();
    }

    sState = 2;
    gZlnEnvList.mAtomic = 0;
    
    sEnv->mType   = SQL_HANDLE_ENV;
    sEnv->mAtomic = 0;

    *aEnv = (void*)sEnv;

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 3 :
            gZlnEnvList.mAtomic = 0;
        case 2 :
            (void)zldDiagFini( SQL_HANDLE_ENV,
                               (void*)sEnv,
                               aErrorStack );
        case 1 :
            stlFreeHeap( sEnv );
            sEnv = NULL;
        default :
            break;
    }

    return STL_FAILURE;
}

stlStatus zlnFree( zlnEnv        * aEnv,
                   stlErrorStack * aErrorStack )
{
    stlUInt32 sOldValue;

    while( 1 )
    {
        sOldValue = stlAtomicCas32( &gZlnEnvList.mAtomic, 1, 0 );

        if( sOldValue == 0 )
        {
            STL_RING_UNLINK( &gZlnEnvList.mEnvRing, aEnv );

            if( STL_RING_IS_EMPTY( &gZlnEnvList.mEnvRing ) == STL_TRUE )
            {
                /* 마지막 Env 해제시에 DBC name hash를 제거 */
                (void)zllFinalizeDbcHash( aErrorStack );

                (void)zlnCloseIni( aErrorStack );
#if defined( ODBC_ALL ) || defined( ODBC_DA )
                (void)zlabCoolDown( aErrorStack );
#endif
            }
            break;
        }

        stlYieldThread();
    }

    gZlnEnvList.mAtomic = 0;
    
    (void)zldDiagFini( SQL_HANDLE_ENV,
                       (void*)aEnv,
                       aErrorStack );
    
    stlMemset( aEnv, 0x00, STL_SIZEOF( zlnEnv ) );

    stlFreeHeap( aEnv );
    aEnv = NULL;

    return STL_SUCCESS;
}

stlStatus zlnSetAttr( zlnEnv        * aEnv,
                      stlInt32        aAttr,
                      void          * aValue,
                      stlInt32        aStrLen,
                      stlErrorStack * aErrorStack )
{
    switch( aAttr )
    {
        case SQL_ATTR_ODBC_VERSION :
            aEnv->mAttrVersion = (stlInt32)STL_INT_FROM_POINTER(aValue);
            break;
        default:
            STL_THROW( RAMP_ERR_INVALID_ATTRIBUTE_OPTION_IDENTIFIER );
            break;
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_ATTRIBUTE_OPTION_IDENTIFIER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_ATTRIBUTE_OPTION_IDENTIFIER,
                      "The value specified for the argument Attribute was not valid "
                      "for the version of ODBC supported by the driver.",
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zlnGetAttr( zlnEnv        * aEnv,
                      stlInt32        aAttr,
                      void          * aValue,
                      stlInt32        aBufferLen,
                      stlInt32      * aStringLen,
                      stlErrorStack * aErrorStack )
{
    switch( aAttr )
    {
        case SQL_ATTR_ODBC_VERSION :
            *(stlUInt32*)aValue = aEnv->mAttrVersion;
            break;
        default:
            STL_THROW( RAMP_ERR_INVALID_ATTRIBUTE_OPTION_IDENTIFIER );
            break;
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_ATTRIBUTE_OPTION_IDENTIFIER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_ATTRIBUTE_OPTION_IDENTIFIER,
                      "The value specified for the argument Attribute was not valid "
                      "for the version of ODBC supported by the driver.",
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
