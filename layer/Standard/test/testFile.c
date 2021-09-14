#include <stl.h>

#define TEST_BUFFER_SIZE 6

stlErrorStack gErrorStack;
stlShm        gShm;

#define PRINTPASS stlPrintf( "  => PASS\n" );
#define PRINTFAIL stlPrintf( "  => FAIL\n" );
#define PRINTERR( aErrorStack )                                         \
    do                                                                  \
    {                                                                   \
        stlPrintf( "ERRSYSTEMCODE: %d ERRCODE: %d, ERRSTRING: %s\n",    \
                   stlGetLastSystemErrorCode( aErrorStack ),            \
                   stlGetLastErrorCode( aErrorStack ),                  \
                   stlGetLastErrorMessage( aErrorStack ) );             \
    } while(0)

#define TEST_VERIFY( aExpression )                                  \
    do                                                              \
    {                                                               \
        if( !(aExpression) )                                        \
        {                                                           \
            stlPrintf( "ERRORIGIN: %s(%d)\n", __FILE__, __LINE__ ); \
            goto STL_FINISH_LABEL;                                  \
        }                                                           \
    } while( 0 )

#define TEST_FILE_NAME  "testFile.tmp"
#define TEST_FILE_NAME2 "testFile.tmp2"

stlStatus test_stlOpenCloseRemoveFile()
{
    stlFile sFile;

    stlPrintf( "- Access violation\n" );
    /*
     * STL_FOPEN_READ나 STL_FOPEN_WRITE가 없는 경우
     */
    TEST_VERIFY( stlOpenFile( &sFile,
                              TEST_FILE_NAME,
                              STL_FOPEN_CREATE,
                              STL_FPERM_OS_DEFAULT,
                              &gErrorStack ) != STL_SUCCESS );
    TEST_VERIFY( stlGetLastErrorCode( &gErrorStack )
                 == STL_ERRCODE_ACCESS );
    stlPopError( &gErrorStack );
    
    /*
     * 기존 파일이 존재하는 경우
     */
    TEST_VERIFY( stlOpenFile( &sFile,
                              TEST_FILE_NAME,
                              STL_FOPEN_CREATE | STL_FOPEN_WRITE,
                              STL_FPERM_OS_DEFAULT,
                              &gErrorStack ) == STL_SUCCESS );
    TEST_VERIFY( stlCloseFile( &sFile,
                               &gErrorStack ) == STL_SUCCESS );
    
    TEST_VERIFY( stlOpenFile( &sFile,
                              TEST_FILE_NAME,
                              STL_FOPEN_EXCL | STL_FOPEN_WRITE,
                              STL_FPERM_OS_DEFAULT,
                              &gErrorStack ) != STL_SUCCESS );
    TEST_VERIFY( stlGetLastErrorCode( &gErrorStack )
                 == STL_ERRCODE_ACCESS );
    stlPopError( &gErrorStack );

    TEST_VERIFY( stlRemoveFile( TEST_FILE_NAME,
                                &gErrorStack ) == STL_SUCCESS );

    /*
     * STL_FPERM_UREAD로 되어 있는 파일을 STL_FOPEN_WRITE로 여는 경우
     */
    TEST_VERIFY( stlOpenFile( &sFile,
                              TEST_FILE_NAME,
                              STL_FOPEN_CREATE | STL_FOPEN_WRITE,
                              STL_FPERM_UREAD,
                              &gErrorStack ) == STL_SUCCESS );
    TEST_VERIFY( stlCloseFile( &sFile,
                               &gErrorStack ) == STL_SUCCESS );
    TEST_VERIFY( stlOpenFile( &sFile,
                              TEST_FILE_NAME,
                              STL_FOPEN_WRITE,
                              STL_FPERM_OS_DEFAULT,
                              &gErrorStack ) != STL_SUCCESS );
    TEST_VERIFY( stlGetLastErrorCode( &gErrorStack )
                 == STL_ERRCODE_ACCESS );
    stlPopError( &gErrorStack );
    
    TEST_VERIFY( stlRemoveFile( TEST_FILE_NAME,
                                &gErrorStack ) == STL_SUCCESS );
    PRINTPASS;
    
    stlPrintf( "- No Entry\n" );
    /*
     * 기존 파일이 존재하지 않는 경우
     */
    TEST_VERIFY( stlOpenFile( &sFile,
                              TEST_FILE_NAME,
                              STL_FOPEN_READ,
                              STL_FPERM_OS_DEFAULT,
                              &gErrorStack ) != STL_SUCCESS );
    TEST_VERIFY( stlGetLastErrorCode( &gErrorStack )
                 == STL_ERRCODE_NO_ENTRY );
    stlPopError( &gErrorStack );
    PRINTPASS;
    
    return STL_SUCCESS;

    STL_FINISH;

    PRINTERR( &gErrorStack );
    
    PRINTFAIL;

    return STL_FAILURE;
    
}

stlStatus test_stlReadWriteFile()
{
    stlFile   sFile;
    stlChar   sBuffer[100];
    stlChar   sBuffer2[100];
    stlSize   sWrittenBytes;
    stlSize   sReadBytes;
    stlSize   sBytes;
    stlOffset sOffset;

    stlStrncpy( sBuffer, "This is a test", 100 );
    stlPrintf( "- Write 0 bytes\n" );
    TEST_VERIFY( stlOpenFile( &sFile,
                              TEST_FILE_NAME,
                              STL_FOPEN_CREATE |
                              STL_FOPEN_WRITE |
                              STL_FOPEN_READ,
                              STL_FPERM_OS_DEFAULT,
                              &gErrorStack ) == STL_SUCCESS );
    TEST_VERIFY( stlWriteFile( &sFile,
                               sBuffer,
                               0,
                               &sWrittenBytes,
                               &gErrorStack ) == STL_SUCCESS );
    TEST_VERIFY( sWrittenBytes == 0 );
    PRINTPASS;
    
    stlPrintf( "- Write n bytes\n" );
    sBytes = stlStrlen( sBuffer );
    TEST_VERIFY( stlWriteFile( &sFile,
                               sBuffer,
                               sBytes,
                               &sWrittenBytes,
                               &gErrorStack ) == STL_SUCCESS );
    TEST_VERIFY( sWrittenBytes == sBytes );
    PRINTPASS;

    stlPrintf( "- Read 0 bytes\n" );
    sOffset = 0;
    TEST_VERIFY( stlSeekFile( &sFile,
                              STL_FSEEK_SET,
                              &sOffset,
                              &gErrorStack ) == STL_SUCCESS );
    TEST_VERIFY( stlReadFile( &sFile,
                              sBuffer,
                              0,
                              &sReadBytes,
                              &gErrorStack ) == STL_SUCCESS );
    TEST_VERIFY( sReadBytes == 0 );
    PRINTPASS;

    stlPrintf( "- Read n bytes\n" );
    sBytes = stlStrlen( sBuffer );
    TEST_VERIFY( stlReadFile( &sFile,
                              sBuffer2,
                              sBytes,
                              &sReadBytes,
                              &gErrorStack ) == STL_SUCCESS );
    TEST_VERIFY( sReadBytes == sBytes );
    TEST_VERIFY( stlStrncmp( sBuffer, sBuffer2, sBytes ) == 0 );
    TEST_VERIFY( stlReadFile( &sFile,
                              sBuffer,
                              sBytes,
                              &sReadBytes,
                              &gErrorStack ) != STL_SUCCESS );
    TEST_VERIFY( stlGetLastErrorCode( &gErrorStack ) == STL_ERRCODE_EOF );
    stlPopError( &gErrorStack );
    PRINTPASS;

    stlPrintf( "- Read bad file\n" );
    stlStrncpy( sBuffer, "This is a test", 100 );
    sBytes = stlStrlen( sBuffer );
    TEST_VERIFY( stlOpenFile( &sFile,
                              TEST_FILE_NAME,
                              STL_FOPEN_CREATE | STL_FOPEN_WRITE,
                              STL_FPERM_OS_DEFAULT,
                              &gErrorStack ) == STL_SUCCESS );
    TEST_VERIFY( stlWriteFile( &sFile,
                               sBuffer,
                               sBytes,
                               &sWrittenBytes,
                               &gErrorStack ) == STL_SUCCESS );
    sOffset = 0;
    TEST_VERIFY( stlSeekFile( &sFile,
                              STL_FSEEK_SET,
                              &sOffset,
                              &gErrorStack ) == STL_SUCCESS );
    TEST_VERIFY( stlReadFile( &sFile,
                              sBuffer,
                              sBytes,
                              &sReadBytes,
                              &gErrorStack ) != STL_SUCCESS );
    TEST_VERIFY( stlGetLastErrorCode( &gErrorStack )
                 == STL_ERRCODE_FILE_BAD );
    stlPopError( &gErrorStack );
    TEST_VERIFY( stlCloseFile( &sFile,
                               &gErrorStack ) == STL_SUCCESS );
    TEST_VERIFY( stlRemoveFile( TEST_FILE_NAME,
                                &gErrorStack ) == STL_SUCCESS );
    PRINTPASS;
    
    return STL_SUCCESS;

    STL_FINISH;

    PRINTERR( &gErrorStack );
    PRINTFAIL;

    return STL_FAILURE;
    
}

stlStatus test_stlCopyFile()
{
    stlFile   sFile;
    stlChar   sBuffer[100];
    stlChar   sBuffer2[100];
    stlSize   sWrittenBytes;
    stlSize   sReadBytes;
    stlSize   sBytes;

    stlStrncpy( sBuffer, "This is a test", 100 );
    TEST_VERIFY( stlOpenFile( &sFile,
                              TEST_FILE_NAME,
                              STL_FOPEN_CREATE |
                              STL_FOPEN_WRITE |
                              STL_FOPEN_READ,
                              STL_FPERM_OS_DEFAULT,
                              &gErrorStack ) == STL_SUCCESS );
    sBytes = stlStrlen( sBuffer );
    TEST_VERIFY( stlWriteFile( &sFile,
                               sBuffer,
                               sBytes,
                               &sWrittenBytes,
                               &gErrorStack ) == STL_SUCCESS );
    TEST_VERIFY( stlCloseFile( &sFile,
                               &gErrorStack ) == STL_SUCCESS );
    TEST_VERIFY( stlCopyFile( TEST_FILE_NAME,
                              TEST_FILE_NAME2,
                              ( STL_FOPEN_CREATE   |
                                STL_FOPEN_WRITE    |
                                STL_FOPEN_TRUNCATE ),
                              STL_FPERM_FILE_SOURCE,
                              NULL,
                              0,
                              &gErrorStack ) == STL_SUCCESS );
    TEST_VERIFY( stlOpenFile( &sFile,
                              TEST_FILE_NAME2,
                              STL_FOPEN_CREATE | STL_FOPEN_READ,
                              STL_FPERM_OS_DEFAULT,
                              &gErrorStack ) == STL_SUCCESS );
    TEST_VERIFY( stlReadFile( &sFile,
                              sBuffer2,
                              sBytes,
                              &sReadBytes,
                              &gErrorStack ) == STL_SUCCESS );
    TEST_VERIFY( sReadBytes == sBytes );
    TEST_VERIFY( stlStrncmp( sBuffer, sBuffer2, sBytes ) == 0 );
    TEST_VERIFY( stlCloseFile( &sFile,
                               &gErrorStack ) == STL_SUCCESS );
    TEST_VERIFY( stlRemoveFile( TEST_FILE_NAME,
                                &gErrorStack ) == STL_SUCCESS );
    TEST_VERIFY( stlRemoveFile( TEST_FILE_NAME2,
                                &gErrorStack ) == STL_SUCCESS );
    PRINTPASS;
    
    return STL_SUCCESS;

    STL_FINISH;

    PRINTERR( &gErrorStack );
    PRINTFAIL;

    return STL_FAILURE;
    
}

stlStatus test_stlAppendFile()
{
    stlFile   sFile;
    stlChar   sBuffer[100];
    stlChar   sBuffer2[100];
    stlSize   sWrittenBytes;
    stlSize   sReadBytes;
    stlSize   sBytes;

    stlStrncpy( sBuffer, "This is a test", 100 );
    TEST_VERIFY( stlOpenFile( &sFile,
                              TEST_FILE_NAME,
                              STL_FOPEN_CREATE |
                              STL_FOPEN_WRITE |
                              STL_FOPEN_READ,
                              STL_FPERM_OS_DEFAULT,
                              &gErrorStack ) == STL_SUCCESS );
    sBytes = stlStrlen( sBuffer );
    TEST_VERIFY( stlWriteFile( &sFile,
                               sBuffer,
                               sBytes,
                               &sWrittenBytes,
                               &gErrorStack ) == STL_SUCCESS );
    TEST_VERIFY( stlCloseFile( &sFile,
                               &gErrorStack ) == STL_SUCCESS );
    TEST_VERIFY( stlCopyFile( TEST_FILE_NAME,
                              TEST_FILE_NAME2,
                              ( STL_FOPEN_CREATE   |
                                STL_FOPEN_WRITE    |
                                STL_FOPEN_TRUNCATE ),
                              STL_FPERM_FILE_SOURCE,
                              NULL,
                              0,
                              &gErrorStack ) == STL_SUCCESS );
    TEST_VERIFY( stlAppendFile( TEST_FILE_NAME,
                                TEST_FILE_NAME2,
                                STL_FPERM_FILE_SOURCE,
                                &gErrorStack ) == STL_SUCCESS );
    TEST_VERIFY( stlOpenFile( &sFile,
                              TEST_FILE_NAME2,
                              STL_FOPEN_CREATE | STL_FOPEN_READ,
                              STL_FPERM_OS_DEFAULT,
                              &gErrorStack ) == STL_SUCCESS );
    TEST_VERIFY( stlReadFile( &sFile,
                              sBuffer2,
                              sBytes*2,
                              &sReadBytes,
                              &gErrorStack ) == STL_SUCCESS );
    TEST_VERIFY( sReadBytes == (sBytes*2) );
    stlStrncpy( sBuffer, "This is a testThis is a test", 100 );
    TEST_VERIFY( stlStrncmp( sBuffer, sBuffer2, sBytes*2 ) == 0 );
    TEST_VERIFY( stlCloseFile( &sFile,
                               &gErrorStack ) == STL_SUCCESS );
    TEST_VERIFY( stlRemoveFile( TEST_FILE_NAME,
                                &gErrorStack ) == STL_SUCCESS );
    TEST_VERIFY( stlRemoveFile( TEST_FILE_NAME2,
                                &gErrorStack ) == STL_SUCCESS );
    PRINTPASS;
    
    return STL_SUCCESS;

    STL_FINISH;

    PRINTERR( &gErrorStack );
    PRINTFAIL;

    return STL_FAILURE;
    
}

stlStatus test_stlTruncateFile()
{
    stlFile   sFile;
    stlChar   sBuffer[100];
    stlChar   sBuffer2[100];
    stlSize   sWrittenBytes;
    stlSize   sReadBytes;
    stlSize   sBytes;
    stlOffset sOffset;

    stlStrncpy( sBuffer, "This is a test", 100 );
    TEST_VERIFY( stlOpenFile( &sFile,
                              TEST_FILE_NAME,
                              STL_FOPEN_CREATE |
                              STL_FOPEN_WRITE |
                              STL_FOPEN_READ,
                              STL_FPERM_OS_DEFAULT,
                              &gErrorStack ) == STL_SUCCESS );
    sBytes = stlStrlen( sBuffer );
    TEST_VERIFY( stlWriteFile( &sFile,
                               sBuffer,
                               sBytes,
                               &sWrittenBytes,
                               &gErrorStack ) == STL_SUCCESS );
    TEST_VERIFY( stlWriteFile( &sFile,
                               sBuffer,
                               sBytes,
                               &sWrittenBytes,
                               &gErrorStack ) == STL_SUCCESS );
    TEST_VERIFY( stlCloseFile( &sFile,
                               &gErrorStack ) == STL_SUCCESS );
    TEST_VERIFY( stlOpenFile( &sFile,
                              TEST_FILE_NAME,
                              STL_FOPEN_CREATE |
                              STL_FOPEN_WRITE |
                              STL_FOPEN_READ,
                              STL_FPERM_OS_DEFAULT,
                              &gErrorStack ) == STL_SUCCESS );
    TEST_VERIFY( stlTruncateFile( &sFile,
                                  sBytes,
                                  &gErrorStack ) == STL_SUCCESS );
    sOffset = 0;
    TEST_VERIFY( stlSeekFile( &sFile,
                              STL_FSEEK_SET,
                              &sOffset,
                              &gErrorStack ) == STL_SUCCESS );
    TEST_VERIFY( stlReadFile( &sFile,
                              sBuffer2,
                              sBytes,
                              &sReadBytes,
                              &gErrorStack ) == STL_SUCCESS );
    TEST_VERIFY( sReadBytes == sBytes );
    TEST_VERIFY( stlStrncmp( sBuffer, sBuffer2, sBytes ) == 0 );
    TEST_VERIFY( stlCloseFile( &sFile,
                               &gErrorStack ) == STL_SUCCESS );
    TEST_VERIFY( stlRemoveFile( TEST_FILE_NAME,
                                &gErrorStack ) == STL_SUCCESS );
    PRINTPASS;
    
    return STL_SUCCESS;

    STL_FINISH;

    PRINTERR( &gErrorStack );
    PRINTFAIL;

    return STL_FAILURE;
    
}

stlStatus test_stlLockUnlockFile()
{
    stlFile        sFile;
    stlProc        sProc;
    stlBool        sIsChild;
    stlInt32       sExitCode;
    stlSemaphore * sSemaphore;
    stlUInt32      sState = 0;

    STL_TRY( stlCreateShm( &gShm,
                           "__SHM",
                           1000000,
                           NULL,
                           STL_SIZEOF(stlSemaphore)*2,
                           STL_FALSE,
                           &gErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    sSemaphore = (stlSemaphore*)stlGetShmBaseAddr( &gShm );

    TEST_VERIFY( stlOpenFile( &sFile,
                              TEST_FILE_NAME,
                              STL_FOPEN_CREATE |
                              STL_FOPEN_WRITE |
                              STL_FOPEN_READ,
                              STL_FPERM_OS_DEFAULT,
                              &gErrorStack ) == STL_SUCCESS );
        
    TEST_VERIFY( stlCreateSemaphore( &sSemaphore[0],
                                     "SEM1",
                                     0,
                                     &gErrorStack ) == STL_SUCCESS );
    sState = 2;
    
    TEST_VERIFY( stlCreateSemaphore( &sSemaphore[1],
                                     "SEM2",
                                     0,
                                     &gErrorStack ) == STL_SUCCESS );
    sState = 3;
    
    TEST_VERIFY( stlForkProc( &sProc,
                              &sIsChild,
                              &gErrorStack ) == STL_SUCCESS );

    if( sIsChild == STL_TRUE )
    {
        TEST_VERIFY( stlLockFile( &sFile,
                                  STL_FLOCK_EXCLUSIVE,
                                  &gErrorStack ) == STL_SUCCESS );
        
        TEST_VERIFY( stlReleaseSemaphore( &sSemaphore[1],
                                          &gErrorStack )
                     == STL_SUCCESS );
        TEST_VERIFY( stlAcquireSemaphore( &sSemaphore[0],
                                          &gErrorStack )
                     == STL_SUCCESS );
        
        TEST_VERIFY( stlUnlockFile( &sFile,
                                    &gErrorStack ) == STL_SUCCESS );
        
        TEST_VERIFY( stlCloseFile( &sFile,
                                   &gErrorStack ) == STL_SUCCESS );
        
        stlExitProc( STL_SUCCESS );
    }
    else
    {
        TEST_VERIFY( stlAcquireSemaphore( &sSemaphore[1],
                                          &gErrorStack )
                     == STL_SUCCESS );
    
        TEST_VERIFY( stlLockFile( &sFile,
                                  STL_FLOCK_EXCLUSIVE |
                                  STL_FLOCK_NONBLOCK,
                                  &gErrorStack ) != STL_SUCCESS );
        
        TEST_VERIFY( stlGetLastErrorCode( &gErrorStack )
                     == STL_ERRCODE_AGAIN );
        stlPopError( &gErrorStack );
        
        TEST_VERIFY( stlReleaseSemaphore( &sSemaphore[0],
                                          &gErrorStack )
                     == STL_SUCCESS );
        
        TEST_VERIFY( stlWaitProc( &sProc,
                                  &sExitCode,
                                  &gErrorStack ) == STL_SUCCESS );
        TEST_VERIFY( sExitCode == STL_SUCCESS );
        
        TEST_VERIFY( stlCloseFile( &sFile,
                                   &gErrorStack ) == STL_SUCCESS );
    }

    if( sIsChild == STL_FALSE )
    {
        TEST_VERIFY( stlRemoveFile( TEST_FILE_NAME,
                                    &gErrorStack ) == STL_SUCCESS );
        sState = 2;
        STL_TRY( stlDestroySemaphore( &sSemaphore[1],
                                      &gErrorStack ) == STL_SUCCESS );
        sState = 1;
        STL_TRY( stlDestroySemaphore( &sSemaphore[0],
                                      &gErrorStack ) == STL_SUCCESS );
        
        sState = 0;
        TEST_VERIFY( stlDestroyShm( &gShm,
                                    &gErrorStack ) == STL_SUCCESS );
        PRINTPASS;
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 3:
            stlDestroySemaphore( &sSemaphore[1],
                                 &gErrorStack );
        case 2:
            stlDestroySemaphore( &sSemaphore[0],
                                 &gErrorStack );
        case 1:
            stlDestroyShm( &gShm,
                           &gErrorStack );
            break;
        default:
            break;
    }

    PRINTERR( &gErrorStack );
    PRINTFAIL;

    return STL_FAILURE;
    
}

stlStatus test_stlRemoveFile()
{
    stlChar sFileName[4096];

    stlMemset( sFileName, 'A', 4096 );
    sFileName[4095] = '\0';
    
    TEST_VERIFY( stlRemoveFile( sFileName,
                                &gErrorStack ) != STL_SUCCESS );
    TEST_VERIFY( stlGetLastErrorCode( &gErrorStack )
                 == STL_ERRCODE_NAMETOOLONG );
    stlPopError( &gErrorStack );
    
    TEST_VERIFY( stlRemoveFile( "./UnknownDir/Zombie",
                                &gErrorStack ) != STL_SUCCESS );

    TEST_VERIFY( stlGetLastErrorCode( &gErrorStack )
                 == STL_ERRCODE_NO_ENTRY );
    stlPopError( &gErrorStack );
    
    PRINTPASS;
    
    return STL_SUCCESS;

    STL_FINISH;

    PRINTERR( &gErrorStack );
    PRINTFAIL;

    return STL_FAILURE;
}

stlStatus test_stlLinkFile()
{
    stlFile sFile;
    stlChar sFileName[4096];

    TEST_VERIFY( stlOpenFile( &sFile,
                              TEST_FILE_NAME,
                              STL_FOPEN_CREATE |
                              STL_FOPEN_WRITE |
                              STL_FOPEN_READ,
                              STL_FPERM_OS_DEFAULT,
                              &gErrorStack ) == STL_SUCCESS );
    TEST_VERIFY( stlCloseFile( &sFile,
                               &gErrorStack ) == STL_SUCCESS );
    
    stlMemset( sFileName, 'A', 4096 );
    sFileName[4096 - 1] = '\0';
    
    TEST_VERIFY( stlLinkFile( TEST_FILE_NAME,
                              sFileName,
                              &gErrorStack ) != STL_SUCCESS );
    TEST_VERIFY( stlGetLastErrorCode( &gErrorStack )
                 == STL_ERRCODE_NAMETOOLONG );
    stlPopError( &gErrorStack );
    
    TEST_VERIFY( stlLinkFile( TEST_FILE_NAME,
                              "./UnknownDir/Zombie",
                              &gErrorStack ) != STL_SUCCESS );
    TEST_VERIFY( stlGetLastErrorCode( &gErrorStack )
                 == STL_ERRCODE_NO_ENTRY );
    stlPopError( &gErrorStack );
    
    TEST_VERIFY( stlRemoveFile( TEST_FILE_NAME,
                                &gErrorStack ) == STL_SUCCESS );
    
    PRINTPASS;
    
    return STL_SUCCESS;

    STL_FINISH;

    PRINTERR( &gErrorStack );
    PRINTFAIL;

    return STL_FAILURE;
}

stlStatus test_stlGetFileStat()
{
    stlFile     sFile;
    stlFileInfo sFileInfo;
    stlChar     sBuffer[100];
    stlSize     sWrittenBytes;
    stlSize     sBytes;

    stlStrncpy( sBuffer, "This is a test", 100 );
    TEST_VERIFY( stlOpenFile( &sFile,
                              TEST_FILE_NAME,
                              STL_FOPEN_CREATE |
                              STL_FOPEN_WRITE |
                              STL_FOPEN_READ,
                              STL_FPERM_OS_DEFAULT,
                              &gErrorStack ) == STL_SUCCESS );

    sBytes = stlStrlen( sBuffer );
    TEST_VERIFY( stlWriteFile( &sFile,
                               sBuffer,
                               sBytes,
                               &sWrittenBytes,
                               &gErrorStack ) == STL_SUCCESS );
    
    TEST_VERIFY( stlCloseFile( &sFile,
                               &gErrorStack ) == STL_SUCCESS );

    TEST_VERIFY( stlGetFileStatByName( &sFileInfo,
                                       TEST_FILE_NAME,
                                       STL_FINFO_NORM,
                                       &gErrorStack )
                 == STL_SUCCESS );
    
    TEST_VERIFY( stlOpenFile( &sFile,
                              TEST_FILE_NAME,
                              STL_FOPEN_READ,
                              STL_FPERM_OS_DEFAULT,
                              &gErrorStack ) == STL_SUCCESS );
    
    TEST_VERIFY( stlGetFileStatByHandle( &sFileInfo,
                                         STL_FINFO_NORM,
                                         &sFile,
                                         &gErrorStack )
                 == STL_SUCCESS );
    
    TEST_VERIFY( stlCloseFile( &sFile,
                               &gErrorStack ) == STL_SUCCESS );
    
    TEST_VERIFY( stlRemoveFile( TEST_FILE_NAME,
                                &gErrorStack ) == STL_SUCCESS );
    
    PRINTPASS;
    
    return STL_SUCCESS;

    STL_FINISH;

    PRINTERR( &gErrorStack );
    PRINTFAIL;

    return STL_FAILURE;
}

stlInt32 main( stlInt32 aArgc, stlChar** aArgv )
{
    stlInt32  sState = 0;
    
    STL_TRY( stlInitialize() == STL_SUCCESS );
    sState = 1;

    STL_INIT_ERROR_STACK( &gErrorStack );

    /*
     * Test stlOpenFile & stlCloseFile
     */
    stlPrintf( "1. Test stlOpenFile & stlCloseFile\n" );
    STL_TRY( test_stlOpenCloseRemoveFile() == STL_SUCCESS );
    
    /*
     * Test stlReadFile & stlWriteFile
     */
    stlPrintf( "2. Test stlReadFile & stlWriteFile\n" );
    STL_TRY( test_stlReadWriteFile() == STL_SUCCESS );
    
    /*
     * Test stlCopyFile
     */
    stlPrintf( "3. Test stlCopyFile\n" );
    STL_TRY( test_stlCopyFile() == STL_SUCCESS );

    /*
     * Test stlAppendFile
     */
    stlPrintf( "4. Test stlAppendFile\n" );
    STL_TRY( test_stlAppendFile() == STL_SUCCESS );

    /*
     * Test stlTruncateFile
     */
    stlPrintf( "5. Test stlTruncateFile\n" );
    STL_TRY( test_stlTruncateFile() == STL_SUCCESS );

    /*
     * Test stlLockFile & stlUnlockFile
     */
    stlPrintf( "6. Test stlLockFile & stlUnlockFile\n" );
    STL_TRY( test_stlLockUnlockFile() == STL_SUCCESS );
    
    /*
     * Test stlRemoveFile
     */
    stlPrintf( "7. Test stlRemoveFile\n" );
    STL_TRY( test_stlRemoveFile() == STL_SUCCESS );

    /*
     * Test stlLinkFile
     */
    stlPrintf( "8. Test stlLinkFile\n" );
    STL_TRY( test_stlLinkFile() == STL_SUCCESS );

    /*
     * Test stlGetFileStat
     */
    stlPrintf( "9. Test stlGetFileStat\n" );
    STL_TRY( test_stlGetFileStat() == STL_SUCCESS );

    sState = 0;
    stlTerminate();

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            stlTerminate();
        default:
            break;
    }

    return STL_FAILURE;
}
