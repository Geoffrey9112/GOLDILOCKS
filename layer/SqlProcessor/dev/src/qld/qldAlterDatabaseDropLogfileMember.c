/*******************************************************************************
 * qldAlterDatabaseDropLogfileMember.c
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
 * @file qldAlterDatabaseDropLogfileMember.c
 * @brief ALTER DATABASE DROP LOGFILE MEMBER 처리 루틴 
 */

#include <qll.h>

#include <qlDef.h>


/**
 * @defgroup qldAlterDatabaseDropLogfileMember ALTER DATABASE DROP LOGFILE MEMBER
 * @ingroup qldAlterDatabase
 * @{
 */


/**
 * @brief ALTER DATABASE DROP LOGFILE MEMBER 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qldValidateAlterDatabaseDropLogfileMember( smlTransId      aTransID,
                                                     qllDBCStmt    * aDBCStmt,
                                                     qllStatement  * aSQLStmt,
                                                     stlChar       * aSQLString,
                                                     qllNode       * aParseTree,
                                                     qllEnv        * aEnv )
{
    qlpAlterDatabaseDropLogfileMember * sParseTree      = NULL;
    qldInitDropLogfileMember          * sInitPlan       = NULL;
    smlLogStreamAttr                  * sLogStreamAttr  = NULL;
    smlLogGroupAttr                   * sLogGroupAttr   = NULL;
    smlLogGroupAttr                   * sTmpGroupAttr   = NULL;
    smlLogGroupAttr                   * sPrevGroupAttr  = NULL;
    smlLogMemberAttr                  * sLogMemberAttr  = NULL;
    smlLogMemberAttr                  * sTmpMemberAttr  = NULL;
    qlpListElement                    * sListElement    = NULL;
    qlpValue                          * sLogfile;
    stlBool                             sGroupExist = STL_FALSE;
    stlInt16                            sGroupId;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_DATABASE_DROP_LOGFILE_MEMBER_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_ALTER_DATABASE_DROP_LOGFILE_MEMBER_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득 
     */

    sParseTree = (qlpAlterDatabaseDropLogfileMember *) aParseTree;

    /**
     * Init Plan 생성 
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qldInitDropLogfileMember),
                                (void**) &sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qldInitDropLogfileMember) );

    /**
     * 유효성 검사
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(smlLogStreamAttr),
                                (void **) & sLogStreamAttr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sLogStreamAttr, 0x00, STL_SIZEOF(smlLogStreamAttr) );
    
    /* File List */

    STL_ASSERT( sParseTree->mLogFileList != NULL );

    sInitPlan->mMemberCount = QLP_LIST_GET_COUNT( sParseTree->mLogFileList );
    
    sPrevGroupAttr = NULL;
    QLP_LIST_FOR_EACH( sParseTree->mLogFileList, sListElement )
    {
        sLogfile = (qlpValue*) QLP_LIST_GET_POINTER_VALUE( sListElement );

        /**
         * Log Member Attr 할당
         */

        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                    STL_SIZEOF(smlLogMemberAttr),
                                    (void **) &sLogMemberAttr,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        stlMemset( sLogMemberAttr, 0x00, STL_SIZEOF(smlLogMemberAttr) );

        stlStrcpy( sLogMemberAttr->mName, QLP_GET_PTR_VALUE(sLogfile) );

        sLogMemberAttr->mNext = NULL;
        
        /* 절대 경로 */
        STL_TRY( smlConvAbsLogfilePath( sLogMemberAttr->mName,
                                        SML_ENV(aEnv) )
                 == STL_SUCCESS );
         
        /* Group 확인 */
        STL_TRY( smlIsUsedLogfile( sLogMemberAttr->mName,
                                   &sGroupExist,
                                   &sGroupId,
                                   SML_ENV(aEnv) )
                 == STL_SUCCESS );

        STL_TRY_THROW( (sGroupExist == STL_TRUE) && (sGroupId != SML_INVALID_LOG_GROUP_ID),
                       RAMP_ERR_LOGFILE_NOT_EXIST_IN_ANY_GROUP );
        
        /* smlLogStreamAttr에서 같은 GroupId 갖는 Group Attr 찾기. */
        sTmpGroupAttr = sLogStreamAttr->mLogGroupAttr;
        while( sTmpGroupAttr != NULL )
        {
            if( ((stlInt16)(sTmpGroupAttr->mLogGroupId)) == sGroupId )
            {
                break;
            }

            sTmpGroupAttr = sTmpGroupAttr->mNext;
        }

        /* Group Attr에 Member Attr 연결 */
        
        if( sTmpGroupAttr == NULL )
        {
            STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                        STL_SIZEOF(smlLogGroupAttr),
                                        (void **) &sLogGroupAttr,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );
            stlMemset( sLogGroupAttr, 0x00, STL_SIZEOF(smlLogGroupAttr) );
            
            sLogGroupAttr->mLogMemberAttr = sLogMemberAttr;
            sLogGroupAttr->mLogGroupId = sGroupId;
            sLogGroupAttr->mNext = NULL;
            
            /* Stream에 연결 */
            if( sPrevGroupAttr == NULL )
            {
                sLogStreamAttr->mLogGroupAttr = sLogGroupAttr;
            }
            else
            {
                sPrevGroupAttr->mNext = sLogGroupAttr;
            }
            sPrevGroupAttr = sLogGroupAttr;

            sInitPlan->mGroupCount++;
        }
        else
        {
            /* Group Attr에 중복 파일 검사.*/
            sTmpMemberAttr = sTmpGroupAttr->mLogMemberAttr;

            while( sTmpMemberAttr != NULL )
            {
                if( stlStrcmp( sTmpMemberAttr->mName, sLogMemberAttr->mName ) == 0 )
                {
                    STL_THROW( RAMP_ERR_DUPLICATE_LOGFILE_NAME );
                }

                if( sTmpMemberAttr->mNext == NULL )
                {
                    break;
                }

                sTmpMemberAttr = sTmpMemberAttr->mNext;
            }

            sTmpMemberAttr->mNext = sLogMemberAttr;
        }
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
    sInitPlan->mStreamAttr = sLogStreamAttr;
    aSQLStmt->mInitPlan = (void *) sInitPlan;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_DUPLICATE_LOGFILE_NAME )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_DUPLICATE_LOGFILE_NAME,
                      qlgMakeErrPosString( aSQLString,
                                           sLogfile->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv ) );
    }

    STL_CATCH( RAMP_ERR_LOGFILE_NOT_EXIST_IN_ANY_GROUP )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_LOG_MEMBER_NOT_FOUND,
                      qlgMakeErrPosString( aSQLString,
                                           sLogfile->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv ) );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief ALTER DATABASE DROP LOGFILE MEMBER 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qldOptCodeAlterDatabaseDropLogfileMember( smlTransId      aTransID,
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
 * @brief ALTER DATABASE DROP LOGFILE MEMBER 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qldOptDataAlterDatabaseDropLogfileMember( smlTransId      aTransID,
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
 * @brief ALTER DATABASE DROP LOGFILE MEMBER 을 수행한다
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aDBCStmt  DBC Statement
 * @param[in] aSQLStmt  SQL Statement
 * @param[in] aEnv      Environment
 */
stlStatus qldExecuteAlterDatabaseDropLogfileMember( smlTransId      aTransID,
                                                    smlStatement  * aStmt,
                                                    qllDBCStmt    * aDBCStmt,
                                                    qllStatement  * aSQLStmt,
                                                    qllEnv        * aEnv )
{
    qldInitDropLogfileMember * sInitPlan;
    smlLogGroupAttr          * sGroupAttr;
    smlLogMemberAttr         * sMemberAttr;
    stlInt32                   sGroupCount;
    stlInt32                   sTotalMemberCount;
    stlInt32                   sMemberCount;
    stlInt32                   sValidationCount = 0;
    stlInt32                   sDataSize;
    stlInt32                   i;
    stlInt32                   j;
    stlChar                  * sData = NULL;
    stlChar                  * sPtr;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_DATABASE_DROP_LOGFILE_MEMBER_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    sInitPlan = (qldInitDropLogfileMember *)aSQLStmt->mInitPlan;

    sGroupCount = sInitPlan->mGroupCount;
    sTotalMemberCount = sInitPlan->mMemberCount;

    /**
     * Marshalling
     *   LogStreamAttr,
     *   LogGroupAttr Count,
     *     LogGroupAttr,
     *     LogMemberAttr Count,
     *       LogMemberAttr [LogMemberAttr]
     *     [LogGroupAttr]...  
     */
    
    sDataSize = STL_ALIGN( STL_SIZEOF(smlLogStreamAttr), 8 ) +
        STL_ALIGN( STL_SIZEOF(stlInt32), 8 ) + /* Group Count */
        (sGroupCount * STL_ALIGN( STL_SIZEOF(smlLogGroupAttr), 8 )) +
        (sGroupCount * STL_ALIGN( STL_SIZEOF(stlInt32), 8 )) + /* Member Count per Group */
        (sTotalMemberCount * STL_ALIGN( STL_SIZEOF(smlLogMemberAttr), 8 ));
    
    STL_TRY( knlAllocRegionMem( &QLL_EXEC_DDL_MEM(aEnv),
                                sDataSize,
                                (void **) &sData,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sData, 0x00, sDataSize );
    sPtr = sData;
    
    stlMemcpy( sPtr, sInitPlan->mStreamAttr, STL_SIZEOF(smlLogStreamAttr) );
    sPtr += STL_ALIGN( STL_SIZEOF(smlLogStreamAttr), 8 );

    stlMemcpy( sPtr, &sGroupCount, STL_SIZEOF(stlInt32) );
    sPtr += STL_ALIGN( STL_SIZEOF(stlInt32), 8 );

    sGroupAttr = sInitPlan->mStreamAttr->mLogGroupAttr;
    for( i = 0; i < sGroupCount; i++ )
    {
        stlMemcpy( sPtr, sGroupAttr, STL_SIZEOF(smlLogGroupAttr) );
        sPtr += STL_ALIGN( STL_SIZEOF(smlLogGroupAttr), 8 );

        sMemberAttr = sGroupAttr->mLogMemberAttr;

        sMemberCount = 0;
        while( sMemberAttr != NULL )
        {
            sMemberCount++;
            sMemberAttr = sMemberAttr->mNext;
        }
        sValidationCount += sMemberCount;
        
        stlMemcpy( sPtr, &sMemberCount, STL_SIZEOF(stlInt32) );
        sPtr += STL_ALIGN( STL_SIZEOF(stlInt32), 8 );
        
        sMemberAttr = sGroupAttr->mLogMemberAttr;
        for( j = 0; j < sMemberCount; j++ )
        {
            stlMemcpy( sPtr, sMemberAttr, STL_SIZEOF(smlLogMemberAttr) );
            sPtr += STL_ALIGN( STL_SIZEOF(smlLogMemberAttr), 8 );
            
            sMemberAttr = sMemberAttr->mNext;
        }
        
        sGroupAttr = sGroupAttr->mNext;
    }

    STL_ASSERT( sTotalMemberCount == sValidationCount );
    STL_ASSERT( sPtr - sData == sDataSize );

    STL_TRY( knlAddEnvEvent( SML_EVENT_DROP_LOGFILE_MEMBER,
                             NULL,            /* aEventMem */
                             sData,           /* aData */
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

/** @} qldAlterDatabaseDropLogfileMember */
