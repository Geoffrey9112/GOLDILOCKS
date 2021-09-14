/*******************************************************************************
 * qllGeneral.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id$
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _QLLGENERAL_H_
#define _QLLGENERAL_H_ 1

/**
 * @file qllGeneral.h
 * @brief Execution Library General Routines
 */

stlStatus qllInitialize();
stlStatus qllInitializeEnv( qllEnv     * aEnv,
                            knlEnvType   aEnvType );
stlStatus qllFinalizeEnv( qllEnv * aEnv );

stlStatus qllBuildPerfViewNoMount( qllEnv * aEnv );
stlStatus qllBuildPerfViewMount( qllEnv * aEnv );

stlStatus qllInitSessionEnv( qllSessionEnv     * aSessEnv,
                             knlConnectionType   aConnectionType,
                             knlSessionType      aSessType,
                             knlSessEnvType      aSessEnvType,
                             qllEnv            * aEnv );

stlStatus qllFiniSessionEnv( qllSessionEnv * aSessEnv,
                             qllEnv        * aEnv );

stlStatus qllCleanupSessionEnv( qllSessionEnv * aSessEnv,
                                qllEnv        * aEnv );

stlStatus qllSetDTFuncVector( void * aEnv );

stlStatus qllInitSessNLS( qllEnv * aEnv );

stlStatus qllInitSessNLSFromProperty( qllEnv * aEnv );

stlStatus qllFiniSessNLSFromProperty( qllEnv * aEnv );

stlStatus qllSetLoginTimeZoneOffset( stlInt32   aTimeZoneOffset,
                                     qllEnv   * aEnv );

stlStatus qllSetLoginCharacterSet( dtlCharacterSet   aCharacterSet,
                                    qllEnv          * aEnv );

stlStatus qllSetSessTimeZoneOffset( stlInt32   aTimeZoneOffset,
                                    qllEnv   * aEnv );

stlStatus qllSetSessNlsFormat( dtlDataType                aDataType,
                               stlChar                  * aFormat,
                               dtlDateTimeFormatInfo    * aFormatInfoBuf,
                               stlChar                  * aOrgFormatString,
                               dtlDateTimeFormatInfo    * aOrgFormatInfo,
                               stlChar                 ** aNewFormatString,
                               dtlDateTimeFormatInfo   ** aNewFormatInfo,
                               qllEnv                   * aEnv );

void qllAddSessStat2SystemStat( qllSessionEnv * aSessEnv );


extern knlStartupFunc     gStartupQP[KNL_STARTUP_PHASE_MAX];
extern knlWarmupFunc      gWarmupQP;
extern knlShutdownFunc    gShutdownQP[KNL_SHUTDOWN_PHASE_MAX];
extern knlCooldownFunc    gCooldownQP;
extern knlCleanupHeapFunc gCleanupHeapQP;

#endif /* _QLLGENERAL_H_ */
