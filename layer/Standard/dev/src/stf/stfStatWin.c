/*******************************************************************************
 * stfStatWin.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: stfStatUnix.c 13496 2014-08-29 05:38:43Z leekmo $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#include <stc.h>
#include <stlDef.h>
#include <stt.h>
#include <ste.h>
#include <stlError.h>
#include <stlStrings.h>
#include <stlLog.h>
#include <stt.h>

#include <AclAPI.h>
#include <wdmguid.h>

#define STF_FREADONLY 0x10000000

#define STF_MORE_OF_HANDLE 0
#define STF_MORE_OF_FSPEC  1

typedef enum
{
    STF_PROT_SCOPE_WORLD = 0,
    STF_PROT_SCOPE_GROUP = 4,
    STF_PROT_SCOPE_USER  = 8
} stfProtScope;

stlGroupID gStfWorldID = NULL;

void stfFreeWorldID()
{
    if( gStfWorldID != NULL )
    {
        FreeSid( gStfWorldID );
        gStfWorldID = NULL;
    }
}

stlStatus stfFillInfo( stlFileInfo               * aFileInfo,
                       WIN32_FILE_ATTRIBUTE_DATA * aWinInfo,
                       stlInt32                    aByHandle,
                       stlInt32                    aWanted,
                       stlBool                   * aWarn,
                       stlErrorStack             * aErrorStack )
{ 
    DWORD * sSize;

    STL_PARAM_VALIDATE( aFileInfo != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aWinInfo != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aWarn != NULL, aErrorStack );

    *aWarn= STL_FALSE;
    sSize = &aWinInfo->nFileSizeHigh + aByHandle;

    sttFileTimeToStlTime( &aWinInfo->ftLastAccessTime, &aFileInfo->mLastAccessedTime );
    sttFileTimeToStlTime( &aWinInfo->ftCreationTime, &aFileInfo->mCreateTime );
    sttFileTimeToStlTime( &aWinInfo->ftLastWriteTime, &aFileInfo->mLastModifiedTime );

    aFileInfo->mSize = (stlOffset)sSize[1] | ((stlOffset)sSize[0] << 32);

    if((aWanted & STL_FINFO_LINK) &&
       (aWinInfo->dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT))
    {
        aFileInfo->mFileType = STL_FTYPE_LNK;
    }
    else if (aWinInfo->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    {
        aFileInfo->mFileType = STL_FTYPE_DIR;
    }
    else if (aWinInfo->dwFileAttributes & FILE_ATTRIBUTE_DEVICE)
    {
        aFileInfo->mFileType = STL_FTYPE_CHR;
    }
    else
    {
        if ((aWinInfo->ftLastWriteTime.dwLowDateTime == 0) &&
            (aWinInfo->ftLastWriteTime.dwHighDateTime == 0) &&
            (aFileInfo->mSize == 0))
        {
            *aWarn = STL_TRUE;
        }
        aFileInfo->mFileType = STL_FTYPE_REG;
    }

    if (aWinInfo->dwFileAttributes & FILE_ATTRIBUTE_READONLY)
    {
        aFileInfo->mProtection = STF_FREADONLY;
    }

    aFileInfo->mValid = STL_FINFO_ATIME | STL_FINFO_CTIME | STL_FINFO_MTIME | STL_FINFO_SIZE | STL_FINFO_TYPE;

    if ((aByHandle == STL_FALSE) ||
        (aWanted & STL_FINFO_LINK))
    {
        aFileInfo->mValid |= STL_FINFO_LINK;
    }
	
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

#if 0

stlFilePerms stfConvertProt(ACCESS_MASK aAcc, stfProtScope aScope)
{
    stlFilePerms sProt = 0;

    if( aAcc & FILE_EXECUTE )
    {
        sProt |= STL_FPROT_WEXECUTE;
    }
    if( aAcc & FILE_WRITE_DATA )
    {
        sProt |= STL_FPROT_WWRITE;
    }
    if( aAcc & FILE_READ_DATA )
    {
        sProt |= STL_FPROT_WREAD;
    }

    return (sProt << aScope);
}

stlStatus stfResolveProt( stlFileInfo   * aFileInfo,
                          stlInt32        aWanted,
                          PACL            aDacl,
                          stlErrorStack * aErrorStack)
{
    TRUSTEE_W                sIdent   = { NULL, NO_MULTIPLE_TRUSTEE, TRUSTEE_IS_SID };
    SID_IDENTIFIER_AUTHORITY sSIDAuth = { SECURITY_WORLD_SID_AUTHORITY };
    ACCESS_MASK              sAcc;
	
    if( (aWanted & STL_FINFO_WPROT) && (gStfWorldID = NULL) )
    {
        if( AllocateAndInitializeSid( &sSIDAuth, 1, SECURITY_WORLD_RID, 0, 0, 0, 0, 0, 0, 0, &gStfWorldID ) )
        {
            atexit(stfFreeWorldID);
        }
        else
        {
            gStfWorldID = NULL;
        }
    }

    if( (aWanted & STL_FINFO_UPROT) && (aFileInfo->mValid & STL_FINFO_USER) )
    {
        sIdent.TrusteeType = TRUSTEE_IS_USER;
        sIdent.ptstrName = aFileInfo->mUserID;

        if( GetEffectiveRightsFromAcl( aDacl, &sIdent, &sAcc ) == ERROR_SUCCESS )
        {
            aFileInfo->mProtection |= stfConvertProt(sAcc, STF_PROT_SCOPE_USER);
            aFileInfo->mValid |= STL_FINFO_UPROT;
        }
    }

    if( (aWanted & STL_FINFO_GPROT) && (aFileInfo->mValid & STL_FINFO_GROUP) )
    {
        sIdent.TrusteeType = TRUSTEE_IS_GROUP;
        sIdent.ptstrName = aFileInfo->mGroupID;

        if( GetEffectiveRightsFromAcl(aDacl, &sIdent, &sAcc) == ERROR_SUCCESS )
        {
            aFileInfo->mProtection |= stfConvertProt( sAcc, STF_PROT_SCOPE_GROUP );
            aFileInfo->mValid |= STL_FINFO_GPROT;
        }
    }

    if( (aWanted & STL_FINFO_WPROT) && (gStfWorldID != NULL) )
    {
        sIdent.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
        sIdent.ptstrName = gStfWorldID;

        if( GetEffectiveRightsFromAcl(aDacl, &sIdent, &sAcc) == ERROR_SUCCESS )
        {
            aFileInfo->mProtection |= stfConvertProt( sAcc, STF_PROT_SCOPE_WORLD );
            aFileInfo->mValid |= STL_FINFO_WPROT;
        }
    }

    return STL_SUCCESS;
}

stlStatus stfGuessProtectionBits( stlFileInfo   * aFileInfo,
                                  stlInt32        aWanted,
                                  stlErrorStack * aErrorStack )
{
    if( aFileInfo->mProtection & STF_FREADONLY )
    {
        aFileInfo->mProtection |= STL_FPROT_WREAD | STL_FPROT_WEXECUTE;
    }
    else
    {
        aFileInfo->mProtection |= STL_FPROT_WREAD | STL_FPROT_WEXECUTE | STL_FPROT_WWRITE;
    }

    aFileInfo->mProtection |= (aFileInfo->mProtection << STF_PROT_SCOPE_GROUP)
        | (aFileInfo->mProtection << STF_PROT_SCOPE_USER);

    aFileInfo->mValid |= STL_FINFO_UPROT | STL_FINFO_GPROT | STL_FINFO_WPROT;

    return STL_SUCCESS;
}

stlStatus stfGetMoreInfo( stlFileInfo   * aFileInfo,
                          const void    * aFileName,
                          stlInt32        aWanted,
                          stlInt32        aWhatFile,
                          stlErrorStack * aErrorStack)
{
    PSID sUser  = NULL;
    PSID sGroup = NULL;
    PACL sDacl  = NULL;

    SECURITY_INFORMATION sSInfo = 0;
    PSECURITY_DESCRIPTOR sPDesc = NULL;

    DWORD sSizeLo;
    DWORD sSizeHi;

    stlInt32 sReturn;

    if( aWanted & (STL_FINFO_PROT | STL_FINFO_OWNER) )
    {
        if(aWanted & (STL_FINFO_USER | STL_FINFO_UPROT) )
        {
            sSInfo |= OWNER_SECURITY_INFORMATION;
        }
        if(aWanted & (STL_FINFO_GROUP | STL_FINFO_GPROT) )
        {
            sSInfo |= GROUP_SECURITY_INFORMATION;
        }
        if( aWanted & STL_FINFO_PROT )
        {
            sSInfo |= DACL_SECURITY_INFORMATION;
        }

        switch (aWhatFile)
        {
            case STF_MORE_OF_FSPEC:
                sReturn = GetNamedSecurityInfo( (stlChar*)aFileInfo,
                                                SE_FILE_OBJECT,
                                                sSInfo,
                                                ((aWanted & (STL_FINFO_USER | STL_FINFO_UPROT)) ? &sUser : NULL),
                                                ((aWanted & (STL_FINFO_GROUP | STL_FINFO_GPROT)) ? &sGroup : NULL),
                                                ((aWanted & STL_FINFO_PROT) ? &sDacl : NULL),
                                                NULL,
                                                &sPDesc );
                break;
            case STF_MORE_OF_HANDLE:
                sReturn = GetSecurityInfo( (HANDLE)aFileInfo,
                                           SE_FILE_OBJECT,
                                           sSInfo,
                                           ((aWanted & (STL_FINFO_USER | STL_FINFO_UPROT)) ? &sUser : NULL),
                                           ((aWanted & (STL_FINFO_GROUP | STL_FINFO_GPROT)) ? &sGroup : NULL),
                                           ((aWanted & STL_FINFO_PROT) ? &sDacl : NULL),
                                           NULL,
                                           &sPDesc );
                break;
            default :
                STL_ASSERT( STL_FALSE );
                break;
        }

        if( sReturn != ERROR_SUCCESS )
        {
            sUser  = NULL;
            sGroup = NULL;
            sDacl  = NULL;
        }

        if( sUser != NULL )
        {
            aFileInfo->mUserID = sUser;
            aFileInfo->mValid |= STL_FINFO_USER;
        }
        if( sGroup != NULL )
        {
            aFileInfo->mUserID = sGroup;
            aFileInfo->mValid |= STL_FINFO_GROUP;
        }
        if( sDacl != NULL )
        {
            STL_TRY( stfResolveProt( aFileInfo, aWanted, sDacl, aErrorStack ) == STL_SUCCESS );
        }
        else if( aWanted & STL_FINFO_PROT )
        {
            STL_TRY( stfGuessProtectionBits( aFileInfo, aWanted, aErrorStack ) == STL_SUCCESS );
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

#endif

stlStatus stfGetStat( stlFileInfo   * aFileInfo,
                      const stlChar * aFileName,
                      stlInt32        aWanted,
                      stlErrorStack * aErrorStack )
{
    stlBool   sWarn;

    WIN32_FILE_ATTRIBUTE_DATA sWinInfo;


    STL_PARAM_VALIDATE( aFileInfo != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aFileName != NULL, aErrorStack );
    
    STL_TRY_THROW( stlStrlen(aFileName) < STL_PATH_MAX, RAMP_ERR_NAMETOOLONG );

    STL_TRY_THROW( GetFileAttributesEx(aFileName, GetFileExInfoStandard, &sWinInfo ) != 0, RAMP_ERR_FILE_STAT );

    STL_TRY( stfFillInfo(aFileInfo, &sWinInfo, 0, aWanted, &sWarn, aErrorStack) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NAMETOOLONG )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_NAMETOOLONG,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_FILE_STAT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_FILE_STAT,
                      NULL,
                      aErrorStack );
        steSetSystemError( GetLastError(), aErrorStack );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus stfGetFileInfo( stlFileInfo   * aFileInfo,
                          stlInt32        aWanted,
                          stlFile       * aFile,
                          stlErrorStack * aErrorStack )
{
    stlBool   sWarn;

    BY_HANDLE_FILE_INFORMATION sInfo;

    STL_TRY_THROW(GetFileInformationByHandle(aFile->mFileDesc, &sInfo) != 0, RAMP_ERR_FILE_STAT);

    STL_TRY(stfFillInfo(aFileInfo, (WIN32_FILE_ATTRIBUTE_DATA*)&sInfo, 1, aWanted, &sWarn, aErrorStack) == STL_SUCCESS);

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_FILE_STAT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_FILE_STAT,
                      NULL,
                      aErrorStack );
        steSetSystemError( GetLastError(), aErrorStack );
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

    STL_TRY_THROW( GetCurrentDirectory(STL_SIZEOF(sPath), sPath) != 0, RAMP_ERR_FILE_PATH );

    STL_TRY_THROW( stlStrlen(sPath) < aPathBufLen, RAMP_ERR_NAMETOOLONG );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NAMETOOLONG )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_NAMETOOLONG,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_FILE_PATH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_FILE_PATH,
                      NULL,
                      aErrorStack );
        steSetSystemError( GetLastError(), aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

