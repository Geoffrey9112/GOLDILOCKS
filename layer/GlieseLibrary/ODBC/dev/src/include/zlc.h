/*******************************************************************************
 * zlc.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: zlc.h 11187 2014-02-10 07:27:24Z htkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#ifndef _ZLC_H_
#define _ZLC_H_ 1

/**
 * @file zlc.h
 * @brief Gliese API Internal Connect Routines.
 */

/**
 * @defgroup zlc Internal Connect Routines
 * @{
 */

stlStatus zlcSetNlsDTFuncVector( zlcDbc * aDbc );

stlStatus zlcCheckSession( zlcDbc        * aDbc,
                           stlErrorStack * aErrorStack );

void zlcCheckSessionAndCleanup( zlcDbc * aDbc );

stlStatus zlcAlloc( zlnEnv         * aEnv,
                    void          ** aDbc,
                    stlErrorStack  * aErrorStack );

stlStatus zlcFree( zlcDbc        * aDbc,
                   stlErrorStack * aErrorStack );

stlStatus zlcGetInfoFromDSN( zlcDbc          * aDbc,
                             stlChar         * aDSN,
                             stlChar         * aHost,
                             stlInt32        * aPort,
                             stlChar         * aUserName,
                             stlChar         * aPassword,
                             stlChar         * aCsMode,
                             zlcProtocolType * aProtocolType,
                             stlErrorStack   * aErrorStack );

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
                      stlErrorStack   * aErrorStack );

stlStatus zlcGetConnName( stlChar         *aInConnectionString,
                          stlInt16         aInStringLen,
                          stlChar         *aConnName,
                          stlInt16        *aConnNameLen,
                          stlErrorStack   *aErrorStack );

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
                            stlErrorStack * aErrorStack );

stlStatus zlcDisconnect( zlcDbc        * aDbc,
                         stlErrorStack * aErrorStack );


stlStatus zlcSetAttr( zlcDbc        * aDbc,
                      stlInt32        aAttr,
                      void          * aValue,
                      stlInt32        aStrLen,
                      stlErrorStack * aErrorStack );

stlStatus zlcGetAttr( zlcDbc        * aDbc,
                      stlInt32        aAttr,
                      void          * aValue,
                      stlInt32        aBufferLen,
                      stlInt32      * aStringLen,
                      stlErrorStack * aErrorStack );

/** @} */

#endif /* _ZLC_H_ */
