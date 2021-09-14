/*******************************************************************************
 * smsManager.h
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


#ifndef _SMSMANAGER_H_
#define _SMSMANAGER_H_ 1

/**
 * @file smsManager.h
 * @brief Storage Manager Layer Segment Manager Component Internal Routines
 */

/**
 * @defgroup smsManager Segment Manager
 * @ingroup smsInternal
 * @{
 */

void smsSetSegType( void       * aSegmentHandle,
                    smlSegType   aSegType );
void smsSetSegRid( void   * aSegmentHandle,
                   smlRid   aSegRid );
void smsSetSegmentId( void   * aSegmentHandle,
                      smlRid   aSegRid );
void smsSetTbsId( void     * aSegmentHandle,
                  smlTbsId   aTbsId );
void smsSetHasRelHdr( void    * aSegmentHandle,
                      stlBool   aHasRelHdr );
void smsSetValidScn( void    * aSegmentHandle,
                     smlScn    aValidScn );
void smsSetValidSeq( void      * aSegmentHandle,
                     stlInt64    aValidSeq );
smsHint * smsFindSegHint( void   * aCache,
                          smlScn   aValidScn,
                          smlEnv * aEnv );
void smsAddNewSegHint( smlPid     aHintPid,
                       void     * aCache,
                       smlScn     aValidScn,
                       smlEnv   * aEnv );
stlStatus smsInitSegHdr( smxmTrans  * aMiniTrans,
                         smpHandle  * aPageHandle,
                         smlSegType   aSegType,
                         smlTbsId     aTbsId,
                         stlBool      aHasRelHdr,
                         smsSegHdr  * aSegHdr,
                         smlEnv     * aEnv );

inline stlUInt64 smsGetPageCountInTempTbsExt( smlTbsId aTbsId );

/** @} */
    
#endif /* _SMSMANAGER_H_ */
