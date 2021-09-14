/*******************************************************************************
 * knlDataValue.h
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


#ifndef _KNL_DATA_VALUE_H_
#define _KNL_DATA_VALUE_H_ 1

/**
 * @file knlDataValue.h
 * @brief Block Read를 위한 Value Block 관리 루틴
 */

/**
 * @addtogroup knlDataValue
 * @{
 */

stlStatus knlAllocDataValueArray( dtlDataValue           * aDataValue,
                                  dtlDataType              aDBType,
                                  stlInt64                 aPrecision,
                                  dtlStringLengthUnit      aStringLengthUnit,
                                  stlInt32                 aArraySize,
                                  knlRegionMem           * aRegionMem,
                                  stlInt64               * aBufferSize,
                                  knlEnv                 * aEnv );


stlStatus knlAllocDataValueArrayDynamic( dtlDataValue           * aDataValue,
                                         dtlDataType              aDBType,
                                         stlInt64                 aPrecision,
                                         dtlStringLengthUnit      aStringLengthUnit,
                                         stlInt32                 aArraySize,
                                         knlDynamicMem          * aDynamicMem,
                                         stlInt64               * aBufferSize,
                                         knlEnv                 * aEnv );


stlStatus knlAllocDataValueArrayFromString( dtlDataValue           * aDataValue,
                                            dtlDataType              aDBType,
                                            stlInt64                 aPrecision,
                                            stlInt64                 aScale,
                                            dtlStringLengthUnit      aStringLengthUnit,
                                            stlChar                * aString,
                                            stlInt64                 aStringLength,
                                            stlInt32                 aArraySize,
                                            knlRegionMem           * aRegionMem,
                                            stlInt64               * aBufferSize,
                                            knlEnv                 * aEnv );

stlStatus knlFreeLongDataValueArray( dtlDataValue * aDataValue,
                                     stlInt32       aArraySize,
                                     knlEnv       * aEnv );

stlStatus knlCopyDataValue( dtlDataValue * aSrcValue,
                            dtlDataValue * aDstValue,
                            knlEnv       * aEnv );

stlStatus knlAllocLongVaryingMem( stlInt32     aAllocSize,
                                  void      ** aAddr,
                                  knlEnv     * aEnv );

stlStatus knlFreeLongVaryingMem( void   * aAddr,
                                 knlEnv * aEnv );

stlStatus knlReallocLongVaryingMem( stlInt32     aAllocSize,
                                    void      ** aAddr,
                                    void       * aEnv );


stlStatus knlReallocAndMoveLongVaryingMem( dtlDataValue  * aDataValue,
                                           stlInt32        aAllocSize,
                                           knlEnv        * aEnv );

void knlVerifyWriteDataValue( knlValueBlockList * aWriteBlockList,
                              stlInt64            aSkipCnt,
                              stlInt64            aUsedCnt,
                              knlEnv            * aEnv );

/** @} knlDataValue */

#endif /* _KNL_DATA_VALUE_H_ */
