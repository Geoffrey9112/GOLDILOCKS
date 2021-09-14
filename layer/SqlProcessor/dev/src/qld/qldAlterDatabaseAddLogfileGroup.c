/*******************************************************************************
 * qldAlterDatabaseAddLogfileGroup.c
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

/**
 * @file qldAlterDatabaseAddLogfileGroup.c
 * @brief ALTER DATABASE ADD LOGFILE GROUP 처리 루틴 
 */

#include <sml.h>
#include <qll.h>

#include <qlDef.h>


/**
 * @defgroup qldAlterDatabaseAddLogfileGroup ALTER DATABASE ADD LOGFILE GROUP
 * @ingroup qldAlterDatabase
 * @{
 */


/**
 * @brief ALTER DATABASE ADD LOGFILE GROUP 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qldValidateAlterDatabaseAddLogfileGroup( smlTransId      aTransID,
                                                   qllDBCStmt    * aDBCStmt,
                                                   qllStatement  * aSQLStmt,
                                                   stlChar       * aSQLString,
                                                   qllNode       * aParseTree,
                                                   qllEnv        * aEnv )
{
    qlpAlterDatabaseAddLogfileGroup * sParseTree;
    qldInitAddLogFileGroup  * sInitPlan = NULL;
    smlLogStreamAttr        * sLogStreamAttr = NULL;
    smlLogGroupAttr         * sLogGroupAttr = NULL;
    smlLogMemberAttr        * sLogMemberAttr = NULL;
    smlLogMemberAttr        * sPrevMemberAttr = NULL;
    smlLogMemberAttr        * sTmpMemberAttr = NULL;
    stlChar                 * sEndPtr;
    stlInt64                  sValue64;
    stlInt64                  sByteUnit;
    qlpListElement *          sListElement = NULL;
    qlpValue                * sLogFile;
    stlBool                   sFileExist;
    stlBool                   sFound = STL_FALSE;
    stlInt64                  sMinLogFileSize;

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_DATABASE_ADD_LOGFILE_GROUP_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_ALTER_DATABASE_ADD_LOGFILE_GROUP_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득 
     */

    sParseTree = (qlpAlterDatabaseAddLogfileGroup *) aParseTree;
    
    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qldInitAddLogFileGroup),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qldInitAddLogFileGroup) );

    /**
     * 유효성 검사
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(smlLogStreamAttr),
                                (void **) & sLogStreamAttr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sLogStreamAttr, 0x00, STL_SIZEOF(smlLogStreamAttr) );

    sInitPlan->mLogStreamAttr = sLogStreamAttr;


    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(smlLogGroupAttr),
                                (void **) & sLogGroupAttr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sLogGroupAttr, 0x00, STL_SIZEOF(smlLogGroupAttr) );

    sLogStreamAttr->mLogGroupAttr = sLogGroupAttr;

    /* GROUP ID 절 */
    STL_ASSERT( sParseTree->mLogGroupId != NULL );
    STL_TRY( stlStrToInt64( QLP_GET_PTR_VALUE( sParseTree->mLogGroupId ),
                            STL_NTS,
                            &sEndPtr,
                            10,
                            &sValue64,
                            KNL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sValue64 <= STL_INT32_MAX, RAMP_ERR_INVALID_LOGGROUP_NUMBER );

    /* check log group identifier */

    STL_TRY( smlCheckLogGroupId( (stlInt32)sValue64,
                                 &sFound,
                                 SML_ENV(aEnv) )
             == STL_SUCCESS );
                                 
    STL_TRY_THROW( sFound == STL_FALSE, RAMP_ERR_USED_LOGGROUP_ID );

    sLogGroupAttr->mLogGroupId = (stlInt32)sValue64;
    sLogGroupAttr->mValidFlags |= SML_LOG_GROUP_GROUPID_YES;

    /* SIZE 절 */
    STL_ASSERT( sParseTree->mLogfileSize != NULL );
    sByteUnit = QLP_GET_BYTES_FROM_SIZE_UNIT( QLP_GET_INT_VALUE( sParseTree->mLogfileSize->mSizeUnit ) );
    STL_TRY( stlStrToInt64( QLP_GET_PTR_VALUE(sParseTree->mLogfileSize->mSize),
                            STL_NTS,
                            &sEndPtr,
                            10,
                            &sValue64,
                            KNL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );
    sValue64 *= sByteUnit;

    sMinLogFileSize = smlGetMinLogFileSize( SML_ENV(aEnv) );
    STL_TRY_THROW( sValue64 >= sMinLogFileSize, RAMP_ERR_TOO_SMALL_LOG_FILE_SIZE );

    sLogGroupAttr->mFileSize = sValue64;
    sLogGroupAttr->mValidFlags |= SML_LOG_GROUP_FILESIZE_YES;

    /* REUSE 절 */
    if( sParseTree->mLogfileReuse != NULL )
    {
        sLogGroupAttr->mValidFlags |= SML_LOG_GROUP_REUSE_YES;
        sLogGroupAttr->mLogFileReuse = (stlBool)QLP_GET_INT_VALUE( sParseTree->mLogfileReuse );
    }

    /**
     * @todo : 아직 하나 이상의 log member를 지원하지 않음
     */
    STL_ASSERT( sParseTree->mLogfileList != NULL );
    STL_TRY_THROW( sParseTree->mLogfileList->mCount == 1, RAMP_ERR_NOT_IMPLEMENTED );

    sPrevMemberAttr = NULL;
    QLP_LIST_FOR_EACH( sParseTree->mLogfileList, sListElement )
    {
        sLogFile = (qlpValue*) QLP_LIST_GET_POINTER_VALUE( sListElement );

        /**
         * Log Member Attr 공간 할당
         */

        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                    STL_SIZEOF(smlLogMemberAttr),
                                    (void **) & sLogMemberAttr,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        stlMemset( sLogMemberAttr, 0x00, STL_SIZEOF(smlLogMemberAttr) );

        stlStrcpy( sLogMemberAttr->mName, QLP_GET_PTR_VALUE(sLogFile) );
        /* 절대 경로로 변경 */
        STL_TRY( smlConvAbsLogfilePath( sLogMemberAttr->mName,
                                        SML_ENV(aEnv) )
                 == STL_SUCCESS );
        /**
         * REUSE OPTION이 없다면 물리적인 파일이 존재하는지 검사한다.
         */
        if( sLogGroupAttr->mLogFileReuse == STL_FALSE )
        {
            STL_TRY( smlExistLogfile( sLogMemberAttr->mName,
                                       &sFileExist,
                                       SML_ENV(aEnv) ) == STL_SUCCESS );

            STL_TRY_THROW( sFileExist == STL_FALSE, RAMP_ERR_EXIST_FILE );
        }

        /**
         * 해당 경로가 Database에서 이미 사용중인지 검사한다.
         * - 파일이 물리적으로 존재하지 않더라도 로그파일 경로는
         *   사용중에 있을수 있다.
         */
        STL_TRY( smlIsUsedLogfile( sLogMemberAttr->mName,
                                   &sFileExist,
                                   NULL, /* aGroupId */
                                   SML_ENV(aEnv) ) == STL_SUCCESS );

        STL_TRY_THROW( sFileExist == STL_FALSE, RAMP_ERR_USED_LOGFILE );

        /**
         * 추가할 파일들의 중복 파일 검사
         */

        sTmpMemberAttr = sLogGroupAttr->mLogMemberAttr;
        
        while( sTmpMemberAttr != NULL )
        {
            if( stlStrcmp( sLogMemberAttr->mName, sTmpMemberAttr->mName ) == 0 )
            {
                STL_THROW( RAMP_ERR_DUPLICATE_LOGFILE_NAME );
            }

            sTmpMemberAttr = sTmpMemberAttr->mNext;
        }
 
        sLogMemberAttr->mValidFlags |= SML_LOG_MEMBER_NAME_YES;
        sLogMemberAttr->mNext = NULL;

        if( sPrevMemberAttr == NULL )
        {
            sLogGroupAttr->mLogMemberAttr = sLogMemberAttr;
        }
        else
        {
            sPrevMemberAttr->mNext = sLogMemberAttr;
        }
        sPrevMemberAttr = sLogMemberAttr;
    }


    /**
     * ALTER DATABASE ON DATABASE 권한 검사
     */

    STL_TRY( qlaCheckDatabasePriv( aTransID,
                                   aDBCStmt,
                                   aSQLStmt,
                                   ELL_DB_PRIV_ACTION_ALTER_DATABASE,
                                   aEnv )
             == STL_SUCCESS );
        
    /**
     * Init Plan 연결 
     */

    aSQLStmt->mInitPlan = (void *) sInitPlan;
 
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_LOGGROUP_NUMBER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_INVALID_LOGGROUP_NUMBER,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mLogGroupId->mNodePos,
                                           aEnv ),
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_CATCH( RAMP_ERR_USED_LOGGROUP_ID )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_ALREADY_USED_LOGGROUP_ID,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mLogGroupId->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_TOO_SMALL_LOG_FILE_SIZE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_TOO_SMALL_LOG_FILE_SIZE,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      sMinLogFileSize );
    }


    STL_CATCH( RAMP_ERR_EXIST_FILE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_ALREADY_EXIST_FILE,
                      qlgMakeErrPosString( aSQLString,
                                           sLogFile->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sLogMemberAttr->mName );
    }

    STL_CATCH( RAMP_ERR_USED_LOGFILE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_ALREADY_USED_LOGFILE,
                      qlgMakeErrPosString( aSQLString,
                                           sLogFile->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sLogMemberAttr->mName );
    }

    STL_CATCH( RAMP_ERR_DUPLICATE_LOGFILE_NAME )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_DUPLICATE_LOGFILE_NAME,
                      qlgMakeErrPosString( aSQLString,
                                           sLogFile->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }
    STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_IMPLEMENTED,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      "qldValidateAlterDatabaseAddLogfileGroup() [two or more log members]" );
    }

    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief ALTER DATABASE ADD LOGFILE GROUP 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qldOptCodeAlterDatabaseAddLogfileGroup( smlTransId      aTransID,
                                                  qllDBCStmt    * aDBCStmt,
                                                  qllStatement  * aSQLStmt,
                                                  qllEnv        * aEnv )
{
    /**
     * nothing to do
     */
    
    return STL_SUCCESS;
}



/**
 * @brief ALTER DATABASE ADD LOGFILE GROUP 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qldOptDataAlterDatabaseAddLogfileGroup( smlTransId      aTransID,
                                                  qllDBCStmt    * aDBCStmt,
                                                  qllStatement  * aSQLStmt,
                                                  qllEnv        * aEnv )
{
    /**
     * nothing to do
     */
    
    return STL_SUCCESS;
}






/**
 * @brief ALTER DATABASE ADD LOGFILE GROUP 을 수행한다
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aDBCStmt  DBC Statement
 * @param[in] aSQLStmt  SQL Statement
 * @param[in] aEnv      Environment
 */
stlStatus qldExecuteAlterDatabaseAddLogfileGroup( smlTransId      aTransID,
                                                  smlStatement  * aStmt,
                                                  qllDBCStmt    * aDBCStmt,
                                                  qllStatement  * aSQLStmt,
                                                  qllEnv        * aEnv )
{
    stlChar               * sBuf = NULL;
    stlChar               * sPtr;
    stlInt32                sMemberCount;
    stlInt32                sArgSize;
    stlInt32                i;
    smlLogMemberAttr       * sMemberAttr;
    qldInitAddLogFileGroup * sInitPlan;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_DATABASE_ADD_LOGFILE_GROUP_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    sInitPlan = (qldInitAddLogFileGroup*)aSQLStmt->mInitPlan;
    /**
     * @todo : Add Logfile Member 기능이 되면 Member 개수를 구해야 한다
     */
    sMemberCount = 1;

    sArgSize = STL_ALIGN( STL_SIZEOF(smlLogStreamAttr), 8 )
        + STL_ALIGN( STL_SIZEOF(smlLogGroupAttr), 8 )
        + STL_ALIGN( STL_SIZEOF(stlInt32), 8 )
        + (sMemberCount * STL_ALIGN( STL_SIZEOF(smlLogMemberAttr), 8 ));

    STL_TRY( knlAllocRegionMem( & QLL_EXEC_DDL_MEM(aEnv),
                                sArgSize,
                                (void **) & sBuf,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sBuf, 0x00, sArgSize );
    sPtr = sBuf;

    stlMemcpy( sPtr, &sInitPlan->mLogStreamAttr, STL_SIZEOF(smlLogStreamAttr) );
    sPtr += STL_ALIGN( STL_SIZEOF(smlLogStreamAttr), 8 );
    stlMemcpy( sPtr, sInitPlan->mLogStreamAttr->mLogGroupAttr, STL_SIZEOF(smlLogGroupAttr) );
    sPtr += STL_ALIGN( STL_SIZEOF(smlLogGroupAttr), 8 );
    stlMemcpy( sPtr, &sMemberCount, STL_SIZEOF(stlInt32) );
    sPtr += STL_ALIGN( STL_SIZEOF(stlInt32), 8 );
    sMemberAttr = sInitPlan->mLogStreamAttr->mLogGroupAttr->mLogMemberAttr;
    for( i = 0; i < sMemberCount; i++ )
    {
        stlMemcpy( sPtr,
                   sInitPlan->mLogStreamAttr->mLogGroupAttr->mLogMemberAttr,
                   STL_SIZEOF(smlLogMemberAttr) );
        sPtr += STL_ALIGN( STL_SIZEOF(smlLogMemberAttr), 8 );
        sMemberAttr = sMemberAttr->mNext;
    }

    STL_ASSERT( sPtr - sBuf == sArgSize );

    STL_TRY( knlAddEnvEvent( SML_EVENT_ADD_LOGFILE_GROUP,
                             NULL,            /* aEventMem */
                             sBuf,            /* aData */
                             sArgSize,        /* aDataSize */
                             0, /* startup env id */
                             KNL_EVENT_WAIT_POLLING,
                             STL_FALSE, /* aForceSuccess */
                             KNL_ENV( aEnv ) ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

/** @} qldAlterDatabaseAddLogfileGroup */
