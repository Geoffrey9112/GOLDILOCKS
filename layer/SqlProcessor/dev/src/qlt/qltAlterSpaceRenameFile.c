/*******************************************************************************
 * qltAlterSpaceRenameFile.c
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
 * @file qltAlterSpaceRenameFile.c
 * @brief ALTER TABLESPACE .. RENAME FILE 처리 루틴 
 */

#include <qll.h>

#include <qlDef.h>


/**
 * @defgroup qltAlterSpaceRenameFile ALTER TABLESPACE .. RENAME FILE
 * @ingroup qltAlterSpace
 * @{
 */


/**
 * @brief ALTER TABLESPACE .. RENAME FILE 구문을 Validation 한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aSQLString SQL String 
 * @param[in] aParseTree Parse Tree
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qltValidateAlterSpaceRenameFile( smlTransId      aTransID,
                                           qllDBCStmt    * aDBCStmt,
                                           qllStatement  * aSQLStmt,
                                           stlChar       * aSQLString,
                                           qllNode       * aParseTree,
                                           qllEnv        * aEnv )
{
    qltInitAlterSpaceRenameFile * sInitPlan = NULL;
    qlpAlterSpaceRenameFile     * sParseTree = NULL;

    ellTablespaceMediaType        sMediaType = ELL_SPACE_MEDIA_TYPE_NA;
    ellTablespaceUsageType        sUsageType = ELL_SPACE_USAGE_TYPE_NA;

    qlpListElement              * sToListElement = NULL;
    qlpListElement              * sFromListElement = NULL;
    
    qlpValue                    * sToDataFile = NULL;
    qlpValue                    * sFromDataFile = NULL;
    stlBool                       sFileExist = STL_FALSE;

    smlDatafileAttr             * sPrevToFileAttr = NULL;
    smlDatafileAttr             * sPrevFromFileAttr = NULL;
    smlDatafileAttr             * sCurrToFileAttr = NULL;
    smlDatafileAttr             * sCurrFromFileAttr = NULL;
    smlDatafileAttr             * sDuplicateFileAttr = NULL;
    
    stlBool                       sObjectExist = STL_FALSE;
    stlInt64                      sSpaceId;
    smlTbsId                      sSmlTbsId;
    smlTbsId                      sTbsId;
    stlInt32                      sFileCount;
    stlInt32                      sCurrentFile;
    stlInt32                      i;
    stlInt64                      sDatabaseTestOption = 0;
    stlBool                       sAged = STL_FALSE;

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_TABLESPACE_RENAME_FILE_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTree->mType == QLL_STMT_ALTER_TABLESPACE_RENAME_FILE_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    sDatabaseTestOption = knlGetPropertyBigIntValueByID( KNL_PROPERTY_DATABASE_TEST_OPTION,
                                                         KNL_ENV(aEnv) );

    STL_TRY_THROW( sDatabaseTestOption < 2, RAMP_ERR_DISALLOW_STATEMENT );
    
    /**
     * 기본 정보 획득 
     */

    sParseTree = (qlpAlterSpaceRenameFile *) aParseTree;

    /**
     * Init Plan 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                STL_SIZEOF(qltInitAlterSpaceRenameFile),
                                (void **) & sInitPlan,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sInitPlan, 0x00, STL_SIZEOF(qltInitAlterSpaceRenameFile) );

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

    sSpaceId = ellGetTablespaceID( &sInitPlan->mSpaceHandle );
    
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

    /*************************************************
     * FILE 수를 계산하여 미리 메모리를 할당한다.
     *************************************************/

    sFileCount = 0;

    QLP_LIST_FOR_EACH( sParseTree->mToFileList, sToListElement )
    {
        sFileCount++;
    }

    sInitPlan->mFileAttrLen = 2 * sFileCount * STL_SIZEOF(smlDatafileAttr);

    /**
     * File Attr 공간 할당
     *  FromFileAttr[sFileCount]         ToFileAttr[sFileCount]       
     *  +--------------------------------+--------------------------------+
     *  |  |  |  |         ...           |  |  |  |       ...             |
     *  +--------------------------------+--------------------------------+
     */
        
    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                sInitPlan->mFileAttrLen,
                                (void **) & sInitPlan->mFileAttr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sInitPlan->mFileAttr, 0x00, sInitPlan->mFileAttrLen );

    sInitPlan->mFromFileAttr = &sInitPlan->mFileAttr[0];
    sInitPlan->mToFileAttr   = &sInitPlan->mFileAttr[sFileCount];

    /*************************************************
     * FILE 정보 검증 
     *************************************************/

    sPrevToFileAttr = NULL;
    sPrevFromFileAttr = NULL;
    sToListElement = NULL;
    sCurrentFile = 0;

    sFromListElement = QLP_LIST_GET_FIRST( sParseTree->mFromFileList );
    
    QLP_LIST_FOR_EACH( sParseTree->mToFileList, sToListElement )
    {
        STL_TRY_THROW( sFromListElement != NULL, RAMP_ERR_MISMATCHED_TO_DATAFILE_PAIRS );
        
        sToDataFile = (qlpValue *) QLP_LIST_GET_POINTER_VALUE( sToListElement );
        sFromDataFile = (qlpValue *) QLP_LIST_GET_POINTER_VALUE( sFromListElement );
        
        sCurrFromFileAttr = &sInitPlan->mFromFileAttr[sCurrentFile];
        sCurrToFileAttr = &sInitPlan->mToFileAttr[sCurrentFile];
        sCurrentFile++;

        sCurrFromFileAttr->mNext = NULL;
        sCurrToFileAttr->mNext = NULL;

        /**
         * File Attribute 정보 설정
         */

        stlStrcpy( sCurrToFileAttr->mName, QLP_GET_PTR_VALUE( sToDataFile ) );
        stlStrcpy( sCurrFromFileAttr->mName, QLP_GET_PTR_VALUE( sFromDataFile ) );
        
        /**
         * Persistent datafile validation
         */
        if( (sMediaType != ELL_SPACE_MEDIA_TYPE_MEMORY) ||
            (sUsageType != ELL_SPACE_USAGE_TYPE_TEMPORARY) )
        {
            STL_TRY( smlConvAbsDatafilePath( sCurrFromFileAttr->mName,
                                             SML_ENV(aEnv) )
                     == STL_SUCCESS );
            STL_TRY( smlConvAbsDatafilePath( sCurrToFileAttr->mName,
                                             SML_ENV(aEnv) )
                     == STL_SUCCESS );

            /**
             * 물리적인 파일이 존재하는지 검사한다.
             */
            STL_TRY( smlExistDatafile( sCurrToFileAttr->mName,
                                       &sFileExist,
                                       SML_ENV(aEnv) ) == STL_SUCCESS );

            STL_TRY_THROW( sFileExist == STL_TRUE, RAMP_ERR_FILE_NOT_EXIST );

            /**
             * 해당 경로가 Database에서 이미 사용중인지 검사한다.
             * - 파일이 물리적으로 존재하지 않더라도 데이타 파일 경로는
             *   사용중에 있을수 있다.
             */
            STL_TRY( smlIsUsedDatafile( sCurrToFileAttr->mName,
                                        NULL,  /* aTbsId */
                                        NULL,  /* aDatafileId */
                                        &sFileExist,
                                        NULL,
                                        SML_ENV(aEnv) ) == STL_SUCCESS );

            STL_TRY_THROW( sFileExist == STL_FALSE, RAMP_ERR_USED_DATAFILE );
            
            STL_TRY( smlIsUsedDatafile( sCurrFromFileAttr->mName,
                                        &sSmlTbsId,
                                        NULL,        /* aDatafileId */
                                        &sFileExist,
                                        &sAged,
                                        SML_ENV(aEnv) ) == STL_SUCCESS );

            /**
             * Rename될 source datafile이 해당 tablespace에 생성된 file이어야 한다.
             */
            STL_TRY_THROW( (sFileExist == STL_TRUE) &&
                           (sSmlTbsId == sSpaceId) &&
                           (sAged == STL_FALSE),
                           RAMP_ERR_DATAFILE_NOT_EXISTS );
        }
        else
        {
            STL_TRY( smlIsUsedMemory( sCurrToFileAttr->mName,
                                      NULL,  /* aTbsId */
                                      NULL,  /* aDatafileId */
                                      &sFileExist,
                                      SML_ENV(aEnv) ) == STL_SUCCESS );
            
            STL_TRY_THROW( sFileExist == STL_FALSE, RAMP_ERR_USED_MEMORY );
            
            STL_TRY( smlIsUsedMemory( sCurrFromFileAttr->mName,
                                      NULL,  /* aTbsId */
                                      NULL,  /* aDatafileId */
                                      &sFileExist,
                                      SML_ENV(aEnv) ) == STL_SUCCESS );
            
            STL_TRY_THROW( sFileExist == STL_TRUE, RAMP_ERR_MEMORY_NOT_EXISTS );
        }

        /**
         * 변경후 파일들의 중복 파일 검사
         */

        for( i = 0; i < sCurrentFile - 1; i++ )
        {
            sDuplicateFileAttr = &sInitPlan->mToFileAttr[i];

            if( stlStrcmp( sCurrToFileAttr->mName, sDuplicateFileAttr->mName ) == 0 )
            {
                STL_THROW( RAMP_ERR_DUPLICATE_DATAFILE_NAME );
            }
        }

        sCurrToFileAttr->mTbsId = sTbsId;
        sCurrFromFileAttr->mTbsId = sTbsId;
        
        /**
         * File Link 연결
         */

        if ( sPrevToFileAttr != NULL )
        {
            sPrevToFileAttr->mNext = sCurrToFileAttr;
            sPrevFromFileAttr->mNext = sCurrFromFileAttr;
        }

        sPrevToFileAttr = sCurrToFileAttr;
        sPrevFromFileAttr = sCurrFromFileAttr;

        sFromListElement = QLP_LIST_GET_NEXT_ELEM( sFromListElement );
    }
    
    STL_TRY_THROW( sFromListElement == NULL, RAMP_ERR_MISMATCHED_FROM_DATAFILE_PAIRS );

    /**
     * Offline Tablespace 검사
     * - MOUNT 상태이거나 OFFLINE tablespace여야 한다.
     * - CDS모드는 OPEN상태에서도 가능하다.
     */
    
    if( knlGetCurrStartupPhase() >= KNL_STARTUP_PHASE_OPEN )
    {
        if( smlGetDataStoreMode() == SML_DATA_STORE_MODE_TDS )
        {
            if( (sMediaType != ELL_SPACE_MEDIA_TYPE_MEMORY) ||
                (sUsageType != ELL_SPACE_USAGE_TYPE_TEMPORARY) )
            {
                STL_TRY_THROW( smlIsOnlineTablespace( sSpaceId ) == STL_FALSE,
                               RAMP_ERR_RENAME_ONLINE_DATAFILE );
            }
        }
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

    STL_CATCH( RAMP_ERR_MISMATCHED_FROM_DATAFILE_PAIRS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_MISMATCHED_DATAFILE_PAIRS,
                      qlgMakeErrPosString( aSQLString,
                                           ((qlpValue*)QLP_LIST_GET_POINTER_VALUE(sFromListElement))->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_CATCH( RAMP_ERR_DATAFILE_NOT_EXISTS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_DATAFILE_NOT_EXISTS,
                      qlgMakeErrPosString( aSQLString,
                                           sFromDataFile->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_CATCH( RAMP_ERR_MEMORY_NOT_EXISTS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_MEMORY_NOT_EXISTS,
                      qlgMakeErrPosString( aSQLString,
                                           sFromDataFile->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_CATCH( RAMP_ERR_MISMATCHED_TO_DATAFILE_PAIRS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_MISMATCHED_DATAFILE_PAIRS,
                      qlgMakeErrPosString( aSQLString,
                                           ((qlpValue*)QLP_LIST_GET_POINTER_VALUE(sToListElement))->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_RENAME_ONLINE_DATAFILE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_RENAME_ONLINE_DATAFILE,
                      qlgMakeErrPosString( aSQLString,
                                           sParseTree->mName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_FILE_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_FILE_NOT_EXIST,
                      qlgMakeErrPosString( aSQLString,
                                           sToDataFile->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_USED_DATAFILE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_ALREADY_USED_DATAFILE,
                      qlgMakeErrPosString( aSQLString,
                                           sToDataFile->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_USED_MEMORY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_ALREADY_USED_MEMORY,
                      qlgMakeErrPosString( aSQLString,
                                           sToDataFile->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_DUPLICATE_DATAFILE_NAME )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_DUPLICATE_DATAFILE_NAME,
                      qlgMakeErrPosString( aSQLString,
                                           sToDataFile->mNodePos,
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
 * @brief ALTER TABLESPACE .. RENAME FILE 구문의 Code Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qltOptCodeAlterSpaceRenameFile( smlTransId      aTransID,
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
 * @brief ALTER TABLESPACE .. RENAME FILE 구문의 Data Area 를 최적화한다.
 * @param[in] aTransID   Transaction ID
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qltOptDataAlterSpaceRenameFile( smlTransId      aTransID,
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
 * @brief ALTER TABLESPACE .. RENAME FILE 을 수행한다
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aDBCStmt  DBC Statement
 * @param[in] aSQLStmt  SQL Statement
 * @param[in] aEnv      Environment
 */
stlStatus qltExecuteAlterSpaceRenameFile( smlTransId      aTransID,
                                          smlStatement  * aStmt,
                                          qllDBCStmt    * aDBCStmt,
                                          qllStatement  * aSQLStmt,
                                          qllEnv        * aEnv )
{
    qltInitAlterSpaceRenameFile * sInitPlan = NULL;
    knlStartupPhase               sStartupPhase = KNL_STARTUP_PHASE_NONE;
    stlTime                       sTime = 0;

    stlBool  sLocked = STL_TRUE;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt->mStmtType == QLL_STMT_ALTER_TABLESPACE_RENAME_FILE_TYPE,
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

    sInitPlan = (qltInitAlterSpaceRenameFile *) aSQLStmt->mInitPlan;
    
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

    sInitPlan = (qltInitAlterSpaceRenameFile *) aSQLStmt->mInitPlan;

    /***************************************************************
     * FILE 추가 
     ***************************************************************/

    if( sStartupPhase >= KNL_STARTUP_PHASE_OPEN )
    {
        STL_TRY( smlRenameDatafiles( aStmt,
                                     ellGetTablespaceID( & sInitPlan->mSpaceHandle ),
                                     sInitPlan->mFromFileAttr,
                                     sInitPlan->mToFileAttr,
                                     SML_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( knlAddEnvEvent( SML_EVENT_RENAME_DATAFILE,
                                 NULL,                               /* aEventMem */
                                 sInitPlan->mFileAttr,               /* aData */
                                 sInitPlan->mFileAttrLen,            /* aDataSize */
                                 0,                                  /* aTargetEnvId */
                                 KNL_EVENT_WAIT_POLLING,
                                 STL_FALSE, /* aForceSuccess */
                                 KNL_ENV( aEnv ) )
                 == STL_SUCCESS );
    }

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




/** @} qltAlterSpaceRenameFile */
