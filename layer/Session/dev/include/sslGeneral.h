/*******************************************************************************
 * sslGeneral.h
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


#ifndef _SSLGENERAL_H_
#define _SSLGENERAL_H_ 1

/**
 * @file sslGeneral.h
 * @brief Session General Routines
 */

stlStatus sslInitialize();
stlStatus sslTerminate();

extern knlStartupFunc     gStartupSS[KNL_STARTUP_PHASE_MAX];
extern knlWarmupFunc      gWarmupSS;
extern knlShutdownFunc    gShutdownSS[KNL_SHUTDOWN_PHASE_MAX];
extern knlCooldownFunc    gCooldownSS;
extern knlCleanupHeapFunc gCleanupHeapSS;

#endif /* _SSLGENERAL_H_ */
