/*******************************************************************************
 * zlcdDesc.c
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
#include <zld.h>

#include <zlci.h>

/**
 * @file zlcdDesc.c
 * @brief ODBC API Internal Desc Routines.
 */

/**
 * @addtogroup zld
 * @{
 */


static stlStatus zlcdDescGetConsiceTypeName( dtlDataType      aDbType,
                                             stlInt16         aConciseType,
                                             stlChar       ** aTypeName,
                                             stlErrorStack  * aErrorStack )
{
    stlChar sNameBuffer[STL_MAX_SQL_IDENTIFIER_LENGTH];
    
    if( *aTypeName != NULL )
    {
        stlFreeHeap( *aTypeName );
        *aTypeName = NULL;
    }
    
    switch( aConciseType )
    {
        case SQL_CHAR :
            stlStrncpy( sNameBuffer,
                        "CHARACTER",
                        STL_MAX_SQL_IDENTIFIER_LENGTH );
            break;
        case SQL_VARCHAR :
            stlStrncpy( sNameBuffer,
                        "CHARACTER VARYING",
                        STL_MAX_SQL_IDENTIFIER_LENGTH );
            break;
        case SQL_LONGVARCHAR :
            stlStrncpy( sNameBuffer,
                        "CHARACTER LONG VARYING",
                        STL_MAX_SQL_IDENTIFIER_LENGTH );
            break;
        case SQL_WCHAR :
            stlStrncpy( sNameBuffer,
                        "N/A",
                        STL_MAX_SQL_IDENTIFIER_LENGTH );
            break;
        case SQL_WVARCHAR :
            stlStrncpy( sNameBuffer,
                        "N/A",
                        STL_MAX_SQL_IDENTIFIER_LENGTH );
            break;
        case SQL_WLONGVARCHAR :
            stlStrncpy( sNameBuffer,
                        "N/A",
                        STL_MAX_SQL_IDENTIFIER_LENGTH );
            break;
        case SQL_NUMERIC :
            stlStrncpy( sNameBuffer,
                        "NUMBER",
                        STL_MAX_SQL_IDENTIFIER_LENGTH );
            break;
        case SQL_DECIMAL :
            stlStrncpy( sNameBuffer,
                        "DECIMAL",
                        STL_MAX_SQL_IDENTIFIER_LENGTH );
            break;
        case SQL_FLOAT :
            if( aDbType == DTL_TYPE_NUMBER )
            {
                stlStrncpy( sNameBuffer,
                            "NUMBER",
                            STL_MAX_SQL_IDENTIFIER_LENGTH );
            }
            else
            {
                stlStrncpy( sNameBuffer,
                            "FLOAT",
                            STL_MAX_SQL_IDENTIFIER_LENGTH );
            }
            break;
        case SQL_SMALLINT :
            stlStrncpy( sNameBuffer,
                        "NATIVE_SMALLINT",
                        STL_MAX_SQL_IDENTIFIER_LENGTH );
            break;
        case SQL_INTEGER :
            stlStrncpy( sNameBuffer,
                        "NATIVE_INTEGER",
                        STL_MAX_SQL_IDENTIFIER_LENGTH );
            break;
        case SQL_BIGINT :
            stlStrncpy( sNameBuffer,
                        "NATIVE_BIGINT",
                        STL_MAX_SQL_IDENTIFIER_LENGTH );
            break;
        case SQL_REAL :
            stlStrncpy( sNameBuffer,
                        "NATIVE_REAL",
                        STL_MAX_SQL_IDENTIFIER_LENGTH );
            break;
        case SQL_DOUBLE :
            stlStrncpy( sNameBuffer,
                        "NATIVE_DOUBLE",
                        STL_MAX_SQL_IDENTIFIER_LENGTH );
            break;
        case SQL_BINARY :
            stlStrncpy( sNameBuffer,
                        "BINARY",
                        STL_MAX_SQL_IDENTIFIER_LENGTH );
            break;
        case SQL_VARBINARY :
            stlStrncpy( sNameBuffer,
                        "BINARY VARYING",
                        STL_MAX_SQL_IDENTIFIER_LENGTH );
            break;
        case SQL_LONGVARBINARY :
            stlStrncpy( sNameBuffer,
                        "BINARY LONG VARYING",
                        STL_MAX_SQL_IDENTIFIER_LENGTH );
            break;
        case SQL_TYPE_DATE :
            stlStrncpy( sNameBuffer,
                        "DATE",
                        STL_MAX_SQL_IDENTIFIER_LENGTH );
            break;
        case SQL_TYPE_TIME :
            stlStrncpy( sNameBuffer,
                        "TIME WITHOUT TIME ZONE",
                        STL_MAX_SQL_IDENTIFIER_LENGTH );
            break;
        case SQL_TYPE_TIME_WITH_TIMEZONE :
            stlStrncpy( sNameBuffer,
                        "TIME WITH TIME ZONE",
                        STL_MAX_SQL_IDENTIFIER_LENGTH );
            break;
        case SQL_TYPE_TIMESTAMP :
            if( aDbType == DTL_TYPE_DATE )
            {
                stlStrncpy( sNameBuffer,
                            "DATE",
                            STL_MAX_SQL_IDENTIFIER_LENGTH );                
            }
            else
            {
                stlStrncpy( sNameBuffer,
                            "TIMESTAMP WITHOUT TIME ZONE",
                            STL_MAX_SQL_IDENTIFIER_LENGTH );
            }
            break;
        case SQL_TYPE_TIMESTAMP_WITH_TIMEZONE :
            stlStrncpy( sNameBuffer,
                        "TIMESTAMP WITH TIME ZONE",
                        STL_MAX_SQL_IDENTIFIER_LENGTH );
            break;
        case SQL_INTERVAL_YEAR :
            stlStrncpy( sNameBuffer,
                        "INTERVAL YEAR",
                        STL_MAX_SQL_IDENTIFIER_LENGTH );
            break;
        case SQL_INTERVAL_MONTH :
            stlStrncpy( sNameBuffer,
                        "INTERVAL MONTH",
                        STL_MAX_SQL_IDENTIFIER_LENGTH );
            break;
        case SQL_INTERVAL_YEAR_TO_MONTH :
            stlStrncpy( sNameBuffer,
                        "INTERVAL YEAR TO MONTH",
                        STL_MAX_SQL_IDENTIFIER_LENGTH );
            break;
        case SQL_INTERVAL_DAY :
            stlStrncpy( sNameBuffer,
                        "INTERVAL DAY",
                        STL_MAX_SQL_IDENTIFIER_LENGTH );
            break;
        case SQL_INTERVAL_HOUR :
            stlStrncpy( sNameBuffer,
                        "INTERVAL HOUR",
                        STL_MAX_SQL_IDENTIFIER_LENGTH );
            break;
        case SQL_INTERVAL_MINUTE :
            stlStrncpy( sNameBuffer,
                        "INTERVAL MINUTE",
                        STL_MAX_SQL_IDENTIFIER_LENGTH );
            break;
        case SQL_INTERVAL_SECOND :
            stlStrncpy( sNameBuffer,
                        "INTERVAL SECOND",
                        STL_MAX_SQL_IDENTIFIER_LENGTH );
            break;
        case SQL_INTERVAL_DAY_TO_HOUR :
            stlStrncpy( sNameBuffer,
                        "INTERVAL DAY TO HOUR",
                        STL_MAX_SQL_IDENTIFIER_LENGTH );
            break;
        case SQL_INTERVAL_DAY_TO_MINUTE :
            stlStrncpy( sNameBuffer,
                        "INTERVAL DAY TO MINUTE",
                        STL_MAX_SQL_IDENTIFIER_LENGTH );
            break;
        case SQL_INTERVAL_DAY_TO_SECOND :
            stlStrncpy( sNameBuffer,
                        "INTERVAL DAY TO SECOND",
                        STL_MAX_SQL_IDENTIFIER_LENGTH );
            break;
        case SQL_INTERVAL_HOUR_TO_MINUTE :
            stlStrncpy( sNameBuffer,
                        "INTERVAL HOUR TO MINUTE",
                        STL_MAX_SQL_IDENTIFIER_LENGTH );
            break;
        case SQL_INTERVAL_HOUR_TO_SECOND :
            stlStrncpy( sNameBuffer,
                        "INTERVAL HOUR TO SECOND",
                        STL_MAX_SQL_IDENTIFIER_LENGTH );
            break;
        case SQL_INTERVAL_MINUTE_TO_SECOND :
            stlStrncpy( sNameBuffer,
                        "INTERVAL MINUTE TO SECOND",
                        STL_MAX_SQL_IDENTIFIER_LENGTH );
            break;
        case SQL_BOOLEAN :
            stlStrncpy( sNameBuffer,
                        "BOOLEAN",
                        STL_MAX_SQL_IDENTIFIER_LENGTH );
            break;
        case SQL_ROWID:
            stlStrncpy( sNameBuffer,
                        "ROWID",
                        STL_MAX_SQL_IDENTIFIER_LENGTH );
            break;
        default :
            stlStrncpy( sNameBuffer,
                        "N/A",
                        STL_MAX_SQL_IDENTIFIER_LENGTH );
            break;
    }

    STL_TRY( stlAllocHeap( (void**) aTypeName,
                           stlStrlen( sNameBuffer ) + 1,
                           aErrorStack ) == STL_SUCCESS );
    
    stlStrcpy( *aTypeName, sNameBuffer );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

typedef stlStatus (*zlcdConvertIRDFunc) ( zldDescElement  * aDescRec,
                                          stlChar         * aCatalogName,
                                          stlChar         * aSchemaName,
                                          stlChar         * aTableName,
                                          stlChar         * aBaseTableName,
                                          stlChar         * aColumnAliasName,
                                          stlChar         * aColumnLabel,
                                          stlChar         * aBaseColumnName,
                                          stlInt64          aCharacterLength,
                                          stlInt64          aOctetLength,
                                          stlInt64          aDisplaySize,
                                          dtlCharacterSet   aSourceCharacterSet,
                                          dtlCharacterSet   aDestCharacterSet,
                                          zlsStmt         * aStmt, 
                                          stlErrorStack   * aErrorStack );

static stlStatus zlcdNonConvertIRD( zldDescElement  * aDescRec,
                                    stlChar         * aCatalogName,
                                    stlChar         * aSchemaName,
                                    stlChar         * aTableName,
                                    stlChar         * aBaseTableName,
                                    stlChar         * aColumnAliasName,
                                    stlChar         * aColumnLabel,
                                    stlChar         * aBaseColumnName,
                                    stlInt64          aCharacterLength,
                                    stlInt64          aOctetLength,
                                    stlInt64          aDisplaySize,
                                    dtlCharacterSet   aSourceCharacterSet,
                                    dtlCharacterSet   aDestCharacterSet,
                                    zlsStmt         * aStmt, 
                                    stlErrorStack   * aErrorStack )
{
    /*
     * Target Name 관련 정보
     */

    if( aCatalogName[0] != '\0' )
    {
        if( aDescRec->mCatalogName != NULL )
        {
            stlFreeHeap( aDescRec->mCatalogName );
            aDescRec->mCatalogName = NULL;
        }
        
        STL_TRY( stlAllocHeap( (void**)&aDescRec->mCatalogName,
                               stlStrlen( aCatalogName ) + 1,
                               aErrorStack ) == STL_SUCCESS );
        
        stlStrcpy( aDescRec->mCatalogName, aCatalogName );
    }

    if( aSchemaName[0] != '\0' )
    {
        if( aDescRec->mSchemaName != NULL )
        {
            stlFreeHeap( aDescRec->mSchemaName );
            aDescRec->mSchemaName = NULL;
        }
        
        STL_TRY( stlAllocHeap( (void**)&aDescRec->mSchemaName,
                               stlStrlen( aSchemaName ) + 1,
                               aErrorStack ) == STL_SUCCESS );
        
        stlStrcpy( aDescRec->mSchemaName, aSchemaName );
    }

    if( aTableName[0] != '\0' )
    {
        if( aDescRec->mTableName != NULL )
        {
            stlFreeHeap( aDescRec->mTableName );
            aDescRec->mTableName = NULL;
        }
        
        STL_TRY( stlAllocHeap( (void**)&aDescRec->mTableName,
                               stlStrlen( aTableName ) + 1,
                               aErrorStack ) == STL_SUCCESS );
        
        stlStrcpy( aDescRec->mTableName, aTableName );
    }

    if( aBaseTableName[0] != '\0' )
    {
        if( aDescRec->mBaseTableName != NULL )
        {
            stlFreeHeap( aDescRec->mBaseTableName );
            aDescRec->mBaseTableName = NULL;
        }
        
        STL_TRY( stlAllocHeap( (void**)&aDescRec->mBaseTableName,
                               stlStrlen( aBaseTableName ) + 1,
                               aErrorStack ) == STL_SUCCESS );
        
        stlStrcpy( aDescRec->mBaseTableName, aBaseTableName );
    }

    if( aColumnAliasName[0] != '\0' )
    {
        if( aDescRec->mName != NULL )
        {
            stlFreeHeap( aDescRec->mName );
            aDescRec->mName = NULL;
        }
        
        STL_TRY( stlAllocHeap( (void**)&aDescRec->mName,
                               stlStrlen( aColumnAliasName ) + 1,
                               aErrorStack ) == STL_SUCCESS );
        
        stlStrcpy( aDescRec->mName, aColumnAliasName );
    }

    if( aColumnLabel[0] != '\0' )
    {
        if( aDescRec->mLabel != NULL )
        {
            stlFreeHeap( aDescRec->mLabel );
            aDescRec->mLabel = NULL;
        }
        
        STL_TRY( stlAllocHeap( (void**)&aDescRec->mLabel,
                               stlStrlen( aColumnLabel ) + 1,
                               aErrorStack ) == STL_SUCCESS );
        
        stlStrcpy( aDescRec->mLabel, aColumnLabel );
    }

    if( aBaseColumnName[0] != '\0' )
    {
        if( aDescRec->mBaseColumnName != NULL )
        {
            stlFreeHeap( aDescRec->mBaseColumnName );
            aDescRec->mBaseColumnName = NULL;
        }
        
        STL_TRY( stlAllocHeap( (void**)&aDescRec->mBaseColumnName,
                               stlStrlen( aBaseColumnName ) + 1,
                               aErrorStack ) == STL_SUCCESS );
        
        stlStrcpy( aDescRec->mBaseColumnName, aBaseColumnName );
    }

    /*
     * Target Type 의 문자 관련 Type 정보
     */
    switch( aDescRec->mConsiceType )
    {
        case SQL_CHAR :
        case SQL_VARCHAR :
        case SQL_LONGVARCHAR :
            aDescRec->mLength = aCharacterLength;
            break;
        case SQL_BINARY :
        case SQL_VARBINARY :
        case SQL_LONGVARBINARY :
            aDescRec->mLength = aOctetLength;
            break;
        case SQL_BIT :
            aDescRec->mLength = 1;
            break;
        case SQL_BOOLEAN :
            aDescRec->mLength = 5;
            break;
        case SQL_TYPE_DATE :
            aDescRec->mLength = 
                ((ZLS_STMT_NLS_DT_VECTOR(aStmt))->mGetDateFormatInfoFunc(aStmt))->mToCharMaxResultLen; 
            break;  
        case SQL_TYPE_TIME :
            aDescRec->mLength = 
                ((ZLS_STMT_NLS_DT_VECTOR(aStmt))->mGetTimeFormatInfoFunc(aStmt))->mToCharMaxResultLen; 
            break; 
        case SQL_TYPE_TIME_WITH_TIMEZONE :
            aDescRec->mLength = 
                ((ZLS_STMT_NLS_DT_VECTOR(aStmt))->mGetTimeWithTimeZoneFormatInfoFunc(aStmt))->mToCharMaxResultLen; 
            break; 
        case SQL_TYPE_TIMESTAMP :
            aDescRec->mLength = 
                ((ZLS_STMT_NLS_DT_VECTOR(aStmt))->mGetTimestampFormatInfoFunc(aStmt))->mToCharMaxResultLen; 
            break;     
        case SQL_TYPE_TIMESTAMP_WITH_TIMEZONE :
            aDescRec->mLength = 
                ((ZLS_STMT_NLS_DT_VECTOR(aStmt))->mGetTimestampWithTimeZoneFormatInfoFunc(aStmt))->mToCharMaxResultLen; 
            break; 
        case SQL_INTERVAL_SECOND :
        case SQL_INTERVAL_DAY_TO_SECOND :
        case SQL_INTERVAL_HOUR_TO_SECOND :
        case SQL_INTERVAL_MINUTE_TO_SECOND :
        case SQL_INTERVAL_YEAR :
        case SQL_INTERVAL_MONTH :
        case SQL_INTERVAL_DAY :
        case SQL_INTERVAL_HOUR :
        case SQL_INTERVAL_MINUTE :
        case SQL_INTERVAL_YEAR_TO_MONTH :
        case SQL_INTERVAL_DAY_TO_HOUR :
        case SQL_INTERVAL_DAY_TO_MINUTE :
        case SQL_INTERVAL_HOUR_TO_MINUTE :
            aDescRec->mLength = aDisplaySize;
            break;
        case SQL_ROWID:
            aDescRec->mLength = aDisplaySize;
            break;            
        case SQL_GUID :
            aDescRec->mLength = 36;
            break;
    }

    aDescRec->mOctetLength = aOctetLength;

    /*
     * Target 의 출력 공간
     */

    if( aDisplaySize == DTL_NO_TOTAL_SIZE )
    {
        aDescRec->mDisplaySize = SQL_NO_TOTAL;
    }
    else
    {
        aDescRec->mDisplaySize = aDisplaySize;

        switch( aDescRec->mConsiceType ) 
        { 
            case SQL_TYPE_DATE : 
            case SQL_TYPE_TIME : 
            case SQL_TYPE_TIME_WITH_TIMEZONE : 
            case SQL_TYPE_TIMESTAMP : 
            case SQL_TYPE_TIMESTAMP_WITH_TIMEZONE : 
                aDescRec->mDisplaySize = aDescRec->mLength; 
                break; 
            default: 
                /* Do Nothing */ 
                break; 
        }    
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

static stlStatus zlcdCovnertIRD( zldDescElement  * aDescRec,
                                 stlChar         * aCatalogName,
                                 stlChar         * aSchemaName,
                                 stlChar         * aTableName,
                                 stlChar         * aBaseTableName,
                                 stlChar         * aColumnAliasName,
                                 stlChar         * aColumnLabel,
                                 stlChar         * aBaseColumnName,
                                 stlInt64          aCharacterLength,
                                 stlInt64          aOctetLength,
                                 stlInt64          aDisplaySize,
                                 dtlCharacterSet   aSourceCharacterSet,
                                 dtlCharacterSet   aDestCharacterSet,
                                 zlsStmt         * aStmt, 
                                 stlErrorStack   * aErrorStack )
{
    stlInt64 sOffset;
    stlInt64 sLen;
    stlSize  sSourceLen;
    stlSize  sDestLen;

    /*
     * Target Name 관련 정보
     */

    if( aCatalogName[0] != '\0' )
    {
        if( aDescRec->mCatalogName != NULL )
        {
            stlFreeHeap( aDescRec->mCatalogName );
            aDescRec->mCatalogName = NULL;
        }
        
        sSourceLen = stlStrlen( aCatalogName );

        if( dtlGetMbMaxLength( aSourceCharacterSet ) > dtlGetMbMaxLength( aDestCharacterSet ) )
        {
            sDestLen = sSourceLen;
        }
        else
        {
            sDestLen = sSourceLen * (dtlGetMbMaxLength( aDestCharacterSet ) / dtlGetMbMaxLength( aSourceCharacterSet ));
        }
        
        STL_TRY( stlAllocHeap( (void**)&aDescRec->mCatalogName,
                               sDestLen + 1,
                               aErrorStack ) == STL_SUCCESS );

        STL_TRY( dtlMbConversion( aSourceCharacterSet,
                                  aDestCharacterSet,
                                  aCatalogName,
                                  sSourceLen,
                                  aDescRec->mCatalogName,
                                  sDestLen,
                                  &sOffset,
                                  &sLen,
                                  aErrorStack ) == STL_SUCCESS );

        STL_DASSERT( sLen <= sDestLen );

        aDescRec->mCatalogName[sLen] = '\0';
    }

    if( aSchemaName[0] != '\0' )
    {
        if( aDescRec->mSchemaName != NULL )
        {
            stlFreeHeap( aDescRec->mSchemaName );
            aDescRec->mSchemaName = NULL;
        }
        
        sSourceLen = stlStrlen( aSchemaName );

        if( dtlGetMbMaxLength( aSourceCharacterSet ) > dtlGetMbMaxLength( aDestCharacterSet ) )
        {
            sDestLen = sSourceLen;
        }
        else
        {
            sDestLen = sSourceLen * (dtlGetMbMaxLength( aDestCharacterSet ) / dtlGetMbMaxLength( aSourceCharacterSet ));
        }
        
        STL_TRY( stlAllocHeap( (void**)&aDescRec->mSchemaName,
                               sDestLen + 1,
                               aErrorStack ) == STL_SUCCESS );

        STL_TRY( dtlMbConversion( aSourceCharacterSet,
                                  aDestCharacterSet,
                                  aSchemaName,
                                  sSourceLen,
                                  aDescRec->mSchemaName,
                                  sDestLen,
                                  &sOffset,
                                  &sLen,
                                  aErrorStack ) == STL_SUCCESS );

        STL_DASSERT( sLen <= sDestLen );

        aDescRec->mSchemaName[sLen] = '\0';
    }

    if( aTableName[0] != '\0' )
    {
        if( aDescRec->mTableName != NULL )
        {
            stlFreeHeap( aDescRec->mTableName );
            aDescRec->mTableName = NULL;
        }
        
        sSourceLen = stlStrlen( aTableName );

        if( dtlGetMbMaxLength( aSourceCharacterSet ) > dtlGetMbMaxLength( aDestCharacterSet ) )
        {
            sDestLen = sSourceLen;
        }
        else
        {
            sDestLen = sSourceLen * (dtlGetMbMaxLength( aDestCharacterSet ) / dtlGetMbMaxLength( aSourceCharacterSet ));
        }
        
        STL_TRY( stlAllocHeap( (void**)&aDescRec->mTableName,
                               sDestLen + 1,
                               aErrorStack ) == STL_SUCCESS );

        STL_TRY( dtlMbConversion( aSourceCharacterSet,
                                  aDestCharacterSet,
                                  aTableName,
                                  sSourceLen,
                                  aDescRec->mTableName,
                                  sDestLen,
                                  &sOffset,
                                  &sLen,
                                  aErrorStack ) == STL_SUCCESS );

        STL_DASSERT( sLen <= sDestLen );

        aDescRec->mTableName[sLen] = '\0';
    }

    if( aBaseTableName[0] != '\0' )
    {
        if( aDescRec->mBaseTableName != NULL )
        {
            stlFreeHeap( aDescRec->mBaseTableName );
            aDescRec->mBaseTableName = NULL;
        }
        
        sSourceLen = stlStrlen( aBaseTableName );

        if( dtlGetMbMaxLength( aSourceCharacterSet ) > dtlGetMbMaxLength( aDestCharacterSet ) )
        {
            sDestLen = sSourceLen;
        }
        else
        {
            sDestLen = sSourceLen * (dtlGetMbMaxLength( aDestCharacterSet ) / dtlGetMbMaxLength( aSourceCharacterSet ));
        }
        
        STL_TRY( stlAllocHeap( (void**)&aDescRec->mBaseTableName,
                               sDestLen + 1,
                               aErrorStack ) == STL_SUCCESS );

        STL_TRY( dtlMbConversion( aSourceCharacterSet,
                                  aDestCharacterSet,
                                  aBaseTableName,
                                  sSourceLen,
                                  aDescRec->mBaseTableName,
                                  sDestLen,
                                  &sOffset,
                                  &sLen,
                                  aErrorStack ) == STL_SUCCESS );

        STL_DASSERT( sLen <= sDestLen );

        aDescRec->mBaseTableName[sLen] = '\0';
    }

    if( aColumnAliasName[0] != '\0' )
    {
        if( aDescRec->mName != NULL )
        {
            stlFreeHeap( aDescRec->mName );
            aDescRec->mName = NULL;
        }
        
        sSourceLen = stlStrlen( aColumnAliasName );

        if( dtlGetMbMaxLength( aSourceCharacterSet ) > dtlGetMbMaxLength( aDestCharacterSet ) )
        {
            sDestLen = sSourceLen;
        }
        else
        {
            sDestLen = sSourceLen * (dtlGetMbMaxLength( aDestCharacterSet ) / dtlGetMbMaxLength( aSourceCharacterSet ));
        }
        
        STL_TRY( stlAllocHeap( (void**)&aDescRec->mName,
                               sDestLen + 1,
                               aErrorStack ) == STL_SUCCESS );

        STL_TRY( dtlMbConversion( aSourceCharacterSet,
                                  aDestCharacterSet,
                                  aColumnAliasName,
                                  sSourceLen,
                                  aDescRec->mName,
                                  sDestLen,
                                  &sOffset,
                                  &sLen,
                                  aErrorStack ) == STL_SUCCESS );

        STL_DASSERT( sLen <= sDestLen );

        aDescRec->mName[sLen] = '\0';
    }

    if( aColumnLabel[0] != '\0' )
    {
        if( aDescRec->mLabel != NULL )
        {
            stlFreeHeap( aDescRec->mLabel );
            aDescRec->mLabel = NULL;
        }
        
        sSourceLen = stlStrlen( aColumnLabel );

        if( dtlGetMbMaxLength( aSourceCharacterSet ) > dtlGetMbMaxLength( aDestCharacterSet ) )
        {
            sDestLen = sSourceLen;
        }
        else
        {
            sDestLen = sSourceLen * (dtlGetMbMaxLength( aDestCharacterSet ) / dtlGetMbMaxLength( aSourceCharacterSet ));
        }
        
        STL_TRY( stlAllocHeap( (void**)&aDescRec->mLabel,
                               sDestLen + 1,
                               aErrorStack ) == STL_SUCCESS );

        STL_TRY( dtlMbConversion( aSourceCharacterSet,
                                  aDestCharacterSet,
                                  aColumnLabel,
                                  sSourceLen,
                                  aDescRec->mLabel,
                                  sDestLen,
                                  &sOffset,
                                  &sLen,
                                  aErrorStack ) == STL_SUCCESS );

        STL_DASSERT( sLen <= sDestLen );

        aDescRec->mLabel[sLen] = '\0';
    }

    if( aBaseColumnName[0] != '\0' )
    {
        if( aDescRec->mBaseColumnName != NULL )
        {
            stlFreeHeap( aDescRec->mBaseColumnName );
            aDescRec->mBaseColumnName = NULL;
        }
        
        sSourceLen = stlStrlen( aBaseColumnName );

        if( dtlGetMbMaxLength( aSourceCharacterSet ) > dtlGetMbMaxLength( aDestCharacterSet ) )
        {
            sDestLen = sSourceLen;
        }
        else
        {
            sDestLen = sSourceLen * (dtlGetMbMaxLength( aDestCharacterSet ) / dtlGetMbMaxLength( aSourceCharacterSet ));
        }
        
        STL_TRY( stlAllocHeap( (void**)&aDescRec->mBaseColumnName,
                               sDestLen + 1,
                               aErrorStack ) == STL_SUCCESS );

        STL_TRY( dtlMbConversion( aSourceCharacterSet,
                                  aDestCharacterSet,
                                  aBaseColumnName,
                                  sSourceLen,
                                  aDescRec->mBaseColumnName,
                                  sDestLen,
                                  &sOffset,
                                  &sLen,
                                  aErrorStack ) == STL_SUCCESS );

        STL_DASSERT( sLen <= sDestLen );

        aDescRec->mBaseColumnName[sLen] = '\0';
    }

    /*
     * Target Type 의 문자 관련 Type 정보 및 출력 공간
     */
    switch( aDescRec->mConsiceType )
    {
        case SQL_CHAR :
        case SQL_VARCHAR :
        case SQL_LONGVARCHAR :
            aDescRec->mLength = aCharacterLength;
            
            if( aCharacterLength == aOctetLength )
            {
                aDescRec->mOctetLength = aOctetLength;

                if( aDisplaySize == DTL_NO_TOTAL_SIZE )
                {
                    aDescRec->mDisplaySize = SQL_NO_TOTAL;
                }
                else
                {
                    aDescRec->mDisplaySize = aDisplaySize;
                }
            }
            else
            {
                aDescRec->mOctetLength = aOctetLength * dtlGetMbMaxLength( aDestCharacterSet ) / dtlGetMbMaxLength( aSourceCharacterSet );

                if( aDisplaySize == DTL_NO_TOTAL_SIZE )
                {
                    aDescRec->mDisplaySize = SQL_NO_TOTAL;
                }
                else
                {
                    aDescRec->mDisplaySize = aDisplaySize * dtlGetMbMaxLength( aDestCharacterSet ) / dtlGetMbMaxLength( aSourceCharacterSet );
                }
            }
            break;
        case SQL_BINARY :
        case SQL_VARBINARY :
        case SQL_LONGVARBINARY :
            aDescRec->mLength = aOctetLength;
            aDescRec->mOctetLength = aOctetLength;

            if( aDisplaySize == DTL_NO_TOTAL_SIZE )
            {
                aDescRec->mDisplaySize = SQL_NO_TOTAL;
            }
            else
            {
                aDescRec->mDisplaySize = aDisplaySize;
            }
            break;
        case SQL_BIT :
            aDescRec->mLength = 1;
            aDescRec->mOctetLength = aOctetLength;
            aDescRec->mDisplaySize = aDisplaySize;
            break;
        case SQL_BOOLEAN :
            aDescRec->mLength = 5;
            aDescRec->mOctetLength = aOctetLength;
            aDescRec->mDisplaySize = aDisplaySize;
            break;
        case SQL_TYPE_DATE :
            aDescRec->mLength = 
                ((ZLS_STMT_NLS_DT_VECTOR(aStmt))->mGetDateFormatInfoFunc(aStmt))->mToCharMaxResultLen; 
            aDescRec->mOctetLength = aOctetLength; 
            aDescRec->mDisplaySize = aDescRec->mLength; 
        case SQL_TYPE_TIME :
            aDescRec->mLength = 
                ((ZLS_STMT_NLS_DT_VECTOR(aStmt))->mGetTimeFormatInfoFunc(aStmt))->mToCharMaxResultLen; 
            aDescRec->mOctetLength = aOctetLength; 
            aDescRec->mDisplaySize = aDescRec->mLength; 
            break;  
        case SQL_TYPE_TIME_WITH_TIMEZONE :
            aDescRec->mLength = 
                ((ZLS_STMT_NLS_DT_VECTOR(aStmt))->mGetTimeWithTimeZoneFormatInfoFunc(aStmt))->mToCharMaxResultLen; 
            aDescRec->mOctetLength = aOctetLength; 
            aDescRec->mDisplaySize = aDescRec->mLength; 
            break;  
        case SQL_TYPE_TIMESTAMP :
            aDescRec->mLength = 
                ((ZLS_STMT_NLS_DT_VECTOR(aStmt))->mGetTimestampFormatInfoFunc(aStmt))->mToCharMaxResultLen; 
            aDescRec->mOctetLength = aOctetLength; 
            aDescRec->mDisplaySize = aDescRec->mLength; 
            break;  
        case SQL_TYPE_TIMESTAMP_WITH_TIMEZONE :
            aDescRec->mLength = 
                ((ZLS_STMT_NLS_DT_VECTOR(aStmt))->mGetTimestampWithTimeZoneFormatInfoFunc(aStmt))->mToCharMaxResultLen; 
            aDescRec->mOctetLength = aOctetLength; 
            aDescRec->mDisplaySize = aDescRec->mLength; 
            break;  
        case SQL_INTERVAL_SECOND :
        case SQL_INTERVAL_DAY_TO_SECOND :
        case SQL_INTERVAL_HOUR_TO_SECOND :
        case SQL_INTERVAL_MINUTE_TO_SECOND :
        case SQL_INTERVAL_YEAR :
        case SQL_INTERVAL_MONTH :
        case SQL_INTERVAL_DAY :
        case SQL_INTERVAL_HOUR :
        case SQL_INTERVAL_MINUTE :
        case SQL_INTERVAL_YEAR_TO_MONTH :
        case SQL_INTERVAL_DAY_TO_HOUR :
        case SQL_INTERVAL_DAY_TO_MINUTE :
        case SQL_INTERVAL_HOUR_TO_MINUTE :
            aDescRec->mLength = aDisplaySize;
            aDescRec->mOctetLength = aOctetLength;
            aDescRec->mDisplaySize = aDisplaySize;
            break;
        case SQL_ROWID:
            aDescRec->mLength = aDisplaySize;
            aDescRec->mOctetLength = aOctetLength;
            aDescRec->mDisplaySize = aDisplaySize;
            break;            
        case SQL_GUID :
            aDescRec->mLength = 36;
            aDescRec->mOctetLength = aOctetLength;
            aDescRec->mDisplaySize = aDisplaySize;
            break;
        default :
            aDescRec->mLength = 0;
            aDescRec->mOctetLength = aOctetLength;
            aDescRec->mDisplaySize = aDisplaySize;
            break;
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zlcdDescBuildResult( zlcDbc        * aDbc,
                               zlsStmt       * aStmt,
                               stlBool         aNeedResultCols,
                               stlErrorStack * aErrorStack )
{
    cmlHandle           * sHandle;
    zldDesc             * sIrd;
    zldDescElement      * sIrdRec = NULL;
    stlChar               sCatalogName[STL_MAX_SQL_IDENTIFIER_LENGTH + 1];
    stlChar               sSchemaName[STL_MAX_SQL_IDENTIFIER_LENGTH + 1];
    stlChar               sTableName[STL_MAX_SQL_IDENTIFIER_LENGTH + 1];
    stlChar               sBaseTableName[STL_MAX_SQL_IDENTIFIER_LENGTH + 1];
    stlChar               sColumnAliasName[STL_MAX_SQL_IDENTIFIER_LENGTH + 1];
    stlChar               sColumnLabel[STL_MAX_SQL_IDENTIFIER_LENGTH + 1];
    stlChar               sBaseColumnName[STL_MAX_SQL_IDENTIFIER_LENGTH + 1];
    stlChar               sLiteralPrefix[STL_MAX_SQL_IDENTIFIER_LENGTH + 1];
    stlChar               sLiteralSuffix[STL_MAX_SQL_IDENTIFIER_LENGTH + 1];
    dtlDataType           sDbType;
    stlInt16              sIntervalCode;
    stlInt16              sNumPrecRadix;
    stlInt16              sDatetimeIntervPrec;
    stlInt64              sDisplaySize;
    dtlStringLengthUnit   sStringLengthUnit;
    stlInt64              sCharacterLength;
    stlInt64              sOctetLength;
    stlInt32              sPrecision;
    stlInt32              sScale;
    stlBool               sNullable;
    stlBool               sAliasUnnamed;
    stlBool               sCaseSensitive;
    stlBool               sUnsigned;
    stlBool               sFixedPrecScale;
    stlBool               sUpdatable;
    stlBool               sAutoUnique;
    stlBool               sRowVersion;
    stlBool               sAbleLike;
    stlBool               sIgnored;
    stlInt8               sControl;
    stlBool               sNeedRecvResult = STL_FALSE;
    stlInt32              i;

    dtlCharacterSet      sSourceCharacterSet = DTL_CHARACTERSET_MAX;
    dtlCharacterSet      sDestCharacterSet   = DTL_CHARACTERSET_MAX;
    zlcdConvertIRDFunc   sConvertIRDFunc;

    /**
     * @todo 컬럼 갯수가 변경된 것은 반영되지 않음
     */
    
    stlMemset( sCatalogName,     0x00, STL_MAX_SQL_IDENTIFIER_LENGTH + 1 );
    stlMemset( sSchemaName,      0x00, STL_MAX_SQL_IDENTIFIER_LENGTH + 1 );
    stlMemset( sTableName,       0x00, STL_MAX_SQL_IDENTIFIER_LENGTH + 1 );
    stlMemset( sBaseTableName,   0x00, STL_MAX_SQL_IDENTIFIER_LENGTH + 1 );
    stlMemset( sColumnAliasName, 0x00, STL_MAX_SQL_IDENTIFIER_LENGTH + 1 );
    stlMemset( sColumnLabel,     0x00, STL_MAX_SQL_IDENTIFIER_LENGTH + 1 );
    stlMemset( sBaseColumnName,  0x00, STL_MAX_SQL_IDENTIFIER_LENGTH + 1 );
    stlMemset( sLiteralPrefix,   0x00, STL_MAX_SQL_IDENTIFIER_LENGTH + 1 );
    stlMemset( sLiteralSuffix,   0x00, STL_MAX_SQL_IDENTIFIER_LENGTH + 1 );

    sHandle = &aDbc->mComm;
    
    sIrd = &aStmt->mIrd;

    if( aDbc->mCharacterSet == aDbc->mNlsCharacterSet )
    {
        sConvertIRDFunc = zlcdNonConvertIRD;
    }
    else
    {
        sSourceCharacterSet = (ZLS_STMT_NLS_DT_VECTOR(aStmt))->mGetCharSetIDFunc(aStmt);
        sDestCharacterSet   = (ZLS_STMT_DT_VECTOR(aStmt))->mGetCharSetIDFunc(aStmt);
        
        sConvertIRDFunc = zlcdCovnertIRD;
    }

    for( i = 1; i <= sIrd->mCount; i++ )
    {
        sControl = CML_CONTROL_NONE;
        
        if( i == 1 )
        {
            sControl |= CML_CONTROL_HEAD;
        }

        if( i == sIrd->mCount )
        {
            sControl |= CML_CONTROL_TAIL;
        }
        
        STL_TRY( cmlWriteDescribeColCommand( sHandle,
                                             sControl,
                                             aStmt->mStmtId,
                                             i,
                                             aErrorStack ) == STL_SUCCESS );
    }

    STL_TRY( cmlSendPacket( sHandle,
                            aErrorStack ) == STL_SUCCESS );

    sNeedRecvResult = STL_TRUE;
    
    for( i = 1, sIrdRec = STL_RING_GET_FIRST_DATA(&sIrd->mDescRecRing);
         i <= sIrd->mCount;
         i++, sIrdRec = STL_RING_GET_NEXT_DATA(&sIrd->mDescRecRing, sIrdRec) )
    {
        STL_TRY( cmlReadDescribeColResult( sHandle,
                                           &sIgnored,
                                           sCatalogName,
                                           sSchemaName,
                                           sTableName,
                                           sBaseTableName,
                                           sColumnAliasName,
                                           sColumnLabel,
                                           sBaseColumnName,
                                           sLiteralPrefix,
                                           sLiteralSuffix,
                                           &sDbType,
                                           &sIntervalCode,
                                           &sNumPrecRadix,
                                           &sDatetimeIntervPrec,
                                           &sDisplaySize,
                                           &sStringLengthUnit,
                                           &sCharacterLength,
                                           &sOctetLength,
                                           &sPrecision,
                                           &sScale,
                                           &sNullable,
                                           &sAliasUnnamed,
                                           &sCaseSensitive,
                                           &sUnsigned,
                                           &sFixedPrecScale,
                                           &sUpdatable,
                                           &sAutoUnique,
                                           &sRowVersion,
                                           &sAbleLike,
                                           aErrorStack ) == STL_SUCCESS );

        /*
         * Target 의 Type 정보
         */

        STL_TRY( zldDescGetSqlType( sDbType,
                                    sScale,
                                    sIntervalCode,
                                    &sIrdRec->mType,
                                    &sIrdRec->mConsiceType,
                                    &sIrdRec->mDatetimeIntervalCode,
                                    aErrorStack )
                 == STL_SUCCESS );


        STL_TRY( zlcdDescGetConsiceTypeName( sDbType,
                                             sIrdRec->mConsiceType,
                                             & sIrdRec->mTypeName,
                                             aErrorStack )
                 == STL_SUCCESS );
        
        STL_TRY( zlcdDescGetConsiceTypeName( sDbType,
                                             sIrdRec->mConsiceType,
                                             & sIrdRec->mLocalTypeName,
                                             aErrorStack )
                 == STL_SUCCESS );

        sIrdRec->mDatetimeIntervalPrecision = sDatetimeIntervPrec;

        STL_TRY( sConvertIRDFunc( sIrdRec,
                                  sCatalogName,
                                  sSchemaName,
                                  sTableName,
                                  sBaseTableName,
                                  sColumnAliasName,
                                  sColumnLabel,
                                  sBaseColumnName,
                                  sCharacterLength,
                                  sOctetLength,
                                  sDisplaySize,
                                  sSourceCharacterSet,
                                  sDestCharacterSet,
                                  aStmt, 
                                  aErrorStack ) == STL_SUCCESS );

        /*
         * Target Name 관련 정보
         */

        if( sAliasUnnamed == STL_TRUE )
        {
            sIrdRec->mUnnamed = SQL_UNNAMED;
        }
        else
        {
            sIrdRec->mUnnamed = SQL_NAMED;
        }

        /*
         * Target 의 관계모델 속성
         */

        if( sNullable == STL_TRUE )
        {
            sIrdRec->mNullable = SQL_NULLABLE;
        }
        else
        {
            sIrdRec->mNullable = SQL_NO_NULLS;
        }

        if( sUpdatable == STL_TRUE )
        {
            sIrdRec->mUpdatable = SQL_ATTR_WRITE;
        }
        else
        {
            sIrdRec->mUpdatable = SQL_ATTR_READONLY;
        }

        if( sAutoUnique == STL_TRUE )
        {
            sIrdRec->mAutoUniqueValue = SQL_TRUE;
        }
        else
        {
            sIrdRec->mAutoUniqueValue = SQL_FALSE;
        }

        if( sRowVersion == STL_TRUE )
        {
            sIrdRec->mRowver = SQL_TRUE;
        }
        else
        {
            sIrdRec->mRowver = SQL_FALSE;
        }

        if( sAbleLike == STL_TRUE )
        {
            sIrdRec->mSearchable = SQL_PRED_SEARCHABLE;
        }
        else
        {
            sIrdRec->mSearchable = SQL_PRED_BASIC;
        }

        /*
         * Target Type 의 숫자 관련 정보
         */

        sIrdRec->mNumPrecRadix = sNumPrecRadix;

        if( sUnsigned == STL_TRUE )
        {
            sIrdRec->mUnsigned = SQL_TRUE;
        }
        else
        {
            sIrdRec->mUnsigned = SQL_FALSE;
        }

        if( sFixedPrecScale == STL_TRUE )
        {
            sIrdRec->mFixedPrecScale = SQL_TRUE;
        }
        else
        {
            sIrdRec->mFixedPrecScale = SQL_FALSE;
        }

        switch( sIrdRec->mConsiceType )
        {
            case SQL_TINYINT :
                sIrdRec->mPrecision = 1;
                break;
            case SQL_SMALLINT :
                sIrdRec->mPrecision = 5;
                break;
            case SQL_INTEGER :
                sIrdRec->mPrecision = 10;
                break;
            case SQL_BIGINT :
                sIrdRec->mPrecision = 19;
                break;
            case SQL_REAL :
                sIrdRec->mPrecision = 7;
                break;
            case SQL_DOUBLE :
                sIrdRec->mPrecision = 15;
                break;
            case SQL_FLOAT :
                if( sDbType == DTL_TYPE_NUMBER )
                {
                    sIrdRec->mPrecision = DTL_FLOAT_MAX_PRECISION;
                    sIrdRec->mNumPrecRadix = 2;
                }
                else
                {
                    sIrdRec->mPrecision = sPrecision;
                }
                break;
            default :
                sIrdRec->mPrecision = sPrecision;
                break;
        }
    
        sIrdRec->mScale = sScale;
    
        /*
         * Target Type 의 문자 관련 Type 정보
         */

        sIrdRec->mStringLengthUnit = sStringLengthUnit;

        if( sCaseSensitive == STL_TRUE )
        {
            sIrdRec->mCaseSensitive = SQL_TRUE;
        }
        else
        {
            sIrdRec->mCaseSensitive = SQL_FALSE;
        }

        if( sLiteralPrefix[0] != '\0' )
        {
            if( sIrdRec->mLiteralPrefix != NULL )
            {
                stlFreeHeap( sIrdRec->mLiteralPrefix );
                sIrdRec->mLiteralPrefix = NULL;
            }
        
            STL_TRY( stlAllocHeap( (void**)&sIrdRec->mLiteralPrefix,
                                   stlStrlen( sLiteralPrefix ) + 1,
                                   aErrorStack ) == STL_SUCCESS );
        
            stlStrcpy( sIrdRec->mLiteralPrefix, sLiteralPrefix );
        }

        if( sLiteralSuffix[0] != '\0' )
        {
            if( sIrdRec->mLiteralSuffix != NULL )
            {
                stlFreeHeap( sIrdRec->mLiteralSuffix );
                sIrdRec->mLiteralSuffix = NULL;
            }
        
            STL_TRY( stlAllocHeap( (void**)&sIrdRec->mLiteralSuffix,
                                   stlStrlen( sLiteralSuffix ) + 1,
                                   aErrorStack ) == STL_SUCCESS );
        
            stlStrcpy( sIrdRec->mLiteralSuffix, sLiteralSuffix );
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    if( sNeedRecvResult == STL_TRUE )
    {
        for( i = i + 1; i <= sIrd->mCount; i++ )
        {
            (void)cmlReadDescribeColResult( sHandle,
                                            &sIgnored,
                                            sCatalogName,
                                            sSchemaName,
                                            sTableName,
                                            sBaseTableName,
                                            sColumnAliasName,
                                            sColumnLabel,
                                            sBaseColumnName,
                                            sLiteralPrefix,
                                            sLiteralSuffix,
                                            &sDbType,
                                            &sIntervalCode,
                                            &sNumPrecRadix,
                                            &sDatetimeIntervPrec,
                                            &sDisplaySize,
                                            &sStringLengthUnit,
                                            &sCharacterLength,
                                            &sOctetLength,
                                            &sPrecision,
                                            &sScale,
                                            &sNullable,
                                            &sAliasUnnamed,
                                            &sCaseSensitive,
                                            &sUnsigned,
                                            &sFixedPrecScale,
                                            &sUpdatable,
                                            &sAutoUnique,
                                            &sRowVersion,
                                            &sAbleLike,
                                            aErrorStack );
        }
    }
    
    return STL_FAILURE;
}

/** @} */
