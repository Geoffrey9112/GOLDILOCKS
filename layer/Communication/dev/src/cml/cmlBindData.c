/*******************************************************************************
 * cmlBindData.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: cmlBindData.c 6676 2012-12-13 08:39:46Z egon $
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
 * @file cmlBindData.c
 * @brief Communication Layer BindData Protocol Routines
 */

/**
 * @addtogroup cmlProtocol
 * @{
 */

/**
 * @brief BindData Command Protocol의 앞부분을 기록한다.
 * @param[in] aHandle       communication handle
 * @param[in] aControl      Protocol Control
 * @param[in] aStatementId  statement identifier
 * @param[in] aColumnCount  column count
 * @param[in] aArraySize    array size
 * @param[in] aErrorStack   error stack pointer
 * 
 * @par protocol sequence:
 * @code
 * 
 * - command (t)ype   : command type            var_int bytes
 * - (p)eriod         : protocol period         1 byte
 * - (s)tatement id   : statement identifier    var_int,var_int bytes
 * - column count(n)  : column count            var_int byte
 * - (a)rray size     : array size              var_int byte
 * - (b)ind type      : bind type               1 byte
 * - column (v)alue   : column value            valiable
 *
 * field sequence : t-p-s-n-a-[b-[v]^a]^n
 * 
 * @endcode
 */
stlStatus cmlWriteBindDataBeginCommand( cmlHandle      * aHandle,
                                        stlInt8          aControl,
                                        stlInt64         aStatementId,
                                        stlInt32         aColumnCount,
                                        stlInt64         aArraySize,
                                        stlErrorStack  * aErrorStack )
{
    stlInt16    sCommandType = CML_COMMAND_BINDDATA;
    cmpCursor   sCursor;

    STL_TRY( cmpBeginWriting( aHandle,
                              &sCursor,
                              aErrorStack )
             == STL_SUCCESS );

    CMP_WRITE_VAR_INT16( aHandle, &sCursor, &sCommandType, aErrorStack );
    CMP_WRITE_INT8( aHandle, &sCursor, &aControl, aErrorStack );
    CMP_WRITE_STATEMENT_ID_ENDIAN( aHandle, &sCursor, &aStatementId, aErrorStack );
    CMP_WRITE_VAR_INT32( aHandle, &sCursor, &aColumnCount, aErrorStack );
    CMP_WRITE_VAR_INT64( aHandle, &sCursor, &aArraySize, aErrorStack );

    STL_TRY( cmpEndWriting( aHandle,
                            &sCursor,
                            aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief BindData Command Protocol을 기록한다.
 * @param[in] aHandle       communication handle
 * @param[in] aOutBind      out bind 유무
 * @param[in] aArraySize    array size
 * @param[in] aBindValue    bind value
 * @param[in] aErrorStack   error stack pointer
 * 
 * @par protocol sequence:
 * @code
 * 
 * - (b)ind type      : bind type               1 byte
 * - column (v)alue   : column value            valiable
 *
 * field sequence : b-[v]^a
 * 
 * @endcode
 */
stlStatus cmlWriteBindDataCommand( cmlHandle      * aHandle,
                                   stlBool          aOutBind,
                                   stlInt64         aArraySize,
                                   dtlDataValue   * aBindValue,
                                   stlErrorStack  * aErrorStack )
{
    cmpCursor   sCursor;
    stlInt8     sBindType = aBindValue->mType;
    stlInt32    i;

    STL_TRY( cmpBeginWriting( aHandle,
                              &sCursor,
                              aErrorStack )
             == STL_SUCCESS );

    if( aOutBind == STL_TRUE )
    {
        sBindType = CMP_BIND_DATA_TYPE_FOR_OUT_BIND;
        CMP_WRITE_INT8( aHandle, &sCursor, &sBindType, aErrorStack );
    }
    else
    {
        CMP_WRITE_INT8( aHandle, &sCursor, &sBindType, aErrorStack );

        for( i = 0; i < aArraySize; i++ )
        {
            if( CMP_IS_EQUAL_ENDIAN( aHandle ) == STL_FALSE )
            {
                STL_TRY( dtlReverseByteOrderValue( sBindType,
                                                   aBindValue[i].mValue,
                                                   STL_FALSE,
                                                   aErrorStack )
                         == STL_SUCCESS );
            }
                
            CMP_WRITE_N_VAR_ENDIAN( aHandle,
                                    &sCursor,
                                    aBindValue[i].mValue,
                                    aBindValue[i].mLength,
                                    aErrorStack );
        }
    }

    STL_TRY( cmpEndWriting( aHandle,
                            &sCursor,
                            aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief BindData Result Protocol을 읽는다.
 * @param[in]  aHandle         Communication Handle
 * @param[out] aIgnored        Ignore protocol 여부
 * @param[in]  aErrorStack     Error Stack Pointer
 * 
 * @par protocol sequence:
 * @code
 * 
 * - result (t)ype         : result type         var_int bytes
 * - e(r)ror level         : error level         1 byte
 * - error (c)ount         : error count         1 byte
 * - (s)ql state           : sql state           4 bytes
 * - n(a)tive error        : native error        4 bytes
 * - (m)essage text        : message text        variable
 *
 * field sequence : if r == 0 then t-r
 *                  else if r == 1 then t-r-c-[s-a-m]^c
 *                  else t-r-c-[s-a-m]^c
 * 
 * @endcode
 */
stlStatus cmlReadBindDataResult( cmlHandle      * aHandle,
                                 stlBool        * aIgnored,
                                 stlErrorStack  * aErrorStack )
{
    cmpCursor    sCursor;
    stlInt16     sResultType;

    STL_TRY( cmpBeginReading( aHandle,
                              &sCursor,
                              aErrorStack )
             == STL_SUCCESS );

    CMP_READ_VAR_INT16( aHandle, &sCursor, &sResultType, aErrorStack );
    STL_DASSERT( sResultType == CML_COMMAND_BINDDATA );

    STL_TRY( cmpReadErrorResult( aHandle,
                                 aIgnored,
                                 &sCursor,
                                 aErrorStack )
             == STL_SUCCESS );

    STL_TRY_THROW( *aIgnored == STL_FALSE, RAMP_IGNORE );

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
