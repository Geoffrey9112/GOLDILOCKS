/*******************************************************************************
 * zlao.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: zlao.h 7308 2013-02-19 05:52:47Z htkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#ifndef _ZLAO_H_
#define _ZLAO_H_ 1

/**
 * @file zlao.h
 * @brief ODBC API Internal XA Routines.
 */

/**
 * @defgroup zlao Internal XA Routines
 * @{
 */

stlStatus zlaoClose( zlcDbc        * aDbc,
                     stlInt32      * aReturn,
                     stlErrorStack * aErrorStack );

stlStatus zlaoStart( zlcDbc        * aDbc,
                     stlXid        * aXid,
                     stlInt32        aRmId,
                     stlInt32        aFlags,
                     stlInt32      * aReturn,
                     stlErrorStack * aErrorStack );

stlStatus zlaoEnd( zlcDbc        * aDbc,
                   stlXid        * aXid,
                   stlInt32        aRmId,
                   stlInt32        aFlags,
                   stlInt32      * aReturn,
                   stlErrorStack * aErrorStack );

stlStatus zlaoRollback( zlcDbc        * aDbc,
                        stlXid        * aXid,
                        stlInt32        aRmId,
                        stlInt32        aFlags,
                        stlInt32      * aReturn,
                        stlErrorStack * aErrorStack );

stlStatus zlaoPrepare( zlcDbc        * aDbc,
                       stlXid        * aXid,
                       stlInt32        aRmId,
                       stlInt32        aFlags,
                       stlInt32      * aReturn,
                       stlErrorStack * aErrorStack );

stlStatus zlaoCommit( zlcDbc        * aDbc,
                      stlXid        * aXid,
                      stlInt32        aRmId,
                      stlInt32        aFlags,
                      stlInt32      * aReturn,
                      stlErrorStack * aErrorStack );

stlStatus zlaoRecover( zlcDbc        * aDbc,
                       stlXid        * aXids,
                       stlInt32        aCount,
                       stlInt32        aRmId,
                       stlInt32        aFlags,
                       stlInt32      * aReturn,
                       stlErrorStack * aErrorStack );

stlStatus zlaoForget( zlcDbc        * aDbc,
                      stlXid        * aXid,
                      stlInt32        aRmId,
                      stlInt32        aFlags,
                      stlInt32      * aReturn,
                      stlErrorStack * aErrorStack );

/** @} */

#endif /* _ZLAO_H_ */
