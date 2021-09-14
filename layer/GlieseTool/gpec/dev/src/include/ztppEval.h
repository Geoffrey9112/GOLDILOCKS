/*******************************************************************************
 * ztppEval.h
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


#ifndef _ZTPPEVAL_H_
#define _ZTPPEVAL_H_ 1

/**
 * @file ztppEval.h
 * @brief C Preprocessor Parser for Gliese Embedded SQL
 */

/**
 * @defgroup ztppEval C Preprocessor Parser
 * @ingroup ztp
 * @{
 */

stlInt64 ztppEvalConstExpr( stlAllocator           *aAllocator,
                            stlErrorStack          *aErrorStack,
                            ztpPPConstExpr         *aConstExpr );

stlInt64 ztppEvalCondExpr( stlAllocator           *aAllocator,
                           stlErrorStack          *aErrorStack,
                           ztpPPCondExpr          *aCondExpr );

stlInt64 ztppEvalOrExpr( stlAllocator           *aAllocator,
                         stlErrorStack          *aErrorStack,
                         ztpPPOrExpr            *aOrExpr );

stlInt64 ztppEvalAndExpr( stlAllocator           *aAllocator,
                          stlErrorStack          *aErrorStack,
                          ztpPPAndExpr           *aAndExpr );

stlInt64 ztppEvalBitOrExpr( stlAllocator           *aAllocator,
                            stlErrorStack          *aErrorStack,
                            ztpPPBitOrExpr         *aBitOrExpr );

stlInt64 ztppEvalBitXorExpr( stlAllocator           *aAllocator,
                             stlErrorStack          *aErrorStack,
                             ztpPPBitXorExpr        *aBitXorExpr );

stlInt64 ztppEvalBitAndExpr( stlAllocator           *aAllocator,
                             stlErrorStack          *aErrorStack,
                             ztpPPBitAndExpr        *aBitAndExpr );

stlInt64 ztppEvalEqualExpr( stlAllocator           *aAllocator,
                            stlErrorStack          *aErrorStack,
                            ztpPPEqualExpr         *aEqualExpr );

stlInt64 ztppEvalRelationalExpr( stlAllocator           *aAllocator,
                                 stlErrorStack          *aErrorStack,
                                 ztpPPRelationalExpr    *aRelationalExpr );

stlInt64 ztppEvalShiftExpr( stlAllocator           *aAllocator,
                            stlErrorStack          *aErrorStack,
                            ztpPPShiftExpr         *aShiftExpr );

stlInt64 ztppEvalAddExpr( stlAllocator           *aAllocator,
                          stlErrorStack          *aErrorStack,
                          ztpPPAddExpr           *aAddExpr );

stlInt64 ztppEvalMulExpr( stlAllocator           *aAllocator,
                          stlErrorStack          *aErrorStack,
                          ztpPPMulExpr           *aMulExpr );

stlInt64 ztppEvalUnaryExpr( stlAllocator           *aAllocator,
                            stlErrorStack          *aErrorStack,
                            ztpPPUnaryExpr         *aUnaryExpr );

stlInt64 ztppEvalPrimaryExpr( stlAllocator           *aAllocator,
                              stlErrorStack          *aErrorStack,
                              ztpPPPrimaryExpr       *aPrimaryExpr );

stlInt64 ztppEvalObjectLikeMacro( stlAllocator      *aAllocator,
                                  stlErrorStack     *aErrorStack,
                                  stlChar           *aMacroName );

stlInt64 ztppEvalFunctionLikeMacro( stlAllocator          *aAllocator,
                                    stlErrorStack         *aErrorStack,
                                    stlChar               *aMacroName,
                                    ztpPPParameterList    *aParameterList );


/** @} */

#endif /* _ZTPPEVAL_H_ */
