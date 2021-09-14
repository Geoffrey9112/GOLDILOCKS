/*******************************************************************************
 * ztym.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: ztym.h $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#ifndef _ZTYM_H_
#define _ZTYM_H_ 1

/**
 * @file ztym.h
 * @brief ztym layer functions 
 */

/**
 * @defgroup ztymMain
 * @{
 */

void ztymPrintHelp();

stlStatus ztymParseArgs( stlInt32    aArgc,
                         stlChar   * aArgv[],
                         stlLogger * aLogger,
                         smlEnv    * aEnv );

/** @} */

/**
 * @defgroup ztymGeneral
 * @{
 */

stlStatus ztymInitialize( smlEnv        * aEnv,
                          smlSessionEnv * aSessionEnv );

stlStatus ztymFinalize( smlEnv        * aEnv,
                        smlSessionEnv * aSessionEnv);

stlStatus ztymGetHomePath( stlChar       * aFilePath,
                           stlErrorStack * aErrorStack );

stlStatus ztymPrintStartLogMsg( stlLogger * aLogger,
                                smlEnv    * aEnv );

void ztymPrintErrorStack( stlErrorStack * aErrorStack );

void ztymWriteLastError( stlErrorStack * aErrorStack );

stlStatus ztymAttachStaticArea( stlShm * aShm,
                                smlEnv * aEnv );

stlStatus ztymDetachStaticArea( stlShm * aShm,
                                smlEnv * aEnv );

stlStatus ztymSetWarmupEntry( stlShm * aShm,
                              smlEnv * aEnv );

stlStatus ztymClearProcess( stlShm * aShm,
                            smlEnv * aEnv );

stlStatus ztymFlushAllLogs( smlEnv * aEnv );

/** @} */

#endif /* _ZTYM_H_ */
