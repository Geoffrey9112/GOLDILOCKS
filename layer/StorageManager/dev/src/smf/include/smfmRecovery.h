/*******************************************************************************
 * smfmRecovery.h
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


#ifndef _SMFMRECOVERY_H_
#define _SMFMRECOVERY_H_ 1

/**
 * @file smfmRecovery.h
 * @brief Storage Manager Layer Memory Datafile Recovery Component Internal Routines
 */

/**
 * @defgroup smfmRecovery Memory Datafile Recovery
 * @ingroup smfInternal
 * @{
 */

stlStatus smfmWriteLogCreate( smxlTransId           aTransId,
                              stlUInt16             aRedoLogType,
                              stlUInt16             aUndoLogType,
                              stlInt32              aTbsPhysicalId,
                              stlInt32              aHintTbsId,
                              smfDatafilePersData * aDatafilePersData,
                              stlBool               aUndoLogging,
                              smlEnv              * aEnv );

stlStatus smfmCreateRedo( smxlTransId           aTransId,
                          stlInt32              aTbsPhysicalId,
                          stlInt32              aHintTbsId,
                          smfDatafilePersData * aDatafilePersData,
                          stlBool               aForRestore,
                          stlBool               aClrLogging,
                          smlEnv              * aEnv );

stlStatus smfmCreateUndo( smxlTransId           aTransId,
                          stlInt32              aTbsPhysicalId,
                          stlInt32              aHintTbsId,
                          smfDatafilePersData * aDatafilePersData,
                          stlBool               aForRestore,
                          stlBool               aClrLogging,
                          smlEnv              * aEnv );

stlStatus smfmDropRedo( smxlTransId           aTransId,
                        stlInt32              aTbsPhysicalId,
                        stlInt32              aHintTbsId,
                        smfDatafilePersData * aDatafilePersData,
                        stlBool               aForRestore,
                        stlBool               aClrLogging,
                        smlEnv              * aEnv );

stlStatus smfmDropUndo( smxlTransId           aTransId,
                        stlInt32              aTbsPhysicalId,
                        stlInt32              aHintTbsId,
                        smfDatafilePersData * aDatafilePersData,
                        stlBool               aForRestore,
                        stlBool               aClrLogging,
                        smlEnv              * aEnv );

/** @} */
    
#endif /* _SMFMRECOVERY_H_ */
