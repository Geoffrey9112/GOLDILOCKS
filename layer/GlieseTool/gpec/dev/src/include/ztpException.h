/*******************************************************************************
 * ztpException.h
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


#ifndef _ZTPEXCEPTION_H_
#define _ZTPEXCEPTION_H_ 1

#include <dtl.h>

/**
 * @file ztpException.h
 * @brief Gliese Embedded SQL in C precompiler exception handling functions
 */


/**
 * @defgroup ztpException Gliese Embedded SQL in C precompiler exception handling functions
 * @ingroup ztp
 * @{
 */

stlStatus ztpExceptCondSqlError(ztpCParseParam  *aParam,
                                stlInt32         aCondType,
                                stlChar         *aExceptionStr);
stlStatus ztpExceptCondSqlWarning(ztpCParseParam  *aParam,
                                  stlInt32         aCondType,
                                  stlChar         *aExceptionStr);
stlStatus ztpExceptCondNotFound(ztpCParseParam  *aParam,
                                stlInt32         aCondType,
                                stlChar         *aExceptionStr);
stlStatus ztpExceptCondSqlState(ztpCParseParam  *aParam,
                                stlInt32         aCondType,
                                stlChar         *aExceptionStr);
stlStatus ztpExceptActGoto(ztpCParseParam  *aParam,
                           stlInt32         aCondType,
                           stlChar         *aExceptionStr);
stlStatus ztpExceptActContinue(ztpCParseParam  *aParam,
                               stlInt32         aCondType,
                               stlChar         *aExceptionStr);
stlStatus ztpExceptActStop(ztpCParseParam  *aParam,
                           stlInt32         aCondType,
                           stlChar         *aExceptionStr);
stlStatus ztpExceptActDo(ztpCParseParam  *aParam,
                         stlInt32         aCondType,
                         stlChar         *aExceptionStr);

/** @} */

#endif /* _ZTPEXCEPTION_H_ */
