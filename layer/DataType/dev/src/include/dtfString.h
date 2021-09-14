/*******************************************************************************
 * dtfString.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: dtfString.h 1734 2011-08-23 04:59:53Z lym999 $
 *
 * NOTES
 *
 *
 ******************************************************************************/


#ifndef _DTF_STRING_H_
#define _DTF_STRING_H_ 1

/**
 * @file dtfString.h
 * @brief String for DataType Layer
 */

/**
 * @addtogroup dtfString String
 * @ingroup dtf
 * @{
 */


stlStatus dtfVarcharConcatenate( stlUInt16        aInputArgumentCnt,
                                 dtlValueEntry  * aInputArgument,
                                 void           * aResultValue,
                                 stlInt64         aPrecision,
                                 dtlFuncVector  * aVectorFunc,
                                 void           * aVectorArg,
                                 void           * aEnv );

stlStatus dtfLongvarcharConcatenate( stlUInt16        aInputArgumentCnt,
                                     dtlValueEntry  * aInputArgument,
                                     void           * aResultValue,
                                     dtlFuncVector  * aVectorFunc,
                                     void           * aVectorArg,
                                     void           * aEnv );

stlStatus dtfVarbinaryConcatenate( stlUInt16        aInputArgumentCnt,
                                   dtlValueEntry  * aInputArgument,
                                   void           * aResultValue,
                                   stlInt64         aPrecision,
                                   dtlFuncVector  * aVectorFunc,
                                   void           * aVectorArg,
                                   void           * aEnv );

stlStatus dtfLongvarbinaryConcatenate( stlUInt16        aInputArgumentCnt,
                                       dtlValueEntry  * aInputArgument,
                                       void           * aResultValue,
                                       dtlFuncVector  * aVectorFunc,
                                       void           * aVectorArg,
                                       void           * aEnv );

stlStatus dtfBitLength( stlUInt16        aInputArgumentCnt,
                        dtlValueEntry  * aInputArgument,
                        void           * aResultValue,
                        void           * aEnv );

stlStatus dtfLongvarcharBitLength( stlUInt16        aInputArgumentCnt,
                                   dtlValueEntry  * aInputArgument,
                                   void           * aResultValue,
                                   void           * aEnv );

stlStatus dtfCharLength( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         dtlFuncVector  * aVectorFunc,
                         void           * aVectorArg,
                         void           * aEnv );

stlStatus dtfLongvarcharLength( stlUInt16        aInputArgumentCnt,
                                dtlValueEntry  * aInputArgument,
                                void           * aResultValue,
                                dtlFuncVector  * aVectorFunc,
                                void           * aVectorArg,
                                void           * aEnv );

stlStatus dtfLower( stlUInt16        aInputArgumentCnt,
                    dtlValueEntry  * aInputArgument,
                    void           * aResultValue,
                    dtlFuncVector  * aVectorFunc,
                    void           * aVectorArg,
                    void           * aEnv );

stlStatus dtfStringOctetLength( stlUInt16        aInputArgumentCnt,
                                dtlValueEntry  * aInputArgument,
                                void           * aResultValue,
                                void           * aEnv );

stlStatus dtfBinaryStringOctetLength( stlUInt16        aInputArgumentCnt,
                                      dtlValueEntry  * aInputArgument,
                                      void           * aResultValue,
                                      void           * aEnv );

stlStatus dtfStringOverlay( stlUInt16        aInputArgumentCnt,
                            dtlValueEntry  * aInputArgument,
                            void           * aResultValue,
                            dtlFuncVector  * aVectorFunc,
                            void           * aVectorArg,
                            void           * aEnv );

stlStatus dtfLongvarcharStringOverlay( stlUInt16        aInputArgumentCnt,
                                       dtlValueEntry  * aInputArgument,
                                       void           * aResultValue,
                                       dtlFuncVector  * aVectorFunc,
                                       void           * aVectorArg,
                                       void           * aEnv );

stlStatus dtfBinaryStringOverlay( stlUInt16        aInputArgumentCnt,
                                  dtlValueEntry  * aInputArgument,
                                  stlInt64         aMaxPrecision,
                                  void           * aResultValue,
                                  void           * aEnv );

stlStatus dtfStringPosition( stlUInt16        aInputArgumentCnt,
                             dtlValueEntry  * aInputArgument,
                             void           * aResultValue,
                             dtlFuncVector  * aVectorFunc,
                             void           * aVectorArg,
                             void           * aEnv );

stlStatus dtfBinaryStringPosition( stlUInt16        aInputArgumentCnt,
                                   dtlValueEntry  * aInputArgument,
                                   void           * aResultValue,
                                   dtlFuncVector  * aVectorFunc,
                                   void           * aVectorArg,
                                   void           * aEnv );

stlStatus dtfStringSubstring( stlUInt16        aInputArgumentCnt,
                              dtlValueEntry  * aInputArgument,
                              void           * aResultValue,
                              dtlFuncVector  * aVectorFunc,
                              void           * aVectorArg,
                              void           * aEnv );

stlStatus dtfBinaryStringSubstring( stlUInt16        aInputArgumentCnt,
                                    dtlValueEntry  * aInputArgument,
                                    void           * aResultValue,
                                    void           * aEnv );

stlStatus dtfRegexpSubstring( stlUInt16        aInputArgumentCnt,
                              dtlValueEntry  * aInputArgument,
                              void           * aResultValue,
                              void           * aEnv );

stlStatus dtfStringTrim( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         dtlFuncVector  * aVectorFunc,
                         void           * aVectorArg,
                         void           * aEnv );

stlStatus dtfBinaryStringTrim( stlUInt16        aInputArgumentCnt,
                               dtlValueEntry  * aInputArgument,
                               void           * aResultValue,
                               void           * aEnv );

stlStatus dtfUpper( stlUInt16        aInputArgumentCnt,
                    dtlValueEntry  * aInputArgument,
                    void           * aResultValue,
                    dtlFuncVector  * aVectorFunc,
                    void           * aVectorArg,
                    void           * aEnv );

stlStatus dtfInitcap( stlUInt16        aInputArgumentCnt,
                      dtlValueEntry  * aInputArgument,
                      void           * aResultValue,
                      dtlFuncVector  * aVectorFunc,
                      void           * aVectorArg,
                      void           * aEnv );

stlStatus dtfStringLpad( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         dtlFuncVector  * aVectorFunc,
                         void           * aVectorArg,
                         void           * aEnv );

stlStatus dtfLongvarcharStringLpad( stlUInt16        aInputArgumentCnt,
                                    dtlValueEntry  * aInputArgument,
                                    void           * aResultValue,
                                    dtlFuncVector  * aVectorFunc,
                                    void           * aVectorArg,
                                    void           * aEnv );

stlStatus dtfBinaryLpad( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         stlInt64         aMaxPrecision,
                         void           * aResultValue,
                         dtlFuncVector  * aVectorFunc,
                         void           * aVectorArg,
                         void           * aEnv );

stlStatus dtfStringRpad( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         dtlFuncVector  * aVectorFunc,
                         void           * aVectorArg,
                         void           * aEnv );

stlStatus dtfLongvarcharStringRpad( stlUInt16        aInputArgumentCnt,
                                    dtlValueEntry  * aInputArgument,
                                    void           * aResultValue,
                                    dtlFuncVector  * aVectorFunc,
                                    void           * aVectorArg,
                                    void           * aEnv );

stlStatus dtfBinaryRpad( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         stlInt64         aMaxPrecision,
                         void           * aResultValue,
                         dtlFuncVector  * aVectorFunc,
                         void           * aVectorArg,
                         void           * aEnv );

stlStatus dtfStringRepeat( stlUInt16        aInputArgumentCnt,
                           dtlValueEntry  * aInputArgument,
                           void           * aResultValue,
                           dtlFuncVector  * aVectorFunc,
                           void           * aVectorArg,
                           void           * aEnv );

stlStatus dtfLongvarcharStringRepeat( stlUInt16        aInputArgumentCnt,
                                      dtlValueEntry  * aInputArgument,
                                      void           * aResultValue,
                                      dtlFuncVector  * aVectorFunc,
                                      void           * aVectorArg,
                                      void           * aEnv );

stlStatus dtfBinaryRepeat( stlUInt16        aInputArgumentCnt,
                           dtlValueEntry  * aInputArgument,
                           stlInt64         aMaxPrecision,
                           void           * aResultValue,
                           dtlFuncVector  * aVectorFunc,
                           void           * aVectorArg,
                           void           * aEnv );

stlStatus dtfReplace( stlUInt16        aInputArgumentCnt,
                      dtlValueEntry  * aInputArgument,
                      void           * aResultValue,
                      dtlFuncVector  * aVectorFunc,
                      void           * aVectorArg,
                      void           * aEnv );

stlStatus dtfLongvarcharReplace( stlUInt16        aInputArgumentCnt,
                                 dtlValueEntry  * aInputArgument,
                                 void           * aResultValue,
                                 dtlFuncVector  * aVectorFunc,
                                 void           * aVectorArg,
                                 void           * aEnv );

stlStatus dtfSplitPart( stlUInt16        aInputArgumentCnt,
                        dtlValueEntry  * aInputArgument,
                        void           * aResultValue,
                        dtlFuncVector  * aVectorFunc,
                        void           * aVectorArg,
                        void           * aEnv );

stlStatus dtfTranslate( stlUInt16        aInputArgumentCnt,
                        dtlValueEntry  * aInputArgument,
                        void           * aResultValue,
                        dtlFuncVector  * aVectorFunc,
                        void           * aVectorArg,
                        void           * aEnv );

stlStatus dtfLike( stlUInt16        aInputArgumentCnt,
                   dtlValueEntry  * aInputArgument,
                   void           * aResultValue,
                   void           * aInfo,
                   dtlFuncVector  * aVectorFunc,
                   void           * aVectorArg,
                   void           * aEnv );

stlStatus dtfLikePattern( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aInfo,
                          dtlFuncVector  * aVectorFunc,
                          void           * aVectorArg,
                          void           * aEnv );

stlStatus dtfNotLike( stlUInt16        aInputArgumentCnt,
                      dtlValueEntry  * aInputArgument,
                      void           * aResultValue,
                      void           * aInfo,
                      dtlFuncVector  * aVectorFunc,
                      void           * aVectorArg,
                      void           * aEnv );

stlStatus dtfExtractFromTimestamp( stlUInt16        aInputArgumentCnt,
                                   dtlValueEntry  * aInputArgument,
                                   void           * aResultValue,
                                   void           * aEnv );

stlStatus dtfExtractFromTimestampTz( stlUInt16        aInputArgumentCnt,
                                     dtlValueEntry  * aInputArgument,
                                     void           * aResultValue,
                                     void           * aEnv );

stlStatus dtfExtractFromDate( stlUInt16        aInputArgumentCnt,
                              dtlValueEntry  * aInputArgument,
                              void           * aResultValue,
                              void           * aEnv );

stlStatus dtfExtractFromTime( stlUInt16        aInputArgumentCnt,
                              dtlValueEntry  * aInputArgument,
                              void           * aResultValue,
                              void           * aEnv );

stlStatus dtfExtractFromTimeTz( stlUInt16        aInputArgumentCnt,
                                dtlValueEntry  * aInputArgument,
                                void           * aResultValue,
                                void           * aEnv );

stlStatus dtfExtractIntervalYearToMonth( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         void           * aEnv );

stlStatus dtfExtractIntervalDayToSecond( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         void           * aEnv );

/*******************************************************************************
 * string 내부 함수들
 ******************************************************************************/

stlStatus dtfBinaryStringSubstringInternal( stlChar           * aValue,
                                            stlInt64            aValueLength,
                                            stlInt64            aPosition,
                                            stlInt64            aLength,
                                            stlChar           * aResult,
                                            stlInt64          * aResultLen,
                                            stlErrorStack     * aErrorStack );

stlStatus dtfCommonPositionInternal( dtlDataValue     * aValue,
                                     dtlDataValue     * aSubValue,
                                     stlBool            aIsBinary,
                                     dtlDataValue     * aResultValue,
                                     dtlFuncVector    * aVectorFunc,
                                     void             * aVectorArg,
                                     stlErrorStack    * aErrorStack );

stlStatus dtfLikeCharacterInfo( stlChar         * aString,
                                stlChar         * aPattern,
                                stlInt64        * aStrLength,
                                stlInt64        * aPtnLength,
                                dtlCharacterSet   aCharSetID,
                                dtlBooleanType  * aResultValue,
                                stlErrorStack   * aErrorStack );


#define DTF_PATTERN_HAS_UNDERSCORE( pattern )                \
    ( ( ( ( pattern )->mAtomPatternCount > 1 ) || ( ( pattern )->mUnderscoreCount[0] > 0 ) ) ? STL_TRUE : STL_FALSE )

#define DTF_LIKE_PATTERN_IS_WILDCARD( aStringBegin, aCurPos, aEscapeList )    \
    ( ( ( *(aCurPos) == '%' ) && ( aEscapeList[(aCurPos) - (aStringBegin)] == STL_FALSE ) ) ? STL_TRUE : STL_FALSE )

#define DTF_LIKE_PATTERN_IS_UNDERSCORE( aStringBegin, aCurPos, aEscapeList )    \
    ( ( ( *(aCurPos) == '_' ) && ( aEscapeList[(aCurPos) - (aStringBegin)] == STL_FALSE ) ) ? STL_TRUE : STL_FALSE )


stlStatus dtfStringSubstrb( stlUInt16        aInputArgumentCnt,
                            dtlValueEntry  * aInputArgument,
                            void           * aResultValue,
                            dtlFuncVector  * aVectorFunc,
                            void           * aVectorArg,
                            void           * aEnv );

stlStatus dtfBinaryStringSubstrb( stlUInt16        aInputArgumentCnt,
                                  dtlValueEntry  * aInputArgument,
                                  void           * aResultValue,
                                  dtlFuncVector  * aVectorFunc,
                                  void           * aVectorArg,
                                  void           * aEnv );

stlStatus dtfStringInstr( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          dtlFuncVector  * aVectorFunc,
                          void           * aVectorArg,
                          void           * aEnv );

stlStatus dtfBinaryStringInstr( stlUInt16        aInputArgumentCnt,
                                dtlValueEntry  * aInputArgument,
                                void           * aResultValue,
                                dtlFuncVector  * aVectorFunc,
                                void           * aVectorArg,
                                void           * aEnv );

/*
 * LTRIM
 */

stlStatus dtfStringLtrim( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          dtlFuncVector  * aVectorFunc,
                          void           * aVectorArg,
                          void           * aEnv );

stlStatus dtfBinaryStringLtrim( stlUInt16        aInputArgumentCnt,
                                dtlValueEntry  * aInputArgument,
                                void           * aResultValue,
                                dtlFuncVector  * aVectorFunc,
                                void           * aVectorArg,
                                void           * aEnv );

/*
 * RTRIM
 */

stlStatus dtfStringRtrim( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          dtlFuncVector  * aVectorFunc,
                          void           * aVectorArg,
                          void           * aEnv );

stlStatus dtfBinaryStringRtrim( stlUInt16        aInputArgumentCnt,
                                dtlValueEntry  * aInputArgument,
                                void           * aResultValue,
                                dtlFuncVector  * aVectorFunc,
                                void           * aVectorArg,
                                void           * aEnv );

/**
 * CHR
 */

stlStatus dtfBigIntChr( stlUInt16        aInputArgumentCnt,
                        dtlValueEntry  * aInputArgument,
                        void           * aResultValue,
                        dtlFuncVector  * aVectorFunc,
                        void           * aVectorArg,
                        void           * aEnv );

stlStatus dtfNumericChr( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         dtlFuncVector  * aVectorFunc,
                         void           * aVectorArg,
                         void           * aEnv );

stlStatus dtfDoubleChr( stlUInt16        aInputArgumentCnt,
                        dtlValueEntry  * aInputArgument,
                        void           * aResultValue,
                        dtlFuncVector  * aVectorFunc,
                        void           * aVectorArg,
                        void           * aEnv );

stlStatus dtfChr( stlUInt64        aChrCode,
                  dtlDataValue   * aResultValue,
                  dtlFuncVector  * aVectorFunc,
                  void           * aVectorArg,
                  stlErrorStack  * aErrorStack );


/***********************************************************************
 * string common
 ***********************************************************************/ 

stlStatus dtfMbLength2BinaryLength( stlChar       * aSource,
                                    stlInt64        aLength,
                                    stlInt64        aMbLength,
                                    stlInt64      * aBinLength,
                                    stlBool       * aIsOver,
                                    dtlFuncVector * aVectorFunc,
                                    void          * aVectorArg,
                                    stlErrorStack * aErrorStack );

stlStatus dtfMbPosition2BinaryPosition( dtlCharacterSet aCharSetID,
                                        stlChar       * aSource,
                                        stlInt64        aLength,
                                        stlInt64        aMbPosition,
                                        stlInt64      * aBinPosition,
                                        stlBool       * bIsOver,
                                        dtlFuncVector * aVectorFunc,
                                        void          * aVectorArg,
                                        stlErrorStack * aErrorStack );

stlStatus dtfMbLength2BinaryLengthForAscii( stlChar       * aSource,
                                            stlInt64        aLength,
                                            stlInt64        aMbLength,
                                            stlInt64      * aBinLength,
                                            stlBool       * aIsOver,
                                            dtlFuncVector * aVectorFunc,
                                            void          * aVectorArg,
                                            stlErrorStack * aErrorStack );

stlStatus dtfMbLength2BinaryLengthForUtf8( stlChar       * aSource,
                                           stlInt64        aLength,
                                           stlInt64        aMbLength,
                                           stlInt64      * aBinLength,
                                           stlBool       * aIsOver,
                                           dtlFuncVector * aVectorFunc,
                                           void          * aVectorArg,
                                           stlErrorStack * aErrorStack );

stlStatus dtfMbLength2BinaryLengthForUhc( stlChar       * aSource,
                                          stlInt64        aLength,
                                          stlInt64        aMbLength,
                                          stlInt64      * aBinLength,
                                          stlBool       * aIsOver,
                                          dtlFuncVector * aVectorFunc,
                                          void          * aVectorArg,
                                          stlErrorStack * aErrorStack );

stlStatus dtfMbLength2BinaryLengthForGb18030( stlChar       * aSource,
                                              stlInt64        aLength,
                                              stlInt64        aMbLength,
                                              stlInt64      * aBinLength,
                                              stlBool       * aIsOver,
                                              dtlFuncVector * aVectorFunc,
                                              void          * aVectorArg,
                                              stlErrorStack * aErrorStack );



typedef stlStatus (*dtfMbLength2BinaryLengthFunc)( stlChar       * aSource,
                                                   stlInt64        aLength,
                                                   stlInt64        aMbLength,
                                                   stlInt64      * aBinLength,
                                                   stlBool       * aIsOver,
                                                   dtlFuncVector * aVectorFunc,
                                                   void          * aVectorArg,
                                                   stlErrorStack * aErrorStack );

typedef stlStatus (*dtfTrimLeadingFunc)( stlChar          * aString,
                                         stlInt64           aStringLen,
                                         stlChar          * aTrimChar,
                                         stlInt8            aTrimCharByte,
                                         stlChar         ** aRemainString,
                                         stlInt64         * aRemainStringLen,
                                         stlErrorStack    * aErrorStack );

typedef stlStatus (*dtfTrimTrailingFunc)( stlChar          * aString,
                                          stlInt64           aStringLen,
                                          stlChar          * aTrimChar,
                                          stlInt8            aTrimCharByte,
                                          stlInt64         * aRemainStringLen,
                                          stlErrorStack    * aErrorStack );

typedef stlStatus (*dtfStringLtrimFunc)( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         dtlFuncVector  * aVectorFunc,
                                         void           * aVectorArg,
                                         void           * aEnv );

typedef stlStatus (*dtfStringRtrimFunc)( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         dtlFuncVector  * aVectorFunc,
                                         void           * aVectorArg,
                                         void           * aEnv );

typedef stlStatus (*dtfInstrGetPositionFunc)( dtlDataValue   * aStrValue,
                                              dtlDataValue   * aSubstrValue,
                                              stlInt64         aPosition,
                                              stlInt64         aPositionByteLen,
                                              stlInt64         aOccurrence,
                                              stlInt64       * aResultPosition,
                                              stlErrorStack  * aErrorStack );

typedef stlStatus (*dtfStringSubstrbFunc)( stlUInt16        aInputArgumentCnt,
                                           dtlValueEntry  * aInputArgument,
                                           void           * aResultValue,
                                           dtlFuncVector  * aVectorFunc,
                                           void           * aVectorArg,
                                           void           * aEnv );


extern dtfMbLength2BinaryLengthFunc dtfMbLength2BinaryLengthFuncList[ DTL_CHARACTERSET_MAX + 1 ];


/** @} */

#endif /* _DTF_STRING_H_ */
