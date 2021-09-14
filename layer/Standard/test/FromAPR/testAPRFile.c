#include <stdio.h>
#include <stdlib.h>
#include <stl.h>

#define DIRNAME          "data"
#define FILENAME DIRNAME "/file_datafile.txt"
#define TESTSTR          "This is the file data file."

#define TESTREAD_BLKSIZE 1024
#define BUFFERSIZE       4096

#define PRINTERR                                            \
    do                                                      \
    {                                                       \
        stlPrintf( "ERRCODE: %d, ERRSTRING: %s\n",          \
                   stlGetLastErrorCode( &sErrStack ),       \
                   stlGetLastErrorMessage( &sErrStack ) );  \
    } while(0)

        
/**
 * @brief stlOpenFile() 함수 테스트
 *        <BR> READ/WRITE 가 없는 Open Flag 사용을 테스트한다.
 */

stlBool TestOpenNoReadWrite()
{
    stlStatus      sStatus;
    stlErrorStack  sErrStack;

    stlFile        sFile;

    STL_INIT_ERROR_STACK( &sErrStack );

    /*
     * READ/WRITE 가 없는 Open Flag 사용
     */

    sStatus = stlOpenFile( & sFile,
                           FILENAME,
                           STL_FOPEN_CREATE | STL_FOPEN_EXCL,
                           STL_FPERM_UREAD | STL_FPERM_UWRITE | STL_FPERM_GREAD,
                           &sErrStack );

    /* 실패해야 함 */
    STL_TRY( sStatus != STL_SUCCESS );
    STL_TRY( stlGetLastErrorCode( &sErrStack ) == STL_ERRCODE_ACCESS );
                           
    return STL_TRUE;

    STL_FINISH;

    PRINTERR;
    
    return STL_FALSE;
}

/**
 * @brief stlOpenFile() 함수 테스트
 *   <BR> 존재하는 File 에 대한 EXCL open flag 사용 테스트 
 */
stlBool TestOpenExcl()
{
    stlStatus      sStatus;
    stlErrorStack  sErrStack;

    stlFile        sFile;

    STL_INIT_ERROR_STACK( &sErrStack );

    /*
     * 존재하는 File 에 대한 EXCL open flag을 사용
     */
    sStatus = stlOpenFile(&sFile,
                          FILENAME,
                          STL_FOPEN_CREATE | STL_FOPEN_EXCL | STL_FOPEN_WRITE, 
                          STL_FPERM_UREAD | STL_FPERM_UWRITE | STL_FPERM_GREAD,
                          &sErrStack );

    /* 실패해야 함 */
    STL_TRY( sStatus != STL_SUCCESS );
    STL_TRY( stlGetLastErrorCode( &sErrStack ) == STL_ERRCODE_FILE_OPEN );

    return STL_TRUE;

    STL_FINISH;

    PRINTERR;
    
    return STL_FALSE;
}

/**
 * @brief stlOpenFile() 함수 테스트
 *   <BR> READ open flag 테스트
 */
stlBool TestOpenRead()
{
    stlStatus      sStatus;
    stlErrorStack  sErrStack;

    stlFile        sFile;

    STL_INIT_ERROR_STACK( &sErrStack );

    sStatus = stlOpenFile( &sFile,
                           FILENAME,
                           STL_FOPEN_READ, 
                           STL_FPERM_UREAD | STL_FPERM_UWRITE | STL_FPERM_GREAD,
                           &sErrStack );
    
    STL_TRY( sStatus == STL_SUCCESS );
    STL_TRY( stlCloseFile( &sFile, &sErrStack ) == STL_SUCCESS );

    return STL_TRUE;

    STL_FINISH;

    PRINTERR;
             
    return STL_FALSE;
}

/**
 * @brief stlOpenFile() 함수 테스트
 *   <BR> READ/WRITE open flag 테스트
 */
stlBool TestOpenReadWrite()
{
    stlStatus      sStatus;
    stlErrorStack  sErrStack;

    stlFile        sFile;

    STL_INIT_ERROR_STACK( &sErrStack );

    sStatus = stlOpenFile( &sFile,
                           FILENAME,
                           STL_FOPEN_READ | STL_FOPEN_WRITE, 
                           STL_FPERM_UREAD | STL_FPERM_UWRITE | STL_FPERM_GREAD,
                           &sErrStack );
    
    STL_TRY( sStatus == STL_SUCCESS );
    STL_TRY( stlCloseFile( &sFile, &sErrStack ) == STL_SUCCESS );
    
    return STL_TRUE;

    STL_FINISH;

    PRINTERR;

    return STL_FALSE;
}

/**
 * @brief stlLinkFile() 함수 테스트
 *   <BR> 정상파일에 대한 link 테스트 
 */
stlBool TestLinkExisting()
{
    stlStatus      sStatus;
    stlErrorStack  sErrStack;

    // stlFile        sFile;

    STL_INIT_ERROR_STACK( &sErrStack );

    sStatus = stlLinkFile( "data/file_datafile.txt",
                           "data/file_datafile2.txt",
                           &sErrStack );
    STL_TRY( sStatus == STL_SUCCESS );
    
    STL_TRY( stlRemoveFile( "data/file_datafile2.txt",
                            &sErrStack ) == STL_SUCCESS );

    STL_TRY( sStatus == STL_SUCCESS );

    return STL_TRUE;

    STL_FINISH;

    PRINTERR;

    return STL_FALSE;
}

/**
 * @brief stlLinkFile() 함수 테스트
 *   <BR> 존재하지 않는 파일에 대한 link 테스트
 */
stlBool TestNonLinkExisting()
{
    stlStatus      sStatus;
    stlErrorStack  sErrStack;

    // stlFile        sFile;

    STL_INIT_ERROR_STACK( &sErrStack );

    /*
     * 실패해야 함
     */
    
    sStatus = stlLinkFile( "data/does_not_exist.txt",
                           "data/fake.txt",
                           &sErrStack );
    STL_TRY( sStatus != STL_SUCCESS );

    return STL_TRUE;

    STL_FINISH;

    PRINTERR;

    return STL_FALSE;
}

/**
 * @brief stlReadFile() 함수 테스트
 */
stlBool TestRead()
{
    stlStatus      sStatus;
    stlErrorStack  sErrStack;

    stlFile        sFile;
    stlSize        sBytes = 256;
    stlChar        sStr[256];

    STL_INIT_ERROR_STACK( &sErrStack );

    sStatus = stlOpenFile( &sFile,
                           FILENAME,
                           STL_FOPEN_READ,
                           STL_FPERM_UREAD | STL_FPERM_UWRITE | STL_FPERM_GREAD,
                           &sErrStack );
    STL_TRY(sStatus == STL_SUCCESS);

    sStatus = stlReadFile( &sFile,
                           sStr,
                           256,
                           &sBytes,
                           &sErrStack );
    STL_TRY( sStatus == STL_SUCCESS );
    /*
    if( sStatus == STL_SUCCESS )
    {
        STL_TRY( stlGetLastErrorCode( &sErrStack ) == STL_ERRCODE_EOF );
    }
    */

    STL_TRY( strlen(TESTSTR) == sBytes );
    STL_TRY( strncmp(TESTSTR, sStr, sBytes) == 0 );

    STL_TRY( stlCloseFile( &sFile, &sErrStack ) == STL_SUCCESS );

    return STL_TRUE;

    STL_FINISH;

    PRINTERR;

    return STL_FALSE;
}

/**
 * @brief stlReadFile() 함수 테스트
 *   <BR> 0 byte read 테스트 
 */
stlBool TestReadZero()
{
    stlStatus      sStatus;
    stlErrorStack  sErrStack;
    
    stlSize   sBytes = 0;
    stlChar   sStr[256];
    stlFile   sFile;

    STL_INIT_ERROR_STACK( &sErrStack );
    
    sStatus = stlOpenFile( &sFile,
                           FILENAME,
                           STL_FOPEN_READ,
                           STL_FPERM_OS_DEFAULT,
                           &sErrStack );
    STL_TRY( sStatus == STL_SUCCESS );

    /*
     * 0 byte read 를 시도할 경우, Error가 발생해서는 안된다.
     */
    sStatus = stlReadFile( &sFile,
                           sStr,
                           sBytes,
                           &sBytes,
                           &sErrStack );
    STL_TRY( sStatus == STL_SUCCESS );

    STL_TRY( 0 == sBytes );

    STL_TRY( stlCloseFile( &sFile, &sErrStack ) == STL_SUCCESS );

    return STL_TRUE;

    STL_FINISH;

    PRINTERR;

    return STL_FALSE;
}

/**
 * @brief stlSeekFile() 함수 테스트
 */
stlBool TestSeek()
{
    stlStatus      sStatus;
    stlErrorStack  sErrStack;
    
    stlFile        sFile;
    stlOffset      sOffset = 5;
    stlSize        sBytes = 256;
    stlChar        sStr[sBytes];

    memset(sStr, 0, 256);

    STL_INIT_ERROR_STACK( &sErrStack );
    
    sStatus = stlOpenFile( &sFile,
                           FILENAME,
                           STL_FOPEN_READ,
                           STL_FPERM_UREAD | STL_FPERM_UWRITE | STL_FPERM_GREAD,
                           &sErrStack );
    STL_TRY(sStatus == STL_SUCCESS);

    /*
     * 전체 Read 테스트
     */
    
    sStatus = stlReadFile( &sFile,
                           sStr,
                           sBytes,
                           &sBytes,
                           &sErrStack );

    STL_TRY( sStatus == STL_SUCCESS );
    /*
    if( sStatus != STL_SUCCESS )
    {
        STL_TRY( stlGetLastErrorCode( &sErrStack ) == STL_ERRCODE_EOF );
    }
    */

    STL_TRY( strlen(TESTSTR) == sBytes );
    STL_TRY( strcmp(TESTSTR, sStr) == 0 );

    memset(sStr, 0, 256);

    /*
     * SEEK_SET Read 테스트
     */
    
    sStatus = stlSeekFile( &sFile,
                           STL_FSEEK_SET,
                           &sOffset,
                           &sErrStack );
    STL_TRY(sStatus == STL_SUCCESS);

    sStatus = stlReadFile( &sFile,
                           sStr,
                           sBytes,
                           &sBytes,
                           &sErrStack );
    STL_TRY( sStatus == STL_SUCCESS );
    /*
    if( sStatus != STL_SUCCESS )
    {
        STL_TRY( stlGetLastErrorCode( &sErrStack ) == STL_ERRCODE_EOF );
    }
    */

    STL_TRY((strlen(TESTSTR) - 5) == sBytes);
    STL_TRY(strcmp((TESTSTR + 5), sStr) == 0);

    STL_TRY( stlCloseFile( &sFile, &sErrStack ) == STL_SUCCESS );

    memset(sStr, 0, 256);
    
    /*
     * SEEK_END 테스트
     */
    
    sStatus = stlOpenFile( &sFile,
                           FILENAME,
                           STL_FOPEN_READ,
                           STL_FPERM_UREAD | STL_FPERM_UWRITE | STL_FPERM_GREAD,
                           &sErrStack );
    STL_TRY(sStatus == STL_SUCCESS);

    sOffset = -5;
    sStatus = stlSeekFile( &sFile,
                           STL_FSEEK_END,
                           &sOffset,
                           &sErrStack );
    STL_TRY( sStatus == STL_SUCCESS );

    sStatus = stlReadFile( &sFile,
                           sStr,
                           sBytes,
                           &sBytes,
                           &sErrStack );
    STL_TRY( sStatus == STL_SUCCESS );
    /*
    if( sStatus != STL_SUCCESS )
    {
        STL_TRY( stlGetLastErrorCode( &sErrStack ) == STL_ERRCODE_EOF );
    }
    */

    STL_TRY( sBytes == 5 );
    STL_TRY( strcmp( (TESTSTR + strlen(TESTSTR) - 5), sStr) == 0);

    return STL_TRUE;

    STL_FINISH;

    PRINTERR;

    return STL_FALSE;
}                

/**
 * @brief stlCloseFile() 함수 테스트
 */
stlBool TestFileClose()
{
    stlStatus      sStatus;
    stlErrorStack  sErrStack;
    
    stlChar        sStr;
    stlSize        sOne = 1;
    stlFile        sFile;

    STL_INIT_ERROR_STACK( &sErrStack );

    sStatus = stlOpenFile( &sFile,
                           FILENAME,
                           STL_FOPEN_READ,
                           STL_FPERM_UREAD | STL_FPERM_UWRITE | STL_FPERM_GREAD,
                           &sErrStack );
    STL_TRY(sStatus == STL_SUCCESS);

    sStatus = stlCloseFile( &sFile, &sErrStack );
    STL_TRY(sStatus == STL_SUCCESS);

    /* We just closed the file, so this should fail */
    sStatus = stlReadFile( &sFile,
                           &sStr,
                           1,
                           &sOne,
                           &sErrStack );
    STL_TRY( stlGetLastErrorCode( &sErrStack ) == STL_ERRCODE_FILE_BAD );

    return STL_TRUE;

    STL_FINISH;

    PRINTERR;

    return STL_FALSE;
}

/**
 * @brief stlRemoveFile() 함수 테스트
 */
stlBool TestFileRemove()
{
    stlStatus      sStatus;
    stlErrorStack  sErrStack;

    stlFile        sFile;

    STL_INIT_ERROR_STACK( &sErrStack );

    sStatus = stlRemoveFile( FILENAME, &sErrStack );
    STL_TRY(sStatus == STL_SUCCESS);

    sStatus = stlOpenFile( &sFile,
                           FILENAME,
                           STL_FOPEN_READ,
                           STL_FPERM_UREAD | STL_FPERM_UWRITE | STL_FPERM_GREAD,
                           &sErrStack );
    STL_TRY( sStatus != STL_SUCCESS );
    STL_TRY( stlGetLastErrorCode( &sErrStack ) == STL_ERRCODE_NO_ENTRY );

    return STL_TRUE;

    STL_FINISH;

    PRINTERR;

    return STL_FALSE;
}

/**
 * @brief stlOpenFile() 함수 테스트
 * <BR> 존재하지 않는 파일을 Open
 */
stlBool TestOpenWrite()
{
    stlStatus      sStatus;
    stlErrorStack  sErrStack;

    stlFile        sFile;

    STL_INIT_ERROR_STACK( &sErrStack );

    sStatus = stlOpenFile( &sFile,
                           FILENAME,
                           STL_FOPEN_WRITE,
                           STL_FPERM_UREAD | STL_FPERM_UWRITE | STL_FPERM_GREAD,
                           &sErrStack );
    STL_TRY( sStatus != STL_SUCCESS );
    STL_TRY( stlGetLastErrorCode( &sErrStack ) == STL_ERRCODE_NO_ENTRY );

    return STL_TRUE;

    STL_FINISH;

    PRINTERR;

    return STL_FALSE;
}

/**
 * @brief stlOpenFile() 함수 테스트
 * <BR> Write를 위한 open(CREATE)
 */
stlBool TestOpenWriteCreate()
{
    stlStatus      sStatus;
    stlErrorStack  sErrStack;

    stlFile        sFile;

    STL_INIT_ERROR_STACK( &sErrStack );
    
    sStatus = stlOpenFile( &sFile,
                           FILENAME,
                           STL_FOPEN_WRITE | STL_FOPEN_CREATE,
                           STL_FPERM_UREAD | STL_FPERM_UWRITE | STL_FPERM_GREAD,
                           &sErrStack );
    STL_TRY( sStatus == STL_SUCCESS );

    STL_TRY( stlCloseFile( &sFile, &sErrStack ) == STL_SUCCESS );

    return STL_TRUE;

    STL_FINISH;

    PRINTERR;

    return STL_FALSE;
}

/**
 * @brief stlWriteFile() 함수 테스트
 */
stlBool TestWrite()
{
    stlStatus      sStatus;
    stlErrorStack  sErrStack;

    stlFile        sFile;
    stlSize        sBytes;

    STL_INIT_ERROR_STACK( &sErrStack );

    sStatus = stlOpenFile( &sFile,
                           FILENAME,
                           STL_FOPEN_WRITE | STL_FOPEN_CREATE,
                           STL_FPERM_UREAD | STL_FPERM_UWRITE | STL_FPERM_GREAD,
                           &sErrStack );
    STL_TRY(sStatus == STL_SUCCESS);

    sStatus = stlWriteFile( &sFile,
                            TESTSTR,
                            strlen(TESTSTR),
                            &sBytes,
                            &sErrStack );
    STL_TRY(sStatus == STL_SUCCESS);

    STL_TRY( stlCloseFile( &sFile, &sErrStack ) == STL_SUCCESS );

    return STL_TRUE;

    STL_FINISH;

    PRINTERR;

    return STL_FALSE;
}


/**
 * @brief stlGetCharacterFile() 함수 테스트
 */
stlBool TestGetc()
{
    stlStatus      sStatus;
    stlErrorStack  sErrStack;

    stlFile        sFile;
    stlChar        sCh;

    STL_INIT_ERROR_STACK( &sErrStack );

    sStatus = stlOpenFile( &sFile,
                           FILENAME,
                           STL_FOPEN_READ,
                           0,
                           &sErrStack );
    STL_TRY(sStatus == STL_SUCCESS);

    sStatus = stlGetCharacterFile( &sCh, &sFile, &sErrStack );
    STL_TRY(sStatus == STL_SUCCESS);
    
    STL_TRY(TESTSTR[0] == sCh);

    STL_TRY( stlCloseFile( &sFile, &sErrStack ) == STL_SUCCESS );

    return STL_TRUE;

    STL_FINISH;

    PRINTERR;

    return STL_FALSE;
}

/**
 * @brief stlGetStringFile() 함수 테스트
 */
stlBool TestGets()
{
    stlStatus      sStatus;
    stlErrorStack  sErrStack;

    stlFile        sFile;
    stlChar        sStr[256];

    STL_INIT_ERROR_STACK( &sErrStack );

    sStatus = stlOpenFile( &sFile,
                           FILENAME,
                           STL_FOPEN_READ,
                           0,
                           &sErrStack );
    STL_TRY(sStatus == STL_SUCCESS);

    sStatus = stlGetStringFile( sStr, 256, &sFile, &sErrStack );
    STL_TRY( sStatus == STL_SUCCESS );

    /*
    if ( sStatus != STL_SUCCESS )
    {
        STL_TRY( stlGetLastErrorCode( &sErrStack ) == STL_ERRCODE_EOF );
    }
    */
    
    STL_TRY(strcmp(TESTSTR, sStr) == 0);

    STL_TRY( stlCloseFile( &sFile, &sErrStack ) == STL_SUCCESS );

    return STL_TRUE;

    STL_FINISH;

    PRINTERR;

    return STL_FALSE;
}

/**
 * @brief stlGetStringFile() 함수 테스트
 */
stlBool TestGetsBuffered()
{
    stlStatus      sStatus;
    stlErrorStack  sErrStack;

    stlFile        sFile;
    stlChar        sStr[256];

    memset(sStr, 0, 256);

    STL_INIT_ERROR_STACK( &sErrStack );

    sStatus = stlOpenFile( &sFile,
                           FILENAME,
                           STL_FOPEN_READ,
                           0,
                           &sErrStack );
    STL_TRY( sStatus == STL_SUCCESS );

    sStatus = stlGetStringFile( sStr, 256, &sFile, &sErrStack );
    STL_TRY( sStatus == STL_SUCCESS );

    /*
    if ( sStatus != STL_SUCCESS )
    {
        STL_TRY( stlGetLastErrorCode( &sErrStack ) == STL_ERRCODE_EOF );
    }
    */
    STL_TRY(strcmp(TESTSTR, sStr) == 0);

    STL_TRY( stlCloseFile( &sFile, &sErrStack ) == STL_SUCCESS );

    return STL_TRUE;

    STL_FINISH;

    PRINTERR;

    return STL_FALSE;
}

/**
 * @brief check File Contents are equal
 */
stlStatus FileContentsEqual( const stlChar * aFileName,
                             const void    * aExpect,
                             stlSize         aExpectLen )
{
    stlStatus      sStatus;
    stlErrorStack  sErrStack;

    stlFile        sFile;
    stlChar        sActualStr[1024];
    stlSize        sBytes;
    
    STL_INIT_ERROR_STACK( &sErrStack );

    sStatus = stlOpenFile( &sFile,
                           aFileName,
                           STL_FOPEN_READ,
                           0,
                           &sErrStack );
    STL_TRY( sStatus == STL_SUCCESS );
    
    sStatus = stlReadFile( &sFile,
                           sActualStr,
                           1024,
                           &sBytes,
                           &sErrStack );
    STL_TRY( sStatus == STL_SUCCESS );
    /*
    if ( sStatus != STL_SUCCESS )
    {
        STL_TRY( stlGetLastErrorCode( &sErrStack ) == STL_ERRCODE_EOF );
    }
    */

    STL_TRY( memcmp(aExpect, sActualStr, aExpectLen ) == 0 );

    STL_TRY( stlCloseFile( &sFile, &sErrStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    PRINTERR;

    return STL_FAILURE;
}

#define LINE1 "this is a line of text\n"
#define LINE2 "this is a second line of text\n"

/**
 * @brief stlOpenFile() 함수 테스트
 */
stlBool TestPuts()
{
    stlStatus      sStatus;
    stlErrorStack  sErrStack;

    stlFile        sFile;

    const stlChar *sFileName = "data/testluts.txt";

    STL_INIT_ERROR_STACK( &sErrStack );

    sStatus = stlOpenFile( &sFile,
                           sFileName,
                           STL_FOPEN_WRITE | STL_FOPEN_CREATE | STL_FOPEN_TRUNCATE, 
                           STL_FPERM_OS_DEFAULT,
                           &sErrStack );
    STL_TRY(sStatus == STL_SUCCESS);

    sStatus = stlPutStringFile(LINE1, &sFile, &sErrStack );
    STL_TRY(sStatus == STL_SUCCESS);

    sStatus = stlPutStringFile(LINE2, &sFile, &sErrStack );
    STL_TRY(sStatus == STL_SUCCESS);

    STL_TRY( stlCloseFile( &sFile, &sErrStack ) == STL_SUCCESS );

    /*
     * check file contents equal
     */

    sStatus = FileContentsEqual(sFileName,
                                LINE1 LINE2,
                                strlen(LINE1 LINE2));
    STL_TRY(sStatus == STL_SUCCESS);
    
    return STL_TRUE;

    STL_FINISH;

    PRINTERR;

    return STL_FALSE;
}


/**
 * @brief stlReadFile() 함수 테스트
 */
stlBool TestBigRead()
{    
    stlStatus      sStatus;
    stlErrorStack  sErrStack;

    stlFile        sFile;

    stlChar        sBuf[BUFFERSIZE * 2];
    stlSize        sBytes;

    STL_INIT_ERROR_STACK( &sErrStack );
    
    /*
     * Create a test file with known content.
     */

    sStatus = stlOpenFile( &sFile,
                           "data/created_file", 
                           STL_FOPEN_CREATE | STL_FOPEN_WRITE | STL_FOPEN_TRUNCATE, 
                           STL_FPERM_UREAD | STL_FPERM_UWRITE,
                           &sErrStack );
    STL_TRY(sStatus == STL_SUCCESS);

    sBytes = BUFFERSIZE;
    memset(sBuf, 0xFE, sBytes);

    sStatus = stlWriteFile( &sFile,
                            sBuf,
                            sBytes,
                            &sBytes,
                            &sErrStack );
    
    STL_TRY(sStatus == STL_SUCCESS);
    STL_TRY(BUFFERSIZE == sBytes);

    sStatus = stlCloseFile( &sFile, &sErrStack );
    STL_TRY(sStatus == STL_SUCCESS);

    sStatus = stlOpenFile( &sFile,
                           "data/created_file", 
                           STL_FOPEN_READ,
                           0,
                           &sErrStack );
    STL_TRY(sStatus == STL_SUCCESS);

    sBytes = sizeof(sBuf);
    sStatus = stlReadFile( &sFile,
                           sBuf,
                           sBytes,
                           &sBytes,
                           &sErrStack );
    STL_TRY(sStatus == STL_SUCCESS);

    /*
    if ( sStatus != STL_SUCCESS )
    {
        STL_TRY( stlGetLastErrorCode( &sErrStack ) == STL_ERRCODE_EOF );
    }
    */
    STL_TRY(BUFFERSIZE == sBytes);

    sStatus = stlCloseFile( &sFile, &sErrStack );
    STL_TRY(sStatus == STL_SUCCESS);

    sStatus = stlRemoveFile( "data/created_file", &sErrStack );
    STL_TRY(sStatus == STL_SUCCESS);

    return STL_TRUE;

    STL_FINISH;

    PRINTERR;

    return STL_FALSE;
}

/**
 * @brief stlSeekFile() 함수 테스트
 */
stlBool TestModNeg()
{
    stlStatus      sStatus;
    stlErrorStack  sErrStack;

    stlFile        sFile;

    const stlChar *sStr;
    stlInt32       sIdx;
    stlSize        sBytes;
    stlChar        sBuf[8192];
    stlOffset      sCur;
    const stlChar *sFileName = "data/modneg.dat";

    STL_INIT_ERROR_STACK( &sErrStack );
    
    sStatus = stlOpenFile( &sFile,
                           sFileName,
                           STL_FOPEN_CREATE | STL_FOPEN_WRITE,
                           STL_FPERM_UREAD | STL_FPERM_UWRITE,
                           &sErrStack );
    STL_TRY(sStatus == STL_SUCCESS);

    /*
     * write file
     * 
     *    body56789\n                       (   10 bytes)
     *    000000000000.......00000000000    ( 7980 bytes)
     *    end456789\n                       (   10 bytes)
     *    111111111111.......11111111111111 (10000 bytes)
     *
     *    전체 18000 bytes
     */
    
    sStr = "body56789\n";
    sBytes = strlen(sStr);

    sStatus = stlWriteFile( &sFile,
                            (void *)sStr,
                            sBytes,
                            &sBytes,
                            &sErrStack );
    STL_TRY( sStatus == STL_SUCCESS );
    STL_TRY( strlen(sStr) == sBytes );
    
    for (sIdx = 0; sIdx < 7980; sIdx++)
    {
        sStr = "0";
        sBytes = strlen(sStr);

        sStatus = stlWriteFile( &sFile,
                                (void *)sStr,
                                sBytes,
                                &sBytes,
                                &sErrStack );
        STL_TRY( sStatus == STL_SUCCESS );
        STL_TRY( strlen(sStr) == sBytes );
    }
    
    sStr = "end456789\n";
    sBytes = strlen(sStr);

    sStatus = stlWriteFile( &sFile,
                            (void *)sStr,
                            sBytes,
                            &sBytes,
                            &sErrStack );
    STL_TRY( sStatus == STL_SUCCESS );
    STL_TRY( strlen(sStr) == sBytes );

    for (sIdx = 0; sIdx < 10000; sIdx++)
    {
        sStr = "1";
        sBytes = strlen(sStr);

        sStatus = stlWriteFile( &sFile,
                                (void *)sStr,
                                sBytes,
                                &sBytes,
                                &sErrStack );
        STL_TRY( sStatus == STL_SUCCESS );
        STL_TRY( strlen(sStr) == sBytes );
    }
    
    sStatus = stlCloseFile( &sFile, &sErrStack );
    STL_TRY( sStatus == STL_SUCCESS );

    sStatus = stlOpenFile( &sFile,
                           sFileName,
                           STL_FOPEN_READ,
                           0,
                           &sErrStack );
    STL_TRY( sStatus == STL_SUCCESS );

    /*
     * 전체 18000 bytes 중 10 bytes read
     */
    
    sStatus = stlGetStringFile( sBuf,
                                8192,
                                &sFile,
                                &sErrStack );
    STL_TRY(sStatus == STL_SUCCESS);
    STL_TRY( strcmp("body56789\n", sBuf) == 0);
    
    sCur = 0;
    sStatus = stlSeekFile( &sFile,
                           STL_FSEEK_CUR,
                           &sCur,
                           &sErrStack );
    STL_TRY(sStatus == STL_SUCCESS);
    STL_TRY(sCur == 10);

    /*
     * 현재 위치로부터 8192 bytes read
     */
    
    sBytes = sizeof(sBuf);
    sStatus = stlReadFile( &sFile,
                           sBuf,
                           sBytes,
                           &sBytes,
                           &sErrStack );
    STL_TRY(sStatus == STL_SUCCESS);
    STL_TRY(sBytes == sizeof(sBuf));

    /*
     * 현재 위치로부터 다음 위치로 이동
     * 
     *    body56789\n                       (   10 bytes)
     *    000000000000.......00000000000    ( 7980 bytes)
     * => end456789\n                       (   10 bytes)
     */
    
    sCur = -( (stlOffset) sBytes - 7980);
    sStatus = stlSeekFile( &sFile,
                           STL_FSEEK_CUR,
                           &sCur,
                           &sErrStack );
    STL_TRY(sStatus == STL_SUCCESS);
    STL_TRY(sCur == 7990);

    sStatus = stlGetStringFile( sBuf,
                                11,
                                &sFile,
                                &sErrStack );
    STL_TRY(sStatus == STL_SUCCESS);
    STL_TRY(strcmp("end456789\n", sBuf) == 0);
    
    sStatus = stlCloseFile( &sFile, &sErrStack );
    STL_TRY( sStatus == STL_SUCCESS );

    sStatus = stlRemoveFile( sFileName, &sErrStack );
    STL_TRY( sStatus == STL_SUCCESS );

    return STL_TRUE;

    STL_FINISH;

    PRINTERR;

    return STL_FALSE;
}

/**
 * @brief stlSeekFile() 함수 테스트
 */
stlBool TestTruncate()
{
    stlStatus      sStatus;
    stlErrorStack  sErrStack;

    stlFile        sFile;

    const stlChar *sFileName = "data/testtruncate.dat";
    const stlChar *sStr;
    stlSize        sBytes;
    stlFileInfo    sFileInfo;
    
    STL_INIT_ERROR_STACK( &sErrStack );

    (void)stlRemoveFile( sFileName, &sErrStack );

    STL_INIT_ERROR_STACK( &sErrStack );
    
    sStatus = stlOpenFile( &sFile,
                           sFileName,
                           STL_FOPEN_CREATE | STL_FOPEN_WRITE,
                           STL_FPERM_UREAD | STL_FPERM_UWRITE,
                           &sErrStack);
    STL_TRY(sStatus == STL_SUCCESS);
    
    sStr   = "some data";
    sBytes = strlen(sStr);

    sStatus = stlWriteFile( &sFile,
                            (void *)sStr,
                            sBytes,
                            &sBytes,
                            &sErrStack );
    STL_TRY( sStatus == STL_SUCCESS );
    STL_TRY( strlen(sStr) == sBytes );

    sStatus = stlCloseFile( &sFile,
                            &sErrStack );
    STL_TRY( sStatus == STL_SUCCESS );

    sStatus = stlOpenFile( &sFile,
                           sFileName,
                           STL_FOPEN_TRUNCATE | STL_FOPEN_WRITE,
                           STL_FPERM_UREAD | STL_FPERM_UWRITE,
                           &sErrStack );
    STL_TRY( sStatus == STL_SUCCESS );

    sStatus = stlCloseFile( &sFile,
                            &sErrStack );
    STL_TRY( sStatus == STL_SUCCESS );

    sStatus = stlGetFileStatByName( &sFileInfo,
                                    sFileName,
                                    STL_FINFO_SIZE,
                                    &sErrStack );
    STL_TRY( sStatus == STL_SUCCESS );
    STL_TRY( sFileInfo.mSize == 0 );
    
    sStatus = stlRemoveFile( sFileName,
                             &sErrStack );
    STL_TRY(sStatus == STL_SUCCESS);

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
     * TEST stlOpenFile()
     */
    sState = 1;
    STL_TRY(TestOpenNoReadWrite() == STL_TRUE);
    
    /*
     * TEST stlOpenFile()
     */
    sState = 2;
    STL_TRY(TestOpenExcl() == STL_TRUE);

    /*
     * TEST stlOpenFile()
     */
    sState = 3;
    STL_TRY(TestOpenRead() == STL_TRUE);

    /*
     * TEST stlOpenFile()
     */
    sState = 4;
    STL_TRY(TestOpenReadWrite() == STL_TRUE);

    /*
     * TEST stlLinkFile()
     */
    
    sState = 5;
    STL_TRY(TestLinkExisting() == STL_TRUE);

    /*
     * TEST stlLinkFile()
     */
    sState = 6;
    STL_TRY(TestNonLinkExisting() == STL_TRUE);

    /*
     * TEST stlReadFile()
     */
    sState = 7;
    STL_TRY(TestRead() == STL_TRUE);
    
    /*
     * TEST stlReadFile()
     */
    sState = 8;
    STL_TRY(TestReadZero() == STL_TRUE);

    /*
     * TEST stlSeekFile()
     */
    sState = 9;
    STL_TRY(TestSeek() == STL_TRUE);

    /*
    sState = 10;
    STL_TRY(TestFileName() == STL_TRUE);
    */
    
    /*
     * TEST stlCloseFile()
     */
    sState = 11;
    STL_TRY(TestFileClose() == STL_TRUE);

    /*
     * TEST stlRemoveFile()
     */
    sState = 12;
    STL_TRY(TestFileRemove() == STL_TRUE);

    /*
     * TEST stlOpenFile()
     */
    sState = 13;
    STL_TRY(TestOpenWrite() == STL_TRUE);

    /*
     * TEST stlOpenFile()
     */
    sState = 14;
    STL_TRY(TestOpenWriteCreate() == STL_TRUE);

    /*
     * TEST stlWriteFile()
     */
    sState = 15;
    STL_TRY(TestWrite() == STL_TRUE);

    sState = 18;
    STL_TRY(TestGetc() == STL_TRUE);

    /*
     * TEST stlGetStringFile()
     */
    sState = 20;
    STL_TRY(TestGets() == STL_TRUE);

    /*
     * TEST stlOpenFile()
     */
    sState = 21;
    STL_TRY(TestGetsBuffered() == STL_TRUE);
    
    /*
     * TEST stlPutStringFile()
     */
    sState = 22;
    STL_TRY(TestPuts() == STL_TRUE);
    
    /*
     * TEST stlReadFile()
     */
    sState = 27;
    STL_TRY(TestBigRead() == STL_TRUE);

    /*
     * TEST stlSeekFile()
     */
    sState = 28;
    STL_TRY(TestModNeg() == STL_TRUE);

    /*
     * TEST stlTruncateFile()
     */
    sState = 29;
    STL_TRY(TestTruncate() == STL_TRUE);

    stlPrintf("PASS");
    
    return 0;

    STL_FINISH;

    stlPrintf("FAILURE %d", sState);

    return (-1);
}
