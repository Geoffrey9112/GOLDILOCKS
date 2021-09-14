/*******************************************************************************
 * ztqMain.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
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
#include <sqlext.h>
#include <ztqDef.h>
#include <ztqSqlLex.h>
#include <ztqCmdParser.h>
#include <ztqSqlParser.h>
#include <ztqExecute.h>
#include <ztqOdbcBridge.h>
#include <ztqDisplay.h>
#include <ztqHostVariable.h>
#include <ztqQueryHistory.h>
#include <ztqNamedStmt.h>
#include <ztqNamedCursor.h>
#include <ztqMain.h>
#include <ztqCommand.h>

extern stlErrorRecord gZtqErrorTable[];
extern SQLHDBC        gZtqDbcHandle;
extern ztqCmdFunc     gZtqCmdFuncs[];
extern stlInt32       gZtqPrintTryCount;
extern stlBool        gZtqCommunicationLinkFailure;
extern stlBool        gZtqConnectionEstablished;
extern stlBool        gZtqSpoolOn;


/**
 * @file ztqMain.c
 * @brief Main Routines
 */

/**
 * @addtogroup ztqMain
 * @{
 */

extern stlInt32  gZtqHistorySize;

stlInt32         gZtqFinish = 0;
stlBool          gZtqPrintCallstack = STL_FALSE;
stlBool          gZtqVerticalColumnHeaderOrder = STL_FALSE;
stlInt32         gZtqLineSize = ZTQ_DEFAULT_LINE_SIZE;
stlInt32         gZtqColSize = ZTQ_DEFAULT_COL_SIZE;
stlInt32         gZtqNumSize = ZTQ_DEFAULT_NUM_SIZE;
stlInt32         gZtqDdlSize = ZTQ_DEFAULT_DDL_SIZE;
stlInt32         gZtqPageSize = ZTQ_DEFAULT_PAGE_SIZE;
stlBool          gZtqEcho = STL_FALSE;
stlBool          gZtqDisplayPrompt = STL_TRUE;
stlBool          gZtqAutocommit = STL_FALSE;
stlBool          gZtqSilent = STL_FALSE;
stlVarInt        gZtqOdbcVersion = 3;
stlInt32         gZtqLineNo = 1;
stlBool          gZtqTiming = STL_FALSE;
stlBool          gZtqVerbose = STL_FALSE;
stlBool          gZtqPrintResult = STL_TRUE;
stlBool          gZtqPrintErrorMessage = STL_TRUE;
dtlCharacterSet  gZtqCharacterSet = DTL_UTF8;
stlBool          gZtqAdmin = STL_FALSE;
stlBool          gZtqSystemAdmin = STL_FALSE;
stlChar          gZtqDriver[STL_MAX_FILE_PATH_LENGTH];
stlChar          gZtqConnString[STL_MAX_FILE_PATH_LENGTH];
stlChar          gZtqUserName[STL_MAX_SQL_IDENTIFIER_LENGTH];
stlChar          gZtqPassword[STL_MAX_SQL_IDENTIFIER_LENGTH];
stlChar          gZtqDsn[STL_MAX_SQL_IDENTIFIER_LENGTH];
stlChar          gZtqImportFile[STL_MAX_FILE_PATH_LENGTH];
stlChar          gZtqPrompt[ZTQ_MAX_PROMPT_SIZE];
stlInt32         gZtqPromptSize;
stlInt32         gZtqPasswordIdx = 0;
stlInt32         gZtqUserNameIdx = 0;
#ifdef GSQLNET
ztqOperationMode gZtqOperationMode = ZTQ_OPERATION_MODE_CS;
#else
ztqOperationMode gZtqOperationMode = ZTQ_OPERATION_MODE_DA;
#endif

stlInt64         gNLSDateFormatInfoBuf[DTL_DATETIME_FORMAT_INFO_MAX_BUFFER_SIZE / STL_SIZEOF(stlInt64)];
stlInt64         gNLSTimeFormatInfoBuf[DTL_DATETIME_FORMAT_INFO_MAX_BUFFER_SIZE / STL_SIZEOF(stlInt64)];
stlInt64         gNLSTimeWithTimeZoneFormatInfoBuf[DTL_DATETIME_FORMAT_INFO_MAX_BUFFER_SIZE / STL_SIZEOF(stlInt64)];
stlInt64         gNLSTimestampFormatInfoBuf[DTL_DATETIME_FORMAT_INFO_MAX_BUFFER_SIZE / STL_SIZEOF(stlInt64)];
stlInt64         gNLSTimestampWithTimeZonwFormatInfoBuf[DTL_DATETIME_FORMAT_INFO_MAX_BUFFER_SIZE / STL_SIZEOF(stlInt64)];

dtlDateTimeFormatInfo  * gZtqNLSDateFormatInfo = (dtlDateTimeFormatInfo*)gNLSDateFormatInfoBuf;
dtlDateTimeFormatInfo  * gZtqNLSTimeFormatInfo = (dtlDateTimeFormatInfo*)gNLSTimeFormatInfoBuf;
dtlDateTimeFormatInfo  * gZtqNLSTimeWithTimeZoneFormatInfo = (dtlDateTimeFormatInfo*)gNLSTimeWithTimeZoneFormatInfoBuf;
dtlDateTimeFormatInfo  * gZtqNLSTimestampFormatInfo = (dtlDateTimeFormatInfo*)gNLSTimestampFormatInfoBuf;
dtlDateTimeFormatInfo  * gZtqNLSTimestampWithTimeZoneFormatInfo = (dtlDateTimeFormatInfo*)gNLSTimestampWithTimeZonwFormatInfoBuf;


stlGetOptOption gZtqOptOption[] =
{ 
    { "version",       'v',  STL_FALSE,  "print version infomation and exit" },
    { "import",        'i',  STL_TRUE,   "import sql FILE" },
    { "no-prompt",     'n',  STL_FALSE,  "suppresses the display of the banner and prompts" },
    { "mode",          'm',  STL_TRUE,   "connect mode (DA or CS)" },                        /* hidden option */
    { "dsn",           's',  STL_TRUE,   "dsn string (default is GOLDILOCKS)" },
    { "driver",        'd',  STL_TRUE,   "driver absolute path" },                           /* hidden option */
    { "conn-string",   'r',  STL_TRUE,   "connection string" },
    { "prompt",        'p',  STL_TRUE,   "change prompt string" },
    { "enable-color",  'c',  STL_FALSE,  "enable colored text" },
    { "as",            'a',  STL_TRUE,   "privilege" },
    { "silent",        'l',  STL_FALSE,  "suppresses the display of the result message and echoing of commands" },
    { "help",          'h',  STL_FALSE,  "print help message" },
    { "odbc-version",  'o',  STL_TRUE,   "odbc version ( hidden option )" },                 /* hidden option */
    { NULL,            '\0', STL_FALSE,  NULL } 
};

stlStatus ztqOpenIni( stlChar       * aDsn,
                      stlErrorStack * aErrorStack )
{
    stlUserID      sUserId;
    stlGroupID     sGroupId;
    stlChar        sUserName[STL_PATH_MAX];
    stlChar        sHome[STL_PATH_MAX];
    stlBool        sFoundProperty = STL_FALSE;
    stlChar        sProperty[STL_MAX_INI_PROPERTY_VALUE];
    stlInt64       sInt64Value;
    stlChar      * sEndPtr = NULL;
    stlBool        sFoundDsn = STL_FALSE;
    stlBool        sExist = STL_TRUE;
    stlInt32       sState = 0;
    stlIni         sIni;
    stlIniObject * sObject = NULL;
    stlChar        sFileName[STL_PATH_MAX];
    stlChar        sUserBuffer[STL_PATH_MAX];
    stlChar        sGroupBuffer[STL_PATH_MAX];
    
    STL_TRY( stlGetCurrentUserID( &sUserId,
                                  sUserBuffer,
                                  STL_PATH_MAX,
                                  &sGroupId,
                                  sGroupBuffer,
                                  STL_PATH_MAX,
                                  aErrorStack )
             == STL_SUCCESS );

    STL_TRY( stlGetUserNameByUserID( sUserName,
                                     STL_PATH_MAX,
                                     sUserId,
                                     aErrorStack )
             == STL_SUCCESS );

    if( stlGetHomeDirByUserName( sHome,
                                 STL_PATH_MAX,
                                 sUserName,
                                 aErrorStack )
        != STL_SUCCESS )
    {
        STL_TRY( stlGetLastErrorCode( aErrorStack ) == STL_ERRCODE_NO_ENTRY );
        (void)stlPopError( aErrorStack );

        STL_THROW( RAMP_FINISH );
    }

    stlSnprintf( sFileName,
                 STL_PATH_MAX,
                 "%s"STL_PATH_SEPARATOR"%s",
                 sHome,
                 ".gsql.ini" );
    
    STL_TRY( stlExistFile( sFileName,
                           &sExist,
                           aErrorStack )
             == STL_SUCCESS );

    STL_TRY_THROW( sExist == STL_TRUE, RAMP_FINISH );

    STL_TRY( stlOpenIni( &sIni,
                         sFileName,
                         aErrorStack )
             == STL_SUCCESS );
    sState = 1;

    STL_TRY( stlFindIniObject( &sIni,
                               aDsn,
                               &sFoundDsn,
                               &sObject,
                               aErrorStack ) == STL_SUCCESS );
    
    STL_TRY_THROW( sFoundDsn == STL_TRUE, RAMP_FINISH );

    /**
     * VERTICAL
     */
    STL_TRY( stlFindIniProperty( sObject,
                                 "VERTICAL",
                                 sProperty,
                                 &sFoundProperty,
                                 aErrorStack )
             == STL_SUCCESS );

    if( sFoundProperty == STL_TRUE )
    {
        if( stlStrcasecmp( sProperty, "ON" ) == 0 )
        {
            gZtqVerticalColumnHeaderOrder = STL_TRUE;
        }
        else if( stlStrcasecmp( sProperty, "OFF" ) == 0 )
        {
            gZtqVerticalColumnHeaderOrder = STL_FALSE;
        }
        else
        {
            /**
             * ignore
             */
        }
    }
    
    /**
     * TIMING
     */
    STL_TRY( stlFindIniProperty( sObject,
                                 "TIMING",
                                 sProperty,
                                 &sFoundProperty,
                                 aErrorStack )
             == STL_SUCCESS );

    if( sFoundProperty == STL_TRUE )
    {
        if( stlStrcasecmp( sProperty, "ON" ) == 0 )
        {
            gZtqTiming = STL_TRUE;
        }
        else if( stlStrcasecmp( sProperty, "OFF" ) == 0 )
        {
            gZtqTiming = STL_FALSE;
        }
        else
        {
            /**
             * ignore
             */
        }
    }
    
    /**
     * AUTOCOMMIT
     */
    STL_TRY( stlFindIniProperty( sObject,
                                 "AUTOCOMMIT",
                                 sProperty,
                                 &sFoundProperty,
                                 aErrorStack )
             == STL_SUCCESS );

    if( sFoundProperty == STL_TRUE )
    {
        if( stlStrcasecmp( sProperty, "ON" ) == 0 )
        {
            gZtqAutocommit = STL_TRUE;
        }
        else if( stlStrcasecmp( sProperty, "OFF" ) == 0 )
        {
            gZtqAutocommit = STL_FALSE;
        }
        else
        {
            /**
             * ignore
             */
        }
    }
    
    /**
     * LINESIZE
     */
    STL_TRY( stlFindIniProperty( sObject,
                                 "LINESIZE",
                                 sProperty,
                                 &sFoundProperty,
                                 aErrorStack )
             == STL_SUCCESS );

    if( sFoundProperty == STL_TRUE )
    {
        STL_TRY( stlStrToInt64( sProperty,
                                STL_MAX_INI_PROPERTY_VALUE,
                                &sEndPtr,
                                10,
                                &sInt64Value,
                                aErrorStack ) == STL_SUCCESS );

        if( (sInt64Value >= ZTQ_MIN_LINE_SIZE) &&
            (sInt64Value <= ZTQ_MAX_LINE_SIZE) )
        {
            gZtqLineSize = (stlInt32)sInt64Value;
        }
        else
        {
            /**
             * ignore
             */
        }
    }
    
    /**
     * PAGESIZE
     */
    STL_TRY( stlFindIniProperty( sObject,
                                 "PAGESIZE",
                                 sProperty,
                                 &sFoundProperty,
                                 aErrorStack )
             == STL_SUCCESS );

    if( sFoundProperty == STL_TRUE )
    {
        STL_TRY( stlStrToInt64( sProperty,
                                STL_MAX_INI_PROPERTY_VALUE,
                                &sEndPtr,
                                10,
                                &sInt64Value,
                                aErrorStack ) == STL_SUCCESS );

        if( (sInt64Value >= ZTQ_MIN_PAGE_SIZE) &&
            (sInt64Value <= ZTQ_MAX_PAGE_SIZE) )
        {
            gZtqPageSize = (stlInt32)sInt64Value;
        }
        else
        {
            /**
             * ignore
             */
        }
    }
    
    /**
     * COLSIZE
     */
    STL_TRY( stlFindIniProperty( sObject,
                                 "COLSIZE",
                                 sProperty,
                                 &sFoundProperty,
                                 aErrorStack )
             == STL_SUCCESS );

    if( sFoundProperty == STL_TRUE )
    {
        STL_TRY( stlStrToInt64( sProperty,
                                STL_MAX_INI_PROPERTY_VALUE,
                                &sEndPtr,
                                10,
                                &sInt64Value,
                                aErrorStack ) == STL_SUCCESS );

        if( (sInt64Value >= ZTQ_MIN_COL_SIZE) &&
            (sInt64Value <= ZTQ_MAX_COL_SIZE) )
        {
            gZtqColSize = (stlInt32)sInt64Value;
        }
        else
        {
            /**
             * ignore
             */
        }
    }
    
    /**
     * NUMSIZE
     */
    STL_TRY( stlFindIniProperty( sObject,
                                 "NUMSIZE",
                                 sProperty,
                                 &sFoundProperty,
                                 aErrorStack )
             == STL_SUCCESS );

    if( sFoundProperty == STL_TRUE )
    {
        STL_TRY( stlStrToInt64( sProperty,
                                STL_MAX_INI_PROPERTY_VALUE,
                                &sEndPtr,
                                10,
                                &sInt64Value,
                                aErrorStack ) == STL_SUCCESS );

        if( (sInt64Value >= ZTQ_MIN_NUM_SIZE) &&
            (sInt64Value <= ZTQ_MAX_NUM_SIZE) )
        {
            gZtqNumSize = (stlInt32)sInt64Value;
        }
        else
        {
            /**
             * ignore
             */
        }
    }
    
    /**
     * DDLSIZE
     */
    STL_TRY( stlFindIniProperty( sObject,
                                 "DDLSIZE",
                                 sProperty,
                                 &sFoundProperty,
                                 aErrorStack )
             == STL_SUCCESS );

    if( sFoundProperty == STL_TRUE )
    {
        STL_TRY( stlStrToInt64( sProperty,
                                STL_MAX_INI_PROPERTY_VALUE,
                                &sEndPtr,
                                10,
                                &sInt64Value,
                                aErrorStack ) == STL_SUCCESS );

        if( (sInt64Value >= ZTQ_MIN_DDL_SIZE) &&
            (sInt64Value <= ZTQ_MAX_DDL_SIZE) )
        {
            gZtqDdlSize = (stlInt32)sInt64Value;
        }
        else
        {
            /**
             * ignore
             */
        }
    }
    
    /**
     * HISTORY
     */
    STL_TRY( stlFindIniProperty( sObject,
                                 "DDLSIZE",
                                 sProperty,
                                 &sFoundProperty,
                                 aErrorStack )
             == STL_SUCCESS );

    if( sFoundProperty == STL_TRUE )
    {
        STL_TRY( stlStrToInt64( sProperty,
                                STL_MAX_INI_PROPERTY_VALUE,
                                &sEndPtr,
                                10,
                                &sInt64Value,
                                aErrorStack ) == STL_SUCCESS );

        if( (sInt64Value >= ZTQ_MIN_HISTORY_BUFFER_SIZE) &&
            (sInt64Value <= ZTQ_MAX_HISTORY_BUFFER_SIZE) )
        {
            gZtqHistorySize = (stlInt32)sInt64Value;
        }
        else
        {
            /**
             * ignore
             */
        }
    }

    STL_RAMP( RAMP_FINISH );

    if( sState == 1 )
    {
        sState = 0;
        STL_TRY( stlCloseIni( &sIni,
                              aErrorStack )
                 == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void) stlCloseIni( &sIni, aErrorStack );
    }

    return STL_FAILURE;
}

stlStatus ztqParseOptions( stlInt32        aArgc,
                           stlChar       * aArgv[],
                           stlErrorStack * aErrorStack )
{
    stlGetOpt         sOpt;
    stlBool           sInvalidOption = STL_FALSE;
    stlChar           sCh;
    const stlChar   * sOptionArg;
    stlInt32          i;
    stlChar         * sEndPtr;
    stlInt64          sInt64 = 0;

    gZtqDriver[0]      = '\0';
    gZtqConnString[0]  = '\0';
    gZtqUserName[0]    = '\0';
    gZtqPassword[0]    = '\0';

    stlStrcpy( gZtqDsn, ZTQ_DEFAULT_DSN );
    stlStrncpy( gZtqPrompt, ZTQ_DEFAULT_PROMPT, ZTQ_MAX_PROMPT_SIZE );

    STL_TRY_THROW( aArgc >= 2, RAMP_ERR_USAGE );

    /*
     * Argument parsing
     */
    for( i = 1; i < aArgc; i++ )
    {
        if( aArgv[i][0] == '-' )
        {
            break;
        }

        switch( i )
        {
            case 2:
                stlStrncpy( gZtqPassword, aArgv[i], STL_MAX_SQL_IDENTIFIER_LENGTH );
                gZtqPasswordIdx = 2;
                break;
            case 1:
                stlStrncpy( gZtqUserName, aArgv[i], STL_MAX_SQL_IDENTIFIER_LENGTH );
                gZtqUserNameIdx = 1;
                break;
            default:
                break;
        }
    }

    if( i < aArgc )
    {
        /*
         * Option argument parsing
         */
        STL_TRY( stlInitGetOption( &sOpt,
                                   aArgc - (i - 1),
                                   (const stlChar* const *)&aArgv[i - 1],
                                   aErrorStack )
                 == STL_SUCCESS );

        while( STL_TRUE )
        {
            if( stlGetOptionLong( &sOpt,
                                  gZtqOptOption,
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
                case 'i':
                    stlStrncpy( gZtqImportFile, sOptionArg, STL_MAX_FILE_PATH_LENGTH );
                    gZtqDisplayPrompt = STL_FALSE;
                    gZtqEcho = STL_TRUE;
                    break;
                case 'h':
                    STL_THROW( RAMP_ERR_USAGE );
                    break;
                case 'v':
                    stlPrintf( "\n" );
                    stlShowCopyright();
                    stlShowVersionInfo();
                    stlPrintf( "\n" );
                    
                    STL_TRY( STL_FALSE );
                    break;
                case 'n':
                    gZtqDisplayPrompt = STL_FALSE;
                    break;
                case 'l':
                    gZtqSilent = STL_TRUE;
                    break;
                case 'o':
                    STL_TRY( stlStrToInt64( sOptionArg,
                                            STL_NTS,
                                            &sEndPtr,
                                            10,
                                            &sInt64,
                                            aErrorStack )
                             == STL_SUCCESS );
                    gZtqOdbcVersion = sInt64;
                    break;
                case 'd':
                    stlStrncpy( gZtqDriver, sOptionArg, STL_MAX_FILE_PATH_LENGTH );
                    break;
                case 'r':
                    stlStrncpy( gZtqConnString, sOptionArg, STL_MAX_FILE_PATH_LENGTH );
                    break;
                case 'm':
                    if( stlStrcasecmp( sOptionArg, "CS" ) == 0 )
                    {
                        gZtqOperationMode = ZTQ_OPERATION_MODE_CS;
                    }
#ifndef GSQLNET
                    else if( stlStrcasecmp( sOptionArg, "DA" ) == 0 )
                    {
                        gZtqOperationMode = ZTQ_OPERATION_MODE_DA;
                    }
#endif
                    else
                    {
                        sInvalidOption = STL_TRUE;
                    }
                    break;
                case 'c':
                    STL_TRY( ztqEnableColoredText( aErrorStack ) == STL_SUCCESS );
                    break;
                case 's':
                    stlStrncpy( gZtqDsn, sOptionArg, STL_MAX_SQL_IDENTIFIER_LENGTH );
                    break;
                case 'a':
                    if( stlStrcasecmp( sOptionArg, "SYSDBA" ) == 0 )
                    {
                        gZtqAdmin = STL_TRUE;
                        gZtqSystemAdmin = STL_FALSE;
                    }
                    else if( stlStrcasecmp( sOptionArg, "ADMIN" ) == 0 )
                    {
                        gZtqAdmin = STL_TRUE;
                        gZtqSystemAdmin = STL_TRUE;
                    }
                    else
                    {
                        sInvalidOption = STL_TRUE;
                    }
                    break;
                case 'p':
                    stlSnprintf( gZtqPrompt,
                                 ZTQ_MAX_PROMPT_SIZE,
                                 "%s"ZTQ_PROMPT_TAIL,
                                 sOptionArg );
                    gZtqPromptSize = stlStrlen( gZtqPrompt );
                    break;
                default:
                    sInvalidOption = STL_TRUE;
                    break;
            }
        }
    
        STL_TRY_THROW( sInvalidOption != STL_TRUE, RAMP_ERR_USAGE );

        if ( gZtqDisplayPrompt == STL_TRUE )
        {
            /*
             * Copyright, Version 정보 출력
             */
            
            stlPrintf( "\n" );
            stlShowCopyright();
            stlShowVersionInfo();
            stlPrintf( "\n" );
        }
        else
        {
            /*
             * Regression Test 를 위해 출력하지 않는다.
             */
        }
    }
    else
    {
        /*
         * Copyright, Version 정보 출력
         */
        
        stlPrintf( "\n" );
        stlShowCopyright();
        stlShowVersionInfo();
        stlPrintf( "\n" );
    }

    STL_TRY( ztqOpenIni( gZtqDsn,
                         aErrorStack )
             == STL_SUCCESS );

    STL_TRY( dtlGetSystemCharacterset( &gZtqCharacterSet,
                                       aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_USAGE )
    {
        /*
         * Copyright, Version 정보 출력
         */
        
        stlPrintf( "\n" );
        stlShowCopyright();
        stlShowVersionInfo();
        
        stlPrintf( "\n"
                   "Usage \n"
                   "\n"
                   "    %s [user_name [password]] [options]\n"
                   "\n"
                   "Arguments:\n"
                   "\n"
                   "    user_name       user name\n"
                   "    password        password\n"
                   "\n"
                   "Options:\n"
                   "\n"
                   "    --version                      %s\n"
                   "    --import       FILE            %s\n"
                   "    --no-prompt                    %s\n"
                   "    --dsn          DSN             %s\n"
                   "    --conn-string  'CONN-STRING'   %s\n"
                   "    --prompt       STRING          %s\n"
                   "    --enable-color                 %s\n"
                   "    --as           {SYSDBA|ADMIN}  %s\n"
                   "    --silent                       %s\n"   
                   "    --help                         %s\n"
                   "\n",
                   aArgv[0],
                   gZtqOptOption[0].mDescription,  /* version */
                   gZtqOptOption[1].mDescription,  /* import */
                   gZtqOptOption[2].mDescription,  /* no-prompt */
                   gZtqOptOption[4].mDescription,  /* dsn */
                   gZtqOptOption[6].mDescription,  /* conn-string */
                   gZtqOptOption[7].mDescription,  /* prompt */
                   gZtqOptOption[8].mDescription,  /* enable-color */
                   gZtqOptOption[9].mDescription,  /* as */
                   gZtqOptOption[10].mDescription, /* silent */
                   gZtqOptOption[11].mDescription  /* help */ );
        stlPrintf( "\n" );
    }

    STL_FINISH;

    return STL_FAILURE;
}

static stlBool ztqIsEmptyString( stlChar * aSqlString )
{
    stlChar  * sPtr;
    stlInt32   sLength;

    sLength = stlStrlen( aSqlString );
    sPtr = aSqlString + sLength - 1;
    
    while( (sPtr >= aSqlString) && stlIsspace( *sPtr ) )
    {
        sPtr--;
    }

    if( sPtr < aSqlString )
    {
        return STL_TRUE;
    }

    return STL_FALSE;
}

static ztqCommentMark ztqCommentString( stlChar * aPtr, ztqCommentMark  aBeginEndMark )
{
    ztqCommentMark sResultMark;
    
    switch ( aBeginEndMark )
    {
        case ZTQ_COMMENT_IS_BEGIN_MARK:
            {
                /* BeginMark Check */
                
                if( *aPtr == '\'' )
                {
                    sResultMark = ZTQ_COMMENT_SINGLE_QUOTE;
                }
                else if ( *aPtr == '"' )
                {
                    sResultMark = ZTQ_COMMENT_DOUBLE_QUOTE;
                }
                else if ( *aPtr == '/' )
                {
                    aPtr ++;
                    
                    if( *aPtr == '*' )
                    {
                        sResultMark = ZTQ_COMMENT_BRACKET_COMMENT;
                    }
                    else
                    {
                        sResultMark = ZTQ_COMMENT_NO_COMMENT;
                    }
                }
                else if ( *aPtr == '-' )
                {
                    aPtr ++;
                    
                    if( *aPtr == '-' )
                    {
                        sResultMark = ZTQ_COMMENT_LINE_COMMENT;
                    }
                    else
                    {
                        sResultMark = ZTQ_COMMENT_NO_COMMENT;
                    }
                }
                else
                {
                    sResultMark = ZTQ_COMMENT_NO_COMMENT;
                }
                
                break;
            }
        case ZTQ_COMMENT_IS_END_MARK:
            {
                /* EndMark Check */
                if( *aPtr == '\'' )
                {
                    sResultMark = ZTQ_COMMENT_SINGLE_QUOTE;
                }
                else if ( *aPtr == '"' )
                {
                    sResultMark = ZTQ_COMMENT_DOUBLE_QUOTE;
                }
                else if ( *aPtr == '*' )
                {
                    aPtr ++;
            
                    if( *aPtr == '/' )
                    {
                        sResultMark = ZTQ_COMMENT_BRACKET_COMMENT;
                    }
                    else
                    {
                        sResultMark = ZTQ_COMMENT_NO_COMMENT;
                    }
                }
                else if ( *aPtr == '\n' )
                {
                    sResultMark = ZTQ_COMMENT_LINE_COMMENT;
                }
                else
                {
                    sResultMark = ZTQ_COMMENT_NO_COMMENT;
                }
                
                break;
            }
        default:
            {
                sResultMark = ZTQ_COMMENT_NO_COMMENT;
                break;
            }
    }
    
    return sResultMark;
    
}

static stlBool ztqIsCompleteSql( void           * aLineBuffer,
                                 ztqCommentMark * aBeginMark )
{
    stlChar        * sPtr;
    stlChar        * sEndPtr;
    stlBool          sResult = STL_FALSE;
    ztqCommentMark   sEndMark;
    
    sEndMark = ZTQ_COMMENT_NO_COMMENT;

    sPtr    = (stlChar*)aLineBuffer;
    sEndPtr = (stlChar*)aLineBuffer + stlStrlen( (stlChar*)aLineBuffer ) -1;

    /* aLineBuffer를 한 character씩 검사함 */
    while( sPtr <= sEndPtr )
    {
        /*
         * BeginMark =  /\* , ' , " , -- 가 없을 때,
         * ';'이 있으면 종결을 의미하므로 sResult에 TRUE를 내보낸다.
         * ';'외에는 BeginMark가 있는지 검사한다.
         **/
        if( *aBeginMark == ZTQ_COMMENT_NO_COMMENT )
        {
            if( *sPtr == ';' )
            {
                sResult = STL_TRUE;
                break;
            }
            else
            {
                /* BeginMark의 여부를 검사한다.*/
                *aBeginMark = ztqCommentString( sPtr, ZTQ_COMMENT_IS_BEGIN_MARK );
            }
        }
        else
        {
            /*
             * EndMark = *\/, ', ", \n
             * BeginMark가 있는 경우로 BeginMark에 맞는 EndMark를 찾는다
             * BeginMark와 EndMark가 같을 경우, 더이상 찾을 필요가 없으므로
             * BeginMark를 초기화시켜준다. 
             **/
            sEndMark = ztqCommentString( sPtr, ZTQ_COMMENT_IS_END_MARK );
            if( *aBeginMark == sEndMark )
            {
                *aBeginMark = ZTQ_COMMENT_NO_COMMENT;
            }
            else
            {
                /* Do Nothing */
                sResult = STL_FALSE;
            }
        }
        sPtr ++;
    }
    return sResult;
}

/*
static stlBool ztqIsCompletePsm( stlChar * aPsmString )
{
    stlChar * sStartPtr;
    stlChar * sEndPtr;
    stlChar * sLeftPtr;
    stlChar * sRightPtr;

    sStartPtr = aPsmString;
    sEndPtr = aPsmString + stlStrlen( aPsmString ) - 1;
    sLeftPtr = sStartPtr;
    sRightPtr = sEndPtr;
    
    while( (sRightPtr > sStartPtr) && stlIsspace( *sRightPtr ) )
    {
        sRightPtr--;
    }

    while( (sLeftPtr < sEndPtr) && stlIsspace( *sLeftPtr ) )
    {
        sLeftPtr++;
    }

     *
     * PSM의 경우는 마지막 라인에 '/'만 존재하는 경우가 종료조건이다.
     *
    if( (*sLeftPtr == '/') && (sLeftPtr == sRightPtr) )
    {
        return STL_TRUE;
    }

    return STL_FALSE;
}
*/

static void ztqRefineSql( stlChar * aSqlString )
{
    stlChar * sPtr;

    sPtr = aSqlString + stlStrlen( aSqlString ) - 1;
    
    while( (sPtr > aSqlString) && stlIsspace( *sPtr ) )
    {
        sPtr--;
    }

    if( *sPtr == ';' )
    {
        *sPtr = '\0';
    }
}

static void ztqRefineCommand( stlChar * aCommandString )
{
    stlChar * sStartPtr;
    stlChar * sEndPtr;
    stlChar * sLeftPtr;
    stlChar * sRightPtr;
    stlInt32  sLength;

    sStartPtr = aCommandString;
    sEndPtr = aCommandString + stlStrlen( aCommandString ) - 1;
    sLeftPtr = sStartPtr;
    sRightPtr = sEndPtr;
    
    while( (sRightPtr > sStartPtr) && stlIsspace( *sRightPtr ) )
    {
        sRightPtr--;
    }

    if( *sRightPtr == ';' )
    {
        *sRightPtr = '\0';
    }

    while( (sLeftPtr < sEndPtr) && stlIsspace( *sLeftPtr ) )
    {
        sLeftPtr++;
    }

    sLength = sRightPtr - sLeftPtr + 1;
    
    stlMemmove( aCommandString, sLeftPtr, sLength );
    aCommandString[sLength] = '\0';
}

/*
static void ztqRefinePsm( stlChar * aSqlString )
{
    stlChar * sPtr;

    sPtr = aSqlString + stlStrlen( aSqlString ) - 1;
    
    while( (sPtr > aSqlString) && stlIsspace( *sPtr ) )
    {
        sPtr--;
    }

    if( *sPtr == '/' )
    {
        *sPtr = '\0';
    }
    else
    {
        STL_ASSERT( 0 );
    }
}
*/

static stlBool ztqIsSqlComment( stlChar  * aSqlString )
{
    stlChar * sStartPtr;
    stlChar * sEndPtr;
    stlChar * sLeftPtr;
    stlBool   sResult = STL_FALSE;
    
    sLeftPtr = aSqlString;
    sEndPtr = aSqlString + stlStrlen( aSqlString ) - 1;
    
    do
    {
        while( (sLeftPtr < sEndPtr ) && stlIsspace( *sLeftPtr ) )
        {
            sLeftPtr ++;
        }
        /* --주석일 경우에는 '\n'까지 그 라인은 주석처리 하기 때문에 바로 TRUE로 보낸다 */
        if( stlStrncmp( sLeftPtr, "--", 2 ) == 0 )
        {
            sResult = STL_TRUE;
            STL_THROW( RAMP_FINISH );           
        }
        /*
         * BRACKET 주석은 주석이 종료되지 않을 경우 다음라인을
         * 받아야 하기 때문에 주석 종료 위치를 받은다음
         * 주석이 끝나고 난 후의 line을 받아 처리한다
         **/
        else if( stlStrncmp( sLeftPtr, "/*", 2 ) == 0 )
        {
            sLeftPtr ++;
            sLeftPtr ++;
            sStartPtr = stlStrstr( sLeftPtr, "*/" );
            /*  주석의 종료가 Line에 존재하지 않을 경우 */
            if( sStartPtr == NULL )
            {
                sResult = STL_FALSE;
            }
            else
            {
                /* 종료 주석이 있을 때는 종료 주석 이후의 Line을 검사해야 함 */
                sLeftPtr = sStartPtr +2; 
                sResult = STL_TRUE;
            }
        }
        /* Line의 끝에 다다랐을 경우 바로 죵료 */
        else if( *sLeftPtr == '\n' || *sLeftPtr == '\r' )
        {
            STL_THROW( RAMP_FINISH );
        }
        else
        {
            sResult = STL_FALSE;
        }

    }
    while( sResult == STL_TRUE );

    STL_RAMP( RAMP_FINISH );

    return sResult;
}

stlBool ztqIsGSQLCommand( stlChar * aString )
{
    stlChar * sEndPtr;
    stlChar * sLeftPtr;
    stlBool   sResult = STL_FALSE;
    
    sLeftPtr = aString;
    sEndPtr  = aString + stlStrlen( aString ) - 1;
    
    while( (sLeftPtr < sEndPtr ) && stlIsspace( *sLeftPtr ) )
    {
        sLeftPtr ++;
    }
        
    if( sLeftPtr[0] == '\\' )
    {
        sResult = STL_TRUE;
    }

    return sResult;
}

static stlStatus ztqAppendString( ztqCommandBuffer * aCommandBuffer,
                                  stlChar          * aString,
                                  stlAllocator     * aAllocator,
                                  stlErrorStack    * aErrorStack )
{
    stlInt32   sStringLength;
    stlInt32   sBufferLength;
    stlChar  * sBuffer;

    STL_ASSERT( aCommandBuffer->mBuffer != NULL );
    
    sStringLength = stlStrlen( aString );

    if( (aCommandBuffer->mPosition + sStringLength) >=
        aCommandBuffer->mLength )
    {
        sBufferLength = STL_MAX( aCommandBuffer->mLength * 2,
                                 aCommandBuffer->mLength + sStringLength + 1 );
        
        STL_TRY( stlAllocRegionMem( aAllocator,
                                    sBufferLength,
                                    (void**)&sBuffer,
                                    aErrorStack )
                 == STL_SUCCESS );

        stlStrcpy( sBuffer, aCommandBuffer->mBuffer );
        
        aCommandBuffer->mLength = sBufferLength;
        aCommandBuffer->mBuffer = sBuffer;
    }

    stlStrcat( aCommandBuffer->mBuffer, aString );
    aCommandBuffer->mPosition += sStringLength;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztqProcessSql( stlChar        * aSqlString,
                         stlAllocator   * aAllocator,
                         stlBool          aPrintResult,
                         stlErrorStack  * aErrorStack )
{
    ztqParseTree * sParseTree;


    STL_TRY_THROW( gZtqConnectionEstablished == STL_TRUE,
                   RAMP_ERR_CONNECTION_DOES_NOT_EXIST );
    
    ztqRefineSql( aSqlString );
    
    STL_TRY( ztqSqlParseIt( aSqlString,
                            aAllocator,
                            &sParseTree,
                            aErrorStack )
             == STL_SUCCESS );

    /*
     * Empty SQL은 skip한다.
     */
    STL_TRY_THROW( sParseTree != ZTQ_EMPTY_PARSE_TREE, RAMP_SKIP );

    switch( sParseTree->mCommandType )
    {
        case ZTQ_COMMAND_TYPE_SQL_BYPASS:
        {
            STL_TRY( ztqDirectExecuteSql( aSqlString,
                                          aAllocator,
                                          sParseTree,
                                          aPrintResult,
                                          aErrorStack )
                     == STL_SUCCESS );
            break;
        }
        case ZTQ_COMMAND_TYPE_SQL_TRANSACTION:
        {
            STL_TRY( ztqDirectExecuteSql( aSqlString,
                                          aAllocator,
                                          sParseTree,
                                          aPrintResult,
                                          aErrorStack )
                     == STL_SUCCESS );
            break;
        }
        default:
        {
            STL_ASSERT( 0 );
            break;
        }
    }

    STL_RAMP( RAMP_SKIP );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_CONNECTION_DOES_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_CONNECTION_DOES_NOT_EXIST,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztqProcessCommand( stlChar       * aCommandString,
                             stlAllocator  * aAllocator,
                             stlErrorStack * aErrorStack )
{
    ztqParseTree * sParseTree;
    
    ztqRefineCommand( aCommandString );

    STL_TRY( ztqCmdParseIt( aCommandString,
                            aAllocator,
                            &sParseTree,
                            aErrorStack )
             == STL_SUCCESS );

    /*
     * Empty 명령어는 skip한다.
     */
    STL_TRY_THROW( sParseTree != ZTQ_EMPTY_PARSE_TREE, RAMP_SUCCESS );

    if ( gZtqConnectionEstablished == STL_TRUE )
    {
        /* nothing to do */
    }
    else
    {
        switch( sParseTree->mCommandType )
        {
            case ZTQ_COMMAND_TYPE_CMD_SET_VERTICAL:
            case ZTQ_COMMAND_TYPE_CMD_SET_TIMING:
            case ZTQ_COMMAND_TYPE_CMD_SET_VERBOSE:
            case ZTQ_COMMAND_TYPE_CMD_SET_LINESIZE:
            case ZTQ_COMMAND_TYPE_CMD_SET_PAGESIZE:
            case ZTQ_COMMAND_TYPE_CMD_SET_COLSIZE:
            case ZTQ_COMMAND_TYPE_CMD_SET_NUMSIZE:
            case ZTQ_COMMAND_TYPE_CMD_SET_DDLSIZE:
            case ZTQ_COMMAND_TYPE_CMD_SET_RESULT:
            case ZTQ_COMMAND_TYPE_CMD_SET_HISTORY:
            case ZTQ_COMMAND_TYPE_CMD_SET_COLOR:
            case ZTQ_COMMAND_TYPE_CMD_SET_ERROR:
            case ZTQ_COMMAND_TYPE_CMD_QUIT:
            case ZTQ_COMMAND_TYPE_CMD_HELP:
            case ZTQ_COMMAND_TYPE_CMD_PRINT:
            case ZTQ_COMMAND_TYPE_CMD_VAR:
            case ZTQ_COMMAND_TYPE_CMD_IMPORT:
            case ZTQ_COMMAND_TYPE_CMD_PRINT_HISTORY:
            case ZTQ_COMMAND_TYPE_CMD_STARTUP:
            case ZTQ_COMMAND_TYPE_CMD_CONNECT:
            case ZTQ_COMMAND_TYPE_CMD_XA_OPEN:
                break;

            default:
                STL_THROW( RAMP_ERR_CONNECTION_DOES_NOT_EXIST );
                break;
        }
    }
    
    STL_TRY( gZtqCmdFuncs[sParseTree->mCommandType]( sParseTree,
                                                     aCommandString,
                                                     aAllocator,
                                                     aErrorStack )
             == STL_SUCCESS );
        

    STL_RAMP( RAMP_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_CONNECTION_DOES_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_CONNECTION_DOES_NOT_EXIST,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztqProcessCommandStream( stlFile       * aInFile,
                                   stlBool         aDisplayPrompt,
                                   stlBool         aEcho,
                                   stlAllocator  * aAllocator,
                                   stlErrorStack * aErrorStack )
{
    stlChar            sBuffer[ZTQ_MAX_COMMAND_BUFFER_SIZE];
    stlChar            sLineBuffer[ZTQ_MAX_COMMAND_BUFFER_SIZE];
    stlStatus          sStatus;
    stlRegionMark      sRegionMark;
    ztqCommandBuffer   sCommandBuffer;
    stlChar          * sCommandString;
    ztqParseTree     * sParseTree;
    ztqCommentMark     sCommentMark;

    STL_TRY( stlMarkRegionMem( aAllocator,
                               &sRegionMark,
                               aErrorStack )
             == STL_SUCCESS );

    sCommentMark = ZTQ_COMMENT_NO_COMMENT;

    do
    {
        ZTQ_INIT_COMMAND_BUFFER( &sCommandBuffer,
                                 sBuffer,
                                 ZTQ_MAX_COMMAND_BUFFER_SIZE );
        
        gZtqLineNo = 1;
        gZtqPrintTryCount = 0;

        ZTQ_PRINT_PROMPT( aDisplayPrompt );
        
        /*
         * 라인 한개를 입력받는다
         */
        stlMemset( sLineBuffer, 0x00, ZTQ_MAX_COMMAND_BUFFER_SIZE );
        
        if( stlGetStringFile( sLineBuffer,
                              ZTQ_MAX_COMMAND_BUFFER_SIZE,
                              aInFile,
                              aErrorStack ) == STL_FAILURE )
        {
            STL_TRY( stlGetLastErrorCode( aErrorStack ) == STL_ERRCODE_EOF );
            STL_THROW( RAMP_FINISH );
        }

        if( (ztqIsEmptyString( sLineBuffer ) == STL_TRUE) ||
            (ztqIsSqlComment( sLineBuffer ) == STL_TRUE) )
        {
            if( aEcho == STL_TRUE )
            {
                ztqEchoPrintf( "%s", sLineBuffer );
            }
            else if( gZtqSpoolOn == STL_TRUE )
            {
                ztqWriteSpoolPrintf( "%s", sLineBuffer );
            }
            
            STL_THROW( RAMP_SKIP );
        }
        
        STL_TRY( ztqAppendString( &sCommandBuffer, 
                                  sLineBuffer, 
                                  aAllocator, 
                                  aErrorStack ) 
                 == STL_SUCCESS ); 
        
        
        if( aEcho == STL_TRUE )
        {
            ztqEchoPrintf( "%s", sLineBuffer );
        }
        else if( gZtqSpoolOn == STL_TRUE )
        {
            ztqWriteSpoolPrintf( "%s", sLineBuffer );
        }

        /*
         * Command가 'EXEC SQL ...'인 경우는 Multi 라인으로 입력될수 있다.
         * EXEC SQL의 끝은 항상 ';'로 끝나야 한다.
         */
        if( ztqIsGSQLCommand( sLineBuffer ) == STL_TRUE )
        {   
            ztqCmdParseIt( sLineBuffer,
                           aAllocator,
                           &sParseTree,
                           aErrorStack );

            /*
             * 명령어가 EXEC SQL인지 확인하는 것이 목적이기 때문에
             * 파싱에러는 무시한다.
             */
            STL_INIT_ERROR_STACK( aErrorStack );
            
            if( (sParseTree != NULL) && (sParseTree != ZTQ_EMPTY_PARSE_TREE) )
            {
                STL_TRY_THROW(
                    (sParseTree->mCommandType == ZTQ_COMMAND_TYPE_CMD_EXEC_SQL) ||
                    (sParseTree->mCommandType == ZTQ_COMMAND_TYPE_CMD_PREPARE_SQL) ||
                    (sParseTree->mCommandType == ZTQ_COMMAND_TYPE_CMD_EXPLAIN_PLAN ) ||
                    (sParseTree->mCommandType == ZTQ_COMMAND_TYPE_CMD_USE_STMT_EXEC_SQL) ||
                    (sParseTree->mCommandType == ZTQ_COMMAND_TYPE_CMD_USE_STMT_PREPARE_SQL),
                    RAMP_SKIP_MORE_SQL );
            }
            else
            {
                STL_THROW( RAMP_SKIP_MORE_SQL );
            }
        }
        
        /*
         * Command가 라인 한개로 종료되지 않는 경우는  ';'이 입력될때까지
         * 계속 읽는다.
         * 첫 라인이 Bracket Mark 일때는 End Bracket Mark를 찾을 때 까지
         * 읽는다. 
         */
        while( ztqIsCompleteSql( sLineBuffer,
                                 &sCommentMark ) != STL_TRUE )
        {
            gZtqLineNo++;
            
            ZTQ_PRINT_PROMPT( aDisplayPrompt );
               
            stlMemset( sLineBuffer,
                       0x00,
                       ZTQ_MAX_COMMAND_BUFFER_SIZE );
            
            if( stlGetStringFile( sLineBuffer,
                                  ZTQ_MAX_COMMAND_BUFFER_SIZE,
                                  aInFile,
                                  aErrorStack ) == STL_FAILURE )
            {
                STL_TRY( stlGetLastErrorCode( aErrorStack ) == STL_ERRCODE_EOF );
                STL_THROW( RAMP_FINISH );
            }

            if( aEcho == STL_TRUE )
            {
                ztqEchoPrintf( "%s", sLineBuffer );
            }
            else if( gZtqSpoolOn == STL_TRUE )
            {
                ztqWriteSpoolPrintf( "%s", sLineBuffer );
            }
            
            STL_TRY( ztqAppendString( &sCommandBuffer,
                                      sLineBuffer,
                                      aAllocator,
                                      aErrorStack )
                     == STL_SUCCESS );
        }
        
        STL_RAMP( RAMP_SKIP_MORE_SQL );

        sCommandString = ZTQ_GET_BUFFER( &sCommandBuffer );
            
        /*
         * Backslash Command라면 gsql 명령어 처리 루틴을 호출하고,
         * 그렇지 않다면 ODBC를 이용하여 SQL을 처리한다.
         */
        if( ztqIsGSQLCommand( sCommandString ) != STL_TRUE )
        {
            sStatus = ztqProcessSql( sCommandString,
                                     aAllocator,
                                     STL_TRUE,  /* aPrintResult */
                                     aErrorStack );
            
            STL_TRY( ztqAddHistory( ZTQ_GET_BUFFER( &sCommandBuffer ),
                                    aErrorStack ) == STL_SUCCESS );
        }
        else
        {
            sStatus = ztqProcessCommand( sCommandString,
                                         aAllocator,
                                         aErrorStack );
        }

        if( sStatus != STL_SUCCESS )
        {
            /*
             * 명령어나 SQL 실행이 실패한 경우 에러를 출력한다
             */
            if( stlGetErrorStackDepth( aErrorStack ) > 0 )
            {
                ztqPrintErrorStack( aErrorStack );
            }
        }
        
        /*
         * Callstack을 출력한다.
         */
        if( gZtqPrintCallstack == 1 )
        {
            ztqPrintErrorCallstack( aErrorStack );
        }

        /*
         * 한 문자라도 출력되었다면 NEW LINE으로 커맨드를 분리한다.
         */
        if( gZtqPrintTryCount > 0 )
        {
            ztqPrintf( "\n" );
        }

        STL_INIT_ERROR_STACK( aErrorStack );

        STL_RAMP( RAMP_SKIP );

        /*
         * gsql은 초기 allocator 사이즈가 충분히 크게 할당되었기 때문에
         * restore시 mark 이후에 할당된 메모리를 해제해도 chunk 경계에서
         * 발생할수 있는 반복적 alloc과 free 현상이 발생하지 않는다.
         */
        STL_TRY( stlRestoreRegionMem( aAllocator,
                                      &sRegionMark,
                                      STL_TRUE,      /* aFree */
                                      aErrorStack )
                 == STL_SUCCESS );

        STL_TRY( gZtqCommunicationLinkFailure == STL_FALSE );
    }
    while( gZtqFinish == 0 );
    
    STL_RAMP( RAMP_FINISH );

    STL_INIT_ERROR_STACK( aErrorStack );

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

int main( int    aArgc,
          char * aArgv[] )
{
    stlUInt32         sState = 0;
    stlFile           sFile;
    stlErrorStack     sErrorStack;
    void            * sDriverHandle = NULL;
    stlAllocator      sAllocator;
    
    STL_INIT_ERROR_STACK( &sErrorStack );

    STL_TRY( dtlInitialize() == STL_SUCCESS );
    sState = 1;
    
    stlRegisterErrorTable( STL_ERROR_MODULE_GLIESE_TOOL_GSQL,
                           gZtqErrorTable );

    STL_TRY( stlCreateRegionAllocator( &sAllocator,
                                       ZTQ_REGION_INIT_SIZE,
                                       ZTQ_REGION_NEXT_SIZE,
                                       &sErrorStack )
             == STL_SUCCESS );
    
    sState = 2;

    STL_TRY( ztqDisableColoredText( &sErrorStack ) == STL_SUCCESS );
    /*
     * 사용자의 옵션 입력을 분석한다.
     */
    STL_TRY( ztqParseOptions( aArgc,
                              aArgv,
                              &sErrorStack )
             == STL_SUCCESS );

    /*
     * ps와 같은 tool에서 ID, Password와 같은 정보가 보이지 않게 감춤 처리한다.
     */
    STL_TRY( stlHideArgument( aArgc,
                              aArgv,
                              &sErrorStack,
                              2,
                              gZtqUserNameIdx,
                              gZtqPasswordIdx )
             == STL_SUCCESS );
    
    if( stlStrlen( gZtqImportFile ) != 0 )
    {
        STL_TRY( stlOpenFile( &sFile,
                              gZtqImportFile,
                              STL_FOPEN_READ,
                              STL_FPERM_OS_DEFAULT,
                              &sErrorStack ) == STL_SUCCESS );
        sState = 3;
    }
    else
    {
        STL_TRY( stlOpenStdinFile( &sFile,
                                   &sErrorStack )
                 == STL_SUCCESS );
        sState = 3;
    }
    
    STL_TRY( ztqOpenDriver( gZtqDriver,
                            &sDriverHandle,
                            &sErrorStack )
             == STL_SUCCESS );
    sState = 4;
    
    STL_TRY( ztqOpenDatabase( gZtqDsn,
                              gZtqUserName,
                              gZtqPassword,
                              NULL, /* aNewPassword */
                              STL_FALSE,    /* Start-Up */
                              &sAllocator,
                              &sErrorStack )
             == STL_SUCCESS );
    sState = 5;
    
    if ( gZtqDisplayPrompt == STL_TRUE )
    {
        if( gZtqConnectionEstablished == STL_TRUE )
        {
            ztqPrintf( "Connected to "STL_PRODUCT_NAME" Database.\n" );
        }
        else
        {
            ztqPrintf( "Connected to an idle instance.\n" );
        }
        ztqPrintf( "\n" );        
    }

    STL_TRY( stlClearRegionMem( &sAllocator,
                                &sErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqInitHistory( &sErrorStack ) == STL_SUCCESS );
    sState = 6;

    STL_TRY( ztqAllocSysHostVariable( &sErrorStack ) == STL_SUCCESS );
    sState = 7;

    /*
     * SQL & Command Loop
     */

    if( gZtqSilent == STL_TRUE )
    {
        gZtqEcho = STL_FALSE;
    }
    
    STL_TRY( ztqProcessCommandStream( &sFile,
                                      gZtqDisplayPrompt,
                                      gZtqEcho,
                                      &sAllocator,
                                      &sErrorStack )
             == STL_SUCCESS );

    sState = 6;
    STL_TRY( ztqDestHostVariables( &sErrorStack ) == STL_SUCCESS );
    
    sState = 5;
    STL_TRY( ztqDestHistory( &sErrorStack ) == STL_SUCCESS );
    
    sState = 4;
    STL_TRY( ztqCloseDatabase( &sErrorStack ) == STL_SUCCESS );

    sState = 3;
    STL_TRY( ztqCloseDriver( sDriverHandle,
                             &sErrorStack )
             == STL_SUCCESS );
    
    sState = 2;
    STL_TRY( stlCloseFile( &sFile,
                           &sErrorStack ) == STL_SUCCESS );
    
    sState = 1;
    STL_TRY( stlDestroyRegionAllocator( &sAllocator,
                                        &sErrorStack )
             == STL_SUCCESS );
    
    sState = 0;
    STL_TRY( dtlTerminate() == STL_SUCCESS );

    return STL_SUCCESS;
    
    STL_FINISH;

    ztqPrintErrorStack( &sErrorStack );
    
    switch( sState )
    {
        case 7:
            (void)ztqSQLEndTran( SQL_HANDLE_DBC,
                                 gZtqDbcHandle,
                                 SQL_ROLLBACK,
                                 STL_TRUE,  /* aIgnoreFailure */
                                 &sErrorStack );

            ztqDestAllNamedCursor( &sErrorStack );
            ztqDestAllNamedStmt( &sErrorStack );
            
            (void)ztqDestPreparedStmt( &sErrorStack );
            (void)ztqDestHostVariables( &sErrorStack );
        case 6:
            (void)ztqDestHistory( &sErrorStack );
        case 5:
            (void)ztqCloseDatabase( &sErrorStack );
        case 4:
            (void)ztqCloseDriver( sDriverHandle, &sErrorStack );
        case 3:
            (void)stlCloseFile( &sFile, &sErrorStack );
        case 2:
            (void)stlDestroyRegionAllocator( &sAllocator, &sErrorStack );
        case 1:
            (void)dtlTerminate();
        default:
            break;
    }

    return STL_FAILURE;
}

/** @} */
