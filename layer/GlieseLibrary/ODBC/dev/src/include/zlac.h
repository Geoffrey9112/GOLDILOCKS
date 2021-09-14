/*******************************************************************************
 * zlac.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: zlac.h 11187 2014-02-10 07:27:24Z htkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#ifndef _ZLAC_H_
#define _ZLAC_H_ 1

/**
 * @file zlac.h
 * @brief Gliese API Internal Connect Routines.
 */

/**
 * @defgroup zlac Internal Connect Routines
 * @{
 */

stlStatus zlacConnect( zlcDbc        * aDbc,
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
                       stlErrorStack * aErrorStack );

stlStatus zlacDisconnect( zlcDbc        * aDbc,
                          stlErrorStack * aErrorStack );

stlStatus zlacSetAttr( zlcDbc        * aDbc,
                       stlInt32        aAttr,
                       stlInt32        aIntValue,
                       stlChar       * aStrValue,
                       stlErrorStack * aErrorStack );
/** @} */

#endif /* _ZLAC_H_ */
