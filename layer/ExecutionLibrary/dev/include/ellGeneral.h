/*******************************************************************************
 * ellGeneral.h
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


#ifndef _ELLGENERAL_H_
#define _ELLGENERAL_H_ 1

/**
 * @file ellGeneral.h
 * @brief Execution Library General Routines
 */

extern knlStartupFunc     gStartupEL[KNL_STARTUP_PHASE_MAX];
extern knlWarmupFunc      gWarmupEL;
extern knlShutdownFunc    gShutdownEL[KNL_SHUTDOWN_PHASE_MAX];
extern knlCooldownFunc    gCooldownEL;
extern knlCleanupHeapFunc gCleanupHeapEL;


stlStatus ellInitialize();
stlStatus ellInitializeEnv( ellEnv      * aEnv,
                            knlEnvType    aEnvType );
stlStatus ellFinalizeEnv( ellEnv * aEnv );

stlStatus ellInitSessionEnv( ellSessionEnv     * aSessEnv,
                             knlConnectionType   aConnectionType,
                             knlSessionType      aSessType,
                             knlSessEnvType      aSessEnvType,
                             ellEnv            * aEnv );

stlStatus ellFiniSessionEnv( ellSessionEnv * aSessEnv,
                             ellEnv        * aEnv );

stlStatus ellCleanupSessionEnv( ellSessionEnv * aSessEnv,
                                ellEnv        * aEnv );

stlStatus ellValidateSessionAuth( smlTransId   aTransID,
                                  smlScn       aViewSCN,
                                  stlBool    * aModified,
                                  ellEnv     * aEnv );

void ellSetLoginUser( ellDictHandle * aAuthHandle, ellEnv * aEnv );
void ellSetSessionUser( ellDictHandle * aAuthHandle, ellEnv * aEnv );
void ellSetCurrentUser( ellDictHandle * aAuthHandle, ellEnv * aEnv );
void ellRollbackCurrentUser( ellEnv * aEnv );
void ellSetSessionRole( ellDictHandle * aAuthHandle, ellEnv * aEnv );

ellDictHandle * ellGetCurrentUserHandle( ellEnv * aEnv );
ellDictHandle * ellGetSessionUserHandle( ellEnv * aEnv );
ellDictHandle * ellGetLoginUserHandle( ellEnv * aEnv );

stlInt64 ellGetSessionAuthID( ellEnv * aEnv );

ellDictHandle * ellGetSessionRoleHandle( ellEnv * aEnv );

stlStatus ellGetCurrentTempSpaceHandle( smlTransId       aTransID,
                                        smlScn           aViewSCN,
                                        ellDictHandle  * aSpaceHandle,
                                        ellEnv         * aEnv );

dtlDataValue * ellGetNullNumber();

#endif /* _ELLGENERAL_H_ */
