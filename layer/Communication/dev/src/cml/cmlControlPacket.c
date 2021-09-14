/*******************************************************************************
 * cmlPeriod.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *
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
 * @file cmlControlPacket.c
 * @brief Communication Layer Cancel Protocol Routines
 */

/**
 * @addtogroup cmlProtocol
 * @{
 */

/**
 * @brief Period Command Protocol을 기록한다.
 * @param[in] aHandle       communication handle
 * @param[in] aControl      protocol control
 * @param[in] aControlOp    control packet의 operation code
 * @param[in] aErrorStack   error stack pointer
 *
 * @par protocol sequence:
 * @code
 *
 * - command (t)ype      : command type            2 bytes
 * 
 * field sequence : t
 *
 * @endcode
 */
stlStatus cmlWriteControlPacketCommand( cmlHandle      * aHandle,
                                        stlInt8          aControl,
                                        stlInt8          aControlOp,
                                        stlErrorStack  * aErrorStack )
{
    stlInt16   sCommandType = CML_COMMAND_CONTROLPACKET;
    cmpCursor  sCursor;
    
    STL_TRY( cmpBeginWriting( aHandle,
                              &sCursor,
                              aErrorStack )
             == STL_SUCCESS );

    CMP_WRITE_VAR_INT16( aHandle, &sCursor, &sCommandType, aErrorStack );
    CMP_WRITE_INT8( aHandle, &sCursor, &aControl, aErrorStack );
    CMP_WRITE_INT8( aHandle, &sCursor, &aControlOp, aErrorStack );
    
    STL_TRY( cmpEndWriting( aHandle,
                            &sCursor,
                            aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief ControlPacket Result Protocol을 읽는다.
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
stlStatus cmlReadControlPacketResult( cmlHandle      * aHandle,
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
    STL_DASSERT( sResultType == CML_COMMAND_CONTROLPACKET );

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
