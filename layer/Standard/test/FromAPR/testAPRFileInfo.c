#include <stdio.h>
#include <stl.h>

#define FILENAME    "data/file_datafile.txt"
#define NEWFILENAME "data/new_datafile.txt"
#define NEWFILEDATA "This is new text in a new file."

struct ViewFileinfo
{
    stlInt32 mBits;
    stlChar *mDescription;
} gViewFileInfo[] = {
    {STL_FINFO_MTIME,  "MTIME"},
    {STL_FINFO_CTIME,  "CTIME"},
    {STL_FINFO_ATIME,  "ATIME"},
    {STL_FINFO_SIZE,   "SIZE"},
    {STL_FINFO_DEV,    "DEV"},
    {STL_FINFO_INODE,  "INODE"},
    {STL_FINFO_NLINK,  "NLINK"},
    {STL_FINFO_TYPE,   "TYPE"},
    {STL_FINFO_USER,   "USER"}, 
    {STL_FINFO_GROUP,  "GROUP"}, 
    {STL_FINFO_UPROT,  "UPROT"}, 
    {STL_FINFO_GPROT,  "GPROT"},
    {STL_FINFO_WPROT,  "WPROT"},
    {0,                NULL}
}; 

stlBool FileInfoEqual(stlFileInfo *aFileInfo1, stlFileInfo *aFileInfo2)
{
    /* Minimum supported flags across all platforms (APR_FINFO_MIN) */
    STL_TRY( (aFileInfo1->mValid & aFileInfo2->mValid & STL_FINFO_TYPE) != 0);
    STL_TRY( aFileInfo1->mFileType == aFileInfo2->mFileType );

    STL_TRY( (aFileInfo1->mValid & aFileInfo2->mValid & STL_FINFO_SIZE) != 0);
    STL_TRY( aFileInfo1->mSize == aFileInfo2->mSize );

    STL_TRY( (aFileInfo1->mValid & aFileInfo2->mValid & STL_FINFO_ATIME) != 0);
    STL_TRY( aFileInfo1->mLastAccessedTime == aFileInfo2->mLastAccessedTime );

    STL_TRY( (aFileInfo1->mValid & aFileInfo2->mValid & STL_FINFO_MTIME) != 0);
    STL_TRY( aFileInfo1->mLastModifiedTime == aFileInfo2->mLastModifiedTime );

    STL_TRY( (aFileInfo1->mValid & aFileInfo2->mValid & STL_FINFO_CTIME) != 0);
    STL_TRY( aFileInfo1->mCreateTime == aFileInfo2->mCreateTime );

    /* Additional supported flags not supported on all platforms */
    if ((aFileInfo1->mValid & aFileInfo2->mValid & STL_FINFO_USER) != 0)
    {
        STL_TRY( aFileInfo1->mUserID == aFileInfo2->mUserID );
    }
    
    if ((aFileInfo1->mValid & aFileInfo2->mValid & STL_FINFO_GROUP) != 0)
    {
        STL_TRY( aFileInfo1->mGroupID == aFileInfo2->mGroupID );
    }
    
    if ((aFileInfo1->mValid & aFileInfo2->mValid & STL_FINFO_INODE) != 0)
    {
        STL_TRY( aFileInfo1->mInodeNumber == aFileInfo2->mInodeNumber );
    }
    
    if ((aFileInfo1->mValid & aFileInfo2->mValid & STL_FINFO_DEV) != 0)
    {
        STL_TRY( aFileInfo1->mDeviceID == aFileInfo2->mDeviceID );
    }
    
    if ((aFileInfo1->mValid & aFileInfo2->mValid & STL_FINFO_NLINK) != 0)
    {
        STL_TRY( aFileInfo1->mLinks == aFileInfo2->mLinks );
    }
    
    if ((aFileInfo1->mValid & aFileInfo2->mValid & STL_FINFO_ASIZE) != 0)
    {
        STL_TRY( aFileInfo1->mAllocatedSize == aFileInfo2->mAllocatedSize );
    }
    
    if ((aFileInfo1->mValid & aFileInfo2->mValid & STL_FINFO_PROT) != 0)
    {
        STL_TRY( aFileInfo1->mProtection == aFileInfo2->mProtection);
    }

    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

/**
 * @brief stlGetFileStatByHandle() 함수 테스트
 */
stlBool TestInfoGet()
{
    stlStatus      sStatus;
    stlErrorStack  sErrStack;

    stlFile        sFile;
    stlFileInfo    sFileInfo;

    STL_INIT_ERROR_STACK( &sErrStack );

    sStatus = stlOpenFile( &sFile,
                           FILENAME,
                           STL_FOPEN_READ,
                           STL_FPERM_OS_DEFAULT,
                           &sErrStack );
    STL_TRY( sStatus == STL_SUCCESS);

    sStatus = stlGetFileStatByHandle( &sFileInfo,
                                      STL_FINFO_NORM,
                                      &sFile,
                                      &sErrStack );
    STL_TRY( sStatus == STL_SUCCESS);
    
    sStatus = stlCloseFile( &sFile, &sErrStack );
    STL_TRY( sStatus == STL_SUCCESS);

    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

/**
 * @brief stlGetFileStatByName() 함수 테스트
 */
stlBool TestStat()
{
    // stlStatus      sStatus;
    stlErrorStack  sErrStack;

    // stlFile        sFile;
    stlFileInfo    sFileInfo;

    STL_INIT_ERROR_STACK( &sErrStack );

    STL_TRY( stlGetFileStatByName( &sFileInfo,
                                   FILENAME,
                                   STL_FINFO_NORM,
                                   &sErrStack )
             == STL_SUCCESS);

    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

/**
 * @brief stlGetFileStatByHandle(), stlGetFileStatByName() 함수 테스트
 */
stlBool TestStatEqualFileInfo()
{
    // stlStatus      sStatus;
    stlErrorStack  sErrStack;

    stlFile        sFile;
    stlFileInfo    sFileInfoHandle;
    stlFileInfo    sFileInfoName;

    STL_INIT_ERROR_STACK( &sErrStack );

    STL_TRY( stlOpenFile( &sFile,
                          FILENAME,
                          STL_FOPEN_READ,
                          STL_FPERM_OS_DEFAULT,
                          &sErrStack )
             == STL_SUCCESS);

    STL_TRY( stlGetFileStatByHandle( &sFileInfoHandle,
                                     STL_FINFO_NORM,
                                     &sFile,
                                     &sErrStack )
             == STL_SUCCESS);
    
    /* Opening the file may have toggled the atime member (time last
     * accessed), so fetch our apr_stat() after getting the fileinfo 
     * of the open file...
     */
    STL_TRY( stlGetFileStatByName( &sFileInfoName,
                                   FILENAME,
                                   STL_FINFO_NORM,
                                   &sErrStack )
             == STL_SUCCESS);

    STL_TRY( stlCloseFile( &sFile, &sErrStack ) == STL_SUCCESS );

    STL_TRY( FileInfoEqual( &sFileInfoHandle,
                            &sFileInfoName )
             == STL_TRUE);

    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

stlInt32 main(stlInt32 aArgc, stlChar** aArgv)
{
    stlInt32 sState = 0;

    STL_TRY(stlInitialize() == STL_SUCCESS);

    /*
     * Test InfoGet
     */
    sState = 1;
    STL_TRY(TestInfoGet() == STL_TRUE);

    /*
     * Test Stat
     */
    sState = 2;
    STL_TRY(TestStat() == STL_TRUE);

    /*
     * Test StatEqualFileInfo
     */
    sState = 3;
    STL_TRY(TestStatEqualFileInfo() == STL_TRUE);

    stlPrintf("PASS");

    return (0);

    STL_FINISH;

    stlPrintf("FAILURE %d", sState);

    return (-1);
}
