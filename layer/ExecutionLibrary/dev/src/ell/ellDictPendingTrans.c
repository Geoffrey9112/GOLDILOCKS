/*******************************************************************************
 * ellDictPendingTrans.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: ellDictPendingTrans.c 8946 2013-07-05 08:31:17Z leekmo $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


/**
 * @file ellDictPendingTrans.c
 * @brief PENDING_TRANS 관리 모듈
 */

#include <ell.h>
#include <elDef.h>

#include <eldt.h>
#include <eldc.h>
#include <eldDictionary.h>


/**
 * @addtogroup ellDictPendingTrans
 * @{
 */


/**
 * @brief PENDING_TRANS에 레코드를 삽입한다.
 * @param[in]  aTransID                 Transaction ID
 * @param[in]  aStmt                    Statement
 * @param[in]  aXidString               XID string
 * @param[in]  aInDoubtTransID          IN-DOUBT Transaction ID
 * @param[in]  aComments                Commit Comments
 * @param[in]  aHeuristicDecision       Heuristic Decision( TRUE:COMMIT, FALSE:ROLLBACK )
 * @param[in]  aEnv                     Execution Library Environment
 * @remarks
 */

stlStatus ellInsertPendingTrans( smlTransId              aTransID,
                                 smlStatement          * aStmt,
                                 stlChar               * aXidString,
                                 smlTransId              aInDoubtTransID,
                                 stlChar               * aComments,
                                 stlBool                 aHeuristicDecision,
                                 ellEnv                * aEnv )
{
    knlValueBlockList        * sDataValueList  = NULL;

    smlRid                 sRowRid;
    smlScn                 sRowScn;
    smlRowBlock            sRowBlock;
    eldMemMark             sMemMark;
    stlTime                sFailTime;

    stlInt32 sState = 0;
    
    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aXidString != NULL, ELL_ERROR_STACK(aEnv) );

    /**************************************************************
     * Record 추가
     **************************************************************/
    
    /**
     * - Data Value List와 Row Block을 위한 공간 할당 및 초기화
     */
    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_PENDING_TRANS,
                                     & sMemMark,
                                     & sDataValueList,
                                     aEnv )
             == STL_SUCCESS );
    sState = 1;

    STL_TRY( smlInitSingleRowBlock( &sRowBlock,
                                    &sRowRid,
                                    &sRowScn,
                                    SML_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * - database privilege descriptor의 정보를 구성한다.
     */

    /*
     * GLOBAL_TRANS_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_PENDING_TRANS,
                                ELDT_PendingTrans_ColumnOrder_GLOBAL_TRANS_ID,
                                sDataValueList,
                                aXidString,
                                aEnv )
             == STL_SUCCESS );
    
    /*
     * LOCAL_TRANS_ID
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_PENDING_TRANS,
                                ELDT_PendingTrans_ColumnOrder_LOCAL_TRANS_ID,
                                sDataValueList,
                                & aInDoubtTransID,
                                aEnv )
             == STL_SUCCESS );

    /*
     * HEURISTIC_DECISION
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_PENDING_TRANS,
                                ELDT_PendingTrans_ColumnOrder_HEURISTIC_DECISION,
                                sDataValueList,
                                & aHeuristicDecision,
                                aEnv )
             == STL_SUCCESS );

    /*
     * FAIL_TIME
     */

    sFailTime = stlNow();

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_PENDING_TRANS,
                                ELDT_PendingTrans_ColumnOrder_FAIL_TIME,
                                sDataValueList,
                                & sFailTime,
                                aEnv )
             == STL_SUCCESS );
    
    /*
     * COMMENTS
     */

    STL_TRY( eldSetColumnValue( ELDT_TABLE_ID_PENDING_TRANS,
                                ELDT_PendingTrans_ColumnOrder_COMMENTS,
                                sDataValueList,
                                aComments,
                                aEnv )
             == STL_SUCCESS );
    
    /**
     * - 레코드를 넣는다.
     */

    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sDataValueList, 0, 1 );
    
    STL_TRY( smlInsertRecord( aStmt,
                              gEldTablePhysicalHandle[ELDT_TABLE_ID_PENDING_TRANS],
                              sDataValueList,
                              NULL, /* unique violation */
                              & sRowBlock,
                              SML_ENV( aEnv ) )
             == STL_SUCCESS );

    /**
     * 사용한 메모리를 해제한다.
     */
    
    sState = 0;
    STL_TRY( eldFreeTableValueList( & sMemMark, aEnv ) == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    switch (sState)
    {
        case 1:
            (void) eldFreeTableValueList( & sMemMark, aEnv );
        default:
            break;
    }
    
    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/** @} ellDictPendingTrans */

