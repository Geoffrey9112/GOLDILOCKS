/*******************************************************************************
 * qltAlterSpaceAddFile.c
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
 * @file qltAlterSpaceAddFile.c
 * @brief ALTER TABLESPACE .. ADD 처리 루틴 
 */

#include <qll.h>

#include <qlDef.h>


/**
 * @defgroup qltAlterSpaceAddFile ALTER TABLESPACE .. ADD FILE 
 * @ingroup qltAlterSpace
 * @{
 */


/**
 * @brief ALTER TABLESPACE .. ADD FILE 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qltValidateAlterSpaceAddFile( smlTransId      aTransID,
                                        qllDBCStmt    * aDBCStmt,
                                        qllStatement  * aSQLStmt,
                                        stlChar       * aSQLString,
                                        qllNode       * aParseTree,
                                        qllEnv        * aEnv )
{
    qltInitAlterSpaceAddFile * sInitPlan = NULL;
    qlpAlterSpaceAddFile     * sParseTree = NULL;

    ellTablespaceMediaType  sMediaType = ELL_SPACE_MEDIA_TYPE_NA;
    ellTablespaceUsageType  sUsageType = ELL_SPACE_USAGE_TYPE_NA;
    stlInt64                sDatabaseTestOption = 0;

    qlpListElement * sListElement = NULL;
    
    qlpDatafile    * sDataFile = NULL;
    stlBool          sFileExist = STL_FALSE;

    smlDatafileAttr * sPrevFileAttr = NULL;
    smlDatafileAttr * sCurrFileAttr = NULL;
    smlDatafileAttr * sDuplicateFileAttr = NULL;
    smlTbsId  sTbsId;
    stlInt32  sFileCount;
    stlInt32  sCurrentFile;
    stlInt32  i;
    
    stlBool   sObjectExist = STL_FALSE;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_TABLESPACE_ADD_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_ALTER_TABLESPACE_ADD_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    sDatabaseTestOption = knlGetPropertyBigIntValueByID( KNL_PROPERTY_DATABASE_TEST_OPTION,
                                                         KNL_ENV(aEnv) );

    STL_TRY_THROW( sDatabaseTestOption < 2, RAMP_ERR_DISALLOW_STATEMENT );
    
    /**
     * 기본 정보 획득 
     */

    sParseTree = (qlpAlterSpaceAddFile *) aParseTree;

    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qltInitAlterSpaceAddFile),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qltInitAlterSpaceAddFile) );

    STL_TRY( qlvSetInitPlan( aDBCStmt,
                             aSQLStmt,
                             & sInitPlan->mCommonInit,
                             aEnv )
             == STL_SUCCESS );
    
    /*************************************************
     * TABLESPACE name 검증 
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
    
    
    /*************************************************
     * TABLESPACE 유형 검증 
     *************************************************/
    
    /**
     * Tablespace 유형 정보 검증 
     */

    sMediaType = ellGetTablespaceMediaType( & sInitPlan->mSpaceHandle );
    sUsageType = ellGetTablespaceUsageType( & sInitPlan->mSpaceHandle );
    sTbsId     = ellGetTablespaceID( & sInitPlan->mSpaceHandle );

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
                    sParseTree->mMediaType = QLP_SPACE_MEDIA_TYPE_MEMORY;
                    STL_TRY_THROW( (sUsageType == ELL_SPACE_USAGE_TYPE_DICT) ||
                                   (sUsageType == ELL_SPACE_USAGE_TYPE_DATA) ||
                                   (sUsageType == ELL_SPACE_USAGE_TYPE_UNDO),
                                   RAMP_ERR_INVALID_MEMORY_DATAFILE_CLAUSE );
                    break;
                case ELL_SPACE_MEDIA_TYPE_DISK:
                    sParseTree->mMediaType = QLP_SPACE_MEDIA_TYPE_DISK;
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

    /*************************************************
     * FILE 수를 계산하여 미리 메모리를 할당한다.
     *************************************************/

    sFileCount = 0;

    QLP_LIST_FOR_EACH( sParseTree->mFileSpecs, sListElement )
    {
        sFileCount++;
    }

    sInitPlan->mFileAttrLen = sFileCount * STL_SIZEOF(smlDatafileAttr);

    /**
     * File Attr 공간 할당
     */
        
    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                sInitPlan->mFileAttrLen,
                                (void **) & sInitPlan->mFileAttr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sInitPlan->mFileAttr, 0x00, sInitPlan->mFileAttrLen );

    /*************************************************
     * FILE 정보 검증 
     *************************************************/

    sPrevFileAttr = NULL;
    sListElement  = NULL;
    sCurrentFile  = 0;

    QLP_LIST_FOR_EACH( sParseTree->mFileSpecs, sListElement )
    {
        sDataFile = (qlpDatafile *) QLP_LIST_GET_POINTER_VALUE( sListElement );
        sCurrFileAttr = &sInitPlan->mFileAttr[sCurrentFile++];
        sCurrFileAttr->mNext = NULL;

        /**
         * File Attribute 정보 설정
         */

        STL_TRY( qltSetFileAttr( aSQLString,
                                 sDataFile,
                                 sParseTree->mMediaType,
                                 sCurrFileAttr,
                                 aEnv )
                 == STL_SUCCESS );
        
        /**
         * Persistent datafile validation
         */
        
        if( (sMediaType != ELL_SPACE_MEDIA_TYPE_MEMORY) ||
            (sUsageType != ELL_SPACE_USAGE_TYPE_TEMPORARY) )
        {
            STL_TRY( smlConvAbsDatafilePath( sCurrFileAttr->mName,
                                             SML_ENV(aEnv) )
                     == STL_SUCCESS );

            /**
             * REUSE OPTION이 없다면 물리적인 파일이 존재하는지 검사한다.
             */
            if( sDataFile->mReuse == NULL )
            {
                STL_TRY( smlExistDatafile( sCurrFileAttr->mName,
                                           &sFileExist,
                                           SML_ENV(aEnv) ) == STL_SUCCESS );
                
                STL_TRY_THROW( sFileExist == STL_FALSE, RAMP_ERR_EXIST_FILE );
            }
            
            /**
             * 해당 경로가 Database에서 이미 사용중인지 검사한다.
             * - 파일이 물리적으로 존재하지 않더라도 데이타 파일 경로는
             *   사용중에 있을수 있다.
             */
            STL_TRY( smlIsUsedDatafile( sCurrFileAttr->mName,
                                        NULL,  /* aTbsId */
                                        NULL,  /* aDatafileId */
                                        &sFileExist,
                                        NULL,
                                        SML_ENV(aEnv) ) == STL_SUCCESS );

            STL_TRY_THROW( sFileExist == STL_FALSE, RAMP_ERR_USED_DATAFILE );
        }
        else
        {
            STL_TRY( smlIsUsedMemory( sCurrFileAttr->mName,
                                      NULL,  /* aTbsId */
                                      NULL,  /* aDatafileId */
                                      &sFileExist,
                                      SML_ENV(aEnv) ) == STL_SUCCESS );
            
            STL_TRY_THROW( sFileExist == STL_FALSE, RAMP_ERR_USED_MEMORY );
        }

        /**
         * 추가할 파일들의 중복 파일 검사
         */

        for( i = 0; i < sCurrentFile - 1; i++ )
        {
            sDuplicateFileAttr = &sInitPlan->mFileAttr[i];

            if( stlStrcmp( sCurrFileAttr->mName, sDuplicateFileAttr->mName ) == 0 )
            {
                STL_THROW( RAMP_ERR_DUPLICATE_DATAFILE_NAME );
            }
        }

        sCurrFileAttr->mTbsId = sTbsId;

        /**
         * File Link 연결
         */

        if ( sPrevFileAttr != NULL )
        {
            sPrevFileAttr->mNext = sCurrFileAttr;
        }

        sPrevFileAttr = sCurrFileAttr;
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

    STL_CATCH( RAMP_ERR_EXIST_FILE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_ALREADY_EXIST_FILE,
                      qlgMakeErrPosString( aSQLString,
                                           sDataFile->mFilePath->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sCurrFileAttr->mName );
    }

    STL_CATCH( RAMP_ERR_USED_DATAFILE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_ALREADY_USED_DATAFILE,
                      qlgMakeErrPosString( aSQLString,
                                           sDataFile->mFilePath->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_USED_MEMORY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_ALREADY_USED_MEMORY,
                      qlgMakeErrPosString( aSQLString,
                                           sDataFile->mFilePath->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_DUPLICATE_DATAFILE_NAME )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_DUPLICATE_DATAFILE_NAME,
                      qlgMakeErrPosString( aSQLString,
                                           sDataFile->mFilePath->mNodePos,
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
 * @brief ALTER TABLESPACE .. ADD FILE 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qltOptCodeAlterSpaceAddFile( smlTransId      aTransID,
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
 * @brief ALTER TABLESPACE .. ADD FILE 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qltOptDataAlterSpaceAddFile( smlTransId      aTransID,
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
 * @brief ALTER TABLESPACE .. ADD FILE 을 수행한다
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aDBCStmt  DBC Statement
 * @param[in] aSQLStmt  SQL Statement
 * @param[in] aEnv      Environment
 */
stlStatus qltExecuteAlterSpaceAddFile( smlTransId      aTransID,
                                       smlStatement  * aStmt,
                                       qllDBCStmt    * aDBCStmt,
                                       qllStatement  * aSQLStmt,
                                       qllEnv        * aEnv )
{
    qltInitAlterSpaceAddFile * sInitPlan = NULL;
    knlStartupPhase            sStartupPhase = KNL_STARTUP_PHASE_NONE;
    stlTime                    sTime = 0;

    stlBool   sLocked = STL_TRUE;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_TABLESPACE_ADD_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mInitPlan != NULL, QLL_ERROR_STACK(aEnv) );

    /***************************************************************
     * DDL 준비 
     ***************************************************************/
    
    /**
     * DDL 수행 시간 설정
     */

    sTime = stlNow();
    STL_TRY( ellBeginStmtDDL( aStmt, sTime, ELL_ENV(aEnv) ) == STL_SUCCESS );

    sStartupPhase = knlGetCurrStartupPhase();

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
    
    sInitPlan = (qltInitAlterSpaceAddFile *) aSQLStmt->mInitPlan;
    
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

    sInitPlan = (qltInitAlterSpaceAddFile *) aSQLStmt->mInitPlan;

    /***************************************************************
     * FILE 추가 
     ***************************************************************/

    if( sStartupPhase >= KNL_STARTUP_PHASE_OPEN )
    {
        STL_TRY( smlAddDatafile( aStmt,
                                 ellGetTablespaceID( & sInitPlan->mSpaceHandle ),
                                 sInitPlan->mFileAttr,
                                 SML_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( knlAddEnvEvent( SML_EVENT_ADD_DATAFILE,
                                 NULL,                               /* aEventMem */
                                 sInitPlan->mFileAttr,               /* aData */
                                 sInitPlan->mFileAttrLen,            /* aDataSize */
                                 0,                                  /* aTargetEnvId */
                                 KNL_EVENT_WAIT_POLLING,
                                 STL_FALSE, /* aForceSuccess */
                                 KNL_ENV( aEnv ) )
                 == STL_SUCCESS );
    }

    KNL_BREAKPOINT( KNL_BREAKPOINT_QLTEXECUTEALTERSPACEADDFILE_AFTER_SMLADDDATAFILE,
                    KNL_ENV(aEnv) );
                                   
    /***************************************************************
     * ALTER 수행시간을 기록 
     ***************************************************************/

    if( sStartupPhase >= KNL_STARTUP_PHASE_OPEN )
    {
        STL_TRY( ellSetTimeAlterTablespace( aTransID,
                                            aStmt,
                                            ellGetTablespaceID( & sInitPlan->mSpaceHandle ),
                                            ELL_ENV(aEnv) )
                 == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}



/**
 * @brief File Attribute 정보를 설정한다.
 * @param[in]  aSQLString   SQL String
 * @param[in]  aParseFile   File Parse Tree
 * @param[in]  aSpaceMediaType Space Media Type
 * @param[out] aFileAttr    File Attribute
 * @param[in]  aEnv         Environment
 * @remarks
 */
stlStatus qltSetFileAttr( stlChar           * aSQLString,
                          qlpDatafile       * aParseFile,
                          qlpSpaceMediaType   aSpaceMediaType,
                          smlDatafileAttr   * aFileAttr,
                          qllEnv            * aEnv )
{
    stlChar * sFilePath = NULL;
    stlInt64  sBigint = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aParseFile != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aFileAttr != NULL, QLL_ERROR_STACK(aEnv) );
    
    /**
     * File Name 정보 
     */
    
    sFilePath = QLP_GET_PTR_VALUE( aParseFile->mFilePath );
    
    STL_TRY_THROW( stlStrlen(sFilePath) < STL_MAX_FILE_PATH_LENGTH,
                   RAMP_ERR_TOO_LONG_FILEPATH );

    stlStrcpy( aFileAttr->mName, sFilePath );

    /**
     * Size 정보
     */
    
    if ( aParseFile->mInitSize == NULL )
    {
        aFileAttr->mValidFlags |= SML_DATAFILE_SIZE_NO;
    }
    else
    {
        aFileAttr->mValidFlags |= SML_DATAFILE_SIZE_YES;
        
        STL_TRY_THROW( dtlGetIntegerFromString(
                           QLP_GET_PTR_VALUE(aParseFile->mInitSize->mSize),
                           stlStrlen( QLP_GET_PTR_VALUE(aParseFile->mInitSize->mSize) ),
                           & sBigint,
                           QLL_ERROR_STACK(aEnv) ) == STL_SUCCESS,
                       RAMP_ERR_INVALID_FILESIZE );
        sBigint *=
            QLP_GET_BYTES_FROM_SIZE_UNIT(
                QLP_GET_INT_VALUE(aParseFile->mInitSize->mSizeUnit) );

        STL_TRY_THROW( (sBigint >= SML_DATAFILE_MIN_SIZE) &&
                       (sBigint <= SML_DATAFILE_MAX_SIZE),
                       RAMP_ERR_INVALID_FILESIZE );

        aFileAttr->mSize = sBigint;
    }
    
    /**
     * Reuse 정보 
     */
    
    if ( aParseFile->mReuse == NULL )
    {
        aFileAttr->mValidFlags |= SML_DATAFILE_REUSE_NO;
    }
    else
    {
        aFileAttr->mValidFlags |= SML_DATAFILE_REUSE_YES;

        aFileAttr->mReuse = QLP_GET_INT_VALUE( aParseFile->mReuse );
    }
        
    /**
     * Auto Extend 정보 
     */
    
    if ( aParseFile->mAutoExtend == NULL )
    {
        aFileAttr->mValidFlags |= SML_DATAFILE_AUTOEXTEND_NO;
    }
    else
    {
        STL_TRY_THROW( aSpaceMediaType != QLP_SPACE_MEDIA_TYPE_MEMORY,
                       RAMP_ERR_MEMORY_TBS_NOT_SUPPORT_AUTOEXTEND );

        aFileAttr->mValidFlags |= SML_DATAFILE_AUTOEXTEND_YES;

        /**
         * @todo Dick TBS 가 되면 지원해야 함.
         */
        
        STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_TOO_LONG_FILEPATH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_FILEPATH_TOO_LONG,
                      qlgMakeErrPosString( aSQLString,
                                           aParseFile->mFilePath->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      STL_MAX_FILE_PATH_LENGTH );
    }

    STL_CATCH( RAMP_ERR_INVALID_FILESIZE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_FILESIZE_OUT_OF_RANGE,
                      qlgMakeErrPosString( aSQLString,
                                           aParseFile->mInitSize->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      SML_DATAFILE_MIN_SIZE,
                      SML_DATAFILE_MAX_SIZE );
    }
    
    STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_IMPLEMENTED,
                      qlgMakeErrPosString( aSQLString,
                                           aParseFile->mAutoExtend->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      "qltSetFileAttr()" );
    }

    STL_CATCH( RAMP_ERR_MEMORY_TBS_NOT_SUPPORT_AUTOEXTEND )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_MEMORY_TABLESPACE_DOES_NOT_SUPPORT_AUTOEXTEND,
                      qlgMakeErrPosString( aSQLString,
                                           aParseFile->mAutoExtend->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    return STL_FAILURE;
}



/** @} qltAlterSpaceAddFile */
