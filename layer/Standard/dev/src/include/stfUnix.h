/*******************************************************************************
 * stfUnix.h
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


#ifndef _STFUNIX_H_
#define _STFUNIX_H_ 1

#if defined( STC_HAVE_FCNTL_H )
#include <fcntl.h>
#endif
#if defined( STC_HAVE_SYS_STAT_H )
#include <sys/stat.h>
#endif
#if defined( STC_HAVE_UNISTD_H )
#include <unistd.h>
#endif

#if STC_HAS_LARGE_FILES && defined(_LARGEFILE64_SOURCE)
#define stat(f,b) stat64(f,b)
#define lstat(f,b) lstat64(f,b)
#define fstat(f,b) fstat64(f,b)
#define lseek(f,o,w) lseek64(f,o,w)
#define ftruncate(f,l) ftruncate64(f,l)
typedef struct stat64 stfStat;
#else
typedef struct stat stfStat;
#endif

stlFileType stfFileTypeFromMode( mode_t aMode );

stlStatus stfFillInfo( stlFileInfo   * aFileInfo,
                       stfStat       * aStat,
                       stlInt32        aWanted,
                       stlErrorStack * aErrorStack );

mode_t stfUnixPerms2Mode( stlInt32 aPerms );

stlInt32 stfUnixMode2Perms( mode_t aMode );

#endif /* _STFUNIX_H_ */
