/*******************************************************************************
 * smlTable.h
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


#ifndef _SMLTABLE_H_
#define _SMLTABLE_H_ 1

/**
 * @file smlTable.h
 * @brief Storage Manager Table Routines
 */

/**
 * @defgroup smlTable Table
 * @ingroup smExternal
 * @{
 */

stlStatus smlCreateTable( smlStatement     * aStatement,
                          smlTbsId           aTbsId,
                          smlRelationType    aRelationType,
                          smlTableAttr     * aAttr,
                          stlInt64         * aRelationId,
                          void            ** aRelationHandle,
                          smlEnv           * aEnv );

stlStatus smlCreateIoTable( smlStatement      * aStatement,
                            smlTbsId            aTbsId,
                            smlRelationType     aRelationType,
                            smlTableAttr      * aTableAttr,
                            smlIndexAttr      * aIndexAttr,
                            knlValueBlockList * aColList,
                            stlUInt16           aKeyColCount,
                            stlUInt8          * aKeyColFlags,
                            stlInt64          * aRelationId,
                            void             ** aRelationHandle,
                            smlEnv            * aEnv );

stlStatus smlCreateSortTable( smlStatement       * aStatement,
                              smlTbsId             aTbsId,
                              smlSortTableAttr   * aAttr,
                              void               * aBaseTableHandle,
                              stlUInt16            aKeyColCount,
                              knlValueBlockList  * aKeyColList,
                              stlUInt8           * aKeyColFlags,
                              stlInt64           * aRelationId,
                              void              ** aRelationHandle,
                              smlEnv             * aEnv );

stlStatus smlCreateHashTable( smlStatement       * aStatement,
                              smlTbsId             aTbsId,
                              smlIndexAttr       * aAttr,
                              void               * aBaseTableHandle,
                              stlUInt16            aKeyColCount,
                              knlValueBlockList  * aKeyColList,
                              stlUInt8           * aKeyColFlags,
                              stlInt64           * aRelationId,
                              void              ** aRelationHandle,
                              smlEnv             * aEnv );

stlStatus smlDropTable( smlStatement * aStatement,
                        void         * aRelationHandle,
                        smlEnv       * aEnv );

stlStatus smlAlterTableAddColumns( smlStatement      * aStatement,
                                   void              * aRelationHandle,
                                   knlValueBlockList * aAddColumns,
                                   smlRowBlock       * aRowBlock,
                                   smlEnv            * aEnv );

stlStatus smlAlterTableDropColumns( smlStatement      * aStatement,
                                    void              * aRelationHandle,
                                    knlValueBlockList * aDropColumns,
                                    smlEnv            * aEnv );

stlStatus smlAlterTableAttr( smlStatement * aStatement,
                             void         * aRelationHandle,
                             smlTableAttr * aTableAttr,
                             smlIndexAttr * aIndexAttr,
                             smlEnv       * aEnv );

stlStatus smlAlterTableMove( smlStatement       * aStatement,
                             void               * aOrgRelHandle,
                             smlTbsId             aTbsId,
                             smlTableAttr       * aTableAttr,
                             knlValueBlockList  * aColumnList,
                             void              ** aNewRelHandle,
                             stlInt64           * aNewRelationId,
                             smlEnv             * aEnv );

stlStatus smlCheckTableExistRow( smlStatement * aStatement,
                                 void         * aRelationHandle,
                                 stlBool      * aExistRow,
                                 smlEnv       * aEnv );

stlStatus smlTruncateTable( smlStatement          * aStatement,
                            void                  * aOrgRelHandle,
                            smlDropStorageOption    aDropStorageOption,
                            void                 ** aNewRelHandle,
                            stlInt64              * aNewRelationId,
                            smlEnv                * aEnv );

stlStatus smlShrinkUndoSegments( smlEnv * aEnv );

stlStatus smlSetFlag( smlStatement      * aStatement,
                      void             ** aRelationHandle,
                      smlRowBlock       * aRids,
                      knlValueBlockList * aFlags,
                      smlEnv            * aEnv );

stlStatus smlGetTotalRecordCount( void      * aRelationHandle,
                                  stlInt64  * aRowCount,
                                  smlEnv    * aEnv );

stlStatus smlGetRelationHandle( stlInt64    aRelationId,
                                void     ** aRelationHandle,
                                smlEnv    * aEnv );

stlStatus smlValidateRelation( void     * aRelationHandle,
                               stlInt32   aValidateBehavior,
                               smlEnv   * aEnv );

stlBool smlIsValidSegmentId( stlInt64 aPhysicalId );

smlRelationType smlGetRelationType( void * aRelationHandle );

stlBool smlIsAlterableTable( void * aTableHandle );

stlInt64 smlSegmentMaxSize( smlTbsId  aTbsId );

/** @} */

#endif /* _SMLTABLE_H_ */
