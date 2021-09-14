#include <stl.h>

stlErrorStack gErrorStack;

stlShm gShm;

stlInt32 main( stlInt32 aArgc, stlChar** aArgv )
{
    stlInt32  sState = 0;
    stlChar * sBuffer;
    
    STL_TRY( stlInitialize() == STL_SUCCESS );
    sState = 1;

    STL_INIT_ERROR_STACK( &gErrorStack );

    STL_TRY( stlAttachShm( &gShm,
                           "__SHM",
                           1000000,
                           (void*)0x80000000,
                           &gErrorStack ) == STL_SUCCESS );

    sBuffer = stlGetShmBaseAddr( &gShm );

    stlStrncpy( sBuffer, "This is a test", 100 );

    STL_TRY( stlDetachShm( &gShm,
                           &gErrorStack ) == STL_SUCCESS );

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
