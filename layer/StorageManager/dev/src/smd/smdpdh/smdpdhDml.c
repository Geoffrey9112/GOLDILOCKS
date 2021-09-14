/*******************************************************************************
 * smdpdhDml.c
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
#include <smdDef.h>

#include <smdpfhDef.h>
#include <smdpdhDef.h>
#include <smdpdh.h>

/**
 * @file smdpdhDml.c
 * @brief Storage Manager Layer Dump Table DML Routines
 */

/**
 * @addtogroup smdpdh
 * @{
 */


/**
 * @brief 주어진 relation에 새 row를 삽입한다
 * @param[in]  aStatement       사용중인 statement
 * @param[in]  aRelationHandle  row를 삽입할 Relation의 handle
 * @param[in]  aValueIdx        삽입할 row가 사용하는 block Idx
 * @param[in]  aInsertColumns   삽입할 row의 컬럼 정보들
 * @param[out] aUniqueViolation not used
 * @param[out] aRowRid          삽입한 row의 위치
 * @param[in]  aEnv             Storage Manager Environment
 * @remarks
 * Dump Table 에 적용할 수 없는 연산임.
 */
stlStatus smdpdhInsert( smlStatement      * aStatement,
                        void              * aRelationHandle,
                        stlInt32            aValueIdx,
                        knlValueBlockList * aInsertColumns,
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
 * @param[in] aStatement        사용하는 statement
 * @param[in] aRelationHandle   갱신할 row가 존재하는 relation의 핸들
 * @param[in] aRowRid           갱신할 row의 row rid
 * @param[in] aRowScn           레코드 검색 당시의 SCN
 * @param[in] aValueIdx         갱신할 row가 사용하는 block Idx
 * @param[in] aAfterColumns     갱신할 컬럼정보들
 * @param[in] aBeforeColumns    이전 이미지를 저장할 공간
 * @param[in] aPrimaryKey       해당 레코드의 primary key
 * @param[out] aVersionConflict update 도중 version conflict를 발견했는지 여부
 * @param[out] aSkipped         이미 갱신된 Row인지 여부
 * @param[in] aEnv              Storage Manager Environment
 * @remarks
 * Dump Table 에 적용할 수 없는 연산임.
 */
stlStatus smdpdhUpdate( smlStatement      * aStatement,
                        void              * aRelationHandle,
                        smlRid            * aRowRid,
                        smlScn              aRowScn,
                        stlInt32            aValueIdx,
                        knlValueBlockList * aAfterColumns,
                        knlValueBlockList * aBeforeColumns,
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
 * @param[in] aStatement        사용하는 statement
 * @param[in] aRelationHandle   삭제할 row가 존재하는 relation의 핸들
 * @param[in] aRowRid           갱신할 row의 row rid
 * @param[in] aRowScn           레코드 검색 당시의 SCN
 * @param[in] aValueIdx         primary key가 사용하고 있는 block idx
 * @param[in] aPrimaryKey       해당 레코드의 primary key
 * @param[out] aVersionConflict update 도중 version conflict를 발견했는지 여부
 * @param[out] aSkipped         이미 삭제된 Row인지 여부
 * @param[in] aEnv              Storage Manager Environment
 * @remarks
 * Dump Table 에 적용할 수 없는 연산임.
 */
stlStatus smdpdhDelete( smlStatement      * aStatement,
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
 * @param[in] aStatement      사용하는 statement
 * @param[in] aRelationHandle lock을 걸 row가 존재하는 relation의 핸들
 * @param[in] aRowRid         lock을 걸 row의 row rid
 * @param[in] aRowScn         레코드 검색 당시의 SCN
 * @param[in] aLockMode       lock을 걸 mode(SML_LOCK_S/SML_LOCK_X)
 * @param[in] aTimeInterval   락 타임아웃 시간( sec단위 )
 * @param[out] aVersionConflict Lock 시도 도중에 version conflict 상황을 만났는지 여부
 * @param[in] aEnv            Storage Manager Environment
 * @remarks
 * Dump Table 에 적용할 수 없는 연산임.
 */
stlStatus smdpdhLockRow( smlStatement * aStatement,
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
 * @param[in] aSlot Slot의 첫번째 포인터
 * @return Slot의 Body 포인터
 * @remarks
 * Dump Table 에 적용할 수 없는 연산임.
 */
inline void * smdpdhGetSlotBody( void * aSlot )
{
    return NULL;
}

/** @} */
