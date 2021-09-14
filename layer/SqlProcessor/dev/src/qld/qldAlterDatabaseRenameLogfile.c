/*******************************************************************************
 * qldAlterDatabaseRenameLogfile.c
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
 * @file qldAlterDatabaseRenameLogfile.c
 * @brief ALTER DATABASE RENAME LOG FILE 처리 루틴 
 */

#include <qll.h>

#include <qlDef.h>


/**
 * @defgroup qldAlterDatabaseRenameLogfile ALTER DATABASE RENAME LOG FILE
 * @ingroup qldAlterDatabase
 * @{
 */


/**
 * @brief ALTER DATABASE RENAME LOG FILE 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qldValidateAlterDatabaseRenameLogfile( smlTransId      aTransID,
                                                 qllDBCStmt    * aDBCStmt,
                                                 qllStatement  * aSQLStmt,
                                                 stlChar       * aSQLString,
                                                 qllNode       * aParseTree,
                                                 qllEnv        * aEnv )
{
    qlpAlterDatabaseRenameLogfile * sParseTree;
    qldInitRenameLogFile          * sInitPlan = NULL;
    smlLogMemberAttr              * sLogMemberAttr = NULL;
    smlLogMemberAttr              * sPrevMemberAttr = NULL;
    smlLogMemberAttr              * sTmpMemberAttr = NULL;
    qlpListElement *                sListElement = NULL;
    qlpValue                      * sLogFile;
    stlBool                         sFileExist;

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_DATABASE_RENAME_LOGFILE_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_ALTER_DATABASE_RENAME_LOGFILE_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득 
     */

    sParseTree = (qlpAlterDatabaseRenameLogfile *) aParseTree;
 
    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qldInitRenameLogFile),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qldInitRenameLogFile) );

    STL_TRY_THROW( sParseTree->mFromFileList->mCount == sParseTree->mToFileList->mCount,
                   RAMP_ERR_LOGFILE_COUNT_MISMATCH );

    sInitPlan->mAttrCount = sParseTree->mFromFileList->mCount;

    /**
     * From File List 작성
     */
    sPrevMemberAttr = NULL;
    QLP_LIST_FOR_EACH( sParseTree->mFromFileList, sListElement )
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


        STL_TRY( smlIsUsedLogfile( sLogMemberAttr->mName,
                                   &sFileExist,
                                   NULL, /* aGroupId */
                                   SML_ENV(aEnv) ) == STL_SUCCESS );

        STL_TRY_THROW( sFileExist == STL_TRUE, RAMP_ERR_MEMBER_NOT_FOUND );

        /**
         * 추가할 파일들의 중복 파일 검사
         */

        sTmpMemberAttr = sInitPlan->mFromFileAttr;

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
            sInitPlan->mFromFileAttr = sLogMemberAttr;
        }
        else
        {
            sPrevMemberAttr->mNext = sLogMemberAttr;
        }
        sPrevMemberAttr = sLogMemberAttr;
    }

    /**
     * To File List 작성
     */
    sPrevMemberAttr = NULL;
    QLP_LIST_FOR_EACH( sParseTree->mToFileList, sListElement )
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


        STL_TRY( smlIsUsedLogfile( sLogMemberAttr->mName,
                                   &sFileExist,
                                   NULL, /* aGroupId */
                                   SML_ENV(aEnv) ) == STL_SUCCESS );

        STL_TRY_THROW( sFileExist != STL_TRUE, RAMP_ERR_USED_LOGFILE );

        /**
         * 추가할 파일들의 중복 파일 검사
         */

        sTmpMemberAttr = sInitPlan->mToFileAttr;

        while( sTmpMemberAttr != NULL )
        {
            if( stlStrcmp( sLogMemberAttr->mName, sTmpMemberAttr->mName ) == 0 )
            {
                STL_THROW( RAMP_ERR_DUPLICATE_LOGFILE_NAME );
            }

            sTmpMemberAttr = sTmpMemberAttr->mNext;
        }

        STL_TRY( smlExistLogfile( sLogMemberAttr->mName,
                                  &sFileExist,
                                  SML_ENV(aEnv) ) == STL_SUCCESS );

        STL_TRY_THROW( sFileExist == STL_TRUE, RAMP_ERR_FILE_NOT_FOUND );

        sLogMemberAttr->mValidFlags |= SML_LOG_MEMBER_NAME_YES;
        sLogMemberAttr->mNext = NULL;

        if( sPrevMemberAttr == NULL )
        {
            sInitPlan->mToFileAttr = sLogMemberAttr;
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

    STL_CATCH( RAMP_ERR_LOGFILE_COUNT_MISMATCH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_LOG_FILE_COUNT_MISMATCH,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_MEMBER_NOT_FOUND )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_LOG_MEMBER_NOT_FOUND,
                      qlgMakeErrPosString( aSQLString,
                                           sLogFile->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }


    STL_CATCH( RAMP_ERR_FILE_NOT_FOUND )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_LOG_FILE_NOT_FOUND,
                      qlgMakeErrPosString( aSQLString,
                                           sLogFile->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
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

    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief ALTER DATABASE RENAME LOG FILE 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qldOptCodeAlterDatabaseRenameLogfile( smlTransId      aTransID,
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
 * @brief ALTER DATABASE RENAME LOG FILE 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qldOptDataAlterDatabaseRenameLogfile( smlTransId      aTransID,
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
 * @brief ALTER DATABASE RENAME LOG FILE 을 수행한다
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aDBCStmt  DBC Statement
 * @param[in] aSQLStmt  SQL Statement
 * @param[in] aEnv      Environment
 */
stlStatus qldExecuteAlterDatabaseRenameLogfile( smlTransId      aTransID,
                                                smlStatement  * aStmt,
                                                qllDBCStmt    * aDBCStmt,
                                                qllStatement  * aSQLStmt,
                                                qllEnv        * aEnv )
{
    stlChar              * sBuf = NULL;
    stlChar              * sPtr;
    stlInt32               sArgSize;
    stlInt32               i;
    smlLogMemberAttr     * sFromAttr;
    smlLogMemberAttr     * sToAttr;
    qldInitRenameLogFile * sInitPlan = (qldInitRenameLogFile*)aSQLStmt->mInitPlan;
 
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_DATABASE_RENAME_LOGFILE_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    sArgSize = STL_ALIGN( STL_SIZEOF(stlInt32), 8 )
        + (2 * sInitPlan->mAttrCount * STL_ALIGN( STL_SIZEOF(smlLogMemberAttr), 8));

    STL_TRY( knlAllocRegionMem( & QLL_EXEC_DDL_MEM(aEnv),
                                sArgSize,
                                (void **) & sBuf,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sBuf, 0x00, sArgSize );
    sPtr = sBuf;

    stlMemcpy( sPtr, &sInitPlan->mAttrCount, STL_SIZEOF(stlInt32) );
    sPtr += STL_ALIGN( STL_SIZEOF(stlInt32), 8 );
    sFromAttr = sInitPlan->mFromFileAttr;
    sToAttr = sInitPlan->mToFileAttr;
    
    for( i = 0; i < sInitPlan->mAttrCount; i++ )
    {
        /**
         * copy from file
         */
        
        stlMemcpy( sPtr,
                   sFromAttr,
                   STL_SIZEOF(smlLogMemberAttr) );
        sPtr += STL_ALIGN( STL_SIZEOF(smlLogMemberAttr), 8 );
        
        /**
         * copy to file
         */
        
        stlMemcpy( sPtr,
                   sToAttr,
                   STL_SIZEOF(smlLogMemberAttr) );
        sPtr += STL_ALIGN( STL_SIZEOF(smlLogMemberAttr), 8);
        sFromAttr = sFromAttr->mNext;
        sToAttr = sToAttr->mNext;
    }

    STL_ASSERT( sPtr - sBuf == sArgSize );

    STL_TRY( knlAddEnvEvent( SML_EVENT_RENAME_LOGFILE,
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

/** @} qldAlterDatabaseRenameLogfile */
