/*******************************************************************************
 * ellDictProfile.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: ellDictProfile.h 15300 2015-06-29 02:41:26Z kja $
 *
 * NOTES
 *
 *
 ******************************************************************************/


#ifndef _ELL_DICT_PROFILE_H_
#define _ELL_DICT_PROFILE_H_ 1

/**
 * @file ellDictProfile.h
 * @brief Profile Dictionary Management
 */

/**
 * @addtogroup ellObjectProfile
 * @{
 */


/*
 * DDL 관련 Object 획득 함수
 */

stlStatus ellGetUserList4DropProfile( smlTransId       aTransID,
                                      smlStatement   * aStmt,
                                      ellDictHandle  * aProfileHandle,
                                      knlRegionMem   * aRegionMem,
                                      ellObjectList ** aUserList,
                                      ellEnv         * aEnv );

stlStatus ellGetUserList4AlterProfile( smlTransId       aTransID,
                                       smlStatement   * aStmt,
                                       ellDictHandle  * aProfileHandle,
                                       knlRegionMem   * aRegionMem,
                                       ellObjectList ** aUserList,
                                       ellEnv         * aEnv );

/*
 * DDL Lock 함수 
 */

stlStatus ellLock4DropProfile( smlTransId      aTransID,
                               smlStatement  * aStmt,
                               ellDictHandle * aProfileHandle,
                               stlBool       * aLockSuccess,
                               ellEnv        * aEnv );

stlStatus ellLock4AlterProfile( smlTransId      aTransID,
                                smlStatement  * aStmt,
                                ellDictHandle * aProfileHandle,
                                stlBool       * aLockSuccess,
                                ellEnv        * aEnv );

/*
 * Dictionary 변경 함수 
 */

stlStatus ellInsertProfileDesc( smlTransId             aTransID,
                                smlStatement         * aStmt,
                                stlInt64             * aProfileID,
                                stlChar              * aProfileName,
                                stlChar              * aProfileComment,
                                ellEnv               * aEnv );

stlStatus ellRemoveDict4DropProfile( smlTransId        aTransID,
                                     smlStatement    * aStmt,
                                     ellDictHandle   * aProfileHandle,
                                     ellEnv          * aEnv );

stlStatus ellInsertProfileParamInteger( smlTransId             aTransID,
                                        smlStatement         * aStmt,
                                        stlInt64               aProfileID,
                                        ellProfileParam        aProfileParam,
                                        stlBool                aIsDefault,
                                        stlInt64               aLimitIntValue,
                                        stlChar              * aLimitString,
                                        ellEnv               * aEnv );

stlStatus ellInsertProfileParamIntervalNumber( smlTransId             aTransID,
                                               smlStatement         * aStmt,
                                               stlInt64               aProfileID,
                                               ellProfileParam        aProfileParam,
                                               stlBool                aIsDefault,
                                               dtlDataValue         * aLimitValue,
                                               stlChar              * aLimitString,
                                               ellEnv               * aEnv );

stlStatus ellInsertProfileParamVerifyFunction( smlTransId             aTransID,
                                               smlStatement         * aStmt,
                                               stlInt64               aProfileID,
                                               stlBool                aIsDefault,
                                               stlInt64               aFunctionID,
                                               stlChar              * aFunctionString,
                                               ellEnv               * aEnv );

stlStatus ellModifyProfileParamInteger( smlTransId             aTransID,
                                        smlStatement         * aStmt,
                                        ellDictHandle        * aProfileHandle,
                                        ellProfileParam        aProfileParam,
                                        stlBool                aIsDefault,
                                        stlInt64               aLimitIntValue,
                                        stlChar              * aLimitString,
                                        ellEnv               * aEnv );


stlStatus ellModifyProfileParamIntervalNumber( smlTransId             aTransID,
                                               smlStatement         * aStmt,
                                               ellDictHandle        * aProfileHandle,
                                               ellProfileParam        aProfileParam,
                                               stlBool                aIsDefault,
                                               dtlDataValue         * aLimitValue,
                                               stlChar              * aLimitString,
                                               ellEnv               * aEnv );


stlStatus ellModifyProfileParamVerifyFunction( smlTransId             aTransID,
                                               smlStatement         * aStmt,
                                               ellDictHandle        * aProfileHandle,
                                               stlBool                aIsDefault,
                                               stlInt64               aFunctionID,
                                               stlChar              * aFunctionString,
                                               ellEnv               * aEnv );


/*
 * Cache 재구축 함수
 */

stlStatus ellRefineCache4CreateProfile( smlTransId           aTransID,
                                        smlStatement       * aStmt,
                                        stlInt64             aProfileID,
                                        ellEnv             * aEnv );

stlStatus ellRefineCache4DropProfile( smlTransId        aTransID,
                                      ellDictHandle   * aProfileHandle,
                                      ellEnv          * aEnv );

stlStatus ellRebuildProfileCache( smlTransId        aTransID,
                                  smlStatement    * aStmt,
                                  ellDictHandle   * aProfileHandle,
                                  ellEnv          * aEnv );


/*
 * Handle 획득 함수 
 */


stlStatus ellGetProfileDictHandleByName( smlTransId           aTransID,
                                         smlScn               aViewSCN,
                                         stlChar            * aProfileName,
                                         ellDictHandle      * aProfileHandle,
                                         stlBool            * aExist,
                                         ellEnv             * aEnv );

stlStatus ellGetProfileDictHandleByID( smlTransId           aTransID,
                                       smlScn               aViewSCN,
                                       stlInt64             aProfileID,
                                       ellDictHandle      * aProfileHandle,
                                       stlBool            * aExist,
                                       ellEnv             * aEnv );

/*
 * 정보 획득 함수
 */


stlInt64    ellGetProfileID( ellDictHandle * aProfileHandle );
stlInt64    ellGetDefaultProfileID();
stlChar   * ellGetProfileName( ellDictHandle * aProfileHandle );

stlBool     ellIsDefaultPassParam( ellDictHandle * aProfileHandle,
                                   ellProfileParam aPassParam );

stlInt64    ellGetProfileParamInteger( ellDictHandle * aProfileHandle,
                                       ellProfileParam aPassParam );

dtlDataValue * ellGetProfileParamNumberInterval( ellDictHandle * aProfileHandle,
                                                 ellProfileParam aPassParam );

stlInt64    ellGetProfileParamVerifyFunction( ellDictHandle * aProfileHandle );

stlStatus ellGetUserProfileParam( smlTransId            aTransID,
                                  smlStatement        * aStmt,
                                  ellDictHandle       * aProfileHandle,
                                  ellUserProfileParam * aProfileParam,
                                  ellEnv              * aEnv );

/** @} ellObjectAuth */



#endif /* _ELL_DICT_PROFILE_H_ */
