/*******************************************************************************
 * eldBuiltInDataType.h
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


#ifndef _ELD_BUILTIN_DATATYPE_H_
#define _ELD_BUILTIN_DATATYPE_H_ 1

/**
 * @file eldBuiltInDataType.h
 * @brief Built-In Data Type Dictionary 관리 루틴 
 */


/**
 * @defgroup eldBuiltInDataType Built-In Data Type Descriptor 관리 루틴 
 * @internal 
 * @ingroup eldDesc
 * @{
 */

stlStatus eldInsertBuiltInTypeDesc( smlTransId                      aTransID,
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


/** @} eldBuiltInDataType  */

#endif /* _ELD_BUILTIN_DATATYPE_H_ */
