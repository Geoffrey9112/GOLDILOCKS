/*******************************************************************************
 * zlai.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: zlai.h 12354 2014-05-13 06:40:52Z htkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#ifndef _ZLAI_H_
#define _ZLAI_H_ 1

/**
 * @file zlai.h
 * @brief ODBC API Internal Bind Routines.
 */

/**
 * @defgroup zlai Internal Bind Routines
 * @{
 */

/*
 * column
 */
stlStatus zlaiBuildNumResultCols( zlsStmt       * aStmt,
                                  stlErrorStack * aErrorStack );

stlStatus zlaiBuildNumResultColsInternal( zlsStmt  * aStmt,
                                          void     * aEnv );

/*
 * parameter
 */

stlStatus zlaiInitParameterBlock( zlsStmt   * aStmt,
                                  stlInt32  * aInitParamCount,
                                  void      * aEnv );

stlStatus zlaiNumParams( zlcDbc        * aDbc,
                         zlsStmt       * aStmt,
                         stlInt16      * aParameterCount,
                         stlErrorStack * aErrorStack );

stlStatus zlaiNumParamsInternal( zlcDbc        * aDbc,
                                 zlsStmt       * aStmt,
                                 stlInt16      * aParameterCount,
                                 void          * aEnv );

stlStatus zlaiGetParameterType( zlcDbc        * aDbc,
                                zlsStmt       * aStmt,
                                stlUInt16       aParamIdx,
                                stlInt16      * aInputOutputType,
                                stlErrorStack * aErrorStack );

/** @} */

#endif /* _ZLAI_H_ */
