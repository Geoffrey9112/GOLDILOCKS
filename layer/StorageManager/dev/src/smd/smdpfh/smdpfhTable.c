/*******************************************************************************
 * smdpfhTable.c
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
#include <smg.h>
#include <smkl.h>
#include <smdpfhDef.h>
#include <smdpfh.h>
#include <smdDef.h>

/**
 * @file smdpfhTable.c
 * @brief Storage Manager Layer Fixed Table Internal Routines
 */

/**
 * @addtogroup smdpfh
 * @{
 */

/* Fixed Table Module */
smdTableModule gSmdpfhTableModule = 
{
    smdpfhCreate,
    NULL, /* CreateIotFunc */
    NULL, /* CreateUndoFunc */
    smdpfhDrop,
    NULL,            /* copy A segment to B segment */
    NULL,            /* add columns */
    NULL,            /* drop columns */
    NULL,            /* alter table attribute */
    NULL,            /* merge table attribute */
    NULL,            /* check whether row exist or not */
    NULL,            /* create for truncate */
    NULL,            /* drop aging */
    smdpfhInitialize,
    smdpfhFinalize,
    smdpfhInsert,
    NULL,            /* block insert */
    smdpfhUpdate,
    smdpfhDelete,
    NULL,            /* merge */
    NULL,            /* rowcount */
    NULL,            /* fetch */
    NULL,            /* fetch with rowid */
    smdpfhLockRow,
    NULL,            /* compare key value */
    NULL,            /* extract key value */
    NULL,            /* extract valid key value */
    smdpfhSort,
    smdpfhGetSlotBody,
    NULL,            /* truncate */
    NULL,            /* set flag */
    NULL,            /* clean up */
    NULL,            /* build cache */
    NULL             /* fetch record for index */
};

smeIteratorModule gSmdpfhIteratorModule = 
{
    STL_SIZEOF(smdpfhIterator),

    smdpfhInitIterator,
    smdpfhFiniIterator,
    smdpfhResetIterator,
    smdpfhMoveToRowRid,
    smdpfhMoveToPosNum,
    {
        smdpfhFetchNext,
        smdpfhFetchPrev,
        (smiFetchAggrFunc)smdpfhFetchNext,
        NULL   /* FetchSampling */
    }
};


/**
 * @brief segment page에 테이블헤더 정보를 기록한다.
 * @param[in]  aStatement       
 * @param[in]  aSegmentRid      
 * @param[in]  aSegmentHandle   
 * @param[in]  aAttr 
 * @param[out] aRelationHandle 
 * @param[in]  aEnv
 * @remarks
 * Fixed Table 에는 적용할 수 없는 연산임.
 */
stlStatus smdpfhInitTableHeader( smlStatement  * aStatement,
                                 smlRid        * aSegmentRid,
                                 void          * aSegmentHandle,
                                 smlTableAttr  * aAttr,
                                 void         ** aRelationHandle,
                                 smlEnv        * aEnv )
{
    stlPushError( STL_ERROR_LEVEL_ABORT,
                  SML_ERRCODE_INVALID_OPERATION_ON_FIXED_TABLE,
                  NULL,
                  KNL_ERROR_STACK( aEnv ) );
    
    return STL_FAILURE;
}




/**
 * @brief 테이블을 생성한다.
 * @param[in]  aStatement 
 * @param[in]  aTbsId 
 * @param[in]  aAttr 
 * @param[in]  aLoggingUndo
 * @param[out] aSegmentId 
 * @param[out] aRelationHandle 
 * @param[in]  aEnv 
 * @remarks
 * Fixed Table 에는 적용할 수 없는 연산임.
 */
stlStatus smdpfhCreate( smlStatement  * aStatement,
                        smlTbsId        aTbsId,
                        smlTableAttr  * aAttr,
                        stlBool         aLoggingUndo,
                        stlInt64      * aSegmentId,
                        void         ** aRelationHandle,
                        smlEnv        * aEnv )
{
    stlPushError( STL_ERROR_LEVEL_ABORT,
                  SML_ERRCODE_INVALID_OPERATION_ON_FIXED_TABLE,
                  NULL,
                  KNL_ERROR_STACK( aEnv ) );
    
    return STL_FAILURE;
}

/**
 * @brief 테이블을 삭제한다.
 * @param[in] aStatement 
 * @param[in] aRelationHandle 
 * @param[in] aEnv 
 * @remarks
 * Fixed Table 에는 적용할 수 없는 연산임.
 */
stlStatus smdpfhDrop( smlStatement * aStatement,
                      void         * aRelationHandle,
                      smlEnv       * aEnv )
{
    stlPushError( STL_ERROR_LEVEL_ABORT,
                  SML_ERRCODE_INVALID_OPERATION_ON_FIXED_TABLE,
                  NULL,
                  KNL_ERROR_STACK( aEnv ) );
    
    return STL_FAILURE;
}

/**
 * @brief 초기화 
 * @param[in] aStatement 
 * @param[in] aRelationHandle 
 * @param[in] aEnv 
 * @remarks
 */
stlStatus smdpfhInitialize( smlStatement * aStatement,
                            void         * aRelationHandle,
                            smlEnv       * aEnv )
{
    /* Nothing To Do */
    return STL_SUCCESS;
}


/**
 * @brief 종료 
 * @param[in] aStatement 
 * @param[in] aRelationHandle 
 * @param[in] aEnv 
 * @remarks
 */
stlStatus smdpfhFinalize( smlStatement * aStatement,
                          void         * aRelationHandle,
                          smlEnv       * aEnv )
{
    /* Nothing To Do */
    return STL_SUCCESS;
}

/**
 * @brief 정렬한다. 
 * @param[in] aRelationHandle 
 * @param[in] aTransactionHandle
 * @remarks
 * Fixed Table 에는 적용할 수 없는 연산임.
 * @todo Env 를 포함한 prototype 으로 바뀌면 Error 설정 해야 함.
 */
stlStatus smdpfhSort( void   * aRelationHandle,
                      void   * aTransactionHandle )
{
    return STL_FAILURE;
}


/** @} */
