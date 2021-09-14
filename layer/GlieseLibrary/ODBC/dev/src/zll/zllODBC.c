/*******************************************************************************
 * zllODBC.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: zllODBC.c 12017 2014-04-15 05:44:19Z mae113 $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#include <cml.h>
#include <goldilocks.h>

#include <zllRowStatusDef.h>
#include <zlDef.h>
#include <zle.h>
#include <zld.h>
#include <zln.h>
#include <zlc.h>
#include <zls.h>

#include <zli.h>
#include <zlf.h>
#include <zlr.h>
#include <zlt.h>

#include <zlp.h>
#include <zlx.h>

#include <zlvCharacterset.h>

/**
 * @file zllODBC.c
 * @brief ODBC API Routines
 */

/**
 * @addtogroup zllODBC
 * @{
 */

#ifdef WIN32
HINSTANCE gZllInstance;

BOOL WINAPI DllMain( HANDLE aModuleHandle,
                     DWORD  aReason,
                     LPVOID aReserved )
{
    gZllInstance = (HINSTANCE)aModuleHandle;

    return TRUE;
}
#endif

static stlInt16 zllGetStringLength( SQLCHAR     * aBuffer,
                                    SQLSMALLINT   aBufferLen )
{
    stlInt16 sStringLen;
    
    if( aBufferLen == SQL_NTS )
    {
        if( aBuffer == NULL)
        {
            sStringLen = 0;
        }
        else
        {
            sStringLen = stlStrlen( (stlChar*)aBuffer );
        }
    }
    else
    {
        sStringLen = aBufferLen;
    }

    return sStringLen;
}

SQLRETURN SQL_API SQLAllocConnect( SQLHENV   aEnvironmentHandle,
                                   SQLHDBC * aConnectionHandle )
{
    SQLRETURN       sRet        = SQL_ERROR;
    zlnEnv        * sEnv        = NULL;
    stlErrorData  * sErrorData  = NULL;
    stlErrorStack   sErrorStack;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sEnv = (zlnEnv*)aEnvironmentHandle;

    STL_TRY_THROW( sEnv != NULL, RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sEnv->mType == SQL_HANDLE_ENV,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( zldDiagClear( SQL_HANDLE_ENV,
                           aEnvironmentHandle,
                           &sErrorStack ) == STL_SUCCESS );

    STL_TRY_THROW( aConnectionHandle != NULL,
                   RAMP_ERR_INVALID_USE_OF_NULL_POINTER );

    switch( sEnv->mAttrVersion )
    {
        case SQL_OV_ODBC2 :
        case SQL_OV_ODBC3 :
        case SQL_OV_ODBC3_80 :
            break;
        default :
            STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR );
            break;
    }

    STL_TRY( zlcAlloc( sEnv,
                       aConnectionHandle,
                       &sErrorStack ) == STL_SUCCESS );

    zldDiagSetRetCode( SQL_HANDLE_ENV, aEnvironmentHandle, sRet );

    sRet = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      "The HandleType argument was SQL_HANDLE_DBC, "
                      "and SQLSetEnvAttr has not been called to set "
                      "the SQL_ODBC_VERSION environment attribute.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_USE_OF_NULL_POINTER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_USE_OF_NULL_POINTER,
                      "The ConnectionHandle argument was a null pointer.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_FINISH;

    if( sRet == SQL_ERROR )
    {
        zldDiagSetRetCode( SQL_HANDLE_ENV, aEnvironmentHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );
        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_ENV,
                               aEnvironmentHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }
    }

    return sRet;
}

SQLRETURN SQL_API SQLAllocEnv( SQLHENV * aEnvironmentHandle )
{
    stlErrorStack sErrorStack;

    STL_INIT_ERROR_STACK( &sErrorStack );

    STL_TRY_THROW( aEnvironmentHandle != NULL,
                   RAMP_ERR_INVALID_USE_OF_NULL_POINTER );

    STL_TRY( zlnAlloc( aEnvironmentHandle,
                       &sErrorStack ) == STL_SUCCESS );

    return SQL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_USE_OF_NULL_POINTER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_USE_OF_NULL_POINTER,
                      "The OutputHandlePtr argument was a null pointer.",
                      &sErrorStack );
    }

    STL_FINISH;

    return SQL_ERROR;
}

SQLRETURN SQL_API SQLAllocHandle( SQLSMALLINT   aHandleType,
                                  SQLHANDLE     aInputHandle,
                                  SQLHANDLE   * aOutputHandlePtr )
{
    SQLRETURN       sRet        = SQL_ERROR;
    zlnEnv        * sEnv        = NULL;
    zlcDbc        * sDbc        = NULL;
    stlErrorData  * sErrorData  = NULL;
    stlErrorStack   sErrorStack;

    STL_INIT_ERROR_STACK( &sErrorStack );

    STL_TRY_THROW( aOutputHandlePtr != NULL,
                   RAMP_ERR_INVALID_USE_OF_NULL_POINTER );
    
    switch( aHandleType )
    {
        case SQL_HANDLE_ENV :
            STL_TRY_THROW( aInputHandle == SQL_NULL_HANDLE,
                           RAMP_ERR_INVALID_HANDLE );

            STL_TRY( zlnAlloc( aOutputHandlePtr,
                               &sErrorStack ) == STL_SUCCESS );
            break;
        case SQL_HANDLE_DBC :
            sEnv = (zlnEnv*)aInputHandle;

            STL_TRY_THROW( sEnv != NULL, RAMP_ERR_INVALID_HANDLE );

            STL_TRY_THROW( sEnv->mType == SQL_HANDLE_ENV,
                           RAMP_ERR_INVALID_HANDLE );

            STL_TRY( zldDiagClear( SQL_HANDLE_ENV,
                                   aInputHandle,
                                   &sErrorStack ) == STL_SUCCESS );

            switch( sEnv->mAttrVersion )
            {
                case SQL_OV_ODBC2 :
                case SQL_OV_ODBC3 :
                case SQL_OV_ODBC3_80 :
                    break;
                default :
                    STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR );
                    break;
            }

            STL_TRY( zlcAlloc( sEnv,
                               aOutputHandlePtr,
                               &sErrorStack ) == STL_SUCCESS );
            break;
        case SQL_HANDLE_STMT :
            sDbc = (zlcDbc*)aInputHandle;

            STL_TRY_THROW( sDbc != NULL, RAMP_ERR_INVALID_HANDLE );

            STL_TRY_THROW( sDbc->mType == SQL_HANDLE_DBC,
                           RAMP_ERR_INVALID_HANDLE );

            STL_TRY( zldDiagClear( SQL_HANDLE_DBC,
                                   aInputHandle,
                                   &sErrorStack ) == STL_SUCCESS );
            
            switch( sDbc->mTransition )
            {
                case ZLC_TRANSITION_STATE_C0 :
                case ZLC_TRANSITION_STATE_C1 :
                    STL_THROW( RAMP_ERR_INVALID_HANDLE );
                    break;
                case ZLC_TRANSITION_STATE_C2 :
                case ZLC_TRANSITION_STATE_C3 :
                    STL_THROW( RAMP_ERR_CONNECTION_NOT_OPEN );
                    break;
                case ZLC_TRANSITION_STATE_C4 :
                case ZLC_TRANSITION_STATE_C5 :
                case ZLC_TRANSITION_STATE_C6 :
                    STL_TRY( zlcCheckSession( sDbc,
                                              &sErrorStack ) == STL_SUCCESS );
                    break;
            }

            STL_TRY( zlsAlloc( sDbc,
                               aOutputHandlePtr,
                               &sErrorStack ) == STL_SUCCESS );
            break;
        case SQL_HANDLE_DESC :
            sDbc = (zlcDbc*)aInputHandle;

            STL_TRY_THROW( sDbc != NULL, RAMP_ERR_INVALID_HANDLE );

            STL_TRY_THROW( sDbc->mType == SQL_HANDLE_DBC,
                           RAMP_ERR_INVALID_HANDLE );

            STL_TRY( zldDiagClear( SQL_HANDLE_DBC,
                                   aInputHandle,
                                   &sErrorStack ) == STL_SUCCESS );

            STL_TRY( zlcCheckSession( sDbc,
                                      &sErrorStack ) == STL_SUCCESS );

            STL_THROW( RAMP_ERR_DRIVER_DOES_NOT_SUPPORT_THIS_FUNCTION );
            break;
        default :
            STL_THROW( RAMP_ERR_INVALID_ATTRIBUTE_OPTION_IDENTIFIER );
            break;
    }
    
    sRet = SQL_SUCCESS;

    switch( aHandleType )
    {
        case SQL_HANDLE_DBC :
            zldDiagSetRetCode( SQL_HANDLE_ENV, aInputHandle, sRet );
            break;
        case SQL_HANDLE_STMT :
        case SQL_HANDLE_DESC :
            zldDiagSetRetCode( SQL_HANDLE_DBC, aInputHandle, sRet );
            break;
    }

    return sRet;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_CATCH( RAMP_ERR_INVALID_USE_OF_NULL_POINTER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_USE_OF_NULL_POINTER,
                      "The OutputHandlePtr argument was a null pointer.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      "The HandleType argument was SQL_HANDLE_DBC, "
                      "and SQLSetEnvAttr has not been called to set "
                      "the SQL_ODBC_VERSION environment attribute.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_CONNECTION_NOT_OPEN )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_CONNECTION_NOT_OPEN,
                      "The HandleType argument was SQL_HANDLE_STMT or SQL_HANDLE_DESC, "
                      "but the connection specified by the InputHandle argument was not open. "
                      "The connection process must be completed successfully "
                      "(and the connection must be open) for the driver to allocate "
                      "a statement or descriptor handle.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_ATTRIBUTE_OPTION_IDENTIFIER )
    {
    }

    STL_CATCH( RAMP_ERR_DRIVER_DOES_NOT_SUPPORT_THIS_FUNCTION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_DRIVER_DOES_NOT_SUPPORT_THIS_FUNCTION,
                      "The HandleType argument was SQL_HANDLE_DESC, "
                      "and the driver does not support allocating a descriptor handle.",
                      &sErrorStack );
    }

    STL_FINISH;

    if( sRet == SQL_ERROR )
    {
        sErrorData = stlGetLastErrorData( &sErrorStack );
        switch( aHandleType )
        {
            case SQL_HANDLE_DBC :
                zldDiagSetRetCode( SQL_HANDLE_ENV, aInputHandle, sRet );
                if( sErrorData != NULL )
                {
                    (void)zldDiagAdds( SQL_HANDLE_ENV,
                                       aInputHandle,
                                       SQL_NO_ROW_NUMBER,
                                       SQL_NO_COLUMN_NUMBER,
                                       &sErrorStack );
                }
                break;
            case SQL_HANDLE_STMT :
            case SQL_HANDLE_DESC :
                zldDiagSetRetCode( SQL_HANDLE_DBC, aInputHandle, sRet );

                if( sErrorData != NULL )
                {
                    (void)zldDiagAdds( SQL_HANDLE_DBC,
                                       aInputHandle,
                                       SQL_NO_ROW_NUMBER,
                                       SQL_NO_COLUMN_NUMBER,
                                       &sErrorStack );
                }
                break;
        }
    }

    return sRet;
}

SQLRETURN SQL_API SQLAllocStmt( SQLHDBC    aConnectionHandle,
                                SQLHSTMT * aStatementHandle )
{
    SQLRETURN       sRet        = SQL_ERROR;
    zlcDbc        * sDbc        = NULL;
    stlErrorData  * sErrorData  = NULL;
    stlErrorStack   sErrorStack;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sDbc = (zlcDbc*)aConnectionHandle;

    STL_TRY_THROW( sDbc != NULL, RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sDbc->mType == SQL_HANDLE_DBC,
                   RAMP_ERR_INVALID_HANDLE );

    switch( sDbc->mTransition )
    {
        case ZLC_TRANSITION_STATE_C0 :
        case ZLC_TRANSITION_STATE_C1 :
            STL_THROW( RAMP_ERR_INVALID_HANDLE );
            break;
        case ZLC_TRANSITION_STATE_C2 :
        case ZLC_TRANSITION_STATE_C3 :
            STL_TRY( zldDiagClear( SQL_HANDLE_DBC,
                                   aConnectionHandle,
                                   &sErrorStack ) == STL_SUCCESS );

            STL_THROW( RAMP_ERR_CONNECTION_NOT_OPEN );
            break;
        case ZLC_TRANSITION_STATE_C4 :
        case ZLC_TRANSITION_STATE_C5 :
        case ZLC_TRANSITION_STATE_C6 :
            STL_TRY( zldDiagClear( SQL_HANDLE_DBC,
                                   aConnectionHandle,
                                   &sErrorStack ) == STL_SUCCESS );
                    
            STL_TRY( zlcCheckSession( sDbc,
                                      &sErrorStack ) == STL_SUCCESS );
            break;
    }

    STL_TRY_THROW( aStatementHandle != NULL,
                   RAMP_ERR_INVALID_USE_OF_NULL_POINTER );

    STL_TRY( zlsAlloc( sDbc,
                       aStatementHandle,
                       &sErrorStack ) == STL_SUCCESS );

    sRet = SQL_SUCCESS;

    zldDiagSetRetCode( SQL_HANDLE_DBC, aConnectionHandle, sRet );

    return sRet;

    STL_CATCH( RAMP_ERR_INVALID_USE_OF_NULL_POINTER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_USE_OF_NULL_POINTER,
                      "The StatementHandle argument was a null pointer.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_CONNECTION_NOT_OPEN )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_CONNECTION_NOT_OPEN,
                      "The HandleType argument was SQL_HANDLE_STMT or SQL_HANDLE_DESC, "
                      "but the connection specified by the ConnectionHandle argument was not open. "
                      "The connection process must be completed successfully "
                      "(and the connection must be open) for the driver to allocate "
                      "a statement or descriptor handle.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_FINISH;

    if( sRet == SQL_ERROR )
    {
        zldDiagSetRetCode( SQL_HANDLE_DBC, aConnectionHandle, sRet );
        
        sErrorData = stlGetLastErrorData( &sErrorStack );
        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_DBC,
                               aConnectionHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }
    }

    return sRet;
}

SQLRETURN SQL_API SQLBindCol( SQLHSTMT       aStatementHandle,
                              SQLUSMALLINT   aColumnNumber,
                              SQLSMALLINT    aTargetType,
                              SQLPOINTER     aTargetValuePtr,
                              SQLLEN         aBufferLength,
                              SQLLEN       * aStrLen_or_Ind )
{
    SQLRETURN       sRet        = SQL_ERROR;
    zlsStmt       * sStmt       = NULL;
    stlErrorData  * sErrorData  = NULL;
    stlErrorStack   sErrorStack;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sStmt = (zlsStmt*)aStatementHandle;

    STL_TRY_THROW( sStmt != NULL,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sStmt->mType == SQL_HANDLE_STMT,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( zldDiagClear( SQL_HANDLE_STMT,
                           aStatementHandle,
                           &sErrorStack ) == STL_SUCCESS );

    STL_TRY( zlcCheckSession( sStmt->mParentDbc,
                              &sErrorStack ) == STL_SUCCESS );
    
    switch( sStmt->mTransition )
    {
        case ZLS_TRANSITION_STATE_S0 :
            STL_THROW( RAMP_ERR_INVALID_HANDLE );
            break;
        case ZLS_TRANSITION_STATE_S8 :
        case ZLS_TRANSITION_STATE_S9 :
        case ZLS_TRANSITION_STATE_S10 :
        case ZLS_TRANSITION_STATE_S11 :
        case ZLS_TRANSITION_STATE_S12 :
            STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR );
            break;
        default :
            break;
    }

    switch( aTargetType )
    {
        case SQL_C_CHAR :
        case SQL_C_SSHORT :
        case SQL_C_USHORT :
        case SQL_C_SHORT :
        case SQL_C_SLONG :
        case SQL_C_ULONG :
        case SQL_C_LONG :
        case SQL_C_FLOAT :
        case SQL_C_DOUBLE :
        case SQL_C_BIT :
        case SQL_C_STINYINT :
        case SQL_C_UTINYINT :
        case SQL_C_TINYINT :
        case SQL_C_SBIGINT :
        case SQL_C_UBIGINT :
        case SQL_C_BINARY :
        case SQL_C_TYPE_DATE :
        case SQL_C_TYPE_TIME :
        case SQL_C_TYPE_TIMESTAMP :
        case SQL_C_INTERVAL_YEAR :
        case SQL_C_INTERVAL_MONTH :
        case SQL_C_INTERVAL_DAY :
        case SQL_C_INTERVAL_HOUR :
        case SQL_C_INTERVAL_MINUTE :
        case SQL_C_INTERVAL_SECOND :
        case SQL_C_INTERVAL_YEAR_TO_MONTH :
        case SQL_C_INTERVAL_DAY_TO_HOUR :
        case SQL_C_INTERVAL_DAY_TO_MINUTE :
        case SQL_C_INTERVAL_DAY_TO_SECOND :
        case SQL_C_INTERVAL_HOUR_TO_MINUTE :
        case SQL_C_INTERVAL_HOUR_TO_SECOND :
        case SQL_C_INTERVAL_MINUTE_TO_SECOND :
        case SQL_C_NUMERIC :
        case SQL_C_DEFAULT :
        case SQL_C_WCHAR :
            /*
             * Driver C datatype
             */
        case SQL_C_BOOLEAN :
        case SQL_C_LONGVARCHAR :
        case SQL_C_LONGVARBINARY :
        case SQL_C_TYPE_TIME_WITH_TIMEZONE :
        case SQL_C_TYPE_TIMESTAMP_WITH_TIMEZONE :
            break;
        case SQL_C_GUID :
            STL_THROW( RAMP_ERR_OPTIONAL_FEATURE_NOT_IMPLEMENTED );
            
        default :
            STL_THROW( RAMP_ERR_INVALID_APPLICATION_BUFFER_TYPE );
            break;
    }

    STL_TRY_THROW( aBufferLength >= 0,
                   RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH );

    STL_TRY( zliBindCol( sStmt,
                         aColumnNumber,
                         aTargetType,
                         aTargetValuePtr,
                         aBufferLength,
                         aStrLen_or_Ind,
                         &sErrorStack ) == STL_SUCCESS );

    sRet = SQL_SUCCESS;
    
    zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

    return sRet;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      NULL,
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_OPTIONAL_FEATURE_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_OPTIONAL_FEATURE_NOT_IMPLEMENTED,
                      "The driver or data source does not support the conversion specified by "
                      "the combination of the TargetType argument and the driver-specific "
                      "SQL data type of the corresponding column.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_APPLICATION_BUFFER_TYPE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_APPLICATION_BUFFER_TYPE,
                      "The argument TargetType was neither "
                      "a valid data type nor SQL_C_DEFAULT.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_STRING_OR_BUFFER_LENGTH,
                      "The value specified for the argument BufferLength was less than 0.",
                      &sErrorStack );
    }

    STL_FINISH;

    if( sRet == SQL_ERROR )
    {
        zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_STMT,
                               aStatementHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }
    }

    return sRet;
}

SQLRETURN SQL_API SQLBindParam( SQLHSTMT       aStatementHandle,
                                SQLUSMALLINT   aParameterNumber,
                                SQLSMALLINT    aValueType,
                                SQLSMALLINT    aParameterType,
                                SQLULEN        aLengthPrecision,
                                SQLSMALLINT    aParameterScale,
                                SQLPOINTER     aParameterValuePtr,
                                SQLLEN       * aStrLen_or_IndPtr )
{
    SQLRETURN       sRet        = SQL_ERROR;
    zlsStmt       * sStmt       = NULL;
    stlErrorData  * sErrorData  = NULL;
    stlErrorStack   sErrorStack;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sStmt = (zlsStmt*)aStatementHandle;

    STL_TRY_THROW( sStmt != NULL, RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sStmt->mType == SQL_HANDLE_STMT,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( zldDiagClear( SQL_HANDLE_STMT,
                           aStatementHandle,
                           &sErrorStack ) == STL_SUCCESS );

    STL_TRY( zlcCheckSession( sStmt->mParentDbc,
                              &sErrorStack ) == STL_SUCCESS );
    
    switch( sStmt->mTransition )
    {
        case ZLS_TRANSITION_STATE_S0 :
            STL_THROW( RAMP_ERR_INVALID_HANDLE );
            break;
        case ZLS_TRANSITION_STATE_S8 :
        case ZLS_TRANSITION_STATE_S9 :
        case ZLS_TRANSITION_STATE_S10 :
        case ZLS_TRANSITION_STATE_S11 :
        case ZLS_TRANSITION_STATE_S12 :
            STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR );
            break;
        default :
            break;
    }

    STL_TRY_THROW( aParameterNumber > 0, RAMP_ERR_INVALID_DESCRIPTOR_INDEX );

    switch( aValueType )
    {
        case SQL_C_CHAR :
        case SQL_C_STINYINT :
        case SQL_C_UTINYINT :
        case SQL_C_TINYINT :            
        case SQL_C_SSHORT :
        case SQL_C_USHORT :
        case SQL_C_SHORT :
        case SQL_C_SLONG :
        case SQL_C_ULONG :
        case SQL_C_LONG :
        case SQL_C_SBIGINT :
        case SQL_C_UBIGINT :
        case SQL_C_FLOAT :
        case SQL_C_DOUBLE :
        case SQL_C_BIT :
        case SQL_C_BINARY :
        case SQL_C_TYPE_DATE :
        case SQL_C_TYPE_TIME :
        case SQL_C_TYPE_TIMESTAMP :
        case SQL_C_INTERVAL_YEAR :
        case SQL_C_INTERVAL_MONTH :
        case SQL_C_INTERVAL_DAY :
        case SQL_C_INTERVAL_HOUR :
        case SQL_C_INTERVAL_MINUTE :
        case SQL_C_INTERVAL_SECOND :
        case SQL_C_INTERVAL_YEAR_TO_MONTH :
        case SQL_C_INTERVAL_DAY_TO_HOUR :
        case SQL_C_INTERVAL_DAY_TO_MINUTE :
        case SQL_C_INTERVAL_DAY_TO_SECOND :
        case SQL_C_INTERVAL_HOUR_TO_MINUTE :
        case SQL_C_INTERVAL_HOUR_TO_SECOND :
        case SQL_C_INTERVAL_MINUTE_TO_SECOND :
        case SQL_C_NUMERIC :
        case SQL_C_DEFAULT:
        case SQL_C_WCHAR :
            /*
             * Driver SQL Type
             */
        case SQL_C_BOOLEAN :
        case SQL_C_LONGVARCHAR :
        case SQL_C_LONGVARBINARY :
        case SQL_C_TYPE_TIME_WITH_TIMEZONE :
        case SQL_C_TYPE_TIMESTAMP_WITH_TIMEZONE :
            break;
        case SQL_C_GUID :
            STL_THROW( RAMP_ERR_OPTIONAL_FEATURE_NOT_IMPLEMENTED1 );
            break;
        default :
            STL_THROW( RAMP_ERR_INVALID_APPLICATION_BUFFER_TYPE );
            break;
    }

    switch( aParameterType )
    {
        case SQL_CHAR :
        case SQL_VARCHAR :
        case SQL_LONGVARCHAR :
        case SQL_NUMERIC :
        case SQL_SMALLINT :
        case SQL_INTEGER :
        case SQL_REAL :
        case SQL_DOUBLE :
        case SQL_FLOAT :
        case SQL_BIT :
        case SQL_TINYINT :
        case SQL_BIGINT :
        case SQL_BINARY :
        case SQL_VARBINARY :
        case SQL_LONGVARBINARY :
        case SQL_TYPE_DATE :
        case SQL_TYPE_TIME :
        case SQL_TYPE_TIMESTAMP :
        case SQL_INTERVAL_MONTH :
        case SQL_INTERVAL_YEAR :
        case SQL_INTERVAL_YEAR_TO_MONTH :
        case SQL_INTERVAL_DAY :
        case SQL_INTERVAL_HOUR :
        case SQL_INTERVAL_MINUTE :
        case SQL_INTERVAL_SECOND :
        case SQL_INTERVAL_DAY_TO_HOUR :
        case SQL_INTERVAL_DAY_TO_MINUTE :
        case SQL_INTERVAL_DAY_TO_SECOND :
        case SQL_INTERVAL_HOUR_TO_MINUTE :
        case SQL_INTERVAL_HOUR_TO_SECOND :
        case SQL_INTERVAL_MINUTE_TO_SECOND :
        case SQL_WCHAR :
            /*
             * Driver SQL Type
             */
        case SQL_BOOLEAN :
        case SQL_TYPE_TIME_WITH_TIMEZONE :
        case SQL_TYPE_TIMESTAMP_WITH_TIMEZONE :
        case SQL_ROWID :            
            break;
        case SQL_WVARCHAR :
        case SQL_WLONGVARCHAR :
        case SQL_DECIMAL :
            STL_THROW( RAMP_ERR_OPTIONAL_FEATURE_NOT_IMPLEMENTED2 );
            break;
        default :
            STL_THROW( RAMP_ERR_INVALID_SQL_DATA_TYPE );
            break;
    }

    STL_TRY_THROW( ( aParameterValuePtr != NULL ) ||
                   ( aStrLen_or_IndPtr  != NULL ),
                   RAMP_ERR_INVALID_ARGUMENT_VALUE );

    STL_TRY( zliBindParameter( sStmt,
                               aParameterNumber,
                               SQL_PARAM_INPUT,
                               aValueType,
                               aParameterType,
                               aLengthPrecision,
                               aParameterScale,
                               aParameterValuePtr,
                               0,
                               aStrLen_or_IndPtr,
                               &sErrorStack ) == STL_SUCCESS );

    sRet = SQL_SUCCESS;
    
    zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

    return sRet;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      NULL,
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_DESCRIPTOR_INDEX )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_DESCRIPTOR_INDEX,
                      "The value specified for the argument ParameterNumber was "
                      "less than 1.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_APPLICATION_BUFFER_TYPE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_APPLICATION_BUFFER_TYPE,
                      "The value specified by the argument ValueType was not "
                      "a valid C data type or SQL_C_DEFAULT.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_SQL_DATA_TYPE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_SQL_DATA_TYPE,
                      "The value specified for the argument ParameterType was "
                      "neither a valid ODBC SQL data type identifier nor "
                      "a driver-specific SQL data type identifier supported by the driver.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_ARGUMENT_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_ARGUMENT_VALUE,
                      "The argument ParameterValuePtr was a null pointer, "
                      "the argument StrLen_or_IndPtr was a null pointer, "
                      "and the argument InputOutputType was not SQL_PARAM_OUTPUT.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_OPTIONAL_FEATURE_NOT_IMPLEMENTED1 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_OPTIONAL_FEATURE_NOT_IMPLEMENTED,
                      "The driver or data source does not support the conversion specified "
                      "by the combination of the value specified for the argument ValueType "
                      "and the driver-specific value specified for the argument ParameterType.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_OPTIONAL_FEATURE_NOT_IMPLEMENTED2 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_OPTIONAL_FEATURE_NOT_IMPLEMENTED,
                      "The value specified for the argument ParameterType was "
                      "a valid ODBC SQL data type identifier for the version of "
                      "ODBC supported by the driver but was not supported by the driver or data source.",
                      &sErrorStack );
    }

    STL_FINISH;

    if( sRet == SQL_ERROR )
    {
        zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_STMT,
                               aStatementHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }
    }

    return sRet;
}

SQLRETURN SQL_API SQLBindParameter( SQLHSTMT      aStatementHandle,
                                    SQLUSMALLINT  aParameterNumber,
                                    SQLSMALLINT   aInputOutputType,
                                    SQLSMALLINT   aValueType,
                                    SQLSMALLINT   aParameterType,
                                    SQLULEN       aColumnSize,
                                    SQLSMALLINT   aDecimalDigits,
                                    SQLPOINTER    aParameterValuePtr,
                                    SQLLEN        aBufferLength,
                                    SQLLEN      * aStrLen_or_IndPtr )
{
    SQLRETURN       sRet        = SQL_ERROR;
    zlsStmt       * sStmt       = NULL;
    stlErrorData  * sErrorData  = NULL;
    stlErrorStack   sErrorStack;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sStmt = (zlsStmt*)aStatementHandle;

    STL_TRY_THROW( sStmt != NULL,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sStmt->mType == SQL_HANDLE_STMT,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( zldDiagClear( SQL_HANDLE_STMT,
                           aStatementHandle,
                           &sErrorStack ) == STL_SUCCESS );

    STL_TRY( zlcCheckSession( sStmt->mParentDbc,
                              &sErrorStack ) == STL_SUCCESS );
    
    switch( sStmt->mTransition )
    {
        case ZLS_TRANSITION_STATE_S0 :
            STL_THROW( RAMP_ERR_INVALID_HANDLE );
            break;
        case ZLS_TRANSITION_STATE_S8 :
        case ZLS_TRANSITION_STATE_S9 :
        case ZLS_TRANSITION_STATE_S10 :
        case ZLS_TRANSITION_STATE_S11 :
        case ZLS_TRANSITION_STATE_S12 :
            STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR );
            break;
        default :
            break;
    }

    STL_TRY_THROW( aParameterNumber > 0,
                   RAMP_ERR_INVALID_DESCRIPTOR_INDEX );

    switch( aValueType )
    {
        case SQL_C_CHAR :
        case SQL_C_STINYINT :
        case SQL_C_UTINYINT :
        case SQL_C_TINYINT :            
        case SQL_C_SSHORT :
        case SQL_C_USHORT :
        case SQL_C_SHORT :
        case SQL_C_SLONG :
        case SQL_C_ULONG :
        case SQL_C_LONG :
        case SQL_C_SBIGINT :
        case SQL_C_UBIGINT :
        case SQL_C_FLOAT :
        case SQL_C_DOUBLE :
        case SQL_C_BIT :
        case SQL_C_BINARY :
        case SQL_C_TYPE_DATE :
        case SQL_C_TYPE_TIME :
        case SQL_C_TYPE_TIMESTAMP :
        case SQL_C_INTERVAL_YEAR :
        case SQL_C_INTERVAL_MONTH :
        case SQL_C_INTERVAL_DAY :
        case SQL_C_INTERVAL_HOUR :
        case SQL_C_INTERVAL_MINUTE :
        case SQL_C_INTERVAL_SECOND :
        case SQL_C_INTERVAL_YEAR_TO_MONTH :
        case SQL_C_INTERVAL_DAY_TO_HOUR :
        case SQL_C_INTERVAL_DAY_TO_MINUTE :
        case SQL_C_INTERVAL_DAY_TO_SECOND :
        case SQL_C_INTERVAL_HOUR_TO_MINUTE :
        case SQL_C_INTERVAL_HOUR_TO_SECOND :
        case SQL_C_INTERVAL_MINUTE_TO_SECOND :
        case SQL_C_NUMERIC :
        case SQL_C_DEFAULT:
        case SQL_C_WCHAR :
            /*
             * Driver SQL Type
             */
        case SQL_C_BOOLEAN :
        case SQL_C_LONGVARCHAR :
        case SQL_C_LONGVARBINARY :
        case SQL_C_TYPE_TIME_WITH_TIMEZONE :
        case SQL_C_TYPE_TIMESTAMP_WITH_TIMEZONE :
            break;
        case SQL_C_GUID :
            STL_THROW( RAMP_ERR_OPTIONAL_FEATURE_NOT_IMPLEMENTED1 );
            break;
        default :
            STL_THROW( RAMP_ERR_INVALID_APPLICATION_BUFFER_TYPE );
            break;
    }

    switch( aParameterType )
    {
        case SQL_CHAR :
        case SQL_VARCHAR :
        case SQL_LONGVARCHAR :
        case SQL_NUMERIC :
        case SQL_SMALLINT :
        case SQL_INTEGER :
        case SQL_REAL :
        case SQL_DOUBLE :
        case SQL_FLOAT :
        case SQL_BIT :
        case SQL_TINYINT :
        case SQL_BIGINT :
        case SQL_BINARY :
        case SQL_VARBINARY :
        case SQL_LONGVARBINARY :
        case SQL_TYPE_DATE :
        case SQL_TYPE_TIME :
        case SQL_TYPE_TIMESTAMP :
        case SQL_INTERVAL_MONTH :
        case SQL_INTERVAL_YEAR :
        case SQL_INTERVAL_YEAR_TO_MONTH :
        case SQL_INTERVAL_DAY :
        case SQL_INTERVAL_HOUR :
        case SQL_INTERVAL_MINUTE :
        case SQL_INTERVAL_SECOND :
        case SQL_INTERVAL_DAY_TO_HOUR :
        case SQL_INTERVAL_DAY_TO_MINUTE :
        case SQL_INTERVAL_DAY_TO_SECOND :
        case SQL_INTERVAL_HOUR_TO_MINUTE :
        case SQL_INTERVAL_HOUR_TO_SECOND :
        case SQL_INTERVAL_MINUTE_TO_SECOND :
            /*
             * Driver SQL Type
             */
        case SQL_BOOLEAN :
        case SQL_TYPE_TIME_WITH_TIMEZONE :
        case SQL_TYPE_TIMESTAMP_WITH_TIMEZONE :
        case SQL_ROWID :            
            break;
        case SQL_WCHAR :
        case SQL_WVARCHAR :
        case SQL_WLONGVARCHAR :
        case SQL_DECIMAL :
            STL_THROW( RAMP_ERR_OPTIONAL_FEATURE_NOT_IMPLEMENTED2 );
            break;
        default :
            STL_THROW( RAMP_ERR_INVALID_SQL_DATA_TYPE );
            break;
    }

    STL_TRY_THROW( aBufferLength >= 0,
                   RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH );

    switch( aInputOutputType )
    {
        case SQL_PARAM_INPUT :
        case SQL_PARAM_INPUT_OUTPUT :
        case SQL_PARAM_INPUT_OUTPUT_STREAM :
        case SQL_PARAM_OUTPUT_STREAM :
            STL_TRY_THROW( ( aParameterValuePtr != NULL ) ||
                           ( aStrLen_or_IndPtr  != NULL ),
                           RAMP_ERR_INVALID_ARGUMENT_VALUE1 );
            break;
            
        case SQL_PARAM_OUTPUT :
            switch( aValueType )
            {
                case SQL_C_CHAR :
                case SQL_C_WCHAR :
                case SQL_C_BINARY :
                    STL_TRY_THROW( ( aParameterValuePtr != NULL ) ||
                                   ( aBufferLength == 0 ),
                                   RAMP_ERR_INVALID_ARGUMENT_VALUE2 );
                    break;
                case SQL_C_LONGVARCHAR :
                case SQL_C_LONGVARBINARY :
                    STL_TRY_THROW( aParameterValuePtr != NULL,
                                   RAMP_ERR_INVALID_ARGUMENT_VALUE2 );
                    break;
                default :
                    break;
            }
            break;
            
        default :
            STL_THROW( RAMP_ERR_INVALID_PARAMETER_TYPE )
                break;
    }

    STL_TRY( zliBindParameter( sStmt,
                               aParameterNumber,
                               aInputOutputType,
                               aValueType,
                               aParameterType,
                               aColumnSize,
                               aDecimalDigits,
                               aParameterValuePtr,
                               aBufferLength,
                               aStrLen_or_IndPtr,
                               &sErrorStack ) == STL_SUCCESS );

    sRet = SQL_SUCCESS;
    
    zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

    return sRet;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      NULL,
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_DESCRIPTOR_INDEX )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_DESCRIPTOR_INDEX,
                      "The value specified for the argument ParameterNumber was "
                      "less than 1.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_APPLICATION_BUFFER_TYPE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_APPLICATION_BUFFER_TYPE,
                      "The value specified by the argument ValueType was not "
                      "a valid C data type or SQL_C_DEFAULT.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_SQL_DATA_TYPE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_SQL_DATA_TYPE,
                      "The value specified for the argument ParameterType was "
                      "neither a valid ODBC SQL data type identifier nor "
                      "a driver-specific SQL data type identifier supported by the driver.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_STRING_OR_BUFFER_LENGTH,
                      "The value in BufferLength was less than 0.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_PARAMETER_TYPE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_PARAMETER_TYPE,
                      "The value specified for the argument InputOutputType was invalid.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_ARGUMENT_VALUE1 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_ARGUMENT_VALUE,
                      "The argument ParameterValuePtr was a null pointer, "
                      "the argument StrLen_or_IndPtr was a null pointer, "
                      "and the argument InputOutputType was not SQL_PARAM_OUTPUT.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_ARGUMENT_VALUE2 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_ARGUMENT_VALUE,
                      "SQL_PARAM_OUTPUT, "
                      "where the argument ParameterValuePtr was a null pointer, "
                      "the C type was char or binary, "
                      "and the BufferLength was greater than 0.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_OPTIONAL_FEATURE_NOT_IMPLEMENTED1 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_OPTIONAL_FEATURE_NOT_IMPLEMENTED,
                      "The driver or data source does not support the conversion specified "
                      "by the combination of the value specified for the argument ValueType "
                      "and the driver-specific value specified for the argument ParameterType.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_OPTIONAL_FEATURE_NOT_IMPLEMENTED2 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_OPTIONAL_FEATURE_NOT_IMPLEMENTED,
                      "The value specified for the argument ParameterType was "
                      "a valid ODBC SQL data type identifier for the version of "
                      "ODBC supported by the driver but was not supported by the driver or data source.",
                      &sErrorStack );
    }

    STL_FINISH;

    if( sRet == SQL_ERROR )
    {
        zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_STMT,
                               aStatementHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }
    }

    return sRet;
}

SQLRETURN SQL_API SQLBrowseConnect( SQLHDBC       aConnectionHandle,
                                    SQLCHAR     * aInConnectionString,
                                    SQLSMALLINT   aStringLength1,
                                    SQLCHAR     * aOutConnectionString,
                                    SQLSMALLINT   aBufferLength,
                                    SQLSMALLINT * aStringLength2Ptr)
{
    zlcDbc        * sDbc         = NULL;
    SQLRETURN       sRet         = SQL_ERROR;
    stlErrorData  * sErrorData   = NULL;
    stlErrorStack   sErrorStack;

    sDbc = (zlcDbc*)aConnectionHandle;
    
    STL_TRY_THROW( sDbc != NULL, RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sDbc->mType == SQL_HANDLE_DBC,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( zldDiagClear( SQL_HANDLE_DBC,
                           aConnectionHandle,
                           &sErrorStack ) == STL_SUCCESS );

    switch( sDbc->mTransition )
    {
        case ZLC_TRANSITION_STATE_C0 :
        case ZLC_TRANSITION_STATE_C1 :
            STL_THROW( RAMP_ERR_INVALID_HANDLE );
            break;
        case ZLC_TRANSITION_STATE_C2 :
        case ZLC_TRANSITION_STATE_C3 :
            break;
        case ZLC_TRANSITION_STATE_C4 :
        case ZLC_TRANSITION_STATE_C5 :
        case ZLC_TRANSITION_STATE_C6 :
            STL_THROW( RAMP_ERR_CONNECTION_NAME_IN_USE );
            break;
    }

    STL_THROW( RAMP_ERR_DRIVER_DOES_NOT_SUPPORT_THIS_FUNCTION );
    
    sRet = SQL_SUCCESS;

    zldDiagSetRetCode( SQL_HANDLE_DBC, aConnectionHandle, sRet );

    return sRet;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_CATCH( RAMP_ERR_CONNECTION_NAME_IN_USE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_CONNECTION_NAME_IN_USE,
                      "The specified ConnectionHandle had already been used to establish a connection with a data source, "
                      "and the connection was open.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_DRIVER_DOES_NOT_SUPPORT_THIS_FUNCTION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_DRIVER_DOES_NOT_SUPPORT_THIS_FUNCTION,
                      "The driver corresponding to the specified data source name does not support the function.",
                      &sErrorStack );
    }
    
    STL_FINISH;

    if( sRet == SQL_ERROR )
    {
        zldDiagSetRetCode( SQL_HANDLE_DBC, aConnectionHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_DBC,
                               aConnectionHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }
    }

    return sRet;
}

SQLRETURN SQL_API SQLBrowseConnectW( SQLHDBC       aConnectionHandle,
                                     SQLWCHAR    * aInConnectionString,
                                     SQLSMALLINT   aStringLength1,
                                     SQLWCHAR    * aOutConnectionString,
                                     SQLSMALLINT   aBufferLength,
                                     SQLSMALLINT * aStringLength2Ptr)
{
    /**
     * aStringLength1
     *     Length of *InConnectionString, in characters.
     * aBufferLength
     *     Length of the *OutConnectionString buffer, in characters.
     * aStringLength2Ptr
     *     The total number of characters
     *     (excluding the null-termination character) available to return
     *     in *OutConnectionString.
     */

    zlcDbc        * sDbc         = NULL;
    SQLRETURN       sRet         = SQL_ERROR;
    stlErrorData  * sErrorData   = NULL;
    stlErrorStack   sErrorStack;

    sDbc = (zlcDbc*)aConnectionHandle;
    
    STL_TRY_THROW( sDbc != NULL, RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sDbc->mType == SQL_HANDLE_DBC,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( zldDiagClear( SQL_HANDLE_DBC,
                           aConnectionHandle,
                           &sErrorStack ) == STL_SUCCESS );

    switch( sDbc->mTransition )
    {
        case ZLC_TRANSITION_STATE_C0 :
        case ZLC_TRANSITION_STATE_C1 :
            STL_THROW( RAMP_ERR_INVALID_HANDLE );
            break;
        case ZLC_TRANSITION_STATE_C2 :
        case ZLC_TRANSITION_STATE_C3 :
            break;
        case ZLC_TRANSITION_STATE_C4 :
        case ZLC_TRANSITION_STATE_C5 :
        case ZLC_TRANSITION_STATE_C6 :
            STL_THROW( RAMP_ERR_CONNECTION_NAME_IN_USE );
            break;
    }

    STL_THROW( RAMP_ERR_DRIVER_DOES_NOT_SUPPORT_THIS_FUNCTION );
    
    sRet = SQL_SUCCESS;

    zldDiagSetRetCode( SQL_HANDLE_DBC, aConnectionHandle, sRet );

    return sRet;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_CATCH( RAMP_ERR_CONNECTION_NAME_IN_USE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_CONNECTION_NAME_IN_USE,
                      "The specified ConnectionHandle had already been used to establish a connection with a data source, "
                      "and the connection was open.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_DRIVER_DOES_NOT_SUPPORT_THIS_FUNCTION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_DRIVER_DOES_NOT_SUPPORT_THIS_FUNCTION,
                      "The driver corresponding to the specified data source name does not support the function.",
                      &sErrorStack );
    }
    
    STL_FINISH;

    if( sRet == SQL_ERROR )
    {
        zldDiagSetRetCode( SQL_HANDLE_DBC, aConnectionHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_DBC,
                               aConnectionHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }
    }

    return sRet;
}

SQLRETURN SQL_API SQLBulkOperations( SQLHSTMT     aStatementHandle,
                                     SQLSMALLINT  aOperation )
{
    SQLRETURN       sRet              = SQL_ERROR;
    zlcDbc        * sDbc              = NULL;
    zlsStmt       * sStmt             = NULL;
    stlErrorData  * sErrorData        = NULL;
    stlErrorStack   sErrorStack;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sStmt = (zlsStmt*)aStatementHandle;

    STL_TRY_THROW( sStmt != NULL, RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sStmt->mType == SQL_HANDLE_STMT,
                   RAMP_ERR_INVALID_HANDLE );

    sDbc = sStmt->mParentDbc;
    
    STL_TRY( zldDiagClear( SQL_HANDLE_STMT,
                           aStatementHandle,
                           &sErrorStack ) == STL_SUCCESS );

    STL_TRY( zlcCheckSession( sDbc,
                              &sErrorStack ) == STL_SUCCESS );

    STL_THROW( RAMP_ERR_DRIVER_DOES_NOT_SUPPORT_THIS_FUNCTION );
    
    sRet = SQL_SUCCESS;

    zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

    return sRet;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_CATCH( RAMP_ERR_DRIVER_DOES_NOT_SUPPORT_THIS_FUNCTION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_DRIVER_DOES_NOT_SUPPORT_THIS_FUNCTION,
                      "The driver associated with the StatementHandle does not support the function.",
                      &sErrorStack );
    }
    
    STL_FINISH;

    if( sRet == SQL_ERROR )
    {
        zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_STMT,
                               aStatementHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }
    }

    return sRet;
}

SQLRETURN SQL_API SQLCancel( SQLHSTMT aStatementHandle )
{
    SQLRETURN       sRet              = SQL_ERROR;
    zlcDbc        * sDbc              = NULL;
    zlsStmt       * sStmt             = NULL;
    stlErrorData  * sErrorData        = NULL;
    stlErrorStack   sErrorStack;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sStmt = (zlsStmt*)aStatementHandle;

    STL_TRY_THROW( sStmt != NULL, RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sStmt->mType == SQL_HANDLE_STMT,
                   RAMP_ERR_INVALID_HANDLE );

    sDbc = sStmt->mParentDbc;
    
    STL_TRY( zldDiagClear( SQL_HANDLE_STMT,
                           aStatementHandle,
                           &sErrorStack ) == STL_SUCCESS );

    STL_TRY( zlcCheckSession( sDbc,
                              &sErrorStack ) == STL_SUCCESS );

    STL_THROW( RAMP_ERR_DRIVER_DOES_NOT_SUPPORT_THIS_FUNCTION );
    
    sRet = SQL_SUCCESS;

    zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

    return sRet;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_CATCH( RAMP_ERR_DRIVER_DOES_NOT_SUPPORT_THIS_FUNCTION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_DRIVER_DOES_NOT_SUPPORT_THIS_FUNCTION,
                      "The driver associated with the StatementHandle does not support the function.",
                      &sErrorStack );
    }
    
    STL_FINISH;

    if( sRet == SQL_ERROR )
    {
        zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_STMT,
                               aStatementHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }
    }

    return sRet;
}

SQLRETURN SQL_API SQLCancelHandle( SQLSMALLINT aHandleType,
                                   SQLHANDLE   aHandle )
{
    SQLRETURN       sRet        = SQL_ERROR;
    zlcDbc        * sDbc        = NULL;
    zlsStmt       * sStmt       = NULL;
    stlErrorData  * sErrorData  = NULL;
    stlErrorStack   sErrorStack;

    STL_INIT_ERROR_STACK( &sErrorStack );

    switch( aHandleType )
    {
        case SQL_HANDLE_DBC :
            sDbc = (zlcDbc*)aHandle;

            STL_TRY_THROW( sDbc != NULL, RAMP_ERR_INVALID_HANDLE );

            STL_TRY_THROW( sDbc->mType == SQL_HANDLE_DBC,
                           RAMP_ERR_INVALID_HANDLE );
            break;
        case SQL_HANDLE_STMT :
            sStmt = (zlsStmt*)aHandle;

            STL_TRY_THROW( sStmt != NULL, RAMP_ERR_INVALID_HANDLE );

            STL_TRY_THROW( sStmt->mType == SQL_HANDLE_STMT,
                           RAMP_ERR_INVALID_HANDLE );
            break;
        default :
            STL_THROW( RAMP_ERR_INVALID_ATTRIBUTE_OPTION_IDENTIFIER );
            break;
    }

    STL_TRY( zldDiagClear( aHandleType,
                           aHandle,
                           &sErrorStack ) == STL_SUCCESS );

    STL_THROW( RAMP_ERR_DRIVER_DOES_NOT_SUPPORT_THIS_FUNCTION );
    
    sRet = SQL_SUCCESS;

    zldDiagSetRetCode( aHandleType, aHandle, sRet );

    return sRet;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_CATCH( RAMP_ERR_INVALID_ATTRIBUTE_OPTION_IDENTIFIER )
    {
    }

    STL_CATCH( RAMP_ERR_DRIVER_DOES_NOT_SUPPORT_THIS_FUNCTION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_DRIVER_DOES_NOT_SUPPORT_THIS_FUNCTION,
                      "The driver associated with the Handle does not support the function.",
                      &sErrorStack );
    }

    STL_FINISH;

    if( sRet == SQL_ERROR )
    {
        switch( aHandleType )
        {
            case SQL_HANDLE_DBC :
            case SQL_HANDLE_STMT :
                zldDiagSetRetCode( aHandleType, aHandle, sRet );
                
                sErrorData = stlGetLastErrorData( &sErrorStack );
                
                if( sErrorData != NULL )
                {
                    (void)zldDiagAdds( aHandleType,
                                       aHandle,
                                       SQL_NO_ROW_NUMBER,
                                       SQL_NO_COLUMN_NUMBER,
                                       &sErrorStack );
                }
                break;
            default :
                break;
        }
    }

    return sRet;
}

SQLRETURN SQL_API SQLCloseCursor( SQLHSTMT aStatementHandle )
{
    SQLRETURN       sRet        = SQL_ERROR;
    zlcDbc        * sDbc        = NULL;
    zlsStmt       * sStmt       = NULL;
    stlErrorData  * sErrorData  = NULL;
    stlErrorStack   sErrorStack;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sStmt = (zlsStmt*)aStatementHandle;

    STL_TRY_THROW( sStmt != NULL,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sStmt->mType == SQL_HANDLE_STMT,
                   RAMP_ERR_INVALID_HANDLE );

    sDbc = sStmt->mParentDbc;

    STL_TRY( zldDiagClear( SQL_HANDLE_STMT,
                           aStatementHandle,
                           &sErrorStack ) == STL_SUCCESS );

    STL_TRY( zlcCheckSession( sDbc,
                              &sErrorStack ) == STL_SUCCESS );
    
    switch( sDbc->mTransition )
    {
        case ZLC_TRANSITION_STATE_C0 :
        case ZLC_TRANSITION_STATE_C1 :
        case ZLC_TRANSITION_STATE_C2 :
        case ZLC_TRANSITION_STATE_C3 :
        case ZLC_TRANSITION_STATE_C4 :
            STL_THROW( RAMP_ERR_INVALID_HANDLE );
            break;
        case ZLC_TRANSITION_STATE_C5 :
        case ZLC_TRANSITION_STATE_C6 :
            break;
    }

    switch( sStmt->mTransition )
    {
        case ZLS_TRANSITION_STATE_S0 :
            STL_THROW( RAMP_ERR_INVALID_HANDLE );
            break;
        case ZLS_TRANSITION_STATE_S1 :
        case ZLS_TRANSITION_STATE_S2 :
        case ZLS_TRANSITION_STATE_S3 :
        case ZLS_TRANSITION_STATE_S4 :
            STL_THROW( RAMP_ERR_INVALID_CURSOR_STATE );
            break;
        case ZLS_TRANSITION_STATE_S9 :
        case ZLS_TRANSITION_STATE_S10 :
        case ZLS_TRANSITION_STATE_S11 :
        case ZLS_TRANSITION_STATE_S12 :
            STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR );
            break;
        default :
            break;
    }

    STL_TRY( zlrClose( sStmt,
                       &sErrorStack ) == STL_SUCCESS );

    if( sStmt->mIsPrepared != STL_TRUE )
    {
        sStmt->mTransition = ZLS_TRANSITION_STATE_S1;
    }
    else
    {
        sStmt->mTransition = ZLS_TRANSITION_STATE_S3;
    }

    if( ( sDbc->mTransition == ZLC_TRANSITION_STATE_C6 ) &&
        ( sDbc->mAttrAutoCommit == STL_TRUE ) )
    {
        sDbc->mTransition = ZLC_TRANSITION_STATE_C5;
    }

    sRet = SQL_SUCCESS;

    zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

    return sRet;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      NULL,
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_CURSOR_STATE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CURSOR_STATE,
                      "No cursor was open on the StatementHandle.",
                      &sErrorStack );
    }

    STL_FINISH;

    if( sRet == SQL_ERROR )
    {
        zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_STMT,
                               aStatementHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }
    }

    return sRet;
}

static SQLRETURN zllColAttribute( SQLHSTMT        aStatementHandle,
                                  SQLUSMALLINT    aColumnNumber,
                                  SQLUSMALLINT    aFieldIdentifier,
                                  SQLPOINTER      aCharacterAttributePtr,
                                  SQLSMALLINT     aBufferLength,
                                  SQLSMALLINT   * aStringLengthPtr,
                                  SQLLEN        * aNumericAttributePtr,
                                  stlErrorStack * aErrorStack )
{
    SQLRETURN       sRet              = SQL_ERROR;
    zlcDbc        * sDbc              = NULL;
    zlsStmt       * sStmt             = NULL;
    zldDesc       * sIrd              = NULL;
    SQLSMALLINT     sSmallInt         = 0;
    SQLINTEGER      sInteger          = 0;
    stlInt32        sStringLength     = 0;
    
    sStmt = (zlsStmt*)aStatementHandle;
    sDbc  = sStmt->mParentDbc;

    STL_TRY( zldDiagClear( SQL_HANDLE_STMT,
                           aStatementHandle,
                           aErrorStack ) == STL_SUCCESS );

    STL_TRY( zlcCheckSession( sDbc,
                              aErrorStack ) == STL_SUCCESS );
    
    switch( sStmt->mTransition )
    {
        case ZLS_TRANSITION_STATE_S0 :
            STL_THROW( RAMP_ERR_INVALID_HANDLE );
            break;
        case ZLS_TRANSITION_STATE_S1 :
            STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR1 );
            break;
        case ZLS_TRANSITION_STATE_S2 :
            STL_TRY_THROW( aFieldIdentifier == SQL_DESC_COUNT,
                           RAMP_ERR_PREPARED_STATEMENT_NOT_A_CURSOR_SPECIFICATION );
            break;
        case ZLS_TRANSITION_STATE_S3 :
            break;
        case ZLS_TRANSITION_STATE_S4 :
            STL_THROW( RAMP_ERR_INVALID_CURSOR_STATE );
            break;
        case ZLS_TRANSITION_STATE_S5 :
        case ZLS_TRANSITION_STATE_S6 :
        case ZLS_TRANSITION_STATE_S7 :
            break;
        case ZLS_TRANSITION_STATE_S8 :
        case ZLS_TRANSITION_STATE_S9 :
        case ZLS_TRANSITION_STATE_S10 :
        case ZLS_TRANSITION_STATE_S11 :
        case ZLS_TRANSITION_STATE_S12 :
            STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR2 );
            break;
    }

    sIrd = &sStmt->mIrd;

    STL_TRY_THROW( aColumnNumber <= sIrd->mCount, RAMP_ERR_INVALID_DESCRIPTOR_INDEX );

    switch( aFieldIdentifier )
    {
        case SQL_DESC_CONCISE_TYPE :
        case SQL_DESC_COUNT :
        case SQL_DESC_FIXED_PREC_SCALE :
        case SQL_DESC_NULLABLE :
        case SQL_DESC_PRECISION :
        case SQL_DESC_SCALE :
        case SQL_DESC_SEARCHABLE :
        case SQL_DESC_TYPE :
        case SQL_DESC_UNNAMED :
        case SQL_DESC_UNSIGNED :
        case SQL_DESC_UPDATABLE :
            STL_TRY( zldDescGetField( sIrd,
                                      aColumnNumber,
                                      aFieldIdentifier,
                                      &sSmallInt,
                                      0,
                                      NULL,
                                      aErrorStack ) == STL_SUCCESS );
            if( aNumericAttributePtr != NULL )
            {
                *aNumericAttributePtr = sSmallInt;
            }
            break;
        case SQL_DESC_AUTO_UNIQUE_VALUE :
        case SQL_DESC_CASE_SENSITIVE :
        case SQL_DESC_NUM_PREC_RADIX :
            STL_TRY( zldDescGetField( sIrd,
                                      aColumnNumber,
                                      aFieldIdentifier,
                                      &sInteger,
                                      0,
                                      NULL,
                                      aErrorStack ) == STL_SUCCESS );
            if( aNumericAttributePtr != NULL )
            {
                *aNumericAttributePtr = sInteger;
            }
            break;
        case SQL_DESC_DISPLAY_SIZE :
        case SQL_DESC_LENGTH :
        case SQL_DESC_OCTET_LENGTH :
            STL_TRY( zldDescGetField( sIrd,
                                      aColumnNumber,
                                      aFieldIdentifier,
                                      (void*)aNumericAttributePtr,
                                      0,
                                      NULL,
                                      aErrorStack ) == STL_SUCCESS );
            break;
        case SQL_DESC_BASE_COLUMN_NAME :
        case SQL_DESC_BASE_TABLE_NAME :
        case SQL_DESC_CATALOG_NAME :
        case SQL_DESC_LABEL :
        case SQL_DESC_LITERAL_PREFIX :
        case SQL_DESC_LITERAL_SUFFIX :
        case SQL_DESC_LOCAL_TYPE_NAME :
        case SQL_DESC_NAME :
        case SQL_DESC_SCHEMA_NAME :
        case SQL_DESC_TABLE_NAME :
        case SQL_DESC_TYPE_NAME :
            STL_TRY_THROW( aBufferLength >= 0, RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH );
            
            STL_TRY( zldDescGetField( sIrd,
                                      aColumnNumber,
                                      aFieldIdentifier,
                                      aCharacterAttributePtr,
                                      aBufferLength,
                                      &sStringLength,
                                      aErrorStack ) == STL_SUCCESS );

            if( aStringLengthPtr != NULL )
            {
                *aStringLengthPtr = sStringLength;
            }
            
            STL_TRY_THROW( aBufferLength > sStringLength, RAMP_SUCCESS_WITH_INFO );            
            break;
        default :
            STL_THROW( RAMP_ERR_INVALID_DESCRIPTOR_FIELD_IDENTIFIER );
            break;
    }

    sRet = STL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }
    
    STL_CATCH( RAMP_SUCCESS_WITH_INFO )
    {
        sRet = SQL_SUCCESS_WITH_INFO;

        stlPushError( STL_ERROR_LEVEL_WARNING,
                      ZLE_ERRCODE_WARNING_STRING_DATA_RIGHT_TRUNCATED,
                      "The buffer *CharacterAttributePtr was not large enough to "
                      "return the entire string value, so the string value was truncated. "
                      "The length of the untruncated string value is returned in "
                      "*StringLengthPtr",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR1 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      "The function was called prior to calling SQLPrepare, SQLExecDirect, "
                      "or a catalog function for the StatementHandle.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR2 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_PREPARED_STATEMENT_NOT_A_CURSOR_SPECIFICATION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_PREPARED_STATEMENT_NOT_A_CURSOR_SPECIFICATION,
                      "The statement associated with the StatementHandle did not return "
                      "a result set and FieldIdentifier was not SQL_DESC_COUNT. "
                      "There were no columns to describe.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_CURSOR_STATE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CURSOR_STATE,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_DESCRIPTOR_INDEX )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_DESCRIPTOR_INDEX,
                      "The value specified for the argument ColumnNumber was "
                      "greater than the number of columns in the result set.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_DESCRIPTOR_FIELD_IDENTIFIER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_DESCRIPTOR_INDEX,
                      "The value specified for the argument FieldIdentifier was not "
                      "one of the defined values and was not an implementation-defined "
                      "value.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_STRING_OR_BUFFER_LENGTH,
                      "*CharacterAttributePtr is a character string, and "
                      "BufferLength was less than 0.",
                      aErrorStack );
    }

    STL_FINISH;

    return sRet;
}

SQLRETURN SQL_API SQLColAttribute( SQLHSTMT      aStatementHandle,
                                   SQLUSMALLINT  aColumnNumber,
                                   SQLUSMALLINT  aFieldIdentifier,
                                   SQLPOINTER    aCharacterAttributePtr,
                                   SQLSMALLINT   aBufferLength,
                                   SQLSMALLINT * aStringLengthPtr,
                                   SQLLEN      * aNumericAttributePtr )
{
    SQLRETURN       sRet  = SQL_ERROR;
    zlsStmt       * sStmt = NULL;
    stlErrorStack   sErrorStack;
    stlErrorData  * sErrorData = NULL;
    stlRegionMark   sMark;
    stlInt32        sState = 0;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sStmt = (zlsStmt*)aStatementHandle;

    STL_TRY_THROW( sStmt != NULL,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sStmt->mType == SQL_HANDLE_STMT,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( stlMarkRegionMem( &sStmt->mAllocator,
                               &sMark,
                               &sErrorStack ) == STL_SUCCESS );
    sState = 1;

    sRet = zllColAttribute( aStatementHandle,
                            aColumnNumber,
                            aFieldIdentifier,
                            aCharacterAttributePtr,
                            aBufferLength,
                            aStringLengthPtr,
                            aNumericAttributePtr,
                            &sErrorStack );

    STL_TRY( sRet == SQL_SUCCESS );

    sState = 0;
    STL_TRY( stlRestoreRegionMem( &sStmt->mAllocator,
                                  &sMark,
                                  STL_TRUE,
                                  &sErrorStack ) == STL_SUCCESS );

    zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

    return SQL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_FINISH;

    if( ( sRet == SQL_ERROR ) ||
        ( sRet == SQL_SUCCESS_WITH_INFO ) )
    {
        zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_STMT,
                               aStatementHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }

        if( sStmt != NULL )
        {
            zlcCheckSessionAndCleanup( ZLS_STMT_DBC(sStmt) );
        }
    }

    switch( sState )
    {
        case 1 :
            stlRestoreRegionMem( &sStmt->mAllocator, &sMark, STL_TRUE, &sErrorStack );
        default :
            break;
    }

    return sRet;
}

SQLRETURN SQL_API SQLColAttributeW( SQLHSTMT      aStatementHandle,
                                    SQLUSMALLINT  aColumnNumber,
                                    SQLUSMALLINT  aFieldIdentifier,
                                    SQLPOINTER    aCharacterAttributePtr,
                                    SQLSMALLINT   aBufferLength,
                                    SQLSMALLINT * aStringLengthPtr,
                                    SQLLEN      * aNumericAttributePtr )
{
    /**
     * aBufferLength
     *     BufferLength refers to the number of bytes available for the value
     *     in CharacterAttributePtr.
     * aStringLengthPtr
     *     Pointer to a buffer in which to return the total number of bytes
     *     (excluding the null-termination byte for character data) available
     *     to return in *CharacterAttributePtr.
     */

    SQLRETURN       sRet  = SQL_ERROR;
    zlsStmt       * sStmt = NULL;
    stlErrorStack   sErrorStack;
    stlErrorData  * sErrorData = NULL;
    stlRegionMark   sMark;
    stlInt32        sState = 0;

    SQLCHAR         sCharacterAttribute[STL_MAX_SQL_IDENTIFIER_LENGTH + 1];
    SQLSMALLINT     sStringLength = 0;
    stlInt64        sBufferLength = 0;
    stlInt64        sLength = 0;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sStmt = (zlsStmt*)aStatementHandle;

    STL_TRY_THROW( sStmt != NULL,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sStmt->mType == SQL_HANDLE_STMT,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( stlMarkRegionMem( &sStmt->mAllocator,
                               &sMark,
                               &sErrorStack ) == STL_SUCCESS );
    sState = 1;

    sRet = zllColAttribute( aStatementHandle,
                            aColumnNumber,
                            aFieldIdentifier,
                            sCharacterAttribute,
                            STL_SIZEOF(sCharacterAttribute),
                            &sStringLength,
                            aNumericAttributePtr,
                            &sErrorStack );

    STL_TRY( SQL_SUCCEEDED(sRet) );

    switch( aFieldIdentifier )
    {
        case SQL_DESC_BASE_COLUMN_NAME :
        case SQL_DESC_BASE_TABLE_NAME :
        case SQL_DESC_CATALOG_NAME :
        case SQL_DESC_LABEL :
        case SQL_DESC_LITERAL_PREFIX :
        case SQL_DESC_LITERAL_SUFFIX :
        case SQL_DESC_LOCAL_TYPE_NAME :
        case SQL_DESC_NAME :
        case SQL_DESC_SCHEMA_NAME :
        case SQL_DESC_TABLE_NAME :
        case SQL_DESC_TYPE_NAME :
            STL_TRY_THROW( aBufferLength >= 0, RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH );

            sBufferLength = aBufferLength / STL_SIZEOF(SQLWCHAR);
            
            STL_TRY( zlvCharToWchar( ZLS_STMT_DBC(sStmt)->mCharacterSet,
                                     sCharacterAttribute,
                                     sStringLength,
                                     (SQLWCHAR*)aCharacterAttributePtr,
                                     sBufferLength,
                                     &sLength,
                                     &sErrorStack ) == STL_SUCCESS );

            if( aStringLengthPtr != NULL )
            {
                *aStringLengthPtr = sLength * STL_SIZEOF(SQLWCHAR);
            }

            STL_TRY_THROW( sLength < aBufferLength, RAMP_SUCCESS_WITH_INFO );

            break;
        default :
            break;
    }

    STL_TRY( sRet == SQL_SUCCESS );

    sState = 0;
    STL_TRY( stlRestoreRegionMem( &sStmt->mAllocator,
                                  &sMark,
                                  STL_TRUE,
                                  &sErrorStack ) == STL_SUCCESS );

    zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

    return SQL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_CATCH( RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_STRING_OR_BUFFER_LENGTH,
                      "*CharacterAttributePtr is a character string, and "
                      "BufferLength was less than 0.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_SUCCESS_WITH_INFO )
    {
        sRet = SQL_SUCCESS_WITH_INFO;

        stlPushError( STL_ERROR_LEVEL_WARNING,
                      ZLE_ERRCODE_WARNING_STRING_DATA_RIGHT_TRUNCATED,
                      "The buffer *CharacterAttributePtr was not large enough to "
                      "return the entire string value, so the string value was truncated. "
                      "The length of the untruncated string value is returned in "
                      "*StringLengthPtr",
                      &sErrorStack );
    }

    STL_FINISH;

    if( ( sRet == SQL_ERROR ) ||
        ( sRet == SQL_SUCCESS_WITH_INFO ) )
    {
        zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_STMT,
                               aStatementHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }

        if( sStmt != NULL )
        {
            zlcCheckSessionAndCleanup( ZLS_STMT_DBC(sStmt) );
        }
    }

    switch( sState )
    {
        case 1 :
            (void)stlRestoreRegionMem( &sStmt->mAllocator, &sMark, STL_TRUE, &sErrorStack );
        default :
            break;
    }

    return sRet;
}

SQLRETURN SQL_API SQLColAttributes( SQLHSTMT      aStatementHandle,
                                    SQLUSMALLINT  aColumnNumber,
                                    SQLUSMALLINT  aFieldIdentifier,
                                    SQLPOINTER    aCharacterAttributePtr,
                                    SQLSMALLINT   aBufferLength,
                                    SQLSMALLINT * aStringLengthPtr,
                                    SQLLEN      * aNumericAttributePtr )
{
    SQLRETURN       sRet  = SQL_ERROR;
    zlsStmt       * sStmt = NULL;
    stlErrorStack   sErrorStack;
    stlErrorData  * sErrorData = NULL;
    stlRegionMark   sMark;
    stlInt32        sState = 0;
    SQLUSMALLINT    sFieldIdentifier;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sStmt = (zlsStmt*)aStatementHandle;

    STL_TRY_THROW( sStmt != NULL,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sStmt->mType == SQL_HANDLE_STMT,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( stlMarkRegionMem( &sStmt->mAllocator,
                               &sMark,
                               &sErrorStack ) == STL_SUCCESS );
    sState = 1;

    switch( aFieldIdentifier )
    {
        case SQL_COLUMN_COUNT :
            sFieldIdentifier = SQL_DESC_COUNT;
            break;
        case SQL_COLUMN_NAME :
            sFieldIdentifier = SQL_DESC_NAME;
            break;
        case SQL_COLUMN_TYPE :
            sFieldIdentifier = SQL_DESC_TYPE;
            break;
        case SQL_COLUMN_LENGTH :
            sFieldIdentifier = SQL_DESC_LENGTH;
            break;
        case SQL_COLUMN_PRECISION :
            sFieldIdentifier = SQL_DESC_PRECISION;
            break;
        case SQL_COLUMN_SCALE :
            sFieldIdentifier = SQL_DESC_SCALE;
            break;
        case SQL_COLUMN_DISPLAY_SIZE :
            sFieldIdentifier = SQL_DESC_DISPLAY_SIZE;
            break;
        case SQL_COLUMN_NULLABLE :
            sFieldIdentifier = SQL_DESC_NULLABLE;
            break;
        case SQL_COLUMN_UNSIGNED :
            sFieldIdentifier = SQL_DESC_UNSIGNED;
            break;
        case SQL_COLUMN_UPDATABLE :
            sFieldIdentifier = SQL_DESC_UPDATABLE;
            break;
        case SQL_COLUMN_AUTO_INCREMENT :
            sFieldIdentifier = SQL_DESC_AUTO_UNIQUE_VALUE;
            break;
        case SQL_COLUMN_CASE_SENSITIVE :
            sFieldIdentifier = SQL_DESC_CASE_SENSITIVE;
            break;
        case SQL_COLUMN_SEARCHABLE :
            sFieldIdentifier = SQL_DESC_SEARCHABLE;
            break;
        case SQL_COLUMN_TYPE_NAME :
            sFieldIdentifier = SQL_DESC_TYPE_NAME;
            break;
        case SQL_COLUMN_TABLE_NAME :
            sFieldIdentifier = SQL_DESC_TABLE_NAME;
            break;
        case SQL_COLUMN_LABEL :
            sFieldIdentifier = SQL_DESC_LABEL;
            break;
        case SQL_COLUMN_QUALIFIER_NAME :
            sFieldIdentifier = SQL_DESC_SCHEMA_NAME;
            break;
        default :
            sFieldIdentifier = aFieldIdentifier;
            break;
    }

    sRet = zllColAttribute( aStatementHandle,
                            aColumnNumber,
                            sFieldIdentifier,
                            aCharacterAttributePtr,
                            aBufferLength,
                            aStringLengthPtr,
                            aNumericAttributePtr,
                            &sErrorStack );

    STL_TRY( sRet == SQL_SUCCESS );

    sState = 0;
    STL_TRY( stlRestoreRegionMem( &sStmt->mAllocator,
                                  &sMark,
                                  STL_TRUE,
                                  &sErrorStack ) == STL_SUCCESS );

    zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

    return SQL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_FINISH;

    if( ( sRet == SQL_ERROR ) ||
        ( sRet == SQL_SUCCESS_WITH_INFO ) )
    {
        zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_STMT,
                               aStatementHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }

        if( sStmt != NULL )
        {
            zlcCheckSessionAndCleanup( ZLS_STMT_DBC(sStmt) );
        }
    }

    switch( sState )
    {
        case 1 :
            stlRestoreRegionMem( &sStmt->mAllocator, &sMark, STL_TRUE, &sErrorStack );
        default :
            break;
    }

    return sRet;
}

SQLRETURN SQL_API SQLColAttributesW( SQLHSTMT      aStatementHandle,
                                     SQLUSMALLINT  aColumnNumber,
                                     SQLUSMALLINT  aFieldIdentifier,
                                     SQLPOINTER    aCharacterAttributePtr,
                                     SQLSMALLINT   aBufferLength,
                                     SQLSMALLINT * aStringLengthPtr,
                                     SQLLEN      * aNumericAttributePtr )
{
    /**
     * aBufferLength
     *     BufferLength refers to the number of bytes available for the value
     *     in CharacterAttributePtr.
     * aStringLengthPtr
     *     Pointer to a buffer in which to return the total number of bytes
     *     (excluding the null-termination byte for character data) available
     *     to return in *CharacterAttributePtr.
     */

    SQLRETURN       sRet  = SQL_ERROR;
    zlsStmt       * sStmt = NULL;
    stlErrorStack   sErrorStack;
    stlErrorData  * sErrorData = NULL;
    stlRegionMark   sMark;
    stlInt32        sState = 0;

    SQLCHAR         sCharacterAttribute[STL_MAX_SQL_IDENTIFIER_LENGTH + 1];
    SQLSMALLINT     sStringLength = 0;
    stlInt64        sBufferLength = 0;
    stlInt64        sLength = 0;
    SQLUSMALLINT    sFieldIdentifier;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sStmt = (zlsStmt*)aStatementHandle;

    STL_TRY_THROW( sStmt != NULL,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sStmt->mType == SQL_HANDLE_STMT,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( stlMarkRegionMem( &sStmt->mAllocator,
                               &sMark,
                               &sErrorStack ) == STL_SUCCESS );
    sState = 1;

    switch( aFieldIdentifier )
    {
        case SQL_COLUMN_COUNT :
            sFieldIdentifier = SQL_DESC_COUNT;
            break;
        case SQL_COLUMN_NAME :
            sFieldIdentifier = SQL_DESC_NAME;
            break;
        case SQL_COLUMN_TYPE :
            sFieldIdentifier = SQL_DESC_TYPE;
            break;
        case SQL_COLUMN_LENGTH :
            sFieldIdentifier = SQL_DESC_LENGTH;
            break;
        case SQL_COLUMN_PRECISION :
            sFieldIdentifier = SQL_DESC_PRECISION;
            break;
        case SQL_COLUMN_SCALE :
            sFieldIdentifier = SQL_DESC_SCALE;
            break;
        case SQL_COLUMN_DISPLAY_SIZE :
            sFieldIdentifier = SQL_DESC_DISPLAY_SIZE;
            break;
        case SQL_COLUMN_NULLABLE :
            sFieldIdentifier = SQL_DESC_NULLABLE;
            break;
        case SQL_COLUMN_UNSIGNED :
            sFieldIdentifier = SQL_DESC_UNSIGNED;
            break;
        case SQL_COLUMN_UPDATABLE :
            sFieldIdentifier = SQL_DESC_UPDATABLE;
            break;
        case SQL_COLUMN_AUTO_INCREMENT :
            sFieldIdentifier = SQL_DESC_AUTO_UNIQUE_VALUE;
            break;
        case SQL_COLUMN_CASE_SENSITIVE :
            sFieldIdentifier = SQL_DESC_CASE_SENSITIVE;
            break;
        case SQL_COLUMN_SEARCHABLE :
            sFieldIdentifier = SQL_DESC_SEARCHABLE;
            break;
        case SQL_COLUMN_TYPE_NAME :
            sFieldIdentifier = SQL_DESC_TYPE_NAME;
            break;
        case SQL_COLUMN_TABLE_NAME :
            sFieldIdentifier = SQL_DESC_TABLE_NAME;
            break;
        case SQL_COLUMN_LABEL :
            sFieldIdentifier = SQL_DESC_LABEL;
            break;
        case SQL_COLUMN_QUALIFIER_NAME :
            sFieldIdentifier = SQL_DESC_SCHEMA_NAME;
            break;
        default :
            sFieldIdentifier = aFieldIdentifier;
            break;
    }

    sRet = zllColAttribute( aStatementHandle,
                            aColumnNumber,
                            sFieldIdentifier,
                            sCharacterAttribute,
                            STL_SIZEOF(sCharacterAttribute),
                            &sStringLength,
                            aNumericAttributePtr,
                            &sErrorStack );

    STL_TRY( SQL_SUCCEEDED(sRet) );

    switch( sFieldIdentifier )
    {
        case SQL_DESC_BASE_COLUMN_NAME :
        case SQL_DESC_BASE_TABLE_NAME :
        case SQL_DESC_CATALOG_NAME :
        case SQL_DESC_LABEL :
        case SQL_DESC_LITERAL_PREFIX :
        case SQL_DESC_LITERAL_SUFFIX :
        case SQL_DESC_LOCAL_TYPE_NAME :
        case SQL_DESC_NAME :
        case SQL_DESC_SCHEMA_NAME :
        case SQL_DESC_TABLE_NAME :
        case SQL_DESC_TYPE_NAME :
            STL_TRY_THROW( aBufferLength >= 0, RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH );

            sBufferLength = aBufferLength / STL_SIZEOF(SQLWCHAR);
            
            STL_TRY( zlvCharToWchar( ZLS_STMT_DBC(sStmt)->mCharacterSet,
                                     sCharacterAttribute,
                                     sStringLength,
                                     (SQLWCHAR*)aCharacterAttributePtr,
                                     sBufferLength,
                                     &sLength,
                                     &sErrorStack ) == STL_SUCCESS );

            if( aStringLengthPtr != NULL )
            {
                *aStringLengthPtr = sLength * STL_SIZEOF(SQLWCHAR);
            }

            STL_TRY_THROW( sLength < aBufferLength, RAMP_SUCCESS_WITH_INFO );

            break;
        default :
            break;
    }

    STL_TRY( sRet == SQL_SUCCESS );

    sState = 0;
    STL_TRY( stlRestoreRegionMem( &sStmt->mAllocator,
                                  &sMark,
                                  STL_TRUE,
                                  &sErrorStack ) == STL_SUCCESS );

    zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

    return SQL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_CATCH( RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_STRING_OR_BUFFER_LENGTH,
                      "*CharacterAttributePtr is a character string, and "
                      "BufferLength was less than 0.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_SUCCESS_WITH_INFO )
    {
        sRet = SQL_SUCCESS_WITH_INFO;

        stlPushError( STL_ERROR_LEVEL_WARNING,
                      ZLE_ERRCODE_WARNING_STRING_DATA_RIGHT_TRUNCATED,
                      "The buffer *CharacterAttributePtr was not large enough to "
                      "return the entire string value, so the string value was truncated. "
                      "The length of the untruncated string value is returned in "
                      "*StringLengthPtr",
                      &sErrorStack );
    }

    STL_FINISH;

    if( ( sRet == SQL_ERROR ) ||
        ( sRet == SQL_SUCCESS_WITH_INFO ) )
    {
        zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_STMT,
                               aStatementHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }

        if( sStmt != NULL )
        {
            zlcCheckSessionAndCleanup( ZLS_STMT_DBC(sStmt) );
        }
    }

    switch( sState )
    {
        case 1 :
            (void)stlRestoreRegionMem( &sStmt->mAllocator, &sMark, STL_TRUE, &sErrorStack );
        default :
            break;
    }

    return sRet;
}

static SQLRETURN zllColumnPrivileges( SQLHSTMT        aStatementHandle,
                                      SQLCHAR       * aCatalogName,
                                      SQLSMALLINT     aNameLength1,
                                      SQLCHAR       * aSchemaName,
                                      SQLSMALLINT     aNameLength2,
                                      SQLCHAR       * aTableName,
                                      SQLSMALLINT     aNameLength3,
                                      SQLCHAR       * aColumnName,
                                      SQLSMALLINT     aNameLength4,
                                      stlErrorStack * aErrorStack )
{
    SQLRETURN       sRet              = SQL_ERROR;
    zlcDbc        * sDbc              = NULL;
    zlsStmt       * sStmt             = NULL;
    stlInt64        sAffectedRowCount = -1;
    stlBool         sSuccessWithInfo  = STL_FALSE;

    stlInt16        sNameLength1 = 0;
    stlInt16        sNameLength2 = 0;
    stlInt16        sNameLength3 = 0;
    stlInt16        sNameLength4 = 0;

    stlChar         sOp[5];
    stlInt32        sPos = 0;

    stlChar         sBuffer[2048];
    stlInt32        sBufferLength = STL_SIZEOF( sBuffer );

    SQLCHAR * sSELECT   = (SQLCHAR*)"SELECT "
        "  CAST(TABLE_CATALOG  AS VARCHAR(128 OCTETS)) AS TABLE_CAT, "
        "  CAST(TABLE_SCHEMA   AS VARCHAR(128 OCTETS)) AS TABLE_SCHEM, "
        "  CAST(TABLE_NAME     AS VARCHAR(128 OCTETS)) AS TABLE_NAME, "
        "  CAST(COLUMN_NAME    AS VARCHAR(128 OCTETS)) AS COLUMN_NAME, "
        "  CAST(GRANTOR        AS VARCHAR(128 OCTETS)) AS GRANTOR, "
        "  CAST(GRANTEE        AS VARCHAR(128 OCTETS)) AS GRANTEE, "
        "  CAST(PRIVILEGE_TYPE AS VARCHAR(128 OCTETS)) AS PRIVILEGE, "
        "  CAST(CASE IS_GRANTABLE "
        "         WHEN TRUE  THEN 'YES' "
        "         WHEN FALSE THEN 'NO' "
        "         ELSE            'NULL' "
        "         END          AS VARCHAR(3 OCTETS))   AS IS_GRANTABLE "
        "FROM INFORMATION_SCHEMA.COLUMN_PRIVILEGES ";
    SQLCHAR * sWHERE    = (SQLCHAR*)"WHERE 1 = 1 ";
    SQLCHAR * sORDER_BY = (SQLCHAR*)"ORDER BY TABLE_CAT, TABLE_SCHEM, TABLE_NAME, COLUMN_NAME, PRIVILEGE ";
    
    sStmt = (zlsStmt*)aStatementHandle;
    sDbc  = sStmt->mParentDbc;

    STL_TRY( zldDiagClear( SQL_HANDLE_STMT,
                           aStatementHandle,
                           aErrorStack ) == STL_SUCCESS );

    STL_TRY( zlcCheckSession( sDbc,
                              aErrorStack ) == STL_SUCCESS );
    
    switch( sDbc->mTransition )
    {
        case ZLC_TRANSITION_STATE_C0 :
        case ZLC_TRANSITION_STATE_C1 :
        case ZLC_TRANSITION_STATE_C2 :
        case ZLC_TRANSITION_STATE_C3 :
        case ZLC_TRANSITION_STATE_C4 :
            STL_THROW( RAMP_ERR_INVALID_HANDLE );
            break;
        case ZLC_TRANSITION_STATE_C5 :
        case ZLC_TRANSITION_STATE_C6 :
            break;
    }

    switch( sStmt->mTransition )
    {
        case ZLS_TRANSITION_STATE_S1 :
        case ZLS_TRANSITION_STATE_S2 :
        case ZLS_TRANSITION_STATE_S3 :
        case ZLS_TRANSITION_STATE_S4 :
            break;
        case ZLS_TRANSITION_STATE_S5 :
            STL_THROW( RAMP_ERR_INVALID_CURSOR_STATE_1 );
            break;
        case ZLS_TRANSITION_STATE_S6 :
        case ZLS_TRANSITION_STATE_S7 :
            STL_THROW( RAMP_ERR_INVALID_CURSOR_STATE_2 );
            break;
        case ZLS_TRANSITION_STATE_S8 :
        case ZLS_TRANSITION_STATE_S9 :
        case ZLS_TRANSITION_STATE_S10 :
        case ZLS_TRANSITION_STATE_S11 :
        case ZLS_TRANSITION_STATE_S12 :
            STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR );
            break;
    }

    if( aNameLength1 != SQL_NTS )
    {
        STL_TRY_THROW( aNameLength1 >= 0, RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH );

        sNameLength1 = aNameLength1;
    }
    else
    {
        if( aCatalogName != NULL )
        {
            sNameLength1 = stlStrlen((stlChar*)aCatalogName);
        }        
    }

    if( aNameLength2 != SQL_NTS )
    {
        STL_TRY_THROW( aNameLength2 >= 0, RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH );

        sNameLength2 = aNameLength2;
    }
    else
    {
        if( aSchemaName != NULL )
        {
            sNameLength2 = stlStrlen((stlChar*)aSchemaName);
        }        
    }

    if( aNameLength3 != SQL_NTS )
    {
        STL_TRY_THROW( aNameLength3 >= 0, RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH );

        sNameLength3 = aNameLength3;
    }
    else
    {
        if( aTableName != NULL )
        {
            sNameLength3 = stlStrlen((stlChar*)aTableName);
        }        
    }

    if( aNameLength4 != SQL_NTS )
    {
        STL_TRY_THROW( aNameLength4 >= 0, RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH );

        sNameLength4 = aNameLength4;
    }
    else
    {
        if( aColumnName != NULL )
        {
            sNameLength4 = stlStrlen((stlChar*)aColumnName);
        }        
    }

    STL_TRY_THROW( aTableName != NULL, RAMP_ERR_INVALID_USE_OF_NULL_POINTER_1 );
    
    if( sStmt->mMetadataId == STL_TRUE )
    {
        if( aCatalogName != NULL )
        {
            if( (aCatalogName[0] == '\"') &&
                (aCatalogName[sNameLength1-1] == '\"') )
            {
                aCatalogName[sNameLength1-1] = 0;
                aCatalogName++;

                sNameLength1 = sNameLength1 - 2;
            }
            else
            {
                stlToupperString( (stlChar*)aCatalogName, (stlChar*)aCatalogName );
            }
        }
        
        STL_TRY_THROW( aSchemaName != NULL, RAMP_ERR_INVALID_USE_OF_NULL_POINTER_2 );

        if( (aSchemaName[0] == '\"') &&
            (aSchemaName[sNameLength2-1] == '\"') )
        {
            aSchemaName[sNameLength2-1] = 0;
            aSchemaName++;

            sNameLength2 = sNameLength2 - 2;
        }
        else
        {
            stlToupperString( (stlChar*)aSchemaName, (stlChar*)aSchemaName );
        }

        if( (aTableName[0] == '\"') &&
            (aTableName[sNameLength3-1] == '\"') )
        {
            aTableName[sNameLength3-1] = 0;
            aTableName++;

            sNameLength3 = sNameLength3 - 2;
        }
        else
        {
            stlToupperString( (stlChar*)aTableName, (stlChar*)aTableName );
        }

        STL_TRY_THROW( aColumnName != NULL, RAMP_ERR_INVALID_USE_OF_NULL_POINTER_2 );

        if( (aColumnName[0] == '\"') &&
            (aColumnName[sNameLength4-1] == '\"') )
        {
            aColumnName[sNameLength4-1] = 0;
            aColumnName++;

            sNameLength4 = sNameLength4 - 2;
        }
        else
        {
            stlToupperString( (stlChar*)aColumnName, (stlChar*)aColumnName );
        }

        stlStrcpy( sOp, "=" );
    }
    else
    {
        stlStrcpy( sOp, "LIKE" );
    }

    sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos,
                         "%s%s",
                         sSELECT,
                         sWHERE );

    if( (aCatalogName != NULL) && (sNameLength1 > 0) )
    {
        sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos,
                             " AND TABLE_CATALOG = '" );
            
        stlMemcpy( sBuffer + sPos, aCatalogName, sNameLength1 );
        sPos += sNameLength1;
        
        sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos, "' " );
    }

    if( (aSchemaName != NULL) && (sNameLength2 > 0) )
    {
        sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos,
                             " AND TABLE_SCHEMA = '" );
            
        stlMemcpy( sBuffer + sPos, aSchemaName, sNameLength2 );
        sPos += sNameLength2;

        sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos, "' " );
    }

    sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos,
                         " AND TABLE_NAME = '" );
            
    stlMemcpy( sBuffer + sPos, aTableName, sNameLength3 );
    sPos += sNameLength3;

    sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos, "' " );

    if( (aColumnName != NULL) && (sNameLength4 > 0) )
    {
        sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos,
                             " AND COLUMN_NAME %s '", sOp );
            
        stlMemcpy( sBuffer + sPos, aColumnName, sNameLength4 );
        sPos += sNameLength4;

        sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos, "' " );
    }

    sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos,
                         "%s",
                         sORDER_BY );

    STL_TRY( zlxExecDirect( sStmt,
                            sBuffer,
                            &sAffectedRowCount,
                            &sSuccessWithInfo,
                            aErrorStack ) == STL_SUCCESS );

    STL_TRY_THROW( sSuccessWithInfo == STL_FALSE, RAMP_SUCCESS_WITH_INFO );

    sRet = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_STRING_OR_BUFFER_LENGTH,
                      "The value of one of the length arguments was less than 0 "
                      "but not equal to SQL_NTS.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_CURSOR_STATE_1 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CURSOR_STATE,
                      "A cursor was open on the StatementHandle, "
                      "but SQLFetch or SQLFetchScroll had not been called.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_CURSOR_STATE_2 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CURSOR_STATE,
                      "A cursor was open on the StatementHandle, "
                      "and SQLFetch or SQLFetchScroll had been called.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_USE_OF_NULL_POINTER_1 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_USE_OF_NULL_POINTER,
                      "The TableName argument was a null pointer.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_USE_OF_NULL_POINTER_2 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_USE_OF_NULL_POINTER,
                      "The SQL_ATTR_METADATA_ID statement attribute was set to SQL_TRUE, "
                      "and the SchemaName or ColumnName argument was a null pointer.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_CATCH( RAMP_SUCCESS_WITH_INFO )
    {
        sRet = SQL_SUCCESS_WITH_INFO;
    }

    STL_FINISH;

    return sRet;
}

SQLRETURN SQL_API SQLColumnPrivileges( SQLHSTMT      aStatementHandle,
                                       SQLCHAR     * aCatalogName,
                                       SQLSMALLINT   aNameLength1,
                                       SQLCHAR     * aSchemaName,
                                       SQLSMALLINT   aNameLength2,
                                       SQLCHAR     * aTableName,
                                       SQLSMALLINT   aNameLength3,
                                       SQLCHAR     * aColumnName,
                                       SQLSMALLINT   aNameLength4 )
{
    SQLRETURN       sRet  = SQL_ERROR;
    zlsStmt       * sStmt = NULL;
    stlErrorStack   sErrorStack;
    stlErrorData  * sErrorData = NULL;
    stlRegionMark   sMark;
    stlInt32        sState = 0;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sStmt = (zlsStmt*)aStatementHandle;

    STL_TRY_THROW( sStmt != NULL,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sStmt->mType == SQL_HANDLE_STMT,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( stlMarkRegionMem( &sStmt->mAllocator,
                               &sMark,
                               &sErrorStack ) == STL_SUCCESS );
    sState = 1;

    sRet = zllColumnPrivileges( aStatementHandle,
                                aCatalogName,
                                aNameLength1,
                                aSchemaName,
                                aNameLength2,
                                aTableName,
                                aNameLength3,
                                aColumnName,
                                aNameLength4,
                                &sErrorStack );

    STL_TRY( sRet == SQL_SUCCESS );

    sState = 0;
    STL_TRY( stlRestoreRegionMem( &sStmt->mAllocator,
                                  &sMark,
                                  STL_TRUE,
                                  &sErrorStack ) == STL_SUCCESS );

    zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

    return SQL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_FINISH;

    if( ( sRet == SQL_ERROR ) ||
        ( sRet == SQL_SUCCESS_WITH_INFO ) )
    {
        zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_STMT,
                               aStatementHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }

        if( sStmt != NULL )
        {
            zlcCheckSessionAndCleanup( ZLS_STMT_DBC(sStmt) );
        }
    }

    switch( sState )
    {
        case 1 :
            stlRestoreRegionMem( &sStmt->mAllocator, &sMark, STL_TRUE, &sErrorStack );
        default :
            break;
    }

    return sRet;
}

SQLRETURN SQL_API SQLColumnPrivilegesW( SQLHSTMT      aStatementHandle,
                                        SQLWCHAR    * aCatalogName,
                                        SQLSMALLINT   aNameLength1,
                                        SQLWCHAR    * aSchemaName,
                                        SQLSMALLINT   aNameLength2,
                                        SQLWCHAR    * aTableName,
                                        SQLSMALLINT   aNameLength3,
                                        SQLWCHAR    * aColumnName,
                                        SQLSMALLINT   aNameLength4 )
{
    /**
     * aNameLength1
     *     Length of *CatalogName, in characters.
     * aNameLength2
     *     Length of *SchemaName, in characters.
     * aNameLength3
     *     Length of *TableName, in characters.
     * aNameLength4
     *     Length of *ColumnName, in characters.
     */

    SQLRETURN       sRet  = SQL_ERROR;
    zlsStmt       * sStmt = NULL;
    stlErrorStack   sErrorStack;
    stlErrorData  * sErrorData = NULL;
    stlRegionMark   sMark;
    stlInt32        sState = 0;

    stlInt64        sBufferLength;
    SQLCHAR       * sCatalogName = NULL;
    stlInt64        sNameLength1 = 0;
    SQLCHAR       * sSchemaName = NULL;
    stlInt64        sNameLength2 = 0;
    SQLCHAR       * sTableName = NULL;
    stlInt64        sNameLength3 = 0;
    SQLCHAR       * sColumnName = NULL;
    stlInt64        sNameLength4 = 0;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sStmt = (zlsStmt*)aStatementHandle;

    STL_TRY_THROW( sStmt != NULL,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sStmt->mType == SQL_HANDLE_STMT,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( stlMarkRegionMem( &sStmt->mAllocator,
                               &sMark,
                               &sErrorStack ) == STL_SUCCESS );
    sState = 1;

    sBufferLength = 0;    
    STL_TRY( zlvAllocCharBuffer( aCatalogName,
                                 aNameLength1,
                                 ZLS_STMT_DBC(sStmt)->mCharacterSet,
                                 &sCatalogName,
                                 &sBufferLength,
                                 &sStmt->mAllocator,
                                 &sErrorStack ) == STL_SUCCESS );
    
    STL_TRY( zlvWcharToChar( aCatalogName,
                             aNameLength1,
                             ZLS_STMT_DBC(sStmt)->mCharacterSet,
                             sCatalogName,
                             sBufferLength,
                             &sNameLength1,
                             &sErrorStack ) == STL_SUCCESS );
    
    sBufferLength = 0;    
    STL_TRY( zlvAllocCharBuffer( aSchemaName,
                                 aNameLength2,
                                 ZLS_STMT_DBC(sStmt)->mCharacterSet,
                                 &sSchemaName,
                                 &sBufferLength,
                                 &sStmt->mAllocator,
                                 &sErrorStack ) == STL_SUCCESS );
    
    STL_TRY( zlvWcharToChar( aSchemaName,
                             aNameLength2,
                             ZLS_STMT_DBC(sStmt)->mCharacterSet,
                             sSchemaName,
                             sBufferLength,
                             &sNameLength2,
                             &sErrorStack ) == STL_SUCCESS );
    
    sBufferLength = 0;    
    STL_TRY( zlvAllocCharBuffer( aTableName,
                                 aNameLength3,
                                 ZLS_STMT_DBC(sStmt)->mCharacterSet,
                                 &sTableName,
                                 &sBufferLength,
                                 &sStmt->mAllocator,
                                 &sErrorStack ) == STL_SUCCESS );
    
    STL_TRY( zlvWcharToChar( aTableName,
                             aNameLength3,
                             ZLS_STMT_DBC(sStmt)->mCharacterSet,
                             sTableName,
                             sBufferLength,
                             &sNameLength3,
                             &sErrorStack ) == STL_SUCCESS );
    
    sBufferLength = 0;    
    STL_TRY( zlvAllocCharBuffer( aColumnName,
                                 aNameLength4,
                                 ZLS_STMT_DBC(sStmt)->mCharacterSet,
                                 &sColumnName,
                                 &sBufferLength,
                                 &sStmt->mAllocator,
                                 &sErrorStack ) == STL_SUCCESS );
    
    STL_TRY( zlvWcharToChar( aColumnName,
                             aNameLength4,
                             ZLS_STMT_DBC(sStmt)->mCharacterSet,
                             sColumnName,
                             sBufferLength,
                             &sNameLength4,
                             &sErrorStack ) == STL_SUCCESS );
    
    sRet = zllColumnPrivileges( aStatementHandle,
                                sCatalogName,
                                sNameLength1,
                                sSchemaName,
                                sNameLength2,
                                sTableName,
                                sNameLength3,
                                sColumnName,
                                sNameLength4,
                                &sErrorStack );

    STL_TRY( sRet == SQL_SUCCESS );

    sState = 0;
    STL_TRY( stlRestoreRegionMem( &sStmt->mAllocator,
                                  &sMark,
                                  STL_TRUE,
                                  &sErrorStack ) == STL_SUCCESS );

    zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

    return SQL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_FINISH;

    if( ( sRet == SQL_ERROR ) ||
        ( sRet == SQL_SUCCESS_WITH_INFO ) )
    {
        zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_STMT,
                               aStatementHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }

        if( sStmt != NULL )
        {
            zlcCheckSessionAndCleanup( ZLS_STMT_DBC(sStmt) );
        }
    }

    switch( sState )
    {
        case 1 :
            (void)stlRestoreRegionMem( &sStmt->mAllocator, &sMark, STL_TRUE, &sErrorStack );
        default :
            break;
    }

    return sRet;
}

static SQLRETURN zllColumns( SQLHSTMT        aStatementHandle,
                             SQLCHAR       * aCatalogName,
                             SQLSMALLINT     aNameLength1,
                             SQLCHAR       * aSchemaName,
                             SQLSMALLINT     aNameLength2,
                             SQLCHAR       * aTableName,
                             SQLSMALLINT     aNameLength3,
                             SQLCHAR       * aColumnName,
                             SQLSMALLINT     aNameLength4,
                             stlErrorStack * aErrorStack )
{
    SQLRETURN       sRet              = SQL_ERROR;
    zlcDbc        * sDbc              = NULL;
    zlsStmt       * sStmt             = NULL;
    stlInt64        sAffectedRowCount = -1;
    stlBool         sSuccessWithInfo  = STL_FALSE;

    stlChar       * sBuffer           = NULL;
    stlSize         sBufferLength     = 20648 + 1;

    stlInt16        sNameLength1 = 0;
    stlInt16        sNameLength2 = 0;
    stlInt16        sNameLength3 = 0;
    stlInt16        sNameLength4 = 0;

    stlChar         sOp[5];
    stlInt32        sPos = 0;

    SQLCHAR * sSELECT   = (SQLCHAR*)"SELECT "
                                    "  CAST(TABLE_CATALOG AS VARCHAR(128 OCTETS)) AS TABLE_CAT, "
                                    "  CAST(TABLE_SCHEMA  AS VARCHAR(128 OCTETS)) AS TABLE_SCHEM, "
                                    "  CAST(TABLE_NAME    AS VARCHAR(128 OCTETS)) AS TABLE_NAME, "
                                    "  CAST(COLUMN_NAME   AS VARCHAR(128 OCTETS)) AS COLUMN_NAME, "
                                    "  CAST(CASE DATA_TYPE "
                                    "         WHEN 'ROWID'                       THEN -101 "
                                    "         WHEN 'NATIVE_BIGINT'               THEN -5 "
                                    "         WHEN 'BINARY LONG VARYING'         THEN -4 "
                                    "         WHEN 'BINARY VARYING'              THEN -3 "
                                    "         WHEN 'BINARY'                      THEN -2 "
                                    "         WHEN 'CHARACTER LONG VARYING'      THEN -1 "
                                    "         WHEN 'CHARACTER'                   THEN 1 "
                                    "         WHEN 'NUMBER'                      THEN 2 "
                                    "         WHEN 'NUMERIC'                     THEN 2 "
                                    "         WHEN 'DECIMAL'                     THEN 3 "
                                    "         WHEN 'NATIVE_INTEGER'              THEN 4 "
                                    "         WHEN 'NATIVE_SMALLINT'             THEN 5 "
                                    "         WHEN 'FLOAT'                       THEN 6 "
                                    "         WHEN 'NATIVE_REAL'                 THEN 7 "
                                    "         WHEN 'NATIVE_DOUBLE'               THEN 8 "
                                    "         WHEN 'CHARACTER VARYING'           THEN 12 "
                                    "         WHEN 'BIT'                         THEN 14 "
                                    "         WHEN 'BIT VARYING'                 THEN 15 "
                                    "         WHEN 'BOOLEAN'                     THEN 16 "
                                    "         WHEN 'REF'                         THEN 20 "
                                    "         WHEN 'BINARY LARGE OBJECT'         THEN 30 "
                                    "         WHEN 'CHARACTER LARGE OBJECT'      THEN 40 "
                                    "         WHEN 'DATE'                        THEN 91 "
                                    "         WHEN 'TIME WITHOUT TIME ZONE'      THEN 92 "
                                    "         WHEN 'TIMESTAMP WITHOUT TIME ZONE' THEN 93 "
                                    "         WHEN 'TIME WITH TIME ZONE'         THEN 94 "
                                    "         WHEN 'TIMESTAMP WITH TIME ZONE'    THEN 95 "
                                    "         WHEN 'INTERVAL YEAR'               THEN 101 "
                                    "         WHEN 'INTERVAL MONTH'              THEN 102 "
                                    "         WHEN 'INTERVAL YEAR TO MONTH'      THEN 107 "
                                    "         WHEN 'INTERVAL DAY'                THEN 103 "
                                    "         WHEN 'INTERVAL HOUR'               THEN 104 "
                                    "         WHEN 'INTERVAL MINUTE'             THEN 105 "
                                    "         WHEN 'INTERVAL SECOND'             THEN 106 "
                                    "         WHEN 'INTERVAL DAY TO HOUR'        THEN 108 "
                                    "         WHEN 'INTERVAL DAY TO MINUTE'      THEN 109 "
                                    "         WHEN 'INTERVAL DAY TO SECOND'      THEN 110 "
                                    "         WHEN 'INTERVAL HOUR TO MINUTE'     THEN 111 "
                                    "         WHEN 'INTERVAL HOUR TO SECOND'     THEN 112 "
                                    "         WHEN 'INTERVAL MINUTE TO SECOND'   THEN 113 "
                                    "         ELSE NULL "
                                    "         END AS NATIVE_SMALLINT) DATA_TYPE, "
                                    "  CAST(DATA_TYPE AS VARCHAR(128 OCTETS)) AS TYPE_NAME, "
                                    "  CAST(CASE "
                                    "         WHEN DATA_TYPE = 'CHARACTER' "
                                    "           OR DATA_TYPE = 'CHARACTER VARYING' "
                                    "           OR DATA_TYPE = 'CHARACTER LONG VARYING' "
                                    "           OR DATA_TYPE = 'CHARACTER LARGE OBJECT' "
                                    "           OR DATA_TYPE = 'BINARY' "
                                    "           OR DATA_TYPE = 'BINARY VARYING' "
                                    "           OR DATA_TYPE = 'BINARY LONG VARYING' "
                                    "           OR DATA_TYPE = 'BINARY LARGE OBJECT' "
                                    "           OR DATA_TYPE = 'BIT' "
                                    "           OR DATA_TYPE = 'BIT VARYING' "
                                    "           THEN CHARACTER_MAXIMUM_LENGTH "
                                    "         WHEN DATA_TYPE = 'NUMBER' "
                                    "           OR DATA_TYPE = 'NUMERIC' "
                                    "           OR DATA_TYPE = 'DECIMAL' "
                                    "           OR DATA_TYPE = 'FLOAT' "
                                    "           OR DATA_TYPE = 'NATIVE_SMALLINT' "
                                    "           OR DATA_TYPE = 'NATIVE_INTEGER' "
                                    "           OR DATA_TYPE = 'NATIVE_BIGINT' "
                                    "           OR DATA_TYPE = 'NATIVE_REAL' "
                                    "           OR DATA_TYPE = 'NATIVE_DOUBLE' "
                                    "           THEN NUMERIC_PRECISION "
                                    "         WHEN DATA_TYPE = 'ROWID'   THEN 23 "
                                    "         WHEN DATA_TYPE = 'BOOLEAN' THEN 5 "
                                    "         WHEN DATA_TYPE = 'DATE'    THEN 10 "
                                    "         WHEN DATA_TYPE = 'TIME WITHOUT TIME ZONE' THEN "
                                    "           CASE "
                                    "             WHEN DATETIME_PRECISION > 0 THEN 9 + DATETIME_PRECISION "
                                    "             ELSE 8 "
                                    "             END "
                                    "         WHEN DATA_TYPE = 'TIMESTAMP WITHOUT TIME ZONE' THEN "
                                    "           CASE "
                                    "             WHEN DATETIME_PRECISION > 0 THEN 20 + DATETIME_PRECISION "
                                    "             ELSE 19 "
                                    "             END "
                                    "         WHEN DATA_TYPE = 'TIME WITH TIME ZONE' THEN "
                                    "           CASE "
                                    "             WHEN DATETIME_PRECISION > 0 THEN 15 + DATETIME_PRECISION "
                                    "             ELSE 14 "
                                    "             END "
                                    "         WHEN DATA_TYPE = 'TIMESTAMP WITH TIME ZONE' THEN "
                                    "           CASE "
                                    "             WHEN DATETIME_PRECISION > 0 THEN 26 + DATETIME_PRECISION "
                                    "             ELSE 25 "
                                    "             END "
                                    "         WHEN DATA_TYPE = 'INTERVAL YEAR' "
                                    "           OR DATA_TYPE = 'INTERVAL MONTH' THEN INTERVAL_PRECISION "
                                    "         WHEN DATA_TYPE = 'INTERVAL YEAR TO MONTH' THEN 3 + INTERVAL_PRECISION "
                                    "         WHEN DATA_TYPE = 'INTERVAL DAY' "
                                    "           OR DATA_TYPE = 'INTERVAL HOUR' "
                                    "           OR DATA_TYPE = 'INTERVAL MINUTE' THEN INTERVAL_PRECISION "
                                    "         WHEN DATA_TYPE = 'INTERVAL SECOND' THEN "
                                    "           CASE "
                                    "           WHEN INTERVAL_PRECISION > 0 "
                                    "             THEN INTERVAL_PRECISION + DATETIME_PRECISION + 1 "
                                    "           ELSE INTERVAL_PRECISION "
                                    "           END "
                                    "         WHEN DATA_TYPE = 'INTERVAL DAY TO HOUR' THEN 3 + INTERVAL_PRECISION "
                                    "         WHEN DATA_TYPE = 'INTERVAL DAY TO MINUTE' THEN 6 + INTERVAL_PRECISION "
                                    "         WHEN DATA_TYPE = 'INTERVAL DAY TO SECOND' THEN "
                                    "           CASE "
                                    "           WHEN INTERVAL_PRECISION > 0 "
                                    "             THEN 10 + INTERVAL_PRECISION + DATETIME_PRECISION "
                                    "           ELSE 9 + INTERVAL_PRECISION "
                                    "           END "
                                    "         WHEN DATA_TYPE = 'INTERVAL HOUR TO MINUTE' THEN 3 + INTERVAL_PRECISION "
                                    "         WHEN DATA_TYPE = 'INTERVAL HOUR TO SECOND' THEN "
                                    "           CASE "
                                    "           WHEN INTERVAL_PRECISION > 0 "
                                    "             THEN 7 + INTERVAL_PRECISION + DATETIME_PRECISION "
                                    "           ELSE 6 + INTERVAL_PRECISION "
                                    "           END "
                                    "         WHEN DATA_TYPE = 'INTERVAL MINUTE TO SECOND' THEN "
                                    "           CASE "
                                    "           WHEN INTERVAL_PRECISION > 0 "
                                    "             THEN 4 + INTERVAL_PRECISION + DATETIME_PRECISION "
                                    "           ELSE 3 + INTERVAL_PRECISION "
                                    "           END "
                                    "       END AS NATIVE_INTEGER) AS COLUMN_SIZE, "
                                    "  CAST(CASE "
                                    "         WHEN DATA_TYPE = 'CHARACTER' "
                                    "           OR DATA_TYPE = 'CHARACTER VARYING' "
                                    "           OR DATA_TYPE = 'CHARACTER LONG VARYING' "
                                    "           OR DATA_TYPE = 'CHARACTER LARGE OBJECT' "
                                    "           OR DATA_TYPE = 'BINARY' "
                                    "           OR DATA_TYPE = 'BINARY VARYING' "
                                    "           OR DATA_TYPE = 'BINARY LONG VARYING' "
                                    "           OR DATA_TYPE = 'BINARY LARGE OBJECT' "
                                    "           OR DATA_TYPE = 'BIT' "
                                    "           OR DATA_TYPE = 'BIT VARYING' "
                                    "           THEN CHARACTER_OCTET_LENGTH "
                                    "         WHEN DATA_TYPE = 'DECIMAL' "
                                    "           OR DATA_TYPE = 'NUMERIC' "
                                    "           OR DATA_TYPE = 'NUMBER' "
                                    "           THEN NUMERIC_PRECISION + 2 "
                                    "         WHEN DATA_TYPE = 'BOOLEAN'         THEN 1 "
                                    "         WHEN DATA_TYPE = 'NATIVE_SMALLINT' THEN 2 "
                                    "         WHEN DATA_TYPE = 'NATIVE_INTEGER'  THEN 4 "
                                    "         WHEN DATA_TYPE = 'NATIVE_BIGINT'   THEN 8 "
                                    "         WHEN DATA_TYPE = 'NATIVE_REAL'     THEN 4 "
                                    "         WHEN DATA_TYPE = 'FLOAT'           THEN 8 "
                                    "         WHEN DATA_TYPE = 'NATIVE_DOUBLE'   THEN 8 "
                                    "         WHEN DATA_TYPE = 'DATE' "
                                    "           OR DATA_TYPE = 'TIME WITHOUT TIME ZONE' "
                                    "           THEN 6 "
                                    "         WHEN DATA_TYPE = 'TIME WITH TIME ZONE' "
                                    "           OR DATA_TYPE = 'TIMESTAMP WITHOUT TIME ZONE' "
                                    "           THEN 16 "
                                    "         WHEN DATA_TYPE = 'TIMESTAMP WITH TIME ZONE' "
                                    "           THEN 20 "
                                    "         WHEN DATA_TYPE = 'INTERVAL YEAR TO MONTH' "
                                    "           OR DATA_TYPE = 'INTERVAL YEAR' "
                                    "           OR DATA_TYPE = 'INTERVAL MONTH' "
                                    "           OR DATA_TYPE = 'INTERVAL YEAR TO MONTH' "
                                    "           OR DATA_TYPE = 'INTERVAL DAY' "
                                    "           OR DATA_TYPE = 'INTERVAL HOUR' "
                                    "           OR DATA_TYPE = 'INTERVAL MINUTE' "
                                    "           OR DATA_TYPE = 'INTERVAL SECOND' "
                                    "           OR DATA_TYPE = 'INTERVAL DAY TO HOUR' "
                                    "           OR DATA_TYPE = 'INTERVAL DAY TO MINUTE' "
                                    "           OR DATA_TYPE = 'INTERVAL DAY TO SECOND'"
                                    "           OR DATA_TYPE = 'INTERVAL HOUR TO MINUTE' "
                                    "           OR DATA_TYPE = 'INTERVAL HOUR TO SECOND'"
                                    "           OR DATA_TYPE = 'INTERVAL MINUTE TO SECOND' "
                                    "           OR DATA_TYPE = 'INTERVAL DAY TO SECOND' "
                                    "           THEN 34 "
                                    "         WHEN DATA_TYPE = 'ROWID' "
                                    "           THEN 23 "
                                    "       END AS NATIVE_INTEGER) AS BUFFER_LENGTH, "
                                    "  CAST(CASE "
                                    "         WHEN DATA_TYPE = 'TIME WITHOUT TIME ZONE' "
                                    "           OR DATA_TYPE = 'TIMESTAMP WITHOUT TIME ZONE' "
                                    "           OR DATA_TYPE = 'TIME WITH TIME ZONE' "
                                    "           OR DATA_TYPE = 'TIMESTAMP WITH TIME ZONE' "
                                    "           THEN DATETIME_PRECISION "
                                    "         WHEN DATA_TYPE = 'NUMERIC' "
                                    "           OR DATA_TYPE = 'NUMBER' "
                                    "           OR DATA_TYPE = 'DECIMAL' "
                                    "           THEN NUMERIC_SCALE "
                                    "         WHEN DATA_TYPE = 'NATIVE_SMALLINT' "
                                    "           OR DATA_TYPE = 'NATIVE_INTEGER' "
                                    "           OR DATA_TYPE = 'NATIVE_BIGINT' "
                                    "           THEN 0 "
                                    "         WHEN DATA_TYPE = 'INTERVAL SECOND' "
                                    "           OR DATA_TYPE = 'INTERVAL DAY TO SECOND' "
                                    "           OR DATA_TYPE = 'INTERVAL HOUR TO SECOND' "
                                    "           OR DATA_TYPE = 'INTERVAL MINUTE TO SECOND' "
                                    "           THEN DATETIME_PRECISION "
                                    "         ELSE NULL "
                                    "       END AS NATIVE_SMALLINT) AS DECIMAL_DIGITS, "
                                    "  CAST(NUMERIC_PRECISION_RADIX AS NATIVE_SMALLINT) AS NUM_PREC_RADIX, "
                                    "  CAST(CASE "
                                    "         WHEN IS_NULLABLE = 'TRUE' THEN 1 "
                                    "         ELSE 0 "
                                    "         END AS NATIVE_SMALLINT) AS NULLABLE, "
                                    "  COMMENTS AS REMARKS, "
                                    "  CAST(COLUMN_DEFAULT AS VARCHAR(2048 OCTETS)) AS COLUMN_DEF, "
                                    "  CAST(CASE DATA_TYPE "
                                    "         WHEN 'ROWID'                       THEN -101 "
                                    "         WHEN 'NATIVE_BIGINT'               THEN -5 "
                                    "         WHEN 'BINARY LONG VARYING'         THEN -4 "
                                    "         WHEN 'BINARY VARYING'              THEN -3 "
                                    "         WHEN 'BINARY'                      THEN -2 "
                                    "         WHEN 'CHARACTER LONG VARYING'      THEN -1 "
                                    "         WHEN 'CHARACTER'                   THEN 1 "
                                    "         WHEN 'NUMBER'                      THEN 2 "
                                    "         WHEN 'NUMERIC'                     THEN 2 "
                                    "         WHEN 'DECIMAL'                     THEN 3 "
                                    "         WHEN 'NATIVE_INTEGER'              THEN 4 "
                                    "         WHEN 'NATIVE_SMALLINT'             THEN 5 "
                                    "         WHEN 'FLOAT'                       THEN 6 "
                                    "         WHEN 'NATIVE_REAL'                 THEN 7 "
                                    "         WHEN 'NATIVE_DOUBLE'               THEN 8 "
                                    "         WHEN 'CHARACTER VARYING'           THEN 12 "
                                    "         WHEN 'BIT'                         THEN 14 "
                                    "         WHEN 'BIT VARYING'                 THEN 15 "
                                    "         WHEN 'BOOLEAN'                     THEN 16 "
                                    "         WHEN 'REF'                         THEN 20 "
                                    "         WHEN 'BINARY LARGE OBJECT'         THEN 30 "
                                    "         WHEN 'CHARACTER LARGE OBJECT'      THEN 40 "
                                    "         WHEN 'DATE'                        THEN 9 "
                                    "         WHEN 'TIME WITHOUT TIME ZONE'      THEN 9 "
                                    "         WHEN 'TIMESTAMP WITHOUT TIME ZONE' THEN 9 "
                                    "         WHEN 'TIME WITH TIME ZONE'         THEN 9 "
                                    "         WHEN 'TIMESTAMP WITH TIME ZONE'    THEN 9 "
                                    "         WHEN 'INTERVAL YEAR'               THEN 10 "
                                    "         WHEN 'INTERVAL MONTH'              THEN 10 "
                                    "         WHEN 'INTERVAL YEAR TO MONTH'      THEN 10 "
                                    "         WHEN 'INTERVAL DAY'                THEN 10 "
                                    "         WHEN 'INTERVAL HOUR'               THEN 10 "
                                    "         WHEN 'INTERVAL MINUTE'             THEN 10 "
                                    "         WHEN 'INTERVAL SECOND'             THEN 10 "
                                    "         WHEN 'INTERVAL DAY TO HOUR'        THEN 10 "
                                    "         WHEN 'INTERVAL DAY TO MINUTE'      THEN 10 "
                                    "         WHEN 'INTERVAL DAY TO SECOND'      THEN 10 "
                                    "         WHEN 'INTERVAL HOUR TO MINUTE'     THEN 10 "
                                    "         WHEN 'INTERVAL HOUR TO SECOND'     THEN 10 "
                                    "         WHEN 'INTERVAL MINUTE TO SECOND'   THEN 10 "
                                    "         END AS NATIVE_SMALLINT) AS SQL_DATA_TYPE, "
                                    "  CAST(CASE DATA_TYPE "
                                    "         WHEN 'DATE'                        THEN 1 "
                                    "         WHEN 'TIME WITHOUT TIME ZONE'      THEN 2 "
                                    "         WHEN 'TIMESTAMP WITHOUT TIME ZONE' THEN 3 "
                                    "         WHEN 'TIME WITH TIME ZONE'         THEN 4 "
                                    "         WHEN 'TIMESTAMP WITH TIME ZONE'    THEN 5 "
                                    "         WHEN 'INTERVAL YEAR'               THEN 1 "
                                    "         WHEN 'INTERVAL MONTH'              THEN 2 "
                                    "         WHEN 'INTERVAL YEAR TO MONTH'      THEN 7 "
                                    "         WHEN 'INTERVAL DAY'                THEN 3 "
                                    "         WHEN 'INTERVAL HOUR'               THEN 4 "
                                    "         WHEN 'INTERVAL MINUTE'             THEN 5 "
                                    "         WHEN 'INTERVAL SECOND'             THEN 6 "
                                    "         WHEN 'INTERVAL DAY TO HOUR'        THEN 8 "
                                    "         WHEN 'INTERVAL DAY TO MINUTE'      THEN 9 "
                                    "         WHEN 'INTERVAL DAY TO SECOND'      THEN 10 "
                                    "         WHEN 'INTERVAL HOUR TO MINUTE'     THEN 11 "
                                    "         WHEN 'INTERVAL HOUR TO SECOND'     THEN 12 "
                                    "         WHEN 'INTERVAL MINUTE TO SECOND'   THEN 13 "
                                    "         ELSE NULL "
                                    "         END AS NATIVE_SMALLINT) AS SQL_DATETIME_SUB, "
                                    "  CAST(CHARACTER_OCTET_LENGTH AS NATIVE_INTEGER) AS CHAR_OCTET_LENGTH, "
                                    "  CAST(ORDINAL_POSITION AS NATIVE_INTEGER ) AS ORDINAL_POSITION, "
                                    "  CAST(CASE "
                                    "         WHEN IS_NULLABLE = 'TRUE' THEN 'YES' "
                                    "         ELSE 'NO' "
                                    "         END AS VARCHAR(254 OCTETS)) AS IS_NULLABLE "
                                    "FROM INFORMATION_SCHEMA.COLUMNS ";
    SQLCHAR * sWHERE    = (SQLCHAR*)"WHERE 1 = 1 ";
    SQLCHAR * sORDER_BY = (SQLCHAR*)"ORDER BY TABLE_CAT, TABLE_SCHEM, TABLE_NAME, ORDINAL_POSITION";
    
    sStmt = (zlsStmt*)aStatementHandle;
    sDbc  = sStmt->mParentDbc;

    STL_TRY( zldDiagClear( SQL_HANDLE_STMT,
                           aStatementHandle,
                           aErrorStack ) == STL_SUCCESS );

    STL_TRY( zlcCheckSession( sDbc,
                              aErrorStack ) == STL_SUCCESS );
    
    switch( sDbc->mTransition )
    {
        case ZLC_TRANSITION_STATE_C0 :
        case ZLC_TRANSITION_STATE_C1 :
        case ZLC_TRANSITION_STATE_C2 :
        case ZLC_TRANSITION_STATE_C3 :
        case ZLC_TRANSITION_STATE_C4 :
            STL_THROW( RAMP_ERR_INVALID_HANDLE );
            break;
        case ZLC_TRANSITION_STATE_C5 :
        case ZLC_TRANSITION_STATE_C6 :
            break;
    }

    switch( sStmt->mTransition )
    {
        case ZLS_TRANSITION_STATE_S1 :
        case ZLS_TRANSITION_STATE_S2 :
        case ZLS_TRANSITION_STATE_S3 :
        case ZLS_TRANSITION_STATE_S4 :
            break;
        case ZLS_TRANSITION_STATE_S5 :
            STL_THROW( RAMP_ERR_INVALID_CURSOR_STATE_1 );
            break;
        case ZLS_TRANSITION_STATE_S6 :
        case ZLS_TRANSITION_STATE_S7 :
            STL_THROW( RAMP_ERR_INVALID_CURSOR_STATE_2 );
            break;
        case ZLS_TRANSITION_STATE_S8 :
        case ZLS_TRANSITION_STATE_S9 :
        case ZLS_TRANSITION_STATE_S10 :
        case ZLS_TRANSITION_STATE_S11 :
        case ZLS_TRANSITION_STATE_S12 :
            STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR );
            break;
    }

    STL_TRY( stlAllocRegionMem( &sStmt->mAllocator,
                                sBufferLength,
                                (void**)&sBuffer,
                                aErrorStack ) == STL_SUCCESS );

    if( aNameLength1 != SQL_NTS )
    {
        STL_TRY_THROW( aNameLength1 >= 0, RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH );

        sNameLength1 = aNameLength1;
    }
    else
    {
        if (aCatalogName != NULL)
        {
            sNameLength1 = stlStrlen((stlChar*)aCatalogName);
        }        
    }

    if( aNameLength2 != SQL_NTS )
    {
        STL_TRY_THROW( aNameLength2 >= 0, RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH );

        sNameLength2 = aNameLength2;
    }
    else
    {
        if (aSchemaName != NULL)
        {
            sNameLength2 = stlStrlen((stlChar*)aSchemaName);
        }        
    }

    if( aNameLength3 != SQL_NTS )
    {
        STL_TRY_THROW( aNameLength3 >= 0, RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH );

        sNameLength3 = aNameLength3;
    }
    else
    {
        if (aTableName != NULL)
        {
            sNameLength3 = stlStrlen((stlChar*)aTableName);
        }        
    }

    if( aNameLength4 != SQL_NTS )
    {
        STL_TRY_THROW( aNameLength4 >= 0, RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH );

        sNameLength4 = aNameLength4;
    }
    else
    {
        if (aColumnName != NULL)
        {
            sNameLength4 = stlStrlen((stlChar*)aColumnName);
        }        
    }

    if( sStmt->mMetadataId == STL_TRUE )
    {
        if( aCatalogName != NULL )
        {
            if( (aCatalogName[0] == '\"') &&
                (aCatalogName[sNameLength1-1] == '\"') )
            {
                aCatalogName[sNameLength1-1] = 0;
                aCatalogName++;

                sNameLength1 = sNameLength1 - 2;
            }
            else
            {
                stlToupperString( (stlChar*)aCatalogName, (stlChar*)aCatalogName );
            }
        }

        STL_TRY_THROW( aSchemaName != NULL, RAMP_ERR_INVALID_USE_OF_NULL_POINTER );

        if( (aSchemaName[0] == '\"') &&
            (aSchemaName[sNameLength2-1] == '\"') )
        {
            aSchemaName[sNameLength2-1] = 0;
            aSchemaName++;

            sNameLength2 = sNameLength2 - 2;
        }
        else
        {
            stlToupperString( (stlChar*)aSchemaName, (stlChar*)aSchemaName );
        }
        
        STL_TRY_THROW( aTableName != NULL, RAMP_ERR_INVALID_USE_OF_NULL_POINTER );

        if( (aTableName[0] == '\"') &&
            (aTableName[sNameLength3-1] == '\"') )
        {
            aTableName[sNameLength3-1] = 0;
            aTableName++;

            sNameLength3 = sNameLength3 - 2;
        }
        else
        {
            stlToupperString( (stlChar*)aTableName, (stlChar*)aTableName );
        }
        
        STL_TRY_THROW( aColumnName != NULL, RAMP_ERR_INVALID_USE_OF_NULL_POINTER );

        if( (aColumnName[0] == '\"') &&
            (aColumnName[sNameLength4-1] == '\"') )
        {
            aColumnName[sNameLength4-1] = 0;
            aColumnName++;

            sNameLength4 = sNameLength4 - 2;
        }
        else
        {
            stlToupperString( (stlChar*)aColumnName, (stlChar*)aColumnName );
        }
        
        stlStrcpy( sOp, "=" );
    }
    else
    {
        stlStrcpy( sOp, "LIKE" );
    }

    sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos,
                         "%s%s",
                         sSELECT,
                         sWHERE );

    if( (aCatalogName != NULL) && (sNameLength1 > 0) )
    {
        sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos,
                             " AND CURRENT_CATALOG = '" );
            
        stlMemcpy( sBuffer + sPos, aCatalogName, sNameLength1 );
        sPos += sNameLength1;

        sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos, "' " );
    }

    if( (aSchemaName != NULL) && (sNameLength2 > 0) )
    {
        sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos,
                             " AND TABLE_SCHEMA %s '", sOp );
            
        stlMemcpy( sBuffer + sPos, aSchemaName, sNameLength2 );
        sPos += sNameLength2;

        sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos, "' " );
    }

    if( (aTableName != NULL) && (sNameLength3 > 0) )
    {
        sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos,
                             " AND TABLE_NAME %s '", sOp );
            
        stlMemcpy( sBuffer + sPos, aTableName, sNameLength3 );
        sPos += sNameLength3;

        sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos, "' " );
    }

    if( (aColumnName != NULL) && (sNameLength4 > 0) )
    {
        sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos,
                             " AND COLUMN_NAME %s '", sOp );
            
        stlMemcpy( sBuffer + sPos, aColumnName, sNameLength4 );
        sPos += sNameLength4;

        sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos, "' " );
    }

    sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos,
                         "%s",
                         sORDER_BY );

    STL_TRY( zlxExecDirect( sStmt,
                            sBuffer,
                            &sAffectedRowCount,
                            &sSuccessWithInfo,
                            aErrorStack ) == STL_SUCCESS );

    STL_TRY_THROW( sSuccessWithInfo == STL_FALSE, RAMP_SUCCESS_WITH_INFO );

    sRet = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_STRING_OR_BUFFER_LENGTH,
                      "The value of one of the length arguments was less than 0 "
                      "but not equal to SQL_NTS.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_CURSOR_STATE_1 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CURSOR_STATE,
                      "A cursor was open on the StatementHandle, "
                      "but SQLFetch or SQLFetchScroll had not been called.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_CURSOR_STATE_2 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CURSOR_STATE,
                      "A cursor was open on the StatementHandle, "
                      "and SQLFetch or SQLFetchScroll had been called.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_USE_OF_NULL_POINTER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_USE_OF_NULL_POINTER,
                      "The SQL_ATTR_METADATA_ID statement attribute was set to SQL_TRUE, "
                      "and the SchemaName or TableName or ColumnName "
                      "argument was a null pointer.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_CATCH( RAMP_SUCCESS_WITH_INFO )
    {
        sRet = SQL_SUCCESS_WITH_INFO;
    }

    STL_FINISH;

    return sRet;
}

SQLRETURN SQL_API SQLColumns( SQLHSTMT      aStatementHandle,
                              SQLCHAR     * aCatalogName,
                              SQLSMALLINT   aNameLength1,
                              SQLCHAR     * aSchemaName,
                              SQLSMALLINT   aNameLength2,
                              SQLCHAR     * aTableName,
                              SQLSMALLINT   aNameLength3,
                              SQLCHAR     * aColumnName,
                              SQLSMALLINT   aNameLength4 )
{
    SQLRETURN       sRet  = SQL_ERROR;
    zlsStmt       * sStmt = NULL;
    stlErrorStack   sErrorStack;
    stlErrorData  * sErrorData = NULL;
    stlRegionMark   sMark;
    stlInt32        sState = 0;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sStmt = (zlsStmt*)aStatementHandle;

    STL_TRY_THROW( sStmt != NULL,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sStmt->mType == SQL_HANDLE_STMT,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( stlMarkRegionMem( &sStmt->mAllocator,
                               &sMark,
                               &sErrorStack ) == STL_SUCCESS );
    sState = 1;

    sRet = zllColumns( aStatementHandle,
                       aCatalogName,
                       aNameLength1,
                       aSchemaName,
                       aNameLength2,
                       aTableName,
                       aNameLength3,
                       aColumnName,
                       aNameLength4,
                       &sErrorStack );
    
    STL_TRY( sRet == SQL_SUCCESS );

    sState = 0;
    STL_TRY( stlRestoreRegionMem( &sStmt->mAllocator,
                                  &sMark,
                                  STL_TRUE,
                                  &sErrorStack ) == STL_SUCCESS );

    zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

    return SQL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_FINISH;

    if( ( sRet == SQL_ERROR ) ||
        ( sRet == SQL_SUCCESS_WITH_INFO ) )
    {
        zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_STMT,
                               aStatementHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }

        if( sStmt != NULL )
        {
            zlcCheckSessionAndCleanup( ZLS_STMT_DBC(sStmt) );
        }
    }

    switch( sState )
    {
        case 1 :
            (void)stlRestoreRegionMem( &sStmt->mAllocator, &sMark, STL_TRUE, &sErrorStack );
        default :
            break;
    }

    return sRet;
}

SQLRETURN SQL_API SQLColumnsW( SQLHSTMT      aStatementHandle,
                               SQLWCHAR    * aCatalogName,
                               SQLSMALLINT   aNameLength1,
                               SQLWCHAR    * aSchemaName,
                               SQLSMALLINT   aNameLength2,
                               SQLWCHAR    * aTableName,
                               SQLSMALLINT   aNameLength3,
                               SQLWCHAR    * aColumnName,
                               SQLSMALLINT   aNameLength4 )
{
    /**
     * aNameLength1
     *     Length of *CatalogName, in characters.
     * aNameLength2
     *     Length of *SchemaName, in characters.
     * aNameLength3
     *     Length of *TableName, in characters.
     * aNameLength4
     *     Length of *ColumnName, in characters.
     */

    SQLRETURN       sRet  = SQL_ERROR;
    zlsStmt       * sStmt = NULL;
    stlErrorStack   sErrorStack;
    stlErrorData  * sErrorData = NULL;
    stlRegionMark   sMark;
    stlInt32        sState = 0;

    stlInt64        sBufferLength;
    SQLCHAR       * sCatalogName = NULL;
    stlInt64        sNameLength1 = 0;
    SQLCHAR       * sSchemaName = NULL;
    stlInt64        sNameLength2 = 0;
    SQLCHAR       * sTableName = NULL;
    stlInt64        sNameLength3 = 0;
    SQLCHAR       * sColumnName = NULL;
    stlInt64        sNameLength4 = 0;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sStmt = (zlsStmt*)aStatementHandle;

    STL_TRY_THROW( sStmt != NULL,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sStmt->mType == SQL_HANDLE_STMT,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( stlMarkRegionMem( &sStmt->mAllocator,
                               &sMark,
                               &sErrorStack ) == STL_SUCCESS );
    sState = 1;

    sBufferLength = 0;    
    STL_TRY( zlvAllocCharBuffer( aCatalogName,
                                 aNameLength1,
                                 ZLS_STMT_DBC(sStmt)->mCharacterSet,
                                 &sCatalogName,
                                 &sBufferLength,
                                 &sStmt->mAllocator,
                                 &sErrorStack ) == STL_SUCCESS );
    
    STL_TRY( zlvWcharToChar( aCatalogName,
                             aNameLength1,
                             ZLS_STMT_DBC(sStmt)->mCharacterSet,
                             sCatalogName,
                             sBufferLength,
                             &sNameLength1,
                             &sErrorStack ) == STL_SUCCESS );
    
    sBufferLength = 0;    
    STL_TRY( zlvAllocCharBuffer( aSchemaName,
                                 aNameLength2,
                                 ZLS_STMT_DBC(sStmt)->mCharacterSet,
                                 &sSchemaName,
                                 &sBufferLength,
                                 &sStmt->mAllocator,
                                 &sErrorStack ) == STL_SUCCESS );
    
    STL_TRY( zlvWcharToChar( aSchemaName,
                             aNameLength2,
                             ZLS_STMT_DBC(sStmt)->mCharacterSet,
                             sSchemaName,
                             sBufferLength,
                             &sNameLength2,
                             &sErrorStack ) == STL_SUCCESS );
    
    sBufferLength = 0;    
    STL_TRY( zlvAllocCharBuffer( aTableName,
                                 aNameLength3,
                                 ZLS_STMT_DBC(sStmt)->mCharacterSet,
                                 &sTableName,
                                 &sBufferLength,
                                 &sStmt->mAllocator,
                                 &sErrorStack ) == STL_SUCCESS );
    
    STL_TRY( zlvWcharToChar( aTableName,
                             aNameLength3,
                             ZLS_STMT_DBC(sStmt)->mCharacterSet,
                             sTableName,
                             sBufferLength,
                             &sNameLength3,
                             &sErrorStack ) == STL_SUCCESS );
    
    sBufferLength = 0;    
    STL_TRY( zlvAllocCharBuffer( aColumnName,
                                 aNameLength4,
                                 ZLS_STMT_DBC(sStmt)->mCharacterSet,
                                 &sColumnName,
                                 &sBufferLength,
                                 &sStmt->mAllocator,
                                 &sErrorStack ) == STL_SUCCESS );
    
    STL_TRY( zlvWcharToChar( aColumnName,
                             aNameLength4,
                             ZLS_STMT_DBC(sStmt)->mCharacterSet,
                             sColumnName,
                             sBufferLength,
                             &sNameLength4,
                             &sErrorStack ) == STL_SUCCESS );
    
    sRet = zllColumns( aStatementHandle,
                       sCatalogName,
                       sNameLength1,
                       sSchemaName,
                       sNameLength2,
                       sTableName,
                       sNameLength3,
                       sColumnName,
                       sNameLength4,
                       &sErrorStack );

    STL_TRY( sRet == SQL_SUCCESS );

    sState = 0;
    STL_TRY( stlRestoreRegionMem( &sStmt->mAllocator,
                                  &sMark,
                                  STL_TRUE,
                                  &sErrorStack ) == STL_SUCCESS );

    zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

    return SQL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_FINISH;

    if( ( sRet == SQL_ERROR ) ||
        ( sRet == SQL_SUCCESS_WITH_INFO ) )
    {
        zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_STMT,
                               aStatementHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }

        if( sStmt != NULL )
        {
            zlcCheckSessionAndCleanup( ZLS_STMT_DBC(sStmt) );
        }
    }

    switch( sState )
    {
        case 1 :
            (void)stlRestoreRegionMem( &sStmt->mAllocator, &sMark, STL_TRUE, &sErrorStack );
        default :
            break;
    }

    return sRet;
}

static SQLRETURN zllConnect( SQLHDBC         aConnectionHandle,
                             SQLCHAR       * aServerName,
                             SQLSMALLINT     aNameLength1,
                             SQLCHAR       * aUserName,
                             SQLSMALLINT     aNameLength2,
                             SQLCHAR       * aAuthentication,
                             SQLSMALLINT     aNameLength3,
                             stlErrorStack * aErrorStack )
{
    zlcDbc        * sDbc         = NULL;
    SQLRETURN       sRet         = SQL_ERROR;
    
    stlInt16        sNameLength1 = 0;
    stlInt16        sNameLength2 = 0;
    stlInt16        sNameLength3 = 0;

    stlChar         sHost[STL_MAX_INI_PROPERTY_VALUE];
    stlChar         sCsMode[STL_MAX_INI_PROPERTY_VALUE] = {0,};
    stlInt32        sPort = 0;
    stlChar         sUID[STL_MAX_INI_PROPERTY_VALUE] = { 0, };
    stlChar         sPWD[STL_MAX_INI_PROPERTY_VALUE] = { 0, };
    stlChar       * sUserName = NULL;
    stlChar       * sPassword = NULL;
    zlcProtocolType sProtocolType = ZLC_PROTOCOL_TYPE_MAX;
    stlBool         sSuccessWithInfo = STL_FALSE;

    sDbc = (zlcDbc*)aConnectionHandle;
    
    STL_TRY( zldDiagClear( SQL_HANDLE_DBC,
                           aConnectionHandle,
                           aErrorStack ) == STL_SUCCESS );

    switch( sDbc->mTransition )
    {
        case ZLC_TRANSITION_STATE_C0 :
        case ZLC_TRANSITION_STATE_C1 :
            STL_THROW( RAMP_ERR_INVALID_HANDLE );
            break;
        case ZLC_TRANSITION_STATE_C2 :
            break;
        case ZLC_TRANSITION_STATE_C3 :
        case ZLC_TRANSITION_STATE_C4 :
        case ZLC_TRANSITION_STATE_C5 :
        case ZLC_TRANSITION_STATE_C6 :
            STL_THROW( RAMP_ERR_CONNECTION_NAME_IN_USE );
            break;
    }

    stlMemset( sHost, 0x00, STL_MAX_INI_PROPERTY_VALUE );

#if defined(ODBC_ALL) || defined(ODBC_DA)
    sProtocolType = ZLC_PROTOCOL_TYPE_DA;
#else
    sProtocolType = ZLC_PROTOCOL_TYPE_TCP;
#endif
    
    sNameLength1 = zllGetStringLength( aServerName, aNameLength1 );

    /*
     * DSN 검사
     */
    if( sNameLength1 > 0 )
    {
        STL_TRY( zlcGetInfoFromDSN( sDbc,
                                    (stlChar*)aServerName,
                                    sHost,
                                    &sPort,
                                    sUID,
                                    sPWD,
                                    sCsMode,
                                    &sProtocolType,
                                    aErrorStack ) == STL_SUCCESS );
    }

    sNameLength2 = zllGetStringLength( aUserName, aNameLength2 );

    if (sNameLength2 == 0)
    {
        sUserName = sUID;
        sNameLength2 = zllGetStringLength((SQLCHAR*)sUserName, SQL_NTS);
    }
    else
    {
        sUserName = (stlChar*)aUserName;
    }

    sNameLength3 = zllGetStringLength( aAuthentication, aNameLength3 );
    
    if (sNameLength3 == 0)
    {
        sPassword = sPWD;
        sNameLength3 = zllGetStringLength((SQLCHAR*)sPassword, SQL_NTS);
    }
    else
    {
        sPassword = (stlChar*)aAuthentication;
    }

    STL_TRY_THROW(sNameLength3 > 0, RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH);

    STL_TRY( zlcConnect( sDbc,
                         sHost,
                         sPort,
                         sProtocolType,
                         sCsMode,
                         sUserName,
                         sNameLength2,
                         sPassword,
                         sNameLength3,
                         &sSuccessWithInfo,
                         aErrorStack ) == STL_SUCCESS );

    STL_TRY_THROW( sSuccessWithInfo == STL_FALSE, RAMP_SUCCESS_WITH_INFO );

    sRet = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_CATCH( RAMP_ERR_CONNECTION_NAME_IN_USE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_CONNECTION_NAME_IN_USE,
                      "The specified ConnectionHandle had already been used to establish a connection with a data source, "
                      "and the connection was still open or the user was browsing for a connection.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_STRING_OR_BUFFER_LENGTH,
                      "The value specified for argument NameLength1, NameLength2, or "
                      "NameLength3 was less than 0 but not equal to SQL_NTS.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_SUCCESS_WITH_INFO )
    {
        sRet = SQL_SUCCESS_WITH_INFO;
    }

    STL_FINISH;

    return sRet;
}

SQLRETURN SQL_API SQLConnect( SQLHDBC       aConnectionHandle,
                              SQLCHAR     * aServerName,
                              SQLSMALLINT   aNameLength1,
                              SQLCHAR     * aUserName,
                              SQLSMALLINT   aNameLength2,
                              SQLCHAR     * aAuthentication,
                              SQLSMALLINT   aNameLength3 )
{ 
    SQLRETURN       sRet = SQL_ERROR;
    zlcDbc        * sDbc = NULL;
    stlErrorStack   sErrorStack;
    stlErrorData  * sErrorData = NULL;
    stlRegionMark   sMark;
    stlInt32        sState = 0;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sDbc = (zlcDbc*)aConnectionHandle;
    
    STL_TRY_THROW( sDbc != NULL, RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sDbc->mType == SQL_HANDLE_DBC,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( stlMarkRegionMem( &sDbc->mAllocator,
                               &sMark,
                               &sErrorStack ) == STL_SUCCESS );
    sState = 1;

    sRet = zllConnect( aConnectionHandle,
                       aServerName,
                       aNameLength1,
                       aUserName,
                       aNameLength2,
                       aAuthentication,
                       aNameLength3,
                       &sErrorStack );

    STL_TRY( sRet == SQL_SUCCESS );

    sState = 0;
    STL_TRY( stlRestoreRegionMem( &sDbc->mAllocator,
                                  &sMark,
                                  STL_TRUE,
                                  &sErrorStack ) == STL_SUCCESS );

    zldDiagSetRetCode( SQL_HANDLE_DBC, aConnectionHandle, sRet );

    return SQL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_FINISH;

    if( ( sRet == SQL_ERROR ) ||
        ( sRet == SQL_SUCCESS_WITH_INFO ) )
    {
        zldDiagSetRetCode( SQL_HANDLE_DBC, aConnectionHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_DBC,
                               aConnectionHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }
    }

    switch( sState )
    {
        case 1 :
            (void)stlRestoreRegionMem( &sDbc->mAllocator, &sMark, STL_TRUE, &sErrorStack );
        default :
            break;
    }

    return sRet;
}

SQLRETURN SQL_API SQLConnectW( SQLHDBC       aConnectionHandle,
                               SQLWCHAR    * aServerName,
                               SQLSMALLINT   aNameLength1,
                               SQLWCHAR    * aUserName,
                               SQLSMALLINT   aNameLength2,
                               SQLWCHAR    * aAuthentication,
                               SQLSMALLINT   aNameLength3 )
{
    /**
     * aNameLength1
     *     Length of *ServerName, in characters.
     * aNameLength2
     *     Length of *UserName, in characters.
     * aNameLength3
     *     Length of *Authentication, in characters.
     */

    SQLRETURN       sRet = SQL_ERROR;
    zlcDbc        * sDbc = NULL;
    stlErrorStack   sErrorStack;
    stlErrorData  * sErrorData = NULL;
    stlRegionMark   sMark;
    stlInt32        sState = 0;

    stlInt64  sBufferLength;
    SQLCHAR * sServerName     = NULL;
    stlInt64  sNameLength1    = 0;
    SQLCHAR * sUserName       = NULL;
    stlInt64  sNameLength2    = 0;
    SQLCHAR * sAuthentication = NULL;
    stlInt64  sNameLength3    = 0;

    STL_INIT_ERROR_STACK( &sErrorStack );
    
    sDbc = (zlcDbc*)aConnectionHandle;
    
    STL_TRY_THROW( sDbc != NULL, RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sDbc->mType == SQL_HANDLE_DBC,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( stlMarkRegionMem( &sDbc->mAllocator,
                               &sMark,
                               &sErrorStack ) == STL_SUCCESS );
    sState = 1;

    sBufferLength = 0;    
    STL_TRY( zlvAllocCharBuffer( aServerName,
                                 aNameLength1,
                                 sDbc->mCharacterSet,
                                 &sServerName,
                                 &sBufferLength,
                                 &sDbc->mAllocator,
                                 &sErrorStack ) == STL_SUCCESS );

    STL_TRY( zlvWcharToChar( aServerName,
                             aNameLength1,
                             sDbc->mCharacterSet,
                             sServerName,
                             sBufferLength,
                             &sNameLength1,
                             &sErrorStack ) == STL_SUCCESS );

    sBufferLength = 0;    
    STL_TRY( zlvAllocCharBuffer( aUserName,
                                 aNameLength2,
                                 sDbc->mCharacterSet,
                                 &sUserName,
                                 &sBufferLength,
                                 &sDbc->mAllocator,
                                 &sErrorStack ) == STL_SUCCESS );

    STL_TRY( zlvWcharToChar( aUserName,
                             aNameLength2,
                             sDbc->mCharacterSet,
                             sUserName,
                             sBufferLength,
                             &sNameLength2,
                             &sErrorStack ) == STL_SUCCESS );

    sBufferLength = 0;    
    STL_TRY( zlvAllocCharBuffer( aAuthentication,
                                 aNameLength3,
                                 sDbc->mCharacterSet,
                                 &sAuthentication,
                                 &sBufferLength,
                                 &sDbc->mAllocator,
                                 &sErrorStack ) == STL_SUCCESS );

    STL_TRY( zlvWcharToChar( aAuthentication,
                             aNameLength3,
                             sDbc->mCharacterSet,
                             sAuthentication,
                             sBufferLength,
                             &sNameLength3,
                             &sErrorStack ) == STL_SUCCESS );

    sRet = zllConnect( aConnectionHandle,
                       sServerName,
                       sNameLength1,
                       sUserName,
                       sNameLength2,
                       sAuthentication,
                       sNameLength3,
                       &sErrorStack );

    STL_TRY( sRet == SQL_SUCCESS );

    sState = 0;
    STL_TRY( stlRestoreRegionMem( &sDbc->mAllocator,
                                  &sMark,
                                  STL_TRUE,
                                  &sErrorStack ) == STL_SUCCESS );

    zldDiagSetRetCode( SQL_HANDLE_DBC, aConnectionHandle, sRet );

    return SQL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_FINISH;

    if( ( sRet == SQL_ERROR ) ||
        ( sRet == SQL_SUCCESS_WITH_INFO ) )
    {
        zldDiagSetRetCode( SQL_HANDLE_DBC, aConnectionHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_DBC,
                               aConnectionHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }
    }

    switch( sState )
    {
        case 1 :
            (void)stlRestoreRegionMem( &sDbc->mAllocator, &sMark, STL_TRUE, &sErrorStack );
        default :
            break;
    }

    return sRet;
}

SQLRETURN SQL_API SQLCopyDesc( SQLHDESC aSourceDescHandle,
                               SQLHDESC aTargetDescHandle )
{
    return SQL_ERROR;
}

static SQLRETURN zllDescribeCol( SQLHSTMT        aStatementHandle,
                                 SQLUSMALLINT    aColumnNumber,
                                 SQLCHAR       * aColumnName,
                                 SQLSMALLINT     aBufferLength,
                                 SQLSMALLINT   * aNameLengthPtr,
                                 SQLSMALLINT   * aDataTypePtr,
                                 SQLULEN       * aColumnSizePtr,
                                 SQLSMALLINT   * aDecimalDigitsPtr,
                                 SQLSMALLINT   * aNullablePtr,
                                 stlErrorStack * aErrorStack )
{
    SQLRETURN       sRet        = SQL_ERROR;
    zlsStmt       * sStmt       = NULL;
    zldDesc       * sIrd        = NULL;
    stlInt16        sNameLength = 0;
    stlUInt64       sColumnSize = 0;

    sStmt = (zlsStmt*)aStatementHandle;

    STL_TRY( zldDiagClear( SQL_HANDLE_STMT,
                           aStatementHandle,
                           aErrorStack ) == STL_SUCCESS );

    STL_TRY( zlcCheckSession( sStmt->mParentDbc,
                              aErrorStack ) == STL_SUCCESS );
    
    switch( sStmt->mTransition )
    {
        case ZLS_TRANSITION_STATE_S0 :
            STL_THROW( RAMP_ERR_INVALID_HANDLE );
            break;
        case ZLS_TRANSITION_STATE_S1 :
            STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR1 );
            break;
        case ZLS_TRANSITION_STATE_S2 :
            STL_THROW( RAMP_ERR_PREPARED_STATEMENT_NOT_A_CURSOR_SPECIFICATION );
            break;
        case ZLS_TRANSITION_STATE_S4 :
            STL_THROW( RAMP_ERR_INVALID_CURSOR_STATE );
            break;
        case ZLS_TRANSITION_STATE_S8 :
        case ZLS_TRANSITION_STATE_S9 :
        case ZLS_TRANSITION_STATE_S10 :
        case ZLS_TRANSITION_STATE_S11 :
        case ZLS_TRANSITION_STATE_S12 :
            STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR2 );
            break;
        default :
            break;
    }

    sIrd = &sStmt->mIrd;

    STL_TRY_THROW( aColumnNumber > 0, RAMP_ERR_INVALID_DESCRIPTOR_INDEX1 );
    STL_TRY_THROW( aColumnNumber <= sIrd->mCount, RAMP_ERR_INVALID_DESCRIPTOR_INDEX2 );

    STL_TRY_THROW( aBufferLength >= 0, RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH );
    
    STL_TRY( zliDescribeCol( sStmt,
                             aColumnNumber,
                             (stlChar*)aColumnName,
                             aBufferLength,
                             &sNameLength,
                             aDataTypePtr,
                             &sColumnSize,
                             aDecimalDigitsPtr,
                             aNullablePtr,
                             aErrorStack ) == STL_SUCCESS );

    if( aNameLengthPtr != NULL )
    {
        *aNameLengthPtr = sNameLength;
    }

    if( aColumnSizePtr != NULL )
    {
        *aColumnSizePtr = (SQLULEN)sColumnSize;
    }

    STL_TRY_THROW( aBufferLength > sNameLength, RAMP_SUCCESS_WITH_INFO );

    sRet = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_CATCH( RAMP_SUCCESS_WITH_INFO )
    {
        sRet = SQL_SUCCESS_WITH_INFO;

        stlPushError( STL_ERROR_LEVEL_WARNING,
                      ZLE_ERRCODE_WARNING_STRING_DATA_RIGHT_TRUNCATED,
                      "The buffer *ColumnName was not large enough to return "
                      "the entire column name, so the column name was truncated. "
                      "The length of the untruncated column name is returned in "
                      "*NameLengthPtr.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR1 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      "The function was called prior to calling SQLPrepare, SQLExecute, "
                      "or a catalog function on the statement handle.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR2 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_PREPARED_STATEMENT_NOT_A_CURSOR_SPECIFICATION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_PREPARED_STATEMENT_NOT_A_CURSOR_SPECIFICATION,
                      "The statement associated with the StatementHandle did not "
                      "return a result set. There were no columns to describe.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_CURSOR_STATE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CURSOR_STATE,
                      "No cursor was open on the StatementHandle.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_DESCRIPTOR_INDEX1 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_DESCRIPTOR_INDEX,
                      "The value specified for the argument ColumnNumber was "
                      "less than 1.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_DESCRIPTOR_INDEX2 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_DESCRIPTOR_INDEX,
                      "The value specified for the argument ColumnNumber was "
                      "greater than the number of columns in the result set.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_STRING_OR_BUFFER_LENGTH,
                      "The value specified for the argument BufferLength was less than 0.",
                      aErrorStack );
    }

    STL_FINISH;

    return sRet;
}

SQLRETURN SQL_API SQLDescribeCol( SQLHSTMT       aStatementHandle,
                                  SQLUSMALLINT   aColumnNumber,
                                  SQLCHAR      * aColumnName,
                                  SQLSMALLINT    aBufferLength,
                                  SQLSMALLINT  * aNameLengthPtr,
                                  SQLSMALLINT  * aDataTypePtr,
                                  SQLULEN      * aColumnSizePtr,
                                  SQLSMALLINT  * aDecimalDigitsPtr,
                                  SQLSMALLINT  * aNullablePtr )
{
    SQLRETURN       sRet  = SQL_ERROR;
    zlsStmt       * sStmt = NULL;
    stlErrorStack   sErrorStack;
    stlErrorData  * sErrorData = NULL;
    stlRegionMark   sMark;
    stlInt32        sState = 0;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sStmt = (zlsStmt*)aStatementHandle;

    STL_TRY_THROW( sStmt != NULL,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sStmt->mType == SQL_HANDLE_STMT,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( stlMarkRegionMem( &sStmt->mAllocator,
                               &sMark,
                               &sErrorStack ) == STL_SUCCESS );
    sState = 1;

    sRet = zllDescribeCol( aStatementHandle,
                           aColumnNumber,
                           aColumnName,
                           aBufferLength,
                           aNameLengthPtr,
                           aDataTypePtr,
                           aColumnSizePtr,
                           aDecimalDigitsPtr,
                           aNullablePtr,
                           &sErrorStack );

    STL_TRY( sRet == SQL_SUCCESS );

    sState = 0;
    STL_TRY( stlRestoreRegionMem( &sStmt->mAllocator,
                                  &sMark,
                                  STL_TRUE,
                                  &sErrorStack ) == STL_SUCCESS );

    zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

    return SQL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_FINISH;

    if( ( sRet == SQL_ERROR ) ||
        ( sRet == SQL_SUCCESS_WITH_INFO ) )
    {
        zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_STMT,
                               aStatementHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }

        if( sStmt != NULL )
        {
            zlcCheckSessionAndCleanup( ZLS_STMT_DBC(sStmt) );
        }
    }

    switch( sState )
    {
        case 1 :
            stlRestoreRegionMem( &sStmt->mAllocator, &sMark, STL_TRUE, &sErrorStack );
        default :
            break;
    }

    return sRet;
}

SQLRETURN SQL_API SQLDescribeColW( SQLHSTMT       aStatementHandle,
                                   SQLUSMALLINT   aColumnNumber,
                                   SQLWCHAR     * aColumnName,
                                   SQLSMALLINT    aBufferLength,
                                   SQLSMALLINT  * aNameLengthPtr,
                                   SQLSMALLINT  * aDataTypePtr,
                                   SQLULEN      * aColumnSizePtr,
                                   SQLSMALLINT  * aDecimalDigitsPtr,
                                   SQLSMALLINT  * aNullablePtr )
{
    /**
     * aBufferLength
     *     Length of the *ColumnName buffer, in characters.
     * aNameLengthPtr
     *     Pointer to a buffer in which to return the total number of
     *     characters (excluding the null-termination character) available to
     *     return in *ColumnName.
     */

    SQLRETURN       sRet  = SQL_ERROR;
    zlsStmt       * sStmt = NULL;
    stlErrorStack   sErrorStack;
    stlErrorData  * sErrorData = NULL;
    stlRegionMark   sMark;
    stlInt32        sState = 0;

    SQLCHAR         sColumnName[STL_MAX_SQL_IDENTIFIER_LENGTH + 1];
    SQLSMALLINT     sNameLength = 0;
    stlInt64        sLength = 0;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sStmt = (zlsStmt*)aStatementHandle;

    STL_TRY_THROW( sStmt != NULL,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sStmt->mType == SQL_HANDLE_STMT,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( stlMarkRegionMem( &sStmt->mAllocator,
                               &sMark,
                               &sErrorStack ) == STL_SUCCESS );
    sState = 1;

    sRet = zllDescribeCol( aStatementHandle,
                           aColumnNumber,
                           sColumnName,
                           STL_SIZEOF(sColumnName),
                           &sNameLength,
                           aDataTypePtr,
                           aColumnSizePtr,
                           aDecimalDigitsPtr,
                           aNullablePtr,
                           &sErrorStack );

    STL_TRY( SQL_SUCCEEDED(sRet) );

    STL_TRY( zlvCharToWchar( ZLS_STMT_DBC(sStmt)->mCharacterSet,
                             sColumnName,
                             sNameLength,
                             aColumnName,
                             aBufferLength,
                             &sLength,
                             &sErrorStack ) == STL_SUCCESS );

    if( aNameLengthPtr != NULL )
    {
        *aNameLengthPtr = sLength;
    }

    STL_TRY_THROW( sLength < aBufferLength, RAMP_SUCCESS_WITH_INFO );

    STL_TRY( sRet == SQL_SUCCESS );

    sState = 0;
    STL_TRY( stlRestoreRegionMem( &sStmt->mAllocator,
                                  &sMark,
                                  STL_TRUE,
                                  &sErrorStack ) == STL_SUCCESS );

    zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

    return SQL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_CATCH( RAMP_SUCCESS_WITH_INFO )
    {
        sRet = SQL_SUCCESS_WITH_INFO;

        stlPushError( STL_ERROR_LEVEL_WARNING,
                      ZLE_ERRCODE_WARNING_STRING_DATA_RIGHT_TRUNCATED,
                      "The buffer *ColumnName was not large enough to return "
                      "the entire column name, so the column name was truncated. "
                      "The length of the untruncated column name is returned in "
                      "*NameLengthPtr.",
                      &sErrorStack );
    }

    STL_FINISH;

    if( ( sRet == SQL_ERROR ) ||
        ( sRet == SQL_SUCCESS_WITH_INFO ) )
    {
        zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_STMT,
                               aStatementHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }

        if( sStmt != NULL )
        {
            zlcCheckSessionAndCleanup( ZLS_STMT_DBC(sStmt) );
        }
    }

    switch( sState )
    {
        case 1 :
            (void)stlRestoreRegionMem( &sStmt->mAllocator, &sMark, STL_TRUE, &sErrorStack );
        default :
            break;
    }

    return sRet;
}

SQLRETURN SQL_API SQLDescribeParam( SQLHSTMT       aStatementHandle,
                                    SQLUSMALLINT   aParameterNumber,
                                    SQLSMALLINT  * aDataTypePtr,
                                    SQLULEN      * aParameterSizePtr,
                                    SQLSMALLINT  * aDecimalDigitsPtr,
                                    SQLSMALLINT  * aNullablePtr )
{
    SQLRETURN       sRet        = SQL_ERROR;
    zlsStmt       * sStmt       = NULL;
    stlUInt64       sParamSize  = 0;
    stlErrorData  * sErrorData  = NULL;
    stlErrorStack   sErrorStack;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sStmt = (zlsStmt*)aStatementHandle;

    STL_TRY_THROW( sStmt != NULL, RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sStmt->mType == SQL_HANDLE_STMT,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( zldDiagClear( SQL_HANDLE_STMT,
                           aStatementHandle,
                           &sErrorStack ) == STL_SUCCESS );

    STL_TRY( zlcCheckSession( sStmt->mParentDbc,
                              &sErrorStack ) == STL_SUCCESS );
    
    switch( sStmt->mTransition )
    {
        case ZLS_TRANSITION_STATE_S0 :
            STL_THROW( RAMP_ERR_INVALID_HANDLE );
            break;
        case ZLS_TRANSITION_STATE_S1 :
            STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR1 );
            break;
        case ZLS_TRANSITION_STATE_S8 :
        case ZLS_TRANSITION_STATE_S9 :
        case ZLS_TRANSITION_STATE_S10 :
        case ZLS_TRANSITION_STATE_S11 :
        case ZLS_TRANSITION_STATE_S12 :
            STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR2 );
            break;
        default :
            break;
    }

    STL_TRY_THROW( aParameterNumber > 0, RAMP_ERR_INVALID_DESCRIPTOR_INDEX1 );
    STL_TRY_THROW( aParameterNumber <= sStmt->mParamCount, RAMP_ERR_INVALID_DESCRIPTOR_INDEX2 );

    STL_TRY( zliDescribeParam( sStmt,
                               aParameterNumber,
                               aDataTypePtr,
                               &sParamSize,
                               aDecimalDigitsPtr,
                               aNullablePtr,
                               &sErrorStack ) == STL_SUCCESS );

    if( aParameterSizePtr != NULL )
    {
        *aParameterSizePtr = (SQLULEN)sParamSize;
    }

    sRet = SQL_SUCCESS;

    zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

    return sRet;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR1 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      "The function was called before calling SQLPrepare or "
                      "SQLExecDirect for the StatementHandle.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR2 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      NULL,
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_DESCRIPTOR_INDEX1 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_DESCRIPTOR_INDEX,
                      "The value specified for the argument ParameterNumber is "
                      "less than 1.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_DESCRIPTOR_INDEX2 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_DESCRIPTOR_INDEX,
                      "The value specified for the argument ParameterNumber was "
                      "greater than the number of parameters in "
                      "the associated SQL statement.",
                      &sErrorStack );
    }

    STL_FINISH;

    if( ( sRet == SQL_ERROR ) || ( sRet == SQL_SUCCESS_WITH_INFO ) )
    {
        zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_STMT,
                               aStatementHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }
        
        if( sStmt != NULL )
        {
            zlcCheckSessionAndCleanup( ZLS_STMT_DBC(sStmt) );
        }
    }

    return sRet;
}

SQLRETURN SQL_API SQLDisconnect( SQLHDBC aConnectionHandle )
{
    zlcDbc        * sDbc        = NULL;
    SQLRETURN       sRet        = SQL_ERROR;
    stlErrorData  * sErrorData  = NULL;
    stlErrorStack   sErrorStack;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sDbc = (zlcDbc*)aConnectionHandle;

    STL_TRY_THROW( sDbc != NULL, RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sDbc->mType == SQL_HANDLE_DBC,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( zldDiagClear( SQL_HANDLE_DBC,
                           aConnectionHandle,
                           &sErrorStack ) == STL_SUCCESS );

    switch( sDbc->mTransition )
    {
        case ZLC_TRANSITION_STATE_C0 :
        case ZLC_TRANSITION_STATE_C1 :
            STL_THROW( RAMP_ERR_INVALID_HANDLE );
            break;
        case ZLC_TRANSITION_STATE_C2 :
            STL_THROW( RAMP_ERR_CONNECTION_NOT_OPEN );
            break;
        case ZLC_TRANSITION_STATE_C3 :
        case ZLC_TRANSITION_STATE_C4 :
        case ZLC_TRANSITION_STATE_C5 :
            break;
        case ZLC_TRANSITION_STATE_C6 :
            STL_THROW( RAMP_ERR_INVALID_TRANSACTION_STATE );
            break;
    }

    STL_TRY( zlcDisconnect( sDbc,
                            &sErrorStack ) == STL_SUCCESS );

    sRet = SQL_SUCCESS;

    zldDiagSetRetCode( SQL_HANDLE_DBC, aConnectionHandle, sRet );

    return sRet;

    STL_CATCH( RAMP_ERR_CONNECTION_NOT_OPEN )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_CONNECTION_NOT_OPEN,
                      "The connection specified in the argument ConnectionHandle "
                      "was not open.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_TRANSACTION_STATE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_TRANSACTION_STATE,
                      "There was a transaction in process on the connection specified by "
                      "the argument ConnectionHandle. The transaction remains active.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_FINISH;

    if( sRet == SQL_ERROR )
    {
        zldDiagSetRetCode( SQL_HANDLE_DBC, aConnectionHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );
        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_DBC,
                               aConnectionHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }
    }

    return sRet;
}

static SQLRETURN zllDriverConnect( SQLHDBC         aConnectionHandle,
                                   SQLHWND         aWindowHandle,
                                   SQLCHAR       * aInConnectionString,
                                   SQLSMALLINT     aStringLength1,
                                   SQLCHAR       * aOutConnectionString,
                                   SQLSMALLINT     aBufferLength,
                                   SQLSMALLINT   * aStringLength2Ptr,
                                   SQLUSMALLINT    aDriverCompletion,
                                   stlErrorStack * aErrorStack )
{
    zlcDbc        * sDbc         = NULL;
    zlnEnv        * sEnv         = NULL;
    SQLRETURN       sRet         = SQL_ERROR;
    stlBool         sIsNoData      = STL_FALSE;
    stlInt16        sStringLength1 = 0;
    stlInt16        sStringLength2 = 0;
    stlBool         sSuccessWithInfo = STL_FALSE;

    sDbc = (zlcDbc*)aConnectionHandle;
    sEnv = sDbc->mParentEnv;

    STL_TRY( zldDiagClear( SQL_HANDLE_DBC,
                           aConnectionHandle,
                           aErrorStack ) == STL_SUCCESS );

    switch( sDbc->mTransition )
    {
        case ZLC_TRANSITION_STATE_C0 :
        case ZLC_TRANSITION_STATE_C1 :
            STL_THROW( RAMP_ERR_INVALID_HANDLE );
            break;
        case ZLC_TRANSITION_STATE_C2 :
            break;
        case ZLC_TRANSITION_STATE_C3 :
        case ZLC_TRANSITION_STATE_C4 :
        case ZLC_TRANSITION_STATE_C5 :
        case ZLC_TRANSITION_STATE_C6 :
            STL_THROW( RAMP_ERR_CONNECTION_NAME_IN_USE );
            break;
    }

    sStringLength1 = zllGetStringLength( aInConnectionString,
                                         aStringLength1 );
    switch( aDriverCompletion )
    {
        case SQL_DRIVER_PROMPT :
        case SQL_DRIVER_COMPLETE :
        case SQL_DRIVER_COMPLETE_REQUIRED :
            if( sEnv->mAttrVersion == SQL_OV_ODBC3 )
            {
                STL_TRY_THROW( aWindowHandle != NULL, RAMP_ERR_DIALOG_FAILED );
            }
            break;
        case SQL_DRIVER_NOPROMPT :
            STL_TRY_THROW( sStringLength1 > 0, RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH );
            break;
        default :
            STL_THROW( RAMP_ERR_INVALID_DRIVER_COMPLETION );
            break;
    }

    STL_TRY( zlcDriverConnect( sDbc,
                               (void*)aWindowHandle,
                               (stlChar*)aInConnectionString,
                               sStringLength1,
                               (stlChar*)aOutConnectionString,
                               aBufferLength,
                               &sStringLength2,
                               aDriverCompletion,
                               &sSuccessWithInfo,
                               &sIsNoData,
                               aErrorStack ) == STL_SUCCESS );

    STL_TRY_THROW( sIsNoData != STL_TRUE, RAMP_NO_DATA );

    if( aStringLength2Ptr != NULL )
    {
        *aStringLength2Ptr = sStringLength2;
    }

    STL_TRY_THROW( sSuccessWithInfo == STL_FALSE, RAMP_SUCCESS_WITH_INFO );
    
    if ( aOutConnectionString != NULL )
    {
        STL_TRY_THROW( aBufferLength > sStringLength2, RAMP_WARNING_STRING_DATA_RIGHT_TRUNCATED );
    }
    
    sRet = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_CATCH( RAMP_SUCCESS_WITH_INFO )
    {
        sRet = SQL_SUCCESS_WITH_INFO;
    }

    STL_CATCH( RAMP_WARNING_STRING_DATA_RIGHT_TRUNCATED )
    {
        sRet = SQL_SUCCESS_WITH_INFO;

        stlPushError( STL_ERROR_LEVEL_WARNING,
                      ZLE_ERRCODE_WARNING_STRING_DATA_RIGHT_TRUNCATED,
                      "The buffer *OutConnectionString was not large enough to return the "
                      "entire connection string, so the connection string was truncated. "
                      "The length of the untruncated connection string is returned "
                      "in *StringLength2Ptr.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_CONNECTION_NAME_IN_USE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_CONNECTION_NAME_IN_USE,
                      "The specified ConnectionHandle had already been used to establish a connection with a data source, "
                      "and the connection was still open or the user was browsing for a connection.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_DIALOG_FAILED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_ATTRIBUTE_OPTION_IDENTIFIER,
                      "WindowHandle was a null pointer, and DriverCompletion was not SQL_DRIVER_NO_PROMPT.",
                      aErrorStack );
    }
    
    STL_CATCH( RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_STRING_OR_BUFFER_LENGTH,
                      "The value specified for argument NameLength1, NameLength2, or "
                      "NameLength3 was less than 0 but not equal to SQL_NTS.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_DRIVER_COMPLETION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_DRIVER_COMPLETION,
                      "The value specified for the argument DriverCompletion was not equal "
                      "to SQL_DRIVER_PROMPT, SQL_DRIVER_COMPLETE, "
                      "SQL_DRIVER_COMPLETE_REQUIRED, or SQL_DRIVER_NOPROMPT.",
                      aErrorStack );
    }
    
    STL_CATCH( RAMP_NO_DATA )
    {
        sRet = SQL_NO_DATA;
    }
    
    STL_FINISH;

    return sRet;
}

SQLRETURN SQL_API SQLDriverConnect( SQLHDBC        aConnectionHandle,
                                    SQLHWND        aWindowHandle,
                                    SQLCHAR      * aInConnectionString,
                                    SQLSMALLINT    aStringLength1,
                                    SQLCHAR      * aOutConnectionString,
                                    SQLSMALLINT    aBufferLength,
                                    SQLSMALLINT  * aStringLength2Ptr,
                                    SQLUSMALLINT   aDriverCompletion )
{
    SQLRETURN       sRet = SQL_ERROR;
    zlcDbc        * sDbc = NULL;
    stlErrorStack   sErrorStack;
    stlErrorData  * sErrorData = NULL;
    stlRegionMark   sMark;
    stlInt32        sState = 0;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sDbc = (zlcDbc*)aConnectionHandle;
    
    STL_TRY_THROW( sDbc != NULL, RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sDbc->mType == SQL_HANDLE_DBC,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( stlMarkRegionMem( &sDbc->mAllocator,
                               &sMark,
                               &sErrorStack ) == STL_SUCCESS );
    sState = 1;

    sRet = zllDriverConnect( aConnectionHandle,
                             aWindowHandle,
                             aInConnectionString,
                             aStringLength1,
                             aOutConnectionString,
                             aBufferLength,
                             aStringLength2Ptr,
                             aDriverCompletion,
                             &sErrorStack );

    STL_TRY( sRet == SQL_SUCCESS );

    sState = 0;
    STL_TRY( stlRestoreRegionMem( &sDbc->mAllocator,
                                  &sMark,
                                  STL_TRUE,
                                  &sErrorStack ) == STL_SUCCESS );

    zldDiagSetRetCode( SQL_HANDLE_DBC, aConnectionHandle, sRet );

    return SQL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_FINISH;

    if( ( sRet == SQL_ERROR ) ||
        ( sRet == SQL_SUCCESS_WITH_INFO ) )
    {
        zldDiagSetRetCode( SQL_HANDLE_DBC, aConnectionHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_DBC,
                               aConnectionHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }
    }

    switch( sState )
    {
        case 1 :
            (void)stlRestoreRegionMem( &sDbc->mAllocator, &sMark, STL_TRUE, &sErrorStack );
        default :
            break;
    }

    return sRet;
}

SQLRETURN SQL_API SQLDriverConnectW( SQLHDBC        aConnectionHandle,
                                     SQLHWND        aWindowHandle,
                                     SQLWCHAR     * aInConnectionString,
                                     SQLSMALLINT    aStringLength1,
                                     SQLWCHAR     * aOutConnectionString,
                                     SQLSMALLINT    aBufferLength,
                                     SQLSMALLINT  * aStringLength2Ptr,
                                     SQLUSMALLINT   aDriverCompletion )
{
    /**
     * aStringLength1
     *     Length of *InConnectionString, in characters.
     * aBufferLength
     *     Length of the *OutConnectionString buffer, in characters.
     *     (Note that this means this argument does NOT have to be an
     *     even number.)
     * aStringLength2Ptr
     *     Pointer to a buffer in which to return the total number of
     *     characters (excluding the null-termination character) available to
     *     return in *OutConnectionString.
     */

    SQLRETURN       sRet = SQL_ERROR;
    zlcDbc        * sDbc = NULL;
    stlErrorStack   sErrorStack;
    stlErrorData  * sErrorData = NULL;
    stlRegionMark   sMark;
    stlInt32        sState = 0;

    stlInt64      sBufferLength;
    SQLCHAR     * sInConnectionString = NULL;
    stlInt64      sStringLength1      = 0;
    SQLCHAR       sOutConnectionString[1024];
    SQLSMALLINT   sStringLength2      = 0;
    stlInt64      sLength = 0;

    STL_INIT_ERROR_STACK( &sErrorStack );
    
    sDbc = (zlcDbc*)aConnectionHandle;
    
    STL_TRY_THROW( sDbc != NULL, RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sDbc->mType == SQL_HANDLE_DBC,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( stlMarkRegionMem( &sDbc->mAllocator,
                               &sMark,
                               &sErrorStack ) == STL_SUCCESS );
    sState = 1;

    sBufferLength = 0;    
    STL_TRY( zlvAllocCharBuffer( aInConnectionString,
                                 aStringLength1,
                                 sDbc->mCharacterSet,
                                 &sInConnectionString,
                                 &sBufferLength,
                                 &sDbc->mAllocator,
                                 &sErrorStack ) == STL_SUCCESS );

    STL_TRY( zlvWcharToChar( aInConnectionString,
                             aStringLength1,
                             sDbc->mCharacterSet,
                             sInConnectionString,
                             sBufferLength,
                             &sStringLength1,
                             &sErrorStack ) == STL_SUCCESS );

    sRet = zllDriverConnect( aConnectionHandle,
                             aWindowHandle,
                             sInConnectionString,
                             sStringLength1,
                             sOutConnectionString,
                             STL_SIZEOF(sOutConnectionString),
                             &sStringLength2,
                             aDriverCompletion,
                             &sErrorStack );

    STL_TRY( SQL_SUCCEEDED(sRet) );
    
    STL_TRY( zlvCharToWchar( sDbc->mCharacterSet,
                             sOutConnectionString,
                             sStringLength2,
                             aOutConnectionString,
                             aBufferLength,
                             &sLength,
                             &sErrorStack ) == STL_SUCCESS );

    if( aStringLength2Ptr != NULL )
    {
        *aStringLength2Ptr = sLength;
    }

    if ( aOutConnectionString != NULL )
    {
        STL_TRY_THROW( sLength < aBufferLength, RAMP_WARNING_STRING_DATA_RIGHT_TRUNCATED );
    }

    STL_TRY( sRet == SQL_SUCCESS );

    sState = 0;
    STL_TRY( stlRestoreRegionMem( &sDbc->mAllocator,
                                  &sMark,
                                  STL_TRUE,
                                  &sErrorStack ) == STL_SUCCESS );

    zldDiagSetRetCode( SQL_HANDLE_DBC, aConnectionHandle, sRet );

    return SQL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_CATCH( RAMP_WARNING_STRING_DATA_RIGHT_TRUNCATED )
    {
        sRet = SQL_SUCCESS_WITH_INFO;

        stlPushError( STL_ERROR_LEVEL_WARNING,
                      ZLE_ERRCODE_WARNING_STRING_DATA_RIGHT_TRUNCATED,
                      "The buffer *OutConnectionString was not large enough to return the "
                      "entire connection string, so the connection string was truncated. "
                      "The length of the untruncated connection string is returned "
                      "in *StringLength2Ptr.",
                      &sErrorStack );
    }

    STL_FINISH;

    if( ( sRet == SQL_ERROR ) ||
        ( sRet == SQL_SUCCESS_WITH_INFO ) )
    {
        zldDiagSetRetCode( SQL_HANDLE_DBC, aConnectionHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_DBC,
                               aConnectionHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }
    }

    switch( sState )
    {
        case 1 :
            (void)stlRestoreRegionMem( &sDbc->mAllocator, &sMark, STL_TRUE, &sErrorStack );
        default :
            break;
    }

    return sRet;
}

SQLRETURN SQL_API SQLEndTran( SQLSMALLINT aHandleType,
                              SQLHANDLE   aHandle,
                              SQLSMALLINT aCompletionType )
{
    SQLRETURN       sRet        = SQL_ERROR;
    zlnEnv        * sEnv        = NULL;
    zlcDbc        * sDbc        = NULL;
    stlErrorData  * sErrorData  = NULL;
    stlErrorStack   sErrorStack;

    STL_INIT_ERROR_STACK( &sErrorStack );

    switch( aHandleType )
    {
        case SQL_HANDLE_DBC :
            sDbc = (zlcDbc*)aHandle;

            STL_TRY_THROW( sDbc != NULL, RAMP_ERR_INVALID_HANDLE );

            STL_TRY_THROW( sDbc->mType == SQL_HANDLE_DBC,
                           RAMP_ERR_INVALID_HANDLE );

            switch( sDbc->mTransition )
            {
                case ZLC_TRANSITION_STATE_C0 :
                case ZLC_TRANSITION_STATE_C1 :
                    STL_THROW( RAMP_ERR_INVALID_HANDLE );
                    break;
                case ZLC_TRANSITION_STATE_C2 :
                case ZLC_TRANSITION_STATE_C3 :
                    STL_TRY( zldDiagClear( SQL_HANDLE_DBC,
                                           aHandle,
                                           &sErrorStack ) == STL_SUCCESS );

                    STL_THROW( RAMP_ERR_CONNECTION_NOT_OPEN );
                    break;
                case ZLC_TRANSITION_STATE_C4 :
                case ZLC_TRANSITION_STATE_C5 :
                case ZLC_TRANSITION_STATE_C6 :
                    STL_TRY( zldDiagClear( SQL_HANDLE_DBC,
                                           aHandle,
                                           &sErrorStack ) == STL_SUCCESS );

                    STL_TRY( zlcCheckSession( sDbc,
                                              &sErrorStack )
                             == STL_SUCCESS );
                    break;
                default:
                    STL_ASSERT( STL_FALSE );
                    break;
            }
            break;
        case SQL_HANDLE_ENV :
            sEnv = (zlnEnv*)aHandle;

            STL_TRY_THROW( sEnv != NULL, RAMP_ERR_INVALID_HANDLE );

            STL_TRY_THROW( sEnv->mType == SQL_HANDLE_ENV,
                           RAMP_ERR_INVALID_HANDLE );

            STL_TRY( zldDiagClear( SQL_HANDLE_ENV,
                                   aHandle,
                                   &sErrorStack ) == STL_SUCCESS );

            STL_THROW( RAMP_ERR_INVALID_ATTRIBUTE_OPTION_IDENTIFIER1 );
            break;
        default :
            STL_THROW( RAMP_ERR_INVALID_ATTRIBUTE_OPTION_IDENTIFIER2 );
            break;
    }

    switch( aCompletionType )
    {
        case SQL_COMMIT :
        case SQL_ROLLBACK :
            break;
        default :
            STL_THROW( RAMP_ERR_INVALID_TRANSACTION_OPERATION_CODE );
            break;
    }

    STL_TRY( zltEndTran( sDbc,
                         aCompletionType,
                         &sErrorStack ) == STL_SUCCESS );

    switch( sDbc->mTransition )
    {
        case ZLC_TRANSITION_STATE_C4 :
        case ZLC_TRANSITION_STATE_C5 :
            break;
        case ZLC_TRANSITION_STATE_C6 :
            sDbc->mTransition = ZLC_TRANSITION_STATE_C5;
            break;
    }

    sRet = SQL_SUCCESS;

    switch( aHandleType )
    {
        case SQL_HANDLE_DBC :
            zldDiagSetRetCode( SQL_HANDLE_DBC, aHandle, sRet );
            break;
        default :
            STL_ASSERT( STL_FALSE );
            break;
    }

    return sRet;

    STL_CATCH( RAMP_ERR_INVALID_ATTRIBUTE_OPTION_IDENTIFIER1 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_ATTRIBUTE_OPTION_IDENTIFIER,
                      "The value specified for the argument HandleType was not SQL_HANDLE_DBC.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_ATTRIBUTE_OPTION_IDENTIFIER2 )
    {
        /**
         * @todo 에러를 어디에 어떻게 설정할 것인가?
         */
    }

    STL_CATCH( RAMP_ERR_CONNECTION_NOT_OPEN )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_CONNECTION_NOT_OPEN,
                      "The HandleType was SQL_HANDLE_DBC, "
                      "and the Handle was not in a connected state.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_TRANSACTION_OPERATION_CODE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_TRANSACTION_OPERATION_CODE,
                      "The value specified for the argument aCompletionType was "
                      "neither SQL_COMMIT nor SQL_ROLLBACK.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_FINISH;

    if( sRet == SQL_ERROR )
    {
        switch( aHandleType )
        {
            case SQL_HANDLE_DBC :
                zldDiagSetRetCode( SQL_HANDLE_DBC, aHandle, sRet );

                sErrorData = stlGetLastErrorData( &sErrorStack );
                
                if( sErrorData != NULL )
                {
                    (void)zldDiagAdds( SQL_HANDLE_DBC,
                                       aHandle,
                                       SQL_NO_ROW_NUMBER,
                                       SQL_NO_COLUMN_NUMBER,
                                       &sErrorStack );
                }
        
                zlcCheckSessionAndCleanup( sDbc );
                break;
            case SQL_HANDLE_ENV :
                zldDiagSetRetCode( SQL_HANDLE_ENV, aHandle, sRet );

                sErrorData = stlGetLastErrorData( &sErrorStack );
                
                if( sErrorData != NULL )
                {
                    (void)zldDiagAdds( SQL_HANDLE_ENV,
                                       aHandle,
                                       SQL_NO_ROW_NUMBER,
                                       SQL_NO_COLUMN_NUMBER,
                                       &sErrorStack );
                }
                break;
            default :
                STL_ASSERT( STL_FALSE );
                break;
        }
    }

    return sRet;
}

SQLRETURN SQL_API SQLError( SQLHENV       aEnvironmentHandle,
                            SQLHDBC       aConnectionHandle,
                            SQLHSTMT      aStatementHandle,
                            SQLCHAR     * aSQLState,
                            SQLINTEGER  * aNativeError,
                            SQLCHAR     * aMessageText,
                            SQLSMALLINT   aBufferLength,
                            SQLSMALLINT * aTextLength )
{
    SQLRETURN   sRet = SQL_ERROR;
    zlnEnv    * sEnv;
    zlcDbc    * sDbc;
    zlsStmt   * sStmt;
    zldDiag   * sDiag = NULL;
    stlInt16    sTextLen = 0;
    stlInt32    sNativeError = 0;
    stlInt32    sODBCVer = 0;

    STL_TRY( aBufferLength >= 0 );
    
    if( aStatementHandle != SQL_NULL_HANDLE )
    {
        sStmt = (zlsStmt*)aStatementHandle;

        STL_TRY_THROW( sStmt->mType == SQL_HANDLE_STMT,
                       RAMP_ERR_INVALID_HANDLE );
        
        sDiag = &sStmt->mDiag;
        sODBCVer = sStmt->mParentDbc->mParentEnv->mAttrVersion;
    }
    else if( aConnectionHandle != SQL_NULL_HANDLE )
    {
        sDbc = (zlcDbc*)aConnectionHandle;

        STL_TRY_THROW( sDbc->mType == SQL_HANDLE_DBC,
                       RAMP_ERR_INVALID_HANDLE );

        sDiag = &sDbc->mDiag;
        sODBCVer = sDbc->mParentEnv->mAttrVersion;
    }
    else if( aEnvironmentHandle != SQL_NULL_HANDLE )
    {
        sEnv = (zlnEnv*)aEnvironmentHandle;

        STL_TRY_THROW( sEnv->mType == SQL_HANDLE_ENV,
                       RAMP_ERR_INVALID_HANDLE );

        sDiag = &sEnv->mDiag;
        sODBCVer = sEnv->mAttrVersion;
    }
    else
    {
        STL_THROW( RAMP_ERR_INVALID_HANDLE );
    }

    STL_DASSERT( sDiag != NULL );

    sDiag->mCurrentRec++;

    STL_TRY_THROW( sDiag->mCurrentRec <= sDiag->mNumber, RAMP_NO_DATA );

    STL_TRY( zldDiagGetRec( sDiag,
                            sDiag->mCurrentRec,
                            (stlChar*)aSQLState,
                            &sNativeError,
                            (stlChar*)aMessageText,
                            aBufferLength,
                            &sTextLen ) == STL_SUCCESS );

    if( sODBCVer == SQL_OV_ODBC2 )
    {
        if( aSQLState != NULL )
        {
            if( (aSQLState[0] == 'H') && (aSQLState[1] == 'Y') )
            {
                aSQLState[0] = 'S';
                aSQLState[1] = '1';
            }
        }
    }

    if( aNativeError != NULL )
    {
        *aNativeError = sNativeError;
    }

    if( aTextLength != NULL )
    {
        *aTextLength = sTextLen;
    }

    if( aBufferLength > sTextLen )
    {
        sRet = SQL_SUCCESS;
    }
    else
    {
        sRet = SQL_SUCCESS_WITH_INFO;
    }

    return sRet;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_CATCH( RAMP_NO_DATA )
    {
        sRet = SQL_NO_DATA;
    }

    STL_FINISH;

    return sRet;    
}

SQLRETURN SQL_API SQLErrorW( SQLHENV       aEnvironmentHandle,
                             SQLHDBC       aConnectionHandle,
                             SQLHSTMT      aStatementHandle,
                             SQLWCHAR    * aSQLState,
                             SQLINTEGER  * aNativeError,
                             SQLWCHAR    * aMessageText,
                             SQLSMALLINT   aBufferLength,
                             SQLSMALLINT * aTextLength )
{
    SQLRETURN         sRet = SQL_ERROR;
    zlnEnv          * sEnv;
    zlcDbc          * sDbc;
    zlsStmt         * sStmt;
    zldDiag         * sDiag = NULL;
    stlInt16          sTextLen = 0;
    stlInt32          sNativeError = 0;
    stlInt32          sODBCVer = 0;
    dtlCharacterSet   sCharacterSet = DTL_SQL_ASCII;
    stlErrorStack     sErrorStack;

    STL_INIT_ERROR_STACK( &sErrorStack );

    STL_TRY( aBufferLength >= 0 );

    if( aStatementHandle != SQL_NULL_HANDLE )
    {
        sStmt = (zlsStmt*)aStatementHandle;

        STL_TRY_THROW( sStmt->mType == SQL_HANDLE_STMT,
                       RAMP_ERR_INVALID_HANDLE );
        
        sDiag = &sStmt->mDiag;
        sCharacterSet = ZLS_STMT_DBC(sStmt)->mCharacterSet;
        sODBCVer = sStmt->mParentDbc->mParentEnv->mAttrVersion;
    }
    else if( aConnectionHandle != SQL_NULL_HANDLE )
    {
        sDbc = (zlcDbc*)aConnectionHandle;

        STL_TRY_THROW( sDbc->mType == SQL_HANDLE_DBC,
                       RAMP_ERR_INVALID_HANDLE );

        sDiag = &sDbc->mDiag;
        sCharacterSet = sDbc->mCharacterSet;
        sODBCVer = sDbc->mParentEnv->mAttrVersion;
    }
    else if( aEnvironmentHandle != SQL_NULL_HANDLE )
    {
        sEnv = (zlnEnv*)aEnvironmentHandle;

        STL_TRY_THROW( sEnv->mType == SQL_HANDLE_ENV,
                       RAMP_ERR_INVALID_HANDLE );

        sDiag = &sEnv->mDiag;
        sODBCVer = sEnv->mAttrVersion;
    }
    else
    {
        STL_THROW( RAMP_ERR_INVALID_HANDLE );
    }

    STL_DASSERT( sDiag != NULL );

    sDiag->mCurrentRec++;

    STL_TRY_THROW( sDiag->mCurrentRec <= sDiag->mNumber, RAMP_NO_DATA );

    STL_TRY( zldDiagGetRecW( sDiag,
                             sDiag->mCurrentRec,
                             sCharacterSet,
                             aSQLState,
                             &sNativeError,
                             aMessageText,
                             aBufferLength,
                             &sTextLen,
                             &sErrorStack ) == STL_SUCCESS );

    if( sODBCVer == SQL_OV_ODBC2 )
    {
        if( aSQLState != NULL )
        {
            if( (aSQLState[0] == 'H') && (aSQLState[1] == 'Y') )
            {
                aSQLState[0] = 'S';
                aSQLState[1] = '1';
            }
        }
    }

    if( aNativeError != NULL )
    {
        *aNativeError = sNativeError;
    }

    if( aTextLength != NULL )
    {
        *aTextLength = sTextLen;
    }

    if( aBufferLength > sTextLen )
    {
        sRet = SQL_SUCCESS;
    }
    else
    {
        sRet = SQL_SUCCESS_WITH_INFO;
    }

    return sRet;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_CATCH( RAMP_NO_DATA )
    {
        sRet = SQL_NO_DATA;
    }

    STL_FINISH;

    return sRet;    
}

static SQLRETURN zllExecDirect( SQLHSTMT        aStatementHandle,
                                SQLCHAR       * aStatementText,
                                SQLINTEGER      aTextLength,
                                stlErrorStack * aErrorStack )
{
    SQLRETURN       sRet              = SQL_ERROR;
    zlnEnv        * sEnv              = NULL;
    zlcDbc        * sDbc              = NULL;
    zlsStmt       * sStmt             = NULL;
    stlChar       * sSQL              = NULL;
    stlInt64        sAffectedRowCount = -1;
    stlBool         sHasDataAtExec    = STL_FALSE;
    stlBool         sSuccessWithInfo  = STL_FALSE;

    sStmt = (zlsStmt*)aStatementHandle;
    sDbc  = sStmt->mParentDbc;
    sEnv  = sDbc->mParentEnv;

    STL_TRY( zldDiagClear( SQL_HANDLE_STMT,
                           aStatementHandle,
                           aErrorStack ) == STL_SUCCESS );

    STL_TRY( zlcCheckSession( sDbc,
                              aErrorStack ) == STL_SUCCESS );
    
    switch( sDbc->mTransition )
    {
        case ZLC_TRANSITION_STATE_C0 :
        case ZLC_TRANSITION_STATE_C1 :
        case ZLC_TRANSITION_STATE_C2 :
        case ZLC_TRANSITION_STATE_C3 :
        case ZLC_TRANSITION_STATE_C4 :
            STL_THROW( RAMP_ERR_INVALID_HANDLE );
            break;
        case ZLC_TRANSITION_STATE_C5 :
        case ZLC_TRANSITION_STATE_C6 :
            break;
    }

    switch( sStmt->mTransition )
    {
        case ZLS_TRANSITION_STATE_S1 :
        case ZLS_TRANSITION_STATE_S2 :
        case ZLS_TRANSITION_STATE_S3 :
        case ZLS_TRANSITION_STATE_S4 :
            break;
        case ZLS_TRANSITION_STATE_S5 :
        case ZLS_TRANSITION_STATE_S6 :
        case ZLS_TRANSITION_STATE_S7 :
            STL_THROW( RAMP_ERR_INVALID_CURSOR_STATE );
            break;
        case ZLS_TRANSITION_STATE_S8 :
        case ZLS_TRANSITION_STATE_S9 :
        case ZLS_TRANSITION_STATE_S10 :
        case ZLS_TRANSITION_STATE_S11 :
        case ZLS_TRANSITION_STATE_S12 :
            STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR );
            break;
    }

    STL_TRY_THROW( aStatementText != NULL, RAMP_ERR_INVALID_USE_OF_NULL_POINTER );

    STL_TRY( sDbc->mConvertSQLFunc( sStmt,
                                    (stlChar*)aStatementText,
                                    aTextLength,
                                    &sSQL,
                                    aErrorStack ) == STL_SUCCESS );

    /*
     * DATA_AT_EXEC, SQL_LEN_DATA_AT_EXEC 검사
     */

    sStmt->mParamCount = ZLS_PARAM_COUNT_UNKNOWN;

    ZLS_INIT_DATA_AT_EXEC( sStmt );
    
    STL_TRY( zliHasDataAtExecIndicator( sStmt,
                                        sStmt->mApd,
                                        &sStmt->mIpd,
                                        &sHasDataAtExec,
                                        aErrorStack ) == STL_SUCCESS );

    if( sHasDataAtExec == STL_TRUE )
    {
        STL_TRY( zlpPrepare( sStmt,
                             sSQL,
                             &sSuccessWithInfo,
                             aErrorStack ) == STL_SUCCESS );

        STL_DASSERT( sStmt->mParamCount != ZLS_PARAM_COUNT_UNKNOWN );

        if( sStmt->mCurrentPutDataIdx <= sStmt->mParamCount )
        {
            STL_TRY( zliReadyDataAtExec( sStmt,
                                         sStmt->mApd,
                                         &sStmt->mIpd,
                                         &sHasDataAtExec,
                                         aErrorStack ) == STL_SUCCESS );

            STL_DASSERT( sHasDataAtExec == STL_TRUE );

            STL_THROW( RAMP_NEED_DATA );
        }
    }

    STL_TRY( zlxExecDirect( sStmt,
                            sSQL,
                            &sAffectedRowCount,
                            &sSuccessWithInfo,
                            aErrorStack ) == STL_SUCCESS );

    if( sEnv->mAttrVersion == SQL_OV_ODBC3 )
    {
        STL_TRY_THROW( sAffectedRowCount != 0, RAMP_NO_DATA );
    }

    STL_TRY_THROW( sSuccessWithInfo == STL_FALSE, RAMP_SUCCESS_WITH_INFO );

    sRet = SQL_SUCCESS;
    
    return sRet;

    STL_CATCH( RAMP_ERR_INVALID_CURSOR_STATE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CURSOR_STATE,
                      "A cursor was open on the StatementHandle.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_USE_OF_NULL_POINTER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_USE_OF_NULL_POINTER,
                      "StatementText was a null pointer.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_CATCH( RAMP_NEED_DATA )
    {
        sStmt->mNeedDataTransition = sStmt->mTransition;
        sStmt->mTransition         = ZLS_TRANSITION_STATE_S8;

        sRet = SQL_NEED_DATA;
    }

    STL_CATCH( RAMP_NO_DATA )
    {
        sRet = SQL_NO_DATA;
    }

    STL_CATCH( RAMP_SUCCESS_WITH_INFO )
    {
        sRet = SQL_SUCCESS_WITH_INFO;
    }

    STL_FINISH;

    return sRet;
}

SQLRETURN SQL_API SQLExecDirect( SQLHSTMT     aStatementHandle,
                                 SQLCHAR    * aStatementText,
                                 SQLINTEGER   aTextLength )
{
    SQLRETURN       sRet  = SQL_ERROR;
    zlsStmt       * sStmt = NULL;
    stlErrorStack   sErrorStack;
    stlErrorData  * sErrorData = NULL;
    stlRegionMark   sMark;
    stlInt32        sState = 0;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sStmt = (zlsStmt*)aStatementHandle;

    STL_TRY_THROW( sStmt != NULL,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sStmt->mType == SQL_HANDLE_STMT,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( stlMarkRegionMem( &sStmt->mAllocator,
                               &sMark,
                               &sErrorStack ) == STL_SUCCESS );
    sState = 1;

    sRet = zllExecDirect( aStatementHandle,
                          aStatementText,
                          aTextLength,
                          &sErrorStack );
    
    STL_TRY( sRet == SQL_SUCCESS );

    sState = 0;
    STL_TRY( stlRestoreRegionMem( &sStmt->mAllocator,
                                  &sMark,
                                  STL_TRUE,
                                  &sErrorStack ) == STL_SUCCESS );

    zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

    return SQL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_FINISH;

    if( ( sRet == SQL_ERROR ) ||
        ( sRet == SQL_SUCCESS_WITH_INFO ) )
    {
        zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_STMT,
                               aStatementHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }

        if( sStmt != NULL )
        {
            zlcCheckSessionAndCleanup( ZLS_STMT_DBC(sStmt) );
        }
    }

    switch( sState )
    {
        case 1 :
            (void)stlRestoreRegionMem( &sStmt->mAllocator, &sMark, STL_TRUE, &sErrorStack );
        default :
            break;
    }

    return sRet;
}

SQLRETURN SQL_API SQLExecDirectW( SQLHSTMT     aStatementHandle,
                                  SQLWCHAR   * aStatementText,
                                  SQLINTEGER   aTextLength )
{
    /**
     * aTextLength
     *     Length of *StatementText, in characters.
     */

    SQLRETURN       sRet  = SQL_ERROR;
    zlsStmt       * sStmt = NULL;
    stlErrorStack   sErrorStack;
    stlErrorData  * sErrorData = NULL;
    stlRegionMark   sMark;
    stlInt32        sState = 0;

    stlInt64        sBufferLength;
    SQLCHAR       * sStatementText = NULL;
    stlInt64        sTextLength = 0;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sStmt = (zlsStmt*)aStatementHandle;

    STL_TRY_THROW( sStmt != NULL,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sStmt->mType == SQL_HANDLE_STMT,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( stlMarkRegionMem( &sStmt->mAllocator,
                               &sMark,
                               &sErrorStack ) == STL_SUCCESS );
    sState = 1;

    sBufferLength = 0;    
    STL_TRY( zlvAllocCharBuffer( aStatementText,
                                 aTextLength,
                                 ZLS_STMT_DBC(sStmt)->mCharacterSet,
                                 &sStatementText,
                                 &sBufferLength,
                                 &sStmt->mAllocator,
                                 &sErrorStack ) == STL_SUCCESS );
    
    STL_TRY( zlvWcharToChar( aStatementText,
                             aTextLength,
                             ZLS_STMT_DBC(sStmt)->mCharacterSet,
                             sStatementText,
                             sBufferLength,
                             &sTextLength,
                             &sErrorStack ) == STL_SUCCESS );
    
    sRet = zllExecDirect( aStatementHandle,
                          sStatementText,
                          sTextLength,
                          &sErrorStack );
    
    STL_TRY( sRet == SQL_SUCCESS );

    sState = 0;
    STL_TRY( stlRestoreRegionMem( &sStmt->mAllocator,
                                  &sMark,
                                  STL_TRUE,
                                  &sErrorStack ) == STL_SUCCESS );

    zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

    return SQL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_FINISH;

    if( ( sRet == SQL_ERROR ) ||
        ( sRet == SQL_SUCCESS_WITH_INFO ) )
    {
        zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_STMT,
                               aStatementHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }

        if( sStmt != NULL )
        {
            zlcCheckSessionAndCleanup( ZLS_STMT_DBC(sStmt) );
        }
    }

    switch( sState )
    {
        case 1 :
            (void)stlRestoreRegionMem( &sStmt->mAllocator, &sMark, STL_TRUE, &sErrorStack );
        default :
            break;
    }

    return sRet;
}

SQLRETURN SQL_API SQLExecute( SQLHSTMT aStatementHandle )
{
    SQLRETURN       sRet              = SQL_ERROR;
    zlnEnv        * sEnv              = NULL;
    zlcDbc        * sDbc              = NULL;
    zlsStmt       * sStmt             = NULL;
    stlInt64        sAffectedRowCount = -1;
    stlErrorData  * sErrorData        = NULL;
    stlErrorStack   sErrorStack;
    stlBool         sSuccessWithInfo  = STL_FALSE;
    stlBool         sHasDataAtExec    = STL_FALSE;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sStmt = (zlsStmt*)aStatementHandle;

    STL_TRY_THROW( sStmt != NULL, RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sStmt->mType == SQL_HANDLE_STMT,
                   RAMP_ERR_INVALID_HANDLE );

    sDbc = sStmt->mParentDbc;
    sEnv = sDbc->mParentEnv;

    STL_TRY( zldDiagClear( SQL_HANDLE_STMT,
                           aStatementHandle,
                           &sErrorStack ) == STL_SUCCESS );

    STL_TRY( zlcCheckSession( sDbc,
                              &sErrorStack ) == STL_SUCCESS );
    
    switch( sDbc->mTransition )
    {
        case ZLC_TRANSITION_STATE_C0 :
        case ZLC_TRANSITION_STATE_C1 :
        case ZLC_TRANSITION_STATE_C2 :
        case ZLC_TRANSITION_STATE_C3 :
        case ZLC_TRANSITION_STATE_C4 :
            STL_THROW( RAMP_ERR_INVALID_HANDLE );
            break;
        case ZLC_TRANSITION_STATE_C5 :
        case ZLC_TRANSITION_STATE_C6 :
            break;
    }

    switch( sStmt->mTransition )
    {
        case ZLS_TRANSITION_STATE_S2 :
        case ZLS_TRANSITION_STATE_S3 :
        case ZLS_TRANSITION_STATE_S4 :
            break;
        case ZLS_TRANSITION_STATE_S5 :
        case ZLS_TRANSITION_STATE_S6 :
        case ZLS_TRANSITION_STATE_S7 :
            STL_THROW( RAMP_ERR_INVALID_CURSOR_STATE );
            break;
        case ZLS_TRANSITION_STATE_S1 :
        case ZLS_TRANSITION_STATE_S8 :
        case ZLS_TRANSITION_STATE_S9 :
        case ZLS_TRANSITION_STATE_S10 :
        case ZLS_TRANSITION_STATE_S11 :
        case ZLS_TRANSITION_STATE_S12 :
            STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR );
            break;
    }

    /*
     * DATA_AT_EXEC, SQL_LEN_DATA_AT_EXEC 검사
     */

    ZLS_INIT_DATA_AT_EXEC( sStmt );
    
    STL_TRY( zliReadyDataAtExec( sStmt,
                                 sStmt->mApd,
                                 &sStmt->mIpd,
                                 &sHasDataAtExec,
                                 &sErrorStack ) == STL_SUCCESS );

    STL_TRY_THROW( sHasDataAtExec == STL_FALSE, RAMP_NEED_DATA );

    STL_TRY( zlxExecute( sStmt,
                         &sAffectedRowCount,
                         &sSuccessWithInfo,
                         &sErrorStack ) == STL_SUCCESS );

    if( sEnv->mAttrVersion == SQL_OV_ODBC3 )
    {
        STL_TRY_THROW( sAffectedRowCount != 0, RAMP_NO_DATA );
    }
    
    STL_TRY_THROW( sSuccessWithInfo == STL_FALSE, RAMP_SUCCESS_WITH_INFO );

    sRet = SQL_SUCCESS;
    
    zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

    return sRet;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      "The StatementHandle was not prepared.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_CURSOR_STATE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CURSOR_STATE,
                      "A cursor was open on the StatementHandle.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_NEED_DATA )
    {
        sStmt->mNeedDataTransition = sStmt->mTransition;
        sStmt->mTransition         = ZLS_TRANSITION_STATE_S8;

        sRet = SQL_NEED_DATA;
    }

    STL_CATCH( RAMP_NO_DATA )
    {
        sRet = SQL_NO_DATA;
    }

    STL_CATCH( RAMP_SUCCESS_WITH_INFO )
    {
        sRet = SQL_SUCCESS_WITH_INFO;
    }

    STL_FINISH;

    if( ( sRet == SQL_ERROR ) ||
        ( sRet == SQL_SUCCESS_WITH_INFO ) )
    {
        zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_STMT,
                               aStatementHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }
        
        if( sStmt != NULL )
        {
            zlcCheckSessionAndCleanup( ZLS_STMT_DBC(sStmt) );
        }
    }

    return sRet;
}

SQLRETURN SQL_API SQLExtendedFetch( SQLHSTMT       aStatementHandle,
                                    SQLUSMALLINT   aFetchOrientation,
                                    SQLLEN         aFetchOffset,
                                    SQLULEN      * aRowCountPtr,
                                    SQLUSMALLINT * aRowStatusArray )
{
    SQLRETURN       sRet               = SQL_ERROR;
    zlsStmt       * sStmt              = NULL;
    zldDesc       * sArd               = NULL;
    zldDesc       * sIrd               = NULL;
    stlUInt64       sArraySize         = 0;
    stlInt64        sLastResultRow     = STL_INT64_MAX;
    stlInt64        sRelativeOffset    = 0;
    stlInt64        sAbsOffset         = 0;
    stlBool         sNotFound          = STL_FALSE;
    stlBool         sSuccessWithInfo   = STL_FALSE;
    stlErrorData  * sErrorData         = NULL;
    stlErrorStack   sErrorStack;
    SQLULEN       * sOrgRowCountPtr    = NULL;
    stlBool         sSetRowCountPtr    = STL_FALSE;
    SQLUSMALLINT  * sOrgRowStatusArray = NULL;
    stlBool         sSetRowStatusArray = STL_FALSE;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sStmt = (zlsStmt*)aStatementHandle;

    STL_TRY_THROW( sStmt != NULL,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sStmt->mType == SQL_HANDLE_STMT,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( zldDiagClear( SQL_HANDLE_STMT,
                           aStatementHandle,
                           &sErrorStack ) == STL_SUCCESS );

    STL_TRY( zlcCheckSession( sStmt->mParentDbc,
                              &sErrorStack ) == STL_SUCCESS );
    
    switch( sStmt->mTransition )
    {
        case ZLS_TRANSITION_STATE_S0 :
            STL_THROW( RAMP_ERR_INVALID_HANDLE );
            break;
        case ZLS_TRANSITION_STATE_S1 :
        case ZLS_TRANSITION_STATE_S2 :
        case ZLS_TRANSITION_STATE_S3 :
            STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR1 );
            break;
        case ZLS_TRANSITION_STATE_S4 :
            STL_THROW( RAMP_ERR_INVALID_CURSOR_STATE1 );
            break;
        case ZLS_TRANSITION_STATE_S6 :
            STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR2 );
            break;
        case ZLS_TRANSITION_STATE_S8 :
        case ZLS_TRANSITION_STATE_S9 :
        case ZLS_TRANSITION_STATE_S10 :
        case ZLS_TRANSITION_STATE_S11 :
        case ZLS_TRANSITION_STATE_S12 :
            STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR3 );
            break;
        default :
            break;
    }

    STL_TRY_THROW( sStmt->mResult.mStatus != ZLL_CURSOR_ROW_STATUS_NA,
                   RAMP_ERR_INVALID_CURSOR_STATE2 );

    if( sStmt->mCursorType == SQL_CURSOR_FORWARD_ONLY )
    {
        STL_TRY_THROW( aFetchOrientation == SQL_FETCH_NEXT,
                       RAMP_ERR_FETCH_TYPE_OUT_OF_RANGE1 );
    }

    if( sStmt->mCursorScrollable == SQL_NONSCROLLABLE )
    {
        STL_TRY_THROW( aFetchOrientation == SQL_FETCH_NEXT,
                       RAMP_ERR_FETCH_TYPE_OUT_OF_RANGE2 );
    }

    sArd       = sStmt->mArd;
    sIrd       = &sStmt->mIrd;
    sArraySize = sStmt->mRowsetSize;

    if( aRowCountPtr != NULL )
    {
        sOrgRowCountPtr = sIrd->mRowProcessed;
        sSetRowCountPtr = STL_TRUE;

        sIrd->mRowProcessed = aRowCountPtr;
    }

    if( aRowStatusArray != NULL )
    {
        sOrgRowStatusArray = sIrd->mArrayStatusPtr;
        sSetRowStatusArray = STL_TRUE;

        sIrd->mArrayStatusPtr = aRowStatusArray;
    }
    
    if( sStmt->mResult.mLastRowIdx != -1 )
    {
        STL_TRY_THROW( sStmt->mResult.mLastRowIdx != 0, RAMP_NO_DATA );

        if( sStmt->mMaxRows == 0 )
        {
            sLastResultRow = sStmt->mResult.mLastRowIdx;
        }
        else
        {
            sLastResultRow = STL_MIN( sStmt->mResult.mLastRowIdx, sStmt->mMaxRows );
        }
    }

    /* Positioning the Cursor */
    switch( aFetchOrientation )
    {
        case SQL_FETCH_NEXT :
            /*================================================================================
              | Condition                                     | First row of new rowset      |
              ================================================================================
              | Before start                                  | 1                            |
              | CurrRowsetStart + RowsetSize <= LastResultRow | CurrRowsetStart + RowsetSize |
              | CurrRowsetStart + RowsetSize >  LastResultRow | After end                    |
              | After end                                     | After end                    |
              ================================================================================ */
            STL_TRY_THROW( sStmt->mResult.mBlockCurrentRow != ZLR_ROWSET_AFTER_END,
                           RAMP_NO_DATA_AFTER_END );
    
            if( sStmt->mResult.mBlockCurrentRow == ZLR_ROWSET_BEFORE_START )
            {
                sStmt->mResult.mCurrentRowIdx = 1;
            }
            else
            {
                STL_TRY_THROW( sStmt->mResult.mCurrentRowIdx <= sLastResultRow - sArraySize,
                               RAMP_NO_DATA_AFTER_END );

                sStmt->mResult.mCurrentRowIdx += sStmt->mUsedRowsetSize;
            }
    
            break;
        case SQL_FETCH_PRIOR :
            /*==================================================================================
              | Condition                                     | First row of new rowset        |
              ==================================================================================
              | Before start                                  | Before start                   |
              | CurrRowsetStart = 1                           | Before start                   |
              | 1 < CurrRowsetStart <= RowsetSize             | 1                              |
              | CurrRowsetStart > RowsetSize                  | CurrRowsetStart - RowsetSize   |
              | After end AND LastResultRow < RowsetSize      | 1                              |
              | After end AND LastResultRow >= RowsetSize     | LastResultRow - RowsetSize + 1 |
              ================================================================================== */
            STL_TRY_THROW( sStmt->mResult.mBlockCurrentRow != ZLR_ROWSET_BEFORE_START,
                           RAMP_NO_DATA_BEFORE_START );

            sStmt->mResult.mIsEndOfScan = STL_FALSE;
    
            if( sStmt->mResult.mBlockCurrentRow == ZLR_ROWSET_AFTER_END )
            {
                if( sLastResultRow < sArraySize )
                {
                    sStmt->mResult.mCurrentRowIdx = 1;
                }
                else
                {
                    sStmt->mResult.mCurrentRowIdx = sLastResultRow - sArraySize + 1;
                }
            }
            else
            {
                STL_TRY_THROW( sStmt->mResult.mCurrentRowIdx != 1, RAMP_NO_DATA_BEFORE_START );

                if( sStmt->mResult.mCurrentRowIdx <= sArraySize )
                {
                    sStmt->mResult.mCurrentRowIdx = 1;
                }
                else
                {
                    sStmt->mResult.mCurrentRowIdx -= sArraySize;
                }
            }
            break;
        case SQL_FETCH_FIRST :
            /*=======================================
              | Condition | First row of new rowset |
              =======================================
              | Any       | 1                       |
              ======================================= */
            sStmt->mResult.mIsEndOfScan = STL_FALSE;
            sStmt->mResult.mCurrentRowIdx = 1;
            break;
        case SQL_FETCH_LAST :
            /*==================================================================================
              | Condition                                     | First row of new rowset        |
              ==================================================================================
              | RowsetSize <= LastResultRow                   | LastResultRow - RowsetSize + 1 |
              | RowsetSize >  LastResultRow                   | 1                              |
              ================================================================================== */
            sStmt->mResult.mIsEndOfScan = STL_FALSE;
    
            /* 마지막 row의 번호를 알 수 없을 경우
             * SQL_FETCH_ABSOLUTE로 뒤에서부터 rowset 크기만큼 갖고 온다. */
            if( sLastResultRow == STL_INT64_MAX )
            {
                sStmt->mResult.mCurrentRowIdx = 0 - sArraySize;
            }
            else
            {
                if( sArraySize <= sLastResultRow )
                {
                    sStmt->mResult.mCurrentRowIdx = sLastResultRow - sArraySize + 1;
                }
                else
                {
                    sStmt->mResult.mCurrentRowIdx = 1;
                }
            }
            break;
        case SQL_FETCH_RELATIVE :
            /*===============================================================================================================================
              | Condition                                                                                   | First row of new rowset       |
              ===============================================================================================================================
              | (Before start AND FetchOffset > 0) OR (After end AND FetchOffset < 0)                       | --                            |
              | BeforeStart AND FetchOffset <= 0                                                            | Before start                  |
              | CurrRowsetStart = 1 AND FetchOffset < 0                                                     | Before start                  |
              | CurrRowsetStart > 1 AND CurrRowsetStart + FetchOffset < 1 AND | FetchOffset | >  RowsetSize | Before start                  |
              | CurrRowsetStart > 1 AND CurrRowsetStart + FetchOffset < 1 AND | FetchOffset | <= RowsetSize | 1                             |
              | 1 <= CurrRowsetStart + FetchOffset <= LastResultRow                                         | CurrRowsetStart + FetchOffset |
              | CurrRowsetStart + FetchOffset > LastResultRow                                               | After end                     |
              | After end AND FetchOffset >= 0                                                              | After end                     |
              =============================================================================================================================== */
            sStmt->mResult.mIsEndOfScan = STL_FALSE;
    
            if( sStmt->mResult.mBlockCurrentRow == ZLR_ROWSET_BEFORE_START )
            {
                STL_TRY_THROW( aFetchOffset > 0, RAMP_NO_DATA_BEFORE_START );

                STL_THROW( RAMP_FETCH_ABSOLUTE );
            }
            else if( sStmt->mResult.mBlockCurrentRow == ZLR_ROWSET_AFTER_END )
            {
                STL_TRY_THROW( aFetchOffset < 0, RAMP_NO_DATA_AFTER_END );

                STL_THROW( RAMP_FETCH_ABSOLUTE );
            }
            else
            {
                if( sStmt->mResult.mCurrentRowIdx == 1 )
                {
                    STL_TRY_THROW( aFetchOffset >= 0, RAMP_NO_DATA_BEFORE_START );
                }
                
                sRelativeOffset = sStmt->mResult.mCurrentRowIdx + aFetchOffset;

                STL_TRY_THROW( sRelativeOffset <= sLastResultRow, RAMP_NO_DATA_AFTER_END );

                if( sRelativeOffset < 1 )
                {
                    sAbsOffset = stlAbsInt64(aFetchOffset);

                    STL_TRY_THROW( sAbsOffset <= sArraySize, RAMP_NO_DATA_BEFORE_START );

                    sStmt->mResult.mCurrentRowIdx = 1;
                }
                else
                {
                    sStmt->mResult.mCurrentRowIdx = sRelativeOffset;
                }
            }
            break;
        case SQL_FETCH_ABSOLUTE :
            STL_RAMP( RAMP_FETCH_ABSOLUTE );
            /*===========================================================================================================================
              | Condition                                                                             | First row of new rowset         |
              ===========================================================================================================================
              | FetchOffset < 0 AND | FetchOffset | <= LastResultRow                                  | LastResultRow + FetchOffset + 1 |
              | FetchOffset < 0 AND | FetchOffset | > LastResultRow AND | FetchOffset | >  RowsetSize | Before start                    |
              | FetchOffset < 0 AND | FetchOffset | > LastResultRow AND | FetchOffset | <= RowsetSize | 1                               |
              | FetchOffset = 0                                                                       | Before start                    |
              | 1 <= FetchOffset <= LastResultRow                                                     | FetchOffset                     |
              | FetchOffset > LastResultRow                                                           | After end                       |
              =========================================================================================================================== */
            STL_TRY_THROW( aFetchOffset <= sLastResultRow, RAMP_NO_DATA_AFTER_END );
            STL_TRY_THROW( aFetchOffset != 0, RAMP_NO_DATA_BEFORE_START );

            sStmt->mResult.mIsEndOfScan = STL_FALSE;

            if( aFetchOffset < 0 )
            {
                /* 마지막 row의 번호를 알 수 없을 경우
                 * SQL_FETCH_ABSOLUTE로 뒤에서부터 rowset 크기만큼 갖고 온다. */
                if( sLastResultRow == STL_INT64_MAX )
                {
                    sStmt->mResult.mCurrentRowIdx = aFetchOffset;
                }
                else
                {
                    sAbsOffset = stlAbsInt64(aFetchOffset);

                    if( sAbsOffset <= sLastResultRow )
                    {
                        sStmt->mResult.mCurrentRowIdx = sLastResultRow + aFetchOffset + 1;
                    }
                    else
                    {
                        STL_TRY_THROW( sAbsOffset <= sArraySize, RAMP_NO_DATA_BEFORE_START );

                        sStmt->mResult.mCurrentRowIdx = 1;
                    }
                }
            }
            else
            {
                sStmt->mResult.mCurrentRowIdx = aFetchOffset;
            }
            break;
        case SQL_FETCH_BOOKMARK :
            STL_THROW( RAMP_ERR_DRIVER_DOES_NOT_SUPPORT_THIS_FUNCTION );
            break;
        default :
            STL_THROW( RAMP_ERR_FETCH_TYPE_OUT_OF_RANGE3 );
            break;
    }
    
    STL_TRY( zlrFetch( aStatementHandle,
                       sArraySize,
                       sStmt->mResult.mCurrentRowIdx,
                       &sNotFound,
                       &sSuccessWithInfo,
                       &sErrorStack ) == STL_SUCCESS );

    sStmt->mTransition = ZLS_TRANSITION_STATE_S7;

    STL_TRY_THROW( sNotFound == STL_FALSE, RAMP_NO_DATA );
    STL_TRY_THROW( sSuccessWithInfo == STL_FALSE, RAMP_SUCCESS_WITH_INFO );
    
    sStmt->mUsedRowsetSize = sArraySize;

    if( sSetRowCountPtr == STL_TRUE )
    {
        sIrd->mRowProcessed = sOrgRowCountPtr;
    }

    if( sSetRowStatusArray == STL_TRUE )
    {
        sIrd->mArrayStatusPtr = sOrgRowStatusArray;
    }
    
    sRet = SQL_SUCCESS;

    zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

    return sRet;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_CATCH( RAMP_NO_DATA )
    {
        sRet = SQL_NO_DATA;
    }

    STL_CATCH( RAMP_NO_DATA_BEFORE_START )
    {
        sRet = SQL_NO_DATA;

        sStmt->mResult.mBlockCurrentRow = ZLR_ROWSET_BEFORE_START;
    }

    STL_CATCH( RAMP_NO_DATA_AFTER_END )
    {
        sRet = SQL_NO_DATA;

        sStmt->mResult.mBlockCurrentRow = ZLR_ROWSET_AFTER_END;
    }

    STL_CATCH( RAMP_SUCCESS_WITH_INFO )
    {
        sRet = SQL_SUCCESS_WITH_INFO;
    }

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR1 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      "The specified StatementHandle was not in an executed state. "
                      "The function was called without first calling "
                      "SQLExecDirect, SQLExecute or a catalog function.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR2 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      "SQLFetch was called for the StatementHandle after "
                      "SQLExtendedFetch was called and before SQLFreeStmt with "
                      "the SQL_CLOSE option was called.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR3 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      NULL,
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_CURSOR_STATE1 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CURSOR_STATE,
                      "The StatementHandle was in an executed state but "
                      "no result set was associated with the StatementHandle.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_CURSOR_STATE2 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CURSOR_STATE,
                      NULL,
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_DRIVER_DOES_NOT_SUPPORT_THIS_FUNCTION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_DRIVER_DOES_NOT_SUPPORT_THIS_FUNCTION,
                      "The driver associated with the StatementHandle does not support "
                      "the function.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_FETCH_TYPE_OUT_OF_RANGE1 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FETCH_TYPE_OUT_OF_RANGE,
                      "The value of the SQL_ATTR_CURSOR_TYPE statement attribute was "
                      "SQL_CURSOR_FORWARD_ONLY, and the value of argument FetchOrientation "
                      "was not SQL_FETCH_NEXT.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_FETCH_TYPE_OUT_OF_RANGE2 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FETCH_TYPE_OUT_OF_RANGE,
                      "TThe value of the SQL_ATTR_CURSOR_SCROLLABLE statement attribute "
                      "was SQL_NONSCROLLABLE, and the value of argument FetchOrientation "
                      "was not SQL_FETCH_NEXT.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_FETCH_TYPE_OUT_OF_RANGE3 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FETCH_TYPE_OUT_OF_RANGE,
                      "The value specified for the argument FetchOrientation was invalid.",
                      &sErrorStack );
    }

    STL_FINISH;

    if( sArd != NULL )
    {
        sStmt->mUsedRowsetSize = sArd->mArraySize;
    }

    if( sSetRowCountPtr == STL_TRUE )
    {
        sIrd->mRowProcessed = sOrgRowCountPtr;
    }

    if( sSetRowStatusArray == STL_TRUE )
    {
        sIrd->mArrayStatusPtr = sOrgRowStatusArray;
    }
    
    if( ( sRet == SQL_ERROR ) ||
        ( sRet == SQL_SUCCESS_WITH_INFO ) )
    {
        zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_STMT,
                               aStatementHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }
        
        if( sStmt != NULL )
        {
            zlcCheckSessionAndCleanup( ZLS_STMT_DBC(sStmt) );
        }
    }

    return sRet;
}

SQLRETURN SQL_API SQLFetch( SQLHSTMT aStatementHandle )
{
    SQLRETURN       sRet             = SQL_ERROR;
    zlsStmt       * sStmt            = NULL;
    zldDesc       * sArd             = NULL;
    stlUInt64       sArraySize       = 0;
    stlInt64        sLastResultRow   = STL_INT64_MAX;
    stlBool         sNotFound        = STL_FALSE;
    stlBool         sSuccessWithInfo = STL_FALSE;
    stlErrorData  * sErrorData       = NULL;
    stlErrorStack   sErrorStack;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sStmt = (zlsStmt*)aStatementHandle;

    STL_TRY_THROW( sStmt != NULL,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sStmt->mType == SQL_HANDLE_STMT,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( zldDiagClear( SQL_HANDLE_STMT,
                           aStatementHandle,
                           &sErrorStack ) == STL_SUCCESS );

    STL_TRY( zlcCheckSession( sStmt->mParentDbc,
                              &sErrorStack ) == STL_SUCCESS );
    
    switch( sStmt->mTransition )
    {
        case ZLS_TRANSITION_STATE_S0 :
            STL_THROW( RAMP_ERR_INVALID_HANDLE );
            break;
        case ZLS_TRANSITION_STATE_S1 :
        case ZLS_TRANSITION_STATE_S2 :
        case ZLS_TRANSITION_STATE_S3 :
            STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR1 );
            break;
        case ZLS_TRANSITION_STATE_S4 :
            STL_THROW( RAMP_ERR_INVALID_CURSOR_STATE1 );
            break;
        case ZLS_TRANSITION_STATE_S7 :
            STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR2 );
            break;
        case ZLS_TRANSITION_STATE_S8 :
        case ZLS_TRANSITION_STATE_S9 :
        case ZLS_TRANSITION_STATE_S10 :
        case ZLS_TRANSITION_STATE_S11 :
        case ZLS_TRANSITION_STATE_S12 :
            STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR3 );
            break;
        default :
            break;
    }

    STL_TRY_THROW( sStmt->mResult.mStatus != ZLL_CURSOR_ROW_STATUS_NA,
                   RAMP_ERR_INVALID_CURSOR_STATE2 );

    sArd       = sStmt->mArd;
    sArraySize = (stlUInt64)sArd->mArraySize;

    STL_TRY_THROW( sStmt->mResult.mBlockCurrentRow != ZLR_ROWSET_AFTER_END,
                   RAMP_NO_DATA );
    
    if( sStmt->mResult.mBlockCurrentRow == ZLR_ROWSET_BEFORE_START )
    {
        sStmt->mResult.mCurrentRowIdx = 1;
    }
    else
    {
        if( sStmt->mResult.mLastRowIdx != -1 )
        {
            if( sStmt->mMaxRows == 0 )
            {
                sLastResultRow = sStmt->mResult.mLastRowIdx;
            }
            else
            {
                sLastResultRow = STL_MIN( sStmt->mResult.mLastRowIdx, sStmt->mMaxRows );
            }
        }

        STL_TRY_THROW( sStmt->mResult.mCurrentRowIdx <= sLastResultRow - (stlInt64)sArraySize,
                       RAMP_NO_DATA );

        sStmt->mResult.mCurrentRowIdx += sStmt->mUsedRowsetSize;
    }
    
    STL_TRY( zlrFetch( aStatementHandle,
                       sArraySize,
                       sStmt->mResult.mCurrentRowIdx,
                       &sNotFound,
                       &sSuccessWithInfo,
                       &sErrorStack ) == STL_SUCCESS );

    sStmt->mTransition = ZLS_TRANSITION_STATE_S6;

    STL_TRY_THROW( sNotFound == STL_FALSE, RAMP_NO_DATA );
    STL_TRY_THROW( sSuccessWithInfo == STL_FALSE, RAMP_SUCCESS_WITH_INFO );

    sStmt->mUsedRowsetSize = (SQLULEN)sArraySize;
    
    sRet = SQL_SUCCESS;

    zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

    return sRet;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_CATCH( RAMP_NO_DATA )
    {
        sRet = SQL_NO_DATA;

        sStmt->mResult.mBlockCurrentRow = ZLR_ROWSET_AFTER_END;
    }

    STL_CATCH( RAMP_SUCCESS_WITH_INFO )
    {
        sRet = SQL_SUCCESS_WITH_INFO;
    }

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR1 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      "The specified StatementHandle was not in an executed state. "
                      "The function was called without first calling "
                      "SQLExecDirect, SQLExecute or a catalog function.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR2 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      "SQLFetch was called for the StatementHandle after "
                      "SQLExtendedFetch was called and before SQLFreeStmt with "
                      "the SQL_CLOSE option was called.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR3 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      NULL,
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_CURSOR_STATE1 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CURSOR_STATE,
                      "The StatementHandle was in an executed state but "
                      "no result set was associated with the StatementHandle.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_CURSOR_STATE2 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CURSOR_STATE,
                      NULL,
                      &sErrorStack );
    }

    STL_FINISH;

    if( sArd != NULL )
    {
        sStmt->mUsedRowsetSize = sArd->mArraySize;
    }

    if( ( sRet == SQL_ERROR ) ||
        ( sRet == SQL_SUCCESS_WITH_INFO ) )
    {
        zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_STMT,
                               aStatementHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }
        
        if( sStmt != NULL )
        {
            zlcCheckSessionAndCleanup( ZLS_STMT_DBC(sStmt) );
        }
    }

    return sRet;
}

SQLRETURN SQL_API SQLFetchScroll( SQLHSTMT    aStatementHandle,
                                  SQLSMALLINT aFetchOrientation,
                                  SQLLEN      aFetchOffset )
{
    SQLRETURN       sRet             = SQL_ERROR;
    zlsStmt       * sStmt            = NULL;
    zldDesc       * sArd             = NULL;
    stlUInt64       sArraySize       = 0;
    stlInt64        sLastResultRow   = STL_INT64_MAX;
    stlInt64        sRelativeOffset  = 0;
    stlInt64        sAbsOffset       = 0;
    stlBool         sNotFound        = STL_FALSE;
    stlBool         sSuccessWithInfo = STL_FALSE;
    stlErrorData  * sErrorData       = NULL;
    stlErrorStack   sErrorStack;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sStmt = (zlsStmt*)aStatementHandle;

    STL_TRY_THROW( sStmt != NULL, RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sStmt->mType == SQL_HANDLE_STMT,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( zldDiagClear( SQL_HANDLE_STMT,
                           aStatementHandle,
                           &sErrorStack ) == STL_SUCCESS );

    STL_TRY( zlcCheckSession( sStmt->mParentDbc,
                              &sErrorStack ) == STL_SUCCESS );
    
    switch( sStmt->mTransition )
    {
        case ZLS_TRANSITION_STATE_S0 :
            STL_THROW( RAMP_ERR_INVALID_HANDLE );
            break;
        case ZLS_TRANSITION_STATE_S1 :
        case ZLS_TRANSITION_STATE_S2 :
        case ZLS_TRANSITION_STATE_S3 :
            STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR1 );
            break;
        case ZLS_TRANSITION_STATE_S4 :
            STL_THROW( RAMP_ERR_INVALID_CURSOR_STATE1 );
            break;
        case ZLS_TRANSITION_STATE_S7 :
            STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR2 );
            break;
        case ZLS_TRANSITION_STATE_S8 :
        case ZLS_TRANSITION_STATE_S9 :
        case ZLS_TRANSITION_STATE_S10 :
        case ZLS_TRANSITION_STATE_S11 :
        case ZLS_TRANSITION_STATE_S12 :
            STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR3 );
            break;
        default :
            break;
    }

    STL_TRY_THROW( sStmt->mResult.mStatus != ZLL_CURSOR_ROW_STATUS_NA,
                   RAMP_ERR_INVALID_CURSOR_STATE2 );

    if( sStmt->mCursorType == SQL_CURSOR_FORWARD_ONLY )
    {
        STL_TRY_THROW( aFetchOrientation == SQL_FETCH_NEXT,
                       RAMP_ERR_FETCH_TYPE_OUT_OF_RANGE1 );
    }

    if( sStmt->mCursorScrollable == SQL_NONSCROLLABLE )
    {
        STL_TRY_THROW( aFetchOrientation == SQL_FETCH_NEXT,
                       RAMP_ERR_FETCH_TYPE_OUT_OF_RANGE2 );
    }

    sArd       = sStmt->mArd;
    sArraySize = sArd->mArraySize;

    if( sStmt->mResult.mLastRowIdx != -1 )
    {
        STL_TRY_THROW( sStmt->mResult.mLastRowIdx != 0, RAMP_NO_DATA );

        if( sStmt->mMaxRows == 0 )
        {
            sLastResultRow = sStmt->mResult.mLastRowIdx;
        }
        else
        {
            sLastResultRow = STL_MIN( sStmt->mResult.mLastRowIdx, sStmt->mMaxRows );
        }
    }

    /* Positioning the Cursor */
    switch( aFetchOrientation )
    {
        case SQL_FETCH_NEXT :
            /*================================================================================
              | Condition                                     | First row of new rowset      |
              ================================================================================
              | Before start                                  | 1                            |
              | CurrRowsetStart + RowsetSize <= LastResultRow | CurrRowsetStart + RowsetSize |
              | CurrRowsetStart + RowsetSize >  LastResultRow | After end                    |
              | After end                                     | After end                    |
              ================================================================================ */
            STL_TRY_THROW( sStmt->mResult.mBlockCurrentRow != ZLR_ROWSET_AFTER_END,
                           RAMP_NO_DATA_AFTER_END );
    
            if( sStmt->mResult.mBlockCurrentRow == ZLR_ROWSET_BEFORE_START )
            {
                sStmt->mResult.mCurrentRowIdx = 1;
            }
            else
            {
                STL_TRY_THROW( sStmt->mResult.mCurrentRowIdx <= sLastResultRow - sArraySize,
                               RAMP_NO_DATA_AFTER_END );

                sStmt->mResult.mCurrentRowIdx += sStmt->mUsedRowsetSize;
            }
    
            break;
        case SQL_FETCH_PRIOR :
            /*==================================================================================
              | Condition                                     | First row of new rowset        |
              ==================================================================================
              | Before start                                  | Before start                   |
              | CurrRowsetStart = 1                           | Before start                   |
              | 1 < CurrRowsetStart <= RowsetSize             | 1                              |
              | CurrRowsetStart > RowsetSize                  | CurrRowsetStart - RowsetSize   |
              | After end AND LastResultRow < RowsetSize      | 1                              |
              | After end AND LastResultRow >= RowsetSize     | LastResultRow - RowsetSize + 1 |
              ================================================================================== */
            STL_TRY_THROW( sStmt->mResult.mBlockCurrentRow != ZLR_ROWSET_BEFORE_START,
                           RAMP_NO_DATA_BEFORE_START );

            sStmt->mResult.mIsEndOfScan = STL_FALSE;
    
            if( sStmt->mResult.mBlockCurrentRow == ZLR_ROWSET_AFTER_END )
            {
                if( sLastResultRow < sArraySize )
                {
                    sStmt->mResult.mCurrentRowIdx = 1;
                }
                else
                {
                    sStmt->mResult.mCurrentRowIdx = sLastResultRow - sArraySize + 1;
                }
            }
            else
            {
                STL_TRY_THROW( sStmt->mResult.mCurrentRowIdx != 1, RAMP_NO_DATA_BEFORE_START );

                if( sStmt->mResult.mCurrentRowIdx <= sArraySize )
                {
                    sStmt->mResult.mCurrentRowIdx = 1;
                }
                else
                {
                    sStmt->mResult.mCurrentRowIdx -= sArraySize;
                }
            }
            break;
        case SQL_FETCH_FIRST :
            /*=======================================
              | Condition | First row of new rowset |
              =======================================
              | Any       | 1                       |
              ======================================= */
            sStmt->mResult.mIsEndOfScan = STL_FALSE;
            sStmt->mResult.mCurrentRowIdx = 1;
            break;
        case SQL_FETCH_LAST :
            /*==================================================================================
              | Condition                                     | First row of new rowset        |
              ==================================================================================
              | RowsetSize <= LastResultRow                   | LastResultRow - RowsetSize + 1 |
              | RowsetSize >  LastResultRow                   | 1                              |
              ================================================================================== */
            sStmt->mResult.mIsEndOfScan = STL_FALSE;
    
            /* 마지막 row의 번호를 알 수 없을 경우
             * SQL_FETCH_ABSOLUTE로 뒤에서부터 rowset 크기만큼 갖고 온다. */
            if( sLastResultRow == STL_INT64_MAX )
            {
                sStmt->mResult.mCurrentRowIdx = 0 - sArraySize;
            }
            else
            {
                if( sArraySize <= sLastResultRow )
                {
                    sStmt->mResult.mCurrentRowIdx = sLastResultRow - sArraySize + 1;
                }
                else
                {
                    sStmt->mResult.mCurrentRowIdx = 1;
                }
            }
            break;
        case SQL_FETCH_RELATIVE :
            /*===============================================================================================================================
              | Condition                                                                                   | First row of new rowset       |
              ===============================================================================================================================
              | (Before start AND FetchOffset > 0) OR (After end AND FetchOffset < 0)                       | --                            |
              | BeforeStart AND FetchOffset <= 0                                                            | Before start                  |
              | CurrRowsetStart = 1 AND FetchOffset < 0                                                     | Before start                  |
              | CurrRowsetStart > 1 AND CurrRowsetStart + FetchOffset < 1 AND | FetchOffset | >  RowsetSize | Before start                  |
              | CurrRowsetStart > 1 AND CurrRowsetStart + FetchOffset < 1 AND | FetchOffset | <= RowsetSize | 1                             |
              | 1 <= CurrRowsetStart + FetchOffset <= LastResultRow                                         | CurrRowsetStart + FetchOffset |
              | CurrRowsetStart + FetchOffset > LastResultRow                                               | After end                     |
              | After end AND FetchOffset >= 0                                                              | After end                     |
              =============================================================================================================================== */
            sStmt->mResult.mIsEndOfScan = STL_FALSE;
    
            if( sStmt->mResult.mBlockCurrentRow == ZLR_ROWSET_BEFORE_START )
            {
                STL_TRY_THROW( aFetchOffset > 0, RAMP_NO_DATA_BEFORE_START );

                STL_THROW( RAMP_FETCH_ABSOLUTE );
            }
            else if( sStmt->mResult.mBlockCurrentRow == ZLR_ROWSET_AFTER_END )
            {
                STL_TRY_THROW( aFetchOffset < 0, RAMP_NO_DATA_AFTER_END );

                STL_THROW( RAMP_FETCH_ABSOLUTE );
            }
            else
            {
                if( sStmt->mResult.mCurrentRowIdx == 1 )
                {
                    STL_TRY_THROW( aFetchOffset >= 0, RAMP_NO_DATA_BEFORE_START );
                }
                
                sRelativeOffset = sStmt->mResult.mCurrentRowIdx + aFetchOffset;

                STL_TRY_THROW( sRelativeOffset <= sLastResultRow, RAMP_NO_DATA_AFTER_END );

                if( sRelativeOffset < 1 )
                {
                    sAbsOffset = stlAbsInt64(aFetchOffset);

                    STL_TRY_THROW( sAbsOffset <= sArraySize, RAMP_NO_DATA_BEFORE_START );

                    sStmt->mResult.mCurrentRowIdx = 1;
                }
                else
                {
                    sStmt->mResult.mCurrentRowIdx = sRelativeOffset;
                }
            }
            break;
        case SQL_FETCH_ABSOLUTE :
            STL_RAMP( RAMP_FETCH_ABSOLUTE );
            /*===========================================================================================================================
              | Condition                                                                             | First row of new rowset         |
              ===========================================================================================================================
              | FetchOffset < 0 AND | FetchOffset | <= LastResultRow                                  | LastResultRow + FetchOffset + 1 |
              | FetchOffset < 0 AND | FetchOffset | > LastResultRow AND | FetchOffset | >  RowsetSize | Before start                    |
              | FetchOffset < 0 AND | FetchOffset | > LastResultRow AND | FetchOffset | <= RowsetSize | 1                               |
              | FetchOffset = 0                                                                       | Before start                    |
              | 1 <= FetchOffset <= LastResultRow                                                     | FetchOffset                     |
              | FetchOffset > LastResultRow                                                           | After end                       |
              =========================================================================================================================== */
            STL_TRY_THROW( aFetchOffset <= sLastResultRow, RAMP_NO_DATA_AFTER_END );
            STL_TRY_THROW( aFetchOffset != 0, RAMP_NO_DATA_BEFORE_START );

            sStmt->mResult.mIsEndOfScan = STL_FALSE;

            if( aFetchOffset < 0 )
            {
                /* 마지막 row의 번호를 알 수 없을 경우
                 * SQL_FETCH_ABSOLUTE로 뒤에서부터 rowset 크기만큼 갖고 온다. */
                if( sLastResultRow == STL_INT64_MAX )
                {
                    sStmt->mResult.mCurrentRowIdx = aFetchOffset;
                }
                else
                {
                    sAbsOffset = stlAbsInt64(aFetchOffset);

                    if( sAbsOffset <= sLastResultRow )
                    {
                        sStmt->mResult.mCurrentRowIdx = sLastResultRow + aFetchOffset + 1;
                    }
                    else
                    {
                        STL_TRY_THROW( sAbsOffset <= sArraySize, RAMP_NO_DATA_BEFORE_START );

                        sStmt->mResult.mCurrentRowIdx = 1;
                    }
                }
            }
            else
            {
                sStmt->mResult.mCurrentRowIdx = aFetchOffset;
            }
            break;
        case SQL_FETCH_BOOKMARK :
            STL_THROW( RAMP_ERR_DRIVER_DOES_NOT_SUPPORT_THIS_FUNCTION );
            break;
        default :
            STL_THROW( RAMP_ERR_FETCH_TYPE_OUT_OF_RANGE3 );
            break;
    }
    
    STL_TRY( zlrFetch( aStatementHandle,
                       sArraySize,
                       sStmt->mResult.mCurrentRowIdx,
                       &sNotFound,
                       &sSuccessWithInfo,
                       &sErrorStack ) == STL_SUCCESS );

    sStmt->mTransition = ZLS_TRANSITION_STATE_S6;

    STL_TRY_THROW( sNotFound == STL_FALSE, RAMP_NO_DATA );
    STL_TRY_THROW( sSuccessWithInfo == STL_FALSE, RAMP_SUCCESS_WITH_INFO );
    
    sStmt->mUsedRowsetSize = sArraySize;
    
    sRet = SQL_SUCCESS;

    zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

    return sRet;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_CATCH( RAMP_NO_DATA )
    {
        sRet = SQL_NO_DATA;
    }

    STL_CATCH( RAMP_NO_DATA_BEFORE_START )
    {
        sRet = SQL_NO_DATA;

        sStmt->mResult.mBlockCurrentRow = ZLR_ROWSET_BEFORE_START;
    }

    STL_CATCH( RAMP_NO_DATA_AFTER_END )
    {
        sRet = SQL_NO_DATA;

        sStmt->mResult.mBlockCurrentRow = ZLR_ROWSET_AFTER_END;
    }

    STL_CATCH( RAMP_SUCCESS_WITH_INFO )
    {
        sRet = SQL_SUCCESS_WITH_INFO;
    }

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR1 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      "The specified StatementHandle was not in an executed state. "
                      "The function was called without first calling "
                      "SQLExecDirect, SQLExecute or a catalog function.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR2 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      "SQLFetch was called for the StatementHandle after "
                      "SQLExtendedFetch was called and before SQLFreeStmt with "
                      "the SQL_CLOSE option was called.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR3 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      NULL,
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_CURSOR_STATE1 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CURSOR_STATE,
                      "The StatementHandle was in an executed state but "
                      "no result set was associated with the StatementHandle.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_CURSOR_STATE2 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CURSOR_STATE,
                      NULL,
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_DRIVER_DOES_NOT_SUPPORT_THIS_FUNCTION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_DRIVER_DOES_NOT_SUPPORT_THIS_FUNCTION,
                      "The driver associated with the StatementHandle does not support "
                      "the function.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_FETCH_TYPE_OUT_OF_RANGE1 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FETCH_TYPE_OUT_OF_RANGE,
                      "The value of the SQL_ATTR_CURSOR_TYPE statement attribute was "
                      "SQL_CURSOR_FORWARD_ONLY, and the value of argument FetchOrientation "
                      "was not SQL_FETCH_NEXT.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_FETCH_TYPE_OUT_OF_RANGE2 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FETCH_TYPE_OUT_OF_RANGE,
                      "TThe value of the SQL_ATTR_CURSOR_SCROLLABLE statement attribute "
                      "was SQL_NONSCROLLABLE, and the value of argument FetchOrientation "
                      "was not SQL_FETCH_NEXT.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_FETCH_TYPE_OUT_OF_RANGE3 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FETCH_TYPE_OUT_OF_RANGE,
                      "The value specified for the argument FetchOrientation was invalid.",
                      &sErrorStack );
    }

    STL_FINISH;

    if( sArd != NULL )
    {
        sStmt->mUsedRowsetSize = sArd->mArraySize;
    }

    if( ( sRet == SQL_ERROR ) ||
        ( sRet == SQL_SUCCESS_WITH_INFO ) )
    {
        zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_STMT,
                               aStatementHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }
        
        if( sStmt != NULL )
        {
            zlcCheckSessionAndCleanup( ZLS_STMT_DBC(sStmt) );
        }
    }

    return sRet;
}

static SQLRETURN zllForeignKeys( SQLHSTMT        aStatementHandle,
                                 SQLCHAR       * aPKCatalogName,
                                 SQLSMALLINT     aNameLength1,
                                 SQLCHAR       * aPKSchemaName,
                                 SQLSMALLINT     aNameLength2,
                                 SQLCHAR       * aPKTableName,
                                 SQLSMALLINT     aNameLength3,
                                 SQLCHAR       * aFKCatalogName,
                                 SQLSMALLINT     aNameLength4,
                                 SQLCHAR       * aFKSchemaName,
                                 SQLSMALLINT     aNameLength5,
                                 SQLCHAR       * aFKTableName,
                                 SQLSMALLINT     aNameLength6,
                                 stlErrorStack * aErrorStack )
{
    SQLRETURN       sRet              = SQL_ERROR;
    zlcDbc        * sDbc              = NULL;
    zlsStmt       * sStmt             = NULL;
    stlInt64        sAffectedRowCount = -1;
    stlBool         sSuccessWithInfo  = STL_FALSE;
    
    stlChar         sBuffer[2048];
    stlInt32        sBufferLength = STL_SIZEOF( sBuffer );

    stlInt16        sNameLength1 = 0;
    stlInt16        sNameLength2 = 0;
    stlInt16        sNameLength3 = 0;
    stlInt16        sNameLength4 = 0;
    stlInt16        sNameLength5 = 0;
    stlInt16        sNameLength6 = 0;

    stlInt32        sPos = 0;

    SQLCHAR * sSELECT      = (SQLCHAR*)"SELECT "
        "  CAST(UNIQUE_CONSTRAINT_CATALOG     AS VARCHAR(128 OCTETS)) AS PKTABLE_CAT, "
        "  CAST(UNIQUE_CONSTRAINT_SCHEMA      AS VARCHAR(128 OCTETS)) AS PKTABLE_SCHEM, "
        "  CAST(UNIQUE_CONSTRAINT_TABLE_NAME  AS VARCHAR(128 OCTETS)) AS PKTABLE_NAME, "
        "  CAST(UNIQUE_CONSTRAINT_COLUMN_NAME AS VARCHAR(128 OCTETS)) AS PKCOLUMN_NAME, "
        "  CAST(CONSTRAINT_CATALOG            AS VARCHAR(128 OCTETS)) AS FKTABLE_CAT, "
        "  CAST(CONSTRAINT_SCHEMA             AS VARCHAR(128 OCTETS)) AS FKTABLE_SCHEM, "
        "  CAST(CONSTRAINT_TABLE_NAME         AS VARCHAR(128 OCTETS)) AS FKTABLE_NAME, "
        "  CAST(CONSTRAINT_COLUMN_NAME        AS VARCHAR(128 OCTETS)) AS FKCOLUMN_NAME, "
        "  CAST(ORDINAL_POSITION              AS NATIVE_SMALLINT)     AS KEY_SEQ, "
        "  CAST(CASE UPDATE_RULE "
        "         WHEN 'CASCADE'     THEN 0 "
        "         WHEN 'SET NULL'    THEN 2 "
        "         WHEN 'NO ACTION'   THEN 3 "
        "         WHEN 'SET DEFAULT' THEN 4 "
        "         END                         AS NATIVE_SMALLINT)     AS UPDATE_RULE, "
        "  CAST(CASE DELETE_RULE "
        "         WHEN 'CASCADE'     THEN 0 "
        "         WHEN 'SET NULL'    THEN 2 "
        "         WHEN 'NO ACTION'   THEN 3 "
        "         WHEN 'SET DEFAULT' THEN 4 "
        "         END                         AS NATIVE_SMALLINT)     AS DELETE_RULE, "
        "  CAST(CONSTRAINT_NAME               AS VARCHAR(128 OCTETS)) AS FK_NAME, "
        "  CAST(UNIQUE_CONSTRAINT_NAME        AS VARCHAR(128 OCTETS)) AS PK_NAME, "
        "  CAST(CASE WHEN IS_DEFERRABLE = FALSE THEN 7 "
        "            ELSE "
        "              CASE WHEN INITIALLY_DEFERRED = TRUE THEN 5 "
        "                   ELSE 6 "
        "                   END "
        "            END                      AS SMALLINT)            AS DEFERRABILITY "
        "FROM INFORMATION_SCHEMA.REFERENTIAL_CONSTRAINTS ";
    SQLCHAR * sWHERE       = (SQLCHAR*)"WHERE IS_PRIMARY_KEY = TRUE ";
    SQLCHAR * sORDER_BY_FK = (SQLCHAR*)"ORDER BY FKTABLE_CAT, FKTABLE_SCHEM, FKTABLE_NAME, KEY_SEQ";
    SQLCHAR * sORDER_BY_PK = (SQLCHAR*)"ORDER BY PKTABLE_CAT, PKTABLE_SCHEM, PKTABLE_NAME, KEY_SEQ";
    
    sStmt = (zlsStmt*)aStatementHandle;
    sDbc  = sStmt->mParentDbc;

    STL_TRY( zldDiagClear( SQL_HANDLE_STMT,
                           aStatementHandle,
                           aErrorStack ) == STL_SUCCESS );

    STL_TRY( zlcCheckSession( sDbc,
                              aErrorStack ) == STL_SUCCESS );
    
    switch( sDbc->mTransition )
    {
        case ZLC_TRANSITION_STATE_C0 :
        case ZLC_TRANSITION_STATE_C1 :
        case ZLC_TRANSITION_STATE_C2 :
        case ZLC_TRANSITION_STATE_C3 :
        case ZLC_TRANSITION_STATE_C4 :
            STL_THROW( RAMP_ERR_INVALID_HANDLE );
            break;
        case ZLC_TRANSITION_STATE_C5 :
        case ZLC_TRANSITION_STATE_C6 :
            break;
    }

    switch( sStmt->mTransition )
    {
        case ZLS_TRANSITION_STATE_S1 :
        case ZLS_TRANSITION_STATE_S2 :
        case ZLS_TRANSITION_STATE_S3 :
        case ZLS_TRANSITION_STATE_S4 :
            break;
        case ZLS_TRANSITION_STATE_S5 :
            STL_THROW( RAMP_ERR_INVALID_CURSOR_STATE_1 );
            break;
        case ZLS_TRANSITION_STATE_S6 :
        case ZLS_TRANSITION_STATE_S7 :
            STL_THROW( RAMP_ERR_INVALID_CURSOR_STATE_2 );
            break;
        case ZLS_TRANSITION_STATE_S8 :
        case ZLS_TRANSITION_STATE_S9 :
        case ZLS_TRANSITION_STATE_S10 :
        case ZLS_TRANSITION_STATE_S11 :
        case ZLS_TRANSITION_STATE_S12 :
            STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR );
            break;
    }

    if( aNameLength1 != SQL_NTS )
    {
        STL_TRY_THROW( aNameLength1 >= 0, RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH );

        sNameLength1 = aNameLength1;
    }
    else
    {
        if (aPKCatalogName != NULL)
        {
            sNameLength1 = stlStrlen((stlChar*)aPKCatalogName);
        }
        
    }

    if( aNameLength2 != SQL_NTS )
    {
        STL_TRY_THROW( aNameLength2 >= 0, RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH );

        sNameLength2 = aNameLength2;
    }
    else
    {
        if (aPKSchemaName != NULL)
        {
            sNameLength2 = stlStrlen((stlChar*)aPKSchemaName);
        }
        
    }

    if( aNameLength3 != SQL_NTS )
    {
        STL_TRY_THROW( aNameLength3 >= 0, RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH );

        sNameLength3 = aNameLength3;
    }
    else
    {
        if (aPKTableName != NULL)
        {
            sNameLength3 = stlStrlen((stlChar*)aPKTableName);
        }        
    }

    if( aNameLength4 != SQL_NTS )
    {
        STL_TRY_THROW( aNameLength4 >= 0, RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH );

        sNameLength4 = aNameLength4;
    }
    else
    {
        if (aFKCatalogName != NULL)
        {
            sNameLength4 = stlStrlen((stlChar*)aFKCatalogName);
        }        
    }

    if( aNameLength5 != SQL_NTS )
    {
        STL_TRY_THROW( aNameLength5 >= 0, RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH );

        sNameLength5 = aNameLength5;
    }
    else
    {
        if (aFKSchemaName != NULL)
        {
            sNameLength5 = stlStrlen((stlChar*)aFKSchemaName);
        }        
    }

    if( aNameLength6 != SQL_NTS )
    {
        STL_TRY_THROW( aNameLength6 >= 0, RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH );

        sNameLength6 = aNameLength6;
    }
    else
    {
        if (aFKTableName != NULL)
        {
            sNameLength6 = stlStrlen((stlChar*)aFKTableName);
        }        
    }

    STL_TRY_THROW( (aPKTableName != NULL) ||
                   (aFKTableName != NULL), RAMP_ERR_INVALID_USE_OF_NULL_POINTER_1 );
    
    if( sStmt->mMetadataId == STL_TRUE )
    {
        if( aPKCatalogName != NULL )
        {
            if( (aPKCatalogName[0] == '\"') &&
                (aPKCatalogName[sNameLength1-1] == '\"') )
            {
                aPKCatalogName[sNameLength1-1] = 0;
                aPKCatalogName++;

                sNameLength1 = sNameLength1 - 2;
            }
            else
            {
                stlToupperString( (stlChar*)aPKCatalogName, (stlChar*)aPKCatalogName );
            }
        }

        STL_TRY_THROW( aPKSchemaName != NULL, RAMP_ERR_INVALID_USE_OF_NULL_POINTER_2 );

        if( (aPKSchemaName[0] == '\"') &&
            (aPKSchemaName[sNameLength2-1] == '\"') )
        {
            aPKSchemaName[sNameLength2-1] = 0;
            aPKSchemaName++;

            sNameLength2 = sNameLength2 - 2;
        }
        else
        {
            stlToupperString( (stlChar*)aPKSchemaName, (stlChar*)aPKSchemaName );
        }

        STL_TRY_THROW( aPKTableName != NULL, RAMP_ERR_INVALID_USE_OF_NULL_POINTER_2 );

        if( (aPKTableName[0] == '\"') &&
            (aPKTableName[sNameLength3-1] == '\"') )
        {
            aPKTableName[sNameLength3-1] = 0;
            aPKTableName++;

            sNameLength3 = sNameLength3 - 2;
        }
        else
        {
            stlToupperString( (stlChar*)aPKTableName, (stlChar*)aPKTableName );
        }

        if( aFKCatalogName != NULL )
        {
            if( (aFKCatalogName[0] == '\"') &&
                (aFKCatalogName[sNameLength4-1] == '\"') )
            {
                aFKCatalogName[sNameLength4-1] = 0;
                aFKCatalogName++;

                sNameLength4 = sNameLength4 - 2;
            }
            else
            {
                stlToupperString( (stlChar*)aFKCatalogName, (stlChar*)aFKCatalogName );
            }
        }

        STL_TRY_THROW( aFKSchemaName != NULL, RAMP_ERR_INVALID_USE_OF_NULL_POINTER_2 );

        if( (aFKSchemaName[0] == '\"') &&
            (aFKSchemaName[sNameLength5-1] == '\"') )
        {
            aFKSchemaName[sNameLength5-1] = 0;
            aFKSchemaName++;

            sNameLength5 = sNameLength5 - 2;
        }
        else
        {
            stlToupperString( (stlChar*)aFKSchemaName, (stlChar*)aFKSchemaName );
        }

        STL_TRY_THROW( aFKTableName != NULL, RAMP_ERR_INVALID_USE_OF_NULL_POINTER_2 );

        if( (aFKTableName[0] == '\"') &&
            (aFKTableName[sNameLength6-1] == '\"') )
        {
            aFKTableName[sNameLength6-1] = 0;
            aFKTableName++;

            sNameLength6 = sNameLength6 - 2;
        }
        else
        {
            stlToupperString( (stlChar*)aFKTableName, (stlChar*)aFKTableName );
        }
    }

    sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos,
                         "%s%s",
                         sSELECT,
                         sWHERE );

    if( (aPKCatalogName != NULL) && (sNameLength1 > 0) )
    {
        sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos,
                             " AND UNIQUE_CONSTRAINT_CATALOG = '" );
            
        stlMemcpy( sBuffer + sPos, aPKCatalogName, sNameLength1 );
        sPos += sNameLength1;

        sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos, "' " );
    }

    if( (aPKSchemaName != NULL) && (sNameLength2 > 0) )
    {
        sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos,
                             " AND UNIQUE_CONSTRAINT_SCHEMA = '" );
            
        stlMemcpy( sBuffer + sPos, aPKSchemaName, sNameLength2 );
        sPos += sNameLength2;

        sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos, "' " );
    }

    if( (aPKTableName != NULL) && (sNameLength3 > 0) )
    {
        sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos,
                             " AND UNIQUE_CONSTRAINT_TABLE_NAME = '" );
            
        stlMemcpy( sBuffer + sPos, aPKTableName, sNameLength3 );
        sPos += sNameLength3;

        sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos, "' " );
    }

    if( (aFKCatalogName != NULL) && (sNameLength4 > 0) )
    {
        sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos,
                             " AND CONSTRAINT_CATALOG = '" );
            
        stlMemcpy( sBuffer + sPos, aFKCatalogName, sNameLength4 );
        sPos += sNameLength4;

        sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos, "' " );
    }

    if( (aFKSchemaName != NULL) && (sNameLength5 > 0) )
    {
        sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos,
                             " AND CONSTRAINT_SCHEMA = '" );
            
        stlMemcpy( sBuffer + sPos, aFKSchemaName, sNameLength5 );
        sPos += sNameLength5;

        sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos, "' " );
    }

    if( (aFKTableName != NULL) && (sNameLength6 > 0) )
    {
        sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos,
                             " AND CONSTRAINT_TABLE_NAME = '" );
            
        stlMemcpy( sBuffer + sPos, aFKTableName, sNameLength6 );
        sPos += sNameLength6;

        sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos, "' " );
    }

    if( aPKTableName != NULL )
    {
        sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos,
                             "%s",
                             sORDER_BY_FK );
    }
    else
    {
        sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos,
                             "%s",
                             sORDER_BY_PK );
    }

    STL_TRY( zlxExecDirect( sStmt,
                            sBuffer,
                            &sAffectedRowCount,
                            &sSuccessWithInfo,
                            aErrorStack ) == STL_SUCCESS );

    STL_TRY_THROW( sSuccessWithInfo == STL_FALSE, RAMP_SUCCESS_WITH_INFO );

    sRet = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_STRING_OR_BUFFER_LENGTH,
                      "The value of one of the length arguments was less than 0 "
                      "but not equal to SQL_NTS.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_CURSOR_STATE_1 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CURSOR_STATE,
                      "A cursor was open on the StatementHandle, "
                      "but SQLFetch or SQLFetchScroll had not been called.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_CURSOR_STATE_2 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CURSOR_STATE,
                      "A cursor was open on the StatementHandle, "
                      "and SQLFetch or SQLFetchScroll had been called.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_USE_OF_NULL_POINTER_1 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_USE_OF_NULL_POINTER,
                      "The arguments PKTableName and FKTableName were both null pointers.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_USE_OF_NULL_POINTER_2 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_USE_OF_NULL_POINTER,
                      "The SQL_ATTR_METADATA_ID statement attribute was set to SQL_TRUE, "
                      "and the FKSchemaName, PKSchemaName, FKTableName, or PKTableName "
                      "argument was a null pointer.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_CATCH( RAMP_SUCCESS_WITH_INFO )
    {
        sRet = SQL_SUCCESS_WITH_INFO;
    }

    STL_FINISH;

    return sRet;
}

SQLRETURN SQL_API SQLForeignKeys( SQLHSTMT      aStatementHandle,
                                  SQLCHAR     * aPKCatalogName,
                                  SQLSMALLINT   aNameLength1,
                                  SQLCHAR     * aPKSchemaName,
                                  SQLSMALLINT   aNameLength2,
                                  SQLCHAR     * aPKTableName,
                                  SQLSMALLINT   aNameLength3,
                                  SQLCHAR     * aFKCatalogName,
                                  SQLSMALLINT   aNameLength4,
                                  SQLCHAR     * aFKSchemaName,
                                  SQLSMALLINT   aNameLength5,
                                  SQLCHAR     * aFKTableName,
                                  SQLSMALLINT   aNameLength6 )
{
    SQLRETURN       sRet  = SQL_ERROR;
    zlsStmt       * sStmt = NULL;
    stlErrorStack   sErrorStack;
    stlErrorData  * sErrorData = NULL;
    stlRegionMark   sMark;
    stlInt32        sState = 0;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sStmt = (zlsStmt*)aStatementHandle;

    STL_TRY_THROW( sStmt != NULL,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sStmt->mType == SQL_HANDLE_STMT,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( stlMarkRegionMem( &sStmt->mAllocator,
                               &sMark,
                               &sErrorStack ) == STL_SUCCESS );
    sState = 1;

    sRet = zllForeignKeys( aStatementHandle,
                           aPKCatalogName,
                           aNameLength1,
                           aPKSchemaName,
                           aNameLength2,
                           aPKTableName,
                           aNameLength3,
                           aFKCatalogName,
                           aNameLength4,
                           aFKSchemaName,
                           aNameLength5,
                           aFKTableName,
                           aNameLength6,
                           &sErrorStack );
    
    STL_TRY( sRet == SQL_SUCCESS );

    sState = 0;
    STL_TRY( stlRestoreRegionMem( &sStmt->mAllocator,
                                  &sMark,
                                  STL_TRUE,
                                  &sErrorStack ) == STL_SUCCESS );

    zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

    return SQL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_FINISH;

    if( ( sRet == SQL_ERROR ) ||
        ( sRet == SQL_SUCCESS_WITH_INFO ) )
    {
        zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_STMT,
                               aStatementHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }

        if( sStmt != NULL )
        {
            zlcCheckSessionAndCleanup( ZLS_STMT_DBC(sStmt) );
        }
    }

    switch( sState )
    {
        case 1 :
            (void)stlRestoreRegionMem( &sStmt->mAllocator, &sMark, STL_TRUE, &sErrorStack );
        default :
            break;
    }

    return sRet;
}

SQLRETURN SQL_API SQLForeignKeysW( SQLHSTMT      aStatementHandle,
                                   SQLWCHAR    * aPKCatalogName,
                                   SQLSMALLINT   aNameLength1,
                                   SQLWCHAR    * aPKSchemaName,
                                   SQLSMALLINT   aNameLength2,
                                   SQLWCHAR    * aPKTableName,
                                   SQLSMALLINT   aNameLength3,
                                   SQLWCHAR    * aFKCatalogName,
                                   SQLSMALLINT   aNameLength4,
                                   SQLWCHAR    * aFKSchemaName,
                                   SQLSMALLINT   aNameLength5,
                                   SQLWCHAR    * aFKTableName,
                                   SQLSMALLINT   aNameLength6 )
{
    /**
     * aNameLength1
     *     Length of *PKCatalogName, in characters.
     * aNameLength2
     *     Length of *PKSchemaName, in characters.
     * aNameLength3
     *     Length of *PKTableName, in characters.
     * aNameLength4
     *     Length of *FKCatalogName, in characters.
     * aNameLength5
     *     Length of *FKSchemaName, in characters.
     * aNameLength6
     *     Length of *FKTableName, in characters.
     */

    SQLRETURN       sRet  = SQL_ERROR;
    zlsStmt       * sStmt = NULL;
    stlErrorStack   sErrorStack;
    stlErrorData  * sErrorData = NULL;
    stlRegionMark   sMark;
    stlInt32        sState = 0;

    stlInt64   sBufferLength;
    SQLCHAR  * sPKCatalogName = NULL;
    stlInt64   sNameLength1   = 0;
    SQLCHAR  * sPKSchemaName  = NULL;
    stlInt64   sNameLength2   = 0;
    SQLCHAR  * sPKTableName   = NULL;
    stlInt64   sNameLength3   = 0;
    SQLCHAR  * sFKCatalogName = NULL;
    stlInt64   sNameLength4   = 0;
    SQLCHAR  * sFKSchemaName  = NULL;
    stlInt64   sNameLength5   = 0;
    SQLCHAR  * sFKTableName   = NULL;
    stlInt64   sNameLength6   = 0;
    
    STL_INIT_ERROR_STACK( &sErrorStack );

    sStmt = (zlsStmt*)aStatementHandle;

    STL_TRY_THROW( sStmt != NULL,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sStmt->mType == SQL_HANDLE_STMT,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( stlMarkRegionMem( &sStmt->mAllocator,
                               &sMark,
                               &sErrorStack ) == STL_SUCCESS );
    sState = 1;

    sBufferLength = 0;    
    STL_TRY( zlvAllocCharBuffer( aPKCatalogName,
                                 aNameLength1,
                                 ZLS_STMT_DBC(sStmt)->mCharacterSet,
                                 &sPKCatalogName,
                                 &sBufferLength,
                                 &sStmt->mAllocator,
                                 &sErrorStack ) == STL_SUCCESS );
    
    STL_TRY( zlvWcharToChar( aPKCatalogName,
                             aNameLength1,
                             ZLS_STMT_DBC(sStmt)->mCharacterSet,
                             sPKCatalogName,
                             sBufferLength,
                             &sNameLength1,
                             &sErrorStack ) == STL_SUCCESS );
    
    sBufferLength = 0;    
    STL_TRY( zlvAllocCharBuffer( aPKSchemaName,
                                 aNameLength2,
                                 ZLS_STMT_DBC(sStmt)->mCharacterSet,
                                 &sPKSchemaName,
                                 &sBufferLength,
                                 &sStmt->mAllocator,
                                 &sErrorStack ) == STL_SUCCESS );
    
    STL_TRY( zlvWcharToChar( aPKSchemaName,
                             aNameLength2,
                             ZLS_STMT_DBC(sStmt)->mCharacterSet,
                             sPKSchemaName,
                             sBufferLength,
                             &sNameLength2,
                             &sErrorStack ) == STL_SUCCESS );
    
    sBufferLength = 0;    
    STL_TRY( zlvAllocCharBuffer( aPKTableName,
                                 aNameLength3,
                                 ZLS_STMT_DBC(sStmt)->mCharacterSet,
                                 &sPKTableName,
                                 &sBufferLength,
                                 &sStmt->mAllocator,
                                 &sErrorStack ) == STL_SUCCESS );
    
    STL_TRY( zlvWcharToChar( aPKTableName,
                             aNameLength3,
                             ZLS_STMT_DBC(sStmt)->mCharacterSet,
                             sPKTableName,
                             sBufferLength,
                             &sNameLength3,
                             &sErrorStack ) == STL_SUCCESS );
    
    sBufferLength = 0;    
    STL_TRY( zlvAllocCharBuffer( aFKCatalogName,
                                 aNameLength4,
                                 ZLS_STMT_DBC(sStmt)->mCharacterSet,
                                 &sFKCatalogName,
                                 &sBufferLength,
                                 &sStmt->mAllocator,
                                 &sErrorStack ) == STL_SUCCESS );
    
    STL_TRY( zlvWcharToChar( aFKCatalogName,
                             aNameLength4,
                             ZLS_STMT_DBC(sStmt)->mCharacterSet,
                             sFKCatalogName,
                             sBufferLength,
                             &sNameLength4,
                             &sErrorStack ) == STL_SUCCESS );
    
    sBufferLength = 0;    
    STL_TRY( zlvAllocCharBuffer( aFKSchemaName,
                                 aNameLength5,
                                 ZLS_STMT_DBC(sStmt)->mCharacterSet,
                                 &sFKSchemaName,
                                 &sBufferLength,
                                 &sStmt->mAllocator,
                                 &sErrorStack ) == STL_SUCCESS );
    
    STL_TRY( zlvWcharToChar( aFKSchemaName,
                             aNameLength5,
                             ZLS_STMT_DBC(sStmt)->mCharacterSet,
                             sFKSchemaName,
                             sBufferLength,
                             &sNameLength5,
                             &sErrorStack ) == STL_SUCCESS );
    
    sBufferLength = 0;    
    STL_TRY( zlvAllocCharBuffer( aFKTableName,
                                 aNameLength6,
                                 ZLS_STMT_DBC(sStmt)->mCharacterSet,
                                 &sFKTableName,
                                 &sBufferLength,
                                 &sStmt->mAllocator,
                                 &sErrorStack ) == STL_SUCCESS );
    
    STL_TRY( zlvWcharToChar( aFKTableName,
                             aNameLength6,
                             ZLS_STMT_DBC(sStmt)->mCharacterSet,
                             sFKTableName,
                             sBufferLength,
                             &sNameLength6,
                             &sErrorStack ) == STL_SUCCESS );
    
    sRet = zllForeignKeys( aStatementHandle,
                           sPKCatalogName,
                           sNameLength1,
                           sPKSchemaName,
                           sNameLength2,
                           sPKTableName,
                           sNameLength3,
                           sFKCatalogName,
                           sNameLength4,
                           sFKSchemaName,
                           sNameLength5,
                           sFKTableName,
                           sNameLength6,
                           &sErrorStack );
    
    STL_TRY( sRet == SQL_SUCCESS );

    sState = 0;
    STL_TRY( stlRestoreRegionMem( &sStmt->mAllocator,
                                  &sMark,
                                  STL_TRUE,
                                  &sErrorStack ) == STL_SUCCESS );

    zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

    return SQL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_FINISH;

    if( ( sRet == SQL_ERROR ) ||
        ( sRet == SQL_SUCCESS_WITH_INFO ) )
    {
        zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_STMT,
                               aStatementHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }

        if( sStmt != NULL )
        {
            zlcCheckSessionAndCleanup( ZLS_STMT_DBC(sStmt) );
        }
    }

    switch( sState )
    {
        case 1 :
            (void)stlRestoreRegionMem( &sStmt->mAllocator, &sMark, STL_TRUE, &sErrorStack );
        default :
            break;
    }

    return sRet;
}

SQLRETURN SQL_API SQLFreeConnect( SQLHDBC aConnectionHandle )
{
    SQLRETURN       sRet        = SQL_ERROR;
    zlcDbc        * sDbc        = NULL;
    stlErrorData  * sErrorData  = NULL;
    stlErrorStack   sErrorStack;

    STL_INIT_ERROR_STACK( &sErrorStack );

    STL_TRY_THROW( aConnectionHandle != NULL, RAMP_ERR_INVALID_HANDLE );

    sDbc = (zlcDbc*)aConnectionHandle;

    STL_TRY_THROW( sDbc->mType == SQL_HANDLE_DBC,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( zldDiagClear( SQL_HANDLE_DBC,
                           aConnectionHandle,
                           &sErrorStack ) == STL_SUCCESS );

    switch( sDbc->mTransition )
    {
        case ZLC_TRANSITION_STATE_C0 :
        case ZLC_TRANSITION_STATE_C1 :
            STL_THROW( RAMP_ERR_INVALID_HANDLE );
            break;
        case ZLC_TRANSITION_STATE_C2 :
            break;
        case ZLC_TRANSITION_STATE_C3 :
        case ZLC_TRANSITION_STATE_C4 :
        case ZLC_TRANSITION_STATE_C5 :
        case ZLC_TRANSITION_STATE_C6 :
            STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR );
            break;
    }

    STL_TRY( zlcFree( sDbc, &sErrorStack ) == STL_SUCCESS );

    sRet = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      "The HandleType argument was SQL_HANDLE_DBC, and "
                      "the function was called before calling SQLDisconnect "
                      "for the connection.",
                      &sErrorStack );
    }

    STL_FINISH;

    if( sRet == SQL_ERROR )
    {
        zldDiagSetRetCode( SQL_HANDLE_DBC, aConnectionHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_DBC,
                               aConnectionHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }
    }

    return sRet;
}

SQLRETURN SQL_API SQLFreeEnv( SQLHENV aEnvironmentHandle )
{
    SQLRETURN       sRet        = SQL_ERROR;
    zlnEnv        * sEnv        = NULL;
    stlErrorData  * sErrorData  = NULL;
    stlErrorStack   sErrorStack;

    STL_INIT_ERROR_STACK( &sErrorStack );

    STL_TRY_THROW( aEnvironmentHandle != NULL, RAMP_ERR_INVALID_HANDLE );

    sEnv = (zlnEnv*)aEnvironmentHandle;

    STL_TRY_THROW( sEnv->mType == SQL_HANDLE_ENV,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( zldDiagClear( SQL_HANDLE_ENV,
                           aEnvironmentHandle,
                           &sErrorStack ) == STL_SUCCESS );

    STL_TRY_THROW( STL_RING_IS_EMPTY( &sEnv->mDbcRing ) == STL_TRUE,
                   RAMP_ERR_FUNCTION_SEQUENCE_ERROR );

    STL_TRY( zlnFree( sEnv, &sErrorStack ) == STL_SUCCESS );

    sRet = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      "The HandleType argument was SQL_HANDLE_ENV, "
                      "and at least one connection was in an allocated or connected state. "
                      "SQLDisconnect and SQLFreeHandle with a HandleType of SQL_HANDLE_DBC "
                      "must be called for each connection before calling SQLFreeHandle "
                      "with a HandleType of SQL_HANDLE_ENV.",
                      &sErrorStack );
    }

    STL_FINISH;

    if( sRet == SQL_ERROR )
    {
        zldDiagSetRetCode( SQL_HANDLE_ENV, aEnvironmentHandle, sRet );
        
        sErrorData = stlGetLastErrorData( &sErrorStack );
        
        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_ENV,
                               aEnvironmentHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }
    }

    return sRet;
}

SQLRETURN SQL_API SQLFreeHandle( SQLSMALLINT aHandleType,
                                 SQLHANDLE   aHandle )
{
    SQLRETURN       sRet        = SQL_ERROR;
    zlnEnv        * sEnv        = NULL;
    zlcDbc        * sDbc        = NULL;
    zlsStmt       * sStmt       = NULL;
    stlErrorData  * sErrorData  = NULL;
    stlErrorStack   sErrorStack;

    STL_INIT_ERROR_STACK( &sErrorStack );

    STL_TRY_THROW( aHandle != NULL, RAMP_ERR_INVALID_HANDLE );

    switch( aHandleType )
    {
        case SQL_HANDLE_ENV :
            sEnv = (zlnEnv*)aHandle;

            STL_TRY_THROW( sEnv->mType == SQL_HANDLE_ENV,
                           RAMP_ERR_INVALID_HANDLE );

            STL_TRY( zldDiagClear( SQL_HANDLE_ENV,
                                   aHandle,
                                   &sErrorStack ) == STL_SUCCESS );

            STL_TRY_THROW( STL_RING_IS_EMPTY( &sEnv->mDbcRing ) == STL_TRUE,
                           RAMP_ERR_FUNCTION_SEQUENCE_ERROR1 );

            STL_TRY( zlnFree( sEnv,
                              &sErrorStack ) == STL_SUCCESS );
            break;
        case SQL_HANDLE_DBC :
            sDbc = (zlcDbc*)aHandle;

            STL_TRY_THROW( sDbc->mType == SQL_HANDLE_DBC,
                           RAMP_ERR_INVALID_HANDLE );

            STL_TRY( zldDiagClear( SQL_HANDLE_DBC,
                                   aHandle,
                                   &sErrorStack ) == STL_SUCCESS );

            switch( sDbc->mTransition )
            {
                case ZLC_TRANSITION_STATE_C0 :
                case ZLC_TRANSITION_STATE_C1 :
                    STL_THROW( RAMP_ERR_INVALID_HANDLE );
                    break;
                case ZLC_TRANSITION_STATE_C2 :
                    break;
                case ZLC_TRANSITION_STATE_C3 :
                case ZLC_TRANSITION_STATE_C4 :
                case ZLC_TRANSITION_STATE_C5 :
                case ZLC_TRANSITION_STATE_C6 :
                    STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR2 );
                    break;
            }

            STL_TRY( zlcFree( sDbc,
                              &sErrorStack ) == STL_SUCCESS );
            break;
        case SQL_HANDLE_STMT :
            sStmt = (zlsStmt*)aHandle;

            STL_TRY_THROW( sStmt->mType == SQL_HANDLE_STMT,
                           RAMP_ERR_INVALID_HANDLE );

            sDbc = sStmt->mParentDbc;

            STL_TRY( zldDiagClear( SQL_HANDLE_STMT,
                                   aHandle,
                                   &sErrorStack ) == STL_SUCCESS );

            switch( sDbc->mTransition )
            {
                case ZLC_TRANSITION_STATE_C0 :
                case ZLC_TRANSITION_STATE_C1 :
                case ZLC_TRANSITION_STATE_C2 :
                case ZLC_TRANSITION_STATE_C3 :
                case ZLC_TRANSITION_STATE_C4 :
                    STL_THROW( RAMP_ERR_INVALID_HANDLE );
                    break;
                case ZLC_TRANSITION_STATE_C5 :
                case ZLC_TRANSITION_STATE_C6 :
                    break;
            }

            switch( sStmt->mTransition )
            {
                case ZLS_TRANSITION_STATE_S0 :
                    STL_THROW( RAMP_ERR_INVALID_HANDLE );
                    break;
                case ZLS_TRANSITION_STATE_S8 :
                case ZLS_TRANSITION_STATE_S9 :
                case ZLS_TRANSITION_STATE_S10 :
                case ZLS_TRANSITION_STATE_S11 :
                case ZLS_TRANSITION_STATE_S12 :
                    STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR3 );
                    break;
            }

            if( zlcCheckSession( sDbc, &sErrorStack ) == STL_SUCCESS )
            {
                STL_TRY( zlsFree( sStmt, STL_FALSE, &sErrorStack ) == STL_SUCCESS );
            }
            else
            {
                STL_TRY( zlsFree( sStmt, STL_TRUE, &sErrorStack ) == STL_SUCCESS );
            }

            switch( sDbc->mTransition )
            {
                case ZLC_TRANSITION_STATE_C5 :
                    if( STL_RING_IS_EMPTY( &sDbc->mStmtRing ) == STL_TRUE )
                    {
                        sDbc->mTransition = ZLC_TRANSITION_STATE_C4;
                    }
                    break;
                case ZLC_TRANSITION_STATE_C6 :
                    if( sDbc->mAttrAutoCommit == STL_TRUE )
                    {
                        if( STL_RING_IS_EMPTY( &sDbc->mStmtRing ) == STL_TRUE )
                        {
                            sDbc->mTransition = ZLC_TRANSITION_STATE_C4;
                        }
                        else
                        {
                            sDbc->mTransition = ZLC_TRANSITION_STATE_C5;
                        }
                    }
                    break;
            }
            break;
        case SQL_HANDLE_DESC :
        default :
            STL_THROW( RAMP_ERR_INVALID_ATTRIBUTE_OPTION_IDENTIFIER );
            break;
    }

    sRet = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_CATCH( RAMP_ERR_INVALID_ATTRIBUTE_OPTION_IDENTIFIER )
    {
        /**
         * @todo 에러를 어디에 어떻게 설정할 것인가?
         */
    }

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR1 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      "The HandleType argument was SQL_HANDLE_ENV, "
                      "and at least one connection was in an allocated or connected state. "
                      "SQLDisconnect and SQLFreeHandle with a HandleType of SQL_HANDLE_DBC "
                      "must be called for each connection before calling SQLFreeHandle "
                      "with a HandleType of SQL_HANDLE_ENV.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR2 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      "The HandleType argument was SQL_HANDLE_DBC, and "
                      "the function was called before calling SQLDisconnect "
                      "for the connection.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR3 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      NULL,
                      &sErrorStack );
    }

    STL_FINISH;

    if( sRet == SQL_ERROR )
    {
        sErrorData = stlGetLastErrorData( &sErrorStack );
        switch( aHandleType )
        {
            case SQL_HANDLE_ENV :
                zldDiagSetRetCode( SQL_HANDLE_ENV, aHandle, sRet );
                if( sErrorData != NULL )
                {
                    (void)zldDiagAdds( SQL_HANDLE_ENV,
                                       aHandle,
                                       SQL_NO_ROW_NUMBER,
                                       SQL_NO_COLUMN_NUMBER,
                                       &sErrorStack );
                }
                break;
            case SQL_HANDLE_DBC :
                zldDiagSetRetCode( SQL_HANDLE_DBC, aHandle, sRet );
                if( sErrorData != NULL )
                {
                    (void)zldDiagAdds( SQL_HANDLE_DBC,
                                       aHandle,
                                       SQL_NO_ROW_NUMBER,
                                       SQL_NO_COLUMN_NUMBER,
                                       &sErrorStack );
                }
                break;
            case SQL_HANDLE_STMT :
                zldDiagSetRetCode( SQL_HANDLE_STMT, aHandle, sRet );
                if( sErrorData != NULL )
                {
                    (void)zldDiagAdds( SQL_HANDLE_STMT,
                                       aHandle,
                                       SQL_NO_ROW_NUMBER,
                                       SQL_NO_COLUMN_NUMBER,
                                       &sErrorStack );
                }
                break;
        }
    }

    return sRet;
}

SQLRETURN SQL_API SQLFreeStmt( SQLHSTMT     aStatementHandle,
                               SQLUSMALLINT aOption )
{
    SQLRETURN       sRet        = SQL_ERROR;
    zlsStmt       * sStmt       = NULL;
    stlErrorData  * sErrorData  = NULL;
    stlErrorStack   sErrorStack;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sStmt = (zlsStmt*)aStatementHandle;

    STL_TRY_THROW( sStmt != NULL,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sStmt->mType == SQL_HANDLE_STMT,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( zldDiagClear( SQL_HANDLE_STMT,
                           aStatementHandle,
                           &sErrorStack ) == STL_SUCCESS );

    STL_TRY( zlcCheckSession( sStmt->mParentDbc,
                              &sErrorStack ) == STL_SUCCESS );
    
    switch( sStmt->mTransition )
    {
        case ZLS_TRANSITION_STATE_S0 :
            STL_THROW( RAMP_ERR_INVALID_HANDLE );
            break;
        case ZLS_TRANSITION_STATE_S8 :
        case ZLS_TRANSITION_STATE_S9 :
        case ZLS_TRANSITION_STATE_S10 :
        case ZLS_TRANSITION_STATE_S11 :
        case ZLS_TRANSITION_STATE_S12 :
            STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR );
            break;
        default :
            break;
    }

    switch( aOption )
    {
        case SQL_DROP :
            STL_TRY( zlsFree( sStmt,
                              STL_FALSE,
                              &sErrorStack ) == STL_SUCCESS );

            sRet = SQL_SUCCESS;
            break;
        case SQL_CLOSE :
        case SQL_UNBIND :
        case SQL_RESET_PARAMS :
            STL_TRY( zlsClear( sStmt,
                               aOption,
                               &sErrorStack ) == STL_SUCCESS );

            sRet = SQL_SUCCESS;

            zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );
            break;
        default :
            STL_THROW( RAMP_ERR_OPTION_TYPE_OUT_OF_RANGE );
            break;
    }

    return sRet;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      NULL,
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_OPTION_TYPE_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_OPTION_TYPE_OUT_OF_RANGE,
                      "The value specified for the argument Option was not: "
                      "SQL_CLOSE SQL_DROP SQL_UNBIND SQL_RESET_PARAMS.",
                      &sErrorStack );
    }

    STL_FINISH;

    if( sRet == SQL_ERROR )
    {
        zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_STMT,
                               aStatementHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }
    }

    return sRet;
}

static SQLRETURN zllGetConnectAttr( SQLHDBC         aConnectionHandle,
                                    SQLINTEGER      aAttribute,
                                    SQLPOINTER      aValuePtr,
                                    SQLINTEGER      aBufferLength,
                                    SQLINTEGER    * aStringLengthPtr,
                                    stlErrorStack * aErrorStack )
{
    zlcDbc        * sDbc          = NULL;
    SQLRETURN       sRet          = SQL_ERROR;
    stlInt32        sStringLength = 0;

    sDbc = (zlcDbc*)aConnectionHandle;

    STL_TRY( zldDiagClear( SQL_HANDLE_DBC,
                           aConnectionHandle,
                           aErrorStack ) == STL_SUCCESS );

    switch( sDbc->mTransition )
    {
        case ZLC_TRANSITION_STATE_C0 :
        case ZLC_TRANSITION_STATE_C1 :
            STL_THROW( RAMP_ERR_INVALID_HANDLE );
            break;
        case ZLC_TRANSITION_STATE_C2 :
            switch( aAttribute )
            {
                case SQL_ATTR_ACCESS_MODE :
                case SQL_ATTR_AUTOCOMMIT :
                case SQL_ATTR_CURRENT_CATALOG :
                case SQL_ATTR_LOGIN_TIMEOUT :
                case SQL_ATTR_ODBC_CURSORS :
                case SQL_ATTR_TRACE :
                case SQL_ATTR_TRACEFILE :
                case SQL_ATTR_TIMEZONE:
                case SQL_ATTR_CHARACTER_SET :
                case SQL_ATTR_DATE_FORMAT :
                case SQL_ATTR_TIME_FORMAT :
                case SQL_ATTR_TIME_WITH_TIMEZONE_FORMAT :
                case SQL_ATTR_TIMESTAMP_FORMAT :
                case SQL_ATTR_TIMESTAMP_WITH_TIMEZONE_FORMAT :
                    break;
                default :
                    STL_THROW( RAMP_ERR_CONNECTION_NOT_OPEN );
                    break;
            }
            break;
        case ZLC_TRANSITION_STATE_C3 :
            STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR );
            break;
        case ZLC_TRANSITION_STATE_C4 :
        case ZLC_TRANSITION_STATE_C5 :
        case ZLC_TRANSITION_STATE_C6 :
            STL_TRY( zlcCheckSession( sDbc, aErrorStack ) == STL_SUCCESS );
            break;
    }

    switch( aAttribute )
    {
        case SQL_ATTR_ACCESS_MODE :
        case SQL_ATTR_AUTOCOMMIT :
        case SQL_ATTR_CONNECTION_DEAD :
        case SQL_ATTR_CURRENT_CATALOG :
        case SQL_ATTR_LOGIN_TIMEOUT :
        case SQL_ATTR_MAX_ROWS :
        case SQL_ATTR_METADATA_ID :
        case SQL_ATTR_QUERY_TIMEOUT :
        case SQL_ATTR_TXN_ISOLATION :
        case SQL_ATTR_TIMEZONE:
        case SQL_ATTR_CHARACTER_SET :
        case SQL_ATTR_DATABASE_CHARACTER_SET :
        case SQL_ATTR_DATE_FORMAT :
        case SQL_ATTR_TIME_FORMAT :
        case SQL_ATTR_TIME_WITH_TIMEZONE_FORMAT :
        case SQL_ATTR_TIMESTAMP_FORMAT :
        case SQL_ATTR_TIMESTAMP_WITH_TIMEZONE_FORMAT :
        case SQL_ATTR_PROTOCOL :
            break;
        case SQL_ATTR_ASYNC_DBC_FUNCTIONS_ENABLE :
        case SQL_ATTR_ASYNC_ENABLE :
        case SQL_ATTR_AUTO_IPD :
        case SQL_ATTR_CONNECTION_TIMEOUT :
        case SQL_ATTR_ENLIST_IN_DTC :
        case SQL_ATTR_ODBC_CURSORS :
        case SQL_ATTR_PACKET_SIZE :
        case SQL_ATTR_QUIET_MODE :
        case SQL_ATTR_TRACE :
        case SQL_ATTR_TRACEFILE :
        case SQL_ATTR_TRANSLATE_LIB :
        case SQL_ATTR_TRANSLATE_OPTION :
            STL_THROW( RAMP_ERR_OPTIONAL_FEATURE_NOT_IMPLEMENTED );
            break;
        default:
            STL_THROW( RAMP_ERR_INVALID_ATTRIBUTE_OPTION_IDENTIFIER );
            break;
    }

    switch( aAttribute )
    {
        case SQL_ATTR_CURRENT_CATALOG :
        case SQL_ATTR_TIMEZONE :
        case SQL_ATTR_CHARACTER_SET :
        case SQL_ATTR_DATABASE_CHARACTER_SET :
        case SQL_ATTR_DATE_FORMAT :
        case SQL_ATTR_TIME_FORMAT :
        case SQL_ATTR_TIME_WITH_TIMEZONE_FORMAT :
        case SQL_ATTR_TIMESTAMP_FORMAT :
        case SQL_ATTR_TIMESTAMP_WITH_TIMEZONE_FORMAT :
            STL_TRY_THROW( aBufferLength >= 0, RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH );
            break;
        default :
            break;
    }
    
    STL_TRY( zlcGetAttr( sDbc,
                         aAttribute,
                         aValuePtr,
                         aBufferLength,
                         &sStringLength,
                         aErrorStack ) == STL_SUCCESS );

    switch( aAttribute )
    {
        case SQL_ATTR_CURRENT_CATALOG :
        case SQL_ATTR_TIMEZONE :
        case SQL_ATTR_CHARACTER_SET :
        case SQL_ATTR_DATABASE_CHARACTER_SET :
        case SQL_ATTR_DATE_FORMAT :
        case SQL_ATTR_TIME_FORMAT :
        case SQL_ATTR_TIME_WITH_TIMEZONE_FORMAT :
        case SQL_ATTR_TIMESTAMP_FORMAT :
        case SQL_ATTR_TIMESTAMP_WITH_TIMEZONE_FORMAT :
            if( aStringLengthPtr != NULL )
            {
                *aStringLengthPtr = sStringLength;
            }

            STL_TRY_THROW( aBufferLength > sStringLength,
                           RAMP_WARNING_STRING_DATA_RIGHT_TRUNCATED );
            break;
        default :
            break;
    }

    sRet = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_ERR_CONNECTION_NOT_OPEN )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_CONNECTION_NOT_OPEN,
                      "An Attribute value that required an open connection was specified.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_ATTRIBUTE_OPTION_IDENTIFIER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_ATTRIBUTE_OPTION_IDENTIFIER,
                      "The value specified for the argument Attribute was not "
                      "valid for the version of ODBC supported by the driver.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_OPTIONAL_FEATURE_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_OPTIONAL_FEATURE_NOT_IMPLEMENTED,
                      "The value specified for the argument Attribute was a valid "
                      "ODBC connection or statement attribute for the version of "
                      "ODBC supported by the driver but was not supported by the driver.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_STRING_OR_BUFFER_LENGTH,
                      "*ValuePtr is a character string, and BufferLength was less than zero.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_WARNING_STRING_DATA_RIGHT_TRUNCATED )
    {
        stlPushError( STL_ERROR_LEVEL_WARNING,
                      ZLE_ERRCODE_WARNING_STRING_DATA_RIGHT_TRUNCATED,
                      "The data returned in *ValuePtr was truncated to be BufferLength minus "
                      "the length of a null-termination character. "
                      "The length of the untruncated string value is returned in *StringLengthPtr.",
                      aErrorStack );

        sRet = SQL_SUCCESS_WITH_INFO;
    }

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_FINISH;

    return sRet;
}

SQLRETURN SQL_API SQLGetConnectAttr( SQLHDBC      aConnectionHandle,
                                     SQLINTEGER   aAttribute,
                                     SQLPOINTER   aValuePtr,
                                     SQLINTEGER   aBufferLength,
                                     SQLINTEGER * aStringLengthPtr )
{
    SQLRETURN       sRet = SQL_ERROR;
    zlcDbc        * sDbc = NULL;
    stlErrorStack   sErrorStack;
    stlErrorData  * sErrorData = NULL;
    stlRegionMark   sMark;
    stlInt32        sState = 0;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sDbc = (zlcDbc*)aConnectionHandle;
    
    STL_TRY_THROW( sDbc != NULL, RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sDbc->mType == SQL_HANDLE_DBC,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( stlMarkRegionMem( &sDbc->mAllocator,
                               &sMark,
                               &sErrorStack ) == STL_SUCCESS );
    sState = 1;

    sRet = zllGetConnectAttr( aConnectionHandle,
                              aAttribute,
                              aValuePtr,
                              aBufferLength,
                              aStringLengthPtr,
                              &sErrorStack );

    STL_TRY( sRet == SQL_SUCCESS );

    sState = 0;
    STL_TRY( stlRestoreRegionMem( &sDbc->mAllocator,
                                  &sMark,
                                  STL_TRUE,
                                  &sErrorStack ) == STL_SUCCESS );

    zldDiagSetRetCode( SQL_HANDLE_DBC, aConnectionHandle, sRet );

    return SQL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_FINISH;

    if( ( sRet == SQL_ERROR ) ||
        ( sRet == SQL_SUCCESS_WITH_INFO ) )
    {
        zldDiagSetRetCode( SQL_HANDLE_DBC, aConnectionHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_DBC,
                               aConnectionHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }
        
        zlcCheckSessionAndCleanup( sDbc );
    }

    switch( sState )
    {
        case 1 :
            (void)stlRestoreRegionMem( &sDbc->mAllocator, &sMark, STL_TRUE, &sErrorStack );
        default :
            break;
    }

    return sRet;
}

SQLRETURN SQL_API SQLGetConnectAttrW( SQLHDBC      aConnectionHandle,
                                      SQLINTEGER   aAttribute,
                                      SQLPOINTER   aValuePtr,
                                      SQLINTEGER   aBufferLength,
                                      SQLINTEGER * aStringLengthPtr )
{
    /**
     * aBufferLength
     *     BufferLength should contain a count of bytes.
     * aStringLengthPtr
     *     A pointer to a buffer in which to return the total number of bytes
     *     (excluding the null-termination character) available to return
     *     in *ValuePtr.
     */

    SQLRETURN       sRet = SQL_ERROR;
    zlcDbc        * sDbc = NULL;
    stlErrorStack   sErrorStack;
    stlErrorData  * sErrorData = NULL;
    stlRegionMark   sMark;
    stlInt32        sState = 0;

    SQLCHAR         sCharacterAttribute[SQL_MAX_OPTION_STRING_LENGTH + 1];
    SQLINTEGER      sStringLength = 0;
    stlInt64        sBufferLength = 0;
    stlInt64        sLength = 0;
    
    STL_INIT_ERROR_STACK( &sErrorStack );
    
    sDbc = (zlcDbc*)aConnectionHandle;
    
    STL_TRY_THROW( sDbc != NULL, RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sDbc->mType == SQL_HANDLE_DBC,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( stlMarkRegionMem( &sDbc->mAllocator,
                               &sMark,
                               &sErrorStack ) == STL_SUCCESS );
    sState = 1;

    switch( aAttribute )
    {
        case SQL_ATTR_CURRENT_CATALOG :
        case SQL_ATTR_TIMEZONE :
        case SQL_ATTR_CHARACTER_SET :
        case SQL_ATTR_DATABASE_CHARACTER_SET :
        case SQL_ATTR_DATE_FORMAT :
        case SQL_ATTR_TIME_FORMAT :
        case SQL_ATTR_TIME_WITH_TIMEZONE_FORMAT :
        case SQL_ATTR_TIMESTAMP_FORMAT :
        case SQL_ATTR_TIMESTAMP_WITH_TIMEZONE_FORMAT :
            STL_TRY_THROW( aBufferLength >= 0, RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH );

            sRet = zllGetConnectAttr( aConnectionHandle,
                                      aAttribute,
                                      (SQLPOINTER)sCharacterAttribute,
                                      STL_SIZEOF(sCharacterAttribute),
                                      &sStringLength,
                                      &sErrorStack );

            STL_TRY( SQL_SUCCEEDED(sRet) );

            sBufferLength = aBufferLength / STL_SIZEOF(SQLWCHAR);

            STL_TRY( zlvCharToWchar( sDbc->mCharacterSet,
                                     sCharacterAttribute,
                                     sStringLength,
                                     (SQLWCHAR*)aValuePtr,
                                     sBufferLength,
                                     &sLength,
                                     &sErrorStack ) == STL_SUCCESS );

            if( aStringLengthPtr != NULL )
            {
                *aStringLengthPtr = sLength * STL_SIZEOF(SQLWCHAR);
            }

            STL_TRY_THROW( sLength < sBufferLength, RAMP_WARNING_STRING_DATA_RIGHT_TRUNCATED );
            break;
        default :
            sRet = zllGetConnectAttr( aConnectionHandle,
                                      aAttribute,
                                      aValuePtr,
                                      aBufferLength,
                                      aStringLengthPtr,
                                      &sErrorStack );
            break;
    }

    STL_TRY( SQL_SUCCEEDED(sRet) );

    sState = 0;
    STL_TRY( stlRestoreRegionMem( &sDbc->mAllocator,
                                  &sMark,
                                  STL_TRUE,
                                  &sErrorStack ) == STL_SUCCESS );

    zldDiagSetRetCode( SQL_HANDLE_DBC, aConnectionHandle, sRet );

    return SQL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_CATCH( RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_STRING_OR_BUFFER_LENGTH,
                      "*ValuePtr is a character string, and BufferLength was less than zero.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_WARNING_STRING_DATA_RIGHT_TRUNCATED )
    {
        stlPushError( STL_ERROR_LEVEL_WARNING,
                      ZLE_ERRCODE_WARNING_STRING_DATA_RIGHT_TRUNCATED,
                      "The data returned in *ValuePtr was truncated to be BufferLength minus "
                      "the length of a null-termination character. "
                      "The length of the untruncated string value is returned in *StringLengthPtr.",
                      &sErrorStack );

        sRet = SQL_SUCCESS_WITH_INFO;
    }

    STL_FINISH;

    if( ( sRet == SQL_ERROR ) ||
        ( sRet == SQL_SUCCESS_WITH_INFO ) )
    {
        zldDiagSetRetCode( SQL_HANDLE_DBC, aConnectionHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_DBC,
                               aConnectionHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }
    }

    switch( sState )
    {
        case 1 :
            (void)stlRestoreRegionMem( &sDbc->mAllocator, &sMark, STL_TRUE, &sErrorStack );
        default :
            break;
    }

    return sRet;
}

SQLRETURN SQL_API SQLGetConnectOption( SQLHDBC      aConnectionHandle,
                                       SQLUSMALLINT aOption,
                                       SQLPOINTER   aValue)
{
    SQLRETURN       sRet = SQL_ERROR;
    zlcDbc        * sDbc = NULL;
    stlErrorStack   sErrorStack;
    stlErrorData  * sErrorData = NULL;
    stlRegionMark   sMark;
    stlInt32        sState = 0;
    SQLINTEGER      sStringLength = 0;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sDbc = (zlcDbc*)aConnectionHandle;
    
    STL_TRY_THROW( sDbc != NULL, RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sDbc->mType == SQL_HANDLE_DBC,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( stlMarkRegionMem( &sDbc->mAllocator,
                               &sMark,
                               &sErrorStack ) == STL_SUCCESS );
    sState = 1;

    sRet = zllGetConnectAttr( aConnectionHandle,
                              aOption,
                              aValue,
                              SQL_MAX_OPTION_STRING_LENGTH,
                              &sStringLength,
                              &sErrorStack );

    STL_TRY( sRet == SQL_SUCCESS );

    sState = 0;
    STL_TRY( stlRestoreRegionMem( &sDbc->mAllocator,
                                  &sMark,
                                  STL_TRUE,
                                  &sErrorStack ) == STL_SUCCESS );

    zldDiagSetRetCode( SQL_HANDLE_DBC, aConnectionHandle, sRet );

    return SQL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_FINISH;

    if( ( sRet == SQL_ERROR ) ||
        ( sRet == SQL_SUCCESS_WITH_INFO ) )
    {
        zldDiagSetRetCode( SQL_HANDLE_DBC, aConnectionHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_DBC,
                               aConnectionHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }
        
        zlcCheckSessionAndCleanup( sDbc );
    }

    switch( sState )
    {
        case 1 :
            (void)stlRestoreRegionMem( &sDbc->mAllocator, &sMark, STL_TRUE, &sErrorStack );
        default :
            break;
    }

    return sRet;
}

SQLRETURN SQL_API SQLGetConnectOptionW( SQLHDBC      aConnectionHandle,
                                        SQLUSMALLINT aOption,
                                        SQLPOINTER   aValue)
{
    SQLRETURN       sRet = SQL_ERROR;
    zlcDbc        * sDbc = NULL;
    stlErrorStack   sErrorStack;
    stlErrorData  * sErrorData = NULL;
    stlRegionMark   sMark;
    stlInt32        sState = 0;

    SQLCHAR         sCharacterAttribute[SQL_MAX_OPTION_STRING_LENGTH + 1];
    SQLINTEGER      sStringLength = 0;
    stlInt64        sBufferLength = 0;
    stlInt64        sLength = 0;
    
    STL_INIT_ERROR_STACK( &sErrorStack );
    
    sDbc = (zlcDbc*)aConnectionHandle;
    
    STL_TRY_THROW( sDbc != NULL, RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sDbc->mType == SQL_HANDLE_DBC,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( stlMarkRegionMem( &sDbc->mAllocator,
                               &sMark,
                               &sErrorStack ) == STL_SUCCESS );
    sState = 1;

    switch( aOption )
    {
        case SQL_ATTR_CURRENT_CATALOG :
        case SQL_ATTR_TIMEZONE :
        case SQL_ATTR_CHARACTER_SET :
        case SQL_ATTR_DATABASE_CHARACTER_SET :
        case SQL_ATTR_DATE_FORMAT :
        case SQL_ATTR_TIME_FORMAT :
        case SQL_ATTR_TIME_WITH_TIMEZONE_FORMAT :
        case SQL_ATTR_TIMESTAMP_FORMAT :
        case SQL_ATTR_TIMESTAMP_WITH_TIMEZONE_FORMAT :
            sRet = zllGetConnectAttr( aConnectionHandle,
                                      aOption,
                                      (SQLPOINTER)sCharacterAttribute,
                                      STL_SIZEOF(sCharacterAttribute),
                                      &sStringLength,
                                      &sErrorStack );

            STL_TRY( SQL_SUCCEEDED(sRet) );

            sBufferLength = SQL_MAX_OPTION_STRING_LENGTH / STL_SIZEOF(SQLWCHAR);

            STL_TRY( zlvCharToWchar( sDbc->mCharacterSet,
                                     sCharacterAttribute,
                                     sStringLength,
                                     (SQLWCHAR*)aValue,
                                     sBufferLength,
                                     &sLength,
                                     &sErrorStack ) == STL_SUCCESS );

            STL_TRY_THROW( sLength * STL_SIZEOF(SQLWCHAR) < SQL_MAX_OPTION_STRING_LENGTH,
                           RAMP_WARNING_STRING_DATA_RIGHT_TRUNCATED );
            break;
        default :
            sRet = zllGetConnectAttr( aConnectionHandle,
                                      aOption,
                                      aValue,
                                      0,
                                      NULL,
                                      &sErrorStack );
            break;
    }

    STL_TRY( SQL_SUCCEEDED(sRet) );

    sState = 0;
    STL_TRY( stlRestoreRegionMem( &sDbc->mAllocator,
                                  &sMark,
                                  STL_TRUE,
                                  &sErrorStack ) == STL_SUCCESS );

    zldDiagSetRetCode( SQL_HANDLE_DBC, aConnectionHandle, sRet );

    return SQL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_CATCH( RAMP_WARNING_STRING_DATA_RIGHT_TRUNCATED )
    {
        stlPushError( STL_ERROR_LEVEL_WARNING,
                      ZLE_ERRCODE_WARNING_STRING_DATA_RIGHT_TRUNCATED,
                      "The data returned in *ValuePtr was truncated to be BufferLength minus "
                      "the length of a null-termination character. "
                      "The length of the untruncated string value is returned in *StringLengthPtr.",
                      &sErrorStack );

        sRet = SQL_SUCCESS_WITH_INFO;
    }

    STL_FINISH;

    if( ( sRet == SQL_ERROR ) ||
        ( sRet == SQL_SUCCESS_WITH_INFO ) )
    {
        zldDiagSetRetCode( SQL_HANDLE_DBC, aConnectionHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_DBC,
                               aConnectionHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }
    }

    switch( sState )
    {
        case 1 :
            (void)stlRestoreRegionMem( &sDbc->mAllocator, &sMark, STL_TRUE, &sErrorStack );
        default :
            break;
    }

    return sRet;
}

static SQLRETURN zllGetCursorName( SQLHSTMT        aStatementHandle,
                                   SQLCHAR       * aCursorName,
                                   SQLSMALLINT     aBufferLength,
                                   SQLSMALLINT   * aNameLengthPtr,
                                   stlErrorStack * aErrorStack )
{
    SQLRETURN       sRet        = SQL_ERROR;
    zlsStmt       * sStmt       = NULL;
    stlInt16        sNameLength = 0;

    sStmt = (zlsStmt*)aStatementHandle;

    STL_TRY( zldDiagClear( SQL_HANDLE_STMT,
                           aStatementHandle,
                           aErrorStack ) == STL_SUCCESS );

    STL_TRY( zlcCheckSession( sStmt->mParentDbc,
                              aErrorStack ) == STL_SUCCESS );
    
    switch( sStmt->mTransition )
    {
        case ZLS_TRANSITION_STATE_S0 :
            STL_THROW( RAMP_ERR_INVALID_HANDLE );
            break;
        case ZLS_TRANSITION_STATE_S1 :
        case ZLS_TRANSITION_STATE_S2 :
        case ZLS_TRANSITION_STATE_S3 :
        case ZLS_TRANSITION_STATE_S4 :
        case ZLS_TRANSITION_STATE_S5 :
        case ZLS_TRANSITION_STATE_S6 :
        case ZLS_TRANSITION_STATE_S7 :
            break;
        case ZLS_TRANSITION_STATE_S8 :
        case ZLS_TRANSITION_STATE_S9 :
        case ZLS_TRANSITION_STATE_S10 :
        case ZLS_TRANSITION_STATE_S11 :
        case ZLS_TRANSITION_STATE_S12 :
            STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR );
            break;
    }

    STL_TRY_THROW( aBufferLength >= 0, RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH );

    STL_TRY( zlrGetCursorName( sStmt,
                               (stlChar*)aCursorName,
                               aBufferLength,
                               &sNameLength,
                               aErrorStack ) == STL_SUCCESS );

    if( aNameLengthPtr != NULL )
    {
        *aNameLengthPtr = sNameLength;
    }

    STL_TRY_THROW( aBufferLength > sNameLength, RAMP_SUCCESS_WITH_INFO );

    sRet = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_CATCH( RAMP_SUCCESS_WITH_INFO )
    {
        sRet = SQL_SUCCESS_WITH_INFO;

        stlPushError( STL_ERROR_LEVEL_WARNING,
                      ZLE_ERRCODE_WARNING_STRING_DATA_RIGHT_TRUNCATED,
                      "The buffer *CursorName was not large enough to return "
                      "the entire cursor name, so the cursor name was truncated. "
                      "The length of the untruncated cursor name is returned in "
                      "*NameLengthPtr.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_STRING_OR_BUFFER_LENGTH,
                      "The value specified for the argument BufferLength was less than 0.",
                      aErrorStack );
    }

    STL_FINISH;

    return sRet;
}

SQLRETURN SQL_API SQLGetCursorName( SQLHSTMT      aStatementHandle,
                                    SQLCHAR     * aCursorName,
                                    SQLSMALLINT   aBufferLength,
                                    SQLSMALLINT * aNameLengthPtr )
{
    SQLRETURN       sRet  = SQL_ERROR;
    zlsStmt       * sStmt = NULL;
    stlErrorStack   sErrorStack;
    stlErrorData  * sErrorData = NULL;
    stlRegionMark   sMark;
    stlInt32        sState = 0;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sStmt = (zlsStmt*)aStatementHandle;

    STL_TRY_THROW( sStmt != NULL,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sStmt->mType == SQL_HANDLE_STMT,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( stlMarkRegionMem( &sStmt->mAllocator,
                               &sMark,
                               &sErrorStack ) == STL_SUCCESS );
    sState = 1;

    sRet = zllGetCursorName( aStatementHandle,
                             aCursorName,
                             aBufferLength,
                             aNameLengthPtr,
                             &sErrorStack );

    STL_TRY( sRet == SQL_SUCCESS );

    sState = 0;
    STL_TRY( stlRestoreRegionMem( &sStmt->mAllocator,
                                  &sMark,
                                  STL_TRUE,
                                  &sErrorStack ) == STL_SUCCESS );

    zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

    return SQL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_FINISH;

    if( ( sRet == SQL_ERROR ) ||
        ( sRet == SQL_SUCCESS_WITH_INFO ) )
    {
        zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_STMT,
                               aStatementHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }

        if( sStmt != NULL )
        {
            zlcCheckSessionAndCleanup( ZLS_STMT_DBC(sStmt) );
        }
    }

    switch( sState )
    {
        case 1 :
            stlRestoreRegionMem( &sStmt->mAllocator, &sMark, STL_TRUE, &sErrorStack );
        default :
            break;
    }

    return sRet;
}

SQLRETURN SQL_API SQLGetCursorNameW( SQLHSTMT      aStatementHandle,
                                     SQLWCHAR    * aCursorName,
                                     SQLSMALLINT   aBufferLength,
                                     SQLSMALLINT * aNameLengthPtr )
{
    /**
     * aBufferLength
     *     Length of *CursorName, in characters.
     *     (Note that this means this argument does NOT have to be
     *     an even number.)
     * aNameLengthPtr
     *     Pointer to memory in which to return the total number of characters
     *     (excluding the null-termination character) available to return
     *     in *CursorName.
     */

    SQLRETURN       sRet  = SQL_ERROR;
    zlsStmt       * sStmt = NULL;
    stlErrorStack   sErrorStack;
    stlErrorData  * sErrorData = NULL;
    stlRegionMark   sMark;
    stlInt32        sState = 0;

    SQLCHAR         sCursorName[STL_MAX_SQL_IDENTIFIER_LENGTH + 1];
    SQLSMALLINT     sNameLength = 0;
    stlInt64        sLength = 0;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sStmt = (zlsStmt*)aStatementHandle;

    STL_TRY_THROW( sStmt != NULL,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sStmt->mType == SQL_HANDLE_STMT,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( stlMarkRegionMem( &sStmt->mAllocator,
                               &sMark,
                               &sErrorStack ) == STL_SUCCESS );
    sState = 1;

    STL_TRY_THROW( aBufferLength >= 0, RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH );
    
    sRet = zllGetCursorName( aStatementHandle,
                             sCursorName,
                             STL_SIZEOF(sCursorName),
                             &sNameLength,
                             &sErrorStack );

    STL_TRY( SQL_SUCCEEDED(sRet) );

    STL_TRY( zlvCharToWchar( ZLS_STMT_DBC(sStmt)->mCharacterSet,
                             sCursorName,
                             sNameLength,
                             aCursorName,
                             aBufferLength,
                             &sLength,
                             &sErrorStack ) == STL_SUCCESS );

    if( aNameLengthPtr != NULL )
    {
        *aNameLengthPtr = sLength;
    }

    STL_TRY_THROW( sLength < aBufferLength, RAMP_SUCCESS_WITH_INFO );

    STL_TRY( sRet == SQL_SUCCESS );

    sState = 0;
    STL_TRY( stlRestoreRegionMem( &sStmt->mAllocator,
                                  &sMark,
                                  STL_TRUE,
                                  &sErrorStack ) == STL_SUCCESS );

    zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

    return SQL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_CATCH( RAMP_SUCCESS_WITH_INFO )
    {
        sRet = SQL_SUCCESS_WITH_INFO;

        stlPushError( STL_ERROR_LEVEL_WARNING,
                      ZLE_ERRCODE_WARNING_STRING_DATA_RIGHT_TRUNCATED,
                      "The buffer *CursorName was not large enough to return "
                      "the entire cursor name, so the cursor name was truncated. "
                      "The length of the untruncated cursor name is returned in "
                      "*NameLengthPtr.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_STRING_OR_BUFFER_LENGTH,
                      "The value specified for the argument BufferLength was less than 0.",
                      &sErrorStack );
    }

    STL_FINISH;

    if( ( sRet == SQL_ERROR ) ||
        ( sRet == SQL_SUCCESS_WITH_INFO ) )
    {
        zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_STMT,
                               aStatementHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }

        if( sStmt != NULL )
        {
            zlcCheckSessionAndCleanup( ZLS_STMT_DBC(sStmt) );
        }
    }

    switch( sState )
    {
        case 1 :
            (void)stlRestoreRegionMem( &sStmt->mAllocator, &sMark, STL_TRUE, &sErrorStack );
        default :
            break;
    }

    return sRet;
}

SQLRETURN SQL_API SQLGetData( SQLHSTMT       aStatementHandle,
                              SQLUSMALLINT   aCol_or_Param_Num,
                              SQLSMALLINT    aTargetType,
                              SQLPOINTER     aTargetValuePtr,
                              SQLLEN         aBufferLength,
                              SQLLEN       * aStrLen_or_IndPtr )
{
    SQLRETURN       sRet           = SQL_ERROR;
    zlsStmt       * sStmt          = NULL;
    stlErrorData  * sErrorData     = NULL;
    stlErrorStack   sErrorStack;
    SQLRETURN       sGetDataReturn = SQL_ERROR;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sStmt = (zlsStmt*)aStatementHandle;

    STL_TRY_THROW( sStmt != NULL,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sStmt->mType == SQL_HANDLE_STMT,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( zldDiagClear( SQL_HANDLE_STMT,
                           aStatementHandle,
                           &sErrorStack ) == STL_SUCCESS );

    STL_TRY( zlcCheckSession( sStmt->mParentDbc,
                              &sErrorStack ) == STL_SUCCESS );
    
    switch( sStmt->mTransition )
    {
        case ZLS_TRANSITION_STATE_S0 :
            STL_THROW( RAMP_ERR_INVALID_HANDLE );
            break;
        case ZLS_TRANSITION_STATE_S1 :
        case ZLS_TRANSITION_STATE_S2 :
        case ZLS_TRANSITION_STATE_S3 :
            STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR1 );
            break;
        case ZLS_TRANSITION_STATE_S4 :
            STL_THROW( RAMP_ERR_INVALID_CURSOR_STATE1 );
            break;
        case ZLS_TRANSITION_STATE_S5 :
            STL_THROW( RAMP_ERR_INVALID_CURSOR_STATE2 );
            break;
        case ZLS_TRANSITION_STATE_S8 :
        case ZLS_TRANSITION_STATE_S9 :
        case ZLS_TRANSITION_STATE_S10 :
        case ZLS_TRANSITION_STATE_S11 :
        case ZLS_TRANSITION_STATE_S12 :
            STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR2 );
            break;
        default :
            break;
    }

    switch( aTargetType )
    {
        case SQL_C_CHAR :
        case SQL_C_SSHORT :
        case SQL_C_USHORT :
        case SQL_C_SHORT :
        case SQL_C_SLONG :
        case SQL_C_ULONG :
        case SQL_C_LONG :
        case SQL_C_FLOAT :
        case SQL_C_DOUBLE :
        case SQL_C_BIT :
        case SQL_C_STINYINT :
        case SQL_C_UTINYINT :
        case SQL_C_TINYINT :
        case SQL_C_SBIGINT :
        case SQL_C_UBIGINT :
        case SQL_C_BINARY :
        case SQL_C_TYPE_DATE :
        case SQL_C_TYPE_TIME :
        case SQL_C_TYPE_TIMESTAMP :
        case SQL_C_INTERVAL_YEAR :
        case SQL_C_INTERVAL_MONTH :
        case SQL_C_INTERVAL_DAY :
        case SQL_C_INTERVAL_HOUR :
        case SQL_C_INTERVAL_MINUTE :
        case SQL_C_INTERVAL_SECOND :
        case SQL_C_INTERVAL_YEAR_TO_MONTH :
        case SQL_C_INTERVAL_DAY_TO_HOUR :
        case SQL_C_INTERVAL_DAY_TO_MINUTE :
        case SQL_C_INTERVAL_DAY_TO_SECOND :
        case SQL_C_INTERVAL_HOUR_TO_MINUTE :
        case SQL_C_INTERVAL_HOUR_TO_SECOND :
        case SQL_C_INTERVAL_MINUTE_TO_SECOND :
        case SQL_C_NUMERIC :
        case SQL_C_DEFAULT :
        case SQL_ARD_TYPE :
        case SQL_C_WCHAR :
            /*
             * Driver C datatype
             */
        case SQL_C_BOOLEAN :
        case SQL_C_LONGVARCHAR :
        case SQL_C_LONGVARBINARY :
        case SQL_C_TYPE_TIME_WITH_TIMEZONE :
        case SQL_C_TYPE_TIMESTAMP_WITH_TIMEZONE :
            /*
              case SQL_APD_TYPE :
            */
            break;
        case SQL_C_GUID :
            STL_THROW( RAMP_ERR_OPTIONAL_FEATURE_NOT_IMPLEMENTED );
            
        default :
            STL_THROW( RAMP_ERR_PROGRAM_TYPE_OUT_OF_RANGE );
            break;
    }

    STL_TRY_THROW( aTargetValuePtr != NULL, RAMP_ERR_INVALID_USE_OF_NULL_POINTER );

    STL_TRY_THROW( aBufferLength >= 0,
                   RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH );

    STL_TRY( zlrGetData( aStatementHandle,
                         aCol_or_Param_Num,
                         aTargetType,
                         aTargetValuePtr,
                         aBufferLength,
                         aStrLen_or_IndPtr,
                         &sGetDataReturn,
                         &sErrorStack ) == STL_SUCCESS );

    switch( sGetDataReturn )
    {
        case SQL_SUCCESS:
            break;
        case SQL_SUCCESS_WITH_INFO:
            STL_THROW( RAMP_SUCCESS_WITH_INFO );
            break;
        case SQL_NO_DATA:
            STL_THROW( RAMP_NO_DATA );
            break;
        case SQL_ERROR:
            STL_THROW( RAMP_ERROR );
            break;
        default:
            STL_ASSERT( STL_FALSE );
            break;
            
    }

    sRet = SQL_SUCCESS;

    zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

    return sRet;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_CATCH( RAMP_NO_DATA )
    {
        sRet = SQL_NO_DATA;
    }

    STL_CATCH( RAMP_SUCCESS_WITH_INFO )
    {
        sRet = SQL_SUCCESS_WITH_INFO;
    }

    STL_CATCH( RAMP_ERROR )
    {
        sRet = SQL_ERROR;
    }

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR1 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      "The specified StatementHandle was not in an executed state. "
                      "The function was called without first calling "
                      "SQLExecDirect, SQLExecute or a catalog function.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR2 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      NULL,
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_CURSOR_STATE1 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CURSOR_STATE,
                      "The StatementHandle was in an executed state but "
                      "no result set was associated with the StatementHandle.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_CURSOR_STATE2 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CURSOR_STATE,
                      "The function was called without first calling SQLFetch or "
                      "SQLFetchScroll to position the cursor on the row of data required.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_USE_OF_NULL_POINTER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_USE_OF_NULL_POINTER,
                      "The argument TargetValuePtr was a null pointer.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_OPTIONAL_FEATURE_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_OPTIONAL_FEATURE_NOT_IMPLEMENTED,
                      "The driver or data source does not support the conversion specified by "
                      "the combination of the TargetType argument and the driver-specific "
                      "SQL data type of the corresponding column.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_PROGRAM_TYPE_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_APPLICATION_BUFFER_TYPE,
                      "The argument TargetType was not a valid data type, SQL_C_DEFAULT, "
                      "SQL_ARD_TYPE (in case of retrieving column data), or "
                      "SQL_APD_TYPE (in case of retrieving parameter data).",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_STRING_OR_BUFFER_LENGTH,
                      "The value specified for the argument BufferLength was less than 0.",
                      &sErrorStack );
    }

    STL_FINISH;

    if( ( sRet == SQL_ERROR ) ||
        ( sRet == SQL_SUCCESS_WITH_INFO ) )
    {
        zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_STMT,
                               aStatementHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }
        
        if( sStmt != NULL )
        {
            zlcCheckSessionAndCleanup( ZLS_STMT_DBC(sStmt) );
        }
    }

    return sRet;
}

static SQLRETURN zllGetDescField( SQLHDESC        aDescriptorHandle,
                                  SQLSMALLINT     aRecNumber,
                                  SQLSMALLINT     aFieldIdentifier,
                                  SQLPOINTER      aValuePtr,
                                  SQLINTEGER      aBufferLength,
                                  SQLINTEGER    * aStringLengthPtr,
                                  stlErrorStack * aErrorStack )
{
    SQLRETURN       sRet        = SQL_ERROR;
    zlsStmt       * sStmt       = NULL;
    zldDesc       * sDesc       = NULL;
    stlInt16        sDescType   = ZLD_DESC_TYPE_UNKNOWN;
    stlInt32        sStringLen  = 0;

    sDesc = (zldDesc*)aDescriptorHandle;
    sStmt = sDesc->mStmt;

    STL_TRY( zldDiagClear( SQL_HANDLE_DESC,
                           aDescriptorHandle,
                           aErrorStack ) == STL_SUCCESS );

    STL_TRY( zlcCheckSession( sStmt->mParentDbc,
                              aErrorStack ) == STL_SUCCESS );
    
    sDescType = sDesc->mDescType;

    switch( sStmt->mTransition )
    {
        case ZLS_TRANSITION_STATE_S0 :
            STL_THROW( RAMP_ERR_INVALID_HANDLE );
            break;
        case ZLS_TRANSITION_STATE_S1 :
            switch( sDescType )
            {
                case ZLD_DESC_TYPE_APD :
                case ZLD_DESC_TYPE_ARD :
                    break;
                default :
                    STL_THROW( RAMP_ERR_ASSOCIATED_STATEMENT_IS_NOT_PREPARED );
                    break;
            }
            break;
        case ZLS_TRANSITION_STATE_S2 :
        case ZLS_TRANSITION_STATE_S4 :
            STL_TRY_THROW( sDescType != ZLD_DESC_TYPE_IRD, RAMP_NO_DATA );
            break;
        case ZLS_TRANSITION_STATE_S3 :
            break;
        case ZLS_TRANSITION_STATE_S8 :
        case ZLS_TRANSITION_STATE_S9 :
        case ZLS_TRANSITION_STATE_S10 :
        case ZLS_TRANSITION_STATE_S11 :
        case ZLS_TRANSITION_STATE_S12 :
            STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR );
            break;
        default :
            break;
    }

    STL_TRY_THROW( aRecNumber >= 0, RAMP_ERR_INVALID_DESCRIPTOR_INDEX1 );
    STL_TRY_THROW( aRecNumber <= sDesc->mCount, RAMP_NO_DATA );

    STL_TRY_THROW( ( aRecNumber != 0 ) || ( sDescType != ZLD_DESC_TYPE_IRD ),
                   RAMP_ERR_INVALID_DESCRIPTOR_INDEX2 );

    if( ( aRecNumber == 0 ) && ( sDescType == ZLD_DESC_TYPE_IPD ) )
    {
        switch( aFieldIdentifier )
        {
            case SQL_DESC_ALLOC_TYPE :
            case SQL_DESC_ARRAY_SIZE :
            case SQL_DESC_ARRAY_STATUS_PTR :
            case SQL_DESC_BIND_OFFSET_PTR :
            case SQL_DESC_BIND_TYPE :
            case SQL_DESC_COUNT :
            case SQL_DESC_ROWS_PROCESSED_PTR :
                break;
            default :
                STL_THROW( RAMP_ERR_INVALID_DESCRIPTOR_INDEX3 );
                break;
        }
    }

    switch( aFieldIdentifier )
    {
        case SQL_DESC_BASE_COLUMN_NAME :
        case SQL_DESC_BASE_TABLE_NAME :
        case SQL_DESC_CATALOG_NAME :
        case SQL_DESC_LABEL :
        case SQL_DESC_LITERAL_PREFIX :
        case SQL_DESC_LITERAL_SUFFIX :
        case SQL_DESC_LOCAL_TYPE_NAME :
        case SQL_DESC_NAME :
        case SQL_DESC_SCHEMA_NAME :
        case SQL_DESC_TABLE_NAME :
        case SQL_DESC_TYPE_NAME :
            STL_TRY_THROW( aBufferLength >= 0, RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH );
            break;
        default :
            break;
    }

    STL_TRY( zldDescGetField( sDesc,
                              aRecNumber,
                              aFieldIdentifier,
                              aValuePtr,
                              aBufferLength,
                              &sStringLen,
                              aErrorStack ) == STL_SUCCESS );

    if( aStringLengthPtr != NULL )
    {
        *aStringLengthPtr = sStringLen;
    }

    switch( aFieldIdentifier )
    {
        case SQL_DESC_BASE_COLUMN_NAME :
        case SQL_DESC_BASE_TABLE_NAME :
        case SQL_DESC_CATALOG_NAME :
        case SQL_DESC_LABEL :
        case SQL_DESC_LITERAL_PREFIX :
        case SQL_DESC_LITERAL_SUFFIX :
        case SQL_DESC_LOCAL_TYPE_NAME :
        case SQL_DESC_NAME :
        case SQL_DESC_SCHEMA_NAME :
        case SQL_DESC_TABLE_NAME :
        case SQL_DESC_TYPE_NAME :
            STL_TRY_THROW( aBufferLength > sStringLen, RAMP_SUCCESS_WITH_INFO );
            break;
        default :
            break;
    }

    sRet = SQL_SUCCESS;
    
    return sRet;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }
    
    STL_CATCH( RAMP_NO_DATA )
    {
        sRet = SQL_NO_DATA;
    }

    STL_CATCH( RAMP_SUCCESS_WITH_INFO )
    {
        sRet = SQL_SUCCESS_WITH_INFO;

        stlPushError( STL_ERROR_LEVEL_WARNING,
                      ZLE_ERRCODE_WARNING_STRING_DATA_RIGHT_TRUNCATED,
                      "The buffer *ValuePtr was not large enough to return the entire "
                      "descriptor field, so the field was truncated. The length of "
                      "the untruncated descriptor field is returned in *StringLengthPtr.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_ASSOCIATED_STATEMENT_IS_NOT_PREPARED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_ASSOCIATED_STATEMENT_IS_NOT_PREPARED,
                      "DescriptorHandle was associated with a StatementHandle as an IRD, "
                      "and the associated statement handle had not been prepared or "
                      "executed.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_DESCRIPTOR_INDEX1 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_DESCRIPTOR_INDEX,
                      "The RecNumber argument was less than 0.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_DESCRIPTOR_INDEX2 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_DESCRIPTOR_INDEX,
                      "The RecNumber argument was set to 0, and "
                      "the SQL_ATTR_USE_BOOKMARKS statement attribute was "
                      "set to SQL_UB_OFF, and the DescriptorHandle argument was "
                      "an IRD handle.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_DESCRIPTOR_INDEX3 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_DESCRIPTOR_INDEX,
                      "The FieldIdentifier argument was a record field, "
                      "the RecNumber argument was set to 0, and "
                      "the DescriptorHandle argument was an IPD handle.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_STRING_OR_BUFFER_LENGTH,
                      "*ValuePtr is a character string, and BufferLength was less than zero.",
                      aErrorStack );
    }

    STL_FINISH;

    return sRet;
}

SQLRETURN SQL_API SQLGetDescField( SQLHDESC      aDescriptorHandle,
                                   SQLSMALLINT   aRecNumber,
                                   SQLSMALLINT   aFieldIdentifier,
                                   SQLPOINTER    aValuePtr,
                                   SQLINTEGER    aBufferLength,
                                   SQLINTEGER  * aStringLengthPtr )
{
    SQLRETURN       sRet        = SQL_ERROR;
    zlsStmt       * sStmt       = NULL;
    zldDesc       * sDesc       = NULL;
    stlErrorData  * sErrorData  = NULL;
    stlErrorStack   sErrorStack;
    stlRegionMark   sMark;
    stlInt32        sState = 0;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sDesc = (zldDesc*)aDescriptorHandle;

    STL_TRY_THROW( sDesc != NULL, RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sDesc->mType == SQL_HANDLE_DESC,
                   RAMP_ERR_INVALID_HANDLE );

    sStmt = sDesc->mStmt;

    STL_TRY( stlMarkRegionMem( &sStmt->mAllocator,
                               &sMark,
                               &sErrorStack ) == STL_SUCCESS );
    sState = 1;

    sRet = zllGetDescField( aDescriptorHandle,
                            aRecNumber,
                            aFieldIdentifier,
                            aValuePtr,
                            aBufferLength,
                            aStringLengthPtr,
                            &sErrorStack );

    STL_TRY( sRet == SQL_SUCCESS );

    sState = 0;
    STL_TRY( stlRestoreRegionMem( &sStmt->mAllocator,
                                  &sMark,
                                  STL_TRUE,
                                  &sErrorStack ) == STL_SUCCESS );

    zldDiagSetRetCode( SQL_HANDLE_DESC, aDescriptorHandle, sRet );

    return sRet;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }
    
    STL_FINISH;

    if( ( sRet == SQL_ERROR ) || ( sRet == SQL_SUCCESS_WITH_INFO ) )
    {
        zldDiagSetRetCode( SQL_HANDLE_DESC, aDescriptorHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_DESC,
                               aDescriptorHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }
        
        if( sStmt != NULL )
        {
            zlcCheckSessionAndCleanup( ZLS_STMT_DBC(sStmt) );
        }
    }

    switch( sState )
    {
        case 1 :
            (void)stlRestoreRegionMem( &sStmt->mAllocator, &sMark, STL_TRUE, &sErrorStack );
        default :
            break;
    }

    return sRet;
}

SQLRETURN SQL_API SQLGetDescFieldW( SQLHDESC      aDescriptorHandle,
                                    SQLSMALLINT   aRecNumber,
                                    SQLSMALLINT   aFieldIdentifier,
                                    SQLPOINTER    aValuePtr,
                                    SQLINTEGER    aBufferLength,
                                    SQLINTEGER  * aStringLengthPtr )
{
    /**
     * aBufferLength
     *     BufferLength should contain a count of bytes.
     * aStringLengthPtr
     *     Pointer to the buffer in which to return the total number of bytes
     *     (excluding the number of bytes required for the null-termination
     *     character) available to return in *ValuePtr.
     */

    SQLRETURN       sRet        = SQL_ERROR;
    zlsStmt       * sStmt       = NULL;
    zldDesc       * sDesc       = NULL;
    stlErrorData  * sErrorData  = NULL;
    stlErrorStack   sErrorStack;
    stlRegionMark   sMark;
    stlInt32        sState = 0;

    SQLCHAR         sCharacterAttribute[STL_MAX_SQL_IDENTIFIER_LENGTH + 1];
    SQLINTEGER      sStringLength = 0;
    stlInt64        sBufferLength = 0;
    stlInt64        sLength = 0;
    
    STL_INIT_ERROR_STACK( &sErrorStack );

    sDesc = (zldDesc*)aDescriptorHandle;

    STL_TRY_THROW( sDesc != NULL, RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sDesc->mType == SQL_HANDLE_DESC,
                   RAMP_ERR_INVALID_HANDLE );

    sStmt = sDesc->mStmt;
    
    STL_TRY( stlMarkRegionMem( &sStmt->mAllocator,
                               &sMark,
                               &sErrorStack ) == STL_SUCCESS );
    sState = 1;

    switch( aFieldIdentifier )
    {
        case SQL_DESC_BASE_COLUMN_NAME :
        case SQL_DESC_BASE_TABLE_NAME :
        case SQL_DESC_CATALOG_NAME :
        case SQL_DESC_LABEL :
        case SQL_DESC_LITERAL_PREFIX :
        case SQL_DESC_LITERAL_SUFFIX :
        case SQL_DESC_LOCAL_TYPE_NAME :
        case SQL_DESC_NAME :
        case SQL_DESC_SCHEMA_NAME :
        case SQL_DESC_TABLE_NAME :
        case SQL_DESC_TYPE_NAME :
            STL_TRY_THROW( aBufferLength >= 0, RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH );

            sRet = zllGetDescField( aDescriptorHandle,
                                    aRecNumber,
                                    aFieldIdentifier,
                                    (SQLPOINTER)sCharacterAttribute,
                                    STL_SIZEOF(sCharacterAttribute),
                                    &sStringLength,
                                    &sErrorStack );

            STL_TRY( SQL_SUCCEEDED(sRet) );

            sBufferLength = aBufferLength / STL_SIZEOF(SQLWCHAR);

            STL_TRY( zlvCharToWchar( ZLS_STMT_DBC(sStmt)->mCharacterSet,
                                     sCharacterAttribute,
                                     sStringLength,
                                     (SQLWCHAR*)aValuePtr,
                                     sBufferLength,
                                     &sLength,
                                     &sErrorStack ) == STL_SUCCESS );

            if( aStringLengthPtr != NULL )
            {
                *aStringLengthPtr = sLength * STL_SIZEOF(SQLWCHAR);
            }

            STL_TRY_THROW( sLength < sBufferLength, RAMP_WARNING_STRING_DATA_RIGHT_TRUNCATED );
            break;
        default :
            sRet = zllGetDescField( aDescriptorHandle,
                                    aRecNumber,
                                    aFieldIdentifier,
                                    aValuePtr,
                                    aBufferLength,
                                    aStringLengthPtr,
                                    &sErrorStack );
            break;
    }

    STL_TRY( SQL_SUCCEEDED(sRet) );

    sState = 0;
    STL_TRY( stlRestoreRegionMem( &sStmt->mAllocator,
                                  &sMark,
                                  STL_TRUE,
                                  &sErrorStack ) == STL_SUCCESS );

    zldDiagSetRetCode( SQL_HANDLE_DESC, aDescriptorHandle, sRet );

    return sRet;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }
    
    STL_CATCH( RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_STRING_OR_BUFFER_LENGTH,
                      "*ValuePtr is a character string, and BufferLength was less than zero.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_WARNING_STRING_DATA_RIGHT_TRUNCATED )
    {
        stlPushError( STL_ERROR_LEVEL_WARNING,
                      ZLE_ERRCODE_WARNING_STRING_DATA_RIGHT_TRUNCATED,
                      "The buffer *ValuePtr was not large enough to return the entire descriptor field, "
                      "so the field was truncated."
                      "The length of the untruncated string value is returned in *StringLengthPtr.",
                      &sErrorStack );

        sRet = SQL_SUCCESS_WITH_INFO;
    }

    STL_FINISH;

    if( ( sRet == SQL_ERROR ) || ( sRet == SQL_SUCCESS_WITH_INFO ) )
    {
        zldDiagSetRetCode( SQL_HANDLE_DESC, aDescriptorHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_DESC,
                               aDescriptorHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }
        
        if( sStmt != NULL )
        {
            zlcCheckSessionAndCleanup( ZLS_STMT_DBC(sStmt) );
        }
    }

    switch( sState )
    {
        case 1 :
            (void)stlRestoreRegionMem( &sStmt->mAllocator, &sMark, STL_TRUE, &sErrorStack );
        default :
            break;
    }

    return sRet;
}

static SQLRETURN zllGetDescRec( SQLHDESC        aDescriptorHandle,
                                SQLSMALLINT     aRecNumber,
                                SQLCHAR       * aName,
                                SQLSMALLINT     aBufferLength,
                                SQLSMALLINT   * aStringLengthPtr,
                                SQLSMALLINT   * aTypePtr,
                                SQLSMALLINT   * aSubTypePtr,
                                SQLLEN        * aLengthPtr,
                                SQLSMALLINT   * aPrecisionPtr,
                                SQLSMALLINT   * aScalePtr,
                                SQLSMALLINT   * aNullablePtr,
                                stlErrorStack * aErrorStack )
{
    SQLRETURN       sRet        = SQL_ERROR;
    zlsStmt       * sStmt       = NULL;
    zldDesc       * sDesc       = NULL;
    stlInt16        sDescType   = ZLD_DESC_TYPE_UNKNOWN;
    stlInt16        sStringLen  = 0;
    stlInt64        sLength     = 0;

    sDesc = (zldDesc*)aDescriptorHandle;
    sStmt = sDesc->mStmt;

    STL_TRY( zldDiagClear( SQL_HANDLE_DESC,
                           aDescriptorHandle,
                           aErrorStack ) == STL_SUCCESS );

    STL_TRY( zlcCheckSession( sStmt->mParentDbc,
                              aErrorStack ) == STL_SUCCESS );
    
    sDescType = sDesc->mDescType;

    switch( sStmt->mTransition )
    {
        case ZLS_TRANSITION_STATE_S0 :
            STL_THROW( RAMP_ERR_INVALID_HANDLE );
            break;
        case ZLS_TRANSITION_STATE_S1 :
            switch( sDescType )
            {
                case ZLD_DESC_TYPE_APD :
                case ZLD_DESC_TYPE_ARD :
                    break;
                default :
                    STL_THROW( RAMP_ERR_ASSOCIATED_STATEMENT_IS_NOT_PREPARED );
                    break;
            }
            break;
        case ZLS_TRANSITION_STATE_S2 :
        case ZLS_TRANSITION_STATE_S4 :
            STL_TRY_THROW( sDescType != ZLD_DESC_TYPE_IRD, RAMP_NO_DATA );
            break;
        case ZLS_TRANSITION_STATE_S3 :
            break;
        case ZLS_TRANSITION_STATE_S8 :
        case ZLS_TRANSITION_STATE_S9 :
        case ZLS_TRANSITION_STATE_S10 :
        case ZLS_TRANSITION_STATE_S11 :
        case ZLS_TRANSITION_STATE_S12 :
            STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR );
            break;
        default :
            break;
    }

    STL_TRY_THROW( aRecNumber > 0, RAMP_ERR_INVALID_DESCRIPTOR_INDEX );
    STL_TRY_THROW( aRecNumber <= sDesc->mCount, RAMP_NO_DATA );

    STL_TRY( zldDescGetRec( sDesc,
                            aRecNumber,
                            (stlChar*)aName,
                            aBufferLength,
                            &sStringLen,
                            aTypePtr,
                            aSubTypePtr,
                            &sLength,
                            aPrecisionPtr,
                            aScalePtr,
                            aNullablePtr,
                            aErrorStack ) == STL_SUCCESS );

    if( aStringLengthPtr != NULL )
    {
        *aStringLengthPtr = sStringLen;
    }

    if( aLengthPtr != NULL )
    {
        *aLengthPtr = (SQLLEN)sLength;
    }

    STL_TRY_THROW( aBufferLength > sStringLen, RAMP_SUCCESS_WITH_INFO );

    sRet = SQL_SUCCESS;
    
    return sRet;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }
    
    STL_CATCH( RAMP_NO_DATA )
    {
        sRet = SQL_NO_DATA;
    }

    STL_CATCH( RAMP_SUCCESS_WITH_INFO )
    {
        sRet = SQL_SUCCESS_WITH_INFO;

        stlPushError( STL_ERROR_LEVEL_WARNING,
                      ZLE_ERRCODE_WARNING_STRING_DATA_RIGHT_TRUNCATED,
                      "The buffer *Name was not large enough to return the entire "
                      "descriptor field. Therefore, the field was truncated. "
                      "The length of the untruncated descriptor field is returned in "
                      "*StringLengthPtr.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_ASSOCIATED_STATEMENT_IS_NOT_PREPARED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_ASSOCIATED_STATEMENT_IS_NOT_PREPARED,
                      "DescriptorHandle was associated with an IRD, "
                      "and the associated statement handle was not in the prepared or "
                      "executed state.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_DESCRIPTOR_INDEX )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_DESCRIPTOR_INDEX,
                      "The RecNumber argument was less than 1.",
                      aErrorStack );
    }

    STL_FINISH;

    return sRet;
}

SQLRETURN SQL_API SQLGetDescRec( SQLHDESC      aDescriptorHandle,
                                 SQLSMALLINT   aRecNumber,
                                 SQLCHAR     * aName,
                                 SQLSMALLINT   aBufferLength,
                                 SQLSMALLINT * aStringLengthPtr,
                                 SQLSMALLINT * aTypePtr,
                                 SQLSMALLINT * aSubTypePtr,
                                 SQLLEN      * aLengthPtr,
                                 SQLSMALLINT * aPrecisionPtr,
                                 SQLSMALLINT * aScalePtr,
                                 SQLSMALLINT * aNullablePtr )
{
    SQLRETURN       sRet        = SQL_ERROR;
    zlsStmt       * sStmt       = NULL;
    zldDesc       * sDesc       = NULL;
    stlErrorData  * sErrorData  = NULL;
    stlErrorStack   sErrorStack;
    stlRegionMark   sMark;
    stlInt32        sState = 0;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sDesc = (zldDesc*)aDescriptorHandle;

    STL_TRY_THROW( sDesc != NULL, RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sDesc->mType == SQL_HANDLE_DESC,
                   RAMP_ERR_INVALID_HANDLE );

    sStmt = sDesc->mStmt;
    
    STL_TRY( stlMarkRegionMem( &sStmt->mAllocator,
                               &sMark,
                               &sErrorStack ) == STL_SUCCESS );
    sState = 1;

    sRet = zllGetDescRec( aDescriptorHandle,
                          aRecNumber,
                          aName,
                          aBufferLength,
                          aStringLengthPtr,
                          aTypePtr,
                          aSubTypePtr,
                          aLengthPtr,
                          aPrecisionPtr,
                          aScalePtr,
                          aNullablePtr,
                          &sErrorStack );

    STL_TRY( sRet == SQL_SUCCESS );

    sState = 0;
    STL_TRY( stlRestoreRegionMem( &sStmt->mAllocator,
                                  &sMark,
                                  STL_TRUE,
                                  &sErrorStack ) == STL_SUCCESS );

    zldDiagSetRetCode( SQL_HANDLE_DESC, aDescriptorHandle, sRet );

    return sRet;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }
    
    STL_FINISH;

    if( ( sRet == SQL_ERROR ) || ( sRet == SQL_SUCCESS_WITH_INFO ) )
    {
        zldDiagSetRetCode( SQL_HANDLE_DESC, aDescriptorHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_DESC,
                               aDescriptorHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }
        
        if( sStmt != NULL )
        {
            zlcCheckSessionAndCleanup( ZLS_STMT_DBC(sStmt) );
        }
    }

    switch( sState )
    {
        case 1 :
            (void)stlRestoreRegionMem( &sStmt->mAllocator, &sMark, STL_TRUE, &sErrorStack );
        default :
            break;
    }

    return sRet;
}

SQLRETURN SQL_API SQLGetDescRecW( SQLHDESC      aDescriptorHandle,
                                  SQLSMALLINT   aRecNumber,
                                  SQLWCHAR    * aName,
                                  SQLSMALLINT   aBufferLength,
                                  SQLSMALLINT * aStringLengthPtr,
                                  SQLSMALLINT * aTypePtr,
                                  SQLSMALLINT * aSubTypePtr,
                                  SQLLEN      * aLengthPtr,
                                  SQLSMALLINT * aPrecisionPtr,
                                  SQLSMALLINT * aScalePtr,
                                  SQLSMALLINT * aNullablePtr )
{
    /**
     * aBufferLength
     *     Length of the *Name buffer, in characters.
     * aStringLengthPtr
     *     A pointer to a buffer in which to return the number of characters
     *     available to return in the *Name buffer, excluding the
     *     null-termination character.
     */
    
    SQLRETURN       sRet        = SQL_ERROR;
    zlsStmt       * sStmt       = NULL;
    zldDesc       * sDesc       = NULL;
    stlErrorData  * sErrorData  = NULL;
    stlErrorStack   sErrorStack;
    stlRegionMark   sMark;
    stlInt32        sState = 0;

    SQLCHAR         sName[STL_MAX_SQL_IDENTIFIER_LENGTH + 1];
    SQLSMALLINT     sStringLength = 0;
    stlInt64        sLength = 0;
    
    STL_INIT_ERROR_STACK( &sErrorStack );

    sDesc = (zldDesc*)aDescriptorHandle;

    STL_TRY_THROW( sDesc != NULL, RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sDesc->mType == SQL_HANDLE_DESC,
                   RAMP_ERR_INVALID_HANDLE );

    sStmt = sDesc->mStmt;

    STL_TRY( stlMarkRegionMem( &sStmt->mAllocator,
                               &sMark,
                               &sErrorStack ) == STL_SUCCESS );
    sState = 1;

    sRet = zllGetDescRec( aDescriptorHandle,
                          aRecNumber,
                          sName,
                          STL_SIZEOF(sName),
                          &sStringLength,
                          aTypePtr,
                          aSubTypePtr,
                          aLengthPtr,
                          aPrecisionPtr,
                          aScalePtr,
                          aNullablePtr,
                          &sErrorStack );

    STL_TRY( SQL_SUCCEEDED(sRet) );

    STL_TRY( zlvCharToWchar( ZLS_STMT_DBC(sStmt)->mCharacterSet,
                             sName,
                             sStringLength,
                             aName,
                             aBufferLength,
                             &sLength,
                             &sErrorStack ) == STL_SUCCESS );

    if( aStringLengthPtr != NULL )
    {
        *aStringLengthPtr = sLength;
    }

    STL_TRY_THROW( sLength < aBufferLength, RAMP_WARNING_STRING_DATA_RIGHT_TRUNCATED );

    sState = 0;
    STL_TRY( stlRestoreRegionMem( &sStmt->mAllocator,
                                  &sMark,
                                  STL_TRUE,
                                  &sErrorStack ) == STL_SUCCESS );

    zldDiagSetRetCode( SQL_HANDLE_DESC, aDescriptorHandle, sRet );

    return sRet;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }
    
    STL_CATCH( RAMP_WARNING_STRING_DATA_RIGHT_TRUNCATED )
    {
        sRet = SQL_SUCCESS_WITH_INFO;

        stlPushError( STL_ERROR_LEVEL_WARNING,
                      ZLE_ERRCODE_WARNING_STRING_DATA_RIGHT_TRUNCATED,
                      "The buffer *Name was not large enough to return the entire "
                      "descriptor field. Therefore, the field was truncated. "
                      "The length of the untruncated descriptor field is returned in "
                      "*StringLengthPtr.",
                      &sErrorStack );
    }

    STL_FINISH;

    if( ( sRet == SQL_ERROR ) || ( sRet == SQL_SUCCESS_WITH_INFO ) )
    {
        zldDiagSetRetCode( SQL_HANDLE_DESC, aDescriptorHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_DESC,
                               aDescriptorHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }
        
        if( sStmt != NULL )
        {
            zlcCheckSessionAndCleanup( ZLS_STMT_DBC(sStmt) );
        }
    }

    switch( sState )
    {
        case 1 :
            (void)stlRestoreRegionMem( &sStmt->mAllocator, &sMark, STL_TRUE, &sErrorStack );
        default :
            break;
    }

    return sRet;
}

SQLRETURN SQL_API SQLGetDiagField( SQLSMALLINT   aHandleType,
                                   SQLHANDLE     aHandle,
                                   SQLSMALLINT   aRecNumber,
                                   SQLSMALLINT   aDiagIdentifier,
                                   SQLPOINTER    aDiagInfoPtr,
                                   SQLSMALLINT   aBufferLength,
                                   SQLSMALLINT * aStringLengthPtr )
{
    SQLRETURN       sRet       = SQL_ERROR;
    zlnEnv        * sEnv       = NULL;
    zlcDbc        * sDbc       = NULL;
    zlsStmt       * sStmt      = NULL;
    zldDesc       * sDesc      = NULL;
    zldDiag       * sDiag      = NULL;
    stlInt16        sStringLen = 0;
    stlInt32        sODBCVer   = 0;
    stlRegionMark   sMark;
    stlInt32        sState = 0;
    stlErrorStack   sErrorStack;

    STL_INIT_ERROR_STACK( &sErrorStack );

    STL_TRY_THROW( aHandle != NULL, RAMP_ERR_INVALID_HANDLE );

    switch( aHandleType )
    {
        case SQL_HANDLE_ENV :
            sEnv = (zlnEnv*)aHandle;
            
            STL_TRY_THROW( sEnv->mType == SQL_HANDLE_ENV,
                           RAMP_ERR_INVALID_HANDLE );
            
            sDiag = &sEnv->mDiag;
            sODBCVer = sEnv->mAttrVersion;
            break;
        case SQL_HANDLE_DBC :
            sDbc = (zlcDbc*)aHandle;
            
            STL_TRY_THROW( sDbc->mType == SQL_HANDLE_DBC,
                           RAMP_ERR_INVALID_HANDLE );
            
            sDiag = &sDbc->mDiag;
            sODBCVer = sDbc->mParentEnv->mAttrVersion;
            break;
        case SQL_HANDLE_STMT :
            sStmt = (zlsStmt*)aHandle;
            
            STL_TRY_THROW( sStmt->mType == SQL_HANDLE_STMT,
                           RAMP_ERR_INVALID_HANDLE );

            switch( sStmt->mTransition )
            {
                case ZLS_TRANSITION_STATE_S0 :
                    STL_THROW( RAMP_ERR_INVALID_HANDLE );
                    break;
                case ZLS_TRANSITION_STATE_S1 :
                case ZLS_TRANSITION_STATE_S2 :
                case ZLS_TRANSITION_STATE_S3 :
                case ZLS_TRANSITION_STATE_S8 :
                case ZLS_TRANSITION_STATE_S9 :
                case ZLS_TRANSITION_STATE_S10 :
                case ZLS_TRANSITION_STATE_S11 :
                case ZLS_TRANSITION_STATE_S12 :
                    STL_TRY( aDiagIdentifier != SQL_DIAG_ROW_COUNT );
                    break;
                default :
                    break;
            }
            
            sDiag = &sStmt->mDiag;
            sODBCVer = sStmt->mParentDbc->mParentEnv->mAttrVersion;
            break;
        case SQL_HANDLE_DESC :
            sDesc = (zldDesc*)aHandle;
            
            STL_TRY_THROW( sDesc->mType == SQL_HANDLE_DESC,
                           RAMP_ERR_INVALID_HANDLE );
            
            sDiag = &sDesc->mDiag;
            sODBCVer = sDesc->mStmt->mParentDbc->mParentEnv->mAttrVersion;
            break;
        default :
            STL_THROW( RAMP_ERR_INVALID_HANDLE );
            break;
    }

    switch( aDiagIdentifier )
    {
        case SQL_DIAG_CURSOR_ROW_COUNT :
        case SQL_DIAG_DYNAMIC_FUNCTION :
        case SQL_DIAG_DYNAMIC_FUNCTION_CODE :
        case SQL_DIAG_NUMBER :
        case SQL_DIAG_RETURNCODE :
        case SQL_DIAG_ROW_COUNT :
            break;
        case SQL_DIAG_CLASS_ORIGIN :
        case SQL_DIAG_COLUMN_NUMBER :
        case SQL_DIAG_CONNECTION_NAME :
        case SQL_DIAG_MESSAGE_TEXT :
        case SQL_DIAG_NATIVE :
        case SQL_DIAG_ROW_NUMBER :
        case SQL_DIAG_SERVER_NAME :
        case SQL_DIAG_SQLSTATE :
        case SQL_DIAG_SUBCLASS_ORIGIN :
            STL_TRY( aRecNumber > 0 );
            break;
        default :
            STL_THROW( RAMP_ERROR );
            break;
    }

    switch( aDiagIdentifier )
    {
        case SQL_DIAG_DYNAMIC_FUNCTION :
        case SQL_DIAG_CLASS_ORIGIN :
        case SQL_DIAG_CONNECTION_NAME :
        case SQL_DIAG_MESSAGE_TEXT :
        case SQL_DIAG_SERVER_NAME :
        case SQL_DIAG_SQLSTATE :
        case SQL_DIAG_SUBCLASS_ORIGIN :
            STL_TRY( aBufferLength >= 0 );
            break;
        default :
            break;
    }

    if( aHandleType != SQL_HANDLE_STMT )
    {
        switch( aDiagIdentifier )
        {
            case SQL_DIAG_CURSOR_ROW_COUNT :
            case SQL_DIAG_DYNAMIC_FUNCTION :
            case SQL_DIAG_DYNAMIC_FUNCTION_CODE :
            case SQL_DIAG_ROW_COUNT :
                STL_THROW( RAMP_ERROR );
                break;
            default :
                break;
        }
    }
    
    STL_TRY_THROW( aRecNumber <= sDiag->mNumber, RAMP_NO_DATA );

    STL_TRY( stlMarkRegionMem( &sDiag->mRegionMem,
                               &sMark,
                               &sErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( zldDiagGetField( sDiag,
                              aRecNumber,
                              aDiagIdentifier,
                              aDiagInfoPtr,
                              aBufferLength,
                              &sStringLen ) == STL_SUCCESS );

    if( aStringLengthPtr != NULL )
    {
        *aStringLengthPtr = sStringLen;
    }

    switch( aDiagIdentifier )
    {
        case SQL_DIAG_SQLSTATE :
            if( (sODBCVer == SQL_OV_ODBC2) && (aBufferLength > 2) )
            {
                if( (((SQLCHAR*)aDiagInfoPtr)[0] == 'H') && (((SQLCHAR*)aDiagInfoPtr)[1] == 'Y') )
                {
                    ((SQLCHAR*)aDiagInfoPtr)[0] = 'S';
                    ((SQLCHAR*)aDiagInfoPtr)[1] = '1';
                }
            }
        case SQL_DIAG_DYNAMIC_FUNCTION :
        case SQL_DIAG_CLASS_ORIGIN :
        case SQL_DIAG_CONNECTION_NAME :
        case SQL_DIAG_MESSAGE_TEXT :
        case SQL_DIAG_SERVER_NAME :
        case SQL_DIAG_SUBCLASS_ORIGIN :
            if( aBufferLength > sStringLen )
            {
                sRet = SQL_SUCCESS;
            }
            else
            {
                sRet = SQL_SUCCESS_WITH_INFO;
            }
            break;
        default :
            sRet = SQL_SUCCESS;
            break;
    }

    sState = 0;
    STL_TRY( stlRestoreRegionMem( &sDiag->mRegionMem,
                                  &sMark,
                                  STL_TRUE,
                                  &sErrorStack ) == STL_SUCCESS );

    return sRet;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }
    
    STL_CATCH( RAMP_NO_DATA )
    {
        sRet = SQL_NO_DATA;
    }

    STL_CATCH( RAMP_ERROR )
    {
        sRet = SQL_ERROR;
    }

    STL_FINISH;

    switch( sState )
    {
        case 1 :
            (void)stlRestoreRegionMem( &sDiag->mRegionMem, &sMark, STL_TRUE, &sErrorStack );
        default :
            break;
    }

    return sRet;
}

SQLRETURN SQL_API SQLGetDiagFieldW( SQLSMALLINT   aHandleType,
                                    SQLHANDLE     aHandle,
                                    SQLSMALLINT   aRecNumber,
                                    SQLSMALLINT   aDiagIdentifier,
                                    SQLPOINTER    aDiagInfoPtr,
                                    SQLSMALLINT   aBufferLength,
                                    SQLSMALLINT * aStringLengthPtr )
{
    /**
     * aBufferLength
     *     BufferLength should contain a count of bytes.
     * aStringLengthPtr
     *     Pointer to a buffer in which to return the total number of bytes
     *     (excluding the number of bytes required for the null-termination
     *     character) available to return in *DiagInfoPtr, for character data. 
     */

    SQLRETURN         sRet       = SQL_ERROR;
    zlnEnv          * sEnv       = NULL;
    zlcDbc          * sDbc       = NULL;
    zlsStmt         * sStmt      = NULL;
    zldDesc         * sDesc      = NULL;
    zldDiag         * sDiag      = NULL;
    stlInt16          sStringLen = 0;
    stlInt32          sODBCVer   = 0;
    stlRegionMark     sMark;
    stlInt32          sState = 0;
    dtlCharacterSet   sCharacterSet = DTL_SQL_ASCII;
    stlErrorStack     sErrorStack;

    STL_INIT_ERROR_STACK( &sErrorStack );

    STL_TRY_THROW( aHandle != NULL, RAMP_ERR_INVALID_HANDLE );

    switch( aHandleType )
    {
        case SQL_HANDLE_ENV :
            sEnv = (zlnEnv*)aHandle;
            
            STL_TRY_THROW( sEnv->mType == SQL_HANDLE_ENV,
                           RAMP_ERR_INVALID_HANDLE );
            
            sDiag = &sEnv->mDiag;
            sODBCVer = sEnv->mAttrVersion;
            break;
        case SQL_HANDLE_DBC :
            sDbc = (zlcDbc*)aHandle;
            
            STL_TRY_THROW( sDbc->mType == SQL_HANDLE_DBC,
                           RAMP_ERR_INVALID_HANDLE );
            
            sDiag = &sDbc->mDiag;
            sCharacterSet = sDbc->mCharacterSet;
            sODBCVer = sDbc->mParentEnv->mAttrVersion;
            break;
        case SQL_HANDLE_STMT :
            sStmt = (zlsStmt*)aHandle;
            
            STL_TRY_THROW( sStmt->mType == SQL_HANDLE_STMT,
                           RAMP_ERR_INVALID_HANDLE );

            switch( sStmt->mTransition )
            {
                case ZLS_TRANSITION_STATE_S0 :
                    STL_THROW( RAMP_ERR_INVALID_HANDLE );
                    break;
                case ZLS_TRANSITION_STATE_S1 :
                case ZLS_TRANSITION_STATE_S2 :
                case ZLS_TRANSITION_STATE_S3 :
                case ZLS_TRANSITION_STATE_S8 :
                case ZLS_TRANSITION_STATE_S9 :
                case ZLS_TRANSITION_STATE_S10 :
                case ZLS_TRANSITION_STATE_S11 :
                case ZLS_TRANSITION_STATE_S12 :
                    STL_TRY( aDiagIdentifier != SQL_DIAG_ROW_COUNT );
                    break;
                default :
                    break;
            }
            
            sDiag = &sStmt->mDiag;
            sCharacterSet = ZLS_STMT_DBC(sStmt)->mCharacterSet;
            sODBCVer = sStmt->mParentDbc->mParentEnv->mAttrVersion;
            break;
        case SQL_HANDLE_DESC :
            sDesc = (zldDesc*)aHandle;
            
            STL_TRY_THROW( sDesc->mType == SQL_HANDLE_DESC,
                           RAMP_ERR_INVALID_HANDLE );
            
            sDiag = &sDesc->mDiag;
            sCharacterSet = ZLS_STMT_DBC(sDesc->mStmt)->mCharacterSet;
            sODBCVer = sDesc->mStmt->mParentDbc->mParentEnv->mAttrVersion;
            break;
        default :
            STL_THROW( RAMP_ERR_INVALID_HANDLE );
            break;
    }

    switch( aDiagIdentifier )
    {
        case SQL_DIAG_CURSOR_ROW_COUNT :
        case SQL_DIAG_DYNAMIC_FUNCTION :
        case SQL_DIAG_DYNAMIC_FUNCTION_CODE :
        case SQL_DIAG_NUMBER :
        case SQL_DIAG_RETURNCODE :
        case SQL_DIAG_ROW_COUNT :
            break;
        case SQL_DIAG_CLASS_ORIGIN :
        case SQL_DIAG_COLUMN_NUMBER :
        case SQL_DIAG_CONNECTION_NAME :
        case SQL_DIAG_MESSAGE_TEXT :
        case SQL_DIAG_NATIVE :
        case SQL_DIAG_ROW_NUMBER :
        case SQL_DIAG_SERVER_NAME :
        case SQL_DIAG_SQLSTATE :
        case SQL_DIAG_SUBCLASS_ORIGIN :
            STL_TRY( aRecNumber > 0 );
            break;
        default :
            STL_THROW( RAMP_ERROR );
            break;
    }

    switch( aDiagIdentifier )
    {
        case SQL_DIAG_DYNAMIC_FUNCTION :
        case SQL_DIAG_CLASS_ORIGIN :
        case SQL_DIAG_CONNECTION_NAME :
        case SQL_DIAG_MESSAGE_TEXT :
        case SQL_DIAG_SERVER_NAME :
        case SQL_DIAG_SQLSTATE :
        case SQL_DIAG_SUBCLASS_ORIGIN :
            STL_TRY( aBufferLength >= 0 );
            break;
        default :
            break;
    }

    if( aHandleType != SQL_HANDLE_STMT )
    {
        switch( aDiagIdentifier )
        {
            case SQL_DIAG_CURSOR_ROW_COUNT :
            case SQL_DIAG_DYNAMIC_FUNCTION :
            case SQL_DIAG_DYNAMIC_FUNCTION_CODE :
            case SQL_DIAG_ROW_COUNT :
                STL_THROW( RAMP_ERROR );
                break;
            default :
                break;
        }
    }
    
    STL_TRY_THROW( aRecNumber <= sDiag->mNumber, RAMP_NO_DATA );

    STL_TRY( stlMarkRegionMem( &sDiag->mRegionMem,
                               &sMark,
                               &sErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( zldDiagGetFieldW( sDiag,
                               aRecNumber,
                               sCharacterSet,
                               aDiagIdentifier,
                               aDiagInfoPtr,
                               aBufferLength,
                               &sStringLen,
                               &sErrorStack ) == STL_SUCCESS );

    if( aStringLengthPtr != NULL )
    {
        *aStringLengthPtr = sStringLen;
    }

    switch( aDiagIdentifier )
    {
        case SQL_DIAG_SQLSTATE :
            if( (sODBCVer == SQL_OV_ODBC2) && (aBufferLength > 2) )
            {
                if( (((SQLWCHAR*)aDiagInfoPtr)[0] == 'H') && (((SQLWCHAR*)aDiagInfoPtr)[1] == 'Y') )
                {
                    ((SQLWCHAR*)aDiagInfoPtr)[0] = 'S';
                    ((SQLWCHAR*)aDiagInfoPtr)[1] = '1';
                }
            }
        case SQL_DIAG_DYNAMIC_FUNCTION :
        case SQL_DIAG_CLASS_ORIGIN :
        case SQL_DIAG_CONNECTION_NAME :
        case SQL_DIAG_MESSAGE_TEXT :
        case SQL_DIAG_SERVER_NAME :
        case SQL_DIAG_SUBCLASS_ORIGIN :
            if( aBufferLength > sStringLen )
            {
                sRet = SQL_SUCCESS;
            }
            else
            {
                sRet = SQL_SUCCESS_WITH_INFO;
            }
            break;
        default :
            sRet = SQL_SUCCESS;
            break;
    }

    sState = 0;
    STL_TRY( stlRestoreRegionMem( &sDiag->mRegionMem,
                                  &sMark,
                                  STL_TRUE,
                                  &sErrorStack ) == STL_SUCCESS );

    return sRet;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }
    
    STL_CATCH( RAMP_NO_DATA )
    {
        sRet = SQL_NO_DATA;
    }

    STL_CATCH( RAMP_ERROR )
    {
        sRet = SQL_ERROR;
    }

    STL_FINISH;

    switch( sState )
    {
        case 1 :
            (void)stlRestoreRegionMem( &sDiag->mRegionMem, &sMark, STL_TRUE, &sErrorStack );
        default :
            break;
    }

    return sRet;
}

SQLRETURN SQL_API SQLGetDiagRec( SQLSMALLINT   aHandleType,
                                 SQLHANDLE     aHandle,
                                 SQLSMALLINT   aRecNumber,
                                 SQLCHAR     * aSQLState,
                                 SQLINTEGER  * aNativeErrorPtr,
                                 SQLCHAR     * aMessageText,
                                 SQLSMALLINT   aBufferLength,
                                 SQLSMALLINT * aTextLengthPtr )
{
    SQLRETURN       sRet     = SQL_ERROR;
    zlnEnv        * sEnv     = NULL;
    zlcDbc        * sDbc     = NULL;
    zlsStmt       * sStmt    = NULL;
    zldDesc       * sDesc    = NULL;
    zldDiag       * sDiag    = NULL;
    stlInt16        sTextLen = 0;
    stlInt32        sNativeError = 0;
    stlInt32        sODBCVer = 0;
    stlRegionMark   sMark;
    stlInt32        sState = 0;
    stlErrorStack   sErrorStack;

    STL_INIT_ERROR_STACK( &sErrorStack );

    STL_TRY_THROW( aHandle != NULL, RAMP_ERR_INVALID_HANDLE );
    
    STL_TRY( aRecNumber > 0 );
    STL_TRY( aBufferLength >= 0 );

    switch( aHandleType )
    {
        case SQL_HANDLE_ENV :
            sEnv = (zlnEnv*)aHandle;
            
            STL_TRY_THROW( sEnv->mType == SQL_HANDLE_ENV,
                           RAMP_ERR_INVALID_HANDLE );
            
            sDiag = &sEnv->mDiag;
            sODBCVer = sEnv->mAttrVersion;
            break;
        case SQL_HANDLE_DBC :
            sDbc = (zlcDbc*)aHandle;
            
            STL_TRY_THROW( sDbc->mType == SQL_HANDLE_DBC,
                           RAMP_ERR_INVALID_HANDLE );
            
            sDiag = &sDbc->mDiag;
            sODBCVer = sDbc->mParentEnv->mAttrVersion;
            break;
        case SQL_HANDLE_STMT :
            sStmt = (zlsStmt*)aHandle;
            
            STL_TRY_THROW( sStmt->mType == SQL_HANDLE_STMT,
                           RAMP_ERR_INVALID_HANDLE );
            
            sDiag = &sStmt->mDiag;
            sODBCVer = sStmt->mParentDbc->mParentEnv->mAttrVersion;
            break;
        case SQL_HANDLE_DESC :
            sDesc = (zldDesc*)aHandle;
            
            STL_TRY_THROW( sDesc->mType == SQL_HANDLE_DESC,
                           RAMP_ERR_INVALID_HANDLE );
            
            sDiag = &sDesc->mDiag;
            sODBCVer = sDesc->mStmt->mParentDbc->mParentEnv->mAttrVersion;
            break;
        default :
            STL_THROW( RAMP_ERR_INVALID_HANDLE );
            break;
    }

    STL_TRY_THROW( aRecNumber <= sDiag->mNumber, RAMP_NO_DATA );

    STL_TRY( stlMarkRegionMem( &sDiag->mRegionMem,
                               &sMark,
                               &sErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( zldDiagGetRec( sDiag,
                            aRecNumber,
                            (stlChar*)aSQLState,
                            &sNativeError,
                            (stlChar*)aMessageText,
                            aBufferLength,
                            &sTextLen ) == STL_SUCCESS );

    if( sODBCVer == SQL_OV_ODBC2 )
    {
        if( aSQLState != NULL )
        {
            if( (aSQLState[0] == 'H') && (aSQLState[1] == 'Y') )
            {
                aSQLState[0] = 'S';
                aSQLState[1] = '1';
            }
        }
    }

    if( aNativeErrorPtr != NULL )
    {
        *aNativeErrorPtr = sNativeError;
    }

    if( aTextLengthPtr != NULL )
    {
        *aTextLengthPtr = sTextLen;
    }

    if( aBufferLength > sTextLen )
    {
        sRet = SQL_SUCCESS;
    }
    else
    {
        sRet = SQL_SUCCESS_WITH_INFO;
    }
    
    sState = 0;
    STL_TRY( stlRestoreRegionMem( &sDiag->mRegionMem,
                                  &sMark,
                                  STL_TRUE,
                                  &sErrorStack ) == STL_SUCCESS );

    return sRet;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }
    
    STL_CATCH( RAMP_NO_DATA )
    {
        sRet = SQL_NO_DATA;
    }

    STL_FINISH;

    switch( sState )
    {
        case 1 :
            (void)stlRestoreRegionMem( &sDiag->mRegionMem, &sMark, STL_TRUE, &sErrorStack );
        default :
            break;
    }

    return sRet;
}

SQLRETURN SQL_API SQLGetDiagRecW( SQLSMALLINT   aHandleType,
                                  SQLHANDLE     aHandle,
                                  SQLSMALLINT   aRecNumber,
                                  SQLWCHAR    * aSQLState,
                                  SQLINTEGER  * aNativeErrorPtr,
                                  SQLWCHAR    * aMessageText,
                                  SQLSMALLINT   aBufferLength,
                                  SQLSMALLINT * aTextLengthPtr )
{
    /**
     * aBufferLength
     *     Length of the *MessageText buffer in characters.
     * aTextLengthPtr
     *     Pointer to a buffer in which to return the total number of
     *     characters (excluding the null-termination character) available to
     *     return in *MessageText. 
     */

    SQLRETURN         sRet     = SQL_ERROR;
    zlnEnv          * sEnv     = NULL;
    zlcDbc          * sDbc     = NULL;
    zlsStmt         * sStmt    = NULL;
    zldDesc         * sDesc    = NULL;
    zldDiag         * sDiag    = NULL;
    stlInt16          sTextLen = 0;
    stlInt32          sNativeError = 0;
    stlInt32          sODBCVer = 0;
    dtlCharacterSet   sCharacterSet = DTL_SQL_ASCII;
    stlRegionMark     sMark;
    stlInt32          sState = 0;
    stlErrorStack     sErrorStack;

    STL_INIT_ERROR_STACK( &sErrorStack );

    STL_TRY_THROW( aHandle != NULL, RAMP_ERR_INVALID_HANDLE );
    
    STL_TRY( aRecNumber > 0 );
    STL_TRY( aBufferLength >= 0 );

    switch( aHandleType )
    {
        case SQL_HANDLE_ENV :
            sEnv = (zlnEnv*)aHandle;
            
            STL_TRY_THROW( sEnv->mType == SQL_HANDLE_ENV,
                           RAMP_ERR_INVALID_HANDLE );
            
            sDiag = &sEnv->mDiag;
            sODBCVer = sEnv->mAttrVersion;
            break;
        case SQL_HANDLE_DBC :
            sDbc = (zlcDbc*)aHandle;
            
            STL_TRY_THROW( sDbc->mType == SQL_HANDLE_DBC,
                           RAMP_ERR_INVALID_HANDLE );
            
            sDiag = &sDbc->mDiag;
            sCharacterSet = sDbc->mCharacterSet;
            sODBCVer = sDbc->mParentEnv->mAttrVersion;
            break;
        case SQL_HANDLE_STMT :
            sStmt = (zlsStmt*)aHandle;
            
            STL_TRY_THROW( sStmt->mType == SQL_HANDLE_STMT,
                           RAMP_ERR_INVALID_HANDLE );
            
            sDiag = &sStmt->mDiag;
            sCharacterSet = ZLS_STMT_DBC(sStmt)->mCharacterSet;
            sODBCVer = sStmt->mParentDbc->mParentEnv->mAttrVersion;
            break;
        case SQL_HANDLE_DESC :
            sDesc = (zldDesc*)aHandle;
            
            STL_TRY_THROW( sDesc->mType == SQL_HANDLE_DESC,
                           RAMP_ERR_INVALID_HANDLE );
            
            sDiag = &sDesc->mDiag;
            sCharacterSet = ZLS_STMT_DBC(sDesc->mStmt)->mCharacterSet;
            sODBCVer = sDesc->mStmt->mParentDbc->mParentEnv->mAttrVersion;
            break;
        default :
            STL_THROW( RAMP_ERR_INVALID_HANDLE );
            break;
    }

    STL_TRY_THROW( aRecNumber <= sDiag->mNumber, RAMP_NO_DATA );

    STL_TRY( stlMarkRegionMem( &sDiag->mRegionMem,
                               &sMark,
                               &sErrorStack ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( zldDiagGetRecW( sDiag,
                             aRecNumber,
                             sCharacterSet,
                             aSQLState,
                             &sNativeError,
                             aMessageText,
                             aBufferLength,
                             &sTextLen,
                             &sErrorStack ) == STL_SUCCESS );

    if( sODBCVer == SQL_OV_ODBC2 )
    {
        if( aSQLState != NULL )
        {
            if( (aSQLState[0] == 'H') && (aSQLState[1] == 'Y') )
            {
                aSQLState[0] = 'S';
                aSQLState[1] = '1';
            }
        }
    }

    if( aNativeErrorPtr != NULL )
    {
        *aNativeErrorPtr = sNativeError;
    }

    if( aTextLengthPtr != NULL )
    {
        *aTextLengthPtr = sTextLen;
    }

    if( aBufferLength > sTextLen )
    {
        sRet = SQL_SUCCESS;
    }
    else
    {
        sRet = SQL_SUCCESS_WITH_INFO;
    }
    
    sState = 0;
    STL_TRY( stlRestoreRegionMem( &sDiag->mRegionMem,
                                  &sMark,
                                  STL_TRUE,
                                  &sErrorStack ) == STL_SUCCESS );

    return sRet;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }
    
    STL_CATCH( RAMP_NO_DATA )
    {
        sRet = SQL_NO_DATA;
    }

    STL_FINISH;

    switch( sState )
    {
        case 1 :
            (void)stlRestoreRegionMem( &sDiag->mRegionMem, &sMark, STL_TRUE, &sErrorStack );
        default :
            break;
    }

    return sRet;
}

SQLRETURN SQL_API SQLGetEnvAttr( SQLHENV      aEnvironmentHandle,
                                 SQLINTEGER   aAttribute,
                                 SQLPOINTER   aValuePtr,
                                 SQLINTEGER   aBufferLength,
                                 SQLINTEGER * aStringLengthPtr )
{
    SQLRETURN       sRet          = SQL_ERROR;
    zlnEnv        * sEnv          = NULL;
    stlInt32        sStringLength = 0;
    stlErrorData  * sErrorData    = NULL;
    stlErrorStack   sErrorStack;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sEnv = (zlnEnv*)aEnvironmentHandle;

    STL_TRY_THROW( sEnv != NULL, RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sEnv->mType == SQL_HANDLE_ENV,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( zldDiagClear( SQL_HANDLE_ENV,
                           aEnvironmentHandle,
                           &sErrorStack ) == STL_SUCCESS );

    switch( sEnv->mAttrVersion )
    {
        case SQL_OV_ODBC2 :
        case SQL_OV_ODBC3 :
        case SQL_OV_ODBC3_80 :
            break;
        default :
            STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR );
            break;
    }

    switch( aAttribute )
    {
        case SQL_ATTR_CONNECTION_POOLING :
        case SQL_ATTR_CP_MATCH :
        case SQL_ATTR_OUTPUT_NTS :
            STL_THROW( RAMP_ERR_OPTIONAL_FEATURE_NOT_IMPLEMENTED );
            break;
        case SQL_ATTR_ODBC_VERSION :
            break;
        default:
            STL_THROW( RAMP_ERR_INVALID_ATTRIBUTE_OPTION_IDENTIFIER );
            break;
    }

    STL_TRY( zlnGetAttr( sEnv,
                         aAttribute,
                         aValuePtr,
                         aBufferLength,
                         &sStringLength,
                         &sErrorStack ) == STL_SUCCESS );

    if( aStringLengthPtr != NULL )
    {
        *aStringLengthPtr = sStringLength;
    }

    sRet = SQL_SUCCESS;

    zldDiagSetRetCode( SQL_HANDLE_ENV, aEnvironmentHandle, sRet );
    
    return sRet;

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      "SQL_ATTR_ODBC_VERSION has not yet been set via SQLSetEnvAttr. "
                      "You do not need to set SQL_ATTR_ODBC_VERSION explicitly "
                      "if you are using SQLAllocHandleStd.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_OPTIONAL_FEATURE_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_OPTIONAL_FEATURE_NOT_IMPLEMENTED,
                      "The value specified for the argument Attribute was "
                      "a valid ODBC environment attribute for the version of ODBC "
                      "supported by the driver, but was not supported by the driver.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_ATTRIBUTE_OPTION_IDENTIFIER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_ATTRIBUTE_OPTION_IDENTIFIER,
                      "The value specified for the argument Attribute was not valid "
                      "for the version of ODBC supported by the driver.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_FINISH;

    if( sRet == SQL_ERROR )
    {
        zldDiagSetRetCode( SQL_HANDLE_ENV, aEnvironmentHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );
        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_ENV,
                               aEnvironmentHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }
    }

    return sRet;
}

SQLRETURN SQL_API SQLGetFunctions( SQLHDBC        aConnectionHandle,
                                   SQLUSMALLINT   aFunctionId,
                                   SQLUSMALLINT * aSupportedPtr )
{
    SQLRETURN       sRet        = SQL_ERROR;
    zlcDbc        * sDbc        = NULL;
    stlErrorData  * sErrorData  = NULL;
    stlErrorStack   sErrorStack;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sDbc = (zlcDbc*)aConnectionHandle;

    STL_TRY_THROW( sDbc != NULL, RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sDbc->mType == SQL_HANDLE_DBC,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( zldDiagClear( SQL_HANDLE_DBC,
                           aConnectionHandle,
                           &sErrorStack ) == STL_SUCCESS );

    switch( sDbc->mTransition )
    {
        case ZLC_TRANSITION_STATE_C0 :
        case ZLC_TRANSITION_STATE_C1 :
            STL_THROW( RAMP_ERR_INVALID_HANDLE );
            break;
        case ZLC_TRANSITION_STATE_C2 :
        case ZLC_TRANSITION_STATE_C3 :
            STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR );
            break;
        default :
            break;
    }

    STL_TRY( zlfGetFunctions( sDbc,
                              aFunctionId,
                              aSupportedPtr,
                              &sErrorStack ) == STL_SUCCESS );

    sRet = SQL_SUCCESS;
    
    zldDiagSetRetCode( SQL_HANDLE_DBC, aConnectionHandle, sRet );

    return sRet;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      "SQLGetFunctions was called before "
                      "SQLConnect, SQLBrowseConnect, or SQLDriverConnect.",
                      &sErrorStack );
    }

    STL_FINISH;

    if( sRet == SQL_ERROR )
    {
        zldDiagSetRetCode( SQL_HANDLE_DBC, aConnectionHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_DBC,
                               aConnectionHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }
        
        zlcCheckSessionAndCleanup( sDbc );
    }

    return sRet;
}

static SQLRETURN zllGetInfo( SQLHDBC         aConnectionHandle,
                             SQLUSMALLINT    aInfoType,
                             SQLPOINTER      aInfoValuePtr,
                             SQLSMALLINT     aBufferLength,
                             SQLSMALLINT   * aStringLengthPtr,
                             stlErrorStack * aErrorStack )
{
    zlcDbc    * sDbc          = NULL;
    SQLRETURN   sRet          = SQL_ERROR;
    stlInt16    sStringLength = 0;

    sDbc = (zlcDbc*)aConnectionHandle;

    STL_TRY( zldDiagClear( SQL_HANDLE_DBC,
                           aConnectionHandle,
                           aErrorStack ) == STL_SUCCESS );

    switch( sDbc->mTransition )
    {
        case ZLC_TRANSITION_STATE_C0 :
        case ZLC_TRANSITION_STATE_C1 :
            STL_THROW( RAMP_ERR_INVALID_HANDLE );
            break;
        case ZLC_TRANSITION_STATE_C2 :
            switch( aInfoType )
            {
                case SQL_ODBC_VER :
                case SQL_DRIVER_ODBC_VER :
                    break;
                default :
                    STL_THROW( RAMP_ERR_CONNECTION_NOT_OPEN );
                    break;
            }
            break;
        case ZLC_TRANSITION_STATE_C3 :
            STL_THROW( RAMP_ERR_CONNECTION_NOT_OPEN );
            break;
        case ZLC_TRANSITION_STATE_C4 :
        case ZLC_TRANSITION_STATE_C5 :
        case ZLC_TRANSITION_STATE_C6 :
            STL_TRY( zlcCheckSession( sDbc, aErrorStack ) == STL_SUCCESS );
            break;
    }

    STL_TRY_THROW( aBufferLength >= 0, RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH );

    switch( aInfoType )
    {
        case SQL_ACCESSIBLE_PROCEDURES :
        case SQL_ACCESSIBLE_TABLES :
        case SQL_ACTIVE_ENVIRONMENTS :
        case SQL_AGGREGATE_FUNCTIONS :
        case SQL_ALTER_DOMAIN :
        case SQL_ALTER_TABLE :
        case SQL_ASYNC_DBC_FUNCTIONS :
        case SQL_ASYNC_MODE :
        case SQL_BATCH_ROW_COUNT :
        case SQL_BATCH_SUPPORT :
        case SQL_BOOKMARK_PERSISTENCE :
        case SQL_CATALOG_LOCATION :
        case SQL_CATALOG_NAME :
        case SQL_CATALOG_NAME_SEPARATOR :
        case SQL_CATALOG_TERM :
        case SQL_CATALOG_USAGE :
        case SQL_COLLATION_SEQ :
        case SQL_COLUMN_ALIAS :
        case SQL_CONCAT_NULL_BEHAVIOR :
        case SQL_CONVERT_BIGINT :
        case SQL_CONVERT_BINARY :
        case SQL_CONVERT_BIT :
        case SQL_CONVERT_CHAR :
        case SQL_CONVERT_GUID :
        case SQL_CONVERT_DATE :
        case SQL_CONVERT_DECIMAL :
        case SQL_CONVERT_DOUBLE :
        case SQL_CONVERT_FLOAT :
        case SQL_CONVERT_INTEGER :
        case SQL_CONVERT_INTERVAL_YEAR_MONTH :
        case SQL_CONVERT_INTERVAL_DAY_TIME :
        case SQL_CONVERT_LONGVARBINARY :
        case SQL_CONVERT_LONGVARCHAR :
        case SQL_CONVERT_NUMERIC :
        case SQL_CONVERT_REAL :
        case SQL_CONVERT_SMALLINT :
        case SQL_CONVERT_TIME :
        case SQL_CONVERT_TIMESTAMP :
        case SQL_CONVERT_TINYINT :
        case SQL_CONVERT_VARBINARY :
        case SQL_CONVERT_VARCHAR :
        case SQL_CONVERT_FUNCTIONS :
        case SQL_CORRELATION_NAME :
        case SQL_CREATE_ASSERTION :
        case SQL_CREATE_CHARACTER_SET :
        case SQL_CREATE_COLLATION :
        case SQL_CREATE_DOMAIN :
        case SQL_CREATE_SCHEMA :
        case SQL_CREATE_TABLE :
        case SQL_CREATE_TRANSLATION :
        case SQL_CREATE_VIEW :
        case SQL_CURSOR_COMMIT_BEHAVIOR :
        case SQL_CURSOR_ROLLBACK_BEHAVIOR :
        case SQL_CURSOR_SENSITIVITY :
        case SQL_DATA_SOURCE_NAME :
        case SQL_DATA_SOURCE_READ_ONLY :
        case SQL_DATABASE_NAME :
        case SQL_DATETIME_LITERALS :
        case SQL_DBMS_NAME :
        case SQL_DBMS_VER :
        case SQL_DDL_INDEX :
        case SQL_DEFAULT_TXN_ISOLATION :
        case SQL_DESCRIBE_PARAMETER :
        case SQL_DM_VER :
        case SQL_DRIVER_HDBC :
        case SQL_DRIVER_HENV :
        case SQL_DRIVER_HDESC :
        case SQL_DRIVER_HLIB :
        case SQL_DRIVER_HSTMT :
        case SQL_DRIVER_NAME :
        case SQL_DRIVER_ODBC_VER :
        case SQL_DRIVER_VER :
        case SQL_DROP_ASSERTION :
        case SQL_DROP_CHARACTER_SET :
        case SQL_DROP_COLLATION :
        case SQL_DROP_DOMAIN :
        case SQL_DROP_SCHEMA :
        case SQL_DROP_TABLE :
        case SQL_DROP_TRANSLATION :
        case SQL_DROP_VIEW :
        case SQL_DYNAMIC_CURSOR_ATTRIBUTES1 :
        case SQL_DYNAMIC_CURSOR_ATTRIBUTES2 :
        case SQL_EXPRESSIONS_IN_ORDERBY :
        case SQL_FILE_USAGE :
        case SQL_FORWARD_ONLY_CURSOR_ATTRIBUTES1 :
        case SQL_FORWARD_ONLY_CURSOR_ATTRIBUTES2 :
        case SQL_GETDATA_EXTENSIONS :
        case SQL_GROUP_BY :
        case SQL_IDENTIFIER_CASE :
        case SQL_IDENTIFIER_QUOTE_CHAR :
        case SQL_INDEX_KEYWORDS :
        case SQL_INFO_SCHEMA_VIEWS :
        case SQL_INSERT_STATEMENT :
        case SQL_INTEGRITY :
        case SQL_KEYSET_CURSOR_ATTRIBUTES1 :
        case SQL_KEYSET_CURSOR_ATTRIBUTES2 :
        case SQL_KEYWORDS :
        case SQL_LIKE_ESCAPE_CLAUSE :
        case SQL_MAX_ASYNC_CONCURRENT_STATEMENTS :
        case SQL_MAX_BINARY_LITERAL_LEN :
        case SQL_MAX_CATALOG_NAME_LEN :
        case SQL_MAX_CHAR_LITERAL_LEN :
        case SQL_MAX_COLUMN_NAME_LEN :
        case SQL_MAX_COLUMNS_IN_GROUP_BY :
        case SQL_MAX_COLUMNS_IN_INDEX :
        case SQL_MAX_COLUMNS_IN_ORDER_BY :
        case SQL_MAX_COLUMNS_IN_SELECT :
        case SQL_MAX_COLUMNS_IN_TABLE :
        case SQL_MAX_CONCURRENT_ACTIVITIES :
        case SQL_MAX_CURSOR_NAME_LEN :
        case SQL_MAX_DRIVER_CONNECTIONS :
        case SQL_MAX_IDENTIFIER_LEN :
        case SQL_MAX_INDEX_SIZE :
        case SQL_MAX_PROCEDURE_NAME_LEN :
        case SQL_MAX_ROW_SIZE :
        case SQL_MAX_ROW_SIZE_INCLUDES_LONG :
        case SQL_MAX_SCHEMA_NAME_LEN :
        case SQL_MAX_STATEMENT_LEN :
        case SQL_MAX_TABLE_NAME_LEN :
        case SQL_MAX_TABLES_IN_SELECT :
        case SQL_MAX_USER_NAME_LEN :
        case SQL_MULT_RESULT_SETS :
        case SQL_MULTIPLE_ACTIVE_TXN :
        case SQL_NEED_LONG_DATA_LEN :
        case SQL_NON_NULLABLE_COLUMNS :
        case SQL_NULL_COLLATION :
        case SQL_NUMERIC_FUNCTIONS :
        case SQL_ODBC_INTERFACE_CONFORMANCE :
        case SQL_ODBC_VER :
        case SQL_OJ_CAPABILITIES :
        case SQL_OUTER_JOINS :
        case SQL_ORDER_BY_COLUMNS_IN_SELECT :
        case SQL_PARAM_ARRAY_ROW_COUNTS :
        case SQL_PARAM_ARRAY_SELECTS :
        case SQL_PROCEDURE_TERM :
        case SQL_PROCEDURES :
        case SQL_QUOTED_IDENTIFIER_CASE :
        case SQL_ROW_UPDATES :
        case SQL_SCHEMA_TERM :
        case SQL_SCHEMA_USAGE :
        case SQL_SCROLL_OPTIONS :
        case SQL_SEARCH_PATTERN_ESCAPE :
        case SQL_SERVER_NAME :
        case SQL_SPECIAL_CHARACTERS :
        case SQL_SQL_CONFORMANCE :
        case SQL_SQL92_DATETIME_FUNCTIONS :
        case SQL_SQL92_FOREIGN_KEY_DELETE_RULE :
        case SQL_SQL92_FOREIGN_KEY_UPDATE_RULE :
        case SQL_SQL92_GRANT :
        case SQL_SQL92_NUMERIC_VALUE_FUNCTIONS :
        case SQL_SQL92_PREDICATES :
        case SQL_SQL92_RELATIONAL_JOIN_OPERATORS :
        case SQL_SQL92_REVOKE :
        case SQL_SQL92_ROW_VALUE_CONSTRUCTOR :
        case SQL_SQL92_STRING_FUNCTIONS :
        case SQL_SQL92_VALUE_EXPRESSIONS :
        case SQL_STANDARD_CLI_CONFORMANCE :
        case SQL_STATIC_CURSOR_ATTRIBUTES1 :
        case SQL_STATIC_CURSOR_ATTRIBUTES2 :
        case SQL_STRING_FUNCTIONS :
        case SQL_SUBQUERIES :
        case SQL_SYSTEM_FUNCTIONS :
        case SQL_TABLE_TERM :
        case SQL_TIMEDATE_ADD_INTERVALS :
        case SQL_TIMEDATE_DIFF_INTERVALS :
        case SQL_TIMEDATE_FUNCTIONS :
        case SQL_TXN_CAPABLE :
        case SQL_TXN_ISOLATION_OPTION :
        case SQL_UNION :
        case SQL_USER_NAME :
        case SQL_XOPEN_CLI_YEAR :
            break;

        /* ODBC deprecated */
        case SQL_FETCH_DIRECTION :
        case SQL_LOCK_TYPES :
        case SQL_ODBC_API_CONFORMANCE :
        case SQL_ODBC_SQL_CONFORMANCE :
        case SQL_POS_OPERATIONS :
        case SQL_POSITIONED_STATEMENTS :
        case SQL_SCROLL_CONCURRENCY :
        case SQL_STATIC_SENSITIVITY :
            break;
        default :
            STL_THROW( RAMP_ERR_INFORMATION_TYPE_OUT_OF_RANGE );
            break;
    }

    STL_TRY( zlfGetInfo( sDbc,
                         aInfoType,
                         aInfoValuePtr,
                         aBufferLength,
                         &sStringLength,
                         aErrorStack ) == STL_SUCCESS );
                          
    switch( aInfoType )
    {
        case SQL_ACCESSIBLE_PROCEDURES :
        case SQL_ACCESSIBLE_TABLES :
        case SQL_CATALOG_NAME :
        case SQL_CATALOG_NAME_SEPARATOR :
        case SQL_CATALOG_TERM :
        case SQL_COLLATION_SEQ :
        case SQL_COLUMN_ALIAS :
        case SQL_DATA_SOURCE_NAME :
        case SQL_DATA_SOURCE_READ_ONLY :
        case SQL_DATABASE_NAME :
        case SQL_DM_VER :
        case SQL_DBMS_NAME :
        case SQL_DBMS_VER :
        case SQL_DESCRIBE_PARAMETER :
        case SQL_DRIVER_NAME :
        case SQL_DRIVER_ODBC_VER :
        case SQL_DRIVER_VER :
        case SQL_EXPRESSIONS_IN_ORDERBY :
        case SQL_IDENTIFIER_QUOTE_CHAR :
        case SQL_INTEGRITY :
        case SQL_KEYWORDS :
        case SQL_LIKE_ESCAPE_CLAUSE :
        case SQL_MAX_ROW_SIZE_INCLUDES_LONG :
        case SQL_MULT_RESULT_SETS :
        case SQL_MULTIPLE_ACTIVE_TXN :
        case SQL_NEED_LONG_DATA_LEN :
        case SQL_ODBC_VER :
        case SQL_ORDER_BY_COLUMNS_IN_SELECT :
        case SQL_OUTER_JOINS :
        case SQL_PROCEDURE_TERM :
        case SQL_PROCEDURES :
        case SQL_ROW_UPDATES :
        case SQL_SCHEMA_TERM :
        case SQL_SEARCH_PATTERN_ESCAPE :
        case SQL_SERVER_NAME :
        case SQL_SPECIAL_CHARACTERS :
        case SQL_TABLE_TERM :
        case SQL_USER_NAME :
        case SQL_XOPEN_CLI_YEAR :
            if( aStringLengthPtr != NULL )
            {
                *aStringLengthPtr = sStringLength;
            }

            STL_TRY_THROW( aBufferLength > sStringLength,
                           RAMP_WARNING_STRING_DATA_RIGHT_TRUNCATED );
            break;
        default :
            break;
    }

    sRet = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_CATCH( RAMP_ERR_CONNECTION_NOT_OPEN )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_CONNECTION_NOT_OPEN,
                      "The type of information requested in InfoType requires "
                      "an open connection. Of the information types reserved by ODBC, "
                      "only SQL_ODBC_VER can be returned without an open connection.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_STRING_OR_BUFFER_LENGTH,
                      "The value specified for argument BufferLength was less than 0.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INFORMATION_TYPE_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INFORMATION_TYPE_OUT_OF_RANGE,
                      "The value specified for the argument InfoType was not valid "
                      "for the version of ODBC supported by the driver.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_WARNING_STRING_DATA_RIGHT_TRUNCATED )
    {
        stlPushError( STL_ERROR_LEVEL_WARNING,
                      ZLE_ERRCODE_WARNING_STRING_DATA_RIGHT_TRUNCATED,
                      "The buffer *InfoValuePtr was not large enough to return all the "
                      "requested information. Therefore, the information was truncated. "
                      "The length of the requested information in its untruncated form is "
                      "returned in *StringLengthPtr.",
                      aErrorStack );

        sRet = SQL_SUCCESS_WITH_INFO;
    }

    STL_FINISH;

    return sRet;
}

SQLRETURN SQL_API SQLGetInfo( SQLHDBC        aConnectionHandle,
                              SQLUSMALLINT   aInfoType,
                              SQLPOINTER     aInfoValuePtr,
                              SQLSMALLINT    aBufferLength,
                              SQLSMALLINT  * aStringLengthPtr )
{
    SQLRETURN       sRet = SQL_ERROR;
    zlcDbc        * sDbc = NULL;
    stlErrorStack   sErrorStack;
    stlErrorData  * sErrorData = NULL;
    stlRegionMark   sMark;
    stlInt32        sState = 0;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sDbc = (zlcDbc*)aConnectionHandle;
    
    STL_TRY_THROW( sDbc != NULL, RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sDbc->mType == SQL_HANDLE_DBC,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( stlMarkRegionMem( &sDbc->mAllocator,
                               &sMark,
                               &sErrorStack ) == STL_SUCCESS );
    sState = 1;

    sRet = zllGetInfo( aConnectionHandle,
                       aInfoType,
                       aInfoValuePtr,
                       aBufferLength,
                       aStringLengthPtr,
                       &sErrorStack );

    STL_TRY( sRet == SQL_SUCCESS );

    sState = 0;
    STL_TRY( stlRestoreRegionMem( &sDbc->mAllocator,
                                  &sMark,
                                  STL_TRUE,
                                  &sErrorStack ) == STL_SUCCESS );

    zldDiagSetRetCode( SQL_HANDLE_DBC, aConnectionHandle, sRet );

    return SQL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_FINISH;

    if( ( sRet == SQL_ERROR ) ||
        ( sRet == SQL_SUCCESS_WITH_INFO ) )
    {
        zldDiagSetRetCode( SQL_HANDLE_DBC, aConnectionHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_DBC,
                               aConnectionHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }
        
        zlcCheckSessionAndCleanup( sDbc );
    }

    switch( sState )
    {
        case 1 :
            (void)stlRestoreRegionMem( &sDbc->mAllocator, &sMark, STL_TRUE, &sErrorStack );
        default :
            break;
    }

    return sRet;
}

SQLRETURN SQL_API SQLGetInfoW( SQLHDBC        aConnectionHandle,
                               SQLUSMALLINT   aInfoType,
                               SQLPOINTER     aInfoValuePtr,
                               SQLSMALLINT    aBufferLength,
                               SQLSMALLINT  * aStringLengthPtr )
{
    /**
     * aBufferLength
     *     BufferLength should contain a count of bytes.
     * aStringLengthPtr
     *     Pointer to a buffer in which to return the total number of bytes
     *     (excluding the null-termination character for character data)
     *     available to return in *InfoValuePtr.
     */

    SQLRETURN       sRet = SQL_ERROR;
    zlcDbc        * sDbc = NULL;
    stlErrorStack   sErrorStack;
    stlErrorData  * sErrorData = NULL;
    stlRegionMark   sMark;
    stlInt32        sState = 0;

    SQLCHAR         sCharacterAttribute[1024 + 1];
    SQLSMALLINT     sStringLength = 0;
    stlInt64        sBufferLength = 0;
    stlInt64        i = 0;
    
    STL_INIT_ERROR_STACK( &sErrorStack );
    
    sDbc = (zlcDbc*)aConnectionHandle;
    
    STL_TRY_THROW( sDbc != NULL, RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sDbc->mType == SQL_HANDLE_DBC,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( stlMarkRegionMem( &sDbc->mAllocator,
                               &sMark,
                               &sErrorStack ) == STL_SUCCESS );
    sState = 1;

    switch( aInfoType )
    {
        case SQL_ACCESSIBLE_PROCEDURES :
        case SQL_ACCESSIBLE_TABLES :
        case SQL_CATALOG_NAME :
        case SQL_CATALOG_NAME_SEPARATOR :
        case SQL_CATALOG_TERM :
        case SQL_COLLATION_SEQ :
        case SQL_COLUMN_ALIAS :
        case SQL_DATA_SOURCE_NAME :
        case SQL_DATA_SOURCE_READ_ONLY :
        case SQL_DATABASE_NAME :
        case SQL_DM_VER :
        case SQL_DBMS_NAME :
        case SQL_DBMS_VER :
        case SQL_DESCRIBE_PARAMETER :
        case SQL_DRIVER_NAME :
        case SQL_DRIVER_ODBC_VER :
        case SQL_DRIVER_VER :
        case SQL_EXPRESSIONS_IN_ORDERBY :
        case SQL_IDENTIFIER_QUOTE_CHAR :
        case SQL_INTEGRITY :
        case SQL_KEYWORDS :
        case SQL_LIKE_ESCAPE_CLAUSE :
        case SQL_MAX_ROW_SIZE_INCLUDES_LONG :
        case SQL_MULT_RESULT_SETS :
        case SQL_MULTIPLE_ACTIVE_TXN :
        case SQL_NEED_LONG_DATA_LEN :
        case SQL_ODBC_VER :
        case SQL_ORDER_BY_COLUMNS_IN_SELECT :
        case SQL_OUTER_JOINS :
        case SQL_PROCEDURE_TERM :
        case SQL_PROCEDURES :
        case SQL_ROW_UPDATES :
        case SQL_SCHEMA_TERM :
        case SQL_SEARCH_PATTERN_ESCAPE :
        case SQL_SERVER_NAME :
        case SQL_SPECIAL_CHARACTERS :
        case SQL_TABLE_TERM :
        case SQL_USER_NAME :
        case SQL_XOPEN_CLI_YEAR :
            STL_TRY_THROW( aBufferLength >= 0, RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH );

            sRet = zllGetInfo( aConnectionHandle,
                               aInfoType,
                               (SQLPOINTER)sCharacterAttribute,
                               STL_SIZEOF(sCharacterAttribute),
                               &sStringLength,
                               &sErrorStack );

            STL_TRY( SQL_SUCCEEDED(sRet) );

            if( aStringLengthPtr != NULL )
            {
                *aStringLengthPtr = sStringLength * STL_SIZEOF(SQLWCHAR);
            }

            if( aInfoValuePtr != NULL )
            {
                sBufferLength = (aBufferLength / (stlInt64)STL_SIZEOF(SQLWCHAR)) - 1;

                if( sStringLength <= sBufferLength )
                {
                    for( i = 0; i < sStringLength; i++ )
                    {
                        ((SQLWCHAR*)aInfoValuePtr)[i] = sCharacterAttribute[i];
                    }

                    ((SQLWCHAR*)aInfoValuePtr)[i] = 0;
                }
                else
                {
                    for( i = 0; i < sBufferLength; i++ )
                    {
                        ((SQLWCHAR*)aInfoValuePtr)[i] = sCharacterAttribute[i];
                    }

                    if( sBufferLength >= 0 )
                    {
                        ((SQLWCHAR*)aInfoValuePtr)[i] = 0;
                    }

                    STL_THROW( RAMP_WARNING_STRING_DATA_RIGHT_TRUNCATED );
                }
            }
            break;
        default :
            sRet = zllGetInfo( aConnectionHandle,
                               aInfoType,
                               aInfoValuePtr,
                               aBufferLength,
                               aStringLengthPtr,
                               &sErrorStack );
            break;
    }

    STL_TRY( SQL_SUCCEEDED(sRet) );

    sState = 0;
    STL_TRY( stlRestoreRegionMem( &sDbc->mAllocator,
                                  &sMark,
                                  STL_TRUE,
                                  &sErrorStack ) == STL_SUCCESS );

    zldDiagSetRetCode( SQL_HANDLE_DBC, aConnectionHandle, sRet );

    return SQL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_CATCH( RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_STRING_OR_BUFFER_LENGTH,
                      "The value specified for argument BufferLength was less than 0.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_WARNING_STRING_DATA_RIGHT_TRUNCATED )
    {
        stlPushError( STL_ERROR_LEVEL_WARNING,
                      ZLE_ERRCODE_WARNING_STRING_DATA_RIGHT_TRUNCATED,
                      "The buffer *InfoValuePtr was not large enough to return all the "
                      "requested information. Therefore, the information was truncated. "
                      "The length of the requested information in its untruncated form is "
                      "returned in *StringLengthPtr.",
                      &sErrorStack );

        sRet = SQL_SUCCESS_WITH_INFO;
    }

    STL_FINISH;

    if( ( sRet == SQL_ERROR ) ||
        ( sRet == SQL_SUCCESS_WITH_INFO ) )
    {
        zldDiagSetRetCode( SQL_HANDLE_DBC, aConnectionHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_DBC,
                               aConnectionHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }
    }

    switch( sState )
    {
        case 1 :
            (void)stlRestoreRegionMem( &sDbc->mAllocator, &sMark, STL_TRUE, &sErrorStack );
        default :
            break;
    }

    return sRet;
}

SQLRETURN SQL_API SQLGetStmtAttr( SQLHSTMT     aStatementHandle,
                                  SQLINTEGER   aAttribute,
                                  SQLPOINTER   aValuePtr,
                                  SQLINTEGER   aBufferLength,
                                  SQLINTEGER * aStringLengthPtr )
{
    SQLRETURN       sRet          = SQL_ERROR;
    zlsStmt       * sStmt         = NULL;
    stlInt32        sStringLength = 0;
    stlErrorData  * sErrorData    = NULL;
    stlErrorStack   sErrorStack;
    stlRegionMark   sMark;
    stlInt32        sState = 0;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sStmt = (zlsStmt*)aStatementHandle;

    STL_TRY_THROW( sStmt != NULL,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sStmt->mType == SQL_HANDLE_STMT,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( zldDiagClear( SQL_HANDLE_STMT,
                           aStatementHandle,
                           &sErrorStack ) == STL_SUCCESS );

    STL_TRY( zlcCheckSession( sStmt->mParentDbc,
                              &sErrorStack ) == STL_SUCCESS );
    
    switch( sStmt->mTransition )
    {
        case ZLS_TRANSITION_STATE_S0 :
            STL_THROW( RAMP_ERR_INVALID_HANDLE );
            break;
        case ZLS_TRANSITION_STATE_S1 :
        case ZLS_TRANSITION_STATE_S2 :
        case ZLS_TRANSITION_STATE_S3 :
        case ZLS_TRANSITION_STATE_S4 :
        case ZLS_TRANSITION_STATE_S5 :
            STL_TRY_THROW( aAttribute != SQL_ATTR_ROW_NUMBER,
                           RAMP_ERR_INVALID_CURSOR_STATE );
            break;
        case ZLS_TRANSITION_STATE_S6 :
        case ZLS_TRANSITION_STATE_S7 :
            /**
             * @todo valid row 정보를 추가해야 한다.
             */
            break;
        case ZLS_TRANSITION_STATE_S8 :
        case ZLS_TRANSITION_STATE_S9 :
        case ZLS_TRANSITION_STATE_S10 :
        case ZLS_TRANSITION_STATE_S11 :
        case ZLS_TRANSITION_STATE_S12 :
            STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR );
            break;
    }

    switch( aAttribute )
    {
        case SQL_ATTR_APP_PARAM_DESC :
        case SQL_ATTR_APP_ROW_DESC :
        case SQL_ATTR_CONCURRENCY :
        case SQL_ATTR_CURSOR_HOLDABLE :
        case SQL_ATTR_CURSOR_SCROLLABLE :
        case SQL_ATTR_CURSOR_SENSITIVITY :
        case SQL_ATTR_CURSOR_TYPE :
        case SQL_ATTR_IMP_PARAM_DESC :
        case SQL_ATTR_IMP_ROW_DESC :
        case SQL_ATTR_METADATA_ID :
        case SQL_ATTR_MAX_ROWS :
        case SQL_ATTR_PARAM_BIND_OFFSET_PTR :
        case SQL_ATTR_PARAM_BIND_TYPE :
        case SQL_ATTR_PARAM_OPERATION_PTR :
        case SQL_ATTR_PARAM_STATUS_PTR :
        case SQL_ATTR_PARAMS_PROCESSED_PTR  :
        case SQL_ATTR_PARAMSET_SIZE :
        case SQL_ATTR_QUERY_TIMEOUT :
        case SQL_ATTR_ROW_ARRAY_SIZE :
        case SQL_ATTR_ROW_BIND_OFFSET_PTR :
        case SQL_ATTR_ROW_BIND_TYPE :
        case SQL_ATTR_ROW_NUMBER :
        case SQL_ATTR_ROW_STATUS_PTR :
        case SQL_ATTR_ROWS_FETCHED_PTR :
        case SQL_ATTR_EXPLAIN_PLAN_OPTION :
        case SQL_ATTR_EXPLAIN_PLAN_TEXT :
        case SQL_ATTR_ATOMIC_EXECUTION :
        case SQL_ATTR_PREFETCH_ROWS :
        case SQL_ROWSET_SIZE :
            break;
        case SQL_ATTR_ASYNC_ENABLE :
        case SQL_ATTR_ENABLE_AUTO_IPD  :
        case SQL_ATTR_FETCH_BOOKMARK_PTR :
        case SQL_ATTR_KEYSET_SIZE :
        case SQL_ATTR_MAX_LENGTH :
        case SQL_ATTR_NOSCAN :
        case SQL_ATTR_RETRIEVE_DATA :
        case SQL_ATTR_ROW_OPERATION_PTR  :
        case SQL_ATTR_SIMULATE_CURSOR :
        case SQL_ATTR_USE_BOOKMARKS :
            STL_THROW( RAMP_ERR_OPTIONAL_FEATURE_NOT_IMPLEMENTED );
            break;
        default :
            STL_THROW( RAMP_ERR_INVALID_ATTRIBUTE_OPTION_IDENTIFIER );
            break;
    }

    STL_TRY( stlMarkRegionMem( &sStmt->mAllocator,
                               &sMark,
                               &sErrorStack ) == STL_SUCCESS );
    sState = 1;

    STL_TRY( zlsGetAttr( sStmt,
                         aAttribute,
                         aValuePtr,
                         aBufferLength,
                         &sStringLength,
                         &sErrorStack ) == STL_SUCCESS );

    switch( aAttribute )
    {
        case SQL_ATTR_EXPLAIN_PLAN_TEXT :
            if( aStringLengthPtr != NULL )
            {
                *aStringLengthPtr = sStringLength;
        
                STL_TRY_THROW( aBufferLength > sStringLength,
                               RAMP_WARNING_STRING_DATA_RIGHT_TRUNCATED );
            }
            break;
        default :
            break;
    }

    sState = 0;
    STL_TRY( stlRestoreRegionMem( &sStmt->mAllocator,
                                  &sMark,
                                  STL_TRUE,
                                  &sErrorStack ) == STL_SUCCESS );

    sRet = SQL_SUCCESS;

    zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

    return sRet;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_CATCH( RAMP_ERR_INVALID_CURSOR_STATE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CURSOR_STATE,
                      "The argument Attribute was SQL_ATTR_ROW_NUMBER and "
                      "the cursor was not open, or the cursor was positioned "
                      "before the start of the result set or after the end of "
                      "the result set.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      NULL,
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_ATTRIBUTE_OPTION_IDENTIFIER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_ATTRIBUTE_OPTION_IDENTIFIER,
                      "The value specified for the argument Attribute was not "
                      "valid for the version of ODBC supported by the driver.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_OPTIONAL_FEATURE_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_OPTIONAL_FEATURE_NOT_IMPLEMENTED,
                      "The value specified for the argument Attribute was a valid "
                      "ODBC connection or statement attribute for the version of "
                      "ODBC supported by the driver but was not supported by the driver.",
                      &sErrorStack );
    }
    
    STL_CATCH( RAMP_WARNING_STRING_DATA_RIGHT_TRUNCATED )
    {
        stlPushError( STL_ERROR_LEVEL_WARNING,
                      ZLE_ERRCODE_WARNING_STRING_DATA_RIGHT_TRUNCATED,
                      "String or binary data returned for a column resulted "
                      "in the truncation of nonblank character or non-NULL binary data. "
                      "If it was a string value, it was right-truncated.",
                      &sErrorStack );

        sRet = SQL_SUCCESS_WITH_INFO;
    }
    
    STL_FINISH;

    if( ( sRet == SQL_ERROR ) || ( sRet == SQL_SUCCESS_WITH_INFO ) )
    {
        zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_STMT,
                               aStatementHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }
        
        if( sStmt != NULL )
        {
            zlcCheckSessionAndCleanup( ZLS_STMT_DBC(sStmt) );
        }
    }

    switch( sState )
    {
        case 1 :
            stlRestoreRegionMem( &sStmt->mAllocator, &sMark, STL_TRUE, &sErrorStack );
        default :
            break;
    }

    return sRet;
}

SQLRETURN SQL_API SQLGetStmtAttrW( SQLHSTMT     aStatementHandle,
                                   SQLINTEGER   aAttribute,
                                   SQLPOINTER   aValuePtr,
                                   SQLINTEGER   aBufferLength,
                                   SQLINTEGER * aStringLengthPtr )
{
    /**
     * aBufferLength
     *     BufferLength should contain a count of bytes.
     * aStringLengthPtr
     *     A pointer to a buffer in which to return the total number of bytes
     *     (excluding the null-termination character) available to return
     *     in *ValuePtr.
     */

    SQLRETURN       sRet          = SQL_ERROR;
    zlsStmt       * sStmt         = NULL;
    stlInt32        sStringLength = 0;
    stlErrorData  * sErrorData    = NULL;
    stlErrorStack   sErrorStack;
    stlRegionMark   sMark;
    stlInt32        sState = 0;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sStmt = (zlsStmt*)aStatementHandle;

    STL_TRY_THROW( sStmt != NULL,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sStmt->mType == SQL_HANDLE_STMT,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( zldDiagClear( SQL_HANDLE_STMT,
                           aStatementHandle,
                           &sErrorStack ) == STL_SUCCESS );

    STL_TRY( zlcCheckSession( sStmt->mParentDbc,
                              &sErrorStack ) == STL_SUCCESS );
    
    switch( sStmt->mTransition )
    {
        case ZLS_TRANSITION_STATE_S0 :
            STL_THROW( RAMP_ERR_INVALID_HANDLE );
            break;
        case ZLS_TRANSITION_STATE_S1 :
        case ZLS_TRANSITION_STATE_S2 :
        case ZLS_TRANSITION_STATE_S3 :
        case ZLS_TRANSITION_STATE_S4 :
        case ZLS_TRANSITION_STATE_S5 :
            STL_TRY_THROW( aAttribute != SQL_ATTR_ROW_NUMBER,
                           RAMP_ERR_INVALID_CURSOR_STATE );
            break;
        case ZLS_TRANSITION_STATE_S6 :
        case ZLS_TRANSITION_STATE_S7 :
            /**
             * @todo valid row 정보를 추가해야 한다.
             */
            break;
        case ZLS_TRANSITION_STATE_S8 :
        case ZLS_TRANSITION_STATE_S9 :
        case ZLS_TRANSITION_STATE_S10 :
        case ZLS_TRANSITION_STATE_S11 :
        case ZLS_TRANSITION_STATE_S12 :
            STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR );
            break;
    }

    switch( aAttribute )
    {
        case SQL_ATTR_APP_PARAM_DESC :
        case SQL_ATTR_APP_ROW_DESC :
        case SQL_ATTR_CONCURRENCY :
        case SQL_ATTR_CURSOR_HOLDABLE :
        case SQL_ATTR_CURSOR_SCROLLABLE :
        case SQL_ATTR_CURSOR_SENSITIVITY :
        case SQL_ATTR_CURSOR_TYPE :
        case SQL_ATTR_IMP_PARAM_DESC :
        case SQL_ATTR_IMP_ROW_DESC :
        case SQL_ATTR_METADATA_ID :
        case SQL_ATTR_MAX_ROWS :
        case SQL_ATTR_PARAM_BIND_OFFSET_PTR :
        case SQL_ATTR_PARAM_BIND_TYPE :
        case SQL_ATTR_PARAM_OPERATION_PTR :
        case SQL_ATTR_PARAM_STATUS_PTR :
        case SQL_ATTR_PARAMS_PROCESSED_PTR  :
        case SQL_ATTR_PARAMSET_SIZE :
        case SQL_ATTR_QUERY_TIMEOUT :
        case SQL_ATTR_ROW_ARRAY_SIZE :
        case SQL_ATTR_ROW_BIND_OFFSET_PTR :
        case SQL_ATTR_ROW_BIND_TYPE :
        case SQL_ATTR_ROW_NUMBER :
        case SQL_ATTR_ROW_STATUS_PTR :
        case SQL_ATTR_ROWS_FETCHED_PTR :
        case SQL_ATTR_EXPLAIN_PLAN_OPTION :
        case SQL_ATTR_EXPLAIN_PLAN_TEXT :
        case SQL_ATTR_ATOMIC_EXECUTION :
        case SQL_ATTR_PREFETCH_ROWS :
        case SQL_ROWSET_SIZE :
            break;
        case SQL_ATTR_ASYNC_ENABLE :
        case SQL_ATTR_ENABLE_AUTO_IPD  :
        case SQL_ATTR_FETCH_BOOKMARK_PTR :
        case SQL_ATTR_KEYSET_SIZE :
        case SQL_ATTR_MAX_LENGTH :
        case SQL_ATTR_NOSCAN :
        case SQL_ATTR_RETRIEVE_DATA :
        case SQL_ATTR_ROW_OPERATION_PTR  :
        case SQL_ATTR_SIMULATE_CURSOR :
        case SQL_ATTR_USE_BOOKMARKS :
            STL_THROW( RAMP_ERR_OPTIONAL_FEATURE_NOT_IMPLEMENTED );
            break;
        default :
            STL_THROW( RAMP_ERR_INVALID_ATTRIBUTE_OPTION_IDENTIFIER );
            break;
    }

    STL_TRY( stlMarkRegionMem( &sStmt->mAllocator,
                               &sMark,
                               &sErrorStack ) == STL_SUCCESS );
    sState = 1;

    STL_TRY( zlsGetAttrW( sStmt,
                          aAttribute,
                          aValuePtr,
                          aBufferLength,
                          &sStringLength,
                          &sErrorStack ) == STL_SUCCESS );

    switch( aAttribute )
    {
        case SQL_ATTR_EXPLAIN_PLAN_TEXT :
            if( aStringLengthPtr != NULL )
            {
                *aStringLengthPtr = sStringLength;
        
                STL_TRY_THROW( aBufferLength > sStringLength,
                               RAMP_WARNING_STRING_DATA_RIGHT_TRUNCATED );
            }
            break;
        default :
            break;
    }

    sState = 0;
    STL_TRY( stlRestoreRegionMem( &sStmt->mAllocator,
                                  &sMark,
                                  STL_TRUE,
                                  &sErrorStack ) == STL_SUCCESS );

    sRet = SQL_SUCCESS;

    zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

    return sRet;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_CATCH( RAMP_ERR_INVALID_CURSOR_STATE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CURSOR_STATE,
                      "The argument Attribute was SQL_ATTR_ROW_NUMBER and "
                      "the cursor was not open, or the cursor was positioned "
                      "before the start of the result set or after the end of "
                      "the result set.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      NULL,
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_ATTRIBUTE_OPTION_IDENTIFIER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_ATTRIBUTE_OPTION_IDENTIFIER,
                      "The value specified for the argument Attribute was not "
                      "valid for the version of ODBC supported by the driver.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_OPTIONAL_FEATURE_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_OPTIONAL_FEATURE_NOT_IMPLEMENTED,
                      "The value specified for the argument Attribute was a valid "
                      "ODBC connection or statement attribute for the version of "
                      "ODBC supported by the driver but was not supported by the driver.",
                      &sErrorStack );
    }
    
    STL_CATCH( RAMP_WARNING_STRING_DATA_RIGHT_TRUNCATED )
    {
        stlPushError( STL_ERROR_LEVEL_WARNING,
                      ZLE_ERRCODE_WARNING_STRING_DATA_RIGHT_TRUNCATED,
                      "String or binary data returned for a column resulted "
                      "in the truncation of nonblank character or non-NULL binary data. "
                      "If it was a string value, it was right-truncated.",
                      &sErrorStack );

        sRet = SQL_SUCCESS_WITH_INFO;
    }
    
    STL_FINISH;

    if( ( sRet == SQL_ERROR ) || ( sRet == SQL_SUCCESS_WITH_INFO ) )
    {
        zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_STMT,
                               aStatementHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }
        
        if( sStmt != NULL )
        {
            zlcCheckSessionAndCleanup( ZLS_STMT_DBC(sStmt) );
        }
    }

    switch( sState )
    {
        case 1 :
            stlRestoreRegionMem( &sStmt->mAllocator, &sMark, STL_TRUE, &sErrorStack );
        default :
            break;
    }

    return sRet;
}

SQLRETURN SQL_API SQLGetStmtOption( SQLHSTMT     aStatementHandle,
                                    SQLUSMALLINT aOption,
                                    SQLPOINTER   aValue )
{
    SQLRETURN       sRet          = SQL_ERROR;
    zlsStmt       * sStmt         = NULL;
    stlInt32        sAttribute;
    stlInt32        sStringLength = 0;
    stlErrorData  * sErrorData    = NULL;
    stlErrorStack   sErrorStack;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sStmt = (zlsStmt*)aStatementHandle;

    STL_TRY_THROW( sStmt != NULL,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sStmt->mType == SQL_HANDLE_STMT,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( zldDiagClear( SQL_HANDLE_STMT,
                           aStatementHandle,
                           &sErrorStack ) == STL_SUCCESS );

    STL_TRY( zlcCheckSession( sStmt->mParentDbc,
                              &sErrorStack ) == STL_SUCCESS );

    sAttribute = (stlInt32)aOption;
    
    switch( sStmt->mTransition )
    {
        case ZLS_TRANSITION_STATE_S0 :
            STL_THROW( RAMP_ERR_INVALID_HANDLE );
            break;
        case ZLS_TRANSITION_STATE_S1 :
        case ZLS_TRANSITION_STATE_S2 :
        case ZLS_TRANSITION_STATE_S3 :
        case ZLS_TRANSITION_STATE_S4 :
        case ZLS_TRANSITION_STATE_S5 :
            STL_TRY_THROW( sAttribute != SQL_ATTR_ROW_NUMBER,
                           RAMP_ERR_INVALID_CURSOR_STATE );
            break;
        case ZLS_TRANSITION_STATE_S6 :
        case ZLS_TRANSITION_STATE_S7 :
            /**
             * @todo valid row 정보를 추가해야 한다.
             */
            break;
        case ZLS_TRANSITION_STATE_S8 :
        case ZLS_TRANSITION_STATE_S9 :
        case ZLS_TRANSITION_STATE_S10 :
        case ZLS_TRANSITION_STATE_S11 :
        case ZLS_TRANSITION_STATE_S12 :
            STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR );
            break;
    }

    switch( sAttribute )
    {
        case SQL_ATTR_APP_PARAM_DESC :
        case SQL_ATTR_APP_ROW_DESC :
        case SQL_ATTR_CONCURRENCY :
        case SQL_ATTR_CURSOR_HOLDABLE :
        case SQL_ATTR_CURSOR_SCROLLABLE :
        case SQL_ATTR_CURSOR_SENSITIVITY :
        case SQL_ATTR_CURSOR_TYPE :
        case SQL_ATTR_IMP_PARAM_DESC :
        case SQL_ATTR_IMP_ROW_DESC :
        case SQL_ATTR_METADATA_ID :
        case SQL_ATTR_MAX_ROWS :
        case SQL_ATTR_PARAM_BIND_OFFSET_PTR :
        case SQL_ATTR_PARAM_BIND_TYPE :
        case SQL_ATTR_PARAM_OPERATION_PTR :
        case SQL_ATTR_PARAM_STATUS_PTR :
        case SQL_ATTR_PARAMS_PROCESSED_PTR  :
        case SQL_ATTR_PARAMSET_SIZE :
        case SQL_ATTR_QUERY_TIMEOUT :
        case SQL_ATTR_ROW_ARRAY_SIZE :
        case SQL_ATTR_ROW_BIND_OFFSET_PTR :
        case SQL_ATTR_ROW_BIND_TYPE :
        case SQL_ATTR_ROW_NUMBER :
        case SQL_ATTR_ROW_STATUS_PTR :
        case SQL_ATTR_ROWS_FETCHED_PTR :
        case SQL_ATTR_EXPLAIN_PLAN_OPTION :
        case SQL_ATTR_EXPLAIN_PLAN_TEXT :
        case SQL_ATTR_ATOMIC_EXECUTION :
        case SQL_ATTR_PREFETCH_ROWS :
        case SQL_ROWSET_SIZE :
            break;
        case SQL_ATTR_ASYNC_ENABLE :
        case SQL_ATTR_ENABLE_AUTO_IPD  :
        case SQL_ATTR_FETCH_BOOKMARK_PTR :
        case SQL_ATTR_KEYSET_SIZE :
        case SQL_ATTR_MAX_LENGTH :
        case SQL_ATTR_NOSCAN :
        case SQL_ATTR_RETRIEVE_DATA :
        case SQL_ATTR_ROW_OPERATION_PTR  :
        case SQL_ATTR_SIMULATE_CURSOR :
        case SQL_ATTR_USE_BOOKMARKS :
            STL_THROW( RAMP_ERR_OPTIONAL_FEATURE_NOT_IMPLEMENTED );
            break;
        default :
            STL_THROW( RAMP_ERR_INVALID_ATTRIBUTE_OPTION_IDENTIFIER );
            break;
    }

    STL_TRY( zlsGetAttr( sStmt,
                         sAttribute,
                         aValue,
                         SQL_MAX_OPTION_STRING_LENGTH,
                         &sStringLength,
                         &sErrorStack ) == STL_SUCCESS );

    switch( sAttribute )
    {
        case SQL_ATTR_EXPLAIN_PLAN_TEXT :
            STL_TRY_THROW( SQL_MAX_OPTION_STRING_LENGTH >= sStringLength,
                           RAMP_WARNING_STRING_DATA_RIGHT_TRUNCATED );
            break;
        default :
            break;
    }
    
    sRet = SQL_SUCCESS;

    zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

    return sRet;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_CATCH( RAMP_ERR_INVALID_CURSOR_STATE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CURSOR_STATE,
                      "The argument Attribute was SQL_ATTR_ROW_NUMBER and "
                      "the cursor was not open, or the cursor was positioned "
                      "before the start of the result set or after the end of "
                      "the result set.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      NULL,
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_ATTRIBUTE_OPTION_IDENTIFIER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_ATTRIBUTE_OPTION_IDENTIFIER,
                      "The value specified for the argument Attribute was not "
                      "valid for the version of ODBC supported by the driver.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_OPTIONAL_FEATURE_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_OPTIONAL_FEATURE_NOT_IMPLEMENTED,
                      "The value specified for the argument Attribute was a valid "
                      "ODBC connection or statement attribute for the version of "
                      "ODBC supported by the driver but was not supported by the driver.",
                      &sErrorStack );
    }
    
    STL_CATCH( RAMP_WARNING_STRING_DATA_RIGHT_TRUNCATED )
    {
        stlPushError( STL_ERROR_LEVEL_WARNING,
                      ZLE_ERRCODE_WARNING_STRING_DATA_RIGHT_TRUNCATED,
                      "String or binary data returned for a column resulted "
                      "in the truncation of nonblank character or non-NULL binary data. "
                      "If it was a string value, it was right-truncated.",
                      &sErrorStack );

        sRet = SQL_SUCCESS_WITH_INFO;
    }
    
    STL_FINISH;

    if( ( sRet == SQL_ERROR ) || ( sRet == SQL_SUCCESS_WITH_INFO ) )
    {
        zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_STMT,
                               aStatementHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }
        
        if( sStmt != NULL )
        {
            zlcCheckSessionAndCleanup( ZLS_STMT_DBC(sStmt) );
        }
    }

    return sRet;
}

static SQLRETURN zllGetTypeInfo( SQLHSTMT    aStatementHandle,
                                 SQLSMALLINT aDataType )
{
    SQLRETURN       sRet              = SQL_ERROR;
    zlcDbc        * sDbc              = NULL;
    zlsStmt       * sStmt             = NULL;
    stlInt64        sAffectedRowCount = -1;
    stlErrorData  * sErrorData        = NULL;
    stlErrorStack   sErrorStack;
    stlBool         sSuccessWithInfo  = STL_FALSE;
    
    stlChar         sBuffer[2048];
    stlInt32        sBufferLength = STL_SIZEOF( sBuffer );

    STL_INIT_ERROR_STACK( &sErrorStack );

    sStmt = (zlsStmt*)aStatementHandle;

    STL_TRY_THROW( sStmt != NULL,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sStmt->mType == SQL_HANDLE_STMT,
                   RAMP_ERR_INVALID_HANDLE );

    sDbc = sStmt->mParentDbc;

    STL_TRY( zldDiagClear( SQL_HANDLE_STMT,
                           aStatementHandle,
                           &sErrorStack ) == STL_SUCCESS );

    STL_TRY( zlcCheckSession( sDbc,
                              &sErrorStack ) == STL_SUCCESS );
    
    switch( sDbc->mTransition )
    {
        case ZLC_TRANSITION_STATE_C0 :
        case ZLC_TRANSITION_STATE_C1 :
        case ZLC_TRANSITION_STATE_C2 :
        case ZLC_TRANSITION_STATE_C3 :
        case ZLC_TRANSITION_STATE_C4 :
            STL_THROW( RAMP_ERR_INVALID_HANDLE );
            break;
        case ZLC_TRANSITION_STATE_C5 :
        case ZLC_TRANSITION_STATE_C6 :
            break;
    }

    switch( sStmt->mTransition )
    {
        case ZLS_TRANSITION_STATE_S1 :
        case ZLS_TRANSITION_STATE_S2 :
        case ZLS_TRANSITION_STATE_S3 :
        case ZLS_TRANSITION_STATE_S4 :
            break;
        case ZLS_TRANSITION_STATE_S5 :
            STL_THROW( RAMP_ERR_INVALID_CURSOR_STATE_1 );
            break;
        case ZLS_TRANSITION_STATE_S6 :
        case ZLS_TRANSITION_STATE_S7 :
            STL_THROW( RAMP_ERR_INVALID_CURSOR_STATE_2 );
            break;
        case ZLS_TRANSITION_STATE_S8 :
        case ZLS_TRANSITION_STATE_S9 :
        case ZLS_TRANSITION_STATE_S10 :
        case ZLS_TRANSITION_STATE_S11 :
        case ZLS_TRANSITION_STATE_S12 :
            STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR );
            break;
    }

    if( aDataType == SQL_ALL_TYPES )
    {
        stlStrncpy( sBuffer,
                    "SELECT TYPE_NAME, ODBC_DATA_TYPE AS DATA_TYPE, "
                    "       COLUMN_SIZE, LITERAL_PREFIX, LITERAL_SUFFIX, CREATE_PARAMS, "
                    "       NULLABLE, CASE_SENSITIVE, SEARCHABLE, UNSIGNED_ATTRIBUTE, "
                    "       FIXED_PREC_SCALE, AUTO_UNIQUE_VALUE, LOCAL_TYPE_NAME, "
                    "       MINIMUM_SCALE, MAXIMUM_SCALE, SQL_DATA_TYPE, SQL_DATETIME_SUB, "
                    "       NUM_PREC_RADIX, INTERVAL_PRECISION "
                    "FROM DEFINITION_SCHEMA.TYPE_INFO",
                    sBufferLength );
    }
    else
    {
        stlSnprintf( sBuffer,
                     sBufferLength,
                     "SELECT /*+ INDEX( DEFINITION_SCHEMA.TYPE_INFO, TYPE_INFO_INDEX_ODBC_DATA_TYPE ) */"
                     "       TYPE_NAME, ODBC_DATA_TYPE AS DATA_TYPE, "
                     "       COLUMN_SIZE, LITERAL_PREFIX, LITERAL_SUFFIX, CREATE_PARAMS, "
                     "       NULLABLE, CASE_SENSITIVE, SEARCHABLE, UNSIGNED_ATTRIBUTE, "
                     "       FIXED_PREC_SCALE, AUTO_UNIQUE_VALUE, LOCAL_TYPE_NAME, "
                     "       MINIMUM_SCALE, MAXIMUM_SCALE, SQL_DATA_TYPE, SQL_DATETIME_SUB, "
                     "       NUM_PREC_RADIX, INTERVAL_PRECISION "
                     "FROM DEFINITION_SCHEMA.TYPE_INFO "
                     "WHERE ODBC_DATA_TYPE = %d",
                     aDataType );
    }
    
    STL_TRY( zlxExecDirect( sStmt,
                            sBuffer,
                            &sAffectedRowCount,
                            &sSuccessWithInfo,
                            &sErrorStack ) == STL_SUCCESS );

    STL_TRY_THROW( sSuccessWithInfo == STL_FALSE, RAMP_SUCCESS_WITH_INFO );

    sRet = SQL_SUCCESS;
    
    zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

    return sRet;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_CATCH( RAMP_SUCCESS_WITH_INFO )
    {
        sRet = SQL_SUCCESS_WITH_INFO;
    }

    STL_CATCH( RAMP_ERR_INVALID_CURSOR_STATE_1 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CURSOR_STATE,
                      "A cursor was open on the StatementHandle, "
                      "but SQLFetch or SQLFetchScroll had not been called.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_CURSOR_STATE_2 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CURSOR_STATE,
                      "A cursor was open on the StatementHandle, "
                      "and SQLFetch or SQLFetchScroll had been called.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      NULL,
                      &sErrorStack );
    }

    STL_FINISH;

    if( ( sRet == SQL_ERROR ) ||
        ( sRet == SQL_SUCCESS_WITH_INFO ) )
    {
        zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_STMT,
                               aStatementHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }

        if( sStmt != NULL )
        {
            zlcCheckSessionAndCleanup( ZLS_STMT_DBC(sStmt) );
        }
    }

    return sRet;
}

SQLRETURN SQL_API SQLGetTypeInfo( SQLHSTMT    aStatementHandle,
                                  SQLSMALLINT aDataType )
{
    return zllGetTypeInfo( aStatementHandle, aDataType );
}

SQLRETURN SQL_API SQLGetTypeInfoW( SQLHSTMT    aStatementHandle,
                                   SQLSMALLINT aDataType )
{
    return zllGetTypeInfo( aStatementHandle, aDataType );
}

SQLRETURN SQL_API SQLMoreResults( SQLHSTMT aStatementHandle )
{
    return SQL_NO_DATA;
}

SQLRETURN SQL_API SQLNativeSql( SQLHDBC      aConnectionHandle,
                                SQLCHAR    * aInStatementText,
                                SQLINTEGER   aTextLength1,
                                SQLCHAR    * aOutStatementText,
                                SQLINTEGER   aBufferLength,
                                SQLINTEGER * aTextLength2Ptr )
{
    return SQL_ERROR;
}

SQLRETURN SQL_API SQLNativeSqlW( SQLHDBC      aConnectionHandle,
                                 SQLWCHAR   * aInStatementText,
                                 SQLINTEGER   aTextLength1,
                                 SQLWCHAR   * aOutStatementText,
                                 SQLINTEGER   aBufferLength,
                                 SQLINTEGER * aTextLength2Ptr )
{
    /**
     * aTextLength1
     *     Length of *InStatementText text string, in characters.
     * aBufferLength
     *     Length of the *OutStatementText buffer, in characters.
     *     (Note that this means this argument does NOT have to be an
     *     even number.)
     * aTextLength2Ptr
     *     Pointer to a buffer in which to return the total number of
     *     characters (excluding the null-termination character) available to
     *     return in *OutStatementText. 
     */
    
    return SQL_ERROR;
}

SQLRETURN SQL_API SQLNumParams( SQLHSTMT      aStatementHandle,
                                SQLSMALLINT * aParameterCountPtr )
{
    SQLRETURN       sRet        = SQL_ERROR;
    zlsStmt       * sStmt       = NULL;
    stlErrorData  * sErrorData  = NULL;
    stlErrorStack   sErrorStack;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sStmt = (zlsStmt*)aStatementHandle;

    STL_TRY_THROW( sStmt != NULL,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sStmt->mType == SQL_HANDLE_STMT,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( zldDiagClear( SQL_HANDLE_STMT,
                           aStatementHandle,
                           &sErrorStack ) == STL_SUCCESS );

    STL_TRY( zlcCheckSession( sStmt->mParentDbc,
                              &sErrorStack ) == STL_SUCCESS );
    
    switch( sStmt->mTransition )
    {
        case ZLS_TRANSITION_STATE_S0 :
            STL_THROW( RAMP_ERR_INVALID_HANDLE );
            break;
        case ZLS_TRANSITION_STATE_S1 :
            STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR1 );
            break;
        case ZLS_TRANSITION_STATE_S8 :
        case ZLS_TRANSITION_STATE_S9 :
        case ZLS_TRANSITION_STATE_S10 :
        case ZLS_TRANSITION_STATE_S11 :
        case ZLS_TRANSITION_STATE_S12 :
            STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR2 );
            break;
        default :
            break;
    }

    if( aParameterCountPtr != NULL )
    {
        if( sStmt->mParamCount == ZLS_PARAM_COUNT_UNKNOWN )
        {
            STL_TRY( zliNumParams( sStmt,
                                   &sStmt->mParamCount,
                                   &sErrorStack ) == STL_SUCCESS );
        }

        *aParameterCountPtr = sStmt->mParamCount;
    }

    sRet = SQL_SUCCESS;

    zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

    return sRet;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR1 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      "The function was called prior to calling SQLPrepare or "
                      "SQLExecDirect for the StatementHandle.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR2 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      NULL,
                      &sErrorStack );
    }

    STL_FINISH;

    if( sRet == SQL_ERROR )
    {
        zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_STMT,
                               aStatementHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }
        
        if( sStmt != NULL )
        {
            zlcCheckSessionAndCleanup( ZLS_STMT_DBC(sStmt) );
        }
    }

    return sRet;
}

SQLRETURN SQL_API SQLNumResultCols( SQLHSTMT      aStatementHandle,
                                    SQLSMALLINT * aColumnCountPtr )
{
    SQLRETURN       sRet        = SQL_ERROR;
    zlsStmt       * sStmt       = NULL;
    stlErrorData  * sErrorData  = NULL;
    stlErrorStack   sErrorStack;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sStmt = (zlsStmt*)aStatementHandle;

    STL_TRY_THROW( sStmt != NULL, RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sStmt->mType == SQL_HANDLE_STMT,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( zldDiagClear( SQL_HANDLE_STMT,
                           aStatementHandle,
                           &sErrorStack ) == STL_SUCCESS );

    STL_TRY( zlcCheckSession( sStmt->mParentDbc,
                              &sErrorStack ) == STL_SUCCESS );
    
    switch( sStmt->mTransition )
    {
        case ZLS_TRANSITION_STATE_S0 :
            STL_THROW( RAMP_ERR_INVALID_HANDLE );
            break;
        case ZLS_TRANSITION_STATE_S1 :
            STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR1 );
            break;
        case ZLS_TRANSITION_STATE_S8 :
        case ZLS_TRANSITION_STATE_S9 :
        case ZLS_TRANSITION_STATE_S10 :
        case ZLS_TRANSITION_STATE_S11 :
        case ZLS_TRANSITION_STATE_S12 :
            STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR2 );
            break;
        default :
            break;
    }

    if( aColumnCountPtr != NULL )
    {
        *aColumnCountPtr = sStmt->mIrd.mCount;
    }

    sRet = SQL_SUCCESS;

    zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

    return sRet;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR1 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      "The function was called prior to calling SQLPrepare or "
                      "SQLExecDirect for the StatementHandle.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR2 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      NULL,
                      &sErrorStack );
    }

    STL_FINISH;

    if( sRet == SQL_ERROR )
    {
        zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            zldDiagAdds( SQL_HANDLE_STMT,
                         aStatementHandle,
                         SQL_NO_ROW_NUMBER,
                         SQL_NO_COLUMN_NUMBER,
                         &sErrorStack );
        }
        
        if( sStmt != NULL )
        {
            zlcCheckSessionAndCleanup( ZLS_STMT_DBC(sStmt) );
        }
    }

    return sRet;
}

SQLRETURN SQL_API SQLParamData( SQLHSTMT     aStatementHandle,
                                SQLPOINTER * aValuePtrPtr )
{
    SQLRETURN       sRet              = SQL_ERROR;
    zlnEnv        * sEnv              = NULL;
    zlcDbc        * sDbc              = NULL;
    zlsStmt       * sStmt             = NULL;
    stlBool         sHasDataAtExec    = STL_FALSE;
    stlInt64        sAffectedRowCount = -1;
    stlBool         sSuccessWithInfo  = STL_FALSE;
    stlErrorData  * sErrorData        = NULL;
    stlErrorStack   sErrorStack;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sStmt = (zlsStmt*)aStatementHandle;

    STL_TRY_THROW( sStmt != NULL, RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sStmt->mType == SQL_HANDLE_STMT,
                   RAMP_ERR_INVALID_HANDLE );

    sDbc = sStmt->mParentDbc;
    sEnv = sDbc->mParentEnv;

    STL_TRY( zldDiagClear( SQL_HANDLE_STMT,
                           aStatementHandle,
                           &sErrorStack ) == STL_SUCCESS );

    STL_TRY( zlcCheckSession( sDbc,
                              &sErrorStack ) == STL_SUCCESS );
    
    switch( sStmt->mTransition )
    {
        case ZLS_TRANSITION_STATE_S1 :
        case ZLS_TRANSITION_STATE_S2 :
        case ZLS_TRANSITION_STATE_S3 :
        case ZLS_TRANSITION_STATE_S4 :
        case ZLS_TRANSITION_STATE_S5 :
        case ZLS_TRANSITION_STATE_S6 :
        case ZLS_TRANSITION_STATE_S7 :
            STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR1 );
            break;
        case ZLS_TRANSITION_STATE_S9 :
            STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR2 );
            break;
        case ZLS_TRANSITION_STATE_S8 :
            *aValuePtrPtr = sStmt->mCurrentPutDataApdRec->mDataPtr;

            sRet = SQL_NEED_DATA;
            break;
        case ZLS_TRANSITION_STATE_S10 :
            STL_TRY( zliProecessDataAtExec( sStmt,
                                            sStmt->mApd,
                                            &sStmt->mIpd,
                                            &sHasDataAtExec,
                                            &sErrorStack ) == STL_SUCCESS );

            if( sHasDataAtExec == STL_TRUE )
            {
                *aValuePtrPtr = sStmt->mCurrentPutDataApdRec->mDataPtr;

                sRet = SQL_NEED_DATA;
            }
            else
            {
                sStmt->mTransition = sStmt->mNeedDataTransition;
                sStmt->mNeedDataTransition = ZLS_TRANSITION_STATE_S0;
                
                STL_TRY( zlxExecute( sStmt,
                                     &sAffectedRowCount,
                                     &sSuccessWithInfo,
                                     &sErrorStack ) == STL_SUCCESS );

                if( sEnv->mAttrVersion == SQL_OV_ODBC3 )
                {
                    STL_TRY_THROW( sAffectedRowCount != 0, RAMP_NO_DATA );
                }
    
                STL_TRY_THROW( sSuccessWithInfo == STL_FALSE, RAMP_SUCCESS_WITH_INFO );

                sRet = SQL_SUCCESS;
            }
            break;
        case ZLS_TRANSITION_STATE_S11 :
        case ZLS_TRANSITION_STATE_S12 :
            STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR3 );
            break;
    }

    if( sRet == SQL_NEED_DATA )
    {
        sStmt->mTransition = ZLS_TRANSITION_STATE_S9;
    }

    zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

    return sRet;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR1 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      "The previous function call was not a call to SQLExecDirect, "
                      "SQLExecute, SQLBulkOperations, or SQLSetPos where the return code "
                      "was SQL_NEED_DATA, or the previous function call was a call to "
                      "SQLPutData.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR2 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      "The previous function call was a call to SQLParamData.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR3 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      NULL,
                      &sErrorStack );
    }

    STL_CATCH( RAMP_NO_DATA )
    {
        sRet = SQL_NO_DATA;
    }

    STL_CATCH( RAMP_SUCCESS_WITH_INFO )
    {
        sRet = SQL_SUCCESS_WITH_INFO;
    }

    STL_FINISH;

    if( ( sRet == SQL_ERROR ) ||
        ( sRet == SQL_SUCCESS_WITH_INFO ) )
    {
        zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_STMT,
                               aStatementHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }
        
        if( sStmt != NULL )
        {
            zlcCheckSessionAndCleanup( ZLS_STMT_DBC(sStmt) );
        }
    }

    return sRet;
}

SQLRETURN SQL_API SQLParamOptions( SQLHSTMT   hstmt,
                                   SQLULEN    crow,
                                   SQLULEN  * pirow )
{
    SQLRETURN       sRet        = SQL_ERROR;
    zlsStmt       * sStmt       = NULL;
    stlErrorData  * sErrorData  = NULL;
    stlErrorStack   sErrorStack;
    stlBool         sSuccessWithInfo = STL_FALSE;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sStmt = (zlsStmt*)hstmt;

    STL_TRY_THROW( sStmt != NULL,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sStmt->mType == SQL_HANDLE_STMT,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( zldDiagClear( SQL_HANDLE_STMT,
                           hstmt,
                           &sErrorStack ) == STL_SUCCESS );

    STL_TRY( zlcCheckSession( sStmt->mParentDbc,
                              &sErrorStack ) == STL_SUCCESS );
    
    switch( sStmt->mTransition )
    {
        case ZLS_TRANSITION_STATE_S0 :
            STL_THROW( RAMP_ERR_INVALID_HANDLE );
            break;
        case ZLS_TRANSITION_STATE_S1 :
        case ZLS_TRANSITION_STATE_S2 :
        case ZLS_TRANSITION_STATE_S3 :
        case ZLS_TRANSITION_STATE_S4 :
        case ZLS_TRANSITION_STATE_S5 :
        case ZLS_TRANSITION_STATE_S6 :
        case ZLS_TRANSITION_STATE_S7 :
            break;
        case ZLS_TRANSITION_STATE_S8 :
        case ZLS_TRANSITION_STATE_S9 :
        case ZLS_TRANSITION_STATE_S10 :
        case ZLS_TRANSITION_STATE_S11 :
        case ZLS_TRANSITION_STATE_S12 :
            STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR );
            break;
    }

    STL_TRY( zlsSetAttr( sStmt,
                         SQL_ATTR_PARAMSET_SIZE,
                         (void*)(stlVarInt)crow,
                         0,
                         &sSuccessWithInfo,
                         &sErrorStack ) == STL_SUCCESS );

    STL_TRY( zlsSetAttr( sStmt,
                         SQL_ATTR_PARAMS_PROCESSED_PTR,
                         (void*)pirow,
                         0,
                         &sSuccessWithInfo,
                         &sErrorStack ) == STL_SUCCESS );

    sRet = SQL_SUCCESS;

    zldDiagSetRetCode( SQL_HANDLE_STMT, hstmt, sRet );

    return sRet;

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      NULL,
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_FINISH;

    if( sRet == SQL_ERROR )
    {
        zldDiagSetRetCode( SQL_HANDLE_STMT, hstmt, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_STMT,
                               hstmt,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }
        
        if( sStmt != NULL )
        {
            zlcCheckSessionAndCleanup( ZLS_STMT_DBC(sStmt) );
        }
    }

    return sRet;
}

static SQLRETURN zllPrepare( SQLHSTMT        aStatementHandle,
                             SQLCHAR       * aStatementText,
                             SQLINTEGER      aTextLength,
                             stlErrorStack * aErrorStack )
{
    SQLRETURN       sRet             = SQL_ERROR;
    zlcDbc        * sDbc             = NULL;
    zlsStmt       * sStmt            = NULL;
    stlChar       * sSQL             = NULL;
    stlBool         sSuccessWithInfo = STL_FALSE;

    sStmt = (zlsStmt*)aStatementHandle;
    sDbc  = sStmt->mParentDbc;

    STL_TRY( zldDiagClear( SQL_HANDLE_STMT,
                           aStatementHandle,
                           aErrorStack ) == STL_SUCCESS );

    STL_TRY( zlcCheckSession( sStmt->mParentDbc,
                              aErrorStack ) == STL_SUCCESS );
    
    switch( sDbc->mTransition )
    {
        case ZLC_TRANSITION_STATE_C0 :
        case ZLC_TRANSITION_STATE_C1 :
        case ZLC_TRANSITION_STATE_C2 :
        case ZLC_TRANSITION_STATE_C3 :
        case ZLC_TRANSITION_STATE_C4 :
            STL_THROW( RAMP_ERR_INVALID_HANDLE );
            break;
        case ZLC_TRANSITION_STATE_C5 :
        case ZLC_TRANSITION_STATE_C6 :
            break;
    }

    STL_TRY_THROW( aStatementText != NULL, RAMP_ERR_INVALID_USE_OF_NULL_POINTER );

    STL_TRY( sDbc->mConvertSQLFunc( sStmt,
                                    (stlChar*)aStatementText,
                                    aTextLength,
                                    &sSQL,
                                    aErrorStack ) == STL_SUCCESS );

    STL_TRY( zlpPrepare( sStmt,
                         sSQL,
                         &sSuccessWithInfo,
                         aErrorStack ) == STL_SUCCESS );

    STL_TRY_THROW( sSuccessWithInfo == STL_FALSE, RAMP_SUCCESS_WITH_INFO );

    sRet = SQL_SUCCESS;
    
    return sRet;

    STL_CATCH( RAMP_ERR_INVALID_USE_OF_NULL_POINTER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_USE_OF_NULL_POINTER,
                      "StatementText was a null pointer.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_CATCH( RAMP_SUCCESS_WITH_INFO )
    {
        sRet = SQL_SUCCESS_WITH_INFO;
    }

    STL_FINISH;

    return sRet;
}

SQLRETURN SQL_API SQLPrepare( SQLHSTMT     aStatementHandle,
                              SQLCHAR    * aStatementText,
                              SQLINTEGER   aTextLength )
{
    SQLRETURN       sRet  = SQL_ERROR;
    zlsStmt       * sStmt = NULL;
    stlErrorStack   sErrorStack;
    stlErrorData  * sErrorData = NULL;
    stlRegionMark   sMark;
    stlInt32        sState = 0;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sStmt = (zlsStmt*)aStatementHandle;

    STL_TRY_THROW( sStmt != NULL,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sStmt->mType == SQL_HANDLE_STMT,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( stlMarkRegionMem( &sStmt->mAllocator,
                               &sMark,
                               &sErrorStack ) == STL_SUCCESS );
    sState = 1;

    sRet = zllPrepare( aStatementHandle,
                       aStatementText,
                       aTextLength,
                       &sErrorStack );
    
    STL_TRY( sRet == SQL_SUCCESS );

    sState = 0;
    STL_TRY( stlRestoreRegionMem( &sStmt->mAllocator,
                                  &sMark,
                                  STL_TRUE,
                                  &sErrorStack ) == STL_SUCCESS );

    zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

    return SQL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_FINISH;

    if( ( sRet == SQL_ERROR ) ||
        ( sRet == SQL_SUCCESS_WITH_INFO ) )
    {
        zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_STMT,
                               aStatementHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }

        if( sStmt != NULL )
        {
            zlcCheckSessionAndCleanup( ZLS_STMT_DBC(sStmt) );
        }
    }

    switch( sState )
    {
        case 1 :
            (void)stlRestoreRegionMem( &sStmt->mAllocator, &sMark, STL_TRUE, &sErrorStack );
        default :
            break;
    }

    return sRet;
}

SQLRETURN SQL_API SQLPrepareW( SQLHSTMT     aStatementHandle,
                               SQLWCHAR   * aStatementText,
                               SQLINTEGER   aTextLength )
{
    SQLRETURN       sRet  = SQL_ERROR;
    zlsStmt       * sStmt = NULL;
    stlErrorStack   sErrorStack;
    stlErrorData  * sErrorData = NULL;
    stlRegionMark   sMark;
    stlInt32        sState = 0;

    stlInt64        sBufferLength;
    SQLCHAR       * sStatementText = NULL;
    stlInt64        sTextLength = 0;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sStmt = (zlsStmt*)aStatementHandle;

    STL_TRY_THROW( sStmt != NULL,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sStmt->mType == SQL_HANDLE_STMT,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( stlMarkRegionMem( &sStmt->mAllocator,
                               &sMark,
                               &sErrorStack ) == STL_SUCCESS );
    sState = 1;

    sBufferLength = 0;    
    STL_TRY( zlvAllocCharBuffer( aStatementText,
                                 aTextLength,
                                 ZLS_STMT_DBC(sStmt)->mCharacterSet,
                                 &sStatementText,
                                 &sBufferLength,
                                 &sStmt->mAllocator,
                                 &sErrorStack ) == STL_SUCCESS );
    
    STL_TRY( zlvWcharToChar( aStatementText,
                             aTextLength,
                             ZLS_STMT_DBC(sStmt)->mCharacterSet,
                             sStatementText,
                             sBufferLength,
                             &sTextLength,
                             &sErrorStack ) == STL_SUCCESS );
    
    sRet = zllPrepare( aStatementHandle,
                       sStatementText,
                       sTextLength,
                       &sErrorStack );
    
    STL_TRY( sRet == SQL_SUCCESS );

    sState = 0;
    STL_TRY( stlRestoreRegionMem( &sStmt->mAllocator,
                                  &sMark,
                                  STL_TRUE,
                                  &sErrorStack ) == STL_SUCCESS );

    zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

    return SQL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_FINISH;

    if( ( sRet == SQL_ERROR ) ||
        ( sRet == SQL_SUCCESS_WITH_INFO ) )
    {
        zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_STMT,
                               aStatementHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }

        if( sStmt != NULL )
        {
            zlcCheckSessionAndCleanup( ZLS_STMT_DBC(sStmt) );
        }
    }

    switch( sState )
    {
        case 1 :
            (void)stlRestoreRegionMem( &sStmt->mAllocator, &sMark, STL_TRUE, &sErrorStack );
        default :
            break;
    }

    return sRet;
}

static SQLRETURN zllPrimaryKeys( SQLHSTMT        aStatementHandle,
                                 SQLCHAR       * aCatalogName,
                                 SQLSMALLINT     aNameLength1,
                                 SQLCHAR       * aSchemaName,
                                 SQLSMALLINT     aNameLength2,
                                 SQLCHAR       * aTableName,
                                 SQLSMALLINT     aNameLength3,
                                 stlErrorStack * aErrorStack )
{
    SQLRETURN       sRet              = SQL_ERROR;
    zlcDbc        * sDbc              = NULL;
    zlsStmt       * sStmt             = NULL;
    stlInt64        sAffectedRowCount = -1;
    stlBool         sSuccessWithInfo  = STL_FALSE;
    
    stlChar         sBuffer[2048];
    stlInt32        sBufferLength = STL_SIZEOF( sBuffer );

    stlInt16        sNameLength1 = 0;
    stlInt16        sNameLength2 = 0;
    stlInt16        sNameLength3 = 0;

    stlInt32        sPos = 0;
    
    SQLCHAR       * sSELECT   = (SQLCHAR*)"SELECT "
        "  CAST(K.TABLE_CATALOG    AS VARCHAR(128 OCTETS))    AS TABLE_CAT, "
        "  CAST(K.TABLE_SCHEMA     AS VARCHAR(128 OCTETS))    AS TABLE_SCHEM, "
        "  CAST(K.TABLE_NAME       AS VARCHAR(128 OCTETS))    AS TABLE_NAME, "
        "  CAST(K.COLUMN_NAME      AS VARCHAR(128 OCTETS))    AS COLUMN_NAME, "
        "  CAST(K.ORDINAL_POSITION AS NATIVE_SMALLINT) AS KEY_SEQ, "
        "  CAST(K.CONSTRAINT_NAME  AS VARCHAR(128 OCTETS))    AS PK_NAME "
        "FROM INFORMATION_SCHEMA.KEY_COLUMN_USAGE  AS K, "
        "     INFORMATION_SCHEMA.TABLE_CONSTRAINTS AS P ";
    SQLCHAR       * sWHERE    = (SQLCHAR*)"WHERE K.CONSTRAINT_CATALOG = P.CONSTRAINT_CATALOG AND "
        "      K.CONSTRAINT_SCHEMA  = P.CONSTRAINT_SCHEMA AND "
        "      K.CONSTRAINT_NAME    = P.CONSTRAINT_NAME AND "
        "      P.CONSTRAINT_TYPE    = 'PRIMARY KEY' ";
    SQLCHAR *       sORDER_BY = (SQLCHAR*)"ORDER BY TABLE_CAT, TABLE_SCHEM, TABLE_NAME, KEY_SEQ";

    sStmt = (zlsStmt*)aStatementHandle;
    sDbc  = sStmt->mParentDbc;

    STL_TRY( zldDiagClear( SQL_HANDLE_STMT,
                           aStatementHandle,
                           aErrorStack ) == STL_SUCCESS );

    STL_TRY( zlcCheckSession( sDbc,
                              aErrorStack ) == STL_SUCCESS );
    
    switch( sDbc->mTransition )
    {
        case ZLC_TRANSITION_STATE_C0 :
        case ZLC_TRANSITION_STATE_C1 :
        case ZLC_TRANSITION_STATE_C2 :
        case ZLC_TRANSITION_STATE_C3 :
        case ZLC_TRANSITION_STATE_C4 :
            STL_THROW( RAMP_ERR_INVALID_HANDLE );
            break;
        case ZLC_TRANSITION_STATE_C5 :
        case ZLC_TRANSITION_STATE_C6 :
            break;
    }

    switch( sStmt->mTransition )
    {
        case ZLS_TRANSITION_STATE_S1 :
        case ZLS_TRANSITION_STATE_S2 :
        case ZLS_TRANSITION_STATE_S3 :
        case ZLS_TRANSITION_STATE_S4 :
            break;
        case ZLS_TRANSITION_STATE_S5 :
            STL_THROW( RAMP_ERR_INVALID_CURSOR_STATE_1 );
            break;
        case ZLS_TRANSITION_STATE_S6 :
        case ZLS_TRANSITION_STATE_S7 :
            STL_THROW( RAMP_ERR_INVALID_CURSOR_STATE_2 );
            break;
        case ZLS_TRANSITION_STATE_S8 :
        case ZLS_TRANSITION_STATE_S9 :
        case ZLS_TRANSITION_STATE_S10 :
        case ZLS_TRANSITION_STATE_S11 :
        case ZLS_TRANSITION_STATE_S12 :
            STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR );
            break;
    }

    if( aNameLength1 != SQL_NTS )
    {
        STL_TRY_THROW( aNameLength1 >= 0, RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH );

        sNameLength1 = aNameLength1;
    }
    else
    {
        if (aCatalogName != NULL)
        {
            sNameLength1 = stlStrlen((stlChar*)aCatalogName);
        }        
    }

    if( aNameLength2 != SQL_NTS )
    {
        STL_TRY_THROW( aNameLength2 >= 0, RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH );

        sNameLength2 = aNameLength2;
    }
    else
    {
        if (aSchemaName != NULL)
        {
            sNameLength2 = stlStrlen((stlChar*)aSchemaName);
        }        
    }

    if( aNameLength3 != SQL_NTS )
    {
        STL_TRY_THROW( aNameLength3 >= 0, RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH );

        sNameLength3 = aNameLength3;
    }
    else
    {
        if (aTableName != NULL)
        {
            sNameLength3 = stlStrlen((stlChar*)aTableName);
        }        
    }

    STL_TRY_THROW( aTableName != NULL, RAMP_ERR_INVALID_USE_OF_NULL_POINTER_1 );
    
    if( sStmt->mMetadataId == STL_TRUE )
    {
        if( aCatalogName != NULL )
        {
            if( (aCatalogName[0] == '\"') &&
                (aCatalogName[sNameLength1-1] == '\"') )
            {
                aCatalogName[sNameLength1-1] = 0;
                aCatalogName++;

                sNameLength1 = sNameLength1 - 2;
            }
            else
            {
                stlToupperString( (stlChar*)aCatalogName, (stlChar*)aCatalogName );
            }
        }
        
        STL_TRY_THROW( aSchemaName != NULL, RAMP_ERR_INVALID_USE_OF_NULL_POINTER_2 );

        if( (aSchemaName[0] == '\"') &&
            (aSchemaName[sNameLength2-1] == '\"') )
        {
            aSchemaName[sNameLength2-1] = 0;
            aSchemaName++;

            sNameLength2 = sNameLength2 - 2;
        }
        else
        {
            stlToupperString( (stlChar*)aSchemaName, (stlChar*)aSchemaName );
        }

        if( (aTableName[0] == '\"') &&
            (aTableName[sNameLength3-1] == '\"') )
        {
            aTableName[sNameLength3-1] = 0;
            aTableName++;

            sNameLength3 = sNameLength3 - 2;
        }
        else
        {
            stlToupperString( (stlChar*)aTableName, (stlChar*)aTableName );
        }
    }

    sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos,
                         "%s%s",
                         sSELECT,
                         sWHERE );

    if( (aCatalogName != NULL) && (sNameLength1 > 0) )
    {
        sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos,
                             " AND K.TABLE_CATALOG = '" );
            
        stlMemcpy( sBuffer + sPos, aCatalogName, sNameLength1 );
        sPos += sNameLength1;

        sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos, "' " );
    }

    if( (aSchemaName != NULL) && (sNameLength2 > 0) )
    {
        sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos,
                             " AND K.TABLE_SCHEMA = '" );
            
        stlMemcpy( sBuffer + sPos, aSchemaName, sNameLength2 );
        sPos += sNameLength2;

        sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos, "' " );
    }

    sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos,
                         " AND K.TABLE_NAME = '" );
            
    stlMemcpy( sBuffer + sPos, aTableName, sNameLength3 );
    sPos += sNameLength3;

    sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos, "' " );

    sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos,
                         "%s",
                         sORDER_BY );

    STL_TRY( zlxExecDirect( sStmt,
                            sBuffer,
                            &sAffectedRowCount,
                            &sSuccessWithInfo,
                            aErrorStack ) == STL_SUCCESS );

    STL_TRY_THROW( sSuccessWithInfo == STL_FALSE, RAMP_SUCCESS_WITH_INFO );

    sRet = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_STRING_OR_BUFFER_LENGTH,
                      "The value of one of the length arguments was less than 0 "
                      "but not equal to SQL_NTS.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_USE_OF_NULL_POINTER_1 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_USE_OF_NULL_POINTER,
                      "The TableName argument was a null pointer.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_USE_OF_NULL_POINTER_2 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_USE_OF_NULL_POINTER,
                      "The SQL_ATTR_METADATA_ID statement attribute was set to SQL_TRUE, "
                      "and the SchemaName argument was a null pointer.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_CATCH( RAMP_SUCCESS_WITH_INFO )
    {
        sRet = SQL_SUCCESS_WITH_INFO;
    }

    STL_CATCH( RAMP_ERR_INVALID_CURSOR_STATE_1 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CURSOR_STATE,
                      "A cursor was open on the StatementHandle, "
                      "but SQLFetch or SQLFetchScroll had not been called.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_CURSOR_STATE_2 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CURSOR_STATE,
                      "A cursor was open on the StatementHandle, "
                      "and SQLFetch or SQLFetchScroll had been called.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    return sRet;
}

SQLRETURN SQL_API SQLPrimaryKeys( SQLHSTMT      aStatementHandle,
                                  SQLCHAR     * aCatalogName,
                                  SQLSMALLINT   aNameLength1,
                                  SQLCHAR     * aSchemaName,
                                  SQLSMALLINT   aNameLength2,
                                  SQLCHAR     * aTableName,
                                  SQLSMALLINT   aNameLength3 )
{
    SQLRETURN       sRet  = SQL_ERROR;
    zlsStmt       * sStmt = NULL;
    stlErrorStack   sErrorStack;
    stlErrorData  * sErrorData = NULL;
    stlRegionMark   sMark;
    stlInt32        sState = 0;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sStmt = (zlsStmt*)aStatementHandle;

    STL_TRY_THROW( sStmt != NULL,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sStmt->mType == SQL_HANDLE_STMT,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( stlMarkRegionMem( &sStmt->mAllocator,
                               &sMark,
                               &sErrorStack ) == STL_SUCCESS );
    sState = 1;

    sRet = zllPrimaryKeys( aStatementHandle,
                           aCatalogName,
                           aNameLength1,
                           aSchemaName,
                           aNameLength2,
                           aTableName,
                           aNameLength3,
                           &sErrorStack );

    STL_TRY( sRet == SQL_SUCCESS );

    sState = 0;
    STL_TRY( stlRestoreRegionMem( &sStmt->mAllocator,
                                  &sMark,
                                  STL_TRUE,
                                  &sErrorStack ) == STL_SUCCESS );

    zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

    return SQL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_FINISH;

    if( ( sRet == SQL_ERROR ) ||
        ( sRet == SQL_SUCCESS_WITH_INFO ) )
    {
        zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_STMT,
                               aStatementHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }

        if( sStmt != NULL )
        {
            zlcCheckSessionAndCleanup( ZLS_STMT_DBC(sStmt) );
        }
    }

    switch( sState )
    {
        case 1 :
            stlRestoreRegionMem( &sStmt->mAllocator, &sMark, STL_TRUE, &sErrorStack );
        default :
            break;
    }

    return sRet;
}

SQLRETURN SQL_API SQLPrimaryKeysW( SQLHSTMT      aStatementHandle,
                                   SQLWCHAR    * aCatalogName,
                                   SQLSMALLINT   aNameLength1,
                                   SQLWCHAR    * aSchemaName,
                                   SQLSMALLINT   aNameLength2,
                                   SQLWCHAR    * aTableName,
                                   SQLSMALLINT   aNameLength3 )
{
    /**
     * aNameLength1
     *     Length of *CatalogName, in characters.
     * aNameLength2
     *     Length of *SchemaName, in characters.
     * aNameLength3
     *     Length of *TableName, in characters.
     */

    SQLRETURN       sRet  = SQL_ERROR;
    zlsStmt       * sStmt = NULL;
    stlErrorStack   sErrorStack;
    stlErrorData  * sErrorData = NULL;
    stlRegionMark   sMark;
    stlInt32        sState = 0;

    stlInt64        sBufferLength;
    SQLCHAR       * sCatalogName = NULL;
    stlInt64        sNameLength1 = 0;
    SQLCHAR       * sSchemaName = NULL;
    stlInt64        sNameLength2 = 0;
    SQLCHAR       * sTableName = NULL;
    stlInt64        sNameLength3 = 0;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sStmt = (zlsStmt*)aStatementHandle;

    STL_TRY_THROW( sStmt != NULL,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sStmt->mType == SQL_HANDLE_STMT,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( stlMarkRegionMem( &sStmt->mAllocator,
                               &sMark,
                               &sErrorStack ) == STL_SUCCESS );
    sState = 1;

    sBufferLength = 0;    
    STL_TRY( zlvAllocCharBuffer( aCatalogName,
                                 aNameLength1,
                                 ZLS_STMT_DBC(sStmt)->mCharacterSet,
                                 &sCatalogName,
                                 &sBufferLength,
                                 &sStmt->mAllocator,
                                 &sErrorStack ) == STL_SUCCESS );
    
    STL_TRY( zlvWcharToChar( aCatalogName,
                             aNameLength1,
                             ZLS_STMT_DBC(sStmt)->mCharacterSet,
                             sCatalogName,
                             sBufferLength,
                             &sNameLength1,
                             &sErrorStack ) == STL_SUCCESS );
    
    sBufferLength = 0;    
    STL_TRY( zlvAllocCharBuffer( aSchemaName,
                                 aNameLength2,
                                 ZLS_STMT_DBC(sStmt)->mCharacterSet,
                                 &sSchemaName,
                                 &sBufferLength,
                                 &sStmt->mAllocator,
                                 &sErrorStack ) == STL_SUCCESS );
    
    STL_TRY( zlvWcharToChar( aSchemaName,
                             aNameLength2,
                             ZLS_STMT_DBC(sStmt)->mCharacterSet,
                             sSchemaName,
                             sBufferLength,
                             &sNameLength2,
                             &sErrorStack ) == STL_SUCCESS );
    
    sBufferLength = 0;    
    STL_TRY( zlvAllocCharBuffer( aTableName,
                                 aNameLength3,
                                 ZLS_STMT_DBC(sStmt)->mCharacterSet,
                                 &sTableName,
                                 &sBufferLength,
                                 &sStmt->mAllocator,
                                 &sErrorStack ) == STL_SUCCESS );
    
    STL_TRY( zlvWcharToChar( aTableName,
                             aNameLength3,
                             ZLS_STMT_DBC(sStmt)->mCharacterSet,
                             sTableName,
                             sBufferLength,
                             &sNameLength3,
                             &sErrorStack ) == STL_SUCCESS );
    
    sRet = zllPrimaryKeys( aStatementHandle,
                           sCatalogName,
                           sNameLength1,
                           sSchemaName,
                           sNameLength2,
                           sTableName,
                           sNameLength3,
                           &sErrorStack );

    STL_TRY( sRet == SQL_SUCCESS );

    sState = 0;
    STL_TRY( stlRestoreRegionMem( &sStmt->mAllocator,
                                  &sMark,
                                  STL_TRUE,
                                  &sErrorStack ) == STL_SUCCESS );

    zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

    return SQL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_FINISH;

    if( ( sRet == SQL_ERROR ) ||
        ( sRet == SQL_SUCCESS_WITH_INFO ) )
    {
        zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_STMT,
                               aStatementHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }

        if( sStmt != NULL )
        {
            zlcCheckSessionAndCleanup( ZLS_STMT_DBC(sStmt) );
        }
    }

    switch( sState )
    {
        case 1 :
            (void)stlRestoreRegionMem( &sStmt->mAllocator, &sMark, STL_TRUE, &sErrorStack );
        default :
            break;
    }

    return sRet;
}

static SQLRETURN zllProcedureColumns( SQLHSTMT        aStatementHandle,
                                      SQLCHAR       * aCatalogName,
                                      SQLSMALLINT     aNameLength1,
                                      SQLCHAR       * aSchemaName,
                                      SQLSMALLINT     aNameLength2,
                                      SQLCHAR       * aProcName,
                                      SQLSMALLINT     aNameLength3,
                                      SQLCHAR       * aColumnName,
                                      SQLSMALLINT     aNameLength4,
                                      stlErrorStack * aErrorStack )
{
    SQLRETURN       sRet              = SQL_ERROR;
    zlcDbc        * sDbc              = NULL;
    zlsStmt       * sStmt             = NULL;
    stlInt64        sAffectedRowCount = -1;
    stlBool         sSuccessWithInfo  = STL_FALSE;
    
    stlChar         sBuffer[2048];
    stlInt32        sBufferLength = STL_SIZEOF( sBuffer );

    stlInt16        sNameLength1 = 0;
    stlInt16        sNameLength2 = 0;
    stlInt16        sNameLength3 = 0;
    stlInt16        sNameLength4 = 0;

    stlChar         sOp[5];
    stlInt32        sPos = 0;

    SQLCHAR * sSELECT   = (SQLCHAR*)"SELECT "
        "  CAST(NULL AS VARCHAR(128 OCTETS)) AS PROCEDURE_CAT, "
        "  CAST(NULL AS VARCHAR(128 OCTETS)) AS PROCEDURE_SCHEM, "
        "  CAST(NULL AS VARCHAR(128 OCTETS)) AS PROCEDURE_NAME, "
        "  CAST(NULL AS VARCHAR(128 OCTETS)) AS COLUMN_NAME, "
        "  CAST(NULL AS NATIVE_SMALLINT)     AS COLUMN_TYPE, "
        "  CAST(NULL AS NATIVE_SMALLINT)     AS DATA_TYPE, "
        "  CAST(NULL AS VARCHAR(128 OCTETS)) AS TYPE_NAME, "
        "  CAST(NULL AS NATIVE_INTEGER)      AS COLUMN_SIZE, "
        "  CAST(NULL AS NATIVE_INTEGER)      AS BUFFER_LENGTH, "
        "  CAST(NULL AS NATIVE_SMALLINT)     AS DECIMAL_DIGITS, "
        "  CAST(NULL AS NATIVE_SMALLINT)     AS NUM_PREC_RADIX, "
        "  CAST(NULL AS NATIVE_SMALLINT)     AS NULLABLE, "
        "  CAST(NULL AS VARCHAR(254 OCTETS)) AS REMARKS, "
        "  CAST(NULL AS VARCHAR(254 OCTETS)) AS COLUMN_DEF, "
        "  CAST(NULL AS NATIVE_SMALLINT)     AS SQL_DATA_TYPE, "
        "  CAST(NULL AS NATIVE_SMALLINT)     AS SQL_DATETIME_SUB, "
        "  CAST(NULL AS NATIVE_INTEGER)      AS CHAR_OCTET_LENGTH, "
        "  CAST(NULL AS NATIVE_INTEGER )     AS ORDINAL_POSITION, "
        "  CAST(NULL AS VARCHAR(254 OCTETS)) AS IS_NULLABLE "
        "FROM DUAL ";
    SQLCHAR * sWHERE    = (SQLCHAR*)"WHERE 1 = 0 ";
    SQLCHAR * sORDER_BY = (SQLCHAR*)"ORDER BY PROCEDURE_CAT, PROCEDURE_SCHEM, PROCEDURE_NAME, COLUMN_TYPE ";

    sStmt = (zlsStmt*)aStatementHandle;
    sDbc  = sStmt->mParentDbc;

    STL_TRY( zldDiagClear( SQL_HANDLE_STMT,
                           aStatementHandle,
                           aErrorStack ) == STL_SUCCESS );

    STL_TRY( zlcCheckSession( sDbc,
                              aErrorStack ) == STL_SUCCESS );
    
    switch( sDbc->mTransition )
    {
        case ZLC_TRANSITION_STATE_C0 :
        case ZLC_TRANSITION_STATE_C1 :
        case ZLC_TRANSITION_STATE_C2 :
        case ZLC_TRANSITION_STATE_C3 :
        case ZLC_TRANSITION_STATE_C4 :
            STL_THROW( RAMP_ERR_INVALID_HANDLE );
            break;
        case ZLC_TRANSITION_STATE_C5 :
        case ZLC_TRANSITION_STATE_C6 :
            break;
    }

    switch( sStmt->mTransition )
    {
        case ZLS_TRANSITION_STATE_S1 :
        case ZLS_TRANSITION_STATE_S2 :
        case ZLS_TRANSITION_STATE_S3 :
        case ZLS_TRANSITION_STATE_S4 :
            break;
        case ZLS_TRANSITION_STATE_S5 :
            STL_THROW( RAMP_ERR_INVALID_CURSOR_STATE_1 );
            break;
        case ZLS_TRANSITION_STATE_S6 :
        case ZLS_TRANSITION_STATE_S7 :
            STL_THROW( RAMP_ERR_INVALID_CURSOR_STATE_2 );
            break;
        case ZLS_TRANSITION_STATE_S8 :
        case ZLS_TRANSITION_STATE_S9 :
        case ZLS_TRANSITION_STATE_S10 :
        case ZLS_TRANSITION_STATE_S11 :
        case ZLS_TRANSITION_STATE_S12 :
            STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR );
            break;
    }
    
    if( aNameLength1 != SQL_NTS )
    {
        STL_TRY_THROW( aNameLength1 >= 0, RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH );

        sNameLength1 = aNameLength1;
    }
    else
    {
        if (aCatalogName != NULL)
        {
            sNameLength1 = stlStrlen((stlChar*)aCatalogName);
        }        
    }

    if( aNameLength2 != SQL_NTS )
    {
        STL_TRY_THROW( aNameLength2 >= 0, RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH );

        sNameLength2 = aNameLength2;
    }
    else
    {
        if (aSchemaName != NULL)
        {
            sNameLength2 = stlStrlen((stlChar*)aSchemaName);
        }
    }

    if( aNameLength3 != SQL_NTS )
    {
        STL_TRY_THROW( aNameLength3 >= 0, RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH );

        sNameLength3 = aNameLength3;
    }
    else
    {
        if (aProcName != NULL)
        {
            sNameLength3 = stlStrlen((stlChar*)aProcName);
        }
    }

    if( aNameLength4 != SQL_NTS )
    {
        STL_TRY_THROW( aNameLength4 >= 0, RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH );

        sNameLength4 = aNameLength4;
    }
    else
    {
        if (aColumnName != NULL)
        {
            sNameLength4 = stlStrlen((stlChar*)aColumnName);
        }
    }

    if( sStmt->mMetadataId == STL_TRUE )
    {
        if( aCatalogName != NULL )
        {
            if( (aCatalogName[0] == '\"') &&
                (aCatalogName[sNameLength1-1] == '\"') )
            {
                aCatalogName[sNameLength1-1] = 0;
                aCatalogName++;

                sNameLength1 = sNameLength1 - 2;
            }
            else
            {
                stlToupperString( (stlChar*)aCatalogName, (stlChar*)aCatalogName );
            }
        }
        
        STL_TRY_THROW( aSchemaName != NULL, RAMP_ERR_INVALID_USE_OF_NULL_POINTER );

        if( (aSchemaName[0] == '\"') &&
            (aSchemaName[sNameLength2-1] == '\"') )
        {
            aSchemaName[sNameLength2-1] = 0;
            aSchemaName++;

            sNameLength2 = sNameLength2 - 2;
        }
        else
        {
            stlToupperString( (stlChar*)aSchemaName, (stlChar*)aSchemaName );
        }

        STL_TRY_THROW( aProcName != NULL, RAMP_ERR_INVALID_USE_OF_NULL_POINTER );

        if( (aProcName[0] == '\"') &&
            (aProcName[sNameLength3-1] == '\"') )
        {
            aProcName[sNameLength3-1] = 0;
            aProcName++;

            sNameLength3 = sNameLength3 - 2;
        }
        else
        {
            stlToupperString( (stlChar*)aProcName, (stlChar*)aProcName );
        }
        
        STL_TRY_THROW( aColumnName != NULL, RAMP_ERR_INVALID_USE_OF_NULL_POINTER );

        if( (aColumnName[0] == '\"') &&
            (aColumnName[sNameLength4-1] == '\"') )
        {
            aColumnName[sNameLength4-1] = 0;
            aColumnName++;

            sNameLength4 = sNameLength4 - 2;
        }
        else
        {
            stlToupperString( (stlChar*)aColumnName, (stlChar*)aColumnName );
        }

        stlStrcpy( sOp, "=" );
    }
    else
    {
        stlStrcpy( sOp, "LIKE" );
    }

    sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos,
                         "%s%s",
                         sSELECT,
                         sWHERE );

    if( (aCatalogName != NULL) && (sNameLength1 > 0) )
    {
        /*
          sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos,
          " AND CURRENT_CATALOG = '" );
            
          stlMemcpy( sBuffer + sPos, aCatalogName, sNameLength1 );
          sPos += sNameLength1;

          sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos, "' " );
        */
    }

    if( (aSchemaName != NULL) && (sNameLength2 > 0) )
    {
        /*
          sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos,
          " AND TABLE_SCHEMA %s '", sOp );
            
          stlMemcpy( sBuffer + sPos, aSchemaName, sNameLength2 );
          sPos += sNameLength2;

          sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos, "' " );
        */
    }

    if( (aProcName != NULL) && (sNameLength3 > 0) )
    {
        /*
          sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos,
          " AND TABLE_NAME %s '", sOp );
            
          stlMemcpy( sBuffer + sPos, aProcName, sNameLength3 );
          sPos += sNameLength3;

          sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos, "' " );
        */
    }

    if( (aColumnName != NULL) && (sNameLength4 > 0) )
    {
        /*
          sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos,
          " AND COLUMN_NAME %s '", sOp );
            
          stlMemcpy( sBuffer + sPos, aColumnName, sNameLength4 );
          sPos += sNameLength4;

          sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos, "' " );
        */
    }

    sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos,
                         "%s",
                         sORDER_BY );

    STL_TRY( zlxExecDirect( sStmt,
                            sBuffer,
                            &sAffectedRowCount,
                            &sSuccessWithInfo,
                            aErrorStack ) == STL_SUCCESS );

    STL_TRY_THROW( sSuccessWithInfo == STL_FALSE, RAMP_SUCCESS_WITH_INFO );

    sRet = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_STRING_OR_BUFFER_LENGTH,
                      "The value of one of the length arguments was less than 0 "
                      "but not equal to SQL_NTS.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_USE_OF_NULL_POINTER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_USE_OF_NULL_POINTER,
                      "The SQL_ATTR_METADATA_ID statement attribute was set to SQL_TRUE, and "
                      "the SchemaName, ProcName, or ColumnName argument was a null pointer.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_CATCH( RAMP_SUCCESS_WITH_INFO )
    {
        sRet = SQL_SUCCESS_WITH_INFO;
    }

    STL_CATCH( RAMP_ERR_INVALID_CURSOR_STATE_1 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CURSOR_STATE,
                      "A cursor was open on the StatementHandle, "
                      "but SQLFetch or SQLFetchScroll had not been called.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_CURSOR_STATE_2 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CURSOR_STATE,
                      "A cursor was open on the StatementHandle, "
                      "and SQLFetch or SQLFetchScroll had been called.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    return sRet;
}

SQLRETURN SQL_API SQLProcedureColumns( SQLHSTMT      aStatementHandle,
                                       SQLCHAR     * aCatalogName,
                                       SQLSMALLINT   aNameLength1,
                                       SQLCHAR     * aSchemaName,
                                       SQLSMALLINT   aNameLength2,
                                       SQLCHAR     * aProcName,
                                       SQLSMALLINT   aNameLength3,
                                       SQLCHAR     * aColumnName,
                                       SQLSMALLINT   aNameLength4 )
{
    SQLRETURN       sRet  = SQL_ERROR;
    zlsStmt       * sStmt = NULL;
    stlErrorStack   sErrorStack;
    stlErrorData  * sErrorData = NULL;
    stlRegionMark   sMark;
    stlInt32        sState = 0;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sStmt = (zlsStmt*)aStatementHandle;

    STL_TRY_THROW( sStmt != NULL,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sStmt->mType == SQL_HANDLE_STMT,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( stlMarkRegionMem( &sStmt->mAllocator,
                               &sMark,
                               &sErrorStack ) == STL_SUCCESS );
    sState = 1;

    sRet = zllProcedureColumns( aStatementHandle,
                                aCatalogName,
                                aNameLength1,
                                aSchemaName,
                                aNameLength2,
                                aProcName,
                                aNameLength3,
                                aColumnName,
                                aNameLength4,
                                &sErrorStack );

    STL_TRY( sRet == SQL_SUCCESS );

    sState = 0;
    STL_TRY( stlRestoreRegionMem( &sStmt->mAllocator,
                                  &sMark,
                                  STL_TRUE,
                                  &sErrorStack ) == STL_SUCCESS );

    zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

    return SQL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_FINISH;

    if( ( sRet == SQL_ERROR ) ||
        ( sRet == SQL_SUCCESS_WITH_INFO ) )
    {
        zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_STMT,
                               aStatementHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }

        if( sStmt != NULL )
        {
            zlcCheckSessionAndCleanup( ZLS_STMT_DBC(sStmt) );
        }
    }

    switch( sState )
    {
        case 1 :
            stlRestoreRegionMem( &sStmt->mAllocator, &sMark, STL_TRUE, &sErrorStack );
        default :
            break;
    }

    return sRet;
}

SQLRETURN SQL_API SQLProcedureColumnsW( SQLHSTMT      aStatementHandle,
                                        SQLWCHAR    * aCatalogName,
                                        SQLSMALLINT   aNameLength1,
                                        SQLWCHAR    * aSchemaName,
                                        SQLSMALLINT   aNameLength2,
                                        SQLWCHAR    * aProcName,
                                        SQLSMALLINT   aNameLength3,
                                        SQLWCHAR    * aColumnName,
                                        SQLSMALLINT   aNameLength4 )
{
    /**
     * aNameLength1
     *     Length of *CatalogName, in characters.
     * aNameLength2
     *     Length of *SchemaName, in characters.
     * aNameLength3
     *     Length of *ProcName, in characters.
     * aNameLength4
     *     Length of *ColumnName, in characters.
     */

    SQLRETURN       sRet  = SQL_ERROR;
    zlsStmt       * sStmt = NULL;
    stlErrorStack   sErrorStack;
    stlErrorData  * sErrorData = NULL;
    stlRegionMark   sMark;
    stlInt32        sState = 0;

    stlInt64        sBufferLength;
    SQLCHAR       * sCatalogName = NULL;
    stlInt64        sNameLength1 = 0;
    SQLCHAR       * sSchemaName = NULL;
    stlInt64        sNameLength2 = 0;
    SQLCHAR       * sProcName = NULL;
    stlInt64        sNameLength3 = 0;
    SQLCHAR       * sColumnName = NULL;
    stlInt64        sNameLength4 = 0;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sStmt = (zlsStmt*)aStatementHandle;

    STL_TRY_THROW( sStmt != NULL,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sStmt->mType == SQL_HANDLE_STMT,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( stlMarkRegionMem( &sStmt->mAllocator,
                               &sMark,
                               &sErrorStack ) == STL_SUCCESS );
    sState = 1;

    sBufferLength = 0;    
    STL_TRY( zlvAllocCharBuffer( aCatalogName,
                                 aNameLength1,
                                 ZLS_STMT_DBC(sStmt)->mCharacterSet,
                                 &sCatalogName,
                                 &sBufferLength,
                                 &sStmt->mAllocator,
                                 &sErrorStack ) == STL_SUCCESS );
    
    STL_TRY( zlvWcharToChar( aCatalogName,
                             aNameLength1,
                             ZLS_STMT_DBC(sStmt)->mCharacterSet,
                             sCatalogName,
                             sBufferLength,
                             &sNameLength1,
                             &sErrorStack ) == STL_SUCCESS );
    
    sBufferLength = 0;    
    STL_TRY( zlvAllocCharBuffer( aSchemaName,
                                 aNameLength2,
                                 ZLS_STMT_DBC(sStmt)->mCharacterSet,
                                 &sSchemaName,
                                 &sBufferLength,
                                 &sStmt->mAllocator,
                                 &sErrorStack ) == STL_SUCCESS );
    
    STL_TRY( zlvWcharToChar( aSchemaName,
                             aNameLength2,
                             ZLS_STMT_DBC(sStmt)->mCharacterSet,
                             sSchemaName,
                             sBufferLength,
                             &sNameLength2,
                             &sErrorStack ) == STL_SUCCESS );
    
    sBufferLength = 0;    
    STL_TRY( zlvAllocCharBuffer( aProcName,
                                 aNameLength3,
                                 ZLS_STMT_DBC(sStmt)->mCharacterSet,
                                 &sProcName,
                                 &sBufferLength,
                                 &sStmt->mAllocator,
                                 &sErrorStack ) == STL_SUCCESS );
    
    STL_TRY( zlvWcharToChar( aProcName,
                             aNameLength3,
                             ZLS_STMT_DBC(sStmt)->mCharacterSet,
                             sProcName,
                             sBufferLength,
                             &sNameLength3,
                             &sErrorStack ) == STL_SUCCESS );
    
    sBufferLength = 0;    
    STL_TRY( zlvAllocCharBuffer( aColumnName,
                                 aNameLength4,
                                 ZLS_STMT_DBC(sStmt)->mCharacterSet,
                                 &sColumnName,
                                 &sBufferLength,
                                 &sStmt->mAllocator,
                                 &sErrorStack ) == STL_SUCCESS );
    
    STL_TRY( zlvWcharToChar( aColumnName,
                             aNameLength4,
                             ZLS_STMT_DBC(sStmt)->mCharacterSet,
                             sColumnName,
                             sBufferLength,
                             &sNameLength4,
                             &sErrorStack ) == STL_SUCCESS );
    
    sRet = zllProcedureColumns( aStatementHandle,
                                sCatalogName,
                                sNameLength1,
                                sSchemaName,
                                sNameLength2,
                                sProcName,
                                sNameLength3,
                                sColumnName,
                                sNameLength4,
                                &sErrorStack );

    STL_TRY( sRet == SQL_SUCCESS );

    sState = 0;
    STL_TRY( stlRestoreRegionMem( &sStmt->mAllocator,
                                  &sMark,
                                  STL_TRUE,
                                  &sErrorStack ) == STL_SUCCESS );

    zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

    return SQL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_FINISH;

    if( ( sRet == SQL_ERROR ) ||
        ( sRet == SQL_SUCCESS_WITH_INFO ) )
    {
        zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_STMT,
                               aStatementHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }

        if( sStmt != NULL )
        {
            zlcCheckSessionAndCleanup( ZLS_STMT_DBC(sStmt) );
        }
    }

    switch( sState )
    {
        case 1 :
            (void)stlRestoreRegionMem( &sStmt->mAllocator, &sMark, STL_TRUE, &sErrorStack );
        default :
            break;
    }

    return sRet;
}

static SQLRETURN zllProcedures( SQLHSTMT        aStatementHandle,
                                SQLCHAR       * aCatalogName,
                                SQLSMALLINT     aNameLength1,
                                SQLCHAR       * aSchemaName,
                                SQLSMALLINT     aNameLength2,
                                SQLCHAR       * aProcName,
                                SQLSMALLINT     aNameLength3,
                                stlErrorStack * aErrorStack )
{
    SQLRETURN       sRet              = SQL_ERROR;
    zlcDbc        * sDbc              = NULL;
    zlsStmt       * sStmt             = NULL;
    stlInt64        sAffectedRowCount = -1;
    stlBool         sSuccessWithInfo  = STL_FALSE;
    
    stlChar         sBuffer[2048];
    stlInt32        sBufferLength = STL_SIZEOF( sBuffer );

    stlInt16        sNameLength1 = 0;
    stlInt16        sNameLength2 = 0;
    stlInt16        sNameLength3 = 0;

    stlChar         sOp[5];
    stlInt32        sPos = 0;

    SQLCHAR * sSELECT   = (SQLCHAR*)"SELECT "
        "  CAST(NULL AS VARCHAR(128 OCTETS)) AS PROCEDURE_CAT, "
        "  CAST(NULL AS VARCHAR(128 OCTETS)) AS PROCEDURE_SCHEM, "
        "  CAST(NULL AS VARCHAR(128 OCTETS)) AS PROCEDURE_NAME, "
        "  CAST(NULL AS NATIVE_INTEGER)      AS NUM_INPUT_PARAMS, "
        "  CAST(NULL AS NATIVE_INTEGER)      AS NUM_OUTPUT_PARAMS, "
        "  CAST(NULL AS NATIVE_INTEGER)      AS NUM_RESULT_SETS, "
        "  CAST(NULL AS VARCHAR(254 OCTETS)) AS REMARKS, "
        "  CAST(NULL AS NATIVE_SMALLINT)     AS PROCEDURE_TYPE "
        "FROM DUAL ";
    SQLCHAR * sWHERE    = (SQLCHAR*)"WHERE 1 = 0 ";
    SQLCHAR * sORDER_BY = (SQLCHAR*)"ORDER BY PROCEDURE_CAT, PROCEDURE_SCHEM, PROCEDURE_NAME";
    
    sStmt = (zlsStmt*)aStatementHandle;
    sDbc  = sStmt->mParentDbc;

    STL_TRY( zldDiagClear( SQL_HANDLE_STMT,
                           aStatementHandle,
                           aErrorStack ) == STL_SUCCESS );

    STL_TRY( zlcCheckSession( sDbc,
                              aErrorStack ) == STL_SUCCESS );
    
    switch( sDbc->mTransition )
    {
        case ZLC_TRANSITION_STATE_C0 :
        case ZLC_TRANSITION_STATE_C1 :
        case ZLC_TRANSITION_STATE_C2 :
        case ZLC_TRANSITION_STATE_C3 :
        case ZLC_TRANSITION_STATE_C4 :
            STL_THROW( RAMP_ERR_INVALID_HANDLE );
            break;
        case ZLC_TRANSITION_STATE_C5 :
        case ZLC_TRANSITION_STATE_C6 :
            break;
    }

    switch( sStmt->mTransition )
    {
        case ZLS_TRANSITION_STATE_S1 :
        case ZLS_TRANSITION_STATE_S2 :
        case ZLS_TRANSITION_STATE_S3 :
        case ZLS_TRANSITION_STATE_S4 :
            break;
        case ZLS_TRANSITION_STATE_S5 :
            STL_THROW( RAMP_ERR_INVALID_CURSOR_STATE_1 );
            break;
        case ZLS_TRANSITION_STATE_S6 :
        case ZLS_TRANSITION_STATE_S7 :
            STL_THROW( RAMP_ERR_INVALID_CURSOR_STATE_2 );
            break;
        case ZLS_TRANSITION_STATE_S8 :
        case ZLS_TRANSITION_STATE_S9 :
        case ZLS_TRANSITION_STATE_S10 :
        case ZLS_TRANSITION_STATE_S11 :
        case ZLS_TRANSITION_STATE_S12 :
            STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR );
            break;
    }

    if( aNameLength1 != SQL_NTS )
    {
        STL_TRY_THROW( aNameLength1 >= 0, RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH );

        sNameLength1 = aNameLength1;
    }
    else
    {
        if (aCatalogName != NULL)
        {
            sNameLength1 = stlStrlen((stlChar*)aCatalogName);
        }
    }

    if( aNameLength2 != SQL_NTS )
    {
        STL_TRY_THROW( aNameLength2 >= 0, RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH );

        sNameLength2 = aNameLength2;
    }
    else
    {
        if (aSchemaName != NULL)
        {
            sNameLength2 = stlStrlen((stlChar*)aSchemaName);
        }
    }

    if( aNameLength3 != SQL_NTS )
    {
        STL_TRY_THROW( aNameLength3 >= 0, RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH );

        sNameLength3 = aNameLength3;
    }
    else
    {
        if (aProcName != NULL)
        {
            sNameLength3 = stlStrlen((stlChar*)aProcName);
        }
    }

    if( sStmt->mMetadataId == STL_TRUE )
    {
        if( aCatalogName != NULL )
        {
            if( (aCatalogName[0] == '\"') &&
                (aCatalogName[sNameLength1-1] == '\"') )
            {
                aCatalogName[sNameLength1-1] = 0;
                aCatalogName++;

                sNameLength1 = sNameLength1 - 2;
            }
            else
            {
                stlToupperString( (stlChar*)aCatalogName, (stlChar*)aCatalogName );
            }
        }

        STL_TRY_THROW( aSchemaName != NULL, RAMP_ERR_INVALID_USE_OF_NULL_POINTER );

        if( (aSchemaName[0] == '\"') &&
            (aSchemaName[sNameLength2-1] == '\"') )
        {
            aSchemaName[sNameLength2-1] = 0;
            aSchemaName++;

            sNameLength2 = sNameLength2 - 2;
        }
        else
        {
            stlToupperString( (stlChar*)aSchemaName, (stlChar*)aSchemaName );
        }

        STL_TRY_THROW( aProcName != NULL, RAMP_ERR_INVALID_USE_OF_NULL_POINTER );
    
        if( (aProcName[0] == '\"') &&
            (aProcName[sNameLength3-1] == '\"') )
        {
            aProcName[sNameLength3-1] = 0;
            aProcName++;

            sNameLength3 = sNameLength3 - 2;
        }
        else
        {
            stlToupperString( (stlChar*)aProcName, (stlChar*)aProcName );
        }

        stlStrcpy( sOp, "=" );
    }
    else
    {
        stlStrcpy( sOp, "LIKE" );
    }

    sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos,
                         "%s%s",
                         sSELECT,
                         sWHERE );

    if( (aCatalogName != NULL) && (sNameLength1 > 0) )
    {
        /*
          sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos,
          " AND PROCEDURE_CATALOG = '" );
            
          stlMemcpy( sBuffer + sPos, aCatalogName, sNameLength1 );
          sPos += sNameLength1;

          sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos, "' " );
        */
    }

    if( (aSchemaName != NULL) && (sNameLength2 > 0) )
    {
        /*
          sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos,
          " AND PROCEDURE_SCHEMA %s '", sOp );
            
          stlMemcpy( sBuffer + sPos, aSchemaName, sNameLength2 );
          sPos += sNameLength2;

          sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos, "' " );
        */
    }

    if( (aProcName != NULL) && (sNameLength3 > 0) )
    {
        /*
          sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos,
          " AND PROCEDURE_NAME %s '", sOp );
            
          stlMemcpy( sBuffer + sPos, aProcName, sNameLength3 );
          sPos += sNameLength3;

          sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos, "' " );
        */
    }
    
    sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos,
                         "%s",
                         sORDER_BY );

    STL_TRY( zlxExecDirect( sStmt,
                            sBuffer,
                            &sAffectedRowCount,
                            &sSuccessWithInfo,
                            aErrorStack ) == STL_SUCCESS );

    STL_TRY_THROW( sSuccessWithInfo == STL_FALSE, RAMP_SUCCESS_WITH_INFO );

    sRet = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_STRING_OR_BUFFER_LENGTH,
                      "The value of one of the length arguments was less than 0 "
                      "but not equal to SQL_NTS.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_CURSOR_STATE_1 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CURSOR_STATE,
                      "A cursor was open on the StatementHandle, "
                      "but SQLFetch or SQLFetchScroll had not been called.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_CURSOR_STATE_2 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CURSOR_STATE,
                      "A cursor was open on the StatementHandle, "
                      "and SQLFetch or SQLFetchScroll had been called.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_USE_OF_NULL_POINTER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_USE_OF_NULL_POINTER,
                      "The SQL_ATTR_METADATA_ID statement attribute was set to SQL_TRUE, "
                      "and the SchemaName or ProcName argument was a null pointer.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_CATCH( RAMP_SUCCESS_WITH_INFO )
    {
        sRet = SQL_SUCCESS_WITH_INFO;
    }

    STL_FINISH;

    return sRet;
}

SQLRETURN SQL_API SQLProcedures( SQLHSTMT      aStatementHandle,
                                 SQLCHAR     * aCatalogName,
                                 SQLSMALLINT   aNameLength1,
                                 SQLCHAR     * aSchemaName,
                                 SQLSMALLINT   aNameLength2,
                                 SQLCHAR     * aProcName,
                                 SQLSMALLINT   aNameLength3 )
{
    SQLRETURN       sRet  = SQL_ERROR;
    zlsStmt       * sStmt = NULL;
    stlErrorStack   sErrorStack;
    stlErrorData  * sErrorData = NULL;
    stlRegionMark   sMark;
    stlInt32        sState = 0;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sStmt = (zlsStmt*)aStatementHandle;

    STL_TRY_THROW( sStmt != NULL,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sStmt->mType == SQL_HANDLE_STMT,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( stlMarkRegionMem( &sStmt->mAllocator,
                               &sMark,
                               &sErrorStack ) == STL_SUCCESS );
    sState = 1;

    sRet = zllProcedures( aStatementHandle,
                          aCatalogName,
                          aNameLength1,
                          aSchemaName,
                          aNameLength2,
                          aProcName,
                          aNameLength3,
                          &sErrorStack );
    
    STL_TRY( sRet == SQL_SUCCESS );

    sState = 0;
    STL_TRY( stlRestoreRegionMem( &sStmt->mAllocator,
                                  &sMark,
                                  STL_TRUE,
                                  &sErrorStack ) == STL_SUCCESS );

    zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

    return SQL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_FINISH;

    if( ( sRet == SQL_ERROR ) ||
        ( sRet == SQL_SUCCESS_WITH_INFO ) )
    {
        zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_STMT,
                               aStatementHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }

        if( sStmt != NULL )
        {
            zlcCheckSessionAndCleanup( ZLS_STMT_DBC(sStmt) );
        }
    }

    switch( sState )
    {
        case 1 :
            (void)stlRestoreRegionMem( &sStmt->mAllocator, &sMark, STL_TRUE, &sErrorStack );
        default :
            break;
    }

    return sRet;
}

SQLRETURN SQL_API SQLProceduresW( SQLHSTMT      aStatementHandle,
                                  SQLWCHAR    * aCatalogName,
                                  SQLSMALLINT   aNameLength1,
                                  SQLWCHAR    * aSchemaName,
                                  SQLSMALLINT   aNameLength2,
                                  SQLWCHAR    * aProcName,
                                  SQLSMALLINT   aNameLength3 )
{
    /**
     * aNameLength1
     *     Length of *CatalogName, in characters.
     * aNameLength2
     *     Length of *SchemaName, in characters.
     * aNameLength3
     *     Length of *ProcName, in characters.
     */

    SQLRETURN       sRet  = SQL_ERROR;
    zlsStmt       * sStmt = NULL;
    stlErrorStack   sErrorStack;
    stlErrorData  * sErrorData = NULL;
    stlRegionMark   sMark;
    stlInt32        sState = 0;

    stlInt64        sBufferLength;
    SQLCHAR       * sCatalogName = NULL;
    stlInt64        sNameLength1 = 0;
    SQLCHAR       * sSchemaName = NULL;
    stlInt64        sNameLength2 = 0;
    SQLCHAR       * sProcName = NULL;
    stlInt64        sNameLength3 = 0;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sStmt = (zlsStmt*)aStatementHandle;

    STL_TRY_THROW( sStmt != NULL,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sStmt->mType == SQL_HANDLE_STMT,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( stlMarkRegionMem( &sStmt->mAllocator,
                               &sMark,
                               &sErrorStack ) == STL_SUCCESS );
    sState = 1;

    sBufferLength = 0;    
    STL_TRY( zlvAllocCharBuffer( aCatalogName,
                                 aNameLength1,
                                 ZLS_STMT_DBC(sStmt)->mCharacterSet,
                                 &sCatalogName,
                                 &sBufferLength,
                                 &sStmt->mAllocator,
                                 &sErrorStack ) == STL_SUCCESS );
    
    STL_TRY( zlvWcharToChar( aCatalogName,
                             aNameLength1,
                             ZLS_STMT_DBC(sStmt)->mCharacterSet,
                             sCatalogName,
                             sBufferLength,
                             &sNameLength1,
                             &sErrorStack ) == STL_SUCCESS );
    
    sBufferLength = 0;    
    STL_TRY( zlvAllocCharBuffer( aSchemaName,
                                 aNameLength2,
                                 ZLS_STMT_DBC(sStmt)->mCharacterSet,
                                 &sSchemaName,
                                 &sBufferLength,
                                 &sStmt->mAllocator,
                                 &sErrorStack ) == STL_SUCCESS );
    
    STL_TRY( zlvWcharToChar( aSchemaName,
                             aNameLength2,
                             ZLS_STMT_DBC(sStmt)->mCharacterSet,
                             sSchemaName,
                             sBufferLength,
                             &sNameLength2,
                             &sErrorStack ) == STL_SUCCESS );
    
    sBufferLength = 0;    
    STL_TRY( zlvAllocCharBuffer( aProcName,
                                 aNameLength3,
                                 ZLS_STMT_DBC(sStmt)->mCharacterSet,
                                 &sProcName,
                                 &sBufferLength,
                                 &sStmt->mAllocator,
                                 &sErrorStack ) == STL_SUCCESS );
    
    STL_TRY( zlvWcharToChar( aProcName,
                             aNameLength3,
                             ZLS_STMT_DBC(sStmt)->mCharacterSet,
                             sProcName,
                             sBufferLength,
                             &sNameLength3,
                             &sErrorStack ) == STL_SUCCESS );
    
    sRet = zllProcedures( aStatementHandle,
                          sCatalogName,
                          sNameLength1,
                          sSchemaName,
                          sNameLength2,
                          sProcName,
                          sNameLength3,
                          &sErrorStack );

    STL_TRY( sRet == SQL_SUCCESS );

    sState = 0;
    STL_TRY( stlRestoreRegionMem( &sStmt->mAllocator,
                                  &sMark,
                                  STL_TRUE,
                                  &sErrorStack ) == STL_SUCCESS );

    zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

    return SQL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_FINISH;

    if( ( sRet == SQL_ERROR ) ||
        ( sRet == SQL_SUCCESS_WITH_INFO ) )
    {
        zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_STMT,
                               aStatementHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }

        if( sStmt != NULL )
        {
            zlcCheckSessionAndCleanup( ZLS_STMT_DBC(sStmt) );
        }
    }

    switch( sState )
    {
        case 1 :
            (void)stlRestoreRegionMem( &sStmt->mAllocator, &sMark, STL_TRUE, &sErrorStack );
        default :
            break;
    }

    return sRet;
}

SQLRETURN SQL_API SQLPutData( SQLHSTMT   aStatementHandle,
                              SQLPOINTER aDataPtr,
                              SQLLEN     aStrLen_or_Ind )
{
    SQLRETURN       sRet              = SQL_ERROR;
    zlcDbc        * sDbc              = NULL;
    zlsStmt       * sStmt             = NULL;
    stlErrorData  * sErrorData        = NULL;
    stlErrorStack   sErrorStack;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sStmt = (zlsStmt*)aStatementHandle;

    STL_TRY_THROW( sStmt != NULL, RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sStmt->mType == SQL_HANDLE_STMT,
                   RAMP_ERR_INVALID_HANDLE );

    sDbc = sStmt->mParentDbc;

    STL_TRY( zldDiagClear( SQL_HANDLE_STMT,
                           aStatementHandle,
                           &sErrorStack ) == STL_SUCCESS );

    STL_TRY( zlcCheckSession( sDbc,
                              &sErrorStack ) == STL_SUCCESS );
    
    switch( sStmt->mTransition )
    {
        case ZLS_TRANSITION_STATE_S1 :
        case ZLS_TRANSITION_STATE_S2 :
        case ZLS_TRANSITION_STATE_S3 :
        case ZLS_TRANSITION_STATE_S4 :
        case ZLS_TRANSITION_STATE_S5 :
        case ZLS_TRANSITION_STATE_S6 :
        case ZLS_TRANSITION_STATE_S7 :
        case ZLS_TRANSITION_STATE_S8 :
            STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR1 );
            break;
        case ZLS_TRANSITION_STATE_S9 :
        case ZLS_TRANSITION_STATE_S10 :
            break;
        case ZLS_TRANSITION_STATE_S11 :
        case ZLS_TRANSITION_STATE_S12 :
            STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR2 );
            break;
    }

    if( aDataPtr == NULL )
    {
        STL_TRY_THROW( (aStrLen_or_Ind == 0) ||
                       (aStrLen_or_Ind == SQL_NULL_DATA),
                       RAMP_ERR_INVALID_USE_OF_NULL_POINTER );
    }
    else
    {
        STL_TRY_THROW( (aStrLen_or_Ind >= 0) ||
                       (aStrLen_or_Ind == SQL_NTS) ||
                       (aStrLen_or_Ind == SQL_NULL_DATA),
                       RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH );
    }

    STL_TRY( zliPutData( sStmt,
                         aDataPtr,
                         aStrLen_or_Ind,
                         &sErrorStack ) == STL_SUCCESS );
    
    sStmt->mTransition = ZLS_TRANSITION_STATE_S10;

    sRet = SQL_SUCCESS;

    zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

    return sRet;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR1 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      "The previous function call was not a call to "
                      "SQLPutData or SQLParamData.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR2 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      NULL,
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_USE_OF_NULL_POINTER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_USE_OF_NULL_POINTER,
                      "The argument DataPtr was a null pointer, "
                      "and the argument StrLen_or_Ind was not 0, SQL_DEFAULT_PARAM, or "
                      "SQL_NULL_DATA.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_STRING_OR_BUFFER_LENGTH,
                      "The argument DataPtr was not a null pointer, "
                      "and the argument StrLen_or_Ind was less than 0 "
                      "but not equal to SQL_NTS or SQL_NULL_DATA.",
                      &sErrorStack );
    }

    STL_FINISH;

    if( sRet == SQL_ERROR )
    {
        zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_STMT,
                               aStatementHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }
        
        if( sStmt != NULL )
        {
            zlcCheckSessionAndCleanup( ZLS_STMT_DBC(sStmt) );
        }
    }

    return sRet;
}

SQLRETURN SQL_API SQLRowCount( SQLHSTMT   aStatementHandle,
                               SQLLEN   * aRowCountPtr )
{
    SQLRETURN       sRet        = SQL_ERROR;
    zlsStmt       * sStmt       = NULL;
    stlErrorData  * sErrorData  = NULL;
    stlErrorStack   sErrorStack;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sStmt = (zlsStmt*)aStatementHandle;

    STL_TRY_THROW( sStmt != NULL, RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sStmt->mType == SQL_HANDLE_STMT,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( zldDiagClear( SQL_HANDLE_STMT,
                           aStatementHandle,
                           &sErrorStack ) == STL_SUCCESS );

    STL_TRY( zlcCheckSession( sStmt->mParentDbc,
                              &sErrorStack ) == STL_SUCCESS );
    
    switch( sStmt->mTransition )
    {
        case ZLS_TRANSITION_STATE_S0 :
            STL_THROW( RAMP_ERR_INVALID_HANDLE );
            break;
        case ZLS_TRANSITION_STATE_S1 :
        case ZLS_TRANSITION_STATE_S2 :
        case ZLS_TRANSITION_STATE_S3 :
            STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR1 );
            break;
        case ZLS_TRANSITION_STATE_S8 :
        case ZLS_TRANSITION_STATE_S9 :
        case ZLS_TRANSITION_STATE_S10 :
        case ZLS_TRANSITION_STATE_S11 :
        case ZLS_TRANSITION_STATE_S12 :
            STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR2 );
            break;
        default :
            break;
    }

    if( aRowCountPtr != NULL )
    {
        *aRowCountPtr = (SQLLEN)sStmt->mRowCount;
    }

    sRet = SQL_SUCCESS;
    
    zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

    return sRet;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR1 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      "The function was called prior to calling SQLExecute, SQLExecDirect, "
                      "SQLBulkOperations, or SQLSetPos for the StatementHandle.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR2 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      NULL,
                      &sErrorStack );
    }

    STL_FINISH;

    if( sRet == SQL_ERROR )
    {
        zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_STMT,
                               aStatementHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }
    }

    return sRet;
}

static SQLRETURN zllSetConnectAttr( SQLHDBC         aConnectionHandle,
                                    SQLINTEGER      aAttribute,
                                    SQLPOINTER      aValuePtr,
                                    SQLINTEGER      aStringLength,
                                    stlErrorStack * aErrorStack )
{
    SQLRETURN   sRet        = SQL_ERROR;
    zlcDbc    * sDbc        = NULL;
    
    sDbc = (zlcDbc*)aConnectionHandle;

    STL_TRY( zldDiagClear( SQL_HANDLE_DBC,
                           aConnectionHandle,
                           aErrorStack ) == STL_SUCCESS );

    switch( sDbc->mTransition )
    {
        case ZLC_TRANSITION_STATE_C0 :
        case ZLC_TRANSITION_STATE_C1 :
            STL_THROW( RAMP_ERR_INVALID_HANDLE );
            break;
        case ZLC_TRANSITION_STATE_C2 :
            switch( aAttribute )
            {
                case SQL_ATTR_TRANSLATE_LIB :
                case SQL_ATTR_TRANSLATE_OPTION :
                    STL_THROW( RAMP_ERR_CONNECTION_NOT_OPEN );
                    break;
                default :
                    break;
            }
            break;
        case ZLC_TRANSITION_STATE_C3 :
            STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR );
            break;
        case ZLC_TRANSITION_STATE_C4 :
        case ZLC_TRANSITION_STATE_C5 :
            STL_TRY( zlcCheckSession( sDbc,
                                      aErrorStack ) == STL_SUCCESS );
            break;
        case ZLC_TRANSITION_STATE_C6 :
            STL_TRY_THROW( aAttribute != SQL_ATTR_TXN_ISOLATION,
                           RAMP_ERR_ATTRIBUTE_CANNOT_BE_SET_NOW );

            STL_TRY( zlcCheckSession( sDbc,
                                      aErrorStack ) == STL_SUCCESS );
            break;
    }
    
    switch( aAttribute )
    {
        case SQL_ATTR_ACCESS_MODE :
        case SQL_ATTR_AUTOCOMMIT :
        case SQL_ATTR_CURRENT_CATALOG :
        case SQL_ATTR_LOGIN_TIMEOUT :
        case SQL_ATTR_MAX_ROWS :
        case SQL_ATTR_METADATA_ID :
        case SQL_ATTR_QUERY_TIMEOUT :
        case SQL_ATTR_TXN_ISOLATION :
        case SQL_ATTR_TIMEZONE:
        case SQL_ATTR_CHARACTER_SET :
        case SQL_ATTR_DATE_FORMAT :
        case SQL_ATTR_TIME_FORMAT :
        case SQL_ATTR_TIME_WITH_TIMEZONE_FORMAT :
        case SQL_ATTR_TIMESTAMP_FORMAT :
        case SQL_ATTR_TIMESTAMP_WITH_TIMEZONE_FORMAT :
        case SQL_ATTR_OLDPWD :
            break;
        case SQL_ATTR_DATABASE_CHARACTER_SET :
        case SQL_ATTR_AUTO_IPD :
        case SQL_ATTR_CONNECTION_DEAD :
            STL_THROW( RAMP_ERR_INVALID_ATTRIBUTE_OPTION_IDENTIFIER1 );
            break;
        case SQL_ATTR_ASYNC_DBC_FUNCTIONS_ENABLE :
        case SQL_ATTR_ASYNC_ENABLE :
        case SQL_ATTR_CONNECTION_TIMEOUT :
        case SQL_ATTR_ENLIST_IN_DTC :
        case SQL_ATTR_ODBC_CURSORS :
        case SQL_ATTR_PACKET_SIZE :
        case SQL_ATTR_QUIET_MODE :
        case SQL_ATTR_TRACE :
        case SQL_ATTR_TRACEFILE :
        case SQL_ATTR_TRANSLATE_LIB :
        case SQL_ATTR_TRANSLATE_OPTION :
            STL_THROW( RAMP_ERR_OPTIONAL_FEATURE_NOT_IMPLEMENTED );
            break;
        default:
            STL_THROW( RAMP_ERR_INVALID_ATTRIBUTE_OPTION_IDENTIFIER2 );
            break;
    }

    switch( aAttribute )
    {
        case SQL_ATTR_CURRENT_CATALOG :
        case SQL_ATTR_TIMEZONE :
        case SQL_ATTR_CHARACTER_SET :
        case SQL_ATTR_DATABASE_CHARACTER_SET :
        case SQL_ATTR_DATE_FORMAT :
        case SQL_ATTR_TIME_FORMAT :
        case SQL_ATTR_TIME_WITH_TIMEZONE_FORMAT :
        case SQL_ATTR_TIMESTAMP_FORMAT :
        case SQL_ATTR_TIMESTAMP_WITH_TIMEZONE_FORMAT :
        case SQL_ATTR_OLDPWD :
            if( aStringLength != SQL_NTS )
            {
                STL_TRY_THROW( aStringLength >= 0, RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH );
            }
            break;
        default :
            break;
    }

    STL_TRY( zlcSetAttr( sDbc,
                         aAttribute,
                         aValuePtr,
                         aStringLength,
                         aErrorStack ) == STL_SUCCESS );

    sRet = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_ERR_CONNECTION_NOT_OPEN )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_CONNECTION_NOT_OPEN,
                      "An Attribute value was specified that required an open connection, "
                      "but the ConnectionHandle was not in a connected state.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_ATTRIBUTE_OPTION_IDENTIFIER1 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_ATTRIBUTE_OPTION_IDENTIFIER,
                      "The value specified for the argument Attribute was a "
                      "read-only attribute.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_ATTRIBUTE_OPTION_IDENTIFIER2 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_ATTRIBUTE_OPTION_IDENTIFIER,
                      "The value specified for the argument Attribute was not "
                      "valid for the version of ODBC supported by the driver.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_ATTRIBUTE_CANNOT_BE_SET_NOW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_ATTRIBUTE_CANNOT_BE_SET_NOW,
                      "The Attribute argument was SQL_ATTR_TXN_ISOLATION, "
                      "and a transaction was open.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_OPTIONAL_FEATURE_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_OPTIONAL_FEATURE_NOT_IMPLEMENTED,
                      "The value specified for the argument Attribute was a valid "
                      "ODBC connection or statement attribute for the version of "
                      "ODBC supported by the driver but was not supported by the driver.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_STRING_OR_BUFFER_LENGTH,
                      "*ValuePtr is a character string, and the StringLength argument was less than 0 "
                      "but was not SQL_NTS.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_FINISH;

    return sRet;
}

SQLRETURN SQL_API SQLSetConnectAttr( SQLHDBC    aConnectionHandle,
                                     SQLINTEGER aAttribute,
                                     SQLPOINTER aValuePtr,
                                     SQLINTEGER aStringLength )
{
    SQLRETURN       sRet = SQL_ERROR;
    zlcDbc        * sDbc = NULL;
    stlErrorStack   sErrorStack;
    stlErrorData  * sErrorData = NULL;
    stlRegionMark   sMark;
    stlInt32        sState = 0;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sDbc = (zlcDbc*)aConnectionHandle;
    
    STL_TRY_THROW( sDbc != NULL, RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sDbc->mType == SQL_HANDLE_DBC,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( stlMarkRegionMem( &sDbc->mAllocator,
                               &sMark,
                               &sErrorStack ) == STL_SUCCESS );
    sState = 1;

    sRet = zllSetConnectAttr( aConnectionHandle,
                              aAttribute,
                              aValuePtr,
                              aStringLength,
                              &sErrorStack );

    STL_TRY( sRet == SQL_SUCCESS );

    sState = 0;
    STL_TRY( stlRestoreRegionMem( &sDbc->mAllocator,
                                  &sMark,
                                  STL_TRUE,
                                  &sErrorStack ) == STL_SUCCESS );

    zldDiagSetRetCode( SQL_HANDLE_DBC, aConnectionHandle, sRet );

    return SQL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_FINISH;

    if( ( sRet == SQL_ERROR ) ||
        ( sRet == SQL_SUCCESS_WITH_INFO ) )
    {
        zldDiagSetRetCode( SQL_HANDLE_DBC, aConnectionHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_DBC,
                               aConnectionHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }
        
        zlcCheckSessionAndCleanup( sDbc );
    }

    switch( sState )
    {
        case 1 :
            (void)stlRestoreRegionMem( &sDbc->mAllocator, &sMark, STL_TRUE, &sErrorStack );
        default :
            break;
    }

    return sRet;
}

SQLRETURN SQL_API SQLSetConnectAttrW( SQLHDBC    aConnectionHandle,
                                      SQLINTEGER aAttribute,
                                      SQLPOINTER aValuePtr,
                                      SQLINTEGER aStringLength )
{
    /**
     * aStringLength
     *     f Attribute is an ODBC-defined attribute and ValuePtr points to
     *     a character string or a binary buffer, this argument should be
     *     the length of *ValuePtr. For character string data, this argument
     *     should contain the number of bytes in the string.
     */

    SQLRETURN       sRet = SQL_ERROR;
    zlcDbc        * sDbc = NULL;
    stlErrorStack   sErrorStack;
    stlErrorData  * sErrorData = NULL;
    stlRegionMark   sMark;
    stlInt32        sState = 0;

    SQLCHAR         sValue[SQL_MAX_OPTION_STRING_LENGTH + 1];
    stlInt64        sLength = 0;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sDbc = (zlcDbc*)aConnectionHandle;
    
    STL_TRY_THROW( sDbc != NULL, RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sDbc->mType == SQL_HANDLE_DBC,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( stlMarkRegionMem( &sDbc->mAllocator,
                               &sMark,
                               &sErrorStack ) == STL_SUCCESS );
    sState = 1;

    switch( aAttribute )
    {
        case SQL_ATTR_CURRENT_CATALOG :
        case SQL_ATTR_TIMEZONE :
        case SQL_ATTR_CHARACTER_SET :
        case SQL_ATTR_DATE_FORMAT :
        case SQL_ATTR_TIME_FORMAT :
        case SQL_ATTR_TIME_WITH_TIMEZONE_FORMAT :
        case SQL_ATTR_TIMESTAMP_FORMAT :
        case SQL_ATTR_TIMESTAMP_WITH_TIMEZONE_FORMAT :
        case SQL_ATTR_OLDPWD :
            if( aStringLength == SQL_NTS )
            {
                aStringLength = zlvStrlenW((SQLWCHAR*)aValuePtr) * STL_SIZEOF(SQLWCHAR);
            }
            else
            {
                STL_TRY_THROW( aStringLength >= 0, RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH );
            }

            STL_TRY( zlvWcharToChar( (SQLWCHAR*)aValuePtr,
                                     aStringLength /  STL_SIZEOF(SQLWCHAR),
                                     sDbc->mCharacterSet,
                                     sValue,
                                     STL_SIZEOF(sValue),
                                     &sLength,
                                     &sErrorStack ) == STL_SUCCESS );

            sRet = zllSetConnectAttr( aConnectionHandle,
                                      aAttribute,
                                      sValue,
                                      sLength,
                                      &sErrorStack );
            break;
        default :
            sRet = zllSetConnectAttr( aConnectionHandle,
                                      aAttribute,
                                      aValuePtr,
                                      aStringLength,
                                      &sErrorStack );
            break;
    }

    STL_TRY( sRet == SQL_SUCCESS );

    sState = 0;
    STL_TRY( stlRestoreRegionMem( &sDbc->mAllocator,
                                  &sMark,
                                  STL_TRUE,
                                  &sErrorStack ) == STL_SUCCESS );

    zldDiagSetRetCode( SQL_HANDLE_DBC, aConnectionHandle, sRet );

    return SQL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_CATCH( RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_STRING_OR_BUFFER_LENGTH,
                      "*ValuePtr is a character string, and the StringLength argument was less than 0 "
                      "but was not SQL_NTS.",
                      &sErrorStack );
    }

    STL_FINISH;

    if( ( sRet == SQL_ERROR ) ||
        ( sRet == SQL_SUCCESS_WITH_INFO ) )
    {
        zldDiagSetRetCode( SQL_HANDLE_DBC, aConnectionHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_DBC,
                               aConnectionHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }
        
        zlcCheckSessionAndCleanup( sDbc );
    }

    switch( sState )
    {
        case 1 :
            (void)stlRestoreRegionMem( &sDbc->mAllocator, &sMark, STL_TRUE, &sErrorStack );
        default :
            break;
    }

    return sRet;
}

SQLRETURN SQL_API SQLSetConnectOption( SQLHDBC      aConnectionHandle,
                                       SQLUSMALLINT aOption,
                                       SQLULEN      aValue )
{
    SQLRETURN       sRet = SQL_ERROR;
    zlcDbc        * sDbc = NULL;
    stlErrorStack   sErrorStack;
    stlErrorData  * sErrorData = NULL;
    stlRegionMark   sMark;
    stlInt32        sState = 0;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sDbc = (zlcDbc*)aConnectionHandle;
    
    STL_TRY_THROW( sDbc != NULL, RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sDbc->mType == SQL_HANDLE_DBC,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( stlMarkRegionMem( &sDbc->mAllocator,
                               &sMark,
                               &sErrorStack ) == STL_SUCCESS );
    sState = 1;

    sRet = zllSetConnectAttr( aConnectionHandle,
                              aOption,
                              (SQLPOINTER)(stlVarInt)aValue,
                              SQL_NTS,
                              &sErrorStack );

    STL_TRY( sRet == SQL_SUCCESS );

    sState = 0;
    STL_TRY( stlRestoreRegionMem( &sDbc->mAllocator,
                                  &sMark,
                                  STL_TRUE,
                                  &sErrorStack ) == STL_SUCCESS );

    zldDiagSetRetCode( SQL_HANDLE_DBC, aConnectionHandle, sRet );

    return SQL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_FINISH;

    if( ( sRet == SQL_ERROR ) ||
        ( sRet == SQL_SUCCESS_WITH_INFO ) )
    {
        zldDiagSetRetCode( SQL_HANDLE_DBC, aConnectionHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_DBC,
                               aConnectionHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }
        
        zlcCheckSessionAndCleanup( sDbc );
    }

    switch( sState )
    {
        case 1 :
            (void)stlRestoreRegionMem( &sDbc->mAllocator, &sMark, STL_TRUE, &sErrorStack );
        default :
            break;
    }

    return sRet;
}

SQLRETURN SQL_API SQLSetConnectOptionW( SQLHDBC      aConnectionHandle,
                                        SQLUSMALLINT aOption,
                                        SQLULEN      aValue )
{
    SQLRETURN       sRet = SQL_ERROR;
    zlcDbc        * sDbc = NULL;
    stlErrorStack   sErrorStack;
    stlErrorData  * sErrorData = NULL;
    stlRegionMark   sMark;
    stlInt32        sState = 0;

    SQLCHAR         sValue[SQL_MAX_OPTION_STRING_LENGTH + 1];
    stlInt64        sLength = 0;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sDbc = (zlcDbc*)aConnectionHandle;
    
    STL_TRY_THROW( sDbc != NULL, RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sDbc->mType == SQL_HANDLE_DBC,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( stlMarkRegionMem( &sDbc->mAllocator,
                               &sMark,
                               &sErrorStack ) == STL_SUCCESS );
    sState = 1;

    switch( aOption )
    {
        case SQL_ATTR_CURRENT_CATALOG :
        case SQL_ATTR_TIMEZONE :
        case SQL_ATTR_CHARACTER_SET :
        case SQL_ATTR_DATE_FORMAT :
        case SQL_ATTR_TIME_FORMAT :
        case SQL_ATTR_TIME_WITH_TIMEZONE_FORMAT :
        case SQL_ATTR_TIMESTAMP_FORMAT :
        case SQL_ATTR_TIMESTAMP_WITH_TIMEZONE_FORMAT :
        case SQL_ATTR_OLDPWD :
            STL_TRY( zlvWcharToChar( (SQLWCHAR*)(stlVarInt)aValue,
                                     SQL_MAX_OPTION_STRING_LENGTH /  STL_SIZEOF(SQLWCHAR),
                                     sDbc->mCharacterSet,
                                     sValue,
                                     STL_SIZEOF(sValue),
                                     &sLength,
                                     &sErrorStack ) == STL_SUCCESS );

            sRet = zllSetConnectAttr( aConnectionHandle,
                                      aOption,
                                      (SQLPOINTER)sValue,
                                      sLength,
                                      &sErrorStack );
            break;
        default :
            sRet = zllSetConnectAttr( aConnectionHandle,
                                      aOption,
                                      (SQLPOINTER)(stlVarInt)aValue,
                                      SQL_NTS,
                                      &sErrorStack );
            break;
    }

    STL_TRY( sRet == SQL_SUCCESS );

    sState = 0;
    STL_TRY( stlRestoreRegionMem( &sDbc->mAllocator,
                                  &sMark,
                                  STL_TRUE,
                                  &sErrorStack ) == STL_SUCCESS );

    zldDiagSetRetCode( SQL_HANDLE_DBC, aConnectionHandle, sRet );

    return SQL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_FINISH;

    if( ( sRet == SQL_ERROR ) ||
        ( sRet == SQL_SUCCESS_WITH_INFO ) )
    {
        zldDiagSetRetCode( SQL_HANDLE_DBC, aConnectionHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_DBC,
                               aConnectionHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }
        
        zlcCheckSessionAndCleanup( sDbc );
    }

    switch( sState )
    {
        case 1 :
            (void)stlRestoreRegionMem( &sDbc->mAllocator, &sMark, STL_TRUE, &sErrorStack );
        default :
            break;
    }

    return sRet;
}

static SQLRETURN zllSetCursorName( SQLHSTMT        aStatementHandle,
                                   SQLCHAR       * aCursorName,
                                   SQLSMALLINT     aNameLength,
                                   stlErrorStack * aErrorStack )

{
    SQLRETURN       sRet        = SQL_ERROR;
    zlsStmt       * sStmt       = NULL;
    stlInt16        sNameLength;

    sStmt = (zlsStmt*)aStatementHandle;

    STL_TRY( zldDiagClear( SQL_HANDLE_STMT,
                           aStatementHandle,
                           aErrorStack ) == STL_SUCCESS );

    STL_TRY( zlcCheckSession( sStmt->mParentDbc,
                              aErrorStack ) == STL_SUCCESS );
    
    switch( sStmt->mTransition )
    {
        case ZLS_TRANSITION_STATE_S0 :
            STL_THROW( RAMP_ERR_INVALID_HANDLE );
            break;
        case ZLS_TRANSITION_STATE_S1 :
        case ZLS_TRANSITION_STATE_S2 :
        case ZLS_TRANSITION_STATE_S3 :
            break;
        case ZLS_TRANSITION_STATE_S4 :
        case ZLS_TRANSITION_STATE_S5 :
        case ZLS_TRANSITION_STATE_S6 :
        case ZLS_TRANSITION_STATE_S7 :
            STL_THROW( RAMP_ERR_INVALID_CURSOR_STATE );
            break;
        case ZLS_TRANSITION_STATE_S8 :
        case ZLS_TRANSITION_STATE_S9 :
        case ZLS_TRANSITION_STATE_S10 :
        case ZLS_TRANSITION_STATE_S11 :
        case ZLS_TRANSITION_STATE_S12 :
            STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR );
            break;
    }

    STL_TRY_THROW( aCursorName != NULL, RAMP_ERR_INVALID_USE_OF_NULL_POINTER );

    if( aNameLength == SQL_NTS )
    {
        sNameLength = stlStrlen( (const stlChar*)aCursorName );
    }
    else
    {
        sNameLength = aNameLength;
    }

    STL_TRY_THROW( sNameLength > 0, RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH );
    
    STL_TRY_THROW( sNameLength < STL_MAX_SQL_IDENTIFIER_LENGTH,
                   RAMP_ERR_INVALID_CURSOR_NAME );

    /*
     * cursor 이름은 SQLCUR 또는 SQL_CUR로 시작할 수 없음
     */
    STL_TRY_THROW( stlStrncasecmp( (const stlChar*)aCursorName, "SQLCUR", 6 ) != 0,
                   RAMP_ERR_INVALID_CURSOR_NAME );
                   
    STL_TRY_THROW( stlStrncasecmp( (const stlChar*)aCursorName, "SQL_CUR", 7 ) != 0,
                   RAMP_ERR_INVALID_CURSOR_NAME );
                   

    STL_TRY( zlrSetCursorName( aStatementHandle,
                               (stlChar*)aCursorName,
                               sNameLength,
                               aErrorStack ) == STL_SUCCESS );

    sRet = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_USE_OF_NULL_POINTER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_USE_OF_NULL_POINTER,
                      "The argument CursorName was a null pointer.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_CURSOR_STATE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CURSOR_STATE,
                      "The statement corresponding to StatementHandle was "
                      "already in an executed or cursor-positioned state.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_CURSOR_NAME )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CURSOR_NAME,
                      "The cursor name specified in *CursorName was invalid "
                      "because it exceeded the maximum length as defined by the driver, "
                      "or it started with \"SQLCUR\" or \"SQL_CUR\".",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_STRING_OR_BUFFER_LENGTH,
                      "The argument NameLength was less than 0 but not equal to SQL_NTS.",
                      aErrorStack );
    }

    STL_FINISH;

    return sRet;
}

SQLRETURN SQL_API SQLSetCursorName( SQLHSTMT      aStatementHandle,
                                    SQLCHAR     * aCursorName,
                                    SQLSMALLINT   aNameLength )
{
    SQLRETURN       sRet  = SQL_ERROR;
    zlsStmt       * sStmt = NULL;
    stlErrorStack   sErrorStack;
    stlErrorData  * sErrorData = NULL;
    stlRegionMark   sMark;
    stlInt32        sState = 0;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sStmt = (zlsStmt*)aStatementHandle;

    STL_TRY_THROW( sStmt != NULL,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sStmt->mType == SQL_HANDLE_STMT,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( stlMarkRegionMem( &sStmt->mAllocator,
                               &sMark,
                               &sErrorStack ) == STL_SUCCESS );
    sState = 1;

    sRet = zllSetCursorName( aStatementHandle,
                             aCursorName,
                             aNameLength,
                             &sErrorStack );
    
    STL_TRY( sRet == SQL_SUCCESS );

    sState = 0;
    STL_TRY( stlRestoreRegionMem( &sStmt->mAllocator,
                                  &sMark,
                                  STL_TRUE,
                                  &sErrorStack ) == STL_SUCCESS );

    zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

    return SQL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_FINISH;

    if( ( sRet == SQL_ERROR ) ||
        ( sRet == SQL_SUCCESS_WITH_INFO ) )
    {
        zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_STMT,
                               aStatementHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }

        if( sStmt != NULL )
        {
            zlcCheckSessionAndCleanup( ZLS_STMT_DBC(sStmt) );
        }
    }

    switch( sState )
    {
        case 1 :
            (void)stlRestoreRegionMem( &sStmt->mAllocator, &sMark, STL_TRUE, &sErrorStack );
        default :
            break;
    }

    return sRet;
}

SQLRETURN SQL_API SQLSetCursorNameW( SQLHSTMT      aStatementHandle,
                                     SQLWCHAR    * aCursorName,
                                     SQLSMALLINT   aNameLength )
{
    /**
     * aNameLength
     *     Length of *CursorName, in characters.
     */
    
    SQLRETURN       sRet  = SQL_ERROR;
    zlsStmt       * sStmt = NULL;
    stlErrorStack   sErrorStack;
    stlErrorData  * sErrorData = NULL;
    stlRegionMark   sMark;
    stlInt32        sState = 0;

    stlInt64        sBufferLength;
    SQLCHAR       * sCursorName = NULL;
    stlInt64        sNameLength = 0;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sStmt = (zlsStmt*)aStatementHandle;

    STL_TRY_THROW( sStmt != NULL,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sStmt->mType == SQL_HANDLE_STMT,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( stlMarkRegionMem( &sStmt->mAllocator,
                               &sMark,
                               &sErrorStack ) == STL_SUCCESS );
    sState = 1;

    sBufferLength = 0;    
    STL_TRY( zlvAllocCharBuffer( aCursorName,
                                 aNameLength,
                                 ZLS_STMT_DBC(sStmt)->mCharacterSet,
                                 &sCursorName,
                                 &sBufferLength,
                                 &sStmt->mAllocator,
                                 &sErrorStack ) == STL_SUCCESS );
    
    STL_TRY( zlvWcharToChar( aCursorName,
                             aNameLength,
                             ZLS_STMT_DBC(sStmt)->mCharacterSet,
                             sCursorName,
                             sBufferLength,
                             &sNameLength,
                             &sErrorStack ) == STL_SUCCESS );
    
    sRet = zllSetCursorName( aStatementHandle,
                             sCursorName,
                             sNameLength,
                             &sErrorStack );
    
    STL_TRY( sRet == SQL_SUCCESS );

    sState = 0;
    STL_TRY( stlRestoreRegionMem( &sStmt->mAllocator,
                                  &sMark,
                                  STL_TRUE,
                                  &sErrorStack ) == STL_SUCCESS );

    zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

    return SQL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_FINISH;

    if( ( sRet == SQL_ERROR ) ||
        ( sRet == SQL_SUCCESS_WITH_INFO ) )
    {
        zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_STMT,
                               aStatementHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }

        if( sStmt != NULL )
        {
            zlcCheckSessionAndCleanup( ZLS_STMT_DBC(sStmt) );
        }
    }

    switch( sState )
    {
        case 1 :
            (void)stlRestoreRegionMem( &sStmt->mAllocator, &sMark, STL_TRUE, &sErrorStack );
        default :
            break;
    }

    return sRet;
}

static SQLRETURN zllSetDescField( SQLHDESC        aDescriptorHandle,
                                  SQLSMALLINT     aRecNumber,
                                  SQLSMALLINT     aFieldIdentifier,
                                  SQLPOINTER      aValuePtr,
                                  SQLINTEGER      aBufferLength,
                                  stlErrorStack * aErrorStack )
{
    SQLRETURN       sRet             = SQL_ERROR;
    zlcDbc        * sDbc             = NULL;
    zlsStmt       * sStmt            = NULL;
    zldDesc       * sDesc            = NULL;
    stlInt16        sDescType        = ZLD_DESC_TYPE_UNKNOWN;
    stlBool         sIsHeaderField   = STL_FALSE;
    stlBool         sSuccessWithInfo = STL_FALSE;

    sDesc = (zldDesc*)aDescriptorHandle;
    sStmt = sDesc->mStmt;
    sDbc  = sStmt->mParentDbc;

    STL_TRY( zldDiagClear( SQL_HANDLE_DESC,
                           aDescriptorHandle,
                           aErrorStack ) == STL_SUCCESS );

    STL_TRY( zlcCheckSession( sDbc,
                              aErrorStack ) == STL_SUCCESS );
    
    sDescType = sDesc->mDescType;

    STL_TRY_THROW( sDescType != ZLD_DESC_TYPE_IRD,
                   RAMP_ERR_IMPLEMENTATION_ROW_DESCRIPTOR );

    switch( sDbc->mTransition )
    {
        case ZLC_TRANSITION_STATE_C0 :
        case ZLC_TRANSITION_STATE_C1 :
        case ZLC_TRANSITION_STATE_C2 :
        case ZLC_TRANSITION_STATE_C3 :
        case ZLC_TRANSITION_STATE_C4 :
            STL_THROW( RAMP_ERR_INVALID_HANDLE );
            break;
        case ZLC_TRANSITION_STATE_C5 :
        case ZLC_TRANSITION_STATE_C6 :
            break;
    }

    switch( sStmt->mTransition )
    {
        case ZLS_TRANSITION_STATE_S0 :
            STL_THROW( RAMP_ERR_INVALID_HANDLE );
            break;
        case ZLS_TRANSITION_STATE_S8 :
        case ZLS_TRANSITION_STATE_S9 :
        case ZLS_TRANSITION_STATE_S10 :
        case ZLS_TRANSITION_STATE_S11 :
        case ZLS_TRANSITION_STATE_S12 :
            STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR );
            break;
        default :
            break;
    }

    STL_TRY_THROW( aRecNumber >= 0, RAMP_ERR_INVALID_DESCRIPTOR_INDEX1 );

    switch( aFieldIdentifier )
    {
        case SQL_DESC_ARRAY_SIZE :
        case SQL_DESC_ARRAY_STATUS_PTR :
        case SQL_DESC_BIND_OFFSET_PTR :
        case SQL_DESC_BIND_TYPE :
        case SQL_DESC_COUNT :
        case SQL_DESC_ROWS_PROCESSED_PTR :
            sIsHeaderField = STL_TRUE;
            break;
        case SQL_DESC_CONCISE_TYPE :
        case SQL_DESC_DATA_PTR :
        case SQL_DESC_DATETIME_INTERVAL_CODE :
        case SQL_DESC_DATETIME_INTERVAL_PRECISION :
        case SQL_DESC_INDICATOR_PTR :
        case SQL_DESC_NAME :
        case SQL_DESC_LENGTH :
        case SQL_DESC_NUM_PREC_RADIX :
        case SQL_DESC_OCTET_LENGTH :
        case SQL_DESC_OCTET_LENGTH_PTR :
        case SQL_DESC_PARAMETER_TYPE :
        case SQL_DESC_PRECISION :
        case SQL_DESC_SCALE :
        case SQL_DESC_TYPE :
        case SQL_DESC_UNNAMED :
        case SQL_DESC_CHAR_LENGTH_UNITS :
            sIsHeaderField = STL_FALSE;
            break;
        case SQL_DESC_ALLOC_TYPE :
        case SQL_DESC_AUTO_UNIQUE_VALUE :
        case SQL_DESC_BASE_COLUMN_NAME :
        case SQL_DESC_BASE_TABLE_NAME :
        case SQL_DESC_CASE_SENSITIVE :
        case SQL_DESC_CATALOG_NAME :
        case SQL_DESC_DISPLAY_SIZE :
        case SQL_DESC_FIXED_PREC_SCALE :
        case SQL_DESC_LABEL :
        case SQL_DESC_LITERAL_PREFIX :
        case SQL_DESC_LITERAL_SUFFIX :
        case SQL_DESC_LOCAL_TYPE_NAME :
        case SQL_DESC_NULLABLE :
        case SQL_DESC_ROWVER :
        case SQL_DESC_SCHEMA_NAME :
        case SQL_DESC_SEARCHABLE :
        case SQL_DESC_TABLE_NAME :
        case SQL_DESC_TYPE_NAME :
        case SQL_DESC_UNSIGNED :
        case SQL_DESC_UPDATABLE :
            STL_THROW( RAMP_ERR_INVALID_DESCRIPTOR_FIELD_IDENTIFIER1 );
            break;  
        default :
            STL_THROW( RAMP_ERR_INVALID_DESCRIPTOR_FIELD_IDENTIFIER2 );
            break;  
    }

    switch( sDescType )
    {
        case ZLD_DESC_TYPE_ARD :
        case ZLD_DESC_TYPE_APD :
            switch( aFieldIdentifier )
            {
                case SQL_DESC_ARRAY_SIZE :
                case SQL_DESC_ARRAY_STATUS_PTR :
                case SQL_DESC_BIND_OFFSET_PTR :
                case SQL_DESC_BIND_TYPE :
                case SQL_DESC_COUNT :
                case SQL_DESC_OCTET_LENGTH_PTR :
                case SQL_DESC_INDICATOR_PTR :
                case SQL_DESC_DATA_PTR :
                case SQL_DESC_UNNAMED :
                case SQL_DESC_CONCISE_TYPE :
                case SQL_DESC_DATETIME_INTERVAL_PRECISION :
                case SQL_DESC_NUM_PREC_RADIX :
                case SQL_DESC_TYPE :
                case SQL_DESC_LENGTH :
                case SQL_DESC_PRECISION :
                case SQL_DESC_SCALE :
                case SQL_DESC_DATETIME_INTERVAL_CODE :
                case SQL_DESC_OCTET_LENGTH :
                    break;
                default :
                    STL_THROW( RAMP_ERR_INVALID_DESCRIPTOR_FIELD_IDENTIFIER3 );
                    break;
            }
            break;
        case ZLD_DESC_TYPE_IPD :
            STL_TRY_THROW( ( sIsHeaderField != STL_FALSE ) || ( aRecNumber != 0 ),
                           RAMP_ERR_INVALID_DESCRIPTOR_INDEX2 );

            switch( aFieldIdentifier )
            {
                case SQL_DESC_ARRAY_STATUS_PTR :
                case SQL_DESC_ROWS_PROCESSED_PTR :
                case SQL_DESC_COUNT :
                case SQL_DESC_DATA_PTR :
                case SQL_DESC_PARAMETER_TYPE :
                case SQL_DESC_NAME :
                case SQL_DESC_UNNAMED :
                case SQL_DESC_CONCISE_TYPE :
                case SQL_DESC_DATETIME_INTERVAL_PRECISION :
                case SQL_DESC_NUM_PREC_RADIX :
                case SQL_DESC_TYPE :
                case SQL_DESC_LENGTH :
                case SQL_DESC_PRECISION :
                case SQL_DESC_SCALE :
                case SQL_DESC_DATETIME_INTERVAL_CODE :
                case SQL_DESC_OCTET_LENGTH :
                case SQL_DESC_CHAR_LENGTH_UNITS :
                    break;
                default :
                    STL_THROW( RAMP_ERR_INVALID_DESCRIPTOR_FIELD_IDENTIFIER3 );
                    break;
            }
            break;
        default :
            STL_THROW( RAMP_ERR_IMPLEMENTATION_ROW_DESCRIPTOR );
            break;
    }

    switch( aFieldIdentifier )
    {
        case SQL_DESC_NAME :
            if( aBufferLength != SQL_NTS )
            {
                STL_TRY_THROW( aBufferLength >= 0, RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH );
            }
            break;
        default :
            break;
    }
    
    if( (aFieldIdentifier == SQL_DESC_ARRAY_SIZE) &&
        (sDesc->mDescType == ZLD_DESC_TYPE_APD ) )
    {
        STL_TRY( zlsSetAttr( sStmt,
                             SQL_ATTR_PARAMSET_SIZE,
                             aValuePtr,
                             aBufferLength,
                             &sSuccessWithInfo,
                             aErrorStack ) == STL_SUCCESS );
    }
    else
    {
        STL_TRY( zldDescSetField( sDesc,
                                  aRecNumber,
                                  aFieldIdentifier,
                                  aValuePtr,
                                  aBufferLength,
                                  aErrorStack ) == STL_SUCCESS );
    }

    sRet = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }
    
    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_DESCRIPTOR_INDEX1 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_DESCRIPTOR_INDEX,
                      "The RecNumber argument was less than 0.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_DESCRIPTOR_INDEX2 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_DESCRIPTOR_INDEX,
                      "The FieldIdentifier argument was a record field, "
                      "the RecNumber argument was 0, and "
                      "the DescriptorHandle argument referred to an IPD handle.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_DESCRIPTOR_FIELD_IDENTIFIER1 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_DESCRIPTOR_FIELD_IDENTIFIER,
                      "The FieldIdentifier argument was a read-only, ODBC-defined field.",
                      aErrorStack );
    }
                
    STL_CATCH( RAMP_ERR_INVALID_DESCRIPTOR_FIELD_IDENTIFIER2 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_DESCRIPTOR_FIELD_IDENTIFIER,
                      "The value specified for the FieldIdentifier argument was not "
                      "an ODBC-defined field and was not an implementation-defined value. ",
                      aErrorStack );
    }
                
    STL_CATCH( RAMP_ERR_INVALID_DESCRIPTOR_FIELD_IDENTIFIER3 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_DESCRIPTOR_FIELD_IDENTIFIER,
                      "The FieldIdentifier argument was invalid for the DescriptorHandle "
                      "argument.",
                      aErrorStack );
    }
                
    STL_CATCH( RAMP_ERR_IMPLEMENTATION_ROW_DESCRIPTOR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_CANNOT_MODIFY_AN_IMPLEMENTATION_ROW_DESCRIPTROR,
                      "The DescriptorHandle argument was associated with an IRD.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_STRING_OR_BUFFER_LENGTH,
                      "*ValuePtr is a character string, and BufferLength was less than zero but was not equal to "
                      "SQL_NTS.",
                      aErrorStack );
    }

    STL_FINISH;

    return sRet;
}

SQLRETURN SQL_API SQLSetDescField( SQLHDESC    aDescriptorHandle,
                                   SQLSMALLINT aRecNumber,
                                   SQLSMALLINT aFieldIdentifier,
                                   SQLPOINTER  aValuePtr,
                                   SQLINTEGER  aBufferLength )
{
    SQLRETURN       sRet = SQL_ERROR;
    zldDesc       * sDesc = NULL;
    zlsStmt       * sStmt = NULL;
    stlErrorStack   sErrorStack;
    stlErrorData  * sErrorData = NULL;
    stlRegionMark   sMark;
    stlInt32        sState = 0;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sDesc = (zldDesc*)aDescriptorHandle;

    STL_TRY_THROW( sDesc != NULL, RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sDesc->mType == SQL_HANDLE_DESC,
                   RAMP_ERR_INVALID_HANDLE );

    sStmt = sDesc->mStmt;

    STL_TRY( stlMarkRegionMem( &sStmt->mAllocator,
                               &sMark,
                               &sErrorStack ) == STL_SUCCESS );
    sState = 1;

    sRet = zllSetDescField( aDescriptorHandle,
                            aRecNumber,
                            aFieldIdentifier,
                            aValuePtr,
                            aBufferLength,
                            &sErrorStack );

    STL_TRY( sRet == SQL_SUCCESS );

    sState = 0;
    STL_TRY( stlRestoreRegionMem( &sStmt->mAllocator,
                                  &sMark,
                                  STL_TRUE,
                                  &sErrorStack ) == STL_SUCCESS );

    zldDiagSetRetCode( SQL_HANDLE_DESC, aDescriptorHandle, sRet );

    return sRet;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }
    
    STL_FINISH;

    if( ( sRet == SQL_ERROR ) ||
        ( sRet == SQL_SUCCESS_WITH_INFO ) )
    {
        zldDiagSetRetCode( SQL_HANDLE_DESC, aDescriptorHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_DESC,
                               aDescriptorHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }
        
        if( sStmt != NULL )
        {
            zlcCheckSessionAndCleanup( ZLS_STMT_DBC(sStmt) );
        }
    }

    switch( sState )
    {
        case 1 :
            (void)stlRestoreRegionMem( &sStmt->mAllocator, &sMark, STL_TRUE, &sErrorStack );
        default :
            break;
    }

    return sRet;
}

SQLRETURN SQL_API SQLSetDescFieldW( SQLHDESC    aDescriptorHandle,
                                    SQLSMALLINT aRecNumber,
                                    SQLSMALLINT aFieldIdentifier,
                                    SQLPOINTER  aValuePtr,
                                    SQLINTEGER  aBufferLength )
{
    /**
     * aBufferLength
     *     If FieldIdentifier is an ODBC-defined field and ValuePtr points to
     *     a character string or a binary buffer, this argument should be
     *     the length of *ValuePtr. For character string data, this argument
     *     should contain the number of bytes in the string.
     */

    SQLRETURN       sRet = SQL_ERROR;
    zldDesc       * sDesc = NULL;
    zlsStmt       * sStmt = NULL;
    stlErrorStack   sErrorStack;
    stlErrorData  * sErrorData = NULL;
    stlRegionMark   sMark;
    stlInt32        sState = 0;

    SQLCHAR         sValue[STL_MAX_SQL_IDENTIFIER_LENGTH + 1];
    stlInt64        sLength = 0;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sDesc = (zldDesc*)aDescriptorHandle;

    STL_TRY_THROW( sDesc != NULL, RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sDesc->mType == SQL_HANDLE_DESC,
                   RAMP_ERR_INVALID_HANDLE );

    sStmt = sDesc->mStmt;

    STL_TRY( stlMarkRegionMem( &sStmt->mAllocator,
                               &sMark,
                               &sErrorStack ) == STL_SUCCESS );
    sState = 1;

    switch( aFieldIdentifier )
    {
        case SQL_DESC_NAME :
            if( aBufferLength == SQL_NTS )
            {
                aBufferLength = zlvStrlenW((SQLWCHAR*)aValuePtr) * STL_SIZEOF(SQLWCHAR);
            }
            else
            {
                STL_TRY_THROW( aBufferLength >= 0, RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH );
            }

            STL_TRY( zlvWcharToChar( (SQLWCHAR*)aValuePtr,
                                     aBufferLength /  STL_SIZEOF(SQLWCHAR),
                                     ZLS_STMT_DBC(sStmt)->mCharacterSet,
                                     sValue,
                                     STL_SIZEOF(sValue),
                                     &sLength,
                                     &sErrorStack ) == STL_SUCCESS );
            
            sRet = zllSetDescField( aDescriptorHandle,
                                    aRecNumber,
                                    aFieldIdentifier,
                                    sValue,
                                    sLength,
                                    &sErrorStack );
            break;
        default :
            sRet = zllSetDescField( aDescriptorHandle,
                                    aRecNumber,
                                    aFieldIdentifier,
                                    aValuePtr,
                                    aBufferLength,
                                    &sErrorStack );
            break;
    }

    STL_TRY( sRet == SQL_SUCCESS );

    sState = 0;
    STL_TRY( stlRestoreRegionMem( &sStmt->mAllocator,
                                  &sMark,
                                  STL_TRUE,
                                  &sErrorStack ) == STL_SUCCESS );

    zldDiagSetRetCode( SQL_HANDLE_DESC, aDescriptorHandle, sRet );

    return sRet;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }
    
    STL_CATCH( RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_STRING_OR_BUFFER_LENGTH,
                      "*ValuePtr is a character string, and BufferLength was less than zero but was not equal to "
                      "SQL_NTS.",
                      &sErrorStack );
    }

    STL_FINISH;

    if( ( sRet == SQL_ERROR ) ||
        ( sRet == SQL_SUCCESS_WITH_INFO ) )
    {
        zldDiagSetRetCode( SQL_HANDLE_DESC, aDescriptorHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_DESC,
                               aDescriptorHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }
        
        if( sStmt != NULL )
        {
            zlcCheckSessionAndCleanup( ZLS_STMT_DBC(sStmt) );
        }
    }

    switch( sState )
    {
        case 1 :
            (void)stlRestoreRegionMem( &sStmt->mAllocator, &sMark, STL_TRUE, &sErrorStack );
        default :
            break;
    }

    return sRet;
}

SQLRETURN SQL_API SQLSetDescRec( SQLHDESC      aDescriptorHandle,
                                 SQLSMALLINT   aRecNumber,
                                 SQLSMALLINT   aType,
                                 SQLSMALLINT   aSubType,
                                 SQLLEN        aLength,
                                 SQLSMALLINT   aPrecision,
                                 SQLSMALLINT   aScale,
                                 SQLPOINTER    aDataPtr,
                                 SQLLEN      * aStringLengthPtr,
                                 SQLLEN      * aIndicatorPtr )
{
    SQLRETURN       sRet        = SQL_ERROR;
    zlcDbc        * sDbc        = NULL;
    zlsStmt       * sStmt       = NULL;
    zldDesc       * sDesc       = NULL;
    stlErrorData  * sErrorData  = NULL;
    stlErrorStack   sErrorStack;
    stlInt16        sDescType   = ZLD_DESC_TYPE_UNKNOWN;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sDesc = (zldDesc*)aDescriptorHandle;

    STL_TRY_THROW( sDesc != NULL, RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sDesc->mType == SQL_HANDLE_DESC,
                   RAMP_ERR_INVALID_HANDLE );

    sStmt = sDesc->mStmt;
    sDbc = sStmt->mParentDbc;

    STL_TRY( zldDiagClear( SQL_HANDLE_DESC,
                           aDescriptorHandle,
                           &sErrorStack ) == STL_SUCCESS );
    
    STL_TRY( zlcCheckSession( sDbc,
                              &sErrorStack ) == STL_SUCCESS );

    sDescType = sDesc->mDescType;

    STL_TRY_THROW( sDescType != ZLD_DESC_TYPE_IRD,
                   RAMP_ERR_IMPLEMENTATION_ROW_DESCRIPTOR );

    switch( sDbc->mTransition )
    {
        case ZLC_TRANSITION_STATE_C0 :
        case ZLC_TRANSITION_STATE_C1 :
        case ZLC_TRANSITION_STATE_C2 :
        case ZLC_TRANSITION_STATE_C3 :
        case ZLC_TRANSITION_STATE_C4 :
            STL_THROW( RAMP_ERR_INVALID_HANDLE );
            break;
        case ZLC_TRANSITION_STATE_C5 :
        case ZLC_TRANSITION_STATE_C6 :
            break;
    }

    switch( sStmt->mTransition )
    {
        case ZLS_TRANSITION_STATE_S0 :
            STL_THROW( RAMP_ERR_INVALID_HANDLE );
            break;
        case ZLS_TRANSITION_STATE_S8 :
        case ZLS_TRANSITION_STATE_S9 :
        case ZLS_TRANSITION_STATE_S10 :
        case ZLS_TRANSITION_STATE_S11 :
        case ZLS_TRANSITION_STATE_S12 :
            STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR );
            break;
        default :
            break;
    }

    STL_TRY_THROW( aRecNumber >= 0, RAMP_ERR_INVALID_DESCRIPTOR_INDEX );

    STL_TRY( zldDescSetRec( sDesc,
                            aRecNumber,
                            aType,
                            aSubType,
                            aLength,
                            aPrecision,
                            aScale,
                            aDataPtr,
                            aStringLengthPtr,
                            aIndicatorPtr,
                            &sErrorStack ) == STL_SUCCESS );

    sRet = SQL_SUCCESS;
    
    zldDiagSetRetCode( SQL_HANDLE_DESC, aDescriptorHandle, sRet );

    return sRet;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }
    
    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      NULL,
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_DESCRIPTOR_INDEX )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_DESCRIPTOR_INDEX,
                      "The RecNumber argument was less than 0.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_IMPLEMENTATION_ROW_DESCRIPTOR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_CANNOT_MODIFY_AN_IMPLEMENTATION_ROW_DESCRIPTROR,
                      "The DescriptorHandle argument was associated with an IRD.",
                      &sErrorStack );
    }

    STL_FINISH;

    if( sRet == SQL_ERROR )
    {
        zldDiagSetRetCode( SQL_HANDLE_DESC, aDescriptorHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_DESC,
                               aDescriptorHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }
        
        if( sStmt != NULL )
        {
            zlcCheckSessionAndCleanup( ZLS_STMT_DBC(sStmt) );
        }
    }

    return sRet;
}

SQLRETURN SQL_API SQLSetEnvAttr( SQLHENV    aEnvironmentHandle,
                                 SQLINTEGER aAttribute,
                                 SQLPOINTER aValuePtr,
                                 SQLINTEGER aStringLength )
{
    SQLRETURN       sRet        = SQL_ERROR;
    zlnEnv        * sEnv        = NULL;
    stlErrorData  * sErrorData  = NULL;
    stlErrorStack   sErrorStack;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sEnv = (zlnEnv*)aEnvironmentHandle;

    STL_TRY_THROW( sEnv != NULL, RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sEnv->mType == SQL_HANDLE_ENV,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( zldDiagClear( SQL_HANDLE_ENV,
                           aEnvironmentHandle,
                           &sErrorStack ) == STL_SUCCESS );

    switch( aAttribute )
    {
        case SQL_ATTR_CONNECTION_POOLING :
        case SQL_ATTR_CP_MATCH :
        case SQL_ATTR_OUTPUT_NTS :
            STL_THROW( RAMP_ERR_OPTIONAL_FEATURE_NOT_IMPLEMENTED );
            break;
        case SQL_ATTR_ODBC_VERSION :
            break;
        default:
            STL_THROW( RAMP_ERR_INVALID_ATTRIBUTE_OPTION_IDENTIFIER );
            break;
    }

    STL_TRY( zlnSetAttr( sEnv,
                         aAttribute,
                         aValuePtr,
                         aStringLength,
                         &sErrorStack ) == STL_SUCCESS );

    sRet = SQL_SUCCESS;
    
    zldDiagSetRetCode( SQL_HANDLE_ENV, aEnvironmentHandle, sRet );
    
    return sRet;

    STL_CATCH( RAMP_ERR_OPTIONAL_FEATURE_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_OPTIONAL_FEATURE_NOT_IMPLEMENTED,
                      "The value specified for the argument Attribute was "
                      "a valid ODBC environment attribute for the version of ODBC "
                      "supported by the driver, but was not supported by the driver.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_ATTRIBUTE_OPTION_IDENTIFIER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_ATTRIBUTE_OPTION_IDENTIFIER,
                      "The value specified for the argument Attribute was not valid "
                      "for the version of ODBC supported by the driver.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_FINISH;

    if( sRet == SQL_ERROR )
    {
        zldDiagSetRetCode( SQL_HANDLE_ENV, aEnvironmentHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );
        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_ENV,
                               aEnvironmentHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }
    }

    return sRet;
}

SQLRETURN SQL_API SQLSetParam( SQLHSTMT      aStatementHandle,
                               SQLUSMALLINT  aParameterNumber,
                               SQLSMALLINT   aValueType,
                               SQLSMALLINT   aParameterType,
                               SQLULEN       aLengthPrecision,
                               SQLSMALLINT   aParameterScale,
                               SQLPOINTER    aParameterValuePtr,
                               SQLLEN      * aStrLen_or_IndPtr )
{
    SQLRETURN       sRet        = SQL_ERROR;
    zlsStmt       * sStmt       = NULL;
    stlErrorData  * sErrorData  = NULL;
    stlErrorStack   sErrorStack;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sStmt = (zlsStmt*)aStatementHandle;

    STL_TRY_THROW( sStmt != NULL, RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sStmt->mType == SQL_HANDLE_STMT,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( zldDiagClear( SQL_HANDLE_STMT,
                           aStatementHandle,
                           &sErrorStack ) == STL_SUCCESS );

    STL_TRY( zlcCheckSession( sStmt->mParentDbc,
                              &sErrorStack ) == STL_SUCCESS );
    
    switch( sStmt->mTransition )
    {
        case ZLS_TRANSITION_STATE_S0 :
            STL_THROW( RAMP_ERR_INVALID_HANDLE );
            break;
        case ZLS_TRANSITION_STATE_S8 :
        case ZLS_TRANSITION_STATE_S9 :
        case ZLS_TRANSITION_STATE_S10 :
        case ZLS_TRANSITION_STATE_S11 :
        case ZLS_TRANSITION_STATE_S12 :
            STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR );
            break;
        default :
            break;
    }

    STL_TRY_THROW( aParameterNumber > 0,
                   RAMP_ERR_INVALID_DESCRIPTOR_INDEX );

    switch( aValueType )
    {
        case SQL_C_CHAR :
        case SQL_C_STINYINT :
        case SQL_C_UTINYINT :
        case SQL_C_TINYINT :            
        case SQL_C_SSHORT :
        case SQL_C_USHORT :
        case SQL_C_SHORT :
        case SQL_C_SLONG :
        case SQL_C_ULONG :
        case SQL_C_LONG :
        case SQL_C_SBIGINT :
        case SQL_C_UBIGINT :
        case SQL_C_FLOAT :
        case SQL_C_DOUBLE :
        case SQL_C_BIT :
        case SQL_C_BINARY :
        case SQL_C_TYPE_DATE :
        case SQL_C_TYPE_TIME :
        case SQL_C_TYPE_TIMESTAMP :
        case SQL_C_INTERVAL_YEAR :
        case SQL_C_INTERVAL_MONTH :
        case SQL_C_INTERVAL_DAY :
        case SQL_C_INTERVAL_HOUR :
        case SQL_C_INTERVAL_MINUTE :
        case SQL_C_INTERVAL_SECOND :
        case SQL_C_INTERVAL_YEAR_TO_MONTH :
        case SQL_C_INTERVAL_DAY_TO_HOUR :
        case SQL_C_INTERVAL_DAY_TO_MINUTE :
        case SQL_C_INTERVAL_DAY_TO_SECOND :
        case SQL_C_INTERVAL_HOUR_TO_MINUTE :
        case SQL_C_INTERVAL_HOUR_TO_SECOND :
        case SQL_C_INTERVAL_MINUTE_TO_SECOND :
        case SQL_C_NUMERIC :
        case SQL_C_DEFAULT:
        case SQL_C_WCHAR :
            /*
             * Driver SQL Type
             */
        case SQL_C_BOOLEAN :
        case SQL_C_LONGVARCHAR :
        case SQL_C_LONGVARBINARY :
        case SQL_C_TYPE_TIME_WITH_TIMEZONE :
        case SQL_C_TYPE_TIMESTAMP_WITH_TIMEZONE :
            break;
        case SQL_C_GUID :
            STL_THROW( RAMP_ERR_OPTIONAL_FEATURE_NOT_IMPLEMENTED1 );
            break;
        default :
            STL_THROW( RAMP_ERR_INVALID_APPLICATION_BUFFER_TYPE );
            break;
    }

    switch( aParameterType )
    {
        case SQL_CHAR :
        case SQL_VARCHAR :
        case SQL_LONGVARCHAR :
        case SQL_NUMERIC :
        case SQL_SMALLINT :
        case SQL_INTEGER :
        case SQL_REAL :
        case SQL_DOUBLE :
        case SQL_FLOAT :
        case SQL_BIT :
        case SQL_TINYINT :
        case SQL_BIGINT :
        case SQL_BINARY :
        case SQL_VARBINARY :
        case SQL_LONGVARBINARY :
        case SQL_TYPE_DATE :
        case SQL_TYPE_TIME :
        case SQL_TYPE_TIMESTAMP :
        case SQL_INTERVAL_MONTH :
        case SQL_INTERVAL_YEAR :
        case SQL_INTERVAL_YEAR_TO_MONTH :
        case SQL_INTERVAL_DAY :
        case SQL_INTERVAL_HOUR :
        case SQL_INTERVAL_MINUTE :
        case SQL_INTERVAL_SECOND :
        case SQL_INTERVAL_DAY_TO_HOUR :
        case SQL_INTERVAL_DAY_TO_MINUTE :
        case SQL_INTERVAL_DAY_TO_SECOND :
        case SQL_INTERVAL_HOUR_TO_MINUTE :
        case SQL_INTERVAL_HOUR_TO_SECOND :
        case SQL_INTERVAL_MINUTE_TO_SECOND :
            /*
             * Driver SQL Type
             */
        case SQL_BOOLEAN :
        case SQL_TYPE_TIME_WITH_TIMEZONE :
        case SQL_TYPE_TIMESTAMP_WITH_TIMEZONE :
        case SQL_ROWID :            
            break;
        case SQL_WCHAR :
        case SQL_WVARCHAR :
        case SQL_WLONGVARCHAR :
        case SQL_DECIMAL :
            STL_THROW( RAMP_ERR_OPTIONAL_FEATURE_NOT_IMPLEMENTED2 );
            break;
        default :
            STL_THROW( RAMP_ERR_INVALID_SQL_DATA_TYPE );
            break;
    }

    STL_TRY_THROW( ( aParameterValuePtr != NULL ) ||
                   ( aStrLen_or_IndPtr  != NULL ),
                   RAMP_ERR_INVALID_ARGUMENT_VALUE );

    STL_TRY( zliBindParameter( sStmt,
                               aParameterNumber,
                               SQL_PARAM_INPUT_OUTPUT,
                               aValueType,
                               aParameterType,
                               aLengthPrecision,
                               aParameterScale,
                               aParameterValuePtr,
                               SQL_SETPARAM_VALUE_MAX,
                               aStrLen_or_IndPtr,
                               &sErrorStack ) == STL_SUCCESS );

    sRet = SQL_SUCCESS;
    
    zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

    return sRet;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      NULL,
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_DESCRIPTOR_INDEX )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_DESCRIPTOR_INDEX,
                      "The value specified for the argument ParameterNumber was "
                      "less than 1.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_APPLICATION_BUFFER_TYPE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_APPLICATION_BUFFER_TYPE,
                      "The value specified by the argument ValueType was not "
                      "a valid C data type or SQL_C_DEFAULT.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_SQL_DATA_TYPE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_SQL_DATA_TYPE,
                      "The value specified for the argument ParameterType was "
                      "neither a valid ODBC SQL data type identifier nor "
                      "a driver-specific SQL data type identifier supported by the driver.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_ARGUMENT_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_ARGUMENT_VALUE,
                      "The argument ParameterValuePtr was a null pointer, "
                      "the argument StrLen_or_IndPtr was a null pointer, "
                      "and the argument InputOutputType was not SQL_PARAM_OUTPUT.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_OPTIONAL_FEATURE_NOT_IMPLEMENTED1 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_OPTIONAL_FEATURE_NOT_IMPLEMENTED,
                      "The driver or data source does not support the conversion specified "
                      "by the combination of the value specified for the argument ValueType "
                      "and the driver-specific value specified for the argument ParameterType.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_OPTIONAL_FEATURE_NOT_IMPLEMENTED2 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_OPTIONAL_FEATURE_NOT_IMPLEMENTED,
                      "The value specified for the argument ParameterType was "
                      "a valid ODBC SQL data type identifier for the version of "
                      "ODBC supported by the driver but was not supported by the driver or data source.",
                      &sErrorStack );
    }

    STL_FINISH;

    if( sRet == SQL_ERROR )
    {
        zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_STMT,
                               aStatementHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }
    }

    return sRet;
}

SQLRETURN SQL_API SQLSetPos( SQLHSTMT      aStatementHandle,
                             SQLSETPOSIROW aRowNumber,
                             SQLUSMALLINT  aOperation,
                             SQLUSMALLINT  aLockType )
{
    SQLRETURN       sRet             = SQL_ERROR;
    zlsStmt       * sStmt            = NULL;
    zldDesc       * sArd             = NULL;
    stlUInt64       sArraySize;
    stlErrorData  * sErrorData       = NULL;
    stlErrorStack   sErrorStack;
    zlrResult     * sResult          = NULL;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sStmt = (zlsStmt*)aStatementHandle;

    STL_TRY_THROW( sStmt != NULL, RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sStmt->mType == SQL_HANDLE_STMT,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( zldDiagClear( SQL_HANDLE_STMT,
                           aStatementHandle,
                           &sErrorStack ) == STL_SUCCESS );

    STL_TRY( zlcCheckSession( sStmt->mParentDbc,
                              &sErrorStack ) == STL_SUCCESS );
    
    switch( sStmt->mTransition )
    {
        case ZLS_TRANSITION_STATE_S0 :
            STL_THROW( RAMP_ERR_INVALID_HANDLE );
            break;
        case ZLS_TRANSITION_STATE_S1 :
        case ZLS_TRANSITION_STATE_S2 :
        case ZLS_TRANSITION_STATE_S3 :
            STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR1 );
            break;
        case ZLS_TRANSITION_STATE_S4 :
            STL_THROW( RAMP_ERR_INVALID_CURSOR_STATE1 );
            break;
        case ZLS_TRANSITION_STATE_S5 :
            STL_THROW( RAMP_ERR_INVALID_CURSOR_STATE2 );
            break;
        case ZLS_TRANSITION_STATE_S8 :
        case ZLS_TRANSITION_STATE_S9 :
        case ZLS_TRANSITION_STATE_S10 :
        case ZLS_TRANSITION_STATE_S11 :
        case ZLS_TRANSITION_STATE_S12 :
            STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR2 );
            break;
        default :
            break;
    }

    STL_TRY_THROW( (sStmt->mCursorConcurrency != SQL_CONCUR_READ_ONLY) ||
                   (sStmt->mCursorScrollable != SQL_NONSCROLLABLE ),
                   RAMP_ERR_INVALID_CURSOR_POSITION1 );
    
    switch( aOperation )
    {
        case SQL_POSITION :
            STL_TRY_THROW( aRowNumber != 0, RAMP_ERR_INVALID_CURSOR_POSITION2 );
            break;
        case SQL_REFRESH :
            STL_THROW( RAMP_ERR_OPTIONAL_FEATURE_NOT_IMPLEMENTED );
            break;
        case SQL_UPDATE :
        case SQL_DELETE :
            STL_TRY_THROW( sStmt->mCursorConcurrency != SQL_CONCUR_READ_ONLY,
                           RAMP_ERR_INVALID_ATTRIBUTE_IDENTIFIER3 );
            STL_THROW( RAMP_ERR_OPTIONAL_FEATURE_NOT_IMPLEMENTED );
            break;
        default :
            STL_THROW( RAMP_ERR_INVALID_ATTRIBUTE_IDENTIFIER1 );
            break;
    }

    switch( aLockType )
    {
        case SQL_LOCK_NO_CHANGE :
        case SQL_LOCK_EXCLUSIVE :
        case SQL_LOCK_UNLOCK :
            break;
        default :
            STL_THROW( RAMP_ERR_INVALID_ATTRIBUTE_IDENTIFIER2 );
            break;
    }

    sArd       = sStmt->mArd;
    sArraySize = sArd->mArraySize;

    STL_TRY_THROW( aRowNumber <= sArraySize, RAMP_ERR_ROW_VALUE_OUT_OF_RANGE );

    sResult = &sStmt->mResult;
    
    STL_TRY_THROW( sResult->mBlockCurrentRow != ZLR_ROWSET_BEFORE_START,
                   RAMP_ERR_INVALID_CURSOR_STATE3 );
    STL_TRY_THROW( sResult->mBlockCurrentRow != ZLR_ROWSET_AFTER_END,
                   RAMP_ERR_INVALID_CURSOR_STATE3 );

    STL_TRY( zlrSetPos( aStatementHandle,
                        (stlInt64)aRowNumber,
                        (stlInt16)aOperation,
                        aLockType,
                        &sErrorStack ) == STL_SUCCESS );
    
    sRet = SQL_SUCCESS;

    zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

    return sRet;
    
    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR1 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      "The specified StatementHandle was not in an executed state. "
                      "The function was called without first calling "
                      "SQLExecDirect, SQLExecute or a catalog function.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR2 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      NULL,
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_CURSOR_STATE1 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CURSOR_STATE,
                      "The StatementHandle was in an executed state, "
                      "but no result set was associated with the StatementHandle.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_CURSOR_STATE2 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CURSOR_STATE,
                      "A cursor was open on the StatementHandle, "
                      "but SQLFetch or SQLFetchScroll had not been called.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_CURSOR_STATE3 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CURSOR_STATE,
                      "A cursor was open on the StatementHandle, "
                      "and SQLFetch or SQLFetchScroll had been called, "
                      "but the cursor was positioned before the start of the result set or "
                      "after the end of the result set.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_CURSOR_POSITION1 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CURSOR_POSITION,
                      "The cursor associated with the StatementHandle was defined as forward-only, "
                      "so the cursor could not be positioned within the rowset.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_CURSOR_POSITION2 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CURSOR_POSITION,
                      "The RowNumber argument was 0, and "
                      "the Operation argument was SQL_POSITION.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_ATTRIBUTE_IDENTIFIER1 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_ATTRIBUTE_OPTION_IDENTIFIER,
                      "The value specified for the Operation argument was invalid.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_ATTRIBUTE_IDENTIFIER2 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_ATTRIBUTE_OPTION_IDENTIFIER,
                      "The value specified for the LockType argument was invalid.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_ATTRIBUTE_IDENTIFIER3 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_ATTRIBUTE_OPTION_IDENTIFIER,
                      "The Operation argument was SQL_UPDATE or SQL_DELETE, and "
                      "the SQL_ATTR_CONCURRENCY statement attribute was "
                      "SQL_ATTR_CONCUR_READ_ONLY.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_ROW_VALUE_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_ROW_VALUE_OUT_OF_RANGE,
                      "The value specified for the argument RowNumber was "
                      "greater than the number of rows in the rowset.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_OPTIONAL_FEATURE_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_OPTIONAL_FEATURE_NOT_IMPLEMENTED,
                      "The driver or data source does not support the operation requested "
                      "in the Operation argument or the LockType argument.",
                      &sErrorStack );
    }

    STL_FINISH;

    if( ( sRet == SQL_ERROR ) ||
        ( sRet == SQL_SUCCESS_WITH_INFO ) )
    {
        zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_STMT,
                               aStatementHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }
        
        if( sStmt != NULL )
        {
            zlcCheckSessionAndCleanup( ZLS_STMT_DBC(sStmt) );
        }
    }

    return sRet;
}

SQLRETURN SQL_API SQLSetScrollOptions( SQLHSTMT     hstmt,
                                       SQLUSMALLINT fConcurrency,
                                       SQLLEN       crowKeyset,
                                       SQLUSMALLINT crowRowset )
{
    return SQL_ERROR;
}

static SQLRETURN zllSetStmtAttr( SQLHSTMT        aStatementHandle,
                                 SQLINTEGER      aAttribute,
                                 SQLPOINTER      aValuePtr,
                                 SQLINTEGER      aStringLength,
                                 stlErrorStack * aErrorStack )
{
    SQLRETURN   sRet        = SQL_ERROR;
    zlsStmt   * sStmt       = NULL;
    stlBool     sSuccessWithInfo = STL_FALSE;

    sStmt = (zlsStmt*)aStatementHandle;

    STL_TRY( zldDiagClear( SQL_HANDLE_STMT,
                           aStatementHandle,
                           aErrorStack ) == STL_SUCCESS );

    STL_TRY( zlcCheckSession( sStmt->mParentDbc,
                              aErrorStack ) == STL_SUCCESS );
    
    switch( sStmt->mTransition )
    {
        case ZLS_TRANSITION_STATE_S0 :
            STL_THROW( RAMP_ERR_INVALID_HANDLE );
            break;
        case ZLS_TRANSITION_STATE_S1 :
            break;
        case ZLS_TRANSITION_STATE_S2 :
        case ZLS_TRANSITION_STATE_S3 :
            switch( aAttribute )
            {
                case SQL_ATTR_CONCURRENCY :
                case SQL_ATTR_CURSOR_TYPE :
                case SQL_ATTR_SIMULATE_CURSOR :
                case SQL_ATTR_USE_BOOKMARKS :
                case SQL_ATTR_CURSOR_SCROLLABLE :
                case SQL_ATTR_CURSOR_SENSITIVITY :
                    STL_THROW( RAMP_ERR_ATTRIBUTE_CANNOT_BE_SET_NOW );
                    break;
                default :
                    break;
            }
            break;
        case ZLS_TRANSITION_STATE_S4 :
        case ZLS_TRANSITION_STATE_S5 :
        case ZLS_TRANSITION_STATE_S6 :
        case ZLS_TRANSITION_STATE_S7 :
            switch( aAttribute )
            {
                case SQL_ATTR_CONCURRENCY :
                case SQL_ATTR_CURSOR_TYPE :
                case SQL_ATTR_SIMULATE_CURSOR :
                case SQL_ATTR_USE_BOOKMARKS :
                case SQL_ATTR_CURSOR_SCROLLABLE :
                case SQL_ATTR_CURSOR_SENSITIVITY :
                    STL_THROW( RAMP_ERR_INVALID_CURSOR_STATE );
                    break;
                default :
                    break;
            }
            break;
        case ZLS_TRANSITION_STATE_S8 :
        case ZLS_TRANSITION_STATE_S9 :
        case ZLS_TRANSITION_STATE_S10 :
        case ZLS_TRANSITION_STATE_S11 :
        case ZLS_TRANSITION_STATE_S12 :
            STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR );
            break;
    }

    switch( aAttribute )
    {
        case SQL_ATTR_CONCURRENCY :
        case SQL_ATTR_CURSOR_HOLDABLE :
        case SQL_ATTR_CURSOR_SCROLLABLE :
        case SQL_ATTR_CURSOR_SENSITIVITY :
        case SQL_ATTR_CURSOR_TYPE :
        case SQL_ATTR_MAX_ROWS :
        case SQL_ATTR_METADATA_ID :
        case SQL_ATTR_PARAM_BIND_OFFSET_PTR :
        case SQL_ATTR_PARAM_BIND_TYPE :
        case SQL_ATTR_PARAM_OPERATION_PTR :
        case SQL_ATTR_PARAM_STATUS_PTR :
        case SQL_ATTR_PARAMS_PROCESSED_PTR  :
        case SQL_ATTR_PARAMSET_SIZE :
        case SQL_ATTR_QUERY_TIMEOUT :
        case SQL_ATTR_RETRIEVE_DATA :
        case SQL_ATTR_ROW_ARRAY_SIZE :
        case SQL_ATTR_ROW_BIND_OFFSET_PTR :
        case SQL_ATTR_ROW_BIND_TYPE :
        case SQL_ATTR_ROW_STATUS_PTR :
        case SQL_ATTR_ROWS_FETCHED_PTR :
        case SQL_ATTR_EXPLAIN_PLAN_OPTION :
        case SQL_ATTR_EXPLAIN_PLAN_TEXT :
        case SQL_ATTR_ATOMIC_EXECUTION :
        case SQL_ATTR_PREFETCH_ROWS :
        case SQL_ROWSET_SIZE :
            break;
        case SQL_ATTR_APP_PARAM_DESC :
        case SQL_ATTR_APP_ROW_DESC :
        case SQL_ATTR_ASYNC_ENABLE :
        case SQL_ATTR_ENABLE_AUTO_IPD  :
        case SQL_ATTR_FETCH_BOOKMARK_PTR :
        case SQL_ATTR_IMP_PARAM_DESC :
        case SQL_ATTR_IMP_ROW_DESC :
        case SQL_ATTR_KEYSET_SIZE :
        case SQL_ATTR_MAX_LENGTH :
        case SQL_ATTR_NOSCAN :
        case SQL_ATTR_ROW_NUMBER :
        case SQL_ATTR_ROW_OPERATION_PTR :
        case SQL_ATTR_SIMULATE_CURSOR :
        case SQL_ATTR_USE_BOOKMARKS :
            STL_THROW( RAMP_ERR_OPTIONAL_FEATURE_NOT_IMPLEMENTED );
            break;
        default :
            STL_THROW( RAMP_ERR_INVALID_ATTRIBUTE_OPTION_IDENTIFIER );
            break;
    }

    STL_TRY( zlsSetAttr( sStmt,
                         aAttribute,
                         aValuePtr,
                         aStringLength,
                         &sSuccessWithInfo,
                         aErrorStack ) == STL_SUCCESS );

    STL_TRY_THROW( sSuccessWithInfo == STL_FALSE, RAMP_SUCCESS_WITH_INFO );

    sRet = SQL_SUCCESS;

    zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

    return sRet;

    STL_CATCH( RAMP_ERR_ATTRIBUTE_CANNOT_BE_SET_NOW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_ATTRIBUTE_CANNOT_BE_SET_NOW,
                      "The Attribute was SQL_ATTR_CONCURRENCY, SQL_ ATTR_CURSOR_TYPE, "
                      "SQL_ ATTR_SIMULATE_CURSOR, SQL_ ATTR_USE_BOOKMARKS, "
                      "SQL_ATTR_CURSOR_SCROLLABLE, or SQL_ATTR_CURSOR_SENSITIVITY, and "
                      "the statement was prepared.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_CURSOR_STATE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CURSOR_STATE,
                      "The Attribute was SQL_ATTR_CONCURRENCY, SQL_ATTR_CURSOR_TYPE, "
                      "SQL_ ATTR_SIMULATE_CURSOR, SQL_ ATTR_USE_BOOKMARKS, "
                      "SQL_ATTR_CURSOR_SCROLLABLE, or SQL_ATTR_CURSOR_SENSITIVITY, and "
                      "the cursor was open.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_ATTRIBUTE_OPTION_IDENTIFIER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_ATTRIBUTE_OPTION_IDENTIFIER,
                      "The value specified for the argument Attribute was not "
                      "valid for the version of ODBC supported by the driver.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_OPTIONAL_FEATURE_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_OPTIONAL_FEATURE_NOT_IMPLEMENTED,
                      "The value specified for the argument Attribute was a valid "
                      "ODBC connection or statement attribute for the version of "
                      "ODBC supported by the driver but was not supported by the driver.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_CATCH( RAMP_SUCCESS_WITH_INFO )
    {
        sRet = SQL_SUCCESS_WITH_INFO;
    }

    STL_FINISH;

    return sRet;
}

SQLRETURN SQL_API SQLSetStmtAttr( SQLHSTMT   aStatementHandle,
                                  SQLINTEGER aAttribute,
                                  SQLPOINTER aValuePtr,
                                  SQLINTEGER aStringLength )
{
    SQLRETURN       sRet  = SQL_ERROR;
    zlsStmt       * sStmt = NULL;
    stlErrorStack   sErrorStack;
    stlErrorData  * sErrorData = NULL;
    stlRegionMark   sMark;
    stlInt32        sState = 0;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sStmt = (zlsStmt*)aStatementHandle;

    STL_TRY_THROW( sStmt != NULL,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sStmt->mType == SQL_HANDLE_STMT,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( stlMarkRegionMem( &sStmt->mAllocator,
                               &sMark,
                               &sErrorStack ) == STL_SUCCESS );
    sState = 1;

    sRet = zllSetStmtAttr( aStatementHandle,
                           aAttribute,
                           aValuePtr,
                           aStringLength,
                           &sErrorStack );

    STL_TRY( sRet == SQL_SUCCESS );

    sState = 0;
    STL_TRY( stlRestoreRegionMem( &sStmt->mAllocator,
                                  &sMark,
                                  STL_TRUE,
                                  &sErrorStack ) == STL_SUCCESS );

    zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

    return SQL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_FINISH;

    if( ( sRet == SQL_ERROR ) ||
        ( sRet == SQL_SUCCESS_WITH_INFO ) )
    {
        zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_STMT,
                               aStatementHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }

        if( sStmt != NULL )
        {
            zlcCheckSessionAndCleanup( ZLS_STMT_DBC(sStmt) );
        }
    }

    switch( sState )
    {
        case 1 :
            stlRestoreRegionMem( &sStmt->mAllocator, &sMark, STL_TRUE, &sErrorStack );
        default :
            break;
    }

    return sRet;
}

SQLRETURN SQL_API SQLSetStmtAttrW( SQLHSTMT   aStatementHandle,
                                   SQLINTEGER aAttribute,
                                   SQLPOINTER aValuePtr,
                                   SQLINTEGER aStringLength )
{
    /**
     * aStringLength
     *     If Attribute is an ODBC-defined attribute and ValuePtr points to
     *     a character string or a binary buffer, this argument should be
     *     the length of *ValuePtr. For character string data, this argument
     *     should contain the number of bytes in the string.
     */

    SQLRETURN       sRet  = SQL_ERROR;
    zlsStmt       * sStmt = NULL;
    stlErrorStack   sErrorStack;
    stlErrorData  * sErrorData = NULL;
    stlRegionMark   sMark;
    stlInt32        sState = 0;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sStmt = (zlsStmt*)aStatementHandle;

    STL_TRY_THROW( sStmt != NULL,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sStmt->mType == SQL_HANDLE_STMT,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( stlMarkRegionMem( &sStmt->mAllocator,
                               &sMark,
                               &sErrorStack ) == STL_SUCCESS );
    sState = 1;

    sRet = zllSetStmtAttr( aStatementHandle,
                           aAttribute,
                           aValuePtr,
                           aStringLength,
                           &sErrorStack );

    STL_TRY( sRet == SQL_SUCCESS );

    sState = 0;
    STL_TRY( stlRestoreRegionMem( &sStmt->mAllocator,
                                  &sMark,
                                  STL_TRUE,
                                  &sErrorStack ) == STL_SUCCESS );

    zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

    return SQL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_FINISH;

    if( ( sRet == SQL_ERROR ) ||
        ( sRet == SQL_SUCCESS_WITH_INFO ) )
    {
        zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_STMT,
                               aStatementHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }

        if( sStmt != NULL )
        {
            zlcCheckSessionAndCleanup( ZLS_STMT_DBC(sStmt) );
        }
    }

    switch( sState )
    {
        case 1 :
            stlRestoreRegionMem( &sStmt->mAllocator, &sMark, STL_TRUE, &sErrorStack );
        default :
            break;
    }

    return sRet;
}

SQLRETURN SQL_API SQLSetStmtOption( SQLHSTMT     aStatementHandle,
                                    SQLUSMALLINT aOption,
                                    SQLULEN      aValue )
{
    SQLRETURN       sRet        = SQL_ERROR;
    zlsStmt       * sStmt       = NULL;
    stlInt32        sAttribute;
    stlErrorData  * sErrorData  = NULL;
    stlErrorStack   sErrorStack;
    stlBool         sSuccessWithInfo = STL_FALSE;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sStmt = (zlsStmt*)aStatementHandle;

    STL_TRY_THROW( sStmt != NULL, RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sStmt->mType == SQL_HANDLE_STMT,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( zldDiagClear( SQL_HANDLE_STMT,
                           aStatementHandle,
                           &sErrorStack ) == STL_SUCCESS );

    STL_TRY( zlcCheckSession( sStmt->mParentDbc,
                              &sErrorStack ) == STL_SUCCESS );

    sAttribute = aOption;
    
    switch( sStmt->mTransition )
    {
        case ZLS_TRANSITION_STATE_S0 :
            STL_THROW( RAMP_ERR_INVALID_HANDLE );
            break;
        case ZLS_TRANSITION_STATE_S1 :
            break;
        case ZLS_TRANSITION_STATE_S2 :
        case ZLS_TRANSITION_STATE_S3 :
            switch( sAttribute )
            {
                case SQL_ATTR_CONCURRENCY :
                case SQL_ATTR_CURSOR_TYPE :
                case SQL_ATTR_SIMULATE_CURSOR :
                case SQL_ATTR_USE_BOOKMARKS :
                case SQL_ATTR_CURSOR_SCROLLABLE :
                case SQL_ATTR_CURSOR_SENSITIVITY :
                    STL_THROW( RAMP_ERR_ATTRIBUTE_CANNOT_BE_SET_NOW );
                    break;
                default :
                    break;
            }
            break;
        case ZLS_TRANSITION_STATE_S4 :
        case ZLS_TRANSITION_STATE_S5 :
        case ZLS_TRANSITION_STATE_S6 :
        case ZLS_TRANSITION_STATE_S7 :
            switch( sAttribute )
            {
                case SQL_ATTR_CONCURRENCY :
                case SQL_ATTR_CURSOR_TYPE :
                case SQL_ATTR_SIMULATE_CURSOR :
                case SQL_ATTR_USE_BOOKMARKS :
                case SQL_ATTR_CURSOR_SCROLLABLE :
                case SQL_ATTR_CURSOR_SENSITIVITY :
                    STL_THROW( RAMP_ERR_INVALID_CURSOR_STATE );
                    break;
                default :
                    break;
            }
            break;
        case ZLS_TRANSITION_STATE_S8 :
        case ZLS_TRANSITION_STATE_S9 :
        case ZLS_TRANSITION_STATE_S10 :
        case ZLS_TRANSITION_STATE_S11 :
        case ZLS_TRANSITION_STATE_S12 :
            STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR );
            break;
    }

    switch( sAttribute )
    {
        case SQL_ATTR_CONCURRENCY :
        case SQL_ATTR_CURSOR_HOLDABLE :
        case SQL_ATTR_CURSOR_SCROLLABLE :
        case SQL_ATTR_CURSOR_SENSITIVITY :
        case SQL_ATTR_CURSOR_TYPE :
        case SQL_ATTR_MAX_ROWS :
        case SQL_ATTR_METADATA_ID :
        case SQL_ATTR_PARAM_BIND_OFFSET_PTR :
        case SQL_ATTR_PARAM_BIND_TYPE :
        case SQL_ATTR_PARAM_OPERATION_PTR :
        case SQL_ATTR_PARAM_STATUS_PTR :
        case SQL_ATTR_PARAMS_PROCESSED_PTR  :
        case SQL_ATTR_PARAMSET_SIZE :
        case SQL_ATTR_QUERY_TIMEOUT :
        case SQL_ATTR_ROW_ARRAY_SIZE :
        case SQL_ATTR_ROW_BIND_OFFSET_PTR :
        case SQL_ATTR_ROW_BIND_TYPE :
        case SQL_ATTR_ROW_STATUS_PTR :
        case SQL_ATTR_ROWS_FETCHED_PTR :
        case SQL_ATTR_EXPLAIN_PLAN_OPTION :
        case SQL_ATTR_EXPLAIN_PLAN_TEXT :
        case SQL_ATTR_ATOMIC_EXECUTION :
        case SQL_ATTR_PREFETCH_ROWS :
        case SQL_ROWSET_SIZE :
            break;
        case SQL_ATTR_APP_PARAM_DESC :
        case SQL_ATTR_APP_ROW_DESC :
        case SQL_ATTR_ASYNC_ENABLE :
        case SQL_ATTR_ENABLE_AUTO_IPD  :
        case SQL_ATTR_FETCH_BOOKMARK_PTR :
        case SQL_ATTR_IMP_PARAM_DESC :
        case SQL_ATTR_IMP_ROW_DESC :
        case SQL_ATTR_KEYSET_SIZE :
        case SQL_ATTR_MAX_LENGTH :
        case SQL_ATTR_NOSCAN :
        case SQL_ATTR_RETRIEVE_DATA :
        case SQL_ATTR_ROW_NUMBER :
        case SQL_ATTR_ROW_OPERATION_PTR :
        case SQL_ATTR_SIMULATE_CURSOR :
        case SQL_ATTR_USE_BOOKMARKS :
            STL_THROW( RAMP_ERR_OPTIONAL_FEATURE_NOT_IMPLEMENTED );
            break;
        default :
            STL_THROW( RAMP_ERR_INVALID_ATTRIBUTE_OPTION_IDENTIFIER );
            break;
    }

    STL_TRY( zlsSetAttr( sStmt,
                         sAttribute,
                         (void*)(stlVarInt)aValue,
                         SQL_NTS,
                         &sSuccessWithInfo,
                         &sErrorStack ) == STL_SUCCESS );

    sRet = SQL_SUCCESS;

    zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

    return sRet;

    STL_CATCH( RAMP_ERR_ATTRIBUTE_CANNOT_BE_SET_NOW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_ATTRIBUTE_CANNOT_BE_SET_NOW,
                      "The Attribute was SQL_ATTR_CONCURRENCY, SQL_ ATTR_CURSOR_TYPE, "
                      "SQL_ ATTR_SIMULATE_CURSOR, SQL_ ATTR_USE_BOOKMARKS, "
                      "SQL_ATTR_CURSOR_SCROLLABLE, or SQL_ATTR_CURSOR_SENSITIVITY, and "
                      "the statement was prepared.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_CURSOR_STATE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CURSOR_STATE,
                      "The Attribute was SQL_ATTR_CONCURRENCY, SQL_ATTR_CURSOR_TYPE, "
                      "SQL_ ATTR_SIMULATE_CURSOR, SQL_ ATTR_USE_BOOKMARKS, "
                      "SQL_ATTR_CURSOR_SCROLLABLE, or SQL_ATTR_CURSOR_SENSITIVITY, and "
                      "the cursor was open.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      NULL,
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_ATTRIBUTE_OPTION_IDENTIFIER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_ATTRIBUTE_OPTION_IDENTIFIER,
                      "The value specified for the argument Attribute was not "
                      "valid for the version of ODBC supported by the driver.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_OPTIONAL_FEATURE_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_OPTIONAL_FEATURE_NOT_IMPLEMENTED,
                      "The value specified for the argument Attribute was a valid "
                      "ODBC connection or statement attribute for the version of "
                      "ODBC supported by the driver but was not supported by the driver.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_FINISH;

    if( sRet == SQL_ERROR )
    {
        zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_STMT,
                               aStatementHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }
        
        if( sStmt != NULL )
        {
            zlcCheckSessionAndCleanup( ZLS_STMT_DBC(sStmt) );
        }
    }

    return sRet;
}

static SQLRETURN zllSpecialColumns( SQLHSTMT         aStatementHandle,
                                    SQLUSMALLINT     aIdentifierType,
                                    SQLCHAR        * aCatalogName,
                                    SQLSMALLINT      aNameLength1,
                                    SQLCHAR        * aSchemaName,
                                    SQLSMALLINT      aNameLength2,
                                    SQLCHAR        * aTableName,
                                    SQLSMALLINT      aNameLength3,
                                    SQLUSMALLINT     aScope,
                                    SQLUSMALLINT     aNullable,
                                    stlErrorStack  * aErrorStack )
{
    SQLRETURN       sRet              = SQL_ERROR;
    zlcDbc        * sDbc              = NULL;
    zlsStmt       * sStmt             = NULL;
    stlInt64        sAffectedRowCount = -1;
    stlBool         sSuccessWithInfo  = STL_FALSE;

    stlChar         sBuffer[2048];
    stlInt32        sBufferLength = STL_SIZEOF( sBuffer );

    stlInt16        sNameLength1 = 0;
    stlInt16        sNameLength2 = 0;
    stlInt16        sNameLength3 = 0;

    stlInt32        sPos = 0;
    stlBool         sEmptyResult = STL_FALSE;
    
    SQLCHAR * sSELECT   = (SQLCHAR*)"SELECT "
        "  CAST(2                   AS NATIVE_SMALLINT)     AS SCOPE, "
        "  CAST('ROWID'             AS VARCHAR(128 OCTETS)) AS COLUMN_NAME, "
        "  CAST(INFO.ODBC_DATA_TYPE AS NATIVE_SMALLINT)     AS DATA_TYPE, "
        "  CAST(INFO.TYPE_NAME      AS VARCHAR(128 OCTETS)) AS TYPE_NAME, "
        "  CAST(NULL                AS NATIVE_INTEGER)      AS COLUMN_SIZE, "
        "  CAST(INFO.COLUMN_SIZE    AS NATIVE_INTEGER)      AS BUFFER_SIZE, "
        "  CAST(NULL                AS NATIVE_SMALLINT)     AS DECIMAL_DIGITS, "
        "  CAST(2                   AS NATIVE_SMALLINT)     AS PSEUDO_COLUMN "
        "FROM INFORMATION_SCHEMA.TABLES   AS TAB, "
        "     DEFINITION_SCHEMA.TYPE_INFO AS INFO ";
    SQLCHAR * sWHERE    = (SQLCHAR*)"WHERE TAB.TABLE_TYPE = 'BASE TABLE' AND "
        "      INFO.TYPE_NAME = 'ROWID' ";
    SQLCHAR * sORDER_BY = (SQLCHAR*)"ORDER BY SCOPE ";

    sStmt = (zlsStmt*)aStatementHandle;
    sDbc  = sStmt->mParentDbc;

    STL_TRY( zldDiagClear( SQL_HANDLE_STMT,
                           aStatementHandle,
                           aErrorStack ) == STL_SUCCESS );

    STL_TRY( zlcCheckSession( sDbc,
                              aErrorStack ) == STL_SUCCESS );
    
    switch( sDbc->mTransition )
    {
        case ZLC_TRANSITION_STATE_C0 :
        case ZLC_TRANSITION_STATE_C1 :
        case ZLC_TRANSITION_STATE_C2 :
        case ZLC_TRANSITION_STATE_C3 :
        case ZLC_TRANSITION_STATE_C4 :
            STL_THROW( RAMP_ERR_INVALID_HANDLE );
            break;
        case ZLC_TRANSITION_STATE_C5 :
        case ZLC_TRANSITION_STATE_C6 :
            break;
    }

    switch( sStmt->mTransition )
    {
        case ZLS_TRANSITION_STATE_S1 :
        case ZLS_TRANSITION_STATE_S2 :
        case ZLS_TRANSITION_STATE_S3 :
        case ZLS_TRANSITION_STATE_S4 :
            break;
        case ZLS_TRANSITION_STATE_S5 :
            STL_THROW( RAMP_ERR_INVALID_CURSOR_STATE_1 );
            break;
        case ZLS_TRANSITION_STATE_S6 :
        case ZLS_TRANSITION_STATE_S7 :
            STL_THROW( RAMP_ERR_INVALID_CURSOR_STATE_2 );
            break;
        case ZLS_TRANSITION_STATE_S8 :
        case ZLS_TRANSITION_STATE_S9 :
        case ZLS_TRANSITION_STATE_S10 :
        case ZLS_TRANSITION_STATE_S11 :
        case ZLS_TRANSITION_STATE_S12 :
            STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR );
            break;
    }

    if( aNameLength1 != SQL_NTS )
    {
        STL_TRY_THROW( aNameLength1 >= 0, RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH );

        sNameLength1 = aNameLength1;
    }
    else
    {
        if (aCatalogName != NULL)
        {
            sNameLength1 = stlStrlen((stlChar*)aCatalogName);
        }
    }

    if( aNameLength2 != SQL_NTS )
    {
        STL_TRY_THROW( aNameLength2 >= 0, RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH );

        sNameLength2 = aNameLength2;
    }
    else
    {
        if (aSchemaName != NULL)
        {
            sNameLength2 = stlStrlen((stlChar*)aSchemaName);
        }
    }

    if( aNameLength3 != SQL_NTS )
    {
        STL_TRY_THROW( aNameLength3 >= 0, RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH );

        sNameLength3 = aNameLength3;
    }
    else
    {
        if (aTableName != NULL)
        {
            sNameLength3 = stlStrlen((stlChar*)aTableName);
        }
    }

    switch( aIdentifierType )
    {
        case SQL_BEST_ROWID :
            break;
        case SQL_ROWVER :
            sEmptyResult = STL_TRUE;
            break;
        default :
            STL_THROW( RAMP_ERR_COLUMN_TYPE_OUT_OF_RANGE );
            break;
    }

    switch( aScope )
    {
        case SQL_SCOPE_CURROW :
        case SQL_SCOPE_TRANSACTION :
            break;
        case SQL_SCOPE_SESSION :
            sEmptyResult = STL_TRUE;
            break;
        default :
            STL_THROW( RAMP_ERR_SCOPE_TYPE_OUT_OF_RANGE );
            break;
    }

    STL_TRY_THROW( (aNullable == SQL_NO_NULLS ) ||
                   (aNullable == SQL_NULLABLE),
                   RAMP_ERR_NULLABLE_TYPE_OUT_OF_RANGE );
    switch( aScope )
    {
        case SQL_SCOPE_CURROW :
        case SQL_SCOPE_TRANSACTION :
            break;
        case SQL_SCOPE_SESSION :
            sEmptyResult = STL_TRUE;
            break;
        default :
            STL_THROW( RAMP_ERR_SCOPE_TYPE_OUT_OF_RANGE );
            break;
    }

    STL_TRY_THROW( aTableName != NULL, RAMP_ERR_INVALID_USE_OF_NULL_POINTER_1 );
    
    if( sStmt->mMetadataId == STL_TRUE )
    {
        if( aCatalogName != NULL )
        {
            if( (aCatalogName[0] == '\"') &&
                (aCatalogName[sNameLength1-1] == '\"') )
            {
                aCatalogName[sNameLength1-1] = 0;
                aCatalogName++;

                sNameLength1 = sNameLength1 - 2;
            }
            else
            {
                stlToupperString( (stlChar*)aCatalogName, (stlChar*)aCatalogName );
            }
        }
        
        STL_TRY_THROW( aSchemaName != NULL, RAMP_ERR_INVALID_USE_OF_NULL_POINTER_2 );

        if( (aSchemaName[0] == '\"') &&
            (aSchemaName[sNameLength2-1] == '\"') )
        {
            aSchemaName[sNameLength2-1] = 0;
            aSchemaName++;

            sNameLength2 = sNameLength2 - 2;
        }
        else
        {
            stlToupperString( (stlChar*)aSchemaName, (stlChar*)aSchemaName );
        }

        if( (aTableName[0] == '\"') &&
            (aTableName[sNameLength3-1] == '\"') )
        {
            aTableName[sNameLength3-1] = 0;
            aTableName++;

            sNameLength3 = sNameLength3 - 2;
        }
        else
        {
            stlToupperString( (stlChar*)aTableName, (stlChar*)aTableName );
        }
    }

    sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos,
                         "%s%s",
                         sSELECT,
                         sWHERE );

    if( (aCatalogName != NULL) && (sNameLength1 > 0) )
    {
        sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos,
                             " AND TAB.TABLE_CATALOG = '" );
            
        stlMemcpy( sBuffer + sPos, aCatalogName, sNameLength1 );
        sPos += sNameLength1;

        sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos, "' " );
    }

    if( (aSchemaName != NULL) && (sNameLength2 > 0) )
    {
        sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos,
                             " AND TAB.TABLE_SCHEMA = '" );
            
        stlMemcpy( sBuffer + sPos, aSchemaName, sNameLength2 );
        sPos += sNameLength2;

        sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos, "' " );
    }

    sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos,
                         " AND TAB.TABLE_NAME = '" );
            
    stlMemcpy( sBuffer + sPos, aTableName, sNameLength3 );
    sPos += sNameLength3;

    sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos, "' " );

    if( sEmptyResult == STL_TRUE )
    {
        sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos,
                             " AND 1 = 0 " );
    }

    sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos,
                         "%s",
                         sORDER_BY );

    STL_TRY( zlxExecDirect( sStmt,
                            sBuffer,
                            &sAffectedRowCount,
                            &sSuccessWithInfo,
                            aErrorStack ) == STL_SUCCESS );

    STL_TRY_THROW( sSuccessWithInfo == STL_FALSE, RAMP_SUCCESS_WITH_INFO );

    sRet = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_STRING_OR_BUFFER_LENGTH,
                      "The value of one of the length arguments was less than 0 "
                      "but not equal to SQL_NTS.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_CURSOR_STATE_1 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CURSOR_STATE,
                      "A cursor was open on the StatementHandle, "
                      "but SQLFetch or SQLFetchScroll had not been called.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_CURSOR_STATE_2 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CURSOR_STATE,
                      "A cursor was open on the StatementHandle, "
                      "and SQLFetch or SQLFetchScroll had been called.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_USE_OF_NULL_POINTER_1 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_USE_OF_NULL_POINTER,
                      "The TableName argument was a null pointer.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_USE_OF_NULL_POINTER_2 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_USE_OF_NULL_POINTER,
                      "The SQL_ATTR_METADATA_ID statement attribute was set to SQL_TRUE, "
                      "and the SchemaName argument was a null pointer.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_COLUMN_TYPE_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_COLUMN_TYPE_OUT_OF_RANGE,
                      "An invalid IdentifierType value was specified.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_SCOPE_TYPE_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_SCOPE_TYPE_OUT_OF_RANGE,
                      "An invalid Scope value was specified.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_NULLABLE_TYPE_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_NULLABLE_TYPE_OUT_OF_RANGE,
                      "An invalid Nullable value was specified.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_CATCH( RAMP_SUCCESS_WITH_INFO )
    {
        sRet = SQL_SUCCESS_WITH_INFO;
    }

    STL_FINISH;

    return sRet;
}

SQLRETURN SQL_API SQLSpecialColumns( SQLHSTMT       aStatementHandle,
                                     SQLUSMALLINT   aIdentifierType,
                                     SQLCHAR      * aCatalogName,
                                     SQLSMALLINT    aNameLength1,
                                     SQLCHAR      * aSchemaName,
                                     SQLSMALLINT    aNameLength2,
                                     SQLCHAR      * aTableName,
                                     SQLSMALLINT    aNameLength3,
                                     SQLUSMALLINT   aScope,
                                     SQLUSMALLINT   aNullable )
{
    SQLRETURN       sRet  = SQL_ERROR;
    zlsStmt       * sStmt = NULL;
    stlErrorStack   sErrorStack;
    stlErrorData  * sErrorData = NULL;
    stlRegionMark   sMark;
    stlInt32        sState = 0;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sStmt = (zlsStmt*)aStatementHandle;

    STL_TRY_THROW( sStmt != NULL,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sStmt->mType == SQL_HANDLE_STMT,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( stlMarkRegionMem( &sStmt->mAllocator,
                               &sMark,
                               &sErrorStack ) == STL_SUCCESS );
    sState = 1;

    sRet = zllSpecialColumns( aStatementHandle,
                              aIdentifierType,
                              aCatalogName,
                              aNameLength1,
                              aSchemaName,
                              aNameLength2,
                              aTableName,
                              aNameLength3,
                              aScope,
                              aNullable,
                              &sErrorStack );

    STL_TRY( sRet == SQL_SUCCESS );

    sState = 0;
    STL_TRY( stlRestoreRegionMem( &sStmt->mAllocator,
                                  &sMark,
                                  STL_TRUE,
                                  &sErrorStack ) == STL_SUCCESS );

    zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

    return SQL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_FINISH;

    if( ( sRet == SQL_ERROR ) ||
        ( sRet == SQL_SUCCESS_WITH_INFO ) )
    {
        zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_STMT,
                               aStatementHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }

        if( sStmt != NULL )
        {
            zlcCheckSessionAndCleanup( ZLS_STMT_DBC(sStmt) );
        }
    }

    switch( sState )
    {
        case 1 :
            stlRestoreRegionMem( &sStmt->mAllocator, &sMark, STL_TRUE, &sErrorStack );
        default :
            break;
    }

    return sRet;
}

SQLRETURN SQL_API SQLSpecialColumnsW( SQLHSTMT       aStatementHandle,
                                      SQLUSMALLINT   aIdentifierType,
                                      SQLWCHAR     * aCatalogName,
                                      SQLSMALLINT    aNameLength1,
                                      SQLWCHAR     * aSchemaName,
                                      SQLSMALLINT    aNameLength2,
                                      SQLWCHAR     * aTableName,
                                      SQLSMALLINT    aNameLength3,
                                      SQLUSMALLINT   aScope,
                                      SQLUSMALLINT   aNullable )
{
    /**
     * aNameLength1
     *     Length of *CatalogName, in characters.
     * aNameLength2
     *     Length of *SchemaName, in characters.
     * aNameLength3
     *     Length of *TableName, in characters.
     */

    SQLRETURN       sRet  = SQL_ERROR;
    zlsStmt       * sStmt = NULL;
    stlErrorStack   sErrorStack;
    stlErrorData  * sErrorData = NULL;
    stlRegionMark   sMark;
    stlInt32        sState = 0;

    stlInt64        sBufferLength;
    SQLCHAR       * sCatalogName = NULL;
    stlInt64        sNameLength1 = 0;
    SQLCHAR       * sSchemaName = NULL;
    stlInt64        sNameLength2 = 0;
    SQLCHAR       * sTableName = NULL;
    stlInt64        sNameLength3 = 0;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sStmt = (zlsStmt*)aStatementHandle;

    STL_TRY_THROW( sStmt != NULL,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sStmt->mType == SQL_HANDLE_STMT,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( stlMarkRegionMem( &sStmt->mAllocator,
                               &sMark,
                               &sErrorStack ) == STL_SUCCESS );
    sState = 1;

    sBufferLength = 0;    
    STL_TRY( zlvAllocCharBuffer( aCatalogName,
                                 aNameLength1,
                                 ZLS_STMT_DBC(sStmt)->mCharacterSet,
                                 &sCatalogName,
                                 &sBufferLength,
                                 &sStmt->mAllocator,
                                 &sErrorStack ) == STL_SUCCESS );
    
    STL_TRY( zlvWcharToChar( aCatalogName,
                             aNameLength1,
                             ZLS_STMT_DBC(sStmt)->mCharacterSet,
                             sCatalogName,
                             sBufferLength,
                             &sNameLength1,
                             &sErrorStack ) == STL_SUCCESS );
    
    sBufferLength = 0;    
    STL_TRY( zlvAllocCharBuffer( aSchemaName,
                                 aNameLength2,
                                 ZLS_STMT_DBC(sStmt)->mCharacterSet,
                                 &sSchemaName,
                                 &sBufferLength,
                                 &sStmt->mAllocator,
                                 &sErrorStack ) == STL_SUCCESS );
    
    STL_TRY( zlvWcharToChar( aSchemaName,
                             aNameLength2,
                             ZLS_STMT_DBC(sStmt)->mCharacterSet,
                             sSchemaName,
                             sBufferLength,
                             &sNameLength2,
                             &sErrorStack ) == STL_SUCCESS );
    
    sBufferLength = 0;    
    STL_TRY( zlvAllocCharBuffer( aTableName,
                                 aNameLength3,
                                 ZLS_STMT_DBC(sStmt)->mCharacterSet,
                                 &sTableName,
                                 &sBufferLength,
                                 &sStmt->mAllocator,
                                 &sErrorStack ) == STL_SUCCESS );
    
    STL_TRY( zlvWcharToChar( aTableName,
                             aNameLength3,
                             ZLS_STMT_DBC(sStmt)->mCharacterSet,
                             sTableName,
                             sBufferLength,
                             &sNameLength3,
                             &sErrorStack ) == STL_SUCCESS );
    
    sRet = zllSpecialColumns( aStatementHandle,
                              aIdentifierType,
                              sCatalogName,
                              sNameLength1,
                              sSchemaName,
                              sNameLength2,
                              sTableName,
                              sNameLength3,
                              aScope,
                              aNullable,
                              &sErrorStack );

    STL_TRY( sRet == SQL_SUCCESS );

    sState = 0;
    STL_TRY( stlRestoreRegionMem( &sStmt->mAllocator,
                                  &sMark,
                                  STL_TRUE,
                                  &sErrorStack ) == STL_SUCCESS );

    zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

    return SQL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_FINISH;

    if( ( sRet == SQL_ERROR ) ||
        ( sRet == SQL_SUCCESS_WITH_INFO ) )
    {
        zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_STMT,
                               aStatementHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }

        if( sStmt != NULL )
        {
            zlcCheckSessionAndCleanup( ZLS_STMT_DBC(sStmt) );
        }
    }

    switch( sState )
    {
        case 1 :
            (void)stlRestoreRegionMem( &sStmt->mAllocator, &sMark, STL_TRUE, &sErrorStack );
        default :
            break;
    }

    return sRet;
}

static SQLRETURN zllStatistics( SQLHSTMT        aStatementHandle,
                                SQLCHAR       * aCatalogName,
                                SQLSMALLINT     aNameLength1,
                                SQLCHAR       * aSchemaName,
                                SQLSMALLINT     aNameLength2,
                                SQLCHAR       * aTableName,
                                SQLSMALLINT     aNameLength3,
                                SQLUSMALLINT    aUnique,
                                SQLUSMALLINT    aReserved,
                                stlErrorStack * aErrorStack )
{
    SQLRETURN       sRet              = SQL_ERROR;
    zlcDbc        * sDbc              = NULL;
    zlsStmt       * sStmt             = NULL;
    stlInt64        sAffectedRowCount = -1;
    stlBool         sSuccessWithInfo  = STL_FALSE;
    
    stlChar         sBuffer[2048];
    stlInt32        sBufferLength = STL_SIZEOF( sBuffer );

    stlInt16        sNameLength1 = 0;
    stlInt16        sNameLength2 = 0;
    stlInt16        sNameLength3 = 0;

    stlInt32        sPos = 0;
    
    SQLCHAR * sSELECT   = (SQLCHAR*)"SELECT "
        "  CAST(TABLE_CATALOG    AS VARCHAR(128 OCTETS)) AS TABLE_CAT, "
        "  CAST(TABLE_SCHEMA     AS VARCHAR(128 OCTETS)) AS TABLE_SCHEM, "
        "  CAST(TABLE_NAME       AS VARCHAR(128 OCTETS)) AS TABLE_NAME, "
        "  CAST(CASE NON_UNIQUE "
        "         WHEN TRUE  THEN 1 "
        "         WHEN FALSE THEN 0 "
        "         ELSE            NULL "
        "         END            AS NATIVE_SMALLINT)     AS NON_UNIQUE, "
        "  CAST(INDEX_SCHEMA     AS VARCHAR(128 OCTETS)) AS INDEX_QUALIFIER, "
        "  CAST(INDEX_NAME       AS VARCHAR(128 OCTETS)) AS INDEX_NAME, "
        "  CAST(CASE STAT_TYPE "
        "         WHEN 'TABLE STAT'      THEN 0 "
        "         WHEN 'INDEX CLUSTERED' THEN 1 "
        "         WHEN 'INDEX HASHED'    THEN 2 "
        "         WHEN 'INDEX OTHER'     THEN 3 "
        "         ELSE                   NULL "
        "         END            AS NATIVE_SMALLINT)     AS TYPE, "
        "  CAST(ORDINAL_POSITION AS NATIVE_SMALLINT)     AS ORDINAL_POSITION, "
        "  CAST(COLUMN_NAME      AS VARCHAR(128 OCTETS)) AS COLUMN_NAME, "
        "  CAST(CASE IS_ASCENDING_ORDER "
        "         WHEN TRUE  THEN 'A' "
        "         WHEN FALSE THEN 'D' "
        "         ELSE            NULL "
        "         END            AS CHAR(1 OCTETS))      AS ASC_OR_DESC, "
        "  CAST(CARDINALITY      AS NATIVE_INTEGER)      AS CARDINALITY, "
        "  CAST(PAGES            AS NATIVE_INTEGER)      AS PAGES, "
        "  FILTER_CONDITION                              AS FILTER_CONDITION "
        "FROM INFORMATION_SCHEMA.STATISTICS ";
    SQLCHAR * sWHERE    = (SQLCHAR*)"WHERE 1 = 1 ";
    SQLCHAR * sORDER_BY = (SQLCHAR*)"ORDER BY NON_UNIQUE, TYPE, INDEX_QUALIFIER, INDEX_NAME, ORDINAL_POSITION";

    sStmt = (zlsStmt*)aStatementHandle;
    sDbc  = sStmt->mParentDbc;

    STL_TRY( zldDiagClear( SQL_HANDLE_STMT,
                           aStatementHandle,
                           aErrorStack ) == STL_SUCCESS );

    STL_TRY( zlcCheckSession( sDbc,
                              aErrorStack ) == STL_SUCCESS );
    
    switch( sDbc->mTransition )
    {
        case ZLC_TRANSITION_STATE_C0 :
        case ZLC_TRANSITION_STATE_C1 :
        case ZLC_TRANSITION_STATE_C2 :
        case ZLC_TRANSITION_STATE_C3 :
        case ZLC_TRANSITION_STATE_C4 :
            STL_THROW( RAMP_ERR_INVALID_HANDLE );
            break;
        case ZLC_TRANSITION_STATE_C5 :
        case ZLC_TRANSITION_STATE_C6 :
            break;
    }

    switch( sStmt->mTransition )
    {
        case ZLS_TRANSITION_STATE_S1 :
        case ZLS_TRANSITION_STATE_S2 :
        case ZLS_TRANSITION_STATE_S3 :
        case ZLS_TRANSITION_STATE_S4 :
            break;
        case ZLS_TRANSITION_STATE_S5 :
            STL_THROW( RAMP_ERR_INVALID_CURSOR_STATE_1 );
            break;
        case ZLS_TRANSITION_STATE_S6 :
        case ZLS_TRANSITION_STATE_S7 :
            STL_THROW( RAMP_ERR_INVALID_CURSOR_STATE_2 );
            break;
        case ZLS_TRANSITION_STATE_S8 :
        case ZLS_TRANSITION_STATE_S9 :
        case ZLS_TRANSITION_STATE_S10 :
        case ZLS_TRANSITION_STATE_S11 :
        case ZLS_TRANSITION_STATE_S12 :
            STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR );
            break;
    }

    if( aNameLength1 != SQL_NTS )
    {
        STL_TRY_THROW( aNameLength1 >= 0, RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH );

        sNameLength1 = aNameLength1;
    }
    else
    {
        if (aCatalogName != NULL)
        {
            sNameLength1 = stlStrlen((stlChar*)aCatalogName);
        }
    }

    if( aNameLength2 != SQL_NTS )
    {
        STL_TRY_THROW( aNameLength2 >= 0, RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH );

        sNameLength2 = aNameLength2;
    }
    else
    {
        if (aSchemaName != NULL)
        {
            sNameLength2 = stlStrlen((stlChar*)aSchemaName);
        }
    }

    if( aNameLength3 != SQL_NTS )
    {
        STL_TRY_THROW( aNameLength3 >= 0, RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH );

        sNameLength3 = aNameLength3;
    }
    else
    {
        if (aTableName != NULL)
        {
            sNameLength3 = stlStrlen((stlChar*)aTableName);
        }
    }

    STL_TRY_THROW( (aUnique == SQL_INDEX_UNIQUE) ||
                   (aUnique == SQL_INDEX_ALL),
                   RAMP_ERR_UNIQUENESS_OPTION_TYPE_OUT_OF_RANGE );

    STL_TRY_THROW( (aReserved == SQL_ENSURE) ||
                   (aReserved == SQL_QUICK),
                   RAMP_ERR_ACCURACY_OPTION_TYPE_OUT_OF_RANGE );

    STL_TRY_THROW( aTableName != NULL, RAMP_ERR_INVALID_USE_OF_NULL_POINTER_1 );
    
    if( sStmt->mMetadataId == STL_TRUE )
    {
        if( aCatalogName != NULL )
        {
            if( (aCatalogName[0] == '\"') &&
                (aCatalogName[sNameLength1-1] == '\"') )
            {
                aCatalogName[sNameLength1-1] = 0;
                aCatalogName++;

                sNameLength1 = sNameLength1 - 2;
            }
            else
            {
                stlToupperString( (stlChar*)aCatalogName, (stlChar*)aCatalogName );
            }
        }
        
        STL_TRY_THROW( aSchemaName != NULL, RAMP_ERR_INVALID_USE_OF_NULL_POINTER_2 );

        if( (aSchemaName[0] == '\"') &&
            (aSchemaName[sNameLength2-1] == '\"') )
        {
            aSchemaName[sNameLength2-1] = 0;
            aSchemaName++;

            sNameLength2 = sNameLength2 - 2;
        }
        else
        {
            stlToupperString( (stlChar*)aSchemaName, (stlChar*)aSchemaName );
        }

        if( (aTableName[0] == '\"') &&
            (aTableName[sNameLength3-1] == '\"') )
        {
            aTableName[sNameLength3-1] = 0;
            aTableName++;

            sNameLength3 = sNameLength3 - 2;
        }
        else
        {
            stlToupperString( (stlChar*)aTableName, (stlChar*)aTableName );
        }
    }

    sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos,
                         "%s%s",
                         sSELECT,
                         sWHERE );

    if( (aCatalogName != NULL) && (sNameLength1 > 0) )
    {
        sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos,
                             " AND TABLE_CATALOG = '" );
            
        stlMemcpy( sBuffer + sPos, aCatalogName, sNameLength1 );
        sPos += sNameLength1;

        sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos, "' " );
    }

    if( (aSchemaName != NULL) && (sNameLength2 > 0) )
    {
        sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos,
                             " AND TABLE_SCHEMA = '" );
            
        stlMemcpy( sBuffer + sPos, aSchemaName, sNameLength2 );
        sPos += sNameLength2;

        sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos, "' " );
    }

    sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos,
                         " AND TABLE_NAME = '" );
            
    stlMemcpy( sBuffer + sPos, aTableName, sNameLength3 );
    sPos += sNameLength3;

    sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos, "' " );

    if( aUnique == SQL_INDEX_UNIQUE )
    {
        sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos,
                             "AND ( NON_UNIQUE IS NULL OR NON_UNIQUE = FALSE )" );
    }

    sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos,
                         "%s",
                         sORDER_BY );

    STL_TRY( zlxExecDirect( sStmt,
                            sBuffer,
                            &sAffectedRowCount,
                            &sSuccessWithInfo,
                            aErrorStack ) == STL_SUCCESS );

    STL_TRY_THROW( sSuccessWithInfo == STL_FALSE, RAMP_SUCCESS_WITH_INFO );

    sRet = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_ERR_INVALID_CURSOR_STATE_1 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CURSOR_STATE,
                      "A cursor was open on the StatementHandle, "
                      "but SQLFetch or SQLFetchScroll had not been called.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_CURSOR_STATE_2 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CURSOR_STATE,
                      "A cursor was open on the StatementHandle, "
                      "and SQLFetch or SQLFetchScroll had been called.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_STRING_OR_BUFFER_LENGTH,
                      "The value of one of the length arguments was less than 0 "
                      "but not equal to SQL_NTS.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_UNIQUENESS_OPTION_TYPE_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_UNIQUENESS_OPTION_TYPE_OUT_OF_RANGE,
                      "An invalid Unique value was specified.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_ACCURACY_OPTION_TYPE_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_ACCURACY_OPTION_TYPE_OUT_OF_RANGE,
                      "An invalid Reserved value was specified.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_USE_OF_NULL_POINTER_1 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_USE_OF_NULL_POINTER,
                      "The TableName argument was a null pointer.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_USE_OF_NULL_POINTER_2 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_USE_OF_NULL_POINTER,
                      "The SQL_ATTR_METADATA_ID statement attribute was set to SQL_TRUE, "
                      "and the SchemaName argument was a null pointer.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_CATCH( RAMP_SUCCESS_WITH_INFO )
    {
        sRet = SQL_SUCCESS_WITH_INFO;
    }

    STL_FINISH;

    return sRet;
}

SQLRETURN SQL_API SQLStatistics( SQLHSTMT       aStatementHandle,
                                 SQLCHAR      * aCatalogName,
                                 SQLSMALLINT    aNameLength1,
                                 SQLCHAR      * aSchemaName,
                                 SQLSMALLINT    aNameLength2,
                                 SQLCHAR      * aTableName,
                                 SQLSMALLINT    aNameLength3,
                                 SQLUSMALLINT   aUnique,
                                 SQLUSMALLINT   aReserved )
{
    SQLRETURN       sRet  = SQL_ERROR;
    zlsStmt       * sStmt = NULL;
    stlErrorStack   sErrorStack;
    stlErrorData  * sErrorData = NULL;
    stlRegionMark   sMark;
    stlInt32        sState = 0;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sStmt = (zlsStmt*)aStatementHandle;

    STL_TRY_THROW( sStmt != NULL,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sStmt->mType == SQL_HANDLE_STMT,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( stlMarkRegionMem( &sStmt->mAllocator,
                               &sMark,
                               &sErrorStack ) == STL_SUCCESS );
    sState = 1;

    sRet = zllStatistics( aStatementHandle,
                          aCatalogName,
                          aNameLength1,
                          aSchemaName,
                          aNameLength2,
                          aTableName,
                          aNameLength3,
                          aUnique,
                          aReserved,
                          &sErrorStack );

    STL_TRY( sRet == SQL_SUCCESS );

    sState = 0;
    STL_TRY( stlRestoreRegionMem( &sStmt->mAllocator,
                                  &sMark,
                                  STL_TRUE,
                                  &sErrorStack ) == STL_SUCCESS );

    zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

    return SQL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_FINISH;

    if( ( sRet == SQL_ERROR ) ||
        ( sRet == SQL_SUCCESS_WITH_INFO ) )
    {
        zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_STMT,
                               aStatementHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }

        if( sStmt != NULL )
        {
            zlcCheckSessionAndCleanup( ZLS_STMT_DBC(sStmt) );
        }
    }

    switch( sState )
    {
        case 1 :
            stlRestoreRegionMem( &sStmt->mAllocator, &sMark, STL_TRUE, &sErrorStack );
        default :
            break;
    }

    return sRet;
}

SQLRETURN SQL_API SQLStatisticsW( SQLHSTMT       aStatementHandle,
                                  SQLWCHAR     * aCatalogName,
                                  SQLSMALLINT    aNameLength1,
                                  SQLWCHAR     * aSchemaName,
                                  SQLSMALLINT    aNameLength2,
                                  SQLWCHAR     * aTableName,
                                  SQLSMALLINT    aNameLength3,
                                  SQLUSMALLINT   aUnique,
                                  SQLUSMALLINT   aReserved )
{
    /**
     * aNameLength1
     *     Length of *CatalogName, in characters.
     * aNameLength2
     *     Length of *SchemaName, in characters.
     * aNameLength3
     *     Length of *TableName, in characters.
     */

    SQLRETURN       sRet  = SQL_ERROR;
    zlsStmt       * sStmt = NULL;
    stlErrorStack   sErrorStack;
    stlErrorData  * sErrorData = NULL;
    stlRegionMark   sMark;
    stlInt32        sState = 0;

    stlInt64        sBufferLength;
    SQLCHAR       * sCatalogName = NULL;
    stlInt64        sNameLength1 = 0;
    SQLCHAR       * sSchemaName = NULL;
    stlInt64        sNameLength2 = 0;
    SQLCHAR       * sTableName = NULL;
    stlInt64        sNameLength3 = 0;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sStmt = (zlsStmt*)aStatementHandle;

    STL_TRY_THROW( sStmt != NULL,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sStmt->mType == SQL_HANDLE_STMT,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( stlMarkRegionMem( &sStmt->mAllocator,
                               &sMark,
                               &sErrorStack ) == STL_SUCCESS );
    sState = 1;

    sBufferLength = 0;    
    STL_TRY( zlvAllocCharBuffer( aCatalogName,
                                 aNameLength1,
                                 ZLS_STMT_DBC(sStmt)->mCharacterSet,
                                 &sCatalogName,
                                 &sBufferLength,
                                 &sStmt->mAllocator,
                                 &sErrorStack ) == STL_SUCCESS );
    
    STL_TRY( zlvWcharToChar( aCatalogName,
                             aNameLength1,
                             ZLS_STMT_DBC(sStmt)->mCharacterSet,
                             sCatalogName,
                             sBufferLength,
                             &sNameLength1,
                             &sErrorStack ) == STL_SUCCESS );
    
    sBufferLength = 0;    
    STL_TRY( zlvAllocCharBuffer( aSchemaName,
                                 aNameLength2,
                                 ZLS_STMT_DBC(sStmt)->mCharacterSet,
                                 &sSchemaName,
                                 &sBufferLength,
                                 &sStmt->mAllocator,
                                 &sErrorStack ) == STL_SUCCESS );
    
    STL_TRY( zlvWcharToChar( aSchemaName,
                             aNameLength2,
                             ZLS_STMT_DBC(sStmt)->mCharacterSet,
                             sSchemaName,
                             sBufferLength,
                             &sNameLength2,
                             &sErrorStack ) == STL_SUCCESS );
    
    sBufferLength = 0;    
    STL_TRY( zlvAllocCharBuffer( aTableName,
                                 aNameLength3,
                                 ZLS_STMT_DBC(sStmt)->mCharacterSet,
                                 &sTableName,
                                 &sBufferLength,
                                 &sStmt->mAllocator,
                                 &sErrorStack ) == STL_SUCCESS );
    
    STL_TRY( zlvWcharToChar( aTableName,
                             aNameLength3,
                             ZLS_STMT_DBC(sStmt)->mCharacterSet,
                             sTableName,
                             sBufferLength,
                             &sNameLength3,
                             &sErrorStack ) == STL_SUCCESS );
    
    sRet = zllStatistics( aStatementHandle,
                          sCatalogName,
                          sNameLength1,
                          sSchemaName,
                          sNameLength2,
                          sTableName,
                          sNameLength3,
                          aUnique,
                          aReserved,
                          &sErrorStack );

    STL_TRY( sRet == SQL_SUCCESS );

    sState = 0;
    STL_TRY( stlRestoreRegionMem( &sStmt->mAllocator,
                                  &sMark,
                                  STL_TRUE,
                                  &sErrorStack ) == STL_SUCCESS );

    zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

    return SQL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_FINISH;

    if( ( sRet == SQL_ERROR ) ||
        ( sRet == SQL_SUCCESS_WITH_INFO ) )
    {
        zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_STMT,
                               aStatementHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }

        if( sStmt != NULL )
        {
            zlcCheckSessionAndCleanup( ZLS_STMT_DBC(sStmt) );
        }
    }

    switch( sState )
    {
        case 1 :
            (void)stlRestoreRegionMem( &sStmt->mAllocator, &sMark, STL_TRUE, &sErrorStack );
        default :
            break;
    }

    return sRet;
}

static SQLRETURN zllTablePrivileges( SQLHSTMT        aStatementHandle,
                                     SQLCHAR       * aCatalogName,
                                     SQLSMALLINT     aNameLength1,
                                     SQLCHAR       * aSchemaName,
                                     SQLSMALLINT     aNameLength2,
                                     SQLCHAR       * aTableName,
                                     SQLSMALLINT     aNameLength3,
                                     stlErrorStack * aErrorStack )
{
    SQLRETURN       sRet              = SQL_ERROR;
    zlcDbc        * sDbc              = NULL;
    zlsStmt       * sStmt             = NULL;
    stlInt64        sAffectedRowCount = -1;
    stlBool         sSuccessWithInfo  = STL_FALSE;

    stlChar         sBuffer[2048];
    stlInt32        sBufferLength = STL_SIZEOF( sBuffer );

    stlInt16        sNameLength1 = 0;
    stlInt16        sNameLength2 = 0;
    stlInt16        sNameLength3 = 0;

    stlChar         sOp[5];
    stlInt32        sPos = 0;

    SQLCHAR * sSELECT   = (SQLCHAR*)"SELECT "
        "  CAST(TABLE_CATALOG  AS VARCHAR(128 OCTETS)) AS TABLE_CAT, "
        "  CAST(TABLE_SCHEMA   AS VARCHAR(128 OCTETS)) AS TABLE_SCHEM, "
        "  CAST(TABLE_NAME     AS VARCHAR(128 OCTETS)) AS TABLE_NAME, "
        "  CAST(GRANTOR        AS VARCHAR(128 OCTETS)) AS GRANTOR, "
        "  CAST(GRANTEE        AS VARCHAR(128 OCTETS)) AS GRANTEE, "
        "  CAST(PRIVILEGE_TYPE AS VARCHAR(128 OCTETS)) AS PRIVILEGE, "
        "  CAST(CASE IS_GRANTABLE "
        "         WHEN TRUE  THEN 'YES' "
        "         WHEN FALSE THEN 'NO' "
        "         ELSE            'NULL' "
        "         END          AS VARCHAR(3 OCTETS))   AS IS_GRANTABLE "
        "FROM INFORMATION_SCHEMA.TABLE_PRIVILEGES ";
    SQLCHAR * sWHERE    = (SQLCHAR*)"WHERE 1 = 1 ";
    SQLCHAR * sORDER_BY = (SQLCHAR*)"ORDER BY TABLE_CAT, TABLE_SCHEM, TABLE_NAME, PRIVILEGE, GRANTEE ";

    sStmt = (zlsStmt*)aStatementHandle;
    sDbc  = sStmt->mParentDbc;

    STL_TRY( zldDiagClear( SQL_HANDLE_STMT,
                           aStatementHandle,
                           aErrorStack ) == STL_SUCCESS );

    STL_TRY( zlcCheckSession( sDbc,
                              aErrorStack ) == STL_SUCCESS );
    
    switch( sDbc->mTransition )
    {
        case ZLC_TRANSITION_STATE_C0 :
        case ZLC_TRANSITION_STATE_C1 :
        case ZLC_TRANSITION_STATE_C2 :
        case ZLC_TRANSITION_STATE_C3 :
        case ZLC_TRANSITION_STATE_C4 :
            STL_THROW( RAMP_ERR_INVALID_HANDLE );
            break;
        case ZLC_TRANSITION_STATE_C5 :
        case ZLC_TRANSITION_STATE_C6 :
            break;
    }

    switch( sStmt->mTransition )
    {
        case ZLS_TRANSITION_STATE_S1 :
        case ZLS_TRANSITION_STATE_S2 :
        case ZLS_TRANSITION_STATE_S3 :
        case ZLS_TRANSITION_STATE_S4 :
            break;
        case ZLS_TRANSITION_STATE_S5 :
            STL_THROW( RAMP_ERR_INVALID_CURSOR_STATE_1 );
            break;
        case ZLS_TRANSITION_STATE_S6 :
        case ZLS_TRANSITION_STATE_S7 :
            STL_THROW( RAMP_ERR_INVALID_CURSOR_STATE_2 );
            break;
        case ZLS_TRANSITION_STATE_S8 :
        case ZLS_TRANSITION_STATE_S9 :
        case ZLS_TRANSITION_STATE_S10 :
        case ZLS_TRANSITION_STATE_S11 :
        case ZLS_TRANSITION_STATE_S12 :
            STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR );
            break;
    }

    if( aNameLength1 != SQL_NTS )
    {
        STL_TRY_THROW( aNameLength1 >= 0, RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH );

        sNameLength1 = aNameLength1;
    }
    else
    {
        if (aCatalogName != NULL)
        {
            sNameLength1 = stlStrlen((stlChar*)aCatalogName);
        }
    }

    if( aNameLength2 != SQL_NTS )
    {
        STL_TRY_THROW( aNameLength2 >= 0, RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH );

        sNameLength2 = aNameLength2;
    }
    else
    {
        if (aSchemaName != NULL)
        {
            sNameLength2 = stlStrlen((stlChar*)aSchemaName);
        }
    }

    if( aNameLength3 != SQL_NTS )
    {
        STL_TRY_THROW( aNameLength3 >= 0, RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH );

        sNameLength3 = aNameLength3;
    }
    else
    {
        if (aTableName != NULL)
        {
            sNameLength3 = stlStrlen((stlChar*)aTableName);
        }
    }

    if( sStmt->mMetadataId == STL_TRUE )
    {
        if( aCatalogName != NULL )
        {
            if( (aCatalogName[0] == '\"') &&
                (aCatalogName[sNameLength1-1] == '\"') )
            {
                aCatalogName[sNameLength1-1] = 0;
                aCatalogName++;

                sNameLength1 = sNameLength1 - 2;
            }
            else
            {
                stlToupperString( (stlChar*)aCatalogName, (stlChar*)aCatalogName );
            }
        }
        
        STL_TRY_THROW( aSchemaName != NULL, RAMP_ERR_INVALID_USE_OF_NULL_POINTER );

        if( (aSchemaName[0] == '\"') &&
            (aSchemaName[sNameLength2-1] == '\"') )
        {
            aSchemaName[sNameLength2-1] = 0;
            aSchemaName++;

            sNameLength2 = sNameLength2 - 2;
        }
        else
        {
            stlToupperString( (stlChar*)aSchemaName, (stlChar*)aSchemaName );
        }

        STL_TRY_THROW( aTableName != NULL, RAMP_ERR_INVALID_USE_OF_NULL_POINTER );

        if( (aTableName[0] == '\"') &&
            (aTableName[sNameLength3-1] == '\"') )
        {
            aTableName[sNameLength3-1] = 0;
            aTableName++;

            sNameLength3 = sNameLength3 - 2;
        }
        else
        {
            stlToupperString( (stlChar*)aTableName, (stlChar*)aTableName );
        }

        stlStrcpy( sOp, "=" );
    }
    else
    {
        stlStrcpy( sOp, "LIKE" );
    }

    sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos,
                         "%s%s",
                         sSELECT,
                         sWHERE );

    if( (aCatalogName != NULL) && (sNameLength1 > 0) )
    {
        sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos,
                             " AND TABLE_CATALOG = '" );
            
        stlMemcpy( sBuffer + sPos, aCatalogName, sNameLength1 );
        sPos += sNameLength1;

        sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos, "' " );
    }

    if( (aSchemaName != NULL) && (sNameLength2 > 0) )
    {
        sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos,
                             " AND TABLE_SCHEMA %s '", sOp );
            
        stlMemcpy( sBuffer + sPos, aSchemaName, sNameLength2 );
        sPos += sNameLength2;

        sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos, "' " );
    }

    if( (aTableName != NULL) && (sNameLength3 > 0) )
    {
        sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos,
                             " AND TABLE_NAME %s '", sOp );
            
        stlMemcpy( sBuffer + sPos, aTableName, sNameLength3 );
        sPos += sNameLength3;

        sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos, "' " );
    }
    
    sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos,
                         "%s",
                         sORDER_BY );

    STL_TRY( zlxExecDirect( sStmt,
                            sBuffer,
                            &sAffectedRowCount,
                            &sSuccessWithInfo,
                            aErrorStack ) == STL_SUCCESS );

    STL_TRY_THROW( sSuccessWithInfo == STL_FALSE, RAMP_SUCCESS_WITH_INFO );

    sRet = SQL_SUCCESS;

    return sRet;

    STL_CATCH( RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_STRING_OR_BUFFER_LENGTH,
                      "The value of one of the length arguments was less than 0 "
                      "but not equal to SQL_NTS.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_CURSOR_STATE_1 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CURSOR_STATE,
                      "A cursor was open on the StatementHandle, "
                      "but SQLFetch or SQLFetchScroll had not been called.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_CURSOR_STATE_2 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CURSOR_STATE,
                      "A cursor was open on the StatementHandle, "
                      "and SQLFetch or SQLFetchScroll had been called.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_USE_OF_NULL_POINTER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_USE_OF_NULL_POINTER,
                      "The SQL_ATTR_METADATA_ID statement attribute was set to SQL_TRUE, "
                      "and the SchemaName or TableName argument was a null pointer.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_CATCH( RAMP_SUCCESS_WITH_INFO )
    {
        sRet = SQL_SUCCESS_WITH_INFO;
    }

    STL_FINISH;

    return sRet;
}

SQLRETURN SQL_API SQLTablePrivileges( SQLHSTMT      aStatementHandle,
                                      SQLCHAR     * aCatalogName,
                                      SQLSMALLINT   aNameLength1,
                                      SQLCHAR     * aSchemaName,
                                      SQLSMALLINT   aNameLength2,
                                      SQLCHAR     * aTableName,
                                      SQLSMALLINT   aNameLength3 )
{
    SQLRETURN       sRet  = SQL_ERROR;
    zlsStmt       * sStmt = NULL;
    stlErrorStack   sErrorStack;
    stlErrorData  * sErrorData = NULL;
    stlRegionMark   sMark;
    stlInt32        sState = 0;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sStmt = (zlsStmt*)aStatementHandle;

    STL_TRY_THROW( sStmt != NULL,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sStmt->mType == SQL_HANDLE_STMT,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( stlMarkRegionMem( &sStmt->mAllocator,
                               &sMark,
                               &sErrorStack ) == STL_SUCCESS );
    sState = 1;

    sRet = zllTablePrivileges( aStatementHandle,
                               aCatalogName,
                               aNameLength1,
                               aSchemaName,
                               aNameLength2,
                               aTableName,
                               aNameLength3,
                               &sErrorStack );

    STL_TRY( sRet == SQL_SUCCESS );

    sState = 0;
    STL_TRY( stlRestoreRegionMem( &sStmt->mAllocator,
                                  &sMark,
                                  STL_TRUE,
                                  &sErrorStack ) == STL_SUCCESS );

    zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

    return SQL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_FINISH;

    if( ( sRet == SQL_ERROR ) ||
        ( sRet == SQL_SUCCESS_WITH_INFO ) )
    {
        zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_STMT,
                               aStatementHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }

        if( sStmt != NULL )
        {
            zlcCheckSessionAndCleanup( ZLS_STMT_DBC(sStmt) );
        }
    }

    switch( sState )
    {
        case 1 :
            stlRestoreRegionMem( &sStmt->mAllocator, &sMark, STL_TRUE, &sErrorStack );
        default :
            break;
    }

    return sRet;
}

SQLRETURN SQL_API SQLTablePrivilegesW( SQLHSTMT      aStatementHandle,
                                       SQLWCHAR    * aCatalogName,
                                       SQLSMALLINT   aNameLength1,
                                       SQLWCHAR    * aSchemaName,
                                       SQLSMALLINT   aNameLength2,
                                       SQLWCHAR    * aTableName,
                                       SQLSMALLINT   aNameLength3 )
{
    /**
     * aNameLength1
     *     Length of *CatalogName, in characters.
     * aNameLength2
     *     Length of *SchemaName, in characters.
     * aNameLength3
     *     Length of *TableName, in characters.
     */

    SQLRETURN       sRet  = SQL_ERROR;
    zlsStmt       * sStmt = NULL;
    stlErrorStack   sErrorStack;
    stlErrorData  * sErrorData = NULL;
    stlRegionMark   sMark;
    stlInt32        sState = 0;

    stlInt64        sBufferLength;
    SQLCHAR       * sCatalogName = NULL;
    stlInt64        sNameLength1 = 0;
    SQLCHAR       * sSchemaName = NULL;
    stlInt64        sNameLength2 = 0;
    SQLCHAR       * sTableName = NULL;
    stlInt64        sNameLength3 = 0;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sStmt = (zlsStmt*)aStatementHandle;

    STL_TRY_THROW( sStmt != NULL,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sStmt->mType == SQL_HANDLE_STMT,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( stlMarkRegionMem( &sStmt->mAllocator,
                               &sMark,
                               &sErrorStack ) == STL_SUCCESS );
    sState = 1;

    sBufferLength = 0;    
    STL_TRY( zlvAllocCharBuffer( aCatalogName,
                                 aNameLength1,
                                 ZLS_STMT_DBC(sStmt)->mCharacterSet,
                                 &sCatalogName,
                                 &sBufferLength,
                                 &sStmt->mAllocator,
                                 &sErrorStack ) == STL_SUCCESS );
    
    STL_TRY( zlvWcharToChar( aCatalogName,
                             aNameLength1,
                             ZLS_STMT_DBC(sStmt)->mCharacterSet,
                             sCatalogName,
                             sBufferLength,
                             &sNameLength1,
                             &sErrorStack ) == STL_SUCCESS );
    
    sBufferLength = 0;    
    STL_TRY( zlvAllocCharBuffer( aSchemaName,
                                 aNameLength2,
                                 ZLS_STMT_DBC(sStmt)->mCharacterSet,
                                 &sSchemaName,
                                 &sBufferLength,
                                 &sStmt->mAllocator,
                                 &sErrorStack ) == STL_SUCCESS );
    
    STL_TRY( zlvWcharToChar( aSchemaName,
                             aNameLength2,
                             ZLS_STMT_DBC(sStmt)->mCharacterSet,
                             sSchemaName,
                             sBufferLength,
                             &sNameLength2,
                             &sErrorStack ) == STL_SUCCESS );
    
    sBufferLength = 0;    
    STL_TRY( zlvAllocCharBuffer( aTableName,
                                 aNameLength3,
                                 ZLS_STMT_DBC(sStmt)->mCharacterSet,
                                 &sTableName,
                                 &sBufferLength,
                                 &sStmt->mAllocator,
                                 &sErrorStack ) == STL_SUCCESS );
    
    STL_TRY( zlvWcharToChar( aTableName,
                             aNameLength3,
                             ZLS_STMT_DBC(sStmt)->mCharacterSet,
                             sTableName,
                             sBufferLength,
                             &sNameLength3,
                             &sErrorStack ) == STL_SUCCESS );
    
    sRet = zllTablePrivileges( aStatementHandle,
                               sCatalogName,
                               sNameLength1,
                               sSchemaName,
                               sNameLength2,
                               sTableName,
                               sNameLength3,
                               &sErrorStack );

    STL_TRY( sRet == SQL_SUCCESS );

    sState = 0;
    STL_TRY( stlRestoreRegionMem( &sStmt->mAllocator,
                                  &sMark,
                                  STL_TRUE,
                                  &sErrorStack ) == STL_SUCCESS );

    zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

    return SQL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_FINISH;

    if( ( sRet == SQL_ERROR ) ||
        ( sRet == SQL_SUCCESS_WITH_INFO ) )
    {
        zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_STMT,
                               aStatementHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }

        if( sStmt != NULL )
        {
            zlcCheckSessionAndCleanup( ZLS_STMT_DBC(sStmt) );
        }
    }

    switch( sState )
    {
        case 1 :
            (void)stlRestoreRegionMem( &sStmt->mAllocator, &sMark, STL_TRUE, &sErrorStack );
        default :
            break;
    }

    return sRet;
}

static SQLRETURN zllTables( SQLHSTMT        aStatementHandle,
                            SQLCHAR       * aCatalogName,
                            SQLSMALLINT     aNameLength1,
                            SQLCHAR       * aSchemaName,
                            SQLSMALLINT     aNameLength2,
                            SQLCHAR       * aTableName,
                            SQLSMALLINT     aNameLength3,
                            SQLCHAR       * aTableType,
                            SQLSMALLINT     aNameLength4,
                            stlErrorStack * aErrorStack )
{
    SQLRETURN       sRet              = SQL_ERROR;
    zlcDbc        * sDbc              = NULL;
    zlsStmt       * sStmt             = NULL;
    stlInt64        sAffectedRowCount = -1;
    stlBool         sSuccessWithInfo  = STL_FALSE;
    stlInt32        sODBCVer          = 0;

    stlChar         sBuffer[2048];
    stlInt32        sBufferLength = STL_SIZEOF( sBuffer );

    stlInt16        sNameLength1 = 0;
    stlInt16        sNameLength2 = 0;
    stlInt16        sNameLength3 = 0;
    stlInt16        sNameLength4 = 0;

    stlChar         sOp[5];
    stlInt32        sPos = 0;
    stlChar       * sSaveStr = NULL;
    stlChar         sDelimeter[] = ",";
    stlChar       * sTableType = NULL;
    stlChar       * sTableTypeString = NULL;
    stlInt32        sTypeLength = 0;

    SQLCHAR * sSELECT_ALL_CATALOGS = (SQLCHAR*)"SELECT DISTINCT CAST(CURRENT_CATALOG() AS VARCHAR(128 OCTETS)) AS TABLE_CAT, "
        "                CAST(NULL              AS VARCHAR(128 OCTETS)) AS TABLE_SCHEM, "
        "                CAST(NULL              AS VARCHAR(128 OCTETS)) AS TABLE_NAME, "
        "                CAST(NULL              AS VARCHAR(254 OCTETS)) AS TABLE_TYPE, "
        "                CAST(NULL              AS VARCHAR(254 OCTETS)) AS REMARKS "
        "FROM INFORMATION_SCHEMA.TABLES ";
    SQLCHAR * sSELECT_ALL_SCHEMAS = (SQLCHAR*)"SELECT DISTINCT CAST(NULL         AS VARCHAR(128 OCTETS)) AS TABLE_CAT, "
        "                CAST(TABLE_SCHEMA AS VARCHAR(128 OCTETS)) AS TABLE_SCHEM, "
        "                CAST(NULL         AS VARCHAR(128 OCTETS)) AS TABLE_NAME, "
        "                CAST(NULL         AS VARCHAR(254 OCTETS)) AS TABLE_TYPE, "
        "                CAST(NULL         AS VARCHAR(254 OCTETS)) AS REMARKS "
        "FROM INFORMATION_SCHEMA.TABLES ";
    SQLCHAR * sSELECT_ALL_TYPES = (SQLCHAR*)"SELECT DISTINCT CAST(NULL           AS VARCHAR(128 OCTETS)) AS TABLE_CAT, "
        "                CAST(NULL           AS VARCHAR(128 OCTETS)) AS TABLE_SCHEM, "
        "                CAST(NULL           AS VARCHAR(128 OCTETS)) AS TABLE_NAME, "
        "                CAST(DBC_TABLE_TYPE AS VARCHAR(254 OCTETS)) AS TABLE_TYPE, "
        "                CAST(NULL           AS VARCHAR(254 OCTETS)) AS REMARKS "
        "FROM INFORMATION_SCHEMA.TABLES ";
    SQLCHAR * sSELECT           = (SQLCHAR*)"SELECT CAST(CURRENT_CATALOG() AS VARCHAR(128 OCTETS)) AS TABLE_CAT, "
        "       CAST(TABLE_SCHEMA      AS VARCHAR(128 OCTETS)) AS TABLE_SCHEM, "
        "       CAST(TABLE_NAME        AS VARCHAR(128 OCTETS)) AS TABLE_NAME, "
        "       CAST(DBC_TABLE_TYPE    AS VARCHAR(254 OCTETS)) AS TABLE_TYPE, "
        "       COMMENTS AS REMARKS "
        "FROM INFORMATION_SCHEMA.TABLES ";
    SQLCHAR * sWHERE            = (SQLCHAR*)"WHERE 1 = 1 ";
    SQLCHAR * sORDER_BY         = (SQLCHAR*)"ORDER BY TABLE_TYPE, TABLE_CAT, TABLE_SCHEM, TABLE_NAME";    

    sStmt = (zlsStmt*)aStatementHandle;
    sDbc  = sStmt->mParentDbc;

    sODBCVer = sDbc->mParentEnv->mAttrVersion;

    STL_TRY( zldDiagClear( SQL_HANDLE_STMT,
                           aStatementHandle,
                           aErrorStack ) == STL_SUCCESS );

    STL_TRY( zlcCheckSession( sDbc,
                              aErrorStack ) == STL_SUCCESS );
    
    switch( sDbc->mTransition )
    {
        case ZLC_TRANSITION_STATE_C0 :
        case ZLC_TRANSITION_STATE_C1 :
        case ZLC_TRANSITION_STATE_C2 :
        case ZLC_TRANSITION_STATE_C3 :
        case ZLC_TRANSITION_STATE_C4 :
            STL_THROW( RAMP_ERR_INVALID_HANDLE );
            break;
        case ZLC_TRANSITION_STATE_C5 :
        case ZLC_TRANSITION_STATE_C6 :
            break;
    }

    switch( sStmt->mTransition )
    {
        case ZLS_TRANSITION_STATE_S1 :
        case ZLS_TRANSITION_STATE_S2 :
        case ZLS_TRANSITION_STATE_S3 :
        case ZLS_TRANSITION_STATE_S4 :
            break;
        case ZLS_TRANSITION_STATE_S5 :
            STL_THROW( RAMP_ERR_INVALID_CURSOR_STATE_1 );
            break;
        case ZLS_TRANSITION_STATE_S6 :
        case ZLS_TRANSITION_STATE_S7 :
            STL_THROW( RAMP_ERR_INVALID_CURSOR_STATE_2 );
            break;
        case ZLS_TRANSITION_STATE_S8 :
        case ZLS_TRANSITION_STATE_S9 :
        case ZLS_TRANSITION_STATE_S10 :
        case ZLS_TRANSITION_STATE_S11 :
        case ZLS_TRANSITION_STATE_S12 :
            STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR );
            break;
    }

    if( aNameLength1 != SQL_NTS )
    {
        STL_TRY_THROW( aNameLength1 >= 0, RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH );

        sNameLength1 = aNameLength1;
    }
    else
    {
        if (aTableName != NULL)
        {
            sNameLength1 = stlStrlen((stlChar*)aCatalogName);
        }
    }
    
    if( aNameLength2 != SQL_NTS )
    {
        STL_TRY_THROW( aNameLength2 >= 0, RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH );

        sNameLength2 = aNameLength2;
    }
    else
    {
        if (aSchemaName != NULL)
        {
            sNameLength2 = stlStrlen((stlChar*)aSchemaName);
        }
    }

    if( aNameLength3 != SQL_NTS )
    {
        STL_TRY_THROW( aNameLength3 >= 0, RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH );

        sNameLength3 = aNameLength3;
    }
    else
    {
        sNameLength3 = 0;

        if (aTableName != NULL)
        {
            sNameLength3 = stlStrlen((stlChar*)aTableName);
        }
    }

    if( aNameLength4 != SQL_NTS )
    {
        STL_TRY_THROW( aNameLength4 >= 0, RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH );

        sNameLength4 = aNameLength4;
    }
    else
    {
        if (aTableType != NULL)
        {
            sNameLength4 = stlStrlen((stlChar*)aTableType);
        }        
    }

    if( sNameLength4 > 0 )
    {
        STL_TRY( stlAllocRegionMem( &sStmt->mAllocator,
                                    sNameLength4 + 1,
                                    (void**)&sTableTypeString,
                                    aErrorStack ) == STL_SUCCESS );

        stlStrncpy(sTableTypeString, (stlChar*)aTableType, sNameLength4 + 1);
    }

    if( sStmt->mMetadataId == STL_TRUE )
    {
        if( aCatalogName != NULL )
        {
            if( (aCatalogName[0] == '\"') &&
                (aCatalogName[sNameLength1-1] == '\"') )
            {
                aCatalogName[sNameLength1-1] = 0;
                aCatalogName++;

                sNameLength1 = sNameLength1 - 2;
            }
            else
            {
                stlToupperString( (stlChar*)aCatalogName, (stlChar*)aCatalogName );
            }
        }
        
        STL_TRY_THROW( aSchemaName != NULL, RAMP_ERR_INVALID_USE_OF_NULL_POINTER );

        if( (aSchemaName[0] == '\"') &&
            (aSchemaName[sNameLength2-1] == '\"') )
        {
            aSchemaName[sNameLength2-1] = 0;
            aSchemaName++;

            sNameLength2 = sNameLength2 - 2;
        }
        else
        {
            stlToupperString( (stlChar*)aSchemaName, (stlChar*)aSchemaName );
        }

        STL_TRY_THROW( aTableName != NULL, RAMP_ERR_INVALID_USE_OF_NULL_POINTER );

        if( (aTableName[0] == '\"') &&
            (aTableName[sNameLength3-1] == '\"') )
        {
            aTableName[sNameLength3-1] = 0;
            aTableName++;

            sNameLength3 = sNameLength3 - 2;
        }
        else
        {
            stlToupperString( (stlChar*)aTableName, (stlChar*)aTableName );
        }
        
        stlStrcpy( sOp, "=" );
    }
    else
    {
        stlStrcpy( sOp, "LIKE" );
    }

    /*
     * 1. If CatalogName is SQL_ALL_CATALOGS and SchemaName and TableName are empty strings,
     *    the result set contains a list of valid catalogs for the data source.  
     */

    if( aCatalogName != NULL )
    {
        if( ( aCatalogName[0] == '%' ) &&
            ( sNameLength2 == 0 ) &&
            ( sNameLength3 == 0 ) )
        {
            STL_TRY_THROW( sODBCVer == SQL_OV_ODBC3, RAMP_ERR_DRIVER_NOT_CAPABLE );

            sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos,
                                 "%s",
                                 sSELECT_ALL_CATALOGS );

            STL_THROW( RAMP_SET_ORDER_BY );
        }
    }

    /*
     * 2. If SchemaName is SQL_ALL_SCHEMAS and CatalogName and TableName are empty strings,
     *    the result set contains a list of valid schemas for the data source.  
     */ 
    
    if( aSchemaName != NULL )
    {
        if( ( aSchemaName[0] == '%' ) &&
            ( sNameLength1 == 0 ) &&
            ( sNameLength3 == 0 ) )
        {
            sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos,
                                 "%s",
                                 sSELECT_ALL_SCHEMAS );

            STL_THROW( RAMP_SET_ORDER_BY );
        }
    }

    /*
     * 3. If TableType is SQL_ALL_TABLE_TYPES and
     *    CatalogName, SchemaName, and TableName are empty strings,
     *    the result set contains a list of valid table types for the data source.
     */

    if( aTableType != NULL )
    {
        if( ( aTableType[0] == '%' ) &&
            ( sNameLength1 == 0 ) &&
            ( sNameLength2 == 0 ) &&
            ( sNameLength3 == 0 ) )
        {
            sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos,
                                 "%s",
                                 sSELECT_ALL_TYPES );

            STL_THROW( RAMP_SET_ORDER_BY );
        }
    }

    /*
     * 4. Otherwise
     */

    sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos,
                         "%s%s",
                         sSELECT,
                         sWHERE );

    if( (aCatalogName != NULL) && (sNameLength1 > 0) )
    {
        sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos,
                             " AND CURRENT_CATALOG %s '", sOp );
            
        stlMemcpy( sBuffer + sPos, aCatalogName, sNameLength1 );
        sPos += sNameLength1;

        sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos, "' " );
    }

    if( (aSchemaName != NULL) && (sNameLength2 > 0) )
    {
        sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos,
                             " AND TABLE_SCHEMA %s '", sOp );
            
        stlMemcpy( sBuffer + sPos, aSchemaName, sNameLength2 );
        sPos += sNameLength2;

        sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos, "' " );
    }

    if( (aTableName != NULL) && (sNameLength3 > 0) )
    {
        sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos,
                             " AND TABLE_NAME %s '", sOp );
            
        stlMemcpy( sBuffer + sPos, aTableName, sNameLength3 );
        sPos += sNameLength3;

        sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos, "' " );
    }

    if( aTableType != NULL )
    {
        /*
         * TableType is a comma-separated list of one or more types of tables
         * that are to be returned in the result set.
         */

        sTableType = stlStrtok( sTableTypeString, sDelimeter, &sSaveStr );

        stlTrimString(sTableType);
        sTypeLength = stlStrlen(sTableType);

        /*
         * Each value may optionally be enclosed within <quote> characters.
         */

        if( (sTableType[0] == '\'') &&
            (sTableType[sTypeLength-1] == '\'') )
        {
            sTableType[sTypeLength-1] = 0;
            sTableType++;
        }
        
        sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos,
                             " AND ( DBC_TABLE_TYPE LIKE '%s' ", sTableType );
        
        sTableType = stlStrtok( NULL, sDelimeter, &sSaveStr );

        while( sTableType != NULL )
        {
            stlTrimString(sTableType);
            sTypeLength = stlStrlen(sTableType);

            if( (sTableType[0] == '\'') &&
                (sTableType[sTypeLength-1] == '\'') )
            {
                sTableType[sTypeLength-1] = 0;
                sTableType++;
            }
        
            sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos,
                                 " OR DBC_TABLE_TYPE LIKE '%s' ", sTableType );
            
            sTableType = stlStrtok( NULL, sDelimeter, &sSaveStr );
        }

        sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos, ") " );
    }

    STL_RAMP( RAMP_SET_ORDER_BY );
        
    sPos += stlSnprintf( sBuffer + sPos, sBufferLength - sPos,
                         "%s",
                         sORDER_BY );

    STL_TRY( zlxExecDirect( sStmt,
                            sBuffer,
                            &sAffectedRowCount,
                            &sSuccessWithInfo,
                            aErrorStack ) == STL_SUCCESS );

    STL_TRY_THROW( sSuccessWithInfo == STL_FALSE, RAMP_SUCCESS_WITH_INFO );

    sRet = SQL_SUCCESS;
    
    return sRet;

    STL_CATCH( RAMP_ERR_INVALID_STRING_OR_BUFFER_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_STRING_OR_BUFFER_LENGTH,
                      "The value of one of the length arguments was less than 0 "
                      "but not equal to SQL_NTS.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_CURSOR_STATE_1 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CURSOR_STATE,
                      "A cursor was open on the StatementHandle, "
                      "but SQLFetch or SQLFetchScroll had not been called.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_CURSOR_STATE_2 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CURSOR_STATE,
                      "A cursor was open on the StatementHandle, "
                      "and SQLFetch or SQLFetchScroll had been called.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_USE_OF_NULL_POINTER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_USE_OF_NULL_POINTER,
                      "The SQL_ATTR_METADATA_ID statement attribute was set to SQL_TRUE, "
                      "and the SchemaName or TableName argument was a null pointer.",
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_DRIVER_NOT_CAPABLE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_OPTIONAL_FEATURE_NOT_IMPLEMENTED,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_CATCH( RAMP_SUCCESS_WITH_INFO )
    {
        sRet = SQL_SUCCESS_WITH_INFO;
    }

    STL_FINISH;

    return sRet;
}

SQLRETURN SQL_API SQLTables( SQLHSTMT      aStatementHandle,
                             SQLCHAR     * aCatalogName,
                             SQLSMALLINT   aNameLength1,
                             SQLCHAR     * aSchemaName,
                             SQLSMALLINT   aNameLength2,
                             SQLCHAR     * aTableName,
                             SQLSMALLINT   aNameLength3,
                             SQLCHAR     * aTableType,
                             SQLSMALLINT   aNameLength4 )
{
    SQLRETURN       sRet  = SQL_ERROR;
    zlsStmt       * sStmt = NULL;
    stlErrorStack   sErrorStack;
    stlErrorData  * sErrorData = NULL;
    stlRegionMark   sMark;
    stlInt32        sState = 0;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sStmt = (zlsStmt*)aStatementHandle;

    STL_TRY_THROW( sStmt != NULL,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sStmt->mType == SQL_HANDLE_STMT,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( stlMarkRegionMem( &sStmt->mAllocator,
                               &sMark,
                               &sErrorStack ) == STL_SUCCESS );
    sState = 1;

    sRet = zllTables( aStatementHandle,
                      aCatalogName,
                      aNameLength1,
                      aSchemaName,
                      aNameLength2,
                      aTableName,
                      aNameLength3,
                      aTableType,
                      aNameLength4,
                      &sErrorStack );
    
    STL_TRY( sRet == SQL_SUCCESS );

    sState = 0;
    STL_TRY( stlRestoreRegionMem( &sStmt->mAllocator,
                                  &sMark,
                                  STL_TRUE,
                                  &sErrorStack ) == STL_SUCCESS );

    zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

    return SQL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_FINISH;

    if( ( sRet == SQL_ERROR ) ||
        ( sRet == SQL_SUCCESS_WITH_INFO ) )
    {
        zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_STMT,
                               aStatementHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }

        if( sStmt != NULL )
        {
            zlcCheckSessionAndCleanup( ZLS_STMT_DBC(sStmt) );
        }
    }

    switch( sState )
    {
        case 1 :
            (void)stlRestoreRegionMem( &sStmt->mAllocator, &sMark, STL_TRUE, &sErrorStack );
        default :
            break;
    }

    return sRet;
}

SQLRETURN SQL_API SQLTablesW( SQLHSTMT      aStatementHandle,
                              SQLWCHAR    * aCatalogName,
                              SQLSMALLINT   aNameLength1,
                              SQLWCHAR    * aSchemaName,
                              SQLSMALLINT   aNameLength2,
                              SQLWCHAR    * aTableName,
                              SQLSMALLINT   aNameLength3,
                              SQLWCHAR    * aTableType,
                              SQLSMALLINT   aNameLength4 )
{
    /**
     * aNameLength1
     *     Length of *CatalogName, in characters.
     * aNameLength2
     *     Length of *SchemaName, in characters.
     * aNameLength3
     *     Length of *TableName, in characters.
     */

    SQLRETURN       sRet  = SQL_ERROR;
    zlsStmt       * sStmt = NULL;
    stlErrorStack   sErrorStack;
    stlErrorData  * sErrorData = NULL;
    stlRegionMark   sMark;
    stlInt32        sState = 0;

    stlInt64        sBufferLength;
    SQLCHAR       * sCatalogName = NULL;
    stlInt64        sNameLength1 = 0;
    SQLCHAR       * sSchemaName = NULL;
    stlInt64        sNameLength2 = 0;
    SQLCHAR       * sTableName = NULL;
    stlInt64        sNameLength3 = 0;
    SQLCHAR       * sTableType = NULL;
    stlInt64        sNameLength4 = 0;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sStmt = (zlsStmt*)aStatementHandle;

    STL_TRY_THROW( sStmt != NULL,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sStmt->mType == SQL_HANDLE_STMT,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( stlMarkRegionMem( &sStmt->mAllocator,
                               &sMark,
                               &sErrorStack ) == STL_SUCCESS );
    sState = 1;

    sBufferLength = 0;    
    STL_TRY( zlvAllocCharBuffer( aCatalogName,
                                 aNameLength1,
                                 ZLS_STMT_DBC(sStmt)->mCharacterSet,
                                 &sCatalogName,
                                 &sBufferLength,
                                 &sStmt->mAllocator,
                                 &sErrorStack ) == STL_SUCCESS );
    
    STL_TRY( zlvWcharToChar( aCatalogName,
                             aNameLength1,
                             ZLS_STMT_DBC(sStmt)->mCharacterSet,
                             sCatalogName,
                             sBufferLength,
                             &sNameLength1,
                             &sErrorStack ) == STL_SUCCESS );
    
    sBufferLength = 0;    
    STL_TRY( zlvAllocCharBuffer( aSchemaName,
                                 aNameLength2,
                                 ZLS_STMT_DBC(sStmt)->mCharacterSet,
                                 &sSchemaName,
                                 &sBufferLength,
                                 &sStmt->mAllocator,
                                 &sErrorStack ) == STL_SUCCESS );
    
    STL_TRY( zlvWcharToChar( aSchemaName,
                             aNameLength2,
                             ZLS_STMT_DBC(sStmt)->mCharacterSet,
                             sSchemaName,
                             sBufferLength,
                             &sNameLength2,
                             &sErrorStack ) == STL_SUCCESS );
    
    sBufferLength = 0;    
    STL_TRY( zlvAllocCharBuffer( aTableName,
                                 aNameLength3,
                                 ZLS_STMT_DBC(sStmt)->mCharacterSet,
                                 &sTableName,
                                 &sBufferLength,
                                 &sStmt->mAllocator,
                                 &sErrorStack ) == STL_SUCCESS );
    
    STL_TRY( zlvWcharToChar( aTableName,
                             aNameLength3,
                             ZLS_STMT_DBC(sStmt)->mCharacterSet,
                             sTableName,
                             sBufferLength,
                             &sNameLength3,
                             &sErrorStack ) == STL_SUCCESS );
    
    sBufferLength = 0;    
    STL_TRY( zlvAllocCharBuffer( aTableType,
                                 aNameLength4,
                                 ZLS_STMT_DBC(sStmt)->mCharacterSet,
                                 &sTableType,
                                 &sBufferLength,
                                 &sStmt->mAllocator,
                                 &sErrorStack ) == STL_SUCCESS );
    
    STL_TRY( zlvWcharToChar( aTableType,
                             aNameLength4,
                             ZLS_STMT_DBC(sStmt)->mCharacterSet,
                             sTableType,
                             sBufferLength,
                             &sNameLength4,
                             &sErrorStack ) == STL_SUCCESS );
    
    sRet = zllTables( aStatementHandle,
                      sCatalogName,
                      sNameLength1,
                      sSchemaName,
                      sNameLength2,
                      sTableName,
                      sNameLength3,
                      sTableType,
                      sNameLength4,
                      &sErrorStack );

    STL_TRY( sRet == SQL_SUCCESS );

    sState = 0;
    STL_TRY( stlRestoreRegionMem( &sStmt->mAllocator,
                                  &sMark,
                                  STL_TRUE,
                                  &sErrorStack ) == STL_SUCCESS );

    zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

    return SQL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_FINISH;

    if( ( sRet == SQL_ERROR ) ||
        ( sRet == SQL_SUCCESS_WITH_INFO ) )
    {
        zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_STMT,
                               aStatementHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }

        if( sStmt != NULL )
        {
            zlcCheckSessionAndCleanup( ZLS_STMT_DBC(sStmt) );
        }
    }

    switch( sState )
    {
        case 1 :
            (void)stlRestoreRegionMem( &sStmt->mAllocator, &sMark, STL_TRUE, &sErrorStack );
        default :
            break;
    }

    return sRet;
}

SQLRETURN SQL_API SQLTransact( SQLHENV      aEnvironmentHandle,
                               SQLHDBC      aConnectionHandle,
                               SQLUSMALLINT aCompletionType )
{
    SQLRETURN       sRet        = SQL_ERROR;
    SQLSMALLINT     sHandleType = 0;
    SQLHANDLE       sHandle     = NULL;
    zlnEnv        * sEnv        = NULL;
    zlcDbc        * sDbc        = NULL;
    stlErrorData  * sErrorData  = NULL;
    stlErrorStack   sErrorStack;

    STL_INIT_ERROR_STACK( &sErrorStack );

    if( aConnectionHandle != SQL_NULL_HANDLE )
    {
        sHandleType = SQL_HANDLE_DBC;
        sHandle = aConnectionHandle;
    }
    else if( aEnvironmentHandle != SQL_NULL_HANDLE )
    {
        sHandleType = SQL_HANDLE_ENV;
        sHandle = aEnvironmentHandle;
    }
    else
    {
        STL_THROW( RAMP_ERR_INVALID_HANDLE );
    }

    switch( sHandleType )
    {
        case SQL_HANDLE_DBC :
            sDbc = (zlcDbc*)sHandle;

            STL_TRY_THROW( sDbc != NULL, RAMP_ERR_INVALID_HANDLE );

            STL_TRY_THROW( sDbc->mType == SQL_HANDLE_DBC,
                           RAMP_ERR_INVALID_HANDLE );

            switch( sDbc->mTransition )
            {
                case ZLC_TRANSITION_STATE_C0 :
                case ZLC_TRANSITION_STATE_C1 :
                    STL_THROW( RAMP_ERR_INVALID_HANDLE );
                    break;
                case ZLC_TRANSITION_STATE_C2 :
                case ZLC_TRANSITION_STATE_C3 :
                    STL_TRY( zldDiagClear( SQL_HANDLE_DBC,
                                           sHandle,
                                           &sErrorStack ) == STL_SUCCESS );

                    STL_THROW( RAMP_ERR_CONNECTION_NOT_OPEN );
                    break;
                case ZLC_TRANSITION_STATE_C4 :
                case ZLC_TRANSITION_STATE_C5 :
                case ZLC_TRANSITION_STATE_C6 :
                    STL_TRY( zldDiagClear( SQL_HANDLE_DBC,
                                           sHandle,
                                           &sErrorStack ) == STL_SUCCESS );

                    STL_TRY( zlcCheckSession( sDbc,
                                              &sErrorStack )
                             == STL_SUCCESS );
                    break;
                default:
                    STL_ASSERT( STL_FALSE );
                    break;
            }
            break;
        case SQL_HANDLE_ENV :
            sEnv = (zlnEnv*)sHandle;

            STL_TRY_THROW( sEnv != NULL, RAMP_ERR_INVALID_HANDLE );

            STL_TRY_THROW( sEnv->mType == SQL_HANDLE_ENV,
                           RAMP_ERR_INVALID_HANDLE );

            STL_TRY( zldDiagClear( SQL_HANDLE_ENV,
                                   sHandle,
                                   &sErrorStack ) == STL_SUCCESS );

            STL_THROW( RAMP_ERR_INVALID_ATTRIBUTE_OPTION_IDENTIFIER );
            break;
        default :
            STL_DASSERT( STL_FALSE );
            break;
    }

    switch( aCompletionType )
    {
        case SQL_COMMIT :
        case SQL_ROLLBACK :
            break;
        default :
            STL_THROW( RAMP_ERR_INVALID_TRANSACTION_OPERATION_CODE );
            break;
    }

    STL_TRY( zltEndTran( sDbc,
                         aCompletionType,
                         &sErrorStack ) == STL_SUCCESS );

    switch( sDbc->mTransition )
    {
        case ZLC_TRANSITION_STATE_C4 :
        case ZLC_TRANSITION_STATE_C5 :
            break;
        case ZLC_TRANSITION_STATE_C6 :
            sDbc->mTransition = ZLC_TRANSITION_STATE_C5;
            break;
    }

    sRet = SQL_SUCCESS;

    zldDiagSetRetCode( sHandleType, sHandle, sRet );

    return sRet;

    STL_CATCH( RAMP_ERR_INVALID_ATTRIBUTE_OPTION_IDENTIFIER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_ATTRIBUTE_OPTION_IDENTIFIER,
                      "The value specified for the argument HandleType was not SQL_HANDLE_DBC.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_CONNECTION_NOT_OPEN )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_CONNECTION_NOT_OPEN,
                      "The HandleType was SQL_HANDLE_DBC, "
                      "and the Handle was not in a connected state.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_TRANSACTION_OPERATION_CODE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_TRANSACTION_OPERATION_CODE,
                      "The value specified for the argument aCompletionType was "
                      "neither SQL_COMMIT nor SQL_ROLLBACK.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_FINISH;

    if( sRet == SQL_ERROR )
    {
        zldDiagSetRetCode( sHandleType, sHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );
                
        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( sHandleType,
                               sHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }

        switch( sHandleType )
        {
            case SQL_HANDLE_DBC :
                zlcCheckSessionAndCleanup( sDbc );
                break;
            case SQL_HANDLE_ENV :
                break;
            default :
                STL_ASSERT( STL_FALSE );
                break;
        }
    }

    return sRet;
}

/*****************************
 * 비표준 함수
 *****************************/

SQLRETURN SQL_API zllExecute4Imp( SQLHSTMT             aStatementHandle,
                                  SQLUSMALLINT         aParameterCount,
                                  dtlValueBlockList ** aParameterDataPtr )
{
    SQLRETURN       sRet              = SQL_ERROR;
    zlcDbc        * sDbc              = NULL;
    zlsStmt       * sStmt             = NULL;
    stlInt64        sAffectedRowCount = -1;
    stlErrorData  * sErrorData        = NULL;
    stlErrorStack   sErrorStack;
    stlBool         sSuccessWithInfo  = STL_FALSE;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sStmt = (zlsStmt*)aStatementHandle;

    STL_TRY_THROW( sStmt != NULL, RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sStmt->mType == SQL_HANDLE_STMT,
                   RAMP_ERR_INVALID_HANDLE );

    sDbc = sStmt->mParentDbc;

    STL_TRY( zldDiagClear( SQL_HANDLE_STMT,
                           aStatementHandle,
                           &sErrorStack ) == STL_SUCCESS );

    STL_TRY( zlcCheckSession( sDbc,
                              &sErrorStack ) == STL_SUCCESS );
    
    switch( sDbc->mTransition )
    {
        case ZLC_TRANSITION_STATE_C0 :
        case ZLC_TRANSITION_STATE_C1 :
        case ZLC_TRANSITION_STATE_C2 :
        case ZLC_TRANSITION_STATE_C3 :
        case ZLC_TRANSITION_STATE_C4 :
            STL_THROW( RAMP_ERR_INVALID_HANDLE );
            break;
        case ZLC_TRANSITION_STATE_C5 :
        case ZLC_TRANSITION_STATE_C6 :
            break;
    }

    STL_TRY( zlxExecute4Imp( sStmt,
                             aParameterCount,
                             aParameterDataPtr,
                             &sAffectedRowCount,
                             &sSuccessWithInfo,
                             &sErrorStack ) == STL_SUCCESS );

    STL_ASSERT( sAffectedRowCount > 0 );
    STL_TRY_THROW( sSuccessWithInfo == STL_FALSE, RAMP_SUCCESS_WITH_INFO );

    sRet = SQL_SUCCESS;
    
    zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

    return sRet;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_CATCH( RAMP_SUCCESS_WITH_INFO )
    {
        sRet = SQL_SUCCESS_WITH_INFO;
    }

    STL_FINISH;

    if( ( sRet == SQL_ERROR ) ||
        ( sRet == SQL_SUCCESS_WITH_INFO ) )
    {
        zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_STMT,
                               aStatementHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }
        
        if( sStmt != NULL )
        {
            zlcCheckSessionAndCleanup( ZLS_STMT_DBC(sStmt) );
        }
    }

    return sRet;
}

SQLRETURN SQL_API zllExtendedDescribeParam( SQLHSTMT       aStatementHandle,
                                            SQLUSMALLINT   aParameterNumber,
                                            SQLSMALLINT  * aInputOutputType,
                                            SQLSMALLINT  * aDataTypePtr,
                                            SQLULEN      * aParameterSizePtr,
                                            SQLSMALLINT  * aDecimalDigitsPtr,
                                            SQLSMALLINT  * aNullablePtr )
{
    SQLRETURN       sRet        = SQL_ERROR;
    zlsStmt       * sStmt       = NULL;
    stlUInt64       sParamSize  = 0;
    stlErrorData  * sErrorData  = NULL;
    stlErrorStack   sErrorStack;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sStmt = (zlsStmt*)aStatementHandle;

    STL_TRY_THROW( sStmt != NULL, RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sStmt->mType == SQL_HANDLE_STMT,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( zldDiagClear( SQL_HANDLE_STMT,
                           aStatementHandle,
                           &sErrorStack ) == STL_SUCCESS );

    STL_TRY( zlcCheckSession( sStmt->mParentDbc,
                              &sErrorStack ) == STL_SUCCESS );
    
    switch( sStmt->mTransition )
    {
        case ZLS_TRANSITION_STATE_S0 :
            STL_THROW( RAMP_ERR_INVALID_HANDLE );
            break;
        case ZLS_TRANSITION_STATE_S1 :
            STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR1 );
            break;
        case ZLS_TRANSITION_STATE_S4 :
        case ZLS_TRANSITION_STATE_S5 :
        case ZLS_TRANSITION_STATE_S6 :
        case ZLS_TRANSITION_STATE_S7 :
        case ZLS_TRANSITION_STATE_S8 :
        case ZLS_TRANSITION_STATE_S9 :
        case ZLS_TRANSITION_STATE_S10 :
        case ZLS_TRANSITION_STATE_S11 :
        case ZLS_TRANSITION_STATE_S12 :
            STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR2 );
            break;
        default :
            break;
    }

    STL_TRY_THROW( aParameterNumber > 0, RAMP_ERR_INVALID_DESCRIPTOR_INDEX1 );
    STL_TRY_THROW( aParameterNumber <= sStmt->mParamCount, RAMP_ERR_INVALID_DESCRIPTOR_INDEX2 );

    STL_TRY( zliExtendedDescribeParam( sStmt,
                                       aParameterNumber,
                                       aInputOutputType,
                                       aDataTypePtr,
                                       &sParamSize,
                                       aDecimalDigitsPtr,
                                       aNullablePtr,
                                       &sErrorStack ) == STL_SUCCESS );

    if( aParameterSizePtr != NULL )
    {
        *aParameterSizePtr = (SQLULEN)sParamSize;
    }

    sRet = SQL_SUCCESS;

    zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

    return sRet;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR1 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      "The function was called before calling SQLPrepare or "
                      "SQLExecDirect for the StatementHandle.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR2 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      NULL,
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_DESCRIPTOR_INDEX1 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_DESCRIPTOR_INDEX,
                      "The value specified for the argument ParameterNumber is "
                      "less than 1.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_DESCRIPTOR_INDEX2 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_DESCRIPTOR_INDEX,
                      "The value specified for the argument ParameterNumber was "
                      "greater than the number of parameters in "
                      "the associated SQL statement.",
                      &sErrorStack );
    }

    STL_FINISH;

    if( ( sRet == SQL_ERROR ) || ( sRet == SQL_SUCCESS_WITH_INFO ) )
    {
        zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_STMT,
                               aStatementHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }
        
        if( sStmt != NULL )
        {
            zlcCheckSessionAndCleanup( ZLS_STMT_DBC(sStmt) );
        }
    }

    return sRet;
}

SQLRETURN SQL_API zllFetch4Exp( SQLHSTMT             aStatementHandle,
                                stlAllocator       * aAllocator,
                                dtlValueBlockList ** aDataPtr )
{
    SQLRETURN       sRet             = SQL_ERROR;
    zlsStmt       * sStmt            = NULL;
    stlBool         sNotFound        = STL_FALSE;
    stlErrorData  * sErrorData       = NULL;
    stlErrorStack   sErrorStack;

    STL_INIT_ERROR_STACK( &sErrorStack );

    sStmt = (zlsStmt*)aStatementHandle;

    STL_TRY_THROW( sStmt != NULL, RAMP_ERR_INVALID_HANDLE );

    STL_TRY_THROW( sStmt->mType == SQL_HANDLE_STMT,
                   RAMP_ERR_INVALID_HANDLE );

    STL_TRY( zldDiagClear( SQL_HANDLE_STMT,
                           aStatementHandle,
                           &sErrorStack ) == STL_SUCCESS );

    STL_TRY( zlcCheckSession( sStmt->mParentDbc,
                              &sErrorStack ) == STL_SUCCESS );
    
    switch( sStmt->mTransition )
    {
        case ZLS_TRANSITION_STATE_S0 :
            STL_THROW( RAMP_ERR_INVALID_HANDLE );
            break;
        case ZLS_TRANSITION_STATE_S1 :
        case ZLS_TRANSITION_STATE_S2 :
        case ZLS_TRANSITION_STATE_S3 :
            STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR1 );
            break;
        case ZLS_TRANSITION_STATE_S4 :
            STL_THROW( RAMP_ERR_INVALID_CURSOR_STATE );
            break;
        case ZLS_TRANSITION_STATE_S7 :
            STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR2 );
            break;
        case ZLS_TRANSITION_STATE_S8 :
        case ZLS_TRANSITION_STATE_S9 :
        case ZLS_TRANSITION_STATE_S10 :
        case ZLS_TRANSITION_STATE_S11 :
        case ZLS_TRANSITION_STATE_S12 :
            STL_THROW( RAMP_ERR_FUNCTION_SEQUENCE_ERROR3 );
            break;
        default :
            break;
    }

    STL_TRY_THROW( sStmt->mResult.mBlockCurrentRow != ZLR_ROWSET_AFTER_END,
                   RAMP_NO_DATA );
    
    if( sStmt->mResult.mBlockCurrentRow == ZLR_ROWSET_BEFORE_START )
    {
        sStmt->mResult.mCurrentRowIdx = 1;
    }
    else
    {
        sStmt->mResult.mCurrentRowIdx += sStmt->mResult.mBlockLastRow;
    }
    
    STL_TRY( zlrFetch4Imp( aStatementHandle,
                           aDataPtr,
                           &sNotFound,
                           &sErrorStack ) == STL_SUCCESS );

    sStmt->mTransition = ZLS_TRANSITION_STATE_S6;

    STL_TRY_THROW( sNotFound == STL_FALSE, RAMP_NO_DATA );
    
    sRet = SQL_SUCCESS;

    zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

    return sRet;

    STL_CATCH( RAMP_ERR_INVALID_HANDLE )
    {
        sRet = SQL_INVALID_HANDLE;
    }

    STL_CATCH( RAMP_NO_DATA )
    {
        sRet = SQL_NO_DATA;
    }

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR1 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      "The specified StatementHandle was not in an executed state. "
                      "The function was called without first calling "
                      "SQLExecDirect, SQLExecute or a catalog function.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR2 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      "SQLFetch was called for the StatementHandle after "
                      "SQLExtendedFetch was called and before SQLFreeStmt with "
                      "the SQL_CLOSE option was called.",
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_FUNCTION_SEQUENCE_ERROR3 )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_FUNCTION_SEQUENCE_ERROR,
                      NULL,
                      &sErrorStack );
    }

    STL_CATCH( RAMP_ERR_INVALID_CURSOR_STATE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZLE_ERRCODE_INVALID_CURSOR_STATE,
                      "The StatementHandle was in an executed state but "
                      "no result set was associated with the StatementHandle.",
                      &sErrorStack );
    }

    STL_FINISH;

    if( sRet == SQL_ERROR )
    {
        zldDiagSetRetCode( SQL_HANDLE_STMT, aStatementHandle, sRet );

        sErrorData = stlGetLastErrorData( &sErrorStack );

        if( sErrorData != NULL )
        {
            (void)zldDiagAdds( SQL_HANDLE_STMT,
                               aStatementHandle,
                               SQL_NO_ROW_NUMBER,
                               SQL_NO_COLUMN_NUMBER,
                               &sErrorStack );
        }
        
        if( sStmt != NULL )
        {
            zlcCheckSessionAndCleanup( ZLS_STMT_DBC(sStmt) );
        }
    }

    return sRet;
}

/** @} */
