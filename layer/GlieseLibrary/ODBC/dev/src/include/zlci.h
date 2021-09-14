/*******************************************************************************
 * zlci.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: zlci.h 12354 2014-05-13 06:40:52Z htkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#ifndef _ZLCI_H_
#define _ZLCI_H_ 1

/**
 * @file zlci.h
 * @brief ODBC API Internal Bind Routines.
 */

/**
 * @defgroup zlci Internal Bind Routines
 * @{
 */

stlStatus zlciBuildTargetType( zlsStmt       * aStmt,
                               stlInt32        aTargetCount,
                               cmlTargetType * aTargetType,
                               stlErrorStack * aErrorStack );

stlStatus zlciInitParameterBlock( zlsStmt       * aStmt,
                                  stlInt32      * aInitParamCount,
                                  stlErrorStack * aErrorStack );

stlStatus zlciExecuteParameterType( zlsStmt       * aStmt,
                                    stlInt8       * aControl,
                                    stlErrorStack * aErrorStack );

stlStatus zlciSetParameterComplete( zlsStmt       * aStmt,
                                    stlErrorStack * aErrorStack );

stlStatus zlciExecuteParameterBlock( zlsStmt       * aStmt,
                                     stlInt8       * aControl,
                                     stlInt32      * aExecParamCount,
                                     stlErrorStack * aErrorStack );

stlStatus zlciExecuteOutParameterBlock( zlsStmt       * aStmt,
                                        stlBool         aIsLast,
                                        stlErrorStack * aErrorStack );

stlStatus zlciGetOutParameterBlock( zlsStmt       * aStmt,
                                    stlBool       * aIgnored,
                                    stlErrorStack * aErrorStack );

stlStatus zlciNumParams( zlcDbc        * aDbc,
                         zlsStmt       * aStmt,
                         stlInt16      * aParameterCount,
                         stlErrorStack * aErrorStack );

stlStatus zlciGetParameterType( zlcDbc        * aDbc,
                                zlsStmt       * aStmt,
                                stlUInt16       aParamIdx,
                                stlInt16      * aInputOutputType,
                                stlErrorStack * aErrorStack );

/** @} */

#endif /* _ZLCI_H_ */
