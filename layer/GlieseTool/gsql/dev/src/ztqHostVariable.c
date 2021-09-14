/*******************************************************************************
 * ztqHostVariable.c
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

#include <stl.h>
#include <goldilocks.h>
#include <sqlext.h>
#include <ztqDef.h>
#include <ztqExecute.h>
#include <ztqDisplay.h>
#include <ztqHostVariable.h>
#include <ztqSqlLex.h>
#include <ztqSqlParser.h>

extern stlInt32 gZtqPageSize;

/**
 * @file ztqHostVariable.c
 * @brief Host Variable Routines
 */

/**
 * @addtogroup ztqHostVariable
 * @{
 */

ztqHostVariable * gZtqHostVarList = NULL;
ztqHostVariable * gZtqLastHostVar = NULL;

stlStatus ztqAssignHostVariable( ztqCmdExecAssignTree * aParseTree,
                                 stlChar              * aCommandString,
                                 stlAllocator         * aAllocator,
                                 stlErrorStack        * aErrorStack )
{
    ztqHostVariable * sHostVariable;
    stlChar         * sSqlString;
    stlChar         * sSqlFormatStr = "SELECT %s INTO :%s FROM DUAL";
    stlChar         * sTargetColumnStr;
    stlInt32          sSqlStringLen;
    ztqParseTree    * sParseTree;
    
    STL_TRY( ztqGetHostVariable( aParseTree->mHostVariable->mName,
                                 &sHostVariable,
                                 aErrorStack )
             == STL_SUCCESS );

    sTargetColumnStr = aCommandString + aParseTree->mSqlStartPos;
    
    while( stlIsspace( *sTargetColumnStr ) == STL_TRUE )
    {
        sTargetColumnStr++;
    }

    /*
     * SQL String의 길이는 Target Column 문자열 길이 +
     * 호스트 변수명의 길이 + SQL format 문자열의 길이이다.
     */
    sSqlStringLen = ( (stlStrlen( sTargetColumnStr ) + 1) +
                      (stlStrlen( aParseTree->mHostVariable->mName ) + 1) +
                      (stlStrlen( sSqlFormatStr ) + 1) );

    STL_TRY( stlAllocRegionMem( aAllocator,
                                sSqlStringLen,
                                (void**)&sSqlString,
                                aErrorStack )
             == STL_SUCCESS );

    stlSnprintf( sSqlString,
                 sSqlStringLen,
                 sSqlFormatStr,
                 sTargetColumnStr,
                 aParseTree->mHostVariable->mName );

    STL_TRY( ztqSqlParseIt( sSqlString,
                            aAllocator,
                            &sParseTree,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqPrepareExecuteSql( sSqlString,
                                   aAllocator,
                                   sParseTree,
                                   STL_FALSE,  /* aPrintResult */
                                   aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus ztqDeclareHostVariable( stlChar       * aVariableName,
                                  ztqDataType   * aDataType,
                                  stlErrorStack * aErrorStack )
{
    ztqHostVariable * sHostVariable = NULL;

    STL_TRY( ztqFindHostVariable( aVariableName,
                                  &sHostVariable,
                                  aErrorStack )
             == STL_SUCCESS );

    if( sHostVariable == NULL )
    {
        STL_TRY( stlAllocHeap( (void**)&sHostVariable,
                               STL_SIZEOF(ztqHostVariable),
                               aErrorStack )
                 == STL_SUCCESS );
        
        ZTQ_INIT_HOST_VARIABLE( sHostVariable );
    
        stlToupperString( aVariableName, sHostVariable->mName );

        sHostVariable->mBindType = ZTQ_HOST_BIND_TYPE_NA;
        sHostVariable->mDataType = aDataType->mDataType;
        sHostVariable->mPrecision = aDataType->mPrecision;
        sHostVariable->mScale = aDataType->mScale;
        sHostVariable->mIndicator = SQL_NULL_DATA;
        sHostVariable->mStringLengthUnit = aDataType->mStringLengthUnit;
        sHostVariable->mNext = NULL;

        stlMemset( sHostVariable->mValue, 0x00, ZTQ_MAX_HOST_VALUE_SIZE );
        
        if( gZtqHostVarList == NULL )
        {
            gZtqHostVarList = sHostVariable;
        }
        else
        {
            gZtqLastHostVar->mNext = sHostVariable;
        }
    
        gZtqLastHostVar = sHostVariable;
    }
    else
    {
        sHostVariable->mDataType = aDataType->mDataType;
        sHostVariable->mPrecision = aDataType->mPrecision;
        sHostVariable->mScale = aDataType->mScale;
        sHostVariable->mIndicator = SQL_NULL_DATA;
        sHostVariable->mStringLengthUnit = aDataType->mStringLengthUnit;

        stlMemset( sHostVariable->mValue, 0x00, ZTQ_MAX_HOST_VALUE_SIZE );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztqPrintAllHostVariable( stlAllocator  * aAllocator,
                                   stlErrorStack * aErrorStack )
{
    ztqTargetColumn   sTargetColumn[2];
    ztqRowBuffer    * sRowBuffer = NULL;
    ztqHostVariable * sHostVariable;
    stlChar           sNullString[STL_SIZEOF(ZTQ_NULL_STRING)];
    stlInt64          sLineCount = 0;
    stlInt32          i = 0;
    stlInt64          sMaxDataSize = 0;
    stlInt64          sDataSize;

    sHostVariable = gZtqHostVarList;

    while( sHostVariable != NULL )
    {
        if( sHostVariable->mIndicator != SQL_NULL_DATA )
        {
            switch( sHostVariable->mDataType )
            {
                case SQL_TYPE_DATE:
                case SQL_TYPE_TIME:
                case SQL_TYPE_TIME_WITH_TIMEZONE:
                case SQL_TYPE_TIMESTAMP:
                case SQL_TYPE_TIMESTAMP_WITH_TIMEZONE:
                    sDataSize = DTL_DATETIME_TO_CHAR_RESULT_STRING_MAX_SIZE;
                    break;
                case SQL_INTERVAL_YEAR:
                case SQL_INTERVAL_MONTH:
                case SQL_INTERVAL_YEAR_TO_MONTH:
                    sDataSize = DTL_INTERVAL_YEAR_TO_MONTH_MAX_DISPLAY_SIZE;
                    break;
                case SQL_INTERVAL_DAY:
                case SQL_INTERVAL_HOUR:
                case SQL_INTERVAL_MINUTE:
                case SQL_INTERVAL_SECOND:
                case SQL_INTERVAL_DAY_TO_HOUR:
                case SQL_INTERVAL_DAY_TO_MINUTE:
                case SQL_INTERVAL_DAY_TO_SECOND:
                case SQL_INTERVAL_HOUR_TO_MINUTE:
                case SQL_INTERVAL_HOUR_TO_SECOND:
                case SQL_INTERVAL_MINUTE_TO_SECOND:
                    sDataSize = DTL_INTERVAL_DAY_TO_SECOND_MAX_DISPLAY_SIZE;
                    break;
                default:            
                    sDataSize = stlStrlen( sHostVariable->mValue );
                    break;
            }
        }
        else
        {
            sDataSize = stlStrlen( ZTQ_NULL_STRING );
        }

        sMaxDataSize = STL_MAX( sMaxDataSize, sDataSize );
        
        sHostVariable = (ztqHostVariable*)sHostVariable->mNext;
    }
    
    sHostVariable = gZtqHostVarList;

    while( sHostVariable != NULL )
    {
        sTargetColumn[0].mDataSize = stlStrlen( sHostVariable->mName );
        sTargetColumn[0].mDataValue = sHostVariable->mName;
        sTargetColumn[0].mDataType = SQL_VARCHAR;
        sTargetColumn[0].mIndicator = stlStrlen( sHostVariable->mName );
        sTargetColumn[0].mDataTypeGroup = ZTQ_GROUP_STRING;
        sTargetColumn[0].mDtlDataType = DTL_TYPE_NA;        
        stlStrcpy( sTargetColumn[0].mColumnFormat, ZTQ_COLUMN_FORMAT_CLASS_STRING );
        stlStrncpy( sTargetColumn[0].mName, "NAME", ZTQ_MAX_COLUMN_NAME );

        stlStrncpy( sTargetColumn[1].mName,
                    "VALUE",
                    ZTQ_MAX_COLUMN_NAME );
        
        sTargetColumn[1].mDataSize = sMaxDataSize;
        
        if( sHostVariable->mIndicator != SQL_NULL_DATA )
        {
            sTargetColumn[1].mDataValue = sHostVariable->mValue;
            sTargetColumn[1].mIntervalColInfo.mLeadingPrecision = sHostVariable->mPrecision;
            sTargetColumn[1].mIntervalColInfo.mSecondPrecision  = sHostVariable->mScale;
        }
        else
        {
            /*
             * null 컬럼 스트링만 받을수 있는 버퍼만 준비한다.
             */
            sTargetColumn[1].mDataValue = sNullString;
        }
    
        sTargetColumn[1].mDataType = sHostVariable->mDataType;
        sTargetColumn[1].mIndicator = sHostVariable->mIndicator;
        sTargetColumn[1].mDtlDataType = DTL_TYPE_NA;

        switch( sTargetColumn[1].mDataType )
        {
            case SQL_CHAR :
            case SQL_VARCHAR :
            case SQL_LONGVARCHAR :
            case SQL_WCHAR :
            case SQL_WVARCHAR :
            case SQL_WLONGVARCHAR :
            case SQL_BINARY :
            case SQL_VARBINARY :
            case SQL_LONGVARBINARY :
            case SQL_BOOLEAN :
            case SQL_TYPE_DATE :
            case SQL_TYPE_TIME :
            case SQL_TYPE_TIME_WITH_TIMEZONE :
            case SQL_TYPE_TIMESTAMP :
            case SQL_TYPE_TIMESTAMP_WITH_TIMEZONE :
            case SQL_INTERVAL_YEAR :
            case SQL_INTERVAL_MONTH :
            case SQL_INTERVAL_DAY :
            case SQL_INTERVAL_HOUR :
            case SQL_INTERVAL_MINUTE :
            case SQL_INTERVAL_SECOND :
            case SQL_INTERVAL_YEAR_TO_MONTH :
            case SQL_INTERVAL_DAY_TO_HOUR :
            case SQL_INTERVAL_DAY_TO_MINUTE :
            case SQL_INTERVAL_DAY_TO_SECOND :
            case SQL_INTERVAL_HOUR_TO_MINUTE :
            case SQL_INTERVAL_HOUR_TO_SECOND :
            case SQL_INTERVAL_MINUTE_TO_SECOND :
                sTargetColumn[1].mDataTypeGroup = ZTQ_GROUP_STRING;
                stlStrcpy( sTargetColumn[1].mColumnFormat,
                           ZTQ_COLUMN_FORMAT_CLASS_STRING );
                break;
            default :
                sTargetColumn[1].mDataTypeGroup = ZTQ_GROUP_NUMERIC;
                stlStrcpy( sTargetColumn[1].mColumnFormat,
                           ZTQ_COLUMN_FORMAT_CLASS_NUMERIC );
                break;
        }
    
        if( sRowBuffer == NULL )
        {
            STL_TRY( ztqAllocRowBuffer( &sRowBuffer,
                                        2,     /* aColumnCount */
                                        gZtqPageSize,
                                        sTargetColumn,
                                        aAllocator,
                                        aErrorStack )
                     == STL_SUCCESS );
        }

        STL_TRY( ztqMakeRowString( sRowBuffer,
                                   sLineCount,
                                   sTargetColumn,
                                   STL_TRUE,
                                   aErrorStack )
                 == STL_SUCCESS );

        sLineCount += 1;
        
        if( sLineCount == gZtqPageSize )
        {
            for( i = 0; i < sLineCount; i++ )
            {
                if( i == 0 )
                {
                    STL_TRY( ztqPrintTableHeader( sRowBuffer,
                                                  sTargetColumn )
                             == STL_SUCCESS );
                }
                
                STL_TRY( ztqPrintRowString( sRowBuffer,
                                            sTargetColumn,
                                            i,
                                            aAllocator,
                                            aErrorStack )
                         == STL_SUCCESS );
            }
            
            ztqPrintf( "\n" );
            
            ztqSetInitRowStringLen( sRowBuffer, sTargetColumn );
            sLineCount = 0;
        }
        
        sHostVariable = (ztqHostVariable*)sHostVariable->mNext;
    }
    
    for( i = 0; i < sLineCount; i++ )
    {
        if( i == 0 )
        {
            STL_TRY( ztqPrintTableHeader( sRowBuffer,
                                          sTargetColumn )
                     == STL_SUCCESS );
        }
                
        STL_TRY( ztqPrintRowString( sRowBuffer,
                                    sTargetColumn,
                                    i,
                                    aAllocator,
                                    aErrorStack )
                 == STL_SUCCESS );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztqPrintHostVariable( stlChar       * aVariableName,
                                stlAllocator  * aAllocator,
                                stlErrorStack * aErrorStack )
{
    ztqTargetColumn   sTargetColumn;
    ztqRowBuffer    * sRowBuffer;
    ztqHostVariable * sHostVariable;
    stlChar           sNullString[STL_SIZEOF(ZTQ_NULL_STRING)];

    STL_TRY( ztqGetHostVariable( aVariableName,
                                 &sHostVariable,
                                 aErrorStack )
             == STL_SUCCESS );

    stlStrncpy( sTargetColumn.mName,
                sHostVariable->mName,
                ZTQ_MAX_COLUMN_NAME );

    if( sHostVariable->mIndicator != SQL_NULL_DATA )
    {
        switch( sHostVariable->mDataType )
        {
            case SQL_TYPE_DATE:
            case SQL_TYPE_TIME:
            case SQL_TYPE_TIME_WITH_TIMEZONE:
            case SQL_TYPE_TIMESTAMP:
            case SQL_TYPE_TIMESTAMP_WITH_TIMEZONE:
                sTargetColumn.mDataSize  = DTL_DATETIME_TO_CHAR_RESULT_STRING_MAX_SIZE;
                break;
            case SQL_INTERVAL_YEAR:
            case SQL_INTERVAL_MONTH:
            case SQL_INTERVAL_YEAR_TO_MONTH:
                sTargetColumn.mDataSize = DTL_INTERVAL_YEAR_TO_MONTH_MAX_DISPLAY_SIZE;
                break;
            case SQL_INTERVAL_DAY:
            case SQL_INTERVAL_HOUR:
            case SQL_INTERVAL_MINUTE:
            case SQL_INTERVAL_SECOND:
            case SQL_INTERVAL_DAY_TO_HOUR:
            case SQL_INTERVAL_DAY_TO_MINUTE:
            case SQL_INTERVAL_DAY_TO_SECOND:
            case SQL_INTERVAL_HOUR_TO_MINUTE:
            case SQL_INTERVAL_HOUR_TO_SECOND:
            case SQL_INTERVAL_MINUTE_TO_SECOND:
                sTargetColumn.mDataSize = DTL_INTERVAL_DAY_TO_SECOND_MAX_DISPLAY_SIZE;
                break;
            default:            
                sTargetColumn.mDataSize = stlStrlen( sHostVariable->mValue );
                break;
        }

        sTargetColumn.mDataValue = sHostVariable->mValue;        
        sTargetColumn.mIntervalColInfo.mLeadingPrecision = sHostVariable->mPrecision;
        sTargetColumn.mIntervalColInfo.mSecondPrecision = sHostVariable->mScale;
        
    }
    else
    {
        /*
         * null 컬럼 스트링만 받을수 있는 버퍼만 준비한다.
         */
        sTargetColumn.mDataValue = sNullString;
        sTargetColumn.mDataSize = stlStrlen( ZTQ_NULL_STRING );
    }
    
    sTargetColumn.mDataType = sHostVariable->mDataType;
    sTargetColumn.mIndicator = sHostVariable->mIndicator;
    sTargetColumn.mDtlDataType = DTL_TYPE_NA;

    switch( sTargetColumn.mDataType )
    {
        case SQL_CHAR :
        case SQL_VARCHAR :
        case SQL_LONGVARCHAR :
        case SQL_WCHAR :
        case SQL_WVARCHAR :
        case SQL_WLONGVARCHAR :
        case SQL_BINARY :
        case SQL_VARBINARY :
        case SQL_LONGVARBINARY :
        case SQL_BOOLEAN :
        case SQL_TYPE_DATE :
        case SQL_TYPE_TIME :
        case SQL_TYPE_TIME_WITH_TIMEZONE :
        case SQL_TYPE_TIMESTAMP :
        case SQL_TYPE_TIMESTAMP_WITH_TIMEZONE :
        case SQL_INTERVAL_YEAR :
        case SQL_INTERVAL_MONTH :
        case SQL_INTERVAL_DAY :
        case SQL_INTERVAL_HOUR :
        case SQL_INTERVAL_MINUTE :
        case SQL_INTERVAL_SECOND :
        case SQL_INTERVAL_YEAR_TO_MONTH :
        case SQL_INTERVAL_DAY_TO_HOUR :
        case SQL_INTERVAL_DAY_TO_MINUTE :
        case SQL_INTERVAL_DAY_TO_SECOND :
        case SQL_INTERVAL_HOUR_TO_MINUTE :
        case SQL_INTERVAL_HOUR_TO_SECOND :
        case SQL_INTERVAL_MINUTE_TO_SECOND :
            sTargetColumn.mDataTypeGroup = ZTQ_GROUP_STRING;
            stlStrcpy( sTargetColumn.mColumnFormat,
                       ZTQ_COLUMN_FORMAT_CLASS_STRING );
            break;
        default :
            sTargetColumn.mDataTypeGroup = ZTQ_GROUP_NUMERIC;
            stlStrcpy( sTargetColumn.mColumnFormat,
                       ZTQ_COLUMN_FORMAT_CLASS_NUMERIC );
            break;
    }

    STL_TRY( ztqAllocRowBuffer( &sRowBuffer,
                                1,     /* aColumnCount */
                                1,     /* aLineBufferCount */
                                &sTargetColumn,
                                aAllocator,
                                aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqMakeRowString( sRowBuffer,
                               0,      /* aLineIdx */
                               &sTargetColumn,
                               STL_TRUE,
                               aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztqPrintTableHeader( sRowBuffer,
                                  &sTargetColumn )
             == STL_SUCCESS );

    STL_TRY( ztqPrintRowString( sRowBuffer,
                                &sTargetColumn,
                                0,
                                aAllocator,
                                aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztqFindHostVariable( stlChar          * aVariableName,
                               ztqHostVariable ** aHostVariable,
                               stlErrorStack    * aErrorStack )
{
    ztqHostVariable * sHostVariable = NULL;

    sHostVariable = gZtqHostVarList;
    
    while( sHostVariable != NULL )
    {
        if( stlStrcasecmp( aVariableName,
                           sHostVariable->mName ) == 0 )
        {
            break;
        }

        sHostVariable = (ztqHostVariable*)sHostVariable->mNext;
    }

    *aHostVariable = sHostVariable;
    
    return STL_SUCCESS;
}

stlStatus ztqGetHostVariable( stlChar          * aVariableName,
                              ztqHostVariable ** aHostVariable,
                              stlErrorStack    * aErrorStack )
{
    ztqHostVariable * sHostVariable = NULL;
    
    STL_TRY( ztqFindHostVariable( aVariableName,
                                  &sHostVariable,
                                  aErrorStack )
             == STL_SUCCESS );

    STL_TRY_THROW( sHostVariable != NULL, RAMP_ERR_HOST_VAR );

    *aHostVariable = sHostVariable;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_HOST_VAR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_NOT_DECLARED_HOST_VARIABLE,
                      NULL,
                      aErrorStack,
                      aVariableName );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztqCloneHostVarList( ztqHostVariable  * aSrcHostVarList,
                               ztqHostVariable ** aDestHostVarList,
                               stlErrorStack    * aErrorStack )
{
    ztqHostVariable * sSrcVar;
    ztqHostVariable * sDestVar;
    ztqHostVariable * sDestPrevVar = NULL;
    
    sSrcVar = aSrcHostVarList;
    *aDestHostVarList = NULL;
    
    while( sSrcVar != NULL )
    {
        STL_TRY( stlAllocHeap( (void**)&sDestVar,
                               STL_SIZEOF( ztqHostVariable ),
                               aErrorStack )
                 == STL_SUCCESS );
        
        stlMemcpy( sDestVar, sSrcVar, STL_SIZEOF( ztqHostVariable ) );

        if( sDestPrevVar != NULL )
        {
            sDestPrevVar->mNext = sDestVar;
        }
        
        sDestPrevVar = sDestVar;

        if( *aDestHostVarList == NULL )
        {
            *aDestHostVarList = sDestVar;
        }
        
        sDestVar->mNext = NULL;
        
        sSrcVar = (ztqHostVariable*)sSrcVar->mNext;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztqAllocSysHostVariable( stlErrorStack * aErrorStack )
{
    ztqDataType sDataType;

    sDataType.mDataType = SQL_VARCHAR;
    sDataType.mPrecision = ZTQ_MAX_HOST_VALUE_SIZE;
    sDataType.mScale = 0;
    sDataType.mStringLengthUnit = SQL_CLU_OCTETS;
    
    STL_TRY( ztqDeclareHostVariable( ZTQ_SYS_VAR_ELAPSED_TIME,
                                     &sDataType,
                                     aErrorStack )
             == STL_SUCCESS );
                                     
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztqDestHostVariables( stlErrorStack * aErrorStack )
{
    ztqHostVariable * sHostVariable = NULL;
    ztqHostVariable * sNextHostVariable;

    sHostVariable = gZtqHostVarList;
    
    while( sHostVariable != NULL )
    {
        sNextHostVariable = (ztqHostVariable*)sHostVariable->mNext;
        stlFreeHeap( sHostVariable );
        sHostVariable = sNextHostVariable;
    }
    
    return STL_SUCCESS;
}

stlStatus ztqAssignSysHostVariable( stlChar       * aVariableName,
                                    stlChar       * aValue,
                                    stlErrorStack * aErrorStack )
{
    ztqHostVariable * sHostVariable;
    
    STL_TRY( ztqFindHostVariable( aVariableName,
                                  &sHostVariable,
                                  aErrorStack )
             == STL_SUCCESS );

    STL_ASSERT( sHostVariable != NULL );

    stlStrncpy( sHostVariable->mValue, aValue, ZTQ_MAX_HOST_VALUE_SIZE );
    sHostVariable->mPrecision = stlStrlen( aValue );
    sHostVariable->mIndicator = stlStrlen( aValue );
        
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */

