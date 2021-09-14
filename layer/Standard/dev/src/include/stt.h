/*******************************************************************************
 * stt.h
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


#ifndef _STT_H_
#define _STT_H_ 1

STL_BEGIN_DECLS

stlStatus sttPutAnsiTime( stlTime     * aResult,
                          stlAnsiTime   aInput );

stlTime sttNow( void );

void sttExplode( stlExpTime * aExpTime,
                 stlTime      aTime,
                 stlInt32     aUseLocaltime );

void sttSleep( stlInterval aInterval );

stlStatus sttCreateTimer( stlInt32        aSigNo,
                          stlInt64        aNanoSecond,
                          stlTimer      * aTimerId,
                          stlErrorStack * aErrorStack );

stlStatus sttDestroyTimer( stlTimer        aTimerId,
                           stlErrorStack * aErrorStack );

#ifdef WIN32

void sttFileTimeToStlTime(FILETIME * aFileTime, stlTime * aStlTime);
void sttStlTimeToFileTime(stlTime aStlTime, FILETIME * aFileTime);
void sttSystemTileToStlExpTime(SYSTEMTIME * aSystemTime, stlExpTime * aExpTime);

#endif

STL_END_DECLS

#endif /* _STT_H_ */
