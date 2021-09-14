/*******************************************************************************
 * smtStartup.c
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
#include <smrDef.h>
#include <smf.h>
#include <smg.h>
#include <smr.h>
#include <smd.h>
#include <sma.h>
#include <smt.h>
#include <smd.h>
#include <sme.h>
#include <smxl.h>
#include <smtTbs.h>
#include <smlGeneral.h>
#include <smlTable.h>

/**
 * @file smtStartup.c
 * @brief Storage Manager Layer Recovery Startup Internal Routines
 */

extern smrRedoVector  gSmtRedoVectors[];
extern smrMtxUndoFunc gSmtMtxUndoFuncs[];
extern knlFxTableDesc gTbsExtTableDesc;
extern knlFxTableDesc gTbsExtBlockDescTableDesc;
extern smxlUndoFunc gSmtUndoFuncs[];

/**
 * @addtogroup smt
 * @{
 */

/**
 * @brief Tablespace Component를 NO MOUNT 모드로 전이한다.
 * @param[out] aWarmupEntry Tablespace Component의 Warmup Entry 포인터 
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smtStartupNoMount( void  ** aWarmupEntry,
                             smlEnv * aEnv )
{
    smrRegisterRedoVectors( SMG_COMPONENT_TABLESPACE,
                            gSmtRedoVectors );
    
    smxlRegisterUndoFuncs( SMG_COMPONENT_TABLESPACE, 
                           gSmtUndoFuncs );

    STL_TRY( smtRegisterFxTables( aEnv ) == STL_SUCCESS );
    
    smgRegisterPendOp( SMG_PEND_ADD_EXTENT_BLOCK, smtAddExtBlockPending );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Tablespace Component를 Open 모드로 전이한다.
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smtStartupOpen( smlEnv * aEnv )
{
    smlTbsId          sTbsId;
    stlBool           sTbsExist;
    stlBool           sDatafileExist;
    smlDatafileId     sDatafileId;
    
    STL_TRY_THROW( smfGetServerState() != SML_SERVER_STATE_NONE,
                   RAMP_FINISH );

    STL_TRY( smfFirstTbs( &sTbsId,
                          &sTbsExist,
                          aEnv ) == STL_SUCCESS );

    while( sTbsExist == STL_TRUE )
    {
        /**
         * Temporary Datafile을 Format한다.
         */
        if( SML_TBS_IS_MEMORY_TEMPORARY( SMF_GET_TBS_ATTR(sTbsId) ) == STL_TRUE )
        {
            STL_TRY( smfFirstDatafile( sTbsId,
                                       &sDatafileId,
                                       &sDatafileExist,
                                       aEnv ) == STL_SUCCESS );

            while( sDatafileExist == STL_TRUE )
            {
                STL_TRY( smtFormatDatafile( NULL,
                                            sTbsId,
                                            sDatafileId,
                                            0,  /* aReservedPageCount */
                                            aEnv )
                         == STL_SUCCESS );
            
                STL_TRY( smfNextDatafile( sTbsId,
                                          &sDatafileId,
                                          &sDatafileExist,
                                          aEnv ) == STL_SUCCESS );
            }
        }

        STL_TRY( smfNextTbs( &sTbsId,
                             &sTbsExist,
                             aEnv ) == STL_SUCCESS );
    }
    
    STL_RAMP( RAMP_FINISH );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
    
/**
 * @brief Tablespace Component를 Warmup 시킨다.
 * @param[in] aWarmupEntry Tablespace Component의 Warmup Entry 포인터 
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smtWarmup( void   * aWarmupEntry,
                     smlEnv * aEnv )
{
    smrRegisterRedoVectors( SMG_COMPONENT_TABLESPACE,
                            gSmtRedoVectors );

    smxlRegisterUndoFuncs( SMG_COMPONENT_TABLESPACE, 
                           gSmtUndoFuncs );

    STL_TRY( smtRegisterFxTables( aEnv ) == STL_SUCCESS );
    
    smgRegisterPendOp( SMG_PEND_ADD_EXTENT_BLOCK, smtAddExtBlockPending );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Tablespace Component를 Cooldown 시킨다.
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smtCooldown( smlEnv * aEnv )
{
    return STL_SUCCESS;
}

/**
 * @brief Tablespace Component를 Bootdown 시킨다.
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smtBootdown( smlEnv * aEnv )
{
    return STL_SUCCESS;
}

/**
 * @brief Tablespace Component를 Close 시킨다.
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smtShutdownClose( smlEnv * aEnv )
{
    return STL_SUCCESS;
}


/**
 * @brief Fixed Table 들을 등록함
 * @param[in]  aEnv   Environment
 * @remarks
 * NO_MOUNT 단계와 Warm-Up 단계에서 동일한 순서로 등록되어야 함.
 */
stlStatus smtRegisterFxTables( smlEnv * aEnv )
{
    STL_TRY( knlRegisterFxTable( &gTbsExtTableDesc,
                                 (knlEnv*)aEnv )
             == STL_SUCCESS );
    
    STL_TRY( knlRegisterFxTable( &gTbsExtBlockDescTableDesc,
                                 (knlEnv*)aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
