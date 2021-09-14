/*******************************************************************************
 * ztnConfigure.h
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


#ifndef _ZTNCONFIGURE_H_
#define _ZTNCONFIGURE_H_ 1

/**
 * @file ztnConfigure.h
 * @brief GlieseTool Cyclone Monitor Configure Routines
 */


/**
 * @defgroup ztnConfigure Configure File Routines
 * @{
 */


#ifndef YY_EXTRA_TYPE
#define YY_EXTRA_TYPE ztnConfigureParam *
#endif
                                  
stlInt32  ztnfConfigurelex_init( void ** );

stlInt32  ztnfConfigurelex_destroy( void * );

stlInt32  ztnfConfigureparse( );

void      ztnfConfigureset_extra( YY_EXTRA_TYPE, void * );

stlStatus ztnfReadConfigure( stlErrorStack * aErrorStack );

void      ztnfAddGroupItem( ztnConfigureParam * aParam,
                            stlChar           * aGroupName );

void      ztnfSetPortToGroupArea( ztnConfigureParam * aParam, 
                                  stlChar           * aPort );

void      ztnfSetDsnToGlobalArea( ztnConfigureParam * aParam, 
                                  stlChar           * aDsn );

void      ztnfSetDsnToGroupArea( ztnConfigureParam * aParam, 
                                 stlChar           * aDsn );

void      ztnfSetHostIpToGlobalArea( ztnConfigureParam * aParam, 
                                     stlChar           * aHostIp );

void      ztnfSetHostIpToGroupArea( ztnConfigureParam * aParam, 
                                    stlChar           * aHostIp );

void      ztnfSetHostPortToGlobalArea( ztnConfigureParam * aParam, 
                                       stlChar           * aPort );

void      ztnfSetHostPortToGroupArea( ztnConfigureParam * aParam, 
                                      stlChar           * aPort );

void      ztnfSetLibConfigToGlobalArea( ztnConfigureParam * aParam,
                                        stlChar           * aLibConfig );

void      ztnfSetMonitorFileToGlobalArea( ztnConfigureParam * aParam,
                                          stlChar           * aMonitorFile );

void      ztnfSetMonitorCycleMsToGlobalArea( ztnConfigureParam * aParam,
                                             stlChar           * aMonitorCycleMs );

/** @} */


/** @} */

#endif /* _ZTNCONFIGURE_H_ */


