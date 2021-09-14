#include <stdio.h>
#include <stl.h>

#define PRINTERR                                            \
    do                                                      \
    {                                                       \
        stlPrintf( "ERRCODE: %d, ERRSTRING: %s\n",          \
                   stlGetLastErrorCode( &sErrStack ),       \
                   stlGetLastErrorMessage( &sErrStack ) );  \
    } while(0)

/**
 * @brief stlGetCurrentUserID() 함수 테스트
 */
stlBool TestUidCurrent()
{
    stlUserID  sUid;
    stlGroupID sGid;

    stlErrorStack  sErrStack;
    
    STL_INIT_ERROR_STACK( &sErrStack );
    
    STL_TRY( stlGetCurrentUserID( &sUid,
                                  &sGid,
                                  &sErrStack )
             == STL_SUCCESS);

    return STL_TRUE;

    STL_FINISH;

    PRINTERR;
    
    return STL_FALSE;
}

/**
 * @brief stlGetUserNameByUserID() 함수 테스트
 */
stlBool TestUserName()
{
    stlUserID    sUid;
    stlGroupID   sGid;
    stlUserID    sRetreivedUid;
    stlGroupID   sRetreivedGid;
    stlChar      sUserName[256];

    stlErrorStack  sErrStack;
    
    STL_INIT_ERROR_STACK( &sErrStack );
    
    STL_TRY( stlGetCurrentUserID( &sUid,
                                  &sGid,
                                  &sErrStack )
             == STL_SUCCESS);

    STL_TRY( stlGetUserNameByUserID( sUserName,
                                     256,
                                     sUid,
                                     &sErrStack )
            == STL_SUCCESS);
                      
    STL_TRY( stlGetUserIDByUserName( &sRetreivedUid,
                                     &sRetreivedGid,
                                     sUserName,
                                     &sErrStack )
             == STL_SUCCESS);

    STL_TRY( stlCompareUserID( sUid, sRetreivedUid ) == STL_TRUE );
    STL_TRY( stlCompareGroupID( sGid, sRetreivedGid ) == STL_TRUE );

    return STL_TRUE;

    STL_FINISH;

    PRINTERR;
    
    return STL_FALSE;
}

/**
 * @brief stlGetGroupName() 함수 테스트
 */
stlBool TestGroupName()
{
    stlUserID    sUid;
    stlGroupID   sGid;
    stlGroupID   sRetreivedGid;
    stlChar      sGroupName[256];

    stlErrorStack  sErrStack;
    
    STL_INIT_ERROR_STACK( &sErrStack );
    
    STL_TRY( stlGetCurrentUserID( &sUid,
                                  &sGid,
                                  &sErrStack )
             == STL_SUCCESS);

    STL_TRY( stlGetGroupNameByGroupID( sGroupName,
                                       256,
                                       sGid,
                                       &sErrStack )
             == STL_SUCCESS);

    STL_TRY( stlGetGroupIDByGroupName( &sRetreivedGid,
                                       sGroupName,
                                       &sErrStack )
             == STL_SUCCESS);

    STL_TRY( stlCompareGroupID( sGid, sRetreivedGid ) == STL_TRUE );

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
     * Test UidCurrent
     */
    sState = 1;
    STL_TRY(TestUidCurrent() == STL_TRUE);

    /*
     * Test UserName
     */
    sState = 2;
    STL_TRY(TestUserName() == STL_TRUE);

    /*
     * Test GroupName
     */
    sState = 3;
    STL_TRY(TestGroupName() == STL_TRUE);

    stlPrintf("PASS");

    return (0);

    STL_FINISH;

    stlPrintf("FAILURE %d", sState);

    return (-1);
}
