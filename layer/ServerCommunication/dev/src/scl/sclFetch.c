/*******************************************************************************
 * cmlFetch.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: cmlFetch.c 6676 2012-12-13 08:39:46Z egon $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#include <stl.h>
#include <dtl.h>
#include <cml.h>
#include <cmlRowStatusDef.h>
#include <knl.h>
#include <sclDef.h>
#include <scDef.h>
#include <scpProtocol.h>
#include <sclGeneral.h>
#include <sccCommunication.h>

/**
 * @file sclFetch.c
 * @brief Communication Layer Fetch Protocol Routines
 */

/**
 * @addtogroup cmlProtocol
 * @{
 */


/**
 * @brief Fetch Command Protocol을 읽는다.
 * @param[in]  aHandle       Communication Handle
 * @param[out] aControl      Protocol Control
 * @param[out] aStatementId  Statement Identifier
 * @param[out] aOffset       Fetch Offset
 * @param[out] aRowCount     Row Count
 * @param[in]  aEnv          env
 * 
 * @par protocol sequence:
 * @code
 * 
 * - command (t)ype : command type         var_int bytes
 * - (p)eriod       : protocol period      1 byte
 * - statement (i)d : statement identifier var_int,var_int bytes
 * - o(f)fset       : fetch offset         var_int bytes
 * - (r)ow count    : row count            var_int bytes
 *
 * field sequence : t-p-i-o-f-r
 * 
 * @endcode
 */
stlStatus sclReadFetchCommand( sclHandle     * aHandle,
                               stlInt8       * aControl,
                               stlInt64      * aStatementId,
                               stlInt64      * aOffset,
                               stlInt32      * aRowCount,
                               sclEnv        * aEnv )
{
    stlInt16    sCommandType = CML_COMMAND_FETCH;
    scpCursor   sCursor;
    
    STL_TRY( scpBeginReading( aHandle,
                              &sCursor,
                              aEnv )
             == STL_SUCCESS );

    SCP_READ_VAR_INT16( aHandle, &sCursor, &sCommandType, aEnv );
    SCP_READ_INT8( aHandle, &sCursor, aControl, aEnv );
    SCP_READ_STATEMENT_ID( aHandle, &sCursor, aStatementId, aEnv );
    SCP_READ_VAR_INT64( aHandle, &sCursor, aOffset, aEnv );
    SCP_READ_VAR_INT32( aHandle, &sCursor, aRowCount, aEnv );

    STL_DASSERT( sCommandType == CML_COMMAND_FETCH );
    
    STL_TRY( scpEndReading( aHandle,
                            &sCursor,
                            aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Fetch Result Protocol을 기록한다.
 * @param[in]  aHandle         communication handle
 * @param[in]  aFirstRowIdx    rowset first row idx
 * @param[in]  aValueBlockList ValueBlockList pointer
 * @param[in]  aErrorStack     기록할 Error Stack
 * @param[in]  aEnv            env
 */
stlStatus sclWriteFetchBegin( sclHandle          * aHandle,
                              stlInt64             aFirstRowIdx,
                              dtlValueBlockList  * aValueBlockList,
                              stlErrorStack      * aErrorStack,
                              sclEnv             * aEnv )
{
    scpCursor           sCursor;
    stlInt16            sResultType = CML_COMMAND_FETCH;
    stlInt32            sColumnCount;
    dtlValueBlockList * sColumn = aValueBlockList;

    STL_TRY( scpBeginWriting( aHandle,
                              &sCursor,
                              aEnv )
             == STL_SUCCESS );

    SCP_WRITE_VAR_INT16( aHandle, &sCursor, &sResultType, aEnv );

    STL_TRY( scpWriteErrorResult( aHandle,
                                  &sCursor,
                                  aErrorStack,
                                  aEnv ) == STL_SUCCESS );

    sColumnCount = 0;
    sColumn = aValueBlockList;
    while( sColumn != NULL )
    {
        sColumn = sColumn->mNext;
        sColumnCount++;
    }

    SCP_WRITE_VAR_INT32( aHandle, &sCursor, &sColumnCount, aEnv );
    SCP_WRITE_VAR_INT64( aHandle, &sCursor, &aFirstRowIdx, aEnv );

    STL_TRY( scpEndWriting( aHandle,
                            &sCursor,
                            aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Fetch Result Protocol을 기록한다.
 * @param[in]  aHandle         communication handle
 * @param[in]  aStartIdx       start block index
 * @param[in]  aRowCount       row count
 * @param[in]  aValueBlockList ValueBlockList pointer
 * @param[in]  aStatus         Memory block for row status
 * @param[in]  aEnv            env
 * 
 * @par protocol sequence:
 * @code
 * 
 * - result (t)ype    : result type         var_int bytes
 * - e(r)ror level    : error level         1 byte
 * - r(o)w count      : row count           var_int bytes
 * - co(l)umn count   : column count        var_int bytes
 * - column (v)alue   : column value        variable
 * - pa(y)load period : payload period      1 byte
 * - error (c)ount    : error count         1 byte
 * - (s)ql state      : sql state           4 bytes
 * - n(a)tive error   : native error        4 bytes
 * - (m)essage text   : message text        variable
 *
 * field sequence : if r == 0 then t-r-o-l-[v]^(o*l)-y
 *                  else if r == 1 then t-r-c-[s-a-m]^c-o-l-[v]^(o*l)-y
 *                  else t-r-c-[s-a-m]^c
 * 
 * @endcode
 */
stlStatus sclWriteFetchResult( sclHandle          * aHandle,
                               stlInt64             aStartIdx,
                               stlInt32             aRowCount,
                               dtlValueBlockList  * aValueBlockList,
                               stlInt8              aStatus,
                               sclEnv             * aEnv )
{
    scpCursor           sCursor;
    stlInt64            i;
    dtlValueBlockList * sColumn = aValueBlockList;
    dtlDataValue      * sDataValue;

    STL_TRY( scpBeginWriting( aHandle,
                              &sCursor,
                              aEnv )
             == STL_SUCCESS );

    SCP_WRITE_VAR_INT32( aHandle, &sCursor, &aRowCount, aEnv );

    for( i = aStartIdx; i < (aStartIdx + aRowCount); i++ )
    {
        SCP_WRITE_INT8( aHandle, &sCursor, &aStatus, aEnv );

        if( aStatus == CML_CURSOR_ROW_STATUS_DELETED )
        {
            sColumn = NULL;
        }
        else
        {
            sColumn = aValueBlockList;
        }

        while( sColumn != NULL )
        {
            sDataValue = DTL_GET_BLOCK_DATA_VALUE( sColumn, i );

            SCP_WRITE_N_VAR( aHandle, &sCursor, sDataValue->mValue, sDataValue->mLength, aEnv );
            
            sColumn = sColumn->mNext;
        }
    }

    STL_TRY( scpEndWriting( aHandle,
                            &sCursor,
                            aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Fetch Result Protocol에 Fetch End Flag를 설정한다.
 * @param[in] aHandle         communication handle
 * @param[in] aLastRowIdx     전체 result set의 마지막 row idx. 모를 경우 -1
 * @param[in] aCursorClosed   Cursor Close 여부
 * @param[in] aEnv     env
 */
stlStatus sclWriteFetchEnd( sclHandle          * aHandle,
                            stlInt64             aLastRowIdx,
                            stlBool              aCursorClosed,
                            sclEnv      * aEnv )
{
    scpCursor   sCursor;
    stlInt32    sRowCountNoMoreRow = 0;

    STL_TRY( scpBeginWriting( aHandle,
                              &sCursor,
                              aEnv )
             == STL_SUCCESS );

    SCP_WRITE_VAR_INT32( aHandle, &sCursor, &sRowCountNoMoreRow, aEnv );
    SCP_WRITE_VAR_INT64( aHandle, &sCursor, &aLastRowIdx, aEnv );
    SCP_WRITE_INT8( aHandle, &sCursor, &aCursorClosed, aEnv );

    STL_TRY( scpEndWriting( aHandle,
                            &sCursor,
                            aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus sclWriteFetchEndWithError( sclHandle       * aHandle,
                                     sclEnv   * aEnv )
{            
    scpCursor      sCursor;
    stlInt32       sRowCountWithError = -1;
    stlErrorData * sErrorData;
    stlInt8        sErrorCount;
    stlInt32       sMessageLen;
    stlInt32       sDetailMessageLen;
    stlInt32       i;
    
    STL_TRY( scpBeginWriting( aHandle,
                              &sCursor,
                              aEnv )
             == STL_SUCCESS );

    SCP_WRITE_VAR_INT32( aHandle, &sCursor, &sRowCountWithError, aEnv );

    sErrorCount = stlGetErrorStackDepth( KNL_ERROR_STACK(aEnv) );
    SCP_WRITE_INT8( aHandle, &sCursor, &sErrorCount, aEnv );

    for( i = 0; i < sErrorCount; i++ )
    {
        sErrorData = &(KNL_ERROR_STACK(aEnv)->mErrorData[i]);
        sMessageLen = stlStrlen( sErrorData->mErrorMessage );
        sDetailMessageLen = stlStrlen( sErrorData->mDetailErrorMessage );

        SCP_WRITE_INT32( aHandle, &sCursor, &sErrorData->mExternalErrorCode, aEnv );
        SCP_WRITE_INT32( aHandle, &sCursor, &sErrorData->mErrorCode, aEnv );
        SCP_WRITE_N_VAR( aHandle, &sCursor, sErrorData->mErrorMessage, sMessageLen, aEnv );
        SCP_WRITE_N_VAR( aHandle, &sCursor, sErrorData->mDetailErrorMessage, sDetailMessageLen, aEnv );
    }

    STL_TRY( scpEndWriting( aHandle,
                            &sCursor,
                            aEnv )
             == STL_SUCCESS );
                                   
    return STL_SUCCESS;            
                 
    STL_FINISH;

    return STL_FAILURE;
}                                 


/** @} */
