/*******************************************************************************
 * cmlGetOutBindData.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: cmlSetConnAttr.c 6676 2012-12-13 08:39:46Z egon $
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
#include <cmpCursor.h>
#include <cmpProtocol.h>
#include <cmgMemory.h>

/**
 * @file cmlGetOutBindData.c
 * @brief Communication Layer GetOutBindData Protocol Routines
 */

/**
 * @addtogroup cmlProtocol
 * @{
 */

/**
 * @brief GetOutBindData Command Protocol을 기록한다.
 * @param[in] aHandle        Communication Handle
 * @param[in] aControl       Protocol Control
 * @param[in] aStatementId   Statement Identifier
 * @param[in] aColumnNumber  Column Number
 * @param[in] aErrorStack    Error Stack Pointer
 * 
 * @par protocol sequence:
 * @code
 * 
 * - command (t)ype    : command type         var_int bytes
 * - (p)eriod          : protocol period      1 byte
 * - statement (i)d    : statement identifier var_int,var_int bytes
 * - column (n)umber   : column number        var_int byte
 *
 * field sequence : t-p-i-n
 * 
 * @endcode
 */
stlStatus cmlWriteGetOutBindDataCommand( cmlHandle      * aHandle,
                                         stlInt8          aControl,
                                         stlInt64         aStatementId,
                                         stlInt32         aColumnNumber,
                                         stlErrorStack  * aErrorStack )
{
    stlInt16    sCommandType = CML_COMMAND_GETOUTBINDDATA;
    cmpCursor   sCursor;

    STL_TRY( cmpBeginWriting( aHandle,
                              &sCursor,
                              aErrorStack )
             == STL_SUCCESS );

    CMP_WRITE_VAR_INT16( aHandle, &sCursor, &sCommandType, aErrorStack );
    CMP_WRITE_INT8( aHandle, &sCursor, &aControl, aErrorStack );
    CMP_WRITE_STATEMENT_ID_ENDIAN( aHandle, &sCursor, &aStatementId, aErrorStack );
    CMP_WRITE_VAR_INT32( aHandle, &sCursor, &aColumnNumber, aErrorStack );

    STL_TRY( cmpEndWriting( aHandle,
                            &sCursor,
                            aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief GetOutBindData Result Protocol을 읽는다.
 * @param[in]  aHandle               communication handle
 * @param[out] aIgnored              Ignore protocol 여부
 * @param[out] aBindValue            data value pointer
 * @param[in]  aReallocMemCallback   long varying memory allocator
 * @param[in]  aCallbackContext      callback context ( statement handle )
 * @param[in]  aErrorStack           error stack pointer
 * 
 * @par protocol sequence:
 * @code
 * 
 * - result (t)ype             : result type             var_int bytes
 * - e(r)ror level             : error level             1 byte
 * - column t(y)pe             : column type             1 byte
 * - column (v)alue            : column value            variable
 * - error (c)ount             : error count             1 byte
 * - (s)ql state               : sql state               4 bytes
 * - n(a)tive error            : native error            4 bytes
 * - (m)essage text            : message text            variable
 *
 * field sequence : if r == 0 then t-r-y-v
 *                  else if r == 1 then t-r-c-[s-a-m]^c-y-v
 *                  else t-r-c-[s-a-m]^c
 * 
 * @endcode
 */
stlStatus cmlReadGetOutBindDataResult( cmlHandle                        * aHandle,
                                       stlBool                          * aIgnored,
                                       dtlDataValue                     * aBindValue,
                                       cmlReallocLongVaryingMemCallback   aReallocMemCallback,
                                       void                             * aCallbackContext,
                                       stlErrorStack                    * aErrorStack )
{
    stlInt16    sResultType;
    cmpCursor   sCursor;
    stlInt8     sBindType;

    STL_TRY( cmpBeginReading( aHandle,
                              &sCursor,
                              aErrorStack )
             == STL_SUCCESS );

    CMP_READ_VAR_INT16( aHandle, &sCursor, &sResultType, aErrorStack );
    STL_DASSERT( sResultType == CML_COMMAND_GETOUTBINDDATA );
    
    STL_TRY( cmpReadErrorResult( aHandle,
                                 aIgnored,
                                 &sCursor,
                                 aErrorStack )
             == STL_SUCCESS );

    STL_TRY_THROW( *aIgnored == STL_FALSE, RAMP_IGNORE );
    
    CMP_READ_INT8( aHandle, &sCursor, &sBindType, aErrorStack );

    aBindValue->mType = sBindType;

    CMP_READ_VAR_LEN_ENDIAN( aHandle,
                             &sCursor,
                             aBindValue->mLength,
                             aErrorStack );

    if( aBindValue->mLength > aBindValue->mBufferSize )
    {
        /**
         * 메모리가 부족하다면 ...
         */
            
        STL_DASSERT( (sBindType == DTL_TYPE_LONGVARCHAR) ||
                     (sBindType == DTL_TYPE_LONGVARBINARY) );
                    
        STL_TRY( aReallocMemCallback( aHandle,
                                      aBindValue,
                                      aCallbackContext,
                                      aErrorStack )
                 == STL_SUCCESS );

        STL_DASSERT( aBindValue->mBufferSize >= aBindValue->mLength );
    }

    CMP_READ_BYTES( aHandle,
                    &sCursor,
                    aBindValue->mValue,
                    aBindValue->mLength,
                    aErrorStack );

    if( CMP_IS_EQUAL_ENDIAN( aHandle ) == STL_FALSE )
    {
        STL_TRY( dtlReverseByteOrderValue( sBindType,
                                           aBindValue->mValue,
                                           STL_FALSE,
                                           aErrorStack )
                 == STL_SUCCESS );
    }
    
    STL_TRY( cmpEndReading( aHandle,
                            &sCursor,
                            aErrorStack )
             == STL_SUCCESS );

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
