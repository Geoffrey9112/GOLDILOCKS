/*******************************************************************************
 * stlGeneral.h
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


#ifndef _STLGENERAL_H_
#define _STLGENERAL_H_ 1

/**
 * @file stlGeneral.h
 * @brief Standard Layer General Routines
 */


STL_BEGIN_DECLS


/**
 * @defgroup stlGeneral General
 * @ingroup STL 
 * @{
 */

stlStatus stlInitialize();

stlStatus stlTerminate();

void stlExit( stlInt32 aStatus );

stlStatus stlAtExit(void (*aFunction)(void) );

void stlSetProcessInitialized( stlLayerClass aLayerClass );
stlBool stlGetProcessInitialized( stlLayerClass aLayerClass );

/** @} */
    
STL_END_DECLS

#endif /* _STLGENERAL_H_ */
