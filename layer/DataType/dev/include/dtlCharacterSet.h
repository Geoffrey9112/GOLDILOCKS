/*******************************************************************************
 * dtlCharacterSet.h
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

#ifndef _DTL_CHARACTERSET_DEF_H_
#define _DTL_CHARACTERSET_DEF_H_ 1

/**
 * @file dtlCharacterSet.h
 * @brief DataType Layer Character Set 들에 대한 정의 
 */

/**
 * @defgroup dtlCharSet  Character Set
 * @ingroup dtlGeneral
 * @{
 */

/**
 * @defgroup dtlCharSetDefinition Character Set 정의 
 * @{
 */

extern stlChar * gDtlCharacterSetString[DTL_CHARACTERSET_MAX];


/** @} dtlCharSetDefinition */





/**
 * @defgroup dtlCharSetControl Character Set 제어 
 * @{
 */

stlStatus dtlMbStrVerify( dtlCharacterSet   aCharacterSet,
                          stlChar         * aSource,
                          stlInt64          aLength,
                          stlInt8         * aMbLength,
                          stlErrorStack   * aErrorStack );

stlStatus dtlGetMbLength( dtlCharacterSet   aCharacterSet,
                          stlChar         * aSource, 
                          stlInt8         * aMbLength,
                          stlErrorStack   * aErrorStack );

stlInt8 dtlGetMbMaxLength( dtlCharacterSet   aCharacterSet );

stlInt32 dtlGetMbDisplayLength( dtlCharacterSet   aCharacterSet,
                                stlUInt8        * aChar);

stlStatus dtlGetMbstrlenWithLen( dtlCharacterSet   aCharacterSet,
                                 stlChar         * aString,
                                 stlInt64          aLimit,
                                 stlInt64        * aMbStrLen,
                                 dtlFuncVector   * aVectorFunc,
                                 void            * aVectorArg,
                                 stlErrorStack   * aErrorStack );

stlStatus dtlMbConversion( dtlCharacterSet   aSourceCharacterSet,
                           dtlCharacterSet   aDestCharacterSet,
                           stlChar         * aSource,
                           stlInt64          aSourceLen,
                           stlChar         * aDest,
                           stlInt64          aDestLen,
                           stlInt64        * aSourceOffset,
                           stlInt64        * aTotalLength,
                           stlErrorStack   * aErrorStack );

stlStatus dtlGetMbDisplayLengthWithLen( dtlCharacterSet   aCharacterSet,
                                        stlChar         * aSource,
                                        stlInt64          aSourceLength,
                                        stlInt64        * aDisplayLength,
                                        stlErrorStack   * aErrorStack );

stlStatus dtlUnicodeToMbConversion( dtlUnicodeEncoding   aUniEncoding,
                                    dtlCharacterSet      aMbCharset,
                                    void               * aSource,
                                    stlInt64             aSourceLen,
                                    stlChar            * aDest,
                                    stlInt64             aDestLen,
                                    stlInt64           * aSourceOffset,
                                    stlInt64           * aTotalLength,
                                    stlErrorStack      * aErrorStack );

stlStatus dtlMbToUnicodeConversion( dtlCharacterSet      aMbCharset,
                                    dtlUnicodeEncoding   aUniEncoding,
                                    stlChar            * aSource,
                                    stlInt64             aSourceLen,
                                    void               * aDest,
                                    stlInt64             aDestLen,
                                    stlInt64           * aSourceOffset,
                                    stlInt64           * aTotalLength,
                                    stlErrorStack      * aErrorStack );


/** @} dtlCharSetDefinition */


stlStatus dtlGetCharacterSet( stlChar         * aName,
                              dtlCharacterSet * aCharacterSet,
                              stlErrorStack   * aErrorStack );

stlStatus dtlGetSystemCharacterset( dtlCharacterSet * aCharacterSet,
                                    stlErrorStack   * aErrorStack );

stlStatus dtlCheckCharacterSet( dtlCharacterSet   aCharacterSet,
                                stlErrorStack   * aErrorStack );

/** @} dtlCharSet */


#endif /* _DTLCHARACTERSETDEF_H_ */
