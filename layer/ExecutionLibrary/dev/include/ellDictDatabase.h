/*******************************************************************************
 * ellDictDatabase.h
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


#ifndef _ELL_DICT_DATABASE_H_
#define _ELL_DICT_DATABASE_H_ 1

/**
 * @file ellDictDatabase.h
 * @brief Database Dictionary Management
 */

/**
 * @addtogroup ellObjectDatabase
 * @{
 */

/*
 * 정보 구축 함수
 */

stlStatus ellBuildDatabaseDictionary( smlTransId            aTransID,
                                      smlStatement        * aStmt,
                                      stlChar             * aDatabaseName,
                                      stlChar             * aDatabaseComment,
                                      stlInt64              aFirstDictPhysicalID,
                                      void                * aFirstDictPhysicalHandle,
                                      smlTbsId              aSystemTbsID,
                                      smlTbsId              aDefaultUserTbsID,
                                      stlChar             * aDefaultUserTbsName,
                                      smlTbsId              aDefaultUndoTbsID,
                                      stlChar             * aDefaultUndoTbsName,
                                      smlTbsId              aDefaultTempTbsID,
                                      stlChar             * aDefaultTempTbsName,
                                      dtlCharacterSet       aCharacterSetID,
                                      dtlStringLengthUnit   aCharLengthUnit,
                                      stlChar             * aEncryptedSysPassword,
                                      stlChar             * aTimeZoneIntervalString,
                                      ellEnv              * aEnv );

stlStatus ellFinishBuildDictionary( ellEnv * aEnv );

stlStatus ellBuildFixedCacheAndCompleteCache( knlStartupPhase   aStartPhase,
                                              ellEnv          * aEnv );

stlStatus ellSetDictionaryDumpHandle( ellEnv * aEnv );

/*
 * 정보 변경 함수
 */

stlStatus ellLock4SetObjectComment( smlTransId              aTransID,
                                    smlStatement          * aStmt,
                                    ellDictHandle         * aObjectHandle,
                                    ellDictHandle         * aRelatedTableHandle,
                                    stlBool               * aLockSuccess,
                                    ellEnv                * aEnv );

stlStatus ellSetObjectComment( smlTransId              aTransID,
                               smlStatement          * aStmt,
                               ellDictHandle         * aObjectHandle,
                               stlChar               * aCommentString,
                               ellEnv                * aEnv );

/*
 * lock 획득 함수
 */

stlStatus ellLogicalObjectLock( smlTransId      aTransID,
                                smlStatement  * aStmt,
                                ellDictHandle * aObjectHandle,
                                stlInt32        aLockMode,
                                stlBool       * aLockSuccess,
                                ellEnv        * aEnv );

stlStatus ellLockRowAfterCreateNonBaseTableObject( smlTransId       aTransID,
                                                   smlStatement   * aStmt,
                                                   ellObjectType    aObjectType,
                                                   stlInt64         aObjectID,
                                                   ellEnv         * aEnv );

/*
 * Handle 획득 함수 
 */

stlStatus ellGetCatalogDictHandle( smlTransId           aTransID,
                                   ellDictHandle      * aCatalogDictHandle,
                                   ellEnv             * aEnv );

/*
 * 정보 획득 함수
 */

stlInt64                   ellGetDbCatalogID();
stlChar                  * ellGetDbCatalogName();
dtlCharacterSet            ellGetDbCharacterSet();
dtlStringLengthUnit        ellGetDbCharLengthUnit();
dtlIntervalDayToSecondType ellGetDbTimezone();

stlStatus ellGetDbDataSpaceHandle( smlTransId      aTransID,
                                   smlStatement  * aStmt,
                                   ellDictHandle * aSpaceHandle,
                                   knlRegionMem  * aRegionMem,
                                   ellEnv        * aEnv );

stlStatus ellGetDbTempSpaceHandle( smlTransId      aTransID,
                                   smlStatement  * aStmt,
                                   ellDictHandle * aSpaceHandle,
                                   knlRegionMem  * aRegionMem,
                                   ellEnv        * aEnv );



/** @} ellObjectDatabase */



#endif /* _ELL_DICT_DATABASE_H_ */
