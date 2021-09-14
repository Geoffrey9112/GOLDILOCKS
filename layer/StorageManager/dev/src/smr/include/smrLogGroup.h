/*******************************************************************************
 * smrLogGroup.h
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


#ifndef _SMRLOGGROUP_H_
#define _SMRLOGGROUP_H_ 1

/**
 * @file smrLogGroup.h
 * @brief Storage Manager Layer Recovery Log Group Component Internal Routines
 */

/**
 * @defgroup smrLogGroup Recovery Log Group
 * @ingroup smrInternal
 * @{
 */

stlStatus smrAddLogGroup( smlLogStreamAttr * aAttr,
                          stlBool            aCreateDb,
                          stlInt64         * aMinLogFileSize,
                          smlEnv           * aEnv );

stlStatus smrDropLogGroup( smlLogStreamAttr * aAttr,
                           smlEnv           * aEnv );

stlStatus smrSwitchLogGroupAtMount( smlEnv * aEnv );
stlStatus smrSwitchLogGroupAtOpen( smlEnv * aEnv );

stlStatus smrFindLogGroupBySeqNo( smrLogStream  * aLogStream,
                                  stlInt64        aFileSeqNo,
                                  smrLogGroup  ** aLogGroup,
                                  smlEnv        * aEnv );
stlStatus smrFindLogGroupByLsn( smrLogStream  * aLogStream,
                                smrLsn          aLsn,
                                stlInt64      * aFileSeqNo,
                                smrLogGroup  ** aLogGroup );
stlStatus smrFindLogGroupById( smrLogStream  * aLogStream,
                               stlInt64        aLogGroupId,
                               smrLogGroup  ** aLogGroup,
                               smlEnv        * aEnv );
stlStatus smrFirstLogGroupPersData( void                 * aLogStreamIterator,
                                    smrLogGroupPersData  * aLogGroupPersData,
                                    void                ** aLogGroupIterator,
                                    smlEnv               * aEnv );
stlStatus smrNextLogGroupPersData( void                 * aLogStreamIterator,
                                   smrLogGroupPersData  * aLogGroupPersData,
                                   void                ** aLogGroupIterator,
                                   smlEnv               * aEnv );
stlInt32  smrGetLogGroupState( smrLogGroup * aLogGroup );

smrLogGroup * smrGetNextLogGroup( smrLogStream * aLogStream,
                                  smrLogGroup  * aLogGroup );
smrLogGroup * smrGetPrevLogGroup( smrLogStream * aLogStream,
                                  smrLogGroup  * aLogGroup );

/** @} */

#endif /* _SMRLOGGROUP_H_ */
