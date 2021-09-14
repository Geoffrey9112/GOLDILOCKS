/*******************************************************************************
 * stiEnv.h
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


#ifndef _STIENV_H_
#define _STIENV_H_ 1

#include <stlDef.h>

STL_BEGIN_DECLS

stlStatus stiGetEnv(stlChar       * aValue,
                    stlInt32        aValueLen,
                    const stlChar * aEnvVar,
                    stlBool       * aIsFound,
                    stlErrorStack * aErrorStack);

stlStatus stiSetEnv(const stlChar * aEnvVar,
                    stlChar       * aValue,
                    stlErrorStack * aErrorStack);

stlStatus stiDelEnv(const stlChar * aEnvVar,
                    stlErrorStack * aErrorStack);

STL_END_DECLS

#endif /* _STIENV_H_ */
