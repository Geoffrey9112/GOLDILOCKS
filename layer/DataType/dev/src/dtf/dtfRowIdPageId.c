/*******************************************************************************
 * dtfRowIdPageId.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id$
 *
 * NOTES
 *
 ******************************************************************************/

/**
 * @file dtfRowIdPageId.c
 * @brief dtfRowIdPageId Function DataType Library Layer
 */

#include <dtl.h>
#include <dtDef.h>
#include <dtfRowId.h>

/**
 * @addtogroup dtfRowIdPageId RowId_Page_Id
 * @ingroup dtf
 * @internal
 * @{
 */

/*******************************************************************************
 * ROWID_PAGE_ID( rowid )
 * ex) ROWID_PAGE_ID( )
 ******************************************************************************/

/**
 * @brief  ROWID_PAGE_ID 함수 [ ROWID_PAGE_ID( rowid ) ]
 *         <BR> ROWID_PAGE_ID( rowid )
 *         <BR> ex) ROWID_PAGE_ID( rowid ) => 34
 *         <BR> ex) ROWID_PAGE_ID( 'AAADuwAAAAAAACAAAAAiAAA' ) => 34
 *
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> ROWID_PAGE_ID(ROWID)
 * @param[out] aResultValue      연산 결과 (결과타입 BIGINT)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfRowIdPageId( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aEnv )
{
    dtlDataValue  * sValue1;
    dtlDataValue  * sResultValue;
    dtlRowIdType  * sRowIdType;
    
    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv );
    
    sValue1 = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;

    /* ROWID_PAGE_ID( rowid ) 의 argument rowid가 NULL일 수도 있어,
     * argument에 대한 check는 하지 않는다. */
    
    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue,
                          DTL_TYPE_NATIVE_BIGINT,
                          (stlErrorStack *)aEnv );
    
    if( DTL_IS_NULL( sValue1 ) == STL_TRUE )
    {
        /*
         * ROWID_PAGE_ID( NULL ) 인 경우, 결과값은 0 이다.
         */ 
        DTF_BIGINT( sResultValue ) = 0;
    }
    else
    {    
        sRowIdType = DTF_ROWID( sValue1 );
        
        DTF_BIGINT( sResultValue )
            = (dtlBigIntType)( (((stlUInt32)(sRowIdType->mDigits[10])) << 24) |
                               (((stlUInt32)(sRowIdType->mDigits[11])) << 16) |
                               (((stlUInt32)(sRowIdType->mDigits[12])) << 8)  |
                               ((stlUInt32)(sRowIdType->mDigits[13])) );
    }
    
    sResultValue->mLength = DTL_NATIVE_BIGINT_SIZE;
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/** @} */
