/*******************************************************************************
 * smlStatement.c
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

#include <dtl.h>
#include <sml.h>
#include <smDef.h>
#include <sma.h>
#include <smDef.h>

/**
 * @file smlStatement.c
 * @brief Storage Manager Layer Statement Routines
 */

/**
 * @addtogroup smlStatement
 * @{
 */

/**
 * @brief Statement를 할당 받는다.
 * @param[in] aTransId Transaction Identifier
 * @param[in] aUpdateRelHandle Updatable Relation Handle (nullable)
 * @param[in] aAttribute Statement 속성
 * @param[in] aLockTimeout Lock Timeout 시간
 * @param[in] aNeedSnapshotIterator Snaphot Iterator가 필요한 Statement 인지 여부
 * @param[out] aStatement 생성된 Statement Handle
 * @param[in,out] aEnv Environment Pointer
 * @see @a aAttribute : smlStatementAttr
 * @remarks @a aRelationHandle은 DML(INSERT/DELETE/UPDATE) Updatable Statement에만 유효하며,
 * <BR> Update 하고자 하는 대상 relation을 의미한다.
 * <BR> aRelationHandle은 Mutating 검사와 Statement Level Visibility 검사 여부를 위해서 사용된다.
 * <BR> 따라서, Mutating 검사와 Statement Level Visibility 검사 여부가 필요없는 DDL이나 SELECT는
 * <BR> aRelationHandle이 NULL일수 있다.
 */
inline stlStatus smlAllocStatement( smlTransId      aTransId,
                                    void          * aUpdateRelHandle,
                                    stlInt32        aAttribute,
                                    stlInt64        aLockTimeout,
                                    stlBool         aNeedSnapshotIterator,
                                    smlStatement ** aStatement,
                                    smlEnv        * aEnv )
{
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    STL_TRY( smaAllocStatement( SMXL_TO_TRANS_ID( aTransId ),
                                aTransId,
                                aUpdateRelHandle,
                                aAttribute,
                                aLockTimeout,
                                aNeedSnapshotIterator,
                                aStatement,
                                aEnv ) == STL_SUCCESS );
    
    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief Statement를 해제한다.
 * @param[in] aStatement 해제할 aStatement
 * @param[in,out] aEnv Environment Pointer
 * @note Statement의 메모리 해제는 해당 세션의 모든 Statement가 완료한 이후에 수행된다.
 */
inline stlStatus smlFreeStatement( smlStatement * aStatement,
                                   smlEnv       * aEnv )
{
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    STL_TRY( smaFreeStatement( aStatement, aEnv ) == STL_SUCCESS );
    
    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief Statement를 RESET 한다.
 * @param[in] aStatement 롤백할 aStatement
 * @param[in] aDoRollback Rollback 수행 여부
 * @param[in,out] aEnv Environment Pointer
 */
stlStatus smlResetStatement( smlStatement * aStatement,
                             stlBool        aDoRollback,
                             smlEnv       * aEnv )
{
    return smaResetStatement( aStatement,
                              aDoRollback,
                              aEnv );
}


/**
 * @brief Statement의 relation handle 정보를 변경한다.
 * @param[in] aStatement 롤백할 aStatement
 * @param[in] aRelHandle Relation Handle
 * @param[in,out] aEnv Environment Pointer
 * @note Iterator가 생성되기 이전에 호출되어야 한다.
 */
stlStatus smlUpdateRelHandle( smlStatement * aStatement,
                              void         * aRelHandle,
                              smlEnv       * aEnv )
{
    aStatement->mUpdateRelHandle = aRelHandle;
    STL_DASSERT( STL_RING_IS_EMPTY( &aStatement->mIterators ) == STL_TRUE );

    return STL_SUCCESS;
}


inline stlInt64 smlGetStatementTimestamp( smlStatement * aStatement )
{
    return aStatement->mTimestamp;
}

/**
 * @brief Statement를 롤백한다.
 * @param[in] aStatement 롤백할 aStatement
 * @param[in,out] aEnv Environment Pointer
 */
inline stlStatus smlRollbackStatement( smlStatement * aStatement,
                                       smlEnv       * aEnv )
{
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    STL_TRY( smaRollbackStatement( aStatement,
                                   STL_TRUE, /* aReleaseLock */
                                   aEnv ) == STL_SUCCESS );
    
    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief Iterator를 할당한다.
 * @param[in]     aStatement        Iterator가 속하는 Statement
 * @param[in]     aRelationHandle   Iterator를 사용하려고 하는 Relation의 Handle
 * @param[in]     aTransReadMode    Transaction간 Read Mode
 * @param[in]     aStmtReadMode     aStatement내 Read Mode
 * @param[in]     aProperty         Iterator의 동작방식에 관련된 속성들
 * @param[in]     aScanDirection    Scan 방향
 * @param[out]    aIterator         할당되고 초기화된 Iterator의 포인터
 * @param[in,out] aEnv              Storage Manager Environment
 * @remarks aTransReadMode와 aStmtReadMode는 Iterator가 읽어야할 대상을 선정하기 위해서 사용된다.
 * @par Example:
 * <table>
 *     <caption align="TOP"> SQL에서 Read Mode의 사용 </caption>
 *     <tr>
 *         <td>  </td>
 *         <th>  SML_TRM_SNAPSHOT  </th>
 *         <th>  SML_TRM_RECENT  </th>
 *     </tr>
 *     <tr>
 *         <th>  SML_SRM_SNAPSHOT </th>
 *         <td>  SELECT, INSERT INTO SELECT, ODBC SENSITIVE CURSOR </td>
 *         <td>  DML, SELECT FOR UPDATE/SHARE </td>
 *     </tr>
 *     <tr>
 *         <th>  SML_SRM_RECENT </th>
 *         <td>  X </td>
 *         <td>  DDL, FOREIGN CONSTRAINT 검사 </td>
 *     </tr>
 * </table>
 */
stlStatus smlAllocIterator( smlStatement              * aStatement,
                            void                      * aRelationHandle,
                            smlTransReadMode            aTransReadMode,
                            smlStmtReadMode             aStmtReadMode,
                            smlIteratorProperty       * aProperty,
                            smlIteratorScanDirection    aScanDirection,
                            void                     ** aIterator,
                            smlEnv                    * aEnv )
{
    STL_DASSERT( smlEnterIterator( aEnv ) == STL_SUCCESS );
    
    STL_TRY( smaAllocIterator( aStatement,
                               aRelationHandle,
                               aTransReadMode,
                               aStmtReadMode,
                               aProperty,
                               aScanDirection,
                               aIterator,
                               aEnv ) == STL_SUCCESS );
    
    STL_DASSERT( smlLeaveIterator( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveIterator( aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief Iterator를 해제한다.
 * @param[in] aIterator Iterator 포인터
 * @param[in,out] aEnv Storage Manager Environment
 */
inline stlStatus smlFreeIterator( void    * aIterator,
                                  smlEnv  * aEnv )
{
    STL_DASSERT( smlEnterIterator( aEnv ) == STL_SUCCESS );
    
    STL_TRY( smaFreeIterator( aIterator,
                              aEnv ) == STL_SUCCESS );
    
    STL_DASSERT( smlLeaveIterator( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveIterator( aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/** @} */
