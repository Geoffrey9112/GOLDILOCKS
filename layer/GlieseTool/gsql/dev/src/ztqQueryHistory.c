/*******************************************************************************
 * ztqQueryHistory.c
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
#include <ztqQueryHistory.h>
#include <ztqMain.h>

extern stlInt32 gZtqPageSize;

/**
 * @file ztqQueryHistory.c
 * @brief Query History Routines
 */

/**
 * @addtogroup ztqQueryHistory
 * @{
 */

ztqHistoryQueue * gZtqHistoryQueue;
stlInt32          gZtqHistoryTop;
stlInt32          gZtqHistorySize;

ztqTargetColumn gZtqHistoryTargetColumn[2] =
{
    {
        "ID",
        ZTQ_COLUMN_FORMAT_CLASS_NUMERIC,
        DTL_NATIVE_BIGINT_DEFAULT_PRECISION,
        NULL,
        SQL_VARCHAR,
        SQL_NULL_DATA,
        ZTQ_GROUP_NUMERIC,
        DTL_TYPE_NA,
        {0,0}
    },
    {
        "SQL",
        ZTQ_COLUMN_FORMAT_CLASS_STRING,
        ZTQ_MAX_COMMAND_BUFFER_SIZE,
        NULL,
        SQL_VARCHAR,
        SQL_NULL_DATA,
        ZTQ_GROUP_STRING,
        DTL_TYPE_NA,
        {0,0}
    }
};

stlStatus ztqAllocHistoryCommandBuffer( ztqHistoryQueue * aHistoryEntry,
                                        stlInt64          aBufSize,
                                        stlErrorStack   * aErrorStack )
{
    STL_TRY( stlAllocHeap( (void **)&aHistoryEntry->mCommand,
                           aBufSize,
                           aErrorStack )
             == STL_SUCCESS );

    stlMemset( aHistoryEntry->mCommand, 0x00, aBufSize );

    aHistoryEntry->mBufSize = aBufSize;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

void ztqFreeHistoryCommandBuffer( ztqHistoryQueue * aHistoryEntry )
{
    stlFreeHeap( aHistoryEntry->mCommand );
    aHistoryEntry->mCommand = NULL;
    aHistoryEntry->mBufSize = 0;
}

stlStatus ztqAllocHistoryQueue( ztqHistoryQueue **aHistoryQueue,
                                stlInt32          aHistorySize,
                                stlErrorStack    *aErrorStack )
{
    ztqHistoryQueue *sHistoryQueue = NULL;
    stlInt32         i             = -1;
    stlInt32         sState        = 0;

    STL_TRY( stlAllocHeap( (void**)&sHistoryQueue,
                           STL_SIZEOF(ztqHistoryQueue) * aHistorySize,
                           aErrorStack )
             == STL_SUCCESS );
    sState = 1;

    stlMemset( sHistoryQueue,
               0x00,
               STL_SIZEOF(ztqHistoryQueue) * aHistorySize );

    for( i = 0; i < aHistorySize; i ++ )
    {
        STL_TRY( ztqAllocHistoryCommandBuffer( &sHistoryQueue[i],
                                               ZTQ_MAX_COMMAND_BUFFER_SIZE,
                                               aErrorStack )
                 == STL_SUCCESS );
    }

    *aHistoryQueue = sHistoryQueue;

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            for( ; i >= 0; i -- )
            {
                ztqFreeHistoryCommandBuffer( &sHistoryQueue[i] );
            }
            stlFreeHeap( sHistoryQueue );
            break;
        default:
            break;
    }

    *aHistoryQueue = NULL;

    return STL_FAILURE;
}

stlStatus ztqFreeHistoryQueue( ztqHistoryQueue  *aHistoryQueue,
                               stlInt32          aHistorySize,
                               stlErrorStack    *aErrorStack )
{
    stlInt32 i;

    if( aHistoryQueue != NULL )
    {
        for( i = 0; i < aHistorySize; i ++ )
        {
            ztqFreeHistoryCommandBuffer( &aHistoryQueue[i] );
        }

        stlFreeHeap( aHistoryQueue );
    }

    return STL_SUCCESS;
}

stlStatus ztqSetHistoryCommand( ztqHistoryQueue *aHistoryEntry,
                                stlChar         *aString,
                                stlInt64         aId,
                                stlErrorStack   *aErrorStack )
{
    stlInt64 sStringLen;

    sStringLen = stlStrlen( aString );

    if( sStringLen > aHistoryEntry->mBufSize )
    {
        ztqFreeHistoryCommandBuffer( aHistoryEntry );
        STL_TRY( ztqAllocHistoryCommandBuffer( aHistoryEntry,
                                               sStringLen + 1,
                                               aErrorStack )
                 == STL_SUCCESS );
    }

    aHistoryEntry->mId = aId;

    stlStrncpy( aHistoryEntry->mCommand,
                aString,
                sStringLen + 1 );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztqInitHistory( stlErrorStack * aErrorStack )
{
    gZtqHistoryTop = 1;
    gZtqHistorySize = ZTQ_DEFAULT_HISTORY_BUFFER_SIZE;

    STL_TRY( ztqAllocHistoryQueue( &gZtqHistoryQueue,
                                   gZtqHistorySize,
                                   aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztqAddHistory( stlChar       * aString,
                         stlErrorStack * aErrorStack )
{
    stlInt32 sHistoryIdx;

    STL_TRY_THROW( gZtqHistorySize > 0, RAMP_FINISH );

    sHistoryIdx = gZtqHistoryTop % gZtqHistorySize;

    STL_TRY( ztqSetHistoryCommand( &gZtqHistoryQueue[sHistoryIdx],
                                   aString,
                                   gZtqHistoryTop,
                                   aErrorStack )
             == STL_SUCCESS );

    gZtqHistoryTop += 1;

    STL_RAMP( RAMP_FINISH );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztqGetCommandFromHistoryByNum( stlInt64   aHistoryNum,
                                         stlChar  **aCommand )
{
    stlChar   * sCommand = NULL;
    stlInt32    i;

    if( aHistoryNum != 0 )
    {
        for( i = 0; i < gZtqHistorySize; i++ )
        {
            if( gZtqHistoryQueue[i].mId == aHistoryNum )
            {
                sCommand = gZtqHistoryQueue[i].mCommand;
                break;
            }
        }
    }
    else
    {
        aHistoryNum = (gZtqHistoryTop-1) % gZtqHistorySize;

        if( gZtqHistoryQueue[aHistoryNum].mId == (gZtqHistoryTop - 1) )
        {
            sCommand = gZtqHistoryQueue[aHistoryNum].mCommand;
        }
    }

    *aCommand = sCommand;

    return STL_SUCCESS;
}

stlStatus ztqExecHistory( stlInt64        aHistoryNum,
                          stlAllocator  * aAllocator,
                          stlErrorStack * aErrorStack )
{
    stlChar   * sCommand = NULL;

    STL_TRY( ztqGetCommandFromHistoryByNum( aHistoryNum,
                                            &sCommand )
             == STL_SUCCESS );

    STL_TRY_THROW( sCommand != NULL, RAMP_SKIP_EXEC );

    if( ztqIsGSQLCommand( sCommand ) != STL_TRUE )
    {
        STL_TRY( ztqProcessSql( sCommand,
                                aAllocator,
                                STL_TRUE, /* aPrintResult */
                                aErrorStack )
                 == STL_SUCCESS );;
    }
    else
    {
        STL_TRY( ztqProcessCommand( sCommand,
                                    aAllocator,
                                    aErrorStack )
                 == STL_SUCCESS );

    }

    STL_RAMP( RAMP_SKIP_EXEC );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztqPrintHistory( stlAllocator  * aAllocator,
                           stlErrorStack * aErrorStack )
{
    ztqRowBuffer * sRowBuffer;
    stlInt32       i;
    stlInt32       j;
    stlInt32       sLineCount = 0;
    stlChar        sHistoryNumStr[DTL_NATIVE_BIGINT_DEFAULT_PRECISION];

    STL_TRY( ztqAllocRowBuffer( &sRowBuffer,
                                2,             /* aColumnCount */
                                gZtqPageSize,     /* aLineBufferCount */
                                gZtqHistoryTargetColumn,
                                aAllocator,
                                aErrorStack )
             == STL_SUCCESS );

    if( gZtqHistorySize > 0 )
    {
        for( i = (gZtqHistoryTop % gZtqHistorySize); i < gZtqHistorySize; i++ )
        {
            if( gZtqHistoryQueue[i].mId != 0 )
            {
                stlSnprintf( sHistoryNumStr,
                             DTL_NATIVE_BIGINT_DEFAULT_PRECISION,
                             "%ld",
                             gZtqHistoryQueue[i].mId );

                gZtqHistoryTargetColumn[0].mDataValue = sHistoryNumStr;
                gZtqHistoryTargetColumn[0].mIndicator = stlStrlen( sHistoryNumStr );
                gZtqHistoryTargetColumn[1].mDataValue = gZtqHistoryQueue[i].mCommand;
                gZtqHistoryTargetColumn[1].mIndicator =
                    STL_MIN( stlStrlen( gZtqHistoryQueue[i].mCommand ),
                             ZTQ_MAX_COMMAND_BUFFER_SIZE );

                STL_TRY( ztqMakeRowString( sRowBuffer,
                                           sLineCount,
                                           gZtqHistoryTargetColumn,
                                           STL_TRUE,
                                           aErrorStack )
                         == STL_SUCCESS );
                sLineCount += 1;

                if( sLineCount == gZtqPageSize )
                {
                    for( j = 0; j < sLineCount; j++ )
                    {
                        if( j == 0 )
                        {
                            STL_TRY( ztqPrintTableHeader( sRowBuffer,
                                                          gZtqHistoryTargetColumn )
                                     == STL_SUCCESS );
                        }

                        STL_TRY( ztqPrintRowString( sRowBuffer,
                                                    gZtqHistoryTargetColumn,
                                                    j,
                                                    aAllocator,
                                                    aErrorStack )
                                 == STL_SUCCESS );

                    }

                    ztqSetInitRowStringLen( sRowBuffer, gZtqHistoryTargetColumn );
                    sLineCount = 0;
                    ztqPrintf( "\n" );
                }
            }
        }

        for( i = 0; i < (gZtqHistoryTop % gZtqHistorySize); i++ )
        {
            if( gZtqHistoryQueue[i].mId != 0 )
            {
                stlSnprintf( sHistoryNumStr,
                             DTL_NATIVE_BIGINT_DEFAULT_PRECISION,
                             "%ld",
                             gZtqHistoryQueue[i].mId );

                gZtqHistoryTargetColumn[0].mDataValue = sHistoryNumStr;
                gZtqHistoryTargetColumn[0].mIndicator = stlStrlen( sHistoryNumStr );
                gZtqHistoryTargetColumn[1].mDataValue = gZtqHistoryQueue[i].mCommand;
                gZtqHistoryTargetColumn[1].mIndicator =
                    STL_MIN( stlStrlen( gZtqHistoryQueue[i].mCommand ),
                             ZTQ_MAX_COMMAND_BUFFER_SIZE );

                STL_TRY( ztqMakeRowString( sRowBuffer,
                                           sLineCount,
                                           gZtqHistoryTargetColumn,
                                           STL_TRUE,
                                           aErrorStack )
                         == STL_SUCCESS );
                sLineCount += 1;

                if( sLineCount == gZtqPageSize )
                {
                    for( j = 0; j < sLineCount; j++ )
                    {
                        if( j == 0 )
                        {
                            STL_TRY( ztqPrintTableHeader( sRowBuffer,
                                                          gZtqHistoryTargetColumn )
                                     == STL_SUCCESS );
                        }

                        STL_TRY( ztqPrintRowString( sRowBuffer,
                                                    gZtqHistoryTargetColumn,
                                                    j,
                                                    aAllocator,
                                                    aErrorStack )
                                 == STL_SUCCESS );

                    }

                    ztqSetInitRowStringLen( sRowBuffer, gZtqHistoryTargetColumn );
                    sLineCount = 0;
                    ztqPrintf( "\n" );
                }
            }
        }
    }

    for( i = 0; i < sLineCount; i++ )
    {
        if( i == 0 )
        {
            STL_TRY( ztqPrintTableHeader( sRowBuffer,
                                          gZtqHistoryTargetColumn )
                     == STL_SUCCESS );
        }

        STL_TRY( ztqPrintRowString( sRowBuffer,
                                    gZtqHistoryTargetColumn,
                                    i,
                                    aAllocator,
                                    aErrorStack )
                 == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztqSetHistoryBufferSize( stlInt64        aHistorySize,
                                   stlErrorStack * aErrorStack )
{
    ztqHistoryQueue * sHistoryQueue = NULL;
    stlInt32          i;

    if( aHistorySize > 0 )
    {
        STL_TRY( ztqAllocHistoryQueue( &sHistoryQueue,
                                       aHistorySize,
                                       aErrorStack )
                 == STL_SUCCESS );
    }

    if( gZtqHistorySize > 0 )
    {
        for( i = (gZtqHistoryTop % gZtqHistorySize); i < gZtqHistorySize; i++ )
        {
            if( gZtqHistoryQueue[i].mId != 0 )
            {
                if( aHistorySize > 0 )
                {
                    STL_TRY( ztqSetHistoryCommand( &sHistoryQueue[gZtqHistoryQueue[i].mId % aHistorySize],
                                                   gZtqHistoryQueue[i].mCommand,
                                                   gZtqHistoryQueue[i].mId,
                                                   aErrorStack )
                             == STL_SUCCESS );
                }
            }
        }

        for( i = 0; i < (gZtqHistoryTop % gZtqHistorySize); i++ )
        {
            if( gZtqHistoryQueue[i].mId != 0 )
            {
                if( aHistorySize > 0 )
                {
                    STL_TRY( ztqSetHistoryCommand( &sHistoryQueue[gZtqHistoryQueue[i].mId % aHistorySize],
                                                   gZtqHistoryQueue[i].mCommand,
                                                   gZtqHistoryQueue[i].mId,
                                                   aErrorStack )
                             == STL_SUCCESS );
                }
            }
        }

        STL_TRY( ztqFreeHistoryQueue( gZtqHistoryQueue,
                                      gZtqHistorySize,
                                      aErrorStack )
                 == STL_SUCCESS );
    }

    gZtqHistoryQueue = sHistoryQueue;
    gZtqHistorySize = aHistorySize;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztqClearHistoryBuffer( stlErrorStack * aErrorStack )
{
    stlInt32 i;

    for( i = 0; i < gZtqHistorySize; i++ )
    {
        gZtqHistoryQueue[i].mId = 0;
        gZtqHistoryQueue[i].mCommand[0] = '\0';
    }

    gZtqHistoryTop = 1;

    return STL_SUCCESS;
}

stlStatus ztqDestHistory( stlErrorStack * aErrorStack )
{
    return ztqFreeHistoryQueue( gZtqHistoryQueue,
                                gZtqHistorySize,
                                aErrorStack );
}

/** @} */

