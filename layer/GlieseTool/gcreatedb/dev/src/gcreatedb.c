/*******************************************************************************
 * createdb.c
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

#include <dtl.h>
#include <scl.h>
#include <ell.h>
#include <qll.h>

#define ZTB_MAX_BUFFER    ( 4096 )

stlChar gZtbDatabaseName[ZTB_MAX_BUFFER];
stlChar gZtbDatabaseComment[ZTB_MAX_BUFFER];
stlChar gZtbTimezone[ZTB_MAX_BUFFER];
stlChar gZtbCharacterSet[ZTB_MAX_BUFFER];
stlChar gZtbCharLengthUnits[ZTB_MAX_BUFFER];
stlBool gZtbSilentMode;
stlBool gZtbHelp;

knlStartupFunc * gZtbStartupLayer[STL_LAYER_MAX] =
{
    NULL,       /* STL_LAYER_NONE */
    NULL,       /* STL_LAYER_STANDARD */
    NULL,       /* STL_LAYER_DATATYPE */
    NULL,       /* STL_LAYER_COMMUNICATION */
    gStartupKN, /* STL_LAYER_KERNEL */
    gStartupSC, /* STL_LAYER_SERVER_COMMUNICATION */
    gStartupSM, /* STL_LAYER_STORAGE_MANAGER */
    gStartupEL, /* STL_LAYER_EXECUTION_LIBRARY */
    NULL,       /* STL_LAYER_SQL_PROCESSOR */
    NULL,       /* STL_LAYER_PSM_PROCESSOR */
    NULL,       /* STL_LAYER_SESSION */
    NULL,       /* STL_LAYER_INSTANCE_MANAGER */
    NULL,       /* STL_LAYER_GLIESE_LIBRARY */
    NULL,       /* STL_LAYER_GLIESE_TOOL */
    NULL,       /* STL_LAYER_GOLDILOCKS_LIBRARY */
    NULL,       /* STL_LAYER_GOLDILOCKS_TOOL */
};


knlShutdownFunc * gZtbShutdownLayer[STL_LAYER_MAX] =
{
    NULL,        /* STL_LAYER_NONE */
    NULL,        /* STL_LAYER_STANDARD */
    NULL,        /* STL_LAYER_DATATYPE */
    NULL,        /* STL_LAYER_COMMUNICATION */
    gShutdownKN, /* STL_LAYER_KERNEL */
    gShutdownSC, /* STL_LAYER_SERVER_COMMUNICATION */
    gShutdownSM, /* STL_LAYER_STORAGE_MANAGER */
    gShutdownEL, /* STL_LAYER_EXECUTION_LIBRARY */
    NULL,        /* STL_LAYER_SQL_PROCESSOR */
    NULL,        /* STL_LAYER_PSM_PROCESSOR */
    NULL,        /* STL_LAYER_SESSION */
    NULL,        /* STL_LAYER_INSTANCE_MANAGER */
    NULL,        /* STL_LAYER_GLIESE_LIBRARY */
    NULL,        /* STL_LAYER_GLIESE_TOOL */
    NULL,        /* STL_LAYER_GOLDILOCKS_LIBRARY */
    NULL,        /* STL_LAYER_GOLDILOCKS_TOOL */
};

stlGetOptOption gZtbOptOption[] =
{ 
    { "db_name",                'd',  STL_TRUE,   "database name" },
    { "db_comment",             'm',  STL_TRUE,   "database comment" },
    { "timezone",               't',  STL_TRUE,   "timezone ( {+/-}{TZH:TZM} )" },
    { "character_set",          'c',  STL_TRUE,   "character set" },
    { "char_length_units",      'l',  STL_TRUE,   "char length units" },
    { "silent",                 's',  STL_FALSE,  "suppresses the display of the result message" },
    { "help",                   'h',  STL_FALSE,  "print help message" },
    { NULL,                     '\0', STL_FALSE,  NULL }
}; 

void ztbPrintUsage( stlChar * aProgramName )
{
    stlChar  sCharacterSet[ZTB_MAX_BUFFER];
    stlChar  sCharLengthUnits[ZTB_MAX_BUFFER];
    stlInt32 i;

    /**
     * construct character set message
     */
    
    sCharacterSet[0] = '\0';

    stlStrnncat( sCharacterSet,
                 gZtbOptOption[3].mDescription,
                 ZTB_MAX_BUFFER,
                 stlStrlen( gZtbOptOption[3].mDescription ) + 1 );
    stlStrnncat( sCharacterSet,
                 "\n",
                 ZTB_MAX_BUFFER,
                 stlStrlen( "\n" ) + 1 );
    
    for( i = 0; i < DTL_CHARACTERSET_MAX; i++ )
    {
        stlStrnncat( sCharacterSet,
                     "                                ",
                     ZTB_MAX_BUFFER,
                     stlStrlen( "                                " ) + 1 );
        stlStrnncat( sCharacterSet,
                     gDtlCharacterSetString[i],
                     ZTB_MAX_BUFFER,
                     stlStrlen( gDtlCharacterSetString[i] ) + 1 );

        if( i >= (DTL_CHARACTERSET_MAX-1) )
        {
            break;
        }
        
        stlStrnncat( sCharacterSet,
                     "\n",
                     ZTB_MAX_BUFFER,
                     stlStrlen( "\n" ) + 1 );
    }

    /**
     * construct char length units
     */
    
    sCharLengthUnits[0] = '\0';

    stlStrnncat( sCharLengthUnits,
                 gZtbOptOption[4].mDescription,
                 ZTB_MAX_BUFFER,
                 stlStrlen( gZtbOptOption[4].mDescription ) + 1 );
    stlStrnncat( sCharLengthUnits,
                 "\n                                ",
                 ZTB_MAX_BUFFER,
                 stlStrlen( "\n                                " ) + 1 );
    stlStrnncat( sCharLengthUnits,
                 gDtlLengthUnitString[DTL_STRING_LENGTH_UNIT_OCTETS],
                 ZTB_MAX_BUFFER,
                 stlStrlen( gDtlLengthUnitString[DTL_STRING_LENGTH_UNIT_OCTETS] ) + 1 );
    stlStrnncat( sCharLengthUnits,
                 "\n                                ",
                 ZTB_MAX_BUFFER,
                 stlStrlen( "\n                                " ) + 1 );
    stlStrnncat( sCharLengthUnits,
                 gDtlLengthUnitString[DTL_STRING_LENGTH_UNIT_CHARACTERS],
                 ZTB_MAX_BUFFER,
                 stlStrlen( gDtlLengthUnitString[DTL_STRING_LENGTH_UNIT_CHARACTERS] ) + 1 );

    stlPrintf( "\n"
               "Usage \n"
               "\n"
               "    %s [options]\n"
               "\n"
               "Options:\n"
               "\n"
               "    --db_name                 %s\n"
               "    --db_comment              %s\n"
               "    --timezone                %s\n"
               "    --character_set           %s\n"
               "    --char_length_units       %s\n"
               "    --silent                  %s\n"
               "    --help                    %s\n"
               "\n",
               aProgramName,
               gZtbOptOption[0].mDescription,    /* db_name */
               gZtbOptOption[1].mDescription,    /* db_comment */
               gZtbOptOption[2].mDescription,    /* timezone */
               sCharacterSet,                    /* character_set */
               sCharLengthUnits,                 /* char_length_units */
               gZtbOptOption[5].mDescription,    /* silent */
               gZtbOptOption[6].mDescription );  /* help */
    
    stlPrintf( "examples:\n\n"
               "    %s "
               "--db_name=\"goldilocks\" "
               "--db_comment=\"goldilocks database\" "
               "--timezone=\"+09:00\" " 
               "--character_set=\"UTF8\" "
               "--char_length_units=\"OCTETS\" "
               "--silent \n\n",
               aProgramName );
}

stlStatus ztbParseOptions( stlInt32    aArgc,
                           stlChar   * aArgv[],
                           qllEnv    * aEnv )
{
    stlGetOpt         sOpt;
    stlBool           sInvalidOption = STL_FALSE;
    stlChar           sCh;
    const stlChar   * sOptionArg;
    dtlCharacterSet   sCharacterSet;
    stlChar         * sErrorPosition = NULL;
    stlChar           sDetailMessage[ZTB_MAX_BUFFER];
    
    dtlDataValue                sTimeZoneValue;
    dtlIntervalDayToSecondType  sTimeZoneInterval;
    stlBool                     sTimeZoneWithInfo = STL_FALSE;

    gZtbDatabaseName[0]        = '\0';
    gZtbDatabaseComment[0]     = '\0';
    gZtbTimezone[0]            = '\0';
    gZtbCharacterSet[0]        = '\0';
    gZtbCharLengthUnits[0]     = '\0';
    gZtbSilentMode             = STL_FALSE;
    gZtbHelp                   = STL_FALSE;

    /**
     * Optional arguments
     */
        
    STL_TRY( stlInitGetOption( &sOpt,
                               aArgc,
                               (const stlChar* const *)aArgv,
                               KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );

    while( STL_TRUE )
    {
        sErrorPosition = (stlChar*)sOpt.mArgv[sOpt.mIndex];

        if( stlGetOptionLong( &sOpt,
                              gZtbOptOption,
                              &sCh,
                              &sOptionArg,
                              KNL_ERROR_STACK( aEnv ) ) != STL_SUCCESS )
        {
            if( stlGetLastErrorCode( KNL_ERROR_STACK( aEnv ) ) != STL_ERRCODE_EOF )
            {
                if( stlGetLastErrorCode( KNL_ERROR_STACK( aEnv ) ) ==
                    STL_ERRCODE_INVALID_ARGUMENT )
                {
                    stlPopError( KNL_ERROR_STACK(aEnv) );
                    STL_THROW( RAMP_ERR_USAGE );
                }
                    
                STL_TRY( STL_FALSE );
            }
                
            stlPopError( KNL_ERROR_STACK( aEnv ) );
            break;
        }

        switch( sCh )
        {
            case 'd':
                stlStrncpy( gZtbDatabaseName, sOptionArg, ZTB_MAX_BUFFER );
                break;
            case 'm':
                stlStrncpy( gZtbDatabaseComment, sOptionArg, ZTB_MAX_BUFFER );
                break;
            case 't':
                stlStrncpy( gZtbTimezone, sOptionArg, ZTB_MAX_BUFFER );
                    
                sTimeZoneValue.mValue = & sTimeZoneInterval;
                STL_TRY( dtlInitDataValue( DTL_TYPE_INTERVAL_DAY_TO_SECOND,
                                           STL_SIZEOF(dtlIntervalDayToSecondType),
                                           & sTimeZoneValue,
                                           KNL_ERROR_STACK(aEnv) )
                         == STL_SUCCESS );
                    
                STL_TRY_THROW( dtlIntervalDayToSecondSetValueFromString(
                                   gZtbTimezone,
                                   stlStrlen( gZtbTimezone ),
                                   DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION,
                                   DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION,
                                   DTL_STRING_LENGTH_UNIT_NA,
                                   DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE,
                                   DTL_INTERVAL_DAY_TO_SECOND_SIZE,
                                   & sTimeZoneValue,
                                   & sTimeZoneWithInfo,
                                   KNL_DT_VECTOR(aEnv),
                                   aEnv,
                                   KNL_DT_VECTOR(aEnv),
                                   aEnv,
                                   KNL_ERROR_STACK(aEnv) ) == STL_SUCCESS,
                               RAMP_ERR_USAGE );
                break;
            case 'c':
                stlStrncpy( gZtbCharacterSet, sOptionArg, ZTB_MAX_BUFFER );
                    
                STL_TRY_THROW( dtlGetCharacterSet( gZtbCharacterSet,
                                                   &sCharacterSet,
                                                   KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS,
                               RAMP_ERR_USAGE );
                break;
            case 'l':
                stlStrncpy( gZtbCharLengthUnits, sOptionArg, ZTB_MAX_BUFFER );

                STL_TRY_THROW( (stlStrcasecmp( gZtbCharLengthUnits,
                                               gDtlLengthUnitString[DTL_STRING_LENGTH_UNIT_CHARACTERS] ) == 0) ||
                               (stlStrcasecmp( gZtbCharLengthUnits,
                                               gDtlLengthUnitString[DTL_STRING_LENGTH_UNIT_OCTETS] ) == 0),
                               RAMP_ERR_USAGE );
                break;
            case 's':
                gZtbSilentMode = STL_TRUE;
                break;
            case 'h':
                gZtbHelp = STL_TRUE;
                break;
            default:
                sInvalidOption = STL_TRUE;
                break;
        }
    
        STL_TRY_THROW( sInvalidOption != STL_TRUE, RAMP_ERR_USAGE );
    }

    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_USAGE )
    {
        if( sErrorPosition != NULL )
        {
            stlSnprintf( sDetailMessage,
                         ZTB_MAX_BUFFER,
                         "%s",
                         sErrorPosition );
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          STL_ERRCODE_INVALID_ARGUMENT,
                          sDetailMessage,
                          KNL_ERROR_STACK( aEnv ) );
        }
        else
        {
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          STL_ERRCODE_INVALID_ARGUMENT,
                          NULL,
                          KNL_ERROR_STACK( aEnv ) );
        }
    }

    STL_FINISH;

    /*
     * Copyright, Version 정보 출력
     */
        
    stlPrintf( "\n" );
    stlShowCopyright();
    stlShowVersionInfo();
    stlPrintf( "\n" );

    return STL_FAILURE;
}

stlStatus ztbStartup( knlStartupPhase aStartPhase,
                      knlStartupPhase aEndPhase,
                      ellEnv        * aEnv )
{
    stlInt32         i;
    knlStartupPhase  sPhase;
    knlStartupInfo   sStartupInfo;

    KNL_INIT_STARTUP_ARG( &sStartupInfo );
    sStartupInfo.mDataAccessMode = SML_DATA_ACCESS_MODE_READ_WRITE;

    for ( sPhase = aStartPhase; sPhase <= aEndPhase; sPhase++ )
    {
        for ( i = 0; i < STL_LAYER_MAX; i++ )
        {
            if ( gZtbStartupLayer[i] == NULL )
            {
                continue;
            }
            else
            {
                if ( gZtbStartupLayer[i][sPhase] == NULL )
                {
                    continue;
                }
                else
                {
                    STL_TRY( gZtbStartupLayer[i][sPhase]( &sStartupInfo,
                                                          (void*)aEnv )
                             == STL_SUCCESS );
                }
            }
        }

        knlSetStartupPhase( sPhase );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztbBootdown( ellEnv * aEnv )
{
    stlInt32          i;
    knlShutdownPhase  sPhase;
    knlStartupPhase   sCurStartupPhase;
    knlShutdownInfo    sShutdownInfo;

    sCurStartupPhase = knlGetCurrStartupPhase();

    KNL_INIT_SHUTDOWN_INFO( &sShutdownInfo );
    sShutdownInfo.mShutdownMode  = KNL_SHUTDOWN_MODE_NORMAL;

    /*
     * Boot Down 과정은 Layer 역순으로 수행
     */
    for ( sPhase = knlGetShutdownPhasePair( sCurStartupPhase );
          sPhase >= KNL_SHUTDOWN_PHASE_INIT;
          sPhase-- )
    {
        for ( i = STL_LAYER_MAX - 1; i >= 0; i-- )
        {
            if ( gZtbShutdownLayer[i] == NULL )
            {
                continue;
            }
            else
            {
                if ( gZtbShutdownLayer[i][sPhase] == NULL )
                {
                    continue;
                }
                else
                {
                    STL_TRY( gZtbShutdownLayer[i][sPhase]( &sShutdownInfo,
                                                           (void*)aEnv ) == STL_SUCCESS );
                }
            }
        }
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztbBuildDictionary( stlChar             * aName,
                              stlChar             * aComment,
                              stlChar             * aTimezone,
                              stlChar             * aCharacterSet,
                              dtlStringLengthUnit   aCharLengthUnit,
                              ellEnv              * aEnv )
{
    stlInt32   sState = 0;
    
    smlTransId     sTransID = SML_INVALID_TRANSID;

    smlStatement * sStmt = NULL;

    void         * sRelationHandle = NULL;
    stlTime        sTime;

    dtlCharacterSet     sCharacterSet;

    /*
     * Character Set 상수 치환 
     */
    
    STL_TRY( dtlGetCharacterSet( aCharacterSet,
                                 &sCharacterSet,
                                 KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );

    /*
     * 트랙잭션 시작
     */
    STL_TRY( smlBeginTrans( SML_TIL_READ_COMMITTED,
                            STL_FALSE,  /* aIsGlobalTrans */
                            &sTransID,
                            SML_ENV( aEnv ) )
             == STL_SUCCESS );
    sState = 4;

    /*
     * Statement 생성
     */
    STL_TRY( smlAllocStatement( sTransID,
                                NULL, /* aUpdateRelHandle */
                                SML_STMT_ATTR_UPDATABLE,
                                SML_LOCK_TIMEOUT_INVALID,
                                STL_FALSE,  /* aIsSingleStmtDdl */
                                &sStmt,
                                SML_ENV( aEnv ) )
             == STL_SUCCESS );
    sState = 5;
             
    /**
     * DDL Statement 의 시작 
     */

    sTime = stlNow();
    STL_TRY( ellBeginStmtDDL( sStmt, sTime, aEnv ) == STL_SUCCESS );
    sState = 6;

    /**
     * 최초 Dictionary Table 의 Handle 을 획득
     */
    STL_TRY( smlGetRelationHandle( smlGetFirstDicTableId(),
                                   &sRelationHandle,
                                   SML_ENV( aEnv ) ) == STL_SUCCESS );

    /**
     * Dictionary 구축 (DEFINITION_SCHEMA 생성)
     */
    STL_TRY( ellBuildDatabaseDictionary( sTransID,
                                         sStmt,
                                         aName,
                                         aComment,
                                         smlGetFirstDicTableId(),
                                         sRelationHandle,
                                         SML_MEMORY_DICT_SYSTEM_TBS_ID, 
                                         SML_MEMORY_DATA_SYSTEM_TBS_ID, 
                                         NULL,
                                         SML_MEMORY_UNDO_SYSTEM_TBS_ID, 
                                         NULL,
                                         SML_MEMORY_TEMP_SYSTEM_TBS_ID, 
                                         NULL,
                                         sCharacterSet,
                                         aCharLengthUnit,
                                         NULL,    /* SYS Password */
                                         aTimezone,
                                         aEnv )
             == STL_SUCCESS );

    /**
     * DDL 구문 종료
     */

    STL_TRY( ellEndStmtDDL( sStmt, aEnv ) == STL_SUCCESS );
    sState = 5;
    
    /**
     * Statement 해제 
     */

    STL_TRY( smlFreeStatement( sStmt, SML_ENV( aEnv ) ) == STL_SUCCESS );
    sState = 4;
    sStmt = NULL;
    
    /*
     * 트랙잭션 종료
     */
    
    STL_TRY( ellPreActionCommitDDL( sTransID, aEnv ) == STL_SUCCESS );
    sState = 3;
    
    STL_TRY( smlCommit( sTransID,
                        NULL, /* aComment */
                        SML_TRANSACTION_CWM_WAIT,
                        SML_ENV( aEnv ) ) == STL_SUCCESS );
    sState = 2;
    
    STL_TRY( ellCommitTransDDL( sTransID, aEnv ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( smlEndTrans( sTransID,
                          SML_PEND_ACTION_COMMIT,
                          SML_TRANSACTION_CWM_WAIT,
                          SML_ENV( aEnv ) ) == STL_SUCCESS );
    sState = 0;
    sTransID = SML_INVALID_TRANSID;

    /**
     * Dictionary 마무리 작업 수행 (트랜잭션 종료 후 수행함)
     * - 서비스 운영 가능한 상태가 됨.
     */

    STL_TRY( ellFinishBuildDictionary( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    switch ( sState )
    {
        case 6:
            (void) ellRollbackStmtDDL( sTransID, aEnv );
        case 5:
            (void) smlFreeStatement( sStmt, SML_ENV( aEnv ) );
        case 4:
            (void) ellPreActionRollbackDDL( sTransID, aEnv );
        case 3:
            (void) smlRollback( sTransID,
                                SML_TRANSACTION_CWM_NO_WAIT,
                                SML_ENV( aEnv ) );
        case 2:
            (void) ellRollbackTransDDL( sTransID, aEnv );
        case 1:
            (void) smlEndTrans( sTransID,
                                SML_PEND_ACTION_ROLLBACK,
                                SML_TRANSACTION_CWM_NO_WAIT,
                                SML_ENV( aEnv ) );
        default:
            break;
    }
    
    return STL_FAILURE;
}

void ztbPrintErrorStack( stlErrorStack * aErrorStack )
{
    stlInt32       i;
    stlErrorData * sErrorData;
    stlChar        sSqlState[10];

    if( aErrorStack != NULL )
    {
        for( i = aErrorStack->mTop; i >= 0; i-- )
        {
            sErrorData = &aErrorStack->mErrorData[i];

            stlMakeSqlState( sErrorData->mExternalErrorCode, sSqlState );

            if( stlStrlen( sErrorData->mDetailErrorMessage ) > 0 )
            {
                stlPrintf( "ERR-%s(%d): %s(%s)\n",
                           sSqlState,
                           sErrorData->mErrorCode,
                           sErrorData->mErrorMessage,
                           sErrorData->mDetailErrorMessage );
            }
            else
            {
                stlPrintf( "ERR-%s(%d): %s\n",
                           sSqlState,
                           sErrorData->mErrorCode,
                           sErrorData->mErrorMessage );
            }
        }

        stlPrintf( "\n" );
    }
}

int main( int     aArgc,
          char ** aArgv )
{
    qllEnv          sSystemEnv;
    qllSessionEnv   sSystemSessionEnv;
    qllEnv        * sEnv = NULL;
    qllSessionEnv * sSessionEnv = NULL;
    stlInt32        sState = 0;

    dtlStringLengthUnit         sCharLengthUnit = DTL_STRING_LENGTH_UNIT_NA;
    stlInt32                    sGMTOffset = 0;
    dtlDataValue                sTimeZoneValue;
    dtlIntervalDayToSecondType  sTimeZoneInterval;
    stlBool                     sTimeZoneWithInfo = STL_FALSE;
    
    STL_TRY( qllInitialize() == STL_SUCCESS );

    STL_TRY( qllInitializeEnv( &sSystemEnv, KNL_ENV_HEAP ) == STL_SUCCESS );
    sState = 1;

    STL_TRY( qllInitSessionEnv( &sSystemSessionEnv,
                                KNL_CONNECTION_TYPE_NONE,
                                KNL_SESSION_TYPE_DEDICATE,
                                KNL_SESS_ENV_HEAP,
                                &sSystemEnv ) == STL_SUCCESS );
    sState = 2;

    KNL_LINK_SESS_ENV( &sSystemEnv, &sSystemSessionEnv );

    STL_TRY( ztbParseOptions( aArgc,
                              aArgv,
                              &sSystemEnv )
             == STL_SUCCESS );

    if( gZtbSilentMode == STL_FALSE )
    {
        stlPrintf( "\n" );
        stlShowCopyright();
        stlShowVersionInfo();
        stlPrintf( "\n" );
    }

    if( gZtbHelp == STL_TRUE )
    {
        ztbPrintUsage( aArgv[0] );
        STL_THROW( RAMP_FINISH );
    }

    STL_TRY( ztbStartup( KNL_STARTUP_PHASE_NO_MOUNT,
                         KNL_STARTUP_PHASE_NO_MOUNT,
                         ELL_ENV( &sSystemEnv ) ) == STL_SUCCESS );
    sState = 3;

    STL_TRY( knlCreateEnvPool( 32,
                               STL_SIZEOF( qllEnv ),
                               KNL_ERROR_STACK( &sSystemEnv ) )
             == STL_SUCCESS );
    
    STL_TRY( knlCreateSessionEnvPool( 32,
                                      STL_SIZEOF( qllSessionEnv ),
                                      KNL_ERROR_STACK( &sSystemEnv ) )
             == STL_SUCCESS );

    STL_TRY( knlAddSar( KNL_SAR_SESSION | KNL_SAR_TRANSACTION,
                        KNL_ENV( &sSystemEnv ) )
             == STL_SUCCESS );

    STL_TRY( knlAllocEnv( (void**)&sEnv,
                          KNL_ERROR_STACK( &sSystemEnv ) )
             == STL_SUCCESS );
    sState = 4;

    STL_TRY( qllInitializeEnv( sEnv, KNL_ENV_SHARED ) == STL_SUCCESS );
    sState = 5;

    STL_TRY( knlAllocSessionEnv( (void**)&sSessionEnv,
                                 KNL_ENV( sEnv ) )
             == STL_SUCCESS );
    sState = 6;

    STL_TRY( qllInitSessionEnv( sSessionEnv,
                                KNL_CONNECTION_TYPE_DA,
                                KNL_SESSION_TYPE_DEDICATE,
                                KNL_SESS_ENV_SHARED,
                                sEnv )
             == STL_SUCCESS );
    sState = 7;

    KNL_LINK_SESS_ENV( sEnv, sSessionEnv );

    /**
     * Load default value
     */

    if( gZtbDatabaseName[0] == '\0' )
    {
        stlStrcpy( gZtbDatabaseName, "goldilocks" );
    }
    if( gZtbDatabaseComment[0] == '\0' )
    {
        stlStrcpy( gZtbDatabaseComment, "goldilocks database" );
    }
    if( gZtbTimezone[0] == '\0' )
    {
        STL_TRY( knlGetPropertyValueByID( KNL_PROPERTY_TIMEZONE,
                                          gZtbTimezone,
                                          KNL_ENV( sEnv ) )
                 == STL_SUCCESS );
    }
    if( gZtbCharacterSet[0] == '\0' )
    {
        STL_TRY( knlGetPropertyValueByID( KNL_PROPERTY_CHARACTER_SET,
                                          gZtbCharacterSet,
                                          KNL_ENV( sEnv ) )
                 == STL_SUCCESS );
    }
    if( gZtbCharLengthUnits[0] == '\0' )
    {
        STL_TRY( knlGetPropertyValueByID( KNL_PROPERTY_CHAR_LENGTH_UNITS,
                                          gZtbCharLengthUnits,
                                          KNL_ENV( sEnv ) )
                 == STL_SUCCESS );
    }

    /** 
     * 입력받은 Char Length Units 을 상수 치환 
     */

    STL_TRY( dtlGetCharLengthUnit( gZtbCharLengthUnits,
                                   & sCharLengthUnit,
                                   QLL_ERROR_STACK(sEnv) )
             == STL_SUCCESS );
    
    STL_TRY( qllSetDTFuncVector( sEnv ) == STL_SUCCESS );
    STL_TRY( qllInitSessNLS( sEnv ) == STL_SUCCESS );

    /**
     * 입력받은 Timezone 을 Session NLS 로 설정
     */

    sTimeZoneValue.mValue = & sTimeZoneInterval;
        
    STL_TRY( dtlInitDataValue( DTL_TYPE_INTERVAL_DAY_TO_SECOND,
                               STL_SIZEOF(dtlIntervalDayToSecondType),
                               & sTimeZoneValue,
                               QLL_ERROR_STACK(sEnv) )
             == STL_SUCCESS );

    STL_TRY( dtlIntervalDayToSecondSetValueFromString(
                 gZtbTimezone,
                 stlStrlen( gZtbTimezone ),
                 DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION,
                 DTL_SCALE_NA,
                 DTL_STRING_LENGTH_UNIT_NA,
                 DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE,
                 DTL_INTERVAL_DAY_TO_SECOND_SIZE,
                 & sTimeZoneValue,
                 & sTimeZoneWithInfo,
                 KNL_DT_VECTOR(sEnv),
                 sEnv,
                 KNL_DT_VECTOR(sEnv),
                 sEnv,
                 QLL_ERROR_STACK(sEnv) )
             == STL_SUCCESS );
        
    STL_TRY( dtlTimeZoneToGMTOffset( & sTimeZoneInterval,
                                     & sGMTOffset,
                                     QLL_ERROR_STACK(sEnv) )
             == STL_SUCCESS );

    STL_TRY( qllSetSessTimeZoneOffset( sGMTOffset, sEnv ) == STL_SUCCESS );

    /*
     * 검증의 용이함을 위해 INSERTABLE_PAGE_HINT를 사용하지 않는다.
     * - 성능은 떨어지나 출력이 입력 순서대로 보이게 됨.
     */

    STL_TRY( smlCreateDatabase( KNL_STARTUP_PHASE_NO_MOUNT,
                                SML_ENV( sEnv ) )
             == STL_SUCCESS );

    STL_TRY( ztbStartup( KNL_STARTUP_PHASE_MOUNT,
                         KNL_STARTUP_PHASE_MOUNT,
                         ELL_ENV(sEnv) ) == STL_SUCCESS );

    STL_TRY( smlCreateDatabase( KNL_STARTUP_PHASE_MOUNT,
                                SML_ENV( sEnv ) )
             == STL_SUCCESS );

    STL_TRY( ztbStartup( KNL_STARTUP_PHASE_PREOPEN,
                         KNL_STARTUP_PHASE_OPEN,
                         ELL_ENV(sEnv) ) == STL_SUCCESS );
    
    STL_TRY( smlCreateDatabase( KNL_STARTUP_PHASE_OPEN,
                                SML_ENV( sEnv ) )
             == STL_SUCCESS );

    STL_TRY( ztbBuildDictionary( gZtbDatabaseName,
                                 gZtbDatabaseComment,
                                 gZtbTimezone,
                                 gZtbCharacterSet,
                                 sCharLengthUnit,
                                 ELL_ENV(sEnv) ) == STL_SUCCESS );

    /**
     * INFORMATION_SCHEMA 는 admin/InformationSchema.sql 의 수행으로 변경함.
     */

    /**
     * Create Database 의 마무리 작업을 수행한다.
     */
    
    STL_TRY( smlCompleteCreateDatabase( SML_ENV( sEnv ) ) == STL_SUCCESS );

    sState = 6;
    STL_TRY( qllFiniSessionEnv( sSessionEnv,
                                sEnv )
             == STL_SUCCESS );

    sState = 5;
    STL_TRY( knlFreeSessionEnv( (void*)sSessionEnv,
                                KNL_ENV( sEnv ) )
             == STL_SUCCESS );
    sSessionEnv = NULL;
    
    sState = 4;
    STL_TRY( qllFinalizeEnv( sEnv ) == STL_SUCCESS );
    
    sState = 3;
    STL_TRY( knlFreeEnv( sEnv,
                         KNL_ERROR_STACK( &sSystemEnv ) ) == STL_SUCCESS );
    sEnv = NULL;

    STL_TRY( smlCheckpoint4Shutdown( SML_ENV(&sSystemEnv) ) == STL_SUCCESS );
    
    sState = 2;
    STL_TRY( ztbBootdown( ELL_ENV(&sSystemEnv) ) == STL_SUCCESS );

    if( gZtbSilentMode == STL_FALSE )
    {
        stlPrintf("Database created\n\n");
    }

    STL_RAMP( RAMP_FINISH );
    
    sState = 1;
    STL_TRY( qllFiniSessionEnv( &sSystemSessionEnv,
                                &sSystemEnv ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( qllFinalizeEnv( &sSystemEnv ) == STL_SUCCESS );

    return STL_SUCCESS;
    
    STL_FINISH;

    switch( sState )
    {
        case 7 :
            (void)qllFiniSessionEnv( sSessionEnv, sEnv );
        case 6 :
            (void)knlFreeSessionEnv( (void*)sSessionEnv, KNL_ENV( sEnv ) );
        case 5 :
            ztbPrintErrorStack( KNL_ERROR_STACK( sEnv ) );
            (void)qllFinalizeEnv( sEnv );
        case 4 :
            (void)knlFreeEnv( sEnv, KNL_ERROR_STACK( &sSystemEnv ) );
        case 3 :
            (void)ztbBootdown( ELL_ENV(&sSystemEnv) );
        case 2 :
            (void)qllFiniSessionEnv( &sSystemSessionEnv, &sSystemEnv );
        case 1 :
            ztbPrintErrorStack( KNL_ERROR_STACK( &sSystemEnv ) );
            (void)qllFinalizeEnv( &sSystemEnv );
        default :
            break;
    }

    return STL_FAILURE;
}
