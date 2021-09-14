/*******************************************************************************
 * ellDictUserSchemaPath.c
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


/**
 * @file ellDictUserSchemaPath.c
 * @brief User Schema Path Dictionary 관리 모듈
 */

#include <ell.h>
#include <elDef.h>

#include <eldt.h>
#include <eldc.h>
#include <eldDictionary.h>

#include <eldUserSchemaPath.h>


/**
 * @addtogroup ellObjectAuth
 * @{
 */

/**
 * @brief User Schema Path Descriptor 를 Dictionary 에 추가한다.
 * @param[in]  aTransID            Transaction ID
 * @param[in]  aStmt               Statement
 * @param[in]  aAuthID             User 또는 PUBLIC 의 Authorization ID
 *                                 <BR> Role 의 Authorization ID 를 사용하면 안됨.
 * @param[in]  aSchemaID           Schema ID
 * @param[in]  aSearchOrder        Schema 이름 없이 사용되는 Table의
 *                                 <BR> Name Resolution을 위해 Schema 를 검색할 순서.
 *                                 <BR> (0부터 시작, DB에는 1부터 저장됨)
 * @param[in]  aEnv                Execution Library Environment
 * @remarks
 */
stlStatus ellInsertUserSchemaPathDesc( smlTransId             aTransID,
                                       smlStatement         * aStmt,
                                       stlInt64               aAuthID,
                                       stlInt64               aSchemaID,
                                       stlInt32               aSearchOrder,
                                       ellEnv               * aEnv )
{
    STL_TRY( eldInsertUserSchemaPathDesc( aTransID,
                                          aStmt,
                                          aAuthID,
                                          aSchemaID,
                                          aSearchOrder,
                                          aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}






/** @} ellObjectAuth */
