/*******************************************************************************
 * dtfRowIdObjectId.c
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
 * @file dtfRowIdObjectId.c
 * @brief dtfRowIdObjectId Function DataType Library Layer
 */

#include <dtl.h>
#include <dtDef.h>
#include <dtfRowId.h>

/**
 * @addtogroup dtfRowIdObjectId RowId_Object_Id
 * @ingroup dtf
 * @internal
 * @{
 */

/*******************************************************************************
 * ROWID_OBJECT_ID( rowid )
 * ex) ROWID_OBJECT_ID( rowid )
 ******************************************************************************/

/**
 * @brief  ROWID_OBJECT_ID 함수 [ ROWID_OBJECT_ID( rowid ) ]
 *         <BR> ROWID_OBJECT_ID( rowid )
 *         <BR> ex) ROWID_OBJECT_ID( rowid ) => 16406775070720
 *         <BR> ex) ROWID_OBJECT_ID( 'AAADuwAAAAAAACAAAAAiAAA' ) => 16406775070720
 *
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> ROWID_OBJECT_ID(ROWID)
 * @param[out] aResultValue      연산 결과 (결과타입 BIGINT)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfRowIdObjectId( stlUInt16        aInputArgumentCnt,
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
    
    /* ROWID_OBJECT_ID( rowid ) 의 argument rowid가 NULL일 수도 있어,
     * argument에 대한 check는 하지 않는다. */
    
    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue,
                          DTL_TYPE_NATIVE_BIGINT,
                          (stlErrorStack *)aEnv );

    if( DTL_IS_NULL( sValue1 ) == STL_TRUE )
    {
        /*
         * ROWID_OBJECT_ID( NULL ) 인 경우, 결과값은 0 이다.
         */ 
        DTF_BIGINT( sResultValue ) = 0;
    }
    else
    {    
        sRowIdType = DTF_ROWID( sValue1 );
        
        DTF_BIGINT( sResultValue )
            = (dtlBigIntType)( (((stlUInt64)(sRowIdType->mDigits[0])) << 56) |
                               (((stlUInt64)(sRowIdType->mDigits[1])) << 48) |
                               (((stlUInt64)(sRowIdType->mDigits[2])) << 40) |
                               (((stlUInt64)(sRowIdType->mDigits[3])) << 32) |
                               (((stlUInt64)(sRowIdType->mDigits[4])) << 24) |
                               (((stlUInt64)(sRowIdType->mDigits[5])) << 16) |
                               (((stlUInt64)(sRowIdType->mDigits[6])) << 8)  |
                               ((stlUInt64)(sRowIdType->mDigits[7])) );
    }
    
    sResultValue->mLength = DTL_NATIVE_BIGINT_SIZE;
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/** @} */
