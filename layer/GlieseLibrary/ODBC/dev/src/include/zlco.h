/*******************************************************************************
 * zlco.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: zlco.h 7308 2013-02-19 05:52:47Z htkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#ifndef _ZLCO_H_
#define _ZLCO_H_ 1

/**
 * @file zlco.h
 * @brief ODBC API Internal XA Routines.
 */

/**
 * @defgroup zlco Internal XA Routines
 * @{
 */

stlStatus zlcoClose( zlcDbc        * aDbc,
                     stlInt32      * aReturn,
                     stlErrorStack * aErrorStack );

stlStatus zlcoStart( zlcDbc        * aDbc,
                     stlXid        * aXid,
                     stlInt32        aRmId,
                     stlInt32        aFlags,
                     stlInt32      * aReturn,
                     stlErrorStack * aErrorStack );

stlStatus zlcoEnd( zlcDbc        * aDbc,
                   stlXid        * aXid,
                   stlInt32        aRmId,
                   stlInt32        aFlags,
                   stlInt32      * aReturn,
                   stlErrorStack * aErrorStack );

stlStatus zlcoRollback( zlcDbc        * aDbc,
                        stlXid        * aXid,
                        stlInt32        aRmId,
                        stlInt32        aFlags,
                        stlInt32      * aReturn,
                        stlErrorStack * aErrorStack );

stlStatus zlcoPrepare( zlcDbc        * aDbc,
                       stlXid        * aXid,
                       stlInt32        aRmId,
                       stlInt32        aFlags,
                       stlInt32      * aReturn,
                       stlErrorStack * aErrorStack );

stlStatus zlcoCommit( zlcDbc        * aDbc,
                      stlXid        * aXid,
                      stlInt32        aRmId,
                      stlInt32        aFlags,
                      stlInt32      * aReturn,
                      stlErrorStack * aErrorStack );

stlStatus zlcoRecover( zlcDbc        * aDbc,
                       stlXid        * aXids,
                       stlInt32        aCount,
                       stlInt32        aRmId,
                       stlInt32        aFlags,
                       stlInt32      * aReturn,
                       stlErrorStack * aErrorStack );

stlStatus zlcoForget( zlcDbc        * aDbc,
                      stlXid        * aXid,
                      stlInt32        aRmId,
                      stlInt32        aFlags,
                      stlInt32      * aReturn,
                      stlErrorStack * aErrorStack );

/** @} */

#endif /* _ZLCO_H_ */
