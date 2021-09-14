/*******************************************************************************
 * knlValueBlock.h
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


#ifndef _KNL_VALUE_BLOCK_H_
#define _KNL_VALUE_BLOCK_H_ 1

/**
 * @file knlValueBlock.h
 * @brief Block Read를 위한 Value Block 관리 루틴
 */

/**
 * @addtogroup knlValueBlock
 * @{
 */

/**
 * @brief Source/Destination 관계를 표현한 Block List
 */
struct knlBlockRelationList
{
    stlBool                   mIsLeftSrc;    /**< Source 가 Left 인지 Right 인지 여부 */
    knlValueBlock           * mSrcBlock;     /**< Source Block */
    knlValueBlock           * mDstBlock;     /**< Destin Block */
    knlCastFuncInfo         * mCastFuncInfo; /**< Src -> Dst Type Cast Function Info */
    knlBlockRelationList    * mNext;
};


stlStatus knlInitBlock( knlValueBlockList ** aValueBlockList,
                        stlInt32             aBlockCnt,
                        stlBool              aIsSepBuff,
                        stlLayerClass        aAllocLayer,
                        stlInt64             aTableID,
                        stlInt32             aColumnOrder,
                        dtlDataType          aDBType,
                        stlInt64             aArgNum1,
                        stlInt64             aArgNum2,
                        dtlStringLengthUnit  aStringLengthUnit,
                        dtlIntervalIndicator aIntervalIndicator,
                        knlRegionMem       * aRegionMem,
                        knlEnv             * aEnv );

stlStatus knlInitBlockWithDataValue( knlValueBlockList ** aValueBlockList,
                                     dtlDataValue       * aDataValue,
                                     dtlDataType          aDBType,
                                     stlLayerClass        aAllocLayer,
                                     stlInt64             aArgNum1,
                                     stlInt64             aArgNum2,
                                     dtlStringLengthUnit  aStringLengthUnit,
                                     dtlIntervalIndicator aIntervalIndicator,
                                     knlRegionMem       * aRegionMem,
                                     knlEnv             * aEnv );

stlStatus knlInitShareBlock( knlValueBlockList ** aValueBlockList,
                             knlValueBlockList  * aSourceBlockList,
                             knlRegionMem       * aRegionMem,
                             knlEnv             * aEnv );

stlStatus knlInitShareBlockFromBlock( knlValueBlockList ** aValueBlockList,
                                      knlValueBlock      * aSourceBlock,
                                      knlRegionMem       * aRegionMem,
                                      knlEnv             * aEnv );

stlStatus knlInitCopyBlock( knlValueBlockList ** aValueBlockList,
                            stlLayerClass        aAllocLayer,
                            stlInt32             aBlockCnt,
                            knlValueBlockList  * aSourceBlockList,
                            knlRegionMem       * aRegionMem,
                            knlEnv             * aEnv );

stlStatus knlInitBlockNoBuffer( knlValueBlockList ** aValueBlockList,
                                stlLayerClass        aAllocLayer,
                                stlInt32             aBlockCnt,
                                dtlDataType          aDBType,
                                knlRegionMem       * aRegionMem,
                                knlEnv             * aEnv );

stlStatus knlGetBlockList( knlValueBlockList   * aSource,
                           stlInt32              aOrder,
                           knlValueBlockList  ** aResult,
                           knlEnv              * aEnv );

stlStatus  knlSetJoinResultBlock( knlBlockRelationList * aRelationList,
                                  stlInt32               aResultBlockIdx,
                                  stlInt32               aLeftBlockIdx,
                                  stlInt32               aRightBlockIdx,
                                  knlEnv               * aEnv );

stlStatus  knlSetLeftNullPaddJoinResultBlock( knlBlockRelationList * aRelationList,
                                              stlInt32               aResultBlockIdx,
                                              stlInt32               aRightBlockIdx,
                                              knlEnv               * aEnv );

stlStatus knlFreeLongVaryingValues( knlValueBlockList * aValueBlockList,
                                    knlEnv            * aEnv );

stlStatus knlAllocParamValues( dtlDataType             aDtlDataType,
                               stlInt64                aArg1,
                               stlInt64                aArg2,
                               dtlStringLengthUnit     aStringLengthUnit,
                               dtlIntervalIndicator    aIntervalIndicator,
                               stlLayerClass           aAllocLayer,
                               stlInt32                aBlockCount,
                               knlValueBlockList    ** aValueBlock,
                               knlEnv                * aEnv );

stlStatus knlFreeParamValues( knlValueBlockList * aValueBlockList,
                              knlEnv            * aEnv );

/** @} knlValueBlock */

#endif /* _KNL_VALUE_BLOCK_H_ */
