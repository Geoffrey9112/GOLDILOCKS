/*******************************************************************************
 * zlymParams.h
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

#ifndef _ZLYMPARAMS_H_
#define _ZLYMPARAMS_H_ 1

/**
 * @file zlymParams.h
 * @brief Python parameters for Gliese Python Database API
 */

stlBool Params_init();

stlBool PrepareAndBind(zlyCursor* cur, PyObject* pSql, PyObject* params, stlBool skip_first);
void FreeParameterData(zlyCursor* cur);
void FreeParameterInfo(zlyCursor* cur);

#endif /* _ZLYMPARAMS_H_ */
