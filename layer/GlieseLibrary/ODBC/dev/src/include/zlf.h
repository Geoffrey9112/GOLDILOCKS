/*******************************************************************************
 * zlf.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: zlf.h 7086 2013-01-29 05:16:28Z htkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#ifndef _ZLF_H_
#define _ZLF_H_ 1

/**
 * @file zlf.h
 * @brief ODBC API Internal Info Routines.
 */

/**
 * @defgroup zlf Internal Info Routines
 * @{
 */

stlStatus zlfGetFunctions( zlcDbc        * aDbc,
                           stlUInt16       aFuncId,
                           stlUInt16     * aSupported,
                           stlErrorStack * aErrorStack );

stlStatus zlfGetInfo( zlcDbc         * aDbc,
                      stlUInt16        aInfoType,
                      void           * aInfoValuePtr,
                      stlInt16         aBufferLength,
                      stlInt16       * aStringLengthPtr,
                      stlErrorStack  * aErrorStack );
/** @} */

#endif /* _ZLF_H_ */
