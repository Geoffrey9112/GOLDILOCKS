/*******************************************************************************
 * zln.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: zln.h 7308 2013-02-19 05:52:47Z htkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#ifndef _ZLN_H_
#define _ZLN_H_ 1

/**
 * @file zln.h
 * @brief Gliese API Internal Environment Routines.
 */

/**
 * @defgroup zln Internal Environment Routines
 * @{
 */

stlStatus zlnAlloc( void          ** aEnv,
                    stlErrorStack  * aErrorStack );

stlStatus zlnFree( zlnEnv        * aEnv,
                   stlErrorStack * aErrorStack );

stlStatus zlnSetAttr( zlnEnv        * aEnv,
                      stlInt32        aAttr,
                      void          * aValue,
                      stlInt32        aStrLen,
                      stlErrorStack * aErrorStack );

stlStatus zlnGetAttr( zlnEnv        * aEnv,
                      stlInt32        aAttr,
                      void          * aValue,
                      stlInt32        aBufferLen,
                      stlInt32      * aStringLen,
                      stlErrorStack * aErrorStack );


/** @} */

#endif /* _ZLN_H_ */
