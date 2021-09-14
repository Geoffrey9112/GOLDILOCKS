/*******************************************************************************
 * ztqDisplay.c
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
#include <dtl.h>
#include <goldilocks.h>
#include <sqlext.h>
#include <ztqDef.h>
#include <ztqDisplay.h>
#include <ztqHostVariable.h>

extern stlBool         gZtqVerticalColumnHeaderOrder;
extern stlInt32        gZtqLineSize;
extern stlBool         gZtqTiming;
extern stlBool         gZtqVerbose;
extern dtlCharacterSet gZtqCharacterSet;

/**
 * @file ztqDisplay.c
 * @brief Display Routines
 */

/**
 * @addtogroup ztqDisplay
 * @{
 */

stlPaintAttr gZtqRowPaintAttr[2];
stlPaintAttr gZtqHeaderPaintAttr;
stlPaintAttr gZtqErrorPaintAttr;
stlInt32     gZtqPrintTryCount = 0;
stlFile      gZtqSpoolFileHandle;
stlChar      gZtqSpoolFilePath[STL_MAX_FILE_PATH_LENGTH];
stlBool      gZtqSpoolOn = STL_FALSE;
stlInt64     gSpoolBufferSize = 0;
stlChar     *gSpoolBuffer;

extern stlBool gZtqSilent;
extern stlTime gZtqBeginTime;
extern stlTime gZtqEndTime;
extern stlTime gZtqPrepareBeginTime;
extern stlTime gZtqPrepareEndTime;
extern stlTime gZtqExecuteBeginTime;
extern stlTime gZtqExecuteEndTime;
extern stlTime gZtqFetchBeginTime;
extern stlTime gZtqFetchEndTime;
extern stlTime gZtqBindBeginTime;
extern stlTime gZtqBindEndTime;
extern stlBool gZtqPrintErrorMessage;

ztqSpoolCmdFunc gZtqSpoolCmdFunc[ZTQ_SPOOL_FLAG_MAX] =
{
    ztqSpoolBegin,
    ztqSpoolBegin,
    ztqSpoolBegin,
    ztqSpoolEnd,
    ztqSpoolStatus,
};

stlInt32 ztqWriteSpoolPrintf( const stlChar *aFormat, ... )
{
    stlInt32  sPrintLength;
    va_list   sVarArgList;

    va_start(sVarArgList, aFormat);
    sPrintLength = stlGetVprintfBufferSize( aFormat,
                                            sVarArgList );
    va_end(sVarArgList);

    va_start(sVarArgList, aFormat);
    ztqWriteSpoolFile( sPrintLength + 1,
                       aFormat,
                       sVarArgList );
    va_end(sVarArgList);

    return sPrintLength;
}

void ztqFreeSpoolBuffer( )
{
    if( gSpoolBuffer != NULL )
    {
        stlFreeHeap( gSpoolBuffer );
        gSpoolBuffer = NULL;
        gSpoolBufferSize = 0;
    }

    return;
}

stlStatus ztqAllocSpoolBuffer( stlSize         aSize,
                               stlErrorStack * aErrorStack )
{
    STL_TRY( stlAllocHeap( (void **)&gSpoolBuffer,
                           aSize,
                           aErrorStack )
             == STL_SUCCESS );
    gSpoolBufferSize = aSize;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztqWriteSpoolFile( stlInt32       aStringLength,
                             const stlChar *aFormat,
                             va_list        aVarArgList )
{
    stlInt32       sSpoolLen = 0;
    stlBool        sMemAlloc = STL_FALSE;
    stlSize        sWrittenBytes;
    stlSize        sAllocSize = 0;
    stlErrorStack  sErrorStack;

    STL_INIT_ERROR_STACK( &sErrorStack );

    if( (aStringLength + 1) > gSpoolBufferSize )
    {
        sAllocSize = STL_ALIGN( aStringLength + 1,
                                ZTQ_DEFAULT_SPOOL_BUFFER_SIZE );

        ztqFreeSpoolBuffer();

        STL_TRY( ztqAllocSpoolBuffer( sAllocSize,
                                      &sErrorStack )
                 == STL_SUCCESS );

        sMemAlloc = STL_TRUE;
    }

    sSpoolLen = stlVsnprintf( gSpoolBuffer, aStringLength + 1, aFormat, aVarArgList );

    STL_TRY( stlWriteFile( &gZtqSpoolFileHandle,
                           gSpoolBuffer,
                           sSpoolLen,
                           &sWrittenBytes,
                           &sErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    ztqSpoolEnd( NULL, ZTQ_SPOOL_FLAG_END, NULL, &sErrorStack );

    if( stlGetErrorStackDepth( &sErrorStack ) > 0 )
    {
        ztqPrintErrorStack( &sErrorStack );
    }
    ztqPrintf( "current spooling terminated abnormally\n" );

    if( sMemAlloc == STL_TRUE )
    {
        ztqFreeSpoolBuffer();
    }

    return 0;
}


stlInt32 ztqEchoPrintf( const stlChar *aFormat, ... )
{
    stlInt32  sReturn = 0;
    va_list   sVarArgList;

    va_start(sVarArgList, aFormat);
    sReturn = stlVprintf( aFormat, sVarArgList );
    va_end(sVarArgList);

    if( gZtqSpoolOn == STL_TRUE )
    {
        va_start(sVarArgList, aFormat);
        ztqWriteSpoolFile( sReturn,
                           aFormat,
                           sVarArgList );
        va_end(sVarArgList);
    }

    return sReturn;
}

stlInt32 ztqPrintf( const stlChar *aFormat, ... )
{
    stlInt32  sReturn = 0;
    va_list   sVarArgList;

    STL_TRY_THROW( gZtqSilent == STL_FALSE, RAMP_SKIP );

    if( gZtqPrintTryCount == 0 )
    {
        stlPrintf( "\n" );
        if( gZtqSpoolOn == STL_TRUE )
        {
            ztqWriteSpoolPrintf( "\n" );
        }
    }

    gZtqPrintTryCount++;

    va_start(sVarArgList, aFormat);
    sReturn = stlVprintf( aFormat, sVarArgList );
    va_end(sVarArgList);

    if( gZtqSpoolOn == STL_TRUE )
    {
        va_start(sVarArgList, aFormat);
        ztqWriteSpoolFile( sReturn,
                           aFormat,
                           sVarArgList );
        va_end(sVarArgList);
    }

    STL_RAMP( RAMP_SKIP );

    return sReturn;
}

stlInt32 ztqPaintf( stlPaintAttr * aAttr, const stlChar *aFormat, ... )
{
    stlInt32  sReturn = 0;
    va_list   sVarArgList;

    STL_TRY_THROW( gZtqSilent == STL_FALSE, RAMP_SKIP );

    if( gZtqPrintTryCount == 0 )
    {
        stlPrintf( "\n" );
        if( gZtqSpoolOn == STL_TRUE )
        {
            ztqWriteSpoolPrintf( "\n" );
        }
    }

    gZtqPrintTryCount++;

    va_start(sVarArgList, aFormat);
    sReturn = stlVpaintf( aAttr, aFormat, sVarArgList );
    va_end(sVarArgList);

    if( gZtqSpoolOn == STL_TRUE )
    {
        va_start(sVarArgList, aFormat);
        ztqWriteSpoolFile( sReturn,
                           aFormat,
                           sVarArgList );
        va_end(sVarArgList);
    }

    STL_RAMP( RAMP_SKIP );

    return sReturn;
}

stlStatus ztqPrintTableHeader( ztqRowBuffer    * aRowBuffer,
                               ztqTargetColumn * aTargetColumn )
{
    stlInt32 i;
    stlInt32 j;
    stlInt32 sStartColumn;
    stlInt32 sEndColumn;
    stlInt32 sAccColumnHeaderSize = 0;
    stlInt32 sPrintSize = 0;

    STL_TRY_THROW( gZtqVerticalColumnHeaderOrder == STL_FALSE, RAMP_SKIP );

    sEndColumn = -1;

    while( (sEndColumn + 1) < aRowBuffer->mColCnt )
    {
        sAccColumnHeaderSize = 0;
        sStartColumn = sEndColumn + 1;

        /*
         * 한 라인에 들어갈 컬럼 리스트 선택
         */
        for( i = sStartColumn; i < aRowBuffer->mColCnt; i++ )
        {
            sAccColumnHeaderSize += aRowBuffer->mColBuf[i].mMaxLength;
                
            if( sAccColumnHeaderSize > gZtqLineSize )
            {
                if( i == sStartColumn )
                {
                    sEndColumn = i;
                }
                else
                {
                    sEndColumn = i - 1;
                }
                break;
            }

            /*
             * 컬럼 Separator(1)
             */
            sAccColumnHeaderSize += 1;
                
            if( i == (aRowBuffer->mColCnt - 1) )
            {
                sEndColumn = i;
                break;
            }
        }

        /*
         * 컬럼 이름 출력
         */
        for( i = sStartColumn; i <= sEndColumn; i++ )
        {
            if ( aRowBuffer->mColBuf[i].mMaxLength > gZtqLineSize )
            {
                sPrintSize = gZtqLineSize;
            }
            else
            {
                sPrintSize = aRowBuffer->mColBuf[i].mMaxLength;
            }
            ztqPaintf( &gZtqHeaderPaintAttr,
                       aTargetColumn[i].mColumnFormat,
                       sPrintSize,
                       aRowBuffer->mColBuf[i].mColName );

            /*
             * 컬럼 Separator
             */
            if( i < sEndColumn )
            {
                ztqPaintf( &gZtqHeaderPaintAttr, " " );
            }
        }
        ztqPrintf( "\n" );
            
        /*
         * 라인 Separator 출력
         * "----------------------------..."
         */
        for( i = sStartColumn; i <= sEndColumn; i++ )
        {
            if ( aRowBuffer->mColBuf[i].mMaxLength > gZtqLineSize )
            {
                sPrintSize = gZtqLineSize;
            }
            else
            {
                sPrintSize = aRowBuffer->mColBuf[i].mMaxLength;
            }

            for( j = 0; j < sPrintSize; j++ )
            {
                ztqPaintf( &gZtqHeaderPaintAttr, "%c", '-' );
            }

            /*
             * 컬럼 Separator
             */
            if( i < sEndColumn )
            {
                ztqPaintf( &gZtqHeaderPaintAttr, " " );
            }
        }
        ztqPrintf( "\n" );
    }

    STL_RAMP( RAMP_SKIP );
    
    return STL_SUCCESS;
}

                     
stlStatus ztqPrintRowString( ztqRowBuffer    * aRowBuffer,
                             ztqTargetColumn * aTargetColumn,
                             stlInt32          aLineIdx,
                             stlAllocator    * aAllocator,
                             stlErrorStack   * aErrorStack )
{
    stlInt32          i = 0;
    stlInt32          sLineLength = 0;
    stlInt32          sPosition = 0;
    stlChar         * sValue = NULL;
    stlInt32          sDisplayLength = 0;
    stlInt8           sMbLength = 0;
    stlChar       *** sColumnLinePtrArray;
    stlRegionMark     sRegionMark;
    stlInt32          sLineIdx = 0;
    stlInt32          sStartIdx = 0;

    STL_TRY( stlMarkRegionMem( aAllocator,
                               &sRegionMark,
                               aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY( stlAllocRegionMem( aAllocator,
                                STL_SIZEOF(stlChar**) * aRowBuffer->mColCnt,
                                (void**)&sColumnLinePtrArray,
                                aErrorStack )
             == STL_SUCCESS );

    /*
     * 각 컬럼의 최대 컬럼 라인 공간을 할당한다.
     */
    for ( i = 0; i < aRowBuffer->mColCnt; i++ )
    {
        STL_TRY( stlAllocRegionMem( aAllocator,
                                    STL_SIZEOF(stlChar*) * aRowBuffer->mColBuf[i].mMaxLineCount,
                                    (void**)&sColumnLinePtrArray[i],
                                    aErrorStack )
                 == STL_SUCCESS );
        
        stlMemset( sColumnLinePtrArray[i],
                   0x00,
                   STL_SIZEOF(stlChar*) * aRowBuffer->mColBuf[i].mMaxLineCount );
    }

    if( gZtqVerticalColumnHeaderOrder == STL_FALSE )
    {
        sLineLength = 0;

        /*
         * 컬럼 라인 배열에 라인별 Value 포인터를 설정한다.
         *
         * ------
         * | c1 | --> L1, L2, L3 ...
         * ------
         * | c2 | --> L1, L2
         * ------
         * | .. |
         */
        for ( i = 0; i < aRowBuffer->mColCnt; i++ )
        {
            sValue = aRowBuffer->mColBuf[i].mDataBuf[aLineIdx];
            sPosition = 0;
            sLineIdx = 0;

            sColumnLinePtrArray[i][sLineIdx] = sValue;
            
            if( aTargetColumn[i].mDataTypeGroup == ZTQ_GROUP_NUMERIC )
            {
                aRowBuffer->mColBuf[i].mActualLineCount = 1;
                continue;
            }

            while( sValue[sPosition] != '\0' )
            {
                STL_TRY( dtlGetMbLength( gZtqCharacterSet,
                                         (stlChar*)sValue + sPosition,
                                         &sMbLength,
                                         aErrorStack )
                         == STL_SUCCESS );

                sDisplayLength += dtlGetMbDisplayLength( gZtqCharacterSet,
                                                         (stlUInt8*)(sValue + sPosition) );

                if( sMbLength == 1 )
                {
                    /*
                     * '\n'이라면 라인수를 늘린다.
                     */
                    if( *(sValue + sPosition) == '\n' )
                    {
                        sLineIdx += 1;
                        sColumnLinePtrArray[i][sLineIdx] = sValue + sPosition + sMbLength;
                        
                        *(sValue + sPosition) = '\0';
                    }
                }
                
                sPosition += sMbLength;
            }

            aRowBuffer->mColBuf[i].mActualLineCount = sLineIdx + 1;
        }
    }
        
    if( gZtqVerticalColumnHeaderOrder == STL_TRUE )
    {
        for ( i = 0; i < aRowBuffer->mColCnt; i++ )
        {
            ztqPaintf( &gZtqRowPaintAttr[aLineIdx % 2],
                       "%24s # ",
                       aRowBuffer->mColBuf[i].mColName );

            ztqPaintf( &gZtqRowPaintAttr[aLineIdx % 2],
                       "%s\n",
                       aRowBuffer->mColBuf[i].mDataBuf[aLineIdx] );
        }
        
        ztqPrintf( "\n" );
    }
    else
    {
        sLineLength = 0;
        
        for( i = 0; i < aRowBuffer->mColCnt; i++ )
        {
            if( (aRowBuffer->mColBuf[i].mMaxLength + sLineLength) > gZtqLineSize )
            {
                /*
                 * Line Length 를 넘어감 
                 */
                STL_TRY( ztqPrintColumnLines( aRowBuffer,
                                              aTargetColumn,
                                              aLineIdx,
                                              sColumnLinePtrArray,
                                              sStartIdx,
                                              i - 1,
                                              aErrorStack )
                         == STL_SUCCESS );
                    
                sStartIdx = i;
                sLineLength = (aRowBuffer->mColBuf[i].mMaxLength + 1);
            }
            else
            {
                /*
                 * Max Length + 컬럼 Separator(1)
                 */
                sLineLength += (aRowBuffer->mColBuf[i].mMaxLength + 1);
            }
        }

        /*
         * 남아 있는 모든 컬럼을 출력한다
         */
        if( sLineLength != 0 )
        {
            STL_TRY( ztqPrintColumnLines( aRowBuffer,
                                          aTargetColumn,
                                          aLineIdx,
                                          sColumnLinePtrArray,
                                          sStartIdx,
                                          i - 1,
                                          aErrorStack )
                     == STL_SUCCESS );
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztqPrintColumnLines( ztqRowBuffer      * aRowBuffer,
                               ztqTargetColumn   * aTargetColumn,
                               stlInt32            aLineIdx,
                               stlChar         *** aColumnLinePtrArray,
                               stlInt32            aStartColumnIdx,
                               stlInt32            aEndColumnIdx,
                               stlErrorStack     * aErrorStack )
{
    stlInt32   i = 0;
    stlInt32   j = 0;
    stlInt32   k = 0;
    stlInt32   sMaxLineCount = 0;
    stlInt32   sPosition = 0;
    stlChar  * sValue = NULL;
    stlInt32   sCharDisplayLength = 0;
    stlInt32   sColumnDisplayLength = 0;
    stlInt8    sMbLength = 0;
    stlChar    sMBChar[5];

    /*
     * 대상 컬럼들중 최대 라인 개수를 구한다.
     */
    for( i = aStartColumnIdx; i <= aEndColumnIdx; i++ )
    {
        sMaxLineCount = STL_MAX( aRowBuffer->mColBuf[i].mActualLineCount,
                                 sMaxLineCount );
    }

    /*
     * 최대 라인 개수 만큼 돌면서 ActualLineCount가 최대 라인보다 작다면
     * 공백을 출력하고, 그렇지 않다면 Data를 출력한다.
     *
     * gsql> insert into t1 values( 10000, 'abc
     *     2 def
     *     3 g' );
     *
     * 1 row created.
     *
     * gsql> select * from t1;
     *
     *    I1 I2 
     * ----- ---
     * 10000 abc
     *       def
     *       g
     *
     * 위에서는 I1의 ActualLineCount는 1이고, I2는 3이다.
     * sMaxLineCount는 3이 되고 I1의 경우 첫번째 라인은 Data가 출력되지만,
     * 두번째, 세번째 라인은 컬럼 폭만큼 공백으로 채운다.
     */
    for( i = 0; i < sMaxLineCount; i++ )
    {
        for( j = aStartColumnIdx; j <= aEndColumnIdx; j++ )
        {
            if( aRowBuffer->mColBuf[j].mActualLineCount > i )
            {
                sValue = aColumnLinePtrArray[j][i];
                sPosition = 0;

                /*
                 * Numeric Class는 Space Padding이 앞쪽에 채워진다
                 */
                if( aTargetColumn[j].mDataTypeGroup == ZTQ_GROUP_NUMERIC )
                {
                    for( k = 0;
                         k < aRowBuffer->mColBuf[j].mMaxLength - stlStrlen(sValue);
                         k++ )
                    {
                        ztqPaintf( &gZtqRowPaintAttr[aLineIdx % 2], " " );
                    }
                }

                sCharDisplayLength = 0;
                sColumnDisplayLength = 0;
                
                while( sValue[sPosition] != '\0' )
                {
                    STL_TRY( dtlGetMbLength( gZtqCharacterSet,
                                             (stlChar*)sValue + sPosition,
                                             &sMbLength,
                                             aErrorStack )
                             == STL_SUCCESS );

                    /**
                     * @todo database character set이 UTF8일 경우에만 화면에 맞추어 출력합니다.
                     * UTF8이 아닌 경우라면 1 바이트가 화면 1 컬럼이라고 가정합니다.
                     * 필요하다면 추가적으로 구현해야 합니다.
                     * 필요한 시점이 되면 아마도 dtl 인터페이스가 만들어지지 않았을까...
                     */
                    sCharDisplayLength = dtlGetMbDisplayLength( gZtqCharacterSet,
                                                                (stlUInt8*)(sValue + sPosition) );
                    sColumnDisplayLength += sCharDisplayLength;

                    /*
                     * 한 컬럼이 한줄을 차지하는 경우는 다수의 라인으로 출력될수 있다.
                     */
                    if( aRowBuffer->mColBuf[j].mMaxLength > gZtqLineSize )
                    {
                        if( sColumnDisplayLength > gZtqLineSize )
                        {
                            ztqPrintf( "\n" );
                            sColumnDisplayLength = sCharDisplayLength;
                        }
                    }

                    if( sMbLength == 1 )
                    {
                        /*
                         * Tab 문자는 n개의 Space로 채워진다
                         */
                        if( *(sValue + sPosition) == '\t' )
                        {
                            /*
                             * n개의 Space가 2라인으로 분리되는 경우
                             */
                            if( sColumnDisplayLength + (ZTQ_TAB_SIZE-1) > gZtqLineSize )
                            {
                                for( k = 0;
                                     k < gZtqLineSize - sColumnDisplayLength + 1;
                                     k++ )
                                {
                                    ztqPaintf( &gZtqRowPaintAttr[aLineIdx % 2], " " );
                                }
                            
                                ztqPrintf( "\n" );
                                sColumnDisplayLength += (ZTQ_TAB_SIZE-1) - gZtqLineSize;
                                
                                for( k = 0;
                                     k < sColumnDisplayLength;
                                     k++ )
                                {
                                    sMBChar[k] = ' ';
                                }
                                sMBChar[k] = '\0';
                            }
                            else
                            {
                                stlSnprintf( sMBChar, 5, "%*s", ZTQ_TAB_SIZE, " " );
                                sColumnDisplayLength += (ZTQ_TAB_SIZE-1);
                            }
                        }
                        else
                        {
                            stlMemcpy( sMBChar, sValue + sPosition, sMbLength );
                            sMBChar[sMbLength] = '\0';
                        }
                    }
                    else
                    {
                        stlMemcpy( sMBChar, sValue + sPosition, sMbLength );
                        sMBChar[sMbLength] = '\0';
                    }
                    ztqPaintf( &gZtqRowPaintAttr[aLineIdx % 2], "%s", sMBChar );
                    sPosition += sMbLength;
                }
                
                /*
                 * String Class는 Space Padding이 뒤쪽에 채워진다
                 */
                if( aTargetColumn[j].mDataTypeGroup == ZTQ_GROUP_STRING )
                {
                    for( k = 0;
                         k < ( STL_MIN( aRowBuffer->mColBuf[j].mMaxLength,
                                        gZtqLineSize ) - sColumnDisplayLength );
                         k++ )
                    {
                        ztqPaintf( &gZtqRowPaintAttr[aLineIdx % 2], " " );
                    }
                }
            }
            else
            {
                /*
                 * Dummy String을 채운다.
                 */
                for( k = 0;
                     k < aRowBuffer->mColBuf[j].mMaxLength;
                     k++ )
                {
                    ztqPaintf( &gZtqRowPaintAttr[aLineIdx % 2], " " );
                }
            }

            if( j < aEndColumnIdx )
            {
                /*
                 * 컬럼 Separator
                 */
                ztqPaintf( &gZtqRowPaintAttr[aLineIdx % 2], " " );
            }
        }

        ztqPrintf( "\n" );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztqDisableColoredText( stlErrorStack * aErrorStack )
{
    gZtqRowPaintAttr[0].mTextAttr  = STL_PTA_OFF;
    gZtqRowPaintAttr[0].mForeColor = STL_PFC_OFF;
    gZtqRowPaintAttr[0].mBackColor = STL_PBC_OFF;
    
    gZtqRowPaintAttr[1].mTextAttr  = STL_PTA_OFF;
    gZtqRowPaintAttr[1].mForeColor = STL_PFC_OFF;
    gZtqRowPaintAttr[1].mBackColor = STL_PBC_OFF;
    
    gZtqHeaderPaintAttr.mTextAttr  = STL_PTA_OFF;
    gZtqHeaderPaintAttr.mForeColor = STL_PFC_OFF;
    gZtqHeaderPaintAttr.mBackColor = STL_PBC_OFF;
    
    gZtqErrorPaintAttr.mTextAttr  = STL_PTA_OFF;
    gZtqErrorPaintAttr.mForeColor = STL_PFC_OFF;
    gZtqErrorPaintAttr.mBackColor = STL_PBC_OFF;
    
    return STL_SUCCESS;
}

stlStatus ztqEnableColoredText( stlErrorStack * aErrorStack )
{
    gZtqRowPaintAttr[0].mTextAttr  = STL_PTA_OFF;
    gZtqRowPaintAttr[0].mForeColor = STL_PFC_OFF;
    gZtqRowPaintAttr[0].mBackColor = STL_PBC_OFF;
    
    gZtqRowPaintAttr[1].mTextAttr  = STL_PTA_REVERSE;
    gZtqRowPaintAttr[1].mForeColor = STL_PFC_BROWN;
    gZtqRowPaintAttr[1].mBackColor = STL_PBC_OFF;
    
    gZtqHeaderPaintAttr.mTextAttr  = STL_PTA_BOLD;
    gZtqHeaderPaintAttr.mForeColor = STL_PFC_OFF;
    gZtqHeaderPaintAttr.mBackColor = STL_PBC_OFF;
    
    gZtqErrorPaintAttr.mTextAttr  = STL_PTA_OFF;
    gZtqErrorPaintAttr.mForeColor = STL_PFC_YELLOW;
    gZtqErrorPaintAttr.mBackColor = STL_PBC_OFF;
    
    return STL_SUCCESS;
}

stlStatus ztqPrintAnswerMessage( ztqStatementType   aStmtType,
                                 ztqObjectType      aObjectType,
                                 stlInt32           aRowCount,
                                 stlErrorStack    * aErrorStack )
{
    switch( aStmtType )
    {
        case ZTQ_STMT_SELECT:
        case ZTQ_STMT_SELECT_INTO:
        case ZTQ_STMT_INSERT:
        case ZTQ_STMT_DELETE:
        case ZTQ_STMT_UPDATE:
        case ZTQ_STMT_FETCH_CURSOR:
            STL_TRY( ztqPrintDmlAnswerMessage( aStmtType,
                                               aRowCount,
                                               aErrorStack )
                     == STL_SUCCESS );
            break;
        default:
            STL_TRY( ztqPrintEtcAnswerMessage( aStmtType,
                                               aObjectType,
                                               aErrorStack )
                     == STL_SUCCESS );
            break;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztqPrintDmlAnswerMessage( ztqStatementType   aStmtType,
                                    stlInt32           aRowCount,
                                    stlErrorStack    * aErrorStack )
{
    stlChar sMessage[ZTQ_MAX_ANSWER_MESSAGE];
    
    switch( aStmtType )
    {
        case ZTQ_STMT_SELECT:
        case ZTQ_STMT_SELECT_INTO:
            stlStrncpy( sMessage, "selected", ZTQ_MAX_ANSWER_MESSAGE );
            break;
        case ZTQ_STMT_INSERT:
            stlStrncpy( sMessage, "created", ZTQ_MAX_ANSWER_MESSAGE );
            break;
        case ZTQ_STMT_DELETE:
            stlStrncpy( sMessage, "deleted", ZTQ_MAX_ANSWER_MESSAGE );
            break;
        case ZTQ_STMT_UPDATE:
            stlStrncpy( sMessage, "updated", ZTQ_MAX_ANSWER_MESSAGE );
            break;
        case ZTQ_STMT_FETCH_CURSOR:
            stlStrncpy( sMessage, "fetched", ZTQ_MAX_ANSWER_MESSAGE );
            break;
        default:
            STL_ASSERT( 0 );
            break;
    }

    if( aRowCount == 0 )
    {
        ztqPrintf( "no rows %s.\n", sMessage );
    }
    else
    {
        if( aRowCount == 1 )
        {
            ztqPrintf( "1 row %s.\n", sMessage );
        }
        else
        {
            ztqPrintf( "%d rows %s.\n", aRowCount, sMessage );
        }
    }
    
    return STL_SUCCESS;
}

stlStatus ztqPrintTranAnswerMessage( stlBool         aIsCommit,
                                     stlErrorStack * aErrorStack )
{
    if( aIsCommit == STL_TRUE )
    {
        ztqPrintf( "Commit complete.\n" );
    }
    else
    {
        ztqPrintf( "Rollback complete.\n" );
    }
    
    return STL_SUCCESS;
}

stlStatus ztqPrintPreparedAnswerMessage( stlErrorStack * aErrorStack )
{
    ztqPrintf( "SQL prepared.\n" );
    
    return STL_SUCCESS;
}

stlStatus ztqPrintEtcAnswerMessage( ztqStatementType   aStmtType,
                                    ztqObjectType      aObjectType,
                                    stlErrorStack    * aErrorStack )
{
    stlChar sStatementStr[ZTQ_MAX_ANSWER_MESSAGE];
    stlChar sObjectStr[ZTQ_MAX_ANSWER_MESSAGE];

    sStatementStr[0] = '\0';
    sObjectStr[0] = '\0';
    
    switch( aObjectType )
    {
        case ZTQ_OBJECT_DATABASE:
            stlStrncpy( sStatementStr, "Database", ZTQ_MAX_ANSWER_MESSAGE );
            break;
        case ZTQ_OBJECT_INDEX:
            stlStrncpy( sStatementStr, "Index", ZTQ_MAX_ANSWER_MESSAGE );
            break;
        case ZTQ_OBJECT_SEQUENCE:
            stlStrncpy( sStatementStr, "Sequence", ZTQ_MAX_ANSWER_MESSAGE );
            break;
        case ZTQ_OBJECT_SESSION:
            stlStrncpy( sStatementStr, "Session", ZTQ_MAX_ANSWER_MESSAGE );
            break;
        case ZTQ_OBJECT_SYSTEM:
            stlStrncpy( sStatementStr, "System", ZTQ_MAX_ANSWER_MESSAGE );
            break;
        case ZTQ_OBJECT_SCHEMA:
            stlStrncpy( sStatementStr, "Schema", ZTQ_MAX_ANSWER_MESSAGE );
            break;
        case ZTQ_OBJECT_PROCEDURE:
            stlStrncpy( sStatementStr, "Procedure", ZTQ_MAX_ANSWER_MESSAGE );
            break;
        case ZTQ_OBJECT_FUNCTION:
            stlStrncpy( sStatementStr, "Function", ZTQ_MAX_ANSWER_MESSAGE );
            break;
        case ZTQ_OBJECT_TABLESPACE:
            stlStrncpy( sStatementStr, "Tablespace", ZTQ_MAX_ANSWER_MESSAGE );
            break;
        case ZTQ_OBJECT_TABLE:
            stlStrncpy( sStatementStr, "Table", ZTQ_MAX_ANSWER_MESSAGE );
            break;
        case ZTQ_OBJECT_USER:
            stlStrncpy( sStatementStr, "User", ZTQ_MAX_ANSWER_MESSAGE );
            break;
        case ZTQ_OBJECT_VIEW:
            stlStrncpy( sStatementStr, "View", ZTQ_MAX_ANSWER_MESSAGE );
            break;
        case ZTQ_OBJECT_ROLE:
            stlStrncpy( sStatementStr, "Role", ZTQ_MAX_ANSWER_MESSAGE );
            break;
        case ZTQ_OBJECT_CONSTRAINTS:
            stlStrncpy( sStatementStr, "Constraints", ZTQ_MAX_ANSWER_MESSAGE );
            break;
        case ZTQ_OBJECT_SAVEPOINT:
            stlStrncpy( sStatementStr, "Savepoint", ZTQ_MAX_ANSWER_MESSAGE );
            break;
        case ZTQ_OBJECT_TRANSACTION:
            stlStrncpy( sStatementStr, "Transaction", ZTQ_MAX_ANSWER_MESSAGE );
            break;
        case ZTQ_OBJECT_SYNONYM:
            stlStrncpy( sStatementStr, "Synonym", ZTQ_MAX_ANSWER_MESSAGE );
            break;
        case ZTQ_OBJECT_PROFILE:
            stlStrncpy( sStatementStr, "Profile", ZTQ_MAX_ANSWER_MESSAGE );
            break;
        default:
            break;
    }
    
    switch( aStmtType )
    {
        case ZTQ_STMT_SELECT:
        case ZTQ_STMT_SELECT_INTO:
            stlStrncpy( sObjectStr, "selected", ZTQ_MAX_ANSWER_MESSAGE );
            break;
        case ZTQ_STMT_INSERT:
            stlStrncpy( sObjectStr, "created", ZTQ_MAX_ANSWER_MESSAGE );
            break;
        case ZTQ_STMT_UPDATE:
            stlStrncpy( sObjectStr, "updated", ZTQ_MAX_ANSWER_MESSAGE );
            break;
        case ZTQ_STMT_DELETE:
            stlStrncpy( sObjectStr, "deleted", ZTQ_MAX_ANSWER_MESSAGE );
            break;
        case ZTQ_STMT_CREATE:
            stlStrncpy( sObjectStr, "created", ZTQ_MAX_ANSWER_MESSAGE );
            break;
        case ZTQ_STMT_DROP:
            stlStrncpy( sObjectStr, "dropped", ZTQ_MAX_ANSWER_MESSAGE );
            break;
        case ZTQ_STMT_ALTER:
            stlStrncpy( sObjectStr, "altered", ZTQ_MAX_ANSWER_MESSAGE );
            break;
        case ZTQ_STMT_TRUNCATE:
            stlStrncpy( sObjectStr, "truncated", ZTQ_MAX_ANSWER_MESSAGE );
            break;
        case ZTQ_STMT_RELEASE:
            stlStrncpy( sObjectStr, "released", ZTQ_MAX_ANSWER_MESSAGE );
            break;
        case ZTQ_STMT_GRANT:
            stlStrncpy( sObjectStr, "Grant succeeded", ZTQ_MAX_ANSWER_MESSAGE );
            break;
        case ZTQ_STMT_REVOKE:
            stlStrncpy( sObjectStr, "Revoke succeeded", ZTQ_MAX_ANSWER_MESSAGE );
            break;
        case ZTQ_STMT_SET:
            stlStrncpy( sObjectStr, "set", ZTQ_MAX_ANSWER_MESSAGE );
            break;
        case ZTQ_STMT_DECLARE_CURSOR:
            stlStrncpy( sObjectStr, "Cursor declared", ZTQ_MAX_ANSWER_MESSAGE );
            break;
        case ZTQ_STMT_OPEN_CURSOR:
            stlStrncpy( sObjectStr, "Cursor is open", ZTQ_MAX_ANSWER_MESSAGE );
            break;
        case ZTQ_STMT_CLOSE_CURSOR:
            stlStrncpy( sObjectStr, "Cursor closed", ZTQ_MAX_ANSWER_MESSAGE );
            break;
        case ZTQ_STMT_FETCH_CURSOR:
            stlStrncpy( sObjectStr, "fetched", ZTQ_MAX_ANSWER_MESSAGE );
            break;
        case ZTQ_STMT_COPY:
            stlStrncpy( sObjectStr, "copied", ZTQ_MAX_ANSWER_MESSAGE );
            break;
        case ZTQ_STMT_LOCK:
            stlStrncpy( sObjectStr, "locked", ZTQ_MAX_ANSWER_MESSAGE );
            break;
        case ZTQ_STMT_COMMENT:
            stlStrncpy( sObjectStr, "Comment created", ZTQ_MAX_ANSWER_MESSAGE );
            break;
        case ZTQ_STMT_CALL:
            stlStrncpy( sObjectStr, "Call complete", ZTQ_MAX_ANSWER_MESSAGE );
            break;
        case ZTQ_STMT_ROLLBACK:
            stlStrncpy( sObjectStr, "Rollback complete", ZTQ_MAX_ANSWER_MESSAGE );
            break;
        case ZTQ_STMT_COMMIT:
            stlStrncpy( sObjectStr, "Commit complete", ZTQ_MAX_ANSWER_MESSAGE );
            break;
        default:
            break;
    }

    if( stlStrlen( sStatementStr ) == 0 )
    {
        ztqPrintf( "%s.\n", sObjectStr );
    }
    else
    {
        ztqPrintf( "%s %s.\n", sStatementStr, sObjectStr );
    }
    
    return STL_SUCCESS;
}

void ztqClearElapsedTime()
{
    gZtqBeginTime = 0;
    gZtqPrepareBeginTime = 0;
    gZtqExecuteBeginTime = 0;
    gZtqFetchBeginTime = 0;
    gZtqBindBeginTime = 0;
    gZtqEndTime = 0;
    gZtqPrepareEndTime = 0;
    gZtqExecuteEndTime = 0;
    gZtqFetchEndTime = 0;
    gZtqBindEndTime = 0;
}

stlStatus ztqPrintElapsedTime( stlErrorStack * aErrorStack )
{
    stlTime sTotalElapsedTime;
    stlTime sPrepareElapsedTime;
    stlTime sExecuteElapsedTime;
    stlTime sFetchElapsedTime;
    stlTime sBindElapsedTime;
    stlChar sElapsedTimeStr[ZTQ_MAX_HOST_VALUE_SIZE];
    
    sTotalElapsedTime = gZtqEndTime - gZtqBeginTime;
    sPrepareElapsedTime = gZtqPrepareEndTime - gZtqPrepareBeginTime;
    sExecuteElapsedTime = gZtqExecuteEndTime - gZtqExecuteBeginTime;
    sFetchElapsedTime = gZtqFetchEndTime - gZtqFetchBeginTime;
    sBindElapsedTime = gZtqBindEndTime - gZtqBindBeginTime;

    if( gZtqTiming == STL_TRUE )
    {
        ztqPrintf( "\n" );
        ztqPrintf( "Elapsed time: %0.5f ms \n",
                   ((stlFloat64)sTotalElapsedTime) / 1000 );

        stlSnprintf( sElapsedTimeStr,
                     ZTQ_MAX_HOST_VALUE_SIZE,
                     "%lf",
                     ((stlFloat64)sTotalElapsedTime) / 1000 );
        
        STL_TRY( ztqAssignSysHostVariable( ZTQ_SYS_VAR_ELAPSED_TIME,
                                           sElapsedTimeStr,
                                           aErrorStack )
                 == STL_SUCCESS );

        if( gZtqVerbose == STL_TRUE )
        {
            ztqPrintf( "[PREPARE : %0.5f ms]\n",
                       ((stlFloat64)sPrepareElapsedTime) / 1000 );
            ztqPrintf( "[EXECUTE : %0.5f ms]\n",
                       ((stlFloat64)sExecuteElapsedTime) / 1000 );
            ztqPrintf( "[BIND    : %0.5f ms]\n",
                       ((stlFloat64)sBindElapsedTime) / 1000 );
            ztqPrintf( "[FETCH   : %0.5f ms]\n",
                       ((stlFloat64)sFetchElapsedTime) / 1000 );
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztqPrintCommandHelp( stlErrorStack * aErrorStack )
{
    ztqPrintf( "\\help                     \n" );
    ztqPrintf( "\\q[uit]                   \n" );
    ztqPrintf( "\\i[mport] {'FILE'}               Import SQL \n" );
    ztqPrintf( "\\ed[it] [{'FILE'|[HISTORY] num}] Edit SQL statement  \n" );
    ztqPrintf( "\\\\                               Executes the most recent history entry \n" );
    ztqPrintf( "\\{n}                             Executes n'th history entry \n" );
    ztqPrintf( "\\hi[story]                       Show history entries \n" );
    ztqPrintf( "\\desc     {[schema.]table_name}  Show table description \n" );
    ztqPrintf( "\\idesc    {[schema.]index_name}  Show index description \n" );
    ztqPrintf( "\\spo[ol]  ['filename' | OFF]     Stores query results in a file \n" );

    /*
     * Hidden option
     * ztqPrintf( "\\set callstack   {ON|OFF} \n" );
     */
    ztqPrintf( "\\set vertical    {ON|OFF} \n" );
    ztqPrintf( "\\set timing      {ON|OFF} \n" );
    ztqPrintf( "\\set color       {ON|OFF} \n" );
    ztqPrintf( "\\set error       {ON|OFF} \n" );
    ztqPrintf( "\\set autocommit  {ON|OFF} \n" );
    ztqPrintf( "\\set linesize    {n}      0 < n <= 100000\n" );
    ztqPrintf( "\\set pagesize    {n}      0 < n <= 100000\n" );
    ztqPrintf( "\\set colsize     {n}      0 < n <= 104857600\n" );
    ztqPrintf( "\\set numsize     {n}      0 < n <= 50\n" );
    ztqPrintf( "\\set ddlsize     {n}      0 < n <= 100000\n" );
    ztqPrintf( "\\set history     {n}      n <= 100000 ( if n < 0, clear history buffer ) \n" );

    ztqPrintf( "\\var             {host_var_name} {INTEGER|BIGINT|VARCHAR(n)} \n" );
    ztqPrintf( "\\exec            [{:host_var_name} := {constant}] \n" );
    ztqPrintf( "\\exec sql        {sql string}                   \n" );
    ztqPrintf( "\\prepare sql     {sql string}                   \n" );
    ztqPrintf( "\\dynamic sql     {host_var_name}                \n" );
    ztqPrintf( "\\explain plan    [{ON|ONLY}] {sql string}       \n" );
    ztqPrintf( "\\print           [{host_var_name}]              \n" );

    /* ztqPrintf( "\\alloc           {stmt_name}                              \n" ); */
    /* ztqPrintf( "\\free            {stmt_name}                              \n" ); */
    /* ztqPrintf( "\\use             {stmt_name} exec sql {sql string}        \n" ); */
    /* ztqPrintf( "\\use             {stmt_name} prepare sql {sql string}     \n" ); */
    /* ztqPrintf( "\\use             {stmt_name} exec                         \n" ); */

    ztqPrintf( "\\ddl_db                             \n" );
    ztqPrintf( "\\ddl_tablespace    {name}           \n" );
    ztqPrintf( "\\ddl_profile       {name}           \n" );
    ztqPrintf( "\\ddl_auth          {name}           \n" );
    ztqPrintf( "\\ddl_schema        {name}           \n" );
    ztqPrintf( "\\ddl_table         {[schema.]name}  \n" );
    ztqPrintf( "\\ddl_constraint    {[schema.]name}  \n" );
    ztqPrintf( "\\ddl_index         {[schema.]name}  \n" );
    ztqPrintf( "\\ddl_view          {[schema.]name}  \n" );
    ztqPrintf( "\\ddl_sequence      {[schema.]name}  \n" );
    ztqPrintf( "\\ddl_synonym       {[schema.]name}  \n" );
    ztqPrintf( "\\ddl_publicsynonym {name}           \n" );
    
    ztqPrintf( "\\startup         {[nomount|mount|open]}                   \n" );
    /*
     * Hidden option
     * ztqPrintf( "\\shutdown operational \n" );
     */
    ztqPrintf( "\\shutdown        {[abort|immediate|transactional|normal]} \n" );
    ztqPrintf( "\\connect         [userid password] [as {sysdba|admin}]    \n" );
    
    return STL_SUCCESS;
}

void ztqPrintErrorStack( stlErrorStack * aErrorStack )
{
    stlInt32 i;
    stlInt32 sErrorCode;
    stlInt32 sExtErrCode;
    stlChar  sSqlState[6];

    for( i = aErrorStack->mTop; i >= 0; i-- )
    {
        sErrorCode = aErrorStack->mErrorData[i].mErrorCode;
        sExtErrCode = stlGetExternalErrorCode( &aErrorStack->mErrorData[i] );
        stlMakeSqlState( sExtErrCode, sSqlState );

        if( gZtqPrintErrorMessage == STL_TRUE )
        {
            ztqPaintf( &gZtqErrorPaintAttr,
                       "ERR-%s(%d): %s %s\n",
                       sSqlState,
                       sErrorCode,
                       aErrorStack->mErrorData[i].mErrorMessage,
                       aErrorStack->mErrorData[i].mDetailErrorMessage );
        }
        else
        {
            ztqPaintf( &gZtqErrorPaintAttr,
                       "ERR-%s(%d)\n",
                       sSqlState,
                       sErrorCode );
        }
    }
}
    
void ztqPrintErrorCallstack( stlErrorStack * aErrorStack )
{
    stlChar  sBuffer[1024];
    stlInt32 i;

    for( i = aErrorStack->mTop; i >= 0; i-- )
    {
        stlPrintFirstErrorCallstackToStr( aErrorStack,
                                          sBuffer,
                                          1024 );

        ztqPaintf( &gZtqErrorPaintAttr, sBuffer );
    }
}

stlStatus ztqSpoolCommand( stlChar       *aSpoolFilePath,
                           ztqSpoolFlag   aSpoolFlag,
                           stlAllocator  *aAllocator,
                           stlErrorStack *aErrorStack )
{
    return gZtqSpoolCmdFunc[aSpoolFlag]( aSpoolFilePath,
                                         aSpoolFlag,
                                         aAllocator,
                                         aErrorStack );
}

stlStatus ztqSpoolBegin( stlChar       *aSpoolFilePath,
                         ztqSpoolFlag   aSpoolFlag,
                         stlAllocator  *aAllocator,
                         stlErrorStack *aErrorStack )
{
    stlInt32   sFlags;
    stlInt32   sState = 0;

    /*
     * 기존에 Spool을 사용중이었으면 먼저 종료를 한다.
     */
    ztqSpoolEnd( NULL, ZTQ_SPOOL_FLAG_END, NULL, aErrorStack );

    sFlags = STL_FOPEN_WRITE;

    switch( aSpoolFlag )
    {
        case ZTQ_SPOOL_FLAG_BEGIN_CREATE:
            sFlags |= STL_FOPEN_CREATE | STL_FOPEN_EXCL;
            break;
        case ZTQ_SPOOL_FLAG_BEGIN_REPLACE:
            sFlags |= STL_FOPEN_CREATE | STL_FOPEN_TRUNCATE;
            break;
        case ZTQ_SPOOL_FLAG_BEGIN_APPEND:
            sFlags |= STL_FOPEN_CREATE | STL_FOPEN_APPEND;
            break;
        default:
            STL_DASSERT( 0 );
            break;
    }

    STL_TRY( stlOpenFile( &gZtqSpoolFileHandle,
                          aSpoolFilePath,
                          sFlags,
                          STL_FPERM_OS_DEFAULT,
                          aErrorStack )
             == STL_SUCCESS );
    sState = 1;

    stlStrncpy( gZtqSpoolFilePath, aSpoolFilePath, STL_MAX_FILE_PATH_LENGTH );
    gZtqSpoolOn = STL_TRUE;

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            (void)stlCloseFile( &gZtqSpoolFileHandle, aErrorStack );
            break;
        default:
            break;
    }

    return STL_FAILURE;
}

stlStatus ztqSpoolEnd( stlChar       *aSpoolFilePath,
                       ztqSpoolFlag   aSpoolFlag,
                       stlAllocator  *aAllocator,
                       stlErrorStack *aErrorStack )
{
    if( gZtqSpoolOn == STL_TRUE )
    {
        gZtqSpoolFilePath[0] = '\0';
        gZtqSpoolOn = STL_FALSE;
        STL_TRY( stlCloseFile( &gZtqSpoolFileHandle,
                               aErrorStack )
                 == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztqSpoolStatus( stlChar       *aSpoolFilePath,
                          ztqSpoolFlag   aSpoolFlag,
                          stlAllocator  *aAllocator,
                          stlErrorStack *aErrorStack )
{
    if( gZtqSpoolOn == STL_TRUE )
    {
        ztqPrintf( "currently spooling to %s\n", gZtqSpoolFilePath );
    }
    else
    {
        ztqPrintf( "not spooling currently\n" );
    }

    return STL_SUCCESS;
}


/** @} */
