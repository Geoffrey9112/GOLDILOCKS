 /*******************************************************************************
 * ztdExecute.c
 *
 * Copyright (c) 2012, SUNJESOFT Inc.
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
#include <stlDef.h>
#include <dtl.h>
#include <goldilocks.h>
#include <ztdDef.h>
#include <ztdDisplay.h>
#include <ztdExecute.h>
#include <ztdExecThread.h>
#include <ztdFileExecute.h>
#include <ztdBufferedFile.h>

extern dtlCharacterSet    gZtdCharacterSet;
extern stlBool            gZtdRunState;

/**
 * @brief gloader의 download를 실행.
 * @param [in]  aEnvHandle       SQLHENV
 * @param [in]  aDbcHandle       SQLHDBC  
 * @param [in]  aAllocato r      main에서 할당한 Region Memory
 * @param [in]  aInputArguments  gloader input aruments  
 * @param [in]  aControlInfo     Control File의 내용을 저장한 struct
 * @param [in]  aFileAndBuffer   File Descriptor 와 log를 담는 struct
 * @param [out] aErrorStack      error stack
 */
stlStatus ztdExportTextData( SQLHENV             aEnvHandle,
                             SQLHDBC             aDbcHandle,
                             stlAllocator      * aAllocator,
                             ztdInputArguments * aInputArguments,
                             ztdControlInfo    * aControlInfo,
                             ztdFileAndBuffer  * aFileAndBuffer,
                             stlErrorStack     * aErrorStack )
{
    SQLHSTMT           sStmt        = NULL;
    SQLHDESC           sDesc        = NULL;
    SQLRETURN          sSQLRet      = -1;
    SQLSMALLINT        sColNums     = 0;
    stlInt32           sIdxColumn   = 0;
    stlInt32           sState       = 0;
    stlInt64           sRecordCount = 0;
    stlChar            sSqlString[ZTD_MAX_COMMAND_BUFFER_SIZE];

    ztdRecordBuffer    sRecordBuffer;
    ztdColumnInfo    * sColumnInfo  = NULL;
    ztdColDataExp    * sColumnData  = NULL;
    stlInt32           sWriteSize;

    STL_TRY( SQLAllocHandle( SQL_HANDLE_STMT,
                             aDbcHandle,
                             &sStmt )
             == SQL_SUCCESS );
    sState = 1;

    /* Table의 Column 개수를 얻는다. */
    if( aControlInfo->mSchema[0] == 0x00 )
    {
        stlSnprintf( sSqlString,
                     ZTD_MAX_COMMAND_BUFFER_SIZE,
                     "SELECT * FROM %s",
                     aControlInfo->mTable );
    }
    else
    {
        stlSnprintf( sSqlString,
                     ZTD_MAX_COMMAND_BUFFER_SIZE,
                     "SELECT * FROM %s.%s",
                     aControlInfo->mSchema,
                     aControlInfo->mTable );
    }
    
    STL_TRY( SQLExecDirect( sStmt,
                            (SQLCHAR *) sSqlString,
                            SQL_NTS )
             == SQL_SUCCESS );
    sState = 2;
    
    STL_TRY( SQLNumResultCols( sStmt, &sColNums )
             == SQL_SUCCESS );
   
    /* Column의 개수 만큼 sColumnInfo을 할당한다. */
    STL_TRY( stlAllocRegionMem( aAllocator,
                                STL_SIZEOF( ztdColumnInfo ) * sColNums,
                                (void **) &sColumnInfo,
                                aErrorStack )
             == STL_SUCCESS );
    
    /* Column의 개수 만큼 sColumnData을 할당한다. */
    STL_TRY( stlAllocRegionMem( aAllocator,
                                STL_SIZEOF( ztdColDataExp ) * sColNums,
                                (void **) &sColumnData,
                                aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY( SQLGetStmtAttr( sStmt,
                             SQL_ATTR_IMP_ROW_DESC,
                             &sDesc,
                             SQL_IS_POINTER,
                             NULL )
             == SQL_SUCCESS );

    if( aControlInfo->mSchema[0] == 0x00 )
    {
        STL_TRY( SQLGetDescField( sDesc,
                                  1,
                                  SQL_DESC_SCHEMA_NAME,
                                  aControlInfo->mSchema,
                                  ZTD_MAX_SCHEMA_NAME,
                                  NULL )
                 == SQL_SUCCESS );
    }
    else
    {
        /* Do Notting */
    }

    sRecordBuffer.mBufferSize = ZTD_MAX_WRITE_RECORD_BUFFER_SIZE;
    
    for( sIdxColumn = 0; sIdxColumn < sColNums; sIdxColumn++ )
    {   
        STL_TRY( SQLDescribeCol( sStmt,
                                 sIdxColumn + 1,
                                 (SQLCHAR*)sColumnInfo[sIdxColumn].mName,
                                 ZTD_MAX_COLUMN_NAME,
                                 &sColumnInfo[sIdxColumn].mNameLength,
                                 &sColumnInfo[sIdxColumn].mSQLType,
                                 &sColumnInfo[sIdxColumn].mDataSize,
                                 &sColumnInfo[sIdxColumn].mDecimalDigits,
                                 &sColumnInfo[sIdxColumn].mNullable )
                 == SQL_SUCCESS );

        STL_TRY( SQLGetDescField( sDesc,
                                  sIdxColumn + 1,
                                  SQL_DESC_DISPLAY_SIZE,
                                  &sColumnData[sIdxColumn].mBufferSize,
                                  SQL_IS_POINTER,
                                  NULL )
                 == SQL_SUCCESS );
        
        switch( sColumnInfo[sIdxColumn].mSQLType )
        {
            /**
             * Record Size는 ZTD_WRITE_RECORD_BUFFER_SIZE(10M)로 정해지고,
             * Column Data가 저장되는 Buffer의 Size는 Display Size로 정해진다.
             * 단, Long Varchar/Varbinary는 ZTD_LONG_VAR_TYPE_SIZE(10M)로 정해진다.
             * Long Data type의 Column이 추가되면 Record Size에 더한다.
             */            
            case SQL_LONGVARCHAR:
            case SQL_LONGVARBINARY:
                sColumnData[sIdxColumn].mBufferSize = ZTD_LONG_VAR_TYPE_SIZE;
                sRecordBuffer.mBufferSize += sColumnData[sIdxColumn].mBufferSize;

                STL_TRY( stlAllocHeap( (void**) &(sColumnData[sIdxColumn].mBuffer),
                                       sColumnData[sIdxColumn].mBufferSize,
                                       aErrorStack )
                         == STL_SUCCESS );
                break;
            default:
                sColumnData[sIdxColumn].mBufferSize += 1;

                STL_TRY( stlAllocHeap( (void**) &(sColumnData[sIdxColumn].mBuffer),
                                       sColumnData[sIdxColumn].mBufferSize,
                                       aErrorStack )
                         == STL_SUCCESS );

                STL_TRY( SQLBindCol( sStmt,
                                     sIdxColumn + 1,
                                     SQL_C_CHAR,
                                     sColumnData[sIdxColumn].mBuffer,
                                     sColumnData[sIdxColumn].mBufferSize,
                                     &sColumnData[sIdxColumn].mIndicator )
                         == SQL_SUCCESS );
                break;
        }       
    }

    sState = 3;

    /* 전체 Column의 사이즈(aRecordSize) 만큼 문자열 크기 할당.  */
    STL_TRY( stlAllocRegionMem( aAllocator,
                                sRecordBuffer.mBufferSize + ZTD_ALIGN_512,
                                (void **) &sRecordBuffer.mBuffer,
                                aErrorStack )
             == STL_SUCCESS );
    
    ZTD_ALIGN_BUFFER( sRecordBuffer.mAlignedBuffer, sRecordBuffer.mBuffer, ZTD_ALIGN_512);

    stlMemset( sRecordBuffer.mAlignedBuffer, ZTD_CONSTANT_SPACE, sRecordBuffer.mBufferSize );

     /* Pointer에 Buffer의 시작위치를 저장한다. */
    sRecordBuffer.mCurrentPtr = sRecordBuffer.mAlignedBuffer;
    
    while( STL_TRUE )
    {
        STL_TRY( gZtdRunState == STL_TRUE );
        sSQLRet = SQLFetch( sStmt );

        /**
         * sSQLRet == SQL_NO_DATA
         * : Buffer에 Data File에 기록할지 판단하고 종료.
         * sSQLRet == SQL_ERROR
         * : Log File에 기록 후 종료.
         * sSQLRet == SQL_SUCCESS
         * sSQLRet == SQL_SUCCESS_WITH_INFO
         * : Buffer에 Data를 기록한다.
         *   LONGVARCHAR/LONGVARBINARY 는 SQLGetData를 통해서 Data를 얻는다.
         */
        
        if( sSQLRet == SQL_NO_DATA )
        {
            if( sRecordBuffer.mCurrentPtr != sRecordBuffer.mAlignedBuffer )
            {
                sWriteSize = sRecordBuffer.mCurrentPtr - sRecordBuffer.mAlignedBuffer;
                
                if( aInputArguments->mDirectIo == STL_TRUE )
                {
                    sWriteSize = STL_ALIGN( sWriteSize , ZTD_ALIGN_512 );
                }

                STL_TRY( stlWriteFile( &(aFileAndBuffer->mDataFile),
                                       sRecordBuffer.mAlignedBuffer,
                                       sWriteSize,
                                       NULL,
                                       aErrorStack ) == STL_SUCCESS );
            }
            else
            {
                /* do nothing */
            }
            break;
        }
        else if( sSQLRet == SQL_ERROR )
        {
            STL_TRY( ztdCheckError( aEnvHandle,
                                    aDbcHandle,
                                    sStmt,
                                    sRecordCount,
                                    aFileAndBuffer,
                                    NULL,
                                    aErrorStack )
                     == STL_SUCCESS );      
            break;
        }
        else if( (sSQLRet == SQL_SUCCESS) ||
                 (sSQLRet == SQL_SUCCESS_WITH_INFO) )
        {
            for( sIdxColumn = 0; sIdxColumn < sColNums; sIdxColumn++ )
            {
                if( (sColumnInfo[sIdxColumn].mSQLType == SQL_LONGVARCHAR) ||
                    (sColumnInfo[sIdxColumn].mSQLType == SQL_LONGVARBINARY) )
                {
                    STL_TRY( ztdGetLongData( sStmt,
                                             sRecordCount,
                                             sIdxColumn + 1,
                                             aFileAndBuffer,
                                             &sColumnData[sIdxColumn],
                                             aErrorStack )
                             == STL_SUCCESS );
                }
                else
                {
                    /* do nothing */
                }
                
                STL_TRY( ztdWriteColumnToBuffer( &(aFileAndBuffer->mDataFile),
                                                 &sRecordBuffer,
                                                 &sColumnData[sIdxColumn],
                                                 sColumnInfo[sIdxColumn].mSQLType,
                                                 aControlInfo,
                                                 aErrorStack )
                         == STL_SUCCESS );

                if( aControlInfo->mClQualifier != 0x0 )
                {
                    STL_TRY( ztdStringAppend( &(aFileAndBuffer->mDataFile),
                                              &sRecordBuffer,
                                              &aControlInfo->mClQualifier,
                                              ZTD_QUALIFIER_LEN,
                                              aErrorStack )
                             == STL_SUCCESS );
                }

                if( sIdxColumn < (sColNums - 1) )
                {
                    STL_TRY( ztdStringAppend( &(aFileAndBuffer->mDataFile),
                                              &sRecordBuffer,
                                              aControlInfo->mDelimiter,
                                              stlStrlen( aControlInfo->mDelimiter ),
                                              aErrorStack )
                             == STL_SUCCESS );
                }

                sColumnData[sIdxColumn].mBuffer[0] = ZTD_CONSTANT_NULL;   
            }
        }
        else
        {
            /* do nothing */
        }

        STL_TRY( ztdStringAppend( &(aFileAndBuffer->mDataFile),
                                  &sRecordBuffer,
                                  "\n",
                                  stlStrlen("\n"),
                                  aErrorStack )
                 == STL_SUCCESS );

        sRecordCount = sRecordCount + 1;
        if( (sRecordCount % ZTD_COMMIT_FREQUENCY) == 0 )
        {
            stlPrintf("\ndownloaded %ld records from %s.%s\n",
                      sRecordCount,
                      aControlInfo->mSchema,
                      aControlInfo->mTable );
        }
    } /* while: sqlfetch */

    /* Log File에 기록할 Log 설정 */
    stlSnprintf( aFileAndBuffer->mLogBuffer,
                 ZTD_MAX_LOG_STRING_SIZE,
                 "COMPLETED IN EXPORTING TABLE: %s.%s, %ld RECORDS ",
                 aControlInfo->mSchema,
                 aControlInfo->mTable,
                 sRecordCount );

    sState = 2;

    for( sIdxColumn = 0; sIdxColumn < sColNums; sIdxColumn++ )
    {
        stlFreeHeap( sColumnData[sIdxColumn].mBuffer );
    }
    
    sState = 1;
    STL_TRY( SQLCloseCursor( sStmt )
             == SQL_SUCCESS );

    sState = 0;
    STL_TRY( SQLFreeHandle( SQL_HANDLE_STMT,
                            sStmt )
             == SQL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    switch( sState )
    {
        case 3:
            for( sIdxColumn = 0; sIdxColumn < sColNums; sIdxColumn++ )
            {
                if( sColumnData[sIdxColumn].mBuffer != NULL )
                {
                    stlFreeHeap( sColumnData[sIdxColumn].mBuffer );
                }
            }
        case 2:
            ztdCheckError( aEnvHandle,
                           aDbcHandle,
                           sStmt,
                           sRecordCount,
                           aFileAndBuffer,
                           NULL,
                           aErrorStack );
            
            (void)SQLCloseCursor( sStmt );

            (void)SQLFreeHandle( SQL_HANDLE_STMT,
                                 sStmt );
            break;
        case 1:
            
            ztdCheckError( aEnvHandle,
                           aDbcHandle,
                           sStmt,
                           sRecordCount,
                           aFileAndBuffer,
                           NULL,
                           aErrorStack );
            
            (void)SQLFreeHandle( SQL_HANDLE_STMT,
                                 sStmt );
            break;
        default :
            break;
    }
    
    return STL_FAILURE;
}
/**
 * @brief Fetch로 얻은 Column Data를 Buffer에 쓴다.
 * @param [in]     aStmt           Statement handle
 * @param [in]     aRecCount       Record Total count
 * @param [in/out] aFileAndbuffer  File descriptor and buffer
 * @param [in]     aIdxColumn      Column Index
 * @param [out]    aColumnData     Data Buffer와 Size, Indicator를 갖는 구조체.
 * @param [in/out] aErrorSTack     error stack
 */

stlStatus ztdGetLongData( SQLHSTMT           aStmt,
                          stlInt64           aRecCount,
                          stlInt32           aIdxColumn,
                          ztdFileAndBuffer * aFileAndBuffer,
                          ztdColDataExp    * aColumnData,
                          stlErrorStack    * aErrorStack )
{
    stlInt64   sInitSize = 0;
    SQLRETURN  sRet;
    stlInt32   sState    = 0;
    
    sRet = SQLGetData( aStmt,
                       aIdxColumn,
                       SQL_C_CHAR,
                       aColumnData->mBuffer,
                       sInitSize,
                       &aColumnData->mIndicator );
    /**
     * sRet은 SQL_SUCCESS_WITH_INFO가 나와야 할 것이다.
     * SQL_NO_DATA: SQLFetch 지났기 때문에 나올 수 없다.
     * SQL_SUCCESS: BufferSize를 0으로 주었기 때문에 나올 수 없다.
     */
    if( sRet != SQL_SUCCESS_WITH_INFO )
    {
        STL_TRY( ztdWriteDiagnostic( SQL_HANDLE_STMT,
                                     aStmt,
                                     STL_TRUE,  /* Is Printf Record Number */
                                     aRecCount,
                                     aFileAndBuffer,
                                     aErrorStack )
                 == STL_SUCCESS );
    }

    if( aColumnData->mIndicator != SQL_NULL_DATA )
    {
        if( (aColumnData->mIndicator + 1) > aColumnData->mBufferSize )
        {
            stlFreeHeap( aColumnData->mBuffer );

            aColumnData->mBufferSize = aColumnData->mIndicator + 1;
        
            STL_TRY( stlAllocHeap( (void**) &aColumnData->mBuffer,
                                   aColumnData->mBufferSize,
                                   aErrorStack )
                     == STL_SUCCESS );
            sState = 1;
        }
        else
        {
            /* do nothing */
        }
    
        STL_TRY( SQLGetData( aStmt,
                             aIdxColumn,
                             SQL_C_CHAR,
                             aColumnData->mBuffer,
                             aColumnData->mBufferSize,
                             &aColumnData->mIndicator )
                 == SQL_SUCCESS );
    }
                         
    return STL_SUCCESS;

    STL_FINISH;
    
    switch( sState )
    {
        case 1:
            (void) stlFreeHeap( aColumnData->mBuffer );
            break;
        default:
            break;
    }
    
    return STL_FAILURE;
}

/**
 * @brief Fetch로 얻은 Column Data를 Buffer에 쓴다.
 * @param [in]     aFile         Datafile descriptor
 * @param [out]    aRecordBuffer Data가 기록되고 Datafile에 쓰여지는 Buffer를 지닌 구조체.
 * @param [in]     aColumnData   Column의 Data를 지닌 구조체 
 * @param [in]     aDataType     Column의 Data Type
 * @param [in]     aControlInfo  Control File Contents
 * @param [in/out] aErrorSTack   error stack
 */
stlStatus ztdWriteColumnToBuffer( stlFile         * aFile,
                                  ztdRecordBuffer * aRecordBuffer,
                                  ztdColDataExp   * aColumnData,
                                  SQLSMALLINT       aDataType,
                                  ztdControlInfo  * aControlInfo,
                                  stlErrorStack   * aErrorStack )
{
    stlInt32   sOffSetData;
    stlInt32   sRemainLen;
    stlChar    sOpQualifier = aControlInfo->mOpQualifier;
    stlChar    sClQualifier = aControlInfo->mClQualifier;
    stlChar  * aSource      = aColumnData->mBuffer;
    stlInt64   aSourceLen   = ((aColumnData->mBufferSize > aColumnData->mIndicator) ?
                               aColumnData->mIndicator : aColumnData->mBufferSize);

    if( sOpQualifier == 0x0 )
    {
        STL_TRY( ztdStringAppend( aFile,
                                  aRecordBuffer,
                                  aSource,
                                  aSourceLen,
                                  aErrorStack )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( ztdStringAppend( aFile,
                                  aRecordBuffer,
                                  &aControlInfo->mOpQualifier,
                                  ZTD_QUALIFIER_LEN,
                                  aErrorStack )
                 == STL_SUCCESS );
        
        switch( aDataType )
        {
            case SQL_CHAR:
            case SQL_VARCHAR:
            case SQL_LONGVARCHAR:
                sRemainLen = ZTD_GET_REMAIN_BUFFER_LENGTH( aRecordBuffer ) ;
                
                for( sOffSetData = 0; sOffSetData < aSourceLen; sOffSetData++ )
                {
                    if( sRemainLen == 0 )
                    {
                        STL_TRY( ztdWriteData( aFile,
                                               aRecordBuffer,
                                               aErrorStack ) == STL_SUCCESS );
                        
                        sRemainLen = ZTD_GET_REMAIN_BUFFER_LENGTH( aRecordBuffer );
                    }
                    
                    if( aSource[sOffSetData] == sClQualifier )
                    {
                        *aRecordBuffer->mCurrentPtr = aSource[sOffSetData];
                        aRecordBuffer->mCurrentPtr++;
                        sRemainLen--;
                        if( sRemainLen == 0 )
                        {
                            STL_TRY( ztdWriteData( aFile,
                                                   aRecordBuffer,
                                                   aErrorStack ) == STL_SUCCESS );

                            sRemainLen = ZTD_GET_REMAIN_BUFFER_LENGTH( aRecordBuffer );
                        }
                    }

                    *aRecordBuffer->mCurrentPtr = aSource[sOffSetData];
                    aRecordBuffer->mCurrentPtr++;
                    sRemainLen--;
                }
                
                if( sRemainLen == 0 )
                {
                    STL_TRY( ztdWriteData( aFile,
                                           aRecordBuffer,
                                           aErrorStack ) == STL_SUCCESS );
                }
                break;
            default:
                STL_TRY( ztdStringAppend( aFile,
                                          aRecordBuffer,
                                          aSource,
                                          aSourceLen,
                                          aErrorStack )
                         == STL_SUCCESS );
                break;       
        }
        
    }
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief Target String 끝에 Source String을 붙인다.
 * @param [in]     aFile         Datafile descriptor
 * @param [in/out] aRecordBuffer Record가 기록되는 Buffer를 지닌 구조체
 * @param [in]     aSource       Buffer에 기록되는 String
 * @param [in]     aSourceLen    Source String의 길이 
 * @param [in/out] aErrorStack   error stack
 */ 
stlStatus ztdStringAppend( stlFile         * aFile,
                           ztdRecordBuffer * aRecordBuffer,
                           stlChar         * aSource,
                           stlInt64          aSourceLen,
                           stlErrorStack   * aErrorStack )
{
    stlChar  * sEndPtr;
    stlInt64   sCount;

    sEndPtr = &aRecordBuffer->mAlignedBuffer[ aRecordBuffer->mBufferSize - 1];

    for( sCount = aSourceLen; sCount > 0 ; sCount-- )
    {
        *aRecordBuffer->mCurrentPtr = *aSource;
        aSource++;
        
        if( aRecordBuffer->mCurrentPtr == sEndPtr )
        {
            STL_TRY( ztdWriteData( aFile,
                                   aRecordBuffer,
                                   aErrorStack ) == STL_SUCCESS );

            continue;
        }
        else
        {
            STL_TRY_THROW( *aRecordBuffer->mCurrentPtr != ZTD_CONSTANT_NULL, EXIT_FUNCTION );
        }

        aRecordBuffer->mCurrentPtr++;
    }

    STL_RAMP(EXIT_FUNCTION);

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 할당한 Heap memory을 해제하고 두 배의 크기로 재할당한다.
 * @param [out]    aString       Heap memory의 문자열
 * @param [in]     aOldSize      Heap memory의 old size
 * @param [out]    aNewSize      Heap memory의 new size
 * @param [in/out] aErrorStack   error stack 
 */
stlStatus ztdExpandHeap( stlChar       ** aString,
                         stlInt64         aOldSize,
                         stlInt64       * aNewSize,
                         stlErrorStack  * aErrorStack )
{
    stlInt64  sAllocSize;
    stlChar * sTempString = NULL;

    sAllocSize = aOldSize * 2;

    STL_TRY( stlAllocHeap( (void **) &sTempString,
                           sAllocSize,
                           aErrorStack )
             == STL_SUCCESS );
    
    sTempString[0] = ZTD_CONSTANT_NULL;
    
    stlMemcpy( sTempString,
               *aString,
               aOldSize );
    
    stlFreeHeap( (void*) *aString );

    *aNewSize = sAllocSize;
    *aString  = sTempString;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
