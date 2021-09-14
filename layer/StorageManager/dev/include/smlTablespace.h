/*******************************************************************************
 * smlTablespace.h
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


#ifndef _SMLTABLESPACE_H_
#define _SMLTABLESPACE_H_ 1

/**
 * @file smlTablespace.h
 * @brief Storage Manager Tablespace Routines
 */

/**
 * @defgroup smlTablespace Tablespace
 * @ingroup smExternal
 * @{
 */
stlStatus smlCreateTablespace( smlStatement * aStatement,
                               smlTbsAttr   * aAttr,
                               stlInt32       aReservedPageCount,
                               smlTbsId     * aTbsId,
                               smlEnv       * aEnv );

stlStatus smlDropTablespace( smlStatement * aStatement,
                             smlTbsId       aTbsId,
                             stlBool        aAndDatafiles,
                             smlEnv       * aEnv );

stlStatus smlOfflineTablespace( smlStatement       * aStatement,
                                smlTbsId             aTbsId,
                                smlOfflineBehavior   aOfflineBehavior,
                                smlEnv             * aEnv );

stlStatus smlOnlineTablespace( smlStatement       * aStatement,
                               smlTbsId             aTbsId,
                               stlInt64           * aPendingRelationId,
                               smlEnv             * aEnv );

stlStatus smlFinalizeOnlineTablespace( smlStatement * aStatement,
                                       smlTbsId       aTbsId,
                                       stlInt64       aPendingRelationId,
                                       smlEnv       * aEnv );

stlStatus smlExistDatafile( stlChar  * aFileName,
                            stlBool  * aExist,
                            smlEnv   * aEnv );

stlStatus smlIsUsedDatafile( stlChar   * aFileName,
                             smlTbsId  * aTbsId,
                             stlInt32  * aDatafileId,
                             stlBool   * aExist,
                             stlBool   * aAged,
                             smlEnv    * aEnv );

stlStatus smlIsUsedMemory( stlChar   * aFileName,
                           smlTbsId  * aTbsId,
                           stlInt32  * aDatafileId,
                           stlBool   * aExist,
                           smlEnv    * aEnv );

stlStatus smlConvAbsDatafilePath( stlChar  * aFileName,
                                  smlEnv   * aEnv );

stlStatus smlAddDatafile( smlStatement    * aStatement,
                          smlTbsId          aTbsId,
                          smlDatafileAttr * aAttr,
                          smlEnv          * aEnv );

stlStatus smlDropDatafile( smlStatement * aStatement,
                           smlTbsId       aTbsId,
                           stlChar      * aDatafileName,
                           smlEnv       * aEnv );

stlStatus smlRenameDatafiles( smlStatement    * aStatement,
                              smlTbsId          aTbsId,
                              smlDatafileAttr * aOrgDatafileAttr,
                              smlDatafileAttr * aNewDatafileAttr,
                              smlEnv          * aEnv );

stlStatus smlFetchFirstTbsInfo( void           ** aIterator,
                                smlTbsId        * aTbsId,
                                stlChar         * aTbsName,
                                stlUInt32       * aTbsAttr,
                                smlEnv          * aEnv );

stlStatus smlFetchNextTbsInfo( void           ** aIterator,
                               smlTbsId        * aTbsId,
                               stlChar         * aTbsName,
                               stlUInt32       * aTbsAttr,
                               smlEnv          * aEnv );

stlStatus smlValidateTablespace( smlTbsId   aTbsId,
                                 stlInt32   aValidateBehavior,
                                 smlEnv   * aEnv );

stlBool smlIsOnlineTablespace( smlTbsId aTbsId );

stlBool smlIsLoggingTablespace( smlTbsId aTbsId );

stlStatus smlRenameTablespace( smlStatement * aStatement,
                               smlTbsId       aTbsId,
                               stlChar      * aNewTablespaceName,
                               smlEnv       * aEnv );

stlStatus smlExistTablespace( stlInt64    aTbsId,
                              void      * aEnv );

stlInt64 smlGetTablespaceExtentSize( smlTbsId aTbsId );

stlStatus smlValidateTbs( smlEnv * aEnv );

stlStatus smlValidateOfflineTablespace( smlOfflineBehavior   aOfflineBehavior,
                                        smlEnv             * aEnv );

stlStatus smlDropUnregisteredTablespace( smlTbsId   aTbsId,
                                         smlEnv   * aEnv );
/** @} */

#endif /* _SMLTABLESPACE_H_ */
