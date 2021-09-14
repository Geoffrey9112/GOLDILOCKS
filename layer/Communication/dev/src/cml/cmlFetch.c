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
#include <cmlDef.h>
#include <cmDef.h>
#include <cmlProtocol.h>
#include <cmlGeneral.h>
#include <cmlRowStatusDef.h>
#include <cmpCursor.h>
#include <cmpProtocol.h>
#include <cmgMemory.h>

/**
 * @file cmlFetch.c
 * @brief Communication Layer Fetch Protocol Routines
 */

/**
 * @addtogroup cmlProtocol
 * @{
 */

/**
 * @brief Fetch Command Protocol을 기록한다.
 * @param[in] aHandle       Communication Handle
 * @param[in] aControl      Protocol Control
 * @param[in] aStatementId  Statement Identifier
 * @param[in] aOffset       Fetch Offset
 * @param[in] aRowCount     Row Count
 * @param[in] aErrorStack   Error Stack Pointer
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
stlStatus cmlWriteFetchCommand( cmlHandle     * aHandle,
                                stlInt8         aControl,
                                stlInt64        aStatementId,
                                stlInt64        aOffset,
                                stlInt32        aRowCount,
                                stlErrorStack * aErrorStack )
{
    stlInt16    sCommandType = CML_COMMAND_FETCH;
    cmpCursor   sCursor;

    STL_TRY( cmpBeginWriting( aHandle,
                              &sCursor,
                              aErrorStack )
             == STL_SUCCESS );

    CMP_WRITE_VAR_INT16( aHandle, &sCursor, &sCommandType, aErrorStack );
    CMP_WRITE_INT8( aHandle, &sCursor, &aControl, aErrorStack );
    CMP_WRITE_STATEMENT_ID_ENDIAN( aHandle, &sCursor, &aStatementId, aErrorStack );
    CMP_WRITE_VAR_INT64( aHandle, &sCursor, &aOffset, aErrorStack );
    CMP_WRITE_VAR_INT32( aHandle, &sCursor, &aRowCount, aErrorStack );

    STL_TRY( cmpEndWriting( aHandle,
                            &sCursor,
                            aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}




/**
 * @brief Fetch Result Protocol을 읽는다.
 * @param[in]  aHandle               Communication Handle
 * @param[out] aIgnored              Ignore protocol 여부
 * @param[out] aValueBlockList       ValueBlockList Pointer
 * @param[out] aFirstRowIdxOfRowset  fetch한 rowset의 첫번째 row의 절대 위치
 * @param[out] aLastRowIdx           전체 resultset의 마지막 row의 절대 위치. 모르면 -1
 * @param[out] aStatus               Row Status Pointer. row status를 저장할 공간. fetch count만큼 할당되어 있어야 함
 * @param[out] aCursorClosed         Cursor Close 여부
 * @param[in]  aReallocMemCallback   long varying memory allocator
 * @param[in]  aCallbackContext      callback context ( statement handle )
 * @param[in]  aErrorStack           Error Stack Pointer
 * 
 * @par protocol sequence:
 * @code
 * 
 * - result (t)ype    : result type         2 bytes
 * - e(r)ror level    : error level         1 byte
 * - r(o)w count      : row count           4 bytes
 * - co(l)umn count   : column count        4 bytes
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
stlStatus cmlReadFetchResult( cmlHandle                        * aHandle,
                              stlBool                          * aIgnored,
                              dtlValueBlockList                * aValueBlockList,
                              stlInt64                         * aFirstRowIdxOfRowset,
                              stlInt64                         * aLastRowIdx,
                              stlInt8                          * aStatus,
                              stlBool                          * aCursorClosed,
                              cmlReallocLongVaryingMemCallback   aReallocMemCallback,
                              void                             * aCallbackContext,
                              stlErrorStack                    * aErrorStack )
{
    cmpCursor           sCursor;
    dtlValueBlockList * sColumn;
    stlInt16            sResultType;
    stlInt32            sColumnCount;
    stlInt32            sRowCount;
    dtlDataValue      * sDataValue;
    stlInt64            sRowIdx;
    stlInt32            i;
    dtlDataType         sDataType;
    stlErrorData        sErrorData;
    stlInt8             sErrorCount;
    stlInt32            sMessageLen;
    stlInt32            sDetailMessageLen;
    stlBool             sHasError = STL_FALSE;

    
    STL_TRY( cmpBeginReading( aHandle,
                              &sCursor,
                              aErrorStack )
             == STL_SUCCESS );

    CMP_READ_VAR_INT16( aHandle, &sCursor, &sResultType, aErrorStack );
    STL_DASSERT( sResultType == CML_COMMAND_FETCH );

    STL_TRY( cmpReadErrorResult( aHandle,
                                 aIgnored,
                                 &sCursor,
                                 aErrorStack )
             == STL_SUCCESS );

    STL_TRY_THROW( *aIgnored == STL_FALSE, RAMP_IGNORE );
    
    CMP_READ_VAR_INT32( aHandle, &sCursor, &sColumnCount, aErrorStack );
    CMP_READ_VAR_INT64( aHandle, &sCursor, aFirstRowIdxOfRowset, aErrorStack );

    sRowIdx = DTL_GET_BLOCK_USED_CNT( aValueBlockList );

    /**
     * copy column data
     */
    while( STL_TRUE )
    {
        CMP_READ_VAR_INT32( aHandle, &sCursor, &sRowCount, aErrorStack );
        if( sRowCount <= 0 )
        {
            /* row count가 0이거나 -1(fetch 도중에 error)이면 while loop를 중단한다. */
            break;
        }

        for( i = 0; i < sRowCount; i++ )
        {
            CMP_READ_INT8( aHandle, &sCursor, aStatus, aErrorStack );

            if( *aStatus == CML_CURSOR_ROW_STATUS_DELETED )
            {
                sColumn = NULL;
            }
            else
            {
                sColumn = aValueBlockList;
            }
    
            while( sColumn != NULL )
            {
                sDataValue = &(sColumn->mValueBlock->mDataValue[sRowIdx]);

                CMP_READ_VAR_LEN_ENDIAN( aHandle,
                                         &sCursor,
                                         sDataValue->mLength,
                                         aErrorStack );

                if( sDataValue->mLength > sDataValue->mBufferSize )
                {
                    /**
                     * 메모리가 부족하다면 ...
                     */
            
                    STL_DASSERT( (sDataValue->mType == DTL_TYPE_LONGVARCHAR) ||
                                 (sDataValue->mType == DTL_TYPE_LONGVARBINARY) );
                    
                    STL_TRY( aReallocMemCallback( aHandle,
                                                  sDataValue,
                                                  aCallbackContext,
                                                  aErrorStack )
                             == STL_SUCCESS );

                    STL_DASSERT( sDataValue->mBufferSize >= sDataValue->mLength );
                }

                CMP_READ_BYTES( aHandle,
                                &sCursor,
                                sDataValue->mValue,
                                sDataValue->mLength,
                                aErrorStack );

                if( CMP_IS_EQUAL_ENDIAN( aHandle ) == STL_FALSE )
                {
                    sDataType = sDataValue->mType;
                    STL_TRY( dtlReverseByteOrderValue( sDataType,
                                                       sDataValue->mValue,
                                                       STL_FALSE,
                                                       aErrorStack )
                             == STL_SUCCESS );
                }
                
                sColumn = sColumn->mNext;
            }

            sRowIdx++;
        }
    }

    if( sRowCount == 0 )
    {
        CMP_READ_VAR_INT64( aHandle, &sCursor, aLastRowIdx, aErrorStack );
        CMP_READ_INT8( aHandle, &sCursor, aCursorClosed, aErrorStack );
    }
    else
    {
        /* fetch 도중에 에러가 발생한 경우이다. */
        sHasError = STL_TRUE;
        
        CMP_READ_INT8( aHandle, &sCursor, &sErrorCount, aErrorStack );

        for( i = 0; i < sErrorCount; i++ )
        {
            sErrorData.mErrorLevel = STL_ERROR_LEVEL_ABORT;
            sErrorData.mSystemErrorCode = 0;
            sErrorData.mDetailErrorMessage[0] = '\0';

            CMP_READ_INT32_ENDIAN( aHandle, &sCursor, &sErrorData.mExternalErrorCode, aErrorStack );
            CMP_READ_INT32_ENDIAN( aHandle, &sCursor, &sErrorData.mErrorCode, aErrorStack );
            CMP_READ_VAR_STRING_ENDIAN( aHandle,
                                        &sCursor,
                                        sErrorData.mErrorMessage,
                                        sMessageLen,
                                        STL_MAX_ERROR_MESSAGE,
                                        aErrorStack );
            CMP_READ_VAR_STRING_ENDIAN( aHandle,
                                        &sCursor,
                                        sErrorData.mDetailErrorMessage,
                                        sDetailMessageLen,
                                        STL_MAX_ERROR_MESSAGE,
                                        aErrorStack );

            stlPushErrorData( &sErrorData, aErrorStack );
        }
    }

    DTL_SET_ONE_BLOCK_USED_CNT( aValueBlockList, sRowIdx );

    STL_TRY( cmpEndReading( aHandle,
                            &sCursor,
                            aErrorStack )
             == STL_SUCCESS );

    STL_TRY( sHasError == STL_FALSE );

    return STL_SUCCESS;

    STL_CATCH( RAMP_IGNORE )
    {
        STL_ASSERT( cmpEndReading( aHandle,
                                   &sCursor,
                                   aErrorStack ) == STL_SUCCESS );
        return STL_SUCCESS;
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
