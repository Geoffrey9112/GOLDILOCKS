/*******************************************************************************
 * smfRecovery.h
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


#ifndef _SMFRECOVERY_H_
#define _SMFRECOVERY_H_ 1

/**
 * @file smfRecovery.h
 * @brief Storage Manager Layer Memory Tablespace Recovery Component Internal Routines
 */

/**
 * @defgroup smfRecovery Memory Tablespace Recovery
 * @ingroup smfInternal
 * @{
 */

stlStatus smfWriteLogTbs( smxlTransId      aTransId,
                          stlUInt16        aRedoLogType,
                          stlUInt16        aUndoLogType,
                          smfTbsPersData * aBeforeTbsPersData,
                          smfTbsPersData * aAfterTbsPersData,
                          stlBool          aUndoLogging,
                          smlEnv         * aEnv );

stlStatus smfWriteLogDatafile( smxlTransId           aTransId,
                               smlTbsId              aTbsId,
                               stlUInt16             aRedoLogType,
                               stlUInt16             aUndoLogType,
                               smfDatafilePersData * aBeforeDatafilePersData,
                               smfDatafilePersData * aAfterDatafilePersData,
                               stlBool               aUndoLogging,
                               smlEnv              * aEnv );

stlStatus smfWriteLogRenameTbs( smxlTransId   aTransId,
                                smlTbsId      aTbsId,
                                stlUInt16     aRedoLogType,
                                stlUInt16     aUndoLogType,
                                stlChar     * aBeforeName,
                                stlChar     * aAfterName,
                                stlBool       aUndoLogging,
                                smlEnv      * aEnv );

stlStatus smfCreateTbsRedo( smfTbsPersData * aTbsPersData,
                            smlEnv         * aEnv );

stlStatus smfCreateTbsUndo( smxlTransId      aTransId,
                            stlInt32         aTbsTypeId,
                            smfTbsPersData * aTbsPersData,
                            stlBool          aClrLogging,
                            smlEnv         * aEnv );

stlStatus smfDropTbsRedo( smfTbsPersData * aTbsPersData,
                          smlEnv         * aEnv );

stlStatus smfDropTbsUndo( smxlTransId      aTransId,
                          smfTbsPersData * aTbsPersData,
                          stlBool          aClrLogging,
                          smlEnv         * aEnv );

stlStatus smfAddDatafileRedo( smlTbsId              aTbsId,
                              smfDatafilePersData * aDatafilePersData,
                              smlEnv              * aEnv );

stlStatus smfAddDatafileUndo( smxlTransId           aTransId,
                              stlInt32              aTbsTypeId,
                              smlTbsId              aTbsId,
                              smfDatafilePersData * aDatafilePersData,
                              stlBool               aClrLogging,
                              smlEnv              * aEnv );

stlStatus smfDropDatafileRedo( smlTbsId              aTbsId,
                               smfDatafilePersData * aDatafilePersData,
                               smlEnv              * aEnv );

stlStatus smfDropDatafileUndo( smxlTransId           aTransId,
                               smlTbsId              aTbsId,
                               smfDatafilePersData * aDatafilePersData,
                               stlBool               aClrLogging,
                               smlEnv              * aEnv );

stlStatus smfRenameTbsRedo( smlTbsId     aTbsId,
                            stlChar    * aTbsName,
                            stlInt32     aNameLen,
                            smlEnv     * aEnv );

stlStatus smfRenameTbsUndo( smxlTransId   aTransId,
                            smlTbsId      aTbsId,
                            stlChar     * aTbsName,
                            stlInt32      aNameLen,
                            stlBool       aClrLogging,
                            smlEnv      * aEnv );

/** @} */
    
#endif /* _SMFRECOVERY_H_ */
