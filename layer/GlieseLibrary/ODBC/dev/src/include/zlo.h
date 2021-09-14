/*******************************************************************************
 * zlo.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: zlo.h 7308 2013-02-19 05:52:47Z htkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#ifndef _ZLO_H_
#define _ZLO_H_ 1

/**
 * @file zlo.h
 * @brief ODBC API Internal XA Routines.
 */

/**
 * @defgroup zlo Internal XA Routines
 * @{
 */

stlStatus zloClose( zlcDbc        * aDbc,
                    stlInt32      * aReturn,
                    stlErrorStack * aErrorStack );

stlStatus zloStart( zlcDbc        * aDbc,
                    stlXid        * aXid,
                    stlInt32        aRmId,
                    stlInt32        aFlags,
                    stlInt32      * aReturn,
                    stlErrorStack * aErrorStack );

stlStatus zloEnd( zlcDbc        * aDbc,
                  stlXid        * aXid,
                  stlInt32        aRmId,
                  stlInt32        aFlags,
                  stlInt32      * aReturn,
                  stlErrorStack * aErrorStack );

stlStatus zloRollback( zlcDbc        * aDbc,
                       stlXid        * aXid,
                       stlInt32        aRmId,
                       stlInt32        aFlags,
                       stlInt32      * aReturn,
                       stlErrorStack * aErrorStack );

stlStatus zloPrepare( zlcDbc        * aDbc,
                      stlXid        * aXid,
                      stlInt32        aRmId,
                      stlInt32        aFlags,
                      stlInt32      * aReturn,
                      stlErrorStack * aErrorStack );

stlStatus zloCommit( zlcDbc        * aDbc,
                     stlXid        * aXid,
                     stlInt32        aRmId,
                     stlInt32        aFlags,
                     stlInt32      * aReturn,
                     stlErrorStack * aErrorStack );

stlStatus zloRecover( zlcDbc        * aDbc,
                      stlXid        * aXids,
                      stlInt32        aCount,
                      stlInt32        aRmId,
                      stlInt32        aFlags,
                      stlInt32      * aReturn,
                      stlErrorStack * aErrorStack );

stlStatus zloForget( zlcDbc        * aDbc,
                     stlXid        * aXid,
                     stlInt32        aRmId,
                     stlInt32        aFlags,
                     stlInt32      * aReturn,
                     stlErrorStack * aErrorStack );

/** @} */

#endif /* _ZLO_H_ */
