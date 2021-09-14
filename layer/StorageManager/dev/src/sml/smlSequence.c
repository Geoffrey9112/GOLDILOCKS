/*******************************************************************************
 * smlSequence.c
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
#include <smq.h>
#include <sma.h>
#include <smxl.h>
#include <smlSequence.h>
#include <smlGeneral.h>


/**
 * @file smlSequence.c
 * @brief Storage Manager Layer Sequence Routines
 */

/**
 * @addtogroup smlSequence
 * @{
 */

/**
 * @brief Sequence Cache를 생성한다.
 * @param[in] aStatement Statement Pointer
 * @param[in] aAttr Seqeunce 속성
 * @param[out] aSequenceId 생성된 Sequence의 물리적 아이디
 * @param[out] aSequenceHandle 생성된 Sequence를 조작할때 사용할 메모리 핸들
 * @param[in,out] aEnv Environment Pointer
 */
stlStatus smlCreateSequence( smlStatement     * aStatement,
                             smlSequenceAttr  * aAttr,
                             stlInt64         * aSequenceId,
                             void            ** aSequenceHandle,
                             smlEnv           * aEnv )
{
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    STL_TRY( smxlInsertTransRecord( SMA_GET_TRANS_ID(aStatement),
                                    aEnv )
             == STL_SUCCESS );
    
    STL_TRY( smqCreateSequence( aStatement,
                                aAttr,
                                aSequenceId,
                                aSequenceHandle,
                                aEnv ) == STL_SUCCESS );
    
    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief Sequence의 속성을 변경한다.
 * @param[in]     aStatement        Statement Handle
 * @param[in]     aOrgSeqHandle     Old Sequence Handle
 * @param[in]     aAttr             New Seqeunce Attriute
 * @param[out]    aNewSeqId         New Sequence Identifier
 * @param[out]    aNewSeqHandle     New Sequence Handle
 * @param[in,out] aEnv              Environment Pointer
 */
stlStatus smlAlterSequence( smlStatement     * aStatement,
                            void             * aOrgSeqHandle,
                            smlSequenceAttr  * aAttr,
                            stlInt64         * aNewSeqId,
                            void            ** aNewSeqHandle,
                            smlEnv           * aEnv )
{
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    STL_TRY( smxlInsertTransRecord( SMA_GET_TRANS_ID(aStatement),
                                    aEnv )
             == STL_SUCCESS );
    
    STL_TRY( smqAlterSequence( aStatement,
                               aOrgSeqHandle,
                               aAttr,
                               aNewSeqId,
                               aNewSeqHandle,
                               aEnv ) == STL_SUCCESS );
    
    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief Sequence Cache를 삭제한다.
 * @param[in] aStatement Statement Pointer
 * @param[in] aSequenceHandle 삭제할 Sequence Handle
 * @param[in,out] aEnv Environment Pointer
 */
stlStatus smlDropSequence( smlStatement * aStatement,
                           void         * aSequenceHandle,
                           smlEnv       * aEnv )
{
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    STL_TRY( smxlInsertTransRecord( SMA_GET_TRANS_ID(aStatement),
                                    aEnv )
             == STL_SUCCESS );
    
    STL_TRY( smqDropSequence( aStatement,
                              aSequenceHandle,
                              aEnv ) == STL_SUCCESS );
    
    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief 다음 Sequence 값을 얻는다.
 * @param[in] aSequenceHandle Sequence Handle
 * @param[out] aNextValue 다음 Sequence 값
 * @param[in,out] aEnv Environment Pointer
 * @note 현재 Sequence 값에 Increment By 만큼 증가시키고 이를 리턴한다.
 */
stlStatus smlGetNextSequenceVal( void      * aSequenceHandle,
                                 stlInt64  * aNextValue,
                                 smlEnv    * aEnv )
{
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    STL_TRY( smqGetNextVal( aSequenceHandle,
                            aNextValue,
                            aEnv ) == STL_SUCCESS );
    
    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief 현재 Session의 Sequence 값을 얻는다.
 * @param[in] aSequenceHandle Sequence Handle
 * @param[out] aCurrValue 현재 Sequence 값
 * @param[in,out] aEnv Environment Pointer
 */
stlStatus smlGetCurrSessSequenceVal( void      * aSequenceHandle,
                                     stlInt64  * aCurrValue,
                                     smlEnv    * aEnv )
{
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    STL_TRY( smqGetCurrSessVal( aSequenceHandle,
                                aCurrValue,
                                aEnv ) == STL_SUCCESS );
    
    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief 현재 Sequence 값을 얻는다.
 * @param[in] aSequenceHandle Sequence Handle
 * @param[out] aCurrValue 현재 Sequence 값
 * @param[in,out] aEnv Environment Pointer
 */
stlStatus smlGetCurrSequenceVal( void      * aSequenceHandle,
                                 stlInt64  * aCurrValue,
                                 smlEnv    * aEnv )
{
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    STL_TRY( smqGetCurrVal( aSequenceHandle,
                            aCurrValue,
                            aEnv ) == STL_SUCCESS );
    
    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief 현재 Sequence 값을 설정한다.
 * @param[in] aSequenceHandle Sequence Handle
 * @param[in] aCurrValue 설정할 Sequence 값
 * @param[in,out] aEnv Environment Pointer
 */
stlStatus smlSetCurrSequenceVal( void      * aSequenceHandle,
                                 stlInt64    aCurrValue,
                                 smlEnv    * aEnv )
{
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    STL_TRY( smqSetCurrVal( aSequenceHandle,
                            aCurrValue,
                            aEnv ) == STL_SUCCESS );
    
    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/**
 * @brief 현재 Sequence 설정을 얻는다.
 * @param[in] aSequenceHandle Sequence Handle
 * @param[out] aSequenceAttr 설정된 Sequence 속성들
 * @param[in,out] aEnv Environment Pointer
 */
stlStatus smlGetSequenceAttr( void            * aSequenceHandle,
                              smlSequenceAttr * aSequenceAttr,
                              smlEnv          * aEnv )
{
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    smqFillSequenceAttr( aSequenceHandle, aSequenceAttr );
    
    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;
}

/**
 * @brief 지정되지 않은 Sequence Attribute 를 채운다
 * @param[in,out] aAttr   Sequence Attribute
 */
void smlFillDefaultSequenceAttr( smlSequenceAttr * aAttr )
{
    if ( (aAttr->mValidFlags & SML_SEQ_INCREMENTBY_MASK) == SML_SEQ_INCREMENTBY_NO )
    {
        aAttr->mIncrementBy = SML_INCREMENTBY_DEFAULT;
    }
    
    if( (aAttr->mValidFlags & SML_SEQ_MINVALUE_MASK) == SML_SEQ_MINVALUE_NO )
    {
        if( aAttr->mIncrementBy > 0 )
        {
            aAttr->mMinValue = SML_POSITIVE_MINVALUE_DEFAULT;
        }
        else
        {
            aAttr->mMinValue = SML_NEGATIVE_MINVALUE_DEFAULT;
        }
    }
    
    if( (aAttr->mValidFlags & SML_SEQ_MAXVALUE_MASK) == SML_SEQ_MAXVALUE_NO )
    {
        if( aAttr->mIncrementBy > 0 )
        {
            aAttr->mMaxValue = SML_POSITIVE_MAXVALUE_DEFAULT;
        }
        else
        {
            aAttr->mMaxValue = SML_NEGATIVE_MAXVALUE_DEFAULT;
        }
    }

    if( (aAttr->mValidFlags & SML_SEQ_STARTWITH_MASK) == SML_SEQ_STARTWITH_NO )
    {
        if( aAttr->mIncrementBy > 0 )
        {
            aAttr->mStartWith = aAttr->mMinValue;
        }
        else
        {
            aAttr->mStartWith = aAttr->mMaxValue;
        }
    }

    if( (aAttr->mValidFlags & SML_SEQ_CYCLE_MASK) == SML_SEQ_CYCLE_NO )
    {
        aAttr->mCycle = STL_FALSE;
    }

    if( (aAttr->mValidFlags & SML_SEQ_CACHESIZE_MASK) == SML_SEQ_CACHESIZE_NO )
    {
        if( (aAttr->mValidFlags & SML_SEQ_NOCACHE_MASK) == SML_SEQ_NOCACHE_YES )
        {
            aAttr->mCacheSize = 1;
        }
        else
        {
            aAttr->mCacheSize = SML_CACHESIZE_DEFAULT;
        }
    }

    aAttr->mValidFlags = ( SML_SEQ_STARTWITH_YES   |
                           SML_SEQ_INCREMENTBY_YES |
                           SML_SEQ_MAXVALUE_YES    |
                           SML_SEQ_MINVALUE_YES    |
                           SML_SEQ_CACHESIZE_YES   |
                           SML_SEQ_CYCLE_YES );
}

/**
 * @brief 주어진 Sequence Identifier로 부터 Sequence Handle을 얻는다.
 * @param[in] aSequenceId Sequence Identifier
 * @param[out] aSequenceHandle Sequence Handle
 * @param[in,out] aEnv Environment Pointer
 * @note Open 단계 이상에만 호출가능하다.
 */
stlStatus smlGetSequenceHandle( stlInt64    aSequenceId,
                                void     ** aSequenceHandle,
                                smlEnv    * aEnv )
{
    smlRid sSequenceRid;

    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    SMG_WRITE_RID( &sSequenceRid, &aSequenceId );

    STL_TRY( smqGetSequenceHandle( sSequenceRid,
                                   aSequenceHandle,
                                   aEnv ) == STL_SUCCESS );
    
    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/** @} */
