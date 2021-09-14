/*******************************************************************************
 * dtlGeneral.h
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


#ifndef _DTL_GENERAL_H_
#define _DTL_GENERAL_H_ 1

/**
 * @file dtlGeneral.h
 * @brief DataType Layer General Routines
 */

stlStatus dtlInitialize();

stlStatus dtlTerminate();

stlStatus dtlInitializeForInternal( stlErrorStack * aErrorStack );

#endif /* _DTL_GENERAL_H_ */
