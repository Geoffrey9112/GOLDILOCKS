/*******************************************************************************
 * knlBindContext.h
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


#ifndef _KNLBINDCONTEXT_H_
#define _KNLBINDCONTEXT_H_ 1

/**
 * @file knlBindContext.h
 * @brief Kernel Layer Bind Context Routines
 */

/**
 * @defgroup knlBindContext Bind Context
 * @ingroup knExternal
 * @{
 */

stlStatus knlCreateBindContext( knlBindContext ** aBindContext,
                                knlRegionMem    * aRegionMem,
                                knlEnv          * aEnv );

stlInt32 knlGetBindContextSize( knlBindContext * aBindContext );

stlStatus knlCreateOrReplaceBindParam( knlBindContext    * aBindContext,
                                       knlRegionMem      * aRegionMem,
                                       stlInt32            aParamId,
                                       knlBindType         aBindType,
                                       knlValueBlockList * aINValueBlock,
                                       knlValueBlockList * aOUTValueBlock,
                                       knlEnv            * aEnv );

stlStatus knlGetParamValueBlock( knlBindContext     * aBindContext,
                                 stlInt32             aParamId,
                                 knlValueBlockList ** aINValueBlock,
                                 knlValueBlockList ** aOUTValueBlock,
                                 stlErrorStack      * aErrorStack );

stlStatus knlGetBindParamType( knlBindContext  * aBindContext,
                               stlInt32          aParamId,
                               knlBindType     * aBindType,
                               stlErrorStack   * aErrorStack );

stlInt32 knlGetExecuteValueBlockSize( knlBindContext * aBindContext );

stlStatus knlGetExecuteParamValueBlock( knlBindContext     * aBindContext,
                                        stlInt32             aParamId,
                                        knlValueBlockList ** aValueBlock,
                                        stlErrorStack      * aErrorStack );

stlStatus knlInitExecuteParamValueBlocks( knlBindContext * aBindContext,
                                          stlUInt64        aMoveSize,
                                          stlErrorStack  * aErrorStack );

stlStatus knlMoveExecuteParamValueBlocks( knlBindContext * aBindContext,
                                          stlUInt64        aMoveSize,        
                                          stlErrorStack  * aErrorStack );

stlStatus knlResetExecuteParamValueBlocks( knlBindContext * aBindContext,
                                           stlErrorStack  * aErrorStack );

stlStatus knlSetExecuteValueBlockSize( knlBindContext * aBindContext,
                                       stlUInt64        aExecuteSize,
                                       stlErrorStack  * aErrorStack );

stlStatus knlFreeServerBindContext( knlBindContext * aBindContext,
                                    knlEnv         * aEnv );

/** @} */
    
#endif /* _KNLBINDCONTEXT_H_ */
