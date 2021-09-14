#include <stl.h>

#define TEST_BUFFER_SIZE 4

stlErrorStack gErrorStack;

#define PRINTPASS stlPrintf( "  => PASS\n" );
#define PRINTFAIL stlPrintf( "  => FAIL\n" );
#define PRINTERR( aErrorStack )                             \
    do                                                      \
    {                                                       \
        stlPrintf( "ERRCODE: %d, ERRSTRING: %s\n",          \
                   stlGetLastErrorCode( aErrorStack ),      \
                   stlGetLastErrorMessage( aErrorStack ) ); \
    } while(0)

#define NAME_SIZE         100
#define INVALID_NAME_SIZE 3
#define INVALID_DIR_SIZE  3

stlInt32 main( stlInt32 aArgc, stlChar** aArgv )
{
    stlInt32   sState = 0;
    stlUserID  sUserID;
    stlGroupID sGroupID;
    stlUserID  sUserID2;
    stlGroupID sGroupID2;
    stlChar    sName[NAME_SIZE];
    stlChar    sDirName[NAME_SIZE];
    
    STL_TRY( stlInitialize() == STL_SUCCESS );
    sState = 1;

    STL_INIT_ERROR_STACK( &gErrorStack );

    stlPrintf("1. Test User\n");
    STL_TRY( stlGetCurrentUserID( &sUserID,
                                  &sGroupID,
                                  &gErrorStack )
             == STL_SUCCESS );
    STL_TRY( stlGetUserNameByUserID( sName,
                                     NAME_SIZE,
                                     sUserID,
                                     &gErrorStack )
             == STL_SUCCESS );
    STL_TRY( stlGetUserIDByUserName( &sUserID2,
                                     &sGroupID2,
                                     sName,
                                     &gErrorStack )
             == STL_SUCCESS );
    STL_TRY( stlCompareUserID( sUserID, sUserID2 ) == STL_TRUE );
    PRINTPASS;
    
    stlPrintf("2. Test Group\n");
    STL_TRY( stlGetCurrentUserID( &sUserID,
                                  &sGroupID,
                                  &gErrorStack )
             == STL_SUCCESS );
    STL_TRY( stlGetGroupNameByGroupID( sName,
                                       NAME_SIZE,
                                       sGroupID,
                                       &gErrorStack )
             == STL_SUCCESS );
    STL_TRY( stlGetGroupIDByGroupName( &sGroupID2,
                                       sName,
                                       &gErrorStack )
             == STL_SUCCESS );
    STL_TRY( stlCompareGroupID( sGroupID, sGroupID2 ) == STL_TRUE );
    PRINTPASS;
    
    stlPrintf("3. Test Abnormal User\n");
    STL_TRY( stlGetUserIDByUserName( &sUserID2,
                                     &sGroupID2,
                                     "@@@@@@@@",
                                     &gErrorStack )
             != STL_SUCCESS );
    PRINTERR( &gErrorStack );
    STL_TRY( stlGetCurrentUserID( &sUserID,
                                  &sGroupID,
                                  &gErrorStack )
             == STL_SUCCESS );
    STL_TRY( stlGetUserNameByUserID( sName,
                                     INVALID_NAME_SIZE,
                                     sUserID,
                                     &gErrorStack )
             != STL_SUCCESS );
    PRINTERR( &gErrorStack );
    STL_TRY( stlGetUserNameByUserID( sName,
                                     NAME_SIZE,
                                     sUserID,
                                     &gErrorStack )
             == STL_SUCCESS );
    STL_TRY( stlGetHomeDirByUserName( sDirName,
                                      INVALID_DIR_SIZE,
                                      sName,
                                      &gErrorStack )
             != STL_SUCCESS );
    PRINTERR( &gErrorStack );
    PRINTPASS;
    
    stlPrintf("4. Test Abnormal Group\n");
    STL_TRY( stlGetGroupIDByGroupName( &sGroupID2,
                                       "@@@@@@@@",
                                       &gErrorStack )
             != STL_SUCCESS );
    PRINTERR( &gErrorStack );
    STL_TRY( stlGetCurrentUserID( &sUserID,
                                  &sGroupID,
                                  &gErrorStack )
             == STL_SUCCESS );
    STL_TRY( stlGetGroupNameByGroupID( sName,
                                       INVALID_NAME_SIZE,
                                       sGroupID,
                                       &gErrorStack )
             != STL_SUCCESS );
    PRINTERR( &gErrorStack );
    PRINTPASS;
    
    sState = 0;
    stlTerminate();

    return STL_SUCCESS;

    STL_FINISH;

    PRINTERR( &gErrorStack );
    
    switch( sState )
    {
        case 1:
            stlTerminate();
        default:
            break;
    }

    PRINTFAIL;

    return STL_FAILURE;
}
