/*******************************************************************************
 * ztpMain.c
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
#include <ztpDef.h>
#include <ztpSqlLex.h>
#include <ztpSqlParser.h>
#include <ztpMisc.h>
#include <ztpCParseFunc.h>
#include <ztpMain.h>
#include <ztpuFile.h>
#include <ztpuOption.h>

extern stlErrorRecord gZtpErrorTable[];

/**
 * @file ztpMain.c
 * @brief Main Routines
 */

/**
 * @addtogroup ztpMain
 * @{
 */

/* pre-compiled output file header */
stlChar         gOutputHeader[] =
    "/***********************************************\n"
    " * This code is inserted automatically by gpec\n"
    " ***********************************************/\n"
    "#include <goldilocksesql.h>\n"
    "/***********************************************\n"
    " * End of gpec automatic inclusion\n"
    " ***********************************************/\n"
    "\n\n";
stlSize          gOutputHeaderSize = 0;

/* Global variables */
stlBool          gIsPathGiven = STL_FALSE;
stlBool          gUnsafeNull = STL_FALSE;
stlBool          gNoLineInfo = STL_FALSE;
stlChar          gInFilePath[STL_MAX_FILE_PATH_LENGTH + 1];
stlChar          gInFileDir[STL_MAX_FILE_PATH_LENGTH + 1];
stlChar          gOutFilePath[STL_MAX_FILE_PATH_LENGTH + 1];
ztpIncludePath   gIncludePath;
ztpSymTabHeader *gRootSymTab = NULL;
ztpSymTabHeader *gCurrSymTab = NULL;
stlBool          gDisplayVersion = STL_TRUE;
ztphStaticHash  *gMacroSymTab = NULL;
ztpParsingLevel  gParsingLevel = ZTP_PARSING_LEVEL_MINIMUM;

/*
 * 임시 SQL buffer를 1M로 생성한다.
 * 이 크기는 수치 자체에 큰 의미는 없으며,
 * 단지 1MB 정도면 변환해야 할 한번의 SQL 문이 저장 될 수 있을 만큼,
 * 여유있다고 생각되어서 지정한 값이다.
 * 이 값에 문제가 있다면, 더 크게 늘리거나, 확장성 있게 관리하는 방안을
 * 고민하도록 한다.
 */
stlChar         *gSqlBuffer = NULL;
stlInt32         gSqlBufLen = 1024 * 1024;
stlInt32         gSqlBufferIdx = 0;

stlGetOptOption  gOptOption[] =
{
    { "no-prompt",     'n',                    STL_FALSE,  "No Print version infomation" },
    { "version",       'v',                    STL_FALSE,  "Print version infomation and exit" },
    { "help",          'h',                    STL_FALSE,  "Print help message" },
    { "output",        'o',                    STL_TRUE,   "Describe output filename" },
    { "unsafe-null",   ZTP_OPTION_UNSAFE_NULL, STL_FALSE,  "Allow a NULL fetch without indicator variable" },
    { "include-path",  'I',                    STL_TRUE,   "Describe header file path" },
    { "no-lineinfo",   ZTP_OPTION_NO_LINEINFO, STL_FALSE,  "Exclude line infomation" },
//    { "parse",         'p',                    STL_TRUE,   "Specify parsing level" },
    { NULL,            ZTP_OPTION_NA,          STL_FALSE,  NULL }
};

stlStatus ztpParseOptions( stlInt32       aArgc,
                           stlChar       *aArgv[],
                           stlAllocator  *aAllocator,
                           stlErrorStack *aErrorStack )
{
    stlGetOpt         sOpt;
    stlBool           sInvalidOption = STL_FALSE;
    stlInt8           sCh;
    stlInt32          i;
    const stlChar    *sOptionArg;

    STL_TRY_THROW( aArgc >= 2, RAMP_ERR_USAGE );

    /**
     * Argument parsing
     */
    for( i = 1; i < aArgc; i++ )
    {
        if( aArgv[i][0] == '-' )
        {
            break;
        }

        stlStrncpy(gInFilePath, aArgv[i], STL_MAX_FILE_PATH_LENGTH + 1);
    }

    /*
    stlSnprintf( gIncludePath.mIncludePath, STL_MAX_FILE_PATH_LENGTH + 1,
                 "%susr%sinclude", STL_PATH_SEPARATOR, STL_PATH_SEPARATOR );
    */
    stlStrncpy( gIncludePath.mIncludePath, ".", STL_MAX_FILE_PATH_LENGTH + 1 );
    gIncludePath.mNext = NULL;

    /**
     * Option argument parsing
     */
    if( i < aArgc )
    {
        STL_TRY(stlInitGetOption(&sOpt,
                                 aArgc - (i - 1),
                                 (const stlChar* const *)&aArgv[i - 1],
                                 aErrorStack)
                == STL_SUCCESS);

        while((sOpt.mIndex + (i - 1)) < aArgc)
        {
            if( stlGetOptionLong( &sOpt,
                                  gOptOption,
                                  (stlChar *)&sCh,
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
                stlStrncpy(gInFilePath, aArgv[sOpt.mIndex + (i - 1)], STL_MAX_FILE_PATH_LENGTH + 1);
                sOpt.mIndex ++;
                continue;
            }
        
            switch(sCh)
            {
                case 'n':
                    gDisplayVersion = STL_FALSE;
                    break;
                case 'h':
                    STL_THROW(RAMP_ERR_USAGE);
                    break;
                case 'v':
                    ztpShowVersion();
                    STL_TRY(STL_FALSE);
                    break;
                case 'o':
                    stlStrncpy(gOutFilePath, sOptionArg, STL_MAX_FILE_PATH_LENGTH + 1);
                    gIsPathGiven = STL_TRUE;
                    break;
                case ZTP_OPTION_UNSAFE_NULL:
                    gUnsafeNull = STL_TRUE;
                    break;
                case ZTP_OPTION_NO_LINEINFO:
                    gNoLineInfo = STL_TRUE;
                    break;
                case 'I':
                    STL_TRY( ztpuAddIncludeDir( aAllocator,
                                                aErrorStack,
                                                sOptionArg )
                             == STL_SUCCESS );
                    break;
                    /*
                case 'p':
                    if( stlStrncmp( sOptionArg, "max", 3 ) == 0 )
                    {
                        gParsingLevel = ZTP_PARSING_LEVEL_MAXIMUM;
                    }
                    else if( stlStrncmp( sOptionArg, "medium", 6 ) == 0 )
                    {
                        gParsingLevel = ZTP_PARSING_LEVEL_MEDIUM;
                    }
                    else if( stlStrncmp( sOptionArg, "min", 3 ) == 0 )
                    {
                        gParsingLevel = ZTP_PARSING_LEVEL_MINIMUM;
                    }
                    else
                    {
                        STL_THROW( RAMP_ERR_USAGE );
                    }
                    break;
                    */
                default:
                    sInvalidOption = STL_TRUE;
                    break;
            }
        }

        STL_TRY_THROW(sInvalidOption != STL_TRUE, RAMP_ERR_USAGE);
    }
    else
    {
        /**
         * Copyright, Version 정보 출력
         */
        if(gDisplayVersion == STL_TRUE)
        {
            ztpShowVersion();
        }
        else
        {
            /**
             * Regression Test 를 위해 Copyright, Version 정보를 출력하지 않는다.
             */
        }
    }

    stlPrintf("FileName: %s\n", gInFilePath);

    if(gUnsafeNull == STL_TRUE)
    {
        stlPrintf("Option : --unsafe-null\n");
    }

    return STL_SUCCESS;

    STL_CATCH(RAMP_ERR_USAGE)
    {
        /**
         * Copyright, Version 정보 출력
         */
        ztpShowVersion();
        ztpPrintUsage(aArgv[0]);
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztpShowVersion()
{
    stlPrintf("\n");
    stlShowCopyright();
    stlShowVersionInfo();
    stlPrintf("\n");

    return STL_SUCCESS;
}

stlStatus ztpPrintUsage(stlChar *aProgramName)
{
    stlInt32   i = 0;

    stlPrintf("%s is the " STL_PRODUCT_NAME " embedded SQL preprocessor for C programs.\n\n",
              aProgramName);
    stlPrintf("Usage:\n"
              "  %s [OPTION]... <input file>\n\n",
              aProgramName);
    stlPrintf("Options:\n");
    for(i = 0; gOptOption[i].mName != NULL; i ++)
    {
        stlPrintf("  --%-13s%s\n", gOptOption[i].mName, gOptOption[i].mDescription);
    }
    stlPrintf("\n");

    return STL_SUCCESS;
}

stlStatus ztpConvertFileStream(ztpConvertContext *aContext)
{
    stlInt32        sState = 0;
    ztpParseTree   *sParseTree;

    STL_TRY( ztpuReadFile(aContext->mInFile,
                          aContext->mAllocator,
                          aContext->mErrorStack,
                          &aContext->mBuffer,
                          &aContext->mLength )
            == STL_SUCCESS );

    STL_TRY( ztpuWriteOutputHeader(aContext->mOutFile,
                                   aContext->mInFileName,
                                   aContext->mOutFileName,
                                   gOutputHeader,
                                   gOutputHeaderSize,
                                   aContext->mErrorStack )
            == STL_SUCCESS );

    gRootSymTab = NULL;
    gCurrSymTab = NULL;

    STL_TRY(ztpCreateSymTab(aContext->mAllocator,
                            aContext->mErrorStack)
            == STL_SUCCESS);

    STL_TRY(ztpCParseIt(aContext,
                        &sParseTree)
            == STL_SUCCESS);

    STL_TRY(ztpDestroySymTab(aContext->mAllocator,
                             aContext->mErrorStack)
            == STL_SUCCESS);

    return STL_SUCCESS;

    STL_FINISH;

    switch(sState)
    {
        case 1:
            break;
        default:
            break;
    }

    /**
     * 구문 분석이 실패한 경우 에러를 출력한다
     */
    if(stlGetErrorStackDepth(aContext->mErrorStack) > 0)
    {
        ztpPrintErrorStack(aContext->mErrorStack);
    }

    return STL_FAILURE;
}

int main( int    aArgc,
          char * aArgv[] )
{
    stlUInt32          sState = 0;
    stlAllocator       sAllocator;
    stlErrorStack      sErrorStack;
    ztpConvertContext  sContext;
    stlFile            sInFile;
    stlFile            sOutFile;
    stlBool            sFileExist = STL_FALSE;

    STL_INIT_ERROR_STACK( &sErrorStack );

    stlMemset( &sContext, 0x00, STL_SIZEOF(ztpConvertContext) );

    STL_TRY( dtlInitialize() == STL_SUCCESS );
    sState = 1;

#ifndef WIN32
#ifndef STL_DEBUG
    /**
     * Relase 모드에서는 Ctrl-C를 무시한다.
     */
    STL_TRY( stlBlockSignal( STL_SIGNAL_INT,
                             &sErrorStack )
             == STL_SUCCESS );
#endif
#endif

    stlRegisterErrorTable( STL_ERROR_MODULE_GLIESE_TOOL_GPEC,
                           gZtpErrorTable );

    STL_TRY( stlCreateRegionAllocator( &sAllocator,
                                       ZTP_REGION_INIT_SIZE,
                                       ZTP_REGION_NEXT_SIZE,
                                       &sErrorStack )
              == STL_SUCCESS );

    sState = 2;

    /* Option parsing 부분이 들어가야 함 */
    /*********************************/
    STL_TRY( ztpParseOptions( aArgc,
                              aArgv,
                              &sAllocator,
                              &sErrorStack )
             == STL_SUCCESS );

    /* include path 등 필요한 환경 변수들 setting이 들어가야 함 */
    /*****************************************************/
    gOutputHeaderSize = stlStrlen(gOutputHeader);
    STL_TRY( stlAllocHeap( (void**)&gSqlBuffer,
                           gSqlBufLen,
                           &sErrorStack )
             == STL_SUCCESS );

    sState = 3;

    /*************************************/
    /* input file에 대하여 pre-compile 실행 */
    /*************************************/
    ztpuMakeInFilePath();
    if( gIsPathGiven == STL_FALSE )
    {
        ztpuMatchInOutFilePath();
    }

    sContext.mAllocator = &sAllocator;
    sContext.mErrorStack = &sErrorStack;

    STL_TRY( stlExistFile( gInFilePath,
                           &sFileExist,
                           &sErrorStack )
             == STL_SUCCESS );

    STL_TRY_THROW( sFileExist == STL_TRUE, ERR_FILE_NOTFOUND );

    stlPrintf("Pre-compile %s -> %s\n", gInFilePath, gOutFilePath);
    sContext.mInFileName = gInFilePath;
    sContext.mOutFileName = gOutFilePath;
    sContext.mNeedPrint = STL_TRUE;

    STL_TRY(stlOpenFile(&sInFile,
                        gInFilePath,
                        STL_FOPEN_READ,
                        STL_FPERM_OS_DEFAULT,
                        &sErrorStack) == STL_SUCCESS);
    sContext.mInFile = &sInFile;
    sState = 4;

    STL_TRY(stlOpenFile(&sOutFile,
                        gOutFilePath,
                        STL_FOPEN_READ | STL_FOPEN_WRITE | STL_FOPEN_CREATE | STL_FOPEN_TRUNCATE,
                        STL_FPERM_OS_DEFAULT,
                        &sErrorStack) == STL_SUCCESS);
    sContext.mOutFile = &sOutFile;
    sState = 5;

    STL_TRY(ztpConvertFileStream(&sContext) == STL_SUCCESS);

    sState = 4;
    STL_TRY(stlCloseFile(sContext.mOutFile, &sErrorStack) == STL_SUCCESS);

    sState = 3;
    STL_TRY(stlCloseFile(sContext.mInFile, &sErrorStack) == STL_SUCCESS);

    sState = 2;
    stlFreeHeap(gSqlBuffer);
    gSqlBuffer = NULL;

    sState = 1;
    STL_TRY(stlDestroyRegionAllocator(&sAllocator,
                                      &sErrorStack)
            == STL_SUCCESS);

    sState = 0;
    STL_TRY(dtlTerminate() == STL_SUCCESS);

    return STL_SUCCESS;

    STL_CATCH( ERR_FILE_NOTFOUND );
    {
        stlPrintf("File Not Found : %s\n", gInFilePath);
        stlPrintf("Program exit!!\n");
    }
    STL_FINISH;

    switch( sState )
    {
        case 5:
            (void)stlCloseFile(sContext.mOutFile, &sErrorStack);
        case 4:
            (void)stlCloseFile(sContext.mInFile, &sErrorStack);
        case 3:
            stlFreeHeap( gSqlBuffer );
            gSqlBuffer = NULL;
            gSqlBufferIdx = 0;
        case 2:
            (void)stlDestroyRegionAllocator(&sAllocator, &sErrorStack);
        case 1:
            (void)dtlTerminate();
        default:
            break;
    }

    return STL_FAILURE;
}

/** @} */
