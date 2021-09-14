/*******************************************************************************
 * qlgStartup.h
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


#ifndef _QLG_STARTUP_H_
#define _QLG_STARTUP_H_ 1

/**
 * @file qlgStartup.h
 * @brief SQL Processor Layer Startup Internal Routines
 */

/**
 * @brief NOMOUNT, MOUNT 단계의 Performance View 정의
 */
typedef struct qlgNonServicePerfView
{
    stlChar * mViewName;             /**< View Name */
    stlChar * mViewDefinition;       /**< View Definition */
} qlgNonServicePerfView;

extern qlgNonServicePerfView gQlgNoMountPerfViewDefinition[];
extern qlgNonServicePerfView gQlgMountPerfViewDefinition[];


stlStatus qlgStartupNoMount( knlStartupInfo * aStartupInfo,
                             void           * aEnv );

stlStatus qlgStartupMount( knlStartupInfo * aStartupInfo,
                           void           * aEnv );
stlStatus qlgStartupPreOpen( knlStartupInfo * aStartupInfo,
                             void           * aEnv );
stlStatus qlgStartupOpen( knlStartupInfo * aStartupInfo,
                          void           * aEnv );
stlStatus qlgWarmup( void * aEnv );
stlStatus qlgCooldown( void * aEnv );
stlStatus qlgShutdownInit( knlShutdownInfo * aShutdownInfo,
                           void            * aEnv );
stlStatus qlgShutdownDismount( knlShutdownInfo * aShutdownInfo,
                               void            * aEnv );
stlStatus qlgShutdownPostClose( knlShutdownInfo * aShutdownInfo,
                                void            * aEnv );
stlStatus qlgShutdownClose( knlShutdownInfo * aShutdownInfo,
                            void            * aEnv );

void qlgFatalHandler( const stlChar * aCauseString,
                      void          * aSigInfo,
                      void          * aContext );


dtlCharacterSet qlgGetCharSetIDFunc( void * aArgs );

stlInt32 qlgGetGMTOffsetFunc( void * aArgs );
stlInt32 qlgGetOriginalGMTOffsetFunc( void * aArgs );

stlStatus qlgReallocLongVaryingMemFunc( void           * aArgs,
                                        stlInt32         aAllocSize,
                                        void          ** aAddr,
                                        stlErrorStack  * aErrorStack );

stlStatus qlgReallocAndMoveLongVaryingMemFunc( void           * aArgs,
                                               dtlDataValue   * aDataValue,
                                               stlInt32         aAllocSize,
                                               stlErrorStack  * aErrorStack );

stlChar * qlgGetDateFormatStringFunc( void * aArgs );
dtlDateTimeFormatInfo * qlgGetDateFormatInfoFunc( void * aArgs );

stlChar * qlgGetTimeFormatStringFunc( void * aArgs );
dtlDateTimeFormatInfo * qlgGetTimeFormatInfoFunc( void * aArgs );

stlChar * qlgGetTimeWithTimeZoneFormatStringFunc( void * aArgs );
dtlDateTimeFormatInfo * qlgGetTimeWithTimeZoneFormatInfoFunc( void * aArgs );

stlChar * qlgGetTimestampFormatStringFunc( void * aArgs );
dtlDateTimeFormatInfo * qlgGetTimestampFormatInfoFunc( void * aArgs );

stlChar * qlgGetTimestampWithTimeZoneFormatStringFunc( void * aArgs );
dtlDateTimeFormatInfo * qlgGetTimestampWithTimeZoneFormatInfoFunc( void * aArgs );


stlStatus qlgBuildNonServicePerfView( knlStartupPhase   aStartPhase,
                                      qllEnv          * aEnv );

#endif /* _QLG_STARTUP_H_ */
