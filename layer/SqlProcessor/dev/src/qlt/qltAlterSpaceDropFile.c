/*******************************************************************************
 * qltAlterSpaceDropFile.c
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
 * @file qltAlterSpaceDropFile.c
 * @brief ALTER TABLESPACE .. DROP 처리 루틴 
 */

#include <qll.h>

#include <qlDef.h>


/**
 * @defgroup qltAlterSpaceDropFile ALTER TABLESPACE .. DROP FILE
 * @ingroup qltAlterSpace
 * @{
 */


/**
 * @brief ALTER TABLESPACE .. DROP FILE 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qltValidateAlterSpaceDropFile( smlTransId      aTransID,
                                         qllDBCStmt    * aDBCStmt,
                                         qllStatement  * aSQLStmt,
                                         stlChar       * aSQLString,
                                         qllNode       * aParseTree,
                                         qllEnv        * aEnv )
{
    qltInitAlterSpaceDropFile * sInitPlan = NULL;
    qlpAlterSpaceDropFile     * sParseTree = NULL;
    stlBool                     sFileExist = STL_FALSE;
    stlBool                     sObjectExist = STL_FALSE;
    stlInt64                    sSpaceID;
    stlInt32                    sDatafileId;
    smlTbsId                    sSmlTbsId;
    ellTablespaceMediaType      sMediaType = ELL_SPACE_MEDIA_TYPE_NA;
    ellTablespaceUsageType      sUsageType = ELL_SPACE_USAGE_TYPE_NA;
    stlInt64                    sDatabaseTestOption = 0;
    stlBool                     sAged = STL_FALSE;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_TABLESPACE_DROP_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_ALTER_TABLESPACE_DROP_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    sDatabaseTestOption = knlGetPropertyBigIntValueByID( KNL_PROPERTY_DATABASE_TEST_OPTION,
                                                         KNL_ENV(aEnv) );

    STL_TRY_THROW( sDatabaseTestOption < 2, RAMP_ERR_DISALLOW_STATEMENT );
    
    /**
     * 기본 정보 획득 
     */

    sParseTree = (qlpAlterSpaceDropFile *) aParseTree;

    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qltInitAlterSpaceDropFile),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qltInitAlterSpaceDropFile) );

    STL_TRY( qlvSetInitPlan( aDBCStmt,
                             aSQLStmt,
                             & sInitPlan->mCommonInit,
                             aEnv )
             == STL_SUCCESS );
    
    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                stlStrlen( QLP_GET_PTR_VALUE(sParseTree->mFileName) ) + 1,
                                (void **) & sInitPlan->mFileName,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlStrcpy( sInitPlan->mFileName, QLP_GET_PTR_VALUE(sParseTree->mFileName) );

    /*************************************************
     * TABLESPACE 검증 
     *************************************************/
    
    /**
     * Tablespace 존재 여부
     */

    STL_TRY( ellGetTablespaceDictHandle( aTransID,
                                         aSQLStmt->mViewSCN,
                                         QLP_GET_PTR_VALUE( sParseTree->mName ),
                                         & sInitPlan->mSpaceHandle,
                                         & sObjectExist,
                                         ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_SPACE_NOT_EXIST );

    /**
     * ALTER TABLESPACE ON DATABASE 권한 검사
     */

    STL_TRY( qlaCheckDatabasePriv( aTransID,
                                   aDBCStmt,
                                   aSQLStmt,
                                   ELL_DB_PRIV_ACTION_ALTER_TABLESPACE,
                                   aEnv )
             == STL_SUCCESS );
    
    /**
     * Tablespace 유형 정보 검증 
     */

    sMediaType = ellGetTablespaceMediaType( & sInitPlan->mSpaceHandle );
    sUsageType = ellGetTablespaceUsageType( & sInitPlan->mSpaceHandle );

    switch( sParseTree->mUsageType )
    {
        case QLP_SPACE_USAGE_TYPE_TEMP:
            switch( sParseTree->mMediaType )
            {
                case QLP_SPACE_MEDIA_TYPE_MEMORY:
                    STL_TRY_THROW( (sMediaType == ELL_SPACE_MEDIA_TYPE_MEMORY) &&
                                   (sUsageType == ELL_SPACE_USAGE_TYPE_TEMPORARY),
                                   RAMP_ERR_INVALID_MEMORY_CLAUSE );
                    break;
                case QLP_SPACE_MEDIA_TYPE_DISK:
                    STL_TRY_THROW( (sMediaType == ELL_SPACE_MEDIA_TYPE_DISK) &&
                                   (sUsageType == ELL_SPACE_USAGE_TYPE_TEMPORARY),
                                   RAMP_ERR_INVALID_TEMPFILE_CLAUSE );
                    break;
                default:
                    STL_DASSERT(0);
                    break;
            }
            break;
        case QLP_SPACE_USAGE_TYPE_DICT:
        case QLP_SPACE_USAGE_TYPE_UNDO:
        case QLP_SPACE_USAGE_TYPE_DATA:
            switch( sMediaType )
            {
                case ELL_SPACE_MEDIA_TYPE_MEMORY:
                    STL_TRY_THROW( (sUsageType == ELL_SPACE_USAGE_TYPE_DICT) ||
                                   (sUsageType == ELL_SPACE_USAGE_TYPE_DATA) ||
                                   (sUsageType == ELL_SPACE_USAGE_TYPE_UNDO),
                                   RAMP_ERR_INVALID_MEMORY_DATAFILE_CLAUSE );
                    break;
                case ELL_SPACE_MEDIA_TYPE_DISK:
                    STL_TRY_THROW( (sUsageType == ELL_SPACE_USAGE_TYPE_DICT) ||
                                   (sUsageType == ELL_SPACE_USAGE_TYPE_DATA) ||
                                   (sUsageType == ELL_SPACE_USAGE_TYPE_UNDO),
                                   RAMP_ERR_INVALID_DATAFILE_CLAUSE );
                    break;
                default:
                    STL_DASSERT(0);
                    break;
            }
            break;
        default:
            {
                STL_DASSERT(0);
                break;
            }
    }
    
    /**
     * Offline tablespace 검사
     */

    sSpaceID = ellGetTablespaceID( &sInitPlan->mSpaceHandle );
                                    
    STL_TRY_THROW( smlIsOnlineTablespace( sSpaceID ) == STL_TRUE,
                   RAMP_ERR_DROP_OFFLINE_DATAFILE );
    
    /**
     * datafile validation
     */

    if( (sMediaType == ELL_SPACE_MEDIA_TYPE_MEMORY) &&
        (sUsageType == ELL_SPACE_USAGE_TYPE_TEMPORARY) )
    {
        STL_TRY( smlIsUsedMemory( sInitPlan->mFileName,
                                  &sSmlTbsId,
                                  &sDatafileId,
                                  &sFileExist,
                                  SML_ENV(aEnv) ) == STL_SUCCESS );
        
        STL_TRY_THROW( sFileExist == STL_TRUE, RAMP_ERR_MEMORY_NOT_EXISTS );
        STL_TRY_THROW( sSpaceID == sSmlTbsId, RAMP_ERR_MEMORY_NOT_EXISTS );
        STL_TRY_THROW( sDatafileId != 0, RAMP_ERR_DROP_FIRST_MEMORY );
    }
    else
    {
        STL_TRY( smlIsUsedDatafile( sInitPlan->mFileName,
                                    &sSmlTbsId,
                                    &sDatafileId,
                                    &sFileExist,
                                    &sAged,
                                    SML_ENV(aEnv) ) == STL_SUCCESS );
        
        STL_TRY_THROW( sFileExist == STL_TRUE, RAMP_ERR_DATAFILE_NOT_EXISTS );
        STL_TRY_THROW( sSpaceID == sSmlTbsId, RAMP_ERR_DATAFILE_NOT_EXISTS );
        STL_TRY_THROW( sAged == STL_FALSE, RAMP_ERR_DATAFILE_NOT_EXISTS );
        STL_TRY_THROW( sDatafileId != 0, RAMP_ERR_DROP_FIRST_DATAFILE );
    }

    /********************************************************
     * Validation Object 추가 
     ********************************************************/

    STL_TRY( ellAppendObjectItem( sInitPlan->mCommonInit.mValidationObjList,
                                  & sInitPlan->mSpaceHandle,
                                  QLL_INIT_PLAN(aDBCStmt),
                                  ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /*************************************************
     * Plan 연결 
     *************************************************/
    
    /**
     * Init Plan 연결 
     */

    aSQLStmt->mInitPlan = (void *) sInitPlan;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_DATAFILE_NOT_EXISTS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_DATAFILE_NOT_EXISTS,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mFileName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_MEMORY_NOT_EXISTS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_MEMORY_NOT_EXISTS,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mFileName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_DROP_FIRST_DATAFILE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_DROP_FIRST_DATAFILE,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mFileName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_DROP_FIRST_MEMORY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_DROP_FIRST_MEMORY,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mFileName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_SPACE_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_TABLESPACE_NOT_EXISTS,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      QLP_GET_PTR_VALUE( sParseTree->mName ) );
    }

    STL_CATCH( RAMP_ERR_DROP_OFFLINE_DATAFILE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_RENAME_OFFLINE_TABLESPACE,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_CATCH( RAMP_ERR_INVALID_MEMORY_DATAFILE_CLAUSE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INVALID_MEMORY_DATAFILE_CLAUSE_FOR_ALTER,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mUsageTypePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      gEllTablespaceMediaTypeString[sMediaType],
                      gEllTablespaceUsageTypeString[sUsageType] );
    }

    STL_CATCH( RAMP_ERR_INVALID_MEMORY_CLAUSE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INVALID_MEMORY_CLAUSE_FOR_ALTER,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mUsageTypePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      gEllTablespaceMediaTypeString[sMediaType],
                      gEllTablespaceUsageTypeString[sUsageType] );
    }

    STL_CATCH( RAMP_ERR_INVALID_DATAFILE_CLAUSE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INVALID_DATAFILE_FOR_ALTER,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mUsageTypePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      gEllTablespaceMediaTypeString[sMediaType],
                      gEllTablespaceUsageTypeString[sUsageType] );
    }

    STL_CATCH( RAMP_ERR_INVALID_TEMPFILE_CLAUSE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INVALID_TEMPFILE_FOR_ALTER,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mUsageTypePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      gEllTablespaceMediaTypeString[sMediaType],
                      gEllTablespaceUsageTypeString[sUsageType] );
    }
    
    STL_CATCH( RAMP_ERR_DISALLOW_STATEMENT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_DISALLOW_STATEMENT,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;
    
    return STL_FAILURE;
}



/**
 * @brief ALTER TABLESPACE .. DROP FILE 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qltOptCodeAlterSpaceDropFile( smlTransId      aTransID,
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
 * @brief ALTER TABLESPACE .. DROP FILE 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qltOptDataAlterSpaceDropFile( smlTransId      aTransID,
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
 * @brief ALTER TABLESPACE .. DROP FILE 을 수행한다
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aDBCStmt  DBC Statement
 * @param[in] aSQLStmt  SQL Statement
 * @param[in] aEnv      Environment
 */
stlStatus qltExecuteAlterSpaceDropFile( smlTransId      aTransID,
                                        smlStatement  * aStmt,
                                        qllDBCStmt    * aDBCStmt,
                                        qllStatement  * aSQLStmt,
                                        qllEnv        * aEnv )
{
    qltInitAlterSpaceDropFile * sInitPlan = NULL;
    stlBool                     sFileExist = STL_FALSE;
    stlInt64                    sSpaceID;
    stlInt32                    sDatafileId;
    smlTbsId                    sSmlTbsId;
    stlTime                     sTime = 0;
    knlStartupPhase             sStartupPhase = KNL_STARTUP_PHASE_NONE;

    stlBool   sLocked = STL_TRUE;
    stlBool   sAged = STL_FALSE;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_TABLESPACE_DROP_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mInitPlan != NULL, QLL_ERROR_STACK(aEnv) );

    /***************************************************************
     * 기본 정보 설정 
     ***************************************************************/
    
    sStartupPhase = knlGetCurrStartupPhase();
    
    /**
     * DDL 수행 시간 설정
     */

    sTime = stlNow();
    STL_TRY( ellBeginStmtDDL( aStmt, sTime, ELL_ENV(aEnv) ) == STL_SUCCESS );

    /***************************************************************
     *  Run-Time Validation
     ***************************************************************/

    STL_RAMP( RAMP_RETRY );
    STL_TRY( knlCheckQueryTimeout( KNL_ENV(aEnv) ) == STL_SUCCESS );
    
    /**
     * Valid Plan 획득
     */

    STL_TRY( qlgGetValidPlan4DDL( aTransID,
                                  aDBCStmt,
                                  aSQLStmt,
                                  QLL_PHASE_EXECUTE,
                                  aEnv )
             == STL_SUCCESS );

    sInitPlan = (qltInitAlterSpaceDropFile *) aSQLStmt->mInitPlan;
    
    /**
     * DDL Lock 획득 
     */

    if( sStartupPhase >= KNL_STARTUP_PHASE_OPEN )
    {
        STL_TRY( ellLock4AlterTablespace( aTransID,
                                          aStmt,
                                          & sInitPlan->mSpaceHandle,
                                          & sLocked,
                                          ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        STL_TRY_THROW( sLocked == STL_TRUE, RAMP_RETRY );
    }

    /**
     * Valid Plan 을 다시 획득
     */

    STL_TRY( qlgGetValidPlan4DDL( aTransID,
                                  aDBCStmt,
                                  aSQLStmt,
                                  QLL_PHASE_EXECUTE,
                                  aEnv )
             == STL_SUCCESS );

    sInitPlan = (qltInitAlterSpaceDropFile *) aSQLStmt->mInitPlan;

    /***************************************************************
     * Drop File 수행
     ***************************************************************/
    
    sSpaceID = ellGetTablespaceID( & sInitPlan->mSpaceHandle );
    
    /**
     * datafile validation
     */

    if( ellGetTablespaceUsageType( &sInitPlan->mSpaceHandle ) == ELL_SPACE_USAGE_TYPE_TEMPORARY )
    {
        STL_TRY( smlIsUsedMemory( sInitPlan->mFileName,
                                  &sSmlTbsId,
                                  &sDatafileId,
                                  &sFileExist,
                                  SML_ENV(aEnv) ) == STL_SUCCESS );
        
        STL_TRY_THROW( sFileExist == STL_TRUE, RAMP_ERR_MEMORY_NOT_EXISTS );
        STL_TRY_THROW( sSpaceID == sSmlTbsId, RAMP_ERR_MEMORY_NOT_EXISTS );
    }
    else
    {
        STL_TRY( smlIsUsedDatafile( sInitPlan->mFileName,
                                    &sSmlTbsId,
                                    &sDatafileId,
                                    &sFileExist,
                                    &sAged,
                                    SML_ENV(aEnv) ) == STL_SUCCESS );
        
        STL_TRY_THROW( sFileExist == STL_TRUE, RAMP_ERR_DATAFILE_NOT_EXISTS );
        STL_TRY_THROW( sSpaceID == sSmlTbsId, RAMP_ERR_DATAFILE_NOT_EXISTS );
        STL_TRY_THROW( sAged == STL_FALSE, RAMP_ERR_DATAFILE_NOT_EXISTS );
    }
    
    /***************************************************************
     * FILE 추가 
     ***************************************************************/

    STL_TRY( smlDropDatafile( aStmt,
                              sSpaceID,
                              sInitPlan->mFileName,
                              SML_ENV(aEnv) )
             == STL_SUCCESS );

    KNL_BREAKPOINT( KNL_BREAKPOINT_QLTEXECUTEALTERSPACEDROPFILE_AFTER_SMLDROPDATAFILE,
                    KNL_ENV(aEnv) );
                                   
    /***************************************************************
     * ALTER 수행시간을 기록 
     ***************************************************************/

    if( sStartupPhase >= KNL_STARTUP_PHASE_OPEN )
    {
        STL_TRY( ellSetTimeAlterTablespace( aTransID,
                                            aStmt,
                                            sSpaceID,
                                            ELL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
                                        
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_DATAFILE_NOT_EXISTS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_DATAFILE_NOT_EXISTS,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_MEMORY_NOT_EXISTS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_MEMORY_NOT_EXISTS,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;
    
    return STL_FAILURE;
}



/** @} qltAlterSpaceDropFile */
