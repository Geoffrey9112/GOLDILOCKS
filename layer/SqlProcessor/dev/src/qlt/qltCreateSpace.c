/*******************************************************************************
 * qltCreateSpace.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: qltCreateSpace.c 5803 2012-09-27 06:40:12Z jhkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file qltCreateSpace.c
 * @brief CREATE TABLESPACE .. 처리 루틴 
 */

#include <qll.h>

#include <qlDef.h>


/**
 * @defgroup qltCreateSpace CREATE TABLESPACE
 * @ingroup qlt
 * @{
 */


/**
 * @brief CREATE TABLESPACE .. 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qltValidateCreateSpace( smlTransId      aTransID,
                                  qllDBCStmt    * aDBCStmt,
                                  qllStatement  * aSQLStmt,
                                  stlChar       * aSQLString,
                                  qllNode       * aParseTree,
                                  qllEnv        * aEnv )
{
    qltInitCreateSpace * sInitPlan = NULL;
    qlpTablespaceDef   * sParseTree = NULL;

    qlpListElement     * sListElement = NULL;
    qlpDatafile        * sDataFile = NULL;
    
    stlBool              sObjectExist = STL_FALSE;
    smlTbsAttr         * sSpaceAttr = NULL;
    stlBool              sFileExist = STL_FALSE;
    
    smlDatafileAttr    * sPrevFileAttr = NULL;
    smlDatafileAttr    * sCurrFileAttr = NULL;
    smlDatafileAttr    * sDuplicateFileAttr = NULL;
    ellDictHandle        sSpaceHandle;
    stlInt64             sBigint;
    qlpSize            * sExtSize = NULL;

    ellDictHandle      * sAuthHandle = NULL;
    stlInt64             sDatabaseTestOption = 0;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_CREATE_TABLESPACE_TYPE, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_CREATE_TABLESPACE_TYPE, QLL_ERROR_STACK(aEnv) );

    sDatabaseTestOption = knlGetPropertyBigIntValueByID( KNL_PROPERTY_DATABASE_TEST_OPTION,
                                                         KNL_ENV(aEnv) );

    STL_TRY_THROW( sDatabaseTestOption <2, RAMP_ERR_DISALLOW_STATEMENT );
    
    /**
     * 기본 정보 획득 
     */

    sParseTree = (qlpTablespaceDef *) aParseTree;

    STL_TRY_THROW( (sParseTree->mMediaType == QLP_SPACE_MEDIA_TYPE_MEMORY) &&
                   ((sParseTree->mUsageType == QLP_SPACE_USAGE_TYPE_DATA) ||
                    (sParseTree->mUsageType == QLP_SPACE_USAGE_TYPE_TEMP)),
                   RAMP_ERR_NOT_IMPLEMENTED );

    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qltInitCreateSpace),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qltInitCreateSpace) );

    STL_TRY( qlvSetInitPlan( aDBCStmt,
                             aSQLStmt,
                             & sInitPlan->mCommonInit,
                             aEnv )
             == STL_SUCCESS );
    
    /*************************************************
     * Tablespace name validation
     *************************************************/
    
    /**
     * Tablespace 존재 여부
     */

    STL_TRY( ellGetTablespaceDictHandle( aTransID,
                                         aSQLStmt->mViewSCN,
                                         QLP_GET_PTR_VALUE( sParseTree->mName ),
                                         & sSpaceHandle,
                                         & sObjectExist,
                                         ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    STL_TRY_THROW( sObjectExist != STL_TRUE, RAMP_ERR_USED_TABLESPACE_NAME );

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF( smlTbsAttr ),
                                (void **) & sSpaceAttr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    sInitPlan->mSpaceAttr = sSpaceAttr;
    
    stlMemset( sSpaceAttr, 0x00, STL_SIZEOF(smlTbsAttr) );
    stlStrcpy( sSpaceAttr->mName, QLP_GET_PTR_VALUE(sParseTree->mName) );
    
    /**
     * CREATE TABLESPACE ON DATABASE 권한 검사
     */

    STL_TRY( qlaCheckDatabasePriv( aTransID,
                                   aDBCStmt,
                                   aSQLStmt,
                                   ELL_DB_PRIV_ACTION_CREATE_TABLESPACE,
                                   aEnv )
             == STL_SUCCESS );
    
    /*************************************************
     * Tablespace attribute validation
     *************************************************/
    
    /**
     * Tablespace management attribute
     */

    if( sParseTree->mTbsAttrs != NULL )
    {
        if( sParseTree->mTbsAttrs->mIsLogging != NULL )
        {
            sSpaceAttr->mValidFlags |= SML_TBS_LOGGING_YES;
            sSpaceAttr->mIsLogging = QLP_GET_INT_VALUE(sParseTree->mTbsAttrs->mIsLogging);
        }
    
        if( sParseTree->mTbsAttrs->mIsOnline != NULL )
        {
            sSpaceAttr->mValidFlags |= SML_TBS_ONLINE_YES;
            sSpaceAttr->mIsOnline = QLP_GET_INT_VALUE(sParseTree->mTbsAttrs->mIsOnline);
        }
        
        if( sParseTree->mTbsAttrs->mExtSize != NULL )
        {
            sExtSize = sParseTree->mTbsAttrs->mExtSize;
            
            STL_TRY( dtlGetIntegerFromString( QLP_GET_PTR_VALUE(sExtSize->mSize),
                                              stlStrlen( QLP_GET_PTR_VALUE(sExtSize->mSize) ),
                                              & sBigint,
                                              QLL_ERROR_STACK(aEnv) )
                     == STL_SUCCESS );
            sBigint *= QLP_GET_BYTES_FROM_SIZE_UNIT( QLP_GET_INT_VALUE(sExtSize->mSizeUnit) );
            
            sSpaceAttr->mValidFlags |= SML_TBS_EXTSIZE_YES;
            sSpaceAttr->mExtSize = sBigint;
        }
    }

    if( (sParseTree->mMediaType == QLP_SPACE_MEDIA_TYPE_MEMORY) &&
        (sParseTree->mUsageType == QLP_SPACE_USAGE_TYPE_DATA) )
    {
        sSpaceAttr->mValidFlags |= SML_TBS_ATTR_YES;
        sSpaceAttr->mAttr |= ( SML_TBS_DEVICE_MEMORY |
                               SML_TBS_TEMPORARY_NO  |
                               SML_TBS_USAGE_DATA );

        sInitPlan->mMediaType = ELL_SPACE_MEDIA_TYPE_MEMORY;
        sInitPlan->mUsageType = ELL_SPACE_USAGE_TYPE_DATA;
    }
    else
    {
        STL_DASSERT( (sParseTree->mMediaType == QLP_SPACE_MEDIA_TYPE_MEMORY) &&
                     (sParseTree->mUsageType == QLP_SPACE_USAGE_TYPE_TEMP) );

        sSpaceAttr->mValidFlags |= SML_TBS_ATTR_YES;
        sSpaceAttr->mAttr |= ( SML_TBS_DEVICE_MEMORY |
                               SML_TBS_TEMPORARY_YES |
                               SML_TBS_USAGE_TEMPORARY );
        
        sInitPlan->mMediaType = ELL_SPACE_MEDIA_TYPE_MEMORY;
        sInitPlan->mUsageType = ELL_SPACE_USAGE_TYPE_TEMPORARY;
    }
    
    /**
     * Tablespace file attribute
     */

    sPrevFileAttr = NULL;
    
    QLP_LIST_FOR_EACH( sParseTree->mFileSpecs, sListElement )
    {
        sDataFile = (qlpDatafile *) QLP_LIST_GET_POINTER_VALUE( sListElement );
        
        /**
         * File Attr 공간 할당
         */
        
        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                    STL_SIZEOF(smlDatafileAttr),
                                    (void **) & sCurrFileAttr,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        stlMemset( sCurrFileAttr, 0x00, STL_SIZEOF(smlDatafileAttr) );

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
         * datafile validation
         */
        if( (sParseTree->mMediaType != QLP_SPACE_MEDIA_TYPE_MEMORY) ||
            (sParseTree->mUsageType != QLP_SPACE_USAGE_TYPE_TEMP) )
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
                                        NULL,  /* aAged */
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

        sDuplicateFileAttr = sSpaceAttr->mDatafileAttr;
        
        while( sDuplicateFileAttr != NULL )
        {
            if( stlStrcmp( sCurrFileAttr->mName, sDuplicateFileAttr->mName ) == 0 )
            {
                STL_THROW( RAMP_ERR_DUPLICATE_DATAFILE_NAME );
            }

            sDuplicateFileAttr = sDuplicateFileAttr->mNext;
        }
        
        /**
         * File Link 연결
         */

        if( sPrevFileAttr == NULL )
        {
            sSpaceAttr->mDatafileAttr = sCurrFileAttr;
        }
        else
        {
            sPrevFileAttr->mNext = sCurrFileAttr;
        }

        sPrevFileAttr = sCurrFileAttr;
    }

    /********************************************************
     * Validation Object 추가 
     ********************************************************/

    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );
    
    STL_TRY( ellAppendObjectItem( sInitPlan->mCommonInit.mValidationObjList,
                                  sAuthHandle,
                                  QLL_INIT_PLAN(aDBCStmt),
                                  ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    
    /****************************************************
     * Init Plan 연결 
     ****************************************************/

    aSQLStmt->mInitPlan = (void *) sInitPlan;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_IMPLEMENTED,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      "qltValidateCreateSpace()" );
    }
    
    STL_CATCH( RAMP_ERR_USED_TABLESPACE_NAME )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_USED_TABLESPACE_NAME,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      QLP_GET_PTR_VALUE( sParseTree->mName ) );
    }

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
 * @brief CREATE TABLESPACE .. 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qltOptCodeCreateSpace( smlTransId      aTransID,
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
 * @brief CREATE TABLESPACE .. 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qltOptDataCreateSpace( smlTransId      aTransID,
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
 * @brief CREATE TABLESPACE .. 을 수행한다
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aDBCStmt  DBC Statement
 * @param[in] aSQLStmt  SQL Statement
 * @param[in] aEnv      Environment
 */
stlStatus qltExecuteCreateSpace( smlTransId      aTransID,
                                 smlStatement  * aStmt,
                                 qllDBCStmt    * aDBCStmt,
                                 qllStatement  * aSQLStmt,
                                 qllEnv        * aEnv )
{
    qltInitCreateSpace * sInitPlan = NULL;
    ellDictHandle      * sAuthHandle = NULL;
    stlTime              sTime = 0;
    smlTbsId             sTbsId;

    stlBool   sLocked = STL_TRUE;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_CREATE_TABLESPACE_TYPE,
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

    sInitPlan = (qltInitCreateSpace *) aSQLStmt->mInitPlan;
    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );
    
    /**
     * CREATE TABLESPACE 구문을 위한 DDL Lock 획득
     */
    
    STL_TRY( ellLock4CreateTablespace( aTransID,
                                       aStmt,
                                       sAuthHandle,
                                       & sLocked,
                                       ELL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sLocked == STL_TRUE, RAMP_RETRY );

    /**
     * Valid Plan 을 다시 획득
     */

    STL_TRY( qlgGetValidPlan4DDL( aTransID,
                                  aDBCStmt,
                                  aSQLStmt,
                                  QLL_PHASE_EXECUTE,
                                  aEnv )
             == STL_SUCCESS );
    
    sInitPlan = (qltInitCreateSpace *) aSQLStmt->mInitPlan;
    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );

    /***************************************************************
     * CREATE TABLESPACE
     ***************************************************************/

    /**
     * Tablespace 생성
     */

    STL_TRY( smlCreateTablespace( aStmt,
                                  sInitPlan->mSpaceAttr,
                                  0,          /* aReservedPageCount */
                                  &sTbsId,
                                  SML_ENV(aEnv) )
             == STL_SUCCESS );

    KNL_BREAKPOINT( KNL_BREAKPOINT_QLTEXECUTECREATESPACE_AFTER_CREATE_TBS,
                    KNL_ENV(aEnv) );

    /**
     * Tablespace Descriptor 추가
     */

    STL_TRY( ellInsertTablespaceDesc( aTransID,
                                      aStmt,
                                      sTbsId,
                                      ellGetAuthID( sAuthHandle ),
                                      sInitPlan->mSpaceAttr->mName,
                                      sInitPlan->mMediaType,
                                      sInitPlan->mUsageType,
                                      NULL,   /* aTablespaceComment */
                                      ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Tablespace Cache 추가
     */
    
    STL_TRY( ellRefineCache4AddTablespace( aTransID,
                                           aStmt,
                                           ellGetAuthID( sAuthHandle ),
                                           sTbsId,
                                           ELL_ENV(aEnv) )
             == STL_SUCCESS );

    KNL_BREAKPOINT( KNL_BREAKPOINT_QLTEXECUTECREATESPACE_AFTER_REFINE_CACHE,
                    KNL_ENV(aEnv) );

    /**
     * 생성한 객체에 대해 Lock 을 획득
     * 
     * commit -> pending -> trans end (unlock) 과정 중에
     * 해당 객체에 대한 DROP, ALTER 가 수행될 경우를 방지하기 위해
     * 생성한 객체에 대한 lock 을 획득해야 함.
     */

    STL_TRY( ellLockRowAfterCreateNonBaseTableObject( aTransID,
                                                      aStmt,
                                                      ELL_OBJECT_TABLESPACE,
                                                      sTbsId,
                                                      ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}



/** @} qltCreateSpace */
