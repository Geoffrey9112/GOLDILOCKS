/*******************************************************************************
 * ztpdConvert.h
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


#ifndef _ZTPDCONVERT_H_
#define _ZTPDCONVERT_H_ 1

#include <dtl.h>
#include <zlplDef.h>

/**
 * @file ztpdConvert.h
 * @brief Gliese Embedded SQL in C precompiler convert data type functions
 */


/**
 * @defgroup ztpd Gliese Embedded SQL in C precompiler data type functions
 * @ingroup ztp
 * @{
 */

stlStatus ztpdConvertNativeType(ztpCParseParam  *aParam,
                                ztpCDeclaration *aCDeclaration,
                                ztpCVariable    *aCVariableListHead);
stlStatus ztpdConvertVarchar(ztpCParseParam  *aParam,
                             ztpCDeclaration *aCDeclaration,
                             ztpCVariable    *aCVariableListHead);
stlStatus ztpdConvertLongVarchar(ztpCParseParam  *aParam,
                                 ztpCDeclaration *aCDeclaration,
                                 ztpCVariable    *aCVariableListHead);
stlStatus ztpdConvertBinary(ztpCParseParam  *aParam,
                            ztpCDeclaration *aCDeclaration,
                            ztpCVariable    *aCVariableListHead);
stlStatus ztpdConvertVarBinary(ztpCParseParam  *aParam,
                               ztpCDeclaration *aCDeclaration,
                               ztpCVariable    *aCVariableListHead);
stlStatus ztpdConvertLongVarBinary(ztpCParseParam  *aParam,
                                   ztpCDeclaration *aCDeclaration,
                                   ztpCVariable    *aCVariableListHead);
stlStatus ztpdConvertBoolean(ztpCParseParam  *aParam,
                             ztpCDeclaration *aCDeclaration,
                             ztpCVariable    *aCVariableListHead);
stlStatus ztpdConvertNumeric(ztpCParseParam  *aParam,
                             ztpCDeclaration *aCDeclaration,
                             ztpCVariable    *aCVariableListHead);
stlStatus ztpdConvertDate(ztpCParseParam  *aParam,
                          ztpCDeclaration *aCDeclaration,
                          ztpCVariable    *aCVariableListHead);
stlStatus ztpdConvertTime(ztpCParseParam  *aParam,
                          ztpCDeclaration *aCDeclaration,
                          ztpCVariable    *aCVariableListHead);
stlStatus ztpdConvertTimestamp(ztpCParseParam  *aParam,
                               ztpCDeclaration *aCDeclaration,
                               ztpCVariable    *aCVariableListHead);
stlStatus ztpdConvertTimeTz(ztpCParseParam  *aParam,
                            ztpCDeclaration *aCDeclaration,
                            ztpCVariable    *aCVariableListHead);
stlStatus ztpdConvertTimestampTz(ztpCParseParam  *aParam,
                                 ztpCDeclaration *aCDeclaration,
                                 ztpCVariable    *aCVariableListHead);
stlStatus ztpdConvertInterval(ztpCParseParam  *aParam,
                              ztpCDeclaration *aCDeclaration,
                              ztpCVariable    *aCVariableListHead);
stlStatus ztpdConvertStruct(ztpCParseParam  *aParam,
                            ztpCDeclaration *aCDeclaration,
                            ztpCVariable    *aCVariableListHead);


/** @} */

#endif /* _ZTPDCONVERT_H_ */
