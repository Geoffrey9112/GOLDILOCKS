/*******************************************************************************
 * stlEnv.h
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


#ifndef _STLENV_H_
#define _STLENV_H_ 1

/**
 * @file stlEnv.h
 * @brief Standard Layer Environment Routines
 */

#include <stlDef.h>

STL_BEGIN_DECLS


/**
 * @defgroup stlEnv Environment
 * @ingroup STL 
 * @{
 */

stlStatus stlGetEnv(stlChar       * aValue,
                    stlInt32        aValueLen,
                    const stlChar * aEnvVar,
                    stlBool       * aIsFound,
                    stlErrorStack * aErrorStack);

stlStatus stlSetEnv(const stlChar * aEnvVar,
                    stlChar       * aValue,
                    stlErrorStack * aErrorStack);

stlStatus stlDelEnv(const stlChar * aEnvVar,
                    stlErrorStack * aErrorStack);

/** @} */
    
STL_END_DECLS

#endif /* _STLSHM_H_ */
