/*******************************************************************************
 * kniDual.c
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
 * @file kniDual.c
 * @brief Kernel Layer Dual Table Internal Routines
 */

#include <stl.h>
#include <dtl.h>
#include <knlDef.h>
#include <knDef.h>
#include <knlFixedTable.h>

/**
 * @addtogroup kniDual
 * @{
 */

/**
 * @brief DUAL Table 정의
 */
knlFxColumnDesc gKnDualColumnDesc[] =
{
    {
        "DUMMY",
        "dummy column",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( kniDualFxRecord, mDummy ),
        1,
        STL_FALSE  /* nullable */
    },
    {
        NULL,
        NULL,
        0,
        0,
        0,
        STL_FALSE  /* nullable */
    }
};

/**
 * @brief DUAL Table의 open 함수
 * @param[in] aStmt Statement Pointer
 * @param[in] aDumpObjHandle Dump Object의 Handle
 * @param[in] aPathCtrl Path Controller
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus openFxKnDualCallback( void   * aStmt,
                                void   * aDumpObjHandle,
                                void   * aPathCtrl,
                                knlEnv * aEnv )
{
    kniDualPathControl * sCtl = (kniDualPathControl*)aPathCtrl;

    sCtl->mRowFetched = STL_FALSE;

    return STL_SUCCESS;
}

/**
 * @brief DUAL Table의 close 함수
 * @param[in] aDumpObjHandle Dump Object의 Handle
 * @param[in] aPathCtrl Path Controller
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus closeFxKnDualCallback( void   * aDumpObjHandle,
                                void   * aPathCtrl,
                                knlEnv * aEnv )
{
    return STL_SUCCESS;
}

/**
 * @brief DUAL Table의 레코드 구축 함수
 * @param[in] aDumpObjHandle Dump Object Handle
 * @param[in] aPathCtrl      Path Controller
 * @param[in,out] aValueList 값을 담을 공간
 * @param[in]  aBlockIdx   Block Idx
 * @param[out] aTupleExist 레코드 존재 여부
 * @param[in,out] aEnv 커널 Environment
 */
stlStatus buildRecordFxKnDualCallback( void              * aDumpObjHandle,
                                       void              * aPathCtrl,
                                       knlValueBlockList * aValueList,
                                       stlInt32            aBlockIdx,
                                       stlBool           * aTupleExist,
                                       knlEnv            * aEnv )
{
    kniDualPathControl * sCtl = (kniDualPathControl*)aPathCtrl;
    kniDualFxRecord      sRec;

    *aTupleExist = STL_FALSE;

    if( sCtl->mRowFetched == STL_FALSE )
    {
        sRec.mDummy[0] = 'X';
        sRec.mDummy[1] = '\0';
        
        STL_TRY( knlBuildFxRecord( gKnDualColumnDesc,
                                   &sRec,
                                   aValueList,
                                   aBlockIdx,
                                   aEnv ) == STL_SUCCESS );
        *aTupleExist = STL_TRUE;
        sCtl->mRowFetched = STL_TRUE;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief X$KN_ENV Table 정보
 */
knlFxTableDesc gKnDualTableDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,
    KNL_STARTUP_PHASE_NO_MOUNT,
    NULL,
    openFxKnDualCallback,
    closeFxKnDualCallback,
    buildRecordFxKnDualCallback,
    STL_SIZEOF( kniDualFxRecord ),
    "DUAL",
    "dual fixed table",
    gKnDualColumnDesc
};

/** @} */
