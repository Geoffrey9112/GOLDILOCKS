/*******************************************************************************
 * scpProtocol.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: scpProtocol.h 6534 2012-12-04 00:37:52Z mkkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _SCPPROTOCOL_H_
#define _SCPPROTOCOL_H_ 1

/**
 * @file scpProtocol.h
 * @brief Server Communication Layer Protocol Component Routines
 */

/**
 * @defgroup scpProtocol Protocol
 * @ingroup cmExternal
 * @{
 */


typedef stlStatus (*scpReadConnectCommandFunc)( sclHandle       * aHandle,
                                                stlInt8         * aControl,
                                                stlChar         * aUserName,
                                                stlChar         * aEncryptedPassword,
                                                stlChar         * aOldPassword,
                                                stlChar         * aNewPassword,
                                                stlChar         * aProgramName,
                                                stlInt32        * aProcessId,
                                                stlInt16        * aCharacterset,
                                                stlInt32        * aTimezone,
                                                cmlDbcType      * aDbcType,
                                                sclEnv          * aEnv );


stlStatus scpReadConnectCommandV1( sclHandle       * aHandle,
                                   stlInt8         * aControl,
                                   stlChar         * aUserName,
                                   stlChar         * aEncryptedPassword,
                                   stlChar         * aOldPassword,
                                   stlChar         * aNewPassword,
                                   stlChar         * aProgramName,
                                   stlInt32        * aProcessId,
                                   stlInt16        * aCharacterset,
                                   stlInt32        * aTimezone,
                                   cmlDbcType      * aDbcType,
                                   sclEnv          * aEnv );

stlStatus scpReadConnectCommandV2( sclHandle       * aHandle,
                                   stlInt8         * aControl,
                                   stlChar         * aUserName,
                                   stlChar         * aEncryptedPassword,
                                   stlChar         * aOldPassword,
                                   stlChar         * aNewPassword,
                                   stlChar         * aProgramName,
                                   stlInt32        * aProcessId,
                                   stlInt16        * aCharacterset,
                                   stlInt32        * aTimezone,
                                   cmlDbcType      * aDbcType,
                                   sclEnv          * aEnv );

stlStatus scpBeginReading( sclHandle       * aHandle,
                           scpCursor       * aCursor,
                           sclEnv          * aEnv );
stlStatus scpEndReading( sclHandle       * aHandle,
                         scpCursor       * aCursor,
                         sclEnv          * aEnv );
stlStatus scpBeginWriting( sclHandle       * aHandle,
                           scpCursor       * aCursor,
                           sclEnv          * aEnv );
stlStatus scpEndWriting( sclHandle       * aHandle,
                         scpCursor       * aCursor,
                         sclEnv          * aEnv );

stlStatus scpSkipNBytes( sclHandle       * aHandle,
                         scpCursor       * aCursor,
                         stlInt32          aLength,
                         sclEnv          * aEnv );

stlStatus scpReadNBytes( sclHandle       * aHandle,
                         scpCursor       * aCursor,
                         stlChar         * aValue,
                         stlInt32          aLength,
                         sclEnv          * aEnv );

stlStatus scpWriteNBytes( sclHandle       * aHandle,
                          scpCursor       * aCursor,
                          stlChar         * aValue,
                          stlInt32          aLength,
                          sclEnv          * aEnv );


stlStatus scpWriteErrorResult( sclHandle       * aHandle,
                               scpCursor       * aCursor,
                               stlErrorStack   * aErrorStack,
                               sclEnv          * aEnv );


stlStatus scpWriteXaErrorResult( sclHandle       * aHandle,
                                 scpCursor       * aCursor,
                                 stlInt32          aXaError,
                                 stlErrorStack   * aErrorStack,
                                 sclEnv          * aEnv );

stlStatus scpAllocSendPoolingBuf( sclHandle     * aHandle,
                                  sclEnv        * aEnv );

stlStatus scpRestoreSendPoolingBuf( sclHandle     * aHandle,
                                    sclEnv        * aEnv );

stlStatus scpAllocOperationMem( sclHandle          * aHandle,
                                stlInt64             aSize,
                                void              ** aMemory,
                                sclEnv             * aEnv );

/** @} */

#endif /* _SCPPROTOCOL_H_ */

