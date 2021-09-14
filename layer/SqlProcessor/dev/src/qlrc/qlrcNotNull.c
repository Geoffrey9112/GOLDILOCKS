/*******************************************************************************
 * qlrcNotNull.c
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
 * @file qlrcNotNull.c
 * @brief SQL Processor Layer Deferrable Constraint
 */

#include <qll.h>
#include <qlDef.h>

/**
 * @defgroup qlrcNotNull NOT NULL 검사 함수 
 * @ingroup qlInternal
 * @{
 */

/**
 * @brief Value Block 에 포함된 NOT NULL 속성을 위반한 Row 의 개수를 얻는다.
 * @param[in]  aNewValueBlockList     New Value block list
 * @param[out] aCollisionCount        Collision Count
 * @param[out] aKeyViolPos            Violated Key Position
 * @remarks
 */
void qlrcCheckNotNull4Insert( knlValueBlockList * aNewValueBlockList,
                              stlInt32          * aCollisionCount,
                              stlInt32          * aKeyViolPos )
{
    stlInt32 sCollCnt = 0;
    stlInt32 sViolPos = 0;
    stlInt32 sKeyPos = 0;
    stlBool  sViolated = STL_FALSE;

    knlValueBlockList * sBlockList = NULL;
    stlInt32 sBlockIdx = 0;

    /**
     * Parameter Validation
     */

    STL_DASSERT( aNewValueBlockList != NULL );
    STL_DASSERT( aCollisionCount != NULL );
    STL_DASSERT( aKeyViolPos != NULL );

    /**
     * Row 에 해당하는 Block 에 NULL 값이 존재하는 지 검사
     */

    for ( sBlockIdx = KNL_GET_BLOCK_SKIP_CNT(aNewValueBlockList);
          sBlockIdx < KNL_GET_BLOCK_USED_CNT(aNewValueBlockList);
          sBlockIdx++ )
    {
        for ( sBlockList = aNewValueBlockList, sKeyPos = 0;
              sBlockList != NULL;
              sBlockList = sBlockList->mNext, sKeyPos++ )
        {
            if ( DTL_IS_NULL( KNL_GET_BLOCK_DATA_VALUE( sBlockList, sBlockIdx ) ) == STL_TRUE )
            {
                if ( sViolated == STL_FALSE )
                {
                    sViolPos = sKeyPos;
                    sViolated = STL_TRUE;
                }
                sCollCnt++;
                break;
            }
            else
            {
                /* no violation */
            }
        }
    }

    /**
     * Output 설정
     */

    *aCollisionCount = sCollCnt;
    *aKeyViolPos = sViolPos;
}



/**
 * @brief 삭제된 Value Block 에 포함된 NOT NULL 속성을 해소하는 Row 의 개수를 얻는다.
 * @param[in]  aIsDeleted             Deleted 여부 Array
 * @param[in]  aOldValueBlockList     Old Value block list
 * @param[out] aCollisionCount        Collision Count
 * @remarks
 */
void qlrcCheckNotNull4Delete( stlBool           * aIsDeleted,
                              knlValueBlockList * aOldValueBlockList,
                              stlInt32          * aCollisionCount )
{
    stlInt32 sCollCnt = 0;

    knlValueBlockList * sBlockList = NULL;
    stlInt32 sBlockIdx = 0;

    /**
     * Parameter Validation
     */

    STL_DASSERT( aIsDeleted != NULL );
    STL_DASSERT( aOldValueBlockList != NULL );
    STL_DASSERT( aCollisionCount != NULL );

    /**
     * Row 에 해당하는 Block 에 NULL 값이 존재하는 지 검사
     */

    for ( sBlockIdx = KNL_GET_BLOCK_SKIP_CNT(aOldValueBlockList);
          sBlockIdx < KNL_GET_BLOCK_USED_CNT(aOldValueBlockList);
          sBlockIdx++ )
    {
        if ( aIsDeleted[sBlockIdx] == STL_TRUE )
        {
            for ( sBlockList = aOldValueBlockList; sBlockList != NULL; sBlockList = sBlockList->mNext )
            {
                if ( DTL_IS_NULL( KNL_GET_BLOCK_DATA_VALUE( sBlockList, sBlockIdx ) ) == STL_TRUE )
                {
                    /**
                     * collision 이 해소됨
                     */
                    
                    sCollCnt--;
                    break;
                }
                else
                {
                    /* no violation */
                }
            }
        }
        else
        {
            /* delete 되지 않은 row 임 */
        }
    }

    /**
     * Output 설정
     */

    *aCollisionCount = sCollCnt;
}



/**
 * @brief 삭제된 Value Block 에 포함된 NOT NULL 속성을 해소하는 Row 의 개수를 얻는다.
 * @param[in]  aIsUpdated             Updated 여부 Array
 * @param[in]  aOldValueBlockList     Old Value block list (nullable)
 * @param[in]  aNewValueBlockList     New Value block list
 * @param[out] aCollisionCount        Collision Count
 * @param[out] aKeyViolPos            Violated Key Position
 * @remarks
 */



void qlrcCheckNotNull4Update( stlBool           * aIsUpdated,
                              knlValueBlockList * aOldValueBlockList,
                              knlValueBlockList * aNewValueBlockList,
                              stlInt32          * aCollisionCount,
                              stlInt32          * aKeyViolPos )
{
    stlInt32 sCollCnt = 0;

    knlValueBlockList * sBlockList = NULL;
    
    stlInt32 sBlockIdx = 0;

    stlBool  sNewHasNull = STL_FALSE;
    stlBool  sOldHasNull = STL_FALSE;

    stlInt32 sViolPos = 0;
    stlInt32 sKeyPos = 0;

    stlBool  sViolated = STL_FALSE;
    
    /**
     * Parameter Validation
     */

    STL_DASSERT( aIsUpdated != NULL );
    STL_DASSERT( aNewValueBlockList != NULL );
    STL_DASSERT( aCollisionCount != NULL );
    STL_DASSERT( aKeyViolPos != NULL );

    /**
     * UPDATE 된 Row 에 해당하는 Block 에 NULL 값이 존재하는 지 검사
     */

    for ( sBlockIdx = KNL_GET_BLOCK_SKIP_CNT(aNewValueBlockList);
          sBlockIdx < KNL_GET_BLOCK_USED_CNT(aNewValueBlockList);
          sBlockIdx++ )
    {
        sOldHasNull = STL_FALSE;
        sNewHasNull = STL_FALSE;

        if ( aIsUpdated[sBlockIdx] == STL_TRUE )
        {
            for ( sBlockList = aOldValueBlockList; sBlockList != NULL; sBlockList = sBlockList->mNext )
            {
                if ( DTL_IS_NULL( KNL_GET_BLOCK_DATA_VALUE( sBlockList, sBlockIdx ) ) == STL_TRUE )
                {
                    sOldHasNull = STL_TRUE;
                    break;
                }
                else
                {
                    /* no violation */
                }
            }

            for ( sBlockList = aNewValueBlockList; sBlockList != NULL; sBlockList = sBlockList->mNext )
            {
                if ( DTL_IS_NULL( KNL_GET_BLOCK_DATA_VALUE( sBlockList, sBlockIdx ) ) == STL_TRUE )
                {
                    if ( sViolated == STL_FALSE )
                    {
                        sViolPos = sKeyPos;
                    }

                    sNewHasNull = STL_TRUE;
                    break;
                }
                else
                {
                    /* no violation */
                }
            }

            if ( sNewHasNull == STL_TRUE )
            {
                if ( sOldHasNull == STL_TRUE )
                {
                    /* NULL 을 포함한 key 가 NULL 을 포함한 key 로 변경됨 */
                }
                else
                {
                    /* NULL 이 없던 key 가 NULL 을 포함한 key 로 변경됨 */
                    sCollCnt++;
                }
            }
            else
            {
                if ( sOldHasNull == STL_TRUE )
                {
                    /* NULL 을 포함한 key 가 NULL 이 없는 key 로 변경됨 */
                    sCollCnt--;
                }
                else
                {
                    /* NULL 이 없던 key 가 NULL 이 없는 key 로 변경됨 */
                }
                
            }
        }
        else
        {
            /* 읽기는 했으나 갱신한 Row 가 아님 */
        }
    }

    /**
     * Output 설정
     */

    *aCollisionCount = sCollCnt;
    *aKeyViolPos = sViolPos; 
}




/** @} qlrcNotNull */


