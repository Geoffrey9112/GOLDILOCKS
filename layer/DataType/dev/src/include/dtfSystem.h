/*******************************************************************************
 * dtfSystem.h
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


#ifndef _DTF_SYSTEM_H_
#define _DTF_SYSTEM_H_ 1

/**
 * @file dtfSystem.h
 * @brief System Functions for DataType Layer
 */

/**
 * @addtogroup dtfSystem System Function
 * @ingroup dtf
 * @internal
 * @{
 */

extern const stlChar gDtfDecimalString[ 256 ][ 3 ];
extern const stlInt32 gDtfDecimalStringSize[ 256 ];
extern dtfBuiltInFuncPtr gDtfDumpFunc[ DTL_TYPE_MAX ];


stlStatus dtfInvalidDump( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aEnv );

stlStatus dtfBooleanDump( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aEnv );

stlStatus dtfSmallIntDump( stlUInt16        aInputArgumentCnt,
                           dtlValueEntry  * aInputArgument,
                           void           * aResultValue,
                           void           * aEnv );

stlStatus dtfIntegerDump( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aEnv );

stlStatus dtfBigIntDump( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aEnv );
    
stlStatus dtfRealDump( stlUInt16        aInputArgumentCnt,
                       dtlValueEntry  * aInputArgument,
                       void           * aResultValue,
                       void           * aEnv );

stlStatus dtfDoubleDump( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aEnv );

stlStatus dtfNumericDump( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aEnv );

stlStatus dtfCharDump( stlUInt16        aInputArgumentCnt,
                       dtlValueEntry  * aInputArgument,
                       void           * aResultValue,
                       void           * aEnv );

stlStatus dtfVarcharDump( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aEnv );

stlStatus dtfLongvarcharDump( stlUInt16        aInputArgumentCnt,
                              dtlValueEntry  * aInputArgument,
                              void           * aResultValue,
                              void           * aEnv );

stlStatus dtfBinaryDump( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aEnv );

stlStatus dtfVarbinaryDump( stlUInt16        aInputArgumentCnt,
                            dtlValueEntry  * aInputArgument,
                            void           * aResultValue,
                            void           * aEnv );

stlStatus dtfLongvarbinaryDump( stlUInt16        aInputArgumentCnt,
                                dtlValueEntry  * aInputArgument,
                                void           * aResultValue,
                                void           * aEnv );

stlStatus dtfDateDump( stlUInt16        aInputArgumentCnt,
                       dtlValueEntry  * aInputArgument,
                       void           * aResultValue,
                       void           * aEnv );

stlStatus dtfTimeDump( stlUInt16        aInputArgumentCnt,
                       dtlValueEntry  * aInputArgument,
                       void           * aResultValue,
                       void           * aEnv );

stlStatus dtfTimestampDump( stlUInt16        aInputArgumentCnt,
                            dtlValueEntry  * aInputArgument,
                            void           * aResultValue,
                            void           * aEnv );

stlStatus dtfTimeTzDump( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aEnv );

stlStatus dtfTimestampTzDump( stlUInt16        aInputArgumentCnt,
                              dtlValueEntry  * aInputArgument,
                              void           * aResultValue,
                              void           * aEnv );

stlStatus dtfIntervalYearToMonthDump( stlUInt16        aInputArgumentCnt,
                                      dtlValueEntry  * aInputArgument,
                                      void           * aResultValue,
                                      void           * aEnv );

stlStatus dtfIntervalDaytoSecondDump( stlUInt16        aInputArgumentCnt,
                                      dtlValueEntry  * aInputArgument,
                                      void           * aResultValue,
                                      void           * aEnv );

stlStatus dtfRowIdDump( stlUInt16        aInputArgumentCnt,
                        dtlValueEntry  * aInputArgument,
                        void           * aResultValue,
                        void           * aEnv );


/** @} */
    
#endif /* _STLSTATUS DTF_SYSTEM_H_ */
