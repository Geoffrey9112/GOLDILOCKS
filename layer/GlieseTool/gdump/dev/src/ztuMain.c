/*******************************************************************************
 * ztuMain.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: ztuMain.c $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#include <dtl.h>
#include <stl.h>
#include <sml.h>
#include <ztuDef.h>
#include <ztuMain.h>

stlGetOptOption ztuOptOption[] =
{
    { "silent",  'S',  STL_FALSE, "silent" },
    /* control file */
    { "section", 's',  STL_TRUE,  "dump controlfile section(default all)" },
    /* data file */
    { "header",  'h',  STL_FALSE, "dump datafile header" },
    { "number",  'n',  STL_TRUE,  "(log or page) sequence number" },
    /* log file*/
    { "offset",  'o',  STL_TRUE,  "offset of lsn(usable if set lsn)" },
    /* etc */
    { "fetch",   'f',  STL_TRUE,  "dump as much as count" },
    /* incremental back file */
    { "body",    'b',  STL_TRUE,  "dump incremental body(default none)" },
    { "tbs",     't',  STL_TRUE,  "dump specific tablespace body(usable if set body)" },
    { NULL,      '\0', STL_FALSE, NULL }
};

void ztuUsage()
{
    stlPrintf( "Usage:\n" );
    stlPrintf( " gdump file_type file_name [options]\n"
               "\n"
               "Arguments:\n"
               "\n"
               "   file_type: CONTROL | LOG | DATA | PROPERTY | BACKUP\n"
               "   file_name: file name to dump\n"
               "\n"
               "options:\n"
               "\n"
               "      -S  --silent                                  %s\n"
               "   file_type = CONTROL\n"
               "      -s, --section  sys | log | db | backup | all  %s\n"
               "   file_type = DATA\n"
               "      -h, --header                                  %s\n"
               "      -n, --number   INTEGER ( >= 0 )               %s\n"
               "      -f, --fetch    INTEGER ( >= 1 )               %s\n"
               "   file_type = LOG\n"
               "      -n, --number   INTEGER ( >= 0 )               %s\n"
               "      -o, --offset   INTEGER                        %s\n"
               "      -f, --fetch    INTEGER ( >= 1 )               %s\n"
               "   file_type = BACKUP\n"
               "      -b, --body     none | header | all            %s\n"
               "      -t, --tbs                                     %s\n"
               "      -n, --number   INTEGER ( >= 0 )               %s\n"
               "      -f, --fetch    INTEGER ( >= 1 )               %s\n"
               "\n"
               "\n"
               "PLEASE NOTE:\n"
               "   Type must be specified matched File.\n"
               "\n",
               ztuOptOption[0].mDescription,
               ztuOptOption[1].mDescription,
               ztuOptOption[2].mDescription,
               ztuOptOption[3].mDescription,
               ztuOptOption[5].mDescription,
               ztuOptOption[3].mDescription,
               ztuOptOption[4].mDescription,
               ztuOptOption[5].mDescription,
               ztuOptOption[6].mDescription,
               ztuOptOption[7].mDescription,
               ztuOptOption[3].mDescription,
               ztuOptOption[5].mDescription );
}

void ztuInitializeArguments( ztuArguments * aArguments )
{
    stlMemset( aArguments, 0x00, STL_SIZEOF( ztuArguments ) );

    aArguments->mFileType     = ZTU_FILE_TYPE_INVALID;
    aArguments->mCtrlSection  = ZTU_CTRL_SECTION_ALL;
    aArguments->mDataHeader   = STL_FALSE;
    aArguments->mNum          = SML_DUMP_INVALID_VALUE;
    aArguments->mOffset       = SML_DUMP_INIT_VALUE;
    aArguments->mFetchCnt     = SML_DUMP_INVALID_VALUE;
    aArguments->mDumpBody     = SML_DUMP_INIT_VALUE;
    aArguments->mTbsId        = SML_INVALID_TBS_ID;
    aArguments->mSilent       = STL_FALSE;
}

stlStatus ztuParseArguments( stlInt32       aArgc,
                             stlChar      * aArgv[],
                             ztuArguments * aArguments,
                             smlEnv       * aEnv )
{
    stlInt32        i = 0;
    stlChar         sCh;
    const stlChar * sOptionArg;
    stlGetOpt       sOpt;
    stlInt64        sInt64;

    if( aArgc < 2 )
    {
        STL_THROW( RAMP_ERR_USAGE );
    }
    
    for( i = 1; i < aArgc; i++ )
    {
        if( aArgv[i][0] == '-' )
        {
            break;
        }

        switch( i )
        {
            case 2:
                STL_TRY_THROW( stlStrlen( aArgv[i] ) < STL_MAX_FILE_PATH_LENGTH,
                               RAMP_ERR_PATH_LEN );
                    
                stlStrncpy( aArguments->mFileName, aArgv[i], STL_MAX_FILE_PATH_LENGTH );
                break;
            case 1:
                if( stlStrcasecmp( aArgv[i], ZTU_STR_FILE_TYPE_DATAFILE ) == 0 )
                {
                    aArguments->mFileType = ZTU_FILE_TYPE_DATAFILE;
                }
                else if( stlStrcasecmp( aArgv[i], ZTU_STR_FILE_TYPE_CONTROLFILE ) == 0)
                {
                    aArguments->mFileType = ZTU_FILE_TYPE_CONTROLFILE;
                }
                else if( stlStrcasecmp( aArgv[i], ZTU_STR_FILE_TYPE_LOG ) == 0)
                {
                    aArguments->mFileType = ZTU_FILE_TYPE_LOG;
                }
                else if( stlStrcasecmp( aArgv[i], ZTU_STR_FILE_TYPE_PROPERTY ) == 0)
                {
                    aArguments->mFileType = ZTU_FILE_TYPE_PROPERTY;
                }
                else if( stlStrcasecmp( aArgv[i], ZTU_STR_FILE_TYPE_BACKUP ) == 0 )
                {
                    aArguments->mFileType = ZTU_FILE_TYPE_BACKUP;
                }
                else
                {
                    STL_THROW( RAMP_ERR_USAGE );
                }
                break;
            default:
                break;
        }
    }

    if( i < aArgc )
    {
       STL_TRY( stlInitGetOption( &sOpt,
                                   aArgc - (i - 1),
                                   (const stlChar* const *)&aArgv[i - 1],
                                   KNL_ERROR_STACK( aEnv ) )
                 == STL_SUCCESS );

        while( STL_TRUE )
        {
            if( stlGetOptionLong( &sOpt,
                                  ztuOptOption,
                                  &sCh,
                                  &sOptionArg,
                                  KNL_ERROR_STACK( aEnv ) ) != STL_SUCCESS )
            {
                if( stlGetLastErrorCode( KNL_ERROR_STACK( aEnv ) ) != STL_ERRCODE_EOF )
                {
                    if( stlGetLastErrorCode( KNL_ERROR_STACK( aEnv ) )
                        == STL_ERRCODE_INVALID_ARGUMENT )
                    {
                        STL_THROW( RAMP_ERR_USAGE );
                    }
                    
                    STL_TRY( STL_FALSE );
                }
                
                stlPopError( KNL_ERROR_STACK( aEnv ) );
                break;
            }
        
            switch( sCh )
            {
                case 'S':
                    aArguments->mSilent = STL_TRUE;
                    break;
                case 's' :
                    if( stlStrcasecmp(sOptionArg, ZTU_STR_CTRL_SECTION_BACKUP) == 0)
                    {
                        aArguments->mCtrlSection = SML_DUMP_CTRLFILE_SECTION_BACKUP;
                    }
                    else if( stlStrcasecmp(sOptionArg, ZTU_STR_CTRL_SECTION_SYS) == 0)
                    {
                        aArguments->mCtrlSection = SML_DUMP_CTRLFILE_SECTION_SYS;
                    }
                    else if( stlStrcasecmp(sOptionArg, ZTU_STR_CTRL_SECTION_LOG) == 0)
                    {
                        aArguments->mCtrlSection = SML_DUMP_CTRLFILE_SECTION_LOG;
                    }
                    else if( stlStrcasecmp(sOptionArg, ZTU_STR_CTRL_SECTION_DB) == 0)
                    {
                        aArguments->mCtrlSection = SML_DUMP_CTRLFILE_SECTION_DB;
                    }
                    else if( stlStrcasecmp(sOptionArg, ZTU_STR_CTRL_SECTION_ALL) == 0)
                    {
                        aArguments->mCtrlSection = SML_DUMP_CTRLFILE_SECTION_ALL;
                    }
                    else
                    {
                        STL_THROW( RAMP_ERR_USAGE );
                    }
                    break;
                case 'h' :
                    aArguments->mDataHeader = STL_TRUE;
                    break;
                case 'n' :
                    STL_TRY( stlStrToInt64( (const stlChar*) sOptionArg,
                                            stlStrlen( sOptionArg ),
                                            NULL,
                                            10,
                                            &sInt64,
                                            KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

                    STL_TRY_THROW( sInt64 >= 0, RAMP_ERR_USAGE );
                    aArguments->mNum = sInt64;
                    break;
                case 'o' :
                    STL_TRY( stlStrToInt64( (const stlChar*) sOptionArg,
                                            stlStrlen( sOptionArg ),
                                            NULL,
                                            10,
                                            &sInt64,
                                            KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

                    aArguments->mOffset = sInt64;
                    break;
                case 'f' :
                    STL_TRY( stlStrToInt64( (const stlChar*) sOptionArg,
                                            stlStrlen( sOptionArg ),
                                            NULL,
                                            10,
                                            &sInt64,
                                            KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

                    STL_TRY_THROW( sInt64 > 0, RAMP_ERR_USAGE );
                    aArguments->mFetchCnt = sInt64;
                    break;
                case 'b':
                    if( stlStrcasecmp(sOptionArg, ZTU_STR_INC_BACKUP_BODY_HEADER) == 0)
                    {
                        aArguments->mDumpBody = SML_DUMP_PAGE_HEADER;
                    }
                    else if( stlStrcasecmp(sOptionArg, ZTU_STR_INC_BACKUP_BODY_ALL) == 0)
                    {
                        aArguments->mDumpBody = SML_DUMP_PAGE_ALL;
                    }
                    else if( stlStrcasecmp(sOptionArg, ZTU_STR_INC_BACKUP_BODY_NONE) == 0)
                    {
                        aArguments->mDumpBody = SML_DUMP_INIT_VALUE;
                    }
                    else
                    {
                        STL_THROW( RAMP_ERR_USAGE );
                    }
                    break;
                case 't':
                    STL_TRY( stlStrToInt64( (const stlChar*) sOptionArg,
                                            stlStrlen( sOptionArg ),
                                            NULL,
                                            10,
                                            &sInt64,
                                            KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

                    STL_TRY_THROW( sInt64 >= 0, RAMP_ERR_USAGE );
                    aArguments->mTbsId = (stlUInt16) sInt64;
                    break;
                default :
                    STL_THROW( RAMP_ERR_USAGE );
                    break;
            }
        }
    }

    if( aArguments->mSilent == STL_FALSE )
    {
        stlPrintf( "\n" );
        stlShowCopyright();
        stlShowVersionInfo();
        stlPrintf( "\n" );
    }
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_USAGE )
    {
        ztuUsage();
    }

    STL_CATCH( RAMP_ERR_PATH_LEN )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_NAMETOOLONG,
                      NULL,
                      KNL_ERROR_STACK( aEnv ),
                      sOptionArg );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

void ztuGetFileTypeString( ztuArguments * aArguments,
                           stlChar      * aFormatStr )
{
    switch( aArguments->mFileType )
    {
        case ZTU_FILE_TYPE_CONTROLFILE:
            stlStrncpy( aFormatStr, "CONTROLFILE", ZTU_FILE_TYPE_STRING_MAX_LENGTH);
            break;
        case ZTU_FILE_TYPE_LOG:
            stlStrncpy( aFormatStr, "LOG FILE", ZTU_FILE_TYPE_STRING_MAX_LENGTH);
            break;
        case ZTU_FILE_TYPE_DATAFILE:
            if( aArguments->mDataHeader == STL_TRUE )
            {
                stlStrncpy( aFormatStr,
                            "DATAFILE HEADER",
                            ZTU_FILE_TYPE_STRING_MAX_LENGTH);
            }
            else
            {
                stlStrncpy( aFormatStr,
                            "DATAFILE PAGE",
                            ZTU_FILE_TYPE_STRING_MAX_LENGTH);
            }
            break;
        case ZTU_FILE_TYPE_PROPERTY:
            stlStrncpy( aFormatStr, "PROPERTY-BINARY", ZTU_FILE_TYPE_STRING_MAX_LENGTH);
            break;
        case ZTU_FILE_TYPE_BACKUP:
            stlStrncpy( aFormatStr, "INCREMENTAL BACKUP", ZTU_FILE_TYPE_STRING_MAX_LENGTH);
        default:
            break;
    }
}

void ztuPrintTime()
{
    stlExpTime sExpTime;

    stlMakeExpTimeByLocalTz( &sExpTime, stlNow() );

    stlPrintf( "TIME: %04d-%02d-%02d %02d:%02d:%02d.%06d\n",
               sExpTime.mYear + 1900,
               sExpTime.mMonth + 1,
               sExpTime.mDay,
               sExpTime.mHour,
               sExpTime.mMinute,
               sExpTime.mSecond,
               sExpTime.mMicroSecond );
}

int main( int     aArgc,
          char ** aArgv )
{
    smlEnv          sEnv;
    smlSessionEnv   sSessionEnv;
    ztuArguments    sArguments;
    stlInt32        sState = 0;
    stlChar         sFormatStr[ZTU_FILE_TYPE_STRING_MAX_LENGTH] = { '\0' };
    stlExpTime      sExpTime;

    STL_TRY( smlInitialize() == STL_SUCCESS );

    STL_TRY( smlInitializeEnv( &sEnv, KNL_ENV_HEAP ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( smlInitializeSessionEnv( &sSessionEnv,
                                      KNL_CONNECTION_TYPE_NONE,
                                      KNL_SESSION_TYPE_DEDICATE,
                                      KNL_SESS_ENV_HEAP,
                                      &sEnv ) == STL_SUCCESS );
    sState = 2;

    KNL_LINK_SESS_ENV( &sEnv, &sSessionEnv );

    ztuInitializeArguments( &sArguments );
    
    STL_TRY( ztuParseArguments( aArgc,
                                aArgv,
                                &sArguments,
                                &sEnv ) == STL_SUCCESS );

    if( sArguments.mSilent == STL_FALSE )
    {
        ztuGetFileTypeString( &sArguments,
                              sFormatStr );
        
        stlPrintf( "===========================================================\n" );
        stlPrintf( "FILE: %s\n", sArguments.mFileName );
        stlPrintf( "TYPE: %s\n", sFormatStr );
        ztuPrintTime();    
        stlPrintf( "===========================================================\n" );
    }

    switch( sArguments.mFileType )
    {
        case ZTU_FILE_TYPE_CONTROLFILE:
            STL_TRY( smlDumpControlFile( sArguments.mFileName,
                                         sArguments.mCtrlSection,
                                         &sEnv ) == STL_SUCCESS );
            break;
        case ZTU_FILE_TYPE_LOG:
            STL_TRY( smlDumpLogFile( sArguments.mFileName,
                                     sArguments.mNum,
                                     sArguments.mOffset,
                                     sArguments.mFetchCnt,
                                     &sEnv ) == STL_SUCCESS );
            break;
        case ZTU_FILE_TYPE_DATAFILE:
            sArguments.mNum = ( (sArguments.mNum == SML_DUMP_INVALID_VALUE) ?
                                0 : sArguments.mNum );
            if( sArguments.mDataHeader == STL_TRUE )
            {
                STL_TRY( smlDumpDatafileHeader( sArguments.mFileName,
                                                &sEnv ) == STL_SUCCESS );
            }
            else
            {
                STL_TRY( smlDumpDatafilePage( sArguments.mFileName,
                                              sArguments.mNum,
                                              sArguments.mFetchCnt,
                                              &sEnv ) == STL_SUCCESS );
            }
            break;
        case ZTU_FILE_TYPE_PROPERTY:
            STL_TRY( knlDumpBinaryPropertyFile( sArguments.mFileName,
                                                KNL_ENV( &sEnv ) ) == STL_SUCCESS );
            break;
        case ZTU_FILE_TYPE_BACKUP:
            STL_TRY( smlDumpIncBackupFile( sArguments.mFileName,
                                           sArguments.mDumpBody,
                                           sArguments.mTbsId,
                                           sArguments.mNum,
                                           sArguments.mFetchCnt,
                                           &sEnv ) == STL_SUCCESS );
            break;
        default:
            STL_THROW( RAMP_ERR_USAGE );
            break;
    }

    sState = 1;
    STL_TRY( smlFinalizeSessionEnv( &sSessionEnv,
                                    &sEnv ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( smlFinalizeEnv( &sEnv ) == STL_SUCCESS );

    if( sArguments.mSilent == STL_FALSE )
    {
        stlMakeExpTimeByLocalTz(&sExpTime, stlNow() );
        
        stlPrintf( "\n===========================================================\n" );
        ztuPrintTime();
        stlPrintf( "===========================================================\n" );
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_USAGE )
    {
        ztuUsage();
    }
    
    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void)smlFinalizeSessionEnv( &sSessionEnv, &sEnv );
        case 1:
            stlTestPrintErrorStack( KNL_ERROR_STACK( &sEnv ) );
            (void)smlFinalizeEnv( &sEnv );
        default:
            break;
    }
    
    return STL_FAILURE;
}
