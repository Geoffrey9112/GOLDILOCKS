/*******************************************************************************
 * smrLogMember.h
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


#ifndef _SMRLOGMEMBER_H_
#define _SMRLOGMEMBER_H_ 1

/**
 * @file smrLogMember.h
 * @brief Storage Manager Layer Recovery Log Member Component Internal Routines
 */

/**
 * @defgroup smrLogMember Recovery Log Member
 * @ingroup smrInternal
 * @{
 */

stlStatus smrAddLogMember( smlLogGroupAttr  * aGroupAttr,
                           stlInt32           aMemberCount,
                           smlEnv           * aEnv );

stlStatus smrDropLogMember( smlLogStreamAttr * aStreamAttr,
                            smlEnv           * aEnv );

stlStatus smrFirstLogMemberPersData( void                 * aLogGroupIterator,
                                     smrLogMemberPersData * aLogMemberPersData,
                                     void                ** aLogMemberIterator,
                                     smlEnv               * aEnv );
stlStatus smrNextLogMemberPersData( void                 * aLogGroupIterator,
                                    smrLogMemberPersData * aLogMemberPersData,
                                    void                ** aLogMemberIterator,
                                    smlEnv               * aEnv );
stlStatus smrCreateLogMember( smlLogMemberAttr  * aAttr,
                              stlInt64            aFileSize,
                              stlInt16            aBlockSize,
                              stlBool             aIsReuseFile,
                              smrLogMember     ** aLogMember,
                              smlEnv            * aEnv );
stlStatus smrDestroyLogMember( smrLogMember * aLogMember,
                               smlEnv       * aEnv );

stlStatus smrRenameLogFileAtMount( stlInt32           aFileCount,
                                   smlLogMemberAttr * aFromList,
                                   smlLogMemberAttr * aToList,
                                   smlEnv           * aEnv );

/** @} */

#endif /* _SMRLOGMEMBER_H_ */
