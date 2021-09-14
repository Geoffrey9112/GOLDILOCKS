/*******************************************************************************
 * ztpdType.h
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


#ifndef _ZTPDTYPE_H_
#define _ZTPDTYPE_H_ 1

#include <dtl.h>
#include <zlplDef.h>

/**
 * @file ztpdType.h
 * @brief Gliese Embedded SQL in C precompiler convert data type functions
 */


/**
 * @defgroup ztpd Gliese Embedded SQL in C precompiler data type functions
 * @ingroup ztp
 * @{
 */

stlInt32       ztpdGetPrecision( zlplCDataType aDataType );
stlInt32       ztpdGetScale( zlplCDataType  aDataType );
stlStatus      ztpdGetCDataType( ztpCParseParam        *aParam,
                                 ztpTypeSpecifierList  *aTypeSpecifierList,
                                 zlplCDataType         *aCDataType,
                                 stlChar              **aPrecision,
                                 stlChar              **aScale,
                                 ztpCVariable         **aCVariable );

stlBool   ztpdIsHostStringType( ztpCVariable   * aCVariable );
stlBool   ztpdIsHostIntegerType( ztpCVariable   * aCVariable );


/** @} */

#endif /* _ZTPDTYPE_H_ */
