#include <sml.h>

stlInt32 main( stlInt32    aArgc,
               stlChar  ** aArgv )
{
    smlEnv    sEnv;
    stlInt32  sState = 0;
    stlInt32  sIdx;
    
    STL_TRY( smlInitialize() == STL_SUCCESS );

    STL_TRY( smlInitializeEnv( &sEnv ) == STL_SUCCESS );
    
    STL_TRY( smlTestWarmup( &sEnv ) == STL_SUCCESS );
    sState = 1;

    for( sIdx = 0; sIdx < 5; sIdx++ )
    {
        STL_TRY( knlAddEnvEvent( SML_EVENT_CHECKPOINT,
                                 NULL,
                                 0,
                                 SML_CHECKPOINT_ENV_ID,
                                 STL_FALSE,
                                 KNL_ENV( &sEnv ) ) == STL_SUCCESS );

        stlSleep( STL_SET_SEC_TIME( 1 ) );
    }

    sState = 0;
    STL_TRY( smlTestCooldown( &sEnv ) == STL_SUCCESS );

    return STL_SUCCESS;
    
    STL_FINISH;

    stlTestPrintErrorStack( KNL_ERROR_STACK( &sEnv ) );

    if( sState == 1 )
    {
        (void)smlTestCooldown( &sEnv );
    }
    
    return STL_FAILURE;
}
