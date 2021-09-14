/*******************************************************************************
 * zlpeSqlca.h
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

#ifndef _ZLPESQLCA_H_
#define _ZLPESQLCA_H_ 1

/**
 * @file zlpeSqlca.h
 * @brief Gliese Embedded SQL in C precompiler library internal header file.
 */

#include <stl.h>
#include <goldilocks.h>
#include <goldilocksesql.h>
#include <sqlca.h>
#include <zlplDef.h>

#define GOLDILOCKSESQL_SQLCA_SET_ERROR( aSqlContext, aSqlArgs )              \
    {                                                                   \
        if ( aSqlArgs->sql_ca->sqlcode == 0 )                           \
        {                                                               \
            if ( aSqlContext == NULL )                                  \
            {                                                           \
                (void) zlpeInternalToSqlca( &gErrorStack,               \
                                            (zlplSqlca *)aSqlArgs->sql_ca, \
                                            (stlChar *)aSqlArgs->sql_state); \
                STL_INIT_ERROR_STACK( &gErrorStack );                   \
            }                                                           \
            else                                                        \
            {                                                           \
                (void) zlpeInternalToSqlca( ZLPL_CTXT_ERROR_STACK((zlplSqlContext *)aSqlContext), \
                                            (zlplSqlca *)aSqlArgs->sql_ca, \
                                            (stlChar *)aSqlArgs->sql_state); \
            }                                                           \
        }                                                               \
        else                                                            \
        {                                                               \
            /* ODBC 함수 호출을 통해 Error 가 설정됨    */              \
        }                                                               \
    }

/************************************************************
 * SQLCA
 ************************************************************/

stlStatus zlpeIsInit();
stlStatus zlpeCheckError(SQLRETURN    sReturn,
                         SQLSMALLINT  aHandleType,
                         SQLHANDLE    aHandle,
                         zlplSqlca   *aSqlca,
                         stlChar     *aSqlState);
stlStatus zlpeSetException(SQLSMALLINT   aHandleType,
                           SQLHANDLE     aHandle,
                           zlplSqlca    *aSqlca,
                           stlChar      *aSqlState);
stlStatus zlpeInternalToSqlca(stlErrorStack *aErrorStack,
                              zlplSqlca     *aSqlca,
                              stlChar       *aSqlState);
stlStatus zlpeSetSuccess(zlplSqlca *aSqlca);

stlStatus zlpeSetErrorMsg(zlplSqlca *aSqlca, stlChar *aMsg);
stlStatus zlpeSetSqlCode(zlplSqlca *aSqlca, stlInt32 aSqlCode);
stlStatus zlpeSetProcessRowCount(zlplSqlca *aSqlca, stlInt32 aRowCount);
stlStatus zlpeSetWarnStringTruncate(zlplSqlca *aSqlca);
stlStatus zlpeSetErrorConnectionDoesNotExist(zlplSqlca *aSqlca);


#endif /* _ZLPESQLCA_H_ */
