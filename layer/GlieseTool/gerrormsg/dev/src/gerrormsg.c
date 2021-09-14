#include <dtl.h>
#include <goldilocks.h>

/**
 * @file gerrormsg.c
 * @brief gerrormsg source
 */

extern stlErrorRecord gStandardErrorTable[];
extern stlErrorRecord gDataTypeErrorTable[];
extern stlErrorRecord gKernelErrorTable[];
extern stlErrorRecord gStorageManagerErrorTable[];
extern stlErrorRecord gExecutionLibraryErrorTable[];
extern stlErrorRecord gSqlProcessorLibraryErrorTable[];
/* psm */
extern stlErrorRecord gSessionErrorTable[];
extern stlErrorRecord gGlieseLibraryErrorTable[];
extern stlErrorRecord gZlplErrorTable[];    /* goldilocksesql */
extern stlErrorRecord gCommunicationErrorTable[];/* communication    */
extern stlErrorRecord gServerLibraryErrorTable[];/* ServerLibrary    */
extern stlErrorRecord gZtqErrorTable[];     /* gsql */
extern stlErrorRecord gZtpErrorTable[];     /* gpec */
extern stlErrorRecord gZtdErrorTable[];     /* gloader */
/* gmigrator */
extern stlErrorRecord gZtmErrorTable[];     /* gmaster */
extern stlErrorRecord gZtleErrorTable[];    /* glsnr */
extern stlErrorRecord gZtcErrorTable[];     /* cyclone */
extern stlErrorRecord gZtrErrorTable[];     /* logmirror */
extern stlErrorRecord gZtnErrorTable[];     /* gcymon */
extern stlErrorRecord gZtseErrorTable[];    /* gserver */
extern stlErrorRecord gZtieErrorTable[];    /* gdispatcher */
extern stlErrorRecord gZtbeErrorTable[];    /* gbalancer */
extern stlErrorRecord gServerCommunicationErrorTable[]; /* Servercommunication */


/**
 * gmaster는 error table없음.
 *
 */

/**
 * 주어진 zteErrCodePathCtrl 구조체를 첫 번째 layer의 첫 번째 error code로 초기화 한다.
 */
#define ZTE_PATHCTRL_INITIALIZATION( aPathCtrl )             \
{                                                            \
    (aPathCtrl).mErrorModule = STL_ERROR_MODULE_STANDARD;    \
    (aPathCtrl).mErrorNO = 0;                                \
}

/**
 * 주어진 zteErrorRecord 구조체를 초기화한다.
 */
#define ZTE_ERROR_RECORD_INITIALIZATION( aErrorRecord )                 \
    stlMemset( &(aErrorRecord), 0x00, STL_SIZEOF( zteErrorRecord ) );

/**
 * gLayerErrorTables에서 aPathCtrl이 가리키는 target error code를 넘겨준다.
 */
#define ZTE_GET_TARGET_ERROR( aPathCtrl, aTargetError )                 \
    (aTargetError) = &gLayerErrorTables[(aPathCtrl)->mErrorModule][(aPathCtrl)->mErrorNO];

/**
 * @brief error code의 위치를 저장할 구초제
 */
typedef struct zteErrorCodePathCtrl
{
    stlErrorModule  mErrorModule;
    stlInt32        mErrorNO;
}zteErrorCodePathCtrl;

/**
 * @brief output error record를 저장하는 구조체
 */
typedef struct zteErrorRecord
{
    stlChar     *mErrorMessage;
    stlInt32     mNativeErrorCode;
    stlChar      mSQLStateCode[STL_SQLSTATE_LENGTH + 1];
}zteErrorRecord;

/**
 * @brief Error table initialize
 * @return 성공시 STL_SUCCESS반환
 */
stlStatus zteInitialize()
{
    stlRegisterErrorTable( STL_ERROR_MODULE_STANDARD,
                           gStandardErrorTable );
    stlRegisterErrorTable( STL_ERROR_MODULE_DATATYPE,
                           gDataTypeErrorTable );
    stlRegisterErrorTable( STL_ERROR_MODULE_KERNEL,
                           gKernelErrorTable );
    stlRegisterErrorTable( STL_ERROR_MODULE_STORAGE_MANAGER,
                           gStorageManagerErrorTable );
    stlRegisterErrorTable( STL_ERROR_MODULE_EXECUTION_LIBRARY,
                           gExecutionLibraryErrorTable);
    stlRegisterErrorTable( STL_ERROR_MODULE_SQL_PROCESSOR,
                           gSqlProcessorLibraryErrorTable );
    stlRegisterErrorTable( STL_ERROR_MODULE_SESSION,
                           gSessionErrorTable);
    stlRegisterErrorTable( STL_ERROR_MODULE_GLIESE_LIBRARY_AODBC,
                           gGlieseLibraryErrorTable);
    stlRegisterErrorTable( STL_ERROR_MODULE_GLIESE_LIBRARY_GOLDILOCKSESQL,
                           gZlplErrorTable);
    stlRegisterErrorTable( STL_ERROR_MODULE_COMMUNICATION,
                           gCommunicationErrorTable);
    stlRegisterErrorTable( STL_ERROR_MODULE_SERVER_LIBRARY,
                           gServerLibraryErrorTable);
    stlRegisterErrorTable( STL_ERROR_MODULE_GLIESE_TOOL_GSQL,
                           gZtqErrorTable);
    stlRegisterErrorTable( STL_ERROR_MODULE_GLIESE_TOOL_GPEC,
                           gZtpErrorTable);
    stlRegisterErrorTable( STL_ERROR_MODULE_GLIESE_TOOL_GLOADER,
                           gZtdErrorTable);
    stlRegisterErrorTable( STL_ERROR_MODULE_GLIESE_TOOL_GLISTENER,
                           gZtleErrorTable);
    stlRegisterErrorTable( STL_ERROR_MODULE_GLIESE_TOOL_CYCLONE,
                           gZtcErrorTable);
    stlRegisterErrorTable( STL_ERROR_MODULE_GLIESE_TOOL_LOGMIRROR,
                           gZtrErrorTable);
    stlRegisterErrorTable( STL_ERROR_MODULE_GLIESE_TOOL_CYMON,
                           gZtnErrorTable);
    stlRegisterErrorTable( STL_ERROR_MODULE_GLIESE_TOOL_GSERVER,
                           gZtseErrorTable);
    stlRegisterErrorTable( STL_ERROR_MODULE_GLIESE_TOOL_GDISPATCHER,
                           gZtieErrorTable);
    stlRegisterErrorTable( STL_ERROR_MODULE_GLIESE_TOOL_GBALANCER,
                           gZtbeErrorTable);
    stlRegisterErrorTable( STL_ERROR_MODULE_SERVER_COMMUNICATION,
                           gServerCommunicationErrorTable);

    return STL_SUCCESS;
}

/**
 * @brief zteGetErrorCode()는 지정된 위치의 error record를 반환한다.
 * @param[out]    aErrorRecord 반환되는 error record
 * @param[in,out] aPathCtrl    반환 받을 error record의 위치, 자동으로 다음 error record의 위치값으로 변경된다. 모든 error record를 반환하고 더이상 반환할 값이 없다면 STL_ERROR_MODULE_MAX 값을 가지게 된다.
 * @return 성공시 STL_SUCCESS반환
 */
stlStatus zteGetErrorCode( zteErrorRecord               *aErrorRecord,
                           zteErrorCodePathCtrl         *aPathCtrl )
{
    zteErrorCodePathCtrl     *sPathCtrl        = NULL;
    zteErrorRecord           *sErrorRecord     = NULL;
    stlErrorRecord           *sTargetError     = NULL;

    STL_ASSERT( aErrorRecord != NULL );
    STL_ASSERT( aPathCtrl    != NULL );

    sPathCtrl    = aPathCtrl;
    sErrorRecord = aErrorRecord;
    
    /**
     * 잘못된 error code의 위치
     */
    STL_TRY( ( ( sPathCtrl->mErrorModule > STL_ERROR_MODULE_NONE ) &&
               (sPathCtrl->mErrorModule < STL_ERROR_MODULE_MAX ) ) );

    /**
     * error code의 값을 구성한다.
     */
    ZTE_GET_TARGET_ERROR( sPathCtrl, sTargetError );
    
    stlMakeSqlState( sTargetError->mExternalErrorCode,
                     sErrorRecord->mSQLStateCode );
    sErrorRecord->mErrorMessage    = sTargetError->mErrorMessage;
    sErrorRecord->mNativeErrorCode = STL_MAKE_ERRCODE( sPathCtrl->mErrorModule,
                                                       sPathCtrl->mErrorNO );

    /**
     * error code의 위치를 재설정한다.
     */
    sPathCtrl->mErrorNO++;
    ZTE_GET_TARGET_ERROR( sPathCtrl, sTargetError );

    /**
     * 만약, layer에 더 이상 error code가 존재하지 않다면 layer를 이동한다.
     */
    if( sTargetError->mErrorMessage == NULL )
    {
        sPathCtrl->mErrorNO = 0;
        do
        {
            sPathCtrl->mErrorModule++;

            /**
             * 더 이상 이동할 layer가 없다면 종료한다. 
             */
            if( sPathCtrl->mErrorModule == STL_ERROR_MODULE_MAX )
            {
                break;
            }
            else
            {
                /* Do Nothing */
            }

            ZTE_GET_TARGET_ERROR( sPathCtrl, sTargetError );
        } while( sTargetError == NULL );
    }

    return STL_SUCCESS;

    STL_FINISH;

    /**
     * 현재 STL_FAILURE를 반환하는 상황은 잘못된 위치의 error code를 요구할 때이다.
     * 이 경우, 개발자의 실수이기 때문에 assert한다.
     */
    STL_ASSERT( 0 );
    
    return STL_FAILURE;
}

/**
 * @brief gerrormsg Main 함수
 * @return 성공여부
 */
int main( int argc, char *argv[] )
{
    zteErrorCodePathCtrl   sPathCtrl;
    zteErrorRecord         sErrorRecord;
    
    /**
     * 사용할 구조체들을 초기화한다.
     */
    ZTE_PATHCTRL_INITIALIZATION( sPathCtrl );
    ZTE_ERROR_RECORD_INITIALIZATION( sErrorRecord );

    /**
     * 각 레이어의 error code를 구성한다.
     */
    STL_TRY( zteInitialize() == STL_SUCCESS );

    stlPrintf( "ERROR_CODE SQL_STATE MESSAGE\n" );
    stlPrintf( "---------- --------- -----------------------------------------------\n" );

    /**
     * 모든 레이어의 error code를 출력한다.
     */
    do
    {
        STL_TRY( zteGetErrorCode( &sErrorRecord,
                                  &sPathCtrl ) == STL_SUCCESS );

        if( sErrorRecord.mErrorMessage != NULL )
        {
            stlPrintf( "%10d%10s %s\n",
                       sErrorRecord.mNativeErrorCode,
                       sErrorRecord.mSQLStateCode,
                       sErrorRecord.mErrorMessage );
        }
    }while( sPathCtrl.mErrorModule != STL_ERROR_MODULE_MAX );

    stlPrintf( "\n" );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
