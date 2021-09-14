/*******************************************************************************
 * ztrc.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _ZTRC_H_
#define _ZTRC_H_ 1

/**
 * @file ztrc.h
 * @brief GlieseTool LogMirror Configure Routines
 */


/**
 * @defgroup ztrc LogMirror Configure File Routines
 * @{
 */

#ifndef YY_EXTRA_TYPE
#define YY_EXTRA_TYPE ztrConfigureParam *
#endif
                                  
stlInt32  ztrcConfigurelex_init( void ** );

stlInt32  ztrcConfigurelex_destroy( void * );

stlInt32  ztrcConfigureparse( );

void      ztrcConfigureset_extra( YY_EXTRA_TYPE, void * );

void      ztrcSetUserId( ztrConfigureParam * aParam, 
                         stlChar           * aUserId );

void      ztrcSetUserPw( ztrConfigureParam * aParam,
                         stlChar           * aUserPw );

void      ztrcSetLogPath( ztrConfigureParam * aParam,
                          stlChar           * aLogPath );

void      ztrcSetPort( ztrConfigureParam * aParam,
                       stlChar           * aPort );

void      ztrcSetMasterIp( ztrConfigureParam * aParam,
                           stlChar           * aMasterIp );

void      ztrcSetDsn( ztrConfigureParam * aParam,
                      stlChar           * aDsn );

void      ztrcSetHostIp( ztrConfigureParam * aParam,
                         stlChar           * aHostIp );
                    
void      ztrcSetHostPort( ztrConfigureParam * aParam,
                           stlChar           * aPort );

void      ztrcSetIbDevName( ztrConfigureParam * aParam,
                            stlChar           * aDevName );

void      ztrcSetIbPort( ztrConfigureParam * aParam,
                         stlChar           * aPort );

stlStatus ztrcReadConfigure( stlErrorStack * aErrorStack );


/** @} */

#endif /* _ZTRC_H_ */
