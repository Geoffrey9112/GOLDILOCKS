/*******************************************************************************
 * zlcsStmt.c
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

#include <cml.h>
#include <goldilocks.h>

#include <zlDef.h>
#include <zle.h>
#include <zld.h>

/**
 * @file zlcsStmt.c
 * @brief ODBC API Internal Statement Routines.
 */

/**
 * @addtogroup zlcsStmt
 * @{
 */

stlStatus zlcsAlloc( zlcDbc        * aDbc,
                     zlsStmt       * aStmt,
                     stlErrorStack * aErrorStack )
{
    STL_TRY( stlCreateRegionAllocator( &aStmt->mResult.mAllocator,
                                       ZL_REGION_INIT_SIZE,
                                       ZL_REGION_NEXT_SIZE,
                                       aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zlcsFree( zlcDbc            * aDbc,
                    zlsStmt           * aStmt,
                    zllFreeStmtOption   aOption,
                    stlBool             aIsCleanup,
                    stlErrorStack     * aErrorStack )
{
    cmlHandle * sHandle;
    stlBool     sIgnored = STL_FALSE;
    stlInt32    sState = 0;

    if( (aIsCleanup == STL_FALSE) && (aStmt->mStmtId != ZLS_INVALID_STATEMENT_ID) )
    {
        sHandle = &aDbc->mComm;
    
        sState = 1;
        STL_TRY( cmlWriteFreeStmtCommand( sHandle,
                                          CML_CONTROL_HEAD | CML_CONTROL_TAIL,
                                          aStmt->mStmtId,
                                          aOption,
                                          aErrorStack ) == STL_SUCCESS );

        STL_TRY( cmlSendPacket( sHandle,
                                aErrorStack ) == STL_SUCCESS );

        STL_TRY( cmlReadFreeStmtResult( sHandle,
                                        &sIgnored,
                                        aErrorStack ) == STL_SUCCESS );
    }

    if( aOption == ZLL_FREE_STMT_OPTION_DROP )
    {
        sState = 0;
        STL_TRY( stlDestroyRegionAllocator( &aStmt->mResult.mAllocator,
                                            aErrorStack ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1 :
            if( aOption == ZLL_FREE_STMT_OPTION_DROP )
            {
                (void)stlDestroyRegionAllocator( &aStmt->mResult.mAllocator, aErrorStack );
            }
        default :
            break;
    }

    return STL_FAILURE;
}

stlStatus zlcsGetAttr( zlcDbc           * aDbc,
                       zlsStmt          * aStmt,
                       zllStatementAttr   aAttr,
                       void             * aValue,
                       stlInt32           aBufferLen,
                       stlInt32         * aStrLen,
                       stlErrorStack    * aErrorStack )
{
    cmlHandle   * sHandle;
    stlInt32      sAttrType;
    dtlDataType   sAttrDataType;
    stlInt32      sAttrValueSize;
    stlInt32      sValue;
    stlBool       sIgnored;

    sHandle = &aDbc->mComm;

    STL_TRY( cmlWriteGetStmtAttrCommand( sHandle,
                                         CML_CONTROL_HEAD | CML_CONTROL_TAIL,
                                         aStmt->mStmtId,
                                         aAttr,
                                         aBufferLen,
                                         aErrorStack ) == STL_SUCCESS );

    STL_TRY( cmlSendPacket( sHandle,
                            aErrorStack ) == STL_SUCCESS );

    switch( aAttr )
    {
        case ZLL_STATEMENT_ATTR_PARAMSET_SIZE :
        case ZLL_STATEMENT_ATTR_CONCURRENCY :
        case ZLL_STATEMENT_ATTR_CURSOR_HOLDABILITY :
        case ZLL_STATEMENT_ATTR_CURSOR_SCROLLABLE :
        case ZLL_STATEMENT_ATTR_CURSOR_SENSITIVITY :
        case ZLL_STATEMENT_ATTR_CURSOR_TYPE :
        case ZLL_STATEMENT_ATTR_ATOMIC_EXECUTION :
        case ZLL_STATEMENT_ATTR_EXPLAIN_PLAN_OPTION :
        case ZLL_STATEMENT_ATTR_QUERY_TIMEOUT :
            STL_TRY( cmlReadGetStmtAttrResult( sHandle,
                                               &sIgnored,
                                               &aStmt->mStmtId,
                                               &sAttrType,
                                               &sAttrDataType,
                                               (void*)&sValue,
                                               &sAttrValueSize,
                                               aStrLen,
                                               aErrorStack ) == STL_SUCCESS );

            *(stlInt64*)aValue = sValue;
            break;
        case ZLL_STATEMENT_ATTR_EXPLAIN_PLAN_TEXT :
            STL_TRY( cmlReadGetStmtAttrResult( sHandle,
                                               &sIgnored,
                                               &aStmt->mStmtId,
                                               &sAttrType,
                                               &sAttrDataType,
                                               aValue,
                                               &sAttrValueSize,
                                               aStrLen,
                                               aErrorStack ) == STL_SUCCESS );
            break;
        default :
            STL_ASSERT( STL_FALSE );
            break;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus zlcsSetAttr( zlcDbc           * aDbc,
                       zlsStmt          * aStmt,
                       zllStatementAttr   aAttr,
                       stlInt64           aIntValue,
                       stlChar          * aStrValue,
                       stlInt32           aStrLength,
                       stlErrorStack *    aErrorStack )
{
    cmlHandle   * sHandle;
    stlBool       sIgnored;
    stlInt32      sValue;

    sHandle = &aDbc->mComm;

    switch( aAttr )
    {
        case ZLL_STATEMENT_ATTR_PARAMSET_SIZE :
        case ZLL_STATEMENT_ATTR_CONCURRENCY :
        case ZLL_STATEMENT_ATTR_CURSOR_HOLDABILITY :
        case ZLL_STATEMENT_ATTR_CURSOR_SCROLLABLE :
        case ZLL_STATEMENT_ATTR_CURSOR_SENSITIVITY :
        case ZLL_STATEMENT_ATTR_CURSOR_TYPE :
        case ZLL_STATEMENT_ATTR_ATOMIC_EXECUTION :
        case ZLL_STATEMENT_ATTR_EXPLAIN_PLAN_OPTION :
        case ZLL_STATEMENT_ATTR_QUERY_TIMEOUT :
            sValue = (stlInt32)aIntValue;
            
            STL_TRY( cmlWriteSetStmtAttrCommand( sHandle,
                                                 CML_CONTROL_HEAD | CML_CONTROL_TAIL,
                                                 aStmt->mStmtId,
                                                 aAttr,
                                                 DTL_TYPE_NATIVE_INTEGER,
                                                 &sValue,
                                                 0,
                                                 aErrorStack ) == STL_SUCCESS );

            STL_TRY( cmlSendPacket( sHandle,
                                    aErrorStack ) == STL_SUCCESS );

            STL_TRY( cmlReadSetStmtAttrResult( sHandle,
                                               &sIgnored,
                                               &aStmt->mStmtId,
                                               aErrorStack ) == STL_SUCCESS );
            break;
        default :
            STL_ASSERT( STL_FALSE );
            break;

    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
