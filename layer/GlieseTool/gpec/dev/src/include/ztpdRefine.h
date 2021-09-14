/*******************************************************************************
 * ztpdRefine.h
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


#ifndef _ZTPDREFINE_H_
#define _ZTPDREFINE_H_ 1

#include <dtl.h>
#include <zlplDef.h>

/**
 * @file ztpdRefine.h
 * @brief Gliese Embedded SQL in C precompiler refine data type functions
 */


/**
 * @addtogroup ztpd
 * @{
 */

stlStatus ztpdRefineNativeType( ztpCVariable     *aCVariable,
                                ztpCDeclaration  *aCDeclaration );
stlStatus ztpdRefineChar( ztpCVariable     *aCVariable,
                          ztpCDeclaration  *aCDeclaration );
stlStatus ztpdRefineVarchar( ztpCVariable     *aCVariable,
                             ztpCDeclaration  *aCDeclaration );
stlStatus ztpdRefineLongVarchar( ztpCVariable     *aCVariable,
                                 ztpCDeclaration  *aCDeclaration );
stlStatus ztpdRefineBinary( ztpCVariable     *aCVariable,
                            ztpCDeclaration  *aCDeclaration );
stlStatus ztpdRefineVarBinary( ztpCVariable     *aCVariable,
                               ztpCDeclaration  *aCDeclaration );
stlStatus ztpdRefineLongVarBinary( ztpCVariable     *aCVariable,
                                   ztpCDeclaration  *aCDeclaration );
stlStatus ztpdRefineNumber( ztpCVariable     *aCVariable,
                            ztpCDeclaration  *aCDeclaration );
stlStatus ztpdRefineStruct( ztpCVariable     *aCVariable,
                            ztpCDeclaration  *aCDeclaration );

stlStatus ztpdRefineNativeTypeHostVar(ztpCVariable    *aCVariable);
stlStatus ztpdRefineCharHostVar(ztpCVariable    *aCVariable);
stlStatus ztpdRefineVarcharHostVar(ztpCVariable    *aCVariable);
stlStatus ztpdRefineLongVarcharHostVar(ztpCVariable    *aCVariable);
stlStatus ztpdRefineBinaryHostVar(ztpCVariable    *aCVariable);
stlStatus ztpdRefineVarBinaryHostVar(ztpCVariable    *aCVariable);
stlStatus ztpdRefineLongVarBinaryHostVar(ztpCVariable    *aCVariable);
stlStatus ztpdRefineStructHostVar(ztpCVariable    *aCVariable);

/** @} */

#endif /* _ZTPDREFINE_H_ */
