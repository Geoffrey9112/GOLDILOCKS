/*******************************************************************************
 * ellDictBuiltInDataType.h
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


#ifndef _ELL_DICT_BUILTIN_DATATYPE_H_
#define _ELL_DICT_BUILTIN_DATATYPE_H_ 1

/**
 * @file ellDictBuiltInDataType.h
 * @brief Built-In Data Type 관리 모듈
 */


/**
 * @addtogroup ellBuiltInDataType 
 * @{
 */

stlStatus ellInsertBuiltInTypeDesc( smlTransId                      aTransID,
                                    smlStatement                  * aStmt,
                                    stlChar                       * aTypeName,
                                    stlInt32                        aODBCDataType,
                                    stlInt32                        aJDBCDataType,
                                    stlInt32                        aColumnSize,
                                    stlChar                       * aLiteralPrefix,
                                    stlChar                       * aLiteralSuffix,
                                    stlChar                       * aCreateParams,
                                    stlInt32                        aNullable,
                                    stlBool                         aCaseSensitive,
                                    dtlSearchable                   aSearchable,
                                    stlInt32                        aUnsignedAttribute,
                                    stlBool                         aFixedPrecScale,
                                    stlInt32                        aAutoUniqueValue,
                                    stlChar                       * aLocalTypeName,
                                    stlInt32                        aMinimumScale,
                                    stlInt32                        aMaximumScale,
                                    stlInt32                        aSQLDataType,
                                    stlInt32                        aSQLDateTimeSub,
                                    dtlNumericPrecRadix             aNumPrecRadix,
                                    stlInt32                        aIntervalPrecision,
                                    stlBool                         aSupportedFeature,
                                    ellEnv                        * aEnv );

/** @} ellBuiltInDataType  */

#endif /* _ELL_DICT_BUILTIN_DATATYPE_H_ */
