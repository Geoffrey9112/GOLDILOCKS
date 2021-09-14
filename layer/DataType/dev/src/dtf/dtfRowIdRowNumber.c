/*******************************************************************************
 * dtfRowIdRowNumber.c
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
 * @file dtfRowIdRowNumber.c
 * @brief dtfRowIdRowNumber Function DataType Library Layer
 */

#include <dtl.h>
#include <dtDef.h>
#include <dtfRowId.h>

/**
 * @addtogroup dtfRowIdRowNumber RowId_Row_Number
 * @ingroup dtf
 * @internal
 * @{
 */

/*******************************************************************************
 * ROWID_ROW_NUMBER( rowid )
 * ex) ROWID_ROW_NUMBER( ) =>
 ******************************************************************************/

/**
 * @brief  ROWID_ROW_NUMBER 함수 [ ROWID_ROW_NUMBER( rowid ) ]
 *         <BR> ROWID_ROW_NUMBER( rowid )
 *         <BR> ex) ROWID_ROW_NUMBER( rowid ) => 1
 *         <BR> ex) ROWID_ROW_NUMBER( AAADuwAAAAAAACAAAAAiAAB ) => 1
 *
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> ROWID_ROW_NUMBER(ROWID)
 * @param[out] aResultValue      연산 결과 (결과타입 INTEGER)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfRowIdRowNumber( stlUInt16        aInputArgumentCnt,
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

    /* ROWID_ROW_NUMBER( rowid ) 의 argument rowid가 NULL일 수도 있어,
     * argument에 대한 check는 하지 않는다. */
    
    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue,
                          DTL_TYPE_NATIVE_INTEGER,
                          (stlErrorStack *)aEnv );
    
    if( DTL_IS_NULL( sValue1 ) == STL_TRUE )
    {
        /*
         * ROWID_ROW_NUMBER( NULL ) 인 경우, 결과값은 0 이다.
         */ 
        DTF_INTEGER( sResultValue ) = 0;
    }
    else
    {    
        sRowIdType = DTF_ROWID( sValue1 );
        
        DTF_INTEGER( sResultValue )
            = (dtlIntegerType)( (((stlUInt16)(sRowIdType->mDigits[14])) << 8)  |
                                ((stlUInt16)(sRowIdType->mDigits[15])) );
    }
    
    sResultValue->mLength = DTL_NATIVE_INTEGER_SIZE;
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/** @} */
