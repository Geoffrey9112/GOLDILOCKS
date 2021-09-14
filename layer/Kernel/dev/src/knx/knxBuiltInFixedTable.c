/*******************************************************************************
 * knxBuiltInFixedTable.c
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
 * @file knxBuiltInFixedTable.c
 * @brief Built-In Fixed Table 정의 
 */


#include <knl.h>

#include <knxDef.h>
#include <knxFixedTable.h>
#include <knxBuiltInFixedTable.h>

/**
 * @ingroup X_TABLES
 * @{
 */

/**
 * @brief X$TABLES을 위한 컬럼 정의
 * @remarks
 * Dictionary Cache 를 위해 사용하고 있는 정보이므로,
 * 변경이 필요하면 knlFxTableColumnOrder 의 정의도 변경해야 한다.
 */

knlFxColumnDesc gXTablesColDesc[] =
{
    {
        "TABLE_ID",
        "table identifier",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( knxXTablesStructure, mTableID ),
        STL_SIZEOF(stlInt64),
        STL_FALSE  /* nullable */
    },
    {
        "TABLE_NAME",
        "fixed table name",
        KNL_FXTABLE_DATATYPE_VARCHAR_POINTER,
        STL_OFFSETOF( knxXTablesStructure, mTableName ),
        STL_MAX_SQL_IDENTIFIER_LENGTH,
        STL_FALSE  /* nullable */
    },
    {
        "COMMENTS",
        "descriptions for a fixed table",
        KNL_FXTABLE_DATATYPE_VARCHAR_POINTER,
        STL_OFFSETOF( knxXTablesStructure, mTableComment ),
        KNL_MAX_FIXEDTABLE_COMMENTS_LENGTH,
        STL_TRUE  /* nullable */
    },
    {
        "COLUMN_COUNT",
        "column count of the table",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( knxXTablesStructure, mColumnCount ),
        STL_SIZEOF( stlInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "USAGE_TYPE_ID",
        "usgae type identifier",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( knxXTablesStructure, mUsageTypeValue ),
        STL_SIZEOF( stlInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "USAGE_TYPE",
        "usage type name (fixed table or dump table)",
        KNL_FXTABLE_DATATYPE_VARCHAR_POINTER,
        STL_OFFSETOF( knxXTablesStructure, mUsageTypeName ),
        KNX_MAX_BUILTIN_NAMESIZE_USAGETYPE,
        STL_FALSE  /* nullable */
    },
    {
        "STARTUP_PHASE_ID",
        "startup phase identifier",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( knxXTablesStructure, mStartupPhaseValue ),
        STL_SIZEOF( stlInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "STARTUP_PHASE",
        "startup phase name",
        KNL_FXTABLE_DATATYPE_VARCHAR_POINTER,
        STL_OFFSETOF( knxXTablesStructure, mStartupPhaseName ),
        KNX_MAX_BUILTIN_NAMESIZE_STARTUPPHASE,
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
 * @brief X$TABLES을 위한 Open Callback 함수 정의 
 * @remarks
 */
stlStatus knxOpenXTablesCallback( void   * aStmt,
                                  void   * aDumpObjHandle,
                                  void   * aPathCtrl,
                                  knlEnv * aKnlEnv )
{
    knxXTablesPathCtrl * sPathCtrl = NULL;

    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aPathCtrl != NULL, KNL_ERROR_STACK(aKnlEnv) );

    sPathCtrl = aPathCtrl;
    
    /**
     * 첫번째 Record를 구성하기 위해 필요한 정보를 설정한다
     */
    
    sPathCtrl->mTabIdx  = 0;
    sPathCtrl->mTableID = -1;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief X$TABLES을 위한 Close Callback 함수 정의 
 * @remarks
 */
stlStatus knxCloseXTablesCallback( void   * aDumpObjHandle,
                                   void   * aPathCtrl,
                                   knlEnv * aKnlEnv )
{
    /** 할 일 없음 */
    return STL_SUCCESS;
}

/**
 * @brief X$TABLES을 위한 BuildRecord 함수 정의 
 * @remarks
 */
stlStatus knxBuildRecordXTablesCallback( void              * aDumpObjHandle,
                                         void              * aPathCtrl,
                                         knlValueBlockList * aValueList,
                                         stlInt32            aBlockIdx,
                                         stlBool           * aIsExist,
                                         knlEnv            * aKnlEnv )
{
    knxXTablesPathCtrl * sPathCtrl = NULL;

    knxFxTableHeapDesc * sFxTableHeapDesc = NULL;
    
    knxXTablesStructure  sXTablesRecord;
    stlMemset( &sXTablesRecord, 0x00, STL_SIZEOF( knxXTablesStructure ) );
        
    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aPathCtrl  != NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( aIsExist   != NULL, KNL_ERROR_STACK(aKnlEnv) );

    sPathCtrl = aPathCtrl;
    
    /**
     * 지정된 위치가 종료 위치인지 검사
     */
    if ( sPathCtrl->mTabIdx >= gKnxFxTableMgr.mFxTableCount )
    {
        /**
         * 종료 지점일 경우 구성할 레코드가 없음을 알림
         */
        *aIsExist = STL_FALSE;
    }
    else
    {
        
        /**
         * 유효한 정보가 있으면 레코드 정보를 구성하고 
         */

        sFxTableHeapDesc = gKnxFxTableMgr.mFxHeapDescMap[sPathCtrl->mTabIdx];
        
        sXTablesRecord.mTableID        = sPathCtrl->mTableID;
        sXTablesRecord.mTableName      = sFxTableHeapDesc->mTableName;
        sXTablesRecord.mTableComment   = sFxTableHeapDesc->mTableComment;
        sXTablesRecord.mColumnCount    = sFxTableHeapDesc->mColumnCount;
        
        sXTablesRecord.mUsageTypeValue = sFxTableHeapDesc->mUsageType;
        sXTablesRecord.mUsageTypeName
            = (stlChar *) gKnlFxTableUsageTypeString[sFxTableHeapDesc->mUsageType];

        sXTablesRecord.mStartupPhaseValue = sFxTableHeapDesc->mStartupPhase;
        sXTablesRecord.mStartupPhaseName
            = (stlChar *) gPhaseString[sFxTableHeapDesc->mStartupPhase];
        
        /**
         * Value List를 생성한 후
         */
        STL_TRY( knlBuildFxRecord( gXTablesColDesc,
                                   & sXTablesRecord,
                                   aValueList,
                                   aBlockIdx,
                                   aKnlEnv )
                 == STL_SUCCESS );

        *aIsExist = STL_TRUE;
        
        /**
         * 다음 레코드를 구성할 정보의 위치를 지정함.
         * 일반 Table 은 양수값, Fixed Table은 음수값을 가짐 
         */
        sPathCtrl->mTabIdx++;
        sPathCtrl->mTableID--;
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief X$TABLES을 위한 테이블 정의
 * @remarks
 * <BR> 모든 Fixed Table들의 테이블 정보를 획득할 수 있다.
 * <BR> knxFxTableDefinition 자료구조에 대한 Fixed Table 정의 방법으로
 * <BR> List 유형의 자료 구조 표현의 Example임.
 */

knlFxTableDesc gXTablesTabDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,      /**< Basic Fixed Table 임 */
    KNL_STARTUP_PHASE_NONE,             /**< NONE 단계부터 조회 가능해야 함(startup시 조회함) */
    NULL,                               /**< Dump Object Handle 획득 함수 */
    knxOpenXTablesCallback,             /**< X$TABLES Open Callback */
    knxCloseXTablesCallback,            /**< X$TABLES Close Callback */
    knxBuildRecordXTablesCallback,      /**< X$TABLES BuildRecord Callback */
    STL_SIZEOF(knxXTablesPathCtrl),     /**< X$TABLES Path Controller Size */
    "X$TABLES",                         /**< Table Name */
    "fixed table lists",                /**< Table Comment */
    gXTablesColDesc                     /**< X$TABLES 의 컬럼 정보 */
};

/** @} */


/**
 * @ingroup X_COLUMNS
 * @{
 */

/**
 * @brief X$COLUMNS을 위한 컬럼 정의
 * @remarks
 * Dictionary Cache 를 위해 사용하고 있는 정보이므로,
 * 변경이 필요하면 knlFxColumnColumnOrder 의 정의도 변경해야 한다.
 */

knlFxColumnDesc gXColumnsColDesc[] =
{
    {
        "TABLE_ID",
        "table identifier of the column",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( knxXColumnsStructure, mTableID ),
        STL_SIZEOF(stlInt64),
        STL_FALSE  /* nullable */
    },
    {
        "COLUMN_ID",
        "column identifier",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( knxXColumnsStructure, mColumnID ),
        STL_SIZEOF(stlInt64),
        STL_FALSE  /* nullable */
    },
    {
        "TABLE_NAME",
        "fixed table name",
        KNL_FXTABLE_DATATYPE_VARCHAR_POINTER,
        STL_OFFSETOF( knxXColumnsStructure, mTableName ),
        STL_MAX_SQL_IDENTIFIER_LENGTH,
        STL_FALSE  /* nullable */
    },
    {
        "COLUMN_NAME",
        "column name",
        KNL_FXTABLE_DATATYPE_VARCHAR_POINTER,
        STL_OFFSETOF( knxXColumnsStructure, mColumnName ),
        STL_MAX_SQL_IDENTIFIER_LENGTH,
        STL_FALSE  /* nullable */
    },
    {
        "ORDINAL_POSITION",
        "ordinal position of the column in the table",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( knxXColumnsStructure, mOrdinalPosition ),
        STL_SIZEOF( stlInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "DATA_TYPE_ID",
        "data type identifier",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( knxXColumnsStructure, mDataTypeID ),
        STL_SIZEOF(stlInt32),
        STL_FALSE  /* nullable */
    },
    {
        "DATA_TYPE",
        "data type name",
        KNL_FXTABLE_DATATYPE_VARCHAR_POINTER,
        STL_OFFSETOF( knxXColumnsStructure, mDataTypeName ),
        KNX_MAX_BUILTIN_NAMESIZE_DTD_IDENTIFIER,
        STL_FALSE  /* nullable */
    },
    {
        "COLUMN_LENGTH",
        "maximum byte length of the column",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( knxXColumnsStructure, mLength ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "IS_NULLABLE",
        "is nullable column",
        KNL_FXTABLE_DATATYPE_BOOL,
        STL_OFFSETOF( knxXColumnsStructure, mNullable ),
        STL_SIZEOF( stlBool ),
        STL_FALSE  /* nullable */
    },
    {
        "COMMENTS",
        "descriptions of the column",
        KNL_FXTABLE_DATATYPE_VARCHAR_POINTER,
        STL_OFFSETOF( knxXColumnsStructure, mColumnComment ),
        KNL_MAX_FIXEDTABLE_COMMENTS_LENGTH,
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
 * @brief X$COLUMNS를 위한 Open Callback 함수 정의 
 * @remarks
 */
stlStatus knxOpenXColumnsCallback( void   * aStmt,
                                   void   * aDumpObjHandle,
                                   void   * aPathCtrl,
                                   knlEnv * aKnlEnv )
{
    knxXColumnsPathCtrl * sPathCtrl = NULL;

    /**
     * Parameter Validation
     */
    STL_PARAM_VALIDATE( aPathCtrl != NULL, KNL_ERROR_STACK(aKnlEnv) );

    sPathCtrl = aPathCtrl;
    
    /**
     * 첫번째 Record를 구성하기 위해 필요한 정보를 설정한다
     */
    
    sPathCtrl->mTabIdx   = 0;
    sPathCtrl->mColIdx   = 0;
    sPathCtrl->mTableID  = -1;
    sPathCtrl->mColumnID = -1;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief X$COLUMNS을 위한 Close Callback 함수 정의 
 * @remarks
 */
stlStatus knxCloseXColumnsCallback( void   * aDumpObjHandle,
                                    void   * aPathCtrl,
                                    knlEnv * aKnlEnv )
{
    /** 할 일 없음 */
    return STL_SUCCESS;
}

/**
 * @brief X$COLUMNS을 위한 BuildRecord 함수 정의 
 * @remarks
 */
stlStatus knxBuildRecordXColumnsCallback( void              * aDumpObjHandle,
                                          void              * aPathCtrl,
                                          knlValueBlockList * aValueList,
                                          stlInt32            aBlockIdx,
                                          stlBool           * aIsExist,
                                          knlEnv            * aKnlEnv )
{
    dtlDataType            sTypeID = DTL_TYPE_MAX;
    knlFxColumnDesc      * sColDesc  = NULL;
    knxXColumnsPathCtrl  * sPathCtrl = NULL;

    knxFxTableHeapDesc * sFxTableHeapDesc = NULL;
    
    knxXColumnsStructure   sXColumnsRecord;
    stlMemset( &sXColumnsRecord, 0x00, STL_SIZEOF(knxXColumnsStructure) );
    
    /**
     * Parameter Validation
     */
    STL_PARAM_VALIDATE( aPathCtrl != NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( aIsExist != NULL, KNL_ERROR_STACK(aKnlEnv) );
    sPathCtrl = aPathCtrl;

    /**
     * 지정된 위치가 종료 위치인지 검사
     */
    if ( sPathCtrl->mTabIdx >= gKnxFxTableMgr.mFxTableCount )
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

        
        sFxTableHeapDesc = gKnxFxTableMgr.mFxHeapDescMap[sPathCtrl->mTabIdx];
        sColDesc = & sFxTableHeapDesc->mColumnDesc[sPathCtrl->mColIdx];

        sXColumnsRecord.mTableID         = sPathCtrl->mTableID;
        sXColumnsRecord.mColumnID        = sPathCtrl->mColumnID;
        sXColumnsRecord.mTableName       = sFxTableHeapDesc->mTableName;
        sXColumnsRecord.mColumnName      = sColDesc->mColumnName;
        sXColumnsRecord.mOrdinalPosition = sPathCtrl->mColIdx + 1;
        
        STL_TRY( knlGetDataTypeIDOfFixedColumn( sColDesc,
                                                & sTypeID,
                                                aKnlEnv )
                 == STL_SUCCESS );
        sXColumnsRecord.mDataTypeID = sTypeID;
        
        sXColumnsRecord.mDataTypeName
            = (stlChar *) gKnlFxTableDataTypeString[sColDesc->mDataType];
        sXColumnsRecord.mLength          = sColDesc->mLength;
        sXColumnsRecord.mNullable        = sColDesc->mNullable;
        sXColumnsRecord.mColumnComment   = sColDesc->mColumnComment;
        
        /**
         * Value List를 생성한 후
         */
        STL_TRY( knlBuildFxRecord( gXColumnsColDesc,
                                   &sXColumnsRecord,
                                   aValueList,
                                   aBlockIdx,
                                   aKnlEnv )
                 == STL_SUCCESS );

        *aIsExist = STL_TRUE;
        
        /**
         * 다음 레코드를 구성할 위치를 지정함.
         */

        if ( (sPathCtrl->mColIdx + 1) < sFxTableHeapDesc->mColumnCount )
        {
            sPathCtrl->mColIdx++;
            
            /* 일반 Table 은 양수값, Fixed Table은 음수값을 가짐 */
            sPathCtrl->mColumnID--;     
        }
        else
        {
            sPathCtrl->mTabIdx++;
            sPathCtrl->mColIdx = 0;

            /* 일반 Table 은 양수값, Fixed Table은 음수값을 가짐 */
            sPathCtrl->mTableID--;
            sPathCtrl->mColumnID--;     
        }
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief X$COLUMNS을 위한 테이블 정의
 * @remarks
 * <BR> 모든 Fixed Table들의 컬럼 정보를 획득할 수 있다.
 * <BR> 여러가지 자료구조를 참조하고,
 * <BR> 연결관계가 List와 Array가 함께 복합적으로 조합된 자료구조에 대한
 * <BR> Fixed Table 구성 Example이다.
 */

knlFxTableDesc gXColumnsTabDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,      /**< Basic Fixed Table 임 */
    KNL_STARTUP_PHASE_NONE,             /**< NONE 단계부터 조회 가능해야 함(startup시 조회함) */
    NULL,                               /**< Dump Object Handle 획득 함수 */
    knxOpenXColumnsCallback,            /**< X$COLUMNS Open Callback */
    knxCloseXColumnsCallback,           /**< X$COLUMNS Close Callback */
    knxBuildRecordXColumnsCallback,     /**< X$COLUMNS BuildRecord Callback */
    STL_SIZEOF(knxXColumnsPathCtrl),    /**< X$COLUMNS Path Controller Size */
    "X$COLUMNS",                        /**< Table Name */
    "column lists of all fixed tables", /**< Table Comment */
    gXColumnsColDesc                    /**< X$COLUMNS 의 컬럼 정보 */
};

/** @} */







/**
 * @addtogroup X_ERROR_CODE
 * @internal
 * @{
 */

/**
 * @brief X$ERROR_CODE을 위한 컬럼 정의
 * @remarks
 */

knlFxColumnDesc gXErrCodeColDesc[] =
{
    {
        "ERROR_CODE",
        "internal error code",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( knxXErrCodeStructure, mErrorCode ),
        STL_SIZEOF(stlInt32),
        STL_FALSE  /* nullable */
    },
    {
        "SQL_STATE",
        "standard SQL state code",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( knxXErrCodeStructure, mSQLState ),
        STL_SQLSTATE_LENGTH,
        STL_FALSE  /* nullable */
    },
    {
        "MESSAGE",
        "error message string",
        KNL_FXTABLE_DATATYPE_VARCHAR_POINTER,
        STL_OFFSETOF( knxXErrCodeStructure, mMessage ),
        STL_MAX_ERROR_MESSAGE,
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
 * @brief X$ERROR_CODE 을 위한 Open Callback 함수 정의 
 * @remarks
 */
stlStatus knxOpenXErrCodeCallback( void   * aStmt,
                                   void   * aDumpObjHandle,
                                   void   * aPathCtrl,
                                   knlEnv * aKnlEnv )
{
    knxXErrCodePathCtrl * sPathCtrl = NULL;

    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aPathCtrl != NULL, KNL_ERROR_STACK(aKnlEnv) );

    sPathCtrl = aPathCtrl;
    
    /**
     * 첫번째 Record를 구성하기 위해 필요한 정보를 설정한다
     */
    
    sPathCtrl->mLayerNO = STL_LAYER_NONE + 1;
    sPathCtrl->mErrorNO = 0;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief X$ERROR_CODE 을 위한 Close Callback 함수 정의 
 * @remarks
 */
stlStatus knxCloseXErrCodeCallback( void   * aDumpObjHandle,
                                    void   * aPathCtrl,
                                    knlEnv * aKnlEnv )
{
    /**
     * 할 일 없음
     */
    
    return STL_SUCCESS;
}


/**
 * @brief X$ERROR_CODE 을 위한 BuildRecord 함수 정의 
 * @remarks
 */
stlStatus knxBuildRecordXErrCodeCallback( void              * aDumpObjHandle,
                                          void              * aPathCtrl,
                                          knlValueBlockList * aValueList,
                                          stlInt32            aBlockIdx,
                                          stlBool           * aIsExist,
                                          knlEnv            * aKnlEnv )
{
    stlErrorRecord  * sErrLayer  = NULL;
    stlErrorRecord  * sErrRecord = NULL;

    stlInt32          sErrorCode = 0;
    stlInt32          sExternCode = 0;
    
    knxXErrCodePathCtrl  * sPathCtrl = NULL;

    knxXErrCodeStructure   sFxRecord;
    stlMemset( & sFxRecord, 0x00, STL_SIZEOF(knxXErrCodeStructure) );

    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aPathCtrl != NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( aIsExist != NULL, KNL_ERROR_STACK(aKnlEnv) );
    sPathCtrl = aPathCtrl;

    /**
     * 지정된 위치가 종료 위치인지 검사
     */
    
    if ( sPathCtrl->mLayerNO == STL_LAYER_MAX )
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

        sErrLayer = gLayerErrorTables[sPathCtrl->mLayerNO];
        
        sErrorCode = STL_MAKE_ERRCODE( sPathCtrl->mLayerNO, sPathCtrl->mErrorNO );
        sErrRecord  = & sErrLayer[sPathCtrl->mErrorNO];
        sExternCode = sErrRecord->mExternalErrorCode;
        
        sFxRecord.mErrorCode = sErrorCode;
        stlMakeSqlState( sExternCode, sFxRecord.mSQLState );
        sFxRecord.mMessage   = sErrRecord->mErrorMessage;
        
        /**
         * Value List를 생성한 후
         */
        STL_TRY( knlBuildFxRecord( gXErrCodeColDesc,
                                   & sFxRecord,
                                   aValueList,
                                   aBlockIdx,
                                   aKnlEnv )
                 == STL_SUCCESS );

        *aIsExist = STL_TRUE;
        
        /**
         * 다음 레코드를 구성할 위치를 지정함.
         */

        sPathCtrl->mErrorNO++;
        sErrRecord  = & sErrLayer[sPathCtrl->mErrorNO];

        if ( sErrRecord->mErrorMessage != NULL )
        {
            /* nothing to do */
        }
        else
        {
            sPathCtrl->mLayerNO++;
            sPathCtrl->mErrorNO = 0;

            while ( (sPathCtrl->mLayerNO < STL_LAYER_MAX) &&
                    (gLayerErrorTables[sPathCtrl->mLayerNO] == NULL) )
            {
                sPathCtrl->mLayerNO++;
            }
        }
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief X$ERROR_CODE 을 위한 테이블 정의
 * @remarks
 */

knlFxTableDesc gXErrCodeTabDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,              /**< Basic Fixed Table 임 */
    KNL_STARTUP_PHASE_NO_MOUNT,                 /**< NO_MOUNT 단계부터 조회 가능 */
    NULL,                                       /**< Dump Object Handle 획득 함수 */
    knxOpenXErrCodeCallback,                    /**< Open Callback */
    knxCloseXErrCodeCallback,                   /**< Close Callback */
    knxBuildRecordXErrCodeCallback,             /**< BuildRecord Callback */
    STL_SIZEOF(knxXErrCodePathCtrl),            /**< Path Controller Size */
    "X$ERROR_CODE",                             /**< Table Name */
    "internal error code information",          /**< Table Comment */
    gXErrCodeColDesc                            /**< 컬럼 정보 */
};


/** @} X_ERROR_CODE */
