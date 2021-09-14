/*******************************************************************************
 * sclBindData.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: sclBindData.c 6676 2012-12-13 08:39:46Z egon $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#include <stl.h>
#include <dtl.h>
#include <knl.h>
#include <sclDef.h>
#include <scDef.h>
#include <scpProtocol.h>
#include <sclGeneral.h>
#include <sccCommunication.h>


/**
 * @brief BindData Command Protocol을 읽는다.
 * @param[in]  aHandle              communication handle
 * @param[out] aControl             Protocol Control
 * @param[out] aProtocolError       Protocol Error
 * @param[in]  aCheckControlCallback check control function pointer
 * @param[in]  aGetValueCallback    bind value function pointer
 * @param[in]  aReallocMemCallback  long varying memory function pointer
 * @param[in]  aEnv                 Environment Pointer
 * 
 * @par protocol sequence:
 * @code
 * 
 * - command (t)ype   : command type            var_int bytes
 * - (p)eriod         : protocol period         1 byte
 * - (s)tatement id   : statement identifier    var_int,var_int bytes
 * - column count(n)  : column count            var_int bytes
 * - (a)rray size     : array size              var_int byte
 * - (b)ind type      : bind type               1 byte
 * - column (v)alue   : column value            valiable
 *
 * field sequence : t-p-s-n-a-[b-[v]^a]^n
 * 
 * @endcode
 */
stlStatus sclReadBindDataCommand( sclHandle                        * aHandle,
                                  stlInt8                          * aControl,
                                  stlBool                          * aProtocolError,
                                  sclCheckControlCallback            aCheckControlCallback,
                                  sclGetBindValueCallback            aGetValueCallback,
                                  sclReallocLongVaryingMemCallback   aReallocMemCallback,
                                  sclEnv                           * aEnv )
{
    stlInt16       sResultType;
    scpCursor      sCursor;
    stlInt8        sBindType;
    stlInt32       i, j;
    stlInt32       sColumnCount;
    dtlDataValue * sBindValue = NULL;
    stlInt64       sArraySize;
    stlInt64       sLength;
    stlInt64       sStatementId;
    stlBool        sIgnore;

    STL_TRY( scpBeginReading( aHandle,
                              &sCursor,
                              aEnv )
             == STL_SUCCESS );

    SCP_READ_VAR_INT16( aHandle, &sCursor, &sResultType, aEnv );
    STL_DASSERT( sResultType == CML_COMMAND_BINDDATA );
    SCP_READ_INT8( aHandle, &sCursor, aControl, aEnv );

    SCP_READ_STATEMENT_ID( aHandle, &sCursor, &sStatementId, aEnv );
    SCP_READ_VAR_INT32( aHandle, &sCursor, &sColumnCount, aEnv );
    SCP_READ_VAR_INT64( aHandle, &sCursor, &sArraySize, aEnv );


    aCheckControlCallback( aHandle,
                           &sStatementId,
                           *aControl,
                           &sIgnore,
                           aProtocolError,
                           aEnv );

    STL_DASSERT( (sStatementId != -1) || (SCL_IS_HEAD(*aControl) == STL_FALSE) );

    for( i = 1; i <= sColumnCount; i++ )
    {
        SCP_READ_INT8( aHandle, &sCursor, &sBindType, aEnv );

        if( sBindType == SCP_BIND_DATA_TYPE_FOR_OUT_BIND )
        {
            /* out bind 등의 컬럼은 skip한다. */
            continue;
        }

        sBindValue = NULL; /* 매 컬럼마다 NULL로 초기화해야 한다. */
        if( sIgnore == STL_FALSE )
        {
            STL_TRY( aGetValueCallback( aHandle,
                                        sStatementId,
                                        i,
                                        *aControl,
                                        &sBindValue,
                                        aEnv )
                     == STL_SUCCESS );
        }

        if( sBindValue != NULL )
        {
            for( j = 0; j < sArraySize; j++ )
            {
                sBindValue[j].mType = sBindType;

                SCP_READ_VAR_LEN( aHandle, &sCursor, sBindValue[j].mLength, aEnv );
        
                if( sBindValue[j].mLength > sBindValue[j].mBufferSize )
                {
                    /**
                     * 메모리가 부족하다면 ...
                     */
            
                    STL_DASSERT( (sBindValue[j].mType == DTL_TYPE_LONGVARCHAR) ||
                                 (sBindValue[j].mType == DTL_TYPE_LONGVARBINARY) );
            
                    STL_TRY( aReallocMemCallback( aHandle,
                                                  &sBindValue[j],
                                                  NULL,
                                                  aEnv )
                             == STL_SUCCESS );

                    STL_DASSERT( sBindValue[j].mLength <= sBindValue[j].mBufferSize );
                }

                SCP_READ_BYTES( aHandle,
                                &sCursor,
                                sBindValue[j].mValue,
                                sBindValue[j].mLength,
                                aEnv );
            }
        }
        else
        {
            /*
             * IGNORE 상황이라 데이터를 소모한다.
             */
            for( j = 0; j < sArraySize; j++ )
            {
                SCP_READ_VAR_LEN( aHandle, &sCursor, sLength, aEnv );
                SCP_SKIP_BYTES( aHandle,
                                &sCursor,
                                sLength,
                                aEnv );
            }        
        }
    }

    STL_TRY( scpEndReading( aHandle,
                            &sCursor,
                            aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief BindData Result Protocol을 기록한다.
 * @param[in] aHandle        Communication Handle
 * @param[in] aErrorStack    기록할 Error Stack
 * @param[in] aEnv           env
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
stlStatus sclWriteBindDataResult( sclHandle         * aHandle,
                                  stlErrorStack     * aErrorStack,
                                  sclEnv            * aEnv )
{
    scpCursor    sCursor;
    stlInt16     sResultType = CML_COMMAND_BINDDATA;

    STL_TRY( scpBeginWriting( aHandle,
                              &sCursor,
                              aEnv )
             == STL_SUCCESS );

    SCP_WRITE_VAR_INT16( aHandle, &sCursor, &sResultType, aEnv );

    STL_TRY( scpWriteErrorResult( aHandle,
                                  &sCursor,
                                  aErrorStack,
                                  aEnv ) == STL_SUCCESS );

    STL_TRY( scpEndWriting( aHandle,
                            &sCursor,
                            aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/** @} */
