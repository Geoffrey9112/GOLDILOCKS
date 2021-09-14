/*******************************************************************************
 * smqStartup.c
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

#include <stl.h>
#include <dtl.h>
#include <knl.h>
#include <scl.h>
#include <smlDef.h>
#include <smDef.h>
#include <sme.h>
#include <smf.h>
#include <smq.h>
#include <smg.h>
#include <smxl.h>
#include <smr.h>
#include <smqDef.h>

/**
 * @file smqStartup.c
 * @brief Storage Manager Layer Segment Startup Internal Routines
 */

smqWarmupEntry * gSmqWarmupEntry;

extern knlFxTableDesc gSmqSequenceTableDesc;
extern smrRedoVector  gSmqRedoVectors[];
extern smxlUndoFunc   gSmqUndoFuncs[];

/**
 * @addtogroup smq
 * @{
 */

/**
 * @brief Sequence Component를 NO MOUNT 모드로 전이한다.
 * @param[out] aWarmupEntry Sequence Component의 Warmup Entry 포인터 
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smqStartupNoMount( void  ** aWarmupEntry,
                             smlEnv * aEnv )
{
    smqWarmupEntry * sWarmupEntry;
    
    smrRegisterRedoVectors( SMG_COMPONENT_SEQUENCE,
                            gSmqRedoVectors );
    
    smxlRegisterUndoFuncs( SMG_COMPONENT_SEQUENCE, 
                           gSmqUndoFuncs );

    STL_TRY( smqRegisterFxTables( aEnv ) == STL_SUCCESS );
    
    STL_TRY( knlAllocFixedStaticArea( STL_SIZEOF( smqWarmupEntry ),
                                      (void**)&sWarmupEntry,
                                      (knlEnv*)aEnv ) == STL_SUCCESS );

    sWarmupEntry->mSequenceMapHandle = NULL;
    gSmqWarmupEntry = sWarmupEntry;
    *aWarmupEntry = (void*)sWarmupEntry;
    
    smgRegisterPendOp( SMG_PEND_DROP_SEQUENCE,
                       smqDropPending );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Sequence Component를 OPEN 모드로 전이한다.
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smqStartupOpen( smlEnv * aEnv )
{
    return STL_SUCCESS;
}

/**
 * @brief Sequence Component를 Warmup 시킨다.
 * @param[in] aWarmupEntry Sequence Component의 Warmup Entry 포인터 
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smqWarmup( void   * aWarmupEntry,
                     smlEnv * aEnv )
{
    gSmqWarmupEntry = (smqWarmupEntry*)aWarmupEntry;
    
    smrRegisterRedoVectors( SMG_COMPONENT_SEQUENCE,
                            gSmqRedoVectors );
    
    smxlRegisterUndoFuncs( SMG_COMPONENT_SEQUENCE, 
                           gSmqUndoFuncs );

    smgRegisterPendOp( SMG_PEND_DROP_SEQUENCE,
                       smqDropPending );
    
    STL_TRY( smqRegisterFxTables( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Sequence Component를 Cooldown 시킨다.
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smqCooldown( smlEnv * aEnv )
{
    return STL_SUCCESS;
}

/**
 * @brief Sequence Component를 Bootdown 시킨다.
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smqBootdown( smlEnv * aEnv )
{
    return STL_SUCCESS;
}

/**
 * @brief Sequence Component를 Close 시킨다.
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smqShutdownClose( smlEnv * aEnv )
{
    return STL_SUCCESS;
}

/**
 * @brief Fixed Table 들을 등록함
 * @param[in]  aEnv   Environment
 * @remarks
 * NO_MOUNT 단계와 Warm-Up 단계에서 동일한 순서로 등록되어야 함.
 */
stlStatus smqRegisterFxTables( smlEnv * aEnv )
{
    STL_TRY( knlRegisterFxTable( &gSmqSequenceTableDesc,
                                 (knlEnv*)aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
