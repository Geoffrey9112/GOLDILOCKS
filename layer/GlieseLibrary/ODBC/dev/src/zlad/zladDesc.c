/*******************************************************************************
 * zladDesc.c
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

#include <goldilocks.h>
#include <cml.h>
#include <ssl.h>
#include <zlDef.h>
#include <zld.h>

#include <zlai.h>

/**
 * @file zladDesc.c
 * @brief ODBC API Internal Desc Routines.
 */

/**
 * @addtogroup zld
 * @{
 */

typedef stlStatus (*zladConvertIRDFunc) ( zldDescElement  * aDescRec,
                                          qllTarget       * aTarget,
                                          dtlCharacterSet   aSourceCharacterSet,
                                          dtlCharacterSet   aDestCharacterSet,
                                          zlsStmt         * aStmt, 
                                          stlErrorStack   * aErrorStack );


static stlStatus zladNonConvertIRD( zldDescElement  * aDescRec,
                                    qllTarget       * aTarget,
                                    dtlCharacterSet   aSourceCharacterSet,
                                    dtlCharacterSet   aDestCharacterSet,
                                    zlsStmt         * aStmt, 
                                    stlErrorStack   * aErrorStack )
{
    /*
     * Target Name 관련 정보
     */

    if( aTarget->mCatalogName != NULL )
    {
        if( aDescRec->mCatalogName != NULL )
        {
            stlFreeHeap( aDescRec->mCatalogName );
            aDescRec->mCatalogName = NULL;
        }
        
        STL_TRY( stlAllocHeap( (void**)&aDescRec->mCatalogName,
                               stlStrlen( aTarget->mCatalogName ) + 1,
                               aErrorStack ) == STL_SUCCESS );
        
        stlStrcpy( aDescRec->mCatalogName, aTarget->mCatalogName );
    }

    if( aTarget->mSchemaName != NULL )
    {
        if( aDescRec->mSchemaName != NULL )
        {
            stlFreeHeap( aDescRec->mSchemaName );
            aDescRec->mSchemaName = NULL;
        }
        
        STL_TRY( stlAllocHeap( (void**)&aDescRec->mSchemaName,
                               stlStrlen( aTarget->mSchemaName ) + 1,
                               aErrorStack ) == STL_SUCCESS );
        
        stlStrcpy( aDescRec->mSchemaName, aTarget->mSchemaName );
    }

    if( aTarget->mTableName != NULL )
    {
        if( aDescRec->mTableName != NULL )
        {
            stlFreeHeap( aDescRec->mTableName );
            aDescRec->mTableName = NULL;
        }
        
        STL_TRY( stlAllocHeap( (void**)&aDescRec->mTableName,
                               stlStrlen( aTarget->mTableName ) + 1,
                               aErrorStack ) == STL_SUCCESS );
        
        stlStrcpy( aDescRec->mTableName, aTarget->mTableName );
    }

    if( aTarget->mBaseTableName != NULL )
    {
        if( aDescRec->mBaseTableName != NULL )
        {
            stlFreeHeap( aDescRec->mBaseTableName );
            aDescRec->mBaseTableName = NULL;
        }
        
        STL_TRY( stlAllocHeap( (void**)&aDescRec->mBaseTableName,
                               stlStrlen( aTarget->mBaseTableName ) + 1,
                               aErrorStack ) == STL_SUCCESS );
        
        stlStrcpy( aDescRec->mBaseTableName, aTarget->mBaseTableName );
    }

    if( aTarget->mColumnAlias != NULL )
    {
        if( aDescRec->mName != NULL )
        {
            stlFreeHeap( aDescRec->mName );
            aDescRec->mName = NULL;
        }
        
        STL_TRY( stlAllocHeap( (void**)&aDescRec->mName,
                               stlStrlen( aTarget->mColumnAlias ) + 1,
                               aErrorStack ) == STL_SUCCESS );
        
        stlStrcpy( aDescRec->mName, aTarget->mColumnAlias );
    }

    if( aTarget->mColumnLabel != NULL )
    {
        if( aDescRec->mLabel != NULL )
        {
            stlFreeHeap( aDescRec->mLabel );
            aDescRec->mLabel = NULL;
        }
        
        STL_TRY( stlAllocHeap( (void**)&aDescRec->mLabel,
                               stlStrlen( aTarget->mColumnLabel ) + 1,
                               aErrorStack ) == STL_SUCCESS );
        
        stlStrcpy( aDescRec->mLabel, aTarget->mColumnLabel );
    }

    if( aTarget->mBaseColumnName != NULL )
    {
        if( aDescRec->mBaseColumnName != NULL )
        {
            stlFreeHeap( aDescRec->mBaseColumnName );
            aDescRec->mBaseColumnName = NULL;
        }
        
        STL_TRY( stlAllocHeap( (void**)&aDescRec->mBaseColumnName,
                               stlStrlen( aTarget->mBaseColumnName ) + 1,
                               aErrorStack ) == STL_SUCCESS );
        
        stlStrcpy( aDescRec->mBaseColumnName, aTarget->mBaseColumnName );
    }

    /*
     * Target Type 의 문자 관련 Type 정보
     */
    
    switch( aDescRec->mConsiceType )
    {
        case SQL_CHAR :
        case SQL_VARCHAR :
        case SQL_LONGVARCHAR :
            aDescRec->mLength = aTarget->mCharacterLength;
            break;
        case SQL_BINARY :
        case SQL_VARBINARY :
        case SQL_LONGVARBINARY :
            aDescRec->mLength = aTarget->mOctetLength;
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
            aDescRec->mLength = aTarget->mDisplaySize;
            break;
        case SQL_ROWID:
            aDescRec->mLength = aTarget->mDisplaySize;
            break;            
        case SQL_GUID :
            aDescRec->mLength = 36;
            break;
        default :
            aDescRec->mLength = 0;
            break;
    }

    aDescRec->mOctetLength = aTarget->mOctetLength;
    
    /*
     * Target 의 출력 공간
     */

    if( aTarget->mDisplaySize == DTL_NO_TOTAL_SIZE )
    {
        aDescRec->mDisplaySize = SQL_NO_TOTAL;
    }
    else
    {
        aDescRec->mDisplaySize = aTarget->mDisplaySize;

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

static stlStatus zladCovnertIRD( zldDescElement  * aDescRec,
                                 qllTarget       * aTarget,
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

    if( aTarget->mCatalogName != NULL )
    {
        if( aDescRec->mCatalogName != NULL )
        {
            stlFreeHeap( aDescRec->mCatalogName );
            aDescRec->mCatalogName = NULL;
        }

        sSourceLen = stlStrlen( aTarget->mCatalogName );

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
                                  aTarget->mCatalogName,
                                  sSourceLen,
                                  aDescRec->mCatalogName,
                                  sDestLen,
                                  &sOffset,
                                  &sLen,
                                  aErrorStack ) == STL_SUCCESS );

        STL_DASSERT( sLen <= sDestLen );

        aDescRec->mCatalogName[sLen] = '\0';
    }

    if( aTarget->mSchemaName != NULL )
    {
        if( aDescRec->mSchemaName != NULL )
        {
            stlFreeHeap( aDescRec->mSchemaName );
            aDescRec->mSchemaName = NULL;
        }
        
        sSourceLen = stlStrlen( aTarget->mSchemaName );

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
                                  aTarget->mSchemaName,
                                  sSourceLen,
                                  aDescRec->mSchemaName,
                                  sDestLen,
                                  &sOffset,
                                  &sLen,
                                  aErrorStack ) == STL_SUCCESS );

        STL_DASSERT( sLen <= sDestLen );

        aDescRec->mSchemaName[sLen] = '\0';
    }

    if( aTarget->mTableName != NULL )
    {
        if( aDescRec->mTableName != NULL )
        {
            stlFreeHeap( aDescRec->mTableName );
            aDescRec->mTableName = NULL;
        }
        
        sSourceLen = stlStrlen( aTarget->mTableName );

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
                                  aTarget->mTableName,
                                  sSourceLen,
                                  aDescRec->mTableName,
                                  sDestLen,
                                  &sOffset,
                                  &sLen,
                                  aErrorStack ) == STL_SUCCESS );

        STL_DASSERT( sLen <= sDestLen );

        aDescRec->mTableName[sLen] = '\0';
    }

    if( aTarget->mBaseTableName != NULL )
    {
        if( aDescRec->mBaseTableName != NULL )
        {
            stlFreeHeap( aDescRec->mBaseTableName );
            aDescRec->mBaseTableName = NULL;
        }
        
        sSourceLen = stlStrlen( aTarget->mBaseTableName );

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
                                  aTarget->mBaseTableName,
                                  sSourceLen,
                                  aDescRec->mBaseTableName,
                                  sDestLen,
                                  &sOffset,
                                  &sLen,
                                  aErrorStack ) == STL_SUCCESS );

        STL_DASSERT( sLen <= sDestLen );

        aDescRec->mBaseTableName[sLen] = '\0';
    }

    if( aTarget->mColumnAlias != NULL )
    {
        if( aDescRec->mName != NULL )
        {
            stlFreeHeap( aDescRec->mName );
            aDescRec->mName = NULL;
        }
        
        sSourceLen = stlStrlen( aTarget->mColumnAlias );

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
                                  aTarget->mColumnAlias,
                                  sSourceLen,
                                  aDescRec->mName,
                                  sDestLen,
                                  &sOffset,
                                  &sLen,
                                  aErrorStack ) == STL_SUCCESS );

        STL_DASSERT( sLen <= sDestLen );

        aDescRec->mName[sLen] = '\0';
    }

    if( aTarget->mColumnLabel != NULL )
    {
        if( aDescRec->mLabel != NULL )
        {
            stlFreeHeap( aDescRec->mLabel );
            aDescRec->mLabel = NULL;
        }
        
        sSourceLen = stlStrlen( aTarget->mColumnLabel );

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
                                  aTarget->mColumnLabel,
                                  sSourceLen,
                                  aDescRec->mLabel,
                                  sDestLen,
                                  &sOffset,
                                  &sLen,
                                  aErrorStack ) == STL_SUCCESS );

        STL_DASSERT( sLen <= sDestLen );

        aDescRec->mLabel[sLen] = '\0';
    }

    if( aTarget->mBaseColumnName != NULL )
    {
        if( aDescRec->mBaseColumnName != NULL )
        {
            stlFreeHeap( aDescRec->mBaseColumnName );
            aDescRec->mBaseColumnName = NULL;
        }
        
        sSourceLen = stlStrlen( aTarget->mBaseColumnName );

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
                                  aTarget->mBaseColumnName,
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
            aDescRec->mLength = aTarget->mCharacterLength;
            
            if( aTarget->mCharacterLength == aTarget->mOctetLength )
            {
                aDescRec->mOctetLength = aTarget->mOctetLength;

                if( aTarget->mDisplaySize == DTL_NO_TOTAL_SIZE )
                {
                    aDescRec->mDisplaySize = SQL_NO_TOTAL;
                }
                else
                {
                    aDescRec->mDisplaySize = aTarget->mDisplaySize;
                }
            }
            else
            {
                aDescRec->mOctetLength = aTarget->mOctetLength * dtlGetMbMaxLength( aDestCharacterSet ) / dtlGetMbMaxLength( aSourceCharacterSet );

                if( aTarget->mDisplaySize == DTL_NO_TOTAL_SIZE )
                {
                    aDescRec->mDisplaySize = SQL_NO_TOTAL;
                }
                else
                {
                    aDescRec->mDisplaySize = aTarget->mDisplaySize * dtlGetMbMaxLength( aDestCharacterSet ) / dtlGetMbMaxLength( aSourceCharacterSet );
                }
            }
            break;
        case SQL_BINARY :
        case SQL_VARBINARY :
        case SQL_LONGVARBINARY :
            aDescRec->mLength = aTarget->mOctetLength;
            aDescRec->mOctetLength = aTarget->mOctetLength;

            if( aTarget->mDisplaySize == DTL_NO_TOTAL_SIZE )
            {
                aDescRec->mDisplaySize = SQL_NO_TOTAL;
            }
            else
            {
                aDescRec->mDisplaySize = aTarget->mDisplaySize;
            }
            break;
        case SQL_BIT :
            aDescRec->mLength = 1;
            aDescRec->mOctetLength = aTarget->mOctetLength;
            aDescRec->mDisplaySize = aTarget->mDisplaySize;
            break;
        case SQL_BOOLEAN :
            aDescRec->mLength = 5;
            aDescRec->mOctetLength = aTarget->mOctetLength;
            aDescRec->mDisplaySize = aTarget->mDisplaySize;
            break;
        case SQL_TYPE_DATE :
            aDescRec->mLength = 
                ((ZLS_STMT_NLS_DT_VECTOR(aStmt))->mGetDateFormatInfoFunc(aStmt))->mToCharMaxResultLen; 
            aDescRec->mOctetLength = aTarget->mOctetLength; 
            aDescRec->mDisplaySize = aDescRec->mLength; 
            break;          
        case SQL_TYPE_TIME :
            aDescRec->mLength = 
                ((ZLS_STMT_NLS_DT_VECTOR(aStmt))->mGetTimeFormatInfoFunc(aStmt))->mToCharMaxResultLen; 
            aDescRec->mOctetLength = aTarget->mOctetLength; 
            aDescRec->mDisplaySize = aDescRec->mLength; 
            break; 
        case SQL_TYPE_TIME_WITH_TIMEZONE :
            aDescRec->mLength = 
                ((ZLS_STMT_NLS_DT_VECTOR(aStmt))->mGetTimeWithTimeZoneFormatInfoFunc(aStmt))->mToCharMaxResultLen; 
            aDescRec->mOctetLength = aTarget->mOctetLength; 
            aDescRec->mDisplaySize = aDescRec->mLength; 
            break;  
        case SQL_TYPE_TIMESTAMP :
            aDescRec->mLength = 
                ((ZLS_STMT_NLS_DT_VECTOR(aStmt))->mGetTimestampFormatInfoFunc(aStmt))->mToCharMaxResultLen; 
            aDescRec->mOctetLength = aTarget->mOctetLength; 
            aDescRec->mDisplaySize = aDescRec->mLength; 
            break;             
        case SQL_TYPE_TIMESTAMP_WITH_TIMEZONE :
            aDescRec->mLength = 
                ((ZLS_STMT_NLS_DT_VECTOR(aStmt))->mGetTimestampWithTimeZoneFormatInfoFunc(aStmt))->mToCharMaxResultLen; 
            aDescRec->mOctetLength = aTarget->mOctetLength; 
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
            aDescRec->mLength = aTarget->mDisplaySize;
            aDescRec->mOctetLength = aTarget->mOctetLength;
            aDescRec->mDisplaySize = aTarget->mDisplaySize;
            break;
        case SQL_ROWID:
            aDescRec->mLength = aTarget->mDisplaySize;
            aDescRec->mOctetLength = aTarget->mOctetLength;
            aDescRec->mDisplaySize = aTarget->mDisplaySize;
            break;            
        case SQL_GUID :
            aDescRec->mLength = 36;
            aDescRec->mOctetLength = aTarget->mOctetLength;
            aDescRec->mDisplaySize = aTarget->mDisplaySize;
            break;
        default :
            aDescRec->mLength = 0;
            aDescRec->mOctetLength = aTarget->mOctetLength;
            aDescRec->mDisplaySize = aTarget->mDisplaySize;
            break;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

static stlStatus zladDescSetIRD( zldDescElement     * aDescRec,
                                 qllTarget          * aTarget,
                                 dtlCharacterSet      aSourceCharacterSet,
                                 dtlCharacterSet      aDestCharacterSet,
                                 zladConvertIRDFunc   aConvertIRDFunc,
                                 zlsStmt            * aStmt, 
                                 stlErrorStack      * aErrorStack )
{
    /*
     * Target 의 Type 정보
     */

    STL_TRY( zldDescGetSqlType( aTarget->mDBType,
                                aTarget->mScale,
                                aTarget->mIntervalCode,
                                &aDescRec->mType,
                                &aDescRec->mConsiceType,
                                &aDescRec->mDatetimeIntervalCode,
                                aErrorStack )
             == STL_SUCCESS );

    if( aTarget->mTypeName != NULL )
    {
        if( aDescRec->mTypeName != NULL )
        {
            stlFreeHeap( aDescRec->mTypeName );
            aDescRec->mTypeName = NULL;
        }
        
        STL_TRY( stlAllocHeap( (void**)&aDescRec->mTypeName,
                               stlStrlen( aTarget->mTypeName ) + 1,
                               aErrorStack ) == STL_SUCCESS );

        stlStrcpy( aDescRec->mTypeName, aTarget->mTypeName );
    }

    if( aTarget->mLocalTypeName != NULL )
    {
        if( aDescRec->mLocalTypeName != NULL )
        {
            stlFreeHeap( aDescRec->mLocalTypeName );
            aDescRec->mLocalTypeName = NULL;
        }
        
        STL_TRY( stlAllocHeap( (void**)&aDescRec->mLocalTypeName,
                               stlStrlen( aTarget->mLocalTypeName ) + 1,
                               aErrorStack ) == STL_SUCCESS );
        
        stlStrcpy( aDescRec->mLocalTypeName, aTarget->mLocalTypeName );
    }

    aDescRec->mDatetimeIntervalPrecision = aTarget->mDateTimeIntervalPrec;

    STL_TRY( aConvertIRDFunc( aDescRec,
                              aTarget,
                              aSourceCharacterSet,
                              aDestCharacterSet,
                              aStmt,
                              aErrorStack ) == STL_SUCCESS );

    if( aTarget->mIsAliasUnnamed == STL_TRUE )
    {
        aDescRec->mUnnamed = SQL_UNNAMED;
    }
    else
    {
        aDescRec->mUnnamed = SQL_NAMED;
    }

    /*
     * Target 의 관계모델 속성
     */

    if( aTarget->mIsNullable == STL_TRUE )
    {
        aDescRec->mNullable = SQL_NULLABLE;
    }
    else
    {
        aDescRec->mNullable = SQL_NO_NULLS;
    }

    if( aTarget->mIsUpdatable == STL_TRUE )
    {
        aDescRec->mUpdatable = SQL_ATTR_WRITE;
    }
    else
    {
        aDescRec->mUpdatable = SQL_ATTR_READONLY;
    }

    if( aTarget->mIsAutoUnique == STL_TRUE )
    {
        aDescRec->mAutoUniqueValue = SQL_TRUE;
    }
    else
    {
        aDescRec->mAutoUniqueValue = SQL_FALSE;
    }

    if( aTarget->mIsRowVersion == STL_TRUE )
    {
        aDescRec->mRowver = SQL_TRUE;
    }
    else
    {
        aDescRec->mRowver = SQL_FALSE;
    }

    if( aTarget->mIsAbleLike == STL_TRUE )
    {
        aDescRec->mSearchable = SQL_PRED_SEARCHABLE;
    }
    else
    {
        aDescRec->mSearchable = SQL_PRED_BASIC;
    }

    /*
     * Target Type 의 숫자 관련 정보
     */

    aDescRec->mNumPrecRadix = aTarget->mNumPrecRadix;

    if( aTarget->mUnsigned == STL_TRUE )
    {
        aDescRec->mUnsigned = STL_TRUE;
    }
    else
    {
        aDescRec->mUnsigned = STL_FALSE;
    }

    if( aTarget->mIsExactNumeric == STL_TRUE )
    {
        aDescRec->mFixedPrecScale = STL_TRUE;
    }
    else
    {
        aDescRec->mFixedPrecScale = STL_FALSE;
    }

    switch( aDescRec->mConsiceType )
    {
        case SQL_TINYINT :
            aDescRec->mPrecision = 1;
            break;
        case SQL_SMALLINT :
            aDescRec->mPrecision = 5;
            break;
        case SQL_INTEGER :
            aDescRec->mPrecision = 10;
            break;
        case SQL_BIGINT :
            aDescRec->mPrecision = 19;
            break;
        case SQL_REAL :
            aDescRec->mPrecision = 7;
            break;
        case SQL_DOUBLE :
            aDescRec->mPrecision = 15;
            break;
        case SQL_FLOAT :
            if( aTarget->mDBType == DTL_TYPE_NUMBER )
            {
                aDescRec->mPrecision = DTL_FLOAT_MAX_PRECISION;
                aDescRec->mNumPrecRadix = 2;
            }
            else
            {
                aDescRec->mPrecision = aTarget->mPrecision;
            }
            break;
        default :
            aDescRec->mPrecision = aTarget->mPrecision;
            break;
    }
    
    aDescRec->mScale = aTarget->mScale;
    
    /*
     * Target Type 의 문자 관련 Type 정보
     */

    aDescRec->mStringLengthUnit = aTarget->mStringLengthUnit;
    
    if( aTarget->mIsCaseSensitive == STL_TRUE )
    {
        aDescRec->mCaseSensitive = SQL_TRUE;
    }
    else
    {
        aDescRec->mCaseSensitive = SQL_FALSE;
    }

    if( aTarget->mLiteralPrefix != NULL )
    {
        if( aDescRec->mLiteralPrefix != NULL )
        {
            stlFreeHeap( aDescRec->mLiteralPrefix );
            aDescRec->mLiteralPrefix = NULL;
        }
        
        STL_TRY( stlAllocHeap( (void**)&aDescRec->mLiteralPrefix,
                               stlStrlen( aTarget->mLiteralPrefix ) + 1,
                               aErrorStack ) == STL_SUCCESS );
        
        stlStrcpy( aDescRec->mLiteralPrefix, aTarget->mLiteralPrefix );
    }

    if( aTarget->mLiteralSuffix != NULL )
    {
        if( aDescRec->mLiteralSuffix != NULL )
        {
            stlFreeHeap( aDescRec->mLiteralSuffix );
            aDescRec->mLiteralSuffix = NULL;
        }
        
        STL_TRY( stlAllocHeap( (void**)&aDescRec->mLiteralSuffix,
                               stlStrlen( aTarget->mLiteralSuffix ) + 1,
                               aErrorStack ) == STL_SUCCESS );
        
        stlStrcpy( aDescRec->mLiteralSuffix, aTarget->mLiteralSuffix );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zladDescBuildResult( zlcDbc        * aDbc,
                               zlsStmt       * aStmt,
                               stlBool         aNeedResultCols,
                               stlErrorStack * aErrorStack )
{
    zldDesc            * sIrd;
    zldDescElement     * sIrdRec;
    qllTarget          * sTargetInfo;
    dtlCharacterSet      sSourceCharacterSet = DTL_CHARACTERSET_MAX;
    dtlCharacterSet      sDestCharacterSet   = DTL_CHARACTERSET_MAX;
    zladConvertIRDFunc   sConvertIRDFunc;
    stlInt32             sState = 0;
    sslEnv             * sEnv;
    
    STL_TRY( sslEnterSession( aDbc->mSessionId,
                              aDbc->mSessionSeq,
                              aErrorStack )
             == STL_SUCCESS );
    sState = 1;

    sEnv = SSL_WORKER_ENV( knlGetSessionEnv(aDbc->mSessionId) );
    
    if( aNeedResultCols == STL_TRUE )
    {
        /*
         * 아직 컬럼 정보가 구축되지 않았거나 변경되었을 수 있음.
         */
        STL_TRY( zlaiBuildNumResultColsInternal( aStmt,
                                                 sEnv )
                 == STL_SUCCESS );
    }

    if( aDbc->mCharacterSet == aDbc->mNlsCharacterSet )
    {
        sConvertIRDFunc = zladNonConvertIRD;
    }
    else
    {
        sSourceCharacterSet = (ZLS_STMT_NLS_DT_VECTOR(aStmt))->mGetCharSetIDFunc(aStmt);
        sDestCharacterSet   = (ZLS_STMT_DT_VECTOR(aStmt))->mGetCharSetIDFunc(aStmt);
        
        sConvertIRDFunc = zladCovnertIRD;
    }

    STL_TRY( sslDescribeCol( aStmt->mStmtId,
                             &sTargetInfo,
                             sEnv ) == STL_SUCCESS );

    sIrd = &aStmt->mIrd;

    STL_RING_FOREACH_ENTRY( &sIrd->mDescRecRing, sIrdRec )
    {
        if( sTargetInfo == NULL )
        {
            break;
        }

        STL_TRY( zladDescSetIRD( sIrdRec,
                                 sTargetInfo,
                                 sSourceCharacterSet,
                                 sDestCharacterSet,
                                 sConvertIRDFunc,
                                 aStmt,
                                 KNL_ERROR_STACK(sEnv) ) == STL_SUCCESS );

        sTargetInfo = sTargetInfo->mNext;
    }

    sState = 0;
    sslLeaveSession( aDbc->mSessionId,
                     aDbc->mSessionSeq );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            stlAppendErrors( aErrorStack, KNL_ERROR_STACK( sEnv ) );
            STL_INIT_ERROR_STACK( KNL_ERROR_STACK( sEnv ) );
            sslLeaveSession( aDbc->mSessionId, aDbc->mSessionSeq );
        default:
            break;
    }

    return STL_FAILURE;
}

/** @} */
