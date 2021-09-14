/*******************************************************************************
 * zlcc.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: zlcc.h 11187 2014-02-10 07:27:24Z htkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#ifndef _ZLCC_H_
#define _ZLCC_H_ 1

/**
 * @file zlcc.h
 * @brief Gliese API Internal Connect Routines.
 */

/**
 * @defgroup zlcc Internal Connect Routines
 * @{
 */

stlStatus zlccConnect( zlcDbc        * aDbc,
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

stlStatus zlccDisconnect( zlcDbc        * aDbc,
                          stlErrorStack * aErrorStack );

stlStatus zlccSetAttr( zlcDbc        * aDbc,
                       stlInt32        aAttr,
                       stlInt32        aIntValue,
                       stlChar       * aStrValue,
                       stlErrorStack * aErrorStack );

stlStatus zlccPreHandshake( zlcDbc         * aDbc,
                            cmlSessionType   aServerMode,
                            stlErrorStack  * aErrorStack );

stlStatus zlccHandshake( zlcDbc        * aDbc,
                         stlChar       * aRoleName,
                         stlInt32      * aBufferSize,
                         stlErrorStack * aErrorStack );
/** @} */

#endif /* _ZLCC_H_ */
