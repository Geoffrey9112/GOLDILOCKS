/*******************************************************************************
 * smlIndex.h
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


#ifndef _SMLINDEX_H_
#define _SMLINDEX_H_ 1

/**
 * @file smlIndex.h
 * @brief Storage Manager Index Routines
 */

/**
 * @defgroup smlIndex Index
 * @ingroup smExternal
 * @{
 */

stlStatus smlCreateIndex( smlStatement      * aStatement,
                          smlTbsId            aTbsId,
                          smlIndexType        aIndexType,
                          smlIndexAttr      * aAttr,
                          void              * aBaseTableHandle,
                          stlUInt16           aKeyColCount,
                          knlValueBlockList * aIndexColList,
                          stlUInt8          * aKeyColFlags,
                          stlInt64          * aRelationId,
                          void             ** aRelationHandle,
                          smlEnv            * aEnv );

stlStatus smlDropIndex( smlStatement * aStatement,
                        void         * aRelationHandle,
                        smlEnv       * aEnv );

stlStatus smlTruncateIndex( smlStatement          * aStatement,
                            void                  * aOrgIndexHandle,
                            void                  * aNewBaseTableHandle,
                            smlDropStorageOption    aDropStorageOption,
                            void                 ** aNewIndexHandle,
                            stlInt64              * aNewIndexRelationId,
                            smlEnv                * aEnv );

stlStatus smlBuildIndex( smlStatement      * aStatement,
                         void              * aIndexHandle,
                         void              * aBaseTableHandle,
                         stlUInt16           aKeyColCount,
                         knlValueBlockList * aIndexColList,
                         stlUInt8          * aKeyColFlags,
                         stlUInt16           aParallelFactor,
                         smlEnv            * aEnv );

stlStatus smlBuildUnusableIndex( smlStatement      * aStatement,
                                 void              * aIndexHandle,
                                 smlEnv            * aEnv );

stlStatus smlAlterDataType( smlStatement * aStatement,
                            void         * aRelationHandle,
                            stlInt32       aColumnOrder,
                            dtlDataType    aDataType,
                            stlInt64       aColumnSize,
                            smlEnv       * aEnv );

stlStatus smlAlterIndexAttr( smlStatement * aStatement,
                             void         * aRelationHandle,
                             smlIndexAttr * aIndexAttr,
                             smlEnv       * aEnv );

/** @} */

#endif /* _SMLINDEX_H_ */
