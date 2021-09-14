/*******************************************************************************
 * qldAlterDatabaseAddLogfileMember.c
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
 * @file qldAlterDatabaseAddLogfileMember.c
 * @brief ALTER DATABASE ADD LOGFILE MEMBER 처리 루틴 
 */

#include <sml.h>
#include <qll.h>

#include <qlDef.h>


/**
 * @defgroup qldAlterDatabaseAddLogfileMember ALTER DATABASE ADD LOGFILE MEMBER
 * @ingroup qldAlterDatabase
 * @{
 */


/**
 * @brief ALTER DATABASE ADD LOGFILE MEMBER 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qldValidateAlterDatabaseAddLogfileMember( smlTransId      aTransID,
                                                    qllDBCStmt    * aDBCStmt,
                                                    qllStatement  * aSQLStmt,
                                                    stlChar       * aSQLString,
                                                    qllNode       * aParseTree,
                                                    qllEnv        * aEnv )
{
    qlpAlterDatabaseAddLogfileMember * sParseTree = NULL;
    qldInitAddLogfileMember          * sInitPlan  = NULL;
    smlLogGroupAttr                  * sLogGroupAttr   = NULL;
    smlLogMemberAttr                 * sLogMemberAttr  = NULL;
    smlLogMemberAttr                 * sPrevMemberAttr = NULL;
    smlLogMemberAttr                 * sTmpMemberAttr  = NULL;
    qlpListElement                   * sListElement    = NULL;
    qlpDatafile                      * sLogfile        = NULL;
    stlChar                          * sEndPtr;
    stlInt64                           sInt64;
    stlBool                            sGroupExist     = STL_FALSE;
    stlBool                            sFileExist      = STL_FALSE;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_DATABASE_ADD_LOGFILE_MEMBER_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_ALTER_DATABASE_ADD_LOGFILE_MEMBER_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득 
     */

    sParseTree = (qlpAlterDatabaseAddLogfileMember *) aParseTree;

    /**
     * Init Plan 생성 
     */
    
    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qldInitAddLogfileMember),
                                (void **) &sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qldInitAddLogfileMember) );
    
    /**
     * 유효성 검사
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(smlLogGroupAttr),
                                (void **) &sLogGroupAttr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sLogGroupAttr, 0x00, STL_SIZEOF(smlLogGroupAttr) );

    sInitPlan->mLogGroupAttr = sLogGroupAttr;
    
    /**
     * @todo : to GROUP ID 이외에 filename으로도 GROUP ID를 구할 수 있게해야 한다.
     */
    /* Group Id */
    STL_ASSERT( sParseTree->mLogGroupId != NULL );
    STL_TRY( stlStrToInt64( QLP_GET_PTR_VALUE( sParseTree->mLogGroupId ),
                            STL_NTS,
                            &sEndPtr,
                            10,
                            &sInt64,
                            KNL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sInt64 <= STL_INT32_MAX, RAMP_ERR_INVALID_LOGGROUP_NUMBER );

    STL_TRY( smlCheckLogGroupId( (stlInt32)sInt64,
                                 &sGroupExist,
                                 SML_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sGroupExist == STL_TRUE, RAMP_ERR_NOT_EXIST_LOGGROUP_ID );

    sLogGroupAttr->mLogGroupId = (stlInt32) sInt64;
    sLogGroupAttr->mValidFlags |= SML_LOG_GROUP_GROUPID_YES;
    
    /* File List */

    STL_ASSERT( sParseTree->mMemberList != NULL );

    sInitPlan->mMemberCount = QLP_LIST_GET_COUNT( sParseTree->mMemberList );

    sPrevMemberAttr = NULL;
    QLP_LIST_FOR_EACH( sParseTree->mMemberList, sListElement )
    {
        sLogfile = (qlpDatafile*) QLP_LIST_GET_POINTER_VALUE( sListElement );
        
        /**
         * Log Member Attr 할당
         */

        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                    STL_SIZEOF(smlLogMemberAttr),
                                    (void **) &sLogMemberAttr,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        stlMemset( sLogMemberAttr, 0x00, STL_SIZEOF(smlLogMemberAttr) );

        stlStrcpy( sLogMemberAttr->mName, QLP_GET_PTR_VALUE(sLogfile->mFilePath) );

        /* 절대 경로로 변경 */
        STL_TRY( smlConvAbsLogfilePath( sLogMemberAttr->mName,
                                        SML_ENV(aEnv) )
                 == STL_SUCCESS );

        /* Reuse */
        if( sLogfile->mReuse == NULL )
        {
            sLogMemberAttr->mValidFlags |= SML_LOG_MEMBER_REUSE_NO;
        }
        else
        {
            sLogMemberAttr->mValidFlags |= SML_LOG_MEMBER_REUSE_YES;
            sLogMemberAttr->mLogFileReuse = QLP_GET_INT_VALUE( sLogfile->mReuse );
        }
        
        /**
         * REUSE OPTION이 없다면 물리적인 파일이 존재하는지 검사한다.
         */
        if( sLogMemberAttr->mLogFileReuse == STL_FALSE )
        {
            STL_TRY( smlExistLogfile( sLogMemberAttr->mName,
                                      &sFileExist,
                                      SML_ENV(aEnv) )
                     == STL_SUCCESS );

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

    STL_CATCH( RAMP_ERR_NOT_EXIST_LOGGROUP_ID )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_EXIST_LOGGROUP_ID,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mLogGroupId->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_EXIST_FILE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_ALREADY_EXIST_FILE,
                      qlgMakeErrPosString( aSQLString,
                                           sLogfile->mFilePath->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sLogMemberAttr->mName );
    }

    STL_CATCH( RAMP_ERR_USED_LOGFILE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_ALREADY_USED_LOGFILE,
                      qlgMakeErrPosString( aSQLString,
                                           sLogfile->mFilePath->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sLogMemberAttr->mName );
    }
    
    STL_CATCH( RAMP_ERR_DUPLICATE_LOGFILE_NAME )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_DUPLICATE_LOGFILE_NAME,
                      qlgMakeErrPosString( aSQLString,
                                           sLogfile->mFilePath->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief ALTER DATABASE ADD LOG MEMEBER 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qldOptCodeAlterDatabaseAddLogfileMember( smlTransId      aTransID,
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
 * @brief ALTER DATABASE ADD LOGFILE MEMBER 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qldOptDataAlterDatabaseAddLogfileMember( smlTransId      aTransID,
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
 * @brief ALTER DATABASE ADD LOGFILE MEMBER 을 수행한다
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aDBCStmt  DBC Statement
 * @param[in] aSQLStmt  SQL Statement
 * @param[in] aEnv      Environment
 */
stlStatus qldExecuteAlterDatabaseAddLogfileMember( smlTransId      aTransID,
                                                   smlStatement  * aStmt,
                                                   qllDBCStmt    * aDBCStmt,
                                                   qllStatement  * aSQLStmt,
                                                   qllEnv        * aEnv )
{
    qldInitAddLogfileMember * sInitPlan;
    smlLogMemberAttr        * sMemberAttr;
    stlInt32                  sMemberCount;
    stlInt32                  sDataSize;
    stlInt32                  i;
    stlChar                 * sBuf = NULL;
    stlChar                 * sPtr;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_DATABASE_ADD_LOGFILE_MEMBER_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    sInitPlan = (qldInitAddLogfileMember *)aSQLStmt->mInitPlan;

    sMemberCount = sInitPlan->mMemberCount;

    /**
     * Marshalling
     *   smlLogGroupAttr
     *   MemberCount
     *     smlLogMemberAttr [smlLogMemberAttr]...
     */ 
    sDataSize = STL_ALIGN( STL_SIZEOF(smlLogGroupAttr), 8 ) +
        STL_ALIGN( STL_SIZEOF(stlInt32), 8 ) +
        (sMemberCount * STL_ALIGN( STL_SIZEOF(smlLogMemberAttr), 8 ));

    STL_TRY( knlAllocRegionMem( &QLL_EXEC_DDL_MEM(aEnv),
                                sDataSize,
                                (void **) &sBuf,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sBuf, 0x00, sDataSize );
    sPtr = sBuf;

    stlMemcpy( sPtr, sInitPlan->mLogGroupAttr, STL_SIZEOF(smlLogGroupAttr) );
    sPtr += STL_ALIGN( STL_SIZEOF(smlLogGroupAttr), 8 );

    stlMemcpy( sPtr, &sMemberCount, STL_SIZEOF(stlInt32) );
    sPtr += STL_ALIGN( STL_SIZEOF(stlInt32), 8 );

    sMemberAttr = sInitPlan->mLogGroupAttr->mLogMemberAttr;
    for( i = 0; i < sMemberCount; i++ )
    {
        stlMemcpy( sPtr, sMemberAttr, STL_SIZEOF(smlLogMemberAttr) );
        sPtr += STL_ALIGN( STL_SIZEOF(smlLogMemberAttr), 8 );

        sMemberAttr = sMemberAttr->mNext;
    }

    STL_ASSERT( sPtr - sBuf == sDataSize );

    STL_TRY( knlAddEnvEvent( SML_EVENT_ADD_LOGFILE_MEMBER,
                             NULL,            /* aEventMem */
                             sBuf,            /* aData */
                             sDataSize,       /* aDataSize */
                             0, /* startup env id */
                             KNL_EVENT_WAIT_POLLING,
                             STL_FALSE, /* aForceSuccess */
                             KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

/** @} qldAlterDatabaseAddLogfileMember */
