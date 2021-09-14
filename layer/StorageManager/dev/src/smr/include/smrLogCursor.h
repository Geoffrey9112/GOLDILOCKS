/*******************************************************************************
 * smrLogCursor.h
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


#ifndef _SMRLOGCURSOR_H_
#define _SMRLOGCURSOR_H_ 1

/**
 * @file smrLogCursor.h
 * @brief Storage Manager Layer Recovery Log Cursor Component Internal Routines
 */

/**
 * @defgroup smrLogCursor Recovery Log Cursor
 * @ingroup smrInternal
 * @{
 */

stlStatus smrOpenLogCursor( smrLsn         sOldestLsn,
                            smrLogCursor * aLogCursor,
                            stlBool        aIsForRecover,
                            smlEnv       * aEnv );
stlStatus smrCloseLogCursor( smrLogCursor * aLogCursor,
                             smlEnv       * aEnv );
stlStatus smrReadNextLog( smrLogCursor * aCursor,
                          stlBool      * aEndOfLog,
                          smlEnv       * aEnv );
inline smrLogHdr * smrGetCurLogHdr( smrLogCursor * aLogCursor );
inline stlChar * smrGetCurLogBody( smrLogCursor * aLogCursor );

stlStatus smrReadLog( smrLogCursor * aLogCursor,
                      stlBool      * aEndOfLog,
                      smlEnv       * aEnv );

/** @} */

#endif /* _SMRLOGCURSOR_H_ */
