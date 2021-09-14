/*******************************************************************************
 * stfStatUnix.c
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

#include <stc.h>
#include <stlDef.h>
#include <stfUnix.h>
#include <stt.h>
#include <ste.h>
#include <stlError.h>
#include <stlStrings.h>
#include <stlLog.h>

#if defined( STC_HAVE_SYS_PARAM_H )
#include <sys/param.h>
#endif

stlFileType stfFileTypeFromMode( mode_t aMode )
{
    stlFileType sFileType;

    switch( aMode & S_IFMT )
    {
        case S_IFREG:
            sFileType = STL_FTYPE_REG;
            break;
        case S_IFDIR:
            sFileType = STL_FTYPE_DIR;
            break;
        case S_IFLNK:
            sFileType = STL_FTYPE_LNK;
            break;
        case S_IFCHR:
            sFileType = STL_FTYPE_CHR;
            break;
        case S_IFBLK:
            sFileType = STL_FTYPE_BLK;
            break;
#if defined(S_IFFIFO)
        case S_IFFIFO:
            sFileType = STL_FTYPE_PIPE;
            break;
#endif
#if !defined(BEOS) && defined(S_IFSOCK)
        case S_IFSOCK:
            sFileType = STL_FTYPE_SOCK;
            break;
#endif

        default:
            /*
             * Work around missing S_IFxxx values above
             * for Linux et al.
             */
#if !defined(S_IFFIFO) && defined(S_ISFIFO)
            if( S_ISFIFO( aMode ) )
            {
                sFileType = STL_FTYPE_PIPE;
            }
            else
#endif
#if !defined(BEOS) && !defined(S_IFSOCK) && defined(S_ISSOCK)
                if( S_ISSOCK( aMode ) )
                {
                    sFileType = STL_FTYPE_SOCK;
                }
                else
#endif
                {
                    sFileType = STL_FTYPE_UNKFILE;
                }
    }
        
    return sFileType;
}

stlStatus stfFillInfo( stlFileInfo   * aFileInfo,
                       stfStat       * aStat,
                       stlInt32        aWanted,
                       stlErrorStack * aErrorStack )
{ 
    STL_PARAM_VALIDATE( aFileInfo != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aStat != NULL, aErrorStack );
    
    aFileInfo->mValid = STL_FINFO_MIN | STL_FINFO_IDENT |
        STL_FINFO_NLINK | STL_FINFO_OWNER | STL_FINFO_PROT;
    aFileInfo->mProtection = stfUnixMode2Perms( aStat->st_mode );
    aFileInfo->mFileType = stfFileTypeFromMode( aStat->st_mode );
    aFileInfo->mUserID = aStat->st_uid;
    aFileInfo->mGroupID = aStat->st_gid;
    aFileInfo->mSize = aStat->st_size;
    aFileInfo->mDeviceID = aStat->st_dev;
    aFileInfo->mLinks = aStat->st_nlink;

    /* Check for overflow if storing a 64-bit st_ino in a 32-bit
     * apr_ino_t for LFS builds: */
    if( ( sizeof(stlInode) >= sizeof(aStat->st_ino) ) ||
        ( (stlInode)aStat->st_ino == aStat->st_ino) )
    {
        aFileInfo->mInodeNumber = aStat->st_ino;
    }
    else
    {
        aFileInfo->mValid &= ~STL_FINFO_INODE;
    }

    sttPutAnsiTime( &aFileInfo->mLastAccessedTime, aStat->st_atime );
    sttPutAnsiTime( &aFileInfo->mLastModifiedTime, aStat->st_mtime );
    sttPutAnsiTime( &aFileInfo->mCreateTime, aStat->st_ctime );

#if defined( STC_HAVE_STRUCT_STAT_ST_BLOCKS )
#if defined( DEV_BSIZE )
    aFileInfo->mAllocatedSize = (off_t)aStat->st_blocks * (off_t)DEV_BSIZE;
#else
    aFileInfo->mAllocatedSize = (off_t)aStat->st_blocks * (off_t)512;
#endif
    aFileInfo->mValid |= STL_FINFO_ASIZE;
#endif

    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus stfGetStat( stlFileInfo   * aFileInfo,
                      const stlChar * aFileName,
                      stlInt32        aWanted,
                      stlErrorStack * aErrorStack )
{
    stfStat   sInfo;
    stlInt32  sStatus;

    STL_PARAM_VALIDATE( aFileInfo != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aFileName != NULL, aErrorStack );
    
    if( aWanted & STL_FINFO_LINK )
    {
        sStatus = lstat( aFileName, &sInfo );
    }
    else
    {
        sStatus = stat( aFileName, &sInfo );
    }

    STL_TRY_THROW( sStatus == 0, RAMP_ERR );
    
    STL_TRY( stfFillInfo( aFileInfo,
                          &sInfo,
                          aWanted,
                          aErrorStack ) == STL_SUCCESS );
        
    if( aWanted & STL_FINFO_LINK)
    {
        aWanted &= ~STL_FINFO_LINK;
    }

    STL_TRY_THROW( !(aWanted & ~aFileInfo->mValid), RAMP_ERR_INCOMPLETE );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INCOMPLETE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_INCOMPLETE,
                      NULL,
                      aErrorStack );
    }
    
    STL_CATCH( RAMP_ERR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_FILE_STAT,
                      NULL,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus stfGetFileInfo( stlFileInfo   * aFileInfo,
                          stlInt32        aWanted,
                          stlFile       * aFile,
                          stlErrorStack * aErrorStack )
{
    stfStat sInfo;

    STL_PARAM_VALIDATE( aFileInfo != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aFile != NULL, aErrorStack );
    
    STL_TRY_THROW( fstat( aFile->mFileDesc, &sInfo ) == 0, RAMP_ERR );

    STL_TRY( stfFillInfo( aFileInfo,
                          &sInfo,
                          aWanted,
                          aErrorStack ) == STL_SUCCESS );
        
    STL_TRY_THROW( !(aWanted & ~aFileInfo->mValid), RAMP_ERR_INCOMPLETE );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INCOMPLETE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_INCOMPLETE,
                      NULL,
                      aErrorStack );
    }
    
    STL_CATCH( RAMP_ERR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_FILE_STAT,
                      NULL,
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus stfGetCurFilePath( stlChar       * aPath,
                             stlInt32        aPathBufLen,
                             stlErrorStack * aErrorStack )
{
    stlChar sPath[STL_MAX_FILE_PATH_LENGTH * 2];

    STL_PARAM_VALIDATE( aPath != NULL, aErrorStack );

    STL_TRY_THROW( getcwd( sPath, STL_MAX_FILE_PATH_LENGTH * 2 ) != NULL, RAMP_ERR );

    STL_TRY_THROW( stlStrlen( sPath ) < aPathBufLen, RAMP_ERR_NAMETOOLONG );

    stlStrncpy( aPath, sPath, aPathBufLen );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NAMETOOLONG )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_NAMETOOLONG,
                      NULL,
                      aErrorStack,
                      sPath );
    }
    STL_CATCH( RAMP_ERR )
    {
        switch( errno )
        {
            case EACCES:
            case EPERM:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_ACCESS,
                              NULL,
                              aErrorStack,
                              aPath );
                break;
            case ENOENT:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_NO_ENTRY,
                              NULL,
                              aErrorStack,
                              aPath );
                break;
            default:
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              STL_ERRCODE_FILE_PATH,
                              NULL,
                              aErrorStack );
                break;
        }
        steSetSystemError( errno, aErrorStack );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}

