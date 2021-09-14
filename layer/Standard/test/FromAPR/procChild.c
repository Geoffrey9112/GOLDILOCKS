#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stl.h>

#define PRINTDEBUG
// #define PRINTDEBUG  fprintf( stderr, "%s:%d\n", __FILE__, __LINE__ );

stlInt32 main()
{
    stlChar  sBuf[256];
    stlSize  sLength;

    stlFile  sStdInFile;
    stlFile  sStdOutFile;

    stlErrorStack sErrStack;
    
    STL_INIT_ERROR_STACK( &sErrStack );

    PRINTDEBUG;
    STL_TRY( stlOpenStdinFile( &sStdInFile,
                               &sErrStack )
             == STL_SUCCESS );
    
    PRINTDEBUG;
    STL_TRY( stlOpenStdoutFile( &sStdOutFile,
                                &sErrStack )
             == STL_SUCCESS );

    PRINTDEBUG;

    /*
     * STDIN 으로부터 읽은 내용을 그대로 STDOUT으로 쓴다.
     */
    sLength = 256;
    STL_TRY( stlReadFile( &sStdInFile,
                          sBuf,
                          sLength,
                          &sLength,
                          &sErrStack )
             == STL_SUCCESS );
    PRINTDEBUG;
    
    if (sLength > 0)
    {
        STL_TRY( stlWriteFile( &sStdOutFile,
                               sBuf,
                               sLength,
                               &sLength,
                               &sErrStack )
             == STL_SUCCESS );
    }
    PRINTDEBUG;

    return 0;  /* just to keep the compiler happy */

    STL_FINISH;

    return -1;
}
