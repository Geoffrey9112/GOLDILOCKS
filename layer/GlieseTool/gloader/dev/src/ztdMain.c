/*******************************************************************************
 * ztdMain.c
 *
 * Copyright (c) 2012, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id$
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#include <stl.h>
#include <dtl.h>
#include <goldilocks.h>
#include <ztdDef.h>
#include <ztdMain.h>
#include <ztdExecute.h>
#include <ztdExecThread.h>
#include <ztdDisplay.h>
#include <ztdFileExecute.h>
#include <ztdCmdParser.h>
#include <ztdBinaryMode.h>
#include <ztdBinaryConsumerThread.h>

extern stlErrorRecord   gZtdErrorTable[];

dtlCharacterSet         gZtdCharacterSet            = DTL_CHARACTERSET_MAX;
dtlCharacterSet         gZtdDatabaseCharacterSet    = DTL_CHARACTERSET_MAX;

stlInt64                gZtdOdbcVersion             = 3;
stlBool                 gZtdRunState                = STL_TRUE;

stlInt32                gZtdBinaryFileFormatVersion = 1;
stlUInt32               gZtdErrorCount              = 0;
stlUInt32               gZtdMaxErrorCount           = ZTD_DEFAULT_MAX_ERROR_COUNT;

stlBool                 gZtdBindDATEAsTIMESTAMP     = STL_FALSE;

ztdModeType             gZtdModeType                = ZTD_MODE_TYPE_INIT;
ztdFormatType           gZtdFormatType              = ZTD_FORMAT_TYPE_TEXT;
ztdManager              gZtdManager;

stlInt32                gZtdUserNameIdx = 0;
stlInt32                gZtdPasswordIdx = 0;

/**
 * @see ztdModeType
 */ 
const stlChar *const gZtdModeString[] =
{
    "INIT",
    "EXPORT",
    "IMPORT",
    NULL
};

/**
 * @brief ztdCloseDatebase()는 Database를 Close한다.
 * @param[in]  aEnvHandle    Environmet Handle
 * @param[in]  aDbcHandle    Dbc Handle
 * @param[in]  aErrorStack   error stack
 */ 
stlStatus ztdCloseDatabase( SQLHENV         aEnvHandle,
                            SQLHDBC         aDbcHandle,
                            stlBool         aIsRollBack,
                            stlErrorStack * aErrorStack )
{
    stlInt32 sState = 3;

    /**
     * End Transaction.
     */
    if( aIsRollBack == STL_TRUE )
    {
        STL_TRY( SQLEndTran( SQL_HANDLE_DBC,
                             aDbcHandle,
                             SQL_ROLLBACK )
                 == SQL_SUCCESS );
    }
    else
    {
        STL_TRY( SQLEndTran( SQL_HANDLE_DBC,
                             aDbcHandle,
                             SQL_COMMIT )
                 == SQL_SUCCESS );
    }
    
    sState = 2;
    STL_TRY( SQLDisconnect( aDbcHandle )
             == SQL_SUCCESS );
    
    sState = 1;
    STL_TRY( SQLFreeHandle( SQL_HANDLE_DBC,
                            aDbcHandle )
             == SQL_SUCCESS );
    
    sState = 0;
    STL_TRY( SQLFreeHandle( SQL_HANDLE_ENV,
                            aEnvHandle )
             == SQL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 3:
            (void) SQLDisconnect( aDbcHandle );
        case 2:
            (void) SQLFreeHandle( SQL_HANDLE_DBC, aDbcHandle );
        case 1:
            (void) SQLFreeHandle( SQL_HANDLE_ENV, aEnvHandle );
        default:
            break;
    }
    return STL_FAILURE;
             
}

/**
 * @brief ztdOpeneDatebase()는 Database를 Open한다.
 * @param[in]   aEnvHandle       Environmet Handle
 * @param[in]   aDbcHandle       Dbc Handle
 * @param[in]   aInputArguments  Database를 open할 때 필요한 정보를 갖는 struct
 * @param[in]   aCharacterSet    client character set
 * @param[out]  aErrorStack      error stack
 */ 
stlStatus ztdOpenDatabase( SQLHENV           * aEnvHandle,
                           SQLHDBC           * aDbcHandle,
                           ztdInputArguments * aInputArguments,
                           stlChar           * aCharacterSet,
                           stlErrorStack     * aErrorStack )
{
    SQLRETURN       sReturn;
    stlInt32        sState = 0;
    stlChar         sCharacterSet[STL_MAX_SQL_IDENTIFIER_LENGTH];
    SQLINTEGER      sStringLength = 0;
    
    STL_TRY_THROW( SQLAllocHandle( SQL_HANDLE_ENV,
                                   NULL,
                                   aEnvHandle )
                   == SQL_SUCCESS,
                   RAMP_ERR_SERVICE_NOT_AVAILABLE );
    sState = 1;
    
    STL_TRY( SQLSetEnvAttr( *aEnvHandle,
                            SQL_ATTR_ODBC_VERSION,
                            (SQLPOINTER)(stlVarInt)gZtdOdbcVersion,
                            0 )
             == SQL_SUCCESS );

    STL_TRY( SQLAllocHandle( SQL_HANDLE_DBC,
                             *aEnvHandle,
                             aDbcHandle )
             == SQL_SUCCESS );
    sState = 2;
    
    sReturn = SQLSetConnectAttr( *aDbcHandle,
                                 SQL_ATTR_CHARACTER_SET,
                                 aCharacterSet,
                                 SQL_NTS );

    STL_TRY_THROW( SQL_SUCCEEDED( sReturn ), RAMP_ERR_CONNECTION );

    STL_TRY( dtlGetCharacterSet( aCharacterSet,
                                 &gZtdCharacterSet,
                                 aErrorStack ) == STL_SUCCESS );

    sReturn = SQLConnect( *aDbcHandle,
                          (SQLCHAR*)  aInputArguments->mDsn,//"GLIESE",
                          SQL_NTS,
                          (SQLCHAR *) aInputArguments->mId,
                          SQL_NTS,
                          (SQLCHAR *) aInputArguments->mPassword,
                          SQL_NTS );

    STL_TRY_THROW( SQL_SUCCEEDED( sReturn ), RAMP_ERR_CONNECTION );

    sReturn = SQLGetConnectAttr( *aDbcHandle,
                                 SQL_ATTR_DATABASE_CHARACTER_SET,
                                 sCharacterSet,
                                 STL_MAX_SQL_IDENTIFIER_LENGTH,
                                 &sStringLength );

    STL_TRY_THROW( SQL_SUCCEEDED( sReturn ), RAMP_ERR_CONNECTION );

    STL_TRY( dtlGetCharacterSet( sCharacterSet,
                                 &gZtdDatabaseCharacterSet,
                                 aErrorStack ) == STL_SUCCESS );

    STL_TRY( SQLSetConnectAttr( *aDbcHandle,
                                SQL_ATTR_AUTOCOMMIT,
                                (SQLPOINTER) SQL_AUTOCOMMIT_OFF,
                                SQL_IS_UINTEGER )
             == SQL_SUCCESS );

    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_SERVICE_NOT_AVAILABLE )
    {
        (void)stlPushError( STL_ERROR_LEVEL_ABORT,
                            ZTD_ERRCODE_SERVICE_NOT_AVAILABLE,
                            NULL,
                            aErrorStack );
    }

    STL_CATCH( RAMP_ERR_CONNECTION )
    {
        (void)ztdPrintDiagnostic( SQL_HANDLE_DBC,
                                  *aDbcHandle,
                                  aErrorStack );
    }
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void) SQLFreeHandle( SQL_HANDLE_DBC,
                                  *aDbcHandle );
        case 1:
            (void) SQLFreeHandle( SQL_HANDLE_ENV,
                                  *aEnvHandle );
        default:
            break;
    }
    return STL_FAILURE;
}

/**
 * @brief ztdParseControl()는 Control File의 contents를 struct ztdControlInfo에 저장한다.
 * @param [in]   aFileAndBuffer  ztdFileAndBuffer: File pointer와 log string.
 * @param [out]  aControlInfo    Control File 내용을 저장할 구조체
 * @param [in]   aAllocator      Region Memory Allocator
 * @param [out]  aErrorStack     error stack
 */
stlStatus ztdParseControl( ztdFileAndBuffer * aFileAndBuffer,
                           ztdControlInfo   * aControlInfo,
                           stlAllocator     * aAllocator, 
                           stlErrorStack    * aErrorStack )
{
    stlFileInfo       sFileInfo;
    stlChar         * sReadBuffer;
    stlSize           sReadSize = 0;
    dtlCharacterSet   sCharacterSet = DTL_CHARACTERSET_MAX;
    
    STL_TRY( stlGetFileStatByHandle( &sFileInfo,
                                     STL_FINFO_SIZE,
                                     &(aFileAndBuffer->mControlFile),
                                     aErrorStack )
             == STL_SUCCESS );
                                   
    STL_TRY( stlAllocRegionMem( aAllocator,
                                sFileInfo.mSize + 1,
                                (void **) &sReadBuffer,
                                aErrorStack )
             == STL_SUCCESS );
                                
    STL_TRY( stlReadFile( &(aFileAndBuffer->mControlFile),
                          sReadBuffer,
                          sFileInfo.mSize,
                          &sReadSize,
                          aErrorStack ) == STL_SUCCESS);
    
    sReadBuffer[sReadSize] = ZTD_CONSTANT_NULL;
    
    STL_TRY( ztdCmdParseIt( sReadBuffer,
                            aAllocator,
                            aControlInfo,
                            aErrorStack )
             == STL_SUCCESS );

    if( aControlInfo->mCharacterSet[0] == '\0' )
    {
        STL_TRY( dtlGetSystemCharacterset( &sCharacterSet,
                                           aErrorStack ) == STL_SUCCESS );

        stlStrcpy( aControlInfo->mCharacterSet, gDtlCharacterSetString[sCharacterSet] );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief ztdInitInputArguments: ztdInputArguments 을 초기화 한다.
 * @param [out]    aInputArguments   argv 내용이 담길 struct
 * @param [in/out] aErrorStack       error stack
 */ 
stlStatus ztdInitInputArguments( ztdInputArguments * aInputArguments,
                                 stlErrorStack     * aErrorStack )
{
    stlMemset( aInputArguments, 0x00, STL_SIZEOF( ztdInputArguments ) );

    aInputArguments->mArraySize   = ZTD_RECORD_ARRAY_SIZE;
    aInputArguments->mCommitUnit  = ZTD_COMMIT_FREQUENCY;
    aInputArguments->mThreadUnit  = 1;
    aInputArguments->mDisplayUnit = ZTD_COMMIT_FREQUENCY;
    aInputArguments->mDirectIo    = STL_TRUE;
    
    stlStrcpy( aInputArguments->mDsn, ZTD_DEFAULT_DSN );
    
    return STL_SUCCESS;
}

/**
 * @brief ztdParseOptions: command option을 분석.
 * @param [in]     aArgc             argument count
 * @param [in]     aArgv             argument vector
 * @param [out]    aInputArguments   argv 내용이 담길 struct
 * @param [in/out] aErrorStack       error stack
 */ 
stlStatus ztdParseOptions( stlInt32            aArgc,
                           stlChar           * aArgv[],
                           ztdInputArguments * aInputArguments,
                           stlErrorStack     * aErrorStack )
{
    stlGetOpt       sOpt;
    stlBool         sInvalidOption = STL_FALSE;
    stlChar         sCh;
    stlChar       * sSep;
    const stlChar * sOptionArg;
    stlInt32        sIdxArgv;
    stlBool         sNoDisplayCopyright = STL_FALSE;
    stlUInt64       sNum64;
    
    stlGetOptOption sOptOption[] =
        {
            { "export",          'e',  STL_FALSE, "export data" },
            { "import",          'i',  STL_FALSE, "import data" },
            { "control",         'c',  STL_TRUE,  "control file" },
            { "data",            'd',  STL_TRUE,  "data file" },
            { "format",          'f',  STL_TRUE,  "file format (Default text)" },
            { "log",             'l',  STL_TRUE,  "log file" },
            { "bad",             'b',  STL_TRUE,  "bad file" },
            { "dsn",             'D',  STL_TRUE,  "dsn string" },
            { "array",           'a',  STL_TRUE,  "number of rows in bind array (Default 1000)" },
            { "filesize",        'm',  STL_TRUE,  "max file size" },
            { "commit",          'C',  STL_TRUE,  "number of commit unit (Default 5000)" },
            { "atomic",          'A',  STL_FALSE, "use atomic function" },
            { "parallel",        'p',  STL_TRUE,  "use parallel in import" },
            { "errors",          'E',  STL_TRUE,  "number of error count to allow (Default 100)" },
            { "AsTIMESTAMP",     't',  STL_FALSE, "bind DATE as TIMESTAMP" },
            { "buffered",        'B',  STL_FALSE, "buffered disk io (Default direct io)" },
            { "no-copyright",    'n',  STL_FALSE, "suppresses the display of the banner" },
            { "silent",          's',  STL_FALSE, "suppresses the display of the result message"},
            { "help",            'h',  STL_FALSE, "print help message" },
            { NULL,              '\0', STL_FALSE,  NULL } 
        };

    /**
     * Argument Parsing
     */
    if( aArgc <= 1 )
    {
        STL_THROW( RAMP_ERR_NO_ARG );
    }
    for( sIdxArgv = 1; sIdxArgv < aArgc; sIdxArgv++ )
    {
        if( aArgv[sIdxArgv][0] == '-' )
        {
            break;
        }

        switch( sIdxArgv )
        {
            case 2:
                stlStrncpy( aInputArguments->mPassword, aArgv[sIdxArgv], STL_MAX_SQL_IDENTIFIER_LENGTH );
                gZtdPasswordIdx = 2;
                break;
            case 1:
                stlStrncpy( aInputArguments->mId, aArgv[sIdxArgv], STL_MAX_SQL_IDENTIFIER_LENGTH );
                gZtdUserNameIdx = 1;
                break;
            default:
                break;
        }
    }
        
    if( sIdxArgv < aArgc )
    {
        STL_TRY( stlInitGetOption( &sOpt,
                                   aArgc - (sIdxArgv - 1),
                                   (const stlChar* const *)&aArgv[sIdxArgv - 1],
                                   aErrorStack )
                 == STL_SUCCESS );
        
        while( STL_TRUE )
        {
            if( stlGetOptionLong( &sOpt,
                                  sOptOption,
                                  &sCh,
                                  &sOptionArg,
                                  aErrorStack ) != STL_SUCCESS )
            {
                if( stlGetLastErrorCode( aErrorStack ) != STL_ERRCODE_EOF )
                {
                    if( stlGetLastErrorCode( aErrorStack ) == STL_ERRCODE_INVALID_ARGUMENT )
                    {
                        sInvalidOption = STL_TRUE;
                        break;
                    }
                    
                    STL_TRY( STL_FALSE );
                }
                
                stlPopError( aErrorStack );
                break;
            }
        
            switch( sCh )
            {
                case 'e':
                    gZtdModeType = ZTD_MODE_TYPE_EXPORT;
                    break;
                case 'i':
                    gZtdModeType = ZTD_MODE_TYPE_IMPORT;
                    break;
                case 'f' :
                    /*
                     * file format은 text또는 binary가 들어와야 한다. (default text)
                     */
                    STL_TRY_THROW( ( stlStrlen( sOptionArg ) < ZTD_MAX_FILE_PATH_LENGTH ),
                                   RAMP_ERR_NAME_LEN );

                    
                    if( stlStrcasecmp( sOptionArg, ZTD_CONSTANT_TEXT_FORMAT ) == 0 )
                    {
                        gZtdFormatType= ZTD_FORMAT_TYPE_TEXT;
                    }
                    else if( stlStrcasecmp( sOptionArg, ZTD_CONSTANT_BINARY_FORMAT ) == 0 )
                    {
                        gZtdFormatType = ZTD_FORMAT_TYPE_BINARY;
                    }
                    else
                    {
                        STL_THROW( RAMP_ERR_USAGE );
                    }
                        
                    break;
                case 'm' :
                    STL_TRY( stlStrToInt64( (const stlChar*) sOptionArg,
                                            stlStrlen( sOptionArg ),
                                            NULL,
                                            10,
                                            &aInputArguments->mMaxFileSize,
                                            aErrorStack )
                             == STL_SUCCESS );
                    break;
                case 'c':
                    STL_TRY_THROW( ( stlStrlen( sOptionArg ) < ZTD_MAX_FILE_PATH_LENGTH ),
                                   RAMP_ERR_NAME_LEN );

                    stlStrncpy( aInputArguments->mControl, sOptionArg, ZTD_MAX_FILE_PATH_LENGTH );
                    break;
                case 'd':
                    /**
                     * Bad 또는 Log File 옵션을 생략한 경우,
                     * Data File의 이름을 기준으로 Bad, Log File 이름을 생성하여서
                     * File 이름에 Separator 가 있는 경우와 없는 경우를 나누어 길이 체크한다.
                     */ 
                    sSep = stlStrrchr( sOptionArg, STL_PATH_SEPARATOR[0] );
                    if( sSep == NULL )
                    {
                        STL_TRY_THROW( ( stlStrlen( sOptionArg ) < ZTD_MAX_FILE_NAME_LENGTH ),
                                       RAMP_ERR_NAME_LEN );
                    }
                    else
                    {
                        STL_TRY_THROW( ( stlStrlen( sOptionArg ) < ZTD_MAX_FILE_PATH_LENGTH ),
                                       RAMP_ERR_NAME_LEN );
                        
                        STL_TRY_THROW( (&sOptionArg[stlStrlen(sOptionArg)] - sSep) <= ZTD_MAX_FILE_NAME_LENGTH ,
                                       RAMP_ERR_NAME_LEN );
                    }
                    STL_TRY_THROW( ( stlStrlen( sOptionArg ) < ZTD_MAX_FILE_PATH_LENGTH ),
                                   RAMP_ERR_NAME_LEN );

                    stlStrncpy( aInputArguments->mData, sOptionArg, ZTD_MAX_FILE_PATH_LENGTH );
                    break;
                case 'l':
                    STL_TRY_THROW( ( stlStrlen( sOptionArg ) < ZTD_MAX_FILE_PATH_LENGTH ),
                                   RAMP_ERR_NAME_LEN );
                    
                    stlStrncpy( aInputArguments->mLog, sOptionArg, ZTD_MAX_FILE_PATH_LENGTH );
                    break;
                case 'b':
                    STL_TRY_THROW( ( stlStrlen( sOptionArg ) < ZTD_MAX_FILE_PATH_LENGTH ),
                                   RAMP_ERR_NAME_LEN );
                    
                    stlStrncpy( aInputArguments->mBad, sOptionArg, ZTD_MAX_FILE_PATH_LENGTH );
                    break;
                case 'D':
                    STL_TRY_THROW( ( stlStrlen( sOptionArg ) < ZTD_MAX_IDENTIFIER_LENGTH ),
                                   RAMP_ERR_NAME_LEN );

                    stlStrncpy( aInputArguments->mDsn, sOptionArg, ZTD_MAX_IDENTIFIER_LENGTH );
                    break;
                case 'a':
                    STL_TRY( stlStrToInt64( (const stlChar*) sOptionArg,
                                            stlStrlen( sOptionArg ),
                                            NULL,
                                            10,
                                            &aInputArguments->mArraySize,
                                            aErrorStack )
                             == STL_SUCCESS );

                    if( (aInputArguments->mArraySize < 1) ||
                        (aInputArguments->mArraySize > 65535) )
                    {
                        STL_THROW( RAMP_ERR_ARRAY_SIZE );
                    }
                    break;
                case 'C':
                    STL_TRY( stlStrToInt64( (const stlChar*) sOptionArg,
                                            stlStrlen( sOptionArg ),
                                            NULL,
                                            10,
                                            &aInputArguments->mCommitUnit,
                                            aErrorStack )
                             == STL_SUCCESS );
                    break;
                case 'A':
                    aInputArguments->mUseAtomic  = STL_TRUE;
                    break;
                case 'p':
                    STL_TRY( stlStrToInt64( (const stlChar*) sOptionArg,
                                            stlStrlen( sOptionArg ),
                                            NULL,
                                            10,
                                            &aInputArguments->mThreadUnit,
                                            aErrorStack )
                             == STL_SUCCESS );

                    if( (aInputArguments->mThreadUnit < 1) ||
                        (aInputArguments->mThreadUnit > 32) )
                    {
                        STL_THROW( RAMP_ERR_MULTI_THREAD );
                    }
                    break;
                case 'E':
                    STL_TRY( stlStrToUInt64( (const stlChar*) sOptionArg,
                                             stlStrlen( sOptionArg ),
                                             NULL,
                                             10,
                                             &sNum64,
                                             aErrorStack )
                             == STL_SUCCESS );
                    
                    aInputArguments->mErrorCount = (stlUInt32) sNum64;
                    gZtdMaxErrorCount = aInputArguments->mErrorCount;
                    break;
                case 'B':
                    aInputArguments->mDirectIo = STL_FALSE;
                    break;
                case 's':
                    aInputArguments->mSilent = STL_TRUE;
                    break;
                case 'n':
                    sNoDisplayCopyright = STL_TRUE;
                    break;
                case 't':
                    gZtdBindDATEAsTIMESTAMP = STL_TRUE;
                    break;
                case 'h':
                    STL_THROW( RAMP_ERR_USAGE );
                    break;
                default:
                    sInvalidOption = STL_TRUE;
                    break;
            }
        }

        STL_TRY_THROW( sInvalidOption != STL_TRUE, RAMP_ERR_USAGE );

        if( aInputArguments->mId[0] == ZTD_CONSTANT_NULL )
        {
            STL_THROW( RAMP_ERR_USAGE );
        }
        if( aInputArguments->mPassword[0] == ZTD_CONSTANT_NULL )
        {
            STL_THROW( RAMP_ERR_USAGE );
        }
        
        if( (gZtdModeType != ZTD_MODE_TYPE_EXPORT) &&
            (gZtdModeType != ZTD_MODE_TYPE_IMPORT) )
        {
            STL_THROW( RAMP_ERR_USAGE );
        }

        if( aInputArguments->mControl[0] == ZTD_CONSTANT_NULL )
        {
            STL_THROW( RAMP_ERR_USAGE );
        }

        if( aInputArguments->mData[0] == ZTD_CONSTANT_NULL )
        {
            STL_THROW( RAMP_ERR_USAGE );
        }
        
        if( sNoDisplayCopyright == STL_TRUE )
        {
            /* do nothing */
        }
        else
        {
            stlPrintf( "\n" );
            stlShowCopyright();
            stlShowVersionInfo();
            stlPrintf( "\n" );
        }
    }
    else
    {
        STL_THROW( RAMP_ERR_USAGE );
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NO_ARG )
    {
        stlPrintf( "Check usage by gloader --help\n");
    }
    
    STL_CATCH( RAMP_ERR_NAME_LEN )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_NAMETOOLONG,
                      NULL,
                      aErrorStack,
                      sOptionArg );                      
    }

    STL_CATCH( RAMP_ERR_MULTI_THREAD )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTD_ERRCODE_MULTI_THREAD_COUNT,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_ARRAY_SIZE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTD_ERRCODE_ARRAY_SIZE,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_USAGE )
    {
        /**
         * Copyright, Version 정보 출력
         */
        if( sNoDisplayCopyright != STL_TRUE )
        {
            stlPrintf( "\n" );
            stlShowCopyright();
            stlShowVersionInfo();
        }
        
        stlPrintf( "\n"
                   "Usage \n"
                   "\n"
                   "    %s user_name password mode control data [format] [options]\n"
                   "\n"
                   "arguments:\n"
                   "\n"
                   "    user_name             user name\n"
                   "    password              password\n"
                   "\n"
                   "mode: gloader's mode. \n"
                   "\n"
                   "    -e, --export                            %s\n"
                   "    -i, --import                            %s\n" 
                   "\n"
                   "control:\n"
                   "\n"
                   "    -c, --control         FILE              %s\n"
                   "\n"
                   "data:\n"
                   "\n"
                   "    -d, --data            FILE              %s\n"
                   "\n"
                   "format:\n"
                   "\n"
                   "    -f, --format          TEXT or BINARY    %s\n"
                   "\n"
                   "options:\n"
                   "\n"
                   "    -l, --log             FILE              %s\n"
                   "    -b, --bad             FILE              %s\n"
                   "    -D, --dsn             DSN               %s\n"
                   "    -a, --array           number of array   %s\n"
                   "    -m, --filesize        byte              %s\n"
                   "    -C, --commit          commit unit       %s\n"
                   "    -A, --atomic                            %s\n"
                   "    -p, --parallel        thread unit       %s\n"
                   "    -E, --errors          error count       %s\n"
                   "    -t, --AsTIMESTAMP                       %s\n"
                   "    -B, --buffered                          %s\n"
                   "    -n  --no-copyright                      %s\n"
                   "    -s, --silent                            %s\n"
                   "\n"
                   "help:\n" 
                   "\n"
                   "    -h, --help                              %s\n"
                   "\n",
                   aArgv[0],
                   sOptOption[0].mDescription,
                   sOptOption[1].mDescription,
                   sOptOption[2].mDescription,
                   sOptOption[3].mDescription,
                   sOptOption[4].mDescription,
                   sOptOption[5].mDescription,
                   sOptOption[6].mDescription,
                   sOptOption[7].mDescription,
                   sOptOption[8].mDescription,
                   sOptOption[9].mDescription,
                   sOptOption[10].mDescription,
                   sOptOption[11].mDescription,
                   sOptOption[12].mDescription,
                   sOptOption[13].mDescription,
                   sOptOption[14].mDescription,
                   sOptOption[15].mDescription,
                   sOptOption[16].mDescription,
                   sOptOption[17].mDescription,
                   sOptOption[18].mDescription,
                   sOptOption[19].mDescription );
         
        stlPrintf( "\n" );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief ztdExamineOptions()는 사용자가 입력한 option 값을 검사한다.
 * @param [in]  aInputArguments  command Arguments를 저장한 구조체
 * @param [out] aErrorStack      error stack
 */
stlStatus ztdExamineOptions( ztdInputArguments   * aInputArguments,
                             stlErrorStack       * aErrorStack )
{
    stlChar      sFileName[ZTD_MAX_FILE_NAME_LENGTH * 2];
    stlChar    * sSep;
    stlChar    * sDot;
    stlChar    * sErrName;

    sSep = stlStrrchr( aInputArguments->mData, STL_PATH_SEPARATOR[0] );
    
    if( stlStrlen( aInputArguments->mLog ) == 0 )
    {
        if( sSep == NULL ) 
        {
            /* 디렉토리 Separator가 없는 경우 */
            stlSnprintf( sFileName,
                         ZTD_MAX_FILE_NAME_LENGTH * 2,
                         "%s",                         
                         aInputArguments->mData );
        }
        else 
        {
            /* 디렉토리 Separator가 있는 경우 */
            stlSnprintf( sFileName,
                         ZTD_MAX_FILE_NAME_LENGTH * 2,
                         "%s",
                         sSep + 1);
        }

        sDot = stlStrrchr( sFileName, ZTD_CONSTANT_DOT );

        /**
         * ztdParseOption 함수에서 Data File의 길이 체크를 하였고,
         * sFileName의 버퍼 길이가 STL_MAX_FILE_NAME_LENGTH의 2배 이기에
         * sFileName의 버퍼 길이를 넘어갈 경우는 없다.
         */
        if( (sDot == NULL) || (sDot == sFileName) )
        {
            stlStrncat( sFileName, ".log", 5 );
        }
        else
        {
            stlStrncpy( sDot, ".log", &sFileName[ZTD_MAX_FILE_NAME_LENGTH * 2 - 1] - sDot );
        }
        
        sErrName = sFileName;
        STL_TRY_THROW( stlStrlen( sFileName ) < ZTD_MAX_FILE_NAME_LENGTH,
                       RAMP_ERR_NAME_LEN );

        stlStrncpy( aInputArguments->mLog, sFileName, ZTD_MAX_FILE_PATH_LENGTH );
    }

    if( stlStrlen( aInputArguments->mBad ) == 0 )
    {
        if( sSep == NULL )
        {
            /* 디렉토리 Separator가 없는 경우 */
            stlSnprintf( sFileName,
                         ZTD_MAX_FILE_NAME_LENGTH * 2,
                         "%s",                         
                         aInputArguments->mData );
        }
        else
        {
            /* 디렉토리 Separator가 있는 경우 */
            stlSnprintf( sFileName,
                         ZTD_MAX_FILE_PATH_LENGTH * 2,
                         "%s",
                         sSep + 1 );
        }

        sDot = stlStrrchr( sFileName, ZTD_CONSTANT_DOT );
        if( (sDot == NULL) || (sDot == sFileName) )
        {
            stlStrncat( sFileName, ".bad", 5 );
        }
        else
        {
            stlStrncpy( sDot, ".bad", &sFileName[ZTD_MAX_FILE_NAME_LENGTH * 2 - 1] - sDot );
        }
        
        sErrName = sFileName;
        STL_TRY_THROW( stlStrlen( sFileName ) < ZTD_MAX_FILE_NAME_LENGTH,
                       RAMP_ERR_NAME_LEN );
        
        stlStrncpy( aInputArguments->mBad, sFileName, ZTD_MAX_FILE_PATH_LENGTH );
    }

    sErrName = aInputArguments->mControl;

    STL_TRY_THROW( stlStrcmp( aInputArguments->mControl, aInputArguments->mData ) != 0,
                   RAMP_ERR_FILE_NAME );
    STL_TRY_THROW( stlStrcmp( aInputArguments->mControl, aInputArguments->mLog )  != 0,
                   RAMP_ERR_FILE_NAME );
    STL_TRY_THROW( stlStrcmp( aInputArguments->mControl, aInputArguments->mBad )  != 0,
                   RAMP_ERR_FILE_NAME );   

    
    sErrName = aInputArguments->mData;

    STL_TRY_THROW( stlStrcmp( aInputArguments->mData, aInputArguments->mLog ) != 0,
                   RAMP_ERR_FILE_NAME );
    STL_TRY_THROW( stlStrcmp( aInputArguments->mData, aInputArguments->mBad ) != 0,
                   RAMP_ERR_FILE_NAME );

    sErrName = aInputArguments->mLog;

    STL_TRY_THROW( stlStrcmp( aInputArguments->mLog, aInputArguments->mBad ) != 0,
                   RAMP_ERR_FILE_NAME );
    
    /**
     * export를 진행할 경우 prallel option을 사용할 수 없다.
     */
    if( gZtdModeType == ZTD_MODE_TYPE_EXPORT )
    {
        if( aInputArguments->mThreadUnit >= 2 )
        {
            STL_THROW( RAMP_ERR_MULTI_THREAD );
        }
    }
    
    /**
     * max file size option은 text mode에서 사용할 수 없다.
     */
    if( (aInputArguments->mMaxFileSize > 0) &&
        (gZtdFormatType == ZTD_FORMAT_TYPE_TEXT) )
    {
        STL_THROW( RAMP_ERR_FILE_SIZE );
    }
    else
    {
        /* Do Nothing */
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NAME_LEN )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_NAMETOOLONG,
                      NULL,
                      aErrorStack,
                      sErrName );                      
    }

    STL_CATCH( RAMP_ERR_FILE_NAME )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTD_ERRCODE_USE_SAME_FILE,
                      NULL,
                      aErrorStack,
                      sErrName );
    }

    STL_CATCH( RAMP_ERR_MULTI_THREAD );
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTD_ERRCODE_EXPORT_MULTI_THREAD,
                      NULL,
                      aErrorStack );
    }
    
    STL_CATCH( RAMP_ERR_FILE_SIZE );
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTD_ERRCODE_TEXT_MAX_FILE_SIZE,
                      NULL,
                      aErrorStack );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}

void ztdSignalFunc( stlInt32 aSignalNo )
{
    gZtdRunState = STL_FALSE;
}

/**
 * @brief ztdInitManager()는 전역변수 gZtdManager를 초기화한다.
 * @param[in,out] aErrorStack  error stack
 */ 
stlStatus ztdInitManager( stlErrorStack * aErrorStack )
{
    gZtdManager.mCntRecord = 0;
    gZtdManager.mCntBad    = 0;

    STL_INIT_SPIN_LOCK( gZtdManager.mConsAddLock );

    STL_TRY( stlCreateSemaphore( &gZtdManager.mFileSema,
                                 "FileSem",
                                 1,
                                 aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief ztdFiniManager()는 전역변수 gZtdManager를 종료한다.
 * @param[in,out] aErrorStack  error stack
 */ 
stlStatus ztdFiniManager( stlErrorStack * aErrorStack )
{
    STL_TRY( stlDestroySemaphore( &gZtdManager.mFileSema,
                                  aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;

}

int main( int aArgc, char * aArgv[] )
{
    SQLHENV            sEnvHandle = NULL;
    SQLHDBC            sDbcHandle = NULL;
    
    stlUInt32          sState = 0;
    stlAllocator       sAllocator;
    stlErrorStack      sErrorStack;
    
    stlTime            sTimeStart = 0;
    stlTime            sTimeEnd   = 0;
    stlExpTime         sExpTimeStart;
    stlExpTime         sExpTimeEnd;
    
    ztdInputArguments  sInputArguments;
    ztdControlInfo     sControlInfo;
    ztdFileAndBuffer   sFileAndBuffer;
#ifndef WIN32
    stlSignalFunc      sOldSignalFunc;
#endif

    stlMemset(&sInputArguments, 0x00, STL_SIZEOF(ztdInputArguments));
    stlMemset(&sControlInfo, 0x00, STL_SIZEOF(ztdControlInfo));
	
    STL_INIT_ERROR_STACK( &sErrorStack );

    STL_TRY( dtlInitialize() == STL_SUCCESS );
    sState = 1;
    
    stlRegisterErrorTable( STL_ERROR_MODULE_GLIESE_TOOL_GLOADER,
                           gZtdErrorTable );
    
    STL_TRY( stlCreateRegionAllocator( &sAllocator,
                                       ZTD_REGION_INIT_SIZE,
                                       ZTD_REGION_NEXT_SIZE,
                                       &sErrorStack )
             == STL_SUCCESS );
    sState = 2;

    /**
     * 수행 속도 측정을 위해 사용.
     */ 
    sTimeStart = stlNow();

    stlMakeExpTimeByLocalTz( &sExpTimeStart,
                             sTimeStart );
    /**
     * 사용자의 입력을 분석한다.
     */
    STL_TRY( ztdInitInputArguments( &sInputArguments,
                                    &sErrorStack )
             == STL_SUCCESS );
        
    STL_TRY( ztdParseOptions( aArgc,
                              aArgv,
                              &sInputArguments,
                              &sErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztdExamineOptions( &sInputArguments,
                                &sErrorStack )
             == STL_SUCCESS );

    /*
     * ps와 같은 tool에서 ID, Password와 같은 정보가 보이지 않게 감춤 처리한다.
     */
    STL_TRY( stlHideArgument( aArgc,
                              aArgv,
                              &sErrorStack,
                              2,
                              gZtdUserNameIdx,
                              gZtdPasswordIdx )
             == STL_SUCCESS );
    /**
     * CONTROL, DATA, LOG, BAD FILES OPEN
     */
    STL_TRY( ztdOpenFiles ( &sFileAndBuffer,
                            &sInputArguments,
                            &sErrorStack )
             == STL_SUCCESS );

    /**
     * Control File을 파싱하여
     * ztdControlInfo에 입력한다.
     */
    STL_TRY( ztdParseControl( &sFileAndBuffer,
                              &sControlInfo,
                              &sAllocator, 
                              &sErrorStack )
             == STL_SUCCESS );
    sState = 3;
    
    /**
     * Database를 Open한다.
     */
    STL_TRY( ztdOpenDatabase( &sEnvHandle,
                              &sDbcHandle,
                              &sInputArguments,
                              sControlInfo.mCharacterSet,
                              &sErrorStack )
             == STL_SUCCESS );
    sState = 4;

#ifndef WIN32
    STL_TRY( stlSetSignalHandler( STL_SIGNAL_QUIT,
                                  ztdSignalFunc,
                                  &sOldSignalFunc,
                                  &sErrorStack )
             == STL_SUCCESS );
#endif

    sState = 5;

    STL_TRY( ztdInitManager( &sErrorStack ) == STL_SUCCESS );
    sState = 6;
    
    /**
     * import 와 export 실행 
     */
    if( gZtdModeType == ZTD_MODE_TYPE_IMPORT )
    {
        if( gZtdFormatType == ZTD_FORMAT_TYPE_TEXT )
        {
            STL_TRY( ztdImportTextData( sEnvHandle,
                                        sDbcHandle,
                                        &sAllocator,
                                        &sControlInfo,
                                        &sFileAndBuffer,
                                        &sInputArguments,
                                        &sErrorStack )
                     == STL_SUCCESS );
        }
        else
        {
            STL_TRY( ztdImportBinaryData( sEnvHandle,
                                          sDbcHandle,
                                          sInputArguments,
                                          &sControlInfo,
                                          &sFileAndBuffer,
                                          &sErrorStack )
                     == STL_SUCCESS );
        }
    }
    else
    {
        if( gZtdFormatType == ZTD_FORMAT_TYPE_TEXT )
        {
            STL_TRY( ztdExportTextData( sEnvHandle,
                                        sDbcHandle,
                                        &sAllocator,
                                        &sInputArguments,
                                        &sControlInfo,
                                        &sFileAndBuffer,
                                        &sErrorStack )
                     == STL_SUCCESS );
        }
        else
        {
            STL_TRY( ztdExportBinaryData( sEnvHandle,
                                          sDbcHandle,
                                          sInputArguments,
                                          &sControlInfo,
                                          &sFileAndBuffer,
                                          &sErrorStack )
                     == STL_SUCCESS );
        }
    }

    sTimeEnd = stlNow();

    stlMakeExpTimeByLocalTz( &sExpTimeEnd,
                             sTimeEnd );

    if( sInputArguments.mSilent == STL_FALSE )
    {
        stlPrintf( " %s\n", sFileAndBuffer.mLogBuffer );
    }

    stlSnprintf( sFileAndBuffer.mLogBuffer,
                 ZTD_MAX_LOG_STRING_SIZE,
                 "%s[ Start Time: %d-%d-%d %d:%d:%d End Time: %d-%d-%d %d:%d:%d Taken Time: %ld micro-sec ]\n\n",
                 sFileAndBuffer.mLogBuffer,
                 sExpTimeStart.mYear + 1900,
                 sExpTimeStart.mMonth + 1,
                 sExpTimeStart.mDay,
                 sExpTimeStart.mHour,
                 sExpTimeStart.mMinute,
                 sExpTimeStart.mSecond,
                 sExpTimeEnd.mYear + 1900,
                 sExpTimeEnd.mMonth + 1,
                 sExpTimeEnd.mDay,
                 sExpTimeEnd.mHour,
                 sExpTimeEnd.mMinute,
                 sExpTimeEnd.mSecond,                 
                 sTimeEnd - sTimeStart );

    STL_TRY( ztdWriteLog( & sFileAndBuffer, &sErrorStack ) == STL_SUCCESS );

    sState = 5;
    STL_TRY( ztdFiniManager( &sErrorStack ) == STL_SUCCESS );

    sState = 4;
#ifndef WIN32
    STL_TRY( stlSetSignalHandler( STL_SIGNAL_QUIT,
                                  sOldSignalFunc,
                                  NULL,
                                  &sErrorStack )
             == STL_SUCCESS );
#endif

    sState = 3;
    STL_TRY( ztdCloseDatabase(  sEnvHandle,
                                sDbcHandle,
                                STL_FALSE,    // RollBack
                                &sErrorStack )
             == STL_SUCCESS );

    sState = 2;
    if( gZtdFormatType == ZTD_FORMAT_TYPE_TEXT )
    {
        STL_TRY( ztdCloseFiles( &sFileAndBuffer,
                                &sErrorStack )
                 == STL_SUCCESS );
    }
    else
    {
        /* Do Nothing */
    }
    
    sState = 1;
    STL_TRY( stlDestroyRegionAllocator( &sAllocator,
                                        &sErrorStack )
             == STL_SUCCESS );
    
    sState = 0;
    STL_TRY( dtlTerminate() == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) ztdEnableColoredText( &sErrorStack );

    if( sInputArguments.mSilent == STL_FALSE )
    {
        (void) ztdPrintErrorStack( &sErrorStack );
    }
    
    switch( sState )
    {
        case 6:
            (void) ztdFiniManager( &sErrorStack );
            
        case 5:
#ifndef WIN32
            (void) stlSetSignalHandler( STL_SIGNAL_QUIT,
                                        sOldSignalFunc,
                                        NULL,
                                        &sErrorStack );
#endif
        case 4:
            if( sControlInfo.mSchema[0] == 0x00 )
            {
                stlSnprintf( sFileAndBuffer.mLogBuffer,
                             ZTD_MAX_LOG_STRING_SIZE,
                             "ERROR: FAILED TO %s TABLE %s\n\n",
                             gZtdModeString[gZtdModeType],
                             sControlInfo.mTable );
            }
            else
            {
                stlSnprintf( sFileAndBuffer.mLogBuffer,
                             ZTD_MAX_LOG_STRING_SIZE,
                             "ERROR: FAILED TO %s TABLE %s.%s\n\n",
                             gZtdModeString[gZtdModeType],
                             sControlInfo.mSchema,
                             sControlInfo.mTable );
            }

            if( sInputArguments.mSilent == STL_FALSE )
            {
                stlPrintf("%s\n", sFileAndBuffer.mLogBuffer );
            }

            (void) ztdWriteLog( &sFileAndBuffer, &sErrorStack );

            (void) ztdCloseDatabase(  sEnvHandle,
                                      sDbcHandle,
                                      STL_TRUE,    // RollBack
                                      &sErrorStack );
            
        case 3:
            if( gZtdFormatType == ZTD_FORMAT_TYPE_TEXT )
            {
                (void) ztdCloseFiles( &sFileAndBuffer, &sErrorStack );
            }
            else
            {
                /* No Nothing */
            }
        case 2:
            (void) stlDestroyRegionAllocator( &sAllocator, &sErrorStack );
        case 1:
            (void) dtlTerminate();
        default:
            break;
    }
    
    return STL_FAILURE;
}
