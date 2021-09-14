/*******************************************************************************
 * cmpProtocol.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: cmpProtocol.h 6534 2012-12-04 00:37:52Z mkkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _CMPPROTOCOL_H_
#define _CMPPROTOCOL_H_ 1

/**
 * @file cmpProtocol.h
 * @brief Communication Layer Protocol Component Routines
 */

/**
 * @defgroup cmpProtocol Protocol
 * @ingroup cmExternal
 * @{
 */


stlStatus cmpReadErrorResult( cmlHandle       * aHandle,
                              stlBool         * aIgnored,
                              cmpCursor       * aCursor,
                              stlErrorStack   * aErrorStack );

stlStatus cmpWriteErrorResult( cmlHandle       * aHandle,
                               cmpCursor       * aCursor,
                               stlErrorStack   * aErrorStack );

stlStatus cmpReadXaErrorResult( cmlHandle       * aHandle,
                                stlBool         * aIgnored,
                                cmpCursor       * aCursor,
                                stlInt32        * aXaError,
                                stlErrorStack   * aErrorStack );

/** @} */

#endif /* _CMPPROTOCOL_H_ */
