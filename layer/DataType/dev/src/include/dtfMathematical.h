/*******************************************************************************
 * dtfMathematical.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: dtfMathematical.h 1580 2011-08-01 02:36:06Z ehpark $
 *
 * NOTES
 *
 *
 ******************************************************************************/


#ifndef _DTF_MATHEMATICAL_H_
#define _DTF_MATHEMATICAL_H_ 1

/**
 * @file dtfMathematical.h
 * @brief Mathematical for DataType Layer
 */

/**
 * @addtogroup dtfMathematical Mathematical
 * @ingroup dtf
 * @{
 */

stlStatus dtfBigIntPower( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aEnv );

stlStatus dtfNumericPower( stlUInt16        aInputArgumentCnt,
                           dtlValueEntry  * aInputArgument,
                           void           * aResultValue,
                           void           * aEnv );

stlStatus dtfDoublePower( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aEnv );


// stlStatus dtfBigIntSqrt( stlUInt16        aInputArgumentCnt,
//                          dtlValueEntry  * aInputArgument,
//                          void           * aResultValue,
//                          void           * aEnv );

stlStatus dtfNumericSqrt( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aEnv );

stlStatus dtfDoubleSqrt( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aEnv );


stlStatus dtfBigIntCbrt( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aEnv );

stlStatus dtfNumericCbrt( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aEnv );

stlStatus dtfDoubleCbrt( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aEnv );


stlStatus dtfBigIntFact( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aEnv );

stlStatus dtfNumericFact( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aEnv );


stlStatus dtfSmallIntAbs( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aInfo,
                          dtlFuncVector  * aVectorFunc,
                          void           * aVectorArg,
                          void           * aEnv );

stlStatus dtfIntegerAbs( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aInfo,
                         dtlFuncVector  * aVectorFunc,
                         void           * aVectorArg,
                         void           * aEnv );

stlStatus dtfBigIntAbs( stlUInt16        aInputArgumentCnt,
                        dtlValueEntry  * aInputArgument,
                        void           * aResultValue,
                        void           * aInfo,
                        dtlFuncVector  * aVectorFunc,
                        void           * aVectorArg,
                        void           * aEnv );

stlStatus dtfNumericAbs( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aInfo,
                         dtlFuncVector  * aVectorFunc,
                         void           * aVectorArg,
                         void           * aEnv );
    
stlStatus dtfRealAbs( stlUInt16        aInputArgumentCnt,
                      dtlValueEntry  * aInputArgument,
                      void           * aResultValue,
                      void           * aInfo,
                      dtlFuncVector  * aVectorFunc,
                      void           * aVectorArg,
                      void           * aEnv );

stlStatus dtfDoubleAbs( stlUInt16        aInputArgumentCnt,
                        dtlValueEntry  * aInputArgument,
                        void           * aResultValue,
                        void           * aInfo,
                        dtlFuncVector  * aVectorFunc,
                        void           * aVectorArg,
                        void           * aEnv );


stlStatus dtfBigIntCeil( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aEnv );

stlStatus dtfNumericCeil( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aEnv );

stlStatus dtfDoubleCeil( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aEnv );


// stlStatus dtfBigIntDegrees( stlUInt16        aInputArgumentCnt,
//                             dtlValueEntry  * aInputArgument,
//                             void           * aResultValue,
//                             void           * aEnv );

stlStatus dtfNumericDegrees( stlUInt16        aInputArgumentCnt,
                             dtlValueEntry  * aInputArgument,
                             void           * aResultValue,
                             void           * aEnv );

stlStatus dtfDoubleDegrees( stlUInt16        aInputArgumentCnt,
                            dtlValueEntry  * aInputArgument,
                            void           * aResultValue,
                            void           * aEnv );


// stlStatus dtfBigIntExp( stlUInt16        aInputArgumentCnt,
//                         dtlValueEntry  * aInputArgument,
//                         void           * aResultValue,
//                         void           * aEnv );

stlStatus dtfNumericExp( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aEnv );

stlStatus dtfDoubleExp( stlUInt16        aInputArgumentCnt,
                        dtlValueEntry  * aInputArgument,
                        void           * aResultValue,
                        void           * aEnv );


stlStatus dtfBigIntFloor( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aEnv );

stlStatus dtfNumericFloor( stlUInt16        aInputArgumentCnt,
                           dtlValueEntry  * aInputArgument,
                           void           * aResultValue,
                           void           * aEnv );

stlStatus dtfDoubleFloor( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aEnv );


stlStatus dtfBigIntLn( stlUInt16        aInputArgumentCnt,
                       dtlValueEntry  * aInputArgument,
                       void           * aResultValue,
                       void           * aEnv );

stlStatus dtfNumericLn( stlUInt16        aInputArgumentCnt,
                        dtlValueEntry  * aInputArgument,
                        void           * aResultValue,
                        void           * aEnv );

stlStatus dtfDoubleLn( stlUInt16        aInputArgumentCnt,
                       dtlValueEntry  * aInputArgument,
                       void           * aResultValue,
                       void           * aEnv );


stlStatus dtfBigIntLog10( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aEnv );

stlStatus dtfNumericLog10( stlUInt16        aInputArgumentCnt,
                           dtlValueEntry  * aInputArgument,
                           void           * aResultValue,
                           void           * aEnv );

stlStatus dtfDoubleLog10( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aEnv );

stlStatus dtfDoubleLog( stlUInt16        aInputArgumentCnt,
                        dtlValueEntry  * aInputArgument,
                        void           * aResultValue,
                        void           * aEnv );

stlStatus dtfNumericLog( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aEnv );


stlStatus dtfPi( stlUInt16        aInputArgumentCnt,
                 dtlValueEntry  * aInputArgument,
                 void           * aResultValue,
                 void           * aEnv );


// stlStatus dtfBigIntRadians( stlUInt16        aInputArgumentCnt,
//                             dtlValueEntry  * aInputArgument,
//                             void           * aResultValue,
//                             void           * aEnv );

stlStatus dtfNumericRadians( stlUInt16        aInputArgumentCnt,
                             dtlValueEntry  * aInputArgument,
                             void           * aResultValue,
                             void           * aEnv );

stlStatus dtfDoubleRadians( stlUInt16        aInputArgumentCnt,
                            dtlValueEntry  * aInputArgument,
                            void           * aResultValue,
                            void           * aEnv );


stlStatus dtfBigIntRandom( stlUInt16        aInputArgumentCnt,
                           dtlValueEntry  * aInputArgument,
                           void           * aResultValue,
                           void           * aEnv );

stlStatus dtfNumericRandom( stlUInt16        aInputArgumentCnt,
                            dtlValueEntry  * aInputArgument,
                            void           * aResultValue,
                            void           * aEnv );

stlStatus dtfDoubleRandom( stlUInt16        aInputArgumentCnt,
                           dtlValueEntry  * aInputArgument,
                           void           * aResultValue,
                           void           * aEnv );


stlStatus dtfBigIntRound( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aEnv );

stlStatus dtfNumericRound( stlUInt16        aInputArgumentCnt,
                           dtlValueEntry  * aInputArgument,
                           void           * aResultValue,
                           void           * aEnv );

stlStatus dtfDoubleRound( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aEnv );

stlStatus dtfDateRound( stlUInt16        aInputArgumentCnt,
                        dtlValueEntry  * aInputArgument,
                        void           * aResultValue,
                        void           * aInfo,
                        dtlFuncVector  * aVectorFunc,
                        void           * aVectorArg,
                        void           * aEnv );            


stlStatus dtfBigIntSign( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aEnv );

stlStatus dtfNumericSign( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aEnv );

stlStatus dtfDoubleSign( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aEnv );


stlStatus dtfBigIntTrunc( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aEnv );

stlStatus dtfNumericTrunc( stlUInt16        aInputArgumentCnt,
                           dtlValueEntry  * aInputArgument,
                           void           * aResultValue,
                           void           * aEnv );

stlStatus dtfDoubleTrunc( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aEnv );

stlStatus dtfDateTrunc( stlUInt16        aInputArgumentCnt,
                        dtlValueEntry  * aInputArgument,
                        void           * aResultValue,
                        void           * aInfo,
                        dtlFuncVector  * aVectorFunc,
                        void           * aVectorArg,
                        void           * aEnv );


stlStatus dtfBigIntWidthBucket( stlUInt16        aInputArgumentCnt,
                                dtlValueEntry  * aInputArgument,
                                void           * aResultValue,
                                dtlFuncVector  * aVectorFunc,
                                void           * aVectorArg,
                                void           * aEnv );

stlStatus dtfNumericWidthBucket( stlUInt16        aInputArgumentCnt,
                                 dtlValueEntry  * aInputArgument,
                                 void           * aResultValue,
                                 dtlFuncVector  * aVectorFunc,
                                 void           * aVectorArg,
                                 void           * aEnv );

stlStatus dtfDoubleWidthBucket( stlUInt16        aInputArgumentCnt,
                                dtlValueEntry  * aInputArgument,
                                void           * aResultValue,
                                dtlFuncVector  * aVectorFunc,
                                void           * aVectorArg,
                                void           * aEnv );


// stlStatus dtfBigIntAcos( stlUInt16        aInputArgumentCnt,
//                          dtlValueEntry  * aInputArgument,
//                          void           * aResultValue,
//                          void           * aEnv );

stlStatus dtfNumericAcos( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aEnv );

stlStatus dtfDoubleAcos( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aEnv );


// stlStatus dtfBigIntAsin( stlUInt16        aInputArgumentCnt,
//                          dtlValueEntry  * aInputArgument,
//                          void           * aResultValue,
//                          void           * aEnv );

stlStatus dtfNumericAsin( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aEnv );

stlStatus dtfDoubleAsin( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aEnv );


// stlStatus dtfBigIntAtan( stlUInt16        aInputArgumentCnt,
//                          dtlValueEntry  * aInputArgument,
//                          void           * aResultValue,
//                          void           * aEnv );

stlStatus dtfNumericAtan( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aEnv );

stlStatus dtfDoubleAtan( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aEnv );


// stlStatus dtfBigIntAtan2( stlUInt16        aInputArgumentCnt,
//                           dtlValueEntry  * aInputArgument,
//                           void           * aResultValue,
//                           void           * aEnv );

stlStatus dtfNumericAtan2( stlUInt16        aInputArgumentCnt,
                           dtlValueEntry  * aInputArgument,
                           void           * aResultValue,
                           void           * aEnv );

stlStatus dtfDoubleAtan2( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aEnv );


// stlStatus dtfBigIntCos( stlUInt16        aInputArgumentCnt,
//                         dtlValueEntry  * aInputArgument,
//                         void           * aResultValue,
//                         void           * aEnv );

stlStatus dtfNumericCos( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aEnv );

stlStatus dtfDoubleCos( stlUInt16        aInputArgumentCnt,
                        dtlValueEntry  * aInputArgument,
                        void           * aResultValue,
                        void           * aEnv );


// stlStatus dtfBigIntCot( stlUInt16        aInputArgumentCnt,
//                         dtlValueEntry  * aInputArgument,
//                         void           * aResultValue,
//                         void           * aEnv );

stlStatus dtfNumericCot( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aEnv );

stlStatus dtfDoubleCot( stlUInt16        aInputArgumentCnt,
                        dtlValueEntry  * aInputArgument,
                        void           * aResultValue,
                        void           * aEnv );


// stlStatus dtfBigIntSin( stlUInt16        aInputArgumentCnt,
//                         dtlValueEntry  * aInputArgument,
//                         void           * aResultValue,
//                         void           * aEnv );

stlStatus dtfNumericSin( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aEnv );

stlStatus dtfDoubleSin( stlUInt16        aInputArgumentCnt,
                        dtlValueEntry  * aInputArgument,
                        void           * aResultValue,
                        void           * aEnv );


// stlStatus dtfBigIntTan( stlUInt16        aInputArgumentCnt,
//                         dtlValueEntry  * aInputArgument,
//                         void           * aResultValue,
//                         void           * aEnv );

stlStatus dtfNumericTan( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aEnv );

stlStatus dtfDoubleTan( stlUInt16        aInputArgumentCnt,
                        dtlValueEntry  * aInputArgument,
                        void           * aResultValue,
                        void           * aEnv );


stlStatus dtfDoubleRoundWithScale( dtlDoubleType    aValue1,
                                   dtlIntegerType   aScale,
                                   stlBool          aIsTrunc,
                                   stlFloat64     * aResultValue,
                                   stlErrorStack  * aErrorStack );

/** @} */

#endif /* _DTF_MATHEMATICAL_H_ */
