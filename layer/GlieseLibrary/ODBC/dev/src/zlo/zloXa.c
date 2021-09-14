/*******************************************************************************
 * zlaoXa.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: zlaoXa.c 12017 2014-04-15 05:44:19Z mae113 $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#include <cml.h>
#include <goldilocks.h>
#include <zlDef.h>
#include <zle.h>
#include <zlc.h>

#include <zlao.h>
#include <zlco.h>

/**
 * @file zlaoXa.c
 * @brief ODBC API Internal XA Routines.
 */

/**
 * @addtogroup zloXa
 * @{
 */

#if defined( ODBC_ALL )
stlStatus (*gZloCloseFunc[ZLC_PROTOCOL_TYPE_MAX]) ( zlcDbc        * aDbc,
                                                    stlInt32      * aReturn,
                                                    stlErrorStack * aErrorStack ) =
{
    zlaoClose,
    zlcoClose
};

stlStatus (*gZloStartFunc[ZLC_PROTOCOL_TYPE_MAX]) ( zlcDbc        * aDbc,
                                                    stlXid        * aXid,
                                                    stlInt32        aRmId,
                                                    stlInt32        aFlags,
                                                    stlInt32      * aReturn,
                                                    stlErrorStack * aErrorStack ) =
{
    zlaoStart,
    zlcoStart
};

stlStatus (*gZloEndFunc[ZLC_PROTOCOL_TYPE_MAX]) ( zlcDbc        * aDbc,
                                                  stlXid        * aXid,
                                                  stlInt32        aRmId,
                                                  stlInt32        aFlags,
                                                  stlInt32      * aReturn,
                                                  stlErrorStack * aErrorStack ) =
{
    zlaoEnd,
    zlcoEnd
};

stlStatus (*gZloRollbackFunc[ZLC_PROTOCOL_TYPE_MAX]) ( zlcDbc        * aDbc,
                                                       stlXid        * aXid,
                                                       stlInt32        aRmId,
                                                       stlInt32        aFlags,
                                                       stlInt32      * aReturn,
                                                       stlErrorStack * aErrorStack ) =
{
    zlaoRollback,
    zlcoRollback
};

stlStatus (*gZloPrepareFunc[ZLC_PROTOCOL_TYPE_MAX]) ( zlcDbc        * aDbc,
                                                      stlXid        * aXid,
                                                      stlInt32        aRmId,
                                                      stlInt32        aFlags,
                                                      stlInt32      * aReturn,
                                                      stlErrorStack * aErrorStack ) =
{
    zlaoPrepare,
    zlcoPrepare
};

stlStatus (*gZloCommitFunc[ZLC_PROTOCOL_TYPE_MAX]) ( zlcDbc        * aDbc,
                                                     stlXid        * aXid,
                                                     stlInt32        aRmId,
                                                     stlInt32        aFlags,
                                                     stlInt32      * aReturn,
                                                     stlErrorStack * aErrorStack ) =
{
    zlaoCommit,
    zlcoCommit
};

stlStatus (*gZloRecoverFunc[ZLC_PROTOCOL_TYPE_MAX]) ( zlcDbc        * aDbc,
                                                      stlXid        * aXids,
                                                      stlInt32        aCount,
                                                      stlInt32        aRmId,
                                                      stlInt32        aFlags,
                                                      stlInt32      * aReturn,
                                                      stlErrorStack * aErrorStack ) =
{
    zlaoRecover,
    zlcoRecover
};

stlStatus (*gZloForgetFunc[ZLC_PROTOCOL_TYPE_MAX]) ( zlcDbc        * aDbc,
                                                     stlXid        * aXid,
                                                     stlInt32        aRmId,
                                                     stlInt32        aFlags,
                                                     stlInt32      * aReturn,
                                                     stlErrorStack * aErrorStack ) =
{
    zlaoForget,
    zlcoForget
};
#endif

stlStatus zloClose( zlcDbc        * aDbc,
                    stlInt32      * aReturn,
                    stlErrorStack * aErrorStack )
{
#if defined( ODBC_ALL )
    STL_TRY( gZloCloseFunc[aDbc->mProtocolType]( aDbc,
                                                 aReturn,
                                                 aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_DA )
    STL_TRY( zlaoClose( aDbc,
                        aReturn,
                        aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_CS )
    STL_TRY( zlcoClose( aDbc,
                        aReturn,
                        aErrorStack ) == STL_SUCCESS );
#else
    STL_ASSERT( STL_FALSE );
#endif

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zloStart( zlcDbc        * aDbc,
                    stlXid        * aXid,
                    stlInt32        aRmId,
                    stlInt32        aFlags,
                    stlInt32      * aReturn,
                    stlErrorStack * aErrorStack )
{
    /**
     * XA Connection은 Autocommit off를 기본으로 한다.
     */
        
    STL_TRY_THROW( zlcSetAttr( aDbc,
                               SQL_AUTOCOMMIT,
                               (SQLPOINTER)SQL_AUTOCOMMIT_OFF,
                               SQL_IS_UINTEGER,
                               aErrorStack ) == STL_SUCCESS,
                   RAMP_ERR_GENERAL );

#if defined( ODBC_ALL )
    STL_TRY( gZloStartFunc[aDbc->mProtocolType]( aDbc,
                                                 aXid,
                                                 aRmId,
                                                 aFlags,
                                                 aReturn,
                                                 aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_DA )
    STL_TRY( zlaoStart( aDbc,
                        aXid,
                        aRmId,
                        aFlags,
                        aReturn,
                        aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_CS )
    STL_TRY( zlcoStart( aDbc,
                        aXid,
                        aRmId,
                        aFlags,
                        aReturn,
                        aErrorStack ) == STL_SUCCESS );
#else
    STL_ASSERT( STL_FALSE );
#endif
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_GENERAL )
    {
        *aReturn = XAER_RMERR;
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zloEnd( zlcDbc        * aDbc,
                  stlXid        * aXid,
                  stlInt32        aRmId,
                  stlInt32        aFlags,
                  stlInt32      * aReturn,
                  stlErrorStack * aErrorStack )
{
#if defined( ODBC_ALL )
    STL_TRY( gZloEndFunc[aDbc->mProtocolType]( aDbc,
                                               aXid,
                                               aRmId,
                                               aFlags,
                                               aReturn,
                                               aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_DA )
    STL_TRY( zlaoEnd( aDbc,
                      aXid,
                      aRmId,
                      aFlags,
                      aReturn,
                      aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_CS )
    STL_TRY( zlcoEnd( aDbc,
                      aXid,
                      aRmId,
                      aFlags,
                      aReturn,
                      aErrorStack ) == STL_SUCCESS );
#else
    STL_ASSERT( STL_FALSE );
#endif
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zloRollback( zlcDbc        * aDbc,
                       stlXid        * aXid,
                       stlInt32        aRmId,
                       stlInt32        aFlags,
                       stlInt32      * aReturn,
                       stlErrorStack * aErrorStack )
{
#if defined( ODBC_ALL )
    STL_TRY( gZloRollbackFunc[aDbc->mProtocolType]( aDbc,
                                                    aXid,
                                                    aRmId,
                                                    aFlags,
                                                    aReturn,
                                                    aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_DA )
    STL_TRY( zlaoRollback( aDbc,
                           aXid,
                           aRmId,
                           aFlags,
                           aReturn,
                           aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_CS )
    STL_TRY( zlcoRollback( aDbc,
                           aXid,
                           aRmId,
                           aFlags,
                           aReturn,
                           aErrorStack ) == STL_SUCCESS );
#else
    STL_ASSERT( STL_FALSE );
#endif
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zloPrepare( zlcDbc        * aDbc,
                      stlXid        * aXid,
                      stlInt32        aRmId,
                      stlInt32        aFlags,
                      stlInt32      * aReturn,
                      stlErrorStack * aErrorStack )
{
#if defined( ODBC_ALL )
    STL_TRY( gZloPrepareFunc[aDbc->mProtocolType]( aDbc,
                                                   aXid,
                                                   aRmId,
                                                   aFlags,
                                                   aReturn,
                                                   aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_DA )
    STL_TRY( zlaoPrepare( aDbc,
                          aXid,
                          aRmId,
                          aFlags,
                          aReturn,
                          aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_CS )
    STL_TRY( zlcoPrepare( aDbc,
                          aXid,
                          aRmId,
                          aFlags,
                          aReturn,
                          aErrorStack ) == STL_SUCCESS );
#else
    STL_ASSERT( STL_FALSE );
#endif
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zloCommit( zlcDbc        * aDbc,
                     stlXid        * aXid,
                     stlInt32        aRmId,
                     stlInt32        aFlags,
                     stlInt32      * aReturn,
                     stlErrorStack * aErrorStack )
{
#if defined( ODBC_ALL )
    STL_TRY( gZloCommitFunc[aDbc->mProtocolType]( aDbc,
                                                  aXid,
                                                  aRmId,
                                                  aFlags,
                                                  aReturn,
                                                  aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_DA )
    STL_TRY( zlaoCommit( aDbc,
                         aXid,
                         aRmId,
                         aFlags,
                         aReturn,
                         aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_CS )
    STL_TRY( zlcoCommit( aDbc,
                         aXid,
                         aRmId,
                         aFlags,
                         aReturn,
                         aErrorStack ) == STL_SUCCESS );
#else
    STL_ASSERT( STL_FALSE );
#endif
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zloRecover( zlcDbc        * aDbc,
                      stlXid        * aXids,
                      stlInt32        aCount,
                      stlInt32        aRmId,
                      stlInt32        aFlags,
                      stlInt32      * aReturn,
                      stlErrorStack * aErrorStack )
{
#if defined( ODBC_ALL )
    STL_TRY( gZloRecoverFunc[aDbc->mProtocolType]( aDbc,
                                                   aXids,
                                                   aCount,
                                                   aRmId,
                                                   aFlags,
                                                   aReturn,
                                                   aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_DA )
    STL_TRY( zlaoRecover( aDbc,
                          aXids,
                          aCount,
                          aRmId,
                          aFlags,
                          aReturn,
                          aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_CS )
    STL_TRY( zlcoRecover( aDbc,
                          aXids,
                          aCount,
                          aRmId,
                          aFlags,
                          aReturn,
                          aErrorStack ) == STL_SUCCESS );
#else
    STL_ASSERT( STL_FALSE );
#endif
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zloForget( zlcDbc        * aDbc,
                     stlXid        * aXid,
                     stlInt32        aRmId,
                     stlInt32        aFlags,
                     stlInt32      * aReturn,
                     stlErrorStack * aErrorStack )
{
#if defined( ODBC_ALL )
    STL_TRY( gZloForgetFunc[aDbc->mProtocolType]( aDbc,
                                                  aXid,
                                                  aRmId,
                                                  aFlags,
                                                  aReturn,
                                                  aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_DA )
    STL_TRY( zlaoForget( aDbc,
                         aXid,
                         aRmId,
                         aFlags,
                         aReturn,
                         aErrorStack ) == STL_SUCCESS );
#elif defined( ODBC_CS )
    STL_TRY( zlcoForget( aDbc,
                         aXid,
                         aRmId,
                         aFlags,
                         aReturn,
                         aErrorStack ) == STL_SUCCESS );
#else
    STL_ASSERT( STL_FALSE );
#endif
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
