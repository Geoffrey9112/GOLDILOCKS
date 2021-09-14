#include <stdio.h>
#include <stl.h>

stlStatus CopyHelper( const stlChar *aFromFile,
                      const stlChar *aToFile,
                      stlInt32       aPerms,
                      stlBool        aAppend,
                      stlErrorStack *aErrStack )
{
    stlFileInfo sCopyFInfo;
    stlFileInfo sOrigFInfo;
    stlFileInfo sDestFInfo;

    stlSize     sDestFileSize;

    stlStatus   sStatus;

    sStatus = stlGetFileStatByName( &sDestFInfo,
                                    aToFile,
                                    STL_FINFO_SIZE,
                                    aErrStack );
    if ( sStatus == STL_SUCCESS )
    {
        sDestFileSize = sDestFInfo.mSize;
    }
    else
    {
        sDestFileSize = 0;
    }

    if ( aAppend == STL_FALSE )
    {
        sStatus = stlCopyFile( aFromFile,
                               aToFile,
                               ( STL_FOPEN_CREATE   |
                                 STL_FOPEN_WRITE    |
                                 STL_FOPEN_TRUNCATE ),
                               aPerms,
                               NULL,
                               0,
                               aErrStack );
        STL_TRY( sStatus == STL_SUCCESS );
    }
    else
    {
        sStatus = stlAppendFile( aFromFile,
                                 aToFile,
                                 aPerms,
                                 aErrStack );
        STL_TRY( sStatus == STL_SUCCESS );
    }

    sStatus = stlGetFileStatByName( &sOrigFInfo,
                                    aFromFile,
                                    STL_FINFO_SIZE,
                                    aErrStack );
    STL_TRY( sStatus == STL_SUCCESS );

    sStatus = stlGetFileStatByName( &sCopyFInfo,
                                    aToFile,
                                    STL_FINFO_SIZE,
                                    aErrStack );
    STL_TRY( sStatus == STL_SUCCESS );
    
    if (aAppend == STL_FALSE)
    {
        STL_TRY( sOrigFInfo.mSize == sCopyFInfo.mSize );
    }
    else
    {
        STL_TRY( (sDestFileSize + sOrigFInfo.mSize) == sCopyFInfo.mSize );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief stlCopyFile() 함수 테스트
 */
stlBool TestCopyShortFile()
{
    stlStatus      sStatus;
    stlErrorStack  sErrStack;

    STL_INIT_ERROR_STACK( &sErrStack );

    /* make absolutely sure that the dest file doesn't exist. */
    (void) stlRemoveFile( "data/file_copy.txt", &sErrStack );
    
    sStatus = CopyHelper( "data/file_datafile.txt",
                          "data/file_copy.txt", 
                          STL_FPERM_FILE_SOURCE,
                          STL_FALSE,    // stlCopyFile()
                          &sErrStack );
    STL_TRY( sStatus == STL_SUCCESS );
    
    sStatus = stlRemoveFile( "data/file_copy.txt",
                             &sErrStack );
    STL_TRY( sStatus == STL_SUCCESS );

    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

/**
 * @brief stlCopyFile() 함수 테스트
 */
stlBool TestCopyOverExisting()
{
    stlStatus      sStatus;
    stlErrorStack  sErrStack;

    STL_INIT_ERROR_STACK( &sErrStack );

    /* make absolutely sure that the dest file doesn't exist. */
    (void) stlRemoveFile( "data/file_copy.txt", &sErrStack );
    
    /* This is a cheat.  I don't want to create a new file, so I just copy
     * one file, then I copy another.  If the second copy succeeds, then
     * this works.
     */
    sStatus = CopyHelper( "data/file_datafile.txt",
                          "data/file_copy.txt", 
                          STL_FPERM_FILE_SOURCE,
                          STL_FALSE,     // stlCopyFile()
                          & sErrStack );
    STL_TRY( sStatus == STL_SUCCESS );

    sStatus = CopyHelper( "data/mmap_datafile.txt",
                          "data/file_copy.txt", 
                          STL_FPERM_FILE_SOURCE,
                          STL_FALSE,    // stlCopyFile()
                          & sErrStack );
    STL_TRY( sStatus == STL_SUCCESS );

    sStatus = stlRemoveFile( "data/file_copy.txt",
                             &sErrStack );
    STL_TRY( sStatus == STL_SUCCESS );

    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

/**
 * @brief stlAppendFile() 함수 테스트
 */
stlBool TestAppendNonExist()
{
    stlStatus      sStatus;
    stlErrorStack  sErrStack;

    STL_INIT_ERROR_STACK( &sErrStack );

    /* make absolutely sure that the dest file doesn't exist. */
    (void) stlRemoveFile( "data/file_copy.txt", &sErrStack );
    
    sStatus = CopyHelper( "data/file_datafile.txt",
                          "data/file_copy.txt", 
                          STL_FPERM_FILE_SOURCE,
                          STL_TRUE,     // stlAppendFile()
                          &sErrStack );
    STL_TRY( sStatus == STL_SUCCESS );

    sStatus = stlRemoveFile( "data/file_copy.txt",
                             &sErrStack );
    STL_TRY( sStatus == STL_SUCCESS );

    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

/**
 * @brief stlAppendFile() 함수 테스트
 */
stlBool TestAppendExist()
{
    stlStatus      sStatus;
    stlErrorStack  sErrStack;

    STL_INIT_ERROR_STACK( &sErrStack );

    /* make absolutely sure that the dest file doesn't exist. */
    (void) stlRemoveFile( "data/file_copy.txt", &sErrStack );
    
    /*
     * This is a cheat.  I don't want to create a new file, so I just copy
     * one file, then I copy another.  If the second copy succeeds, then
     * this works.
     */
    sStatus = CopyHelper( "data/file_datafile.txt",
                          "data/file_copy.txt", 
                          STL_FPERM_FILE_SOURCE,
                          STL_FALSE, // stlCopyFile()
                          &sErrStack );
    STL_TRY( sStatus == STL_SUCCESS );

    sStatus = CopyHelper( "data/mmap_datafile.txt",
                          "data/file_copy.txt", 
                          STL_FPERM_FILE_SOURCE,
                          STL_TRUE,  // stlAppendFile()
                          &sErrStack );
    STL_TRY( sStatus == STL_SUCCESS );

    sStatus = stlRemoveFile( "data/file_copy.txt",
                             &sErrStack );
    STL_TRY( sStatus == STL_SUCCESS );

    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

stlInt32 main(stlInt32 aArgc, stlChar** aArgv)
{
    stlInt32 sState = 0;

    STL_TRY(stlInitialize() == STL_SUCCESS);

    /*
     * Test CopyShortFile
     */
    sState = 1;
    STL_TRY(TestCopyShortFile() == STL_TRUE);

    /*
     * Test CopyOverExisting
     */
    sState = 2;
    STL_TRY(TestCopyOverExisting() == STL_TRUE);
    
    /*
     * Test AppendNonExist
     */
    sState = 3;
    STL_TRY(TestAppendNonExist() == STL_TRUE);
    
    /*
     * Test AppendNonExist
     */
    sState = 4;
    STL_TRY(TestAppendExist() == STL_TRUE);

    stlPrintf("PASS");

    return (0);

    STL_FINISH;

    stlPrintf("FAILURE %d", sState);

    return (-1);
}
