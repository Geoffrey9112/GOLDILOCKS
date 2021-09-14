/*******************************************************************************
 * cmpCursor.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: cmpCursor.h 6534 2012-12-04 00:37:52Z mkkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _CMPCURSOR_H_
#define _CMPCURSOR_H_ 1

/**
 * @file cmpCursor.h
 * @brief Communication Layer Cursor Component Routines
 */

/**
 * @defgroup cmpCursor Protocol
 * @ingroup cmExternal
 * @{
 */

stlStatus cmpBeginReading( cmlHandle       * aHandle,
                           cmpCursor       * aCursor,
                           stlErrorStack   * aErrorStack );
stlStatus cmpEndReading( cmlHandle       * aHandle,
                         cmpCursor       * aCursor,
                         stlErrorStack   * aErrorStack );
stlStatus cmpBeginWriting( cmlHandle       * aHandle,
                           cmpCursor       * aCursor,
                           stlErrorStack   * aErrorStack );
stlStatus cmpEndWriting( cmlHandle       * aHandle,
                         cmpCursor       * aCursor,
                         stlErrorStack   * aErrorStack );

stlStatus cmpReadNBytes( cmlHandle       * aHandle,
                         cmpCursor       * aCursor,
                         stlChar         * aValue,
                         stlInt32          aLength,
                         stlErrorStack   * aErrorStack );

stlStatus cmpWriteNBytes( cmlHandle       * aHandle,
                          cmpCursor       * aCursor,
                          stlChar         * aValue,
                          stlInt32          aLength,
                          stlErrorStack   * aErrorStack );

/** @} */

#endif /* _CMPCURSOR_H_ */
