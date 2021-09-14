/*******************************************************************************
 * ztpdRefineHostVar.h
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


#ifndef _ZTPDREFINEHOSTVAR_H_
#define _ZTPDREFINEHOSTVAR_H_ 1

#include <dtl.h>
#include <zlplDef.h>

/**
 * @file ztpdRefineHostVar.h
 * @brief Gliese Embedded SQL in C precompiler refine data type functions
 */


/**
 * @addtogroup ztpd
 * @{
 */

stlStatus ztpdRefineNativeTypeHostVar(ztpCVariable    *aCVariable);
stlStatus ztpdRefineCharHostVar(ztpCVariable    *aCVariable);
stlStatus ztpdRefineVarcharHostVar(ztpCVariable    *aCVariable);
stlStatus ztpdRefineLongVarcharHostVar(ztpCVariable    *aCVariable);
stlStatus ztpdRefineBinaryHostVar(ztpCVariable    *aCVariable);
stlStatus ztpdRefineVarBinaryHostVar(ztpCVariable    *aCVariable);
stlStatus ztpdRefineLongVarBinaryHostVar(ztpCVariable    *aCVariable);
stlStatus ztpdRefineStructHostVar(ztpCVariable    *aCVariable);

/** @} */

#endif /* _ZTPDREFINEHOSTVAR_H_ */
