#include <stdio.h>
#include <stl.h>

#define TEST "Testing\n"
#define TEST2 "Testing again\n"
#define FILEPATH "data/"

#define PRINTERR                                            \
    do                                                      \
    {                                                       \
        stlPrintf( "ERRCODE: %d, ERRSTRING: %s\n",          \
                   stlGetLastErrorCode( &sErrStack ),       \
                   stlGetLastErrorMessage( &sErrStack ) );  \
    } while(0)

/**
 * @brief stlDuplicateFile(1) 함수 테스트
 */
stlBool TestFileDup()
{
    stlFile     sFile1;
    stlFile     sFile2;
    stlFileInfo sFileInfo;

    stlErrorStack  sErrStack;
    
    STL_INIT_ERROR_STACK( &sErrStack );
    
    /* First, create a new file, empty... */
    STL_TRY( stlOpenFile( &sFile1,
                          FILEPATH "testdup.file",
                          STL_FOPEN_READ | STL_FOPEN_WRITE | STL_FOPEN_CREATE,
                          STL_FPERM_OS_DEFAULT,
                          &sErrStack )
             == STL_SUCCESS );

    STL_TRY( stlDuplicateFile( &sFile2,
                               &sFile1,
                               1,
                               &sErrStack )
             == STL_SUCCESS);

    STL_TRY( stlCloseFile( &sFile1, &sErrStack ) == STL_SUCCESS );

    /* cleanup after ourselves */
    STL_TRY( stlCloseFile( &sFile2, &sErrStack ) == STL_SUCCESS);

    STL_TRY( stlGetFileStatByName( &sFileInfo,
                                   FILEPATH "testdup.file",
                                   STL_FINFO_NORM,
                                   &sErrStack )
             == STL_SUCCESS );

    return STL_TRUE;

    STL_FINISH;

    PRINTERR;
    
    return STL_FALSE;
}  

/**
 * @brief stlDuplicateFile(1) 함수 테스트
 */
stlBool TestFileReadWrite()
{
    stlFile     sFile1;
    stlFile     sFile2;
    stlFileInfo sFileInfo;
    
    stlSize     sTxtLen = sizeof(TEST);
    stlChar     sBuf[50];
    stlOffset   sFilePos;

    stlErrorStack  sErrStack;
    
    STL_INIT_ERROR_STACK( &sErrStack );

    /* First, create a new file, empty... */
    STL_TRY( stlOpenFile(&sFile1,
                         FILEPATH "testdup.readwrite.file",
                         STL_FOPEN_READ | STL_FOPEN_WRITE | STL_FOPEN_CREATE,
                         STL_FPERM_OS_DEFAULT,
                         &sErrStack )
             == STL_SUCCESS);

    STL_TRY( stlDuplicateFile( &sFile2,
                               &sFile1,
                               1,
                               &sErrStack )
             == STL_SUCCESS);

    STL_TRY( stlWriteFile( &sFile2,
                           TEST,
                           sTxtLen,
                           &sTxtLen,
                           &sErrStack )
             == STL_SUCCESS);
    STL_TRY(sizeof(TEST) == sTxtLen);
    
    sFilePos = 0;

    STL_TRY( stlSeekFile( &sFile1,
                          STL_FSEEK_SET,
                          &sFilePos,
                          &sErrStack )
             == STL_SUCCESS);
    STL_TRY(0 == sFilePos);

    sTxtLen = 50;

    STL_TRY( stlReadFile( &sFile1,
                          sBuf,
                          sTxtLen,
                          &sTxtLen,
                          &sErrStack )
             == STL_SUCCESS);
    STL_TRY( strcmp(TEST, sBuf) == 0 );

    /* cleanup after ourselves */
    STL_TRY( stlCloseFile( &sFile1, &sErrStack ) == STL_SUCCESS);
    STL_TRY( stlCloseFile( &sFile2, &sErrStack ) == STL_SUCCESS);

    STL_TRY( stlGetFileStatByName( &sFileInfo,
                                   FILEPATH "testdup.readwrite.file",
                                   STL_FINFO_NORM,
                                   &sErrStack )
             == STL_SUCCESS );

    return STL_TRUE;

    STL_FINISH;

    PRINTERR;
    
    return STL_FALSE;
}  

/**
 * @brief stlDuplicateFile(2) 함수 테스트
 */
stlBool TestDup2()
{
    stlFile  sTestFile;
    stlFile  sErrFile;
    stlFile  sSaveErr;

    stlErrorStack  sErrStack;
    
    STL_INIT_ERROR_STACK( &sErrStack );
    
    STL_TRY( stlOpenFile( &sTestFile,
                          FILEPATH "testdup2.file",
                          STL_FOPEN_READ | STL_FOPEN_WRITE | STL_FOPEN_CREATE,
                          STL_FPERM_OS_DEFAULT,
                          &sErrStack )
             == STL_SUCCESS );

    STL_TRY( stlOpenStderrFile( &sErrFile, &sErrStack ) == STL_SUCCESS);

    /* Set aside the real errfile */
    STL_TRY( stlDuplicateFile( &sSaveErr,
                               &sErrFile,
                               1,
                               &sErrStack )
             == STL_SUCCESS );

    STL_TRY( stlDuplicateFile( &sErrFile,
                               &sTestFile,
                               2,
                               &sErrStack )
             == STL_SUCCESS );

    STL_TRY( stlCloseFile( &sTestFile, &sErrStack ) == STL_SUCCESS );

    STL_TRY( stlDuplicateFile( &sErrFile,
                               &sSaveErr,
                               2,
                               &sErrStack )
             == STL_SUCCESS );

    STL_TRY( stlCloseFile( &sSaveErr, &sErrStack ) == STL_SUCCESS );

    return STL_TRUE;

    STL_FINISH;

    PRINTERR;
    
    return STL_FALSE;
}

/**
 * @brief stlDuplicateFile(2) 함수 테스트
 */
stlBool TestDup2ReadWrite()
{
    stlFile  sErrFile;
    stlFile  sTestFile;
    stlFile  sSaveErr ;
    
    stlSize     sTxtLen   = sizeof(TEST);
    stlChar     sBuf[50];
    stlOffset   sFilePos;

    stlErrorStack  sErrStack;
    
    STL_INIT_ERROR_STACK( &sErrStack );
    
    STL_TRY( stlOpenFile( &sTestFile,
                          FILEPATH "testdup2.readwrite.file",
                          STL_FOPEN_READ | STL_FOPEN_WRITE | STL_FOPEN_CREATE,
                          STL_FPERM_OS_DEFAULT,
                          &sErrStack )
             == STL_SUCCESS );

    STL_TRY( stlOpenStderrFile( &sErrFile, &sErrStack ) == STL_SUCCESS );

    /* Set aside the real errfile */
    STL_TRY( stlDuplicateFile( &sSaveErr,
                               &sErrFile,
                               1,
                               &sErrStack )
             == STL_SUCCESS );

    STL_TRY( stlDuplicateFile( &sErrFile,
                               &sTestFile,
                               2,
                               &sErrStack )
             == STL_SUCCESS );

    sTxtLen = sizeof(TEST2);
    STL_TRY( stlWriteFile( &sErrFile,
                           TEST2,
                           sTxtLen,
                           &sTxtLen,
                           &sErrStack )
             == STL_SUCCESS );
    STL_TRY( sizeof(TEST2) == sTxtLen );

    sFilePos = 0;
    STL_TRY( stlSeekFile( &sTestFile,
                          STL_FSEEK_SET,
                          &sFilePos,
                          &sErrStack )
             == STL_SUCCESS );
    STL_TRY( 0 == sFilePos );

    sTxtLen = 50;
    STL_TRY( stlReadFile( &sTestFile,
                          sBuf,
                          sTxtLen,
                          &sTxtLen,
                          &sErrStack )
             == STL_SUCCESS);
    STL_TRY( strcmp(TEST2, sBuf) == 0);

    STL_TRY( stlCloseFile( &sTestFile, &sErrStack ) == STL_SUCCESS );

    STL_TRY( stlDuplicateFile( &sErrFile,
                               &sSaveErr,
                               2,
                               &sErrStack )
             == STL_SUCCESS);

    STL_TRY( stlCloseFile( &sSaveErr, &sErrStack ) == STL_SUCCESS );

    return STL_TRUE;

    STL_FINISH;

    PRINTERR;
    
    return STL_FALSE;
}


stlInt32 main(stlInt32 aArgc, stlChar** aArgv)
{
    stlInt32 sState = 0;

    STL_TRY(stlInitialize() == STL_SUCCESS);

    /*
     * Test FileDup
     */
    sState = 1;
    STL_TRY(TestFileDup() == STL_TRUE);

    /*
     * Test FileReadWrite
     */
    sState = 2;
    STL_TRY(TestFileReadWrite() == STL_TRUE);

    /*
     * Test Dup2
     */
    sState = 3;
    STL_TRY(TestDup2() == STL_TRUE);

    /*
     * Test Dup2ReadWrite
     */
    sState = 4;
    STL_TRY(TestDup2ReadWrite() == STL_TRUE);

    stlPrintf("PASS");

    return (0);

    STL_FINISH;

    stlPrintf("FAILURE %d", sState);

    return (-1);
}
