/*******************************************************************************
 * elgFixedTable.c
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
 * @file elgFixedTable.c
 * @brief Fixed Tables for Execution Library Layer
 */

#include <ell.h>
#include <eldt.h>
#include <eldc.h>

#include <elgFixedTable.h>

/**
 * @addtogroup elgFixedTable
 * @internal
 * @{
 */

/**
 * @brief Execution Library Layer 의 Fixed Table 과 Dump Table 들을 등록한다.
 * @param[in]   aEnv    Environment
 * @remarks
 */
stlStatus elgRegisterFixedTables( ellEnv * aEnv )
{
    knlDumpNameHelper   sDumpNameHelper;
    stlMemset( & sDumpNameHelper, 0x00, STL_SIZEOF(knlDumpNameHelper) );

    /*********************************************
     * Fixed Table 을 등록
     *********************************************/

    /**
     * X$EXE_SYSTEM_INFO Fixed Table 을 등 록
     */

    STL_TRY( knlRegisterFxTable( & gElgFxExeSystemInfoTabDesc, KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    
    /**
     * X$EXE_PROC_ENV Fixed Table 을 등 록
     */

    STL_TRY( knlRegisterFxTable( & gElgFxExeProcEnvTabDesc, KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    
    /**
     * X$EXE_SESS_ENV Fixed Table 을 등 록
     */

    STL_TRY( knlRegisterFxTable( & gElgFxExeSessEnvTabDesc, KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    
    /**
     * X$BUILTIN_FUNCTION Fixed Table 을 등록
     */
    
    STL_TRY( knlRegisterFxTable( & gElgFixedBuiltInFunctionTabDesc, KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    /**
     * X$PSEUDO_FUNCTION Fixed Table 을 등록
     */
    
    STL_TRY( knlRegisterFxTable( & gElgFixedPseudoFunctionTabDesc, KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    /**
     * X$NAMED_CURSOR Fixed Table 을 등록
     */
    
    STL_TRY( knlRegisterFxTable( & gElgNamedCursorTabDesc, KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    
    /*********************************************
     * Dump Table 을 등록
     *********************************************/
    
    /**
     * D$OBJECT_CACHE Dump Table 을 등록
     */
    
    STL_TRY( knlRegisterFxTable( & gDumpObjectCacheTabDesc, KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    /**
     * D$PRIV_CACHE Dump Table 을 등록
     */
    
    STL_TRY( knlRegisterFxTable( & gDumpPrivCacheTabDesc, KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    /**
     * D$PRODUCT Dump Table 을 등록
     */
    
    STL_TRY( knlRegisterFxTable( & gDumpProductTabDesc, KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    /**
     * Dump Object 의 Option String 을 해석하기 위해 도움을 주는 함수를 등록
     */

    sDumpNameHelper.mGetBaseTableHandle = elgGetBaseTableHandleDumpCallback;
    sDumpNameHelper.mGetIndexHandle     = elgGetIndexHandleDumpCallback;
    sDumpNameHelper.mGetSequenceHandle  = elgGetSequenceHandleDumpCallback;
    sDumpNameHelper.mGetSpaceHandle     = elgGetSpaceHandleDumpCallback;
    
    
    knlSetDumpTableNameHelper( & sDumpNameHelper );
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief Base Table 의 physical Handle 을 얻기 위한 Dump Name Helper Callback
 * @param[in]  aTransID     Transaction ID
 * @param[in]  aViewSCN     View SCN
 * @param[in]  aSchemaName  Schema Name (null 인 경우 PUBLIC)
 * @param[in]  aTableName   Table Name
 * @param[in]  aNA          N/A (ignored)
 * @param[out] aTableHandle Table Handle
 * @param[out] aExist       존재 여부
 * @param[in]  aEnv         Envirionment
 * @remarks
 */
stlStatus elgGetBaseTableHandleDumpCallback( stlInt64   aTransID,
                                             stlInt64   aViewSCN,
                                             stlChar  * aSchemaName,
                                             stlChar  * aTableName,
                                             stlChar  * aNA,
                                             void    ** aTableHandle,
                                             stlBool  * aExist,
                                             knlEnv   * aEnv )
{
    smlTransId     sTransID = SML_INVALID_TRANSID;
    smlScn         sViewSCN = SML_INFINITE_SCN;

    stlBool        sValidObject = STL_FALSE;

    ellDictHandle * sAuthHandle = NULL;
    
    ellDictHandle  sSchemaHandle;
    ellDictHandle  sTableHandle;

    stlMemset(   & sSchemaHandle, 0x00, STL_SIZEOF(ellDictHandle) );
    stlMemset(   & sTableHandle, 0x00, STL_SIZEOF(ellDictHandle) );
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aTableName != NULL, KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableHandle != NULL, KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExist != NULL, KNL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */
    
    sTransID = (smlTransId) aTransID;
    sViewSCN = (smlScn) aViewSCN;

    sAuthHandle   = ellGetCurrentUserHandle( ELL_ENV(aEnv) );
    
    /**
     * Schema Dictionary Handle 을 획득
     */

    if ( aSchemaName == NULL )
    {
        STL_TRY( ellGetTableDictHandleWithAuth( sTransID,
                                                sViewSCN,
                                                sAuthHandle,
                                                aTableName,
                                                & sTableHandle,
                                                & sValidObject,
                                                ELL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( ellGetSchemaDictHandle( sTransID,
                                         sViewSCN,
                                         aSchemaName,
                                         & sSchemaHandle,
                                         & sValidObject,
                                         ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        STL_TRY_THROW( sValidObject == STL_TRUE, RAMP_FINISH );

        STL_TRY( ellGetTableDictHandleWithSchema( sTransID,
                                                  sViewSCN,
                                                  & sSchemaHandle,
                                                  aTableName,
                                                  & sTableHandle,
                                                  & sValidObject,
                                                  ELL_ENV(aEnv) )
                 == STL_SUCCESS );
    }

    STL_RAMP( RAMP_FINISH );
    
    /**
     * Table Handle 을 설정
     */
    
    if ( sValidObject == STL_TRUE )
    {
        if ( ellGetTableType( & sTableHandle ) == ELL_TABLE_TYPE_BASE_TABLE )
        {
            *aTableHandle = ellGetTableHandle( & sTableHandle );
        }
        else
        {
            sValidObject = STL_FALSE;
            *aTableHandle = NULL;
        }
    }
    else
    {
        *aTableHandle = NULL;
    }
       
    /**
     * Output 설정
     */

    *aExist = sValidObject;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief Index Handle 을 얻기 위한 Dump Name Helper Callback
 * @param[in]  aTransID     Transaction ID
 * @param[in]  aViewSCN     View SCN
 * @param[in]  aSchemaName  Schema Name (null 인 경우 PUBLIC)
 * @param[in]  aIndexName   Index Name
 * @param[in]  aNA          N/A (ignored)
 * @param[out] aIndexHandle Index Handle
 * @param[out] aExist       존재 여부
 * @param[in]  aEnv         Envirionment
 * @remarks
 */
stlStatus elgGetIndexHandleDumpCallback( stlInt64   aTransID,
                                         stlInt64   aViewSCN,
                                         stlChar  * aSchemaName,
                                         stlChar  * aIndexName,
                                         stlChar  * aNA,
                                         void    ** aIndexHandle,
                                         stlBool  * aExist,
                                         knlEnv   * aEnv )
{
    smlTransId     sTransID = SML_INVALID_TRANSID;
    smlScn         sViewSCN = SML_INFINITE_SCN;

    stlBool        sValidObject = STL_FALSE;

    ellDictHandle * sAuthHandle = NULL;
    
    ellDictHandle  sSchemaHandle;
    ellDictHandle  sIndexHandle;
    
    stlMemset(   & sSchemaHandle, 0x00, STL_SIZEOF(ellDictHandle) );
    stlMemset(   & sIndexHandle, 0x00, STL_SIZEOF(ellDictHandle) );
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aIndexName != NULL, KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIndexHandle != NULL, KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExist != NULL, KNL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */
    
    sTransID = (smlTransId) aTransID;
    sViewSCN = (smlScn) aViewSCN;

    sAuthHandle   = ellGetCurrentUserHandle( ELL_ENV(aEnv) );
    
    /**
     * Schema Dictionary Handle 을 획득
     */

    if ( aSchemaName == NULL )
    {
        STL_TRY( ellGetIndexDictHandleWithAuth( sTransID,
                                                sViewSCN,
                                                sAuthHandle,
                                                aIndexName,
                                                & sIndexHandle,
                                                & sValidObject,
                                                ELL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( ellGetSchemaDictHandle( sTransID,
                                         sViewSCN,
                                         aSchemaName,
                                         & sSchemaHandle,
                                         & sValidObject,
                                         ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        STL_TRY_THROW( sValidObject == STL_TRUE, RAMP_FINISH );
        
        STL_TRY( ellGetIndexDictHandleWithSchema( sTransID,
                                                  sViewSCN,
                                                  & sSchemaHandle,
                                                  aIndexName,
                                                  & sIndexHandle,
                                                  & sValidObject,
                                                  ELL_ENV(aEnv) )
                 == STL_SUCCESS );
    }

    STL_RAMP( RAMP_FINISH );
    
    /**
     * Index Handle 을 설정
     */
    
    if ( sValidObject == STL_TRUE )
    {
        *aIndexHandle = ellGetIndexHandle( & sIndexHandle );
    }
    else
    {
        *aIndexHandle = NULL;
    }
       
    /**
     * Output 설정
     */

    *aExist = sValidObject;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Sequence Handle 을 얻기 위한 Dump Name Helper Callback
 * @param[in]  aTransID     Transaction ID
 * @param[in]  aViewSCN     View SCN
 * @param[in]  aSchemaName  Schema Name (null 인 경우 PUBLIC)
 * @param[in]  aSeqName     Sequence Name
 * @param[in]  aNA          N/A (ignored)
 * @param[out] aSeqHandle   Sequence Handle
 * @param[out] aExist       존재 여부
 * @param[in]  aEnv         Envirionment
 * @remarks
 */
stlStatus elgGetSequenceHandleDumpCallback( stlInt64   aTransID,
                                            stlInt64   aViewSCN,
                                            stlChar  * aSchemaName,
                                            stlChar  * aSeqName,
                                            stlChar  * aNA,
                                            void    ** aSeqHandle,
                                            stlBool  * aExist,
                                            knlEnv   * aEnv )
{
    smlTransId     sTransID = SML_INVALID_TRANSID;
    smlScn         sViewSCN = SML_INFINITE_SCN;

    stlBool        sValidObject = STL_FALSE;

    ellDictHandle * sAuthHandle = NULL;
    
    ellDictHandle  sSchemaHandle;
    ellDictHandle  sSeqHandle;
    
    stlMemset(   & sSchemaHandle, 0x00, STL_SIZEOF(ellDictHandle) );
    stlMemset(   & sSeqHandle, 0x00, STL_SIZEOF(ellDictHandle) );
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aSeqName != NULL, KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSeqHandle != NULL, KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExist != NULL, KNL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */
    
    sTransID = (smlTransId) aTransID;
    sViewSCN = (smlScn) aViewSCN;

    sAuthHandle   = ellGetCurrentUserHandle( ELL_ENV(aEnv) );
    
    /**
     * Schema Dictionary Handle 을 획득
     */

    if ( aSchemaName == NULL )
    {
        STL_TRY( ellGetSequenceDictHandleWithAuth( sTransID,
                                                   sViewSCN,
                                                   sAuthHandle,
                                                   aSeqName,
                                                   & sSeqHandle,
                                                   & sValidObject,
                                                   ELL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( ellGetSchemaDictHandle( sTransID,
                                         sViewSCN,
                                         aSchemaName,
                                         & sSchemaHandle,
                                         & sValidObject,
                                         ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        STL_TRY_THROW( sValidObject == STL_TRUE, RAMP_FINISH );
        
        STL_TRY( ellGetSequenceDictHandleWithSchema( sTransID,
                                                     sViewSCN,
                                                     & sSchemaHandle,
                                                     aSeqName,
                                                     & sSeqHandle,
                                                     & sValidObject,
                                                     ELL_ENV(aEnv) )
                 == STL_SUCCESS );
    }

    STL_RAMP( RAMP_FINISH );

    /**
     * Index Dictionary Handle 을 획득
     */

    if( sValidObject == STL_TRUE )
    {
        *aSeqHandle = ellGetSequenceHandle( & sSeqHandle );
    }
    else
    {
        *aSeqHandle = NULL;
    }
       
    /**
     * Output 설정
     */

    *aExist = sValidObject;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Tablespace ID 를 얻기 위한 Dump Name Helper Callback
 * @param[in]  aTransID     Transaction ID
 * @param[in]  aViewSCN     View SCN
 * @param[in]  aNA          N/A (ignored)
 * @param[in]  aSpaceName   Space Name
 * @param[in]  aNA2         N/A (ignored)
 * @param[out] aSpaceID     Tablespace ID  ( stlInt64 * 를 넘기기로 약속 )
 * @param[out] aExist       존재 여부
 * @param[in]  aEnv         Envirionment
 * @remarks
 */
stlStatus elgGetSpaceHandleDumpCallback( stlInt64   aTransID,
                                         stlInt64   aViewSCN,
                                         stlChar  * aNA,
                                         stlChar  * aSpaceName,
                                         stlChar  * aNA2,
                                         void    ** aSpaceID,
                                         stlBool  * aExist,
                                         knlEnv   * aEnv )
{
    smlTransId     sTransID = SML_INVALID_TRANSID;
    smlScn         sViewSCN = SML_INFINITE_SCN;

    stlBool        sValidObject = STL_FALSE;

    stlInt64       sSpaceID;
    ellDictHandle  sSpaceHandle;
    stlMemset(   & sSpaceHandle, 0x00, STL_SIZEOF(ellDictHandle) );
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aSpaceName != NULL, KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSpaceID != NULL, KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExist != NULL, KNL_ERROR_STACK(aEnv) );

    sTransID = (smlTransId) aTransID;
    sViewSCN = (smlScn) aViewSCN;

    /**
     * Tablespace Dictionary Handle 획득
     */

    STL_TRY( ellGetTablespaceDictHandle( sTransID,
                                         sViewSCN,
                                         aSpaceName,
                                         & sSpaceHandle,
                                         & sValidObject,
                                         ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Tablespace ID 설정
     */

    if ( sValidObject == STL_TRUE )
    {
        sSpaceID = ellGetTablespaceID( & sSpaceHandle );
    }
    else
    {
        sSpaceID = ELL_DICT_TABLESPACE_ID_NA;
    }

    /**
     * Output 설정
     */

    stlMemcpy( aSpaceID,
               & sSpaceID,
               STL_SIZEOF(stlInt64) );
    
    *aExist = sValidObject;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/** @} elgFixedTable */









/**
 * @addtogroup elgD_OBJECT_CACHE
 * @internal
 * @{
 */

/**
 * @brief D$OBJECT_CACHE 를 위한 컬럼 정의
 * @remarks
 */

knlFxColumnDesc gElgDumpObjectCacheColDesc[] =
{
    {
        "BUCKET_NO",
        "hash bucket number",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( elgDumpObjectCache, mBucketNO ),
        STL_SIZEOF(stlInt32),
        STL_FALSE  /* nullable */
    },
    {
        "ELEMENT_NO",
        "hash element number in a hash bucket",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( elgDumpObjectCache, mElementNO ),
        STL_SIZEOF(stlInt32),
        STL_FALSE  /* nullable */
    },
    {
        "ELEMENT_HASH_VALUE",
        "hash value of the hash element",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( elgDumpObjectCache, mElementHashValue ),
        STL_SIZEOF(stlInt64),
        STL_FALSE  /* nullable */
    },
    {
        "ELEMENT_CREATE_TRANS_ID",
        "create transaction ID of the hash element",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( elgDumpObjectCache, mElementCreateTransID ),
        STL_SIZEOF(smlTransId),
        STL_FALSE  /* nullable */
    },
    {
        "ELEMENT_CREATE_COMMIT_SCN",
        "create commit SCN of the hash element",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( elgDumpObjectCache, mElementCreateCommitSCN ),
        STL_SIZEOF(smlScn),
        STL_FALSE  /* nullable */
    },
    {
        "ELEMENT_DROP_TRANS_ID",
        "drop transaction ID of the hash element",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( elgDumpObjectCache, mElementDropTransID ),
        STL_SIZEOF(smlTransId),
        STL_FALSE  /* nullable */
    },
    {
        "ELEMENT_DROP_COMMIT_SCN",
        "drop commit SCN of the hash element",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( elgDumpObjectCache, mElementDropCommitSCN ),
        STL_SIZEOF(smlScn),
        STL_FALSE  /* nullable */
    },
    {
        "ELEMENT_DROP_FLAG",
        "drop flag of the hash element",
        KNL_FXTABLE_DATATYPE_BOOL,
        STL_OFFSETOF( elgDumpObjectCache, mElementDropFlag ),
        STL_SIZEOF(stlBool),
        STL_FALSE  /* nullable */
    },
    {
        "ELEMENT_MODIFYING_CNT",
        "modifying count of the hash element",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( elgDumpObjectCache, mElementModifyingCnt ),
        STL_SIZEOF(stlInt64),
        STL_FALSE  /* nullable */
    },
    {
        "ELEMENT_TRY_MODIFY_CNT",
        "global modifying count of the hash element",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( elgDumpObjectCache, mElementTryModifyCnt ),
        STL_SIZEOF(stlInt64),
        STL_FALSE  /* nullable */
    },
    {
        "ELEMENT_MEM_USE_CNT",
        "memory use count of the hash element",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( elgDumpObjectCache, mElementMemUseCnt ),
        STL_SIZEOF(stlInt64),
        STL_FALSE  /* nullable */
    },
    {
        "HASH_DATA",
        "hash data of the hash element",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( elgDumpObjectCache, mBufferHashData ),
        ELDC_DUMP_PRINT_BUFFER_SIZE,
        STL_FALSE  /* nullable */
    },
    {
        "OBJECT_DESC",
        "object descriptor",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( elgDumpObjectCache, mBufferObjectDesc ),
        ELDC_DUMP_PRINT_BUFFER_SIZE,
        STL_TRUE  /* nullable */
    },
    {
        NULL,
        NULL,
        0,
        0,
        0,
        STL_FALSE
    }
};



/**
 * @brief Dump Option String 에 대응하는 Dump Object Handle 을 리턴
 * @param[in]    aTransID        N/A
 * @param[in]    aStmt           N/A
 * @param[out]   aDumpObjHandle  Dump Object Handle
 * @param[in]    aDumpOption     Dump Option String
 * @param[in]    aKnlEnv         Kernel Environment
 * @remarks
 */
stlStatus elgGetHandleDumpObjectCacheCallback( stlInt64   aTransID,
                                               void     * aStmt,
                                               stlChar  * aDumpOption,
                                               void    ** aDumpObjHandle,
                                               knlEnv   * aKnlEnv )
{
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDumpObjHandle != NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( aDumpOption != NULL, KNL_ERROR_STACK(aKnlEnv) );

    /**
     * Dump Object 에 대응하는 Dump Object Handle을 설정
     */

    if ( stlStrcmp( aDumpOption, "COLUMN_ID" ) == 0 )
    {
        *aDumpObjHandle = & gEldcObjectDump[ELDC_OBJECTCACHE_COLUMN_ID];
    }
    else if ( stlStrcmp( aDumpOption, "COLUMN_NAME" ) == 0 )
    {
        *aDumpObjHandle = & gEldcObjectDump[ELDC_OBJECTCACHE_COLUMN_NAME];
    }
    else if ( stlStrcmp( aDumpOption, "INDEX_ID" ) == 0 )
    {
        *aDumpObjHandle = & gEldcObjectDump[ELDC_OBJECTCACHE_INDEX_ID];
    }
    else if ( stlStrcmp( aDumpOption, "INDEX_NAME" ) == 0 )
    {
        *aDumpObjHandle = & gEldcObjectDump[ELDC_OBJECTCACHE_INDEX_NAME];
    }
    else if ( stlStrcmp( aDumpOption, "CONSTRAINT_ID" ) == 0 )
    {
        *aDumpObjHandle = & gEldcObjectDump[ELDC_OBJECTCACHE_CONSTRAINT_ID];
    }
    else if ( stlStrcmp( aDumpOption, "CONSTRAINT_NAME" ) == 0 )
    {
        *aDumpObjHandle = & gEldcObjectDump[ELDC_OBJECTCACHE_CONSTRAINT_NAME];
    }
    else if ( stlStrcmp( aDumpOption, "TABLE_ID" ) == 0 )
    {
        *aDumpObjHandle = & gEldcObjectDump[ELDC_OBJECTCACHE_TABLE_ID];
    }
    else if ( stlStrcmp( aDumpOption, "TABLE_NAME" ) == 0 )
    {
        *aDumpObjHandle = & gEldcObjectDump[ELDC_OBJECTCACHE_TABLE_NAME];
    }
    else if ( stlStrcmp( aDumpOption, "SEQUENCE_ID" ) == 0 )
    {
        *aDumpObjHandle = & gEldcObjectDump[ELDC_OBJECTCACHE_SEQUENCE_ID];
    }
    else if ( stlStrcmp( aDumpOption, "SEQUENCE_NAME" ) == 0 )
    {
        *aDumpObjHandle = & gEldcObjectDump[ELDC_OBJECTCACHE_SEQUENCE_NAME];
    }
    else if ( stlStrcmp( aDumpOption, "SCHEMA_ID" ) == 0 )
    {
        *aDumpObjHandle = & gEldcObjectDump[ELDC_OBJECTCACHE_SCHEMA_ID];
    }
    else if ( stlStrcmp( aDumpOption, "SCHEMA_NAME" ) == 0 )
    {
        *aDumpObjHandle = & gEldcObjectDump[ELDC_OBJECTCACHE_SCHEMA_NAME];
    }
    else if ( stlStrcmp( aDumpOption, "TABLESPACE_ID" ) == 0 )
    {
        *aDumpObjHandle = & gEldcObjectDump[ELDC_OBJECTCACHE_SPACE_ID];
    }
    else if ( stlStrcmp( aDumpOption, "TABLESPACE_NAME" ) == 0 )
    {
        *aDumpObjHandle = & gEldcObjectDump[ELDC_OBJECTCACHE_SPACE_NAME];
    }
    else if ( stlStrcmp( aDumpOption, "AUTHORIZATION_ID" ) == 0 )
    {
        *aDumpObjHandle = & gEldcObjectDump[ELDC_OBJECTCACHE_AUTH_ID];
    }
    else if ( stlStrcmp( aDumpOption, "AUTHORIZATION_NAME" ) == 0 )
    {
        *aDumpObjHandle = & gEldcObjectDump[ELDC_OBJECTCACHE_AUTH_NAME];
    }
    else if ( stlStrcmp( aDumpOption, "CATALOG_ID" ) == 0 )
    {
        *aDumpObjHandle = & gEldcObjectDump[ELDC_OBJECTCACHE_CATALOG_ID];
    }
    else if ( stlStrcmp( aDumpOption, "CATALOG_NAME" ) == 0 )
    {
        *aDumpObjHandle = & gEldcObjectDump[ELDC_OBJECTCACHE_CATALOG_NAME];
    }
    else if ( stlStrcmp( aDumpOption, "SYNONYM_ID" ) == 0 )
    {
        *aDumpObjHandle = & gEldcObjectDump[ELDC_OBJECTCACHE_SYNONYM_ID];
    }
    else if ( stlStrcmp( aDumpOption, "SYNONYM_NAME" ) == 0 )
    {
        *aDumpObjHandle = & gEldcObjectDump[ELDC_OBJECTCACHE_SYNONYM_NAME];
    }
    else if ( stlStrcmp( aDumpOption, "PUBLIC_SYNONYM_ID" ) == 0 )
    {
        *aDumpObjHandle = & gEldcObjectDump[ELDC_OBJECTCACHE_PUBLIC_SYNONYM_ID];
    }
    else if ( stlStrcmp( aDumpOption, "PUBLIC_SYNONYM_NAME" ) == 0 )
    {
        *aDumpObjHandle = & gEldcObjectDump[ELDC_OBJECTCACHE_PUBLIC_SYNONYM_NAME];
    }
    else if ( stlStrcmp( aDumpOption, "PROFILE_ID" ) == 0 )
    {
        *aDumpObjHandle = & gEldcObjectDump[ELDC_OBJECTCACHE_PROFILE_ID];
    }
    else if ( stlStrcmp( aDumpOption, "PROFILE_NAME" ) == 0 )
    {
        *aDumpObjHandle = & gEldcObjectDump[ELDC_OBJECTCACHE_PROFILE_NAME];
    }
    else
    {
        /**
         * 해당하는 Dump Object 가 없음 
         */
        
        *aDumpObjHandle = NULL;
    }
                  
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief D$OBJECT_CACHE 를 위한 Open Callback 함수 정의
 * @param[in]  aStmt            Statement Pointer
 * @param[in]  aDumpObjHandle   Dump Object Handle
 * @param[in]  aPathCtrl        Path Controller
 * @param[in]  aKnlEnv          Kernel Envrionment
 * @remarks
 */
stlStatus elgOpenDumpObjectCacheCallback( void   * aStmt,
                                          void   * aDumpObjHandle,
                                          void   * aPathCtrl,
                                          knlEnv * aKnlEnv )
{
    eldcDictDump               * sDictDump = NULL;
    elgDumpObjectCachePathCtrl * sPathCtrl = NULL;

    /*
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aDumpObjHandle != NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( aPathCtrl != NULL, KNL_ERROR_STACK(aKnlEnv) );

    /**
     * 지역 변수 설정 
     */

    sDictDump = (eldcDictDump *) aDumpObjHandle;
    sPathCtrl = (elgDumpObjectCachePathCtrl *) aPathCtrl;

    /**
     * Path Controller 초기화 
     */

    stlMemset( sPathCtrl, 0x00, STL_SIZEOF(elgDumpObjectCachePathCtrl) );
    
    STL_TRY( eldcGetFirstHashElement( sDictDump->mDictHash,
                                      & sPathCtrl->mBucketNO,
                                      & sPathCtrl->mHashElement,
                                      ELL_ENV(aKnlEnv) )
             == STL_SUCCESS );

    sPathCtrl->mElementNO = 1;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief D$OBJECT_CACHE 를 위한 Close Callback 함수 정의
 * @param[in]  aDumpObjHandle   Dump Object Handle
 * @param[in]  aPathCtrl        Path Controller
 * @param[in]  aKnlEnv          Kernel Envrionment
 * @remarks
 */
stlStatus elgCloseDumpObjectCacheCallback( void   * aDumpObjHandle,
                                           void   * aPathCtrl,
                                           knlEnv * aKnlEnv )
{
    /**< 할 일 없음 */
    return STL_SUCCESS;
}


/**
 * @brief D$OBJECT_CACHE 를 위한 Build Record Callback 함수 정의
 * @param[in]  aDumpObjHandle   Dump Object Handle
 * @param[in]  aPathCtrl        Path Controller
 * @param[in]  aValueList       Data Value List
 * @param[in]  aBlockIdx        Block Idx
 * @param[out] aIsExist         레코드 존재 여부 
 * @param[in]  aKnlEnv          Kernel Envrionment
 * @remarks
 */
stlStatus elgBuildRecordDumpObjectCacheCallback( void              * aDumpObjHandle,
                                                 void              * aPathCtrl,
                                                 knlValueBlockList * aValueList,
                                                 stlInt32            aBlockIdx,
                                                 stlBool           * aIsExist,
                                                 knlEnv            * aKnlEnv )
{
    stlUInt32                   sOldBucketNO = 0;

    stlBool   sTimeout      = STL_FALSE;
    stlBool   sBucketLatch  = STL_FALSE;
    stlBool   sElementLatch = STL_FALSE;
    
    eldcDictDump               * sDictDump = NULL;
    elgDumpObjectCachePathCtrl * sPathCtrl = NULL;

    eldcHashElement            * sHashElement = NULL;

    elgDumpObjectCache           sDumpRecord;
    stlMemset( &sDumpRecord, 0x00, STL_SIZEOF(elgDumpObjectCache) );

    /*
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aDumpObjHandle != NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( aPathCtrl != NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( aIsExist != NULL, KNL_ERROR_STACK(aKnlEnv) );
    

    /**
     * 지역 변수 설정 
     */

    sDictDump = (eldcDictDump *) aDumpObjHandle;
    sPathCtrl = (elgDumpObjectCachePathCtrl *) aPathCtrl;

    /**
     * D$OBJECT_CACHE 를 위한 하나의 레코드를 생성 
     */
    
    if ( sPathCtrl->mHashElement == NULL )
    {
        /**
         * 종료 지점일 경우 구성할 레코드가 없음을 알림
         */
        
        *aIsExist = STL_FALSE;
    }
    else
    {
        /**
         * DDL 의 간섭이 발생하지 않도록 try latch
         */
        
        STL_TRY( knlAcquireLatch( & sDictDump->mDictHash->mBucket[sPathCtrl->mBucketNO].mInsDelLatch,
                                  KNL_LATCH_MODE_EXCLUSIVE,
                                  KNL_LATCH_PRIORITY_NORMAL,
                                  0,   /* interval */
                                  & sTimeout,
                                  aKnlEnv )
                 == STL_SUCCESS );
        if( sTimeout == STL_TRUE )
        {
            *aIsExist = STL_FALSE;
            
            sBucketLatch = STL_FALSE;
            STL_THROW( RAMP_FINISH );
        }
        else
        {
            sBucketLatch = STL_TRUE;
        }
        
        STL_TRY( knlAcquireLatch( & sPathCtrl->mHashElement->mWriteLatch,
                                  KNL_LATCH_MODE_EXCLUSIVE,
                                  KNL_LATCH_PRIORITY_NORMAL,
                                  0,   /* interval */
                                  & sTimeout,
                                  aKnlEnv )
                 == STL_SUCCESS );
        if( sTimeout == STL_TRUE )
        {
            *aIsExist = STL_FALSE;
            
            sElementLatch = STL_FALSE;
            STL_THROW( RAMP_FINISH );
        }
        else
        {
            sElementLatch = STL_TRUE;
        }
        
        /**
         * 유효한 정보가 있으면 레코드를 위한 정보를 구성
         */

        sHashElement = sPathCtrl->mHashElement;

        /**
         * Bucket NO
         */
        
        sDumpRecord.mBucketNO = sPathCtrl->mBucketNO;
        
        /**
         * Hash Element 정보를 구성
         * - 필요한 정보임 
         */
        
        sDumpRecord.mBucketNO          = sPathCtrl->mBucketNO;
        
        sDumpRecord.mElementNO         = sPathCtrl->mElementNO;
        sDumpRecord.mElementHashValue  = (stlInt64) sHashElement->mHashValue;
        
        sDumpRecord.mElementCreateTransID   = sHashElement->mVersionInfo.mCreateTransID;
        sDumpRecord.mElementCreateCommitSCN = sHashElement->mVersionInfo.mCreateCommitSCN;
        sDumpRecord.mElementDropTransID     = sHashElement->mVersionInfo.mDropTransID;
        sDumpRecord.mElementDropCommitSCN   = sHashElement->mVersionInfo.mDropCommitSCN;
        sDumpRecord.mElementDropFlag        = sHashElement->mVersionInfo.mDropFlag;
        sDumpRecord.mElementModifyingCnt    = sHashElement->mVersionInfo.mModifyingCnt;
        sDumpRecord.mElementTryModifyCnt    = sHashElement->mVersionInfo.mTryModifyCnt;
        sDumpRecord.mElementMemUseCnt       = sHashElement->mVersionInfo.mMemUseCnt;
        
        sDictDump->mPrintHashData( sHashElement->mHashData,
                                   sDumpRecord.mBufferHashData );
            
        
        if ( sHashElement->mObjectDesc != NULL )
        {
            sDictDump->mPrintObjectDesc( sHashElement->mObjectDesc,
                                         sDumpRecord.mBufferObjectDesc );
        }

        /**
         * Value List를 생성
         */
        STL_TRY( knlBuildFxRecord( gElgDumpObjectCacheColDesc,
                                   & sDumpRecord,
                                   aValueList,
                                   aBlockIdx,
                                   aKnlEnv )
                 == STL_SUCCESS );

        *aIsExist = STL_TRUE;

        /**
         * Latch 해제
         */
        
        if ( sBucketLatch == STL_TRUE )
        {
            sBucketLatch = STL_FALSE;
            STL_TRY( knlReleaseLatch( & sDictDump->mDictHash->mBucket[sPathCtrl->mBucketNO].mInsDelLatch,
                                      aKnlEnv )
                     == STL_SUCCESS );
        }
        
        if ( sElementLatch == STL_TRUE )
        {
            sElementLatch = STL_FALSE;
            STL_TRY( knlReleaseLatch( & sPathCtrl->mHashElement->mWriteLatch, aKnlEnv ) == STL_SUCCESS );
        }
        
        /**
         * 다음 레코드를 위한 정보 설정
         */

        while ( 1 )
        {
            /**
             * 다음 Hash Element 가 있는지 확인
             */

            sOldBucketNO = sPathCtrl->mBucketNO;
            
            STL_TRY( eldcGetNextHashElement( sDictDump->mDictHash,
                                             & sPathCtrl->mBucketNO,
                                             & sPathCtrl->mHashElement,
                                             ELL_ENV(aKnlEnv) )
                     == STL_SUCCESS );

            if ( sPathCtrl->mBucketNO == sOldBucketNO )
            {
                /* Bucket 이 변경되지 않았음 */
                sPathCtrl->mElementNO++;
            }
            else
            {
                /* Bucket 이 변경됨 */
                sPathCtrl->mElementNO = 1;
            }
            
            break;
        }
    }

    STL_RAMP( RAMP_FINISH );

    /**
     * Latch 해제
     */
    
    if ( sBucketLatch == STL_TRUE )
    {
        sBucketLatch = STL_FALSE;
        STL_TRY( knlReleaseLatch( & sDictDump->mDictHash->mBucket[sPathCtrl->mBucketNO].mInsDelLatch,
                                  aKnlEnv )
                 == STL_SUCCESS );
    }

    if ( sElementLatch == STL_TRUE )
    {
        sElementLatch = STL_FALSE;
        STL_TRY( knlReleaseLatch( & sPathCtrl->mHashElement->mWriteLatch, aKnlEnv ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    if ( sBucketLatch == STL_TRUE )
    {
        (void) knlReleaseLatch( & sDictDump->mDictHash->mBucket[sPathCtrl->mBucketNO].mInsDelLatch, aKnlEnv );
        sBucketLatch = STL_FALSE;
    }

    if ( sElementLatch == STL_TRUE )
    {
        (void) knlReleaseLatch( & sPathCtrl->mHashElement->mWriteLatch, aKnlEnv );
        sElementLatch = STL_FALSE;
    }
    
    return STL_FAILURE;
}



/**
 * @brief D$OBJECT_CACHE 를 위한 테이블 정의
 * @remarks
 */

knlFxTableDesc gDumpObjectCacheTabDesc =
{
    KNL_FXTABLE_USAGE_DUMP_TABLE,         /**< Dump Table */
    KNL_STARTUP_PHASE_OPEN,               /**< OPEN 단계부터 조회 가능 */
    elgGetHandleDumpObjectCacheCallback,  /**< D$OBJECT_CACHE Dump Object Handle 획득 함수 */
    elgOpenDumpObjectCacheCallback,        /**< D$OBJECT_CACHE Open Callback */
    elgCloseDumpObjectCacheCallback,       /**< D$OBJECT_CACHE Close Callback */
    elgBuildRecordDumpObjectCacheCallback, /**< D$OBJECT_CACHE BuildRecord Callback */
    STL_SIZEOF(elgDumpObjectCachePathCtrl),/**< D$OBJECT_CACHE Path Controller Size */
    "D$OBJECT_CACHE",                      /**< Table Name */
    "dump table for SQL-object cache",    /**< Table Comment */
    gElgDumpObjectCacheColDesc             /**< D$OBJECT_CACHE 의 컬럼 정보 */
};

/** @} elgD_OBJECT_CACHE */



/**
 * @addtogroup elgD_PRIV_CACHE
 * @internal
 * @{
 */


/**
 * @brief Dump Option String 에 대응하는 Dump Object Handle 을 리턴
 * @param[in]    aTransID        N/A
 * @param[in]    aStmt           N/A
 * @param[in]    aDumpOption     Dump Option String
 * @param[out]   aDumpObjHandle  Dump Object Handle
 * @param[in]    aKnlEnv         Kernel Environment
 * @remarks
 */
stlStatus elgGetHandleDumpPrivCacheCallback( stlInt64   aTransID,
                                             void     * aStmt,
                                             stlChar  * aDumpOption,
                                             void    ** aDumpObjHandle,
                                             knlEnv   * aKnlEnv )
{
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDumpObjHandle != NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( aDumpOption != NULL, KNL_ERROR_STACK(aKnlEnv) );

    /**
     * Dump Object 에 대응하는 Dump Object Handle을 설정
     */

    if ( stlStrcmp( aDumpOption, "DATABASE" ) == 0 )
    {
        *aDumpObjHandle = & gEldcPrivDump[ELDC_PRIVCACHE_DATABASE];
    }
    else if ( stlStrcmp( aDumpOption, "TABLESPACE" ) == 0 )
    {
        *aDumpObjHandle = & gEldcPrivDump[ELDC_PRIVCACHE_SPACE];
    }
    else if ( stlStrcmp( aDumpOption, "SCHEMA" ) == 0 )
    {
        *aDumpObjHandle = & gEldcPrivDump[ELDC_PRIVCACHE_SCHEMA];
    }
    else if ( stlStrcmp( aDumpOption, "TABLE" ) == 0 )
    {
        *aDumpObjHandle = & gEldcPrivDump[ELDC_PRIVCACHE_TABLE];
    }
    else if ( stlStrcmp( aDumpOption, "USAGE" ) == 0 )
    {
        *aDumpObjHandle = & gEldcPrivDump[ELDC_PRIVCACHE_USAGE];
    }
    else if ( stlStrcmp( aDumpOption, "COLUMN" ) == 0 )
    {
        *aDumpObjHandle = & gEldcPrivDump[ELDC_PRIVCACHE_COLUMN];
    }
    else
    {
        /**
         * 해당하는 Dump Object 가 없음 
         */

        *aDumpObjHandle = NULL;
    }
                  
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief D$PRIV_CACHE 를 위한 테이블 정의
 * @remarks
 * Dump Handle 을 얻는 Callback 을 제외하고 D$OBJECT_CACHE 와 구조가 동일하다.
 */

knlFxTableDesc gDumpPrivCacheTabDesc =
{
    KNL_FXTABLE_USAGE_DUMP_TABLE,          /**< Dump Table */
    KNL_STARTUP_PHASE_OPEN,                /**< OPEN 단계부터 조회 가능 */
    elgGetHandleDumpPrivCacheCallback,     /**< D$PRIV_CACHE Dump Object Handle 획득 함수 */
    elgOpenDumpObjectCacheCallback,        /**< D$PRIV_CACHE Open Callback */
    elgCloseDumpObjectCacheCallback,       /**< D$PRIV_CACHE Close Callback */
    elgBuildRecordDumpObjectCacheCallback, /**< D$PRIV_CACHE BuildRecord Callback */
    STL_SIZEOF(elgDumpObjectCachePathCtrl),/**< D$PRIV_CACHE Path Controller Size */
    "D$PRIV_CACHE",                        /**< Table Name */
    "dump table for privilege cache",      /**< Table Comment */
    gElgDumpObjectCacheColDesc             /**< D$PRIV_CACHE 의 컬럼 정보 */
};

/** @} elgD_PRIV_CACHE */



/**
 * @addtogroup elgD_PRODUCT
 * @internal
 * @{
 */


/**
 * @brief D$PRODUCT 를 위한 컬럼 정의
 * @remarks
 */

knlFxColumnDesc gElgDumpProductColDesc[] =
{
    {
        "NAME",
        "name",
        KNL_FXTABLE_DATATYPE_VARCHAR_POINTER,
        STL_OFFSETOF( elgDumpProduct, mName ),
        ELG_PRODUCT_NAME_LEN,
        STL_TRUE  /* nullable */
    },
    {
        "COMMENTS",
        "comments",
        KNL_FXTABLE_DATATYPE_VARCHAR_POINTER,
        STL_OFFSETOF( elgDumpProduct, mComment ),
        ELG_PRODUCT_COMMENTS_LEN,
        STL_TRUE  /* nullable */
    },
    {
        NULL,
        NULL,
        0,
        0,
        0,
        STL_FALSE
    }
};


elgDumpProduct gElgDeveloper[] =
{
    { "이경모", "2010-08-09 Execution Library Layer" },
    { "이환재", "2010-08-09 Index, Heap Relation" },
    { "김명근", "2010-08-26 Storage Manager Layer" },
    { "박정민", "2010-09-01 Client Server Layer" },
    { "온경오", "2010-11-15 Recovery Management" },
    { "김희택", "2010-11-15 Standard, Kernel Layer" },
    { "박은희", "2010-12-01 Data Type Layer" },
    { "김정현", "2011-03-07 SQL Processor Layer" },
    { "이현철", "2011-03-14 Repliaction" },
    { "이용문", "2011-04-25 Data Type Function" },
    { "유병섭", "2011-11-14 SQL Processor Layer" },
    { "손우상", "2012-06-01 Storage Manager Layer, JDBC" },
    { NULL, NULL }
};




/**
 * @brief Dump Option String 에 대응하는 Dump Object Handle 을 리턴
 * @param[in]    aTransID        N/A
 * @param[in]    aStmt           N/A
 * @param[in]    aDumpOption     Dump Option String
 * @param[out]   aDumpObjHandle  Dump Object Handle
 * @param[in]    aKnlEnv         Kernel Environment
 * @remarks
 */
stlStatus elgGetHandleDumpProductCallback( stlInt64   aTransID,
                                           void     * aStmt,
                                           stlChar  * aDumpOption,
                                           void    ** aDumpObjHandle,
                                           knlEnv   * aKnlEnv )
{
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDumpObjHandle != NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( aDumpOption != NULL, KNL_ERROR_STACK(aKnlEnv) );

    /**
     * Dump Object 에 대응하는 Dump Object Handle을 설정
     */
    
    if ( stlStrcmp( aDumpOption, "DEVELOPER" ) == 0 )
    {
        *aDumpObjHandle = (void *) & gElgDeveloper;
    }
    else
    {
        *aDumpObjHandle = NULL;
    }
                  
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief D$PRODUCT 를 위한 Open Callback 함수 정의
 * @param[in]  aStmt            Statement Pointer
 * @param[in]  aDumpObjHandle   Dump Object Handle
 * @param[in]  aPathCtrl        Path Controller
 * @param[in]  aKnlEnv          Kernel Envrionment
 * @remarks
 */
stlStatus elgOpenDumpProductCallback( void   * aStmt,
                                      void   * aDumpObjHandle,
                                      void   * aPathCtrl,
                                      knlEnv * aKnlEnv )
{
    elgDumpProductPathCtrl * sPathCtrl = NULL;

    /*
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aDumpObjHandle != NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( aPathCtrl != NULL, KNL_ERROR_STACK(aKnlEnv) );

    /**
     * 지역 변수 설정 
     */

    sPathCtrl = (elgDumpProductPathCtrl *) aPathCtrl;

    /**
     * Path Controller 초기화 
     */

    stlMemset( sPathCtrl, 0x00, STL_SIZEOF(elgDumpProductPathCtrl) );
    
    sPathCtrl->mNO = 0;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief D$PRODUCT 를 위한 Close Callback 함수 정의
 * @param[in]  aDumpObjHandle   Dump Object Handle
 * @param[in]  aPathCtrl        Path Controller
 * @param[in]  aKnlEnv          Kernel Envrionment
 * @remarks
 */
stlStatus elgCloseDumpProductCallback( void   * aDumpObjHandle,
                                       void   * aPathCtrl,
                                       knlEnv * aKnlEnv )
{
    /**< 할 일 없음 */
    return STL_SUCCESS;
}


/**
 * @brief D$PRODUCT 를 위한 Build Record Callback 함수 정의
 * @param[in]  aDumpObjHandle   Dump Object Handle
 * @param[in]  aPathCtrl        Path Controller
 * @param[in]  aValueList       Data Value List
 * @param[in]  aBlockIdx        Block Idx
 * @param[out] aIsExist         레코드 존재 여부 
 * @param[in]  aKnlEnv          Kernel Envrionment
 * @remarks
 */
stlStatus elgBuildRecordDumpProductCallback( void              * aDumpObjHandle,
                                             void              * aPathCtrl,
                                             knlValueBlockList * aValueList,
                                             stlInt32            aBlockIdx,
                                             stlBool           * aIsExist,
                                             knlEnv            * aKnlEnv )
{
    elgDumpProduct         * sDumpObj = NULL;
    elgDumpProductPathCtrl * sPathCtrl = NULL;
    
    elgDumpProduct           sDumpRecord;
    stlMemset( &sDumpRecord, 0x00, STL_SIZEOF(elgDumpProduct) );
    
    /*
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aDumpObjHandle != NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( aPathCtrl != NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( aIsExist != NULL, KNL_ERROR_STACK(aKnlEnv) );
    

    /**
     * 지역 변수 설정 
     */

    sDumpObj  = (elgDumpProduct *) aDumpObjHandle;
    sPathCtrl = (elgDumpProductPathCtrl *) aPathCtrl;
    

    /**
     * D$PRODUCT 를 위한 하나의 레코드를 생성 
     */
    
    if ( sDumpObj[sPathCtrl->mNO].mName == NULL )
    {
        /**
         * 종료 지점일 경우 구성할 레코드가 없음을 알림
         */
        
        *aIsExist = STL_FALSE;
    }
    else
    {
        /**
         * 유효한 정보가 있으면 레코드를 위한 정보를 구성
         */

        sDumpRecord.mName    = sDumpObj[sPathCtrl->mNO].mName;
        sDumpRecord.mComment = sDumpObj[sPathCtrl->mNO].mComment;
        

        /**
         * Value List를 생성
         */
        STL_TRY( knlBuildFxRecord( gElgDumpProductColDesc,
                                   & sDumpRecord,
                                   aValueList,
                                   aBlockIdx,
                                   aKnlEnv )
                 == STL_SUCCESS );

        *aIsExist = STL_TRUE;
        
        /**
         * 다음 레코드를 위한 정보 설정
         */

        sPathCtrl->mNO++;
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief D$PRODUCT 를 위한 테이블 정의
 * @remarks
 */

knlFxTableDesc gDumpProductTabDesc =
{
    KNL_FXTABLE_USAGE_DUMP_TABLE,      /**< Dump Table */
    KNL_STARTUP_PHASE_NO_MOUNT,        /**< NO_MOUNT 단계부터 조회 가능 */
    elgGetHandleDumpProductCallback,   /**< D$PRODUCT Dump Object Handle 획득 함수 */
    elgOpenDumpProductCallback,        /**< D$PRODUCT Open Callback */
    elgCloseDumpProductCallback,       /**< D$PRODUCT Close Callback */
    elgBuildRecordDumpProductCallback, /**< D$PRODUCT BuildRecord Callback */
    STL_SIZEOF(elgDumpProductPathCtrl),/**< D$PRODUCT Path Controller Size */
    "D$PRODUCT",                       /**< Table Name */
    "dump table for product information",  /**< Table Comment */
    gElgDumpProductColDesc             /**< D$PRODUCT 의 컬럼 정보 */
};

/** @} elgD_PRODUCT */




/**
 * @addtogroup elgX_BuiltIn_Function
 * @internal
 * @{
 */

/**
 * @brief X$BUILTIN_FUNCTION을 위한 컬럼 정의
 * @remarks
 */

knlFxColumnDesc gElgFixedBuiltInFunctionColDesc[] =
{
    {
        "FUNC_ID",
        "function identifier",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( elgFixedBuiltInFunction, mFuncID ),
        STL_SIZEOF(stlInt32),
        STL_FALSE  /* nullable */
    },
    {
        "FUNC_NAME",
        "function name",
        KNL_FXTABLE_DATATYPE_VARCHAR_POINTER,
        STL_OFFSETOF( elgFixedBuiltInFunction, mFuncName ),
        STL_MAX_SQL_IDENTIFIER_LENGTH,
        STL_FALSE  /* nullable */
    },
    {
        "MIN_ARG_COUNT",
        "minimum argument count",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( elgFixedBuiltInFunction, mMinArgCnt ),
        STL_SIZEOF( stlInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "MAX_ARG_COUNT",
        "maximum argument count",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( elgFixedBuiltInFunction, mMaxArgCnt ),
        STL_SIZEOF( stlInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "RETURN_VALUE_CLASS",
        "return value class",
        KNL_FXTABLE_DATATYPE_VARCHAR_POINTER,
        STL_OFFSETOF( elgFixedBuiltInFunction, mReturnValueClass ),
        STL_MAX_SQL_IDENTIFIER_LENGTH,
        STL_FALSE  /* nullable */
    },
    {
        "ITERATION_TIME",
        "iteration time",
        KNL_FXTABLE_DATATYPE_VARCHAR_POINTER,
        STL_OFFSETOF( elgFixedBuiltInFunction, mIterationTime ),
        STL_MAX_SQL_IDENTIFIER_LENGTH,
        STL_FALSE  /* nullable */
    },
    {
        "NEED_CAST_IN_GROUP",
        "need type casting in same group",
        KNL_FXTABLE_DATATYPE_BOOL,
        STL_OFFSETOF( elgFixedBuiltInFunction, mNeedCastInGroup ),
        STL_SIZEOF(stlBool),
        STL_FALSE  /* nullable */
    },
    {
        "EXCEPTION_ACTION",
        "action by argument exception",
        KNL_FXTABLE_DATATYPE_VARCHAR_POINTER,
        STL_OFFSETOF( elgFixedBuiltInFunction, mExceptionAction ),
        STL_MAX_SQL_IDENTIFIER_LENGTH,
        STL_FALSE  /* nullable */
    },
    {
        "EXCEPTION_RESULT",
        "result value by exception action",
        KNL_FXTABLE_DATATYPE_VARCHAR_POINTER,
        STL_OFFSETOF( elgFixedBuiltInFunction, mExceptionResult ),
        STL_MAX_SQL_IDENTIFIER_LENGTH,
        STL_FALSE  /* nullable */
    },
    {
        NULL,
        NULL,
        0,
        0,
        0,
        STL_FALSE
    }
};


/**
 * @brief X$BUILTIN_FUNCTION 을 위한 Open Callback 함수 정의 
 * @remarks
 */
stlStatus elgOpenFixedBuiltInFunctionCallback( void   * aStmt,
                                               void   * aDumpObjHandle,
                                               void   * aPathCtrl,
                                               knlEnv * aKnlEnv )
{
    elgFixedBuiltInFunctionPathCtrl * sPathCtrl = NULL;

    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aPathCtrl != NULL, KNL_ERROR_STACK(aKnlEnv) );

    sPathCtrl = aPathCtrl;
    
    /**
     * 첫번째 Record를 구성하기 위해 필요한 정보를 설정한다
     */
    
    sPathCtrl->mNO = KNL_BUILTIN_FUNC_RETURN_PASS + 1;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief X$BUILTIN_FUNCTION 을 위한 Close Callback 함수 정의 
 * @remarks
 */
stlStatus elgCloseFixedBuiltInFunctionCallback( void   * aDumpObjHandle,
                                                void   * aPathCtrl,
                                                knlEnv * aKnlEnv )
{
    /**
     * 할 일 없음
     */
    
    return STL_SUCCESS;
}


/**
 * @brief X$BUILTIN_FUNCTION 을 위한 BuildRecord 함수 정의 
 * @remarks
 */
stlStatus elgBuildRecordBuiltInFunctionCallback( void              * aDumpObjHandle,
                                                 void              * aPathCtrl,
                                                 knlValueBlockList * aValueList,
                                                 stlInt32            aBlockIdx,
                                                 stlBool           * aIsExist,
                                                 knlEnv            * aKnlEnv )
{
    dtlBuiltInFuncInfo               * sBuiltInFunc  = NULL;
    elgFixedBuiltInFunctionPathCtrl  * sPathCtrl = NULL;

    elgFixedBuiltInFunction   sXBuiltInRecord;
    stlMemset( & sXBuiltInRecord, 0x00, STL_SIZEOF(elgFixedBuiltInFunction) );

    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aPathCtrl != NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( aIsExist != NULL, KNL_ERROR_STACK(aKnlEnv) );
    sPathCtrl = aPathCtrl;

    /**
     * 지정된 위치가 종료 위치인지 검사
     */
    
    if ( sPathCtrl->mNO == KNL_BUILTIN_FUNC_MAX )
    {
        /**
         * 종료 지점일 경우 구성할 레코드가 없음을 알림
         */
        *aIsExist = STL_FALSE;
    }
    else
    {
        /**
         * 유효한 정보가 있으면 레코드를 위한 정보를 구성하고 
         */

        sBuiltInFunc = & gBuiltInFuncInfoArr[sPathCtrl->mNO];

        sXBuiltInRecord.mFuncID           = sBuiltInFunc->mID;
        sXBuiltInRecord.mFuncName         = (stlChar *) sBuiltInFunc->mName;
        sXBuiltInRecord.mMinArgCnt        = sBuiltInFunc->mArgumentCntMin;
        sXBuiltInRecord.mMaxArgCnt        = sBuiltInFunc->mArgumentCntMax;
        
        sXBuiltInRecord.mReturnValueClass =
            (stlChar *) gDtlFuncReturnClassString[sBuiltInFunc->mReturnTypeClass];
        STL_ASSERT(
            (sBuiltInFunc->mReturnTypeClass > DTL_FUNC_RETURN_TYPE_CLASS_INVALID) &&
            (sBuiltInFunc->mReturnTypeClass < DTL_FUNC_RETURN_TYPE_CLASS_MAX) );
                     
        sXBuiltInRecord.mIterationTime =
            (stlChar *) gDtlIterationTimeString[sBuiltInFunc->mIterationTime];
        STL_ASSERT(
            (sBuiltInFunc->mIterationTime >= DTL_ITERATION_TIME_FOR_EACH_EXPR ) &&
            (sBuiltInFunc->mIterationTime < DTL_ITERATION_TIME_INVALID) );
        
        sXBuiltInRecord.mNeedCastInGroup  = sBuiltInFunc->mIsNeedCastInGroup;
        
        sXBuiltInRecord.mExceptionAction  =
            (stlChar *) gDtlActionTypeString[sBuiltInFunc->mExceptionType];
        STL_ASSERT(
            (sBuiltInFunc->mExceptionType >= DTL_ACTION_TYPE_NORMAL) &&
            (sBuiltInFunc->mExceptionType < DTL_ACTION_TYPE_MAX) );
        
        sXBuiltInRecord.mExceptionResult  =
            (stlChar *) gDtlActionReturnString[sBuiltInFunc->mExceptionResult];
        STL_ASSERT(
            (sBuiltInFunc->mExceptionResult >= DTL_ACTION_RETURN_PASS) &&
            (sBuiltInFunc->mExceptionResult < DTL_ACTION_RETURN_MAX) );
        
        /**
         * Value List를 생성한 후
         */
        STL_TRY( knlBuildFxRecord( gElgFixedBuiltInFunctionColDesc,
                                   & sXBuiltInRecord,
                                   aValueList,
                                   aBlockIdx,
                                   aKnlEnv )
                 == STL_SUCCESS );

        *aIsExist = STL_TRUE;
        
        /**
         * 다음 레코드를 구성할 위치를 지정함.
         */

        sPathCtrl->mNO++;
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief X$BUILTIN_FUNCTION 을 위한 테이블 정의
 * @remarks
 */

knlFxTableDesc gElgFixedBuiltInFunctionTabDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,               /**< Basic Fixed Table 임 */
    KNL_STARTUP_PHASE_NO_MOUNT,                  /**< NO_MOUNT 단계부터 조회 가능 */
    NULL,                                        /**< Dump Object Handle 획득 함수 */
    elgOpenFixedBuiltInFunctionCallback,         /**< Open Callback */
    elgCloseFixedBuiltInFunctionCallback,        /**< Close Callback */
    elgBuildRecordBuiltInFunctionCallback,       /**< BuildRecord Callback */
    STL_SIZEOF(elgFixedBuiltInFunctionPathCtrl), /**< Path Controller Size */
    "X$BUILTIN_FUNCTION",                        /**< Table Name */
    "supported built-in functions",              /**< Table Comment */
    gElgFixedBuiltInFunctionColDesc              /**< 컬럼 정보 */
};


/** @} elgX_BuiltIn_Function */







/**
 * @addtogroup elgX_Pseudo_Function
 * @internal
 * @{
 */

/**
 * @brief X$PSEUDO_FUNCTION을 위한 컬럼 정의
 * @remarks
 */

knlFxColumnDesc gElgFixedPseudoFunctionColDesc[] =
{
    {
        "FUNC_ID",
        "function identifier",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( elgFixedPseudoFunction, mFuncID ),
        STL_SIZEOF(stlInt32),
        STL_FALSE  /* nullable */
    },
    {
        "FUNC_NAME",
        "function name",
        KNL_FXTABLE_DATATYPE_VARCHAR_POINTER,
        STL_OFFSETOF( elgFixedPseudoFunction, mFuncName ),
        STL_MAX_SQL_IDENTIFIER_LENGTH,
        STL_FALSE  /* nullable */
    },
    {
        "ITERATION_TIME",
        "iteration time",
        KNL_FXTABLE_DATATYPE_VARCHAR_POINTER,
        STL_OFFSETOF( elgFixedPseudoFunction, mIterationTime ),
        STL_MAX_SQL_IDENTIFIER_LENGTH,
        STL_FALSE  /* nullable */
    },
    {
        NULL,
        NULL,
        0,
        0,
        0,
        STL_FALSE  /* nullable */
    }
};


/**
 * @brief X$PSEUDO_FUNCTION 을 위한 Open Callback 함수 정의 
 * @remarks
 */
stlStatus elgOpenFixedPseudoFunctionCallback( void   * aStmt,
                                              void   * aDumpObjHandle,
                                              void   * aPathCtrl,
                                              knlEnv * aKnlEnv )
{
    elgFixedPseudoFunctionPathCtrl * sPathCtrl = NULL;

    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aPathCtrl != NULL, KNL_ERROR_STACK(aKnlEnv) );

    sPathCtrl = aPathCtrl;
    
    /**
     * 첫번째 Record를 구성하기 위해 필요한 정보를 설정한다
     */
    
    sPathCtrl->mNO = KNL_PSEUDO_COL_INVALID + 1;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief X$PSEUDO_FUNCTION 을 위한 Close Callback 함수 정의 
 * @remarks
 */
stlStatus elgCloseFixedPseudoFunctionCallback( void   * aDumpObjHandle,
                                                void   * aPathCtrl,
                                                knlEnv * aKnlEnv )
{
    /**
     * 할 일 없음
     */
    
    return STL_SUCCESS;
}


/**
 * @brief X$PSEUDO_FUNCTION 을 위한 BuildRecord 함수 정의 
 * @remarks
 */
stlStatus elgBuildRecordPseudoFunctionCallback( void              * aDumpObjHandle,
                                                 void              * aPathCtrl,
                                                 knlValueBlockList * aValueList,
                                                 stlInt32            aBlockIdx,
                                                 stlBool           * aIsExist,
                                                 knlEnv            * aKnlEnv )
{
    ellPseudoColInfo                * sPseudoFunc  = NULL;
    elgFixedPseudoFunctionPathCtrl  * sPathCtrl = NULL;

    elgFixedPseudoFunction   sXPseudoRecord;
    stlMemset( & sXPseudoRecord, 0x00, STL_SIZEOF(elgFixedPseudoFunction) );

    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aPathCtrl != NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( aIsExist != NULL, KNL_ERROR_STACK(aKnlEnv) );
    sPathCtrl = aPathCtrl;

    /**
     * 지정된 위치가 종료 위치인지 검사
     */
    
    if ( sPathCtrl->mNO == KNL_PSEUDO_COL_MAX )
    {
        /**
         * 종료 지점일 경우 구성할 레코드가 없음을 알림
         */
        *aIsExist = STL_FALSE;
    }
    else
    {
        /**
         * 유효한 정보가 있으면 레코드를 위한 정보를 구성하고 
         */

        sPseudoFunc = & gPseudoColInfoArr[sPathCtrl->mNO];

        sXPseudoRecord.mFuncID           = sPseudoFunc->mPseudoId;
        sXPseudoRecord.mFuncName         = (stlChar *) sPseudoFunc->mName;

        sXPseudoRecord.mIterationTime =
            (stlChar *) gDtlIterationTimeString[sPseudoFunc->mIterationTime];
        STL_ASSERT(
            (sPseudoFunc->mIterationTime >= DTL_ITERATION_TIME_FOR_EACH_EXPR) &&
            (sPseudoFunc->mIterationTime < DTL_ITERATION_TIME_INVALID) );
        
        /**
         * Value List를 생성한 후
         */
        STL_TRY( knlBuildFxRecord( gElgFixedPseudoFunctionColDesc,
                                   & sXPseudoRecord,
                                   aValueList,
                                   aBlockIdx,
                                   aKnlEnv )
                 == STL_SUCCESS );

        *aIsExist = STL_TRUE;
        
        /**
         * 다음 레코드를 구성할 위치를 지정함.
         */

        sPathCtrl->mNO++;
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief X$PSEUDO_FUNCTION 을 위한 테이블 정의
 * @remarks
 */

knlFxTableDesc gElgFixedPseudoFunctionTabDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,              /**< Basic Fixed Table 임 */
    KNL_STARTUP_PHASE_NO_MOUNT,                 /**< NO_MOUNT 단계부터 조회 가능 */
    NULL,                                       /**< Dump Object Handle 획득 함수 */
    elgOpenFixedPseudoFunctionCallback,         /**< Open Callback */
    elgCloseFixedPseudoFunctionCallback,        /**< Close Callback */
    elgBuildRecordPseudoFunctionCallback,       /**< BuildRecord Callback */
    STL_SIZEOF(elgFixedPseudoFunctionPathCtrl), /**< Path Controller Size */
    "X$PSEUDO_FUNCTION",                        /**< Table Name */
    "supported pseudo column functions",        /**< Table Comment */
    gElgFixedPseudoFunctionColDesc              /**< 컬럼 정보 */
};


/** @} elgX_Pseudo_Function */




/**
 * @addtogroup elgX_EXE_SYSTEM_INFO
 * @internal
 * @{
 */


/**
 * @brief X$EXE_SYSTEM_INFO 을 위한 여러 레코드 정의
 * @remarks
 */
knlFxSystemInfo gElgExeSystemInfoRows[ELG_EXE_SYSTEM_INFO_ROW_COUNT] =
{
    {
        "DICTIONARY_CACHE_OBJECT_MEMORY_TOTAL_SIZE",
        0,
        KNL_STAT_CATEGORY_TOTAL_MEMORY,
        "memory total size of dictionary object cache"
    },
    {
        "DICTIONARY_CACHE_OBJECT_MEMORY_INIT_SIZE",
        0,
        KNL_STAT_CATEGORY_INIT_MEMORY,
        "memory init size of dictionary object cache"
    },
    {
        "DICTIONARY_HASH_ELEMENT_MEMORY_TOTAL_SIZE",
        0,
        KNL_STAT_CATEGORY_TOTAL_MEMORY,
        "memory total size of dictionary hash element"
    },
    {
        "DICTIONARY_HASH_ELEMENT_MEMORY_INIT_SIZE",
        0,
        KNL_STAT_CATEGORY_INIT_MEMORY,
        "memory init size of dictionary hash element"
    },
    {
        "DICTIONARY_HASH_RELATION_MEMORY_TOTAL_SIZE",
        0,
        KNL_STAT_CATEGORY_TOTAL_MEMORY,
        "memory total size of dictionary hash relation"
    },
    {
        "DICTIONARY_HASH_RELATION_MEMORY_INIT_SIZE",
        0,
        KNL_STAT_CATEGORY_INIT_MEMORY,
        "memory init size of dictionary hash relation"
    },
    {
        "DICTIONARY_HASH_RELATED_TRANS_MEMORY_TOTAL_SIZE",
        0,
        KNL_STAT_CATEGORY_TOTAL_MEMORY,
        "memory total size of dictionary hash related transaction"
    },
    {
        "DICTIONARY_HASH_RELATED_TRANS_MEMORY_INIT_SIZE",
        0,
        KNL_STAT_CATEGORY_INIT_MEMORY,
        "memory init size of dictionary hash related transaction"
    },
    {
        "DDL_PENDING_OPERATION_MEMORY_TOTAL_SIZE",
        0,
        KNL_STAT_CATEGORY_TOTAL_MEMORY,
        "memory total size of DDL pending operation"
    },
    {
        "DDL_PENDING_OPERATION_MEMORY_INIT_SIZE",
        0,
        KNL_STAT_CATEGORY_INIT_MEMORY,
        "memory init size of DDL pending operation"
    }
};



/**
 * @brief X$EXE_SYSTEM_INFO 를 위한 Open Callback 함수 정의 
 * @remarks
 */
stlStatus elgOpenFxExeSystemInfoCallback( void   * aStmt,
                                          void   * aDumpObjHandle,
                                          void   * aPathCtrl,
                                          knlEnv * aKnlEnv )
{
    elgFxExeSystemInfoPathCtrl * sPathCtrl;
    stlInt64                   * sValues;

    sPathCtrl  = (elgFxExeSystemInfoPathCtrl *) aPathCtrl;
    sPathCtrl->mIterator = 0;

    sValues = sPathCtrl->mValues;

    /**
     * DICTIONARY_CACHE_OBJECT_MEMORY_TOTAL_SIZE
     */
    
    sValues[0] = gEllSharedEntry->mCurrCacheEntry->mMemDictCache.mTotalSize;

    /**
     * DICTIONARY_CACHE_OBJECT_MEMORY_INIT_SIZE
     */
    
    sValues[1] = gEllSharedEntry->mCurrCacheEntry->mMemDictCache.mInitSize;
    

    /**
     * DICTIONARY_HASH_ELEMENT_MEMORY_TOTAL_SIZE
     */

    sValues[2] = gEllSharedEntry->mCurrCacheEntry->mMemDictHashElement.mTotalSize;
    
    /**
     * DICTIONARY_HASH_ELEMENT_MEMORY_INIT_SIZE
     */

    sValues[3] = gEllSharedEntry->mCurrCacheEntry->mMemDictHashElement.mInitSize;
    
    /**
     *  DICTIONARY_HASH_RELATION_MEMORY_TOTAL_SIZE
     */

    sValues[4] = gEllSharedEntry->mCurrCacheEntry->mMemDictHashRelated.mTotalSize;
    
    /**
     *  DICTIONARY_HASH_RELATION_MEMORY_INIT_SIZE
     */

    sValues[5] = gEllSharedEntry->mCurrCacheEntry->mMemDictHashRelated.mInitSize;

    /**
     *  DICTIONARY_HASH_RELATED_TRANS_MEMORY_TOTAL_SIZE
     */

    sValues[6] = gEllSharedEntry->mCurrCacheEntry->mMemDictRelatedTrans.mTotalSize;
    
    /**
     *  DICTIONARY_HASH_RELATED_TRANS_MEMORY_INIT_SIZE
     */

    sValues[7] = gEllSharedEntry->mCurrCacheEntry->mMemDictRelatedTrans.mInitSize;
    
    /**
     *  DDL_PENDING_OPERATION_MEMORY_TOTAL_SIZE
     */

    sValues[8] = gEllSharedEntry->mCurrCacheEntry->mMemPendOP.mTotalSize;
    
    /**
     *  DDL_PENDING_OPERATION_MEMORY_INIT_SIZE
     */

    sValues[9] = gEllSharedEntry->mCurrCacheEntry->mMemPendOP.mInitSize;
    
    return STL_SUCCESS;
}


/**
 * @brief X$EXE_SYSTEM_INFO 을 위한 Close Callback 함수 정의 
 * @remarks
 */
stlStatus elgCloseFxExeSystemInfoCallback( void   * aDumpObjHandle,
                                           void   * aPathCtrl,
                                           knlEnv * aKnlEnv )
{
    return STL_SUCCESS;
}


/**
 * @brief X$EXE_SYSTEM_INFO 을 위한 BuildRecord 함수 정의 
 * @remarks
 */
stlStatus elgBuildRecordFxExeSystemInfoCallback( void              * aDumpObjHandle,
                                                 void              * aPathCtrl,
                                                 knlValueBlockList * aValueList,
                                                 stlInt32            aBlockIdx,
                                                 stlBool           * aIsExist,
                                                 knlEnv            * aKnlEnv )
{
    elgFxExeSystemInfoPathCtrl * sPathCtrl;
    knlFxSystemInfo              sFxSystemInfo;
    knlFxSystemInfo            * sRowDesc;
    stlInt64                   * sValues;

    *aIsExist = STL_FALSE;
    
    sPathCtrl = (elgFxExeSystemInfoPathCtrl *) aPathCtrl;
    sValues = sPathCtrl->mValues;

    sRowDesc = & gElgExeSystemInfoRows[sPathCtrl->mIterator];

    if( sPathCtrl->mIterator < ELG_EXE_SYSTEM_INFO_ROW_COUNT )
    {
        sFxSystemInfo.mName     = sRowDesc->mName;
        sFxSystemInfo.mValue    = sValues[sPathCtrl->mIterator];
        sFxSystemInfo.mComment  = sRowDesc->mComment;
        sFxSystemInfo.mCategory = sRowDesc->mCategory;

        STL_TRY( knlBuildFxRecord( gKnlSystemInfoColumnDesc,
                                   &sFxSystemInfo,
                                   aValueList,
                                   aBlockIdx,
                                   aKnlEnv )
                 == STL_SUCCESS );

        *aIsExist = STL_TRUE;
        sPathCtrl->mIterator += 1;
    }

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


knlFxTableDesc gElgFxExeSystemInfoTabDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,
    KNL_STARTUP_PHASE_NO_MOUNT,
    NULL,
    elgOpenFxExeSystemInfoCallback,
    elgCloseFxExeSystemInfoCallback,
    elgBuildRecordFxExeSystemInfoCallback,
    STL_SIZEOF( elgFxExeSystemInfoPathCtrl ),
    "X$EXE_SYSTEM_INFO",
    "system information of execution library layer",
    gKnlSystemInfoColumnDesc
};


    
/** @} elgX_EXE_SYSTEM_INFO */







/**
 * @addtogroup elgX_EXE_PROC_ENV
 * @internal
 * @{
 */

/**
 * @brief X$EXE_PROC_ENV 을 위한 여러 레코드 정의
 * @remarks
 */
knlFxSessionInfo gElgExeProcEnvRows[ELG_EXE_PROC_ENV_ROW_COUNT] =
{
    {
        "DDL_OPERATION_MEMORY_TOTAL_SIZE",
        0,
        KNL_STAT_CATEGORY_TOTAL_MEMORY,
        0
    },
    {
        "DDL_OPERATION_MEMORY_INIT_SIZE",
        0,
        KNL_STAT_CATEGORY_INIT_MEMORY,
        0
    }
};



/**
 * @brief X$EXE_PROC_ENV 을 위한 Open Callback 함수 정의 
 * @remarks
 */
stlStatus elgOpenFxExeProcEnvCallback( void   * aStmt,
                                       void   * aDumpObjHandle,
                                       void   * aPathCtrl,
                                       knlEnv * aKnlEnv )
{
    elgFxExeProcEnvPathCtrl * sPathCtrl;
    void                    * sProcEnv;
    ellEnv                  * sEllEnv;
    stlInt64                * sValues;

    sPathCtrl  = (elgFxExeProcEnvPathCtrl*) aPathCtrl;

    sPathCtrl->mCurrEnv = NULL;
    sPathCtrl->mIterator = 0;
    sValues = sPathCtrl->mValues;

    /**
     * 연결이 유효한 첫 Proc Env 에 대해 레코드들을 구축한다.
     */
    sProcEnv = knlGetFirstEnv();

    while( sProcEnv != NULL )
    {
        if( knlIsUsedEnv( sProcEnv ) == STL_TRUE )
        {
            STL_TRY( knlGetEnvId( sProcEnv,
                                  & sPathCtrl->mProcId )
                     == STL_SUCCESS );
            
            sPathCtrl->mCurrEnv = sProcEnv;
            sEllEnv = ELL_ENV(sProcEnv);

            /**
             * DDL_OPERATION_MEMORY_TOTAL_SIZE
             */
            
            sValues[0] = sEllEnv->mMemDictOP.mTotalSize;

            /**
             * DDL_OPERATION_MEMORY_INIT_SIZE
             */
            
            sValues[1] = sEllEnv->mMemDictOP.mInitSize;

            break;
        }
        else
        {
            sProcEnv = knlGetNextEnv( sProcEnv );
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief X$EXE_PROC_ENV 을 위한 Close Callback 함수 정의 
 * @remarks
 */
stlStatus elgCloseFxExeProcEnvCallback( void   * aDumpObjHandle,
                                        void   * aPathCtrl,
                                        knlEnv * aKnlEnv )
{
    return STL_SUCCESS;
}



/**
 * @brief X$EXE_PROC_ENV 을 위한 BuildRecord 함수 정의 
 * @remarks
 */
stlStatus elgBuildRecordFxExeProcEnvCallback( void              * aDumpObjHandle,
                                              void              * aPathCtrl,
                                              knlValueBlockList * aValueList,
                                              stlInt32            aBlockIdx,
                                              stlBool           * aIsExist,
                                              knlEnv            * aKnlEnv )
{
    elgFxExeProcEnvPathCtrl  * sPathCtrl;
    knlFxSessionInfo           sFxProcEnvInfo;
    knlFxSessionInfo         * sRowDesc;
    stlInt64                 * sValues;
    void                     * sProcEnv;
    ellEnv                   * sEllEnv;

    *aIsExist = STL_FALSE;
    
    sPathCtrl = (elgFxExeProcEnvPathCtrl*)aPathCtrl;
    sValues = sPathCtrl->mValues;

    /**
     * 하나의 Proc Env 에 대한 모든 레코드들을 Fetch 했다면,
     * 다음 유효한 Proc Env 에 대해 레코드들을 구축 
     */
    if( sPathCtrl->mIterator >= ELG_EXE_PROC_ENV_ROW_COUNT )
    {
        sProcEnv = knlGetNextEnv( sPathCtrl->mCurrEnv );

        while( sProcEnv != NULL )
        {
            if( knlIsUsedEnv( sProcEnv ) == STL_TRUE )
            {
                STL_TRY( knlGetEnvId( sProcEnv,
                                      & sPathCtrl->mProcId )
                         == STL_SUCCESS );
                
                sPathCtrl->mCurrEnv = sProcEnv;
                sEllEnv = ELL_ENV(sProcEnv);
                
                /**
                 * DDL_OPERATION_MEMORY_TOTAL_SIZE
                 */
                
                sValues[0] = sEllEnv->mMemDictOP.mTotalSize;
                
                /**
                 * DDL_OPERATION_MEMORY_INIT_SIZE
                 */
            
                sValues[1] = sEllEnv->mMemDictOP.mInitSize;

                break;
            }
            else
            {
                sProcEnv = knlGetNextEnv( sProcEnv );
            }
        }

        sPathCtrl->mCurrEnv = sProcEnv;
        sPathCtrl->mIterator = 0;
    }
    else
    {
        /**
         * 해당 Session 의 레코드 정보가 아직 있음
         */
    }
    
    STL_TRY_THROW( sPathCtrl->mCurrEnv != NULL, RAMP_SUCCESS );

    sRowDesc = &gElgExeProcEnvRows[sPathCtrl->mIterator];

    sFxProcEnvInfo.mName     = sRowDesc->mName;
    sFxProcEnvInfo.mCategory = sRowDesc->mCategory;
    sFxProcEnvInfo.mSessId   = sPathCtrl->mProcId;
    sFxProcEnvInfo.mValue    = sValues[sPathCtrl->mIterator];

    STL_TRY( knlBuildFxRecord( gKnlSessionInfoColumnDesc,
                               &sFxProcEnvInfo,
                               aValueList,
                               aBlockIdx,
                               aKnlEnv )
             == STL_SUCCESS );

    *aIsExist = STL_TRUE;
    sPathCtrl->mIterator += 1;

    STL_RAMP( RAMP_SUCCESS );

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

knlFxTableDesc gElgFxExeProcEnvTabDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,
    KNL_STARTUP_PHASE_NO_MOUNT,
    NULL,
    elgOpenFxExeProcEnvCallback,
    elgCloseFxExeProcEnvCallback,
    elgBuildRecordFxExeProcEnvCallback,
    STL_SIZEOF( elgFxExeProcEnvPathCtrl ),
    "X$EXE_PROC_ENV",
    "process environment information of execution library layer",
    gKnlSessionInfoColumnDesc
};

/** @} elgX_EXE_PROC_ENV */





/**
 * @addtogroup elgX_EXE_SESS_ENV
 * @internal
 * @{
 */

/**
 * @brief X$EXE_SESS_ENV 을 위한 여러 레코드 정의
 * @remarks
 */
knlFxSessionInfo gElgExeSessEnvRows[ELG_EXE_SESS_ENV_ROW_COUNT] =
{
    {
        "AUTHORIZATION_ID",
        0,
        KNL_STAT_CATEGORY_NONE,
        0
    },
    {
        "HAS_UNCOMMITED_DDL",
        0,
        KNL_STAT_CATEGORY_NONE,
        0
    },
    {
        "LOCAL_MODIFY_SEQ",
        0,
        KNL_STAT_CATEGORY_NONE,
        0
    },    
    {
        "SESSION_BASED_HASH_MEMORY_TOTAL_SIZE",
        0,
        KNL_STAT_CATEGORY_TOTAL_MEMORY,
        0
    },
    {
        "SESSION_BASED_HASH_MEMORY_INIT_SIZE",
        0,
        KNL_STAT_CATEGORY_INIT_MEMORY,
        0
    },
    {
        "SESSION_BASED_OBJECT_MEMORY_TOTAL_SIZE",
        0,
        KNL_STAT_CATEGORY_TOTAL_MEMORY,
        0
    },
    {
        "SESSION_BASED_OBJECT_MEMORY_INIT_SIZE",
        0,
        KNL_STAT_CATEGORY_INIT_MEMORY,
        0
    },
    {
        "SESSION_BASED_CURSOR_SLOT_MEMORY_TOTAL_SIZE",
        0,
        KNL_STAT_CATEGORY_TOTAL_MEMORY,
        0
    },
    {
        "SESSION_BASED_CURSOR_SLOT_MEMORY_INIT_SIZE",
        0,
        KNL_STAT_CATEGORY_INIT_MEMORY,
        0
    }
    
};



/**
 * @brief X$EXE_SESS_ENV 을 위한 Open Callback 함수 정의 
 * @remarks
 */
stlStatus elgOpenFxExeSessEnvCallback( void   * aStmt,
                                       void   * aDumpObjHandle,
                                       void   * aPathCtrl,
                                       knlEnv * aKnlEnv )
{
    elgFxExeSessEnvPathCtrl * sPathCtrl;
    knlSessionEnv           * sSessEnv;
    ellSessionEnv           * sEllSessEnv;
    stlInt64                * sValues;

    sPathCtrl  = (elgFxExeSessEnvPathCtrl*) aPathCtrl;

    sPathCtrl->mCurEnv = NULL;
    sPathCtrl->mIterator = 0;
    sValues = sPathCtrl->mValues;

    /**
     * 연결이 유효한 첫 Session 에 대해 레코드들을 구축한다.
     */
    sSessEnv = knlGetFirstSessionEnv();

    while( sSessEnv != NULL )
    {
        if( knlIsUsedSessionEnv( sSessEnv ) == STL_TRUE )
        {
            STL_TRY( knlGetSessionEnvId( sSessEnv,
                                         &sPathCtrl->mSessId,
                                         aKnlEnv )
                     == STL_SUCCESS );
            
            sPathCtrl->mCurEnv = sSessEnv;
            sEllSessEnv = (ellSessionEnv *) sSessEnv;

            /**
             * AUTHORIZATION_ID
             */
            
            sValues[0] = sEllSessEnv->mLoginAuthID;

            /**
             * HAS_COMMITED_DDL
             */
            
            if ( sEllSessEnv->mIncludeDDL == STL_TRUE )
            {
                sValues[1] = 1;
            }
            else
            {
                sValues[1] = 0;
            }

            /**
             * LOCAL_MODIFY_SEQ
             */
            
            sValues[2] = sEllSessEnv->mLocalModifySeq;
            
            /**
             * SESSION_BASED_HASH_MEMORY_TOTAL_SIZE
             */
            
            sValues[3] = sEllSessEnv->mSessObjMgr.mMemSessHash.mTotalSize;
            
            /**
             * SESSION_BASED_HASH_MEMORY_INIT_SIZE
             */
            
            sValues[4] = sEllSessEnv->mSessObjMgr.mMemSessHash.mInitSize;
            
            /**
             * SESSION_BASED_OBJECT_MEMORY_TOTAL_SIZE
             */
            
            sValues[5] = sEllSessEnv->mSessObjMgr.mMemSessObj.mTotalSize;
            
            /**
             * SESSION_BASED_OBJECT_MEMORY_INIT_SIZE
             */
            
            sValues[6] = sEllSessEnv->mSessObjMgr.mMemSessObj.mInitSize;

            /**
             * SESSION_BASED_CURSOR_SLOT_MEMORY_TOTAL_SIZE
             */
            
            sValues[7] = sEllSessEnv->mSessObjMgr.mMemCursorSlot.mTotalSize;

            /**
             * SESSION_BASED_CURSOR_SLOT_MEMORY_INIT_SIZE
             */
            
            sValues[8] = sEllSessEnv->mSessObjMgr.mMemCursorSlot.mInitSize;

            
            break;
        }
        else
        {
            sSessEnv = knlGetNextSessionEnv( sSessEnv );
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief X$EXE_SESS_ENV 을 위한 Close Callback 함수 정의 
 * @remarks
 */
stlStatus elgCloseFxExeSessEnvCallback( void   * aDumpObjHandle,
                                        void   * aPathCtrl,
                                        knlEnv * aKnlEnv )
{
    return STL_SUCCESS;
}



/**
 * @brief X$EXE_SESS_ENV 을 위한 BuildRecord 함수 정의 
 * @remarks
 */
stlStatus elgBuildRecordFxExeSessEnvCallback( void              * aDumpObjHandle,
                                              void              * aPathCtrl,
                                              knlValueBlockList * aValueList,
                                              stlInt32            aBlockIdx,
                                              stlBool           * aIsExist,
                                              knlEnv            * aKnlEnv )
{
    elgFxExeSessEnvPathCtrl  * sPathCtrl;
    knlFxSessionInfo           sFxSessionInfo;
    knlFxSessionInfo         * sRowDesc;
    stlInt64                 * sValues;
    knlSessionEnv            * sSessEnv;
    ellSessionEnv            * sEllSessEnv;

    *aIsExist = STL_FALSE;
    
    sPathCtrl = (elgFxExeSessEnvPathCtrl*)aPathCtrl;
    sValues = sPathCtrl->mValues;

    /**
     * 하나의 Session 에 대한 모든 레코드들을 Fetch 했다면,
     * 다음 유효한 Session 에 대해 레코드들을 구축 
     */
    if( sPathCtrl->mIterator >= ELG_EXE_SESS_ENV_ROW_COUNT )
    {
        sSessEnv = knlGetNextSessionEnv( sPathCtrl->mCurEnv );

        while( sSessEnv != NULL )
        {
            if( knlIsUsedSessionEnv( sSessEnv ) == STL_TRUE )
            {
                STL_TRY( knlGetSessionEnvId( sSessEnv,
                                             &sPathCtrl->mSessId,
                                             aKnlEnv )
                         == STL_SUCCESS );

                sPathCtrl->mCurEnv = sSessEnv;
                sEllSessEnv = (ellSessionEnv *) sSessEnv;
            
                /**
                 * AUTHORIZATION_ID
                 */

                sValues[0] = sEllSessEnv->mLoginAuthID;
                
                /**
                 * HAS_COMMITED_DDL
                 */
                
                if ( sEllSessEnv->mIncludeDDL == STL_TRUE )
                {
                    sValues[1] = 1;
                }
                else
                {
                    sValues[1] = 0;
                }

                /**
                 * LOCAL_MODIFY_COUNT
                 */
            
                sValues[2] = sEllSessEnv->mLocalModifySeq;
                
                /**
                 * SESSION_BASED_HASH_MEMORY_TOTAL_SIZE
                 */
                
                sValues[3] = sEllSessEnv->mSessObjMgr.mMemSessHash.mTotalSize;
                
                /**
                 * SESSION_BASED_HASH_MEMORY_INIT_SIZE
                 */
            
                sValues[4] = sEllSessEnv->mSessObjMgr.mMemSessHash.mInitSize;

                /**
                 * SESSION_BASED_OBJECT_MEMORY_TOTAL_SIZE
                 */
                
                sValues[5] = sEllSessEnv->mSessObjMgr.mMemSessObj.mTotalSize;
                
                /**
                 * SESSION_BASED_OBJECT_MEMORY_INIT_SIZE
                 */
            
                sValues[6] = sEllSessEnv->mSessObjMgr.mMemSessObj.mInitSize;

                /**
                 * SESSION_BASED_CURSOR_SLOT_MEMORY_TOTAL_SIZE
                 */
                
                sValues[7] = sEllSessEnv->mSessObjMgr.mMemCursorSlot.mTotalSize;
                
                /**
                 * SESSION_BASED_CURSOR_SLOT_MEMORY_INIT_SIZE
                 */
                
                sValues[8] = sEllSessEnv->mSessObjMgr.mMemCursorSlot.mInitSize;
                
                break;
            }
            else
            {
                sSessEnv = knlGetNextSessionEnv( sSessEnv );
            }
        }

        sPathCtrl->mCurEnv = sSessEnv;
        sPathCtrl->mIterator = 0;
    }
    else
    {
        /**
         * 해당 Session 의 레코드 정보가 아직 있음
         */
    }
    
    STL_TRY_THROW( sPathCtrl->mCurEnv != NULL, RAMP_SUCCESS );

    sRowDesc = &gElgExeSessEnvRows[sPathCtrl->mIterator];

    sFxSessionInfo.mName     = sRowDesc->mName;
    sFxSessionInfo.mSessId   = sPathCtrl->mSessId;
    sFxSessionInfo.mValue    = sValues[sPathCtrl->mIterator];
    sFxSessionInfo.mCategory = sRowDesc->mCategory;

    STL_TRY( knlBuildFxRecord( gKnlSessionInfoColumnDesc,
                               &sFxSessionInfo,
                               aValueList,
                               aBlockIdx,
                               aKnlEnv )
             == STL_SUCCESS );

    *aIsExist = STL_TRUE;
    sPathCtrl->mIterator += 1;

    STL_RAMP( RAMP_SUCCESS );

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

knlFxTableDesc gElgFxExeSessEnvTabDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,
    KNL_STARTUP_PHASE_NO_MOUNT,
    NULL,
    elgOpenFxExeSessEnvCallback,
    elgCloseFxExeSessEnvCallback,
    elgBuildRecordFxExeSessEnvCallback,
    STL_SIZEOF( elgFxExeSessEnvPathCtrl ),
    "X$EXE_SESS_ENV",
    "session environment information of execution library layer",
    gKnlSessionInfoColumnDesc
};

/** @} elgX_EXE_SESS_ENV */





/**
 * @addtogroup elgX_NAMED_CURSOR
 * @internal
 * @{
 */


knlFxColumnDesc gElgNamedCursorColDesc[] =
{
    {
        "SLOT_ID",
        "named cursor slot id",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( elgNameCursorFxRecord, mSlotID ),
        STL_SIZEOF( stlInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "SESSION_ID",
        "session id",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( elgNameCursorFxRecord, mSessID ),
        STL_SIZEOF( stlInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "CURSOR_NAME",
        "cursor name",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( elgNameCursorFxRecord, mCursorName ),
        STL_MAX_SQL_IDENTIFIER_LENGTH,
        STL_FALSE  /* nullable */
    },
    {
        "IS_OPEN",
        "cursor is open or not",
        KNL_FXTABLE_DATATYPE_BOOL,
        STL_OFFSETOF( elgNameCursorFxRecord, mIsOpen ),
        STL_SIZEOF( stlBool ),
        STL_FALSE  /* nullable */
    },
    {
        "OPEN_TIME",
        "open time",
        KNL_FXTABLE_DATATYPE_TIMESTAMP,
        STL_OFFSETOF( elgNameCursorFxRecord, mOpenTime ),
        STL_SIZEOF( stlTime ),
        STL_FALSE  /* nullable */
    },
    {
        "VIEW_SCN",
        "view scn",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( elgNameCursorFxRecord, mViewScn ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "IS_SENSITIVE",
        "cursor is sensitive or not",
        KNL_FXTABLE_DATATYPE_BOOL,
        STL_OFFSETOF( elgNameCursorFxRecord, mIsSensitive ),
        STL_SIZEOF( stlBool ),
        STL_FALSE  /* nullable */
    },
    {
        "IS_SCROLLABLE",
        "cursor is scrollable or not",
        KNL_FXTABLE_DATATYPE_BOOL,
        STL_OFFSETOF( elgNameCursorFxRecord, mIsScrollable ),
        STL_SIZEOF( stlBool ),
        STL_FALSE  /* nullable */
    },
    {
        "IS_HOLDABLE",
        "cursor is holdable or not",
        KNL_FXTABLE_DATATYPE_BOOL,
        STL_OFFSETOF( elgNameCursorFxRecord, mIsHoldable ),
        STL_SIZEOF( stlBool ),
        STL_FALSE  /* nullable */
    },
    {
        "IS_UPDATABLE",
        "cursor is updatable or not",
        KNL_FXTABLE_DATATYPE_BOOL,
        STL_OFFSETOF( elgNameCursorFxRecord, mIsUpdatable ),
        STL_SIZEOF( stlBool ),
        STL_FALSE  /* nullable */
    },
    {
        "CURSOR_QUERY",
        "cursor query",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( elgNameCursorFxRecord, mCursorQuery ),
        ELL_MAX_NAMED_CURSOR_QUERY_SIZE,
        STL_TRUE  /* nullable */
    },
    
    {
        NULL,
        NULL,
        0,
        0,
        0,
        STL_FALSE  /* nullable */
    }
};


stlStatus elgOpenFxNamedCursorCallback( void   * aStmt,
                                        void   * aDumpObjHandle,
                                        void   * aPathCtrl,
                                        knlEnv * aKnlEnv )
{
    elgNameCursorPathCtrl * sPathCtrl;

    sPathCtrl = (elgNameCursorPathCtrl *) aPathCtrl;

    sPathCtrl->mCurEnv  = NULL;
    sPathCtrl->mCurSlot = -1;

    return STL_SUCCESS;
}


stlStatus elgCloseFxNamedCursorCallback( void   * aDumpObjHandle,
                                         void   * aPathCtrl,
                                         knlEnv * aKnlEnv )
{
    return STL_SUCCESS;
}


stlStatus elgBuildRecordFxNamedCursorCallback( void              * aDumpObjHandle,
                                               void              * aPathCtrl,
                                               knlValueBlockList * aValueList,
                                               stlInt32            aBlockIdx,
                                               stlBool           * aIsExist,
                                               knlEnv            * aKnlEnv )
{
    elgNameCursorPathCtrl  * sPathCtrl = NULL;
    elgNameCursorFxRecord    sFxRecord;

    knlSessionEnv         * sSessEnv = NULL;
    ellSessionEnv         * sSession = NULL;

    stlInt32              sMaxSlotCount = 0;
    stlInt32              i = 0;
    
    ellNamedCursorSlot    ** sSlotArray  = NULL;
    ellNamedCursorSlot     * sCursorSlot = NULL;

    /**
     * 정보 초기화 
     */
    
    *aIsExist = STL_FALSE;
    
    sPathCtrl  = (elgNameCursorPathCtrl*) aPathCtrl;

    /**
     * Session 획득
     */
    
    if( sPathCtrl->mCurEnv == NULL )
    {
        sSessEnv = knlGetFirstSessionEnv();
        
        while( sSessEnv != NULL )
        {
            /**
             * admin session은 DBC를 위해 heap memory를 사용한다.
             */
        
            if( (knlIsUsedSessionEnv( sSessEnv ) == STL_TRUE) &&
                (knlIsAdminSession( sSessEnv ) == STL_FALSE) )
            {
                break;
            }
        
            sSessEnv = knlGetNextSessionEnv( sSessEnv );
        }

        sPathCtrl->mCurEnv = sSessEnv;
    }

    /**
     * Cursor Slot 획득
     */
    
    while( sPathCtrl->mCurEnv != NULL )
    {
        sSession = (ellSessionEnv*)sPathCtrl->mCurEnv;
        
        sMaxSlotCount = sSession->mSessObjMgr.mMaxCursorCount;
        
        sCursorSlot = NULL;
        sSlotArray = sSession->mSessObjMgr.mCursorSlot;

        if( sSlotArray != NULL )
        {
            /**
             * 다음 Cursor Slot 을 획득
             */
            
            for( i = sPathCtrl->mCurSlot + 1; i < sMaxSlotCount; i++ )
            {
                if( sSlotArray[i] != NULL )
                {
                    sCursorSlot = sSlotArray[i];
                    sPathCtrl->mCurSlot = i;

                    if( sCursorSlot != NULL )
                    {
                        /**
                         * garbage 라도 유효한 포인터라면 출력한다.
                         */
                        if( knlIsValidStaticAddress( sCursorSlot, STL_SIZEOF(ellNamedCursorSlot) ) == STL_TRUE )
                        {
                            break;
                        }
                
                        /**
                         * goto next Session
                         */
                        
                        sCursorSlot = NULL;
                        break;
                    }
                    else
                    {
                        /**
                         * next slot
                         */
                        continue;
                    }
                }
            }
        }

        /**
         * Cursor Slot 이 없는 경우 Next Session 획득
         */
        
        if( sCursorSlot != NULL )
        {
            break;
        }
        else
        {
            /**
             * Next Session 획득
             */
            
            sSessEnv = knlGetNextSessionEnv( sPathCtrl->mCurEnv );

            while( sSessEnv != NULL )
            {
                /**
                 * admin session은 DBC를 위해 heap memory를 사용한다.
                 */
        
                if( (knlIsUsedSessionEnv( sSessEnv ) == STL_TRUE) &&
                    (knlIsAdminSession( sSessEnv ) == STL_FALSE) )
                {
                    break;
                }
        
                sSessEnv = knlGetNextSessionEnv( sSessEnv );
            }
            
            sPathCtrl->mCurEnv = sSessEnv;
        }

        sPathCtrl->mCurSlot = -1;
    }
    
    STL_TRY_THROW( sCursorSlot != NULL, RAMP_SUCCESS );
    STL_DASSERT( sPathCtrl->mCurEnv != NULL );

    /**
     * Fixed Table Record 정보 구성
     */

    sFxRecord.mSlotID = sPathCtrl->mCurSlot;
    
    STL_TRY( knlGetSessionEnvId( sPathCtrl->mCurEnv,
                                 & sFxRecord.mSessID,
                                 aKnlEnv )
             == STL_SUCCESS );
    
    stlStrncpy( sFxRecord.mCursorName, sCursorSlot->mCursorName, STL_MAX_SQL_IDENTIFIER_LENGTH );
    sFxRecord.mIsOpen = sCursorSlot->mIsOpen;

    sFxRecord.mOpenTime = sCursorSlot->mOpenTime;
    sFxRecord.mViewScn  = sCursorSlot->mViewScn;

    if ( sCursorSlot->mCursorProperty.mSensitivity == ELL_CURSOR_SENSITIVITY_SENSITIVE )
    {
        sFxRecord.mIsSensitive = STL_TRUE;
    }
    else
    {
        sFxRecord.mIsSensitive = STL_FALSE;
    }

    if ( sCursorSlot->mCursorProperty.mScrollability == ELL_CURSOR_SCROLLABILITY_SCROLL )
    {
        sFxRecord.mIsScrollable = STL_TRUE;
    }
    else
    {
        sFxRecord.mIsScrollable = STL_FALSE;
    }

    if ( sCursorSlot->mCursorProperty.mHoldability == ELL_CURSOR_HOLDABILITY_WITHOUT_HOLD )
    {
        sFxRecord.mIsHoldable = STL_FALSE;
    }
    else
    {
        sFxRecord.mIsHoldable = STL_TRUE;
    }

    if ( sCursorSlot->mCursorProperty.mUpdatability == ELL_CURSOR_UPDATABILITY_FOR_UPDATE )
    {
        sFxRecord.mIsUpdatable = STL_TRUE;
    }
    else
    {
        sFxRecord.mIsUpdatable = STL_FALSE;
    }

    stlStrncpy( sFxRecord.mCursorQuery, sCursorSlot->mCursorQuery, ELL_MAX_NAMED_CURSOR_QUERY_SIZE );
    
    STL_TRY( knlBuildFxRecord( gElgNamedCursorColDesc,
                               & sFxRecord,
                               aValueList,
                               aBlockIdx,
                               aKnlEnv )
             == STL_SUCCESS );

    *aIsExist = STL_TRUE;

    STL_RAMP( RAMP_SUCCESS );

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


knlFxTableDesc gElgNamedCursorTabDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,
    KNL_STARTUP_PHASE_NO_MOUNT,
    NULL,
    elgOpenFxNamedCursorCallback,
    elgCloseFxNamedCursorCallback,
    elgBuildRecordFxNamedCursorCallback,
    STL_SIZEOF( elgNameCursorPathCtrl ),
    "X$NAMED_CURSOR",
    "named cursor information",
    gElgNamedCursorColDesc
};


/** @} elgX_NAMED_CURSOR */
