/*******************************************************************************
 * smdpfhDml.c
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
#include <sms.h>
#include <smxm.h>
#include <smxl.h>
#include <smp.h>
#include <sme.h>
#include <sma.h>
#include <smdpfhDef.h>
#include <smdDef.h>
#include <smdpfh.h>

/**
 * @file smdpfhDml.c
 * @brief Storage Manager Layer Fixed Table DML Routines
 */

/**
 * @addtogroup smd
 * @{
 */


/**
 * @brief 주어진 relation에 새 row를 삽입한다
 * @remarks
 * Fixed Table 에 적용할 수 없는 연산임.
 */
stlStatus smdpfhInsert( smlStatement      * aStatement,
                        void              * aRelationHandle,
                        stlInt32            aValueIdx,
                        knlValueBlockList * aInsertCols,
                        knlValueBlockList * aUniqueViolation,
                        smlRid            * aRowRid,
                        smlEnv            * aEnv )
{
    stlPushError( STL_ERROR_LEVEL_ABORT,
                  SML_ERRCODE_INVALID_OPERATION_ON_FIXED_TABLE,
                  NULL,
                  KNL_ERROR_STACK( aEnv ) );
    
    return STL_FAILURE;
}




/**
 * @brief 주어진 row rid의 row의 컬럼들을 갱신한다
 * @remarks
 * Fixed Table 에 적용할 수 없는 연산임.
 */
stlStatus smdpfhUpdate( smlStatement      * aStatement,
                        void              * aRelationHandle,
                        smlRid            * aRowRid,
                        smlScn              aRowScn,
                        stlInt32            aValueIdx,
                        knlValueBlockList * aAfterCols,
                        knlValueBlockList * aBeforeCols,
                        knlValueBlockList * aPrimaryKey,
                        stlBool           * aVersionConflict,
                        stlBool           * aSkipped,
                        smlEnv            * aEnv )
{
    stlPushError( STL_ERROR_LEVEL_ABORT,
                  SML_ERRCODE_INVALID_OPERATION_ON_FIXED_TABLE,
                  NULL,
                  KNL_ERROR_STACK( aEnv ) );
    
    return STL_FAILURE;
}


/**
 * @brief 주어진 row rid의 row를 삭제한다
 * @remarks
 * Fixed Table 에 적용할 수 없는 연산임.
 */
stlStatus smdpfhDelete( smlStatement      * aStatement,
                        void              * aRelationHandle,
                        smlRid            * aRowRid,
                        smlScn              aRowScn,
                        stlInt32            aValueIdx,
                        knlValueBlockList * aPrimaryKey,
                        stlBool           * aVersionConflict,
                        stlBool           * aSkipped,
                        smlEnv            * aEnv )
{
    stlPushError( STL_ERROR_LEVEL_ABORT,
                  SML_ERRCODE_INVALID_OPERATION_ON_FIXED_TABLE,
                  NULL,
                  KNL_ERROR_STACK( aEnv ) );
    
    return STL_FAILURE;
}


/**
 * @brief 주어진 row rid의 row에 lock을 건다
 * @remarks
 * Fixed Table 에 적용할 수 없는 연산임.
 */
stlStatus smdpfhLockRow( smlStatement * aStatement,
                         void         * aRelationHandle,
                         smlRid       * aRowRid,
                         smlScn         aRowScn,
                         stlUInt16      aLockMode,
                         stlInt64       aTimeInterval,
                         stlBool      * aVersionConflict,
                         smlEnv       * aEnv )
{
    stlPushError( STL_ERROR_LEVEL_ABORT,
                  SML_ERRCODE_INVALID_OPERATION_ON_FIXED_TABLE,
                  NULL,
                  KNL_ERROR_STACK( aEnv ) );
    
    return STL_FAILURE;
}



/**
 * @brief 주어진 Slot의 Slot Body 포인터를 얻는다.
 * @remarks
 * Fixed Table 에 적용할 수 없는 연산임.
 */
inline void * smdpfhGetSlotBody( void * aSlot )
{
    return NULL;
}

/** @} */
