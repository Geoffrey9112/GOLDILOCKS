/*******************************************************************************
 * zllXa.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: zllXa.c 12017 2014-04-15 05:44:19Z mae113 $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#include <cml.h>
#include <goldilocks.h>

#include <zlDef.h>
#include <zln.h>
#include <zlc.h>
#include <zllDbcNameHash.h>

#include <zle.h>
#include <zlo.h>

/**
 * @file zllXa.c
 * @brief XA API Routines
 */

extern zlnEnvList gZlnEnvList;

zlnEnv * gZlXaEnv = NULL;

/**
 * @addtogroup zllXa
 * @{
 */

#define ZLL_COPY_XID_TO_SXID( aSxid, aXid )                             \
    {                                                                   \
        (aSxid)->mFormatId = (aXid)->formatID;                          \
        (aSxid)->mTransLength = (aXid)->gtrid_length;                   \
        (aSxid)->mBranchLength = (aXid)->bqual_length;                  \
        stlMemcpy( (aSxid)->mData, (aXid)->data, STL_XID_DATA_SIZE );   \
    }
    
#define ZLL_COPY_SXID_TO_XID( aXid, aSxid )                             \
    {                                                                   \
        (aXid)->formatID = (aSxid)->mFormatId;                          \
        (aXid)->gtrid_length = (aSxid)->mTransLength;                   \
        (aXid)->bqual_length = (aSxid)->mBranchLength;                  \
        stlMemcpy( (aXid)->data, (aSxid)->mData, STL_XID_DATA_SIZE );   \
    }

static stlStatus zllFindMyDbc( zlcDbc        ** aDbc,
                               stlInt32       * aRmId,
                               stlErrorStack  * aErrorStack )
{
    zlnEnv          * sEnv;
    stlBool           sFind = STL_FALSE;
    stlUInt32         sOldValue1;
    stlUInt32         sOldValue2;
    stlThreadHandle   sThread;
    zlcDbc          * sMyDbc = NULL;
    zlcDbc          * sDbc = NULL;

    STL_TRY( stlThreadSelf( &sThread,
                            aErrorStack ) == STL_SUCCESS );

    while( 1 )
    {
        sOldValue1 = stlAtomicCas32( &gZlnEnvList.mAtomic, 1, 0 );

        if( sOldValue1 == 0 )
        {
            STL_RING_FOREACH_ENTRY( &gZlnEnvList.mEnvRing, sEnv )
            {
                while( 1 )
                {
                    sOldValue2 = stlAtomicCas32( &sEnv->mAtomic, 1, 0 );

                    if( sOldValue2 == 0 )
                    {
                        STL_RING_FOREACH_ENTRY( &sEnv->mDbcRing, sDbc )
                        {
                            if( stlIsEqualThread( &sDbc->mThread, &sThread ) == STL_TRUE )
                            {
                                if( aRmId == NULL )
                                {
                                    sMyDbc = sDbc;

                                    sFind = STL_TRUE;
                                    break;
                                }
                                else
                                {
                                    if( *aRmId == sDbc->mRmId )
                                    {
                                        sMyDbc = sDbc;

                                        sFind = STL_TRUE;
                                        break;
                                    }
                                }
                            }
                        }
            
                        break;
                    }

                    stlYieldThread();
                }

                sEnv->mAtomic = 0;
                
                if( sFind == STL_TRUE )
                {
                    break;
                }
            }
            
            break;
        }

        stlYieldThread();
    }

    gZlnEnvList.mAtomic = 0;
    
    *aDbc = sMyDbc;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlInt32 zllXaOpen( stlChar  * aXaInfo,
                    stlInt32   aRmId,
                    long       aFlags )
{
    zlnEnv        * sEnv;
    stlErrorStack   sErrorStack;
    stlInt32        sReturn = XA_OK;
    zlcDbc        * sDbc = NULL;
    zlcDbc        * sTempDbc = NULL;
    stlInt16        sXaInfoLength = 0;
    stlChar         sConnName[MAXINFOSIZE];
    stlInt16        sConnNameLength;
    stlBool         sSuccessWithInfo = STL_FALSE;
    stlBool         sNoData = STL_FALSE;

    STL_INIT_ERROR_STACK( &sErrorStack );

    STL_TRY_THROW( aFlags == TMNOFLAGS, RAMP_ERR_INVAL );

    if( gZlXaEnv == NULL )
    {
        if( STL_RING_IS_EMPTY( &gZlnEnvList.mEnvRing ) == STL_TRUE )
        {
            STL_TRY_THROW( zlnAlloc( (SQLHENV*)&sEnv,
                                     &sErrorStack ) == STL_SUCCESS,
                           RAMP_ERR_ENV );

            STL_TRY_THROW( zlnSetAttr( sEnv,
                                       SQL_ATTR_ODBC_VERSION,
                                       (SQLPOINTER)SQL_OV_ODBC3,
                                       0,
                                       &sErrorStack ) == STL_SUCCESS,
                           RAMP_ERR_ENV );
        }
        else
        {
            sEnv = STL_RING_GET_FIRST_DATA( &gZlnEnvList.mEnvRing );
        }

        gZlXaEnv = sEnv;
    }
    else
    {
        sEnv = gZlXaEnv;
    }

    STL_TRY_THROW( zllFindMyDbc( &sDbc, &aRmId, &sErrorStack ) == STL_SUCCESS, RAMP_ERR_DBC );

    if( sDbc == NULL )
    {
        STL_TRY_THROW( zlcAlloc( sEnv,
                                 (SQLHDBC*)&sDbc,
                                 &sErrorStack ) == STL_SUCCESS,
                       RAMP_ERR_DBC );

        if( aXaInfo == NULL )
        {
            sXaInfoLength = 0;
        }
        else
        {
            sXaInfoLength = stlStrlen( aXaInfo );
        }

        /*
         * Connection Name만 Parsing
         */
        STL_TRY( zlcGetConnName( aXaInfo,
                                 sXaInfoLength,
                                 sConnName,
                                 &sConnNameLength,
                                 &sErrorStack )
                 == STL_SUCCESS );

        /*
         * xa_info에 CONN_NAME 항목이 주어진 경우
         */
        if( sConnNameLength > 0 )
        {
            /*
             * 주어진 Connection Name으로 기존에 Connection이 존재하는지 확인
             */
            STL_TRY( zllFindDbcFromNameHash( (void **)&sTempDbc,
                                             sConnName,
                                             &sErrorStack )
                     == STL_SUCCESS );

            /*
             * 기존에 같은 이름의 Connection이 존재한다면 Error
             */
            STL_TRY_THROW( sTempDbc == NULL, RAMP_ERR_INVAL );
        }

        STL_TRY_THROW( zlcDriverConnect( sDbc,
                                         NULL,
                                         (stlChar*)aXaInfo,
                                         sXaInfoLength,
                                         NULL,
                                         0,
                                         NULL,
                                         SQL_DRIVER_NOPROMPT,
                                         &sSuccessWithInfo,
                                         &sNoData,
                                         &sErrorStack ) == STL_SUCCESS,
                       RAMP_ERR_DBC );

        /*
         * Connection Name 해쉬 테이블에 DBC를 삽입한다.
         */
        sDbc->mRmId = aRmId;
        if( sConnNameLength > 0 )
        {
            STL_TRY( zllSetDbcName( sDbc,
                                    sConnName,
                                    &sErrorStack )
                     == STL_SUCCESS );

            STL_DASSERT( sDbc->mDbcName != NULL );

            STL_TRY_THROW( zllAddDbcToNameHash( sDbc,
                                                &sErrorStack ) == STL_SUCCESS,
                           RAMP_ERR_DBC );
        }
        else
        {
            sDbc->mDbcName = NULL;
        }
    }
    
    return sReturn;
    
    STL_CATCH( RAMP_ERR_ENV )
    {
        sReturn = XAER_RMFAIL;
    }
    
    STL_CATCH( RAMP_ERR_DBC )
    {
        sReturn = XAER_RMERR;
    }
    
    STL_CATCH( RAMP_ERR_INVAL )
    {
        sReturn = XAER_INVAL;
    }
    
    STL_FINISH;

    if( sDbc != NULL )
    {
        zlcFree( sDbc, &sErrorStack );
    }
    
    return sReturn;
}

stlInt32 zllXaClose( stlChar  * aXaInfo,
                     stlInt32   aRmId,
                     long       aFlags )
{
    stlErrorStack   sErrorStack;
    stlErrorData  * sErrorData = NULL;
    stlInt32        sReturn = XA_OK;
    zlcDbc        * sDbc = NULL;

    STL_INIT_ERROR_STACK( &sErrorStack );

    STL_TRY_THROW( aFlags == TMNOFLAGS, RAMP_ERR_INVAL );
    
    STL_TRY_THROW( zllFindMyDbc( &sDbc,
                                 &aRmId,
                                 &sErrorStack ) == STL_SUCCESS,
                   RAMP_ERR_GENERAL );

    if( sDbc != NULL )
    {
        if( sDbc->mDbcName != NULL )
        {
            STL_TRY_THROW( zllRemoveDbcFromNameHash( sDbc,
                                                     &sErrorStack ) == STL_SUCCESS,
                           RAMP_ERR_GENERAL );
        }

        STL_TRY( zloClose( sDbc,
                           &sReturn,
                           &sErrorStack ) == STL_SUCCESS );

        STL_TRY_THROW( zlcDisconnect( sDbc,
                                      &sErrorStack ) == STL_SUCCESS,
                       RAMP_ERR_GENERAL );

        STL_TRY_THROW( zlcFree( sDbc,
                                &sErrorStack ) == STL_SUCCESS,
                       RAMP_ERR_GENERAL );
    }

    return sReturn;
    
    STL_CATCH( RAMP_ERR_GENERAL )
    {
        sReturn = XAER_RMERR;
    }
    
    STL_CATCH( RAMP_ERR_INVAL )
    {
        sReturn = XAER_INVAL;
    }
    
    STL_FINISH;

    if( sDbc != NULL )
    {
        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            if( stlGetExternalErrorCode( sErrorData ) ==
                STL_EXT_ERRCODE_CONNECTION_EXCEPTION_COMMUNICATION_LINK_FAILURE )
            {
                sDbc->mBrokenConnection = STL_TRUE;
            }
        }
        
        zlcCheckSessionAndCleanup( sDbc );
    }

    return sReturn;
}

stlInt32 zllXaStart( XID      * aXid,
                     stlInt32   aRmId,
                     long       aFlags )
{
    stlErrorStack   sErrorStack;
    stlErrorData  * sErrorData = NULL;
    stlInt32        sReturn = XA_OK;
    zlcDbc        * sDbc = NULL;
    stlXid          sXid;

    STL_INIT_ERROR_STACK( &sErrorStack );
    
    STL_TRY_THROW( !(aFlags & ~(TMJOIN | TMRESUME | TMNOWAIT)),
                   RAMP_ERR_INVAL );

    ZLL_COPY_XID_TO_SXID( &sXid, aXid );

    STL_TRY_THROW( dtlValidateXid( &sXid ) == STL_TRUE,
                   RAMP_ERR_INVAL );
    
    STL_TRY_THROW( zllFindMyDbc( &sDbc,
                                 &aRmId,
                                 &sErrorStack ) == STL_SUCCESS,
                   RAMP_ERR_GENERAL );

    STL_TRY_THROW( sDbc != NULL, RAMP_ERR_DBC );

    STL_TRY( zloStart( sDbc,
                       &sXid,
                       aRmId,
                       aFlags,
                       &sReturn,
                       &sErrorStack ) == STL_SUCCESS );

    return sReturn;

    STL_CATCH( RAMP_ERR_GENERAL )
    {
        sReturn = XAER_RMERR;
    }
    
    STL_CATCH( RAMP_ERR_DBC )
    {
        sReturn = XAER_RMFAIL;
    }
    
    STL_CATCH( RAMP_ERR_INVAL )
    {
        sReturn = XAER_INVAL;
    }
    
    STL_FINISH;

    if( sDbc != NULL )
    {
        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            if( stlGetExternalErrorCode( sErrorData ) ==
                STL_EXT_ERRCODE_CONNECTION_EXCEPTION_COMMUNICATION_LINK_FAILURE )
            {
                sDbc->mBrokenConnection = STL_TRUE;
            }
        }
        
        zlcCheckSessionAndCleanup( sDbc );
    }

    return sReturn;
}

stlInt32 zllXaEnd( XID      * aXid,
                   stlInt32   aRmId,
                   long       aFlags )
{
    stlErrorStack   sErrorStack;
    stlErrorData  * sErrorData = NULL;
    stlInt32        sReturn = XA_OK;
    zlcDbc        * sDbc = NULL;
    stlXid          sXid;

    STL_INIT_ERROR_STACK( &sErrorStack );
    
    STL_TRY_THROW( !(aFlags & ~(TMSUSPEND | TMMIGRATE | TMSUCCESS | TMFAIL)),
                   RAMP_ERR_INVAL );
    
    ZLL_COPY_XID_TO_SXID( &sXid, aXid );

    STL_TRY_THROW( dtlValidateXid( &sXid ) == STL_TRUE,
                   RAMP_ERR_INVAL );
    
    STL_TRY_THROW( zllFindMyDbc( &sDbc,
                                 &aRmId,
                                 &sErrorStack ) == STL_SUCCESS,
                   RAMP_ERR_GENERAL );

    STL_TRY_THROW( sDbc != NULL, RAMP_ERR_DBC );

    STL_TRY( zloEnd( sDbc,
                     &sXid,
                     aRmId,
                     aFlags,
                     &sReturn,
                     &sErrorStack ) == STL_SUCCESS );

    if( sReturn == XA_OK )
    {
        if( STL_RING_IS_EMPTY( &sDbc->mStmtRing ) == STL_TRUE )
        {
            sDbc->mTransition = ZLC_TRANSITION_STATE_C4;
        }
        else
        {
            sDbc->mTransition = ZLC_TRANSITION_STATE_C5;
        }
    }

    return sReturn;

    STL_CATCH( RAMP_ERR_GENERAL )
    {
        sReturn = XAER_RMERR;
    }
    
    STL_CATCH( RAMP_ERR_DBC )
    {
        sReturn = XAER_RMFAIL;
    }
    
    STL_CATCH( RAMP_ERR_INVAL )
    {
        sReturn = XAER_INVAL;
    }
    
    STL_FINISH;

    if( sDbc != NULL )
    {
        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            if( stlGetExternalErrorCode( sErrorData ) ==
                STL_EXT_ERRCODE_CONNECTION_EXCEPTION_COMMUNICATION_LINK_FAILURE )
            {
                sDbc->mBrokenConnection = STL_TRUE;
            }
        }
        
        zlcCheckSessionAndCleanup( sDbc );
    }

    return sReturn;
}

stlInt32 zllXaRollback( XID      * aXid,
                        stlInt32   aRmId,
                        long       aFlags )
{
    stlErrorStack   sErrorStack;
    stlErrorData  * sErrorData = NULL;
    stlInt32        sReturn = XA_OK;
    zlcDbc        * sDbc = NULL;
    stlXid          sXid;

    STL_INIT_ERROR_STACK( &sErrorStack );
    
    STL_TRY_THROW( aFlags == TMNOFLAGS, RAMP_ERR_INVAL );
    
    ZLL_COPY_XID_TO_SXID( &sXid, aXid );
    
    STL_TRY_THROW( dtlValidateXid( &sXid ) == STL_TRUE,
                   RAMP_ERR_INVAL );
    
    STL_TRY_THROW( zllFindMyDbc( &sDbc,
                                 &aRmId,
                                 &sErrorStack ) == STL_SUCCESS,
                   RAMP_ERR_GENERAL );

    STL_TRY_THROW( sDbc != NULL, RAMP_ERR_DBC );

    STL_TRY( zloRollback( sDbc,
                          &sXid,
                          aRmId,
                          aFlags,
                          &sReturn,
                          &sErrorStack ) == STL_SUCCESS );
    
    return sReturn;

    STL_CATCH( RAMP_ERR_GENERAL )
    {
        sReturn = XAER_RMERR;
    }
    
    STL_CATCH( RAMP_ERR_DBC )
    {
        sReturn = XAER_RMFAIL;
    }
    
    STL_CATCH( RAMP_ERR_INVAL )
    {
        sReturn = XAER_INVAL;
    }
    
    STL_FINISH;

    if( sDbc != NULL )
    {
        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            if( stlGetExternalErrorCode( sErrorData ) ==
                STL_EXT_ERRCODE_CONNECTION_EXCEPTION_COMMUNICATION_LINK_FAILURE )
            {
                sDbc->mBrokenConnection = STL_TRUE;
            }
        }
        
        zlcCheckSessionAndCleanup( sDbc );
    }
    
    return sReturn;
}

stlInt32 zllXaPrepare( XID      * aXid,
                       stlInt32   aRmId,
                       long       aFlags )
{
    stlErrorStack   sErrorStack;
    stlErrorData  * sErrorData = NULL;
    stlInt32        sReturn = XA_OK;
    zlcDbc        * sDbc = NULL;
    stlXid          sXid;

    STL_INIT_ERROR_STACK( &sErrorStack );
    
    STL_TRY_THROW( aFlags == TMNOFLAGS, RAMP_ERR_INVAL );
    
    ZLL_COPY_XID_TO_SXID( &sXid, aXid );
    
    STL_TRY_THROW( dtlValidateXid( &sXid ) == STL_TRUE,
                   RAMP_ERR_INVAL );
    
    STL_TRY_THROW( zllFindMyDbc( &sDbc,
                                 &aRmId,
                                 &sErrorStack ) == STL_SUCCESS,
                   RAMP_ERR_GENERAL );

    STL_TRY_THROW( sDbc != NULL, RAMP_ERR_DBC );

    STL_TRY( zloPrepare( sDbc,
                         &sXid,
                         aRmId,
                         aFlags,
                         &sReturn,
                         &sErrorStack ) == STL_SUCCESS );
    
    return sReturn;

    STL_CATCH( RAMP_ERR_GENERAL )
    {
        sReturn = XAER_RMERR;
    }
    
    STL_CATCH( RAMP_ERR_DBC )
    {
        sReturn = XAER_RMFAIL;
    }
    
    STL_CATCH( RAMP_ERR_INVAL )
    {
        sReturn = XAER_INVAL;
    }
    
    STL_FINISH;

    if( sDbc != NULL )
    {
        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            if( stlGetExternalErrorCode( sErrorData ) ==
                STL_EXT_ERRCODE_CONNECTION_EXCEPTION_COMMUNICATION_LINK_FAILURE )
            {
                sDbc->mBrokenConnection = STL_TRUE;
            }
        }
        
        zlcCheckSessionAndCleanup( sDbc );
    }
    
    return sReturn;
}

stlInt32 zllXaCommit( XID      * aXid,
                      stlInt32   aRmId,
                      long       aFlags )
{
    stlErrorStack   sErrorStack;
    stlErrorData  * sErrorData = NULL;
    stlInt32        sReturn = XA_OK;
    zlcDbc        * sDbc = NULL;
    stlXid          sXid;

    STL_INIT_ERROR_STACK( &sErrorStack );
    
    STL_TRY_THROW( !(aFlags & ~(TMNOWAIT | TMONEPHASE)),
                   RAMP_ERR_INVAL );
    
    ZLL_COPY_XID_TO_SXID( &sXid, aXid );
    
    STL_TRY_THROW( dtlValidateXid( &sXid ) == STL_TRUE,
                   RAMP_ERR_INVAL );
    
    STL_TRY_THROW( zllFindMyDbc( &sDbc,
                                 &aRmId,
                                 &sErrorStack ) == STL_SUCCESS,
                   RAMP_ERR_GENERAL );

    STL_TRY_THROW( sDbc != NULL, RAMP_ERR_DBC );

    STL_TRY( zloCommit( sDbc,
                        &sXid,
                        aRmId,
                        aFlags,
                        &sReturn,
                        &sErrorStack ) == STL_SUCCESS );
    
    return sReturn;

    STL_CATCH( RAMP_ERR_GENERAL )
    {
        sReturn = XAER_RMERR;
    }
    
    STL_CATCH( RAMP_ERR_DBC )
    {
        sReturn = XAER_RMFAIL;
    }
    
    STL_CATCH( RAMP_ERR_INVAL )
    {
        sReturn = XAER_INVAL;
    }
    
    STL_FINISH;

    if( sDbc != NULL )
    {
        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            if( stlGetExternalErrorCode( sErrorData ) ==
                STL_EXT_ERRCODE_CONNECTION_EXCEPTION_COMMUNICATION_LINK_FAILURE )
            {
                sDbc->mBrokenConnection = STL_TRUE;
            }
        }
        
        zlcCheckSessionAndCleanup( sDbc );
    }
    
    return sReturn;
}

stlInt32 zllXaRecover( XID      * aXids,
                       long       aCount,
                       stlInt32   aRmId,
                       long       aFlags )
{
    stlErrorStack   sErrorStack;
    stlErrorData  * sErrorData = NULL;
    stlInt32        sReturn = XA_OK;
    zlcDbc        * sDbc = NULL;
    stlXid        * sXids = NULL;
    stlInt32        sState = 0;
    stlInt32        i;

    STL_INIT_ERROR_STACK( &sErrorStack );
    
    STL_TRY_THROW( !(aFlags & ~(TMSTARTRSCAN | TMENDRSCAN)),
                   RAMP_ERR_INVAL );
    
    STL_TRY_THROW( zllFindMyDbc( &sDbc,
                                 &aRmId,
                                 &sErrorStack ) == STL_SUCCESS,
                   RAMP_ERR_GENERAL );

    STL_TRY_THROW( sDbc != NULL, RAMP_ERR_DBC );

    STL_TRY( stlAllocHeap( (void**)&sXids,
                           STL_SIZEOF(stlXid) * aCount,
                           &sErrorStack )
             == STL_SUCCESS );
    sState = 1;

    STL_TRY( zloRecover( sDbc,
                         sXids,
                         aCount,
                         aRmId,
                         aFlags,
                         &sReturn,
                         &sErrorStack ) == STL_SUCCESS );

    for( i = 0; i < sReturn; i++ )
    {
        ZLL_COPY_SXID_TO_XID( &aXids[i], &sXids[i] );
    }

    sState = 0;
    stlFreeHeap( sXids );
    sXids = NULL;

    return sReturn;

    STL_CATCH( RAMP_ERR_GENERAL )
    {
        sReturn = XAER_RMERR;
    }
    
    STL_CATCH( RAMP_ERR_DBC )
    {
        sReturn = XAER_RMFAIL;
    }
    
    STL_CATCH( RAMP_ERR_INVAL )
    {
        sReturn = XAER_INVAL;
    }
    
    STL_FINISH;

    if( sDbc != NULL )
    {
        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            if( stlGetExternalErrorCode( sErrorData ) ==
                STL_EXT_ERRCODE_CONNECTION_EXCEPTION_COMMUNICATION_LINK_FAILURE )
            {
                sDbc->mBrokenConnection = STL_TRUE;
            }
        }
        
        zlcCheckSessionAndCleanup( sDbc );
    }

    if( sState == 1 )
    {
        stlFreeHeap( sXids );
        sXids = NULL;
    }
    
    return sReturn;
}

stlInt32 zllXaForget( XID      * aXid,
                      stlInt32   aRmId,
                      long       aFlags )
{
    stlErrorStack   sErrorStack;
    stlErrorData  * sErrorData = NULL;
    stlInt32        sReturn = XA_OK;
    zlcDbc        * sDbc = NULL;
    stlXid          sXid;

    STL_INIT_ERROR_STACK( &sErrorStack );
    
    STL_TRY_THROW( aFlags == TMNOFLAGS, RAMP_ERR_INVAL );
    
    ZLL_COPY_XID_TO_SXID( &sXid, aXid );
    
    STL_TRY_THROW( dtlValidateXid( &sXid ) == STL_TRUE,
                   RAMP_ERR_INVAL );
    
    STL_TRY_THROW( zllFindMyDbc( &sDbc,
                                 &aRmId,
                                 &sErrorStack ) == STL_SUCCESS,
                   RAMP_ERR_GENERAL );

    STL_TRY_THROW( sDbc != NULL, RAMP_ERR_DBC );

    STL_TRY( zloForget( sDbc,
                        &sXid,
                        aRmId,
                        aFlags,
                        &sReturn,
                        &sErrorStack ) == STL_SUCCESS );
    
    return sReturn;

    STL_CATCH( RAMP_ERR_GENERAL )
    {
        sReturn = XAER_RMERR;
    }
    
    STL_CATCH( RAMP_ERR_DBC )
    {
        sReturn = XAER_RMFAIL;
    }
    
    STL_CATCH( RAMP_ERR_INVAL )
    {
        sReturn = XAER_INVAL;
    }
    
    STL_FINISH;

    if( sDbc != NULL )
    {
        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            if( stlGetExternalErrorCode( sErrorData ) ==
                STL_EXT_ERRCODE_CONNECTION_EXCEPTION_COMMUNICATION_LINK_FAILURE )
            {
                sDbc->mBrokenConnection = STL_TRUE;
            }
        }
        
        zlcCheckSessionAndCleanup( sDbc );
    }
    
    return sReturn;
}

xa_switch_t goldilocks_xa_switch =
{
    "GOLDILOCKS",
    TMNOMIGRATE,
    0,
    zllXaOpen,
    zllXaClose,
    zllXaStart,
    zllXaEnd,
    zllXaRollback,
    zllXaPrepare,
    zllXaCommit,
    zllXaRecover,
    zllXaForget,
    NULL
};

/*****************************
 * 비표준 함수
 *****************************/

xa_switch_t* SQLGetXaSwitch()
{
    return &goldilocks_xa_switch;
}

SQLHANDLE SQLGetXaConnectionHandle(int *aRmId)
{
    stlErrorStack   sErrorStack;
    zlcDbc        * sDbc = NULL;

    STL_INIT_ERROR_STACK( &sErrorStack );
    
    STL_TRY( zllFindMyDbc( &sDbc,
                           aRmId,
                           &sErrorStack ) == STL_SUCCESS );

    return sDbc;

    STL_FINISH;

    return NULL;
}

/** @} */
