/*******************************************************************************
 * zldDiag.c
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

#include <cml.h>
#include <goldilocks.h>
#include <zlDef.h>
#include <zle.h>
#include <zld.h>

#include <zllConstDef.h>
#include <zlvCharacterset.h>

/**
 * @file zldDiag.c
 * @brief Gliese API Internal Diagnostic Routines.
 */

/**
 * @addtogroup zld
 * @{
 */

stlChar * gZldDiagAlterDomain =         "ALTER DOMAIN";
stlChar * gZldDiagAlterTable =          "ALTER TABLE";
stlChar * gZldDiagCall =                "CALL";
stlChar * gZldDiagCreateAssertion =     "CREATE ASSERTION";
stlChar * gZldDiagCreateCharacterSet =  "CREATE CHARACTER SET";
stlChar * gZldDiagCreateCollation =     "CREATE COLLATION";
stlChar * gZldDiagCreateDomain =        "CREATE DOMAIN";
stlChar * gZldDiagCreateIndex =         "CREATE INDEX";
stlChar * gZldDiagCreateSchema =        "CREATE SCHEMA";
stlChar * gZldDiagCreateTable =         "CREATE TABLE";
stlChar * gZldDiagCreateTranslation =   "CREATE TRANSLATION";
stlChar * gZldDiagCreateView =          "CREATE VIEW";
stlChar * gZldDiagDeleteWhere =         "DELETE WHERE";
stlChar * gZldDiagDropAssertion =       "DROP ASSERTION";
stlChar * gZldDiagDropCharacterSet =    "DROP CHARACTER SET";
stlChar * gZldDiagDropCollation =       "DROP COLLATION";
stlChar * gZldDiagDropDomain =          "DROP DOMAIN";
stlChar * gZldDiagDropIndex =           "DROP INDEX";
stlChar * gZldDiagDropSchema =          "DROP SCHEMA";
stlChar * gZldDiagDropTable =           "DROP TABLE";
stlChar * gZldDiagDropTranslation =     "DROP TRANSLATION";
stlChar * gZldDiagDropView =            "DROP VIEW";
stlChar * gZldDiagDynamicDeleteCursor = "DYNAMIC DELETE CURSOR";
stlChar * gZldDiagDynamicUpdateCursor = "DYNAMIC UPDATE CURSOR";
stlChar * gZldDiagGrant =               "GRANT";
stlChar * gZldDiagInsert =              "INSERT";
stlChar * gZldDiagRevoke =              "REVOKE";
stlChar * gZldDiagSelectCursor =        "SELECT CURSOR";
stlChar * gZldDiagUpdateWhere =         "UPDATE WHERE";
stlChar * gZldDiagUnknown =             "";

stlChar * gZldDiagEmptyString =         "";

#define ZLD_DIAG_INIT_HEADER( aDiag )                                   \
    STL_RING_INIT_HEADLINK( &(aDiag)->mDiagRing,                        \
                            STL_OFFSETOF( zldDiagElement, mLink ) );    \
    (aDiag)->mSpinLock = 0;                                             \
    (aDiag)->mCurrentRec = 0;                                           \
    (aDiag)->mCursorRowCount = 0;                                       \
    (aDiag)->mDynamicFunction = gZldDiagUnknown;                        \
    (aDiag)->mDynamicFunctionCode = SQL_DIAG_UNKNOWN_STATEMENT;         \
    (aDiag)->mNumber = 0;                                               \
    (aDiag)->mReturnCode = SQL_SUCCESS;                                 \
    (aDiag)->mRowCount = -1

#define ZLD_DIAG_INIT_REC( aDiagRec )                       \
    (aDiagRec)->mClassOrigin[0] = 0;                        \
    (aDiagRec)->mColumnNumber = SQL_NO_COLUMN_NUMBER;       \
    (aDiagRec)->mConnectionName = gZldDiagEmptyString;      \
    (aDiagRec)->mMessageText = gZldDiagEmptyString;         \
    (aDiagRec)->mNative = 0;                                \
    (aDiagRec)->mRowNumber = SQL_NO_ROW_NUMBER;             \
    (aDiagRec)->mServerName = gZldDiagEmptyString;          \
    (aDiagRec)->mSqlState[0] = 0;                           \
    (aDiagRec)->mSubClassOrigin[0] = 0

stlStatus zldDiagInit( stlInt16        aHandleType,
                       void          * aHandle,
                       stlErrorStack * aErrorStack )
{
    zldDiag * sDiag = NULL;

    switch( aHandleType )
    {
        case SQL_HANDLE_ENV :
            sDiag = &((zlnEnv*)aHandle)->mDiag;
            break;
        case SQL_HANDLE_DBC :
            sDiag = &((zlcDbc*)aHandle)->mDiag;
            break;
        case SQL_HANDLE_STMT :
            sDiag = &((zlsStmt*)aHandle)->mDiag;
            break;
        case SQL_HANDLE_DESC :
            sDiag = &((zldDesc*)aHandle)->mDiag;
            break;
        default :
            STL_ASSERT( STL_FALSE );
            break;
    }

    STL_TRY( stlCreateRegionAllocator( &sDiag->mRegionMem,
                                       ZL_REGION_INIT_SIZE,
                                       ZL_REGION_NEXT_SIZE,
                                       aErrorStack ) == STL_SUCCESS );

    ZLD_DIAG_INIT_HEADER( sDiag );
                                 
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zldDiagFini( stlInt16        aHandleType,
                       void          * aHandle,
                       stlErrorStack * aErrorStack )
{
    zldDiag * sDiag = NULL;

    switch( aHandleType )
    {
        case SQL_HANDLE_ENV :
            sDiag = &((zlnEnv*)aHandle)->mDiag;
            break;
        case SQL_HANDLE_DBC :
            sDiag = &((zlcDbc*)aHandle)->mDiag;
            break;
        case SQL_HANDLE_STMT :
            sDiag = &((zlsStmt*)aHandle)->mDiag;
            break;
        case SQL_HANDLE_DESC :
            sDiag = &((zldDesc*)aHandle)->mDiag;
            break;
        default :
            STL_ASSERT( STL_FALSE );
            break;
    }

    STL_TRY( stlDestroyRegionAllocator( &sDiag->mRegionMem,
                                        aErrorStack ) == STL_SUCCESS );

    ZLD_DIAG_INIT_HEADER( sDiag );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

static stlStatus zldDiagAdd( stlInt16        aHandleType,
                             void          * aHandle,
                             stlInt64        aRowNumber,
                             stlInt32        aColumnNumber,
                             stlChar       * aExtraMessage,
                             stlErrorData  * aErrorData,
                             stlErrorStack * aErrorStack )
{
    zldDiagElement      * sDiagRec;
    stlUInt32             sExternalErrorCode;
    stlSize               sServerLen;
    zldDiag             * sDiag = NULL;
    zlcDbc              * sDbc = NULL;
    zlsStmt             * sStmt = NULL;
    zlvConvertErrorFunc   sErrorFunc = zlvNonConvertError;

    switch( aHandleType )
    {
        case SQL_HANDLE_ENV :
            sDiag = &((zlnEnv*)aHandle)->mDiag;
            sErrorFunc = zlvNonConvertError;
            break;
        case SQL_HANDLE_DBC :
            sDbc = (zlcDbc*)aHandle;
            sDiag = &sDbc->mDiag;
            sErrorFunc = zlvNonConvertError;
            break;
        case SQL_HANDLE_STMT :
            sStmt = (zlsStmt*)aHandle;
            sDbc = sStmt->mParentDbc;
            sDiag = &sStmt->mDiag;
            if( sDbc != NULL )
            {
                sErrorFunc = sDbc->mConvertErrorFunc;
            }
            break;
        case SQL_HANDLE_DESC :
            sStmt = ((zldDesc*)aHandle)->mStmt;
            sDbc = ZLS_STMT_DBC(sStmt);
            sDiag = &((zldDesc*)aHandle)->mDiag;
            if( sDbc != NULL )
            {
                sErrorFunc = sDbc->mConvertErrorFunc;
            }
            break;
        default :
            STL_ASSERT( STL_FALSE );
            break;
    }

    STL_TRY( stlAllocRegionMem( &sDiag->mRegionMem,
                                STL_SIZEOF( zldDiagElement ),
                                (void**)&sDiagRec,
                                aErrorStack ) == STL_SUCCESS );

    STL_RING_INIT_DATALINK( ( sDiagRec ), STL_OFFSETOF( zldDiagElement, mLink ) );

    ZLD_DIAG_INIT_REC( sDiagRec );

    sDiagRec->mRowNumber    = aRowNumber;
    sDiagRec->mColumnNumber = aColumnNumber;
    
    sExternalErrorCode = stlGetExternalErrorCode( aErrorData );
    switch( sExternalErrorCode )
    {
        case STL_EXT_ERRCODE_WARNING_INVALID_CONNECTION_STRING_ATTRIBUTE :
        case STL_EXT_ERRCODE_WARNING_OPTION_VALUE_CHANGED :
        case STL_EXT_ERRCODE_WARNING_ATTEMPT_TO_FETCH_BEFORE_THE_RESULT_SET_RETURNED_THE_FIRST_ROWSET :
        case STL_EXT_ERRCODE_WARNING_FRACTIONAL_TRUNCATION :
        case STL_EXT_ERRCODE_DYNAMIC_SQL_ERROR_INVALID_USE_OF_DEFAULT_PARAMETER :
        case STL_EXT_ERRCODE_CONNECTION_EXCEPTION_COMMUNICATION_LINK_FAILURE :
        case STL_EXT_ERRCODE_CARDINALITY_VIOLATION_INSERT_VALUE_LIST_DOES_NOT_MATCH_COLUMN_LIST :
        case STL_EXT_ERRCODE_CARDINALITY_VIOLATION_DEGREE_OF_DERIVED_TABLE_DOES_NOT_MATCH_COLUMN_LIST :
        case STL_EXT_ERRCODE_INVALID_TRANSACTION_STATE_TRANSACTION_STATE :
        case STL_EXT_ERRCODE_INVALID_TRANSACTION_STATE_TRANSACTION_IS_STILL_ACTIVE :
        case STL_EXT_ERRCODE_INVALID_TRANSACTION_STATE_TRANSACTION_IS_ROLLED_BACK :
        case STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_BASE_TABLE_OR_VIEW_ALREADY_EXISTS :
        case STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_BASE_TABLE_OR_VIEW_NOT_FOUND :
        case STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_INDEX_ALREADY_EXISTS :
        case STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_INDEX_NOT_FOUND :
        case STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_COLUMN_ALREADY_EXISTS :
        case STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_COLUMN_NOT_FOUND :
        case STL_EXT_ERRCODE_CLI_SPECIFIC_CONDITION_INVALID_FUNCTION_ID_SPECIFIED :
        case STL_EXT_ERRCODE_CLI_SPECIFIC_CONDITION_COLUMN_TYPE_OUT_OF_RANGE :
        case STL_EXT_ERRCODE_CLI_SPECIFIC_CONDITION_SCOPE_OUT_OF_RANGE :
        case STL_EXT_ERRCODE_CLI_SPECIFIC_CONDITION_NULLABLE_TYPE_OUT_OF_RANGE :
        case STL_EXT_ERRCODE_CLI_SPECIFIC_CONDITION_UNIQUENESS_OPTION_TYPE_OUT_OF_RANGE :
        case STL_EXT_ERRCODE_CLI_SPECIFIC_CONDITION_ACCURACY_OPTION_TYPE_OUT_OF_RANGE :
        case STL_EXT_ERRCODE_CLI_SPECIFIC_CONDITION_INVALID_PARAMETER_MODE :
        case STL_EXT_ERRCODE_CLI_SPECIFIC_CONDITION_ROW_VALUE_OUT_OF_RANGE :
        case STL_EXT_ERRCODE_CLI_SPECIFIC_CONDITION_INVALID_CURSOR_POSITION :
        case STL_EXT_ERRCODE_CLI_SPECIFIC_CONDITION_INVALID_DRIVER_COMPLETION :
        case STL_EXT_ERRCODE_CLI_SPECIFIC_CONDITION_INVALID_BOOKMARK_VALUE :
        case STL_EXT_ERRCODE_CLI_SPECIFIC_CONDITION_TIMEOUT_EXPIRED :
        case STL_EXT_ERRCODE_CLI_SPECIFIC_CONDITION_CONNECTION_TIMEOUT_EXPIRED :
        case STL_EXT_ERRCODE_DRIVER_DOES_NOT_SUPPORT_THIS_FUNCTION :
        case STL_EXT_ERRCODE_DATA_SOURCE_NAME_NOT_FOUND_AND_NO_DEFAULT_DRIVER_SPECIFIED :
        case STL_EXT_ERRCODE_SPECIFIED_DRIVER_COULD_NOT_BE_LOADED :
        case STL_EXT_ERRCODE_DRIVERS_SQLALLOCHANDLE_ON_SQL_HANDLE_ENV_FAILED :
        case STL_EXT_ERRCODE_DRIVERS_SQLALLOCHANDLE_ON_SQL_HANDLE_DBC_FAILED :
        case STL_EXT_ERRCODE_DRIVERS_SQLSETCONNECTATTR_FAILED :
        case STL_EXT_ERRCODE_NO_DATA_SOURCE_OR_DRIVER_SPECIFIED_DIALOG_PROHIBITED :
        case STL_EXT_ERRCODE_DIAGLOG_FAILED :
        case STL_EXT_ERRCODE_UNABLE_TO_LOAD_TRNSLATION_DLL :
        case STL_EXT_ERRCODE_DATA_SOURCE_NAME_TOO_LONG :
        case STL_EXT_ERRCODE_DRIVER_NAME_TOO_LONG :
        case STL_EXT_ERRCODE_DRIVER_KEYWORD_SYNTAX_ERROR :
            stlStrcpy( sDiagRec->mClassOrigin, "ODBC 3.0" );
            stlStrcpy( sDiagRec->mSubClassOrigin, "ODBC 3.0" );
            break;
        default :
            stlStrcpy( sDiagRec->mClassOrigin, "ISO 9075" );
            stlStrcpy( sDiagRec->mSubClassOrigin, "ISO 9075" );
            break;
    }

    sDiagRec->mNative = (stlInt32)aErrorData->mErrorCode;
    stlMakeSqlState( sExternalErrorCode, sDiagRec->mSqlState );

    STL_TRY( sErrorFunc( sStmt,
                         aErrorData,
                         aExtraMessage,
                         sDiag,
                         sDiagRec,
                         aErrorStack ) == STL_SUCCESS );

    switch( aHandleType )
    {
        case SQL_HANDLE_STMT :
            if( (sDbc != NULL) && (sDbc->mServerName != NULL) )
            {
                sServerLen = stlStrlen( sDbc->mServerName );

                STL_TRY( stlAllocRegionMem( &sDiag->mRegionMem,
                                            sServerLen + 1,
                                            (void**)&sDiagRec->mServerName,
                                            aErrorStack ) == STL_SUCCESS );
                stlStrcpy( sDiagRec->mServerName, sDbc->mServerName );
            }
            break;
        case SQL_HANDLE_DBC :
            if( sDbc->mServerName != NULL )
            {
                sServerLen = stlStrlen( sDbc->mServerName );

                STL_TRY( stlAllocRegionMem( &sDiag->mRegionMem,
                                            sServerLen + 1,
                                            (void**)&sDiagRec->mServerName,
                                            aErrorStack ) == STL_SUCCESS );
                stlStrcpy( sDiagRec->mServerName, sDbc->mServerName );

                STL_TRY( stlAllocRegionMem( &sDiag->mRegionMem,
                                            sServerLen + 1,
                                            (void**)&sDiagRec->mConnectionName,
                                            aErrorStack ) == STL_SUCCESS );
                stlStrcpy( sDiagRec->mConnectionName, sDbc->mServerName );
            }
            break;
    }

    STL_RING_ADD_LAST( &sDiag->mDiagRing, sDiagRec );
    sDiag->mNumber++;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zldDiagAdds( stlInt16        aHandleType,
                       void          * aHandle,
                       stlInt64        aRowNumber,
                       stlInt32        aColumnNumber,
                       stlErrorStack * aErrorStack )
{
    zldDiag      * sDiag = NULL;
    stlErrorData * sErrorData;
    stlInt16       sOldState;
    stlBool        sHasLock = STL_FALSE;
    zlcDbc       * sDbc = NULL;

    switch( aHandleType )
    {
        case SQL_HANDLE_ENV :
            sDiag = &((zlnEnv*)aHandle)->mDiag;
            sDbc = NULL;
            break;
        case SQL_HANDLE_DBC :
            sDiag = &((zlcDbc*)aHandle)->mDiag;
            sDbc = (zlcDbc*)aHandle;
            break;
        case SQL_HANDLE_STMT :
            sDiag = &((zlsStmt*)aHandle)->mDiag;
            sDbc = ZLS_STMT_DBC( (zlsStmt*)aHandle );
            break;
        case SQL_HANDLE_DESC :
            sDiag = &((zldDesc*)aHandle)->mDiag;
            sDbc = ZLS_STMT_DBC( ((zldDesc*)aHandle)->mStmt );
            break;
        default :
            STL_ASSERT( STL_FALSE );
            break;
    }

    sErrorData = stlGetLastErrorData( aErrorStack );

    STL_TRY_THROW( sErrorData != NULL, RAMP_SUCCESS );

    while( 1 )
    {
        sOldState = stlAtomicCas32( &sDiag->mSpinLock, 1, 0 );

        if( sOldState == 0 )
        {
            sHasLock = STL_TRUE;
            break;
        }
    }
    
    sErrorData = stlPopError( aErrorStack );
    
    while( sErrorData != NULL )
    {
        /**
         * DBC 에 Communication Link Failure 설정
         */
        if( stlGetExternalErrorCode( sErrorData ) ==
            STL_EXT_ERRCODE_CONNECTION_EXCEPTION_COMMUNICATION_LINK_FAILURE )
        {
            if( sDbc != NULL )
            {
                sDbc->mBrokenConnection = STL_TRUE;
            }
        }
    
        STL_TRY( zldDiagAdd( aHandleType,
                             aHandle,
                             aRowNumber,
                             aColumnNumber,
                             NULL,
                             sErrorData,
                             aErrorStack )
                 == STL_SUCCESS );

        sErrorData = stlPopError( aErrorStack );
    }

    sHasLock = STL_FALSE;
    sDiag->mSpinLock = 0;

    STL_RAMP( RAMP_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    if( sHasLock == STL_TRUE )
    {
        sDiag->mSpinLock = 0;
    }

    return STL_FAILURE;
}


stlStatus zldDiagClear( stlInt16        aHandleType,
                        void          * aHandle,
                        stlErrorStack * aErrorStack )
{
    zldDiag  * sDiag = NULL;
    
    switch( aHandleType )
    {
        case SQL_HANDLE_ENV :
            sDiag = &((zlnEnv*)aHandle)->mDiag;
            break;
        case SQL_HANDLE_DBC :
            sDiag = &((zlcDbc*)aHandle)->mDiag;
            break;
        case SQL_HANDLE_STMT :
            sDiag = &((zlsStmt*)aHandle)->mDiag;
            break;
        case SQL_HANDLE_DESC :
            sDiag = &((zldDesc*)aHandle)->mDiag;
            break;
        default :
            STL_ASSERT( STL_FALSE );
            break;
    }

    STL_TRY_THROW( sDiag->mNumber != 0, RAMP_SUCCESS );

    STL_TRY( stlClearRegionMem( &sDiag->mRegionMem,
                                aErrorStack ) == STL_SUCCESS );

    ZLD_DIAG_INIT_HEADER( sDiag );

    STL_RAMP( RAMP_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

void zldDiagSetRetCode( stlInt16   aHandleType,
                        void     * aHandle,
                        stlInt16   aRetCode )
{
    zldDiag * sDiag = NULL;

    switch( aHandleType )
    {
        case SQL_HANDLE_ENV :
            sDiag = &((zlnEnv*)aHandle)->mDiag;
            break;
        case SQL_HANDLE_DBC :
            sDiag = &((zlcDbc*)aHandle)->mDiag;
            break;
        case SQL_HANDLE_STMT :
            sDiag = &((zlsStmt*)aHandle)->mDiag;
            break;
        case SQL_HANDLE_DESC :
            sDiag = &((zldDesc*)aHandle)->mDiag;
            break;
        default :
            STL_ASSERT( STL_FALSE );
            break;
    }

    sDiag->mReturnCode = aRetCode;
}

void zldDiagSetRowCount( zldDiag  * aDiag,
                         stlInt64   aRowCount )
{
    aDiag->mRowCount = aRowCount;
}

void zldDiagSetCursorRowCount( zldDiag  * aDiag,
                               stlInt64   aFetchedRowCount )
{
    aDiag->mCursorRowCount = aFetchedRowCount;
}

void zldDiagSetDynamicFunctionCode( zldDiag  * aDiag,
                                    stlInt32   aStatementType )
{
    switch( aStatementType )
    {
        case ZLL_STMT_TYPE_ALTER_DOMAIN :
            aDiag->mDynamicFunction = gZldDiagAlterDomain;
            aDiag->mDynamicFunctionCode = SQL_DIAG_ALTER_DOMAIN;
            break;
        case ZLL_STMT_TYPE_ALTER_TABLE :
            aDiag->mDynamicFunction = gZldDiagAlterTable;
            aDiag->mDynamicFunctionCode = SQL_DIAG_ALTER_TABLE;
            break;
        case ZLL_STMT_TYPE_CALL :
            aDiag->mDynamicFunction = gZldDiagCall;
            aDiag->mDynamicFunctionCode = SQL_DIAG_CALL;
            break;
        case ZLL_STMT_TYPE_CREATE_ASSERTION :
            aDiag->mDynamicFunction = gZldDiagCreateAssertion;
            aDiag->mDynamicFunctionCode = SQL_DIAG_CREATE_ASSERTION;
            break;
        case ZLL_STMT_TYPE_CREATE_CHARACTER_SET :
            aDiag->mDynamicFunction = gZldDiagCreateCharacterSet;
            aDiag->mDynamicFunctionCode = SQL_DIAG_CREATE_CHARACTER_SET;
            break;
        case ZLL_STMT_TYPE_CREATE_COLLATION :
            aDiag->mDynamicFunction = gZldDiagCreateCollation;
            aDiag->mDynamicFunctionCode = SQL_DIAG_CREATE_COLLATION;
            break;
        case ZLL_STMT_TYPE_CREATE_DOMAIN :
            aDiag->mDynamicFunction = gZldDiagCreateDomain;
            aDiag->mDynamicFunctionCode = SQL_DIAG_CREATE_DOMAIN;
            break;
        case ZLL_STMT_TYPE_CREATE_INDEX :
            aDiag->mDynamicFunction = gZldDiagCreateIndex;
            aDiag->mDynamicFunctionCode = SQL_DIAG_CREATE_INDEX;
            break;
        case ZLL_STMT_TYPE_CREATE_SCHEMA :
            aDiag->mDynamicFunction = gZldDiagCreateSchema;
            aDiag->mDynamicFunctionCode = SQL_DIAG_CREATE_SCHEMA;
            break;
        case ZLL_STMT_TYPE_CREATE_TABLE :
            aDiag->mDynamicFunction = gZldDiagCreateTable;
            aDiag->mDynamicFunctionCode = SQL_DIAG_CREATE_TABLE;
            break;
        case ZLL_STMT_TYPE_CREATE_TRANSLATION :
            aDiag->mDynamicFunction = gZldDiagCreateTranslation;
            aDiag->mDynamicFunctionCode = SQL_DIAG_CREATE_TRANSLATION;
            break;
        case ZLL_STMT_TYPE_CREATE_VIEW :
            aDiag->mDynamicFunction = gZldDiagCreateView;
            aDiag->mDynamicFunctionCode = SQL_DIAG_CREATE_VIEW;
            break;
        case ZLL_STMT_TYPE_DELETE_WHERE :
            aDiag->mDynamicFunction = gZldDiagDeleteWhere;
            aDiag->mDynamicFunctionCode = SQL_DIAG_DELETE_WHERE;
            break;
        case ZLL_STMT_TYPE_DROP_ASSERTION :
            aDiag->mDynamicFunction = gZldDiagDropAssertion;
            aDiag->mDynamicFunctionCode = SQL_DIAG_DROP_ASSERTION;
            break;
        case ZLL_STMT_TYPE_DROP_CHARACTER_SET :
            aDiag->mDynamicFunction = gZldDiagDropCharacterSet;
            aDiag->mDynamicFunctionCode = SQL_DIAG_DROP_CHARACTER_SET;
            break;
        case ZLL_STMT_TYPE_DROP_COLLATION :
            aDiag->mDynamicFunction = gZldDiagDropCollation;
            aDiag->mDynamicFunctionCode = SQL_DIAG_DROP_COLLATION;
            break;
        case ZLL_STMT_TYPE_DROP_DOMAIN :
            aDiag->mDynamicFunction = gZldDiagDropDomain;
            aDiag->mDynamicFunctionCode = SQL_DIAG_DROP_DOMAIN;
            break;
        case ZLL_STMT_TYPE_DROP_INDEX :
            aDiag->mDynamicFunction = gZldDiagDropIndex;
            aDiag->mDynamicFunctionCode = SQL_DIAG_DROP_INDEX;
            break;
        case ZLL_STMT_TYPE_DROP_SCHEMA :
            aDiag->mDynamicFunction = gZldDiagDropSchema;
            aDiag->mDynamicFunctionCode = SQL_DIAG_DROP_SCHEMA;
            break;
        case ZLL_STMT_TYPE_DROP_TABLE :
            aDiag->mDynamicFunction = gZldDiagDropTable;
            aDiag->mDynamicFunctionCode = SQL_DIAG_DROP_TABLE;
            break;
        case ZLL_STMT_TYPE_DROP_TRANSLATION :
            aDiag->mDynamicFunction = gZldDiagDropTranslation;
            aDiag->mDynamicFunctionCode = SQL_DIAG_DROP_TRANSLATION;
            break;
        case ZLL_STMT_TYPE_DROP_VIEW :
            aDiag->mDynamicFunction = gZldDiagDropView;
            aDiag->mDynamicFunctionCode = SQL_DIAG_DROP_VIEW;
            break;
        case ZLL_STMT_TYPE_DYNAMIC_DELETE_CURSOR :
            aDiag->mDynamicFunction = gZldDiagDynamicDeleteCursor;
            aDiag->mDynamicFunctionCode = SQL_DIAG_DYNAMIC_DELETE_CURSOR;
            break;
        case ZLL_STMT_TYPE_DYNAMIC_UPDATE_CURSOR :
            aDiag->mDynamicFunction = gZldDiagDynamicUpdateCursor;
            aDiag->mDynamicFunctionCode = SQL_DIAG_DYNAMIC_UPDATE_CURSOR;
            break;
        case ZLL_STMT_TYPE_GRANT :
            aDiag->mDynamicFunction = gZldDiagGrant;
            aDiag->mDynamicFunctionCode = SQL_DIAG_GRANT;
            break;
        case ZLL_STMT_TYPE_INSERT :
            aDiag->mDynamicFunction = gZldDiagInsert;
            aDiag->mDynamicFunctionCode = SQL_DIAG_INSERT;
            break;
        case ZLL_STMT_TYPE_REVOKE :
            aDiag->mDynamicFunction = gZldDiagRevoke;
            aDiag->mDynamicFunctionCode = SQL_DIAG_REVOKE;
            break;
        case ZLL_STMT_TYPE_SELECT_CURSOR :
            aDiag->mDynamicFunction = gZldDiagSelectCursor;
            aDiag->mDynamicFunctionCode = SQL_DIAG_SELECT_CURSOR;
            break;
        case ZLL_STMT_TYPE_UPDATE_WHERE :
            aDiag->mDynamicFunction = gZldDiagUpdateWhere;
            aDiag->mDynamicFunctionCode = SQL_DIAG_UPDATE_WHERE;
            break;
        default :
            aDiag->mDynamicFunction = gZldDiagUnknown;
            aDiag->mDynamicFunctionCode = SQL_DIAG_UNKNOWN_STATEMENT;
            break;
    }
}

stlStatus zldDiagGetRec( zldDiag  * aDiag,
                         stlInt16   aRecNumber,
                         stlChar  * aSQLState,
                         stlInt32 * aNativeError,
                         stlChar  * aMessage,
                         stlInt16   aBufferLen,
                         stlInt16 * aTextLen )
{
    stlInt16         sIdx;
    zldDiagElement * sDiagRec;

    sIdx = 1;
    STL_RING_FOREACH_ENTRY( &aDiag->mDiagRing, sDiagRec )
    {
        if( sIdx == aRecNumber )
        {
            break;
        }

        sIdx++;
    }

    if( aSQLState != NULL )
    {
        stlStrcpy( aSQLState, sDiagRec->mSqlState );
    }

    if( aNativeError != NULL )
    {
        *aNativeError = sDiagRec->mNative;
    }

    if( aMessage != NULL )
    {
        stlStrncpy( aMessage, sDiagRec->mMessageText, aBufferLen );
    }

    if( aTextLen != NULL )
    {
        *aTextLen = (stlInt16)stlStrlen( sDiagRec->mMessageText );
    }

    return STL_SUCCESS;
}

stlStatus zldDiagGetRecW( zldDiag         * aDiag,
                          stlInt16          aRecNumber,
                          dtlCharacterSet   aCharacterSet,
                          SQLWCHAR        * aSQLState,
                          stlInt32        * aNativeError,
                          SQLWCHAR        * aMessage,
                          stlInt16          aBufferLen,
                          stlInt16        * aTextLen,
                          stlErrorStack   * aErrorStack )
{
    stlInt16         sIdx;
    zldDiagElement * sDiagRec;
    stlInt64         sLength = 0;

    sIdx = 1;
    STL_RING_FOREACH_ENTRY( &aDiag->mDiagRing, sDiagRec )
    {
        if( sIdx == aRecNumber )
        {
            break;
        }

        sIdx++;
    }

    if( aSQLState != NULL )
    {
        STL_TRY( zlvCharToWchar( aCharacterSet,
                                 (SQLCHAR*)sDiagRec->mSqlState,
                                 STL_SQLSTATE_LENGTH,
                                 aSQLState,
                                 STL_SQLSTATE_LENGTH + 1,
                                 &sLength,
                                 aErrorStack ) == STL_SUCCESS );
    }

    if( aNativeError != NULL )
    {
        *aNativeError = sDiagRec->mNative;
    }

    STL_TRY( zlvCharToWchar( aCharacterSet,
                             (SQLCHAR*)sDiagRec->mMessageText,
                             STL_NTS,
                             aMessage,
                             aBufferLen,
                             &sLength,
                             aErrorStack ) == STL_SUCCESS );

    if( aTextLen != NULL )
    {
        *aTextLen = sLength;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zldDiagGetField( zldDiag  * aDiag,
                           stlInt16   aRecNumber,
                           stlInt16   aDiagId,
                           void     * aDiagInfo,
                           stlInt16   aBufferLen,
                           stlInt16 * aStringLen )
{
    stlInt16         sIdx;
    zldDiagElement * sDiagRec;
    SQLLEN           sCount;

    switch( aDiagId )
    {
        case SQL_DIAG_CURSOR_ROW_COUNT :
            sCount = (SQLLEN)aDiag->mCursorRowCount;
            if( aDiagInfo != NULL )
            {
                stlMemcpy( aDiagInfo, &sCount, STL_SIZEOF( SQLLEN ) );
            }
            break;
        case SQL_DIAG_DYNAMIC_FUNCTION :
            if( aDiagInfo != NULL )
            {
                stlStrncpy( (stlChar*)aDiagInfo, aDiag->mDynamicFunction, aBufferLen );
            }
            if( aStringLen != NULL )
            {
                *aStringLen = (stlInt16)stlStrlen( aDiag->mDynamicFunction );
            }
            break;
        case SQL_DIAG_DYNAMIC_FUNCTION_CODE :
            if( aDiagInfo != NULL )
            {
                stlMemcpy( aDiagInfo,
                           &aDiag->mDynamicFunctionCode,
                           STL_SIZEOF( stlInt32 ) );
            }
            break;
        case SQL_DIAG_NUMBER :
            if( aDiagInfo != NULL )
            {
                stlMemcpy( aDiagInfo,
                           &aDiag->mNumber,
                           STL_SIZEOF( stlInt32 ) );
            }
            break;
        case SQL_DIAG_RETURNCODE :
            if( aDiagInfo != NULL )
            {
                stlMemcpy( aDiagInfo,
                           &aDiag->mReturnCode,
                           STL_SIZEOF( stlInt16 ) );
            }
            break;
        case SQL_DIAG_ROW_COUNT :
            sCount = (SQLLEN)aDiag->mRowCount;
            if( aDiagInfo != NULL )
            {
                stlMemcpy( aDiagInfo, &sCount, STL_SIZEOF( SQLLEN ) );
            }
            break;
        default :
            sIdx = 1;
            STL_RING_FOREACH_ENTRY( &aDiag->mDiagRing, sDiagRec )
            {
                if( sIdx == aRecNumber )
                {
                    break;
                }

                sIdx++;
            }

            switch( aDiagId )
            {
                case SQL_DIAG_CLASS_ORIGIN :
                    if( aDiagInfo != NULL )
                    {
                        stlStrncpy( (stlChar*)aDiagInfo,
                                    sDiagRec->mClassOrigin,
                                    aBufferLen );
                    }
                    if( aStringLen != NULL )
                    {
                        *aStringLen = (stlInt16)stlStrlen( sDiagRec->mClassOrigin );
                    }
                    break;
                case SQL_DIAG_COLUMN_NUMBER :
                    if( aDiagInfo != NULL )
                    {
                        stlMemcpy( aDiagInfo,
                                   &sDiagRec->mColumnNumber,
                                   STL_SIZEOF( stlInt32 ) );
                    }
                    break;
                case SQL_DIAG_CONNECTION_NAME :
                    if( aDiagInfo != NULL )
                    {
                        stlStrncpy( (stlChar*)aDiagInfo,
                                    sDiagRec->mConnectionName,
                                    aBufferLen );
                    }
                    if( aStringLen != NULL )
                    {
                        *aStringLen = (stlInt16)stlStrlen( sDiagRec->mConnectionName );
                    }
                    break;
                case SQL_DIAG_MESSAGE_TEXT :
                    if( aDiagInfo != NULL )
                    {
                        stlStrncpy( (stlChar*)aDiagInfo,
                                    sDiagRec->mMessageText,
                                    aBufferLen );
                    }
                    if( aStringLen != NULL )
                    {
                        *aStringLen = (stlInt16)stlStrlen( sDiagRec->mMessageText );
                    }
                    break;
                case SQL_DIAG_NATIVE :
                    if( aDiagInfo != NULL )
                    {
                        stlMemcpy( aDiagInfo,
                                   &sDiagRec->mNative,
                                   STL_SIZEOF( stlInt32 ) );
                    }
                    break;
                case SQL_DIAG_ROW_NUMBER :
                    sCount = (SQLLEN)sDiagRec->mRowNumber;
                    if( aDiagInfo != NULL )
                    {
                        stlMemcpy( aDiagInfo,
                                   &sCount,
                                   STL_SIZEOF( SQLLEN ) );
                    }
                    break;
                case SQL_DIAG_SERVER_NAME :
                    if( aDiagInfo != NULL )
                    {
                        stlStrncpy( (stlChar*)aDiagInfo,
                                    sDiagRec->mServerName,
                                    aBufferLen );
                    }
                    if( aStringLen != NULL )
                    {
                        *aStringLen = (stlInt16)stlStrlen( sDiagRec->mServerName );
                    }
                    break;
                case SQL_DIAG_SQLSTATE :
                    if( aDiagInfo != NULL )
                    {
                        stlStrncpy( (stlChar*)aDiagInfo,
                                    sDiagRec->mSqlState,
                                    aBufferLen );
                    }
                    if( aStringLen != NULL )
                    {
                        *aStringLen = (stlInt16)stlStrlen( sDiagRec->mSqlState );
                    }
                    break;
                case SQL_DIAG_SUBCLASS_ORIGIN :
                    if( aDiagInfo != NULL )
                    {
                        stlStrncpy( (stlChar*)aDiagInfo,
                                    sDiagRec->mSubClassOrigin,
                                    aBufferLen );
                    }
                    if( aStringLen != NULL )
                    {
                        *aStringLen = (stlInt16)stlStrlen( sDiagRec->mSubClassOrigin );
                    }
                    break;
            }
            break;
    }

    return STL_SUCCESS;
}
                       
stlStatus zldDiagGetFieldW( zldDiag         * aDiag,
                            stlInt16          aRecNumber,
                            dtlCharacterSet   aCharacterSet,
                            stlInt16          aDiagId,
                            void            * aDiagInfo,
                            stlInt16          aBufferLen,
                            stlInt16        * aStringLen,
                            stlErrorStack   * aErrorStack )
{
    stlInt16         sIdx;
    zldDiagElement * sDiagRec;
    stlInt64         sLength = 0;
    stlInt64         sBufferLength = 0;

    switch( aDiagId )
    {
        case SQL_DIAG_DYNAMIC_FUNCTION :
        case SQL_DIAG_CLASS_ORIGIN :
        case SQL_DIAG_CONNECTION_NAME :
        case SQL_DIAG_MESSAGE_TEXT :
        case SQL_DIAG_SERVER_NAME :
        case SQL_DIAG_SQLSTATE :
        case SQL_DIAG_SUBCLASS_ORIGIN :
            break;
        default :
            STL_TRY( zldDiagGetField( aDiag,
                                      aRecNumber,
                                      aDiagId,
                                      aDiagInfo,
                                      aBufferLen,
                                      aStringLen ) == STL_SUCCESS );
            STL_THROW( RAMP_FINISH );
            break;
    }

    sBufferLength = aBufferLen / STL_SIZEOF(SQLWCHAR);
    
    if( aDiagId == SQL_DIAG_DYNAMIC_FUNCTION )
    {
        STL_TRY( zlvCharToWchar( aCharacterSet,
                                 (SQLCHAR*)aDiag->mDynamicFunction,
                                 STL_NTS,
                                 aDiagInfo,
                                 sBufferLength,
                                 &sLength,
                                 aErrorStack ) == STL_SUCCESS );
    }
    else
    {
        sIdx = 1;
        STL_RING_FOREACH_ENTRY( &aDiag->mDiagRing, sDiagRec )
        {
            if( sIdx == aRecNumber )
            {
                break;
            }

            sIdx++;
        }

        switch( aDiagId )
        {
            case SQL_DIAG_CLASS_ORIGIN :
                STL_TRY( zlvCharToWchar( aCharacterSet,
                                         (SQLCHAR*)sDiagRec->mClassOrigin,
                                         STL_NTS,
                                         aDiagInfo,
                                         sBufferLength,
                                         &sLength,
                                         aErrorStack ) == STL_SUCCESS );
                break;
            case SQL_DIAG_CONNECTION_NAME :
                STL_TRY( zlvCharToWchar( aCharacterSet,
                                         (SQLCHAR*)sDiagRec->mConnectionName,
                                         STL_NTS,
                                         aDiagInfo,
                                         sBufferLength,
                                         &sLength,
                                         aErrorStack ) == STL_SUCCESS );
                break;
            case SQL_DIAG_MESSAGE_TEXT :
                STL_TRY( zlvCharToWchar( aCharacterSet,
                                         (SQLCHAR*)sDiagRec->mMessageText,
                                         STL_NTS,
                                         aDiagInfo,
                                         sBufferLength,
                                         &sLength,
                                         aErrorStack ) == STL_SUCCESS );
                break;
            case SQL_DIAG_SERVER_NAME :
                STL_TRY( zlvCharToWchar( aCharacterSet,
                                         (SQLCHAR*)sDiagRec->mServerName,
                                         STL_NTS,
                                         aDiagInfo,
                                         sBufferLength,
                                         &sLength,
                                         aErrorStack ) == STL_SUCCESS );
                break;
            case SQL_DIAG_SQLSTATE :
                STL_TRY( zlvCharToWchar( aCharacterSet,
                                         (SQLCHAR*)sDiagRec->mSqlState,
                                         STL_NTS,
                                         aDiagInfo,
                                         sBufferLength,
                                         &sLength,
                                         aErrorStack ) == STL_SUCCESS );
                break;
            case SQL_DIAG_SUBCLASS_ORIGIN :
                STL_TRY( zlvCharToWchar( aCharacterSet,
                                         (SQLCHAR*)sDiagRec->mSubClassOrigin,
                                         STL_NTS,
                                         aDiagInfo,
                                         sBufferLength,
                                         &sLength,
                                         aErrorStack ) == STL_SUCCESS );
                break;
            default :
                STL_DASSERT( STL_FALSE );
                break;
        }
    }

    if( aStringLen != NULL )
    {
        *aStringLen = sLength * STL_SIZEOF(SQLWCHAR);
    }

    STL_RAMP( RAMP_FINISH );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
